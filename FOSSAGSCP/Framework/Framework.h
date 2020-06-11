#pragma once

#include "FrameworkObjects/ResourceManagementFrameworkObject.h"
#include "FrameworkObjects/DatabaseFrameworkObject.h"
#include "FrameworkObjects/MessagingFrameworkObject.h"

namespace FOSSA
{

class Framework
{
public:
    Framework() {}
    ~Framework() {}
private:
    ResourceManagementFrameworkObject m_resourceManagementFrameworkObject;
    DatabaseFrameworkObject m_databaseFrameworkObject;
    MessagingFrameworkObject m_messagingFrameworkObject;
};

}
