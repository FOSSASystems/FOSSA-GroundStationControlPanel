#ifndef IINTERPRETER_H
#define IINTERPRETER_H

#include "IDatagram.h"

class IInterpreter
{
public:
    virtual ~IInterpreter() {}
    virtual IDatagram* Interpret(const char * serialData, char serialDataLength) = 0;
};

#endif // IINTERPRETER_H
