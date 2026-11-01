#pragma once
#include "original_collision_base_owner50la.h"
#include "trigger_sphere50in.h"
#include "original_source_collision_observer50la.h"
#include "original_collision_membership_guards50la.h"
#include "original_source_portal_overlap231150.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <memory>
#include <new>
namespace ra_pod_scene50LA {class Owner;}
namespace rr::game {
// One actual 2A0290 source object. This is not a ready TriggerActions owner.
// Only the World source descriptor owns it. All source aliases are separate
// from native allocation custody, including allocations not yet published.
class OriginalWorldTriggerInstance50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    using C=OriginalWreckComponent50LA;
    using B=OriginalCollisionBaseOwner50LA;
    using S=OriginalWreckComponentStatus50LA;
    C entity_;
    C* serviceParent_=nullptr;
    bool serviceRegistrationAttempted_=false,serviceRegistered_=false,serviceTopologyWithdrawn_=false;
    OriginalApplicationEntities50LA* application_=nullptr;
    bool counted_=false,enrolled_=false,colliderEnrolled_=false;
    bool constructed_=false,setupAttempted_=false,setupReturned_=false,nativeClosed_=false;
    std::uint32_t pc_=0,vtable08_=0;
    SpringVector4_48N position30_{},box40_{};
    BodyMatrix48G matrix50_{};
    std::uint8_t flags1B8_=0,flagsKnown_=0;
    std::optional<std::uint32_t> random90_,capacity94_,count98_,field1A8_,field1B4_;
    std::optional<float> radius1AC_;
    std::array<std::optional<char>,260> parentA4_{}; //Unknown until actual2DC068 strcpy.
    bool oneTimeWritten_=false,parentWritten_=false;
    std::unique_ptr<const void*[]> callbacks9C_,argumentsA0_;
    std::optional<const void*> source9C_,sourceA0_;
    std::unique_ptr<B> collider_;
    std::optional<B*> source1B0_; // Constructor does not write 1B0.
    struct Shape {
        TriggerSphereShape50IN value;
        std::uint8_t known=0;
        std::optional<const void*> owner60;
    };
    struct Result {
        std::optional<std::uint32_t> capacity00,count04;
        // Source never writes 08. Native zero backing is not knowledge.
        std::optional<float> distance08;
        SpringVector4_48N center20{};
        std::array<SpringVector4_48N,3> normals30{},points60{};
    };
    std::unique_ptr<Shape> shape_;
    std::unique_ptr<Result> result_;
    bool shapeAttempted_=false,shapeReturned_=false,refreshReturned_=false;
    bool colliderAllocationReturned_=false,shapeAllocationReturned_=false,resultAllocationReturned_=false;
    bool callbacksAllocationReturned_=false,argumentsAllocationReturned_=false;
    // Code-address identity, not a cast host callback or execution receipt.
    std::optional<std::uint32_t> callbackE8PC_;
    // Distinct 2DC2A4 -> 2A0A80 occurrence. No membership, callback,
    // registration or 2A0A80 return is implied by the geometry prefix.
    bool serviceEntered_=false,serviceMatrixCopied_=false,serviceCenterStored_=false;
    bool serviceRadiusStored_=false,serviceGeometryReturned_=false;
    std::optional<std::uint32_t> serviceAttachment1A8_;
    std::optional<BodyMatrix48G> serviceMatrix50_;
    bool refreshService2A0A80(std::uint32_t& pc)noexcept {
        if(serviceEntered_||!setupReturned_||!preflightNative())return false;
        serviceEntered_=true;pc=pc_=0x2a0a94;
        if(!field1A8_)return false;serviceAttachment1A8_=*field1A8_;
        pc=pc_=0x2a0a98;
        if(*serviceAttachment1A8_){pc=pc_=0x2a0aa0;return false;} //Actual model branch unjoined.
        pc=pc_=0x2a0b00;
        if(!source1B0_||*source1B0_!=collider_.get()||!collider_)return false;
        pc=pc_=0x2a0b04;
        // 2A0B08 passes this actual, NONNULL retained matrix, unlike setup.
        serviceMatrix50_=matrix50_;pc=pc_=0x11f680;
        auto& b=*collider_;
        if(b.shape88_!=5||!shape_||shape_->known!=63||!b.shape8C_||*b.shape8C_!=shape_.get())return false;
        pc=pc_=0x11fcc8;
        for(unsigned pair=0;pair<8;++pair){
            std::uint32_t words[2];std::memcpy(words,matrix50_.v+2*pair,sizeof words);
            pc=pc_=0x11fce0;std::memcpy(shape_->value.world.v+2*pair,words,4);
            pc=pc_=0x11fce4;std::memcpy(shape_->value.world.v+2*pair+1,words+1,4);
        }
        serviceMatrixCopied_=true;
        pc=pc_=0x11fd00;b.cachedCenter60_=shape_->value.localCenter;serviceCenterStored_=true;
        pc=pc_=0x11fd0c;b.radius80_=shape_->value.radius50;serviceRadiusStored_=true;
        pc=pc_=0x11fd2c;
        // Bounded identity VU adaptation, not a general matrix implementation.
        // All source copies/stores above survive an unsupported transform.
        BodyMatrix48G id;identity(id);
        if(std::memcmp(matrix50_.v,id.v,sizeof id.v))return false;
        for(unsigned k=0;k<3;++k)if(!std::isfinite(b.cachedCenter60_.v[k])||!(b.cachedCenter60_.v[k]>0.f))return false;
        if(std::bit_cast<std::uint32_t>(b.cachedCenter60_.v[3])!=0x3f800000u)return false;
        pc=pc_=0x11fd44; //Exact identity result retains the already copied center.
        pc=pc_=0x11fe40;serviceGeometryReturned_=true;
        pc=pc_=0x2a0b0c;return true; //Geometry only; 120070 mode1 has NOT run.
    }
    using Membership=OriginalSourceCollisionObserver50LA;
    std::unique_ptr<Membership> serviceStackMembership_,serviceHeapMembership_;
    bool membershipEntered_=false,membershipStackReturned_=false,membershipHeapAllocationReturned_=false;
    bool membershipHeapReturned_=false,membershipE0Published_=false,membershipNativeWithdrawn_=false;
    std::optional<std::uint8_t> membershipFlagsF8_;
    bool membershipOldE0NullRead_=false;
    bool beginServiceMembership120070(OriginalGarageObserverShared50KG& shared,std::uint32_t& pc)noexcept {
        if(!serviceGeometryReturned_||membershipEntered_||!preflightNative())return false;
        pc=pc_=0x120070;
        // Native backing for actual callee stack50, not a guest allocation.
        try{serviceStackMembership_.reset(new Membership);}catch(...){return false;}
        membershipEntered_=true;pc=pc_=0x120094;
        if(!serviceStackMembership_->construct280890(shared)){pc=pc_=serviceStackMembership_->pc();return false;}
        membershipStackReturned_=true;pc=pc_=0x12009c;
        membershipFlagsF8_=collider_->flagsF8_;
        pc=pc_=0x1200a8;
        if(*membershipFlagsF8_&8u){pc=pc_=0x1200b4;return false;} //Temporary-list branch unjoined.
        pc=pc_=0x1200b8;
        if(!collider_->membershipE0_||*collider_->membershipE0_)return false;
        membershipOldE0NullRead_=true;pc=pc_=0x1200d0;
        // Native backing for the original nullable30-byte guest allocation.
        try{serviceHeapMembership_.reset(new Membership);}catch(...){serviceHeapMembership_.reset();}
        membershipHeapAllocationReturned_=true;
        if(serviceHeapMembership_){pc=pc_=0x1200e0;
            if(!serviceHeapMembership_->construct280890(shared)){pc=pc_=serviceHeapMembership_->pc();return false;}
            membershipHeapReturned_=true;
        }
        pc=pc_=0x1200e8;
        collider_->membershipE0_=serviceHeapMembership_?serviceHeapMembership_->identity():nullptr;
        membershipE0Published_=true;pc=pc_=0x1200f0;
        return true; //Only allocation/publication prefix, NOT120070 return.
    }
    bool withdrawServiceMembershipNative()noexcept {
        if(membershipNativeWithdrawn_)return true;
        if(!preflightNative())return false;
        if(!withdrawServicePrimaryNative())return false;
        // Preserve source E0 history/backing until instance disposal. Native
        // withdrawal does not fabricate a source NULL store or280920 return.
        if(serviceHeapMembership_&&(!(serviceHeapMembership_->primary_.entered?
           serviceHeapMembership_->canWithdrawActiveRegistrationNative():serviceHeapMembership_->canWithdrawUnregisteredNative())||
           !serviceHeapMembership_->withdrawNative()))return false;
        if(serviceStackMembership_&&(!serviceStackMembership_->canWithdrawUnregisteredNative()||
           !serviceStackMembership_->withdrawNative()))return false;
        membershipNativeWithdrawn_=true;return true;
    }
    OriginalCollisionMembershipGuards50LA::History membershipAllocationGuard_;
    bool boundsScratchCenter_=false,boundsScratchHalf_=false,boundsEntered_=false,boundsReturned_=false;
    // Native fingerprint only, not a claim that all source loads occurred.
    std::optional<TriggerSphereShape50IN> boundsNativeShape_;
    std::array<std::optional<std::uint32_t>,3> boundsProducts_;
    std::uint8_t boundsLocalCenterStores_=0,boundsLocalHalfStores_=0,boundsHalfStores_=0;
    bool boundsCenterStored_=false;
    std::optional<std::uint32_t> boundsRadiusProduct_;
    std::uint8_t boundsMatrixWords_=0;
    BodyMatrix48G boundsMatrix_{};
    SpringVector4_48N boundsLocalCenter_{{0,0,0,1}},boundsLocalHalf_{{0,0,0,1}};
    SpringVector4_48N boundsCenter_{},boundsHalf_{};
    static bool positiveNormalBits(std::uint32_t b)noexcept {
        const auto exponent=b&0x7f800000u;return !(b&0x80000000u)&&exponent&&exponent!=0x7f800000u;
    }
    bool continueServiceBounds1200F0(std::uint32_t& pc)noexcept {
        if(!membershipE0Published_||membershipAllocationGuard_.entered||membershipNativeWithdrawn_||!preflightNative())return false;
        if(!OriginalCollisionMembershipGuards50LA::beginAllocation(membershipAllocationGuard_)){
            pc=pc_=membershipAllocationGuard_.pc;return false;}
        if(!OriginalCollisionMembershipGuards50LA::finishAllocation(membershipAllocationGuard_,bool(serviceHeapMembership_))){
            pc=pc_=membershipAllocationGuard_.pc;return false;}
        pc=pc_=0x12015c;boundsCenter_={{0,0,0,1}};boundsScratchCenter_=true;
        pc=pc_=0x120164;boundsHalf_={{0,0,0,1}};boundsScratchHalf_=true;
        pc=pc_=0x1202e0;
        if(!shape_||shape_->known!=63||!collider_||collider_->shape88_!=5||
           !collider_->shape8C_||*collider_->shape8C_!=shape_.get())return false;
        boundsEntered_=true;boundsNativeShape_=shape_->value;
        const auto& v=shape_->value;pc=pc_=0x1208f4;
        const auto scale=std::bit_cast<std::uint32_t>(v.centerScale5C);
        constexpr std::uint32_t multiplyPC[3]={0x120904,0x120908,0x120910};
        // Unit-scale, positive-normal exact subset; no host arithmetic or
        // denormal/overflow assumptions are used to supply original results.
        for(unsigned axis=0;axis<3;++axis){pc=pc_=multiplyPC[axis];
            const auto bits=std::bit_cast<std::uint32_t>(v.localCenter.v[axis]);
            if(scale!=0x3f800000u||!positiveNormalBits(bits))return false;
            boundsProducts_[axis]=bits;
            if(axis==1){pc=pc_=0x12090c;boundsLocalCenter_.v[0]=std::bit_cast<float>(*boundsProducts_[0]);boundsLocalCenterStores_|=1;}
        }
        pc=pc_=0x120914;boundsLocalCenter_.v[1]=std::bit_cast<float>(*boundsProducts_[1]);boundsLocalCenterStores_|=2;
        pc=pc_=0x120918;boundsLocalCenter_.v[2]=std::bit_cast<float>(*boundsProducts_[2]);boundsLocalCenterStores_|=4;
        pc=pc_=0x12091c;boundsLocalCenter_.v[3]=1;
        pc=pc_=0x120944;
        const auto radius=std::bit_cast<std::uint32_t>(v.radius50);
        if(std::bit_cast<std::uint32_t>(v.radiusScale58)!=0x3f800000u||!positiveNormalBits(radius))return false;
        boundsRadiusProduct_=radius;
        // 120954/958/95C multiply this product by source(1,1,1).
        constexpr std::uint32_t halfPC[3]={0x120960,0x120964,0x120968};
        for(unsigned axis=0;axis<3;++axis){pc=pc_=halfPC[axis];boundsLocalHalf_.v[axis]=std::bit_cast<float>(radius);boundsLocalHalfStores_|=1u<<axis;}
        pc=pc_=0x12096c;boundsLocalHalf_.v[3]=1;
        for(unsigned i=0;i<16;++i){pc=pc_=(i&1)?0x120990:0x12098c;
            std::memcpy(boundsMatrix_.v+i,v.world.v+i,4);++boundsMatrixWords_;}
        pc=pc_=0x120cc4;BodyMatrix48G id;identity(id);
        if(std::memcmp(boundsMatrix_.v,id.v,sizeof id.v))return false;
        pc=pc_=0x120cd4;boundsCenter_=boundsLocalCenter_;boundsCenterStored_=true;
        // Identity axes retain the radius; abs+zero sums are exact in this
        // positive-normal domain. Source writes only XYZ; caller's W survives.
        constexpr std::uint32_t outPC[3]={0x120dbc,0x120e04,0x120e48};
        for(unsigned axis=0;axis<3;++axis){pc=pc_=outPC[axis];boundsHalf_.v[axis]=boundsLocalHalf_.v[axis];boundsHalfStores_|=1u<<axis;}
        pc=pc_=0x120e64;boundsReturned_=true;pc=pc_=0x120188;return true;
    }
    bool serviceBoundsCurrentNative()const noexcept {
        if(!OriginalCollisionMembershipGuards50LA::allocationHistory(membershipAllocationGuard_)||
           (membershipAllocationGuard_.entered&&!membershipE0Published_)||
           (boundsScratchCenter_&&!membershipAllocationGuard_.returned)||(boundsScratchHalf_&&!boundsScratchCenter_)||
           (boundsEntered_&&(!boundsScratchHalf_||!boundsNativeShape_))||
           (boundsNativeShape_&&!boundsEntered_)||boundsLocalCenterStores_>7||boundsLocalHalfStores_>7||boundsHalfStores_>7||
           (boundsLocalCenterStores_&(boundsLocalCenterStores_+1u))||
           (boundsLocalHalfStores_&(boundsLocalHalfStores_+1u))||(boundsHalfStores_&(boundsHalfStores_+1u))||
           (boundsProducts_[1]&&!boundsProducts_[0])||(boundsProducts_[2]&&!boundsProducts_[1])||
           (boundsRadiusProduct_&&boundsLocalCenterStores_!=7)||(boundsLocalHalfStores_&&!boundsRadiusProduct_)||
           boundsMatrixWords_>16||(boundsMatrixWords_&&boundsLocalHalfStores_!=7)||
           (boundsCenterStored_&&boundsMatrixWords_!=16)||(boundsHalfStores_&&!boundsCenterStored_)||
           (boundsReturned_&&boundsHalfStores_!=7))return false;
        if(boundsNativeShape_){
            if(!shape_)return false;const auto& old=*boundsNativeShape_;const auto& now=shape_->value;
            if(std::memcmp(old.localCenter.v,now.localCenter.v,sizeof old.localCenter.v)||
               std::memcmp(old.world.v,now.world.v,sizeof old.world.v)||
               std::bit_cast<std::uint32_t>(old.radius50)!=std::bit_cast<std::uint32_t>(now.radius50)||
               std::bit_cast<std::uint32_t>(old.radiusScale58)!=std::bit_cast<std::uint32_t>(now.radiusScale58)||
               std::bit_cast<std::uint32_t>(old.centerScale5C)!=std::bit_cast<std::uint32_t>(now.centerScale5C))return false;
            for(unsigned k=0;k<3;++k){
                if(boundsProducts_[k]&&*boundsProducts_[k]!=std::bit_cast<std::uint32_t>(old.localCenter.v[k]))return false;
                if((boundsLocalCenterStores_&(1u<<k))&&(!boundsProducts_[k]||
                   std::bit_cast<std::uint32_t>(boundsLocalCenter_.v[k])!=*boundsProducts_[k]))return false;
                if((boundsLocalHalfStores_&(1u<<k))&&(!boundsRadiusProduct_||
                   std::bit_cast<std::uint32_t>(boundsLocalHalf_.v[k])!=*boundsRadiusProduct_))return false;
                if(boundsScratchCenter_&&std::bit_cast<std::uint32_t>(boundsCenter_.v[k])!=
                   (boundsCenterStored_?std::bit_cast<std::uint32_t>(boundsLocalCenter_.v[k]):0u))return false;
                if(boundsScratchHalf_&&std::bit_cast<std::uint32_t>(boundsHalf_.v[k])!=
                   ((boundsHalfStores_&(1u<<k))?std::bit_cast<std::uint32_t>(boundsLocalHalf_.v[k]):0u))return false;
            }
            if(boundsRadiusProduct_&&*boundsRadiusProduct_!=std::bit_cast<std::uint32_t>(old.radius50))return false;
            for(unsigned k=0;k<boundsMatrixWords_;++k)if(std::bit_cast<std::uint32_t>(boundsMatrix_.v[k])!=std::bit_cast<std::uint32_t>(old.world.v[k]))return false;
        }
        if((boundsScratchCenter_&&std::bit_cast<std::uint32_t>(boundsCenter_.v[3])!=0x3f800000u)||
           (boundsScratchHalf_&&std::bit_cast<std::uint32_t>(boundsHalf_.v[3])!=0x3f800000u))return false;
        return true;
    }
    struct SourceMembershipPortalEdge {
        const void* edge=nullptr;const void* next=nullptr;
        const OriginalWreckComponent50LA* neighbor=nullptr;const OriginalWreckComponent50LA* portal=nullptr;
        OriginalNativeListMethodCustody50LA* previousMethod=nullptr;
        OriginalNativeListMethodCustody50LA* method=nullptr;
        std::optional<std::uint32_t> slot,distance;
        source_portal_overlap231150::History overlap;
        unsigned edgeIndex=0;std::uint8_t distanceStoreMask=0,linkStoreMask=0;
        bool entered=false,overlapReturned=false,hit=false,methodReturned=false,selectionPending=false;
        bool methodPinned=false,methodPinWithdrawn=false,selectionAttempted=false,selectionReturned=false,selectionCanceled=false;
        bool counterAdvanced=false,inserted=false,slotWithdrawn=false,allocationAttempted=false,allocationRecorded=false,allocationReturned=false;
        bool headPublished=false,linkReleased=false,complete=false;
        std::uint32_t freeHeadBefore=0,lastUsedBefore=0,freeHeadAfter=0,lastUsedAfter=0;
        Membership::SecondaryLink* link=nullptr;const void* oldHead=nullptr;
    };
    struct SourceMembershipPortals280BF0 {
        bool entered=false,helperEntered=false,rootRead=false,returned=false,nativeWithdrawn=false;
        std::uint32_t pc=0;std::size_t count=0;
        const void* root=nullptr;const void* cursor=nullptr;
        OriginalNativeListMethodCustody50LA* cursorMethod=nullptr;
        std::shared_ptr<const void> graphPin;
        std::array<SourceMembershipPortalEdge,20> edges{};
    };
    struct ServicePrimary2809C0 {
        bool entered=false,methodResolved=false,methodPinned=false,methodUnpinned=false;
        bool selectionAttempted=false,selectionReturned=false,selectionPending=false,selectionCanceled=false;
        bool counterAdvanced=false,inserted=false,published=false,slotWithdrawn=false,nativeWithdrawn=false;
        const void* zone=nullptr;
        const ::ra_pod_scene50LA::Owner* world=nullptr;
        OriginalNativeListMethodCustody50LA* method=nullptr;
        std::optional<std::uint32_t> slot;
        std::uint32_t pc=0,freeBefore=0,lastBefore=0,freeAfter=0,lastAfter=0;
        std::array<std::uint32_t,4> center{};
        SourceMembershipPortals280BF0 portals;
    } servicePrimary_;
    template<class ActualMethod>
    bool continueServicePrimary2809C0(const ::ra_pod_scene50LA::Owner* world,const void* zone,
        ActualMethod&& actualMethod,std::uint32_t& pc)noexcept {
        auto& h=servicePrimary_;
        if(!boundsReturned_||!world||!zone||h.entered||membershipNativeWithdrawn_||!serviceHeapMembership_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        h.entered=true;h.world=world;h.zone=zone;at(0x2809c0);
        std::memcpy(h.center.data(),boundsCenter_.v,16);
        auto& observer=*serviceHeapMembership_;
        if(!observer.beginRegistration2809C0(&h,zone,h.center)){at(observer.registrationPC()?observer.registrationPC():0x2809c0u);return false;}
        at(0x27f5a0);
        if(!actualMethod(&h,collider_.get(),h.method,h.slot,h.selectionPending,h.pc,false)){at(h.pc);return false;}
        h.methodResolved=true;at(0x280a80);
        if(h.method){auto& c=*h.method;
            if(c.application_!=application_||c.world_!=world||!c.sourceMode_||c.usage10_!=1||
               c.ordinal_/10u!=1u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return false;
            for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)return false;
            try{c.sourceCollisionRegistrationBorrowers_.push_back(&h);}catch(...){return false;}
            h.methodPinned=true;
        }
        if(!observer.recordMethod280A80(h.method)){at(observer.registrationPC());return false;}
        at(0x280afc);h.selectionAttempted=true;
        if(!actualMethod(&h,collider_.get(),h.method,h.slot,h.selectionPending,h.pc,true)){at(h.pc);return false;}
        h.selectionReturned=true;at(0x27e568);
        if(!h.slot||!observer.recordSelection280B08(*h.slot)){at(h.slot?observer.registrationPC():0x280b08u);return false;}
        at(0x280b88);auto& c=*h.method;auto& e=c.sourceEntries_;auto& state=c.source_;const auto slot=*h.slot;
        if(!slot||slot>1024||!state.field40||!state.lastUsed38||e.pending!=&h||
           e.pendingIdentity!=collider_.get()||e.pendingSlot!=slot||!h.selectionPending)return false;
        h.freeBefore=*state.field40;h.lastBefore=*state.lastUsed38;
        if(*state.field40==slot){at(0x27e594);if(!e.words||e.members[slot-1])return false;state.field40=e.nativeFreeNext[slot-1];}
        else{at(0x27e5a0);state.lastUsed38=*state.lastUsed38+1u;}
        h.freeAfter=*state.field40;h.lastAfter=*state.lastUsed38;e.pendingConsumed=true;h.counterAdvanced=true;
        at(0x27e5a8);if(!e.words)return false;
        e.members[slot-1]=collider_.get();e.borrowers[slot-1]=&h;e.wordKnown[slot-1]=false;
        e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
        h.selectionPending=false;h.inserted=true;at(0x27e5ac);
        if(!observer.publishPrimary280B90()){at(observer.registrationPC());return false;}
        h.published=true;at(0x280bf0);return true; //Portal custody remains next, not2809C0 return.
    }
    template<class ActualPortal>
    bool continueServicePortals280BF0(ActualPortal&& actualPortal)noexcept {
        auto& h=servicePrimary_;auto& p=h.portals;
        auto& observer=*serviceHeapMembership_;
        const auto stop=[&](std::uint32_t pc){h.pc=p.pc=pc;return false;};
        if(!h.published||p.entered)return stop(0x280bf0u);
        p.entered=true;p.pc=0x280bf0u;
        if(!observer.beginSecondary280BF0(&h))return stop(p.pc);
        p.helperEntered=true;p.cursorMethod=h.method;
        SourceMembershipPortalEdge initial;
        if(!actualPortal(0u,&h,collider_.get(),h.zone,h.center,nullptr,initial,p.graphPin,p.pc))return stop(p.pc);
        p.rootRead=true;p.root=p.cursor=initial.next;
        while(p.cursor){
            if(p.count==p.edges.size())return stop(0x280c40u);
            auto& s=p.edges[p.count++];s.previousMethod=p.cursorMethod;
            const bool edgeOkay=actualPortal(1u,&h,collider_.get(),h.zone,h.center,p.cursor,s,p.graphPin,p.pc);
            if(s.methodReturned)p.cursorMethod=s.method;
            if(!edgeOkay)return stop(p.pc);
            if(s.hit){
                p.cursorMethod=s.method;
                if(!s.methodReturned||!s.method)return stop(0x280c78u);
                auto& c=*s.method;
                if(c.application_!=application_||c.world_!=h.world||!c.sourceMode_||c.usage10_!=1u||c.ordinal_/10u!=1u||
                   c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return stop(0x280c90u);
                try{c.sourceCollisionRegistrationBorrowers_.push_back(&s);}catch(...){return stop(0x280c90u);}
                s.methodPinned=true;s.selectionAttempted=true;
                if(!actualPortal(2u,&h,collider_.get(),h.zone,h.center,p.cursor,s,p.graphPin,p.pc))return stop(p.pc);
                s.selectionReturned=true;
                if(!s.slot)return stop(0x280c9cu);
                auto& e=c.sourceEntries_;
                if(!*s.slot){
                    if(e.pending!=&s||e.pendingIdentity!=collider_.get()||e.pendingSlot)return stop(0x280c9cu);
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;s.selectionCanceled=true;
                }else{
                    const auto slot=*s.slot;
                    if(slot>1024||!s.selectionPending||e.pending!=&s||e.pendingIdentity!=collider_.get()||
                       e.pendingSlot!=slot||!c.source_.field40||!c.source_.lastUsed38)return stop(0x280cbcu);
                    s.freeHeadBefore=*c.source_.field40;s.lastUsedBefore=*c.source_.lastUsed38;
                    if(*c.source_.field40==slot){if(!e.words||e.members[slot-1])return stop(0x27e594u);
                        c.source_.field40=e.nativeFreeNext[slot-1];}
                    else c.source_.lastUsed38=*c.source_.lastUsed38+1u;
                    s.freeHeadAfter=*c.source_.field40;s.lastUsedAfter=*c.source_.lastUsed38;
                    e.pendingConsumed=true;s.counterAdvanced=true;
                    if(!e.words)return stop(0x27e5a8u);
                    e.members[slot-1]=collider_.get();e.borrowers[slot-1]=&s;e.wordKnown[slot-1]=false;
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
                    s.selectionPending=false;s.inserted=true;s.allocationAttempted=true;
                    const auto allocated=observer.allocateSecondary105720(&s);
                    s.allocationRecorded=observer.secondaryAllocationRecordedNative(&s);
                    if(allocated.status==Membership::SecondaryLinkStatus::nativeRefused)return stop(allocated.sourcePC);
                    s.allocationReturned=true;s.link=allocated.record;
                    if(!s.link)return stop(0x280cd0u); //Inserted membership survives NULL pool result.
                    s.link->slot14=slot;s.linkStoreMask|=1u;
                    s.link->child04=s.neighbor;s.linkStoreMask|=2u;
                    s.link->collisionMethod08=s.method;s.link->method08=nullptr;s.linkStoreMask|=4u;
                    s.link->portal0C=s.portal;s.linkStoreMask|=8u;
                    if(!s.distance)return stop(0x280ce8u);
                    s.link->distance10=*s.distance;s.linkStoreMask|=16u;
                    s.oldHead=observer.secondaryHeadNative();
                    s.link->next00=static_cast<Membership::SecondaryLink*>(const_cast<void*>(s.oldHead));s.linkStoreMask|=32u;
                    if(!observer.publishSecondary280CF4(&s,s.link,s.oldHead))return stop(0x280cf4u);
                    s.linkStoreMask|=64u;s.headPublished=true;
                }
            }
            if(!actualPortal(4u,&h,collider_.get(),h.zone,h.center,p.cursor,s,p.graphPin,p.pc))return stop(p.pc);
            s.complete=true;p.cursor=s.next;
        }
        if(!observer.finishSecondary280D30(&h))return stop(0x280d30u);
        p.returned=true;h.pc=p.pc=0x280d38u;return true; //Portal return only;120070 suffix not claimed.
    }
    bool servicePortalsCurrentNative()const noexcept {
        const auto& h=servicePrimary_;const auto& p=h.portals;
        if(!p.entered)return !p.helperEntered&&!p.rootRead&&!p.returned&&!p.nativeWithdrawn&&!p.count&&!p.pc&&!p.graphPin;
        if(!h.published||p.count>20||!p.pc||(p.returned&&(!p.rootRead||p.cursor||p.pc!=0x280d38u))||
           (p.rootRead&&!p.nativeWithdrawn&&!p.graphPin))return false;
        const auto* observer=serviceHeapMembership_.get();if(!observer)return false;
        if(observer->secondary_.entered!=p.helperEntered||observer->secondary_.returned!=p.returned||
           (p.helperEntered&&observer->secondary_.nativeWithdrawn!=p.nativeWithdrawn))return false;
        if(!p.helperEntered)return !p.rootRead&&!p.graphPin&&!p.count&&!p.returned;
        const void* cursor=p.root;auto* method=h.method;
        for(std::size_t i=0;i<p.count;++i){const auto& s=p.edges[i];
            if(s.previousMethod!=method||!source_portal_overlap231150::historyCurrent(s.overlap)||
               s.overlapReturned!=s.overlap.returned||s.hit!=s.overlap.hit||s.distanceStoreMask>3u||
               (s.entered&&(s.edge!=cursor||!(s.edgeIndex&1u)||s.edgeIndex>=40))||
               (s.distanceStoreMask&&(!s.hit||!s.distance||s.distance!=s.overlap.outputDistance))||
               (s.methodReturned&&(!s.hit||s.distanceStoreMask!=3u))||
               (s.methodPinned&&(!s.methodReturned||!s.method))||(s.methodPinWithdrawn&&!s.methodPinned)||
               (s.selectionAttempted&&!s.methodPinned)||(s.selectionReturned&&(!s.selectionAttempted||!s.slot))||
               (s.counterAdvanced&&!s.selectionReturned)||(s.inserted&&!s.counterAdvanced)||
               (s.allocationAttempted&&!s.inserted)||(s.allocationRecorded&&!s.allocationAttempted)||(s.allocationReturned&&!s.allocationRecorded)||
               (s.headPublished&&(!s.link||s.linkStoreMask!=127u))||(s.slotWithdrawn&&!s.inserted)||
               (s.linkReleased&&!s.allocationAttempted)||(s.selectionCanceled&&s.selectionPending))return false;
            if(s.counterAdvanced){
                if(!s.slot||!*s.slot||*s.slot>1024)return false;
                if(s.freeHeadBefore==*s.slot){if(s.lastUsedAfter!=s.lastUsedBefore||s.freeHeadAfter>1024)return false;}
                else if(s.freeHeadBefore||*s.slot!=s.lastUsedBefore+1u||s.lastUsedAfter!=s.lastUsedBefore+1u||s.freeHeadAfter)return false;
            }
            if(s.methodReturned)method=s.method;
            if(s.methodPinned&&!s.methodPinWithdrawn){const auto& c=*s.method;const auto& e=c.sourceEntries_;
                if(c.application_!=application_||c.world_!=h.world||!c.sourceMode_||!c.sourceCollisionEntriesCurrentNative()||c.usage10_!=1u||c.ordinal_/10u!=1u)return false;
                unsigned pins=0;for(auto pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&s)++pins;
                if(pins!=1||s.selectionPending!=(e.pending==&s))return false;
                if(s.selectionPending&&(e.pendingIdentity!=collider_.get()||e.pendingConsumed!=s.counterAdvanced||
                    (s.slot&&e.pendingSlot!=*s.slot)))return false;
                if(s.inserted&&!s.slotWithdrawn&&(!s.slot||!*s.slot||*s.slot>1024||
                    e.members[*s.slot-1]!=collider_.get()||e.borrowers[*s.slot-1]!=&s||e.wordKnown[*s.slot-1]))return false;
            }
            if(s.link&&!s.linkReleased){if(!observer->ownsSecondaryLinkNative(s.link))return false;
                const auto& l=*s.link;
                if(((s.linkStoreMask&1u)&&(!s.slot||l.slot14!=*s.slot))||((s.linkStoreMask&2u)&&l.child04!=s.neighbor)||
                   ((s.linkStoreMask&4u)&&(l.collisionMethod08!=s.method||l.method08))||
                   ((s.linkStoreMask&8u)&&l.portal0C!=s.portal)||((s.linkStoreMask&16u)&&(!s.distance||l.distance10!=*s.distance))||
                   ((s.linkStoreMask&32u)&&l.next00!=s.oldHead))return false;
            }
            if(s.complete){if(!s.overlapReturned||(s.hit&&(!s.selectionReturned||(*s.slot&&!s.headPublished))))return false;cursor=s.next;}
            else if(i+1!=p.count)return false;
            if(p.nativeWithdrawn&&(s.selectionPending||(s.methodPinned&&!s.methodPinWithdrawn)||
               (s.inserted&&!s.slotWithdrawn)||(s.allocationRecorded&&!s.linkReleased)))return false;
        }
        return cursor==p.cursor&&method==p.cursorMethod&&(!p.nativeWithdrawn||!p.graphPin);
    }
    bool withdrawServicePortalsNative()noexcept {
        auto& h=servicePrimary_;auto& p=h.portals;
        if(!p.entered||p.nativeWithdrawn)return true;
        if(!servicePortalsCurrentNative())return false;
        auto& observer=*serviceHeapMembership_;
        for(std::size_t i=p.count;i>0;--i){auto& s=p.edges[i-1];
            if(s.methodPinned&&!s.methodPinWithdrawn){auto& c=*s.method;auto& e=c.sourceEntries_;
                if(s.selectionPending){if(e.pending!=&s)return false;
                    if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;s.selectionCanceled=true;}
                if(s.inserted&&!s.slotWithdrawn){if(!s.slot||!e.words||e.pending||!c.source_.field40||
                    e.members[*s.slot-1]!=collider_.get()||e.borrowers[*s.slot-1]!=&s)return false;
                    e.nativeFreeNext[*s.slot-1]=*c.source_.field40;e.members[*s.slot-1]=e.borrowers[*s.slot-1]=nullptr;
                    e.wordKnown[*s.slot-1]=false;c.source_.field40=*s.slot;s.slotWithdrawn=true;}
            }
            if(s.allocationRecorded&&!s.linkReleased){if(!observer.releaseSecondaryNative(&s))return false;s.linkReleased=true;}
            if(s.methodPinned&&!s.methodPinWithdrawn){auto& pins=s.method->sourceCollisionRegistrationBorrowers_;
                auto j=pins.begin();while(j!=pins.end()&&*j!=&s)++j;if(j==pins.end())return false;
                pins.erase(j);s.methodPinWithdrawn=true;}
        }
        if(p.helperEntered&&!observer.withdrawSecondaryNative(&h))return false;
        //All graph-derived addresses now historical opaque tokens; never dereferenced after this reset.
        p.graphPin.reset();p.nativeWithdrawn=true;return true;
    }
    bool servicePrimaryCurrentNative()const noexcept {
        const auto& h=servicePrimary_;
        if(!h.entered)return !h.world&&!h.zone&&!h.method&&!h.pc&&!h.methodPinned&&!h.nativeWithdrawn;
        if(!boundsReturned_||!h.world||!h.zone||!serviceHeapMembership_||!h.pc||
           (h.methodPinned&&(!h.methodResolved||!h.method))||(h.methodUnpinned&&!h.methodPinned)||
           (h.selectionAttempted&&!h.methodPinned)||(h.selectionReturned&&(!h.selectionAttempted||!h.slot))||
           (h.counterAdvanced&&!h.selectionReturned)||(h.inserted&&!h.counterAdvanced)||
           (h.published&&!h.inserted)||(h.slotWithdrawn&&!h.inserted)||
           (h.selectionCanceled&&h.selectionPending))return false;
        const auto& o=serviceHeapMembership_->primary_;
        if(o.entered){std::array<std::uint32_t,4> center;std::memcpy(center.data(),boundsCenter_.v,16);
            if(o.occurrence!=&h||o.zone!=h.zone||o.center!=center||h.center!=center||o.nativeWithdrawn!=h.nativeWithdrawn||
               (o.methodRead&&(!h.methodResolved||o.method!=h.method))||
               (o.slotRead&&(!h.selectionReturned||!h.slot||o.slot!=*h.slot))||
               (h.published&&o.stores!=31))return false;
        }else if(h.methodResolved||h.methodPinned||h.selectionAttempted)return false;
        if(h.counterAdvanced){if(!h.slot||!*h.slot||*h.slot>1024)return false;
            if(h.freeBefore==*h.slot){if(h.lastAfter!=h.lastBefore||h.freeAfter>1024)return false;}
            else if(h.freeBefore||*h.slot!=h.lastBefore+1u||h.lastAfter!=h.lastBefore+1u||h.freeAfter)return false;}
        if(h.methodPinned&&!h.methodUnpinned){
            const auto& c=*h.method;const auto& e=c.sourceEntries_;
            if(c.application_!=application_||c.world_!=h.world||!c.sourceMode_||c.usage10_!=1||
               c.ordinal_/10u!=1u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return false;
            unsigned pins=0;for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)++pins;
            if(pins!=1||h.selectionPending!=(e.pending==&h))return false;
            if(h.selectionPending&&(e.pendingIdentity!=collider_.get()||
               (h.selectionReturned&&e.pendingSlot!=*h.slot)||e.pendingConsumed!=h.counterAdvanced))return false;
            if(h.inserted&&!h.slotWithdrawn&&(!e.words||e.members[*h.slot-1]!=collider_.get()||
               e.borrowers[*h.slot-1]!=&h||e.wordKnown[*h.slot-1]))return false;
        }
        if(h.nativeWithdrawn&&((h.methodPinned&&!h.methodUnpinned)||h.selectionPending||(h.inserted&&!h.slotWithdrawn)))return false;
        return servicePortalsCurrentNative();
    }
    bool withdrawServicePrimaryNative()noexcept {
        auto& h=servicePrimary_;if(!h.entered||h.nativeWithdrawn)return true;
        if(!servicePrimaryCurrentNative()||!serviceHeapMembership_||!serviceHeapMembership_->preflightNative())return false;
        if(serviceHeapMembership_->primary_.entered&&!serviceHeapMembership_->canWithdrawActiveRegistrationNative())return false;
        if(!withdrawServicePortalsNative())return false;
        if(h.methodPinned&&!h.methodUnpinned){auto& c=*h.method;auto& e=c.sourceEntries_;
            if(h.selectionPending){if(e.pending!=&h)return false;
                if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
                e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;h.selectionPending=false;h.selectionCanceled=true;}
            if(h.inserted&&!h.slotWithdrawn){if(!h.slot||!*h.slot||*h.slot>1024||!e.words||e.pending||!c.source_.field40||
                   e.members[*h.slot-1]!=collider_.get()||e.borrowers[*h.slot-1]!=&h)return false;
                e.nativeFreeNext[*h.slot-1]=*c.source_.field40;e.members[*h.slot-1]=e.borrowers[*h.slot-1]=nullptr;
                e.wordKnown[*h.slot-1]=false;c.source_.field40=*h.slot;h.slotWithdrawn=true;}
        }
        if(serviceHeapMembership_->primary_.entered&&!serviceHeapMembership_->withdrawRegistrationNative(&h))return false;
        if(h.methodPinned&&!h.methodUnpinned){auto& pins=h.method->sourceCollisionRegistrationBorrowers_;
            auto i=pins.begin();while(i!=pins.end()&&*i!=&h)++i;if(i==pins.end())return false;pins.erase(i);h.methodUnpinned=true;}
        h.nativeWithdrawn=true;return true; //Historical source fields/pointers not reset.
    }
    struct ServiceReturn120190 {
        bool entered=false,observerReturned=false,zoneRead=false,stackDestroyCalled=false;
        bool stackDestroyReturned=false,membershipReturned=false,triggerReturned=false;
        const void* selectedZone04=nullptr;
        std::uint32_t pc=0;
        OriginalCollisionMembershipGuards50LA::History zoneGuard;
    } serviceReturn_;
    bool finishServiceMembership120190(std::uint32_t& pc)noexcept {
        auto& h=serviceReturn_;
        if(h.entered||!servicePrimary_.portals.returned||servicePrimary_.portals.cursor||
           membershipNativeWithdrawn_||!serviceStackMembership_||!serviceHeapMembership_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        h.entered=true;at(0x280bc0);at(0x280bec);h.observerReturned=true;
        at(0x120190);
        if(!OriginalCollisionMembershipGuards50LA::beginZone(h.zoneGuard)){at(h.zoneGuard.pc);return false;}
        at(0x1201bc);h.selectedZone04=serviceHeapMembership_->record_.zone04;h.zoneRead=true;
        if(!OriginalCollisionMembershipGuards50LA::finishZone(h.zoneGuard,h.selectedZone04!=nullptr)){
            at(h.zoneGuard.pc);return false;}
        at(0x1201fc);at(0x120200);h.stackDestroyCalled=true;
        // Actual stack50, NOT selected persistent heapE0. Source argument -1
        // suppresses allocation free; the heap's registration/pool links stay.
        at(0x280920);
        if(!serviceStackMembership_->destroy280920(-1))return false;
        h.stackDestroyReturned=true;at(0x2809b8);
        at(0x120224);h.membershipReturned=true;
        at(0x2a0b1c);at(0x2a0b2c);h.triggerReturned=true;at(0x2dc2ac);
        return true; //Void caller return only; no manufactured V0 or action service.
    }
    bool serviceReturnCurrentNative()const noexcept {
        const auto& h=serviceReturn_;
        if(!OriginalCollisionMembershipGuards50LA::zoneHistory(h.zoneGuard))return false;
        if(!h.entered)return !h.observerReturned&&!h.zoneRead&&!h.selectedZone04&&!h.pc&&
            !h.stackDestroyCalled&&!h.stackDestroyReturned&&!h.membershipReturned&&!h.triggerReturned&&!h.zoneGuard.entered;
        if(!servicePrimary_.portals.returned||!serviceHeapMembership_||!serviceStackMembership_||
           !h.observerReturned||(h.zoneRead&&h.selectedZone04!=serviceHeapMembership_->record_.zone04)||
           (h.stackDestroyCalled&&(!h.zoneRead||!h.selectedZone04||!h.zoneGuard.returned))||
           (h.stackDestroyReturned&&!h.stackDestroyCalled)||(h.membershipReturned&&!h.stackDestroyReturned)||
           (h.triggerReturned&&!h.membershipReturned))return false;
        if(!h.zoneGuard.returned)return !h.stackDestroyCalled&&!h.membershipReturned&&h.pc==h.zoneGuard.pc;
        const auto& stack=*serviceStackMembership_;
        if(!h.stackDestroyReturned)return h.stackDestroyCalled&&!stack.destroyEntered_&&h.pc==0x280920u;
        if(!stack.destroyEntered_||!stack.destroyReturned_||!stack.countDecremented_||stack.lease_||
           stack.destructionArgument_!=-1||stack.sourceDeleted_||stack.destroyPC_!=0x2809b8u)return false;
        return h.membershipReturned&&h.triggerReturned&&h.pc==0x2dc2acu;
    }
    struct ServiceAction2A0A30 {
        bool entered=false,dataStored=false,codeStored=false,countIncremented=false,returned=false,nativeWithdrawn=false;
        const void* payload=nullptr;
        std::optional<std::uint32_t> countRead,capacityRead,codeA0;
    } serviceAction_;
    bool installServiceAction2A0A30(const void* actualPayload,std::uint32_t& pc)noexcept {
        auto& h=serviceAction_;
        if(h.entered||!actualPayload||!serviceReturn_.triggerReturned||membershipNativeWithdrawn_||!preflightNative())return false;
        h.entered=true;h.payload=actualPayload;pc=pc_=0x2a0a30;
        if(!count98_)return false;h.countRead=*count98_;
        pc=pc_=0x2a0a34;if(!capacity94_)return false;h.capacityRead=*capacity94_;
        pc=pc_=0x2a0a38;
        if(static_cast<std::int32_t>(*h.countRead)<static_cast<std::int32_t>(*h.capacityRead)){
            //Exact source authored action0/capacity1 owner; no unknown slot access.
            pc=pc_=0x2a0a44;
            if(*h.countRead!=0||*h.capacityRead!=1||!source9C_||*source9C_!=callbacks9C_.get()||!callbacks9C_)return false;
            pc=pc_=0x2a0a50;callbacks9C_[0]=actualPayload;h.dataStored=true;
            pc=pc_=0x2a0a54;if(!count98_||*count98_!=*h.countRead)return false;
            pc=pc_=0x2a0a58;if(!sourceA0_||*sourceA0_!=argumentsA0_.get()||!argumentsA0_)return false;
            //Source9C is DATA; sourceA0 is CODE, despite old backing names.
            //Typed source-PC knowledge supersedes unknown native pointer slot;
            //never reinterpret integer1B27C0 as an executable host pointer.
            pc=pc_=0x2a0a64;h.codeA0=0x1b27c0u;h.codeStored=true;
            pc=pc_=0x2a0a68;if(!count98_||*count98_!=*h.countRead)return false;
            pc=pc_=0x2a0a70;count98_=*h.countRead+1u;h.countIncremented=true;
        }
        pc=pc_=0x2a0a7c;h.returned=true;return true;
    }
    bool serviceActionCurrentNative()const noexcept {
        const auto& h=serviceAction_;
        if(!h.entered)return !h.payload&&!h.countRead&&!h.capacityRead&&!h.codeA0&&!h.dataStored&&
            !h.codeStored&&!h.countIncremented&&!h.returned&&!h.nativeWithdrawn;
        if(!serviceReturn_.triggerReturned||!h.payload||(h.capacityRead&&!h.countRead)||
           (h.dataStored&&(!h.countRead||!h.capacityRead||*h.countRead!=0||*h.capacityRead!=1))||
           (h.codeStored!=h.codeA0.has_value())||(h.codeStored&&(!h.dataStored||*h.codeA0!=0x1b27c0u))||
           (h.countIncremented&&!h.codeStored)||(h.returned&&(!h.countRead||!h.capacityRead||
               (static_cast<std::int32_t>(*h.countRead)<static_cast<std::int32_t>(*h.capacityRead)&&!h.countIncremented))))return false;
        if(h.countRead&&(!count98_||*count98_!=*h.countRead+(h.countIncremented?1u:0u)))return false;
        if(h.capacityRead&&capacity94_!=h.capacityRead)return false;
        if(h.dataStored&&!h.nativeWithdrawn&&(!callbacks9C_||callbacks9C_[0]!=h.payload))return false;
        return true;
    }
    bool withdrawServiceActionNative(const void* actualPayload)noexcept {
        auto& h=serviceAction_;if(!h.entered||h.nativeWithdrawn)return true;
        if(h.payload!=actualPayload||!serviceActionCurrentNative())return false;
        if(h.dataStored)callbacks9C_[0]=nullptr; //Native alias withdrawal, not a source9C clear receipt.
        h.nativeWithdrawn=true;return true;
    }
    static void identity(BodyMatrix48G& m)noexcept {
        std::memset(m.v,0,sizeof m.v);m.v[15]=1;m.v[10]=1;m.v[5]=1;m.v[0]=1;
    }
    bool construct2A0290(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        std::uint64_t key,const std::array<std::uint32_t,4>& actual4438D0,std::uint32_t& pc)noexcept {
        if(application_||counted_||constructed_||nativeClosed_)return false;
        application_=&app;pc=pc_=0x2a029c;
        const auto r=initializeOriginalWreckComponent50LA(entity_,OriginalWreckComponentKind50LA::trigger2A0290,
            generation,key,true,app.objectCount423F60());
        if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}counted_=true;
        pc_=0x2a02b0;vtable08_=0x3d3930;
        position30_={{0,0,0,1}};box40_={{0,0,0,1}}; // 2A02BC/2A02C8
        flags1B8_|=1;flagsKnown_|=1; //2A02FC
        flags1B8_|=2;flagsKnown_|=2; //2A032C
        source9C_=nullptr;sourceA0_=nullptr;field1A8_=0; //2A0338/340/348
        for(unsigned bit=2;bit<6;++bit){flags1B8_&=static_cast<std::uint8_t>(~(1u<<bit));flagsKnown_|=1u<<bit;}
        random90_=100; //2A03A0; upper two flag bits remain unknown.
        std::memcpy(&position30_,actual4438D0.data(),16); //2A03AC
        radius1AC_=0.f; //2A03B8 delay slot before genuine identity14D840.
        identity(matrix50_); // source eight 8-byte copies2A03D8/DC.
        capacity94_=0;count98_=0;field1B4_=0;
        constructed_=true;pc=pc_=0x2dbffc;return true;
    }
    // Native finite/FE_TONEAREST adaptation explicitly authorized for this
    // authored cohort. NOT an EE bit-exact MUL.S emulation claim. Refusal at
    // the operation retains every preceding allocation/publication/store.
    bool sphere11C0B0(const SpringVector4_48N& position,float radius,std::uint32_t& pc)noexcept {
        auto& b=*collider_;if(!b.freshShapeNull()||shapeAttempted_)return false;
        shapeAttempted_=true;pc=pc_=0x11c0dc; //11B600 genuine fresh-NULL branch.
        b.shape8C_=nullptr;b.result90_=nullptr;
        pc=pc_=0x11c0e4;try{shape_.reset(new Shape);}catch(...){shape_.reset();}shapeAllocationReturned_=true;
        if(shape_){shape_->value.localCenter={{0,0,0,1}};shape_->known|=1;} //11C0F4
        b.shape8C_=shape_.get(); //11C0F8 even NULL.
        pc=pc_=0x11c104;b.shape88_=5; //11C108 delay slot BEFORE malloc160.
        try{result_.reset(new Result);}catch(...){result_.reset();}resultAllocationReturned_=true;
        if(result_){const SpringVector4_48N vf0{{0,0,0,1}};result_->center20=vf0;
            for(auto& v:result_->normals30)v=vf0;for(auto& v:result_->points60)v=vf0;}
        pc=pc_=0x11c160;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(radius))return false;
        volatile float squared=radius*radius;if(!std::isfinite(squared))return false;
        pc=pc_=0x11c164;if(!result_)return false;
        result_->capacity00=3;result_->count04=0;b.result90_=result_.get(); //164/168/16C
        pc=pc_=0x11c180;if(!shape_)return false;
        shape_->owner60=nullptr;
        auto& s=shape_->value;s.localCenter=position;shape_->known|=1; //11C18C
        s.radius50=radius;shape_->known|=2;s.radiusSquared54=squared;shape_->known|=4;
        s.radiusScale58=1;shape_->known|=8;s.centerScale5C=1;shape_->known|=16;
        identity(s.world);shape_->known|=32;shapeReturned_=true;return true;
    }
    bool setup2A04B0(const std::array<std::uint32_t,4>& position,const std::array<std::uint32_t,4>& box,
        const std::array<std::uint32_t,4>& look,const std::array<std::uint32_t,4>& up,
        std::uint32_t radiusBits,std::int32_t capacity,std::int32_t random,
        const std::array<std::uint32_t,4>& actualSentinel,std::uint32_t actualCategory,
        std::uint8_t& actualCollisionValid,std::uint32_t& pc)noexcept {
        if(!constructed_||setupAttempted_||nativeClosed_)return false;
        setupAttempted_=true;
        // Four actual stack vector copies precede no-op141C20. They are
        // consumed immediately; look/up affect only the unjoined box branch.
        SpringVector4_48N p,bx,lk,u;std::memcpy(&p,position.data(),16);std::memcpy(&bx,box.data(),16);
        std::memcpy(&lk,look.data(),16);std::memcpy(&u,up.data(),16);
        pc=pc_=0x2a0514;position30_=p;box40_=bx;radius1AC_=std::bit_cast<float>(radiusBits);
        pc=pc_=0x2a0524;random90_=static_cast<std::uint32_t>(random); //2A0528 delay slot.
        try{collider_.reset(new B);}catch(...){collider_.reset();}colliderAllocationReturned_=true;
        if(collider_){SpringVector4_48N sentinel;std::memcpy(&sentinel,actualSentinel.data(),16);
            const auto r=collider_->construct11B190(*application_,entity_.sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&collider_->entity_)),sentinel);
            if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}}
        source1B0_=collider_.get();pc=pc_=0x11b574;if(!collider_)return false; //2A053C then11B570.
        auto& cb=*collider_;
        cb.flagsF8_&=static_cast<std::uint8_t>(~8u);cb.flagsF8_|=2;cb.flagsF8_|=4;actualCollisionValid=0;
        pc=pc_=0x2a055c;cb.categoryA4_=actualCategory;
        pc=pc_=0x2a0564;
        if(!std::isfinite(*radius1AC_)||!(*radius1AC_>0.f))return false; //Box branch not implemented.
        if(!sphere11C0B0(p,*radius1AC_,pc))return false;
        cb.modeC8_=1; //124004 return delay reached by2A0704.
        cb.owner9C_=&entity_;cb.callbacksE4_[1].reset();callbackE8PC_=0x2a00a0; //2A0718/720. Typed code-PC lane supersedes base null pointer knowledge; no callback invocation.
        // Actual11F570 shape5 with source owner60 NULL ->11ED60(NULL).
        // Both11F3B0/B4 and11FCAC/B0 write identity. VU identity-transform
        // result is exact for the admitted finite, positive authored XYZ,W=1;
        // there is no general matrix/bounds projection or query readiness.
        pc=pc_=0x11f3a4;identity(shape_->value.world);
        cb.cachedCenter60_=shape_->value.localCenter;cb.radius80_=shape_->value.radius50; //11F404/410
        pc=pc_=0x11f430;
        for(unsigned k=0;k<3;++k)if(!std::isfinite(cb.cachedCenter60_.v[k])||!(cb.cachedCenter60_.v[k]>0.f))return false;
        if(std::bit_cast<std::uint32_t>(cb.cachedCenter60_.v[3])!=0x3f800000u)return false;
        //11F444 exact identity result already equals cached60.
        identity(shape_->value.world); //11F680 shape5 NULL, second copy.
        cb.cachedCenter60_=shape_->value.localCenter;cb.radius80_=shape_->value.radius50; //11FD00/0C
        refreshReturned_=true;
        C* nodes[]{&entity_,&cb.entity_};OriginalWreckComponentDomain50LA domain{entity_.sessionGeneration,nodes,2};
        pc=pc_=0x2a073c;const auto r=cb.append141960(domain,entity_,2);
        if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}
        if(capacity<=0)capacity=1;
        pc=pc_=0x2a0754;try{callbacks9C_.reset(new const void*[static_cast<std::size_t>(capacity)]);}catch(...){callbacks9C_.reset();}
        callbacksAllocationReturned_=true;source9C_=callbacks9C_.get(); //2A075C; elements unknown.
        pc=pc_=0x2a0760;try{argumentsA0_.reset(new const void*[static_cast<std::size_t>(capacity)]);}catch(...){argumentsA0_.reset();}
        argumentsAllocationReturned_=true;sourceA0_=argumentsA0_.get(); //2A0768; elements unknown.
        capacity94_=static_cast<std::uint32_t>(capacity);count98_=0; //2A076C/770
        setupReturned_=true;pc=pc_=0x2dc024;return true;
    }
    C* colliderEntity()noexcept{return collider_?&collider_->entity_:nullptr;}
    const C* colliderEntity()const noexcept{return collider_?&collider_->entity_:nullptr;}
    std::size_t countedEntities()const noexcept{return std::size_t(counted_)+(collider_&&collider_->entity_.reference00?1u:0u);}
    bool preflightNative()const noexcept {
        if(nativeClosed_)return !counted_&&!collider_&&!shape_&&!result_&&!callbacks9C_&&!argumentsA0_;
        if(!application_||application_->attachmentLock423F58()||!serviceBoundsCurrentNative()||!servicePrimaryCurrentNative()||!serviceReturnCurrentNative()||!serviceActionCurrentNative())return false;
        if((membershipEntered_&&(!serviceGeometryReturned_||!serviceStackMembership_))||
           (membershipStackReturned_&&!membershipEntered_)||(membershipFlagsF8_&&!membershipStackReturned_)||
           (membershipOldE0NullRead_&&(!membershipFlagsF8_||(*membershipFlagsF8_&8u)))||
           (membershipHeapAllocationReturned_&&!membershipOldE0NullRead_)||
           (serviceHeapMembership_&&!membershipHeapAllocationReturned_)||
           (membershipHeapReturned_&&!serviceHeapMembership_)||
           (membershipE0Published_&&(!membershipHeapAllocationReturned_||(serviceHeapMembership_&&!membershipHeapReturned_))))return false;
        if(serviceStackMembership_&&(!serviceStackMembership_->preflightNative()||
           serviceStackMembership_->returned()!=membershipStackReturned_))return false;
        if(serviceHeapMembership_&&(!serviceHeapMembership_->preflightNative()||
           serviceHeapMembership_->returned()!=membershipHeapReturned_))return false;
        if(membershipNativeWithdrawn_&&((serviceStackMembership_&&!serviceStackMembership_->nativeWithdrawn_)||
           (serviceHeapMembership_&&!serviceHeapMembership_->nativeWithdrawn_)))return false;
        if(membershipFlagsF8_&&(!collider_||*membershipFlagsF8_!=collider_->flagsF8_))return false;

        if((serviceEntered_&&!setupReturned_)||(serviceAttachment1A8_&&!serviceEntered_)||
           (serviceMatrix50_&&(!serviceAttachment1A8_||*serviceAttachment1A8_))||
           (serviceMatrixCopied_&&!serviceMatrix50_)||(serviceCenterStored_&&!serviceMatrixCopied_)||
           (serviceRadiusStored_&&!serviceCenterStored_)||(serviceGeometryReturned_&&!serviceRadiusStored_))return false;
        if(serviceAttachment1A8_&&serviceAttachment1A8_!=field1A8_)return false;
        if(serviceMatrix50_&&std::memcmp(serviceMatrix50_->v,matrix50_.v,sizeof matrix50_.v))return false;
        if(serviceMatrixCopied_&&(!shape_||std::memcmp(shape_->value.world.v,matrix50_.v,sizeof matrix50_.v)))return false;
        if(serviceCenterStored_&&(!collider_||!shape_||std::memcmp(collider_->cachedCenter60_.v,shape_->value.localCenter.v,sizeof shape_->value.localCenter.v)))return false;
        if(serviceRadiusStored_&&(!collider_||!shape_||!collider_->radius80_||
           std::bit_cast<std::uint32_t>(*collider_->radius80_)!=std::bit_cast<std::uint32_t>(shape_->value.radius50)))return false;

        if((serviceRegistrationAttempted_&&(!serviceReturn_.triggerReturned||!serviceParent_))||
           (serviceParent_&&!serviceRegistrationAttempted_)||(serviceRegistered_&&!serviceRegistrationAttempted_)||
           (serviceTopologyWithdrawn_&&!serviceRegistered_))return false;
        if(counted_&&(entity_.reference00!=1||entity_.nativeFlags29||entity_.mask20!=(serviceRegistered_?0x402u:0u)))return false;
        if(serviceRegistered_&&!serviceTopologyWithdrawn_){
            if(entity_.parent1C!=serviceParent_)return false; //Exact siblings are checked by source list owner.
        }else if(entity_.previous10||entity_.next14||entity_.parent1C)return false;
        if(!counted_&&entity_.reference00)return false;
        if(setupReturned_&&(!constructed_||!refreshReturned_||!capacity94_||*capacity94_!=1||!count98_||*count98_!=(serviceAction_.countIncremented?1u:0u)||
            !callbacksAllocationReturned_||!argumentsAllocationReturned_))return false;
        if(source9C_&&*source9C_!=callbacks9C_.get())return false;
        if(sourceA0_&&*sourceA0_!=argumentsA0_.get())return false;
        if(source1B0_&&*source1B0_!=collider_.get())return false;
        if(!collider_)return !entity_.firstChild18&&!entity_.descendants24&&!shape_&&!result_;
        const auto& b=*collider_;const auto& c=b.entity_;
        if(c.reference00&&c.reference00!=1)return false;
        if(c.firstChild18||c.previous10||c.next14||c.nativeFlags29)return false;
        if(b.appended_){if(c.parent1C!=&entity_||entity_.firstChild18!=&c||c.mask20!=2||entity_.descendants24!=2)return false;}
        else if(c.parent1C||entity_.firstChild18||entity_.descendants24||c.mask20)return false;
        if(b.phase_==B::Phase::complete){
            if(membershipE0Published_){
                const void* expected=serviceHeapMembership_?serviceHeapMembership_->identity():nullptr;
                if(!b.membershipE0_||*b.membershipE0_!=expected||b.listC0_||b.listCC_||b.listD8_||
                   b.countC4_||b.countD0_||b.countDC_)return false;
            }else if(!b.constructorNullListsCurrent())return false;
            if(!b.bodyA0_||*b.bodyA0_)return false;
            if(b.owner9C_&&*b.owner9C_&&*b.owner9C_!=&entity_)return false;
            for(unsigned i=0;i<4;++i){if(i==1&&callbackE8PC_){
                if(*callbackE8PC_!=0x2a00a0||b.callbacksE4_[i])return false;
            }else if(!b.callbacksE4_[i]||*b.callbacksE4_[i])return false;}
            if(b.shape8C_&&*b.shape8C_!=shape_.get())return false;
            //Result can be allocated but unpublished before11C16C.
            if(b.result90_&&*b.result90_&&*b.result90_!=result_.get())return false;
        }else if(shape_||result_||b.appended_)return false;
        return application_->objectCount423F60()>=countedEntities();
    }
    // Called only after all actual authority entries have been withdrawn by
    // the same World preflight. No fallible source destructor/dispatch claim.
    void closeNativeAfterAuthority()noexcept {
        if(serviceRegistered_&&!serviceTopologyWithdrawn_)std::terminate();
        if(serviceAction_.entered&&!serviceAction_.nativeWithdrawn){
            if(!withdrawServiceActionNative(serviceAction_.payload))std::terminate();
        }
        // World withdraws both observer leases before authority/count disposal.
        if((serviceStackMembership_&&!serviceStackMembership_->nativeWithdrawn_)||
           (serviceHeapMembership_&&!serviceHeapMembership_->nativeWithdrawn_))std::terminate();
        if(collider_)collider_->membershipE0_.reset(); //Native alias withdrawal only.
        serviceHeapMembership_.reset();serviceStackMembership_.reset();
        source9C_.reset();sourceA0_.reset();source1B0_.reset();
        callbacks9C_.reset();argumentsA0_.reset();shape_.reset();result_.reset();
        if(collider_){auto& b=*collider_;if(b.appended_){entity_.firstChild18=nullptr;entity_.descendants24=0;}
            if(b.entity_.reference00){--b.entity_.reference00;--application_->objectCount423F60();}
            b.phase_=B::Phase::released;collider_.reset();}
        if(counted_){--entity_.reference00;--application_->objectCount423F60();counted_=false;}
        nativeClosed_=true;
    }
public:
    OriginalWorldTriggerInstance50LA()=default;
    OriginalWorldTriggerInstance50LA(const OriginalWorldTriggerInstance50LA&)=delete;
    OriginalWorldTriggerInstance50LA& operator=(const OriginalWorldTriggerInstance50LA&)=delete;
    ~OriginalWorldTriggerInstance50LA(){if(entity_.reference00||collider_)std::terminate();}
};
} //namespace rr::game
