#pragma once
#include <array>
#include <cstdint>
namespace rr::game {
class OriginalWreckGroupSparksOwner50LA;
// ONE source stack Config occurrence from15AE1C. Native embedding preserves
// identity across a bounded continuation; no heap allocation or File success.
class OriginalWreckSparksConfig50LA final {
    friend class OriginalWreckGroupSparksOwner50LA;
    std::int64_t file00_;
    std::uint32_t binary08_,opened0C_,mode10_,count14_,field18_,selected20_;
    const void* text1C_;const void* child24_;
    std::array<char,0x400> name28_; //Only byte0 is produced by157354.
    std::uint32_t field428_,field42C_,field430_,field434_,cacheCount438_;
    std::array<std::uint8_t,0x80> cache43C_;
    const void* index4BC_;std::uint32_t indexBytes4C0_;
    const void* block4C4_;std::uint32_t capacity4C8_;const void* names4CC_;
    const char* filename_=nullptr; //Native borrow of SAME source emitter78, not a Config field.
    bool constructed_=false,retired_=false;
    std::uint32_t pc_=0;
    OriginalWreckSparksConfig50LA()noexcept=default;
    ~OriginalWreckSparksConfig50LA()=default;
    OriginalWreckSparksConfig50LA(const OriginalWreckSparksConfig50LA&)=delete;
    OriginalWreckSparksConfig50LA& operator=(const OriginalWreckSparksConfig50LA&)=delete;
    bool fieldsCurrent()const noexcept {
        if(!constructed_||!filename_||file00_!=-1||binary08_||opened0C_||mode10_||count14_||field18_||
           text1C_||selected20_||child24_||name28_[0]||field428_||field42C_||field430_||field434_||
           cacheCount438_||index4BC_||indexBytes4C0_||block4C4_||capacity4C8_||names4CC_)return false;
        for(auto byte:cache43C_)if(byte)return false;
        return true; //Never inspects source-unwritten name28_[1..1023].
    }
    bool construct157320(const char* sameFilename)noexcept {
        if(constructed_||retired_||!sameFilename)return false;
        filename_=sameFilename;pc_=0x157330u;file00_=-1; //SD, not a32bit handle store.
        binary08_=0;opened0C_=0;mode10_=0;count14_=0;field18_=0;
        text1C_=nullptr;selected20_=0;child24_=nullptr;name28_[0]=0;
        field428_=0;field42C_=0;field430_=0;field434_=0;cacheCount438_=0;
        cache43C_.fill(0); //157378→306478, exactly0x80 bytes.
        names4CC_=nullptr;index4BC_=nullptr;indexBytes4C0_=0;block4C4_=nullptr;capacity4C8_=0;
        constructed_=true;pc_=0x15ae2cu;return true; //BEFORE1577D0; no file open/load/Config return.
    }
    bool current()const noexcept{return !retired_&&pc_==0x15ae2cu&&fieldsCurrent();}
    const char* filename()const noexcept{return filename_;}
    bool retireUnopened1573B0()noexcept {
        if(retired_)return retiredCurrent();
        if(!current())return false;
        //Cancellation follows proven1573B0 destructor with a1=-1 (stack object).
        //file00=-1 skips12F290/131040; child24NULL skips its release path.
        pc_=0x157ce4u;selected20_=0;child24_=nullptr; //157CB0 suffix.
        //text1CNULL, cacheCount438=0, names4CC/index4BC/block4C4NULL:
        //all corresponding release branches are skipped, no fake frees.
        cacheCount438_=0; //157478.
        retired_=true;pc_=0x1574f4u;return true; //Negative a1 suppresses2E8B00.
    }
    bool retiredCurrent()const noexcept{return retired_&&pc_==0x1574f4u&&fieldsCurrent();}
};
} //namespace rr::game
