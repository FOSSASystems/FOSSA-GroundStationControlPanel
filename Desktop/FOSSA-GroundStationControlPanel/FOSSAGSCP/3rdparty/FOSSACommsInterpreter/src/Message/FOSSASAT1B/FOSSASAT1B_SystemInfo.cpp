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

#include "FOSSASAT1B_SystemInfo.h"

#include <sstream>

FOSSASAT1B::Messages::SystemInfo::SystemInfo(Frame &frame) {
	this->batteryVoltage = frame.GetByteAt(0);
	this->batteryVoltage *= 20;

	this->batteryChargingCurrent = frame.GetByteAt(1) | (frame.GetByteAt(2) << 8);
	this->batteryChargingCurrent *= 10;

	this->batteryChargingVoltage = frame.GetByteAt(3);
	this->batteryChargingVoltage *= 20;

	this->timeSinceLastReset = frame.GetByteAt(4);
	timeSinceLastReset |= (frame.GetByteAt(5) << 8);
	timeSinceLastReset |= (frame.GetByteAt(6) << 16);
	timeSinceLastReset |= (frame.GetByteAt(7) << 24);

	this->powerConfiguration = frame.GetByteAt(8);
	this->lowPowerModeActive = powerConfiguration & 0b00000001;
	this->lowPowerModeEnabled = (powerConfiguration & 0b00000010) >> 1;
	this->mpptTemperatureSwitchEnabled = (powerConfiguration & 0b00000100) >> 2;
	this->mpptKeepAliveEnabled = (powerConfiguration & 0b00001000) >> 3;
	this->transmissionsEnabled = (powerConfiguration & 0b00010000) >> 4;

	this->resetCounter = frame.GetByteAt(9) | (frame.GetByteAt(10) << 8);

	this->solarCellAVoltage = frame.GetByteAt(11);
	this->solarCellAVoltage *= 20;

	this->solarCellBVoltage = frame.GetByteAt(12);
	this->solarCellBVoltage *= 20;

	this->solarCellCVoltage = frame.GetByteAt(13);
	this->solarCellCVoltage *= 20;

	{
		int16_t t = frame.GetByteAt(14) | (frame.GetByteAt(15) << 8);
		this->batteryTemperature = t * 0.01f;
	}

	{
		int16_t t = frame.GetByteAt(16) | (frame.GetByteAt(17) << 8);
		this->obcBoardTemperature = t * 0.01f;
	}

    this->mcuTemperature = frame.GetByteAt(18);
}

std::string FOSSASAT1B::Messages::SystemInfo::ToString()
{
    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT1B" << std::endl;
    ss << "Message Name: SystemInfo" << std::endl;
    ss << "Battery voltage: " << this->batteryVoltage << " mV" << std::endl;
    ss << "Battery charging current: " << this->batteryChargingCurrent << " uA" << std::endl;
    ss << "Battery charging voltage: " << this->batteryChargingVoltage << std::endl;
    ss << "Time since last reset:  " << this->timeSinceLastReset << " seconds" << std::endl;
    ss << "LowPower mode active: " << this->lowPowerModeActive << std::endl;
    ss << "LowPower mode enabled: " << this->lowPowerModeEnabled << std::endl;
    ss << "MPPT temperature switch enabled: " << this->mpptTemperatureSwitchEnabled << std::endl;
    ss << "MPPT keep alive enabled: " << this->mpptKeepAliveEnabled << std::endl;
    ss << "Transmissions enabled: " << this->transmissionsEnabled << std::endl;
    ss << "Reset counter: " << this->resetCounter << std::endl;
    ss << "Solar panel A voltage: " << this->solarCellAVoltage << " mV" << std::endl;
    ss << "Solar panel B voltage: " << this->solarCellBVoltage << " mV" << std::endl;
    ss << "Solar panel C voltage: " << this->solarCellCVoltage << " mV" << std::endl;
    ss << "Battery temperature: " << this->batteryTemperature << " deg. C." << std::endl;
    ss << "OBC board temperature: " << this->obcBoardTemperature << " deg. C." << std::endl;
    ss << "MCU temperature: " << this->mcuTemperature << " deg. C" << std::endl;

    std::string out;
    ss >> out;
    return out;
}

std::string FOSSASAT1B::Messages::SystemInfo::ToJSON()
{
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "\"Satellite Version\": \"FOSSASAT1B\"," << std::endl;
    ss << "\"Message Name\": \"SystemInfo\"," << std::endl;
    ss << "\"Battery voltage\": " << this->batteryVoltage << "," << std::endl;
    ss << "\"Battery charging current\": " << this->batteryChargingCurrent << "," << std::endl;
    ss << "\"Battery charging voltage\": " << this->batteryChargingVoltage << "," << std::endl;
    ss << "\"Time since last reset\":  " << this->timeSinceLastReset << "," << std::endl;
    ss << "\"LowPower mode active\": " << this->lowPowerModeActive << "," << std::endl;
    ss << "\"LowPower mode enabled\": " << this->lowPowerModeEnabled << "," << std::endl;
    ss << "\"MPPT temperature switch enabled\": " << this->mpptTemperatureSwitchEnabled << "," << std::endl;
    ss << "\"MPPT keep alive enabled\": " << this->mpptKeepAliveEnabled << "," << std::endl;
    ss << "\"Transmissions enabled\": " << this->transmissionsEnabled << "," << std::endl;
    ss << "\"Reset counter\": " << this->resetCounter << "," << std::endl;
    ss << "\"Solar panel A voltage\": " << this->solarCellAVoltage << "," << std::endl;
    ss << "\"Solar panel B voltage\": " << this->solarCellBVoltage << "," << std::endl;
    ss << "\"Solar panel C voltage\": " << this->solarCellCVoltage << "," << std::endl;
    ss << "\"Battery temperature\": " << this->batteryTemperature << "," << std::endl;
    ss << "\"OBC board temperature\": " << this->obcBoardTemperature << "," << std::endl;
    ss << "\"MCU temperature\": " << this->mcuTemperature << std::endl;
    ss << "}" << std::endl;

    std::string out;
    ss >> out;
    return out;
}
