#ifndef SerialPortThread_H
#define SerialPortThread_H

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QByteArray>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QTime>
#include <iostream>

class SerialPortThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialPortThread(QObject* parent = nullptr);
    ~SerialPortThread();

public slots:
    void Open();
    void Close();

    void SetPortName(QString portName);
    void SetBaudRate(int baudRate);

    void Write(const QByteArray& data); // move data into the thread.
signals:
    void HandleError(const QString& str);    // move data out of the thread.
    void HandleTimeout(const QString& str);
    void HandleRead(QByteArray data);
private slots:
    void HandleBytesWritten(qint64 bytes);
    void HandleReadyRead();
private:
    QString m_portName;
    int m_portBaudRate;
    QSerialPort m_serialPort;

    QByteArray m_receivedDataBuffer;
};

#endif // SerialPortThread_H
