CONFIG += testcase
TARGET = tst_qlayout

QT += widgets widgets-private testlib

SOURCES += tst_qlayout.cpp
wince* {
   addFiles.files = baseline
   addFiles.path = .
   DEPLOYMENT += addFiles
} else {
   TESTDATA += baseline/*
}
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
