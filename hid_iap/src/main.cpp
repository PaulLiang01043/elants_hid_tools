/******************************************************************************
 *
 * Implementation of Elan HID (I2C-HID / SPI-HID) Firmware Update Tool
 *
 * Release:
 *		2024/3
 *
 * Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <linux/input.h>    // BUS_TYPE
#include "ElanTsDebug.h"
#include "HIDLinuxGet.h"
#include "ElanTsHidUtility.h"
#include "ElanTsFuncApi.h"
#include "ElanTsFwFileIoUtility.h"
#include "ElanTsFwUpdateFlow.h"
#include "ElanGen8TsHidHwParameters.h"
#include "ElanGen8TsFwUpdateFlow.h"

/*******************************************
 * Definitions
 ******************************************/

// SW Version
#ifndef ELAN_TOOL_SW_VERSION
#define ELAN_TOOL_SW_VERSION           "0.2"
#endif //ELAN_TOOL_SW_VERSION

// SW Release Date
#ifndef ELAN_TOOL_SW_RELEASE_DATE
#define ELAN_TOOL_SW_RELEASE_DATE	"2024-05-21"
#endif //ELAN_TOOL_SW_RELEASE_DATE

/*******************************************
 * Feature Configurations
 ******************************************/

/*******************************************
 * Global Variables Declaration
 ******************************************/

// InterfaceGet Class
CHIDLinuxGet *g_pIntfGet = NULL; // Pointer to HID Inteface Class (CHIDLinuxGet)

// PID
int g_pid = 0;

// Flag for Firmware Update
bool g_update_fw = false;

// Firmware File Information
char g_firmware_filename[FILE_NAME_LENGTH_MAX] = {0};

// Firmware Inforamtion
bool g_get_fw_info = false;

// Re-Calibration (Re-K)
bool g_rek = false;

// Calibration Counter
bool g_get_rek_counter = false;

// Skip Action Code
int g_skip_action_code = 0;

// Message Mode
message_mode_t	g_msg_mode = FULL_MESSAGE;

// Help Info.
bool g_help = false;

// Parameter Option Settings
const char* const short_options = "p:P:f:s:oikcqdh";
const struct option long_options[] =
{
    { "pid",                     1, NULL, 'p'},
    { "pid_hex",                 1, NULL, 'P'},
    { "file_path",               1, NULL, 'f'},
    { "skip_action",             1, NULL, 's'},
    { "firmware_information",    0, NULL, 'i'},
    { "calibration",             0, NULL, 'k'},
    { "calibration_counter",     0, NULL, 'c'},
    { "quiet",                   0, NULL, 'q'},
    { "debug",                   0, NULL, 'd'},
    { "help",                    0, NULL, 'h'},
};

/*******************************************
 * Function Prototype
 ******************************************/

// Help
void show_help_information(void);

// HID Raw I/O Function
int __hidraw_write(unsigned char* buf, int len, int timeout_ms);
int __hidraw_read(unsigned char* buf, int len, int timeout_ms);

// Abstract Device I/O Function
int write_cmd(unsigned char *cmd_buf, int len, int timeout_ms);
int read_data(unsigned char *data_buf, int len, int timeout_ms);
int write_vendor_cmd(unsigned char *cmd_buf, int len, int timeout_ms);
int open_device(void);
int close_device(void);
int get_bus_type(unsigned int *bus_type);
int reconnect_device(void);

// Default Function
int process_parameter(int argc, char **argv);
int resource_init(void);
int resource_free(void);
int main(int argc, char **argv);

/*******************************************
 * HID Raw I/O Functions
 ******************************************/

int __hidraw_write(unsigned char* buf, int len, int timeout_ms)
{
    int nRet = ERR_SUCCESS;

    if(g_pIntfGet == NULL)
    {
        nRet = ERR_NO_INTERFACE_CREATED;
        goto __HIDRAW_WRITE_EXIT;
    }

    nRet = g_pIntfGet->WriteRawBytes(buf, len, timeout_ms);

__HIDRAW_WRITE_EXIT:
    return nRet;
}

