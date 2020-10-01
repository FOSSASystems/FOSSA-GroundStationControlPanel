#ifndef FOSSASAT2_MESSAGEHANDLER_H
#define FOSSASAT2_MESSAGEHANDLER_H

#include <Interpreter.h>

#include "../systeminformationpane.h"

namespace FOSSASAT2 {

class MessageHandler
{
public:
    static void Handle(FOSSASAT2::Messages::SystemInfo msg, systeminformationpane* systemInfoPane);
    static void Handle(FOSSASAT2::Messages::PacketInfo msg);
    static void Handle(FOSSASAT2::Messages::Statistics msg);
    static void Handle(FOSSASAT2::Messages::FullSystemInfo msg);
    static void Handle(FOSSASAT2::Messages::DeploymentState msg);
    static void Handle(FOSSASAT2::Messages::RecordedIMU msg);
    static void Handle(FOSSASAT2::Messages::GPSLogState msg);
};

}


#endif // MESSAGEHANDLER_H
