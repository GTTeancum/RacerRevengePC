#pragma once
#include "original_wreck_smoke_owner50la.h"
#include <exception>
#include <limits>

namespace rr::game {
class OriginalWreckRacerStorage50LA;
using OriginalWreckSplashBinding50LA=OriginalWreckSmokeBinding50LA;
// Actual piece16C splash, owned by the separate particle forest.
// No runtime entry is exposed before the collection suffix is joined.
class OriginalWreckSplashOwner50LA final {
    friend class OriginalWreckRacerStorage50LA;
    friend class ::ra_cold_roster50LA::NativeWreckEffectChildren;
    OriginalWreckComponent50LA component_;
    EffectOwner50BE controls_{};
    OriginalWreckSplashDefinition50LA definition_{};
    OriginalWreckSplashRuntime50LA runtime_{};
    const OriginalWreckEffectResourceContext50LA* context_=nullptr;
    const std::vector<std::uint8_t>* bytes_=nullptr;
    std::vector<std::uint8_t> authored_; //private immutable authenticated snapshot
    OriginalWreckSplashBinding50LA binding_{};
    bool coldAttempted_=false,prepared_=false,initialized_=false,appended_=false,retired_=false;
    std::uint32_t boundary_=0;
    //Fresh source prefix: knownness metadata carries NO duplicate numeric state.
    //Existing controls/runtime/definition-backed scalar lanes remain sole owners.
    bool freshRoute_=false,freshSetupAttempted_=false,freshConstructorComplete_=false;
    std::array<bool,0x260/4> freshKnown_{};
    std::uint8_t freshFlags1D4Known_=0;
    std::array<std::uint32_t,4> freshOffset50_{}; //Absent from canonical controls.
    std::uint32_t freshColor194_=0;
    float freshDelay1DC_=0;
    std::uint32_t freshRange220W_=0,freshRange230W_=0,freshOffset240W_=0;
    std::optional<const void*> freshSide250_;
    const std::array<std::uint32_t,4>* freshActual433F50_=nullptr;
    bool freshNativePartialCancelled_=false;
    bool freshReaderAttempted_=false,freshReaderComplete_=false;
    std::int32_t freshForcedCount1C8_=0;
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
    bool constructFresh19DBE0(const OriginalWreckSplashBinding50LA& b,
        const std::array<std::uint32_t,4>* actual433F50)noexcept {
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
        const auto result=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect19DBE0,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;controls_.id=b.controlOwnerId;controls_.profile=EffectProfile50BE::Splash19DBE0;
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
        runtime_.carriedSize18C=0;knownFresh(0x18c);
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
        auto derivedVector=[&](unsigned offset,std::array<float,3>& xyz,std::uint32_t& w){
            xyz={{0.f,0.f,0.f}};w=0x3f800000u;vectorKnownFresh(offset);};
        derivedVector(0x220,definition_.minRange220,freshRange220W_); //19DC04
        derivedVector(0x230,definition_.maxRange230,freshRange230W_); //19DC0C
        derivedVector(0x240,definition_.positionOffset240,freshOffset240W_); //19DC14
        number(0x200,definition_.sizeModifier200,0); //19DC18
        boundary_=0x19dc20u;
        if(!actual433F50)return false; //Retain genuine counted prefix;250 still unknown.
        freshActual433F50_=actual433F50;
        std::memcpy(controls_.state.velocity60,actual433F50->data(),16);vectorKnownFresh(0x60); //19DC28
        controls_.state.intensity204=0;knownFresh(0x204); //Actual fractional204 at19DC2C.
        freshSide250_=nullptr;knownFresh(0x250); //19DC30, NOT implied by backing defaults.
        freshConstructorComplete_=true;boundary_=0x2cb874u;return true;
    }
    bool setupFresh19DCD0(const OriginalWreckEffectResourceContext50LA& context,
        const std::string* actualDirectory36B530)noexcept {
        if(!freshRoute_||!freshConstructorComplete_||!initialized_||freshSetupAttempted_||retired_||
           boundary_!=0x2cb874u||!context.particleManager||!context.particleManager->prepared())return false;
        freshSetupAttempted_=true;context_=&context;boundary_=0x19dcfcu;
        freshManager70_=context.particleManager; //15AC40.
        //Actual a3/t0/t1 NULL: no side250 allocation or attachment74/offset50 writes.
        boundary_=0x19dd80u;freshManager70_=context.particleManager; //19DD88 call delay.
        if(!actualDirectory36B530)return false;
        freshDirectory36B530_=actualDirectory36B530;
        if(actualDirectory36B530->size()>=freshFilename78_.size()||actualDirectory36B530->find('\0')!=std::string::npos)return false;
        std::memcpy(freshFilename78_.data(),actualDirectory36B530->data(),actualDirectory36B530->size());
        freshFilenameLength_=actualDirectory36B530->size();freshFilename78_[freshFilenameLength_]=0;freshFilenameKnown_=true;
        boundary_=0x19ddbcu;constexpr char member[]="podWreckSplash.emt"; //Actual3C26D0.
        if(freshFilenameLength_+sizeof(member)>freshFilename78_.size())return false;
        std::memcpy(freshFilename78_.data()+freshFilenameLength_,member,sizeof(member));
        freshFilenameLength_+=sizeof(member)-1u;boundary_=0x19ddc8u;return true;
    }
    bool readFresh19DDC8(const std::vector<std::uint8_t>& actualEmt,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(!freshRoute_||!freshConstructorComplete_||!initialized_||retired_||appended_||
           freshReaderAttempted_||boundary_!=0x19ddc8u||!freshFilenameKnown_||
           !context_||context_->podWreckSplashEmt!=&actualEmt||!freshManager70_||!*freshManager70_||
           *freshManager70_!=context_->particleManager||!context_->particleManager->prepared())return false;
        freshReaderAttempted_=true;boundary_=0x15ae2cu;
        //Authenticated native transport/admission, NOT guest Config/FileManager
        //execution or a fabricated Config-load failure. Staging is not live state.
        OriginalWreckSplashDefinition50LA staged;
        OriginalWreckSplashSourceMetadata50LA meta;
        if(readOriginalWreckSplashSourceDefinition50LA(actualEmt,
            staged,meta,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        auto number=[&](unsigned offset,float& lane,float value,std::uint32_t pc){
            boundary_=pc;lane=value;knownFresh(offset);};
        auto xyz=[&](unsigned offset,std::array<float,3>& lane,const std::array<float,3>& value,
            bool present,std::uint32_t callPC){
            boundary_=callPC;if(!present)return false; //No unproved default-global read.
            for(unsigned k=0;k<3;++k)number(offset+4*k,lane[k],value[k],0x158c7cu+(k==0?0u:k==1?0x24u:0x48u));
            return true; //158C00 preserves each constructor W lane.
        };
        if(!xyz(0x220,definition_.minRange220,staged.minRange220,meta.minRangePresent,0x19ea30)||
           !xyz(0x230,definition_.maxRange230,staged.maxRange230,meta.maxRangePresent,0x19ea4c)||
           !xyz(0x240,definition_.positionOffset240,staged.positionOffset240,meta.positionOffsetPresent,0x19ea68))return false;
        number(0x208,definition_.minStreamHeight208,staged.minStreamHeight208,0x19ea84);
        number(0x20c,definition_.maxStreamHeight20C,staged.maxStreamHeight20C,0x19eaa0);
        number(0x200,definition_.sizeModifier200,staged.sizeModifier200,0x19eabc);
        number(0x210,definition_.fillerLifeModifier210,staged.fillerLifeModifier210,0x19eadc);
        number(0x214,definition_.fillerSizeModifier214,staged.fillerSizeModifier214,0x19eafc);
        number(0x218,definition_.fillerVelocityModifier218,staged.fillerVelocityModifier218,0x19eb18);
        number(0x21c,definition_.sideVelocityModifier21C,staged.sideVelocityModifier21C,0x19eb34);
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
        //158C00 writes XYZ only. Constructor W0 is retained; immutable staged
        //velocity is not copied into definition_.velocity60 as another live lane.
        boundary_=0x15b040u;controls_.mode1D8=staged.timing.mode1D8;knownFresh(0x1d8);
        number(0x1e0,definition_.timing.delayTime1E0,staged.timing.delayTime1E0,0x15b03c);
        number(0x1dc,freshDelay1DC_,definition_.timing.delayTime1E0,0x15b068);
        number(0x1a4,definition_.groundLevel,staged.groundLevel,0x15b064);
        number(0x1a8,definition_.gravityMultiplier,staged.gravityMultiplier,0x15b080);
        number(0x1ac,definition_.restitution,staged.restitution,0x15b0a0);
        boundary_=0x15b0a8u;
        //Actual EmitMode1 reads OneShotTime into1EC, then copies SAME lane to1E4.
        //No random/timed branch or unrelated min/max/off stores are executed.
        if(controls_.mode1D8!=1u)return false;
        number(0x1ec,definition_.timing.maxTimeOn1EC,staged.timing.maxTimeOn1EC,0x15b0c8);
        boundary_=0x15b0d8u;
        //LWC1/SWC1 copies float bits into the canonical uint32 duration lane.
        std::memcpy(&controls_.duration1E4,&definition_.timing.maxTimeOn1EC,4);knownFresh(0x1e4);
        definition_.randomMirrorX=staged.randomMirrorX;knownFresh(0x1cc);boundary_=0x15b2a8u;
        definition_.randomMirrorY=staged.randomMirrorY;knownFresh(0x1d0);boundary_=0x15b2c0u;
        const std::uint32_t masks[]={2,4,8,0x1000,1,0x2000,0x20,0x800,0x200};
        const std::uint32_t pcs[]={0x15b2e0,0x15b30c,0x15b338,0x15b364,0x15b390,0x15b3bc,0x15b3e8,0x15b414,0x15b458};
        //Individual RMW source lanes, not grouped final flag assignment.
        for(unsigned i=0;i<9;++i){boundary_=pcs[i]+((staged.particleFlags198&masks[i])?0u:0x10u);definition_.particleFlags198=
            (definition_.particleFlags198&~masks[i])|(staged.particleFlags198&masks[i]);}
        number(0x1bc,definition_.framesPerSet,staged.framesPerSet,0x15b490);
        boundary_=0x15b4b0u;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(definition_.framesPerSet)||definition_.framesPerSet==0.f)return false;
        volatile float reciprocal=1.f/definition_.framesPerSet;
        if(!std::isfinite(reciprocal))return false;
        number(0x1c0,definition_.inverseFramesPerSet,reciprocal,0x15b4c4);
        boundary_=0x15b4c0u;freshForcedCount1C8_=meta.forcedCount;knownFresh(0x1c8);
        boundary_=definition_.framesPerSet>1.f?0x15b4f4u:0x15b504u;
        definition_.particleFlags198=(definition_.particleFlags198&~0x400u)|(staged.particleFlags198&0x400u);
        boundary_=0x15b508u;if(freshForcedCount1C8_>0)return false; //No forced allocation slice.
        boundary_=0x19eb48u;
        if(!context_||!freshManager70_||*freshManager70_!=context_->particleManager||
           !context_->particleManager||!context_->particleManager->prepared())return false;
        //Same retained manager table is native161EA0 projection. No registration
        //or fractional204/carriedSize18C reset occurs in the original reader.
        const auto* group=context_->particleManager->group(definition_.particleGroup1B8);
        //This slice admits the actual retained group. A null native lookup
        //does not prove source13C=NULL or a valid guest161EA0 null return.
        if(!group){boundary_=0x19eb58u;return false;}
        boundary_=0x19eb6cu;runtime_.lastFrame1B4=group->last;knownFresh(0x1b4);
        boundary_=0x19eb74u;runtime_.firstFrame1B0=group->first;knownFresh(0x1b0);
        boundary_=0x19eb78u;runtime_.currentFrame190=runtime_.firstFrame1B0;knownFresh(0x190);
        //Successful native admitted reader corresponds to loaded Config return1.
        //No fabricated load-failure fallback growth/flag stores. Source Config
        //transport/destruction itself remains an explicit native adaptation.
        definition_.parsed=true;freshReaderComplete_=true;boundary_=0x19de7cu;return true;
    }
    bool stopFresh15AD00()noexcept {
        if(!freshReaderComplete_||boundary_!=0x19de7cu||!preflightFreshUnappended())return false;
        stopEffect50BC(controls_.state);boundary_=0x2cb8b4u;return true;
    }
    bool appendFresh141960()noexcept {
        if(!freshReaderComplete_||boundary_!=0x2cb8b4u||!preflightFreshUnappended())return false;
        boundary_=0x2cb8bcu;
        const auto result=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,
            component_,0x0402u,*binding_.actualAttachmentLock423F58);
        if(result.status!=OriginalWreckComponentStatus50LA::complete){boundary_=result.sourcePC;return false;}
        appended_=true;boundary_=0x2cb8c4u;return true;
    }
    bool preflightFreshChainMember(std::size_t nativeDomainIndex,
        const OriginalWreckComponent50LA* exactPrevious,
        const OriginalWreckComponent50LA* exactNext)const noexcept {
        using namespace original_wreck_component_detail50LA;
        const auto& b=binding_;const auto& c=component_;
        if(!initialized_||retired_)return false;
        if(!appended_){
            return !exactPrevious&&!exactNext&&b.domain&&nativeDomainIndex<b.domain->count&&nativeDomainIndex+1u==b.domain->count&&
                preflightFreshUnappended();
        }
        if(!freshRoute_||retired_||prepared_||definition_.parsed!=freshReaderComplete_||runtime_.bound||
           runtime_.managerIdentity||runtime_.managerOwnerId||!freshForced1C4_||*freshForced1C4_||
           !freshAttachment74_||*freshAttachment74_||freshFlags1D4Known_!=7u||
           !b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||!b.domainCapacity||
           b.domainCapacity>maxOriginalWreckComponents50LA||!b.domain->count||b.domain->count>b.domainCapacity||
           nativeDomainIndex>=b.domain->count||b.domainStorage[nativeDomainIndex]!=&c||!domain(*b.domain)||
           !b.particleParent||!contains(*b.domain,b.particleParent)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||
           !b.actualObjectCount423F60||!*b.actualObjectCount423F60||c.reference00!=1u||
           c.kind!=OriginalWreckComponentKind50LA::effect19DBE0||c.firstChild18||
           c.nativeEffectControlId!=&controls_.id||controls_.id!=b.controlOwnerId||
           controls_.profile!=EffectProfile50BE::Splash19DBE0)return false;
        if(!freshReaderComplete_||c.parent1C!=b.particleParent||
           c.previous10!=exactPrevious||c.next14!=exactNext)return false;
        if(exactPrevious){if(!contains(*b.domain,exactPrevious)||exactPrevious->reference00!=1u||
            exactPrevious->parent1C!=c.parent1C||exactPrevious->next14!=&c)return false;}
        else if(c.parent1C->firstChild18!=&c)return false;
        if(exactNext&&(!contains(*b.domain,exactNext)||exactNext->reference00!=1u||
            exactNext->parent1C!=c.parent1C||exactNext->previous10!=&c))return false;
        if(!freshConstructorComplete_){
            //Only supported incomplete cut: missing actual global BEFORE19DC28/250.
            return boundary_==0x19dc20u&&!freshActual433F50_&&!freshSide250_&&!freshSetupAttempted_&&
                freshKnown_[0x200/4]&&!freshKnown_[0x204/4]&&!freshKnown_[0x250/4];
        }
        if(!freshActual433F50_||!freshSide250_||*freshSide250_)return false;
        return !freshSetupAttempted_||(context_&&freshManager70_&&*freshManager70_==context_->particleManager&&
            context_->particleManager&&context_->particleManager->prepared());
    }
    bool preflightFreshUnappended(bool allowAppended=false)const noexcept {
        using namespace original_wreck_component_detail50LA;
        const auto& b=binding_;const auto& c=component_;
        if(!initialized_)return !coldAttempted_&&!prepared_&&!definition_.parsed&&!appended_&&!runtime_.bound&&
            !runtime_.managerIdentity&&!runtime_.managerOwnerId&&!c.reference00&&
            c.kind==OriginalWreckComponentKind50LA::unbound&&!c.parent1C&&!c.previous10&&!c.next14&&
            !c.firstChild18&&!c.nativeEffectControlId;
        if(!freshRoute_||retired_||prepared_||definition_.parsed!=freshReaderComplete_||(appended_&&!allowAppended)||runtime_.bound||
           runtime_.managerIdentity||runtime_.managerOwnerId||!freshForced1C4_||*freshForced1C4_||
           !freshAttachment74_||*freshAttachment74_||freshFlags1D4Known_!=7u||
           !b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||!b.domainCapacity||
           b.domainCapacity>maxOriginalWreckComponents50LA||!b.domain->count||b.domain->count>b.domainCapacity||
           b.domainStorage[b.domain->count-1u]!=&c||!domain(*b.domain)||
           !b.particleParent||!contains(*b.domain,b.particleParent)||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||
           !b.actualObjectCount423F60||!*b.actualObjectCount423F60||c.reference00!=1u||
           c.kind!=OriginalWreckComponentKind50LA::effect19DBE0||c.next14||c.firstChild18||
           c.nativeEffectControlId!=&controls_.id||controls_.id!=b.controlOwnerId||
           controls_.profile!=EffectProfile50BE::Splash19DBE0)return false;
        if(appended_){
            if(!freshReaderComplete_||c.parent1C!=b.particleParent)return false;
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
        if(!freshConstructorComplete_){
            //Only supported incomplete cut: missing actual global BEFORE19DC28/250.
            return boundary_==0x19dc20u&&!freshActual433F50_&&!freshSide250_&&!freshSetupAttempted_&&
                freshKnown_[0x200/4]&&!freshKnown_[0x204/4]&&!freshKnown_[0x250/4];
        }
        if(!freshActual433F50_||!freshSide250_||*freshSide250_)return false;
        return !freshSetupAttempted_||(context_&&freshManager70_&&*freshManager70_==context_->particleManager&&
            context_->particleManager&&context_->particleManager->prepared());
    }
    bool retireFreshUnappended(bool allowAppended=false)noexcept {
        if(retired_)return true;
        if(!preflightFreshUnappended(allowAppended))return false;
        if(!initialized_){retired_=true;return true;}
        if(appended_){
            if(component_.previous10)component_.previous10->next14=nullptr;
            if(component_.parent1C->firstChild18==&component_)component_.parent1C->firstChild18=nullptr;
            //Preserve source parent/previous/16C history after actual tail unlink.
        }
        //Full constructor: genuine NULL1C4/250 branches. Incomplete19DC20:
        //native counted-prefix cancellation ONLY, never read unknown250 or
        //claim execution of completed19DC50 guest destructor. Keep all knownness.
        freshNativePartialCancelled_=!freshConstructorComplete_;
        --component_.reference00;component_.kind=OriginalWreckComponentKind50LA::entity141300;
        --*binding_.actualObjectCount423F60;--binding_.domain->count;
        component_.nativeEffectControlId=nullptr;retired_=true;
        if(!freshNativePartialCancelled_)boundary_=0x141418u;
        return true;
    }
    bool admit(const OriginalWreckSplashBinding50LA& b)const noexcept {
        using namespace original_wreck_component_detail50LA;
        if(!prepared_||initialized_||retired_||!context_||!bytes_||context_->podWreckSplashEmt!=bytes_||
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
    bool construct19DBE0(const OriginalWreckSplashBinding50LA& b)noexcept {
        if(!admit(b))return false;binding_=b;boundary_=0x19dbe0u;
        const auto r=initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect19DBE0,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        initialized_=true;component_.nativeEffectControlId=&controls_.id;
        b.domainStorage[b.domain->count++]=&component_;
        //15A70C initializes1C4=NULL. Authenticated splash reader admits no
        //positive forced-set count, so subsequent setup cannot allocate it.
        //19DC30 sets250=NULL; actual19DCD0 a3=0 leaves it unallocated.
        boundary_=0x2cb874u;return true;
    }
    bool setupStop19DCD0()noexcept {
        if(!initialized_||appended_||retired_||boundary_!=0x2cb874u)return false;
        boundary_=0x19dcd0u;
        if(!assets::buildEffectControlOwner50BJ(binding_.controlOwnerId,EffectProfile50BE::Splash19DBE0,
            reinterpret_cast<const char*>(authored_.data()),authored_.size(),{},controls_))return false;
        const auto r=bindOriginalWreckSplash50LA(runtime_,definition_,*context_->particleManager,controls_);
        if(!r.okay()){boundary_=r.sourcePC;return false;}
        stopEffect50BC(controls_.state); //actual virtual84 at2CB8AC
        boundary_=0x2cb8b4u;return true;
    }
    bool append141960()noexcept {
        if(!initialized_||appended_||retired_||boundary_!=0x2cb8b4u)return false;
        boundary_=0x2cb8bcu;
        const auto a=appendOriginalWreckComponent50LA(*binding_.domain,*binding_.particleParent,component_,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(a.status!=OriginalWreckComponentStatus50LA::complete){boundary_=a.sourcePC;return false;}
        appended_=true;boundary_=0x2cb8c4u;return true;
    }
public:
    OriginalWreckSplashOwner50LA()=default;
    OriginalWreckSplashOwner50LA(const OriginalWreckSplashOwner50LA&)=delete;
    OriginalWreckSplashOwner50LA& operator=(const OriginalWreckSplashOwner50LA&)=delete;
    ~OriginalWreckSplashOwner50LA(){if(initialized_&&!retired_)std::terminate();}
    bool prepareCold(const OriginalWreckEffectResourceContext50LA& context,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        if(freshRoute_||coldAttempted_)return false;coldAttempted_=true;
        if(!context.podWreckSplashEmt||!context.particleManager||!context.particleManager->prepared())return false;
        try {authored_=*context.podWreckSplashEmt;}catch(...){return false;}
        if(readOriginalWreckSplashDefinition50LA(authored_,definition_,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        context_=&context;bytes_=context.podWreckSplashEmt;prepared_=true;return true;
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
           c.kind!=OriginalWreckComponentKind50LA::effect19DBE0||c.firstChild18||c.next14)return false;
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
        --c.reference00; //104970, actual zero invokes19DC50->15A720
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
