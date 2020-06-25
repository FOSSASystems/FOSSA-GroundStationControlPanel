#ifndef SerialPortThread_H
#define SerialPortThread_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QTime>

class SerialPortThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialPortThread(QString& serialPortName, QObject* parent = nullptr);
    ~SerialPortThread();

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
    QString m_request;
    uint32_t m_waitTimeout = 0;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_quit = false;

    QSerialPort m_serialPort;
};

#endif // SerialPortThread_H
