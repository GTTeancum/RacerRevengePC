#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::game {

// Native component subobjects, embedded by their ONE real owners. Model and
// collision objects originally embed this subobject at +8. These links are
// actual ownership/scheduling links, never a copied list of successful calls.
enum class OriginalWreckComponentKind50LA : std::uint8_t {
    unbound,collection22BF00,piece2CB4B0,group2CBFC0,model2403A0,scene2893A0,
    body167920,gravity282670,detector295290,contact295A40,collision11B190,
    // A real outside parent may participate in141960 mask propagation. Its
    // notification/update methods remain unjoined and cannot be presumed base.
    external,effect19A5E0,effect19DBE0,effect15F100,effect15E800,
    entity141300,effect15DE00,effect19CC50,effect19BA50,
    racer27E130,overlay199840,animation1536F0,particleManager160B10,
    staticModels27B610,podCables173CC0,strip23BCA0,constraint1276E0,podSphere1B6200,body164ED0,searchNode27F120,collisionObjects27BB40,
    camera236180, // Cold typed ownership only; no live camera dispatch.
    character11A750,uiDispatcher1B7140,uiGarageManager1D1860,toyList2D9B40,animationManager101330,wattoWidget1D4320,soundGroup2350D0,wattoEmitter2921C0,lightManager147ED0,light146720,fog13E6B0,fogOff13F850,fogOn13F980,uiOverlayText2A8F00,uiTextOverlay29C850,uiMovementControl2AAF40,uiGaragePage1F9FF0,uiRouter2A6FD0,uiControl1D5D10,uiControlGroup2A4B70,sceneEmitter15F100,sceneEmitter15DE00,sceneEmitter15B860,sceneEmitter15E800,loopSoundList2795E0,auralScape102BE0, // Exact Garage constructor identities; no dispatch admission.
    cameraController174A30, // Same Session-owned controller; cold ownership, no dispatch.
    uiOptionsManager1D0470, // Appended identity; unpublished constructor prefix only.
    triggerList2DC320,trigger2A0290, // RA native lifetime identities, no new dispatch.
    applicationRequest2C6C20, // Actual late native Results request owner, not early boot history.
    lightFlare14AA10, //Actual counted local flare; update/draw dispatch unjoined.
    uiCourseManager1D0070,uiCoursePage1D8D70,uiIcon2AC4B0,uiSelectableIcon2ACF00,uiSoultree2A85F0,effect15B860,terrain1B0EE0,engine2E7510,aeroBase100330,aero16CCD0, // Distinct detached Course constructors; no dispatch claim.
    smoker288C30,smokerEmitter288E90, //Appended source smoker identities; no update/emission admission.
    physicsObject1659C0, //Base PhysicsObject identity; no derived Racer/setup dispatch.
    speech2C1000, //Actual detached speech constructor; no Player or speech-load dispatch.
    music2C0980 //Actual detached stream constructor; no playback/child dispatch admission.
};
struct OriginalWreckComponent50LA final {
    //1048F0 base constructor owns the actual64-bit reference at source00.
    // Fresh native storage is not a constructed source reference.
    std::uint64_t reference00=0;
    std::uint64_t sessionGeneration=0,objectKey=0;
    // Native borrow of the actual effect control identity, never a source field.
    // Valid only while this canonical component remains in its live domain.
    const std::uint32_t* nativeEffectControlId=nullptr;
    OriginalWreckComponentKind50LA kind=OriginalWreckComponentKind50LA::unbound;
    OriginalWreckComponent50LA* previous10=nullptr;
    OriginalWreckComponent50LA* next14=nullptr;
    OriginalWreckComponent50LA* firstChild18=nullptr;
    OriginalWreckComponent50LA* parent1C=nullptr;
    std::uint32_t mask20=0,descendants24=0;
    std::uint8_t category28=0,flags29=0;
    // Optional read authority for an already-live native backend. When bound,
    // raw flags29 records ONLY the constructor prefix; it is not a second
    // mutable scheduling state. No generic mutator/constructor may overwrite
    // the backend through this observation binding. Owner revokes before death.
    const std::uint8_t* nativeFlags29=nullptr;
    std::uint8_t flags29Value()const noexcept{return nativeFlags29?*nativeFlags29:flags29;}
    OriginalWreckComponent50LA() noexcept=default;
    OriginalWreckComponent50LA(const OriginalWreckComponent50LA&)=delete;
    OriginalWreckComponent50LA& operator=(const OriginalWreckComponent50LA&)=delete;
};
inline constexpr std::size_t maxOriginalWreckComponents50LA=64;
// Borrow of stable canonical component addresses, not an additional forest.
// Include real outside ancestors when141960 must propagate through them.
struct OriginalWreckComponentDomain50LA {
    std::uint64_t sessionGeneration=0;
    OriginalWreckComponent50LA* const* objects=nullptr;
    std::size_t count=0;
};
enum class OriginalWreckComponentStatus50LA : std::uint8_t {
    complete,owner,topology,capacity,attachmentLocked,notificationMethod
};
struct OriginalWreckComponentResult50LA {
    OriginalWreckComponentStatus50LA status=OriginalWreckComponentStatus50LA::owner;
    std::uint32_t sourcePC=0;
};
namespace original_wreck_component_detail50LA {
using C=OriginalWreckComponent50LA;using D=OriginalWreckComponentDomain50LA;
inline bool effectControlIdentityBound(const C& c) noexcept {
    using K=OriginalWreckComponentKind50LA;
    switch(c.kind){
    case K::effect19A5E0:case K::effect19DBE0:case K::effect15F100:
    case K::effect15E800:case K::effect15DE00:case K::effect19CC50:
    case K::effect19BA50:case K::effect15B860:return c.nativeEffectControlId&&*c.nativeEffectControlId;
    default:return true;
    }
}
inline bool domain(const D& d) noexcept {
    if(!d.sessionGeneration||!d.objects||!d.count||d.count>maxOriginalWreckComponents50LA)return false;
    for(std::size_t i=0;i<d.count;++i){
        const auto* p=d.objects[i];
        if(!p||!p->reference00||!p->objectKey||p->sessionGeneration!=d.sessionGeneration||
           p->kind==OriginalWreckComponentKind50LA::unbound)return false;
        for(std::size_t j=0;j<i;++j)
            if(d.objects[j]==p||d.objects[j]->objectKey==p->objectKey)return false;
    }
    return true;
}
inline bool contains(const D& d,const C* p) noexcept {
    for(std::size_t i=0;i<d.count;++i)if(d.objects[i]==p)return true;
    return false;
}
inline bool baseNotification(OriginalWreckComponentKind50LA k) noexcept {
    return k!=OriginalWreckComponentKind50LA::unbound&&
           k!=OriginalWreckComponentKind50LA::external;
}
// Both141460(+28) and1414C0(+2C) are recursive notifications, with no own
// activity write. All admitted concrete vtables use these exact two methods.
// Check the reached tree before changing the root. Children with29 bit3 set
// are skipped regardless of their kind, activity bit0, or mask20.
inline OriginalWreckComponentResult50LA notification(const D& d,const C& root,
    bool enabling,std::size_t& left) noexcept {
    using S=OriginalWreckComponentStatus50LA;
    auto* p=root.firstChild18;const C* previous=nullptr;
    while(p){
        if(!left--)return {S::capacity,enabling?0x1414d8u:0x141478u};
        if(!contains(d,p)||p->parent1C!=&root||p->previous10!=previous)
            return {S::topology,enabling?0x1414d8u:0x141478u};
        if(!(p->flags29Value()&8u)){
            if(!baseNotification(p->kind))return {S::notificationMethod,enabling?0x1414f4u:0x141494u};
            const auto r=notification(d,*p,enabling,left);
            if(r.status!=S::complete)return r;
        }
        previous=p;p=p->next14;
    }
    return {S::complete,enabling?0x141518u:0x1414b8u};
}
}

