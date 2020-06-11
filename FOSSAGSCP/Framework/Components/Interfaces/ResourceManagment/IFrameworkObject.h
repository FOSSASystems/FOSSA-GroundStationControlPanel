#pragma once

#include "IContextObject.hpp"
#include "IRegistry.hpp"
#include "IService.hpp"

class IFrameworkObject : public IContextObject , public IRegistry<IService>
{
public:
    virtual ~IFrameworkObject() {}
};
