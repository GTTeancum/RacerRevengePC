#pragma once
#include "original_wreck_fire_owner50la.h"
#include "original_wreck_physical50la.h"
#include "original_wreck_sparks_config50la.h"
#include <optional>

namespace ra_cold_roster50LA {class NativeWreckEffectChildren;}
namespace rr::game {
using OriginalWreckGroupSparksBinding50LA=OriginalWreckSmokeBinding50LA;
// Counted group child in the SAME external forest. No runtime entry is
// exposed by this bounded load owner. Manager value rows outlive its Entity.
class OriginalWreckGroupSparksOwner50LA final {
    friend class OriginalWreckRacerStorage50LA;
    friend class ::ra_cold_roster50LA::NativeWreckEffectChildren;
    OriginalWreckComponent50LA component_;
    EffectOwner50BE controls_{};
    OriginalGcPodSparksDefinition50KG definition_{};
    OriginalGcPodSparksRuntime50KG runtime_{};
    OriginalWreckPhysical50LA* actualBody_=nullptr;
    SceneGraph49P attachmentGraph_{};
    OriginalGcCommonAttachment50KG attachment_{};
    const OriginalWreckEffectResourceContext50LA* context_=nullptr;
    const std::vector<std::uint8_t>* bytes_=nullptr;
    std::vector<std::uint8_t> authored_; //private immutable authenticated snapshot
    OriginalWreckGroupSparksBinding50LA binding_{};
    bool nativeRacer_=false,nativeNullAttachmentKnown_=false;
    bool coldAttempted_=false,prepared_=false,initialized_=false,appended_=false,retired_=false;
    std::uint32_t boundary_=0;
    std::optional<float> nativeDelay1DC_; //unknown until actual Reset88 stores it
    // Fresh source constructor only. Native defaults outside known lanes remain unknown.
    bool freshRoute_=false,freshConstructorComplete_=false;
    std::array<bool,0x240/4> freshKnown_{};
    std::uint8_t freshFlags1D4Known_=0;
    std::array<std::uint32_t,4> freshOffset50_{};
    std::uint32_t freshColor194_=0,freshMinRange220W_=0,freshMaxRange230W_=0;
    float freshDelay1DC_=0;
    std::optional<OriginalGcParticleManager50KG*> freshManager70_;
    std::optional<const OriginalRacerSceneObject50LA*> freshAttachment74_;
    std::optional<const void*> freshForced1C4_;
    bool freshSetupAttempted_=false,freshFilenameKnown_=false,freshFilenameComplete_=false;
    std::uint32_t freshSetupPC_=0;
    const OriginalRacerSceneObject50LA* freshSetupModel_=nullptr; // Borrow only, never released here.
    const std::string* freshDirectory36B530_=nullptr;
    std::array<char,256> freshFilename78_{}; // Native conservative bound, not guessed guest sizing.
    std::size_t freshDirectoryLength_=0,freshFilenameLength_=0;
    OriginalWreckSparksConfig50LA freshConfig_;
    bool freshConfigAttempted_=false;
    void knownFresh(unsigned o)noexcept{freshKnown_[o/4]=true;}
    void vectorKnownFresh(unsigned o)noexcept{for(unsigned i=0;i<4;++i)knownFresh(o+4*i);}
    static float freshFloat(std::uint32_t b)noexcept{float f;std::memcpy(&f,&b,4);return f;}
    bool constructFresh15F100(const OriginalWreckGroupSparksBinding50LA& b)noexcept {
        using namespace original_wreck_component_detail50LA;
        if(freshRoute_||coldAttempted_||prepared_||initialized_||retired_||!b.domain||!b.domainStorage||
           b.domain->objects!=b.domainStorage||!b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||
           b.domain->count>=b.domainCapacity||!domain(*b.domain)||!b.particleParent||!contains(*b.domain,b.particleParent)||
           !b.actualObjectCount423F60||*b.actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max()||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||!b.componentKey||!b.controlOwnerId)return false;
        for(std::size_t i=0;i<b.domain->count;++i){const auto* c=b.domainStorage[i];
            if(!effectControlIdentityBound(*c)||c->objectKey==b.componentKey||
               (c->nativeEffectControlId&&*c->nativeEffectControlId==b.controlOwnerId))return false;}
        freshRoute_=true;binding_=b;boundary_=0x15a5bcu;
        const auto result=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect15F100,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;controls_.id=b.controlOwnerId;controls_.profile=EffectProfile50BE::Ordinary15F100;
        component_.nativeEffectControlId=&controls_.id;b.domainStorage[b.domain->count++]=&component_;
        const std::uint32_t vf0[4]={0,0,0,0x3f800000u};
        auto vector=[&](unsigned offset,std::uint32_t* words){std::memcpy(words,vf0,16);vectorKnownFresh(offset);};
        vector(0x30,controls_.state.position30); //15A5D4
        vector(0x40,controls_.state.previous40); //15A5DC
        vector(0x50,freshOffset50_.data()); //15A5E4
        vector(0x60,controls_.state.velocity60); //15A5EC
        vector(0x30,controls_.state.position30); //15A618
        vector(0x40,controls_.state.previous40); //15A61C
        vector(0x50,freshOffset50_.data()); //15A644
        vector(0x60,controls_.state.velocity60); //15A66C
        controls_.state.flags1D4=std::uint8_t((controls_.state.flags1D4&~1u)|1u);freshFlags1D4Known_|=1u; //15A684
        freshManager70_=nullptr;knownFresh(0x70); //15A688
        freshAttachment74_=nullptr;knownFresh(0x74); //15A68C
        auto number=[&](unsigned offset,float& lane,std::uint32_t bits){lane=freshFloat(bits);knownFresh(offset);};
        number(0x17c,definition_.life,0x457a0000u);
        number(0x180,definition_.growth,0x40c00000u);
        number(0x184,definition_.size,0x3f800000u);
        number(0x188,definition_.rate,0x41200000u);
        runtime_.carriedSize18C=0;knownFresh(0x18c);
        controls_.state.flags1D4=std::uint8_t(controls_.state.flags1D4&~2u);freshFlags1D4Known_|=2u; //15A6B0
        definition_.mirrorX=false;knownFresh(0x1cc);
        definition_.mirrorY=false;knownFresh(0x1d0);
        runtime_.currentFrame=0;knownFresh(0x190);
        controls_.state.flags1D4=std::uint8_t(controls_.state.flags1D4&~4u);freshFlags1D4Known_|=4u; //15A6CC
        freshColor194_=0xffffffu;knownFresh(0x194);
        definition_.flags=0;knownFresh(0x198);
        controls_.state.word19C=0;knownFresh(0x19c);
        number(0x1a0,definition_.birthAge,0);
        runtime_.firstFrame=0;knownFresh(0x1b0);
        runtime_.lastFrame=0;knownFresh(0x1b4);
        definition_.group=-1;knownFresh(0x1b8);
        controls_.mode1D8=0;knownFresh(0x1d8);
        freshDelay1DC_=0;knownFresh(0x1dc);
        number(0x1e0,definition_.timing.delayTime1E0,0);
        controls_.duration1E4=0;knownFresh(0x1e4);
        number(0x1e8,definition_.timing.minTimeOn1E8,0);
        number(0x1ec,definition_.timing.maxTimeOn1EC,0);
        number(0x1f0,definition_.timing.minTimeOff1F0,0);
        number(0x1f4,definition_.timing.maxTimeOff1F4,0);
        freshForced1C4_=nullptr;knownFresh(0x1c4); //15A70C;1C8 remains unknown.
        // Actual15F100 derived constructor, no reader/setup dependency.
        definition_.minRange={{0.f,0.f,0.f}};freshMinRange220W_=0x3f800000u;vectorKnownFresh(0x220); //15F124
        definition_.maxRange={{0.f,0.f,0.f}};freshMaxRange230W_=0x3f800000u;vectorKnownFresh(0x230); //15F12C
        std::memcpy(controls_.state.previous40,controls_.state.position30,16);vectorKnownFresh(0x40); //15F138
        controls_.state.word19C=0;knownFresh(0x19c); //15F13C
        freshConstructorComplete_=true;boundary_=0x2cc0d0u;return true;
    }
    bool setupFresh15F1E0(const OriginalWreckEffectResourceContext50LA& context,
        const OriginalRacerSceneObject50LA* actualModel,const std::array<std::uint32_t,4>& offset,
        const std::string* actualDirectory36B530)noexcept {
        const std::uint32_t vf0[4]={0,0,0,0x3f800000u};
        if(freshSetupAttempted_||!freshConstructorFieldsCurrent()||!preflightFreshUnappended()||
           !context.particleManager||!context.particleManager->prepared()||std::memcmp(offset.data(),vf0,16))return false;
        freshSetupAttempted_=true;context_=&context;freshSetupModel_=actualModel;
        //141C20 has no source stores.15F214 branch delay slot always writes70.
        freshManager70_=context.particleManager; //15F218
        if(actualModel){
            freshAttachment74_=actualModel; //15F220 branch delay slot, even with NULL offset in original.
            std::memcpy(freshOffset50_.data(),offset.data(),16); //15F228; actual caller t0 is known stack50.
        } //NULL model skips both74 and50, retaining constructor values.
        boundary_=freshSetupPC_=0x15f2acu;freshDirectory36B530_=actualDirectory36B530;
        if(!actualDirectory36B530||actualDirectory36B530->size()>=freshFilename78_.size()||
           actualDirectory36B530->find('\0')!=std::string::npos)return false;
        freshManager70_=context.particleManager; //15F2B0 call delay slot, before309230 copy.
        freshDirectoryLength_=actualDirectory36B530->size();
        std::memcpy(freshFilename78_.data(),actualDirectory36B530->data(),freshDirectoryLength_);
        freshFilenameLength_=freshDirectoryLength_;freshFilename78_[freshFilenameLength_]=0;freshFilenameKnown_=true;
        boundary_=freshSetupPC_=0x15f2e4u;
        static constexpr char name[]="podWreckSparks.emt"; //Actual immutable3C27C0 caller operand.
        if(sizeof(name)>freshFilename78_.size()-freshFilenameLength_)return false;
        std::memcpy(freshFilename78_.data()+freshFilenameLength_,name,sizeof(name));
        freshFilenameLength_+=sizeof(name)-1u;freshFilenameComplete_=true;
        boundary_=freshSetupPC_=0x15f2f0u;return true; //BEFORE15AE00 Config/reader, no fallback/readiness.
    }
    bool beginFreshConfig15F2F0()noexcept {
        if(freshConfigAttempted_||!freshFilenameComplete_||!freshSetupAttempted_||
           freshSetupPC_!=0x15f2f0u||boundary_!=0x15f2f0u||!preflightFreshUnappended())return false;
        if(!freshConfig_.construct157320(freshFilename78_.data()))return false;
        freshConfigAttempted_=true;boundary_=0x15ae2cu;return true;
        //Before1577D0: no FileManager handle/open/read/Config success or derived dispatch.
    }
    bool freshSetupPrefixCurrent(bool live)const noexcept {
        if(!freshSetupAttempted_)return !context_&&!freshSetupModel_&&!freshDirectory36B530_&&
            !freshSetupPC_&&!freshFilenameKnown_&&!freshFilenameComplete_&&!freshDirectoryLength_&&
            !freshFilenameLength_&&freshManager70_&&!*freshManager70_&&freshAttachment74_&&!*freshAttachment74_;
        if(!context_||!freshManager70_||!*freshManager70_||!freshAttachment74_||
           *freshAttachment74_!=freshSetupModel_)return false;
        //After retirement these are identity-only borrows; do not dereference model, context or directory.
        if(live&&(!context_->particleManager||*freshManager70_!=context_->particleManager||
           !context_->particleManager->prepared()))return false;
        if(!freshFilenameKnown_)return freshSetupPC_==0x15f2acu&&!freshFilenameComplete_&&
            !freshDirectoryLength_&&!freshFilenameLength_&&(!live||!freshDirectory36B530_||
            freshDirectory36B530_->size()>=freshFilename78_.size()||freshDirectory36B530_->find('\0')!=std::string::npos);
        if(!freshDirectory36B530_||freshDirectoryLength_>=freshFilename78_.size()||
           freshFilenameLength_>=freshFilename78_.size()||freshFilename78_[freshFilenameLength_])return false;
        if(live&&(freshDirectory36B530_->size()!=freshDirectoryLength_||
           freshDirectory36B530_->find('\0')!=std::string::npos||
           std::memcmp(freshFilename78_.data(),freshDirectory36B530_->data(),freshDirectoryLength_)))return false;
        static constexpr char name[]="podWreckSparks.emt";
        if(!freshFilenameComplete_)return freshSetupPC_==0x15f2e4u&&
            freshFilenameLength_==freshDirectoryLength_&&sizeof(name)>freshFilename78_.size()-freshDirectoryLength_;
        return freshSetupPC_==0x15f2f0u&&sizeof(name)<=freshFilename78_.size()-freshDirectoryLength_&&
            freshFilenameLength_==freshDirectoryLength_+sizeof(name)-1u&&
            !std::memcmp(freshFilename78_.data()+freshDirectoryLength_,name,sizeof(name));
    }
    bool freshConstructorFieldsCurrent()const noexcept {
        if(!freshRoute_||!freshConstructorComplete_||!initialized_||retired_||prepared_||coldAttempted_||appended_||
           nativeRacer_||bytes_||!authored_.empty()||actualBody_||attachment_.graph||
           definition_.parsed||runtime_.bound||runtime_.manager||runtime_.ownerId||runtime_.canonicalOwnerId||
           !freshSetupPrefixCurrent(true)||
           !freshForced1C4_||*freshForced1C4_||freshFlags1D4Known_!=7u||
           (controls_.state.flags1D4&7u)!=1u||boundary_!=(freshConfigAttempted_?0x15ae2cu:freshSetupAttempted_?freshSetupPC_:0x2cc0d0u)||
           (freshConfigAttempted_&&(!freshFilenameComplete_||freshSetupPC_!=0x15f2f0u||
              !freshConfig_.current()||freshConfig_.filename()!=freshFilename78_.data())))return false;
        const unsigned scalar[]={0x70,0x74,0x17c,0x180,0x184,0x188,0x18c,0x190,0x194,0x198,0x19c,0x1a0,
            0x1b0,0x1b4,0x1b8,0x1c4,0x1cc,0x1d0,0x1d8,0x1dc,0x1e0,0x1e4,0x1e8,0x1ec,0x1f0,0x1f4};
        for(unsigned i=0;i<freshKnown_.size();++i){const unsigned o=i*4;
            bool expected=(o>=0x30&&o<0x70)||(o>=0x220&&o<0x240);
            for(auto v:scalar)if(v==o)expected=true;
            if(freshKnown_[i]!=expected)return false;
        }
        const std::uint32_t vf0[4]={0,0,0,0x3f800000u};
        if(std::memcmp(controls_.state.position30,vf0,16)||std::memcmp(controls_.state.previous40,vf0,16)||
           std::memcmp(controls_.state.velocity60,vf0,16)||std::memcmp(freshOffset50_.data(),vf0,16))return false;
        for(unsigned i=0;i<3;++i)if(definition_.minRange[i]!=0.f||definition_.maxRange[i]!=0.f)return false;
        return freshMinRange220W_==0x3f800000u&&freshMaxRange230W_==0x3f800000u&&
            definition_.life==4000.f&&definition_.growth==6.f&&definition_.size==1.f&&definition_.rate==10.f&&
            runtime_.carriedSize18C==0.f&&runtime_.currentFrame==0&&freshColor194_==0xffffffu&&
            !definition_.mirrorX&&!definition_.mirrorY&&!definition_.flags&&!controls_.state.word19C&&
            definition_.birthAge==0.f&&runtime_.firstFrame==0&&runtime_.lastFrame==0&&definition_.group==-1&&
            !controls_.mode1D8&&!controls_.duration1E4&&freshDelay1DC_==0.f&&
            definition_.timing.delayTime1E0==0.f&&definition_.timing.minTimeOn1E8==0.f&&
            definition_.timing.maxTimeOn1EC==0.f&&definition_.timing.minTimeOff1F0==0.f&&definition_.timing.maxTimeOff1F4==0.f;
    }
    bool preflightFreshChainMember(std::size_t index,const OriginalWreckComponent50LA* previous,
        const OriginalWreckComponent50LA* next)const noexcept {
        using namespace original_wreck_component_detail50LA;
        const auto& b=binding_;const auto& c=component_;
        if(!freshConstructorFieldsCurrent()||previous||next||!b.domain||!b.domainStorage||
           b.domain->objects!=b.domainStorage||!b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||
           !b.domain->count||b.domain->count>b.domainCapacity||index+1u!=b.domain->count||
           b.domainStorage[index]!=&c||!domain(*b.domain)||!b.particleParent||!contains(*b.domain,b.particleParent)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||!b.actualObjectCount423F60||
           !*b.actualObjectCount423F60||c.reference00!=1u||c.kind!=OriginalWreckComponentKind50LA::effect15F100||
           c.parent1C||c.previous10||c.next14||c.firstChild18||c.nativeEffectControlId!=&controls_.id||
           controls_.id!=b.controlOwnerId||controls_.profile!=EffectProfile50BE::Ordinary15F100||
           c.objectKey!=b.componentKey||c.sessionGeneration!=b.domain->sessionGeneration||c.mask20||c.descendants24||c.category28!=0xffu||
           (c.flags29&15u)!=3u||c.nativeFlags29)return false;
        return true;
    }
    bool preflightFreshUnappended(bool allowAppended=false)const noexcept {
        (void)allowAppended; // This bounded source stage never admits append.
        if(!initialized_)return !coldAttempted_&&!prepared_&&!definition_.parsed&&!appended_&&
            !runtime_.manager&&!runtime_.bound&&!component_.reference00&&
            component_.kind==OriginalWreckComponentKind50LA::unbound&&!component_.nativeEffectControlId&&
            !component_.parent1C&&!component_.previous10&&!component_.next14&&!component_.firstChild18;
        return binding_.domain&&binding_.domain->count&&
            preflightFreshChainMember(binding_.domain->count-1u,nullptr,nullptr);
    }
    bool retireFreshUnappended(bool allowAppended=false)noexcept {
        if(retired_)return true;
        if(!preflightFreshUnappended(allowAppended))return false;
        if(!initialized_){retired_=true;return true;}
        //Native cancellation of the still-open source stack invocation, using
        //the exact unopened1573B0 destructor path; no15AE00 return is claimed.
        if(freshConfigAttempted_&&!freshConfig_.retireUnopened1573B0())return false;
        --component_.reference00; //104970→15F150; actual constructor1C4 is knownNULL.
        component_.kind=OriginalWreckComponentKind50LA::entity141300; //15F1A0→1413E0
        --*binding_.actualObjectCount423F60;--binding_.domain->count;
        component_.nativeEffectControlId=nullptr;retired_=true;boundary_=0x141418u;return true;
    }
    bool retiredFreshConstructorCurrent()const noexcept {
        using namespace original_wreck_component_detail50LA;
        const auto& b=binding_;const auto& c=component_;
        if(!freshRoute_||!freshConstructorComplete_||!initialized_||!retired_||prepared_||coldAttempted_||appended_||
           boundary_!=0x141418u||c.reference00||c.nativeEffectControlId||c.kind!=OriginalWreckComponentKind50LA::entity141300||
           c.objectKey!=b.componentKey||c.parent1C||c.previous10||c.next14||c.firstChild18||
           !freshForced1C4_||*freshForced1C4_||!freshSetupPrefixCurrent(false)||runtime_.bound||runtime_.manager||
           !b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||
           b.domain->count>b.domainCapacity||c.sessionGeneration!=b.domain->sessionGeneration||!domain(*b.domain)||contains(*b.domain,&c))return false;
        return controls_.id==b.controlOwnerId&&controls_.profile==EffectProfile50BE::Ordinary15F100&&
            (!freshConfigAttempted_||(freshConfig_.retiredCurrent()&&freshConfig_.filename()==freshFilename78_.data()));
    }
    bool admit(const OriginalWreckGroupSparksBinding50LA& b)const noexcept {
        using namespace original_wreck_component_detail50LA;
        if(freshRoute_||!prepared_||initialized_||retired_||!context_||!bytes_||(nativeRacer_?context_->podExplWreckSparksEmt:context_->podWreckSparksEmt)!=bytes_||
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
    bool constructSource(const OriginalWreckGroupSparksBinding50LA& b)noexcept {
        if(!admit(b))return false;binding_=b;boundary_=0x15f100u;
        const auto r=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect15F100,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;component_.nativeEffectControlId=&controls_.id;
        b.domainStorage[b.domain->count++]=&component_;
        //15A70C initializes1C4=NULL. Authenticated reader admits no
        //positive forced-set count, so subsequent setup cannot allocate it.
        if(nativeRacer_)nativeNullAttachmentKnown_=true; //actual15A68C constructor74=NULL
        boundary_=nativeRacer_?0x212becu:0x2cc0d0u;return true;
    }
    bool setupStopAppendSource(OriginalWreckAssets50LA& assets,OriginalWreckPhysical50LA& body)noexcept {
        if(freshRoute_||nativeRacer_||!initialized_||appended_||retired_||boundary_!=0x2cc0d0u)return false;
        const auto graph=assets.graph();
        if(!assets.prepared()||!body.bodyConstructorFieldsApplied||!body.body.scenePresent||
           body.model288!=assets.model14C()||body.modelGraphNodes!=graph.nodes||body.modelGraphCount!=graph.count||
           !validateSceneGraph49P(graph))return false;
        actualBody_=&body;attachmentGraph_=graph;attachment_.graph=&attachmentGraph_;
        attachment_.node=body.model288;attachment_.offset50={{0.f,0.f,0.f,1.f}};
        boundary_=0x15f1e0u;
        if(!assets::buildEffectControlOwner50BJ(binding_.controlOwnerId,EffectProfile50BE::Ordinary15F100,
            reinterpret_cast<const char*>(authored_.data()),authored_.size(),{},controls_))return false;
        //Cold control construction has no enabled bit; its normalization is
        //not a source dispatch. The actual caller Stop84 follows registration.
        const auto r=bindOriginalGcPodSparks50KG(runtime_,definition_,*context_->particleManager,controls_);
        if(!r.okay()){boundary_=r.sourcePC;return false;}
        stopEffect50BC(controls_.state); //actual2CC108/2CC178
        boundary_=0x2cc118u;
        const auto a=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,component_,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(a.status!=OriginalWreckComponentStatus50LA::complete){boundary_=a.sourcePC;return false;}
        appended_=true;boundary_=0x2cc120u;return true;
    }
    bool prepareNativeRacerCold(const OriginalWreckEffectResourceContext50LA& context,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(freshRoute_||coldAttempted_||initialized_)return false;
        nativeRacer_=true;return prepareCold(context,diagnostic);
    }
    bool setupAppendStopDisableNativeRacer()noexcept {
        if(!nativeRacer_||!nativeNullAttachmentKnown_||!initialized_||appended_||retired_||
           boundary_!=0x212becu||actualBody_||attachment_.graph)return false;
        //Actual caller a3/t0 are NULL. Constructor15A68C left74 NULL; no
        //primary-model attachment or invented lookup is supplied here.
        boundary_=0x15f1e0u;
        if(!assets::buildEffectControlOwner50BJ(binding_.controlOwnerId,EffectProfile50BE::Ordinary15F100,
            reinterpret_cast<const char*>(authored_.data()),authored_.size(),{},controls_))return false;
        const auto bound=bindOriginalGcPodSparks50KG(runtime_,definition_,*context_->particleManager,controls_);
        if(!bound.okay()){boundary_=bound.sourcePC;return false;}
        boundary_=0x212c14u;
        const auto appended=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,component_,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete){boundary_=appended.sourcePC;return false;}
        appended_=true;boundary_=0x212c28u;if(!stopSource84())return false;
        boundary_=0x212c3cu;
        if(setOriginalWreckBaseActivity50LA(*binding_.domain,component_,false).status!=
            OriginalWreckComponentStatus50LA::complete)return false; //actual virtual18=141520
        boundary_=0x212c44u;return true;
    }
    bool resetSource88()noexcept {
        if(!initialized_||!appended_||retired_||!context_||component_.reference00!=1u||
           controls_.id!=binding_.controlOwnerId||controls_.mode1D8!=definition_.timing.mode1D8||
           controls_.mode1D8>1u)return false;
        nativeDelay1DC_=definition_.timing.delayTime1E0;
        if(controls_.mode1D8==1u)std::memcpy(&controls_.duration1E4,&definition_.timing.maxTimeOn1EC,4);
        return true;
    }
    bool stopSource84()noexcept {
        if(!initialized_||!appended_||retired_||!context_||component_.reference00!=1u||
           controls_.id!=binding_.controlOwnerId)return false;
        stopEffect50BC(controls_.state);return true;
    }
public:
    OriginalWreckGroupSparksOwner50LA()=default;
    OriginalWreckGroupSparksOwner50LA(const OriginalWreckGroupSparksOwner50LA&)=delete;
    OriginalWreckGroupSparksOwner50LA& operator=(const OriginalWreckGroupSparksOwner50LA&)=delete;
    ~OriginalWreckGroupSparksOwner50LA(){if(initialized_&&!retired_)std::terminate();}
    bool prepareCold(const OriginalWreckEffectResourceContext50LA& context,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(freshRoute_||coldAttempted_)return false;coldAttempted_=true;
        const auto* bytes=nativeRacer_?context.podExplWreckSparksEmt:context.podWreckSparksEmt;
        if(!bytes||!context.particleManager||!context.particleManager->prepared())return false;
        try {authored_=*bytes;}catch(...){return false;}
        if(readOriginalGcPodSparksDefinition50KG(authored_,nativeRacer_?OriginalGcPodSparksMember50KG::racerExplWreckSparks:OriginalGcPodSparksMember50KG::wreckSparks,
            definition_,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        context_=&context;bytes_=bytes;prepared_=true;return true;
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
           c.kind!=OriginalWreckComponentKind50LA::effect15F100||c.firstChild18||c.next14)return false;
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
        --c.reference00; //104970, actual zero invokes the derived source destructor
        //Authentic forced-set count0 proves the source1C4 NULL free branch.
        c.kind=OriginalWreckComponentKind50LA::entity141300;--*b.actualObjectCount423F60;
        --b.domain->count; //end native live-domain borrow; retained slot history
        c.nativeEffectControlId=nullptr; //the actual external Entity lifetime ended
        retired_=true;boundary_=0x141418u;
        //Do not remove manager rows or particles, or reset historical controls.
        context_=nullptr;bytes_=nullptr;actualBody_=nullptr;return true;
    }
    bool retired()const noexcept{return retired_;}
    const OriginalWreckComponent50LA& component()const noexcept{return component_;}
    const EffectOwner50BE& controls()const noexcept{return controls_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
};
using OriginalWreckGroupExplosionBinding50LA=OriginalWreckSmokeBinding50LA;
// Counted group child in the SAME external forest. No runtime entry is
// exposed by this bounded load owner. Manager value rows outlive its Entity.
class OriginalWreckGroupExplosionOwner50LA final {
    friend class OriginalWreckRacerStorage50LA;
    friend class ::ra_cold_roster50LA::NativeWreckEffectChildren;
    OriginalWreckComponent50LA component_;
    EffectOwner50BE controls_{};
    OriginalWreckExplosionDefinition50LA definition_{};
    OriginalWreckExplosionRuntime50LA runtime_{};
    OriginalWreckPhysical50LA* actualBody_=nullptr;
    SceneGraph49P attachmentGraph_{};
    OriginalGcCommonAttachment50KG attachment_{};
    const OriginalWreckEffectResourceContext50LA* context_=nullptr;
    const std::vector<std::uint8_t>* bytes_=nullptr;
    std::vector<std::uint8_t> authored_; //private immutable authenticated snapshot
    OriginalWreckGroupExplosionBinding50LA binding_{};
    bool nativeRacer_=false,nativeNullAttachmentKnown_=false;
    bool coldAttempted_=false,prepared_=false,initialized_=false,appended_=false,retired_=false;
    std::uint32_t boundary_=0;
    std::optional<float> nativeDelay1DC_; //unknown until actual Reset88 stores it
    bool admit(const OriginalWreckGroupExplosionBinding50LA& b)const noexcept {
        using namespace original_wreck_component_detail50LA;
        if(!prepared_||initialized_||retired_||!context_||!bytes_||(nativeRacer_?context_->podExplWreckExplosionEmt:context_->podExplosionEmt)!=bytes_||
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
    bool constructSource(const OriginalWreckGroupExplosionBinding50LA& b)noexcept {
        if(!admit(b))return false;binding_=b;boundary_=0x15e800u;
        const auto r=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect15E800,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;component_.nativeEffectControlId=&controls_.id;
        b.domainStorage[b.domain->count++]=&component_;
        //15A70C initializes1C4=NULL. Authenticated reader admits no
        //positive forced-set count, so subsequent setup cannot allocate it.
        if(nativeRacer_)nativeNullAttachmentKnown_=true; //actual15A68C constructor74=NULL
        boundary_=nativeRacer_?0x212b78u:0x2cc140u;return true;
    }
    bool setupStopAppendSource(OriginalWreckAssets50LA& assets,OriginalWreckPhysical50LA& body)noexcept {
        if(nativeRacer_||!initialized_||appended_||retired_||boundary_!=0x2cc140u)return false;
        const auto graph=assets.graph();
        if(!assets.prepared()||!body.bodyConstructorFieldsApplied||!body.body.scenePresent||
           body.model288!=assets.model14C()||body.modelGraphNodes!=graph.nodes||body.modelGraphCount!=graph.count||
           !validateSceneGraph49P(graph))return false;
        actualBody_=&body;attachmentGraph_=graph;attachment_.graph=&attachmentGraph_;
        attachment_.node=body.model288;attachment_.offset50={{0.f,0.f,0.f,1.f}};
        boundary_=0x15e8f0u;
        if(!assets::buildEffectControlOwner50BJ(binding_.controlOwnerId,EffectProfile50BE::Deferred15E800,
            reinterpret_cast<const char*>(authored_.data()),authored_.size(),{},controls_))return false;
        //Cold control construction has no enabled bit; its normalization is
        //not a source dispatch. The actual caller Stop84 follows registration.
        const auto r=bindOriginalWreckExplosion50LA(runtime_,definition_,*context_->particleManager,controls_);
        if(!r.okay()){boundary_=r.sourcePC;return false;}
        if(stopDeferredEffect50BD(controls_.state,controls_.mode1D8,controls_.duration1E4)==EffectStopResult50BD::UnsupportedDuration)return false; //actual2CC108/2CC178
        boundary_=0x2cc188u;
        const auto a=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,component_,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(a.status!=OriginalWreckComponentStatus50LA::complete){boundary_=a.sourcePC;return false;}
        appended_=true;boundary_=0x2cc190u;return true;
    }
    bool prepareNativeRacerCold(const OriginalWreckEffectResourceContext50LA& context,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(coldAttempted_||initialized_)return false;
        nativeRacer_=true;return prepareCold(context,diagnostic);
    }
    bool setupAppendStopDisableNativeRacer()noexcept {
        if(!nativeRacer_||!nativeNullAttachmentKnown_||!initialized_||appended_||retired_||
           boundary_!=0x212b78u||actualBody_||attachment_.graph)return false;
        //Actual caller a3/t0 are NULL. Constructor15A68C left74 NULL; no
        //primary-model attachment or invented lookup is supplied here.
        boundary_=0x15e8f0u;
        if(!assets::buildEffectControlOwner50BJ(binding_.controlOwnerId,EffectProfile50BE::Deferred15E800,
            reinterpret_cast<const char*>(authored_.data()),authored_.size(),{},controls_))return false;
        const auto bound=bindOriginalWreckExplosion50LA(runtime_,definition_,*context_->particleManager,controls_);
        if(!bound.okay()){boundary_=bound.sourcePC;return false;}
        boundary_=0x212ba0u;
        const auto appended=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,component_,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete){boundary_=appended.sourcePC;return false;}
        appended_=true;boundary_=0x212bb4u;if(!stopSource84())return false;
        boundary_=0x212bc8u;
        if(setOriginalWreckBaseActivity50LA(*binding_.domain,component_,false).status!=
            OriginalWreckComponentStatus50LA::complete)return false; //actual virtual18=141520
        boundary_=0x212bd0u;return true;
    }
    bool resetSource88()noexcept {
        if(!initialized_||!appended_||retired_||!context_||component_.reference00!=1u||
           controls_.id!=binding_.controlOwnerId||controls_.mode1D8!=definition_.timing.mode1D8||
           controls_.mode1D8>1u)return false;
        nativeDelay1DC_=definition_.timing.delayTime1E0;
        if(controls_.mode1D8==1u)std::memcpy(&controls_.duration1E4,&definition_.timing.maxTimeOn1EC,4);
        return true;
    }
    bool stopSource84()noexcept {
        if(!initialized_||!appended_||retired_||!context_||component_.reference00!=1u||
           controls_.id!=binding_.controlOwnerId)return false;
        return stopDeferredEffect50BD(controls_.state,controls_.mode1D8,controls_.duration1E4)!=EffectStopResult50BD::UnsupportedDuration;
    }
public:
    OriginalWreckGroupExplosionOwner50LA()=default;
    OriginalWreckGroupExplosionOwner50LA(const OriginalWreckGroupExplosionOwner50LA&)=delete;
    OriginalWreckGroupExplosionOwner50LA& operator=(const OriginalWreckGroupExplosionOwner50LA&)=delete;
    ~OriginalWreckGroupExplosionOwner50LA(){if(initialized_&&!retired_)std::terminate();}
    bool prepareCold(const OriginalWreckEffectResourceContext50LA& context,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(coldAttempted_)return false;coldAttempted_=true;
        const auto* bytes=nativeRacer_?context.podExplWreckExplosionEmt:context.podExplosionEmt;
        if(!bytes||!context.particleManager||!context.particleManager->prepared())return false;
        try {authored_=*bytes;}catch(...){return false;}
        if(readOriginalWreckExplosionDefinition50LA(authored_,nativeRacer_?OriginalWreckExplosionMember50LA::racerExplWreckExplosion:OriginalWreckExplosionMember50LA::groupExplosion,
            definition_,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        context_=&context;bytes_=bytes;prepared_=true;return true;
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
           c.kind!=OriginalWreckComponentKind50LA::effect15E800||c.firstChild18||c.next14)return false;
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
        --c.reference00; //104970, actual zero invokes the derived source destructor
        //Authentic forced-set count0 proves the source1C4 NULL free branch.
        c.kind=OriginalWreckComponentKind50LA::entity141300;--*b.actualObjectCount423F60;
        --b.domain->count; //end native live-domain borrow; retained slot history
        c.nativeEffectControlId=nullptr; //the actual external Entity lifetime ended
        retired_=true;boundary_=0x141418u;
        //Do not remove manager rows or particles, or reset historical controls.
        context_=nullptr;bytes_=nullptr;actualBody_=nullptr;return true;
    }
    bool retired()const noexcept{return retired_;}
    const OriginalWreckComponent50LA& component()const noexcept{return component_;}
    const EffectOwner50BE& controls()const noexcept{return controls_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
};
}
