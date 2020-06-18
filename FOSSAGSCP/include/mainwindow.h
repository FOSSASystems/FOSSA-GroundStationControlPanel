#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SerialPortThread.h"
#include "systeminformationpane.h"
#include "messagelogframe.h"

#include "Interfaces/IGroundStationSerialMessage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SendSerialData(IGroundStationSerialMessage* datagram);
private slots:
    void on_actionView_Serial_Ports_triggered();

    // receive from serial port.
    void ResponseReceived(const QString& response);
    void ErrorReceived(const QString& str);
    void TimeoutReceived(const QString& str);

    // controls tab
    void on_baseOpsPingButton_clicked();

    void on_baseOpsDeploybutton_clicked();

private:
    Ui::MainWindow *ui;
    systeminformationpane *m_sytemInfoPane;
    MessageLogFrame *m_messageLogFrame;

    FOSSAService m_service;

    // serial port thread.
    SerialPortThread m_serialPortThread;
};
#endif // MAINWINDOW_H
