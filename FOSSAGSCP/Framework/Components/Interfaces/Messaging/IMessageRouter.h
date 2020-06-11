#pragma once

#include <string>

#include "Framework/ResourceManagmentFramework/Components/IRegistry.hpp"

class IMessageRouter : public IRegistry<std::string>
{
public:
    virtual ~IMessageRouter() {}
};
