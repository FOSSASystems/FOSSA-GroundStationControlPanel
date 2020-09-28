#include "systeminformationpane.h"
#include "ui_systeminformationpane.h"

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
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Get_Full_System_Info();
        emit this->SendDataFromSystemInformationPane(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        Datagram datagram = FOSSASAT2::DatagramEncoder::Get_Full_System_Info();
        emit this->SendDataFromSystemInformationPane(datagram);
    }
}

void systeminformationpane::on_SystemInformation_RequestSystemInformation_PushButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Transmit_System_Info();
        emit this->SendDataFromSystemInformationPane(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        Datagram datagram = FOSSASAT2::DatagramEncoder::Transmit_System_Info();
        emit this->SendDataFromSystemInformationPane(datagram);
    }
}

void systeminformationpane::on_SystemInformation_RecordSolarCells_PushButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        char numSamples = ui->SystemInformation_RecordSolarCells_NumSamples_SpinBox->value();
        uint16_t samplingPeriod = ui->SystemInformation_RecordSolarCells_NumSamples_SpinBox->value();

        Datagram  datagram = FOSSASAT1B::DatagramEncoder::Record_Solar_Cells(numSamples, samplingPeriod);
        emit this->SendDataFromSystemInformationPane(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {

    }
}


#define SatelliteStatsTab_End }





////////////////////////////
// Packet information tab //
///////////////////////////
#define SatellitePacketTab_Start {

void systeminformationpane::on_PacketInformation_Controls_RequestPacketInfo_PushButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        char bChargingVoltage = ui->StatisticsControls_Flags_1B_ChargingVoltage_Enable->isChecked();
        char bChargingCurrent = ui->StatisticsControls_Flags_1B_ChargingCurrent_Enabled->isChecked();
        char bBatteryVoltage = ui->StatisticsControls_Flags_1B_BatteryVoltage_Enabled->isChecked();
        char bSolarCellAVoltage = ui->StatisticsControls_Flags_1B_SolarCellA_Enabled->isChecked();
        char bSolarCellBVoltage = ui->StatisticsControls_Flags_1B_SolarCellB_Enabled->isChecked();
        char bSolarCellCVoltage = ui->StatisticsControls_Flags_1B_SolarCellC_Enbled->isChecked();
        char bBatteryTemperature = ui->StatisticsControls_Flags_1B_BatteryTemperature_Enabled->isChecked();
        char bBoardTemperature = ui->StatisticsControls_Flags_1B_BoardTemperature_Enabled->isChecked();

        char flags = bChargingVoltage;
        flags |= (bChargingCurrent << 1);
        flags |= (bBatteryVoltage << 2);
        flags |= (bSolarCellAVoltage << 3);
        flags |= (bSolarCellBVoltage << 4);
        flags |= (bSolarCellCVoltage << 5);
        flags |= (bBatteryTemperature << 6);
        flags |= (bBoardTemperature << 7);

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Get_Statistics(flags);
        emit this->SendDataFromSystemInformationPane(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        char temperatures = ui->StatisticsControls_Flags_2_Temperatures_Enabled->isChecked();
        char currents = ui->StatisticsControls_Flags_2_Currents_Enabled->isChecked();
        char voltages = ui->StatisticsControls_Flags_2_Voltages_Enabled->isChecked();
        char lightSensors = ui->StatisticsControls_Flags_2_LightSensors_Enabled->isChecked();
        char imu = ui->StatisticsControls_Flags_2_IMU_Enabled->isChecked();

        char flags = temperatures;
        flags |= (currents << 1);
        flags |= (voltages << 2);
        flags |= (lightSensors << 3);
        flags |= (imu << 4);

        Datagram datagram = FOSSASAT2::DatagramEncoder::Get_Statistics(flags);
        emit this->SendDataFromSystemInformationPane(datagram);
    }
}

#define SatellitePacketTab_End }







////////////////////////////////
// Statistics information tab //
////////////////////////////////
#define SatelliteStatsTab_Start {


void systeminformationpane::on_LiveStatistics_Request_PushButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Get_Packet_Info();
        emit this->SendDataFromSystemInformationPane(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        Datagram datagram = FOSSASAT2::DatagramEncoder::Get_Packet_Info();
        emit this->SendDataFromSystemInformationPane(datagram);
    }
}



#define SatelliteStatsTab_End }

