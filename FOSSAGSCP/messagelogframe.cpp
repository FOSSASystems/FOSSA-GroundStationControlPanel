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
}

void MessageLogFrame::ReceivedMessageLogged(char* timestamp, char dirBit, char opId, char* payload)
{
    m_messageLogListModel->insertRow(m_messageLogListModel->rowCount());
    QModelIndex row = m_messageLogListModel->index(m_messageLogListModel->rowCount() - 1, 0);
    m_messageLogListModel->setData(row, payload);
}

MessageLogFrame::~MessageLogFrame()
{
    delete ui;
}
