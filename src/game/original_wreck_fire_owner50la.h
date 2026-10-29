#pragma once
#include "original_wreck_smoke_owner50la.h"
#include "original_gc_pod_fire50kg.h"
#include <exception>
#include <cfenv>
#include <cmath>
#include <array>
#include <cstring>
#include <optional>
#include <string>
#include <limits>

namespace rr::game {
class OriginalWreckRacerStorage50LA;
using OriginalWreckFireBinding50LA=OriginalWreckSmokeBinding50LA;
// A real external-forest owner, never embedded in or destroyed by piece164.
// This bounded constructor exposes no runtime entry before the later Stop84/
//collection suffix is joined. Manager emitter rows outlive this object.
class OriginalWreckFireOwner50LA final {
    friend class OriginalWreckRacerStorage50LA;
    friend class ::ra_cold_roster50LA::NativeWreckEffectChildren;
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
    //Fresh-only knownness/absent lanes. Canonical emission/controls/runtime
    //remain sole authorities for every already represented numeric field.
    bool freshRoute_=false,freshSetupAttempted_=false,freshConstructorComplete_=false;
    bool freshReaderAttempted_=false,freshReaderComplete_=false;
    float freshInverseFrames1C0_=0; //Absent scalar; unknown until source15B4C4.
    std::int32_t freshForcedCount1C8_=0;
    std::array<bool,0x230/4> freshKnown_{};
    std::uint8_t freshFlags1D4Known_=0;
    std::array<std::uint32_t,4> freshOffset50_{};
    std::uint32_t freshWord18C_=0,freshColor194_=0;
    std::int32_t freshCurrentFrame190_=0;
    float freshAgeAtBirth1A0_=0,freshDelay1DC_=0,freshDelayTime1E0_=0;
    float freshMinTimeOn1E8_=0,freshMaxTimeOn1EC_=0,freshMinTimeOff1F0_=0,freshMaxTimeOff1F4_=0;
    std::uint32_t freshMinRange200W_=0,freshMaxRange210W_=0;
    std::optional<OriginalGcParticleManager50KG*> freshManager70_;
    std::optional<const OriginalRacerSceneObject50LA*> freshAttachment74_;
    std::optional<const void*> freshForced1C4_;
    std::array<char,256> freshFilename78_{}; //Conservative native bound, not guest capacity.
    std::size_t freshFilenameLength_=0;bool freshFilenameKnown_=false;
    const std::string* freshDirectory36B530_=nullptr;
    void knownFresh(unsigned offset)noexcept{freshKnown_[offset/4]=true;}
    void vectorKnownFresh(unsigned offset)noexcept{for(unsigned i=0;i<4;++i)knownFresh(offset+4*i);}
    static float freshFloat(std::uint32_t bits)noexcept{float value;std::memcpy(&value,&bits,4);return value;}
    bool constructFresh15DE00(const OriginalWreckFireBinding50LA& b)noexcept {
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
        const auto result=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect15DE00,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;controls_.id=b.controlOwnerId;controls_.profile=EffectProfile50BE::Fire15DE00;
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
        number(0x17c,definition_.emission.particleLife,0x457a0000u);
        number(0x180,definition_.emission.growth,0x40c00000u);
        number(0x184,definition_.emission.initialSize,0x3f800000u);
        number(0x188,definition_.emission.emitRate,0x41200000u);
        freshWord18C_=0;knownFresh(0x18c);
        controls_.state.flags1D4=std::uint8_t(controls_.state.flags1D4&~2u);freshFlags1D4Known_|=2u; //15A6B0
        definition_.emission.randomMirrorX=false;knownFresh(0x1cc);
        definition_.emission.randomMirrorY=false;knownFresh(0x1d0);
        freshCurrentFrame190_=0;knownFresh(0x190);
        controls_.state.flags1D4=std::uint8_t(controls_.state.flags1D4&~4u);freshFlags1D4Known_|=4u; //15A6CC
        freshColor194_=0xffffffu;knownFresh(0x194);
        definition_.emission.particleFlags=0;knownFresh(0x198);
        controls_.state.word19C=0;knownFresh(0x19c);
        number(0x1a0,freshAgeAtBirth1A0_,0);
        definition_.emission.firstFrame=0;knownFresh(0x1b0);
        definition_.emission.lastFrame=0;knownFresh(0x1b4);
        definition_.emission.particleGroup=-1;knownFresh(0x1b8);
        controls_.mode1D8=0;knownFresh(0x1d8);
        freshDelay1DC_=0;knownFresh(0x1dc);
        number(0x1e0,freshDelayTime1E0_,0);
        controls_.duration1E4=0;knownFresh(0x1e4);
        number(0x1e8,freshMinTimeOn1E8_,0);
        number(0x1ec,freshMaxTimeOn1EC_,0);
        number(0x1f0,freshMinTimeOff1F0_,0);
        number(0x1f4,freshMaxTimeOff1F4_,0);
        freshForced1C4_=nullptr;knownFresh(0x1c4); //15A70C;1C8 remains unknown.
        //15DE00 derived constructor, SAME canonical emission/control/runtime.
        definition_.emission.minRange={{0.f,0.f,0.f}};freshMinRange200W_=0x3f800000u;vectorKnownFresh(0x200); //15DE24
        definition_.emission.maxRange={{0.f,0.f,0.f}};freshMaxRange210W_=0x3f800000u;vectorKnownFresh(0x210); //15DE2C
        std::memcpy(controls_.state.previous40,controls_.state.position30,16);vectorKnownFresh(0x40); //15DE3C
        controls_.state.word19C=0;knownFresh(0x19c); //15DE40
        definition_.emission.applySpreadToVelocity=true;knownFresh(0x228); //15DE44
        runtime_.fractional224=0;knownFresh(0x224); //15DE48
        //220 has NO constructor store. Native backing's default is not known.
        //Fire200/210 belong to emission; controls.intensity204 is NOT Fire204.
        freshConstructorComplete_=true;boundary_=0x2cb758u;return true;
    }
    bool setupFresh15DEF0(const OriginalWreckEffectResourceContext50LA& context,
        const std::string* actualDirectory36B530)noexcept {
        if(!freshRoute_||!freshConstructorComplete_||!initialized_||freshSetupAttempted_||retired_||
           boundary_!=0x2cb758u||!context.particleManager||!context.particleManager->prepared())return false;
        freshSetupAttempted_=true;context_=&context;boundary_=0x15df1cu;
        freshManager70_=context.particleManager; //15DF28;141C20 has no source field effects.
        //Actual a3/t0 are NULL:15DF24 skips74/50 writes, preserving constructor values.
        boundary_=0x15dfdcu;
        freshManager70_=context.particleManager; //15DFE4 in309230 call delay slot.
        if(!actualDirectory36B530)return false; //Missing authority is not synthetic empty prefix.
        freshDirectory36B530_=actualDirectory36B530;
        if(actualDirectory36B530->size()>=freshFilename78_.size()||actualDirectory36B530->find('\0')!=std::string::npos)return false;
        std::memcpy(freshFilename78_.data(),actualDirectory36B530->data(),actualDirectory36B530->size());
        freshFilenameLength_=actualDirectory36B530->size();freshFilename78_[freshFilenameLength_]=0;freshFilenameKnown_=true;
        boundary_=0x15e018u;
        constexpr char member[]="podFire.emt";
        if(freshFilenameLength_+sizeof(member)>freshFilename78_.size())return false;
        std::memcpy(freshFilename78_.data()+freshFilenameLength_,member,sizeof(member));
        freshFilenameLength_+=sizeof(member)-1u;boundary_=0x15e024u;return true;
    }
    bool readFresh15E024(const std::vector<std::uint8_t>& actualEmt,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(!freshRoute_||!freshConstructorComplete_||!initialized_||retired_||appended_||
           freshReaderAttempted_||boundary_!=0x15e024u||!freshFilenameKnown_||
           !context_||context_->podFireEmt!=&actualEmt||!freshManager70_||!*freshManager70_||
           *freshManager70_!=context_->particleManager||!context_->particleManager->prepared())return false;
        freshReaderAttempted_=true;boundary_=0x15ae2cu;
        //Authenticated native transport/admission, NOT guest Config/FileManager
        //execution or a fabricated Config-load failure. Staging is not live state.
        OriginalGcPodFireDefinition50KG staged;
        OriginalGcPodFireSourceMetadata50KG meta;
        if(readOriginalGcPodFireSourceDefinition50KG(actualEmt,
            staged,meta,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        auto number=[&](unsigned offset,float& lane,float value,std::uint32_t pc){
            boundary_=pc;lane=value;knownFresh(offset);};
        //15E6D0 derived reads precede common15AEA0.158C00 touchesXYZonly.
        boundary_=0x15e710u;if(!meta.minRangePresent)return false;
        for(unsigned k=0;k<3;++k)number(0x200+4*k,definition_.emission.minRange[k],
            staged.emission.minRange[k],0x158c7cu+(k==0?0u:k==1?0x24u:0x48u));
        boundary_=0x15e72cu;if(!meta.maxRangePresent)return false;
        for(unsigned k=0;k<3;++k)number(0x210+4*k,definition_.emission.maxRange[k],
            staged.emission.maxRange[k],0x158c7cu+(k==0?0u:k==1?0x24u:0x48u));
        boundary_=0x15e744u;definition_.emission.applySpreadToVelocity=staged.emission.applySpreadToVelocity;knownFresh(0x228);
        boundary_=0x15aee0u;definition_.emission.particleGroup=staged.emission.particleGroup;knownFresh(0x1b8);
        number(0x17c,definition_.emission.particleLife,staged.emission.particleLife,0x15af70);
        number(0x184,definition_.emission.initialSize,staged.emission.initialSize,0x15af8c);
        number(0x180,definition_.emission.growth,staged.emission.growth,0x15afa4);
        number(0x188,definition_.emission.emitRate,staged.emission.emitRate,0x15afc0);
        number(0x1a0,freshAgeAtBirth1A0_,meta.rawAgeAtBirth,0x15afd8);
        boundary_=0x15aff8u;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(freshAgeAtBirth1A0_)||
           !std::isfinite(definition_.emission.particleLife))return false;
        //Explicit native finite/nearest arithmetic, not an EE FPU equivalence.
        volatile float age=freshAgeAtBirth1A0_*definition_.emission.particleLife;
        if(!std::isfinite(age))return false;
        number(0x1a0,freshAgeAtBirth1A0_,age,0x15b008);
        boundary_=0x15b004u;if(!meta.velocityPresent)return false; //No invented4338B0.
        for(unsigned k=0;k<3;++k){boundary_=0x158c7cu+(k==0?0u:k==1?0x24u:0x48u);
            std::memcpy(&controls_.state.velocity60[k],&staged.authoredVelocity[k],4);knownFresh(0x60+4*k);}
        //158C00 writes XYZ only. Constructor W is retained; immutable staged
        //velocity is not copied into definition_.authoredVelocity as another live lane.
        boundary_=0x15b040u;controls_.mode1D8=meta.mode;knownFresh(0x1d8);
        number(0x1e0,freshDelayTime1E0_,meta.delayTime,0x15b03c);
        number(0x1dc,freshDelay1DC_,freshDelayTime1E0_,0x15b068);
        number(0x1a4,definition_.groundLevel,staged.groundLevel,0x15b064);
        number(0x1a8,definition_.gravityMultiplier,staged.gravityMultiplier,0x15b080);
        number(0x1ac,definition_.restitution,staged.restitution,0x15b0a0);
        boundary_=0x15b0a8u;
        //Exact authenticated podFire omits EmitMode (0). No fabricated RNG
        //or other mode's duration stores; legacy/general parser stays unchanged.
        if(controls_.mode1D8!=0u)return false;
        definition_.emission.randomMirrorX=staged.emission.randomMirrorX;knownFresh(0x1cc);boundary_=0x15b2a8u;
        definition_.emission.randomMirrorY=staged.emission.randomMirrorY;knownFresh(0x1d0);boundary_=0x15b2c0u;
        const std::uint32_t masks[]={2,4,8,0x1000,1,0x2000,0x20,0x800,0x200};
        const std::uint32_t pcs[]={0x15b2e0,0x15b30c,0x15b338,0x15b364,0x15b390,0x15b3bc,0x15b3e8,0x15b414,0x15b458};
        //Individual RMW source lanes, not grouped final flag assignment.
        for(unsigned i=0;i<9;++i){boundary_=pcs[i]+((staged.emission.particleFlags&masks[i])?0u:0x10u);definition_.emission.particleFlags=
            (definition_.emission.particleFlags&~masks[i])|(staged.emission.particleFlags&masks[i]);}
        number(0x1bc,definition_.emission.framesPerSet,staged.emission.framesPerSet,0x15b490);
        boundary_=0x15b4b0u;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(definition_.emission.framesPerSet)||
           definition_.emission.framesPerSet==0.f)return false;
        volatile float reciprocal=1.f/definition_.emission.framesPerSet;
        if(!std::isfinite(reciprocal))return false;
        number(0x1c0,freshInverseFrames1C0_,reciprocal,0x15b4c4);
        boundary_=0x15b4c0u;freshForcedCount1C8_=meta.forcedCount;knownFresh(0x1c8);
        boundary_=definition_.emission.framesPerSet>1.f?0x15b4f4u:0x15b504u;
        definition_.emission.particleFlags=(definition_.emission.particleFlags&~0x400u)|(staged.emission.particleFlags&0x400u);
        boundary_=0x15b508u;if(freshForcedCount1C8_>0)return false; //No forced allocation slice.
        //Actual source64 consumed after common reader, not stagedcachedY.
        number(0x220,definition_.emission.authoredVelocityY220,freshFloat(controls_.state.velocity60[1]),0x15e75c);
        boundary_=0x15e760u;
        if(!context_||!freshManager70_||*freshManager70_!=context_->particleManager||
           !context_->particleManager||!context_->particleManager->prepared())return false;
        const auto* group=context_->particleManager->group(definition_.emission.particleGroup);
        //Absent native lookup is not proof of source13CNULL/fallbackframes.
        if(!group){boundary_=0x15e770u;return false;}
        boundary_=0x15e784u;definition_.emission.lastFrame=group->last;knownFresh(0x1b4);
        boundary_=0x15e78cu;definition_.emission.firstFrame=group->first;knownFresh(0x1b0);
        boundary_=0x15e794u;freshCurrentFrame190_=definition_.emission.firstFrame;knownFresh(0x190);
        //Actual success15E02C/15E030 returns self at15E0B0, not fallback15E0AC.
        //No registerEmitter, runtime.manager grant, or fractional224 reset.
        definition_.parsed=true;freshReaderComplete_=true;boundary_=0x15e0b0u;return true;
    }
    bool appendFresh141960()noexcept {
        if(!freshReaderComplete_||boundary_!=0x15e0b0u||!preflightFreshUnappended())return false;
        boundary_=0x2cb770u;
        const auto result=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,
            component_,0x0402u,*binding_.actualAttachmentLock423F58);
        if(result.status!=OriginalWreckComponentStatus50LA::complete){boundary_=result.sourcePC;return false;}
        appended_=true;boundary_=0x2cb778u;return true;
    }
    // Read-only whole-forest admission; actual retirement remains strict tail-only.
    // Caller supplies the exact native slot and real source neighbors, never a
    // fabricated shortened domain or an already-retired successor.
    bool preflightFreshChainMember(std::size_t nativeDomainIndex,
        const OriginalWreckComponent50LA* exactPrevious,
        const OriginalWreckComponent50LA* exactNext)const noexcept {
        using namespace original_wreck_component_detail50LA;
        const auto& b=binding_;const auto& c=component_;
        if(!freshRoute_||!initialized_||retired_||!freshConstructorComplete_||prepared_||
           definition_.parsed!=freshReaderComplete_||runtime_.manager||runtime_.managerOwnerId||
           !freshForced1C4_||*freshForced1C4_||!freshAttachment74_||*freshAttachment74_||
           freshFlags1D4Known_!=7u||!b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||
           !b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||
           !b.domain->count||b.domain->count>b.domainCapacity||nativeDomainIndex>=b.domain->count||
           b.domainStorage[nativeDomainIndex]!=&c||!domain(*b.domain)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||
           !b.actualObjectCount423F60||!*b.actualObjectCount423F60||c.reference00!=1u||
           c.kind!=OriginalWreckComponentKind50LA::effect15DE00||c.firstChild18||
           c.nativeEffectControlId!=&controls_.id||controls_.id!=b.controlOwnerId||
           controls_.profile!=EffectProfile50BE::Fire15DE00)return false;
        if(appended_){
            if(!freshReaderComplete_||!b.particleParent||c.parent1C!=b.particleParent||
               !contains(*b.domain,c.parent1C)||c.previous10!=exactPrevious||c.next14!=exactNext)return false;
            if(exactPrevious){if(!contains(*b.domain,exactPrevious)||exactPrevious->reference00!=1u||
                exactPrevious->parent1C!=c.parent1C||exactPrevious->next14!=&c)return false;}
            else if(c.parent1C->firstChild18!=&c)return false;
            if(exactNext&&(!contains(*b.domain,exactNext)||exactNext->reference00!=1u||
                exactNext->parent1C!=c.parent1C||exactNext->previous10!=&c))return false;
        }else if(exactPrevious||exactNext||c.parent1C||c.previous10||c.next14||
            nativeDomainIndex+1u!=b.domain->count)return false;
        return !freshSetupAttempted_||(context_&&freshManager70_&&*freshManager70_==context_->particleManager&&
            context_->particleManager&&context_->particleManager->prepared());
    }
    bool preflightFreshUnappended(bool allowAppended=false)const noexcept {
        using namespace original_wreck_component_detail50LA;
        const auto& b=binding_;const auto& c=component_;
        if(!initialized_){
            //Native allocation/admission may fail before the first source141300.
            //No counted receipt or source defaults are invented for that cut.
            return !coldAttempted_&&!prepared_&&!definition_.parsed&&!appended_&&!runtime_.manager&&
                !runtime_.managerOwnerId&&!c.reference00&&c.kind==OriginalWreckComponentKind50LA::unbound&&
                !c.parent1C&&!c.previous10&&!c.next14&&!c.firstChild18&&!c.nativeEffectControlId;
        }
        if(!freshRoute_||!initialized_||retired_||!freshConstructorComplete_||prepared_||definition_.parsed!=freshReaderComplete_||
           (appended_&&!allowAppended)||runtime_.manager||runtime_.managerOwnerId||!freshForced1C4_||*freshForced1C4_||
           !freshAttachment74_||*freshAttachment74_||freshFlags1D4Known_!=7u||
           !b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||!b.domain->count||
           b.domain->count>b.domainCapacity||b.domainStorage[b.domain->count-1]!=&c||!domain(*b.domain)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||!b.actualObjectCount423F60||
           !*b.actualObjectCount423F60||c.reference00!=1u||c.kind!=OriginalWreckComponentKind50LA::effect15DE00||
           c.next14||c.firstChild18||c.nativeEffectControlId!=&controls_.id)return false;
        if(appended_){
            if(!freshReaderComplete_||c.parent1C!=b.particleParent||!contains(*b.domain,c.parent1C))return false;
            std::size_t remaining=b.domain->count;const OriginalWreckComponent50LA* previous=nullptr;
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
        return !freshSetupAttempted_||(context_&&freshManager70_&&*freshManager70_==context_->particleManager&&
            context_->particleManager&&context_->particleManager->prepared());
    }
    bool retireFreshUnappended(bool allowAppended=false)noexcept {
        if(retired_)return true;
        if(!preflightFreshUnappended(allowAppended))return false;
        if(!initialized_){retired_=true;return true;} //Native unused backing cancellation only.
        if(appended_){
            if(component_.previous10)component_.previous10->next14=nullptr;
            if(component_.parent1C->firstChild18==&component_)component_.parent1C->firstChild18=nullptr;
            //1424E0 tail retirement preserves actual parent/previous/164 history.
        }
        --component_.reference00; //104970 ->15DE60 ->15A720 genuine knownNULL1C4.
        component_.kind=OriginalWreckComponentKind50LA::entity141300;--*binding_.actualObjectCount423F60;
        --binding_.domain->count;component_.nativeEffectControlId=nullptr;
        retired_=true;boundary_=0x141418u;return true; //Keep source fields/manager/filename history.
    }
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
        if(freshRoute_||coldAttempted_)return false;coldAttempted_=true;
        if(!context.podFireEmt||!context.particleManager||!context.particleManager->prepared())return false;
        try {authored_=*context.podFireEmt;}catch(...){return false;}
        if(readOriginalGcPodFireDefinition50KG(authored_,
            definition_,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        context_=&context;bytes_=context.podFireEmt;prepared_=true;return true;
    }
    bool retireTail1424E0()noexcept {
        if(freshRoute_)return false;
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
