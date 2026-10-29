#pragma once
#include "original_racer_ember_definition50la.h"
#include "original_wreck_smoke_owner50la.h"
#include <memory>
#include <optional>

namespace ra_cold_roster50LA {class NativeWreckEffectChildren;}
namespace rr::game {
// Load-only19BA50 +15B860 composite. Numeric runtime dispatch is deliberately
// unavailable. Every represented numeric source lane has one authority here;
// unknown bits are copied as unknown, never supplied as synthetic zero data.
class OriginalRacerEmberOwner50LA final {
    friend class ::ra_cold_roster50LA::NativeWreckEffectChildren;
    struct Word {std::uint32_t value=0,known=0;};
    struct Fields {
        std::array<Word,0x380/4> words{};
        OriginalGcParticleManager50KG* manager70=nullptr;
        bool managerKnown=false,attachment74NullKnown=false,forced1C4NullKnown=false;
        std::string filename78;
        Word& at(unsigned offset)noexcept{return words[offset/4];}
        const Word& at(unsigned offset)const noexcept{return words[offset/4];}
        void put(unsigned offset,std::uint32_t value)noexcept{at(offset)={value,0xffffffffu};}
        void number(unsigned offset,float value)noexcept{std::uint32_t v;std::memcpy(&v,&value,4);put(offset,v);}
        void masked(unsigned offset,std::uint32_t mask,std::uint32_t value)noexcept{
            auto& w=at(offset);w.value=(w.value&~mask)|(value&mask);w.known|=mask;
        }
        void copy(unsigned offset,const Fields& other)noexcept{at(offset)=other.at(offset);}
        void vector(unsigned offset,const std::array<float,3>& v)noexcept{
            for(unsigned i=0;i<3;++i)number(offset+4*i,v[i]);put(offset+12,0x3f800000u);
        }
        void zeroVector(unsigned offset)noexcept{vector(offset,{});}
        void copyVector(unsigned offset,const Fields& other)noexcept{
            for(unsigned i=0;i<4;++i)copy(offset+4*i,other);
        }
        void construct15A5B0()noexcept{
            zeroVector(0x30);zeroVector(0x40);zeroVector(0x50);zeroVector(0x60);
            zeroVector(0x30);zeroVector(0x40);zeroVector(0x50);zeroVector(0x60);
            masked(0x1d4,1,1);manager70=nullptr;managerKnown=true;attachment74NullKnown=true;
            put(0x17c,0x457a0000);put(0x180,0x40c00000);put(0x184,0x3f800000);put(0x188,0x41200000);
            put(0x18c,0);masked(0x1d4,2,0);put(0x1cc,0);put(0x1d0,0);put(0x190,0);
            masked(0x1d4,4,0);put(0x194,0xffffff);put(0x198,0);put(0x19c,0);put(0x1a0,0);
            put(0x1b0,0);put(0x1b4,0);put(0x1b8,0xffffffffu);
            for(unsigned offset=0x1d8;offset<=0x1f4;offset+=4)put(offset,0);
            forced1C4NullKnown=true;
        }
        void stop15AD00()noexcept{
            masked(0x1d4,2,0);masked(0x1d4,4,0);
            for(unsigned i=0;i<4;++i)at(0x40+4*i)=at(0x30+4*i);
            put(0x19c,0);
        }
        bool clone15B630(const Fields& from){
            // The actual caller passes a2=0: no30/40/50/74 copy.
            if(!forced1C4NullKnown||!from.forced1C4NullKnown||
               from.at(0x1c8).known!=0xffffffffu||from.at(0x1c8).value)return false;
            copyVector(0x60,from);masked(0x1d4,1,from.at(0x1d4).value);
            manager70=from.manager70;managerKnown=from.managerKnown;filename78=from.filename78;
            for(unsigned o:{0x17cu,0x180u,0x184u,0x188u,0x18cu})copy(o,from);
            masked(0x1d4,2,from.at(0x1d4).value);
            for(unsigned o:{0x1ccu,0x1d0u,0x190u})copy(o,from);
            masked(0x1d4,4,from.at(0x1d4).value);
            for(unsigned o:{0x194u,0x198u,0x19cu,0x1a0u,0x1b0u,0x1b4u,0x1b8u,0x1a4u,0x1a8u,0x1acu,
                0x1d8u,0x1dcu,0x1e0u,0x1e4u,0x1e8u,0x1ecu,0x1f0u,0x1f4u,0x1bcu,0x1c0u,0x1c8u})copy(o,from);
            return true; //genuine fresh NULL1C4 and copied zero count branch
        }
    } parent_,smoke_;
    OriginalWreckComponent50LA component_,smokeComponent_;
    std::uint32_t controlId_=0,smokeControlId_=0;
    OriginalWreckSmokeBinding50LA binding_{};
    OriginalRacerEmberDefinition50LA fireDefinition_,smokeDefinition_;
    std::unique_ptr<std::array<std::uint32_t,5>> controller334_,controller338_,controller33C_;
    // Real typed pointers, source publication separate from native allocation.
    OriginalWreckComponent50LA* smoke330_=nullptr;
    std::array<std::uint32_t,5>* source334_=nullptr,*source338_=nullptr,*source33C_=nullptr;
    std::string smokeFilename22C_;
    std::uint32_t managerEmitter_=0,smokeManagerEmitter_=0;
    bool prepared_=false,attempted_=false,constructed_=false,smokeConstructed_=false;
    bool smokeAppended_=false,appended_=false,complete_=false,retired_=false;
    std::uint32_t pc_=0x19ba50u;
    bool applyCommon(Fields& fields,const OriginalRacerEmberDefinition50LA& f)noexcept{
        if(!f.parsed||f.timing.mode1D8>1)return false;
        fields.put(0x1b8,static_cast<std::uint32_t>(f.particleGroup1B8));
        fields.number(0x17c,f.particleLife17C);fields.number(0x184,f.initialSize184);
        fields.number(0x180,f.growth180);fields.number(0x188,f.emitRate188);
        fields.number(0x1a0,f.authoredAgeFraction1A0);fields.number(0x1a0,f.ageAtBirth1A0);fields.vector(0x60,f.velocity60);
        fields.put(0x1d8,f.timing.mode1D8);fields.number(0x1e0,f.timing.delayTime1E0);
        fields.number(0x1dc,f.timing.delayTime1E0);fields.number(0x1a4,f.groundLevel);
        fields.number(0x1a8,f.gravityMultiplier);fields.number(0x1ac,f.restitution);
        if(f.timing.mode1D8==1){fields.number(0x1ec,f.timing.maxTimeOn1EC);fields.number(0x1e4,f.timing.maxTimeOn1EC);}
        fields.put(0x1cc,f.randomMirrorX?1u:0u);fields.put(0x1d0,f.randomMirrorY?1u:0u);
        for(unsigned mask:{2u,4u,8u,0x1000u,1u,0x2000u,0x20u,0x800u,0x200u})
            fields.masked(0x198,mask,f.particleFlags198);
        fields.number(0x1bc,f.framesPerSet);
        // Inactive smoke FramesPerSet0 divide has no consuming branch. Keep
        // source1C0 unknown instead of promoting native zero to guest data.
        if(f.framesPerSet!=0)fields.number(0x1c0,f.inverseFramesPerSet);
        fields.put(0x1c8,0);fields.masked(0x198,0x400,f.particleFlags198);
        return true;
    }
    bool registerNative(Fields& fields,const OriginalRacerEmberDefinition50LA& f,std::uint32_t& handle,bool sourceLookup=true)noexcept{
        if(!fields.managerKnown||!fields.manager70||!fields.manager70->prepared())return false;
        const auto* group=fields.manager70->group(f.particleGroup1B8);if(!group)return false;
        if(sourceLookup){fields.put(0x1b4,static_cast<std::uint32_t>(group->last));
        fields.put(0x1b0,static_cast<std::uint32_t>(group->first));fields.put(0x190,static_cast<std::uint32_t>(group->first));}
        OriginalGcParticleEmitterState50KG value;value.particleGroup=f.particleGroup1B8;
        value.framesPerSet=f.framesPerSet;value.inverseFramesPerSet=f.inverseFramesPerSet;
        value.gravity=f.gravityMultiplier;value.bouncePlane=f.groundLevel;value.bounceFactor=f.restitution;
        handle=fields.manager70->registerEmitter(value);return handle!=0;
    }
    bool allocateControllers(){
        // Host allocation refusal is a retained native admission failure,
        // not the original malloc-NULL continuation. Each record is initialized
        // in actual store order0,4,C,10,8 before its pointer publication.
        auto one=[](auto& owner,auto*& source){
            owner=std::make_unique<std::array<std::uint32_t,5>>();
            (*owner)[0]=0;(*owner)[1]=0x3ba3d70a;(*owner)[3]=0x7f7fffff;
            (*owner)[4]=0xff7fffff;(*owner)[2]=0x3f800000;source=owner.get();
        };
        if(source334_||source338_||source33C_)return false;
        one(controller334_,source334_);one(controller338_,source338_);one(controller33C_,source33C_);return true;
    }
    bool drawTail(std::uint32_t& actualRaw)noexcept{
        using Math=water_wake_detail50jc::Math;Math m;
        const float a=drawOriginalGcParticleRandom50KG(actualRaw);
        const float b=drawOriginalGcParticleRandom50KG(actualRaw);
        const float c=drawOriginalGcParticleRandom50KG(actualRaw);
        const float twiceC=m.mul(c,2,0x19bea8);
        const float twiceA=m.mul(a,2,0x19beac);
        const float x=m.sub(twiceC,1,0x19beb8);
        const float z=m.sub(twiceA,1,0x19bec0);
        if(m.badPC){pc_=m.badPC;return false;}
        parent_.vector(0x360,{{x,b,z}});
        const float d=drawOriginalGcParticleRandom50KG(actualRaw);parent_.number(0x370,d);return true;
    }
    bool prepareCold(const std::vector<std::uint8_t>& fire,const std::vector<std::uint8_t>& smoke,
        assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept{
        if(prepared_||attempted_)return false;
        if(original_racer_ember_definition_detail50LA::read(fire,false,fireDefinition_,diagnostic)!=OriginalGcSprayStatus50KG::complete||
           original_racer_ember_definition_detail50LA::read(smoke,true,smokeDefinition_,diagnostic)!=OriginalGcSprayStatus50KG::complete)return false;
        prepared_=true;return true;
    }
    bool prepareClone(const OriginalRacerEmberOwner50LA& prototype)noexcept{
        if(prepared_||attempted_||!prototype.complete_||prototype.retired_||
           !prototype.smokeConstructed_||prototype.smoke330_!=&prototype.smokeComponent_)return false;
        fireDefinition_=prototype.fireDefinition_;smokeDefinition_=prototype.smokeDefinition_;prepared_=true;return true;
    }
    bool construct19BA50(const OriginalWreckSmokeBinding50LA& b)noexcept{
        using namespace original_wreck_component_detail50LA;
        if(!prepared_||attempted_||!b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||
           !b.domainCapacity||b.domainCapacity>maxOriginalWreckComponents50LA||b.domain->count>=b.domainCapacity||
           !domain(*b.domain)||!b.particleParent||!contains(*b.domain,b.particleParent)||
           !b.actualObjectCount423F60||*b.actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max()||
           !b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||!b.componentKey||!b.controlOwnerId)return false;
        for(std::size_t i=0;i<b.domain->count;++i){const auto* c=b.domainStorage[i];
            if(!effectControlIdentityBound(*c)||c->objectKey==b.componentKey||
               (c->nativeEffectControlId&&*c->nativeEffectControlId==b.controlOwnerId))return false;}
        attempted_=true;binding_=b;controlId_=b.controlOwnerId;
        if(initializeOriginalWreckComponent50LA(component_,OriginalWreckComponentKind50LA::effect19BA50,
            b.domain->sessionGeneration,b.componentKey,true,*b.actualObjectCount423F60).status!=OriginalWreckComponentStatus50LA::complete)return false;
        constructed_=true;component_.nativeEffectControlId=&controlId_;b.domainStorage[b.domain->count++]=&component_;
        parent_.construct15A5B0();parent_.zeroVector(0x200);parent_.zeroVector(0x210);parent_.zeroVector(0x360);
        smoke330_=nullptr;source334_=nullptr;source338_=nullptr;source33C_=nullptr;
        for(unsigned i=0;i<4;++i)parent_.at(0x40+4*i)=parent_.at(0x30+4*i);
        parent_.put(0x19c,0);parent_.put(0x228,1);parent_.masked(0x198,0x100,0x100);
        pc_=0x212c74u;return true;
    }
    bool constructSmoke(std::uint32_t id)noexcept{
        auto& b=binding_;pc_=0x15b860u;
        if(!id||id==controlId_||!b.domain||!original_wreck_component_detail50LA::domain(*b.domain)||
           b.domain->count>=b.domainCapacity||!b.actualObjectCount423F60||
           *b.actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max())return false;
        const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&smokeComponent_));
        for(std::size_t i=0;i<b.domain->count;++i)if(b.domainStorage[i]->objectKey==key||
            (b.domainStorage[i]->nativeEffectControlId&&*b.domainStorage[i]->nativeEffectControlId==id))return false;
        smokeControlId_=id;
        if(initializeOriginalWreckComponent50LA(smokeComponent_,OriginalWreckComponentKind50LA::effect15B860,
            b.domain->sessionGeneration,key,true,*b.actualObjectCount423F60).status!=OriginalWreckComponentStatus50LA::complete)return false;
        smokeConstructed_=true;smokeComponent_.nativeEffectControlId=&smokeControlId_;
        b.domainStorage[b.domain->count++]=&smokeComponent_;
        smoke_.construct15A5B0();smoke_.zeroVector(0x240);smoke_.zeroVector(0x250);
        smoke_.put(0x200,0);smoke_.put(0x204,0x3f800000);smoke_.put(0x208,0);
        smoke_.put(0x20c,0x40a00000);smoke_.put(0x210,0x3c23d70a);smoke_.put(0x214,0x43fa0000);
        smoke_.put(0x218,0x455ac000);smoke_.put(0x21c,0x3f000000);smoke_.put(0x220,0x3f800000);
        smoke_.put(0x224,0x40a00000);smoke_.put(0x228,0);smoke_.put(0x22c,0);
        smoke_.put(0x230,0x3f800000);smoke_.put(0x234,0x3c23d70a);smoke_.put(0x238,0);smoke_.put(0x23c,0x41c80000);
        smoke_.zeroVector(0x240);smoke_.zeroVector(0x250);
        smoke330_=&smokeComponent_;return true; //actual19BD54 or19CA04
    }
    bool appendSmoke()noexcept{
        auto& b=binding_;
        if(appendOriginalWreckComponent50LA(*b.domain,component_,smokeComponent_,0x402,
            *b.actualAttachmentLock423F58).status!=OriginalWreckComponentStatus50LA::complete)return false;
        smokeAppended_=true;return true;
    }
    bool loadFirst(OriginalGcParticleManager50KG& manager,const std::string& directory,std::uint32_t smokeId,std::uint32_t& raw)noexcept{
        if(!constructed_||retired_||pc_!=0x212c74u)return false;
        try{
            pc_=0x19bb70;parent_.manager70=&manager;parent_.managerKnown=true;parent_.filename78=directory+"podEmberFire.emt";
            const auto& f=fireDefinition_;
            parent_.number(0x224,f.velocityY224);parent_.vector(0x200,f.minRange200);parent_.vector(0x210,f.maxRange210);
            parent_.put(0x228,f.applySpread228?1u:0u);parent_.number(0x348,f.minDeath348);parent_.number(0x34c,f.maxDeath34C);
            pc_=0x19c69c;const float draw=drawOriginalGcParticleRandom50KG(raw);
            water_wake_detail50jc::Math m;
            const auto duration=m.add(f.minDeath348,m.mul(m.sub(f.maxDeath34C,f.minDeath348,0x19c6d0),draw,0x19c6d4),0x19c6d8);
            if(m.badPC){pc_=m.badPC;return false;}
            parent_.number(0x344,duration);parent_.put(0x340,0);parent_.number(0x220,f.emberFrames220);
            pc_=0x19c708;if(!applyCommon(parent_,f)||!registerNative(parent_,f,managerEmitter_))return false;
            smokeFilename22C_="podEmberSmoke.emt";
            if(!constructSmoke(smokeId))return false;
            pc_=0x15b9d0;smoke_.manager70=&manager;smoke_.managerKnown=true;smoke_.filename78=directory+smokeFilename22C_;
            if(!applyCommon(smoke_,smokeDefinition_))return false;
            const auto& s=smokeDefinition_;
            smoke_.number(0x21c,s.minimumPercentLife21C);smoke_.number(0x220,s.minimumPercentLifeConvergance220);
            smoke_.number(0x208,s.minimumInitialSize208);smoke_.number(0x20c,s.maximumInitialSize20C);
            smoke_.number(0x210,s.initialSizeToSpeedRatio210);smoke_.number(0x214,s.minimumLife214);smoke_.number(0x218,s.maximumLife218);
            smoke_.number(0x224,s.lifeToSpeedRatio224);smoke_.number(0x22c,s.minimumEmitRate22C);smoke_.number(0x230,s.maximumEmitRate230);
            smoke_.number(0x234,s.particlesToDistanceRatio234);smoke_.number(0x238,s.particlesToSpeedRatio238);
            smoke_.number(0x228,s.initialPercentAge228);smoke_.number(0x23c,s.minimumEmissionSpeed23C);
            smoke_.vector(0x240,s.minimumVelocityNoise240);smoke_.vector(0x250,s.maximumVelocityNoise250);
            if(!registerNative(smoke_,s,smokeManagerEmitter_))return false;
            pc_=0x19bd78;if(!appendSmoke())return false;
            pc_=0x19bd80;if(!allocateControllers())return false;
            pc_=0x19be48;if(!drawTail(raw))return false;
            pc_=0x212c94;return true;
        }catch(...){return false;}
    }
    bool loadClone(const OriginalRacerEmberOwner50LA& prototype,std::uint32_t smokeId,std::uint32_t& raw)noexcept{
        if(!constructed_||retired_||pc_!=0x212c74u||!prototype.complete_||prototype.retired_||
           !prototype.smokeConstructed_||prototype.smoke330_!=&prototype.smokeComponent_)return false;
        try{
            pc_=0x19c980;if(!parent_.clone15B630(prototype.parent_))return false;
            parent_.copyVector(0x200,prototype.parent_);parent_.copyVector(0x210,prototype.parent_);
            parent_.copy(0x220,prototype.parent_);parent_.copy(0x224,prototype.parent_);parent_.copy(0x228,prototype.parent_);
            smokeFilename22C_=prototype.smokeFilename22C_;
            if(!constructSmoke(smokeId))return false;
            pc_=0x15cd20;
            // Derived fields FIRST;15CDAC tail-jumps15B630, not vice versa.
            for(unsigned offset=0x200;offset<=0x23c;offset+=4)smoke_.copy(offset,prototype.smoke_);
            smoke_.copyVector(0x240,prototype.smoke_);smoke_.copyVector(0x250,prototype.smoke_);
            if(!smoke_.clone15B630(prototype.smoke_))return false;
            // Native manager value rows are per real owner, not source IDs.
            if(!registerNative(parent_,fireDefinition_,managerEmitter_,false)||
               !registerNative(smoke_,smokeDefinition_,smokeManagerEmitter_,false))return false;
            pc_=0x19ca40;if(!appendSmoke())return false;
            pc_=0x19ca48;if(!allocateControllers())return false;
            for(unsigned offset:{0x340u,0x344u,0x348u,0x34cu,0x350u})parent_.copy(offset,prototype.parent_);
            //350 is still UNKNOWN. Its source read copies unknown provenance.
            if(!drawTail(raw))return false;pc_=0x212d5c;return true;
        }catch(...){return false;}
    }
    bool appendStopDisable()noexcept{
        if(!constructed_||retired_||appended_||!smokeAppended_||(pc_!=0x212c94&&pc_!=0x212d5c))return false;
        auto& b=binding_;
        if(appendOriginalWreckComponent50LA(*b.domain,*b.particleParent,component_,0x402,
            *b.actualAttachmentLock423F58).status!=OriginalWreckComponentStatus50LA::complete)return false;
        appended_=true;pc_=0x19c900;parent_.stop15AD00();
        if(smoke330_!=&smokeComponent_||!smokeConstructed_)return false;
        smoke_.stop15AD00();pc_=0x141520;
        if(setOriginalWreckBaseActivity50LA(*b.domain,component_,false).status!=OriginalWreckComponentStatus50LA::complete)return false;
        complete_=true;pc_=0x212dc8;return true;
    }

