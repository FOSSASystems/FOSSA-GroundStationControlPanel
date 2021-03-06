#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtWidgets>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDir>
#include <QByteArray>
#include <QThread>
#include <QDebug>

#include <queue>
#include <iostream>
#include <string.h>
#include <cstring>
#include <stdexcept>

#include <FOSSA-Comms.h>

#include "systeminformationpane.h"
#include "messagelogframe.h"
#include "SerialPortThread.h"

#include "MessageHandlers/AllMessageHandlers.h"

#include "DopplerShiftCorrector.h"
#include "Settings.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SendDatagram(Datagram datagram);
    void SendHandshake();
    Ui::MainWindow* GetUI();
private:
    void ReceivedByte(uint8_t data);

    void LoadControlPanelSettingsUI();
    void LoadGroundStationSettingsUI();
    void LoadSatelliteConfigurationUI();
    void LoadSatelliteControlsUI();

public slots:
    void StartDopplerCorrector();
private slots:
    void on_actionView_Serial_Ports_triggered();

    // receive from serial port.
    void HandleRead(QByteArray data);
    void ErrorReceived(const QString& str);
    void TimeoutReceived(const QString& str);

    // control panel settings tab.
    void on_handshakeSendButton_clicked();
    void on_ControlPanelSettings_refreshSerialPortButton_clicked();
    void on_ControlPanelSettings_serialPort_SetButton_clicked();
    void on_ControlPanelSettings_securitySetButton_clicked();
    void on_ControlPanelSettings_Security_Reveal_Button_clicked();
    void on_ControlPanelSettings_Doppler_Update_Settings_Button_clicked();

    // Doppler shift correction method.
    void SendDopplerShiftedFrequency();

    // Receive message from message logger.
    void ReceivedMessagefromMessageLog(QString msg);
    void ReceivedDatagramfromSystemInformationPane(Datagram datagram);


    void on_CameraControl_Capture_Button_clicked();

    void on_CameraControl_GetPictureLength_GetPictureLength_Button_clicked();

    void on_CameraControl_PictureBurst_GetPictureBurst_Button_clicked();

    void on_Satelliteconfig_ADCs_Ephemerides_DataStack_Push_Button_clicked();

    void on_SatelliteConfig_ADCs_Controller_Set_Button_clicked();

    void on_Satelliteconfig_ADCs_Ephemerides_DataStack_Clear_Button_clicked();

    void on_aSatelliteconfig_ADCs_Ephemerides_DataStack_Send_Button_clicked();

    void on_SatelliteConfig_SatelliteVersion_SetVersion_PushButton_clicked();

    void on_EEPROM_Control_Wipe_Button_clicked();

    void on_SatelliteConfig_Transmission_Send_Button_clicked();

    void on_ControlPanelSettings_Doppler_Shift_Enable_RadioButton_clicked();

    void on_ControlPanelSettings_Doppler_Shift_Disable_RadioButton_clicked();

    void on_SatelliteControls_BaseOps_Ping_Button_clicked();

    void on_SatelliteControls_BaseOps_Restart_Button_clicked();

    void on_SatelliteControls_BaseOps_Deploy_Button_clicked();

    void on_SatelliteControls_BaseOps_Abort_Button_clicked();

    void on_GPSControl_GetGPSState_GetLogState_Button_clicked();

    void on_GPSControl_LogGPS_StartLogging_Button_clicked();

    void on_GPSControl_GetGPSLog_GetLog_Button_clicked();

    void on_GPSControl_RunGPSCmd_RunCommand_Button_clicked();
    void on_StoreAndForward_Message_AddMessage_Button_clicked();
    void on_storeAndForwardStoreAndForward_RequestMessage_RequestMessage_Button_clicked();
    void on_TransmissionRouter_Retransmission_Retransmit_Button_clicked();
    void on_TransmissionRouter_RetransmissionCustom_RetransmitCustom_Button_clicked();

    void on_RouteCommand_RouteCommand_Button_clicked();

    void on_RecordIMU_RecordIMU_Button_clicked();

    void on_FlashControl_SetFlashContent_Button_clicked();

    void on_FlashControl_GetFlashContent_Get_Button_clicked();

    void on_FlashControl_EraseFlash_Erase_Button_clicked();

    void on_ADCSManeuvering_RunADCManeuver_Button_clicked();

    void on_SatelliteControls_Maneuver_Run_Button_clicked();

    void on_Detumble_Execute_Button_2_clicked();

    void on_SatelliteConfig_callsignSetButton_clicked();

    void on_SatelliteConfig_TLESetButton_clicked();

    void on_SatelliteConfig_ReceiveWindow_SetButton_clicked();

    void on_SatelliteConfig_PowerLimits_Set_Button_clicked();

    void on_SatelliteConfig_SpreadingFactor_Set_Button_clicked();

    void on_SatelliteConfig_SleepInterval_PushToStack_Button_clicked();

    void on_SatelliteConfig_SleepInterval_ClearInterval_Button_clicked();

    void on_SatelliteConfig_SleepInterval_SendoStack_Button_clicked();

    void on_SatelliteConfig_MPPT_Set_Button_clicked();

    void on_SatelliteConfig_RTC_Set_Button_clicked();

    void on_SatelliteConfig_LowPowerMode_Set_Button_clicked();

    void on_SatelliteConfig_IMU_Set_Button_clicked();
    void on_SatelliteConfig_IMU_Callibration_Set_PushButton_clicked();

    void on_SatelliteConfig_ADCs_Parameters_Set_Button_clicked();

    void on_SatelliteConfig_ADCs_Controller_FromFiles_Button_clicked();

    void on_SatelliteConfig_ADCs_Ephemerides_SetFromFiles_Button_clicked();

    void on_GroundStationSettings_GFSKConfigSetButton_clicked();

private:
    Ui::MainWindow *ui;

    // GUI Frames
    systeminformationpane *m_sytemInfoPane;
    MessageLogFrame *m_messageLogFrame;

    // serial port thread.
    SerialPortThread m_serialPortThread;

    bool m_handshakeReceived = false;

    // Timer for dopplerShiftCorrector;
    QTimer* m_dopplerCorrectionTimer = nullptr;
    DopplerShiftCorrector m_dopplerShiftCorrector;

    bool m_frameStreaming = false;
    bool m_lengthByte = false;
    uint8_t m_messageLength = 0;
    uint8_t m_fcpFrameLength = 0;
    QByteArray m_datagramData;
public:


};
#endif // MAINWINDOW_H
