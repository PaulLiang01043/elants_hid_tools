//
// ErrCode.h: Declaration of Error Code.
//
// Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef __ERR_CODE_H__
#define __ERR_CODE_H__
#pragma once

//////////////////////////////////////////////////////////////
// Error code definition
//////////////////////////////////////////////////////////////

/** Function execution is success. **/
#ifndef ERR_SUCCESS
#define ERR_SUCCESS                             0x0000
#endif //ERR_SUCCESS

/** This function is not support. **/
#ifndef ERR_FUNC_NOT_SUPPORT
#define ERR_FUNC_NOT_SUPPORT                    0x0001
#endif //ERR_FUNC_NOT_SUPPORT

/** Read/Write data timeout **/
#ifndef ERR_IO_TIMEOUT
#define ERR_IO_TIMEOUT                          0x0003
#endif //ERR_IO_TIMEOUT

/** For any read command, sometimes the return data should meet some rule. **/
#ifndef ERR_DATA_PATTERN
#define ERR_DATA_PATTERN                        0x0005
#endif //ERR_DATA_PATTERN

/** Fail to create interface. **/
#ifndef ERR_NO_INTERFACE_CREATED
#define ERR_NO_INTERFACE_CREATED                0x0006
#endif //ERR_NO_INTERFACE_CREATED

/** Invalid Parameter **/
#ifndef ERR_INVALID_PARAM
#define ERR_INVALID_PARAM                       0x0008
#endif //ERR_INVALID_PARAM

/** I/O Error **/
#ifndef ERR_IO_ERROR
#define ERR_IO_ERROR                            0x0009
#endif //ERR_IO_ERROR

/** Value(s) mismatched for some cases **/
#ifndef ERR_DATA_MISMATCHED
#define ERR_DATA_MISMATCHED                     0x000A
#endif //ERR_DATA_MISMATCHED

/** Connect Elan Bridge and not get hello packet **/
#ifndef ERR_CONNECT_NO_HELLO_PACKET
#define ERR_CONNECT_NO_HELLO_PACKET             0x0102
#endif //ERR_CONNECT_NO_HELLO_PACKET

/** Did not find any support device connectted to PC. Please check connectoin. **/
#ifndef ERR_DEVICE_NOT_FOUND
#define ERR_DEVICE_NOT_FOUND                    0x0104
#endif //ERR_DEVICE_NOT_FOUND

/** Something Like File or Directory Not Found  **/
#ifndef ERR_FILE_NOT_FOUND
#define ERR_FILE_NOT_FOUND                      0x0105
#endif //ERR_FILE_NOT_FOUND

/** Data Not Found  **/
#ifndef ERR_DATA_NOT_FOUND
#define ERR_DATA_NOT_FOUND                      0x0106
#endif //ERR_FILE_NOT_FOUND

/** File I/O Error  **/
#ifndef ERR_FILE_IO_ERROR
#define ERR_FILE_IO_ERROR                       0x0107
#endif //ERR_FILE_IO_ERROR

/** Unknown Device Type **/
#ifndef ERR_UNKNOWN_DEVICE_TYPE
#define ERR_UNKNOWN_DEVICE_TYPE                 0x010f
#endif //ERR_UNKNOWN_DEVICE_TYPE

/** Fail to get data. **/
#ifndef ERR_GET_DATA_FAIL
#define ERR_GET_DATA_FAIL                       0x0202
#endif //ERR_GET_DATA_FAIL

/* System Command Fail */
#ifndef ERR_SYSTEM_COMMAND_FAIL
#define ERR_SYSTEM_COMMAND_FAIL                 0x0701
#endif //ERR_SYSTEM_COMMAND_FAIL

#endif //__ERR_CODE_H__
