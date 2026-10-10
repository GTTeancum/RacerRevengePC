XBE_TITLE = RacerRevengeCoreCheck
ROOT := $(abspath ../..)
NXDK_DIR ?= $(ROOT)/toolchains/nxdk
OUTPUT_DIR = $(ROOT)/build/xbox-core
SRCS = $(CURDIR)/main.cpp $(ROOT)/src/core/cycle_ledger.cpp $(ROOT)/src/core/compact_registry.cpp $(ROOT)/tests/core_suite.cpp
NXDK_CXXFLAGS += -std=c++17 -O2 -fno-rtti -mno-sse2 -I$(ROOT)/src/core -I$(ROOT)/tests
include $(NXDK_DIR)/Makefile
