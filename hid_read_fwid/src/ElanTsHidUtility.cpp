/** @file

  Implementation of Function Utility for Elan HID (I2C-HID / SPI-HID) Touchscreen.

  Copyright (c) ELAN microelectronics corp. 2019, All Rights Reserved

  Module Name:
	ElanTsHidUtility.cpp

  Environment:
	All kinds of Linux-like Platform.

**/

#include "HIDLinuxGet.h"
#include "ElanTsHidUtility.h"

/***************************************************
 * TP Functions
 ***************************************************/

// FW ID
int send_fw_id_command(void)
{
    int err = ERR_SUCCESS;
    unsigned char fw_id_cmd[4] = {0x53, 0xf0, 0x00, 0x01};


    /* Send FW ID Command to touch */
    DEBUG_PRINTF("cmd: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", fw_id_cmd[0], fw_id_cmd[1], fw_id_cmd[2], fw_id_cmd[3]);
    err = write_cmd(fw_id_cmd, sizeof(fw_id_cmd), ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
        ERROR_PRINTF("Fail to send FW ID command! err=0x%x.\r\n", err);

    return err;
}

int read_fw_id_data(void)
{
    int err = ERR_SUCCESS,
        major_fw_id = 0,
        minor_fw_id = 0;
    unsigned char cmd_data[4] = {0};

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to read FW ID data, err=0x%x.\n", err);
        goto READ_FW_ID_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);

    /* Check if Data is Firmware ID */
    if ((cmd_data[0] == 0x52) && (((cmd_data[1] & 0xf0) >> 4) == 0xf))
    {
        major_fw_id = ((cmd_data[1] & 0x0f) << 4) | ((cmd_data[2] & 0xf0) >> 4);
        minor_fw_id = ((cmd_data[2] & 0x0f) << 4) | ((cmd_data[3] & 0xf0) >> 4);
        printf("Firmware ID: %02x.%02x\r\n", major_fw_id, minor_fw_id);
    }

    err = ERR_SUCCESS;

READ_FW_ID_DATA_EXIT:
    return err;
}

int get_fw_id_data(unsigned short *p_fw_id)
{
    int err = ERR_SUCCESS,
        major_fw_id = 0,
        minor_fw_id = 0;
    unsigned short fw_id = 0;
    unsigned char cmd_data[4] = {0};

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to read FW ID data, err=0x%x.\n", err);
        goto GET_FW_ID_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);

    /* Check if Data is Firmware ID */
    if ((cmd_data[0] != 0x52) || (((cmd_data[1] & 0xf0) >> 4) != 0xf))
    {
        err = ERR_DATA_PATTERN;
        ERROR_PRINTF("Invalid Data Format (%02x %02x), err=0x%x.\r\n", cmd_data[0], cmd_data[1], err);
        goto GET_FW_ID_DATA_EXIT;
    }

    major_fw_id = ((cmd_data[1] & 0x0f) << 4) | ((cmd_data[2] & 0xf0) >> 4);
    minor_fw_id = ((cmd_data[2] & 0x0f) << 4) | ((cmd_data[3] & 0xf0) >> 4);
    fw_id = (major_fw_id << 8) | minor_fw_id;
    DEBUG_PRINTF("fw_id: %04x\r\n", fw_id);

    *p_fw_id = fw_id;
    err = ERR_SUCCESS;

GET_FW_ID_DATA_EXIT:
    return err;
}

// FW Version
int send_fw_version_command(void)
{
    int err = ERR_SUCCESS;
    unsigned char fw_ver_cmd[4] = {0x53, 0x00, 0x00, 0x01};

    /* Send FW ID Command to touch */
    DEBUG_PRINTF("cmd: 0x%02x, 0x%02x, 0x%02x, 0x%02x.\r\n", fw_ver_cmd[0], fw_ver_cmd[1], fw_ver_cmd[2], fw_ver_cmd[3]);
    err = write_cmd(fw_ver_cmd, sizeof(fw_ver_cmd), ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
        ERROR_PRINTF("Fail to send FW Version command! err=0x%x.\r\n", err);

    return err;
}

int read_fw_version_data(bool quiet /* Silent Mode */)
{
    int fw_ver = 0,
        major_fw_ver = 0,
        minor_fw_ver = 0,
        err = ERR_SUCCESS;
    unsigned char cmd_data[4] = {0};

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to read FW Version data, err=0x%x.\r\n", err);
        goto READ_FW_VERSION_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);

    /* Check if Data is Firmware Version */
    if ((cmd_data[0] == 0x52) && (((cmd_data[1] & 0xf0) >> 4) == 0))
    {
        major_fw_ver = ((cmd_data[1] & 0x0f) << 4) | ((cmd_data[2] & 0xf0) >> 4);
        minor_fw_ver = ((cmd_data[2] & 0x0f) << 4) | ((cmd_data[3] & 0xf0) >> 4);
        fw_ver = (major_fw_ver << 8) | minor_fw_ver;
        if(quiet == true) // Enable Silent Mode
            printf("%04x", fw_ver);
        else // Disable Silent Mode
            printf("Firmware Version: %02x.%02x\r\n", major_fw_ver, minor_fw_ver);
    }

    err = ERR_SUCCESS;

READ_FW_VERSION_DATA_EXIT:
    return err;
}

