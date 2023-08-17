#pragma once

#if defined( WIN32 )
#	define LF_LICENSE_IMPORT __declspec(dllimport)
#	define LF_LICENSE_EXPORT __declspec(dllexport)
#	define LF_LICENSE_LOCAL
#elif defined ( __GNUC__ )
#	define LF_LICENSE_IMPORT __attribute__ ((visibility("default")))
#	define LF_LICENSE_EXPORT __attribute__ ((visibility("default")))
#	define LF_LICENSE_LOCAL  __attribute__ ((visibility("hidden")))
#else
#	define LF_LICENSE_IMPORT
#	define LF_LICENSE_EXPORT
#	define LF_LICENSE_LOCAL
#endif

#if defined(LF_LICENSE_BUILD_DLL) //Build dll
#   define LF_LICENSE_DECL      LF_LICENSE_EXPORT
#else
#   define LF_LICENSE_DECL      LF_LICENSE_IMPORT
#endif
