QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/FOSSA-Comms/FOSSA-Comms.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrix.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gObserver.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gSatTEME.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTime.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTimeSpan.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleFileAccessor.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleParser.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVector.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/mathUtils.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4ext.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4io.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4unit.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/solarProcedures.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/ObserverInformation.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SatelliteInformation.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SatelliteSimulation.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SimulationResult.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/TLE.cpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/tiny-aes/aes.cpp \
    ../../../FOSSA-GroundStationControlPanel/SerialPortThread.cpp \
    ../../../FOSSA-GroundStationControlPanel/Settings.cpp \
    ../../../FOSSA-GroundStationControlPanel/main.cpp \
    ../../../FOSSA-GroundStationControlPanel/mainwindow.cpp \
    ../../../FOSSA-GroundStationControlPanel/messagelogframe.cpp \
    ../../../FOSSA-GroundStationControlPanel/systeminformationpane.cpp

HEADERS += \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/FOSSA-Comms/FOSSA-Comms.h \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gException.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrix.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gMatrixTempl.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gObserver.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gSatTEME.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTime.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleFileAccessor.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTleParser.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVector.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gVectorTempl.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/mathUtils.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4ext.h \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4io.h \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/sgp4unit.h \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/solarProcedures.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/libs/gsat-r11-fixed/src/stdsat.h \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/ObserverInformation.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SatelliteInformation.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SatelliteSimulation.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/SimulationResult.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/fossasattracker/src/TLE.hpp \
    ../../../FOSSA-GroundStationControlPanel/3rdparty/tiny-aes/aes.h \
    ../../../FOSSA-GroundStationControlPanel/DopplerShiftCorrector.h \
    ../../../FOSSA-GroundStationControlPanel/SerialPortThread.h \
    ../../../FOSSA-GroundStationControlPanel/Settings.h \
    ../../../FOSSA-GroundStationControlPanel/mainwindow.h \
    ../../../FOSSA-GroundStationControlPanel/messagelogframe.h \
    ../../../FOSSA-GroundStationControlPanel/systeminformationpane.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    ../../../FOSSA-GroundStationControlPanel/mainwindow.ui \
    ../../../FOSSA-GroundStationControlPanel/messagelogframe.ui \
    ../../../FOSSA-GroundStationControlPanel/systeminformationpane.ui
