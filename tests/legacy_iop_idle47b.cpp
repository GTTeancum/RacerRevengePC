// Reuse the existing physical IRX fixtures without changing their guest code.
#define main preserved_iop_fixture_suite_main
#include "iop_emulator_tests.cpp"
#undef main
#include "iop_emulator.h"
#include <cstdlib>
#include <iomanip>

using Emulator = ps2x::iop::detail::IopEmulator;
namespace {
uint64_t hashBytes(uint64_t h, const void* bytes, size_t count) {
    const auto* p = static_cast<const uint8_t*>(bytes);
    for (size_t i=0; i<count; ++i) h=(h^p[i])*1099511628211ull;
    return h;
}
std::vector<std::string> state(Emulator& emu) {
    auto lines=emu.debugDiagnostics();
    lines.erase(std::remove_if(lines.begin(),lines.end(),[](const auto& s){return s.rfind("idle-batch ",0)==0;}),lines.end());
    return lines;
}
}
int main(int argc, char**) {
    _putenv_s("RR_IOP_IDLE_STATS","1");
    const bool reference = argc > 1;
    unsigned comparisons=0;
    for (unsigned scenario=0; scenario<4; ++scenario) {
        TestHost a(0x20000), b(0x20000);
        _putenv_s("RR_IOP_IDLE_BATCH","0");
        Emulator baseline(a);
        _putenv_s("RR_IOP_IDLE_BATCH","1");
        Emulator candidate(b);
        using Fixture=void(*)(TestHost&,uint32_t);
        const Fixture fixtures[]={nullptr,writeVblankSchedulingIrx,writeCdvdSeekIrx,writeMinimalIrx};
        if(fixtures[scenario]) {
            fixtures[scenario](a,0x100); fixtures[scenario](b,0x100);
            auto x=baseline.loadModuleBuffer(0x100,nullptr,0), y=candidate.loadModuleBuffer(0x100,nullptr,0);
            if(!expect(x.handled && y.handled && x.startResult==y.startResult,"module result")) return 1;
        }
        std::vector<uint8_t> ra(2*1024*1024), rb(ra.size());
        uint64_t signature=14695981039346656037ull, pcmSamples=0;
        uint32_t heapA=0,heapB=0;
        for(unsigned n=0;n<1500000;++n) {
            // Mostly the actual 8-EE-cycle dispatch cadence, including fractional
            // carry and chunks crossing audio, callback and VBlank boundaries.
            const uint64_t cycles=(n%4096==0)?6139u:((n%257==0)?3u:8u);
            baseline.runEeCycles(cycles); candidate.runEeCycles(cycles);
            if(n%16384==0 || n==1499999) {
                ++comparisons;
                if(!expect(baseline.cycles()==candidate.cycles(),"cycle equality") ||
                   !expect(baseline.instructions()==candidate.instructions(),"instruction equality") ||
                   !expect(state(baseline)==state(candidate),"thread/module state equality") ||
                   !expect(a.logs==b.logs && a.guest==b.guest,"host effects equality")) return 2;
                if(!baseline.readMemory(0,ra.data(),ra.size()) || !candidate.readMemory(0,rb.data(),rb.size())) return 3;
                if(!expect(ra==rb,"full 2 MiB IOP RAM equality")) return 4;
                for(unsigned core=0;core<2;++core) {
                    auto pa=baseline.takeAutoDmaPcm(core), pb=candidate.takeAutoDmaPcm(core);
                    if(!expect(pa==pb,"exact mixed PCM equality")) return 5;
                    pcmSamples+=pa.size();
                    signature=hashBytes(signature,pa.data(),pa.size()*sizeof(int16_t));
                }
                // Invalidate cached idle state via ordinary public memory and
                // allocation operations, including freeing/reusing a handle.
                if(heapA) { if(!baseline.freeMemory(heapA) || !candidate.freeMemory(heapB)) return 6; }
                heapA=baseline.allocateMemory(32); heapB=candidate.allocateMemory(32);
                if(!expect(heapA==heapB && heapA!=0,"allocation equality")) return 7;
                if(!baseline.writeMemory(heapA,&n,sizeof(n)) || !candidate.writeMemory(heapB,&n,sizeof(n))) return 8;
                if(!baseline.zeroMemory(heapA+8,8) || !candidate.zeroMemory(heapB+8,8)) return 9;
                signature=hashBytes(signature,ra.data(),ra.size());
            }
        }
        if(scenario==1) {
            uint32_t marker=0;
            if(!baseline.readMemory(0x10400,&marker,sizeof(marker))) return 10;
            if(!expect(marker==1 && baseline.instructions()>40,"VBlank thread execution")) return 10;
        }
        if(scenario==2) {
            uint32_t reason=0;
            if(!baseline.readMemory(0x101c0,&reason,sizeof(reason))) return 11;
            if(!expect(reason==4,"physical CDVD callback executed")) return 11;
        }
        bool advanced=false;
        for(const auto& line:candidate.debugDiagnostics()) {
            if(line.rfind("idle-batch ",0)==0) {
                std::cout<<line<<"\n";
                advanced=line.find("advances=0 ")==std::string::npos;
            }
        }
        if(!reference && !expect(advanced,"candidate must exercise idle batching")) return 12;
        std::cout<<"state scenario="<<scenario<<" cycles="<<baseline.cycles()
            <<" instructions="<<baseline.instructions()<<" pcm_samples="<<pcmSamples
            <<" signature="<<std::hex<<signature<<std::dec<<"\n";
    }
    std::cout<<"differential checkpoints="<<comparisons<<" failures=0\n";
}
