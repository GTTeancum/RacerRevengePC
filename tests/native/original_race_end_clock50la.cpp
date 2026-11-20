#include <cstdio>
#include "original_camera_session50kx.h"
#include "original_race_constructor50la.h"
#include "original_race_hud_scope50la.h"
#include "native_ai_session50iv_fixture.h"
#include "original_race_world_join50la.h"
#include "race_music50ie_fixture.h"
#include "source_announcer50ie_fixture.h"
#include <bit>

namespace original_camera_session50KX {
// Every counted race (every course) once its counted Camera34 and end HUD are
// both bound. An unengaged Session keeps the previous end policy below.
bool Session::canonicalCourseEndRoute()const noexcept {
    if(!nativeTaIssuance_)return false;
    return nativeTaIssuance_->issuer&&
        nativeTaIssuance_->issuer->consumedTaCamera34Engaged(*this,true);
}
rr::game::OriginalHumanEndClockResult50KG Session::advanceCanonicalHumanEndRoot()noexcept {
    using S=rr::game::OriginalHumanEndClockStatus50KG;
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return {S::callerBoundary,0x1a7da0,0};
    const auto& issued=nativeTaIssuance_;std::uint32_t pause=0;
    // The immutable issuance's issuer is dereferenced only while its existing
    // Application animation lifetime observation is still current.
    if(!issued||!canonicalCourseEndRoute()||!issued->issuer||
       !actual_.applicationAnimation.readPause11C(pause))return {S::callerBoundary,0x1a7b94,0};
    return issued->issuer->advanceConsumedSessionEndClock1A7DA0(*this);
}
Result Session::advanceCanonicalRaceFadeInPrefix()noexcept {
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return {Status::owner,0x1a7bd0};
    const auto& issued=nativeTaIssuance_;std::uint32_t pause=0;
    if(!issued||!canonicalCourseEndRoute()||!issued->issuer||
       !actual_.applicationAnimation.readPause11C(pause))return {Status::owner,0x1a7b94};
    return issued->issuer->advanceConsumedSessionFadeIn1A7BD0(*this);
}
rr::game::OriginalHumanEndClockResult50KG Session::advanceHumanEndRootWithOwners(float legacyDelta,
    race_music50IE::Owner* music,source_announcer50IE::Owner* announcer)noexcept {
    using S=rr::game::OriginalHumanEndClockStatus50KG;
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return {S::callerBoundary,0x1a7da0,0};
    if(canonicalCourseEndRoute()){
        if(!actual_.humanCount34||!coldBound_||abandoned_||pendingDisplayWork()||
           (frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published))
            return {S::callerBoundary,0x1a7da0,0};
        // No supplied delta/audio observations enter canonical source state.
        return advanceCanonicalHumanEndRoot();
    }
    // Preserve the old snapshot/call policy for unsupported/unissued routes.
    bool active=false,fading=false,speaking=false;
    if(!music||!announcer||!music->readEndingActivity(active,fading)||
       !announcer->anyActive2C4E70(speaking))return {S::callerBoundary,0x1a7e5c,0};
    auto result=advanceHumanEndRoot(legacyDelta,active,fading,speaking);
    if(result.status==S::needMusicFade1A4AB0){
        if(!music->requestEndFade1A4AB0().completed())return {S::callerBoundary,0x1a7e84,0};
        if(!music->readEndingActivity(active,fading)||!announcer->anyActive2C4E70(speaking))
            return {S::callerBoundary,0x1a7ec4,0};
        result=resumeHumanEndMusic(active,speaking);
    }
    return result;
}
}

namespace original_main_entry50KG {
rr::game::OriginalHumanEndClockResult50KG Application::advanceConsumedSessionEndClock1A7DA0(
    original_camera_session50KX::Session& session)const noexcept {
    using S=rr::game::OriginalHumanEndClockStatus50KG;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return {S::callerBoundary,0x140ce4,0};
    const auto race=sourceRace28COwner_;
    if(!enabled||!race||raceConstructorColdOwner50LA_.lock()!=race||!entitiesOwner_||
       race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_||
       !nativeTaRootEntered_||nativeTaRootAborted_||nativeTaRootRace_!=race.get()||
       sourceUpdateRenderBypass228Bit3_||sourceApplicationMode22C_!=2||!sourceApplicationElapsed23C_||
       !nativeTaRootController_||race->nativeIntroControllerSlot48()!=nativeTaRootController_||
       race->entity_.sessionGeneration!=nativeTaRootGeneration_)
        return {S::callerBoundary,0x140ce4,0};
    original_ui_source_timing50LH::Sample sample;std::uint64_t publication=0;
    if(!nativeTaRootTiming_.readPrepared(sample,publication)||publication!=nativeTaRootPublication_||
       std::bit_cast<std::uint32_t>(sample.updateSeconds)!=
       std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_))return {S::callerBoundary,0x140ce4,0};
    const auto scope=race->endHud_.lock();
    if(!scope)return {S::callerBoundary,0x1a7da0,0};
    return scope->advanceEndClock1A7DA0(*race,session,*sourceApplicationElapsed23C_,publication);
}
original_camera_session50KX::Result Application::advanceConsumedSessionFadeIn1A7BD0(
    original_camera_session50KX::Session& session)const noexcept {
    using S=original_camera_session50KX::Status;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return {S::owner,0x1a7bd0};
    const auto race=sourceRace28COwner_;
    if(!enabled||!race||raceConstructorColdOwner50LA_.lock()!=race||!entitiesOwner_||
       race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_)return {S::owner,0x1a7bd0};
    const auto scope=race->endHud_.lock();
    if(!scope)return {S::owner,0x1a7bd0};
    return scope->advanceFadeIn1A7BD0(*race,session);
}
}

