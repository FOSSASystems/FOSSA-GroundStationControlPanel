#pragma once

#include <cstdint>

#include "../ResourceManagment/IBuffer.h"

class IMessage;
class IMessageChannel : public IBuffer<IMessage>
{
public:
    virtual ~IMessageChannel() {}
};
