#ifndef FOSSASAT1B_MESSAGEHANDLER_H
#define FOSSASAT1B_MESSAGEHANDLER_H

#include <Interpreter.h>

#include "../systeminformationpane.h"

namespace FOSSASAT1B {

class MessageHandler
{
public:
    static void Handle(FOSSASAT1B::Messages::SystemInfo systemInfo, systeminformationpane* systemInfoPane);
};

}

#endif // MESSAGEHANDLER_H