// Exact represented-field141300 initialization, reached by the concrete
// owner constructors. The caller supplies the ONE real global423F60 counter;
// increment is32-bit source arithmetic. Opaque upper29 bits are preserved.
// Does not load an asset, install a body, construct an effect, or register a
// scene/query. Call once on fresh, unlinked native storage; no allocator runs.
inline OriginalWreckComponentResult50LA initializeOriginalWreckComponent50LA(
    OriginalWreckComponent50LA& c,OriginalWreckComponentKind50LA kind,
    std::uint64_t session,std::uint64_t key,bool sourceA1,
    std::uint32_t& actualObjectCount423F60) noexcept {
    using S=OriginalWreckComponentStatus50LA;
    if(!session||!key||kind==OriginalWreckComponentKind50LA::unbound||
       c.nativeFlags29||c.kind!=OriginalWreckComponentKind50LA::unbound||c.reference00||c.objectKey||
       c.previous10||c.next14||c.firstChild18||c.parent1C)return {S::owner,0x141300u};
    c.reference00=1; //141314->1048F0, before the Entity-specific stores.
    c.sessionGeneration=session;c.objectKey=key;c.kind=kind;
    c.previous10=nullptr;c.next14=nullptr;c.firstChild18=nullptr;c.parent1C=nullptr;
    c.category28=0xffu;
    c.flags29=static_cast<std::uint8_t>((c.flags29&0xf0u)|(sourceA1?3u:0u));
    c.mask20=0;c.descendants24=0;
    ++actualObjectCount423F60;
    return {S::complete,0x1413c0u};
}

