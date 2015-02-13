TARGET = xgine

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
    LIBS += ../debug/libsdl2.lib
} else {
    DESTDIR = ../release
    LIBS += ../release/libsdl2.lib
}
