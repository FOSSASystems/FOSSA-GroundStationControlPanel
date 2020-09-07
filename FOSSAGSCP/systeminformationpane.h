#ifndef SYSTEMINFORMATIONPANE_H
#define SYSTEMINFORMATIONPANE_H

#include <QWidget>

#include "IDatagram.h"
#include "DatagramInterpreter.h"

namespace Ui {
class systeminformationpane;
}

class systeminformationpane : public QWidget
{
    Q_OBJECT
public:
    systeminformationpane( DatagramInterpreter* interpreter, QWidget *parent = nullptr);
    ~systeminformationpane();

    Ui::systeminformationpane *ui;
signals:
    void SendDataFromSystemInformationPane(IDatagram* datagram);
private:
    DatagramInterpreter* m_interpreter = nullptr;
private slots:
    void on_LiveStatistics_Request_PushButton_clicked();
    void on_SystemInformation_RecordSolarCells_PushButton_clicked();
    void on_SystemInformation_RequestSystemInformation_PushButton_clicked();
    void on_SystemInformation_RequestFullSystemInformation_PushButton_clicked();
    void on_PacketInformation_Controls_RequestPacketInfo_PushButton_clicked();
};

#endif // SYSTEMINFORMATIONPANE_H
