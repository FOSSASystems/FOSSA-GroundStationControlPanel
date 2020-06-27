#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // load the system information pane
    m_sytemInfoPane = new systeminformationpane();
    m_sytemInfoPane->setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);
    m_sytemInfoPane->show();

    // load the message log frame
    m_messageLogFrame = new MessageLogFrame();
    m_messageLogFrame->setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);
    m_messageLogFrame->show();

    // initialize the 4 main tabs.
    this->LoadControlPanelSettingsUI();
    this->LoadGroundStationSettingsUI();
    this->LoadSatelliteConfigurationUI();
    this->LoadSatelliteControlsUI();

    // startup the serial port thread.
    m_serialPortThread.start();

    // data piping from serial thread to gui thread
    connect(&(this->m_serialPortThread), &SerialPortThread::HandleRead, this, &MainWindow::HandleRead, Qt::QueuedConnection);
    connect(&(this->m_serialPortThread), &SerialPortThread::HandleError, this, &MainWindow::ErrorReceived, Qt::AutoConnection);
    connect(&(this->m_serialPortThread), &SerialPortThread::HandleTimeout, this, &MainWindow::TimeoutReceived, Qt::AutoConnection);

    // data piping from message log service to message log frame.
    connect(&m_messageLog, &MessageLog::MessageLogged, m_messageLogFrame, &MessageLogFrame::ReceivedMessageLogged, Qt::AutoConnection);
}

MainWindow::~MainWindow()
{
    m_serialPortThread.quit();

    delete m_sytemInfoPane;
    delete ui;
}

void MainWindow::SendHandshake()
{
    //
    // Handshake the ground station.
    //
    char handshakeMessage = 0b10000000;
    QByteArray msg;
    msg.append(handshakeMessage);
    this->m_serialPortThread.Write(msg);
}

void MainWindow::ReceivedHandshake()
{
    QMessageBox msgBox;
    msgBox.setText("Handshake received!");
    msgBox.exec();
}


// Response received is a slot, this slot is signalled from the serial port thread.
void MainWindow::HandleRead(QByteArray data)
{
    const char* respData = data.data();

    // interpret the serial data into the message.
    IGroundStationSerialMessage* msg = this->SerialData_To_GroundStationSerialMessage(respData, data.size());

    // push the message to the log panel.
    m_messageLog.PushMessage(msg);

    // interpret the received message into function calls.
    this->Interpret_Received_Message(msg);

    delete msg;
}

// The IGroundStationSerialMessage object is created in Interpreter.h  function name: Create_GroundStationSerialMessage()
void MainWindow::SendSerialData(IGroundStationSerialMessage* msg)
{
    // log the message.
    m_messageLog.PushMessage(msg);

    // get the string.
    std::string cmdDataStr = msg->GetRawData();

    // convert it to a byte array.
    QByteArray byteArray;
    for (char& v : cmdDataStr)
    {
        byteArray.push_back(v);
    }

    // send the message to the ground station via serial.
    this->m_serialPortThread.Write(byteArray);

    delete msg;
}



void MainWindow::ErrorReceived(const QString &str)
{
    throw std::runtime_error(str.toUtf8());
}

void MainWindow::TimeoutReceived(const QString &str)
{
    //throw std::runtime_error(str.toUtf8());
}





//
// Satellite controls tab.
//
void MainWindow::LoadSatelliteControlsUI()
{

}

//
// Satellite configuration tab.
//
void MainWindow::LoadSatelliteConfigurationUI()
{

}




///////////////////////////////////
/// CGround station settings tab //
///////////////////////////////////

#define GroundPanelSettingsTab_Start {

//
// Ground station config tab
//
void MainWindow::LoadGroundStationSettingsUI()
{

}


void MainWindow::on_handshakeSendButton_clicked()
{
    m_serialPortThread.Write(QByteArray("ABC"));
}

///////////////////////////////////////
/// CGround station settings tab END //
///////////////////////////////////////
#define GroundPanelSettingsTab_End }







/////////////////////////////////
/// Control panel settings tab //
/////////////////////////////////

#define ControlPanelSettingsTab_Start {

