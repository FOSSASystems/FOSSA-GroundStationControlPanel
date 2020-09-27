#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Interpreter.h>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // load the message log frame
    m_messageLogFrame = new MessageLogFrame();
    m_messageLogFrame->show();

    // load the system information pane
    m_sytemInfoPane = new systeminformationpane();
    m_sytemInfoPane->show();

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
        m_datagramData.push_back(data);

        if (m_lengthByte)
        {
            m_messageLength = data + 2;
            m_fcpFrameLength = data - 2;

            m_lengthByte = false;
        }

        if (m_datagramData.size() >= m_messageLength)
        {
            std::vector<uint8_t> tempBuffer;
            for (int i = 0; i < m_datagramData.size(); i++) {
                tempBuffer.push_back(m_datagramData.at(i));
            }

            Datagram datagram = Datagram(Settings::GetSatVersion(), Settings::getCallsign(), tempBuffer, true);

            if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
            {
                if (datagram.GetOperationID() == OperationID::FRAME)
                {
                    Frame frame = datagram.GetFrame();

                    if (datagram.GetFrameFunctionID() == RESP_SYSTEM_INFO)
                    {
                        FOSSASAT1B::Messages::SystemInfo systemInfo = FOSSASAT1B::FrameDecoder::DecodeSystemInfo(frame);
                    }
                }
            }
            else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
            {

            }

            // push the datagram to the log panel.
            m_messageLogFrame->WriteDatagram(datagram);

            // reset data and flags
            m_datagramData.clear();

            m_frameStreaming = false;
        }

        return;
    }
    else
    {
        if (IsControlByte(data))
        {
            m_datagramData.push_back(data);

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

void MainWindow::SendDatagram(Datagram datagram)
{
    // log the datagram.
    m_messageLogFrame->WriteDatagram(datagram);

    // convert it to an byte array.
    QByteArray datagramDataArr;
    std::vector<uint8_t> datagramData = datagram.Serialize();
    for (int i = 0; i < datagramData.size(); i++)
    {
        uint8_t v = datagramData[i];
        datagramDataArr.push_back(v);
    }

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

void MainWindow::ReceivedDatagramfromSystemInformationPane(Datagram datagram)
{
    this->SendDatagram(datagram);
}

void MainWindow::SendDopplerShiftedFrequency()
{
    QString modemType = ui->GroundStationSettings_ModemTypeComboBox->currentText();

    QString currentCarrierFreqStr = ui->GroundStationSettings_ConfigCarrierFrequencyLineEdit->text();

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
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Ping();
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        Datagram datagram = FOSSASAT2::DatagramEncoder::Ping();
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteControls_BaseOps_Restart_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Restart();
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        Datagram datagram = FOSSASAT2::DatagramEncoder::Restart();
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteControls_BaseOps_Deploy_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Deploy();
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        Datagram datagram = FOSSASAT2::DatagramEncoder::Deploy();
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteControls_BaseOps_Abort_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Abort();
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        Datagram datagram = FOSSASAT2::DatagramEncoder::Abort();
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_CameraControl_Capture_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        char pictureSlot = (char)this->ui->CameraControl_PictureSlot_SpinBox->value();
        char lightMode = (char)this->ui->CameraControl_LightMode_SpinBox->value();
        char pictureSize = (char)this->ui->CameraControl_PictureSisze_SpinBox->value();
        char brightness = (char)this->ui->CameraControl_Brightness_SpinBox->value();
        char saturation = (char)this->ui->CameraControl_Saturation_SpinBox->value();
        char specialFilter = (char)this->ui->CameraControl_SpecialFilter_SpinBox->value();
        char contrast = (char)this->ui->CameraControl_Contrast_SpinBox->value();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Camera_Capture(pictureSlot,
                                                                        lightMode,
                                                                        pictureSize,
                                                                        brightness,
                                                                        saturation,
                                                                        specialFilter,
                                                                        contrast);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        char pictureSlot = (char)this->ui->CameraControl_PictureSlot_SpinBox->value();
        char lightMode = (char)this->ui->CameraControl_LightMode_SpinBox->value();
        char pictureSize = (char)this->ui->CameraControl_PictureSisze_SpinBox->value();
        char brightness = (char)this->ui->CameraControl_Brightness_SpinBox->value();
        char saturation = (char)this->ui->CameraControl_Saturation_SpinBox->value();
        char specialFilter = (char)this->ui->CameraControl_SpecialFilter_SpinBox->value();
        char contrast = (char)this->ui->CameraControl_Contrast_SpinBox->value();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Camera_Capture(pictureSlot,
                                                                        lightMode,
                                                                        pictureSize,
                                                                        brightness,
                                                                        saturation,
                                                                        specialFilter,
                                                                        contrast);
        this->SendDatagram(datagram);
    }

}



void MainWindow::on_CameraControl_GetPictureLength_GetPictureLength_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        uint8_t pictureSlot = ui->CameraControl_GetPictureLength_PictureSlot_SpinBox->value();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Get_Picture_Length(pictureSlot);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        uint8_t pictureSlot = ui->CameraControl_GetPictureLength_PictureSlot_SpinBox->value();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Get_Picture_Length(pictureSlot);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_CameraControl_PictureBurst_GetPictureBurst_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        char pictureSlot = (char)this->ui->CameraControl_PictureBurst_PictureSlot_SpinBox->value();

        QString id = this->ui->CameraControl_PictureBurst_PicturePacketID_LineEdit->text();
        uint16_t packetId = id.toUInt();

        // 1 is full picture, 0 is scan data.
        char fullPictureOrScandata = (char)this->ui->CameraControl_PictureBurst_FullPictureModeFullPicture_RadioButton->isChecked();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Get_Picture_Burst(pictureSlot, packetId, fullPictureOrScandata);

        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        char pictureSlot = (char)this->ui->CameraControl_PictureBurst_PictureSlot_SpinBox->value();

        QString id = this->ui->CameraControl_PictureBurst_PicturePacketID_LineEdit->text();
        uint16_t packetId = id.toUInt();

        // 1 is full picture, 0 is scan data.
        char fullPictureOrScandata = (char)this->ui->CameraControl_PictureBurst_FullPictureModeFullPicture_RadioButton->isChecked();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Get_Picture_Burst(pictureSlot, packetId, fullPictureOrScandata);

        this->SendDatagram(datagram);
    }
}



void MainWindow::on_EEPROM_Control_Wipe_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Wipe_EEPROM();
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        char wipeSystemInfo = ui->EEPROM_Control_Wipe_SystemInfoAndConfig_CheckBox->isChecked();
        char wipeStatistics = ui->EEPROM_Control_Wipe_Statistics_CheckBox->isChecked();
        char wipeStoreAndForward = ui->EEPROM_Control_Wipe_StoreAndForwardFrames_CheckBox->isChecked();
        char wipeNMEALog = ui->EEPROM_Control_Wipe_NMEALOG_CheckBox->isChecked();
        char wipeImageStorage = ui->EEPROM_Control_Wipe_ImageStorage_CheckBox->isChecked();
        char wipeADCSparameters = ui->EEPROM_Control_Wipe_ADCS_CheckBox->isChecked();
        char wipeADCSEphemerides = ui->EEPROM_Control_Wipe_Ephemerides_CheckBox->isChecked();

        char flags = wipeSystemInfo | wipeStatistics | wipeStoreAndForward | wipeNMEALog | wipeImageStorage | wipeADCSparameters | wipeADCSEphemerides;

        Datagram datagram = FOSSASAT2::DatagramEncoder::Wipe_EEPROM(flags);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteConfig_Transmission_Send_Button_clicked()
{

    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        char transmitEnabled = ui->SatelliteConfig_Transmission_Enabled_RadioButton->isChecked();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_Transmit_Enable(transmitEnabled);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        char transmitEnabled = ui->SatelliteConfig_Transmission_Enabled_RadioButton->isChecked();
        char automatedStatsTransmissionEnabled = ui->SatelliteConfig_Transmission_AutoStatsEnabled_RadioButton->isChecked();
        char FSKMandatedForLargePacketsEnabled = ui->SatelliteConfig_Transmission_FSKMandated_Enabled_RadioButton->isChecked();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_Transmit_Enable(transmitEnabled, automatedStatsTransmissionEnabled, FSKMandatedForLargePacketsEnabled);
        this->SendDatagram(datagram);
    }
}


