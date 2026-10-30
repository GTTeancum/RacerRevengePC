#pragma once
#include "original_source_physics_body50la.h"
#include "original_source_physics_collision50la.h"
#include "../assets/original_source_contact_point_math50la.h"
#include "original_wreck_tuning50la.h"
#include <cfenv>
#include <vector>
#include <array>
#include <memory>
#include <new>
#include <cstdint>
#include <cstring>
#include <limits>
#include <exception>
namespace rr::game {
//Conditional165E90 contactDC, not later detectorD8. Root owns predecessor,
//ledger enrollment and source callsite. No point-selection or runtime issuer.
class OriginalSourcePhysicsContact50LA final {
    friend class OriginalSourcePhysicsDetector50LA;
    friend class OriginalSourcePhysicsObject50LA;
    friend class ::ra_static_model_cold50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using Base=OriginalSourcePhysicsObject50LA;
    using Body=OriginalSourcePhysicsBody50LA;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    enum class Status:std::uint8_t {reached,bounded,owner};
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;};
    struct Storage {
        C entity;
        OriginalWreckPhysical50LA physical;
        std::unique_ptr<std::uint8_t[]> raw140;
        std::array<std::uint32_t,0x140/4> words{},known{},constructorWords{},constructorKnown{};
        std::optional<const void*> source80,source8C,body30;
        std::uint32_t dispatch08=0;
        std::uint8_t flags29Known=0;
        void word(unsigned off,std::uint32_t value)noexcept {
            words[off/4]=value;known[off/4]=0xffffffffu;
            std::memcpy(raw140.get()+off,&value,4);
        }
        void vector(unsigned off,const std::array<std::uint32_t,4>& v)noexcept {
            for(unsigned i=0;i<4;++i)word(off+4*i,v[i]);
        }
    };
    const void* detectorBorrower_=nullptr;
    const void* detector48Occurrence_=nullptr; //Identity history only; never dereference after withdrawal.
    bool detectorCleared48_=false,detector48NativeWithdrawn_=false;
    bool detector48HistoryNative()const noexcept {
        if(!detectorCleared48_)return !detector48Occurrence_&&!detector48NativeWithdrawn_;
        if(!detector48Occurrence_||!suffixReturned_||!attachReturned_||
           (detector48NativeWithdrawn_?bool(detectorBorrower_):detectorBorrower_!=detector48Occurrence_))return false;
        if(nativeWithdrawn_)return detector48NativeWithdrawn_;
        return storage_&&storage_->known[0x48/4]==0xffffffffu&&storage_->words[0x48/4]==0&&
            !storage_->physical.generated.pointContacts48;
    }
    Base* base_=nullptr;
    Body* bodyOwner_=nullptr;
    std::unique_ptr<Storage> storage_;
    std::unique_ptr<std::uint8_t[]> points80_,generated8C_;
    std::array<std::uint8_t,4800> points80Known_{},generated8CKnown_{};
    const std::array<std::uint32_t,4>* origin4435A0_=nullptr;
    C* publishedIdentity_=nullptr;
    bool attempted_=false,controlAllocated_=false,rawAllocationAttempted_=false;
    bool counted_=false,constructorReturned_=false,published_=false;
    bool capacityEntered_=false,capacityStored_=false,firstAllocation_=false,secondAllocation_=false;
    bool firstPublished_=false,secondPublished_=false,identityReturned_=false,capacityReturned_=false;
    bool bodyStored_=false,nativeRetired_=false,nativeWithdrawn_=false;
    std::uint32_t pc_=0;
    Result at(Status s,std::uint32_t pc)noexcept {pc_=pc;return {s,pc};}
    void stores295A40()noexcept {
        auto& s=*storage_;const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        s.dispatch08=0x3d35f0u;
        s.vector(0x100,vf0);s.vector(0x110,vf0);s.vector(0x120,vf0);
        s.word(0x7c,0);s.word(0x84,0);s.word(0x88,0);s.body30=nullptr;s.word(0x30,0);
        s.source80=nullptr;s.word(0x80,0);s.source8C=nullptr;s.word(0x8c,0);
        for(auto off:{0x130u,0x34u,0x3cu,0x38u,0x70u})s.word(off,0);
        for(auto off:{0x48u,0x4cu,0x54u,0x50u})s.word(off,1);
        for(auto off:{0x5cu,0x58u,0x60u,0x64u,0x68u,0x6cu,0x74u,0x78u,0x40u,0x44u,0x90u})s.word(off,0);
        s.word(0x94,0x461c4000u);
        for(auto off:{0x9cu,0xa0u,0xa4u,0xa8u})s.word(off,0);
        for(auto off:{0xacu,0xb0u,0xb4u,0xc4u})s.word(off,0x3f800000u);
        s.word(0xcc,0x3d4ccccdu);s.word(0xd0,0);s.word(0x98,0x3f800000u);
        s.vector(0x100,*origin4435A0_);s.vector(0x120,*origin4435A0_);
        for(unsigned i=0;i<10;++i)s.word(0xd8+4*i,0);
        for(auto off:{0xd4u,0xb8u,0xc0u,0xbcu})s.word(off,0);
        const SpringRecord48N* a=nullptr;const SpringRecord48N* b=nullptr;
        initializeFreshWreckContact295A40_50LA(s.physical,*origin4435A0_,a,b);
        s.constructorWords=s.words;s.constructorKnown=s.known;
        //Numeric mirror only. Raw buffer custody/knownness below does not
        //admit semantic record containers or point payload readiness.
    }
    Result allocateSource1660A0(Base& base,Body& body,OriginalSourcePhysicsCollision50LA& col)noexcept {
        if(attempted_||nativeWithdrawn_||!base.preflightNative()||!base.object_||
           base.contactBorrower_||base.object_->contactDC||body.base_!=&base||
           col.base_!=&base||!col.attachReturned_||col.detachedNative_||!col.transformReturned_||
           !col.preflightNative()||
           !body.preflightNative()||!body.body_||!body.disableReturned_||
           !body.countedEntityNative()||base.application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
            return {Status::owner,0x1660a0u};
        //Exact conditional predecessor proof; no injected successful-call boolean.
        base_=&base;attempted_=true;base.contactBorrower_=this;
        origin4435A0_=&base.application_->springZero4435A0_;
        try{storage_=std::make_unique<Storage>();}catch(...){return at(Status::bounded,0x1660a0u);}
        controlAllocated_=true;rawAllocationAttempted_=true;
        storage_->raw140.reset(new(std::nothrow) std::uint8_t[0x140]);
        if(storage_->raw140){
            const auto made=initializeOriginalWreckComponent50LA(storage_->entity,K::entity141300,
                base.generation_,Base::key(&storage_->entity),true,base.application_->objectCount423F60());
            if(made.status!=OriginalWreckComponentStatus50LA::complete)return at(Status::bounded,made.sourcePC);
            counted_=true;storage_->flags29Known=0x0f;storage_->entity.kind=K::contact295A40;
            stores295A40();constructorReturned_=true;
        }
        publishedIdentity_=constructorReturned_?&storage_->entity:nullptr;
        base.object_->contactDC=publishedIdentity_;published_=true;
        return at(Status::reached,0x1660c0u);
    }
    Result configureCapacitySource295C20()noexcept {
        if(capacityEntered_||!published_||pc_!=0x1660c0u||!preflightNative())return {Status::owner,0x1660c4u};
        capacityEntered_=true;
        if(!constructorReturned_)return at(Status::bounded,0x295c2cu);
        storage_->word(0x88,50);storage_->physical.spring.generatedCapacity88=50;capacityStored_=true;
        firstAllocation_=true;points80_.reset(new(std::nothrow) std::uint8_t[4800]);
        storage_->source80=static_cast<const void*>(points80_.get());
        if(points80_)storage_->known[0x80/4]=0;else storage_->word(0x80,0);firstPublished_=true;
        secondAllocation_=true;generated8C_.reset(new(std::nothrow) std::uint8_t[4800]);
        storage_->source8C=static_cast<const void*>(generated8C_.get());
        if(generated8C_)storage_->known[0x8c/4]=0;else storage_->word(0x8c,0);secondPublished_=true;
        //Both independently nullable raw buffers have NO payload initialization.
        //141C20 is actual identity return, not a retain or successful allocation.
        identityReturned_=true;capacityReturned_=true;
        return at(Status::reached,0x1660ccu);
    }
    Result bindBodySource295C80(Body& body)noexcept {
        if(bodyStored_||!capacityReturned_||pc_!=0x1660ccu||!preflightNative()||
           body.base_!=base_||!body.preflightNative()||!body.countedEntityNative()||
           !body.body_||body.contactBodyBorrower_||!base_->object_->bodyD0||
           *base_->object_->bodyD0!=&body.body_->entity)return {Status::owner,0x1660d0u};
        bodyOwner_=&body;body.contactBodyBorrower_=this;
        storage_->body30=static_cast<const void*>(&body.body_->entity);storage_->known[0x30/4]=0;
        storage_->physical.contactBody30=&body.body_->physical.body;bodyStored_=true;
        return at(Status::reached,0x1660d8u); //Before296190; no point selection.
    }
#include "original_source_physics_contact_points50la.inl"
#include "original_source_physics_contact_suffix50la.inl"
#include "original_source_physics_contact_tuning50la.inl"
    C* countedEntityNative()const noexcept {
        return counted_&&!nativeRetired_&&storage_?&storage_->entity:nullptr;
    }
    bool preflightNative()const noexcept {
        if(!detector48HistoryNative())return false;
        if(!attempted_)return !detectorBorrower_&&!base_&&!storage_&&!counted_&&!published_&&!nativeWithdrawn_;
        if(nativeWithdrawn_)return !detectorBorrower_&&!base_&&!storage_&&!bodyOwner_&&!points80_&&!generated8C_&&(!counted_||nativeRetired_);
        if(!base_||!base_->object_||base_->contactBorrower_!=this||!base_->preflightNative()||
           origin4435A0_!=&base_->application_->springZero4435A0_)return false;
        if(!storage_)return !controlAllocated_&&!rawAllocationAttempted_&&!counted_&&!published_&&pc_==0x1660a0u;
        if(!controlAllocated_||!rawAllocationAttempted_)return false;
        if(published_){if(!base_->object_->contactDC||*base_->object_->contactDC!=publishedIdentity_)return false;}
        else if(base_->object_->contactDC)return false;
        if(!storage_->raw140)return !counted_&&!constructorReturned_&&published_&&!publishedIdentity_&&
            !capacityStored_&&!firstAllocation_&&!secondAllocation_&&!bodyStored_&&
            pc_==(capacityEntered_?0x295c2cu:0x1660c0u);
        const auto& e=storage_->entity;
        if(!counted_)return !constructorReturned_&&!published_&&!e.reference00&&e.kind==K::unbound;
        if(nativeRetired_||!constructorReturned_||!published_||publishedIdentity_!=&e||e.reference00!=1||
           e.kind!=K::contact295A40||e.sessionGeneration!=base_->generation_||e.objectKey!=Base::key(&e)||
           e.firstChild18||e.mask20||e.descendants24||e.nativeFlags29||
           e.category28!=0xffu||(e.flags29&15u)!=3u||storage_->flags29Known!=15u||
           storage_->dispatch08!=0x3d35f0u||!base_->application_->objectCount423F60())return false;
        if(!sourceTuningHistoryNative()||!sourceScalarSuffixHistoryNative()||!attachmentHistoryNative())return false;
        if(!attachEntered_&&(e.parent1C||e.previous10||e.next14))return false;
        for(unsigned i=0;i<storage_->known.size();++i){
            const bool pointer=i==0x30/4||i==0x80/4||i==0x8c/4;
            bool suffixWord=false;
            if(suffixReturned_)for(auto off:suffixOffsets_)if(i==off/4){suffixWord=true;break;}
            if(!pointer&&!suffixWord&&!sourceTuningWrittenNative(i*4)&&!(detectorCleared48_&&i==0x48/4)&&i!=0x88/4&&i!=0x7c/4&&(storage_->known[i]!=storage_->constructorKnown[i]||
               storage_->words[i]!=storage_->constructorWords[i]))return false;
        }
        if(storage_->known[0x88/4]!=0xffffffffu||
           storage_->known[0x30/4]!=(bodyStored_?0u:0xffffffffu)||
           storage_->known[0x80/4]!=(points80_?0u:0xffffffffu)||
           storage_->known[0x8c/4]!=(generated8C_?0u:0xffffffffu))return false;
        for(unsigned i=0;i<storage_->known.size();++i)if(storage_->known[i]){
            std::uint32_t raw;std::memcpy(&raw,storage_->raw140.get()+i*4,4);
            if(storage_->known[i]!=0xffffffffu||raw!=storage_->words[i])return false;
        }
        if(capacityEntered_!=capacityStored_||capacityStored_!=firstAllocation_||
           firstAllocation_!=firstPublished_||firstPublished_!=secondAllocation_||
           secondAllocation_!=secondPublished_||secondPublished_!=identityReturned_||
           identityReturned_!=capacityReturned_)return false;
        if(!storage_->physical.contactConstructorFieldsApplied||
           storage_->physical.spring.pointCount7C!=sourcePoints_.recordCount||storage_->physical.spring.generatedCount84||
           storage_->physical.spring.generatedCapacity88!=(capacityStored_?50u:0u)||
           (!bodyStored_&&storage_->physical.contactBody30)||
           storage_->words[0x88/4]!=(capacityStored_?50u:0u)||
           storage_->source80!=std::optional<const void*>(points80_.get())||
           storage_->source8C!=std::optional<const void*>(generated8C_.get()))return false;
        if(!preflightSourcePointsNative())return false;
        for(auto k:generated8CKnown_)if(k)return false;
        if(bodyStored_){
            if(!capacityReturned_||!bodyOwner_||bodyOwner_->base_!=base_||bodyOwner_->contactBodyBorrower_!=this||
               !bodyOwner_->preflightNative()||!bodyOwner_->body_||
               storage_->body30!=std::optional<const void*>(&bodyOwner_->body_->entity)||
               storage_->physical.contactBody30!=&bodyOwner_->body_->physical.body||(!sourcePoints_.attempted&&pc_!=0x1660d8u))return false;
        }else if(bodyOwner_||storage_->body30!=std::optional<const void*>(nullptr)||
                 pc_!=(capacityReturned_?0x1660ccu:0x1660c0u))return false;
        return true;
    }
    template<class RetireLedger> bool withdrawNative(RetireLedger&& retire)noexcept {
        if(!attempted_||nativeWithdrawn_)return true;
        if(detectorBorrower_||(attached_&&!detachedNative_)||!preflightNative()||!withdrawSourcePointsNative())return false;
        //Point-resource unpin is explicit native history before ledger callback.
        //A later refusal retains all Contact buffers/count and Body/Model custody.
        if(counted_&&!nativeRetired_&&(!base_->application_->objectCount423F60()||!retire(storage_->entity)))return false;
        if(bodyOwner_){bodyOwner_->contactBodyBorrower_=nullptr;bodyOwner_=nullptr;}
        points80_.reset();generated8C_.reset();
        if(counted_&&!nativeRetired_){storage_->entity.reference00=0;--base_->application_->objectCount423F60();nativeRetired_=true;}
        if(published_){base_->object_->contactDC=nullptr;base_->object_->contactDCNativeWithdrawn=true;}
        base_->contactBorrower_=nullptr;storage_.reset();base_=nullptr;origin4435A0_=nullptr;nativeWithdrawn_=true;
        return true; //Native compensation; never a295B90 source-call receipt.
    }
public:
    OriginalSourcePhysicsContact50LA()=default;
    OriginalSourcePhysicsContact50LA(const OriginalSourcePhysicsContact50LA&)=delete;
    OriginalSourcePhysicsContact50LA& operator=(const OriginalSourcePhysicsContact50LA&)=delete;
    ~OriginalSourcePhysicsContact50LA(){if(attempted_&&!nativeWithdrawn_)std::terminate();}
};
}
