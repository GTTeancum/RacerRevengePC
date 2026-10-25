#pragma once
#include <array>
#include <cstdint>
#include <cmath>
#include <limits>
namespace rr::platform {
class AudioMixer50HD;
// Original EE audio-interface allocation policy, not a guest device. The
// resulting core is needed by the native two-history effect. Native logical
// voice capacity is unchanged. Never derive a core from a logical voice ID.
class NativeSourceVoiceRoutes50KG final {
    friend class AudioMixer50HD;
    friend class NativeRaceIntroTransport50LA;
    enum class Kind : std::uint8_t { unknown, sample, stream };
    struct Cell {
        float age=180.f;
        std::uint16_t generation=0;
        Kind kind=Kind::unknown;
        bool reserved=false;
        // Actual source A3C and loop/pause bits, distinct from PCM cursor.
        float elapsed=0.f,duration=0.f;
        bool sourceBound=false,loop=false,paused=false;
        std::uint8_t streamSlot=0;
        std::uint64_t streamGeneration=0;
    };
    struct Route {
        std::array<std::uint8_t,2> cells{};
        std::array<std::uint16_t,2> generations{};
        std::uint8_t count=0;
    };
    std::array<Cell,48> cells_{};
    bool current(const Route& r)const noexcept {
        if(!r.count||r.count>2)return false;
        for(unsigned i=0;i<r.count;++i){
            if(r.cells[i]>=cells_.size())return false;
            const auto& c=cells_[r.cells[i]];
            if(!c.reserved||c.generation!=r.generations[i]||c.kind==Kind::unknown)return false;
        }
        return r.count!=2||r.cells[0]!=r.cells[1];
    }
    bool release(Route& r)noexcept {
        if(!r.count)return true;
        if(!current(r))return false;
        for(unsigned i=0;i<r.count;++i){auto& c=cells_[r.cells[i]];
            c.reserved=false;
            // Actual26AC80..9C /26ACEC..AD08: stream0, sample3.
            c.age=c.kind==Kind::stream?0.f:3.f;
        }
        r={};return true;
    }
    bool reserve(Kind kind,unsigned channels,Route& out)noexcept {
        if(kind==Kind::unknown||channels<1||channels>2||out.count)return false;
        std::array<unsigned,2> selected{};unsigned found=0;
        const auto index=[&](unsigned corePass,unsigned channelPass){
            return kind==Kind::sample?corePass*24+channelPass:(1-corePass)*24+23-channelPass;
        };
        if(channels==1){
            //266DA0: highest eligible age across the original scan order;
            // stable ties and early exit at the actual180 threshold.
            float best=0.f;
            for(unsigned pass=0;pass<48;++pass){
                const auto n=index(pass/24,pass%24);const auto& c=cells_[n];
                if(c.reserved||!(c.age>2.5f))continue;
                if(!found||c.age>best){selected[0]=n;best=c.age;found=1;}
                if(found&&best>=180.f)break;
            }
        }else{
            //267160 selects the first two eligible channels on one core.
            // A core with only one eligible channel is discarded, not paired
            // across cores. Sample order is ascending; stream order descending.
            for(unsigned corePass=0;corePass<2;++corePass){
                found=0;
                for(unsigned channelPass=0;channelPass<24&&found<2;++channelPass){
                    const auto n=index(corePass,channelPass);const auto& c=cells_[n];
                    if(!c.reserved&&c.age>2.5f)selected[found++]=n;
                }
                if(found==2)break;
            }
        }
        if(found!=channels)return false;
        // Do not overwrite an outstanding stream-slot association after an
        // unusually large source delta. Its real Aural suffix must retire it.
        for(unsigned i=0;i<channels;++i)
            if(cells_[selected[i]].kind==Kind::stream&&cells_[selected[i]].sourceBound)return false;
        Route next;next.count=static_cast<std::uint8_t>(channels);
        for(unsigned i=0;i<channels;++i){auto& c=cells_[selected[i]];
            c.generation=static_cast<std::uint16_t>(c.generation==500?0:c.generation+1);
            c.kind=kind;c.reserved=true;c.age=0.f;c.elapsed=0.f;
            c.duration=0.f;c.sourceBound=false;c.loop=false;c.paused=false;
            c.streamSlot=0;c.streamGeneration=0;
            next.cells[i]=static_cast<std::uint8_t>(selected[i]);next.generations[i]=c.generation;
        }
        out=next;return true;
    }
    // Called only on the unpublished allocation copy, after the actual
    // SourceSound/cache owner has validated its retained sample metadata.
    // No source sample integer or duration is inferred from decoded PCM.
    bool bindSamplePublication266DA0(const Route& route,float duration,bool loop)noexcept {
        if(!current(route)||!std::isfinite(duration)||duration<0.f)return false;
        for(unsigned i=0;i<route.count;++i){const auto& c=cells_[route.cells[i]];
            if(c.kind!=Kind::sample||c.sourceBound||c.elapsed!=0.f||c.age!=0.f)return false;}
        for(unsigned i=0;i<route.count;++i){auto& c=cells_[route.cells[i]];
            c.duration=duration;c.loop=loop;c.paused=false;c.sourceBound=true;}
        return true;
    }
    bool bindStreamPublication266DA0(const Route& route,float duration,bool loop,
        const std::array<std::uint8_t,2>& slots,
        const std::array<std::uint64_t,2>& generations,unsigned count)noexcept {
        if(!current(route)||route.count!=count||!std::isfinite(duration)||duration<0.f||
           (count==2&&slots[0]==slots[1]))return false;
        for(unsigned i=0;i<count;++i){const auto& c=cells_[route.cells[i]];
            if(c.kind!=Kind::stream||c.sourceBound||c.elapsed!=0.f||c.age!=0.f||
               slots[i]>=8||!generations[i])return false;}
        for(unsigned i=0;i<count;++i){auto& c=cells_[route.cells[i]];
            c.duration=duration;c.loop=loop;c.paused=false;c.sourceBound=true;
            c.streamSlot=slots[i];c.streamGeneration=generations[i];}
        return true;
    }
    bool streamReferences267CB0(std::array<std::uint64_t,8>& out)const noexcept {
        std::array<std::uint64_t,8> next{};
        for(const auto& c:cells_)if(c.kind==Kind::stream){
            if(!c.sourceBound||c.streamSlot>=next.size()||!c.streamGeneration||
               next[c.streamSlot])return false;
            next[c.streamSlot]=c.streamGeneration;
        }
        out=next;return true;
    }
    bool setSourcePause26A9D0(const Route& route,bool paused)noexcept {
        if(!current(route))return false;
        for(unsigned i=0;i<route.count;++i)if(!cells_[route.cells[i]].sourceBound)return false;
        for(unsigned i=0;i<route.count;++i)cells_[route.cells[i]].paused=paused;
        return true;
    }
    bool canServiceChannels267CB0(float sourceSeconds)const noexcept {
        if(!std::isfinite(sourceSeconds)||sourceSeconds<0.f)return false;
        for(const auto& c:cells_){
            if(!std::isfinite(c.age)||c.age<0.f)return false;
            if(c.kind==Kind::stream&&(!c.sourceBound||c.streamSlot>=8||!c.streamGeneration))return false;
            if(c.reserved){
                if(c.kind==Kind::unknown||!c.sourceBound||!std::isfinite(c.elapsed)||c.elapsed<0.f||
                   !std::isfinite(c.duration)||c.duration<0.f)return false;
                const float margin=c.kind==Kind::stream?.5f:.25f;
                if(!c.loop&&!c.paused&&(!std::isfinite(c.elapsed+sourceSeconds)||
                   !std::isfinite(c.duration+margin)))return false;
            }else if(c.age<180.f&&!std::isfinite(c.age+sourceSeconds))return false;
        }
        std::array<std::uint64_t,8> references{};
        return streamReferences267CB0(references);
    }
    bool serviceChannels267CB0(float sourceSeconds)noexcept {
        if(!canServiceChannels267CB0(sourceSeconds))return false;
        for(auto& c:cells_){
            if(c.reserved){
                // Original loop/pause branches do not advance source A3C.
                // Stream expiry preserves A40; sample expiry writes3.
                // Neither expiry also visits the free-age increment this time.
                if(!c.loop&&!c.paused){
                    volatile float elapsed=c.elapsed+sourceSeconds;
                    volatile float threshold=c.duration+(c.kind==Kind::stream?.5f:.25f);
                    c.elapsed=elapsed;
                    if(!(elapsed<threshold)){c.reserved=false;if(c.kind==Kind::sample)c.age=3.f;}
                }
            }else{
                //267E80..267FB8 observes the entry age, before267FD8.
                // Aural commits actual slot retirement from this exact preview.
                if(c.kind==Kind::stream&&c.age>1.f){
                    c.kind=Kind::sample;c.streamSlot=0;c.streamGeneration=0;
                }
                if(c.age<180.f){volatile float age=c.age+sourceSeconds;c.age=age;}
            }
        }
        return true;
    }
    bool advanceFreeVoiceAges267FD8(float sourceSeconds)noexcept {
        if(!std::isfinite(sourceSeconds)||sourceSeconds<0.f)return false;
        for(const auto& c:cells_)if(!c.reserved&&c.age<180.f&&!std::isfinite(c.age+sourceSeconds))return false;
        // Called only by a reached source103758 Aural update, never inferred
        // from DSP sample time or the later267BE0 command flush.
        for(auto& c:cells_)if(!c.reserved&&c.age<180.f)c.age+=sourceSeconds;
        return true;
    }
};
}
