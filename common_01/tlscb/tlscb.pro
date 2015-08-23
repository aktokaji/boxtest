TEMPLATE = app
#TARGET = main
#QT -= core gui
QT += core
QT -= gui
CONFIG += console

DESTDIR = E:\testbed

SOURCES += main.cpp \
    ../win32_print.cpp

msvc {
  DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS _UNICODE
  QMAKE_CFLAGS_RELEASE += -Z7
  QMAKE_CXXFLAGS_RELEASE += -Z7
  QMAKE_CFLAGS_DEBUG += -WX
  QMAKE_CXXFLAGS_DEBUG += -WX
  #QMAKE_LFLAGS -= /DYNAMICBASE /NXCOMPAT
  #BQMAKE_LFLAGS += /FIXED /BASE:0x10000000
}

gcc {
  DEFINES += STRSAFE_NO_DEPRECATE
  QMAKE_CFLAGS_RELEASE += -g -Werror -finput-charset=UTF-8 -fexec-charset=CP932 -Wno-error=unused-function -Wno-error=unused-parameter -Wno-error=empty-body -Wno-error=unused-variable
  QMAKE_CXXFLAGS_RELEASE += -std=gnu++11 -g -Werror -finput-charset=UTF-8 -fexec-charset=CP932 -Wno-error=unused-function -Wno-error=unused-parameter -Wno-error=empty-body -Wno-error=unused-variable
  QMAKE_LFLAGS += -municode
  QMAKE_LFLAGS += -Wl,--image-base=0x10000000
}

INCLUDEPATH += ..
QMAKE_LIBS += -luser32
QMAKE_LIBS += -ldbghelp
QMAKE_LIBS += -lpsapi

LIBS += -LE:\testbed -ltlsdll

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message()

HEADERS += \
    ../vardecl.h \
    ../win32_print.h
