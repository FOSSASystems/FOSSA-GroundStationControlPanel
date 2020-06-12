#ifndef IDATAGRAM_H
#define IDATAGRAM_H

class IDatagram
{
public:
    virtual ~IDatagram() {}

    virtual char GetControlByte() = 0;
    virtual char GetPayloadLengthByte() = 0;
    virtual char* GetPayload() = 0;

    virtual void SetControlByte(char data) = 0;
    virtual void SetPayload(char* data, char length) = 0;

    virtual char GetDirectionBit() = 0;
    virtual char GetOperationID() = 0;
};

#endif // IDATAGRAM_H
