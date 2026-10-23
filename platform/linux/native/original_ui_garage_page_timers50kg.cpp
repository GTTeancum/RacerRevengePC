#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_emitters50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_toy_fields50kg.h"
#include "original_ui_garage_watto_owner50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::sourceTimerFrameCurrent(
    const OriginalUiGarageSession50KG& session)const noexcept {
    if(!issuer_||!grant_||!sourceEntryReturned1FAE00()||!managerAttached1D1C40_||
       !grant_->garageAttachedPageCurrent(*this,entity_)||session.page_.get()!=this||
       !session.sameRoot(*issuer_)||session.grant_!=grant_||session.podScene_!=scene4C0_||
       !scene4C0_||sceneBacking_.get()!=scene4C0_||!session.actors_||!session.runtimeFrame_||
       scene4C0_->actorOwner3C8_!=session.actors_.get()||
       !session.actors_->scenePopulationCurrent2742F0(*scene4C0_)||
       session.runtimeManagerChild_!=&entity_||
       session.runtimePhase_!=OriginalUiGarageSession50KG::RuntimePhase::managerChildren||
       !shared_.manager270||*shared_.manager270!=grant_->manager()||
       entity_.parent1C!=grant_->manager()||!updatePublication_||updateFailed_||updateReturned_||
       updatedPageChildren_||std::fegetround()!=FE_TONEAREST)return false;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    float dt=0;std::uint32_t pause=0;
    return session.entryTiming_.readPrepared(sample,serial)&&serial==updatePublication_&&
        session.actors_->currentRuntimeFrame(*session.runtimeFrame_,dt,pause)&&
        dt==sample.updateSeconds&&std::isfinite(dt)&&dt>=0.f&&dt<=std::bit_cast<float>(0x3d75c28fu);
}
bool OriginalUiGaragePageOwner50KG::runPositiveTimer1FB708(
    const OriginalUiGarageSession50KG& session,std::size_t ordinal,Diagnostic& d)noexcept {
    constexpr std::uint32_t calls[]{0x1FB708,0x1FB8B8,0x1FB9A4};
    constexpr std::uint32_t clears[]{0x1FB870,0x1FB950,0x1FBA04};
    if(ordinal>=3||!sourceTimerFrameCurrent(session)||timerCallbackPending_||speechStopPending_||
       timerCallbackPublication_>updatePublication_||
       (timerCallbackPublication_==updatePublication_&&ordinal<=timerCallbackOrdinal_)||
       speechStopPublication_==updatePublication_||!enabled34C8_[ordinal==2?1:0]||
       !std::isfinite(std::bit_cast<float>(timer34B4_[ordinal]))||
       std::bit_cast<float>(timer34B4_[ordinal])>0.f)
        return fail(d,Status::runtimePending,ordinal<3?calls[ordinal]:0x1FB6A0);
    // The enclosing source branch already stored old-dt. Never re-subtract,
    // re-arm or borrow a second numeric timer bank. A failed prefix stays put.
    timerCallbackPublication_=updatePublication_;timerCallbackOrdinal_=ordinal;
    timerCallbackStep_=0;timerCallbackPending_=true;d.sourceEffectsCommitted=true;
    auto stop=[&](std::uint32_t pc)noexcept {
        updateFailed_=true;updateSourcePC_=pc;d.status=Status::runtimePending;
        d.sourcePC=pc;d.sourceEffectsCommitted=true;return false;
    };
    auto actor=[&](const char* name,const char* sequence,std::uint32_t pc)noexcept {
        updateSourcePC_=pc;
        if(!session.actors_->trigger101C10(name,sequence,d.loadResources))
            return stop(d.loadResources.sourcePC?d.loadResources.sourcePC:pc);
        ++timerCallbackStep_;return true;
    };
    auto emitter=[&](const char* name,std::uint32_t lookup,std::uint32_t reset)noexcept {
        updateSourcePC_=lookup;
        if(!scene4C0_->runPageTimerEmitter27E050(*this,name,lookup,reset,d.loadResources))
            return stop(d.loadResources.sourcePC?d.loadResources.sourcePC:lookup);
        return true;
    };
    if(ordinal==0){
        if(!actor("pd_fire","RUN",0x1FB708)||
           !emitter("pd_fire.emt",0x1FB724,0x1FB740)||
           !emitter("pd_smoke.emt",0x1FB77C,0x1FB798)||
           !emitter("garageExplosion.emt",0x1FB7D4,0x1FB7F0)||
           !emitter("garageExplSparks.emt",0x1FB82C,0x1FB848))return false;
    }else if(ordinal==1){
        if(!actor("pd_ext","RUN",0x1FB8B8)||!actor("nozzle","RUN",0x1FB8D4)||
           !actor("tank","RUN",0x1FB8F0)||!emitter("pd_ext.emt",0x1FB90C,0x1FB928))return false;
    }else if(!actor("pd_fall","FALL",0x1FB9A4)||
             !emitter("pd_fall.emt",0x1FB9C0,0x1FB9DC))return false;
    timer34B4_[ordinal]=0; // Actual zero store, only after every source return.
    timerCallbackPending_=false;updateSourcePC_=clears[ordinal];d.sourcePC=clears[ordinal];
    return true;
}
bool OriginalUiGaragePodScene50KG::runPageTimerEmitter27E050(
    OriginalUiGaragePageOwner50KG& page,const char* name,std::uint32_t lookupPC,
    std::uint32_t resetPC,D& d)noexcept {
    using S=assets::OriginalStandardMaterialStatus50LA;
    d={};auto reject=[&](std::uint32_t pc)noexcept {d.status=S::phase;d.sourcePC=pc;return false;};
    if(!name||!*name||!grant_||!sceneEmitters_||!toyFields_||!toyFields_->particles3D8_||
       !sceneParticles_||!sceneParticles_->numeric||!sceneParticles_->numeric->prepared()||
       !sceneParticles_->assetsOwner||!nativeApplication_||page.grant_!=grant_||
       page.scene4C0_!=this||page.sceneBacking_.get()!=this||!page.timerCallbackPending_||
       !page.updatePublication_||page.timerCallbackPublication_!=page.updatePublication_||
       page.updateFailed_||page.updateReturned_||page.updatedPageChildren_||
       !sourceLoadReturned1FB59C())return reject(lookupPC);
    auto& owned=*sceneEmitters_;auto& list=*toyFields_->particles3D8_;
    const auto* host=grant_->issuer();
    if(!host||page.issuer_!=host||host->state!=nativeApplication_||
       !grant_->garageLateScenePageChildCurrent(page,*this,0,list.entity)||
       page.sceneLateChildren_[0]!=&list.entity||!owned.complete||owned.failed||owned.retired||
       owned.listRetiring||owned.listRetired||owned.updateFailed||
       owned.updatePublication==page.updatePublication_||!list.live||list.count30!=7||!list.records34)
        return reject(lookupPC);
    ++page.timerCallbackStep_; // Genuine list+7C invocation, including null return.
    // Source27E050 uses309BA8 (strstr), not case-folding or basename equality.
    // Walk actual wrappers in list order; all admitted constructors published
    // exactly one live instance at wrapper+0C and retained full filename78.
    for(std::size_t i=0;i<owned.rows.size();++i){
        const auto* wrapper=list.records34[i];auto* row=owned.rows[i].get();
        if(!wrapper||wrapper!=list.records[i].get()||!wrapper->live00||wrapper->temp04||
           !wrapper->unread08To10[1]||*wrapper->unread08To10[1]!=1||!row||
           wrapper->live00[0]!=&row->entity||!row->entityLive||!row->numericConstructed||
           !row->loaded||!row->attached||row->retiring||row->retired||
           !row->filenameKnown||row->filenameKnown>row->filename78.size()||
           row->filename78[row->filenameKnown-1]||
           std::memchr(row->filename78.data(),0,row->filenameKnown-1))return reject(0x27E050);
        if(!std::strstr(row->filename78.data(),name))continue;
        if(row->entity.parent1C!=&list.entity||row->entity.firstChild18||row->entity.mask20!=0x402u||
           row->manager70!=&sceneParticles_->entity||!sceneParticles_->entityLive||
           !grant_->preflightGarageSceneEmitter15A720(this,i,row->entity).okay())return reject(resetPC);
        const auto id=static_cast<game::OriginalUiGarageEmitter50KG>(i);
        page.updateSourcePC_=resetPC;d.sourceEffectsCommitted=true;
        const auto reset=sceneParticles_->numeric->reset88(id,nativeApplication_->rawRandom308030);
        if(!reset.okay()){d.status=S::source;d.sourcePC=reset.sourcePC;return false;}
        ++page.timerCallbackStep_;page.updateSourcePC_=resetPC+0x10;
        // Real base+20 on this enrolled childless Entity. Its actual+2C has
        // no children to visit. Mirror the same enabled state numerically.
        const auto enabled=grant_->enableGarageSceneEmitter27D7A0(this,i,row->entity,true);
        if(!enabled.okay()||!sceneParticles_->numeric->enable20(id)){
            d.status=S::source;d.sourcePC=0x141560;return false;}
        ++page.timerCallbackStep_;page.updateSourcePC_=resetPC+0x20;
        if(!sceneParticles_->numeric->start80(id)){d.status=S::source;d.sourcePC=0x15ACE0;return false;}
        ++page.timerCallbackStep_;d.sourcePC=resetPC+0x20;return true;
    }
    // A genuine no-match return skips all three calls, exactly as the Page
    // does. It does not fabricate an emitter or a completed derived update.
    d.sourcePC=lookupPC;return true;
}
bool OriginalUiGaragePageOwner50KG::stopSpeechTimer2924A0(
    const OriginalUiGarageSession50KG& session,Diagnostic& d)noexcept {
    if(!sourceTimerFrameCurrent(session)||timerCallbackPending_||speechStopPending_||
       speechStopPublication_>=updatePublication_||!watto33B0_||!entryField4E0_||!*entryField4E0_||
       !std::isfinite(std::bit_cast<float>(timer34B4_[4]))||std::bit_cast<float>(timer34B4_[4])>0.f)
        return fail(d,Status::runtimePending,0x1FBA80);
    speechStopPublication_=updatePublication_;speechStopPending_=true;updateSourcePC_=0x1FBA80;
    d.sourceEffectsCommitted=true;
    if(!watto33B0_->stopPageSpeech2924A0(*this,d.loadResources)){
        updateFailed_=true;updateSourcePC_=d.loadResources.sourcePC?d.loadResources.sourcePC:0x2924A0;
        d.status=Status::runtimePending;d.sourcePC=updateSourcePC_;return false;}
    speechStopPending_=false;d.sourcePC=0x1FBA80;
    // No source write clears34C4,4E0,readyA4,exitA0 or the current motion.
    return true;
}
} // namespace rr::platform
