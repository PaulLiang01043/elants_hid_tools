/** @file

  Implementation of Function APIs for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Module Name:
	ElanTsFuncApi.cpp

  Environment:
	All kinds of Linux-like Platform.

  Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
**/

#include <time.h>       		/* time_t, struct tm, time, localtime, asctime */
#include "InterfaceGet.h"
#include "ElanTsHidUtility.h"
#include "ElanTsFuncApi.h"

/***************************************************
 * Global Variable Declaration
 ***************************************************/

/***************************************************
 * Function Implements
 ***************************************************/

int get_firmware_id(unsigned short *p_fw_id)
{
    int err = ERR_SUCCESS;
    unsigned short fw_id = 0;

    err = send_fw_id_command();
    if(err != ERR_SUCCESS)
        goto GET_FIRMWARE_ID_EXIT;

    err = get_fw_id_data(&fw_id);
    if(err != ERR_SUCCESS)
        goto GET_FIRMWARE_ID_EXIT;

    *p_fw_id = fw_id;
    err = ERR_SUCCESS;

GET_FIRMWARE_ID_EXIT:
    return err;
}

int get_fw_version(unsigned short *p_fw_version)
{
    int err = ERR_SUCCESS;
    unsigned short fw_version = 0;

    err = send_fw_version_command();
    if(err != ERR_SUCCESS)
        goto GET_FW_VERSION_EXIT;

    err = get_fw_version_data(&fw_version);
    if(err != ERR_SUCCESS)
        goto GET_FW_VERSION_EXIT;

    *p_fw_version = fw_version;
    err = ERR_SUCCESS;

GET_FW_VERSION_EXIT:
    return err;
}

int get_test_version(unsigned short *p_test_version)
{
    int err = ERR_SUCCESS;
    unsigned short test_version = 0;

    err = send_test_version_command();
    if(err != ERR_SUCCESS)
        goto GET_TEST_VERSION_EXIT;

    err = get_test_version_data(&test_version);
    if(err != ERR_SUCCESS)
        goto GET_TEST_VERSION_EXIT;

    *p_test_version = test_version;
    err = ERR_SUCCESS;

GET_TEST_VERSION_EXIT:
    return err;
}

// Solution ID
int get_solution_id(unsigned char *p_solution_id)
{
    int err = ERR_SUCCESS;
    unsigned short fw_version = 0;
    unsigned char  solution_id = 0;

    // Check if Parameter Invalid
    if (p_solution_id == NULL)
    {
        ERROR_PRINTF("%s: Invalid Parameter! (p_solution_id=0x%p)\r\n", __func__, p_solution_id);
        err = ERR_INVALID_PARAM;
        goto GET_SOLUTION_ID_EXIT;
    }

    // Get Firmware Version
    err = get_fw_version(&fw_version);
    if(err != ERR_SUCCESS)
    {
        ERROR_PRINTF("%s: Fail to Get FW Version! err=0x%x.\r\n", __func__, err);
        goto GET_SOLUTION_ID_EXIT;
    }
    DEBUG_PRINTF("%s: FW Version: 0x%04x.\r\n", __func__, fw_version);

    // Get Solution ID
    solution_id = HIGH_BYTE(fw_version);
    DEBUG_PRINTF("Solution ID: 0x%02x.\r\n", solution_id);

    *p_solution_id = solution_id;
    err = ERR_SUCCESS;

GET_SOLUTION_ID_EXIT:
    return err;
}

int get_boot_code_version(unsigned short *p_bc_version)
{
    int err = ERR_SUCCESS;
    unsigned short bc_version = 0;

    err = send_boot_code_version_command();
    if(err != ERR_SUCCESS)
        goto GET_BOOT_CODE_VERSION_EXIT;

    err = get_boot_code_version_data(&bc_version);
    if(err != ERR_SUCCESS)
        goto GET_BOOT_CODE_VERSION_EXIT;

    *p_bc_version = bc_version;
    err = ERR_SUCCESS;

GET_BOOT_CODE_VERSION_EXIT:
    return err;
}

