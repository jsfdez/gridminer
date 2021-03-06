TARGET = gridminer
CONFIG -= qt

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
#    LIBS += ../debug/libsdl2.lib
#    LIBS += ../debug/libsdl2_image.lib
} else {
    DESTDIR = ../release
#    LIBS += ../release/libsdl2.lib
#    LIBS += ../release/libsdl2_image.lib
}

LIBS += User32.lib
LIBS += Shell32.lib
LIBS += Winmm.lib
LIBS += Ole32.lib
LIBS += Gdi32.lib
LIBS += Version.lib
LIBS += Imm32.lib
LIBS += OleAut32.lib
LIBS += ../3rdParty/libsdl2/lib/$$(PROCESSOR_ARCHITECTURE)/SDL2.lib
LIBS += ../3rdParty/libsdl2/lib/$$(PROCESSOR_ARCHITECTURE)/SDL2main.lib
LIBS += ../3rdParty/libsdl2_image/lib/$$(PROCESSOR_ARCHITECTURE)/SDL2_image.lib
LIBS += ../3rdParty/libsdl2_ttf/lib/$$(PROCESSOR_ARCHITECTURE)/SDL2_ttf.lib

INCLUDEPATH += ../3rdParty/libsdl2_image
INCLUDEPATH += ../3rdParty/libsdl2/include
INCLUDEPATH += ../3rdParty/libsdl2_ttf/include

HEADERS += $$files(*.h)
SOURCES += $$files(*.cpp)
