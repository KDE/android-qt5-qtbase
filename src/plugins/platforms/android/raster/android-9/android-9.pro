PLUGIN_TYPE = platforms
load(qt_plugin)

TARGET = android-9

DEFINES += QT_STATICPLUGIN

CONFIG += dll

!contains(ANDROID_PLATFORM, android-9) {
    INCLUDEPATH += $$NDK_ROOT/platforms/android-9/arch-$$ANDROID_ARCHITECTURE/usr/include
    LIBS += -L$$NDK_ROOT/platforms/android-9/arch-$$ANDROID_ARCHITECTURE/usr/lib -ljnigraphics -landroid
} else : LIBS += -ljnigraphics -landroid

include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/src.pri)
include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/raster/raster.pri)

INSTALLS += target