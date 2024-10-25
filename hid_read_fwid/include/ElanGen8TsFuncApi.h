/** @file

  Header of Function APIs for Elan Gen8 HID (I2C-HID / SPI-HID) Touchscreen.

  Module Name:
	ElanGen8TsFuncApi.h

  Environment:
	All kinds of Linux-like Platform.

  Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
********************************************************************
 Revision History

**/

#ifndef __ELAN_GEN8_TS_FUNC_API_H__
#define __ELAN_GEN8_TS_FUNC_API_H__
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ElanGen8TsMemInfo.h"

/***************************************************
 * Definitions
 ***************************************************/

/***************************************************
 * Macros
 ***************************************************/

// High Word
#ifndef HIGH_WORD_OF_UINT
#define HIGH_WORD_OF_UINT(uint_value)    ((unsigned short)((uint_value & 0xFFFF0000) >> 16))
#endif //HIGH_WORD_OF_UINT

// Low Word
#ifndef LOW_WORD_OF_UINT
#define LOW_WORD_OF_UINT(uint_value)     ((unsigned short)(uint_value & 0x0000FFFF))
#endif //LOW_WORD_OF_UINT

// Low Byte of Unsigned Int Value
#ifndef LOW_BYTE_OF_UINT
#define LOW_BYTE_OF_UINT(uint_value)     ((unsigned char)(uint_value & 0x000000FF))
#endif //LOW_BYTE_OF_UINT

/***************************************************
 * Declaration of Data Structure
 ***************************************************/

/***************************************************
 * Global Data Structure Declaration
 ***************************************************/

/***************************************************
 * Global Variables Declaration
 ***************************************************/

/***************************************************
 * Extern Variables Declaration
 ***************************************************/

/***************************************************
 * Function Prototype
 ***************************************************/

// ROM Data
int gen8_get_rom_data(unsigned int addr, unsigned char data_len, unsigned int *p_data);

// Information FWID
int gen8_read_info_fwid(unsigned short *p_info_fwid, bool recovery);

#endif //__ELAN_GEN8_TS_FUNC_API_H__
