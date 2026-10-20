#pragma once
// Explicit native ownership policy for the original3DF5B8 lifetime seam.
// Original124580 leaves a returned stack binding. This port restores only the
// borrowed outer binding at the actual nested-result lifetime end. It does NOT
// claim original memory parity or restore flags/records/caches/physical state.
#include "static_type2_producer49x.h"
#include "reset_tree50ak.h"
#include <cstdint>
#include <limits>
#include <exception>

namespace rr::game {
struct SourceQueryIdentity50MB {
    const void* issuer=nullptr;
    std::uint64_t generation=0,serial=0;
    std::size_t rosterSlot=0;
    std::uint32_t queryCollider=0;
};
inline bool sameSourceQueryIdentity50MB(const SourceQueryIdentity50MB& a,
    const SourceQueryIdentity50MB& b) noexcept {
    return a.issuer==b.issuer&&a.generation==b.generation&&a.serial==b.serial&&
        a.rosterSlot==b.rosterSlot&&a.queryCollider==b.queryCollider;
}
class CollisionResultBindingOwner50MB;
class SourceOuterResultLease50MB final {
    friend class CollisionResultBindingOwner50MB;
    CollisionResultBindingOwner50MB* owner_=nullptr;
    SourceQueryIdentity50MB identity_{};
public:
    SourceOuterResultLease50MB() noexcept=default;
    ~SourceOuterResultLease50MB();
    SourceOuterResultLease50MB(const SourceOuterResultLease50MB&)=delete;
    SourceOuterResultLease50MB& operator=(const SourceOuterResultLease50MB&)=delete;
    SourceOuterResultLease50MB(SourceOuterResultLease50MB&&)=delete;
    SourceOuterResultLease50MB& operator=(SourceOuterResultLease50MB&&)=delete;
};
class SourceGroundResultLease50MB final {
    friend class CollisionResultBindingOwner50MB;
    CollisionResultBindingOwner50MB* owner_=nullptr;
    const SourceOuterResultLease50MB* parent_=nullptr;
    ResetTreeResult50AK* result_=nullptr;
public:
    SourceGroundResultLease50MB() noexcept=default;
    ~SourceGroundResultLease50MB();
    SourceGroundResultLease50MB(const SourceGroundResultLease50MB&)=delete;
    SourceGroundResultLease50MB& operator=(const SourceGroundResultLease50MB&)=delete;
    SourceGroundResultLease50MB(SourceGroundResultLease50MB&&)=delete;
    SourceGroundResultLease50MB& operator=(SourceGroundResultLease50MB&&)=delete;
};
class CollisionResultBindingOwner50MB final {
    friend class SourceOuterResultLease50MB;
    friend class SourceGroundResultLease50MB;
    SourceOuterResultLease50MB* outer_=nullptr;
    SourceGroundResultLease50MB* child_=nullptr;
    const Type2StaticQuery49X* query_=nullptr;
    Type2StaticResult49X* result_=nullptr;
    SpringCumulativePair48N* pairs_=nullptr;
    Type2PairMetadata49X* metadata_=nullptr;
    std::size_t pairSlots_=0;
    struct BorrowedSpan {const void* data=nullptr;std::size_t bytes=0;};
    BorrowedSpan scratch_[12]{};bool scratchBound_=false;
    bool valid_=true;
    std::uint64_t nestedBindings_=0,nestedRestorations_=0;
    static bool disjoint(const void* a,std::size_t an,const void* b,std::size_t bn) noexcept {
        if(!an||!bn)return true;if(!a||!b)return false;
        const auto aa=reinterpret_cast<std::uintptr_t>(a),bb=reinterpret_cast<std::uintptr_t>(b);
        if(an>std::numeric_limits<std::uintptr_t>::max()-aa||bn>std::numeric_limits<std::uintptr_t>::max()-bb)return false;
        return aa+an<=bb||bb+bn<=aa;
    }
public:
    CollisionResultBindingOwner50MB() noexcept=default;
    ~CollisionResultBindingOwner50MB(){if(borrowed())std::terminate();}
    CollisionResultBindingOwner50MB(const CollisionResultBindingOwner50MB&)=delete;
    CollisionResultBindingOwner50MB& operator=(const CollisionResultBindingOwner50MB&)=delete;
    // Parent authenticates this identity against its actual Bank reservation.
    // This operation binds existing storage; it never initializes/copies it.
    bool beginOuter(const SourceQueryIdentity50MB& identity,const Type2StaticQuery49X& query,
        Type2StaticResult49X& result,SpringCumulativePair48N* pairs,
        Type2PairMetadata49X* metadata,std::size_t slots,SourceOuterResultLease50MB& lease) noexcept {
        if(!valid_||outer_||child_||lease.owner_||!identity.issuer||!identity.generation||!identity.serial||
           !identity.queryCollider||!query.filter||query.filter->identity!=identity.queryCollider||
           !query.resource||!slots||slots<query.resource->rayCount||!pairs||!metadata||
           slots>std::numeric_limits<std::size_t>::max()/sizeof(*pairs)||
           slots>std::numeric_limits<std::size_t>::max()/sizeof(*metadata))return false;
        if(!disjoint(&result,sizeof result,pairs,slots*sizeof(*pairs))||
           !disjoint(&result,sizeof result,metadata,slots*sizeof(*metadata))||
           !disjoint(pairs,slots*sizeof(*pairs),metadata,slots*sizeof(*metadata)))return false;
        outer_=&lease;query_=&query;result_=&result;pairs_=pairs;metadata_=metadata;pairSlots_=slots;
        lease.owner_=this;lease.identity_=identity;return true;
    }
    bool outerCurrent(const SourceOuterResultLease50MB& lease,
        const SourceQueryIdentity50MB& identity,const Type2StaticQuery49X& query,
        const Type2StaticResult49X& result,const SpringCumulativePair48N* pairs,
        const Type2PairMetadata49X* metadata,std::size_t slots) const noexcept {
        return valid_&&outer_==&lease&&lease.owner_==this&&!child_&&
            sameSourceQueryIdentity50MB(lease.identity_,identity)&&query_==&query&&
            result_==&result&&pairs_==pairs&&metadata_==metadata&&pairSlots_==slots&&
            query.filter&&query.filter->identity==identity.queryCollider;
    }
    bool outerCurrent(const SourceOuterResultLease50MB& lease) const noexcept {
        return valid_&&outer_==&lease&&lease.owner_==this&&!child_;
    }
    bool outerQueryCurrent(const SourceOuterResultLease50MB& lease,const Type2StaticQuery49X& actual) const noexcept {
        return outerCurrent(lease)&&query_==&actual&&actual.filter&&
            actual.filter->identity==lease.identity_.queryCollider;
    }
    // Exact declared outer numerical storage and retained source list. These
    // twelve descriptors are not a nesting stack or a capacity enlargement.
    bool bindOuterScratch(const SourceOuterResultLease50MB& lease,const Type2StaticWorkspace49X& w,
        const Type2StaticDestination49X& d,const std::size_t* indices,std::size_t count,
        const SpringRecord48N* records,std::size_t recordSlots) noexcept {
        if(!outerCurrent(lease))return false;
        BorrowedSpan spans[12]{};std::size_t used=0;
        const auto add=[&](const auto* ptr,std::size_t n) noexcept {
            if(used==12||n>std::numeric_limits<std::size_t>::max()/sizeof(*ptr)||(n&&!ptr))return false;
            spans[used++]={ptr,n*sizeof(*ptr)};return true;
        };
        if(!add(w.pairs,w.pairCapacity)||!add(w.metadata,w.metadataCapacity)||!add(w.records,w.recordCapacity)||
           !add(w.effects,w.effectCapacity)||!add(w.stack,w.stackCapacity)||!add(w.trace,w.traceCapacity)||
           !add(d.pairs,d.pairCapacity)||!add(d.metadata,d.metadataCapacity)||!add(d.records,d.recordCapacity)||
           !add(d.effects,d.effectCapacity)||!add(indices,count)||!add(records,recordSlots))return false;
        if(scratchBound_){for(std::size_t n=0;n<12;++n)if(scratch_[n].data!=spans[n].data||scratch_[n].bytes!=spans[n].bytes)return false;}
        else {for(std::size_t n=0;n<12;++n)scratch_[n]=spans[n];scratchBound_=true;}
        return true;
    }
    bool disjointOuterStorage(const void* data,std::size_t bytes) const noexcept {
        if(!valid_||!outer_||!scratchBound_||!query_||!result_||
           !disjoint(data,bytes,query_,sizeof(*query_))||!disjoint(data,bytes,result_,sizeof(*result_))||
           !disjoint(data,bytes,pairs_,pairSlots_*sizeof(*pairs_))||
           !disjoint(data,bytes,metadata_,pairSlots_*sizeof(*metadata_)))return false;
        for(const auto& span:scratch_)if(!disjoint(data,bytes,span.data,span.bytes))return false;
        return true;
    }
    // Exactly one child: admitted ground collection/narrowphase has no E8 or
    // callbacks. Multiple candidates and twenty attempts are sequential.
    bool beginGround(const SourceOuterResultLease50MB& parent,ResetTreeResult50AK& actual,
        SourceGroundResultLease50MB& lease) noexcept {
        if(!outerCurrent(parent)||lease.owner_||!result_||!pairs_||!metadata_||
           !disjoint(&actual,sizeof actual,result_,sizeof(*result_))||
           !disjoint(&actual,sizeof actual,pairs_,pairSlots_*sizeof(*pairs_))||
           !disjoint(&actual,sizeof actual,metadata_,pairSlots_*sizeof(*metadata_))||
           !disjoint(&actual,sizeof actual,this,sizeof(*this))||
           !disjoint(&actual,sizeof actual,&parent,sizeof(parent))||
           !disjoint(&actual,sizeof actual,&lease,sizeof(lease)))return false;
        lease.owner_=this;lease.parent_=&parent;lease.result_=&actual;child_=&lease;
        if(nestedBindings_!=std::numeric_limits<std::uint64_t>::max())++nestedBindings_;
        return true;
    }
    bool groundCurrent(const SourceGroundResultLease50MB& lease,const ResetTreeResult50AK& actual) const noexcept {
        return valid_&&child_==&lease&&lease.owner_==this&&lease.parent_==outer_&&lease.result_==&actual;
    }
    bool endGround(SourceGroundResultLease50MB& lease) noexcept {
        if(!valid_||child_!=&lease||lease.owner_!=this||lease.parent_!=outer_||!lease.result_){
            valid_=false;return false;
        }
        // Native policy: only the binding changes. Child's actual result,
        // flags, normal mode, records and all other source owners are untouched.
        child_=nullptr;lease.owner_=nullptr;lease.parent_=nullptr;lease.result_=nullptr;
        if(nestedRestorations_!=std::numeric_limits<std::uint64_t>::max())++nestedRestorations_;
        return true;
    }
    bool endOuter(SourceOuterResultLease50MB& lease) noexcept {
        if(child_||outer_!=&lease||lease.owner_!=this){valid_=false;return false;}
        outer_=nullptr;lease.owner_=nullptr;lease.identity_={};
        // Inactive identities are not dereferenceable. No original global
        // reset-to-null store is claimed by this native borrow release.
        query_=nullptr;result_=nullptr;pairs_=nullptr;metadata_=nullptr;pairSlots_=0;scratchBound_=false;
        for(auto& span:scratch_)span={};
        return valid_;
    }
    bool valid() const noexcept{return valid_;}
    bool borrowed() const noexcept{return outer_||child_;}
    bool childBorrowed() const noexcept{return child_!=nullptr;}
    std::uint64_t nestedBindings() const noexcept{return nestedBindings_;}
    std::uint64_t nestedRestorations() const noexcept{return nestedRestorations_;}
};
inline SourceGroundResultLease50MB::~SourceGroundResultLease50MB(){if(owner_)owner_->endGround(*this);}
inline SourceOuterResultLease50MB::~SourceOuterResultLease50MB(){if(owner_)owner_->endOuter(*this);}
} // namespace rr::game
