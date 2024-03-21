#ifndef __INTERFACE_GET_H__
#define __INTERFACE_GET_H__
#pragma once

/**************************************************************************
* IntefaceGet.h
* Base Class of All-Interface Class.
* This class contains of 8 virtual methods needed to be implemented:
*
*	int GetDeviceHandle(void);
*	int GetDeviceHandle(int nVID, int nPID);
*	void Close(void);
*	bool IsConnected(void);
*
*	int WriteData(unsigned char* cBuf, size_t nLen);
*	int ReadData(unsigned char* cBuf, size_t nLen);
*	int ReadData(unsigned char* cBuf, size_t nLen, int nTimeout);
*
*	int GetVersion(void);
*	int GetPID(void);
*
* Date: 2013/06/26
**************************************************************************/
#include <cstdio>
#include "BuildConfig.h"
#include "ErrCode.h"

//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

// Interface Type
#ifndef INTF_TYPE_HID_LINUX
#define INTF_TYPE_HID_LINUX                   4
#endif//INTF_TYPE_HID_LINUX

#ifndef INTF_TYPE_I2CHID_LINUX
#define INTF_TYPE_I2CHID_LINUX                ( INTF_TYPE_HID_LINUX )
#endif //INTF_TYPE_I2CHID_LINUX

// Timeout Setting
#ifndef ELAN_READ_DATA_TIMEOUT_MSEC
#define ELAN_READ_DATA_TIMEOUT_MSEC           1000
#endif //ELAN_READ_DATA_TIMEOUT_MSEC

#ifndef ELAN_WRITE_DATA_TIMEOUT_MSEC
#define ELAN_WRITE_DATA_TIMEOUT_MSEC          1000
#endif //ELAN_WRITE_DATA_TIMEOUT_MSEC

//////////////////////////////////////////////////////////////////////
// Declaration of Data Structure
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Prototype
//////////////////////////////////////////////////////////////////////

class CInterfaceGet
{
public:
    // Constructor / Deconstructor
    CInterfaceGet(void) {};
    virtual ~CInterfaceGet(void) {};

    // Interface Info.
    virtual int GetInterfaceType(void) = 0;
    virtual const char* GetInterfaceVersion(void) = 0;

    // Basic Functions
    //virtual int GetDeviceHandle(void) = 0; // No Longer In Use
    /*******************************************************
     * GetDeviceHandle: Connect Device & Get Handle
     * nParam1: DrvInterface when I2C, VID when HID/I2CHID
     * nParam2: BusID when I2C, PID when HID/I2CHID
     *******************************************************/
    virtual int GetDeviceHandle(int nParam1, int nParam2) = 0;
    virtual void Close(void) = 0;
    virtual bool IsConnected(void) = 0;

    //
    // TP Command / Data Access Functions in Main Code
    //
    // pszCommandBuf only contains the command data which firmware can recognize.
    // Report ID or brige command is not includes.
    virtual int WriteCommand(unsigned char* pszCommandBuf, int nBufLen, int nTimeoutMS, int nDevIdx) = 0;
    virtual int ReadData(unsigned char* pszDataBuf, int nBufLen, int nTimeoutMS, int nDevIdx, bool bFilter) = 0;

    //
    // Raw Data Access Functions
    // Read and Write RAW data support multiple devices
    //
    virtual int WriteRawBytes(unsigned char* pszBuf, int nBufLen, int nTimeoutMS, int nDevIdx) = 0;
    virtual int ReadRawBytes(unsigned char* pszBuf, int nBufLen, int nTimeoutMS, int nDevIdx) = 0;

    // Buffer Size Info.
    virtual int GetInBufferSize(void) { return 0; }
    virtual int GetOutBufferSize(void) { return 0; }

    //
    // For multiple device on one pc, nDevIdx will specify the target device.
    // Only windows platform support the following function.
    //
    virtual int GetDevCount(void) { return 1; }
    virtual int GetDevVidPid(unsigned int* p_nVid, unsigned int* p_nPid, int nDevIdx) { return ERR_FUNC_NOT_SUPPORT; }

protected:

};
#endif //__INTERFACE_GET_H__
