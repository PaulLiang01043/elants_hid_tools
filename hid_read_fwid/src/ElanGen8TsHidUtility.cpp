/** @file

  Implementation of Function Utility for Elan Gen8 HID (I2C-HID / SPI-HID) Touchscreen.

  Copyright (c) ELAN microelectronics corp. 2022, All Rights Reserved

  Module Name:
	ElanGen8TsHidUtility.cpp

  Environment:
	All kinds of Linux-like Platform.

**/

#include "HIDLinuxGet.h"
#include "ElanGen8TsHidUtility.h"

/***************************************************
 * TP Functions
 ***************************************************/

// ROM Data
int gen8_send_read_rom_data_command(unsigned int addr, unsigned char data_len)
{
    int err = ERR_SUCCESS;
    unsigned char new_read_rom_data_cmd[10] =  {0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} /* Read 32-bit RAM/ROM Data Command */;

    // Check if Parameter Invalid
    if ((data_len != 1) && (data_len != 2) && (data_len != 4))
    {
        ERROR_PRINTF("%s: Invalid Parameter! (data_len=%d)\r\n", __func__, data_len);
        err = ERR_INVALID_PARAM;
        goto GEN8_SEND_READ_ROM_DATA_COMMAND_EXIT;
    }

    /* Set Address & Length */

    // Set Length
    new_read_rom_data_cmd[1] = data_len;

    // Set Address
    new_read_rom_data_cmd[2] = (unsigned char)((addr & 0xFF000000) >> 24); //ADDR_3, ex: 0x00
    new_read_rom_data_cmd[3] = (unsigned char)((addr & 0x00FF0000) >> 16); //ADDR_2, ex: 0x04
    new_read_rom_data_cmd[4] = (unsigned char)((addr & 0x0000FF00) >>  8); //ADDR_1, ex: 0x00
    new_read_rom_data_cmd[5] = (unsigned char)((addr & 0x000000FF)      ); //ADDR_0, ex: 0x00

    /* Send Read 32-bit RAM/ROM Data Command */
    DEBUG_PRINTF("cmd: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x.\r\n", \
                 new_read_rom_data_cmd[0], new_read_rom_data_cmd[1], \
                 new_read_rom_data_cmd[2], new_read_rom_data_cmd[3], new_read_rom_data_cmd[4], new_read_rom_data_cmd[5], \
                 new_read_rom_data_cmd[6], new_read_rom_data_cmd[7], new_read_rom_data_cmd[8], new_read_rom_data_cmd[9]);
    err = write_cmd(new_read_rom_data_cmd, sizeof(new_read_rom_data_cmd), ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("%s: Fail to send Read 32-bit RAM/ROM Data Command! err=0x%x.\r\n", __func__, err);
        goto GEN8_SEND_READ_ROM_DATA_COMMAND_EXIT;
    }

    // Success
    err = ERR_SUCCESS;

GEN8_SEND_READ_ROM_DATA_COMMAND_EXIT:
    return err;
}

int gen8_receive_rom_data(unsigned int *p_rom_data)
{
    int err = ERR_SUCCESS;
    unsigned char cmd_data[10] = {0};
    unsigned short rom_data = 0;

    // Check if Parameter Invalid
    if (p_rom_data == NULL)
    {
        ERROR_PRINTF("%s: Invalid Parameter! (p_rom_data=0x%p)\r\n", __func__, p_rom_data);
        err = ERR_INVALID_PARAM;
        goto GEN8_RECEIVE_ROM_DATA_EXIT;
    }

    // Read 10-byte Command Data
    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if(err != ERR_SUCCESS) // Error or Timeout
    {
        ERROR_PRINTF("Fail to receive ROM data! err=0x%x.\r\n", err);
        goto GEN8_RECEIVE_ROM_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x.\r\n", \
                 cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3], cmd_data[4], cmd_data[5], \
                 cmd_data[6], cmd_data[7], cmd_data[8], cmd_data[9]);

    /* Check if data invalid */
    //if ((cmd_data[0] != 0x95) || (cmd_data[1] != 0x04))
    if ( (cmd_data[0] != 0x95) || \
            ((cmd_data[1] != 0x01) && (cmd_data[1] != 0x02) && (cmd_data[1] != 0x04)))
    {
        err = ERR_DATA_PATTERN;
        ERROR_PRINTF("%s: Data Format Invalid! (cmd_data[0]=0x%02x, cmd_data[1]=0x%02x), err=0x%x.\r\n", \
                     __func__, cmd_data[0], cmd_data[1], err);
        goto GEN8_RECEIVE_ROM_DATA_EXIT;
    }

    // Load 4-byte ROM Data to Input Buffer
    rom_data = (unsigned int)((cmd_data[6] << 24) | (cmd_data[7] << 16) | (cmd_data[8] << 8) | cmd_data[9]);
    DEBUG_PRINTF("ROM Data: 0x%08x.\r\n", rom_data);
    *p_rom_data = rom_data;

    // Success
    err = ERR_SUCCESS;

GEN8_RECEIVE_ROM_DATA_EXIT:
    return err;
}

