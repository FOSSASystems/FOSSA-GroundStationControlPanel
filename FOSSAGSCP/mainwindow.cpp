#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // load the message log frame
    m_messageLogFrame = new MessageLogFrame();
    m_messageLogFrame->show();

    // initialize the interpereter.
    assert(ui != nullptr);

    // create the interpreter
    m_interpreter = new DatagramInterpreter(ui, m_messageLogFrame);

    // load the system information pane
    m_sytemInfoPane = new systeminformationpane(m_interpreter);
    m_sytemInfoPane->show();

    m_interpreter->SetSystemInformationPane(m_sytemInfoPane->ui);

    // create the processor
    m_processor = new DatagramProcessor(ui, m_sytemInfoPane->ui, m_messageLogFrame);

    // create the doppler correction timer.
    // must be initialized before the UI
    m_dopplerCorrectionTimer = new QTimer(this);
    connect(m_dopplerCorrectionTimer, &QTimer::timeout, this, &MainWindow::SendDopplerShiftedFrequency);


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

    // signal piping from the interpeter to this.
    connect(m_processor, &DatagramProcessor::StartDopplerCorrector, this, &MainWindow::StartDopplerCorrector);

    // data piping from message log frame to serial port thread.
    connect(m_messageLogFrame, &MessageLogFrame::SendDataFromMessageLogFrame, this, &MainWindow::ReceivedMessagefromMessageLog);

    // data piping from the system information frame to the serial port thread.
    connect(m_sytemInfoPane, &systeminformationpane::SendDataFromSystemInformationPane, this, &MainWindow::ReceivedDatagramfromSystemInformationPane);
}

MainWindow::~MainWindow()
{
    m_serialPortThread.quit();

    if (m_dopplerCorrectionTimer != nullptr)
    {
        m_dopplerCorrectionTimer->stop();
        delete m_dopplerCorrectionTimer;
    }

    delete m_interpreter;
    delete m_sytemInfoPane;
    delete ui;
}



bool IsControlByte(uint8_t value)
{
    uint8_t dir = value >> 7;
    uint8_t op = value & 0b01111111;

    if (dir == 1)
    {
        if (op == 0 || op == 1 || op == 2 || op == 3)
        {
            return true;
        }
    }

    return false;
}

void MainWindow::ReceivedByte(uint8_t data)
{
    if (m_frameStreaming)
    {
        m_frameData.push_back(data);

        if (m_lengthByte)
        {
            m_messageLength = data + 2;
            m_fcpFrameLength = data - 2;

            m_lengthByte = false;
        }

        if (m_frameData.size() >= m_messageLength)
        {
            IDatagram* datagram = m_interpreter->SerialData_To_Datagram(m_frameData);

            // push the datagram to the log panel.
            m_messageLogFrame->WriteDatagram(datagram);

            // process the received datagram into function calls.
            m_processor->ProcessDatagram(datagram);

            delete datagram;

            // reset data and flags
            m_frameData.clear();

            m_frameStreaming = false;
        }

        return;
    }
    else
    {
        if (IsControlByte(data))
        {
            m_frameData.push_back(data);

            m_frameStreaming = true;

            m_lengthByte = true;

            return;
        }
    }
}

// Response received is a slot, this slot is signalled from the serial port thread.
void MainWindow::HandleRead(QByteArray data)
{
    for (int i = 0; i < data.length(); i++)
    {
        uint8_t v = data[i];
        ReceivedByte(v);
    }

    // this is a debug mode switch, enable this to see all incoming data.
    bool enableSerialSniffing = m_messageLogFrame->GetEnableSerialSniffingState();
    if (enableSerialSniffing)
    {
        QByteArray dataHexValues = data.toHex();
        QString rawMessage = QString(dataHexValues);
        m_messageLogFrame->RawWriteToLog(rawMessage);
    }
}

