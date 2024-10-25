// 
// Log.cpp: Implementation of the CLog class.
//
// Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h" // Comment out this line if not windows
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include "ErrCode.h" // Error Code
#include "BaseLog.h"

//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Global Variable
//////////////////////////////////////////////////////////////////////

// Debug
bool g_bEnableDebug = true;
bool g_bEnableOutputBufferDebug = true;
bool g_bEnableErrorMsg = true;

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

CBaseLog::CBaseLog(char *pszLogDirPath, char *pszLogFileName)
{
    struct stat file_stat;
    //printf("%s: pszLogDirPath=\"%s\", pszLogFileName=\"%s\".\r\n", __func__, pszLogDirPath, pszLogFileName);

    memset(m_szLogBuf, 0, sizeof(m_szLogBuf));
    memset(m_szDateBuffer, 0, sizeof(m_szDateBuffer));
    memset(m_szDateTimeBuffer, 0, sizeof(m_szDateTimeBuffer));
    memset(m_szDebugBuf, 0, sizeof(m_szDebugBuf));
    memset(m_szLogDirPath, 0, sizeof(m_szLogDirPath));
    memset(m_szLogFileName, 0, sizeof(m_szLogFileName));
    memset(m_szLogFilePath, 0, sizeof(m_szLogFilePath));

    // Initialize mutex
    sem_init(&m_semFileIoMutex,  0 /*scope is in this file*/, 1 /*active in initial*/);

    // File I/O Lock Counter to Make Sure File Access Once at Any Moment
    m_nFileIoLockCounter = 0;
    //printf("%s: m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    // Initialize Debug Directory Path
    if (pszLogDirPath != NULL)
        SetLogDirPath(pszLogDirPath);

    /* Initialize Log File Name & Path */
    if ((pszLogFileName == NULL) || (strcmp(pszLogFileName, "") == 0))
    {
        // Initialize Log File Name
        strcpy(m_szLogFileName, DEFAULT_LOG_FILE);

        // Update File Path
        if (strcmp(m_szLogDirPath, "") == 0) // Log Dir Path is Null
            sprintf(m_szLogFilePath, "%s", m_szLogFileName);
        else
            sprintf(m_szLogFilePath, "%s/%s", m_szLogDirPath, m_szLogFileName);

        // Clear Content of Log File
        if (stat(m_szLogFilePath, &file_stat) == 0)
        {
            remove(m_szLogFilePath);
        }
    }
    else // (pszLogFileName != NULL) && (strcmp(pszLogFileName, "") != 0)
    {
        SetLogFileName(pszLogFileName);
    }
    //printf("%s: DebugLogFileName=\"%s\", DebugLogFilePath=\"%s\".\r\n", __func__, m_szLogFileName, m_szLogFilePath);
}

CBaseLog::~CBaseLog(void)
{
    // Destroy mutex (semaphore)
    //DBG("Destroy mutext/semaphore (address=%p).", &m_semFileIoMutex);
    sem_destroy(&m_semFileIoMutex);
}

//////////////////////////////////////////////////////////////////////
// Function Implementation
//////////////////////////////////////////////////////////////////////

