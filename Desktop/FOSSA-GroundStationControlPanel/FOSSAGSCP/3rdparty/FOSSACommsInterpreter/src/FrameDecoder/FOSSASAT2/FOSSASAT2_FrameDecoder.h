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

#ifndef FOSSASAT2_FRAMEDECODER_H
#define FOSSASAT2_FRAMEDECODER_H

#include "../../Frame.h"

#include "../../Message/FOSSASAT2/FOSSASAT2_SystemInfo.h"
#include "../../Message/FOSSASAT2/FOSSASAT2_PacketInfo.h"
#include "../../Message/FOSSASAT2/FOSSASAT2_SystemInfo.h"
#include "../../Message/FOSSASAT2/FOSSASAT2_Statistics.h"
#include "../../Message/FOSSASAT2/FOSSASAT2_FullSystemInfo.h"
#include "../../Message/FOSSASAT2/FOSSASAT2_DeploymentState.h"
#include "../../Message/FOSSASAT2/FOSSASAT2_RecordedIMU.h"
#include "../../Message/FOSSASAT2/FOSSASAT2_GPSLogState.h"
#include "../../Message/FOSSASAT2/FOSSASAT2_Acknowledge.h"

namespace FOSSASAT2
{

class FrameDecoder {
public:
    static FOSSASAT2::Messages::SystemInfo DecodeSystemInfo(Frame &frame);
    static FOSSASAT2::Messages::PacketInfo DecodePacketInfo(Frame &frame);
    static FOSSASAT2::Messages::Statistics DecodeStatistics(Frame &frame);
    static FOSSASAT2::Messages::FullSystemInfo DecodeFullSystemInfo(Frame &frame);
    static FOSSASAT2::Messages::DeploymentState DecodeDeploymentState(Frame &frame);
    static FOSSASAT2::Messages::RecordedIMU DecodeRecordedIMU(Frame &frame);
    static FOSSASAT2::Messages::GPSLogState DecodeGPSLogState(Frame &frame);
    static FOSSASAT2::Messages::Acknowledge DecodeAcknowledge(Frame &frame);
};

}


#endif //FOSSASAT2_FRAMEDECODER_H
