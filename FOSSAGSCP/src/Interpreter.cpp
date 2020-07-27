#include "Interpreter.h"

IGroundStationSerialMessage *Interpreter::SerialData_To_GroundStationSerialMessage(char *serialData, char serialDataLength)
{
    // first byte is the control byte.
    char controlByte = serialData[0];

    // second byte is the payload length.
    char lengthByte = serialData[1];

    // rest of the bytes is the payload.
    char* payload = new char[lengthByte];
    memcpy(payload, &(serialData[2]), lengthByte);

    GroundStationSerialMessage* message = new GroundStationSerialMessage(controlByte, lengthByte, payload);

    // delete the payload, it is copied in the datagram ctor.
    delete[] payload;

    return dynamic_cast<IGroundStationSerialMessage*>(message);
}

// this is the base class for all the Create_CMD_XXX methods.
IGroundStationSerialMessage* Interpreter::Create_GroundStationSerialMessage(char operationId, uint8_t functionId, uint8_t optDataLength, char* optData)
{
    char directionBit = FCPI_DIR_TO_GROUND_STATION; // all outgoing messages are to the ground station.

    // check whether this function id is for encryption.
    bool encrypt = functionId >= PRIVATE_OFFSET;

    //
    // Get information
    //
    std::string callsignStr = m_mainWindowUI->SatelliteConfig_callsignLineEdit->text().toStdString();
    std::string passwordStr = m_settings.GetPassword();

    const char* callsign = callsignStr.c_str();
    const char* password = passwordStr.c_str();
    const uint8_t* key = m_settings.GetKey();

    //
    // Check that the encryption is setup.
    //
    if (encrypt)
    {
        bool passwordSet = m_settings.IsPasswordSet();
        bool keySet = m_settings.IsKeySet();

        if ( !passwordSet || !keySet )
        {
            return nullptr;
        }
    }

    //
    // Get the entire frame's length.
    //
    uint8_t frameLength = 0;
    if (encrypt)
    {
        frameLength = FCP_Get_Frame_Length((char*)callsign, optDataLength, password);
    }
    else
    {
        frameLength = FCP_Get_Frame_Length((char*)callsign, optDataLength);
    }

    //
    // Create and encoded (at once) frame.
    //
    uint8_t* frame = new uint8_t[frameLength];
    if (encrypt)
    {
        FCP_Encode(frame, (char*)callsign, functionId, optDataLength, (uint8_t*)optData, key, password);
    }
    else
    {
        FCP_Encode(frame, (char*)callsign, functionId, optDataLength);
    }


    //
    // Send the frame wrapped in the gcs protocol.
    //
    char controlByte = directionBit | operationId;
    GroundStationSerialMessage* msg = new GroundStationSerialMessage(controlByte, (char)frameLength, (char*)frame); // the payload IS the FCP Frame above.

    delete[] frame;

    return dynamic_cast<IGroundStationSerialMessage*>(msg);
}

void Interpreter::SetSatelliteVersion(QString satVersion)
{
    m_satVersion = satVersion;
}

void Interpreter::Interpret_Received_Message(IGroundStationSerialMessage *inMsg)
{
    // route via the operation id
    int operationId = inMsg->GetOperationID();

    if (operationId == 0)
    {
        // handshake
        Interpret_Handshake(inMsg);
    }
    else if (operationId == 1)
    {
        // FCP frame.
        Interpret_FCP_Frame(inMsg);
    }
    else if (operationId == 2)
    {
        // configuration change.
        Interpret_Config_Change(inMsg);
    }
}

void Interpreter::Interpret_Handshake(IGroundStationSerialMessage *inMsg)
{
    GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);

    m_mainWindowUI->handshookRadioButton->setChecked(true);

    m_settings.SetHandshookValue(true);

    m_mainWindowUI->statusbar->showMessage("Ground station handshook successfully");

    emit ReceivedHandshake();
}

void Interpreter::Interpret_Config_Change(IGroundStationSerialMessage *inMsg)
{
    GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);
}

