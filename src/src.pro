TARGET = gridminer
CONFIG -= qt

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
    LIBS += ../debug/libsdl2.lib
} else {
    DESTDIR = ../release
    LIBS += ../release/libsdl2.lib
}

LIBS += User32.lib
LIBS += Shell32.lib

INCLUDEPATH += ../3rdParty/libsdl2/include

SOURCES += main.cpp
