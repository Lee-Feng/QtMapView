#pragma once

#if defined( WIN32 )
#	define LF_LASERDEVAPI_IMPORT __declspec(dllimport)
#	define LF_LASERDEVAPI_EXPORT __declspec(dllexport)
#	define LF_LASERDEVAPI_LOCAL
#elif defined ( __GNUC__ )
#	define LF_LASERDEVAPI_IMPORT __attribute__ ((visibility("default")))
#	define LF_LASERDEVAPI_EXPORT __attribute__ ((visibility("default")))
#	define LF_LASERDEVAPI_LOCAL  __attribute__ ((visibility("hidden")))
#else
#	define LF_LASERDEVAPI_IMPORT
#	define LF_LASERDEVAPI_EXPORT
#	define LF_LASERDEVAPI_LOCAL
#endif

#if defined(LF_LASERDEVAPI_BUILD_DLL) //Build dll
#   define LF_LASERDEVAPI_DECL      LF_LASERDEVAPI_EXPORT
#else
#   define LF_LASERDEVAPI_DECL      LF_LASERDEVAPI_IMPORT
#endif
