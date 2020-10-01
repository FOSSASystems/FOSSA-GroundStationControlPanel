#include "FOSSASAT2_MessageHandler.h"

#include "../ui_systeminformationpane.h"

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::SystemInfo msg, systeminformationpane* systemInfoPane)
{
    systemInfoPane->GetUI()->SystemInformation_MPPT_OutputVoltage_SpinBox->setValue(msg.GetMpptOutputVoltage());
    systemInfoPane->GetUI()->SystemInformation_MPPT_OutputCurrent_SpinBox->setValue(msg.GetMpptOutputCurrent());

    systemInfoPane->GetUI()->SystemInformation_UnixTimestamp_LineEdit->setText(QString::number(msg.GetUnixTimestamp()));

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_TransmissionsEnabled_Enabled_RadioButton->setChecked(msg.IsTransmissionsEnabled());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_TransmissionsEnabled_Disabled_RadioButton->setChecked(!msg.IsTransmissionsEnabled());

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_LowPowerMode_Enabled_RadioButton->setChecked(msg.IsLowPowerModeEnabled());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_LowPowerMode_Disabled_RadioButton->setChecked(!msg.IsLowPowerModeEnabled());

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_LowPowerActiveMode_LP_RadioButton->setChecked(msg.IsLowPowerModeActive());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_LowPowerActiveMode_Sleep_RadioButton->setChecked(msg.IsSleepMode());

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Enabled_RadioButton->setChecked(msg.GetMpptTemperatureSwitchEnabled());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Disabled_RadioButton->setChecked(!msg.GetMpptTemperatureSwitchEnabled());

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Enabled_RadioButton->setChecked(msg.GetMpptKeepAliveEnabled());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Disabled_RadioButton->setChecked(!msg.GetMpptKeepAliveEnabled());

    systemInfoPane->GetUI()->SystemInformation_ResetCounter_SpinBox->setValue(msg.GetResetCounter());

    systemInfoPane->GetUI()->SystemInformation_SolarPanelsPowerInformation_SolarPanelXAVoltage_SpinBox->setValue(msg.GetSolarPanelXaVoltage());
    systemInfoPane->GetUI()->SystemInformation_SolarPanelsPowerInformation_SolarPanelXBVoltage_SpinBox->setValue(msg.GetSolarPanelXbVoltage());
    systemInfoPane->GetUI()->SystemInformation_SolarPanelsPowerInformation_SolarPanelZAVoltage_SpinBox->setValue(msg.GetSolarPanelZaVoltage());
    systemInfoPane->GetUI()->SystemInformation_SolarPanelsPowerInformation_SolarPanelZBVoltage_SpinBox->setValue(msg.GetSolarPanelZbVoltage());
    systemInfoPane->GetUI()->SystemInformation_SolarPanelsPowerInformation_SolarPanelYVoltage_SpinBox->setValue(msg.GetSolarPanelYVoltage());

    systemInfoPane->GetUI()->SystemInformation_TemperatureInformation_BatteryTemperature_DoubleSpinBox->setValue(msg.GetBatteryTemperature());
    systemInfoPane->GetUI()->SystemInformation_TemperatureInformation_OBCBoardTemperature_DoubleSpinBox->setValue(msg.GetObcBoardTemperature());

    systemInfoPane->GetUI()->SystemInformation_FlashInformation_CRCError_SpinBox->setValue(msg.GetFlashSystemInfoPageCrcErrorCounter());
}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::PacketInfo msg)
{

}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::Statistics msg)
{

}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::FullSystemInfo msg)
{

}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::DeploymentState msg)
{

}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::RecordedIMU msg)
{

}

void FOSSASAT2::MessageHandler::Handle(FOSSASAT2::Messages::GPSLogState msg)
{

}
