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




/////////////////////////////
// Satellite controls tab. //
/////////////////////////////
#define SatelliteControlsTab_Start {
void MainWindow::LoadSatelliteControlsUI()
{

}



void MainWindow::on_SatelliteControls_BaseOps_Ping_Button_clicked()
{
    IDatagram* datagram = m_interpreter->Create_CMD_Ping();
    this->SendDatagram(datagram);
}

void MainWindow::on_SatelliteControls_BaseOps_Restart_Button_clicked()
{
    IDatagram* datagram = m_interpreter->Create_CMD_Restart();
    this->SendDatagram(datagram);
}

void MainWindow::on_SatelliteControls_BaseOps_Deploy_Button_clicked()
{
    IDatagram* datagram = m_interpreter->Create_CMD_Deploy();
    this->SendDatagram(datagram);
}

void MainWindow::on_SatelliteControls_BaseOps_Abort_Button_clicked()
{
    IDatagram* datagram = m_interpreter->Create_CMD_Abort();
    this->SendDatagram(datagram);
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
    uint8_t pictureSlot = ui->CameraControl_GetPictureLength_PictureSlot_SpinBox->value();

    IDatagram* datagram = m_interpreter->Create_CMD_Get_Picture_Length(pictureSlot);
    this->SendDatagram(datagram);
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


void MainWindow::on_GPSControl_GetGPSState_GetLogState_Button_clicked()
{
    IDatagram* datagram = m_interpreter->Create_CMD_Get_GPS_Log_State();
    this->SendDatagram(datagram);
}

void MainWindow::on_GPSControl_LogGPS_StartLogging_Button_clicked()
{
    uint32_t loggingDuration = ui->GPSControl_LogGPS_LoggingDuration_LineEdit->text().toInt();
    uint32_t loggingStartOffset = ui->GPSControl_LogGPS_StartOffset_LineEdit->text().toInt();

    IDatagram* datagram = m_interpreter->Create_CMD_Log_GPS(loggingDuration, loggingStartOffset);
    this->SendDatagram(datagram);
}

void MainWindow::on_GPSControl_GetGPSLog_GetLog_Button_clicked()
{
    uint8_t newestEntriesFirst;
    if ((ui->GPSControl_GetGPSLog_NewestFirst_RadioButton->isChecked()) && (!ui->GPSControl_GetGPSLog_OldestFirst_RadioButton->isChecked()))
    {
        newestEntriesFirst = 1;
    }
    else if (!ui->GPSControl_GetGPSLog_NewestFirst_RadioButton->isChecked() && (ui->GPSControl_GetGPSLog_OldestFirst_RadioButton->isChecked()))
    {
        newestEntriesFirst = 0;
    }

    int16_t gpsLogStartOffset = ui->GPSControl_GetGPSLog_Offset_SpinBox->value();

    uint16_t numNMEASentencesDownlink = ui->GPSControl_GetGPSLog_NMEASeqCount_SpinBox->value(); // 0 = all

    IDatagram* datagram = m_interpreter->Create_CMD_Get_GPS_Log(newestEntriesFirst, gpsLogStartOffset, numNMEASentencesDownlink);
    this->SendDatagram(datagram);
}

void MainWindow::on_GPSControl_RunGPSCmd_RunCommand_Button_clicked()
{
    QString binMessage = ui->GPSControl_RunGPSCmd_SkytraqBinary_LineEdit->text();
    std::string binMessageStr = binMessage.toStdString();
    const char * binMessageCStr = binMessageStr.c_str();

    IDatagram* datagram = m_interpreter->Create_CMD_Run_GPS_Command((char*)binMessageCStr);
    this->SendDatagram(datagram);
}


void MainWindow::on_StoreAndForward_Message_AddMessage_Button_clicked()
{
    uint32_t messageId = ui->StoreAndForward_Message_MessageID_SpinBox->value();
    QString messageStr = ui->StoreAndForward_Message_MessageContent_PlainTextEdit->toPlainText();
    std::string messageStdStr = messageStr.toStdString();
    const char * messageCStr = messageStdStr.c_str();


    IDatagram* datagram = m_interpreter->Create_CMD_Store_And_Forward_Add(messageId, (char*)messageCStr);
    this->SendDatagram(datagram);
}

void MainWindow::on_storeAndForwardStoreAndForward_RequestMessage_RequestMessage_Button_clicked()
{
    uint32_t messageId = ui->StoreAndForward_RequestMessage_MessageID_SpinBox->value();

    IDatagram* datagram = m_interpreter->Create_CMD_Store_And_Forward_Request(messageId);
    this->SendDatagram(datagram);

}

void MainWindow::on_TransmissionRouter_Retransmission_Retransmit_Button_clicked()
{
    uint32_t senderId = ui->TransmissionRouter_Retransmission_SendID_SpinBox->value();
    QString msg = ui->TransmissionRouter_Retransmission_MessageContent_PlainTextEdit->toPlainText();
    std::string msgStdStr = msg.toStdString();
    char * msgCStr = (char*)msgStdStr.c_str();

    IDatagram* datagram = m_interpreter->Create_CMD_Retransmit(senderId, msgCStr);
    this->SendDatagram(datagram);
}

void MainWindow::on_TransmissionRouter_RetransmissionCustom_RetransmitCustom_Button_clicked()
{
    uint8_t bandwidth = 0;
    if (ui->TransmissionRouter_RetransmissionCustom_Bandwidth_78_RadioButton->isChecked())
    {
        bandwidth = 0x00;
    }
    else if (ui->TransmissionRouter_RetransmissionCustom_Bandwidth_125_RadioButton->isChecked())
    {
        bandwidth = 0x07;
    }

    uint8_t spreadingFactor = 0;
    if (ui->TransmissionRouter_RetransmissionCustom_SpreadingFactor_SF5_RadioButton->isChecked())
    {
        spreadingFactor = 0x00;
    }
    else if (ui->TransmissionRouter_RetransmissionCustom_SpreadingFactor_SF12_RadioButton->isChecked())
    {
        spreadingFactor = 0x07;
    }

    uint8_t codingRate = 0;
    if (ui->TransmissionRouter_RetransmissionCustom_CodingRate_45_RadioButton->isChecked())
    {
        codingRate = 0x00;
    }
    else if (ui->TransmissionRouter_RetransmissionCustom_CodingRate_48_RadioButton->isChecked())
    {
        spreadingFactor = 0x08;
    }

    uint16_t preambleLength = ui->TransmissionRouter_RetransmissionCustom_PreambleLength_SpinBox->value();


    uint8_t crcEnabled = 0;
    if (ui->TransmissionRouter_RetransmissionCustom_CRCEnabled_Enabled_RadioButton->isChecked())
    {
        codingRate = 0x01;
    }
    else if (ui->TransmissionRouter_RetransmissionCustom_CRCEnabled_Disabled_RadioButton->isChecked())
    {
        spreadingFactor = 0x00;
    }

    int8_t outputPower = ui->TransmissionRouter_RetransmissionCustom_OutputPower_SpinBox->value();

    uint32_t senderId = ui->TransmissionRouter_RetransmissionCustom_SenderID_SpinBox->value();

    QString msg = ui->TransmissionRouter_RetransmissionCustom_MessageContent_PlainTextEdit->toPlainText();
    std::string msgStdStr = msg.toStdString();
    const char * msgCStr = msgStdStr.c_str();

    IDatagram* datagram = m_interpreter->Create_CMD_Retransmit_Custom(bandwidth, spreadingFactor, codingRate, preambleLength, crcEnabled, outputPower, senderId, (char*)msgCStr);
    this->SendDatagram(datagram);
}

void MainWindow::on_RouteCommand_RouteCommand_Button_clicked()
{
    QString frame = ui->RouteCommand_FCPFrameContent_PlainTextEdit->toPlainText();
    std::string frameStr = frame.toStdString();
    const char* frameCStr = frameStr.c_str();

    IDatagram* datagram = m_interpreter->Create_CMD_Route((char*)frameCStr);
    this->SendDatagram(datagram);
}

void MainWindow::on_RecordIMU_RecordIMU_Button_clicked()
{
    uint16_t numSamples = ui->RecordIMU_NumberOfSamples_SpinBox->value();
    uint16_t samplingPeriod = ui->RecordIMU_SamplingPeriod_SpinBox->value();

    uint8_t flags = 0;

    if (ui->RecordIMU_DeviceSelection_Gyroscope_Checkbox->isChecked()) {
        flags |= 0x01;
    }

    if (ui->RecordIMU_DeviceSelection_Accelerometer_Checkbox->isChecked()) {
        flags |= 0x02;
    }

    if (ui->RecordIMU_DeviceSelection_Magnetometer_Checkbox->isChecked()) {
        flags |= 0x04;
    }

    IDatagram* datagram = m_interpreter->Create_CMD_Record_IMU(numSamples, samplingPeriod, flags);
    this->SendDatagram(datagram);
}


void MainWindow::on_FlashControl_SetFlashContent_Button_clicked()
{
    bool ok;
    uint32_t flashAddress = ui->FlashControl_FlashAddress_LineEdit->text().toInt(&ok, 16);
    if (!ok)
    {
        QMessageBox msgBox;
        msgBox.setText("Flash address must be a hex string e.g. 0xFF");
        msgBox.exec();
    }

    QString flashData = ui->FlashControl_FlashData_PlainTextBox->toPlainText();
    std::string flashDataStdStr = flashData.toStdString();
    const char * flashDataCStr = flashDataStdStr.c_str();


    IDatagram* datagram = m_interpreter->Create_CMD_Set_Flash_Contents(flashAddress, (char*)flashDataCStr);
    this->SendDatagram(datagram);
}

void MainWindow::on_FlashControl_GetFlashContent_Get_Button_clicked()
{
    bool ok;
    uint32_t flashAddress = ui->FlashControl_GetFlashContent_Address_LineEdit->text().toInt(&ok, 16);
    if (!ok)
    {
        QMessageBox msgBox;
        msgBox.setText("Flash address must be a hex string e.g. 0xFF");
        msgBox.exec();
    }

    uint8_t numBytesToRead = ui->FlashControl_GetFlashContent_NumberOfBytes_SpinBox->value();

    IDatagram* datagram = m_interpreter->Create_CMD_Get_Flash_Contents(flashAddress, numBytesToRead);
    this->SendDatagram(datagram);
}

void MainWindow::on_FlashControl_EraseFlash_Erase_Button_clicked()
{
    bool ok;
    uint32_t addressToErase = ui->FlashControl_EraseFlash_Address_LineEdit->text().toInt(&ok, 16);
    if (!ok)
    {
        QMessageBox msgBox;
        msgBox.setText("Flash sector address must be a hex string e.g. 0xFF");
        msgBox.exec();
    }

    IDatagram* datagram = m_interpreter->Create_CMD_Erase_Flash(addressToErase);
    this->SendDatagram(datagram);
}

void MainWindow::on_ADCSManeuvering_RunADCManeuver_Button_clicked()
{
    int8_t xAxisHBridgeHighMag = ui->ADCSManeuvering_HBridge_X_High_SpinBox->value();
    int8_t xAxisHBridgeLowMag = ui->ADCSManeuvering_HBridge_X_Low_SpinBox->value();
    int8_t yAxisHBridgeHighMag = ui->ADCSManeuvering_HBridge_Y_High_SpinBox->value();
    int8_t yAxisHBridgeLowMag = ui->ADCSManeuvering_HBridge_Y_Low_SpinBox->value();
    int8_t zAxisHBridgeHighMag = ui->ADCSManeuvering_HBridge_Z_High_SpinBox->value();
    int8_t zAxisHBridgeLowhMag = ui->ADCSManeuvering_HBridge_Z_Low_SpinBox->value();

    uint32_t xAxisPulseLength = ui->ADCSManeuvering_X_PulseLength_SpinBox->value();
    uint32_t yAxisPulseLength = ui->ADCSManeuvering_Y_PulseLength_SpinBox->value();
    uint32_t zAxisPulseLength = ui->ADCSManeuvering_Z_PulseLength_SpinBox->value();

    uint32_t maneuverDuration = ui->ADCSManeuvering_ManeuverDuration_SpinBox->value();

    uint8_t flags = 0;
    if (ui->ADCSManeuvering_IgnoreFaults_XAxis_CheckBox->isChecked())
    {
        flags |= 0x01;
    }
    if (ui->ADCSManeuvering_IgnoreFaults_YAxis_CheckBox->isChecked())
    {
        flags |= 0x02;
    }
    if (ui->ADCSManeuvering_IgnoreFaults_ZAxis_CheckBox->isChecked())
    {
        flags |= 0x04;
    }

    IDatagram* datagram = m_interpreter->Create_CMD_Run_Manual_ACS(xAxisHBridgeHighMag, xAxisHBridgeLowMag, yAxisHBridgeHighMag, yAxisHBridgeLowMag, zAxisHBridgeHighMag, zAxisHBridgeLowhMag, xAxisPulseLength, yAxisPulseLength, zAxisPulseLength, maneuverDuration, flags);
    this->SendDatagram(datagram);
}

void MainWindow::on_SatelliteControls_Maneuver_Run_Button_clicked()
{
    uint8_t overrideXAxisFaultCheck = 0;
    if (ui->SatelliteControls_RunManeuver_EnableOverride_XAxisHbridgeFaultCheck_RadioButton->isChecked())
    {
        overrideXAxisFaultCheck = 1;
    }
    else if (ui->SatelliteControls_RunManeuver_DisableOverride_XAxisHbridgeFaultCheck_RadioButton->isChecked())
    {
        overrideXAxisFaultCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("X H-Bridge fault check override radio buttons must be set.");
        msgBox.exec();
    }

    uint8_t overrideYAxisFaultCheck = 0;
    if (ui->SatelliteControls_RunManeuver_EnableOverride_YAxisHbridgeFaultCheck_RadioButton->isChecked())
    {
        overrideYAxisFaultCheck = 1;
    }
    else if (ui->SatelliteControls_RunManeuver_DisableOverride_YAxisHbridgeFaultCheck_RadioButton->isChecked())
    {
        overrideYAxisFaultCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Y H-Bridge fault check override radio buttons must be set.");
        msgBox.exec();
    }

    uint8_t overrideZAxisFaultCheck = 0;
    if (ui->SatelliteControls_RunManeuver_EnableOverride_ZAxisHbridgeFaultCheck_RadioButton->isChecked())
    {
        overrideZAxisFaultCheck = 1;
    }
    else if (ui->SatelliteControls_RunManeuver_DisableOverride_ZAxisHbridgeFaultCheck_RadioButton->isChecked())
    {
        overrideZAxisFaultCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Z H-Bridge fault check override radio buttons must be set.");
        msgBox.exec();
    }

    uint8_t overrideEulerAngleToleranceCheck = 0;
    if (ui->SatelliteControls_RunManeuver_EnableOverride_EulerAngleToleranceCheck_RadioButton->isChecked())
    {
        overrideEulerAngleToleranceCheck = 1;
    }
    else if (ui->SatelliteControls_RunManeuver_DisableOverride_EulerAngleToleranceCheck_RadioButton->isChecked())
    {
        overrideEulerAngleToleranceCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Euler angle tolerance check radio buttons must be set.");
        msgBox.exec();
    }


    uint8_t overrideAngularVelocityToleranceCheck = 0;
    if (ui->SatelliteControls_RunManeuver_EnableOverride_AngularVelocityToleranceCheck_RadioButton_2->isChecked())
    {
        overrideAngularVelocityToleranceCheck = 1;
    }
    else if (ui->SatelliteControls_RunManeuver_DisableOverride_AngularVelocityToleranceCheck_RadioButton->isChecked())
    {
        overrideAngularVelocityToleranceCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Angular velocity tolerance check radio buttons must be set.");
        msgBox.exec();
    }

    bool ok;
    uint32_t maneuverLength = ui->SatelliteControls_ManeuverLength_LineEdit->text().toInt(&ok, 10);
    if (!ok)
    {
        QMessageBox msgBox;
        msgBox.setText("Maneuver length entered is an invalid number.");
        msgBox.exec();
    }

    uint8_t flags = overrideXAxisFaultCheck;
    flags |= (overrideYAxisFaultCheck << 1);
    flags |= (overrideZAxisFaultCheck << 2);
    flags |= (overrideEulerAngleToleranceCheck << 4);
    flags |= (overrideAngularVelocityToleranceCheck << 5);

    IDatagram* datagram = m_interpreter->Create_CMD_Maneuver(flags, maneuverLength);
    this->SendDatagram(datagram);
}

void MainWindow::on_Detumble_Execute_Button_2_clicked()
{

    uint8_t overrideXAxisFaultCheck = 0;
    if (ui->Detumble_OverrideXHBridge_FaultCheck_Enable_RadioButton->isChecked())
    {
        overrideXAxisFaultCheck = 1;
    }
    else if (ui->Detumble_OverrideXHBridge_FaultCheck_Disable_RadioButton->isChecked())
    {
        overrideXAxisFaultCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("X H-Bridge fault check override radio buttons must be set.");
        msgBox.exec();
    }

    uint8_t overrideYAxisFaultCheck = 0;
    if (ui->Detumble_OverrideYHBridge_FaultCheck_Enable_RadioButton->isChecked())
    {
        overrideYAxisFaultCheck = 1;
    }
    else if (ui->Detumble_OverrideYHBridge_FaultCheck_Disable_RadioButton->isChecked())
    {
        overrideYAxisFaultCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Y H-Bridge fault check override radio buttons must be set.");
        msgBox.exec();
    }

    uint8_t overrideZAxisFaultCheck = 0;
    if (ui->Detumble_OverrideZHBridge_FaultCheck_Enable_RadioButton->isChecked())
    {
        overrideZAxisFaultCheck = 1;
    }
    else if (ui->Detumble_OverrideZHBridge_FaultCheck_Disable_RadioButton->isChecked())
    {
        overrideZAxisFaultCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Z H-Bridge fault check override radio buttons must be set.");
        msgBox.exec();
    }


    uint8_t overrideDetumblingAngularVelocityToleranceCheck = 0;
    if (ui->Detumble_OverrideAngleVelocity_FaultCheck_Enable_RadioButton->isChecked())
    {
        overrideDetumblingAngularVelocityToleranceCheck = 1;
    }
    else if (ui->Detumble_OverrideAngleVelocity_FaultCheck_Disable_RadioButton->isChecked())
    {
        overrideDetumblingAngularVelocityToleranceCheck = 0;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Detumbling angular velocity fault check override radio buttons must be set.");
        msgBox.exec();
    }

    bool ok;
    uint32_t detumblingLength = ui->Detumble_Length_LineEdit->text().toInt(&ok, 10);
    if (!ok)
    {
        QMessageBox msgBox;
        msgBox.setText("Detumbling length entered is an invalid number.");
        msgBox.exec();
    }

    uint8_t flags = overrideXAxisFaultCheck;
    flags |= (overrideYAxisFaultCheck << 1);
    flags |= (overrideZAxisFaultCheck << 2);
    flags |= (overrideDetumblingAngularVelocityToleranceCheck << 3);

    IDatagram* datagram = m_interpreter->Create_CMD_Detumble(flags, detumblingLength);
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


void MainWindow::on_SatelliteConfig_callsignSetButton_clicked()
{
    QString callsign = ui->SatelliteConfig_callsignLineEdit->text();
    std::string callsignStdStr = callsign.toStdString();
    const char * callsignCStr = callsignStdStr.c_str();

    IDatagram* datagram = m_interpreter->Create_CMD_Set_Callsign((char*)(callsignCStr));
    this->SendDatagram(datagram);
}

void MainWindow::on_SatelliteConfig_TLESetButton_clicked()
{
    QString tle = ui->SatelliteConfig_callsignLineEdit->text();
    std::string tleStdStr = tle.toStdString();
    const char * tleCStr = tleStdStr.c_str();

    IDatagram* datagram = m_interpreter->Create_CMD_Set_TLE((char*)(tleCStr));
    this->SendDatagram(datagram);
}


void MainWindow::on_SatelliteConfig_ReceiveWindow_SetButton_clicked()
{
    uint8_t fskrxLen = ui->SatelliteConfig_ReceiveWindow_FSKRXLength_SpinBox->value();
    uint8_t lorarxLen = ui->SatelliteConfig_ReceiveWindow_LoraRXLength_SpinBox->value();

    IDatagram* datagram = m_interpreter->Create_CMD_Set_Receive_Windows(fskrxLen, lorarxLen);
    this->SendDatagram(datagram);
}

void MainWindow::on_SatelliteConfig_PowerLimits_Set_Button_clicked()
{
    int16_t deploymentVoltageLimit = ui->SatelliteConfig_PowerLimits_DeploymentVoltage_SpinBox->value();
    int16_t heaterVoltageLimit = ui->SatelliteConfig_PowerLimits_HeaterVoltage_SpinBox->value();
    int16_t cwBeepVoltageLimit = ui->SatelliteConfig_PowerLimits_CWBeepVoltage_SpinBox->value();
    int16_t lowPowerVoltageLimit = ui->SatelliteConfig_PowerLimits_LowPowerVoltage_SpinBox->value();

    bool ok = false;
    float heaterTemperature = ui->SatelliteConfig_PowerLimits_HeaterTemperature_LineEdit->text().toFloat(&ok);
    if (!ok)
    {
        QMessageBox msgBox;
        msgBox.setText("Heater temperature limit input invalid.");
        msgBox.exec();
    }


    float mpptSwichTemperature = ui->SatelliteConfig_PowerLimits_MPPTSwitchTemperature_LineEdit->text().toFloat(&ok);
    if (!ok)
    {
        QMessageBox msgBox;
        msgBox.setText("MPPT temperature switch limit input invalid.");
        msgBox.exec();
    }


    uint8_t heaterDutyCycle = ui->SatelliteConfig_PowerLimits_HeaterDutyCycle_SpinBox->value();

    IDatagram* datagram = m_interpreter->Create_CMD_Set_Power_Limits(deploymentVoltageLimit, heaterVoltageLimit, cwBeepVoltageLimit, lowPowerVoltageLimit, heaterTemperature, mpptSwichTemperature, heaterDutyCycle);
    this->SendDatagram(datagram);
}


void MainWindow::on_SatelliteConfig_SpreadingFactor_Set_Button_clicked()
{
    bool standardChecked = ui->SatelliteConfig_SpreadingFactor_StandardSetting_RadioButton->isChecked();
    bool alternChecked = ui->SatelliteConfig_SpreadingFactor_AlternativeSetting_RadioButton->isChecked();

    uint8_t sfMode = 0;
    if (standardChecked)
    {
        sfMode = 0;
    }
    else if (alternChecked)
    {
        sfMode= 1;
    }
    else
    {
        sfMode = 0;
    }

    IDatagram* datagram = m_interpreter->Create_CMD_Set_SF_Mode(sfMode);
    this->SendDatagram(datagram);
}



static std::vector<sleep_interval_t> g_sleepIntervalStack;

void MainWindow::on_SatelliteConfig_SleepInterval_PushToStack_Button_clicked()
{
    int16_t firstVoltageLevel = ui->SatelliteConfig_SleepInterval_VoltageLevelFirst_SpinBox->value();
    uint16_t firstSleepInterval = ui->SatelliteConfig_SleepInterval_FirstSleepIntervalLength_SpinBox->value();
    uint16_t secondSleepInvtervals = ui->SatelliteConfig_SleepInterval_SecondSleepIntervalVoltages_SpinBox->value();
    uint16_t thirdSleepInterval = ui->SatelliteConfig_SleepInterval_ThirdSleepInterval_SpinBox->value();


    sleep_interval_t sleepInterval;
    sleepInterval.firstSleepIntervalVoltageLevel = firstVoltageLevel;
    sleepInterval.firstSleepIntervalLength = firstSleepInterval;
    sleepInterval.secondSleepIntervalLength = secondSleepInvtervals;
    sleepInterval.thirdSleepIntervalLength = thirdSleepInterval;

    g_sleepIntervalStack.push_back(sleepInterval);
    ui->SatelliteConfig_SleepInterval_Stack_StackCount_SpinBox->setValue(g_sleepIntervalStack.size());
}

void MainWindow::on_SatelliteConfig_SleepInterval_ClearInterval_Button_clicked()
{
    g_sleepIntervalStack.clear();
    ui->SatelliteConfig_SleepInterval_Stack_StackCount_SpinBox->setValue(0);
}



void MainWindow::on_SatelliteConfig_SleepInterval_SendoStack_Button_clicked()
{
    IDatagram* datagram = m_interpreter->Create_CMD_Set_Sleep_Intervals(g_sleepIntervalStack);
    this->SendDatagram(datagram);
    g_sleepIntervalStack.clear();
    ui->SatelliteConfig_SleepInterval_Stack_StackCount_SpinBox->setValue(0);
}


void MainWindow::on_SatelliteConfig_MPPT_Set_Button_clicked()
{
    bool temperatureSwitchEnabled = ui->SatelliteConfig_MPPT_TemperatureSwitchEnabled_RadioButton->isChecked();
    bool temperatureSwitchDisabled = ui->SatelliteConfig_MPPT_TemperatureSwitchDisabled_RadioButton->isChecked();
    bool keepAliveSwitchEnabled = ui->SatelliteConfig_MPPT_KeepAliveEnabled_RadioButton->isChecked();
    bool keepAliveSwitchDisabled = ui->SatelliteConfig_MPPT_KeepAliveDisabled_RadioButton->isChecked();

    uint8_t tempSwitchState = 1;
    if (temperatureSwitchEnabled)
    {
        tempSwitchState = 1;
    }
    else if (temperatureSwitchDisabled)
    {
        tempSwitchState = 0;
    }

    uint8_t keepAliveState = 0;
    if (keepAliveSwitchEnabled)
    {
        keepAliveState = 1;
    }
    else if (keepAliveSwitchDisabled)
    {
        keepAliveState = 0;
    }


    IDatagram* datagram = m_interpreter->Create_CMD_Set_MPPT_Mode(tempSwitchState, keepAliveState);
    this->SendDatagram(datagram);
}

#define SatelliteConfigurationTab_End }




