#ifndef MESSAGELOGFRAME_H
#define MESSAGELOGFRAME_H

#include <QWidget>
#include <QStringListModel>
#include <QFileDialog>

namespace Ui {
class MessageLogFrame;
}

class MessageLogFrame : public QWidget
{
    Q_OBJECT

public:
    explicit MessageLogFrame(QWidget *parent = nullptr);
    ~MessageLogFrame();

public slots:
    void ReceivedMessageLogged(char* timestamp, char dirBit, char opId, char* payload); // attached to MessageLog.h
private slots:
    void on_enableTimestampsCheckBox_stateChanged(int arg1);

    void on_saveAsButton_clicked();

private:
    Ui::MessageLogFrame *ui;
    QStringListModel * m_messageLogListModel;
    bool m_logTimestamps = false;
};

#endif // MESSAGELOGFRAME_H
