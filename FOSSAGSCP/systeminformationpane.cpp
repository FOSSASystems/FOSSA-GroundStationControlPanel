#include "systeminformationpane.h"
#include "ui_systeminformationpane.h"

systeminformationpane::systeminformationpane(QWidget *parent, Interpreter* interpreter) :
    QWidget(parent),
    ui(new Ui::systeminformationpane)
{
    ui->setupUi(this);
    m_interpreter = interpreter;
}

systeminformationpane::~systeminformationpane()
{
    delete ui;
}

void systeminformationpane::on_SystemInformation_RequestFullSystemInformation_clicked()
{
    IGroundStationSerialMessage* msg = m_interpreter->Create_CMD_Get_Full_System_Info();
    this->SendSerialData(msg);
}
