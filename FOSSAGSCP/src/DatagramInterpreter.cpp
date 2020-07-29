#include "DatagramInterpreter.h"

DatagramInterpreter::DatagramInterpreter(Ui::MainWindow *mainWindowUI)
    : m_mainWindowUI(mainWindowUI)
{
}

IDatagram *DatagramInterpreter::SerialData_To_Datagram(QByteArray datagramData)
{
    // first byte is the control byte.
    char controlByte = datagramData[0];

    // second byte is the payload length.
    uint8_t lengthByte = (uint8_t)datagramData[1];

    if (lengthByte < (datagramData.size() - 4))
    {
        return nullptr;
    }

    // third and fourth byte is the radiolib status code.
    /// @todo handle status codes.
    int16_t radiolibStatusCode = datagramData[2] | (datagramData[3] << 8);

    // rest of the bytes is the frame.
    int frameLength = lengthByte - 2;
    uint8_t* frameData = nullptr;
    if (frameLength > 0)
    {
        frameData = new uint8_t[frameLength];
        memcpy(frameData, &(datagramData.constData()[4]), frameLength);
    }

    IFrame* frame = new FCPFrame(frameData, frameLength);
    IDatagram* datagram = new InboundDatagram(controlByte, lengthByte, radiolibStatusCode, frame);

    return datagram;
}

IDatagram* DatagramInterpreter::Create_Datagram(char operationId, uint8_t functionId, uint8_t optDataLength, char* optData)
{
    char directionBit = FCPI_DIR_TO_GROUND_STATION; // all outgoing messages are to the ground station.

    // check whether this function id is for encryption.
    bool encrypt = functionId >= PRIVATE_OFFSET;

    //
    // Get information
    //
    assert(m_mainWindowUI != nullptr);

    QString callsignQStr = m_mainWindowUI->SatelliteConfig_callsignLineEdit->text();
    std::string callsignStr = callsignQStr.toStdString();
    std::string passwordStr = Settings::GetPassword();

    const char* callsign = callsignStr.c_str();
    const char* password = passwordStr.c_str();
    const uint8_t* key = Settings::GetKey();

    //
    // Check that the encryption is setup.
    //
    if (encrypt)
    {
        bool passwordSet = Settings::IsPasswordSet();
        bool keySet = Settings::IsKeySet();

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
    uint8_t* frameData = new uint8_t[frameLength];
    if (encrypt)
    {
        FCP_Encode(frameData, (char*)callsign, functionId, optDataLength, (uint8_t*)optData, key, password);
    }
    else
    {
        FCP_Encode(frameData, (char*)callsign, functionId, optDataLength);
    }


    //
    // Send the frame wrapped in the gcs protocol.
    //
    char controlByte = directionBit | operationId;


    IFrame* frame = new FCPFrame(frameData, frameLength);
    IDatagram* datagram = new OutboundDatagram(controlByte, frameLength, frame); // the payload IS the FCP Frame above.

    return datagram;
}







IDatagram *DatagramInterpreter::Create_CMD_Ping()
{
    return this->Create_Datagram(FCPI_FRAME_OP, CMD_PING, 0, nullptr);
}

IDatagram *DatagramInterpreter::Create_CMD_Retransmit(uint32_t senderId, char *message)
{
    int messageLen = strlen(message) + 1;
    int optDataLen = messageLen+4;

    if (optDataLen > 32)
    {
        throw "transmit message too long (max 32)";
    }

    char* optData = new char[optDataLen];
    optData[0] = senderId;
    optData[1] = senderId >> 8;
    optData[2] = senderId >> 16;
    optData[4] = senderId >> 24;

    strcpy_s(&(optData[5]), messageLen, message);

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_RETRANSMIT, optDataLen, optData);

    delete[] optData;
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Retransmit_Custom(uint8_t bandwidth, uint8_t spreadingFactor, uint8_t codingRate, uint16_t preambleSymbolLength, uint8_t crcEnabled, int8_t outputPower, uint32_t senderId, char *message)
{
    int messageLen = strlen(message) + 1;
    int optDataLen = 11 + messageLen;

    if (optDataLen > 43)
    {
        throw "transmit message too long (max 43)";
    }

    char* optData = new char[optDataLen];
    optData[0] = bandwidth;
    optData[1] = spreadingFactor;
    optData[2] = codingRate;
    optData[3] = preambleSymbolLength;
    optData[4] = preambleSymbolLength >> 8;
    optData[5] = crcEnabled;
    optData[6] = outputPower;
    optData[7] = outputPower >> 8;
    optData[8] = outputPower >> 16;
    optData[9] = outputPower >> 24;

    strcpy_s(&(optData[10]), messageLen, message);

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_RETRANSMIT_CUSTOM, optDataLen, optData);

    delete[] optData;

    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Transmit_System_Info()
{
    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_TRANSMIT_SYSTEM_INFO, 0, nullptr);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Get_Packet_Info()
{
    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_GET_PACKET_INFO, 0, nullptr);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Get_Statistics(char flagsB, char flags)
{
    IDatagram* msg = nullptr;

    if (Settings::GetSatVersion() == VERSION_1B)
    {
        char optData[1]; // copy the parameter into this locally scoped variable for safety.
        optData[0] = flagsB;
        msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_GET_STATISTICS, 1, optData);
    }
    else if (Settings::GetSatVersion() == VERSION_2)
    {
        char optData[1];
        optData[0] = flags;
        msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_GET_STATISTICS, 1, optData);
    }

    if (msg == nullptr)
    {
        throw "get statistics failed, unknown satellite version";
    }
}

