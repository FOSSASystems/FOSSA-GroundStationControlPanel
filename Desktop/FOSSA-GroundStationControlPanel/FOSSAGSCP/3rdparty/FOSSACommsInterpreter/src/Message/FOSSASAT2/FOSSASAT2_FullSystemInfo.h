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

#ifndef FOSSASAT2_FULLSYSTEMINFO_H
#define FOSSASAT2_FULLSYSTEMINFO_H

#include "../../Frame.h"
#include "../IMessage.h"

namespace FOSSASAT2
{

namespace Messages
{

class FullSystemInfo  : public IMessage {
public:
    explicit FullSystemInfo(Frame& frame);
    virtual ~FullSystemInfo() = default;
    virtual std::string ToString() override;
    virtual std::string ToJSON() override;
private:
    uint8_t mpptOutputVoltage;
    int16_t mpptOutputCurrent;

    uint32_t unixTimestamp;

    uint8_t powerConfiguration;
    uint8_t transmissionsEnabled;
    uint8_t lowPowerModeEnabled;

    uint32_t currentlyActivePowerMode;

    bool noLowPowerMode;
    bool sleepMode;


    uint8_t mpptTemperatureSwitchEnabled;
    uint8_t mpptKeepAliveEnabled;
    uint8_t scienceModeActive;

    uint16_t resetCounter;

    uint8_t solarPanel_XA_Voltage;
    int16_t solarPanel_XA_Current;

    uint8_t solarPanel_XB_Voltage;
    int16_t solarPanel_XB_Current;

    uint8_t solarPanel_ZA_Voltage;
    int16_t solarPanel_ZA_Current;

    uint8_t solarPanel_ZB_Voltage;
    int16_t solarPanel_ZB_Current;

    uint8_t solarPanel_Y_Voltage;
    int16_t solarPanel_Y_Current;

    int16_t solarPanelYTemperature;
    int16_t obcBoardTemperature;
    int16_t bottomBoardTemperature;
    int16_t batteryTemperature;
    int16_t secondBatteryTemperature;
    int16_t mcuTemperature;

    float yPanelLightSensor;

    float topBoardLightSensor;

    uint8_t lastXAxisHBridgeFault;
    uint8_t lastYAxisHBridgeFault;
    uint8_t lastZAxisHBridgeFault;


    int32_t flashSystemInfoPageCRCErrorCounter;

    uint8_t fskWindowReceiveLength;
    uint8_t loraWindowReceiveLength;

    uint8_t sensorStates;
    uint8_t lightSensorTop;
    uint8_t lightSensorY;
    uint8_t currentSensorMPPT;
    uint8_t currentSensorY;
    uint8_t currentSensorZB;
    uint8_t currentSensorZA;
    uint8_t currentSensorXB;
    uint8_t currentSensorXA;

    uint8_t lastADCSResult;
};

}

}





#endif //FOSSASAT2_SYSTEMINFO_H
