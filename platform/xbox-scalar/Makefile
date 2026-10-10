XBE_TITLE = RacerRevengeScalarCheck
ROOT := $(abspath ../..)
NXDK_DIR ?= $(ROOT)/toolchains/nxdk
OUTPUT_DIR = $(ROOT)/build/xbox-scalar47M
SRCS = $(CURDIR)/main.cpp $(ROOT)/src/core/mmi47k.cpp $(ROOT)/src/core/scalar47m.cpp $(ROOT)/src/core/compact_leaf47m.cpp $(ROOT)/tests/scalar_reference47m.cpp $(ROOT)/tests/scalar_suite47m.cpp
NXDK_CXXFLAGS += -std=c++17 -O2 -fno-rtti -mno-sse2 -fwrapv -fno-strict-aliasing -I$(ROOT)/src/core -I$(ROOT)/tests -I$(ROOT)/third_party/pcsx2-mmi47k/reference_shim
include $(NXDK_DIR)/Makefile
