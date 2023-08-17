#-------------------------------------------------
#
# QtCreator Project attributions
#
# describe the paths the project depend on
# date:   2014-01-17
# version:V1.0
#
# modifydate:V1.0 : 2014-11-08
#
#-------------------------------------------------

###################################################################
#
#
#各个软件项目单独版本控制时，项目配置管理目录如下：
#    software/
#       3rd/
#           LibName                         # 这是LIB_DIRS指定的目录
#               Windows
#                   include
#                       LibName
#                           xxxxx.h
#                   lib_x64
#                           LIBLIB_x64.lib      # 这里的LIBLIB是VAR_DEPLIBS指定的名字
#                           LIBLIB_x64.dll
#                           LIBLIB_x64_d.lib
#                           LIBLIB_x64_d.dll
#                   lib_x86
#                           LIBLIB_x86.lib      # 这里的LIBLIB是VAR_DEPLIBS指定的名字
#                           LIBLIB_x86.dll
#                           LIBLIB_x86_d.lib
#                           LIBLIB_x86_d.dll
#               Linux
#                   ....
#               ArmLinux
#                   ....
#               Android
#                   ....
#       include/
#       lib/        #当前编译出来的库
#       project/    #项目文件
#       src/        #源文件
#       bin/        #生成的binary
#       test/       #测试
#       doc/
#       build/
#
#每个工程的project目录如下
#    project
#       QTCreator
#           project.pro
#           sub_project/
#               sub_project.pro
#           pub_pris/
#               build_app.pri   # 普通应用程序pri
#               build_dynlib.pri# 普通动态库程序pri
#               ...
#
##################################################

##################请根据实际配置修改这一行##########################
#CONFIG += cmit_product

##################请不要修改一下所有配置##########################

# 一次编译debug和release
CONFIG += build_all

# x86 还是 x64架构
ARCH = x86
contains(QMAKE_HOST.arch, x86_64):{
    ARCH = x64
}
ARCHSUFFIX = _$$ARCH
DEFINES += $$ARCHSUFFIX

TOOLSET =
TOOLSETSUFFIX =
win32{
    lessThan(QMAKE_MSC_VER,1600) {
        error("only suport vs2010 +")
    } lessThan(QMAKE_MSC_VER,1700) {
        TOOLSET = vc100 # 1600对应vs2010
    } lessThan(QMAKE_MSC_VER,1800) {
        TOOLSET = vc100 # 1700对应vs2012
    } lessThan(QMAKE_MSC_VER,1900) {
        TOOLSET = vc100 # 1800对应vs2013
    } else {
        TOOLSET = vc140 # 1900对应vs2025, 对于vs编译器，从vs2015(vc140)开始，二进制兼容，之前的版本需要分开
    }
    TOOLSETSUFFIX = _$$TOOLSET
}
LIBSUFFIX = $$TOOLSETSUFFIX
LIBSUFFIX = $$join(LIBSUFFIX,,,_$$ARCH)

# plantform define
win32:contains(QMAKE_CC, cl){
    CONFIG += PROPALTFORM
    PROPALTFORM = Windows
}
unix{
    CONFIG += PROPALTFORM
    PROPALTFORM = Linux
}
COMPILERNAME=$$basename(QMAKE_CC)
CC_PARTS = $$split(COMPILERNAME,-)
if(contains(CC_PARTS,"arm")) {
    CONFIG += PROPALTFORM
    PROPALTFORM = ArmLinux
    # ArmLinus is _Linux
    DEFINES += _Linux
}else{
}
android{
    PROPALTFORM = Android
    DEFINES += _Android
}
DEFINES += _$$PROPALTFORM

#build type define
BUILDTYPE=
CONFIG(debug, debug|release) {
    BUILDTYPE = Debug
} else {
    BUILDTYPE = Release
}

