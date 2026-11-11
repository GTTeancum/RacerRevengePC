#include "native_common_results50pd.h"
#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "original_camera_session50kx.h"
#include "original_race_display_lifetime50la.h"
#include "ra_cold_aggregate50la.h"
#include "../../src/game/original_pause_storage50la.h"
#include "../../src/game/original_results104_storage50la.h"
#include "original_results_native_frame50la.h"
#include "original_results_renderer50la.h"
#include "original_position_stats_adapter50la.h"
#include "original_race_hud_scope50la.h"
#include "../../platform/linux/native/native_camera_view50dx.h"
#include <cstring>
#include <bit>
#include <thread>

namespace rr::game {
bool OriginalResults10450LA::pinNativeFrameCollector50LA(
    assets::OriginalStandardMaterialTable50LA& expected,
    const assets::OriginalStandardMaterialCollectorReceipt50LA& receipt)noexcept {
    if(!impl_||impl_->nativeCaptureThread!=std::this_thread::get_id()||impl_->nativeFrameBorrows||
       !impl_->domain||!impl_->nativeDisplay||&impl_->nativeDisplay->receipt!=&receipt)return false;
    const auto table=impl_->domain->materials382B38();
    return table.get()==&expected&&table->pinNativeCollector(receipt);
}
void OriginalResults10450LA::unpinNativeFrameCollector50LA(
    assets::OriginalStandardMaterialTable50LA& expected)noexcept {
    if(!impl_||impl_->nativeCaptureThread!=std::this_thread::get_id()||impl_->nativeFrameBorrows||
       !impl_->domain)std::terminate();
    const auto table=impl_->domain->materials382B38();
    if(table.get()!=&expected)std::terminate();
    table->unpinNativeCollector();
}
}

