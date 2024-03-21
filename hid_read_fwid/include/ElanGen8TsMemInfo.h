/** @file

  Header of Memory Information for Elan Gen8 HID (I2C-HID / SPI-HID) Touchscreen.

  Copyright (c) ELAN microelectronics corp. 2022, All Rights Reserved

  Module Name:
	ElanGen8TsMemInfo.h

  Environment:
	All kinds of Linux-like Platform.

********************************************************************
 Revision History

**/

#ifndef __ELAN_GEN8_TS_MEM_INFO_H__
#define __ELAN_GEN8_TS_MEM_INFO_H__
#pragma once

/***************************************************
 * Definitions
 ***************************************************/

// Memory Page Size
#ifndef ELAN_GEN8_MEMORY_PAGE_SIZE
#define ELAN_GEN8_MEMORY_PAGE_SIZE               2048  // 0x800
#endif //ELAN_GEN8_MEMORY_PAGE_SIZE

// Information ROM Address
#ifndef ELAN_GEN8_INFO_ROM_MEMORY_ADDR
#define ELAN_GEN8_INFO_ROM_MEMORY_ADDR           0x00040000
#endif //ELAN_GEN8_INFO_ROM_MEMORY_ADDR

// Information ROM FWID
#ifndef ELAN_GEN8_INFO_ROM_FWID_MEMORY_ADDR
#define ELAN_GEN8_INFO_ROM_FWID_MEMORY_ADDR	   (ELAN_GEN8_INFO_ROM_MEMORY_ADDR)
#endif //ELAN_GEN8_INFO_ROM_FWID_MEMORY_ADDR

// Information Memory Page Address (The 4th Info. Memory Page)
#ifndef ELAN_GEN8_INFO_MEMORY_PAGE_3_ADDR
#define ELAN_GEN8_INFO_MEMORY_PAGE_3_ADDR        0x00041800
#endif //ELAN_GEN8_INFO_MEMORY_PAGE_3_ADDR

// Update Counter
#ifndef ELAN_GEN8_UPDATE_COUNTER_ADDR
#define ELAN_GEN8_UPDATE_COUNTER_ADDR            0x00041C00
#endif //ELAN_GEN8_UPDATE_COUNTER_ADDR

// Last Update Time (Year)
#ifndef ELAN_GEN8_LAST_UPDATE_TIME_YEAR_ADDR
#define ELAN_GEN8_LAST_UPDATE_TIME_YEAR_ADDR     0x00041C10
#endif //ELAN_GEN8_LAST_UPDATE_TIME_YEAR_ADDR

// Last Update Time (Month)
#ifndef ELAN_GEN8_LAST_UPDATE_TIME_MONTH_ADDR
#define ELAN_GEN8_LAST_UPDATE_TIME_MONTH_ADDR    0x00041C14
#endif //ELAN_GEN8_LAST_UPDATE_TIME_MONTH_ADDR

// Last Update Time (Day)
#ifndef ELAN_GEN8_LAST_UPDATE_TIME_DAY_ADDR
#define ELAN_GEN8_LAST_UPDATE_TIME_DAY_ADDR      0x00041C18
#endif //ELAN_GEN8_LAST_UPDATE_TIME_DAY_ADDR

// Last Update Time (Hour)
#ifndef ELAN_GEN8_LAST_UPDATE_TIME_HOUR_ADDR
#define ELAN_GEN8_LAST_UPDATE_TIME_HOUR_ADDR     0x00041C1C
#endif //ELAN_GEN8_LAST_UPDATE_TIME_HOUR_ADDR

// Last Update Time (Minute)
#ifndef ELAN_GEN8_LAST_UPDATE_TIME_MINUTE_ADDR
#define ELAN_GEN8_LAST_UPDATE_TIME_MINUTE_ADDR   0x00041C20
#endif //ELAN_GEN8_LAST_UPDATE_TIME_MINUTE_ADDR


// Remark ID Length
#ifndef ELAN_GEN8_REMARK_ID_LEN
#define ELAN_GEN8_REMARK_ID_LEN                  16
#endif //ELAN_GEN8_REMARK_ID_LEN

// Remark ID Index
#ifndef ELAN_GEN8_REMARK_ID_INDEX_ADDR
#define ELAN_GEN8_REMARK_ID_INDEX_ADDR           0x00042200
#endif //ELAN_GEN8_REMARK_ID_INDEX_ADDR

#endif //__ELAN_GEN8_TS_MEM_INFO_H__
