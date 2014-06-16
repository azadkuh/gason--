QT       -= core
QT       -= gui
CONFIG   += console
unix:macx:CONFIG   -= app_bundle

TARGET = builder1
TEMPLATE = app
include(../commondir.pri)


SOURCES += main.cpp \
    ../../src/gason.cpp

HEADERS += \
    ../../src/gason.hpp

