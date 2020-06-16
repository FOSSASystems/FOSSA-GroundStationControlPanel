#ifndef IINTERPRETER_H
#define IINTERPRETER_H

#include "IMessage.h"
#include "IGroundStationSerialMessage.h"

#include <cstdint>

class IInterpreter
{
public:
    virtual ~IInterpreter() {}
    virtual IGroundStationSerialMessage* SerialData_To_GroundStationSerialMessage(const char * serialData, char serialDataLength) = 0;
    virtual IGroundStationSerialMessage* Create_GroundStationSerialMessage(char directionBit, char operationId, uint8_t functionId, char* optData, bool encrypt) = 0;
};

#endif // IINTERPRETER_H
