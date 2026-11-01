#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <vector>
#include <exception>
#include <new>
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_pod_scene50LA {class Owner;}
namespace rr::platform {class OriginalUiCourseOwnedLifecycle50KG;class OriginalUiGarageVisibility50KG;class OriginalUiGarageManagerSearch50KG;class OriginalUiLightManagerOwner50KG;class OriginalUiSearchListCold50KG;}
namespace rr::game {
struct OriginalWreckComponent50LA;
class OriginalNativeListMethodCustody50LA;
// Actual Application lifetime for280890/280920's443138 allocator and443140
// observer count. RA local observers allocate secondary memberships in this
// SAME pool. Existing Garage and RA default owners remain constructor-only.
// Typed native records are wider than source24 bytes: the source4096-byte /
// 169-item accounting is represented explicitly, not as guest-pointer storage.
class OriginalGarageObserverShared50KG final {
    friend class rr::platform::OriginalUiGarageVisibility50KG;
    friend class rr::platform::OriginalUiCourseOwnedLifecycle50KG;
    friend class rr::platform::OriginalUiGarageManagerSearch50KG;
    friend class OriginalApplicationEntities50LA;
    friend class OriginalSourceCollisionObserver50LA;
    friend class OriginalSourcePhysicsModel50LA;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    // Owner-neutral actual process assertion records reached by model28E8A0
    // and Observer2809C0/281490. No separate per-Course copy or readiness flag.
    struct AssertionRecords {
        std::array<std::atomic<std::uint8_t>,10> guards{};
        std::array<std::atomic<std::int32_t>,10> words{};
        std::atomic<std::uint32_t> unknownMask{0};
    };
    static AssertionRecords& assertions()noexcept {static AssertionRecords records;return records;}
    static bool assertion(std::size_t i,bool condition)noexcept {
        if(i>=10)return false;auto& a=assertions();
        if(!a.guards[i].load(std::memory_order_acquire)){
            a.words[i].store(1,std::memory_order_relaxed);a.guards[i].store(1,std::memory_order_release);
        }
        return !a.words[i].load(std::memory_order_relaxed)||condition;
    }
    // Indices1/2/3 are the existing2809C0 process globals. Index6 is
    //1474A0: byteGP-7C68=3D5D08, enabled426F68; index7 is its
    //postcondition byte3D5D0C/enabled426F70. Startup-zero, not per World.
    struct SourceAssertionHistory {
        bool entered=false,ready=false,conditionRead=false,passed=false;
        std::uint8_t guard=0;std::int32_t enabled=0;std::uint8_t stores=0;
    };
    static void invalidateNativeLightAssertions()noexcept {
        assertions().unknownMask.fetch_or((1u<<1)|(1u<<2)|(1u<<3)|(1u<<6)|(1u<<7),std::memory_order_release);
    }
    //14BA00 entry/postcondition: guards3D5D4C/50, words427068/70.
    static void invalidateNativeFlareAssertions()noexcept {
        assertions().unknownMask.fetch_or((1u<<1)|(1u<<2)|(1u<<3)|(1u<<8)|(1u<<9),std::memory_order_release);
    }
    static bool beginSourceAssertion(std::size_t i,SourceAssertionHistory& h)noexcept {
        if(i>=10||h.entered)return false;h.entered=true;auto& a=assertions();
        if(a.unknownMask.load(std::memory_order_acquire)&(1u<<i))return false;
        h.guard=a.guards[i].load(std::memory_order_acquire);
        if(!h.guard){a.words[i].store(1,std::memory_order_relaxed);h.stores|=1;
            a.guards[i].store(1,std::memory_order_release);h.stores|=2;}
        h.enabled=a.words[i].load(std::memory_order_relaxed);h.ready=true;return true;
    }
    static bool finishSourceAssertion(SourceAssertionHistory& h,bool condition)noexcept {
        if(!h.ready||h.conditionRead||h.passed)return false;
        if(h.enabled){h.conditionRead=true;if(!condition)return false;}
        h.passed=true;return true; //False assertion stops before diagnostics/BRK, never returns success.
    }
    static bool sourceAssertionHistoryCurrent(const SourceAssertionHistory& h)noexcept {
        if(!h.entered)return !h.ready&&!h.conditionRead&&!h.passed&&!h.guard&&!h.enabled&&!h.stores;
        if(h.guard>1||h.stores>3||(h.stores&&h.guard)||
           (h.ready&&!h.guard&&h.stores!=3)||(h.conditionRead&&(!h.ready||!h.enabled)))return false;
        return !h.passed||(h.ready&&(!h.enabled||h.conditionRead));
    }
    struct Link105720 {
        Link105720* next00=nullptr;
        const OriginalWreckComponent50LA* child04=nullptr;
        rr::platform::OriginalUiSearchListCold50KG* method08=nullptr;
        // Mutually exclusive typed native alternative for source method08.
        // No additional guest field; never reinterpret a collision List as UI.
        const OriginalNativeListMethodCustody50LA* collisionMethod08=nullptr;
        const OriginalWreckComponent50LA* portal0C=nullptr;
        std::uint32_t distance10=0,slot14=0;
        //Native allocation custody, not an additional source field or refcount.
        //Zero-initialized host storage is not a receipt for source payload stores.
        const void* nativeBorrower=nullptr;
    };
    struct Chunk105870 {
        Chunk105870* next00=nullptr;
        std::array<Link105720,169> records;
    };
    struct Allocator105620 {
        std::uint64_t reference00=1; //1048F0.
        std::uint32_t perBlock10=(4096u-24u)/24u;
        Chunk105870* first14=nullptr;
        Chunk105870* current18=nullptr;
        Chunk105870* cached1C=nullptr;
        std::uint32_t used20=perBlock10,itemSize24=24,blockSize28=4096;
        Link105720* free2C=nullptr;
        std::uint32_t allocatedBlockBytes30=0;
        //105620 does NOT write34. +24/-24 preserves that unknown value.
        //Only actual1058F0 final clearing establishes zero before destruction.
        std::optional<std::uint32_t> allocatedBytes34;
        //Stable native allocations implement source block custody; no new pool.
        std::vector<std::unique_ptr<Chunk105870>> chunks;
    };
    std::unique_ptr<Allocator105620> allocator443138_;
    std::uint32_t observers443140_=0;
    std::vector<const void*> owners_;
    //Native lifetime custody after source280920(-1), NOT another source observer.
    //The retained observer/callback backing owns exact List and link receipts.
    struct DetachedMembership {
        const void* owner=nullptr;const void* occurrence=nullptr;const void* custody=nullptr;
        const void* group=nullptr;const Allocator105620* allocator=nullptr;
    };
    std::vector<DetachedMembership> detachedMemberships_;
    const DetachedMembership* detachedMembership(const void* owner)const noexcept {
        for(const auto& d:detachedMemberships_)if(d.owner==owner)return &d;
        return nullptr;
    }
    bool detachedMembershipCurrent(const void* group,const void* owner,
        const void* occurrence,const void* custody)const noexcept {
        const auto* d=detachedMembership(owner);
        return d&&d->group==group&&d->occurrence==occurrence&&d->custody==custody&&
            sourceGroupCurrent(group);
    }
    bool reserveDetachedMembershipNative(const void* owner)noexcept {
        if(!owner||detachedMembership(owner)||sourceObserver280890_||
           !ownsReturnedSourceObserver(owner)||owner==sourceObserverGroup_||observers443140_<2u)return false;
        try{detachedMemberships_.reserve(detachedMemberships_.size()+1u);}catch(...){return false;}
        return true;
    }
    //Caller authenticates immutable exact List/link receipts before this
    //nonallocating atomic ledger transfer and source count store. No callbacks.
    bool detachMembership28094C(const void* owner,const void* occurrence,const void* custody)noexcept {
        if(!occurrence||!custody||!owner||detachedMembership(owner)||sourceObserver280890_||
           !ownsReturnedSourceObserver(owner)||owner==sourceObserverGroup_||observers443140_<2u||
           detachedMemberships_.size()==detachedMemberships_.capacity())return false;
        for(const auto& d:detachedMemberships_)if(d.occurrence==occurrence||d.custody==custody)return false;
        detachedMemberships_.push_back({owner,occurrence,custody,sourceObserverGroup_,allocator443138_.get()});
        for(auto i=sourceReturnedOwners_.begin();i!=sourceReturnedOwners_.end();++i)
            if(*i==owner){sourceReturnedOwners_.erase(i);break;}
        for(auto i=owners_.begin();i!=owners_.end();++i)if(*i==owner){owners_.erase(i);break;}
        --observers443140_;return true;
    }
    bool releaseDetachedMembershipNative(const void* owner,const void* custody)noexcept {
        const auto* d=detachedMembership(owner);
        if(!d||d->custody!=custody||!sourceGroupCurrent(d->group)||ownerHasLinks(owner))return false;
        //Exact List slots/pins must already have been retired by receipt owner.
        for(auto i=detachedMemberships_.begin();i!=detachedMemberships_.end();++i)
            if(i->owner==owner){detachedMemberships_.erase(i);return true;}
        return false;
    }
    //Native provenance for completed source280890 invocations. These remain
    //in the SAME owners_/443140 ledger and443138 pool; not a second allocator.
    std::vector<const void*> sourceReturnedOwners_;
    bool sourceReturnedRecorded(const void* owner)const noexcept {
        for(const auto* p:sourceReturnedOwners_)if(p==owner)return true;
        return false;
    }
    bool ownerRecorded(const void* owner)const noexcept {
        if(!owner)return false;
        for(const auto* p:owners_)if(p==owner)return true;
        return false;
    }
    bool allocatedRecord(const Link105720* record)const noexcept {
        if(!record||!allocator443138_)return false;
        const auto& a=*allocator443138_;
        for(std::size_t c=0;c<a.chunks.size();++c){
            const auto* chunk=a.chunks[c].get();if(!chunk)return false;
            const auto used=c+1u==a.chunks.size()?a.used20:a.perBlock10;
            if(used>169u)return false;
            for(std::size_t i=0;i<used;++i)if(record==&chunk->records[i])return true;
        }
        return false;
    }
    //Exclusive native custody for one executing fresh280890 frame. These are
    //not duplicate guest globals. The existing443138/443140/owners remain sole authority.
    const void* sourceObserver280890_=nullptr;
    std::uint32_t sourceObserverBaselineCount_=0;
    const Allocator105620* sourceObserverBaselineAllocator_=nullptr;
    bool sourceObserverAllocatorReached_=false,sourceObserverCountStored_=false;
    //Same-App source cohort retains exclusion against legacy/native mutators
    //after constructor return. Child constructors must name this exact root.
    const void* sourceObserverGroup_=nullptr;
    std::uint32_t sourceGroupBaselineCount_=0;
    std::vector<const void*> sourceGroupBaselineOwners_;
    const Allocator105620* sourceGroupBaselineAllocator_=nullptr;
    const Allocator105620* sourceGroupRetainedAllocator_=nullptr;
    bool coherent()const noexcept {return !sourceObserverGroup_&&!sourceObserver280890_&&coherentState(false);}
    bool sourceGroupCurrent(const void* group)const noexcept {
        if(!group||sourceObserverGroup_!=group||!coherentState(true)||
           observers443140_!=sourceGroupBaselineCount_+sourceReturnedOwners_.size()+(sourceObserverCountStored_?1u:0u)||
           (sourceGroupBaselineAllocator_&&allocator443138_.get()!=sourceGroupBaselineAllocator_))return false;
        if(sourceGroupBaselineOwners_.size()!=sourceGroupBaselineCount_)return false;
        for(std::size_t i=0;i<sourceGroupBaselineOwners_.size();++i)
            if(owners_[i]!=sourceGroupBaselineOwners_[i])return false;
        for(std::size_t i=0;i<sourceReturnedOwners_.size();++i)
            if(owners_[sourceGroupBaselineCount_+i]!=sourceReturnedOwners_[i])return false;
        if(!sourceReturnedOwners_.empty()&&(sourceReturnedOwners_.front()!=group||
           !sourceGroupRetainedAllocator_||allocator443138_.get()!=sourceGroupRetainedAllocator_))return false;
        if(sourceObserverCountStored_&&(!sourceObserver280890_||owners_.back()!=sourceObserver280890_))return false;
        return true;
    }
    bool coherentState(bool sourceFrame)const noexcept {
        if(observers443140_!=owners_.size()||
           (!sourceFrame&&bool(allocator443138_)!=bool(observers443140_)))return false;
        for(std::size_t i=0;i<owners_.size();++i){if(!owners_[i])return false;
            for(std::size_t j=0;j<i;++j)if(owners_[i]==owners_[j])return false;}
        for(std::size_t i=0;i<sourceReturnedOwners_.size();++i){
            const auto* p=sourceReturnedOwners_[i];
            if(!p||p==sourceObserver280890_||!ownerRecorded(p))return false;
            for(std::size_t j=0;j<i;++j)if(sourceReturnedOwners_[j]==p)return false;
        }
        for(std::size_t i=0;i<detachedMemberships_.size();++i){const auto& d=detachedMemberships_[i];
            if(!sourceFrame||!d.owner||!d.occurrence||!d.custody||!d.group||
               d.group!=sourceObserverGroup_||!ownerRecorded(d.group)||!sourceReturnedRecorded(d.group)||
               d.owner==sourceObserver280890_||ownerRecorded(d.owner)||sourceReturnedRecorded(d.owner)||
               !d.allocator||d.allocator!=allocator443138_.get()||d.allocator!=sourceGroupRetainedAllocator_)return false;
            for(std::size_t j=0;j<i;++j)if(detachedMemberships_[j].owner==d.owner||
                detachedMemberships_[j].custody==d.custody||detachedMemberships_[j].occurrence==d.occurrence)return false;
        }
        if(!allocator443138_)return detachedMemberships_.empty();
        const auto& a=*allocator443138_;
        if(a.reference00!=1||a.perBlock10!=169||a.itemSize24!=24||a.blockSize28!=4096||
           a.cached1C||a.allocatedBytes34||a.chunks.size()>std::numeric_limits<std::uint32_t>::max()/4096u||
           a.allocatedBlockBytes30!=a.chunks.size()*4096u)return false;
        //No105990 cache producer has been admitted. Do not simulate that branch.
        if(a.chunks.empty())return a.used20==169&&!a.first14&&!a.current18&&!a.free2C;
        if(!a.used20||a.used20>169u||a.first14!=a.chunks.front().get()||
           a.current18!=a.chunks.back().get())return false;
        std::size_t usedTotal=0,liveTotal=0;
        for(std::size_t c=0;c<a.chunks.size();++c){
            const auto* chunk=a.chunks[c].get();
            if(!chunk||chunk->next00!=(c+1u<a.chunks.size()?a.chunks[c+1u].get():nullptr))return false;
            const auto used=c+1u==a.chunks.size()?a.used20:a.perBlock10;usedTotal+=used;
            for(std::size_t i=0;i<169u;++i){const auto* borrower=chunk->records[i].nativeBorrower;
                if(i>=used){if(borrower)return false;}
                else if(borrower){if(!ownerRecorded(borrower)&&!detachedMembership(borrower))return false;++liveTotal;}
            }
        }
        std::size_t freeTotal=0;
        for(auto* record=a.free2C;record;record=record->next00){
            //Validate address before any dereference; cycle/repetition cannot
            //terminate within usedTotal. Live source next00 is caller-owned.
            if(++freeTotal>usedTotal||!allocatedRecord(record)||record->nativeBorrower)return false;
        }
        return freeTotal+liveTotal==usedTotal;
    }
    bool beginSourceObserver280890(const void* owner,const void* group=nullptr)noexcept {
        //Native bookkeeping reserve occurs before any represented280890 instruction.
        //Failure is a native admission refusal, never a guest allocation return.
        if(!owner||ownerRecorded(owner)||detachedMembership(owner)||sourceObserver280890_||
           observers443140_==std::numeric_limits<std::uint32_t>::max())return false;
        if(sourceObserverGroup_){
            if(!group||!sourceGroupCurrent(group)||sourceReturnedOwners_.empty())return false;
        }else if(group||!coherent())return false;
        try{
            owners_.reserve(owners_.size()+1u);sourceReturnedOwners_.reserve(sourceReturnedOwners_.size()+1u);
            if(!sourceObserverGroup_)sourceGroupBaselineOwners_=owners_;
        }catch(...){return false;}
        if(!sourceObserverGroup_){
            sourceObserverGroup_=owner;sourceGroupBaselineCount_=observers443140_;
            sourceGroupBaselineAllocator_=allocator443138_.get();
        }
        sourceObserver280890_=owner;sourceObserverBaselineCount_=observers443140_;
        sourceObserverBaselineAllocator_=allocator443138_.get();
        sourceObserverAllocatorReached_=sourceObserverCountStored_=false;return true;
    }
    bool sourceObserverCurrent280890(const void* owner)const noexcept {
        if(!owner||sourceObserver280890_!=owner||!sourceGroupCurrent(sourceObserverGroup_)||
           observers443140_!=sourceObserverBaselineCount_+(sourceObserverCountStored_?1u:0u)||
           (sourceObserverBaselineAllocator_&&allocator443138_.get()!=sourceObserverBaselineAllocator_))return false;
        if(!sourceObserverAllocatorReached_&&allocator443138_.get()!=sourceObserverBaselineAllocator_)return false;
        if(sourceObserverCountStored_)
            return !owners_.empty()&&owners_.back()==owner&&ownerRecorded(owner);
        return !ownerRecorded(owner);
    }
    bool sourceObserverAllocator2808AC(const void* owner,std::uint32_t& pc)noexcept {
        pc=0x2808acu;
        if(!sourceObserverCurrent280890(owner)||sourceObserverAllocatorReached_)return false;
        if(!allocator443138_){
            pc=0x2808b8u;
            //Typed host size differs from guest38. The actual allocation is nullable.
            //105620/1048F0's represented stores are supplied by this same pool type;
            //its untouched34 remains unknown. No blocks or links are allocated.
            std::unique_ptr<Allocator105620> returned(new(std::nothrow) Allocator105620);
            pc=0x2808dcu;allocator443138_=std::move(returned); //Includes genuine NULL.
        }
        sourceObserverAllocatorReached_=true;pc=0x2808e0u;return true;
    }
    bool sourceObserverIncrement28090C(const void* owner,std::uint32_t& pc)noexcept {
        pc=0x2808fcu;
        if(!sourceObserverCurrent280890(owner)||!sourceObserverAllocatorReached_||sourceObserverCountStored_)return false;
        //Capacity was reserved before the source frame; pointer insertion cannot allocate.
        owners_.push_back(owner);
        pc=0x28090cu;++observers443140_;sourceObserverCountStored_=true;
        pc=0x280918u;return true; //NULL443138 does not suppress the source count or return.
    }
    bool finishSourceObserver280918(const void* owner)noexcept {
        //Only after all280890 scalar/vector stores and actual count publication.
        //This transfers exclusive constructor custody to retained source ownership;
        //it changes NO guest count, owner, allocator, links or borrowed payload.
        //A NULL pool returned by source allocation remains a bounded active frame,
        //not silently replaced by a fresh allocator or normal live-pool readiness.
        if(!sourceObserverCurrent280890(owner)||!sourceObserverAllocatorReached_||
           !sourceObserverCountStored_||!allocator443138_||sourceReturnedRecorded(owner)||
           sourceReturnedOwners_.size()==sourceReturnedOwners_.capacity())return false;
        if(sourceReturnedOwners_.empty())sourceGroupRetainedAllocator_=allocator443138_.get();
        sourceReturnedOwners_.push_back(owner); //Capacity reserved before280890.
        sourceObserver280890_=nullptr;sourceObserverBaselineAllocator_=nullptr;
        sourceObserverBaselineCount_=0;sourceObserverAllocatorReached_=sourceObserverCountStored_=false;
        return sourceGroupCurrent(sourceObserverGroup_);
    }
    bool ownsReturnedSourceObserver(const void* owner)const noexcept {
        //A later source constructor may hold its own exclusive frame while this
        //previously completed owner continues borrowing the unchanged shared pool.
        if(sourceObserver280890_){if(!sourceObserverCurrent280890(sourceObserver280890_))return false;}
        else if(!sourceGroupCurrent(sourceObserverGroup_))return false;
        return owner&&sourceReturnedRecorded(owner)&&ownerRecorded(owner);
    }
    bool retireReturnedSourceObserverNative(const void* owner)noexcept {
        //Native-only withdrawal, NOT execution/verification of guest280920.
        //No active constructor may lose its baseline owner/pool underneath it.
        if(sourceObserver280890_||(owner==sourceObserverGroup_&&!detachedMemberships_.empty())||
           !sourceGroupCurrent(sourceObserverGroup_)||
           !ownsReturnedSourceObserver(owner)||sourceReturnedOwners_.back()!=owner||ownerHasLinks(owner))return false;
        for(auto i=sourceReturnedOwners_.begin();i!=sourceReturnedOwners_.end();++i)
            if(*i==owner){sourceReturnedOwners_.erase(i);break;}
        for(auto i=owners_.begin();i!=owners_.end();++i)if(*i==owner){owners_.erase(i);break;}
        --observers443140_;
        //Reverse source withdrawal preserves every baseline owner and pool.
        //The root cannot retire while a later source child is still recorded.
        if(!sourceReturnedOwners_.empty())return sourceGroupCurrent(sourceObserverGroup_);
        if(observers443140_!=sourceGroupBaselineCount_)return false;
        if(!sourceGroupBaselineAllocator_)allocator443138_.reset();
        sourceObserverGroup_=nullptr;sourceGroupBaselineCount_=0;sourceGroupBaselineOwners_.clear();sourceGroupBaselineAllocator_=nullptr;sourceGroupRetainedAllocator_=nullptr;
        return coherent();
    }
    bool abortSourceObserver280890(const void* owner)noexcept {
        if(!sourceObserverCurrent280890(owner)||ownerHasLinks(owner)||
           (!sourceObserverBaselineAllocator_&&!detachedMemberships_.empty()))return false;
        if(sourceObserverCountStored_){owners_.pop_back();--observers443140_;}
        //Native-only unwind of this invocation. Existing pool/chunks/links and
        //other owners are untouched. No guest280920 completion is claimed.
        if(!sourceObserverBaselineAllocator_)allocator443138_.reset();
        sourceObserver280890_=nullptr;sourceObserverBaselineAllocator_=nullptr;
        sourceObserverBaselineCount_=0;sourceObserverAllocatorReached_=sourceObserverCountStored_=false;
        if(!sourceReturnedOwners_.empty())return sourceGroupCurrent(sourceObserverGroup_);
        sourceObserverGroup_=nullptr;sourceGroupBaselineCount_=0;sourceGroupBaselineOwners_.clear();sourceGroupBaselineAllocator_=nullptr;sourceGroupRetainedAllocator_=nullptr;
        return coherent();
    }
    bool idleForApplicationRelease()const noexcept {
        return coherent()&&!observers443140_&&!allocator443138_&&owners_.empty()&&detachedMemberships_.empty();
    }
    bool owns(const void* owner)const noexcept {return coherent()&&ownerRecorded(owner);}
    bool ownsLink(const void* owner,const Link105720* link)const noexcept {
        return owns(owner)&&allocatedRecord(link)&&link->nativeBorrower==owner;
    }
    bool ownerHasLinks(const void* owner)const noexcept {
        if(!owner||!allocator443138_)return false;
        for(const auto& chunk:allocator443138_->chunks)if(chunk)
            for(const auto& record:chunk->records)if(record.nativeBorrower==owner)return true;
        return false;
    }
    enum class SourceLinkStatus : std::uint8_t { nativeRefused,returnedNull,returnedRecord };
    struct SourceLinkResult {
        SourceLinkStatus status=SourceLinkStatus::nativeRefused;
        Link105720* record=nullptr;
        std::uint32_t sourcePC=0x105720;
    };
    bool sourceLinkAccessCurrent(const void* group,const void* owner)const noexcept {
        //Keep source-cohort exclusion; no temporary release of coherent().
        //An unfinished constructor (including poolNULL/count-positive) cannot
        //borrow records, nor can a baseline/foreign observer enter this route.
        return !sourceObserver280890_&&sourceGroupCurrent(group)&&
            ownsReturnedSourceObserver(owner)&&allocator443138_;
    }
    bool ownsSourceLink(const void* group,const void* owner,const Link105720* link)const noexcept {
        //Ownership/native withdrawal can survive source observer death; allocation
        //below intentionally retains ACTIVE-only sourceLinkAccessCurrent.
        return !sourceObserver280890_&&sourceGroupCurrent(group)&&
            (ownsReturnedSourceObserver(owner)||detachedMembership(owner))&&
            allocatedRecord(link)&&link->nativeBorrower==owner;
    }
    SourceLinkResult allocateSource105720(const void* group,const void* owner)noexcept {
        if(!sourceLinkAccessCurrent(group,owner))return {};
        auto& a=*allocator443138_;Link105720* record=nullptr;
        //Source105738 chooses unused tail BEFORE105760 free-list reuse.
        if(a.used20<a.perBlock10){
            record=&a.current18->records[a.used20];++a.used20; //105754.
        }else if(a.free2C){
            record=a.free2C;a.free2C=record->next00; //10576C/105774.
        }else{
            //The same shared pool still excludes unsupported cached1C producer.
            //Native width is larger than guest4096; native ledger reservation
            //must refuse BEFORE the represented105870 allocation attempt.
            if(a.cached1C||a.allocatedBlockBytes30>std::numeric_limits<std::uint32_t>::max()-4096u)
                return {};
            try{a.chunks.reserve(a.chunks.size()+1u);}catch(...){return {};}
            //105880->305700 is nullable;105890/1057E8 propagateNULL without
            //pool publications or the unknown34+24 write. This is distinct from
            //native admission refusal and from observer caller280CD0 success.
            std::unique_ptr<Chunk105870> returned(new(std::nothrow) Chunk105870);
            if(!returned)return {SourceLinkStatus::returnedNull,nullptr,0x105824};
            auto* actual=returned.get();a.chunks.push_back(std::move(returned));
            if(!a.first14){a.current18=actual;a.first14=actual;} //1058A4/AC.
            else{a.current18->next00=actual;a.current18=actual;} //1058B4/B8.
            actual->next00=nullptr; //1058C4, not record payload initialization.
            a.allocatedBlockBytes30+=4096u; //1058D4.
            record=&actual->records[0];a.used20=1; //1057F0..105800.
        }
        //10580C/105818 preserve unknown34 through unknown+24. The native tag
        //only pins custody; caller must still perform its ordered payload stores.
        record->nativeBorrower=owner;record->collisionMethod08=nullptr;
        return {SourceLinkStatus::returnedRecord,record,0x105824};
    }
    bool releaseSource105830(const void* group,const void* owner,Link105720* record,
        std::uint32_t& pc)noexcept {
        pc=0x105830;
        if(!ownsSourceLink(group,owner,record))return false;
        auto& a=*allocator443138_;
        pc=0x105844;record->next00=a.free2C;
        pc=0x105848;a.free2C=record;record->nativeBorrower=nullptr;
        //Known source itemSize24==24 passes105838. Unknown34 remains unknown
        //after105858 subtraction. Retain other payload and block/free history.
        pc=0x10585c;return true;
    }
    Link105720* allocate105720(const void* owner)noexcept {
        if(!owns(owner))return nullptr;
        auto& a=*allocator443138_;Link105720* result=nullptr;
        //105738 prefers unused current-tail capacity even when free2C exists.
        if(a.used20<a.perBlock10){result=&a.current18->records[a.used20];++a.used20;}
        else if(a.free2C){result=a.free2C;a.free2C=result->next00;} //105760..774.
        else {
            //cached1C is genuinely null in this admitted domain;1057D8->105870.
            if(a.allocatedBlockBytes30>std::numeric_limits<std::uint32_t>::max()-4096u)return nullptr;
            try {
                a.chunks.reserve(a.chunks.size()+1u);
                auto chunk=std::make_unique<Chunk105870>();auto* actual=chunk.get();
                a.chunks.push_back(std::move(chunk)); //Native custody before source publication.
                if(a.current18)a.current18->next00=actual;else a.first14=actual;
                a.current18=actual;actual->next00=nullptr;a.allocatedBlockBytes30+=4096u;
                a.used20=1;result=&actual->records[0];
            }catch(...){return nullptr;}
        }
        result->nativeBorrower=owner;
        result->collisionMethod08=nullptr; // Native tag withdrawal on reuse, not guest payload reset.
        //10580C reads unknown34 and105818 writes unknown+24: stays unknown.
        //The caller publishes all six source payload fields after this returns.
        return result;
    }
    bool release105830(const void* owner,Link105720* link)noexcept {
        if(!ownsLink(owner,link))return false;
        auto& a=*allocator443138_;
        link->next00=a.free2C;a.free2C=link;link->nativeBorrower=nullptr;
        //105850..858 unknown34 minus24 remains unknown; other payload retained.
        return true;
    }
    bool construct280890(const void* owner)noexcept {
        if(!owner||!coherent()||owns(owner)||observers443140_==std::numeric_limits<std::uint32_t>::max())return false;
        try {
            owners_.reserve(owners_.size()+1u);
            if(!allocator443138_)allocator443138_=std::make_unique<Allocator105620>();
            owners_.push_back(owner);++observers443140_;return true;
        }catch(...){return false;}
    }
    bool release280920(const void* owner)noexcept {
        //281490 must have removed every secondary before observer destruction.
        //This also protects still-live records owned by a different observer.
        if(!owns(owner)||sourceReturnedRecorded(owner)||ownerHasLinks(owner))return false;
        for(auto i=owners_.begin();i!=owners_.end();++i)if(*i==owner){owners_.erase(i);break;}
        --observers443140_;
        if(!observers443140_){
            auto& a=*allocator443138_;--a.reference00; //280970->104970 last reference.
            //1058F0 destroys actual chunks in source first14/next00 order.
            //No cached chain exists. Free2C is not dereferenced after its backing.
            for(auto& chunk:a.chunks)chunk.reset();a.chunks.clear();
            a.first14=nullptr;a.current18=nullptr;a.used20=a.perBlock10;
            a.free2C=nullptr;a.allocatedBlockBytes30=0;a.allocatedBytes34=0;a.cached1C=nullptr;
            allocator443138_.reset(); //280984 clears443138 after actual destructor.
        }
        return true;
    }
public:
    OriginalGarageObserverShared50KG()=default;
    ~OriginalGarageObserverShared50KG(){if(!detachedMemberships_.empty()||sourceObserverGroup_||sourceObserver280890_||!sourceReturnedOwners_.empty()||observers443140_||allocator443138_)std::terminate();}
    OriginalGarageObserverShared50KG(const OriginalGarageObserverShared50KG&)=delete;
    OriginalGarageObserverShared50KG& operator=(const OriginalGarageObserverShared50KG&)=delete;
};
}
