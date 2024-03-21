/** @file

  Header of HID Device Utility for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Copyright (c) ELAN microelectronics corp. 2022, All Rights Reserved

  Module Name:
	ElanTsHidDevUtility.h

  Environment:
	All kinds of Linux-like Platform.

********************************************************************
 Revision History

**/

#ifndef __ELAN_TS_HID_DEVICE_UTILITY_H__
#define __ELAN_TS_HID_DEVICE_UTILITY_H__
#pragma once

#include <linux/hidraw.h>	/* hidraw */
#include "HidConfig.h"
#include "ElanTsDebug.h"

/*******************************************
 * Definitions
 ******************************************/

// Device Information Set
#ifndef DEV_INFO_SET_MAX
#define DEV_INFO_SET_MAX		100
#endif //DEV_INFO_SET_MAX

/*******************************************
 * Global Data Structure Declaration
 ******************************************/

/*******************************************
 * Global Variables Declaration
 ******************************************/

/*******************************************
 * Extern Variables Declaration
 ******************************************/

/*******************************************
 * Function Prototype
 ******************************************/

// HID Device Info.
const char *bus_str(int bus);
int get_hid_dev_info(struct hidraw_devinfo *p_hid_dev_info, size_t dev_info_size);
int show_hid_dev_info(struct hidraw_devinfo *p_hid_dev_info, size_t dev_info_size);

// Validate Elan Device
int validate_elan_hid_device(struct hidraw_devinfo *p_hid_dev_info, size_t dev_info_size, int pid, bool silent_mode);

#endif //__ELAN_TS_HID_DEVICE_UTILITY_H__
