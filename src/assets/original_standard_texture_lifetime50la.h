#pragma once
#include <cstdint>
#include <exception>
#include <limits>

namespace original_main_entry50KG { struct Application; class RaceDisplayLifetime50LA; }
namespace rr::platform { class OriginalUiSharedDialogs50KG; }

namespace rr::assets {
class OriginalStandardMaterialContext50LA;
class OriginalStandardTextureResource50LA;
class OriginalStandardTextureLifetime50LA;
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
    OriginalStandardTextureResource50LA* const resource_;
    OriginalStandardTextureLifetimeLink50LA* nextA0_=nullptr;
    OriginalStandardTextureLifetimeLink50LA* previousA4_=nullptr;
    OriginalStandardTextureLifetime50LA* owner_=nullptr;
    explicit OriginalStandardTextureLifetimeLink50LA(
        OriginalStandardTextureResource50LA& resource) noexcept :resource_(&resource){}
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
    OriginalStandardTextureLifetime50LA() noexcept=default;

    bool register2557E0(OriginalStandardTextureLifetimeLink50LA& link) noexcept {
        if(link.owner_||link.nextA0_||link.previousA4_||
            count0C_==std::numeric_limits<std::uint32_t>::max())return false;
        //29DDD0: first object initializes head and tail; subsequent objects
        // append at the old tail, preserving original construction order.
        if(!head00_){head00_=&link;tail04_=&link;}
        else{tail04_->nextA0_=&link;link.previousA4_=tail04_;tail04_=&link;}
        link.owner_=this;
        ++count0C_;
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
        if(head00_||tail04_||count0C_||nativeUploads_||publishedTransfer13C_||coldTransferOwner_||treeCursor442B60_||nativeWarningDemandOwner_||nativeWarningDemandEpoch_)std::terminate();
    }
};
}
