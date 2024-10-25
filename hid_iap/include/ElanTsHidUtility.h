/** @file

  Header of Function Utility for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Module Name:
	ElanTsHidUtility.h

  Environment:
	All kinds of Linux-like Platform.

  Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
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

// Slave Address
#ifndef ELAN_I2C_SLAVE_ADDR
#define ELAN_I2C_SLAVE_ADDR                 0x20
#endif //ELAN_I2C_SLAVE_ADDR

// Calibration Response Timeout
#ifndef ELAN_READ_CALI_RESP_TIMEOUT_MSEC
#define ELAN_READ_CALI_RESP_TIMEOUT_MSEC    10000 //30000
#endif //ELAN_READ_CALI_RESP_TIMEOUT_MSEC

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

// Solution ID
int get_solution_id(unsigned char *p_solution_id);

// Test Version
int send_test_version_command(void);
int read_test_version_data(void);
int get_test_version_data(unsigned short *p_test_version);

// Boot Code Version
int send_boot_code_version_command(void);
int read_boot_code_version_data(void);
int get_boot_code_version_data(unsigned short *p_bc_version);

// Calibration
int send_rek_command(void);
int receive_rek_response(void);

// Calibration Counter
int send_rek_counter_command(void);
int receive_rek_counter_data(unsigned short *p_rek_counter);

// Test Mode
int send_enter_test_mode_command(void);
int send_exit_test_mode_command(void);

// ROM Data
int send_read_rom_data_command(unsigned short addr, bool recovery, unsigned char info);
int receive_rom_data(unsigned short *p_rom_data);

// Bulk ROM Data
int send_show_bulk_rom_data_command(unsigned short addr, unsigned short len);

// Bulk ROM Data (in Boot Code)
int send_show_bulk_rom_data_command(unsigned short addr);
int receive_bulk_rom_data(unsigned short *p_rom_data);

// IAP Mode
int send_write_flash_key_command(void);
int send_enter_iap_command(void);
int send_slave_address(void);

// Frame Data
int write_frame_data(int data_offset, int data_len, unsigned char *frame_buf, int frame_buf_size);

// Flash Write
int send_flash_write_command(void);
int receive_flash_write_response(void);

// Hello Packet
int send_request_hello_packet_command(void);

#endif //__ELAN_TS_HID_UTILITY_H__
