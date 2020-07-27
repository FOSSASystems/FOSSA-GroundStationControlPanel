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



//////////////////////////////////////
// Satellite system information tab //
//////////////////////////////////////
#define SatelliteStatsTab_Start {

void systeminformationpane::on_SystemInformation_RequestFullSystemInformation_clicked()
{
    IGroundStationSerialMessage* msg = m_interpreter->Create_CMD_Get_Full_System_Info();
    this->SendDataFromSystemInformationPane(msg);
}

void systeminformationpane::on_SystemInformation_RequestSystemInformation_PushButton_clicked()
{

}

#define SatelliteStatsTab_End }





////////////////////////////
// Packet information tab //
///////////////////////////
#define SatellitePacketTab_Start {
#define SatellitePacketTab_Start }







////////////////////////////////
// Statistics information tab //
////////////////////////////////
#define SatelliteStatsTab_Start {


void systeminformationpane::on_LiveStatistics_Request_PushButton_clicked()
{
    IGroundStationSerialMessage* msg = m_interpreter->Create_CMD_Get_Statistics()
}


#define SatelliteStatsTab_Start }
