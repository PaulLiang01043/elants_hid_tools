#ifndef __BUILD_CONFIG_H__
#define __BUILD_CONFIG_H__
#pragma once

#include <sys/time.h>     // gettimeofday
#include <semaphore.h>    // semaphore
#include <unistd.h>       // usleep

#define SleepMS( ms )     usleep( (ms) * 1000)

// Max Length of Full Path Name
#ifndef MAX_PATH
#define MAX_PATH          260
#endif //MAX_PATH

// Type Re-define
typedef unsigned char     byte;
typedef unsigned short    word;

// WIN32 Definition
typedef void*             HWND;
#endif // __BUILD_CONFIG_H__
