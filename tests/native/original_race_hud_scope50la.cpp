#include "original_race_hud_scope50la.h"
#include "terrain_interactive_continuation50du.h"
#include "terrain_interactive_publication50du.h"
#include "original_consumed_ra_stage50du.h"
#include "original_consumed_ta_stage50du.h"
#include "original_consumed_ma_stage50du.h"
#include "original_consumed_sa_stage50du.h"
#include "original_consumed_ga_stage50du.h"
#include "original_consumed_gc_stage50du.h"
#include "original_race_constructor50la.h"
#include "ra_cold_aggregate50la.h"
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
    const auto& app=actual.originalMainApplication;const auto& race=actual.consumedRaStage.race();
    if(!race||race->resultsFeedbackThread_!=std::this_thread::get_id()||
       app.race28C()!=race.get()||!race->counted_||race->rolledBack_||
       !race->entity_.sessionGeneration||race->application_!=app.entities()||
       !actual.raWorld||race->world74_!=actual.raWorld||!race->endHud_.expired())return;
    try {
        //The exact Context field is borrowed by Publication and Frame too.
        //It may not yet have loaded/constructed views: this registers lifetime
        //only. Consumption below separately checks all reached source rows.
        auto registration=std::shared_ptr<Registration>(new Registration(app,race,bank,actual.originalAiSession,
            actual.sourceRaceHud,actual.raceMusic.get(),actual.sourceAnnouncer.get(),
            actual.liveAudio.get(),actual.originalAiAudioEnabled?actual.originalAiAudio.get():nullptr,
            actual.raceAudio.get()));
        race->endHud_=registration;registration_=std::move(registration);
    }catch(...){/*No source writes or startup admission change on allocation failure.*/}
}
void Scope::revoke()noexcept {
    if(!registration_)return;
    if(const auto race=registration_->race_.lock())
        if(race->music10C_)race->music10C_->revokeNativeProviders(*registration_);
    registration_->active_=false;registration_->hud_=nullptr;registration_->camera_=nullptr;
    registration_->music_=nullptr;registration_->announcer_=nullptr;
    registration_->gameplay_=nullptr;registration_->engines_=nullptr;
    registration_->countdown_=nullptr;
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
    //Successful HUD construction is shared by all these existing consumed
    //courses. Select ONLY the actual published owner; unsupported non-RA
    //construction must withdraw fresh-null knownness too, never gain a Scope.
    const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>* selected=nullptr;
    for(const auto* candidate:{&actual.consumedRaStage.race(),&actual.consumedTaStage.race(),
        &actual.consumedMaStage.race(),&actual.consumedSaStage.race(),
        &actual.consumedGaStage.race(),&actual.consumedGcStage.race()}){
        if(candidate->get()==published){selected=candidate;break;}
    }
    if(!selected)return;
    const auto& race=*selected;
    if(!race->counted_||race->application_!=actual.originalMainApplication.entities()||race->rolledBack_)return;
    //The successful actual producer has passed the original text/gleam
    //stores. Even without a borrow registration these slots are no longer
    //truthfully described as fresh constructor-null. Gleams are not joined.
    race->hudGleamConstructorNullKnown_=false;
    const auto registration=race->endHud_.lock();
    if(!registration||registration->thread_!=std::this_thread::get_id()||
       !registration->active_||registration->application_!=&actual.originalMainApplication||
       registration->bank_!=&actual.racerBank||registration->hud_!=&actual.sourceRaceHud||
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
       !race.hudTextSlots_||!race.world74_)return false;
    try {
        //This existing synchronous borrow retains actual world/query/model
        //backing and prevents Camera/world closure during the HUD operation.
        const auto world=race.world74_;ra_cold_aggregate50LA::Diagnostic wd;
        const auto camera=world->borrowRuntimeSourceCamera(world,wd);
        if(!camera||camera.get()!=camera_)return false;
        if(!camera->coldBound_||!camera->readsBound_||camera->closed_||camera->abandoned_||
           camera->generation_!=generation_||&camera->actual_.bank!=bank_||
           camera->actual_.applicationEntities!=race.application_||camera->actual_.raceHud!=hud_||
           camera->actual_.humanCount34!=1u||(camera->raceFlags1B0_&3u)!=1u||
           !camera->nativeTaIssuance_||camera->nativeTaIssuance_->issuer!=application_||
           camera->nativeTaIssuance_->generation!=generation_||
           camera->nativeTaIssuance_->course!=rr::game::OriginalCameraStartCourse::ra||
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
}
