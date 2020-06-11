#pragma once

namespace FOSSA
{

class IActiveRecord
{
public:
    virtual ~IActiveRecord() = 0;
    virtual void Update() = 0;
    virtual void Delete() = 0;
};

}