//
// Main initialize function.
//
void MainWindow::LoadControlPanelSettingsUI()
{
    // load the key from the settings
    bool keyLoaded = m_settings.LoadKeyFromSettings();
    if (keyLoaded)
    {
        m_settings.SetKeySet();
    }

    // Load password from the settings
    bool passwordLoaded = m_settings.LoadPasswordFromSettings();
    if (passwordLoaded)
    {
        m_settings.SetPasswordSet();
    }

    QMap<QString, QList<qint32>> baudRates;

    // get a list of available ports.
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    QStringList portNames;
    for (auto iter = availablePorts.begin(); iter != availablePorts.end(); iter++)
    {
        QSerialPortInfo& portInfo = *iter;
        QString portName = portInfo.portName();
        baudRates[portName] = portInfo.standardBaudRates();
        portNames.push_back(portName);
    }

    // set the combo box to the available ports.
    this->ui->ControlPanelSettings_serialPortNameComboBox->addItems(portNames);

    // get the first item in the combo box.
    if (portNames.length() <= 0)
    {
        QString portName = QString("TestPort1");

        m_serialPortThread.SetPortName(portName); // set the port name

        this->ui->statusbar->showMessage("[ERROR] No ports found!", 5000);
    }
    else
    {
        // get the baud rate.
        int baudRate = this->ui->ControlPanelSettings_serialPortBaudRate_ComboBox->currentText().toInt();

        // set the settings port name.
        QString firstPortName = portNames.first();

        QList<qint32> baudRatesForThisPortName = baudRates[firstPortName];
        QStringList baudRatesStringList;
        for (qint32& baudrate : baudRatesForThisPortName)
        {
            baudRatesStringList.push_back(QString::number(baudrate));
        }
        this->ui->ControlPanelSettings_serialPortBaudRate_ComboBox->addItems(baudRatesStringList);

    }
}



//
// UI Elements
//

void MainWindow::on_ControlPanelSettings_refreshSerialPortButton_clicked()
{
    // clear the ui elements.
    this->ui->ControlPanelSettings_serialPortBaudRate_ComboBox->clear();
    this->ui->ControlPanelSettings_serialPortNameComboBox->clear();

    QMap<QString, QList<qint32>> baudRates;

    // get a list of available ports.
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    QStringList portNames;
    for (auto iter = availablePorts.begin(); iter != availablePorts.end(); iter++)
    {
        QSerialPortInfo& portInfo = *iter;
        QString portName = portInfo.portName();
        baudRates[portName] = portInfo.standardBaudRates();
        portNames.push_back(portName);
    }

    // set the combo box to the available ports.
    this->ui->ControlPanelSettings_serialPortNameComboBox->addItems(portNames);

    // get the first item in the combo box.
    if (portNames.length() <= 0)
    {
        this->ui->statusbar->showMessage("[ERROR] No ports found!", 5000);
    }
    else
    {
        QString firstPortName = portNames.first();

        QList<qint32> baudRatesForThisPortName = baudRates[firstPortName];
        QStringList baudRatesStringList;
        for (qint32& baudrate : baudRatesForThisPortName)
        {
            baudRatesStringList.push_back(QString::number(baudrate));
        }

        this->ui->ControlPanelSettings_serialPortBaudRate_ComboBox->addItems(baudRatesStringList);
    }
}

void MainWindow::on_ControlPanelSettings_serialPort_SetButton_clicked()
{
    QString portName = this->ui->ControlPanelSettings_serialPortNameComboBox->currentText();
    int baudRate = this->ui->ControlPanelSettings_serialPortBaudRate_ComboBox->currentText().toInt();

    m_serialPortThread.Close();
    m_serialPortThread.SetPortName(portName);
    m_serialPortThread.SetBaudRate(baudRate);
    m_serialPortThread.Open();

    this->ui->statusbar->showMessage("[SUCCESS] Port set to: " + portName + " @ " + QString::number(baudRate), 5000);
}

/////////////////////////////////////
/// Control panel settings tab END //
/////////////////////////////////////
#define ControlPanelSettingsTab_End }


