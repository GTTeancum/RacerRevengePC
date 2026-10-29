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
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    // Owner-neutral actual process assertion records reached by model28E8A0
    // and Observer2809C0/281490. No separate per-Course copy or readiness flag.
    struct AssertionRecords {
        std::array<std::atomic<std::uint8_t>,6> guards{};
        std::array<std::atomic<std::int32_t>,6> words{};
    };
    static AssertionRecords& assertions()noexcept {static AssertionRecords records;return records;}
    static bool assertion(std::size_t i,bool condition)noexcept {
        if(i>=6)return false;auto& a=assertions();
        if(!a.guards[i].load(std::memory_order_acquire)){
            a.words[i].store(1,std::memory_order_relaxed);a.guards[i].store(1,std::memory_order_release);
        }
        return !a.words[i].load(std::memory_order_relaxed)||condition;
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
    bool coherent()const noexcept {return !sourceObserver280890_&&coherentState(false);}
    bool coherentState(bool sourceFrame)const noexcept {
        if(observers443140_!=owners_.size()||
           (!sourceFrame&&bool(allocator443138_)!=bool(observers443140_)))return false;
        for(std::size_t i=0;i<owners_.size();++i){if(!owners_[i])return false;
            for(std::size_t j=0;j<i;++j)if(owners_[i]==owners_[j])return false;}
        if(!allocator443138_)return true;
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
                else if(borrower){if(!ownerRecorded(borrower))return false;++liveTotal;}
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
    bool beginSourceObserver280890(const void* owner)noexcept {
        //Native bookkeeping reserve occurs before any represented280890 instruction.
        //Failure is a native admission refusal, never a guest allocation return.
        if(!owner||!coherent()||ownerRecorded(owner)||
           observers443140_==std::numeric_limits<std::uint32_t>::max())return false;
        try{owners_.reserve(owners_.size()+1u);}catch(...){return false;}
        sourceObserver280890_=owner;sourceObserverBaselineCount_=observers443140_;
        sourceObserverBaselineAllocator_=allocator443138_.get();
        sourceObserverAllocatorReached_=sourceObserverCountStored_=false;return true;
    }
    bool sourceObserverCurrent280890(const void* owner)const noexcept {
        if(!owner||sourceObserver280890_!=owner||!coherentState(true)||
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
    bool abortSourceObserver280890(const void* owner)noexcept {
        if(!sourceObserverCurrent280890(owner)||ownerHasLinks(owner))return false;
        if(sourceObserverCountStored_){owners_.pop_back();--observers443140_;}
        //Native-only unwind of this invocation. Existing pool/chunks/links and
        //other owners are untouched. No guest280920 completion is claimed.
        if(!sourceObserverBaselineAllocator_)allocator443138_.reset();
        sourceObserver280890_=nullptr;sourceObserverBaselineAllocator_=nullptr;
        sourceObserverBaselineCount_=0;sourceObserverAllocatorReached_=sourceObserverCountStored_=false;
        return coherent();
    }
    bool idleForApplicationRelease()const noexcept {
        return coherent()&&!observers443140_&&!allocator443138_&&owners_.empty();
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
        if(!owns(owner)||ownerHasLinks(owner))return false;
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
    ~OriginalGarageObserverShared50KG(){if(sourceObserver280890_||observers443140_||allocator443138_)std::terminate();}
    OriginalGarageObserverShared50KG(const OriginalGarageObserverShared50KG&)=delete;
    OriginalGarageObserverShared50KG& operator=(const OriginalGarageObserverShared50KG&)=delete;
};
}
