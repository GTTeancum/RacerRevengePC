#pragma once
#include "../../src/assets/original_standard_archive_provider50la.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <exception>
#include <memory>
#include <string>
#include <thread>

namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace original_world_sky50LA {
class NativeSkyPalette2E38C0;
// Native synchronous member/cursor custody at the actual1295B0 call. Not a
// fabricated131230 physical file ID, guest cache, second decode, or GPU owner.
class NativeSkyCUBFile final {
    friend class NativeSkyPalette2E38C0;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    std::shared_ptr<Provider> provider_;
    Provider::MemberView member_{};
    std::string name_;
    std::thread::id thread_{};
    std::size_t cursor_=0;
    const NativeSkyPalette2E38C0* nativeSelectedPalette_=nullptr;
    bool attempted_=false,opened_=false,closed_=false;
    bool open131230(std::shared_ptr<Provider> provider,const std::string& name,
        const std::uint8_t* authenticatedCUB,std::size_t authenticatedBytes,std::uint32_t& pc)noexcept {
        pc=0x1295ec;
        if(attempted_||closed_||!provider||name!="skies\\ra.cub"||(!authenticatedCUB||authenticatedBytes!=161825u))return false;
        attempted_=true;thread_=std::this_thread::get_id();
        try {
            Provider::MemberView found;
            if(provider->lookup132820(name,found)!=Provider::Lookup::present||
               !found.member||!found.data||!found.bankGeneration||found.size!=authenticatedBytes||
               found.revision!=provider->revision()||
               std::memcmp(found.data,authenticatedCUB,found.size))return false;
            // Caller authenticates the retained RA CUB digest. This equality
            // joins its bytes to the actual provider winner, even if allocations differ.
            name_=name;provider_=std::move(provider);member_=found;opened_=true;return true;
        }catch(...){return false;}
    }
    bool current()const noexcept {
        if(!opened_||closed_||thread_!=std::this_thread::get_id()||!provider_||
           !member_.member||!member_.data||cursor_>member_.size)return false;
        Provider::MemberView now;
        return provider_->lookup132820(name_,now)==Provider::Lookup::present&&
            now.member==member_.member&&now.data==member_.data&&now.size==member_.size&&
            now.mount==member_.mount&&now.ordinal==member_.ordinal&&
            now.revision==member_.revision&&now.bankGeneration==member_.bankGeneration;
    }
    bool read(void* destination,std::size_t bytes)noexcept {
        if(!current()||(!destination&&bytes)||bytes>member_.size-cursor_)return false;
        if(bytes)std::memcpy(destination,member_.data+cursor_,bytes);
        cursor_+=bytes;return true;
    }
    bool readWord(std::uint32_t& out)noexcept {
        std::array<std::uint8_t,4> bytes;
        if(!read(bytes.data(),bytes.size()))return false;
        out=std::uint32_t(bytes[0])|(std::uint32_t(bytes[1])<<8)|
            (std::uint32_t(bytes[2])<<16)|(std::uint32_t(bytes[3])<<24);return true;
    }
    bool seek(std::size_t absolute)noexcept {
        if(!current()||absolute>member_.size)return false;cursor_=absolute;return true;
    }
    bool tell(std::size_t& out)const noexcept {
        if(!current())return false;out=cursor_;return true;
    }
    bool close131040()noexcept {
        if(nativeSelectedPalette_||(attempted_&&thread_!=std::this_thread::get_id()))return false;
        if(closed_)return true;
        opened_=false;member_={};provider_.reset();name_.clear();cursor_=0;closed_=true;return true;
    }
public:
    NativeSkyCUBFile()noexcept=default;
    NativeSkyCUBFile(const NativeSkyCUBFile&)=delete;
    NativeSkyCUBFile& operator=(const NativeSkyCUBFile&)=delete;
    ~NativeSkyCUBFile(){if(!close131040())std::terminate();}
};
// Source2E38C0 inherits1048F0 ref64=1, NOT Entity141300 or global Entity count.
// Only the actual raw RA branch is admitted. The buffer fields below are real
// owned palette data, not copies of decoded sky textures or renderer resources.
class NativeSkyPalette2E38C0 final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    NativeSkyCUBFile* file_=nullptr;
    std::thread::id thread_{};
    std::uint64_t references00_=1;
    std::uint32_t first10_=0,count14_=0,sourceEncoding_=0;
    std::array<std::uint8_t,768> rgb18_;
    std::array<std::uint16_t,256> rgb555_318_,rgb565_518_;
    std::array<std::uint8_t,32768> indices718_;
    std::array<std::uint8_t,65536> lookup8718_;
    // Native partial-custody extents only; no initialized unknown source bytes.
    std::size_t wordsRead_=0,bytesRead_=0;
    bool attempted_=false,complete_=false,released_=false;
    bool load2E38C0(NativeSkyCUBFile& file,std::uint32_t& pc)noexcept {
        pc=0x2e38c0;
        if(attempted_||released_||references00_!=1||!file.current()||
           file.cursor_!=16u||file.nativeSelectedPalette_)return false;
        attempted_=true;thread_=std::this_thread::get_id();file_=&file;
        // Explicit native association: same opened source file's palette borrow.
        // It is NOT a receipt that guest file-manager slot+1C was populated.
        file.nativeSelectedPalette_=this;
        pc=0x2e3908;if(!file.readWord(first10_))return false;++wordsRead_;
        pc=0x2e3930;if(!file.readWord(count14_))return false;++wordsRead_;
        pc=0x2e3954;if(!file.readWord(sourceEncoding_))return false;++wordsRead_;
        if(first10_!=0u||count14_!=226u||sourceEncoding_!=0u)return false;
        pc=0x2e3b7c;if(!file.read(rgb18_.data(),rgb18_.size()))return false;bytesRead_+=rgb18_.size();
        pc=0x2e3ba4;if(!file.read(indices718_.data(),indices718_.size()))return false;bytesRead_+=indices718_.size();
        pc=0x2e3bd0;if(!file.read(lookup8718_.data(),lookup8718_.size()))return false;bytesRead_+=lookup8718_.size();
        // Actual count226 takes2E3BF8..2E3C98, including both table-byte stores.
        const auto index=first10_+count14_;
        pc=0x2e3c14;rgb18_[index*3u]=255;
        pc=0x2e3c34;rgb18_[index*3u+1u]=0;
        pc=0x2e3c54;rgb18_[index*3u+2u]=255;
        pc=0x2e3c6c;indices718_[0x8337u-0x718u]=static_cast<std::uint8_t>(index);
        pc=0x2e3c88;lookup8718_[0x17f37u-0x8718u]=static_cast<std::uint8_t>(index);
        pc=0x2e3c98;++count14_;
        // Integer-only exact source lane widths; no floating-point adaptation.
        for(std::size_t i=0;i<256u;++i){
            const auto r=rgb18_[i*3u],g=rgb18_[i*3u+1u],b=rgb18_[i*3u+2u];
            pc=0x2e3ce4;rgb555_318_[i]=static_cast<std::uint16_t>(((r>>3)<<10)|((g>>3)<<5)|(b>>3));
            pc=0x2e3d28;rgb565_518_[i]=static_cast<std::uint16_t>(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
        }
        pc=0x2e3d48;complete_=true;return true;
    }
    bool current()const noexcept {
        return !released_&&attempted_&&references00_==1&&thread_==std::this_thread::get_id()&&
            file_&&file_->nativeSelectedPalette_==this&&file_->current();
    }
    bool release104970(std::uint32_t& pc)noexcept {
        pc=0x104970;
        if(released_)return false;
        if(attempted_&&(thread_!=std::this_thread::get_id()||!file_||file_->nativeSelectedPalette_!=this))return false;
        if(references00_!=1)return false;
        references00_=0;pc=0x2e3d80;
        // Same native slot withdrawal models the relevant1327D0 lifetime:
        // no provider mount mutation, guest-cache claim, or source file close.
        if(file_)file_->nativeSelectedPalette_=nullptr;
        file_=nullptr;released_=true;complete_=false;pc=0x2e3d8c;return true;
    }
public:
    NativeSkyPalette2E38C0()noexcept=default;
    NativeSkyPalette2E38C0(const NativeSkyPalette2E38C0&)=delete;
    NativeSkyPalette2E38C0& operator=(const NativeSkyPalette2E38C0&)=delete;
    ~NativeSkyPalette2E38C0(){if(!released_){std::uint32_t pc=0;if(!release104970(pc))std::terminate();}}
};
} // namespace original_world_sky50LA
