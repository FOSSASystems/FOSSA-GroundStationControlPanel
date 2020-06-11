#pragma once

namespace FOSSA
{

template <class T_KEY, class T_VALUE>
class IContainer
{
public:
    virtual ~IContainer() {}
    virtual void Insert(T_KEY key, T_VALUE* obj) = 0;
    virtual T_VALUE* Get(T_KEY key) = 0;
};

}
