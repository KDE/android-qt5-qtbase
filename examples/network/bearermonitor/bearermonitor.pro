TARGET = bearermonitor
QT = core gui network widgets

HEADERS = sessionwidget.h \
          bearermonitor.h

SOURCES = main.cpp \
          bearermonitor.cpp \
          sessionwidget.cpp

FORMS = bearermonitor_240_320.ui \
        bearermonitor_640_480.ui \
        sessionwidget.ui

win32:!wince*:LIBS += -lws2_32
wince*:LIBS += -lws2

CONFIG += console

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtbase/network/bearermonitor
sources.files = $$SOURCES $$HEADERS *.ui *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtbase/network/bearermonitor
INSTALLS += target sources
