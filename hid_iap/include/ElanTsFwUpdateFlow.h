/** @file

  Header of APIs of Firmware Update Flow for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Module Name:
	ElanTsFwUpdateFlow.h

  Environment:
	All kinds of Linux-like Platform.

  Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
********************************************************************
 Revision History

**/

#ifndef __ELAN_TS_FW_UPDATE_FLOW_H__
#define __ELAN_TS_FW_UPDATE_FLOW_H__
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/***************************************************
 * Definitions
 ***************************************************/

/*
 * Action Code of Firmware Update
 */

// Remark ID Check
#ifndef ACTION_CODE_REMARK_ID_CHECK
#define ACTION_CODE_REMARK_ID_CHECK		0x01
#endif // ACTION_CODE_REMARK_ID_CHECK

// Update Information Section
#ifndef ACTION_CODE_INFORMATION_UPDATE
#define ACTION_CODE_INFORMATION_UPDATE	0x02
#endif // ACTION_CODE_INFORMATION_UPDATE

/*******************************************
 * Data Structure Declaration
 ******************************************/

/*
 * Message MODE DEFINITION
 */
enum message_mode
{
    FULL_MESSAGE    = 0,    // Full Message Output
    SILENT_MODE     = 1,    // Only Key Value Output
    NO_MESSAGE      = 2     // No Message Ouput
};
typedef enum message_mode message_mode_t;

/***************************************************
 * Extern Variables Declaration
 ***************************************************/

/***************************************************
 * Function Prototype
 ***************************************************/

// Firmware Information
int get_firmware_information(message_mode_t msg_mode);

// Calibration Counter
int get_calibration_counter(message_mode_t msg_mode);

// Remark ID Check
int check_remark_id(bool recovery);

// Firmware Update
int update_firmware(char *filename, size_t filename_len, bool recovery, int skip_action_code);

#endif //__ELAN_TS_FW_UPDATE_FLOW_H__
