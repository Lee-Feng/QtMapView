#pragma once

#ifndef LF_EXTERN_C
#	ifdef __cplusplus
#		define LF_EXTERN_C    extern "C"
#	else
#		define LF_EXTERN_C    extern
#	endif
#endif

#if defined( WIN32 )
#	define LF_SYMBOL_IMPORT __declspec(dllimport)
#	define LF_SYMBOL_EXPORT __declspec(dllexport)
#	define LF_SYMBOL_IMPORT_C LF_EXTERN_C __declspec(dllimport)
#	define LF_SYMBOL_EXPORT_C LF_EXTERN_C __declspec(dllexport)
#	define LF_SYMBOL_LOCAL
#elif defined ( __GNUC__ )
#	define LF_SYMBOL_IMPORT __attribute__ ((visibility("default")))
#	define LF_SYMBOL_EXPORT __attribute__ ((visibility("default")))
#	define LF_SYMBOL_IMPORT_C  LF_EXTERN_C __attribute__ ((visibility("default")))
#	define LF_SYMBOL_EXPORT_C  LF_EXTERN_C __attribute__ ((visibility("default")))
#	define LF_SYMBOL_LOCAL  __attribute__ ((visibility("hidden")))
#else
#	define LF_SYMBOL_IMPORT
#	define LF_SYMBOL_EXPORT
#	define LF_SYMBOL_IMPORT_C
#	define LF_SYMBOL_EXPORT_C
#	define LF_SYMBOL_LOCAL
#endif
