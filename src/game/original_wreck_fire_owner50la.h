#pragma once
#include "original_wreck_smoke_owner50la.h"
#include "original_gc_pod_fire50kg.h"
#include <exception>
#include <limits>

namespace rr::game {
class OriginalWreckRacerStorage50LA;
using OriginalWreckFireBinding50LA=OriginalWreckSmokeBinding50LA;
// A real external-forest owner, never embedded in or destroyed by piece164.
// This bounded constructor exposes no runtime entry before the later Stop84/
//collection suffix is joined. Manager emitter rows outlive this object.
class OriginalWreckFireOwner50LA final {
    friend class OriginalWreckRacerStorage50LA;
    OriginalWreckComponent50LA component_;
    EffectOwner50BE controls_{};
    OriginalGcPodFireDefinition50KG definition_{};
    OriginalGcPodFireRuntime50KG runtime_{};
    const OriginalWreckEffectResourceContext50LA* context_=nullptr;
    const std::vector<std::uint8_t>* bytes_=nullptr;
    std::vector<std::uint8_t> authored_; //private immutable authenticated snapshot
    OriginalWreckFireBinding50LA binding_{};
    bool coldAttempted_=false,prepared_=false,initialized_=false,appended_=false,retired_=false;
    std::uint32_t boundary_=0;
    bool admit(const OriginalWreckFireBinding50LA& b)const noexcept {
        using namespace original_wreck_component_detail50LA;
        if(!prepared_||initialized_||retired_||!context_||!bytes_||context_->podFireEmt!=bytes_||
           *bytes_!=authored_||!context_->particleManager||!context_->particleManager->prepared()||
           !b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||
           !b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||b.domain->count>=b.domainCapacity||
           !domain(*b.domain)||!b.particleParent||!contains(*b.domain,b.particleParent)||
           !b.actualObjectCount423F60||*b.actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max()||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||!b.componentKey||!b.controlOwnerId)return false;
        for(std::size_t i=0;i<b.domain->count;++i){const auto* c=b.domainStorage[i];
            if(!effectControlIdentityBound(*c)||c->objectKey==b.componentKey||
               (c->nativeEffectControlId&&*c->nativeEffectControlId==b.controlOwnerId))return false;}
        return true;
    }
    bool construct15DE00(const OriginalWreckFireBinding50LA& b)noexcept {
        if(!admit(b))return false;binding_=b;boundary_=0x15de00u;
        const auto r=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect15DE00,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;component_.nativeEffectControlId=&controls_.id;
        b.domainStorage[b.domain->count++]=&component_;
        //15A70C initializes1C4=NULL. Authenticated fire reader admits no
        //positive forced-set count, so subsequent setup cannot allocate it.
        boundary_=0x2cb758u;return true;
    }
    bool setupAppend15DEF0()noexcept {
        if(!initialized_||appended_||retired_||boundary_!=0x2cb758u)return false;
        boundary_=0x15def0u;
        if(!assets::buildEffectControlOwner50BJ(binding_.controlOwnerId,EffectProfile50BE::Fire15DE00,
            reinterpret_cast<const char*>(authored_.data()),authored_.size(),{},controls_))return false;
        //The control projection's fresh Stop is numerically inert. The source
        //post-fire Stop84 invocation at2CB7BC remains unreached by this owner.
        const auto r=bindOriginalGcPodFire50KG(runtime_,definition_,*context_->particleManager,controls_);
        if(!r.okay()){boundary_=r.sourcePC;return false;}
        boundary_=0x2cb770u;
        const auto a=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,component_,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(a.status!=OriginalWreckComponentStatus50LA::complete){boundary_=a.sourcePC;return false;}
        appended_=true;boundary_=0x2cb778u;return true;
    }
public:
    OriginalWreckFireOwner50LA()=default;
    OriginalWreckFireOwner50LA(const OriginalWreckFireOwner50LA&)=delete;
    OriginalWreckFireOwner50LA& operator=(const OriginalWreckFireOwner50LA&)=delete;
    ~OriginalWreckFireOwner50LA(){if(initialized_&&!retired_)std::terminate();}
    bool prepareCold(const OriginalWreckEffectResourceContext50LA& context,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(coldAttempted_)return false;coldAttempted_=true;
        if(!context.podFireEmt||!context.particleManager||!context.particleManager->prepared())return false;
        try {authored_=*context.podFireEmt;}catch(...){return false;}
        if(readOriginalGcPodFireDefinition50KG(authored_,
            definition_,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        context_=&context;bytes_=context.podFireEmt;prepared_=true;return true;
    }
    bool retireTail1424E0()noexcept {
        using namespace original_wreck_component_detail50LA;
        if(retired_)return true;
        auto& b=binding_;auto& c=component_;
        if(!initialized_||!prepared_||!b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||
           !b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||
           !b.domain->count||b.domain->count>b.domainCapacity||b.domainStorage[b.domain->count-1]!=&c||!domain(*b.domain)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||
           !b.actualObjectCount423F60||!*b.actualObjectCount423F60||c.reference00!=1||
           c.kind!=OriginalWreckComponentKind50LA::effect15DE00||c.firstChild18||c.next14)return false;
        if(appended_){
            if(c.parent1C!=b.particleParent||!contains(*b.domain,c.parent1C))return false;
            std::size_t remaining=b.domain->count;OriginalWreckComponent50LA* previous=nullptr;
            bool reached=false;
            for(auto* node=c.parent1C->firstChild18;node;node=node->next14){
                if(!remaining--||!contains(*b.domain,node)||node->parent1C!=c.parent1C||node->previous10!=previous)return false;
                if(node==&c)reached=true;previous=node;
            }
            if(!reached)return false;
            if(c.previous10){if(!contains(*b.domain,c.previous10)||c.previous10->next14!=&c||
                c.previous10->parent1C!=c.parent1C)return false;}
            else if(c.parent1C->firstChild18!=&c)return false;
        }else if(c.parent1C||c.previous10)return false;
        //1424E0 suffix is exactly this tail.142590 repairs the parent's head
        //only if needed; source does not clear parent/previous or recompute24.
        if(c.previous10)c.previous10->next14=nullptr;
        c.next14=nullptr;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=nullptr;
        --c.reference00; //104970, actual zero invokes15DE60
        //Authentic forced-set count0 proves the source1C4 NULL free branch.
        c.kind=OriginalWreckComponentKind50LA::entity141300;--*b.actualObjectCount423F60;
        --b.domain->count; //end native live-domain borrow; retained slot history
        c.nativeEffectControlId=nullptr; //the actual external Entity lifetime ended
        retired_=true;boundary_=0x141418u;
        //Do not remove manager rows or particles, or reset historical controls.
        context_=nullptr;bytes_=nullptr;return true;
    }
    bool retired()const noexcept{return retired_;}
    const OriginalWreckComponent50LA& component()const noexcept{return component_;}
    const EffectOwner50BE& controls()const noexcept{return controls_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
};
}
