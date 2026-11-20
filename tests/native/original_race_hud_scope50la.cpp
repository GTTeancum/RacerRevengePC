#include <cstdio>
#include "original_race_hud_scope50la.h"
#include "native_common_race50pd.h"
#include "terrain_interactive_continuation50du.h"
#include "terrain_interactive_publication50du.h"
#include "original_consumed_stage50du.h"
#include "original_race_constructor50la.h"
#include "original_race_world_join50la.h"
#include "common_track50cr.h"
#include "original_camera_session50kx.h"
#include "../../src/game/lap_event6_subscriber50id.h"
#include "../../src/game/dynamic_impact_events50iw.h"
#include "../../src/game/lap_host_clock50id.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"
#include "race_music50ie_fixture.h"
#include "source_announcer50ie_fixture.h"
#include "live_audio50hf_fixture.h"
#include "original_ai_audio_owner.h"
#include "lap_drive50gj_fixture.h"
#include "native_ai_session50iv_fixture.h"
#include <bit>
#include <utility>

namespace original_race_hud_scope50LA {
Registration::Registration(const original_main_entry50KG::Application& application,
    const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& race,
    const native_racer50HN::Bank& bank,const native_ai50IV::Session& manager,rr::game::OriginalRaceHud50KG& hud,
    race_music50IE::Owner* music,source_announcer50IE::Owner* announcer,
    live_audio50HF::Owner* gameplay,native_original_ai_audio::Owner* engines,race_audio50HQ::Owner* countdown)noexcept
    :thread_(std::this_thread::get_id()),application_(&application),race_(race),
     bank_(&bank),manager_(&manager),generation_(race->entity_.sessionGeneration),hud_(&hud),
     music_(music),announcer_(announcer),gameplay_(gameplay),engines_(engines),countdown_(countdown){}

Scope::Scope(::interactive_continuation50DU::Context& actual,const native_racer50HN::Bank& bank)noexcept {
    const auto& app=actual.originalMainApplication;
    // The one consumed race, checked against its one World and the running Track
    // (the same rule on every course).
    const auto& race=native_common_race50PD::race(actual);
    const bool worldCurrent=race&&native_common_race50PD::worldCurrent(actual,*race);
    if(!race||race->resultsFeedbackThread_!=std::this_thread::get_id()||
       app.race28C()!=race.get()||!race->counted_||race->rolledBack_||
       !race->entity_.sessionGeneration||race->application_!=app.entities()||
       !worldCurrent||!race->endHud_.expired())return;
    try {
        //The exact Context field is borrowed by Publication and Frame too.
        //It may not yet have loaded/constructed views: this registers lifetime
        //only. Consumption below separately checks all reached source rows.
        auto registration=std::shared_ptr<Registration>(new Registration(app,race,bank,actual.originalAiSession,
            actual.sourceRaceHud,actual.raceMusic.get(),actual.sourceAnnouncer.get(),
            actual.liveAudio.get(),actual.originalAiAudioEnabled?actual.originalAiAudio.get():nullptr,
            actual.raceAudio.get()));
        if(actual.dynamicContactEnabled){
            //Exact lexical Session FIFO shared by material/contact/damage/lap
            //producers and Loop; never the distinct mode1Owner.events queue.
            registration->events_=&actual.dynamicEvents;
            registration->eventBindings_=&actual.dynamicEventBindings;
            registration->lapCC8_=&actual.lapContinuity.state.vehicleCC8;
        }
        race->endHud_=registration;registration_=std::move(registration);
    }catch(...){/*No source writes or startup admission change on allocation failure.*/}
}
void Scope::revoke()noexcept {
    if(!registration_)return;
    if(const auto race=registration_->race_.lock())
        if(race->music10C_)race->music10C_->revokeNativeProviders(*registration_);
    if(registration_->gameplay_&&registration_->gameplay_->speechApplication_==registration_->application_&&
       registration_->gameplay_->speechBankOwner_==registration_->bank_&&
       registration_->gameplay_->speechGeneration_==registration_->generation_&&
       // A player whose speech owner was constructed at the pod load (D48,
       // forward-bound) is retired by its racer's own release (2C10B0, in the
       // camera close that follows this revoke); that authenticated retirement
       // withdraws these same fields. Only an unbound borrow is revoked here.
       !registration_->gameplay_->speechForwardBound_)
        registration_->gameplay_->revokeNativeHumanSpeech();
    if(registration_->engines_&&registration_->engines_->nativeSpeechApplication_==registration_->application_&&
       registration_->engines_->nativeSpeechBank_==registration_->bank_&&
       registration_->engines_->nativeSpeechGeneration_==registration_->generation_)
        registration_->engines_->revokeNativeSpeechReceipt();
    registration_->active_=false;registration_->hud_=nullptr;registration_->camera_=nullptr;
    registration_->music_=nullptr;registration_->announcer_=nullptr;
    registration_->gameplay_=nullptr;registration_->engines_=nullptr;
    registration_->countdown_=nullptr;
    //No pending FIFO reset: an interrupted End consumer remains blocked until
    //this lexical Session queue dies. Its guard is never dereferenced.
    registration_->events_=nullptr;registration_->eventBindings_=nullptr;registration_->lapCC8_=nullptr;
    if(const auto race=registration_->race_.lock())
        if(race->endHud_.lock()==registration_){
            //Invalidate native borrowed-slot knownness, not a guest null store.
            race->hudTextSlots_.reset();race->endHud_.reset();
        }
    registration_.reset();
}
Scope::~Scope()noexcept {revoke();}

void Registration::observeConstructed1A8CC4(::interactive_publication50DU::Context& actual)noexcept {
    //Private synchronous Publication call: Context and its Application slot
    //are owned by this same lexical thread, not an external cross-thread API.
    const auto* published=actual.originalMainApplication.race28C();
    if(!published||published->resultsFeedbackThread_!=std::this_thread::get_id())return;
    //Successful HUD construction is common to every race. Select ONLY the
    //actual published owner (the one consumed stage's race).
    const auto& race=actual.consumedStage.race();
    if(!race||race.get()!=published)return;
    if(!race->counted_||race->application_!=actual.originalMainApplication.entities()||race->rolledBack_)return;
    //The successful actual producer has passed the original text/gleam
    //stores. Even without a borrow registration these slots are no longer
    //truthfully described as fresh constructor-null. Gleams are not joined.
    race->hudGleamConstructorNullKnown_=false;
    const auto registration=race->endHud_.lock();
    if(!registration||registration->thread_!=std::this_thread::get_id()||
       !registration->active_||registration->application_!=&actual.originalMainApplication||
       registration->bank_!=&actual.racerBank||registration->hud_!=&actual.sourceRaceHud||
       (actual.dynamicContactEnabled?
           (registration->events_!=&actual.dynamicEvents||registration->eventBindings_!=&actual.dynamicEventBindings||
            registration->lapCC8_!=&actual.lapContinuity.state.vehicleCC8):
           registration->events_!=nullptr)||
       registration->generation_!=race->entity_.sessionGeneration){
        race->hudTextSlots_.reset();return;
    }
    if(registration->attempted_)return; //No replay of partial native publication.
    registration->attempted_=true;
    const auto& camera=actual.sourceCameraSession;
    if(!race->hudTextSlots_||!camera||camera->closed_||camera->abandoned_||!camera->coldBound_||
       camera->generation_!=registration->generation_||&camera->actual_.bank!=registration->bank_||
       camera->actual_.raceHud!=registration->hud_||camera->actual_.humanCount34!=1u||
       camera->actual_.applicationEntities!=race->application_||
       !camera->viewport_.prepared||camera->viewport_.sessionGeneration!=registration->generation_||
       !actual.sourceRaceHud.loaded()||actual.sourceRaceHud.viewCount()!=1u){
        race->hudTextSlots_.reset();return;
    }
    auto& slots=*race->hudTextSlots_;
    for(std::size_t view=0;view<2;++view){
        if(slots.status17C[view]||slots.time164[view])return;
        for(const auto* value:slots.records12C[view])if(value)return;
        for(const auto* value:slots.explosions16C[view])if(value)return;
    }
    bool complete=true;
    const auto publish=[&](const rr::game::OriginalRaceHudText50KG*& slot,
        std::uint32_t offset)noexcept {
        const auto* value=actual.sourceRaceHud.textAt(0,offset);
        //A foreign viewport is an unsupported native borrow, not a source
        //29CA40 null result. Retain previously reached stores on rejection.
        if((value&&value->viewport!=&camera->viewport_)||actual.sourceRaceHud.textAt(1,offset))return false;
        slot=value; //Original always stores its result, including null.
        if(!value)complete=false; //Null does not roll back or skip later slots.
        return true;
    };
    //Original source order, NOT an atomic afterimage: seven/view1A8D28,
    //then per-view17C1A8DE0,1641A8E58,16C1A8ED0,1701A8F48.
    for(std::size_t i=0;i<7;++i)if(!publish(slots.records12C[0][i],0x12cu+4u*i))return;
    if(!publish(slots.status17C[0],0x17c))return;
    if(!publish(slots.time164[0],0x164))return;
    for(std::size_t i=0;i<2;++i)if(!publish(slots.explosions16C[0][i],0x16cu+4u*i))return;
    //Native HUD construction itself preallocates transactionally. This join
    //does not claim to implement original29CA40 allocation/resource failures
    //or their logging, and admits end-stop only for the complete actual owner.
    if(complete){registration->camera_=camera.get();registration->published_=true;}

}

bool Registration::stop1AD230(const rr::game::OriginalRaceConstructor50LA& race)noexcept {
    //Immutable fence BEFORE mutable liveness/HUD/weak state. Scope never
    //moves or outlives the same-thread Context run and revokes before close.
    if(std::this_thread::get_id()!=thread_)return false;
    if(!active_||!published_||!hud_||!bank_||!camera_)return false;
    const auto retained=race_.lock();
    if(retained.get()!=&race||application_->race28C()!=&race||
       application_->entities()!=race.application_||!race.counted_||race.rolledBack_||
       race.entity_.sessionGeneration!=generation_||race.endHud_.lock().get()!=this||
       !race.hudTextSlots_||!race.anyWorld())return false;
    try {
        //This existing synchronous borrow retains the Application-issued
        //Session bound as Camera34 during the HUD operation (every race).
        const auto camera=race.borrowRaceCamera34();
        if(!camera||camera.get()!=camera_)return false;
        if(!camera->coldBound_||!camera->readsBound_||camera->closed_||camera->abandoned_||
           camera->generation_!=generation_||&camera->actual_.bank!=bank_||
           camera->actual_.applicationEntities!=race.application_||camera->actual_.raceHud!=hud_||
           camera->actual_.humanCount34!=1u||(camera->raceFlags1B0_&3u)!=1u||
           !camera->nativeTaIssuance_||camera->nativeTaIssuance_->issuer!=application_||
           camera->nativeTaIssuance_->generation!=generation_||
           !race.raceCamera34Issued(*camera)||
           !camera->identitiesCurrent()||
           !camera->viewport_.prepared||camera->viewport_.sessionGeneration!=generation_||
           !hud_->loaded()||hud_->viewCount()!=1u)return false;
        //Actual1A8CC4 publication constructs eleven distinct view0 objects
        //against this exact viewport; view1 remains null. No gleam stop occurs.
        constexpr std::uint32_t offsets[]{0x17c,0x164,0x12c,0x16c,0x130,0x170,
            0x134,0x138,0x13c,0x140,0x144};
        for(const auto offset:offsets){
            const auto* text=hud_->textAt(0,offset);
            if(!text||text->viewport!=&camera->viewport_||hud_->textAt(1,offset))return false;
        }
        const auto& slots=*race.hudTextSlots_;
        for(std::size_t view=0;view<2;++view){
            if(slots.status17C[view]!=hud_->textAt(view,0x17c)||
               slots.time164[view]!=hud_->textAt(view,0x164))return false;
            for(std::size_t i=0;i<7;++i)
                if(slots.records12C[view][i]!=hud_->textAt(view,0x12cu+4u*i))return false;
            for(std::size_t i=0;i<2;++i)
                if(slots.explosions16C[view][i]!=hud_->textAt(view,0x16cu+4u*i))return false;
        }
        //No allocation/callback/yield follows this preflight. Existing helper
        //performs1AD230's exact row order and29DB10's four stores; do not copy
        //those fields into another HUD or reimplement its animation machine.
        return hud_->stopAll1AD230();
    }catch(...){return false;}
}

//Direct1A5940 paths that do not enter unowned D48 speech. A real event1
//with AI actors or failed speech chances can reach its actual Announcer1/2
//helper and return; selected speech never becomes a fabricated silent return.
bool Registration::readNativeListenerScene(const rr::game::OriginalRaceConstructor50LA& race,
    const original_camera_session50KX::Session& camera,bool heldSourceScene,
    std::uint64_t previousPublication,std::uint64_t& publication,
    std::array<const float*,9>& actualXYZ)const noexcept {
    const bool human=camera_==&camera&&endStateCurrent(race,camera);
    // Dedicated zero-human, one-view ALL8 adapter. The actual Application
    // proves this Race's unique Session/Bank/manager/display and lexical Scope.
    // Do not reuse endStateCurrent or publish HUD camera_ to obtain authority.
    // This borrow deliberately does not check listenerStepStopped: the caller
    // already claimed the actual mix token before these reached camera reads.
    const bool all8=!camera_&&!published_&&application_&&race_.lock().get()==&race&&
        race.endHud_.lock().get()==this&&application_->consumedRaAll8ListenerCustodyNativeCurrent(camera);
    {
        // Read-only refusal diagnostic (first few only).
        static int reported=0;
        const bool refuse=(!human&&!all8)||!camera.coldBound_||camera.closed_||camera.abandoned_||!camera.hasView()||
            camera.frameStage_!=original_camera_session50KX::Session::FrameStage::published||
            !race.ownsNativeCameraAdapter34(camera)||
            camera.publicationCount_<previousPublication||
            (!heldSourceScene&&camera.publicationCount_==previousPublication);
        if(refuse&&reported<4){++reported;
            std::fprintf(stderr,"listener_scene_refused human=%d all8=%d scope_camera=%d end_state=%d bound=%d closed=%d view=%d stage=%d pending=%d owns34=%d publication=%llu previous=%llu held=%d\n",
                int(human),int(all8),int(camera_==&camera),int(camera_==&camera&&endStateCurrent(race,camera)),
                int(camera.coldBound_),int(camera.closed_||camera.abandoned_),int(camera.hasView()),
                int(camera.frameStage_),int(camera.pendingDisplayWork()),int(race.ownsNativeCameraAdapter34(camera)),
                static_cast<unsigned long long>(camera.publicationCount_),
                static_cast<unsigned long long>(previousPublication),int(heldSourceScene));}
    }
    if((!human&&!all8)||!camera.coldBound_||
       camera.closed_||camera.abandoned_||!camera.hasView()||
       camera.frameStage_!=original_camera_session50KX::Session::FrameStage::published||
       // The original listener update (1A4440) reads Race34's scene every
       // frame, including while a video fade runs (the intro exit fades in
       // over the held start scene); a pending fade does not change the
       // published pose read here.
       !race.ownsNativeCameraAdapter34(camera)||
       camera.publicationCount_<previousPublication||
       (!heldSourceScene&&camera.publicationCount_==previousPublication))return false;
    publication=camera.publicationCount_;
    // Actual native projection of source Race34 scene180/190/1A0. Borrow
    // member addresses only: source reads occur later at their reached sites.
    // Neither renderer output nor the separate AI/audioListener copy is read.
    actualXYZ={{&camera.pose_.position.x,&camera.pose_.position.y,&camera.pose_.position.z,
        &camera.scene_.look.x,&camera.scene_.look.y,&camera.scene_.look.z,
        &camera.scene_.up.x,&camera.scene_.up.y,&camera.scene_.up.z}};
    return true;
}

bool Registration::prepareNativeContactEvent1(const rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::RaceEvent50ID& event,rr::game::DynamicContactEventCursor50IW& c,
    std::array<std::optional<std::uint32_t>,18>& words,rr::game::LapEventRequest50ID& request,
    bool& needsAnnouncer,bool& needsSpeech,bool& taunt,std::uint32_t& speechActor,std::uint32_t& pc)const noexcept {
    using namespace rr::game;using P=DynamicContactEventPhase50IW;
    request={};needsAnnouncer=false;needsSpeech=false;taunt=false;speechActor=0;pc=0x1a5964;
    if(!camera_||!endStateCurrent(race,*camera_)||!events_)return false;
    c.event=event;c.phase=P::Begin;
    if(!event.actor0C||!event.actor10){c.phase=P::Complete;pc=0x1a5c10;return true;}
    const auto& camera=*camera_;
    const auto readActor=[&](std::uint32_t actor,bool identity60,std::size_t lane,std::uint32_t sourcePC){
        pc=sourcePC;
        if(!actor||actor>bank_->count())return false;
        const auto slot=static_cast<std::size_t>(actor-1u);
        if(!bank_->active(slot)||slot>=camera.racerEntities_.size()||
           !camera.racerEntities_[slot].prepared())return false;
        try {words[lane]=identity60?camera.actual_.racer60[slot]:bank_->read(slot,camera.reads_.class58);}
        catch(...){return false;}
        return true;
    };
    const auto chance=[&](unsigned field,std::size_t lane,std::uint32_t drawPC,
        std::uint32_t convertPC,std::uint32_t loadPC,std::uint32_t mulPC,
        std::uint32_t comparePC,bool& passed){
        pc=drawPC;const auto state=application_->state;
        if(!state||camera.actual_.arena.borrowedNoise50KG!=&state->sharedNoise308030)return false;
        auto& raw=state->rawRandom308030;
        raw=raw*0x41c64e6du+0x3039u;words[lane]=raw&0x7fffffffu;
        ++c.randomDraws;c.chanceDrawPending=true; //Native custody AFTER actual raw store.
        pc=convertPC;if(std::fegetround()!=FE_TONEAREST)return false;
        volatile float draw=static_cast<float>(*words[lane]);
        pc=loadPC;if(!eventBindings_)return false;
        //Read only the REACHED actual authored lane, after the source draw.
        if(field==0x138u){
            if(!eventBindings_->chance138Bound)return false;
            words[lane+1u]=std::bit_cast<std::uint32_t>(eventBindings_->chance138);
        }else if(field==0x13cu){
            if(!eventBindings_->chance13CBound)return false;
            words[lane+1u]=std::bit_cast<std::uint32_t>(eventBindings_->chance13C);
        }else{
            if(!eventBindings_->chance14CBound)return false;
            words[lane+1u]=std::bit_cast<std::uint32_t>(eventBindings_->chance14C);
        }
        pc=mulPC;volatile float fraction=std::bit_cast<float>(std::uint32_t{0x30000000})*draw;
        words[lane+2u]=std::bit_cast<std::uint32_t>(static_cast<float>(fraction));
        c.chanceFraction=fraction;
        pc=comparePC;const float threshold=std::bit_cast<float>(*words[lane+1u]);
        if(!std::isfinite(fraction)||!std::isfinite(threshold))return false;
        passed=fraction<threshold;c.chanceDrawPending=false;return true;
    };
    if(!readActor(event.actor0C,false,0,0x1a598c))return false;
    c.partnerSpeech=false;c.selfSpeech=false; //Source S3/S4=0, not callback receipts.
    if(!*words[0]){
        c.phase=P::PartnerHuman;
        if(!chance(0x138u,2,0x1a599c,0x1a59ac,0x1a59b4,0x1a59b8,0x1a59bc,c.partnerSpeech))return false;
        if(!c.partnerSpeech){
            c.phase=P::PartnerHumanFallback;
            if(!chance(0x13cu,5,0x1a59dc,0x1a59ec,0x1a59f4,0x1a59f8,0x1a59fc,c.selfSpeech))return false;
        }
    }else{
        if(!readActor(event.actor10,false,1,0x1a5a18))return false;
        if(!*words[1]){
            c.phase=P::SelfHuman;
            if(!chance(0x138u,2,0x1a5a24,0x1a5a34,0x1a5a3c,0x1a5a40,0x1a5a44,c.selfSpeech))return false;
            if(!c.selfSpeech){
                c.phase=P::SelfHumanFallback;
                if(!chance(0x13cu,5,0x1a5a64,0x1a5a74,0x1a5a7c,0x1a5a80,0x1a5a84,c.partnerSpeech))return false;
            }
        }
    }
    c.phase=P::PartnerSpeech;
    if(c.partnerSpeech){pc=0x1a5aa4;needsSpeech=true;taunt=true;speechActor=event.actor0C;return true;}
    c.phase=P::SelfSpeech;
    if(c.selfSpeech){pc=0x1a5ab8;needsSpeech=true;speechActor=event.actor10;return true;}
    return finishNativeContactEvent1(race,c,words,request,needsAnnouncer,pc);
}

bool Registration::finishNativeContactEvent1(const rr::game::OriginalRaceConstructor50LA& race,
    rr::game::DynamicContactEventCursor50IW& c,std::array<std::optional<std::uint32_t>,18>& words,
    rr::game::LapEventRequest50ID& request,bool& needsAnnouncer,std::uint32_t& pc)const noexcept {
    using namespace rr::game;using P=DynamicContactEventPhase50IW;
    request={};needsAnnouncer=false;pc=0x1a5ac4;
    if(!camera_||!endStateCurrent(race,*camera_)||!events_)return false;
    const auto& camera=*camera_;const auto& event=c.event;
    const auto readActor=[&](std::uint32_t actor,bool identity60,std::size_t lane,std::uint32_t sourcePC){
        pc=sourcePC;
        if(!actor||actor>bank_->count())return false;
        const auto slot=static_cast<std::size_t>(actor-1u);
        if(!bank_->active(slot)||slot>=camera.racerEntities_.size()||
           !camera.racerEntities_[slot].prepared())return false;
        try {words[lane]=identity60?camera.actual_.racer60[slot]:bank_->read(slot,camera.reads_.class58);}
        catch(...){return false;}
        return true;
    };
    const auto chance=[&](unsigned field,std::size_t lane,std::uint32_t drawPC,
        std::uint32_t convertPC,std::uint32_t loadPC,std::uint32_t mulPC,
        std::uint32_t comparePC,bool& passed){
        pc=drawPC;const auto state=application_->state;
        if(!state||camera.actual_.arena.borrowedNoise50KG!=&state->sharedNoise308030)return false;
        auto& raw=state->rawRandom308030;
        raw=raw*0x41c64e6du+0x3039u;words[lane]=raw&0x7fffffffu;
        ++c.randomDraws;c.chanceDrawPending=true; //Native custody AFTER actual raw store.
        pc=convertPC;if(std::fegetround()!=FE_TONEAREST)return false;
        volatile float draw=static_cast<float>(*words[lane]);
        pc=loadPC;if(!eventBindings_)return false;
        //Read only the REACHED actual authored lane, after the source draw.
        if(field==0x138u){
            if(!eventBindings_->chance138Bound)return false;
            words[lane+1u]=std::bit_cast<std::uint32_t>(eventBindings_->chance138);
        }else if(field==0x13cu){
            if(!eventBindings_->chance13CBound)return false;
            words[lane+1u]=std::bit_cast<std::uint32_t>(eventBindings_->chance13C);
        }else{
            if(!eventBindings_->chance14CBound)return false;
            words[lane+1u]=std::bit_cast<std::uint32_t>(eventBindings_->chance14C);
        }
        pc=mulPC;volatile float fraction=std::bit_cast<float>(std::uint32_t{0x30000000})*draw;
        words[lane+2u]=std::bit_cast<std::uint32_t>(static_cast<float>(fraction));
        c.chanceFraction=fraction;
        pc=comparePC;const float threshold=std::bit_cast<float>(*words[lane+1u]);
        if(!std::isfinite(fraction)||!std::isfinite(threshold))return false;
        passed=fraction<threshold;c.chanceDrawPending=false;return true;
    };
    c.phase=P::AttackChance;pc=0x1a5ac4;words[8]=event.word8;
    if(!*words[8]){c.phase=P::Complete;pc=0x1a5c10;return true;}
    bool attack=false;
    if(!chance(0x14cu,9,0x1a5ad0,0x1a5ae0,0x1a5ae8,0x1a5aec,0x1a5af0,attack))return false;
    if(!attack){c.phase=P::Complete;pc=0x1a5c10;return true;}
    c.phase=P::AnnouncerActor;
    if(!readActor(event.actor0C,false,12,0x1a5b00))return false; //Actual later reload, not saved first58.
    c.announcerPartnerHuman=!*words[12];c.phase=P::AnnouncerChance;
    if(c.announcerPartnerHuman){
        if(!chance(0x138u,13,0x1a5b0c,0x1a5b1c,0x1a5b24,0x1a5b28,0x1a5b2c,c.announcerNamed))return false;
    }else if(!chance(0x13cu,13,0x1a5b48,0x1a5b58,0x1a5b60,0x1a5b64,0x1a5b68,c.announcerNamed))return false;
    c.phase=P::AnnouncerRequest;
    std::uint32_t type,racer,variant;
    if(c.announcerNamed){
        if(!readActor(event.actor0C,true,16,0x1a5b94))return false;
        racer=*words[16];type=2u;
        variant=racer==16u?1u:(c.announcerPartnerHuman?0u:2u);
        pc=racer==16u?0x1a5bacu:0x1a5bc4u;
    }else{
        if(!readActor(event.actor10,true,16,0x1a5bd4))return false;
        racer=*words[16];
        if(racer==16u){
            if(!readActor(event.actor0C,true,17,0x1a5be8))return false;
            racer=*words[17];type=2u;variant=2u;pc=0x1a5bf0;
        }else{type=1u;variant=0u;pc=0x1a5c08;}
    }
    request={LapEventCall50ID::Announcer2C2C20,1u,pc,0u,type,racer,variant};
    needsAnnouncer=true;c.phase=P::AfterAnnouncer;return true;
}

//Actual native D48 inputs for event1/5. Never use the atomic50HG kernel
//or substitute the human state/provider for a distinct actual AI Bank slot.
bool Registration::readNativePodSpeechOwner(const rr::game::OriginalRaceConstructor50LA& race,
    std::uint32_t actor,const rr::game::PodSpeechCounts50HG*& counts,
    rr::game::PodSpeechState50HG*& speech,std::array<std::uint32_t,3>& local,
    std::array<std::uint32_t,4>& main70,std::array<std::optional<std::uint32_t>,16>& words,
    std::uint32_t& pc)const noexcept {
    using namespace rr::game;counts=nullptr;speech=nullptr;
    if(!actor||actor>bank_->count()||!camera_||!endStateCurrent(race,*camera_))return false;
    const auto slot=static_cast<std::size_t>(actor-1u);const auto& camera=*camera_;
    const auto appState=application_->state;
    if(!bank_->active(slot)||!appState||camera.actual_.arena.borrowedNoise50KG!=&appState->sharedNoise308030)return false;
    if(slot==0){
        PodSpeechCounts50HG* human=nullptr;
        if(!gameplay_||!gameplay_->nativeHumanSpeechOwner(*application_,*bank_,generation_,camera.actual_.racer60[slot],
            appState->rawRandom308030,human,speech))return false;
        counts=human;
    }else if(!engines_||!engines_->nativeSpeechOwner(*application_,*bank_,generation_,
        appState->rawRandom308030,bank_->count(),slot,camera.actual_.racer60[slot],counts,speech))return false;
    //2C205C/64: source scratch10/20=(0,0,0,1), before D48.20C/340.
    for(std::size_t k=0;k<3;++k){words[k]=main70[k]=0u;words[4+k]=local[k]=0u;}
    words[3]=main70[3]=0x3f800000u;words[15]=0x3f800000u;
    bool read=false;
    try {camera.actual_.bank.visit(slot,[&](){
        const auto& racer=camera.racerEntities_[slot];const auto& frame=camera.actual_.arena;
        pc=0x2c206c;
        if(!racer.prepared()||!racer.body340||!racer.body340->entity().reference00||
           racer.body340->entity().sessionGeneration!=generation_||
           racer.body340->canonicalNumericAddress()!=&frame.bodies.main)return;
        //2C2070 LQ main340+70: actual XYZ plus its retained original W lane.
        pc=0x2c2070;
        for(std::size_t k=0;k<3;++k)words[k]=main70[k]=std::bit_cast<std::uint32_t>(frame.bodies.main.components.root.body.velocity.v[k]);
        words[3]=main70[3]=std::bit_cast<std::uint32_t>(frame.bodies.mainSphere.velocity70W);
        //Only the later22A180 getter reaches actual340->288.28B180 is raw
        //localF0/F4/F8, NOT28B1A0's lazy world refresh; dirty is not repaired.
        pc=0x22a198;const auto* scene=racer.body340->scene288();
        if(!scene||scene!=racer.scene190||!scene->constructed||scene->released||
           !scene->entity().reference00||scene->entity().sessionGeneration!=generation_||
           scene->sceneIndex!=camera.actual_.bindings.lifecycle.main190)return;
        const auto index=static_cast<int>(scene->sceneIndex);
        if(index<0||static_cast<std::size_t>(index)>=frame.sceneNodeCount)return;
        pc=0x28b180;
        for(std::size_t k=0;k<3;++k)words[4+k]=local[k]=std::bit_cast<std::uint32_t>(frame.nodes[index].localC0.v[12+k]);
        read=true;
    });}catch(...){return false;}
    return read;
}
bool Registration::readNativeDamageSpeech5Owner(const rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::RaceEvent50ID& event,const rr::game::PodSpeechCounts50HG*& counts,
    rr::game::PodSpeechState50HG*& speech,std::array<std::uint32_t,3>& local,
    std::array<std::uint32_t,4>& main70,std::array<std::optional<std::uint32_t>,16>& words,
    std::uint32_t& pc)const noexcept {
    pc=0x1a532c;if(event.actor0C!=1u)return false; //Unchanged selected5 human-only domain.
    return readNativePodSpeechOwner(race,event.actor0C,counts,speech,local,main70,words,pc);
}
bool Registration::playNativePodSpeech(const rr::game::OriginalRaceConstructor50LA& race,
    std::uint32_t actor,bool taunt,std::uint32_t selectedOrdinal,std::uint32_t& pc)const noexcept {
    using namespace rr::game;pc=taunt?0x2c1cb8u:0x2c1bd0u;
    if(!actor||actor>bank_->count()||!camera_||!endStateCurrent(race,*camera_)||!application_->state)return false;
    const auto slot=static_cast<std::size_t>(actor-1u);
    const AudioCommand50HE command{AudioOperation50HE::Start,
        {taunt?AudioBank50HE::PodTaunts:AudioBank50HE::PodCurses,selectedOrdinal},1.f,1.f,false,pc};
    if(slot==0){
        PodSpeechCounts50HG* counts=nullptr;PodSpeechState50HG* speech=nullptr;
        if(!gameplay_||!gameplay_->nativeHumanSpeechOwner(*application_,*bank_,generation_,camera_->actual_.racer60[slot],
            application_->state->rawRandom308030,counts,speech))return false;
        return gameplay_->applySourceCommand(command).okay();
    }
    return engines_&&engines_->applyNativeSpeech(*application_,*bank_,generation_,
        application_->state->rawRandom308030,bank_->count(),slot,camera_->actual_.racer60[slot],command);
}
bool Registration::playNativeDamageSpeech5(const rr::game::OriginalRaceConstructor50LA& race,
    std::uint32_t selectedOrdinal,std::uint32_t& pc)const noexcept {
    return playNativePodSpeech(race,1u,false,selectedOrdinal,pc);
}

bool Registration::prepareNativeDamageEvent5(const rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::RaceEvent50ID& event,std::array<std::optional<std::uint32_t>,7>& words,
    rr::game::LapEventRequest50ID& request,bool& needsAnnouncer,bool& needsSpeech,std::uint32_t& pc)const noexcept {
    using namespace rr::game;
    request={};needsAnnouncer=false;needsSpeech=false;pc=0x1a52c8;
    if(!camera_||!endStateCurrent(race,*camera_)||!events_||!event.actor0C||
       event.actor0C>bank_->count())return false;
    const auto slot=static_cast<std::size_t>(event.actor0C-1u);const auto& camera=*camera_;
    try {
        if(!bank_->active(slot)||slot>=camera.racerEntities_.size()||
           !camera.racerEntities_[slot].prepared())return false;
        pc=0x1a52d0;words[0]=bank_->read(slot,camera.reads_.state40);
        if(*words[0]){pc=0x1a5388;return true;} //No58, draw, chance, repeat or60 read.
        bool selectedSpeech=false; //Source S2=0 in1A52D8 delay slot.
        pc=0x1a52dc;words[1]=bank_->read(slot,camera.reads_.class58);
        if(!*words[1]){
            pc=0x1a52e8;const auto state=application_->state;
            if(!state||camera.actual_.arena.borrowedNoise50KG!=&state->sharedNoise308030)return false;
            //Actual308030 raw store NOW, before scalar admission or138 load.
            //No seed/selector/range/clock operation is present in this source.
            auto& raw=state->rawRandom308030;
            raw=raw*0x41c64e6du+0x3039u;words[2]=raw&0x7fffffffu;
            pc=0x1a52f8;if(std::fegetround()!=FE_TONEAREST)return false;
            volatile float draw=static_cast<float>(*words[2]);
            pc=0x1a5300;
            //Real SPCHPCT.INI-bound native138 provider used by Setup/Loop,
            //not the historical constructor .5 or an external chance input.
            if(!eventBindings_||!eventBindings_->chance138Bound)return false;
            words[3]=std::bit_cast<std::uint32_t>(eventBindings_->chance138);
            pc=0x1a5304;
            volatile float fraction=std::bit_cast<float>(std::uint32_t{0x30000000})*draw;
            words[6]=std::bit_cast<std::uint32_t>(static_cast<float>(fraction));
            pc=0x1a5308;const float chance=std::bit_cast<float>(*words[3]);
            if(!std::isfinite(fraction)||!std::isfinite(chance))return false;
            selectedSpeech=fraction<chance; //Ordered C.LT.S; no extra[0,1] clamp/admission.
        }
        if(selectedSpeech){
            pc=0x1a532c;needsSpeech=true;return true; //Actual held native D48 call, not a returned subscriber.
        }
        pc=0x1a5338;words[4]=event.word8;
        if(*words[4]){pc=0x1a5388;return true;} //Any original nonzero repeat value suppresses.
        pc=0x1a5354;words[5]=camera.actual_.racer60[slot]; //Reached fresh actual identity60 only.
        const auto racer=*words[5];pc=racer==16u?0x1a5368u:0x1a5380u;
        //Typed request is native retained call custody; arguments are exactly
        //source type12 / current60 / racer16?1:0, not a supplied event/callback.
        request={LapEventCall50ID::Announcer2C2C20,1u,pc,0u,12u,racer,racer==16u?1u:0u};
        needsAnnouncer=true;return true;
    }catch(...){return false;}
}

//Exact fresh source reads for the retained event actor. dynamicActorResolver
//contains a native scheduling snapshot and is deliberately NOT used here.
bool Registration::beginNativeLapEvent6(const rr::game::OriginalRaceConstructor50LA& race,
    std::uint32_t actor,rr::game::LapEventCursor50ID& c,std::uint32_t& pc)const noexcept {
    using namespace rr::game;using P=LapEventPhase50ID;using K=LapEventCall50ID;
    pc=0x1a560c;
    if(!camera_||!endStateCurrent(race,*camera_)||!events_||!lapCC8_||
       !actor||actor>bank_->count())return false;
    const auto slot=static_cast<std::size_t>(actor-1u);const auto& camera=*camera_;
    try {
        if(!bank_->active(slot)||slot>=camera.racerEntities_.size()||
           !camera.racerEntities_[slot].prepared())return false;
        c.actor=actor;
        pc=0x1a5614;const auto actorClass=bank_->read(slot,camera.reads_.class58);
        pc=0x1a5618;
        //The real original rank pass owns CCC. Identity checks validate its
        //publication; no HOST standings or display-interpolated rank enters.
        if(!manager_->managerBound||!manager_->rankBound||!manager_->fields[slot].bound||
           manager_->fields[slot].placeCCC!=manager_->rank[slot].placeCCC)return false;
        c.place=std::bit_cast<std::uint32_t>(manager_->rank[slot].placeCCC);
        pc=0x1a561c;c.lap=bank_->read(slot,bank_->bindRead(*lapCC8_));
        c.phase=P::Complete;
        if(actorClass){pc=0x1a5914;return true;} //CCC/CC8 already read, flags/60 dead.
        pc=0x1a5628;
        const auto flags=bank_->read(slot,bank_->bindRead(camera.actual_.arena.fields.impactFlagsCD8));
        c.finished=(flags&1u)!=0;
        if(!c.finished&&c.place!=1u){pc=0x1a5914;return true;} //No60 read on this branch.
        pc=c.finished?0x1a5638:0x1a5738;
        c.racer60=camera.actual_.racer60[slot]; //Actual identity, never uninitialized human AI lane.
        if(c.finished){
            std::uint32_t type=6,variant=1;pc=0x1a5650;
            if(c.racer60!=16u){
                if(c.place==1u){variant=2;pc=0x1a5670;}
                else if(c.place==2u){type=7;variant=2;pc=0x1a568c;}
                else if(c.place==3u){type=8;variant=2;pc=0x1a56a8;}
                else pc=0x1a56c0;
            }
            const auto issued=lap_event_detail50ID::issue(c,K::Announcer2C2C20,pc,
                P::AfterAnnouncer,type,c.racer60,variant);
            return issued.status==LapEventStatus50ID::NeedHelper;
        }
        if(c.racer60==16u){pc=0x1a5914;return true;}
        if(c.lap>=1u&&c.lap<=3u){
            constexpr std::uint32_t callPC[]{0x1a5754u,0x1a5770u,0x1a578cu};
            pc=callPC[c.lap-1u];
            const auto issued=lap_event_detail50ID::issue(c,K::Announcer2C2C20,pc,
                P::AfterAnnouncer,c.lap+2u,c.racer60,0u);
            return issued.status==LapEventStatus50ID::NeedHelper;
        }
        c.phase=P::LastLap;pc=0x1a57b0;return true;
    }catch(...){return false;}
}
rr::game::LapEventResult50ID Registration::advanceNativeLapEvent6(
    const rr::game::OriginalRaceConstructor50LA& race,rr::game::LapEventCursor50ID& c,
    std::uint32_t& selectedIndex)const noexcept {
    using namespace rr::game;using P=LapEventPhase50ID;using S=LapEventStatus50ID;
    if(!camera_||!endStateCurrent(race,*camera_))return {S::ActorUnavailable,0x1a55f0,{}};
    if(c.pending.sequence)return lap_event_detail50ID::pending(c);
    if(c.phase==P::Complete)return {S::Complete,0x1a5914,{}};
    auto& selectors=camera_->actual_.sharedRandom;
    //Read current profile BEFORE admitting any reached lazy RNG dependency.
    //A non-last-lap return and finished→Music do not read those statics.
    const bool checksLastLap=c.phase==P::LastLap||(c.phase==P::AfterAnnouncer&&!c.finished);
    std::uint8_t lapCount=0;
    if(checksLastLap){
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection current;
        if(!race.profile_||race.profile_!=application_->riderProfile||
           !race.profile_->courseRaceSnapshot(current))return {S::ActorUnavailable,0x1a57c4,{}};
        lapCount=static_cast<std::uint8_t>(current.packed1125C); //Reached actual current profile1125C.
        if(c.lap!=static_cast<std::uint32_t>(lapCount&31u)-1u){
            c.phase=P::Complete;return {S::Complete,0x1a5914,{}};
        }
    }
    if((checksLastLap||c.phase==P::AfterClock)&&(!application_->state||
       &selectors!=&application_->state->sharedRandomSelectors))return {S::ActorUnavailable,c.phase==P::AfterClock?0x1a5810u:0x1a57d8u,{}};
    if(c.phase==P::AfterDraw){
        //Do not use the generic helper's early selected>=3 rejection: original
        //range lazy stores occur AFTER conversion even for rounded index4.
        if(std::fegetround()!=FE_TONEAREST||c.lastReturn>0x7fffffffu)
            return {S::UnsupportedNumeric,0x1a5830,{}};
        volatile float draw=static_cast<float>(c.lastReturn); //1A5838.
        if(!application_->state||&selectors!=&application_->state->sharedRandomSelectors)
            return {S::ActorUnavailable,0x1a583c,{}};
        auto& range=selectors.range;
        const bool wasInitialized=range.lazy80AC; //1A583C before MULs.
        volatile float fraction=std::bit_cast<float>(std::uint32_t{0x30000000})*draw;
        if(!std::isfinite(fraction))return {S::UnsupportedNumeric,0x1a5848,{}};
        volatile float chosen=3.f*fraction;
        if(!std::isfinite(chosen)||chosen<0.f||chosen>3.f)
            return {S::UnsupportedNumeric,0x1a5850,{}};
        selectedIndex=static_cast<std::uint32_t>(chosen)+1u; //CVT.W.S, then ADDIU1A585C.
        if(!wasInitialized){
            range.rangeCheck3D6B38=1u; //1A5868 before guard1A586C.
            range.lazy80AC=true;
        }
        //For valid1..3, actual133660(A0=1) returns0 without callbacks along
        //BOTH diagnostic-enable branches. Invalid indices remain unsupported:
        //never execute assertion callbacks or invent surviving S2/music event.
        if(selectedIndex<1u||selectedIndex>3u)
            return {S::RandomRange,range.rangeCheck3D6B38?0x1a58a4u:0x1a58f4u,{}};
        const auto music=selectedIndex==1u?34u:(selectedIndex==2u?29u:33u);
        return lap_event_detail50ID::issue(c,LapEventCall50ID::Music1A46E0,
            0x1a590c,P::AfterMusic,music);
    }
    //Existing source scalar machine mutates ONLY the real Application selector
    //cache. Its lazy writes and clock/seed/draw requests are never staged.
    return advanceLapEvent6Subscriber50ID(c,lapCount,selectors.seed);
}
bool Registration::invokeNativeLapEvent6(const rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::LapEventRequest50ID& request,std::uint32_t& v0)const noexcept {
    using namespace rr::game;using K=LapEventCall50ID;
    if(!request.sequence||!camera_||!endStateCurrent(race,*camera_))return false;
    v0=0; //Ignored helper return lane, not a fabricated clock/draw result.
    if(request.kind==K::Announcer2C2C20||request.kind==K::Music1A46E0){
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection current;
        if(!race.profile_||race.profile_!=application_->riderProfile||
           !race.profile_->courseRaceSnapshot(current))return false;
        if(request.kind==K::Announcer2C2C20)
            return announcer_&&announcer_->request(request.argumentA1,request.argumentA2,
                request.argumentA3,current.subtype,manager_->manager.flags58);
        return gameplay_&&gameplay_->outputOpen()&&music_&&
            music_->request(request.argumentA1,request.argumentA2,current.subtype).completed();
    }
    //Actual process owners retained by Main startup and borrowed by this
    //Camera. No local clock epoch, raw stream, selector copy or reseed policy.
    const auto state=application_->state;
    if(!state||&camera_->actual_.sharedRandom!=&state->sharedRandomSelectors)return false;
    if(request.kind==K::Clock101A50){
        if(!state->clock101A50||state->clock101A50.get()!=&camera_->actual_.applicationClock)return false;
        return sampleHostTimestamp50ID(*state->clock101A50,v0); //Existing explicit native clock backend.
    }
    if(request.kind==K::Seed308020||request.kind==K::Draw308030){
        if(camera_->actual_.arena.borrowedNoise50KG!=&state->sharedNoise308030)return false;
        //Raw-only seed/draw clients do not clear or consume incomplete noise
        //tables. Actual raw write is retained even if a later helper stops.
        return applyLapEventRandom50ID(request,state->rawRandom308030,v0);
    }
    return false;
}
}

