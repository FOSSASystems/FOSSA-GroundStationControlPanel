#ifndef ICHANNEL_H
#define ICHANNEL_H

#include "IDatagram.h"

class IChannel
{
public:
    virtual ~IChannel() {}
    virtual void Push(IDatagram* datagram) = 0;
    virtual IDatagram* Peek() = 0;
    virtual void Pop() = 0;
};

#endif // ICHANNEL_H
