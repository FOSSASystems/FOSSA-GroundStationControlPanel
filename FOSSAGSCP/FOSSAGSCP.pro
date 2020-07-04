QT       += core gui serialport location quickwidgets widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    3rdparty/FOSSA-Comms/FOSSA-Comms.cpp \
    3rdparty/tiny-aes/aes.cpp \
    mappane.cpp \
    messagelogframe.cpp \
    src/SerialPortThread.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    systeminformationpane.cpp

HEADERS += \
    3rdparty/FOSSA-Comms/FOSSA-Comms.h \
    3rdparty/tiny-aes/aes.h \
    include/Interfaces/IGroundStationSerialMessage.h \
    include/Interfaces/IMessage.h \
    include/Components/MessageLog.h \
    include/Components/Settings.h \
    include/Components/GroundStationSerialMessage.h \
    include/SerialPortThread.h \
    include/mainwindow.h \
    mappane.h \
    messagelogframe.h \
    systeminformationpane.h \

INCLUDEPATH += include/
INCLUDEPATH += 3rdparty/FOSSA-Comms
INCLUDEPATH += 3rdparty/quicktle-master/include
INCLUDEPATH += 3rdparty/tiny-aes

FORMS += \
    mainwindow.ui \
    mappane.ui \
    messagelogframe.ui \
    systeminformationpane.ui

TRANSLATIONS += \
    FOSSAGSCP_en_GB.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += qmlResource.qrc
