#-------------------------------------------------
#
# QtCreator Project attributions
#
# describe the libs the project depend on
#
# date:   2013-11-28
# version:V0.2
#-------------------------------------------------
#you may need to use microsoft visual c++ compiler istead of gcc(MinGW) 
win32:contains(QMAKE_CC, gcc):error("you may need to use microsoft visual c++ compiler istead of gcc(MinGW),this could be done under tool->options->make and run->compiler of QT Creator")

############################flags##############################
unix{
    QMAKE_CXXFLAGS += -Wall
}
unix:debug {
    QMAKE_CXXFLAGS += -g
}
win32:contains(QMAKE_CC, cl){
    QMAKE_CXXFLAGS -= -Zc:wchar_t-
    QMAKE_CXXFLAGS += -Zc:wchar_t
}

############################libs ##############################
# QT creator pro public defines
DEFINES += UNUSE_LIB_AUTOLINK

#suffix the target
beacomponent{
    TARGET = $$join(TARGET,,,_c)
}
classloader_plugin{
    TARGET = $$join(TARGET,,,_plugin)
}
TARGET = $$join(TARGET,,,$$LIBSUFFIX)
CONFIG(debug,debug|release):TARGET = $$join(TARGET,,,_d)

# for android lib sufix is XXX_armeabi-v7a.so
LIB_ABIS=
android{
LIB_ABIS=_$$ANDROID_ABIS
}

#libs to be dependent on
!isEmpty(VAR_DEPLIBS):{
    for (libvar_d, VAR_DEPLIBS){
        libvar_d = $$join(libvar_d,,,$$LIBSUFFIX)
        CONFIG(debug,debug|release):libvar_d = $$join(libvar_d,,,_d)
        android{
        }
        LIBS += -l$$libvar_d$$LIB_ABIS
    }
}

#3rd part libs to be dependent on
!isEmpty(VAR_3RDLIBS):{
    for (libvar_3rd, VAR_3RDLIBS){
        LIBS += -l$$libvar_3rd
    }
}

#VAR_3RDLIBS_DEBUG_END_WITH_D
!isEmpty(VAR_3RDLIBS_DEBUG_END_WITH_D):{
    for (libvard, VAR_3RDLIBS_DEBUG_END_WITH_D){
        CONFIG(debug,debug|release):libvard = $$join(libvard,,,d)
        LIBS += -l$$libvard$$LIB_ABIS
    }
}

#-------------------------------------------------
# debug define in unix
#-------------------------------------------------

CONFIG(debug,debug|release): DEFINES+= DEBUG _DEBUG
