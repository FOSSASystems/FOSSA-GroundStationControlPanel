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

#include "FOSSASAT2_GPSLogState.h"

FOSSASAT2::Messages::GPSLogState::GPSLogState(Frame &frame)
{
    this->gpsLogLength = frame.GetByteAt(0);
    this->gpsLogLength |= (frame.GetByteAt(1)<< 8);
    this->gpsLogLength |= (frame.GetByteAt(2)<< 16);
    this->gpsLogLength |= (frame.GetByteAt(3)<< 24);

    this->lastNMEAEntryAddr = frame.GetByteAt(4);
    this->lastNMEAEntryAddr |= (frame.GetByteAt(5)<< 8);
    this->lastNMEAEntryAddr |= (frame.GetByteAt(6)<< 16);
    this->lastNMEAEntryAddr |= (frame.GetByteAt(7)<< 24);

    this->lastNMEAFixAddr = frame.GetByteAt(8);
    this->lastNMEAFixAddr |= (frame.GetByteAt(9)<< 8);
    this->lastNMEAFixAddr |= (frame.GetByteAt(10)<< 16);
    this->lastNMEAFixAddr |= (frame.GetByteAt(11)<< 24);
}

std::string FOSSASAT2::Messages::GPSLogState::ToString()
{

    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT2" << std::endl;
    ss << "Message Name: GPSLogState" << std::endl;
    ss << "GPS Log Length: " << this->gpsLogLength << std::endl;
    ss << "Last NMEA Entry Address: " << this->lastNMEAEntryAddr << std::endl;
    ss << "Last NMEA Fix Address: " << this->lastNMEAFixAddr << std::endl;

    std::string out;
    ss >> out;
    return out;
}

std::string FOSSASAT2::Messages::GPSLogState::ToJSON()
{
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "\"Satellite Version\": \"FOSSASAT2\"," << std::endl;
    ss << "\"Message Name\": \"GPSLogState\"," << "\"" << std::endl;
    ss << "\"GPS Log Length\": \"" << this->gpsLogLength << "\"" << std::endl;
    ss << "\"Last NMEA Entry Address\": \"" << this->lastNMEAEntryAddr << "\"" << std::endl;
    ss << "\"Last NMEA Fix Address\": \"" << this->lastNMEAFixAddr << "\"" << std::endl;
    ss << "}" << std::endl;


    std::string out;
    ss >> out;
    return out;
}

uint32_t FOSSASAT2::Messages::GPSLogState::GetGpsLogLength() const {
    return gpsLogLength;
}

uint32_t FOSSASAT2::Messages::GPSLogState::GetLastNmeaEntryAddr() const {
    return lastNMEAEntryAddr;
}

uint32_t FOSSASAT2::Messages::GPSLogState::GetLastNmeaFixAddr() const {
    return lastNMEAFixAddr;
}
