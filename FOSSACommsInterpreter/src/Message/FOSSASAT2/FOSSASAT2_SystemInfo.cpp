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



uint32_t FOSSASAT2::Messages::SystemInfo::getMpptOutputVoltage() const {
	return mpptOutputVoltage;
}
int32_t FOSSASAT2::Messages::SystemInfo::getMpptOutputCurrent() const {
	return mpptOutputCurrent;
}
uint32_t FOSSASAT2::Messages::SystemInfo::getUnixTimestamp() const {
	return unixTimestamp;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getPowerConfiguration() const {
	return powerConfiguration;
}
bool FOSSASAT2::Messages::SystemInfo::isTransmissionsEnabled() const {
	return transmissionsEnabled;
}
bool FOSSASAT2::Messages::SystemInfo::isLowPowerModeEnabled() const {
	return lowPowerModeEnabled;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getCurrentlyActivePowerModeLSB() const {
	return currentlyActivePowerModeLSB;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getCurrentlyActivePowerModeA() const {
	return currentlyActivePowerModeA;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getCurrentlyActivePowerModeMSB() const {
	return currentlyActivePowerModeMSB;
}
uint32_t FOSSASAT2::Messages::SystemInfo::getCurrentlyActivePowerMode() const {
	return currentlyActivePowerMode;
}
bool FOSSASAT2::Messages::SystemInfo::isLowPowerModeActive() const {
	return lowPowerModeActive;
}
bool FOSSASAT2::Messages::SystemInfo::isSleepMode() const {
	return sleepMode;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getMpptTemperatureSwitchEnabled() const {
	return mpptTemperatureSwitchEnabled;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getMpptKeepAliveEnabled() const {
	return mpptKeepAliveEnabled;
}
uint16_t FOSSASAT2::Messages::SystemInfo::getResetCounter() const {
	return resetCounter;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getSolarPanel_XA_Voltage() const {
	return solarPanel_XA_Voltage;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getSolarPanel_XB_Voltage() const {
	return solarPanel_XB_Voltage;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getSolarPanel_ZA_Voltage() const {
	return solarPanel_ZA_Voltage;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getSolarPanel_ZB_Voltage() const {
	return solarPanel_ZB_Voltage;
}
uint8_t FOSSASAT2::Messages::SystemInfo::getSolarPanel_Y_Voltage() const {
	return solarPanel_Y_Voltage;
}
float FOSSASAT2::Messages::SystemInfo::getBatteryTemperature() const {
	return batteryTemperature;
}
float FOSSASAT2::Messages::SystemInfo::getObcBoardTemperature() const {
	return obcBoardTemperature;
}
int32_t FOSSASAT2::Messages::SystemInfo::getFlashSystemInfoPageCRCErrorCounter() const {
	return flashSystemInfoPageCRCErrorCounter;
}
