#-------------------------------------------------
#
# QtCreator's static lib project's private attributions
#
# date:   2013-11-28
# version:v0.1(project template version)
#-------------------------------------------------

# static lib
TEMPLATE = lib
CONFIG  += staticlib
CONFIG  -= qt
QT      -= core gui

win32:TARGET_EXT = .lib

#  path
include(path.pri)

# lib dest to lib dir
DESTDIR = $$PTVAR_LIB

# unix install path
unix{
	target.path = /usr/lib
	INSTALLS += target
}
