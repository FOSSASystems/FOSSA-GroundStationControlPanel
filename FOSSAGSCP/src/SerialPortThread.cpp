#include "SerialPortThread.h"

SerialPortThread::SerialPortThread(QObject *parent) :
    QThread(parent)
{
}

SerialPortThread::~SerialPortThread()
{
    m_mutex.lock();
    m_quit = true;
    m_condition.wakeOne();
    m_mutex.unlock();
    wait();
}

void SerialPortThread::Transaction(const QString& portName, uint32_t waitTimeout, const QString& request)
{
    const QMutexLocker lock(&m_mutex);
    m_portName = portName;
    m_waitTimeout = waitTimeout;
    m_request = request;

    if (!isRunning())
    {
        start();
    }
    else
    {
        m_condition.wakeOne();
    }
}

void SerialPortThread::run()
{
    bool currentPortNameChanged = false;

    m_mutex.lock();

    QString currentPortName;
    if (currentPortName != m_portName)
    {
        currentPortName = m_portName;
        currentPortNameChanged = true;
    }


    uint32_t currentWaitTimeout = m_waitTimeout;
    QString currentRequest = m_request;

    m_mutex.unlock();



    QSerialPort serial;
    if (currentPortName.isEmpty())
    {
        emit Error("No port name specified.");
    }



    while (!m_quit)
    {
        if (currentPortNameChanged)
        {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite))
            {
                emit Error(tr("Can't open %1, error code %2")
                           .arg(m_portName).arg(serial.error()));
                return;
            }
        }


        const QByteArray requestData = currentRequest.toLocal8Bit();
        serial.write(requestData);

        if (serial.waitForBytesWritten(m_waitTimeout))
        {
            if (serial.waitForReadyRead(currentWaitTimeout))
            {
                QString responseData = serial.readAll();

                while (serial.waitForReadyRead(10))
                {
                    responseData += serial.readAll();
                }

                emit Response(responseData);
            }
            else
            {
                emit Timeout(tr("Wait read response timeout %1").arg(QTime::currentTime().toString()));
            }
        }
        else
        {
            emit Timeout(tr("Wait write request timeout %1").arg(QTime::currentTime().toString()));
        }

        // locks until the Transaction() function is called.
        m_mutex.lock();
        m_condition.wait(&m_mutex);

        if (currentPortName != m_portName)
        {
            currentPortName = m_portName;
            currentPortNameChanged = true;
        }
        else
        {
            currentPortNameChanged = false;
        }

        currentWaitTimeout = m_waitTimeout;
        currentRequest = m_request;

        m_mutex.unlock();
    }
}
