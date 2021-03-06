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


Datagram::Datagram(SatVersion satVersion, std::string callsign, std::vector<uint8_t> data, bool inbound, bool encrypted, std::vector<uint8_t> key, std::string password) {
    this->encrypted = encrypted;
    this->key = key;
    this->password = password;
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

std::string Datagram::ToHexString()
{
    std::string hexString;

    char hexChar[5];
    hexChar[4] = '\0';
    sprintf(&(hexChar[0]), "%02x, ", this->controlByte);
    hexString.append(hexChar);

    sprintf(&(hexChar[0]), "%02x, ", this->lengthByte);
    hexChar[4] = '\0';
    hexString.append(hexChar);

    if (this->inbound)
    {
        sprintf(&(hexChar[0]), "%02x, ", (uint8_t)this->radiolibStatusCode);
        hexChar[4] = '\0';
        hexString.append(hexChar);

        sprintf(&(hexChar[0]), "%02x, ", (uint8_t)(this->radiolibStatusCode >> 8));
        hexChar[4] = '\0';
        hexString.append(hexChar);
    }

    std::string frameStr = this->frame.ToHexString();
    hexString.append(frameStr);

    return hexString;
}

bool Datagram::IsEncrypted()
{
    return this->encrypted;
}


void Datagram::ExtractRadiolibStatusCode(std::vector<uint8_t> data) {
    this->radiolibStatusCode = data[2] | (data[3] << 8);
}

void Datagram::ExtractFrame(std::string callsign, std::vector<uint8_t> data) {

    if (this->encrypted)
    {
        // inbound frames have a radiolib status code prepended.
        if (this->inbound)
        {
            uint8_t frameLength = this->lengthByte - 2;
            if (frameLength > 0)
            {
                this->frameExists = true;
                std::vector<uint8_t> frameData;
                frameData.insert(frameData.end(), data.begin() + 4, data.end());

                this->frame = Frame(this->satVersion, callsign, frameData, this->key, this->password, true);
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

                this->frame = Frame(this->satVersion, callsign, frameData, this->key, this->password, true);
            }
            else
            {
                this->frameExists = false;
            }
        }
    }
    else
    {
        // inbound frames have a radiolib status code prepended.
        if (this->inbound)
        {
            uint8_t frameLength = this->lengthByte - 2;
            if (frameLength > 0)
            {
                this->frameExists = true;
                std::vector<uint8_t> frameData;
                frameData.insert(frameData.end(), data.begin() + 4, data.end());

                this->frame = Frame(this->satVersion, callsign, frameData, std::vector<uint8_t>(), std::string(), false);
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

                this->frame = Frame(this->satVersion, callsign, frameData, std::vector<uint8_t>(), std::string(), false);
            }
            else
            {
                this->frameExists = false;
            }
        }
    }


}
