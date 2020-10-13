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

#ifndef DATAGRAM_H
#define DATAGRAM_H

#include "SatVersion.h"
#include "OperationID.h"
#include "Frame.h"

#include <vector>
#include <stdexcept>

class Datagram {
public:
    Datagram(SatVersion satVersion, std::string callsign, std::vector<uint8_t> data, bool inbound, bool encrypted = false, std::vector<uint8_t> key = std::vector<uint8_t>(), std::string password = std::string());
    int16_t GetFrameFunctionID();
    OperationID GetOperationID();
    Frame GetFrame();
    std::vector<uint8_t> Serialize();
    std::string ToString();
    std::string ToHexString();

    bool IsEncrypted();
private:
    void ExtractRadiolibStatusCode(std::vector<uint8_t> data);
    void ExtractFrame(std::string callsign, std::vector<uint8_t> data);
private:
    bool encrypted = false;

    Frame frame;
    bool frameExists = false;

    uint8_t controlByte;
    uint8_t lengthByte;
    OperationID operationId;

    int16_t radiolibStatusCode;
    bool inbound;

    std::string callsign;
    SatVersion satVersion;
    std::vector<uint8_t> key;
    std::string password;
};

#endif //DATAGRAM_H
