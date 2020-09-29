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

#ifndef GROUNDSTATION_DATAGRAMDECODER_H
#define GROUNDSTATION_DATAGRAMDECODER_H

#include "../../Frame.h"

#include <Datagram.h>

namespace GroundStation
{

class DatagramEncoder {
private:
    static Datagram GroundStation::DatagramEncoder::Encode(OperationID operationId, std::vector<uint8_t> payloadData);
public:
    static Datagram Handshake();
    static Datagram ConfigurationChange(uint8_t modemTypeFlag,
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
                                                    uint8_t gfskDataShapingBTProduct);
    static Datagram CarrierFrequencyChange(float newCarrierFrequency);
};

}



#endif //GROUNDSTATION_DATAGRAMDECODER_H
