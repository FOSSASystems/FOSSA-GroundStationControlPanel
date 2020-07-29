#include "DatagramProcessor.h"

DatagramProcessor::DatagramProcessor(Ui::MainWindow* mainWindowUI, Ui::systeminformationpane* systemInfoIUI)
    : m_mainWindowUI(mainWindowUI), m_systemInfoUI(systemInfoIUI)
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
         qInfo() << functionId;
         qInfo() << optionalDataLength;
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

        }
        else if (functionId == RESP_REPEATED_MESSAGE_CUSTOM)
        {

        }
        else if (functionId == RESP_SYSTEM_INFO)
        {
            if (Settings::GetSatVersion() == VERSION_1B)
            {
                uint8_t batteryVoltage = optionalData[0];
                int16_t batteryChargingCurrent = optionalData[1] | (optionalData[2] << 8);
                uint8_t batteryChargingVoltage = optionalData[3];

                uint32_t timeSinceLastReset = optionalData[4] |  optionalData[5] | optionalData[6] | optionalData[7];

                uint8_t powerConfiguration = optionalData[8];
                bool lowPowerModeActive = powerConfiguration & 0b00000001;
                bool lowPowerModeEnabled = powerConfiguration & 0b00000010;
                bool mpptTemperatureSwitchEnabled = powerConfiguration & 0b00000100;
                bool mpptKeepAliveEnabled = powerConfiguration & 0b00010000;

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

                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerModeActive_Activated_RadioButton->setChecked(lowPowerModeActive);
                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerMode_Enabled_RadioButton->setChecked(lowPowerModeEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Enabled_RadioButton->setChecked(mpptTemperatureSwitchEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Enabled_RadioButton->setChecked(mpptKeepAliveEnabled);

                m_systemInfoUI->SystemInformation_ResetCounter_SpinBox->setValue(resetCounter);

                m_systemInfoUI->SystemInformation_RecordSolarCells_SolarCellAVoltage_SpinBox->setValue(solarCellAVoltage * 20);
                m_systemInfoUI->SystemInformation_RecordSolarCells_SolarCellBVoltage_SpinBox->setValue(solarCellBVoltage * 20);
                m_systemInfoUI->SystemInformation_RecordSolarCells_SolarCellCVoltage_SpinBox->setValue(solarCellCVoltage * 20);

                m_systemInfoUI->SystemInformation_TemperatureInformation_BatteryTemperature_SpinBox->setValue(batteryTemperature * 0.01);
                m_systemInfoUI->SystemInformation_TemperatureInformation_OBCBoardTemperature_SpinBox->setValue(obcBoardTemperature * 0.01);
                m_systemInfoUI->SystemInformation_TemperatureInformation_MCUTemperature_SpinBox->setValue(mcuTemperature);
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


                m_systemInfoUI->SystemInformation_MPPT_OutputVoltage_SpinBox->setValue(mpptOutputVoltage);
                m_systemInfoUI->SystemInformation_MPPT_OutputCurrent_SpinBox->setValue(mpptOutputCurrent);

                m_systemInfoUI->SystemInformation_UnixTimestamp_SpinBox->setValue(unixTimestamp);

                m_systemInfoUI->SystemInformation_PowerConfiguration_TransmissionsEnabled_Enabled_RadioButton->setChecked(transmissionsEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerMode_Enabled_RadioButton->setChecked(lowPowerModeEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_LowPowerModeActive_Activated_RadioButton->setChecked(currentlyActivePowerMode);
                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Enabled_RadioButton->setChecked(mpptTemperatureSwitchEnabled);
                m_systemInfoUI->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Enabled_RadioButton->setChecked(mpptKeepAliveEnabled);

                m_systemInfoUI->SystemInformation_ResetCounter_SpinBox->setValue(resetCounter);

                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelXAVoltage_SpinBox->setValue(solarPanel_XA_Voltage);
                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelXBVoltage_SpinBox->setValue(solarPanel_XB_Voltage);
                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelZAVoltage_SpinBox->setValue(solarPanel_ZA_Voltage);
                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelZBVoltage_SpinBox->setValue(solarPanel_ZB_Voltage);
                m_systemInfoUI->SystemInformation_SolarPanelsPowerInformation_SolarPanelYVoltage_SpinBox->setValue(solarPanel_Y_Voltage);

                m_systemInfoUI->SystemInformation_TemperatureInformation_BatteryTemperature_SpinBox->setValue(batteryTemperature);
                m_systemInfoUI->SystemInformation_TemperatureInformation_OBCBoardTemperature_SpinBox->setValue(obcBoardTemperature);

                m_systemInfoUI->SystemInformation_FlashInformation_CRCError_SpinBox->setValue(flashSystemInfoPageCRCErrorCounter);
            }

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


