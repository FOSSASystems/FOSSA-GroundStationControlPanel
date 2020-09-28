#ifndef IMESSAGE_H
#define IMESSAGE_H

#include "IFrame.h"

#include <QString>

class IDatagram
{
public:
    virtual ~IDatagram();

    virtual QString ToString() const = 0;

    virtual const IFrame * const GetFrame() const = 0;

    virtual uint8_t GetOperationId() const = 0;
    virtual uint8_t GetControlByte() const = 0;
    virtual uint8_t GetLengthByte() const = 0;

    virtual const uint8_t* GetData() const = 0;

    /// @brief entire length of the datagram.
    virtual const uint32_t GetLength() const = 0;
};

#endif // IMESSAGE_H
