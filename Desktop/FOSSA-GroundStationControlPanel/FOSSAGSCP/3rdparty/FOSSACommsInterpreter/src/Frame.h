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

#ifndef FRAME_H
#define FRAME_H

#include "SatVersion.h"

#include <FOSSA-Comms.h>

#include <vector>
#include <string>

class Frame {
public:
    Frame();
    ~Frame();
    Frame(SatVersion satVersion, std::string callsign, std::vector<uint8_t> data);

    uint8_t GetByteAt(uint32_t index);
    std::vector<uint8_t> Serialize();
    std::string ToHexString();

    int16_t GetFunctionID();
private:
    void ExtractOptionalData(std::string callsign, std::vector<uint8_t> &data, int16_t optionalDataLength);
private:
    SatVersion satVersion;
    int16_t functionId;
    std::vector<uint8_t> optionalData;
    bool hasOptionalData;
};


#endif //FRAME_H
