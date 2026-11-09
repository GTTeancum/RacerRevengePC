#include "original_results_feedback_scope50la.h"
#include "terrain_interactive_continuation50du.h"
#include "original_consumed_ra_stage50du.h"
#include "original_consumed_ta_stage50du.h"
#include "original_race_constructor50la.h"
#include "../../src/game/controller_feedback50kz.h"
#include <utility>

namespace original_results_feedback50LA {
Registration::Registration(const original_main_entry50KG::Application& application,
    const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& race,
    const native_racer50HN::Bank& bank,
    rr::game::ControllerFeedbackState50KZ (&devices)[2])noexcept
    :thread_(std::this_thread::get_id()),application_(&application),race_(race),
     generation_(race->entity_.sessionGeneration),bank_(&bank),devices_(&devices){}

Scope::Scope(::interactive_continuation50DU::Context& actual,
    const native_racer50HN::Bank& bank)noexcept {attach(actual,bank);}
void Scope::bindAfterPublication(::interactive_continuation50DU::Context& actual,
    const native_racer50HN::Bank& bank)noexcept {if(!registration_)attach(actual,bank);}
void Scope::attach(::interactive_continuation50DU::Context& actual,
    const native_racer50HN::Bank& bank)noexcept {
    const auto& application=actual.originalMainApplication;
    // RA's world join or the common race (TA) with its own counted world.
    const bool raRace=bool(actual.consumedRaStage.race());
    const auto& race=raRace?actual.consumedRaStage.race():actual.consumedTaStage.race();
    // This is only an additional Results borrow. Legacy/non-RA paths keep
    // their previous behavior, with NO registration, rather than acquiring
    // a surrogate Race/manager or changing startup admission.
    if(!race||race->resultsFeedbackThread_!=std::this_thread::get_id()||
       application.race28C()!=race.get()||!race->counted_||race->rolledBack_||
       !race->entity_.sessionGeneration||race->application_!=application.entities()||
       (raRace?(!actual.raWorld||race->world74_!=actual.raWorld):
           (race->world74_||!actual.taOriginalWorld||race->taWorld74_!=actual.taOriginalWorld))||!race->results104_||
       !race->resultsFeedback_.expired())return;
    try {
        // Construction uses precisely the borrowed setup array forwarded to
        // Frame, not a caller-selected device or playerSource5C substitution.
        auto registration=std::shared_ptr<Registration>(new Registration(
            application,race,bank,actual.feedbackDevices));
        race->resultsFeedback_=registration;
        registration_=std::move(registration);
    }catch(...){
        // Allocation failure grants no capability and changes no source slot.
        // A future private Results tail will reject the absent owner normally.
    }
}

void Scope::revoke()noexcept {
    if(!registration_)return;
    // Scope cannot move or escape Context::run; closeWindow and its fallback
    // destructor run on the creating thread. Clear has no callbacks/yields,
    // so same-thread consumption cannot overlap this revocation.
    registration_->active_=false;
    registration_->devices_=nullptr;
    if(const auto race=registration_->race_.lock())
        if(race->resultsFeedback_.lock()==registration_)race->resultsFeedback_.reset();
    registration_.reset();
}

Scope::~Scope()noexcept {revoke();}

bool Registration::clearHumans1AF770(const rr::game::OriginalRaceConstructor50LA& race,
    const native_racer50HN::Bank* bank,std::uint32_t actualHumanCount)noexcept {
    // Check immutable thread identity BEFORE reading mutable liveness/device
    // fields. Other threads have no borrow; no global mutex or device lease is
    // manufactured. The source caller and lifetime factory are synchronous.
    if(std::this_thread::get_id()!=thread_)return false;
    if(!active_||!devices_||!bank||bank!=bank_||!actualHumanCount||actualHumanCount>2u)return false;
    const auto retainedRace=race_.lock();
    if(retainedRace.get()!=&race||application_->race28C()!=&race||
       application_->entities()!=race.application_||!race.counted_||race.rolledBack_||
       race.entity_.sessionGeneration!=generation_||race.resultsFeedback_.lock().get()!=this)return false;
    // Exact1AF794..1AF7CC: device indices0..humanCount-1, in that order.
    //238870 is infallible and only zeros6CC/6D0 for argument2. In particular
    // it has NO connection/capability gate, hardware call, counter reset,
    // actuator write, input acknowledgment or renderer side effect.
    for(std::uint32_t index=0;index<actualHumanCount;++index)
        rr::game::clearControllerFeedback50KZ((*devices_)[index],2);
    return true;
}
}
