#pragma once

#include <cstdint>
#include <queue>

namespace FOSSA
{

template <class T>
class IBuffer
{
public:
    virtual ~IBuffer() {}

    virtual void Enqueue(T* obj) = 0;
    virtual T* Dequeue() = 0;

    virtual void Clear() = 0;
    virtual uint32_t Count() = 0;
};
}
