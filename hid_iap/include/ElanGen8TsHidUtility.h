/** @file

  Header of Function Utility for Elan Gen8 HID (I2C-HID / SPI-HID) Touchscreen.

  Module Name:
	ElanGen8TsHidUtility.h

  Environment:
	All kinds of Linux-like Platform.

  Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
********************************************************************
 Revision History

**/

#ifndef __ELAN_GEN8_TS_HID_UTILITY_H__
#define __ELAN_GEN8_TS_HID_UTILITY_H__
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "ElanTsDebug.h"

/***************************************************
 * Definitions
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

// Write Command
extern int write_cmd(unsigned char *cmd_buf, int len, int timeout_ms);

// Read Data
extern int read_data(unsigned char *data_buf, int len, int timeout_ms);

// Write Vendor Command
extern int write_vendor_cmd(unsigned char *cmd_buf, int len, int timeout_ms);

// HID Raw I/O
extern int __hidraw_write(unsigned char* buf, int len, int timeout_ms);
extern int __hidraw_read(unsigned char* buf, int len, int timeout_ms);

/***************************************************
 * Function Prototype
 ***************************************************/

// Test Version
int gen8_read_test_version_data(void);
int gen8_get_test_version_data(unsigned short *p_test_version);

// ROM Data
int gen8_send_read_rom_data_command(unsigned int addr, unsigned char data_len);
int gen8_receive_rom_data(unsigned int *p_rom_data);

// IAP Mode
int send_gen8_write_flash_key_command(void);

// Erase Flash Section
int send_erase_flash_section_command(unsigned int address, unsigned short page_count);
int receive_erase_flash_section_response(void);

#endif //__ELAN_GEN8_TS_HID_UTILITY_H__
