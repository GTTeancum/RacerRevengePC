XBE_TITLE = RacerRevengeMmiCheck
ROOT := $(abspath ../..)
NXDK_DIR ?= $(ROOT)/toolchains/nxdk
OUTPUT_DIR = $(ROOT)/build/xbox-mmi47K
SRCS = $(CURDIR)/main.cpp $(ROOT)/src/core/mmi47k.cpp $(ROOT)/tests/mmi_reference47k.cpp $(ROOT)/tests/mmi_suite47k.cpp
NXDK_CXXFLAGS += -std=c++17 -O2 -fno-rtti -mno-sse2 -fwrapv -fno-strict-aliasing -I$(ROOT)/src/core -I$(ROOT)/tests -I$(ROOT)/third_party/pcsx2-mmi47k/reference_shim
include $(NXDK_DIR)/Makefile
