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

#include "FOSSASAT2_Statistics.h"

FOSSASAT2::Messages::Statistics::Statistics(Frame &frame)
{
    uint8_t flagByte = frame.GetByteAt(0);
    this->temperaturesIncluded = flagByte & 0x01;
    this->currentsIncluded = (flagByte >> 1) & 0x01;
    this->voltagesIncluded = (flagByte >> 2) & 0x01;
    this->lightSensorsIncluded = (flagByte >> 3) & 0x01;
    this->imuIncluded = (flagByte >> 4) & 0x01;

    if (this->temperaturesIncluded)
    {
        for (int i = 0; i < 15; i++)
        {
            int startIndex = 1 + (i*2);
            uint8_t lsb = frame.GetByteAt(startIndex);
            uint8_t msb = frame.GetByteAt(startIndex + 1);
            int16_t temperature = lsb | (msb << 8);
            float temperaturesRealValue = temperature * 0.01f;
            this->temperatures.push_back(temperaturesRealValue);
        }
    }

    if (this->currentsIncluded)
    {
        for (int i = 0; i < 18; i++)
        {
            int startIndex = 31 + (i*2);
            uint8_t lsb = frame.GetByteAt(startIndex);
            uint8_t msb = frame.GetByteAt(startIndex + 1);
            int16_t current = lsb | (msb << 8);
            float currentsValue = current * 10;
            this->currents.push_back(currentsValue);
        }
    }

    if (this->voltagesIncluded)
    {
        for (int i = 0; i < 18; i++)
        {
            int startIndex = 67 + i;
            float voltage = frame.GetByteAt(startIndex) * 20;
            this->voltages.push_back(voltage);
        }
    }

    if (this->lightSensorsIncluded)
    {
        for (int i = 0; i < 6; i++)
        {
            int startIndex = 85 + (i * 4);
            uint8_t lsb = frame.GetByteAt(startIndex);
            uint8_t a = frame.GetByteAt(startIndex + 1);
            uint8_t b = frame.GetByteAt(startIndex + 2);
            uint8_t msb = frame.GetByteAt(startIndex + 3);

            uint32_t bytesVal = lsb;
            bytesVal |= a << 8;
            bytesVal |= b << 16;
            bytesVal |= msb << 24;

            float lightSensorValue = 0.0f;
            memcpy(&lightSensorValue, &bytesVal, 4);
            this->currents.push_back(lightSensorValue);
        }
    }

    if (this->imuIncluded)
    {
        for (int i = 0; i < 16; i++)
        {
            int startIndex = 109 + (i * 4);

            uint8_t lsb = frame.GetByteAt(startIndex);
            uint8_t a = frame.GetByteAt(startIndex + 1);
            uint8_t b = frame.GetByteAt(startIndex + 2);
            uint8_t msb = frame.GetByteAt(startIndex + 3);

            uint32_t bytesVal = lsb;
            bytesVal |= a << 8;
            bytesVal |= b << 16;
            bytesVal |= msb << 24;

            float imuValue = 0.0f;
            memcpy(&imuValue, &bytesVal, 4);
            this->imus.push_back(imuValue);
        }
    }
}

std::string FOSSASAT2::Messages::Statistics::ToString()
{

    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT2" << std::endl;
    ss << "Message Name: Statistics" << std::endl;
    ss << "Temperatures included: " << this->temperaturesIncluded << std::endl;
    ss << "Currents included: " << this->currentsIncluded << std::endl;
    ss << "Voltages included: " << this->voltagesIncluded << std::endl;
    ss << "Light sensors included: " << this->lightSensorsIncluded << std::endl;
    ss << "IMU included: " << this->imuIncluded << std::endl;

    ss << "Temperatures: " << std::endl;
    for (float temp : this->temperatures)
    {
        ss << " " << temp << " deg. C" << std::endl;
    }
    ss << "Currents: " << std::endl;
    for (float current : this->currents)
    {
        ss << " " << current << " uA" << std::endl;
    }

    ss << "Voltages: " << std::endl;
    for (float voltage : this->voltages)
    {
        ss << " " << voltage << " mV" << std::endl;
    }

    ss << "Light sensors: " << std::endl;
    for (float lightSensor : this->lightSensors)
    {
        ss << " " << lightSensor << std::endl;
    }

    ss << "IMU: " << std::endl;
    for (float imu : this->imus)
    {
        ss << " " << imu << std::endl;
    }


    std::string out;
    ss >> out;
    return out;
}

std::string FOSSASAT2::Messages::Statistics::ToJSON()
{
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "\"Satellite Version\": \"FOSSASAT2\"," << std::endl;
    ss << "\"Message Name\": \"Statistics\"," << std::endl;
    ss << "\"Temperatures included\": " << this->temperaturesIncluded << std::endl;
    ss << "\"Currents included\": " << this->currentsIncluded << std::endl;
    ss << "\"Voltages included\": " << this->voltagesIncluded << std::endl;
    ss << "\"Light sensors included\": " << this->lightSensorsIncluded << std::endl;
    ss << "\"IMU included\": " << this->imuIncluded << std::endl;

    ss << "\"Temperatures\": " << std::endl;
    ss << "{" << std::endl;
    for (float temp : this->temperatures)
    {
        ss << temp << "," << std::endl;
    }
    ss << "\"Currents\": " << std::endl;
    ss << "{" << std::endl;
    for (float current : this->currents)
    {
        ss << " \"" << current << "\"," << std::endl;
    }
    ss << "}" << std::endl;

    ss << "\"Voltages\": " << std::endl;
    ss << "{" << std::endl;
    for (float voltage : this->voltages)
    {
        ss << " \"" << voltage << "\"," << std::endl;
    }
    ss << "}" << std::endl;

    ss << "\"Light sensors\": " << std::endl;
    ss << "{" << std::endl;
    for (float lightSensor : this->lightSensors)
    {
        ss << " \"" << lightSensor << "\"," << std::endl;
    }
    ss << "}" << std::endl;

    ss << "\"IMU\": " << std::endl;
    ss << "{" << std::endl;
    for (float imu : this->imus)
    {
        ss << " \"" << imu << "\"," << std::endl;
    }
    ss << "}" << std::endl;

    ss << "}" << std::endl;


    std::string out;
    ss >> out;
    return out;
}

bool FOSSASAT2::Messages::Statistics::IsTemperaturesIncluded() const {
    return temperaturesIncluded;
}

bool FOSSASAT2::Messages::Statistics::IsCurrentsIncluded() const {
    return currentsIncluded;
}

bool FOSSASAT2::Messages::Statistics::IsVoltagesIncluded() const {
    return voltagesIncluded;
}

bool FOSSASAT2::Messages::Statistics::IsLightSensorsIncluded() const {
    return lightSensorsIncluded;
}

bool FOSSASAT2::Messages::Statistics::IsImuIncluded() const {
    return imuIncluded;
}

const std::vector<float> &FOSSASAT2::Messages::Statistics::GetTemperatures() const {
    return temperatures;
}

const std::vector<float> &FOSSASAT2::Messages::Statistics::GetCurrents() const {
    return currents;
}

const std::vector<float> &FOSSASAT2::Messages::Statistics::GetVoltages() const {
    return voltages;
}

const std::vector<float> &FOSSASAT2::Messages::Statistics::GetLightSensors() const {
    return lightSensors;
}

const std::vector<float> &FOSSASAT2::Messages::Statistics::GetImus() const {
    return imus;
}