int get_fw_version_data(unsigned short *p_fw_version)
{
    int err = ERR_SUCCESS;
    unsigned short fw_ver = 0,
                   major_fw_ver = 0,
                   minor_fw_ver = 0;
    unsigned char cmd_data[4] = {0};

    /* Check Data Buffer */
    if(p_fw_version == NULL)
    {
        ERROR_PRINTF("%s: NULL Pointer!\r\n", __func__);
        err = ERR_INVALID_PARAM;
        goto GET_FW_VERSION_DATA_EXIT;
    }

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to read FW Version data, err=0x%x.\r\n", err);
        goto GET_FW_VERSION_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%02x, 0x%02x, 0x%02x, 0x%02x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);

    /* Check if Data is Firmware Version */
    if ((cmd_data[0] != 0x52) || (((cmd_data[1] & 0xf0) >> 4) != 0))
    {
        err = ERR_DATA_PATTERN;
        ERROR_PRINTF("Invalid Data Format (%02x %02x), err=0x%x.\r\n", cmd_data[0], cmd_data[1], err);
        goto GET_FW_VERSION_DATA_EXIT;
    }

    major_fw_ver = ((cmd_data[1] & 0x0f) << 4) | ((cmd_data[2] & 0xf0) >> 4);
    minor_fw_ver = ((cmd_data[2] & 0x0f) << 4) | ((cmd_data[3] & 0xf0) >> 4);
    fw_ver = (major_fw_ver << 8) | minor_fw_ver;
    DEBUG_PRINTF("fw_ver: %04x\r\n", fw_ver);

    *p_fw_version = fw_ver;
    err = ERR_SUCCESS;

GET_FW_VERSION_DATA_EXIT:
    return err;
}

int send_test_version_command(void)
{
    int err = ERR_SUCCESS;
    unsigned char test_ver_cmd[4] = {0x53, 0xe0, 0x00, 0x01};

    /* Send Test Version Command to touch */
    DEBUG_PRINTF("cmd: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", test_ver_cmd[0], test_ver_cmd[1], test_ver_cmd[2], test_ver_cmd[3]);
    err = write_cmd(test_ver_cmd, sizeof(test_ver_cmd), ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
        ERROR_PRINTF("Fail to send Test Version command! err=0x%x.\r\n", err);

    return err;
}

// Test Version
int read_test_version_data(void)
{
    int err = ERR_SUCCESS,
        test_ver = 0,
        solution_ver = 0;
    unsigned char cmd_data[4] = {0};

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to receive Test Version data, err=0x%x.\r\n", err);
        goto READ_TEST_VERSION_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);

    /* Check if Data is for Test Version */
    if ((cmd_data[0] == 0x52) && (((cmd_data[1] & 0xf0) >> 4) == 0xe))
    {
        test_ver = ((cmd_data[1] & 0x0f) << 4) | ((cmd_data[2] & 0xf0) >> 4);
        solution_ver = ((cmd_data[2] & 0x0f) << 4) | ((cmd_data[3] & 0xf0) >> 4);
        printf("Test Version: %02x.%02x\r\n", test_ver, solution_ver);
    }

    err = ERR_SUCCESS;

READ_TEST_VERSION_DATA_EXIT:
    return err;
}