void MainWindow::on_GPSControl_GetGPSState_GetLogState_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Get_GPS_Log_State();
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {

        Datagram datagram = FOSSASAT2::DatagramEncoder::Get_GPS_Log_State();
        this->SendDatagram(datagram);
    }

}

void MainWindow::on_GPSControl_LogGPS_StartLogging_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        uint32_t loggingDuration = ui->GPSControl_LogGPS_LoggingDuration_LineEdit->text().toInt();
        uint32_t loggingStartOffset = ui->GPSControl_LogGPS_StartOffset_LineEdit->text().toInt();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Log_GPS(loggingDuration, loggingStartOffset);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        uint32_t loggingDuration = ui->GPSControl_LogGPS_LoggingDuration_LineEdit->text().toInt();
        uint32_t loggingStartOffset = ui->GPSControl_LogGPS_StartOffset_LineEdit->text().toInt();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Log_GPS(loggingDuration, loggingStartOffset);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_GPSControl_GetGPSLog_GetLog_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Get_GPS_Log(newestEntriesFirst, gpsLogStartOffset, numNMEASentencesDownlink);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Get_GPS_Log(newestEntriesFirst, gpsLogStartOffset, numNMEASentencesDownlink);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_GPSControl_RunGPSCmd_RunCommand_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        QString binMessage = ui->GPSControl_RunGPSCmd_SkytraqBinary_LineEdit->text();
        std::string binMessageStr = binMessage.toStdString();
        const char * binMessageCStr = binMessageStr.c_str();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Run_GPS_Command((char*)binMessageCStr);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        QString binMessage = ui->GPSControl_RunGPSCmd_SkytraqBinary_LineEdit->text();
        std::string binMessageStr = binMessage.toStdString();
        const char * binMessageCStr = binMessageStr.c_str();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Run_GPS_Command((char*)binMessageCStr);
        this->SendDatagram(datagram);
    }
}


void MainWindow::on_StoreAndForward_Message_AddMessage_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        uint32_t messageId = ui->StoreAndForward_Message_MessageID_SpinBox->value();
        QString messageStr = ui->StoreAndForward_Message_MessageContent_PlainTextEdit->toPlainText();
        std::string messageStdStr = messageStr.toStdString();
        const char * messageCStr = messageStdStr.c_str();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Store_And_Forward_Add(messageId, (char*)messageCStr);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        uint32_t messageId = ui->StoreAndForward_Message_MessageID_SpinBox->value();
        QString messageStr = ui->StoreAndForward_Message_MessageContent_PlainTextEdit->toPlainText();
        std::string messageStdStr = messageStr.toStdString();
        const char * messageCStr = messageStdStr.c_str();


        Datagram datagram = FOSSASAT2::DatagramEncoder::Store_And_Forward_Add(messageId, (char*)messageCStr);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_storeAndForwardStoreAndForward_RequestMessage_RequestMessage_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        uint32_t messageId = ui->StoreAndForward_RequestMessage_MessageID_SpinBox->value();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Store_And_Forward_Request(messageId);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        uint32_t messageId = ui->StoreAndForward_RequestMessage_MessageID_SpinBox->value();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Store_And_Forward_Request(messageId);
        this->SendDatagram(datagram);
    }

}

void MainWindow::on_TransmissionRouter_Retransmission_Retransmit_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        uint32_t senderId = ui->TransmissionRouter_Retransmission_SendID_SpinBox->value();
        QString msg = ui->TransmissionRouter_Retransmission_MessageContent_PlainTextEdit->toPlainText();
        std::string msgStdStr = msg.toStdString();
        char * msgCStr = (char*)msgStdStr.c_str();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Retransmit(senderId, msgCStr);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        uint32_t senderId = ui->TransmissionRouter_Retransmission_SendID_SpinBox->value();
        QString msg = ui->TransmissionRouter_Retransmission_MessageContent_PlainTextEdit->toPlainText();
        std::string msgStdStr = msg.toStdString();
        char * msgCStr = (char*)msgStdStr.c_str();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Retransmit(senderId, msgCStr);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_TransmissionRouter_RetransmissionCustom_RetransmitCustom_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Retransmit_Custom(bandwidth, spreadingFactor, codingRate, preambleLength, crcEnabled, outputPower, senderId, (char*)msgCStr);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Retransmit_Custom(bandwidth, spreadingFactor, codingRate, preambleLength, crcEnabled, outputPower, senderId, (char*)msgCStr);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_RouteCommand_RouteCommand_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        QString frame = ui->RouteCommand_FCPFrameContent_PlainTextEdit->toPlainText();
        std::string frameStr = frame.toStdString();
        const char* frameCStr = frameStr.c_str();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Route((char*)frameCStr);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        QString frame = ui->RouteCommand_FCPFrameContent_PlainTextEdit->toPlainText();
        std::string frameStr = frame.toStdString();
        const char* frameCStr = frameStr.c_str();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Route((char*)frameCStr);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_RecordIMU_RecordIMU_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Record_IMU(numSamples, samplingPeriod, flags);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Record_IMU(numSamples, samplingPeriod, flags);
        this->SendDatagram(datagram);
    }
}


void MainWindow::on_FlashControl_SetFlashContent_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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


        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_Flash_Contents(flashAddress, (char*)flashDataCStr);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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


        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_Flash_Contents(flashAddress, (char*)flashDataCStr);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_FlashControl_GetFlashContent_Get_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Get_Flash_Contents(flashAddress, numBytesToRead);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Get_Flash_Contents(flashAddress, numBytesToRead);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_FlashControl_EraseFlash_Erase_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        bool ok;
        uint32_t addressToErase = ui->FlashControl_EraseFlash_Address_LineEdit->text().toInt(&ok, 16);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Flash sector address must be a hex string e.g. 0xFF");
            msgBox.exec();
        }

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Erase_Flash(addressToErase);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        bool ok;
        uint32_t addressToErase = ui->FlashControl_EraseFlash_Address_LineEdit->text().toInt(&ok, 16);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Flash sector address must be a hex string e.g. 0xFF");
            msgBox.exec();
        }

        Datagram datagram = FOSSASAT2::DatagramEncoder::Erase_Flash(addressToErase);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_ADCSManeuvering_RunADCManeuver_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Run_Manual_ACS(xAxisHBridgeHighMag, xAxisHBridgeLowMag, yAxisHBridgeHighMag, yAxisHBridgeLowMag, zAxisHBridgeHighMag, zAxisHBridgeLowhMag, xAxisPulseLength, yAxisPulseLength, zAxisPulseLength, maneuverDuration, flags);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Run_Manual_ACS(xAxisHBridgeHighMag, xAxisHBridgeLowMag, yAxisHBridgeHighMag, yAxisHBridgeLowMag, zAxisHBridgeHighMag, zAxisHBridgeLowhMag, xAxisPulseLength, yAxisPulseLength, zAxisPulseLength, maneuverDuration, flags);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteControls_Maneuver_Run_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Maneuver(flags, maneuverLength);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Maneuver(flags, maneuverLength);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_Detumble_Execute_Button_2_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Detumble(flags, detumblingLength);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Detumble(flags, detumblingLength);
        this->SendDatagram(datagram);
    }

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

    SatVersion satVersion;

    if (text == "FOSSASAT-1B")
    {
        satVersion = SatVersion::V_FOSSASAT1B;
    }
    else if (text == "FOSSASAT-2")
    {
        satVersion = SatVersion::V_FOSSASAT2;
    }

    Settings::SetSatVersion(satVersion);
}


