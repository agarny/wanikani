TARGET = WaniKani

TEMPLATE = app

QT += core gui network widgets

DEFINES += QT_DEPRECATED_WARNINGS

win32: LIBS += -lUser32
mac: LIBS += -framework AppKit

INCLUDEPATH += src/3rdparty/QtSingleApplication \
               src/3rdparty/zlib

SOURCES = src/main.cpp \
          src/wanikani.cpp \
          src/widget.cpp \
          src/3rdparty/QtSingleApplication/qtlocalpeer.cpp \
          src/3rdparty/QtSingleApplication/qtsingleapplication.cpp \
          src/3rdparty/zlib/adler32.c \
          src/3rdparty/zlib/compress.c \
          src/3rdparty/zlib/crc32.c \
          src/3rdparty/zlib/deflate.c \
          src/3rdparty/zlib/gzclose.c \
          src/3rdparty/zlib/gzlib.c \
          src/3rdparty/zlib/gzread.c \
          src/3rdparty/zlib/gzwrite.c \
          src/3rdparty/zlib/infback.c \
          src/3rdparty/zlib/inffast.c \
          src/3rdparty/zlib/inflate.c \
          src/3rdparty/zlib/inftrees.c \
          src/3rdparty/zlib/trees.c \
          src/3rdparty/zlib/uncompr.c \
          src/3rdparty/zlib/zutil.c

HEADERS = src/wanikani.h \
          src/widget.h \
          src/3rdparty/QtSingleApplication/qtlocalpeer.h \
          src/3rdparty/QtSingleApplication/qtsingleapplication.h \
          src/3rdparty/zlib/zutil.h \
          src/3rdparty/zlib/crc32.h \
          src/3rdparty/zlib/deflate.h \
          src/3rdparty/zlib/gzguts.h \
          src/3rdparty/zlib/inffast.h \
          src/3rdparty/zlib/inffixed.h \
          src/3rdparty/zlib/inflate.h \
          src/3rdparty/zlib/inftrees.h \
          src/3rdparty/zlib/trees.h \
          src/3rdparty/zlib/zconf.h \
          src/3rdparty/zlib/zlib.h

FORMS = src/widget.ui

mac: OBJECTIVE_SOURCES = src/macos.mm

RESOURCES = res/resources.qrc

mac: QMAKE_INFO_PLIST = res/Info.plist