//Private Race forwarding definition; the one World remains owned by Race74.
namespace rr::game {
// One rule replacing raceCameraCourse34(course): the issued Session records
// THIS race and ITS one World (R2's NativeTaIssuance race/world pair).
bool OriginalRaceConstructor50LA::raceCamera34Issued(const original_camera_session50KX::Session& session)const noexcept {
    const auto& issued=session.nativeTaIssuance_;
    return world74_&&issued&&issued->race==this&&issued->world==world74_.get()&&
        issued->generation==entity_.sessionGeneration;
}

}

namespace original_main_entry50KG {
// 149p: 1A87A0's two reached leaves on a human Start (before the race1B1
// request). 1A8860 199770(F8,0,1,1,1,1): the race's own attached F8, the same
// leaf the finish (1A7DF8) uses. 1A8868 1AD230(race): the race's live HUD
// scope (Registration::stop1AD230 authenticates race/camera/HUD itself).
void Application::countedPauseRequest1A87A0_149p(rr::game::OriginalRaceConstructor50LA& race,
    bool& f8,bool& hud)const noexcept {
    f8=hud=false;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||sourceRace28COwner_.get()!=&race||
       race.application_!=entitiesOwner_||!race.counted_||race.rolledBack_)return;
    if(race.f8Attached_&&race.f8Owner_&&race.f8Source_==race.f8Owner_.get()&&
       race.f8Source_->belongsTo(race.application_,race.entity_.sessionGeneration)){
        const auto* entity=race.f8Source_->entity();
        if(entity&&entity->parent1C==&race.entity_&&entity->mask20==0xffffffffu)
            f8=race.f8Source_->visibility199770(0,1,1,1,1);
    }
    if(const auto scope=race.endHud_.lock())hud=scope->stop1AD230(race);
}
}
