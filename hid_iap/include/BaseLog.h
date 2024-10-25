//
// BaseLog.h: Header of CBaseLog class, for logging message to log file.
//
// Date: 2024/03/05
//
// Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef __BASE_LOG_H__
#define __BASE_LOG_H__
#pragma once

#include <stdio.h>
#include <string.h>
#include <time.h> //<ctime>_
#include <sys/time.h> // struct timeval & gettimeofday()
#include <semaphore.h>	/* semaphore */

//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

#ifndef DATE_BUF_SIZE
#define DATE_BUF_SIZE                  80
#endif //DATE_BUF_SIZE

#ifndef DATE_TIME_BUF_SIZE
#define DATE_TIME_BUF_SIZE             128
#endif //DATE_TIME_BUF_SIZE

#ifndef LOG_BUF_DATA_SIZE
#define LOG_BUF_DATA_SIZE              8192
#endif //LOG_BUF_DATA_SIZE

#ifndef LOG_BUF_SIZE
#define LOG_BUF_SIZE                   (DATE_TIME_BUF_SIZE + (LOG_BUF_DATA_SIZE * 3)) // 3 = ${data_byte} + ' ', ex: " FF" = ' ' + 'F' + 'F'
#endif //LOG_BUF_SIZE

#ifndef PATH_LEN_MAX
#define PATH_LEN_MAX                   512
#endif //PATH_LEN_MAX

#ifndef DEFAULT_LOG_DIR
#define DEFAULT_LOG_DIR                "/tmp"
#endif //DEFAULT_LOG_DIR

#ifndef DEFAULT_LOG_FILE
#define DEFAULT_LOG_FILE               "elants_hid_iap_log.txt"
#endif // DEFAULT_LOG_FILE

//////////////////////////////////////////////////////////////////////
// Global Variable
//////////////////////////////////////////////////////////////////////

extern bool g_bEnableDebug;
extern bool g_bEnableOutputBufferDebug;
extern bool g_bEnableErrorMsg;

//////////////////////////////////////////////////////////////////////
// Macro
//////////////////////////////////////////////////////////////////////

#define DEBUG(format, args...) if(g_bEnableDebug)    DebugLogFormat(format, ##args)
#define   DBG(format, args...) if(g_bEnableDebug)    DebugLogFormat(format, ##args)

#define ERROR(format, args...) \
do{\
   printf("[ERROR] " format "\r\n", ##args); \
   if(g_bEnableErrorMsg) \
      ErrorLogFormat(format, ##args); \
}while(0)

#define ERR(format, args...) \
do{\
   printf("[ERR] " format "\r\n", ##args); \
   if(g_bEnableErrorMsg) \
      ErrorLogFormat(format, ##args); \
}while(0)

#define INFO(format, args...) \
do{\
   printf("[INFO] " format "\r\n", ##args); \
   if(g_bEnableDebug) \
      DebugLogFormat(format, ##args); \
}while(0)

//////////////////////////////////////////////////////////////////////
// Prototype
//////////////////////////////////////////////////////////////////////

class CBaseLog
{
public:
    // Constructor / Deconstructor
    //CBaseLog(void);
    CBaseLog(char *pszLogDirPath = (char *)DEFAULT_LOG_DIR, char *pszLogFileName = (char *)DEFAULT_LOG_FILE);
    ~CBaseLog(void);

    // Log Functions
    void DebugLog(char *pszLog);
    void ErrorLog(char *pszLog);
    void DebugLogFormat(const char *pszFormat, ...);
    void ErrorLogFormat(const char *pszFormat, ...);
    void DebugPrintBuffer(unsigned char *pbyBuf, int nLen);
    void DebugPrintBuffer(const char *pszBufName, unsigned char *pbyBuf, int nLen);

    // Log Path
    int CreateDirPath(char *pszDirPath);
    int SetLogDirPath(char *pszDirPath);
    int SetLogFileName(char *pszFileName);

    // Data Member
    time_t m_tRawTime;
    struct timeval m_tvCurTime;
    char m_szDateBuffer[DATE_BUF_SIZE];
    char m_szDateTimeBuffer[DATE_TIME_BUF_SIZE];
    char m_szLogBuf[LOG_BUF_SIZE];
    char m_szDebugBuf[LOG_BUF_SIZE];

    // Log Path
    char m_szLogDirPath[PATH_LEN_MAX];
    char m_szLogFileName[PATH_LEN_MAX];
    char m_szLogFilePath[PATH_LEN_MAX*2+2]; // MAX(LogDirPathLen) + MAX(DebugLogFileNameLen) + sizeof('/') or sizeof('\\')

protected:
    sem_t m_semFileIoMutex;
    int m_nFileIoLockCounter;
}; //CBaseLog

#endif //ndef __BASE_LOG_H__
