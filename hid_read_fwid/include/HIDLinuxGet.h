// HIDLinuxGet.h: Declaration of CHIDLinuxGet Class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HID_LINUX_GET_H__
#define __HID_LINUX_GET_H__
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <semaphore.h>     /* semaphore */
#include <sys/select.h>    /* select    */
#include <sys/time.h>      /* timeval   */
#include <errno.h>         /* errno     */
#include "InterfaceGet.h"
#include "BaseLog.h"
#include "HidConfig.h"

//////////////////////////////////////////////////////////////////////
// Version of Interface Implementation
//////////////////////////////////////////////////////////////////////
#ifndef HID_LINUX_INTF_IMPL_VER
#define HID_LINUX_INTF_IMPL_VER	"HIDLinuxGet Version : 0.0.0.1"
#endif //HID_LINUX_INTF_IMPL_VER

//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet Class

class CHIDLinuxGet: public CInterfaceGet, public CBaseLog
{
public:
    // Constructor / Deconstructor
    CHIDLinuxGet(char *pszLogDirPath = (char *)DEFAULT_LOG_DIR, char *pszLogFileName = (char *)DEFAULT_LOG_FILE);
    ~CHIDLinuxGet(void);

    // Interface Info.
    int GetInterfaceType(void);
    const char* GetInterfaceVersion(void);

    // Basic Functions
    int GetDeviceHandle(int nVID, int nPID);
    void Close(void);
    bool IsConnected(void);

    // TP Command / Data Access Functions
    int WriteCommand(unsigned char* pszCommandBuf, int nCommandLen, int nTimeout = ELAN_WRITE_DATA_TIMEOUT_MSEC, int nDevIdx = 0);
    int ReadData(unsigned char* pszDataBuf, int nDataLen, int nTimeout = ELAN_READ_DATA_TIMEOUT_MSEC, int nDevIdx = 0, bool bFilter = true);

    // Raw Data Access Functions
    int WriteRawBytes(unsigned char* pszBuf, int nLen, int nTimeout = ELAN_WRITE_DATA_TIMEOUT_MSEC, int nDevIdx = 0);
    int ReadRawBytes(unsigned char* pszBuf, int nLen, int nTimeout = ELAN_READ_DATA_TIMEOUT_MSEC, int nDevIdx = 0);

    // Buffer Size Info.
    int GetInBufferSize(void);
    int GetOutBufferSize(void);

    // PID
    int GetDevVidPid(unsigned int* p_nVid, unsigned int* p_nPid, int nDevIdx = 0);

    // Bus Type
    int GetDevBusType(unsigned int* p_uiBusType, int nDevIdx = 0);

protected:
    // Find HIDRaw Device
    int FindHidrawDevice(int nVID, int nPID, char *pszDevicePath, size_t nDevicePathBufLen);

    // Bus Info.
    const char* bus_str(int bus);
    unsigned int m_uiBusType;

    // Time Info.
    struct timeval m_tvRead;

    // HID Info.
    unsigned short m_usVID;        // Vendor ID
    unsigned short m_usPID;        // Product ID
    unsigned short m_usVersion;    // HID Version

    // HIDRaw Device
    int m_nHidrawFd;
    fd_set m_fdsHidraw;

    // I/O Mutex
    sem_t m_ioMutex;

    // Command/Data Buffer
    unsigned char m_szOutputBuf[ELAN_HID_OUTPUT_BUFFER_SIZE]; // Command Raw Buffer
    unsigned char m_szInputBuf[ELAN_HID_INPUT_BUFFER_SIZE];   // Data Raw Buffer

    // I/O Buffer
    unsigned char *m_inBuf;
    unsigned char *m_outBuf;
    unsigned int m_inBufSize;
    unsigned int m_outBufSize;
};
#endif //__HID_LINUX_GET_H__