    bool retireTail()noexcept{
        using namespace original_wreck_component_detail50LA;
        if(retired_)return true;
        if(!constructed_)return !component_.reference00&&component_.kind==OriginalWreckComponentKind50LA::unbound;
        auto& b=binding_;auto& c=component_;
        if(!b.domain||!b.domainStorage||b.domain->objects!=b.domainStorage||!domain(*b.domain)||
           b.domain->count>b.domainCapacity||!b.actualAttachmentLock423F58||*b.actualAttachmentLock423F58||
           !b.actualObjectCount423F60||*b.actualObjectCount423F60<(smokeConstructed_?2u:1u)||
           b.domain->count<(smokeConstructed_?2u:1u)||c.reference00!=1||c.kind!=OriginalWreckComponentKind50LA::effect19BA50||
           c.next14||b.domainStorage[b.domain->count-(smokeConstructed_?2u:1u)]!=&c||
           source334_!=controller334_.get()||source338_!=controller338_.get()||source33C_!=controller33C_.get()||
           !parent_.forced1C4NullKnown)return false;
        auto tail=[&](const OriginalWreckComponent50LA& node,const OriginalWreckComponent50LA* parent,bool appended){
            if(!appended)return !node.parent1C&&!node.previous10&&!node.next14;
            if(node.parent1C!=parent||!contains(*b.domain,parent)||node.next14)return false;
            const OriginalWreckComponent50LA* previous=nullptr;std::size_t left=b.domain->count;
            for(auto* current=parent->firstChild18;current;current=current->next14){
                if(!left--||!contains(*b.domain,current)||current->parent1C!=parent||current->previous10!=previous)return false;
                if(current==&node)return true;previous=current;
            }return false;
        };
        if(!tail(c,b.particleParent,appended_))return false;
        if(smokeConstructed_){
            const auto& smoke=smokeComponent_;
            if(smoke330_!=&smoke||smoke.reference00!=1||smoke.kind!=OriginalWreckComponentKind50LA::effect15B860||
               smoke.firstChild18||b.domainStorage[b.domain->count-1]!=&smoke||!smoke_.forced1C4NullKnown||
               !tail(smoke,&c,smokeAppended_)||c.firstChild18!=(smokeAppended_?&smokeComponent_:nullptr))return false;
        }else if(smoke330_||c.firstChild18||smokeComponent_.reference00||
            smokeComponent_.kind!=OriginalWreckComponentKind50LA::unbound)return false;
        // All aliases/topology preflighted before the first source mutation.
        // Source1424E0 truncates the sibling tail;142590 traverses genuine child15B940, then
        //19BAD0 frees334->338->33C and invokes15A720/1413E0.
        auto unlink=[](OriginalWreckComponent50LA& node){
            if(node.previous10)node.previous10->next14=nullptr;
            node.next14=nullptr;
            if(node.parent1C&&node.parent1C->firstChild18==&node)node.parent1C->firstChild18=nullptr;
        };
        if(c.previous10)c.previous10->next14=nullptr;c.next14=nullptr; //1424E0 caller prefix
        if(smokeConstructed_){
            unlink(smokeComponent_);--smokeComponent_.reference00;
            smokeComponent_.kind=OriginalWreckComponentKind50LA::entity141300;
            --*b.actualObjectCount423F60;--b.domain->count;smokeComponent_.nativeEffectControlId=nullptr;
        }
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=nullptr; //14264C, after children
        --c.reference00;controller334_.reset();controller338_.reset();controller33C_.reset();
        c.kind=OriginalWreckComponentKind50LA::entity141300;--*b.actualObjectCount423F60;
        --b.domain->count;c.nativeEffectControlId=nullptr;retired_=true;pc_=0x141418;
        // Pointer/field/source-slot history is retained. No numeric dispatch or
        // manager removal is implied; shared manager value rows outlive us.
        return true;
    }

public:
    OriginalRacerEmberOwner50LA()=default;
    OriginalRacerEmberOwner50LA(const OriginalRacerEmberOwner50LA&)=delete;
    OriginalRacerEmberOwner50LA& operator=(const OriginalRacerEmberOwner50LA&)=delete;
    ~OriginalRacerEmberOwner50LA(){if(constructed_&&!retired_)std::terminate();}
    const OriginalWreckComponent50LA& component()const noexcept{return component_;}
};
} // namespace rr::game
