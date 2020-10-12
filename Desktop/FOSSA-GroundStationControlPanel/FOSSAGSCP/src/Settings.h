#ifndef SETTINGS_H
#define SETTINGS_H

#include <SatVersion.h>

#include <QString>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>

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
    static bool LoadKeyFromSettings();
    static void SaveKeyToSettings();

    static bool LoadPasswordFromSettings();
    static void SavePasswordToSettings();


    static void SetKey(uint8_t* key);
    static void SetKeySet();
    static uint8_t* GetKey();
    static std::vector<uint8_t> GetKeyVector();
    static bool IsKeySet();


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

    static std::string getCallsign();
    static void setCallsign(const std::string &value);

protected:
private:
    static uint8_t m_key[16];
    static bool m_keySet;

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
