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

void MessageLogFrame::ReceivedMessageLogged(char* timestamp, char dirBit, char opId, char* payload)
{
    m_messageLogListModel->insertRow(m_messageLogListModel->rowCount());
    QModelIndex row = m_messageLogListModel->index(m_messageLogListModel->rowCount() - 1, 0);

    std::string loggedMessage;
    if (m_logTimestamps)
    {
        loggedMessage += timestamp;
    }

    loggedMessage += "(dir=";
    loggedMessage += std::to_string(dirBit);
    loggedMessage += ") ";

    loggedMessage += "(op=";
    loggedMessage += std::to_string(opId);
    loggedMessage += ") \r\n";

    loggedMessage += payload;

    const char * msgTolog = loggedMessage.c_str();
    m_messageLogListModel->setData(row, msgTolog);

    ui->messageLogListView->scrollToBottom();
}

MessageLogFrame::~MessageLogFrame()
{
    delete ui;
}

void MessageLogFrame::on_enableTimestampsCheckBox_stateChanged(int arg1)
{
    m_logTimestamps = arg1;
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
