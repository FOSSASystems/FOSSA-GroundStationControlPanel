#ifndef INBOUNDDATAGRAM_H
#define INBOUNDDATAGRAM_H

#include "IDatagram.h"

// inbound datagrams include the radiolib status code
class InboundDatagram : public IDatagram
{
public:
    InboundDatagram(uint8_t controlByte, uint8_t lengthByte, int16_t radiolibStatusCode, IFrame* frame);
    InboundDatagram(const InboundDatagram& other);
    virtual ~InboundDatagram();

    virtual QString ToString() const override;

    virtual const IFrame *const GetFrame() const override;

    virtual uint8_t GetOperationId() const override;
    virtual uint8_t GetControlByte() const override;
    virtual uint8_t GetLengthByte() const override;

    virtual const uint8_t* GetData() const override;
    /// returns the length of the enitre datagram from control byte to end.
    virtual const uint32_t GetLength() const override;

    const uint16_t GetStatusCode() const;
private:
    uint8_t m_controlByte;
    uint8_t m_lengthByte;
    uint8_t m_radiolibStatusCode;
    IFrame* m_frame = nullptr;
};

#endif // INBOUNDDATAGRAM_H