int __hidraw_read(unsigned char* buf, int len, int timeout_ms)
{
    int nRet = ERR_SUCCESS;

    if(g_pIntfGet == NULL)
    {
        nRet = ERR_NO_INTERFACE_CREATED;
        goto __HIDRAW_READ_EXIT;
    }

    nRet = g_pIntfGet->ReadRawBytes(buf, len, timeout_ms);

__HIDRAW_READ_EXIT:
    return nRet;
}

int __hidraw_write_command(unsigned char* buf, int len, int timeout_ms)
{
    int nRet = ERR_SUCCESS;

    if(g_pIntfGet == NULL)
    {
        nRet = ERR_NO_INTERFACE_CREATED;
        goto __HIDRAW_WRITE_EXIT;
    }

    nRet = g_pIntfGet->WriteCommand(buf, len, timeout_ms);

__HIDRAW_WRITE_EXIT:
    return nRet;
}

int __hidraw_read_data(unsigned char* buf, int len, int timeout_ms)
{
    int nRet = ERR_SUCCESS;

    if(g_pIntfGet == NULL)
    {
        nRet = ERR_NO_INTERFACE_CREATED;
        goto __HIDRAW_READ_EXIT;
    }

    nRet = g_pIntfGet->ReadData(buf, len, timeout_ms);

__HIDRAW_READ_EXIT:
    return nRet;
}

/***************************************************
 * Abstract I/O Functions
 ***************************************************/

int write_cmd(unsigned char *cmd_buf, int len, int timeout_ms)
{
    //write_bytes_from_buffer_to_i2c(cmd_buf); //pseudo function

    /*** example *********************/
    return __hidraw_write_command(cmd_buf, len, timeout_ms);
    /*********************************/
}

int read_data(unsigned char *data_buf, int len, int timeout_ms)
{
    //read_bytes_from_i2c_to_buffer(data_buf, len, timeout); //pseudo function

    /*** example *********************/
    return __hidraw_read_data(data_buf, len, timeout_ms);
    /*********************************/
}

int write_vendor_cmd(unsigned char *cmd_buf, int len, int timeout_ms)
{
    unsigned char vendor_cmd_buf[ELAN_HID_OUTPUT_BUFFER_SIZE] = {0};

    // Add HID Header
    vendor_cmd_buf[0] = ELAN_HID_OUTPUT_REPORT_ID;
    memcpy(&vendor_cmd_buf[1], cmd_buf, len);

    return __hidraw_write(vendor_cmd_buf, sizeof(vendor_cmd_buf), timeout_ms);
}

/*******************************************
 * Function Implementation
 ******************************************/

/*******************************************
 * Help
 ******************************************/

void show_help_information(void)
{
    printf("--------------------------------\r\n");
    printf("SYNOPSIS:\r\n");

    // PID
    printf("\n[PID]\r\n");
    printf("-p <pid in decimal>.\r\n");
    printf("Ex: hid_iap -p 1842\r\n");
    printf("-P <PID in hex>.\r\n");
    printf("Ex: hid_iap -P 732 (0x732)\r\n");

    // File Path
    printf("\n[File Path]\r\n");
    printf("-f <file_path>.\r\n");
    printf("Ex: hid_iap -f firmware.ekt\r\n");
    printf("Ex: hid_iap -f /tmp/firmware.ekt\r\n");

    // Skip Action
    printf("\n[Skip Action]\r\n");
    printf("-s <action_code>.\r\n");
    printf("Ex: hid_iap -s 1 \r\n");

    // Firmware Information
    printf("\n[Firmware Information]\r\n");
    printf("-i.\r\n");
    printf("Ex: hid_iap -i\r\n");

    // Re-Calibarion
    printf("\n[Re-Calibarion]\r\n");
    printf("-k.\r\n");
    printf("Ex: hid_iap -k\r\n");

    // Re-Calibarion
    printf("\n[Calibarion Counter]\r\n");
    printf("-c.\r\n");
    printf("Ex: hid_iap -c\r\n");

    // Silent (Quiet) Mode
    printf("\n[Silent Mode]\r\n");
    printf("-q.\r\n");
    printf("Ex: hid_iap -q\r\n");

    // Debug Information
    printf("\n[Debug]\r\n");
    printf("-d.\r\n");
    printf("Ex: hid_iap -d\r\n");

    // Help Information
    printf("\n[Help]\r\n");
    printf("-h.\r\n");
    printf("Ex: hid_iap -h\r\n");

    return;
}

