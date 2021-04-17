QT -= gui

TARGET = machine
CONFIG += c++14
CONFIG(debug, debug|release) {
    CONFIG += sanitizer sanitize_address
}

TEMPLATE = lib
CONFIG += staticlib

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS_DEBUG += -ggdb
QMAKE_CXXFLAGS_DEBUG += -Wno-c99-designator
QMAKE_CXXFLAGS_DEBUG += -Wextra -Wnull-dereference -Wdouble-promotion -Wformat=2
QMAKE_CXXFLAGS_RELEASE += -Wno-c99-designator

DEFINES += MACHINE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    machine.cpp \
    simulator_exception.cpp \
    core.cpp \
    instruction.cpp \
    registers.cpp \
    programloader.cpp \
    alu.cpp \
    machineconfig.cpp \
    symboltable.cpp \
    cop0state.cpp \
    memory/cache/cache.cpp \
    memory/cache/cache_policy.cpp \
    memory/memory_bus.cpp \
    memory/frontend_memory.cpp \
    memory/backend/basic_memory.cpp \
    memory/backend/peripspiled.cpp \
    memory/backend/serialport.cpp \
    memory/backend/lcddisplay.cpp \
    memory/backend/mmap_memory.cpp

HEADERS += \
    machine.h \
    simulator_exception.h \
    core.h \
    instruction.h \
    registers.h \
    programloader.h \
    alu.h \
    machineconfig.h \
    utils.h \
    machinedefs.h \
    programloader.h \
    symboltable.h \
    cop0state.h \
    memory/backend/basic_memory.h \
    memory/cache/cache.h \
    memory/cache/cache_policy.h \
    memory/cache/cache_types.h \
    memory/memory_utils.h \
    memory/endian.h \
    memory/memory_bus.h \
    memory/frontend_memory.h \
    memory/backend/peripspiled.h \
    memory/backend/lcddisplay.h \
    memory/backend/peripheral.h \
    memory/backend/serialport.h \
    memory/backend/backend_memory.h \
    memory/backend/mmap_memory.h