void MainWindow::SendDatagram(const IDatagram* datagram)
{
    // log the datagram.
    m_messageLogFrame->WriteDatagram(datagram);

    // convert the datagram to a byte array.
    uint32_t datagramDataLength = datagram->GetLength();
    const uint8_t* datagramData = datagram->GetData();

    QByteArray datagramDataArr;
    for (int i = 0; i < datagramDataLength; i++)
    {
        uint8_t v = datagramData[i];
        datagramDataArr.push_back(v);
    }

    delete[] datagramData;

    // send the message to the ground station via serial and block until successful.
    while ( (this->m_serialPortThread.Write(datagramDataArr)) == false )
    {
        // the serial thread is currently locked, wait for 10ms and try again.
        QThread::msleep(20);
    }
}



void MainWindow::ErrorReceived(const QString &str)
{
    throw std::runtime_error(str.toUtf8());
}

void MainWindow::TimeoutReceived(const QString &str)
{
    throw std::runtime_error(str.toUtf8());
}

void MainWindow::StartDopplerCorrector()
{
    QMessageBox msgBox;
    QString message = QString("Handshook the ground station!");
    msgBox.setText(message);
    msgBox.exec();

    //
    // Configure the simulation if enabled.
    //
    if (Settings::GetDopplerShiftCorrectionEnabled())
    {
        if (Settings::GetHandshookValue())
        {
            this->ui->statusbar->showMessage("Doppler shift correction started!", 10);

            m_dopplerCorrectionTimer->start(1000 * 10); // 10s between doppler shift corrections.
        }
    }
}


void MainWindow::ReceivedMessagefromMessageLog(QString msg)
{
    QByteArray bytes = msg.toLocal8Bit();
    m_serialPortThread.Write(bytes);
}

void MainWindow::ReceivedDatagramfromSystemInformationPane(IDatagram* datagram)
{
    this->SendDatagram(datagram);
}

void MainWindow::SendDopplerShiftedFrequency()
{
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
    bool dopplerShiftOk = m_dopplerShiftCorrector.GetDopplerShiftNow(currentSatelliteNameStdStr, &dopplerShiftedFreq);

    if (!dopplerShiftOk)
    {
        QMessageBox msgBox;
        msgBox.setText("TLE, LLA or Observer Position are invalid.");
        msgBox.exec();
        return;
    }


    // current configured frequency.
    double currentCarrierFrequency = currentCarrierFreqStr.toDouble();

    // new frequency = current + (current*dopplerShift)
    double newCarrierFrequency = currentCarrierFrequency + (currentCarrierFrequency * (1.0 - dopplerShiftedFreq));

    ui->statusbar->showMessage(QString::number(newCarrierFrequency), 10000);

    // Send to the ground station.
    QByteArray msg;

    uint8_t directionBit = 0x00;
    uint8_t operationId = 0x03;
    uint8_t controlByte = directionBit | operationId;
    msg.append(controlByte);

    uint8_t length = 4;
    msg.append(length);


    float freqAsFloat = (float)newCarrierFrequency;
    QByteArray freqFloatByteArray;
    QDataStream out(&freqFloatByteArray,QIODevice::OpenModeFlag::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::FloatingPointPrecision::SinglePrecision);
    out << freqAsFloat;

    // reverse to lsb first.
    std::reverse(freqFloatByteArray.begin(), freqFloatByteArray.end());

    msg.append(freqFloatByteArray);

    this->m_serialPortThread.Write(msg);
}

















/////////////////////////////
// Satellite controls tab. //
/////////////////////////////
#define SatelliteControlsTab_Start {
void MainWindow::LoadSatelliteControlsUI()
{

}


void MainWindow::on_CameraControl_Capture_Button_clicked()
{
    char pictureSlot = (char)this->ui->CameraControl_PictureSlot_SpinBox->value();
    char lightMode = (char)this->ui->CameraControl_LightMode_SpinBox->value();
    char pictureSize = (char)this->ui->CameraControl_PictureSisze_SpinBox->value();
    char brightness = (char)this->ui->CameraControl_Brightness_SpinBox->value();
    char saturation = (char)this->ui->CameraControl_Saturation_SpinBox->value();
    char specialFilter = (char)this->ui->CameraControl_SpecialFilter_SpinBox->value();
    char contrast = (char)this->ui->CameraControl_Contrast_SpinBox->value();

    IDatagram* datagram = m_interpreter->Create_CMD_Camera_Capture(pictureSlot,
                                                                            lightMode,
                                                                            pictureSize,
                                                                            brightness,
                                                                            saturation,
                                                                            specialFilter,
                                                                            contrast);
    this->SendDatagram(datagram);
}



