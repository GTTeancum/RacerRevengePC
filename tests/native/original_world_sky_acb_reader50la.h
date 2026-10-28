#pragma once
#include "../../src/assets/original_standard_archive_provider50la.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <exception>
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace original_world_sky50LA {
// Only the reached failed-open branch of stack Config157320/1577D0/1573B0.
// Native negative transport against the SAME real provider; no guest physical
// file/cache/fallback claim, allocation of parser resources, or Entity count.
class NativeAbsentSkyACB157320 final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    enum class Stage:std::uint8_t {fresh,attempted,absent,retired};
    Stage stage_=Stage::fresh;
    std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<Provider> provider_;
    std::string filename_;
    std::uint64_t observedRevision_=0;
    Provider::Lookup observedResult_=Provider::Lookup::unknown;
    bool absenceRetired_=false,constructorReached_=false;
    // Native storage defaults only until constructorReached_ at129A0C.
    // Then these are the exact reached constructor-known lanes. The remainder of the
    // source character buffer29..427 is neither stored nor claimed known.
    std::int64_t handle00_=-1;
    std::array<std::uint32_t,8> fields08Through24_{};
    std::uint8_t firstCharacter28_=0;
    std::array<std::uint32_t,5> fields428Through438_{};
    std::array<std::uint8_t,128> rows43C_{};
    std::array<std::uint32_t,5> fields4BCThrough4CC_{};
    bool knownFresh157320()const noexcept {
        if(handle00_!=-1||firstCharacter28_)return false;
        for(auto v:fields08Through24_)if(v)return false;
        for(auto v:fields428Through438_)if(v)return false;
        for(auto v:rows43C_)if(v)return false;
        for(auto v:fields4BCThrough4CC_)if(v)return false;
        return true;
    }
    bool absentCurrent()const noexcept {
        if(!constructorReached_||stage_!=Stage::absent||thread_!=std::this_thread::get_id()||!provider_||
           !knownFresh157320()||!provider_->sealed()||provider_->revision()!=observedRevision_||
           observedResult_!=Provider::Lookup::absent)return false;
        Provider::MemberView now;
        return provider_->lookup132820(filename_,now)==Provider::Lookup::absent&&
            now.revision==observedRevision_;
    }
    bool attempt1577D0(std::shared_ptr<Provider> provider,const std::string& actualCubName,
        std::uint32_t& pc)noexcept {
        pc=0x1299a0;
        if(stage_!=Stage::fresh||thread_!=std::this_thread::get_id()||!knownFresh157320())return false;
        stage_=Stage::attempted; // Any failure is one-shot, never a retry/reset.
        try {
            if(!provider||actualCubName!="skies\\ra.cub")return false;
            filename_=actualCubName;
            pc=0x1299d0;
            const auto dot=filename_.find_last_of('.'); // Actual1169D0 reverse scan.
            if(dot==std::string::npos){pc=0x129a00;filename_+=".acb";}
            else {pc=0x1299e8;filename_.replace(dot,std::string::npos,".acb");}
            if(filename_!="skies\\ra.acb")return false;
            pc=0x129a0c;constructorReached_=true; // Source157320 after name conversion.
            provider_=std::move(provider);
            pc=0x1578d0;
            Provider::MemberView found;
            observedResult_=provider_->lookup132820(filename_,found);
            observedRevision_=found.revision;
            // Unknown is NOT absence; a positive member is unsupported, NOT
            // a fabricated opened handle. Keep observed outcome for cleanup.
            if(observedResult_!=Provider::Lookup::absent||!provider_->sealed()||
               provider_->revision()!=observedRevision_)return false;
            pc=0x1578d8;handle00_=-1;
            pc=0x1578f0;stage_=Stage::absent;
            return absentCurrent(); // True means supported negative result, not source read success.
        }catch(...){return false;}
    }
    bool retire1573B0(std::uint32_t& pc)noexcept {
        pc=constructorReached_?0x129a30:0;
        if(thread_!=std::this_thread::get_id()||stage_==Stage::retired||!knownFresh157320())return false;
        if(!constructorReached_){
            // Only native pre-constructor transport storage exists. No source
            //1573B0 call, field retirement, or epilogue receipt is minted.
            provider_.reset();stage_=Stage::retired;absenceRetired_=false;return true;
        }
        const bool admitted=absentCurrent();
        // Source handle−1 skips12F290/131040;157CB0 sees null24, then
        // zeros20/24. All other reached pointer/count tests are zero: nofree.
        pc=0x157ce4;fields08Through24_[6]=0;
        pc=0x157ce8;fields08Through24_[7]=0;
        pc=0x157478;fields428Through438_[4]=0;
        // Source a1=-1 prevents deletion of the stack reader. Native string
        // and provider pins are transport custody, not source parser buffers.
        provider_.reset();stage_=Stage::retired;absenceRetired_=admitted;
        pc=0x12a0d4;return true;
    }
public:
    NativeAbsentSkyACB157320()noexcept=default;
    NativeAbsentSkyACB157320(const NativeAbsentSkyACB157320&)=delete;
    NativeAbsentSkyACB157320& operator=(const NativeAbsentSkyACB157320&)=delete;
    ~NativeAbsentSkyACB157320(){
        if(stage_!=Stage::retired){std::uint32_t pc=0;if(!retire1573B0(pc))std::terminate();}
    }
};
} // namespace original_world_sky50LA
