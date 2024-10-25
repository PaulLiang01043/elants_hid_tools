# 
# Readme document for hid_read_fwid (I2C-HID / SPI-HID Interface)
# Date: 2024/08/02
# 
# Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
Elan Touchscreen Firmware ID Tool (I2C-HID / SPI-HID Interface)
---
    Get ELAN Touchscreen Firmware ID for Current System.

Compilation
--- 
    make: to build the exectue project "hid_read_fwid".
    $ make
   
Run
---
Get Device Information :

    ./hid_read_fwid -P {hid_pid} -i
ex:

    ./hid_read_fwid -P 2a03 -i

Get Firmware ID :

    ./hid_read_fwid -P {hid_pid} -f {fwid_mapping_table_file} -s {system}

ex:

    ./hid_read_fwid -P 2a03 -f /tmp/fwid_mapping_table.txt -s chrome

    ./hid_read_fwid -P 2a03 -f /tmp/fwid_mapping_table.txt -s windows

Enable Silent Mode :

    ./hid_read_fwid -P {hid_pid} -f {fwid_mapping_table_file} -s {system} -q

ex:

    ./hid_read_fwid -P 2a03 -f /tmp/fwid_mapping_table.txt -s chrome -q

    ./hid_read_fwid -P 2a03 -f /tmp/fwid_mapping_table.txt -s windows -q

Get Help Information :

    ./hid_read_fwid -h

ex: 

    ./hid_read_fwid -h

