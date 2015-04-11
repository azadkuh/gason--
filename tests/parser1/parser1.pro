QT       -= core
QT       -= gui
CONFIG   += console c++11
unix:macx:CONFIG   -= app_bundle

TARGET = parser1
TEMPLATE = app
include(../commondir.pri)

DEFINES  += GASON_DEBUG_ALLOCATOR


SOURCES += main.cpp \
    ../../src/gason.cpp

HEADERS += \
    ../../src/gason.hpp

