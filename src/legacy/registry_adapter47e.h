#pragma once
#include "ps2_runtime.h"
namespace rr47e {
PS2Runtime::RecompiledFunction lookup(uint32_t address);
bool replace(uint32_t address,PS2Runtime::RecompiledFunction function);
uint32_t allocatedPages();
uint32_t directoryBytes();
uint32_t replacementBytes();
}
