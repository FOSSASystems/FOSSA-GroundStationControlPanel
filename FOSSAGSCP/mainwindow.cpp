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

    // initialize the interpereter.
    m_interpreter = new Interpreter(m_settings, ui);

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

    m_dopplerCorrectionTimer->stop();
    delete m_dopplerCorrectionTimer;

    delete m_interpreter;
    delete m_sytemInfoPane;
    delete ui;
}


// Response received is a slot, this slot is signalled from the serial port thread.
void MainWindow::HandleRead(QByteArray data)
{
    // append the data to the buffer.
    m_commandBuffer.push_back(data);


    // search for the start of the frame.
    for (int i = 0; i < m_commandBuffer.length(); i++)
    {
        uint8_t potentialControlByte = (uint8_t)m_commandBuffer[i];

        // if this value is a control byte of dir 1 and op id 0
        // then this value is a ctonrol byte data structure.
        uint8_t dirCheckMask = 0b10000000;
        uint8_t isCorrectDirection = potentialControlByte & dirCheckMask;
        isCorrectDirection = isCorrectDirection >> 7;
        if (isCorrectDirection)
        {
            uint8_t opIdCheckMask = 0b01111111;
            uint8_t potentialOpId = potentialControlByte & opIdCheckMask;
            if (potentialOpId == 0x0 ||
                    potentialOpId == 0x1 ||
                    potentialOpId == 0x2 ||
                    potentialOpId == 0x3)
            {
                m_commandStartIndex = i;
                m_commandStartFound = true;

                break;
            }
        }
    }


    // if there is a start of a frame in the buffer.
    if (m_commandStartFound)
    {
        // if there are bytes after the start index.

        // check for the length byte.
        if ( (m_commandStartIndex + 1) < m_commandBuffer.length() )
        {
            m_commandLength = m_commandBuffer[m_commandStartIndex + 1];
            m_commandLengthFound = true;
        }

        if (m_commandLengthFound)
        {
            int endOfCommandIndex = m_commandStartIndex + 1 + 1 + m_commandLength;
            // if we have enough data in the command buffer that the command specifies.
            if (endOfCommandIndex <= m_commandBuffer.length())
            {
                //
                // here we have all the data required.
                //

                // remove the bytes before the command frame in the buffer.
                m_commandBuffer.remove(0, m_commandStartIndex);

                // copy the command buffer to a new array.
                QByteArray commandData = m_commandBuffer;

                commandData.remove(endOfCommandIndex,
                                   (m_commandBuffer.length() - endOfCommandIndex));

                // remove the frame from the command buffer.
                m_commandBuffer.remove(0, endOfCommandIndex);


                // interpret the serial data into the message.
                char *cmdData = commandData.data();

                IGroundStationSerialMessage* msg = m_interpreter->SerialData_To_GroundStationSerialMessage(cmdData, commandData.length());

                // push the message to the log panel.
                m_messageLog.PushMessage(msg);

                // interpret the received message into function calls.
                m_interpreter->Interpret_Received_Message(msg);

                delete msg;

                // reset command buffer found flags.
                m_commandStartFound = false;
                m_commandLengthFound = false;

                m_commandBuffer.clear();
            }
        }
    }
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

    // send the message to the ground station via serial and block until successful.
    while ( (this->m_serialPortThread.Write(byteArray)) == false )
    {
        // the serial thread is currently locked, wait for 10ms and try again.
        QThread::msleep(20);
    }

    // delete the message.
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


void MainWindow::SendHandshake()
{

}

void MainWindow::ReceivedHandshake()
{
    QMessageBox msgBox;
    msgBox.setText("Handshake received!");
    msgBox.exec();
}

void MainWindow::SendDopplerShiftedFrequency()
{
    //return;

    QString modemType = ui->GroundStationSettings_ModemTypeComboBox->currentText();

    QString currentCarrierFreqStr;
    if (modemType == "LoRa")
    {
        currentCarrierFreqStr = ui->GroundStationSettings_loraConfigurationCarrierFrequencyLineEdit->text();
    }
    else if (modemType == "GFSK")
    {
        currentCarrierFreqStr = ui->GroundStationSettings_GFSKConfigCarrierFrequencyLineEdit->text();
    }

    QString currentSatelliteNameStr = ui->SatelliteConfig_callsignLineEdit->text();
    std::string currentSatelliteNameStdStr = currentSatelliteNameStr.toStdString();

    // get the doppler amount.
    double dopplerShiftedFreq;
    m_dopplerShiftCorrector.GetDopplerShiftNow(currentSatelliteNameStdStr, &dopplerShiftedFreq);

    // current configured frequency.
    double currentCarrierFrequency = currentCarrierFreqStr.toDouble();

    // new frequency = current + (current*dopplerShift)
    double newCarrierFrequency = currentCarrierFrequency + (currentCarrierFrequency * (1.0 - dopplerShiftedFreq));

    ui->statusbar->showMessage(QString::number(newCarrierFrequency), 10000);

    // Send to the ground station.
    QByteArray msg;

    uint8_t directionBit = 0x00;
    uint8_t operationId = 0x03;
    uint8_t controlByte = directionBit & operationId;
    msg.append(controlByte);

    uint8_t length = 4;
    msg.append(length);


    float freqAsFloat = (float)newCarrierFrequency;
    QByteArray freqFloatByteArray;
    QDataStream out(freqFloatByteArray);
    out << freqAsFloat;
    msg.append(freqFloatByteArray);

    this->m_serialPortThread.Write(msg);
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
/// Ground station settings tab //
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
    //
    // Handshake the ground station.
    //
    char handshakeMessage = 0b10000000;
    QByteArray msg;
    msg.append(handshakeMessage);
    this->m_serialPortThread.Write(msg);
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
    //
    // Load the serial port interface
    //
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


    //
    // Load the key and password into the UI.
    //

    // load the key from the settings
    bool keyLoaded = m_settings.LoadKeyFromSettings();
    if (keyLoaded)
    {
        m_settings.SetKeySet();

        uint8_t* key = m_settings.GetKey();
        QString keyAsStr;
        for (int i = 0; i < 16; i++)
        {
            uint8_t v = key[i];

            char hexStr[2];
            sprintf(&(hexStr[0]), "%02x", (uint8_t)v);
            keyAsStr.append(hexStr);
        }

        this->ui->ControlPanelSettings_securityKeyLineEdit->setText(keyAsStr);
    }

    // Load password from the settings
    bool passwordLoaded = m_settings.LoadPasswordFromSettings();
    if (passwordLoaded)
    {
        m_settings.SetPasswordSet();

        std::string password = m_settings.GetPassword();
        QString passwordStr = QString::fromStdString(password);
        this->ui->ControlPanelSettings_securityPasswordLineEdit->setText(passwordStr);
    }


    //
    // Load the latitude, longitude and altitude settings into the UI
    //
    bool latLongAltLoaded = m_settings.LoadLatLongAlt();

    if (latLongAltLoaded)
    {
        QString latStr = QString::number(m_settings.GetLatitude());
        QString lonStr = QString::number(m_settings.GetLongitude());
        QString altStr = QString::number(m_settings.GetAltitude());

        this->ui->ControlPanelSettings_Doppler_Shift_Latitude_LineEdit->setText(latStr);
        this->ui->ControlPanelSettings_Doppler_Shift_Longitude_LineEdit->setText(lonStr);
        this->ui->ControlPanelSettings_Doppler_Shift_Altitude_LineEdit->setText(altStr);
        //! @todo make the map pane move to this coordinate.
    }
    else
    {
        this->ui->ControlPanelSettings_Doppler_Shift_Latitude_LineEdit->setText("0.0");
        this->ui->ControlPanelSettings_Doppler_Shift_Longitude_LineEdit->setText("0.0");
        this->ui->ControlPanelSettings_Doppler_Shift_Altitude_LineEdit->setText("0.0");
    }

    //
    // Load the doppler shift correction into the UI
    //
    bool dopplerShiftCorrectionEnabled = m_settings.LoadDopplerShiftCorrectionEnabled();

    if (dopplerShiftCorrectionEnabled)
    {
        this->ui->ControlPanelSettings_Doppler_Shift_Enable_RadioButton->setChecked(true);
        this->ui->ControlPanelSettings_Doppler_Shift_Disable_RadioButton->setChecked(false);
    }
    else
    {
        this->ui->ControlPanelSettings_Doppler_Shift_Enable_RadioButton->setChecked(false);
        this->ui->ControlPanelSettings_Doppler_Shift_Disable_RadioButton->setChecked(true);
    }

    //
    // Load the TLE into the UI.
    //
    m_settings.LoadTLElines();

    std::string tleLine1 = m_settings.GetTLELine1();
    std::string tleLine2 = m_settings.GetTLELine2();

    QString tleLine1Str = QString::fromStdString(tleLine1);
    QString tleLine2Str = QString::fromStdString(tleLine2);

    this->ui->ControlPanelSettings_Doppler_Shift_TLE_1_LineEdit->setText(tleLine1Str);
    this->ui->ControlPanelSettings_Doppler_Shift_TLE_2_LineEdit->setText(tleLine2Str);

    //
    // Configure the simulation if enabled.
    //
    if (dopplerShiftCorrectionEnabled)
    {
        double latitude = m_settings.GetLatitude();
        double longitude = m_settings.GetLongitude();
        double attitude = m_settings.GetAltitude();

        m_dopplerShiftCorrector.SetObserverParameters(latitude, longitude, attitude);
        m_dopplerShiftCorrector.SetTLELines(tleLine1, tleLine2);
        this->ui->statusbar->showMessage("Doppler shift correction configured successfully.", 10);

        if (m_dopplerCorrectionTimer == nullptr)
        {
            // Setup the doppler shift timer.
            m_dopplerCorrectionTimer = new QTimer(this);
            connect(m_dopplerCorrectionTimer, &QTimer::timeout, this, &MainWindow::SendDopplerShiftedFrequency);
            if (m_settings.GetDopplerShiftCorrectionEnabled())
            {
                m_dopplerCorrectionTimer->start(1000 * 10); // 10s between doppler shift corrections.
            }
        }
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


void MainWindow::on_ControlPanelSettings_securitySetButton_clicked()
{
    QString passwordStr = this->ui->ControlPanelSettings_securityPasswordLineEdit->text();
    std::string passwordStdStr = passwordStr.toStdString();

    QString key = this->ui->ControlPanelSettings_securityKeyLineEdit->text();
    QByteArray keyAsBytes = key.toLocal8Bit();

    uint8_t keyBytes[16];
    for (int i = 0; i < 16; i++)
    {
        uint8_t v = keyAsBytes[i];
        keyBytes[i] = v;
    }

    m_settings.SetPassword(passwordStdStr);
    m_settings.SavePasswordToSettings();

    m_settings.SetKey(keyBytes);
    m_settings.SaveKeyToSettings();
}


void MainWindow::on_ControlPanelSettings_Security_Reveal_Button_clicked()
{
    const QLineEdit::EchoMode currentEchoMode = this->ui->ControlPanelSettings_securityPasswordLineEdit->echoMode();

    QLineEdit::EchoMode newEchoMode;
    if (currentEchoMode == QLineEdit::EchoMode::Password)
    {
        newEchoMode = QLineEdit::EchoMode::Normal;
    }
    else
    {
        newEchoMode = QLineEdit::EchoMode::Password;
    }

    this->ui->ControlPanelSettings_securityPasswordLineEdit->setEchoMode(newEchoMode);
    this->ui->ControlPanelSettings_securityKeyLineEdit->setEchoMode(newEchoMode);
}


void MainWindow::on_ControlPanelSettings_Doppler_Update_Settings_Button_clicked()
{

    double latitude = this->ui->ControlPanelSettings_Doppler_Shift_Latitude_LineEdit->text().toDouble();
    double longitude = this->ui->ControlPanelSettings_Doppler_Shift_Longitude_LineEdit->text().toDouble();
    double attitude = this->ui->ControlPanelSettings_Doppler_Shift_Altitude_LineEdit->text().toDouble();

    m_settings.SetLatitude(latitude);
    m_settings.SetLongitude(longitude);
    m_settings.SetAltitude(attitude);

    m_settings.SaveLatLongAlt();

    bool dopplerShiftCorrectionEnabled = this->ui->ControlPanelSettings_Doppler_Shift_Enable_RadioButton->isChecked();

    m_settings.SetDopplerShiftCorrectionEnabled(dopplerShiftCorrectionEnabled);
    m_settings.SaveDopplerShiftCorrectionEnabled();

    std::string tleLine1 = this->ui->ControlPanelSettings_Doppler_Shift_TLE_1_LineEdit->text().toStdString();
    std::string tleLine2 = this->ui->ControlPanelSettings_Doppler_Shift_TLE_2_LineEdit->text().toStdString();

    m_settings.SetTLELine1(tleLine1);
    m_settings.SetTLELine2(tleLine2);

    m_settings.SaveTLELines();


    //
    // Configure the simulation if enabled.
    //
    if (dopplerShiftCorrectionEnabled)
    {
        m_dopplerShiftCorrector.SetObserverParameters(latitude, longitude, attitude);
        m_dopplerShiftCorrector.SetTLELines(tleLine1, tleLine2);

        this->ui->statusbar->showMessage("Doppler shift correction configured successfully.", 10);

        m_dopplerCorrectionTimer->start(1000 * 10); // 10s between doppler shift corrections.
    }
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
    IGroundStationSerialMessage* msg = m_interpreter->Create_CMD_Ping();
    this->SendSerialData(msg);
}

void MainWindow::on_baseOpsDeploybutton_clicked()
{
    IGroundStationSerialMessage* msg = m_interpreter->Create_CMD_Deploy();
    this->SendSerialData(msg);
}




















































