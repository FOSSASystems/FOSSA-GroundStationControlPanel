#ifndef MESSAGELOGFRAME_H
#define MESSAGELOGFRAME_H

#include <QWidget>
#include <QStringListModel>

namespace Ui {
class MessageLogFrame;
}

class MessageLogFrame : public QWidget
{
    Q_OBJECT

public:
    explicit MessageLogFrame(QWidget *parent = nullptr);
    ~MessageLogFrame();

public slots:
    void ReceivedMessageLogged(char* timestamp, char dirBit, char opId, char* payload); // attached to MessageLog.h
private:
    Ui::MessageLogFrame *ui;
    QStringListModel * m_messageLogListModel;
};

#endif // MESSAGELOGFRAME_H