int CBaseLog::CreateDirPath(char *pszDirPath)
{
    int nRet = ERR_SUCCESS,
        nDirNameLen = 0,
        nIndex = 0,
        nDir = 0;
    char szDirName[PATH_LEN_MAX] = {0},
         szTempDirName[PATH_LEN_MAX+1] = {0}; // MAX(szDirName) + sizeof('\0')
    struct stat file_stat;

    // Make Sure Input Directory Path Valid
    if (pszDirPath == NULL)
    {
        //DBG("%s: Null Directory Path!", __func__);
        nRet = ERR_INVALID_PARAM;
        goto CREATE_DIR_PATH_EXIT;
    }

    // Make Sure Directory Not Exist
    if ((stat(pszDirPath, &file_stat) == 0) && (S_ISDIR(file_stat.st_mode)))
    {
        //DBG("%s: Directory \"%s\" Exists!", __func__, szDirPath);
        nRet = ERR_INVALID_PARAM;
        goto CREATE_DIR_PATH_EXIT;
    }

    strcpy(szDirName, pszDirPath);
    nDirNameLen = strlen(szDirName);
    if (szDirName[nDirNameLen - 1] != '/')
    {
        strcat(szDirName, "/");
        nDirNameLen++;
    }
    //DBG("%s: DirPath=\"%s\", DirName=\"%s\", DirNameLen=%d.", __func__, pszDirPath, szDirName, nDirNameLen);

    for (nIndex = 1 /* Skip root '/' */; nIndex < nDirNameLen; nIndex++)
    {
        if ((szDirName[nIndex] == '/') /*linux*/ || (szDirName[nIndex] == '\\') /* WIN32 */)
        {
            // Temp Directory Name
            memset(szTempDirName, 0, sizeof(szTempDirName));
            //strncpy(szTempDirName, szDirName, nIndex);
            memcpy(szTempDirName, szDirName, nIndex);
            szTempDirName[nIndex] = '\0';

            // Check if directory has existed
            nDir = access(szTempDirName, 0);
            if (nDir != 0) // not exist
            {
                // Create new directory
                //DBG("Create Directory %s.", szTempDirName);
                nDir = mkdir(szTempDirName, S_IRWXU | S_IRWXG | S_IRWXO);
                if (nDir == -1) // create fail
                {
                    //ERR("Fail to create directory \"%s\".", szTempDirName);
                    printf("%s: Fail to create directory \"%s\".\r\n", __func__, szTempDirName);
                    nRet = ERR_IO_ERROR;
                    break;
                }
            }
        }
    }

CREATE_DIR_PATH_EXIT:
    return nRet;
}

int CBaseLog::SetLogDirPath(char *pszDirPath)
{
    int nRet = ERR_SUCCESS;
    size_t nPathLen = 0;
    char szCurLogFilePath[PATH_LEN_MAX] = {0};
    struct stat file_stat;

    // Make Sure Dir Path Valid
    if (pszDirPath == NULL)
    {
        printf("%s: Invalid Log Dir Path! (pszDirPath=%p)\r\n", __func__, pszDirPath);
        nRet = ERR_INVALID_PARAM;
        goto SET_LOG_DIR_PATH_EXIT;
    }

    // Make Sure If Dir Path Exist
    if (strcmp(pszDirPath, "") == 0)
    {
        //printf("%s: Null Log Dir Path!\r\n", __func__);
        goto SET_LOG_DIR_PATH_EXIT;
    }

    // Make Sure If Dir Path Changed
    if (strcmp(m_szLogDirPath, pszDirPath) == 0)
    {
        //printf("%s: The Same Dir Path as Orginal One!\r\n", __func__);
        goto SET_LOG_DIR_PATH_EXIT;
    }

    // Update Dir Path
    memset(m_szLogDirPath, 0, sizeof(m_szLogDirPath));
    //sprintf(m_szLogDirPath, "%s", pszDirPath);
    strcpy(m_szLogDirPath, pszDirPath);
    //printf("%s: LogDirPath=\"%s\".\r\n", __func__, m_szLogDirPath);

    // Remove the last '/' or '\'
    nPathLen = strlen(m_szLogDirPath);
    //printf("%s: LogDirPath[%zd]=\'%c\'.\r\n", __func__, nPathLen-1, m_szLogDirPath[nPathLen-1]);
    if ((m_szLogDirPath[nPathLen - 1] == '/') /* linux */ || (m_szLogDirPath[nPathLen - 1] == '\\' /* WIN32 */))
    {
        //printf("%s: Remove the last token '/' or '\' from path of log directory.", __func__);
        m_szLogDirPath[nPathLen - 1] = '\0';
        //printf("%s: LogDirPath = \"%s\".\r\n", __func__, m_szLogDirPath);
    }

    // Create Dir Path
    CreateDirPath(m_szLogDirPath);

    // Move Log File If Exist
    if (strcmp(m_szLogFileName, "") != 0)
    {
        // Backup Original Log File Path
        strcpy(szCurLogFilePath, m_szLogFilePath);

        // Update Log File Path
        memset(m_szLogFilePath, 0, sizeof(m_szLogFilePath));
        sprintf(m_szLogFilePath, "%s/%s", m_szLogDirPath, m_szLogFileName);
        printf("%s: DebugLogFileName=\"%s\", DebugLogFilePath=\"%s\".\r\n", __func__, \
               m_szLogFileName, m_szLogFilePath);

        // Move Log File If Exist
        if (stat(szCurLogFilePath, &file_stat) == 0)
        {
            // Mutex locks the critical section_
            sem_wait(&m_semFileIoMutex);

            // Make Sure File Unlocked
            //printf("%s: [0] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);
            assert(m_nFileIoLockCounter == 0);

            // Lock File
            m_nFileIoLockCounter++;
            //printf("%s: [1] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

            // Move Log File
            //printf("%s: Move file \"%s\" to \"%s\".\r\n", __func__, szCurLogFilePath, m_szLogFilePath);
            rename(szCurLogFilePath, m_szLogFilePath);

            // Unlock File
            m_nFileIoLockCounter--;
            //printf("%s: [2] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

            // Mutex unlocks the critical section
            sem_post(&m_semFileIoMutex);
        }
    }

SET_LOG_DIR_PATH_EXIT:
    return nRet;
}

