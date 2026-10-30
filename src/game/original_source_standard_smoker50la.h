#pragma once
#include "original_application_entities50la.h"
#include "original_racer_model_scene50la.h"
#include <array>
#include <memory>
#include <optional>
#include <exception>
#include <limits>
#include <cmath>
#include <cfenv>
#include <cstring>
namespace ra_static_model_cold50LA {class Owner;}
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
// Conditional source-only owner. No issuer, simulation/emission backend,
// ParticleManager registration, or substitute model is constructed here.
class OriginalSourceStandardSmoker50LA final {
    friend class ::ra_static_model_cold50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using V=std::array<std::uint32_t,4>;
    enum class Status : std::uint8_t { reached,bounded,allocation,owner };
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;};
    template<std::size_t Bytes> struct RawFields {
        // Native zero storage does not certify source initialization. A bit
        // has source authority only when its corresponding known mask is set.
        std::array<std::uint32_t,Bytes/4> words{},known{};
        void word(std::uint32_t at,std::uint32_t value)noexcept {
            words[at/4]=value;known[at/4]=0xffffffffu;
        }
        void vector(std::uint32_t at,const V& value)noexcept {
            for(unsigned i=0;i<4;++i)word(at+4*i,value[i]);
        }
    };
    struct Smoker {
        C entity;
        RawFields<0x50> fields;
        std::uint32_t dispatch08=0;
        const OriginalRacerSceneObject50LA* model40=nullptr;
        bool model40Known=false,child44Known=false;
        C* child44=nullptr;
    };
    struct Emitter {
        C entity;
        RawFields<0x220> fields;
        std::uint32_t dispatch08=0;
        // Current bounded stage writes genuineNULL70/74 only; future live
        // manager storage requires an exact source World4A0 borrower receipt.
        const void* manager70=nullptr;
        bool manager70Known=false;
    };
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    std::uint64_t generation_=0;
    std::unique_ptr<Smoker> smoker_;
    std::unique_ptr<Emitter> emitter_;
    std::optional<C*>* wrapper0C_=nullptr;
    C* published0CIdentity_=nullptr;
    V descriptor150_{},descriptor160_{};
    std::uint32_t pc_=0;
    bool allocationAttempted_=false,smokerCounted_=false,smokerConstructorReturned_=false;
    bool wrapperPublished_=false,configureEntered_=false,descriptorVectorsCopied_=false;
    bool emitterAllocationAttempted_=false,emitterCounted_=false,emitterConstructorReturned_=false;
    bool childPublished_=false,baseConfigureReturned_=false,vector200Stored_=false;
    bool emitterRetired_=false,smokerRetired_=false,nativeWithdrawn_=false;
    struct Direction288F70 {
        bool entered=false,stackLengthStored=false,length218Stored=false,scaled17CStored=false,returned=false;
        std::array<std::optional<std::uint32_t>,7> intermediates;
        std::array<std::uint32_t,3> scalarArguments{};
        std::uint8_t scalarStoreMask=0;
    } direction_;
    static float nativeFloat(std::uint32_t bits)noexcept {float f;std::memcpy(&f,&bits,4);return f;}
    static std::uint32_t nativeWord(float f)noexcept {std::uint32_t bits;std::memcpy(&bits,&f,4);return bits;}
    Result continueDirectionScalars288F98(std::uint32_t first,std::uint32_t second,std::uint32_t third)noexcept {
        if(pc_!=0x288f98u||direction_.entered||!preflightNative()||!vector200Stored_||!emitter_||
           nativeWithdrawn_||emitterRetired_||smokerRetired_)return at(Status::owner,0x288f98);
        auto& h=direction_;h.entered=true;h.scalarArguments={{first,second,third}};
        //Same explicit finite/binary32/nearest native policy as pod_shape49W's
        //MUL.S,ADDA.S,MADD.S,SQRT.S sequence, not EE exception/bit parity.
        const auto keep=[&](float f,unsigned slot,std::uint32_t pc)noexcept {
            pc_=pc;if(!std::isfinite(f))return false;h.intermediates[slot]=nativeWord(f);return true;
        };
        if(std::fegetround()!=FE_TONEAREST)return at(Status::bounded,0x288f98);
        const float x=nativeFloat(descriptor160_[0]),y=nativeFloat(descriptor160_[1]),z=nativeFloat(descriptor160_[2]);
        if(!std::isfinite(x))return at(Status::bounded,0x288f98);
        volatile float xx=x*x;if(!keep(xx,0,0x288f98))return at(Status::bounded,pc_);
        if(!std::isfinite(y))return at(Status::bounded,0x288f9c);
        volatile float yy=y*y;if(!keep(yy,1,0x288f9c))return at(Status::bounded,pc_);
        volatile float xy=xx+yy;if(!keep(xy,2,0x288fa0))return at(Status::bounded,pc_);
        if(!std::isfinite(z))return at(Status::bounded,0x288fa4);
        volatile float zz=z*z;volatile float sum=xy+zz;
        if(!std::isfinite(zz)||!keep(sum,3,0x288fa4))return at(Status::bounded,0x288fa4);
        //Match the accepted pod_shape49w Math::root adapter: exponent-zero
        //SQRT operands flush to signed zero; normal finite roots round once
        //from double to binary32. This is not a general EE parity claim.
        const auto sumWord=nativeWord(sum);
        volatile float roundedRoot=(sumWord&0x7f800000u)==0?
            nativeFloat(sumWord&0x80000000u):
            static_cast<float>(std::sqrt(static_cast<double>(sum)));
        const float length=roundedRoot;
        if(!keep(length,4,0x288fa8))return at(Status::bounded,pc_);
        pc_=0x288fac;h.stackLengthStored=true;
        //DIV precedes emitter218 store. A bounded zero/overflow division must
        //not fabricate later length/rate fields or source returned0.
        if(length==0)return at(Status::bounded,0x288fc0);
        volatile float scale=20.0f/length;if(!keep(scale,5,0x288fc0))return at(Status::bounded,pc_);
        pc_=0x288fc4;emitter_->fields.word(0x218,nativeWord(length));h.length218Stored=true;
        pc_=0x288fc8;if(emitter_->fields.known[0x17c/4]!=0xffffffffu)return at(Status::bounded,pc_);
        const float rate=nativeFloat(emitter_->fields.words[0x17c/4]);
        if(!std::isfinite(rate))return at(Status::bounded,0x288fd0);
        volatile float scaled=rate*scale;if(!keep(scaled,6,0x288fd0))return at(Status::bounded,pc_);
        pc_=0x288fd8;emitter_->fields.word(0x17c,nativeWord(scaled));h.scaled17CStored=true;h.returned=true;
        pc_=0x288fe0;emitter_->fields.word(0x210,first);h.scalarStoreMask|=1;
        pc_=0x288fe4;emitter_->fields.word(0x18c,first);h.scalarStoreMask|=2;
        pc_=0x288fec;emitter_->fields.word(0x184,first);h.scalarStoreMask|=4;
        pc_=0x288ff4;emitter_->fields.word(0x214,second);h.scalarStoreMask|=8;
        pc_=0x289004;emitter_->fields.word(0x21c,third);h.scalarStoreMask|=16;
        return at(Status::reached,0x288da0); //Next genuine child141960, NOT model transform/activation/return.
    }
    bool directionHistoryCurrentNative()const noexcept {
        const auto& h=direction_;if(!h.entered){if(h.stackLengthStored||h.length218Stored||h.scaled17CStored||h.returned||h.scalarStoreMask)return false;
            for(const auto& w:h.intermediates)if(w)return false;return true;}
        if(!vector200Stored_||!emitter_||h.scalarStoreMask>31)return false;
        bool missing=false;for(const auto& w:h.intermediates){if(!w)missing=true;else if(missing||!std::isfinite(nativeFloat(*w)))return false;}
        if(h.stackLengthStored&&!h.intermediates[4])return false;
        if(h.length218Stored&&(!h.stackLengthStored||!h.intermediates[5]||
           emitter_->fields.known[0x218/4]!=0xffffffffu||emitter_->fields.words[0x218/4]!=*h.intermediates[4]))return false;
        if(h.scaled17CStored&&(!h.length218Stored||!h.intermediates[6]||
           emitter_->fields.known[0x17c/4]!=0xffffffffu||emitter_->fields.words[0x17c/4]!=*h.intermediates[6]))return false;
        if(h.returned!=h.scaled17CStored||(h.scalarStoreMask&&!h.returned))return false;
        constexpr std::uint32_t offsets[5]={0x210,0x18c,0x184,0x214,0x21c};
        for(unsigned i=0;i<5;++i)if(h.scalarStoreMask&(1u<<i)){
            if(emitter_->fields.known[offsets[i]/4]!=0xffffffffu||
               emitter_->fields.words[offsets[i]/4]!=h.scalarArguments[i<3?0:i-2])return false;
        }
        return true;
    }
    OriginalRacerModelSceneOwner50LA* modelOwner_=nullptr;
    bool modelBorrowed_=false,childAttachEntered_=false,childAttachReturned_=false,childAttached_=false,childDetachedNative_=false;
    std::optional<std::uint32_t> childAttachLock_;
    OriginalRacerModelSceneOwner50LA::SourcePoint28BAB0 point_;
    V worldPosition_{};
    bool pointEntered_=false,pointReturned_=false,emitterPositionStored_=false,activated_=false,configureReturned_=false;
    template<class ActualAttach>
    Result attachEmitter141960(ActualAttach&& attach)noexcept {
        if(pc_!=0x288da0u||childAttachEntered_||!preflightNative()||
           !direction_.returned||direction_.scalarStoreMask!=31||!smoker_||!emitter_||
           !smokerCounted_||!emitterCounted_)return at(Status::owner,0x288da0);
        childAttachEntered_=true;pc_=0x141970;childAttachLock_=application_->attachmentLock423F58();
        if(!*childAttachLock_){
            //Only actual friended root supplies same-App authority closure;
            //both counted nodes have been enrolled before this source call.
            const auto result=attach(smoker_->entity,emitter_->entity,0xffffffffu,*childAttachLock_);
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return at(Status::bounded,result.sourcePC);
            childAttached_=true;
        } //Actual lockedNULL return ignored, NOT a failed configuration.
        childAttachReturned_=true;return at(Status::reached,0x288da8);
    }
    Result continueModelPoint288DB0(const SceneGraph49P& actualGraph)noexcept {
        if(pc_!=0x288da8u||pointEntered_||!childAttachReturned_||!preflightNative()||
           !smoker_||!emitter_||!smoker_->model40Known)return at(Status::owner,0x288db0);
        pointEntered_=true;
        if(!smoker_->model40)return at(Status::bounded,0x28bad8); //Actual NULL model190 read.
        if(!modelOwner_||smoker_->model40!=&modelOwner_->model_||
           !modelBorrowed_||modelOwner_->sourceSmokerBorrower_!=this)return at(Status::owner,0x28bab0);
        V local{};for(unsigned i=0;i<4;++i){if(smoker_->fields.known[(0x30+4*i)/4]!=0xffffffffu)return at(Status::bounded,0x28bb74);
            local[i]=smoker_->fields.words[(0x30+4*i)/4];}
        const auto result=modelOwner_->transformSourcePoint28BAB0(actualGraph,local,worldPosition_,point_);
        if(!result.okay())return at(Status::bounded,result.sourcePC);pointReturned_=true;
        pc_=0x288dc0;emitter_->fields.vector(0x30,worldPosition_);emitterPositionStored_=true;
        //Actual virtual80=15ACE0 sets only bit1 and clears19C; high unknown
        //bits in source1D4 remain unknown, no emitter/runtime registration.
        pc_=0x15acf4;emitter_->fields.words[0x1d4/4]|=2u;emitter_->fields.known[0x1d4/4]|=2u;
        pc_=0x15acfc;emitter_->fields.word(0x19c,0);activated_=true;
        pc_=0x288dd8; //141C20 no-op: no reference acquisition.
        configureReturned_=true;configurationReturnV0_=&smoker_->entity;return at(Status::reached,0x288df8); //Actual returned self, never synthetic0.
    }
    C* configurationReturnV0_=nullptr;
    bool outerAttachEntered_=false,outerAttachReturned_=false,outerAttached_=false,outerDetachedNative_=false;
    std::optional<std::uint32_t> outerAttachLock_;
    C* outerParent_=nullptr;
    bool managerBound_=false;
    const void* managerIdentity_=nullptr;
    template<class ActualAttach>
    Result attachModel141960(ActualAttach&& attach)noexcept {
        if(outerAttachEntered_||!configureReturned_||!configurationReturnV0_||!preflightNative()||
           !modelOwner_||!smoker_||configurationReturnV0_!=&smoker_->entity||
           modelOwner_->model_.entity8.firstChild18)return at(Status::owner,0x27af58);
        outerParent_=&modelOwner_->model_.entity8;outerAttachEntered_=true;
        outerAttachLock_=application_->attachmentLock423F58();
        if(!*outerAttachLock_){
            const auto r=attach(*outerParent_,smoker_->entity,0xffffffffu,*outerAttachLock_);
            if(r.status!=OriginalWreckComponentStatus50LA::complete)return at(Status::bounded,r.sourcePC);
            outerAttached_=true;
        } //Original ignored lockedNULL return; manager binding still follows.
        outerAttachReturned_=true;return at(Status::reached,0x27af60);
    }
    Result bindManager288E60(const void* actualManager4A0)noexcept {
        if(!outerAttachReturned_||managerBound_||!actualManager4A0||!preflightNative()||
           !emitter_||!childPublished_||smoker_->child44!=&emitter_->entity)return at(Status::owner,0x288e68);
        //Actual virtual78=15ACA0 is ONLY a borrowed pointer70 store. Raw guest
        //address bits are unknown; neither a registration nor source retain.
        emitter_->manager70=actualManager4A0;emitter_->manager70Known=true;
        emitter_->fields.known[0x70/4]=0;managerIdentity_=actualManager4A0;managerBound_=true;
        return at(Status::reached,0x288e88); //Original288E80 produces1; caller ignores it.
    }
    static std::uint64_t key(const C* c)noexcept {
        // Native ledger identity only; never a guest pointer/address field.
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(c));
    }
    Result at(Status status,std::uint32_t pc)noexcept {pc_=pc;return {status,pc};}
    bool countEntity(C& entity)noexcept {
        if(application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
        return initializeOriginalWreckComponent50LA(entity,K::entity141300,generation_,key(&entity),true,
            application_->objectCount423F60()).status==OriginalWreckComponentStatus50LA::complete;
    }
    Result allocateSmoker27AE68(std::shared_ptr<OriginalApplicationEntities50LA> app,
        std::uint64_t generation,std::optional<C*>& actualWrapper0C)noexcept {
        if(allocationAttempted_||nativeWithdrawn_||!app||!generation||
           !app->application10()||!app->application26C())return at(Status::owner,0x27ae68);
        // Root has authenticated true descriptor14C and nonNULL source4A0,
        // reserved native ledger capacity, and qualified this exact wrapper.
        // Its previously unknown0C is not read or synthesized asNULL here.
        application_=std::move(app);generation_=generation;wrapper0C_=&actualWrapper0C;
        allocationAttempted_=true;
        try{smoker_=std::make_unique<Smoker>();}catch(...){smoker_.reset();}
        if(smoker_){
            if(!countEntity(smoker_->entity))return at(Status::bounded,0x141300);
            smokerCounted_=true;
            smoker_->dispatch08=0x3d3110; //288C4C after genuine base Entity.
            smoker_->fields.vector(0x30,V{0,0,0,0x3f800000u}); //sqc2 vf00
            smoker_->model40=nullptr;smoker_->model40Known=true;
            smoker_->child44=nullptr;smoker_->child44Known=true;
            smoker_->entity.kind=K::smoker288C30;smokerConstructorReturned_=true;
        }
        published0CIdentity_=smoker_?&smoker_->entity:nullptr;
        actualWrapper0C=published0CIdentity_;wrapperPublished_=true;
        return at(Status::reached,0x27ae84); //Nullable pointer publication, not288CE0 return.
    }
    Result configurePrefix288CE0(OriginalRacerModelSceneOwner50LA* actualNullableModelOwner,
        const V& actualDescriptor150,const V& actualDescriptor160)noexcept {
        if(!allocationAttempted_||!wrapperPublished_||configureEntered_||nativeWithdrawn_||
           !wrapper0C_||!*wrapper0C_||**wrapper0C_!=published0CIdentity_||
           (smoker_&&!smokerConstructorReturned_))return at(Status::owner,0x288ce0);
        if(actualNullableModelOwner&&(actualNullableModelOwner->sourceSmokerBorrower_||
           !actualNullableModelOwner->preflightSourceRenderableReturn26E820Native()||
           actualNullableModelOwner->binding_.actualObjectCount423F60!=&application_->objectCount423F60()||
           actualNullableModelOwner->binding_.sessionGeneration!=generation_))return at(Status::owner,0x288ce0);
        configureEntered_=true;
        // Original copies both full vectors before first smoker dereference.
        descriptor150_=actualDescriptor150;descriptor160_=actualDescriptor160;descriptorVectorsCopied_=true;
        if(!smoker_)return at(Status::bounded,0x288d20);
        modelOwner_=actualNullableModelOwner;
        smoker_->model40=modelOwner_?&modelOwner_->model_:nullptr;smoker_->model40Known=true;
        if(modelOwner_){modelOwner_->sourceSmokerBorrower_=this;modelBorrowed_=true;}
        smoker_->fields.vector(0x30,descriptor150_); //288D28
        emitterAllocationAttempted_=true;
        try{emitter_=std::make_unique<Emitter>();}catch(...){emitter_.reset();}
        if(emitter_){
            if(!countEntity(emitter_->entity))return at(Status::bounded,0x141300);
            emitterCounted_=true;
            auto& f=emitter_->fields;emitter_->dispatch08=0x3cdd90;
            const V vf00{0,0,0,0x3f800000u};
            f.vector(0x30,vf00);f.vector(0x40,vf00);f.vector(0x50,vf00);f.vector(0x60,vf00);
            //15A5B0 changes only low3 bits of source byte1D4.
            f.words[0x1d4/4]=1;f.known[0x1d4/4]=7;
            f.word(0x70,0);f.word(0x74,0);emitter_->manager70=nullptr;emitter_->manager70Known=true;
            f.word(0x17c,0x457a0000);f.word(0x180,0x40c00000);
            f.word(0x184,0x3f800000);f.word(0x188,0x41200000);f.word(0x18c,0);
            f.word(0x1cc,0);f.word(0x1d0,0);f.word(0x190,0);f.word(0x194,0x00ffffff);
            f.word(0x198,0);f.word(0x19c,0);f.word(0x1a0,0);f.word(0x1b0,0);f.word(0x1b4,0);
            f.word(0x1b8,0xffffffff);for(std::uint32_t offset=0x1d8;offset<=0x1f4;offset+=4)f.word(offset,0);
            f.word(0x1c4,0);
            emitter_->dispatch08=0x3d3070; //288EAC
            f.vector(0x200,vf00);f.word(0x17c,0x457a0000);f.word(0x198,0x2e);
            f.word(0x194,0x00323232);f.word(0x204,0x42480000);f.word(0x200,0x41f00000);
            f.word(0x208,0);f.word(0x1b4,0x0d);f.word(0x1b0,0);f.word(0x190,0);
            emitter_->entity.kind=K::smokerEmitter288E90;emitterConstructorReturned_=true;
        }
        smoker_->child44=emitter_?&emitter_->entity:nullptr;childPublished_=true;
        if(!emitter_)return at(Status::bounded,0x288d58); //NULL virtual74 dereference.
        //Actual virtual74=15AC10(0,0,0).141C20 is no-op, not retain.
        emitter_->manager70=nullptr;emitter_->manager70Known=true;emitter_->fields.word(0x70,0);
        baseConfigureReturned_=true;
        emitter_->fields.vector(0x200,descriptor160_);vector200Stored_=true; //288F88
        //The original vector length/division begins288F98. No supplied length,
        //prepared transform or fake zero-return can skip that real dependency.
        //Thus neither141960 nor manager15ACA0 has run in this bounded facet.
        return at(Status::bounded,0x288f98);
    }
    C* sourceCountedSmokerNative()noexcept {
        return smokerCounted_&&!smokerRetired_&&smoker_?&smoker_->entity:nullptr;
    }
    C* sourceCountedEmitterNative()noexcept {
        return emitterCounted_&&!emitterRetired_&&emitter_?&emitter_->entity:nullptr;
    }
    bool preflightNative()const noexcept {
        if(!allocationAttempted_)return !smoker_&&!emitter_;
        if(nativeWithdrawn_)return !smoker_&&!emitter_;
        if(!application_||!generation_||!wrapper0C_||!directionHistoryCurrentNative())return false;
        if(modelBorrowed_&&(!modelOwner_||!smoker_||smoker_->model40!=&modelOwner_->model_||
           modelOwner_->sourceSmokerBorrower_!=this||!modelOwner_->preflightSourceRenderableReturn26E820Native()))return false;
        if(modelOwner_&&!modelBorrowed_)return false;
        if(childAttachEntered_&&(!direction_.returned||direction_.scalarStoreMask!=31||!childAttachLock_))return false;
        if(childAttachReturned_&&(!childAttachEntered_||(*childAttachLock_?childAttached_:!childAttached_)))return false;
        if(childDetachedNative_&&!childAttached_)return false;
        if(pointEntered_&&!childAttachReturned_)return false;
        if(point_.entered&&(!modelOwner_||!modelOwner_->sourcePointHistoryCurrentNative(point_)))return false;
        if(pointReturned_&&(!pointEntered_||!point_.returned))return false;
        if(emitterPositionStored_){if(!pointReturned_||!emitter_)return false;
            for(unsigned i=0;i<4;++i)if(emitter_->fields.known[(0x30+4*i)/4]!=0xffffffffu||
                emitter_->fields.words[(0x30+4*i)/4]!=worldPosition_[i])return false;}
        if(activated_&&(!emitterPositionStored_||!emitter_||
           (emitter_->fields.known[0x1d4/4]&2u)!=2u||(emitter_->fields.words[0x1d4/4]&2u)!=2u||
           emitter_->fields.known[0x19c/4]!=0xffffffffu||emitter_->fields.words[0x19c/4]))return false;
        if(configureReturned_&&(!activated_||!smoker_||configurationReturnV0_!=&smoker_->entity))return false;
        if(outerAttachEntered_&&(!configureReturned_||!modelOwner_||outerParent_!=&modelOwner_->model_.entity8||!outerAttachLock_))return false;
        if(outerAttachReturned_&&(!outerAttachEntered_||(*outerAttachLock_?outerAttached_:!outerAttached_)))return false;
        if(outerDetachedNative_&&!outerAttached_)return false;
        if(managerBound_&&(!outerAttachReturned_||!managerIdentity_||!emitter_||
           !emitter_->manager70Known||emitter_->manager70!=managerIdentity_||emitter_->fields.known[0x70/4]))return false;
        if(wrapperPublished_&&(!*wrapper0C_||**wrapper0C_!=published0CIdentity_))return false;
        auto entity=[&](const C& e,K kind)->bool {
            return e.reference00==1&&e.sessionGeneration==generation_&&e.objectKey==key(&e)&&
                e.kind==kind&&e.category28==0xff&&(e.flags29&15u)==3u&&!e.nativeFlags29;
        };
        if(smoker_){const auto& e=smoker_->entity;
            if(e.parent1C!=((outerAttached_&&!outerDetachedNative_)?outerParent_:nullptr)||e.previous10||e.next14||
               e.mask20!=(outerAttached_?0xffffffffu:0u)||
               e.firstChild18!=((childAttached_&&!childDetachedNative_&&emitter_)?&emitter_->entity:nullptr)||
               e.descendants24!=((childAttached_&&!childDetachedNative_)?0xffffffffu:0u))return false;}
        if(emitter_){const auto& e=emitter_->entity;
            if(e.parent1C!=((childAttached_&&!childDetachedNative_&&smoker_)?&smoker_->entity:nullptr)||
               e.previous10||e.next14||e.firstChild18||e.descendants24||e.mask20!=(childAttached_?0xffffffffu:0u))return false;}
        if(smokerCounted_&&!smokerRetired_&&(!smoker_||!entity(smoker_->entity,
           smokerConstructorReturned_?K::smoker288C30:K::entity141300)))return false;
        if(emitterCounted_&&!emitterRetired_&&(!emitter_||!entity(emitter_->entity,
           emitterConstructorReturned_?K::smokerEmitter288E90:K::entity141300)))return false;
        const std::uint32_t counted=(smokerCounted_&&!smokerRetired_?1u:0u)+(emitterCounted_&&!emitterRetired_?1u:0u);
        if(application_->objectCount423F60()<counted)return false;
        if(configureEntered_&&!descriptorVectorsCopied_)return false;
        if(childPublished_&&(!smoker_||smoker_->child44!=(emitter_?&emitter_->entity:nullptr)))return false;
        if(vector200Stored_&&(!emitter_||!baseConfigureReturned_||
           emitter_->fields.words[0x200/4]!=descriptor160_[0]||
           emitter_->fields.words[0x204/4]!=descriptor160_[1]||
           emitter_->fields.words[0x208/4]!=descriptor160_[2]||
           emitter_->fields.words[0x20c/4]!=descriptor160_[3]))return false;
        return true;
    }
    // Only the friended actual root can supply its exact ledger retirement
    // closure. Native cancellation is not original142590/destructor dispatch.
    // Each counted node must leave that ledger before reference/count release.
    template<class DetachFromActualAuthority,class RetireFromActualLedger>
    bool withdrawNative(DetachFromActualAuthority&& detach,RetireFromActualLedger&& retire)noexcept {
        if(!allocationAttempted_||nativeWithdrawn_)return true;
        if(!preflightNative())return false;
        if(outerAttached_&&!outerDetachedNative_){
            const auto lock=application_->attachmentLock423F58();if(lock)return false;
            const auto result=detach(smoker_->entity,lock);
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;outerDetachedNative_=true;
        }
        if(childAttached_&&!childDetachedNative_){
            const auto lock=application_->attachmentLock423F58();if(lock)return false;
            const auto result=detach(emitter_->entity,lock);
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;childDetachedNative_=true;
        }
        if(emitterCounted_&&!emitterRetired_){
            if(!application_->objectCount423F60()||!retire(emitter_->entity))return false;
            emitter_->entity.reference00=0;--application_->objectCount423F60();emitterRetired_=true;
        }
        if(smokerCounted_&&!smokerRetired_){
            if(!application_->objectCount423F60()||!retire(smoker_->entity))return false;
            smoker_->entity.reference00=0;--application_->objectCount423F60();smokerRetired_=true;
        }
        //Withdraw native alias before destroying storage. History remains in
        //published0CIdentity_/childPublished_ and is not dereferenced thereafter.
        if(wrapperPublished_)*wrapper0C_=nullptr;
        if(modelBorrowed_){if(!modelOwner_||modelOwner_->sourceSmokerBorrower_!=this)return false;
            modelOwner_->sourceSmokerBorrower_=nullptr;modelBorrowed_=false;modelOwner_=nullptr;}
        if(smoker_){smoker_->model40=nullptr;smoker_->child44=nullptr;}
        emitter_.reset();smoker_.reset();nativeWithdrawn_=true;return true;
    }
public:
    OriginalSourceStandardSmoker50LA()=default;
    OriginalSourceStandardSmoker50LA(const OriginalSourceStandardSmoker50LA&)=delete;
    OriginalSourceStandardSmoker50LA& operator=(const OriginalSourceStandardSmoker50LA&)=delete;
    ~OriginalSourceStandardSmoker50LA(){if(allocationAttempted_&&!nativeWithdrawn_)std::terminate();}
};
} //namespace rr::game
