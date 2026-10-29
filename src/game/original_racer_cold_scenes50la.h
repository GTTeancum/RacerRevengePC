#pragma once
#include "original_racer_model_scene50la.h"

namespace rr::game {
// Separate retained2893A0 allocations. Only the real native model constructor
// may enter this boundary; indices alone cannot attest a constructed Entity.
// Includes198 because its211808 append intervenes between190 and194.
class OriginalRacerColdScenes50LA final {
    friend class ::native_racer_model50LA::Owner;
    using O=OriginalRacerSceneObject50LA;
    using C=OriginalWreckComponent50LA;
    using I=SceneNodeIndex49P;
    using S=OriginalRacerModelSceneStatus50LA;
    using R=OriginalRacerModelSceneResult50LA;
    std::array<std::unique_ptr<O>,3> objects_; //190,198,194 source allocation order
    OriginalRacerModelSceneBinding50LA binding_{};
    bool attempted_=false,parented_=false,closed_=false;
    unsigned initialized_=0,appended_=0;
    static SceneNode49P& node(const SceneGraph49P& graph,const O& o)noexcept {
        return graph.nodes[static_cast<std::uint32_t>(o.sceneIndex)];
    }
    static C* initialize141C20(C& actualEntity8)noexcept{return &actualEntity8;}
    R construct(const OriginalRacerModelSceneBinding50LA& b,const SceneGraph49P& graph,
        I main190,I hidden198,I container194)noexcept {
        if(attempted_||closed_)return {S::phase,0x21176cu};
        if(!b.sessionGeneration||!b.actualObjectCount423F60||!b.actualTypes383408||
           !b.actualAttachmentLock423F58||!validateSceneGraph49P(graph)||
           main190==hidden198||main190==container194||hidden198==container194)
            return {S::binding,0x21176cu};
        const std::array<I,3> indices{{main190,hidden198,container194}};
        for(auto i:indices)if(!OriginalRacerModelSceneOwner50LA::freshNode(graph,i))
            return {S::graph,0x21176cu};
        attempted_=true;binding_=b;
        for(unsigned i=0;i<objects_.size();++i){
            try{objects_[i]=std::make_unique<O>();}
            catch(...){return {S::allocation,i==0?0x211770u:i==1?0x2117c0u:0x2118ccu};}
            const auto r=OriginalRacerModelSceneOwner50LA::constructStandaloneScene2893A0(
                binding_,*objects_[i],indices[i],graph,true,OriginalWreckComponentKind50LA::scene2893A0);
            if(!r.okay())return r;
            // Actual Entity table3D3198+30=141C20: returns the same Entity8,
            // without stores, allocations or readiness/activation writes.
            (void)initialize141C20(objects_[i]->entity8);
            ++initialized_;
        }
        return {S::complete,0x2118f8u};
    }
    R parent211A34(const SceneGraph49P& graph)noexcept {
        if(initialized_!=3||parented_||closed_)return {S::phase,0x211a34u};
        for(const auto& o:objects_)if(!OriginalRacerModelSceneOwner50LA::freshNode(graph,o->sceneIndex))
            return {S::graph,0x211a34u};
        node(graph,*objects_[0]).firstChild184=objects_[2]->sceneIndex;
        node(graph,*objects_[2]).parent180=objects_[0]->sceneIndex;
        parented_=true;return {S::complete,0x211a38u};
    }
    // Separate actual2124E8 allocation. Native allocation refusal stops rather
    // than claiming the guest NULL/dereference branch. Upper flag bits are
    // opaque backing; only the constructor's low03 bits are source-known.
    std::unique_ptr<O> secondary19C_;
    bool secondaryAttempted_=false,secondaryComplete_=false,secondaryAppended_=false;
    std::uint8_t secondaryFlags03Known_=0,secondaryFlags191Known_=0;
    R constructSecondary2124E8(const SceneGraph49P& graph,I index)noexcept {
        if(secondaryAttempted_||closed_||initialized_!=3||!binding_.sessionGeneration||
           !binding_.actualObjectCount423F60||!binding_.actualTypes383408||
           !binding_.actualAttachmentLock423F58||!validateSceneGraph49P(graph)||
           !OriginalRacerModelSceneOwner50LA::freshNode(graph,index))return {S::binding,0x2124e8u};
        for(const auto& o:objects_)if(o&&o->sceneIndex==index)return {S::graph,0x2124e8u};
        secondaryAttempted_=true;
        try{secondary19C_=std::make_unique<O>();}catch(...){return {S::allocation,0x2124e8u};}
        const auto result=OriginalRacerModelSceneOwner50LA::constructStandaloneScene2893A0(
            binding_,*secondary19C_,index,graph,true,OriginalWreckComponentKind50LA::scene2893A0);
        secondaryFlags03Known_=7u;
        if(!result.okay())return result;
        secondaryFlags191Known_=3u; //Other backing bits are not source-known.
        secondaryComplete_=true;return {S::complete,0x212504u};
    }
    C* secondaryLiveEntity()noexcept {
        return secondary19C_&&secondary19C_->constructed&&!secondary19C_->released?&secondary19C_->entity8:nullptr;
    }
    R initializeSecondary212514()noexcept {
        if(!secondaryComplete_||!secondaryLiveEntity())return {S::phase,0x212514u};
        //Actual3D3198+30=141C20 returns this Entity8 without other effects.
        if(initialize141C20(secondary19C_->entity8)!=&secondary19C_->entity8)return {S::entity,0x212514u};
        return {S::complete,0x21251cu};
    }
    R appendSecondary212530(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(!secondaryComplete_||secondaryAppended_||!secondaryLiveEntity())return {S::phase,0x212530u};
        const auto r=appendOriginalWreckComponent50LA(domain,racer,secondary19C_->entity8,2u,
            *binding_.actualAttachmentLock423F58);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return {S::entity,r.sourcePC};
        secondaryAppended_=true;return {S::complete,0x212538u};
    }
    bool preflightSecondary142590(const SceneGraph49P& graph,const C* parent,
        const C* previous,const C* next)const noexcept {
        if(!secondary19C_||!secondary19C_->constructed||secondary19C_->released)return true;
        const auto& o=*secondary19C_;const auto& c=o.entity8;
        if(!binding_.actualObjectCount423F60||!*binding_.actualObjectCount423F60||
           !binding_.actualAttachmentLock423F58||*binding_.actualAttachmentLock423F58||
           !validateSceneGraph49P(graph)||!OriginalRacerModelSceneOwner50LA::freshNode(graph,o.sceneIndex)||
           o.previous18C!=I::none||o.observer19C||c.reference00!=1||c.firstChild18||
           c.sessionGeneration!=binding_.sessionGeneration||c.kind!=OriginalWreckComponentKind50LA::scene2893A0)
            return false;
        if(secondaryAppended_)return parent&&previous&&c.parent1C==parent&&c.previous10==previous&&
            previous->next14==&c&&c.next14==next&&(!next||next->previous10==&c);
        return !parent&&!previous&&!next&&!c.parent1C&&!c.previous10&&!c.next14;
    }
    R closeSecondary142590(const SceneGraph49P& graph,C* parent,C* previous)noexcept {
        if(!secondaryLiveEntity())return {S::complete,0};
        if(!preflightSecondary142590(graph,parent,previous,nullptr))return {S::entity,0x142590u};
        auto& o=*secondary19C_;auto& c=o.entity8;
        if(secondaryAppended_){previous->next14=nullptr;if(parent->firstChild18==&c)parent->firstChild18=nullptr;}
        --c.reference00;
        //Exact childless/observer-null2896D0 cut; no source links are cleared.
        o.dispatch1A8=OriginalWreckComponentKind50LA::scene2893A0;
        --*binding_.actualObjectCount423F60;o.released=true;
        return {S::complete,0x2897acu};
    }
    void addDomain(C** storage,std::size_t& count)noexcept {
        for(auto& o:objects_)if(o&&o->constructed&&!o->released)storage[count++]=&o->entity8;
        if(auto* secondary=secondaryLiveEntity())storage[count++]=secondary;
    }
    C* lastEntity()noexcept{return objects_[2]?&objects_[2]->entity8:nullptr;}
    R append(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(initialized_!=3||appended_||closed_)return {S::phase,0x2117b8u};
        for(unsigned i=0;i<objects_.size();++i){
            const auto r=appendOriginalWreckComponent50LA(domain,racer,objects_[i]->entity8,2u,
                *binding_.actualAttachmentLock423F58);
            if(r.status!=OriginalWreckComponentStatus50LA::complete)return {S::entity,r.sourcePC};
            ++appended_;
        }
        return {S::complete,0x211918u};
    }
    // Native partial-owner close, only after the later chariot animation.
    // Source stale Entity parent/previous pointers stay stale after142590.
    // Extra graph descendants/observers must close through their own owners.
    R close(const SceneGraph49P& graph)noexcept {
        if(secondaryLiveEntity())return {S::phase,0x2896d0u};
        if(closed_||!attempted_){closed_=true;return {S::complete,0};}
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x1425a8u};
        if(!validateSceneGraph49P(graph))return {S::graph,0x289710u};
        for(unsigned i=0;i<objects_.size();++i){
            const auto& o=objects_[i];if(!o||!o->constructed||o->released)continue;
            const auto& c=o->entity8;const auto& n=node(graph,*o);
            const auto expectedParent=parented_&&i==2?objects_[0]->sceneIndex:I::none;
            const auto expectedChild=parented_&&i==0?objects_[2]->sceneIndex:I::none;
            if(c.reference00!=1||c.firstChild18||o->observer19C||n.parent180!=expectedParent||
               n.firstChild184!=expectedChild||n.nextSibling188!=I::none||o->previous18C!=I::none)
                return {S::hierarchy,0x289710u};
            if(i<appended_){
                if(!c.parent1C||!c.previous10||c.previous10->next14!=&c||
                   (i+1<appended_?c.next14!=&objects_[i+1]->entity8:c.next14!=nullptr))
                    return {S::entity,0x142590u};
            }else if(c.parent1C||c.previous10||c.next14)return {S::entity,0x142590u};
        }
        for(unsigned i=3;i--;)if(objects_[i]&&objects_[i]->constructed&&!objects_[i]->released){
            auto& o=*objects_[i];auto& c=o.entity8;
            if(i<appended_){
                if(c.previous10)c.previous10->next14=c.next14;
                if(c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
            }
            --c.reference00;
            if(parented_&&i==2){
                // Shared28BFC0 dirty/cache prefix must see the still-linked
                //194->190 chain. Preflight above admits only the leaf case.
                OriginalRacerModelSceneOwner50LA::prepareDetachScene28BFC0(graph,o);
                node(graph,*objects_[0]).firstChild184=I::none;
                node(graph,o).parent180=I::none;
            }
            --*binding_.actualObjectCount423F60;o.released=true;
        }
        closed_=true;return {S::complete,0x2897acu};
    }
public:
    OriginalRacerColdScenes50LA()=default;
    OriginalRacerColdScenes50LA(const OriginalRacerColdScenes50LA&)=delete;
    OriginalRacerColdScenes50LA& operator=(const OriginalRacerColdScenes50LA&)=delete;
    ~OriginalRacerColdScenes50LA(){if(secondaryLiveEntity())std::terminate();for(const auto& o:objects_)if(o&&o->constructed&&!o->released)std::terminate();}
};
} //namespace rr::game
