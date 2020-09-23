#include "systeminformationpane.h"
#include "ui_systeminformationpane.h"
#include <Encoder.h>

systeminformationpane::systeminformationpane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::systeminformationpane)
{
    ui->setupUi(this);
}

systeminformationpane::~systeminformationpane()
{
    delete ui;
}

//////////////////////////////////////
// Satellite system information tab //
//////////////////////////////////////
#define SatelliteStatsTab_Start {

void systeminformationpane::on_SystemInformation_RequestFullSystemInformation_PushButton_clicked()
{
    OutboundDatagram datagram = Encoder::Create_CMD_Get_Full_System_Info();
    emit this->SendDataFromSystemInformationPane(datagram);
}

void systeminformationpane::on_SystemInformation_RequestSystemInformation_PushButton_clicked()
{
    OutboundDatagram datagram = Encoder::Create_CMD_Transmit_System_Info();
    emit this->SendDataFromSystemInformationPane(datagram);
}

void systeminformationpane::on_SystemInformation_RecordSolarCells_PushButton_clicked()
{
    char numSamples = ui->SystemInformation_RecordSolarCells_NumSamples_SpinBox->value();
    uint16_t samplingPeriod = ui->SystemInformation_RecordSolarCells_NumSamples_SpinBox->value();

    OutboundDatagram  datagram = Encoder::Create_CMD_Record_Solar_Cells(numSamples, samplingPeriod);
    emit this->SendDataFromSystemInformationPane(datagram);
}


#define SatelliteStatsTab_End }





////////////////////////////
// Packet information tab //
///////////////////////////
#define SatellitePacketTab_Start {

void systeminformationpane::on_PacketInformation_Controls_RequestPacketInfo_PushButton_clicked()
{
    OutboundDatagram  datagram = Encoder::Create_CMD_Get_Packet_Info();
    emit this->SendDataFromSystemInformationPane(datagram);
}

#define SatellitePacketTab_End }







////////////////////////////////
// Statistics information tab //
////////////////////////////////
#define SatelliteStatsTab_Start {


void systeminformationpane::on_LiveStatistics_Request_PushButton_clicked()
{
    // fossasat-1b
    char bChargingVoltage = ui->StatisticsControls_Flags_1B_ChargingVoltage_Enable->isChecked();
    char bChargingCurrent = ui->StatisticsControls_Flags_1B_ChargingCurrent_Enabled->isChecked();
    char bBatteryVoltage = ui->StatisticsControls_Flags_1B_BatteryVoltage_Enabled->isChecked();
    char bSolarCellAVoltage = ui->StatisticsControls_Flags_1B_SolarCellA_Enabled->isChecked();
    char bSolarCellBVoltage = ui->StatisticsControls_Flags_1B_SolarCellB_Enabled->isChecked();
    char bSolarCellCVoltage = ui->StatisticsControls_Flags_1B_SolarCellC_Enbled->isChecked();
    char bBatteryTemperature = ui->StatisticsControls_Flags_1B_BatteryTemperature_Enabled->isChecked();
    char bBoardTemperature = ui->StatisticsControls_Flags_1B_BoardTemperature_Enabled->isChecked();

    char flagsB = bChargingVoltage;
    flagsB = flagsB | (bChargingCurrent << 1);
    flagsB = flagsB | (bBatteryVoltage << 2);
    flagsB = flagsB | (bSolarCellAVoltage << 3);
    flagsB = flagsB | (bSolarCellBVoltage << 4);
    flagsB = flagsB | (bSolarCellCVoltage << 5);
    flagsB = flagsB | (bBatteryTemperature << 6);
    flagsB = flagsB | (bBoardTemperature << 7);

    // fossasat-2
    char temperatures = ui->StatisticsControls_Flags_2_Temperatures_Enabled->isChecked();
    char currents = ui->StatisticsControls_Flags_2_Currents_Enabled->isChecked();
    char voltages = ui->StatisticsControls_Flags_2_Voltages_Enabled->isChecked();
    char lightSensors = ui->StatisticsControls_Flags_2_LightSensors_Enabled->isChecked();
    char imu = ui->StatisticsControls_Flags_2_IMU_Enabled->isChecked();

    char flags = temperatures;
    flags = flags | (currents << 1);
    flags = flags | (voltages << 2);
    flags = flags | (lightSensors << 3);
    flags = flags | (imu << 4);

    /// @todo add version switch
    OutboundDatagram datagram = Encoder::Create_CMD_Get_Statistics(flags);
    emit this->SendDataFromSystemInformationPane(datagram);
}



#define SatelliteStatsTab_Start }

