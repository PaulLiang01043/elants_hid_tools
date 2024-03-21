# Makefile for Elan Touchscreen HID (I2C-HID / SPI-HID) Tools
# Date: 2024/03/21
dir_hid_iap := hid_iap
dir_hid_read_fwid := hid_read_fwid

.PHONY: all
all: 
	@for directory in $(dir_hid_iap) $(dir_hid_read_fwid); \
	do							\
		$(MAKE) -C $$directory;	\
	done
		
.PHONY: clean
clean:
	@for directory in $(dir_hid_iap) $(dir_hid_read_fwid); \
	do									\
		$(MAKE) clean -C $$directory;	\
	done

