#pragma once
#include "original_wreck_physical50la.h"
#include <memory>
#include <cmath>
#include <array>
#include <optional>
#include <new>

namespace rr::game {
// Borrow actual piece fields, canonical D0 and its piece Entity. This is not
// a OnePieceWreckOwner and never constructs another group around a fragment.
struct OriginalWreckGroupPiece50LA {
    OriginalWreckPieceFields50LA* piece=nullptr;
    OriginalWreckPhysical50LA* physical=nullptr;
    OriginalWreckComponent50LA* component=nullptr;
};
enum class OriginalWreckContactBodyDestination50LA : std::uint8_t {primaryBody,ownBody};
enum class OriginalWreckGroupArrayStatus50LA : std::uint8_t {
    coldBacking,fieldsInitialized,primaryBound,memberStored,primarySkipped,rootRefreshed,contactBindingStored,full,
    binding,allocation,arithmetic
};
struct OriginalWreckGroupArrayResult50LA {
    OriginalWreckGroupArrayStatus50LA status=OriginalWreckGroupArrayStatus50LA::binding;
    std::uint32_t sourcePC=0;
    std::size_t slot=0;
};
// Sole six-array backing of one actual shared group. prepareCold performs no
// source constructor writes. Initialization/binding below represent only their
// named source field phases, never component construction,2CC060 effects,
//2CA480 attachment/exclusions, or source retirement/reference completion.
class OriginalWreckGroupArrays50LA final {
    friend class OriginalWreckRacerStorage50LA;
    bool nativeRetired_=false;
    std::size_t allocated_=0;
    bool cold_=false,fields_=false,bound_=false;
    OriginalWreckComponent50LA* component_=nullptr;
    OriginalWreckGroupPiece50LA primary_{};
    std::uint32_t registered30_=0,capacity34_=0;
    float mass4C_=0.f,thresholdAC_=0.f;
    SpringVector4_48N center50_{};
    BodyMatrix48G inertia60_{};
    std::unique_ptr<OriginalWreckGroupPiece50LA[]> pieces44_;
    std::unique_ptr<SpringVector4_48N[]> offsets48_;
    std::unique_ptr<std::uint32_t[]> attachedA0_;
    std::unique_ptr<float[]> breakRateA4_,breakScaleA8_;
    std::unique_ptr<std::uint32_t[]> breakLatchB0_;
    // Native selection ledger, not guest allocation results. The legacy
    // reservation prewarmed six owners before any source constructor. Fresh
    // selection explicitly cancels ONLY that unused backing; allocated_ keeps
    // its historical native capacity. No source destructor/null store is implied.
    bool freshSelected_=false;
    bool freshPrimaryAttempted_=false,freshPrimaryComplete_=false;
    std::uint32_t freshPrimaryPC_=0;
    std::uint8_t freshPrimaryInitMask_=0; //Reached44/A0/B0 stores, capacity1 only.
    std::array<const void*,6> freshPrimaryAllocated_{}; //Native allocation identities, never dereferenced after retirement.

