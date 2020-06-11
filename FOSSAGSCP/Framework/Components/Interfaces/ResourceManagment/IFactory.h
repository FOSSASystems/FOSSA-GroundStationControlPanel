#pragma once


namespace FOSSA
{

template <class T>
class IFactory
{
public:
    virtual ~IFactory() {}
    virtual T* Create() = 0;
    virtual void Dispose(T* obj) = 0;
};

}
