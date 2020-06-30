#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <string>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>

////////////////
/// Settings ///
////////////////
class Settings
{
public:
    ///
    /// \brief This function takes the hex string from the settings file, and converts it to uint8_t array.
    /// \return
    ///
    bool LoadKeyFromSettings()
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

        this->SetKey(tempKey);

        return true;
    }

    void SaveKeyToSettings()
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        QString filename = "settings.ini" ;
        QSettings settings(path + "/"+ filename, QSettings::IniFormat);

        uint8_t* key = this->GetKey(); // 16 byte key needs to turn into a 32 hex string.

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



    bool LoadPasswordFromSettings()
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
            this->SetPassword(password);

            return true;
        }
    }

    void SavePasswordToSettings()
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        QString filename = "settings.ini" ;
        QSettings settings(path + "/"+ filename, QSettings::IniFormat);

        std::string password = this->GetPassword();
        QString str = QString::fromStdString(password);
        settings.setValue("password", str);
    }







    void SetKey(uint8_t* key)
    {
        for (int i = 0; i < 16; i++)
        {
            uint8_t v = key[i];
            m_key[i] = v;
        }
    }
    void SetKeySet()
    {
        m_keySet = true;
    }

    uint8_t* GetKey() { return m_key; }
    bool IsKeySet() { return m_keySet; }


    void SetPassword(std::string password)
    {
        m_password = password;
    }
    void SetPasswordSet()
    {
        m_passwordSet = true;
    }

    std::string GetPassword() { return m_password; }
    bool IsPasswordSet() { return m_passwordSet; }
private:
    // AES KEY
    uint8_t m_key[16];
    bool m_keySet = false;

    // Password.
    std::string m_password = "";
    bool m_passwordSet = false;

};

#endif // SETTINGS_H
