#ifndef __HID_CONFIG_H__
#define __HID_CONFIG_H__
#pragma once

//////////////////////////////////////////////////////////////
// Definition
//////////////////////////////////////////////////////////////

/*
 * VID & PID
 */

// ELAN Default VID
#ifndef ELAN_HID_VID
#define ELAN_HID_VID                  0x04F3
#endif //ELAN_HID_VID

// ELAN Recovery PID
#ifndef ELAN_HID_RECOVERY_PID
#define ELAN_HID_RECOVERY_PID         0x0732
#endif //ELAN_HID_RECOVERY_PID

// PID for Default Connect
#ifndef ELAN_HID_FORCE_CONNECT_PID
#define ELAN_HID_FORCE_CONNECT_PID    0x0
#endif //ELAN_HID_FORCE_CONNECT_PID

/*              ELAN Output Buffer Format                       *
 *                                                              *
 *    | Report ID (0x03, 1-byte) | Output Report (32-byte) |	  *
 *                                                              *
 *              ELAN Input Buffer Format                        *
 *                                                              *
 *    | Report ID (0x02, 1-byte) | Input Report (64-byte)  |	  *
 */

/*
 * HID Report ID
 */

// In Report
#ifndef ELAN_HID_INPUT_REPORT_ID
#define ELAN_HID_INPUT_REPORT_ID    0x2
#endif//ELAN_HID_INPUT_REPORT_ID

// Out Report
#ifndef ELAN_HID_OUTPUT_REPORT_ID
#define ELAN_HID_OUTPUT_REPORT_ID    0x3
#endif//ELAN_HID_OUTPUT_REPORT_ID

// Finger Report
#ifndef ELAN_HID_FINGER_REPORT_ID
#define ELAN_HID_FINGER_REPORT_ID    0x1
#endif//ELAN_HID_FINGER_REPORT_ID

// Pen Report
#ifndef ELAN_HID_PEN_REPORT_ID
#define ELAN_HID_PEN_REPORT_ID          0x7
#endif //ELAN_HID_PEN_REPORT_ID	

// Pen Debug Report
#ifndef ELAN_HID_PEN_DEBUG_REPORT_ID
#define ELAN_HID_PEN_DEBUG_REPORT_ID	0x17
#endif //ELAN_HID_PEN_DEBUG_REPORT_ID

// For I2C FW with M680 Bridge (PID 0xb)
#ifndef ELAN_HID_OUTPUT_REPORT_ID_PID_B
#define ELAN_HID_OUTPUT_REPORT_ID_PID_B    0x0
#endif//ELAN_HID_OUTPUT_REPORT_ID_PID_B

#ifndef ELAN_HID_INPUT_REPORT_ID_PID_B
#define ELAN_HID_INPUT_REPORT_ID_PID_B    0x0
#endif//ELAN_HID_INPUT_REPORT_ID_PID_B

// For I2C/I2CHID FW with Universal Bridge (PID 0x7)
#ifndef ELAN_HID_OUTPUT_REPORT_ID_PID_7
#define ELAN_HID_OUTPUT_REPORT_ID_PID_7    0x0
#endif//ELAN_HID_OUTPUT_REPORT_ID_PID_7

#ifndef ELAN_HID_INPUT_REPORT_ID_PID_7
#define ELAN_HID_INPUT_REPORT_ID_PID_7     0x0
#endif//ELAN_HID_INPUT_REPORT_ID_PID_7

/*
 * Buffer Size
 */

// Out Report Size / Output Report Buffer Size
//  for I2C-HID / SPI-HID Interface
#ifndef ELAN_HID_OUTPUT_BUFFER_SIZE
#define ELAN_HID_OUTPUT_BUFFER_SIZE      33 // 0x21
#endif //ELAN_HID_OUTPUT_BUFFER_SIZE

/* Elan-TS HID Input Report Data Format (63-byte)  *
 * | Report ID | Data Length |  Data   |           *
 * |   0x02    |  $(length)  | ${data} |           *
 */

// In Report Size / Input Report Buffer Size
//  for I2C-HID / SPI-HID Interface
#ifndef ELAN_HID_INPUT_BUFFER_SIZE
#define ELAN_HID_INPUT_BUFFER_SIZE       65 // 0x41
#endif//ELAN_HID_INPUT_BUFFER_SIZE

// In Report Data Size / Input Report Data Size
#ifndef ELAN_HID_DATA_BUFFER_SIZE
#define ELAN_HID_DATA_BUFFER_SIZE        ((ELAN_HID_INPUT_BUFFER_SIZE) - 2) /* $(ELAN_HID_INPUT_BUFFER_SIZE) - 1 (Report ID) - 1(Data_Length)  = 63 Byte */
#endif //ELAN_HID_DATA_BUFFER_SIZE

// ELAN HID Buffer Size for IAP
#ifndef ELAN_HID_PAGE_FRAME_SIZE
#define ELAN_HID_PAGE_FRAME_SIZE         0x1C /* 33-3(3-Byte Vendor Command)-1(ReportID)=29 Byte=>28 Byte(14Word)*/
#endif //ELAN_HID_PAGE_FRAME_SIZE

// ELAN HID Frame Size for Page Read
#ifndef ELAN_HID_READ_PAGE_FRAME_SIZE
#define ELAN_HID_READ_PAGE_FRAME_SIZE    0x3C /* 63 - 1(Packet Header 0x99) - 1(Packet Index) -1(Data Length) = 60 Byte */
#endif //ELAN_HID_READ_PAGE_FRAME_SIZE

/*
 * Bridge Commands
 */

// Power Down & Reset
#ifndef BRIDGE_COMMAND_PHY_POWER_DOWN_AND_RESET
#define BRIDGE_COMMAND_PHY_POWER_DOWN_AND_RESET    0x11
#endif//BRIDGE_COMMAND_PHY_POWER_DOWN_AND_RESET

// Receive Page
#ifndef BRIDGE_COMMAND_RECEIVE_ONE_PAGE_DATA
#define BRIDGE_COMMAND_RECEIVE_ONE_PAGE_DATA    0x21
#endif//BRIDGE_COMMAND_RECEIVE_ONE_PAGE_DATA

// Write Flash
#ifndef BRIDGE_COMMAND_WRITE_FLASH_AND_RESPONSE_FA
#define BRIDGE_COMMAND_WRITE_FLASH_AND_RESPONSE_FA    0x22
#endif//BRIDGE_COMMAND_WRITE_FLASH_AND_RESPONSE_FA

#endif //__HID_CONFIG_H__
