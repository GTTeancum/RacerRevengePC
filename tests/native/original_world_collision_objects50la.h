#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/original_wreck_components50la.h"
#include "../../src/game/original_application_entities50la.h"
#include "../../src/game/original_collision_base_owner50la.h"
#include "../../src/game/original_source_collision_path50la.h"
#include "../../src/game/original_source_collision_observer50la.h"
#include "../../src/game/original_collision_membership_guards50la.h"
#include "ra_static_candidates50la.h"
#include "../../src/game/original_source_portal_overlap231150.h"
#include <limits>
#include <cstring>
#include <cstdio>
#include <optional>
#include <array>
#include <cmath>
#include <cfenv>
#include <memory>
#include <string>
#include <vector>
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Source collision transform requires strict binary32 arithmetic"
#endif
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_pod_scene50LA {class Owner;}
namespace ra_static_model_cold50LA {class Owner;}
namespace rr::game {
// Same collision collection, exclusive actual1B4EC0 descriptor-source mode
// or legacy native271230/reached27B040 custody. Source mode owns descriptors
// and the reached1B5590 counted collision-constructor prefix, including shared
// type-registry effects; never decoded candidates, geometry, runtime queries,
// collection membership or physics readiness.
class OriginalWorldCollisionObjects50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_static_model_cold50LA::Owner;
    struct SourceDescriptor {
        std::array<char,260> name00;std::size_t nameKnown=0;
        std::array<std::uint32_t,4> position110,look120,up130,smokeOffset150,smokeVelocity160;
        std::uint32_t vtable170=0,useLighting140=0,dynamic144=0,shadow148=0,smoker14C=0;
        std::uint32_t field180=0,field184=0;std::uint8_t vectorKnown=0;
        bool constructed=false,loaded=false;
        //Native backing initialization is not source knowledge. Unwritten
        //name tail,174/178/17C/188/18C have no source consumer or value.
    };
    struct Descriptor {
        std::optional<SourceDescriptor> sourceFields;
        const assets::WorldInstance48F* source=nullptr;
        std::string name00;
        std::array<float,4> position110{0,0,0,1},look120{0,0,1,1},up130{0,1,0,1};
        std::array<float,4> smokeOffset150{0,0,0,1},smokeVelocity160{0,0,0,1};
        std::uint32_t useLighting140=1,useDynamicLighting144=0,field148=1,isSmoker14C=0;
        std::uint32_t field180=0,field184=0;
        //3CA8F0..3CA94C constructs actual442F80=(0,0,0,1).
    };
    struct NativeQuery {
        OriginalWreckComponent50LA entity; // Actual source collision+8 identity.
        OriginalApplicationEntities50LA* application=nullptr;
        const Type2StaticCandidate49X* candidate=nullptr;
        const assets::WorldInstance48F* source=nullptr;
        const std::uint32_t* liveTerrain180=nullptr;
        std::string name,member;
        std::size_t resourceIndex=std::numeric_limits<std::size_t>::max();
        std::uint32_t sourceCategoryA4=0;
        std::uint8_t sourceType02=0xff,sourceTerrainA8=0,sourceMaterialA9=0;
        // Only source-written lanes; upper unknown bits are not fabricated.
        std::uint8_t queryFlags03KnownMask=7,queryFlags03Value=0;
        std::uint8_t flagsF8KnownMask=0x37,flagsF8Value=6;
        bool counted=false,typed=false,flagsApplied=false;
        bool resourceStarted=false,loaded=false,resourceWithdrawn=false;
        bool appended=false,registered=false,retired=false,terminal=false;
        bool negativeLoadReturned=false;
        //Typed native temporary280890 frame; NOT persistent collisionE0.
        struct TemporaryObserver {
            const void* next00=nullptr;
            const OriginalWreckComponent50LA* child04=nullptr;
            const void* method08=nullptr;
            std::uint32_t field0C=0,field10=0,slot14=0;
            std::array<float,4> center20{{0.f,0.f,0.f,1.f}};
            //Source18/1C untouched; no inferred values or consumers.
        };
        std::unique_ptr<TemporaryObserver> observerToken;
        bool observerSharedConstructed=false,placementAdopted=false,observerAttempted=false;
        std::size_t primaryZone=std::numeric_limits<std::size_t>::max();
        std::uint32_t primarySlot=0;

