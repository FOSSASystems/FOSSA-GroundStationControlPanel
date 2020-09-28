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

#ifndef FOSSASAT1B_DATAGRAMDECODER_H
#define FOSSASAT1B_DATAGRAMDECODER_H

#include "../../Frame.h"

#include <Datagram.h>
#include <Ephemerides.h>
#include <SleepInterval.h>

namespace FOSSASAT1B
{

class DatagramEncoder {
private:
    static std::string callsign;
    static std::string password;
    static std::vector<uint8_t> key;

    static Datagram Encode(OperationID operationId, int16_t functionId, uint8_t optionalDataLength, char* optionalData);
public:
    static void SetCallsign(std::string callsign);
    static void SetPassword(std::string password);
    static void SetKey(std::vector<uint8_t> key);

    /**--------------------------------------------------------
     *
     * @name Public unencrypted uplink commands.
     *
     * @{
     *
     --------------------------------------------------------*/
    static Datagram Ping();
    static Datagram Retransmit(uint32_t senderId, char *message);
    static Datagram Retransmit_Custom(uint8_t bandwidth,
                                                         uint8_t spreadingFactor,
                                                         uint8_t codingRate,
                                                         uint16_t preambleSymbolLength,
                                                         uint8_t crcEnabled,
                                                         int8_t outputPower,
                                                         uint32_t senderId,
                                                         char *message);
    static Datagram Transmit_System_Info();
    static Datagram Get_Packet_Info();
    static Datagram Get_Statistics(char flags);
    static Datagram Get_Full_System_Info();
    static Datagram Store_And_Forward_Add(uint32_t messageId, char *message);
    static Datagram Store_And_Forward_Request(uint32_t messageId);
    static Datagram Request_Public_Picture(uint8_t pictureSlot,
                                                              uint16_t picturePacketId); // only 80-100 slot numbers.

    /**--------------------------------------------------------
     *
     * @}
     *
     --------------------------------------------------------*/




    /**--------------------------------------------------------
     *
     * @name Private encrypted uplink commands.
     *
     * @{
     *
     --------------------------------------------------------*/
    static Datagram Deploy();
    static Datagram Restart();
    static Datagram Wipe_EEPROM();
    static Datagram Set_Transmit_Enable(char transmitEnabled);
    static Datagram Set_Callsign(char *callsign);
    static Datagram Set_SF_Mode(uint8_t spreadingFactor);
    static Datagram Set_MPPT_Mode(uint8_t temperatureSwitchEnabled, uint8_t keepAliveEnabled);
    static Datagram Set_Low_Power_Mode_Enable(uint8_t lowPowerModeEnabled);
    static Datagram Set_Receive_Windows(uint8_t fskReceiveWindowLength, uint8_t loraReceiveWindowLength);
    static Datagram Record_Solar_Cells(char numSamples, uint16_t samplingPeriod);
    static Datagram Camera_Capture(char pictureSlot,
                                                      char lightMode,
                                                      char pictureSize,
                                                      char brightness,
                                                      char saturation,
                                                      char specialFilter,
                                                      char contrast);
    static Datagram Set_Power_Limits(int16_t deploymentVoltageLimit,
                                                        int16_t heaterVoltageLimit,
                                                        int16_t cwBeepVoltageLimit,
                                                        int16_t lowPowerVoltageLimit,
                                                        float heaterTemperatureLimit,
                                                        float mpptSwitchtemperatureLimit,
                                                        uint8_t heaterDutyCycle);
    static Datagram Set_RTC(uint8_t year,
                                               uint8_t month,
                                               uint8_t day,
                                               uint8_t dayOfWeek,
                                               uint8_t hours,
                                               uint8_t minutes,
                                               uint8_t seconds); // year is offset from 2000.
    static Datagram Record_IMU(uint16_t sampleNumber, uint16_t samplingPeriod, uint8_t flags);
    static Datagram Run_Manual_ACS(int8_t xHighBridgeMag,
                                                      int8_t xLowBridgeMag,
                                                      int8_t yHighBridgeMag,
                                                      int8_t yLowBrightMag,
                                                      int8_t zHBridgeHighMag,
                                                      int8_t zBridgeLowMag,
                                                      uint32_t xPulseLength,
                                                      uint32_t yPulseLength,
                                                      uint32_t zPulseLength,
                                                      uint32_t maneuverDuration,
                                                      uint8_t hbridgefaultFlags);
    static Datagram Log_GPS(uint32_t gpsLoggingDuration, uint32_t gpsLoggingStartOffset);
    static Datagram Get_GPS_Log(uint8_t newestEntriesFirst,
                                                   uint16_t gpsLogOffsetAsNMEASeq,
                                                   uint16_t NMEASentencesToDownlink); // NMEA sentences set to 0 means entire log.
    static Datagram Get_Flash_Contents(uint32_t flashAddress, uint8_t numBytes);
    static Datagram Get_Picture_Length(uint8_t pictureSlot);
    static Datagram Get_Picture_Burst(char pictureSlot, uint16_t pictureId, char fullOrScandata);
    static Datagram Route(char *fcpFrame);
    static Datagram Set_Flash_Contents(uint32_t flashAddress, char *data); // data MUST include a null terminator
    static Datagram Set_TLE(char *tle); // MUST include a null terminator.
    static Datagram Get_GPS_Log_State();
    static Datagram Run_GPS_Command(char *skyTraqBinaryProtocolMessage);
    static Datagram Set_Sleep_Intervals(std::vector<SleepInterval> sleepIntervals);
    static Datagram Abort();
    static Datagram Maneuver(uint8_t controlFlags, uint32_t maneuverLength);
    static Datagram Set_ADCS_Parameters(float maximumPulseIntensity,
                                                           float maximumPulseLength,
                                                           float detumblingAngularVelocity,
                                                           float minimumIntertialMoment,
                                                           float pulseAmplitude,
                                                           float calculationTolerance,
                                                           float activeControlEulerAngleChangeTolerance,
                                                           float activeControlAngularVelocityChangeTolerance,
                                                           float eclipseThreshold,
                                                           float rotationMatrixWeightRatio,
                                                           float rotationVerificationTriggerLevel,
                                                           float kalmanFilterDisturbanceCovariance,
                                                           float kalmanFilterNoiseCovariance,
                                                           uint32_t adcsUpdateTimeStepPeriod,
                                                           uint32_t hbridgeTimerUpdatePeriod,
                                                           int8_t hbridgeValueHighOutput,
                                                           int8_t hbridgeValueLowOutput,
                                                           uint8_t numControllers);
    static Datagram Erase_Flash(uint32_t sectorAddress);
    static Datagram Set_ADCS_Controller(char controllerId, float controllerMatrix[3][6]);
    static Datagram Set_ADCS_Ephemerides(uint16_t chunkId, std::vector<Ephemerides> ephemeridesDataQueue);
    static Datagram Detumble(uint8_t flags, uint32_t length);
    static Datagram Set_IMU_Offset(float xAxisGyroOffset,
                                                      float yAxisGyroOffset,
                                                      float zAxisGyroOffset,
                                                      float xAxisAcceleOffset,
                                                      float yAxisAcceleOffset,
                                                      float zAxisAcceleOffset,
                                                      float xAxisMagneticOffset,
                                                      float yAxisMagneticOffset,
                                                      float zAxisMagneticOffset);
    static Datagram Set_IMU_Calibration(float transformationMatrix[9], float biasVector[3]);

    /**--------------------------------------------------------
     *
     * @}
     *
     --------------------------------------------------------*/
};

}



#endif //FOSSASAT1B_DATAGRAMDECODER_H
