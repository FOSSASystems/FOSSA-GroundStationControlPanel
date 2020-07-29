#include "InboundDatagram.h"

InboundDatagram::InboundDatagram(uint8_t controlByte, uint8_t lengthByte, int16_t radiolibStatusCode, IFrame *frame)
    : m_controlByte(controlByte), m_lengthByte(lengthByte), m_radiolibStatusCode(radiolibStatusCode), m_frame(frame)
{

}

InboundDatagram::InboundDatagram(const InboundDatagram &other)
{
    m_controlByte = other.m_controlByte;
    m_lengthByte = other.m_lengthByte;
    m_radiolibStatusCode = other.m_radiolibStatusCode;

    if (m_lengthByte - 2 > 0)
    {

    }
}

InboundDatagram::~InboundDatagram()
{

}

QString InboundDatagram::ToString() const
{
    QString datagramStr;

    char hexChar[5];
    hexChar[4] = '\0';
    sprintf(&(hexChar[0]), "%02x, ", m_controlByte);
    datagramStr.append(hexChar);

    sprintf(&(hexChar[0]), "%02x, ", m_lengthByte);
    hexChar[4] = '\0';
    datagramStr.append(hexChar);

    sprintf(&(hexChar[0]), "%02x, ", (uint8_t)m_radiolibStatusCode);
    hexChar[4] = '\0';
    datagramStr.append(hexChar);

    sprintf(&(hexChar[0]), "%02x, ", (uint8_t)(m_radiolibStatusCode >> 8));
    hexChar[4] = '\0';
    datagramStr.append(hexChar);

    QString frameStr = m_frame->ToString();
    datagramStr.append(frameStr);

    return datagramStr;
}

const IFrame *const InboundDatagram::GetFrame() const
{
    return m_frame;
}

uint8_t InboundDatagram::GetOperationId() const
{
    return (m_controlByte & 0b01111111);
}

uint8_t InboundDatagram::GetControlByte() const
{
    return m_controlByte;
}

uint8_t InboundDatagram::GetLengthByte() const
{
    return m_lengthByte;
}

// inbound datagrams include the radiolib status code
const uint8_t *InboundDatagram::GetData() const
{
    // control byte, length byte, radlib status code lsb, radio status code msb, fcp frame
    uint8_t controlByte = GetControlByte();
    uint8_t lengthByte = GetLengthByte(); // includes status code.
    uint16_t statusCode = GetStatusCode();

    uint32_t datagramLength = GetLength();

    uint32_t frameLength = GetFrame()->GetLength();
    const uint8_t* frameData = GetFrame()->GetData();



    uint8_t* outDatagramData = new uint8_t[datagramLength];
    outDatagramData[0] = controlByte;
    outDatagramData[1] = lengthByte;
    outDatagramData[2] = statusCode;
    outDatagramData[3] = statusCode >> 8;
    memcpy_s(&(outDatagramData[4]), frameLength, frameData, frameLength);

    // control byte, length byte, fcp frame
    return outDatagramData;
}

const uint32_t InboundDatagram::GetLength() const
{
    return GetLengthByte() + 2; // length byte includes status code, excludes itself and control byte.
}

const uint16_t InboundDatagram::GetStatusCode() const
{
    return m_radiolibStatusCode;
}
