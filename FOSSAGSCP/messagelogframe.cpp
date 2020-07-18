#include "messagelogframe.h"
#include "ui_messagelogframe.h"

MessageLogFrame::MessageLogFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageLogFrame)
{
    ui->setupUi(this);

    ui->messageLogListView->setViewMode(QListView::ViewMode::ListMode);

    m_messageLogListModel = new QStringListModel(ui->messageLogListView);
    ui->messageLogListView->setModel(m_messageLogListModel);
    ui->messageLogListView->setAutoScroll(true);
}

void MessageLogFrame::ReceivedMessageLogged(IGroundStationSerialMessage* msg)
{
    // insert a new empty row.
    m_messageLogListModel->insertRow(m_messageLogListModel->rowCount());
    QModelIndex row = m_messageLogListModel->index(m_messageLogListModel->rowCount() - 1, 0);

    // fill a string with data.
    auto timestamp = std::chrono::system_clock::now();
    auto timenow = std::chrono::system_clock::to_time_t(timestamp);
    char* timestampString = std::ctime(&timenow);

    char* payload = msg->GetPayload();
    uint8_t payloadLength = msg->GetPayloadLengthByte();

    QString loggedMessage = QString("");
    if (m_logTimestamps)
    {
        loggedMessage += timestampString;
    }

    // control byte as hex character string
    char controlByteHexStr[3];
    char controlByte = msg->GetControlByte();
    sprintf(&(controlByteHexStr[0]), "%02x", (uint8_t)controlByte);

    loggedMessage += controlByteHexStr;

    loggedMessage += ' ';

    // length byte as hex character string.
    char lengthByteHexStr[2];
    char payloadLengthByte = msg->GetPayloadLengthByte();
    sprintf(&(lengthByteHexStr[0]), "%02x", (uint8_t)payloadLengthByte);
    loggedMessage += lengthByteHexStr;

    loggedMessage += " ";

    if (payload != nullptr)
    {
        for (int i = 0; i < payloadLength; i++)
        {
            char hexChar[2];
            char rawData = payload[i];
            sprintf(&(hexChar[0]), "%02x", (uint8_t)rawData);
            loggedMessage.push_back(hexChar[0]);
            loggedMessage.push_back(hexChar[1]);
            loggedMessage += " ";
        }
    }

    // put the data in the new empty row.
    std::string logMessageStdString = loggedMessage.toStdString();

    const char * msgTolog = logMessageStdString.c_str();
    m_messageLogListModel->setData(row, msgTolog);

    // make sure we are always at the most recent row.
    ui->messageLogListView->scrollToBottom();

    // default serial sniffing to enabled.
    m_enableSerialSniffing = true;
    m_logTimestamps = true;
}

MessageLogFrame::~MessageLogFrame()
{
    delete ui;
}

bool MessageLogFrame::GetEnableSerialSniffingState()
{
    return m_enableSerialSniffing;
}

void MessageLogFrame::RawWriteToLog(QString msg)
{
    // insert a new empty row.
    m_messageLogListModel->insertRow(m_messageLogListModel->rowCount());
    QModelIndex row = m_messageLogListModel->index(m_messageLogListModel->rowCount() - 1, 0);

    m_messageLogListModel->setData(row, msg);
}

void MessageLogFrame::on_enableTimestampsCheckBox_stateChanged(int arg1)
{
    if (arg1 == 0)
    {
        m_logTimestamps = false;
    }
    else
    {
        m_logTimestamps = true;
    }
}

void MessageLogFrame::on_enableSerialSniffingCheckBox_stateChanged(int arg1)
{
    if (arg1 == 0)
    {
        m_enableSerialSniffing = false;
    }
    else
    {
        m_enableSerialSniffing = true;
    }
}


void MessageLogFrame::on_saveAsButton_clicked()
{
    QStringList log = m_messageLogListModel->stringList();

    QString outputLog = log.join("\r\n");
    std::string outputLogStr = outputLog.toStdString();

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::FileMode::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec())
    {
        QStringList filenames = dialog.selectedFiles();

        for (int i = 0; i < filenames.size(); i++)
        {
            QFile file(filenames[i]);

            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                file.write(outputLogStr.c_str());
            }
        }
    }
}


void MessageLogFrame::on_messageSendButton_clicked()
{
    QString messageToSend = ui->messageSubmitLineEdit->text();
    this->SendDataFromMessageLogFrame(messageToSend);
}