IDatagram *DatagramInterpreter::Create_CMD_Deploy()
{
    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_DEPLOY, 0, nullptr);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Restart()
{
    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_RESTART, 0, nullptr);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Wipe_EEPROM(char flags)
{
    IDatagram* msg;

    if (Settings::GetSatVersion() == VERSION_1B)
    {
        this->Create_Datagram(FCPI_FRAME_OP, CMD_WIPE_EEPROM, 0, nullptr);
    }
    else if (Settings::GetSatVersion() == VERSION_2)
    {
        char optData[1];
        optData[0] = flags;

        this->Create_Datagram(FCPI_FRAME_OP, CMD_WIPE_EEPROM, 1, optData);
    }

    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_Transmit_Enable(char transmitEnabled, char autoStatsEnabled, char fskMandatedEnabled)
{
    IDatagram* msg;

    if (Settings::GetSatVersion() == VERSION_1B)
    {
        char optData[1];
        optData[0] = transmitEnabled;

        this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_TRANSMIT_ENABLE, 1, optData);
    }
    else if (Settings::GetSatVersion() == VERSION_2)
    {
        char optData[3];
        optData[0] = transmitEnabled;
        optData[1] = autoStatsEnabled;
        optData[2] = fskMandatedEnabled;

        this->Create_Datagram(FCPI_FRAME_OP, CMD_WIPE_EEPROM, 3, optData);
    }

    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_Callsign(char *callsign)
{
    int callsignLen = strlen(callsign) + 1;

    if (callsignLen > 32)
    {
       throw "callsign cannot be more than 32 characters";
    }

    char* optData = new char[callsignLen];

    strcpy_s(&(optData[10]), callsignLen, callsign);

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_CALLSIGN, 0, nullptr);

    delete[] optData;
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_SF_Mode(uint8_t spreadingFactor)
{
    char optData[1];
    optData[0] = spreadingFactor;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_SF_MODE, 1, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_MPPT_Mode(uint8_t temperatureSwitchEnabled, uint8_t keepAliveEnabled)
{
    char optData[2];
    optData[0] = temperatureSwitchEnabled;
    optData[1] = keepAliveEnabled;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_MPPT_MODE, 2, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_Low_Power_Mode_Enable(uint8_t lowPowerModeEnabled)
{
    char optData[1];
    optData[0] = lowPowerModeEnabled;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_LOW_POWER_ENABLE, 1, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_Receive_Windows(uint8_t fskReceiveWindowLength, uint8_t loraReceiveWindowLength)
{
    char optData[2];
    optData[0] = fskReceiveWindowLength;
    optData[1] = loraReceiveWindowLength;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_RECEIVE_WINDOWS, 2, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Record_Solar_Cells(char numSamples, uint16_t samplingPeriod)
{
    if (Settings::GetSatVersion() != VERSION_1B)
    {
        throw "only compatable with fossasat-1b";
    }

    char optData[3];
    optData[0] = numSamples;
    optData[1] = samplingPeriod;
    optData[2] = samplingPeriod >> 8;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_RECORD_SOLAR_CELLS, 3, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Camera_Capture(char pictureSlot, char lightMode, char pictureSize, char brightness, char saturation, char specialFilter, char contrast)
{
    char optData[4];
    optData[0] = pictureSlot;
    optData[1] = (pictureSize << 4) | lightMode;
    optData[2] = (saturation << 4) | brightness;
    optData[3] = (contrast << 4) | specialFilter;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_CAMERA_CAPTURE, 4, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_Power_Limits(int16_t deploymentVoltageLimit, int16_t heaterVoltageLimit, int16_t cwBeepVoltageLimit, int16_t lowPowerVoltageLimit, float heaterTemperatureLimit, float mpptSwitchtemperatureLimit, uint8_t heaterDutyCycle)
{
    char optData[17];
    optData[0] = deploymentVoltageLimit;
    optData[1] = deploymentVoltageLimit >> 8;
    optData[2] = heaterVoltageLimit;
    optData[3] = heaterVoltageLimit >> 8;
    optData[4] = cwBeepVoltageLimit;
    optData[5] = cwBeepVoltageLimit >> 8;
    optData[6] = lowPowerVoltageLimit;
    optData[7] = lowPowerVoltageLimit >> 8;
    char* heaterTemperatureArray = (char*)(&heaterTemperatureLimit);
    optData[8] = heaterTemperatureArray[0];
    optData[9] = heaterTemperatureArray[1];
    optData[10] = heaterTemperatureArray[2];
    optData[11] = heaterTemperatureArray[3];
    char* mpptSwitchTemperatureArray = (char*)(&mpptSwitchtemperatureLimit);
    optData[12] = mpptSwitchTemperatureArray[0];
    optData[13] = mpptSwitchTemperatureArray[1];
    optData[14] = mpptSwitchTemperatureArray[2];
    optData[15] = mpptSwitchTemperatureArray[3];
    optData[16] = heaterDutyCycle;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_POWER_LIMITS, 17, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_RTC(uint8_t year, uint8_t month, uint8_t day, uint8_t dayOfWeek, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    char optData[7];
    optData[0] = year;
    optData[1] = month;
    optData[2] = day;
    optData[3] = dayOfWeek;
    optData[4] = hours;
    optData[5] = minutes;
    optData[6] = seconds;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_RTC, 7, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Record_IMU(uint16_t sampleNumber, uint16_t samplingPeriod, uint8_t flags)
{
    char optData[5];
    optData[0] = sampleNumber;
    optData[1] = sampleNumber >> 8;
    optData[2] = samplingPeriod;
    optData[3] = samplingPeriod >> 8;
    optData[4] = flags;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_RECORD_IMU, 5, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Run_Manual_ACS(int8_t xHighBridgeMag, int8_t xLowBridgeMag, int8_t yHighBridgeMag, int8_t yLowBrightMag, int8_t zHBridgeHighMag, int8_t zBridgeLowMag, uint32_t xPulseLength, uint32_t yPulseLength, uint32_t zPulseLength, uint32_t maneuverDuration, uint8_t hbridgefaultFlags)
{
    char optData[23];
    optData[0] = xHighBridgeMag;
    optData[1] = xLowBridgeMag;
    optData[2] = yHighBridgeMag;
    optData[3] = yLowBrightMag;
    optData[4] = zHBridgeHighMag;
    optData[5] = zBridgeLowMag;
    optData[6] = xPulseLength;
    optData[7] = xPulseLength >> 8;
    optData[8] = xPulseLength >> 16;
    optData[9] = xPulseLength >> 24;
    optData[10] = yPulseLength;
    optData[11] = yPulseLength >> 8;
    optData[12] = yPulseLength >> 16;
    optData[13] = yPulseLength >> 24;
    optData[14] = zPulseLength;
    optData[15] = zPulseLength >> 8;
    optData[16] = zPulseLength >> 16;
    optData[17] = zPulseLength >> 24;
    optData[18] = maneuverDuration;
    optData[19] = maneuverDuration >> 8;
    optData[20] = maneuverDuration >> 16;
    optData[21] = maneuverDuration >> 24;
    optData[22] = hbridgefaultFlags;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_RUN_MANUAL_ACS, 23, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Log_GPS(uint32_t gpsLoggingDuration, uint32_t gpsLoggingStartOffset)
{
    char optData[8];
    optData[0] = gpsLoggingDuration;
    optData[1] = gpsLoggingDuration >> 8;
    optData[2] = gpsLoggingDuration >> 16;
    optData[3] = gpsLoggingDuration >> 24;
    optData[4] = gpsLoggingStartOffset;
    optData[5] = gpsLoggingStartOffset >> 8;
    optData[6] = gpsLoggingStartOffset >> 16;
    optData[7] = gpsLoggingStartOffset >> 24;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_LOG_GPS, 8, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Get_GPS_Log(uint8_t newestEntriesFirst, uint16_t gpsLogOffsetAsNMEASeq, uint16_t NMEASentencesToDownlink)
{
    char optData[5];
    optData[0] = newestEntriesFirst;
    optData[1] = gpsLogOffsetAsNMEASeq;
    optData[2] = gpsLogOffsetAsNMEASeq >> 8;
    optData[3] = NMEASentencesToDownlink;
    optData[4] = NMEASentencesToDownlink >> 8;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_GET_GPS_LOG, 5, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Get_Flash_Contents(uint32_t flashAddress, uint8_t numBytes)
{
    char optData[5];
    optData[0] = flashAddress;
    optData[1] = flashAddress >> 8;
    optData[2] = flashAddress >> 16;
    optData[3] = flashAddress >> 24;
    optData[4] = numBytes;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_GET_FLASH_CONTENTS, 5, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Get_Picture_Length(uint8_t pictureSlot)
{
    char optData[1];
    optData[0] = pictureSlot;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_GET_PICTURE_LENGTH, 1, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Get_Picture_Burst(char pictureSlot, uint16_t pictureId, char fullOrScandata)
{
    char optData[4];
    optData[0] = pictureSlot;
    optData[1] = pictureId;
    optData[2] = pictureId >> 8;
    optData[3] = fullOrScandata;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_GET_FULL_SYSTEM_INFO, 4, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Route(char *fcpFrame)
{
    int messageLen = strlen(fcpFrame) + 1;

    char* optData = new char[messageLen];
    strcpy_s(optData, messageLen, fcpFrame);

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_ROUTE, messageLen, optData);

    delete[] optData;

    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_Flash_Contents(uint32_t flashAddress, char *data)
{
    int dataLen = strlen(data); // ignore the null terminator + 1;
    int messageLen = dataLen + 4;

    char* optData = new char[messageLen];
    optData[0] = flashAddress;
    optData[1] = flashAddress >> 8;
    optData[2] = flashAddress >> 16;

    memcpy_s(&(optData[3]), dataLen, data, dataLen); // copy to opt data without the null terminator.

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_FLASH_CONTENTS, messageLen, optData);

    delete[] optData;

    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_TLE(char *tle)
{
    int optDataLen = strlen(tle);
    char* optData = new char[optDataLen];

    memcpy_s(optData, optDataLen, tle, optDataLen); // no line endings.

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_TLE, optDataLen, optData);

    delete[] optData;
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Run_GPS_Command(char *skyTraqBinaryProtocolMessage)
{
    int optDataLen = strlen(skyTraqBinaryProtocolMessage); // without terminator.

    char* optData = new char[optDataLen];
    memcpy_s(optData, optDataLen, skyTraqBinaryProtocolMessage, optDataLen);

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_RUN_GPS_COMMAND, optDataLen, optData);

    delete optData;

    return msg;
}


IDatagram *DatagramInterpreter::Create_CMD_Set_Sleep_Intervals(std::vector<sleep_interval_t> sleepIntervals)
{
    int numIntervals = sleepIntervals.size();
    int optDataLen = numIntervals * sizeof(sleep_interval_t);


    char* optData = new char[optDataLen];

    int optDataIndex = 0;
    for (int i = 0; i < numIntervals; i++)
    {
        sleep_interval_t interval = sleepIntervals[i];

        memcpy_s(&(optData[optDataIndex]), sizeof(sleep_interval_t), &interval, sizeof(sleep_interval_t));

        optDataIndex += sizeof(sleep_interval_t);
    }

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_SLEEP_INTERVALS, optDataLen, optData);

    delete[] optData;

    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Abort()
{
    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_ABORT, 0, nullptr);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Maneuver(uint8_t controlFlags, uint32_t maneuverLength)
{
    char optData[5];
    optData[0] = controlFlags;
    optData[1] = maneuverLength;
    optData[2] = maneuverLength >> 8;
    optData[3] = maneuverLength >> 16;
    optData[4] = maneuverLength >> 24;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_MANEUVER, 5, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_ADCS_Parameters(float maximumPulseIntensity, float maximumPulseLength, float detumblingAngularVelocity, float minimumIntertialMoment, float pulseAmplitude, float calculationTolerance, float activeControlEulerAngleChangeTolerance, float activeControlAngularVelocityChangeTolerance, float eclipseThreshold, float rotationMatrixWeightRatio, float rotationVerificationTriggerLevel, float kalmanFilterDisturbanceCovariance, float kalmanFilterNoiseCovariance, uint32_t adcsUpdateTimeStepPeriod, uint32_t hbridgeTimerUpdatePeriod, int8_t hbridgeValueHighOutput, int8_t hbridgeValueLowOutput, uint8_t numControllers)
{
    char optData[63];

    char* maximumPulseIntensityArray = (char*)(&maximumPulseIntensity);
    char* maximumPulseLengthArray = (char*)(&maximumPulseLength);
    char* detumblingAngularVelocityArray = (char*)(&detumblingAngularVelocity);
    char* minimumIntertialMomentArray = (char*)(&minimumIntertialMoment);
    char* pulseAmplitudeArray = (char*)(&pulseAmplitude);
    char* calculationToleranceArray = (char*)(&calculationTolerance);
    char* activeControlEulerAngleChangeToleranceArray = (char*)(&activeControlEulerAngleChangeTolerance);
    char* activeControlAngularVelocityChangeToleranceArray = (char*)(&activeControlAngularVelocityChangeTolerance);
    char* eclipseThresholdArray = (char*)(&eclipseThreshold);
    char* rotationMatrixWeightRatioArray = (char*)(&rotationMatrixWeightRatio);
    char* rotationVerificationTriggerLevelArray = (char*)(&rotationVerificationTriggerLevel);
    char* kalmanFilterDisturbanceCovarianceArray = (char*)(&kalmanFilterDisturbanceCovariance);
    char* kalmanFilterNoiseCovarianceArray = (char*)(&kalmanFilterNoiseCovariance);

    optData[0] = maximumPulseIntensityArray[0];
    optData[1] = maximumPulseIntensityArray[1];
    optData[2] = maximumPulseIntensityArray[2];
    optData[3] = maximumPulseIntensityArray[3];

    optData[4] = maximumPulseLengthArray[0];
    optData[5] = maximumPulseLengthArray[1];
    optData[6] = maximumPulseLengthArray[2];
    optData[7] = maximumPulseLengthArray[3];

    optData[8] = detumblingAngularVelocityArray[0];
    optData[9] = detumblingAngularVelocityArray[1];
    optData[10] = detumblingAngularVelocityArray[2];
    optData[11] = detumblingAngularVelocityArray[3];

    optData[12] = minimumIntertialMomentArray[0];
    optData[13] = minimumIntertialMomentArray[1];
    optData[14] = minimumIntertialMomentArray[2];
    optData[15] = minimumIntertialMomentArray[3];

    optData[16] = pulseAmplitudeArray[0];
    optData[17] = pulseAmplitudeArray[1];
    optData[18] = pulseAmplitudeArray[2];
    optData[19] = pulseAmplitudeArray[3];

    optData[20] = calculationToleranceArray[0];
    optData[21] = calculationToleranceArray[1];
    optData[22] = calculationToleranceArray[2];
    optData[23] = calculationToleranceArray[3];

    optData[24] = activeControlEulerAngleChangeToleranceArray[0];
    optData[25] = activeControlEulerAngleChangeToleranceArray[1];
    optData[26] = activeControlEulerAngleChangeToleranceArray[2];
    optData[27] = activeControlEulerAngleChangeToleranceArray[3];

    optData[28] = activeControlAngularVelocityChangeToleranceArray[0];
    optData[29] = activeControlAngularVelocityChangeToleranceArray[1];
    optData[30] = activeControlAngularVelocityChangeToleranceArray[2];
    optData[31] = activeControlAngularVelocityChangeToleranceArray[3];

    optData[32] = eclipseThresholdArray[0];
    optData[33] = eclipseThresholdArray[1];
    optData[34] = eclipseThresholdArray[2];
    optData[35] = eclipseThresholdArray[3];

    optData[36] = rotationMatrixWeightRatioArray[0];
    optData[37] = rotationMatrixWeightRatioArray[1];
    optData[38] = rotationMatrixWeightRatioArray[2];
    optData[39] = rotationMatrixWeightRatioArray[3];

    optData[40] = rotationVerificationTriggerLevelArray[0];
    optData[41] = rotationVerificationTriggerLevelArray[1];
    optData[42] = rotationVerificationTriggerLevelArray[2];
    optData[43] = rotationVerificationTriggerLevelArray[3];

    optData[44] = kalmanFilterDisturbanceCovarianceArray[0];
    optData[45] = kalmanFilterDisturbanceCovarianceArray[1];
    optData[46] = kalmanFilterDisturbanceCovarianceArray[2];
    optData[47] = kalmanFilterDisturbanceCovarianceArray[3];

    optData[48] = kalmanFilterNoiseCovarianceArray[0];
    optData[49] = kalmanFilterNoiseCovarianceArray[1];
    optData[50] = kalmanFilterNoiseCovarianceArray[2];
    optData[51] = kalmanFilterNoiseCovarianceArray[3];

    optData[52] = adcsUpdateTimeStepPeriod;
    optData[53] = adcsUpdateTimeStepPeriod >> 8;
    optData[54] = adcsUpdateTimeStepPeriod >> 16;
    optData[55] = adcsUpdateTimeStepPeriod >> 24;

    optData[56] = hbridgeTimerUpdatePeriod;
    optData[57] = hbridgeTimerUpdatePeriod >> 8;
    optData[58] = hbridgeTimerUpdatePeriod >> 16;
    optData[59] = hbridgeTimerUpdatePeriod >> 24;

    optData[60] = hbridgeValueHighOutput;
    optData[61] = hbridgeValueLowOutput;
    optData[62] = numControllers;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_ADCS_PARAMETERS, 63, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Erase_Flash(uint32_t sectorAddress)
{
    char optData[4];
    optData[0] = sectorAddress;
    optData[1] = sectorAddress >> 8;
    optData[2] = sectorAddress >> 16;
    optData[3] = sectorAddress >> 24;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_ERASE_FLASH, 4, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_ADCS_Controller(char controllerId, float controllerMatrix[3][6])
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

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_ADCS_CONTROLLER, 77, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_ADCS_Ephemerides(uint16_t chunkId, std::vector<ephemerides_t> ephemeridesDataQueue)
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

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_ADCS_EPHEMERIDES, optDataLen, data);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Detumble(uint8_t flags, uint32_t length)
{
    char optData[5];
    optData[0] = flags;
    optData[1] = length;
    optData[2] = length >> 8;
    optData[3] = length >> 16;
    optData[4] = length >> 24;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_DETUMBLE, 5, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_IMU_Offset(uint8_t xAxisGyroOffset, uint8_t yAxisGyroOffset, uint8_t zAxisGyroOffset, uint8_t xAxisAcceleOffset, uint8_t yAxisAcceleOffset, uint8_t zAxisAcceleOffset, uint8_t xAxisMagneticOffset, uint8_t yAxisMagneticOffset, uint8_t zAxisMagneticOffset)
{
    char* xAxisGyroOffsetArray = (char*)(&xAxisGyroOffset);
    char* yAxisGyroOffsetArray = (char*)(&yAxisGyroOffset);
    char* zAxisGyroOffsetArray = (char*)(&zAxisGyroOffset);
    char* xAxisAcceleOffsetArray = (char*)(&xAxisAcceleOffset);
    char* yAxisAcceleOffsetArray = (char*)(&yAxisAcceleOffset);
    char* zAxisAcceleOffsetArray = (char*)(&zAxisAcceleOffset);
    char* xAxisMagneticOffsetArray = (char*)(&xAxisMagneticOffset);
    char* yAxisMagneticOffsetArray = (char*)(&yAxisMagneticOffset);
    char* zAxisMagneticOffsetArray = (char*)(&zAxisMagneticOffset);


    char optData[36];
    optData[0] = xAxisGyroOffsetArray[0];
    optData[1] = xAxisGyroOffsetArray[1];
    optData[2] = xAxisGyroOffsetArray[2];
    optData[3] = xAxisGyroOffsetArray[3];

    optData[4] = yAxisGyroOffsetArray[0];
    optData[5] = yAxisGyroOffsetArray[1];
    optData[6] = yAxisGyroOffsetArray[2];
    optData[7] = yAxisGyroOffsetArray[3];

    optData[8] = zAxisGyroOffsetArray[0];
    optData[9] = zAxisGyroOffsetArray[1];
    optData[10] = zAxisGyroOffsetArray[2];
    optData[11] = zAxisGyroOffsetArray[3];

    optData[12] = xAxisAcceleOffsetArray[0];
    optData[13] = xAxisAcceleOffsetArray[1];
    optData[14] = xAxisAcceleOffsetArray[2];
    optData[15] = xAxisAcceleOffsetArray[3];

    optData[16] = yAxisAcceleOffsetArray[0];
    optData[17] = yAxisAcceleOffsetArray[1];
    optData[18] = yAxisAcceleOffsetArray[2];
    optData[19] = yAxisAcceleOffsetArray[3];

    optData[20] = zAxisAcceleOffsetArray[0];
    optData[21] = zAxisAcceleOffsetArray[1];
    optData[22] = zAxisAcceleOffsetArray[2];
    optData[23] = zAxisAcceleOffsetArray[3];

    optData[24] = xAxisMagneticOffsetArray[0];
    optData[25] = xAxisMagneticOffsetArray[1];
    optData[26] = xAxisMagneticOffsetArray[2];
    optData[27] = xAxisMagneticOffsetArray[3];

    optData[28] = yAxisMagneticOffsetArray[0];
    optData[29] = yAxisMagneticOffsetArray[1];
    optData[30] = yAxisMagneticOffsetArray[2];
    optData[31] = yAxisMagneticOffsetArray[3];

    optData[32] = zAxisMagneticOffsetArray[0];
    optData[33] = zAxisMagneticOffsetArray[1];
    optData[34] = zAxisMagneticOffsetArray[2];
    optData[35] = zAxisMagneticOffsetArray[3];

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_IMU_OFFSET, 36, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Set_IMU_Calibration(float transformationMatrix[9], float biasVector[3])
{
    char* matrixA = (char*)(&transformationMatrix[0]);
    char* matrixB = (char*)(&transformationMatrix[1]);
    char* matrixC = (char*)(&transformationMatrix[2]);
    char* matrixD = (char*)(&transformationMatrix[3]);
    char* matrixE = (char*)(&transformationMatrix[4]);
    char* matrixF = (char*)(&transformationMatrix[5]);
    char* matrixG = (char*)(&transformationMatrix[6]);
    char* matrixH = (char*)(&transformationMatrix[7]);
    char* matrixI = (char*)(&transformationMatrix[8]);

    char* vectorA = (char*)(&biasVector[0]);
    char* vectorB = (char*)(&biasVector[1]);
    char* vectorC = (char*)(&biasVector[2]);

    char optData[47];
    optData[0] = matrixA[0];
    optData[1] = matrixA[1];
    optData[2] = matrixA[2];
    optData[3] = matrixA[3];

    optData[4] = matrixB[0];
    optData[5] = matrixB[1];
    optData[6] = matrixB[2];
    optData[7] = matrixB[3];

    optData[8] = matrixC[0];
    optData[9] = matrixC[1];
    optData[10] = matrixC[2];
    optData[11] = matrixC[3];

    optData[12] = matrixD[0];
    optData[13] = matrixD[1];
    optData[14] = matrixD[2];
    optData[15] = matrixD[3];

    optData[16] = matrixE[0];
    optData[17] = matrixE[1];
    optData[18] = matrixE[2];
    optData[19] = matrixE[3];

    optData[20] = matrixF[0];
    optData[21] = matrixF[1];
    optData[22] = matrixF[2];
    optData[23] = matrixF[3];

    optData[24] = matrixG[0];
    optData[25] = matrixG[1];
    optData[26] = matrixG[2];
    optData[27] = matrixG[3];

    optData[28] = matrixH[0];
    optData[29] = matrixH[1];
    optData[30] = matrixH[2];
    optData[31] = matrixH[3];

    optData[32] = matrixI[0];
    optData[33] = matrixI[1];
    optData[34] = matrixI[2];
    optData[35] = matrixI[3];



    optData[36] = vectorA[0];
    optData[37] = vectorA[1];
    optData[38] = vectorA[2];
    optData[39] = vectorA[3];

    optData[40] = vectorB[0];
    optData[41] = vectorB[1];
    optData[42] = vectorB[2];
    optData[43] = vectorB[3];

    optData[44] = vectorC[0];
    optData[45] = vectorC[1];
    optData[46] = vectorC[2];
    optData[47] = vectorC[3];

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_SET_IMU_CALIBRATION, 47, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Get_Full_System_Info()
{
    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_GET_FULL_SYSTEM_INFO, 0, nullptr);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Store_And_Forward_Add(uint32_t messageId, char *message)
{
    int messageLen = strlen(message) + 1;
    int optDataLen = 4 + messageLen;

    if (optDataLen > 32)
    {
        throw "store and forward message too long (max 32)";
    }

    char* optData = new char[optDataLen];
    optData[0] = messageId;
    optData[1] = messageId >> 8;
    optData[2] = messageId >> 16;
    optData[3] = messageId >> 24;

    strcpy_s(&(optData[4]), messageLen, message);

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_STORE_AND_FORWARD_ADD, optDataLen, optData);

    delete[] optData;

    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Store_And_Forward_Request(uint32_t messageId)
{
    char optData[4];
    optData[0] = messageId;
    optData[1] = messageId >> 8;
    optData[2] = messageId >> 16;
    optData[3] = messageId >> 24;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_STORE_AND_FORWARD_REQUEST, 4, optData);
    return msg;
}

IDatagram *DatagramInterpreter::Create_CMD_Request_Public_Picture(uint8_t pictureSlot, uint16_t picturePacketId)
{
    if (pictureSlot < 80 || pictureSlot > 100)
    {
        throw "only picture slots 80 <= id <= 100 are allowed";
    }

    char optData[3];
    optData[0] = pictureSlot;
    optData[1] = picturePacketId;
    optData[2] = picturePacketId >> 8;

    IDatagram* msg = this->Create_Datagram(FCPI_FRAME_OP, CMD_REQUEST_PUBLIC_PICTURE, 3, optData);
    return msg;
}











