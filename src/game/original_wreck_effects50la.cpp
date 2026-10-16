#include "original_wreck_effects50la.h"
#include "original_camera_start_native.h"
#include <cstring>

namespace rr::game {
namespace {
using S=OriginalGcSprayStatus50KG;
using Slot=OriginalWreckEffectSlot50LA;
using Kind=OriginalWreckComponentKind50LA;
OriginalGcSprayReceipt50KG receipt(S s,std::uint32_t pc)noexcept {
    OriginalGcSprayReceipt50KG r;r.status=s;r.sourcePC=pc;return r;
}
float bits(std::uint32_t u)noexcept {float f;std::memcpy(&f,&u,4);return f;}
bool commonStart(void* p,EffectControlState50BC& c)noexcept {
    auto* owner=static_cast<EffectOwner50BE*>(p);if(!owner||&owner->state!=&c)return false;
    startEffect50BC(c);return true;
}
bool commonStop(void* p,EffectControlState50BC& c)noexcept {
    auto* owner=static_cast<EffectOwner50BE*>(p);if(!owner||&owner->state!=&c)return false;
    if(owner->profile==EffectProfile50BE::Deferred15E800)
        return stopDeferredEffect50BD(c,owner->mode1D8,owner->duration1E4)!=EffectStopResult50BD::UnsupportedDuration;
    stopEffect50BC(c);return true;
}
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::result(S status,std::uint32_t pc)const noexcept {
    return {status,pc,constructedChildren_,initializedChildren_!=0};
}
const OriginalGcCommonEmitterTiming50KG& OriginalWreckEffects50LA::timing(unsigned i)const noexcept {
    if(i==0)return dustDefinition_.timing;if(i==1)return splashDefinition_.timing;
    if(i==2)return sparksDefinition_.timing;return explosionDefinition_.timing;
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::prepareCold(
    const OriginalWreckEffectResourceContext50LA& c,assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};if(preparationAttempted_)return result(S::alreadyPrepared,0x2cb7e4);
    preparationAttempted_=true;
    if(!c.particleManager||!c.particleManager->prepared()||!c.podWreckDustEmt||
       !c.podWreckSplashEmt||!c.podWreckSparksEmt||!c.podExplosionEmt||
       !c.sharedRawRandom||!c.sharedRandomOwner||!c.hostClock||!c.sparksGlobals||!c.explosionGlobals)
        return result(S::unprepared,0x2cb618);
    auto s=readOriginalGcPodDustDefinition50KG(*c.podWreckDustEmt,
        OriginalGcPodDustMember50KG::wreckDust,dustDefinition_,d);
    if(s!=S::complete)return result(s,d.sourcePC?d.sourcePC:0x19b060);
    s=readOriginalWreckSplashDefinition50LA(*c.podWreckSplashEmt,splashDefinition_,d);
    if(s!=S::complete)return result(s,d.sourcePC?d.sourcePC:0x19e9f0);
    s=readOriginalGcPodSparksDefinition50KG(*c.podWreckSparksEmt,
        OriginalGcPodSparksMember50KG::wreckSparks,sparksDefinition_,d);
    if(s!=S::complete)return result(s,d.sourcePC?d.sourcePC:0x15fc10);
    s=readOriginalWreckExplosionDefinition50LA(*c.podExplosionEmt,explosionDefinition_,d);
    if(s!=S::complete)return result(s,d.sourcePC?d.sourcePC:0x15ef70);
    context_=c;prepared_=true;return result(S::complete,0);
}
bool OriginalWreckEffects50LA::binding(const OriginalWreckEffectComponentBinding50LA& b,bool same)const noexcept {
    using namespace original_wreck_component_detail50LA;
    if(!b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||
       b.domain->count>b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||
       b.domainCapacity-b.domain->count<4-initializedChildren_||!domain(*b.domain)||
       !b.particleParent||!contains(*b.domain,b.particleParent)||!b.actualObjectCount423F60||
       !b.actualAttachmentLock423F58)return false;
    if(same&&(b.domain!=binding_.domain||b.domainStorage!=binding_.domainStorage||
       b.domainCapacity!=binding_.domainCapacity||b.particleParent!=binding_.particleParent||
       b.actualObjectCount423F60!=binding_.actualObjectCount423F60||
       b.actualAttachmentLock423F58!=binding_.actualAttachmentLock423F58||
       b.componentKeys!=binding_.componentKeys||b.controlOwnerIds!=binding_.controlOwnerIds))return false;
    for(unsigned i=0;i<4;++i){
        if(!b.componentKeys[i]||!b.controlOwnerIds[i])return false;
        for(unsigned j=0;j<i;++j)if(b.componentKeys[i]==b.componentKeys[j]||b.controlOwnerIds[i]==b.controlOwnerIds[j])return false;
        for(std::size_t j=0;j<b.domain->count;++j){const auto* existing=b.domain->objects[j];
            if(existing->objectKey==b.componentKeys[i]&&existing!=&components_[i])return false;}
    }
    return true;
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::constructOne(unsigned i) noexcept {
    const Kind kinds[]={Kind::effect19A5E0,Kind::effect19DBE0,Kind::effect15F100,Kind::effect15E800};
    const EffectProfile50BE profiles[]={EffectProfile50BE::Particle19A5E0,EffectProfile50BE::Splash19DBE0,
        EffectProfile50BE::Ordinary15F100,EffectProfile50BE::Deferred15E800};
    const std::vector<std::uint8_t>* members[]={context_.podWreckDustEmt,context_.podWreckSplashEmt,
        context_.podWreckSparksEmt,context_.podExplosionEmt};
    const std::uint32_t appendPC[]={0x2cb84c,0x2cb8bc,0x2cc118,0x2cc188};
    auto fail=[&](S status,std::uint32_t pc){constructionFailed_=true;return result(status,pc);};
    if(i!=initializedChildren_||i!=constructedChildren_||!binding(binding_,true))return fail(S::storage,0x141300);
    auto& d=*binding_.domain;
    const auto initialized=initializeOriginalWreckComponent50LA(components_[i],kinds[i],
        d.sessionGeneration,binding_.componentKeys[i],true,*binding_.actualObjectCount423F60);
    if(initialized.status!=OriginalWreckComponentStatus50LA::complete)return fail(S::storage,initialized.sourcePC);
    ++initializedChildren_;
    // Publish only this genuinely initialized source object into the existing
    // external domain view. The next three unconstructed slots stay absent.
    binding_.domainStorage[d.count++]=&components_[i];
    if(i>=2){
        // Actual15F220/15E930 attachment74 and copied15F228/15E938 offset50.
        //2CC098..AC created the stack vector0,0,0,1 before either constructor.
        auto& attachment=groupAttachments_[i-2];
        attachment.graph=&groupAttachmentGraph_;attachment.node=modelAssets_->model14C();
        attachment.offset50={{0,0,0,1}};
    }
    const auto& bytes=*members[i];
    if(!assets::buildEffectControlOwner50BJ(binding_.controlOwnerIds[i],profiles[i],
       reinterpret_cast<const char*>(bytes.data()),bytes.size(),{},owners_[i]))return fail(S::authoredData,0x15aea0);
    delays1DC_[i]=timing(i).delayTime1E0;
    OriginalGcSprayReceipt50KG r;
    if(i==0)r=bindOriginalGcPodDust50KG(dust_,dustDefinition_,*context_.particleManager,owners_[i]);
    else if(i==1)r=bindOriginalWreckSplash50LA(splash_,splashDefinition_,*context_.particleManager,owners_[i]);
    else if(i==2)r=bindOriginalGcPodSparks50KG(sparks_,sparksDefinition_,*context_.particleManager,owners_[i]);
    else r=bindOriginalWreckExplosion50LA(explosion_,explosionDefinition_,*context_.particleManager,owners_[i]);
    if(!r.okay())return fail(r.status,r.sourcePC);
    // buildEffectControlOwner50BJ performed the source immediate Stop84 once;
    // no shadow bank or repeated Stop/Reset invocation is introduced here.
    const auto appended=appendOriginalWreckComponent50LA(d,*binding_.particleParent,components_[i],
        0x0402u,*binding_.actualAttachmentLock423F58);
    if(appended.status!=OriginalWreckComponentStatus50LA::complete)return fail(S::storage,appended.sourcePC);
    ++constructedChildren_;return result(S::complete,appendPC[i]);
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::constructPiece(
    const OriginalWreckEffectComponentBinding50LA& b,std::uint32_t count174) noexcept {
    if(!prepared_)return result(S::unprepared,0x2cb618);
    if(constructionFailed_||initializedChildren_)return result(S::alreadyPrepared,0x2cb7e4);
    if(count174)return result(S::unsupported,0x2cb628);
    if(!binding(b,false))return result(S::storage,0x141300);
    binding_=b;
    auto r=constructOne(0);if(!r.okay())return r;
    return constructOne(1);
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::constructGroup(
    const OriginalWreckEffectComponentBinding50LA& b,OriginalWreckAssets50LA& assets,
    OriginalWreckVisibility50LA& visibility,const OriginalWreckEffectRendererBinding50LA& renderer) noexcept {
    if(!prepared_||constructedChildren_!=2||constructionFailed_)return result(S::unprepared,0x2cc098);
    if(!binding(b,true)||!assets.prepared()||!visibility.prepared()||!visibility.boundTo(assets))
        return result(S::storage,0x2cc0f0);
    const auto graph=assets.graph();const auto node=static_cast<std::int32_t>(assets.model14C());
    if(!validateSceneGraph49P(graph)||node<0||static_cast<std::uint32_t>(node)>=graph.count)
        return result(S::storage,0x2cc0f0);
    modelAssets_=&assets;modelVisibility_=&visibility;renderer_=renderer;groupAttachmentGraph_=graph;
    auto r=constructOne(2);if(!r.okay())return r;
    return constructOne(3);
}
bool OriginalWreckEffects50LA::coherent()const noexcept {
    using namespace original_wreck_component_detail50LA;
    if(!prepared_||constructionFailed_||runtimeFailed_||constructedChildren_!=4||initializedChildren_!=4||
       !binding_.domain||binding_.domain->objects!=binding_.domainStorage||
       binding_.domain->count>binding_.domainCapacity||!domain(*binding_.domain)||
       !contains(*binding_.domain,binding_.particleParent)||!context_.particleManager||!context_.particleManager->prepared()||
       !modelAssets_||!modelAssets_->prepared()||!modelVisibility_||!modelVisibility_->boundTo(*modelAssets_))return false;
    const Kind kinds[]={Kind::effect19A5E0,Kind::effect19DBE0,Kind::effect15F100,Kind::effect15E800};
    const EffectProfile50BE profiles[]={EffectProfile50BE::Particle19A5E0,EffectProfile50BE::Splash19DBE0,
        EffectProfile50BE::Ordinary15F100,EffectProfile50BE::Deferred15E800};
    for(unsigned i=0;i<4;++i){const auto& c=components_[i];
        if(!contains(*binding_.domain,&c)||c.kind!=kinds[i]||c.objectKey!=binding_.componentKeys[i]||
           c.sessionGeneration!=binding_.domain->sessionGeneration||c.parent1C!=binding_.particleParent||
           c.mask20!=0x0402u||owners_[i].id!=binding_.controlOwnerIds[i]||owners_[i].profile!=profiles[i]||
           owners_[i].mode1D8!=timing(i).mode1D8)return false;}
    return true;
}
EffectOwner50BE* OriginalWreckEffects50LA::control(Slot slot) noexcept {
    const auto i=static_cast<unsigned>(slot);return i<constructedChildren_?&owners_[i]:nullptr;
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::start80(Slot slot) noexcept {
    auto* p=control(slot);if(!p||constructionFailed_||runtimeFailed_)return result(S::unprepared,0x15ace0);
    startEffect50BC(p->state);return result(S::complete,0x15acf8);
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::stop84(Slot slot) noexcept {
    auto* p=control(slot);if(!p||constructionFailed_||runtimeFailed_)return result(S::unprepared,0x15ad00);
    if(!commonStop(p,p->state))return result(S::unsupported,0x15f070);
    return result(S::complete,p->profile==EffectProfile50BE::Deferred15E800?0x15f0b8u:0x15ad40u);
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::reset88(Slot slot) noexcept {
    const auto i=static_cast<unsigned>(slot);auto* p=control(slot);
    if(!p||constructionFailed_||runtimeFailed_)return result(S::unprepared,0x15ad50);
    const auto& t=timing(i);
    if(p->mode1D8!=t.mode1D8||t.mode1D8>1)return result(S::unsupported,0x15ad6c);
    delays1DC_[i]=t.delayTime1E0;
    if(t.mode1D8==1)std::memcpy(&p->duration1E4,&t.maxTimeOn1EC,4);
    return result(S::complete,0x15ade0);
}
OriginalWreckEffectsReceipt50LA OriginalWreckEffects50LA::deactivateGroupEffects() noexcept {
    if(!groupConstructed())return result(S::unprepared,0x2cc9c0);
    for(unsigned i=2;i<4;++i){const auto s=static_cast<Slot>(i);
        auto r=reset88(s);if(!r.okay())return r;
        r=stop84(s);if(!r.okay())return r;
    }
    auto r=stop84(Slot::splash16C);if(!r.okay())return r;
    //2CCA54 group141520 belongs to the actual caller's group component.
    return result(S::complete,0x2cca50);
}
bool OriginalWreckEffects50LA::rendererGate(OriginalWreckExplosionAttachmentGate50LA& out)const noexcept {
    if(!modelAssets_||!modelVisibility_||!modelVisibility_->prepared()||
       !modelVisibility_->boundTo(*modelAssets_))return false;
    OriginalWreckExplosionAttachmentGate50LA next;
    next.sourceBound=true;next.attachment74Present=true;
    next.attachment19CPresent=modelVisibility_->attached();
    if(next.attachment19CPresent){
        if(!renderer_.actualChild04TagA4Field||!*renderer_.actualChild04TagA4Field||
           !renderer_.actualCurrentTag8BE0)return false;
        next.attachment19CChild04Bound=true;
        next.currentTag8BE0=*renderer_.actualCurrentTag8BE0;
        next.attachment19CChild04TagA4=**renderer_.actualChild04TagA4Field;
    }
    out=next;return true;
}
OriginalGcSprayReceipt50KG OriginalWreckEffects50LA::advanceRegisteredChild(
    OriginalWreckComponent50LA& child,float dt,bool paused,const OriginalRendererDispatchFields& fields,
    platform::VehicleWindowGl50DS& renderer,platform::VehicleWindowDiagnostic50DS& diagnostic) noexcept {
    if(runtimeFailed_)return stoppedAt_;
    unsigned i=0;while(i<constructedChildren_&& &components_[i]!=&child)++i;
    if(i>=constructedChildren_||constructionFailed_||child.parent1C!=binding_.particleParent||
       child.mask20!=0x0402u)return receipt(S::unprepared,0x141ed0);
    if(!originalWreckChildEligible50LA(child)){
        auto r=receipt(S::complete,0x141f68);r.commonSkipped=true;return r;}
    std::int32_t actualV0=0;
    const auto service=serviceOriginalRenderer244D60(fields,false,renderer,diagnostic,actualV0);
    if(service.status!=OriginalCameraStartStatus::complete)return receipt(S::unsupported,service.sourcePC);
    return advanceRegisteredChildAfterParentDispatch(child,dt,paused);
}
OriginalGcSprayReceipt50KG OriginalWreckEffects50LA::advanceRegisteredChildAfterParentDispatch(
    OriginalWreckComponent50LA& child,float dt,bool paused) noexcept {
    if(runtimeFailed_)return stoppedAt_;
    unsigned i=0;while(i<constructedChildren_&& &components_[i]!=&child)++i;
    if(i>=constructedChildren_||constructionFailed_||!binding_.domain||
       child.parent1C!=binding_.particleParent||child.mask20!=0x0402u||
       !originalWreckChildEligible50LA(child))return receipt(S::unprepared,0x141ed0);
    auto fail=[&](OriginalGcSprayReceipt50KG r){runtimeFailed_=true;stoppedAt_=r;return r;};
    auto& owner=owners_[i];const OriginalGcCommonEmitterDispatch50KG calls{&owner,commonStart,commonStop};
    const OriginalGcCommonAttachment50KG* attachment=nullptr;
    if(i>=2){
        if(!modelAssets_||!modelAssets_->prepared())return fail(receipt(S::unprepared,0x15a810));
        const auto graph=modelAssets_->graph();attachment=&groupAttachments_[i-2];
        if(graph.nodes!=groupAttachmentGraph_.nodes||graph.count!=groupAttachmentGraph_.count||
           attachment->graph!=&groupAttachmentGraph_||attachment->node!=modelAssets_->model14C())
            return fail(receipt(S::storage,0x15a810));
    }
    auto common=advanceOriginalGcCommonEmitter50KG(owner.state,timing(i),delays1DC_[i],
        owner.duration1E4,i>=2,dt,paused,*context_.sharedRawRandom,&calls,attachment);
    if(!common.okay())return fail(common);
    if(common.commonSkipped)return common;
    OriginalGcSprayReceipt50KG r;
    if(i==0)r=advanceOriginalGcPodDustAfterCommon50KG(dust_,dustDefinition_,*context_.particleManager,
        owner,dt,*context_.sharedRawRandom,*context_.sharedRandomOwner,*context_.hostClock);
    else if(i==1)r=advanceOriginalWreckSplashAfterCommon50LA(splash_,splashDefinition_,*context_.particleManager,
        owner,dt,*context_.sharedRawRandom,*context_.sharedRandomOwner,*context_.hostClock);
    else {
        OriginalWreckExplosionAttachmentGate50LA gate;
        //15F3C0's stale timer early return precedes the sparks visibility gate.
        const bool needGate=i==3||!(bits(owner.state.word19C)>.25f);
        if(needGate&&!rendererGate(gate)){
            r=receipt(S::unprepared,i==2?0x15f410u:0x15eb28u);r.randomDraws=common.randomDraws;return fail(r);}
        if(i==2){const OriginalGcPodSparksAttachmentGate50KG sparksGate{gate.sourceBound,
            gate.attachment74Present,gate.attachment19CPresent,gate.attachment19CChild04Bound,
            gate.attachment19CChild04TagA4,gate.currentTag8BE0};
            r=advanceOriginalGcPodSparksAfterCommon50KG(sparks_,sparksDefinition_,*context_.particleManager,
                owner,dt,*context_.sharedRawRandom,*context_.sparksGlobals,&sparksGate);
        }else r=advanceOriginalWreckExplosionAfterCommon50LA(explosion_,explosionDefinition_,*context_.particleManager,
            owner,dt,*context_.sharedRawRandom,*context_.explosionGlobals,gate);
    }
    r.randomDraws+=common.randomDraws;
    if(!r.okay())return fail(r);return r;
}
} // namespace rr::game