void MainWindow::on_SatelliteConfig_callsignSetButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        QString callsign = ui->SatelliteConfig_callsignLineEdit->text();
        std::string callsignStdStr = callsign.toStdString();
        const char * callsignCStr = callsignStdStr.c_str();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_Callsign((char*)(callsignCStr));
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        QString callsign = ui->SatelliteConfig_callsignLineEdit->text();
        std::string callsignStdStr = callsign.toStdString();
        const char * callsignCStr = callsignStdStr.c_str();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_Callsign((char*)(callsignCStr));
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteConfig_TLESetButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        QString tle = ui->SatelliteConfig_callsignLineEdit->text();
        std::string tleStdStr = tle.toStdString();
        const char * tleCStr = tleStdStr.c_str();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_TLE((char*)(tleCStr));
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        QString tle = ui->SatelliteConfig_callsignLineEdit->text();
        std::string tleStdStr = tle.toStdString();
        const char * tleCStr = tleStdStr.c_str();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_TLE((char*)(tleCStr));
        this->SendDatagram(datagram);
    }
}


void MainWindow::on_SatelliteConfig_ReceiveWindow_SetButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        uint8_t fskrxLen = ui->SatelliteConfig_ReceiveWindow_FSKRXLength_SpinBox->value();
        uint8_t lorarxLen = ui->SatelliteConfig_ReceiveWindow_LoraRXLength_SpinBox->value();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_Receive_Windows(fskrxLen, lorarxLen);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        uint8_t fskrxLen = ui->SatelliteConfig_ReceiveWindow_FSKRXLength_SpinBox->value();
        uint8_t lorarxLen = ui->SatelliteConfig_ReceiveWindow_LoraRXLength_SpinBox->value();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_Receive_Windows(fskrxLen, lorarxLen);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteConfig_PowerLimits_Set_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_Power_Limits(deploymentVoltageLimit, heaterVoltageLimit, cwBeepVoltageLimit, lowPowerVoltageLimit, heaterTemperature, mpptSwichTemperature, heaterDutyCycle);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_Power_Limits(deploymentVoltageLimit, heaterVoltageLimit, cwBeepVoltageLimit, lowPowerVoltageLimit, heaterTemperature, mpptSwichTemperature, heaterDutyCycle);
        this->SendDatagram(datagram);
    }
}


void MainWindow::on_SatelliteConfig_SpreadingFactor_Set_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_SF_Mode(sfMode);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_SF_Mode(sfMode);
        this->SendDatagram(datagram);
    }
}



static std::vector<SleepInterval> g_sleepIntervalStack;

void MainWindow::on_SatelliteConfig_SleepInterval_PushToStack_Button_clicked()
{
    int16_t firstVoltageLevel = ui->SatelliteConfig_SleepInterval_VoltageLevelFirst_SpinBox->value();
    uint16_t firstSleepInterval = ui->SatelliteConfig_SleepInterval_FirstSleepIntervalLength_SpinBox->value();
    uint16_t secondSleepInvtervals = ui->SatelliteConfig_SleepInterval_SecondSleepIntervalVoltages_SpinBox->value();
    uint16_t thirdSleepInterval = ui->SatelliteConfig_SleepInterval_ThirdSleepInterval_SpinBox->value();


    SleepInterval sleepInterval;
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
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_Sleep_Intervals(g_sleepIntervalStack);
        this->SendDatagram(datagram);
        g_sleepIntervalStack.clear();
        ui->SatelliteConfig_SleepInterval_Stack_StackCount_SpinBox->setValue(0);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_Sleep_Intervals(g_sleepIntervalStack);
        this->SendDatagram(datagram);
        g_sleepIntervalStack.clear();
        ui->SatelliteConfig_SleepInterval_Stack_StackCount_SpinBox->setValue(0);
    }
}


void MainWindow::on_SatelliteConfig_MPPT_Set_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
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
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Temperature switch radio buttons not set");
            msgBox.exec();
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
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Keep alive radio buttons not set");
            msgBox.exec();
        }


        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_MPPT_Mode(tempSwitchState, keepAliveState);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
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
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Temperature switch radio buttons not set");
            msgBox.exec();
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
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Keep alive radio buttons not set");
            msgBox.exec();
        }


        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_MPPT_Mode(tempSwitchState, keepAliveState);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteConfig_RTC_Set_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        uint8_t yearOffset = ui->SatelliteConfig_RTC_Year_SpinBox->value();
        uint8_t month = ui->SatelliteConfig_RTC_Month_SpinBox->value();
        uint8_t day = ui->SatelliteConfig_RTC_Day_SpinBox->value();
        uint8_t dow = ui->SatelliteConfig_RTC_DoW_SpinBox->value();
        uint8_t hours = ui->SatelliteConfig_RTC_Hours_SpinBox->value();
        uint8_t mins = ui->SatelliteConfig_RTC_Minutes_SpinBox->value();
        uint8_t secs = ui->SatelliteConfig_RTC_Seconds_SpinBox->value();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_RTC(yearOffset, month, day, dow, hours, mins, secs);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        uint8_t yearOffset = ui->SatelliteConfig_RTC_Year_SpinBox->value();
        uint8_t month = ui->SatelliteConfig_RTC_Month_SpinBox->value();
        uint8_t day = ui->SatelliteConfig_RTC_Day_SpinBox->value();
        uint8_t dow = ui->SatelliteConfig_RTC_DoW_SpinBox->value();
        uint8_t hours = ui->SatelliteConfig_RTC_Hours_SpinBox->value();
        uint8_t mins = ui->SatelliteConfig_RTC_Minutes_SpinBox->value();
        uint8_t secs = ui->SatelliteConfig_RTC_Seconds_SpinBox->value();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_RTC(yearOffset, month, day, dow, hours, mins, secs);
        this->SendDatagram(datagram);
    }
}


