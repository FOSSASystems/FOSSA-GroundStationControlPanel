// MIT LICENSE
//
// Copyright (c) 2020 FOSSA Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "FOSSASAT2_FullSystemInfo.h"

FOSSASAT2::Messages::FullSystemInfo::FullSystemInfo(Frame &frame)
{
    this->mpptOutputVoltage = frame.GetByteAt(0);
    this->mpptOutputVoltage *= 20;

    this->mpptOutputCurrent = frame.GetByteAt(1)| (frame.GetByteAt(2)<< 8);
    this->mpptOutputCurrent *= 10;

    this->unixTimestamp = frame.GetByteAt(3);
    this->unixTimestamp |= (frame.GetByteAt(4) << 8);
    this->unixTimestamp |= (frame.GetByteAt(5) << 16);
    this->unixTimestamp |= (frame.GetByteAt(6) << 24);

    this->powerConfiguration = frame.GetByteAt(7);
    this->transmissionsEnabled = powerConfiguration & 0b00000001; // bit 0
    this->lowPowerModeEnabled = (powerConfiguration & 0b00000010) >> 1; // bit 1

    uint8_t currentlyActivePowerModeLSB = (powerConfiguration & 0b00000100) >> 2; // bit 2
    uint8_t currentlyActivePowerModeA = (powerConfiguration & 0b00001000) >> 3; // bit 3
    uint8_t currentlyActivePowerModeMSB = (powerConfiguration & 0b00010000) >> 4; // bit 4
    this->currentlyActivePowerMode = currentlyActivePowerModeLSB;
    this->currentlyActivePowerMode |= (currentlyActivePowerModeA << 1);
    this->currentlyActivePowerMode |= (currentlyActivePowerModeMSB << 2);

    this->noLowPowerMode = currentlyActivePowerMode == 0;
    this->sleepMode = currentlyActivePowerMode == 2;


    this->mpptTemperatureSwitchEnabled = (powerConfiguration & 0b00100000) >> 5; // bit 5
    this->mpptKeepAliveEnabled = (powerConfiguration  & 0b01000000) >> 6; // bit 6
    this->scienceModeActive = (powerConfiguration & 0b10000000) >> 7; // bit 7

    this->resetCounter = frame.GetByteAt(8) | (frame.GetByteAt(9)<< 8);

    this->solarPanel_XA_Voltage = frame.GetByteAt(10);
    this->solarPanel_XA_Voltage *= 20;
    this->solarPanel_XA_Current = frame.GetByteAt(11)| (frame.GetByteAt(12)<< 8);
    this->solarPanel_XA_Current *= 10;

    this->solarPanel_XB_Voltage = frame.GetByteAt(13);
    this->solarPanel_XB_Voltage *= 20;
    this->solarPanel_XB_Current = frame.GetByteAt(14)| (frame.GetByteAt(15)<< 8);
    this->solarPanel_XB_Current *= 10;

    this->solarPanel_ZA_Voltage = frame.GetByteAt(16);
    this->solarPanel_ZA_Voltage *= 20;
    this->solarPanel_ZA_Current = frame.GetByteAt(17)| (frame.GetByteAt(18)<< 8);
    this->solarPanel_ZA_Current *= 10;

    this->solarPanel_ZB_Voltage = frame.GetByteAt(19);
    this->solarPanel_ZB_Voltage *= 20;
    this->solarPanel_ZB_Current = frame.GetByteAt(20)| (frame.GetByteAt(21)<< 8);
    this->solarPanel_ZB_Current *= 10;

    this->solarPanel_Y_Voltage = frame.GetByteAt(22);
    this->solarPanel_Y_Voltage *= 20;
    this->solarPanel_Y_Current = frame.GetByteAt(23)| (frame.GetByteAt(24)<< 8);
    this->solarPanel_Y_Current *= 10;

    this->solarPanelYTemperature = frame.GetByteAt(25)| (frame.GetByteAt(26)<< 8);
    this->solarPanelYTemperature *= 0.01f;
    this->obcBoardTemperature = frame.GetByteAt(27)| (frame.GetByteAt(28)<< 8);
    this->obcBoardTemperature *= 0.01f;
    this->bottomBoardTemperature = frame.GetByteAt(29)| (frame.GetByteAt(30)<< 8);
    this->bottomBoardTemperature *= 0.01f;
    this->batteryTemperature = frame.GetByteAt(31)| (frame.GetByteAt(32)<< 8);
    this->batteryTemperature *= 0.01f;
    this->secondBatteryTemperature = frame.GetByteAt(33)| (frame.GetByteAt(34)<< 8);
    this->secondBatteryTemperature *= 0.01f;
    this->mcuTemperature = frame.GetByteAt(35)| (frame.GetByteAt(36)<< 8);
    this->mcuTemperature *= 0.01f;

    uint32_t temp_yPanelLightSensor = frame.GetByteAt(37);
    temp_yPanelLightSensor = frame.GetByteAt(38)<< 8;
    temp_yPanelLightSensor = frame.GetByteAt(39)<< 16;
    temp_yPanelLightSensor = frame.GetByteAt(40)<< 24;
    memcpy(&this->yPanelLightSensor, &temp_yPanelLightSensor, 4);

    uint32_t temp_topBoardLightSensor = frame.GetByteAt(41);
    temp_yPanelLightSensor = frame.GetByteAt(42)<< 8;
    temp_yPanelLightSensor = frame.GetByteAt(43)<< 16;
    temp_yPanelLightSensor = frame.GetByteAt(44)<< 24;
    memcpy(&this->topBoardLightSensor, &temp_topBoardLightSensor, 4);

    this->lastXAxisHBridgeFault = frame.GetByteAt(45);
    this->lastYAxisHBridgeFault = frame.GetByteAt(46);
    this->lastZAxisHBridgeFault = frame.GetByteAt(47);


    this->flashSystemInfoPageCRCErrorCounter = frame.GetByteAt(48);
    this->flashSystemInfoPageCRCErrorCounter |= (frame.GetByteAt(49)<< 8);
    this->flashSystemInfoPageCRCErrorCounter |= (frame.GetByteAt(50)<< 16);
    this->flashSystemInfoPageCRCErrorCounter |= (frame.GetByteAt(51)<< 24);

    this->fskWindowReceiveLength = frame.GetByteAt(52);
    this->loraWindowReceiveLength = frame.GetByteAt(53);

    this->sensorStates = frame.GetByteAt(54);
    this->lightSensorTop = sensorStates & 0b00000001; // bit 0
    this->lightSensorY = sensorStates & (0b00000001 << 1); // bit 1
    this->currentSensorMPPT = sensorStates & (0b00000001 << 1); // bit 2
    this->currentSensorY = sensorStates &  (0b00000001 << 2); // bit 3
    this->currentSensorZB = sensorStates & (0b00000001 << 3); // bit 4
    this->currentSensorZA = sensorStates & (0b00000001 << 4); // bit 5
    this->currentSensorXB = sensorStates & (0b00000001 << 5); // bit 6
    this->currentSensorXA = sensorStates & (0b00000001 << 6); // bit 7

    this->lastADCSResult = frame.GetByteAt(55);
}

