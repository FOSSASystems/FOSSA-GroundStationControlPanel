#ifndef SYSTEMINFORMATIONPANE_H
#define SYSTEMINFORMATIONPANE_H

#include <QWidget>

#include "include/Interpreter.h"

namespace Ui {
class systeminformationpane;
}

class systeminformationpane : public QWidget
{
    Q_OBJECT
public:
    systeminformationpane(QWidget *parent = nullptr, Interpreter* interpreter = nullptr);
    ~systeminformationpane();
    Ui::systeminformationpane *ui;
signals:
    void SendDataFromSystemInformationPane(IGroundStationSerialMessage* message);
private:
    Interpreter* m_interpreter = nullptr;
private slots:
    void on_SystemInformation_RequestFullSystemInformation_clicked();
    void on_SystemInformation_RequestSystemInformation_PushButton_clicked();
    void on_LiveStatistics_Request_PushButton_clicked();
    void on_SystemInformation_RecordSolarCells_PushButton_clicked();
};

#endif // SYSTEMINFORMATIONPANE_H
