/*
*/
#pragma once

/*
USAGE:
库生成：
	说明：生成库的名字组成包括（1、前缀（有lib为静态库，无lib为动态库）， 2、工程名， 3、后缀（_d标示调试版，否则为发布版））。如libtinyxml_d.lib(tinyxml的调试静态库lib, tinyxml.lib tinyxml的发布动态库lib)
    如果是动态库工程，则需要在库工程中 预定义UTILES_LOGGER_BUILD_DLL，修改工程debug属性配置中“目标文件名”为“$(ProjectName)_d”, 修改工程release属性配置中“目标文件名”为“$(ProjectName)”
    如果是静态库工程，则需要在库工程中 预定义UTILES_LOGGER_BUILD_LIB，修改工程debug属性配置中“目标文件名”为“lib$(ProjectName)_d”, 修改工程release属性配置中“目标文件名”为“lib$(ProjectName)”

库使用：
	用户可以在工程中通过预定义的方式指明使用的是动态库还是静态库。
    也可以使用默认的配置，#define UTILES_LOGGER_DEFAULTUSE_LIB 则默认使用静态库，否则如果包含 “#define UTILES_LOGGER_DEFAULTUSE_DLL”则默认使用动态库

源码使用：
    如果不生产库，也不使用库，而是使用源码方式，则可在应用程序工程中预定义 UTILES_LOGGER_USE_SOURCE，定义了UTILES_LOGGER_USE_SOURCE之后不会自动连接library。
*/
#define UTILES_LOGGER_DEFAULTUSE_DLL
//#define UTILES_LOGGER_DEFAULTUSE_LIB

// default UTILES_LOGGER_USE_LIB--------------------------------------------------//
#if !defined(UTILES_LOGGER_BUILD_DLL) && !defined(UTILES_LOGGER_BUILD_LIB) && !defined(UTILES_LOGGER_USE_DLL) && !defined(UTILES_LOGGER_USE_LIB)
#	if defined(UTILES_LOGGER_DEFAULTUSE_DLL)
#		define UTILES_LOGGER_USE_DLL
#	else
#		define UTILES_LOGGER_USE_LIB
#	endif
#endif

#if defined(UTILES_LOGGER_USE_DLL) && defined(UTILES_LOGGER_USE_LIB)
#	if defined(UTILES_LOGGER_DEFAULTUSE_DLL)
#		undef UTILES_LOGGER_USE_LIB
#	else
#		undef UTILES_LOGGER_USE_DLL
#	endif
#endif

// if use source in you code ,yue must define UTILES_LOGGER_USE_SOURCE
#if defined(UTILES_LOGGER_USE_SOURCE)
#	if defined(UTILES_LOGGER_BUILD_DLL)
#		undef UTILES_LOGGER_BUILD_DLL
#	endif

#	if defined(UTILES_LOGGER_BUILD_LIB)
#		undef UTILES_LOGGER_BUILD_LIB
#	endif

#	if defined(UTILES_LOGGER_USE_DLL)
#		undef UTILES_LOGGER_USE_DLL
#	endif

#	if defined(UTILES_LOGGER_USE_LIB)
#		undef UTILES_LOGGER_USE_LIB
#	endif
#endif

#include "suffix.hpp"
//  enable dynamic linking  ------------------------------------------------------------// 
#define UTILES_LOGGER_DECL_LOCAL   LF_SYMBOL_LOCAL
#if defined(UTILES_LOGGER_BUILD_DLL) //Build dll
#   define UTILES_LOGGER_DECL      LF_SYMBOL_EXPORT
#   define UTILES_LOGGER_DECL_C    LF_SYMBOL_EXPORT_C
#elif defined(UTILES_LOGGER_USE_DLL) //Use dll
#   define UTILES_LOGGER_DECL      LF_SYMBOL_IMPORT
#   define UTILES_LOGGER_DECL_C    LF_SYMBOL_EXPORT_C
#else
#   define UTILES_LOGGER_DECL      LF_SYMBOL_LOCAL
#   define UTILES_LOGGER_DECL_C    LF_SYMBOL_LOCAL
#endif

//  enable automatic library variant selection  ----------------------------------------// 
//#if defined(_MSC_VER) || (defined(WIN32) && (!defined ( __MINGW32__ )))
#if !defined(UNUSE_LIB_AUTOLINK)
#if !defined(UTILES_LOGGER_BUILD_DLL) && !defined(UTILES_LOGGER_BUILD_LIB) && !defined(UTILES_LOGGER_USE_SOURCE)
//
// Tell the autolink to link dynamically, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#if defined(UTILES_LOGGER_USE_DLL)
#   define LF_DYN_LINK
#endif

#define LF_LIB_NAME logger
#define LF_LIB_IGNORE_TOOLSET
//
// And include the header that does the work:
//
#include <auto_link/auto_link.hpp>
#endif// auto-linking disabled
#endif
