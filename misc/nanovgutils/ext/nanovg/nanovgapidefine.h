#ifndef _NANOVG_DEFINE_H_
#define _NANOVG_DEFINE_H_

#ifdef INVIWO_ALL_DYN_LINK //DYNAMIC
// If we are building DLL files we must declare dllexport/dllimport
#ifdef NANOVG_EXPORTS
#ifdef _WIN32
#define NANOVG_API __declspec(dllexport)
#else //UNIX (GCC)
#define NANOVG_API __attribute__((visibility ("default")))
#endif
#else
#ifdef _WIN32
#define NANOVG_API __declspec(dllimport)
#else
#define NANOVG_API
#endif
#endif
#else //STATIC
#define NANOVG_API
#endif

#endif /* _NANOVG_DEFINE_H_ */
