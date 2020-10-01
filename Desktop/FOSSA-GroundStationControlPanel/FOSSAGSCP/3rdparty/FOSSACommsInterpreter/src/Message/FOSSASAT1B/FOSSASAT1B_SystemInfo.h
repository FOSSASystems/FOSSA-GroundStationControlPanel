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

#ifndef FOSSASAT1B_SYSTEMINFO_H
#define FOSSASAT1B_SYSTEMINFO_H

#include "../../Frame.h"
#include "../IMessage.h"

namespace FOSSASAT1B
{

namespace Messages
{

class SystemInfo : public IMessage {
public:
	explicit SystemInfo(Frame& frame);
    virtual ~SystemInfo() = default;
    virtual std::string ToString() override;
    virtual std::string ToJSON() override;

    uint32_t GetBatteryVoltage() const;
    int32_t GetBatteryChargingCurrent() const;
    uint32_t GetBatteryChargingVoltage() const;
    uint32_t GetTimeSinceLastReset() const;
    uint8_t GetPowerConfiguration() const;
    bool GetLowPowerModeActive() const;
    bool GetLowPowerModeEnabled() const;
    bool GetMpptTemperatureSwitchEnabled() const;
    bool GetMpptKeepAliveEnabled() const;
    bool GetTransmissionsEnabled() const;
    uint16_t GetResetCounter() const;
    uint32_t GetSolarCellAVoltage() const;
    uint32_t GetSolarCellBVoltage() const;
    uint32_t GetSolarCellCVoltage() const;
    float GetBatteryTemperature() const;
    float GetObcBoardTemperature() const;
    int8_t GetMcuTemperature() const;

private:
    uint32_t batteryVoltage;
    int32_t batteryChargingCurrent;
	uint32_t batteryChargingVoltage;

	uint32_t timeSinceLastReset;

	uint8_t powerConfiguration;
	bool lowPowerModeActive;
	bool lowPowerModeEnabled;
	bool mpptTemperatureSwitchEnabled;
	bool mpptKeepAliveEnabled;
	bool transmissionsEnabled;

	uint16_t resetCounter;

	uint32_t solarCellAVoltage;
	uint32_t solarCellBVoltage;
	uint32_t solarCellCVoltage;

	float batteryTemperature;
	float obcBoardTemperature;
	int8_t mcuTemperature;
};

}

}



#endif //FOSSASAT1_SYSTEMINFO_H
