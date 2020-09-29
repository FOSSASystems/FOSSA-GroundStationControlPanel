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

#ifndef FOSSASAT2_GPSLOGSTATE_H
#define FOSSASAT2_GPSLOGSTATE_H

#include "../../Frame.h"
#include "../IMessage.h"

namespace FOSSASAT2
{

namespace Messages
{

class GPSLogState : public IMessage  {
public:
    explicit GPSLogState(Frame& frame);
    virtual ~GPSLogState() = default;
    virtual std::string ToString() override;
    virtual std::string ToJSON() override;
private:
    uint32_t gpsLogLength;

    uint32_t lastNMEAEntryAddr;

    uint32_t lastNMEAFixAddr;
};

}

}




#endif //FOSSASAT2_SYSTEMINFO_H
