# 
# Readme document for hid_iap (I2C-HID / SPI-HID Interface)
# Date: 2024/08/01
# 
# Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.
Elan Touchscreen Firmware Update Tool (I2C-HID / SPI-HID Interface)
---
    Get ELAN Touchscreen Firmware Information & Update Firmware.

Compilation
--- 
    make: to build the exectue project "hid_iap".
    $ make
   
Run
---
Get Firmware Information :

    ./hid_iap -P {hid_pid} -i
ex:

    ./hid_iap -P 2a03 -i

Update Firmware :

    ./hid_iap -P {hid_pid} -f {firmware_file}

ex:

    ./hid_iap -P 2a03 -f /tmp/elants_hid_2a03.bin

Calibrate Touchscreen :

    ./hid_iap -P {hid_pid} -k

ex: 

    ./hid_iap -P 2a03 -k

Enable Silent Mode :

    ./hid_iap -P {hid_pid} -f {firmware_file} -s

ex: 

    ./hid_iap -P 2a03 -f /tmp/elants_hid_2a03.bin -s

Get Help Information :

    ./hid_iap -h

ex: 

    ./hid_iap -h

