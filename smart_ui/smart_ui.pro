
QT       += core gui network remoteobjects

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    logo_widget.cpp \
    main.cpp \
    remote.cpp \
    serial_key.cpp \
    settings_widget.cpp \
    status_bar.cpp \
    title_bar.cpp \
    title_widget.cpp \
    widget.cpp

HEADERS += \
    logo_widget.h \
    remote.h \
    serial_key.h \
    settings_widget.h \
    status_bar.h \
    title_bar.h \
    title_widget.h \
    widget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rsc.qrc

RC_ICONS += "G:\Rason Detre\Projects\FYP_RENEW\Project_Switch\smart_ui\logo\win_icon.ico"

unix|win32: LIBS += -lWtsapi32
unix|win32: LIBS += -luser32

REPC_SOURCE  += \
    remote.rep

