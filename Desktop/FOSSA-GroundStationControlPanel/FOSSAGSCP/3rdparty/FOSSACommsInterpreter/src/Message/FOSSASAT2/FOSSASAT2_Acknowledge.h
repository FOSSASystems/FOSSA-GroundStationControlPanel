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

#ifndef FOSSASAT2_ACKNOWLEDGE_H
#define FOSSASAT2_ACKNOWLEDGE_H

#include "../../Frame.h"
#include "../IMessage.h"

namespace FOSSASAT2
{

namespace Messages
{

class Acknowledge : public IMessage {
public:
    explicit Acknowledge(Frame& frame);
    virtual ~Acknowledge() = default;

    virtual std::string ToString() override;
    virtual std::string ToJSON() override;

    uint8_t GetFunctionID();

    bool CommandSuccess();
    bool CallsignMismatch();
    bool RXFailed();
    bool UnreadableFunctionID();
    bool DecryptionFailed();
    bool OptDataLengthFailed();
    bool UnknownFunctionID();
private:
    uint8_t functionID;
    bool success;
    bool callsignMismatch;
    bool rxFailed;
    bool unableToReadFunctionID;
    bool decryptionFailed;
    bool optionalDataLengthFailed;
    bool unknownFunctionID;
};

}

}





#endif //FOSSASAT2_ACKNOWLEDGE_H
