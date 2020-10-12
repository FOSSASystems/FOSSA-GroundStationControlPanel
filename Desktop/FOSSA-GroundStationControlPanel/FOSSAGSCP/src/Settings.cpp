#include "Settings.h"

// AES KEY
std::string Settings::keyString;
uint8_t Settings::m_keyBytes[16];

// Password.
std::string Settings::m_password = "";
bool Settings::m_passwordSet = false;

bool Settings::m_handshook = false;

double Settings:: m_latitude;
double Settings::m_longitude;
double Settings::m_altitude;
bool Settings::m_dopplerShiftCorrectionEnabled;

std::string Settings::m_tleLine1;
std::string Settings::m_tleLine2;

SatVersion Settings::m_satVersion = SatVersion::V_FOSSASAT2;
std::string  Settings::callsign;


bool Settings::LoadPasswordFromSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    QString passwordText = settings.value("password", "").toString();
    std::string password = passwordText.toStdString();

    if (password.length() <= 0)
    {
        return false;
    }
    else
    {
        Settings::SetPassword(password);

        return true;
    }
}

void Settings::SavePasswordToSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    std::string password = Settings::GetPassword();
    QString str = QString::fromStdString(password);
    settings.setValue("password", str);
}

void Settings::SetKeyString(std::string keyString)
{
    Settings::keyString = keyString;
}

std::string Settings::GetKeyString()
{
    return Settings::keyString;
}

void Settings::LoadKeyStringFromFile()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    QString keyText = settings.value("key", "").toString(); // this is a string of hex ASCII characters e.g. FFAAFFAA
    Settings::SetKeyString(keyText.toStdString());
}

void Settings::SaveKeyStringToFile()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    std::string key = Settings::GetKeyString(); // 16 byte key needs to turn into a 32 hex string.
    settings.setValue("key", QString::fromStdString(key));
    settings.sync();
}

std::vector<uint8_t> Settings::KeyStringAsByteVector()
{
    uint8_t* key = Settings::KeyStringAsByteArray();
    std::vector<uint8_t> keyAsBytes;
    for (int i = 0; i < 16; i++) {
        keyAsBytes.push_back(i);
    }
    return keyAsBytes;
}

uint8_t *Settings::KeyStringAsByteArray()
{
    std::string keyString = Settings::GetKeyString();

    static uint8_t tempKey[16] = { 0 };

    bool keySet = keyString.length() != 0;
    if (keySet)
    {
        bool keyHasValidLength = keyString.length() == (16*2) + 15;
        if (keyHasValidLength)
        {
            QString keyQString = QString::fromStdString(keyString);
            QStringList keyParts = keyQString.split(",");

            int index = 0;
            for (QString hexNumber : keyParts) {

                // convert 2 characters in the key tex to a string
                // e.g. FA
                char byteAsHexDigits[2];
                byteAsHexDigits[1] = hexNumber[0].toLatin1(); // str[1] = F
                byteAsHexDigits[0] = hexNumber[1].toLatin1(); // str[0] = A --> str = 'FA'

                // convert 2 hex digits to byte.
                tempKey[index] = (uint8_t)strtol(byteAsHexDigits, NULL, 16);

                index++;
            }
        }
        else
        {
            Settings::SetKeyString("00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00");
            Settings::SaveKeyStringToFile();

            QMessageBox msgBox;
            QString message = QString("Key string is invalid, must be 16 (1 byte, 2 hex characters) delimninated with commas.");
            msgBox.setText(message);
            msgBox.exec();

            return tempKey;
        }
    }

    return tempKey;

}

void Settings::SetPassword(std::string password)
{
    m_password = password;
}

void Settings::SetPasswordSet()
{
    m_passwordSet = true;
}

std::string Settings::GetPassword() { return m_password; }

bool Settings::IsPasswordSet() { return m_passwordSet; }

double Settings::GetLatitude() { return m_latitude; }

void Settings::SetLatitude(double latitude) { m_latitude = latitude; }

double Settings::GetLongitude() { return m_longitude; }

void Settings::SetLongitude(double longitude) { m_longitude = longitude; }

double Settings::GetAltitude() { return m_altitude;  }

void Settings::SetAltitude(double altitude) { m_altitude = altitude; }

