QT -= gui

TARGET = qtmips_asm
CONFIG += c++14
CONFIG(debug, debug|release) {
    CONFIG += sanitizer sanitize_address
}

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/../qtmips_machine $$PWD/../qtmips_osemu
DEPENDPATH += $$PWD/../qtmips_machine

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS_DEBUG += -ggdb
QMAKE_CXXFLAGS_DEBUG += -Wextra -Wnull-dereference -Wdouble-promotion -Wformat=2
QMAKE_CXXFLAGS_DEBUG += -Wno-c99-designator
QMAKE_CXXFLAGS_RELEASE += -Wno-c99-designator

DEFINES += QTMIPS_MACHINE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    fixmatheval.cpp \
    simpleasm.cpp

HEADERS += \
    fixmatheval.h \
    messagetype.h \
    simpleasm.h
