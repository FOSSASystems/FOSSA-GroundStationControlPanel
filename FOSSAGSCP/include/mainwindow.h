#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SerialPortThread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SendSerialData(QString& datagram);
private slots:
    void on_actionView_Serial_Ports_triggered();

    // receive from serial port.
    void ResponseReceived(const QString& response);
    void ErrorReceived(const QString& str);
    void TimeoutReceived(const QString& str);

private:
    Ui::MainWindow *ui;

    // AES KEY
    void LoadKeyFromFile();
    uint8_t m_key[16];

    // Datagram service.
    Service m_service;

    // serial port thread.
    SerialPortThread m_serialPortThread;

    // selected port name.
    QString m_portName;

};
#endif // MAINWINDOW_H
