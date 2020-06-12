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

private slots:
    void on_actionView_Serial_Ports_triggered();

private:
    Ui::MainWindow *ui;

    // AES KEY
    void LoadKeyFromFile();
    uint8_t m_key[16];

    // serial port thread.
    SerialPortThread m_serialPortThread;
};
#endif // MAINWINDOW_H
