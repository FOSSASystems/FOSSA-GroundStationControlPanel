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

#include "FOSSASAT2_FrameDecoder.h"

FOSSASAT2::Messages::SystemInfo FOSSASAT2::FrameDecoder::DecodeSystemInfo(Frame &frame) {
    return FOSSASAT2::Messages::SystemInfo(frame);
}

FOSSASAT2::Messages::PacketInfo FOSSASAT2::FrameDecoder::DecodePacketInfo(Frame &frame)
{
    return FOSSASAT2::Messages::PacketInfo(frame);
}

FOSSASAT2::Messages::Statistics FOSSASAT2::FrameDecoder::DecodeStatistics(Frame &frame)
{
    return FOSSASAT2::Messages::Statistics(frame);
}

FOSSASAT2::Messages::FullSystemInfo FOSSASAT2::FrameDecoder::DecodeFullSystemInfo(Frame &frame)
{
    return FOSSASAT2::Messages::FullSystemInfo(frame);
}

FOSSASAT2::Messages::DeploymentState FOSSASAT2::FrameDecoder::DecodeDeploymentState(Frame &frame)
{
    return FOSSASAT2::Messages::DeploymentState(frame);
}

FOSSASAT2::Messages::RecordedIMU FOSSASAT2::FrameDecoder::DecodeRecordedIMU(Frame &frame)
{
    return FOSSASAT2::Messages::RecordedIMU(frame);
}

FOSSASAT2::Messages::GPSLogState FOSSASAT2::FrameDecoder::DecodeGPSLogState(Frame &frame)
{
    return FOSSASAT2::Messages::GPSLogState(frame);
}

FOSSASAT2::Messages::Acknowledge FOSSASAT2::FrameDecoder::DecodeAcknowledge(Frame &frame)
{
    return FOSSASAT2::Messages::Acknowledge(frame);
}
