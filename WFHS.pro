#-------------------------------------------------
#
# Project created by QtCreator 2014-08-22T08:48:48
#
#-------------------------------------------------

QT       += core gui svg network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WFHS
TEMPLATE = app

SOURCES += main.cpp\
        widget.cpp \
    3rd-party/potentiometer/potentiometer.cpp \
    3rd-party/serialport/qextserialbase.cpp \
    3rd-party/navigationbar/navigationbarview.cpp \
    3rd-party/navigationbar/navigationitem.cpp \
    3rd-party/digit/digit.cpp \
    room.cpp \
    datapipethread.cpp \
    tcpclient.cpp \
    tcpserver.cpp \
    global.cpp \
    dataparsethread.cpp \
    settimedlg.cpp \
    addroomdlg.cpp \
    roomschedulethread.cpp \
    comboboxdelegate.cpp \
    readonlydelegate.cpp \
    user.cpp \
    adduserdlg.cpp \
    logindlg.cpp \
    userandroommgrdlg.cpp \
    nwkmgrdlg.cpp \
    creatadminstratordlg.cpp

HEADERS  += widget.h \
    3rd-party/potentiometer/potentiometer.h \
    3rd-party/serialport/qextserialbase.h \
    3rd-party/navigationbar/navigationbarview.h \
    3rd-party/navigationbar/navigationitem.h \
    3rd-party/digit/digit.h \
    room.h \
    datapipethread.h \
    tcpclient.h \
    tcpserver.h \
    global.h \
    dataparsethread.h \
    settimedlg.h \
    addroomdlg.h \
    roomschedulethread.h \
    comboboxdelegate.h \
    readonlydelegate.h \
    user.h \
    adduserdlg.h \
    logindlg.h \
    userandroommgrdlg.h \
    nwkmgrdlg.h \
    creatadminstratordlg.h

win32 {
    HEADERS += 3rd-party/serialport/win_qextserialport.h
    SOURCES += 3rd-party/serialport/win_qextserialport.cpp
}

unix {
    HEADERS += 3rd-party/serialport/posix_qextserialport.h
    SOURCES += 3rd-party/serialport/posix_qextserialport.cpp
}

FORMS    += widget.ui \
    settimedlg.ui \
    addroomdlg.ui \
    adduserdlg.ui\
    login.ui \
    userandroommgrdlg.ui \
    nwkmgrdlg.ui \
    creatadminstratordlg.ui

RESOURCES += \
    res/potentiometer.qrc \
    res/togglebutton.qrc \
    res/navigationbar.qrc \

CONFIG(release, debug | release) {
    DEFINES += _RELEASE
    HEADERS += 3rd-party/SYSZUXpinyin/syszuxpinyin.h \
        3rd-party/SYSZUXpinyin/syszuxim.h
    SOURCES += 3rd-party/SYSZUXpinyin/syszuxpinyin.cpp \
        3rd-party/SYSZUXpinyin/syszuxim.cpp
    FORMS += 3rd-party/SYSZUXpinyin/syszuxpinyin.ui
    RESOURCES += 3rd-party/SYSZUXpinyin/syszux/syszuxpinyin.qrc
}

DISTFILES +=
