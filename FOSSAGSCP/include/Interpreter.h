#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Interfaces/IInterpreter.h"
#include "Datagram.h"

#include <cstring>
#include <stdexcept>

class Interpreter : public IInterpreter
{
public:
    virtual ~Interpreter() {};

    /// @todo checks on the serial data length.
    virtual Datagram* Interpret(const char* serialData, char serialDataLength) override final
    {
        // first byte is the control byte.
        char controlByte = serialData[0];

        // second byte is the payload length.
        char lengthByte = serialData[1];

        // rest of the bytes is the payload.
        char* payload = new char[lengthByte];
        memcpy(payload, &(serialData[2]), lengthByte);

        Datagram* datagram = new Datagram(controlByte, lengthByte, payload);

        // delete the payload, it is copied in the datagram ctor.
        delete[] payload;

        return datagram;
    }
};

#endif // INTERPRETER_H
