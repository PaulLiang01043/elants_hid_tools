/** @file

  Header of EDID Utility for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Copyright (c) ELAN microelectronics corp. 2022, All Rights Reserved

  Module Name:
	ElanTsEdidUtility.h

  Environment:
	All kinds of Linux-like Platform.

********************************************************************
 Revision History

**/

#ifndef __ELAN_TS_EDID_UTILITY_H__
#define __ELAN_TS_EDID_UTILITY_H__
#pragma once

#include "ElanTsDebug.h" 

/*******************************************
 * Definitions
 ******************************************/

// Path of modetest command
#ifndef MODETEST_CMD
#define MODETEST_CMD            "/usr/bin/modetest -a"
#endif //MODETEST_CMD

// EDID Header
#ifndef STANDARD_EDID_HEADER
#define STANDARD_EDID_HEADER    "00ffffffffffff00"
#endif //STANDARD_EDID_HEADER

#ifndef AUO_EDID_HEADER
#define AUO_EDID_HEADER         "b36f0200b004ec04"
#endif //AUO_EDID_HEADER

#ifndef BOE_EDID_HEADER
#define BOE_EDID_HEADER         "ac700200b0040005"
#endif //BOE_EDID_HEADER

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

// EDID Info.
int get_edid_manufacturer_product_code(unsigned short *p_manufacturer_code, unsigned short *p_product_code);
int show_edid_manufacturer_product_code(unsigned short manufacturer_code, unsigned short product_code);

#endif //__ELAN_TS_EDID_UTILITY_H__