int CBaseLog::SetLogFileName(char *pszFileName)
{
    int nRet = ERR_SUCCESS;
    char szCurLogFilePath[PATH_LEN_MAX] = {0};
    struct stat file_stat;
    //printf("%s: pszFileName=\"%s\".\r\n", __func__, pszFileName);

    // Make Sure File Name Valid
    if (pszFileName == NULL)
    {
        printf("%s: No Input File Name!\r\n", __func__);
        nRet = ERR_INVALID_PARAM;
        goto SET_LOG_FILE_NAME_EXIT;
    }

    // Make Sure If Log File Name Exist
    if (strcmp(pszFileName, "") == 0)
    {
        //printf("%s: Null Log File Name!\r\n", __func__);
        goto SET_LOG_FILE_NAME_EXIT;
    }

    // Make Sure Log File Change
    if (strcmp(m_szLogFileName, pszFileName) == 0)
    {
        printf("%s: Log File Has Exist!\r\n", __func__);
        goto SET_LOG_FILE_NAME_EXIT;
    }

    // Backup Original Log File Path
    strcpy(szCurLogFilePath, m_szLogFilePath);

    // Update File Name
    memset(m_szLogFileName, 0, sizeof(m_szLogFileName));
    sprintf(m_szLogFileName, "%s", pszFileName);
    //printf("%s: DebugLogFileName=\"%s\".\r\n", __func__, m_szLogFileName);

    // Update File Path
    memset(m_szLogFilePath, 0, sizeof(m_szLogFilePath));
    if (strcmp(m_szLogDirPath, "") == 0) // Log Dir Path is Null
        sprintf(m_szLogFilePath, "%s", m_szLogFileName);
    else
        sprintf(m_szLogFilePath, "%s/%s", m_szLogDirPath, m_szLogFileName);
    //printf("%s: DebugLogFileName=\"%s\", DebugLogFilePath=\"%s\".\r\n", __func__, m_szLogFileName, m_szLogFilePath);

    // Move Log File If Exist
    if (stat(szCurLogFilePath, &file_stat) == 0)
    {
        // Mutex locks the critical section
        sem_wait(&m_semFileIoMutex);

        // Make Sure File Unlocked
        //printf("%s: [0] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);
        assert(m_nFileIoLockCounter == 0);

        // Lock File
        m_nFileIoLockCounter++;
        //printf("%s: [1] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

        // Move Log File
        //printf("%s: Move file \"%s\" to \"%s\".\r\n", __func__, szCurLogFilePath, m_szLogFilePath);
        rename(szCurLogFilePath, m_szLogFilePath);

        // Unlock File
        m_nFileIoLockCounter--;
        //printf("%s: [2] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

        // Mutex unlocks the critical section
        sem_post(&m_semFileIoMutex);
    }

SET_LOG_FILE_NAME_EXIT:
    return nRet;
}

