#ifndef Interpreter_H
#define Interpreter_H

////////////////////////////////
/// INTERPRETER              ///
/// 25/06/2020 - R.G.Bamford ///
/// 07/07/2020 - R.G.Bamford ///
////////////////////////////////

#include <FOSSA-Comms.h>
#include <QObject>
#include <vector>

#include "CStructs.h"

#include "GroundStationSerialMessage.h"
#include "Settings.h"
#include "ui_mainwindow.h"
#include "ui_systeminformationpane.h"

/////////////////////
/// Direction Bit ///
/////////////////////
#define FCPI_DIR_TO_GROUND_STATION  0
#define FCPI_DIR_FROM_GROUND_STATION 1

////////////////////
/// Operation ID ///
////////////////////
//
// Handshake request/response. Sent from control panel to initialize the serial protocol,
// ground station sends this operation ID in response.
//
#define FCPI_HANDSHAKE_OP 0 // handshake

//
// FCP frame transfer. Payload is only the FCP frame to uplink when sent with direction = 0,
// or 2-byte RadioLib reception status code followed by the received FCP frame when sent with direction = 1.
//
#define FCPI_FRAME_OP 1 // fcp frame

//
// Ground station configuration change request/result. Payload is the ground station configuration to set when sent with direction = 0,
// or configuration result when sent with direction = 1 (typically a 2-byte RadioLib status code).
//
#define FCPI_CONFIG_OP 2

#define VERSION_1B QString("FOSSASAT-1B")
#define VERSION_2 QString("FOSSASAT-2")


class Interpreter : public QObject
{
    Q_OBJECT
public:
    Interpreter(Settings& settings, Ui::MainWindow* mainWindowUI, Ui::systeminformationpane* systemInfoPaneUI)
        : m_settings(settings), m_mainWindowUI(mainWindowUI), m_systemInfoUI(systemInfoPaneUI) {}
    virtual ~Interpreter() {}

    /// The ground station serial message is an internal command structure.
    /// This creates it from raw serial datagrams.
    /// @todo checks on the serial data length.
    IGroundStationSerialMessage* SerialData_To_GroundStationSerialMessage(char* serialData, char serialDataLength);

    // this is the base class for all the Create_CMD_XXX methods.
    IGroundStationSerialMessage* Create_GroundStationSerialMessage(char operationId, uint8_t functionId, uint8_t optDataLength, char* optData);

    // set the satellite version
    void SetSatelliteVersion(QString satVersion);


    void Interpret_Received_Message(IGroundStationSerialMessage* inMsg);
    void Interpret_Handshake(IGroundStationSerialMessage* inMsg);
    void Interpret_Config_Change(IGroundStationSerialMessage* inMsg);
    void Interpret_FCP_Frame(IGroundStationSerialMessage* inMsg);


    //////
    /// public unencrypted uplink message commands.
    /////
    IGroundStationSerialMessage* Create_CMD_Ping();
    IGroundStationSerialMessage* Create_CMD_Retransmit();
    IGroundStationSerialMessage* Create_CMD_Retransmit_Custom();
    IGroundStationSerialMessage* Create_CMD_Transmit_System_Info();
    IGroundStationSerialMessage* Create_CMD_Get_Packet_Info();
    IGroundStationSerialMessage *Create_CMD_Get_Statistics(char flags);
    IGroundStationSerialMessage* Create_CMD_Get_Full_System_Info();
    IGroundStationSerialMessage* Create_CMD_Store_And_Forward_Add();
    IGroundStationSerialMessage* Create_CMD_Store_And_Forward_Request();
    IGroundStationSerialMessage* Create_CMD_Request_Public_Picture();

    /////
    /// Private commands (encrypted uplink messages)
    /////
    IGroundStationSerialMessage* Create_CMD_Deploy();
    IGroundStationSerialMessage* Create_CMD_Restart();
    IGroundStationSerialMessage *Create_CMD_Wipe_EEPROM(char flags);
    IGroundStationSerialMessage* Create_CMD_Set_Transmit_Enable(char transmitEnabled, char autoStatsEnabled, char fskMandatedEnabled);
    IGroundStationSerialMessage* Create_CMD_Set_Callsign();
    IGroundStationSerialMessage* Create_CMD_Set_SF_Mode();
    IGroundStationSerialMessage* Create_CMD_Set_MPPT_Mode();
    IGroundStationSerialMessage* Create_CMD_Set_Low_Power_Mode_Enable();
    IGroundStationSerialMessage* Create_CMD_Set_Receive_Windows();
    IGroundStationSerialMessage* Create_CMD_Record_Solar_Cells();
    IGroundStationSerialMessage* Create_CMD_Camera_Capture(char pictureSlot, char lightMode, char pictureSize, char brightness, char saturation, char specialFilter, char contrast);
    IGroundStationSerialMessage* Create_CMD_Set_Power_Limits();
    IGroundStationSerialMessage* Create_CMD_Set_RTC();
    IGroundStationSerialMessage* Create_CMD_Record_IMU();
    IGroundStationSerialMessage* Create_CMD_Run_Manual_ACS();
    IGroundStationSerialMessage* Create_CMD_Log_GPS();
    IGroundStationSerialMessage* Create_CMD_Get_GPS_Log();
    IGroundStationSerialMessage* Create_CMD_Get_Flash_Contents();
    IGroundStationSerialMessage *Create_CMD_Get_Picture_Length();
    IGroundStationSerialMessage* Create_CMD_Get_Picture_Burst(char pictureSlot, uint16_t pictureId, char fullOrScandata);
    IGroundStationSerialMessage* Create_CMD_Route();
    IGroundStationSerialMessage* Create_CMD_Set_Flash_Contents();
    IGroundStationSerialMessage* Create_CMD_Set_TLE();
    IGroundStationSerialMessage* Create_CMD_Get_GPS_Log_State();
    IGroundStationSerialMessage* Create_CMD_Run_GPS_Command();
    IGroundStationSerialMessage* Create_CMD_Set_Sleep_Intervals();
    IGroundStationSerialMessage* Create_CMD_Abort();
    IGroundStationSerialMessage* Create_CMD_Maneuver();
    IGroundStationSerialMessage* Create_CMD_Set_ADCS_Parameters();
    IGroundStationSerialMessage *Create_CMD_Set_ADCS_Controller(char controllerId, float controllerMatrix[3][6]);
    IGroundStationSerialMessage* Create_CMD_Set_ADCS_Ephemerides(uint16_t chunkId, std::vector<ephemerides_t> ephemeridesDataQueue);
    IGroundStationSerialMessage* Create_CMD_Set_ADCS_Detumble();
    IGroundStationSerialMessage* Create_CMD_Erase_Flash();
signals:
    void ReceivedHandshake();
    void ReceivedSatelliteSystemInformation(IGroundStationSerialMessage* message);
private:
    Settings& m_settings;
    Ui::MainWindow* m_mainWindowUI = nullptr;
    Ui::systeminformationpane* m_systemInfoUI = nullptr;
    QString m_satVersion = "FOSSASAT-2";
};

#endif // Interpreter_H