    const std::array<std::uint32_t,4>* freshOrigin4441F0_=nullptr;
    std::array<std::optional<const void*>,6> freshArrayAliases_{}; //44,48,A0,A4,A8,B0
    bool selectFreshUnusedBacking()noexcept {
        if(freshSelected_||nativeRetired_||!cold_||fields_||bound_||component_||
           registered30_||capacity34_||primary_.piece||primary_.physical||primary_.component)
            return false;
        pieces44_.reset();offsets48_.reset();attachedA0_.reset();
        breakRateA4_.reset();breakScaleA8_.reset();breakLatchB0_.reset();
        cold_=false;freshSelected_=true;return true;
    }
    void initializeFreshFields2C9E40(OriginalWreckComponent50LA& component,
        const std::array<std::uint32_t,4>& actual4441F0)noexcept {
        // Called immediately after the SAME counted141300 Entity. All source
        // writes below are nonfallible raw stores; no arithmetic/finite gate.
        component_=&component;
        center50_={{0.f,0.f,0.f,1.f}}; //2C9E6C VF0
        registered30_=0; //2C9E7C
        capacity34_=0; //2C9E84
        primary_.physical=nullptr; //2C9E8C body3C
        primary_.piece=nullptr;primary_.component=nullptr; //2C9E94 piece40/typed custody
        freshArrayAliases_[0]=nullptr; //2C9E9C 44
        freshArrayAliases_[1]=nullptr; //2C9EA4 48
        mass4C_=0.f; //2C9EAC
        freshOrigin4441F0_=&actual4441F0;
        std::memcpy(center50_.v,actual4441F0.data(),16); //2C9EBC
        //14D7F0 produces sixteen ZERO words, not an identity matrix.
        //2C9EDC/E0 stores eight word-pairs into the same canonical60.
        for(unsigned i=0;i<16u;i+=2u){inertia60_.v[i]=0.f;inertia60_.v[i+1u]=0.f;}
        freshArrayAliases_[2]=nullptr; //2C9EFC A0
        freshArrayAliases_[3]=nullptr; //2C9F04 A4
        freshArrayAliases_[4]=nullptr; //2C9F0C A8
        thresholdAC_=original_wreck_physical_detail50LA::word(0x3ca3d70au); //2C9F14
        freshArrayAliases_[5]=nullptr; //2C9F1C B0
        fields_=true; //38 remains source-unwritten; no primary/array allocation.
    }
    bool freshConstructorFieldsCurrent(const OriginalWreckComponent50LA& component)const noexcept {
        if(!freshSelected_||cold_||!fields_||bound_||component_!=&component||!freshOrigin4441F0_||
           registered30_||capacity34_||primary_.piece||primary_.physical||primary_.component||
           pieces44_||offsets48_||attachedA0_||breakRateA4_||breakScaleA8_||breakLatchB0_||mass4C_!=0.f)
            return false;
        for(const auto& alias:freshArrayAliases_)if(!alias||*alias)return false;
        if(std::memcmp(center50_.v,freshOrigin4441F0_->data(),16)!=0||
           thresholdAC_!=original_wreck_physical_detail50LA::word(0x3ca3d70au))return false;
        for(float v:inertia60_.v){std::uint32_t bits;std::memcpy(&bits,&v,4);if(bits)return false;}
        return true;
    }
    bool freshPrimaryPrefixCurrent(const OriginalWreckComponent50LA& component,
        const OriginalWreckGroupPiece50LA& expected,std::uint32_t capacity)const noexcept {
        if(!freshSelected_||cold_||!fields_||!freshPrimaryAttempted_||component_!=&component||
           !freshOrigin4441F0_||capacity>1u||capacity34_!=capacity||registered30_||
           primary_.piece!=expected.piece||primary_.physical!=expected.physical||primary_.component!=expected.component||
           !expected.piece||!expected.physical||!expected.component||
           thresholdAC_!=original_wreck_physical_detail50LA::word(0x3ca3d70au))return false;
        const void* owners[6]={pieces44_.get(),offsets48_.get(),attachedA0_.get(),breakRateA4_.get(),breakScaleA8_.get(),breakLatchB0_.get()};
        for(unsigned k=0;k<6;++k){
            if(!freshArrayAliases_[k]||*freshArrayAliases_[k]!=freshPrimaryAllocated_[k])return false;
            if(nativeRetired_){if(owners[k])return false;}
            else if(owners[k]!=freshPrimaryAllocated_[k])return false;
            if(!capacity&&freshPrimaryAllocated_[k])return false;
        }
        if(freshPrimaryComplete_){
            if(!bound_||freshPrimaryPC_!=0x2ca17cu||freshPrimaryInitMask_!=(capacity?7u:0u))return false;
        }else{
            if(bound_||!capacity)return false;
            if(freshPrimaryPC_==0x2ca0e4u){if(freshPrimaryInitMask_||freshPrimaryAllocated_[0])return false;}
            else if(freshPrimaryPC_==0x2ca0f0u){if(freshPrimaryInitMask_!=1u||!freshPrimaryAllocated_[0]||freshPrimaryAllocated_[2])return false;}
            else if(freshPrimaryPC_==0x2ca100u){if(freshPrimaryInitMask_!=3u||!freshPrimaryAllocated_[0]||!freshPrimaryAllocated_[2]||freshPrimaryAllocated_[5])return false;}
            else return false;
        }
        //After native retirement source aliases are historical. Never touch the
        //primary or freed arrays; the same stored identities remain the proof.
        if(nativeRetired_)return true;
        if(!pieceBound(expected,component.sessionGeneration))return false;
        if(capacity){
            if((freshPrimaryInitMask_&1u)&&(pieces44_[0].piece||pieces44_[0].physical||pieces44_[0].component))return false;
            if((freshPrimaryInitMask_&2u)&&attachedA0_[0])return false;
            if((freshPrimaryInitMask_&4u)&&breakLatchB0_[0])return false;
            if(offsets48_){const std::uint32_t vf0[4]={0,0,0,0x3f800000u};
                if(std::memcmp(offsets48_[0].v,vf0,16))return false;}
            //A4/A8 are genuinely source-unknown: never read either element.
        }
        if(freshPrimaryComplete_){
            if(expected.piece->wake30!=1u||expected.component->mask20||
               std::memcmp(&mass4C_,&expected.physical->parameters.body.mass,4)||
               std::memcmp(inertia60_.v,expected.physical->inertia100.v,64)||
               std::memcmp(center50_.v,expected.physical->parameters.centerOffset.v,12)||
               std::memcmp(center50_.v+3,&expected.physical->parameters.centerOffsetW,4))return false;
        }else{
            const std::uint32_t zero=0;
            if(std::memcmp(&mass4C_,&zero,4)||std::memcmp(center50_.v,freshOrigin4441F0_->data(),16))return false;
            for(float v:inertia60_.v){std::uint32_t bits;std::memcpy(&bits,&v,4);if(bits)return false;}
        }
        return true;
    }
    bool bindFreshPrimary2CA020(const OriginalWreckGroupPiece50LA& primary,
        std::uint32_t actualCapacity,std::uint32_t& pc)noexcept {
        pc=0x2ca020u;
        if(!component_||nativeRetired_||freshPrimaryAttempted_||actualCapacity>1u||
           !freshConstructorFieldsCurrent(*component_)||!pieceBound(primary,component_->sessionGeneration)||
           primary.component==component_||primary.component->objectKey==component_->objectKey)return false;
        freshPrimaryAttempted_=true;
        const auto at=[&](std::uint32_t value){pc=freshPrimaryPC_=value;};
        at(0x2ca030u);capacity34_=actualCapacity;
        at(0x2ca034u);primary_.piece=primary.piece;primary_.component=primary.component;
        at(0x2ca040u);primary_.physical=primary.physical;
        if(static_cast<std::int32_t>(capacity34_)>0){
            //The same six native owners now stand for actual source allocations;
            //no prewarm reuse. Native pointer/element sizes replace guest bytes.
            at(0x2ca050u);pieces44_.reset(new(std::nothrow) OriginalWreckGroupPiece50LA[capacity34_]);
            at(0x2ca058u);freshArrayAliases_[0]=freshPrimaryAllocated_[0]=pieces44_.get();
            at(0x2ca060u);attachedA0_.reset(new(std::nothrow) std::uint32_t[capacity34_]);
            at(0x2ca068u);freshArrayAliases_[2]=freshPrimaryAllocated_[2]=attachedA0_.get();
            at(0x2ca074u);offsets48_.reset(new(std::nothrow) SpringVector4_48N[capacity34_]);
            //2E8990 returns NULL for NULL at2E89C8. Otherwise its1048E0
            //element constructors store VF0 before the returned48 publication.
            at(0x2ca090u);if(offsets48_)for(std::size_t i=0;i<capacity34_;++i)offsets48_[i]={{0.f,0.f,0.f,1.f}};
            at(0x2ca098u);freshArrayAliases_[1]=freshPrimaryAllocated_[1]=offsets48_.get();
            at(0x2ca0a0u);breakRateA4_.reset(new(std::nothrow) float[capacity34_]);
            at(0x2ca0a8u);freshArrayAliases_[3]=freshPrimaryAllocated_[3]=breakRateA4_.get();
            at(0x2ca0b0u);breakScaleA8_.reset(new(std::nothrow) float[capacity34_]);
            at(0x2ca0b8u);freshArrayAliases_[4]=freshPrimaryAllocated_[4]=breakScaleA8_.get();
            at(0x2ca0c0u);breakLatchB0_.reset(new(std::nothrow) std::uint32_t[capacity34_]);
            at(0x2ca0c8u);freshArrayAliases_[5]=freshPrimaryAllocated_[5]=breakLatchB0_.get();
            //Only actual count1/2 reaches this private route, so at most one row.
            at(0x2ca0e4u);if(!pieces44_)return false;pieces44_[0]={};freshPrimaryInitMask_|=1u;
            at(0x2ca0f0u);if(!attachedA0_)return false;attachedA0_[0]=0;freshPrimaryInitMask_|=2u;
            at(0x2ca100u);if(!breakLatchB0_)return false;breakLatchB0_[0]=0;freshPrimaryInitMask_|=4u;
        }
        at(0x2ca128u);primary_.piece->wake30=1;
        at(0x2ca130u);primary_.component->mask20=0;
        at(0x2ca13cu);std::memcpy(&mass4C_,&primary_.physical->parameters.body.mass,4);
        for(unsigned i=0;i<16u;i+=2u){
            at(0x2ca154u);std::memcpy(inertia60_.v+i,primary_.physical->inertia100.v+i,4);
            at(0x2ca158u);std::memcpy(inertia60_.v+i+1u,primary_.physical->inertia100.v+i+1u,4);
        }
        at(0x2ca178u);std::memcpy(center50_.v,primary_.physical->parameters.centerOffset.v,12);
        std::memcpy(center50_.v+3,&primary_.physical->parameters.centerOffsetW,4);
        //141C20 returns self without writes. Source38 remains unproduced.
        bound_=freshPrimaryComplete_=true;at(0x2ca17cu);return true;
    }
    bool retireFreshConstructor2C9F30()noexcept {
        if(!component_||component_->reference00||nativeRetired_)return false;
        if(freshPrimaryAttempted_){
            if(!freshPrimaryPrefixCurrent(*component_,primary_,capacity34_))return false;
        }else if(!freshConstructorFieldsCurrent(*component_))return false;
        //Source destructor order44,48,A0,A4,A8,B0. Retain aliases and native
        //identity ledgers for post-retirement proof; no fabricated NULL stores.
        pieces44_.reset();offsets48_.reset();attachedA0_.reset();
        breakRateA4_.reset();breakScaleA8_.reset();breakLatchB0_.reset();
        nativeRetired_=true;return true;
    }
    static bool pieceBound(const OriginalWreckGroupPiece50LA& p,std::uint64_t generation)noexcept {
        return p.piece&&p.physical&&p.component&&p.physical->bodyConstructorFieldsApplied&&
            p.component->kind==OriginalWreckComponentKind50LA::piece2CB4B0&&
            p.component->reference00&&p.component->objectKey&&p.component->sessionGeneration==generation;
    }
    bool retireNativeUnactivated2C9F30(bool authenticatedNativeTuning=false)noexcept {
        if(nativeRetired_)return true;
        if(!cold_||!fields_||!component_||component_->reference00||(!authenticatedNativeTuning&&registered30_))return false;
        //Exact source disposal44,48,A0,A4,A8,B0; no piece/body ownership.
        //Prewarmed backing disposal does not claim guest malloc results.
        //Registered members require the storage caller's same-owner source
        //history validation; attached members are never admitted here.
        if(bound_)for(std::size_t i=0;i<allocated_;++i)
            if((!authenticatedNativeTuning&&pieces44_[i].piece)||attachedA0_[i]||breakLatchB0_[i])return false;
        pieces44_.reset();offsets48_.reset();attachedA0_.reset();
        breakRateA4_.reset();breakScaleA8_.reset();breakLatchB0_.reset();
        nativeRetired_=true;return true;
    }
public:
    OriginalWreckGroupArrays50LA()=default;
    OriginalWreckGroupArrays50LA(const OriginalWreckGroupArrays50LA&)=delete;
    OriginalWreckGroupArrays50LA& operator=(const OriginalWreckGroupArrays50LA&)=delete;
    OriginalWreckGroupArrays50LA(OriginalWreckGroupArrays50LA&&)=delete;
    OriginalWreckGroupArrays50LA& operator=(OriginalWreckGroupArrays50LA&&)=delete;
    OriginalWreckGroupArrayResult50LA prepareCold(std::size_t actualNonprimaryCount)noexcept {
        using S=OriginalWreckGroupArrayStatus50LA;
        // Same authored512 total-piece admission as the actual inventory.
        if(freshSelected_||nativeRetired_||cold_||actualNonprimaryCount>511u)return {S::binding,0x2ca030u};
        try {
            auto p=std::make_unique<OriginalWreckGroupPiece50LA[]>(actualNonprimaryCount);
            auto o=std::make_unique<SpringVector4_48N[]>(actualNonprimaryCount);
            auto a=std::make_unique<std::uint32_t[]>(actualNonprimaryCount);
            auto r=std::make_unique<float[]>(actualNonprimaryCount);
            auto s=std::make_unique<float[]>(actualNonprimaryCount);
            auto w=std::make_unique<std::uint32_t[]>(actualNonprimaryCount);
            pieces44_=std::move(p);offsets48_=std::move(o);attachedA0_=std::move(a);
            breakRateA4_=std::move(r);breakScaleA8_=std::move(s);breakLatchB0_=std::move(w);
            allocated_=actualNonprimaryCount;cold_=true;return {S::coldBacking,0u};
        }catch(const std::bad_alloc&){return {S::allocation,0x2ca050u};}
    }
    // Reached base2C9E40 represented fields only, after actual141300 has
    // constructed the same derived group component.38 is source-unwritten.
    OriginalWreckGroupArrayResult50LA initializeFields2C9E40(OriginalWreckComponent50LA& component,
        const SpringVector4_48N& actualVector4441F0)noexcept {
        using S=OriginalWreckGroupArrayStatus50LA;
        if(freshSelected_||nativeRetired_||!cold_||fields_||component.kind!=OriginalWreckComponentKind50LA::group2CBFC0||
           !component.reference00||!component.objectKey||!component.sessionGeneration)return {S::binding,0x2c9e40u};
        component_=&component;registered30_=capacity34_=0;primary_={};mass4C_=0.f;
        center50_=actualVector4441F0;inertia60_=original_wreck_physical_detail50LA::identity();
        thresholdAC_=original_wreck_physical_detail50LA::word(0x3ca3d70au);
        fields_=true;return {S::fieldsInitialized,0x2c9f28u};
    }
    OriginalWreckGroupArrayResult50LA bindPrimary2CA020(const OriginalWreckGroupPiece50LA& primary)noexcept {
        using S=OriginalWreckGroupArrayStatus50LA;
        if(freshSelected_||nativeRetired_||!fields_||bound_||!pieceBound(primary,component_->sessionGeneration)||
           primary.component==component_||primary.component->objectKey==component_->objectKey)
            return {S::binding,0x2ca020u};
        capacity34_=static_cast<std::uint32_t>(allocated_);primary_=primary; //2CA030..40
        // Array48 element constructor1048E0 stores vf0=(0,0,0,1).
        //2CA0D8..100 explicitly initializes44,A0,B0; A4/A8 await2CA190.
        for(std::size_t i=0;i<allocated_;++i)offsets48_[i]={{0.f,0.f,0.f,1.f}};
        for(std::size_t i=0;i<allocated_;++i){pieces44_[i]={};attachedA0_[i]=0;breakLatchB0_[i]=0;}
        primary.piece->wake30=1;primary.component->mask20=0; //2CA128/130
        mass4C_=primary.physical->parameters.body.mass;inertia60_=primary.physical->inertia100;
        for(unsigned i=0;i<3;++i)center50_.v[i]=primary.physical->parameters.centerOffset.v[i];
        center50_.v[3]=primary.physical->parameters.centerOffsetW;
        bound_=true;return {S::primaryBound,0x2ca174u}; //141C20 is a no-op returning self
    }
    // Exact2CA190: primary is skipped; existing member gets only A4/A8
    // updates; otherwise first null44 slot is filled. No attachment/exclusion
    // or aggregate recomputation occurs here. A full array is an explicit
    // native report of the source no-write return, not a missing-piece success.
    OriginalWreckGroupArrayResult50LA registerMember2CA190(const OriginalWreckGroupPiece50LA& member,
        float breakRate,float breakScale)noexcept {
        using S=OriginalWreckGroupArrayStatus50LA;
        if(freshSelected_||nativeRetired_||!bound_||!pieceBound(member,component_->sessionGeneration))return {S::binding,0x2ca190u};
        if(member.piece==primary_.piece){
            if(member.physical!=primary_.physical||member.component!=primary_.component)return {S::binding,0x2ca194u};
            return {S::primarySkipped,0x2ca2d8u};
        }
        if(member.physical==primary_.physical||member.component==primary_.component||
           member.component->objectKey==primary_.component->objectKey||member.component==component_||
           member.component->objectKey==component_->objectKey)return {S::binding,0x2ca190u};
        std::size_t free=allocated_;
        for(std::size_t i=0;i<allocated_;++i){
            const auto& p=pieces44_[i];
            if(p.piece==member.piece){
                if(p.physical!=member.physical||p.component!=member.component)return {S::binding,0x2ca1b8u,i};
                std::memcpy(&breakRateA4_[i],&breakRate,4);std::memcpy(&breakScaleA8_[i],&breakScale,4);
                return {S::memberStored,0x2ca1dcu,i};
            }
            if(p.piece&&(p.physical==member.physical||p.component==member.component||
               p.component->objectKey==member.component->objectKey))return {S::binding,0x2ca1b8u,i};
            if(!p.piece&&free==allocated_)free=i;
        }
        if(free==allocated_)return {S::full,0x2ca2d8u};
        // Native numerical admission before publication. Within admitted
        // normal-or-zero operands, use separate binary32 subtractions exactly.
        if(std::fegetround()!=FE_TONEAREST)return {S::arithmetic,0x2ca268u,free};
        SpringVector4_48N offset{{0.f,0.f,0.f,1.f}};
        for(unsigned i=0;i<3;++i){
            const float a=member.piece->authored50.v[i],b=primary_.piece->authored50.v[i];
            if(!std::isfinite(a)||!std::isfinite(b)||(a!=0.f&&!std::isnormal(a))||
               (b!=0.f&&!std::isnormal(b)))return {S::arithmetic,0x2ca268u,free};
            volatile float v=a-b;
            if(!std::isfinite(v)||(v!=0.f&&!std::isnormal(v)))return {S::arithmetic,0x2ca268u,free};
            offset.v[i]=v;
        }
        pieces44_[free]=member;
        std::memcpy(&breakRateA4_[free],&breakRate,4);std::memcpy(&breakScaleA8_[free],&breakScale,4);
        offsets48_[free]=offset;++registered30_; //2CA2A0/2CA2B0
        return {S::memberStored,0x2ca2d8u,free};
    }
    // Exact295C80 leaf reached by2CA480 and2CA2F0: ONLY DC.30 changes.
    // Caller must preserve preceding body reset/disable and following exclusion
    // ordering. This is not group activation or permission to run response.
    OriginalWreckGroupArrayResult50LA bindMemberContactBody295C80(std::size_t slot,
        OriginalWreckContactBodyDestination50LA destination)noexcept {
        using S=OriginalWreckGroupArrayStatus50LA;
        if(freshSelected_||nativeRetired_||!bound_||slot>=allocated_||
           (destination!=OriginalWreckContactBodyDestination50LA::primaryBody&&
            destination!=OriginalWreckContactBodyDestination50LA::ownBody))return {S::binding,0x295c80u,slot};
        const auto& p=pieces44_[slot];
        if(!pieceBound(p,component_->sessionGeneration)||
           !pieceBound(primary_,component_->sessionGeneration)||!p.physical->contactConstructorFieldsApplied||
           (p.physical->contactBody30!=&p.physical->body&&
            p.physical->contactBody30!=&primary_.physical->body))return {S::binding,0x295c80u,slot};
        p.physical->contactBody30=destination==OriginalWreckContactBodyDestination50LA::primaryBody?
            &primary_.physical->body:&p.physical->body;
        return {S::contactBindingStored,0x295c84u,slot};
    }
    //22C5B0..22C5F0 follows the ENTIRE ascending collection tuning loop.
    // Copy only root mass and full inertia, preserving saved center50.
    OriginalWreckGroupArrayResult50LA refreshSavedRoot22C5B0()noexcept {
        using S=OriginalWreckGroupArrayStatus50LA;
        if(freshSelected_||nativeRetired_||!bound_||registered30_!=capacity34_||!pieceBound(primary_,component_->sessionGeneration))
            return {S::binding,0x22c5b0u};
        for(std::size_t i=0;i<allocated_;++i)if(!pieces44_[i].piece||attachedA0_[i])
            return {S::binding,0x22c5b0u,i};
        mass4C_=primary_.physical->parameters.body.mass;inertia60_=primary_.physical->inertia100;
        return {S::rootRefreshed,0x22c5f0u};
    }
    const OriginalWreckGroupPiece50LA* primary()const noexcept{return !nativeRetired_&&bound_?&primary_:nullptr;}
    bool coldBackingPrepared()const noexcept{return !nativeRetired_&&cold_;}
    bool primaryFieldsBound()const noexcept{return !nativeRetired_&&bound_;}
    std::size_t allocationCapacity()const noexcept{return allocated_;}
    std::uint32_t registeredMemberCount30()const noexcept{return registered30_;}
    // A populated44 slot makes its48/A4/A8 payload valid. A0 andB0 are
    // initialized by2CA020 but never made active by this bounded owner.
    const OriginalWreckGroupPiece50LA* member(std::size_t i)const noexcept {
        return !nativeRetired_&&bound_&&i<allocated_&&pieces44_[i].piece?&pieces44_[i]:nullptr;
    }
    const SpringVector4_48N* relativeOffset48(std::size_t i)const noexcept{return member(i)?&offsets48_[i]:nullptr;}
    const float* breakRateA4(std::size_t i)const noexcept{return member(i)?&breakRateA4_[i]:nullptr;}
    const float* breakScaleA8(std::size_t i)const noexcept{return member(i)?&breakScaleA8_[i]:nullptr;}
};
} // namespace rr::game