void CBaseLog::DebugLog(char *pszLog)
{
    FILE *fd = NULL;

    // Make Sure Log String Valid
    if (pszLog == NULL)
        goto DEBUG_LOG_EXIT;

    // Mutex locks the critical section
    sem_wait(&m_semFileIoMutex);

    // Make Sure File Unlocked
    //printf("%s: [0] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);
    assert(m_nFileIoLockCounter == 0);

    // Lock File
    m_nFileIoLockCounter++;
    //printf("%s: [1] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    fd = fopen(m_szLogFilePath, "a+");
    if (fd == NULL)
    {
        printf("%s: Fail to open \"%s\"! (errno=%d)\r\n", __func__, m_szLogFilePath, errno);
        goto DEBUG_LOG_EXIT_1;
    }

    // Clear Buffer
    memset(m_szDateBuffer, 0, sizeof(m_szDateBuffer));
    memset(m_szDateTimeBuffer, 0, sizeof(m_szDateTimeBuffer));

    // Get Current Time
    time(&m_tRawTime);
    strftime(m_szDateBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&m_tRawTime)); //"2022-12-31 12:59:59"
    gettimeofday(&m_tvCurTime, NULL);
    sprintf(m_szDateTimeBuffer, "%s:%03d:%03d", m_szDateBuffer, (int)m_tvCurTime.tv_usec / 1000, (int)m_tvCurTime.tv_usec % 1000); // "${80}:500:500"

    //Write data to file
    fprintf(fd, "%s [DEBUG] %s\n", m_szDateTimeBuffer, pszLog);

    fclose(fd);

DEBUG_LOG_EXIT_1:

    // Unlock File
    m_nFileIoLockCounter--;
    //printf("%s: [2] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    // Mutex unlocks the critical section
    sem_post(&m_semFileIoMutex);

DEBUG_LOG_EXIT:
    return;
};

void CBaseLog::DebugLogFormat(const char *pszFormat, ...)
{
    FILE *fd = NULL;
    char szLogBuffer[LOG_BUF_SIZE] = {0};
    va_list pArgs;

    // Make Sure Log String Valid
    if (pszFormat == NULL)
        goto DEBUG_LOG_FORMAT_EXIT;

    // Mutex locks the critical section
    sem_wait(&m_semFileIoMutex);

    // Make Sure File Unlocked
    //printf("%s: [0] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);
    assert(m_nFileIoLockCounter == 0);

    // Lock File
    m_nFileIoLockCounter++;
    //printf("%s: [1] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    // Make Sure Log File Exist
    fd = fopen(m_szLogFilePath, "a+");
    if (fd == NULL)
    {
        printf("%s: Fail to open \"%s\"! (errno=%d)\r\n", __func__, m_szLogFilePath, errno);
        goto DEBUG_LOG_FORMAT_EXIT_1;
    }

    // Clear Buffer
    memset(m_szDateBuffer, 0, sizeof(m_szDateBuffer));
    memset(m_szDateTimeBuffer, 0, sizeof(m_szDateTimeBuffer));

    // Get Current Time
    time(&m_tRawTime);
    strftime(m_szDateBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&m_tRawTime));
    gettimeofday(&m_tvCurTime, NULL);
    sprintf(m_szDateTimeBuffer, "%s:%03d:%03d", m_szDateBuffer, (int)m_tvCurTime.tv_usec / 1000, (int)m_tvCurTime.tv_usec % 1000);

    // Load String to LogBuffer with Variable Argument List
    va_start(pArgs, pszFormat);
    vsnprintf(szLogBuffer, sizeof(szLogBuffer), pszFormat, pArgs);
    va_end(pArgs);

    //Write data to file
    fprintf(fd, "%s [DEBUG] %s\n", m_szDateTimeBuffer, szLogBuffer);

    fclose(fd);

DEBUG_LOG_FORMAT_EXIT_1:

    // Unlock File
    m_nFileIoLockCounter--;
    //printf("%s: [2] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    // Mutex unlocks the critical section
    sem_post(&m_semFileIoMutex);

DEBUG_LOG_FORMAT_EXIT:
    return;
}

void CBaseLog::ErrorLog(char *pszLog)
{
    FILE *fd = NULL;

    // Make Sure Log String Valid
    if (pszLog == NULL)
        goto ERROR_LOG_EXIT;

    // Mutex locks the critical section
    sem_wait(&m_semFileIoMutex);

    // Make Sure File Unlocked
    //printf("%s: [0] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);
    assert(m_nFileIoLockCounter == 0);

    // Lock File
    m_nFileIoLockCounter++;
    //printf("%s: [1] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    fd = fopen(m_szLogFilePath, "a+");
    if (fd == NULL)
    {
        printf("%s: Fail to open \"%s\"! (errno=%d)\r\n", __func__, m_szLogFilePath, errno);
        goto ERROR_LOG_EXIT_1;
    }

    // Clear Buffer
    memset(m_szDateBuffer, 0, sizeof(m_szDateBuffer));
    memset(m_szDateTimeBuffer, 0, sizeof(m_szDateTimeBuffer));

    // Get Current Time
    time(&m_tRawTime);
    strftime(m_szDateBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&m_tRawTime));
    gettimeofday(&m_tvCurTime, NULL);
    sprintf(m_szDateTimeBuffer, "%s:%03d:%03d", m_szDateBuffer, (int)m_tvCurTime.tv_usec / 1000, (int)m_tvCurTime.tv_usec % 1000);

    //Write data to file
    fprintf(fd, "%s [ERROR] %s\n", m_szDateTimeBuffer, pszLog);

    fclose(fd);

