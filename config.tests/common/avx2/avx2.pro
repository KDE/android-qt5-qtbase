SOURCES = avx2.cpp
CONFIG -= x11 qt
mac:CONFIG -= app_bundle
isEmpty(QMAKE_CFLAGS_AVX2):error("This compiler does not support AVX2")
else:QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_AVX2