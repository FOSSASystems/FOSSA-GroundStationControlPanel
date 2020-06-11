#ifndef SERIALPORTREADER_H
#define SERIALPORTREADER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>

class SerialPortReader : public QObject
{
    Q_OBJECT
public:
    SerialPortReader(QSerialPort* serialPort, QObject* parent = nullptr)
        : QObject(parent),
          m_serialPort(serialPort)
    {
        connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPortReader::handleReadyToRead);
        connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialPortReader::handleError);
    };
private slots:
    void handleReadyToRead()
    {
        QByteArray arr = m_serialPort->readAll();
        m_readData.push_back(arr);
    }
    void handleError(QSerialPort::SerialPortError error)
    {
        throw error;
    }
private:
    QSerialPort* m_serialPort = nullptr;
    QByteArray m_readData;
};

#endif // SERIALPORTREADER_H