void MainWindow::on_SatelliteConfig_LowPowerMode_Set_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        bool lowPowerEnabled = ui->SatelliteConfig_LowPowerMode_Enabled_RadioButton->isChecked();
        bool lowPowerModeDisabled =  ui->SatelliteConfig_LowPowerMode_Disabled_RadioButton->isChecked();

        uint8_t enabledFlag = 0x01;
        if (lowPowerEnabled)
        {
            enabledFlag = 0x01;
        }
        else if (lowPowerModeDisabled)
        {
            enabledFlag= 0x00;
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Low Power Mode radio buttons not set");
            msgBox.exec();
        }

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_Low_Power_Mode_Enable(enabledFlag);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        bool lowPowerEnabled = ui->SatelliteConfig_LowPowerMode_Enabled_RadioButton->isChecked();
        bool lowPowerModeDisabled =  ui->SatelliteConfig_LowPowerMode_Disabled_RadioButton->isChecked();

        uint8_t enabledFlag = 0x01;
        if (lowPowerEnabled)
        {
            enabledFlag = 0x01;
        }
        else if (lowPowerModeDisabled)
        {
            enabledFlag= 0x00;
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Low Power Mode radio buttons not set");
            msgBox.exec();
        }

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_Low_Power_Mode_Enable(enabledFlag);
        this->SendDatagram(datagram);
    }

}

void MainWindow::on_SatelliteConfig_IMU_Set_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        bool ok = false;
        float xAxisGyroscopeOffset = ui->SatelliteConfig_IMU_XAxisGyroOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("X Axis gyroscope offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float yAxisGyroscopeOffset = ui->SatelliteConfig_IMU_YAxisGyroOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Y Axis gyroscope offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float zAxisGyroscopeOffset = ui->SatelliteConfig_IMU_ZAxisGyroOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Z Axis gyroscope offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float xAxisAccele = ui->SatelliteConfig_IMU_XAxisAcceleOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("X Axis accele offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float yAxisAccele = ui->SatelliteConfig_IMU_YAxisAcceleOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Y Axis accele offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float zAxisAccele = ui->SatelliteConfig_IMU_ZAxisAcceleOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Z Axis accele offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float xAxisMagnet = ui->SatelliteConfig_IMU_XAxisMagnetometerOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("X Axis magnetometer offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float yAxisMagnet = ui->SatelliteConfig_IMU_YAxisMagnetometerOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Y Axis magnetometer offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float zAxisMagnet = ui->SatelliteConfig_IMU_ZAxisMagnetometerOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Z Axis magnetometer offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_IMU_Offset(xAxisGyroscopeOffset, yAxisGyroscopeOffset, zAxisGyroscopeOffset,
                                                                        xAxisAccele, yAxisAccele, zAxisAccele,
                                                                        xAxisMagnet, yAxisMagnet, zAxisMagnet);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        bool ok = false;
        float xAxisGyroscopeOffset = ui->SatelliteConfig_IMU_XAxisGyroOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("X Axis gyroscope offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float yAxisGyroscopeOffset = ui->SatelliteConfig_IMU_YAxisGyroOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Y Axis gyroscope offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float zAxisGyroscopeOffset = ui->SatelliteConfig_IMU_ZAxisGyroOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Z Axis gyroscope offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float xAxisAccele = ui->SatelliteConfig_IMU_XAxisAcceleOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("X Axis accele offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float yAxisAccele = ui->SatelliteConfig_IMU_YAxisAcceleOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Y Axis accele offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float zAxisAccele = ui->SatelliteConfig_IMU_ZAxisAcceleOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Z Axis accele offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float xAxisMagnet = ui->SatelliteConfig_IMU_XAxisMagnetometerOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("X Axis magnetometer offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float yAxisMagnet = ui->SatelliteConfig_IMU_YAxisMagnetometerOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Y Axis magnetometer offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        float zAxisMagnet = ui->SatelliteConfig_IMU_ZAxisMagnetometerOffset_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Z Axis magnetometer offset is an invalid number, must be a float.");
            msgBox.exec();
        }

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_IMU_Offset(xAxisGyroscopeOffset, yAxisGyroscopeOffset, zAxisGyroscopeOffset,
                                                                        xAxisAccele, yAxisAccele, zAxisAccele,
                                                                        xAxisMagnet, yAxisMagnet, zAxisMagnet);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteConfig_IMU_Callibration_Set_PushButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {

        QString maQStr = ui->SatelliteConfig_IMU_Callibration_MA_LineEdit->text();
        QString mbQStr = ui->SatelliteConfig_IMU_Callibration_MB_LineEdit->text();
        QString mcQStr = ui->SatelliteConfig_IMU_Callibration_MC_LineEdit->text();

        QStringList maQStrParts = maQStr.split(",");
        QStringList mbQStrParts = mbQStr.split(",");
        QStringList mcQStrParts = mcQStr.split(",");

        bool ok = false;
        float maa = maQStrParts[0].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 1, column 1 is an invalid float.");
            msgBox.exec();
        }

        float mab = maQStrParts[1].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 1, column 2  is an invalid float.");
            msgBox.exec();
        }

        float mac = maQStrParts[2].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 1, column 3 is an invalid float.");
            msgBox.exec();
        }



        float mba = mbQStrParts[0].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 2, column 1 is an invalid float.");
            msgBox.exec();
        }
        float mbb = mbQStrParts[1].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 2, column 2 is an invalid float.");
            msgBox.exec();
        }
        float mbc = mbQStrParts[2].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 2, column 3 is an invalid float.");
            msgBox.exec();
        }


        float mca = mcQStrParts[0].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 3, column 1 is an invalid float.");
            msgBox.exec();
        }
        float mcb = mcQStrParts[1].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 3, column 2 is an invalid float.");
            msgBox.exec();
        }
        float mcc = mcQStrParts[2].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 3, column 3 is an invalid float.");
            msgBox.exec();
        }



        QString biasQStr = ui->SatelliteConfig_IMU_Callibration_BiasVector_LineEdit->text();
        QStringList biasStrParts = biasQStr.split(",");

        float bvx = biasStrParts[0].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Bias vector x is an invalid float.");
            msgBox.exec();
        }
        float bvy = biasStrParts[1].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Bias vector y is an invalid float.");
            msgBox.exec();
        }
        float bvz = biasStrParts[2].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Bias vector z is an invalid float.");
            msgBox.exec();
        }


        float matrix[9] = {
            maa, mab, mac,
            mba, mbb, mbc,
            mca, mcb, mcc
        };

        float bvec[3] = {
            bvx, bvy, bvz
        };

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_IMU_Calibration(matrix, bvec);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {

        QString maQStr = ui->SatelliteConfig_IMU_Callibration_MA_LineEdit->text();
        QString mbQStr = ui->SatelliteConfig_IMU_Callibration_MB_LineEdit->text();
        QString mcQStr = ui->SatelliteConfig_IMU_Callibration_MC_LineEdit->text();

        QStringList maQStrParts = maQStr.split(",");
        QStringList mbQStrParts = mbQStr.split(",");
        QStringList mcQStrParts = mcQStr.split(",");

        bool ok = false;
        float maa = maQStrParts[0].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 1, column 1 is an invalid float.");
            msgBox.exec();
        }

        float mab = maQStrParts[1].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 1, column 2  is an invalid float.");
            msgBox.exec();
        }

        float mac = maQStrParts[2].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 1, column 3 is an invalid float.");
            msgBox.exec();
        }



        float mba = mbQStrParts[0].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 2, column 1 is an invalid float.");
            msgBox.exec();
        }
        float mbb = mbQStrParts[1].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 2, column 2 is an invalid float.");
            msgBox.exec();
        }
        float mbc = mbQStrParts[2].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 2, column 3 is an invalid float.");
            msgBox.exec();
        }


        float mca = mcQStrParts[0].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 3, column 1 is an invalid float.");
            msgBox.exec();
        }
        float mcb = mcQStrParts[1].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 3, column 2 is an invalid float.");
            msgBox.exec();
        }
        float mcc = mcQStrParts[2].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Matrix row 3, column 3 is an invalid float.");
            msgBox.exec();
        }



        QString biasQStr = ui->SatelliteConfig_IMU_Callibration_BiasVector_LineEdit->text();
        QStringList biasStrParts = biasQStr.split(",");

        float bvx = biasStrParts[0].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Bias vector x is an invalid float.");
            msgBox.exec();
        }
        float bvy = biasStrParts[1].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Bias vector y is an invalid float.");
            msgBox.exec();
        }
        float bvz = biasStrParts[2].toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Bias vector z is an invalid float.");
            msgBox.exec();
        }


        float matrix[9] = {
            maa, mab, mac,
            mba, mbb, mbc,
            mca, mcb, mcc
        };

        float bvec[3] = {
            bvx, bvy, bvz
        };

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_IMU_Calibration(matrix, bvec);
        this->SendDatagram(datagram);
    }
}

