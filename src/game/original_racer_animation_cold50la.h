#pragma once
#include "original_wreck_components50la.h"
#include <cstdint>
#include <exception>
#include <optional>
#include <string>

namespace native_racer_model50LA { class Owner; }
namespace rr::game {
enum class OriginalRacerAnimationColdStatus50LA : std::uint8_t {
    complete,binding,alreadyConstructed,component,attachmentLocked,topology,allocationState
};
struct OriginalRacerAnimationColdResult50LA {
    OriginalRacerAnimationColdStatus50LA status=OriginalRacerAnimationColdStatus50LA::binding;
    std::uint32_t sourcePC=0;
    bool okay()const noexcept{return status==OriginalRacerAnimationColdStatus50LA::complete;}
};
struct OriginalRacerAnimationColdBinding50LA {
    std::uint64_t sessionGeneration=0,animationKey=0;
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
};

// Native semantic fields, not a packed guest image. Disengaged optionals mean
// that no producer has run; they do NOT mean a source null/zero was stored.
// In particular1536F0 never writes04,08,188,18C or1E8. The five strings hold
// only the source-produced C-string value, not invented bytes after its NUL.
struct OriginalRacerAnimationColdFields50LA {
    const OriginalWreckComponent50LA* self00=nullptr;
    std::optional<const void*> model04,configuration08;
    std::uint32_t field0C=0,field10=0;
    std::int32_t motionCount14=0;
    const void* motion18=nullptr;
    const void* motion1C=nullptr;
    std::uint32_t field20=0,field24=0,field28=0,field2C=0,field30=0;
    float time34=0.f,time38=0.f;
    std::string definition48,model98,motionE8,rig138,directory190;
    std::optional<std::uint32_t> rigOverride188,field18C;
    std::uint32_t field1E0=0;
    const void* motionArray1E4=nullptr;
    std::optional<std::int32_t> nodeCount1E8;
    const void* allocation1EC=nullptr;
    const void* allocation1F0=nullptr;
    const void* allocation1F4=nullptr;
    std::uint32_t field1F8=0;
    const void* allocation1FC=nullptr;
    const void* allocation200=nullptr;
    std::uint32_t field204=0,count208=0;
    const void* allocation20C=nullptr;
    const void* allocation210=nullptr;
    std::uint32_t field214=0;
    const void* allocation218=nullptr;
    // Native identities for the two source dispatch tables, never guest
    // pointers. Entity220.kind represents its source+8 dispatch identity.
    OriginalWreckComponentKind50LA dispatch21C=OriginalWreckComponentKind50LA::unbound;
    std::int32_t entity220Adjustment30=0;
};

// The actual fresh1536F0(1,1) owner reached by210C40, before1538B0 at210C5C.
// The native right-model lifetime owns this object at a stable address; Session
// borrows that same owner. Entity220 is genuinely initialized against the
// application's counter. This constructor alone does not append it or load a
// model; the concrete model owner performs those later operations explicitly.
class OriginalRacerAnimationCold50LA final {
    friend class ::native_racer_model50LA::Owner;
public:
    OriginalRacerAnimationCold50LA()=default;
    OriginalRacerAnimationCold50LA(const OriginalRacerAnimationCold50LA&)=delete;
    OriginalRacerAnimationCold50LA& operator=(const OriginalRacerAnimationCold50LA&)=delete;
    OriginalRacerAnimationCold50LA(OriginalRacerAnimationCold50LA&&)=delete;
    OriginalRacerAnimationCold50LA& operator=(OriginalRacerAnimationCold50LA&&)=delete;
    ~OriginalRacerAnimationCold50LA(){
        // The Session must explicitly close the bounded owner while its
        // application borrow is alive. Never silently discard a live Entity.
        if(constructed_&&!released_)std::terminate();
    }
    OriginalRacerAnimationColdResult50LA constructFresh1536F0(
        const OriginalRacerAnimationColdBinding50LA& b)noexcept {
        using S=OriginalRacerAnimationColdStatus50LA;
        if(constructed_||released_)return {S::alreadyConstructed,0x1536f0u};
        if(!b.sessionGeneration||!b.animationKey||!b.actualObjectCount423F60||
           !b.actualAttachmentLock423F58)return {S::binding,0x1536f0u};
        binding_=b;
        //153710..720: source00 points to this owner's real Entity220. No
        // guest+220 arithmetic is applied to the native, non-packed layout.
        fields_.self00=&entity220_;
        const auto initialized=initializeOriginalWreckComponent50LA(entity220_,
            OriginalWreckComponentKind50LA::animation1536F0,b.sessionGeneration,
            b.animationKey,true,*b.actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)
            return {S::component,initialized.sourcePC};
        //153738/750/764: native dispatch identities and the zero adjustment
        // produced because source00 is exactly this owner's embedded Entity.
        fields_.dispatch21C=OriginalWreckComponentKind50LA::animation1536F0;
        fields_.entity220Adjustment30=0;
        fields_.field10=1;fields_.field0C=0;fields_.motionCount14=0;
        fields_.motion18=nullptr;fields_.time34=0.f;
        fields_.field20=1;fields_.field24=1;fields_.field28=0;fields_.field2C=1;
        fields_.motion1C=nullptr;fields_.time38=0.f;fields_.field30=0;
        //1537CC/7E0/7F4/808/81C call309230(strcpy) from3A5F80, whose first
        // byte is NUL (original-tail-all.txt3A5F80=00000000). No full-buffer
        // clearing, filenames or uninitialized trailing bytes are invented.
        fields_.definition48.clear();fields_.model98.clear();fields_.motionE8.clear();
        fields_.rig138.clear();fields_.directory190.clear();
        fields_.allocation1EC=nullptr;fields_.allocation1F4=nullptr;
        fields_.allocation1FC=nullptr;fields_.allocation200=nullptr;fields_.field1F8=0;
        fields_.field1E0=1;fields_.motionArray1E4=nullptr;fields_.allocation1F0=nullptr;
        fields_.field204=0;fields_.count208=0;fields_.allocation210=nullptr;
        fields_.allocation20C=nullptr;fields_.field214=0;fields_.allocation218=nullptr;
        constructed_=true;
        return {S::complete,0x1538a4u};
    }
    bool constructed1536F0()const noexcept{return constructed_&&!released_;}
    bool released()const noexcept{return released_;}
    const OriginalRacerAnimationColdFields50LA* fields()const noexcept {
        return constructed_?&fields_:nullptr;
    }
    const OriginalWreckComponent50LA* entity220()const noexcept {
        return constructed_?&entity220_:nullptr;
    }

