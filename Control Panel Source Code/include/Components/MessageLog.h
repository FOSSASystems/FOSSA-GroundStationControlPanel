#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include "Interfaces/IGroundStationSerialMessage.h"

#include <queue>
#include <chrono>
#include <ctime>

#include <QObject>

/// This acts as a middleground for GUI objects to subscribe to.
class MessageLog : public QObject
{
    Q_OBJECT
public:
    ~MessageLog() {}

    void PushMessage(IGroundStationSerialMessage* msg)
    {
        emit MessageLogged(msg);
    }
signals:
    void MessageLogged(IGroundStationSerialMessage* msg);
};


#endif // MESSAGELOG_H
