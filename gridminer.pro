!build_pass:message(GridMiner by Jesus Fernandez (jsfdez@gmail.com))

TEMPLATE = subdirs

SUBDIRS += src \
    libsdl2

libsdl2.file = 3rdParty/libsdl2.pro
src.subdir = src
#src.depends = 3rdParty

#OTHER_FILES += \
#    .gitignore \
#    shader/vertex.vsh \
#    shader/fragment.fsh \
#    README.md \
#    shader/plane.frag \
#    shader/plane.vert
