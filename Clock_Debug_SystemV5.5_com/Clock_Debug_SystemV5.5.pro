QT       += core gui
QT       += core gui sql
QT       += core gui serialport
QT       +=serialport
QT       +=sql
QT       +=gui

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
    Editdb.cpp \
    RTCdata.cpp \
    history.cpp \
    main.cpp \
    mainwindow.cpp \
    notice.cpp \
    relation.cpp \
    rtcmessage.cpp \
    selectdevice.cpp

HEADERS += \
    Editdb.h \
    RTCdata.h \
    history.h \
    mainwindow.h \
    notice.h \
    relation.h \
    rtcmessage.h \
    selectdevice.h

FORMS += \
    history.ui \
    mainwindow.ui \
    notice.ui \
    relation.ui \
    rtcmessage.ui \
    selectdevice.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pic.qrc

TARGET = RtcDebugSystem

DISTFILES += \
    logo.rc

RC_FILE = logo.rc
