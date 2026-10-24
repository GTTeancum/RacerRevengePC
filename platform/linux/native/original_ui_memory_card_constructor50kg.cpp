#include "original_ui_memory_card_constructor50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <exception>
#include <limits>
namespace rr::platform {
OriginalUiMemoryCardConstructor50KG::~OriginalUiMemoryCardConstructor50KG()noexcept {
    if(entity_.reference00||constructedQuads_!=retiredQuads_||displayPinned_||
       phase_==Phase::detachedConstructed||phase_==Phase::retiring)std::terminate();
}
bool OriginalUiMemoryCardConstructor50KG::inputsCurrent()const noexcept {
    return root_&&root_->memoryCardConstructorOwner_.get()==this&&scope_&&scope_->issuer_==root_&&root_->sharedUiScopeOwner_==scope_&&
        scope_->live().okay()&&domain_&&domain_==root_->standardMaterialDomainOwner_&&domain_->prepared()&&
        context_&&context_==root_->materialContextOwner_&&context_->live()&&
        video_&&video_==root_->videoOwner_&&video_->originalMain&&video_->packetZMaximum==0xffffff&&
        context_->applicationAnimationClock().sameOwner(root_->animationClock())&&
        root_->materialApplication&&context_->materialApplication()==root_->materialApplication&&
        domain_->materials382B38()->screenApplicationMatches(*root_->materialApplication);
}
bool OriginalUiMemoryCardConstructor50KG::constructDetached1FEC80(const Root& root,
    std::shared_ptr<OriginalUiMemoryCardConstructor50KG>& output,Diagnostic& d)noexcept {
    d={};if(output||root.memoryCardConstructorOwner_){d.sourcePC=0x1D6670;return false;}
    try {
        auto next=std::shared_ptr<OriginalUiMemoryCardConstructor50KG>(new OriginalUiMemoryCardConstructor50KG);
        next->root_=&root;next->scope_=root.sharedUiScopeOwner_;
        next->domain_=root.standardMaterialDomainOwner_;next->context_=root.materialContextOwner_;next->video_=root.videoOwner_;
        next->phase_=Phase::bound;
        // Establish real cleanup custody BEFORE any counted source effect.
        const auto display=root.raceDisplayOwner50LA_.lock();
        if(!display||display->application_!=&root||display->video_!=next->video_||
           !display->frontendOwnerCurrent50KG()||!display->canRetain()){
            d.sourcePC=0x1D6670;return false;
        }
        next->display_=display;display->pin();next->displayPinned_=true;
        root.memoryCardConstructorOwner_=next;
        const auto rejectBeforeCount=[&]()noexcept {
            display->unpin();next->displayPinned_=false;next->display_.reset();
            root.memoryCardConstructorOwner_.reset();next->phase_=Phase::empty;
        };
        if(!next->inputsCurrent()){rejectBeforeCount();d.sourcePC=0x1FEC80;return false;}
        d.entity=next->scope_->constructDetachedMemoryCard1FEC80(*next,next->entity_);
        if(!d.entity.okay()){rejectBeforeCount();d.sourcePC=d.entity.sourcePC;return false;}
        // All remaining constructor stores are nonfallible. No loader, callback
        // or allocation occurs after the real Entity count/enrollment.
        next->vtable_=0x3D0990;
        constexpr std::array<std::uint32_t,7> offsets{{0x40,0xA0,0x100,0x160,0x1C0,0x220,0x280}};
        for(std::size_t i=0;i<offsets.size();++i){
            if(!next->quads_[i].constructMemoryCardPrefix2DEF40(offsets[i]))std::terminate();
            ++next->constructedQuads_;
        }
        next->ids3BC_.fill(-1);next->timer398_=0;next->completed3A4_=false;next->animate3B0_=true;
        next->phase_=Phase::detachedConstructed;
        output=std::move(next);d.sourceEffectsCommitted=true;d.sourcePC=0x1FEE38;return true;
    }catch(...){d.sourcePC=0x1D6670;return false;}
}
bool OriginalUiMemoryCardConstructor50KG::resourcePrefixCurrent()const noexcept {
    return inputsCurrent()&&(root_->materialDirectory36B970_=="ui\\textures\\"||root_->materialDirectory36B970_=="ui\\textures")&&
        phase_==Phase::detachedConstructed&&display_&&displayPinned_&&
        display_->application_==root_&&display_->video_==video_&&!display_->retiring_&&
        display_->memoryCardConstructorDrainCurrent50KG()&&
        scope_->detachedMemoryCardCurrent(*this,entity_,false);
}
bool OriginalUiMemoryCardConstructor50KG::firstIconOccurrenceCurrent(
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row)const noexcept {
    row=-1;if(!resourcePrefixCurrent()||!firstIconRetained_||constructedQuads_!=7||retiredQuads_||
       domain_->materials382B38().get()!=&table)return false;
    const auto& role=loadRoles_[0];const auto& quad=quads_[role.constructionIndex];
    if(!quad.memoryCardPrefixOnly_||quad.offset!=role.offset||quad.stage_!=Quad::Stage::materialRetained||
       quad.pendingPC_!=0x2DF144||!quad.constructed||quad.resource58||quad.packetOwner_||
       !quad.material54||quad.material54!=quad.primitiveOwner_.get()||
       !quad.material54->materialStored||!quad.material54->materialRetained||
       quad.material54->type0C!=15||(quad.material54->flags0F&1u)||
       quad.material54->domain!=domain_||quad.material54->context!=context_||
       quad.domain_!=domain_||quad.context_!=context_||quad.video_!=video_)return false;
    row=quad.material54->material00;return row>=0;
}
bool OriginalUiMemoryCardConstructor50KG::preflightExistingFirstIconBinding(Diagnostic& d)const noexcept {
    d={};if(!inputsCurrent()){d.sourcePC=0x2DF158;return false;}
    const auto table=domain_->materials382B38();
    const bool result=table&&table->preflightMemoryCardFirstIconBinding(*this,d.quad.material);
    d.resourcePopulationPending=result;d.sourcePC=d.quad.material.sourcePC;return result;
}
bool OriginalUiMemoryCardConstructor50KG::retainFirstIconPrefix1FF058(Diagnostic& d)noexcept {
    d={};
    if(!resourcePrefixCurrent()){d.sourcePC=0x1FF058;return false;}
    // True means only this component's reference prefix is represented. It
    // does not mean1FF058 returned: population and packet work remain pending.
    if(firstIconRetained_){d.sourceEffectsCommitted=true;d.resourcePopulationPending=true;d.sourcePC=0x2DF144;return true;}
    const bool result=quads_[loadRoles_[0].constructionIndex].retainMemoryCardFirstIcon2465B0(*this,d.quad);
    d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;d.resourcePopulationPending=result;d.sourcePC=d.quad.sourcePC;return result;
}
bool OriginalUiMemoryCardConstructor50KG::drainColdQuad2DEFE0(Diagnostic& d)noexcept {
    if(phase_!=Phase::retiring||drainPhase_>2||!inputsCurrent()||
       !scope_->detachedMemoryCardCurrent(*this,entity_,true)){d.sourcePC=0x2DF018;return false;}
    if(!display_||!displayPinned_){d.sourcePC=0x2DF018;return false;}
    auto& display=*display_;
    if(!displayPinned_||display.application_!=root_||display.video_!=video_||
       !display.memoryCardConstructorDrainCurrent50KG()){d.sourcePC=0x2DF018;return false;}
    if(!drainPhase_){std::int32_t actualV0=0;
        const auto result=game::serviceOriginalRenderer244D60(display.dispatch_,true,display.renderer_,d.renderer,actualV0,
            game::OriginalCameraDrainPolicy::initializedContext);
        if(result.status!=game::OriginalCameraStartStatus::complete){d.sourcePC=result.sourcePC;return false;}
        drainPhase_=1;
    }
    if(drainPhase_==1){
        // Slot24's blocking242EE0(1,3) cannot be replaced by GL completion
        // while an unrepresented source callback or either queue remains.
        if(display.dispatch_.pendingCallbackDC){d.sourcePC=0x242F78;return false;}
        if(display.dispatch_.count54||display.dispatch_.count60){d.sourcePC=0x245090;return false;}
        if(!display.renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted){d.sourcePC=0x2DF030;return false;}
        drainPhase_=2;
    }
    return true;
}
bool OriginalUiMemoryCardConstructor50KG::retireDetached142590(Diagnostic& d)noexcept {
    d={};if(phase_==Phase::retired){d.sourcePC=0x1413E0;return true;}
    if(!inputsCurrent()||(phase_!=Phase::detachedConstructed&&phase_!=Phase::retiring)){
        d.sourcePC=0x142590;return false;}
    if(phase_==Phase::detachedConstructed){
        if(constructedQuads_!=7||retiredQuads_||drainPhase_||!display_||!displayPinned_)return false;
        for(std::size_t i=0;i<quads_.size();++i){const auto& q=quads_[i];
            const bool retainedFirst=i==loadRoles_[0].constructionIndex&&firstIconRetained_;
            const bool expected=retainedFirst?
                (q.stage_==Quad::Stage::materialRetained&&q.material54==q.primitiveOwner_.get()&&
                 q.material54&&q.material54->materialStored&&q.material54->materialRetained&&
                 q.material54->domain==domain_&&q.material54->context==context_):
                (q.stage_==Quad::Stage::constructed&&!q.material54);
            if(!q.memoryCardPrefixOnly_||!expected||q.resource58||!q.preflightRetirement2DEFE0(d.quad)){
                d.sourcePC=0x1FEE40;return false;
            }
        }
        for(auto p:dynamicArrays_)if(p){d.sourcePC=0x1FEF20;return false;}
        for(auto p:staticArrays34C_)if(p){d.sourcePC=0x1FEE7C;return false;}
        d.entity=scope_->beginDetachedMemoryCardRetirement142590(*this,entity_);
        if(!d.entity.okay()){d.sourcePC=d.entity.sourcePC;return false;}
        phase_=Phase::retiring;d.sourceEffectsCommitted=true;
        // Empty dynamic arrays reset1FEF20, then nine static array count stores.
        dynamicCounts_.fill(0);for(auto& count:staticCounts370_)count=0;
    }
    d.sourceEffectsCommitted=true;
    while(retiredQuads_<7){auto& q=quads_[6-retiredQuads_];
        if(!q.preflightRetirement2DEFE0(d.quad)){d.sourcePC=d.quad.sourcePC;return false;}
        if(!drainColdQuad2DEFE0(d))return false;
        q.retirementDrainPhase_=drainPhase_;
        if(!q.retireResources2DF038(d.quad)){d.sourcePC=d.quad.sourcePC;return false;}
        ++retiredQuads_;drainPhase_=0;
    }
    const auto keepAlive=root_->memoryCardConstructorOwner_;
    d.entity=scope_->finishDetachedMemoryCardRetirement1413E0(*this,entity_);
    if(!d.entity.okay()){d.sourcePC=d.entity.sourcePC;return false;}
    phase_=Phase::retired;
    if(displayPinned_){display_->unpin();displayPinned_=false;}
    display_.reset();root_->memoryCardConstructorOwner_.reset();d.sourcePC=0x1413E0;return true;
}
}
namespace rr::game {
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::constructDetachedMemoryCard1FEC80(MemoryCardConstructor& owner,C& c)noexcept {
    auto r=live();if(!r.okay())return r;
    if(!dispatcherLive_||!linked(dispatcher_,&application_->root10_)||detachedMemoryCardOwner_||detachedMemoryCard_||
       owner.scope_.get()!=this||owner.root_!=issuer_||!owner.inputsCurrent()||
       owner.phase_!=MemoryCardConstructor::Phase::bound||&c!=&owner.entity_||c.reference00||
       application_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max())return {Status::phase,0x1FEC80};
    if(!application_->entityAuthority_.reserveAdditional(1)||!application_->entityAuthority_.canEnroll(c,key(&c)))
        return {Status::capacity,0x1D6670};
    const auto init=initializeOriginalWreckComponent50LA(c,K::entity141300,scopeGeneration_,key(&c),true,application_->objectCount423F60_);
    if(init.status!=OriginalWreckComponentStatus50LA::complete||!application_->entityAuthority_.enroll(c))std::terminate();
    detachedMemoryCardOwner_=&owner;detachedMemoryCard_=&c;return {Status::complete,0x1FEC8C};
}
bool OriginalUiEntityScope50KG::detachedMemoryCardCurrent(const MemoryCardConstructor& owner,const C& c,bool retiring)const noexcept {
    if(!live().okay()||detachedMemoryCardOwner_!=&owner||detachedMemoryCard_!=&c||
       owner.scope_.get()!=this||owner.root_!=issuer_||&c!=&owner.entity_||detachedMemoryCardRetiring_!=retiring||
       c.kind!=K::entity141300||c.sessionGeneration!=scopeGeneration_||c.objectKey!=key(&c)||
       c.previous10||c.next14||c.firstChild18||c.parent1C||c.mask20||c.descendants24)return false;
    return retiring?(!c.reference00&&!contains(application_->entityAuthority_.view(),&c)&&application_->objectCount423F60_):
        (c.reference00==1&&linked(c,nullptr));
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::beginDetachedMemoryCardRetirement142590(MemoryCardConstructor& owner,C& c)noexcept {
    if(!detachedMemoryCardCurrent(owner,c,false)||!application_->entityAuthority_.canRetire(c))return {Status::phase,0x142590};
    if(!application_->entityAuthority_.retire(c))std::terminate();
    --c.reference00;detachedMemoryCardRetiring_=true;return {Status::complete,0x104970};
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::finishDetachedMemoryCardRetirement1413E0(MemoryCardConstructor& owner,C& c)noexcept {
    if(!detachedMemoryCardCurrent(owner,c,true)||owner.retiredQuads_!=7||owner.drainPhase_)
        return {Status::phase,0x1413E0};
    --application_->objectCount423F60_;detachedMemoryCard_=nullptr;detachedMemoryCardOwner_=nullptr;
    detachedMemoryCardRetiring_=false;return {Status::complete,0x1413E0};
}
}
