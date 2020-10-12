#ifndef FOSSASAT2_MESSAGEHANDLER_H
#define FOSSASAT2_MESSAGEHANDLER_H

#include <Interpreter.h>

class systeminformationpane;
class MainWindow;

namespace FOSSASAT2 {

class MessageHandler
{
public:
    static void Handle(FOSSASAT2::Messages::SystemInfo msg, systeminformationpane* systemInfoPane);
    static void Handle(FOSSASAT2::Messages::PacketInfo msg, systeminformationpane* systemInfoPane);
    static void Handle(FOSSASAT2::Messages::Statistics msg, systeminformationpane* systemInfoPane);
    static void Handle(FOSSASAT2::Messages::FullSystemInfo msg, systeminformationpane* systemInfoPane);
    static void Handle(FOSSASAT2::Messages::DeploymentState msg, systeminformationpane* systemInfoPane);
    static void Handle(FOSSASAT2::Messages::RecordedIMU msg, systeminformationpane* systemInfoPane);
    static void Handle(FOSSASAT2::Messages::GPSLogState msg, MainWindow* mainWindow);
};

}


#endif // MESSAGEHANDLER_H
