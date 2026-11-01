#pragma once
#include "ra_cold_sources50la.h"
#include "original_world_collision_objects50la.h"
#include "original_world_physics_index50la.h"
#include "../../src/game/original_entity_chains50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/original_racer_model_packets50la.h"
#include "../../src/game/original_source_standard_smoker50la.h"
#include "../../src/game/original_source_physics_object50la.h"
#include "../../src/game/original_source_physics_model50la.h"
#include "../../src/game/original_source_physics_body50la.h"
#include "../../src/game/original_source_physics_collision50la.h"
#include "../../src/game/original_source_physics_contact50la.h"
#include "../../src/game/original_source_physics_detector50la.h"
#include "../../src/assets/original_source_psg_missing50la.h"
#include "../../src/game/original_ui_user_rotate50kg.h"
#include "../../src/game/original_standard_placement_globals50la.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "../../src/game/model_bounds49o.h"
#include "../../src/assets/original_standard_model_resource50la.h"
#include "../../src/assets/original_standard_material_table50la.h"
#include <limits>
#include <optional>
#include <vector>
#include <thread>

namespace rr::game {class OriginalTrackParticleCold50LA;}
namespace ra_pod_scene50LA {class Owner;}
namespace ra_static_world50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_static_model_cold50LA {
namespace g=rr::game;namespace a=rr::assets;
// This owns real cold construction prefixes. The source calls28E8A0 before
//27AE38 appends the model Entity to StaticModels. The explicit observer stage
// below performs these in source order against the SAME visibility lifecycle.
// No selected191, traversal or renderer capability is issued.
// Explicit continuation owns genuine CPU packets and authored placement.
class Owner final {
    friend class ra_static_world50LA::Owner;
    friend class ra_pod_scene50LA::Owner;
    friend class ra_cold_aggregate50LA::Owner;
    using C=g::OriginalWreckComponent50LA;
    using K=g::OriginalWreckComponentKind50LA;
    using I=g::SceneNodeIndex49P;
public:
    struct Diagnostic {
        const char* failure=nullptr;
        std::size_t placement=0,parents=0,models=0,unresolvedSlots=0;
        g::OriginalRacerModelSceneResult50LA scene{};
        a::GeometryDiagnostic49O hierarchy{};
        a::OriginalStandardResourceDiagnostic50LA resource{};
        a::OriginalStandardTextureDiagnostic50LA cache{};
        g::OriginalRacerModelPacketResult50LA packets{};
        g::LiveZoneResult50EN visibility{};
    };
    struct Counts {std::size_t parents=0,models=0,unresolvedSlots=0;};
    // Supported cold operations, not per-instance successful-stage receipts.
    static constexpr bool observerRegistrationAdmitted=true;
    static constexpr bool modelEntityAppendAdmitted=true;
    static constexpr bool selectedQueuesAdmitted=false;
private:
    struct Input {
        std::size_t worldInstance=0;
        std::shared_ptr<const a::OriginalStandardModelResource50LA> payload;
        a::OriginalStandardMaterialReferences50LA* materials=nullptr;
    };
    struct SourceDescriptor27A100 {
        std::array<char,260> name00,name180;
        std::size_t name00Known=0,name180Known=0;
        std::array<std::uint32_t,4> position110,look120,up130,smokeOffset150,smokeVelocity160;
        std::uint32_t vtable170=0;
        std::optional<std::int32_t> useLighting140,dynamic144,shadow148,smoker14C;
        std::uint32_t vectorKnown=0;bool constructed=false,loaded=false;
    };
    struct Descriptor {
        //Exclusive actual descriptor owner; legacy decoded fields below are NOT readiness.
        std::optional<SourceDescriptor27A100> sourceFields;
        const a::WorldInstance48F* source=nullptr;
        std::array<float,3> position{},look{},up{};
        bool useDynamicLighting144=false;
        //Unknown native metadata until genuine271B4C reaches THIS descriptor.
        std::optional<std::int32_t> allowShadowReceiving148;
        std::string modelName180;
        std::optional<bool> isSmoker14C; // unknown until actual default/read join
    };
    //Sole typed backing for the actual nullable210 allocation. Capacity64 is
    //native storage only: source graph begins with just the constructed root.
    struct SourceStandardPlacement {
        bool entered=false,shadowStored=false,nameStored=false,parentChecked=false,parentNotSelf=false;
        bool positionReturned=false,orientationEntered=false,orientationReturned=false;
        std::optional<std::int32_t> shadow148;
        std::uint8_t priorFlags1B3=0;
        std::array<char,128> copiedName{};std::size_t nameLength=0;
        std::array<std::uint32_t,4> position{},look{},up{};
        g::BodyMatrix48G oriented{};
        g::OriginalStandardPlacementGlobals50LA::History parentGuard,orientationGuard;
    };
    struct SourceModelFrame {
        std::array<g::SceneNode49P,a::maxModelNodes49O> nodes{};
        std::uint32_t graphCount=1;
        std::shared_ptr<a::OriginalSourcePsgFile50LA> sourceFile;
        bool sourceFileNativeClosed=false;
        SourceStandardPlacement placement;
        const void* sourceObserverOccurrence=nullptr;const void* sourceObserver19C=nullptr; //Exact source root-owned occurrence/publication borrow.
        bool parentAttachEntered=false,parentAttachReturned=false,parentAttached=false,parentDetachedNative=false;
        std::optional<std::uint32_t> parentAttachLock;
        std::uint32_t parentAttachZone=0;
        C* parentAttachBorrow=nullptr;
        g::OriginalRacerModelSceneOwner50LA scene;
        g::OriginalRacerModelPackets50LA sourcePackets; //Owner exists even when original1F0 remainsNULL.
        bool packetCallEntered=false,packetCallReturned=false;
        bool warmLookupAttempted=false,warmAcquireEntered=false;
        bool warmRowsEntered=false,warmRowsReturned=false,warmMaterialsEntered=false,warmMaterialsReturned=false;
        bool warmParametersEntered=false,warmParametersReturned=false,warmRowTableEntered=false,warmRowTableReturned=false;
        SourceModelFrame* warmCreator=nullptr; //Same actual earlier cold prototype owner, never native metadata.
        const a::OriginalStandardModelGeometry50LA* warmGeometryHistory=nullptr;
        g::OriginalSourceStandardSmoker50LA sourceSmoker;
        bool smokerCallEntered=false,smokerEnrolled=false,emitterEnrolled=false;
        bool smokerConfigurationReturned=false,smokerManagerBound=false;
        g::OriginalTrackParticleCold50LA* smokerManagerBorrow=nullptr;
        const void* smokerManagerHistory=nullptr;
        bool counted=false,enrolled=false,nativeRetired=false,nativeAliasWithdrawn=false,nameStored=false;
        bool lookupEntered=false,providerResolved=false,resolverResolved=false;
        bool rowsCallEntered=false,rowsCallReturned=false;
        bool materialCallEntered=false,materialCallReturned=false,quantizationCallEntered=false,quantizationCallReturned=false;
            bool geometryCallEntered=false,geometryLoadCallEntered=false;
        bool geometryPaletteCallEntered=false,geometryPaletteCallReturned=false;
        bool geometryPrototypeCallEntered=false,geometryPrototypeCallReturned=false;
        bool sourceCacheCallEntered=false,sourceCacheCallReturned=false;
        bool sourceCloseCallEntered=false,sourceCloseCallReturned=false;
        bool rowTableCallEntered=false,rowTableCallReturned=false;
        const C* renderableReturnedEntity=nullptr;
        std::shared_ptr<a::OriginalStandardMaterialTable50LA> materialTable;
        const std::string* materialPrefix=nullptr;
        std::array<C*,a::maxModelNodes49O> warmCloneEnrolled{}; //Stable physical identity; never enumeration ordinal.
        bool warmCloneEntered=false,warmCloneReturned=false,warmEnumerationEntered=false;
        std::array<bool,a::maxModelNodes49O> rowEnrolled{}; //Row0 uses counted/enrolled above.
        std::optional<std::uint32_t> rowsReturnV0;
        std::optional<bool> lookupReturn;
        bool absentLogAttempted=false,sourceReferenceDropped=false,sourceSceneReleased=false,sourceOuterDeleted=false;
        std::size_t absentLogWritten=0;
        std::optional<std::uint32_t> releaseLock423F58,releaseReturnV0,loaderReturnV0;
        const C* loaderReturnedEntity=nullptr; //Actual nonzero240740 V0 identity, not literal1 or guest address bits.
        std::uint32_t destructorStoreMask=0,destructorVtable1A8=0,destructorVtable10=0;
        std::uint8_t destructorNullSkips=0;
        std::optional<std::int32_t> dynamicArgument;
        std::string requestedPsg;
        std::shared_ptr<a::OriginalStandardArchiveProvider50LA> provider;
        std::shared_ptr<a::OriginalStandardTextureResolver50LA> resolver;
        a::OriginalStandardArchiveProvider50LA::MemberView member{};
        a::OriginalStandardModelFileView50LA file{};
        g::SceneGraph49P graph()noexcept{return {nodes.data(),graphCount};}
        g::SceneGraph49P graph()const noexcept{return {const_cast<g::SceneNode49P*>(nodes.data()),graphCount};}
    };
    //Native tagged shell preserves the existing actual parent34 identity.
    //The type2 guest allocation is16 bytes, not Standard's20 bytes. Descriptor
    //0C and body8 have separate typed lanes; never reinterpret smoker0C/model+8.
    struct SourcePhysicsDescriptor27A650 {
        SourceDescriptor27A100 common;
        std::array<std::uint32_t,11> scalar284;
        std::uint16_t scalarKnown=0;
        std::array<std::uint32_t,4> vector2B0;
        bool vectorKnown=false;
    };
    struct SourcePhysicsWrapper27BA3C {
        std::optional<C*> body8; //1659C0 Entity is at BASE, distinct from Model Entity+8.
        std::unique_ptr<g::OriginalSourcePhysicsObject50LA> bodyOwner;
        std::unique_ptr<g::OriginalSourcePhysicsModel50LA> nestedModel;
        std::unique_ptr<g::OriginalSourcePhysicsBody50LA> sourceBodyD0;
        std::unique_ptr<g::OriginalSourcePhysicsGravity50LA> sourceGravityD4;
        std::unique_ptr<g::OriginalSourcePhysicsCollision50LA> sourceCollisionE0;
        std::unique_ptr<g::OriginalSourcePhysicsCollision50LA> sourceCollisionE4;
        std::unique_ptr<g::OriginalSourcePhysicsCollision50LA> sourceCollisionE8;
        bool sourceProceduralCollisionCallEntered=false,sourceProceduralCollisionCallReturned=false,sourceProceduralCollisionEnrolled=false;
        bool sourceProceduralCollisionConfigEntered=false,sourceProceduralCollisionConfigReturned=false;
        bool sourceProceduralCollisionAttachEntered=false,sourceProceduralCollisionAttachReturned=false;
        bool sourceProceduralCollisionLateEntered=false,sourceProceduralCollisionLateReturned=false;
        bool sourceProceduralCollisionShapeEntered=false;
        bool sourceProceduralCollisionResultEntered=false,sourceProceduralCollisionResultReturned=false;
        std::uint16_t sourceLocalFilterEntered=0;
        std::array<std::optional<g::OriginalSourcePhysicsCollision50LA::Result>,9> sourceLocalFilterCalls;
        bool sourceSetup165E90Returned=false,sourceSetup165C30Returned=false,sourceSetupPositive27B470=false;
        C* sourceSetupReturnedBase=nullptr; //Typed original self return, not guest pointer bits or boolean1.
        struct SourceConfigure166400 {
            bool entered=false,argumentsPrepared=false,calleeEntered=false,scratchReturned=false,scratchCopied=false,boundsCallReached=false;
            bool boundsEntered=false,boundsReturned=false;
            bool dimensionsEntered=false,dimensionsReturned=false;
            bool inertiaEntered=false,inertiaReturned=false;
            bool contactEntered=false,contactReturned=false,frictionEntered=false,frictionReturned=false;
            bool tailEntered=false,returned74=false;
            bool updateEntered=false,descriptorQuadLoaded=false,bodyQuadEntered=false,bodyQuadReturned=false,updateCallReached=false;
            std::array<std::uint32_t,4> descriptorQuad{};
            C* quadBodyD0=nullptr;
            std::uint32_t updateDispatch08=0,updateTarget78=0,updatePC=0;
            std::uint8_t nullMembershipEntered=0,nullMembershipReturned=0;
            std::array<std::optional<g::OriginalSourcePhysicsCollision50LA::Result>,2> nullMembershipResults;
            bool nullObserverEntered=false,nullObserverReturned=false;
            bool positionEntered=false,positionReturned=false;
            std::optional<g::OriginalSourcePhysicsModel50LA::Result> positionResult;
            bool orientationEntered=false,orientationReturned=false;
            std::optional<g::OriginalSourcePhysicsModel50LA::Result> orientationResult;
            bool directUpdateEntered=false,directUpdateReturned=false;
            std::optional<g::OriginalSourcePhysicsCollision50LA::Result> directUpdateResult;
            bool resultUpdateEntered=false,resultUpdateReturned=false;
            std::optional<g::OriginalSourcePhysicsCollision50LA::Result> resultUpdateResult;
            bool directMembershipEntered=false,directMembershipReturned=false;
            std::optional<const void*> directMembershipZone; //Actual typed BaseB8 load; engaged NULL is known.
            std::uint32_t directMembershipMode=0;
            std::optional<g::OriginalSourcePhysicsCollision50LA::Result> directMembershipResult;
            bool resultMembershipEntered=false,resultMembershipReturned=false;
            std::optional<const void*> resultMembershipZone;
            std::uint32_t resultMembershipMode=0;
            std::optional<g::OriginalSourcePhysicsCollision50LA::Result> resultMembershipResult;
            std::optional<g::OriginalWorldPhysicsIndex50LA*> manager3D4;
            std::optional<std::int32_t> managerZone;
            C* managerBase=nullptr;
            bool managerCallEntered=false,managerReturned=false,managerNativeWithdrawn=false;
            std::optional<std::uint32_t> managerRegistrationIndex,managerReturnV0,wrapperReturnV0;
            std::uint32_t managerPC=0,wrapperReturnPC=0;
            bool parentAttachEntered=false,parentAttachReturned=false;
            std::optional<std::uint32_t> parentAttachZone;
            C* parentAttachIdentity=nullptr;
            std::optional<g::OriginalSourcePhysicsObject50LA::Result> parentAttachResult;
            bool modelObserverEntered=false,modelObserverReturned=false;
            bool bodyResetEntered=false,bodyResetReturned=false,updateReturned=false;
            C* resetBodyD0=nullptr;
            std::uint32_t resetDispatch08=0,resetTargetAC=0,updateReturnPC=0;
            std::optional<g::OriginalSourcePhysicsBody50LA::Result> bodyResetResult;
            std::optional<const void*> modelObserverZone; //Actual BaseB4, not collisionB8.
            std::uint32_t modelObserverMode=0;
            std::optional<g::OriginalSourcePhysicsModel50LA::Result> modelObserverResult;
            std::optional<g::OriginalSourcePhysicsModel50LA::Result> nullObserverResult;
            std::optional<g::OriginalSourcePhysicsBody50LA::Result> bodyQuadResult;
            std::array<std::uint32_t,8> contactArguments{};
            std::optional<g::OriginalSourcePhysicsContact50LA::Result> contactResult,frictionResult;
            std::optional<g::OriginalSourcePhysicsObject50LA::Result> tailResult;
            std::optional<g::OriginalSourcePhysicsBody50LA::Result> inertiaResult;
            std::optional<g::OriginalSourcePhysicsBody50LA::Result> dimensionsResult;
            std::optional<g::OriginalSourcePhysicsModel50LA::Result> boundsResult; //Native call result, distinct from source166400 return.
            const SourcePhysicsDescriptor27A650* descriptor=nullptr;
            C* receiver=nullptr;
            const g::OriginalSourcePhysicsModel50LA* nestedOwner=nullptr;
            const void* model14C=nullptr;
            std::uint32_t dispatch08=0,target74=0,pc=0;
            std::array<std::uint32_t,12> callerStack{}; //Only offsets0,8,10,18,20,28 low32 are written.
            std::uint16_t callerStackKnown=0;
            std::array<std::uint32_t,8> argumentFpu{},calleeFpu{};
            std::uint8_t argumentFpuKnown=0,calleeFpuKnown=0;
            std::uint8_t savedOldFpuStores=0,savedOldFpuKnown=0; //Reached saves, unavailable caller-old bits remain unknown.
            std::array<std::uint32_t,4> center40{},half50{};
            std::uint8_t centerKnown=0,halfKnown=0;
            std::array<std::uint32_t,16> scratchA0{},working60{};
            std::uint16_t scratchKnown=0,workingKnown=0;
        } configure166400;
        std::optional<g::OriginalSourcePhysicsCollision50LA::Result> sourceProceduralCollisionResultCall;
        std::optional<g::OriginalSourcePhysicsCollision50LA::Result> sourceProceduralCollisionShapeResult; //Native call result, never source11C710 return.
        std::array<char,256> directCollisionSetupName{};
        std::size_t directCollisionSetupNameLength=0;
        std::optional<std::size_t> directCollisionSetupDot;
        bool directCollisionNameEntered=false,directCollisionNameCopied=false,directCollisionDotSearched=false,directCollisionNameReturned=false;
        bool sourceDirectCollisionCallEntered=false,sourceDirectCollisionCallReturned=false,sourceDirectCollisionEnrolled=false;
        bool sourceDirectCollisionConfigEntered=false,sourceDirectCollisionConfigReturned=false;
        bool sourceDirectCollisionPathEntered=false,sourceDirectCollisionPathReturned=false;
        bool sourceDirectCollisionSearchEntered=false,sourceDirectCollisionSearchAccepted=false,sourceDirectCollisionMissingEntered=false;
        bool sourceDirectCollisionLoaderReturned=false;
        bool sourceDirectCollisionFoundEntered=false,sourceDirectCollisionHeaderReturned=false;
        bool sourceDirectCollisionType0Entered=false,sourceDirectCollisionArraysEntered=false;
        bool sourceDirectCollisionNodesEntered=false,sourceDirectCollisionImportReturned=false;
        bool sourceDirectCollisionResourceEntered=false,sourceDirectCollisionPayloadReturned=false;
        bool sourceDirectCollisionCloseEntered=false,sourceDirectCollisionCloseReturned=false;
        bool sourceDirectCollisionFoundTransformEntered=false,sourceDirectCollisionWorldCallReached=false;
        bool sourceDirectCollisionMatrixEntered=false,sourceDirectCollisionMatrixReturned=false;
        bool sourceDirectCollisionAttachEntered=false,sourceDirectCollisionAttachReturned=false;
        bool sourceDirectCollisionLateEntered=false,sourceDirectCollisionLateReturned=false;
        bool sourceDirectCollisionTransformEntered=false,sourceDirectCollisionNullModelBound=false;
        std::shared_ptr<a::OriginalSourcePsgMissingLookup50LA> directCollisionSearch;
        std::unique_ptr<g::OriginalSourcePhysicsContact50LA> sourceContactDC;
        std::unique_ptr<g::OriginalSourcePhysicsDetector50LA> sourceDetectorD8;
        bool terrainZero27B42C=false,terrainSaved165C50=false,terrainForwarded165D98=false,terrainSaved165EAC=false;
        bool sourceDetectorCallEntered=false,sourceDetectorCallReturned=false,sourceDetectorEnrolled=false;
        bool sourceDetectorConfigEntered=false,sourceDetectorConfigReturned=false;
        bool sourceDetectorEnableEntered=false,sourceDetectorEnableReturned=false;
        bool sourceDetectorBindingEntered=false,sourceDetectorBindingReturned=false;
        bool sourceDetectorAttachEntered=false,sourceDetectorAttachReturned=false;
        bool sourceContactCallEntered=false,sourceContactCallReturned=false,sourceContactEnrolled=false;
        bool sourceContactCapacityEntered=false,sourceContactCapacityReturned=false;
        bool sourceContactBodyEntered=false,sourceContactBodyReturned=false;
        bool sourceContactPointsEntered=false,sourceContactPointsReturned=false;
        bool sourceContactScalarsEntered=false,sourceContactScalarsReturned=false;
        bool sourceContactAttachEntered=false,sourceContactAttachReturned=false;
        std::shared_ptr<a::OriginalSourcePsgMissingLookup50LA> collisionSearch;
        bool sourceBodyAttachEntered=false,sourceBodyAttachReturned=false;
        bool sourceGravityCallEntered=false,sourceGravityCallReturned=false,sourceGravityEnrolled=false;
        bool sourceGravityTargetEntered=false,sourceGravityTargetReturned=false;
        bool sourceGravityAttachEntered=false,sourceGravityAttachReturned=false;
        std::array<char,256> collisionSetupName{}; //Native bounded scratch, not original buffer-size authority.
        std::size_t collisionSetupNameLength=0;
        std::optional<std::size_t> collisionSetupDot;
        bool collisionNameEntered=false,collisionNameCopied=false,collisionDotSearched=false,collisionNameReturned=false;
        bool sourceCollisionCallEntered=false,sourceCollisionCallReturned=false,sourceCollisionEnrolled=false;
        bool sourceCollisionConfigEntered=false,sourceCollisionConfigReturned=false;
        bool sourceCollisionNameEntered=false,sourceCollisionNameReturned=false;
        bool sourceCollisionSearchEntered=false,sourceCollisionSearchAccepted=false,sourceCollisionMissingEntered=false;
        bool sourceCollisionFoundEntered=false,sourceCollisionFoundHeaderReturned=false;
        bool sourceCollisionType0Entered=false;
        bool sourceCollisionArraysEntered=false;
        bool sourceCollisionNodesEntered=false,sourceCollisionImportReturned=false;
        bool sourceCollisionResourceEntered=false,sourceCollisionPayloadReturned=false;
        bool sourceCollisionCloseEntered=false,sourceCollisionLoaderReturned=false;
        bool sourceCollisionResultEntered=false,sourceCollisionResultReturned=false;
        bool sourceCollisionTransformEntered=false,sourceCollisionTransformReturned=false;
        bool sourceCollisionAttachEntered=false,sourceCollisionAttachReturned=false;
        bool nestedCallEntered=false,nestedCallReturned=false,nestedModelEnrolled=false;
        bool nestedNameEntered=false,nestedNameReturned=false;
        std::shared_ptr<a::OriginalSourcePsgMissingLookup50LA> missingLookup;
        std::shared_ptr<a::OriginalSourcePsgFile50LA> foundFile;
        std::shared_ptr<a::OriginalStandardArchiveProvider50LA> foundProvider;
        std::shared_ptr<a::OriginalStandardTextureResolver50LA> foundResolver;
        a::OriginalStandardArchiveProvider50LA::MemberView foundMember;
        a::OriginalStandardModelFileView50LA foundNativeFile;
        const a::OriginalStandardModelGeometry50LA* foundGeometryHistory=nullptr;
        bool foundEntered=false,foundProviderResolved=false,foundNativeResolved=false;
        bool foundCaptureEntered=false,foundCaptureReturned=false,foundNativeWithdrawn=false;
        std::optional<std::int64_t> foundHandle678;
        std::optional<const a::OriginalStandardModelGeometry50LA*> foundResource684;
        bool nestedLookupEntered=false,nestedLookupReturned=false;
        bool nestedReleaseEntered=false,nestedLoaderReturned=false;
        bool nestedRowsEntered=false,nestedRowsReturned=false;
        bool nestedMaterialEntered=false,nestedMaterialReturned=false;
        bool nestedQuantizationEntered=false,nestedQuantizationReturned=false;
        bool nestedGeometryEntered=false,nestedGeometryAllocated=false;
        bool nestedGeometryLoadEntered=false,nestedGeometryLodsReturned=false;
        bool nestedGeometryPaletteEntered=false,nestedGeometryReturned=false;
        bool nestedPrototypeEntered=false,nestedPrototypeReturned=false;
        bool nestedCacheEntered=false,nestedCacheReturned=false;
        bool nestedCloseEntered=false,nestedCloseReturned=false;
        bool nestedRowTableEntered=false,nestedRowTableReturned=false;
        const C* nestedRenderableEntity=nullptr; //Actual26E820 Entity+8, not final240740 return.
        bool nestedPacketsEntered=false,nestedPacketsReturned=false;
        bool nestedAttachEntered=false,nestedAttachReturned=false;
        bool nestedPlacementEntered=false,nestedPlacementReturned=false;
        bool setup165E90Entered=false,setupNoOpReturned=false;
        bool visibilityB4Stored=false,collisionB8Stored=false;
        bool sourceBodyCallEntered=false,sourceBodyCallReturned=false,sourceBodyEnrolled=false;
        bool sourceBodyConfigEntered=false,sourceBodyConfigReturned=false;
        const C* nestedLoaderEntity=nullptr; //Actual final240740 Entity+8, never a numeric pointer tag.
        std::optional<std::uint32_t> nestedRowsReturnV0;
        std::array<C*,a::maxModelNodes49O> nestedRowEnrolled{};
        std::optional<std::uint32_t> nestedLoaderReturnV0;
        std::optional<std::int32_t> sourceLookupReturnV0;
        bool callbackEntered=false,nameFormatted=false,bodyCallEntered=false,bodyCallReturned=false;
        bool secondCallback27B600Returned=false; //Actual JR/NOP; no produced V0 or descriptor disposal.
        bool bodyEnrolled=false,setupEntered=false;
        std::array<char,256> setupName{};std::size_t setupNameLength=0;
        std::optional<std::uint32_t> collisionZone,visibilityZone;
        const void* collisionNode=nullptr;const void* visibilityNode=nullptr;
        bool collisionRead=false,visibilityRead=false;
        std::unique_ptr<SourcePhysicsDescriptor27A650> descriptor0C;
        bool factoryReturned=false,descriptorLaneKnown=false;
        bool descriptorAttempted=false,descriptorAllocationReturned=false,descriptorPublished=false;
        bool descriptorReadEntered=false,descriptorReadReturned=false,countedInParent=false;
        std::optional<std::uint32_t> descriptorReturnV0;
    };
    struct StandardWrapper {
        enum class Dispatch { unbound,standard3D26F0,physics3D26C0 } dispatch=Dispatch::unbound;
        bool sourceMode=false,sourceTypeKnown=false,sourceDescriptorAllocationReturned=false,sourceDescriptorPublished=false;
        std::uint32_t sourceVtable04=0;
        std::uint32_t type00=0;
        g::OriginalRacerModelSceneOwner50LA* model8=nullptr; // Alias, not a second owner.
        std::unique_ptr<Descriptor> descriptor10;
        std::unique_ptr<SourceModelFrame> sourceModel;
        bool sourceModelAllocationAttempted=false,sourceModelAllocationReturned=false,sourceModelPublished=false;
        bool sourceModelNameFormatted=false,sourceModelCallbackEntered=false;
        bool sourceNoneLogAttempted=false,sourceModelCleared=false,sourceDescriptorDisposed=false,sourceDescriptorNullStored=false;
        std::size_t sourceNoneLogWritten=0;
        std::optional<std::int32_t> sourceSmokerRead;
        bool sourceSmokerManagerRead=false;
        const void* sourceSmokerManager4A0=nullptr; //Raw source branch input; no manager retain.
        std::optional<std::uint32_t> sourceCallbackReturnV0;
        SourcePhysicsWrapper27BA3C sourcePhysics;
        std::optional<C*> smoker0C; // source factory does NOT initialize this lane
    };
    struct Observer {
        g::CollectionRef50BX object=0;
        std::vector<g::CollectionZoneLink50BX> links;
        std::vector<g::LiveZoneEntry50EN> entries;
        g::LiveZoneMembership50EN membership;
    };
    struct Model {
        Input source;
        // One canonical graph per independent source model, never a copied
        // raster matrix or an alias of another placement sharing this PSG.
        g::SceneNode49P node{};
        g::OriginalRacerModelSceneOwner50LA scene;
        StandardWrapper wrapper;
        std::shared_ptr<a::OriginalStandardModelGeometry50LA> geometry;
        g::OriginalRacerModelPackets50LA packets;
        bool resourceRetained=false,referenceReleased=false,rowTableFreed=false,placed=false;
        std::size_t parent=0,slot=0;
        bool rows=false;
        std::unique_ptr<Observer> observer;
        bool parentAttached=false;
        g::SceneGraph49P graph()noexcept{return {&node,1};}
    };
    struct FailedModel {
        g::SceneNode49P node{};g::OriginalRacerModelSceneOwner50LA scene;
        bool referenceReleased=false,rowTableFreed=false;
        g::SceneGraph49P graph()noexcept{return {&node,1};}
    };
    struct Slot {
        std::size_t worldInstance=0;
        Model* model=nullptr;
        std::unique_ptr<StandardWrapper> unresolvedWrapper;
        std::unique_ptr<FailedModel> failedModel;
        bool sourceDataAttempted=false,sourceDataComplete=false;
        bool sourceInitStarted=false,sourceInitComplete=false;
        // Null represents an explicitly unconstructed geometry-less prefix,
        // not a successful source loader or a skipped visibility registration.
    };
    struct Parent {
        C entity;
        std::uint32_t zone=0,count30=0;
        std::vector<Slot> entries34;
        bool constructed=false,released=false;
        //Source34 is RAW uninitialized pointers, not value-initialized slots.
        bool sourceMode=false,sourceCountKnown=false,sourceArrayReturned=false,sourceAuthorityEnrolled=false;
        std::uint32_t sourceVtable08=0;
        std::unique_ptr<StandardWrapper*[]> sourceEntries34;
        std::size_t sourceEntriesKnown=0;
        std::unique_ptr<StandardWrapper> sourceFirstWrapper;
        std::unique_ptr<Descriptor> sourcePendingDescriptor; //Before actual27AB90 publication.
    };
    bool sourceMode_=false,sourceOuterReturned_=false,sourceFirstDescriptorReturned_=false;
    bool sourceLedgerAttempted_=false,sourceLedgerPrepared_=false,sourceFirstTransferred_=false,sourceAllDescriptorsReturned_=false;
    std::array<std::vector<std::unique_ptr<StandardWrapper>>,10> sourceWrapperLedger_;
    //Nonowning exact source1B577C suffix custody.
    //Every collision child must leave before Parent storage retires.
    //Ordered non-owning source collision children, appended after Standard children.
    //Container addresses stay stable; reverse native retirement pops only the exact tail.
    std::array<std::vector<C*>,10> sourceCollisionTailNative_{};
    std::array<std::uint32_t,10> sourcePhysicsCounts_{}; //Actual27B8AC increments, not Entity counts.
    std::array<std::uint32_t,10> sourceZoneCounts_{}; //Native allocation admission, not guestcount30 stores.
    std::array<bool,10> sourceGroupOuterReturned_{};
    std::array<bool,10> sourceLogAttempted_{};
    std::array<std::size_t,10> sourceLogWritten_{};
    std::thread::id sourceThread_;
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::shared_ptr<g::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<a::OriginalStandardMaterialTable50LA> materials_;
    std::vector<std::unique_ptr<Parent>> parents_;
    const ra_pod_scene50LA::Owner* sourceWorld3C4Borrower_=nullptr;
    std::vector<std::unique_ptr<Model>> models_;
    std::uint64_t generation_=0;
    std::size_t unresolved_=0;
    bool attempted_=false,constructed_=false,closed_=false;
    bool resourcesAttempted_=false,resourcesComplete_=false;
    std::shared_ptr<a::OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> visibility_;
    bool observersAttempted_=false,observersComplete_=false;
    //Synchronous private-domain borrow; World withdraws it before either owner dies.
    g::OriginalWorldCollisionObjects50LA* nativeCollisionForest_=nullptr;
    bool nativeCollisionForestRetiring_=false; //Actual detach transaction; never reappend.