void Interpreter::Interpret_FCP_Frame(IGroundStationSerialMessage *inMsg)
{
    GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);

    // raw characters
    char* payloadData = msg->GetPayload();
    char payloadLength = msg->GetPayloadLengthByte();

    // converted to uint8_t
    uint8_t frameLength = (uint8_t)payloadLength;
    uint8_t* frame = new uint8_t[frameLength];
    for (int i = 0; i < frameLength; i++)
    {
        frame[i] = (uint8_t)(payloadData[i]);
    }

    char * callsign = (char*)m_mainWindowUI->SatelliteConfig_callsignLineEdit->text().toStdString().c_str(); /// @todo fix stripping of const.

    // get the function ID.
    int16_t functionId = FCP_Get_FunctionID(callsign, frame, frameLength);

    // get the optional data length.
    int16_t optionalDataLength = FCP_Get_OptData_Length(callsign, frame, frameLength, NULL, NULL);

    if (optionalDataLength >= 0)
    {
        // frame contains metadata
        uint8_t* optionalData = new uint8_t[optionalDataLength];
        FCP_Get_OptData(callsign, frame, frameLength, optionalData, NULL, NULL);

        ///////////////////////////////
        /// Interpret the commands. ///
        ///////////////////////////////

        // Public responses
        if (functionId == RESP_PONG)
        {
            // do nothing, already logged.
        }
        else if (functionId == RESP_REPEATED_MESSAGE)
        {

        }
        else if (functionId == RESP_REPEATED_MESSAGE_CUSTOM)
        {

        }
        else if (functionId == RESP_SYSTEM_INFO)
        {
            assert(optionalDataLength == 23);

            uint8_t mpptOutputVoltage = optionalData[0];
            int16_t mpptOutputCurrent = optionalData[1] | (optionalData[2] << 8);

            uint32_t unixTimestamp = optionalData[3];
            unixTimestamp = unixTimestamp | (optionalData[4] << 8);
            unixTimestamp = unixTimestamp | (optionalData[5] << 16);
            unixTimestamp = unixTimestamp | (optionalData[6] << 24);

            uint8_t powerConfiguration = optionalData[7];
            uint8_t transmissionsEnabled = powerConfiguration & 1;
            uint8_t lowPowerModeEnabled = (powerConfiguration >> 1) & 1;
            uint8_t currentlyActivePowerMode = (powerConfiguration >> 2) & 0b00000011;
            uint8_t mpptTemperatureSwitchEnabled = (powerConfiguration >> 5) & 1;
            uint8_t mpptKeepAliveEnabled = (powerConfiguration >> 6) & 1;

            uint16_t resetCounter = optionalData[8] | (optionalData[9] << 8);

            uint8_t solarPanel_XA_Voltage = optionalData[10];
            uint8_t solarPanel_XB_Voltage = optionalData[11];
            uint8_t solarPanel_ZA_Voltage = optionalData[12];
            uint8_t solarPanel_ZB_Voltage = optionalData[13];
            uint8_t solarPanel_Y_Voltage = optionalData[14];

            int16_t batteryTemperature = optionalData[15] | (optionalData[16] << 8);
            int16_t obcBoardTemperature = optionalData[17] | (optionalData[18] << 8);

            int32_t flashSystemInfoPageCRCErrorCounter = optionalData[19];
            flashSystemInfoPageCRCErrorCounter |= (optionalData[20] << 8);
            flashSystemInfoPageCRCErrorCounter |= (optionalData[21] << 16);
            flashSystemInfoPageCRCErrorCounter |= (optionalData[22] << 24);

        }
        else if (functionId == RESP_PACKET_INFO)
        {

        }
        else if (functionId == RESP_STATISTICS)
        {

        }
        else if (functionId == RESP_FULL_SYSTEM_INFO)
        {

        }
        else if (functionId == RESP_STORE_AND_FORWARD_ASSIGNED_SLOT)
        {

        }
        else if (functionId == RESP_FORWARDED_MESSAGE)
        {

        }
        else if (functionId == RESP_PUBLIC_PICTURE)
        {

        }
        else if (functionId == RESP_DEPLOYMENT_STATE)
        {

        }
        else if (functionId == RESP_RECORDED_SOLAR_CELLS)
        {

        }
        else if (functionId == RESP_CAMERA_STATE)
        {

        }
        else if (functionId == RESP_RECORDED_IMU)
        {

        }
        else if (functionId == RESP_MANUAL_ACS_RESULT)
        {

        }
        else if (functionId == RESP_GPS_LOG)
        {

        }
        else if (functionId == RESP_GPS_LOG_STATE)
        {

        }
        else if (functionId == RESP_FLASH_CONTENTS)
        {

        }
        else if (functionId == RESP_CAMERA_PICTURE)
        {

        }
        else if (functionId == RESP_CAMERA_PICTURE_LENGTH)
        {

        }
        else if (functionId == RESP_GPS_COMMAND_RESPONSE)
        {

        }
        else if (functionId == RESP_ACKNOWLEDGE)
        {

        }

        // Private
        // there are no privately downlinked messages.


        delete[] optionalData;
    }

    delete[] frame;
}



















