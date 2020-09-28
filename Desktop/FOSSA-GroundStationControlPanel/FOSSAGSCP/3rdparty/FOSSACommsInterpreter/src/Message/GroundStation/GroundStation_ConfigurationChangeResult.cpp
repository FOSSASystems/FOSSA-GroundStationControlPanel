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

#include "GroundStation_ConfigurationChangeResult.h"

GroundStation::Messages::ConfigurationChangeResult::ConfigurationChangeResult(Frame &frame)
{
    this->modemType = frame.GetByteAt(0);

    {
        uint32_t temp = frame.GetByteAt(1);
        temp |= (frame.GetByteAt(2) << 8);
        temp |= (frame.GetByteAt(3) << 16);
        temp |= (frame.GetByteAt(4) << 24);

        float tempFloat = 0.0f;
        memcpy(&tempFloat, &temp, 4);

        this->carrierFrequency = tempFloat;
    }


    int8_t outputPower = frame.GetByteAt(5);

    {
        uint32_t temp = frame.GetByteAt(6);
        temp |= (frame.GetByteAt(7) << 8);
        temp |= (frame.GetByteAt(8) << 16);
        temp |= (frame.GetByteAt(9) << 24);

        float tempFloat = 0.0f;
        memcpy(&tempFloat, &temp, 4);

        this->currentLimit = tempFloat;
    }

    {
        uint32_t temp = frame.GetByteAt(10);
        temp |= (frame.GetByteAt(11) << 8);
        temp |= (frame.GetByteAt(12) << 16);
        temp |= (frame.GetByteAt(13) << 24);

        float tempFloat = 0.0f;
        memcpy(&tempFloat, &temp, 4);

        this->loraBandwidth = tempFloat;
    }


    this->loraSpreadingFactor = frame.GetByteAt(14);
    this->loraCodingRate = frame.GetByteAt(15);
    this->loraPreambleLength = frame.GetByteAt(16) | (frame.GetByteAt(17) << 8);

    {
        uint32_t temp = frame.GetByteAt(18);
        temp |= (frame.GetByteAt(19) << 8);
        temp |= (frame.GetByteAt(20) << 16);
        temp |= (frame.GetByteAt(21) << 24);

        float tempFloat = 0.0f;
        memcpy(&tempFloat, &temp, 4);

        this->gfskBitRate = tempFloat;
    }


    {
        uint32_t temp = frame.GetByteAt(22);
        temp |= (frame.GetByteAt(23) << 8);
        temp |= (frame.GetByteAt(24) << 16);
        temp |= (frame.GetByteAt(25) << 24);

        float tempFloat = 0.0f;
        memcpy(&tempFloat, &temp, 4);

        this->gfskFrequencyDeviation = tempFloat;
    }

    {
        uint32_t temp = frame.GetByteAt(26);
        temp |= (frame.GetByteAt(27) << 8);
        temp |= (frame.GetByteAt(28) << 16);
        temp |= (frame.GetByteAt(29) << 24);

        float tempFloat = 0.0f;
        memcpy(&tempFloat, &temp, 4);

        this->gfskRxBandwidth = tempFloat;
    }

    this->gfskPreambleLength = frame.GetByteAt(30) | (frame.GetByteAt(31) << 8);

    this->gfskDataShapingBTProduct = frame.GetByteAt(32);
}
