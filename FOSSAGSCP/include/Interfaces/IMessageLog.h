#ifndef IMESSAGELOG_H
#define IMESSAGELOG_H

class IGroundStationSerialMessage;
class IMessageLog
{
public:
    virtual ~IMessageLog() {}
    virtual void PushMessage(IGroundStationSerialMessage* msg) = 0;
};

#endif // IMESSAGELOG_H
