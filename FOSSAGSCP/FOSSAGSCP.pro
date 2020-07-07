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
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrix.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gObserver.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gSatTEME.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTime.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTimeSpan.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleFileAccessor.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleParser.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVector.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/mathUtils.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4ext.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4io.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4unit.cpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/solarProcedures.cpp \
    3rdparty/fossasattracker/src/ObserverInformation.cpp \
    3rdparty/fossasattracker/src/SatelliteInformation.cpp \
    3rdparty/fossasattracker/src/SatelliteSimulation.cpp \
    3rdparty/fossasattracker/src/SimulationResult.cpp \
    3rdparty/fossasattracker/src/TLE.cpp \
    3rdparty/tiny-aes/aes.cpp \
    mappane.cpp \
    messagelogframe.cpp \
    src/Interpreter.cpp \
    SerialPortThread.cpp \
    main.cpp \
    mainwindow.cpp \
    systeminformationpane.cpp

HEADERS += \
    3rdparty/FOSSA-Comms/FOSSA-Comms.h \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gException.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrix.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrixTempl.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gObserver.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gSatTEME.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTime.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleFileAccessor.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleParser.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVector.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVectorTempl.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/mathUtils.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4ext.h \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4io.h \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4unit.h \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/solarProcedures.hpp \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/src/stdsat.h \
    3rdparty/fossasattracker/src/ObserverInformation.hpp \
    3rdparty/fossasattracker/src/SatelliteInformation.hpp \
    3rdparty/fossasattracker/src/SatelliteSimulation.hpp \
    3rdparty/fossasattracker/src/SimulationResult.hpp \
    3rdparty/fossasattracker/src/TLE.hpp \
    3rdparty/tiny-aes/aes.h \
    include/Interpreter.h \
    include/Interfaces/IGroundStationSerialMessage.h \
    include/Interfaces/IMessage.h \
    include/MessageLog.h \
    include/Settings.h \
    include/GroundStationSerialMessage.h \
    SerialPortThread.h \
    mainwindow.h \
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

DISTFILES += \
    3rdparty/fossasattracker/libs/gsat-r11-fixed/Cmakelists.txt
