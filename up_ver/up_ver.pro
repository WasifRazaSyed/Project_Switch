QT += core
CONFIG += c++17
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        saver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    saver.h
QMAKE_LFLAGS += -mwindows

RC_ICONS += "G:\Rason Detre\Projects\FYP_RENEW\Project_Switch\up_ver\logo\icons8-buy-upgrade-32.ico"

unix|win32: LIBS += -lWtsapi32

RESOURCES +=
