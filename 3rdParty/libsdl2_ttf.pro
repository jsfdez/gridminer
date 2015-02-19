TEMPLATE = lib
CONFIG += staticlib

include(../config.pri)

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
    OBJECTS_DIR = libsdl2_ttf_debug_obj
} else {
    DESTDIR = ../release
    OBJECTS_DIR = libsdl2_ttf_release_obj
}

DEFINES += _SCL_SECURE_NO_WARNINGS
DEFINES += _WINDOWS
DEFINES += BUILD_WINDOWS
DEFINES += _WIN32

LIBS += winmm.lib

INCLUDEPATH += libsdl2_image
INCLUDEPATH += libsdl2/include

HEADERS += $$files("libsdl2_ttf/*.h")
libsdl_precompiled {
    OTHER_FILES += $$files(libsdl2_ttf/lib/*.*, true)
} else {
    SOURCES += $$files("libsdl2_ttf/*.c")
}
