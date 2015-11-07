#-------------------------------------------------
#
# Project created 2014-08-02
#
#-------------------------------------------------

TEMPLATE = lib
TARGET = gmock

# Set the path for the generated library
GENERATED_DIR = ../../generated

# Use common project definitions
include(../../common.pri)

# compile gmock as static library
CONFIG -= qt app_bundle
CONFIG += staticlib thread

# suppress compiler warnings
CONFIG += warn_off

INCLUDEPATH += \
    gtest \
    gtest/include \
    include

SOURCES += \
    gtest/src/gtest-all.cc \
    src/gmock-all.cc
