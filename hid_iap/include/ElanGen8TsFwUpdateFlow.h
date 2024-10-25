/** @file

  Header of APIs of Firmware Update Flow for Elan Gen8 HID (I2C-HID / SPI-HID) Touchscreen.

  Module Name:
	ElanGen8TsFwUpdateFlow.h

  Environment:
	All kinds of Linux-like Platform.

  Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
********************************************************************
 Revision History

**/

#ifndef __ELAN_GEN8_TS_FW_UPDATE_FLOW_H__
#define __ELAN_GEN8_TS_FW_UPDATE_FLOW_H__
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ElanTsFwUpdateFlow.h" // message_mode_t

/***************************************************
 * Definitions
 ***************************************************/

#if 0 //ndef __ENABLE_GEN8_REMARK_ID_CHECK__
#define __ENABLE_GEN8_REMARK_ID_CHECK__
#endif //__ENABLE_GEN8_REMARK_ID_CHECK__

/***************************************************
 * Extern Variables Declaration
 ***************************************************/

/***************************************************
 * Function Prototype
 ***************************************************/

// Firmware Information
int gen8_get_firmware_information(message_mode_t msg_mode);

// Calibration Counter
int gen8_get_calibration_counter(message_mode_t msg_mode);

// Remark ID Check
int gen8_check_remark_id(bool recovery);

// Firmware Update
int gen8_update_firmware(char *filename, size_t filename_len, bool recovery, int skip_action_code);

#endif //__ELAN_GEN8_TS_FW_UPDATE_FLOW_H__
