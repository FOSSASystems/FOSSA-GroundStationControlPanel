#ifndef SerialPortThread_H
#define SerialPortThread_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QTime>

#include "Service.h"

class SerialPortThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialPortThread(QObject* parent = nullptr);

    ~SerialPortThread();

    // move data into the thread.
    void Transaction(const QString& portName, uint32_t waitTimeout, const QString& request);
signals:
    // move data out of the thread.
    void Response(const QString& response);
    void Error(const QString& str);
    void Timeout(const QString& str);
private:
    void run() override;

    QString m_portName;
    QString m_request;
    uint32_t m_waitTimeout = 0;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_quit = false;
};

#endif // SerialPortThread_H
