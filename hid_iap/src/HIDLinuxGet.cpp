//
// HIDLinuxGet.cpp : Implementation of CHIDLinuxGet Class.
//
// Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include <fcntl.h>        /* open */
#include <unistd.h>       /* close */
#include <sys/ioctl.h>    /* ioctl */
#include <dirent.h>       // opendir, readdir, closedir
#include <linux/hidraw.h> // hidraw
#include <linux/input.h>  // BUS_TYPE
#include <errno.h>        // errno
#include "HIDLinuxGet.h"

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::CHIDLinuxGet()
// 1. Set Initial Value to Member Variables
// 2. Allocate Memeroy to Resource Needed
// 3. Initialize mutex (semaphore)
// 4. Initialize libusb

CHIDLinuxGet::CHIDLinuxGet(char *pszLogDirPath, char *pszLogFileName) : CBaseLog(pszLogDirPath, pszLogFileName)
{
    //DBG("Construct CHIDLinuxGet.");

    // Initialize hidraw device handler
    m_nHidrawFd = -1;

    // Initialize file descriptor monitor
    memset(&m_tvRead, 0, sizeof(struct timeval));

    // Assign initial values to chip data
    m_usVID     = 0;
    m_usPID     = 0;
    m_usVersion = 0;
    
    // Assign Initial Value to Bus Type
    m_uiBusType = 0;

    // Assign Initial values to buffers
    m_inBuf      = NULL;
    m_inBufSize  = 0;
    m_outBuf     = NULL;
    m_outBufSize = 0;

    // Initialize mutex
    sem_init(&m_ioMutex,  0 /*scope is in this file*/, 1 /*active in initial*/);

    // Allocate memory to inBuffer
    m_inBufSize = ELAN_HID_INPUT_BUFFER_SIZE;
    //DBG("Allocate %d bytes to inBuffer.", m_inBufSize);
    m_inBuf = (unsigned char*)malloc(sizeof(unsigned char) * m_inBufSize);
    memset(m_inBuf, 0, sizeof(unsigned char)*m_inBufSize);

    // Allocate memory to outBuffer
    m_outBufSize = ELAN_HID_OUTPUT_BUFFER_SIZE;
    //DBG("Allocate %d bytes to outBuffer.", m_outBufSize);
    m_outBuf = (unsigned char*)malloc(sizeof(unsigned char) * m_outBufSize);
    memset(m_outBuf, 0, sizeof(unsigned char)*m_outBufSize);

    return;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::~CHIDLinuxGet()
// 1. Free class members
// 2. Deinitialize mutex (semaphore)
// 3. Deinitialize libusb

CHIDLinuxGet::~CHIDLinuxGet(void)
{
    // Deinitialize mutex (semaphore)
    sem_destroy(&m_ioMutex);

    // Release input buffer
    if (m_inBuf)
    {
        //DBG("Release input buffer (address=%p).", m_inBuf);
        free(m_inBuf);
        m_inBuf     = NULL;
        m_inBufSize = 0;
    }

    // Release output buffer
    if (m_outBuf)
    {
        //DBG("Release output buffer (address=%p).", m_outBuf);
        free(m_outBuf);
        m_outBuf     = NULL;
        m_outBufSize = 0;
    }

    // Clear Chip Data
    m_usVID     = 0;
    m_usPID     = 0;
    m_usVersion = 0;
    
    // Clear Bus Type
    m_uiBusType = 0;

    return;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::Close()
// 1. Close the current hidraw device
// 2. Reatach the kernel driver previously used
// 2. Release the resource occupied and clear device attributes.

void CHIDLinuxGet::Close(void)
{
    if (m_nHidrawFd >= 0)
    {
        // Release acquired hidraw device handler
        DBG("%s: Release hidraw device handle (fd=%d).", __func__, m_nHidrawFd);
        close(m_nHidrawFd);
        m_nHidrawFd = -1;
    }

    // Clear Chip Data
    m_usVID     = 0;
    m_usPID     = 0;

    // Clear Bus Type
    m_uiBusType = 0;

    return;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::GetDeviceHandle(int nParam1, int nParam2)
// 1. Connect to hid-raw device
// 2. Open requested hid device with input VID & PID
// 3. Allocate I/O buffer and fill device attibutes
// 4. Print system hid information if debug is enable

int CHIDLinuxGet::GetDeviceHandle(int nVID, int nPID)
{
    int nRet = ERR_SUCCESS,
        nError = 0;
    char szHidrawDevPath[MAX_PATH] = {0};

    // Look for elan hidraw device with specific PID
    nError = FindHidrawDevice(nVID, nPID, szHidrawDevPath, sizeof(szHidrawDevPath));
    if(nError == ERR_DEVICE_NOT_FOUND)
    {
        DBG("%s: hidraw device (VID 0x%x, PID 0x%x) not found! Retry with PID 0x%x.", __func__, nVID, nPID, ELAN_HID_FORCE_CONNECT_PID);
        nError = FindHidrawDevice(nVID, ELAN_HID_FORCE_CONNECT_PID, szHidrawDevPath, sizeof(szHidrawDevPath));
        if (nError != ERR_SUCCESS)
        {
            ERR("%s: hidraw device (VID 0x%x, PID 0x%x) not found!", __func__, nVID, ELAN_HID_FORCE_CONNECT_PID);
            nRet = ERR_DEVICE_NOT_FOUND;
            goto GET_DEVICE_HANDLE_EXIT;
        }
    }
    else if (nError != ERR_SUCCESS)
    {
        ERR("%s: hidraw device (VID 0x%x, PID 0x%x) not found!", __func__, nVID, nPID);
        nRet = ERR_DEVICE_NOT_FOUND;
        goto GET_DEVICE_HANDLE_EXIT;
    }

    // Acquire hidraw device handler for I/O
    nError = open(szHidrawDevPath, O_RDWR | O_NONBLOCK);
    if (nError < 0)
    {
        ERR("%s: Fail to Open Device %s! errno=%d.", __func__, szHidrawDevPath, nError);
        nRet = ERR_DEVICE_NOT_FOUND;
        goto GET_DEVICE_HANDLE_EXIT;
    }

    // Success
    m_nHidrawFd = nError;
    DBG("%s: Open hidraw device \'%s\' (non-blocking), fd=%d.", __func__, szHidrawDevPath, m_nHidrawFd);

GET_DEVICE_HANDLE_EXIT:
    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::IsConnected()
// Check if device connected
bool CHIDLinuxGet::IsConnected(void)
{
    bool bRet = false;

    if (m_nHidrawFd >= 0)
    {
        //DBG("Device is connected!");
        bRet = true;
    }

    return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::WriteRawBytes()
// Write Data to HID device
// cBuf: Buffer to write
// nLen: Data length to write
// nTimeout: Time to wait for device respond

int CHIDLinuxGet::WriteRawBytes(unsigned char* pszBuf, int nLen, int nTimeout, int nDevIdx)
{
    int nRet = 0,
        nResult = 0,
        nPollIndex = 0,
        nPollCount = 0;

    nPollCount = nTimeout;

    if ((unsigned)nLen > m_outBufSize)
    {
        ERR("%s: Data length too large(data=%d, buffer size=%d), ret=%d.", __func__, nLen, m_outBufSize, nRet);
        nRet = ERR_INVALID_PARAM;
        goto WRITE_RAW_BYTES_EXIT;
    }

    // Mutex locks the critical section
    sem_wait(&m_ioMutex);

    // Copy data to local buffer and write buffer data to usb
    memset(m_outBuf, 0, sizeof(unsigned char)*m_outBufSize);
    memcpy(m_outBuf, pszBuf, ((unsigned)nLen <= m_outBufSize) ? nLen : m_outBufSize);

#if defined(__ENABLE_DEBUG__) && defined(__ENABLE_OUTBUF_DEBUG__)
    if ((g_bEnableDebug == true) && (g_bEnableOutputBufferDebug == true))
        DebugPrintBuffer("m_outBuf", m_outBuf, nLen);
#endif //__ENABLE_DEBUG__ && __ENABLE_OUTBUF_DEBUG__

    // Write Buffer Data to hidraw device
    // Since ELAN i2c-hid FW has its special limit, make sure to send all 33 byte once to IC.
    // If data size is not 33, FW will not accept the command even if data format is correct.
    for (nPollIndex = 0; nPollIndex < nPollCount; nPollIndex++)
    {
        nResult = write(m_nHidrawFd, m_outBuf, m_outBufSize);
        if (nResult < 0)
        {
            //ERR("%s: Fail to write data! errno=%d.", __func__, nResult);
            nRet = ERR_IO_ERROR;
        }
        else if ((unsigned)nResult != m_outBufSize)
        {
            //ERR("%s: Fail to write data! (write_bytes=%d, data_total=%d)", __func__, nResult, m_outBufSize);
            nRet = ERR_IO_ERROR;
        }
        else // Write len bytes of data
        {
            nRet = ERR_SUCCESS;
            break;
        }
    }

    // Mutex unlocks the critical section
    sem_post(&m_ioMutex);

    // Error Report
    if (nRet != ERR_SUCCESS)
    {
        if (nResult < 0)
        {
            ERR("%s: Fail to write data! errno=%d.", __func__, nResult);
        }
        else
        {
            ERR("%s: Fail to write data! (write_bytes=%d, data_total=%d)", __func__, nResult, m_outBufSize);
        }
    }

WRITE_RAW_BYTES_EXIT:
    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::WriteCommand()
// Write Command Data to HID device
// cBuf: Buffer to write
// nLen: Data length to write
// nTimeout: Time to wait for device respond

int CHIDLinuxGet::WriteCommand(unsigned char* pszCommandBuf, int nCommandLen, int nTimeout, int nDevIdx)
{
    int nRet = ERR_SUCCESS;

    // Clear Command Raw Buffer
    memset(m_szOutputBuf, 0, sizeof(m_szOutputBuf));

    // Insert 3-Byte Header Before Command
    if (m_usPID == 0x7)
        m_szOutputBuf[0] = ELAN_HID_OUTPUT_REPORT_ID_PID_B; // HID Report ID
    else
        m_szOutputBuf[0] = ELAN_HID_OUTPUT_REPORT_ID; // HID Report ID
    m_szOutputBuf[1] = 0x0; // Bridge Command
    m_szOutputBuf[2] = nCommandLen; // Command Length

    // Copy 4-Byte / 6-Byte I2C TP Command to Buffer
    memcpy(&m_szOutputBuf[3], pszCommandBuf, nCommandLen);

    // Output Command Raw Buffer
    nRet = WriteRawBytes(m_szOutputBuf, nCommandLen + 3, nTimeout, nDevIdx);
    if (nRet != ERR_SUCCESS)
    {
        ERR("%s: Fail to Write Raw Bytes! err=%d.", __func__, nRet);
    }

    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::ReadRawBytes()
// Read Data from HID device
// cBuf: Buffer to read
// nLen: Data length to read
// nTimeout: Time to wait for device respond
int CHIDLinuxGet::ReadRawBytes(unsigned char* pszBuf, int nLen, int nTimeout, int nDevIdx)
{
    int nRet = ERR_SUCCESS,
        nError = 0;

    // Mutex locks the critical section
    sem_wait(&m_ioMutex);

    // Re-initialize file descriptor monitor
    FD_ZERO(&m_fdsHidraw);

    // Add hidraw device handler to file descriptor monitor
    FD_SET(m_nHidrawFd, &m_fdsHidraw);

    // Set wait time up to nTimeout millisecond
    m_tvRead.tv_sec = nTimeout / 1000; // sec
    m_tvRead.tv_usec = (nTimeout % 1000) * 1000; // usec

    // Add file descriptor & timeout to file descriptor monitor
    nError = select(m_nHidrawFd + 1, &m_fdsHidraw, NULL, NULL, &m_tvRead);
    if (nError < 0)
    {
        ERR("%s: File descriptor monitor select fail! errno=%d.", __func__, nError);
        nRet = ERR_IO_ERROR;
        goto READ_RAW_BYTES_EXIT;
    }
    else if (nError == 0)
    {
        DBG("%s: timeout (%d ms)!", __func__, nTimeout);
        nRet = ERR_IO_TIMEOUT; // Timeout error
        goto READ_RAW_BYTES_EXIT;
    }
    else // Read Successfully
    {
        memset(m_inBuf, 0, sizeof(unsigned char)*m_inBufSize);

        if (FD_ISSET(m_nHidrawFd, &m_fdsHidraw))
        {
            nError = read(m_nHidrawFd, m_inBuf, m_inBufSize);
            if (nError < 0)
            {
                ERR("%s: Fail to Read Data! errno=%d.", __func__, nError);
                nRet = ERR_IO_ERROR;
                goto READ_RAW_BYTES_EXIT;
            }

            //DBG("Succesfully read %d bytes.\n", nError);
            nRet = ERR_SUCCESS;
        }
    }

    //DBG("Successfully read %d bytes of data from device, return %d.", transfer_cnt, ret);

#if defined(__ENABLE_DEBUG__) && defined(__ENABLE_INBUF_DEBUG__)
    if (g_bEnableDebug)
        DebugPrintBuffer("m_inBuf", m_inBuf, nLen);
#endif //__ENABLE_DEBUG__ && __ENABLE_INBUF_DEBUG__

    // Copy inBuf data to input buffer pointer
    memcpy(pszBuf, m_inBuf, ((unsigned)nLen <= m_inBufSize) ? nLen : m_inBufSize);

READ_RAW_BYTES_EXIT:
    // Mutex unlocks the critical section
    sem_post(&m_ioMutex);

    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::ReadData()
// Read Data from HID device
// pszDataBuf: Buffer to read
// nDataLen: Data length to read
// nTimeout: Time to wait for device respond

int CHIDLinuxGet::ReadData(unsigned char* pszDataBuf, int nDataLen, int nTimeout, int nDevIdx, bool bFilter)
{
    int nRet = ERR_SUCCESS,
        nInputReportID = 0,
        nDataLength = 0;

    // Clear Data Raw Buffer
    memset(m_szInputBuf, 0, sizeof(m_szInputBuf));

    /* HID Input Report Data Format of Elan Data (63-byte)  *
     * | Report ID | Data Length |  Data   |                *
     * |   0x02    |  $(length)  | ${data} |                *
     */

    // Config. Data Length
    if((bFilter == true) && (nDataLen <= ELAN_HID_DATA_BUFFER_SIZE /* $(ELAN_HID_INPUT_BUFFER_SIZE) - 2 */))
        nDataLength = nDataLen + 2;
    else
        nDataLength = nDataLen;

    // Read 2-Byte Header & Command Data to Data Raw Buffer
    nRet = ReadRawBytes(m_szInputBuf, nDataLength, nTimeout, nDevIdx);
    if (nRet != ERR_SUCCESS)
    {
        ERR("%s: Fail to Read Raw Bytes! err=0x%x.", __func__, nRet);
        goto READ_DATA_EXIT;
    }

    // Set Report ID Number for Checking
    if (m_usPID == 0xb)
        nInputReportID = ELAN_HID_INPUT_REPORT_ID_PID_B; // Report ID of Command Report
    else
        nInputReportID = ELAN_HID_INPUT_REPORT_ID; // Report ID of Command Report

    // Check if Report ID of Packet is correct
    if ((m_szInputBuf[0] != nInputReportID) &&
        (m_szInputBuf[0] != ELAN_HID_FINGER_REPORT_ID) &&
        (m_szInputBuf[0] != ELAN_HID_PEN_REPORT_ID)	 &&
        (m_szInputBuf[0] != ELAN_HID_PEN_DEBUG_REPORT_ID))
    {
        nRet = ERR_DATA_PATTERN;
        goto READ_DATA_EXIT;
    }

    if(m_szInputBuf[0] == nInputReportID) // Command Report
    {
        if((bFilter == true) && (nDataLen <= ELAN_HID_DATA_BUFFER_SIZE /* $(ELAN_HID_INPUT_BUFFER_SIZE) - 2 */))
        {
            // Strip 2-Byte Report Header & Load Data to Buffer
            memcpy(pszDataBuf, &m_szInputBuf[2], nDataLen);
        }
        else // Don't filt
        {
            // Load Report Header & Data to Buffer
            memcpy(pszDataBuf, m_szInputBuf, nDataLen);
        }
    }
    else // Finger or Pen Report
    {
        // Load Report Header & Data to Buffer
        memcpy(pszDataBuf, m_szInputBuf, nDataLen);
    }

READ_DATA_EXIT:
    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::GetDevVidPid()
// Return Current VID & PID

int CHIDLinuxGet::GetDevVidPid(unsigned int* p_nVid, unsigned int* p_nPid, int nDevIdx)
{
    int nRet = ERR_SUCCESS;

    // Make Sure Input Pointers Valid
    if((p_nVid == NULL) || (p_nPid == NULL))
    {
        ERR("%s: Input Parameters Invalid! (p_nVid=%p, p_nPid=%p)", __func__, p_nVid, p_nPid);
        nRet = ERR_INVALID_PARAM;
        goto GET_DEV_VID_PID_EXIT;
    }

    // Make Sure Device Found
    if((m_usVID == 0) && (m_usPID == 0))
    {
        ERR("%s: HID Device Not Found!", __func__);
        nRet = ERR_DEVICE_NOT_FOUND;
        goto GET_DEV_VID_PID_EXIT;
    }

    // Set PID & VID
    *p_nVid = m_usVID;
    *p_nPid = m_usPID;

    // Success
    nRet = ERR_SUCCESS;

GET_DEV_VID_PID_EXIT:
    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::GetDevBusType()
// Return Current Bus Type

int CHIDLinuxGet::GetDevBusType(unsigned int* p_uiBusType, int nDevIdx)
{
    int nRet = ERR_SUCCESS;

    // Make Sure Input Pointers Valid
    if (p_uiBusType == NULL)
    {
        ERR("%s: Input Parameters Invalid! (p_uiBusType=%p)", __func__, p_uiBusType);
        nRet = ERR_INVALID_PARAM;
        goto GET_DEV_BUS_TYPE_EXIT;
    }

    // Make Sure Device Bus Type is Valid 
    DBG("%s: Bus Type = 0x%x.", __func__, m_uiBusType);
    if (m_uiBusType == 0)
    {
        ERR("%s: Invalid Device Bus Type! (Bus Type: 0x%x)", __func__, m_uiBusType);
        nRet = ERR_UNKNOWN_DEVICE_TYPE;
        goto GET_DEV_BUS_TYPE_EXIT;
    }

    // Set Bus Type
    *p_uiBusType = m_uiBusType;

    // Success
    nRet = ERR_SUCCESS;

GET_DEV_BUS_TYPE_EXIT:
    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::GetInBufferSize()
// Return Input Buffer Size

int CHIDLinuxGet::GetInBufferSize(void)
{
    //DBG("Current Input Buffer Size = %d.", m_inBufSize);
    return m_inBufSize;
}

/////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::GetOutBufferSize()
// Return Output Buffer Size

int CHIDLinuxGet::GetOutBufferSize(void)
{
    //DBG("Current Output Buffer Size = %d.", m_outBufSize);
    return m_outBufSize;
}

////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::GetInterfaceType()
// Return Interface Type

int CHIDLinuxGet::GetInterfaceType(void)
{
    return INTF_TYPE_HID_LINUX;
}

////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::GetInterfaceVersion()
// Return Version of Interface Inplementation

const char* CHIDLinuxGet::GetInterfaceVersion(void)
{
    return HID_LINUX_INTF_IMPL_VER;
}

////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::bus_str()
// Get Bus String from BUS ID
const char* CHIDLinuxGet::bus_str(int bus)
{
    switch (bus)
    {
        case BUS_USB:
            return "USB";

        case BUS_HIL:
            return "HIL";

        case BUS_BLUETOOTH:
            return "Bluetooth";

#if 0 // Disable this convert since the definition is not include in input.h of android sdk.
        case BUS_VIRTUAL:
            return "Virtual";
#endif // 0

        case BUS_PCI:
            return "PCI"; // THC (Touch Host Controller) IC

        case BUS_I2C:
            return "I2C";

        case BUS_SPI:
            return "SPI";

        default:
            return "Other";
    }
}

////////////////////////////////////////////////////////////////////////////
// CHIDLinuxGet::FindHidrawDevice()
// Find hidraw device name with specific VID and PID, such as /dev/hidraw0
int CHIDLinuxGet::FindHidrawDevice(int nVID, int nPID, char *pszDevicePath, size_t nDevicePathBufLen)
{
    int nRet = ERR_SUCCESS,
        nError = 0,
        nFd = 0;
    bool bFound = false;
    DIR *pDirectory = NULL;
    struct dirent *pDirEntry = NULL;
    const char *pszPath = "/dev";
    //char szFile[64] = {0};
    char szFile[MAX_PATH] = {0}; // > (sizeof('/dev') + sizeof('/') + sizeof(dirent->dname[256]))
    struct hidraw_devinfo info;

    // Check if filename ptr is valid
    if ((pszDevicePath == NULL) || (nDevicePathBufLen == 0))
    {
        ERR("%s: Invalid Parameter! (pszDevicePath=%p, nDevicePathBufLen=%zd)", __func__, pszDevicePath, nDevicePathBufLen);
        nRet = ERR_INVALID_PARAM;
        goto FIND_ELAN_HIDRAW_DEVICE_EXIT;
    }

    if(nDevicePathBufLen < MAX_PATH)
    {
        ERR("%s: Buffer Too Small! (nDevicePathBufLen=%zd, Minimal Requirement: %d)", __func__, nDevicePathBufLen, MAX_PATH);
        nRet = ERR_INVALID_PARAM;
        goto FIND_ELAN_HIDRAW_DEVICE_EXIT;
    }

    // Open Directory
    pDirectory = opendir(pszPath);
    if (pDirectory == NULL)
    {
        ERR("%s: Fail to Open Directory %s.", __func__, pszPath);
        nRet = ERR_DEVICE_NOT_FOUND;
        goto FIND_ELAN_HIDRAW_DEVICE_EXIT;
    }

    // Traverse Directory Elements
    while ((pDirEntry = readdir(pDirectory)) != NULL)
    {
        // Only reserve hidraw devices
        if (strncmp(pDirEntry->d_name, "hidraw", 6))
            continue;

        memset(szFile, 0, sizeof(szFile));
        sprintf(szFile, "%s/%s", pszPath, pDirEntry->d_name);
        DBG("%s: file=\"%s\".", __func__, szFile);

        /* Open the Device with non-blocking reads. In real life,
          don't use a hard coded path; use libudev instead. */
        nError = open(szFile, O_RDWR | O_NONBLOCK);
        if (nError < 0)
        {
            DBG("%s: Fail to Open Device %s! errno=%d.", __func__, pDirEntry->d_name, nError);
            continue;
        }
        nFd = nError;

        /* Get Raw Info */
        nError = ioctl(nFd, HIDIOCGRAWINFO, &info);
        if (nError >= 0)
        {
            DBG("--------------------------------");
            DBG("  bustype: 0x%02x (%s)", info.bustype, bus_str(info.bustype));
            DBG("  vendor: 0x%04hx", info.vendor);
            DBG("  product: 0x%04hx", info.product);

            // Force touch device to connect if bustype=0x18(BUS_I2C) or 0x1C(BUS_SPI) or 0x01(BUS_PCI), VID=0x4f3, and PID=0x0
            if (((info.bustype == BUS_I2C) || (info.bustype == BUS_SPI) || (info.bustype == BUS_PCI) /* THC IC */ ) &&
                (info.vendor == ELAN_HID_VID) /* nVID = usb_dev_desc.idVendor = ELAN_HID_VID */  &&
                (nPID == ELAN_HID_FORCE_CONNECT_PID))
            {
                // Use found PID from Hid-Raw
                nPID = info.product;
                DBG("%s: bustype=0x%02x, VID=0x%04x, PID=0x%04x => PID changes to 0x%04x.", __func__, info.bustype, ELAN_HID_VID, ELAN_HID_FORCE_CONNECT_PID, nPID);
            }

            if ((info.vendor == nVID) && (info.product == nPID))
            {
                DBG("%s: Found hidraw device (VID: 0x%x, PID: 0x%x, BusType: 0x%x)!", __func__, nVID, nPID, info.bustype);
                m_usVID = (unsigned short) nVID;
                m_usPID = (unsigned short) nPID;
                m_uiBusType = info.bustype;
                memcpy(pszDevicePath, szFile, sizeof(szFile));
                bFound = true;
            }
        }

        // Close Device
        close(nFd);

        // Stop the loop if found
        if (bFound == true)
            break;
    }

    if (!bFound)
        nRet = ERR_DEVICE_NOT_FOUND;

    // Close Directory
    closedir(pDirectory);

FIND_ELAN_HIDRAW_DEVICE_EXIT:
    return nRet;
}
