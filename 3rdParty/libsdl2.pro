TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
} else {
    DESTDIR = ../release
}

DEFINES += _SCL_SECURE_NO_WARNINGS
DEFINES += _WINDOWS
DEFINES += BUILD_WINDOWS
DEFINES += _WIN32

INCLUDEPATH += libsdl2/include
INCLUDEPATH += libsdl2/include/SDL

HEADERS += $$files(libsdl2/include/*.h)

HEADERS += $$files(libsdl2/src/*.h)
SOURCES += $$files(libsdl2/src/*.c)
SOURCES += $$files(libsdl2/src/file/*.c)
SOURCES += $$files(libsdl2/src/libm/*.c)
SOURCES += $$files(libsdl2/src/test/*.c)
SOURCES += $$files(libsdl2/src/atomic/*.c)
SOURCES += $$files(libsdl2/src/dynapi/*.c)
SOURCES += $$files(libsdl2/src/events/*.c)
SOURCES += $$files(libsdl2/src/stdlib/*.c)
SOURCES += $$files(libsdl2/src/cpuinfo/*.c)

SOURCES += $$files(libsdl2/src/audio/*.c)
SOURCES += $$files(libsdl2/src/audio/xaudio2/*.c)

SOURCES += $$files(libsdl2/src/timer/*.c)
SOURCES += $$files(libsdl2/src/timer/windows/*.c)

SOURCES += $$files(libsdl2/src/core/*.c)
SOURCES += $$files(libsdl2/src/core/windows/*.c)

SOURCES += $$files(libsdl2/src/main/*.c)
SOURCES += $$files(libsdl2/src/main/windows/*.c)

SOURCES += $$files(libsdl2/src/power/*.c)
SOURCES += $$files(libsdl2/src/power/windows/*.c)

SOURCES += $$files(libsdl2/src/thread/*.c)
SOURCES += $$files(libsdl2/src/thread/windows/*.c)
SOURCES += $$files(libsdl2/src/thread/stdcpp/*.cpp)

SOURCES += $$files(libsdl2/src/loadso/*.c)
SOURCES += $$files(libsdl2/src/loadso/windows/*.c)

SOURCES += $$files(libsdl2/src/haptic/*.c)
SOURCES += $$files(libsdl2/src/haptic/windows/*.c)

SOURCES += $$files(libsdl2/src/render/*.c)
SOURCES += $$files(libsdl2/src/render/software/*.c)
SOURCES += $$files(libsdl2/src/render/direct3d/*.c)

SOURCES += $$files(libsdl2/src/joystick/*.c)
SOURCES += $$files(libsdl2/src/joystick/windows/*.c)

SOURCES += $$files(libsdl2/src/filesystem/*.c)
SOURCES += $$files(libsdl2/src/filesystem/windows/*.c)

SOURCES += $$files(libsdl2/src/video/*.c)
SOURCES += $$files(libsdl2/src/video/windows/*.c)
