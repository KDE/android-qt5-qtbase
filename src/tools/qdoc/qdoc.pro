option(host_build)
TEMPLATE = app
TARGET = qdoc

DESTDIR = ../../../bin
DEFINES += QDOC2_COMPAT

include(../bootstrap/bootstrap.pri)
DEFINES -= QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_TRANSLATION

INCLUDEPATH += $$QT_SOURCE_TREE/src/tools/qdoc \
               $$QT_SOURCE_TREE/src/tools/qdoc/qmlparser \
               $$QT_BUILD_TREE/include/QtXml \
               $$QT_BUILD_TREE/include/QtXml/$$QT_VERSION \
               $$QT_BUILD_TREE/include/QtXml/$$QT_VERSION/QtXml

DEPENDPATH += $$QT_SOURCE_TREE/src/tools/qdoc \
              $$QT_SOURCE_TREE/src/tools/qdoc/qmlparser \
              $$QT_SOURCE_TREE/src/xml

# Increase the stack size on MSVC to 4M to avoid a stack overflow
win32-msvc*:{
    QMAKE_LFLAGS += /STACK:4194304
}

HEADERS += atom.h \
           codechunk.h \
           codemarker.h \
           codeparser.h \
           config.h \
           cppcodemarker.h \
           cppcodeparser.h \
           ditaxmlgenerator.h \
           doc.h \
           editdistance.h \
           generator.h \
           helpprojectwriter.h \
           htmlgenerator.h \
           location.h \
           node.h \
           openedlist.h \
           plaincodemarker.h \
           puredocparser.h \
           quoter.h \
           separator.h \
           text.h \
           tokenizer.h \
           tr.h \
           tree.h
SOURCES += atom.cpp \
           codechunk.cpp \
           codemarker.cpp \
           codeparser.cpp \
           config.cpp \
           cppcodemarker.cpp \
           cppcodeparser.cpp \
           ditaxmlgenerator.cpp \
           doc.cpp \
           editdistance.cpp \
           generator.cpp \
           helpprojectwriter.cpp \
           htmlgenerator.cpp \
           location.cpp \
           main.cpp \
           node.cpp \
           openedlist.cpp \
           plaincodemarker.cpp \
           puredocparser.cpp \
           quoter.cpp \
           separator.cpp \
           text.cpp \
           tokenizer.cpp \
           tree.cpp \
           yyindent.cpp \
           ../../corelib/tools/qcryptographichash.cpp \
           ../../xml/dom/qdom.cpp \
           ../../xml/sax/qxml.cpp

### QML/JS Parser ###

include(qmlparser/qmlparser.pri)

HEADERS += jscodemarker.h \
            qmlcodemarker.h \
            qmlcodeparser.h \
            qmlmarkupvisitor.h \
            qmlvisitor.h

SOURCES += jscodemarker.cpp \
            qmlcodemarker.cpp \
            qmlcodeparser.cpp \
            qmlmarkupvisitor.cpp \
            qmlvisitor.cpp

### Documentation for qdoc ###

qtPrepareTool(QDOC, qdoc)
qtPrepareTool(QHELPGENERATOR, qhelpgenerator)

equals(QMAKE_DIR_SEP, /) {
    QDOC = QT_BUILD_TREE=$$QT_BUILD_TREE QT_SOURCE_TREE=$$QT_SOURCE_TREE $$QDOC
} else {
    QDOC = set QT_BUILD_TREE=$$QT_BUILD_TREE&& set QT_SOURCE_TREE=$$QT_SOURCE_TREE&& $$QDOC
    QDOC = $$replace(QDOC, "/", "\\")
}

html-docs.commands = $$QDOC $$PWD/doc/config/qdoc.qdocconf
html-docs.files = $$PWD/doc/html

qch-docs.commands = $$QHELPGENERATOR $$PWD/doc/html/qdoc.qhp -o $$PWD/doc/qch/qdoc.qch
qch-docs.files = $$PWD/doc/qch
qch-docs.path = $$[QT_INSTALL_DOCS]
qch-docs.CONFIG += no_check_exist directory

QMAKE_EXTRA_TARGETS += html-docs qch-docs

target.path = $$[QT_HOST_BINS]
INSTALLS += target

# Ensure that qdoc is built before we generate the docs
docs.commands =
docs.depends = first
QMAKE_EXTRA_TARGET += docs

load(qt_targets)
