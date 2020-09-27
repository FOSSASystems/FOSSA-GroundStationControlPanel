#include "Settings.h"

// AES KEY
uint8_t Settings::m_key[16];
bool Settings::m_keySet = false;

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

QString Settings::m_satVersion = "FOSSASAT-2";

bool Settings::LoadKeyFromSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    QString keyText = settings.value("key", "").toString(); // this is a string of hex ASCII characters e.g. FFAAFFAA
    std::string keyTextStr = keyText.toStdString();

    if (keyText.length() != 32) // key must have 32 characters for 16 bytes.
    {
        return false;
    }

    uint8_t tempKey[16];
    for (uint8_t i = 0; i < 32; i+=2)
    {
        // convert 2 characters in the key tex to a string
        // e.g. FA
        char byteAsHexDigits[2];
        byteAsHexDigits[1] = keyTextStr[i]; // str[1] = F
        byteAsHexDigits[0] = keyTextStr[i+1]; // str[0] = A --> str = 'FA'

        // convert 2 hex digits to byte.
        tempKey[i/2] = (uint8_t)strtol(byteAsHexDigits, NULL, 16);
    }

    Settings::SetKey(tempKey);

    return true;
}

void Settings::SaveKeyToSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString filename = "settings.ini" ;
    QSettings settings(path + "/"+ filename, QSettings::IniFormat);

    uint8_t* key = Settings::GetKey(); // 16 byte key needs to turn into a 32 hex string.

    QString str;
    for (uint8_t i = 0; i < 16; i++)
    {
        uint8_t keyValue = key[i];

        // convert byte into 2 character hex (1 hex = 4 bits)
        char hexStr[2];
        sprintf(&(hexStr[0]), "%02x", (uint8_t)keyValue);
        str.append(hexStr);
    }
    settings.setValue("key", str);
    settings.sync();
}

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

void Settings::SetKey(uint8_t *key)
{
    for (int i = 0; i < 16; i++)
    {
        uint8_t v = key[i];
        m_key[i] = v;
    }
}

void Settings::SetKeySet()
{
    m_keySet = true;
}

uint8_t *Settings::GetKey() { return m_key; }

bool Settings::IsKeySet() { return m_keySet; }

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

void Settings::SetSatVersion(QString satVersion)
{
    m_satVersion = satVersion;
}

QString Settings::GetSatVersion()
{
    return m_satVersion;
}