void MainWindow::on_actionView_Serial_Ports_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Click show details to see all available serial ports.");

    QString str;

    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for (auto iter = list.begin(); iter != list.end(); iter++)
    {
        QSerialPortInfo info = *iter;
        str.append(info.portName());

        str.append("\r\n\t");
        str.append(info.serialNumber());

        str.append("\r\n\t");
        str.append(info.description());

        str.append("\r\n\t");
        str.append(info.systemLocation());

        str.append("\r\n\tIs Busy = ");
        str.append(info.isBusy());
        str.append("\r\n");
        str.append("\r\n");
        str.append("\r\n");
    }

    msgBox.setDetailedText(str);

    msgBox.exec();
}

//
// Controls tab
//

// ping button
void MainWindow::on_baseOpsPingButton_clicked()
{
    IGroundStationSerialMessage* msg = this->Create_CMD_Ping();
    this->SendSerialData(msg);
}

void MainWindow::on_baseOpsDeploybutton_clicked()
{
    IGroundStationSerialMessage* msg = this->Create_CMD_Deploy();
    this->SendSerialData(msg);
}


///
/////////////////////////
/// FOSSA INTERPRETER ///
/////////////////////////
///
IGroundStationSerialMessage *MainWindow::SerialData_To_GroundStationSerialMessage(const char *serialData, char serialDataLength)
{
    // first byte is the control byte.
    char controlByte = serialData[0];

    // second byte is the payload length.
    char lengthByte = serialData[1];

    // rest of the bytes is the payload.
    char* payload = new char[lengthByte];
    memcpy(payload, &(serialData[2]), lengthByte);

    GroundStationSerialMessage* message = new GroundStationSerialMessage(controlByte, lengthByte, payload);

    // delete the payload, it is copied in the datagram ctor.
    delete[] payload;

    return dynamic_cast<IGroundStationSerialMessage*>(message);
}

// this is the base class for all the Create_CMD_XXX methods.
IGroundStationSerialMessage* MainWindow::Create_GroundStationSerialMessage(char operationId, uint8_t functionId, uint8_t optDataLength, char* optData)
{
    char directionBit = FCPI_DIR_TO_GROUND_STATION; // all outgoing messages are to the ground station.

    // check whether this function id is for encryption.
    bool encrypt = functionId >= PRIVATE_OFFSET;

    //
    // Get information
    //
    std::string callsignStr = ui->SatelliteConfig_callsignLineEdit->text().toStdString();
    std::string passwordStr = m_settings.GetPassword();

    const char* callsign = callsignStr.c_str();
    const char* password = passwordStr.c_str();
    const uint8_t* key = m_settings.GetKey();

    //
    // Check that the encryption is setup.
    //
    if (encrypt)
    {
        bool passwordSet = m_settings.IsPasswordSet();
        bool keySet = m_settings.IsKeySet();

        if ( !passwordSet || !keySet )
        {
            return nullptr;
        }
    }

    //
    // Get the entire frame's length.
    //
    uint8_t frameLength = 0;
    if (encrypt)
    {
        frameLength = FCP_Get_Frame_Length((char*)callsign, optDataLength, password);
    }
    else
    {
        frameLength = FCP_Get_Frame_Length((char*)callsign, optDataLength);
    }

    //
    // Create and encoded (at once) frame.
    //
    uint8_t* frame = new uint8_t[frameLength];
    if (encrypt)
    {
        FCP_Encode(frame, (char*)callsign, functionId, optDataLength, (uint8_t*)optData, key, password);
    }
    else
    {
        FCP_Encode(frame, (char*)callsign, functionId, optDataLength);
    }


    //
    // Send the frame wrapped in the gcs protocol.
    //
    char controlByte = directionBit | operationId;
    GroundStationSerialMessage* msg = new GroundStationSerialMessage(controlByte, (char)frameLength, (char*)frame); // the payload IS the FCP Frame above.

    delete[] frame;

    return dynamic_cast<IGroundStationSerialMessage*>(msg);
}

