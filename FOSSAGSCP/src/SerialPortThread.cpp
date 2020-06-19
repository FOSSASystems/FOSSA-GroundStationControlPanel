#include "SerialPortThread.h"

SerialPortThread::SerialPortThread(QString& serialPortName, QObject *parent) :
    QThread(parent)
{
    m_serialPort.setPortName(serialPortName);
    if (!m_serialPort.open(QIODevice::ReadWrite))
    {
        emit HandleError(tr("Can't open %1, error code %2")
                   .arg(m_portName).arg(m_serialPort.error()));
        return;
    }

    // attach the ready read event of the serial port to our local slot.
    connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialPortThread::HandleReadyRead);
    connect(&m_serialPort, &QSerialPort::bytesWritten, this, &SerialPortThread::HandleBytesWritten);
}

SerialPortThread::~SerialPortThread()
{
    m_serialPort.close();
}

// When the owning thread wants to write data to the serial port, this funciton is used directly.
void SerialPortThread::Write(const QByteArray& data)
{
    const qint64 bytesWritten = m_serialPort.write(data);
}

// when the serial port is ready for data, forward the private signal to the HandleRead public signal.
void SerialPortThread::HandleReadyRead()
{
    emit HandleRead(m_serialPort.readAll());
}

void SerialPortThread::HandleBytesWritten(qint64 bytes)
{

}