std::string FOSSASAT2::Messages::FullSystemInfo::ToString()
{
    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT2" << std::endl;
    ss << "Message Name: FullSystemInfo" << std::endl;
    ss << "MPPT output voltage: " << this->mpptOutputVoltage << "mV" << std::endl;
    ss << "MPPT output current: " << this->mpptOutputCurrent  << "uA" << std::endl;
    ss << "Timestamp: " << this->unixTimestamp  << std::endl;
    ss << "Transmissions enabled: " << this->transmissionsEnabled << std::endl;
    ss << "LowPower mode enabled: " << this->lowPowerModeEnabled << std::endl;
    ss << "Currently active low power mode: " << this->currentlyActivePowerMode << std::endl;
    ss << "MPPT temperature switch enabled: " << this->mpptTemperatureSwitchEnabled  << std::endl;
    ss << "MPPT keep alive enabled: " << this->mpptKeepAliveEnabled  << std::endl;
    ss << "Science mode active: " << this->scienceModeActive << std::endl;
    ss << "Reset counter: " << this->resetCounter << std::endl;
    ss << "Solar panel XA voltage: " << this->solarPanel_XA_Voltage  << "mV" << std::endl;
    ss << "solar panel XA current: " << this->solarPanel_XA_Current  << "uA" << std::endl;
    ss << "Solar panel XB voltage: " << this->solarPanel_XB_Voltage  << "mV" << std::endl;
    ss << "Solar panel XB current: " << this->solarPanel_XB_Current  << "uA" << std::endl;
    ss << "Solar panel ZA voltage: " << this->solarPanel_ZA_Voltage    << "mV" << std::endl;
    ss << "Solar panel ZA current : " << this->solarPanel_ZA_Current  << "uA" << std::endl;
    ss << "Solar panel ZB voltage: " << this->solarPanel_ZB_Voltage  << "mV" << std::endl;
    ss << "Solar panel ZB current: " << this->solarPanel_ZB_Current  << "uA" << std::endl;
    ss << "Solar panel Y voltage: " << this->solarPanel_Y_Voltage  << "mV" << std::endl;
    ss << "Solar panel Y current: " << this->solarPanel_Y_Current  << "uA" << std::endl;
    ss << "Y panel temperature: " << this->solarPanelYTemperature  << "Deg. C." << std::endl;
    ss << "OBC board temperature: " << this->obcBoardTemperature    << "Deg. C." << std::endl;
    ss << "Bottom board temperature : " << this->bottomBoardTemperature  << "Deg. C." << std::endl;
    ss << "Battery temperature: " << this->batteryTemperature  << "Deg. C." << std::endl;
    ss << "Second battery temperature: " << this->secondBatteryTemperature  << "Deg. C." << std::endl;
    ss << "MCU temperature: " << this->mcuTemperature  << "Deg. C." << std::endl;
    ss << "Y panel light sensor: " << this->yPanelLightSensor << std::endl;
    ss << "Top board light sensor: " << this->topBoardLightSensor << std::endl;
    ss << "Last X axis H-bridge fault: " << this->lastXAxisHBridgeFault  << std::endl;
    ss << "Last Y axis H-bridge fault: " << this->lastYAxisHBridgeFault  << std::endl;
    ss << "Last Z axis H-bridge fault: " << this->lastZAxisHBridgeFault  << std::endl;
    ss << "External flash system info page CRC error counter: " << this->flashSystemInfoPageCRCErrorCounter << std::endl;
    ss << "FSK window receive length: " << this->fskWindowReceiveLength  << "s" << std::endl;
    ss << "LoRa window receive length: " << this->loraWindowReceiveLength  << "s" <<  std::endl;
    ss << "Light sensor top: " << this->lightSensorTop  << " Lux" << std::endl;
    ss << "Light sensor Y: " << this->lightSensorY  << " Lux" <<  std::endl;
    ss << "Current sensor MPPT: " << this->currentSensorMPPT  << std::endl;
    ss << "current sensor Y: " << this->currentSensorY  << std::endl;
    ss << "current sensor ZB: " << this->currentSensorZB  << std::endl;
    ss << "current sensor ZA: " << this->currentSensorZA  << std::endl;
    ss << "current sensor XB: " << this->currentSensorXB  << std::endl;
    ss << "current sensor XA: " << this->currentSensorXA  << std::endl;
    ss << "Last ADCS result: " << this->lastADCSResult  << std::endl;

    std::string out;
    ss >> out;
    return out;
}