void MainWindow::on_CameraControl_GetPictureLength_GetPictureLength_Button_clicked()
{
}

void MainWindow::on_CameraControl_PictureBurst_GetPictureBurst_Button_clicked()
{
    char pictureSlot = (char)this->ui->CameraControl_PictureBurst_PictureSlot_SpinBox->value();

    QString id = this->ui->CameraControl_PictureBurst_PicturePacketID_LineEdit->text();
    uint16_t packetId = id.toUInt();

    // 1 is full picture, 0 is scan data.
    char fullPictureOrScandata = (char)this->ui->CameraControl_PictureBurst_FullPictureModeFullPicture_RadioButton->isChecked();

    IDatagram* datagram = m_interpreter->Create_CMD_Get_Picture_Burst(pictureSlot, packetId, fullPictureOrScandata);

    this->SendDatagram(datagram);
}


void MainWindow::on_SatelliteConfig_ADCs_Controller_Set_Button_clicked()
{
    char controllerId =(char)this->ui->SatelliteConfig_ADCs_Controller_ControllerID_LineEdit->text().toStdString()[0];

    QString firstLine = this->ui->SatelliteConfig_ADCs_Controller_M50_LineEdit->text();
    QString secondLine = this->ui->SatelliteConfig_ADCs_Controller_M51_LineEdit->text();
    QString thirdLine = this->ui->SatelliteConfig_ADCs_Controller_M52_LineEdit->text();

    QStringList firstLineElements;
    firstLineElements = firstLine.split(',');

    if (firstLineElements.length() != 6)
    {
        throw "Number of elements != 3";
    }

    QStringList secondLineElements;
    secondLineElements = secondLine.split(',');

    if (secondLineElements.length() != 6)
    {
        throw "Number of elements != 6";
    }

    QStringList thirdLineElements;
    thirdLineElements = thirdLine.split(',');

    if (thirdLineElements.length() != 6)
    {
        throw "Number of elements != 6";
    }


    float controllerMatrix[3][6];
    for (int i = 0; i < 6; i++)
    {
        QString ele = firstLineElements[i];
        float val = ele.toDouble();
        controllerMatrix[0][i] = val;
    }
    for (int i = 0; i < 6; i++)
    {
        QString ele = secondLineElements[i];
        float val = ele.toDouble();
        controllerMatrix[1][i] = val;
    }
    for (int i = 0; i < 6; i++)
    {
        QString ele = thirdLineElements[i];
        float val = ele.toDouble();
        controllerMatrix[2][i] = val;
    }

    IDatagram* datagram = m_interpreter->Create_CMD_Set_ADCS_Controller(controllerId, controllerMatrix);

    this->SendDatagram(datagram);
}





static std::vector<ephemerides_t> g_ephemeridesControllerStack;

void MainWindow::on_Satelliteconfig_ADCs_Ephemerides_DataStack_Push_Button_clicked()
{
    QString solar = ui->SatelliteConfig_ADCs_Ephemerides_SolarEphemeris_LineEdit->text();
    QStringList solarElements = solar.split(',');

    QString magnetic = ui->SatelliteConfig_ADCs_Ephemerides_MagneticEphemeris_LineEdit->text();
    QStringList magneticElements = magnetic.split(',');

    uint8_t controllerId = ui->SatelliteConfig_ADCs_Ephemerides_ControllerID_SpinBox->text().toUInt();

    ephemerides_t ephemeridesStruct;
    ephemeridesStruct.solar_x = solarElements[0].toFloat();
    ephemeridesStruct.solar_y = solarElements[1].toFloat();
    ephemeridesStruct.solar_z = solarElements[2].toFloat();
    ephemeridesStruct.magnetic_x = magneticElements[0].toFloat();
    ephemeridesStruct.magnetic_y = magneticElements[1].toFloat();
    ephemeridesStruct.magnetic_z = magneticElements[2].toFloat();
    ephemeridesStruct.controllerId = controllerId;

    g_ephemeridesControllerStack.push_back(ephemeridesStruct);

    ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
}


void MainWindow::on_Satelliteconfig_ADCs_Ephemerides_DataStack_Clear_Button_clicked()
{
    g_ephemeridesControllerStack.clear();
    ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
}

