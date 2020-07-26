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
#include <QThread>
#include <queue>

#include "CStructs.h"

#include <FOSSA-Comms.h>

#include "SerialPortThread.h"

#include "systeminformationpane.h"
#include "messagelogframe.h"

#include "GroundStationSerialMessage.h"
#include "MessageLog.h"
#include "Settings.h"
#include "Interpreter.h"
#include "DopplerShiftCorrector.h"

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

public slots:
    void ReceivedHandshake();

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
    void ReceivedMessagefromSystemInformationPane(IGroundStationSerialMessage* msg);

    void on_CameraControl_Capture_Button_clicked();

    void on_CameraControl_GetPictureLength_GetPictureLength_Button_clicked();

    void on_CameraControl_PictureBurst_GetPictureBurst_Button_clicked();

    void on_Satelliteconfig_ADCs_Ephemerides_DataStack_Push_Button_clicked();

    void on_SatelliteConfig_ADCs_Controller_Set_Button_clicked();

    void on_Satelliteconfig_ADCs_Ephemerides_DataStack_Clear_Button_clicked();

    void on_aSatelliteconfig_ADCs_Ephemerides_DataStack_Send_Button_clicked();

private:

    Ui::MainWindow *ui; // this pointer is private and only available in mainwindow.h
                        // this means that we can't pass it to other systems, therefore
                        // we must do all UI interactivity in this class or create functions for
                        // all elements? 21:52 25/06/2020 R.G.Bamford

    // GUI Frames
    systeminformationpane *m_sytemInfoPane;
    MessageLogFrame *m_messageLogFrame;


    Settings m_settings;
    MessageLog m_messageLog;
    Interpreter* m_interpreter;
    DopplerShiftCorrector m_dopplerShiftCorrector;

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

    // Timer for dopplerShiftCorrector;
    QTimer* m_dopplerCorrectionTimer = nullptr;

    // ****
    // These are the main entry points for the program, each tab loads itself here.
    // ****
    void LoadControlPanelSettingsUI();
    void LoadGroundStationSettingsUI();
    void LoadSatelliteConfigurationUI();
    void LoadSatelliteControlsUI();

public:


};
#endif // MAINWINDOW_H
