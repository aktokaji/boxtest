TEMPLATE = app
TARGET = main
#QT -= core gui
QT += core
QT -= gui
CONFIG += console

SOURCES += main.cpp ../win32.cpp ../win32_print.cpp ../win32_vector.cpp \
    ../sbox.cpp \
    ../wine.cpp
SOURCES +=

msvc {
  DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS _UNICODE
  QMAKE_CFLAGS_RELEASE += -Z7
  QMAKE_CXXFLAGS_RELEASE += -Z7
  QMAKE_CFLAGS_DEBUG += -WX
  QMAKE_CXXFLAGS_DEBUG += -WX
  QMAKE_LFLAGS -= /DYNAMICBASE /NXCOMPAT
  QMAKE_LFLAGS += /FIXED /BASE:0x10000000
  #QMAKE_LFLAGS += /STACK:20000000
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

#INCLUDEPATH += V:\QtBuild\+lua\lua-5.3.0-$$[QMAKESPEC]\src
#QMAKE_LFLAGS += -L"V:\QtBuild\+lua\lua-5.3.0-$$[QMAKESPEC]\src\release"
#QMAKE_LIBS += -llua -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwinspool -lwinmm -lshell32 -lcomctl32 -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 -lwsock32 -lodbc32

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(QMAKESPEC: $$[QMAKESPEC])
message()

HEADERS += \
    ../MemoryModule.h \
    ../sbox.h \
    ../strconv.h \
    ../vardecl.h \
    ../win32.h \
    ../win32_print.h \
    ../win32_vector.h \
    ../wine.h \
    ../wine_teb.h
