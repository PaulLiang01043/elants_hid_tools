/** @file

  Header of HW Parameters of Elan HID (I2C-HID / SPI-HID) Touchscreen.
  The touch controller aforementioned includes Elan Gen5 / Gen6 / Gen7 touch series.

  Copyright (c) ELAN microelectronics corp. 2022, All Rights Reserved

  Module Name:
	ElanTsHidHwParameters.h

  Environment:
	All kinds of Linux-like Platform.

********************************************************************
 Revision History

**/

#ifndef __ELAN_TS_HID_HW_PARAMETERS_H__
#define __ELAN_TS_HID_HW_PARAMETERS_H__
#pragma once

/***************************************************
 * Definitions
 ***************************************************/

// Hello Packet (Normal IAP)
#ifndef ELAN_HID_NORMAL_MODE_HELLO_PACKET
#define ELAN_HID_NORMAL_MODE_HELLO_PACKET	0x20
#endif //ELAN_HID_NORMAL_MODE_HELLO_PACKET

// Hello Packet (Recovery IAP)
#ifndef ELAN_HID_RECOVERY_MODE_HELLO_PACKET
#define ELAN_HID_RECOVERY_MODE_HELLO_PACKET	0x56
#endif //ELAN_HID_RECOVERY_MODE_HELLO_PACKET

//
// Solution ID (High Byte of FW Version)
//

// 6315x1
#ifndef SOLUTION_ID_EKTH6315x1
#define SOLUTION_ID_EKTH6315x1        0x61
#endif //SOLUTION_ID_EKTH6315x1

// 6315x2
#ifndef SOLUTION_ID_EKTH6315x2
#define SOLUTION_ID_EKTH6315x2        0x62
#endif //SOLUTION_ID_EKTH6315x2

// 6315 remark to 5015M
#ifndef SOLUTION_ID_EKTH6315to5015M
#define SOLUTION_ID_EKTH6315to5015M   0x59
#endif //SOLUTION_ID_EKTH6315to5015M

// 6315 remark to 3915
#ifndef SOLUTION_ID_EKTH6315to3915P
#define SOLUTION_ID_EKTH6315to3915P   0x15
#endif //SOLUTION_ID_EKTH6315to3915P

// 7315x1 
#ifndef SOLUTION_ID_EKTH7315x1
#define SOLUTION_ID_EKTH7315x1        0x64
#endif //SOLUTION_ID_EKTH7315x1

// 7315x2
#ifndef SOLUTION_ID_EKTH7315x2
#define SOLUTION_ID_EKTH7315x2        0x65
#endif //SOLUTION_ID_EKTH7315x2

// 7318x1
#ifndef SOLUTION_ID_EKTH7318x1
#define SOLUTION_ID_EKTH7318x1        0x67
#endif //SOLUTION_ID_EKTH7318x1

//
// High Byte of Boot Code Version (in Recovery Mode)
//

// 6315
#ifndef BC_VER_H_BYTE_FOR_EKTA6315_HID
#define BC_VER_H_BYTE_FOR_EKTA6315_HID                 0xA7
#endif //BC_VER_H_BYTE_FOR_EKTA6315_HID

// 6315 to 5015M
#ifndef BC_VER_H_BYTE_FOR_EKTH6315_TO_5015M_HID
#define BC_VER_H_BYTE_FOR_EKTH6315_TO_5015M_HID        0xE6
#endif //BC_VER_H_BYTE_FOR_EKTH6315_TO_5015M_HID

// 6315 to 3915P
#ifndef BC_VER_H_BYTE_FOR_EKTH6315_TO_3915P_HID
#define BC_VER_H_BYTE_FOR_EKTH6315_TO_3915P_HID        0xF6
#endif //BC_VER_H_BYTE_FOR_EKTH6315_TO_3915P_HID

// 6308
#ifndef BC_VER_H_BYTE_FOR_EKTA6308_HID
#define BC_VER_H_BYTE_FOR_EKTA6308_HID                 0xA8
#endif //BC_VER_H_BYTE_FOR_EKTA6308_HID

// 7315
#ifndef BC_VER_H_BYTE_FOR_EKTA7315_HID
#define BC_VER_H_BYTE_FOR_EKTA7315_HID                 0xA9
#endif //BC_VER_H_BYTE_FOR_EKTA7315_HID

#endif //__ELAN_TS_HID_HW_PARAMETERS_H__
