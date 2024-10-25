#
# Makefile for hid_read_fwid (I2C-HID / SPI-HID Interface)
# Date: 2024/10/22
#
# Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.

# Variables
PROGRAM := hid_read_fwid
SRCS := ElanTsDebug.cpp \
        BaseLog.cpp \
        HIDLinuxGet.cpp \
        ElanTsHidUtility.cpp \
        ElanTsFuncApi.cpp \
        ElanTsHidDevUtility.cpp \
        ElanTsEdidUtility.cpp\
        ElanTsLcmDevUtility.cpp \
        ElanGen8TsHidUtility.cpp \
        ElanGen8TsFuncApi.cpp \
        main.cpp
OBJS := $(SRCS:.cpp=.o)
LIBS := stdc++ rt pthread

# Paths
srcdir     := ./src
includedir := ./include 
bindir     := ./bin

# Variables Used by Implicit Rules
CXX      ?= g++
CXXFLAGS := -Wall -Wno-format-overflow -ansi -O3 -g
CXXFLAGS += -D__ENABLE_DEBUG__
CXXFLAGS += -D__ENABLE_OUTBUF_DEBUG__
CXXFLAGS += -D__ENABLE_INBUF_DEBUG__
CXXFLAGS += $(addprefix -I, $(includedir))
LDLIBS   += $(addprefix -l, $(LIBS))

# Search Paths
VPATH = $(srcdir):$(includedir):$(bindir)
vpath %.cpp $(srcdir)
vpath %.h   $(includedir)
vpath %     $(bindir)

.SUFFIXS: .cpp .h
.PHONY: all
all: $(OBJS)
	$(CXX) $^ $(CXXFLAGS) $(LDLIBS) -o $(PROGRAM)
	@chmod 777 $(PROGRAM)
	@mv $(PROGRAM) $(bindir)
	@$(RM) $^
	
%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(LDLIBS)
	
.PHONY: clean
clean: 
	@$(RM) $(bindir)/$(PROGRAM) $(OBJS)

