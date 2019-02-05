#define TFB_CURRENT_VERSION 5

#ifndef _IVW_MODULE_TENSORVISBASE_DEFINE_H_
#define _IVW_MODULE_TENSORVISBASE_DEFINE_H_

#ifdef INVIWO_ALL_DYN_LINK //DYNAMIC
// If we are building DLL files we must declare dllexport/dllimport
#ifdef IVW_MODULE_TENSORVISBASE_EXPORTS
#ifdef _WIN32
#define IVW_MODULE_TENSORVISBASE_API __declspec(dllexport)
#else //UNIX (GCC)
#define IVW_MODULE_TENSORVISBASE_API __attribute__ ((visibility ("default")))
#endif
#else
#ifdef _WIN32
#define IVW_MODULE_TENSORVISBASE_API __declspec(dllimport)
#else
#define IVW_MODULE_TENSORVISBASE_API
#endif
#endif
#else //STATIC
#define IVW_MODULE_TENSORVISBASE_API
#endif

#endif /* _IVW_MODULE_TENSORVISBASE_DEFINE_H_ */