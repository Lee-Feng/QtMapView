#pragma once

#if defined( WIN32 )
#	define LF_QMAPVIEW_IMPORT __declspec(dllimport)
#	define LF_QMAPVIEW_EXPORT __declspec(dllexport)
#	define LF_QMAPVIEW_LOCAL
#elif defined ( __GNUC__ )
#	define LF_QMAPVIEW_IMPORT __attribute__ ((visibility("default")))
#	define LF_QMAPVIEW_EXPORT __attribute__ ((visibility("default")))
#	define LF_QMAPVIEW_LOCAL  __attribute__ ((visibility("hidden")))
#else
#	define LF_QMAPVIEW_IMPORT
#	define LF_QMAPVIEW_EXPORT
#	define LF_QMAPVIEW_LOCAL
#endif

#if defined(LF_QMAPVIEW_BUILD_DLL) //Build dll
#   define LF_QMAPVIEW_DECL      LF_QMAPVIEW_EXPORT
#else
#   define LF_QMAPVIEW_DECL      LF_QMAPVIEW_IMPORT
#endif