void Settings::SaveLatLongAlt()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    settings.setValue("latitude", GetLatitude());
    settings.setValue("longitude", GetLongitude());
    settings.setValue("altitude", GetAltitude());
    settings.setValue("llaset", true);
}

bool Settings::LoadLatLongAlt()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    bool llaSet = settings.value("llaset").toBool();

    if (llaSet)
    {
        double latitude = settings.value("latitude", 0.0).toDouble();
        SetLatitude(latitude);

        double longitude = settings.value("longitude", 0.0).toDouble();
        SetLongitude(longitude);

        double altitude = settings.value("altitude", 0.0).toDouble();
        SetAltitude(altitude);
    }

    return llaSet;
}

bool Settings::GetDopplerShiftCorrectionEnabled() { return m_dopplerShiftCorrectionEnabled; }

void Settings::SetDopplerShiftCorrectionEnabled(bool dopplerShiftEnabled)
{
    m_dopplerShiftCorrectionEnabled = dopplerShiftEnabled;
    SaveDopplerShiftCorrectionEnabled();
}

void Settings::SaveDopplerShiftCorrectionEnabled()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    settings.setValue("dopplerShiftCorrectionEnabled", GetDopplerShiftCorrectionEnabled());
}

bool Settings::LoadDopplerShiftCorrectionEnabled()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    bool dopplerShiftCorrecEnabled = settings.value("dopplerShiftCorrectionEnabled", false).toBool();
    SetDopplerShiftCorrectionEnabled(dopplerShiftCorrecEnabled);

    return dopplerShiftCorrecEnabled;
}

void Settings::SetTLELine1(std::string lineText)
{
    m_tleLine1 = lineText;
}

std::string Settings::GetTLELine1() { return m_tleLine1; }

void Settings::SetTLELine2(std::string lineText)
{
    m_tleLine2 = lineText;
}

std::string Settings::GetTLELine2() { return m_tleLine2; }

void Settings::SaveTLELines()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    std::string tleStr = GetTLELine1();
    QString tleQtStr = QString::fromStdString(tleStr);
    settings.setValue("tleLine1", tleQtStr);

    tleStr = GetTLELine2();
    tleQtStr = QString::fromStdString(tleStr);
    settings.setValue("tleLine2", tleQtStr);
}

void Settings::LoadTLElines()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    QString tleLine1 = settings.value("tleLine1", "").toString();
    QString tleLine2 = settings.value("tleLine2", "").toString();

    std::string tleLine = tleLine1.toStdString();
    const char* tleLineData = tleLine.c_str();
    SetTLELine1(tleLineData);

    tleLine = tleLine2.toStdString();
    tleLineData = tleLine.c_str();
    SetTLELine2(tleLineData);
}

bool Settings::GetHandshookValue()
{
    return m_handshook;
}

void Settings::SetHandshookValue(bool handshook)
{
    m_handshook = handshook;
}

std::string Settings::getCallsign()
{
    return callsign;
}

void Settings::setCallsign(const std::string &value)
{
    callsign = value;
}

bool Settings::LoadCallsign()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    QString callsignText = settings.value("callsign", "").toString();
    if (callsignText == "") {
        Settings::setCallsign("FOSSASAT-2");
    } else {
        std::string callsign = callsignText.toStdString();
        Settings::setCallsign(callsign);
    }

    return true;
}

void Settings::SaveCallsign()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    std::string callsign = Settings::getCallsign();
    QString str = QString::fromStdString(callsign);
    settings.setValue("callsign", str);
}

void Settings::SetSatVersion(SatVersion satVersion)
{
    m_satVersion = satVersion;
}

SatVersion Settings::GetSatVersion()
{
    return m_satVersion;
}

void Settings::SaveSatVersion()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    SatVersion version = Settings::GetSatVersion();
    std::string versionStr = SatVersionToString(version);

    QString str = QString::fromStdString(versionStr);
    settings.setValue("satversion", str);
}

void Settings::LoadSatVersion()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    QString versionText = settings.value("satversion", "").toString();
    if (versionText == "") {
        Settings::SetSatVersion(SatVersion::V_FOSSASAT2);
    } else {
        std::string versionStr = versionText.toStdString();
        SatVersion version = SatVersionFromString(versionStr);
        Settings::SetSatVersion(version);
    }
}