        std::uint32_t failurePC=0;
    };
    //Dedicated1B5590 callback history. Never a Physics BASE E0/E4 occurrence
    //or a NativeQuery/candidate facade. The raw source04 names the OUTER;
    //only its embedded Entity+8 is counted by11B190.
    //Private retained1B5784 stack occurrence, not a supplied ready matrix.
    //Native storage defaults confer no source knownness.
    struct SourceTransform {
        bool entered=false,firstSetterEntered=false;
        std::uint8_t stackKnown=0,matrixRows=0;
        std::array<std::uint32_t,4> stack1B0{},stack1A0{},stack190{};
        std::array<std::array<std::uint32_t,4>,4> matrix150{};
    };
    struct SourceSetter {
        bool entered=false,radiusStored=false,centerTransformed=false,returned=false;
        unsigned previousWords=0,inputWords=0,composedRows=0,composedCalculatedRows=0,centerLanes=0,halfLanes=0;
        //Immutable numeric output receipts. Native withdrawal must NOT rerun
        //FP or depend on the caller preserving rounding/FTZ/FCSR settings.
        std::array<std::array<std::uint32_t,4>,4> inputMatrix{},composedOutput{};
        std::array<std::uint32_t,4> transformedCenter{};
        std::array<std::uint32_t,4> stack50{};
        std::uint32_t stack15C=0,radiusBits=0;
    };
    struct SourceMembershipBounds {
        bool entered=false,returned=false,centerStored=false;
        const void* shape=nullptr;const void* node=nullptr;
        unsigned reads=0,matrixWords=0;std::uint8_t halfStores=0;
        std::uint32_t pc=0;
        std::array<std::uint32_t,4> localCenter{{0,0,0,0x3f800000u}},localHalf{{0,0,0,0x3f800000u}};
        std::array<std::uint32_t,16> composed{};
        //Immutable numeric receipts; currentness/abort never reexecute FP.
        std::array<std::uint32_t,4> projectedCenter{},projectedHalf{};
    };
    using MembershipObserver=OriginalSourceCollisionObserver50LA;
    using MembershipGuards=OriginalCollisionMembershipGuards50LA;
    //Stable per-edge addresses are occurrence tokens; no callback survives this call.
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
        MembershipObserver::SecondaryLink* link=nullptr;const void* oldHead=nullptr;
    };
    struct SourceMembershipPortals280BF0 {
        bool entered=false,helperEntered=false,rootRead=false,returned=false,nativeWithdrawn=false;
        std::uint32_t pc=0;std::size_t count=0;
        const void* root=nullptr;const void* cursor=nullptr;
        OriginalNativeListMethodCustody50LA* cursorMethod=nullptr;
        std::shared_ptr<const void> graphPin;
        std::array<SourceMembershipPortalEdge,20> edges{};
    };
    struct SourceMembershipReturn120190 {
        const void* selectedIdentity=nullptr;
        const void* zone04=nullptr;
        bool entered=false,observerReturned=false,zoneRead=false;
        bool stackDestroyCalled=false,stackDestroyReturned=false,returned=false;
        std::int32_t stackArgument=0;
        std::uint32_t observerReturnPC=0,stackReturnPC=0,pc=0;
        MembershipGuards::History zoneGuard;
    };
    struct SourceMembership {
        //Exact World4C4[World354] input. Live membership is separately
        //proved by primary/portal receipts, never a BodyB8 or Physics alias.
        const void* zoneIdentity=nullptr;
        std::uint32_t zone354=0,mode=0,pc=0;
        bool entered=false,stackCalled=false,stackReturned=false,flagsRead=false;
        bool stackSelected=false,scratchInitialized=false,nativeWithdrawn=false;
        std::uint8_t flagsF8=0;
        //Native retained backing for this invocation's stack50, NOT guest
        //heap allocation and NEVER published into the collision's E0 field.
        std::unique_ptr<OriginalSourceCollisionObserver50LA> stack;
        OriginalCollisionMembershipGuards50LA::History allocationGuard;
        SourceMembershipBounds bounds;
        bool primaryCalled=false,methodResolved=false,methodPinned=false,methodPinWithdrawn=false;
        bool selectionAttempted=false,selectionReturned=false,selectionPending=false,selectionCanceled=false;
        bool insertEntered=false,counterAdvanced=false,inserted=false,slotWithdrawn=false;
        bool primaryPublished=false,primaryNativeWithdrawn=false;
        OriginalNativeListMethodCustody50LA* method=nullptr;
        std::optional<std::uint32_t> slot;
        std::uint32_t freeHeadBefore=0,lastUsedBefore=0,freeHeadAfter=0,lastUsedAfter=0;
        std::uint32_t selectionReturnPC=0,insertionReturnPC=0;
        const void* outerIdentity=nullptr;
        const ::ra_pod_scene50LA::Owner* world=nullptr;
        bool returned=false;
        SourceMembershipPortals280BF0 portals;
        SourceMembershipReturn120190 finish;
        std::array<std::uint32_t,4> center{},half{};
    };
    struct SourceCallback {
        std::unique_ptr<OriginalCollisionBaseOwner50LA> outer;
        OriginalSourceCollisionPath50LA loaderPath;
        OriginalCollisionBaseOwner50LA* source04=nullptr; //Original27AFB0 NULL until1B5618.
        const Descriptor* descriptorIdentity=nullptr; //Live borrow only; cleared before source free.
        //Native history of initialized, consumed fields only. Not a surviving
        //source descriptor, allocator object, or replacement wrapper+08.
        std::optional<SourceDescriptor> descriptorHistory;
        bool descriptorFreed=false,descriptorCleared=false,returned=false;
        enum class Skip:std::uint8_t {none,type2,missingName};
        Skip skip=Skip::none;
        bool skipDiagnosticAttempted=false;std::size_t skipDiagnosticWritten=0;
        std::optional<std::uint32_t> returnV0;
        OriginalApplicationEntities50LA* application=nullptr;
        std::uint64_t generation=0;
        std::array<char,0x104> stackName;std::size_t stackNameKnown=0;
        std::array<std::uint32_t,4> sentinel{};
        bool entered=false,nameCompared=false,allocationAttempted=false;
        bool counted=false,constructorReturned=false,published=false;
        bool mode13Returned=false,categoryStored=false,descriptorBytesStored=false;
        bool loaderSuccessObserved=false,attachmentEnrolled=false,parentAttached=false,parentDetachedNative=false;
        OriginalWreckComponent50LA* parentBorrow=nullptr;
        //Stable per-zone container identity, not a pointer into vector storage.
        //The ordinal is native custody only; source Entity sibling links remain authoritative.
        std::vector<OriginalWreckComponent50LA*>* parentLedger=nullptr;
        std::optional<std::size_t> parentLedgerIndex;
        std::optional<std::uint32_t> parentZone354;
        std::uint32_t attachmentPC=0;
        SourceTransform transform;
        SourceSetter setter; //JAL1B587C, return1B5884.
        SourceMembership membership;
        SourceSetter secondSetter; //Distinct JAL1B5888, return1B5890; never first-call replay.
        std::size_t foldedNameBytes=0;
        std::uint32_t startupCategoryA4=0;
        const OriginalWreckComponent50LA* modeReturn=nullptr; //Actual141C20 result, not outer04.
        std::uint32_t pc=0,dispatchFC=0,dispatch10=0;
    };
    struct Wrapper {
        bool sourceMode=false,sourceTypeKnown=false,sourceDescriptorPublished=false;
        std::uint32_t sourceVtable0C=0;
        SourceCallback sourceCallback;
        std::uint32_t type00=1;
        OriginalWreckComponent50LA* collision04=nullptr;
        std::unique_ptr<Descriptor> descriptor08;
        const ra_cold_sources50LA::CollisionOwner* decodedCollision=nullptr;
        // Immutable authored identity survives source descriptor08 destruction.
        const assets::WorldInstance48F* source=nullptr;
        std::unique_ptr<NativeQuery> nativeQuery;
        bool nativeAttempted=false,descriptorRetired=false;
    };
    static bool sourceCallbackCurrentNative(const Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation)noexcept {
        using B=OriginalCollisionBaseOwner50LA;const auto& h=w.sourceCallback;
        if(h.stackNameKnown>h.stackName.size())return false;
        if(h.skip!=SourceCallback::Skip::none){
            //These are actual V0=0 returns, not loaded collision objects. None
            //retains +08; only type2 executes the early descriptor free.
            const bool type2=h.skip==SourceCallback::Skip::type2;
            if(!h.entered||!h.returned||h.returnV0!=0u||h.pc!=0x27bee8u||
               h.application!=&app||h.generation!=generation||!w.sourceMode||
               !w.sourceTypeKnown||w.type00!=(type2?2u:1u)||!w.sourceDescriptorPublished||
               w.collision04||w.nativeQuery||w.nativeAttempted||h.outer||h.source04||
               h.allocationAttempted||h.counted||h.constructorReturned||h.published||
               h.stackNameKnown||h.dispatchFC||h.dispatch10||h.mode13Returned||
               h.categoryStored||h.descriptorBytesStored||h.loaderPath.entered_||
               h.loaderSuccessObserved||h.attachmentEnrolled||h.parentAttached||
               h.parentBorrow||h.parentLedger||h.parentLedgerIndex||h.parentZone354||
               h.parentDetachedNative||h.attachmentPC||h.foldedNameBytes||h.startupCategoryA4||h.modeReturn||
               h.transform.entered||h.setter.entered||h.secondSetter.entered||h.membership.entered||
               h.descriptorHistory||h.descriptorFreed!=type2||h.descriptorCleared!=type2||
               w.descriptorRetired!=type2||h.nameCompared==type2||
               h.skipDiagnosticAttempted==type2)return false;
            if(!setterCurrentNative(h)||!membershipCurrentNative(h)||
               !h.loaderPath.currentNative(app.collisionDirectory36B0F0_,app.collisionName3E0730_,
                   std::string_view{},h.pc))return false;
            if(type2)return !w.descriptor08&&!h.descriptorIdentity&&!h.skipDiagnosticWritten;
            if(!w.descriptor08||h.descriptorIdentity!=w.descriptor08.get()||!w.descriptor08->sourceFields)return false;
            const auto& d=*w.descriptor08->sourceFields;
            return d.loaded&&d.nameKnown==5&&std::memcmp(d.name00.data(),"none",5)==0;
        }
        if(h.skipDiagnosticAttempted||h.skipDiagnosticWritten)return false;
        if(h.descriptorFreed!=w.descriptorRetired||h.descriptorFreed!=bool(h.descriptorHistory)||
           h.descriptorFreed!=h.descriptorCleared||h.descriptorFreed!=h.returned||
           h.returned!=bool(h.returnV0))return false;
        if(h.returned&&(*h.returnV0!=1u||h.pc!=0x27bee8u||w.descriptor08||h.descriptorIdentity||
           !h.membership.returned||h.membership.pc!=0x120224u||!h.parentAttached||!h.loaderSuccessObserved))return false;
        const SourceDescriptor* descriptor=h.descriptorHistory?&*h.descriptorHistory:
            (w.descriptor08&&w.descriptor08->sourceFields?&*w.descriptor08->sourceFields:nullptr);
        if(h.loaderSuccessObserved&&(!h.loaderPath.loaderReturned_||h.loaderPath.loaderReturnV0_!=1||
           !h.outer||!h.source04||(!h.transform.firstSetterEntered&&(h.pc<0x1b5750u||h.pc>0x1b587cu))||
           (h.transform.firstSetterEntered&&!h.setter.entered&&h.pc!=0x11f680u)))return false;
        if(!h.loaderSuccessObserved&&(h.attachmentEnrolled||h.parentAttached||h.parentDetachedNative||
           h.parentBorrow||h.parentLedger||h.parentLedgerIndex||h.parentZone354||h.attachmentPC))return false;
        if(h.parentZone354&&*h.parentZone354>=10)return false;
        if(h.parentAttached&&(!h.attachmentEnrolled||!h.parentZone354||h.attachmentPC!=0x141a58u||
           (!h.transform.entered&&h.pc!=0x1b5784u)))return false;
        if(h.parentDetachedNative&&(!h.parentAttached||h.parentBorrow||h.parentLedger||h.parentLedgerIndex))return false;
        const auto& t=h.transform;
        if(!t.entered&&(t.stackKnown||t.matrixRows||t.firstSetterEntered))return false;
        if(t.entered&&(!h.parentAttached||!descriptor))return false;
        if(t.stackKnown!=0&&t.stackKnown!=1&&t.stackKnown!=3&&t.stackKnown!=7)return false;
        if(t.matrixRows!=0&&t.matrixRows!=1&&t.matrixRows!=3&&t.matrixRows!=7&&t.matrixRows!=15)return false;
        if(t.entered){const auto& d=*descriptor;
            if((t.stackKnown&1)&&t.stack1B0!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})return false;
            if((t.stackKnown&2)&&(!(d.vectorKnown&2)||t.stack1A0!=d.look120))return false;
            if((t.stackKnown&4)&&(!(d.vectorKnown&4)||t.stack190!=d.up130))return false;
            if(t.matrixRows&&(t.stackKnown!=7||(d.vectorKnown&7)!=7))return false;
            for(unsigned r=0;r<4;++r)if((t.matrixRows&(1u<<r))&&t.matrix150[r][3]!=(r==3?0x3f800000u:0u))return false;
            if(t.matrixRows&8)for(unsigned k=0;k<3;++k)if(t.matrix150[3][k]!=d.position110[k])return false;
            if(t.firstSetterEntered&&(t.matrixRows!=15||(!h.setter.entered&&h.pc!=0x11f680u)))return false;
        }
        if(!setterCurrentNative(h)||!membershipCurrentNative(h))return false;
        if(h.attachmentEnrolled&&(!h.outer||!app.entityAuthority_.canRetire(h.outer->entity_)))return false;
        if(h.parentAttached&&!h.parentDetachedNative){
            const auto& e=h.outer->entity_;
            if(!h.parentBorrow||!h.parentLedger||!h.parentLedgerIndex||
               *h.parentLedgerIndex>=h.parentLedger->size()||
               (*h.parentLedger)[*h.parentLedgerIndex]!=&e||e.parent1C!=h.parentBorrow||
               (e.previous10?e.previous10->next14!=&e:h.parentBorrow->firstChild18!=&e))return false;
            const auto index=*h.parentLedgerIndex;const auto& ledger=*h.parentLedger;
            if((index&&e.previous10!=ledger[index-1])||
               e.next14!=(index+1<ledger.size()?ledger[index+1]:nullptr)||
               (e.next14&&e.next14->previous10!=&e))return false;
        }
        if(h.loaderPath.headerEntered_&&h.loaderPath.headerOuter_!=h.outer.get())return false;
        if(h.loaderPath.partialType0_&&h.loaderPath.partialType0_->payload.entered&&
           h.loaderPath.partialType0_->payload.application!=&app)return false;
        if(h.loaderPath.entered_&&(!h.descriptorBytesStored||!h.mode13Returned))return false;
        if(!h.loaderPath.currentNative(app.collisionDirectory36B0F0_,app.collisionName3E0730_,
            std::string_view(h.stackName.data(),h.stackNameKnown?h.stackNameKnown-1:0),h.loaderSuccessObserved?0x11bbb8u:h.pc))return false;
        if(h.mode13Returned&&(!h.entered||!h.allocationAttempted||!h.counted||!h.outer||!h.source04||!h.constructorReturned||!h.published))return false;
        if(!h.mode13Returned&&(h.categoryStored||h.descriptorBytesStored||h.foldedNameBytes||h.startupCategoryA4||h.modeReturn))return false;
        if(!h.entered)return !h.outer&&!h.source04&&!h.descriptorIdentity&&!h.application&&!h.generation&&
            !h.stackNameKnown&&!h.nameCompared&&!h.allocationAttempted&&!h.counted&&!h.constructorReturned&&!h.published&&!h.pc&&!h.dispatchFC&&!h.dispatch10;
        if(!w.sourceMode||!w.sourceTypeKnown||w.type00!=1||!w.sourceDescriptorPublished||
           !descriptor||!descriptor->loaded||
           (!h.returned&&(!w.descriptor08||h.descriptorIdentity!=w.descriptor08.get()))||
           h.application!=&app||h.generation!=generation||
           !h.nameCompared||w.collision04||w.nativeQuery||w.nativeAttempted)return false;
        const auto& d=*descriptor;
        if(!h.allocationAttempted)return !h.outer&&!h.source04&&!h.counted&&!h.constructorReturned&&!h.published&&
            !h.stackNameKnown&&!h.dispatchFC&&!h.dispatch10&&h.pc==0x1b55e4u&&d.nameKnown==5&&std::memcmp(d.name00.data(),"none",5)==0;
        if(h.stackNameKnown!=d.nameKnown||!h.stackNameKnown||h.stackNameKnown>h.stackName.size()||
           h.foldedNameBytes>=h.stackNameKnown||h.stackName[h.stackNameKnown-1])return false;
        for(std::size_t i=0;i<h.stackNameKnown;++i){
            auto expected=static_cast<unsigned char>(d.name00[i]);
            if(i<h.foldedNameBytes){if(expected>127)return false;if(expected>='A'&&expected<='Z')expected+=32;}
            if(static_cast<unsigned char>(h.stackName[i])!=expected)return false;
        }
        if(!h.outer)return h.published&&!h.source04&&!h.counted&&!h.constructorReturned&&!h.dispatchFC&&!h.dispatch10&&h.pc==0x11b570u;
        const auto& c=*h.outer;const auto& e=c.entity_;
        if(c.application_!=&app||c.appended_!=h.parentAttached||c.field00_||c.flags03_.knownMask!=7||(c.flags03_.value&7)||
           h.source04!=(h.published?h.outer.get():nullptr))return false;
        if(!h.counted)return !h.constructorReturned&&!h.published&&c.phase_==B::Phase::fresh&&!e.reference00&&!h.dispatchFC&&!h.dispatch10;
        if(e.reference00!=1||e.kind!=OriginalWreckComponentKind50LA::collision11B190||
           e.sessionGeneration!=generation||e.objectKey!=nativeKey(&e)||((!h.parentAttached||h.parentDetachedNative)&&(e.parent1C||e.previous10||e.next14))||
           e.firstChild18||e.mask20||e.descendants24||e.nativeFlags29||e.category28!=0xff||
           (e.flags29&15)!=3||h.dispatchFC!=0x3ccd00u||h.dispatch10!=0x3ccd08u||!app.objectCount423F60())return false;
        if(!h.constructorReturned)return !h.published&&c.phase_==B::Phase::entity&&c.type02_==0xff;
        if(!h.published||c.phase_!=B::Phase::complete||c.type02_==0xff||
           !c.constructorNullListsCurrent()||!h.loaderPath.shapeCurrentNative(c)||!c.owner9C_||*c.owner9C_||!c.bodyA0_||*c.bodyA0_||
           (c.flagsF8_&(h.mode13Returned?0x3fu:0x37u))!=(h.mode13Returned?12u:6u)||(!h.setter.radiusStored&&c.radius80_)||
           std::memcmp(c.copied40_.v,h.sentinel.data(),16)||std::memcmp(c.copied50_.v,h.sentinel.data(),16))return false;
        if(!h.mode13Returned)return h.pc==0x11b570u&&!c.categoryA4_&&!c.fieldA8_&&!c.fieldA9_;
        if(!h.categoryStored||h.modeReturn!=&e||h.startupCategoryA4!=app.collisionStartup3C81A0_.globals3E0708[3]||
           c.categoryA4_!=h.startupCategoryA4)return false;
        if(h.foldedNameBytes+1<h.stackNameKnown)return h.pc==0x30b158u&&!h.descriptorBytesStored&&
            static_cast<unsigned char>(d.name00[h.foldedNameBytes])>127&&!c.fieldA8_&&!c.fieldA9_;
        const char* const names[]={"floor","water","tube"};
        const std::uint32_t reads[]={0x1b5684u,0x1b56b4u,0x1b56e4u};
        for(unsigned i=0;i<3;++i)if(std::strstr(h.stackName.data(),names[i]))
            return h.pc==reads[i]&&!h.descriptorBytesStored&&!c.fieldA8_&&!c.fieldA9_;
        const auto material=(d.field184!=0&&(d.field184&0x80000000u)==0)?static_cast<std::uint8_t>(d.field184):0u;
        return (h.loaderPath.entered_||h.pc==0x11b920u)&&h.descriptorBytesStored&&c.fieldA8_==static_cast<std::uint8_t>(d.field180)&&c.fieldA9_==material;
    }
    static bool returnSourceSkippedCallback1B5590(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        auto& h=w.sourceCallback;
        if(pc!=0x1b5590u||h.entered||!generation||!sourceCallbackCurrentNative(w,app,generation)||
           !w.sourceMode||!w.sourceTypeKnown||!w.sourceDescriptorPublished||
           !w.descriptor08||!w.descriptor08->sourceFields||!w.descriptor08->sourceFields->loaded||
           w.descriptorRetired||w.collision04||w.nativeQuery||w.nativeAttempted)return false;
        const auto& d=*w.descriptor08->sourceFields;
        const bool type2=w.type00==2;
        const bool missing=w.type00==1&&d.nameKnown==5&&std::memcmp(d.name00.data(),"none",5)==0;
        if(!type2&&!missing)return false;
        h.entered=true;h.application=&app;h.generation=generation;h.descriptorIdentity=w.descriptor08.get();
        if(type2){
            h.skip=SourceCallback::Skip::type2;
            pc=h.pc=0x1b55b8u;h.descriptorIdentity=nullptr;w.descriptor08.reset();h.descriptorFreed=true;
            pc=h.pc=0x1b55c0u;h.descriptorCleared=true;w.descriptorRetired=true;
            pc=h.pc=0x1b55c8u;h.returnV0=0u;
        }else{
            h.skip=SourceCallback::Skip::missingName;h.nameCompared=true;
            pc=h.pc=0x1b55e4u;
            static constexpr char message[]="Unable to load model because the filename is missing.\n";
            //Native diagnostic adapter; guest1336C0 internals are not claimed.
            h.skipDiagnosticAttempted=true;h.skipDiagnosticWritten=std::fwrite(message,1,sizeof(message)-1,stdout);
            pc=h.pc=0x1b55f0u;h.returnV0=0u;
        }
        //Both branch to1B58C0: restore frame, JR1B58D0/delay1B58D4.
        pc=h.pc=0x1b58c0u;pc=h.pc=0x1b58d4u;
        h.returned=true;pc=h.pc=0x27bee8u;return true;
    }
    static bool beginSourceCallback1B5590(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,const std::array<std::uint32_t,4>& actual3E02C0,std::uint32_t& pc)noexcept {
        using B=OriginalCollisionBaseOwner50LA;auto& h=w.sourceCallback;
        if(h.entered||!sourceCallbackCurrentNative(w,app,generation)||!generation||
           !w.sourceMode||!w.sourceTypeKnown||w.type00!=1||!w.sourceDescriptorPublished||
           !w.descriptor08||!w.descriptor08->sourceFields||!w.descriptor08->sourceFields->loaded||
           w.collision04||w.nativeQuery||w.nativeAttempted||w.descriptorRetired||
           app.attachmentLock423F58()||!app.application10()||
           app.objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
        const auto& d=*w.descriptor08->sourceFields;
        if(!d.nameKnown||d.nameKnown>d.name00.size()||d.nameKnown>h.stackName.size()||
           d.name00[d.nameKnown-1]||std::memchr(d.name00.data(),0,d.nameKnown-1))return false;
        h.entered=true;h.descriptorIdentity=w.descriptor08.get();h.application=&app;h.generation=generation;
        //Actual type1 takes1B55B0 branch, not type2's early descriptor free.
        pc=h.pc=0x1b55d4;h.nameCompared=true; //3090E8 case-sensitive comparison.
        if(d.nameKnown==5&&std::memcmp(d.name00.data(),"none",5)==0){pc=h.pc=0x1b55e4;return false;}
        pc=h.pc=0x1b55f8;std::memcpy(h.stackName.data(),d.name00.data(),d.nameKnown);h.stackNameKnown=d.nameKnown;
        pc=h.pc=0x1b5600;h.allocationAttempted=true;h.sentinel=actual3E02C0;
        try{h.outer=std::make_unique<B>();}catch(...){h.outer.reset();}
        if(h.outer){
            SpringVector4_48N sentinel;std::memcpy(sentinel.v,actual3E02C0.data(),16);
            pc=h.pc=0x1b5610;
            const auto result=h.outer->construct11B190(app,generation,nativeKey(&h.outer->entity_),sentinel);
            h.counted=h.outer->phase_!=B::Phase::fresh;
            if(h.counted){h.dispatchFC=0x3ccd00u;h.dispatch10=0x3ccd08u;} //11B224/22C before fallible registry.
            if(result.status!=OriginalWreckComponentStatus50LA::complete){pc=h.pc=result.sourcePC;return false;}
            h.constructorReturned=true;
        }
        pc=h.pc=0x1b5618;h.source04=h.outer.get();h.published=true; //NULL allocation is published too.
        pc=h.pc=0x11b570;
        if(!h.source04)return false; //NULL published receiver faults at11B574; no mode/global effects.
        if(!h.outer->setMode13_11B570(app.collisionGlobalValid3E0730_))return false;
        h.modeReturn=&h.outer->entity_;h.mode13Returned=true; //141C20 returns Entity+8, not NULL.
        //Actual1B5628 therefore falls through. Same Application's genuine
        //3C8448-produced STATICOBJECT global, never the host category registry.
        h.startupCategoryA4=app.collisionStartup3C81A0_.globals3E0708[3];
        h.outer->categoryA4_=h.startupCategoryA4;h.categoryStored=true; //1B5640.
        for(;h.foldedNameBytes+1<h.stackNameKnown;++h.foldedNameBytes){
            const auto byte=static_cast<unsigned char>(h.stackName[h.foldedNameBytes]);
            pc=h.pc=0x30b158;
            //Actual30B158 uses the signed-byte ctype table3C6391. The
            //authenticated ASCII subset maps A..Z to+32, all others unchanged.
            //Do not guess negative signed-char table accesses for non-ASCII.
            if(byte>127)return false;
            if(byte>='A'&&byte<='Z')h.stackName[h.foldedNameBytes]=static_cast<char>(byte+32);
        }
        const char* const names[]={"floor","water","tube"}; //Original3ABF08/10/18.
        const std::uint32_t reads[]={0x1b5684u,0x1b56b4u,0x1b56e4u};
        for(unsigned i=0;i<3;++i)if(std::strstr(h.stackName.data(),names[i])){
            pc=h.pc=reads[i];return false; //Actual4342A0/A8/B0 producer not joined; no invented mask.
        }
        //Source signed-word >0 check, then source byte store. Low byte only;
        //do not replace with a host terrain/material lookup or booleanization.
        if(d.field184!=0&&(d.field184&0x80000000u)==0)h.outer->fieldA9_=static_cast<std::uint8_t>(d.field184);
        h.outer->fieldA8_=static_cast<std::uint8_t>(d.field180);h.descriptorBytesStored=true;
        pc=h.pc=0x11b920;
        h.loaderPath.begin11B920(app.collisionDirectory36B0F0_,app.collisionName3E0730_,
            std::string_view(h.stackName.data(),h.stackNameKnown-1),h.pc);
        pc=h.pc;return false; //Dedicated source path reached; generic132820 search is next.
    }
    static bool sourceCallbackSearchDomainCurrentNative(const Wrapper& w,
        const assets::OriginalStandardTextureResolver50LA* resolver,
        const assets::OriginalRaTrackSourceFile50LA* track,bool applicationDomainCurrent)noexcept {
        const auto& path=w.sourceCallback.loaderPath;
        return !path.searchAttempted_||(applicationDomainCurrent&&resolver&&track&&
            path.resolverIdentity_==resolver&&path.trackIdentity_==track);
    }
    static bool continueSourceCallbackSearchNative(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,std::shared_ptr<assets::OriginalStandardTextureResolver50LA> resolver,
        std::shared_ptr<assets::OriginalRaTrackSourceFile50LA> track,std::uint32_t& pc)noexcept {
        if(!sourceCallbackCurrentNative(w,app,generation)||w.sourceCallback.pc!=0x11ba2c)return false;
        auto& h=w.sourceCallback;
        h.loaderPath.continueSearch11BA2C(std::move(resolver),std::move(track),h.pc);
        if(h.pc==0x11ba34&&h.outer)h.loaderPath.continueHeader11BA34(*h.outer,h.pc);
        if(h.pc==0x124fb8&&h.outer)h.loaderPath.continueType124FB8(*h.outer,h.pc);
        if(h.pc==0x12500c&&h.outer)h.loaderPath.continuePayload125890(*h.outer,app,h.pc);
        if(h.pc==0x126410&&h.outer)h.loaderPath.continueResource125014(*h.outer,h.pc);
        if(h.pc==0x1250e4&&h.outer)h.loaderPath.continueAssociation1250E4(*h.outer,h.pc);
        pc=h.pc;return false; //11B920 receipt may be complete;1B5730 onward remains unjoined.
    }
    static bool observeSourceLoaderReturn1B5730(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        auto& h=w.sourceCallback;
        if(h.loaderSuccessObserved||h.pc!=0x11bbb8u||!sourceCallbackCurrentNative(w,app,generation)||
           !h.loaderPath.loaderReturned_||h.loaderPath.loaderReturnV0_!=1||h.source04!=h.outer.get())return false;
        h.loaderSuccessObserved=true;h.pc=pc=0x1b5750u; //Actual nonzero V0 branch, not a new return.
        h.pc=pc=0x1b5768u;return true; //Nonnull outer04, A1 becomes exact embedded Entity+8.
    }
    static bool attachSourceCallback1B577C(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,OriginalWreckComponent50LA& parent,
        std::vector<OriginalWreckComponent50LA*>& ledger,std::uint32_t zone,std::uint32_t& pc)noexcept {
        auto& h=w.sourceCallback;
        if(!h.loaderSuccessObserved||h.parentAttached||h.pc!=0x1b5768u||zone>=10||
           !sourceCallbackCurrentNative(w,app,generation)||!app.entityAuthority_.canRetire(parent))return false;
        //Reserve native ledger capacity before any source attachment mutation.
        //No callback stores an element address, so growth cannot invalidate older custody.
        for(std::size_t i=0;i<ledger.size();++i){const auto* e=ledger[i];
            if(!e||e->parent1C!=&parent||e->kind!=OriginalWreckComponentKind50LA::collision11B190||
               e->sessionGeneration!=generation||e->reference00!=1||e->mask20||e->descendants24||e->firstChild18||
               (i&&e->previous10!=ledger[i-1])||
               e->next14!=(i+1<ledger.size()?ledger[i+1]:nullptr))return false;
        }
        if(ledger.size()==ledger.max_size())return false;
        try{ledger.reserve(ledger.size()+1);}catch(...){return false;}
        h.parentZone354=zone;h.pc=pc=0x1b577cu;
        auto& entity=h.outer->entity_;auto& authority=app.entityAuthority_;
        if(!h.attachmentEnrolled){
            if(!authority.reserveAdditional(1)||!authority.canEnroll(entity,entity.objectKey)||!authority.enroll(entity))return false;
            h.attachmentEnrolled=true;
        }
        const auto result=authority.attachChildren(parent,entity,0,app.attachmentLock423F58());
        h.attachmentPC=result.sourcePC;
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        h.outer->appended_=true;h.parentBorrow=&parent;h.parentLedger=&ledger;
        h.parentLedgerIndex=ledger.size();ledger.push_back(&entity);
        h.parentAttached=true;h.pc=pc=0x1b5784u;return true;
    }
    //Actual t1=0 skips57D8..5804: normalize up and look independently,
    //then up cross look. No Gram-Schmidt, Physics facade, or setter success.
    //Bounded host binary32, separately rounded operations, FE_TONEAREST.
    //This does NOT execute/emulate EE VU Q/ACC, exception flags or FTZ/DAZ.
    static bool transformSourceCallback1B5784(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        auto& h=w.sourceCallback;auto& t=h.transform;
        if(h.pc!=0x1b5784u||t.entered||h.parentDetachedNative||
           !sourceCallbackCurrentNative(w,app,generation))return false;
        t.entered=true;const auto& d=*w.descriptor08->sourceFields;
        auto at=[&](std::uint32_t p){pc=h.pc=p;};
        at(0x1b5790);t.stack1B0={{0,0,0,0x3f800000u}};t.stackKnown=1;
        at(0x1b5794);if(!(d.vectorKnown&2))return false;
        at(0x1b57a4);t.stack1A0=d.look120;t.stackKnown|=2;
        at(0x1b57a8);if(!(d.vectorKnown&4))return false;
        at(0x1b57b0);t.stack190=d.up130;t.stackKnown|=4;
        at(0x1b57c4);if(!(d.vectorKnown&1))return false;
        at(0x1b5808);
        static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
            std::numeric_limits<float>::digits==24,"Source transform requires binary32");
        if(std::fegetround()!=FE_TONEAREST)return false;
        auto admitted=[](float v){return std::isfinite(v)&&(v==0.f||std::fpclassify(v)!=FP_SUBNORMAL);};
        //Reject operands BEFORE host evaluation; no exceptional operand is
        //silently normalized. Volatile destinations prohibit fused contraction.
        auto binary=[&](float a,float b,float& out,unsigned op,std::uint32_t p){
            at(p);if(!admitted(a)||!admitted(b))return false;
            //Binary64 admission catches tiny nonzero results before a host
            //FTZ mode could erase them. It does not supply the stored value.
            const double wide=op==0?double(a)*double(b):op==1?double(a)+double(b):double(a)-double(b);
            if(wide!=0.0&&std::fabs(wide)<double(std::numeric_limits<float>::min()))return false;
            volatile float v=op==0?a*b:op==1?a+b:a-b;
            if(!admitted(v))return false;out=v;return true;};
        std::array<float,4> up,look;std::array<float,3> right,product;
        std::memcpy(up.data(),t.stack190.data(),16);std::memcpy(look.data(),t.stack1A0.data(),16);
        auto normalize=[&](std::array<float,4>& v,std::uint32_t start){
            std::array<float,3> square;
            for(unsigned k=0;k<3;++k)if(!binary(v[k],v[k],square[k],0,start))return false;
            float sum;
            if(!binary(square[0],square[1],sum,1,start+4)||!binary(sum,square[2],sum,1,start+8))return false;
            at(start+12);if(!(sum>0.f))return false;
            volatile float root=std::sqrt(sum);if(!admitted(root)||root==0.f)return false;
            volatile float q=1.f/root;if(!admitted(q))return false;
            //581C/583C broadcast original W before XYZ is overwritten.
            float broadcast;if(!binary(0.f,v[3],broadcast,1,start+20))return false;
            for(unsigned k=0;k<3;++k)if(!binary(v[k],q,v[k],0,start+28))return false;
            v[3]=broadcast;return true;
        };
        if(!normalize(up,0x1b5808)||!normalize(look,0x1b5828))return false;
        //5848 VOPMULA writes all ACC lanes before584C VOPMSUB consumes.
        for(unsigned k=0;k<3;++k)if(!binary(up[(k+1)%3],look[(k+2)%3],product[k],0,0x1b5848))return false;
        for(unsigned k=0;k<3;++k){float reverse;
            if(!binary(up[(k+2)%3],look[(k+1)%3],reverse,0,0x1b584c)||
               !binary(product[k],reverse,right[k],2,0x1b584c))return false;}
        //5850/5854 same-register VU W-clears are represented symbolically.
        //vf5.w was unknown: do NOT read invented host backing to subtract it.
        //Likewise5858/585C/5860 clear W and5864 installs vf0.w=1.
        //This explicit zeroing adaptation is not a raw VF-register receipt.
        at(0x1b5850);at(0x1b5854);at(0x1b5858);at(0x1b585c);at(0x1b5860);at(0x1b5864);
        const std::array<float,3> rows[]={right,{{up[0],up[1],up[2]}},{{look[0],look[1],look[2]}}};
        for(unsigned r=0;r<3;++r){at(0x1b5868+4*r);
            for(unsigned k=0;k<3;++k)std::memcpy(&t.matrix150[r][k],&rows[r][k],4);
            t.matrix150[r][3]=0;t.matrixRows|=1u<<r;}
        at(0x1b5874);t.matrix150[3]=d.position110;t.matrix150[3][3]=0x3f800000u;t.matrixRows|=8;
        at(0x1b587c);t.firstSetterEntered=true;at(0x11f680);
        return false; //First call entered; its shape80/C0/radius stores not yet executed at this boundary.
    }
    //Bounded scalar projection of the actual11F680 VU/FPU operations. No
    //guest ACC/Q/FCSR parity is claimed; reject nonfinite/subnormal operands
    //and underflow before host FTZ can silently erase a source result.
    static bool setterBinary(float a,float b,float& out,bool multiply,std::uint32_t where,
        std::uint32_t& pc)noexcept {
        pc=where;
        auto admitted=[](float v){return std::isfinite(v)&&(v==0.f||std::fpclassify(v)!=FP_SUBNORMAL);};
        if(std::fegetround()!=FE_TONEAREST||!admitted(a)||!admitted(b))return false;
        const double wide=multiply?double(a)*double(b):double(a)+double(b);
        if(wide!=0.0&&std::fabs(wide)<double(std::numeric_limits<float>::min()))return false;
        volatile float v=multiply?a*b:a+b;
        if(!admitted(v))return false;out=v;return true;
    }
    static float setterFloat(std::uint32_t bits)noexcept {float x;std::memcpy(&x,&bits,4);return x;}
    static std::uint32_t setterBits(float x)noexcept {std::uint32_t bits;std::memcpy(&bits,&x,4);return bits;}
    static bool composeSetterRow(const std::array<std::array<std::uint32_t,4>,4>& input,
        const std::array<std::uint32_t,4>& local,std::array<std::uint32_t,4>& out,
        std::uint32_t first,std::uint32_t& pc)noexcept {
        //VMULAx, VMADDAy, VMADDAz, VMADDw; all four destination lanes.
        std::array<float,4> accum{};
        for(unsigned term=0;term<4;++term)for(unsigned lane=0;lane<4;++lane){float product;
            if(!setterBinary(setterFloat(input[term][lane]),setterFloat(local[term]),product,true,first+4*term,pc))return false;
            if(term){if(!setterBinary(accum[lane],product,accum[lane],false,first+4*term,pc))return false;}
            else accum[lane]=product;
        }
        for(unsigned lane=0;lane<4;++lane)out[lane]=setterBits(accum[lane]);return true;
    }
    static bool setterCenter(const std::array<std::array<std::uint32_t,4>,4>& input,const std::array<std::uint32_t,4>& center,
        std::array<std::uint32_t,4>& out,std::uint32_t& pc)noexcept {
        //11F834 VMOVE.w vf9,vf0 followed by XYZ-only x/y/z products and
        //translation ADD. W remains the actual vf0.w=1, not transformed W.
        std::array<float,3> accum{};
        for(unsigned term=0;term<3;++term)for(unsigned lane=0;lane<3;++lane){float product;
            if(!setterBinary(setterFloat(input[term][lane]),setterFloat(center[term]),product,true,0x11f838+4*term,pc))return false;
            if(term){if(!setterBinary(accum[lane],product,accum[lane],false,0x11f838+4*term,pc))return false;}
            else accum[lane]=product;
        }
        for(unsigned lane=0;lane<3;++lane){float v;
            if(!setterBinary(accum[lane],setterFloat(input[3][lane]),v,false,0x11f844,pc))return false;
            out[lane]=setterBits(v);}
        out[3]=0x3f800000u;return true;
    }
    static bool setterRadius(const std::array<std::uint32_t,4>& halves,std::uint32_t& bits,
        std::uint32_t& pc)noexcept {
        const float x=setterFloat(halves[0]),y=setterFloat(halves[1]),z=setterFloat(halves[2]);
        float xx,yy,zz,sum;
        if(!setterBinary(x,x,xx,true,0x11f7fc,pc)||!setterBinary(y,y,yy,true,0x11f800,pc)||
           !setterBinary(xx,yy,sum,false,0x11f804,pc)||!setterBinary(z,z,zz,true,0x11f808,pc)||
           !setterBinary(sum,zz,sum,false,0x11f808,pc))return false;
        pc=0x11f80c;volatile float radius=std::sqrt(sum);
        if(!std::isfinite(radius)||(radius!=0.f&&std::fpclassify(radius)==FP_SUBNORMAL))return false;
        bits=setterBits(radius);return true;
    }
    static bool setterReceiptCurrentNative(const SourceCallback& h,const SourceSetter& s)noexcept {
        if(!s.entered)return !s.previousWords&&!s.inputWords&&!s.composedRows&&!s.composedCalculatedRows&&
            !s.centerLanes&&!s.halfLanes&&!s.radiusStored&&!s.centerTransformed&&!s.returned;
        if(!h.transform.firstSetterEntered||!h.outer||!h.loaderPath.partialType0_||h.outer->shape88_!=0||
           s.inputMatrix!=h.transform.matrix150)return false;
        if(s.previousWords>16||s.inputWords>16||s.composedRows>4||s.composedCalculatedRows>4||s.centerLanes>4||s.halfLanes>4||
           (s.inputWords&&s.previousWords!=16)||(s.composedCalculatedRows&&s.inputWords!=16)||
           (s.composedRows&&s.composedCalculatedRows!=4)||
           (s.centerLanes&&s.composedRows!=4)||(s.halfLanes&&s.centerLanes!=4)||
           (s.radiusStored&&s.halfLanes!=4)||(s.centerTransformed&&!s.radiusStored)||
           (s.returned&&!s.centerTransformed))return false;
        const auto& shape=*h.loaderPath.partialType0_;
        for(unsigned row=0;row<s.composedCalculatedRows;++row)
            for(unsigned lane=0;lane<4;++lane)if(shape.payload.matrix40[4*row+lane].known!=15)return false;
        const auto& nodes=shape.payload.records.nodes;
        if((s.centerLanes||s.halfLanes)&&(!nodes.count||!nodes.raw||nodes.known.size()<36))return false;
        auto word=[&](unsigned off,std::uint32_t& value){value=0;for(unsigned k=0;k<4;++k){if(!nodes.known[off+k])return false;value|=std::uint32_t(nodes.raw[off+k])<<(8*k);}return true;};
        for(unsigned k=0;k<s.centerLanes&&k<3;++k){std::uint32_t value;if(!word(12+4*k,value))return false;}
        for(unsigned k=0;k<s.halfLanes;++k){std::uint32_t value=0x3f800000u;if(k<3&&!word(24+4*k,value))return false;if(s.stack50[k]!=value)return false;}
        if(s.centerTransformed&&s.transformedCenter[3]!=0x3f800000u)return false;
        return !s.radiusStored||s.stack15C==s.radiusBits;
    }
    static bool setterCurrentNative(const SourceCallback& h)noexcept {
        //Both occurrences retain independent immutable operation receipts.
        //Only the active occurrence owns current mutable outputs; successful
        //first-call outputs remain predecessor evidence during second refusal.
        const auto& first=h.setter;const auto& second=h.secondSetter;
        if(!setterReceiptCurrentNative(h,first)||!setterReceiptCurrentNative(h,second)||
           (second.entered&&!first.returned))return false;
        if(!first.entered){
            if(h.loaderPath.partialType0_){const auto& shape=*h.loaderPath.partialType0_;
                for(unsigned i=0;i<16;++i)if(shape.matrix00[i].known||shape.matrix80[i].known||shape.matrixC0[i].known)return false;}
            return !second.entered;
        }
        const bool later=second.entered;const auto& s=later?second:first;
        const auto& shape=*h.loaderPath.partialType0_;
        for(unsigned i=0;i<16;++i){
            //First copies unknown80; second copies first's fully known80.
            //No default zero, synthesized identity, or first-call assumption.
            const bool previousKnown=later&&i<s.previousWords;
            if(shape.matrixC0[i].known!=(previousKnown?15u:0u)||
               (previousKnown&&shape.matrixC0[i].bits()!=first.inputMatrix[i/4][i%4]))return false;
            const bool inputWritten=i<s.inputWords;
            if(shape.matrix80[i].known!=((later||inputWritten)?15u:0u)||
               ((later||inputWritten)&&shape.matrix80[i].bits()!=
                (inputWritten?s.inputMatrix:first.inputMatrix)[i/4][i%4]))return false;
            const bool composedWritten=i/4<s.composedRows;
            if(shape.matrix00[i].known!=((later||composedWritten)?15u:0u)||
               ((later||composedWritten)&&shape.matrix00[i].bits()!=
                (composedWritten?s.composedOutput:first.composedOutput)[i/4][i%4]))return false;
        }
        //A second-call refusal preserves first-call lanes until their exact
        //source stores, and preserves first radius until11F81C is reached.
        std::array<std::uint32_t,4> expected=later?first.transformedCenter:
            std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}};
        const auto& nodes=shape.payload.records.nodes;
        for(unsigned k=0;k<s.centerLanes&&k<3;++k){expected[k]=0;
            for(unsigned b=0;b<4;++b)expected[k]|=std::uint32_t(nodes.raw[12+4*k+b])<<(8*b);}
        if(s.centerLanes==4)expected[3]=0x3f800000u;
        if(s.centerTransformed)expected=s.transformedCenter;
        if(std::memcmp(h.outer->cachedCenter60_.v,expected.data(),16))return false;
        const bool radiusKnown=later||s.radiusStored;
        if(bool(h.outer->radius80_)!=radiusKnown)return false;
        if(radiusKnown){std::uint32_t actualRadius;std::memcpy(&actualRadius,&*h.outer->radius80_,4);
            if(actualRadius!=(s.radiusStored?s.radiusBits:first.radiusBits))return false;}
        //Integer/byte checks only. Never reexecute arithmetic or consult FE
        //mode in currentness: cleanup must survive any later FP environment.
        return s.returned?(h.membership.entered?(later&&(h.pc==h.membership.pc||
            (h.returned&&h.membership.returned&&h.pc==0x27bee8u))):h.pc==(later?0x1b5890u:0x1b5884u)):
            (h.pc>=0x11f680u&&h.pc<=0x11f84cu);
    }
    static bool membershipCurrentNative(const SourceCallback& h)noexcept {
        const auto& m=h.membership;
        if(!OriginalCollisionMembershipGuards50LA::allocationHistory(m.allocationGuard))return false;
        if(!m.entered)return !m.zoneIdentity&&!m.zone354&&!m.mode&&!m.pc&&!m.stack&&
            !m.stackCalled&&!m.stackReturned&&!m.flagsRead&&!m.stackSelected&&
            !m.scratchInitialized&&!m.nativeWithdrawn&&!m.flagsF8&&!m.allocationGuard.entered&&!m.bounds.entered;
        if(!h.secondSetter.returned||!m.zoneIdentity||m.zone354>=10||m.mode!=1||!m.stack||
           !m.stackCalled||!m.stack->preflightNative()||m.stackReturned!=m.stack->returned()||
           (m.flagsRead&&!m.stackReturned)||(m.stackSelected!=m.flagsRead)||
           (m.flagsRead&&(!(m.flagsF8&8u)||m.flagsF8!=h.outer->flagsF8_))||
           (m.allocationGuard.entered&&!m.stackSelected))return false;
        if(m.nativeWithdrawn!=m.stack->nativeWithdrawn_)return false;
        //This bounded first-callback route is the latest source observer.
        //Collection-wide release admission uses this predicate before clearing
        //World3CC or detaching parents: currentness alone is not LIFO authority.
        if(!m.nativeWithdrawn&&!m.primaryCalled&&!m.stack->canWithdrawUnregisteredNative())return false;
        if(m.primaryCalled&&(!m.world||m.outerIdentity!=h.outer.get()||!membershipPrimaryHistoryNative(h)||
           (!m.nativeWithdrawn&&!m.stack->detached_.entered&&!m.stack->canWithdrawActiveRegistrationNative())))return false;
        if(!m.stackReturned)return m.pc==m.stack->pc();
        if(!m.allocationGuard.returned)return m.pc==m.allocationGuard.pc;
        return m.scratchInitialized&&membershipBoundsCurrentNative(h);
    }
