#ifndef DATAGRAMPROCESSOR_H
#define DATAGRAMPROCESSOR_H

#include "InboundDatagram.h"
#include "OutboundDatagram.h"
#include "FCPFrame.h"

#include "Settings.h"

#include "CStructs.h"
#include "VersionStrings.h"

#include "ui_mainwindow.h"
#include "ui_systeminformationpane.h"
#include "ui_messagelogframe.h"
#include "messagelogframe.h"

#include <FOSSA-Comms.h>
#include <QObject>
#include <vector>
#include <QDebug>

class DatagramProcessor : public QObject
{
    Q_OBJECT
public:
    DatagramProcessor(Ui::MainWindow* mainWindowUI, Ui::systeminformationpane* systemInfoUI, MessageLogFrame* messageLogFrame);

    void ProcessDatagram(IDatagram* datagram);

    /// @todo replace char with uint8_t and int8_t. 27/07/2020 R.B
    /// @todo verify that transmit functions take char* with \0 terminator for Strlen(). 27/07/2020 R.B
    /// @todo replace uint8_t values with bool where appropriate. 27/07/2020 R.B

signals:
    void StartDopplerCorrector();

private:
    Ui::MainWindow* m_mainWindowUI;
    Ui::systeminformationpane* m_systemInfoUI;
    MessageLogFrame* m_messageLog;

    void ProcessHandshake(IDatagram *datagram);
    void ProcessGroundStationConfigurationChange(IDatagram *datagram);
    void ProcessFrame(IDatagram *datagram);
    void ProcessGroundstationCarrierFrequencyChange(IDatagram* datagram);
};

#endif // DATAGRAMPROCESSOR_H
