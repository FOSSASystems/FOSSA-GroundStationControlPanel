#ifndef MESSAGELOGFRAME_H
#define MESSAGELOGFRAME_H

#include <QWidget>
#include <QStringListModel>
#include <QFileDialog>

#include <chrono>
#include <ctime>

#include "src/IDatagram.h"

namespace Ui {
class MessageLogFrame;
}

class MessageLogFrame : public QWidget
{
    Q_OBJECT

public:
    explicit MessageLogFrame(QWidget *parent = nullptr);
    ~MessageLogFrame();

    bool GetEnableSerialSniffingState();
    void RawWriteToLog(QString msg);

public slots:
    void WriteDatagram(const IDatagram* datagram); // attached to MessageLog.h
signals:
    void SendDataFromMessageLogFrame(QString data);
private slots:
    void on_saveAsButton_clicked();
    void on_messageSendButton_clicked();

    void on_enableSerialSniffingCheckBox_stateChanged(int arg1);
    void on_enableTimestampsCheckBox_stateChanged(int arg1);


private:
    Ui::MessageLogFrame *ui;
    QStringListModel * m_messageLogListModel;

    bool m_logTimestamps = false;
    bool m_enableSerialSniffing = false;
};

#endif // MESSAGELOGFRAME_H
