#ifndef SYSTEMINFORMATIONPANE_H
#define SYSTEMINFORMATIONPANE_H

#include <QWidget>

#include <Interpreter.h>
#include "src/Settings.h"

namespace Ui {
class systeminformationpane;
}

class systeminformationpane : public QWidget
{
    Q_OBJECT
public:
    systeminformationpane(QWidget *parent = nullptr);
    ~systeminformationpane();

public:
    Ui::systeminformationpane* GetUI() {
        return ui;
    }
private:
    Ui::systeminformationpane *ui;
signals:
    void SendDataFromSystemInformationPane(Datagram datagram);
private slots:
    void on_LiveStatistics_Request_PushButton_clicked();
    void on_SystemInformation_RecordSolarCells_PushButton_clicked();
    void on_SystemInformation_RequestSystemInformation_PushButton_clicked();
    void on_SystemInformation_RequestFullSystemInformation_PushButton_clicked();
    void on_PacketInformation_Controls_RequestPacketInfo_PushButton_clicked();
};

#endif // SYSTEMINFORMATIONPANE_H
