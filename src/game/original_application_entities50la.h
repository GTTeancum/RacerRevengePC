#pragma once
namespace original_cable_selection50LA { class Binding; }
#include "original_wreck_components50la.h"
#include "portal_geometry50eo.h"
#include <array>
#include <cstring>
#include <exception>
#include <string_view>
#include <vector>

namespace rr::game::original_gc_gameplay_lighting_candidate { class Owner; }
namespace rr::game {
class OriginalRacerStripOwners50LA;
class OriginalTrackParticleCold50LA;
// One native application owns the source383408 name registry. IDs are actual
// insertion indices in this same registry, not retail boot constants or the
// unrelated collision-category bit namespace. Only reached real constructors
// insert names; startup does not seed a guessed list of predecessor types.
class OriginalEntityTypeRegistry50LA final {
public:
    enum class Status : std::uint8_t {complete,input,capacity,allocation,terminal};
    struct Result {
        Status status=Status::input;
        std::uint32_t sourcePC=0;
        std::uint8_t type=0xffu;
        bool okay()const noexcept{return status==Status::complete;}
    };
    OriginalEntityTypeRegistry50LA()=default;
    OriginalEntityTypeRegistry50LA(const OriginalEntityTypeRegistry50LA&)=delete;
    OriginalEntityTypeRegistry50LA& operator=(const OriginalEntityTypeRegistry50LA&)=delete;
    Result findOrAppend2A31E0(std::string_view name)noexcept {
        if(terminal_)return {Status::terminal,0x2a31e0u};
        // Real callers pass zero-terminated type names. Preserve the source
        //128-byte comparison/copy; do not fold case, separators or long names.
        if(name.find('\0')!=std::string_view::npos)return {Status::input,0x2a31e0u};
        std::array<char,128> requested{};
        const auto bytes=name.size()<requested.size()?name.size():requested.size();
        if(bytes)std::memcpy(requested.data(),name.data(),bytes);
        for(std::size_t i=0;i<names_.size();++i)
            if(std::strncmp(requested.data(),names_[i].data(),requested.size())==0)
                return {Status::complete,0x2a3228u,static_cast<std::uint8_t>(i)};
        // Actual shared lazy assertion guard gp8D04/443978, initialized by
        // the first miss, even when the ordinary less-than255 branch succeeds.
        if(!assertionInitialized_){assertion443978_=1;assertionInitialized_=true;}
        if(assertion443978_&&count00_==255u)return {Status::capacity,0x2a327cu};
        ++count00_; //2A32B0..B8 precedes source realloc; failure is terminal.
        try {names_.push_back(requested);}
        catch(...){terminal_=true;return {Status::allocation,0x2a32c4u};}
        return {Status::complete,0x2a3308u,static_cast<std::uint8_t>(count00_-1u)};
    }
    Result searchableGameObject27E1BC()noexcept {
        if(terminal_)return {Status::terminal,0x27e1bcu};
        // Native fresh initialization uses the source invalid-cache sentinel.
        // No retail gp address, numerical type ID or prior registry is imported.
        if(searchableTypeGp801C_!=0xffu)
            return {Status::complete,0x27e1e0u,searchableTypeGp801C_};
        auto result=findOrAppend2A31E0("SearchableGameObject");
        if(result.okay())searchableTypeGp801C_=result.type;
        return result;
    }
    std::uint32_t count00()const noexcept{return count00_;}
    const std::array<char,128>* name(std::uint8_t type)const noexcept {
        return !terminal_&&type<names_.size()?&names_[type]:nullptr;
    }
private:
    //3CB140..168's source registry initialization; no Entity construction.
    std::uint32_t count00_=0,assertion443978_=0;
    std::vector<std::array<char,128>> names_;
    std::uint8_t searchableTypeGp801C_=0xffu;
    bool assertionInitialized_=false,terminal_=false;
};

// Application's real base-Entity construction prefix13FDC0..13FE0C. The
// process owner constructs this once, before first UI or direct gameplay,
// and retains it across serial native race lifetimes. It is NOT a complete
// retail application forest or a substitute race/manager ancestor chain.
class OriginalApplicationEntities50LA final {
    friend class ::original_cable_selection50LA::Binding;
    friend class OriginalRacerStripOwners50LA;
    friend class OriginalCableStripAttributes50LA;
    struct CableLazy {std::uint8_t guard=0;std::uint32_t word=0;};
    // Actual224720 shared lazy fields4347B0/B8/C0/C8/D0, not per-racer state.
    std::array<CableLazy,5> cableSelectionGlobals_;
    std::uint8_t racerSelectionGuard87F0_=0;
    std::int32_t racerSelectionAssert434820_=0; //Startup BSS, then229B0C=-1.
    // Startup239C54 ->2EADE0 ->2E91F0 visits table3CC4F4 ->3C9490.
    // Numeric static-constructor values, owned once by this fresh Application.
    // No caller vector or BSS-after-constructor assumption is accepted.
    // Startup table3CC548 ->3C9F70, store3C9FFC. Retained genuine vector;
    // detached224720 consumption remains blocked on its actual field48 owner.
    const std::array<float,4> binderTriple434650_{{0.f,1.f,0.f,1.f}}; //3C9FEC
    const std::array<float,4> cableForward434660_{{0.f,0.f,1.f,1.f}};
    const std::array<float,4> cableOrigin433CD0_{{0.f,0.f,0.f,1.f}};
    const std::array<float,4> cableTriple433CF0_{{0.f,1.f,0.f,1.f}};
    friend class original_gc_gameplay_lighting_candidate::Owner;
    // Process-lifetime source startup100008 zero stores, never per-race reset.
    std::uint16_t portalAdmission8BE0_=0,portalDispatch8BE4_=0;
    std::uint32_t portalDepth443120_=0,portalDebug443100_=0;
    //36ADA8's authenticated data-image word. No native configuration writer
    // currently changes it; keep the real camera-entry gate explicit.
    std::uint32_t portalEnabled36ADA8_=1;
    std::uint32_t processedModels278_=0; //13FC94 constructor store.
    std::uint32_t useMembership3833D8_=1,guardInside4354F0_=0;
    float modelLod383360_=1.f,projectedRadius383368_=1.f; //authentic image.
    bool selectedLazy8BCC_=false;
    std::int32_t selectedAssert443090_=0; //BSS then27EB2C=-1.

