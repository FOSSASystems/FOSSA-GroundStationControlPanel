#ifndef GROUNDSTATIONSERIALMESSAGE_H
#define GROUNDSTATIONSERIALMESSAGE_H

#include <cstdlib>
#include <cstring>
#include <string>

#include "Interfaces/IGroundStationSerialMessage.h"

class GroundStationSerialMessage : public IGroundStationSerialMessage
{
public:
    GroundStationSerialMessage(char controlByte, char lengthByte, char* payload)
    {
        m_controlByte = controlByte;
        m_payloadLengthByte = lengthByte;

        if (lengthByte > 0)
        {
            m_payload = new char[lengthByte];
            memcpy(m_payload, payload, lengthByte);
        }
        else
        {
            m_payload = nullptr;
        }
    }

    GroundStationSerialMessage(const GroundStationSerialMessage& other)
    {
        if (m_payload != nullptr)
        {
            delete[] m_payload;
        }

        m_controlByte = other.m_controlByte;
        m_payloadLengthByte = other.m_payloadLengthByte;

        if (other.m_payload != nullptr)
        {
            if (m_payloadLengthByte > 0)
            {
                m_payload = new char[m_payloadLengthByte];
                memcpy(m_payload, other.m_payload, m_payloadLengthByte);
            }
        }
    }

    virtual ~GroundStationSerialMessage()
    {
        if (m_payload != nullptr)
        {
            delete[] m_payload;
        }
    }

    virtual char GetControlByte() override { return m_controlByte; }
    virtual char GetPayloadLengthByte() override { return m_payloadLengthByte; };
    virtual char *GetPayload() override { return m_payload; };

    virtual void SetControlByte(char data) override { m_controlByte = data; }
    virtual void SetPayload(char *data, char length) override
    {
        m_payloadLengthByte = length;

        if (m_payloadLengthByte > 0)
        {
            m_payload = new char[length];
            memcpy(m_payload, data, length);
        }
    };

    virtual char GetDirectionBit() override
    {
        return ((m_controlByte & 0b10000000) >> 7);
    }

    virtual char GetOperationID() override
    {
        return m_controlByte & 0b01111111;
    }

    virtual std::string GetRawData() override
    {
        std::string rawData;
        rawData.push_back(m_controlByte);
        rawData.push_back(m_payloadLengthByte);

        for (int i = 0; i < m_payloadLengthByte; i++)
        {
            rawData.push_back(m_payload[i]);
        }
        return rawData;
    }

private:
    char m_controlByte;
    char m_payloadLengthByte;
    char* m_payload = nullptr;
};

#endif // GROUNDSTATIONSERIALMESSAGE_H
