#include "DatagramProcessor.h"

DatagramProcessor::DatagramProcessor(Ui::MainWindow *mainWindowUI,  Ui::systeminformationpane* systemInfoUI, MessageLogFrame* messageLogFrame)
    : m_mainWindowUI(mainWindowUI), m_messageLog(messageLogFrame), m_systemInfoUI(systemInfoUI)
{

}


void DatagramProcessor::ProcessDatagram(IDatagram* datagram)
{
    uint8_t operationId = datagram->GetOperationId();

    if (operationId == 0)
    {
        ProcessHandshake(datagram);
    }
    else if (operationId == 1)
    {
        ProcessFrame(datagram);
    }
    else if (operationId == 2)
    {
        ProcessGroundStationConfigurationChange(datagram);
    }
    else if (operationId == 3)
    {
        ProcessGroundstationCarrierFrequencyChange(datagram);
    }
}



void DatagramProcessor::ProcessHandshake(IDatagram *datagram)
{
    m_mainWindowUI->handshookRadioButton->setChecked(true);

    Settings::SetHandshookValue(true);

    m_mainWindowUI->statusbar->showMessage("Ground station handshook successfully");

    emit StartDopplerCorrector();
}

void DatagramProcessor::ProcessGroundStationConfigurationChange(IDatagram *datagram)
{

}

void DatagramProcessor::ProcessGroundstationCarrierFrequencyChange(IDatagram *datagram)
{

}


