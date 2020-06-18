#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <string.h>

#include <QtCore>

#include <QtWidgets>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDir>

#include <FOSSA-Comms.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString path = QDir::currentPath();

    // Load key from key.txt
    bool keyLoaded = FOSSAService::GetSettings()->LoadKeyFromFile();
    if (keyLoaded)
    {
        FOSSAService::GetSettings()->SetKeySet();
    }

    // Load password for password.txt
    bool passwordLoaded = FOSSAService::GetSettings()->LoadPasswordFromFile();
    if (passwordLoaded)
    {
        FOSSAService::GetSettings()->SetPasswordSet();
    }

    // Set port name.
    FOSSAService::GetSettings()->SetPortName("TestPort1");

    // data piping from serial thread to gui thread
    connect(&this->m_serialPortThread, &SerialPortThread::Response, this, &MainWindow::ResponseReceived, Qt::AutoConnection);
    connect(&this->m_serialPortThread, &SerialPortThread::Error, this, &MainWindow::ErrorReceived, Qt::AutoConnection);
    connect(&this->m_serialPortThread, &SerialPortThread::Timeout, this, &MainWindow::TimeoutReceived, Qt::AutoConnection);

    // load the system information pane
    m_sytemInfoPane = new systeminformationpane();
    m_sytemInfoPane->setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);
    m_sytemInfoPane->show();
}

MainWindow::~MainWindow()
{
    delete m_sytemInfoPane;
    delete ui;
}

// Response received is a slot, this slot is signalled from the serial port thread.
void MainWindow::ResponseReceived(const QString& response)
{
    QByteArray arr = response.toLocal8Bit();
    const char* respData = arr.data();

    // interpret the serial data into the message.
    IGroundStationSerialMessage* msg = FOSSAService::GetInterpreter()->SerialData_To_GroundStationSerialMessage(respData, arr.length());

    // push the message to the log panel.
    FOSSAService::GetMessageLog()->PushMessage(msg);

    // interpret the received message into function calls.
    FOSSAService::GetInterpreter()->Interpret_Received_Message(msg, FOSSAService::GetSettings());

    delete msg;
}

// The IGroundStationSerialMessage object is created in Interpreter.h  function name: Create_GroundStationSerialMessage()
void MainWindow::SendSerialData(IGroundStationSerialMessage* msg)
{
    // log the message.
    FOSSAService::GetMessageLog()->PushMessage(msg);

    // get the string.
    const char* cmdData = msg->GetRawData().c_str();

    // send the message to the ground station via serial.
    this->m_serialPortThread.Transaction(FOSSAService::GetSettings()->GetPortName(), 250, cmdData);

    delete msg;
}





void MainWindow::ErrorReceived(const QString &str)
{
    throw std::runtime_error(str.toUtf8());
}

void MainWindow::TimeoutReceived(const QString &str)
{
    throw std::runtime_error(str.toUtf8());
}






void MainWindow::on_actionView_Serial_Ports_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Click show details to see all available serial ports.");

    QString str;

    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for (auto iter = list.begin(); iter != list.end(); iter++)
    {
        QSerialPortInfo info = *iter;
        str.append(info.portName());

        str.append("\r\n\t");
        str.append(info.serialNumber());

        str.append("\r\n\t");
        str.append(info.description());

        str.append("\r\n\t");
        str.append(info.systemLocation());

        str.append("\r\n\tIs Busy = ");
        str.append(info.isBusy());
        str.append("\r\n");
        str.append("\r\n");
        str.append("\r\n");
    }

    msgBox.setDetailedText(str);

    msgBox.exec();
}