IGroundStationSerialMessage *Interpreter::Create_CMD_Ping()
{
    return this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_PING, 0, nullptr);
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Get_Statistics(char flagsB, char flags)
{
    IGroundStationSerialMessage* msg = nullptr;

    if (m_satVersion == VERSION_1B)
    {
        char optData[1]; // copy the parameter into this locally scoped variable for safety.
        optData[0] = flagsB;
        msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_GET_STATISTICS, 1, optData);
    }
    else if (m_satVersion == VERSION_2)
    {
        char optData[1];
        optData[0] = flags;
        msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_GET_STATISTICS, 1, optData);
    }

    if (msg == nullptr)
    {
        throw "get statistics failed, unknown satellite version";
    }
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Deploy()
{
    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_DEPLOY, 0, nullptr);
    return msg;
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Wipe_EEPROM(char flags)
{
    IGroundStationSerialMessage* msg;

    if (m_satVersion == VERSION_1B)
    {
        this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_WIPE_EEPROM, 0, nullptr);
    }
    else if (m_satVersion == VERSION_2)
    {
        char optData[1];
        optData[0] = flags;

        this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_WIPE_EEPROM, 1, optData);
    }

    return msg;
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Set_Transmit_Enable(char transmitEnabled, char autoStatsEnabled, char fskMandatedEnabled)
{
    IGroundStationSerialMessage* msg;

    if (m_satVersion == VERSION_1B)
    {
        char optData[1];
        optData[0] = transmitEnabled;

        this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_SET_TRANSMIT_ENABLE, 1, optData);
    }
    else if (m_satVersion == VERSION_2)
    {
        char optData[3];
        optData[0] = transmitEnabled;
        optData[1] = autoStatsEnabled;
        optData[2] = fskMandatedEnabled;

        this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_WIPE_EEPROM, 3, optData);
    }

    return msg;
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Record_Solar_Cells(char numSamples, uint16_t samplingPeriod)
{
    if (m_satVersion != VERSION_1B)
    {
        throw "only compatable with fossasat-1b";
    }

    char optData[3];
    optData[0] = numSamples;
    optData[1] = samplingPeriod;
    optData[2] = samplingPeriod >> 8;

    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_RECORD_SOLAR_CELLS, 3, optData);
    return msg;
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Camera_Capture(char pictureSlot, char lightMode, char pictureSize, char brightness, char saturation, char specialFilter, char contrast)
{
    char optData[4];
    optData[0] = pictureSlot;
    optData[1] = (pictureSize << 4) | lightMode;
    optData[2] = (saturation << 4) | brightness;
    optData[3] = (contrast << 4) | specialFilter;

    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_CAMERA_CAPTURE, 4, optData);
    return msg;
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Get_Picture_Burst(char pictureSlot, uint16_t pictureId, char fullOrScandata)
{
    char optData[4];
    optData[0] = pictureSlot;
    optData[1] = pictureId;
    optData[2] = pictureId >> 8;
    optData[3] = fullOrScandata;

    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_GET_FULL_SYSTEM_INFO, 4, optData);
    return msg;
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Set_ADCS_Controller(char controllerId, float controllerMatrix[3][6])
{
    char optData[77];
    optData[0] = controllerId;

    int i = 1;
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 6; y++)
        {
            /// @todo check for endianess here.
            float value = controllerMatrix[x][y];
            char* valueAsArray = (char*)&value;

            optData[i] = valueAsArray[0];
            optData[i+1] = valueAsArray[1];
            optData[i+2] = valueAsArray[2];
            optData[i+3] = valueAsArray[3];

            i += 4;
        }
    }

    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_SET_ADCS_CONTROLLER, 77, optData);
    return msg;
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Set_ADCS_Ephemerides(uint16_t chunkId, std::vector<ephemerides_t> ephemeridesDataQueue)
{
    size_t optDataLen = ephemeridesDataQueue.size() * sizeof(ephemerides_t);
    optDataLen += sizeof(uint16_t);

    if (optDataLen > 192)
    {
        throw "too much data in ephemerides stack (max is 192 bytes)";
    }


    std::vector<char> optData;
    optData.push_back(chunkId);
    optData.push_back(chunkId >> 8);

    for (int i = 0; i < ephemeridesDataQueue.size(); i++)
    {
        ephemerides_t ephemeridesStruct = ephemeridesDataQueue[i];

        // convert the struct to a char* array
        char* ephemeridesData = (char*)&ephemeridesStruct;

        // for each byte, push it to the optDataVector.
        for (int k = 0; k < sizeof(ephemerides_t); k++)
        {
            optData.push_back(ephemeridesData[k]);
        }
    }

    // get the vector as char array.
    char* data = optData.data();

    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_SET_ADCS_EPHEMERIDES, optDataLen, data);
    return msg;
}

IGroundStationSerialMessage *Interpreter::Create_CMD_Get_Full_System_Info()
{
    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_GET_FULL_SYSTEM_INFO, 0, nullptr);
    return msg;
}

