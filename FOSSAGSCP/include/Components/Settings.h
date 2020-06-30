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
    bool LoadKeyFromSettings()
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        QString filename = "settings.ini" ;
        QSettings settings(path + "/"+ filename, QSettings::IniFormat);

        QByteArray keyText = settings.value("key", "").toByteArray();

        if (keyText.length() != 16)
        {
            return false;
        }

        uint8_t tempKey[16];
        for (uint8_t i = 0; i < 16; i++)
        {
            uint8_t v = keyText[i];
            tempKey[i] = v;
        }

        this->SetKey(tempKey);

        return true;
    }

    void SaveKeyToSettings()
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        QString filename = "settings.ini" ;
        QSettings settings(path + "/"+ filename, QSettings::IniFormat);

        uint8_t* key = this->GetKey();

        QByteArray arr;
        for (uint8_t i = 0; i < 16; i++)
        {
            arr.push_back(key[i]);
        }
        settings.setValue("key", arr);
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
