!build_pass:message(GridMiner by Jesus Fernandez (jsfdez@gmail.com))
TEMPLATE = subdirs
include(config.pri)

SUBDIRS += src
SUBDIRS += libsdl2
SUBDIRS += libsdl2_image

libsdl2.file = 3rdParty/libsdl2.pro
libsdl2_image.file = 3rdParty/libsdl2_image.pro

libsdl2_image.depends = libsdl2
src.depends = libsdl2 libsdl2_image
