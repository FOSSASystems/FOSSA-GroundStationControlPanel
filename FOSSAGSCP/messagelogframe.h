#ifndef MESSAGELOGFRAME_H
#define MESSAGELOGFRAME_H

#include <QWidget>
#include <QStringListModel>
#include <QFileDialog>

#include <chrono>
#include <ctime>

#include "Interfaces/IGroundStationSerialMessage.h"

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
    void ReceivedMessageLogged(IGroundStationSerialMessage* msg); // attached to MessageLog.h
signals:
    void SendDataFromMessageLogFrame(QString data);
private slots:
    void on_enableTimestampsCheckBox_stateChanged(int arg1);

    void on_saveAsButton_clicked();

    void on_messageSendButton_clicked();

private:
    Ui::MessageLogFrame *ui;
    QStringListModel * m_messageLogListModel;
    bool m_logTimestamps = false;
};

#endif // MESSAGELOGFRAME_H
