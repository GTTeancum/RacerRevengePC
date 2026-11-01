#pragma once
#include <cstdint>
#include <exception>
#include <limits>
#include <array>
#include <cstddef>
#include <memory>
#include <thread>
#include "original_config_process_globals50la.h"
namespace ra_cold_aggregate50LA {class Owner;}

namespace original_main_entry50KG { struct Application; class RaceDisplayLifetime50LA; }
namespace rr::platform { class OriginalUiSharedDialogs50KG; }

namespace rr::assets {
class OriginalStandardMaterialContext50LA;
class OriginalStandardTextureResource50LA;
class OriginalStandardTextureLifetime50LA;
class OriginalSourceTextureLifetimeFacet50LA;
class OriginalStandardTextureTransferCold50LA;
struct OriginalStandardTextureTransferNode50LA;
class OriginalNativeTextureUploadScope50LA;
class OriginalNativeTextureUploadBinding50LA;

// The resource owns these native links. Source29C6E0 initializes+A0/+A4,
// and29DDD0/29DE20 append/remove that SAME object. No integer guest pointer,
// copied texture row, external callback, or secondary name registry is used.
class OriginalStandardTextureLifetimeLink50LA final {
    friend class OriginalStandardTextureResource50LA;
    friend class OriginalStandardTextureLifetime50LA;
    friend class OriginalSourceTextureLifetimeFacet50LA;
    OriginalStandardTextureResource50LA* const resource_;
    OriginalSourceTextureLifetimeFacet50LA* const sourceResource_;
    OriginalStandardTextureLifetimeLink50LA* nextA0_=nullptr;
    OriginalStandardTextureLifetimeLink50LA* previousA4_=nullptr;
    OriginalStandardTextureLifetime50LA* owner_=nullptr;
    explicit OriginalStandardTextureLifetimeLink50LA(
        OriginalStandardTextureResource50LA& resource) noexcept :resource_(&resource),sourceResource_(nullptr){}
    explicit OriginalStandardTextureLifetimeLink50LA(OriginalSourceTextureLifetimeFacet50LA& source)noexcept
        :resource_(nullptr),sourceResource_(&source){}
public:
    OriginalStandardTextureLifetimeLink50LA()=delete;
    OriginalStandardTextureLifetimeLink50LA(const OriginalStandardTextureLifetimeLink50LA&)=delete;
    OriginalStandardTextureLifetimeLink50LA& operator=(const OriginalStandardTextureLifetimeLink50LA&)=delete;
    ~OriginalStandardTextureLifetimeLink50LA(){if(owner_)std::terminate();}
};

// CPU registration facet of original443870, allocated by Application140030
// and constructed253D80. This is DISTINCT from rendering Video443A60.
// Manager+30 is the14CF30 list owner:2557E0/2557F0 delegate to14D040/14D0E0.
// The actual native Application constructs this once; its sealed material
// context pins it with the real Video and immutable process material runtime.
// Resources own their links and retain that context until unregistration.
// No GPU allocator, residency, upload, or complete254AD0/254B20 is represented.
// Serial/nonreentrant, as is the source list and its shared cursor.
class OriginalStandardTextureLifetime50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class OriginalSourceTextureLifetimeFacet50LA;
    friend class OriginalStandardTextureResource50LA; // Exact typed PST cleanup preflight, no public list access.
    friend struct original_main_entry50KG::Application;
    friend class OriginalStandardMaterialContext50LA;
    friend class OriginalNativeTextureUploadScope50LA;
    friend class OriginalNativeTextureUploadBinding50LA;
    friend class ::original_main_entry50KG::RaceDisplayLifetime50LA;
    friend class OriginalStandardTextureTransferCold50LA;
    friend class rr::platform::OriginalUiSharedDialogs50KG;
    // Native in-flight first-pass fence. This is not source texture state.
    const rr::platform::OriginalUiSharedDialogs50KG* nativeWarningDemandOwner_=nullptr;
    std::uint64_t nativeWarningDemandEpoch_=0;
    //253DF0 initializes source13C null. Staging never publishes this field.
    OriginalStandardTextureTransferCold50LA* publishedTransfer13C_=nullptr;
    OriginalStandardTextureTransferCold50LA* coldTransferOwner_=nullptr;
    OriginalStandardTextureTransferNode50LA* treeCursor442B60_=nullptr;
    // Native direct-transfer ledger, distinct from source registration list30.
    OriginalNativeTextureUploadScope50LA* nativeUploads_=nullptr;
    std::uint64_t nativeUploadAttempts_=0,nativeUploadCompletions_=0;
    // Native capability lifetime only, not the source13C pointer or readiness.
    std::uint64_t nativeUploadGeneration_=1;
    OriginalStandardTextureLifetimeLink50LA* head00_=nullptr;
    OriginalStandardTextureLifetimeLink50LA* tail04_=nullptr;
    OriginalStandardTextureLifetimeLink50LA* cursor08_=nullptr;
    std::uint32_t count0C_=0;
    // Same443870 owner:29DEA8/AC/B0/B4 initialize34/3C/38/40.
    // These persist across texture calls; native call cancellation releases
    // borrows only. Manager destruction owns their backing, like29DED0.
    struct SourceReusableBuffer {
        std::unique_ptr<std::uint8_t[]> storage;
        std::uint32_t capacity=0;
        std::size_t knownPrefix=0; // Only proven written prefix; fresh allocation is unknown.
        std::uint64_t generation=0;
        const void* borrower=nullptr;
    };
    struct SourceBufferCall {
        bool entered=false,oldFreed=false,mallocAttempted=false,mallocReturned=false,returned=false,withdrawn=false;
        std::uint32_t requested=0,capacityBefore=0,capacityAfter=0;
        std::uint64_t generation=0;
        std::uint8_t* borrowed=nullptr;
    };
    std::array<SourceReusableBuffer,2> sourceBuffers_;
    const std::thread::id sourceBufferThread_=std::this_thread::get_id();
    bool sourceBuffersKnown_=true; //Any newly admitted unjoined alias must poison this before access.
    bool sourceBufferCurrentNative(std::size_t slot,const void* borrower,const SourceBufferCall& call)const noexcept {
        if(slot>=2||!sourceBuffersKnown_||sourceBufferThread_!=std::this_thread::get_id())return false;
        const auto& b=sourceBuffers_[slot];
        if(b.knownPrefix>b.capacity||(!b.storage&&b.knownPrefix))return false;
        if(!call.entered)return !call.borrowed&&!call.returned;
        if(call.withdrawn)return !call.borrowed&&b.borrower!=borrower;
        return b.borrower==borrower&&call.generation==b.generation&&call.capacityAfter==b.capacity&&call.borrowed==b.storage.get();
    }
    bool sourceBufferCallNative(std::size_t slot,std::uint32_t size,const void* borrower,SourceBufferCall& call,std::uint32_t& pc)noexcept {
        const auto delta=slot?0x80u:0u;pc=0x29dfe0+delta;
        if(slot>=2||!borrower||call.entered||!sourceBuffersKnown_||sourceBufferThread_!=std::this_thread::get_id())return false;
        auto& b=sourceBuffers_[slot];if(b.borrower||b.generation==std::numeric_limits<std::uint64_t>::max())return false;
        // Native admission before source side effects; shared allocator/reent
        // owner, not a claim of executing guest malloc/free internals.
        if(size>b.capacity&&!OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
        call.entered=true;call.requested=size;call.capacityBefore=b.capacity;b.borrower=borrower;
        if(b.capacity<size){pc=0x29dff4+delta;
            if(b.storage){pc=0x29e000+delta;b.storage.reset();b.knownPrefix=0;call.oldFreed=true;pc=0x29e008+delta;}
            ++b.generation;b.knownPrefix=0;pc=0x29e010+delta;call.mallocAttempted=true;
            try{b.storage.reset(new std::uint8_t[size]);}catch(...){b.storage.reset();}
            call.mallocReturned=true;pc=0x29e01c+delta;
            // Failure publishesNULL but keeps the OLD capacity, exactly source.
            if(b.storage){pc=0x29e028+delta;b.capacity=size;}
        }
        call.borrowed=b.storage.get();call.capacityAfter=b.capacity;call.generation=b.generation;
        call.returned=true;pc=0x29e044+delta;return true;
    }
    bool withdrawSourceBufferNative(std::size_t slot,const void* borrower,SourceBufferCall& call)noexcept {
        if(!call.entered||call.withdrawn)return true;
        if(!sourceBufferCurrentNative(slot,borrower,call))return false;
        sourceBuffers_[slot].borrower=nullptr;call.borrowed=nullptr;call.withdrawn=true;return true;
    }
    OriginalStandardTextureLifetime50LA() noexcept=default;

    bool register2557E0(OriginalStandardTextureLifetimeLink50LA& link,std::uint32_t* sourcePC=nullptr) noexcept {
        if(link.owner_||link.nextA0_||link.previousA4_||
            count0C_==std::numeric_limits<std::uint32_t>::max())return false;
        std::uint32_t pc=0x14d040;
        if(!OriginalConfigProcessGlobals50LA::checkTextureList14D040(bool(link.resource_)!=bool(link.sourceResource_),pc)){
            if(sourcePC)*sourcePC=pc;return false;
        }
        //29DDD0: first object initializes head and tail; subsequent objects
        // append at the old tail, preserving original construction order.
        if(!head00_){head00_=&link;tail04_=&link;}
        else{tail04_->nextA0_=&link;link.previousA4_=tail04_;tail04_=&link;}
        link.owner_=this;
        ++count0C_;
        if(sourcePC)*sourcePC=0x14d0c8;
        return true;
    }
    bool unregister2557F0(OriginalStandardTextureLifetimeLink50LA& link) noexcept {
        if(link.owner_!=this||!count0C_)return false;
        //29DE20: repair both neighbours before replacing head/tail and
        // decrementing count. The source does not modify its shared cursor.
        if(link.previousA4_)link.previousA4_->nextA0_=link.nextA0_;
        if(link.nextA0_)link.nextA0_->previousA4_=link.previousA4_;
        if(head00_==&link)head00_=link.nextA0_;
        if(tail04_==&link)tail04_=link.previousA4_;
        --count0C_;
        // Native detached-node guard only. Source29C7A0 destroys the resource
        // after this removal, so no source consumer can read these again.
        link.nextA0_=nullptr;link.previousA4_=nullptr;link.owner_=nullptr;
        return true;
    }
    //14D180 resets canonical resource+10 by traversing this SAME live list.
    // Defined beside the concrete resource. It is private for the future
    // actual Application texture-manager254AD0 frame entry; this candidate
    // installs no caller and never resets demand at lookup or selection.
    // A future caller must handle false without claiming a source reset.
    bool resetDemand14D180() noexcept;
    // Exact443870 vtable3D2220+88=255800: jr ra; addiu v0,zero,1.
    //The actual initialized owner's callback has no service/transfer effects.
    std::int32_t loadedResources255800()const noexcept{return 1;}
public:
    OriginalStandardTextureLifetime50LA(const OriginalStandardTextureLifetime50LA&)=delete;
    OriginalStandardTextureLifetime50LA& operator=(const OriginalStandardTextureLifetime50LA&)=delete;
    ~OriginalStandardTextureLifetime50LA(){
        // The retained context prevents manager teardown under live resources.
        // A violated native lifetime is not repaired by dropping real entries.
        if(sourceBuffers_[0].borrower||sourceBuffers_[1].borrower||head00_||tail04_||count0C_||nativeUploads_||publishedTransfer13C_||coldTransferOwner_||treeCursor442B60_||nativeWarningDemandOwner_||nativeWarningDemandEpoch_)std::terminate();
    }
};
//Fresh source24E1B0 backing, not a prepared CUB/resource receipt. Only the
//actual cold source Owner can construct it and publish its same-App manager.
class OriginalSourceTextureLifetimeFacet50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class OriginalStandardTextureLifetime50LA;
    const std::thread::id thread_=std::this_thread::get_id();
    std::unique_ptr<std::uint8_t[]> storage_;
    std::array<bool,0x210> known_{};
    OriginalStandardTextureLifetimeLink50LA link_;
    std::shared_ptr<OriginalStandardTextureLifetime50LA> manager_;
    const original_main_entry50KG::Application* issuer_=nullptr;
    bool registered_=false,nativeDetached_=false;
    OriginalSourceTextureLifetimeFacet50LA()noexcept:link_(*this){}
    bool topologyCurrentNative()const noexcept {
        if(thread_!=std::this_thread::get_id()||link_.resource_||link_.sourceResource_!=this)return false;
        if(!manager_)return !registered_&&!link_.owner_&&!link_.nextA0_&&!link_.previousA4_&&!issuer_;
        if(!issuer_)return false;
        std::uint32_t seen=0;bool found=false;const OriginalStandardTextureLifetimeLink50LA* previous=nullptr;
        for(auto* node=manager_->head00_;node;node=node->nextA0_){
            if(seen>=manager_->count0C_||node->owner_!=manager_.get()||node->previousA4_!=previous||
               bool(node->resource_)==bool(node->sourceResource_))return false;
            if(node==&link_)found=true;++seen;previous=node;
        }
        if(seen!=manager_->count0C_||previous!=manager_->tail04_)return false;
        return registered_?(found&&link_.owner_==manager_.get()):(!found&&!link_.owner_&&!link_.nextA0_&&!link_.previousA4_);
    }
    bool capture443870(std::shared_ptr<OriginalStandardTextureLifetime50LA> manager,
        const original_main_entry50KG::Application* issuer)noexcept {
        if(thread_!=std::this_thread::get_id()||manager_||issuer_||nativeDetached_||!manager||!issuer||!storage_)return false;
        manager_=std::move(manager);issuer_=issuer;return true;
    }
    bool register29C780(std::uint32_t& pc)noexcept {
        pc=0x29c780;if(!topologyCurrentNative()||!manager_||registered_||nativeDetached_||!storage_||
           manager_->nativeWarningDemandOwner_||manager_->nativeWarningDemandEpoch_)return false;
        //This source constructor has actually acquired ref1 and initialized
        //its intrusive link lanes. No generic resource constructor is borrowed.
        for(std::size_t k=0;k<8;++k)if(!known_[k]||storage_[k]!=(k?0:1))return false;
        for(std::size_t k=0;k<8;++k)if(!known_[0xa0+k]||storage_[0xa0+k])return false;
        if(!manager_->register2557E0(link_,&pc))return false;
        registered_=true;
        //Typed links carry actual host identity. EE pointer bit patterns are
        //not fabricated in the source-byte facet after insertion.
        for(std::size_t k=0;k<8;++k)known_[0xa0+k]=false;
        pc=0x29c788;return true;
    }
    bool detachNative()noexcept {
        if(!topologyCurrentNative())return false;
        if(registered_){if(!manager_->unregister2557F0(link_))return false;registered_=false;}
        manager_.reset();issuer_=nullptr;nativeDetached_=true;return true;
        //Exact same-list native inverse, not a guest29C7A0 destructor receipt.
    }
    bool resetDemand14D1A8()noexcept {
        if(!registered_||!topologyCurrentNative()||!storage_)return false;
        for(std::size_t k=0;k<4;++k){storage_[0x10+k]=0;known_[0x10+k]=true;}return true;
    }
public:
    OriginalSourceTextureLifetimeFacet50LA(const OriginalSourceTextureLifetimeFacet50LA&)=delete;
    OriginalSourceTextureLifetimeFacet50LA& operator=(const OriginalSourceTextureLifetimeFacet50LA&)=delete;
    ~OriginalSourceTextureLifetimeFacet50LA(){if(registered_||link_.owner_||manager_)std::terminate();}
};

}
