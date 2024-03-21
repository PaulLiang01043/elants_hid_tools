/** @file

  Header of Function Utility for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Copyright (c) ELAN microelectronics corp. 2019, All Rights Reserved

  Module Name:
	ElanTsHidUtility.h

  Environment:
	All kinds of Linux-like Platform.

********************************************************************
 Revision History

**/

#ifndef __ELAN_TS_HID_UTILITY_H__
#define __ELAN_TS_HID_UTILITY_H__
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "HidConfig.h"
#include "ElanTsDebug.h"
#include "ElanTsHidHwParameters.h" // HW Parameters for Elan Gen5 / Gen6 / Gen7 Touch Controllers

/***************************************************
 * Definitions
 ***************************************************/

/*******************************************
 * Global Data Structure Declaration
 ******************************************/

/*******************************************
 * Global Variables Declaration
 ******************************************/

/*******************************************
 * Extern Variables Declaration
 ******************************************/

// Write Command
extern int write_cmd(unsigned char *cmd_buf, int len, int timeout_ms);

// Read Data
extern int read_data(unsigned char *data_buf, int len, int timeout_ms);

// Write Vendor Command
extern int write_vendor_cmd(unsigned char *cmd_buf, int len, int timeout_ms);

// HID Raw I/O
extern int __hidraw_write(unsigned char* buf, int len, int timeout_ms);
extern int __hidraw_read(unsigned char* buf, int len, int timeout_ms);

/*******************************************
 * Function Prototype
 ******************************************/

// FW ID
int send_fw_id_command(void);
int read_fw_id_data(void);
int get_fw_id_data(unsigned short *p_fw_id);

// FW Version
int send_fw_version_command(void);
int read_fw_version_data(bool quiet /* Silent Mode */);
int get_fw_version_data(unsigned short *p_fw_version);

// Test Version
int send_test_version_command(void);
int read_test_version_data(void);
int get_test_version_data(unsigned short *p_test_version);

// Boot Code Version
int send_boot_code_version_command(void);
int read_boot_code_version_data(void);
int get_boot_code_version_data(unsigned short *p_bc_version);

// ROM Data
int send_read_rom_data_command(unsigned short addr, bool recovery, unsigned char info);
int receive_rom_data(unsigned short *p_rom_data);

// Bulk ROM Data
int send_show_bulk_rom_data_command(unsigned short addr, unsigned short len);

// Bulk ROM Data (in Boot Code)
int send_show_bulk_rom_data_command(unsigned short addr);
int receive_bulk_rom_data(unsigned short *p_rom_data);

// Hello Packet
int send_request_hello_packet_command(void);

#endif //__ELAN_TS_HID_UTILITY_H__
