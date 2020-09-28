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

#ifndef FOSSASAT2_SYSTEMINFO_H
#define FOSSASAT2_SYSTEMINFO_H

#include "../../Frame.h"

namespace FOSSASAT2
{

namespace Messages
{

class SystemInfo {
public:
	explicit SystemInfo(Frame& frame);

	uint32_t getMpptOutputVoltage() const;
	int32_t getMpptOutputCurrent() const;
	uint32_t getUnixTimestamp() const;
	uint8_t getPowerConfiguration() const;
	bool isTransmissionsEnabled() const;
	bool isLowPowerModeEnabled() const;
	uint8_t getCurrentlyActivePowerModeLSB() const;
	uint8_t getCurrentlyActivePowerModeA() const;
	uint8_t getCurrentlyActivePowerModeMSB() const;
	uint32_t getCurrentlyActivePowerMode() const;
	bool isLowPowerModeActive() const;
	bool isSleepMode() const;
	uint8_t getMpptTemperatureSwitchEnabled() const;
	uint8_t getMpptKeepAliveEnabled() const;
	uint16_t getResetCounter() const;
	uint8_t getSolarPanel_XA_Voltage() const;
	uint8_t getSolarPanel_XB_Voltage() const;
	uint8_t getSolarPanel_ZA_Voltage() const;
	uint8_t getSolarPanel_ZB_Voltage() const;
	uint8_t getSolarPanel_Y_Voltage() const;
	float getBatteryTemperature() const;
	float getObcBoardTemperature() const;
	int32_t getFlashSystemInfoPageCRCErrorCounter() const;
private:
	uint32_t mpptOutputVoltage;
	int32_t mpptOutputCurrent;

	uint32_t unixTimestamp;

	uint8_t powerConfiguration;
	bool transmissionsEnabled;
	bool lowPowerModeEnabled;

	uint8_t currentlyActivePowerModeLSB;
	uint8_t currentlyActivePowerModeA;
	uint8_t currentlyActivePowerModeMSB;
	uint32_t currentlyActivePowerMode;

	bool lowPowerModeActive;
	bool sleepMode;


	uint8_t mpptTemperatureSwitchEnabled;
	uint8_t mpptKeepAliveEnabled;

	uint16_t resetCounter;

	uint8_t solarPanel_XA_Voltage;
	uint8_t solarPanel_XB_Voltage;
	uint8_t solarPanel_ZA_Voltage;
	uint8_t solarPanel_ZB_Voltage;
	uint8_t solarPanel_Y_Voltage;

	float batteryTemperature;
	float obcBoardTemperature;

	int32_t flashSystemInfoPageCRCErrorCounter;
};

}

}




#endif //FOSSASAT2_SYSTEMINFO_H
