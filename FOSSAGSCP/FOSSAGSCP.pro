QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    src/main.cpp \
    src/mainwindow.cpp

HEADERS += \
    Framework/Components/Interfaces/DatabaseAccess/IActiveRecord.h \
    Framework/Components/Interfaces/Messaging/IEndpoint.h \
    Framework/Components/Interfaces/Messaging/IMessage.h \
    Framework/Components/Interfaces/Messaging/IMessageChannel.h \
    Framework/Components/Interfaces/Messaging/IMessageCommandProcessor.h \
    Framework/Components/Interfaces/Messaging/IMessageRouter.h \
    Framework/Components/Interfaces/ResourceManagment/IBuffer.h \
    Framework/Components/Interfaces/ResourceManagment/IContainer.h \
    Framework/Components/Interfaces/ResourceManagment/IContextObject.h \
    Framework/Components/Interfaces/ResourceManagment/IFactory.h \
    Framework/Components/Interfaces/ResourceManagment/IFrameworkObject.h \
    Framework/Components/Interfaces/ResourceManagment/IManagedObject.h \
    Framework/Components/Interfaces/ResourceManagment/IRegistry.h \
    Framework/Components/Interfaces/ResourceManagment/IService.h \
    Framework/Framework.h \
    Framework/FrameworkObjects/DatabaseFrameworkObject.h \
    Framework/FrameworkObjects/MessagingFrameworkObject.h \
    Framework/FrameworkObjects/ResourceManagementFrameworkObject.h \
    Framework/Services/ChannelsService.h \
    Framework/Services/DatabaseService.h \
    Framework/Services/MessageProcessorsService.h \
    Framework/Services/RoutersService.h \
    include/mainwindow.h

INCLUDEPATH += include
INCLUDEPATH += 3rdparty/FOSSA-Comms
INCLUDEPATH += 3rdparty/quicktle-master/include
INCLUDEPATH += 3rdparty/tiny-aes
FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    FOSSAGSCP_en_GB.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    key.txt \

#RESOURCES +=
