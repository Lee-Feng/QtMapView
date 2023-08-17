#pragma once

#if defined( WIN32 )
#	define LF_ASYNCINVK_IMPORT __declspec(dllimport)
#	define LF_ASYNCINVK_EXPORT __declspec(dllexport)
#	define LF_ASYNCINVK_LOCAL
#elif defined ( __GNUC__ )
#	define LF_ASYNCINVK_IMPORT __attribute__ ((visibility("default")))
#	define LF_ASYNCINVK_EXPORT __attribute__ ((visibility("default")))
#	define LF_ASYNCINVK_LOCAL  __attribute__ ((visibility("hidden")))
#else
#	define LF_ASYNCINVK_IMPORT
#	define LF_ASYNCINVK_EXPORT
#	define LF_ASYNCINVK_LOCAL
#endif

#if defined(LF_ASYNCINVK_BUILD_DLL) //Build dll
#   define LF_ASYNCINVK_DECL      LF_ASYNCINVK_EXPORT
#else
#   define LF_ASYNCINVK_DECL      LF_ASYNCINVK_IMPORT
#endif
