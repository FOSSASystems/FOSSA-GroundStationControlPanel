#pragma once

#include "../Components/ResourceManagment/IFrameworkObject.hpp"

namespace FOSSA
{

class MessagingFrameworkObject : public IFrameworkObject
{
public:
    virtual ~MessagingFrameworkObject() {}

    // IRegistry interface
    virtual void Register(std::string key, IService* obj);
    virtual void Unregister(std::string key);
    virtual IService *Find(std::string key);
};

}