namespace original_main_entry50KG {
bool Application::updateNativeResultsAfterRace50LA(const original_camera_session50KX::Session& returnedCamera,
    rr::game::OriginalResults104Diagnostic50LA& d)const noexcept {
    using S=rr::game::OriginalResults104Diagnostic50LA::Status;
    using Race=rr::game::OriginalRaceConstructor50LA;
    using Camera=original_camera_session50KX::Session;
    d={};
    const auto stop=[&](S status,std::uint32_t pc,const char* why)noexcept {
        d.status=status;d.sourcePC=pc;d.operation=why;return false;
    };
    const auto skipped=[&](const char* why)noexcept {
        // Success of this NATIVE scheduling decision, not of an uncalled
        // source callback. No update attempt, timer or source history is made.
        d.status=S::complete;d.sourcePC=0;d.operation=why;return true;
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return stop(S::owner,0,"Native Results return requires Application owner thread");
    const auto race=sourceRace28COwner_;
    // Preserve spectator and legacy roots exactly. Only the separately
    // published native accepted-world HUMAN chain has acquired this join.
    if(!race)return skipped("No canonical Race: existing native route unchanged");
    if(race->resultsFeedbackThread_!=std::this_thread::get_id())
        return stop(S::owner,0,"Native Results return requires its Race owner thread");
    if(race->nativeHumanRootStep_==Race::NativeHumanRootStep::unreached)
        return skipped("No native human-root publication: existing route unchanged");
    if(race->nativeHumanRootStep_!=Race::NativeHumanRootStep::published)
        return stop(S::owner,0,"Native human-root publication is partial or retiring");
    if(!nativeTaRootEntered_)
        return skipped("No accepted active native Root: Results not scheduled");
    if(nativeTaRootAborted_||nativeTaRootRace_!=race.get()||sourceUpdateRenderBypass228Bit3_||
       sourceApplicationMode22C_!=2||!sourceApplicationElapsed23C_||!nativeTaRootController_||
       race->nativeIntroControllerSlot48()!=nativeTaRootController_||
       race->entity_.sessionGeneration!=nativeTaRootGeneration_)
        return stop(S::owner,0x140ce4,"Native Results suffix requires same accepted active Root23C");
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!nativeTaRootTiming_.readPrepared(sample,serial)||serial!=nativeTaRootPublication_||
       std::bit_cast<std::uint32_t>(sample.updateSeconds)!=
           std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_)||
       !sourceAural00Owner_||!sourceAural00Owner_->raceIntroPublicationServiced(nativeTaRootTiming_,serial))
        return stop(S::owner,0x140ce4,"Native Results suffix requires actual current capped23C and serviced Aural publication");
    // Separate native ownership check permits a genuinely disabled Entity;
    // the source eligibility tests below decide whether slot38 is reached.
    // Existing input/direct-update consumers retain their active requirement.
    if(!resultsPublishedRoot50LA(*race,d,false))return false;
    auto* root=entitiesOwner_->application10();
    const auto& outer=entitiesOwner_->outer26C_;
    const auto authority=entitiesOwner_->entityAuthority_.view();
    if(!contains(authority,&outer)||outer.firstChild18!=root||root->parent1C!=&outer||
       root->previous10||root->next14)
        return stop(S::owner,0x141ed0,"Native Results suffix actual outer26C/root10 ancestry changed");
    const auto eligible=[](const rr::game::OriginalWreckComponent50LA& c)noexcept {
        //141EFC..141F34: activity bit0, neither bit2 nor bit3, OWN mask bit1.
        return (c.flags29&13u)==1u&&(c.mask20&2u);
    };
    if(!(outer.descendants24&2u)||!eligible(*root)||!(root->descendants24&2u))
        return skipped("Actual outer/root update traversal is masked or disabled");
    // This join extends the existing native Race callback route, not an
    // alternate dispatcher that executes UI siblings while Race is disabled.
    if(!eligible(race->entity_))
        return skipped("Native Race update domain disabled; existing route unchanged");
    const auto* pause=&race->pause100_->entity_->component;
    if(eligible(*pause))
        return stop(S::incomplete,0x141f60,"Earlier active Pause slot38 must return before Results; native suffix unsupported");
    const auto* results=race->results104_->entity();
    if(!eligible(*results))
        return skipped("Actual Results slot38 skipped by its source activity/mask gates");
    // The old native frame result is NOT completion of the separately
    // suspended canonical1A7DD0/1AB520 invocation. Even dispatchReturned leaves
    //1A7E40 and the rest of1A7B40 unrepresented; never schedule past that cut.
    // Common race: 1AB520 returned with the race78 playback on. The race
    // keeps updating (1A7B40) and the Results update follows every replay
    // frame, as the original Results run over the replay.
    const bool commonReplay=race->endDispatchReturned_&&
        native_common_results50PD::state().replayActive;
    if(!commonReplay&&(race->endEntryStep_!=Race::EndEntryStep::subtype||race->endEntryProfile_||
       race->endDispatchPublication_||race->endDispatchReturned_))
        return stop(S::incomplete,0,"Canonical Race end continuation has not returned; native Camera publication is not its receipt");
    try {
        const auto world=race->world74_;
        if((!world&&!commonReplay)||race->cameraNativeAdapter34_!=&returnedCamera||
           !race->ownsNativeCameraAdapter34(returnedCamera)||
           race->cameraInitStep_!=Race::CameraInitStep::nativeReady)
            return stop(S::owner,0,"Native Results return lacks actual same-world Camera34 custody");
        ra_cold_aggregate50LA::Diagnostic worldDiagnostic;
        std::shared_ptr<const Camera> borrowed;
        if(world)borrowed=world->borrowRuntimeSourceCamera(world,worldDiagnostic);
        const Camera* camera=world?borrowed.get():&returnedCamera;
        if(!camera||camera!=&returnedCamera||camera->closed_||camera->abandoned_||
           !camera->coldBound_||!camera->readsBound_||camera->actual_.applicationEntities!=entitiesOwner_||
           camera->generation_!=nativeTaRootGeneration_||camera->frameStage_!=Camera::FrameStage::published||
           !camera->frameNumber_||camera->completedFrame_!=camera->frameNumber_||!camera->publicationCount_||
           std::bit_cast<std::uint32_t>(camera->frameDelta_)!=std::bit_cast<std::uint32_t>(sample.updateSeconds))
            return stop(S::owner,0,"Native Results requires current actual returned Camera frame, not a caller snapshot");
        if(camera->pendingDisplayWork())
            return skipped("Actual native display continuation still pending; Results not scheduled");
        const auto display=raceDisplayOwner50LA_.lock();
        if(!display||display!=race->loadDisplay_||display->application_!=this||display->video_!=videoOwner_||
           !display->gameplayDisplayIdentityCurrent50LA()||display->selectedGameplayCamera0_!=camera||
           camera->selectedGameplayDisplay_.lock()!=display)
            return stop(S::owner,0,"Native Results requires same selected Camera/Display/Video lifetime");
        // The real Loop alone calls at its two successful ordered native
        // callback-return cuts, BEFORE140CFC closes mode2. The existing
        // native Aural/Camera/world route is not full1483E0/103720/141ED0:
        // original Light/dialog updates remain outside this adapter. Do not
        // retrospectively invoke them here or claim renderer+20(0) / full
        // Race1A7B40 completion.
        // updateConsumedResults authenticates the same Root again, then runs
        //1C8AE0 ->1B9FE0 with actual current host availability, not input ACK.
        return updateConsumedResults1C8AE0(d);
    }catch(...){
        return stop(S::owner,0,"Native Results same-world synchronous Camera borrow unavailable");
    }
}
}

