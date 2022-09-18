QT       += core gui xml multimedia network webenginewidgets websockets webchannel
RC_ICONS = note.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calendarlabel.cpp \
    chatserver.cpp \
    controller.cpp \
    customcalendarwidget.cpp \
    lobal.cpp \
    main.cpp \
    mainwindow.cpp \
    mydef.cpp \
    mynote.cpp \
    newscheduledialog.cpp \
    schedule.cpp \
    schedulemanager.cpp \
    scrolledlabel.cpp \
    settingdialog.cpp \
    singleapplication.cpp \
    speechrecognizer.cpp \
    wavrecorder.cpp \
    websocketclientwrapper.cpp \
    websockettransport.cpp \
    worker.cpp

HEADERS += \
    calendarlabel.h \
    chatserver.h \
    controller.h \
    customcalendarwidget.h \
    lobal.h \
    mainwindow.h \
    mydef.h \
    mynote.h \
    newscheduledialog.h \
    schedule.h \
    schedulemanager.h \
    scrolledlabel.h \
    settingdialog.h \
    singleapplication.h \
    speechrecognizer.h \
    wavrecorder.h \
    websocketclientwrapper.h \
    websockettransport.h \
    worker.h

FORMS += \
    mainwindow.ui \
    mynote.ui \
    newscheduledialog.ui \
    schedulemanager.ui \
    settingdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    sources.qrc
