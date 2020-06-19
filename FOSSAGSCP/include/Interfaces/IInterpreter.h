#ifndef IINTERPRETER_H
#define IINTERPRETER_H

#include <cstdint>

#include "IMessage.h"
#include "IGroundStationSerialMessage.h"
#include "ISettings.h"

class IInterpreter
{
public:

    virtual ~IInterpreter() {}
    virtual IGroundStationSerialMessage* SerialData_To_GroundStationSerialMessage(const char * serialData, char serialDataLength) = 0;
    virtual IGroundStationSerialMessage* Create_GroundStationSerialMessage(char operationId, uint8_t functionId, uint8_t optDataLength, char* optData) = 0;
    virtual void RegisterSettings(ISettings* settings) = 0;
};

#endif // IINTERPRETER_H
