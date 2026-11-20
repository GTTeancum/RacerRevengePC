#pragma once
#include "original_pause_entity50la.h"
#include "../../platform/linux/native/original_spectator_menu_input50la.h"
#include "original_race_pause_parent50la.h"
#include "original_race_pause_forward50la.h"
#include "original_camera_session50kx.h"
#include "ra_cold_sources50la.h"
#include "../../src/game/original_pause_storage50la.h"
#include "../../src/game/original_pause_layout50la.h"
#include "../../src/game/original_pause_native_draw50la.h"
#include "../../src/game/original_pause_viewport_scope50la.h"

namespace original_pause_observation50LA {
// Serial actual-owner observation and bounded navigation. Application must outlive this borrow; caller unbinds
// before Application destruction. All concrete/profile ownership is weak.
// This facet cannot supply arbitrary frames, selection, controller or settings.
class Owner final {
    friend struct ::original_main_entry50KG::Application;
    using Camera=original_camera_session50KX::Session;
    using Track=ra_cold_sources50LA::DecodedSources;
    // Exact consumed Race retained weakly alongside the world-owned Session.
    // No extra source count or lifetime pin; each use revalidates custody.
    std::weak_ptr<rr::game::OriginalRaceConstructor50LA> race_;
    std::weak_ptr<Camera> camera_;
    std::weak_ptr<const Track> track_;
    std::uint64_t cameraGeneration_=0;
    mutable bool lifecycleRejected_=false;
    // Every-track human race route: no consumed RA race; see bindHost.
    bool host_=false;std::size_t hostTrackText_=0;
    // Registry (add-on) course: the row's track name; empty = the LOC title above.
    std::string hostTrackName_;
    const original_main_entry50KG::Application* application_=nullptr;
    std::weak_ptr<rr::game::OriginalPauseStorage50LA> pause_;
    std::weak_ptr<rr::game::ControlPresets49D> controls_;
    std::weak_ptr<rr::game::OriginalUiGameSettingsProfile50KG> settings_;
public:
    using Diagnostic=rr::game::OriginalPauseStorage50LA::Diagnostic;
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool bind(const original_main_entry50KG::Application& application,
        const std::shared_ptr<rr::game::OriginalPauseStorage50LA>& pause)noexcept {
        if(application_||!pause||pause->stage_!=rr::game::OriginalPauseStorage50LA::Stage::derivedResources||
           !pause->context_||pause->context_->materialApplication()!=application.materialApplication||
           !application.controlsProfile||!application.gameSettingsProfile||
           !rr::game::validOriginalUiGameSettingsProfile50KG(*application.gameSettingsProfile))return false;
        application_=&application;pause_=pause;controls_=application.controlsProfile;
        settings_=application.gameSettingsProfile;return true;
    }
    bool captureSettings(Diagnostic& d)const noexcept {
        d={};if(!application_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();const auto settings=settings_.lock();
        if(!pause||!controls||!settings||!pause->context_||
           pause->context_->materialApplication()!=application_->materialApplication||
           application_->controlsProfile!=controls||
           application_->gameSettingsProfile!=settings)return false;
        return pause->refreshSettings1BB590(*controls,*settings,d);
    }
    // RA-only current native source join. The shared source is the aggregate's
    // actual loaded-track borrow, not SessionAssets or a host track label.
    bool bindRace(const std::shared_ptr<Camera>& camera,
        const std::shared_ptr<const Track>& track)noexcept {
        if(cameraGeneration_||!application_||!camera||!track)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        if(!pause||!controls||!raceIdentity(*camera,*track,*pause,*controls))return false;
        race_=application_->sourceRace28COwner_;
        camera_=camera;track_=track;cameraGeneration_=camera->generation_;return true;
    }
    bool captureTitle(Diagnostic& d)const noexcept {
        d={};if(!application_||!cameraGeneration_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto camera=camera_.lock();const auto track=track_.lock();
        if(!pause||!controls||!camera||!track||camera->generation_!=cameraGeneration_||
           application_->controlsProfile!=controls||!raceIdentity(*camera,*track,*pause,*controls))return false;
        // Original descriptor17EA64/6C: trackID7's1127C slot is localization108.
        // The camera is proven to use this SAME RA map and selected metadata.
        return pause->refreshTitle1BB010(camera->raceFlags1B0_,108,d);
    }
    // Inactive CPU preparation only. Caller must first obtain actual parent
    // and last-camera admission; this does not dispatch1B9220 or a GPU draw.
    bool captureLayout(rr::game::OriginalPauseLayout50LA::Commands& out,
        Diagnostic& observation,rr::game::OriginalPauseLayout50LA::Diagnostic& layout)const noexcept {
        out={};observation={};layout={};
        if(!application_||!cameraGeneration_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto settings=settings_.lock();const auto camera=camera_.lock();const auto track=track_.lock();
        if(!pause||!controls||!settings||!camera||!track||camera->generation_!=cameraGeneration_||
           application_->controlsProfile!=controls||application_->gameSettingsProfile!=settings||
           !raceIdentity(*camera,*track,*pause,*controls))return false;
        // Source gating precedes title/settings observation. Existing standalone
        // captureTitle/captureSettings remain observations, not callback calls.
        if(!rr::game::OriginalPauseLayout50LA::visible(*pause))
            return rr::game::OriginalPauseLayout50LA::prepare1BB010(*pause,out,layout);
        if(!pause->refreshTitle1BB010(camera->raceFlags1B0_,108,observation)||
           !pause->refreshSettings1BB590(*controls,*settings,observation))return false;
        return rr::game::OriginalPauseLayout50LA::prepare1BB010(*pause,out,layout);
    }
    // Inactive outer one-view render preparation. Actual shared profile1125C
    // determines count; the ALL8 manager human count is unrelated.
    bool captureOuterLayout(rr::game::OriginalPauseLayout50LA::Commands& out,
        Diagnostic& observation,rr::game::OriginalPauseLayout50LA::Diagnostic& layout)const noexcept {
        out={};observation={};layout={};
        if(!application_||!cameraGeneration_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto camera=camera_.lock();const auto track=track_.lock();
        const auto profile=application_->profile;
        if(!pause||!controls||!camera||!track||!profile||camera->generation_!=cameraGeneration_||
           application_->controlsProfile!=controls||!raceIdentity(*camera,*track,*pause,*controls))return false;
        const auto count=(profile->playerBits1125C>>5)&3u;
        if(count>=2||pause->mode1F0_!=0){layout.sourcePC=count>=2?0x1B9DB0:0x1B9C24;return false;}
        rr::game::OriginalPauseLayout50LA::Commands derived;
        if(count&&!captureLayout(derived,observation,layout))return false;
        return rr::game::OriginalPauseLayout50LA::prepareOuterTail1B9D84(
            *pause,derived,profile->playerBits1125C,out,layout);
    }

    // Native draw-tail preparation under the actual retained camera's115A60
    // scope. This is not last-camera admission, a timer tick or full1B9220.
    bool captureOuterNativeFrame(rr::game::OriginalPauseNativeDraw50LA& native,
        Diagnostic& observation,rr::game::OriginalPauseLayout50LA::Diagnostic& layout,
        rr::game::OriginalPauseNativeResources50LA::Diagnostic& draw)const noexcept {
        observation={};layout={};draw={};
        if(native.frame_)native.frame_->prepared=false;
        if(!application_||!cameraGeneration_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto camera=camera_.lock();const auto track=track_.lock();const auto profile=application_->profile;
        if(!pause||!controls||!camera||!track||!profile||native.owner_!=pause||!native.current()||
           camera->generation_!=cameraGeneration_||!camera->publicationCount_||
           application_->controlsProfile!=controls||!raceIdentity(*camera,*track,*pause,*controls))return false;
        if(pause->mode1F0_!=0){layout.sourcePC=0x1B9C24;return false;}
        const auto count=(profile->playerBits1125C>>5)&3u;
        rr::game::OriginalPauseViewportScope50LA viewport(camera->viewport_,count>=2);
        if(!viewport.valid_){layout.sourcePC=0x115A60;return false;}
        rr::game::OriginalPauseLayout50LA::Commands derived,outer;
        if(count&&!captureLayout(derived,observation,layout))return false;
        if(!rr::game::OriginalPauseLayout50LA::prepareOuterTail1B9D84(
            *pause,derived,profile->playerBits1125C,outer,layout))return false;
        // Private concrete join can consume a multiview-tagged command list;
        // public prepareOuter cannot bypass this real viewport mutation/restore.
        return native.prepareCommands(outer,camera->viewport_,camera->publicationCount_,draw);
    }

    struct NavigationResult {
        enum class Status : std::uint8_t {complete,owner,parent,actionUnavailable,audioUnavailable,childRoute};
        Status status=Status::owner;
        std::uint32_t sourcePC=0,originalReturn=0;
        bool selectionChanged=false,settingsChanged=false;
        original_camera_cycle_audio50KX::Owner::Result audio=
            original_camera_cycle_audio50KX::Owner::Result::unprepared;
    };
    // Bounded actual1BD7C0 navigation, HUD rows2..4 and audio rows5..8.
    // Parent admission remains mandatory; no host selection/profile setter.
    // Confirm uses separate actual row0 and retained row9/10 paths; row1 remains unjoined.
    NavigationResult pressNavigation1BD7C0(rr::game::InputEvent48Z& event,
        rr::game::DeviceButtonRecord48Y& record)const noexcept {
        using Result=NavigationResult;using Status=Result::Status;
        Result result;
        if(lifecycleRejected_||!application_||!cameraGeneration_)return result;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto camera=camera_.lock();const auto track=track_.lock();
        if(!pause||!controls||!camera||!track||camera->generation_!=cameraGeneration_||
           application_->controlsProfile!=controls||!raceIdentity(*camera,*track,*pause,*controls)||
           !pause->entity_||!pause->entity_->initialized||pause->entity_->releasing||
           pause->entity_->application!=application_->entities()||
           pause->entity_->component.sessionGeneration!=cameraGeneration_)return result;
        const auto& component=pause->entity_->component;
        // Cannot activate the current detached cold prefix. Only an actual
        // future race sibling-chain attachment can supply the exact App10 parent,
        // source1402 mask and enabled flag (1A7818, then17D6A0).
        if(component.parent1C!=pause->entity_->application->application10()||
           component.mask20!=0x1402u||!(component.flags29&1u)||(component.flags29&8u)||
           component.firstChild18||component.descendants24){
            result.status=Status::parent;result.sourcePC=0x142350;return result;
        }
        if(!pause->mode1F0_&&event.deviceId==pause->controller884_&&event.category==2u){
            //1277D0 compares code/category. Its context query is category0
            //only; the source0x80000000 argument does not filter category2.
            if(event.code==12u||event.code==14u){
                if(pause->selection880_>=11)return result;
                if(event.code==12u)pause->selection880_=pause->selection880_?pause->selection880_-1u:10u;
                else pause->selection880_=pause->selection880_==10u?0u:pause->selection880_+1u;
                result.selectionChanged=true;
                result.sourcePC=event.code==12u?0x1BD844:0x1BD8A0;
                // Source mutation precedes audio. On native rejection retain
                // the mutation and report partial progress; do not retry it.
                result.audio=camera->actual_.cycleAudio.dispatchPauseFeedback(event.code==12u?3u:4u);
                using A=original_camera_cycle_audio50KX::Owner::Result;
                if(result.audio!=A::started&&result.audio!=A::unavailable&&result.audio!=A::muted){
                    lifecycleRejected_=true;result.status=Status::audioUnavailable;return result;
                }
            }else if(event.code==15u||event.code==13u){
                const auto row=pause->selection880_;const bool right=event.code==13u;
                if(row>=11)return result;
                // Actual jump tables3ACBC0/3ACB90 ignore rows0/1/9/10.
                if(row>=5&&row<=8){
                    const auto settings=settings_.lock();
                    const auto& race=application_->sourceRace28COwner_;
                    constexpr std::uint32_t leftPC[]={0x1BDEB8,0x1BE004,0x1BE0B4,0x1BE164};
                    constexpr std::uint32_t rightPC[]={0x1BE4B0,0x1BE600,0x1BE6B4,0x1BE768};
                    result.sourcePC=right?rightPC[row-5]:leftPC[row-5];
                    if(!attachedLifecycle(*camera,*pause)||!pause->active200_||!settings||
                       application_->gameSettingsProfile!=settings||!application_->riderProfile||
                       !race||race->profile_!=application_->riderProfile||
                       application_->raceConstructorColdOwner50LA_.lock()!=race)return result;
                    if(!race->adjustPauseAudio1BD7C0(*pause,*settings,application_->animationClock(),
                        row-5,right,result.settingsChanged)){
                        result.status=Status::actionUnavailable;return result;
                    }
                    // Actual profile/helper and ordered group setters precede
                    // feedback. At a bound neither store is reached: badcall6.
                    constexpr std::uint32_t leftChanged[]={0x1BDFDC,0x1BE08C,0x1BE13C,0x1BE1DC};
                    constexpr std::uint32_t rightChanged[]={0x1BE5D8,0x1BE68C,0x1BE740,0x1BE7E4};
                    constexpr std::uint32_t leftLimit[]={0x1BDFF4,0x1BE0A4,0x1BE154,0x1BE1F4};
                    constexpr std::uint32_t rightLimit[]={0x1BE5F0,0x1BE6A4,0x1BE758,0x1BE7FC};
                    result.sourcePC=result.settingsChanged?(right?rightChanged[row-5]:leftChanged[row-5]):
                        (right?rightLimit[row-5]:leftLimit[row-5]);
                    result.audio=camera->actual_.cycleAudio.dispatchPauseVolumeFeedback(
                        result.settingsChanged?(right?4u:3u):6u);
                    using A=original_camera_cycle_audio50KX::Owner::Result;
                    if(result.audio!=A::started&&result.audio!=A::unavailable&&result.audio!=A::muted){
                        lifecycleRejected_=true;result.status=Status::audioUnavailable;return result;
                    }
                }
                if(row>=2&&row<=4){
                    if(!attachedLifecycle(*camera,*pause)||!pause->active200_||
                       pause->controller884_>=rr::game::controlPresetSlots49D)return result;
                    auto& slot=controls->slots[pause->controller884_];
                    if((row==2&&slot.field10F94>7)||(row==3&&slot.field10F9C>7))return result;
                    std::uint32_t subtype=0;
                    if(row==3){
                        const auto profile=application_->profile;
                        const auto owner=application_->riderProfile;
                        const auto& race=application_->sourceRace28COwner_;
                        if(!profile||!profile->known||!owner||!race||race->profile_!=owner||
                           owner->commonProfile()!=profile.get())return result;
                        // The consumed ALL8 Course route has an actual count1
                        // profile even though its native humanCount34 is0.
                        // Other counts reach17FCD0's real player+40 lookup;
                        // no replacement for that unjoined owner is invented.
                        if(((profile->playerBits1125C>>5)&3u)!=1u){
                            result.status=Status::actionUnavailable;result.sourcePC=right?0x1BE2F8:0x1BDD00;return result;}
                        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection current;
                        if(!owner->courseRaceSnapshot(current)||current.subtype!=camera->actual_.launchKind11258)return result;
                        subtype=current.subtype;
                    }
                    if(row==4){
                        slot.field10FA4=slot.field10FA4?0u:1u; //1BDE98 /1BE490 whole-word store.
                    }else if(row==3&&subtype==3){
                        slot.field10F9C=slot.field10F9C?0u:6u; //1BDD4C/54 and1BE344/34C.
                    }else{
                        auto& value=row==2?slot.field10F94:slot.field10F9C;
                        value+=right?1u:0xffffffffu; //Source store precedes signed wrap check.
                        if(right){if(value>=8)value=0;}
                        else if(std::bit_cast<std::int32_t>(value)<0)value=7;
                    }
                    result.settingsChanged=true;
                    result.sourcePC=row==2?(right?0x1BE2C4:0x1BDCCC):
                        row==3?(right?0x1BE448:0x1BDE50):(right?0x1BE4A0:0x1BDEA8);
                    // Preserve the actual profile write if feedback rejects.
                    // The terminal latch prevents replaying a partial callback.
                    result.audio=camera->actual_.cycleAudio.dispatchPauseFeedback(right?4u:3u);
                    using A=original_camera_cycle_audio50KX::Owner::Result;
                    if(result.audio!=A::started&&result.audio!=A::unavailable&&result.audio!=A::muted){
                        lifecycleRejected_=true;result.status=Status::audioUnavailable;return result;
                    }
                }
            }else if(event.code==2u){
                result.status=Status::actionUnavailable;result.sourcePC=0x1BD8BC;return result;
            }
        }
        // This concrete owner has no Entity children. Use the genuine actual
        // descendant mask rather than inventing a successful child callback.
        const rr::game::EventChildRouter48Z children{component.descendants24,
            rr::game::noEventChild48Z,nullptr,0};
        const auto routed=rr::game::routeChildInputEvent48Z(children,true,event,record,nullptr,nullptr,0);
        result.status=routed.status==rr::game::EventRouteStatus48Z::Complete?Status::complete:Status::childRoute;
        result.sourcePC=0x1BE80C;result.originalReturn=routed.handled?1u:0u;return result;
    }
    // Every-track human race route. The original Pause storage, layout and
    // native draw are driven from the host race session instead of a consumed
    // RA race entity: the host session owns world/clock pause, and row actions
    // (resume, restart, HUD/audio settings, main menu, quit) map to its owners.
    // trackTextId is the original track-ID title (17EA08 order).
    bool bindHost(const original_main_entry50KG::Application& application,
        const std::shared_ptr<rr::game::OriginalPauseStorage50LA>& pause,
        const std::shared_ptr<Camera>& camera,std::size_t trackTextId)noexcept {
        if(host_||cameraGeneration_||!camera||!camera->generation_||!bind(application,pause))return false;
        camera_=camera;cameraGeneration_=camera->generation_;hostTrackText_=trackTextId;host_=true;return true;
    }
    // Registry (add-on) course, 146r (PORT EXTENSION, NOT ORIGINAL): the title
    // is the row's name (rr::game::dlc50DL::AddonCourseLoad::name), never a
    // stock course's LOC title. Empty name = refused (no owner).
    bool bindHostNamed(const original_main_entry50KG::Application& application,
        const std::shared_ptr<rr::game::OriginalPauseStorage50LA>& pause,
        const std::shared_ptr<Camera>& camera,const std::string& trackName)noexcept {
        if(trackName.empty()||!bindHost(application,pause,camera,0))return false;
        try{hostTrackName_=trackName;}catch(...){unbind();return false;}
        return true;
    }
    bool hostBound()const noexcept{return host_;}
    bool hostOpen()const noexcept{
        const auto pause=pause_.lock();return host_&&pause&&pause->active200_;
    }
    std::uint32_t hostSelection()const noexcept{
        const auto pause=pause_.lock();return pause?pause->selection880_:0u;
    }
    bool openHost()const noexcept {
        const auto pause=pause_.lock();const auto camera=camera_.lock();
        if(!host_||!pause||!camera||camera->generation_!=cameraGeneration_||pause->active200_||
           pause->mode1F0_||!pause->entity_)return false;
        pause->selection880_=0;pause->opening30_=.25f; //1BE8C8 opening slide.
        camera->actual_.cycleAudio.dispatchPauseFeedback(2);
        if(!pause->entity_->setActivity141630(true))return false;
        pause->active200_=1;pause->controller884_=0; //The one human's controls slot.
        return true;
    }
    bool closeHost()const noexcept {
        const auto pause=pause_.lock();
        if(!host_||!pause||!pause->active200_||!pause->entity_)return false;
        pause->active200_=0;
        return pause->entity_->setActivity141630(false);
    }
    // 1BD7C0 vertical navigation: rows 0..10 wrap, same feedback cues.
    bool moveHostSelection(bool down)const noexcept {
        const auto pause=pause_.lock();const auto camera=camera_.lock();
        if(!host_||!pause||!camera||!pause->active200_||pause->selection880_>=11)return false;
        if(down)pause->selection880_=pause->selection880_==10u?0u:pause->selection880_+1u;
        else pause->selection880_=pause->selection880_?pause->selection880_-1u:10u;
        camera->actual_.cycleAudio.dispatchPauseFeedback(down?4u:3u);
        return true;
    }
    void hostSettingFeedback(bool changed,bool right)const noexcept {
        if(const auto camera=camera_.lock())
            camera->actual_.cycleAudio.dispatchPauseFeedback(changed?(right?4u:3u):6u);
    }
    // One retained camera view: resources, opening/arrow geometry, title and
    // settings refresh, layout and native draw commands (1B9324/1BB010/1B9D84).
    bool prepareHostPresentation(rr::game::OriginalPauseNativeDraw50LA& native,Diagnostic& observation,
        rr::game::OriginalPauseLayout50LA::Diagnostic& layout,
        rr::game::OriginalPauseNativeResources50LA::Diagnostic& draw)const noexcept {
        observation={};layout={};draw={};if(native.frame_)native.frame_->prepared=false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto settings=settings_.lock();const auto camera=camera_.lock();
        const auto profile=application_?application_->profile:nullptr;
        if(!host_||!application_||!pause||!controls||!settings||!camera||!profile||native.owner_!=pause||
           !native.current()||camera->generation_!=cameraGeneration_||!camera->publicationCount_||
           !pause->active200_||application_->controlsProfile!=controls||
           application_->gameSettingsProfile!=settings)return false;
        if(!application_->beginUpdateClock140AC0())return false;
        const auto views=camera->display_.cameraCount264?camera->display_.cameraCount264:1u;
        bool complete=pause->demandResources1BAE00(views-1u,views,observation)&&
            pause->advanceGeometry1B9324(application_->animationClock(),observation);
        if(complete&&pause->mode1F0_!=0){layout.sourcePC=0x1B9C24;complete=false;}
        if(complete){
            const auto count=(profile->playerBits1125C>>5)&3u;
            rr::game::OriginalPauseViewportScope50LA viewport(camera->viewport_,count>=2);
            if(!viewport.valid_){layout.sourcePC=0x115A60;complete=false;}
            rr::game::OriginalPauseLayout50LA::Commands derived,outer;
            if(complete&&count){
                if(rr::game::OriginalPauseLayout50LA::visible(*pause))
                    complete=(hostTrackName_.empty()?
                        pause->refreshTitle1BB010(camera->raceFlags1B0_,hostTrackText_,observation):
                        pause->refreshTitleText1BB010(camera->raceFlags1B0_,hostTrackName_,observation))&&
                        pause->refreshSettings1BB590(*controls,*settings,observation);
                complete=complete&&rr::game::OriginalPauseLayout50LA::prepare1BB010(*pause,derived,layout);
            }
            complete=complete&&rr::game::OriginalPauseLayout50LA::prepareOuterTail1B9D84(
                *pause,derived,profile->playerBits1125C,outer,layout)&&
                native.prepareCommands(outer,camera->viewport_,camera->publicationCount_,draw);
        }
        const bool committed=application_->commitUpdateClock140D30();
        return complete&&committed;
    }
    // Explicit native spectator opening policy; the original human-device
    // acceptance1A87A0 is unavailable with the actual ALL8 humanCount34==0.
    bool openNativeAll8SpectatorMenu(const rr::game::InputEvent48Z&,Diagnostic&)const noexcept;
    NavigationResult continueRow0(rr::game::InputEvent48Z&,rr::game::DeviceButtonRecord48Y&)const noexcept;
    bool nativeSpectatorMenuOpen()const noexcept;
    // Live caller only: consumes the actual producer-owned physical batch
    // once, preserving event/record identity and per-edge callback order.
    bool dispatchNativeInputBatch(rr::platform::OriginalSpectatorMenuInput50LA&,
        rr::platform::OriginalVideoBlockingNativeBackend*,Diagnostic&)const noexcept;
    bool closeNativeSpectatorMenuForShutdown(Diagnostic&)const noexcept;
    bool prepareNativePresentation(rr::game::OriginalPauseNativeDraw50LA&,Diagnostic&,
        rr::game::OriginalPauseLayout50LA::Diagnostic&,
        rr::game::OriginalPauseNativeResources50LA::Diagnostic&)const noexcept;
    bool resetNativePresentationClock()const noexcept;
    void unbind()noexcept {
        host_=false;hostTrackText_=0;hostTrackName_.clear();
        race_.reset();camera_.reset();track_.reset();cameraGeneration_=0;lifecycleRejected_=false;
        application_=nullptr;pause_.reset();controls_.reset();settings_.reset();
    }
private:
    bool dispatchNativeInputRemainder(rr::platform::OriginalSpectatorMenuInput50LA&,
        rr::platform::OriginalVideoBlockingNativeBackend*,Diagnostic&)const noexcept;
    bool finishNativePauseReturnInput(rr::platform::OriginalSpectatorMenuInput50LA&,Diagnostic&)const noexcept;
    bool closeLifecycle1BE830(Diagnostic&)const noexcept;
    bool retainedAttachedLifecycle(const Camera&,const rr::game::OriginalPauseStorage50LA&)const noexcept;
    bool attachedLifecycle(const Camera&,const rr::game::OriginalPauseStorage50LA&)const noexcept;
    bool raceIdentity(const Camera& c,const Track& track,
        const rr::game::OriginalPauseStorage50LA& p,const rr::game::ControlPresets49D& controls)const noexcept {
        return retainedRaceIdentity(c,track,p,controls)&&
            application_->consumedRaAll8CameraCustodyNative34Current(c);
    }
    bool retainedRaceIdentity(const Camera& c,const Track& track,
        const rr::game::OriginalPauseStorage50LA& p,const rr::game::ControlPresets49D& controls)const noexcept {
        // During bind there is no retained Race yet. Subsequent observations
        // must retain that SAME actual28C owner, not a replacement with equal
        // profile/map/generation. Lifetime comparison must remain valid after
        // a gameplay failure, without dereferencing an unavailable Bank or
        // demanding live Display work merely to retire an inactive Pause.
        if(!application_)return false;
        const auto& race=application_->sourceRace28COwner_;
        if(!race||(cameraGeneration_&&race_.lock()!=race)||race->pause100_.get()!=&p||
           !race->nativeCameraAdapter34LifetimeCurrent(c))return false;
        const auto& selection=track.freshSelection;
        return !c.closed_&&!c.abandoned_&&c.coldBound_&&c.generation_&&
            p.stage_==rr::game::OriginalPauseStorage50LA::Stage::derivedResources&&
            p.context_&&p.context_->live()&&p.context_->materialApplication()==application_->materialApplication&&
            c.actual_.applicationMaterials==p.domain_->materials382B38()&&
            &c.actual_.preferences==&controls&&c.actual_.actualRadarMapB8==&track.map&&
            selection.bound&&selection.selectedTrackId1125F==7&&selection.selectedOrder187B10==4&&
            selection.freshCourseRecordPod11280==9&&selection.actualRaceClass==rr::game::AuthoredClass49N::A&&
            c.actual_.launchKind11258==static_cast<std::uint32_t>(p.subtype11258_);
    }
};
}

#include "original_pause_callback_scheduler50la.inl"
