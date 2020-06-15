#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Interfaces/IInterpreter.h"

#include "GroundStationSerialMessage.h"

#include <cstring>
#include <stdexcept>

class Interpreter : public IInterpreter
{
public:
    virtual ~Interpreter() {};

    /// @todo checks on the serial data length.
    virtual GroundStationSerialMessage* SerialData_To_GroundStationSerialMessage(const char* serialData, char serialDataLength) override final
    {
        // first byte is the control byte.
        char controlByte = serialData[0];

        // second byte is the payload length.
        char lengthByte = serialData[1];

        // rest of the bytes is the payload.
        char* payload = new char[lengthByte];
        memcpy(payload, &(serialData[2]), lengthByte);

        GroundStationSerialMessage* message = new GroundStationSerialMessage(controlByte, lengthByte, payload);

        // delete the payload, it is copied in the datagram ctor.
        delete[] payload;

        return message;
    }

    virtual GroundStationSerialMessage* Create_GroundStationSerialMessage(char directionBit, char operationId, const char * payloadData) override final
    {
        /// @todo check for over-run.
        char length = strlen(payloadData);

        char* payload = new char[length];
        memcpy(payload, payloadData, length);

        char controlByte = directionBit | operationId;

        GroundStationSerialMessage* datagram = new GroundStationSerialMessage(controlByte, length, payload);

        delete[] payload;

        return datagram;
    }
};

#endif // INTERPRETER_H
