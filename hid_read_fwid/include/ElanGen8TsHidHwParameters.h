/** @file

  Header of HW Parameters of Elan Gen8 HID (I2C-HID / SPI-HID) Touchscreen.
  The touch controller aforementioned includes Gen8 touch series.

  Copyright (c) ELAN microelectronics corp. 2022, All Rights Reserved

  Module Name:
	ElanGen8TsHidHwParameters.h

  Environment:
	All kinds of Linux-like Platform.

********************************************************************
 Revision History

**/

#ifndef __ELAN_GEN8_TS_HID_HW_PARAMETERS_H__
#define __ELAN_GEN8_TS_HID_HW_PARAMETERS_H__
#pragma once

/***************************************************
 * Definitions
 ***************************************************/

// Hello Packet (Normal IAP)
#ifndef ELAN_GEN8_HID_NORMAL_MODE_HELLO_PACKET
#define ELAN_GEN8_HID_NORMAL_MODE_HELLO_PACKET	0x21
#endif //ELAN_GEN8_HID_NORMAL_MODE_HELLO_PACKET

// Hello Packet (Recovery IAP)
#ifndef ELAN_GEN8_HID_RECOVERY_MODE_HELLO_PACKET
#define ELAN_GEN8_HID_RECOVERY_MODE_HELLO_PACKET	0x57
#endif //ELAN_GEN8_HID_RECOVERY_MODE_HELLO_PACKET

//
// High Byte of Boot Code Version (in Recovery Mode)
//

// EM32F901
#ifndef BC_VER_H_BYTE_FOR_EM32F901_HID
#define BC_VER_H_BYTE_FOR_EM32F901_HID                0x95
#endif //BC_VER_H_BYTE_FOR_EM32F901_HID

// EM32F902
#ifndef BC_VER_H_BYTE_FOR_EM32F902_HID
#define BC_VER_H_BYTE_FOR_EM32F902_HID                0x9C
#endif //BC_VER_H_BYTE_FOR_EM32F902_HID

#endif //__ELAN_GEN8_TS_HID_HW_PARAMETERS_H__
