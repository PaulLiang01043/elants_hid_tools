/** @file

  Header of Function APIs for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Module Name:
	ElanTsFuncApi.h

  Environment:
	All kinds of Linux-like Platform.

  Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
********************************************************************
 Revision History

**/

#ifndef __ELAN_TS_FUNC_API_H__
#define __ELAN_TS_FUNC_API_H__
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ElanTsMemInfo.h"
#include "ElanTsDebug.h"

/***************************************************
 * Definitions
 ***************************************************/

// Error Retry Count
#ifndef ERROR_RETRY_COUNT
#define ERROR_RETRY_COUNT	3
#endif //ERROR_RETRY_COUNT

/***************************************************
 * Macros
 ***************************************************/

// High Byte
#ifndef HIGH_BYTE
#define HIGH_BYTE(data_word)    ((unsigned char)((data_word & 0xFF00) >> 8))
#endif //HIGH_BYTE

// Low Byte
#ifndef LOW_BYTE
#define LOW_BYTE(data_word)    ((unsigned char)(data_word & 0x00FF))
#endif //LOW_BYTE

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

// Firmware Information
int get_boot_code_version(unsigned short *p_bc_version);
int get_firmware_id(unsigned short *p_fw_id);
int get_fw_version(unsigned short *p_fw_version);
int get_test_version(unsigned short *p_test_version);

// Solution ID
int get_solution_id(unsigned char *p_solution_id);

// Hello Packet / BC Version
int get_hello_packet_bc_version(unsigned char *p_hello_packet, unsigned short *p_bc_version);
int get_hello_packet_bc_version_with_error_retry(unsigned char *p_hello_packet, unsigned short *p_bc_version, int retry_count);
int get_hello_packet_with_error_retry(unsigned char *p_hello_packet, int retry_count);

// Information FWID
int read_info_fwid(unsigned short *p_info_fwid, bool recovery);

// ROM Data
int get_rom_data(unsigned short addr, bool recovery, unsigned short *p_data);

// Bulk ROM Data
int get_bulk_rom_data(unsigned short addr, unsigned short *p_data);

#endif //__ELAN_TS_FUNC_API_H__
