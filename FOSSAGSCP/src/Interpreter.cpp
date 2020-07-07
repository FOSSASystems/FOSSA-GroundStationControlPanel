#include "Interpreter.h"

Interpreter::Interpreter(Settings& settings, Ui::MainWindow* ui) : m_settings(settings), m_ui(ui)
{

}

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
    std::string callsignStr = m_ui->SatelliteConfig_callsignLineEdit->text().toStdString();
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

    m_ui->handshookRadioButton->setChecked(true);
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

    char * callsign = (char*)m_ui->SatelliteConfig_callsignLineEdit->text().toStdString().c_str(); /// @todo fix stripping of const.

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

IGroundStationSerialMessage *Interpreter::Create_CMD_Deploy()
{
    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_DEPLOY, 0, nullptr);

    return msg;
}
