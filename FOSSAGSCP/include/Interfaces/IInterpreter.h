#ifndef IINTERPRETER_H
#define IINTERPRETER_H

#include "IDatagram.h"

class IInterpreter
{
public:
    virtual ~IInterpreter() {}
    virtual IDatagram* SerialMessageToDatagram(const char * serialData, char serialDataLength) = 0;
    virtual IDatagram* GUIMessageToDatagram(char directionBit, char operationId, const char * payloadData) = 0;
};

#endif // IINTERPRETER_H
