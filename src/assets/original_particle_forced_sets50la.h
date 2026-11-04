#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>

namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
class OriginalParticleSourceConfig50LA;
// Native custody for the source emitter's owned uint32_t[count] at1C4.
// Declare this BEFORE the emitter's Config owner: the Config borrows this
// object and must die first. Neither Config1573B0 nor native Config withdrawal
// frees it. Native destruction is NOT a source15A720/2E8AE0 receipt.
class OriginalParticleForcedSets50LA final {
    friend class OriginalParticleSourceConfig50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    using Words=std::array<std::uint32_t,0x260/4>;
    struct Entry15B540 {
        // Unknown native backing is never read without the associated receipt.
        std::array<char,20> key;
        std::size_t keyKnownBytes=0;
        std::optional<std::uint32_t> ordinal;
        bool formatReturned=false,lookupAttempted=false;
        std::optional<bool> found;
        std::uint32_t lookupReturnPC=0,conversionPC=0;
        bool conversionAttempted=false;
        std::optional<std::uint32_t> convertedWord,rawWord;
        bool getterReturned=false,indexAdvanced=false;
        std::uint32_t getterReturnPC=0;
        std::optional<std::uint32_t> loaded15B580,decremented15B584;
        bool decrementStored15B588=false;
        std::optional<std::uint32_t> countRead15B590;
        std::optional<bool> loopAgain15B598;
    };
    const void* emitter_=nullptr; // Native identity only, never a guest word.
    std::optional<std::uint32_t*> pointer1C4_;
    std::unique_ptr<std::uint32_t[]> values_;
    std::unique_ptr<Entry15B540[]> entries_;
    std::optional<std::uint32_t> countRead15B508_,countRead15B528_,allocationBytes_;
    bool oldPointerRead15B514_=false,extentAdmitted_=false;
    bool allocationEntered_=false,allocationReturned_=false,publication15B534_=false;
    std::uint32_t capacity_=0,indexS0_=0,entriesStarted_=0,entriesStored_=0;
    bool indexInitialized15B53C_=false;
    std::optional<std::uint32_t> initialCount15B590_;
    std::optional<bool> initialLoop15B598_;
    bool returned15B5C4_=false;