#include "original_world_collision_membership_bounds50la.inl"
#include "original_world_collision_membership_registration50la.inl"

    static bool beginSourceMembership1B58A8(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,const void* actualZone,std::uint32_t actual354,std::uint32_t& pc)noexcept {
        auto& h=w.sourceCallback;auto& m=h.membership;
        if(h.pc!=0x1b5890u||!h.secondSetter.returned||m.entered||!actualZone||actual354>=10||
           !sourceCallbackCurrentNative(w,app,generation))return false;
        //Native backing admission before any source constructor effects. A
        //host failure does not fabricate the source nullable30 allocation:
        //the F8bit8 branch never performs that allocation at all.
        try{m.stack=std::make_unique<OriginalSourceCollisionObserver50LA>();}catch(...){return false;}
        const auto at=[&](std::uint32_t p){pc=h.pc=m.pc=p;};
        m.entered=true;m.zoneIdentity=actualZone;m.zone354=actual354;
        at(0x1b58a8u);m.mode=1; //Actual delay1B58AC, not the physics caller's mode2.
        at(0x120070u);m.stackCalled=true;at(0x280890u);
        if(!m.stack->construct280890(app.garageObserverShared_)){at(m.stack->pc());return false;}
        m.stackReturned=true;at(0x12009cu);m.flagsRead=true;m.flagsF8=h.outer->flagsF8_;
        //11B570(mode13) already proves bit8. No existing E0 read, heap
        //observer allocation, pointer store, or foreign observer destruction.
        if(!(m.flagsF8&8u))return false;
        m.stackSelected=true;at(0x1200f0u);
        if(!OriginalCollisionMembershipGuards50LA::beginAllocation(m.allocationGuard)){
            at(m.allocationGuard.pc);return false;}
        if(!OriginalCollisionMembershipGuards50LA::finishAllocation(m.allocationGuard,m.stack->identity()!=nullptr)){
            at(m.allocationGuard.pc);return false;}
        at(0x12015cu);m.center={{0,0,0,0x3f800000u}};
        at(0x120164u);m.half=m.center;m.scratchInitialized=true;
        at(0x1202e0u);
        if(!runSourceMembershipBounds1202E0(h,pc))return false;
        //Actual120170 supplies zero overlap radius to2809C0. The World
        //caller supplies its exact +34/World4CC resolvers in a separate join.
        at(0x2809c0u);return false; //Registration continuation is caller-qualified.
    }
    static bool firstSourceSetter11F680(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        auto& h=w.sourceCallback;
        if(h.pc!=0x11f680u||h.setter.entered||!sourceCallbackCurrentNative(w,app,generation))return false;
        return runSourceSetter11F680(h,h.setter,0x1b5884u,pc);
    }
    static bool secondSourceSetter1B5888(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        auto& h=w.sourceCallback;
        if(h.pc!=0x1b5884u||!h.setter.returned||h.secondSetter.entered||
           !sourceCallbackCurrentNative(w,app,generation))return false;
        //1B5884 reloads the same live wrapper04 outer. The1B5888 delay
        //slot1B588C passes the unchanged callback sp+150, not shape00/80.
        //This is a new source invocation, including new arithmetic receipts.
        return runSourceSetter11F680(h,h.secondSetter,0x1b5890u,pc);
    }
    static bool runSourceSetter11F680(SourceCallback& h,SourceSetter& s,
        std::uint32_t returnPC,std::uint32_t& pc)noexcept {
        s.entered=true;s.inputMatrix=h.transform.matrix150;
        auto at=[&](std::uint32_t p){pc=h.pc=p;};at(0x11f680);
        at(0x11f698);if(h.outer->shape88_!=0)return false;
        at(0x11f6c8);if(!h.loaderPath.partialType0_)return false;
        auto& shape=*h.loaderPath.partialType0_;
        for(unsigned pair=0;pair<8;++pair)for(unsigned k=0;k<2;++k){at(0x11f6e4+4*k);
            const unsigned i=2*pair+k;shape.matrixC0[i]=shape.matrix80[i];++s.previousWords;}
        for(unsigned pair=0;pair<8;++pair)for(unsigned k=0;k<2;++k){at(0x11f714+4*k);
            const unsigned i=2*pair+k;shape.matrix80[i].store(s.inputMatrix[i/4][i%4]);++s.inputWords;}
        std::array<std::array<std::uint32_t,4>,4> local,composed;
        auto loadRow=[&](unsigned row,std::uint32_t where){at(where);
            for(unsigned lane=0;lane<4;++lane){const auto& word=shape.payload.matrix40[4*row+lane];
                if(word.known!=15)return false;local[row][lane]=word.bits();}return true;};
        auto composeRow=[&](unsigned row,std::uint32_t where){std::uint32_t numericPC;
            if(!composeSetterRow(s.inputMatrix,local[row],composed[row],where,numericPC)){
                at(numericPC);return false;}s.composedOutput[row]=composed[row];++s.composedCalculatedRows;return true;};
        //Original loads run one local row ahead of the arithmetic. Preserve
        //that order even if a successful short read left a lane unknown.
        if(!loadRow(0,0x11f730)||!loadRow(1,0x11f744)||!composeRow(0,0x11f748)||
           !loadRow(2,0x11f758)||!composeRow(1,0x11f75c)||!loadRow(3,0x11f76c)||
           !composeRow(2,0x11f770)||!composeRow(3,0x11f780))return false;
        for(unsigned row=0;row<4;++row){at(0x11f790+4*row);
            for(unsigned lane=0;lane<4;++lane)shape.matrix00[4*row+lane].store(composed[row][lane]);++s.composedRows;}
        const auto& nodes=shape.payload.records.nodes;
        at(0x11f7b0); //Bound before the original unguarded first-node dereference.
        if(!nodes.count||!nodes.raw||nodes.known.size()<36)return false;
        auto word=[&](unsigned off,std::uint32_t& value){value=0;for(unsigned k=0;k<4;++k){if(!nodes.known[off+k])return false;value|=std::uint32_t(nodes.raw[off+k])<<(8*k);}return true;};
        std::array<std::uint32_t,4> center;
        for(unsigned lane=0;lane<3;++lane){at(0x11f7b0+8*lane);if(!word(12+4*lane,center[lane]))return false;
            at(0x11f7b4+8*lane);std::memcpy(&h.outer->cachedCenter60_.v[lane],&center[lane],4);++s.centerLanes;}
        at(0x11f7c8);center[3]=0x3f800000u;std::memcpy(&h.outer->cachedCenter60_.v[3],&center[3],4);++s.centerLanes;
        for(unsigned lane=0;lane<3;++lane){at(0x11f7d4+8*lane);std::uint32_t value;if(!word(24+4*lane,value))return false;
            at(0x11f7d8+8*lane);s.stack50[lane]=value;++s.halfLanes;}
        at(0x11f7ec);s.stack50[3]=0x3f800000u;++s.halfLanes;
        std::uint32_t numericPC,radiusBits;
        if(!setterRadius(s.stack50,radiusBits,numericPC)){at(numericPC);return false;}
        at(0x11f810);s.stack15C=radiusBits;
        at(0x11f81c);s.radiusBits=s.stack15C;h.outer->radius80_=setterFloat(radiusBits);s.radiusStored=true;
        std::array<std::uint32_t,4> transformed;
        if(!setterCenter(s.inputMatrix,center,transformed,numericPC)){at(numericPC);return false;}
        at(0x11f84c);s.transformedCenter=transformed;
        std::memcpy(h.outer->cachedCenter60_.v,s.transformedCenter.data(),16);s.centerTransformed=true;
        s.returned=true;at(returnPC);return true; //No120070 membership or descriptor retirement.
    }
    static bool finishSourceCallback1B58B0(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        auto& h=w.sourceCallback;
        if(pc!=0x120224u||h.pc!=pc||!h.membership.returned||h.returned||
           !sourceCallbackCurrentNative(w,app,generation)||!w.descriptor08||
           !w.descriptor08->sourceFields)return false;
        const auto& d=*w.descriptor08->sourceFields;
        if(!d.loaded||d.nameKnown==0||d.nameKnown>d.name00.size()||(d.vectorKnown&7)!=7)return false;
        //Capture only initialized source knowledge needed by immutable history
        //checks. In particular, do not copy unwritten name tail or other raw
        //descriptor bytes. No transform, loader or membership operation repeats.
        h.descriptorHistory.emplace();auto& saved=*h.descriptorHistory;
        saved.nameKnown=d.nameKnown;std::memcpy(saved.name00.data(),d.name00.data(),d.nameKnown);
        saved.position110=d.position110;saved.look120=d.look120;saved.up130=d.up130;
        saved.vectorKnown=7;saved.field180=d.field180;saved.field184=d.field184;saved.loaded=true;
        pc=h.pc=0x1b58b0u; //JAL2E8B00, delay1B58B4 loads this same wrapper+08.
        h.descriptorIdentity=nullptr;w.descriptor08.reset();
        h.descriptorFreed=true;w.descriptorRetired=true;
        pc=h.pc=0x1b58b8u;h.returnV0=1u;
        pc=h.pc=0x1b58bcu;h.descriptorCleared=true; //Source SW zero,+08.
        //1B58C0..CC restore caller registers; JR1B58D0/+D4 releases frame.
        //Neither wrapper+04, payload/resource, parent nor durable membership is freed.
        pc=h.pc=0x27bee8u;h.returned=true;return true;
    }
    static bool withdrawSourceCallbackMembershipNative(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation)noexcept {
        auto& h=w.sourceCallback;auto& m=h.membership;
        if(!sourceCallbackCurrentNative(w,app,generation))return false;
        if(!m.entered||m.nativeWithdrawn)return true;
        if(m.stack->detached_.entered){
            if(!m.stack->withdrawDetachedMembershipNative())return false;
            //The helper atomically retired the native observer with its registry.
            //Synchronize every caller-owned receipt before any root may retire.
            m.primaryNativeWithdrawn=true;m.slotWithdrawn=m.inserted;m.methodPinWithdrawn=m.methodPinned;
            for(std::size_t i=0;i<m.portals.count;++i){auto& e=m.portals.edges[i];
                e.slotWithdrawn=e.inserted;e.linkReleased=e.allocationRecorded;e.methodPinWithdrawn=e.methodPinned;}
            m.portals.graphPin.reset();m.portals.nativeWithdrawn=true;
            m.nativeWithdrawn=true;return true;
        }
        if(!withdrawSourceMembershipPrimaryNative(h)||!m.stack->withdrawNative())return false;
        m.nativeWithdrawn=true;return true; //Native cancellation, never120200/280920.
    }
    static bool withdrawSourceCallbackParentNative(Wrapper& w,OriginalApplicationEntities50LA& app,
        std::uint64_t generation)noexcept {
        auto& h=w.sourceCallback;
        if(!sourceCallbackCurrentNative(w,app,generation))return false;
        if(!h.parentAttached||h.parentDetachedNative)return true;
        auto& e=h.outer->entity_;
        //Native tail withdrawal only. This mask-zero child has no descendants;
        //there is no guest141A70, ref decrement, or ancestor-mask reset here.
        if(app.attachmentLock423F58()||e.mask20||e.descendants24||e.firstChild18||e.next14||
           !h.parentLedger||h.parentLedger->empty()||h.parentLedger->back()!=&e||
           !h.parentLedgerIndex||*h.parentLedgerIndex+1!=h.parentLedger->size())return false;
        if(e.previous10)e.previous10->next14=nullptr;else h.parentBorrow->firstChild18=nullptr;
        e.parent1C=nullptr;e.previous10=nullptr;h.parentLedger->pop_back();
        h.parentLedger=nullptr;h.parentLedgerIndex.reset();h.parentBorrow=nullptr;h.parentDetachedNative=true;return true;
    }
    static void withdrawSourceCallbackNative(Wrapper& w,OriginalApplicationEntities50LA& app)noexcept {
        auto& h=w.sourceCallback;
        //Called only after collection-wide preflight/count admission. Neither
        //source11B4A0 nor the completed1B58B0 descriptor-free path is replayed.
        if(!withdrawSourceCallbackMembershipNative(w,app,h.generation)||
           !withdrawSourceCallbackParentNative(w,app,h.generation))std::terminate();
        if(h.attachmentEnrolled){if(!app.entityAuthority_.retire(h.outer->entity_))std::terminate();h.attachmentEnrolled=false;}
        if(!h.loaderPath.withdrawSearchNative())std::terminate();
        if(h.counted){h.outer->entity_.reference00=0;--app.objectCount423F60();
            h.outer->phase_=OriginalCollisionBaseOwner50LA::Phase::released;h.counted=false;}
        h.source04=nullptr;h.outer.reset(); //Wrapper/descriptor storage leaves immediately afterward.
    }
    struct Zone {
        OriginalWreckComponent50LA entity;
        std::uint32_t count30=0;
        std::vector<Wrapper> entries34;
        bool counted=false;
        bool sourceMode=false,sourceEnrolled=false,sourceCountKnown=false,sourceArrayReturned=false;
        std::uint32_t sourceVtable08=0;
        std::unique_ptr<Wrapper*[]> sourceEntries34;
        std::size_t sourceEntriesKnown=0;
    };
    bool sourceMode_=false;
    std::array<std::unique_ptr<Zone>,10> sourceZones_;
    std::array<std::vector<std::unique_ptr<Wrapper>>,10> sourceWrappers_;
    std::unique_ptr<OriginalWreckComponent50LA*[]> sourceArray3CC_;
    std::size_t sourceArrayKnown_=0;
    bool sourceArrayReturned_=false;
    std::array<bool,10> sourceGroupAllocationReturned_{},sourceLogAttempted_{};
    std::array<std::size_t,10> sourceLogWritten_{};
    std::size_t sourceCountedGroups()const noexcept {
        std::size_t n=0;for(const auto& p:sourceZones_)if(p&&p->counted)++n;return n;
    }
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::array<Zone,10> zones_;
    OriginalWorldCollisionObjects50LA()noexcept=default;
    bool prepareBacking(std::shared_ptr<const ra_cold_sources50LA::Owner> source)noexcept {
        if(sourceMode_||source_||!source||!source->sourcesDecoded())return false;
        const auto& s=*source->sourcesDecoded();const auto& w=s.world;
        if(w.zones.size()!=10||w.instances.size()!=1261)return false;
        try {
            std::size_t wi=0,ci=0;
            for(std::size_t z=0;z<10;++z){const auto& authored=w.zones[z];
                if(authored.zone!=z+1||!authored.models||authored.parameter_block>=w.parameters.blocks.size())return false;
                // Actual RA fallback2764EC..276508: no explicit count means
                // NumberOfStaticModels and ModelN names, never ObjectN names.
                for(const auto& f:w.parameters.blocks[authored.parameter_block].fields)
                    if(f.name=="numberofcollisionobjects"||f.name=="collisionobjects")return false;
                auto& zone=zones_[z];zone.entries34.resize(authored.models);
                for(auto& wrapper:zone.entries34){
                    if(wi>=w.instances.size())return false;
                    const auto& in=w.instances[wi++];
                    if(in.zone!=z+1||in.ordinal!=&wrapper-zone.entries34.data()+1||
                       in.parameter_block>=w.parameters.blocks.size())return false;
                    const auto& b=w.parameters.blocks[in.parameter_block];if(b.name!=in.section)return false;
                    bool type=false,slt=false,lighting=false;
                    auto d=std::make_unique<Descriptor>();d->source=&in;
                    for(const auto& f:b.fields){
                        if(f.name=="statictype"){if(type||f.value!="Standard")return false;type=true;}
                        else if(f.name=="slt"){
                            if(slt)return false;slt=true;const auto dot=f.value.find('.');
                            if(dot==std::string::npos||!dot||dot>50)return false;
                            d->name00=f.value.substr(0,dot);
                        }else if(f.name=="uselighting"){
                            if(lighting||(f.value!="T"&&f.value!="F"))return false;
                            lighting=true;d->useLighting140=f.value=="T";
                        }else if(f.name=="col"||f.name=="usedynamiclighting"||f.name=="issmoker"||
                                 f.name=="smokeoffset"||f.name=="smokevelocity")return false;
                    }
                    if(!slt)return false;
                    for(unsigned a=0;a<3;++a){
                        const double values[]={in.position[a],in.look[a],in.up[a]};
                        float* dest[]={&d->position110[a],&d->look120[a],&d->up130[a]};
                        for(unsigned n=0;n<3;++n){const float v=static_cast<float>(values[n]);
                            if(!std::isfinite(v)||double(v)!=values[n])return false;*dest[n]=v;}
                    }
                    std::string member="geometry\\"+d->name00+".col";
                    for(char& c:member)if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
                    if(member!=in.collision_member)return false;
                    std::size_t archiveMatches=0;
                    for(const auto& entry:s.archive.entries){
                        if(entry.name.size()!=member.size())continue;
                        bool equal=true;
                        for(std::size_t n=0;n<member.size();++n){char c=entry.name[n];
                            if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
                            if(c!=member[n]){equal=false;break;}}
                        if(equal)++archiveMatches;
                    }
                    if(archiveMatches>1)return false;
                    if(ci<s.collisions.size()&&s.collisions[ci].zone==in.zone&&s.collisions[ci].ordinal==in.ordinal){
                        if(s.collisions[ci].member!=in.collision_member)return false;
                        wrapper.decodedCollision=&s.collisions[ci++];
                    }
                    if(bool(wrapper.decodedCollision)!=(archiveMatches==1))return false;
                    wrapper.source=&in;wrapper.descriptor08=std::move(d);
                }
            }
            if(wi!=w.instances.size()||ci!=s.collisions.size())return false;
        }catch(...){return false;}
        source_=std::move(source);return true;
    }
    static std::uint64_t nativeKey(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    Wrapper* nativeSlot(std::size_t zone,std::size_t row)noexcept {
        return !sourceMode_&&zone<zones_.size()&&row<zones_[zone].entries34.size()?
            &zones_[zone].entries34[row]:nullptr;
    }
    // Private caller authenticates actual current directory/category producer
    // and candidate owner. Terrain comes from the EXISTING272520 native alias,
    // not stale cold Descriptor180. No global authority enrollment here: the
    // World caller owns the explicit private typed parent/child forest.
    bool beginNativeQuery27B040(std::size_t zone,std::size_t row,
        OriginalApplicationEntities50LA& app,std::uint64_t generation,
        const Type2StaticCandidate49X* candidate,std::size_t resourceIndex,
        std::uint32_t sourceCategory,std::uint32_t& pc)noexcept {
        pc=0x27b0ac;auto* w=nativeSlot(zone,row);
        if(!w||!source_||!w->source||!w->descriptor08||w->descriptorRetired||
           w->descriptor08->source!=w->source||w->type00!=1||w->nativeAttempted||
           w->nativeQuery||w->collision04||!generation||app.attachmentLock423F58()||
           !app.application10()||!zones_[zone].counted||
           zones_[zone].entity.sessionGeneration!=generation||
           app.objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
        const auto& d=*w->descriptor08;
        if(d.name00.empty()||d.name00=="none"||d.field184!=0||
           bool(candidate)!=bool(w->decodedCollision))return false;
        if(candidate){
            const auto* decoded=source_->sourcesDecoded();
            if(!decoded||resourceIndex>=decoded->collisions.size()||
               w->decodedCollision!=&decoded->collisions[resourceIndex]||
               w->decodedCollision->zone!=w->source->zone||
               w->decodedCollision->ordinal!=w->source->ordinal||
               candidate->candidate.collision.geometry.mesh!=&w->decodedCollision->mesh||
               !candidate->terrainDescriptor180||*candidate->terrainDescriptor180>1u)
                return false;
        }else if(resourceIndex!=std::numeric_limits<std::size_t>::max()||d.field180!=0)return false;
        w->nativeAttempted=true; // Allocation failure terminal; never recount.
        try {
            w->nativeQuery=std::make_unique<NativeQuery>();auto& q=*w->nativeQuery;
            q.application=&app;q.source=w->source;q.candidate=candidate;
            q.name=d.name00;q.member=w->source->collision_member;
            q.resourceIndex=resourceIndex;q.sourceCategoryA4=sourceCategory;
            q.liveTerrain180=candidate?candidate->terrainDescriptor180:nullptr;
            q.sourceTerrainA8=static_cast<std::uint8_t>(q.liveTerrain180?*q.liveTerrain180:d.field180);
        }catch(...){if(w->nativeQuery){w->nativeQuery->terminal=true;w->nativeQuery->failurePC=pc;}return false;}
        auto& q=*w->nativeQuery;
        pc=0x141300;
        const auto result=initializeOriginalWreckComponent50LA(q.entity,
            OriginalWreckComponentKind50LA::collision11B190,generation,nativeKey(&q.entity),
            true,app.objectCount423F60());
        if(result.status!=OriginalWreckComponentStatus50LA::complete){q.terminal=true;q.failurePC=pc;return false;}
        q.counted=true;
        // Real constructor type service, before wrapper04 publication. Failure
        // retains its already counted prefix; no native candidate tag rewrite.
        pc=0x11b270;const auto type=app.types383408().collisionObject11B270();
        if(!type.okay()){q.terminal=true;q.failurePC=type.sourcePC;return false;}
        q.sourceType02=type.type;q.typed=true;
        pc=0x27b0c4;w->collision04=&q.entity;
        //11B570(0xD): bit3=1,bit1=0,bit2=1. No fullF8 or source
        //global3E0730 publication claimed by these private constructor fields.
        q.flagsF8KnownMask=0x3f;q.flagsF8Value=0x0c;q.flagsApplied=true;
        pc=0x27b118;return true;
    }
    bool nativeQueryCurrent27B040(const Wrapper& w)const noexcept {
        if(!w.nativeQuery)return false;const auto& q=*w.nativeQuery;const auto& e=q.entity;
        if(!q.application||q.source!=w.source||q.retired||!q.counted||
           e.kind!=OriginalWreckComponentKind50LA::collision11B190||
           e.objectKey!=nativeKey(&e)||!e.sessionGeneration||e.reference00!=1||
           e.category28!=0xff||e.flags29Value()!=3||e.nativeFlags29||
           e.firstChild18||e.mask20||e.descendants24)return false;
        if(q.typed&&w.collision04!=&e)return false;
        if(q.candidate){
            if(!w.decodedCollision||q.candidate->candidate.collision.geometry.mesh!=&w.decodedCollision->mesh||
               q.liveTerrain180!=q.candidate->terrainDescriptor180||!q.liveTerrain180||
               static_cast<std::uint8_t>(*q.liveTerrain180)!=q.sourceTerrainA8)return false;
        }else if(w.decodedCollision||q.liveTerrain180)return false;
        return true;
    }
    // The caller invokes this immediately BEFORE actual existing resource
    // load11B920, preserving a failed allocation/publication for cleanup.
    bool beginNativeResource11B920(std::size_t zone,std::size_t row,std::uint32_t& pc)noexcept {
        pc=0x27b11c;auto* w=nativeSlot(zone,row);
        if(!w||!nativeQueryCurrent27B040(*w))return false;auto& q=*w->nativeQuery;
        if(q.terminal||!q.typed||!q.flagsApplied||q.resourceStarted||q.loaded||q.retired)return false;
        q.resourceStarted=true;return true;
    }
    // Private parent calls only after actual resource load returned, not from
    // prepared(). On false, retain all partial resource/query ownership.
    bool finishNativeResource11B920(std::size_t zone,std::size_t row,bool returned,
        std::uint32_t failurePC,std::uint32_t& pc)noexcept {
        pc=0x27b124;auto* w=nativeSlot(zone,row);
        if(!w||!nativeQueryCurrent27B040(*w))return false;auto& q=*w->nativeQuery;
        if(!q.resourceStarted||q.loaded||q.terminal||!q.candidate)return false;
        if(!returned){q.terminal=true;q.failurePC=failurePC?failurePC:0x11b920;pc=q.failurePC;return false;}
        q.loaded=true;return true;
    }
    bool canRetireNativeQuery11B4A0(const Wrapper& w)const noexcept {
        if(!w.nativeQuery)return !w.collision04;
        const auto& q=*w.nativeQuery;const auto& e=q.entity;
        if(q.retired)return !q.counted&&!w.collision04&&!e.reference00;
        if(q.observerSharedConstructed||q.observerToken||q.appended||q.registered||e.parent1C||e.previous10||e.next14||e.firstChild18||
           e.mask20||e.descendants24||(q.resourceStarted&&!q.resourceWithdrawn))return false;
        if(!q.counted)return !e.reference00&&!w.collision04;
        return q.application&&q.application->objectCount423F60()>0&&
            !q.application->attachmentLock423F58()&&e.reference00==1&&
            e.kind==OriginalWreckComponentKind50LA::collision11B190&&
            e.objectKey==nativeKey(&e)&&e.category28==0xff&&e.flags29Value()==3&&
            !e.nativeFlags29&&(!w.collision04||w.collision04==&e);
    }
    bool retireNativeQuery11B4A0(std::size_t zone,std::size_t row,std::uint32_t& pc)noexcept {
        pc=0x11b4a0;auto* w=nativeSlot(zone,row);
        if(!w||!canRetireNativeQuery11B4A0(*w))return false;
        if(!w->nativeQuery)return true;auto& q=*w->nativeQuery;
        if(q.retired)return true;
        // Owned e0, dynamic identity lists and callbacks were never introduced
        // in this bounded route. Actual resource withdrawal must precede this.
        if(q.counted){--q.entity.reference00;--q.application->objectCount423F60();q.counted=false;}
        w->collision04=nullptr;q.candidate=nullptr;q.liveTerrain180=nullptr;q.application=nullptr;
        q.retired=true;pc=0x1413e0;return true;
    }
    // The caller's SAME resolver + mounted archive provider must both prove
    // this exact member absent immediately before entry. Original guest fallback
    // history remains unknown: this is the bounded native negative-load route.
    bool completeNativeNegativeLoad27B12C(std::size_t zone,std::size_t row,
        const std::string& authenticatedAbsentMember,std::uint32_t& pc)noexcept {
        pc=0x27b12c;auto* w=nativeSlot(zone,row);
        if(!w||!nativeQueryCurrent27B040(*w))return false;auto& q=*w->nativeQuery;
        if(q.candidate||q.resourceIndex!=std::numeric_limits<std::size_t>::max()||
           !q.resourceStarted||q.loaded||q.terminal||q.appended||q.registered||
           authenticatedAbsentMember!=q.member)return false;
        q.negativeLoadReturned=true;q.resourceWithdrawn=true; // No acquired shape on file miss.
        if(!retireNativeQuery11B4A0(zone,row,pc))return false;
        pc=0x27b2a4;w->descriptor08.reset();w->descriptorRetired=true;pc=0x27b2b0;return true;
    }
    bool freeNativeDescriptor27B2A4(std::size_t zone,std::size_t row,std::uint32_t& pc)noexcept {
        pc=0x27b2a4;auto* w=nativeSlot(zone,row);
        if(!w||!nativeQueryCurrent27B040(*w)||!w->descriptor08||w->descriptorRetired)return false;
        const auto& q=*w->nativeQuery;
        if(q.terminal||!q.loaded||!q.appended||!q.registered||q.resourceWithdrawn)return false;
        w->descriptor08.reset();w->descriptorRetired=true;pc=0x27b2b0;return true;
    }

public:
    ~OriginalWorldCollisionObjects50LA()noexcept {
        for(const auto& p:sourceZones_)if(p&&p->counted)std::terminate();
        for(const auto& z:zones_){
            if(z.counted)std::terminate();
            for(const auto& w:z.entries34)if(w.collision04||
                (w.nativeQuery&&w.nativeQuery->counted))std::terminate();
        }
    }
};
}
