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

#ifndef FOSSASAT2_PACKETINFO_H
#define FOSSASAT2_PACKETINFO_H

#include "../../Frame.h"
#include "../IMessage.h"

namespace FOSSASAT2
{

namespace Messages
{

class PacketInfo : public IMessage  {
public:
    explicit PacketInfo(Frame& frame);
    virtual ~PacketInfo() = default;
    virtual std::string ToString() override;
    virtual std::string ToJSON() override;

    int32_t GetSnr() const;

    int32_t GetRssi() const;

    uint16_t GetNumReceivedValidLoraFrames() const;

    uint16_t GetNumReceivedInvalidLoraFrames() const;

    uint16_t GetNumReceivedValidFskFrames() const;

    uint16_t GetNumReceivedInvalidFskFrames() const;

private:
    int32_t snr;
    int32_t rssi;
    uint16_t numReceivedValidLoraFrames;
    uint16_t numReceivedInvalidLoraFrames;
    uint16_t numReceivedValidFSKFrames;
    uint16_t numReceivedInvalidFSKFrames;
};

}

}




#endif //FOSSASAT2_SYSTEMINFO_H
