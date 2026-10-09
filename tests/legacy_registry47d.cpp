// Compare encoded lookup to the actual original static initializers, including
// resume/recovered registrations. No game loop, device setup or replacements.
#include "ps2_runtime.h"
#include "compact_registry.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

extern const uint32_t g_ps2RecompiledFunctionTableBase;
extern const uint32_t g_ps2RecompiledFunctionTableSlotCount;
extern PS2Runtime::RecompiledFunction g_ps2RecompiledFunctionTable[];
extern PS2Runtime::RecompiledFunction registry47d_bindings[];

template<class T> std::vector<T> load(const std::string& name) {
    std::ifstream f(name,std::ios::binary|std::ios::ate);
    if(!f) throw std::runtime_error("Missing registry input: "+name);
    const auto size=f.tellg();
    if(size<0 || uint64_t(size)%sizeof(T)) throw std::runtime_error("Bad registry input size");
    std::vector<T> data(size_t(size)/sizeof(T));
    f.seekg(0); f.read(reinterpret_cast<char*>(data.data()),size);
    if(!f) throw std::runtime_error("Registry input read failed");
    return data;
}
int main(int argc,char** argv) {
    if(argc!=3) { std::cerr<<"Usage: registry_check <data directory> <symbol count>\n"; return 2; }
    try {
        const std::string dir=std::string(argv[1])+"/";
        const auto pages=load<rr::RegistryPage>(dir+"pages.bin");
        const auto patterns=load<uint8_t>(dir+"patterns.bin");
        const auto owners=load<uint16_t>(dir+"owners.bin");
        const auto expected=load<uint16_t>(dir+"expected.bin");
        const uint32_t symbol_count=static_cast<uint32_t>(std::stoul(argv[2]));
        rr::CompactRegistry table;
        if(patterns.size()%64 || expected.size()!=g_ps2RecompiledFunctionTableSlotCount ||
           !table.configure(g_ps2RecompiledFunctionTableBase,g_ps2RecompiledFunctionTableSlotCount,
                pages.data(),uint32_t(pages.size()),patterns.data(),uint32_t(patterns.size()/64),
                owners.data(),uint32_t(owners.size()),symbol_count)) return 3;
        unsigned registered=0;
        for(uint32_t i=0;i<g_ps2RecompiledFunctionTableSlotCount;++i) {
            const uint32_t address=g_ps2RecompiledFunctionTableBase+i*4u;
            const uint16_t id=table.lookup(address);
            if(id!=expected[i] || registry47d_bindings[id]!=g_ps2RecompiledFunctionTable[i]) {
                std::cerr<<"Mismatch at 0x"<<std::hex<<address<<"\n"; return 4;
            }
            registered+=id!=0;
            if(table.lookup(address+1u) || table.lookup(address+2u) || table.lookup(address+3u)) return 5;
        }
        if(table.lookup(g_ps2RecompiledFunctionTableBase-4u) ||
           table.lookup(g_ps2RecompiledFunctionTableBase+g_ps2RecompiledFunctionTableSlotCount*4u) ||
           table.lookup(0xfffffffcu)) return 6;
        std::cout<<"registry-check slots="<<expected.size()<<" registered="<<registered
            <<" misaligned="<<expected.size()*3<<" actual_native_pointer_mismatches=0\n";
        return 0;
    } catch(const std::exception& e) { std::cerr<<e.what()<<"\n"; return 7; }
}
