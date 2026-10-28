#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "../../src/game/original_race_f8_50la.h"
#include <bit>
#include <thread>

namespace original_main_entry50KG {
bool Application::updateConsumedF8199320(rr::game::OriginalRaceF8Diagnostic50LA& d)const noexcept {
    d={};const auto reject=[&](std::uint32_t pc,const char* why)noexcept {
        d.sourcePC=pc;d.operation=why;return false;
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return reject(0x140ce4,"F8 update requires the Application owner thread");
    // No public Race/timing/factor input. This is the SAME reached Root140CE0
    // publication already retained by Application, not a newly sampled clock.
    const auto& race=sourceRace28COwner_;
    if(!race||nativeTaRootRace_!=race.get()||!nativeTaRootEntered_||nativeTaRootAborted_||
       sourceUpdateRenderBypass228Bit3_||sourceApplicationMode22C_!=2||!sourceApplicationElapsed23C_||
       !nativeTaRootController_||race->nativeIntroControllerSlot48()!=nativeTaRootController_||
       race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_||
       !race->f8Attached_||!race->f8Owner_||race->f8Source_!=race->f8Owner_.get()||
       race->entity_.sessionGeneration!=nativeTaRootGeneration_||
       !race->f8Owner_->belongsTo(entitiesOwner_,nativeTaRootGeneration_))
        return reject(0x140ce4,"F8 update requires the same active consumed Root/Race/F8");
    original_ui_source_timing50LH::Sample sample;std::uint64_t publication=0;
    if(!nativeTaRootTiming_.readPrepared(sample,publication)||publication!=nativeTaRootPublication_||
       std::bit_cast<std::uint32_t>(sample.updateSeconds)!=
           std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_))
        return reject(0x140ce4,"F8 requires actual capped23C, not raw250 or caller time");
    const auto* f8=race->f8Owner_->entity();
    if(!f8||f8->parent1C!=&race->entity_||f8->mask20!=0xffffffffu||
       (f8->flags29&13u)!=1u||f8->firstChild18||f8->descendants24)
        return reject(0x141f00,"F8 actual attached empty-child update admission");
    // Reached at the bounded native accepted-Root pre-Camera cut. Original
    // F8 attachment1AAF54 precedes controller48 attachment1AB1E8; no post-
    // Camera timer shift is introduced. This still does not implement the
    // original141ED0 dispatcher/renderer20(0), end flow or rendering, and
    // never synchronizes the separate Camera timer projection.
    return race->f8Owner_->update199320(*sourceApplicationElapsed23C_,publication,d);
}
}
