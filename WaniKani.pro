ARGET = WaniKani

TEMPLATE = app

QT += core gui network widgets

DEFINES += QT_DEPRECATED_WARNINGS

win32: LIBS += -lUser32
mac: LIBS += -framework AppKit

SOURCES = src/main.cpp \
          src/dialog.cpp

HEADERS = src/dialog.h

FORMS = src/dialog.ui

mac: OBJECTIVE_SOURCES = src/macos.mm

RESOURCES = res/resources.qrc

mac: QMAKE_INFO_PLIST = Res/Info.plist