ERROR_LOG_EXIT_1:

    // Unlock File
    m_nFileIoLockCounter--;
    //printf("%s: [2] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    // Mutex unlocks the critical section
    sem_post(&m_semFileIoMutex);

ERROR_LOG_EXIT:
    return;
};

void CBaseLog::ErrorLogFormat(const char *pszFormat, ...)
{
    FILE *fd = NULL;
    char szLogBuffer[LOG_BUF_SIZE] = {0};
    va_list pArgs;

    // Make Sure Log String Valid
    if (pszFormat == NULL)
        goto ERROR_LOG_FORMAT_EXIT;

    // Mutex locks the critical section
    sem_wait(&m_semFileIoMutex);

    // Make Sure File Unlocked
    //printf("%s: [0] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);
    assert(m_nFileIoLockCounter == 0);

    // Lock File
    m_nFileIoLockCounter++;
    //printf("%s: [1] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    fd = fopen(m_szLogFilePath, "a+");
    if (fd == NULL)
    {
        printf("%s: Fail to open \"%s\"! (errno=%d)\r\n", __func__, m_szLogFilePath, errno);
        goto ERROR_LOG_FORMAT_EXIT_1;
    }

    // Clear Buffer
    memset(m_szDateBuffer, 0, sizeof(m_szDateBuffer));
    memset(m_szDateTimeBuffer, 0, sizeof(m_szDateTimeBuffer));

    // Get Current Time
    time(&m_tRawTime);
    strftime(m_szDateBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&m_tRawTime));
    gettimeofday(&m_tvCurTime, NULL);
    sprintf(m_szDateTimeBuffer, "%s:%03d:%03d", m_szDateBuffer, (int)m_tvCurTime.tv_usec / 1000, (int)m_tvCurTime.tv_usec % 1000);

    // Load String to LogBuffer with Variable Argument List
    va_start(pArgs, pszFormat);
    vsnprintf(szLogBuffer, sizeof(szLogBuffer), pszFormat, pArgs);
    va_end(pArgs);

    //Write data to file
    fprintf(fd, "%s [ERROR] %s\n", m_szDateTimeBuffer, szLogBuffer);

    fclose(fd);

ERROR_LOG_FORMAT_EXIT_1:

    // Unlock File
    m_nFileIoLockCounter--;
    //printf("%s: [2] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    // Mutex unlocks the critical section
    sem_post(&m_semFileIoMutex);

ERROR_LOG_FORMAT_EXIT:
    return;
}

void CBaseLog::DebugPrintBuffer(unsigned char *pbyBuf, int nLen)
{
    int nIndex = 0;
    unsigned char *pbyData = NULL;
    char szBuffer[8] = { 0 };
    FILE *fd = NULL;

    // Validate Input Parameters
    if((pbyBuf == NULL) || (nLen == 0) || (nLen > LOG_BUF_DATA_SIZE))
    {
        printf("%s: Invalid Input Parameter! (pbyBuf=0x%p, nLen=%d)\r\n", __func__, pbyBuf, nLen);
        goto DEBUG_PRINT_BUFFER_EXIT;
    }

    // Mutex locks the critical section
    sem_wait(&m_semFileIoMutex);

    // Make Sure All Locks Unlocked
    //printf("%s: [0] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);
    assert(m_nFileIoLockCounter == 0);

    // Lock File
    m_nFileIoLockCounter++;
    //printf("%s: [1] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    fd = fopen(m_szLogFilePath, "a+");
    if (fd == NULL)
    {
        printf("%s: Fail to open file \"%s\"! (errno=%d)\r\n", __func__, m_szLogFilePath, errno);
        goto DEBUG_PRINT_BUFFER_EXIT_1;
    }

    // Set data to buffer
    memset(m_szDebugBuf, 0, sizeof(m_szDebugBuf));
    for (nIndex = 0, pbyData = pbyBuf; nIndex < nLen; nIndex++, pbyData++)
    {
        memset(szBuffer, 0, sizeof(szBuffer));
        sprintf(szBuffer, " %02x", *pbyData);
        strcat(m_szDebugBuf, szBuffer);
    }

    // Clear Buffer
    memset(m_szDateBuffer, 0, sizeof(m_szDateBuffer));
    memset(m_szDateTimeBuffer, 0, sizeof(m_szDateTimeBuffer));

    // Get Current Time
    time(&m_tRawTime);
    strftime(m_szDateBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&m_tRawTime));
    gettimeofday(&m_tvCurTime, NULL);
    sprintf(m_szDateTimeBuffer, "%s:%03d:%03d", m_szDateBuffer, (int)m_tvCurTime.tv_usec / 1000, (int)m_tvCurTime.tv_usec % 1000);

    // Write buffer to file
    fprintf(fd, "%s [DEBUG] buffer[%d]=%s.\n", m_szDateTimeBuffer, nIndex, m_szDebugBuf);

    fclose(fd);