void MainWindow::on_SatelliteConfig_ADCs_Parameters_Set_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {

        bool ok = false;
        float maximumPulseIntensity = ui->SatelliteConfig_ADCs_Parameters_MaxPulseIntensity_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Maximum pulse intensity is an invalid float.");
            msgBox.exec();
        }

        float maximumPulseLength = ui->SatelliteConfig_ADCs_Parameters_MaxPulseLength_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Maximum pulse length is an invalid float.");
            msgBox.exec();
        }


        float detumblingAngularVelocityChangeTolerance = ui->SatelliteConfig_ADCs_Parameters_DetumblingAngVelChangeTol_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Detumbling angular velocity change tolerance is an invalid float.");
            msgBox.exec();
        }

        float minimumIntertialMoment = ui->SatelliteConfig_ADCs_Parameters_MinInertialMoment_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Min intertial moment is an invalid float.");
            msgBox.exec();
        }

        float pulseAmp = ui->SatelliteConfig_ADCs_Parameters_PulseAmplitude_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Pulse amplitude is an invalid float.");
            msgBox.exec();
        }

        float calcTolerance = ui->SatelliteConfig_ADCs_Parameters_CalculationTolerance_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Caclulation tolerance is an invalid float.");
            msgBox.exec();
        }

        float activeControlEulerAngleChangeTolerance = ui->SatelliteConfig_ADCs_Parameters_ActiveControlEulerAngleChangeTol_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Active control Euler angle change tolerance is an invalid float.");
            msgBox.exec();
        }

        float activeControlAngularVelocityChangeTolerance = ui->SatelliteConfig_ADCs_Parameters_ActiveControlAngularVelChangeTol_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Active control angluar velocity change tolerance is an invalid float.");
            msgBox.exec();
        }

        float eclipseThreshold = ui->SatelliteConfig_ADCs_Parameters_EclipseThreshold_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Eclipse threshold is an invalid float.");
            msgBox.exec();
        }

        float rotationMatrixWeightRatio = ui->SatelliteConfig_ADCs_Parameters_RotationMatrixWeightRatio_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Rotation matrix weigh ratio is an invalid float.");
            msgBox.exec();
        }

        float rotationVerificationTriggerLevel = ui->SatelliteConfig_ADCs_Parameters_RotationVerificationTriggerLevel_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Rotation verification trigger level is an invalid float.");
            msgBox.exec();
        }

        float KalmanFilterDisturbanceCovariance = ui->SatelliteConfig_ADCs_Parameters_KalmanFilterDistrubanceCovariance_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Kalman filter disturbance covariance is an invalid float.");
            msgBox.exec();
        }

        float KalmanFilterNoiseCovariance = ui->SatelliteConfig_ADCs_Parameters_KalmanFilterNoiseCovariance_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Kalman filter noise covariance is an invalid float.");
            msgBox.exec();
        }

        uint32_t timeStepADCSUpdatems = ui->SatelliteConfig_ADCs_Parameters_ADCUpdateTimeStep_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("ADCS update time step is an invalid uint32_t.");
            msgBox.exec();
        }

        uint32_t hBridgeTimerUpdatePeriod = ui->SatelliteConfig_ADCs_Parameters_HBridgeUpdateTimePeriod_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("H bridge timer update period is an invalid uint32_t.");
            msgBox.exec();
        }

        int8_t hBridgeHighOutputValue = ui->SatelliteConfig_ADCs_Parameters_HBridgeValueForHighOutput_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("H Bridge high output value is an invalid int8_t (requires -63 to 63)");
            msgBox.exec();
        }

        int8_t hBridgeLowOutputValue = ui->SatelliteConfig_ADCs_Parameters_HBridgeValueForLowOutput_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("H Bridge low output value is an invalid int8_t (requires -63 to 63)");
            msgBox.exec();
        }

        uint8_t numControllers = ui->SatelliteConfig_ADCs_Parameters_ControllerNumber_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("H Bridge low output value is an invalid uint8_t");
            msgBox.exec();
        }

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_ADCS_Parameters(
            maximumPulseIntensity,
            maximumPulseLength,
            detumblingAngularVelocityChangeTolerance,
            minimumIntertialMoment,
            pulseAmp,
            calcTolerance,
            activeControlEulerAngleChangeTolerance,
            activeControlAngularVelocityChangeTolerance,
            eclipseThreshold,
            rotationMatrixWeightRatio,
            rotationVerificationTriggerLevel,
            KalmanFilterDisturbanceCovariance,
            KalmanFilterNoiseCovariance,
            timeStepADCSUpdatems,
            hBridgeTimerUpdatePeriod,
            hBridgeHighOutputValue,
            hBridgeLowOutputValue,
            numControllers);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {

        bool ok = false;
        float maximumPulseIntensity = ui->SatelliteConfig_ADCs_Parameters_MaxPulseIntensity_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Maximum pulse intensity is an invalid float.");
            msgBox.exec();
        }

        float maximumPulseLength = ui->SatelliteConfig_ADCs_Parameters_MaxPulseLength_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Maximum pulse length is an invalid float.");
            msgBox.exec();
        }


        float detumblingAngularVelocityChangeTolerance = ui->SatelliteConfig_ADCs_Parameters_DetumblingAngVelChangeTol_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Detumbling angular velocity change tolerance is an invalid float.");
            msgBox.exec();
        }

        float minimumIntertialMoment = ui->SatelliteConfig_ADCs_Parameters_MinInertialMoment_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Min intertial moment is an invalid float.");
            msgBox.exec();
        }

        float pulseAmp = ui->SatelliteConfig_ADCs_Parameters_PulseAmplitude_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Pulse amplitude is an invalid float.");
            msgBox.exec();
        }

        float calcTolerance = ui->SatelliteConfig_ADCs_Parameters_CalculationTolerance_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Caclulation tolerance is an invalid float.");
            msgBox.exec();
        }

        float activeControlEulerAngleChangeTolerance = ui->SatelliteConfig_ADCs_Parameters_ActiveControlEulerAngleChangeTol_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Active control Euler angle change tolerance is an invalid float.");
            msgBox.exec();
        }

        float activeControlAngularVelocityChangeTolerance = ui->SatelliteConfig_ADCs_Parameters_ActiveControlAngularVelChangeTol_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Active control angluar velocity change tolerance is an invalid float.");
            msgBox.exec();
        }

        float eclipseThreshold = ui->SatelliteConfig_ADCs_Parameters_EclipseThreshold_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Eclipse threshold is an invalid float.");
            msgBox.exec();
        }

        float rotationMatrixWeightRatio = ui->SatelliteConfig_ADCs_Parameters_RotationMatrixWeightRatio_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Rotation matrix weigh ratio is an invalid float.");
            msgBox.exec();
        }

        float rotationVerificationTriggerLevel = ui->SatelliteConfig_ADCs_Parameters_RotationVerificationTriggerLevel_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Rotation verification trigger level is an invalid float.");
            msgBox.exec();
        }

        float KalmanFilterDisturbanceCovariance = ui->SatelliteConfig_ADCs_Parameters_KalmanFilterDistrubanceCovariance_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Kalman filter disturbance covariance is an invalid float.");
            msgBox.exec();
        }

        float KalmanFilterNoiseCovariance = ui->SatelliteConfig_ADCs_Parameters_KalmanFilterNoiseCovariance_LineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Kalman filter noise covariance is an invalid float.");
            msgBox.exec();
        }

        uint32_t timeStepADCSUpdatems = ui->SatelliteConfig_ADCs_Parameters_ADCUpdateTimeStep_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("ADCS update time step is an invalid uint32_t.");
            msgBox.exec();
        }

        uint32_t hBridgeTimerUpdatePeriod = ui->SatelliteConfig_ADCs_Parameters_HBridgeUpdateTimePeriod_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("H bridge timer update period is an invalid uint32_t.");
            msgBox.exec();
        }

        int8_t hBridgeHighOutputValue = ui->SatelliteConfig_ADCs_Parameters_HBridgeValueForHighOutput_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("H Bridge high output value is an invalid int8_t (requires -63 to 63)");
            msgBox.exec();
        }

        int8_t hBridgeLowOutputValue = ui->SatelliteConfig_ADCs_Parameters_HBridgeValueForLowOutput_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("H Bridge low output value is an invalid int8_t (requires -63 to 63)");
            msgBox.exec();
        }

        uint8_t numControllers = ui->SatelliteConfig_ADCs_Parameters_ControllerNumber_SpinBox->value();
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("H Bridge low output value is an invalid uint8_t");
            msgBox.exec();
        }

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_ADCS_Parameters(
            maximumPulseIntensity,
            maximumPulseLength,
            detumblingAngularVelocityChangeTolerance,
            minimumIntertialMoment,
            pulseAmp,
            calcTolerance,
            activeControlEulerAngleChangeTolerance,
            activeControlAngularVelocityChangeTolerance,
            eclipseThreshold,
            rotationMatrixWeightRatio,
            rotationVerificationTriggerLevel,
            KalmanFilterDisturbanceCovariance,
            KalmanFilterNoiseCovariance,
            timeStepADCSUpdatems,
            hBridgeTimerUpdatePeriod,
            hBridgeHighOutputValue,
            hBridgeLowOutputValue,
            numControllers);
        this->SendDatagram(datagram);
    }

}


