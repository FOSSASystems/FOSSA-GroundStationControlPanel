#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <iostream>
#include <string.h>
#include <cstring>
#include <stdexcept>

#include <QtCore>
#include <QtWidgets>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDir>
#include <QByteArray>

#include <FOSSA-Comms.h>

#include "SerialPortThread.h"

#include "systeminformationpane.h"
#include "messagelogframe.h"
#include "mappane.h"

#include "Components/GroundStationSerialMessage.h"
#include "Components/MessageLog.h"
#include "Components/Settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SendSerialData(IGroundStationSerialMessage* datagram);
    void SendHandshake();
    void ReceivedHandshake();
private slots:
    void on_actionView_Serial_Ports_triggered();

    // receive from serial port.
    void HandleRead(QByteArray data);
    void ErrorReceived(const QString& str);
    void TimeoutReceived(const QString& str);

    // controls tab
    void on_baseOpsPingButton_clicked();
    void on_baseOpsDeploybutton_clicked();

    // control panel settings tab.
    void on_ControlPanelSettings_refreshSerialPortButton_clicked();
    void on_ControlPanelSettings_serialPort_SetButton_clicked();
    void on_handshakeSendButton_clicked();
    void on_ControlPanelSettings_securitySetButton_clicked();

    void on_ControlPanelSettings_Security_Reveal_Button_clicked();

private:

    Ui::MainWindow *ui; // this pointer is private and only available in mainwindow.h
                        // this means that we can't pass it to other systems, therefore
                        // we must do all UI interactivity in this class or create functions for
                        // all elements? 21:52 25/06/2020 R.G.Bamford

    // GUI Frames
    systeminformationpane *m_sytemInfoPane;
    MessageLogFrame *m_messageLogFrame;
    MapPane* m_mapFrame;

    // Logs and settings.
    Settings m_settings;
    MessageLog m_messageLog;

    // serial port thread.
    SerialPortThread m_serialPortThread;

    // Since the serial port will not frame the commands correctly,
    // we must manually interpret the received data into command frames.
    bool m_commandBeingRead = false;
    QByteArray m_commandBuffer;
    bool m_commandStartFound = false;
    int m_commandStartIndex;
    uint8_t m_commandLength;
    bool m_commandLengthFound = false;

    bool m_handshakeReceived = false;

    void LoadControlPanelSettingsUI();
    void LoadGroundStationSettingsUI();
    void LoadSatelliteConfigurationUI();
    void LoadSatelliteControlsUI();

public:
    ////////////////////////////////////////
    /// INTERPRETER                      ///
    /// 21:30 - 25/06/2020 - R.G.Bamford ///
    ////////////////////////////////////////

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


    /// The ground station serial message is an internal command structure.
    /// This creates it from raw serial datagrams.
    /// @todo checks on the serial data length.
    IGroundStationSerialMessage* SerialData_To_GroundStationSerialMessage(char* serialData, char serialDataLength);

    // this is the base class for all the Create_CMD_XXX methods.
    IGroundStationSerialMessage* Create_GroundStationSerialMessage(char operationId, uint8_t functionId, uint8_t optDataLength, char* optData);


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
    IGroundStationSerialMessage* Create_CMD_Get_Statistics();
    IGroundStationSerialMessage* Create_CMD_Get_Full_System_Info();
    IGroundStationSerialMessage* Create_CMD_Store_And_Forward_Add();
    IGroundStationSerialMessage* Create_CMD_Store_And_Forward_Request();
    IGroundStationSerialMessage* Create_CMD_Request_Public_Picture();

    /////
    /// Private commands (encrypted uplink messages)
    /////
    IGroundStationSerialMessage* Create_CMD_Deploy();
    IGroundStationSerialMessage* Create_CMD_Restart();
    IGroundStationSerialMessage* Create_CMD_Wipe_EEPROM();
    IGroundStationSerialMessage* Create_CMD_Set_Transmit_Enable();
    IGroundStationSerialMessage* Create_CMD_Set_Callsign();
    IGroundStationSerialMessage* Create_CMD_Set_SF_Mode();
    IGroundStationSerialMessage* Create_CMD_Set_MPPT_Mode();
    IGroundStationSerialMessage* Create_CMD_Set_Low_Power_Mode_Enable();
    IGroundStationSerialMessage* Create_CMD_Set_Receive_Windows();
    IGroundStationSerialMessage* Create_CMD_Record_Solar_Cells();
    IGroundStationSerialMessage* Create_CMD_Camera_Capture();
    IGroundStationSerialMessage* Create_CMD_Set_Power_Limits();
    IGroundStationSerialMessage* Create_CMD_Set_RTC();
    IGroundStationSerialMessage* Create_CMD_Record_IMU();
    IGroundStationSerialMessage* Create_CMD_Run_Manual_ACS();
    IGroundStationSerialMessage* Create_CMD_Log_GPS();
    IGroundStationSerialMessage* Create_CMD_Get_GPS_Log();
    IGroundStationSerialMessage* Create_CMD_Get_Flash_Contents();
    IGroundStationSerialMessage* Create_CMD_Get_Picture_Length();
    IGroundStationSerialMessage* Create_CMD_Get_Picture_Burst();
    IGroundStationSerialMessage* Create_CMD_Route();
    IGroundStationSerialMessage* Create_CMD_Set_Flash_Contents();
    IGroundStationSerialMessage* Create_CMD_Set_TLE();
    IGroundStationSerialMessage* Create_CMD_Get_GPS_Log_State();
    IGroundStationSerialMessage* Create_CMD_Run_GPS_Command();
    IGroundStationSerialMessage* Create_CMD_Set_Sleep_Intervals();
    IGroundStationSerialMessage* Create_CMD_Abort();
    IGroundStationSerialMessage* Create_CMD_Maneuver();
    IGroundStationSerialMessage* Create_CMD_Set_ADCS_Parameters();
    IGroundStationSerialMessage* Create_CMD_Erase_Flash();

};
#endif // MAINWINDOW_H
