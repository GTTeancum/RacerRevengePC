#pragma once
#include "original_wreck_physical50la.h"
#include <memory>
#include <cmath>

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
    static bool pieceBound(const OriginalWreckGroupPiece50LA& p,std::uint64_t generation)noexcept {
        return p.piece&&p.physical&&p.component&&p.physical->bodyConstructorFieldsApplied&&
            p.component->kind==OriginalWreckComponentKind50LA::piece2CB4B0&&
            p.component->reference00&&p.component->objectKey&&p.component->sessionGeneration==generation;
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
        if(cold_||actualNonprimaryCount>511u)return {S::binding,0x2ca030u};
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
        if(!cold_||fields_||component.kind!=OriginalWreckComponentKind50LA::group2CBFC0||
           !component.reference00||!component.objectKey||!component.sessionGeneration)return {S::binding,0x2c9e40u};
        component_=&component;registered30_=capacity34_=0;primary_={};mass4C_=0.f;
        center50_=actualVector4441F0;inertia60_=original_wreck_physical_detail50LA::identity();
        thresholdAC_=original_wreck_physical_detail50LA::word(0x3ca3d70au);
        fields_=true;return {S::fieldsInitialized,0x2c9f28u};
    }
    OriginalWreckGroupArrayResult50LA bindPrimary2CA020(const OriginalWreckGroupPiece50LA& primary)noexcept {
        using S=OriginalWreckGroupArrayStatus50LA;
        if(!fields_||bound_||!pieceBound(primary,component_->sessionGeneration)||
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
        if(!bound_||!pieceBound(member,component_->sessionGeneration))return {S::binding,0x2ca190u};
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
        if(!bound_||slot>=allocated_||
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
        if(!bound_||registered30_!=capacity34_||!pieceBound(primary_,component_->sessionGeneration))
            return {S::binding,0x22c5b0u};
        for(std::size_t i=0;i<allocated_;++i)if(!pieces44_[i].piece||attachedA0_[i])
            return {S::binding,0x22c5b0u,i};
        mass4C_=primary_.physical->parameters.body.mass;inertia60_=primary_.physical->inertia100;
        return {S::rootRefreshed,0x22c5f0u};
    }
    const OriginalWreckGroupPiece50LA* primary()const noexcept{return bound_?&primary_:nullptr;}
    bool coldBackingPrepared()const noexcept{return cold_;}
    bool primaryFieldsBound()const noexcept{return bound_;}
    std::size_t allocationCapacity()const noexcept{return allocated_;}
    std::uint32_t registeredMemberCount30()const noexcept{return registered30_;}
    // A populated44 slot makes its48/A4/A8 payload valid. A0 andB0 are
    // initialized by2CA020 but never made active by this bounded owner.
    const OriginalWreckGroupPiece50LA* member(std::size_t i)const noexcept {
        return bound_&&i<allocated_&&pieces44_[i].piece?&pieces44_[i]:nullptr;
    }
    const SpringVector4_48N* relativeOffset48(std::size_t i)const noexcept{return member(i)?&offsets48_[i]:nullptr;}
    const float* breakRateA4(std::size_t i)const noexcept{return member(i)?&breakRateA4_[i]:nullptr;}
    const float* breakScaleA8(std::size_t i)const noexcept{return member(i)?&breakScaleA8_[i]:nullptr;}
};
} // namespace rr::game
