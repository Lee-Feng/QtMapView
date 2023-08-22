#-------------------------------------------------
#
# QtCreator's dynamic lib project attributions
#
# date:   2013-11-28
#
# V2.0 date:   2014-11-10
# version:V2.0(not project version)
#-------------------------------------------------

#-------------------USAGE---------------------------
#特性：
# 1) 这是一个通用工程模板，仅需少许更改即可适用于当前所有项目
# 2) 模板中的配置顺序为：基本设置、依赖库、工程源文件（.h .cpp .ui等）
# 3) 通过基本设置中包含不同的pri实现不同工程模板。
#
#使用方法：
# 第1步（可选）：根据实际情况设置版本号：如：VERSION = 1.0.0
# 第2步（可选）：添加自定义的CONFIG，如想打开控制台时使用CONFIG += console
# 第3步（可选）：依赖库的目录设置。未正确设置可能导致编译时出现找不到对应文件的错误。
# 第4步（可选）：设置工程生成的目标名。默认使用工程名。
# 第5步（可选）：预定义宏。
#*第6步（必选）：包含工程模板文件。
#*第7步（必选）：设置工程依赖的库。
# 第8步（可选）：添加工程源文件。
#
# 注意：请不要随便更改使用方法的步骤
#      可选步骤确实没有使用到就可以注释掉。
#      必选步骤必须根据实际情况选择
#
#----------------END-OF-USAGE-----------------------


#----------------基本设置---------------------------
#
# user defInitions are presented below
# notice:don't try to change the orders in any case
#
#-------------------------------------------------
#步骤1：设置版本号#version
VERSION = 1.0.0
# 1. 激光清障仪控制接口

QT += location location-private
#步骤2.1：添加自定义的CONFIG项

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#步骤3：设置本项目依赖的第三方库的目录
#项目自己编译出来的库都会放到$PT/lib目录。该目录会自动添加到库的寻找目录。
#如果项目依赖了非自己代码编译的库那么库可以放在$PT/3rd目录。然后通过LIB_3RD_DIRS指定。
#如果项目依赖了非自己代码编译的库那么库可以放在$PT/../lib目录。然后通过LIB_DIRS指定。
#
LIB_3RD_DIRS =
#LIB_SRC_DIRS =
LIB_DIRS =

#步骤4：设置工程生成的目标名
# target name . default set as projectname. you can replace the target name  with your own target name.
#TARGET =

#步骤5：预定义宏
# defInitions, Add declaration for build dll which will be used in config.hpp
DEFINES += LF_QMAPVIEW_BUILD_DLL
DEFINES += VERSION_STR=$$VERSION

#步骤6：包含工程模板文件。
# build as a dynamic(shared) lib
# 说明：包含不同的pri来实现不同的工程：
#         build_app.pri         普通应用程序工程
#         build_qtapp.pri       QT应用程序工程
#         build_component.pri   组件动态库工程
#         build_dynlib.pri      普通动态库工程
#         build_qtdynlib.pri    QT动态库工程
#         build_staticlib.pri   普通静态库工程
#         build_cplugin.pri     普通classloader插件工程
#         build_cqtplugin.pri   QT classloader插件工程
#根据不同的工程，使用include (../pub_pris/build_xxx.pri)包含对应的pri文件即可
include (../pub_pris/build_qtdynlib.pri)

#------------------依赖库--------------------------
#
# manage dependences
# Additional libs list. Please Add your actural Additional libs to VAR_DEPLIBS
# VAR_DEPLIBS = testlib1 testlib2 testlib3
#
#-------------------------------------------------
#步骤7：设置工程依赖的库。
#本工程以来的库有3种来源：1）属于本配置体系生成的库，（debug版本默认加了"_d"）如第三方库：test3rdlib，其debug版本的名字为：test3rdlib_d.dll
#                     2）纯的第三方库没有什么后缀。
#                     3）带“d”后缀的第三方库。如第三方库：test3rdlib，其debug版本的名字为：test3rdlibd.dll
#                     4）其他形式的库请自己根据平台以及debug/release的实际情况添加到VAR_3RDLIBS中。示例如下：
#                          unix:CONFIG(debug,debug|release):{
#                                VAR_3RDLIBS += test3rdlib_DD4
#                          }
# 设置完依赖的库变量之后，包含libs_inc.pri文件include (../pub_pris/libs_inc.pri)
VAR_DEPLIBS =
VAR_3RDLIBS =
VAR_3RDLIBS_DEBUG_END_WITH_D =

#manage dependences. (notice:do not change this line below)
include (../pub_pris/libs_inc.pri)


#--------------工程源文件--------------------------
# files for the project are list here
# available variables :PTVAR_SRC
#                      PTVAR_INC
#-------------------------------------------------
#步骤7：添加工程源文件。
#可以使用相对目录，也可以只给文件名让qtcreator自己在 PTVAR_SRC、 PTVAR_INC、 PTVAR_SAMP、 PTVAR_TEST 下自己依次搜索
#注意：手动添加时文件时，不要错误使用SOURCES和HEADERS。头文件不能添加到SOURCES下

#include(../pub_pris/deploy_to_publib.pri)

#项目源代码

HEADERS += $$PTVAR_INC/QtMapView/QtMapViewConfig.hpp
HEADERS += $$PTVAR_INC/QtMapView/QtMapItem.hpp
SOURCES += $$PTVAR_SRC/QtMapView/QtMapItem.cpp
HEADERS += $$PTVAR_INC/QtMapView/QtMapView.hpp
SOURCES += $$PTVAR_SRC/QtMapView/QtMapView.cpp

HEADERS += $$PTVAR_INC/QtMapView/QtMapGifItem.hpp