    // Bounded142590->104970->156FF0->153F50(1)->1413E0(0), solely for the
    // fresh, never-loaded, unlinked reference1 owner above. Idempotent after
    // success; an attachment lock or inadmissible state makes no mutation.
    // This is not the destructor of a loaded animation/model or whole racer.
    OriginalRacerAnimationColdResult50LA releaseFresh153F50()noexcept {
        using S=OriginalRacerAnimationColdStatus50LA;
        if(released_)return {S::complete,0x154130u};
        if(!constructed_){released_=true;return {S::complete,0};}
        if(!binding_.actualObjectCount423F60||!binding_.actualAttachmentLock423F58)
            return {S::binding,0x1425a4u};
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x1425a8u};
        if(fields_.self00!=&entity220_||entity220_.reference00!=1u||
           entity220_.kind!=OriginalWreckComponentKind50LA::animation1536F0||
           entity220_.sessionGeneration!=binding_.sessionGeneration||
           entity220_.objectKey!=binding_.animationKey||entity220_.previous10||
           entity220_.next14||entity220_.firstChild18||entity220_.parent1C)
            return {S::topology,0x142590u};
        // Admit exactly the constructed null-allocation branch. No read of
        // unproduced04/08/1E8 is needed or permitted to justify cleanup.
        if(fields_.allocation1F0||fields_.allocation1F4||fields_.allocation1FC||
           fields_.allocation200||fields_.motionArray1E4||fields_.allocation20C||
           fields_.allocation210||fields_.allocation218||fields_.allocation1EC)
            return {S::allocationState,0x153fa4u};
        //142590 has no links to repair;104970 takes this sole reference to
        // zero and dispatches through the real1536F0 Entity's156FF0 thunk.
        --entity220_.reference00;
        fields_.dispatch21C=OriginalWreckComponentKind50LA::animation1536F0;
        fields_.entity220Adjustment30=0; //153F7C..F9C
        fields_.motion18=nullptr;       //153FA0
        // All optional frees and both allocation loops are skipped by the
        // actual constructed null pointers.154094 still writes count208=0.
        fields_.count208=0;
        //1540E0->1327D0 scans resource-manager rows for this owner identity.
        // Its only possible mutation is clearing a matching row+1C. A match
        // requires earlier1326A0 publication (153A18/153A50 in1538B0). This
        // class never calls1538B0, exposes no mutable Entity/fields, and has
        // no cache-publication API. Therefore this fresh identity has never
        // been a row payload: the scan is a no-match, with no shared-cache
        // state to alter. Do not replace that invariant with a fake manager.
        //1540F4->1413E0 decrements the SAME application's count exactly once;
        //104910 adds no reference write. Native owner storage remains with
        // the Session; no guest deallocation at15410C is simulated.
        --*binding_.actualObjectCount423F60;
        released_=true;
        return {S::complete,0x154130u};
    }
private:
    OriginalWreckComponent50LA entity220_;
    OriginalRacerAnimationColdFields50LA fields_;
    OriginalRacerAnimationColdBinding50LA binding_{};
    bool constructed_=false,released_=false;
};
} // namespace rr::game
