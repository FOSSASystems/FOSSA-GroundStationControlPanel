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

#include "GroundStation_DatagramEncoder.h"

#include <DirectionBits.h>

Datagram GroundStation::DatagramEncoder::Encode(OperationID operationId, std::vector<uint8_t> payloadData)
{
    char directionBit = FCPI_DIR_TO_GROUND_STATION;
    char controlByte = directionBit | operationId;


    std::vector<uint8_t> datagramData;
    datagramData.push_back(controlByte);
    datagramData.push_back(payloadData.size());
    datagramData.insert(datagramData.end(), payloadData.begin(), payloadData.end());

    return Datagram(SatVersion::V_FOSSASAT1B, "NA", datagramData, false);
}

Datagram GroundStation::DatagramEncoder::Handshake()
{
    std::vector<uint8_t> msgData;
    msgData.push_back(FCPI_DIR_FROM_GROUND_STATION);
    msgData.push_back(OperationID::HANDSHAKE);

    return Datagram(SatVersion::V_NA, "NA", msgData, false);
}

Datagram GroundStation::DatagramEncoder::ConfigurationChange(uint8_t modemTypeFlag,
                                                             float carrierFrequency,
                                                             uint8_t outputPowerDBM,
                                                             float currentLimit,
                                                             float loraBandwidth,
                                                             uint8_t loraSpreadingFactor,
                                                             uint8_t loraCodingRate,
                                                             int16_t loraPreambleLength,
                                                             float gfskBitRate,
                                                             float gfskFrequencyDeviation,
                                                             float gfskRxBandwidth,
                                                             int16_t gfskPreambleLength,
                                                             uint8_t gfskDataShapingBTProduct)
{

    std::vector<uint8_t> data;
    data.push_back(modemTypeFlag);

    {
        char *tempArray = (char *) (&carrierFrequency);
        data.push_back(tempArray[0]);
        data.push_back(tempArray[1]);
        data.push_back(tempArray[2]);
        data.push_back(tempArray[3]);
    }

    data.push_back(outputPowerDBM);

    {
        char *tempArray = (char *) (&currentLimit);
        data.push_back(tempArray[0]);
        data.push_back(tempArray[1]);
        data.push_back(tempArray[2]);
        data.push_back(tempArray[3]);
    }

    {
        char *tempArray = (char *) (&loraBandwidth);
        data.push_back(tempArray[0]);
        data.push_back(tempArray[1]);
        data.push_back(tempArray[2]);
        data.push_back(tempArray[3]);
    }

    data.push_back(loraSpreadingFactor);

    data.push_back(loraCodingRate);

    data.push_back(loraPreambleLength);
    data.push_back(loraPreambleLength >> 8);


    {
        char *tempArray = (char *) (&gfskBitRate);
        data.push_back(tempArray[0]);
        data.push_back(tempArray[1]);
        data.push_back(tempArray[2]);
        data.push_back(tempArray[3]);
    }


    {
        char *tempArray = (char *) (&gfskFrequencyDeviation);
        data.push_back(tempArray[0]);
        data.push_back(tempArray[1]);
        data.push_back(tempArray[2]);
        data.push_back(tempArray[3]);
    }

    {
        char *tempArray = (char *) (&gfskRxBandwidth);
        data.push_back(tempArray[0]);
        data.push_back(tempArray[1]);
        data.push_back(tempArray[2]);
        data.push_back(tempArray[3]);
    }

    data.push_back(gfskPreambleLength);
    data.push_back(gfskPreambleLength >> 8);


    data.push_back(gfskDataShapingBTProduct);

    return GroundStation::DatagramEncoder::Encode(OperationID::CONFIG, data);

}

Datagram GroundStation::DatagramEncoder::CarrierFrequencyChange(float newCarrierFrequency)
{
    std::vector<uint8_t> data;

    {
        char *tempArray = (char *) (&newCarrierFrequency);
        data.push_back(tempArray[0]);
        data.push_back(tempArray[1]);
        data.push_back(tempArray[2]);
        data.push_back(tempArray[3]);
    }

    return GroundStation::DatagramEncoder::Encode(OperationID::CARRIER, data);
}