int get_test_version_data(unsigned short *p_test_version)
{
    int err = ERR_SUCCESS,
        test_ver = 0,
        solution_ver = 0;
    unsigned short test_solution_ver = 0;
    unsigned char cmd_data[4] = {0};

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to receive Test Version data, err=0x%x.\r\n", err);
        goto GET_TEST_VERSION_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);

    /* Check if Data is for Test Version */
    if ((cmd_data[0] != 0x52) || (((cmd_data[1] & 0xf0) >> 4) != 0xe))
    {
        err = ERR_DATA_PATTERN;
        ERROR_PRINTF("Invalid Data Format (%02x %02x), err=0x%x.\r\n", cmd_data[0], cmd_data[1], err);
        goto GET_TEST_VERSION_DATA_EXIT;
    }

    test_ver = ((cmd_data[1] & 0x0f) << 4) | ((cmd_data[2] & 0xf0) >> 4);
    solution_ver = ((cmd_data[2] & 0x0f) << 4) | ((cmd_data[3] & 0xf0) >> 4);
    test_solution_ver = (test_ver << 8) | solution_ver;
    DEBUG_PRINTF("test_solution_ver: %04x\r\n", test_solution_ver);

    *p_test_version = test_solution_ver;
    err = ERR_SUCCESS;

GET_TEST_VERSION_DATA_EXIT:
    return err;
}

// Boot Code Version
int send_boot_code_version_command(void)
{
    int err = ERR_SUCCESS;
    unsigned char bc_ver_cmd[4] = {0x53, 0x10, 0x00, 0x01};

    /* Send Boot Code Version Command to touch */
    DEBUG_PRINTF("cmd: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", bc_ver_cmd[0], bc_ver_cmd[1], bc_ver_cmd[2], bc_ver_cmd[3]);
    err = write_cmd(bc_ver_cmd, sizeof(bc_ver_cmd), ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
        ERROR_PRINTF("Fail to send Boot Code Version command! err=0x%x.\r\n", err);

    return err;
}

int read_boot_code_version_data(void)
{
    int err = ERR_SUCCESS,
        major_bc_ver = 0,
        minor_bc_ver = 0;
    unsigned char cmd_data[4] = {0};

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to read Boot Code Version data, err=0x%x.\r\n", err);
        goto READ_BOOT_CODE_VERSION_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);

    /* Check if Data is Boot Code Version */
    if ((cmd_data[0] == 0x52) && (((cmd_data[1] & 0xf0) >> 4) == 0x1))
    {
        major_bc_ver = ((cmd_data[1] & 0x0f) << 4) | ((cmd_data[2] & 0xf0) >> 4);
        minor_bc_ver = ((cmd_data[2] & 0x0f) << 4) | ((cmd_data[3] & 0xf0) >> 4);
        printf("Boot Code Version: %02x.%02x\r\n", major_bc_ver, minor_bc_ver);
    }

    err = ERR_SUCCESS;

READ_BOOT_CODE_VERSION_DATA_EXIT:
    return err;
}

int get_boot_code_version_data(unsigned short *p_bc_version)
{
    int err = ERR_SUCCESS;
    unsigned short bc_ver = 0,
                   major_bc_ver = 0,
                   minor_bc_ver = 0;
    unsigned char cmd_data[4] = {0};

    /* Check Data Buffer */
    if(p_bc_version == NULL)
    {
        ERROR_PRINTF("%s: NULL Pointer!\r\n", __func__);
        err = ERR_INVALID_PARAM;
        goto GET_BOOT_CODE_VERSION_DATA_EXIT;
    }

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to read Boot Code Version data, err=0x%x.\r\n", err);
        goto GET_BOOT_CODE_VERSION_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);

    /* Check if Data is Boot Code Version */
    if ((cmd_data[0] != 0x52) || (((cmd_data[1] & 0xf0) >> 4) != 0x1))
    {
        err = ERR_DATA_PATTERN;
        ERROR_PRINTF("Invalid Data Format (%02x %02x), err=0x%x.\r\n", cmd_data[0], cmd_data[1], err);
        goto GET_BOOT_CODE_VERSION_DATA_EXIT;
    }

    major_bc_ver = ((cmd_data[1] & 0x0f) << 4) | ((cmd_data[2] & 0xf0) >> 4);
    minor_bc_ver = ((cmd_data[2] & 0x0f) << 4) | ((cmd_data[3] & 0xf0) >> 4);
    bc_ver = (major_bc_ver << 8) | minor_bc_ver;
    DEBUG_PRINTF("bc_ver: %04x\r\n", bc_ver);

    *p_bc_version = bc_ver;
    err = ERR_SUCCESS;

GET_BOOT_CODE_VERSION_DATA_EXIT:
    return err;
}

