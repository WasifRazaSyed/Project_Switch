QT += core remoteobjects network gui widgets
CONFIG += c++17 windows
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        remote.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

REPC_REPLICA += \
    remote.rep

QMAKE_LFLAGS += -mwindows

HEADERS += \
    remote.h

RESOURCES += \
    logo.qrc

unix|win32: LIBS += -lWtsapi32
unix|win32: LIBS += -luser32

RC_ICONS += \
        "G:\Rason Detre\Projects\FYP_RENEW\Project_Switch\signal\logo\icons8-signal-32.ico" \


