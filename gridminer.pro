!build_pass:message(GridMiner by Jesus Fernandez (jsfdez@gmail.com))
TEMPLATE = subdirs
CONFIG -= qt

SUBDIRS += src
#SUBDIRS += libsdl2
#SUBDIRS += libsdl2_image
#src.depends = libsdl2

libsdl2.file = 3rdParty/libsdl2.pro
libsdl2_image.file = 3rdParty/libsdl2_image.pro
