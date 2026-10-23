#include "original_localization_renderer_lease50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include "../../../src/assets/original_application_localization50kg.h"
#include <atomic>
#include <exception>
#include <limits>
#include <utility>
namespace rr::platform {
namespace {
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
using Status=OriginalLocalizationRendererStatus50KG;
using Diagnostic=OriginalLocalizationRendererDiagnostic50KG;
bool reject(Diagnostic& d,Status s,std::uint32_t pc)noexcept {d={s,pc,0,0,false};return false;}
}
// These are process-lifetime source records, not per-Application defaults.
// Atomics provide only native race safety. They do not claim guest atomics.
// The native no-error adapter touches only its reached lazy initializer.
// It does not read these unjoined guest busy/frame/recovery-history values.
bool OriginalLocalizationRendererLease50KG::ProcessState::beginIdle242F08()noexcept {
    if(!guard88F4.load()){
        lastErrorFrame4355E0.store(0); //242F20 before guard242F24.
        guard88F4.store(1);
        return true;
    }
    return false;
}
bool OriginalLocalizationRendererLease50KG::ProcessState::finishNoError2439FC(
    std::uint32_t videoFrame,std::uint32_t& rendererError)noexcept {
    // SLL/SUBU/SLL/ADDU produce a wrapping32-bit threshold, then SLTU.
    const auto scale=recoveryScale382FA8.load();
    const auto thirty=((scale<<4)-scale)<<1;
    const auto threshold=lastErrorFrame4355E0.load()+thirty;
    if(threshold<videoFrame){rendererError=0;return true;} //243A44.
    return false;
}
OriginalLocalizationRendererLease50KG::ProcessState&
OriginalLocalizationRendererLease50KG::processState()noexcept {
    static ProcessState records;return records;
}
bool OriginalLocalizationRendererLease50KG::processContinuityCurrent()noexcept {
    // Existing native completion helpers do not own the actual443DC8 producer
    // or all382FA8/4355E0 consumers. ELF initial values cannot attest live
    // history. There is intentionally no caller-supplied ready flag/token.
    return false;
}
bool OriginalLocalizationRendererLease50KG::sourceCallExclusiveCurrent()noexcept {
    // No general guest root-dispatch/frame-history claim exists. The dedicated
    // native adapter instead owns one non-yielding same-thread scope and never
    // returns a resumable boundary between its two ordered service calls.
    return false;
}
OriginalLocalizationRendererLease50KG::OriginalLocalizationRendererLease50KG(
    const Application& root,std::shared_ptr<Authority> authority,std::shared_ptr<Dictionary> dictionary,
    std::shared_ptr<Display> display,std::uint64_t invocation)noexcept
    :issuer_(&root),authority_(std::move(authority)),dictionary_(std::move(dictionary)),
     display_(std::move(display)),lifetime_(root.animationClock()),invocation_(invocation),
     displayGeneration_(display_->generation_) {}
bool OriginalLocalizationRendererLease50KG::stage(const Application& root,
    std::shared_ptr<OriginalLocalizationRendererLease50KG>& out,Diagnostic& d)noexcept {
    d={};
    const auto authority=root.entities(); //Immutable actual authority before mutable root reads.
    if(!authority||authority->nativeOwnerThread_!=std::this_thread::get_id()||
       !authority->constructed_||authority->released_||out)
        return reject(d,Status::authority,0x14C5A0);
    std::uint32_t pause=0;
    if(!root.animationClock().readPause11C(pause))return reject(d,Status::authority,0x14C5A0);
    const auto dictionary=root.sourceLocalization32COwner_;
    if(!dictionary||!dictionary->current()||dictionary->phase_!=Dictionary::Phase::indexed)
        return reject(d,Status::dictionaryPhase,0x14C5A0);
    if(root.localizationRendererCall14C5A0_||
       sameOwner(root.localizationRendererAttempted32C_.lock(),dictionary)||
       root.localizationRendererInvocationSerial_==std::numeric_limits<std::uint64_t>::max())
        return reject(d,Status::nativeInvocationEnvelope,0x14C5A0);
    auto display=root.raceDisplayOwner50LA_.lock();
    if(!display||!display->ownerThread()||display->application_!=&root||
       !sameOwner(display,display->weak_from_this().lock())||display->generation_!=root.raceDisplayGeneration50LA_||
       !sameOwner(display->video_,root.videoOwner_)||!display->canRetainSourceFogField())
        return reject(d,Status::displayOwner,0x14C5D0);
    // Constructed owner retention is allowed while a real context/producer
    // remains pending. It does NOT pass canRetain() or assert GPU readiness.
    try {
        auto value=std::shared_ptr<OriginalLocalizationRendererLease50KG>(
            new OriginalLocalizationRendererLease50KG(root,authority,dictionary,display,
                root.localizationRendererInvocationSerial_+1));
        // All allocation precedes native pins/root publication. A shared_ptr
        // control-block failure therefore destroys only an unpinned object.
        value->issued_=value;
        display->pin();value->pinned_=true;
        root.localizationRendererInvocationSerial_=value->invocation_;
        root.localizationRendererAttempted32C_=dictionary;
        root.localizationRendererCall14C5A0_=value;out=value;
        value->finish(d,Status::staged,0x14C5D0);return true;
    }catch(...){return reject(d,Status::allocation,0x14C5A0);}
}
bool OriginalLocalizationRendererLease50KG::cleanupIdentityCurrent()const noexcept {
    if(thread_!=std::this_thread::get_id()||!pinned_||!issuer_||!authority_||!dictionary_||!display_||
       phase_==Phase::cancelled||phase_==Phase::retired)return false;
    std::uint32_t pause=0;
    if(!lifetime_.readPause11C(pause)||authority_->nativeOwnerThread_!=thread_||
       !authority_->constructed_||authority_->released_)return false;
    // Only after the immutable thread and retained lifetime checks may the
    // raw issuer's mutable owner/control-block identities be observed.
    return sameOwner(authority_,issuer_->entities())&&lifetime_.sameOwner(issuer_->animationClock())&&
        sameOwner(dictionary_,issuer_->sourceLocalization32COwner_)&&dictionary_->cleanupIdentityCurrent()&&
        sameOwner(issued_.lock(),issuer_->localizationRendererCall14C5A0_)&&
        sameOwner(dictionary_,issuer_->localizationRendererAttempted32C_.lock())&&
        issuer_->localizationRendererInvocationSerial_==invocation_&&
        display_->ownerThread()&&display_->application_==issuer_&&display_->generation_==displayGeneration_&&
        issuer_->raceDisplayGeneration50LA_==displayGeneration_&&
        sameOwner(display_,display_->weak_from_this().lock())&&
        sameOwner(display_,issuer_->raceDisplayOwner50LA_.lock())&&
        sameOwner(display_->video_,issuer_->videoOwner_)&&display_->localizationServiceIdentityCurrent50KG();
}
bool OriginalLocalizationRendererLease50KG::current()const noexcept {
    return cleanupIdentityCurrent()&&dictionary_->current();
}
bool OriginalLocalizationRendererLease50KG::permitsEnglishSelection14C5F0(
    const Application& root,const Dictionary& dictionary)const noexcept {
    if(thread_!=std::this_thread::get_id()||!current()||issuer_!=&root||
       dictionary_.get()!=&dictionary||!sameOwner(dictionary_,root.sourceLocalization32COwner_)||
       !sameOwner(issued_.lock(),root.localizationRendererCall14C5A0_)||
       invocation_!=root.localizationRendererInvocationSerial_||phase_!=Phase::servicesComplete||
       completedCalls_!=2||everFaulted_||display_->retiring_||display_->localizationServiceFaulted_||
       display_->localizationServiceActive_)return false;
    return true;
}
bool OriginalLocalizationRendererLease50KG::finish(Diagnostic& d,Status status,std::uint32_t pc)const noexcept {
    diagnosticPC_=pc;d={status,pc,invocation_,completedCalls_,sourceEffects_};return status==Status::complete;
}
bool OriginalLocalizationRendererLease50KG::service242EE0(Diagnostic& d,std::uint32_t callPC,bool firstCall)noexcept {
    // Only advance's non-yielding same-thread scope calls this helper. It
    // invokes no caller callbacks, event dispatch, source frames or uploads.
    if(!display_->localizationServiceActive_){
        everFaulted_=true;phase_=Phase::faulted;display_->localizationServiceFaulted_=true;
        return finish(d,Status::nativeCompletionFault,callPC);
    }
    auto& process=processState();
    sourceEffects_=process.beginIdle242F08()||sourceEffects_;
    VehicleWindowDiagnostic50DS native;
    bool completed=false;
    if(firstCall){
        std::int32_t resultV0=0;
        const auto result=game::serviceOriginalRenderer244D60(display_->dispatch_,true,display_->renderer_,
            native,resultV0,game::OriginalCameraDrainPolicy::initializedContext);
        completed=result.status==game::OriginalCameraStartStatus::complete&&native.contextWorkCompleted;
    }else completed=display_->renderer_.finishInitializedContextWork(native)&&native.contextWorkCompleted;
    if(!completed){
        // Native failure is terminal, not source hardware recovery and not a
        // retryable completed prefix. Keep both exact pins until cancellation.
        everFaulted_=true;phase_=Phase::faulted;display_->localizationServiceFaulted_=true;
        return finish(d,Status::nativeCompletionFault,callPC);
    }
    // Source2439FC..243A44 either preserves C4 or stores zero. Under the exact
    // C4==0 no-error admission both have identical observable state, without
    // reading an unproduced Video40 or claiming a conditional store happened.
    return true;
}
bool OriginalLocalizationRendererLease50KG::advance(const Application& root,Diagnostic& d)noexcept {
    if(thread_!=std::this_thread::get_id())return reject(d,Status::authority,0x14C5D0);
    if(!current()||issuer_!=&root)return finish(d,Status::authority,0x14C5D0);
    if(display_->retiring_)return finish(d,Status::displayOwner,0x14C5D0);
    if(phase_==Phase::faulted||display_->localizationServiceFaulted_)
        return finish(d,Status::nativeCompletionFault,completedCalls_?0x14C5E8u:0x14C5D0u);
    if(phase_==Phase::servicesComplete)return finish(d,Status::complete,0x14C5F0);
    // No resumable boundary exists between these two calls. A completed first
    // call can only lead to second-call completion or terminal native fault.
    if(phase_!=Phase::beforeFirst||display_->localizationServiceActive_)
        return finish(d,Status::sourceCallExclusivityPending,0x14C5A0);
    if(display_->sourceRendererService_.errorC4_!=0)
        return finish(d,Status::nativeErrorStateUnsupported,0x2439E0);
    if(display_->dispatch_.pendingCallbackDC)
        return finish(d,Status::callbackPending,0x242F78);
    if(display_->dispatch_.count54!=0||display_->dispatch_.count60!=0||
       (display_->dispatch_.currentQueue44!=1&&display_->dispatch_.currentQueue44!=2))
        return finish(d,Status::queuePending,0x244E10);
    if(!display_->created_||!display_->frontendCreated_||!display_->video_->originalMain||
       !display_->renderer_.initialized()||!display_->video_->host.created()||!display_->video_->host.current())
        return finish(d,Status::nativeContextPending,0x14C5D0);
    if(!display_->localizationFrontendTransfersIdle50KG())
        return finish(d,Status::nativeTransferPending,0x242F2C);
    VehicleWindowDiagnostic50DS native;
    if(!display_->localizationServiceIdle50KG(native))
        return finish(d,Status::nativeTransferPending,0x14C5D0);
    struct SynchronousScope {
        bool& active;
        explicit SynchronousScope(bool& value)noexcept:active(value){active=true;}
        ~SynchronousScope()noexcept{active=false;}
    } scope(display_->localizationServiceActive_);
    if(display_->dispatch_.flagsD8&1u){
        if(!service242EE0(d,0x14C5D0,true))return false;
    }else{
        //244D7C has no drain and no242EE0 lazy initializer. The following
        //second call is still unconditional. Never add an artificial first GL finish.
        std::int32_t v0=0;
        const auto result=game::serviceOriginalRenderer244D60(display_->dispatch_,true,display_->renderer_,
            native,v0,game::OriginalCameraDrainPolicy::initializedContext);
        if(result.status!=game::OriginalCameraStartStatus::complete){
            everFaulted_=true;phase_=Phase::faulted;display_->localizationServiceFaulted_=true;
            return finish(d,Status::nativeCompletionFault,0x14C5D0);
        }
    }
    completedCalls_=1;phase_=Phase::beforeSecond;
    if(!service242EE0(d,0x14C5E8,false))return false;
    completedCalls_=2;phase_=Phase::servicesComplete;
    // Only these services complete. Authenticated14C850/BIN/row.text selection
    // has a separate owner and is not performed or made ready here.
    return finish(d,Status::complete,0x14C5F0);
}
bool OriginalLocalizationRendererLease50KG::cancel(const Application& root,Diagnostic& d)noexcept {
    if(thread_!=std::this_thread::get_id())return reject(d,Status::authority,0x14C5A0);
    if(!cleanupIdentityCurrent()||issuer_!=&root)return finish(d,Status::authority,0x14C5A0);
    if(display_->localizationServiceActive_)return finish(d,Status::sourceCallExclusivityPending,0x14C5A0);
    auto keepAlive=root.localizationRendererCall14C5A0_;
    // Native cancellation releases retention only; it never rolls back shared
    // source effects. The consumed control-block ledger remains. This bounded
    // one-invocation policy is NOT an original prohibition of future14C5A0.
    phase_=Phase::cancelled;
    root.localizationRendererCall14C5A0_.reset();
    display_->unpin();pinned_=false;
    display_.reset();dictionary_.reset();authority_.reset();issued_.reset();
    issuer_=nullptr;lifetime_={};
    finish(d,Status::cancelled,0x14C5A0);return true;
}
bool OriginalLocalizationRendererLease50KG::inspect(Observation& out)const noexcept {
    if(thread_!=std::this_thread::get_id())return false;
    if(phase_!=Phase::cancelled&&phase_!=Phase::retired&&!current())return false;
    out={invocation_,displayGeneration_,completedCalls_,pinned_,sourceEffects_,
        phase_==Phase::servicesComplete,phase_==Phase::cancelled,everFaulted_,false,
        phase_==Phase::cancelled?0u:diagnosticPC_};return true;
}
OriginalLocalizationRendererLease50KG::~OriginalLocalizationRendererLease50KG()noexcept {
    if(pinned_)std::terminate(); //Actual root cancellation/retirement is required.
}
}
namespace original_main_entry50KG {
bool Application::stageLocalizationRenderer14C5A0(
    std::shared_ptr<rr::platform::OriginalLocalizationRendererLease50KG>& out,
    rr::platform::OriginalLocalizationRendererDiagnostic50KG& d)const noexcept {
    return rr::platform::OriginalLocalizationRendererLease50KG::stage(*this,out,d);
}
bool Application::advanceLocalizationRenderer14C5A0(rr::platform::OriginalLocalizationRendererLease50KG& call,
    rr::platform::OriginalLocalizationRendererDiagnostic50KG& d)const noexcept {return call.advance(*this,d);}
bool Application::cancelLocalizationRenderer14C5A0(rr::platform::OriginalLocalizationRendererLease50KG& call,
    rr::platform::OriginalLocalizationRendererDiagnostic50KG& d)const noexcept {return call.cancel(*this,d);}
}
