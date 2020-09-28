#ifndef GROUNDSTATIONSERIALMESSAGE_H
#define GROUNDSTATIONSERIALMESSAGE_H

#include <QDebug>

#include <cstdlib>
#include <cstring>
#include <string>

#include "Interfaces/IGroundStationSerialMessage.h"

class GroundStationSerialMessage : public IGroundStationSerialMessage
{
public:
    GroundStationSerialMessage(uint8_t controlByte, uint8_t lengthByte, uint8_t* fcpFrame, int16_t statusCode = 0)
    {
        m_controlByte = controlByte;
        m_lengthByte = lengthByte;
        m_statusCode = statusCode;
        m_fcpFrameLength = m_lengthByte - 2;

        if (m_fcpFrameLength > 0)
        {
            m_fcpFrame = new uint8_t[m_fcpFrameLength];
            memcpy(m_fcpFrame, fcpFrame, m_fcpFrameLength);
        }
        else
        {
            m_fcpFrame = nullptr;
        }
    }

    GroundStationSerialMessage(const GroundStationSerialMessage& other)
    {
        if (m_fcpFrame != nullptr)
        {
            delete[] m_fcpFrame;
        }

        m_controlByte = other.m_controlByte;
        m_fcpFrameLength = other.m_fcpFrameLength;
        m_lengthByte = other.m_lengthByte;

        if (other.m_fcpFrame != nullptr)
        {
            if (other.m_fcpFrameLength > 0)
            {
                m_fcpFrame = new uint8_t[m_fcpFrameLength];
                memcpy(m_fcpFrame, other.m_fcpFrame, m_fcpFrameLength);
            }
            else
            {
                m_fcpFrame = nullptr;
            }
        }
    }

    virtual ~GroundStationSerialMessage()
    {
        if (m_fcpFrame != nullptr)
        {
            delete[] m_fcpFrame;
        }
    }

    virtual uint8_t GetControlByte() override
    {
        return m_controlByte;
    }

    virtual uint8_t GetLengthByte() override
    {
        return m_lengthByte;
    };

    virtual uint8_t GetFCPFrameLength() override
    {
        return m_fcpFrameLength;
    }

    /// Excludes the radiolib status code.
    virtual void GetFCPFrame(uint8_t* destination) override
    {
        memcpy_s(destination, m_fcpFrameLength, m_fcpFrame, m_fcpFrameLength);
    };

    virtual void SetControlByte(uint8_t data) override
    {
        m_controlByte = data;
    }

    virtual uint8_t GetDirectionBit() override
    {
        return ((m_controlByte & 0b10000000) >> 7);
    }

    virtual uint8_t GetOperationID() override
    {
        return m_controlByte & 0b01111111;
    }

    virtual QString AsString() override
    {
        QString str;


        char hexChar[5];
        hexChar[4] = '\0';
        sprintf(&(hexChar[0]), "%02x, ", m_controlByte);
        str.append(hexChar);

        sprintf(&(hexChar[0]), "%02x, ", m_lengthByte);
        hexChar[4] = '\0';
        str.append(hexChar);

        sprintf(&(hexChar[0]), "%02x, ", (uint8_t)(m_statusCode));
        hexChar[4] = '\0';
        str.append(hexChar);

        sprintf(&(hexChar[0]), "%02x, ", (uint8_t)(m_statusCode >> 8));
        hexChar[4] = '\0';
        str.append(hexChar);

        for (int i = 0; i < m_fcpFrameLength; i++)
        {
            sprintf(&(hexChar[0]), "%02x, ", m_fcpFrame[i]);
            hexChar[4] = '\0';
            str += hexChar;
        }

        return str;
    }

    virtual int GetDataForGroundStationLength() override
    {
        return m_fcpFrameLength + 2;
    }

    virtual void GetDataForGroundStation(uint8_t* destination) override
    {
        destination = new uint8_t[GetDataForGroundStationLength()];

        memcpy(destination, &m_controlByte, 1);
        memcpy(&(destination[1]), &m_lengthByte, 1);
        memcpy(&(destination[2]), &m_fcpFrame, m_fcpFrameLength);
    }


private:
    uint8_t m_controlByte;
    uint8_t m_lengthByte;
    uint16_t m_statusCode;

    uint8_t* m_fcpFrame = nullptr;
    uint8_t m_fcpFrameLength;
};

#endif // GROUNDSTATIONSERIALMESSAGE_H
