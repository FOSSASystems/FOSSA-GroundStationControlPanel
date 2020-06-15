#ifndef SERVICE_H
#define SERVICE_H

#include "Interfaces/IService.h"

#include "Interpreter.h"

class Service : public IService
{
public:
    Service()
    {
        Initialize();
    }

    virtual ~Service()
    {
        Finialize();
    }

    virtual void Initialize() override
    {
        m_interpreter = new Interpreter();
    }

    virtual void Finialize() override
    {
        delete m_interpreter;
        delete m_channel;
    }

    static IInterpreter* GetInterpreter()
    {
        return m_interpreter;
    }
private:
    static IInterpreter* m_interpreter;
};

#endif // SERVICE_H
