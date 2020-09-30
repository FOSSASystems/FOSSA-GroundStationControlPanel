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

#include "FOSSASAT2_RecordedIMU.h"

FOSSASAT2::Messages::RecordedIMU::RecordedIMU(Frame &frame)
{
    uint8_t deviceFlags = frame.GetByteAt(0);
    this->gyro = deviceFlags & 0b00000001;
    this->accele = (deviceFlags>>1) & 0b00000001;
    this->magn = (deviceFlags>>2) & 0b00000001;

    if (this->gyro)
    {
        uint32_t gyroSampleX = frame.GetByteAt(1);
        gyroSampleX |= frame.GetByteAt(2) << 8;
        gyroSampleX |= frame.GetByteAt(3) << 16;
        gyroSampleX |= frame.GetByteAt(4) << 24;
        memcpy(&this->gyroX, &gyroSampleX, 4);

        uint32_t gyroSampleY = frame.GetByteAt(5);
        gyroSampleY |= frame.GetByteAt(6) << 8;
        gyroSampleY |= frame.GetByteAt(7) << 16;
        gyroSampleY |= frame.GetByteAt(8) << 24;
        memcpy(&this->gyroY, &gyroSampleY, 4);

        uint32_t gyroSampleZ = frame.GetByteAt(9);
        gyroSampleZ |= frame.GetByteAt(10) << 8;
        gyroSampleZ |= frame.GetByteAt(11) << 16;
        gyroSampleZ |= frame.GetByteAt(12) << 24;
        memcpy(&this->gyroZ, &gyroSampleZ, 4);
    }

    if (this->accele)
    {
        uint32_t acceleSampleX = frame.GetByteAt(13);
        acceleSampleX |= frame.GetByteAt(14) << 8;
        acceleSampleX |= frame.GetByteAt(15) << 16;
        acceleSampleX |= frame.GetByteAt(16) << 24;
        memcpy(&this->acceleX, &acceleSampleX, 4);

        uint32_t acceleSampleY = frame.GetByteAt(17);
        acceleSampleY |= frame.GetByteAt(18) << 8;
        acceleSampleY |= frame.GetByteAt(19) << 16;
        acceleSampleY |= frame.GetByteAt(20) << 24;
        memcpy(&this->acceleY, &acceleSampleY, 4);

        uint32_t acceleSampleZ = frame.GetByteAt(21);
        acceleSampleZ |= frame.GetByteAt(22) << 8;
        acceleSampleZ |= frame.GetByteAt(23) << 16;
        acceleSampleZ |= frame.GetByteAt(24) << 24;
        memcpy(&this->acceleZ, &acceleSampleZ, 4);
    }

    if (this->magn)
    {
        uint32_t magnSampleX = frame.GetByteAt(25);
        magnSampleX |= frame.GetByteAt(26) << 8;
        magnSampleX |= frame.GetByteAt(27) << 16;
        magnSampleX |= frame.GetByteAt(28) << 24;
        memcpy(&this->magnX, &magnSampleX, 4);

        uint32_t magnSampleY = frame.GetByteAt(29);
        magnSampleY |= frame.GetByteAt(30) << 8;
        magnSampleY |= frame.GetByteAt(31) << 16;
        magnSampleY |= frame.GetByteAt(32) << 24;
        memcpy(&this->magnY, &magnSampleY, 4);

        uint32_t magnSampleZ = frame.GetByteAt(33);
        magnSampleZ |= frame.GetByteAt(34) << 8;
        magnSampleZ |= frame.GetByteAt(35) << 16;
        magnSampleZ |= frame.GetByteAt(36) << 24;
        memcpy(&this->magnZ, &magnSampleZ, 4);
    }
}

std::string FOSSASAT2::Messages::RecordedIMU::ToString()
{
    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT2" << std::endl;
    ss << "Message Name: RecordedIMU" << std::endl;
    ss << "Gyroscope flag: " << this->gyro << std::endl;
    ss << "Accelerometer flag: " << this->gyro << std::endl;
    ss << "Magnetometer flag: " << this->gyro << std::endl;
    ss << "Gyroscope sample: " << this->gyroX << "," << this->gyroY << "," << this->gyroZ <<std::endl;
    ss << "Accelerometer sample: " << this->acceleX << "," <<this->acceleY << "," << this->acceleZ <<std::endl;
    ss << "Magnetometer sample: " << this->magnX  << "," <<this->magnY  << "," << this->magnZ  <<std::endl;

    std::string out;
    ss >> out;
    return out;
}

std::string FOSSASAT2::Messages::RecordedIMU::ToJSON()
{
    std::stringstream ss;
    ss << "\"Satellite Version\": \"FOSSASAT2\"" << std::endl;
    ss << "\"Message Name\": \"RecordedIMU\"" << std::endl;
    ss << "\"Gyroscope flag\": \"" << this->gyro << "\"" << std::endl;
    ss << "\"Accelerometer flag\": \"" << this->gyro << "\"" << std::endl;
    ss << "\"Magnetometer flag\": \"" << this->gyro << "\"" << std::endl;

    ss << "\"Gyroscope sample\": {" << std::endl;
    ss << "\"x: \"" << this->gyroX << "\"" << std::endl;
    ss << "\"y: \"" << this->gyroY << "\"" << std::endl;
    ss << "\"z: \"" << this->gyroZ << "\"" << std::endl;
    ss << "}" << std::endl;


    ss << "\"Accelerometer sample\": {" << std::endl;
    ss << "\"x: \"" << this->acceleX << "\"" << std::endl;
    ss << "\"y: \"" << this->acceleY << "\"" << std::endl;
    ss << "\"z: \"" << this->acceleZ << "\"" << std::endl;
    ss << "}" << std::endl;

    ss << "\"Magnetometer sample\": {" << std::endl;
    ss << "\"x: \"" << this->magnX << "\"" << std::endl;
    ss << "\"y: \"" << this->magnY << "\"" << std::endl;
    ss << "\"z: \"" << this->magnZ << "\"" << std::endl;
    ss << "}" << std::endl;

    std::string out;
    ss >> out;
    return out;
}