namespace original_race_hud_scope50LA {
bool Registration::endStateCurrent(const rr::game::OriginalRaceConstructor50LA& race,
    const original_camera_session50KX::Session& session)const noexcept {
    if(std::this_thread::get_id()!=thread_)return false;
    // Reuse the lexical registration, not the later HUD-completion gate.
    // Each caller below borrows the actual world Camera before any mutation.
    if(!active_||!manager_||!bank_||(camera_&&camera_!=&session))return false;
    const auto retained=race_.lock();
    if(retained.get()!=&race||application_->race28C()!=&race||application_->entities()!=race.application_||
       !race.counted_||race.rolledBack_||race.entity_.sessionGeneration!=generation_||
       race.endHud_.lock().get()!=this||!race.anyWorld())return false;
    const auto& issued=session.nativeTaIssuance_;
    if(!issued||issued->issuer!=application_||issued->generation!=generation_||
       !race.raceCamera34Issued(session)||!session.coldBound_||!session.readsBound_||
       session.closed_||session.abandoned_||session.generation_!=generation_||!session.displayInputs_||
       &session.actual_.bank!=bank_||&session.actual_.all8Manager!=&manager_->manager||
       &session.actual_.race94!=&race.field94_||session.actual_.humanCount34!=1u||
       session.actual_.applicationEntities!=race.application_||
       !session.actual_.applicationAnimation.sameOwner(application_->animationClock()))return false;
    try{return session.identitiesCurrent();}catch(...){return false;}
}

rr::game::OriginalHumanEndClockResult50KG Registration::advanceEndClock1A7DA0(
    rr::game::OriginalRaceConstructor50LA& race,original_camera_session50KX::Session& session,
    float actual23C,std::uint64_t publication)noexcept {
    using S=rr::game::OriginalHumanEndClockStatus50KG;
    const auto boundary=[](std::uint32_t pc)noexcept {return rr::game::OriginalHumanEndClockResult50KG{S::callerBoundary,pc,0};};
    if(!endStateCurrent(race,session)||!publication)return boundary(0x1a7da0);
    try {
        const auto actual=race.borrowRaceCamera34();
        if(!actual||actual.get()!=&session)return boundary(0x1a7da0);
        std::uint32_t pause=0;
        if(!session.actual_.applicationAnimation.readPause11C(pause))return boundary(0x1a7b94);
        auto& cursor=session.endingClockCursor_;
        auto& progress=race.endClockProgress_;
        if(pause){
            if(progress&&progress->status!=S::advanced&&progress->status!=S::unchanged)
                return boundary(0x1a7b94); //Never discard an already suspended invocation.
            return {S::unchanged,0x1a7ba4,0}; //Original skips the scalar interval.
        }
        if(race.endClockRootPublication_!=publication){
            if(publication<race.endClockRootPublication_||(progress&&
               progress->status!=S::advanced&&progress->status!=S::unchanged))return boundary(0x1a7da0);
            // Starts one actual Root call, never a new clock on a retry. The
            // cursor's source98/94 effects precede the later owner queries.
            progress=cursor.beginSource1A7DA0(race.endClock98_,race.field94_,race.flags1B0KnownValue_,
                session.actual_.all8Manager.flags58,actual23C,true);
            race.endClockRootPublication_=publication;
            //1A7B74 saved the original Root f12 argument in f20. The
            //same canonical23C is supplied here; capture ONLY when this new
            //invocation actually reaches1A7DD0, not at a later resume/frame.
            //No caller is dispatched or end predicate overridden by this.
            if(progress->status==S::needOriginalEndDispatch1A7DD0)
                race.endEntrySourceF20_=std::bit_cast<std::uint32_t>(actual23C);
        }
        if(!progress)return boundary(0x1a7da0);
        for(;;){
            bool active=false,fading=false;
            switch(progress->status){
            case S::needMusicActivity1A4AD0:
                if(!music_||!music_->readEndingActivity(active,fading))return boundary(progress->sourcePC);
                *progress=cursor.afterMusicActivity1A4AD0(active);break;
            case S::needMusicFading1A65A0:
                if(!music_||!music_->readEndingActivity(active,fading))return boundary(progress->sourcePC);
                *progress=cursor.afterMusicFading1A65A0(fading);break;
            case S::needMusicFade1A4AB0:
                // The actual provider resumes its own pending partial fade;
                // do not restart98 or infer silence from a successful request.
                if(!music_||!music_->requestEndFade1A4AB0().completed())return boundary(progress->sourcePC);
                *progress=cursor.afterActualMusicFade1A4AB0();break;
            case S::needAnnouncerActivity2C4E70:
                if(!announcer_||!announcer_->anyActive2C4E70(active))return boundary(progress->sourcePC);
                *progress=cursor.afterAnnouncerActivity2C4E70(active);break;
            case S::needFade2AED60:{
                auto& display=*session.displayInputs_;
                const auto request=rr::game::requestOriginalHumanEndFade2AED60(display.video,
                    display.fadeManagerA8,display.fadeDuration98,progress->fadeMilliseconds);
                if(request.status!=rr::game::OriginalCameraStartStatus::complete)return boundary(request.sourcePC);
                // Source ignores legitimate busy/zero-duration returns too.
                // No failure/yield separates that return, bit2 and the cursor.
                *progress=cursor.afterVideoFadeRequest();
                session.raceFlags1B0_=std::uint8_t((session.raceFlags1B0_&~4u)|(race.flags1B0KnownValue_&4u));
                break;
            }
            default:return *progress; //Advanced, unchanged, exact end cut or retained numeric boundary.
            }
        }
    }catch(...){return boundary(race.endClockProgress_?race.endClockProgress_->sourcePC:0x1a7da0);}
}

original_camera_session50KX::Result Registration::advanceFadeIn1A7BD0(
    rr::game::OriginalRaceConstructor50LA& race,original_camera_session50KX::Session& session)noexcept {
    using S=original_camera_session50KX::Status;
    if(!endStateCurrent(race,session))return {S::owner,0x1a7bd0};
    try {
        const auto actual=race.borrowRaceCamera34();
        if(!actual||actual.get()!=&session)return {S::owner,0x1a7bd0};
        auto& app=*session.displayInputs_;auto& video=app.video;
        {static int reported=-1;const int key=video.flags404Low6|(video.covered3DC<<8)|(app.fadeManagerA8<<12)|
            ((race.flags1B0KnownValue_&8u)?1<<20:0)|(session.endingFadeRequest_.pending()?1<<21:0)|
            (session.retainedFadeInRequestPending50LA()?1<<22:0);
         if(key!=reported){reported=key;std::fprintf(stderr,
            "fade_in_1a7bd0 flags404=%02x covered=%u manager=%u race_bit3=%d ending_pending=%d retained_in=%d\n",
            unsigned(video.flags404Low6),unsigned(video.covered3DC),unsigned(app.fadeManagerA8),
            (race.flags1B0KnownValue_&8u)?1:0,session.endingFadeRequest_.pending()?1:0,
            session.retainedFadeInRequestPending50LA()?1:0);}}
        if(session.endingFadeRequest_.pending()||
           (session.retainedFadeInRequestPending50LA()&&app.fadeManagerA8!=8))return session.set(S::owner,0x2ae93c);
        if((video.flags404Low6&3u)&&!(video.flags404Low6&4u)&&
           !(app.fadeManagerA8==8&&session.retainedFadeInRequestPending50LA()))return session.set(S::videoPending);
        if((race.flags1B0KnownMask_&8u)!=8u)return session.set(S::owner,0x1a7bd0);
        if(race.flags1B0KnownValue_&8u){
            // Existing real accepted-restored-image scheduling; no fabricated
            // guest FC/null, fade acknowledgment or alternative video object.
            if(app.fadeManagerA8||!(video.covered3DC||(video.flags404Low6&16u)))
                return session.set(S::videoPending,0x1a7bf8);
            session.result_.start=rr::game::requestOriginalFirstMainFade1B7D50(video,app.fadeManagerA8,app.fadeDuration98);
            if(session.result_.start.status!=rr::game::OriginalCameraStartStatus::complete)
                return session.set(S::videoPending,session.result_.start.sourcePC);
            race.flags1B0KnownValue_=std::uint8_t(race.flags1B0KnownValue_&~8u); //1A7C18, ONLY after actual acceptance.
        }
        // One-way native camera projection; unknown low2 and all other lanes
        // retain their independent ownership. Later video failure keeps bit3.
        session.raceFlags1B0_=std::uint8_t((session.raceFlags1B0_&~8u)|(race.flags1B0KnownValue_&8u));
        if(app.fadeManagerA8==8){
            session.result_.start=session.serviceAsyncFadeInRequest260610();
            if(session.result_.start.status!=rr::game::OriginalCameraStartStatus::complete)
                return session.set(S::videoPending,session.result_.start.sourcePC);
        }else if(app.fadeManagerA8)return session.set(S::sourcePath,0x2ae7fc);
        return session.set(S::complete,0x2ae958);
    }catch(...){return {S::owner,0x1a7bd0};}
}
}
