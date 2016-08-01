TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += ../__submoudles/googletest/googletest/include \
  ../__submoudles/googletest/googletest

LIBS += -pthread

SOURCES += main.cpp \
  cache.cpp \
    ../__submoudles/googletest/googletest/src/gtest-all.cc \
    cache_test.cpp

HEADERS += \
  cache.h
