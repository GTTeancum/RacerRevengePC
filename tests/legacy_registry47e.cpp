#include "registry_adapter47e.h"
#include <iostream>
extern const uint32_t g_ps2RecompiledFunctionTableBase;
extern const uint32_t g_ps2RecompiledFunctionTableSlotCount;
extern PS2Runtime::RecompiledFunction g_ps2RecompiledFunctionTable[];
void replacement47e(uint8_t*,R5900Context*,PS2Runtime*) {}
int main() {
    PS2Runtime runtime;
    uint32_t first=0,alias=0,hole=0;
    PS2Runtime::RecompiledFunction shared=nullptr;
    for(uint32_t n=0;n<g_ps2RecompiledFunctionTableSlotCount;++n) {
        const uint32_t address=g_ps2RecompiledFunctionTableBase+n*4u;
        const auto expected=g_ps2RecompiledFunctionTable[n];
        if(rr47e::lookup(address)!=expected || runtime.hasFunction(address)!=(expected!=nullptr)) return 1;
        if(expected && runtime.lookupFunction(address)!=expected) return 2;
        if(!expected && !hole) hole=address;
        if(expected && !first) { first=address; shared=expected; }
        else if(expected==shared && !alias) alias=address;
    }
    if(!first || !alias || !hole) return 3;
    if(!runtime.replaceFunction(first,replacement47e) || runtime.lookupFunction(first)!=replacement47e ||
        runtime.lookupFunction(alias)!=shared) return 4;
    if(!runtime.replaceFunction(first,nullptr) || runtime.hasFunction(first) ||
        runtime.lookupFunction(alias)!=shared) return 5;
    if(!runtime.registerFunction(hole,replacement47e) || !runtime.hasFunction(hole) ||
        runtime.lookupFunction(hole)!=replacement47e) return 6;
    if(runtime.replaceFunction(first+1,replacement47e) || runtime.replaceFunction(0,replacement47e)) return 7;
    if(!runtime.replaceFunction(first,shared) || !runtime.replaceFunction(hole,nullptr)) return 8;
    for(uint32_t n=0;n<g_ps2RecompiledFunctionTableSlotCount;++n)
        if(rr47e::lookup(g_ps2RecompiledFunctionTableBase+n*4u)!=g_ps2RecompiledFunctionTable[n]) return 9;
    std::cout<<"compact-runtime slots="<<g_ps2RecompiledFunctionTableSlotCount
        <<" complete_passes=2 pointer_mismatches=0 replacement_contract=pass pages="<<rr47e::allocatedPages()
        <<" directory_bytes="<<rr47e::directoryBytes()<<" replacement_bytes="<<rr47e::replacementBytes()<<"\n";
}
