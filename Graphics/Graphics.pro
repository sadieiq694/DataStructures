TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
SOURCES += main.cpp \
    graphics.cpp \
    vec2d.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    window.h \
    graphics.h

SUBDIRS += \
    SimpleGraphics.pro \
    SimpleGraphics.pro

