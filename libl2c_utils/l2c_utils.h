#pragma once

#ifdef L2C_UTILS_EXPORTS
#define L2C_UTILS_API __declspec(dllexport)
#pragma message( "Build utils DLL" )
#else
#define L2C_UTILS_API __declspec(dllimport)
#endif



extern L2C_UTILS_API void l2c_ansi_to_unicode( const char *ansi, wchar_t *unicode, size_t maxLen );
extern L2C_UTILS_API void l2c_unicode_to_ansi( const wchar_t *unicode, char *ansi, size_t maxLen );

extern L2C_UTILS_API void base64_encode_string( const char *str_in, int len_in, char *str_out );
extern L2C_UTILS_API void base64_decode_string( const char *str_in, int len_in, char *str_out );

//extern L2C_UTILS_API int nl2c_utils;

// other library includes

#include "l2c_configFile.h"
#include "l2c_db.h"
#include "l2c_lock.h"
#include "l2c_exception.h"