DEBUG_PRINT_BUFFER_EXIT_1:

    // Unlock File
    m_nFileIoLockCounter--;
    //printf("%s: [2] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    // Mutex unlocks the critical section
    sem_post(&m_semFileIoMutex);

DEBUG_PRINT_BUFFER_EXIT:
    return;
}

void CBaseLog::DebugPrintBuffer(const char *pszBufName, unsigned char *pbyBuf, int nLen)
{
    int nIndex = 0;
    unsigned char *pbyData = NULL;
    char szBuffer[8] = { 0 };
    FILE *fd = NULL;

    // Validate Input Parameters
    if((pszBufName == NULL) || (pbyBuf == NULL) || (nLen == 0) || (nLen > LOG_BUF_DATA_SIZE))
    {
        printf("%s: Invalid Input Parameter! (pszBufName=0x%p, pbyBuf=0x%p, nLen=%d)\r\n", __func__, pszBufName, pbyBuf, nLen);
        goto DEBUG_PRINT_BUFFER_2_EXIT;
    }

    // Mutex locks the critical section
    sem_wait(&m_semFileIoMutex);

    // Make Sure All Locks Unlocked
    //printf("%s: [0] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);
    assert(m_nFileIoLockCounter == 0);

    // Lock File
    m_nFileIoLockCounter++;
    //printf("%s: [1] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    fd = fopen(m_szLogFilePath, "a+");
    if (fd == NULL)
    {
        printf("%s: Fail to open file \"%s\"! (errno=%d)\r\n", __func__, m_szLogFilePath, errno);
        goto DEBUG_PRINT_BUFFER_2_EXIT_1;
    }

    // Set data to buffer
    memset(m_szDebugBuf, 0, sizeof(m_szDebugBuf));
    for (nIndex = 0, pbyData = pbyBuf; nIndex < nLen; nIndex++, pbyData++)
    {
        memset(szBuffer, 0, sizeof(szBuffer));
        sprintf(szBuffer, " %02x", *pbyData);
        strcat(m_szDebugBuf, szBuffer);
    }

    // Clear Buffer
    memset(m_szDateBuffer, 0, sizeof(m_szDateBuffer));
    memset(m_szDateTimeBuffer, 0, sizeof(m_szDateTimeBuffer));

    // Get Current Time
    time(&m_tRawTime);
    gettimeofday(&m_tvCurTime, NULL);
    strftime(m_szDateBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&m_tRawTime));
    sprintf(m_szDateTimeBuffer, "%s:%03d:%03d", m_szDateBuffer, (int)m_tvCurTime.tv_usec / 1000, (int)m_tvCurTime.tv_usec % 1000);

    // Write buffer to file
    fprintf(fd, "%s [DEBUG] %s[%d]=%s.\n", m_szDateTimeBuffer, pszBufName, nIndex, m_szDebugBuf);

    fclose(fd);

DEBUG_PRINT_BUFFER_2_EXIT_1:

    // Unlock File
    m_nFileIoLockCounter--;
    //printf("%s: [2] m_nFileIoLockCounter=%d.\r\n", __func__, m_nFileIoLockCounter);

    // Mutex unlocks the critical section
    sem_post(&m_semFileIoMutex);

DEBUG_PRINT_BUFFER_2_EXIT:
    return;
}

