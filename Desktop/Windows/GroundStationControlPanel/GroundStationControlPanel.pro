QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/FOSSA-Comms/FOSSA-Comms.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrix.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gObserver.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gSatTEME.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTime.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTimeSpan.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleFileAccessor.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleParser.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVector.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/mathUtils.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4ext.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4io.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4unit.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/solarProcedures.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/ObserverInformation.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SatelliteInformation.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SatelliteSimulation.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SimulationResult.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/TLE.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/tiny-aes/aes.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/SerialPortThread.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/Settings.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/main.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/mainwindow.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/messagelogframe.cpp \
    ../../Sources/FOSSA-GroundStationControlPanel/systeminformationpane.cpp

HEADERS += \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/FOSSA-Comms/FOSSA-Comms.h \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gException.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrix.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrixTempl.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gObserver.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gSatTEME.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTime.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleFileAccessor.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleParser.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVector.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVectorTempl.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/mathUtils.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4ext.h \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4io.h \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4unit.h \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/solarProcedures.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/stdsat.h \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/ObserverInformation.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SatelliteInformation.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SatelliteSimulation.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SimulationResult.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/TLE.hpp \
    ../../Sources/FOSSA-GroundStationControlPanel/3rdparty/tiny-aes/aes.h \
    ../../Sources/FOSSA-GroundStationControlPanel/DopplerShiftCorrector.h \
    ../../Sources/FOSSA-GroundStationControlPanel/SerialPortThread.h \
    ../../Sources/FOSSA-GroundStationControlPanel/Settings.h \
    ../../Sources/FOSSA-GroundStationControlPanel/mainwindow.h \
    ../../Sources/FOSSA-GroundStationControlPanel/messagelogframe.h \
    ../../Sources/FOSSA-GroundStationControlPanel/systeminformationpane.h

FORMS += \
    ../../Sources/FOSSA-GroundStationControlPanel/mainwindow.ui \
    ../../Sources/FOSSA-GroundStationControlPanel/messagelogframe.ui \
    ../../Sources/FOSSA-GroundStationControlPanel/systeminformationpane.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
