#pragma once
namespace original_cable_selection50LA { class Binding; }
#include "original_ui_main_entry50kg.h"
#include "../../src/assets/original_native_texture_upload50la.h"
#include "../../src/game/original_camera_start_native.h"
#include <exception>
#include <limits>
#include <memory>
#include <thread>

namespace rr::game { class OriginalPositionStatsPrimitiveOwner50LA; class OriginalPositionStatsResources50LA; }
namespace original_main_entry50KG {
class NativeCollectorFifo50LA;
class PositionStatsRendererLease50LA;
class PositionStatsTextureLease50LA;
// Actual native session storage, not a replacement renderer or completion token.
// Application alone constructs it. The existing root, camera and coordinator
// borrow these same members. No PositionStats resources are activated here.
class RaceDisplayLifetime50LA final : public std::enable_shared_from_this<RaceDisplayLifetime50LA> {
    friend struct Application;
    friend class NativeCollectorFifo50LA;
    friend class PositionStatsRendererLease50LA;
    friend class PositionStatsTextureLease50LA;
    friend class rr::game::OriginalPositionStatsPrimitiveOwner50LA;
    friend class rr::game::OriginalPositionStatsResources50LA;
    friend class ::original_cable_selection50LA::Binding;
    // Read-only cable inspection lifetime, not a FIFO/drain/GPU lease. It is
    // created from the renderer's private back-pointer and actual dispatch,
    // never from a caller-ready flag. No submission can outlive this scope.
    class CableInspection final {
        friend class RaceDisplayLifetime50LA;
        friend class NativeCollectorFifo50LA;
        std::shared_ptr<RaceDisplayLifetime50LA> owner_;
        explicit CableInspection(std::shared_ptr<RaceDisplayLifetime50LA> owner)
            :owner_(std::move(owner)){owner_->pin();}
    public:
        CableInspection(const CableInspection&)=delete;
        CableInspection& operator=(const CableInspection&)=delete;
        ~CableInspection()noexcept{owner_->unpin();}
    };
    static std::unique_ptr<CableInspection> retainCableInspection(
        rr::platform::VehicleWindowGl50DS& renderer,
        const rr::game::OriginalRendererDispatchFields& dispatch,const Video* actualVideo) {
        const auto* observed=static_cast<const RaceDisplayLifetime50LA*>(renderer.sessionDisplayOwner50LA_);
        // This private pointer is installed only by this real owner's ctor.
        if(!observed||&observed->renderer_!=&renderer||&observed->dispatch_!=&dispatch||
           observed->video_.get()!=actualVideo||!observed->canRetain())return {};
        auto* owner=const_cast<RaceDisplayLifetime50LA*>(observed);
        return std::unique_ptr<CableInspection>(new CableInspection(owner->shared_from_this()));
    }
    const Application* const application_; // Actual factory owner, no caller replacement.
    const std::shared_ptr<rr::assets::OriginalStandardTextureLifetime50LA> textureLifetime_;
    const std::shared_ptr<Video> video_; // Authoritative Application control block.
    const std::uint64_t generation_;
    const std::thread::id thread_=std::this_thread::get_id();
    rr::platform::VehicleWindowGl50DS renderer_;
    rr::game::OriginalRendererDispatchFields dispatch_{1,2,0,0,0};
    // SAME renderer's scalar2C: base26F880 initializes zero; race load
    // writes one at1A6AD8.243FC0 reads it to choose default clear mask2/3.
    // It is separate from current scene0 and dispatch queue44.
    std::uint32_t sourceClearMode2C_=0;
    void beginRaceClear1A6AD8() noexcept { sourceClearMode2C_=1; }
    void clearRaceClear1A687C() noexcept { sourceClearMode2C_=0; }