void MainWindow::on_SatelliteConfig_ADCs_Controller_Set_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {

        char controllerId =(char)this->ui->SatelliteConfig_ADCs_Controller_ControllerID_LineEdit->text().toStdString()[0];

        QString firstLine = this->ui->SatelliteConfig_ADCs_Controller_M50_LineEdit->text();
        QString secondLine = this->ui->SatelliteConfig_ADCs_Controller_M51_LineEdit->text();
        QString thirdLine = this->ui->SatelliteConfig_ADCs_Controller_M52_LineEdit->text();

        QStringList firstLineElements;
        firstLineElements = firstLine.split(',');

        if (firstLineElements.length() != 6)
        {
            QMessageBox msgBox;
            msgBox.setText("Number of elements != 6");
            msgBox.exec();
        }

        QStringList secondLineElements;
        secondLineElements = secondLine.split(',');

        if (secondLineElements.length() != 6)
        {
            QMessageBox msgBox;
            msgBox.setText("Number of elements != 6");
            msgBox.exec();
        }

        QStringList thirdLineElements;
        thirdLineElements = thirdLine.split(',');

        if (thirdLineElements.length() != 6)
        {
            QMessageBox msgBox;
            msgBox.setText("Number of elements != 6");
            msgBox.exec();
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

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_ADCS_Controller(controllerId, controllerMatrix);
        this->SendDatagram(datagram);
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {


        char controllerId =(char)this->ui->SatelliteConfig_ADCs_Controller_ControllerID_LineEdit->text().toStdString()[0];

        QString firstLine = this->ui->SatelliteConfig_ADCs_Controller_M50_LineEdit->text();
        QString secondLine = this->ui->SatelliteConfig_ADCs_Controller_M51_LineEdit->text();
        QString thirdLine = this->ui->SatelliteConfig_ADCs_Controller_M52_LineEdit->text();

        QStringList firstLineElements;
        firstLineElements = firstLine.split(',');

        if (firstLineElements.length() != 6)
        {
            QMessageBox msgBox;
            msgBox.setText("Number of elements != 6");
            msgBox.exec();
        }

        QStringList secondLineElements;
        secondLineElements = secondLine.split(',');

        if (secondLineElements.length() != 6)
        {
            QMessageBox msgBox;
            msgBox.setText("Number of elements != 6");
            msgBox.exec();
        }

        QStringList thirdLineElements;
        thirdLineElements = thirdLine.split(',');

        if (thirdLineElements.length() != 6)
        {
            QMessageBox msgBox;
            msgBox.setText("Number of elements != 6");
            msgBox.exec();
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

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_ADCS_Controller(controllerId, controllerMatrix);
        this->SendDatagram(datagram);
    }
}


void MainWindow::on_SatelliteConfig_ADCs_Controller_FromFiles_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        QFileDialog::getOpenFileContent("*.*", [this](const QString& fileName, const QByteArray &fileContent) {
            if (fileName.isEmpty())
            {
                QMessageBox msgBox;
                msgBox.setText("No file selected");
                msgBox.exec();
            }
            else
            {

                /*
             Jan (08/16/2020) - controllers are basically 3x6 float matrix, ephemerides are 7 numbers per line - first 3 are solar, next 3 are mangetic, the last number is controller type
            */

                QString fileContents = QString(fileContent);

                QStringList lines = fileContents.split("[\r\n]", QString::SkipEmptyParts);

                int numLines = lines.length();
                int numControllerEntries = numLines/3;

                // for each controller matrix entry
                for (int controllerNumber = 0; controllerNumber < numControllerEntries; controllerNumber++)
                {
                    // construct this controller matrix.
                    float controllerMatrix[3][6];

                    // for each row in this entry (3 rows)
                    for (int row = 0; row < 3; row++)
                    {
                        // split this row into columns (6 columns)
                        QStringList columnsList = lines[row].split(' ');

                        if (columnsList.length() != 6)
                        {
                            QMessageBox msgBox;
                            msgBox.setText(QString("Invalid number of columns at entry: " + QString(controllerNumber) + " at row: " + QString(row)));
                            msgBox.exec();
                        }

                        // for each column in the row.
                        for (int column = 0; column < 6; column++)
                        {
                            bool ok = false;

                            // get this column-row-cell as a string
                            QString cellEntry = columnsList.at(column);

                            // convert it to a float
                            float cellEntryFloat = cellEntry.toFloat(&ok);

                            if (!ok)
                            {
                                QMessageBox msgBox;
                                msgBox.setText(QString("Column entry is an invalid float! ") + row + QString(", ") + column + QString(" = ") + columnsList.at(column));
                                msgBox.exec();
                            }

                            // set it in our matrix
                            controllerMatrix[row][column] = cellEntryFloat;
                        }
                    }

                    // finished building this controllerMatrix, create datagram and send it.
                    Datagram datagram = FOSSASAT2::DatagramEncoder::Set_ADCS_Controller(controllerNumber, controllerMatrix);
                    this->SendDatagram(datagram);
                }
            }
        });
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        QFileDialog::getOpenFileContent("*.*", [this](const QString& fileName, const QByteArray &fileContent) {
            if (fileName.isEmpty())
            {
                QMessageBox msgBox;
                msgBox.setText("No file selected");
                msgBox.exec();
            }
            else
            {

                /*
             Jan (08/16/2020) - controllers are basically 3x6 float matrix, ephemerides are 7 numbers per line - first 3 are solar, next 3 are mangetic, the last number is controller type
            */

                QString fileContents = QString(fileContent);

                QStringList lines = fileContents.split("[\r\n]", QString::SkipEmptyParts);

                int numLines = lines.length();
                int numControllerEntries = numLines/3;

                // for each controller matrix entry
                for (int controllerNumber = 0; controllerNumber < numControllerEntries; controllerNumber++)
                {
                    // construct this controller matrix.
                    float controllerMatrix[3][6];

                    // for each row in this entry (3 rows)
                    for (int row = 0; row < 3; row++)
                    {
                        // split this row into columns (6 columns)
                        QStringList columnsList = lines[row].split(' ');

                        if (columnsList.length() != 6)
                        {
                            QMessageBox msgBox;
                            msgBox.setText(QString("Invalid number of columns at entry: " + QString(controllerNumber) + " at row: " + QString(row)));
                            msgBox.exec();
                        }

                        // for each column in the row.
                        for (int column = 0; column < 6; column++)
                        {
                            bool ok = false;

                            // get this column-row-cell as a string
                            QString cellEntry = columnsList.at(column);

                            // convert it to a float
                            float cellEntryFloat = cellEntry.toFloat(&ok);

                            if (!ok)
                            {
                                QMessageBox msgBox;
                                msgBox.setText(QString("Column entry is an invalid float! ") + row + QString(", ") + column + QString(" = ") + columnsList.at(column));
                                msgBox.exec();
                            }

                            // set it in our matrix
                            controllerMatrix[row][column] = cellEntryFloat;
                        }
                    }

                    // finished building this controllerMatrix, create datagram and send it.
                    Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_ADCS_Controller(controllerNumber, controllerMatrix);
                    this->SendDatagram(datagram);
                }
            }
        });
    }

}

