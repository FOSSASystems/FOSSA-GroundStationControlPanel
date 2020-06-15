#ifndef SERVICE_H
#define SERVICE_H

#include "Interfaces/IService.h"

#include "Components/Interpreter.h"
#include "Components/MessageLog.h"
#include "Components/Settings.h"

class FOSSAService : public IService
{
public:
    FOSSAService()
    {
        Initialize();
    }

    virtual ~FOSSAService()
    {
        Finialize();
    }

    static Interpreter* GetInterpreter()
    {
        return m_interpreter;
    }

    static MessageLog* GetMessageLog()
    {
        return m_messageLog;
    }

    static Settings* GetSettings()
    {
        return m_settings;
    }
private:
    virtual void Initialize() override
    {
        m_settings = new Settings();
        m_interpreter = new Interpreter();
        m_messageLog = new MessageLog();
    }

    virtual void Finialize() override
    {
        delete m_interpreter;
        delete m_messageLog;
        delete m_settings;
    }

    static Interpreter* m_interpreter;
    static MessageLog* m_messageLog;
    static Settings* m_settings;
};

#endif // SERVICE_H