void MainWindow::on_aSatelliteconfig_ADCs_Ephemerides_DataStack_Send_Button_clicked()
{
    uint16_t chunkId = ui->SatelliteConfig_ADCs_Ephemerides_ChunkID_SpinBox->value();

    IDatagram* datagram = m_interpreter->Create_CMD_Set_ADCS_Ephemerides(chunkId, g_ephemeridesControllerStack);
    this->SendDatagram(datagram);

    g_ephemeridesControllerStack.clear();

    ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
}

void MainWindow::on_EEPROM_Control_Wipe_Button_clicked()
{
    // fossasat-2.
    char wipeSystemInfo = ui->EEPROM_Control_Wipe_SystemInfoAndConfig_CheckBox->isChecked();
    char wipeStatistics = ui->EEPROM_Control_Wipe_Statistics_CheckBox->isChecked();
    char wipeStoreAndForward = ui->EEPROM_Control_Wipe_StoreAndForwardFrames_CheckBox->isChecked();
    char wipeNMEALog = ui->EEPROM_Control_Wipe_NMEALOG_CheckBox->isChecked();
    char wipeImageStorage = ui->EEPROM_Control_Wipe_ImageStorage_CheckBox->isChecked();
    char wipeADCSparameters = ui->EEPROM_Control_Wipe_ADCS_CheckBox->isChecked();
    char wipeADCSEphemerides = ui->EEPROM_Control_Wipe_Ephemerides_CheckBox->isChecked();

    char flags = wipeSystemInfo | wipeStatistics | wipeStoreAndForward | wipeNMEALog | wipeImageStorage | wipeADCSparameters | wipeADCSEphemerides;

    IDatagram* datagram = m_interpreter->Create_CMD_Wipe_EEPROM(flags);
    this->SendDatagram(datagram);
}

void MainWindow::on_SatelliteConfig_Transmission_Send_Button_clicked()
{
    // fossasat-2.
    char transmitEnabled = ui->SatelliteConfig_Transmission_Enabled_RadioButton->isChecked();
    char automatedStatsTransmissionEnabled = ui->SatelliteConfig_Transmission_AutoStatsEnabled_RadioButton->isChecked();
    char FSKMandatedForLargePacketsEnabled = ui->SatelliteConfig_Transmission_FSKMandated_Enabled_RadioButton->isChecked();

    IDatagram* datagram = m_interpreter->Create_CMD_Set_Transmit_Enable(transmitEnabled, automatedStatsTransmissionEnabled, FSKMandatedForLargePacketsEnabled);
    this->SendDatagram(datagram);
}


#define SatelliteControlsTab_End }












//////////////////////////////////
// Satellite configuration tab. //
//////////////////////////////////
#define SatelliteConfigurationTab_Start {
void MainWindow::LoadSatelliteConfigurationUI()
{

}