void DatagramProcessor::ProcessFrame(IDatagram *datagram)
{
    // get the callsign
    QString callsignQStr = m_mainWindowUI->SatelliteConfig_callsignLineEdit->text();
    std::string callsignStr = callsignQStr.toStdString();
    char * callsignCStr = (char*)callsignStr.c_str();
    uint32_t callsignLen = callsignStr.size();


    // Get the frame from the datagram.
    const IFrame* frame = datagram->GetFrame();

    // Get the length of the frame's data.
    uint32_t frameLength = frame->GetLength();

    // Get the data of the frame.
    uint8_t* frameData = frame->GetData();

    // get the function ID.
    int16_t functionId = FCP_Get_FunctionID(callsignCStr, frameData, frameLength);

    // get the optional data length.
    int16_t optionalDataLength = FCP_Get_OptData_Length(callsignCStr, frameData, frameLength);

    // for (int i = 0; i < payloadLength; i++)
     //{
        // qInfo() << functionId;
         //qInfo() << optionalDataLength;
     //}
    if (optionalDataLength >= 0)
    {
        // frame contains metadata
        uint8_t* optionalData = new uint8_t[optionalDataLength];
        FCP_Get_OptData(callsignCStr, frameData, frameLength, optionalData, NULL, NULL);

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
            // do nothing, already alogged.
        }
        else if (functionId == RESP_REPEATED_MESSAGE_CUSTOM)
        {
            // do nothing
        }
        else if (functionId == RESP_SYSTEM_INFO)
        {
            // message the messageframe
            m_messageLog->RawWriteToLog("[System information frame]");

            if (Settings::GetSatVersion() == VERSION_1B)
            {
                uint8_t batteryVoltage = optionalData[0];
                int16_t batteryChargingCurrent = optionalData[1] | (optionalData[2] << 8);
                uint8_t batteryChargingVoltage = optionalData[3];

                uint32_t timeSinceLastReset = optionalData[4];
                timeSinceLastReset |= (optionalData[5] << 8);
                timeSinceLastReset |= (optionalData[6] << 16);
                timeSinceLastReset |= (optionalData[7] << 24);

                uint8_t powerConfiguration = optionalData[8];
                bool lowPowerModeActive = powerConfiguration & 0b00000001;
                bool lowPowerModeEnabled = (powerConfiguration & 0b00000010) >> 1;
                bool mpptTemperatureSwitchEnabled = (powerConfiguration & 0b00000100) >> 2;
                bool mpptKeepAliveEnabled = (powerConfiguration & 0b00001000) >> 3;
                bool transmissionsEnabled = (powerConfiguration & 0b00010000) >> 4;

                uint16_t resetCounter = optionalData[9] | (optionalData[10] << 8);

                uint8_t solarCellAVoltage = optionalData[11];
                uint8_t solarCellBVoltage = optionalData[12];
                uint8_t solarCellCVoltage = optionalData[13];

                int16_t batteryTemperature = optionalData[14] | (optionalData[15] << 8);
                int16_t obcBoardTemperature = optionalData[16] | (optionalData[17] << 8);
                int8_t mcuTemperature = optionalData[18];


                m_systemInfoUI->SystemInformation_1B_BatteryVoltage_SpinBox->setValue(batteryVoltage * 20);
                m_systemInfoUI->SystemInformation_1B_BatteryChargingCurrent_SpinBox->setValue(batteryChargingCurrent * 10);
                m_systemInfoUI->SystemInformation_1B_BatteryChargingVoltage_SpinBox->setValue(batteryChargingVoltage * 20);

                m_systemInfoUI->SystemInformation_1B_TimeSinceLastReset_SpinBox->setValue(timeSinceLastReset);

                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerMode_Enabled_RadioButton->setChecked(lowPowerModeEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerMode_Disabled_RadioButton->setChecked(!lowPowerModeEnabled);

                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Enabled_RadioButton->setChecked(mpptTemperatureSwitchEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Disabled_RadioButton->setChecked(!mpptTemperatureSwitchEnabled);

                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Enabled_RadioButton->setChecked(mpptKeepAliveEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Disabled_RadioButton->setChecked(!mpptKeepAliveEnabled);

                m_systemInfoUI->SystemInformation_PowerConfiguration_TransmissionsEnabled_Enabled_RadioButton->setChecked(transmissionsEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_TransmissionsEnabled_Disabled_RadioButton->setChecked(!transmissionsEnabled);

                m_systemInfoUI->SystemInformation_ResetCounter_SpinBox->setValue(resetCounter);

                m_systemInfoUI->SystemInformation_RecordSolarCells_SolarCellAVoltage_SpinBox->setValue(solarCellAVoltage * 20);
                m_systemInfoUI->SystemInformation_RecordSolarCells_SolarCellBVoltage_SpinBox->setValue(solarCellBVoltage * 20);
                m_systemInfoUI->SystemInformation_RecordSolarCells_SolarCellCVoltage_SpinBox->setValue(solarCellCVoltage * 20);

                m_systemInfoUI->SystemInformation_TemperatureInformation_BatteryTemperature_DoubleSpinBox->setValue(batteryTemperature * 0.01);
                m_systemInfoUI->SystemInformation_TemperatureInformation_OBCBoardTemperature_DoubleSpinBox->setValue(obcBoardTemperature * 0.01);
                m_systemInfoUI->SystemInformation_TemperatureInformation_MCUTemperature_DoubleSpinBox->setValue(mcuTemperature);
            }
            else if (Settings::GetSatVersion() == VERSION_2)
            {
                uint8_t mpptOutputVoltage = optionalData[0];
                int16_t mpptOutputCurrent = optionalData[1] | (optionalData[2] << 8);

                uint32_t unixTimestamp = optionalData[3];
                unixTimestamp = unixTimestamp | (optionalData[4] << 8);
                unixTimestamp = unixTimestamp | (optionalData[5] << 16);
                unixTimestamp = unixTimestamp | (optionalData[6] << 24);

                uint8_t powerConfiguration = optionalData[7];
                uint8_t transmissionsEnabled = powerConfiguration & 0b00000001; // bit 0
                uint8_t lowPowerModeEnabled = (powerConfiguration & 0b00000010) >> 1; // bit 1

                uint8_t currentlyActivePowerModeLSB = (powerConfiguration & 0b00000100) >> 2; // bit 2
                uint8_t currentlyActivePowerModeA = (powerConfiguration & 0b00001000) >> 3; // bit 3
                uint8_t currentlyActivePowerModeMSB = (powerConfiguration & 0b00010000) >> 4; // bit 4
                uint32_t currentlyActivePowerMode = currentlyActivePowerModeLSB;
                currentlyActivePowerMode |= (currentlyActivePowerModeA << 1);
                currentlyActivePowerMode |= (currentlyActivePowerModeMSB << 2);
                bool noLowPowerMode = currentlyActivePowerMode == 0;
                bool sleepMode = currentlyActivePowerMode == 2;


                uint8_t mpptTemperatureSwitchEnabled = (powerConfiguration & 0b00100000) >> 5; // bit 5
                uint8_t mpptKeepAliveEnabled = (powerConfiguration  & 0b01000000) >> 6; // bit 6

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


                m_systemInfoUI->SystemInformation_MPPT_OutputVoltage_SpinBox->setValue(mpptOutputVoltage * 20);
                m_systemInfoUI->SystemInformation_MPPT_OutputCurrent_SpinBox->setValue(mpptOutputCurrent * 10);

                m_systemInfoUI->SystemInformation_UnixTimestamp_LineEdit->setText(QString::number(unixTimestamp));

                m_systemInfoUI->SystemInformation_PowerConfiguration_TransmissionsEnabled_Enabled_RadioButton->setChecked(transmissionsEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_TransmissionsEnabled_Disabled_RadioButton->setChecked(!transmissionsEnabled);

                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerMode_Enabled_RadioButton->setChecked(lowPowerModeEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerMode_Disabled_RadioButton->setChecked(!lowPowerModeEnabled);

                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerActiveMode_LP_RadioButton->setChecked(!noLowPowerMode);
                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerActiveMode_Sleep_RadioButton->setChecked(sleepMode);

                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Enabled_RadioButton->setChecked(mpptTemperatureSwitchEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Disabled_RadioButton->setChecked(!mpptTemperatureSwitchEnabled);

                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Enabled_RadioButton->setChecked(mpptKeepAliveEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Disabled_RadioButton->setChecked(!mpptKeepAliveEnabled);

                m_systemInfoUI->SystemInformation_ResetCounter_SpinBox->setValue(resetCounter);

                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelXAVoltage_SpinBox->setValue(solarPanel_XA_Voltage);
                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelXBVoltage_SpinBox->setValue(solarPanel_XB_Voltage);
                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelZAVoltage_SpinBox->setValue(solarPanel_ZA_Voltage);
                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelZBVoltage_SpinBox->setValue(solarPanel_ZB_Voltage);
                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelYVoltage_SpinBox->setValue(solarPanel_Y_Voltage);

                m_systemInfoUI->SystemInformation_TemperatureInformation_BatteryTemperature_DoubleSpinBox->setValue(batteryTemperature * 0.01f);
                m_systemInfoUI->SystemInformation_TemperatureInformation_OBCBoardTemperature_DoubleSpinBox->setValue(obcBoardTemperature * 0.01f);

                m_systemInfoUI->SystemInformation_FlashInformation_CRCError_SpinBox->setValue(flashSystemInfoPageCRCErrorCounter);
            }

        }
        else if (functionId == RESP_PACKET_INFO)
        {
            int8_t snr = optionalData[0];
            uint8_t rssi = optionalData[1];
            uint16_t numReceivedValidLoraFrames = optionalData[2];
            numReceivedValidLoraFrames |= optionalData[3] << 8;
            uint16_t numReceivedInvalidLoraFrames = optionalData[4];
            numReceivedInvalidLoraFrames |= optionalData[5] << 8;
            uint16_t numReceivedValidFSKFrames = optionalData[6];
            numReceivedValidFSKFrames |= optionalData[7] << 8;
            uint16_t numReceivedInvalidFSKFrames = optionalData[8];
            numReceivedInvalidFSKFrames |= optionalData[9] << 8;

            m_systemInfoUI->PacketInformation_SNR_SpinBox->setValue(snr);
            m_systemInfoUI->PacketInformation_RSSI_SpinBox->setValue(rssi);

            m_systemInfoUI->PacketInformation_LoraFrameInformation_NumValidFrames_SpinBox->setValue(numReceivedValidLoraFrames);
            m_systemInfoUI->PacketInformation_LoraFrameInformation_NumInvalidFrames_SpinBox->setValue(numReceivedInvalidLoraFrames);
            m_systemInfoUI->PacketInformation_FSKFrameInformation_NumValidFrames_SpinBox->setValue(numReceivedValidFSKFrames);
            m_systemInfoUI->PacketInformation_FSKFrameInformation_NumInvalidFrames_SpinBox->setValue(numReceivedInvalidFSKFrames);
        }
        else if (functionId == RESP_STATISTICS)
        {
            uint8_t flagByte = optionalData[0];
            bool temperaturesIncluded = flagByte & 0x01;
            bool currentsIncluded = (flagByte >> 1) & 0x01;
            bool voltagesIncluded = (flagByte >> 2) & 0x01;
            bool lightSensorsIncluded = (flagByte >> 3) & 0x01;
            bool imuIncluded = (flagByte >> 4) & 0x01;

            m_systemInfoUI->StatisticsControls_Flags_2_Temperatures_Enabled->setChecked(temperaturesIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_Temperatures_Disabled->setChecked(!temperaturesIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_Currents_Enabled->setChecked(currentsIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_Currents_Disabled->setChecked(!currentsIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_Voltages_Enabled->setChecked(voltagesIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_Voltages_Disabled->setChecked(!voltagesIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_LightSensors_Enabled->setChecked(lightSensorsIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_LightSensors_Disabled->setChecked(!lightSensorsIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_IMU_Enabled->setChecked(imuIncluded);
            m_systemInfoUI->StatisticsControls_Flags_2_IMU_Disabled->setChecked(!imuIncluded);

            m_systemInfoUI->LiveStatistics_Temperatures_PlainTextEdit->clear();
            m_systemInfoUI->LiveStatistics_Currents_PlainTextEdit->clear();
            m_systemInfoUI->LiveStatistics_Voltages_PlainTextEdit->clear();
            m_systemInfoUI->LiveStatistics_LightSensors_PlainTextEdit->clear();
            m_systemInfoUI->LiveStatistics_IMU_PlainTextEdit->clear();

            if (temperaturesIncluded)
            {
                for (int i = 0; i < 15; i++)
                {
                    int startIndex = 1 + (i*2);
                    uint8_t lsb = optionalData[startIndex];
                    uint8_t msb = optionalData[startIndex + 1];
                    int16_t temperature = lsb | (msb << 8);
                    float temperaturesRealValue = temperature * 0.01f;
                    m_systemInfoUI->LiveStatistics_Temperatures_PlainTextEdit->appendPlainText(QString::number(temperaturesRealValue) + QString(" ºC\n"));
                }
            }

            if (currentsIncluded)
            {
                for (int i = 0; i < 18; i++)
                {
                    int startIndex = 31 + (i*2);
                    uint8_t lsb = optionalData[startIndex];
                    uint8_t msb = optionalData[startIndex + 1];
                    int16_t current = lsb | (msb << 8);
                    float currentsValue = current * 10;
                    m_systemInfoUI->LiveStatistics_Currents_PlainTextEdit->appendPlainText(QString::number(currentsValue) + QString(" µA\n"));
                }
            }

            if (voltagesIncluded)
            {
                for (int i = 0; i < 18; i++)
                {
                    int startIndex = 67 + i;
                    uint8_t voltage = optionalData[startIndex];
                    m_systemInfoUI->LiveStatistics_Voltages_PlainTextEdit->appendPlainText(QString::number(voltage) + QString(" mV\n"));
                }
            }

            if (lightSensorsIncluded)
            {
                for (int i = 0; i < 6; i++)
                {
                    int startIndex = 85 + (i * 4);
                    uint8_t lsb = optionalData[startIndex];
                    uint8_t a = optionalData[startIndex + 1];
                    uint8_t b = optionalData[startIndex + 2];
                    uint8_t msb = optionalData[startIndex + 3];

                    uint32_t bytesVal = lsb;
                    bytesVal |= a << 8;
                    bytesVal |= b << 16;
                    bytesVal |= msb << 24;

                    float lightSensorValue = 0.0f;
                    memcpy(&lightSensorValue, &bytesVal, 4);
                    m_systemInfoUI->LiveStatistics_LightSensors_PlainTextEdit->appendPlainText(QString::number(lightSensorValue) + QString(" UNKNOWN\n"));
                }
            }

            if (imuIncluded)
            {
                for (int i = 0; i < 16; i++)
                {
                    int startIndex = 109 + (i * 4);

                    uint8_t lsb = optionalData[startIndex];
                    uint8_t a = optionalData[startIndex + 1];
                    uint8_t b = optionalData[startIndex + 2];
                    uint8_t msb = optionalData[startIndex + 3];

                    uint32_t bytesVal = lsb;
                    bytesVal |= a << 8;
                    bytesVal |= b << 16;
                    bytesVal |= msb << 24;

                    float imuValue = 0.0f;
                    memcpy(&imuValue, &bytesVal, 4);
                    m_systemInfoUI->LiveStatistics_IMU_PlainTextEdit->appendPlainText(QString::number(imuValue) + QString(" UNKNOWN\n"));
                }
            }
        }
        else if (functionId == RESP_FULL_SYSTEM_INFO)
        {
            uint8_t mpptOutputVoltage = optionalData[0];
            int16_t mpptOutputCurrent = optionalData[1] | (optionalData[2] << 8);

            uint32_t unixTimestamp = optionalData[3];
            unixTimestamp = unixTimestamp | (optionalData[4] << 8);
            unixTimestamp = unixTimestamp | (optionalData[5] << 16);
            unixTimestamp = unixTimestamp | (optionalData[6] << 24);

            uint8_t powerConfiguration = optionalData[7];
            uint8_t transmissionsEnabled = powerConfiguration & 0b00000001; // bit 0
            uint8_t lowPowerModeEnabled = (powerConfiguration & 0b00000010) >> 1; // bit 1

            uint8_t currentlyActivePowerModeLSB = (powerConfiguration & 0b00000100) >> 2; // bit 2
            uint8_t currentlyActivePowerModeA = (powerConfiguration & 0b00001000) >> 3; // bit 3
            uint8_t currentlyActivePowerModeMSB = (powerConfiguration & 0b00010000) >> 4; // bit 4
            uint32_t currentlyActivePowerMode = currentlyActivePowerModeLSB;
            currentlyActivePowerMode |= (currentlyActivePowerModeA << 1);
            currentlyActivePowerMode |= (currentlyActivePowerModeMSB << 2);
            bool noLowPowerMode = currentlyActivePowerMode == 0;
            bool sleepMode = currentlyActivePowerMode == 2;


            uint8_t mpptTemperatureSwitchEnabled = (powerConfiguration & 0b00100000) >> 5; // bit 5
            uint8_t mpptKeepAliveEnabled = (powerConfiguration  & 0b01000000) >> 6; // bit 6
            uint8_t scienceModeActive = (powerConfiguration & 0b10000000) >> 7; // bit 7

            uint16_t resetCounter = optionalData[8] | (optionalData[9] << 8);

            uint8_t solarPanel_XA_Voltage = optionalData[10];
            int16_t solarPanel_XA_Current = optionalData[11] | (optionalData[12] << 8);

            uint8_t solarPanel_XB_Voltage = optionalData[13];
            int16_t solarPanel_XB_Current = optionalData[14] | (optionalData[15] << 8);

            uint8_t solarPanel_ZA_Voltage = optionalData[16];
            int16_t solarPanel_ZA_Current = optionalData[17] | (optionalData[18] << 8);

            uint8_t solarPanel_ZB_Voltage = optionalData[19];
            int16_t solarPanel_ZB_Current = optionalData[20] | (optionalData[21] << 8);

            uint8_t solarPanel_Y_Voltage = optionalData[22];
            int16_t solarPanel_Y_Current = optionalData[23] | (optionalData[24] << 8);

            int16_t solarPanelYTemperature = optionalData[25] | (optionalData[26] << 8);
            int16_t obcBoardTemperature = optionalData[27] | (optionalData[28] << 8);
            int16_t bottomBoardTemperature = optionalData[29] | (optionalData[30] << 8);
            int16_t batteryTemperature = optionalData[31] | (optionalData[32] << 8);
            int16_t secondBatteryTemperature = optionalData[33] | (optionalData[34] << 8);
            int16_t mcuTemperature = optionalData[35] | (optionalData[36] << 8);

            uint32_t temp_yPanelLightSensor = optionalData[37];
            temp_yPanelLightSensor = optionalData[38] << 8;
            temp_yPanelLightSensor = optionalData[39] << 16;
            temp_yPanelLightSensor = optionalData[40] << 24;
            float yPanelLightSensor = 0.0f;
            memcpy(&yPanelLightSensor, &temp_yPanelLightSensor, 4);

            uint32_t temp_topBoardLightSensor = optionalData[41];
            temp_yPanelLightSensor = optionalData[42] << 8;
            temp_yPanelLightSensor = optionalData[43] << 16;
            temp_yPanelLightSensor = optionalData[44] << 24;
            float topBoardLightSensor = 0.0f;
            memcpy(&topBoardLightSensor, &temp_topBoardLightSensor, 4);

            uint8_t lastXAxisHBridgeFault = optionalData[45];
            uint8_t lastYAxisHBridgeFault = optionalData[46];
            uint8_t lastZAxisHBridgeFault = optionalData[47];


            int32_t flashSystemInfoPageCRCErrorCounter = optionalData[48];
            flashSystemInfoPageCRCErrorCounter |= (optionalData[49] << 8);
            flashSystemInfoPageCRCErrorCounter |= (optionalData[50] << 16);
            flashSystemInfoPageCRCErrorCounter |= (optionalData[51] << 24);

            uint8_t fskWindowReceiveLength = optionalData[52];
            uint8_t loraWindowReceiveLength = optionalData[53];

            uint8_t sensorStates = optionalData[54];
            uint8_t lightSensorTop = sensorStates & 0b00000001; // bit 0
            uint8_t lightSensorY = sensorStates & (0b00000001 << 1); // bit 1
            uint8_t currentSensorMPPT = sensorStates & (0b00000001 << 1); // bit 2
            uint8_t currentSensorY = sensorStates &  (0b00000001 << 2); // bit 3
            uint8_t currentSensorZB = sensorStates & (0b00000001 << 3); // bit 4
            uint8_t currentSensorZA = sensorStates & (0b00000001 << 4); // bit 5
            uint8_t currentSensorXB = sensorStates & (0b00000001 << 5); // bit 6
            uint8_t currentSensorXA = sensorStates & (0b00000001 << 6); // bit 7

            uint8_t lastADCSResult = optionalData[55];

            m_systemInfoUI->SystemInformation_MPPT_OutputVoltage_SpinBox->setValue(mpptOutputVoltage * 20);
            m_systemInfoUI->SystemInformation_MPPT_OutputCurrent_SpinBox->setValue(mpptOutputCurrent * 10);
            m_systemInfoUI->SystemInformation_UnixTimestamp_LineEdit->setText(QString::number(unixTimestamp));

            m_systemInfoUI->SystemInformation_PowerConfiguration_TransmissionsEnabled_Enabled_RadioButton->setChecked(transmissionsEnabled);
            m_systemInfoUI->SystemInformation_PowerConfiguration_TransmissionsEnabled_Disabled_RadioButton->setChecked(!transmissionsEnabled);

            m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerMode_Enabled_RadioButton->setChecked(lowPowerModeEnabled);
            m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerMode_Disabled_RadioButton->setChecked(!lowPowerModeEnabled);

            m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerActiveMode_LP_RadioButton->setChecked(!noLowPowerMode);
            m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerActiveMode_Sleep_RadioButton->setChecked(sleepMode);

            m_systemInfoUI->SystemInformation_ResetCounter_SpinBox->setValue(resetCounter);

            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelXAVoltage_SpinBox->setValue(solarPanel_XA_Voltage * 20);
            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelXACurrent_SpinBox->setValue(solarPanel_XA_Current * 10);

            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelXBVoltage_SpinBox->setValue(solarPanel_XB_Voltage * 20);
            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelXBCurrent_SpinBox->setValue(solarPanel_XB_Current * 10);

            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelZAVoltage_SpinBox->setValue(solarPanel_ZA_Voltage * 20);
            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelZACurrent_SpinBox->setValue(solarPanel_ZA_Current * 10);

            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelZBVoltage_SpinBox->setValue(solarPanel_ZB_Voltage * 20);
            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelZBCurrent_SpinBox->setValue(solarPanel_ZB_Current * 10);

            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelYVoltage_SpinBox->setValue(solarPanel_Y_Voltage * 20);
            m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelYCurrent_SpinBox->setValue(solarPanel_Y_Current * 10);

            m_systemInfoUI->SystemInformation_TemperatureInformation_YPanelTemperature_DoubleSpinBox->setValue(solarPanelYTemperature * 0.01f);
            m_systemInfoUI->SystemInformation_TemperatureInformation_OBCBoardTemperature_DoubleSpinBox->setValue(obcBoardTemperature * 0.01f);
            m_systemInfoUI->SystemInformation_TemperatureInformation_BottomBoardTemperature_DoubleSpinBox->setValue(bottomBoardTemperature * 0.01f);
            m_systemInfoUI->SystemInformation_TemperatureInformation_BatteryTemperature_DoubleSpinBox->setValue(batteryTemperature * 0.01f);
            m_systemInfoUI->SystemInformation_TemperatureInformation_SecondBatteryTemperature_DoubleSpinBox->setValue(secondBatteryTemperature * 0.01f);
            m_systemInfoUI->SystemInformation_TemperatureInformation_MCUTemperature_DoubleSpinBox->setValue(mcuTemperature * 0.01f);

            m_systemInfoUI->SystemInformation_Sensors_YPanelLightSensor_LineEdit->setText(QString::number(yPanelLightSensor));
            m_systemInfoUI->SystemInformation_Sensors_TopBoardLightSensor_LineEdit->setText(QString::number(topBoardLightSensor));

            m_systemInfoUI->SystemInformation_LastHBridgeFaults_XAxisHBridgeFault_SpinBox->setValue(lastXAxisHBridgeFault);
            m_systemInfoUI->SystemInformation_LastHBridgeFaults_YAxisHBridgeFault_SpinBox->setValue(lastYAxisHBridgeFault);
            m_systemInfoUI->SystemInformation_LastHBridgeFaults_ZAxisHBridgeFault_SpinBox->setValue(lastZAxisHBridgeFault);

            m_systemInfoUI->SystemInformation_FlashInformation_CRCError_SpinBox->setValue(flashSystemInfoPageCRCErrorCounter);

            m_systemInfoUI->SystemInformation_WindowReceiveLength_LoraWindowReceiveLength_SpinBox->setValue(fskWindowReceiveLength);
            m_systemInfoUI->SystemInformation_WindowReceiveLength_LoraWindowReceiveLength_SpinBox->setValue(loraWindowReceiveLength);

            ///\todo sensor states.

            m_systemInfoUI->SystemInformation_ADCResult_LastADCSResult_LineEdit->setText(QString::number(lastADCSResult));
        }
        else if (functionId == RESP_STORE_AND_FORWARD_ASSIGNED_SLOT)
        {
            ///\todo add ui component to signal slot assigned.
        }
        else if (functionId == RESP_FORWARDED_MESSAGE)
        {

        }
        else if (functionId == RESP_PUBLIC_PICTURE)
        {

        }
        else if (functionId == RESP_DEPLOYMENT_STATE)
        {
            uint8_t counter = optionalData[0];
            m_systemInfoUI->SystemInformation_Deployment_Counter_SpinBox->setValue(counter);
        }
        else if (functionId == RESP_CAMERA_STATE)
        {

        }
        else if (functionId == RESP_RECORDED_IMU)
        {
            uint8_t deviceFlags = optionalData[0];
            uint8_t gyro = deviceFlags & 0b00000001;
            uint8_t accele = (deviceFlags>>1) & 0b00000001;
            uint8_t magn = (deviceFlags>>2) & 0b00000001;

            uint32_t gyroSampleX = optionalData[1];
            gyroSampleX |= optionalData[2] << 8;
            gyroSampleX |= optionalData[3] << 16;
            gyroSampleX |= optionalData[4] << 24;
            float gyroX = 0.0f;
            memcpy(&gyroX, &gyroSampleX, 4);

            uint32_t gyroSampleY = optionalData[5];
            gyroSampleY |= optionalData[6] << 8;
            gyroSampleY |= optionalData[7] << 16;
            gyroSampleY |= optionalData[8] << 24;
            float gyroY = 0.0f;
            memcpy(&gyroY, &gyroSampleY, 4);

            uint32_t gyroSampleZ = optionalData[9];
            gyroSampleZ |= optionalData[10] << 8;
            gyroSampleZ |= optionalData[11] << 16;
            gyroSampleZ |= optionalData[12] << 24;
            float gyroZ = 0.0f;
            memcpy(&gyroZ, &gyroSampleZ, 4);

            uint32_t acceleSampleX = optionalData[13];
            acceleSampleX |= optionalData[14] << 8;
            acceleSampleX |= optionalData[15] << 16;
            acceleSampleX |= optionalData[16] << 24;
            float acceleX = 0.0f;
            memcpy(&acceleX, &acceleSampleX, 4);

            uint32_t acceleSampleY = optionalData[17];
            acceleSampleY |= optionalData[18] << 8;
            acceleSampleY |= optionalData[19] << 16;
            acceleSampleY |= optionalData[20] << 24;
            float acceleY = 0.0f;
            memcpy(&acceleY, &acceleSampleY, 4);

            uint32_t acceleSampleZ = optionalData[21];
            acceleSampleZ |= optionalData[22] << 8;
            acceleSampleZ |= optionalData[23] << 16;
            acceleSampleZ |= optionalData[24] << 24;
            float acceleZ = 0.0f;
            memcpy(&acceleZ, &acceleSampleZ, 4);

            uint32_t magnSampleX = optionalData[25];
            magnSampleX |= optionalData[26] << 8;
            magnSampleX |= optionalData[27] << 16;
            magnSampleX |= optionalData[28] << 24;
            float magnX = 0.0f;
            memcpy(&magnX, &magnSampleX, 4);

            uint32_t magnSampleY = optionalData[29];
            magnSampleY |= optionalData[30] << 8;
            magnSampleY |= optionalData[31] << 16;
            magnSampleY |= optionalData[32] << 24;
            float magnY = 0.0f;
            memcpy(&magnY, &magnSampleY, 4);

            uint32_t magnSampleZ = optionalData[33];
            magnSampleZ |= optionalData[34] << 8;
            magnSampleZ |= optionalData[35] << 16;
            magnSampleZ |= optionalData[36] << 24;
            float magnZ = 0.0f;
            memcpy(&magnZ, &magnSampleZ, 4);

            m_systemInfoUI->SystemInformation_IMUInformation_GryroX_DoubleSpinBox->setValue(gyroX);
            m_systemInfoUI->SystemInformation_IMUInformation_GryroY_DoubleSpinBox->setValue(gyroY);
            m_systemInfoUI->SystemInformation_IMUInformation_GryroZ_DoubleSpinBox->setValue(gyroZ);

            m_systemInfoUI->SystemInformation_IMUInformation_AcceleX_DoubleSpinBox->setValue(acceleX);
            m_systemInfoUI->SystemInformation_IMUInformation_AcceleX_DoubleSpinBox->setValue(acceleY);
            m_systemInfoUI->SystemInformation_IMUInformation_AcceleX_DoubleSpinBox->setValue(acceleZ);

            m_systemInfoUI->SystemInformation_IMUInformation_MagX_DoubleSpinBox->setValue(magnX);
            m_systemInfoUI->SystemInformation_IMUInformation_MagY_DoubleSpinBox->setValue(magnY);
            m_systemInfoUI->SystemInformation_IMUInformation_MagZ_DoubleSpinBox->setValue(magnZ);

        }
        else if (functionId == RESP_CAMERA_PICTURE_LENGTH)
        {

        }
        else if (functionId == RESP_RECORDED_SOLAR_CELLS)
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


