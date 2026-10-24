#pragma once
namespace original_main_entry50KG {struct Application;}
namespace rr::platform { class OriginalUiSharedDialogs50KG; class OriginalUiGarageDrawContribution50KG; class OriginalUiGarageCameraFrame50KG;class OriginalUiGarageVisibility50KG;class OriginalUiGaragePodScene50KG;}
namespace track_runtime_storage50IG {struct Storage;}
namespace rr::assets {class OriginalApplicationLocalization50KG;}
namespace rr::platform {class OriginalApplicationAural50KG;class OriginalUiGarageSceneCollision50KG;class OriginalAudioPendingWaves50KG;class OriginalUiLightManagerOwner50KG;class OriginalUiGarageScreenText50KG;class OriginalLocalizationRendererLease50KG;class OriginalUiGarageWidget1Owner50KG;}
namespace ra_pod_scene50LA { class Owner; }
namespace original_cable_selection50LA { class Binding; }
namespace native_racer_model50LA { class Owner; }
#include "original_wreck_components50la.h"
#include "original_garage_character50gn.h"
#include "original_garage_sphere_bounds50gn.h"
#include "original_garage_manager50gn.h"
#include "original_garage_observer_shared50kg.h"
#include "original_garage_emitter_shared50kg.h"
#include "original_application_entity_authority50la.h"
#include "original_native_collision_categories.h"
#include "portal_geometry50eo.h"
#include <array>
#include <memory>
#include <cstring>
#include <exception>
#include <string_view>
#include <string>
#include <optional>
#include <vector>
#include <thread>

