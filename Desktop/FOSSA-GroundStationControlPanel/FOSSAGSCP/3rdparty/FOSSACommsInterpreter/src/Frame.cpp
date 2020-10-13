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

Frame::Frame() {

}

Frame::~Frame() {

}

Frame::Frame(SatVersion satVersion, std::string callsign, std::vector<uint8_t> data, bool encrypted) {
    this->encrypted = encrypted;
    this->satVersion = satVersion;
    this->functionId = FCP_Get_FunctionID((char *)callsign.c_str(), data.data(), data.size());

    int16_t optionalDataLength = FCP_Get_OptData_Length((char *)callsign.c_str(), data.data(), data.size());
    if (optionalDataLength > 0) {
        this->hasOptionalData = true;
        this->ExtractOptionalData(callsign, data, optionalDataLength);
    }
    else {
        this->hasOptionalData = false;
    }
}

uint8_t Frame::GetByteAt(uint32_t index) {
    return this->optionalData[index];
}

std::vector<uint8_t> Frame::Serialize() {
    std::vector<uint8_t> data;
    data.insert(data.end(), this->optionalData.begin(), this->optionalData.end());
    return data;
}

std::string Frame::ToHexString()
{
    std::string frameStr;

    for (uint8_t value : this->optionalData) {
        char hexChar[5];
        hexChar[4] = '\0';
        sprintf(&(hexChar[0]), "%02x, ", value);
        frameStr.append(hexChar);
    }

    return frameStr;
}

int16_t Frame::GetFunctionID()
{
    return this->functionId;
}

void Frame::SetFunctionID(int16_t functionID)
{
    this->functionId = functionID;
}


void Frame::ExtractOptionalData(std::string callsign, std::vector<uint8_t> &data, int16_t optionalDataLength) {
    uint8_t *tempBuffer = new uint8_t[optionalDataLength];
    FCP_Get_OptData((char *)callsign.c_str(), data.data(), data.size(), tempBuffer, NULL, NULL);

    for (int16_t i = 0; i < optionalDataLength; i++) {
        this->optionalData.push_back(tempBuffer[i]);
    }

    delete[] tempBuffer;
}
