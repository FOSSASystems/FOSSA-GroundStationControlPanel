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

#include "FOSSASAT2_Acknowledge.h"

FOSSASAT2::Messages::Acknowledge::Acknowledge(Frame &frame)
{
    this->functionID = frame.GetByteAt(0);
    uint8_t frameParsingResults = frame.GetByteAt(1);
    this->success = frameParsingResults == 0x01;
    this->callsignMismatch = frameParsingResults == 0x02;
    this->rxFailed = frameParsingResults == 0x03;
    this->unableToReadFunctionID = frameParsingResults == 0x04;
    this->decryptionFailed  = frameParsingResults == 0x05;
    this->unknownFunctionID = frameParsingResults == 0x06;
}

std::string FOSSASAT2::Messages::Acknowledge::ToString()
{
    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT2" << std::endl;
    ss << "Message Name: Acknowledge" << std::endl;

    std::string out;
    ss >> out;
    return out;
}

std::string FOSSASAT2::Messages::Acknowledge::ToJSON()
{
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "\"Satellite Version\": \"FOSSASAT2\"," << std::endl;
    ss << "\"Message Name\": \"Acknowledge\"," << std::endl;
    ss << "}" << std::endl;

    std::string out;
    ss >> out;
    return out;
}

uint8_t FOSSASAT2::Messages::Acknowledge::GetFunctionID()
{
    return this->functionID;
}

bool FOSSASAT2::Messages::Acknowledge::CommandSuccess()
{
    return this->success;
}

bool FOSSASAT2::Messages::Acknowledge::CallsignMismatch()
{
    return this->callsignMismatch;
}

bool FOSSASAT2::Messages::Acknowledge::RXFailed()
{
    return this->rxFailed;
}

bool FOSSASAT2::Messages::Acknowledge::UnreadableFunctionID()
{
    return this->unableToReadFunctionID;
}

bool FOSSASAT2::Messages::Acknowledge::DecryptionFailed()
{
    return this->decryptionFailed;
}

bool FOSSASAT2::Messages::Acknowledge::OptDataLengthFailed()
{
    return this->optionalDataLengthFailed;
}

bool FOSSASAT2::Messages::Acknowledge::UnknownFunctionID()
{
    return this->unknownFunctionID;
}
