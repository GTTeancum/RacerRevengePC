#include "original_ui_main_entry50kg.h"
#include "original_camera_session50kx.h"
#include "original_race_constructor50la.h"
#include "common_race_world50cr.h"
#include "../../src/game/original_race_f8_storage50la.h"
#include "../../src/game/original_session_f8_intro50la.h"
#include <thread>
#include <cstdio>

namespace rr::game {
bool OriginalSessionF8Intro50LA::reveal(std::uint8_t& projection,bool direct)const noexcept {
    // Immutable construction-thread identity precedes every Session read.
    // The Session must still be alive; this is not cross-thread synchronization.
    if(ownerThread_!=std::this_thread::get_id())return false;
    auto& session=*session_;
    if(&projection!=&session.uiFlags1E4_||
       session.frameStage_!=original_camera_session50KX::Session::FrameStage::exit||
       session.directStartPending_!=direct||(!direct&&session.taExitReason_>3))return false;
    const auto& issuance=session.nativeTaIssuance_;
    if(!issuance){
        // Preserve existing standalone/unissued numeric behavior. An unissued
        // session gains no canonical F8 authority from a projection.
        revealOriginalRaceUi199770(projection);return true;
    }
    // Every Application-issued course session (one rule, B-C06): the race's
    // real F8 199770. Its issuance retains the Application lifetime observation. Check
    // it before dereferencing the issuer; a failed actual-owner join NEVER
    // falls back to writing only the native projection and reporting success.
    std::uint32_t ignoredPause=0;
    if(!issuance->issuer||!session.actual_.applicationAnimation.readPause11C(ignoredPause))return false;
    return issuance->issuer->revealConsumedSessionF8Intro50LA(session,direct);
}
}

namespace original_main_entry50KG {
bool Application::revealConsumedSessionF8Intro50LA(original_camera_session50KX::Session& session,
    bool direct)const noexcept {
    const auto refused=[&](const char* field){
        std::fprintf(stderr,"race_intro_reveal_refused field=%s humans=%u race_flags=%u camera_count=%u race_mode=%d display_mode=%d\n",
            field,unsigned(session.actual_.humanCount34),unsigned(session.raceFlags1B0_),
            unsigned(session.display_.cameraCount264),int(session.raceMode8C_),int(session.display_.viewportMode8C));
        return false;
    };
    // Existing immutable Application fence, before shared/weak owners or
    // Session/Entity state. The caller's concrete service fenced its Session.
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto race=sourceRace28COwner_;
    if(!enabled||!race||raceConstructorColdOwner50LA_.lock()!=race||!entitiesOwner_||
       race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_||!race->world74_||
       !race->f8Attached_||!race->f8Owner_||race->f8Source_!=race->f8Owner_.get())return refused("race");
    const auto& issued=session.nativeTaIssuance_;
    if(!issued||issued->issuer!=this||issued->race!=race.get()||issued->world!=race->world74_.get()||
       issued->generation!=session.generation_||session.generation_!=race->entity_.sessionGeneration||
       !session.coldBound_||session.closed_||session.abandoned_||!session.actual_.humanCount34||session.actual_.humanCount34>2u||
       (session.raceFlags1B0_&3u)!=session.actual_.humanCount34||
       session.display_.cameraCount264!=session.actual_.humanCount34||
       session.actual_.applicationEntities!=entitiesOwner_||
       !session.actual_.applicationAnimation.sameOwner(animationClock())||
       session.frameStage_!=original_camera_session50KX::Session::FrameStage::exit||
       session.directStartPending_!=direct||(!direct&&session.taExitReason_>3))return refused("session");
    // The race's own issued Session (strong borrow for the leaf) on its counted
    // World. A separately constructed Session, copied generation or legacy
    // scene key is not this producer.
    const auto actual=issuedCourseCameraSession50LA();
    if(!actual||actual.get()!=&session||!race->ownsNativeCameraAdapter34(session))return refused("camera");
    auto& source=*race->f8Owner_;
    if(!source.loadedCold()||!source.belongsTo(entitiesOwner_,session.generation_))return refused("f8");
    const auto* entity=source.entity();
    if(!entity||entity->parent1C!=&race->entity_||entity->mask20!=0xffffffffu)return refused("entity");
    // Direct176330 and natural/skip/no7C1766EC/1765D8/17652C/175D30 all
    // call(0,1,1,1,1). Immediate1 preserves source1E0; there is no copied
    // Session timer or raw frameDelta.
    // The canonical leaf rejects active native bar borrows before any store.
    if(!source.visibility199770(0,1,1,1,1))return refused("visibility");
    // One-way native HUD projection only, after the genuine canonical setter.
    // Preserve unrepresented upper projection lanes; never copy them to F8.
    session.uiFlags1E4_=std::uint8_t((session.uiFlags1E4_&0xe0u)|
        (source.impl_->fields->flags1E4Low5&0x1fu));
    // No fallible work follows the committed setter. The calling real cursor
    // advances its exact cut once; later cooldown/audio failure cannot replay it.
    return true;
}
}
