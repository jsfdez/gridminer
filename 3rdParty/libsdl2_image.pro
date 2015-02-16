TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
    OBJECTS_DIR = libsdl2_image_debug_obj
} else {
    DESTDIR = ../release
    OBJECTS_DIR = libsdl2_image_release_obj
}


DEFINES += _SCL_SECURE_NO_WARNINGS
DEFINES += _WINDOWS
DEFINES += BUILD_WINDOWS
DEFINES += _WIN32

LIBS += winmm.lib

INCLUDEPATH += libsdl2_image
INCLUDEPATH += libsdl2/include

SOURCES += $$files("libsdl2_image/*.c")
