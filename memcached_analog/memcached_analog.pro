TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += ../__submoudles/googletest/googletest/include \
  ../__submoudles/googletest/googletest \
  ../__submoudles/asio/asio/include

LIBS += -pthread

DEFINES += ASIO_STANDALONE

SOURCES += main.cpp \
  cache.cpp \
    ../__submoudles/googletest/googletest/src/gtest-all.cc \
    cache_test.cpp \
    server.cpp \
    protocol.cpp \
    protocol_test.cpp \
    storage.cpp \
    storage_test.cpp

HEADERS += \
  cache.h \
    server.h \
    protocol.h \
    storage.h