void MainWindow::on_SatelliteConfig_SatelliteVersion_SetVersion_PushButton_clicked()
{
    QString text = ui->SatelliteConfig_SatelliteVersion_ComboBox->currentText();

    if ( (text != "FOSSASAT-1B") && (text != "FOSSASAT-2") )
    {
        throw "invalid satellite version.";
    }

    Settings::SetSatVersion(text);
}
#define SatelliteConfigurationTab_End }





















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
    Settings::SetHandshookValue(false);

    //
    // Handshake the ground station.
    //
    char handshakeMessage = 0x00;
    QByteArray msg;
    msg.append(handshakeMessage);
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
    bool keyLoaded = Settings::LoadKeyFromSettings();
    if (keyLoaded)
    {
        Settings::SetKeySet();

        uint8_t* key = Settings::GetKey();
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
    bool passwordLoaded = Settings::LoadPasswordFromSettings();
    if (passwordLoaded)
    {
        Settings::SetPasswordSet();

        std::string password = Settings::GetPassword();
        QString passwordStr = QString::fromStdString(password);
        this->ui->ControlPanelSettings_securityPasswordLineEdit->setText(passwordStr);
    }


    //
    // Load the latitude, longitude and altitude settings into the UI
    //
    bool latLongAltLoaded = Settings::LoadLatLongAlt();

    if (latLongAltLoaded)
    {
        QString latStr = QString::number(Settings::GetLatitude());
        QString lonStr = QString::number(Settings::GetLongitude());
        QString altStr = QString::number(Settings::GetAltitude());

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
    bool dopplerShiftCorrectionEnabled = Settings::LoadDopplerShiftCorrectionEnabled();

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
    Settings::LoadTLElines();

    std::string tleLine1 = Settings::GetTLELine1();
    std::string tleLine2 = Settings::GetTLELine2();

    QString tleLine1Str = QString::fromStdString(tleLine1);
    QString tleLine2Str = QString::fromStdString(tleLine2);

    this->ui->ControlPanelSettings_Doppler_Shift_TLE_1_LineEdit->setText(tleLine1Str);
    this->ui->ControlPanelSettings_Doppler_Shift_TLE_2_LineEdit->setText(tleLine2Str);

    //
    // Configure the simulation if enabled.
    //
    if (dopplerShiftCorrectionEnabled)
    {
        double latitude = Settings::GetLatitude();
        double longitude = Settings::GetLongitude();
        double attitude = Settings::GetAltitude();

        m_dopplerShiftCorrector.SetObserverParameters(latitude, longitude, attitude);
        m_dopplerShiftCorrector.SetTLELines(tleLine1, tleLine2);

        this->ui->statusbar->showMessage("Doppler shift correction configured successfully.", 10);

        m_dopplerCorrectionTimer->start(1000 * 10);
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

    uint8_t keyBytes[16];
    QStringList keyHexElements = key.split(',');
    for (int i = 0; i < keyHexElements.size(); i++)
    {
        QString hexElement = keyHexElements[i];
        bool status = false;
        uint8_t hexDigit = hexElement.toUInt(&status, 16);
        if (status == false)
        {
            throw "could not convert hex digit, e.g. 0x01";
        }
        keyBytes[i] = hexDigit;
    }


    Settings::SetPassword(passwordStdStr);
    Settings::SavePasswordToSettings();

    Settings::SetKey(keyBytes);
    Settings::SaveKeyToSettings();
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

    Settings::SetLatitude(latitude);
    Settings::SetLongitude(longitude);
    Settings::SetAltitude(attitude);

    Settings::SaveLatLongAlt();

    bool dopplerShiftCorrectionEnabled = this->ui->ControlPanelSettings_Doppler_Shift_Enable_RadioButton->isChecked();

    Settings::SetDopplerShiftCorrectionEnabled(dopplerShiftCorrectionEnabled);
    Settings::SaveDopplerShiftCorrectionEnabled();

    std::string tleLine1 = this->ui->ControlPanelSettings_Doppler_Shift_TLE_1_LineEdit->text().toStdString();
    std::string tleLine2 = this->ui->ControlPanelSettings_Doppler_Shift_TLE_2_LineEdit->text().toStdString();

    Settings::SetTLELine1(tleLine1);
    Settings::SetTLELine2(tleLine2);

    Settings::SaveTLELines();
}


void MainWindow::on_ControlPanelSettings_Doppler_Shift_Enable_RadioButton_clicked()
{
    double latitude = this->ui->ControlPanelSettings_Doppler_Shift_Latitude_LineEdit->text().toDouble();
    double longitude = this->ui->ControlPanelSettings_Doppler_Shift_Longitude_LineEdit->text().toDouble();
    double attitude = this->ui->ControlPanelSettings_Doppler_Shift_Altitude_LineEdit->text().toDouble();

    std::string tleLine1 = this->ui->ControlPanelSettings_Doppler_Shift_TLE_1_LineEdit->text().toStdString();
    std::string tleLine2 = this->ui->ControlPanelSettings_Doppler_Shift_TLE_2_LineEdit->text().toStdString();

    this->ui->statusbar->showMessage("Doppler shift correction started!", 10);

    m_dopplerCorrectionTimer->start(1000 * 10); // 10s between doppler shift corrections.
}


void MainWindow::on_ControlPanelSettings_Doppler_Shift_Disable_RadioButton_clicked()
{
    this->ui->statusbar->showMessage("Doppler shift correction stopped.", 10);

    m_dopplerCorrectionTimer->stop();
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
