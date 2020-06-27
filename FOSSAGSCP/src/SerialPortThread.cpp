#include "SerialPortThread.h"

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

// When the owning thread wants to write data to the serial port, this funciton is used directly.
void SerialPortThread::Write(const QByteArray& data)
{
    const qint64 bytesWritten = m_serialPort.write(data);
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
    emit HandleRead(m_serialPort.readAll());
}

void SerialPortThread::HandleBytesWritten(qint64 bytes)
{

}
