#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
// Independent native byte transform of2B4778..4814, NOT its source invocation.
// It cannot know whether GS/VIF readback happened or whether untouched bytes
// are source-known. No live water owner is wired here and no completion flag
// is accepted. Only a later genuine owner may apply this to source state.
class OriginalWaterReadbackFixup2B4778Native50LA final {
public:
    enum class Status : std::uint8_t {applied,empty,missingStorage,nativeBounds,nativeOverflow};
    struct Result {
        Status status=Status::nativeBounds;
        std::size_t requiredBytes=0;
        std::uint32_t recordsWritten=0;
    };
    static Result applyNumeric(std::uint8_t* bytes,std::size_t capacityBytes,
        std::uint32_t bandsD8,std::uint32_t columnsDC)noexcept {
        // Original jump4770 ->4800 first checks the unsigned band count. No
        // column value or record byte is consumed on the actual zero branch.
        if(!bandsD8)return {Status::empty,0,0};
        // A native bounded, nonwrapped contiguous-record subset. The original
        // wrapped guest pointer/alias path is not mapped onto a host address.
        if(columnsDC<3)return {Status::nativeBounds,0,0};
        const std::uint64_t records=std::uint64_t(bandsD8)*columnsDC;
        if(records>std::uint64_t{0xffffffffu}/368u)
            return {Status::nativeOverflow,0,0};
        const auto required=records*368u;
        if(required>std::numeric_limits<std::size_t>::max())
            return {Status::nativeOverflow,0,0};
        if(!bytes)return {Status::missingStorage,std::size_t(required),0};
        if(capacityBytes<required)return {Status::nativeBounds,std::size_t(required),0};
        // All validation precedes writes. This is native all-or-nothing bounds
        // admission, not a source rollback or claim that earlier MMIO ran.
        for(std::uint32_t band=0;band<bandsD8;++band){
            for(std::uint32_t k=1;k<=3;++k){
                const auto index=(std::uint64_t(band)+1u)*columnsDC-k;
                auto* record=bytes+std::size_t(index*368u);
                std::memset(record+0x10,0,0xf0); //2B47C8: exactly240 bytes.
                // Original stores descend from150 to110. Each qword is the
                // four-word0xFFF sentinel written to443D00..0C beforehand.
                for(std::size_t offset=0x150;;offset-=0x10){
                    for(unsigned lane=0;lane<4;++lane){
                        auto* word=record+offset+4*lane;
                        word[0]=0xff;word[1]=0x0f;word[2]=0;word[3]=0;
                    }
                    if(offset==0x110)break;
                }
                // Keep[0,10),[100,110),[160,170), all other records, and
                // any extra allocation suffix exactly as supplied. We never
                // make those bytes known or initialize them by inference.
            }
        }
        // Bounds above imply3*bands fits32 bits; no source counter is written.
        return {Status::applied,std::size_t(required),bandsD8*3u};
    }
};
}
