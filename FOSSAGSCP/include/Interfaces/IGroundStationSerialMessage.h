#ifndef IGROUNDSTATIONSERIALMESSAGE_H
#define IGROUNDSTATIONSERIALMESSAGE_H

#include "IMessage.h"
#include <string>

class IGroundStationSerialMessage : public IMessage
{
public:
    virtual ~IGroundStationSerialMessage() {}

    virtual char GetControlByte() = 0;
    virtual char GetPayloadLengthByte() = 0;
    virtual char* GetPayload() = 0;

    virtual void SetControlByte(char data) = 0;
    virtual void SetPayload(char* data, char length) = 0;

    virtual char GetDirectionBit() = 0;
    virtual char GetOperationID() = 0;

    virtual std::string GetRawData() = 0;
};

#endif // IGROUNDSTATIONSERIALMESSAGE_H
