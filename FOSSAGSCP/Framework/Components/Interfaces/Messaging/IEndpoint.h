#pragma once

class IEndpoint
{
public:
    virtual ~IEndpoint() {}
    virtual void Write() {}
    virtual void Read() {}
};
