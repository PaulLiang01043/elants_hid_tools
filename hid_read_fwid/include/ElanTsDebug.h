// ElanTsDebug.h: Definition Header of Elan Touchscreen Debug Utility.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ELAN_TS_DEBUG_H__
#define __ELAN_TS_DEBUG_H__
#pragma once

//////////////////////////////////////////////////////////////////////
// Global Variable
//////////////////////////////////////////////////////////////////////

// Debug
extern bool g_debug;

//////////////////////////////////////////////////////////////////////
// Macro
//////////////////////////////////////////////////////////////////////

// Print Function of Debug Message
#ifndef DEBUG_PRINTF
#define DEBUG_PRINTF(fmt, argv...) \
do{ \
    if(g_debug) printf(fmt, ##argv); \
}while(0)
#endif //DEBUG_PRINTF

// Print Function of Error Message
#ifndef ERROR_PRINTF
#define ERROR_PRINTF(fmt, argv...) \
do{ \
    fprintf(stderr, fmt, ##argv); \
}while(0)
#endif //ERROR_PRINTF

#endif //__ELAN_TS_DEBUG_H__