# project path define
P_T = $$PWD/../../.. # project top path
SOFRWARE_PATH = $$P_T/..
#project path vars
PTVAR_SRC = $$P_T/src
PTVAR_INC = $$P_T/include
PTVAR_SAMP = $$P_T/samples
PTVAR_TEST = $$P_T/test
PTVAR_BIN = $$P_T/bin/$$PROPALTFORM$$ARCHSUFFIX/
#PTVAR_BIN = $$P_T/bin/$$PROPALTFORM/$$BUILDTYPE
PTVAR_LIB = $$P_T/lib/$$ARCH # 本项目编译生成的库路径
PTVAR_3rdLIB = $$P_T/3rd
PTVAR_EXTLIB = $$P_T/../lib
PTVAR_BUILD = $$P_T/build/$$PROPALTFORM$$LIBSUFFIX

message("current platform is" $$PROPALTFORM "with compiler" $$ARCH $$COMPILERNAME LIBSUFFIX -> $$LIBSUFFIX)
message("current bin path is" $$PTVAR_BIN)
message("current build path is" $$PTVAR_BUILD)

#-------------------------------------------------
#
# QtCreator's project's private path attributions
#
# author: jiangming dangbinghoo
# date:   2013-11-28
# version:v0.1(project template version)
#-------------------------------------------------

# this tells qmake to find files in project top when in trouble
VPATH += $$P_T
VPATH += $$PTVAR_SRC
VPATH += $$PTVAR_INC
VPATH += $$PTVAR_SAMP
VPATH += $$PTVAR_TEST

isEmpty(LIB_3RD_DIRS){
#   LIB_3RD_DIRS = boost
}

############################INCLUDE PATH##############################
# make include avalible to all sources.
INCLUDEPATH += $$PTVAR_INC
INCLUDEPATH += $$PTVAR_SRC
INCLUDEPATH += $$PTVAR_TEST


#3rd part library inclues
for(NAME_I , LIB_3RD_DIRS) {
    PROPALTFORM{
        NAME_1 = $$PTVAR_3rdLIB/$$NAME_I/$$PROPALTFORM/include
        INCLUDEPATH += $$NAME_1
    }
}

#外部lib part library inclues
for(NAME_I , LIB_DIRS) {
    PROPALTFORM{
        NAME_1 = $$PTVAR_EXTLIB/$$NAME_I/$$PROPALTFORM/include
        INCLUDEPATH += $$NAME_1
    }
}

############################LIB PATH##############################
LIBS += -L$$PTVAR_BIN
LIBS += -L$$PTVAR_LIB

#3rd part library libs
for(NAME_L , LIB_3RD_DIRS) {
    PROPALTFORM{
        NAME_1 = $$PTVAR_3rdLIB/$$NAME_L/$$PROPALTFORM/lib$$ARCHSUFFIX
        LIBS += -L$$NAME_1
    }
}
# 外部lib
for(NAME_L , LIB_DIRS) {
    PROPALTFORM{
        NAME_1 = $$PTVAR_EXTLIB/$$NAME_L/$$PROPALTFORM/lib$$ARCHSUFFIX
        LIBS += -L$$NAME_1
    }
}



TARGET_DIR = $$TARGET
isEmpty(TARGET) {
    TARGET_DIR=other
}

OBJECTS_DIR = $$PTVAR_BUILD/$$BUILDTYPE/$$TARGET_DIR/o
UI_DIR      = $$PTVAR_BUILD/$$BUILDTYPE/$$TARGET_DIR/ui
MOC_DIR     = $$PTVAR_BUILD/$$BUILDTYPE/$$TARGET_DIR/moc
RCC_DIR     = $$PTVAR_BUILD/$$BUILDTYPE/$$TARGET_DIR/rcc


#win32:contains(QMAKE_CC, cl):LIBS += /LIBPATH:$$P_T/lib
#include($$SOFRWARE_PATH/dlloutpath.pri)

# auto copy dll to dir(just for remote depoly)
unix{
#target.path = /home/pi/qt5/AMS
#INSTALLS += target
}

