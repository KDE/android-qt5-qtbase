TARGET = qcorewlanbearer
load(qt_plugin)

QT = core-private network-private
LIBS += -framework Foundation -framework SystemConfiguration

contains(QT_CONFIG, corewlan) {
    !contains(QMAKE_MAC_SDK, ".*MacOSX10\\.[345]\\.sdk") {
         LIBS += -framework CoreWLAN -framework Security
    }
}

HEADERS += qcorewlanengine.h \
           ../qnetworksession_impl.h \
           ../qbearerengine_impl.h

SOURCES += main.cpp \
           ../qnetworksession_impl.cpp

OBJECTIVE_SOURCES += qcorewlanengine.mm

OTHER_FILES += corewlan.json

DESTDIR = $$QT.network.plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
