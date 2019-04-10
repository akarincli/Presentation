#pragma once

#if defined(__WIN32__) || defined(__WINRT__)
#  ifdef __BORLANDC__
#    define ____DECLSPEC
#  else
#    define ____DECLSPEC __declspec(dllexport)
#  endif
#elif defined(__OS2__)
#  define ____DECLSPEC __declspec(dllexport)
#else
#  if defined(__GNUC__) && __GNUC__>=4
#    define ____DECLSPEC __attribute__ ((visibility("default")))
#  else
#    define ____DECLSPEC
#  endif
#endif

#define AK_PUBLIC extern "C" ____DECLSPEC

#if (defined(__WIN32__) || defined(__WINRT__)) && !defined(__GNUC__)
#  define AK_CALL __cdecl
#elif defined(__OS2__) || defined(__EMX__)
#  define AK_CALL _System
#  if defined (__GNUC__) && !defined(_System)
#   define _System /* for old EMX/GCC compat.  */
#  endif
#else
#  define AK_CALL
#endif

constexpr int VersionMajor = 0;
constexpr int VersionMinor = 0;
constexpr int VersionRevision = 1;
