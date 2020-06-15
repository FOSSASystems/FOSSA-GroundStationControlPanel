#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <string.h>

#include <QtCore>

#include <QtWidgets>

#include <QSerialPort>
#include <QSerialPortInfo>








MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->LoadKeyFromFile();

    m_portName = "TestPort1";

    // data piping from serial thread to gui thread
    connect(&this->m_serialPortThread, &SerialPortThread::Response, this, &MainWindow::ResponseReceived, Qt::AutoConnection);
    connect(&this->m_serialPortThread, &SerialPortThread::Error, this, &MainWindow::ErrorReceived, Qt::AutoConnection);
    connect(&this->m_serialPortThread, &SerialPortThread::Timeout, this, &MainWindow::TimeoutReceived, Qt::AutoConnection);

    m_sytemInfoPane = new systeminformationpane();
    m_sytemInfoPane->setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);
    m_sytemInfoPane->show();
}

MainWindow::~MainWindow()
{
    delete m_sytemInfoPane;
    delete ui;
}




void MainWindow::ResponseReceived(const QString& response)
{
    QByteArray arr = response.toLocal8Bit();
    const char* respData = arr.data();

    IDatagram* datagram = Service::GetInterpreter()->MessageToDatagram(respData, response.length());

    // check authentication.
    // log
    // to->signals
}

void MainWindow::SendSerialData(QString& datagram)
{
    this->m_serialPortThread.Transaction(m_portName, 250, datagram);
}





void MainWindow::ErrorReceived(const QString &str)
{
    throw std::runtime_error(str.toUtf8());
}

void MainWindow::TimeoutReceived(const QString &str)
{
    throw std::runtime_error(str.toUtf8());
}






void MainWindow::LoadKeyFromFile()
{
    FILE* f = fopen("key.txt", "r");

    if (f == nullptr)
    {
        throw std::runtime_error("Could not find key.txt");
    }

    for (int i = 0; i < 32; i+=2)
    {
        char asciiCharacterA = getc(f);
        char asciiCharacterB = getc(f);
        char byteHexStr[3];

        byteHexStr[0] = asciiCharacterA;
        byteHexStr[1] = asciiCharacterB;
        byteHexStr[2] = '\0';

        uint8_t hexValueAsByte= (uint8_t)strtol(byteHexStr, NULL, 16);
        this->m_key[i/2] = hexValueAsByte;
    }

    fclose(f);

    ui->statusbar->showMessage("Success: AES Key loaded from key.txt file.", 5000);
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

        str.append("\r\n\tStandard baud rates: ");
        for (auto baudIter = info.standardBaudRates().begin(); baudIter != info.standardBaudRates().end(); baudIter++)
        {
            qint32 baudRate = *baudIter;
            str.append(baudRate);
            str.append(",");
        }
    }

    msgBox.exec();
}
