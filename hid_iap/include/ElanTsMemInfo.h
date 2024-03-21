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
 
// Memory Page Size
#ifndef ELAN_MEMORY_PAGE_SIZE
#define ELAN_MEMORY_PAGE_SIZE                     128  // 0x40 (in word)
#endif //ELAN_MEMORY_PAGE_SIZE

// Information ROM Address
#ifndef ELAN_INFO_ROM_MEMORY_ADDR
#define ELAN_INFO_ROM_MEMORY_ADDR                 0x8000
#endif //ELAN_INFO_ROM_MEMORY_ADDR

// Information Memory Page Address (The 2nd Info. Memory Page)
#ifndef ELAN_INFO_MEMORY_PAGE_1_ADDR
#define ELAN_INFO_MEMORY_PAGE_1_ADDR              0x8040
#endif //ELAN_INFO_MEMORY_PAGE_1_ADDR

// Information Page Address to Write
#ifndef ELAN_INFO_PAGE_WRITE_MEMORY_ADDR
#define ELAN_INFO_PAGE_WRITE_MEMORY_ADDR          0x0040
#endif //ELAN_INFO_PAGE_WRITE_MEMORY_ADDR

// Update Counter
#ifndef ELAN_UPDATE_COUNTER_ADDR
#define ELAN_UPDATE_COUNTER_ADDR                  0x8060
#endif //ELAN_UPDATE_COUNTER_ADDR

// Last Update Time (Year)
#ifndef ELAN_LAST_UPDATE_TIME_YEAR_ADDR
#define ELAN_LAST_UPDATE_TIME_YEAR_ADDR           0x8061
#endif //ELAN_LAST_UPDATE_TIME_YEAR_ADDR

// Last Update Time (Month_Day)
#ifndef ELAN_LAST_UPDATE_TIME_MONTH_DAY_ADDR
#define ELAN_LAST_UPDATE_TIME_MONTH_DAY_ADDR      0x8062
#endif //ELAN_LAST_UPDATE_TIME_MONTH_DAY_ADDR

// Last Update Time (Hour_Minute)
#ifndef ELAN_LAST_UPDATE_TIME_HOUR_MINUTE_ADDR
#define ELAN_LAST_UPDATE_TIME_HOUR_MINUTE_ADDR    0x8063
#endif //ELAN_LAST_UPDATE_TIME_HOUR_MINUTE_ADDR

// Information Page ROM FWID Address
#ifndef ELAN_INFO_ROM_FWID_MEMORY_ADDR
#define ELAN_INFO_ROM_FWID_MEMORY_ADDR	    0x8080
#endif //ELAN_INFO_ROM_FWID_MEMORY_ADDR

// Elan ROM Address of Remark ID
#ifndef ELAN_INFO_ROM_REMARK_ID_MEMORY_ADDR
#define ELAN_INFO_ROM_REMARK_ID_MEMORY_ADDR       0x801F
#endif //ELAN_INFO_ROM_REMARK_ID_MEMORY_ADDR

// Elan Remark ID of Non-Remark IC
#ifndef ELAN_REMARK_ID_OF_NON_REMARK_IC
#define ELAN_REMARK_ID_OF_NON_REMARK_IC           0xFFFF
#endif //ELAN_REMARK_ID_OF_NON_REMARK_IC

#endif //__ELAN_TS_MEM_INFO_H__