// ROM Data
int send_read_rom_data_command(unsigned short addr, bool recovery, unsigned char info)
{
    int err = ERR_SUCCESS;
    unsigned char read_rom_data_cmd[6] =  {0x96, 0x00, 0x00, 0x00, 0x00, 0x11} /* Show Bulk ROM Data Command */,
                  solution_id = 0,
                  bc_version_high_byte = 0;

    // Assign info to appropriate variable
    if(recovery == false) // Normal Mode
    {
        solution_id = info;
    }
    else // Recovery Mode
    {
        bc_version_high_byte = info;
    }

    /* Set Address & Length */
    read_rom_data_cmd[1] = (addr & 0xFF00) >> 8;	//ADDR_H
    read_rom_data_cmd[2] =  addr & 0x00FF; 		    //ADDR_L

    // Information Command Parameter
    // [Note] Paul @ 20191106
    // Since Solution ID (FW Version) is only available in normal mode,
    //   we use high byte of bc_version to decide IC solution of the current touch controller in recovery mode.
    if(recovery == false) // Normal Mode
    {
        if (/* 63XX Solution */
            (solution_id == SOLUTION_ID_EKTH6315x1) || \
            (solution_id == SOLUTION_ID_EKTH6315x2) || \
            (solution_id == SOLUTION_ID_EKTH6315to5015M) || \
            (solution_id == SOLUTION_ID_EKTH6315to3915P) || \
            /* 73XX Solution */
            (solution_id == SOLUTION_ID_EKTH7315x1) || \
            (solution_id == SOLUTION_ID_EKTH7315x2) || \
            (solution_id == SOLUTION_ID_EKTH7318x1))
            read_rom_data_cmd[5] = 0x21; // 63XX or 73XX: byte[5]=0x21 => Read Information
        else
            read_rom_data_cmd[5] = 0x11; // 53XX: byte[5]=0x11 => Read Information
    }
    else // Recovery Mode
    {
        if (/* 63XX Solution */ \
            (bc_version_high_byte == BC_VER_H_BYTE_FOR_EKTA6315_HID) || \
            (bc_version_high_byte == BC_VER_H_BYTE_FOR_EKTA6308_HID) || \
            (bc_version_high_byte == BC_VER_H_BYTE_FOR_EKTH6315_TO_5015M_HID) || \
            (bc_version_high_byte == BC_VER_H_BYTE_FOR_EKTH6315_TO_3915P_HID) || \
            /* 73XX Solution */ \
            (bc_version_high_byte == BC_VER_H_BYTE_FOR_EKTA7315_HID))
            read_rom_data_cmd[5] = 0x21; // 63XX: byte[5]=0x21 => Read Information
        else
            read_rom_data_cmd[5] = 0x11; // 53XX: byte[5]=0x11 => Read Information
    }

    /* Send Show Bulk ROM Data Command */
    DEBUG_PRINTF("cmd: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x.\r\n", \
                 read_rom_data_cmd[0], read_rom_data_cmd[1], read_rom_data_cmd[2], \
                 read_rom_data_cmd[3], read_rom_data_cmd[4], read_rom_data_cmd[5]);
    err = write_cmd(read_rom_data_cmd, 6, ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
        ERROR_PRINTF("Fail to send Read ROM Data command! err=0x%x.\r\n", err);

    return err;
}

int receive_rom_data(unsigned short *p_rom_data)
{
    int err = ERR_SUCCESS;
    unsigned char cmd_data[6] = {0};
    unsigned short rom_data = 0;

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if(err != ERR_SUCCESS) // Error or Timeout
    {
        ERROR_PRINTF("Fail to receive ROM data! err=0x%x.\r\n", err);
        goto RECEIVE_ROM_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3], cmd_data[4], cmd_data[5]);

    /* Check if data invalid */
    if (cmd_data[0] != 0x95)
    {
        err = ERR_DATA_PATTERN;
        ERROR_PRINTF("Data Format Invalid! err=0x%x.\r\n", err);
        goto RECEIVE_ROM_DATA_EXIT;
    }

    // Load ROM Data to Input Buffer
    rom_data = (unsigned short)((cmd_data[3] << 8) | cmd_data[4]);
    DEBUG_PRINTF("ROM Data: 0x%04x.\r\n", rom_data);

    *p_rom_data = rom_data;
    err = ERR_SUCCESS;

RECEIVE_ROM_DATA_EXIT:
    return err;
}

