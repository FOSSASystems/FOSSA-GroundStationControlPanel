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

    uint32_t GetMpptOutputVoltage() const;

    int32_t GetMpptOutputCurrent() const;

    uint32_t GetUnixTimestamp() const;

    uint8_t GetPowerConfiguration() const;

    uint8_t GetTransmissionsEnabled() const;

    uint8_t GetLowPowerModeEnabled() const;

    uint32_t GetCurrentlyActivePowerMode() const;

    bool IsNoLowPowerMode() const;

    bool IsSleepMode() const;

    bool IsMpptTemperatureSwitchEnabled() const;

    bool IsMpptKeepAliveEnabled() const;

    bool IsScienceModeActive() const;

    uint16_t GetResetCounter() const;

    uint32_t GetSolarPanelXaVoltage() const;

    int32_t GetSolarPanelXaCurrent() const;

    uint32_t GetSolarPanelXbVoltage() const;

    int32_t GetSolarPanelXbCurrent() const;

    uint32_t GetSolarPanelZaVoltage() const;

    int32_t GetSolarPanelZaCurrent() const;

    uint32_t GetSolarPanelZbVoltage() const;

    int32_t GetSolarPanelZbCurrent() const;

    uint32_t GetSolarPanelYVoltage() const;

    int32_t GetSolarPanelYCurrent() const;

    float GetSolarPanelYTemperature() const;

    float GetObcBoardTemperature() const;

    float GetBottomBoardTemperature() const;

    float GetBatteryTemperature() const;

    float GetSecondBatteryTemperature() const;

    float GetMcuTemperature() const;

    float GetYPanelLightSensor() const;

    float GetTopBoardLightSensor() const;

    uint8_t GetLastXAxisHBridgeFault() const;

    uint8_t GetLastYAxisHBridgeFault() const;

    uint8_t GetLastZAxisHBridgeFault() const;

    int32_t GetFlashSystemInfoPageCrcErrorCounter() const;

    uint8_t GetFskWindowReceiveLength() const;

    uint8_t GetLoraWindowReceiveLength() const;

    bool IsSensorStates() const;

    bool IsLightSensorTop() const;

    bool IsLightSensorY() const;

    bool IsCurrentSensorMppt() const;

    bool IsCurrentSensorY() const;

    bool IsCurrentSensorZb() const;

    bool IsCurrentSensorZa() const;

    bool IsCurrentSensorXb() const;

    bool IsCurrentSensorXa() const;

    uint8_t GetLastAdcsResult() const;

private:
    uint32_t mpptOutputVoltage;
    int32_t mpptOutputCurrent;

    uint32_t unixTimestamp;

    uint8_t powerConfiguration;
    uint8_t transmissionsEnabled;
    uint8_t lowPowerModeEnabled;

    uint32_t currentlyActivePowerMode;

    bool noLowPowerMode;
    bool sleepMode;


    bool mpptTemperatureSwitchEnabled;
    bool mpptKeepAliveEnabled;
    bool scienceModeActive;

    uint16_t resetCounter;

    uint32_t solarPanel_XA_Voltage;
    int32_t solarPanel_XA_Current;

    uint32_t solarPanel_XB_Voltage;
    int32_t solarPanel_XB_Current;

    uint32_t solarPanel_ZA_Voltage;
    int32_t solarPanel_ZA_Current;

    uint32_t solarPanel_ZB_Voltage;
    int32_t solarPanel_ZB_Current;

    uint32_t solarPanel_Y_Voltage;
    int32_t solarPanel_Y_Current;

    float solarPanelYTemperature;
    float obcBoardTemperature;
    float bottomBoardTemperature;
    float batteryTemperature;
    float secondBatteryTemperature;
    float mcuTemperature;

    float yPanelLightSensor;
    float topBoardLightSensor;

    uint8_t lastXAxisHBridgeFault;
    uint8_t lastYAxisHBridgeFault;
    uint8_t lastZAxisHBridgeFault;


    int32_t flashSystemInfoPageCRCErrorCounter;

    uint8_t fskWindowReceiveLength;
    uint8_t loraWindowReceiveLength;

    bool sensorStates;
    bool lightSensorTop;
    bool lightSensorY;
    bool currentSensorMPPT;
    bool currentSensorY;
    bool currentSensorZB;
    bool currentSensorZA;
    bool currentSensorXB;
    bool currentSensorXA;

    uint8_t lastADCSResult;
};

}

}





#endif //FOSSASAT2_SYSTEMINFO_H
