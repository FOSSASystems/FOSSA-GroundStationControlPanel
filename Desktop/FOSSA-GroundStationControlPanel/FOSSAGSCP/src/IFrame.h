#ifndef IFRAME_H
#define IFRAME_H

#include <QString>

class IFrame
{
public:
    virtual ~IFrame();
    virtual QString ToString() = 0;
    virtual uint32_t GetLength() const = 0;
    virtual uint8_t* const GetData() const = 0;
};

#endif // IFRAME_H
