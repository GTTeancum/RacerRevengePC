#pragma once
#include "original_wreck_effects50la.h"
#include <exception>
#include <cfenv>
#include <cmath>
#include <limits>
#include <optional>
#include <string>

namespace ra_cold_roster50LA {class NativeWreckEffectChildren;}
namespace rr::game {
struct OriginalRacerSceneObject50LA;
class OriginalWreckRacerStorage50LA;
struct OriginalWreckSmokeBinding50LA {
    OriginalWreckComponentDomain50LA* domain=nullptr;
    OriginalWreckComponent50LA** domainStorage=nullptr;
    std::size_t domainCapacity=0;
    OriginalWreckComponent50LA* particleParent=nullptr;
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
    std::uint64_t componentKey=0;
    std::uint32_t controlOwnerId=0;
};
// A real external-forest owner, never embedded in or destroyed by piece160.
// This bounded constructor exposes no runtime entry before the later fire/
//Stop84/collection suffix is joined. Manager emitter rows outlive this object.
class OriginalWreckSmokeOwner50LA final {
    friend class OriginalWreckRacerStorage50LA;
    friend class ::ra_cold_roster50LA::NativeWreckEffectChildren;
    OriginalWreckComponent50LA component_;
    EffectOwner50BE controls_{};
    OriginalGcPodDustDefinition50KG definition_{};
    OriginalGcPodDustRuntime50KG runtime_{};
    const OriginalWreckEffectResourceContext50LA* context_=nullptr;
    const std::vector<std::uint8_t>* bytes_=nullptr;
    std::vector<std::uint8_t> authored_; //private immutable authenticated snapshot
    OriginalWreckSmokeBinding50LA binding_{};
    bool coldAttempted_=false,prepared_=false,initialized_=false,appended_=false,retired_=false;
    std::uint32_t boundary_=0;
    //Fresh source prefix: knownness metadata carries NO duplicate numeric state.
    //Existing controls/runtime/definition-backed scalar lanes remain sole owners.
    bool freshRoute_=false,freshSetupAttempted_=false,freshConstructorComplete_=false;
    bool freshReaderAttempted_=false,freshReaderComplete_=false,freshReciprocal1C0Opaque_=false;
    std::int32_t freshForcedCount1C8_=0; //Absent scalar lane; unknown until reader1C8.
    std::array<bool,0x260/4> freshKnown_{};
    std::uint8_t freshFlags1D4Known_=0;
    std::array<std::uint32_t,4> freshOffset50_{}; //Absent from canonical controls.
    std::uint32_t freshWord18C_=0,freshColor194_=0;
    float freshDelay1DC_=0;
    std::uint32_t freshNoise240W_=0,freshNoise250W_=0;
    std::optional<OriginalGcParticleManager50KG*> freshManager70_;
    std::optional<const OriginalRacerSceneObject50LA*> freshAttachment74_;
    std::optional<const void*> freshForced1C4_;
    std::array<char,256> freshFilename78_{}; //Conservative native bound, not guest capacity claim.
    std::size_t freshFilenameLength_=0;
    bool freshFilenameKnown_=false;
    const std::string* freshDirectory36B530_=nullptr;
    void knownFresh(unsigned offset)noexcept{freshKnown_[offset/4]=true;}
    void vectorKnownFresh(unsigned offset)noexcept{for(unsigned i=0;i<4;++i)knownFresh(offset+4*i);}
    static float freshFloat(std::uint32_t bits)noexcept{float value;std::memcpy(&value,&bits,4);return value;}
    bool constructFresh19A5E0(const OriginalWreckSmokeBinding50LA& b)noexcept {
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
        const auto result=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect19A5E0,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;controls_.id=b.controlOwnerId;controls_.profile=EffectProfile50BE::Particle19A5E0;
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
        number(0x17c,definition_.particleLife17C,0x457a0000u);
        number(0x180,definition_.growth180,0x40c00000u);
        number(0x184,definition_.initialSize184,0x3f800000u);
        number(0x188,definition_.emitRate188,0x41200000u);
        freshWord18C_=0;knownFresh(0x18c);
        controls_.state.flags1D4=std::uint8_t(controls_.state.flags1D4&~2u);freshFlags1D4Known_|=2u; //15A6B0
        definition_.randomMirrorX=false;knownFresh(0x1cc);
        definition_.randomMirrorY=false;knownFresh(0x1d0);
        runtime_.currentFrame190=0;knownFresh(0x190);
        controls_.state.flags1D4=std::uint8_t(controls_.state.flags1D4&~4u);freshFlags1D4Known_|=4u; //15A6CC
        freshColor194_=0xffffffu;knownFresh(0x194);
        definition_.particleFlags198=0;knownFresh(0x198);
        controls_.state.word19C=0;knownFresh(0x19c);
        number(0x1a0,definition_.ageAtBirth1A0,0);
        runtime_.firstFrame1B0=0;knownFresh(0x1b0);
        runtime_.lastFrame1B4=0;knownFresh(0x1b4);
        definition_.particleGroup1B8=-1;knownFresh(0x1b8);
        controls_.mode1D8=0;knownFresh(0x1d8);
        freshDelay1DC_=0;knownFresh(0x1dc);
        number(0x1e0,definition_.timing.delayTime1E0,0);
        controls_.duration1E4=0;knownFresh(0x1e4);
        number(0x1e8,definition_.timing.minTimeOn1E8,0);
        number(0x1ec,definition_.timing.maxTimeOn1EC,0);
        number(0x1f0,definition_.timing.minTimeOff1F0,0);
        number(0x1f4,definition_.timing.maxTimeOff1F4,0);
        freshForced1C4_=nullptr;knownFresh(0x1c4); //15A70C;1C8 remains unknown.
        auto noise=[&](unsigned offset,std::array<float,3>& xyz,std::uint32_t& w){
            xyz={{0.f,0.f,0.f}};w=0x3f800000u;vectorKnownFresh(offset);};
        noise(0x240,definition_.minimumVelocityNoise240,freshNoise240W_); //19A604
        noise(0x250,definition_.maximumVelocityNoise250,freshNoise250W_); //19A60C
        runtime_.remainder200=0;knownFresh(0x200); //19A610
        controls_.state.intensity204=0x3f800000u;knownFresh(0x204); //19A618
        number(0x208,definition_.minimumInitialSize208,0);
        number(0x20c,definition_.maximumInitialSize20C,0x40a00000u);
        number(0x210,definition_.initialSizeToSpeedRatio210,0x3c23d70au);
        number(0x214,definition_.minimumLife214,0x43fa0000u);
        number(0x218,definition_.maximumLife218,0x455ac000u);
        number(0x21c,definition_.minimumPercentLife21C,0x3f000000u);
        number(0x220,definition_.minimumPercentLifeConvergance220,0x3f800000u);
        number(0x224,definition_.lifeToSpeedRatio224,0x40a00000u);
        number(0x228,definition_.initialPercentAge228,0);
        number(0x22c,definition_.minimumEmitRate22C,0);
        number(0x230,definition_.maximumEmitRate230,0x3f800000u);
        number(0x234,definition_.particlesToDistanceRatio234,0x3c23d70au);
        number(0x238,definition_.particlesToSpeedRatio238,0);
        number(0x23c,definition_.minimumEmissionSpeed23C,0x41c80000u);
        noise(0x240,definition_.minimumVelocityNoise240,freshNoise240W_); //19A68C
        noise(0x250,definition_.maximumVelocityNoise250,freshNoise250W_); //19A6A4
        freshConstructorComplete_=true;boundary_=0x2cb700u;return true;
    }
    bool setupFresh19A720(const OriginalWreckEffectResourceContext50LA& context,
        const std::string* actualDirectory36B530)noexcept {
        if(!freshRoute_||!freshConstructorComplete_||!initialized_||freshSetupAttempted_||retired_||
           boundary_!=0x2cb700u||!context.particleManager||!context.particleManager->prepared())return false;
        freshSetupAttempted_=true;context_=&context;boundary_=0x19a744u;
        freshManager70_=context.particleManager; //15AC40;141C20 has no source field effects.
        //Actual a3/t0 are NULL:15AC3C skips74/50 writes, preserving constructor values.
        boundary_=0x19a788u;
        freshManager70_=context.particleManager; //19A790 in309230 call delay slot.
        if(!actualDirectory36B530)return false; //Missing authority is not synthetic empty prefix.
        freshDirectory36B530_=actualDirectory36B530;
        if(actualDirectory36B530->size()>=freshFilename78_.size()||actualDirectory36B530->find('\0')!=std::string::npos)return false;
        std::memcpy(freshFilename78_.data(),actualDirectory36B530->data(),actualDirectory36B530->size());
        freshFilenameLength_=actualDirectory36B530->size();freshFilename78_[freshFilenameLength_]=0;freshFilenameKnown_=true;
        boundary_=0x19a7c4u;
        constexpr char member[]="podWreckSmoke.emt";
        if(freshFilenameLength_+sizeof(member)>freshFilename78_.size())return false;
        std::memcpy(freshFilename78_.data()+freshFilenameLength_,member,sizeof(member));
        freshFilenameLength_+=sizeof(member)-1u;boundary_=0x19a7d0u;return true;
    }
    bool readFresh19A7D0(const std::vector<std::uint8_t>& actualEmt,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(!freshRoute_||!freshConstructorComplete_||!initialized_||retired_||appended_||
           freshReaderAttempted_||boundary_!=0x19a7d0u||!freshFilenameKnown_||
           !context_||context_->podWreckSmokeEmt!=&actualEmt||!freshManager70_||!*freshManager70_||
           *freshManager70_!=context_->particleManager||!context_->particleManager->prepared())return false;
        freshReaderAttempted_=true;boundary_=0x15ae2cu;
        //Authenticated native transport/admission, NOT guest Config/FileManager
        //execution or a fabricated Config-load failure. Staging is not live state.
        OriginalGcPodDustDefinition50KG staged;
        OriginalGcPodDustSourceMetadata50KG meta;
        if(readOriginalGcPodDustSourceDefinition50KG(actualEmt,OriginalGcPodDustMember50KG::wreckSmoke,
            staged,meta,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        auto number=[&](unsigned offset,float& lane,float value,std::uint32_t pc){
            boundary_=pc;lane=value;knownFresh(offset);};
        boundary_=0x15aee0u;definition_.particleGroup1B8=staged.particleGroup1B8;knownFresh(0x1b8);
        number(0x17c,definition_.particleLife17C,staged.particleLife17C,0x15af70);
        number(0x184,definition_.initialSize184,staged.initialSize184,0x15af8c);
        number(0x180,definition_.growth180,staged.growth180,0x15afa4);
        number(0x188,definition_.emitRate188,staged.emitRate188,0x15afc0);
        number(0x1a0,definition_.ageAtBirth1A0,meta.rawAgeAtBirth,0x15afd8);
        boundary_=0x15aff8u;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(definition_.ageAtBirth1A0)||
           !std::isfinite(definition_.particleLife17C))return false;
        //Explicit native finite/nearest arithmetic, not an EE FPU equivalence.
        volatile float age=definition_.ageAtBirth1A0*definition_.particleLife17C;
        if(!std::isfinite(age))return false;
        number(0x1a0,definition_.ageAtBirth1A0,age,0x15b008);
        boundary_=0x15b004u;if(!meta.velocityPresent)return false; //No invented4338B0.
        for(unsigned k=0;k<3;++k){boundary_=0x158c7cu+(k==0?0u:k==1?0x24u:0x48u);
            std::memcpy(&controls_.state.velocity60[k],&staged.velocity60[k],4);knownFresh(0x60+4*k);}
        //158C00 writes XYZ only. Constructor W is retained; immutable staged
        //velocity is not copied into definition_.velocity60 as another live lane.
        boundary_=0x15b040u;controls_.mode1D8=staged.timing.mode1D8;knownFresh(0x1d8);
        number(0x1e0,definition_.timing.delayTime1E0,staged.timing.delayTime1E0,0x15b03c);
        number(0x1dc,freshDelay1DC_,definition_.timing.delayTime1E0,0x15b068);
        number(0x1a4,definition_.groundLevel,staged.groundLevel,0x15b064);
        number(0x1a8,definition_.gravityMultiplier,staged.gravityMultiplier,0x15b080);
        number(0x1ac,definition_.restitution,staged.restitution,0x15b0a0);
        boundary_=0x15b0a8u;
        //Exact authenticated wreckSmoke omits EmitMode (0). No fabricated RNG
        //or other mode's duration stores; legacy/general parser stays unchanged.
        if(controls_.mode1D8!=0u)return false;
        definition_.randomMirrorX=staged.randomMirrorX;knownFresh(0x1cc);boundary_=0x15b2a8u;
        definition_.randomMirrorY=staged.randomMirrorY;knownFresh(0x1d0);boundary_=0x15b2c0u;
        const std::uint32_t masks[]={2,4,8,0x1000,1,0x2000,0x20,0x800,0x200};
        const std::uint32_t pcs[]={0x15b2e0,0x15b30c,0x15b338,0x15b364,0x15b390,0x15b3bc,0x15b3e8,0x15b414,0x15b458};
        //Individual RMW source lanes, not grouped final flag assignment.
        for(unsigned i=0;i<9;++i){boundary_=pcs[i]+((staged.particleFlags198&masks[i])?0u:0x10u);definition_.particleFlags198=
            (definition_.particleFlags198&~masks[i])|(staged.particleFlags198&masks[i]);}
        number(0x1bc,definition_.framesPerSet,staged.framesPerSet,0x15b490);
        boundary_=0x15b4b0u;
        if(definition_.framesPerSet!=0.f){
            if(std::fegetround()!=FE_TONEAREST||!std::isfinite(definition_.framesPerSet))return false;
            volatile float reciprocal=1.f/definition_.framesPerSet;
            if(!std::isfinite(reciprocal))return false;
            number(0x1c0,definition_.inverseFramesPerSet,reciprocal,0x15b4c4);
        }else{
            //Actual source performs1/0/store. EE result is deliberately opaque;
            //do not invent native0 or infinity as a source receipt. No consumer
            //may read1C0: source400 is clear and this owner remains runtime-unbound.
            freshKnown_[0x1c0/4]=false;freshReciprocal1C0Opaque_=true;
        }
        boundary_=0x15b4c0u;freshForcedCount1C8_=meta.forcedCount;knownFresh(0x1c8);
        boundary_=definition_.framesPerSet>1.f?0x15b4f4u:0x15b504u;
        definition_.particleFlags198=(definition_.particleFlags198&~0x400u)|(staged.particleFlags198&0x400u);
        boundary_=0x15b508u;if(freshForcedCount1C8_>0)return false; //No forced allocation slice.
        number(0x21c,definition_.minimumPercentLife21C,staged.minimumPercentLife21C,0x19b0a8);
        number(0x220,definition_.minimumPercentLifeConvergance220,staged.minimumPercentLifeConvergance220,0x19b168);
        number(0x208,definition_.minimumInitialSize208,staged.minimumInitialSize208,0x19b208);
        number(0x20c,definition_.maximumInitialSize20C,staged.maximumInitialSize20C,0x19b2ac);
        number(0x210,definition_.initialSizeToSpeedRatio210,staged.initialSizeToSpeedRatio210,0x19b354);
        number(0x214,definition_.minimumLife214,staged.minimumLife214,0x19b3f8);
        number(0x218,definition_.maximumLife218,staged.maximumLife218,0x19b4a0);
        number(0x224,definition_.lifeToSpeedRatio224,staged.lifeToSpeedRatio224,0x19b544);
        number(0x22c,definition_.minimumEmitRate22C,staged.minimumEmitRate22C,0x19b5e4);
        number(0x230,definition_.maximumEmitRate230,staged.maximumEmitRate230,0x19b688);
        number(0x234,definition_.particlesToDistanceRatio234,staged.particlesToDistanceRatio234,0x19b730);
        number(0x238,definition_.particlesToSpeedRatio238,staged.particlesToSpeedRatio238,0x19b7d0);
        number(0x228,definition_.initialPercentAge228,staged.initialPercentAge228,0x19b870);
        number(0x23c,definition_.minimumEmissionSpeed23C,staged.minimumEmissionSpeed23C,0x19b910);
        boundary_=0x19b9b4u;if(!meta.minimumNoisePresent)return false;
        for(unsigned k=0;k<3;++k)number(0x240+4*k,definition_.minimumVelocityNoise240[k],
            staged.minimumVelocityNoise240[k],0x158c7cu+(k==0?0u:k==1?0x24u:0x48u));
        boundary_=0x19b9d0u;if(!meta.maximumNoisePresent)return false;
        for(unsigned k=0;k<3;++k)number(0x250+4*k,definition_.maximumVelocityNoise250[k],
            staged.maximumVelocityNoise250[k],0x158c7cu+(k==0?0u:k==1?0x24u:0x48u));
        boundary_=0x19b9d8u;
        if(!context_||!freshManager70_||*freshManager70_!=context_->particleManager||
           !context_->particleManager||!context_->particleManager->prepared())return false;
        //Same retained manager table is native161EA0 projection. No registration
        //or remainder200 reset occurs in the original reader.
        const auto* group=context_->particleManager->group(definition_.particleGroup1B8);
        //This slice admits the actual retained group. A null native lookup
        //does not prove source13C=NULL or a valid guest161EA0 null return.
        if(!group){boundary_=0x19b9e8u;return false;}
        boundary_=0x19b9fcu;runtime_.lastFrame1B4=group->last;knownFresh(0x1b4);
        boundary_=0x19ba04u;runtime_.firstFrame1B0=group->first;knownFresh(0x1b0);
        boundary_=0x19ba0cu;runtime_.currentFrame190=runtime_.firstFrame1B0;knownFresh(0x190);
        //Successful native admitted reader corresponds to loaded Config return1.
        //No fabricated load-failure fallback growth/flag stores. Source Config
        //transport/destruction itself remains an explicit native adaptation.
        definition_.member=OriginalGcPodDustMember50KG::wreckSmoke;
        definition_.parsed=true;freshReaderComplete_=true;boundary_=0x19a7f8u;return true;
    }
    bool appendFresh141960()noexcept {
        if(!freshReaderComplete_||boundary_!=0x19a7f8u||!preflightFreshUnappended())return false;
        boundary_=0x2cb718u;
        const auto result=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,
            component_,0x0402u,*binding_.actualAttachmentLock423F58);
        if(result.status!=OriginalWreckComponentStatus50LA::complete){boundary_=result.sourcePC;return false;}
        appended_=true;boundary_=0x2cb720u;return true;
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
           definition_.parsed!=freshReaderComplete_||runtime_.bound||runtime_.managerOwnerId||
           !freshForced1C4_||*freshForced1C4_||!freshAttachment74_||*freshAttachment74_||
           freshFlags1D4Known_!=7u||!b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||
           !b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||
           !b.domain->count||b.domain->count>b.domainCapacity||nativeDomainIndex>=b.domain->count||
           b.domainStorage[nativeDomainIndex]!=&c||!domain(*b.domain)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||
           !b.actualObjectCount423F60||!*b.actualObjectCount423F60||c.reference00!=1u||
           c.kind!=OriginalWreckComponentKind50LA::effect19A5E0||c.firstChild18||
           c.nativeEffectControlId!=&controls_.id||controls_.id!=b.controlOwnerId||
           controls_.profile!=EffectProfile50BE::Particle19A5E0)return false;
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
    bool preflightFreshUnappended(bool allowAppended=false,
        const OriginalWreckComponent50LA* actualFollowingFire=nullptr)const noexcept {
        using namespace original_wreck_component_detail50LA;
        const auto& b=binding_;const auto& c=component_;
        if(!initialized_){
            //Native allocation/admission may fail before the first source141300.
            //No counted receipt or source defaults are invented for that cut.
            return !coldAttempted_&&!prepared_&&!definition_.parsed&&!appended_&&!runtime_.bound&&
                !runtime_.managerOwnerId&&!c.reference00&&c.kind==OriginalWreckComponentKind50LA::unbound&&
                !c.parent1C&&!c.previous10&&!c.next14&&!c.firstChild18&&!c.nativeEffectControlId;
        }
        if(!freshRoute_||!initialized_||retired_||!freshConstructorComplete_||prepared_||definition_.parsed!=freshReaderComplete_||
           (appended_&&!allowAppended)||runtime_.bound||runtime_.managerOwnerId||!freshForced1C4_||*freshForced1C4_||
           !freshAttachment74_||*freshAttachment74_||freshFlags1D4Known_!=7u||
           !b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||!b.domain->count||
           b.domain->count>b.domainCapacity||!domain(*b.domain)||
           (actualFollowingFire?(b.domain->count<2u||b.domainStorage[b.domain->count-2]!=&c||
             b.domainStorage[b.domain->count-1]!=actualFollowingFire):b.domainStorage[b.domain->count-1]!=&c)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||!b.actualObjectCount423F60||
           !*b.actualObjectCount423F60||c.reference00!=1u||c.kind!=OriginalWreckComponentKind50LA::effect19A5E0||
           c.next14!=(actualFollowingFire&&actualFollowingFire->parent1C?actualFollowingFire:nullptr)||
           c.firstChild18||c.nativeEffectControlId!=&controls_.id)return false;
        // Whole-owner preflight may include one exact later counted fire, before or after its append.
        // Actual smoke retirement still uses the strict tail-only default after fire retires.
        if(actualFollowingFire&&(!allowAppended||!appended_||
           actualFollowingFire->reference00!=1u||actualFollowingFire->kind!=OriginalWreckComponentKind50LA::effect15DE00||
           actualFollowingFire->sessionGeneration!=c.sessionGeneration||actualFollowingFire->next14||actualFollowingFire->firstChild18||
           (actualFollowingFire->parent1C?(actualFollowingFire->parent1C!=c.parent1C||actualFollowingFire->previous10!=&c):
             actualFollowingFire->previous10!=nullptr)))return false;
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
            //1424E0 tail retirement preserves source parent/previous/array history.
        }
        --component_.reference00; //104970 ->19A6C0 ->15A720 genuine knownNULL1C4.
        component_.kind=OriginalWreckComponentKind50LA::entity141300;--*binding_.actualObjectCount423F60;
        --binding_.domain->count;component_.nativeEffectControlId=nullptr;
        retired_=true;boundary_=0x141418u;return true; //Keep source fields/manager/filename history.
    }
    bool admit(const OriginalWreckSmokeBinding50LA& b)const noexcept {
        using namespace original_wreck_component_detail50LA;
        if(!prepared_||initialized_||retired_||!context_||!bytes_||context_->podWreckSmokeEmt!=bytes_||
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
    bool construct19A5E0(const OriginalWreckSmokeBinding50LA& b)noexcept {
        if(!admit(b))return false;binding_=b;boundary_=0x19a5e0u;
        const auto r=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect19A5E0,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;component_.nativeEffectControlId=&controls_.id;
        b.domainStorage[b.domain->count++]=&component_;
        //15A70C initializes1C4=NULL. Authenticated smoke reader admits no
        //positive forced-set count, so subsequent setup cannot allocate it.
        boundary_=0x2cb700u;return true;
    }
    bool setupAppend19A720()noexcept {
        if(!initialized_||appended_||retired_||boundary_!=0x2cb700u)return false;
        boundary_=0x19a720u;
        if(!assets::buildEffectControlOwner50BJ(binding_.controlOwnerId,EffectProfile50BE::Particle19A5E0,
            reinterpret_cast<const char*>(authored_.data()),authored_.size(),{},controls_))return false;
        //The control projection's fresh Stop is numerically inert. The source
        //post-fire Stop84 invocation at2CB78C remains unreached by this owner.
        const auto r=bindOriginalGcPodDust50KG(runtime_,definition_,*context_->particleManager,controls_);
        if(!r.okay()){boundary_=r.sourcePC;return false;}
        boundary_=0x2cb718u;
        const auto a=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,component_,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(a.status!=OriginalWreckComponentStatus50LA::complete){boundary_=a.sourcePC;return false;}
        appended_=true;boundary_=0x2cb720u;return true;
    }
public:
    OriginalWreckSmokeOwner50LA()=default;
    OriginalWreckSmokeOwner50LA(const OriginalWreckSmokeOwner50LA&)=delete;
    OriginalWreckSmokeOwner50LA& operator=(const OriginalWreckSmokeOwner50LA&)=delete;
    ~OriginalWreckSmokeOwner50LA(){if(initialized_&&!retired_)std::terminate();}
    bool prepareCold(const OriginalWreckEffectResourceContext50LA& context,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(freshRoute_||coldAttempted_)return false;coldAttempted_=true;
        if(!context.podWreckSmokeEmt||!context.particleManager||!context.particleManager->prepared())return false;
        try {authored_=*context.podWreckSmokeEmt;}catch(...){return false;}
        if(readOriginalGcPodDustDefinition50KG(authored_,
            OriginalGcPodDustMember50KG::wreckSmoke,definition_,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        context_=&context;bytes_=context.podWreckSmokeEmt;prepared_=true;return true;
    }
    bool retireTail1424E0()noexcept {
        using namespace original_wreck_component_detail50LA;
        if(freshRoute_)return false;
        if(retired_)return true;
        auto& b=binding_;auto& c=component_;
        if(!initialized_||!prepared_||!b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||
           !b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||
           !b.domain->count||b.domain->count>b.domainCapacity||b.domainStorage[b.domain->count-1]!=&c||!domain(*b.domain)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||
           !b.actualObjectCount423F60||!*b.actualObjectCount423F60||c.reference00!=1||
           c.kind!=OriginalWreckComponentKind50LA::effect19A5E0||c.firstChild18||c.next14)return false;
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
        --c.reference00; //104970, actual zero invokes19A6C0->15A720
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