/*******************************************
 *  Open & Close Device
 ******************************************/

int open_device(void)
{
    int err = ERR_SUCCESS;

    // open specific device on i2c bus //pseudo function

    /*** example *********************/
    if(g_pIntfGet == NULL)
    {
        err = ERR_NO_INTERFACE_CREATED;
        goto OPEN_DEVICE_EXIT;
    }

    // Connect to Device
    DEBUG_PRINTF("Get HID Device Handle (VID=0x%x, PID=0x%x).\r\n", ELAN_HID_VID, g_pid);
    err = g_pIntfGet->GetDeviceHandle(ELAN_HID_VID, g_pid);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Device can't connected! err=0x%x.\n", err);
    }

OPEN_DEVICE_EXIT:
    /*********************************/

    return err;
}

int close_device(void)
{
    int err = ERR_SUCCESS;

    // close opened i2c device; //pseudo function

    /*** example *********************/
    if(g_pIntfGet == NULL)
    {
        err = ERR_NO_INTERFACE_CREATED;
        goto CLOSE_DEVICE_EXIT;
    }

    // Release acquired touch device handler
    g_pIntfGet->Close();

CLOSE_DEVICE_EXIT:
    /*********************************/

    return err;
}

int get_bus_type(unsigned int *bus_type)
{
    int nRet = ERR_SUCCESS;

    // return bus type; //pseudo function

    /*** example *********************/
    if (bus_type == NULL)
    {
        nRet = ERR_INVALID_PARAM;
        goto GET_BUS_TYPE_EXIT;
    }

    nRet = g_pIntfGet->GetDevBusType(bus_type);

GET_BUS_TYPE_EXIT:
    /*********************************/

    return nRet;
}

int reconnect_device(void)
{
    int err = ERR_SUCCESS;
    
    // close and open device again; //pseudo function
    
    /*** example *********************/

    // Make Sure Interface Has been Established
    if(g_pIntfGet == NULL)
    {
        err = ERR_NO_INTERFACE_CREATED;
        goto RE_CONNECT_DEVICE_EXIT;
    }
    
    // Release acquired touch device handler
    g_pIntfGet->Close();

    // Connect to Device
    DEBUG_PRINTF("Get HID Device Handle (VID=0x%x, PID=0x%x).\r\n", ELAN_HID_VID, g_pid);
    err = g_pIntfGet->GetDeviceHandle(ELAN_HID_VID, g_pid);
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Device can't connected! err=0x%x.\n", err);
    }

RE_CONNECT_DEVICE_EXIT:
    /*********************************/

    return err;
}

/*******************************************
 *  Initialize & Free Resource
 ******************************************/

int resource_init(void)
{
    int err = ERR_SUCCESS,
        firmware_size = 0;

    //initialize_resource(); //pseudo function

    /*** example *********************/

    // Initialize Interface
    g_pIntfGet = new CHIDLinuxGet();
    DEBUG_PRINTF("g_pIntfGet=%p.\n", g_pIntfGet);
    if (g_pIntfGet == NULL)
    {
        ERROR_PRINTF("Fail to initialize HID Interface!");
        err = ERR_NO_INTERFACE_CREATED;
        goto RESOURCE_INIT_EXIT;
    }

    if(g_update_fw == true)
    {
        // Open Firmware File
        err = open_firmware_file(g_firmware_filename, strlen(g_firmware_filename));
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("Fail to open firmware file \"%s\"! err=0x%x.\r\n", g_firmware_filename, err);
            goto RESOURCE_INIT_EXIT;
        }

        // Get Firmware Size
        err = get_firmware_size(&firmware_size);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("Fail to open firmware size \"%s\"! err=0x%x.\r\n", g_firmware_filename, err);
            goto RESOURCE_INIT_EXIT;
        }

        // Make Sure Firmware File Valid
        //DEBUG_PRINTF("Firmware Size: %d.\r\n", firmware_size);
        if(firmware_size <= 0)
        {
            ERROR_PRINTF("Invalid Firmware Size: %d!\r\n", firmware_size);
            err = ERR_FILE_NOT_FOUND;
            goto RESOURCE_INIT_EXIT;
        }

    }

    // Success
    err = ERR_SUCCESS;

