#ifndef FCPFRAME_H
#define FCPFRAME_H

#include "IFrame.h"

class FCPFrame : public IFrame
{
public:
    FCPFrame(uint8_t* data, uint32_t length);
    virtual ~FCPFrame();
    FCPFrame(const FCPFrame& other);

    virtual QString ToString();
    virtual uint32_t GetLength() const;
    virtual uint8_t *const GetData() const;
private:
    uint8_t m_length;
    uint8_t* m_data = nullptr;
};

#endif // FCPFRAME_H
