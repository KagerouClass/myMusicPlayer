#-------------------------------------------------
#
# Project created by QtCreator 2017-05-25T15:55:18
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = music_test
TEMPLATE = app

RC_FILE =myico.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    musicmessage.cpp \
    VictorCode/channelswidget.cpp \
    VictorCode/getbaiduchannel.cpp \
    VictorCode/music_radio_driver.cpp \
    VictorCode/getsongplaylistid.cpp \
    VictorCode/downloadfile.cpp \
    VictorCode/getsongreallink.cpp \
    VictorCode/getlrc.cpp \
    VictorCode/getaristpic.cpp

HEADERS  += mainwindow.h \
    musicmessage.h \
    VictorCode/channelswidget.h \
    VictorCode/getbaiduchannel.h \
    VictorCode/music_radio_driver.h \
    VictorCode/getsongplaylistid.h \
    VictorCode/downloadfile.h \
    VictorCode/getsongreallink.h \
    VictorCode/getlrc.h \
    VictorCode/getaristpic.h

FORMS    += mainwindow.ui \
    musicmessage.ui \
    VictorCode/channelswidget.ui

RESOURCES += \
   image.qrc

ISTFILES += \
    myico.rc
