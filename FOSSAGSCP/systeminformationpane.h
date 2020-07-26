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
};

#endif // SYSTEMINFORMATIONPANE_H
