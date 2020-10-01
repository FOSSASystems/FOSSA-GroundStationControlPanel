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

#include "FOSSASAT2_SystemInfo.h"

FOSSASAT2::Messages::SystemInfo::SystemInfo(Frame &frame) {
	this->mpptOutputVoltage = frame.GetByteAt(0);
	this->mpptOutputVoltage *= 20;

	this->mpptOutputCurrent = frame.GetByteAt(1) | (frame.GetByteAt(2) << 8);
	this->mpptOutputCurrent *= 20;

	this->unixTimestamp = frame.GetByteAt(3);
	this->unixTimestamp |= (frame.GetByteAt(4) << 8);
	this->unixTimestamp |= (frame.GetByteAt(5) << 16);
	this->unixTimestamp |= (frame.GetByteAt(6) << 24);

	this->powerConfiguration = frame.GetByteAt(7);
	this->transmissionsEnabled = powerConfiguration & 0b00000001; // bit 0
	this->lowPowerModeEnabled = (powerConfiguration & 0b00000010) >> 1; // bit 1

	this->currentlyActivePowerModeLSB = (powerConfiguration & 0b00000100) >> 2; // bit 2
	this->currentlyActivePowerModeA = (powerConfiguration & 0b00001000) >> 3; // bit 3
	this->currentlyActivePowerModeMSB = (powerConfiguration & 0b00010000) >> 4; // bit 4
	this->currentlyActivePowerMode = currentlyActivePowerModeLSB;
	this->currentlyActivePowerMode |= (currentlyActivePowerModeA << 1);
	this->currentlyActivePowerMode |= (currentlyActivePowerModeMSB << 2);

	this->lowPowerModeActive = currentlyActivePowerMode == 0;
	this->sleepMode = currentlyActivePowerMode == 2;


	this->mpptTemperatureSwitchEnabled = (powerConfiguration & 0b00100000) >> 5; // bit 5
	this->mpptKeepAliveEnabled = (powerConfiguration  & 0b01000000) >> 6; // bit 6

	this->resetCounter = frame.GetByteAt(8) | (frame.GetByteAt(9) << 8);

	this->solarPanel_XA_Voltage = frame.GetByteAt(10);
	this->solarPanel_XB_Voltage = frame.GetByteAt(11);
	this->solarPanel_ZA_Voltage = frame.GetByteAt(12);
	this->solarPanel_ZB_Voltage = frame.GetByteAt(13);
	this->solarPanel_Y_Voltage = frame.GetByteAt(14);

	{
		int16_t t = frame.GetByteAt(15) | (frame.GetByteAt(16) << 8);
		this->batteryTemperature = t * 0.01f;
	}

	{
		int16_t t = frame.GetByteAt(17) | (frame.GetByteAt(18) << 8);
		this->obcBoardTemperature = t * 0.01f;
	}

	this->flashSystemInfoPageCRCErrorCounter = frame.GetByteAt(19);
	this->flashSystemInfoPageCRCErrorCounter |= (frame.GetByteAt(20) << 8);
	this->flashSystemInfoPageCRCErrorCounter |= (frame.GetByteAt(21) << 16);
    this->flashSystemInfoPageCRCErrorCounter |= (frame.GetByteAt(22) << 24);
}

std::string FOSSASAT2::Messages::SystemInfo::ToString()
{

    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT2" << std::endl;
    ss << "Message Name: SystemInfo" << std::endl;
    ss << "MPPT output voltage: " << this->mpptOutputVoltage << "mV" << std::endl;
    ss << "MPPT output current: " << this->mpptOutputCurrent  << "uA" << std::endl;
    ss << "Timestamp: " << this->unixTimestamp  << std::endl;
    ss << "Transmissions enabled: " << this->transmissionsEnabled << std::endl;
    ss << "LowPower mode active: " << this->lowPowerModeActive << std::endl;
    ss << "LowPower mode enabled: " << this->lowPowerModeEnabled << std::endl;
    ss << "Currently active low power mode: " << this->currentlyActivePowerMode << std::endl;
    ss << "MPPT temperature switch enabled: " << this->mpptTemperatureSwitchEnabled  << std::endl;
    ss << "MPPT keep alive enabled: " << this->mpptKeepAliveEnabled  << std::endl;
    ss << "Reset counter: " << this->resetCounter << std::endl;
    ss << "Solar panel XA voltage: " << this->solarPanel_XA_Voltage  << "mV" << std::endl;
    ss << "Solar panel XB voltage: " << this->solarPanel_XB_Voltage  << "mV" << std::endl;
    ss << "Solar panel ZA voltage: " << this->solarPanel_ZA_Voltage    << "mV" << std::endl;
    ss << "Solar panel ZB voltage: " << this->solarPanel_ZB_Voltage  << "mV" << std::endl;
    ss << "Solar panel Y voltage: " << this->solarPanel_Y_Voltage  << "mV" << std::endl;
    ss << "OBC board temperature: " << this->obcBoardTemperature    << "Deg. C." << std::endl;
    ss << "Battery temperature: " << this->batteryTemperature  << "Deg. C." << std::endl;
    ss << "External flash system info page CRC error counter: " << this->flashSystemInfoPageCRCErrorCounter << std::endl;

    std::string out;
    ss >> out;
    return out;;
}