RESOURCE_INIT_EXIT:
    /*********************************/

    return err;
}

int resource_free(void)
{
    int err = ERR_SUCCESS;

    //release_resource(); //pseudo function

    /*** example *********************/

    if(g_update_fw == true)
    {
        // Close Firmware File
        close_firmware_file();
    }

    // Release Interface
    if (g_pIntfGet)
    {
        delete dynamic_cast<CHIDLinuxGet *>(g_pIntfGet);
        g_pIntfGet = NULL;
    }

    /*********************************/

    return err;
}

/***************************************************
* Parser command
***************************************************/

int process_parameter(int argc, char **argv)
{
    int err = ERR_SUCCESS,
        opt = 0,
        option_index = 0,
        pid = 0,
        pid_str_len = 0,
        file_path_len = 0,
        action_code = 0;
    char file_path[FILE_NAME_LENGTH_MAX] = {0};

    while (1)
    {
        opt = getopt_long(argc, argv, short_options, long_options, &option_index);
        if (opt == EOF)	break;

        switch (opt)
        {
            case 'p': /* PID (Decimal) */

                // Make Sure Data Valid
                pid = atoi(optarg);
                if (pid < 0)
                {
                    ERROR_PRINTF("%s: Invalid PID: %d!\n", __func__, pid);
                    err = ERR_INVALID_PARAM;
                    goto PROCESS_PARAM_EXIT;
                }

                // Set Global ADC Type
                g_pid = pid;
                DEBUG_PRINTF("%s: PID=%d(0x%x).\r\n", __func__, g_pid, g_pid);
                break;

            case 'P': /* PID (Hex) */

                // Make Sure Format Valid
                pid_str_len = strlen(optarg);
                if (pid_str_len > 4)
                {
                    ERROR_PRINTF("%s: Invalid String Length for PID: %d!\n", __func__, pid_str_len);
                    err = ERR_INVALID_PARAM;
                    goto PROCESS_PARAM_EXIT;
                }

                // Make Sure Data Valid
                pid = strtol(optarg, NULL, 16);
                if (pid < 0)
                {
                    ERROR_PRINTF("%s: Invalid PID: %d!\n", __func__, pid);
                    err = ERR_INVALID_PARAM;
                    goto PROCESS_PARAM_EXIT;
                }

                // Set PID
                g_pid = pid;
                DEBUG_PRINTF("%s: PID=0x%x.\r\n", __func__, g_pid);
                break;

            case 'f': /* FW File Path */

                // Check if filename is valid
                file_path_len = strlen(optarg);
                if ((file_path_len == 0) || (file_path_len > FILE_NAME_LENGTH_MAX))
                {
                    ERROR_PRINTF("%s: Firmware Path (%s) Invalid!\r\n", __func__, optarg);
                    err = ERR_INVALID_PARAM;
                    goto PROCESS_PARAM_EXIT;
                }

                // Check if file path is valid
                strcpy(file_path, optarg);
                //DEBUG_PRINTF("%s: fw file path=\"%s\".\r\n", __func__, file_path);
                if(strncmp(file_path, "", strlen(file_path)) == 0)
                {
                    ERROR_PRINTF("%s: NULL Firmware Path!\r\n", __func__);
                    err = ERR_INVALID_PARAM;
                    goto PROCESS_PARAM_EXIT;
                }

                // Set FW Update Flag
                g_update_fw = true;

                // Set Global File Path
                strcpy(g_firmware_filename, file_path);
                DEBUG_PRINTF("%s: Update FW: %s, File Path: \"%s\".\r\n", __func__, (g_update_fw) ? "Yes" : "No", g_firmware_filename);
                break;

            case 's': /* Skip Action */

                // Make Sure Data Valid
                action_code = atoi(optarg);
                if (action_code < 0)
                {
                    ERROR_PRINTF("%s: Invalid Action Code: %d!\n", __func__, action_code);
                    err = ERR_INVALID_PARAM;
                    goto PROCESS_PARAM_EXIT;
                }

                // Set Global ADC Type
                g_skip_action_code = action_code;
                DEBUG_PRINTF("%s: Skip Action Code: %d.\r\n", __func__, g_skip_action_code);
                break;

            case 'i': /* Firmware Information */

                // Set "Get FW Info." Flag
                g_get_fw_info = true;
                DEBUG_PRINTF("%s: Get FW Inforamtion: %s.\r\n", __func__, (g_get_fw_info) ? "Enable" : "Disable");
                break;

            case 'k': /* Re-Calibration (Re-K) */

                // Set Re-K Flag
                g_rek = true;
                DEBUG_PRINTF("%s: Re-Calibration: %s.\r\n", __func__, (g_rek) ? "Enable" : "Disable");
                break;

            case 'c': /* Calibration Counter */

                // Set "Get Calibration Counter" Flag
                g_get_rek_counter = true;
                DEBUG_PRINTF("%s: Get Calibration Counter: %s.\r\n", __func__, (g_get_rek_counter) ? "Enable" : "Disable");
                break;

            case 'q': /* Silent Mode (Quiet) */

                // Enable Silent Mode
                g_msg_mode = SILENT_MODE;
                DEBUG_PRINTF("%s: Silent Mode: %s.\r\n", __func__, (g_msg_mode == SILENT_MODE) ? "Enable" : "Disable");
                break;

            case 'd': /* Debug Option */

                // Enable Debug & Output Buffer Debug
                g_debug = true;
                DEBUG_PRINTF("Debug: %s.\r\n", (g_debug) ? "Enable" : "Disable");
                break;

            case 'h': /* Help */

                // Set debug
                g_help = true;
                DEBUG_PRINTF("Help Information: %s.\r\n", (g_help) ? "Enable" : "Disable");
                break;

            default:
                ERROR_PRINTF("%s: Unknown Command!\r\n", __func__);
                break;
        }
    }

    // Check if PID is not null
    if(g_pid == 0)
    {
        DEBUG_PRINTF("%s: PID is not set, look for an appropriate PID...\r\n", __func__);
    }

    return ERR_SUCCESS;

PROCESS_PARAM_EXIT:
    DEBUG_PRINTF("[ELAN] ParserCmd: Exit because of an error occurred, err=0x%x.\r\n", err);
    return err;
}