namespace original_main_entry50KG {
namespace {
using ResultsD=rr::game::OriginalResults104Diagnostic50LA;
bool resultsFrameReject(ResultsD& d,std::uint32_t pc,const char* why)noexcept {
    d.status=ResultsD::Status::owner;d.sourcePC=pc;d.operation=why;return false;
}
bool resultsMatrixCurrent(const original_camera_session50KX::Session& camera,
    const rr::platform::VehicleWindowFrame50DS& frame)noexcept {
    if(frame.width<=0||frame.height<=0||!frame.worldViewProjection||!camera.hasView())return false;
    const rr::platform::NativeCameraViewContract50DX contract{
        rr::platform::NativeCameraViewProvenance50DX::currentCore49FForOutput,frame.width,frame.height,
        rr::platform::NativeCameraClipDepth50DX::negativeOneToOne};
    rr::platform::NativeCameraMatrix50DX converted;rr::platform::NativeCameraViewError50DX error;
    return rr::platform::convertNativeCameraView50DX(camera.view(),contract,converted,error)&&
        !std::memcmp(frame.worldViewProjection,converted.worldViewProjection,sizeof converted.worldViewProjection);
}
}
struct NativeResultsFrame50LA::Impl {
    const std::thread::id thread=std::this_thread::get_id();
    const Application* issuer=nullptr;
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> race;
    std::shared_ptr<ra_cold_aggregate50LA::Owner> world;
    std::shared_ptr<const original_camera_session50KX::Session> camera; //Common race: non-owning alias.
    std::shared_ptr<RaceDisplayLifetime50LA> display;
    std::shared_ptr<ResultsRendererLease50LA> lease;
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table;
    rr::game::OriginalResults10450LA* source=nullptr;
    const rr::game::OriginalResults10450LA::Impl::NativeDisplayCapture* capture=nullptr;
    rr::platform::VehicleWindowFrame50DS* frame=nullptr;
    std::uint64_t rootPublication=0,cameraPublication=0,selectionRevision=0,displayGeneration=0;
    std::int32_t width=0,height=0;
    bool tablePinned=false,sourcePinned=false;
};
NativeResultsFrame50LA::NativeResultsFrame50LA():impl_(new Impl){}
NativeResultsFrame50LA::~NativeResultsFrame50LA()noexcept {
    if(!impl_)return;auto& n=*impl_;
    if(n.thread!=std::this_thread::get_id())std::terminate();
    if(n.frame&&n.frame->nativeResults==this)n.frame->nativeResults=nullptr;
    if(n.sourcePinned){
        if(!n.source||!n.source->impl_||n.source->impl_->nativeFrameBorrows!=1)std::terminate();
        --n.source->impl_->nativeFrameBorrows;
    }
    if(n.tablePinned)n.source->unpinNativeFrameCollector50LA(*n.table);
    // Real renderer owns copied bytes; source lease retains subsequent actual
    // texture/polygon retirement. Scope release is not a GPU/source receipt.
}
bool NativeResultsFrame50LA::current(const rr::platform::VehicleWindowGl50DS& renderer,
    const rr::platform::VehicleWindowFrame50DS& frame)const noexcept {
    return impl_&&impl_->thread==std::this_thread::get_id()&&impl_->issuer&&
        impl_->issuer->nativeResultsFrameCurrent50LA(*this,renderer,frame);
}
bool Application::captureNativeResultsAfterRoot50LA(const original_camera_session50KX::Session& returned,
    ResultsD& d)const noexcept {
    d={};
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return resultsFrameReject(d,0x1a8698,"Results native accepted return Application thread");
    const auto race=sourceRace28COwner_;
    if(!race||race->nativeHumanRootStep_==rr::game::OriginalRaceConstructor50LA::NativeHumanRootStep::unreached)return true;
    if(race->resultsFeedbackThread_!=std::this_thread::get_id()||!race->results104_||!race->results104_->impl_)
        return resultsFrameReject(d,0x1a8698,"Results actual source owner absent");
    auto& source=*race->results104_;auto& p=*source.impl_;
    if(p.nativeCaptureThread!=std::this_thread::get_id()||race->application_!=entitiesOwner_||
       p.application!=entitiesOwner_||race->profile_!=riderProfile||!race->counted_||race->rolledBack_||
       race->entity_.sessionGeneration!=p.component.sessionGeneration||nativeTaRootRace_!=race.get()||
       nativeTaRootGeneration_!=p.component.sessionGeneration||
       race->nativeHumanRootStep_!=rr::game::OriginalRaceConstructor50LA::NativeHumanRootStep::published)
        return resultsFrameReject(d,0x1a8698,"Results same published native source root absent");
    if(p.nativeFrameBorrows)return resultsFrameReject(d,0x1a8698,"Results prior Frame is still borrowed");
    p.nativeDisplay.reset(); //A skipped/failed current offer never retains a prior image as current.
    if(!resultsPublishedRoot50LA(*race,d,false))return false;
    const auto* result=source.entity();
    //1A8698..86BC is a DIRECT call: only Results flags29 bit0 is tested.
    //Its sibling maskFFF7 deliberately excludes root141D70 duplicate calls.
    if(!result||(result->flags29&1u)==0)return true;
    const auto eligible=[](const rr::game::OriginalWreckComponent50LA& e){return (e.flags29&9u)==1u&&(e.mask20&8u);};
    const auto* rootEntity=entitiesOwner_->application10();const auto& outer=entitiesOwner_->outer26C_;
    if(!(outer.descendants24&8u)||!rootEntity||!eligible(*rootEntity)||
       !(rootEntity->descendants24&8u)||!eligible(race->entity_))return true;
    // A successful native scheduling offer can be a source update skip. Do
    // not draw stale state or turn such inactive/masked paths into an abort.
    if(!p.updateCpu||p.updateCpu->publication!=nativeTaRootPublication_||
       p.updateCpu->prefix!=rr::game::OriginalResults10450LA::UpdateCpuPrefix::baseReturned||
       p.updateCpu->nextSourcePC!=0x1c8c1c)return true;
    const auto* pause=race->pause100_?race->pause100_->entity_.get():nullptr;
    if(pause&&(pause->component.flags29&1u)){
        d.status=ResultsD::Status::incomplete;d.sourcePC=0x1a8690;
        d.operation="Results adapter unselected: active preceding Pause tail unsupported";return true;
    }
    if(nativeTaRootEntered_||nativeTaRootAborted_||nativeTaRootRace_!=race.get()||
       nativeTaRootGeneration_!=race->entity_.sessionGeneration||sourceApplicationMode22C_!=0||
       sourceUpdateRenderBypass228Bit3_||!p.updateCpu||p.updateCpu->publication!=nativeTaRootPublication_||
       p.updateCpu->prefix!=rr::game::OriginalResults10450LA::UpdateCpuPrefix::baseReturned||
       p.updateCpu->nextSourcePC!=0x1c8c1c||!race->ownsNativeCameraAdapter34(returned))
        return resultsFrameReject(d,0x1a86bc,"Results capture needs this genuinely returned native Root/update");
    const auto display=raceDisplayOwner50LA_.lock();
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    if(!display||display!=race->loadDisplay_||display->application_!=this||display->video_!=videoOwner_||
       !display->gameplayDisplayIdentityCurrent50LA()||returned.actual_.humanCount34!=1||
       returned.frameStage_!=original_camera_session50KX::Session::FrameStage::published||
       !returned.frameNumber_||returned.completedFrame_!=returned.frameNumber_||returned.pendingDisplayWork()||
       !display->readGameplayViewport115990(returned,rectangle,revision)||
       rectangle!=std::array<std::int32_t,4>{{0,0,640,448}})
        return resultsFrameReject(d,0x1a86bc,"Results accepted native final one-human Camera/Display absent");
    const auto hud=race->endHud_.lock();
    if(!hud||!hud->endStateCurrent(*race,returned)||!hud->manager_)
        return resultsFrameReject(d,0x1c2124,"Results actual lexical Camera/manager observation absent");
    original_position_stats_adapter50LA::Owner observer;
    original_position_stats_adapter50LA::Owner::Diagnostic observed;
    if(!observer.bind(returned,*hud->manager_,observed))
        return resultsFrameReject(d,0x1c17a0,"Results actual observer bind unavailable");
    if(!p.rendererLease&&!retainConsumedResultsRenderer50LA(d))return false;
    if(!p.rendererLease->identityCurrent())return resultsFrameReject(d,0x1c17a0,"Results native renderer custody changed");
    rr::platform::VehicleWindowDiagnostic50DS renderer;std::int32_t actualV0=0;
    const auto service=rr::game::serviceOriginalRenderer244D60(display->dispatch_,false,display->renderer_,renderer,actualV0);
    if(service.status!=rr::game::OriginalCameraStartStatus::complete)
        return resultsFrameReject(d,service.sourcePC,"Results native renderer20(0) incomplete");
    const auto cameraPublication=returned.publicationCount();
    // Native preparation pass before the source-ordered slot48 capture. It
    // consumes actual canonical texture objects per source occurrence; never
    // a blanket preload or a substitute last-camera/mode3 receipt.
    if(!source.demandNativeFinalCamera1C15B0(*this,*race,returned,observer,d))return false;
    const auto prefix=source.captureNativeSlot48Cpu1C17A0(*this,*race,returned,observer,d);
    if(prefix==rr::game::OriginalResults10450LA::Slot48CpuPrefix::resetBranch)return true;
    if(prefix!=rr::game::OriginalResults10450LA::Slot48CpuPrefix::sharedBaseCpu||!p.nativeDisplay||
       !p.nativeDisplay->boundedSharedReturned||returned.publicationCount()!=cameraPublication)
        return false;
    p.nativeDisplay->acceptedCamera=&returned;
    p.nativeDisplay->acceptedTiming=nativeTaRootTiming_;
    p.nativeDisplay->acceptedCameraPublication=cameraPublication;p.nativeDisplay->acceptedDisplayRevision=revision;
    p.nativeDisplay->acceptedTextureGate=entitiesOwner_->screenQuadEnabled383678_!=0;
    p.nativeDisplay->acceptedTextGate=entitiesOwner_->screenTextEnabled383688_!=0;
    p.nativeDisplay->acceptedPolygonGate=entitiesOwner_->apertureRenderEnabled383680_!=0;
    return true;
}
bool Application::nativeResultsFinalCameraCurrent50LA(const rr::game::OriginalRaceConstructor50LA& race,
    const original_camera_session50KX::Session& camera)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||race.resultsFeedbackThread_!=std::this_thread::get_id())return false;
    if(sourceRace28COwner_.get()!=&race||race.application_!=entitiesOwner_||!race.counted_||race.rolledBack_||
       !race.ownsNativeCameraAdapter34(camera)||nativeTaRootRace_!=&race||nativeTaRootEntered_||nativeTaRootAborted_||
       nativeTaRootGeneration_!=race.entity_.sessionGeneration||sourceApplicationMode22C_!=0||sourceUpdateRenderBypass228Bit3_||
       camera.actual_.humanCount34!=1||camera.actual_.applicationEntities!=entitiesOwner_||
       camera.generation_!=race.entity_.sessionGeneration||!camera.nativeTaIssuance_||
       camera.nativeTaIssuance_->issuer!=this||camera.nativeTaIssuance_->generation!=camera.generation_||
       (race.world74_&&camera.nativeTaIssuance_->course!=rr::game::OriginalCameraStartCourse::ra)||
       camera.frameStage_!=original_camera_session50KX::Session::FrameStage::published||
       !camera.frameNumber_||camera.completedFrame_!=camera.frameNumber_||camera.pendingDisplayWork())return false;
    const auto display=raceDisplayOwner50LA_.lock();
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    return display&&display==race.loadDisplay_&&display->application_==this&&display->video_==videoOwner_&&
        display->generation_==raceDisplayGeneration50LA_&&display->gameplayDisplayIdentityCurrent50LA()&&
        display->readGameplayViewport115990(camera,rectangle,revision)&&
        rectangle==std::array<std::int32_t,4>{{0,0,640,448}};
}
bool Application::nativeResultsFrameCurrent50LA(const NativeResultsFrame50LA& batch,
    const rr::platform::VehicleWindowGl50DS& renderer,const rr::platform::VehicleWindowFrame50DS& frame)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!batch.impl_)return false;
    const auto& n=*batch.impl_;
    if(n.thread!=std::this_thread::get_id()||n.issuer!=this||!n.sourcePinned||!n.tablePinned||!n.table||
       !n.race||sourceRace28COwner_!=n.race||n.race->world74_!=n.world||
       !n.source||n.race->results104_.get()!=n.source||!n.source->impl_||
       n.race->application_!=entitiesOwner_||!n.race->counted_||n.race->rolledBack_||
       !n.camera||!n.race->ownsNativeCameraAdapter34(*n.camera)||!n.display||
       n.race->loadDisplay_!=n.display||raceDisplayOwner50LA_.lock()!=n.display||
       raceDisplayGeneration50LA_!=n.displayGeneration||n.display->generation_!=n.displayGeneration||
       n.display->application_!=this||n.display->video_!=videoOwner_||&n.display->renderer_!=&renderer||
       n.display->retiring_||!n.lease||!n.lease->identityCurrent()||n.frame!=&frame||
       frame.nativeResults!=&batch||frame.width!=n.width||frame.height!=n.height||
       nativeTaRootEntered_||nativeTaRootAborted_||sourceApplicationMode22C_!=0||sourceUpdateRenderBypass228Bit3_||
       nativeTaRootRace_!=n.race.get()||nativeTaRootPublication_!=n.rootPublication||
       nativeTaRootGeneration_!=n.race->entity_.sessionGeneration||
       n.camera->publicationCount()!=n.cameraPublication||!resultsMatrixCurrent(*n.camera,frame))return false;
    const auto& p=*n.source->impl_;
    original_ui_source_timing50LH::Sample pending;std::uint64_t pendingSerial=0;
    const auto* pause=n.race->pause100_?n.race->pause100_->entity_.get():nullptr;
    if(p.retirementStarted||p.nativeFrameBorrows!=1||p.rendererLease!=n.lease||
       !n.source->retirementCurrent()||!(p.component.flags29&1u)||
       (pause&&(pause->component.flags29&1u))||p.nativeDisplay.get()!=n.capture||
       !n.capture||!n.capture->boundedSharedReturned||n.capture->building||
       !n.capture->acceptedTiming.sameOwner(nativeTaRootTiming_)||
       n.capture->acceptedTiming.readPrepared(pending,pendingSerial)||
       n.capture->sharedRootPublication!=n.rootPublication||n.capture->acceptedCamera!=n.camera.get()||
       n.capture->acceptedCameraPublication!=n.cameraPublication||!p.context||!p.context->live()||
       n.capture->acceptedTextureGate!=(entitiesOwner_->screenQuadEnabled383678_!=0)||
       n.capture->acceptedTextGate!=(entitiesOwner_->screenTextEnabled383688_!=0)||
       n.capture->acceptedPolygonGate!=(entitiesOwner_->apertureRenderEnabled383680_!=0)||
       p.domain!=standardMaterialDomainOwner_||!n.table->coldCollectorCurrent(n.capture->receipt))return false;
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    return n.display->readGameplayViewport115990(*n.camera,rectangle,revision)&&revision==n.selectionRevision&&
        rectangle==std::array<std::int32_t,4>{{0,0,640,448}};
}
bool Application::prepareNativeResultsFrame50LA(const original_camera_session50KX::Session& expected,
    rr::platform::VehicleWindowFrame50DS& frame,std::unique_ptr<NativeResultsFrame50LA>& output,ResultsD& d)const noexcept {
    d={};
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return resultsFrameReject(d,0x1a86bc,"Results Frame Application thread");
    if(output||frame.nativeResults)return resultsFrameReject(d,0x1a86bc,"Results Frame already borrowed");
    const auto race=sourceRace28COwner_;
    if(!race||!race->results104_||!race->results104_->impl_){d.operation="Results Frame: no Results owner";return true;}
    auto& source=*race->results104_;auto& p=*source.impl_;
    if(race->resultsFeedbackThread_!=std::this_thread::get_id()||p.nativeCaptureThread!=std::this_thread::get_id())
        return resultsFrameReject(d,0x1a86bc,"Results Frame source thread");
    if(!(p.component.flags29&1u)){if(!p.nativeFrameBorrows)p.nativeDisplay.reset();d.operation="Results Frame: entity inactive";return true;}
    if(!p.nativeDisplay||!p.nativeDisplay->acceptedCamera){d.operation="Results Frame: no accepted capture";return true;} //No accepted contribution, not source success.
    if(nativeTaRootEntered_||nativeTaRootAborted_||p.nativeDisplay->sharedRootPublication!=nativeTaRootPublication_){
        if(!p.nativeFrameBorrows)p.nativeDisplay.reset();
        d.operation="Results Frame: root publication changed";
        return true; //An early/failed/suspended newer Root never reuses an older accepted image.
    }
    const auto& capture=*p.nativeDisplay;
    original_ui_source_timing50LH::Sample pending;std::uint64_t pendingSerial=0;
    if(!capture.acceptedTiming.sameOwner(nativeTaRootTiming_)||capture.acceptedTiming.readPrepared(pending,pendingSerial))
        return resultsFrameReject(d,0x1a86bc,"Results captured timing has not committed or its actual owner expired");
    if(p.retirementStarted||p.nativeFrameBorrows||!source.retirementCurrent()||!(p.component.flags29&1u)||
       capture.acceptedCamera!=&expected||!capture.boundedSharedReturned||capture.building||
       nativeTaRootEntered_||nativeTaRootAborted_||sourceApplicationMode22C_!=0||sourceUpdateRenderBypass228Bit3_||
       nativeTaRootRace_!=race.get()||capture.sharedRootPublication!=nativeTaRootPublication_||
       capture.acceptedCameraPublication!=expected.publicationCount()||!resultsMatrixCurrent(expected,frame))
        return resultsFrameReject(d,0x1a86bc,"Results retained capture is not this accepted Camera/Root/Frame");
    if(!resultsPublishedRoot50LA(*race,d,false))return false;
    const auto display=raceDisplayOwner50LA_.lock();
    if(!display||display!=race->loadDisplay_||display->application_!=this||display->video_!=videoOwner_||
       !display->gameplayDisplayIdentityCurrent50LA()||!p.rendererLease||!p.rendererLease->identityCurrent())
        return resultsFrameReject(d,0x1a86bc,"Results Frame retained Display/renderer lease changed");
    const auto* pause=race->pause100_?race->pause100_->entity_.get():nullptr;
    if(pause&&(pause->component.flags29&1u))
        return resultsFrameReject(d,0x1a8690,"Results cannot skip active preceding Pause draw");
    try {
        const auto world=race->world74_;ra_cold_aggregate50LA::Diagnostic wd;
        // RA borrows its world join's Session; the common race uses the
        // race's own Camera34 (lifetime held by the loop around this frame).
        std::shared_ptr<const original_camera_session50KX::Session> camera;
        if(world)camera=world->borrowRuntimeSourceCamera(world,wd);
        else if(race->cameraNativeAdapter34_==&expected)
            camera=std::shared_ptr<const original_camera_session50KX::Session>(std::shared_ptr<void>{},&expected);
        std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
        if(!camera||camera.get()!=&expected||!race->ownsNativeCameraAdapter34(*camera)||
           camera->actual_.humanCount34!=1||!camera->nativeTaIssuance_||camera->nativeTaIssuance_->issuer!=this||
           (world&&camera->nativeTaIssuance_->course!=rr::game::OriginalCameraStartCourse::ra)||
           camera->generation_!=race->entity_.sessionGeneration||camera->actual_.applicationEntities!=entitiesOwner_||
           camera->frameStage_!=original_camera_session50KX::Session::FrameStage::published||camera->pendingDisplayWork()||
           !display->readGameplayViewport115990(*camera,rectangle,revision)||revision!=capture.acceptedDisplayRevision||
           rectangle!=std::array<std::int32_t,4>{{0,0,640,448}})
            return resultsFrameReject(d,0x115990,"Results current actual final native Camera/viewport absent");
        auto batch=std::unique_ptr<NativeResultsFrame50LA>(new NativeResultsFrame50LA);auto& n=*batch->impl_;
        n.issuer=this;n.race=race;n.world=world;n.camera=camera;n.display=display;n.source=&source;
        n.lease=p.rendererLease;n.capture=&capture;n.rootPublication=capture.sharedRootPublication;
        n.cameraPublication=camera->publicationCount();n.selectionRevision=revision;n.displayGeneration=display->generation_;
        n.frame=&frame;n.width=frame.width;n.height=frame.height;n.table=p.domain->materials382B38();
        if(!n.table||!source.pinNativeFrameCollector50LA(*n.table,capture.receipt))
            return resultsFrameReject(d,0x246760,"Results captured material registry no longer current");
        n.tablePinned=true;++p.nativeFrameBorrows;n.sourcePinned=true;
        rr::platform::VehicleWindowDiagnostic50DS renderer;
        if(!display->renderer_.prepareNativeResultsResources50LA(source,renderer)||
           !display->renderer_.prepareNativeResultsDrawResources50LA(renderer))
            return resultsFrameReject(d,0x246760,"Results actual copied-image/geometry/scratch preparation failed");
        frame.nativeResults=batch.get();
        if(!nativeResultsFrameCurrent50LA(*batch,display->renderer_,frame))
            return resultsFrameReject(d,0x1a86bc,"Results Frame owner changed during preparation");
        output=std::move(batch);return true;
    }catch(...){return resultsFrameReject(d,0x1a86bc,"Results Frame native ownership allocation failed");}
}
}
