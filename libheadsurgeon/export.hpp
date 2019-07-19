#pragma once

#if defined(LIBHEADSURGEON_STATIC) // Using static.
#define LIBHEADSURGEON_SYMEXPORT
#elif defined(LIBHEADSURGEON_STATIC_BUILD) // Building static.
#define LIBHEADSURGEON_SYMEXPORT
#elif defined(LIBHEADSURGEON_SHARED) // Using shared.
#ifdef _WIN32
#define LIBHEADSURGEON_SYMEXPORT __declspec(dllimport)
#else
#define LIBHEADSURGEON_SYMEXPORT
#endif
#elif defined(LIBHEADSURGEON_SHARED_BUILD) // Building shared.
#ifdef _WIN32
#define LIBHEADSURGEON_SYMEXPORT __declspec(dllexport)
#else
#define LIBHEADSURGEON_SYMEXPORT
#endif
#else
// If none of the above macros are defined, then we assume we are being used
// by some third-party build system that cannot/doesn't signal the library
// type. Note that this fallback works for both static and shared but in case
// of shared will be sub-optimal compared to having dllimport.
//
#define LIBHEADSURGEON_SYMEXPORT // Using static or shared.
#endif
