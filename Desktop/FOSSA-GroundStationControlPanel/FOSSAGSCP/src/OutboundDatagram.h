#ifndef OUTBOUNDDATAGRAM_H
#define OUTBOUNDDATAGRAM_H

#include "IFrame.h"

#include "IDatagram.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <string.h>

// outbound datagrams do not have the radiolib status code
class OutboundDatagram : public IDatagram
{
public:
    OutboundDatagram(uint8_t controlByte, uint8_t lengthByte, IFrame* frame);
    OutboundDatagram(const OutboundDatagram& other);
    virtual ~OutboundDatagram();

    virtual QString ToString() const override;

    virtual const IFrame *const GetFrame() const override;

    virtual uint8_t GetOperationId() const override;
    virtual uint8_t GetControlByte() const override;
    virtual uint8_t GetLengthByte() const override;

    virtual const uint8_t* GetData() const override;
    virtual const uint32_t GetLength() const override;
private:
    uint8_t m_controlByte;
    uint8_t m_lengthByte;

    IFrame* m_frame;
};

#endif // OUTBOUNDDATAGRAM_H