/*******************************************
 * Main Function
 ******************************************/

int main(int argc, char **argv)
{
    int err = ERR_SUCCESS;
    unsigned int bus_type = 0;
    unsigned short fw_bc_version = 0,
                   bc_bc_version = 0;
    unsigned char hello_packet = 0;
    bool gen8_touch = false,	// True if Gen8 Touch
         recovery = false;		// True if Recovery Mode
    message_mode_t msg_mode;

    // Process Parameter
    err = process_parameter(argc, argv);
    if (err != ERR_SUCCESS)
    {
        goto EXIT;
    }

    if(g_msg_mode == FULL_MESSAGE) // Disable Silent Mode
    {
        printf("hid_iap v%s %s.\r\n", ELAN_TOOL_SW_VERSION, ELAN_TOOL_SW_RELEASE_DATE);
    }

    /* Show Help Information */
    if(g_help == true)
    {
        show_help_information();
        goto EXIT;
    }

    /* Initialize Resource */
    err = resource_init();
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to Init Resource! err=0x%x.\r\n", err);
        goto EXIT1;
    }

    /* Open Device */
    err = open_device() ;
    if (err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to Open Device! err=0x%x.\r\n", err);
        goto EXIT2;
    }

    /* Detect Touch State */

    // Get Hello Packet
    err = get_hello_packet_bc_version_with_error_retry(&hello_packet, &bc_bc_version, ERROR_RETRY_COUNT);
    if(err != ERR_SUCCESS)
    {
        ERROR_PRINTF("Fail to Get Hello Packet (& BC Version)! err=0x%x.\r\n", err);
        goto EXIT2;
    }
    DEBUG_PRINTF("Hello Packet: 0x%02x, Recovery Mode BC Version: 0x%04x.\r\n", hello_packet, bc_bc_version);

    // Identify HW Series & Touch State
    switch (hello_packet)
    {
        case ELAN_HID_NORMAL_MODE_HELLO_PACKET:
            // BC Version (Normal Mode)
            err = get_boot_code_version(&fw_bc_version);
            if(err != ERR_SUCCESS)
            {
                ERROR_PRINTF("%s: Fail to Get BC Version (Normal Mode)! err=0x%x.\r\n", __func__, err);
                goto EXIT2;
            }
            DEBUG_PRINTF("Normal Mode BC Version: 0x%04x.\r\n", fw_bc_version);

            // Special Case: First BC of EM32F901 / EM32F902
            if((HIGH_BYTE(fw_bc_version) == BC_VER_H_BYTE_FOR_EM32F901_HID) /* EM32F901 */ ||
               (HIGH_BYTE(fw_bc_version) == BC_VER_H_BYTE_FOR_EM32F902_HID) /* FM32F902 */)
                gen8_touch = true;     // Gen8 Touch
            else
                gen8_touch = false;    // Gen5/6/7 Touch
            recovery = false;          // Normal Mode
            break;

        case ELAN_GEN8_HID_NORMAL_MODE_HELLO_PACKET:
            gen8_touch = true;         // Gen8 Touch
            recovery = false;          // Normal Mode
            break;

        case ELAN_HID_RECOVERY_MODE_HELLO_PACKET:
            // Special Case: First BC of EM32F901 / EM32F902
            if((HIGH_BYTE(bc_bc_version) == BC_VER_H_BYTE_FOR_EM32F901_HID) /* EM32F901 */ ||
               (HIGH_BYTE(bc_bc_version) == BC_VER_H_BYTE_FOR_EM32F902_HID) /* FM32F902 */)
                gen8_touch = true;     // Gen8 Touch
            else
                gen8_touch = false;    // Gen5/6/7 Touch
            recovery = true;           // Recovery Mode
            break;

        case ELAN_GEN8_HID_RECOVERY_MODE_HELLO_PACKET:
            gen8_touch = true;         // Gen8 Touch
            recovery = true;           // Recovery Mode
            break;

        default:
            ERROR_PRINTF("%s: Unknown Hello Packet! (0x%02x) \r\n", __func__, hello_packet);
            err = ERR_UNKNOWN_DEVICE_TYPE;
            goto EXIT2;
    }

    // Reconfigure if Recovery Mode
    if(recovery == true)
    {
        printf("In Recovery Mode.\r\n");
        g_get_fw_info = false;         // Disable Get FW Info.
        g_rek = false;                 // Disable Re-Calibration
        g_get_rek_counter = false;     // Disable Get Calibration Counter
    }

    /* Get FW Information */
    if((g_get_fw_info == true) && (g_update_fw == false))
    {
        DEBUG_PRINTF("Get FW Info.\r\n");
        if(gen8_touch) // Gen8 Touch
            err = gen8_get_firmware_information(g_msg_mode);
        else // Gen5/6/7 Touch
            err = get_firmware_information(g_msg_mode);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("Fail to Get FW Info!\r\n");
            goto EXIT2;
        }
    }

    /* Get Calibration Counter */
    if((g_get_rek_counter == true) && (g_update_fw == false))
    {
        DEBUG_PRINTF("Get Calibration Counter.\r\n");

        // If with calibration, change message mode to NO_MESSAGE.
        if(g_rek == true)
            msg_mode = NO_MESSAGE;
        else // In General Case
            msg_mode = g_msg_mode;

        // Get Calibration Counter
        if(gen8_touch) // Gen8 Touch
            err = gen8_get_calibration_counter(msg_mode);
        else // Gen5/6/7 Touch
            err = get_calibration_counter(msg_mode);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("Fail to Get Calibration Counter!\r\n");
            goto EXIT2;
        }
    }

    /* Re-calibrate Touch */
    if(g_rek == true)
    {
        if(gen8_touch == false) // Gen5/6/7 Touch
        {
            DEBUG_PRINTF("Calibrate Touch...\r\n");
            err = calibrate_touch_with_error_retry(ERROR_RETRY_COUNT);
            if (err != ERR_SUCCESS)
            {
                ERROR_PRINTF("Fail to Calibrate Touch!\r\n");
                goto EXIT2;
            }

            // If with getting calibration counter, change message mode to FULL_MESSAGE.
            if(g_get_rek_counter == true)
                msg_mode = FULL_MESSAGE;
            else // In General Case
                msg_mode = NO_MESSAGE;

            // Verify Calibration with Counter
            err = get_calibration_counter(msg_mode);
            if(err != ERR_SUCCESS)
            {
                ERROR_PRINTF("Fail to Get Calibration Counter!\r\n");
                goto EXIT2;
            }
        }
        else // Gen8 Touch
        {
            /* [Note] 2022/06/29
            * Re-Calibration is not supported from Gen8 touch.
            * To avoid timeout waiting, this function is disable in Gen8 case.
            */
            ERROR_PRINTF("Re-Calibration is not supported from Gen8 touch!\r\n");
        }
    }

    /* Update FW */
    if(g_update_fw == true)
    {
        if(recovery == false) // Normal IAP
        {
            // Get FW Info.
            DEBUG_PRINTF("Get FW Info.\r\n");
            if(gen8_touch) // Gen8 Touch
                err = gen8_get_firmware_information(FULL_MESSAGE); // Disable Silent Mode
            else // Gen5/6/7 Touch
                err = get_firmware_information(FULL_MESSAGE); // Disable Silent Mode
            if(err != ERR_SUCCESS)
            {
                ERROR_PRINTF("Fail to Get FW Info!\r\n");
                goto EXIT2;
            }
        }

        // Update Firmware
        DEBUG_PRINTF("Update Firmware (%s), Gen8 Touch: %s, Recovery: %s, Skip Action Code: 0x%x.\r\n", \
                     g_firmware_filename, \
                     (gen8_touch) ? "true" : "false", \
                     (recovery) ? "true" : "false", \
                     g_skip_action_code);
        if(gen8_touch) // Gen8 Touch
            err = gen8_update_firmware(g_firmware_filename, strlen(g_firmware_filename), recovery, g_skip_action_code);
        else // Gen5/6/7 Touch
            err = update_firmware(g_firmware_filename, strlen(g_firmware_filename), recovery, g_skip_action_code);
        if (err != ERR_SUCCESS)
        {
            ERROR_PRINTF("Fail to Update Firmware (%s)!\r\n", g_firmware_filename);
            goto EXIT2;
        }

        // Get Bus Type
        err = get_bus_type(&bus_type);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("Fail to Get Bus Type! err=0x%x.\r\n", err);
        }
        DEBUG_PRINTF("Bus Type: 0x%02x.\r\n", bus_type);

        // Re-connect Device if SPI
        if (bus_type == BUS_SPI)
        {
            // Re-connect Device
            err = reconnect_device();
            if(err != ERR_SUCCESS)
            {
                ERROR_PRINTF("Fail to Re-connect Device! err=0x%x.\r\n", err);
                goto EXIT2;
            }
        }

        // Re-calibrate Touch
        DEBUG_PRINTF("Calibrate Touch...\r\n");
        if(gen8_touch)
        {
            /* [Note] 2022/06/06
            * With the information from FW Solution Team, it takes 100ms for touch to self-calibrate after power-on.
            * For safety reasons, a waiting time of 300ms is recommended.
            */
            usleep(300 * 1000); // wait 300ms
        }
        else // Gen5/6/7 Touch
        {
            err = calibrate_touch_with_error_retry(ERROR_RETRY_COUNT);
            if (err != ERR_SUCCESS)
            {
                ERROR_PRINTF("Fail to Calibrate Touch!\r\n");
                goto EXIT2;
            }
        }

        // Verify FW Update with FW Information
        DEBUG_PRINTF("Get FW Info.\r\n");
        if(gen8_touch) // Gen8 Touch
            err = gen8_get_firmware_information(FULL_MESSAGE); // Disable Silent Mode
        else // Gen5/6/7 Touch
            err = get_firmware_information(FULL_MESSAGE); // Disable Silent Mode
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("Fail to Get FW Info!\r\n");
            goto EXIT2;
        }

        // Verify Calibration with Counter
        err = get_calibration_counter(NO_MESSAGE);
        if(err != ERR_SUCCESS)
        {
            ERROR_PRINTF("Fail to Get Calibration Counter!\r\n");
            goto EXIT2;
        }
    }

    // Success
    err = ERR_SUCCESS;

EXIT2:
    /* Close Device */
    close_device();

EXIT1:
    /* Release Resource */
    resource_free();

EXIT:
    if(g_msg_mode == FULL_MESSAGE) // Disable Silent Mode
    {
        // End of Output Stream
        printf("\r\n");
    }

    return err;
}
