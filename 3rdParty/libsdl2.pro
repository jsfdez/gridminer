TEMPLATE = lib
CONFIG += staticlib
#CONFIG += warn_off
CONFIG -= qt

DEFINES += _CRT_SECURE_NO_WARNINGS
DEFINES += _SCL_SECURE_NO_WARNINGS

INCLUDEPATH += libsdl2/include

HEADERS += $$files(libsdl2/include/*.h)
SOURCES += $$files(libsdl2/src/*.cpp, true)