// Hello Packet & BC Version
int get_hello_packet_bc_version(unsigned char *p_hello_packet, unsigned short *p_bc_version)
{
    int err = ERR_SUCCESS;
    unsigned char hello_packet[4] = {0};
    unsigned short bc_version = 0;

    // Make Sure Page Data Buffer Valid
    if(p_hello_packet == NULL)
    {
        ERROR_PRINTF("%s: NULL Page Data Buffer!\r\n", __func__);
        err = ERR_INVALID_PARAM;
        goto GET_HELLO_PACKET_BC_VERSION_EXIT;
    }

    // Send 7-bit I2C Slave Address
    err = send_request_hello_packet_command();
    if(err != ERR_SUCCESS)
    {
        ERROR_PRINTF("%s: Fail to Send Request Hello Packet Command! err=0x%x.\r\n", __func__, err);
        goto GET_HELLO_PACKET_BC_VERSION_EXIT;
    }

    // Receive Hello Packet
    err = read_data(hello_packet, sizeof(hello_packet), ELAN_READ_DATA_TIMEOUT_MSEC);
    if(err == ERR_IO_TIMEOUT)
    {
        DEBUG_PRINTF("%s: Fail to Receive Hello Packet! Timeout!\r\n", __func__);
        goto GET_HELLO_PACKET_BC_VERSION_EXIT;
    }
    if(err != ERR_SUCCESS)
    {
        ERROR_PRINTF("%s: Fail to Receive Hello Packet! err=0x%x.\r\n", __func__, err);
        goto GET_HELLO_PACKET_BC_VERSION_EXIT;
    }
    DEBUG_PRINTF("vendor_cmd_data: %02x %02x %02x %02x.\r\n", hello_packet[0], hello_packet[1], hello_packet[2], hello_packet[3]);

    // Hello Packet
    DEBUG_PRINTF("Hello Packet: %02x.\r\n", hello_packet[0]);
    *p_hello_packet = hello_packet[0];

    // BC Version
    bc_version = (hello_packet[2] << 8) | hello_packet[3];
    DEBUG_PRINTF("BC Version: %04x.\r\n", bc_version);
    *p_bc_version = bc_version;

    // Success
    err = ERR_SUCCESS;

GET_HELLO_PACKET_BC_VERSION_EXIT:
    return err;
}

int get_hello_packet_bc_version_with_error_retry(unsigned char *p_hello_packet, unsigned short *p_bc_version, int retry_count)
{
    int err = ERR_SUCCESS,
        retry_index = 0;

    // Make Sure Page Data Buffer Valid
    if(p_hello_packet == NULL)
    {
        ERROR_PRINTF("%s: NULL Page Data Buffer!\r\n", __func__);
        err = ERR_INVALID_PARAM;
        goto GET_HELLO_PACKET_BC_VERSION_WITH_ERROR_RETRY_EXIT;
    }

    // Make Sure Retry Count Positive
    if(retry_count <= 0)
        retry_count = 1;

    for(retry_index = 0; retry_index < retry_count; retry_index++)
    {
        err = get_hello_packet_bc_version(p_hello_packet, p_bc_version);
        if(err == ERR_SUCCESS)
        {
            // Without any error => Break retry loop and continue.
            break;
        }

        // With Error => Retry at most 3 times
        DEBUG_PRINTF("%s: [%d/3] Fail to Get Hello Packet (& BC Version)! err=0x%x.\r\n", __func__, retry_index+1, err);
        if(retry_index == 2)
        {
            // Have retried for 3 times and can't fix it => Stop this function
            ERROR_PRINTF("%s: Fail to Get Hello Packet (& BC Version)! err=0x%x.\r\n", __func__, err);
            goto GET_HELLO_PACKET_BC_VERSION_WITH_ERROR_RETRY_EXIT;
        }
        else // retry_index = 0, 1
        {
            // wait 50ms
            usleep(50*1000);

            continue;
        }
    }

GET_HELLO_PACKET_BC_VERSION_WITH_ERROR_RETRY_EXIT:
    return err;
}

int get_hello_packet_with_error_retry(unsigned char *p_hello_packet, int retry_count)
{
    int err = ERR_SUCCESS,
        retry_index = 0;
    unsigned short bc_bc_version = 0;

    // Make Sure Page Data Buffer Valid
    if(p_hello_packet == NULL)
    {
        ERROR_PRINTF("%s: NULL Page Data Buffer!\r\n", __func__);
        err = ERR_INVALID_PARAM;
        goto GET_HELLO_PACKET_WITH_ERROR_RETRY_EXIT;
    }

    // Make Sure Retry Count Positive
    if(retry_count <= 0)
        retry_count = 1;

    for(retry_index = 0; retry_index < retry_count; retry_index++)
    {
        err = get_hello_packet_bc_version(p_hello_packet, &bc_bc_version);
        if(err == ERR_SUCCESS)
        {
            // Without any error => Break retry loop and continue.
            break;
        }

        // With Error => Retry at most 3 times
        DEBUG_PRINTF("%s: [%d/3] Fail to Get Hello Packet! err=0x%x.\r\n", __func__, retry_index+1, err);
        if(retry_index == 2)
        {
            // Have retried for 3 times and can't fix it => Stop this function
            ERROR_PRINTF("%s: Fail to Get Hello Packet! err=0x%x.\r\n", __func__, err);
            goto GET_HELLO_PACKET_WITH_ERROR_RETRY_EXIT;
        }
        else // retry_index = 0, 1
        {
            // wait 50ms
            usleep(50*1000);

            continue;
        }
    }

GET_HELLO_PACKET_WITH_ERROR_RETRY_EXIT:
    return err;
}

