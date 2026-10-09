#pragma once
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <stdexcept>
// Owned by the serialized native GS backend. Elapsed time includes the complete
// DLL call: producer work, queued GS work, waits and host preemption. It is not
// exclusive CPU time, GPU execution time or a measurement of queue wait alone.
namespace rr_gs47o {
class Profile {
    using Clock=std::chrono::steady_clock;
    struct Metric {uint64_t calls=0,ns=0;};
    mutable Metric values[9]{};
    mutable uint64_t packet_bytes=0,buckets[6]{};
    std::FILE* file=nullptr;
    Clock::time_point begin{};
public:
    enum Kind {Packet,Vsync,Flush,Fifo,Snapshot,Vram,Read,Reset,Open};
    Profile(){
        const char* path=std::getenv("RR_PROFILE_GS47O");
        if(path&&*path){file=std::fopen(path,"wb");if(!file)throw std::runtime_error("Cannot open GS47O profile");
            begin=Clock::now();std::fputs("stage,vsync_calls,host_ns,metric,calls,elapsed_ns,packet_bytes,p16,p64,p256,p4096,p65536,plarge\n",file);}
    }
    Profile(const Profile&)=delete;Profile& operator=(const Profile&)=delete;
    ~Profile(){if(file){dump("close");std::fclose(file);}}
    template<class F> int invoke(Kind kind,F&& fn) const {
        if(!file)return std::forward<F>(fn)();
        auto start=Clock::now();
        struct End {const Profile& owner;Kind kind;Clock::time_point start;~End(){auto& m=owner.values[kind];++m.calls;m.ns+=std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-start).count();}} end{*this,kind,start};
        return std::forward<F>(fn)();
    }
    void packet(uint32_t bytes){if(file){packet_bytes+=bytes;unsigned i=bytes<=16?0:bytes<=64?1:bytes<=256?2:bytes<=4096?3:bytes<=65536?4:5;++buckets[i];}}
    void tick(){if(file && values[Vsync].calls%100==0)dump("vsync");}
    void dump(const char* stage) const {
        if(!file)return;
        static const char* names[]={"packet","vsync","flush","fifo","snapshot","vram","read","reset","open"};
        auto host=std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-begin).count();
        for(unsigned i=0;i<9;++i)std::fprintf(file,"%s,%llu,%llu,%s,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu\n",stage,
            (unsigned long long)values[Vsync].calls,(unsigned long long)host,names[i],(unsigned long long)values[i].calls,(unsigned long long)values[i].ns,
            (unsigned long long)packet_bytes,(unsigned long long)buckets[0],(unsigned long long)buckets[1],(unsigned long long)buckets[2],(unsigned long long)buckets[3],(unsigned long long)buckets[4],(unsigned long long)buckets[5]);
        std::fflush(file);
    }
};
}