// Actual141960 append plus ancestor mask propagation. This bounded cold path
// admits the actual one-fresh-child calls made by165C30/165E90/22BFD0, not the
// source's optional prelinked sibling-chain splice. The caller must pass the
// real global423F58 attachment lock. Nonzero lock performs no mutation and is
// reported distinctly (source returns null). No activity bits are changed.
inline OriginalWreckComponentResult50LA appendOriginalWreckComponent50LA(
    const OriginalWreckComponentDomain50LA& d,OriginalWreckComponent50LA& parent,
    OriginalWreckComponent50LA& child,std::uint32_t sourceMask,
    std::uint32_t actualAttachmentLock423F58) noexcept {
    using namespace original_wreck_component_detail50LA;
    using S=OriginalWreckComponentStatus50LA;
    if(actualAttachmentLock423F58)return {S::attachmentLocked,0x14197cu};
    if(!domain(d)||!contains(d,&parent)||!contains(d,&child)||&parent==&child)
        return {S::owner,0x141984u};
    if(child.parent1C||child.previous10||child.next14)return {S::topology,0x14198cu};
    std::size_t left=d.count;C* tail=nullptr;
    for(auto* p=parent.firstChild18;p;p=p->next14){
        if(!left--)return {S::capacity,0x1419a8u};
        if(!contains(d,p)||p==&child||p->parent1C!=&parent||p->previous10!=tail)
            return {S::topology,0x1419a8u};
        tail=p;
    }
    left=d.count;
    for(auto* p=&parent;p;p=p->parent1C){
        if(!left--)return {S::capacity,0x141a00u};
        if(!contains(d,p)||p==&child)return {S::topology,0x141a00u};
    }
    // All admission checks precede the actual source stores.
    if(tail)tail->next14=&child;else parent.firstChild18=&child;
    child.previous10=tail;child.parent1C=&parent;child.mask20=sourceMask;
    auto* below=&child;
    for(auto* p=&parent;p;p=p->parent1C){
        p->descendants24|=below->descendants24|below->mask20;
        below=p;
    }
    return {S::complete,0x141a58u};
}

// Exact BASE141560/141520 calls and their real concrete notification trees.
// This is not a replacement for an overriding slot18, such as22CE10,
//2CBEB0 or2CC9C0: those owners must perform their earlier effect/removal work.
// Notifications do not activate/deactivate children. Bit3 alone skips them.
inline OriginalWreckComponentResult50LA setOriginalWreckBaseActivity50LA(
    const OriginalWreckComponentDomain50LA& d,OriginalWreckComponent50LA& c,
    bool active) noexcept {
    using namespace original_wreck_component_detail50LA;
    using S=OriginalWreckComponentStatus50LA;
    if(c.nativeFlags29||!domain(d)||!contains(d,&c))return {S::owner,active?0x141560u:0x141520u};
    std::size_t left=d.count;
    const auto r=notification(d,c,active,left);
    if(r.status!=S::complete)return r;
    c.flags29=static_cast<std::uint8_t>((c.flags29&~3u)|(active?3u:0u));
    return {S::complete,active?0x141598u:0x141558u};
}
// PodPhysicsObject slot20:2CBFB0->166780->141560. Its only extra store is
// source contactTime12C=+0. Does not enable/reset the body or attach visibility.
inline OriginalWreckComponentResult50LA activateOriginalWreckPiece50LA(
    const OriginalWreckComponentDomain50LA& d,OriginalWreckComponent50LA& piece,
    float& actualContactTime12C) noexcept {
    using namespace original_wreck_component_detail50LA;
    using S=OriginalWreckComponentStatus50LA;
    if(piece.nativeFlags29||!domain(d)||!contains(d,&piece)||piece.kind!=OriginalWreckComponentKind50LA::piece2CB4B0)
        return {S::owner,0x2cbfb0u};
    std::size_t left=d.count;
    const auto r=notification(d,piece,true,left);
    if(r.status!=S::complete)return r;
    actualContactTime12C=0.f;
    piece.flags29=static_cast<std::uint8_t>(piece.flags29|3u);
    return {S::complete,0x141598u};
}
// Exact141ED0 child eligibility only. The scheduler must still perform actual
// renderer442F70 slot20(0), then the concrete child's slot38, then read next14.
// This predicate is not a dispatch or a success receipt. Do not substitute
// descendants24 for a child's mask20, or body.enabled204 for activity29.
inline bool originalWreckChildEligible50LA(const OriginalWreckComponent50LA& child) noexcept {
    const auto flags=child.flags29Value();
    return (flags&1u)&&!(flags&4u)&&!(flags&8u)&&(child.mask20&2u);
}
}
