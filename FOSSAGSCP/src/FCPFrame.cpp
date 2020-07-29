#include "FCPFrame.h"

FCPFrame::FCPFrame(uint8_t *data, uint32_t length)
    : m_data(data), m_length(length)
{

}

FCPFrame::~FCPFrame()
{
    if (m_data != nullptr)
    {
        delete[] m_data;
    }
}

FCPFrame::FCPFrame(const FCPFrame &other)
{
    m_length = other.m_length;

    delete[] m_data;

    if (m_length > 0)
    {
        m_data = new uint8_t[m_length];
        memcpy(m_data, other.m_data, m_length);
    }
}

QString FCPFrame::ToString()
{
    QString frameStr;

    for (int i = 0; i < m_length; i++)
    {
        uint8_t v = m_data[i];

        char hexChar[5];
        hexChar[4] = '\0';
        sprintf(&(hexChar[0]), "%02x, ", v);
        frameStr.append(hexChar);
    }

    return frameStr;
}

uint32_t FCPFrame::GetLength() const
{
    return m_length;
}

uint8_t * const FCPFrame::GetData() const
{
    return m_data;
}
