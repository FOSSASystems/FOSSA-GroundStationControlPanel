#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include "Interfaces/IMessageLog.h"
#include "Interfaces/IGroundStationSerialMessage.h"

#include <queue>
#include <chrono>
#include <ctime>

#include <QObject>

/// This acts as a middleground for GUI objects to subscribe to.
class MessageLog : public QObject, public IMessageLog
{
    Q_OBJECT
public:
    virtual ~MessageLog() {}

    virtual void PushMessage(IGroundStationSerialMessage* msg) override final
    {
        auto timestamp = std::chrono::system_clock::now();
        auto timenow = std::chrono::system_clock::to_time_t(timestamp);

        char* timestampString = std::ctime(&timenow);

        char directionBit = msg->GetDirectionBit();
        char operationId = msg->GetOperationID();
        char* payload = msg->GetPayload();

        emit MessageLogged(timestampString, directionBit, operationId, payload);
    }
signals:
    void MessageLogged(char* timestamp, char dirBit, char opId, char* payload);
};


#endif // MESSAGELOG_H
