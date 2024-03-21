/** @file

  Header of Function Utility for Elan Gen8 HID (I2C-HID / SPI-HID) Touchscreen.

  Copyright (c) ELAN microelectronics corp. 2022, All Rights Reserved

  Module Name:
	ElanGen8TsHidUtility.h

  Environment:
	All kinds of Linux-like Platform.

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

// ROM Data
int gen8_send_read_rom_data_command(unsigned int addr, unsigned char data_len);
int gen8_receive_rom_data(unsigned int *p_rom_data);

#endif //__ELAN_GEN8_TS_HID_UTILITY_H__
