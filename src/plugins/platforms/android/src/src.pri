load(qt_plugin)

QT += core-private gui-private widgets-private platformsupport-private
DESTDIR = $$QT.gui.plugins/platforms/android
CONFIG += qpa/genericunixfontdatabase

OTHER_FILES += $$PWD/android.json

INCLUDEPATH += $$PWD

contains(QT, opengl): DEFINES += ANDROID_PLUGIN_OPENGL

SOURCES += $$PWD/androidplatformplugin.cpp \
           $$PWD/androidjnimain.cpp \
           $$PWD/qandroidplatformintegration.cpp \
           $$PWD/qandroidplatformservices.cpp \
           $$PWD/qandroidassetsfileenginehandler.cpp \
           $$PWD/qandroidinputcontext.cpp \
           $$PWD/qandroidplatformfontdatabase.cpp \
           $$PWD/qandroidplatformclipboard.cpp

HEADERS += $$PWD/qandroidplatformintegration.h \
           $$PWD/androidjnimain.h \
           $$PWD/qandroidplatformservices.h \
           $$PWD/qandroidassetsfileenginehandler.h \
           $$PWD/qandroidinputcontext.h \
           $$PWD/qandroidplatformfontdatabase.h \
           $$PWD/qandroidplatformclipboard.h


target.path += $$[QT_INSTALL_PLUGINS]/platforms/android
INSTALLS += target