    bool attempted_=false,created_=false,retiring_=false;
    bool positionStatsTextureLeaseActive_=false;
    RaceDisplayLifetime50LA(std::shared_ptr<Video> video,std::uint64_t generation,const Application& application,
        std::shared_ptr<rr::assets::OriginalStandardTextureLifetime50LA> textureLifetime)
        :application_(&application),textureLifetime_(std::move(textureLifetime)),video_(std::move(video)),generation_(generation) {
        require(video_&&textureLifetime_&&generation_&&!video_->host.sessionDisplayRenderer50LA_,
            "display requires the actual unclaimed Application Video");
        renderer_.sessionDisplayOwner50LA_=this;
        video_->host.sessionDisplayRenderer50LA_=&renderer_;
    }
    bool ownerThread()const noexcept{return thread_==std::this_thread::get_id();}
    static rr::platform::VehicleWindowGlProc50DS proc(void* context,const char* name)noexcept {
        return static_cast<rr::platform::GameHost50ER*>(context)->procAddress(name);
    }
    static bool current(void* context)noexcept {
        return static_cast<rr::platform::GameHost50ER*>(context)->current();
    }
    static bool uploadBindingLive(const RaceDisplayLifetime50LA& owner)noexcept {
        return owner.ownerThread()&&!owner.retiring_&&owner.video_->originalMain&&
            owner.video_->host.created()&&owner.video_->host.current()&&
            owner.video_->host.sessionDisplayRenderer50LA_==&owner.renderer_&&
            owner.renderer_.sessionDisplayOwner50LA_==&owner;
    }
    // Only the future genuine primitive owner can obtain/use a lease. No caller
    // can supply a renderer pointer, queue copy, ready flag or completion callback.
    std::unique_ptr<PositionStatsRendererLease50LA> retainPositionStats16();
    std::unique_ptr<PositionStatsTextureLease50LA> retainPositionStatsFixedResources();
    bool beginPositionStatsTextureRetirement()noexcept {
        if(!ownerThread())return false;
        retiring_=true; //No new leases; existing primitive drain retries remain valid.
        //The independent16-primitive lease must already have retired. Observe
        //actual backing pins, not a caller's assertion of primitive completion.
        return positionStatsTextureLeaseActive_&&renderer_.sessionDependentPins50LA_==1&&
            video_->host.sessionDependentPins50LA_==1;
    }
    bool canRetain()const noexcept {
        return ownerThread()&&!retiring_&&video_->originalMain&&created_&&renderer_.initialized()&&
            video_->host.created()&&video_->host.current()&&
            video_->host.sessionDisplayRenderer50LA_==&renderer_&&
            renderer_.sessionDisplayOwner50LA_==this&&
            renderer_.sessionDependentPins50LA_<std::numeric_limits<std::uint32_t>::max()&&
            video_->host.sessionDependentPins50LA_<std::numeric_limits<std::uint32_t>::max();
    }
    void pin()noexcept {
        ++renderer_.sessionDependentPins50LA_;++video_->host.sessionDependentPins50LA_;
    }
    void unpin()noexcept {
        if(!ownerThread()||!renderer_.sessionDependentPins50LA_||
           !video_->host.sessionDependentPins50LA_)std::terminate();
        --renderer_.sessionDependentPins50LA_;--video_->host.sessionDependentPins50LA_;
    }
public:
    RaceDisplayLifetime50LA(const RaceDisplayLifetime50LA&)=delete;
    RaceDisplayLifetime50LA& operator=(const RaceDisplayLifetime50LA&)=delete;
    ~RaceDisplayLifetime50LA()noexcept {
        if(!ownerThread()||renderer_.sessionDependentPins50LA_)std::terminate();
        rr::platform::VehicleWindowDiagnostic50DS d;
        renderer_.close(d);
        if(d.cleanupIncomplete||renderer_.ownsResources())std::terminate();
        // Renderer dies before the retained Video; no dangling host back-pointer.
        video_->host.sessionDisplayRenderer50LA_=nullptr;
        renderer_.sessionDisplayOwner50LA_=nullptr;
    }
    rr::platform::VehicleWindowGl50DS& renderer()noexcept{return renderer_;}
    rr::game::OriginalRendererDispatchFields& dispatch()noexcept{return dispatch_;}
    std::uint64_t generation()const noexcept{return generation_;}
    // Cold uploads occur before renderer.create; the actual current Video and
    // display identity, rather than an invented GPU-ready flag, admit them.
    bool bindTextureUploadRequest(rr::platform::MaterialTextureGlRequest50EB& request) {
        if(!uploadBindingLive(*this)||request.context!=&video_->host||request.isCurrent!=&current)
            return false;
        using Binding=rr::assets::OriginalNativeTextureUploadBinding50LA;
        request.originalUploadBinding=std::shared_ptr<const Binding>(new Binding(
            shared_from_this(),textureLifetime_,&video_->host,&current,&uploadBindingLive));
        return true;
    }
    void bindCreationRequest(rr::platform::VehicleWindowRequest50DS& request)const {
        require(ownerThread()&&!attempted_&&!retiring_,"display creation already attempted/retired");
        request.context=&video_->host;request.procAddress=&proc;request.isCurrent=&current;
        using Binding=rr::assets::OriginalNativeTextureUploadBinding50LA;
        request.originalUploadBinding=std::shared_ptr<const Binding>(new Binding(
            shared_from_this(),textureLifetime_,&video_->host,&current,&uploadBindingLive));
    }
    bool create(const rr::platform::VehicleWindowRequest50DS& request,
        const rr::assets::VehicleDrawAssets50CR& assets,
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept {
        d={};
        if(!ownerThread()||attempted_||retiring_||request.context!=&video_->host||
           request.procAddress!=&proc||request.isCurrent!=&current||
           !video_->host.created()||!video_->host.current()) {
            d.error=rr::platform::VehicleWindowError50DS::request;
            d.operation="race display requires its actual current host and canonical creation callbacks";
            return false;
        }
        attempted_=true;
        renderer_.sessionDisplayCreate50LA_=true;
        created_=renderer_.create(request,assets,d); // Real creation, same callbacks/banks.
        renderer_.sessionDisplayCreate50LA_=false;
        return created_;
    }
    // Reached renderer suffix of texture-manager2543F0. This owns the SAME
    // renderer442F70/dispatch, never a copied queue or caller completion flag.
    // It is intentionally not full2543F0: manager13C retirement is separate.
    // Live child pins are allowed: source1A68C4 resets before child destruction.
    rr::game::OriginalCameraStartResult resetRenderer2450A0(
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept {
        using S=rr::game::OriginalCameraStartStatus;
        d={};
        if(!ownerThread()||!created_||!video_->originalMain||
           !video_->host.created()||!video_->host.current()||
           video_->host.sessionDisplayRenderer50LA_!=&renderer_||
           renderer_.sessionDisplayOwner50LA_!=this) {
            d.error=rr::platform::VehicleWindowError50DS::context;
            d.operation="race reset requires its actual current Application display owner";
            return {S::owner,0x2450b0};
        }
        // A real client-transfer scope still owns the borrowed upload bytes.
        // This is incremental admission, not a claim that other upload paths
        // have been registered or that full manager13C is covered.
        if(textureLifetime_->nativeUploads_) {
            d.error=rr::platform::VehicleWindowError50DS::frame;
            d.operation="race reset rejected during registered native texture transfer";
            return {S::rendererDrain,0x2450b8};
        }
        if(!renderer_.idleForRaceReset50LA(d))return {S::rendererDrain,0x2450b8};
        // Exactly one actual drain, including when D8 bit0 was already clear.
        // Failure leaves D8 intact. No GL resources, CPU references or pins
        // are discarded here; downstream source destructors retain ownership.
        return rr::game::resetOriginalRenderer2450A0(dispatch_,renderer_,d);
    }
    // Native direct-upload capability retirement corresponding to the reached
    //2543F0 ordering. This retires REAL sealed bindings, never persistent images
    // or a fabricated guest13C allocation. Arbitrary unbound uploaders and a
    // future cached-transfer backend require their own explicit lifecycle join.
    rr::game::OriginalCameraStartResult retireDirectUploadBindings2543F0(
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept {
        using S=rr::game::OriginalCameraStartStatus;
        d={};
        if(!ownerThread()||textureLifetime_->nativeUploads_||
           textureLifetime_->nativeUploadGeneration_==std::numeric_limits<std::uint64_t>::max()) {
            d.error=rr::platform::VehicleWindowError50DS::request;
            d.operation="native upload retirement requires idle transferable generation";
            return {S::owner,0x254404};
        }
        const auto result=resetRenderer2450A0(d);
        if(result.status!=S::complete)return result;
        // No fallible operation follows the real drain/D8 suffix. Old copies
        // now reject before upload; a later genuine cold owner must rebind.
        ++textureLifetime_->nativeUploadGeneration_;
        return {S::complete,0x254420};
    }
    // Called before root cleanup touches any backing. Outstanding dependents
    // retire explicitly through their real drain path, then root retries here.
    // Failure forbids new acquisitions but does not disable drain retries.
    bool beginCloseAfterDependents()noexcept {
        if(!ownerThread())return false;
        retiring_=true;
        return renderer_.sessionDependentPins50LA_==0&&video_->host.sessionDependentPins50LA_==0;
    }
};

// Private future integration contract for ONE original sixteen-element array.
// No geometry, material references, allocations or gameplay are supplied here.
class PositionStatsRendererLease50LA final {
    friend class RaceDisplayLifetime50LA;
    friend class rr::game::OriginalPositionStatsPrimitiveOwner50LA;
    std::shared_ptr<RaceDisplayLifetime50LA> owner_;
    int next_=15;
    unsigned phase_=0;
    bool constructed_=false;
    explicit PositionStatsRendererLease50LA(std::shared_ptr<RaceDisplayLifetime50LA> owner)
        :owner_(std::move(owner)){owner_->pin();}
    // Call after all16 nonthrowing2E1920 constructors and BEFORE any2E1AE0 load.
    // If allocation fails before construction, ordinary lease disposal unpins.
    bool constructedAll16()noexcept {
        if(!owner_||!owner_->ownerThread()||constructed_)return false;
        constructed_=true;return true;
    }
    int nextElement()const noexcept{return next_;}
    rr::game::OriginalCameraStartResult drainNext2E1980(
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept {
        using S=rr::game::OriginalCameraStartStatus;
        if(!owner_||!owner_->ownerThread()||!constructed_||next_<0)return {S::owner,0x2e1980};
        if(phase_==2)return {S::complete,0x2e19d4};
        if(!phase_){
            std::int32_t actualV0=0;
            const auto r=rr::game::serviceOriginalRenderer244D60(owner_->dispatch_,true,
                owner_->renderer_,d,actualV0,rr::game::OriginalCameraDrainPolicy::initializedContext);
            if(r.status!=S::complete)return r;
            phase_=1;
        }
        // Unconditional2E19CC, including disabled-D8 service and unloaded elements.
        if(!owner_->renderer_.finishInitializedContextWork(d)||!d.contextWorkCompleted)
            return {S::rendererDrain,0x2e19cc};
        phase_=2;return {S::complete,0x2e19d4};
    }
    // Only the concrete friend can acknowledge its real post-drain frees:
    // ascending primitives/ref releases, packets, then owned vertices. No callback.
    bool finishedElementResources(int index)noexcept {
        if(!owner_||!owner_->ownerThread()||!constructed_||next_<0||index!=next_||phase_!=2)return false;
        --next_;phase_=0;
        if(next_<0){owner_->unpin();owner_.reset();}
        return true;
    }
public:
    PositionStatsRendererLease50LA(const PositionStatsRendererLease50LA&)=delete;
    PositionStatsRendererLease50LA& operator=(const PositionStatsRendererLease50LA&)=delete;
    ~PositionStatsRendererLease50LA()noexcept {
        if(constructed_&&next_>=0)std::terminate(); // Never auto-release undrained refs.
        if(owner_)owner_->unpin();
    }
};
inline std::unique_ptr<PositionStatsRendererLease50LA> RaceDisplayLifetime50LA::retainPositionStats16() {
    if(!canRetain())return {};
    return std::unique_ptr<PositionStatsRendererLease50LA>(new PositionStatsRendererLease50LA(shared_from_this()));
}
//Independent of the16-element kind5 lease. Retains the actual display through
//all127 ScreenTexture drains, all5 ScreenText frees, and final Entity cleanup.
//Only the future concrete full owner can construct/use this private protocol.
//No full owner, fixed arrays, resources or staging invocation are supplied here.
class PositionStatsTextureLease50LA final {
    friend class RaceDisplayLifetime50LA;
    friend class rr::game::OriginalPositionStatsResources50LA;
    std::shared_ptr<RaceDisplayLifetime50LA> owner_;
    int nextTexture_=126,nextText_=4;
    unsigned phase_=0;
    bool constructed_=false,retirementStarted_=false,finished_=false;
    explicit PositionStatsTextureLease50LA(std::shared_ptr<RaceDisplayLifetime50LA> owner)
        :owner_(std::move(owner)) {
        owner_->pin();owner_->positionStatsTextureLeaseActive_=true;
    }
    void releasePin()noexcept {
        if(!owner_||!owner_->ownerThread()||!owner_->positionStatsTextureLeaseActive_)std::terminate();
        owner_->unpin();owner_->positionStatsTextureLeaseActive_=false;owner_.reset();
    }
    //18FF80 fixed constructors have no loader allocations. Acquire this lease
    //before constructing5 texts/9 textures/118 textures/clip; mark only after
    //all constructors finish, BEFORE190190 effects or any primitive16 load.
    bool constructedFixedResources18FF80()noexcept {
        if(!owner_||!owner_->ownerThread()||constructed_||retirementStarted_)return false;
        constructed_=true;return true;
    }
    //1900B0: first close actual primitive16 (or observe its original null), then
    //destroy embedded clip119F10. The concrete friend calls here AFTER that
    //resource-free clip destructor; actual pin counts enforce primitive retirement.
    bool beginAfterClip119F10()noexcept {
        if(!owner_||!owner_->ownerThread()||!constructed_||finished_)return false;
        if(retirementStarted_)return true;
        if(!owner_->beginPositionStatsTextureRetirement())return false;
        retirementStarted_=true;return true;
    }
    std::uint32_t nextTextureOffset()const noexcept {
        if(!retirementStarted_||nextTexture_<0)return 0;
        //126..9 =>31B0..5D0 (118 map textures);8..0 =>570..270.
        return nextTexture_<9?0x270u+std::uint32_t(nextTexture_)*0x60u:
            0x5d0u+std::uint32_t(nextTexture_-9)*0x60u;
    }
    rr::game::OriginalCameraStartResult drainNext2DEFE0(
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept {
        using S=rr::game::OriginalCameraStartStatus;
        if(!owner_||!owner_->ownerThread()||!constructed_||!retirementStarted_||nextTexture_<0)
            return {S::owner,0x2defe0};
        if(phase_==2)return {S::complete,0x2df038};
        if(!phase_){
            std::int32_t actualV0=0;
            const auto r=rr::game::serviceOriginalRenderer244D60(owner_->dispatch_,true,
                owner_->renderer_,d,actualV0,rr::game::OriginalCameraDrainPolicy::initializedContext);
            if(r.status!=S::complete)return r;
            phase_=1;
        }
        //2DF030 is unconditional, even if slot20's disabled-D8 path did no GL
        //work, and even for a constructor-only ScreenTexture with null buffers.
        if(!owner_->renderer_.finishInitializedContextWork(d)||!d.contextWorkCompleted)
            return {S::rendererDrain,0x2df030};
        phase_=2;return {S::complete,0x2df038};
    }
    //Concrete friend frees buffer58, clears primitive10 and destroys the actual
    //primitive/material in2DF038..74 order BEFORE acknowledging this exact offset.
    bool finishedTextureResources(std::uint32_t offset)noexcept {
        if(!owner_||!owner_->ownerThread()||nextTexture_<0||!retirementStarted_||
           phase_!=2||offset!=nextTextureOffset())return false;
        --nextTexture_;phase_=0;return true; //Keep pin through texts and Entity.
    }
    std::uint32_t nextTextOffset()const noexcept {
        if(!retirementStarted_||nextTexture_>=0||nextText_<0)return 0;
        return 0x40u+std::uint32_t(nextText_)*0x70u;
    }
    //2E1340->2DFC60 has NO renderer calls. Concrete owner frees string8,
    //glyph arrayC, primitives4 ascending, then optional packet buffers64 and
    //entry array; retires its actual font borrow; only then acknowledges.
    bool finishedTextResources(std::uint32_t offset)noexcept {
        if(!owner_||!owner_->ownerThread()||!retirementStarted_||nextTexture_>=0||
           nextText_<0||offset!=nextTextOffset())return false;
        --nextText_;return true;
    }
    //Native bookkeeping acknowledgement only, not a synthetic completion
    //callback: the sole concrete friend must perform real Entity1413E0 first.
    bool finishedEntity1413E0()noexcept {
        if(!owner_||!owner_->ownerThread()||!constructed_||!retirementStarted_||
           nextTexture_>=0||nextText_>=0||finished_)return false;
        finished_=true;releasePin();return true;
    }
public:
    PositionStatsTextureLease50LA(const PositionStatsTextureLease50LA&)=delete;
    PositionStatsTextureLease50LA& operator=(const PositionStatsTextureLease50LA&)=delete;
    ~PositionStatsTextureLease50LA()noexcept {
        if(constructed_&&!finished_)std::terminate();
        //Allocation failure before fixed construction has no source objects
        //whose destructors must run. Every constructed path requires protocol.
        if(owner_)releasePin();
    }
};
inline std::unique_ptr<PositionStatsTextureLease50LA> RaceDisplayLifetime50LA::retainPositionStatsFixedResources() {
    if(!canRetain()||positionStatsTextureLeaseActive_)return {};
    return std::unique_ptr<PositionStatsTextureLease50LA>(new PositionStatsTextureLease50LA(shared_from_this()));
}
inline std::shared_ptr<RaceDisplayLifetime50LA> Application::raceDisplayForSession50LA()const {
    require(videoOwner_&&raceDisplayOwner50LA_.expired()&&!videoOwner_->host.created()&&
        !videoOwner_->host.ownsResources(),"race display requires actual closed Application Video");
    require(raceDisplayGeneration50LA_!=std::numeric_limits<std::uint64_t>::max(),
        "race display generation exhausted");
    auto owner=std::shared_ptr<RaceDisplayLifetime50LA>(new RaceDisplayLifetime50LA(
        videoOwner_,raceDisplayGeneration50LA_+1,*this,textureLifetimeOwner_));
    ++raceDisplayGeneration50LA_;raceDisplayOwner50LA_=owner;return owner;
}
}