static std::vector<Ephemerides> g_ephemeridesControllerStack;

void MainWindow::on_SatelliteConfig_ADCs_Ephemerides_SetFromFiles_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        QFileDialog::getOpenFileContent("*.*", [this](const QString& fileName, const QByteArray &fileContent) {
            QString fileContents = QString(fileContent);
            QStringList lines = fileContents.split("[\r\n]", QString::SkipEmptyParts);

            // for each line in the file.
            for (QString line : lines)
            {
                // split each line into columns
                QStringList columnsList = line.split(' ');

                if (columnsList.length() != 7) {
                    QMessageBox msgBox;
                    msgBox.setText("Invalid ephemerides column count, must be 7 columns.");
                    msgBox.exec();
                }

                bool ok = false;
                float solarX = columnsList.at(0).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Solar X is an invalid float");
                    msgBox.exec();
                }

                float solarY = columnsList.at(1).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Solar Y is an invalid float");
                    msgBox.exec();
                }

                float solarZ = columnsList.at(2).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Solar Z is an invalid float");
                    msgBox.exec();
                }

                float magnetX = columnsList.at(3).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Magnet X is an invalid float");
                    msgBox.exec();
                }

                float magnetY = columnsList.at(4).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Magnet Y is an invalid float");
                    msgBox.exec();
                }

                float magnetZ = columnsList.at(5).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Magnet Z is an invalid float");
                    msgBox.exec();
                }

                uint8_t controllerId = columnsList.at(6).toUInt(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Magnet Z is an invalid uint8_t");
                    msgBox.exec();
                }

                float ephemeridesMat[6];
                ephemeridesMat[0] = solarX;
                ephemeridesMat[1] = solarY;
                ephemeridesMat[2] = solarZ;
                ephemeridesMat[3] = magnetX;
                ephemeridesMat[4] = magnetY;
                ephemeridesMat[5] = magnetZ;


                Ephemerides eph;
                eph.solar_x = solarX;
                eph.solar_y = solarY;
                eph.solar_z = solarZ;
                eph.magnetic_x = magnetX;
                eph.magnetic_y = magnetY;
                eph.magnetic_z = magnetZ;
                eph.controllerId = controllerId;

                g_ephemeridesControllerStack.push_back(eph);
                ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
            }
        });
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        QFileDialog::getOpenFileContent("*.*", [this](const QString& fileName, const QByteArray &fileContent) {
            QString fileContents = QString(fileContent);
            QStringList lines = fileContents.split("[\r\n]", QString::SkipEmptyParts);

            // for each line in the file.
            for (QString line : lines)
            {
                // split each line into columns
                QStringList columnsList = line.split(' ');

                if (columnsList.length() != 7) {
                    QMessageBox msgBox;
                    msgBox.setText("Invalid ephemerides column count, must be 7 columns.");
                    msgBox.exec();
                }

                bool ok = false;
                float solarX = columnsList.at(0).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Solar X is an invalid float");
                    msgBox.exec();
                }

                float solarY = columnsList.at(1).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Solar Y is an invalid float");
                    msgBox.exec();
                }

                float solarZ = columnsList.at(2).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Solar Z is an invalid float");
                    msgBox.exec();
                }

                float magnetX = columnsList.at(3).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Magnet X is an invalid float");
                    msgBox.exec();
                }

                float magnetY = columnsList.at(4).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Magnet Y is an invalid float");
                    msgBox.exec();
                }

                float magnetZ = columnsList.at(5).toFloat(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Magnet Z is an invalid float");
                    msgBox.exec();
                }

                uint8_t controllerId = columnsList.at(6).toUInt(&ok);
                if (!ok)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Magnet Z is an invalid uint8_t");
                    msgBox.exec();
                }

                float ephemeridesMat[6];
                ephemeridesMat[0] = solarX;
                ephemeridesMat[1] = solarY;
                ephemeridesMat[2] = solarZ;
                ephemeridesMat[3] = magnetX;
                ephemeridesMat[4] = magnetY;
                ephemeridesMat[5] = magnetZ;


                Ephemerides eph;
                eph.solar_x = solarX;
                eph.solar_y = solarY;
                eph.solar_z = solarZ;
                eph.magnetic_x = magnetX;
                eph.magnetic_y = magnetY;
                eph.magnetic_z = magnetZ;
                eph.controllerId = controllerId;

                g_ephemeridesControllerStack.push_back(eph);
                ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
            }
        });
    }
}




