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

#include "Datagram.h"

#include <sstream>


Datagram::Datagram(SatVersion satVersion, std::string callsign, std::vector<uint8_t> data, bool inbound) {
	this->satVersion = satVersion;
	this->inbound = inbound;
	this->callsign = callsign;

	this->controlByte = data[0];
	this->lengthByte = data[1];
	this->operationId = (OperationID)(this->controlByte & 0b01111111);

    if (inbound) {
        switch (this->operationId) {
        case OperationID::HANDSHAKE:
            break;
        case OperationID::FRAME:
            this->ExtractRadiolibStatusCode(data);
            this->ExtractFrame(callsign, data);
            break;
        case OperationID::CONFIG:
            this->ExtractRadiolibStatusCode(data);
            break;
        case OperationID::CARRIER:
            this->ExtractRadiolibStatusCode(data);
            break;
        default:
            throw std::runtime_error("Datagram operation id invalid.");
            break;
        }
    } else {
        switch (this->operationId) {
        case OperationID::HANDSHAKE:
            break;
        case OperationID::FRAME:
            this->ExtractFrame(callsign, data);
            break;
        case OperationID::CONFIG:
            break;
        case OperationID::CARRIER:
            break;
        default:
            throw std::runtime_error("Datagram operation id invalid.");
            break;
        }
    }


}

int16_t Datagram::GetFrameFunctionID()
{
    return this->frame.GetFunctionID();
}

OperationID Datagram::GetOperationID()
{
    return this->operationId;
}

Frame Datagram::GetFrame()
{
    return this->frame;
}

std::vector<uint8_t> Datagram::Serialize()
{
    std::vector<uint8_t> data;

    data.push_back(this->controlByte);
    data.push_back(this->lengthByte);

    if (this->frameExists)
    {
        std::vector<uint8_t> frameData = this->frame.Serialize();
        data.insert(data.end(), frameData.begin(), frameData.end());
    }

    return data;
}

std::string Datagram::ToString()
{
    std::stringstream ss;

    std::vector<uint8_t> data = this->Serialize();

    for (uint8_t& v : data) {
        ss << v << ",";
    }
    ss << std::endl;

    std::string str;
    ss >> str;

    return str;
}

void Datagram::ExtractRadiolibStatusCode(std::vector<uint8_t> data) {
	this->radiolibStatusCode = data[2] | (data[3] << 8);
}

void Datagram::ExtractFrame(std::string callsign, std::vector<uint8_t> data) {
    // inbound frames have a radiolib status code prepended.
    if (this->inbound)
    {
        uint8_t frameLength = this->lengthByte - 2;
        if (frameLength > 0)
        {
            this->frameExists = true;
            std::vector<uint8_t> frameData;
            frameData.insert(frameData.end(), data.begin() + 4, data.end());

            this->frame = Frame(this->satVersion, callsign, frameData);
        }
        else
        {
            this->frameExists = false;
        }
    }
    else
    {
        uint8_t frameLength = this->lengthByte;
        if (frameLength > 0)
        {
            this->frameExists = true;
            std::vector<uint8_t> frameData;
            frameData.insert(frameData.end(), data.begin() + 2, data.end());

            this->frame = Frame(this->satVersion, callsign, frameData);
        }
        else
        {
            this->frameExists = false;
        }
    }

}
