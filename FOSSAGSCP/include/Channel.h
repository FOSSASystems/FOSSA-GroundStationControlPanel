#ifndef CHANNEL_H
#define CHANNEL_H

#include "Interfaces/IChannel.h"

#include <queue>

class Channel : public IChannel
{
    // IChannel interface
public:
    virtual void Push(IDatagram *datagram) override
    {
        m_buffer.push(datagram);
    }

    virtual IDatagram *Peek() override
    {
        return m_buffer.front();
    }

    virtual void Pop() override
    {
        IDatagram* datagram = Peek();
        delete datagram;
        m_buffer.pop();
    }
private:
    std::queue<IDatagram*> m_buffer;
};

#endif // CHANNEL_H
