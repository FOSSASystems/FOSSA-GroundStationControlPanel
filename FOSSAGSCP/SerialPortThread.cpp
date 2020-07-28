#include "SerialPortThread.h"
#include <QDebug>

SerialPortThread::SerialPortThread(QObject *parent) :
    QThread(parent)
{
    // attach the ready read event of the serial port to our local slot.
    connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialPortThread::HandleReadyRead);
    connect(&m_serialPort, &QSerialPort::bytesWritten, this, &SerialPortThread::HandleBytesWritten);
}

SerialPortThread::~SerialPortThread()
{
    m_serialPort.close();
}

void SerialPortThread::Open()
{
    if (m_serialPort.isOpen()) this->Close();

    m_serialPort.setPortName(m_portName);
    m_serialPort.setBaudRate(m_portBaudRate);

    if (!m_serialPort.open(QIODevice::ReadWrite))
    {
        emit HandleError(tr("Can't open %1, error code %2")
                   .arg(m_portName).arg(m_serialPort.error()));
        return;
    }
}

void SerialPortThread::Close()
{
    m_serialPort.close();
}

/*
    // test that the serial ports mutex works.
    // run 2 commands back to back.
    QString a("a");
    QString b("b");
    QByteArray firstMessage = a.toUtf8();
    QByteArray secondMessage = b.toUtf8();

    while (!m_serialPortThread.Write(firstMessage))
    {
        // serial port is busy, wait.
        //  Sleep(20);
        qInfo("Wrote first message");
    }

    while (!m_serialPortThread.Write(secondMessage))
    {
        // busy wait
        Sleep(20);
        qInfo("Wrote second message");
    }
*/
/**
 * @brief SerialPortThread::Write
 * @param data
 * @return
 */
bool SerialPortThread::Write(const QByteArray& data)
{
    // if this function is called twice then we need to make sure
    // that the serial port.write() is called one at a time.

    bool aquiredLock = m_writeMutex.tryLock();

    if (aquiredLock)
    {
        const qint64 bytesWritten = m_serialPort.write(data);
        m_writeMutex.unlock();
        return true;
    }
    else
    {
        return false; // if we did not get the lock then tell the calling code to wait.
    }


}

void SerialPortThread::SetPortName(QString portName)
{
    m_portName = portName;
}

void SerialPortThread::SetBaudRate(int baudRate)
{
    m_portBaudRate = baudRate;
}



// when the serial port is ready for data, forward the private signal to the HandleRead public signal.
void SerialPortThread::HandleReadyRead()
{
    QByteArray portData = m_serialPort.readAll();
    m_serialPort.clear();
    emit HandleRead(portData);
}

void SerialPortThread::HandleBytesWritten(qint64 bytes)
{

}
