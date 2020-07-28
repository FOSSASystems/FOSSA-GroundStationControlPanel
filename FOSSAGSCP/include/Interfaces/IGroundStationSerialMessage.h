#ifndef IGROUNDSTATIONSERIALMESSAGE_H
#define IGROUNDSTATIONSERIALMESSAGE_H

#include "IMessage.h"
#include <string>
#include <QString>

class IGroundStationSerialMessage : public IMessage
{
public:
    virtual ~IGroundStationSerialMessage() {}

    virtual uint8_t GetControlByte() = 0;

    virtual uint8_t GetLengthByte() = 0;

    virtual uint8_t GetFCPFrameLength() = 0;
    virtual void GetFCPFrame(uint8_t* destination) = 0;

    virtual void SetControlByte(uint8_t data) = 0;

    virtual uint8_t GetDirectionBit() = 0;
    virtual uint8_t GetOperationID() = 0;

    virtual QString AsString() = 0;
    virtual void GetDataForGroundStation(uint8_t* destination) = 0;
    virtual int GetDataForGroundStationLength() = 0;
};

#endif // IGROUNDSTATIONSERIALMESSAGE_H
