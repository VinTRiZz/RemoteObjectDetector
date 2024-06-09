QT += core network widgets sql
CONFIG += c++17

INCLUDEPATH += src
INCLUDEPATH += $$PWD/include
TEMPLATE = app
TARGET = ControlGUI

LIBS += -lcrypto -lxcb

include(src/src.pri)
include(build.pri)
