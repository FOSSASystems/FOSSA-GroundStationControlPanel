#ifndef SETTINGS_H
#define SETTINGS_H

#include <SatVersion.h>

#include <QString>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>

#include <string>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

////////////////
/// Settings ///
////////////////
class Settings
{
public:
    static bool LoadPasswordFromSettings();
    static void SavePasswordToSettings();

    static void SetKeyString(std::string keyString);
    static std::string GetKeyString();
    static void LoadKeyStringFromFile();
    static void SaveKeyStringToFile();
    static uint8_t* KeyStringAsByteArray();
    static std::vector<uint8_t> KeyStringAsByteVector();


    static void SetPassword(std::string password);
    static void SetPasswordSet();
    static std::string GetPassword();
    static bool IsPasswordSet();

    static double GetLatitude();
    static void SetLatitude(double latitude);

    static double GetLongitude();
    static void SetLongitude(double longitude);

    static double GetAltitude();
    static void SetAltitude(double altitude);

    static void SaveLatLongAlt();
    static bool LoadLatLongAlt();


    static bool GetDopplerShiftCorrectionEnabled();
    static void SetDopplerShiftCorrectionEnabled(bool dopplerShiftEnabled);
    static void SaveDopplerShiftCorrectionEnabled();
    static bool LoadDopplerShiftCorrectionEnabled();


    static void SetTLELine1(std::string lineText);
    static std::string GetTLELine1();
    static void SetTLELine2(std::string lineText);
    static std::string GetTLELine2();
    static void SaveTLELines();
    static void LoadTLElines();

    static bool GetHandshookValue();
    static void SetHandshookValue(bool handshook);

    static void SetSatVersion(SatVersion satVersion);
    static SatVersion GetSatVersion();
    static void SaveSatVersion();
    static void LoadSatVersion();

    static std::string getCallsign();
    static void setCallsign(const std::string &value);
    static bool LoadCallsign();
    static void SaveCallsign();

protected:
private:
    static std::string keyString;
    static uint8_t m_keyBytes[16];

    static std::string m_password;
    static bool m_passwordSet;

    static bool m_handshook;

    static double m_latitude;
    static double m_longitude;
    static double m_altitude;
    static bool m_dopplerShiftCorrectionEnabled;

    static std::string m_tleLine1;
    static std::string m_tleLine2;

    static SatVersion m_satVersion;

    static std::string callsign;
};

#endif // SETTINGS_H