// ROM Data
int get_rom_data(unsigned short addr, bool recovery, unsigned short *p_data)
{
    int err = ERR_SUCCESS;
    unsigned short bc_bc_version = 0,
                   word_data = 0;
    unsigned char  hello_packet = 0,
                   solution_id = 0,
                   bc_version_high_byte = 0;

    // Check if Parameter Invalid
    if (p_data == NULL)
    {
        ERROR_PRINTF("%s: Invalid Parameter! (p_data=0x%p)\r\n", __func__, p_data);
        err = ERR_INVALID_PARAM;
        goto GET_ROM_DATA_EXIT;
    }

    if(!recovery) // Normal Mode
    {
        // Solution ID
        err = get_solution_id(&solution_id);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("%s: Fail to Get Solution ID! err=0x%x.\r\n", __func__, err);
            goto GET_ROM_DATA_EXIT;
        }
        DEBUG_PRINTF("%s: [Normal Mode] Solution ID: 0x%02x.\r\n", __func__, solution_id);
    }
    else // Recovery Mode
    {
        // BC Version (Recovery Mode)
        err = get_hello_packet_bc_version(&hello_packet, &bc_bc_version);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("%s: Fail to Get BC Version (Recovery Mode)! err=0x%x.\r\n", __func__, err);
            goto GET_ROM_DATA_EXIT;
        }
        DEBUG_PRINTF("%s: [Recovery Mode] BC Version: 0x%04x.\r\n", __func__, bc_bc_version);
    }

    /* Read Data from ROM */
    err = send_read_rom_data_command(addr, recovery, (recovery) ? bc_version_high_byte : solution_id);
    if(err != ERR_SUCCESS)
    {
        ERROR_PRINTF("%s: Fail to Send Read ROM Data Command! err=0x%x.\r\n", __func__, err);
        goto GET_ROM_DATA_EXIT;
    }

    err = receive_rom_data(&word_data);
    if(err != ERR_SUCCESS)
    {
        ERROR_PRINTF("%s: Fail to Receive ROM Data! err=0x%x.\r\n", __func__, err);
        goto GET_ROM_DATA_EXIT;
    }

    *p_data = word_data;
    err = ERR_SUCCESS;

GET_ROM_DATA_EXIT:
    return err;
}

// Bulk ROM Data
int get_bulk_rom_data(unsigned short addr, unsigned short *p_data)
{
    int err = ERR_SUCCESS;
    unsigned short data = 0;

    // Check if Parameter Invalid
    if (p_data == NULL)
    {
        ERROR_PRINTF("%s: Invalid Parameter! (p_data=0x%p)\r\n", __func__, p_data);
        err = ERR_INVALID_PARAM;
        goto GET_BULK_ROM_DATA_EXIT;
    }

    err = send_show_bulk_rom_data_command(addr);
    if(err != ERR_SUCCESS)
    {
        ERROR_PRINTF("%s: Fail to Send Show Bulk ROM Data Command (addr=0x%04x)! err=0x%x.\r\n", __func__, addr, err);
        goto GET_BULK_ROM_DATA_EXIT;
    }

    err = receive_bulk_rom_data(&data);
    if(err != ERR_SUCCESS)
    {
        ERROR_PRINTF("%s: Fail to Receive Bulk ROM Data! err=0x%x.\r\n", __func__, err);
        goto GET_BULK_ROM_DATA_EXIT;
    }

    *p_data = data;
    err = ERR_SUCCESS;

GET_BULK_ROM_DATA_EXIT:
    return err;
}

// Information FWID
int read_info_fwid(unsigned short *p_info_fwid, bool recovery)
{
    int err = ERR_SUCCESS;
    unsigned short info_fwid = 0;

    // Check if Parameter Invalid
    if (p_info_fwid == NULL)
    {
        ERROR_PRINTF("%s: Invalid Parameter! (p_info_fwid=0x%p)\r\n", __func__, p_info_fwid);
        err = ERR_INVALID_PARAM;
        goto READ_INFO_FWID_EXIT;
    }

    // Get FWID from ROM
    if(recovery) // Recovery Mode
    {
        // Read FWID from ROM (with bulk rom data command)
        // [Note] Paul @ 20191025
        // Since Read ROM Command only supported by main code,
        //   we can only use Show Bulk ROM Data Command (0x59) in recovery mode (boot code stage).
        err = get_bulk_rom_data(ELAN_INFO_ROM_FWID_MEMORY_ADDR, &info_fwid);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("%s: Fail to Get Bulk ROM Data Command! err=0x%x.\r\n", __func__, err);
            goto READ_INFO_FWID_EXIT;
        }
    }
    else // Normal Mode
    {
        /* Read FWID from ROM */
        // [Note] Paul @ 20191025
        // In normal mode, just use Read ROM Command (0x96).
        err = get_rom_data(ELAN_INFO_ROM_FWID_MEMORY_ADDR, recovery, &info_fwid);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("%s: Fail to Get ROM Data! err=0x%x.\r\n", __func__, err);
            goto READ_INFO_FWID_EXIT;
        }
    }

    *p_info_fwid = info_fwid;
    err = ERR_SUCCESS;

READ_INFO_FWID_EXIT:
    return err;
}

