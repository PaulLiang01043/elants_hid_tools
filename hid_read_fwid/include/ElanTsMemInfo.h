/** @file

  Header of Memory Information for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Copyright (c) ELAN microelectronics corp. 2024, All Rights Reserved

  Module Name:
	ElanTsMemInfo.h

  Environment:
	All kinds of Linux-like Platform.

********************************************************************
 Revision History

**/

#ifndef __ELAN_TS_MEM_INFO_H__
#define __ELAN_TS_MEM_INFO_H__
#pragma once

/***************************************************
 * Definitions
 ***************************************************/

// Information ROM Address
#ifndef ELAN_INFO_ROM_MEMORY_ADDR
#define ELAN_INFO_ROM_MEMORY_ADDR                 0x8000
#endif //ELAN_INFO_ROM_MEMORY_ADDR

// Information Page ROM FWID Address
#ifndef ELAN_INFO_ROM_FWID_MEMORY_ADDR
#define ELAN_INFO_ROM_FWID_MEMORY_ADDR	    0x8080
#endif //ELAN_INFO_ROM_FWID_MEMORY_ADDR

#endif //__ELAN_TS_MEM_INFO_H__
