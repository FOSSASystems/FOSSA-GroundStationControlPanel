#include "FOSSASAT2_MessageHandler.h"

#include "../systeminformationpane.h"
#include "../ui_systeminformationpane.h"
#include "../mainwindow.h"
#include "../ui_mainwindow.h"

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::SystemInfo msg, systeminformationpane* systemInfoPane)
{
    Ui::systeminformationpane* ui = systemInfoPane->GetUI();

    ui->SystemInformation_MPPT_OutputVoltage_SpinBox->setValue(msg.GetMpptOutputVoltage());
    ui->SystemInformation_MPPT_OutputCurrent_SpinBox->setValue(msg.GetMpptOutputCurrent());

    ui->SystemInformation_UnixTimestamp_LineEdit->setText(QString::number(msg.GetUnixTimestamp()));

    ui->SystemInformation_PowerConfiguration_TransmissionsEnabled_Enabled_RadioButton->setChecked(msg.IsTransmissionsEnabled());
    ui->SystemInformation_PowerConfiguration_TransmissionsEnabled_Disabled_RadioButton->setChecked(!msg.IsTransmissionsEnabled());

    ui->SystemInformation_PowerConfiguration_LowPowerMode_Enabled_RadioButton->setChecked(msg.IsLowPowerModeEnabled());
    ui->SystemInformation_PowerConfiguration_LowPowerMode_Disabled_RadioButton->setChecked(!msg.IsLowPowerModeEnabled());

    ui->SystemInformation_PowerConfiguration_LowPowerActiveMode_LP_RadioButton->setChecked(msg.IsLowPowerModeActive());
    ui->SystemInformation_PowerConfiguration_LowPowerActiveMode_Sleep_RadioButton->setChecked(msg.IsSleepMode());

    ui->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Enabled_RadioButton->setChecked(msg.GetMpptTemperatureSwitchEnabled());
    ui->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Disabled_RadioButton->setChecked(!msg.GetMpptTemperatureSwitchEnabled());

    ui->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Enabled_RadioButton->setChecked(msg.GetMpptKeepAliveEnabled());
    ui->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Disabled_RadioButton->setChecked(!msg.GetMpptKeepAliveEnabled());

    ui->SystemInformation_ResetCounter_SpinBox->setValue(msg.GetResetCounter());

    ui->SystemInformation_SolarPanelsPowerInformation_SolarPanelXAVoltage_SpinBox->setValue(msg.GetSolarPanelXaVoltage());
    ui->SystemInformation_SolarPanelsPowerInformation_SolarPanelXBVoltage_SpinBox->setValue(msg.GetSolarPanelXbVoltage());
    ui->SystemInformation_SolarPanelsPowerInformation_SolarPanelZAVoltage_SpinBox->setValue(msg.GetSolarPanelZaVoltage());
    ui->SystemInformation_SolarPanelsPowerInformation_SolarPanelZBVoltage_SpinBox->setValue(msg.GetSolarPanelZbVoltage());
    ui->SystemInformation_SolarPanelsPowerInformation_SolarPanelYVoltage_SpinBox->setValue(msg.GetSolarPanelYVoltage());

    ui->SystemInformation_TemperatureInformation_BatteryTemperature_DoubleSpinBox->setValue(msg.GetBatteryTemperature());
    ui->SystemInformation_TemperatureInformation_OBCBoardTemperature_DoubleSpinBox->setValue(msg.GetObcBoardTemperature());

    ui->SystemInformation_FlashInformation_CRCError_SpinBox->setValue(msg.GetFlashSystemInfoPageCrcErrorCounter());
}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::PacketInfo msg, systeminformationpane* systemInfoPane)
{
    Ui::systeminformationpane* ui = systemInfoPane->GetUI();

    ui->PacketInformation_SNR_SpinBox->setValue(msg.GetSnr());
    ui->PacketInformation_RSSI_SpinBox->setValue(msg.GetRssi());

    ui->PacketInformation_LoraFrameInformation_NumValidFrames_SpinBox->setValue(msg.GetNumReceivedValidLoraFrames());
    ui->PacketInformation_LoraFrameInformation_NumInvalidFrames_SpinBox->setValue(msg.GetNumReceivedInvalidLoraFrames());
    ui->PacketInformation_FSKFrameInformation_NumValidFrames_SpinBox->setValue(msg.GetNumReceivedValidFskFrames());
    ui->PacketInformation_FSKFrameInformation_NumInvalidFrames_SpinBox->setValue(msg.GetNumReceivedInvalidFskFrames());
}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::Statistics msg, systeminformationpane* systemInfoPane)
{
    Ui::systeminformationpane* ui = systemInfoPane->GetUI();

    ui->StatisticsControls_Flags_2_Temperatures_Enabled->setChecked(msg.IsTemperaturesIncluded());
    ui->StatisticsControls_Flags_2_Temperatures_Disabled->setChecked(!msg.IsTemperaturesIncluded());

    ui->StatisticsControls_Flags_2_Currents_Enabled->setChecked(msg.IsCurrentsIncluded());
    ui->StatisticsControls_Flags_2_Currents_Disabled->setChecked(!msg.IsCurrentsIncluded());

    ui->StatisticsControls_Flags_2_Voltages_Enabled->setChecked(msg.IsVoltagesIncluded());
    ui->StatisticsControls_Flags_2_Voltages_Disabled->setChecked(!msg.IsVoltagesIncluded());

    ui->StatisticsControls_Flags_2_LightSensors_Enabled->setChecked(msg.IsLightSensorsIncluded());
    ui->StatisticsControls_Flags_2_LightSensors_Disabled->setChecked(!msg.IsLightSensorsIncluded());

    ui->StatisticsControls_Flags_2_IMU_Enabled->setChecked(msg.IsImuIncluded());
    ui->StatisticsControls_Flags_2_IMU_Disabled->setChecked(!msg.IsImuIncluded());

    ui->LiveStatistics_Temperatures_PlainTextEdit->clear();
    ui->LiveStatistics_Currents_PlainTextEdit->clear();
    ui->LiveStatistics_Voltages_PlainTextEdit->clear();
    ui->LiveStatistics_LightSensors_PlainTextEdit->clear();
    ui->LiveStatistics_IMU_PlainTextEdit->clear();

    if (msg.IsTemperaturesIncluded())
    {
        const std::vector<float>& temperatures = msg.GetTemperatures();
        for (float temperature : temperatures)
        {
            ui->LiveStatistics_Temperatures_PlainTextEdit->appendPlainText(QString::number(temperature) + QString(" ºC\n"));
        }
    }


    if (msg.IsVoltagesIncluded())
    {
        const std::vector<float>& voltages = msg.GetVoltages();
        for (float voltage : voltages)
        {
            ui->LiveStatistics_Voltages_PlainTextEdit->appendPlainText(QString::number(voltage) + QString(" mV\n"));
        }
    }

    if (msg.IsLightSensorsIncluded())
    {
        const std::vector<float>& lightSensors = msg.GetLightSensors();
        for (float lightSensor : lightSensors) {

            ui->LiveStatistics_LightSensors_PlainTextEdit->appendPlainText(QString::number(lightSensor) + QString(" ???\n"));
        }
    }

    if (msg.IsImuIncluded())
    {
        const std::vector<float>& imus = msg.GetImus();
        for (float imu : imus) {

            ui->LiveStatistics_IMU_PlainTextEdit->appendPlainText(QString::number(imu) + QString(" ???\n"));
        }
    }
}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::FullSystemInfo msg, systeminformationpane* systemInfoPane)
{

}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::DeploymentState msg, systeminformationpane* systemInfoPane)
{

}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::RecordedIMU msg, systeminformationpane* systemInfoPane)
{

}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::GPSLogState msg, MainWindow* mainWindow)
{

}
