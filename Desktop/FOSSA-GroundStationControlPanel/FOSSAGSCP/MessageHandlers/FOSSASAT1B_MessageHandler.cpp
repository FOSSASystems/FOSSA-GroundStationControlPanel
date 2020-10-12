#include "FOSSASAT1B_MessageHandler.h"

#include "../systeminformationpane.h"
#include "../ui_systeminformationpane.h"

void FOSSASAT1B::MessageHandler::Handle(FOSSASAT1B::Messages::SystemInfo systemInfo, systeminformationpane* systemInfoPane)
{
    systemInfoPane->GetUI()->SystemInformation_1B_BatteryVoltage_SpinBox->setValue(systemInfo.GetBatteryVoltage());
    systemInfoPane->GetUI()->SystemInformation_1B_BatteryChargingCurrent_SpinBox->setValue(systemInfo.GetBatteryChargingCurrent());
    systemInfoPane->GetUI()->SystemInformation_1B_BatteryChargingVoltage_SpinBox->setValue(systemInfo.GetBatteryChargingVoltage());

    systemInfoPane->GetUI()->SystemInformation_1B_TimeSinceLastReset_SpinBox->setValue(systemInfo.GetTimeSinceLastReset());

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_LowPowerMode_Enabled_RadioButton->setChecked(systemInfo.GetLowPowerModeEnabled());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_LowPowerMode_Disabled_RadioButton->setChecked(!systemInfo.GetLowPowerModeEnabled());

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Enabled_RadioButton->setChecked(systemInfo.GetMpptTemperatureSwitchEnabled());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_MPPTTemperatureSwitchEnabled_Disabled_RadioButton->setChecked(!systemInfo.GetMpptTemperatureSwitchEnabled());

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Enabled_RadioButton->setChecked(systemInfo.GetMpptKeepAliveEnabled());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_MPPTKeepAliveEnabled_Disabled_RadioButton->setChecked(!systemInfo.GetMpptKeepAliveEnabled());

    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_TransmissionsEnabled_Enabled_RadioButton->setChecked(systemInfo.GetTransmissionsEnabled());
    systemInfoPane->GetUI()->SystemInformation_PowerConfiguration_TransmissionsEnabled_Disabled_RadioButton->setChecked(!systemInfo.GetTransmissionsEnabled());

    systemInfoPane->GetUI()->SystemInformation_ResetCounter_SpinBox->setValue(systemInfo.GetResetCounter());

    systemInfoPane->GetUI()->SystemInformation_RecordSolarCells_SolarCellAVoltage_SpinBox->setValue(systemInfo.GetSolarCellAVoltage());
    systemInfoPane->GetUI()->SystemInformation_RecordSolarCells_SolarCellBVoltage_SpinBox->setValue(systemInfo.GetSolarCellBVoltage());
    systemInfoPane->GetUI()->SystemInformation_RecordSolarCells_SolarCellCVoltage_SpinBox->setValue(systemInfo.GetSolarCellCVoltage());

    systemInfoPane->GetUI()->SystemInformation_TemperatureInformation_BatteryTemperature_DoubleSpinBox->setValue(systemInfo.GetBatteryTemperature());
    systemInfoPane->GetUI()->SystemInformation_TemperatureInformation_OBCBoardTemperature_DoubleSpinBox->setValue(systemInfo.GetObcBoardTemperature());
    systemInfoPane->GetUI()->SystemInformation_TemperatureInformation_MCUTemperature_DoubleSpinBox->setValue(systemInfo.GetMcuTemperature());
}
