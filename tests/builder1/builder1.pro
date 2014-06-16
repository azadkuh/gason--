QT       -= core
QT       -= gui
CONFIG   += c++11 console
unix:macx:CONFIG   -= app_bundle

TARGET = parser1
TEMPLATE = app
include(../commondir.pri)


SOURCES += main.cpp \
    ../../src/gason.cpp

HEADERS += \
    ../../src/gason.hpp