std::string FOSSASAT2::Messages::FullSystemInfo::ToJSON()
{

    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "\"Satellite Version\": \"FOSSASAT2\"," << std::endl;
    ss << "\"Message Name\": \"FullSystemInfo\"," << std::endl;
    ss << "\"MPPT output voltage: \"" << this->mpptOutputVoltage << "," << std::endl;
    ss << "\"MPPT output current: \"" << this->mpptOutputCurrent  << "," << std::endl;
    ss << "\"Timestamp: \"" << this->unixTimestamp  << "," << std::endl;
    ss << "\"Transmissions enabled: \"" << this->transmissionsEnabled << "," << std::endl;
    ss << "\"LowPower mode enabled: \"" << this->lowPowerModeEnabled << "," << std::endl;
    ss << "\"Currently active low power mode: \"" << this->currentlyActivePowerMode << "," << std::endl;
    ss << "\"MPPT temperature switch enabled: \"" << this->mpptTemperatureSwitchEnabled  << "," << std::endl;
    ss << "\"MPPT keep alive enabled: \"" << this->mpptKeepAliveEnabled  << "," << std::endl;
    ss << "\"Science mode active: \"" << this->scienceModeActive << "," << std::endl;
    ss << "\"Reset counter: \"" << this->resetCounter << "," << std::endl;
    ss << "\"Solar panel XA voltage: \"" << this->solarPanel_XA_Voltage  << "," << std::endl;
    ss << "\"solar panel XA current: \"" << this->solarPanel_XA_Current  << "," << std::endl;
    ss << "\"Solar panel XB voltage: \"" << this->solarPanel_XB_Voltage  << "," <<  std::endl;
    ss << "\"Solar panel XB current: \"" << this->solarPanel_XB_Current  << "," <<  std::endl;
    ss << "\"Solar panel ZA voltage: \"" << this->solarPanel_ZA_Voltage    << "," <<  std::endl;
    ss << "\"Solar panel ZA current : \"" << this->solarPanel_ZA_Current  << "," <<  std::endl;
    ss << "\"Solar panel ZB voltage: \"" << this->solarPanel_ZB_Voltage  << "," <<  std::endl;
    ss << "\"Solar panel ZB current: \"" << this->solarPanel_ZB_Current  << "," <<  std::endl;
    ss << "\"Solar panel Y voltage: \"" << this->solarPanel_Y_Voltage  << "," <<  std::endl;
    ss << "\"Solar panel Y current: \"" << this->solarPanel_Y_Current  << "," << std::endl;
    ss << "\"Y panel temperature: \"" << this->solarPanelYTemperature  << "," << std::endl;
    ss << "\"OBC board temperature: \"" << this->obcBoardTemperature    << "," <<  std::endl;
    ss << "\"Bottom board temperature : \"" << this->bottomBoardTemperature  << "," << std::endl;
    ss << "\"Battery temperature: \"" << this->batteryTemperature  << "," << std::endl;
    ss << "\"Second battery temperature: \"" << this->secondBatteryTemperature  << "," << std::endl;
    ss << "\"MCU temperature: \"" << this->mcuTemperature  << "," << std::endl;
    ss << "\"Y panel light sensor: \"" << this->yPanelLightSensor << "," << std::endl;
    ss << "\"Top board light sensor: \"" << this->topBoardLightSensor << "," << std::endl;
    ss << "\"Last X axis H-bridge fault: \"" << this->lastXAxisHBridgeFault  << "," << std::endl;
    ss << "\"Last Y axis H-bridge fault: \"" << this->lastYAxisHBridgeFault  << "," << std::endl;
    ss << "\"Last Z axis H-bridge fault: \"" << this->lastZAxisHBridgeFault  << "," << std::endl;
    ss << "\"External flash system info page CRC error counter: \"" << "," << this->flashSystemInfoPageCRCErrorCounter << std::endl;
    ss << "\"FSK window receive length: \"" << this->fskWindowReceiveLength  << "," <<  std::endl;
    ss << "\"LoRa window receive length: \"" << this->loraWindowReceiveLength  << "," <<  std::endl;
    ss << "\"Light sensor top: \"" << this->lightSensorTop  << "," << std::endl;
    ss << "\"Light sensor Y: \"" << this->lightSensorY  << "," << std::endl;
    ss << "\"Current sensor MPPT: \"" << this->currentSensorMPPT  << "," << std::endl;
    ss << "\"Current sensor Y: \"" << this->currentSensorY  << "," << std::endl;
    ss << "\"Current sensor ZB: \"" << this->currentSensorZB  << "," << std::endl;
    ss << "\"Current sensor ZA: \"" << this->currentSensorZA  << "," << std::endl;
    ss << "\"Current sensor XB: \"" << this->currentSensorXB  << "," << std::endl;
    ss << "\"Current sensor XA: \"" << this->currentSensorXA  << "," << std::endl;
    ss << "\"Last ADCS result: \"" << this->lastADCSResult  << std::endl;
    ss << "}" << std::endl;

    std::string out;
    ss >> out;
    return out;
}