    Owner()=default;
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    static bool reject(Diagnostic& d,const char* why)noexcept{d.failure=why;return false;}
#include "ra_static_model_source_lifetime50la.inl"
    bool prepare(std::shared_ptr<const ra_cold_sources50LA::Owner> source,
        std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::shared_ptr<a::OriginalStandardMaterialTable50LA> materials,
        std::uint64_t generation,const std::vector<Input>& inputs,Diagnostic& d)noexcept {
        d={};if(sourceMode_||attempted_||closed_)return reject(d,"static model prefix already attempted");
        attempted_=true;
        if(!source||!source->sourcesDecoded()||!application||!application->application10()||
           !materials||!generation||application->attachmentLock423F58())
            return reject(d,"same live source/Application/material owners required");
        source_=std::move(source);application_=std::move(application);materials_=std::move(materials);
        generation_=generation;
        try {
            const auto& world=source_->sourcesDecoded()->world;
            // Native backing admission/allocation precedes counted constructors.
            parents_.reserve(world.zones.size());models_.reserve(inputs.size());
            std::size_t wi=0;
            for(std::size_t z=0;z<world.zones.size();++z){const auto& authored=world.zones[z];
                if(authored.zone!=z+1)return reject(d,"source zone order differs");
                auto p=std::make_unique<Parent>();p->zone=authored.zone;
                p->entries34.resize(authored.models);
                for(std::size_t j=0;j<p->entries34.size();++j,++wi){
                    if(wi>=world.instances.size()||world.instances[wi].zone!=authored.zone||
                       world.instances[wi].ordinal!=j+1)return reject(d,"source StaticModels entry order differs");
                    p->entries34[j].worldInstance=wi;
                }
                parents_.push_back(std::move(p));
            }
            if(wi!=world.instances.size()||inputs.size()>wi)return reject(d,"source StaticModels count differs");
            for(std::size_t i=0;i<inputs.size();++i){const auto& input=inputs[i];d.placement=i;
                if(input.worldInstance>=world.instances.size()||(i&&input.worldInstance<=inputs[i-1].worldInstance)||
                   !input.payload||!input.payload->prepared()||input.payload->nodeCount()!=1||
                   !input.payload->originalBytes()||!input.payload->member()||!input.payload->scene()||
                   !input.materials||!input.materials->complete()||
                   input.materials->table382B38()!=materials_.get())return reject(d,"actual placement resource/material prefix missing");
                const auto& instance=world.instances[input.worldInstance];
                if(instance.parameter_block>=world.parameters.blocks.size())return reject(d,"source model descriptor absent");
                bool typeSeen=false;
                for(const auto& f:world.parameters.blocks[instance.parameter_block].fields)if(f.name=="statictype"){
                    //27B834 StaticType, default Standard. Never reinterpret
                    // PhysicsObject or the source's unknown-type fallback.
                    if(typeSeen||f.value!="Standard")return reject(d,"unadmitted actual static wrapper type");
                    typeSeen=true;
                }
                if(!instance.zone||instance.zone>parents_.size()||!instance.ordinal||
                   instance.ordinal>parents_[instance.zone-1]->entries34.size())return reject(d,"source slot outside parent");
                auto m=std::make_unique<Model>();m->source=input;m->parent=instance.zone-1;m->slot=instance.ordinal-1;
                //27BA04..27BAC4 Standard wrapper construction, followed by
                // retained native descriptor authoring (27AB90). No model alias
                // is published until its genuine2403A0 constructor succeeds.
                m->wrapper.dispatch=StandardWrapper::Dispatch::standard3D26F0;m->wrapper.type00=1;
                m->wrapper.descriptor10=std::make_unique<Descriptor>();
                auto& descriptor=*m->wrapper.descriptor10;descriptor.source=&instance;
                for(unsigned axis=0;axis<3;++axis){
                    const double values[]={instance.position[axis],instance.look[axis],instance.up[axis]};
                    float* destinations[]={&descriptor.position[axis],&descriptor.look[axis],&descriptor.up[axis]};
                    for(unsigned v=0;v<3;++v){const float f=static_cast<float>(values[v]);
                        if(!std::isfinite(f)||double(f)!=values[v])return reject(d,"original descriptor scalar is not binary32");
                        *destinations[v]=f;
                    }
                }
                bool dynamicSeen=false;
                for(const auto& f:world.parameters.blocks[instance.parameter_block].fields)if(f.name=="usedynamiclighting"){
                    if(dynamicSeen||(f.value!="T"&&f.value!="F"))return reject(d,"unsupported original UseDynamicLighting");
                    descriptor.useDynamicLighting144=f.value=="T";dynamicSeen=true;
                }
                bool sltSeen=false;
                for(const auto& f:world.parameters.blocks[instance.parameter_block].fields)if(f.name=="slt"){
                    if(sltSeen)return reject(d,"duplicate source SLT descriptor");sltSeen=true;
                    const auto dot=f.value.find('.');
                    //27A524 strcspn("."), cap50; this admitted corpus must not
                    // silently resolve a different file through truncation.
                    if(dot==std::string::npos||!dot||dot>50)return reject(d,"unsupported original SLT stem");
                    descriptor.modelName180=f.value.substr(0,dot)+".slt"; //27AC14 sprintf.
                }
                if(!sltSeen)return reject(d,"actual SLT descriptor missing");
                auto& slot=parents_[m->parent]->entries34[m->slot];
                if(slot.model||slot.worldInstance!=input.worldInstance)return reject(d,"duplicate actual parent slot");
                slot.model=m.get();models_.push_back(std::move(m));
            }
            unresolved_=world.instances.size()-models_.size();
            const auto additional=parents_.size()+models_.size();
            if(additional>std::numeric_limits<std::uint32_t>::max()-application_->objectCount423F60())
                return reject(d,"Application object count cannot admit this cold prefix");
            //271150 passes a1=1 to27B610. It constructs a genuine Entity,
            // installs StaticModels dispatch, and clears count30/entries34.
            // The ordered native array above is backing, not source publication.
            for(auto& p:parents_){
                const auto r=g::initializeOriginalWreckComponent50LA(p->entity,K::staticModels27B610,
                    generation_,key(&p->entity),true,application_->objectCount423F60());
                if(r.status!=g::OriginalWreckComponentStatus50LA::complete)return reject(d,"StaticModels Entity construction failed");
                p->constructed=true;++d.parents;
                //27B780 supplies this authored count. Native slots preserve
                //27B7A4's ordering; they do not impersonate constructed
                //StandardStaticModel/PhysicsObjectStaticModel wrapper tables.
                p->count30=static_cast<std::uint32_t>(p->entries34.size());
            }
            for(std::size_t i=0;i<models_.size();++i){auto& m=*models_[i];d.placement=i;
                const I root=static_cast<I>(0);
                g::OriginalRacerModelSceneBinding50LA binding;
                binding.sessionGeneration=generation_;
                binding.actualObjectCount423F60=&application_->objectCount423F60();
                binding.actualAttachmentLock423F58=&application_->attachmentLock423F58();
                binding.actualTypes383408=&application_->types383408();
                binding.modelIndex=root;binding.sourceRowIndices=&root;binding.sourceRowCount=1;
                //27AC30 a1=1;27AC44 loader a2=0: the model IS PSG row0.
                d.scene=m.scene.constructDirectRoot2403A0(binding,m.graph(),true);
                if(!d.scene.okay())return reject(d,"actual direct-root2403A0 prefix failed");
                m.wrapper.model8=&m.scene; //27AC38, not ownership transfer.
                d.scene=m.scene.prepareDirectRoot26E934(m.graph(),*m.source.payload->member());
                if(!d.scene.okay())return reject(d,"actual model name prefix failed");
                const auto& bytes=*m.source.payload->originalBytes();a::PsgHierarchyView49O hierarchy{};
                if(!a::readPsgHierarchy49O(bytes.data(),bytes.size(),hierarchy,d.hierarchy)||hierarchy.nodeCount!=1)
                    return reject(d,"retained original PSG hierarchy rejected");
                // Cache lookup in the resource continuation chooses fresh28CAC0
                // or the actual warm childless26EAC4 branch. Never pre-load
                // fresh rows before that choice (nodeCount1A0 differs).
                ++d.models;
                // Resource/placement continuation is a separate explicit stage.
            }
            d.unresolvedSlots=unresolved_;constructed_=true;return true;
        }catch(...){return reject(d,"cold static allocation failed; counted prefixes retained for close");}
    }
#include "ra_static_model_resources50la.inl"
#include "ra_static_model_observers50la.inl"
    StandardWrapper* nativeStandardWrapper(Slot& slot)noexcept {
        return slot.model?&slot.model->wrapper:slot.unresolvedWrapper.get();
    }
    const StandardWrapper* nativeStandardWrapper(const Slot& slot)const noexcept {
        return slot.model?&slot.model->wrapper:slot.unresolvedWrapper.get();
    }
    bool failedModelCold(const FailedModel& f)const noexcept {
        if(!f.scene.model()||f.scene.released())return true;
        const auto& v=f.scene.fields();const auto* m=f.scene.model();
        return m&&f.scene.binding_.sessionGeneration==generation_&&
            f.scene.binding_.actualObjectCount423F60==&application_->objectCount423F60()&&
            f.scene.binding_.actualAttachmentLock423F58==&application_->attachmentLock423F58()&&
            f.scene.binding_.actualTypes383408==&application_->types383408()&&!m->observer19C&&!m->configuration1A4&&!v.geometry1B4&&!v.sourceRows1B8&&
            !v.materialCount1C0&&!v.materials1C4&&!v.field1C8&&!v.packets1F0&&
            f.scene.preflightScene2896D0({const_cast<g::SceneNode49P*>(&f.node),1}).okay();
    }
    bool releaseFailedModel26E9D4(FailedModel& f,Diagnostic& d)noexcept {
        if(f.scene.released())return true;
        if(!failedModelCold(f))return reject(d,"failed-load model acquired foreign resources");
        const g::OriginalWreckComponentDomain50LA domain{generation_,nullptr,0};
        if(!f.referenceReleased){
            d.scene=f.scene.releaseModelReference142590(domain);
            if(!d.scene.okay())return reject(d,"failed-load actual142590 release rejected");
            f.referenceReleased=true;
        }
        //241920->240450->26E460 see only genuine NULL packets/material/
        //geometry/config/observer. No resources are fabricated or skipped.
        if(!f.rowTableFreed){
            d.scene=f.scene.freeSourceRowTable26E5CC();
            if(!d.scene.okay())return reject(d,"failed-load null row-table retirement rejected");
            f.rowTableFreed=true;
        }
        d.scene=f.scene.releaseSceneBase2896D0(f.graph());
        return d.scene.okay()||reject(d,"failed-load actual base Entity retirement rejected");
    }
    bool prepareNativeStandardSlot27AB60(std::size_t zone,std::size_t ordinal,Diagnostic& d)noexcept {
        d={};if(!constructed_||closed_||!resourcesComplete_||!observersComplete_||
            zone>=parents_.size()||ordinal>=parents_[zone]->entries34.size()||!canClose())
            return reject(d,"current existing static owner required");
        auto& slot=parents_[zone]->entries34[ordinal];
        if(slot.sourceInitStarted||slot.sourceInitComplete||slot.sourceDataAttempted)
            return reject(d,"source wrapper data already attempted");
        const auto& world=source_->sourcesDecoded()->world;
        if(slot.worldInstance>=world.instances.size())return reject(d,"actual authored slot missing");
        const auto& instance=world.instances[slot.worldInstance];
        if(instance.parameter_block>=world.parameters.blocks.size()||instance.zone!=zone+1||instance.ordinal!=ordinal+1)
            return reject(d,"actual authored slot identity differs");
        const auto& authored=world.parameters.blocks[instance.parameter_block];
        // Authenticated RA corpus has neither key. Do not broaden other data.
        for(const auto& field:authored.fields)
            if(field.name=="statictype"||field.name=="issmoker")
                return reject(d,"unadmitted explicit type/smoker descriptor");
        slot.sourceDataAttempted=true;
        try {
            if(!slot.model){
                slot.unresolvedWrapper=std::make_unique<StandardWrapper>();
                auto& wrapper=*slot.unresolvedWrapper;
                wrapper.dispatch=StandardWrapper::Dispatch::standard3D26F0;wrapper.type00=1;
                wrapper.descriptor10=std::make_unique<Descriptor>();
                auto& descriptor=*wrapper.descriptor10;descriptor.source=&instance;
                for(unsigned axis=0;axis<3;++axis){
                    const double values[]={instance.position[axis],instance.look[axis],instance.up[axis]};
                    float* destinations[]={&descriptor.position[axis],&descriptor.look[axis],&descriptor.up[axis]};
                    for(unsigned v=0;v<3;++v){const float scalar=static_cast<float>(values[v]);
                        if(!std::isfinite(scalar)||double(scalar)!=values[v])return reject(d,"descriptor scalar not binary32");
                        *destinations[v]=scalar;
                    }
                }
                bool slt=false,dynamic=false;
                for(const auto& field:authored.fields){
                    if(field.name=="slt"){
                        if(slt)return reject(d,"duplicate actual SLT");slt=true;
                        const auto dot=field.value.find('.');
                        if(dot==std::string::npos||!dot||dot>50)return reject(d,"unsupported actual SLT stem");
                        descriptor.modelName180=field.value.substr(0,dot)+".slt";
                    }else if(field.name=="usedynamiclighting"){
                        if(dynamic||(field.value!="T"&&field.value!="F"))return reject(d,"unsupported actual dynamic lighting");
                        dynamic=true;descriptor.useDynamicLighting144=field.value=="T";
                    }
                }
                if(!slt)return reject(d,"missing actual SLT");
            }
            auto* wrapper=nativeStandardWrapper(slot);
            if(!wrapper||wrapper->type00!=1||wrapper->dispatch!=StandardWrapper::Dispatch::standard3D26F0||
               !wrapper->descriptor10||wrapper->descriptor10->source!=&instance)
                return reject(d,"existing Standard wrapper provenance differs");
            auto& descriptor=*wrapper->descriptor10;
            // Newly reached constructor-adoption evidence27A234; preserve ALL
            // earlier real TOB stores. This does not rewrite earlier seals.
            if(!descriptor.allowShadowReceiving148)descriptor.allowShadowReceiving148=1;
            descriptor.isSmoker14C=false; // actual absent-key default27A5F4
            slot.sourceDataComplete=true;return true;
        }catch(...){return reject(d,"wrapper/descriptor allocation failed; custody retained");}
    }
    // Caller is private actual World owner and has just checked SAME mounted
    // provider lookup132820 absent AND actual resolver.lookupModel132820
    // unresolved for the exact supplied geometry member. Original provider+170
    // fallback/history is unknown: this is a bounded native negative-load path.
    // Existing visual branch instead requires empty supplied absence name.
    bool completeNativeStandardSlot27ABC0(std::size_t zone,std::size_t ordinal,
        const std::string& authenticatedAbsentPsg,Diagnostic& d)noexcept {
        d={};if(closed_||zone>=parents_.size()||ordinal>=parents_[zone]->entries34.size()||!canClose())
            return reject(d,"current idle static owner required");
        auto& parent=*parents_[zone];auto& slot=parent.entries34[ordinal];
        auto* wrapper=nativeStandardWrapper(slot);
        if(!slot.sourceDataComplete||slot.sourceInitStarted||!wrapper||!wrapper->descriptor10||
           wrapper->type00!=1||wrapper->dispatch!=StandardWrapper::Dispatch::standard3D26F0)
            return reject(d,"actual Standard data continuation required");
        auto& descriptor=*wrapper->descriptor10;
        if(!descriptor.source||!descriptor.isSmoker14C||*descriptor.isSmoker14C||
           !descriptor.allowShadowReceiving148)
            return reject(d,"source descriptor inputs unknown");
        if(slot.model){
            auto& m=*slot.model;
            if(!authenticatedAbsentPsg.empty()||wrapper->model8!=&m.scene||!m.placed||!m.parentAttached||
               m.referenceReleased||!m.resourceRetained||!m.geometry||!m.rows||!m.observer||
               m.scene.model_.observer19C!=m.observer.get()||!m.observer->membership.attached||
               m.observer->membership.object!=m.observer->object||
               m.observer->membership.method!=g::ZoneMethod50CL::First||
               m.scene.fields().geometry1B4!=m.geometry.get()||
               m.scene.fields().packets1F0!=&m.packets||!parentTopology(parent))
                return reject(d,"actual model/geometry/observer/parent currentness differs");
        }else if(authenticatedAbsentPsg.empty()||authenticatedAbsentPsg!=descriptor.source->geometry_member)
            return reject(d,"same-provider exact missing geometry name required");
        slot.sourceInitStarted=true;
        try {
            if(slot.model){auto& m=*slot.model;
                // Copy first: allocation failure must not claim descriptor
                // retirement. Successful prefix uses actual existing model.
                const auto name=descriptor.modelName180.substr(0,127);
                auto& fields=m.scene.fields();
                fields.flags1B3=static_cast<std::uint8_t>((fields.flags1B3&~0x10u)|
                    ((static_cast<std::uint32_t>(*descriptor.allowShadowReceiving148)&1u)<<4));
                m.scene.model_.name38=name; //27AD20..44, source .slt spelling
                // Placement/observer/attachment were genuinely produced early
                // in this native owner; adopt their identities, NEVER replay.
            }else {
                if(application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
                    return reject(d,"actual Application count cannot admit temporary model");
                slot.failedModel=std::make_unique<FailedModel>();auto& failed=*slot.failedModel;
                const I root=static_cast<I>(0);g::OriginalRacerModelSceneBinding50LA binding;
                binding.sessionGeneration=generation_;binding.actualObjectCount423F60=&application_->objectCount423F60();
                binding.actualAttachmentLock423F58=&application_->attachmentLock423F58();
                binding.actualTypes383408=&application_->types383408();binding.modelIndex=root;
                binding.sourceRowIndices=&root;binding.sourceRowCount=1;
                // One native graph CAPACITY. No PSG row/table/load is claimed.
                d.scene=failed.scene.constructDirectRoot2403A0(binding,failed.graph(),true);
                if(!d.scene.okay())return reject(d,"actual failed-load model constructor rejected");
                wrapper->model8=&failed.scene; //27AC38 owns no extra reference
                d.scene=failed.scene.prepareDirectRoot26E934(failed.graph(),authenticatedAbsentPsg);
                if(!d.scene.okay())return reject(d,"actual missing-model resolved name rejected");
                if(!releaseFailedModel26E9D4(failed,d))return false;
                wrapper->model8=nullptr; //27AE48 in admitted native negative-load branch;
                // no claim unknown original provider+170 fallback returned null.
                slot.failedModel.reset();
            }
            // IsSmoker14C was authenticated false; no0C write/Emitter created.
            wrapper->descriptor10.reset(); //27AF6C/78 real owned metadata free
            slot.sourceInitComplete=true;return true;
        }catch(...){return reject(d,"partial Standard initialization retained for close");}
    }
    bool nativeStandardSlotCurrent27ABC0(std::size_t zone,std::size_t ordinal)const noexcept {
        if(closed_||zone>=parents_.size()||ordinal>=parents_[zone]->entries34.size())return false;
        const auto& slot=parents_[zone]->entries34[ordinal];const auto* wrapper=nativeStandardWrapper(slot);
        return slot.sourceInitComplete&&wrapper&&wrapper->type00==1&&
            wrapper->dispatch==StandardWrapper::Dispatch::standard3D26F0&&!wrapper->descriptor10&&
            !slot.failedModel&&(!slot.model?!wrapper->model8:wrapper->model8==&slot.model->scene);
    }
    bool canClose()const noexcept {
        if(sourceMode_)return canCloseSourceNative();
        if(closed_)return true;
        if(!application_)return parents_.empty()&&models_.empty();
        if(application_->attachmentLock423F58())return false;
        if(visibility_&&!visibility_->raStaticOwnerIdle(this))return false;
        std::size_t counted=0;
        for(const auto& p:parents_)if(p&&p->constructed&&!p->released){const auto& e=p->entity;
            if(e.reference00!=1||e.kind!=K::staticModels27B610||e.sessionGeneration!=generation_||
               e.parent1C||e.previous10||e.next14||e.mask20||e.descendants24||!parentTopology(*p))return false;
            ++counted;
        }
        for(const auto& m:models_)if(m&&m->scene.model()&&!m->scene.released()){
            const auto* object=m->scene.model();const auto& e=object->entity();
            const auto& fields=m->scene.fields();
            if((!m->parentAttached&&(e.parent1C||e.previous10||e.next14))||e.firstChild18||
               e.reference00!=(m->referenceReleased?0u:1u)||
               fields.field1C8||
               object->observer19C!=m->observer.get()||object->configuration1A4||
               !m->scene.preflightRetainedScene2896D0({const_cast<g::SceneNode49P*>(&m->node),1},m->observer.get()).okay())return false;
            if(m->observer){const auto& o=*m->observer;
                if(!visibility_||!o.object||o.membership.storage.links!=o.links.data()||
                   o.membership.storage.entries!=o.entries.data()||o.membership.storage.capacity!=o.links.size()||
                   o.links.size()!=o.entries.size()||
                   (o.membership.attached&&(o.membership.object!=o.object||
                    o.membership.method!=g::ZoneMethod50CL::First)))return false;
            }
            ++counted;
        }
        for(const auto& p:parents_)if(p)for(const auto& slot:p->entries34)if(slot.failedModel){
            const auto& f=*slot.failedModel;
            if(!failedModelCold(f))return false;
            if(f.scene.model()&&!f.scene.released())++counted;
        }
        if(!resourcesCanClose())return false;
        return counted<=application_->objectCount423F60();
    }
    bool close()noexcept {
        if(sourceMode_)return closeSourceNative();
        if(closed_)return true;if(sourceWorld3C4Borrower_||!canClose()||nativeCollisionForest_||nativeCollisionSuffixPresent27B170())return false;
        if(visibility_&&!visibility_->beginRaStaticAccess(this,true))return false;
        const auto failClose=[&](){if(visibility_)visibility_->endRaStaticAccess(this);return false;};
        // Retire any real temporary missing-PSG model before parent resources.
        // This is partial-load cleanup, not a successful27ABC0 receipt.
        for(auto& p:parents_)if(p)for(auto& slot:p->entries34)if(slot.failedModel){
            Diagnostic failure;auto& f=*slot.failedModel;
            if(f.scene.constructed2403A0()){
                if(!releaseFailedModel26E9D4(f,failure))return failClose();
            }else if(f.scene.model()&&!f.scene.closeFreshUnlinked(f.graph()).okay())return failClose();
            if(slot.unresolvedWrapper)slot.unresolvedWrapper->model8=nullptr;
            slot.failedModel.reset();
        }
        // Private incomplete-prefix recovery, not the attached27B650 path.
        // Constructed packets do execute their actual240450 retirement leaf.
        //27AAD0 destroys wrapper metadata, not the model8 alias. This owner
        // retires real linked/unlinked model prefixes before releasing backing.
        for(auto it=models_.rbegin();it!=models_.rend();++it)if(*it){auto& m=**it;
            if(resourcesAttempted_){if(!closeResources(m))return failClose();}
            else if(!m.scene.closeFreshUnlinked(m.graph()).okay())return failClose();
        }
        if(visibility_&&!visibility_->releaseRaStaticOwner(this))return failClose();
        visibility_.reset();
        models_.clear();
        for(auto& p:parents_)if(p&&p->constructed&&!p->released){
            for(auto& slot:p->entries34)slot.model=nullptr;
            p->entries34.clear();
            --p->entity.reference00; // Private root's actual104970 reference.
            --application_->objectCount423F60(); // Empty base1413E0.
            p->released=true;
        }
        parents_.clear();resolver_.reset();materials_.reset();source_.reset();application_.reset();closed_=true;return true;
    }
public:
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    Counts counts()const noexcept {
        return constructed_&&!closed_?Counts{parents_.size(),models_.size(),unresolved_}:Counts{};
    }
    const g::OriginalRacerModelSceneOwner50LA* model(std::size_t placement)const noexcept {
        return constructed_&&!closed_&&placement<models_.size()?&models_[placement]->scene:nullptr;
    }
    const g::SceneNode49P* sourceRoot(std::size_t placement)const noexcept {
        return model(placement)?&models_[placement]->node:nullptr;
    }
    bool resourcesAndPlacementConstructed()const noexcept{return resourcesComplete_&&!closed_;}
    const g::OriginalRacerModelPackets50LA* coldPackets(std::size_t placement)const noexcept {
        return resourcesAndPlacementConstructed()&&placement<models_.size()?&models_[placement]->packets:nullptr;
    }
    const C* parent(std::size_t zoneIndex)const noexcept {
        return constructed_&&!closed_&&zoneIndex<parents_.size()?&parents_[zoneIndex]->entity:nullptr;
    }
    // Exact authored slot order, including unresolved slots; no selected list.
    bool parentSlot(std::size_t zoneIndex,std::size_t ordinalIndex,std::size_t& worldInstance,
        const g::OriginalRacerModelSceneOwner50LA*& modelOut)const noexcept {
        if(!constructed_||closed_||zoneIndex>=parents_.size()||ordinalIndex>=parents_[zoneIndex]->entries34.size())return false;
        const auto& slot=parents_[zoneIndex]->entries34[ordinalIndex];worldInstance=slot.worldInstance;
        modelOut=slot.model?&slot.model->scene:nullptr;return true;
    }
};
}