    //13FBF8 explicitly clears Application14. No diagnostic owner is created
    // by this bounded native Application; a future constructor must own it here.
    const void* diagnostic14_=nullptr;
    // Populated by actual retained world portal construction, never by camera
    // pose/fog or host viewport dimensions. Empty worlds do not produce it.
    PortalProjectionQuad50EO portalViewport434940_{};
    bool portalViewportConstructed_=false;

    friend class OriginalTrackParticleCold50LA;
    OriginalWreckComponent50LA outer26C_,root10_;
    std::array<OriginalWreckComponent50LA*,2> roots_{{&outer26C_,&root10_}};
    OriginalWreckComponentDomain50LA rootDomain_{};
    // Source100008..100030 BSS initialization. Every later represented
    //141300/1413E0 contributes directly; never reset on race/menu entry.
    std::uint32_t objectCount423F60_=0,attachmentLock423F58_=0;
    // Source data-image382BD8 starts at500. The reached160E78 stores the
    // signed capacity/2 here. This is shared application state, not a count
    // of constructors, a per-track copy, or a value reset during cleanup.
    std::int32_t particleDepth382BD8_=500;
    OriginalEntityTypeRegistry50LA types383408_;
    bool constructed_=false,released_=false;
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
public:
    OriginalApplicationEntities50LA()noexcept {
        const auto applicationKey=key(this);
        const auto first=initializeOriginalWreckComponent50LA(outer26C_,
            OriginalWreckComponentKind50LA::entity141300,applicationKey,key(&outer26C_),true,objectCount423F60_);
        const auto second=initializeOriginalWreckComponent50LA(root10_,
            OriginalWreckComponentKind50LA::entity141300,applicationKey,key(&root10_),true,objectCount423F60_);
        rootDomain_={applicationKey,roots_.data(),roots_.size()};
        const auto attached=appendOriginalWreckComponent50LA(rootDomain_,outer26C_,root10_,
            0xffffffffu,attachmentLock423F58_);
        if(first.status!=OriginalWreckComponentStatus50LA::complete||
           second.status!=OriginalWreckComponentStatus50LA::complete||
           attached.status!=OriginalWreckComponentStatus50LA::complete)std::terminate();
        constructed_=true;
    }
    OriginalApplicationEntities50LA(const OriginalApplicationEntities50LA&)=delete;
    OriginalApplicationEntities50LA& operator=(const OriginalApplicationEntities50LA&)=delete;
    ~OriginalApplicationEntities50LA(){
        // Native fail-stop preserves ownership if a future caller attaches an
        // unrecovered child or retains a source reference without its release.
        if(!releaseApplication1411AC())std::terminate();
    }
    OriginalWreckComponent50LA* application10()noexcept{return released_?nullptr:&root10_;}
    OriginalWreckComponent50LA* application26C()noexcept{return released_?nullptr:&outer26C_;}
    OriginalEntityTypeRegistry50LA& types383408()noexcept{return types383408_;}
    std::uint32_t& objectCount423F60()noexcept{return objectCount423F60_;}
    const std::uint32_t& attachmentLock423F58()const noexcept{return attachmentLock423F58_;}
    const std::int32_t& particleDepth382BD8()const noexcept{return particleDepth382BD8_;}
    const OriginalWreckComponentDomain50LA& initialRootDomain()const noexcept{return rootDomain_;}
    // Bounded142590/104970/1413E0: exactly these genuine base roots, with no
    // later children or extra references. No blanket subtract-two, fabricated
    // child destructor, detach141A70 call or whole-tree mask reset is used.
    bool releaseApplication1411AC()noexcept {
        if(released_)return true;
        if(!constructed_||attachmentLock423F58_||outer26C_.reference00!=1u||root10_.reference00!=1u||
           outer26C_.kind!=OriginalWreckComponentKind50LA::entity141300||
           root10_.kind!=OriginalWreckComponentKind50LA::entity141300||
           outer26C_.parent1C||outer26C_.previous10||outer26C_.next14||
           outer26C_.firstChild18!=&root10_||root10_.parent1C!=&outer26C_||
           root10_.previous10||root10_.next14||root10_.firstChild18)return false;
        //142590 visits child10 first, repairs the parent's head, then releases
        // its actual64-bit reference. Only the zero result calls1413E0.
        outer26C_.firstChild18=root10_.next14;
        --root10_.reference00;
        if(!root10_.reference00)--objectCount423F60_;
        // Parent26C has no sibling or parent to repair.142654 releases it.
        --outer26C_.reference00;
        if(!outer26C_.reference00)--objectCount423F60_;
        released_=true;return true;
    }
};
}
