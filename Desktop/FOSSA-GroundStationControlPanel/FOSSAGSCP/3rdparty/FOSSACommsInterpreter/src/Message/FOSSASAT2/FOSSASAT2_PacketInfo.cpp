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

#include "FOSSASAT2_PacketInfo.h"

FOSSASAT2::Messages::PacketInfo::PacketInfo(Frame &frame)
{
    this->snr = frame.GetByteAt(0);
    this->snr *= 4;
    uint8_t rssiData = frame.GetByteAt(1);
    this->rssi *= rssiData * -2;
    this->numReceivedValidLoraFrames = frame.GetByteAt(2);
    this->numReceivedValidLoraFrames |= frame.GetByteAt(3) << 8;
    this->numReceivedInvalidLoraFrames = frame.GetByteAt(4);
    this->numReceivedInvalidLoraFrames |= frame.GetByteAt(5)<< 8;
    this->numReceivedValidFSKFrames = frame.GetByteAt(6);
    this->numReceivedValidFSKFrames |= frame.GetByteAt(7)<< 8;
    this->numReceivedInvalidFSKFrames = frame.GetByteAt(8);
    this->numReceivedInvalidFSKFrames |= frame.GetByteAt(9)<< 8;
}

std::string FOSSASAT2::Messages::PacketInfo::ToString()
{

    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT2" << std::endl;
    ss << "Message Name: PacketInfo" << std::endl;
    ss << "SNR of the last received packet: " << this->snr << " dB" << std::endl;
    ss << "RSSI of the last received packet: " << this->rssi << " dBm" << std::endl;
    ss << "Number of received valid LoRa frames: " << this->numReceivedValidLoraFrames << std::endl;
    ss << "Number of received invalid LoRa frames: " << this->numReceivedInvalidLoraFrames << std::endl;
    ss << "Number of received valid FSK frames: " << this->numReceivedValidFSKFrames << std::endl;
    ss << "Number of received invalid FSK frames: " << this->numReceivedInvalidFSKFrames << std::endl;

    std::string out;
    ss >> out;
    return out;
}

std::string FOSSASAT2::Messages::PacketInfo::ToJSON()
{
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "\"Satellite Version\": \"FOSSASAT2\"," << std::endl;
    ss << "\"Message Name\": \"PacketInfo\"," << std::endl;
    ss << "\"SNR of the last received packet\": \"" << this->snr << "\"" << std::endl;
    ss << "\"RSSI of the last received packet\": \"" << this->rssi << "\"" <<  std::endl;
    ss << "\"Number of received valid LoRa frames\": \"" << this->numReceivedValidLoraFrames << "\"" <<  std::endl;
    ss << "\"Number of received invalid LoRa frames\": \"" << this->numReceivedInvalidLoraFrames << "\"" <<  std::endl;
    ss << "\"Number of received valid FSK frames\": \"" << this->numReceivedValidFSKFrames << "\"" <<  std::endl;
    ss << "\"Number of received invalid FSK frames\": \"" << this->numReceivedInvalidFSKFrames << "\"" <<  std::endl;
    ss << "}" << std::endl;


    std::string out;
    ss >> out;
    return out;
}
