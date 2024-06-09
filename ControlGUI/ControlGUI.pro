QT += core network widgets sql
CONFIG += c++17

INCLUDEPATH += src
INCLUDEPATH += $$PWD/include
TEMPLATE = app
TARGET = ControlGUI

LIBS += -lcrypto -lxcb -lopencv_imgcodecs -lopencv_core -lopencv_imgproc

include(src/src.pri)
include(build.pri)
