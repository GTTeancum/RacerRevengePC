// Included once by original_ui_main_entry50kg.cpp after the actual Display.
#include <exception>
#include <limits>
#include <new>
#include <utility>
namespace original_main_entry50KG {
namespace native_video_invocation50LA {
template<class A,class B> bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
struct CallScope final {
    bool& value;
    explicit CallScope(bool& v)noexcept:value(v){value=true;}
    ~CallScope()noexcept{value=false;}
};
}
NativeVideoPacketInvocation50LA::NativeVideoPacketInvocation50LA(const Application& root,
    std::shared_ptr<RaceDisplayLifetime50LA> display,
    std::unique_ptr<const rr::game::OriginalVideoClearPacket25CAD0Native50LA> clear,
    std::unique_ptr<const rr::game::OriginalVideoFinishPacket25AB2CNative50LA> finish,
    std::uint64_t invocation)noexcept
    :application_(&root),lifetime_(root.animationClock()),display_(std::move(display)),
     clear_(std::move(clear)),finish_(std::move(finish)),invocation_(invocation),displayGeneration_(display_->generation_) {}
NativeVideoPacketInvocation50LA::~NativeVideoPacketInvocation50LA()noexcept {
    if(pinned_||custody_||busy_)std::terminate();
}
bool NativeVideoPacketInvocation50LA::reject(Diagnostic& d,const char* message)noexcept {
    d.error=rr::platform::VehicleWindowError50DS::frame;d.operation=message;return false;
}
void NativeVideoPacketInvocation50LA::observe(Info& out)const noexcept {
    out={state_,invocation_,displayGeneration_,pinned_&&custody_,clearAccepted_,finishInfo_};
}
bool NativeVideoPacketInvocation50LA::identityCurrent()const noexcept {
    if(thread_!=std::this_thread::get_id()||!pinned_||!custody_||!display_||!application_)return false;
    std::uint32_t pause=0;
    if(!lifetime_.readPause11C(pause)||!display_->ownerThread())return false;
    const auto& root=*application_;
    using native_video_invocation50LA::sameOwner;
    // The lifetime read precedes every mutable access through the raw root.
    // Display's existing identity query checks its exact renderer/host pins
    // and back-pointers only. It does not execute a localization service.
    return lifetime_.sameOwner(root.animationClock())&&root.nativeVideoPacketInvocation50LA_.get()==this&&
        root.nativeVideoPacketInvocationSerial50LA_==invocation_&&display_->application_==application_&&
        display_->generation_==displayGeneration_&&root.raceDisplayGeneration50LA_==displayGeneration_&&
        sameOwner(display_,root.raceDisplayOwner50LA_.lock())&&
        sameOwner(display_,display_->weak_from_this().lock())&&sameOwner(display_->video_,root.videoOwner_)&&
        display_->localizationServiceIdentityCurrent50KG();
}
bool NativeVideoPacketInvocation50LA::begin(const Application& root,
    std::uint32_t red,std::uint32_t green,std::uint32_t blue,std::uint32_t alpha,
    Policies policies,std::size_t maximumGpuColorBytes,Info& out,Diagnostic& d)noexcept {
    out={};d={};rr::platform::OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return reject(d,"Options excludes native Video invocation");
    // The Application entry checked its immutable owner thread before these
    // mutable root fields. No supplied Display/Video/context can replace them.
    if(root.nativeVideoPacketInvocation50LA_){root.nativeVideoPacketInvocation50LA_->observe(out);
        return reject(d,"Application already owns a native Video invocation");}
    if(root.nativeVideoPacketInvocationSerial50LA_==std::numeric_limits<std::uint64_t>::max())
        return reject(d,"native Video invocation serial exhausted");
    auto display=root.raceDisplayOwner50LA_.lock();
    using native_video_invocation50LA::sameOwner;
    if(!display||!display->ownerThread()||display->application_!=&root||!display->canRetain()||
       !display->frontendOwnerCurrent50KG()||display->generation_!=root.raceDisplayGeneration50LA_||
       !sameOwner(display,display->weak_from_this().lock())||!sameOwner(display->video_,root.videoOwner_))
        return reject(d,"native Video invocation needs this Application's current actual Display");
    // Retain two independent immutable artifacts from THIS source Video. Their
    // encoding does not execute original control flow or mutate source+88.
    auto clear=root.encodeNativeSourceVideoClearPacket25CAD0(red,green,blue,alpha);
    auto finish=root.encodeNativeSourceVideoFinishPacket25AB2C();
    if(!clear.packet||!finish.packet)return reject(d,"source Video has no owned native packet encoding");
    std::unique_ptr<NativeVideoPacketInvocation50LA> next(new(std::nothrow) NativeVideoPacketInvocation50LA(
        root,display,std::move(clear.packet),std::move(finish.packet),root.nativeVideoPacketInvocationSerial50LA_+1));
    if(!next){d.error=rr::platform::VehicleWindowError50DS::allocation;
        d.operation="allocate native Video invocation";return false;}
    display->pin();next->pinned_=true;
    if(!display->renderer_.claimNativeVideoInvocation50LA(*next,d)){
        display->unpin();next->pinned_=false;return false;
    }
    next->custody_=true;
    root.nativeVideoPacketInvocationSerial50LA_=next->invocation_;
    root.nativeVideoPacketInvocation50LA_=std::move(next);
    auto& call=*root.nativeVideoPacketInvocation50LA_;
    native_video_invocation50LA::CallScope scope(call.busy_);
    auto& renderer=display->renderer_;
    // No yield or user callback between these ordered operations. A failed
    // preparation/clear/FINISH is terminal for this invocation, never replayed.
    bool accepted=renderer.prepareNativeVideoPacketClearImpl50LA(*call.clear_,policies,maximumGpuColorBytes,d,&call);
    if(accepted){accepted=renderer.submitNativeVideoPacketClearImpl50LA(*call.clear_,d,&call);
        call.clearAccepted_=accepted;}
    if(accepted)accepted=renderer.beginNativeVideoPacketFinishImpl50LA(*call.finish_,call.finishInfo_,d,&call);
    if(accepted)call.state_=State::pending;
    call.observe(out);return accepted;
}
bool NativeVideoPacketInvocation50LA::poll(Info& out,Diagnostic& d)noexcept {
    out={};d={};
    if(!identityCurrent()||busy_)return reject(d,"native Video poll needs its idle retained invocation");
    if(state_!=State::pending&&state_!=State::completed){observe(out);
        return reject(d,"faulted native Video invocation requires cancellation");}
    native_video_invocation50LA::CallScope scope(busy_);
    rr::platform::NativeVideoPacketFinishInfo50LA observed;
    const bool accepted=display_->renderer_.pollNativeVideoPacketFinishImpl50LA(observed,d,this);
    // Early admission failure supplies no new observation. Preserve the last
    // one instead of reporting that an actually retained marker disappeared.
    if(accepted||observed.operationSerial)finishInfo_=observed;
    if(!accepted)state_=State::faulted;
    else if(finishInfo_.state==rr::platform::NativeVideoPacketFinishState50LA::completed)state_=State::completed;
    // Only this private renderer call, under exact pointer custody, can change
    // pending to completed. A caller's copied serial/info never enters here.
    observe(out);return accepted;
}
bool NativeVideoPacketInvocation50LA::releaseCustody(Diagnostic& d)noexcept {
    if(!display_->renderer_.releaseNativeVideoInvocation50LA(*this,d))return false;
    custody_=false;display_->unpin();pinned_=false;
    clear_.reset();finish_.reset();return true;
}
bool NativeVideoPacketInvocation50LA::cancel(Info& out,Diagnostic& d)noexcept {
    out={};d={};
    if(!identityCurrent()||busy_)return reject(d,"native Video cancel needs its idle retained invocation");
    native_video_invocation50LA::CallScope scope(busy_);
    // Cancellation does not require initialized targets, a non-faulted state,
    // new-pin capacity or a non-retiring Display. Uncertain retirement retains
    // the exact command/marker custody, packets and both dependent pins.
    rr::platform::NativeVideoPacketFinishInfo50LA observed;
    const bool cancelled=display_->renderer_.cancelNativeVideoPacketFinishImpl50LA(observed,d,this);
    if(cancelled||observed.operationSerial)finishInfo_=observed;
    if(!cancelled||!releaseCustody(d)){
        state_=State::faulted;observe(out);return false;
    }
    state_=State::cancelled;observe(out);return true;
}
bool NativeVideoPacketInvocation50LA::retire(Info& out,Diagnostic& d)noexcept {
    out={};d={};
    if(!identityCurrent()||busy_||state_!=State::completed)
        return reject(d,"native Video retirement requires its completed invocation");
    native_video_invocation50LA::CallScope scope(busy_);
    // Revalidate the actual renderer's historical completion and fault state
    // before releasing custody. No copied observation can authorize retirement.
    rr::platform::NativeVideoPacketFinishInfo50LA observed;
    const bool completed=display_->renderer_.pollNativeVideoPacketFinishImpl50LA(observed,d,this);
    if(completed||observed.operationSerial)finishInfo_=observed;
    if(!completed||observed.state!=rr::platform::NativeVideoPacketFinishState50LA::completed||!releaseCustody(d)){
        state_=State::faulted;observe(out);return false;
    }
    state_=State::retired;observe(out);return true;
}
bool Application::beginNativeVideoPacketInvocation50LA(std::uint32_t red,std::uint32_t green,
    std::uint32_t blue,std::uint32_t alpha,rr::platform::OriginalVideoPacketCpuRenderer50LA::NativePolicies policies,
    std::size_t maximumGpuColorBytes,NativeVideoPacketInvocationInfo50LA& out,
    rr::platform::VehicleWindowDiagnostic50DS& d)const noexcept {
    out={};d={};
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this))
        return NativeVideoPacketInvocation50LA::reject(d,"native Video entry requires actual Application owner thread");
    return NativeVideoPacketInvocation50LA::begin(*this,red,green,blue,alpha,policies,maximumGpuColorBytes,out,d);
}
bool Application::pollNativeVideoPacketInvocation50LA(NativeVideoPacketInvocationInfo50LA& out,
    rr::platform::VehicleWindowDiagnostic50DS& d)const noexcept {
    out={};d={};
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!nativeVideoPacketInvocation50LA_)
        return NativeVideoPacketInvocation50LA::reject(d,"Application has no current native Video invocation");
    return nativeVideoPacketInvocation50LA_->poll(out,d);
}
bool Application::cancelNativeVideoPacketInvocation50LA(NativeVideoPacketInvocationInfo50LA& out,
    rr::platform::VehicleWindowDiagnostic50DS& d)const noexcept {
    out={};d={};
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!nativeVideoPacketInvocation50LA_)
        return NativeVideoPacketInvocation50LA::reject(d,"Application has no current native Video invocation");
    if(!nativeVideoPacketInvocation50LA_->cancel(out,d))return false;
    nativeVideoPacketInvocation50LA_.reset();return true;
}
bool Application::retireNativeVideoPacketInvocation50LA(NativeVideoPacketInvocationInfo50LA& out,
    rr::platform::VehicleWindowDiagnostic50DS& d)const noexcept {
    out={};d={};
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!nativeVideoPacketInvocation50LA_)
        return NativeVideoPacketInvocation50LA::reject(d,"Application has no current native Video invocation");
    if(!nativeVideoPacketInvocation50LA_->retire(out,d))return false;
    nativeVideoPacketInvocation50LA_.reset();return true;
}
}