std::string FOSSASAT2::Messages::SystemInfo::ToJSON()
{
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "\"MPPT output voltage\": \"" << this->mpptOutputVoltage << "\"" << std::endl;
    ss << "\"MPPT output current\": \"" << this->mpptOutputCurrent  << "\"" << std::endl;
    ss << "\"Timestamp\": \"" << this->unixTimestamp << "\""  << std::endl;
    ss << "\"Transmissions enabled\": \"" << this->transmissionsEnabled << "\""  << std::endl;
    ss << "\"LowPower mode active\": \"" << this->lowPowerModeActive << "\""  << std::endl;
    ss << "\"LowPower mode enabled\": \"" << this->lowPowerModeEnabled << "\""  << std::endl;
    ss << "\"Currently active low power mode\": \"" << this->currentlyActivePowerMode << "\""  << std::endl;
    ss << "\"MPPT temperature switch enabled\": \"" << this->mpptTemperatureSwitchEnabled << "\""   << std::endl;
    ss << "\"MPPT keep alive enabled\": \"" << this->mpptKeepAliveEnabled << "\""   << std::endl;
    ss << "\"Reset counter\": \"" << this->resetCounter << "\""  << std::endl;
    ss << "\"Solar panel XA voltage\": \"" << this->solarPanel_XA_Voltage  << "\"" << std::endl;
    ss << "\"Solar panel XB voltage\": \"" << this->solarPanel_XB_Voltage  << "\"" << std::endl;
    ss << "\"Solar panel ZA voltage\": \"" << this->solarPanel_ZA_Voltage    << "\"" << std::endl;
    ss << "\"Solar panel ZB voltage\": \"" << this->solarPanel_ZB_Voltage  << "\"" << std::endl;
    ss << "\"Solar panel Y voltage\": \"" << this->solarPanel_Y_Voltage  << "\"" << std::endl;
    ss << "\"OBC board temperature\": \"" << this->obcBoardTemperature    << "\"" << std::endl;
    ss << "\"Battery temperature\": \"" << this->batteryTemperature  << "\"" << std::endl;
    ss << "\"External flash system info page CRC error counter\": \"" << this->flashSystemInfoPageCRCErrorCounter << "\""  << std::endl;
    ss << "}" << std::endl;

    std::string out;
    ss >> out;
    return out;
}

uint32_t FOSSASAT2::Messages::SystemInfo::GetMpptOutputVoltage() const {
    return mpptOutputVoltage;
}

int32_t FOSSASAT2::Messages::SystemInfo::GetMpptOutputCurrent() const {
    return mpptOutputCurrent;
}

uint32_t FOSSASAT2::Messages::SystemInfo::GetUnixTimestamp() const {
    return unixTimestamp;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetPowerConfiguration() const {
    return powerConfiguration;
}

bool FOSSASAT2::Messages::SystemInfo::IsTransmissionsEnabled() const {
    return transmissionsEnabled;
}

bool FOSSASAT2::Messages::SystemInfo::IsLowPowerModeEnabled() const {
    return lowPowerModeEnabled;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetCurrentlyActivePowerModeLsb() const {
    return currentlyActivePowerModeLSB;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetCurrentlyActivePowerModeA() const {
    return currentlyActivePowerModeA;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetCurrentlyActivePowerModeMsb() const {
    return currentlyActivePowerModeMSB;
}

uint32_t FOSSASAT2::Messages::SystemInfo::GetCurrentlyActivePowerMode() const {
    return currentlyActivePowerMode;
}

bool FOSSASAT2::Messages::SystemInfo::IsLowPowerModeActive() const {
    return lowPowerModeActive;
}

bool FOSSASAT2::Messages::SystemInfo::IsSleepMode() const {
    return sleepMode;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetMpptTemperatureSwitchEnabled() const {
    return mpptTemperatureSwitchEnabled;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetMpptKeepAliveEnabled() const {
    return mpptKeepAliveEnabled;
}

uint16_t FOSSASAT2::Messages::SystemInfo::GetResetCounter() const {
    return resetCounter;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetSolarPanelXaVoltage() const {
    return solarPanel_XA_Voltage;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetSolarPanelXbVoltage() const {
    return solarPanel_XB_Voltage;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetSolarPanelZaVoltage() const {
    return solarPanel_ZA_Voltage;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetSolarPanelZbVoltage() const {
    return solarPanel_ZB_Voltage;
}

uint8_t FOSSASAT2::Messages::SystemInfo::GetSolarPanelYVoltage() const {
    return solarPanel_Y_Voltage;
}

float FOSSASAT2::Messages::SystemInfo::GetBatteryTemperature() const {
    return batteryTemperature;
}

float FOSSASAT2::Messages::SystemInfo::GetObcBoardTemperature() const {
    return obcBoardTemperature;
}

int32_t FOSSASAT2::Messages::SystemInfo::GetFlashSystemInfoPageCrcErrorCounter() const {
    return flashSystemInfoPageCRCErrorCounter;
}
