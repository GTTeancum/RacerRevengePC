#pragma once
#include "original_source_physics_contact50la.h"
namespace rr::game {
//Conditional original16616C occurrence; no issuer, query adoption or2955F0.
class OriginalSourcePhysicsDetector50LA final {
    friend class OriginalSourcePhysicsCollision50LA;
    friend class ::ra_static_model_cold50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using Base=OriginalSourcePhysicsObject50LA;
    using Contact=OriginalSourcePhysicsContact50LA;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    enum class Status:std::uint8_t {reached,bounded,owner};
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;};
    struct Storage {
        C entity;
        std::unique_ptr<std::uint8_t[]> raw80;
        std::array<std::uint32_t,32> words{},known{},constructorWords{},constructorKnown{};
        std::optional<const void*> contact30,terrain34,collision38,pairs60,objects74;
        std::uint32_t dispatch08=0;
        std::uint8_t flags29Known=0;
        void word(unsigned off,std::uint32_t value)noexcept {
            words[off/4]=value;known[off/4]=0xffffffffu;std::memcpy(raw80.get()+off,&value,4);
        }
        void vector(unsigned off,const std::array<std::uint32_t,4>& v)noexcept {
            for(unsigned i=0;i<4;++i)word(off+4*i,v[i]);
        }
    };
    const void* directCollisionBorrower_=nullptr; //Live E4 predecessor pin, including nullable allocation.
    Base* base_=nullptr;Contact* contact_=nullptr;
    std::unique_ptr<Storage> storage_;
    const std::array<std::uint32_t,4>* origin443550_=nullptr;
    const std::array<float,4>* zero433B10_=nullptr;
    std::optional<const void*> terrainArgument_;
    std::array<std::uint32_t,4> stack50_{};
    C* publishedIdentity_=nullptr;
    bool attempted_=false,controlAllocated_=false,rawAllocationAttempted_=false;
    bool counted_=false,constructorReturned_=false,published_=false;
    bool configureEntered_=false,stackCopied_=false,contactStored_=false,terrainStored_=false;
    bool contact48Stored_=false,identityReturned_=false,configureReturned_=false;
    bool nativeRetired_=false,nativeWithdrawn_=false;
    std::uint32_t pc_=0;
    Result at(Status s,std::uint32_t pc)noexcept {pc_=pc;return {s,pc};}
    void constructorStores()noexcept {
        auto& s=*storage_;s.dispatch08=0x3d3570u;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        s.vector(0x40,vf0);s.vector(0x50,vf0);
        s.word(0x64,0);s.word(0x68,1);s.word(0x6c,1);
        for(auto off:{0x30u,0x34u,0x38u,0x60u,0x70u,0x74u})s.word(off,0);
        s.contact30=nullptr;s.terrain34=nullptr;s.collision38=nullptr;s.pairs60=nullptr;s.objects74=nullptr;
        s.vector(0x40,*origin443550_);s.vector(0x50,*origin443550_);
        s.constructorWords=s.words;s.constructorKnown=s.known;
    }
    Result allocateSource16616C(Base& base,Contact& contact)noexcept {
        if(attempted_||nativeWithdrawn_||!base.preflightNative()||!base.object_||base.detectorBorrower_||
           base.object_->detectorD8||!base.sourceTerrain165E90_||*base.sourceTerrain165E90_!=nullptr||
           contact.base_!=&base||!contact.sourcePoints_.returned||contact.sourcePoints_.nativeWithdrawn||!contact.suffixReturned_||!contact.attachReturned_||contact.detachedNative_||
           contact.detectorBorrower_||contact.detector48Occurrence_||!contact.preflightNative()||!contact.countedEntityNative()||
           base.application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return {Status::owner,0x16616cu};
        base_=&base;contact_=&contact;attempted_=true;base.detectorBorrower_=this;contact.detectorBorrower_=this;
        terrainArgument_=base.sourceTerrain165E90_;origin443550_=&base.application_->detectorOrigin443550_;
        zero433B10_=base.zero433B10_;
        try{storage_=std::make_unique<Storage>();}catch(...){return at(Status::bounded,0x16616cu);}
        controlAllocated_=true;rawAllocationAttempted_=true;
        storage_->raw80.reset(new(std::nothrow) std::uint8_t[0x80]);
        if(storage_->raw80){
            const auto r=initializeOriginalWreckComponent50LA(storage_->entity,K::entity141300,base.generation_,
                Base::key(&storage_->entity),true,base.application_->objectCount423F60());
            if(r.status!=OriginalWreckComponentStatus50LA::complete)return at(Status::bounded,r.sourcePC);
            counted_=true;storage_->entity.kind=K::detector295290;storage_->flags29Known=15;
            constructorStores();constructorReturned_=true;
        }
        publishedIdentity_=constructorReturned_?&storage_->entity:nullptr;
        base.object_->detectorD8=publishedIdentity_;published_=true;
        return at(Status::reached,0x16618cu);
    }
    Result configureSource2953A0()noexcept {
        if(configureEntered_||!published_||pc_!=0x16618cu||!preflightNative())return {Status::owner,0x1661a8u};
        configureEntered_=true;
        std::memcpy(stack50_.data(),zero433B10_->data(),16);stackCopied_=true;
        if(!constructorReturned_)return at(Status::bounded,0x2953d0u);
        storage_->contact30=static_cast<const void*>(&contact_->storage_->entity);storage_->known[0x30/4]=0;contactStored_=true;
        storage_->terrain34=*terrainArgument_;storage_->word(0x34,0);terrainStored_=true;
        contact_->storage_->word(0x48,0);contact_->storage_->physical.generated.pointContacts48=false;
        contact_->detectorCleared48_=true;contact_->detector48Occurrence_=this;
        contact_->detector48NativeWithdrawn_=false;contact48Stored_=true;
        //Exact a3=t0=0 route skips all auxiliary allocations.141C20 is identity.
        identityReturned_=true;configureReturned_=true;
        return at(Status::reached,0x1661b0u);
    }
    using Collision=OriginalSourcePhysicsCollision50LA;
    Collision* collisionBinding_=nullptr;
    const void* collisionBindingHistory_=nullptr;
    bool enabled64Stored_=false,bindingEntered_=false,collision38Stored_=false;
    bool bindingReturned_=false,bindingNativeWithdrawn_=false;
    std::uint32_t bindingPC_=0;
    Result enableSource1661B8()noexcept {
        if(nativeWithdrawn_||nativeRetired_||enabled64Stored_||!configureReturned_||!preflightNative())return {Status::owner,0x1661b8u};
        if(!storage_||!storage_->raw80)return {Status::bounded,0x1661b8u};
        storage_->word(0x64,1);enabled64Stored_=true;bindingPC_=0x1661bcu;
        return {Status::reached,bindingPC_};
    }
    Result bindCollisionSource2955F0(Collision& col)noexcept {
        if(nativeWithdrawn_||nativeRetired_||bindingEntered_||!enabled64Stored_||bindingPC_!=0x1661bcu||!preflightNative()||
           col.base_!=base_||col.bodyBorrow_!=contact_->bodyOwner_||!contact_->storage_||
           !contact_->bodyOwner_||!contact_->storage_->body30||!*contact_->storage_->body30||
           !col.pinSourceDetector2955F0(this,contact_,&contact_->storage_->entity,*contact_->storage_->body30))
            return {Status::owner,0x2955f0u};
        col.detectorBinding_.successorToken=&directCollisionBorrower_;
        bindingEntered_=true;collisionBinding_=&col;collisionBindingHistory_=&col;bindingPC_=0x2955f0u;
        storage_->collision38=static_cast<const void*>(col.collision_.get());storage_->known[0x38/4]=0;
        collision38Stored_=true;bindingPC_=0x295604u;
        if(!col.publishSourceDetector295604(this))return {Status::bounded,bindingPC_};
        bindingReturned_=true;bindingPC_=0x1661c8u;
        return {Status::reached,bindingPC_}; //Actual tail123EA4 return; no numeric V0 or callback invocation.
    }
    bool bindingCurrentNative()const noexcept {
        if(!bindingEntered_)return !collisionBinding_&&!collisionBindingHistory_&&!collision38Stored_&&
            !bindingReturned_&&!bindingNativeWithdrawn_&&bindingPC_==(enabled64Stored_?0x1661bcu:0u);
        if(!enabled64Stored_||!collisionBindingHistory_||!collision38Stored_||
           (bindingReturned_?bindingPC_!=0x1661c8u:bindingPC_!=0x295604u))return false;
        if(bindingNativeWithdrawn_)return !collisionBinding_&&storage_->collision38==std::optional<const void*>(nullptr);
        if(!collisionBinding_||static_cast<const void*>(collisionBinding_)!=collisionBindingHistory_)return false;
        const auto& h=collisionBinding_->detectorBinding_;
        return h.occurrence==this&&!h.nativeWithdrawn&&h.successorToken==&directCollisionBorrower_&&h.contactOccurrence==contact_&&h.contact==&contact_->storage_->entity&&
            h.body==*contact_->storage_->body30&&h.modeStored==bindingReturned_&&
            storage_->collision38==std::optional<const void*>(collisionBinding_->collision_.get())&&
            collisionBinding_->detectorBindingCurrentNative();
    }
    bool withdrawSourceBindingNative()noexcept {
        if(directCollisionBorrower_)return false;
        if(!bindingEntered_||bindingNativeWithdrawn_)return true;
        if(!bindingCurrentNative()||!collisionBinding_->withdrawSourceDetectorBindingNative(this))return false;
        storage_->collision38=nullptr;collisionBinding_=nullptr;bindingNativeWithdrawn_=true;
        return true; //Prior source stores remain historical. Contact borrow remains until whole Detector withdrawal.
    }
#include "original_source_physics_detector_attachment50la.inl"
    C* countedEntityNative()const noexcept {return counted_&&!nativeRetired_&&storage_?&storage_->entity:nullptr;}
    bool preflightNative()const noexcept {
        if(!attempted_)return !base_&&!contact_&&!storage_&&!published_&&!nativeWithdrawn_;
        if(nativeWithdrawn_)return !base_&&!contact_&&!storage_&&!directCollisionBorrower_&&(!counted_||nativeRetired_);
        if(!base_||!contact_||!base_->object_||base_->detectorBorrower_!=this||contact_->detectorBorrower_!=this||
           !base_->preflightNative()||!contact_->preflightNative()||contact_->base_!=base_||
           !contact_->sourcePoints_.returned||contact_->sourcePoints_.nativeWithdrawn||!contact_->suffixReturned_||!contact_->attachReturned_||contact_->detachedNative_||
           terrainArgument_!=std::optional<const void*>(nullptr)||base_->sourceTerrain165E90_!=terrainArgument_||
           origin443550_!=&base_->application_->detectorOrigin443550_||zero433B10_!=base_->zero433B10_)return false;
        if(!storage_)return !controlAllocated_&&!rawAllocationAttempted_&&!counted_&&!published_&&pc_==0x16616cu;
        if(!controlAllocated_||!rawAllocationAttempted_||!bindingCurrentNative())return false;
        if(published_){if(!base_->object_->detectorD8||*base_->object_->detectorD8!=publishedIdentity_)return false;}
        else if(base_->object_->detectorD8)return false;
        if(configureEntered_!=stackCopied_)return false;
        if(stackCopied_){std::array<std::uint32_t,4> v;std::memcpy(v.data(),zero433B10_->data(),16);if(v!=stack50_)return false;}
        if(!storage_->raw80)return !counted_&&!constructorReturned_&&published_&&!publishedIdentity_&&
            !contactStored_&&!terrainStored_&&!contact48Stored_&&!configureReturned_&&pc_==(configureEntered_?0x2953d0u:0x16618cu);
        const auto& e=storage_->entity;
        if(!counted_)return !constructorReturned_&&!published_&&!e.reference00&&e.kind==K::unbound;
        if(nativeRetired_||!constructorReturned_||!published_||publishedIdentity_!=&e||e.reference00!=1||
           e.kind!=K::detector295290||e.sessionGeneration!=base_->generation_||e.objectKey!=Base::key(&e)||
           e.firstChild18||e.mask20||e.descendants24||e.nativeFlags29||
           e.category28!=0xffu||(e.flags29&15u)!=3u||storage_->flags29Known!=15||
           storage_->dispatch08!=0x3d3570u||!base_->application_->objectCount423F60())return false;
        if(!attachmentHistoryNative()||(!attachEntered_&&(e.parent1C||e.previous10||e.next14)))return false;
        for(unsigned i=0;i<32;++i){
            if(i==0x30/4){if(storage_->known[i]!=(contactStored_?0u:0xffffffffu))return false;}
            else if(i==0x38/4){if(storage_->known[i]!=(collision38Stored_?0u:0xffffffffu))return false;}
            else if(i==0x64/4){if(storage_->known[i]!=0xffffffffu||storage_->words[i]!=(enabled64Stored_?1u:0u))return false;}
            else if(storage_->words[i]!=storage_->constructorWords[i]||storage_->known[i]!=storage_->constructorKnown[i])return false;
            if(storage_->known[i]){std::uint32_t raw;std::memcpy(&raw,storage_->raw80.get()+4*i,4);if(raw!=storage_->words[i])return false;}
        }
        if(storage_->contact30!=std::optional<const void*>(contactStored_?static_cast<const void*>(&contact_->storage_->entity):nullptr)||
           storage_->terrain34!=std::optional<const void*>(nullptr)||(!bindingEntered_&&storage_->collision38!=std::optional<const void*>(nullptr))||
           storage_->pairs60!=std::optional<const void*>(nullptr)||storage_->objects74!=std::optional<const void*>(nullptr))return false;
        if(configureEntered_!=contactStored_||contactStored_!=terrainStored_||terrainStored_!=contact48Stored_||
           contact48Stored_!=identityReturned_||identityReturned_!=configureReturned_||
           contact_->detectorCleared48_!=contact48Stored_||
           (contact48Stored_&&(contact_->detector48Occurrence_!=this||contact_->detector48NativeWithdrawn_)))return false;
        return pc_==(configureReturned_?0x1661b0u:0x16618cu);
    }
    template<class RetireLedger>bool withdrawNative(RetireLedger&& retire)noexcept {
        if(!attempted_||nativeWithdrawn_)return true;
        if(directCollisionBorrower_||(attached_&&!detachedNative_)||!preflightNative()||!withdrawSourceBindingNative())return false;
        if(counted_&&(!base_->application_->objectCount423F60()||!retire(storage_->entity)))return false;
        if(counted_){storage_->entity.reference00=0;--base_->application_->objectCount423F60();nativeRetired_=true;}
        if(published_){base_->object_->detectorD8=nullptr;base_->object_->detectorD8NativeWithdrawn=true;}
        if(contact48Stored_)contact_->detector48NativeWithdrawn_=true;
        contact_->detectorBorrower_=nullptr;base_->detectorBorrower_=nullptr;
        storage_.reset();base_=nullptr;contact_=nullptr;origin443550_=nullptr;zero433B10_=nullptr;nativeWithdrawn_=true;
        return true; //Explicit native compensation, not source295320.
    }
public:
    OriginalSourcePhysicsDetector50LA()=default;
    OriginalSourcePhysicsDetector50LA(const OriginalSourcePhysicsDetector50LA&)=delete;
    OriginalSourcePhysicsDetector50LA& operator=(const OriginalSourcePhysicsDetector50LA&)=delete;
    ~OriginalSourcePhysicsDetector50LA(){if(attempted_&&!nativeWithdrawn_)std::terminate();}
};
}