    bool constructorNull15A70C(const void* emitter)noexcept {
        if(!emitter||emitter_||pointer1C4_||values_||entries_)return false;
        emitter_=emitter;pointer1C4_=nullptr;return true;
    }
    static bool extentFitsNative(std::uint32_t count)noexcept {
        //15B530 SLL count,2 is a 32-bit byte extent, not a fixture limit.
        //The wrapping source path remains unjoined; retain its original count.
        return count&&count<=0xffffffffu/4u&&
            std::uintmax_t(count)<=std::numeric_limits<std::size_t>::max()/sizeof(std::uint32_t)&&
            std::uintmax_t(count)<=std::uintmax_t(std::numeric_limits<std::ptrdiff_t>::max())/sizeof(std::uint32_t)&&
            std::uintmax_t(count)<=std::numeric_limits<std::size_t>::max()/sizeof(Entry15B540)&&
            std::uintmax_t(count)<=std::uintmax_t(std::numeric_limits<std::ptrdiff_t>::max())/sizeof(Entry15B540);
    }
    bool prepareExtentNative(std::uint32_t count)noexcept {
        if(extentAdmitted_||entries_||values_||capacity_||!extentFitsNative(count))return false;
        //Sidecar allocation is native admission, never a source allocator NULL.
        try{entries_.reset(new Entry15B540[count]);}catch(...){return false;}
        capacity_=count;allocationBytes_=count*4u;extentAdmitted_=true;return true;
    }
    bool allocate2E8D90Native()noexcept {
        if(!extentAdmitted_||!entries_||allocationEntered_||values_)return false;
        allocationEntered_=true;
        //2E8D90's new-handler383730/throw-byte383738 paths are not owned here.
        //Only successful native backing admits the original allocation return.
        //A host failure stays pending2E8D90; never publish invented source NULL.
        try{values_.reset(new std::uint32_t[capacity_]);}catch(...){return false;}
        allocationReturned_=true;return true;
    }
    static bool emptyEntry(const Entry15B540& e)noexcept {
        return !e.keyKnownBytes&&!e.ordinal&&!e.formatReturned&&!e.lookupAttempted&&!e.found&&
            !e.lookupReturnPC&&!e.conversionPC&&!e.conversionAttempted&&!e.convertedWord&&!e.rawWord&&
            !e.getterReturned&&!e.getterReturnPC&&!e.indexAdvanced&&!e.loaded15B580&&!e.decremented15B584&&
            !e.decrementStored15B588&&!e.countRead15B590&&!e.loopAgain15B598;
    }
    static bool integerStorePC(std::uint32_t pc)noexcept {
        return pc==0x158968u||pc==0x158990u||pc==0x1589b4u||pc==0x1589d8u||
            pc==0x158a00u||pc==0x158a28u||pc==0x158a44u;
    }
    static bool integerBoundPC(std::uint32_t pc)noexcept {
        return pc==0x309580u||pc==0x308f00u||pc==0x30ac94u||pc==0x30adc0u||pc==0x30ae18u;
    }
    bool currentNative(const void* emitter,const Words& words,const Words& known)const noexcept {
        if(!emitter)return false;
        //A counted partial base constructor may precede the actual15A70C
        //store. Native zero backing does not make source1C4 knownNULL.
        if(!emitter_)return !pointer1C4_&&!values_&&!entries_&&!countRead15B508_&&!countRead15B528_&&
            !allocationBytes_&&!oldPointerRead15B514_&&!extentAdmitted_&&!allocationEntered_&&
            !allocationReturned_&&!publication15B534_&&!capacity_&&!indexS0_&&!entriesStarted_&&!entriesStored_&&
            !indexInitialized15B53C_&&!initialCount15B590_&&!initialLoop15B598_&&!returned15B5C4_&&
            !known[0x1c4/4];
        if(emitter_!=emitter||!pointer1C4_||words[0x1c4/4]!=0||
           known[0x1c4/4]!=(publication15B534_?0u:0xffffffffu))return false;
        if(publication15B534_){
            if(!allocationReturned_||!values_||*pointer1C4_!=values_.get())return false;
        }else if(*pointer1C4_)return false;
        if(countRead15B508_&&(known[0x1c8/4]!=0xffffffffu||words[0x1c8/4]!=*countRead15B508_))return false;
        const bool positive=countRead15B508_&&*countRead15B508_&&((*countRead15B508_&0x80000000u)==0);
        if(oldPointerRead15B514_&&!positive)return false;
        if(countRead15B528_&&(!oldPointerRead15B514_||countRead15B528_!=countRead15B508_))return false;
        if(extentAdmitted_!=bool(entries_)||extentAdmitted_!=bool(allocationBytes_)||extentAdmitted_!=(capacity_!=0))return false;
        if(extentAdmitted_&&(!countRead15B528_||capacity_!=*countRead15B528_||
            !extentFitsNative(capacity_)||*allocationBytes_!=capacity_*4u))return false;
        if(allocationEntered_&&!extentAdmitted_)return false;
        if(allocationReturned_!=bool(values_)||(allocationReturned_&&!allocationEntered_))return false;
        if(indexInitialized15B53C_&&!publication15B534_)return false;
        if(initialCount15B590_&&(!indexInitialized15B53C_||initialCount15B590_!=countRead15B508_))return false;
        if(initialLoop15B598_&&(!initialCount15B590_||!*initialLoop15B598_))return false;
        if(entriesStarted_>capacity_||entriesStored_>entriesStarted_||indexS0_>entriesStarted_||
           entriesStarted_>entriesStored_+1u||indexS0_<entriesStored_||indexS0_>entriesStored_+1u)return false;
        if(entriesStarted_&&initialLoop15B598_!=true)return false;
        std::uint32_t stored=0,advanced=0;
        for(std::uint32_t i=0;i<capacity_;++i){
            const auto& entry=entries_[i];
            if(i>=entriesStarted_){if(!emptyEntry(entry))return false;continue;}
            if(!entry.ordinal||*entry.ordinal!=i+1u||!entry.keyKnownBytes||
               entry.keyKnownBytes>entry.key.size()||entry.key[entry.keyKnownBytes-1]!=0)return false;
            //Verify the exact generated key without consulting unknown tail.
            constexpr char prefix[]="ForcedSet";
            if(entry.keyKnownBytes<11)return false;
            for(std::size_t c=0;c<9;++c)if(entry.key[c]!=prefix[c])return false;
            std::uint32_t ordinal=0;
            for(std::size_t c=9;c+1<entry.keyKnownBytes;++c){
                if(entry.key[c]<'0'||entry.key[c]>'9'||(c==9&&entry.key[c]=='0'))return false;
                const auto digit=static_cast<std::uint32_t>(entry.key[c]-'0');
                if(ordinal>(0x7fffffffu-digit)/10u)return false;ordinal=ordinal*10u+digit;
            }
            if(ordinal!=*entry.ordinal)return false;
            if(entry.lookupAttempted&&!entry.formatReturned)return false;
            if(entry.found&&(!entry.lookupAttempted||entry.lookupReturnPC!=(*entry.found?0x1588b4u:0x1588e4u)))return false;
            if(!entry.found&&entry.lookupReturnPC)return false;
            if(entry.conversionAttempted&&entry.found!=true)return false;
            if(!entry.conversionAttempted&&entry.conversionPC)return false;
            if(entry.convertedWord&&(!entry.conversionAttempted||!integerStorePC(entry.conversionPC)))return false;
            if(entry.conversionAttempted&&!entry.convertedWord&&!integerBoundPC(entry.conversionPC))return false;
            if(entry.rawWord){
                if(!entry.found||!publication15B534_)return false;
                if(*entry.found){if(!entry.convertedWord||entry.rawWord!=entry.convertedWord)return false;}
                else if(*entry.rawWord!=0xffffffffu||entry.conversionPC||entry.conversionAttempted||entry.convertedWord)return false;
                const auto expected=entry.decrementStored15B588?*entry.rawWord-1u:*entry.rawWord;
                if(values_[i]!=expected)return false;
            }
            if(entry.getterReturned&&(!entry.rawWord||entry.getterReturnPC!=0x158b78u))return false;
            if(!entry.getterReturned&&entry.getterReturnPC)return false;
            if(entry.indexAdvanced&&!entry.getterReturned)return false;
            if(entry.loaded15B580&&(!entry.indexAdvanced||entry.loaded15B580!=entry.rawWord))return false;
            if(entry.decremented15B584&&(!entry.loaded15B580||*entry.decremented15B584!=*entry.loaded15B580-1u))return false;
            if(entry.decrementStored15B588&&!entry.decremented15B584)return false;
            if(entry.countRead15B590&&(!entry.decrementStored15B588||entry.countRead15B590!=countRead15B508_))return false;
            if(entry.loopAgain15B598&&(!entry.countRead15B590||*entry.loopAgain15B598!=(i+1u<capacity_)))return false;
            if(i+1u<entriesStarted_&&entry.loopAgain15B598!=true)return false;
            if(entry.decrementStored15B588)++stored;if(entry.indexAdvanced)++advanced;
        }
        if(stored!=entriesStored_||advanced!=indexS0_)return false;
        if(returned15B5C4_){
            if(!countRead15B508_)return false;
            if(positive&&(!publication15B534_||entriesStored_!=capacity_||!capacity_||
                entries_[capacity_-1].loopAgain15B598!=false))return false;
        }
        return true;
    }
public:
    OriginalParticleForcedSets50LA()noexcept=default;
    ~OriginalParticleForcedSets50LA()=default;
    OriginalParticleForcedSets50LA(const OriginalParticleForcedSets50LA&)=delete;
    OriginalParticleForcedSets50LA& operator=(const OriginalParticleForcedSets50LA&)=delete;
    OriginalParticleForcedSets50LA(OriginalParticleForcedSets50LA&&)=delete;
    OriginalParticleForcedSets50LA& operator=(OriginalParticleForcedSets50LA&&)=delete;
};
}