// Bulk ROM Data
int send_show_bulk_rom_data_command(unsigned short addr, unsigned short len)
{
    int err = ERR_SUCCESS;
    unsigned char show_bulk_rom_data_cmd[6] =  {0x59, 0x10, 0x00, 0x00, 0x00, 0x00}; /* Show Bulk ROM Data Command */

    /* Set Address & Length */
    show_bulk_rom_data_cmd[2] = (addr & 0xFF00) >> 8;	//ADDR_H
    show_bulk_rom_data_cmd[3] =  addr & 0x00FF; 		//ADDR_L
    show_bulk_rom_data_cmd[4] = (len  & 0xFF00) >> 8;	//LEN_H
    show_bulk_rom_data_cmd[5] =  len  & 0x00FF;			//LEN_L

    /* Send Show Bulk ROM Data Command */
    DEBUG_PRINTF("cmd: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x.\r\n", \
                 show_bulk_rom_data_cmd[0], show_bulk_rom_data_cmd[1], show_bulk_rom_data_cmd[2], \
                 show_bulk_rom_data_cmd[3], show_bulk_rom_data_cmd[4], show_bulk_rom_data_cmd[5]);
    err = write_cmd(show_bulk_rom_data_cmd, 6, ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
        ERROR_PRINTF("Fail to send Show Bulk ROM Data command! err=0x%x.\r\n", err);

    return err;
}

// Bulk ROM Data (in Boot Code)
int send_show_bulk_rom_data_command(unsigned short addr)
{
    int err = ERR_SUCCESS;
    unsigned char show_bulk_rom_data_cmd[6] =  {0x59, 0x00, 0x00, 0x00, 0x00, 0x01}; /* Show Bulk ROM Data Command (cmd[1]=0x00 in Boot Code) */

    /* Set Address */
    show_bulk_rom_data_cmd[2] = (addr & 0xFF00) >> 8;	//ADDR_H
    show_bulk_rom_data_cmd[3] =  addr & 0x00FF; 		//ADDR_L

    /* Send Show Bulk ROM Data Command */
    DEBUG_PRINTF("cmd: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x.\r\n", \
                 show_bulk_rom_data_cmd[0], show_bulk_rom_data_cmd[1], show_bulk_rom_data_cmd[2], \
                 show_bulk_rom_data_cmd[3], show_bulk_rom_data_cmd[4], show_bulk_rom_data_cmd[5]);
    err = write_cmd(show_bulk_rom_data_cmd, 6, ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if (err != ERR_SUCCESS)
        ERROR_PRINTF("Fail to send Show Bulk ROM Data command! err=0x%x.\r\n", err);

    return err;
}

int receive_bulk_rom_data(unsigned short *p_rom_data)
{
    int err = ERR_SUCCESS;
    unsigned char cmd_data[5] = {0};
    unsigned short rom_data = 0;

    err = read_data(cmd_data, sizeof(cmd_data), ELAN_READ_DATA_TIMEOUT_MSEC);
    if(err != ERR_SUCCESS) // Error or Timeout
    {
        ERROR_PRINTF("Fail to receive Bulk ROM data! err=0x%x.\r\n", err);
        goto RECEIVE_BULK_ROM_DATA_EXIT;
    }
    DEBUG_PRINTF("cmd_data: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x.\r\n", cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3], cmd_data[4]);

    /* Check if data invalid */
    if (cmd_data[0] != 0x99)
    {
        err = ERR_DATA_PATTERN;
        ERROR_PRINTF("Bulk Data Format Invalid! err=0x%x.\r\n", err);
        goto RECEIVE_BULK_ROM_DATA_EXIT;
    }

    // Load ROM Data to Input Buffer
    rom_data = (unsigned short)((cmd_data[3] << 8) | cmd_data[4]);
    DEBUG_PRINTF("Bulk ROM Data: 0x%04x.\r\n", rom_data);

    *p_rom_data = rom_data;
    err = ERR_SUCCESS;

RECEIVE_BULK_ROM_DATA_EXIT:
    return err;
}

// Hello Packet
// Bridge CMD 0x18: If command <0x18> is issued, feedback Hello packet for Recovery Mode.
int send_request_hello_packet_command(void)
{
    int err = ERR_SUCCESS;
    unsigned char request_iap_recovery_hello_packet_cmd = 0x18; // Vendor Command

    /* Send Request IAP Hello Packet Command (Vendor Command) */
    DEBUG_PRINTF("vendor_cmd: 0x%02x.\r\n", request_iap_recovery_hello_packet_cmd);
    err = write_vendor_cmd(&request_iap_recovery_hello_packet_cmd, 1, ELAN_WRITE_DATA_TIMEOUT_MSEC);
    if(err != ERR_SUCCESS)
        ERROR_PRINTF("Failed to write vendor command 0x%x, err=0x%x.\r\n", request_iap_recovery_hello_packet_cmd, err);

    return err;
}
