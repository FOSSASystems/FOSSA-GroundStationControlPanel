#pragma once

#include <string>

template <class T>
class IRegistry
{
public:
    virtual ~IRegistry() {}
    virtual void Register(std::string key, T* obj) = 0;
    virtual void Unregister(std::string key) = 0;
    virtual T* Find(std::string key) = 0;
};