void MainWindow::on_Satelliteconfig_ADCs_Ephemerides_DataStack_Push_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        QString solar = ui->SatelliteConfig_ADCs_Ephemerides_SolarEphemeris_LineEdit->text();
        QStringList solarElements = solar.split(',');

        QString magnetic = ui->SatelliteConfig_ADCs_Ephemerides_MagneticEphemeris_LineEdit->text();
        QStringList magneticElements = magnetic.split(',');

        uint8_t controllerId = ui->SatelliteConfig_ADCs_Ephemerides_ControllerID_SpinBox->text().toUInt();

        Ephemerides ephemeridesStruct;
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
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        QString solar = ui->SatelliteConfig_ADCs_Ephemerides_SolarEphemeris_LineEdit->text();
        QStringList solarElements = solar.split(',');

        QString magnetic = ui->SatelliteConfig_ADCs_Ephemerides_MagneticEphemeris_LineEdit->text();
        QStringList magneticElements = magnetic.split(',');

        uint8_t controllerId = ui->SatelliteConfig_ADCs_Ephemerides_ControllerID_SpinBox->text().toUInt();

        Ephemerides ephemeridesStruct;
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
}


void MainWindow::on_Satelliteconfig_ADCs_Ephemerides_DataStack_Clear_Button_clicked()
{
    g_ephemeridesControllerStack.clear();
    ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
}

void MainWindow::on_aSatelliteconfig_ADCs_Ephemerides_DataStack_Send_Button_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        uint16_t chunkId = ui->SatelliteConfig_ADCs_Ephemerides_ChunkID_SpinBox->value();

        Datagram datagram = FOSSASAT1B::DatagramEncoder::Set_ADCS_Ephemerides(chunkId, g_ephemeridesControllerStack);
        this->SendDatagram(datagram);

        g_ephemeridesControllerStack.clear();

        ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {
        uint16_t chunkId = ui->SatelliteConfig_ADCs_Ephemerides_ChunkID_SpinBox->value();

        Datagram datagram = FOSSASAT2::DatagramEncoder::Set_ADCS_Ephemerides(chunkId, g_ephemeridesControllerStack);
        this->SendDatagram(datagram);

        g_ephemeridesControllerStack.clear();

        ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
    }
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


void MainWindow::on_GroundStationSettings_GFSKConfigSetButton_clicked()
{
    if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT1B)
    {
        QString modemTypeStr = ui->GroundStationSettings_ModemTypeComboBox->itemData(ui->GroundStationSettings_ModemTypeComboBox->currentIndex()).toString();
        uint8_t modemTypeFlag = 0x0;
        if (modemTypeStr == "LoRa")
        {
            modemTypeFlag = 0x0;
        }
        else if (modemTypeStr == "GFSK")
        {
            modemTypeFlag = 0x1;
        }

        bool ok = false;

        float carrierFrequency = ui->GroundStationSettings_ConfigCarrierFrequencyLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Carrier frequency is an invalid float");
            msgBox.exec();
        }

        uint8_t outputPowerDBM = ui->GroundStationSettings_ConfigOutputPowerLineEdit->text().toUInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Output power is an invalid uint8_t");
            msgBox.exec();
        }

        float currentLimit = ui->GroundStationSettings_ConfigCurrentLimitLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Current limit is an invalid float");
            msgBox.exec();
        }

        float loraBandwidth = ui->GroundStationSettings_loraConfigurationBandwidthLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("LoRa bandwidth is an invalid float");
            msgBox.exec();
        }

        uint8_t loraCodingRate = ui->GroundStationSettings_loraConfigurationSpreadingFactorSpinBox->text().toUInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("LoRa coding factor is an invalid uint8_t");
            msgBox.exec();
        }

        int16_t loraPreambleLength = ui->GroundStationSettings_loraConfigurationPreambleLengthSpinBox->text().toInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("LoRa bandiwdth is an invalid int16_t");
            msgBox.exec();
        }

        float gfskBitRate = ui->GroundStationSettings_GFSKConfigBitRateLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK Bit rate is an invalid float");
            msgBox.exec();
        }

        float gfskFrequencyDeviation = ui->GroundStationSettings_GFSKConfigFreqDeviationLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK frequency devidation is an invalid float");
            msgBox.exec();
        }

        float gfskRxBandwidth = ui->GroundStationSettings_GFSKConfigRxBandwidthLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK RX bandwidth is an invalid float");
            msgBox.exec();
        }

        int16_t gfskPreambleLength = ui->GroundStationSettings_GFSKConfigPreambleLengthLineEdit->text().toInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK preamble length is an invalid int16_t");
            msgBox.exec();
        }

        uint8_t gfskDataShapingBTProduct = ui->GroundStationSettings_GFSKConfigDataShapingBTProductLineEdit->text().toUInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK data shaping BT product is an invalid uint8_t");
            msgBox.exec();
        }

        ui->Satelliteconfig_ADCs_Ephemerides_DataStack_StackCoun_SpinBox->setValue(g_ephemeridesControllerStack.size());
    }
    else if (Settings::GetSatVersion() == SatVersion::V_FOSSASAT2)
    {

        QString modemTypeStr = ui->GroundStationSettings_ModemTypeComboBox->itemData(ui->GroundStationSettings_ModemTypeComboBox->currentIndex()).toString();
        uint8_t modemTypeFlag = 0x0;
        if (modemTypeStr == "LoRa")
        {
            modemTypeFlag = 0x0;
        }
        else if (modemTypeStr == "GFSK")
        {
            modemTypeFlag = 0x1;
        }

        bool ok = false;

        float carrierFrequency = ui->GroundStationSettings_ConfigCarrierFrequencyLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Carrier frequency is an invalid float");
            msgBox.exec();
        }

        uint8_t outputPowerDBM = ui->GroundStationSettings_ConfigOutputPowerLineEdit->text().toUInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Output power is an invalid uint8_t");
            msgBox.exec();
        }

        float currentLimit = ui->GroundStationSettings_ConfigCurrentLimitLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Current limit is an invalid float");
            msgBox.exec();
        }

        float loraBandwidth = ui->GroundStationSettings_loraConfigurationBandwidthLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("LoRa bandwidth is an invalid float");
            msgBox.exec();
        }

        uint8_t loraCodingRate = ui->GroundStationSettings_loraConfigurationSpreadingFactorSpinBox->text().toUInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("LoRa coding factor is an invalid uint8_t");
            msgBox.exec();
        }

        int16_t loraPreambleLength = ui->GroundStationSettings_loraConfigurationPreambleLengthSpinBox->text().toInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("LoRa bandiwdth is an invalid int16_t");
            msgBox.exec();
        }

        float gfskBitRate = ui->GroundStationSettings_GFSKConfigBitRateLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK Bit rate is an invalid float");
            msgBox.exec();
        }

        float gfskFrequencyDeviation = ui->GroundStationSettings_GFSKConfigFreqDeviationLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK frequency devidation is an invalid float");
            msgBox.exec();
        }

        float gfskRxBandwidth = ui->GroundStationSettings_GFSKConfigRxBandwidthLineEdit->text().toFloat(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK RX bandwidth is an invalid float");
            msgBox.exec();
        }

        int16_t gfskPreambleLength = ui->GroundStationSettings_GFSKConfigPreambleLengthLineEdit->text().toInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK preamble length is an invalid int16_t");
            msgBox.exec();
        }

        uint8_t gfskDataShapingBTProduct = ui->GroundStationSettings_GFSKConfigDataShapingBTProductLineEdit->text().toUInt(&ok);
        if (!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("GFSK data shaping BT product is an invalid uint8_t");
            msgBox.exec();
        }
    }



}

///////////////////////////////////////
/// CGround station settings tab END //
///////////////////////////////////////
#define GroundPanelSettingsTab_End }


