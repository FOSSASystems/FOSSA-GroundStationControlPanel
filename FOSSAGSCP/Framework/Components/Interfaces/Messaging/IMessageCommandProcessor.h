#pragma once

#include <string>

namespace FOSSA
{

class IMessage;
class IMessageCommandProcessor
{
public:
    virtual ~IMessageCommandProcessor() {}
    virtual IMessage* Process(std::string data) = 0;
};

}
