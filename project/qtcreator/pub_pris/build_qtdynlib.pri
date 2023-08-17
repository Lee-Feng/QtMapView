#-------------------------------------------------
#
# QtCreator's dynamic lib project's private attributions
#
# date:   2013-11-28
# version:v0.1(project template version)
#-------------------------------------------------

# shared lib
TEMPLATE = lib
CONFIG += qt
QT += core
CONFIG += shared

win32:TARGET_EXT = .dll

#  path
include(path.pri)

# Specifies where to put the target file(.dll and .lib(.a) in windows .so and .a in linux)
win32:DESTDIR = $$PTVAR_LIB

#!isEmpty(VAR_COMPNAME):unix:DESTDIR = $$P_T/bin/$$VAR_COMPNAME
#else:unix:DESTDIR = $$PTVAR_BIN
unix:DESTDIR = $$PTVAR_BIN

#Specifies where to copy the target dll. usually to the directory "bin" which placed in the project root directory
win32:DLLDESTDIR += $$PTVAR_BIN

# unix install path
unix{
#	target.path = /usr/lib
#	INSTALLS += target
}

## android can not set DESTDIR,that will cause link faild
#android{
#DESTDIR =
#}
