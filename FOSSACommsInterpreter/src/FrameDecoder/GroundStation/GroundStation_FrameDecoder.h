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

#ifndef GROUNDSTATION_FRAMEDECODER_H
#define GROUNDSTATION_FRAMEDECODER_H

#include "../../Frame.h"

#include "../../Message/GroundStation/GroundStation_CarrierChangeResult.h"
#include "../../Message/GroundStation/GroundStation_ConfigurationChangeResult.h"
#include "../../Message/GroundStation/GroundStation_HandshakeResult.h"

namespace GroundStation
{

class FrameDecoder {
public:
    static GroundStation::Messages::HandshakeResult DecodeHandshakeResult(Frame &frame);
    static GroundStation::Messages::CarrierChangeResult DecodeCarrierChangeResult(Frame &frame);
    static GroundStation::Messages::ConfigurationChangeResult DecodeConfigurationChangeResult(Frame &frame);
};

}


#endif //FOSSASAT2_FRAMEDECODER_H
