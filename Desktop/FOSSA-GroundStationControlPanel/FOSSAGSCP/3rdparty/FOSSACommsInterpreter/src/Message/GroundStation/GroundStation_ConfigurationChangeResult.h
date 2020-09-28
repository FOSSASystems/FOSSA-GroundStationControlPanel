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

#ifndef GroundStation_CONFIG_CHANGE_H
#define GroundStation_CONFIG_CHANGE_H

#include "../../Frame.h"

namespace GroundStation
{

namespace Messages
{

class ConfigurationChangeResult {
public:
    explicit ConfigurationChangeResult(Frame& frame);
private:
    uint8_t modemType;

    float carrierFrequency;

    int8_t outputPower;
    float currentLimit;

    float loraBandwidth;
    uint8_t loraSpreadingFactor;
    uint8_t loraCodingRate;
    uint16_t loraPreambleLength;
    float gfskBitRate;
    float gfskFrequencyDeviation;
    float gfskRxBandwidth;
    uint16_t gfskPreambleLength;
    int8_t gfskDataShapingBTProduct;
};

}

}




#endif //FOSSASAT2_SYSTEMINFO_H