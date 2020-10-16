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

#include "Frame.h"

Frame::Frame()
    : hasFunctionId(false), hasOptionalData(false), encrypted(false) {
}

Frame::~Frame() {

}

Frame::Frame(SatVersion satVersion,
             std::string callsign,
             std::vector<uint8_t> data,
             std::vector<uint8_t> key,
             std::string password,
             bool encrypted)
{
    this->encrypted = encrypted;
    this->satVersion = satVersion;

    // convert the vectors and strings into C compatable variables.
    char* callsignStr = (char*)callsign.c_str();
    uint8_t* dataBytes = data.data();
    size_t dataBytesLength = data.size();
    const uint8_t* keyBytes = key.data();
    size_t keyBytesLength = key.size();
    const char* passwordChars = password.c_str();

    // extract the function ID from the given frameData.
    this->functionId = FCP_Get_FunctionID(callsignStr, dataBytes, dataBytesLength);

    // extract the optional data length from the frameData.
    int16_t optionalDataLength = -5;
    if (this->encrypted)
    {
        optionalDataLength = FCP_Get_OptData_Length(callsignStr, dataBytes, dataBytesLength, keyBytes, passwordChars);
    }
    else
    {
        optionalDataLength = FCP_Get_OptData_Length(callsignStr, dataBytes, dataBytesLength, NULL, NULL);
    }

    // if there is optional data to parse
    if (optionalDataLength > 0)
    {
        this->hasOptionalData = true;
        this->ExtractOptionalData(callsign, data, optionalDataLength, key, password);
    }
    else
    {
        this->hasOptionalData = false;
    }

}

uint8_t Frame::GetByteAt(uint32_t index)
{
    return this->optionalData[index];
}

std::vector<uint8_t> Frame::Serialize()
{
    std::vector<uint8_t> data;
    if (this->hasOptionalData)
    {
        data.insert(data.end(), this->optionalData.begin(), this->optionalData.end());
    }
    return data;
}

std::string Frame::ToHexString()
{
    std::string frameStr;

    if (this->hasFunctionId)
    {
        char hexChar[5];
        hexChar[4] = '\0';
        sprintf(&(hexChar[0]), "%02x, ", this->functionId);
        frameStr.append(hexChar);
    }

    if (this->hasOptionalData)
    {
        for (uint8_t value : this->optionalData)
        {
            char hexChar[5];
            hexChar[4] = '\0';
            sprintf(&(hexChar[0]), "%02x, ", value);
            frameStr.append(hexChar);
        }
    }


    return frameStr;
}

int16_t Frame::GetFunctionID()
{
    return this->functionId;
}

bool Frame::Encrypted()
{
    return this->encrypted;
}


void Frame::ExtractOptionalData(std::string callsign, std::vector<uint8_t> &data, int16_t optionalDataLength, std::vector<uint8_t> key, std::string password)
{
    uint8_t *tempBuffer = new uint8_t[optionalDataLength];

    char* callsignStr = (char*)callsign.c_str();
    uint8_t* dataBytes = data.data();
    size_t dataBytesLength = data.size();
    const uint8_t* keyBytes = key.data();
    size_t keyBytesLength = key.size();
    const char* passwordChars = password.c_str();

    if (this->Encrypted())
    {
        FCP_Get_OptData(callsignStr, dataBytes, dataBytesLength, tempBuffer, keyBytes, passwordChars);
    }
    else
    {
        FCP_Get_OptData(callsignStr, dataBytes, dataBytesLength, tempBuffer, NULL, NULL);
    }

    for (int16_t i = 0; i < optionalDataLength; i++)
    {
        this->optionalData.push_back(tempBuffer[i]);
    }

    delete[] tempBuffer;
}