void MainWindow::Interpret_Received_Message(IGroundStationSerialMessage *inMsg)
{
    // route via the operation id
    int operationId = inMsg->GetOperationID();

    if (operationId == 0)
    {
        // handshake
        Interpret_Handshake(inMsg);
    }
    else if (operationId == 1)
    {
        // FCP frame.
        Interpret_FCP_Frame(inMsg);
    }
    else if (operationId == 2)
    {
        // configuration change.
        Interpret_Config_Change(inMsg);
    }
}

void MainWindow::Interpret_Handshake(IGroundStationSerialMessage *inMsg)
{
    GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);
}

void MainWindow::Interpret_Config_Change(IGroundStationSerialMessage *inMsg)
{
    GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);
}

void MainWindow::Interpret_FCP_Frame(IGroundStationSerialMessage *inMsg)
{
    GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);

    // raw characters
    char* payloadData = msg->GetPayload();
    char payloadLength = msg->GetPayloadLengthByte();

    // converted to uint8_t
    uint8_t frameLength = (uint8_t)payloadLength;
    uint8_t* frame = new uint8_t[frameLength];
    for (int i = 0; i < frameLength; i++)
    {
        frame[i] = (uint8_t)(payloadData[i]);
    }

    char * callsign = (char*)ui->SatelliteConfig_callsignLineEdit->text().toStdString().c_str(); /// @todo fix stripping of const.

    // get the function ID.
    int16_t functionId = FCP_Get_FunctionID(callsign, frame, frameLength);

    // get the optional data length.
    int16_t optionalDataLength = FCP_Get_OptData_Length(callsign, frame, frameLength, NULL, NULL);

    if (optionalDataLength >= 0)
    {
        // frame contains metadata
        uint8_t* optionalData = new uint8_t[optionalDataLength];
        FCP_Get_OptData(callsign, frame, frameLength, optionalData, NULL, NULL);

        ///////////////////////////////
        /// Interpret the commands. ///
        ///////////////////////////////

        // Public responses
        if (functionId == RESP_PONG)
        {
            // do nothing, already logged.
        }
        else if (functionId == RESP_REPEATED_MESSAGE)
        {

        }
        else if (functionId == RESP_REPEATED_MESSAGE_CUSTOM)
        {

        }
        else if (functionId == RESP_SYSTEM_INFO)
        {

        }
        else if (functionId == RESP_PACKET_INFO)
        {

        }
        else if (functionId == RESP_STATISTICS)
        {

        }
        else if (functionId == RESP_FULL_SYSTEM_INFO)
        {

        }
        else if (functionId == RESP_STORE_AND_FORWARD_ASSIGNED_SLOT)
        {

        }
        else if (functionId == RESP_FORWARDED_MESSAGE)
        {

        }
        else if (functionId == RESP_PUBLIC_PICTURE)
        {

        }
        else if (functionId == RESP_DEPLOYMENT_STATE)
        {

        }
        else if (functionId == RESP_RECORDED_SOLAR_CELLS)
        {

        }
        else if (functionId == RESP_CAMERA_STATE)
        {

        }
        else if (functionId == RESP_RECORDED_IMU)
        {

        }
        else if (functionId == RESP_MANUAL_ACS_RESULT)
        {

        }
        else if (functionId == RESP_GPS_LOG)
        {

        }
        else if (functionId == RESP_GPS_LOG_STATE)
        {

        }
        else if (functionId == RESP_FLASH_CONTENTS)
        {

        }
        else if (functionId == RESP_CAMERA_PICTURE)
        {

        }
        else if (functionId == RESP_CAMERA_PICTURE_LENGTH)
        {

        }
        else if (functionId == RESP_GPS_COMMAND_RESPONSE)
        {

        }
        else if (functionId == RESP_ACKNOWLEDGE)
        {

        }

        // Private
        // there are no privately downlinked messages.


        delete[] optionalData;
    }

    delete[] frame;
}

IGroundStationSerialMessage *MainWindow::Create_CMD_Ping()
{
    return this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_PING, 0, nullptr);
}

IGroundStationSerialMessage *MainWindow::Create_CMD_Deploy()
{
    IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_DEPLOY, 0, nullptr);

    return msg;
}


