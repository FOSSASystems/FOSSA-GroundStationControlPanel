#include "OutboundDatagram.h"

OutboundDatagram::OutboundDatagram(uint8_t controlByte, uint8_t lengthByte, IFrame *frame)
    : m_controlByte(controlByte), m_lengthByte(lengthByte), m_frame(frame)
{
}

OutboundDatagram::OutboundDatagram(const OutboundDatagram &other)
{
    m_controlByte = other.m_controlByte;
    m_lengthByte = other.m_lengthByte;
    m_frame = other.m_frame;
}

OutboundDatagram::~OutboundDatagram()
{
}

QString OutboundDatagram::ToString() const
{
    QString datagramStr;

    char hexChar[5];
    hexChar[4] = '\0';
    sprintf(&(hexChar[0]), "%02x, ", m_controlByte);
    datagramStr.append(hexChar);

    sprintf(&(hexChar[0]), "%02x, ", m_lengthByte);
    hexChar[4] = '\0';
    datagramStr.append(hexChar);

    QString frameStr = m_frame->ToString();
    datagramStr.append(frameStr);

    return datagramStr;
}

const IFrame * const OutboundDatagram::GetFrame() const
{
    return m_frame;
}

const uint8_t* OutboundDatagram::GetData() const
{
    uint8_t controlByte = GetControlByte();
    uint8_t lengthByte = GetLengthByte();
    uint32_t frameLength = GetFrame()->GetLength();

    uint32_t datagramLength = GetLength();

    const uint8_t* frameData = GetFrame()->GetData();

    uint8_t* outDatagramData = new uint8_t[datagramLength];
    outDatagramData[0] = controlByte;
    outDatagramData[1] = lengthByte;
    memcpy_s(&(outDatagramData[2]), frameLength, frameData, frameLength);

    // control byte, length byte, fcp frame
    return outDatagramData;
};

uint8_t OutboundDatagram::GetOperationId() const
{
    return (m_controlByte & 0b01111111);
}

uint8_t OutboundDatagram::GetControlByte() const
{
    return m_controlByte;
}

uint8_t OutboundDatagram::GetLengthByte() const
{
    return m_lengthByte;
}

const uint32_t OutboundDatagram::GetLength() const
{
    return GetLengthByte() + 2; // + 2, length byte excludes itself and the control byte.
}
