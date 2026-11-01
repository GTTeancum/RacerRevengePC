#pragma once
#include "original_application_entities50la.h"
#include <array>
#include <memory>
#include <optional>
#include <cstring>
#include <exception>
#include <limits>
#include <type_traits>
namespace ra_static_model_cold50LA {class Owner;}
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
class OriginalCollisionBaseOwner50LA;
class OriginalSourcePhysicsContact50LA;
class OriginalWorldPhysicsIndex50LA;
//Conditional constructor-only owner. No caller,165C30/165E90 setup, child
//Model, prepared wreck state, source destructor dispatch or runtime readiness.
class OriginalSourcePhysicsObject50LA final {
    friend class OriginalSourcePhysicsModel50LA;
    friend class OriginalWorldPhysicsIndex50LA;
    friend class OriginalSourcePhysicsBody50LA;
    friend class OriginalSourcePhysicsGravity50LA;
    friend class OriginalSourcePhysicsCollision50LA;
    friend class OriginalSourcePhysicsContact50LA;
    friend class OriginalSourcePhysicsDetector50LA;
    friend class ::ra_static_model_cold50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using V=std::array<std::uint32_t,4>;
    enum class Status : std::uint8_t { reached,bounded,owner };
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;};
    struct Fields {
        //Native allocation zeroes these arrays only for storage safety.
        //Source authority comes exclusively from per-word known masks.
        std::array<std::uint32_t,0x160/4> words{},known{};
        void word(unsigned offset,std::uint32_t bits)noexcept {
            words[offset/4]=bits;known[offset/4]=0xffffffffu;
        }
        void vector(unsigned offset,const V& bits)noexcept {
            for(unsigned i=0;i<4;++i)word(offset+i*4,bits[i]);
        }
    };
    struct Storage {
        C entity; //PhysicsObject Entity at BASE, never Model's Entity+8.
        Fields fields;
        std::uint32_t dispatch08=0;
        std::uint8_t flags29Known=0;
        //Typed nullable fields; no fabricated 32-bit guest addresses.
        const void* waterB0=nullptr;
        const void* model14C=nullptr;
        const void* pointer150=nullptr;
        bool waterKnown=false,modelKnown=false,pointer150Known=false;
        //Unknown until actual165EC8/165ED0. Native typed identities do
        //not fabricate original32-bit pointer bits in raw Fields storage.
        std::optional<const void*> visibilityB4,collisionB8;
        std::optional<C*> bodyD0; //Unknown until actual165EE8; not constructor1659C0 zero.
        bool bodyD0NativeWithdrawn=false;
        std::optional<C*> detectorD8; //Actual166188 nullable publication.
        bool detectorD8NativeWithdrawn=false;
        std::optional<C*> contactDC; //Actual1660BC only.
        bool contactDCNativeWithdrawn=false;
        std::optional<C*> gravityD4; //Unknown until actual165F60, not1659C0 initialization.
        bool gravityD4NativeWithdrawn=false;
        //Actual outer collision pointer. Embedded Entity+8 is a separate
        //typed counted identity, never substituted for sourceE0.
        std::optional<OriginalCollisionBaseOwner50LA*> collisionE0;
        bool collisionE0NativeWithdrawn=false;
        std::optional<OriginalCollisionBaseOwner50LA*> collisionE4;
        bool collisionE4NativeWithdrawn=false;
        std::optional<OriginalCollisionBaseOwner50LA*> collisionE8;
        bool collisionE8NativeWithdrawn=false;
    };
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    std::unique_ptr<Storage> object_;
    const std::array<float,4>* zero433B10_=nullptr;
    std::uint64_t generation_=0;
    std::optional<C*>* wrapper08_=nullptr;
    C* publishedIdentity_=nullptr;
    std::uint32_t pc_=0;
    bool allocationAttempted_=false,baseCounted_=false,constructorReturned_=false;
    bool wrapperPublished_=false,nativeRetired_=false,nativeWithdrawn_=false;
    Fields expectedFields_;
    struct SourceInverseExtent118 {
        const void* occurrence=nullptr;
        std::optional<std::uint32_t> value;
        std::uint32_t before=0;
        bool nativeWithdrawn=false;
    } sourceInverseExtent118_; //Only reached16652C; never a constructor-default rewrite.
    struct SourceConfigurationTail166650 {
        const void* occurrence=nullptr; //Native same-call scratch identity, not a guest pointer store.
        const void* contactIdentity=nullptr; //Opaque producer identity retained for history, never dereferenced.
        bool entered=false,returned=false;
        std::uint32_t pc=0;
        std::uint16_t inputKnown=0;
        std::uint8_t stores=0;
        std::array<std::uint32_t,5> arguments{};
        std::array<std::uint32_t,7> before{},beforeKnown{},values{};
    } sourceConfigurationTail_;
    static constexpr std::array<unsigned,7> configurationTailOffsets_{{0xec,0xf0,0xf4,0xf8,0xfc,0x100,0x104}};
    bool sourceConfigurationTailHistoryNative()const noexcept {
        const auto& h=sourceConfigurationTail_;
        if(!h.entered)return !h.occurrence&&!h.contactIdentity&&!h.returned&&!h.pc&&!h.inputKnown&&!h.stores;
        if(!h.occurrence||!h.contactIdentity||!h.returned||h.pc!=0x1666b8u||h.inputKnown!=0x555u||h.stores!=0x7fu)return false;
        for(unsigned i=0;i<7;++i){const auto index=configurationTailOffsets_[i]/4;
            if(h.before[i]!=expectedFields_.words[index]||h.beforeKnown[i]!=expectedFields_.known[index]||
               h.values[i]!=(i<5?h.arguments[i]:h.arguments[i-5]))return false;}
        return true;
    }
    template<class ActualContact>
    bool sourceConfigurationTailReturnCurrentNative(const ActualContact& actualContact,const void* occurrence)const noexcept {
        static_assert(std::is_same<ActualContact,OriginalSourcePhysicsContact50LA>::value,
            "Only same-source Contact authenticates the live configuration return");
        return occurrence&&sourceConfigurationTail_.occurrence==occurrence&&
            sourceConfigurationTail_.contactIdentity==&actualContact&&sourceConfigurationTail_.returned&&
            !nativeWithdrawn_&&!nativeRetired_&&object_&&preflightNative()&&
            actualContact.sourceConfigurationTailAdmissionNative(this);
    }
    template<class ActualContact>
    Result publishSourceConfigurationTail166650(const ActualContact& actualContact,const void* actualOccurrence,
        const std::array<std::uint32_t,12>& callerStack,std::uint16_t callerKnown)noexcept {
        static_assert(std::is_same<ActualContact,OriginalSourcePhysicsContact50LA>::value,
            "Only the exact same-source Contact can authorize this configuration tail");
        auto& h=sourceConfigurationTail_;
        if(h.entered||!actualOccurrence||callerKnown!=0x555u||nativeWithdrawn_||nativeRetired_||
           !constructorReturned_||!object_||!preflightNative()||
           !actualContact.sourceConfigurationTailAdmissionNative(this))return {Status::owner,0x166650u};
        h.entered=true;h.occurrence=actualOccurrence;h.contactIdentity=&actualContact;h.inputKnown=callerKnown;
        for(unsigned i=0;i<7;++i){const auto index=configurationTailOffsets_[i]/4;
            h.before[i]=object_->fields.words[index];h.beforeKnown[i]=object_->fields.known[index];}
        for(unsigned i=0;i<5;++i){h.arguments[i]=callerStack[2*(i+1)];h.pc=0x166654u+8u*i;
            object_->fields.word(configurationTailOffsets_[i],h.arguments[i]);h.values[i]=h.arguments[i];h.stores|=std::uint8_t(1u<<i);}
        //Actual source reloads EC and F0 after their preceding stores; no numeric conversion.
        h.pc=0x16667cu;h.values[5]=object_->fields.words[0xec/4];object_->fields.word(0x100,h.values[5]);h.stores|=32u;
        h.pc=0x166684u;h.values[6]=object_->fields.words[0xf0/4];object_->fields.word(0x104,h.values[6]);h.stores|=64u;
        h.returned=true;h.pc=0x1666b8u;return {Status::reached,h.pc}; //JR1666B0/delay1666B4 control only, no defined new V0.
    }
    const void* proceduralCollisionBorrower_=nullptr; //Distinct nullable BaseE8 occurrence.
    const void* directCollisionBorrower_=nullptr; //Distinct nullable BaseE4 occurrence.
    const void* collisionBorrower_=nullptr; //Exact nullable100 occurrence.
    const void* contactBorrower_=nullptr;
    const void* detectorBorrower_=nullptr;
    //Actual165EAC argument receipt; not a guest object field. Root alone issues.
    std::optional<const void*> sourceTerrain165E90_;
    const void* gravityBorrower_=nullptr; //Exact nullable60 occurrence.
    const void* bodyBorrower_=nullptr; //Exact conditional nullable330 occurrence.
    C* nestedChild141960_=nullptr; //Exact live nested Model Entity+8, mode0.
    C* bodyChild141960_=nullptr; //Exact live Body D0, modeFFFFFFFF.
    C* gravityChild141960_=nullptr; //Exact live Gravity D4, mode0.
    C* detectorChild141960_=nullptr; //Exact Detector EntityBASE, mode0.
    C* contactChild141960_=nullptr; //Exact Contact EntityBASE, mode0.
    C* proceduralCollisionChild141960_=nullptr; //Exact eighth E8 embedded Entity+8.
    const OriginalCollisionBaseOwner50LA* proceduralCollisionChildOuter141960_=nullptr;
    C* directCollisionChild141960_=nullptr; //Exact seventh E4 embedded Entity+8.
    const OriginalCollisionBaseOwner50LA* directCollisionChildOuter141960_=nullptr;
    C* collisionChild141960_=nullptr; //Exact COL embedded Entity+8, mode0.
    const OriginalCollisionBaseOwner50LA* collisionChildOuter141960_=nullptr;
    //Structural, nonrecursive custody: no payload/currentness promotion.
    //Only these eight source occurrences may occupy this BASE child chain.
    bool sourceChildChainCurrentNative()const noexcept {
        if(!object_)return !nestedChild141960_&&!bodyChild141960_&&!gravityChild141960_&&!collisionChild141960_&&!collisionChildOuter141960_&&!contactChild141960_&&!detectorChild141960_&&!directCollisionChild141960_&&!directCollisionChildOuter141960_&&!proceduralCollisionChild141960_&&!proceduralCollisionChildOuter141960_;
        const auto& o=*object_;const auto& parent=o.entity;
        C* ordered[8]={nestedChild141960_,bodyChild141960_,gravityChild141960_,collisionChild141960_,contactChild141960_,detectorChild141960_,directCollisionChild141960_,proceduralCollisionChild141960_};
        const K kinds[8]={K::model2403A0,K::body167920,K::gravity282670,K::collision11B190,K::contact295A40,K::detector295290,K::collision11B190,K::collision11B190};
        const std::uint32_t masks[8]={0u,0xffffffffu,0u,0u,0u,0u,0u,0u};
        if(nestedChild141960_&&(!nestedModelBorrower_||!o.model14C))return false;
        if(bodyChild141960_&&(!bodyBorrower_||!o.bodyD0||*o.bodyD0!=bodyChild141960_||o.bodyD0NativeWithdrawn))return false;
        if(gravityChild141960_&&(!gravityBorrower_||!o.gravityD4||*o.gravityD4!=gravityChild141960_||o.gravityD4NativeWithdrawn))return false;
        if(bool(collisionChild141960_)!=bool(collisionChildOuter141960_)||
           (collisionChild141960_&&(!collisionBorrower_||!o.collisionE0||
            *o.collisionE0!=collisionChildOuter141960_||o.collisionE0NativeWithdrawn)))return false;
        if(contactChild141960_&&(!contactBorrower_||!o.contactDC||*o.contactDC!=contactChild141960_||o.contactDCNativeWithdrawn))return false;
        if(detectorChild141960_&&(!detectorBorrower_||!o.detectorD8||*o.detectorD8!=detectorChild141960_||o.detectorD8NativeWithdrawn))return false;
        if(bool(directCollisionChild141960_)!=bool(directCollisionChildOuter141960_)||
           (directCollisionChild141960_&&(!directCollisionBorrower_||!o.collisionE4||
            *o.collisionE4!=directCollisionChildOuter141960_||o.collisionE4NativeWithdrawn)))return false;
        if(bool(proceduralCollisionChild141960_)!=bool(proceduralCollisionChildOuter141960_)||
           (proceduralCollisionChild141960_&&(!proceduralCollisionBorrower_||!o.collisionE8||
            *o.collisionE8!=proceduralCollisionChildOuter141960_||o.collisionE8NativeWithdrawn)))return false;
        C* previous=nullptr;C* first=nullptr;std::uint32_t descendants=0;
        for(unsigned i=0;i<8;++i)if(auto* child=ordered[i]){
            for(unsigned j=0;j<i;++j)if(ordered[j]==child)return false;
            C* next=nullptr;for(unsigned j=i+1;j<8;++j)if(ordered[j]){next=ordered[j];break;}
            if(!first)first=child;
            if(child->kind!=kinds[i]||child->reference00!=1||child->sessionGeneration!=generation_||
               child->objectKey!=key(child)||child->parent1C!=&parent||child->previous10!=previous||
               child->next14!=next||child->firstChild18||child->mask20!=masks[i]||child->descendants24)return false;
            descendants|=child->mask20;previous=child;
        }
        return parent.firstChild18==first&&parent.descendants24==descendants;
    }
    const void* physicsIndexBorrower_=nullptr; //Exact27C290 manager receipt, no guest retain.
    const void* nestedModelBorrower_=nullptr; //Exact165C30 occurrence, includingNULL/partial publication.
    static std::uint64_t key(const C* c)noexcept {
        //Native identity only, never an original allocation address.
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(c));
    }
    Result at(Status status,std::uint32_t pc)noexcept {pc_=pc;return {status,pc};}
    V readActualZeroNative()const noexcept {
        V bits{};std::memcpy(bits.data(),zero433B10_->data(),16);return bits;
    }
    void stores1659E0()noexcept {
        auto& o=*object_;auto& f=o.fields;
        o.dispatch08=0x3cdf80u;
        const V vf00{{0,0,0,0x3f800000u}};
        f.vector(0x50,vf00);f.vector(0x60,vf00);f.vector(0x70,vf00);
        f.vector(0x80,vf00);f.vector(0x90,vf00); //2E8860/1048E0 two-element ctor.
        f.vector(0xa0,vf00);f.vector(0xc0,vf00);
        //Six original LQ/SQ pairs, same actual Application lifetime global.
        f.vector(0x50,readActualZeroNative());f.vector(0x60,readActualZeroNative());
        f.vector(0x70,readActualZeroNative());f.vector(0xa0,readActualZeroNative());
        f.vector(0x80,readActualZeroNative());f.vector(0x90,readActualZeroNative());
        f.word(0x3c,0);f.word(0x40,0);
        o.waterB0=nullptr;o.waterKnown=true; //165ACC
        o.model14C=nullptr;o.modelKnown=true; //165AD4
        o.pointer150=nullptr;o.pointer150Known=true; //165ADC, not unknown.
        f.word(0x30,1);f.word(0x34,0);f.word(0x38,0);
        f.word(0x120,0x3f000000u);f.word(0x110,0);
        f.word(0x114,0x3f800000u);f.word(0x118,0x3f800000u);
        f.word(0xec,0);f.word(0xf0,0);f.word(0xf4,0);f.word(0xf8,0);
        f.word(0xfc,0);f.word(0x100,0);f.word(0x104,0);
        f.word(0x108,0x43480000u);f.word(0x10c,0x42480000u);
        f.word(0x11c,0);f.word(0x124,0);f.word(0x128,0);
        f.word(0x12c,0);f.word(0x130,0);
        f.word(0x134,0x41200000u);f.word(0x138,0x3f000000u);
        f.word(0x13c,0x41200000u);f.word(0x140,0x3f800000u);
        f.word(0x144,0);f.word(0x148,0);
        //B4/B8 andD0..E8 remain unknown/unpublished. No derived stores160+.
        expectedFields_=f;
    }
    Result allocateSource27B400(std::shared_ptr<OriginalApplicationEntities50LA> app,
        std::uint64_t generation,std::optional<C*>& actualWrapper08)noexcept {
        if(allocationAttempted_||nativeWithdrawn_||!app||!generation||
           !app->application10()||!app->application26C()||!actualWrapper08||*actualWrapper08||
           app->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
            return at(Status::owner,0x27b400);
        //Root authenticates descriptor/type2/zone and reserves actual ledger
        //capacity BEFORE this source allocation. No prepared Wreck authority.
        application_=std::move(app);generation_=generation;wrapper08_=&actualWrapper08;
        zero433B10_=&application_->wreckAssetVectors_.zero433B10;
        allocationAttempted_=true;
        try{object_=std::make_unique<Storage>();}catch(...){object_.reset();}
        if(object_){
            const auto r=initializeOriginalWreckComponent50LA(object_->entity,K::entity141300,
                generation_,key(&object_->entity),true,application_->objectCount423F60());
            if(r.status!=OriginalWreckComponentStatus50LA::complete)return at(Status::bounded,0x141300);
            baseCounted_=true;object_->flags29Known=0x0fu; //UpperF0 remains source unknown.
            stores1659E0();object_->entity.kind=K::physicsObject1659C0;
            constructorReturned_=true;
        }
        //27B41C publishes evenNULL; NULL does not enroll/count an Entity.
        publishedIdentity_=object_?&object_->entity:nullptr;
        actualWrapper08=publishedIdentity_;wrapperPublished_=true;
        return at(Status::reached,0x27b420);
    }
    //1B3C5C forms an INLINE byte address (+38), not a name-pointer load.
    //165AF4 initialized this word to zero. Read only its first little-endian
    //byte;308F00 terminates on that byte, so no invented string storage or
    //nested Model14C/name substitution is needed for the empty prefix.
    bool readSourceInlineName38FirstByteNative(const OriginalApplicationEntities50LA* actualApplication,
        std::uint64_t actualGeneration,const C* actualWrapperBody,std::uint8_t& byte)const noexcept {
        if(!actualApplication||application_.get()!=actualApplication||generation_!=actualGeneration||
           !actualWrapperBody||!constructorReturned_||nativeWithdrawn_||nativeRetired_||
           !object_||!wrapperPublished_||publishedIdentity_!=actualWrapperBody||
           actualWrapperBody!=&object_->entity||!preflightNative())return false;
        const auto& fields=object_->fields;
        if((fields.known[0x38/4]&0xffu)!=0xffu)return false;
        byte=static_cast<std::uint8_t>(fields.words[0x38/4]&0xffu);
        return true;
    }
    C* countedEntityNative()const noexcept {
        return object_&&baseCounted_&&!nativeRetired_?&object_->entity:nullptr;
    }
    #include "original_source_physics_parent_attachment50la.inl"
    bool preflightNative()const noexcept {
        if(!sourceParentAttachmentHistoryNative()||
           (physicsIndexBorrower_&&(!parentAttachment_.returned||nativeWithdrawn_)))return false;
        if(!allocationAttempted_)return !application_&&!object_&&!baseCounted_&&!wrapperPublished_&&!nativeWithdrawn_;
        if(nativeWithdrawn_)return !object_&&!nestedModelBorrower_&&!nestedChild141960_&&!bodyChild141960_&&!gravityChild141960_&&!collisionChild141960_&&!collisionChildOuter141960_&&!contactChild141960_&&!detectorChild141960_&&!directCollisionChild141960_&&!directCollisionChildOuter141960_&&!proceduralCollisionChild141960_&&!proceduralCollisionChildOuter141960_&&!bodyBorrower_&&!gravityBorrower_&&!collisionBorrower_&&!directCollisionBorrower_&&!proceduralCollisionBorrower_&&!contactBorrower_&&!detectorBorrower_&&(!baseCounted_||nativeRetired_);
        if(!application_||!generation_||!wrapper08_||
           zero433B10_!=&application_->wreckAssetVectors_.zero433B10)return false;
        if(wrapperPublished_&&(!*wrapper08_||**wrapper08_!=publishedIdentity_))return false;
        if(!object_)return !baseCounted_&&!constructorReturned_&&!nativeRetired_&&
            wrapperPublished_&&!publishedIdentity_;
        const auto& o=*object_;const auto& e=o.entity;
        if(!baseCounted_){ //Only an uncounted native constructor-admission refusal.
            return !constructorReturned_&&!wrapperPublished_&&!nativeRetired_&&
                !e.reference00&&e.kind==K::unbound&&!o.dispatch08&&!o.flags29Known;
        }
        if(nativeRetired_||!constructorReturned_||!wrapperPublished_||publishedIdentity_!=&e||
           e.reference00!=1||e.sessionGeneration!=generation_||e.objectKey!=key(&e)||
           e.kind!=K::physicsObject1659C0||!sourceChildChainCurrentNative()||
           e.nativeFlags29||e.category28!=0xffu||
           (e.flags29&0x0fu)!=3u||o.flags29Known!=0x0fu||o.dispatch08!=0x3cdf80u||
           !application_->objectCount423F60())return false;
        if(!o.waterKnown||!o.modelKnown||!o.pointer150Known||o.waterB0||
           (o.model14C&&!nestedModelBorrower_)||o.pointer150)return false;
        if(o.bodyD0){
            if(o.bodyD0NativeWithdrawn){if(*o.bodyD0||bodyBorrower_)return false;}
            else if(!bodyBorrower_)return false;
        }else if(o.bodyD0NativeWithdrawn)return false;
        if(o.gravityD4){
            if(o.gravityD4NativeWithdrawn){if(*o.gravityD4||gravityBorrower_)return false;}
            else if(!gravityBorrower_)return false;
        }else if(o.gravityD4NativeWithdrawn)return false;
        if(o.detectorD8){
            if(o.detectorD8NativeWithdrawn){if(*o.detectorD8||detectorBorrower_)return false;}
            else if(!detectorBorrower_)return false;
        }else if(o.detectorD8NativeWithdrawn)return false;
        if(o.contactDC){
            if(o.contactDCNativeWithdrawn){if(*o.contactDC||contactBorrower_)return false;}
            else if(!contactBorrower_)return false;
        }else if(o.contactDCNativeWithdrawn)return false;
        if(o.collisionE0){
            if(o.collisionE0NativeWithdrawn){if(*o.collisionE0||collisionBorrower_)return false;}
            else if(!collisionBorrower_)return false;
        }else if(o.collisionE0NativeWithdrawn)return false;
        if(o.collisionE4){
            if(o.collisionE4NativeWithdrawn){if(*o.collisionE4||directCollisionBorrower_)return false;}
            else if(!directCollisionBorrower_)return false;
        }else if(o.collisionE4NativeWithdrawn)return false;
        if(o.collisionE8){
            if(o.collisionE8NativeWithdrawn){if(*o.collisionE8||proceduralCollisionBorrower_)return false;}
            else if(!proceduralCollisionBorrower_)return false;
        }else if(o.collisionE8NativeWithdrawn)return false;
        const auto& extent=sourceInverseExtent118_;
        if(extent.value){
            if(!extent.occurrence||extent.before!=expectedFields_.words[0x118/4]||
               expectedFields_.known[0x118/4]!=0xffffffffu||
               (extent.nativeWithdrawn?bodyBorrower_!=nullptr:bodyBorrower_!=extent.occurrence))return false;
        }else if(extent.occurrence||extent.nativeWithdrawn||extent.before)return false;
        if(!sourceConfigurationTailHistoryNative())return false;
        for(unsigned i=0;i<o.fields.words.size();++i){
            auto expected=i==0x118/4&&extent.value?*extent.value:expectedFields_.words[i];
            auto known=expectedFields_.known[i];
            if(sourceConfigurationTail_.entered)for(unsigned lane=0;lane<7;++lane)
                if(i==configurationTailOffsets_[lane]/4&&(sourceConfigurationTail_.stores&(1u<<lane))){
                    expected=sourceConfigurationTail_.values[lane];known=0xffffffffu;break;}
            if(o.fields.known[i]!=known||((o.fields.words[i]^expected)&o.fields.known[i]))return false;
        }
        return true;
    }
    template<class RetireFromActualLedger>
    bool withdrawNative(RetireFromActualLedger&& retire)noexcept {
        if(!allocationAttempted_||nativeWithdrawn_)return true;
        if(physicsIndexBorrower_||(parentAttachment_.attached&&!parentAttachment_.nativeDetached)||nestedModelBorrower_||bodyBorrower_||gravityBorrower_||collisionBorrower_||directCollisionBorrower_||proceduralCollisionBorrower_||contactBorrower_||detectorBorrower_||!preflightNative())return false;
        if(baseCounted_&&!nativeRetired_){
            //Exact root retires ledger/authority membership before count/free.
            //Refusal leaves source custody intact and allows a later retry.
            if(!retire(object_->entity))return false;
            object_->entity.reference00=0;--application_->objectCount423F60();nativeRetired_=true;
        }
        if(wrapperPublished_)*wrapper08_=nullptr; //Native alias withdrawal, not source store.
        object_.reset();nativeWithdrawn_=true;zero433B10_=nullptr;application_.reset();return true;
    }
public:
    OriginalSourcePhysicsObject50LA()=default;
    OriginalSourcePhysicsObject50LA(const OriginalSourcePhysicsObject50LA&)=delete;
    OriginalSourcePhysicsObject50LA& operator=(const OriginalSourcePhysicsObject50LA&)=delete;
    ~OriginalSourcePhysicsObject50LA(){if(allocationAttempted_&&!nativeWithdrawn_)std::terminate();}
};
}