namespace rr::game::original_gc_gameplay_lighting_candidate { class Owner; }
namespace rr::game {
class OriginalVideoPreFlipList50KG;
class OriginalTunnelTrack50LA;
class OriginalVisibilityQuadTree50LA;
class OriginalRacerStripOwners50LA;
class OriginalTrackParticleCold50LA;
class OriginalRacerMainBodyOwner50LA;
class OriginalRacerGravityOwner50LA;
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
    // Actual11B270..11B2E4 constructor registry suffix. The second name
    // is Vegetation, despite legacy native callers naming its tag "group".
    // Lazy process caches belong to this SAME registry, not to each query.
    Result collisionObject11B270()noexcept {
        if(terminal_)return {Status::terminal,0x11b270u};
        if(collisionTypeGp8010_==0xffu){
            auto result=findOrAppend2A31E0("CollisionObject");
            if(!result.okay())return result;
            collisionTypeGp8010_=result.type;
        }
        if(vegetationTypeGp8014_==0xffu){
            auto result=findOrAppend2A31E0("Vegetation");
            if(!result.okay())return result;
            vegetationTypeGp8014_=result.type;
        }
        //11B2C8 is unconditional, including warm-cache constructors. Its
        //returned byte, not a fixed native tag, becomes query.type02.
        return findOrAppend2A31E0("CollisionObject");
    }
    //146938..146964: the actual shared LightEmitter lazy cache, not a fixed tag.
    Result lightEmitter146938()noexcept {
        if(terminal_)return {Status::terminal,0x146938u};
        if(lightEmitterTypeGp8018_!=0xffu)return {Status::complete,0x14695cu,lightEmitterTypeGp8018_};
        auto result=findOrAppend2A31E0("LightEmitter");
        if(result.okay())lightEmitterTypeGp8018_=result.type;
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
    std::uint8_t searchableTypeGp801C_=0xffu,lightEmitterTypeGp8018_=0xffu;
    std::uint8_t collisionTypeGp8010_=0xffu,vegetationTypeGp8014_=0xffu;
    bool assertionInitialized_=false,terminal_=false;
};

// Application's real base-Entity construction prefix13FDC0..13FE0C. The
// process owner constructs this once, before first UI or direct gameplay,
// and retains it across serial native race lifetimes. It is NOT a complete
// retail application forest or a substitute race/manager ancestor chain.
class OriginalApplicationEntities50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class rr::platform::OriginalUiGarageDrawContribution50KG;
    friend class rr::platform::OriginalUiSharedDialogs50KG;
    friend class rr::platform::OriginalUiGarageCameraFrame50KG;
    friend class rr::platform::OriginalUiGarageSceneCollision50KG;
    friend class OriginalFrontendCameraCold50LA;
    friend class OriginalUiEntityScope50KG;
    friend class OriginalVideoPreFlipList50KG;
    friend class rr::platform::OriginalAudioPendingWaves50KG;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class rr::platform::OriginalApplicationAural50KG;
    friend class rr::assets::OriginalApplicationLocalization50KG;
    friend class rr::platform::OriginalUiGarageScreenText50KG;
    friend class rr::platform::OriginalLocalizationRendererLease50KG;
    friend class rr::platform::OriginalUiGarageWidget1Owner50KG;
    // Original initialized process word383688 is1; admitted source has no writer.
    // Retained across managers, not a caller-issued rendering-ready flag.
    std::uint32_t screenTextEnabled383688_=1;
    // Actual initialized process383680=1; sole direct access2E225C is a load.
    // Shared across Pages; no invented per-Aperture admission flag.
    std::uint32_t apertureRenderEnabled383680_=1;
    std::uint32_t screenQuadEnabled383678_=1; //Exact ELF initialized word; bounded source has no writer.
    // Actual initialized process382BC8=1. The admitted executable has one
    // direct load162D64 and no direct writer.254AF0 writes442BC8 instead.
    // This is shared source state, not per-Scene empty/render readiness.
    std::uint32_t particleRenderEnabled382BC8_=1;
    // Actual process BSS words433A08/10/18/20, with their four source-local
    // lazy guards gp84F0/84F4/84F8/84FC. Reached only on a positive162D30.
    std::array<std::int32_t,4> particleDrawProfile433A08_{};
    std::array<std::uint32_t,4> particleDrawProfileGuards84F0_{};
    // Source BSS singleton and lazy assertion history, not a per-menu cache.
    OriginalWreckComponent50LA* lightManager426F60_=nullptr;
    const rr::platform::OriginalUiLightManagerOwner50KG* lightManagerNativeOwner_=nullptr;
    //100008 BSS and147A00 first-use state. Same process/Application
    //history as the Light singleton; never reset by Scene or manager teardown.
    std::uint8_t lightUpdateGuard83A0_=0;
    std::uint32_t lightUpdateGeometry426F78_=0;
    std::uint8_t lightManagerGuard83A8_=0;
    std::uint32_t lightManagerAssertion426F88_=0;
    const std::array<std::uint32_t,4> lightVector426F10_{{0,0,0,0x3f800000u}}; //3C8A0C
    // Native affinity of this actual shared owner, not a source field.
    // Other owners retain their existing serialized-operation contract.
    const std::thread::id nativeOwnerThread_=std::this_thread::get_id();
    friend class OriginalPauseStorage50LA;
    friend class OriginalTunnelTrack50LA;
    friend class OriginalRaceConstructor50LA;
    friend class rr::platform::RaceAudioGroups50IC;
    // Actual process434A10 slot authority. BSS startup initializes all25 free.
    std::array<OriginalWreckComponent50LA*,25> soundGroups434A10_{};
    //232AA0 stores integer1 in the two master slots, not Entity pointers.
    // Native typed occupancy keeps those values distinct from real Group borrows.
    std::array<bool,25> soundGroupReserved434A10_{};
    bool soundMasterSlotsInitialized_=false;
    OriginalWreckComponent50LA* aural00_=nullptr; //13FE2C, retained source identity.
    const rr::platform::OriginalApplicationAural50KG* auralNativeOwner_=nullptr;
    bool aural00Live_=false,auralPrefixIssued_=false;
    // Actual audio-interface pending namespace; privately issued by the same
    // live Application. Not a per-group table or native sample-readiness flag.
    std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG> pendingWaves98A4_;
    // Native single-issuance envelope, NOT a source field or a claim that
    // original audio-interface reconstruction is impossible.
    bool pendingWavesCreated_=false;
    // Native borrowed cleanup guard; not source32C or an Entity contribution.
    const rr::assets::OriginalApplicationLocalization50KG* localization32CNativeOwner_=nullptr;
    // Native cleanup responsibility includes constructed failed-append prefixes.
    const OriginalVideoPreFlipList50KG* videoPreFlipNativeBorrow_=nullptr;
    // Process BSS,298A30 first-use assertion initializer; root clear does
    // not reset these shared lanes.
    std::uint8_t pendingCountGuard8CB0_=0;
    std::uint32_t pendingCountAssertion4435F0_=0;
    // Process BSS, lazily initialized by235270 BEFORE Sound pointer assertion.
    std::uint8_t soundInsertGuard881C_=0;
    std::uint32_t soundInsertAssertion434A88_=0;
    std::uint32_t worldRegistration434320_=0; //BSS startup;1B28D0 clears on actual world construction.
    // Absolute process vectors, not offsets in Application or Scene. Main
    //239C54 dispatches static constructor3CA8F0 via table entry3CC588;
    //3CA94C/3CA96C publish these exact float4 defaults once at startup.
    const std::array<float,4> worldOrigin442F80_{{0.f,0.f,0.f,1.f}};
    const std::array<float,4> worldForward442FB0_{{0.f,0.f,1.f,1.f}};
    OriginalApplicationEntityAuthority50LA entityAuthority_;
    friend class ::ra_pod_scene50LA::Owner;
    friend struct ::track_runtime_storage50IG::Storage;
    // Process BSS3E0730. Storage borrows this byte; allocating another world
    // does not reset it. Only reached original query methods write it.
    // Native process-lifetime namespace, initialized from actual file-backed
    //zero data at36B0F0, not reset on menu/Scene entry.
    std::string collisionDirectory36B0F0_;
    // Borrow the exact immutable HOST namespace used by native track/query
    //consumers. This is not a recovered guest category registry or its values.
    const std::array<OriginalNativeCollisionCategories::NamedBinding,8>&
        nativeCollisionCategoryBindings_=OriginalNativeCollisionCategories::bindings;
    //11B570 clears byte0;11B920 copies the actual lowercased stem here.
    //Existing byte consumers retain their exact stable byte0 alias.
    std::array<std::uint8_t,256> collisionName3E0730_{};
    std::uint8_t& collisionGlobalValid3E0730_=collisionName3E0730_.front();
    friend class OriginalVisibilityQuadTree50LA;
    friend class rr::platform::OriginalUiGarageVisibility50KG;
    friend class rr::platform::OriginalUiGaragePodScene50KG;
    //Startup BSS; one real process slot, never a per-scene forced null.
    std::int32_t treeSearchFilter443AA8_=0; //100018 zero-fill, lazy2B00FC writes-1.
    bool treeSearchFilterInitialized3D6698_=false; //Actual GP-72D8 guard.
    OriginalVisibilityQuadTree50LA* visibility443A98_=nullptr;
    OriginalVisibilityQuadTree50LA* activeTree442E18_=nullptr;
    OriginalWreckComponent50LA* environment443108_=nullptr;
    //Actual process global443148. Typed Visibility owns the280890 allocation;
    //this retained Application slot is its sole source publication, not a copy
    //of a registry row or the shared443138/443140 allocator/count owner.
    const void* defaultZoneObserver443148_=nullptr;
    std::optional<float> treeQuantum442E20_;
    friend class ::native_racer_model50LA::Owner;
    friend class ::original_cable_selection50LA::Binding;
    friend class OriginalRacerStripOwners50LA;
    friend class OriginalRacerMainBodyOwner50LA;
    friend class OriginalRacerGravityOwner50LA;
    // Original static3CAA20 ->3CAA7C; shared SelectiveGravityModel vector.
    const std::array<std::uint32_t,4> gravityOrigin4431C0_{{0,0,0,0x3f800000u}};
    // Source startup3C9190/3C9290 and file382B60, shared once per Application.
    const std::array<std::uint32_t,4> bodyOrigin433AD0_{{0,0,0,0x3f800000u}};
    const std::array<std::uint32_t,4> bodyReset433B80_{{0,0,0,0x3f800000u}};
    const std::array<std::uint32_t,4> bodyQuaternion382B60_{{0,0,0,0}};
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
    std::uint32_t portalDrawDebug443110_=0,portalDrawDepth443118_=0; // Same startup BSS.
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
    //BSS100008..100030 initially clears both guards and diagnostic slots.
    //161334/33C and161354/35C set slot=-1 then guard=1 on first unpaused
    //ParticleManager update. Keep them across Scene/manager retirement.
    std::uint8_t particleUpdateGuard84D4_=0,particleUpdateGuard84D8_=0;
    std::int32_t particleDiagnostic4339D0_=0,particleDiagnostic4339D8_=0;
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
    OriginalGarageCharacterShared50GN garageCharacterShared_; // Original process BSS, never reset on page entry.
    OriginalGarageSphereShared50GN garageSphereShared_;
    OriginalGarageObserverShared50KG garageObserverShared_; // Process lifetime443138/443140; no page reset.
    OriginalGarageEmitterShared50KG garageEmitterShared_; // Exact retained290F10 globals.
    OriginalGarageAnimationManagerFields50GN* garageAnimationManager3DF400_=nullptr;
    bool garageAnimationManagerLive_=false; // Native lifetime only; source pointer is not cleared by101FE0.
    // Actual13FC80 -> virtual44/1401B0 constructor stores. These are raw
    // source Camera/Entity borrows, not native race aggregate defaults.
    std::array<OriginalWreckComponent50LA*,4> cameraSlots254_{};
    std::uint32_t cameraCount264_=1; //13FC8C overwrites the initial null count.
    bool cameraSlots254Known_=false;
    bool snapshotCameras140200(std::array<OriginalWreckComponent50LA*,4>& out)const noexcept {
        if(!constructed_||released_||!cameraSlots254Known_||
           nativeOwnerThread_!=std::this_thread::get_id())return false;
        out=cameraSlots254_;return true;
    }
    bool rewriteCameras1401B0(const std::array<OriginalWreckComponent50LA*,4>& values)noexcept {
        if(!constructed_||released_||!cameraSlots254Known_||
           nativeOwnerThread_!=std::this_thread::get_id())return false;
        cameraSlots254_=values;cameraCount264_=0;
        for(const auto* value:cameraSlots254_)if(value)++cameraCount264_;
        return true;
    }
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
        if(!entityAuthority_.enroll(outer26C_)||!entityAuthority_.enroll(root10_))std::terminate();
        // One process-owned constructor initialization. No Page may replay
        // this to erase prior Camera identities or restore source count1.
        cameraSlots254_.fill(nullptr);cameraCount264_=1;cameraSlots254Known_=true;
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
        // Namespace retirement is explicit; do not let shared_ptr destruction
        // silently drop borrowed pending Sound representatives after roots die.
        if(auralNativeOwner_||aural00Live_||videoPreFlipNativeBorrow_||localization32CNativeOwner_||pendingWaves98A4_||visibility443A98_||environment443108_||defaultZoneObserver443148_||activeTree442E18_||entityAuthority_.count_!=2||
           !garageObserverShared_.idleForApplicationRelease())return false;
        if(!constructed_||attachmentLock423F58_||outer26C_.reference00!=1u||root10_.reference00!=1u||
           outer26C_.kind!=OriginalWreckComponentKind50LA::entity141300||
           root10_.kind!=OriginalWreckComponentKind50LA::entity141300||
           outer26C_.parent1C||outer26C_.previous10||outer26C_.next14||
           outer26C_.firstChild18!=&root10_||root10_.parent1C!=&outer26C_||
           root10_.previous10||root10_.next14||root10_.firstChild18)return false;
        //142590 visits child10 first, repairs the parent's head, then releases
        // its actual64-bit reference. Only the zero result calls1413E0.
        outer26C_.firstChild18=root10_.next14;
        if(!entityAuthority_.retire(root10_))std::terminate();
        --root10_.reference00;
        if(!root10_.reference00)--objectCount423F60_;
        // Parent26C has no sibling or parent to repair.142654 releases it.
        if(!entityAuthority_.retire(outer26C_))std::terminate();
        --outer26C_.reference00;
        if(!outer26C_.reference00)--objectCount423F60_;
        released_=true;return true;
    }
};
}
