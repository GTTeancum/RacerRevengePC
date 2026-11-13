#pragma once
namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_cold_roster50LA { class WreckBacking; class NativeWreckEffectChildren; }
#include "original_wreck_asset_inventory50la.h"
#include "original_wreck_forces50la.h"
#include "original_wreck_collection_tuning50la.h"
#include "original_wreck_shared_response50la.h"
#include "original_racer_model_packets50la.h"
#include "original_wreck_effect_arrays50la.h"
#include "original_wreck_smoke_owner50la.h"
#include "original_wreck_fire_owner50la.h"
#include "original_wreck_dust_owner50la.h"
#include "original_wreck_splash_owner50la.h"
#include "original_wreck_group_effect_owners50la.h"
#include "reset_visibility50ey.h"
#include <limits>
#include <optional>
#include <exception>
#include <vector>
#include "../assets/original_phy_cursor50la.h"
#include <charconv>

namespace rr::game {
class OriginalWreckRacerStorage50LA;
// One borrowed warm-model creator candidate (26E9F0..26EA10 prototype1C).
// Supplied by the caller only from models THIS racer actually holds (its own
// chariot/engine scenes); never retained past the call. Earlier wreck pieces of
// the same racer are found by the storage itself.
struct OriginalWreckWarmCreator50LA {
    const OriginalRacerModelSceneOwner50LA* scene=nullptr;
    const rr::assets::OriginalStandardMaterialReferences50LA* materials=nullptr;
    SceneGraph49P graph{};
};
// Addresses of the actual caller fields, not optional copied context values.
//22BFD0 forwards its resource argument and particle-parent argument to2CB5A0.
struct OriginalWreckPieceEffectInputs50LA {
    const OriginalWreckEffectResourceContext50LA* const* resourceContextField=nullptr;
    OriginalWreckComponent50LA* const* particleParentField=nullptr;
};
enum class OriginalWreckConstructorPrefixStatus50LA : std::uint8_t {
    binding,capacity,allocation,components,physical,publishedLifetimeRequired,stopped,modelLoadRequired,warmModelRequired,modelAppendRequired,geometryAdmission,pieceSetupRequired,queryPublicationRequired,queryConstructionFailed,contactConstructorRequired,contactConstructionFailed,e4ConstructorRequired,e4ConstructionFailed,e4PreparationRequired,e4PreparationFailed,e8ConstructorRequired,e8ConstructionFailed,mutualFiltersRequired,mutualFiltersFailed,effectInputsRequired,effectConstructorRequired,effectReturnRequired,effectArraysFailed,effectChildrenRequired,effectSmokeFailed,effectFireRequired,effectFireFailed,effectNextPairRequired,effectDustRequired,effectDustFailed,effectSplashRequired,effectSplashFailed,collectionFiltersRequired,collectionFiltersFailed,nextPieceRequired,groupConstructorRequired,e0PreparationRequired,detectorConstructorRequired,e4NativeLoadComplete
};
struct OriginalWreckConstructorPrefixResult50LA {
    OriginalWreckConstructorPrefixStatus50LA status=OriginalWreckConstructorPrefixStatus50LA::binding;
    std::uint32_t sourcePC=0x22bfd0u;
    std::size_t initializedComponents=0;
    bool sourcePrefixCommitted=false;
    // Existing typed geometry loader is transactional. This diagnostic is
    // not a claim that its internal source phase was committed.
    std::uint32_t nativeLoaderDiagnosticPC=0;
};
enum class OriginalWreckPrefixRetirementStatus50LA : std::uint8_t {
    binding,attachmentLocked,topology,modelRetirementRequired,pieceRetirementRequired,modelResource,prefixRetired,pieceRetired
};
struct OriginalWreckPrefixRetirementResult50LA {
    OriginalWreckPrefixRetirementStatus50LA status=OriginalWreckPrefixRetirementStatus50LA::binding;
    std::uint32_t sourcePC=0x142590u;
    std::size_t referencesReleased=0,objectsDestroyed=0;
};
// Native continuation state belongs to the same retained authored occurrence
// as its model/body/query/effect owners. No next-piece selection or reset is
// provided by this stage; the established first-piece entry points remain.
struct OriginalWreckPieceProgress50LA {
    bool constructorAttempted=false;
    bool modelRetirementAttempted=false,geometryReferenceReleased=false,appended22C1B8=false,pieceRetired=false;
    bool e0ConstructorAttempted=false,e0PrefixRetired=false,e0PublicationAttempted=false,e0NativeLoadAttempted=false,e0FreshPrepareAttempted=false,e0FreshAppended=false;
    bool contactSuffixAttempted=false,contactSuffixRetired=false,dcReached=false,d8Reached=false;
    std::size_t contactSuffixDestroyed=0;
    bool e4ConstructorAttempted=false,e4PrefixRetired=false,e4PublicationAttempted=false;
    bool e8ConstructorAttempted=false,e8PrefixRetired=false,mutualConstructionAttempted=false;
    bool retirementAttempted=false,freshModelLoadAttempted=false,modelAppendAttempted=false,pieceSetupAttempted=false;
    bool warmModelLoadAttempted=false; //One-shot26E9F0 continuation; never replayed.
    OriginalWreckConstructorPrefixResult50LA prefixResult{};
    // Completed piece receipt is never overwritten by collection retirement.
    OriginalWreckPrefixRetirementResult50LA retirementResult{};
};
// Native backing for ONE actual authored occurrence. Its body/gravity/Entity
// storage is not initialized by any source constructor merely by allocation.
// In particular it is not a disguised one-piece collection/group owner.
class OriginalWreckPieceStorage50LA final {
    friend class OriginalWreckRacerStorage50LA;
    OriginalWreckPieceProgress50LA progress_;
    // One native source invocation, not an inferred atomic tuning receipt.
    struct NativeTuningHistory {
        bool entered=false,complete=false,gravityStored=false;
        std::uint32_t pc=0x22c310u;
        std::optional<std::size_t> publishedSlot;
        unsigned registrationStores=0; //44,A4,A8,48,30, in source order
        std::unique_ptr<OriginalRacerModelSceneOwner50LA::SourceLocalBounds28D670> bounds;
        std::array<std::uint32_t,4> center40{},half50{};
    } nativeTuning_;

    bool nativePreBodyRetired_=false;
    std::optional<OriginalRacerSceneObject50LA*> sourceModel14C_;
    OriginalWreckAssets50LA* assets_=nullptr; // owned by the adopted inventory
    std::size_t collectionOrdinal_=0,pieceOrdinal_=0,sceneCount_=0,componentCount_=0;
    //Only absent source lanes, never a duplicate physical/body authority.
    struct FreshBodySource {
        bool attempted=false;
        std::optional<SpringVector4_48N> vector40,vector50,vectorF0;
        std::optional<std::uint32_t> word30,word284,method290,count264,flags200;
        std::optional<const void*> allocation28C;
        std::optional<OriginalRacerSceneObject50LA*> model288;
        std::optional<OriginalWreckPhysical50LA* const*> gravity50;
        std::optional<std::array<char,0x110>> filename60;
    } freshBody_;
    struct FreshE4Source {
        bool attempted=false;
        //Conservative native stack170 projection, not an asserted guest capacity.
        std::optional<std::array<char,256>> filename170;
    } freshE4_;
    OriginalWreckPhysical50LA physical_;
    OriginalWreckPieceFields50LA piece_;
    OriginalWreckGravity50LA gravity_;
    OriginalWreckDetector50LA detector_;
    struct FreshContactDC {
        bool attempted=false,constructed=false,appended=false,retired=false;
        const std::array<std::uint32_t,4>* origin4435A0=nullptr;
        const SpringRecord48N* source80=nullptr;const SpringRecord48N* source8C=nullptr;
        std::array<const OriginalRacerSceneObject50LA*,50> pointScenes{};
        std::array<std::uint8_t,50> pointFlagsKnownMask{};
    } freshContactDC_;
    struct FreshDetectorD8 {
        bool attempted=false,constructed=false,bound=false,appended=false,retired=false;
        const std::array<std::uint32_t,4>* origin443550=nullptr;
        const std::array<float,4>* local433B10=nullptr;
        const OriginalWreckTerrainProvider50LA* const* terrain330Field=nullptr;
    } freshDetectorD8_;
    std::vector<std::uint16_t> pointStack_; // cold native DFS workspace only
    //165EC8/165ED0 retain separate native zone namespaces, consumed by
    //166750 visibility28E8A0 and166730/166740 collision120070.
    // Original B4/B8 operands are SearchNode pointers. Native numeric refs below
    // are projections into two distinct registries, not replacement source words.
    const void* sourceVisibilityNodeB4_=nullptr;
    const void* sourceCollisionNodeB8_=nullptr;
    // Unknown before actual native setup stores, never planned-slot exposure.
    std::optional<OriginalWreckComponent50LA*> sourceBodyD0_,sourceGravityD4_;
    std::optional<OriginalWreckComponent50LA*> sourceE0_,sourceDC_,sourceD8_,sourceE4_,sourceE8_;
    ResetVisibilityWorld50EY* visibilityWorldB4_=nullptr;
    LiveZoneWorld50EN* collisionWorldB8_=nullptr;
    ZoneLifecycle50CL* collisionLifecycleB8_=nullptr;
    CollectionRef50BX visibilityZoneB4_=0,collisionZoneB8_=0;
    bool setupZonesBound_=false;
    // Model/PSG Entity identity belongs ONLY to this canonical owner. Source
    // resource facets stay empty until their genuine load phases are reached.
    OriginalRacerModelSceneOwner50LA modelOwner_;
    rr::assets::OriginalStandardMaterialReferences50LA modelMaterials_;
    std::shared_ptr<rr::assets::OriginalStandardModelGeometry50LA> modelGeometry_;
    OriginalRacerModelPackets50LA modelPackets_;
    std::unique_ptr<OriginalWreckComponent50LA*[]> componentSlots_;
    OriginalWreckEffectFields50LA effects_;
    OriginalWreckEffectArrays50LA effectArrays_;
    OriginalWreckComponent50LA* dust168_=nullptr;
    OriginalWreckComponent50LA* splash16C_=nullptr; //native unpublished storage until actual source stores
    OriginalWreckPieceEffectInputs50LA effectInputs_{};
    const OriginalWreckEffectResourceContext50LA* effectResource_=nullptr;
    OriginalWreckComponent50LA* effectParent_=nullptr;
    bool effectInputsCommitted_=false;
    bool freshEffectCountWritten_=false,freshEffectInputsAttempted_=false;
    OriginalWreckSplashOwner50LA* freshSplashOwner_=nullptr;
    bool freshSplashAttempted_=false;
    OriginalWreckDustOwner50LA* freshDustOwner_=nullptr; //Native borrower of actual source168 owner.
    bool freshDustAttempted_=false;
    OriginalWreckFireOwner50LA* freshFireOwner_=nullptr; //External typed borrow; never owned by164.
    bool freshFireAttempted_=false;
    OriginalWreckSmokeOwner50LA* freshSmokeOwner_=nullptr; //External owner borrow, never owned by160.
    bool freshSmokeAttempted_=false;
    std::size_t freshSmokeAttempts_=0,freshFireAttempts_=0; //Monotone native attempts; no source pointer shadow list.
    const WaterHeightProvider50JC* actualWaterB0_=nullptr;
    const WaterHeightGlobals50JC* actualWaterGlobals_=nullptr;
    std::unique_ptr<OriginalWreckComponent50LA[]> components_;
    // One receiver/query owner per actual occurrence. Declaration order keeps
    // these borrows alive through query-domain token revocation on destruction.
    OriginalWreckPieceQueryOwner50LA queryOwner_;
    OriginalWreckQueries50LA queries_;
public:
    const OriginalWreckPieceProgress50LA& sourceProgress()const noexcept{return progress_;}
    OriginalWreckPieceStorage50LA()=default;
    OriginalWreckPieceStorage50LA(const OriginalWreckPieceStorage50LA&)=delete;
    OriginalWreckPieceStorage50LA& operator=(const OriginalWreckPieceStorage50LA&)=delete;
    OriginalWreckPieceStorage50LA(OriginalWreckPieceStorage50LA&&)=delete;
    OriginalWreckPieceStorage50LA& operator=(OriginalWreckPieceStorage50LA&&)=delete;
    std::size_t collectionOrdinal()const noexcept{return collectionOrdinal_;}
    std::size_t pieceOrdinal()const noexcept{return pieceOrdinal_;}
    const OriginalWreckAssets50LA& assets()const noexcept{return *assets_;}
    std::size_t sceneCount()const noexcept{return sceneCount_;}
    std::size_t componentCapacity()const noexcept{return componentCount_;}
    const OriginalWreckPieceQueryOwner50LA& queryReceiverBacking()const noexcept{return queryOwner_;}
    // Read-only cold inspection. No public constructor-completion flag or
    // mutable body/query/camera view exists at this stage.
    const OriginalWreckPhysical50LA& physicalBacking()const noexcept{return physical_;}
    bool effectCountCommitted()const noexcept{return effectInputsCommitted_;}
    std::uint32_t effectCount174()const noexcept{return effects_.count174;}
    bool effectInputsStillBound()const noexcept {
        return effectInputsCommitted_&&effectInputs_.resourceContextField&&effectInputs_.particleParentField&&
            *effectInputs_.resourceContextField==effectResource_&&*effectInputs_.particleParentField==effectParent_;
    }
    const OriginalWreckComponent50LA* componentAt(std::size_t index)const noexcept {
        return index<componentCount_?componentSlots_[index]:nullptr;
    }
};
class OriginalWreckCollectionStorage50LA final {
    friend class OriginalWreckRacerStorage50LA;
    std::size_t ordinal_=0,count_=0;
    std::uint32_t sourceCount30_=0;
    std::optional<const ::ra_cold_aggregate50LA::Owner*> sourceWorld40_;
    bool freshLoadAttempted_=false; // Separate source-first22BFD0 admission.
    std::size_t sourceSlots38_=0,completedPieceConstructors_=0;
    //Fresh loop source locals: SAME completedPieceConstructors_ owns s0.
    bool freshPieceIndexKnown_=false;
    std::optional<std::uint32_t> freshPieceByteOffset_; //Actual s2, not written-slot extent.
    bool constructorAttempted_=false,retirementAttempted_=false;
    bool nativeSourceMode_=false,nativeConstructorOnly_=false;
    OriginalWreckPieceStorage50LA* const* sourceArray38_=nullptr;
    const void* source34_=nullptr;const void* source3C_=nullptr;

    OriginalWreckPrefixRetirementResult50LA retirementResult_{};
    OriginalWreckComponent50LA collectionComponent_,groupComponent_;
    bool nativeGroupAttempted_=false,nativeGroupConstructed_=false,nativeGroupBound_=false;
    bool nativeGroupAppended_=false,nativeGroupRetired_=false;
    std::array<bool,2> nativeGroupChildAttempted_{};
    OriginalWreckComponent50LA* sourceGroupC0_=nullptr;
    OriginalWreckComponent50LA* sourceGroupC4_=nullptr; //unwritten until genuine2CBFC0
    std::uint32_t nativeGroupPC_=0x22c278u;
    bool freshGroupContextChecked_=false;
    OriginalWreckPieceEffectInputs50LA freshGroupInputs_{};
    const OriginalWreckEffectResourceContext50LA* freshGroupContext_=nullptr;
    OriginalWreckComponent50LA* freshGroupParent_=nullptr;
    std::optional<std::array<std::uint32_t,4>> freshGroupPendingOriginV0_;
    std::optional<std::array<std::uint32_t,4>> freshGroupStack50_;
    bool freshGroupSparksPublished_=false;
    bool freshGroupSparksSetupAttempted_=false;
    bool freshGroupSparksConfigAttempted_=false;
    std::uint32_t freshGroupSparksSetupPC_=0;
    std::optional<std::uint32_t> freshGroupPendingExplosionSize_;
    std::optional<const OriginalRacerSceneObject50LA*> freshGroupModel288_;

    OriginalWreckGroupSparksOwner50LA* freshGroupSparks_=nullptr; //Borrowed typed owner; external forest owns lifetime.


    bool nativeLoadDeactivateAttempted_=false,nativeLoadDeactivateComplete_=false;
    std::size_t nativeDeactivatePieces_=0;
    bool nativeRacerAppendAttempted_=false,nativeRacerAppended_=false,nativeRacerTailUnlinked_=false;
    OriginalWreckComponent50LA* nativeRacerParent_=nullptr;
    OriginalWreckComponent50LA* nativeRacerPrevious_=nullptr;
    std::size_t nativeRacerExclusionCursor_=0;
    bool nativeRacerExclusionFailed_=false;
    std::unique_ptr<OriginalWreckPieceStorage50LA[]> pieces_;
    // Genuine planned pointer order for collection38, not yet a source
    // publication. Each slot names its distinct stable occurrence owner.
    std::unique_ptr<OriginalWreckPieceStorage50LA*[]> ordered38_;
    // Destroy group backing before every piece/component it may later borrow.
    OriginalWreckGroupArrays50LA group_;
public:
    OriginalWreckCollectionStorage50LA()=default;
    OriginalWreckCollectionStorage50LA(const OriginalWreckCollectionStorage50LA&)=delete;
    OriginalWreckCollectionStorage50LA& operator=(const OriginalWreckCollectionStorage50LA&)=delete;
    OriginalWreckCollectionStorage50LA(OriginalWreckCollectionStorage50LA&&)=delete;
    OriginalWreckCollectionStorage50LA& operator=(OriginalWreckCollectionStorage50LA&&)=delete;
    const OriginalWreckGroupArrays50LA& groupBacking()const noexcept{return group_;}
    const OriginalWreckComponent50LA& collectionComponentBacking()const noexcept{return collectionComponent_;}
    std::size_t ordinal()const noexcept{return ordinal_;}
    std::size_t plannedPieceCount()const noexcept{return count_;}
    //22C16C publishes each source array slot before its piece setup; Entity
    //append22C1B8 is a distinct later phase. This extent is not activation.
    std::size_t reachedSourcePieceSlotCount()const noexcept{return sourceSlots38_;}
    std::size_t completedPieceConstructorCount()const noexcept{return completedPieceConstructors_;}
    const OriginalWreckPrefixRetirementResult50LA& sourceRetirement()const noexcept{return retirementResult_;}
    const OriginalWreckPieceStorage50LA* piece(std::size_t i)const noexcept {
        return ordered38_&&pieces_&&i<count_?&pieces_[i]:nullptr;
    }
    //2CC060's future primary is piece0; capacity is count-1. This describes
    //authoring only, not source group40/34 or aggregate construction completion.
    const OriginalWreckPieceStorage50LA* plannedPrimary()const noexcept{return piece(0);}
    std::size_t plannedNonprimaryCount()const noexcept{return count_?count_-1u:0u;}
};
enum class OriginalWreckRacerStorageStatus50LA : std::uint8_t {coldBacking,binding,inventory,capacity,allocation};
struct OriginalWreckRacerStorageResult50LA {
    OriginalWreckRacerStorageStatus50LA status=OriginalWreckRacerStorageStatus50LA::binding;
    std::size_t collectionOrdinal=0,pieceOrdinal=0,collections=0,pieces=0,componentCapacity=0;
};
// Slots supplied by the actual canonical world allocator, in collection-major,
// piece-major order. Each triple retains the source E0/E4/E8 order.
struct OriginalWreckPieceQuerySlots50LA {
    std::array<CollectionRef50BX,3> refs{};
    std::array<std::uint32_t,3> identities{};
};
enum class OriginalWreckOrderedQueryStatus50LA : std::uint8_t {
    coldPrepared,binding,capacity,allocation,queryPreparation
};
struct OriginalWreckOrderedQueryResult50LA {
    OriginalWreckOrderedQueryStatus50LA status=OriginalWreckOrderedQueryStatus50LA::binding;
    std::size_t collectionOrdinal=0,pieceOrdinal=0,preparedPieces=0;
    std::uint32_t sourcePC=0x165e90u;
    OriginalWreckQueryResult50LA query{};
};
// Genuine application identities/globals for the first reached source piece.
// Keys are the actual native addresses of collection and eight non-model
// components (piece,D0,D4,E0,DC,D8,E4,E8). Model/PSG keys belong to their owner.
struct OriginalWreckConstructorPrefixBinding50LA {
    std::uint64_t sessionGeneration=0,racerKey=0,collectionKey=0;
    const std::uint64_t* pieceComponentKeys=nullptr;
    std::size_t pieceComponentKeyCount=0;
    // These same application fields are retained through source retirement.
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
    const OriginalWreckConstructorGlobals50LA* constructorGlobals=nullptr;
    const SpringVector4_48N* actualVector4431C0=nullptr;
    OriginalEntityTypeRegistry50LA* actualTypes383408=nullptr;
    const WaterHeightProvider50JC* const* actualWaterB0Field=nullptr;
    const WaterHeightGlobals50JC* actualWaterGlobals=nullptr;
};
// Source-owned zone projections, not inferred host/current-camera zones.
// B4/B8 select distinct visibility/collision namespaces; equal numeric refs
// never make those worlds interchangeable.
struct OriginalWreckSetupZones50LA {
    ResetVisibilityWorld50EY* visibility4C0=nullptr;
    const CollectionRef50BX* visibilityZoneField=nullptr;
    LiveZoneWorld50EN* collision4C4=nullptr;
    ZoneLifecycle50CL* collisionLifecycle=nullptr;
    const CollectionRef50BX* collisionZoneField=nullptr;
    // Required by the private adopted-source path only; standalone callers keep
    // their existing field contract. Borrow lifetime is retained by actual World.
    const void* nativeVisibilityNode=nullptr;
    const void* nativeCollisionNode=nullptr;
};
struct OriginalWreckContactConstructorBinding50LA {
    const SpringVector4_48N* actualVector443550=nullptr;
    const SpringVector4_48N* actualVector4435A0=nullptr;
    const OriginalWreckTerrainProvider50LA* const* actualTerrain330Field=nullptr;
};
// Own the complete original inventory and the nested native arrays together.
// The inventory outlives every borrower by declaration/destruction order.
// This class cannot publish an active racer374/death-camera view. It has no
// gameplay destructor callbacks. Its bounded constructor reaches the first E0
// and contact/detector append; retirement is never inferred from backing release.
class OriginalWreckRacerStorage50LA final {
    friend class ::ra_cold_roster50LA::WreckBacking;
    friend class ::ra_cold_roster50LA::NativeWreckEffectChildren;
    struct Storage {
        std::unique_ptr<OriginalWreckAssetInventory50LA> inventory; // dies LAST
        std::uint64_t sessionGeneration=0,racerKey=0;
        std::size_t count=0,totalPieces=0,totalComponents=0;
        bool identityBound=false,nativeIdentityBindingAttempted=false,nativeColdRetired=false;
        bool nativeSourceMode=false,nativeSourcePrefixRetired=false;
        std::size_t nativeCollectionCursor=0,nativePublished374=0;
        const OriginalWreckComponent50LA* nativeFreshRacer=nullptr;
        const std::uint32_t* nativeFreshCount370=nullptr; //Actual reached158920 output field.
        std::uint32_t nativeFreshCountValue=0; //History guard, never an authoring-produced source count.
        OriginalWreckConstructorGlobalViews50LA nativeGlobals;
        const void* nativeGravity4431C0=nullptr;

        // Declared before collections so every borrowing query dies first.
        OriginalWreckSceneDomain50LA modelScenes;
        bool queryPreparationAttempted=false,queriesColdPrepared=false,queryBorrowsReleased=false;
        std::size_t preparedQueries=0;
        OriginalEntityTypeRegistry50LA* actualTypes383408=nullptr;
        const OriginalWreckConstructorGlobals50LA* actualConstructorGlobals=nullptr;
        const SpringVector4_48N* actualGravityVector4431C0=nullptr;
        std::uint32_t* actualObjectCount423F60=nullptr;
        const std::uint32_t* actualAttachmentLock423F58=nullptr;
        // Every component this racer's wreck chain constructs. Sized once at
        // adoption from the racer's real authored count (totalComponents);
        // every append is checked against size() before the write.
        struct PrefixRows {
            std::unique_ptr<OriginalWreckComponent50LA*[]> rows;std::size_t capacity=0;
            OriginalWreckComponent50LA*& operator[](std::size_t i)noexcept{return rows[i];}
            OriginalWreckComponent50LA* operator[](std::size_t i)const noexcept{return rows[i];}
            OriginalWreckComponent50LA** data()noexcept{return rows.get();}
            std::size_t size()const noexcept{return capacity;}
        } prefixComponents;
        OriginalWreckComponentDomain50LA prefixDomain{};
        std::unique_ptr<OriginalWreckCollectionStorage50LA[]> collections;
        std::unique_ptr<OriginalWreckCollectionStorage50LA*[]> ordered374;
        ~Storage(){
            // Runs before collections die. Cold allocation is freely releasable;
            // reached source references require real104970/slot08 retirement.
            for(std::size_t i=0;i<prefixDomain.count;++i)
                if(!prefixComponents[i]||prefixComponents[i]->reference00)std::terminate();
        }
    };
    std::unique_ptr<Storage> storage_;
    bool nativeTuningAttempted_=false,nativeTuningComplete_=false;
    std::uint32_t nativeTuningPC_=0x215af8u;
    #include "original_wreck_native_tuning50la.inl"

    // All callers retain their existing storage/adoption admission. These
    // selectors deliberately remain fixed to the established first occurrence.
    OriginalWreckCollectionStorage50LA& firstCollection()noexcept{return storage_->collections[0];}
    const OriginalWreckCollectionStorage50LA& firstCollection()const noexcept{return storage_->collections[0];}
    OriginalWreckPieceProgress50LA& firstProgress()noexcept{return firstCollection().pieces_[0].progress_;}
    const OriginalWreckPieceProgress50LA& firstProgress()const noexcept{return firstCollection().pieces_[0].progress_;}
    template<class Binding> bool sameExternalEffectForest(const Binding& binding)const noexcept {
        if(!sourceIdentityBound()||!binding.domain||binding.domain->sessionGeneration!=storage_->sessionGeneration||
           binding.domain==&storage_->prefixDomain)return false;
        for(std::size_t ci=0;ci<storage_->count;++ci)for(std::size_t pi=0;pi<storage_->collections[ci].count_;++pi){
            const auto& prior=storage_->collections[ci].pieces_[pi];const auto& arrays=prior.effectArrays_;
            if(arrays.externalDomain_&&(prior.effectParent_!=binding.particleParent||
               arrays.externalDomain_!=binding.domain||arrays.externalStorage_!=binding.domainStorage||
               arrays.externalCapacity_!=binding.domainCapacity))return false;
        }
        return true;
    }
    OriginalWreckPieceStorage50LA* constructionOccurrence(std::size_t ci,std::size_t pi)noexcept {
        if(!sourceIdentityBound()||ci>=storage_->count||!orderedQueriesColdPrepared())return nullptr;
        auto& c=storage_->collections[ci];
        if(!c.constructorAttempted_||c.retirementAttempted_||!c.ordered38_||pi>=c.count_||
           c.sourceSlots38_!=pi+1u||c.completedPieceConstructors_!=pi)return nullptr;
        auto& p=c.pieces_[pi];const auto& progress=p.progress_;
        if(c.ordered38_[pi]!=&p||!progress.constructorAttempted||progress.retirementAttempted||
           progress.modelRetirementAttempted||progress.pieceRetired||progress.e0PrefixRetired||
           progress.contactSuffixRetired||progress.e4PrefixRetired||progress.e8PrefixRetired)return nullptr;
        auto* linked=c.collectionComponent_.firstChild18;
        for(std::size_t i=0;i<pi+(progress.appended22C1B8?1u:0u);++i){
            const auto& previous=c.pieces_[i];const auto& state=previous.progress_;
            if(!state.appended22C1B8||state.retirementAttempted||state.pieceRetired||state.e0PrefixRetired||
               state.contactSuffixRetired||state.e4PrefixRetired||state.e8PrefixRetired||
               linked!=previous.componentSlots_[0])return nullptr;
            linked=linked->next14;
        }
        if(linked)return nullptr;
        OriginalWreckDomainRows50LA live;if(!liveRows(live))return nullptr;
        OriginalWreckComponentDomain50LA domain{};
        if(!liveRetirementDomain(live,domain)||
           !original_wreck_component_detail50LA::domain(storage_->prefixDomain))return nullptr;
        return &p;
    }
    OriginalWreckPieceStorage50LA* retirementOccurrence(std::size_t ci,std::size_t pi)noexcept {
        if(!sourceIdentityBound()||ci>=storage_->count)return nullptr;
        auto& c=storage_->collections[ci];
        if(!c.constructorAttempted_||pi>=c.sourceSlots38_||c.sourceSlots38_>c.count_)return nullptr;
        auto& p=c.pieces_[pi];
        if(!p.componentSlots_)return nullptr;
        if(c.retirementAttempted_&&p.progress_.pieceRetired)return &p;
        if(!c.ordered38_||c.ordered38_[pi]!=&p)return nullptr;
        if(!p.progress_.pieceRetired)for(std::size_t later=pi+1u;later<c.sourceSlots38_;++later)
            if(!c.pieces_[later].progress_.pieceRetired)return nullptr;
        return &p;
    }
    bool nativeRacerChainCurrent(const OriginalWreckCollectionStorage50LA& c)const noexcept {
        if(!c.nativeRacerAppended_)return true;
        const auto* root=c.nativeRacerParent_;const auto& child=c.collectionComponent_;
        if(!root||root->kind!=OriginalWreckComponentKind50LA::racer27E130||root->reference00!=1u||
           root->objectKey!=storage_->racerKey||root->sessionGeneration!=storage_->sessionGeneration||
           root->parent1C||root->previous10||root->next14||child.parent1C!=root||
           child.previous10!=c.nativeRacerPrevious_)return false;
        const OriginalWreckComponent50LA* previous=nullptr;std::size_t left=maxOriginalWreckComponents50LA;
        bool found=false;
        for(auto* n=root->firstChild18;n;n=n->next14){
            if(!left--||!n->reference00||!n->objectKey||n->sessionGeneration!=root->sessionGeneration||
               n->parent1C!=root||n->previous10!=previous)return false;
            if(n==&child)found=true;
            previous=n;
        }
        if(c.nativeRacerTailUnlinked_)return !found&&!child.next14&&previous==c.nativeRacerPrevious_;
        if(!found)return false;
        //Any following source sibling is the next genuinely appended collection,
        //never a fabricated tail or an unrelated unrepresented producer.
        if(child.next14){
            const auto ordinal=static_cast<std::size_t>(&c-storage_->collections.get());
            if(ordinal+1u>=storage_->count)return false;
            const auto& next=storage_->collections[ordinal+1u];
            if(!next.nativeRacerAppended_||next.nativeRacerTailUnlinked_||
               child.next14!=&next.collectionComponent_)return false;
        }
        return true;
    }
    // Borrow every live canonical Entity from retained occurrence backing.
    // Zero-reference source history stays retained but cannot enter a live domain.
    // The model owner joins its own Entity; its PSG rows retire separately.
    // Rows for a live domain over this racer: its real component count plus
    // the two outside racer anchors (racer, its pre-existing tail child).
    bool liveRows(OriginalWreckDomainRows50LA& rows)const noexcept {
        return storage_&&storage_->totalComponents<=std::numeric_limits<std::size_t>::max()-2u&&
            sizeOriginalWreckDomainRows50LA(rows,storage_->totalComponents+2u);
    }
    bool liveRetirementDomain(
        OriginalWreckDomainRows50LA& rows,
        OriginalWreckComponentDomain50LA& out,
        const OriginalWreckPieceStorage50LA* excludedModel=nullptr)const noexcept {
        if(!sourceIdentityBound())return false;
        out={storage_->sessionGeneration,rows.data(),0};
        auto add=[&](OriginalWreckComponent50LA* c){
            if(!c)return true; //cold, not-yet-created canonical PSG row
            if(!c->reference00)return c->kind==OriginalWreckComponentKind50LA::unbound||
                c->kind==OriginalWreckComponentKind50LA::entity141300;
            for(std::size_t i=0;i<out.count;++i)if(rows[i]==c)return true;
            if(out.count==rows.size())return false;
            rows[out.count++]=c;return true;
        };
        for(std::size_t ci=0;ci<storage_->count;++ci){
            auto& c=storage_->collections[ci];
            if(!add(&c.collectionComponent_)||!add(&c.groupComponent_))return false;
            if(c.nativeRacerAppended_&&c.collectionComponent_.reference00){
                if(!nativeRacerChainCurrent(c)||!add(c.nativeRacerParent_)||!add(c.nativeRacerPrevious_))return false;
            }
            for(std::size_t pi=0;pi<c.count_;++pi){
                auto& p=c.pieces_[pi];
                for(std::size_t i=0;i<p.componentCount_;++i){
                    if(&p==excludedModel&&i>=1u&&i<=p.sceneCount_)continue;
                    const auto* historical=p.componentSlots_[i];
                    // Native tail-first cleanup retains destroyed model/scene
                    // kinds. Admit no arbitrary zero-ref history: require the
                    // same actual owner, released row and enclosing piece end.
                    if(storage_->nativeSourceMode&&p.nativePreBodyRetired_&&p.progress_.pieceRetired&&
                       p.modelOwner_.released()&&i>=1u&&i<=p.sceneCount_&&historical&&!historical->reference00){
                        bool released=i==1u&&historical==p.modelOwner_.modelEntity8()&&p.modelOwner_.model_.released;
                        if(i>1u)for(const auto& row:p.modelOwner_.rows_)
                            if(row && &row->entity()==historical&&row->constructed&&row->released)released=true;
                        if(!released)return false;
                        continue;
                    }
                    if(!add(p.componentSlots_[i]))return false;
                }
            }
        }
        if(!original_wreck_component_detail50LA::domain(out))return false;
        if(!excludedModel)for(std::size_t i=0;i<out.count;++i){
            const auto& c=*rows[i];
            bool externalAnchor=false,dyingRoot=false,sourceCollection=false;
            if(storage_->nativeSourceMode)for(std::size_t n=0;n<storage_->count;++n){const auto& collection=storage_->collections[n];
                sourceCollection=sourceCollection||&c==&collection.collectionComponent_;
                if(collection.nativeRacerAppended_&&collection.collectionComponent_.reference00){
                    externalAnchor=externalAnchor||&c==collection.nativeRacerParent_||&c==collection.nativeRacerPrevious_;
                    dyingRoot=dyingRoot||(&c==&collection.collectionComponent_&&collection.nativeRacerTailUnlinked_);
                }
            }
            if(externalAnchor&&!sourceCollection)continue; //only unrelated Racer/7FC anchor subtrees; owned collections remain strict
            if(dyingRoot){
                if(c.firstChild18&&(!original_wreck_component_detail50LA::contains(out,c.firstChild18)||
                   c.firstChild18->parent1C!=&c||c.firstChild18->previous10))return false;
                continue; //source parent/previous remain history after real caller tail unlink
            }
            for(const auto* link:{c.parent1C,c.previous10,c.next14,c.firstChild18})
                if(link&&!original_wreck_component_detail50LA::contains(out,link))return false;
            if(c.previous10&&(c.previous10->next14!=&c||c.previous10->parent1C!=c.parent1C))return false;
            if(c.next14&&(c.next14->previous10!=&c||c.next14->parent1C!=c.parent1C))return false;
            if(c.firstChild18&&(c.firstChild18->parent1C!=&c||c.firstChild18->previous10))return false;
            if(c.parent1C&&!c.previous10&&c.parent1C->firstChild18!=&c)return false;
        }
        return true;
    }
public:
    OriginalWreckRacerStorage50LA()=default;
    OriginalWreckRacerStorage50LA(const OriginalWreckRacerStorage50LA&)=delete;
    OriginalWreckRacerStorage50LA& operator=(const OriginalWreckRacerStorage50LA&)=delete;
    OriginalWreckRacerStorage50LA(OriginalWreckRacerStorage50LA&&)=delete;
    OriginalWreckRacerStorage50LA& operator=(OriginalWreckRacerStorage50LA&&)=delete;

    // Native cold transaction only. On failure, caller retains inventory and
    // all its exact asset addresses; on success it transfers exclusive ownership.
    // Existing nonowning asset borrows remain stable and must obey this owner's
    // lifetime. Identity values must be those of the actual retained racer.
    OriginalWreckRacerStorageResult50LA adoptCompleteInventory(
        std::unique_ptr<OriginalWreckAssetInventory50LA>& inventory,
        std::uint64_t sessionGeneration,std::uint64_t racerKey)noexcept {
        return adoptInventoryBacking(inventory,sessionGeneration,racerKey,false);
    }
    struct NativeColdReservation {
        OriginalWreckQueries50LA* queries=nullptr;
        const OriginalWreckAssets50LA* assets=nullptr;
    };
private:
    bool sourceIdentityBound()const noexcept {
        return storage_&&!storage_->nativeColdRetired&&storage_->identityBound&&storage_->sessionGeneration&&storage_->racerKey&&
            storage_->inventory&&storage_->inventory->modelSourcesBound();
    }
    // Native backing withdrawal only. Reached source objects require the
    // separate genuine source retirement before this native-backing phase.
    bool canRetireNativeColdQueries(const LiveZoneWorld50EN* world)const noexcept {
        if(!storage_)return true;
        if((storage_->prefixDomain.count||storage_->actualObjectCount423F60)&&!storage_->nativeSourcePrefixRetired)return false;
        if(storage_->nativeSourcePrefixRetired)for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(!storage_->prefixComponents[i]||storage_->prefixComponents[i]->reference00)return false;
        for(std::size_t c=0;c<storage_->count;++c){
            const auto& collection=storage_->collections[c];
            if((!storage_->nativeSourcePrefixRetired&&(collection.constructorAttempted_||collection.sourceSlots38_||
               collection.sourceCount30_||collection.completedPieceConstructors_))||
               collection.collectionComponent_.reference00||collection.groupComponent_.reference00)return false;
            for(std::size_t i=0;i<collection.count_;++i){const auto& piece=collection.pieces_[i];
                if(piece.setupZonesBound_||piece.queries_.constructed())return false;
                if(piece.progress_.constructorAttempted&&!piece.nativePreBodyRetired_){
                    // A stopped load can mark its attempt before the first
                    // source piece allocation/22C16C. Do not turn that flag
                    // into a counted object or fabricate a retirement receipt.
                    if(!storage_->nativeSourcePrefixRetired||piece.modelOwner_.model_.constructed||
                       piece.modelMaterials_.started()||piece.modelGeometry_||
                       piece.modelPackets_.constructed240740()||piece.effectArrays_.attempted())return false;
                    for(std::size_t j=0;j<piece.componentCount_;++j){const auto* entity=piece.componentSlots_[j];
                        if(entity&&(entity->kind!=OriginalWreckComponentKind50LA::unbound||entity->reference00||
                           entity->parent1C||entity->previous10||entity->next14||entity->firstChild18))return false;
                    }
                }
                for(std::size_t j=0;j<8u;++j)if(piece.components_[j].reference00)return false;
                const auto& queries=piece.queries_;
                if(!queries.canWithdrawFreshE0())return false;
                if(queries.prepared()&&(!world||!(queries.hasFullE0PublicationHistory()?
                    queries.canReleaseSourceRetiredWorld(*world):queries.canReleaseUnpublishedWorld(*world))))return false;
            }
        }
        return !storage_->queryPreparationAttempted||storage_->queryBorrowsReleased||world;
    }
    bool retireNativeColdQueries(const LiveZoneWorld50EN* world)noexcept {
        if(!canRetireNativeColdQueries(world))return false;
        if(!storage_||storage_->nativeColdRetired)return true;
        if(storage_->queryPreparationAttempted&&!storage_->queryBorrowsReleased){
            bool sourceQueries=false;
            for(std::size_t ci=0;ci<storage_->count;++ci)
                for(std::size_t pi=0;pi<storage_->collections[ci].count_;++pi)
                    sourceQueries=sourceQueries||storage_->collections[ci].pieces_[pi].queries_.hasFullE0PublicationHistory();
            if(!world||!(sourceQueries?releaseSourceRetiredQueries(*world):releaseUnpublishedQueries(*world)))return false;
        }
        for(std::size_t ci=0;ci<storage_->count;++ci)
            for(std::size_t pi=0;pi<storage_->collections[ci].count_;++pi)
                storage_->collections[ci].pieces_[pi].queries_.withdrawFreshE0();
        //Fresh source count/racer borrows end only at native withdrawal,
        //after genuine counted retirement (when any constructor was reached).
        storage_->nativeFreshRacer=nullptr;storage_->nativeFreshCount370=nullptr;
        storage_->nativeColdRetired=true;return true;
    }
    bool canReleaseNativeColdBacking()const noexcept {
        return !storage_||(storage_->nativeColdRetired&&
            (!storage_->queryPreparationAttempted||storage_->queryBorrowsReleased)&&
            !storage_->modelScenes.prepared()&&canRetireNativeColdQueries(nullptr));
    }
OriginalWreckPrefixRetirementResult50LA retireNativePieceBeforeE0(
    std::size_t collectionIndex,std::size_t pieceIndex,
    const LiveZoneWorld50EN& actualWorld)noexcept {
    return retirePieceBeforeE0Core(collectionIndex,pieceIndex,&actualWorld,false,false);
}
//A retired second prefix is NOT selected by the live-source gate. Its world
//borrows have ended and its payload arrays may be freed. Inspect only retained
//owner/Entity metadata and source aliases, never released array elements.
//Source-indexed second occurrence loop history; no readiness or copied cursor.
static bool freshSecondPairStopHistory(const OriginalWreckPieceStorage50LA& p)noexcept {
    using S=OriginalWreckConstructorPrefixStatus50LA;const auto& a=p.effectArrays_;
    if(a.pairsStopped_>a.count_||a.smokeWritten_>a.count_||a.fireWritten_>a.smokeWritten_||
       a.smokeWritten_>a.fireWritten_+1u||a.pairsStopped_>a.fireWritten_||
       a.fireWritten_>a.pairsStopped_+1u||a.smokeWritten_>a.pairsStopped_+1u||
       p.freshSmokeAttempts_>a.count_||p.freshFireAttempts_>a.count_)return false;
    if(a.freshPairByteOffset_&&*a.freshPairByteOffset_!=4u*a.pairsStopped_)return false;
    if((a.pairsStopped_||a.smokeWritten_||a.fireWritten_)&&(!a.freshPairByteOffset_||
       !p.freshEffectCountWritten_||!p.effects_.count174||(p.effects_.count174&0x80000000u)||
       a.count_!=p.effects_.count174||a.count_>512u||a.anchorsWritten_!=a.count_))return false;
    if(p.freshSmokeAttempts_!=a.smokeWritten_+(p.freshSmokeOwner_&&!p.freshSmokeOwner_->initialized_?1u:0u)||
       p.freshFireAttempts_!=a.fireWritten_+(p.freshFireOwner_&&!p.freshFireOwner_->initialized_?1u:0u))return false;
    if(a.pairsStopped_&&p.freshSmokeAttempts_==a.pairsStopped_&&p.freshFireAttempts_==a.pairsStopped_){
        const bool more=a.pairsStopped_<p.effects_.count174;
        if(p.freshDustAttempted_){
            //Only this genuine producer consumes the completed loop boundary.
            if(more||a.boundary_!=0x2cb7e0u||!p.freshDustOwner_||
               (p.freshDustOwner_->appended_&&(!p.freshDustOwner_->freshReaderComplete_||
                (!p.freshDustOwner_->retired_&&p.freshDustOwner_->sourceBoundary()!=0x2cb854u))))return false;
        }else if(p.progress_.prefixResult.status!=(more?S::effectNextPairRequired:S::effectDustRequired)||
           p.progress_.prefixResult.sourcePC!=(more?0x2cb6ccu:0x2cb7e0u)||
           a.boundary_!=p.progress_.prefixResult.sourcePC)return false;
    }
    return true;
}
//Only the second actual source increment, never a range-based cursor grant.
static bool freshSecondCursorAdvanced22C260(const OriginalWreckCollectionStorage50LA& c)noexcept {
    using S=OriginalWreckConstructorPrefixStatus50LA;
    if(c.count_<2u||!c.freshPieceIndexKnown_||c.completedPieceConstructors_!=2u||
       !c.freshPieceByteOffset_||*c.freshPieceByteOffset_!=8u||
       (c.sourceCount30_&0x80000000u)||c.sourceCount30_<=1u||c.sourceSlots38_!=2u||
       !c.ordered38_||c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&c.pieces_[1])return false;
    const auto& p=c.pieces_[1];const auto* q=p.queries_.storage_.get();
    if(!p.progress_.appended22C1B8||!q||!q->freshCollectionAttempted||!q->freshCollectionE0Selected||
       !q->freshNamedWaterBit||q->freshCollectionCursor!=2u||q->freshCollectionPC!=0x22c25cu)return false;
    const bool more=c.sourceCount30_>2u; //Signed source30 is proven positive above.
    return p.progress_.prefixResult.status==(more?S::nextPieceRequired:S::groupConstructorRequired)&&
        p.progress_.prefixResult.sourcePC==(more?0x22c0a0u:0x22c278u);
}
static bool freshSecondCursorCurrent(const OriginalWreckCollectionStorage50LA& c)noexcept {
    if(!c.freshPieceIndexKnown_||!c.freshPieceByteOffset_||
       (c.sourceCount30_&0x80000000u)||c.sourceCount30_<=1u)return false;
    return (c.completedPieceConstructors_==1u&&*c.freshPieceByteOffset_==4u)||freshSecondCursorAdvanced22C260(c);
}
//Reached caller/Query ledger only: safe after category payloads have been
//freed. Live category contents are independently checked by Queries preflight.
static bool freshSecondCollectionStatus(const OriginalWreckPieceStorage50LA& p,
    const OriginalWreckCollectionStorage50LA& c)noexcept {
    using S=OriginalWreckConstructorPrefixStatus50LA;
    const auto* q=p.queries_.storage_.get();const auto& result=p.progress_.prefixResult;
    if(!p.progress_.appended22C1B8||!q)return false;
    if(c.completedPieceConstructors_==2u)return &p==&c.pieces_[1]&&freshSecondCursorAdvanced22C260(c);
    if(!q->freshCollectionAttempted){
        return !q->freshCollectionE0Selected&&!q->freshCollectionCursor&&!q->freshCollectionPC&&
            !q->freshNamedWaterBit&&result.sourcePC==0x22c21cu&&
            (result.status==S::collectionFiltersRequired||result.status==S::collectionFiltersFailed);
    }
    if(!q->freshNamedWaterBit||q->freshCollectionCursor>2u)return false;
    const auto cursor=q->freshCollectionCursor;
    if(!q->freshCollectionE0Selected&&cursor)return false;
    const std::uint32_t pc=!q->freshCollectionE0Selected?0x22c21cu:
        cursor==0u?0x22c238u:cursor==1u?0x22c254u:0x22c25cu;
    return q->freshCollectionPC==pc&&result.sourcePC==pc&&
        result.status==(cursor==2u?S::stopped:S::collectionFiltersFailed);
}
//Exact source sibling topology only. This is not a generic non-tail
//retirement grant: actual first retirement still requires next14 NULL.
const OriginalWreckComponent50LA* freshLiveSecondSibling(
    const OriginalWreckPieceStorage50LA& first)const noexcept {
    using K=OriginalWreckComponentKind50LA;using S=OriginalWreckConstructorPrefixStatus50LA;
    if(!storage_||!storage_->count||storage_->nativeSourcePrefixRetired)return nullptr;
    const auto& c=storage_->collections[0];
    if(c.count_<2u||&first!=&c.pieces_[0]||c.sourceSlots38_!=2u||!c.ordered38_||
       c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[0]!=&first||c.ordered38_[1]!=&c.pieces_[1]||
       !freshSecondCursorCurrent(c))return nullptr;
    const auto& second=c.pieces_[1];const auto& progress=second.progress_;
    if(!first.componentSlots_||!first.components_||!second.componentSlots_||!second.components_||
       !first.progress_.appended22C1B8||first.progress_.pieceRetired||first.nativePreBodyRetired_||
       !progress.appended22C1B8||progress.pieceRetired||second.nativePreBodyRetired_||
       !freshSecondCollectionStatus(second,c))return nullptr;
    const auto* a=first.componentSlots_[0];const auto* b=second.componentSlots_[0];
    if(a!=&first.components_[0]||b!=&second.components_[0]||
       a->reference00!=1u||b->reference00!=1u||a->kind!=K::piece2CB4B0||b->kind!=K::piece2CB4B0||
       c.collectionComponent_.reference00!=1u||c.collectionComponent_.kind!=K::collection22BF00||
       c.collectionComponent_.firstChild18!=a||a->parent1C!=&c.collectionComponent_||a->previous10||
       a->next14!=b||b->parent1C!=&c.collectionComponent_||b->previous10!=a||b->next14)return nullptr;
    return b;
}
const OriginalWreckPieceStorage50LA* exactRetiredSecondPiece()const noexcept {
    using K=OriginalWreckComponentKind50LA;using R=OriginalWreckPrefixRetirementStatus50LA;
    if(!storage_||!storage_->count||storage_->nativeSourcePrefixRetired)return nullptr;
    const auto& c=storage_->collections[0];
    if(c.count_<2u||c.sourceSlots38_!=2u||!c.ordered38_||c.sourceArray38_!=c.ordered38_.get()||
       !freshSecondCursorCurrent(c))return nullptr;
    const auto& p=c.pieces_[1];const auto& r=p.progress_.retirementResult;
    if(!freshSecondPairStopHistory(p)||c.ordered38_[1]!=&p||!p.nativePreBodyRetired_||!p.progress_.pieceRetired||
       !p.progress_.constructorAttempted||
       r.status!=R::pieceRetired||r.sourcePC!=0x141418u||!r.objectsDestroyed||
       r.referencesReleased!=r.objectsDestroyed||!p.components_||!p.componentSlots_||
       p.componentCount_!=p.sceneCount_+8u||!p.sceneCount_||
       p.componentSlots_[0]!=&p.components_[0]||p.components_[0].reference00||
       p.components_[0].kind!=K::entity141300||p.components_[0].next14||p.components_[0].firstChild18||
       p.setupZonesBound_||p.visibilityWorldB4_||p.collisionWorldB8_||p.collisionLifecycleB8_)return nullptr;
    if(p.progress_.appended22C1B8){
        const auto& first=c.pieces_[0];const auto* predecessor=first.componentSlots_?first.componentSlots_[0]:nullptr;
        //Resolve the canonical live first root, never dereference the dead
        //second's historical previous10 to discover or validate an owner.
        if(!first.components_||predecessor!=&first.components_[0]||!first.progress_.appended22C1B8||
           predecessor->reference00!=1u||predecessor->kind!=K::piece2CB4B0||
           predecessor->parent1C!=&c.collectionComponent_||predecessor->previous10||predecessor->next14||
           c.collectionComponent_.firstChild18!=predecessor||
           p.components_[0].parent1C!=&c.collectionComponent_||p.components_[0].previous10!=predecessor||
           !freshSecondCollectionStatus(p,c))return nullptr;
    }else if(p.components_[0].parent1C||p.components_[0].previous10)return nullptr;
    std::size_t destroyed=0;
    for(std::size_t i=0;i<8u;++i){const auto& e=p.components_[i];
        if(e.reference00||e.nativeEffectControlId)return nullptr;
        if(e.kind==K::entity141300){if(e.sessionGeneration!=storage_->sessionGeneration)return nullptr;++destroyed;}
        else if(e.kind!=K::unbound||e.parent1C||e.previous10||e.next14||e.firstChild18)return nullptr;
        if(i&&p.componentSlots_[p.sceneCount_+i]!=&e)return nullptr;
    }
    const auto& model=p.modelOwner_;
    auto sceneRetired=[&](const auto& object){
        if(!object.constructed)return !object.entity().reference00;
        if(!object.released||object.entity().reference00||
           object.entity().sessionGeneration!=storage_->sessionGeneration)return false;
        std::size_t matches=0;for(std::size_t j=1;j<=p.sceneCount_;++j)
            if(p.componentSlots_[j]==&object.entity())++matches;
        if(matches!=1u)return false;++destroyed;return true;
    };
    if(!sceneRetired(model.model_)||(model.model_.constructed&&!model.released_))return nullptr;
    for(const auto& row:model.rows_)if(row&&!sceneRetired(*row))return nullptr;
    //Scene Entities retain their original kind:2896D0 marks released rather
    //than rewriting entity8.kind. Every nonnull slot must name its real owner.
    for(std::size_t j=1;j<=p.sceneCount_;++j){const auto* e=p.componentSlots_[j];if(!e)continue;
        bool owned=e==&model.model_.entity();
        for(const auto& row:model.rows_)if(row&&e==&row->entity())owned=true;
        if(!owned||e->reference00)return nullptr;
    }
    if(destroyed!=r.objectsDestroyed||
       (p.progress_.e0ConstructorAttempted&&(!p.progress_.e0PrefixRetired||!p.queries_.canWithdrawFreshE0())))return nullptr;
    if(p.effectArrays_.freshRoute_&&(!p.effectArrays_.retired_||p.effectArrays_.smoke160_||
       p.effectArrays_.fire164_||p.effectArrays_.anchors170_))return nullptr;
    if(p.effectArrays_.smokeWritten_||p.freshSmokeOwner_){
        const auto& a=p.effectArrays_;const auto& first=c.pieces_[0];
        const auto written=a.smokeWritten_+a.fireWritten_;
        if(!a.retired_||!freshSecondPairStopHistory(p)||!p.freshSmokeOwner_)return nullptr;
        auto retired=[&](const auto& o,std::size_t attempts,bool fire){
            const auto& e=o.component_;
            if(!o.retired_||e.reference00||e.nativeEffectControlId||
               e.kind!=(o.initialized_?K::entity141300:K::unbound)||e.next14||e.firstChild18)return false;
            if(!o.initialized_)return !e.parent1C&&!e.previous10;
            if(!attempts||!a.externalFirstSlot_||!a.externalStorage_)return false;
            const auto slot=*a.externalFirstSlot_+2u*(attempts-1u)+(fire?1u:0u);
            return slot<a.externalCapacity_&&a.externalStorage_[slot]==&e&&
                o.binding_.domain==a.externalDomain_&&o.binding_.domainStorage==a.externalStorage_&&
                o.binding_.domainCapacity==a.externalCapacity_&&o.binding_.particleParent==p.effectParent_&&
                e.sessionGeneration==storage_->sessionGeneration&&
                (!o.freshSetupAttempted_||o.context_==p.effectResource_);
        };
        if(written){
            const auto begin=4u+first.effectArrays_.smokeWritten_+first.effectArrays_.fireWritten_;
            if(!first.freshSplashOwner_||first.splash16C_!=&first.freshSplashOwner_->component_||
               !a.externalFirstSlot_||*a.externalFirstSlot_!=begin||!a.externalDomain_||!a.externalStorage_||
               a.externalDomain_->objects!=a.externalStorage_||begin>=a.externalCapacity_||written>a.externalCapacity_-begin||
               a.externalStorage_[begin-1u]!=first.splash16C_||!a.source160_||!*a.source160_||
               (a.fireWritten_&&(!a.source164_||!*a.source164_)))return nullptr;
            for(std::size_t n=0;n<written;++n){const auto* e=a.externalStorage_[begin+n];
                if(!e||e->reference00||e->kind!=K::entity141300||e->nativeEffectControlId||e->next14||e->firstChild18||
                   e->sessionGeneration!=storage_->sessionGeneration)return nullptr;
                const auto* previous=n?a.externalStorage_[begin+n-1u]:first.splash16C_;
                if(e->parent1C){if(e->parent1C!=p.effectParent_||e->previous10!=previous)return nullptr;}
                else{
                    if(e->previous10||n+1u!=written)return nullptr;
                    const bool actualUnlinked=(p.freshSmokeOwner_&&e==&p.freshSmokeOwner_->component_&&!p.freshSmokeOwner_->appended_)||
                        (p.freshFireOwner_&&e==&p.freshFireOwner_->component_&&!p.freshFireOwner_->appended_);
                    if(!actualUnlinked)return nullptr;
                }
            }
        }else if(a.externalFirstSlot_||a.externalDomain_||a.externalStorage_||a.externalCapacity_)return nullptr;
        if(!retired(*p.freshSmokeOwner_,p.freshSmokeAttempts_,false)||
           (p.freshFireOwner_&&!retired(*p.freshFireOwner_,p.freshFireAttempts_,true)))return nullptr;
        if(p.freshDustOwner_){const auto& o=*p.freshDustOwner_;const auto& e=o.component_;
            if(!p.freshDustAttempted_||!o.retired_||
               e.reference00||e.nativeEffectControlId||e.next14||e.firstChild18||
               e.kind!=(o.initialized_?K::entity141300:K::unbound)||
               (o.freshSetupAttempted_&&o.context_!=p.effectResource_))return nullptr;
            if(o.initialized_){
                if(!a.externalFirstSlot_||!a.externalStorage_||p.dust168_!=&e||
                   *a.externalFirstSlot_+written>=a.externalCapacity_||
                   a.externalStorage_[*a.externalFirstSlot_+written]!=&e||
                   o.binding_.domain!=a.externalDomain_||o.binding_.domainStorage!=a.externalStorage_||
                   o.binding_.domainCapacity!=a.externalCapacity_||o.binding_.particleParent!=p.effectParent_||
                   e.sessionGeneration!=storage_->sessionGeneration)return nullptr;
                const auto* historicalPrevious=a.externalStorage_[*a.externalFirstSlot_+written-1u];
                if(o.appended_?(!o.freshReaderComplete_||e.parent1C!=p.effectParent_||e.previous10!=historicalPrevious):
                    (e.parent1C||e.previous10))return nullptr;
            }else if(p.dust168_||o.appended_||e.parent1C||e.previous10)return nullptr;
        }else if(p.freshDustAttempted_||p.dust168_)return nullptr;
        if(p.freshSplashOwner_){const auto& o=*p.freshSplashOwner_;const auto& e=o.component_;
            if(!p.freshSplashAttempted_||!p.freshDustOwner_||!p.freshDustOwner_->appended_||
               !o.retired_||e.reference00||e.nativeEffectControlId||e.next14||e.firstChild18||
               e.kind!=(o.initialized_?K::entity141300:K::unbound)||
               (o.freshSetupAttempted_&&o.context_!=p.effectResource_))return nullptr;
            if(o.initialized_){
                if(!a.externalFirstSlot_||!a.externalStorage_||*a.externalFirstSlot_+written+1u>=a.externalCapacity_||
                   a.externalStorage_[*a.externalFirstSlot_+written+1u]!=&e||
                   o.binding_.domain!=a.externalDomain_||o.binding_.domainStorage!=a.externalStorage_||
                   o.binding_.domainCapacity!=a.externalCapacity_||o.binding_.particleParent!=p.effectParent_||
                   e.sessionGeneration!=storage_->sessionGeneration)return nullptr;
                if(o.freshConstructorComplete_){
                    if(p.splash16C_!=&e||o.freshNativePartialCancelled_||!o.freshSide250_||*o.freshSide250_)return nullptr;
                    if(o.appended_?(!o.freshReaderComplete_||e.parent1C!=p.effectParent_||e.previous10!=p.dust168_):
                        (e.parent1C||e.previous10))return nullptr;
                }else if(p.splash16C_||!o.freshNativePartialCancelled_||o.sourceBoundary()!=0x19dc20u||
                    o.freshActual433F50_||o.freshSide250_||o.freshSetupAttempted_||o.appended_||
                    o.freshReaderAttempted_||e.parent1C||e.previous10)return nullptr;
            }else if(p.splash16C_||o.appended_||e.parent1C||e.previous10)return nullptr;
        }else if(p.freshSplashAttempted_||p.splash16C_)return nullptr;
        return &p; //No source160/164 dereference after free; actual native slots remain owned.
    }
    //Exactly one initial child and its bounded successor. Typed
    //owners/native slots survive array free; source160/164 are compare-only.
    if((p.freshSmokeOwner_&&p.freshDustOwner_)||p.effectArrays_.fireWritten_>1u||
       (p.freshFireOwner_&&(!p.freshSmokeOwner_||p.freshSplashOwner_))||
       (p.freshSplashOwner_&&(!p.freshDustOwner_||p.freshFireOwner_)))return nullptr;
    auto retiredChild=[&](const auto& owner,const OriginalWreckComponent50LA* historicalPrevious){const auto& e=owner.component_;
        return owner.retired_&&!e.reference00&&!e.nativeEffectControlId&&
            e.kind==(owner.initialized_?K::entity141300:K::unbound)&&
            (!owner.initialized_||(e.sessionGeneration==storage_->sessionGeneration&&
                owner.binding_.particleParent==p.effectParent_))&&
            (!owner.freshSetupAttempted_||owner.context_==p.effectResource_)&&
            !e.next14&&!e.firstChild18&&
            (owner.appended_?(owner.initialized_&&owner.freshReaderComplete_&&historicalPrevious&&
                e.parent1C==p.effectParent_&&e.previous10==historicalPrevious):
                (!e.parent1C&&!e.previous10));
    };
    if(p.freshSmokeOwner_){const auto& o=*p.freshSmokeOwner_;const auto& a=p.effectArrays_;
        if(!p.freshSmokeAttempted_||p.freshSmokeAttempts_!=1u||!retiredChild(o,c.pieces_[0].freshSplashOwner_?&c.pieces_[0].freshSplashOwner_->component_:nullptr))return nullptr;
        if(o.initialized_){
            if(a.smokeWritten_!=1u||!a.source160_||!*a.source160_||!a.externalFirstSlot_||
               !a.externalStorage_||*a.externalFirstSlot_>=a.externalCapacity_||
               a.externalStorage_[*a.externalFirstSlot_]!=&o.component_||
               o.binding_.domain!=a.externalDomain_||o.binding_.domainStorage!=a.externalStorage_)return nullptr;
        }else if(a.smokeWritten_)return nullptr;
    }else if(p.effectArrays_.smokeWritten_)return nullptr;
    if(p.freshDustOwner_){const auto& o=*p.freshDustOwner_;
        if(!p.freshDustAttempted_||!retiredChild(o,c.pieces_[0].freshSplashOwner_?&c.pieces_[0].freshSplashOwner_->component_:nullptr)||
           (o.initialized_?p.dust168_!=&o.component_:p.dust168_!=nullptr))return nullptr;
    }else if(p.dust168_)return nullptr;
    const auto& first=c.pieces_[0];
    auto retiredSuccessorSlot=[&](const auto& owner,const auto& initial){
        const auto& b=owner.binding_;const auto& ib=initial.binding_;
        const auto slot=4u+first.effectArrays_.smokeWritten_+first.effectArrays_.fireWritten_;
        return b.domain==ib.domain&&b.domainStorage==ib.domainStorage&&b.domainCapacity==ib.domainCapacity&&
            b.particleParent==ib.particleParent&&b.domain&&b.domainStorage&&b.domain->objects==b.domainStorage&&
            first.freshSplashOwner_&&first.splash16C_==&first.freshSplashOwner_->component_&&
            slot<b.domainCapacity&&b.domainStorage[slot-1u]==first.splash16C_&&
            b.domainStorage[slot]==&initial.component_&&slot+1u<b.domainCapacity&&
            b.domainStorage[slot+1u]==&owner.component_;
    };
    if(p.freshFireOwner_){const auto& o=*p.freshFireOwner_;const auto& a=p.effectArrays_;
        if(!p.freshFireAttempted_||p.freshFireAttempts_!=1u||
           !retiredChild(o,&p.freshSmokeOwner_->component_))return nullptr;
        if(o.initialized_){
            if(a.fireWritten_!=1u||!a.source164_||!*a.source164_||
               !retiredSuccessorSlot(o,*p.freshSmokeOwner_))return nullptr;
        }else if(a.fireWritten_)return nullptr;
    }else if(p.freshFireAttempted_||p.freshFireAttempts_||p.effectArrays_.fireWritten_)return nullptr;
    if(p.freshSplashOwner_){const auto& o=*p.freshSplashOwner_;
        if(!p.freshSplashAttempted_||!retiredChild(o,&p.freshDustOwner_->component_))return nullptr;
        if(o.initialized_){
            if(!retiredSuccessorSlot(o,*p.freshDustOwner_))return nullptr;
            if(o.freshConstructorComplete_){if(p.splash16C_!=&o.component_||o.freshNativePartialCancelled_)return nullptr;}
            else if(o.appended_||p.splash16C_||!o.freshNativePartialCancelled_||o.sourceBoundary()!=0x19dc20u||
                o.freshActual433F50_||o.freshSide250_||o.freshSetupAttempted_)return nullptr;
        }else if(p.splash16C_)return nullptr;
    }else if(p.freshSplashAttempted_||p.splash16C_)return nullptr;
    return &p;
}
//Exact typed final Group child, never a generic extra-count grant. This
//read-only join authenticates the retained sourceC0 and actual forest tail.
bool freshGroupSparksSuffix(const OriginalWreckComponent50LA*& result)const noexcept {
    result=nullptr;if(!storage_||!storage_->count)return false;
    const auto& c=storage_->collections[0];
    if(!c.freshGroupSparks_)return !c.sourceGroupC0_;
    const auto& owner=*c.freshGroupSparks_;const auto& b=owner.binding_;
    if(!c.freshGroupSparksPublished_||!c.nativeGroupChildAttempted_[0]||!c.freshGroupContextChecked_||
       c.sourceGroupC0_!=&owner.component_||c.nativeGroupPC_!=(c.freshGroupSparksConfigAttempted_?0x15ae2cu:c.freshGroupSparksSetupAttempted_?c.freshGroupSparksSetupPC_:0x2cc0d4u)||
       !c.sourceCount30_||c.sourceCount30_>2u||c.completedPieceConstructors_!=c.sourceCount30_||
       b.particleParent!=c.freshGroupParent_||b.actualObjectCount423F60!=storage_->actualObjectCount423F60||
       b.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58)return false;
    if(owner.retired_)return owner.retiredFreshConstructorCurrent();
    if(!owner.preflightFreshUnappended())return false;
    const auto& last=c.pieces_[c.sourceCount30_-1u];
    const auto* splash=last.freshSplashOwner_;
    if(!splash||!splash->appended_||splash->retired_||!splash->freshReaderComplete_||
       last.splash16C_!=&splash->component_||!last.effectInputsStillBound()||
       b.particleParent!=last.effectParent_||splash->binding_.domain!=b.domain||
       splash->binding_.domainStorage!=b.domainStorage||splash->binding_.domainCapacity!=b.domainCapacity||
       b.domain->count<4u||b.domainStorage[b.domain->count-2u]!=last.splash16C_||
       splash->component_.next14||splash->component_.parent1C!=b.particleParent)return false;
    result=&owner.component_;return true;
}
template<class Owner> bool freshOwnerBeforeGroupSparks(const Owner& owner,
    const OriginalWreckComponent50LA* suffix)const noexcept {
    if(!suffix||!storage_||!storage_->count)return false;
    const auto* groupOwner=storage_->collections[0].freshGroupSparks_;
    const auto& b=owner.binding_;const auto& e=owner.component_;
    if(!groupOwner||suffix!=&groupOwner->component_||!owner.initialized_||!owner.appended_||owner.retired_||
       b.domain!=groupOwner->binding_.domain||b.domainStorage!=groupOwner->binding_.domainStorage||
       b.domainCapacity!=groupOwner->binding_.domainCapacity||b.particleParent!=groupOwner->binding_.particleParent||
       b.actualObjectCount423F60!=groupOwner->binding_.actualObjectCount423F60||
       b.actualAttachmentLock423F58!=groupOwner->binding_.actualAttachmentLock423F58||
       !b.domain||!b.domain->count||b.domainStorage[b.domain->count-1u]!=suffix)return false;
    //Search the actual domain, never shorten it or fake a source next link to an unappended child.
    for(std::size_t i=0;i+1u<b.domain->count;++i)if(b.domainStorage[i]==&e)
        return owner.preflightFreshChainMember(i,e.previous10,e.next14);
    return false;
}
//The sole later occurrence is authenticated from its REAL typed owner/source
//fields. No caller receipt, copied forest or persistent following-pointer list.
bool freshFollowingInitialChild(const OriginalWreckPieceStorage50LA& first,
    const OriginalWreckComponent50LA*& following,
    const OriginalWreckComponent50LA*& followingTail,
    const OriginalWreckEffectArrays50LA*& followingPairs)noexcept {
    following=nullptr;followingTail=nullptr;followingPairs=nullptr;
    if(!storage_||!storage_->count)return false;
    const auto& collection=storage_->collections[0];
    const OriginalWreckComponent50LA* groupSparks=nullptr;
    if(!freshGroupSparksSuffix(groupSparks))return false;
    const std::size_t groupExtra=groupSparks?1u:0u;
    if(&first!=&collection.pieces_[0]||collection.sourceSlots38_<2u)return true;
    if(collection.pieces_[1].nativePreBodyRetired_||collection.pieces_[1].progress_.pieceRetired)
        return exactRetiredSecondPiece()!=nullptr;
    auto* second=freshRetirementPiece(1u);if(!second)return false;
    if(!second->freshSmokeOwner_&&!second->freshDustOwner_)return true;
    if(!first.freshDustOwner_||!first.freshSplashOwner_||
       !first.effectInputsStillBound()||!second->effectInputsStillBound()||
       first.effectInputs_.resourceContextField!=second->effectInputs_.resourceContextField||
       first.effectInputs_.particleParentField!=second->effectInputs_.particleParentField)return false;
    if(second->freshSmokeOwner_||second->effectArrays_.smokeWritten_){
        const auto& a=second->effectArrays_;
        std::size_t live=0;
        const auto* splash=second->freshSplashOwner_&&second->freshSplashOwner_->initialized_?
            &second->freshSplashOwner_->component_:nullptr;
        if(second->freshSplashOwner_){const auto& o=*second->freshSplashOwner_;const auto& e=o.component_;
            if(!second->freshDustOwner_||!second->freshDustOwner_->appended_||
               (o.appended_&&(!o.freshConstructorComplete_||!o.freshReaderComplete_))||
               (!o.retired_&&!(groupSparks?freshOwnerBeforeGroupSparks(o,groupSparks):o.preflightFreshUnappended(true)))||
               (o.freshConstructorComplete_?second->splash16C_!=&e:second->splash16C_!=nullptr))return false;
            if(o.initialized_&&(o.binding_.domain!=a.externalDomain_||o.binding_.domainStorage!=a.externalStorage_||
               o.binding_.domainCapacity!=a.externalCapacity_||o.binding_.particleParent!=second->effectParent_||
               o.binding_.actualObjectCount423F60!=storage_->actualObjectCount423F60||
               o.binding_.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
               (o.freshSetupAttempted_&&o.context_!=second->effectResource_)))return false;
            if(o.retired_&&(e.reference00||e.nativeEffectControlId||
               e.kind!=(o.initialized_?OriginalWreckComponentKind50LA::entity141300:OriginalWreckComponentKind50LA::unbound)))return false;
        }
        if(!first.effectArrays_.preflightFollowingFreshPairs(a,first.dust168_,first.splash16C_,live,second->dust168_,splash,groupSparks))return false;
        if(second->freshDustOwner_){const auto& o=*second->freshDustOwner_;const auto& e=o.component_;
            if((o.appended_&&!o.freshReaderComplete_)||
               (!o.retired_&&!(groupSparks?freshOwnerBeforeGroupSparks(o,groupSparks):o.preflightFreshUnappended(true,splash&&splash->reference00==1u?splash:nullptr)))||
               (o.initialized_?second->dust168_!=&e:second->dust168_!=nullptr))return false;
            if(o.initialized_&&(o.binding_.domain!=a.externalDomain_||o.binding_.domainStorage!=a.externalStorage_||
               o.binding_.domainCapacity!=a.externalCapacity_||o.binding_.particleParent!=second->effectParent_||
               o.binding_.actualObjectCount423F60!=storage_->actualObjectCount423F60||
               o.binding_.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
               (o.freshSetupAttempted_&&o.context_!=second->effectResource_)))return false;
            if(o.retired_&&(e.reference00||e.nativeEffectControlId||
               e.kind!=(o.initialized_?OriginalWreckComponentKind50LA::entity141300:OriginalWreckComponentKind50LA::unbound)))return false;
        }
        if(second->freshSmokeOwner_&&!second->freshSmokeOwner_->retired_&&
           !freshEffectOwnerCurrent(*second,*second->freshSmokeOwner_,second->freshSmokeAttempts_,false))return false;
        if(second->freshFireOwner_&&!second->freshFireOwner_->retired_&&
           !freshEffectOwnerCurrent(*second,*second->freshFireOwner_,second->freshFireAttempts_,true))return false;
        if(live){following=a.smoke160_[0];followingPairs=&a;}
        return true;
    }
    auto tail=[&](const auto& owner)->bool {
        const auto& e=owner.component_;
        if(owner.retired_)return !e.reference00&&!e.nativeEffectControlId&&
            e.kind==(owner.initialized_?OriginalWreckComponentKind50LA::entity141300:OriginalWreckComponentKind50LA::unbound);
        if(!(groupSparks?freshOwnerBeforeGroupSparks(owner,groupSparks):owner.preflightFreshUnappended(true)))return false;
        if(owner.initialized_)followingTail=&e;
        return true;
    };
    if(second->freshFireOwner_){const auto& o=*second->freshFireOwner_;
        if(!tail(o))return false;
        if(o.initialized_&&(second->effectArrays_.fireWritten_!=1u||!second->effectArrays_.fire164_||
           second->effectArrays_.fire164_[0]!=&o.component_))return false;
    }
    if(second->freshSplashOwner_){const auto& o=*second->freshSplashOwner_;
        if(!tail(o)||(o.freshConstructorComplete_?second->splash16C_!=&o.component_:second->splash16C_!=nullptr))return false;
    }
    auto check=[&](const auto& owner,bool smoke)->bool {
        const auto& e=owner.component_;
        if(owner.retired_)return !followingTail&&!e.reference00&&!e.nativeEffectControlId&&
            e.kind==(owner.initialized_?OriginalWreckComponentKind50LA::entity141300:OriginalWreckComponentKind50LA::unbound);
        if(!(groupSparks?freshOwnerBeforeGroupSparks(owner,groupSparks):owner.preflightFreshUnappended(true,followingTail)))return false;
        if(!owner.initialized_)return !followingTail&&(smoke?!second->effectArrays_.smokeWritten_:!second->dust168_);
        const auto& b=owner.binding_;const auto& prior=first.freshSplashOwner_->binding_;
        const std::size_t extra=followingTail?1u:0u;
        if(!first.freshSplashOwner_->appended_||!first.freshDustOwner_->appended_||
           first.splash16C_!=&first.freshSplashOwner_->component_||first.dust168_!=&first.freshDustOwner_->component_||
           b.domain!=prior.domain||b.domainStorage!=prior.domainStorage||b.domainCapacity!=prior.domainCapacity||
           b.particleParent!=first.effectParent_||b.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           b.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
           !b.domain||b.domain->count<3u+extra+groupExtra||b.domainStorage[b.domain->count-2u-extra-groupExtra]!=first.splash16C_||
           b.domainStorage[b.domain->count-1u-extra-groupExtra]!=&e||
           (followingTail&&b.domainStorage[b.domain->count-1u-groupExtra]!=followingTail)||
           (owner.freshSetupAttempted_&&owner.context_!=second->effectResource_))return false;
        if(smoke){const auto& a=second->effectArrays_;
            if(a.smokeWritten_!=1u||!a.smoke160_||a.smoke160_[0]!=&e||
               !a.externalFirstSlot_||*a.externalFirstSlot_+1u+extra+groupExtra!=b.domain->count||
               a.externalDomain_!=b.domain||a.externalStorage_!=b.domainStorage||a.externalCapacity_!=b.domainCapacity)return false;
        }else if(second->dust168_!=&e||second->effectArrays_.attempted_)return false;
        if(followingTail){
            if(!owner.appended_)return false;
            auto sameBinding=[&](const auto& tb){return tb.domain==b.domain&&tb.domainStorage==b.domainStorage&&
                tb.domainCapacity==b.domainCapacity&&tb.particleParent==b.particleParent&&
                tb.actualObjectCount423F60==b.actualObjectCount423F60&&tb.actualAttachmentLock423F58==b.actualAttachmentLock423F58;};
            if(smoke?!sameBinding(second->freshFireOwner_->binding_):!sameBinding(second->freshSplashOwner_->binding_))return false;
        }
        const auto* linkedTail=followingTail&&followingTail->parent1C?followingTail:nullptr;
        if(linkedTail&&(linkedTail->parent1C!=b.particleParent||linkedTail->previous10!=&e||linkedTail->next14))return false;
        if(owner.appended_){
            if(!owner.freshReaderComplete_||e.parent1C!=first.effectParent_||
               e.previous10!=first.splash16C_||first.splash16C_->next14!=&e||e.next14!=linkedTail)return false;
        }else if(e.parent1C||e.previous10||e.next14||first.splash16C_->next14)return false;
        following=&e;return true;
    };
    return second->freshSmokeOwner_?check(*second->freshSmokeOwner_,true):check(*second->freshDustOwner_,false);
}
OriginalWreckPrefixRetirementResult50LA retirePieceBeforeE0Core(
    std::size_t collectionIndex,std::size_t pieceIndex,const LiveZoneWorld50EN* world,
    bool fresh,bool preflightOnly)noexcept {
    using S=OriginalWreckPrefixRetirementStatus50LA;
    using K=OriginalWreckComponentKind50LA;
    OriginalWreckPrefixRetirementResult50LA rejected;
    auto* selected=fresh?(collectionIndex==0u?freshRetirementPiece(pieceIndex):nullptr):retirementOccurrence(collectionIndex,pieceIndex);
    if(!selected)return rejected;
    auto& p=*selected;auto& progress=p.progress_;
    if(p.nativePreBodyRetired_)return progress.retirementResult;
    auto& collection=storage_->collections[collectionIndex];
    const OriginalWreckComponent50LA* groupSparks=nullptr;
    if(fresh&&(!freshGroupSparksSuffix(groupSparks)||(groupSparks&&!preflightOnly)))return rejected;
    const OriginalWreckComponent50LA* followingInitialChild=nullptr;
    const OriginalWreckComponent50LA* followingInitialTail=nullptr;
    const OriginalWreckEffectArrays50LA* followingPairs=nullptr;
    if(fresh&&!freshFollowingInitialChild(p,followingInitialChild,followingInitialTail,followingPairs))return rejected;
    const OriginalWreckEffectArrays50LA* priorEffectArrays=nullptr;
    const OriginalWreckComponent50LA* priorDust=nullptr;const OriginalWreckComponent50LA* priorSplash=nullptr;
    if(fresh&&pieceIndex==1u&&p.effectArrays_.smokeWritten_){
        const auto& first=collection.pieces_[0];
        if(!first.freshDustOwner_||!first.freshSplashOwner_||
           first.dust168_!=&first.freshDustOwner_->component_||first.splash16C_!=&first.freshSplashOwner_->component_||
           first.freshDustOwner_->binding_.domain!=p.effectArrays_.externalDomain_||
           first.freshSplashOwner_->binding_.domain!=p.effectArrays_.externalDomain_||
           first.freshDustOwner_->binding_.domainStorage!=p.effectArrays_.externalStorage_||
           first.freshSplashOwner_->binding_.domainStorage!=p.effectArrays_.externalStorage_||
           first.freshDustOwner_->binding_.domainCapacity!=p.effectArrays_.externalCapacity_||
           first.freshSplashOwner_->binding_.domainCapacity!=p.effectArrays_.externalCapacity_||
           first.freshDustOwner_->binding_.particleParent!=p.effectParent_||
           first.freshSplashOwner_->binding_.particleParent!=p.effectParent_)return rejected;
        priorEffectArrays=&first.effectArrays_;priorDust=first.dust168_;priorSplash=first.splash16C_;
        if(p.effectArrays_.externalFirstSlot_&&p.effectArrays_.externalDomain_&&
           p.effectArrays_.externalDomain_->count<*p.effectArrays_.externalFirstSlot_&&
           (!first.freshDustOwner_->retired_||!first.freshSplashOwner_->retired_||
            (first.freshSmokeOwner_&&!first.freshSmokeOwner_->retired_)||
            (first.freshFireOwner_&&!first.freshFireOwner_->retired_)))return rejected;
    }
    if(!collection.nativeSourceMode_||collection.retirementAttempted_||progress.pieceRetired||
       progress.retirementAttempted||progress.modelRetirementAttempted||
       !storage_->actualObjectCount423F60||!*storage_->actualObjectCount423F60||
       !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
       !p.assets_||!p.sceneCount_||p.componentCount_<=p.sceneCount_+1u||
       (p.effectArrays_.attempted()&&!(fresh&&p.effectArrays_.freshRoute_))||(p.effectInputsCommitted_&&!(fresh&&p.freshEffectInputsAttempted_))||(p.dust168_&&!(fresh&&p.freshDustAttempted_&&p.freshDustOwner_))||(p.splash16C_&&!(fresh&&p.freshSplashAttempted_&&p.freshSplashOwner_))||
       p.queries_.constructed()||p.queries_.modelSceneIdentity().value||
       (fresh?((p.queries_.prepared()&&!p.queries_.freshE0Backing())||(p.setupZonesBound_&&!p.freshBody_.attempted)):
           (!world||!p.queries_.canReleaseUnpublishedWorld(*world))))return rejected;
    //Native counted suffix custody is distinct from source16C publication.
    //A constructor refusal can retain a counted owner while source16C is NULL.
    const auto* freshSplashAlias=fresh&&p.freshSplashOwner_&&p.freshSplashOwner_->initialized_?
        &p.freshSplashOwner_->component_:nullptr;
    if(fresh&&p.freshSplashOwner_){
        const auto& owner=*p.freshSplashOwner_;const auto& c=owner.component_;
        if(!p.freshSplashAttempted_||!p.freshDustOwner_||
           (!owner.retired_&&!(groupSparks?freshOwnerBeforeGroupSparks(owner,groupSparks):followingInitialChild?
               (owner.binding_.domain&&owner.binding_.domain->count>=2u+(followingInitialTail?1u:0u)&&
                owner.preflightFreshChainMember(followingPairs?*followingPairs->externalFirstSlot_-1u:
                    owner.binding_.domain->count-2u-(followingInitialTail?1u:0u),p.dust168_,
                    followingInitialChild->parent1C?followingInitialChild:nullptr)):
               owner.preflightFreshUnappended(true))))return rejected;
        if(owner.initialized_){
            if(freshSplashAlias!=&c||
               (owner.freshConstructorComplete_?p.splash16C_!=&c:p.splash16C_!=nullptr)||
               owner.binding_.particleParent!=p.effectParent_||
               (owner.freshSetupAttempted_&&owner.context_!=p.effectResource_))return rejected;
            if(owner.binding_.domain!=p.freshDustOwner_->binding_.domain||
               owner.binding_.domainStorage!=p.freshDustOwner_->binding_.domainStorage)return rejected;
        }else if(freshSplashAlias||p.splash16C_)return rejected;
        if(owner.retired_&&(c.reference00||c.kind!=(owner.initialized_?K::entity141300:K::unbound)))return rejected;
    }else if(freshSplashAlias||p.splash16C_)return rejected;
    const auto* freshLiveSplash=freshSplashAlias&&freshSplashAlias->reference00==1u?freshSplashAlias:nullptr;
    const auto* freshDustAlias=fresh?p.dust168_:nullptr;
    if(fresh&&p.freshDustOwner_){
        const auto& owner=*p.freshDustOwner_;const auto& c=owner.component_;
        if(!p.freshDustAttempted_||!p.freshEffectInputsAttempted_||
           (!owner.retired_&&!(groupSparks?freshOwnerBeforeGroupSparks(owner,groupSparks):followingInitialChild?
               (owner.binding_.domain&&owner.binding_.domain->count>=3u+(followingInitialTail?1u:0u)&&
                owner.preflightFreshChainMember(followingPairs?*followingPairs->externalFirstSlot_-2u:
                    owner.binding_.domain->count-3u-(followingInitialTail?1u:0u),
                    p.effectArrays_.fireWritten_?p.effectArrays_.fire164_[p.effectArrays_.fireWritten_-1u]:nullptr,p.splash16C_)):
               owner.preflightFreshUnappended(true,freshLiveSplash))))return rejected;
        if(owner.initialized_){
            if(freshDustAlias!=&c||owner.binding_.particleParent!=p.effectParent_||
               (owner.freshSetupAttempted_&&owner.context_!=p.effectResource_))return rejected;
            if(p.effectArrays_.attempted_&&(owner.binding_.domain!=p.effectArrays_.externalDomain_||
               owner.binding_.domainStorage!=p.effectArrays_.externalStorage_))return rejected;
        }else if(freshDustAlias)return rejected;
        if(owner.retired_&&(c.reference00||c.kind!=(owner.initialized_?K::entity141300:K::unbound)))return rejected;
    }else if(freshDustAlias)return rejected;
    const auto* freshFireAlias=fresh&&p.freshFireOwner_&&p.freshFireAttempts_==p.effectArrays_.fireWritten_?&p.freshFireOwner_->component_:nullptr;
    if(fresh&&p.freshFireOwner_){
        const auto& owner=*p.freshFireOwner_;const auto& entity=owner.component_;
        if(!p.freshFireAttempted_||!p.freshSmokeOwner_||!p.freshEffectInputsAttempted_||
           (!owner.retired_&&!freshEffectOwnerCurrent(p,owner,p.freshFireAttempts_,true)))return rejected;
        if(owner.retired_&&(entity.reference00||entity.kind!=(owner.initialized_?K::entity141300:K::unbound)))return rejected;
        if(owner.initialized_&&(owner.binding_.particleParent!=p.effectParent_||
           owner.binding_.domain!=p.effectArrays_.externalDomain_||
           owner.binding_.domainStorage!=p.effectArrays_.externalStorage_))return rejected;
    }
    const auto* freshSmokeAlias=fresh&&p.freshSmokeOwner_&&p.freshSmokeAttempts_==p.effectArrays_.smokeWritten_?&p.freshSmokeOwner_->component_:nullptr;
    if(fresh&&p.freshSmokeOwner_){
        if(!p.freshSmokeAttempted_||!p.freshEffectInputsAttempted_||
           (!p.freshSmokeOwner_->retired_&&!freshEffectOwnerCurrent(p,*p.freshSmokeOwner_,p.freshSmokeAttempts_,false)))return rejected;
        const auto& smokeEntity=p.freshSmokeOwner_->component_;
        if(p.freshSmokeOwner_->retired_&&(smokeEntity.reference00||
           smokeEntity.kind!=(p.freshSmokeOwner_->initialized_?K::entity141300:K::unbound)))return rejected;
        if(freshSmokeAlias&&(!p.freshSmokeOwner_->initialized_||
           p.freshSmokeOwner_->binding_.particleParent!=p.effectParent_||
           p.freshSmokeOwner_->binding_.domain!=p.effectArrays_.externalDomain_||
           p.freshSmokeOwner_->binding_.domainStorage!=p.effectArrays_.externalStorage_))return rejected;
    }
    if(fresh&&p.effectArrays_.freshRoute_&&!p.effectArrays_.preflightFreshArrays(freshSmokeAlias,freshFireAlias,freshDustAlias,freshSplashAlias,followingPairs?nullptr:followingInitialChild,priorEffectArrays,priorDust,priorSplash,followingInitialTail,followingPairs,followingPairs?collection.pieces_[1].dust168_:nullptr,
        followingPairs&&collection.pieces_[1].freshSplashOwner_&&collection.pieces_[1].freshSplashOwner_->initialized_?
            &collection.pieces_[1].freshSplashOwner_->component_:nullptr,groupSparks))return rejected;
    if(fresh&&p.effectArrays_.freshRoute_){
        const auto& a=p.effectArrays_;
        const auto written=a.smokeWritten_+a.fireWritten_;
        for(std::size_t n=0;n<written;++n){
            const auto* c=(n&1u)?a.fire164_[n/2u]:a.smoke160_[n/2u];
            if(!c->reference00)continue; //Retired source links/history are not current forest membership.
            if(c->parent1C){if(c->parent1C!=p.effectParent_)return rejected;}
            else if(n+1u!=written||c->previous10||c->next14)return rejected;
            if(n&&c->parent1C){
                const auto* previous=(n&1u)?a.smoke160_[n/2u]:a.fire164_[n/2u-1u];
                if(c->previous10!=previous||previous->next14!=c)return rejected;
            }
        }
    }
    if(fresh&&p.effectArrays_.attempted()&&(!p.freshEffectCountWritten_||!p.freshEffectInputsAttempted_||
       p.effectArrays_.count_!=p.effects_.count174||p.effectArrays_.freshModel_!=&p.modelOwner_||
       p.effectArrays_.freshGraph_!=p.assets_->graph().nodes))return rejected;
    if(fresh&&p.freshEffectInputsAttempted_&&(!p.freshEffectCountWritten_||!p.queries_.storage_||
       !p.queries_.storage_->freshDerivedBound||!p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_))return rejected;
    if(fresh&&p.modelOwner_.model_.constructed&&p.sourceModel14C_&&
       *p.sourceModel14C_&&*p.sourceModel14C_!=&p.modelOwner_.model_)return rejected;
    auto& root=*p.componentSlots_[0];auto& container=collection.collectionComponent_;
    if(root.reference00!=1u||root.kind!=K::piece2CB4B0||
       (root.next14&&!(fresh&&preflightOnly&&collectionIndex==0u&&pieceIndex==0u&&
           freshLiveSecondSibling(p)==root.next14))||
       container.reference00!=1u||container.kind!=K::collection22BF00)return rejected;
    const auto body=1u+p.sceneCount_;
    auto& d0=*p.componentSlots_[body];auto& d4=*p.componentSlots_[body+1u];
    const bool bodyCut=p.setupZonesBound_;
    const bool freshBody=fresh&&p.freshBody_.attempted;
    const bool d0Live=d0.reference00!=0u,d4Live=d4.reference00!=0u;
    const bool d0Linked=d0.parent1C==&root,d4Linked=d4.parent1C==&root;
    const bool freshE0=fresh&&p.queries_.freshE0Backing();
    auto& e0=*p.componentSlots_[body+2u];
    const bool e0Linked=freshE0&&e0.parent1C==&root;
    auto& dc=*p.componentSlots_[body+3u];auto& contact=p.freshContactDC_;
    const bool freshDC=fresh&&contact.attempted;
    const bool dcLive=freshDC&&dc.reference00!=0u;
    const bool dcLinked=dcLive&&dc.parent1C==&root;
    auto& d8=*p.componentSlots_[body+4u];auto& detector=p.freshDetectorD8_;
    const bool freshD8=fresh&&detector.attempted;
    const bool d8Live=freshD8&&d8.reference00!=0u;
    const bool d8Linked=d8Live&&d8.parent1C==&root;
    auto& e4=*p.componentSlots_[body+5u];
    const bool freshE4=fresh&&p.queries_.storage_&&p.queries_.storage_->freshE4.started;
    const bool e4Live=freshE4&&e4.reference00!=0u;
    const bool e4Linked=e4Live&&p.queries_.storage_->freshE4.appended;
    auto& e8=*p.componentSlots_[body+6u];
    const bool freshE8=fresh&&p.queries_.storage_&&p.queries_.storage_->freshE8.started;
    const bool e8Live=freshE8&&e8.reference00!=0u;
    const bool e8Linked=e8Live&&p.queries_.storage_->freshE8.appended;
    if(e8Linked&&(!e4Linked||e8.parent1C!=&root||e8.previous10!=&e4||e4.next14!=&e8))return rejected;
    if(freshE8&&(!progress.e8ConstructorAttempted||!p.queries_.canRetireFreshE8(e8)))return rejected;
    if(e4Linked&&(!d8Linked||e4.parent1C!=&root||e4.previous10!=&d8||d8.next14!=&e4||
       e4.next14!=(e8Linked?&e8:nullptr)))return rejected;
    if(freshE4&&(!p.freshE4_.attempted||!progress.e4ConstructorAttempted||
       !p.queries_.canRetireFreshE4(e4)))return rejected;
    if(freshD8){
        if(!freshDC||!dcLinked||detector.retired||!detector.origin443550||!detector.local433B10||
           !detector.terrain330Field||detector.constructed!=d8Live||detector.appended!=d8Linked||
           p.detector_.constructed!=detector.constructed)return rejected;
        if(d8Live){
            const auto& d=p.detector_;
            if(d8.reference00!=1u||d8.kind!=K::detector295290||d8.next14!=(e4Linked?&e4:nullptr)||d8.firstChild18||
               !progress.d8Reached||!p.sourceD8_||*p.sourceD8_!=&d8||
               d.contact30!=&p.physical_||d.terrain34!=*detector.terrain330Field||
               d.pairs60||d.count70||d.objects74||d.enabled64!=1u||d.queryEnabled68!=1u||d.reset6C!=1u||
               bool(d.query38)!=detector.bound||(!d.terrain34&&p.physical_.generated.pointContacts48))return rejected;
            if(d8Linked){if(d8.previous10!=&dc||dc.next14!=&d8)return rejected;}
            else if(d8.parent1C||d8.previous10)return rejected;
        }else if(d8.kind!=K::unbound||d8.parent1C||d8.previous10||d8.next14||d8.firstChild18||p.sourceD8_)return rejected;
    }
    if(freshDC){
        if(!freshE0||!e0Linked||contact.retired||!contact.origin4435A0||
           !progress.contactSuffixAttempted||contact.constructed!=dcLive||contact.appended!=dcLinked||
           p.physical_.contactConstructorFieldsApplied!=contact.constructed)return rejected;
        if(dcLive){
            const auto& physical=p.physical_;
            if(dc.reference00!=1u||dc.kind!=K::contact295A40||dc.firstChild18||dc.next14!=(d8Linked?&d8:nullptr)||
               !p.sourceDC_||*p.sourceDC_!=&dc||!progress.dcReached||
               !physical.pointRecords.explicitSource_||!physical.records.explicitSource_||
               physical.pointRecords.retired_||physical.records.retired_||
               contact.source80!=physical.pointRecords.data()||contact.source8C!=physical.records.data()||
               physical.spring.pointCount7C>50u||physical.spring.generatedCount84||
               physical.spring.generatedCapacity88!=50||
               (physical.pointRecords.data()&&physical.pointRecords.size()!=50u)||
               (physical.records.data()&&physical.records.size()!=50u)||
               (physical.records.attempted_&&!physical.pointRecords.data())||
               (physical.contactBody30&&physical.contactBody30!=&physical.body)||
               (physical.contactBody30&&(!physical.pointRecords.data()||!physical.records.data()))||
               (physical.spring.pointCount7C&&!physical.contactBody30))return rejected;
            if(dcLinked){if(dc.previous10!=&e0||e0.next14!=&dc)return rejected;}
            else if(dc.parent1C||dc.previous10)return rejected;
            for(std::size_t i=0;i<physical.spring.pointCount7C;++i){
                if(contact.pointScenes[i]!=&p.modelOwner_.model_||contact.pointFlagsKnownMask[i]!=1u||
                   physical.pointRecords[i].scene40.value)return rejected;
            }
        }else if(dc.kind!=K::unbound||dc.parent1C||dc.previous10||dc.next14||dc.firstChild18||
                 p.sourceDC_||p.physical_.pointRecords.explicitSource_||p.physical_.records.explicitSource_)return rejected;
    }
    if(freshBody){
        if(!bodyCut||!progress.pieceSetupAttempted||(progress.e0ConstructorAttempted&&!p.queries_.freshE0Backing())||
           !p.sourceVisibilityNodeB4_||!p.sourceCollisionNodeB8_||
           p.sourceVisibilityNodeB4_==p.sourceCollisionNodeB8_||
           !p.visibilityWorldB4_||!p.collisionWorldB8_||!p.collisionLifecycleB8_||
           &p.visibilityWorldB4_->lifecycle==p.collisionLifecycleB8_||
           &p.visibilityWorldB4_->portals==p.collisionWorldB8_||
           (p.physical_.contactConstructorFieldsApplied&&!freshDC)||p.physical_.linkedCount268||
           p.physical_.modelSceneDomain)return rejected;
        const auto* model=p.componentSlots_[1];
        if(!model||root.firstChild18!=model||model->parent1C!=&root||model->previous10||
           model->next14!=(d0Linked?&d0:nullptr))return rejected;
        auto absent=[](const OriginalWreckComponent50LA& e){
            return !e.reference00&&e.kind==K::unbound&&!e.parent1C&&!e.previous10&&!e.next14&&!e.firstChild18;
        };
        if(d0Live){
            if(d0.reference00!=1u||d0.kind!=K::body167920||d0.firstChild18||
               !p.physical_.bodyConstructorFieldsApplied||!p.sourceBodyD0_||*p.sourceBodyD0_!=&d0||
               !p.freshBody_.allocation28C||*p.freshBody_.allocation28C||!p.freshBody_.model288)return rejected;
            const bool bound=*p.freshBody_.model288!=nullptr;
            if(bound){
                if(!p.sourceModel14C_||*p.freshBody_.model288!=*p.sourceModel14C_||
                   *p.sourceModel14C_!=&p.modelOwner_.model_||p.physical_.body320!=1u||
                   p.physical_.body.enabled||!p.physical_.body.scenePresent)return rejected;
            }else if(p.physical_.body320||!p.physical_.body.enabled||p.physical_.body.scenePresent||d0Linked)return rejected;
            if(d0Linked){if(d0.previous10!=model||d0.next14!=(d4Linked?&d4:nullptr))return rejected;}
            else if(d0.parent1C||d0.previous10||d0.next14)return rejected;
        }else if(!absent(d0)||p.sourceBodyD0_||p.physical_.bodyConstructorFieldsApplied||d4Live)return rejected;
        if(d4Live){
            if(!d0Linked||d4.reference00!=1u||d4.kind!=K::gravity282670||d4.firstChild18||
               d4.next14!=(e0Linked?&e0:nullptr)||
               !p.sourceGravityD4_||*p.sourceGravityD4_!=&d4||!p.freshBody_.gravity50)return rejected;
            if(p.gravity_.target50){
                if(p.gravity_.target50!=&p.physical_||p.gravity_.count30!=1u||p.gravity_.capacity34!=1u||
                   *p.freshBody_.gravity50!=&p.gravity_.target50)return rejected;
            }else if(p.gravity_.count30||p.gravity_.capacity34||*p.freshBody_.gravity50||d4Linked)return rejected;
            if(d4Linked){if(d4.previous10!=&d0)return rejected;}
            else if(d4.parent1C||d4.previous10)return rejected;
        }else if(!absent(d4)||p.sourceGravityD4_||p.freshBody_.gravity50||p.gravity_.target50)return rejected;
    }
    // Only the genuine successful165E90 cut at166018 is added here. Mid-D0
    // or mid-gravity constructor failures remain retained, not inferred done.
    if(bodyCut&&!freshBody){
        const auto graph=p.assets_->graph();
        const auto* model=p.componentSlots_[1];
        if(!progress.pieceSetupAttempted||progress.e0ConstructorAttempted||
           progress.prefixResult.status!=OriginalWreckConstructorPrefixStatus50LA::publishedLifetimeRequired||
           progress.prefixResult.sourcePC!=0x166018u||
           !p.sourceBodyD0_||*p.sourceBodyD0_!=&d0||!p.sourceGravityD4_||*p.sourceGravityD4_!=&d4||
           !p.sourceVisibilityNodeB4_||!p.sourceCollisionNodeB8_||
           p.sourceVisibilityNodeB4_==p.sourceCollisionNodeB8_||
           p.collisionWorldB8_!=world||!p.visibilityWorldB4_||!p.collisionLifecycleB8_||
           p.collisionLifecycleB8_!=p.queries_.worldBinding().lifecycle||
           !model||root.firstChild18!=model||model->parent1C!=&root||model->previous10||
           model->next14!=&d0||d0.parent1C!=&root||d0.previous10!=model||d0.next14!=&d4||
           d4.parent1C!=&root||d4.previous10!=&d0||d4.next14||d0.firstChild18||d4.firstChild18||
           d0.reference00!=1u||d4.reference00!=1u||d0.kind!=K::body167920||d4.kind!=K::gravity282670||
           !p.physical_.bodyConstructorFieldsApplied||p.physical_.contactConstructorFieldsApplied||
           p.physical_.linkedCount268||p.physical_.modelSceneDomain||p.physical_.body320!=1u||
           p.physical_.body.enabled||!p.physical_.body.scenePresent||
           p.physical_.model288!=p.assets_->model14C()||p.physical_.modelGraphNodes!=graph.nodes||
           p.physical_.modelGraphCount!=graph.count||p.gravity_.target50!=&p.physical_||
           p.gravity_.count30!=1u||p.gravity_.capacity34!=1u)return rejected;
    }
    if(e0Linked&&(!d4Linked||e0.previous10!=&d4||e0.next14!=(dcLinked?&dc:nullptr)||!progress.e0FreshAppended))return rejected;
    if(freshE0&&(!progress.e0ConstructorAttempted||
       !p.queries_.canRetireFreshE0(e0,p.sourceE0_,e0Linked?&root:nullptr,e0Linked?&d4:nullptr,dcLinked?&dc:nullptr,d8Linked?&d8:nullptr,e4Linked?&e4:nullptr,e8Linked?&e8:nullptr)))return rejected;
    // Only selected fresh E0/DC/D8/E4/E8 may own the reached counted tail here.
    for(std::size_t i=body+(bodyCut?2u:0u)+(freshE0?1u:0u)+(dcLive?1u:0u)+(d8Live?1u:0u)+(e4Live?1u:0u)+(e8Live?1u:0u);i<p.componentCount_;++i){
        const auto* child=p.componentSlots_[i];
        if(!child||child->reference00||child->kind!=K::unbound||child->parent1C||
           child->previous10||child->next14||child->firstChild18)return rejected;
    }
    // Build a current outside domain without rewriting zero-ref historical
    // model/PSG kinds. Retired rows are deliberately not live Entity members.
    OriginalWreckDomainRows50LA objects;if(!liveRows(objects))return rejected;
    OriginalWreckComponentDomain50LA outside{storage_->sessionGeneration,objects.data(),0};
    auto add=[&](OriginalWreckComponent50LA* c){
        if(!c||!c->reference00)return true;
        if(outside.count==objects.size())return false;
        objects[outside.count++]=c;return true;
    };
    for(std::size_t ci=0;ci<storage_->count;++ci){
        auto& c=storage_->collections[ci];
        if(!add(&c.collectionComponent_)||!add(&c.groupComponent_))return rejected;
        for(std::size_t pi=0;pi<c.count_;++pi){auto& member=c.pieces_[pi];
            if(!member.componentSlots_)return rejected;
            for(std::size_t i=0;i<member.componentCount_;++i){
                if(&member==&p&&i>=1u&&i<=p.sceneCount_)continue;
                if(!add(member.componentSlots_[i]))return rejected;
            }
        }
    }
    using namespace original_wreck_component_detail50LA;
    if(!domain(outside))return rejected;
    // The source tail may be unattached on a failed setup; do not synthesize
    // 22C1B8. A linked tail must have its genuine reciprocal collection links.
    if(root.parent1C){
        if(root.parent1C!=&container||
           (root.previous10?(!contains(outside,root.previous10)||
               root.previous10->next14!=&root||root.previous10->parent1C!=&container):
               container.firstChild18!=&root))return rejected;
    }else if(root.previous10)return rejected;
    auto& owner=p.modelOwner_;const auto& fields=owner.fields();
    const bool model=owner.model_.constructed;
    const bool packets=p.modelPackets_.constructed240740();
    const bool geometry=p.modelGeometry_&&p.modelGeometry_->constructed240110();
    if(owner.released_||owner.referenceReleased_||p.modelPackets_.released()||
       (p.modelMaterials_.started()&&p.modelMaterials_.released26E4C0()))return rejected;
    if(packets){
        if(fields.packets1F0!=&p.modelPackets_||!p.modelPackets_.preflight240450().okay())return rejected;
    }else if(fields.packets1F0)return rejected;
    if(!p.modelMaterials_.canRelease26E4C0())return rejected;
    if(p.modelMaterials_.started()){
        if(!p.modelMaterials_.count1C0()||fields.materialCount1C0!=
           static_cast<std::int32_t>(*p.modelMaterials_.count1C0())||
           fields.materials1C4!=p.modelMaterials_.allocationBase1C4())return rejected;
    }else if(fields.materialCount1C0||fields.materials1C4)return rejected;
    if(geometry){
        if(fields.geometry1B4!=p.modelGeometry_.get()||
           !p.modelGeometry_->canReleaseForModel(&owner))return rejected;
    }else if(fields.geometry1B4||(p.modelGeometry_&&p.modelGeometry_->holdsModelReference(&owner)))return rejected;
    auto graph=p.assets_->graph();
    if(model){
        const auto& entity=owner.uiScopeModelEntity8();
        OriginalWreckDomainRows50LA joinedObjects;if(!liveRows(joinedObjects))return rejected;
        OriginalWreckComponentDomain50LA joined{};
        if(!owner.joinDomain(outside,joinedObjects.data(),joinedObjects.size(),joined)||
           entity.reference00!=1u||entity.kind!=K::model2403A0||entity.firstChild18||
           p.componentSlots_[1]!=&entity||entity.previous10||((freshBody?d0Linked:bodyCut)?entity.next14!=&d0:entity.next14!=nullptr)||
           (root.firstChild18?root.firstChild18!=&entity||entity.parent1C!=&root:entity.parent1C!=nullptr)||
           !owner.preflightNativePartialScene2896D0(graph).okay()||
           (!bodyCut&&!owner.preflightModelReference142590(outside).okay()))return rejected;
    }else{
        const auto& entity=owner.uiScopeModelEntity8();
        if(entity.reference00||entity.kind!=K::unbound||entity.parent1C||entity.previous10||
           entity.next14||entity.firstChild18||root.firstChild18||packets||geometry||
           p.modelMaterials_.started()||fields.sourceRows1B8)return rejected;
        for(std::size_t i=1;i<=p.sceneCount_;++i){const auto* row=p.componentSlots_[i];
            if(row&&(row->reference00||row->kind!=K::unbound))return rejected;
        }
    }
    std::size_t retiring=1u+(model?1u:0u)+(e8Live?1u:0u)+(e4Live?1u:0u)+(d8Live?1u:0u)+(dcLive?1u:0u)+(freshE0&&e0.reference00?1u:0u)+(freshBody?(d0Live?1u:0u)+(d4Live?1u:0u):(bodyCut?2u:0u));
    for(const auto& row:owner.rows_)if(row&&row->constructed&&!row->released)++retiring;
    const auto before=*storage_->actualObjectCount423F60;
    if(before<retiring)return rejected;
    if(preflightOnly)return {S::pieceRetired,0x141418u,retiring,retiring};
    // Every fallible admission above precedes the first mutation. All calls
    // below are synchronous, preflighted owner operations, not retry tokens.
    if(fresh&&p.freshSplashOwner_&&!p.freshSplashOwner_->retired_)std::terminate();
    if(fresh&&p.freshDustOwner_&&!p.freshDustOwner_->retired_)std::terminate();
    if(fresh&&p.freshFireOwner_&&!p.freshFireOwner_->retired_)std::terminate();
    if(fresh&&p.freshSmokeOwner_&&!p.freshSmokeOwner_->retired_)std::terminate(); //External prefix must retire first.
    if(fresh&&p.effectArrays_.freshRoute_&&!p.effectArrays_.releaseFresh2CB500(freshSmokeAlias,freshFireAlias,freshDustAlias,freshSplashAlias,priorEffectArrays,priorDust,priorSplash))std::terminate();
    if(freshE8)p.queries_.retireFreshE8(e8,*storage_->actualObjectCount423F60);
    if(freshE4)p.queries_.retireFreshE4(e4,*storage_->actualObjectCount423F60);
    if(freshD8){
        if(d8Live){
            if(d8Linked)dc.next14=nullptr; //1424E0 tail unlink; retain dyingD8 links.
            --d8.reference00;
            //295320 sees genuine NULLpairs60: no pairarray/query child ever allocated.
            d8.kind=K::entity141300;--*storage_->actualObjectCount423F60;
        }
        detector.retired=true;
    }
    if(freshDC){
        if(dcLive){
            if(dcLinked)e0.next14=nullptr; //1424E0 tail unlink, preserve dying DC links.
            --dc.reference00;
            //295B90 frees actual80 before8C; NULL failed allocation is allowed.
            if(!p.physical_.pointRecords.release295B90()||!p.physical_.records.release295B90())std::terminate();
            dc.kind=K::entity141300;--*storage_->actualObjectCount423F60;
        }
        contact.retired=true;progress.contactSuffixRetired=true;
        progress.contactSuffixDestroyed=(dcLive?1u:0u)+(d8Live?1u:0u);
    }
    if(freshE0){
        if(e0Linked)d4.next14=nullptr; //1424E0 tail unlink; dying parent/previous remain history.
        p.queries_.retireFreshE0(e0,*storage_->actualObjectCount423F60,e0Linked?&root:nullptr,e0Linked?&d4:nullptr);
        progress.e0PrefixRetired=true;
    }
    if(bodyCut){
        //1424E0 tail-first child traversal: sever predecessor14, then142590
        //drops the actual ref. Preserve dying parent1C/previous10 and body data.
        auto retireBodyLeaf=[&](OriginalWreckComponent50LA& leaf){
            if(leaf.previous10)leaf.previous10->next14=nullptr;leaf.next14=nullptr;
            --leaf.reference00;
            //2826D0 frees only its one borrowed-target pointer-array backing;
            //the native slot is embedded.1679D0->165260 has actual28C=NULL
            //on this pre-E0 route. Both reach1413E0 once, never delete model288.
            if(leaf.kind==K::body167920)leaf.kind=K::body164ED0;
            leaf.kind=K::entity141300;--*storage_->actualObjectCount423F60;
        };
        if(!freshBody||d4Live)retireBodyLeaf(d4);
        if(!freshBody||d0Live)retireBodyLeaf(d0);
        //Outside domain is live-only. Drop retired D0/D4 entries, not history.
        std::size_t live=0;
        for(std::size_t i=0;i<outside.count;++i)
            if(objects[i]->reference00)objects[live++]=objects[i];
        outside.count=live;
    }
    if(model){
        if(!owner.releaseModelReference142590(outside).okay())std::terminate();
        if(packets&&!p.modelPackets_.release240450().okay())std::terminate();
        rr::assets::OriginalStandardMaterialDiagnostic50LA md;
        if(!p.modelMaterials_.release26E4C0(md))std::terminate();
        if(geometry){rr::assets::OriginalStandardResourceDiagnostic50LA gd;
            if(!p.modelGeometry_->release26E598(&owner,p.modelMaterials_,gd))std::terminate();
            progress.geometryReferenceReleased=true;
        }
        if(!owner.freeSourceRowTable26E5CC().okay())std::terminate();
        if(p.modelMaterials_.started()&&!p.modelMaterials_.freeAllocation26E5EC())std::terminate();
        if(!owner.releaseNativePartialSceneBase2896D0(graph).okay())std::terminate();
    }
    // Actual142590 for the piece after its genuine model child is retired.
    // Preserve the dying parent's previous10/parent1C and all payload aliases.
    if(root.firstChild18)std::terminate();
    if(root.previous10)root.previous10->next14=root.next14;
    if(root.parent1C&&root.parent1C->firstChild18==&root)root.parent1C->firstChild18=root.next14;
    --root.reference00;
    //2CB500: actual constructor160/164/170 null branches (no arrays attempted).
    //165BD0 then1413E0. No query source completion is manufactured.
    root.kind=K::entity141300;--*storage_->actualObjectCount423F60;
    //Native world borrow admission ends only at the genuine enclosing piece
    //lifetime boundary, not when the earlier model/body child dies.
    p.visibilityWorldB4_=nullptr;p.collisionWorldB8_=nullptr;p.collisionLifecycleB8_=nullptr;
    p.setupZonesBound_=false;
    OriginalWreckPrefixRetirementResult50LA out;
    out.status=S::pieceRetired;out.sourcePC=0x141418u;
    out.objectsDestroyed=before-*storage_->actualObjectCount423F60;
    out.referencesReleased=out.objectsDestroyed; //Each admitted genuine Entity had exactly ref1.
    progress.pieceRetired=true;progress.retirementResult=out;p.nativePreBodyRetired_=true;
    return out;
}

    OriginalWreckConstructorPrefixResult50LA constructNativeOccurrenceContact(std::size_t pieceIndex,const void* vector443550,
        const void* vector4435A0,const OriginalWreckTerrainProvider50LA* const* terrain330)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!vector443550||!vector4435A0||!terrain330)return {};
        SpringVector4_48N detector,spring;
        original_wreck_physical_detail50LA::copy(detector,vector443550);
        original_wreck_physical_detail50LA::copy(spring,vector4435A0);
        //Synchronous copy-only call from the SAME App global lanes. No retained
        //alternate global authority; source330 remains its real racer field.
        return constructOccurrenceContactSuffix(storage_->nativeCollectionCursor,pieceIndex,{&detector,&spring,terrain330});
    }
    OriginalWreckConstructorPrefixResult50LA publishNativeOccurrenceE0(std::size_t pieceIndex)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode)return {};
        auto* piece=constructionOccurrence(storage_->nativeCollectionCursor,pieceIndex);if(!piece)return {};
        const auto id=piece->queries_.nativeModelIdentityForPublication();
        if(!id.value)return {OriginalWreckConstructorPrefixStatus50LA::binding,0x1253f0u};
        return publishPrepareAppendOccurrenceE0(storage_->nativeCollectionCursor,pieceIndex,id);
    }
    // Private native occurrence continuation through the actual query
    // prefix and effects/WATER, before the group. Reuse concrete source
    // destructors and their real progress, never relabel as an unreached E0.
    OriginalWreckPrefixRetirementResult50LA retireNativeQueryPrefixPiece(
        std::size_t ci,std::size_t pi,const LiveZoneWorld50EN& world)noexcept {
        using C=OriginalWreckConstructorPrefixStatus50LA;
        using R=OriginalWreckPrefixRetirementStatus50LA;
        OriginalWreckPrefixRetirementResult50LA rejected;
        auto* piece=retirementOccurrence(ci,pi);if(!piece)return rejected;
        auto& p=*piece;auto& progress=p.progress_;
        if(p.nativePreBodyRetired_)return progress.retirementResult;
        //Each actual source cohort retains its own array38/ordinal and
        //tail-first retirement gates. Never substitute collection0 for ci.
        const auto& cohort=storage_->collections[ci];
        if(!storage_->nativeSourceMode||!cohort.nativeSourceMode_||
           cohort.sourceArray38_!=cohort.ordered38_.get()||cohort.sourceArray38_[pi]!=&p||
           p.collectionOrdinal_!=cohort.ordinal_||p.pieceOrdinal_!=pi+1u||
           (cohort.nativeGroupAttempted_&&!cohort.nativeGroupRetired_)||
           !progress.e0ConstructorAttempted||!p.setupZonesBound_||p.collisionWorldB8_!=&world)return rejected;
        switch(progress.prefixResult.status){
        case C::queryPublicationRequired:case C::queryConstructionFailed:
        case C::contactConstructorRequired:case C::contactConstructionFailed:
        case C::e4ConstructorRequired:case C::e4PreparationRequired:
        case C::e4ConstructionFailed:case C::e4PreparationFailed:
        case C::e8ConstructorRequired:case C::e8ConstructionFailed:break;
        case C::mutualFiltersRequired:
            if(progress.prefixResult.sourcePC!=0x166390u)return rejected;
            break;
        case C::mutualFiltersFailed:case C::effectInputsRequired:
        case C::effectConstructorRequired:case C::effectReturnRequired:
        case C::effectArraysFailed:case C::effectChildrenRequired:
        case C::effectSmokeFailed:case C::effectFireRequired:case C::effectFireFailed:
        case C::effectNextPairRequired:case C::effectDustRequired:case C::effectDustFailed:
        case C::effectSplashRequired:case C::effectSplashFailed:
        case C::collectionFiltersRequired:case C::collectionFiltersFailed:
        case C::nextPieceRequired:case C::groupConstructorRequired:break;
        default:return rejected;
        }
        // External effect children belong to the actual Racer7FC forest, not
        // the piece. Detached cancellation retires that collection's actual
        //external suffix first; attached source teardown retains live borrowers
        //until all later Racer collection siblings have been destroyed.
        // Read only source-written slots, including a partially filled pair.
        if(p.effectInputsCommitted_){
            const bool sourceParentOrder=storage_->collections[ci].nativeRacerAppended_;
            if(!p.effectInputsStillBound()||!p.effectParent_||p.effectParent_->reference00!=1u)return rejected;
            const auto retired=[&](const OriginalWreckComponent50LA* child){
                if(sourceParentOrder)return child&&child->reference00==1u&&child->parent1C==p.effectParent_&&
                    (child->kind==OriginalWreckComponentKind50LA::effect19A5E0||
                     child->kind==OriginalWreckComponentKind50LA::effect15DE00||child->kind==OriginalWreckComponentKind50LA::effect19DBE0)&&
                    original_wreck_component_detail50LA::effectControlIdentityBound(*child);
                return child&&!child->reference00&&child->kind==OriginalWreckComponentKind50LA::entity141300;
            };
            const auto& arrays=p.effectArrays_;
            if(arrays.smokeWritten_>arrays.count_||arrays.fireWritten_>arrays.count_||
               (arrays.smokeWritten_&&!arrays.smoke160_)||
               (arrays.fireWritten_&&!arrays.fire164_))return rejected;
            for(std::size_t i=0;i<arrays.smokeWritten_;++i)if(!retired(arrays.smoke160_[i]))return rejected;
            for(std::size_t i=0;i<arrays.fireWritten_;++i)if(!retired(arrays.fire164_[i]))return rejected;
            if((p.dust168_&&!retired(p.dust168_))||(p.splash16C_&&!retired(p.splash16C_)))return rejected;
        }else if(p.effectArrays_.attempted()||p.dust168_||p.splash16C_)return rejected;
        // Each concrete routine preflights before its own source mutations.
        // A later refusal retains those genuine completed retirement steps;
        // retry uses their real idempotent cursors, not invented completion.
        if(progress.e8ConstructorAttempted&&!retireE8PrefixOccurrence(ci,pi))return rejected;
        if(progress.e4ConstructorAttempted&&!(progress.e4PublicationAttempted?
            retirePreparedE4Occurrence(ci,pi):retireE4PrefixOccurrence(ci,pi)))return rejected;
        if(progress.contactSuffixAttempted&&!retireContactSuffixOccurrence(ci,pi))return rejected;
        if(!(progress.e0PublicationAttempted?retirePublishedE0Occurrence(ci,pi):
            retireUnpublishedE0Occurrence(ci,pi)))return rejected;
        const auto children=retireOrderedPrefixChildrenOccurrence(ci,pi);
        if(children.status!=R::modelRetirementRequired&&children.status!=R::pieceRetirementRequired)return children;
        const auto model=retireLoadedModelOccurrence(ci,pi);
        if(model.status!=R::pieceRetirementRequired)return model;
        const auto result=retirePieceOccurrence(ci,pi,world);
        if(result.status==R::pieceRetired)p.nativePreBodyRetired_=true; //native cancellation cursor only
        return result;
    }

    bool freshGroupConstructorPrefixCurrent(const OriginalWreckCollectionStorage50LA& c)const noexcept {
        using K=OriginalWreckComponentKind50LA;
        const auto& g=c.groupComponent_;
        const auto count=c.sourceCount30_;
        if((count!=1u&&count!=2u)||c.count_<count||c.sourceSlots38_!=count||
           c.completedPieceConstructors_!=count||!c.freshPieceIndexKnown_||!c.freshPieceByteOffset_||
           *c.freshPieceByteOffset_!=4u*count||!c.sourceArray38_||c.sourceArray38_!=c.ordered38_.get())return false;
        const auto& reached=c.pieces_[count-1u].progress_.prefixResult;
        if(reached.status!=OriginalWreckConstructorPrefixStatus50LA::groupConstructorRequired||
           reached.sourcePC!=0x22c278u)return false;
        if(!c.nativeConstructorOnly_||!c.nativeGroupAttempted_||!c.nativeGroupConstructed_||
           c.nativeGroupAppended_||c.nativeGroupChildAttempted_[1]||c.sourceGroupC4_||
           c.source3C_!=&g||
           g.parent1C||g.previous10||g.next14||g.firstChild18||
           g.sessionGeneration!=storage_->sessionGeneration||
           g.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&g)))return false;
        if(c.group_.freshPrimaryAttempted_){
            //Stable backing identities only: after group-first retirement the
            //primary can itself be retired; no historical body/array read.
            const auto& first=c.pieces_[0];
            const OriginalWreckGroupPiece50LA primary{const_cast<OriginalWreckPieceFields50LA*>(&first.piece_),
                const_cast<OriginalWreckPhysical50LA*>(&first.physical_),const_cast<OriginalWreckComponent50LA*>(&first.components_[0])};
            if(!c.group_.freshPrimaryPrefixCurrent(g,primary,count-1u)||
               c.nativeGroupBound_!=c.group_.freshPrimaryComplete_||
               c.nativeGroupPC_!=(c.group_.freshPrimaryComplete_?
                   (c.freshGroupSparksConfigAttempted_?0x15ae2cu:c.freshGroupSparksSetupAttempted_?c.freshGroupSparksSetupPC_:c.freshGroupSparksPublished_?0x2cc0d4u:c.nativeGroupChildAttempted_[0]?0x2cc0bcu:
                    c.freshGroupContextChecked_?0x2cc0b4u:0x2cc088u):c.group_.freshPrimaryPC_))return false;
            if(c.freshGroupContextChecked_){
                const std::array<std::uint32_t,4> vf0{0,0,0,0x3f800000u};
                if(!c.group_.freshPrimaryComplete_||!c.freshGroupPendingOriginV0_||*c.freshGroupPendingOriginV0_!=vf0||
                   c.freshGroupInputs_.resourceContextField!=first.effectInputs_.resourceContextField||
                   c.freshGroupInputs_.particleParentField!=first.effectInputs_.particleParentField||
                   !c.freshGroupContext_||c.freshGroupContext_!=first.effectResource_||
                   !c.freshGroupParent_||c.freshGroupParent_!=first.effectParent_)return false;
                //After group-first retirement these are historical identities;
                //context/parent lifetime is no longer dereferenced for proof.
                if(!c.nativeGroupRetired_&&(!first.effectInputsStillBound()||
                   *c.freshGroupInputs_.resourceContextField!=c.freshGroupContext_||
                   *c.freshGroupInputs_.particleParentField!=c.freshGroupParent_))return false;
            }else if(c.freshGroupInputs_.resourceContextField||c.freshGroupInputs_.particleParentField||
                     c.freshGroupContext_||c.freshGroupParent_||c.freshGroupPendingOriginV0_)return false;
        }else if(c.nativeGroupBound_||c.nativeGroupPC_!=0x22c298u||!c.group_.freshConstructorFieldsCurrent(g))return false;
        if(c.nativeGroupChildAttempted_[0]){
            if(!c.freshGroupContextChecked_||!c.freshGroupStack50_||
               c.freshGroupStack50_!=c.freshGroupPendingOriginV0_)return false;
            if(c.freshGroupSparksPublished_){
                if(c.freshGroupSparks_){
                    if(c.sourceGroupC0_!=&c.freshGroupSparks_->component_)return false;
                    const auto& child=*c.freshGroupSparks_;
                    //The whole-owner external suffix retires before borrowed Group/piece backing.
                    //Preserve C0 as history, not a live source child grant.
                    if(child.retired_){if(!child.retiredFreshConstructorCurrent())return false;}
                    else if(!child.preflightFreshUnappended())return false;
                }else if(c.sourceGroupC0_)return false; //Real allocation NULL was published.
            }else if(c.freshGroupSparks_||c.sourceGroupC0_)return false;
        }else if(c.freshGroupStack50_||c.freshGroupSparksPublished_||c.freshGroupSparks_||c.sourceGroupC0_)return false;
        if(c.freshGroupSparksSetupAttempted_){
            if(!c.freshGroupSparksPublished_)return false;
            if(c.freshGroupSparks_){
                const auto& child=*c.freshGroupSparks_;
                if(c.freshGroupPendingExplosionSize_||!c.freshGroupModel288_||!child.freshSetupAttempted_||
                   (c.freshGroupSparksSetupPC_!=0x15f2acu&&c.freshGroupSparksSetupPC_!=0x15f2e4u&&c.freshGroupSparksSetupPC_!=0x15f2f0u)||
                   (!child.retired_&&child.boundary_!=(c.freshGroupSparksConfigAttempted_?0x15ae2cu:c.freshGroupSparksSetupPC_))||
                   child.context_!=c.freshGroupContext_||child.freshSetupModel_!=*c.freshGroupModel288_)return false;
                if(!c.nativeGroupRetired_){const auto& primary=c.pieces_[0];
                    if(!primary.freshBody_.model288||*primary.freshBody_.model288!=*c.freshGroupModel288_||
                       !primary.sourceModel14C_||*primary.sourceModel14C_!=*c.freshGroupModel288_)return false;
                }
            }else if(c.freshGroupSparksSetupPC_!=0x2cc124u||c.freshGroupModel288_||
                !c.freshGroupPendingExplosionSize_||*c.freshGroupPendingExplosionSize_!=0x230u)return false;
        }else if(c.freshGroupSparksSetupPC_||c.freshGroupModel288_||c.freshGroupPendingExplosionSize_)return false;
        if(c.freshGroupSparksConfigAttempted_&&(!c.freshGroupSparksSetupAttempted_||
           c.freshGroupSparksSetupPC_!=0x15f2f0u||!c.freshGroupSparks_||!c.freshGroupSparks_->freshConfigAttempted_))return false;
        if(c.nativeGroupRetired_){if(g.reference00||g.kind!=K::entity141300||!c.group_.nativeRetired_)return false;}
        else if(g.reference00!=1u||g.kind!=K::group2CBFC0||c.group_.nativeRetired_)return false;
        std::size_t occurrences=0;
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]==&g)++occurrences;
        return occurrences==1u;
    }
    bool constructFreshGroup22C278(const std::array<std::uint32_t,4>& actual4441F0,
        std::uint32_t& pc)noexcept {
        pc=0x22c278u;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||
           storage_->nativeCollectionCursor||!preflightFreshConstructorOnlyRetirement())return false;
        auto& c=storage_->collections[0];using P=OriginalWreckConstructorPrefixStatus50LA;
        const auto count=c.sourceCount30_;
        if(!c.nativeConstructorOnly_||(count!=1u&&count!=2u)||c.count_<count||
           c.sourceSlots38_!=count||c.completedPieceConstructors_!=count||
           !c.freshPieceIndexKnown_||!c.freshPieceByteOffset_||*c.freshPieceByteOffset_!=4u*count||
           !c.sourceArray38_||c.sourceArray38_!=c.ordered38_.get()||
           c.nativeGroupAttempted_||c.source3C_||c.retirementAttempted_||
           c.group_.allocated_!=c.count_-1u||
           !storage_->actualObjectCount423F60||*storage_->actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max()||
           storage_->prefixDomain.count>=storage_->prefixComponents.size())return false;
        for(std::size_t i=0;i<count;++i)
            if(c.ordered38_[i]!=&c.pieces_[i]||!c.pieces_[i].progress_.appended22C1B8||
               c.pieces_[i].progress_.pieceRetired||!c.pieces_[i].effectInputsStillBound())return false;
        const auto& last=c.pieces_[count-1u].progress_.prefixResult;
        if(last.status!=P::groupConstructorRequired||last.sourcePC!=0x22c278u)return false;
        auto& g=c.groupComponent_;
        if(g.nativeFlags29||g.kind!=OriginalWreckComponentKind50LA::unbound||g.reference00||g.objectKey||
           g.previous10||g.next14||g.parent1C||g.firstChild18)return false;
        if(!c.group_.selectFreshUnusedBacking())return false;
        //The native component already exists. Guest malloc(0xD0)/NULL branch
        //is not emulated; unused native array prewarm was explicitly cancelled.
        c.nativeGroupAttempted_=true;
        const auto r=initializeOriginalWreckComponent50LA(g,OriginalWreckComponentKind50LA::entity141300,
            storage_->sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&g)),
            true,*storage_->actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)std::terminate(); //preflight excludes every refusal
        storage_->prefixComponents[storage_->prefixDomain.count++]=&g;
        c.group_.initializeFreshFields2C9E40(g,actual4441F0);
        //Native kind discriminates the derived owner, not raw guest vtable bytes.
        g.kind=OriginalWreckComponentKind50LA::group2CBFC0; //2CBFDC
        c.sourceGroupC0_=nullptr; //2CBFE0
        c.sourceGroupC4_=nullptr; //2CBFE4
        c.nativeGroupConstructed_=true;
        c.source3C_=&g; //22C294, after completed2CBFC0 self-return
        pc=c.nativeGroupPC_=0x22c298u;return true; //before38/count/primary reads
    }
    bool bindFreshGroupPrimary22C298(std::uint32_t& pc)noexcept {
        pc=0x22c298u;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||
           storage_->nativeCollectionCursor||!preflightFreshConstructorOnlyRetirement())return false;
        auto& c=storage_->collections[0];
        if(!freshGroupConstructorPrefixCurrent(c)||c.nativeGroupRetired_||
           c.group_.freshPrimaryAttempted_||c.nativeGroupBound_||c.nativeGroupPC_!=pc)return false;
        //The source reads SAME38, actual30, SAME3C and38[0]. No native
        //reservation capacity substitutes for actual Config count minus one.
        pc=0x22c298u;if(!c.sourceArray38_||c.sourceArray38_!=c.ordered38_.get())return false;
        pc=0x22c29cu;const auto count=c.sourceCount30_;if(count!=1u&&count!=2u)return false;
        pc=0x22c2a4u;if(c.source3C_!=&c.groupComponent_)return false;
        pc=0x22c2acu;if(c.sourceSlots38_<1u||c.ordered38_[0]!=&c.pieces_[0])return false;
        auto& primary=c.pieces_[0];
        if(!primary.progress_.appended22C1B8||primary.progress_.pieceRetired||
           !primary.componentSlots_||primary.componentSlots_[0]!=&primary.components_[0]||
           !primary.sourceBodyD0_||*primary.sourceBodyD0_!=primary.componentSlots_[1u+primary.sceneCount_]||
           !*primary.sourceBodyD0_||!(*primary.sourceBodyD0_)->reference00||
           !primary.physical_.bodyConstructorFieldsApplied||!primary.effectInputsStillBound())return false;
        //Native identity admission precedes source writes. Actual D0 has a
        //separate component identity and SAME canonical physical field owner.
        pc=0x22c2b4u;const auto capacity=count-1u;
        const bool done=c.group_.bindFreshPrimary2CA020(
            {&primary.piece_,&primary.physical_,primary.componentSlots_[0]},capacity,pc);
        if(c.group_.freshPrimaryAttempted_)c.nativeGroupPC_=pc;
        if(!done)return false; //Retains exact partial allocation/store boundary.
        c.nativeGroupBound_=true;pc=c.nativeGroupPC_=0x2cc088u;
        return true; //Before actual context/parent branches; no group children.
    }
    bool beginFreshGroupSparksConfig15F2F0(std::uint32_t& pc)noexcept {
        pc=0x15f2f0u;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||storage_->nativeCollectionCursor||
           !preflightFreshConstructorOnlyRetirement())return false;
        auto& c=storage_->collections[0];
        if(c.nativeGroupRetired_||!c.freshGroupSparksSetupAttempted_||c.freshGroupSparksSetupPC_!=pc||
           c.nativeGroupPC_!=pc||c.freshGroupSparksConfigAttempted_||!c.freshGroupSparks_||
           c.sourceGroupC0_!=&c.freshGroupSparks_->component_||!c.freshGroupSparks_->beginFreshConfig15F2F0())return false;
        c.freshGroupSparksConfigAttempted_=true;pc=c.nativeGroupPC_=0x15ae2cu;return true;
    }
    bool setupFreshGroupSparks2CC0D4(const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        pc=0x2cc0d4u;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||storage_->nativeCollectionCursor||
           !preflightFreshConstructorOnlyRetirement())return false;
        auto& c=storage_->collections[0];
        if(c.nativeGroupRetired_||!c.freshGroupSparksPublished_||c.freshGroupSparksSetupAttempted_||c.nativeGroupPC_!=pc)return false;
        //2CC0D8 is BEQL: knownNULL executes delay-slot size230 then jumps2CC124.
        //The nonnull route annuls2CC0DC; no explosion allocation has run here.
        if(!c.sourceGroupC0_){
            if(c.freshGroupSparks_)return false;
            c.freshGroupSparksSetupAttempted_=true;
            c.freshGroupPendingExplosionSize_=0x230u; //Taken BEQL delay slot2CC0DC.
            pc=c.nativeGroupPC_=c.freshGroupSparksSetupPC_=0x2cc124u;return true;
        }
        auto& primary=c.pieces_[0];auto* child=c.freshGroupSparks_;
        const auto* bound=c.group_.primary();
        if(!child||c.sourceGroupC0_!=&child->component_||!child->freshConstructorFieldsCurrent()||
           !bound||bound->physical!=&primary.physical_||bound->piece!=&primary.piece_||
           bound->component!=primary.componentSlots_[0]||!primary.sourceBodyD0_||
           *primary.sourceBodyD0_!=primary.componentSlots_[1u+primary.sceneCount_]||
           !primary.freshBody_.model288||!primary.sourceModel14C_||
           *primary.freshBody_.model288!=*primary.sourceModel14C_||
           *primary.sourceModel14C_!=&primary.modelOwner_.model_||primary.modelOwner_.released()||
           !primary.modelOwner_.model_.constructed||primary.modelOwner_.model_.released||
           !c.freshGroupStack50_||!c.freshGroupContext_||!primary.effectInputsStillBound())return false;
        const auto graph=primary.assets_->graph();
        if(primary.physical_.model288!=primary.assets_->model14C()||
           primary.physical_.modelGraphNodes!=graph.nodes||primary.physical_.modelGraphCount!=graph.count||
           primary.modelOwner_.model_.sceneIndex!=primary.assets_->model14C())return false;
        //Actual2CC0E0 primary3C then2CC0F0 body288; same typed Model, not graph-index substitution.
        const auto* model=*primary.freshBody_.model288;
        if(child->freshSetupAttempted_)return false;
        const bool done=child->setupFresh15F1E0(*c.freshGroupContext_,model,*c.freshGroupStack50_,actualDirectory36B530);
        if(!child->freshSetupAttempted_)return false; //Native admission refusal preserves prior cleanup state.
        c.freshGroupModel288_=model;c.freshGroupSparksSetupAttempted_=true;
        pc=c.nativeGroupPC_=c.freshGroupSparksSetupPC_=child->boundary_;
        return done; //Config15AE00, Stop84 and append remain unentered.
    }
    bool beginFreshGroupSparks2CC0B4(std::uint32_t& pc)noexcept {
        pc=0x2cc0b4u;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||storage_->nativeCollectionCursor||
           !preflightFreshConstructorOnlyRetirement())return false;
        auto& c=storage_->collections[0];
        if(c.nativeGroupRetired_||!c.freshGroupContextChecked_||c.nativeGroupPC_!=pc||
           c.nativeGroupChildAttempted_[0]||!c.freshGroupPendingOriginV0_)return false;
        //JAL delay-slot SQ50 executes BEFORE the allocator returns or the NULL branch.
        c.freshGroupStack50_=c.freshGroupPendingOriginV0_;
        c.nativeGroupChildAttempted_[0]=true;pc=c.nativeGroupPC_=0x2cc0bcu;return true;
    }
    bool finishFreshGroupSparks2CC0BC(OriginalWreckGroupSparksOwner50LA* child,
        const OriginalWreckGroupSparksBinding50LA* binding,std::uint32_t& pc)noexcept {
        pc=0x2cc0bcu;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||storage_->nativeCollectionCursor||
           !preflightFreshConstructorOnlyRetirement())return false;
        auto& c=storage_->collections[0];
        if(c.nativeGroupRetired_||!c.nativeGroupChildAttempted_[0]||c.freshGroupSparksPublished_||
           c.nativeGroupPC_!=pc||!c.freshGroupStack50_)return false;
        if(child){
            if(!binding||binding->particleParent!=c.freshGroupParent_||
               !sameExternalEffectForest(*binding)||!child->constructFresh15F100(*binding))return false;
            c.freshGroupSparks_=child;
            c.sourceGroupC0_=&child->component_; //2CC0D0 only AFTER15F100 returns.
        }else{
            if(binding)return false;
            c.sourceGroupC0_=nullptr; //2CC0BC NULL branch skips constructor; same2CC0D0 store.
        }
        c.freshGroupSparksPublished_=true;pc=c.nativeGroupPC_=0x2cc0d4u;
        return true; //No2CC0E0 primary model288 read, setup, Stop84 or append.
    }
    bool consumeFreshGroupContext2CC088(const OriginalWreckPieceEffectInputs50LA& inputs,
        std::uint32_t& pc)noexcept {
        pc=0x2cc088u;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||
           storage_->nativeCollectionCursor||!preflightFreshConstructorOnlyRetirement())return false;
        auto& c=storage_->collections[0];
        if(!freshGroupConstructorPrefixCurrent(c)||c.nativeGroupRetired_||
           !c.group_.freshPrimaryComplete_||!c.nativeGroupBound_||c.nativeGroupPC_!=pc||
           c.freshGroupContextChecked_)return false;
        const auto& primary=c.pieces_[0];
        //22BFD0 saved incoming Track4A0 in s3 and actual7FC on stack320.
        //22C2A8/22C2A0 pass those SAME borrowed fields, not new context owners.
        if(!primary.effectInputsStillBound()||
           inputs.resourceContextField!=primary.effectInputs_.resourceContextField||
           inputs.particleParentField!=primary.effectInputs_.particleParentField||
           !inputs.resourceContextField||!inputs.particleParentField)return false;
        const auto* context=*inputs.resourceContextField;
        if(!context||context!=primary.effectResource_)return false; //2CC088: admitted nonnull route only.
        pc=0x2cc090u;auto* parent=*inputs.particleParentField;
        if(!parent||parent!=primary.effectParent_)return false; //No fabricated NULL-return branch.
        //The source transient60 stores precede its LQ into pending v0.
        //This raw vector is not an effect constructor, pool or child grant.
        std::array<std::uint32_t,4> local60;
        pc=0x2cc098u;local60[0]=0;
        pc=0x2cc09cu;local60[1]=0;
        pc=0x2cc0a0u;local60[2]=0;
        pc=0x2cc0a8u;local60[3]=0x3f800000u;
        c.freshGroupInputs_=inputs;c.freshGroupContext_=context;c.freshGroupParent_=parent;
        pc=0x2cc0acu;c.freshGroupPendingOriginV0_=local60; //Actual LQ value, not yet SQ50.
        pc=0x2cc0b4u; //Stop before allocation call and its delay-slot SQ50.
        c.freshGroupContextChecked_=true;c.nativeGroupPC_=pc;
        return true;
    }
    bool constructNativeGroup22C278(const void* actual4441F0,std::uint32_t& pc)noexcept {
        pc=0x22c278u;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||!actual4441F0)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(c.nativeGroupAttempted_||c.retirementAttempted_||c.nativeConstructorOnly_||
           !c.sourceCount30_||c.sourceCount30_!=c.count_||c.sourceSlots38_!=c.count_||
           c.completedPieceConstructors_!=c.count_||c.source3C_||!c.sourceArray38_||
           c.sourceArray38_!=c.ordered38_.get()||c.group_.allocationCapacity()!=c.count_-1u||
           !c.group_.coldBackingPrepared()||!storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return false;
        for(std::size_t i=0;i<c.count_;++i){const auto& p=c.pieces_[i];
            if(c.ordered38_[i]!=&p||!p.progress_.appended22C1B8||p.progress_.pieceRetired||
               p.progress_.retirementAttempted||!p.effectInputsStillBound())return false;
        }
        if(c.pieces_[c.count_-1u].progress_.prefixResult.status!=OriginalWreckConstructorPrefixStatus50LA::groupConstructorRequired||
           storage_->prefixDomain.count>=storage_->prefixComponents.size())return false;
        c.nativeGroupAttempted_=true;
        auto& g=c.groupComponent_;
        const auto r=initializeOriginalWreckComponent50LA(g,OriginalWreckComponentKind50LA::group2CBFC0,
            storage_->sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&g)),
            true,*storage_->actualObjectCount423F60);
        pc=c.nativeGroupPC_=r.sourcePC;if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        storage_->prefixComponents[storage_->prefixDomain.count++]=&g;
        SpringVector4_48N origin;original_wreck_physical_detail50LA::copy(origin,actual4441F0);
        if(c.group_.initializeFields2C9E40(g,origin).status!=OriginalWreckGroupArrayStatus50LA::fieldsInitialized)return false;
        c.sourceGroupC0_=nullptr; //2CBFE0
        c.sourceGroupC4_=nullptr; //2CBFE4
        c.nativeGroupConstructed_=true;
        c.source3C_=&g;pc=c.nativeGroupPC_=0x22c294u;
        auto& primary=*c.ordered38_[0];
        if(!primary.sourceBodyD0_||*primary.sourceBodyD0_!=primary.componentSlots_[1u+primary.sceneCount_]||
           !primary.sourceBodyD0_.value()->reference00||!primary.assets_)return false;
        const auto bound=c.group_.bindPrimary2CA020({&primary.piece_,&primary.physical_,primary.componentSlots_[0]});
        pc=c.nativeGroupPC_=bound.sourcePC;if(bound.status!=OriginalWreckGroupArrayStatus50LA::primaryBound)return false;
        c.nativeGroupBound_=true;pc=c.nativeGroupPC_=0x2cc0b4u;return true;
    }
    template<class Child,class Binding> bool constructNativeGroupEffect(
        Child& child,const Binding& binding,bool explosion,std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];const auto index=explosion?1u:0u;
        if(!c.nativeGroupBound_||c.nativeGroupRetired_||c.nativeGroupChildAttempted_[index]||
           c.nativeGroupPC_!=(explosion?0x2cc120u:0x2cc0b4u)||c.source3C_!=&c.groupComponent_)return false;
        auto& primary=*c.ordered38_[0];
        if(!primary.effectInputsStillBound()||child.context_!=primary.effectResource_||
           binding.particleParent!=primary.effectParent_||!sameExternalEffectForest(binding)||!child.admit(binding))return false;
        c.nativeGroupChildAttempted_[index]=true;
        if(!child.constructSource(binding)){pc=c.nativeGroupPC_=child.sourceBoundary();return false;}
        (explosion?c.sourceGroupC4_:c.sourceGroupC0_)=&child.component_; //2CC140/2CC0D0 before setup
        const bool done=child.setupStopAppendSource(*primary.assets_,primary.physical_);
        pc=c.nativeGroupPC_=child.sourceBoundary();return done;
    }
    bool appendNativeGroup22C2C0(std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(!c.nativeGroupBound_||c.nativeGroupAppended_||c.nativeGroupRetired_||c.nativeGroupPC_!=0x2cc190u||
           c.source3C_!=&c.groupComponent_||!c.sourceGroupC0_||!c.sourceGroupC4_||
           c.sourceGroupC0_->reference00!=1u||c.sourceGroupC4_->reference00!=1u)return false;
        const auto r=appendOriginalWreckComponent50LA(storage_->prefixDomain,c.collectionComponent_,c.groupComponent_,
            2u,*storage_->actualAttachmentLock423F58);
        pc=c.nativeGroupPC_=r.sourcePC;if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        c.nativeGroupAppended_=true;pc=c.nativeGroupPC_=0x22c2c8u;
        return true; //actual next virtual18 is22CE10; it has NOT run
    }
    bool beginNativeLoadDeactivate22CE10(std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(c.nativeLoadDeactivateAttempted_||!c.nativeGroupAppended_||c.nativeGroupRetired_||
           c.nativeGroupPC_!=0x22c2c8u||c.source3C_!=&c.groupComponent_||
           c.sourceSlots38_!=c.count_||c.completedPieceConstructors_!=c.count_)return false;
        c.nativeLoadDeactivateAttempted_=true;pc=c.nativeGroupPC_=0x22ce24u;
        const auto r=setOriginalWreckBaseActivity50LA(storage_->prefixDomain,c.collectionComponent_,false);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        pc=c.nativeGroupPC_=0x22ce2cu;return true;
    }
    bool deactivateNativeGroup2CC9C0(OriginalWreckGroupSparksOwner50LA& sparks,
        OriginalWreckGroupExplosionOwner50LA& explosion,OriginalWreckSplashOwner50LA& splash,
        std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];auto& primary=c.pieces_[0];
        const auto* actualPrimary=c.group_.primary();
        if(!actualPrimary||actualPrimary->piece!=&primary.piece_||actualPrimary->physical!=&primary.physical_||
           actualPrimary->component!=primary.componentSlots_[0])return false;
        if(!c.nativeLoadDeactivateAttempted_||c.nativeGroupPC_!=0x22ce2cu||c.nativeGroupRetired_||
           c.sourceGroupC0_!=&sparks.component_||c.sourceGroupC4_!=&explosion.component_||
           primary.splash16C_!=&splash.component_||!primary.effectInputsStillBound()||
           !splash.appended_||splash.retired_||splash.component_.reference00!=1u)return false;
        pc=c.nativeGroupPC_=0x2cc9e4u;if(!sparks.resetSource88())return false;
        pc=c.nativeGroupPC_=0x2cc9f8u;if(!sparks.stopSource84())return false;
        pc=c.nativeGroupPC_=0x2cca14u;if(!explosion.resetSource88())return false;
        pc=c.nativeGroupPC_=0x2cca28u;if(!explosion.stopSource84())return false;
        pc=c.nativeGroupPC_=0x2cca48u;stopEffect50BC(splash.controls_.state);
        pc=c.nativeGroupPC_=0x2cca54u;
        if(setOriginalWreckBaseActivity50LA(storage_->prefixDomain,c.groupComponent_,false).status!=
            OriginalWreckComponentStatus50LA::complete)return false;
        pc=c.nativeGroupPC_=0x22ce40u;return true;
    }
    template<class SmokeSlots,class FireSlots> bool deactivateNativePiece2CBEB0(std::size_t pi,
        const SmokeSlots& smoke,const FireSlots& fire,OriginalWreckDustOwner50LA& dust,
        OriginalWreckSplashOwner50LA& splash,std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(c.nativeGroupPC_!=0x22ce40u||pi!=c.nativeDeactivatePieces_||pi>=c.sourceSlots38_||
           c.nativeGroupRetired_||c.ordered38_[pi]!=&c.pieces_[pi])return false;
        auto& p=c.pieces_[pi];auto& a=p.effectArrays_;const auto count=p.effects_.count174;
        if(!p.effectInputsStillBound()||p.progress_.pieceRetired||!p.progress_.appended22C1B8||
           p.dust168_!=&dust.component_||p.splash16C_!=&splash.component_||
           !dust.appended_||dust.retired_||!splash.appended_||splash.retired_||
           dust.component_.reference00!=1u||splash.component_.reference00!=1u||
           count>512u||smoke.size()!=count||fire.size()!=count||
           (count&&(!a.smoke160_||!a.fire164_||a.smokeWritten_!=count||a.fireWritten_!=count)))return false;
        pc=c.nativeGroupPC_=0x2cbeb0u;
        //Actual signed174 loop: dust/splash calls remain INSIDE each pair,
        //including repeated178/17C stores. Count0 invokes no effect call.
        for(std::size_t i=0;i<count;++i){
            if(!smoke[i]||!fire[i]||a.smoke160_[i]!=&smoke[i]->component_||
               a.fire164_[i]!=&fire[i]->component_||!smoke[i]->appended_||smoke[i]->retired_||
               !fire[i]->appended_||fire[i]->retired_||smoke[i]->component_.reference00!=1u||
               fire[i]->component_.reference00!=1u)return false;
            pc=c.nativeGroupPC_=0x2cbef8u;stopEffect50BC(smoke[i]->controls_.state);
            pc=c.nativeGroupPC_=0x2cbf24u;stopEffect50BC(fire[i]->controls_.state);
            pc=c.nativeGroupPC_=0x2cbf44u;stopEffect50BC(dust.controls_.state);
            p.effects_.dustActive178=0;p.effects_.dustTime17C=0.f;
            pc=c.nativeGroupPC_=0x2cbf6cu;stopEffect50BC(splash.controls_.state);
        }
        pc=c.nativeGroupPC_=0x2cbf90u;
        if(setOriginalWreckBaseActivity50LA(storage_->prefixDomain,*p.componentSlots_[0],false).status!=
            OriginalWreckComponentStatus50LA::complete)return false;
        pc=c.nativeGroupPC_=0x22ce68u;return true;
    }
    bool finishNativePieceLoadDeactivate(std::size_t pi,std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(c.nativeGroupPC_!=0x22ce68u||pi!=c.nativeDeactivatePieces_||pi>=c.sourceSlots38_||
           c.ordered38_[pi]!=&c.pieces_[pi]||c.nativeGroupRetired_)return false;
        auto& p=c.pieces_[pi];
        if(!p.sourceBodyD0_||*p.sourceBodyD0_!=p.componentSlots_[1u+p.sceneCount_]||
           !(*p.sourceBodyD0_)->reference00||!p.modelOwner_.constructed2403A0())return false;
        pc=c.nativeGroupPC_=0x22ce80u;
        if(setOriginalWreckBodyEnabled50LA(p.physical_,false)!=OriginalWreckBodyControlStatus50LA::complete)return false;
        pc=c.nativeGroupPC_=0x22ce94u;
        p.modelOwner_.model_.flags03=static_cast<std::uint8_t>(p.modelOwner_.model_.flags03|4u); //28C6E0
        pc=c.nativeGroupPC_=0x22cea8u;
        //Actual fresh model19C remains NULL: preserve the positive observer
        //branch as an owner boundary rather than inventing a removal receipt.
        if(p.modelOwner_.model_.observer19C)return false;
        const std::uint32_t pcs[]={0x22cebcu,0x22ced0u,0x22cee4u};
        for(unsigned i=0;i<3u;++i){pc=c.nativeGroupPC_=pcs[i];
            if(!p.queries_.removeNativeConstructorNull120290(i))return false;}
        ++c.nativeDeactivatePieces_;
        pc=c.nativeGroupPC_=c.nativeDeactivatePieces_<c.sourceCount30_?0x22ce40u:0x22cf08u;
        return true;
    }
    bool finishNativeCollectionLoad22CF18(std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(!c.nativeLoadDeactivateAttempted_||c.nativeLoadDeactivateComplete_||c.nativeGroupPC_!=0x22cf08u||
           c.nativeDeactivatePieces_!=c.sourceCount30_)return false;
        c.nativeLoadDeactivateComplete_=true;pc=c.nativeGroupPC_=0x212a98u;
        return true; //separate actual212AAC admission follows
    }
    bool appendNativeCollection212AAC(OriginalWreckComponent50LA& racer,std::uint32_t& pc)noexcept {
        pc=0x212aacu;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(c.nativeRacerAppendAttempted_||!c.nativeLoadDeactivateComplete_||c.nativeGroupPC_!=0x212a98u||
           c.retirementAttempted_||racer.kind!=OriginalWreckComponentKind50LA::racer27E130||racer.reference00!=1u||
           racer.objectKey!=storage_->racerKey||racer.sessionGeneration!=storage_->sessionGeneration||
           racer.parent1C||racer.previous10||racer.next14)return false;
        std::array<OriginalWreckComponent50LA*,maxOriginalWreckComponents50LA> anchors{};
        OriginalWreckComponentDomain50LA d{storage_->sessionGeneration,anchors.data(),2u};
        anchors[0]=&racer;anchors[1]=&c.collectionComponent_;
        OriginalWreckComponent50LA* previous=nullptr;
        for(auto* n=racer.firstChild18;n;n=n->next14){
            if(d.count==anchors.size()||n==&c.collectionComponent_||n->parent1C!=&racer||n->previous10!=previous||
               n->reference00!=1u||n->sessionGeneration!=racer.sessionGeneration)return false;
            anchors[d.count++]=n;previous=n;
        }
        //212AAC appends at the racer's ACTUAL tail. 210770 has already appended
        //7FC and then its models, bodies, collision and contact owners before
        //2129C0 (210C40..2125D0, 141960 each), so collection0 follows whatever
        //is last. Later collections follow the previous appended collection:
        //nothing else joins the racer between them. The 7FC particle parent
        //must still be a direct child (pieces' effects hang under it).
        const auto ordinal=storage_->nativeCollectionCursor;
        bool parentSeen=false;
        for(auto* n=racer.firstChild18;n;n=n->next14)parentSeen=parentSeen||n==c.pieces_[0].effectParent_;
        if(!c.sourceSlots38_||!previous||!c.pieces_[0].effectParent_||!parentSeen||
           (ordinal&&previous!=&storage_->collections[ordinal-1u].collectionComponent_))return false;
        c.nativeRacerAppendAttempted_=true;
        const auto r=appendOriginalWreckComponent50LA(d,racer,c.collectionComponent_,0xffffffffu,*storage_->actualAttachmentLock423F58);
        pc=c.nativeGroupPC_=r.sourcePC;if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        c.nativeRacerParent_=&racer;c.nativeRacerPrevious_=previous;c.nativeRacerAppended_=true;
        pc=c.nativeGroupPC_=0x212abcu;return true;
    }
    bool appendNativeOwnPodExclusion(std::size_t pi,std::uint32_t actual694,
        std::uint32_t& actualE4,std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(!c.nativeRacerAppended_||c.nativeRacerTailUnlinked_||c.nativeRacerExclusionFailed_||
           c.nativeRacerExclusionCursor_!=pi||pi>=c.sourceSlots38_||c.nativeGroupPC_!=0x212abcu||
           !nativeRacerChainCurrent(c)||c.ordered38_[pi]!=&c.pieces_[pi])return false;
        auto& p=c.pieces_[pi];
        if(!p.sourceE0_||!p.sourceE4_||*p.sourceE0_!=p.componentSlots_[p.sceneCount_+3u]||
           *p.sourceE4_!=p.componentSlots_[p.sceneCount_+6u]||!(*p.sourceE0_)->reference00||!(*p.sourceE4_)->reference00)return false;
        c.nativeRacerExclusionFailed_=true;pc=c.nativeGroupPC_=0x212ae0u;
        if(!p.queries_.appendNativeOwnPod694123EB0(actual694,actualE4))return false;
        pc=c.nativeGroupPC_=0x212b0cu;return true;
    }
    bool commitNativeMain690Exclusion(std::size_t pi,std::uint32_t& pc)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(!c.nativeRacerExclusionFailed_||c.nativeRacerExclusionCursor_!=pi||c.nativeGroupPC_!=0x212b0cu)return false;
        ++c.nativeRacerExclusionCursor_;c.nativeRacerExclusionFailed_=false;
        //Only advance after the actual source30 paired list loop completed.
        //The source370 loop selects a new constructor, never a planned slot.
        pc=c.nativeGroupPC_=c.nativeRacerExclusionCursor_<c.sourceCount30_?0x212abcu:
            (storage_->nativeCollectionCursor+1u<storage_->count?0x2129e0u:0x212b50u);return true;
    }
    std::size_t nativeCollectionOrdinal()const noexcept {return storage_?storage_->nativeCollectionCursor:0u;}
    bool nativeCollectionsReached212B50()const noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count||
           storage_->nativeCollectionCursor+1u!=storage_->count||storage_->nativePublished374!=storage_->count)return false;
        for(std::size_t i=0;i<storage_->count;++i){const auto& c=storage_->collections[i];
            if(storage_->ordered374[i]!=&c||!c.nativeLoadDeactivateComplete_||!c.nativeRacerAppended_||
               c.nativeRacerTailUnlinked_||c.retirementAttempted_||c.nativeRacerExclusionFailed_||
               c.nativeRacerExclusionCursor_!=c.sourceCount30_||c.sourceCount30_!=c.sourceSlots38_||
               c.sourceSlots38_!=c.completedPieceConstructors_||!nativeRacerChainCurrent(c)||
               c.nativeGroupPC_!=(i+1u<storage_->count?0x2129e0u:0x212b50u))return false;
        }
        return true;
    }
    bool nativeCollectionAppendedToRacer(std::size_t ordinal)const noexcept {
        return storage_&&ordinal<storage_->count&&storage_->collections[ordinal].nativeRacerAppended_;
    }
    bool nativeCollectionAppendedToRacer()const noexcept {
        return nativeCollectionAppendedToRacer(0); //any prior append implies collection0
    }
    bool unlinkNativeRacerTailForRetirement(std::size_t ordinal)noexcept {
        if(!nativeCollectionAppendedToRacer(ordinal))return true;
        auto& c=storage_->collections[ordinal];
        if(c.nativeRacerTailUnlinked_)return !c.collectionComponent_.reference00||nativeRacerChainCurrent(c);
        if(!nativeRacerChainCurrent(c)||c.collectionComponent_.next14||!storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return false;
        //Actual caller1424E0 tail unlink precedes142590 child traversal. Do
        //not clear the dying child's parent/previous or recompute ancestor24.
        if(!c.nativeRacerPrevious_)return false;
        c.nativeRacerPrevious_->next14=nullptr; //142560; source142564 next already NULL
        c.nativeRacerTailUnlinked_=true;return true;
    }
    bool retireNativeGroupBeforeActivation(std::size_t ordinal)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return false;
        auto& c=storage_->collections[ordinal];if(!c.nativeGroupAttempted_||c.nativeGroupRetired_)return true;
        auto& g=c.groupComponent_;using K=OriginalWreckComponentKind50LA;
        if(!c.nativeGroupConstructed_&&!g.reference00&&g.kind==K::unbound&&
           !g.parent1C&&!g.previous10&&!g.next14&&!g.firstChild18&&!c.source3C_&&
           !c.group_.fields_&&!c.nativeGroupChildAttempted_[0]&&!c.nativeGroupChildAttempted_[1]){
            c.nativeGroupRetired_=true;return true; //native unconstructed cancellation only
        }
        if(!c.nativeGroupConstructed_||!c.group_.fields_||(!nativeTuningAttempted_&&c.group_.registered30_)||
           c.source3C_!=&g||g.reference00!=1u||g.kind!=K::group2CBFC0||g.firstChild18||g.next14||
           !storage_->actualObjectCount423F60||!*storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return false;
        const OriginalWreckComponent50LA* groupChildren[]={c.sourceGroupC0_,c.sourceGroupC4_};
        const K groupKinds[]={K::effect15F100,K::effect15E800};
        for(unsigned i=0;i<2u;++i)if(const auto* child=groupChildren[i]){
            if(c.nativeRacerAppended_){
                if(child->reference00!=1u||child->kind!=groupKinds[i]||child->parent1C!=c.pieces_[0].effectParent_||
                   !original_wreck_component_detail50LA::effectControlIdentityBound(*child))return false;
            }else if(child->reference00||child->kind!=K::entity141300)return false;
        }
        if(nativeTuningAttempted_){if(!nativeTuningRowsRetirable(c))return false;}
        else if(c.group_.bound_)for(std::size_t i=0;i<c.group_.allocated_;++i)
            if(c.group_.pieces44_[i].piece||c.group_.attachedA0_[i]||c.group_.breakLatchB0_[i])return false;
        if(c.nativeGroupAppended_){
            if(g.parent1C!=&c.collectionComponent_||!g.previous10||
               g.previous10!=c.pieces_[c.count_-1u].componentSlots_[0]||g.previous10->next14!=&g)return false;
        }else if(g.parent1C||g.previous10)return false;
        if(g.previous10)g.previous10->next14=nullptr;
        --g.reference00;
        if(!c.group_.retireNativeUnactivated2C9F30(nativeTuningAttempted_))std::terminate();
        g.kind=K::entity141300;--*storage_->actualObjectCount423F60;
        c.nativeGroupRetired_=true;return true; //source3C/C0/C4 retained as history
    }
    bool retireNativeCollectionPieceSourcePrefix(const LiveZoneWorld50EN& world)noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(!sourceIdentityBound()||!storage_->nativeSourceMode)return false;
        if(storage_->nativeSourcePrefixRetired)return true;
        if(!storage_->actualObjectCount423F60){
            //Global borrow binding/370/374 adoption can fail before any source
            //collection constructor. No counted reference may be cancelled.
            for(std::size_t i=0;i<storage_->count;++i)if(storage_->collections[i].constructorAttempted_)return false;
            storage_->nativeSourcePrefixRetired=true;return true;
        }
        if(!storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return false;
        for(std::size_t ci=storage_->count;ci>0;--ci){auto& c=storage_->collections[ci-1u];
            if(!c.constructorAttempted_)continue;
            if(c.retirementAttempted_)continue;
            if(!unlinkNativeRacerTailForRetirement(ci-1u)||!retireNativeGroupBeforeActivation(ci-1u))return false;
            for(std::size_t pi=c.sourceSlots38_;pi>0;--pi){
                const auto& piece=c.pieces_[pi-1u];
                const auto result=piece.progress_.e0ConstructorAttempted?
                    retireNativeQueryPrefixPiece(ci-1u,pi-1u,world):retireNativePieceBeforeE0(ci-1u,pi-1u,world);
                if(result.status!=OriginalWreckPrefixRetirementStatus50LA::pieceRetired)return false;
            }
            auto& entity=c.collectionComponent_;
            if((!c.nativeRacerTailUnlinked_&&(entity.parent1C||entity.previous10))||entity.next14||entity.firstChild18||
               c.groupComponent_.reference00||(c.groupComponent_.kind!=K::unbound&&!(c.nativeGroupRetired_&&c.groupComponent_.kind==K::entity141300)))return false;
            for(std::size_t pi=0;pi<c.count_;++pi){const auto& piece=c.pieces_[pi];
                if(pi<c.sourceSlots38_&&(!piece.nativePreBodyRetired_||!piece.progress_.pieceRetired))return false;
                if(piece.setupZonesBound_||piece.queries_.constructed())return false;
                for(std::size_t j=0;j<piece.componentCount_;++j){const auto* component=piece.componentSlots_[j];
                    if(component&&component->reference00)return false;
                    if(!component&&(j<2u||j>=piece.sceneCount_+1u))return false;
                }
            }
            //22BF50/22BF84/1413E0 for the actual collection; untouched native
            //array backing on a ctor-only cut is merely cancelled, not read.
            if(entity.reference00){
                if(entity.reference00!=1u||entity.kind!=K::collection22BF00||!*storage_->actualObjectCount423F60)return false;
                --entity.reference00;c.ordered38_.reset();
                entity.kind=K::entity141300;--*storage_->actualObjectCount423F60;
                c.retirementResult_={OriginalWreckPrefixRetirementStatus50LA::prefixRetired,0x22bf90u,1u,1u};
            }else{
                if(entity.kind!=K::unbound)return false; //Failed ctor, no source object to destroy.
                c.ordered38_.reset();
            }
            c.retirementAttempted_=true;c.nativeRacerParent_=nullptr;c.nativeRacerPrevious_=nullptr;
        }
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(!storage_->prefixComponents[i]||storage_->prefixComponents[i]->reference00)return false;
        storage_->nativeSourcePrefixRetired=true;return true;
    }
    bool bindNativeConstructorGlobalViews(const OriginalWreckConstructorGlobalViews50LA& globals,
        const void* gravity4431C0)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!globals.current()||!gravity4431C0||
           storage_->nativeGlobals.current()||storage_->prefixDomain.count)return false;
        storage_->nativeGlobals=globals;storage_->nativeGravity4431C0=gravity4431C0;return true;
    }
    bool nativeWreckCount158920(std::uint32_t& value)const noexcept {
        if(!sourceIdentityBound()||!orderedQueriesColdPrepared()||!storage_->nativeSourceMode)return false;
        const auto count=storage_->inventory->authoring().sourceNumberOfParts;
        if(!count||count!=storage_->count)return false;value=count;return true;
    }
    OriginalWreckCollectionStorage50LA* const* nativeArray374Backing()const noexcept {
        return sourceIdentityBound()&&storage_->nativeSourceMode?storage_->ordered374.get():nullptr;
    }
    bool nativeSource374AliasCurrent(OriginalWreckCollectionStorage50LA* const* array,
        std::uint32_t count,std::size_t published)const noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||array!=storage_->ordered374.get()||
           count!=storage_->inventory->authoring().sourceNumberOfParts||count!=storage_->count||published>count||published!=storage_->nativePublished374)return false;
        for(std::size_t i=0;i<published;++i)if(array[i]!=&storage_->collections[i])return false;
        return true;
    }
    bool constructNativeCollection22BF00(std::size_t ordinal,
        std::uint32_t& objectCount,const std::uint32_t& attachmentLock,
        OriginalEntityTypeRegistry50LA& types)noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(!sourceIdentityBound()||!orderedQueriesColdPrepared()||!storage_->nativeSourceMode||
           storage_->nativeFreshCount370||ordinal>=storage_->count||attachmentLock||!storage_->nativeGlobals.current()||
           !storage_->nativeGravity4431C0)return false;
        auto& c=storage_->collections[ordinal];
        if(c.constructorAttempted_||c.retirementAttempted_||storage_->nativePublished374!=ordinal||
           storage_->prefixDomain.count>=storage_->prefixComponents.size())return false;
        if(ordinal){const auto& prior=storage_->collections[ordinal-1u];
            if(storage_->nativeCollectionCursor+1u!=ordinal||!prior.nativeRacerAppended_||
               prior.nativeRacerExclusionFailed_||prior.nativeRacerExclusionCursor_!=prior.sourceCount30_||
               prior.nativeGroupPC_!=0x2129e0u||!nativeRacerChainCurrent(prior)||
               storage_->actualObjectCount423F60!=&objectCount||storage_->actualAttachmentLock423F58!=&attachmentLock||
               storage_->actualTypes383408!=&types)return false;
        }else if(storage_->nativeCollectionCursor||storage_->prefixDomain.count)return false;
        return constructNativeCollectionCore22BF00(ordinal,objectCount,attachmentLock,types);
    }
    //Mutation core shared only after each distinct route's full admission.
    bool constructNativeCollectionCore22BF00(std::size_t ordinal,
        std::uint32_t& objectCount,const std::uint32_t& attachmentLock,
        OriginalEntityTypeRegistry50LA& types)noexcept {
        using K=OriginalWreckComponentKind50LA;
        auto& c=storage_->collections[ordinal];
        storage_->nativeCollectionCursor=ordinal;
        c.constructorAttempted_=true; // Source-attempt custody before counted construction.
        storage_->actualTypes383408=&types;storage_->actualObjectCount423F60=&objectCount;
        storage_->actualAttachmentLock423F58=&attachmentLock;
        auto& domain=storage_->prefixDomain;
        if(!ordinal)domain={storage_->sessionGeneration,storage_->prefixComponents.data(),0};
        const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&c.collectionComponent_));
        const auto initialized=initializeOriginalWreckComponent50LA(c.collectionComponent_,K::collection22BF00,
            storage_->sessionGeneration,key,true,objectCount);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)return false;
        storage_->prefixComponents[domain.count++]=&c.collectionComponent_;
        c.sourceCount30_=0; //22BF20
        c.sourceArray38_=nullptr; //22BF24; prewarmed backing is NOT this alias.
        c.source3C_=nullptr; //22BF28
        c.source34_=nullptr; //22BF2C
        c.nativeConstructorOnly_=true;return true;
    }
    bool publishNativeCollection212A4C(std::size_t ordinal)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||ordinal!=storage_->nativeCollectionCursor||ordinal>=storage_->count||ordinal!=storage_->nativePublished374)return false;
        auto& c=storage_->collections[ordinal];
        if(!c.nativeConstructorOnly_||c.collectionComponent_.reference00!=1u||c.retirementAttempted_)return false;
        storage_->ordered374[ordinal]=&c;++storage_->nativePublished374;return true;
    }
    //Separate source-first route: actual158920 output is the authority. The
    //inventory supplies capacity only; no inventory value becomes source370.
    //No query/world preparation is inferred or permitted by these methods.
    bool freshSourceNoPieceOrQueryAttempts()const noexcept {
        if(!storage_||!storage_->nativeSourceMode||storage_->queryPreparationAttempted||
           storage_->queriesColdPrepared||storage_->queryBorrowsReleased||storage_->preparedQueries||
           storage_->modelScenes.prepared())return false;
        for(std::size_t ci=0;ci<storage_->count;++ci){const auto& c=storage_->collections[ci];
            if(c.sourceSlots38_||c.completedPieceConstructors_||c.nativeGroupAttempted_||
               c.nativeRacerAppendAttempted_||c.nativeLoadDeactivateAttempted_)return false;
            for(std::size_t pi=0;pi<c.count_;++pi){const auto& p=c.pieces_[pi];
                if(p.progress_.constructorAttempted||p.progress_.retirementAttempted||p.setupZonesBound_||
                   p.queries_.prepared()||p.queries_.constructed()||p.modelOwner_.model_.constructed||
                   p.modelMaterials_.started()||p.modelGeometry_||p.modelPackets_.constructed240740()||
                   p.effectArrays_.attempted())return false;
                for(std::size_t j=0;j<p.componentCount_;++j){const auto* e=p.componentSlots_[j];
                    if(e&&(e->reference00||e->kind!=OriginalWreckComponentKind50LA::unbound||
                       e->parent1C||e->previous10||e->next14||e->firstChild18))return false;
                }
            }
        }
        return true;
    }
    bool freshSourceCountCurrent()const noexcept {
        return sourceIdentityBound()&&storage_->nativeSourceMode&&storage_->nativeFreshRacer&&
            storage_->nativeFreshRacer->reference00==1u&&
            storage_->nativeFreshRacer->kind==OriginalWreckComponentKind50LA::racer27E130&&
            storage_->nativeFreshRacer->sessionGeneration==storage_->sessionGeneration&&
            storage_->nativeFreshRacer->objectKey==storage_->racerKey&&
            storage_->nativeFreshCount370&&*storage_->nativeFreshCount370==storage_->nativeFreshCountValue&&
            storage_->nativeFreshCountValue&&storage_->nativeFreshCountValue==storage_->count;
    }
    bool bindFreshSourceCount158920(const OriginalWreckComponent50LA& actualRacer,
        const std::uint32_t& actual370)noexcept {
        if(!sourceIdentityBound()||!freshSourceNoPieceOrQueryAttempts()||storage_->nativeFreshRacer||storage_->nativeFreshCount370||
           storage_->nativeSourcePrefixRetired||storage_->prefixDomain.count||storage_->nativePublished374||
           storage_->nativeCollectionCursor||storage_->actualObjectCount423F60||
           actualRacer.reference00!=1u||actualRacer.kind!=OriginalWreckComponentKind50LA::racer27E130||
           actualRacer.sessionGeneration!=storage_->sessionGeneration||actualRacer.objectKey!=storage_->racerKey||
           !actual370||actual370!=storage_->count)return false;
        for(std::size_t i=0;i<storage_->count;++i){const auto& c=storage_->collections[i];
            if(c.constructorAttempted_||c.retirementAttempted_||c.collectionComponent_.reference00||
               c.collectionComponent_.kind!=OriginalWreckComponentKind50LA::unbound||c.groupComponent_.reference00)
                return false;
        }
        storage_->nativeFreshRacer=&actualRacer;storage_->nativeFreshCount370=&actual370;
        storage_->nativeFreshCountValue=actual370;return true;
    }
    bool freshSource374AliasCurrent(OriginalWreckCollectionStorage50LA* const* array,
        const std::uint32_t& actual370,std::size_t published)const noexcept {
        if(!freshSourceCountCurrent()||&actual370!=storage_->nativeFreshCount370||
           array!=storage_->ordered374.get()||published!=storage_->nativePublished374||
           published>actual370||published>1u)return false;
        for(std::size_t i=0;i<published;++i)if(array[i]!=&storage_->collections[i])return false;
        return true;
    }
    std::size_t freshSourcePublished374()const noexcept {
        return freshSourceCountCurrent()?storage_->nativePublished374:0u;
    }
    bool constructFreshFirstCollection22BF00(std::uint32_t& objectCount,
        const std::uint32_t& attachmentLock,OriginalEntityTypeRegistry50LA& types)noexcept {
        if(!freshSourceCountCurrent()||!freshSourceNoPieceOrQueryAttempts()||storage_->nativeSourcePrefixRetired||
           attachmentLock||storage_->nativeCollectionCursor||storage_->nativePublished374||storage_->prefixDomain.count||
           storage_->actualObjectCount423F60)return false;
        auto& c=storage_->collections[0];
        if(c.constructorAttempted_||c.retirementAttempted_)return false;
        return constructNativeCollectionCore22BF00(0u,objectCount,attachmentLock,types);
    }
    bool publishFreshFirstCollection212A4C()noexcept {
        if(!freshSourceCountCurrent()||!freshSourceNoPieceOrQueryAttempts()||storage_->nativeSourcePrefixRetired)
            return false;
        return publishNativeCollection212A4C(0u);
    }
    bool preflightFreshUnloadedConstructorRetirement()const noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(!storage_)return true; //No native adoption, hence no source constructor.
        if(!freshSourceNoPieceOrQueryAttempts()||storage_->nativeCollectionCursor||
           storage_->nativePublished374>1u)return false;
        if(!storage_->nativeFreshCount370){
            //A failed source binding before actual count adoption cannot have
            //counted constructor effects. Never invent a retirement receipt.
            if(storage_->nativeFreshRacer||storage_->prefixDomain.count||storage_->actualObjectCount423F60||
               storage_->nativePublished374)return false;
            for(std::size_t i=0;i<storage_->count;++i){const auto& c=storage_->collections[i];
                if(c.constructorAttempted_||c.retirementAttempted_||c.collectionComponent_.reference00||
                   c.collectionComponent_.kind!=K::unbound||c.groupComponent_.reference00)return false;
            }
            return true;
        }
        if(!freshSourceCountCurrent())return false;
        if(storage_->nativeSourcePrefixRetired)return true;
        for(std::size_t i=0;i<storage_->count;++i){const auto& c=storage_->collections[i];
            const auto& e=c.collectionComponent_;
            if(c.groupComponent_.reference00||c.groupComponent_.kind!=K::unbound||
               e.parent1C||e.previous10||e.next14||e.firstChild18||c.retirementAttempted_)return false;
            if(i||!c.constructorAttempted_){
                if(c.constructorAttempted_||e.reference00||e.kind!=K::unbound)return false;
                continue;
            }
            if(!storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
               *storage_->actualAttachmentLock423F58)return false;
            if(e.reference00){
                if(e.reference00!=1u||e.kind!=K::collection22BF00||!c.nativeConstructorOnly_||
                   c.sourceCount30_||c.sourceArray38_||c.source34_||c.source3C_||
                   !*storage_->actualObjectCount423F60||storage_->prefixDomain.count!=1u||
                   storage_->prefixDomain.sessionGeneration!=storage_->sessionGeneration||
                   storage_->prefixComponents[0]!=&e||e.sessionGeneration!=storage_->sessionGeneration)return false;
            }else if(e.kind!=K::unbound||storage_->prefixDomain.count||storage_->nativePublished374)return false;
        }
        if(storage_->nativePublished374&&storage_->ordered374[0]!=&storage_->collections[0])return false;
        return true;
    }
    bool retireFreshUnloadedConstructor22BF50()noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(!preflightFreshUnloadedConstructorRetirement())return false;
        if(!storage_||!storage_->nativeFreshCount370)return true;
        if(storage_->nativeSourcePrefixRetired)return true;
        auto& c=storage_->collections[0];auto& e=c.collectionComponent_;
        if(e.reference00){
            --e.reference00; //142590/104970 before22BF50.
            //Actual38 is NULL:22BF7C takes its genuine no-array-free branch.
            //ordered38_ is unreached native reserve, NOT the source38 pointer.
            e.kind=K::entity141300;--*storage_->actualObjectCount423F60;
            c.retirementAttempted_=true;
            c.retirementResult_={OriginalWreckPrefixRetirementStatus50LA::prefixRetired,0x22bf90u,1u,1u};
        }
        //No constructor reference means no source retirement receipt. Keep
        //370/374, publication cursor and all source aliases as history.
        if(c.constructorAttempted_)storage_->nativeSourcePrefixRetired=true;
        return true;
    }
    //Distinct source-first22BFD0 admission. The retained Session owns World;
    //this source40 word is historical, not a second World lifetime authority.
    bool beginFreshCollectionLoad22C02C(const ::ra_cold_aggregate50LA::Owner& world,
        const OriginalWreckConstructorGlobalViews50LA& globals,const void* gravity)noexcept {
        if(!freshSourceCountCurrent()||!freshSourceNoPieceOrQueryAttempts()||
           storage_->nativePublished374!=1u||storage_->nativeCollectionCursor||
           storage_->nativeSourcePrefixRetired||!globals.current()||!gravity)return false;
        auto& c=storage_->collections[0];
        if(c.freshLoadAttempted_||!c.nativeConstructorOnly_||c.retirementAttempted_||
           c.collectionComponent_.reference00!=1u||storage_->ordered374[0]!=&c||
           c.sourceArray38_||c.sourceCount30_||storage_->nativeGlobals.current())return false;
        c.freshLoadAttempted_=true;
        storage_->nativeGlobals=globals;storage_->nativeGravity4431C0=gravity;
        c.sourceWorld40_=&world; //22C02C, before157D00's returned selection.
        return true;
    }
    std::uint32_t* freshCollectionCount30Field158B30()noexcept {
        if(!freshSourceCountCurrent()||storage_->nativeSourcePrefixRetired)return nullptr;
        auto& c=storage_->collections[0];
        if(!c.freshLoadAttempted_||c.retirementAttempted_||c.sourceArray38_||c.sourceSlots38_)return nullptr;
        return &c.sourceCount30_; //Actual Config output, never inventory-produced.
    }
    bool publishFreshCollectionArray22C08C()noexcept {
        if(!freshSourceCountCurrent()||storage_->nativeSourcePrefixRetired)return false;
        auto& c=storage_->collections[0];
        if(!c.freshLoadAttempted_||c.retirementAttempted_||!c.sourceWorld40_||
           c.sourceArray38_||c.sourceSlots38_||!c.sourceCount30_||c.sourceCount30_!=c.count_||
           !c.ordered38_)return false;
        c.sourceArray38_=c.ordered38_.get(); //Native retained allocation, not guest allocator equivalence.
        c.completedPieceConstructors_=0u;c.freshPieceIndexKnown_=true; //22C090 s0=0.
        c.freshPieceByteOffset_=0u; //22C098 s2=0 in jump delay before source30 comparison.
        return true;
    }
    bool freshPieceAdmission(std::size_t pieceIndex=0u)const noexcept {
        if(!freshSourceCountCurrent()||storage_->nativeSourcePrefixRetired||
           storage_->queryPreparationAttempted||storage_->queriesColdPrepared||
           storage_->modelScenes.prepared()||!storage_->nativeGlobals.current())return false;
        const auto& c=storage_->collections[0];
        if(pieceIndex==1u){
            if(!c.freshLoadAttempted_||!c.sourceWorld40_||!*c.sourceWorld40_||!c.nativeConstructorOnly_||
               c.retirementAttempted_||c.count_<2u||!c.freshPieceIndexKnown_||c.completedPieceConstructors_!=1u||
               !c.freshPieceByteOffset_||*c.freshPieceByteOffset_!=4u||
               (c.sourceCount30_&0x80000000u)||c.sourceCount30_<=1u||c.sourceCount30_>c.count_||
               c.sourceSlots38_!=1u||!c.ordered38_||c.sourceArray38_!=c.ordered38_.get()||
               c.ordered38_[0]!=&c.pieces_[0]||c.pieces_[1].progress_.constructorAttempted||
               storage_->nativePublished374!=1u||storage_->ordered374[0]!=&c)return false;
            const auto& prior=c.pieces_[0].progress_;
            return prior.appended22C1B8&&!prior.pieceRetired&&!prior.retirementAttempted&&
                prior.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::nextPieceRequired&&
                prior.prefixResult.sourcePC==0x22c0a0u&&preflightFreshConstructorOnlyRetirement();
        }
        if(pieceIndex)return false;
        return c.freshLoadAttempted_&&c.sourceWorld40_&&*c.sourceWorld40_&&
            c.nativeConstructorOnly_&&!c.retirementAttempted_&&c.collectionComponent_.reference00==1u&&
            storage_->nativePublished374==1u&&storage_->ordered374[0]==&c&&
            c.sourceCount30_==c.count_&&c.count_&&c.ordered38_&&
            c.sourceArray38_==c.ordered38_.get()&&!c.sourceSlots38_&&!c.completedPieceConstructors_&&
            c.freshPieceIndexKnown_&&c.freshPieceByteOffset_&&!*c.freshPieceByteOffset_&&
            !c.pieces_[0].progress_.constructorAttempted;
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshFirstPiece2CB4B0(const std::string& modelPath,
        const std::array<float,4>& position,const std::array<float,4>& look,const std::array<float,4>& up,
        const WaterHeightProvider50JC* const* water334,const WaterHeightGlobals50JC* waterGlobals)noexcept {
        if(!freshPieceAdmission()||!water334||(*water334&&!waterGlobals))return {};
        auto& c=storage_->collections[0];auto& p=c.pieces_[0];
        if(!p.assets_||modelPath!=p.assets_->receipt().sltRequest)return {};
        std::array<std::uint64_t,8> keys{};
        for(std::size_t i=0;i<keys.size();++i)
            keys[i]=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&p.components_[i]));
        OriginalWreckConstructorPrefixBinding50LA b;
        b.sessionGeneration=storage_->sessionGeneration;b.racerKey=storage_->racerKey;
        b.collectionKey=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&c.collectionComponent_));
        b.pieceComponentKeys=keys.data();b.pieceComponentKeyCount=keys.size();
        b.actualObjectCount423F60=storage_->actualObjectCount423F60;
        b.actualAttachmentLock423F58=storage_->actualAttachmentLock423F58;b.actualTypes383408=storage_->actualTypes383408;
        b.actualWaterB0Field=water334;b.actualWaterGlobals=waterGlobals;
        return constructPiecePrefixCore(0,0,b,true,&position,&look,&up);
    }
    bool freshSourcePieceCursor(std::size_t& index,std::uint32_t& count,std::uint32_t& byteOffset)const noexcept {
        if(!freshPieceAdmission(1u))return false;
        const auto& c=storage_->collections[0];
        index=c.completedPieceConstructors_;count=c.sourceCount30_;byteOffset=*c.freshPieceByteOffset_;
        return true; //Actual known s0=1, s2=4 and signed source30>1; no authored index.
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshSecondPiece2CB4B0(const std::string& modelPath,
        const std::array<float,4>& position,const std::array<float,4>& look,const std::array<float,4>& up,
        const WaterHeightProvider50JC* const* water334,const WaterHeightGlobals50JC* waterGlobals)noexcept {
        if(!freshPieceAdmission(1u)||!water334||(*water334&&!waterGlobals))return {};
        auto& c=storage_->collections[0];auto& p=c.pieces_[1];
        if(!p.assets_||modelPath!=p.assets_->receipt().sltRequest)return {};
        std::array<std::uint64_t,8> keys{};
        for(std::size_t i=0;i<keys.size();++i)
            keys[i]=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&p.components_[i]));
        OriginalWreckConstructorPrefixBinding50LA b;
        b.sessionGeneration=storage_->sessionGeneration;b.racerKey=storage_->racerKey;
        b.collectionKey=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&c.collectionComponent_));
        b.pieceComponentKeys=keys.data();b.pieceComponentKeyCount=keys.size();
        b.actualObjectCount423F60=storage_->actualObjectCount423F60;
        b.actualAttachmentLock423F58=storage_->actualAttachmentLock423F58;b.actualTypes383408=storage_->actualTypes383408;
        b.actualWaterB0Field=water334;b.actualWaterGlobals=waterGlobals;
        return constructPiecePrefixCore(0,1,b,true,&position,&look,&up);
    }
    //Read through the reached source cursor, not native total authoring or the
    //constructor-only admission. The returned addresses are existing owners.
    bool freshSecondE0Reservation166018(std::size_t& index,NativeColdReservation& row)const noexcept {
        auto* p=const_cast<OriginalWreckRacerStorage50LA*>(this)->freshRetirementPiece(1u);
        if(!p||storage_->nativeSourcePrefixRetired||p->progress_.e0ConstructorAttempted||
           !p->freshBody_.attempted||!p->freshBody_.filename60||
           p->progress_.prefixResult.status!=OriginalWreckConstructorPrefixStatus50LA::publishedLifetimeRequired||
           p->progress_.prefixResult.sourcePC!=0x166018u||
           !p->sourceModel14C_||*p->sourceModel14C_!=&p->modelOwner_.model_||
           !p->sourceBodyD0_||!p->sourceGravityD4_||!p->assets_)return false;
        const auto& c=storage_->collections[0];
        const auto* root=p->componentSlots_[0];
        if(c.sourceSlots38_!=2u||!c.ordered38_||c.sourceArray38_!=c.ordered38_.get()||
           c.ordered38_[1]!=p||!root||root->parent1C||root->previous10||root->next14||
           !preflightFreshConstructorOnlyRetirement())return false;
        index=c.completedPieceConstructors_;row={&p->queries_,p->assets_};return true;
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshOccurrenceE0To11BBC0(
        const OriginalWreckQueryWorld50LA& world,OriginalEntityTypeRegistry50LA& types,
        std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;const auto body=1u+p.sceneCount_;
        if(pieceIndex==1u){
            std::size_t actualIndex=0;NativeColdReservation row;
            if(!freshSecondE0Reservation166018(actualIndex,row)||actualIndex!=pieceIndex||
               row.queries!=&p.queries_||row.assets!=p.assets_)return {};
        }
        if(!p.freshBody_.attempted||progress.e0ConstructorAttempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::publishedLifetimeRequired||progress.prefixResult.sourcePC!=0x166018u||
           &types!=storage_->actualTypes383408||!storage_->actualObjectCount423F60||
           *storage_->actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max()||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           world.world!=p.collisionWorldB8_||world.lifecycle!=p.collisionLifecycleB8_||
           !p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||
           !p.sourceBodyD0_||*p.sourceBodyD0_!=p.componentSlots_[body]||
           !p.sourceGravityD4_||*p.sourceGravityD4_!=p.componentSlots_[body+1u]||
           !p.freshBody_.filename60||p.sourceE0_||storage_->prefixDomain.count>=storage_->prefixComponents.size())return {};
        const std::array<OriginalWreckComponent50LA*,3> slots{{p.componentSlots_[body+2u],p.componentSlots_[body+5u],p.componentSlots_[body+6u]}};
        if(!p.queries_.reserveFreshE0Backing(p.queryOwner_,world,*p.componentSlots_[1],slots))return {};
        progress.e0ConstructorAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        auto& e0=*slots[0];p.queries_.beginFreshE0Header11B190();
        const auto init=initializeOriginalWreckComponent50LA(e0,OriginalWreckComponentKind50LA::collision11B190,
            storage_->sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e0)),
            true,*storage_->actualObjectCount423F60);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::components,init.sourcePC);
        storage_->prefixComponents[storage_->prefixDomain.count++]=&e0;
        const auto result=p.queries_.continueFreshE0Counted11B210(e0,types,p.sourceE0_);
        if(result.status!=OriginalWreckQueryStatus50LA::complete)return finish(S::queryConstructionFailed,result.sourcePC);
        return finish(S::queryPublicationRequired,0x166074u); //Real request/cache11B920 still unjoined.
    }
    OriginalWreckConstructorPrefixResult50LA loadFreshOccurrenceE0NativeTransport166074(
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        const std::string& actualDirectory36B0F0,std::array<std::uint8_t,256>& actualName3E0730,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic,
        std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !root||root->parent1C||root->previous10||root->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.e0NativeLoadAttempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::queryPublicationRequired||progress.prefixResult.sourcePC!=0x166074u||
           !p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||!p.sourceE0_||
           *p.sourceE0_!=p.componentSlots_[3u+p.sceneCount_]||!p.freshBody_.filename60)return {};
        progress.e0NativeLoadAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        try {
            const auto& scratch=*p.freshBody_.filename60;
            const auto* end=static_cast<const char*>(std::memchr(scratch.data(),0,scratch.size()));
            if(!end)return finish(S::stopped,0x166074u);
            //Use actual retained309230/rewrite output, not authored filename.
            const std::string request(scratch.data(),static_cast<std::size_t>(end-scratch.data()));
            const auto result=p.queries_.loadFreshE0Type2NativeTransport(p.modelOwner_.model_,request,
                resolver,actualDirectory36B0F0,actualName3E0730,diagnostic);
            if(result.status!=OriginalWreckQueryStatus50LA::complete)
                return finish(S::queryConstructionFailed,result.sourcePC?result.sourcePC:0x166074u);
            return finish(S::e0PreparationRequired,0x16607cu);
        }catch(...){return finish(S::allocation,0x166074u);}
    }
    OriginalWreckConstructorPrefixResult50LA readStopAppendFreshSplash19DDC8(
        OriginalWreckSplashOwner50LA& owner,const std::vector<std::uint8_t>& actualEmt,
        assets::OriginalUiPageDiagnostic50KG& diagnostic,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];const auto& a=p.effectArrays_;
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !root||root->parent1C||root->previous10||root->next14||
               !a.attempted_||!p.effects_.count174||(p.effects_.count174&0x80000000u)||
               a.pairsStopped_!=p.effects_.count174||a.smokeWritten_!=p.effects_.count174||
               a.fireWritten_!=p.effects_.count174||a.boundary_!=0x2cb7e0u||
               !owner.freshConstructorComplete_||p.splash16C_!=&owner.component_||
               !p.freshDustOwner_||!p.freshDustOwner_->appended_||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.retirementAttempted||p.freshSplashOwner_!=&owner||
           !p.freshSplashAttempted_||p.splash16C_!=&owner.component_||!p.freshDustOwner_||
           p.dust168_!=&p.freshDustOwner_->component_||progress.prefixResult.status!=S::stopped||
           progress.prefixResult.sourcePC!=0x19ddc8u||!p.effectInputsStillBound()||
           !p.effectArrays_.preflightFreshArrays(nullptr,nullptr,p.dust168_,p.splash16C_)||
           !owner.preflightFreshUnappended()||!p.freshDustOwner_->preflightFreshUnappended(true,&owner.component_)||
           owner.binding_.particleParent!=p.effectParent_||owner.context_!=p.effectResource_)return {};
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(!owner.readFresh19DDC8(actualEmt,diagnostic))return finish(S::effectSplashFailed,owner.sourceBoundary());
        if(!owner.stopFresh15AD00())return finish(S::effectSplashFailed,owner.sourceBoundary());
        if(!owner.appendFresh141960())return finish(S::effectSplashFailed,owner.sourceBoundary());
        return finish(S::stopped,0x2cb8c4u); //Exact append return, before enclosing epilogue/caller.
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshSplash2CB854(
        OriginalWreckSplashOwner50LA& owner,const OriginalWreckSplashBinding50LA& binding,
        const std::array<std::uint32_t,4>* actual433F50,const std::string* actualDirectory36B530,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !root||root->parent1C||root->previous10||root->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.retirementAttempted||p.freshSplashAttempted_||p.splash16C_||
           progress.prefixResult.status!=S::stopped||progress.prefixResult.sourcePC!=0x2cb854u||
           !p.freshDustOwner_||p.dust168_!=&p.freshDustOwner_->component_||
           !p.freshDustOwner_->appended_||!p.freshDustOwner_->preflightFreshUnappended(true)||
           !p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_||
           !p.effectArrays_.preflightFreshArrays(nullptr,nullptr,p.dust168_)||
           binding.particleParent!=p.effectParent_||binding.domain!=p.freshDustOwner_->binding_.domain||
           binding.domainStorage!=p.freshDustOwner_->binding_.domainStorage||
           binding.domainCapacity!=p.freshDustOwner_->binding_.domainCapacity||!sameExternalEffectForest(binding)||
           binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58)return {};
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]->objectKey==binding.componentKey)return {};
        p.freshSplashAttempted_=true;p.freshSplashOwner_=&owner;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(!owner.constructFresh19DBE0(binding,actual433F50))return finish(S::effectSplashFailed,
            owner.sourceBoundary()?owner.sourceBoundary():0x2cb858u);
        p.splash16C_=&owner.component_; //Actual2CB874 AFTER constructor, before setup.
        const bool done=owner.setupFresh19DCD0(*p.effectResource_,actualDirectory36B530);
        return finish(done?S::stopped:S::effectSplashFailed,owner.sourceBoundary());
    }
    OriginalWreckConstructorPrefixResult50LA readStopAppendFreshDust19A7D0(
        OriginalWreckDustOwner50LA& owner,const std::vector<std::uint8_t>& actualEmt,
        assets::OriginalUiPageDiagnostic50KG& diagnostic,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& arrays=p.effectArrays_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !root||root->parent1C||root->previous10||root->next14||
               !arrays.attempted_||!p.effects_.count174||(p.effects_.count174&0x80000000u)||
               arrays.pairsStopped_!=p.effects_.count174||arrays.smokeWritten_!=p.effects_.count174||
               arrays.fireWritten_!=p.effects_.count174||arrays.boundary_!=0x2cb7e0u||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.retirementAttempted||p.freshDustOwner_!=&owner||
           !p.freshDustAttempted_||p.dust168_!=&owner.component_||progress.prefixResult.status!=S::stopped||
           progress.prefixResult.sourcePC!=0x19a7d0u||!p.effectInputsStillBound()||
           !arrays.preflightFreshArrays(nullptr,nullptr,&owner.component_)||!owner.preflightFreshUnappended()||
           owner.binding_.particleParent!=p.effectParent_||owner.context_!=p.effectResource_)return {};
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(!owner.readFresh19A7D0(actualEmt,diagnostic))return finish(S::effectDustFailed,owner.sourceBoundary());
        if(!owner.stopFresh15AD00())return finish(S::effectDustFailed,owner.sourceBoundary());
        p.effects_.dustActive178=0; //Actual2CB83C AFTER Dust Stop84.
        p.effects_.dustTime17C=0.f; //Actual2CB840 BEFORE append141960.
        if(!owner.appendFresh141960())return finish(S::effectDustFailed,owner.sourceBoundary());
        return finish(S::stopped,0x2cb854u); //Exact call return, before next size/allocation.
    }
    OriginalWreckConstructorPrefixResult50LA readFreshSecondInitialChild19A7D0(
        const std::vector<std::uint8_t>& actualEmt,assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(1u);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;const auto& collection=storage_->collections[0];
        const auto* root=p.componentSlots_[0];
        if(!sourceIdentityBound()||storage_->nativeSourcePrefixRetired||collection.sourceSlots38_!=2u||!collection.ordered38_||
           collection.sourceArray38_!=collection.ordered38_.get()||collection.ordered38_[1]!=&p||
           !root||root->parent1C||root->previous10||root->next14||progress.retirementAttempted||
           progress.prefixResult.status!=S::stopped||progress.prefixResult.sourcePC!=0x19a7d0u||
           !p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_||
           bool(p.freshSmokeOwner_)==bool(p.freshDustOwner_)||
           !preflightFreshConstructorOnlyRetirement())return {};
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        if(p.freshSmokeOwner_){
            auto& owner=*p.freshSmokeOwner_;const auto& arrays=p.effectArrays_;
            if(!p.freshSmokeAttempted_||p.freshSmokeAttempts_!=1u||arrays.smokeWritten_!=1u||
               !arrays.smoke160_||arrays.smoke160_[0]!=&owner.component_||
               !arrays.preflightFreshArrays(&owner.component_)||!owner.preflightFreshUnappended()||
               owner.context_!=p.effectResource_||owner.binding_.particleParent!=p.effectParent_)return {};
            if(!owner.readFresh19A7D0(actualEmt,diagnostic))return finish(S::effectSmokeFailed,owner.sourceBoundary());
        }else{
            auto& owner=*p.freshDustOwner_;
            if(!p.freshDustAttempted_||p.dust168_!=&owner.component_||p.effectArrays_.attempted_||
               !p.effectArrays_.preflightFreshArrays(nullptr,nullptr,&owner.component_)||
               !owner.preflightFreshUnappended()||owner.context_!=p.effectResource_||
               owner.binding_.particleParent!=p.effectParent_)return {};
            if(!owner.readFresh19A7D0(actualEmt,diagnostic))return finish(S::effectDustFailed,owner.sourceBoundary());
        }
        return finish(S::stopped,0x19a7f8u); //Before caller Stop/append and178/17C stores.
    }
    OriginalWreckConstructorPrefixResult50LA readFreshSecondSuccessorChild(
        const std::vector<std::uint8_t>& actualEmt,assets::OriginalUiPageDiagnostic50KG& diagnostic)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(1u);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;const auto& collection=storage_->collections[0];
        const auto* root=p.componentSlots_[0];
        if(!sourceIdentityBound()||storage_->nativeSourcePrefixRetired||collection.sourceSlots38_!=2u||!collection.ordered38_||
           collection.sourceArray38_!=collection.ordered38_.get()||collection.ordered38_[1]!=&p||
           !root||root->parent1C||root->previous10||root->next14||progress.retirementAttempted||
           progress.prefixResult.status!=S::stopped||
           !p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_||
           bool(p.freshFireOwner_)==bool(p.freshSplashOwner_)||
           !preflightFreshConstructorOnlyRetirement())return {};
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        if(p.freshFireOwner_){
            auto& owner=*p.freshFireOwner_;const auto& arrays=p.effectArrays_;
            if(progress.prefixResult.sourcePC!=0x15e024u||!p.freshFireAttempted_||p.freshFireAttempts_!=1u||
               !p.freshSmokeOwner_||!p.freshSmokeOwner_->appended_||arrays.fireWritten_!=1u||
               !arrays.fire164_||arrays.fire164_[0]!=&owner.component_||
               !arrays.preflightFreshArrays(&p.freshSmokeOwner_->component_,&owner.component_)||
               !owner.freshConstructorComplete_||!owner.preflightFreshUnappended()||
               !p.freshSmokeOwner_->preflightFreshUnappended(true,&owner.component_)||
               owner.context_!=p.effectResource_||owner.binding_.particleParent!=p.effectParent_||
               owner.sourceBoundary()!=0x15e024u)return {};
            if(!owner.readFresh15E024(actualEmt,diagnostic))return finish(S::effectFireFailed,owner.sourceBoundary());
            return finish(S::stopped,0x15e0b0u); //Before141960 append, either Stop or loop cursor.
        }else{
            auto& owner=*p.freshSplashOwner_;
            if(progress.prefixResult.sourcePC!=0x19ddc8u||!p.freshSplashAttempted_||
               !p.freshDustOwner_||!p.freshDustOwner_->appended_||p.splash16C_!=&owner.component_||
               p.effectArrays_.attempted_||!p.effectArrays_.preflightFreshArrays(nullptr,nullptr,p.dust168_,p.splash16C_)||
               !owner.freshConstructorComplete_||!owner.preflightFreshUnappended()||
               !p.freshDustOwner_->preflightFreshUnappended(true,&owner.component_)||
               owner.context_!=p.effectResource_||owner.binding_.particleParent!=p.effectParent_||
               owner.sourceBoundary()!=0x19ddc8u)return {};
            if(!owner.readFresh19DDC8(actualEmt,diagnostic))return finish(S::effectSplashFailed,owner.sourceBoundary());
            return finish(S::stopped,0x19de7cu); //Before Stop84 or append; no carried-size reset.
        }
    }
    OriginalWreckConstructorPrefixResult50LA completeFreshSecondSuccessorChild()noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(1u);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;const auto& collection=storage_->collections[0];
        const auto* root=p.componentSlots_[0];
        if(!sourceIdentityBound()||storage_->nativeSourcePrefixRetired||collection.sourceSlots38_!=2u||!collection.ordered38_||
           collection.sourceArray38_!=collection.ordered38_.get()||collection.ordered38_[1]!=&p||
           !root||root->parent1C||root->previous10||root->next14||progress.retirementAttempted||
           progress.prefixResult.status!=S::stopped||
           !p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_||
           bool(p.freshFireOwner_)==bool(p.freshSplashOwner_)||
           !preflightFreshConstructorOnlyRetirement())return {};
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        if(p.freshFireOwner_){
            auto& owner=*p.freshFireOwner_;const auto& arrays=p.effectArrays_;
            if(progress.prefixResult.sourcePC!=0x15e0b0u||!p.freshFireAttempted_||p.freshFireAttempts_!=1u||
               !p.freshSmokeOwner_||!p.freshSmokeOwner_->appended_||arrays.fireWritten_!=1u||
               !arrays.fire164_||arrays.fire164_[0]!=&owner.component_||
               !arrays.preflightFreshArrays(&p.freshSmokeOwner_->component_,&owner.component_)||
               !owner.freshConstructorComplete_||!owner.freshReaderComplete_||!owner.preflightFreshUnappended()||
               !p.freshSmokeOwner_->preflightFreshUnappended(true,&owner.component_)||
               owner.context_!=p.effectResource_||owner.binding_.particleParent!=p.effectParent_||
               owner.sourceBoundary()!=0x15e0b0u)return {};
            if(!owner.appendFresh141960())return finish(S::effectFireFailed,owner.sourceBoundary());
            return finish(S::stopped,0x2cb778u); //Before either pair Stop84 or loop cursor.
        }else{
            auto& owner=*p.freshSplashOwner_;
            if(progress.prefixResult.sourcePC!=0x19de7cu||!p.freshSplashAttempted_||
               !p.freshDustOwner_||!p.freshDustOwner_->appended_||p.splash16C_!=&owner.component_||
               p.effectArrays_.attempted_||!p.effectArrays_.preflightFreshArrays(nullptr,nullptr,p.dust168_,p.splash16C_)||
               !owner.freshConstructorComplete_||!owner.freshReaderComplete_||!owner.preflightFreshUnappended()||
               !p.freshDustOwner_->preflightFreshUnappended(true,&owner.component_)||
               owner.context_!=p.effectResource_||owner.binding_.particleParent!=p.effectParent_||
               owner.sourceBoundary()!=0x19de7cu)return {};
            if(!owner.stopFresh15AD00())return finish(S::effectSplashFailed,owner.sourceBoundary());
            if(!owner.appendFresh141960())return finish(S::effectSplashFailed,owner.sourceBoundary());
            return finish(S::stopped,0x2cb8c4u); //Before enclosing return/collection append.
        }
    }
    OriginalWreckConstructorPrefixResult50LA completeFreshSecondInitialChild19A7F8()noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(1u);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;const auto& collection=storage_->collections[0];
        const auto* root=p.componentSlots_[0];
        if(!sourceIdentityBound()||storage_->nativeSourcePrefixRetired||collection.sourceSlots38_!=2u||!collection.ordered38_||
           collection.sourceArray38_!=collection.ordered38_.get()||collection.ordered38_[1]!=&p||
           !root||root->parent1C||root->previous10||root->next14||progress.retirementAttempted||
           progress.prefixResult.status!=S::stopped||progress.prefixResult.sourcePC!=0x19a7f8u||
           !p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_||
           bool(p.freshSmokeOwner_)==bool(p.freshDustOwner_)||
           !preflightFreshConstructorOnlyRetirement())return {};
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        if(p.freshSmokeOwner_){
            auto& owner=*p.freshSmokeOwner_;const auto& arrays=p.effectArrays_;
            if(!p.freshSmokeAttempted_||p.freshSmokeAttempts_!=1u||arrays.smokeWritten_!=1u||
               !arrays.smoke160_||arrays.smoke160_[0]!=&owner.component_||
               !arrays.preflightFreshArrays(&owner.component_)||!owner.preflightFreshUnappended()||
               owner.context_!=p.effectResource_||owner.binding_.particleParent!=p.effectParent_||
               !owner.freshReaderComplete_||owner.sourceBoundary()!=0x19a7f8u)return {};
            if(!owner.appendFresh141960())return finish(S::effectSmokeFailed,owner.sourceBoundary());
            return finish(S::effectFireRequired,0x2cb720u); //No second Fire allocation.
        }else{
            auto& owner=*p.freshDustOwner_;
            if(!p.freshDustAttempted_||p.dust168_!=&owner.component_||p.effectArrays_.attempted_||
               !p.effectArrays_.preflightFreshArrays(nullptr,nullptr,&owner.component_)||
               !owner.preflightFreshUnappended()||owner.context_!=p.effectResource_||
               owner.binding_.particleParent!=p.effectParent_||
               !owner.freshReaderComplete_||owner.sourceBoundary()!=0x19a7f8u)return {};
            if(!owner.stopFresh15AD00())return finish(S::effectDustFailed,owner.sourceBoundary());
            p.effects_.dustActive178=0; //2CB83C: SAME second piece, after Stop84.
            p.effects_.dustTime17C=0.f; //2CB840: before141960 append.
            if(!owner.appendFresh141960())return finish(S::effectDustFailed,owner.sourceBoundary());
            return finish(S::stopped,0x2cb854u); //No second Splash allocation.
        }
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshDust2CB7E0(
        OriginalWreckDustOwner50LA& owner,const OriginalWreckDustBinding50LA& binding,
        const std::string* actualDirectory36B530,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& arrays=p.effectArrays_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];
            const auto& first=c.pieces_[0];
            if(!first.freshSplashOwner_||!first.freshSplashOwner_->appended_||
               !first.effectInputsStillBound()||
               p.effectInputs_.resourceContextField!=first.effectInputs_.resourceContextField||
               p.effectInputs_.particleParentField!=first.effectInputs_.particleParentField||
               binding.domain!=first.freshSplashOwner_->binding_.domain||
               binding.domainStorage!=first.freshSplashOwner_->binding_.domainStorage||
               binding.domainCapacity!=first.freshSplashOwner_->binding_.domainCapacity||
               storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !root||root->parent1C||root->previous10||root->next14||
               p.freshDustAttempted_||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||p.freshDustAttempted_||p.dust168_||progress.retirementAttempted||
           progress.prefixResult.status!=S::effectDustRequired||!p.freshEffectCountWritten_||
           !p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_||
           !arrays.preflightFreshArrays()||binding.particleParent!=p.effectParent_||
           binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
           binding.domain==&storage_->prefixDomain||!sameExternalEffectForest(binding))return {};
        const bool positive=p.effects_.count174&&!(p.effects_.count174&0x80000000u);
        if(positive){
            if(progress.prefixResult.sourcePC!=0x2cb7e0u||!arrays.attempted_||
               arrays.count_!=p.effects_.count174||arrays.pairsStopped_!=arrays.count_||
               arrays.smokeWritten_!=arrays.count_||arrays.fireWritten_!=arrays.count_||
               !p.freshFireOwner_||!p.freshFireOwner_->preflightFreshUnappended(true)||
               binding.domain!=arrays.externalDomain_||binding.domainStorage!=arrays.externalStorage_||
               binding.domainCapacity!=arrays.externalCapacity_)return {};
        }else if(progress.prefixResult.sourcePC!=0x2cb7e4u||arrays.attempted_||
            arrays.smokeWritten_||arrays.fireWritten_||p.freshSmokeOwner_||p.freshFireOwner_)return {};
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]->objectKey==binding.componentKey)return {};
        p.freshDustAttempted_=true;p.freshDustOwner_=&owner;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(!owner.constructFresh19A5E0(binding))return finish(S::effectDustFailed,owner.sourceBoundary()?owner.sourceBoundary():0x2cb7e4u);
        p.dust168_=&owner.component_; //Actual2CB800, before19A720.
        const bool done=owner.setupFresh19A720(*p.effectResource_,actualDirectory36B530);
        return finish(done?S::stopped:S::effectDustFailed,owner.sourceBoundary());
    }
    bool freshEffectCursor(std::size_t& index,std::uint32_t& count,std::uint32_t& pc,std::size_t pieceIndex=0u)const noexcept {
        //Read-through genuine loop state only, never an independent caller count.
        const auto* p=const_cast<OriginalWreckRacerStorage50LA*>(this)->freshRetirementPiece(pieceIndex);
        if(!p||!sourceIdentityBound()||p->progress_.retirementAttempted||!p->freshEffectCountWritten_||
           !p->effectInputsStillBound()||!p->effectArrays_.preflightFreshArrays()||
           !p->effectArrays_.freshPairByteOffset_||p->effectArrays_.count_!=p->effects_.count174)return false;
        index=p->effectArrays_.pairsStopped_;count=p->effects_.count174;pc=p->progress_.prefixResult.sourcePC;
        return true;
    }
    template<class Owner>
    static bool freshEffectOwnerCurrent(const OriginalWreckPieceStorage50LA& p,const Owner& owner,
        std::size_t attempts,bool fire)noexcept {
        const auto& a=p.effectArrays_;const auto& c=owner.component_;
        if(!attempts)return false;
        if(!owner.initialized_)return owner.preflightFreshUnappended();
        if(!a.externalFirstSlot_||!a.externalDomain_||!a.externalStorage_)return false;
        const auto slot=*a.externalFirstSlot_+2u*(attempts-1u)+(fire?1u:0u);
        if(slot>=a.externalDomain_->count||a.externalStorage_[slot]!=&c)return false;
        const OriginalWreckComponent50LA* previous=nullptr;
        const OriginalWreckComponent50LA* next=nullptr;
        if(owner.appended_){
            //First member retains its actual source previous link; the fresh wrapper
            //proves the initially empty7FC child list. Later predecessors are exact effect slots.
            previous=slot>*a.externalFirstSlot_?a.externalStorage_[slot-1u]:c.previous10;
            if(slot+1u<a.externalDomain_->count){
                const auto* candidate=a.externalStorage_[slot+1u];
                if(candidate&&candidate->parent1C)next=candidate;
            }
        }
        return owner.preflightFreshChainMember(slot,previous,next);
    }
    OriginalWreckConstructorPrefixResult50LA stopFreshFirstPair2CB778(std::size_t pieceIndex=0u)noexcept {
        auto* p=freshRetirementPiece(pieceIndex);if(!p||p->effectArrays_.pairsStopped_)return {};
        return stopFreshPair2CB778(pieceIndex);
    }
    OriginalWreckConstructorPrefixResult50LA stopFreshPair2CB778(std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& arrays=p.effectArrays_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !arrays.freshPairByteOffset_||*arrays.freshPairByteOffset_!=4u*arrays.pairsStopped_||
               !root||root->parent1C||root->previous10||root->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.retirementAttempted||
           progress.prefixResult.status!=S::stopped||progress.prefixResult.sourcePC!=0x2cb778u||
           !p.freshSmokeOwner_||!p.freshFireOwner_||!p.freshSmokeAttempted_||!p.freshFireAttempted_||
           !p.effectInputsStillBound()||!p.freshEffectCountWritten_||
           !arrays.freshPairByteOffset_||*arrays.freshPairByteOffset_!=4u*arrays.pairsStopped_||
           arrays.pairsStopped_>=arrays.count_||p.freshSmokeAttempts_!=arrays.pairsStopped_+1u||
           p.freshFireAttempts_!=arrays.pairsStopped_+1u||
           !p.effects_.count174||p.effects_.count174>512u||arrays.count_!=p.effects_.count174||
           arrays.anchorsWritten_!=arrays.count_)return {};
        auto& smoke=*p.freshSmokeOwner_;auto& fire=*p.freshFireOwner_;
        //Exact source160/164 pointees and fully appended same-parent tail.
        //Slot84 identities:3CEC84=15AD00 and3CDB94=15AD00 (not a generic effect).
        if(!arrays.preflightFreshArrays(&smoke.component_,&fire.component_)||
           !smoke.freshReaderComplete_||!fire.freshReaderComplete_||!smoke.appended_||!fire.appended_||
           smoke.boundary_!=0x2cb720u||fire.boundary_!=0x2cb778u||
           smoke.controls_.profile!=EffectProfile50BE::Particle19A5E0||
           fire.controls_.profile!=EffectProfile50BE::Fire15DE00||
           smoke.context_!=p.effectResource_||fire.context_!=p.effectResource_||
           smoke.binding_.particleParent!=p.effectParent_||fire.binding_.particleParent!=p.effectParent_||
           smoke.binding_.domain!=arrays.externalDomain_||fire.binding_.domain!=arrays.externalDomain_||
           smoke.binding_.domainStorage!=arrays.externalStorage_||fire.binding_.domainStorage!=arrays.externalStorage_||
           !smoke.preflightFreshUnappended(true,&fire.component_)||!fire.preflightFreshUnappended(true))return {};
        //Whole preflight above; neither Stop calls allocation, manager, registry or parser.
        //Each preserves unknown upper1D4 bits and raw vectorW, stores bit1 thenbit2,
        //full30->40, then19C=0 at15AD40. Source order is Smoke thenFire.
        stopEffect50BC(smoke.controls_.state);smoke.boundary_=0x2cb798u;
        stopEffect50BC(fire.controls_.state);fire.boundary_=0x2cb7c0u;
        *arrays.freshPairByteOffset_+=4u; //2CB7C8; both pointers genuinely nonNULL.
        ++arrays.pairsStopped_; //2CB7CC: actual s0; not source174 and not written-count.
        //Source SLT2CB7D4 is signed. Native capacity admission above proves174 positive.
        const bool more=static_cast<std::int32_t>(arrays.pairsStopped_)<static_cast<std::int32_t>(p.effects_.count174);
        arrays.boundary_=more?0x2cb6ccu:0x2cb7e0u;
        progress.prefixResult={more?S::effectNextPairRequired:S::effectDustRequired,
            arrays.boundary_,storage_->prefixDomain.count,true};return progress.prefixResult;
    }
    OriginalWreckConstructorPrefixResult50LA readAppendFreshFire15E024(
        OriginalWreckFireOwner50LA& owner,const std::vector<std::uint8_t>& actualEmt,
        assets::OriginalUiPageDiagnostic50KG& diagnostic,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& arrays=p.effectArrays_;
        if(pieceIndex==1u&&!preflightFreshConstructorOnlyRetirement())return {};
        if(!sourceIdentityBound()||progress.retirementAttempted||p.freshFireOwner_!=&owner||
           !p.freshFireAttempted_||!p.freshSmokeOwner_||progress.prefixResult.status!=S::stopped||
           progress.prefixResult.sourcePC!=0x15e024u||!p.effectInputsStillBound()||
           !arrays.preflightFreshArrays(&p.freshSmokeOwner_->component_,&owner.component_)||
           !owner.preflightFreshUnappended()||!p.freshSmokeOwner_->preflightFreshUnappended(true,&owner.component_)||
           owner.binding_.particleParent!=p.effectParent_||owner.context_!=p.effectResource_)return {};
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(!owner.readFresh15E024(actualEmt,diagnostic))return finish(S::effectFireFailed,owner.sourceBoundary());
        if(!owner.appendFresh141960())return finish(S::effectFireFailed,owner.sourceBoundary());
        return finish(S::stopped,0x2cb778u); //Before either actual Stop84 call.
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshFire2CB720(
        OriginalWreckFireOwner50LA& owner,const OriginalWreckFireBinding50LA& binding,
        const std::string* actualDirectory36B530,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& arrays=p.effectArrays_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !root||root->parent1C||root->previous10||root->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||p.freshFireAttempts_!=arrays.pairsStopped_||progress.retirementAttempted||
           progress.prefixResult.status!=S::effectFireRequired||progress.prefixResult.sourcePC!=0x2cb720u||
           !p.freshSmokeOwner_||!p.freshSmokeOwner_->appended_||
           !p.freshSmokeOwner_->preflightFreshUnappended(true)||!p.effectInputsStillBound()||
           !p.effectResource_||!p.effectParent_||!arrays.preflightFreshArrays(&p.freshSmokeOwner_->component_)||
           !arrays.fire164_||!p.effects_.count174||arrays.pairsStopped_>=arrays.count_||
           arrays.fireWritten_!=arrays.pairsStopped_||arrays.smokeWritten_!=arrays.pairsStopped_+1u||
           p.freshSmokeAttempts_!=arrays.pairsStopped_+1u||binding.particleParent!=p.effectParent_||
           binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
           binding.domain!=arrays.externalDomain_||binding.domainStorage!=arrays.externalStorage_||
           binding.domainCapacity!=arrays.externalCapacity_||!sameExternalEffectForest(binding))return {};
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]->objectKey==binding.componentKey)return {};
        p.freshFireAttempted_=true;++p.freshFireAttempts_;p.freshFireOwner_=&owner;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(!owner.constructFresh15DE00(binding))return finish(S::effectFireFailed,owner.sourceBoundary()?owner.sourceBoundary():0x2cb720u);
        arrays.fire164_[arrays.pairsStopped_]=&owner.component_;++arrays.fireWritten_; //Actual2CB758, before15DEF0.
        const bool done=owner.setupFresh15DEF0(*p.effectResource_,actualDirectory36B530);
        return finish(done?S::stopped:S::effectFireFailed,owner.sourceBoundary());
    }
    OriginalWreckConstructorPrefixResult50LA readAppendFreshSmoke19A7D0(
        OriginalWreckSmokeOwner50LA& owner,const std::vector<std::uint8_t>& actualEmt,
        assets::OriginalUiPageDiagnostic50KG& diagnostic,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& arrays=p.effectArrays_;
        if(pieceIndex==1u&&!preflightFreshConstructorOnlyRetirement())return {};
        if(!sourceIdentityBound()||progress.retirementAttempted||p.freshSmokeOwner_!=&owner||
           !p.freshSmokeAttempted_||progress.prefixResult.status!=S::stopped||
           progress.prefixResult.sourcePC!=0x19a7d0u||!p.effectInputsStillBound()||
           !arrays.preflightFreshArrays(&owner.component_)||!owner.preflightFreshUnappended()||
           owner.binding_.particleParent!=p.effectParent_||owner.context_!=p.effectResource_)return {};
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(!owner.readFresh19A7D0(actualEmt,diagnostic))return finish(S::effectSmokeFailed,owner.sourceBoundary());
        if(!owner.appendFresh141960())return finish(S::effectSmokeFailed,owner.sourceBoundary());
        return finish(S::effectFireRequired,0x2cb720u);
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshSmoke2CB6CC(
        OriginalWreckSmokeOwner50LA& owner,const OriginalWreckSmokeBinding50LA& binding,
        const std::string* actualDirectory36B530,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& arrays=p.effectArrays_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];
            const auto& first=c.pieces_[0];
            if(!first.freshSplashOwner_||!first.freshSplashOwner_->appended_||
               !first.effectInputsStillBound()||
               p.effectInputs_.resourceContextField!=first.effectInputs_.resourceContextField||
               p.effectInputs_.particleParentField!=first.effectInputs_.particleParentField||
               binding.domain!=first.freshSplashOwner_->binding_.domain||
               binding.domainStorage!=first.freshSplashOwner_->binding_.domainStorage||
               binding.domainCapacity!=first.freshSplashOwner_->binding_.domainCapacity||
               storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !root||root->parent1C||root->previous10||root->next14||
               p.freshDustAttempted_||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||p.freshSmokeAttempts_!=arrays.pairsStopped_||progress.retirementAttempted||
           (progress.prefixResult.status!=(arrays.pairsStopped_?S::effectNextPairRequired:S::effectChildrenRequired))||progress.prefixResult.sourcePC!=0x2cb6ccu||
           !p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_||
           !arrays.preflightFreshArrays()||!arrays.attempted_||arrays.pairsStopped_>=arrays.count_||
           arrays.smokeWritten_!=arrays.pairsStopped_||arrays.fireWritten_!=arrays.pairsStopped_||
           p.freshFireAttempts_!=arrays.pairsStopped_||arrays.anchorsWritten_!=p.effects_.count174||
           !p.effects_.count174||binding.particleParent!=p.effectParent_||
           binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
           binding.domain==&storage_->prefixDomain||!sameExternalEffectForest(binding))return {};
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]->objectKey==binding.componentKey)return {};
        if(arrays.pairsStopped_&&(binding.domain!=arrays.externalDomain_||binding.domainStorage!=arrays.externalStorage_||
           binding.domainCapacity!=arrays.externalCapacity_||!arrays.externalFirstSlot_||
           binding.domain->count!=*arrays.externalFirstSlot_+2u*arrays.pairsStopped_))return {};
        const auto firstSlot=binding.domain->count;
        p.freshSmokeAttempted_=true;++p.freshSmokeAttempts_;p.freshSmokeOwner_=&owner;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(!owner.constructFresh19A5E0(binding))return finish(S::effectSmokeFailed,owner.sourceBoundary()?owner.sourceBoundary():0x2cb6ccu);
        if(!arrays.externalFirstSlot_)arrays.externalFirstSlot_=firstSlot;
        arrays.externalDomain_=binding.domain;arrays.externalStorage_=binding.domainStorage;arrays.externalCapacity_=binding.domainCapacity;
        arrays.smoke160_[arrays.pairsStopped_]=&owner.component_;++arrays.smokeWritten_; //Actual2CB700, BEFORE19A720.
        const bool done=owner.setupFresh19A720(*p.effectResource_,actualDirectory36B530);
        return finish(done?S::stopped:S::effectSmokeFailed,owner.sourceBoundary());
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshEffectArrays2CB628(std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* selectedRoot=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !selectedRoot||selectedRoot->parent1C||selectedRoot->previous10||selectedRoot->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.retirementAttempted||!p.freshEffectCountWritten_||
           !p.freshEffectInputsAttempted_||!p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_||
           progress.prefixResult.status!=S::effectConstructorRequired||progress.prefixResult.sourcePC!=0x2cb628u||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           !p.effectArrays_.preflightFreshArrays()||p.effectArrays_.attempted())return {};
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        const auto count=p.effects_.count174;
        //Signed BLEZ without implementation-defined unsigned-to-signed cast.
        if(!count||(count&0x80000000u))return finish(S::effectDustRequired,0x2cb7e4u);
        if(count>512u)return finish(S::effectArraysFailed,0x2cb634u); //Native capacity, actual174 unchanged.
        const auto graph=p.assets_->graph();
        if(!p.freshBody_.model288||!*p.freshBody_.model288||!p.sourceModel14C_||
           *p.freshBody_.model288!=*p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||
           !p.sourceBodyD0_||*p.sourceBodyD0_!=p.componentSlots_[1u+p.sceneCount_]||
           (*p.sourceBodyD0_)->reference00!=1u||(*p.sourceBodyD0_)->kind!=OriginalWreckComponentKind50LA::body167920||
           p.modelOwner_.released()||!p.modelOwner_.model_.constructed||p.modelOwner_.model_.released||
           p.modelOwner_.model_.sceneIndex!=p.assets_->model14C()||
           p.physical_.model288!=p.assets_->model14C()||p.physical_.modelGraphNodes!=graph.nodes)
            return finish(S::effectArraysFailed,0x2cb634u); //Native custody before allocation.
        const bool ready=p.effectArrays_.allocateFreshAndFind2CB634(count,p.modelOwner_,graph);
        return finish(ready?S::effectChildrenRequired:S::effectArraysFailed,p.effectArrays_.sourceBoundary());
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshMutualAndCount166390(
        const std::optional<std::uint32_t>& actualSelectedDamage,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* selectedRoot=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !selectedRoot||selectedRoot->parent1C||selectedRoot->previous10||selectedRoot->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.mutualConstructionAttempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::mutualFiltersRequired||progress.prefixResult.sourcePC!=0x166390u||
           !p.queries_.storage_||!p.queries_.storage_->freshE8.categoryComplete||p.freshEffectCountWritten_)return {};
        progress.mutualConstructionAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        const auto result=p.queries_.constructFreshMutualAndDerived166390();
        if(result.status!=OriginalWreckQueryStatus50LA::complete||result.sourcePC!=0x2cb614u)
            return finish(S::mutualFiltersFailed,result.sourcePC?result.sourcePC:0x166390u);
        if(!actualSelectedDamage)return finish(S::stopped,0x2cb614u);
        //Actual incoming local from22C100, NOT inventory authoring.2CB61C is
        //the2CB618 delay slot and writes174 even when context is NULL.
        p.effects_.count174=*actualSelectedDamage;p.freshEffectCountWritten_=true;
        return finish(S::effectInputsRequired,0x2cb618u);
    }
    OriginalWreckConstructorPrefixResult50LA bindFreshEffectInputs2CB618(
        const OriginalWreckPieceEffectInputs50LA& binding,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* selectedRoot=p.componentSlots_[0];
            const auto& first=c.pieces_[0];
            //Both original calls consume the SAME incoming Track4A0/7FC.
            //Borrow the existing fields, never another external forest/context.
            if(!first.effectInputsStillBound()||
               binding.resourceContextField!=first.effectInputs_.resourceContextField||
               binding.particleParentField!=first.effectInputs_.particleParentField||
               storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !selectedRoot||selectedRoot->parent1C||selectedRoot->previous10||selectedRoot->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||p.freshEffectInputsAttempted_||!p.freshEffectCountWritten_||progress.retirementAttempted||
           progress.prefixResult.status!=S::effectInputsRequired||progress.prefixResult.sourcePC!=0x2cb618u||
           !binding.resourceContextField||!binding.particleParentField||
           !*binding.resourceContextField||!*binding.particleParentField)return {};
        //This admitted native context represents the SAME proven nonnull
        //incoming Track4A0 argument and actual7FC field. No fabricatedNULL path.
        p.freshEffectInputsAttempted_=true;p.effectInputs_=binding;
        p.effectResource_=*binding.resourceContextField; //2CB618 context consumption.
        p.effectParent_=*binding.particleParentField; //2CB620 parent consumption.
        p.effectInputsCommitted_=true;
        progress.prefixResult={S::effectConstructorRequired,0x2cb628u,storage_->prefixDomain.count,true};
        return progress.prefixResult; //Before count branch/arrays/effect constructors.
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshE8To166390(OriginalEntityTypeRegistry50LA& actualTypes,
        std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* selectedRoot=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !selectedRoot||selectedRoot->parent1C||selectedRoot->previous10||selectedRoot->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.e8ConstructorAttempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::e8ConstructorRequired||progress.prefixResult.sourcePC!=0x1662e4u||
           &actualTypes!=storage_->actualTypes383408||!storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||!p.queries_.storage_||
           !p.queries_.storage_->freshE4.appended||!p.queries_.storage_->freshE4.prepareComplete||
           p.queries_.storage_->freshE4.retired||p.sourceE8_||
           !p.sourceE4_||*p.sourceE4_!=p.componentSlots_[6u+p.sceneCount_])return {};
        auto& root=*p.componentSlots_[0];auto& e4=**p.sourceE4_;auto& e8=*p.componentSlots_[7u+p.sceneCount_];
        if(root.reference00!=1u||root.kind!=K::piece2CB4B0||e4.reference00!=1u||e4.kind!=K::collision11B190||
           e4.parent1C!=&root||e4.next14||e8.reference00||e8.kind!=K::unbound||e8.objectKey||
           e8.parent1C||e8.previous10||e8.next14||e8.firstChild18)return {};
        progress.e8ConstructorAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(storage_->prefixDomain.count>=storage_->prefixComponents.size())return finish(S::capacity,0x1662e4u);
        //Native preallocated component backing is not guestmalloc equivalence.
        //Counted/header cuts remain distinct from sourceE8 publication166300.
        p.queries_.beginFreshE8Header11B190(p.sourceE8_);
        const auto initialized=initializeOriginalWreckComponent50LA(e8,K::collision11B190,storage_->sessionGeneration,
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e8)),true,*storage_->actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::components,initialized.sourcePC);
        storage_->prefixComponents[storage_->prefixDomain.count++]=&e8;
        const auto constructed=p.queries_.continueFreshE8Counted11B210(e8,actualTypes,p.sourceE8_);
        if(constructed.status!=OriginalWreckQueryStatus50LA::complete||constructed.sourcePC!=0x166310u)
            return finish(S::e8ConstructionFailed,constructed.sourcePC?constructed.sourcePC:0x1662f8u);
        //Actual166324 append PRECEDES owner9C/category/11C710 allocation.
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,root,e8,0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::e8ConstructionFailed,appended.sourcePC);
        p.queries_.noteFreshE8Appended166324();
        const auto loaded=p.queries_.loadFreshE8Type3To166390();
        if(loaded.status!=OriginalWreckQueryStatus50LA::complete||loaded.sourcePC!=0x166390u)
            return finish(S::e8ConstructionFailed,loaded.sourcePC?loaded.sourcePC:0x16632cu);
        return finish(S::mutualFiltersRequired,0x166390u);
    }
    OriginalWreckConstructorPrefixResult50LA prepareAppendFreshE416629C(std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        using K=OriginalWreckComponentKind50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* selectedRoot=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !selectedRoot||selectedRoot->parent1C||selectedRoot->previous10||selectedRoot->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.retirementAttempted||
           progress.prefixResult.status!=S::e4NativeLoadComplete||progress.prefixResult.sourcePC!=0x16629cu||
           !p.queries_.storage_||p.queries_.storage_->freshE4.prepareAttempted||
           !p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||
           !p.sourceE4_||*p.sourceE4_!=p.componentSlots_[6u+p.sceneCount_]||
           !storage_->actualAttachmentLock423F58||!p.freshDetectorD8_.bound||
           !p.freshDetectorD8_.appended||p.freshDetectorD8_.retired)return {};
        auto& e4=**p.sourceE4_;auto& root=*p.componentSlots_[0];auto& d8=*p.componentSlots_[5u+p.sceneCount_];
        auto& d0=*p.componentSlots_[1u+p.sceneCount_];
        if(!p.sourceBodyD0_||*p.sourceBodyD0_!=&d0||d0.reference00!=1u||d0.kind!=K::body167920||
           d0.parent1C!=&root||!p.physical_.bodyConstructorFieldsApplied)return {};
        if(!p.sourceD8_||*p.sourceD8_!=&d8||d8.reference00!=1u||d8.kind!=K::detector295290||
           d8.parent1C!=&root||d8.next14||e4.reference00!=1u||e4.kind!=K::collision11B190||
           e4.parent1C||e4.previous10||e4.next14)return {};
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        const auto result=p.queries_.prepareFreshE411F570(p.modelOwner_.model_,p.assets_->graph());
        if(result.status!=OriginalWreckQueryStatus50LA::complete||result.sourcePC!=0x1662c8u)
            return finish(S::e4PreparationFailed,result.sourcePC?result.sourcePC:0x16629cu);
        //1662DC append mask0, after complete preparation; no World publication.
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,root,e4,0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::components,appended.sourcePC);
        p.queries_.noteFreshE4Appended1662DC();
        return finish(S::e8ConstructorRequired,0x1662e4u);
    }
    OriginalWreckConstructorPrefixResult50LA constructLoadFreshE41661D8(const std::string& actualFilename,
        OriginalEntityTypeRegistry50LA& types,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        const std::string& directory,std::array<std::uint8_t,256>& name,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;const auto body=1u+p.sceneCount_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* selectedRoot=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !selectedRoot||selectedRoot->parent1C||selectedRoot->previous10||selectedRoot->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        auto& e4=*p.componentSlots_[body+5u];
        if(!sourceIdentityBound()||p.freshE4_.attempted||progress.e4ConstructorAttempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::e4ConstructorRequired||progress.prefixResult.sourcePC!=0x1661d8u||
           !p.freshDetectorD8_.bound||!p.freshDetectorD8_.appended||p.freshDetectorD8_.retired||
           &types!=storage_->actualTypes383408||!storage_->actualObjectCount423F60||
           !p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||p.sourceE4_||
           e4.reference00||e4.kind!=K::unbound||e4.objectKey||e4.parent1C||e4.previous10||e4.next14||e4.firstChild18)return {};
        p.freshE4_.attempted=true;progress.e4ConstructorAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        if(actualFilename.size()>=256u||actualFilename.find('\0')!=std::string::npos)return finish(S::stopped,0x1661dcu);
        auto& scratch=p.freshE4_.filename170.emplace();
        std::memcpy(scratch.data(),actualFilename.c_str(),actualFilename.size()+1u); //309230 second copy from actual s0.
        std::size_t dot=actualFilename.size();while(dot&&scratch[dot-1u]!='.')--dot;
        if(!dot)return finish(S::stopped,0x166248u);--dot;
        if(dot+sizeof("f.col")>scratch.size())return finish(S::stopped,0x166248u);
        std::memcpy(scratch.data()+dot,"f.col",sizeof("f.col")); //Actual3A7678, not E0 m.col replay.
        if(storage_->prefixDomain.count>=storage_->prefixComponents.size())return finish(S::capacity,0x166250u);
        p.queries_.beginFreshE4Header11B190(p.sourceE4_);
        const auto initialized=initializeOriginalWreckComponent50LA(e4,K::collision11B190,storage_->sessionGeneration,
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e4)),true,*storage_->actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::components,initialized.sourcePC);
        storage_->prefixComponents[storage_->prefixDomain.count++]=&e4;
        const auto constructed=p.queries_.continueFreshE4Counted11B210(e4,types,p.sourceE4_);
        if(constructed.status!=OriginalWreckQueryStatus50LA::complete)return finish(S::e4ConstructionFailed,constructed.sourcePC);
        try {
            const std::string request(scratch.data());
            const auto loaded=p.queries_.loadFreshE4Type0NativeTransport(p.modelOwner_.model_,request,resolver,directory,name,diagnostic);
            if(loaded.status!=OriginalWreckQueryStatus50LA::complete)return finish(S::e4ConstructionFailed,loaded.sourcePC);
            return finish(S::e4NativeLoadComplete,0x16629cu);
        }catch(...){return finish(S::allocation,0x166294u);}
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshDetectorD816616C(
        const std::array<std::uint32_t,4>& actual443550,const std::array<float,4>& actual433B10,
        const OriginalWreckTerrainProvider50LA* const* actualTerrain330Field,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& f=p.freshDetectorD8_;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* selectedRoot=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !selectedRoot||selectedRoot->parent1C||selectedRoot->previous10||selectedRoot->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        const auto body=1u+p.sceneCount_;auto& dc=*p.componentSlots_[body+3u];
        auto& d8=*p.componentSlots_[body+4u];auto& root=*p.componentSlots_[0];
        if(!sourceIdentityBound()||f.attempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::detectorConstructorRequired||progress.prefixResult.sourcePC!=0x16616cu||
           !actualTerrain330Field||!storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           !p.freshContactDC_.constructed||!p.freshContactDC_.appended||p.freshContactDC_.retired||
           !p.sourceDC_||*p.sourceDC_!=&dc||dc.reference00!=1u||dc.parent1C!=&root||dc.next14||
           p.physical_.contactBody30!=&p.physical_.body||
           d8.reference00||d8.kind!=K::unbound||d8.objectKey||d8.parent1C||d8.previous10||d8.next14||d8.firstChild18||
           p.sourceD8_||p.detector_.constructed||
           storage_->prefixDomain.count>=storage_->prefixComponents.size()||
           !original_wreck_component_detail50LA::domain(storage_->prefixDomain))return {};
        f.attempted=true;f.origin443550=&actual443550;f.local433B10=&actual433B10;f.terrain330Field=actualTerrain330Field;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        //Native preallocated component is not a claim of guest2E8D90 allocation/null handling.
        const auto init=initializeOriginalWreckComponent50LA(d8,K::detector295290,storage_->sessionGeneration,
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&d8)),true,*storage_->actualObjectCount423F60);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::contactConstructionFailed,init.sourcePC);
        storage_->prefixComponents[storage_->prefixDomain.count++]=&d8;progress.d8Reached=true;
        auto& d=p.detector_;
        d.center40={{0,0,0,1}}; //2952B4
        d.half50={{0,0,0,1}}; //2952BC
        d.enabled64=0;d.queryEnabled68=1;d.reset6C=1;
        d.contact30=nullptr;d.terrain34=nullptr;d.query38=nullptr;
        d.pairs60=nullptr;d.count70=0;d.objects74=nullptr;
        std::memcpy(&d.center40,actual443550.data(),16); //2952F4
        std::memcpy(&d.half50,actual443550.data(),16); //295300
        f.constructed=true;d.constructed=true;p.sourceD8_=&d8; //166188
        //2953B8/CC actual t1=App433B10 raw copy. For source a3=t0=0
        //the local is never consumed: do not publish it as detector geometry.
        std::array<std::uint32_t,4> local50;
        std::memcpy(local50.data(),actual433B10.data(),16);
        d.contact30=&p.physical_; //2953D0
        d.terrain34=*actualTerrain330Field; //2953D4, same retained Racer330 field.
        if(!d.terrain34)p.physical_.generated.pointContacts48=false; //2953E8
        //Both optional query/pair modes are zero; direct141C20 identity return.
        d.enabled64=1; //1661B8
        if(!p.queries_.bindFreshContact2955F0(dc,d8,d.query38))return finish(S::contactConstructionFailed,0x2955f0u);
        f.bound=true;
        const auto attached=appendOriginalWreckComponent50LA(storage_->prefixDomain,root,d8,0u,*storage_->actualAttachmentLock423F58);
        if(attached.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::contactConstructionFailed,attached.sourcePC);
        f.appended=true;return finish(S::e4ConstructorRequired,0x1661d8u);
    }
    OriginalWreckConstructorPrefixResult50LA constructFreshContactDC1660A0(
        const std::array<std::uint32_t,4>& actual4435A0,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& f=p.freshContactDC_;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* selectedRoot=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !selectedRoot||selectedRoot->parent1C||selectedRoot->previous10||selectedRoot->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        const auto body=1u+p.sceneCount_;auto& dc=*p.componentSlots_[body+3u];
        auto& e0=*p.componentSlots_[body+2u];auto& root=*p.componentSlots_[0];
        if(!sourceIdentityBound()||f.attempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::contactConstructorRequired||progress.prefixResult.sourcePC!=0x1660a0u||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           !p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||
           !p.sourceE0_||*p.sourceE0_!=&e0||!p.sourceBodyD0_||*p.sourceBodyD0_!=p.componentSlots_[body]||
           e0.parent1C!=&root||e0.next14||!progress.e0FreshAppended||
           dc.reference00||dc.kind!=K::unbound||dc.objectKey||dc.parent1C||dc.previous10||dc.next14||dc.firstChild18||
           p.physical_.contactConstructorFieldsApplied||
           storage_->prefixDomain.count>=storage_->prefixComponents.size()||
           !original_wreck_component_detail50LA::domain(storage_->prefixDomain))return {};
        Type2ResourceView49X resource;
        if(!p.queries_.borrowFreshE0PointResource(p.modelOwner_.model_,e0,resource)||
           !resource.nodes||!resource.rays||!resource.nodeCount||!resource.rayCount||
           p.pointStack_.size()<resource.nodeCount+resource.rayCount)return {};
        f.attempted=true;f.origin4435A0=&actual4435A0;progress.contactSuffixAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        const auto init=initializeOriginalWreckComponent50LA(dc,K::contact295A40,storage_->sessionGeneration,
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&dc)),true,*storage_->actualObjectCount423F60);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::contactConstructionFailed,init.sourcePC);
        storage_->prefixComponents[storage_->prefixDomain.count++]=&dc;progress.dcReached=true;
        auto& physical=p.physical_;
        if(!physical.pointRecords.selectSource()||!physical.records.selectSource())std::terminate();
        initializeFreshWreckContact295A40_50LA(physical,actual4435A0,f.source80,f.source8C);
        f.constructed=true;p.sourceDC_=&dc; //1660BC after real295A40.
        physical.spring.generatedCapacity88=50; //295C2C
        if(!physical.pointRecords.allocate295C20())return finish(S::contactConstructionFailed,0x295c40u);
        f.source80=physical.pointRecords.data(); //295C48
        if(!physical.records.allocate295C20())return finish(S::contactConstructionFailed,0x295c58u);
        f.source8C=physical.records.data(); //295C60;141C20 identity return.
        physical.contactBody30=&physical.body; //295C84
        auto append=[&](const SpringVector4_48N& point){
            auto& state=physical.spring;if(state.pointCount7C>=50u)return;
            for(std::size_t i=0;i<state.pointCount7C;++i){const auto& old=physical.pointRecords[i];
                if(old.localPoint10.v[0]==point.v[0]&&old.localPoint10.v[1]==point.v[1]&&
                   old.localPoint10.v[2]==point.v[2]&&f.pointScenes[i]==&p.modelOwner_.model_)return;}
            const auto i=state.pointCount7C;auto& record=physical.pointRecords[i];
            f.pointScenes[i]=&p.modelOwner_.model_; //295EFC, actual source40 typed; numeric scene40 unwritten.
            std::memcpy(&record.localPoint10,&point,sizeof point); //295F24
            record.flags50=static_cast<std::uint8_t>((record.flags50&~1u)|1u);f.pointFlagsKnownMask[i]=1u;
            record.margin48=0; //295F68
            std::memcpy(&record.normal20,actual4435A0.data(),16); //295F88
            std::memcpy(&record.surfacePoint30,actual4435A0.data(),16); //295FAC
            ++state.pointCount7C; //295FB8
        };
        std::size_t top=0,remaining=resource.nodeCount+resource.rayCount;p.pointStack_[top++]=resource.root;
        while(top){
            if(!remaining--)return finish(S::contactConstructionFailed,0x296308u);
            const auto tag=p.pointStack_[--top];
            if(tag&type2Leaf49X){const auto ray=static_cast<std::size_t>(tag&~type2Leaf49X);
                if(ray>=resource.rayCount)return finish(S::contactConstructionFailed,0x295fd0u);
                append(resource.rays[ray].second);
            }else{
                if(tag>=resource.nodeCount||top+2u>p.pointStack_.size())return finish(S::contactConstructionFailed,0x296308u);
                p.pointStack_[top++]=resource.nodes[tag].children[1];p.pointStack_[top++]=resource.nodes[tag].children[0];
            }
        }
        applyOriginalWreckContact1660F0_50LA(physical);
        const auto attached=appendOriginalWreckComponent50LA(storage_->prefixDomain,root,dc,0u,*storage_->actualAttachmentLock423F58);
        if(attached.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::contactConstructionFailed,attached.sourcePC);
        f.appended=true;return finish(S::detectorConstructorRequired,0x16616cu);
    }
    OriginalWreckConstructorPrefixResult50LA prepareAppendFreshE016607C(std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        if(pieceIndex==1u){
            const auto& c=storage_->collections[0];const auto* root=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||c.sourceSlots38_!=2u||!c.ordered38_||
               c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[1]!=&p||
               !root||root->parent1C||root->previous10||root->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(!sourceIdentityBound()||progress.e0FreshPrepareAttempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::e0PreparationRequired||progress.prefixResult.sourcePC!=0x16607cu||
           !p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||!p.sourceE0_||
           *p.sourceE0_!=p.componentSlots_[3u+p.sceneCount_]||!storage_->actualAttachmentLock423F58)return {};
        auto& e0=**p.sourceE0_;auto& root=*p.componentSlots_[0];auto& d4=*p.componentSlots_[2u+p.sceneCount_];
        if(!p.sourceGravityD4_||*p.sourceGravityD4_!=&d4||d4.reference00!=1u||d4.parent1C!=&root||
           d4.next14||e0.parent1C||e0.previous10||e0.next14)return {};
        progress.e0FreshPrepareAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        const auto result=p.queries_.prepareFreshE011F570(p.modelOwner_.model_,p.assets_->graph());
        if(result.status!=OriginalWreckQueryStatus50LA::complete||result.sourcePC!=0x166084u)
            return finish(S::queryConstructionFailed,result.sourcePC?result.sourcePC:0x16607cu);
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,root,e0,0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::components,appended.sourcePC);
        progress.e0FreshAppended=true;p.queries_.noteFreshE0Appended166098();
        return finish(S::contactConstructorRequired,0x1660a0u);
    }
    OriginalWreckPieceStorage50LA* freshRetirementPiece(std::size_t pieceIndex=0u)noexcept {
        if(!freshSourceCountCurrent()||storage_->nativeCollectionCursor||
           storage_->queryPreparationAttempted||storage_->queriesColdPrepared||storage_->modelScenes.prepared())return nullptr;
        auto& c=storage_->collections[0];
        if(!c.freshLoadAttempted_||!c.constructorAttempted_||!c.count_||c.sourceSlots38_>2u||c.sourceSlots38_>c.count_||
           pieceIndex>1u||pieceIndex>=c.count_)return nullptr;
        auto& p=c.pieces_[pieceIndex];
        if(pieceIndex==1u&&(!freshSecondCursorCurrent(c)||
           p.freshBody_.attempted!=p.setupZonesBound_||
           p.freshBody_.attempted!=p.progress_.pieceSetupAttempted||
           (p.progress_.appended22C1B8&&(!p.componentSlots_||freshLiveSecondSibling(c.pieces_[0])!=p.componentSlots_[0]))||
           !freshSecondPairStopHistory(p)||
           bool(p.freshFireOwner_)!=p.freshFireAttempted_||bool(p.freshSplashOwner_)!=p.freshSplashAttempted_||
           (p.freshSmokeOwner_&&(!p.freshSmokeAttempts_||
               (p.freshSmokeOwner_->appended_&&(!p.freshSmokeOwner_->freshReaderComplete_||
                   (!p.freshSmokeOwner_->retired_&&p.freshSmokeOwner_->sourceBoundary()!=
                       (p.freshSmokeAttempts_<=p.effectArrays_.pairsStopped_?0x2cb798u:0x2cb720u))))))||
           (p.freshFireOwner_&&(!p.freshSmokeOwner_||!p.freshFireAttempts_||
               (p.freshFireOwner_->appended_&&(!p.freshFireOwner_->freshReaderComplete_||
                   (!p.freshFireOwner_->retired_&&p.freshFireOwner_->sourceBoundary()!=
                       (p.freshFireAttempts_<=p.effectArrays_.pairsStopped_?0x2cb7c0u:0x2cb778u))))))||
           (p.freshSplashOwner_&&(!p.freshDustOwner_||!p.freshDustOwner_->appended_||
               (p.freshSplashOwner_->appended_&&(!p.freshSplashOwner_->freshReaderComplete_||
                   (!p.freshSplashOwner_->retired_&&p.freshSplashOwner_->sourceBoundary()!=0x2cb8c4u)))))||
           bool(p.freshSmokeOwner_)!=p.freshSmokeAttempted_||bool(p.freshDustOwner_)!=p.freshDustAttempted_||
           (p.freshDustOwner_&&(p.effectArrays_.attempted_?
               (!p.effects_.count174||(p.effects_.count174&0x80000000u)||
                p.effectArrays_.pairsStopped_!=p.effects_.count174||
                p.effectArrays_.smokeWritten_!=p.effects_.count174||p.effectArrays_.fireWritten_!=p.effects_.count174||
                !p.freshSmokeOwner_||!p.freshFireOwner_||
                (p.freshSplashOwner_&&p.freshSplashOwner_->appended_&&
                 (!p.freshSplashOwner_->freshConstructorComplete_||!p.freshSplashOwner_->freshReaderComplete_||
                  (!p.freshSplashOwner_->retired_&&p.freshSplashOwner_->sourceBoundary()!=0x2cb8c4u)))||
                (p.freshDustOwner_->appended_&&(!p.freshDustOwner_->freshReaderComplete_||
                 (!p.freshDustOwner_->retired_&&p.freshDustOwner_->sourceBoundary()!=0x2cb854u)))):
               (p.freshSmokeOwner_||p.freshFireOwner_||
                (p.freshDustOwner_->appended_&&(!p.freshDustOwner_->freshReaderComplete_||
                    (!p.freshDustOwner_->retired_&&p.freshDustOwner_->sourceBoundary()!=0x2cb854u)))||
                (p.effects_.count174&&!(p.effects_.count174&0x80000000u)))))||
           p.freshEffectInputsAttempted_!=p.effectInputsCommitted_||
           (p.effectInputsCommitted_?(!p.effectInputsStillBound()||!p.effectResource_||!p.effectParent_):
               (p.effectResource_||p.effectParent_))||
           (p.dust168_&&!p.freshDustOwner_)||(p.splash16C_&&!p.freshSplashOwner_)))return nullptr;
        if(pieceIndex==1u){
            const auto* query=p.queries_.storage_.get();
            if(bool(query)!=p.progress_.e0ConstructorAttempted||
               (p.sourceE0_&&!query))return nullptr;
            if(query&&(!p.freshBody_.attempted||!p.queries_.freshE0Backing()||
               (query->freshCollectionAttempted&&!freshSecondCollectionStatus(p,c))))return nullptr;
        }
        if(!p.progress_.constructorAttempted||!p.componentSlots_||
           ((p.progress_.pieceSetupAttempted||p.setupZonesBound_)&&!p.freshBody_.attempted)||
           (p.queries_.prepared()&&!p.queries_.freshE0Backing())||p.queries_.constructed())return nullptr;
        if(pieceIndex<c.sourceSlots38_&&(!c.ordered38_||c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[pieceIndex]!=&p))return nullptr;
        return &p;
    }
    OriginalWreckConstructorPrefixResult50LA appendFreshOccurrenceModel165CF0(std::size_t pieceIndex=0u)noexcept {
        auto* p=freshRetirementPiece(pieceIndex);
        if(!p||p->freshBody_.attempted||storage_->nativeSourcePrefixRetired||p->progress_.pieceRetired||
           p->progress_.retirementAttempted||p->progress_.modelRetirementAttempted||
           !p->sourceModel14C_||*p->sourceModel14C_!=&p->modelOwner_.model_||
           storage_->collections[0].sourceSlots38_!=pieceIndex+1u)return {};
        return appendModelCore165CF0(*p,storage_->sessionGeneration,storage_->racerKey,
            storage_->nativeGlobals.pieceVector433B10);
    }
    //Source-only D0 constructor on the SAME canonical physical owner. Only
    //source lanes absent from that numeric projection live in freshBody_.
    void initializeFreshBody167920(OriginalWreckPieceStorage50LA& piece,
        OriginalWreckComponent50LA& entity)noexcept {
        initializeSourceBody16792050LA(piece.physical_,piece.freshBody_,entity,storage_->nativeGlobals);
    }
    OriginalWreckConstructorPrefixResult50LA setupFreshOccurrenceBody165E90(
        const OriginalWreckSetupZones50LA& zones,const std::string& actualFilename,
        std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;
        //Only the genuine second cold-model return may enter this bounded
        //extension. Whole-owner preflight precedes B4/B8 and every body store;
        //the first completed piece and its effects remain unchanged.
        if(pieceIndex==1u){
            const auto& collection=storage_->collections[0];
            const auto* root=p.componentSlots_[0];
            if(storage_->nativeSourcePrefixRetired||collection.sourceSlots38_!=2u||
               collection.sourceArray38_!=collection.ordered38_.get()||
               collection.ordered38_[1]!=&p||!root||root->parent1C||root->previous10||root->next14||
               !preflightFreshConstructorOnlyRetirement())return {};
        }
        if(progress.pieceSetupAttempted||p.freshBody_.attempted||p.setupZonesBound_||
           progress.pieceRetired||progress.retirementAttempted||progress.modelRetirementAttempted||
           progress.prefixResult.status!=S::pieceSetupRequired||progress.prefixResult.sourcePC!=0x165e90u||
           !p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||
           !p.modelOwner_.model_.constructed||p.modelOwner_.model_.released||
           p.physical_.bodyConstructorFieldsApplied||p.sourceBodyD0_||p.sourceGravityD4_||
           p.gravity_.target50||p.gravity_.count30||p.gravity_.capacity34||
           !storage_->nativeGlobals.current()||!storage_->nativeGravity4431C0||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           !zones.nativeVisibilityNode||!zones.nativeCollisionNode||
           zones.nativeVisibilityNode==zones.nativeCollisionNode||
           !zones.visibility4C0||!zones.collision4C4||!zones.collisionLifecycle||
           !zones.visibilityZoneField||!zones.collisionZoneField||
           &zones.visibility4C0->lifecycle==zones.collisionLifecycle||
           &zones.visibility4C0->portals==zones.collision4C4||
           !*zones.visibilityZoneField||*zones.visibilityZoneField>zones.visibility4C0->lifecycle.zoneCount||
           *zones.visibilityZoneField>zones.visibility4C0->portals.zoneCount||
           !*zones.collisionZoneField||*zones.collisionZoneField>zones.collisionLifecycle->zoneCount||
           *zones.collisionZoneField>zones.collision4C4->zoneCount||
           storage_->prefixDomain.count+2u>storage_->prefixComponents.size())return {};
        progress.pieceSetupAttempted=true;p.freshBody_.attempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        p.sourceVisibilityNodeB4_=zones.nativeVisibilityNode; //165EC8, same incoming210770 t3.
        p.visibilityWorldB4_=zones.visibility4C0;p.visibilityZoneB4_=*zones.visibilityZoneField;
        p.sourceCollisionNodeB8_=zones.nativeCollisionNode; //165ED0, same incoming210770 stack0.
        p.collisionWorldB8_=zones.collision4C4;p.collisionLifecycleB8_=zones.collisionLifecycle;
        p.collisionZoneB8_=*zones.collisionZoneField;p.setupZonesBound_=true;
        const auto body=1u+p.sceneCount_;
        auto& d0=*p.componentSlots_[body];auto& d4=*p.componentSlots_[body+1u];
        auto initialize=[&](OriginalWreckComponent50LA& e,K kind){
            const auto result=initializeOriginalWreckComponent50LA(e,kind,storage_->sessionGeneration,
                static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e)),true,*storage_->actualObjectCount423F60);
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
            storage_->prefixComponents[storage_->prefixDomain.count++]=&e;return true;
        };
        auto append=[&](OriginalWreckComponent50LA& e,std::uint32_t mask){
            return appendOriginalWreckComponent50LA(storage_->prefixDomain,*p.componentSlots_[0],e,mask,
                *storage_->actualAttachmentLock423F58).status==OriginalWreckComponentStatus50LA::complete;
        };
        if(!initialize(d0,K::body164ED0))return finish(S::physical,0x164edcu);
        initializeFreshBody167920(p,d0);
        p.sourceBodyD0_=&d0; //165EE8 after completed167920.
        //141C20 slot30 has no source effects.1652E0 only stores288; it does
        //NOT copy a world matrix or flags. Graph identities below are metadata.
        if(!p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||
           !p.modelOwner_.model_.constructed||p.modelOwner_.model_.released)
            return finish(S::physical,0x165f10u);
        p.freshBody_.model288=*p.sourceModel14C_; //1652E8.
        const auto graph=p.assets_->graph();p.physical_.model288=p.assets_->model14C();
        p.physical_.modelGraphNodes=graph.nodes;p.physical_.modelGraphCount=graph.count;
        p.physical_.body.scenePresent=true;
        p.physical_.body320=1; //165F1C.
        p.physical_.body.enabled=false; //1653C0(0).
        if(!append(d0,0xffffffffu))return finish(S::components,0x165f3cu);
        if(!initialize(d4,K::gravity282670))return finish(S::physical,0x28267cu);
        if(!constructOriginalWreckGravity282670(p.gravity_,d4,storage_->nativeGravity4431C0))
            return finish(S::physical,0x282694u);
        p.freshBody_.gravity50=nullptr; //2826BC constructor NULL allocation pointer.
        p.sourceGravityD4_=&d4; //165F60 before source282750.
        //Actual one-slot native reserve, not emulated guest allocation. Keep
        //the source allocation alias distinct from its target payload.
        p.gravity_.count30=0;p.gravity_.capacity34=1; //28275C/282760.
        p.freshBody_.gravity50=&p.gravity_.target50; //282774.
        p.gravity_.target50=&p.physical_; //2827B0.
        p.gravity_.count30=1; //2827BC.
        if(!append(d4,0u))return finish(S::components,0x165f94u);
        //The next stack buffer begins at170. Retain this bounded native
        //projection of the actual60 buffer; never infer a source path from assets.
        if(actualFilename.size()>=0x110u||actualFilename.find('\0')!=std::string::npos)
            return finish(S::stopped,0x165fa0u);
        auto& scratch=p.freshBody_.filename60.emplace();
        std::memcpy(scratch.data(),actualFilename.c_str(),actualFilename.size()+1u); //309230 copy.
        std::size_t dot=actualFilename.size();
        while(dot&&scratch[dot-1u]!='.')--dot;
        //The guest reaches a NULL destination when no dot exists. Bound that
        //unsupported case before overwrite, preserving the actual copied prefix.
        if(!dot)return finish(S::stopped,0x166010u);
        --dot;
        if(dot+sizeof("m.col")>scratch.size())return finish(S::stopped,0x166010u);
        std::memcpy(scratch.data()+dot,"m.col",sizeof("m.col")); //Literal3A7670,309230.
        return finish(S::publishedLifetimeRequired,0x166018u); //Before first E0 allocation.
    }
    //Whole-owner preflight: only actual constructed rows and written38 slots
    //are visited. Source30 may differ from capacity after a rejected read.
    bool preflightFreshConstructorOnlyRetirement()const noexcept {
        if(!storage_||!storage_->count||!storage_->collections[0].freshLoadAttempted_)
            return preflightFreshUnloadedConstructorRetirement();
        using K=OriginalWreckComponentKind50LA;
        if(!freshSourceCountCurrent()||storage_->nativeCollectionCursor||storage_->nativePublished374!=1u||
           storage_->queryPreparationAttempted||storage_->queriesColdPrepared||storage_->preparedQueries||
           storage_->queryBorrowsReleased||storage_->modelScenes.prepared())return false;
        if(storage_->nativeSourcePrefixRetired)return true;
        const auto& first=storage_->collections[0];
        if(!first.sourceWorld40_||!*first.sourceWorld40_||first.retirementAttempted_||
           first.sourceSlots38_>2u||
           (first.completedPieceConstructors_!=0u&&first.completedPieceConstructors_!=1u&&!freshSecondCursorAdvanced22C260(first))||
           (first.sourceArray38_&&first.sourceArray38_!=first.ordered38_.get())||
           (first.sourceSlots38_&&!first.sourceArray38_)||storage_->ordered374[0]!=&first)return false;
        const auto& root=first.collectionComponent_;
        const auto& firstPiece=first.pieces_[0];
        if(first.sourceArray38_){
            if(!first.freshPieceIndexKnown_||!first.freshPieceByteOffset_||
               *first.freshPieceByteOffset_!=4u*first.completedPieceConstructors_)return false;
            if(first.completedPieceConstructors_==1u||freshSecondCursorAdvanced22C260(first)){
                const auto& progress=firstPiece.progress_;
                const auto* query=firstPiece.queries_.storage_.get();
                const bool more=(first.sourceCount30_&0x80000000u)==0u&&first.sourceCount30_>1u;
                if(first.sourceSlots38_<1u||!progress.appended22C1B8||!query||
                   !query->freshCollectionAttempted||query->freshCollectionCursor!=2u||query->freshCollectionPC!=0x22c25cu||
                   progress.prefixResult.status!=(more?OriginalWreckConstructorPrefixStatus50LA::nextPieceRequired:
                       OriginalWreckConstructorPrefixStatus50LA::groupConstructorRequired)||
                   progress.prefixResult.sourcePC!=(more?0x22c0a0u:0x22c278u))return false;
            }
        }else if(first.freshPieceIndexKnown_||first.freshPieceByteOffset_||first.completedPieceConstructors_)return false;
        if(first.sourceSlots38_==2u){
            if(first.count_<2u||!freshSecondCursorCurrent(first)||
               !first.ordered38_||first.ordered38_[1]!=&first.pieces_[1]||
               !first.pieces_[1].progress_.constructorAttempted)return false;
        }
        const bool pieceAppended=firstPiece.progress_.appended22C1B8;
        const auto* liveSecond=freshLiveSecondSibling(firstPiece);
        const auto* pieceRoot=firstPiece.componentSlots_?firstPiece.componentSlots_[0]:nullptr;
        if(pieceAppended&&(!pieceRoot||first.sourceSlots38_<1u||!first.sourceArray38_||
           first.ordered38_[0]!=&firstPiece||pieceRoot!=&firstPiece.components_[0]||
           pieceRoot->reference00!=1u||pieceRoot->kind!=K::piece2CB4B0||
           pieceRoot->parent1C!=&root||pieceRoot->previous10||pieceRoot->next14!=liveSecond))return false;
        if(!pieceAppended&&pieceRoot&&(pieceRoot->parent1C||pieceRoot->previous10))return false;
        if(root.reference00!=1u||root.kind!=K::collection22BF00||root.parent1C||root.previous10||
           root.next14||root.firstChild18!=(pieceAppended?pieceRoot:nullptr)||!storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return false;
        const auto* retiredSecond=exactRetiredSecondPiece();
        std::size_t live=1u;
        for(std::size_t ci=0;ci<storage_->count;++ci){const auto& c=storage_->collections[ci];
            if(c.nativeRacerAppendAttempted_||c.nativeLoadDeactivateAttempted_||c.source34_)return false;
            if(c.nativeGroupAttempted_){
                if(ci||!freshGroupConstructorPrefixCurrent(c))return false;
                if(!c.nativeGroupRetired_)++live;
            }else if(c.groupComponent_.reference00||c.groupComponent_.kind!=K::unbound||c.source3C_)return false;
            if(ci&&(c.constructorAttempted_||c.retirementAttempted_||c.collectionComponent_.reference00||
               c.collectionComponent_.kind!=K::unbound||c.sourceSlots38_||c.sourceArray38_))return false;
            for(std::size_t pi=0;pi<c.count_;++pi){const auto& p=c.pieces_[pi];
                if(ci==0u&&pi==1u&&retiredSecond==&p)continue;
                if(((p.setupZonesBound_||p.progress_.pieceSetupAttempted)&&!p.freshBody_.attempted)||
                   (p.queries_.prepared()&&!p.queries_.freshE0Backing())||p.queries_.constructed()||(p.effectArrays_.attempted()&&!p.effectArrays_.freshRoute_)||
                   (p.effectInputsCommitted_&&!p.freshEffectInputsAttempted_)||(p.progress_.e0ConstructorAttempted&&!p.queries_.freshE0Backing()))return false;
                if(ci==0u&&pi<2u&&p.progress_.constructorAttempted&&p.components_[0].reference00){
                    if(pi==1u&&(!freshSecondCursorCurrent(first)||
                       (p.progress_.appended22C1B8?liveSecond!=&p.components_[0]:
                        (p.components_[0].parent1C||p.components_[0].previous10||p.components_[0].next14))))return false;
                    const auto r=const_cast<OriginalWreckRacerStorage50LA*>(this)->retirePieceBeforeE0Core(0,pi,nullptr,true,true);
                    if(r.status!=OriginalWreckPrefixRetirementStatus50LA::pieceRetired)return false;
                    live+=r.objectsDestroyed;continue;
                }
                if(p.modelOwner_.model_.constructed||p.modelMaterials_.started()||p.modelGeometry_||
                   p.modelPackets_.constructed240740()||p.progress_.pieceRetired)return false;
                for(std::size_t j=0;j<p.componentCount_;++j){const auto* e=p.componentSlots_[j];
                    if(e&&(e->reference00||e->kind!=K::unbound||e->parent1C||e->previous10||e->next14||e->firstChild18))return false;
                }
            }
        }
        if(*storage_->actualObjectCount423F60<live)return false;
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i){
            const auto* e=storage_->prefixComponents[i];if(!e)return false;
            if(e->reference00)continue;
            bool exact=false;
            if(retiredSecond)for(std::size_t j=0;j<retiredSecond->componentCount_;++j)
                if(retiredSecond->componentSlots_[j]==e)exact=true;
            if(first.nativeGroupRetired_&&e==&first.groupComponent_&&freshGroupConstructorPrefixCurrent(first))exact=true;
            if(!exact)return false; //Only exact retired second slots or the validated fresh group Entity.
        }
        return true;
    }
    bool retireFreshConstructorOnly22BF50()noexcept {
        if(!storage_||!storage_->count||!storage_->collections[0].freshLoadAttempted_)
            return retireFreshUnloadedConstructor22BF50();
        if(!preflightFreshConstructorOnlyRetirement())return false;
        if(storage_->nativeSourcePrefixRetired)return true;
        auto& c=storage_->collections[0];
        //The real unlinked group was constructed after the pieces. Retire
        //its reached array destructor prefix first; preserve source3C/C0/C4.
        if(c.nativeGroupAttempted_&&!c.nativeGroupRetired_){
            auto& g=c.groupComponent_;--g.reference00;
            if(!c.group_.retireFreshConstructor2C9F30())std::terminate();
            g.kind=OriginalWreckComponentKind50LA::entity141300;
            --*storage_->actualObjectCount423F60;c.nativeGroupRetired_=true;
        }
        //Whole owner preflight above precedes ALL mutation. Close the actual
        //second reached prefix before the first attached physics/effect piece.
        //A genuine second collection sibling is unlinked first, restoring the
        //first root's strict next14 NULL before its actual retirement.
        for(std::size_t end=std::min<std::size_t>(c.count_,2u);end;--end){
            auto& p=c.pieces_[end-1u];
            if(p.components_[0].reference00){
                const auto r=retirePieceBeforeE0Core(0,end-1u,nullptr,true,false);
                if(r.status!=OriginalWreckPrefixRetirementStatus50LA::pieceRetired)std::terminate();
            }
        }
        --c.collectionComponent_.reference00;
        //Bounded native cancellation of a partial array: unwritten slots are
        //never read. This is not a literal guest22BF50 failure-unwind claim.
        if(c.sourceArray38_)c.ordered38_.reset();
        c.collectionComponent_.kind=OriginalWreckComponentKind50LA::entity141300;
        --*storage_->actualObjectCount423F60;c.retirementAttempted_=true;
        c.retirementResult_={OriginalWreckPrefixRetirementStatus50LA::prefixRetired,0x22bf90u,1u,1u};
        storage_->nativeSourcePrefixRetired=true;
        //Source30/38/40/14C and374 remain historical; native borrows end later.
        return true;
    }
    OriginalWreckConstructorPrefixResult50LA loadNativeOccurrencePiece22BFD0(std::size_t pieceIndex,
        const WaterHeightProvider50JC* const* water334,
        const WaterHeightGlobals50JC* waterGlobals)noexcept {
        if(!sourceIdentityBound()||!storage_->nativeSourceMode||!storage_->count)return {};
        auto& c=storage_->collections[storage_->nativeCollectionCursor];
        if(storage_->nativePublished374<=storage_->nativeCollectionCursor||
           storage_->ordered374[storage_->nativeCollectionCursor]!=&c||
           (pieceIndex?c.nativeConstructorOnly_:!c.nativeConstructorOnly_)||
           c.sourceSlots38_!=pieceIndex||c.completedPieceConstructors_!=pieceIndex)return {};
        if(pieceIndex>=c.count_)return {};
        auto& piece=c.pieces_[pieceIndex];std::array<std::uint64_t,8> keys{};
        for(std::size_t i=0;i<keys.size();++i)
            keys[i]=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&piece.components_[i]));
        OriginalWreckConstructorPrefixBinding50LA b;
        b.sessionGeneration=storage_->sessionGeneration;b.racerKey=storage_->racerKey;
        b.collectionKey=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&c.collectionComponent_));
        b.pieceComponentKeys=keys.data();b.pieceComponentKeyCount=keys.size();
        b.actualObjectCount423F60=storage_->actualObjectCount423F60;
        b.actualAttachmentLock423F58=storage_->actualAttachmentLock423F58;b.actualTypes383408=storage_->actualTypes383408;
        b.actualWaterB0Field=water334;b.actualWaterGlobals=waterGlobals;
        return constructOrderedPiecePrefix(storage_->nativeCollectionCursor,pieceIndex,b);
    }
    OriginalWreckRacerStorageResult50LA adoptNativeColdInventory(
        std::unique_ptr<OriginalWreckAssetInventory50LA>& inventory)noexcept {
        return adoptInventoryBacking(inventory,0,0,true);
    }
    OriginalWreckInventoryResult50LA bindNativeColdModelSources(
        rr::assets::OriginalStandardMaterialDomain50LA& domain,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        if(!storage_||storage_->identityBound||storage_->nativeIdentityBindingAttempted||
           storage_->queryPreparationAttempted||!storage_->inventory)
            return {OriginalWreckInventoryStatus50LA::binding,0,0,0};
        return storage_->inventory->bindModelSources(domain,diagnostic);
    }
    //No source ctor or query publication is legal until the actual later
    //Session racer exists. Never invent an early Rig/key/generation substitute.
    bool bindNativeColdIdentity(const OriginalWreckComponent50LA& racer)noexcept {
        if(!storage_||storage_->identityBound||storage_->nativeIdentityBindingAttempted||
           storage_->sessionGeneration||storage_->racerKey||storage_->queryPreparationAttempted||
           !storage_->inventory||!storage_->inventory->modelSourcesBound()||
           racer.kind!=OriginalWreckComponentKind50LA::racer27E130||racer.reference00!=1||
           !racer.sessionGeneration||!racer.objectKey)return false;
        storage_->nativeIdentityBindingAttempted=true;
        storage_->sessionGeneration=racer.sessionGeneration;storage_->racerKey=racer.objectKey;
        for(std::size_t c=0;c<storage_->count;++c)for(std::size_t i=0;i<storage_->collections[c].count_;++i){
            auto& piece=storage_->collections[c].pieces_[i];
            if(!piece.queryOwner_.bindCold(*piece.assets_,piece.physical_,piece.piece_,
                piece.components_[0],racer.sessionGeneration))return false;
        }
        storage_->identityBound=true;return true;
    }
    bool nativeColdReservations(std::vector<NativeColdReservation>& out)const {
        if(!storage_||storage_->identityBound||storage_->nativeIdentityBindingAttempted||
           storage_->queryPreparationAttempted||!out.empty())return false;
        out.reserve(storage_->totalPieces);
        for(std::size_t c=0;c<storage_->count;++c){
            auto& collection=storage_->collections[c];
            if(collection.constructorAttempted_||collection.sourceSlots38_)return false;
            for(std::size_t i=0;i<collection.count_;++i){auto& piece=collection.pieces_[i];
                if(!piece.assets_||!piece.assets_->prepared()||piece.queries_.prepared()||
                   piece.queries_.constructed()||piece.progress_.constructorAttempted)return false;
                out.push_back({&piece.queries_,piece.assets_});
            }
        }
        return out.size()==storage_->totalPieces;
    }
    OriginalWreckRacerStorageResult50LA adoptInventoryBacking(
        std::unique_ptr<OriginalWreckAssetInventory50LA>& inventory,
        std::uint64_t sessionGeneration,std::uint64_t racerKey,bool nativeUnbound)noexcept {
        using S=OriginalWreckRacerStorageStatus50LA;
        OriginalWreckRacerStorageResult50LA r;
        auto fail=[&](S status){r.status=status;return r;};
        if(storage_||!inventory||(nativeUnbound?(sessionGeneration||racerKey):(!sessionGeneration||!racerKey)))
            return fail(S::binding);
        if(!inventory->complete()||(!nativeUnbound&&!inventory->modelSourcesBound()))return fail(S::inventory);
        const auto& authoring=inventory->authoring();const auto& planned=inventory->collections();
        if(planned.empty()||planned.size()!=authoring.collections.size())return fail(S::inventory);
        try {
            auto next=std::make_unique<Storage>();next->count=planned.size();next->nativeSourceMode=nativeUnbound;
            next->sessionGeneration=sessionGeneration;next->racerKey=racerKey;
            next->collections=std::make_unique<OriginalWreckCollectionStorage50LA[]>(next->count);
            next->ordered374=std::unique_ptr<OriginalWreckCollectionStorage50LA*[]>(new OriginalWreckCollectionStorage50LA*[next->count]);
            for(std::size_t c=0;c<next->count;++c){
                r.collectionOrdinal=c+1u;r.pieceOrdinal=0;
                const auto& source=authoring.collections[c];auto& collection=next->collections[c];
                if(source.ordinal!=c+1u||planned[c].ordinal!=source.ordinal||source.pieces.empty()||
                   source.pieces.size()!=planned[c].pieces.size())return fail(S::inventory);
                collection.ordinal_=source.ordinal;collection.count_=source.pieces.size();collection.nativeSourceMode_=nativeUnbound;
                const auto group=collection.group_.prepareCold(collection.count_-1u);
                if(group.status!=OriginalWreckGroupArrayStatus50LA::coldBacking)
                    return fail(group.status==OriginalWreckGroupArrayStatus50LA::allocation?S::allocation:S::inventory);
                collection.pieces_=std::make_unique<OriginalWreckPieceStorage50LA[]>(collection.count_);
                collection.ordered38_=std::unique_ptr<OriginalWreckPieceStorage50LA*[]>(new OriginalWreckPieceStorage50LA*[collection.count_]);
                if(next->totalComponents>std::numeric_limits<std::size_t>::max()-2u)return fail(S::capacity);
                next->totalComponents+=2u; // collection and group backing, still unconstructed
                for(std::size_t p=0;p<collection.count_;++p){
                    r.pieceOrdinal=p+1u;
                    auto* assets=inventory->pieceAssets(c,p);const auto& occurrence=planned[c].pieces[p];
                    if(!assets||!assets->prepared()||source.pieces[p].ordinal!=p+1u||
                       occurrence.collectionOrdinal!=c+1u||occurrence.pieceOrdinal!=p+1u||
                       assets->selected().ordinal!=c+1u||assets->selectedPiece().ordinal!=p+1u||
                       assets->selectedPiece().stem!=source.pieces[p].stem)return fail(S::inventory);
                    auto& piece=collection.pieces_[p];piece.assets_=assets;
                    piece.collectionOrdinal_=c+1u;piece.pieceOrdinal_=p+1u;
                    piece.sceneCount_=assets->graph().count;
                    if(!piece.sceneCount_||piece.sceneCount_>std::numeric_limits<std::size_t>::max()-8u)
                        return fail(S::capacity);
                    // piece + model/PSG entities + D0,D4,E0,DC,D8,E4,E8.
                    // PSG entities remain a separate hierarchy, not fabricated
                    // Entity children. This is capacity only; no141960 is called.
                    piece.componentCount_=piece.sceneCount_+8u;
                    if(next->totalComponents>std::numeric_limits<std::size_t>::max()-piece.componentCount_)
                        return fail(S::capacity);
                    // Eight actual non-model components: piece,D0,D4,E0,DC,D8,E4,E8.
                    // No dormant duplicate Model/PSG Entity row is allocated.
                    piece.components_=std::make_unique<OriginalWreckComponent50LA[]>(8u);
                    piece.componentSlots_=std::make_unique<OriginalWreckComponent50LA*[]>(piece.componentCount_);
                    piece.componentSlots_[0]=&piece.components_[0];
                    piece.componentSlots_[1]=piece.modelOwner_.wreckModelStorage();
                    for(std::size_t slot=0;slot<7u;++slot)
                        piece.componentSlots_[1u+piece.sceneCount_+slot]=&piece.components_[1u+slot];
                    if(assets->storage_->canonicalGraphBacking)return fail(S::inventory);
                    if(!nativeUnbound&&!piece.queryOwner_.bindCold(*assets,piece.physical_,piece.piece_,piece.components_[0],sessionGeneration))
                        return fail(S::binding);
                    next->totalComponents+=piece.componentCount_;++next->totalPieces;
                    if(!nativeUnbound)collection.ordered38_[p]=&piece;
                }
                if(!nativeUnbound)next->ordered374[c]=&collection;
            }
            if(next->totalComponents>(std::numeric_limits<std::size_t>::max()-2u)/sizeof(void*))return fail(S::capacity);
            next->prefixComponents.rows=std::make_unique<OriginalWreckComponent50LA*[]>(next->totalComponents);
            next->prefixComponents.capacity=next->totalComponents;
            // All allocations/identity checks finish before ownership transfer.
            // No original bytes, model transforms or runtime globals changed.
            // Exclusive cold adoption restages this SAME graph allocation as
            // backing before any canonical model/query construction. No caller
            // can select this route after published source objects exist.
            for(std::size_t c=0;c<next->count;++c)for(std::size_t p=0;p<next->collections[c].count_;++p){
                auto& asset=*next->collections[c].pieces_[p].assets_;
                for(auto& node:asset.storage_->nodes)node=SceneNode49P{};
                asset.storage_->cachedBoundsReady=false;asset.storage_->canonicalGraphBacking=true;
            }
            next->identityBound=!nativeUnbound;
            next->inventory=std::move(inventory);storage_=std::move(next);
            return {S::coldBacking,0,0,storage_->count,storage_->totalPieces,storage_->totalComponents};
        }catch(const std::bad_alloc&){return fail(S::allocation);}
    }
public:
    // Reserve/stage the actual full inventory without running11B190/1253F0,
    // appending children, or publishing any canonical world definition. The
    // common world has no per-piece slots or external model domain; those are
    // supplied below by the actual slot allocator and this retained owner.
    // A reached failure retains its prepared prefix and is terminal. Its world
    // must outlive this owner or pass releaseUnpublishedQueries below.
    OriginalWreckOrderedQueryResult50LA prepareOrderedQueriesCold(
        const OriginalWreckQueryWorld50LA& commonWorld,
        const OriginalWreckPieceQuerySlots50LA* slots,std::size_t count)noexcept {
        using S=OriginalWreckOrderedQueryStatus50LA;OriginalWreckOrderedQueryResult50LA out;
        if(!sourceIdentityBound()||storage_->nativeFreshCount370||storage_->queryPreparationAttempted||!slots||count!=storage_->totalPieces||
           commonWorld.modelScenes||commonWorld.groupPieceCapacity!=1u)return out;
        for(unsigned i=0;i<3;++i)if(commonWorld.refs[i]||commonWorld.identities[i])return out;
        if(!count||count>512u){out.status=S::capacity;return out;}
        // Check all inter-piece collisions BEFORE the first owner is staged.
        // prepareCold additionally authenticates actual world registrations,
        // existing identities, asset shapes and static type2 capacity.
        for(std::size_t p=0;p<count;++p)for(unsigned i=0;i<3;++i){
            if(!slots[p].refs[i]||slots[p].refs[i]>commonWorld.objectCount||!slots[p].identities[i])return out;
            for(std::size_t q=0;q<=p;++q)for(unsigned j=0;j<3;++j){
                if(q==p&&j>=i)break;
                if(slots[p].refs[i]==slots[q].refs[j]||slots[p].identities[i]==slots[q].identities[j])return out;
            }
        }
        storage_->queryPreparationAttempted=true;
        if(!storage_->modelScenes.prepareCold(storage_->sessionGeneration,count)){
            out.status=S::allocation;out.sourcePC=0x11bbc0u;return out;
        }
        std::size_t ordinal=0;
        for(std::size_t c=0;c<storage_->count;++c){
            auto& collection=storage_->collections[c];out.collectionOrdinal=c+1u;
            for(std::size_t p=0;p<collection.count_;++p,++ordinal){
                out.pieceOrdinal=p+1u;auto& piece=collection.pieces_[p];
                auto world=commonWorld;world.refs=slots[ordinal].refs;world.identities=slots[ordinal].identities;
                world.groupPieceCapacity=collection.count_;world.modelScenes=&storage_->modelScenes;
                // Model/PSG rows start at1. They are separate hierarchy rows,
                // not piece Entity children. The seven following components
                // retain D0,D4,E0,DC,D8,E4,E8 constructor/append order.
                const auto body=1u+piece.sceneCount_;
                const std::array<OriginalWreckComponent50LA*,3> queries{{
                    piece.componentSlots_[body+2u],piece.componentSlots_[body+5u],piece.componentSlots_[body+6u]}};
                try {const auto& resource=piece.assets_->queryE0();
                    piece.pointStack_.resize(resource.nodeCount+resource.rayCount);
                }catch(...){out.status=S::allocation;out.sourcePC=0x296190u;return out;}
                out.query=piece.queries_.prepareBackingCold(piece.queryOwner_,world,*piece.componentSlots_[1],queries);
                out.sourcePC=out.query.sourcePC;
                if(out.query.status!=OriginalWreckQueryStatus50LA::complete){
                    out.status=S::queryPreparation;out.preparedPieces=storage_->preparedQueries;return out;
                }
                ++storage_->preparedQueries;
            }
        }
        storage_->queriesColdPrepared=true;out.status=S::coldPrepared;
        out.collectionOrdinal=0;out.pieceOrdinal=0;out.preparedPieces=storage_->preparedQueries;return out;
    }
    bool orderedQueriesColdPrepared()const noexcept {
        return sourceIdentityBound()&&storage_->queriesColdPrepared&&!storage_->queryBorrowsReleased;
    }
    // Native unpublished-world borrow release only. Validate the entire prefix
    // first. No source unregister/destructor/effect receipt is manufactured.
    bool releaseUnpublishedQueries(const LiveZoneWorld50EN& actualWorld)noexcept {
        if(!sourceIdentityBound()||!storage_->queryPreparationAttempted||storage_->queryBorrowsReleased)return false;
        for(std::size_t c=0;c<storage_->count;++c)for(std::size_t p=0;p<storage_->collections[c].count_;++p){
            if(storage_->collections[c].pieces_[p].setupZonesBound_)return false;
            const auto& queries=storage_->collections[c].pieces_[p].queries_;
            if(queries.prepared()&&!queries.canReleaseUnpublishedWorld(actualWorld))return false;
        }
        for(std::size_t c=0;c<storage_->count;++c)for(std::size_t p=0;p<storage_->collections[c].count_;++p){
            auto& queries=storage_->collections[c].pieces_[p].queries_;
            if(queries.prepared()&&!queries.releaseUnpublishedWorld(actualWorld))return false;
        }
        if(storage_->modelScenes.prepared()&&!storage_->modelScenes.closeAfterNativeRevocation())return false;
        storage_->queryBorrowsReleased=true;storage_->queriesColdPrepared=false;return true;
    }
    // Separate native backing closure after every reached E0 source retirement.
    // Unreached occurrences must still pass their unpublished backing gates.
    bool releaseSourceRetiredQueries(const LiveZoneWorld50EN& actualWorld)noexcept {
        if(!sourceIdentityBound()||storage_->queryBorrowsReleased)return false;
        bool publishedHistory=false;
        for(std::size_t c=0;c<storage_->count;++c)for(std::size_t p=0;p<storage_->collections[c].count_;++p){
            const auto& piece=storage_->collections[c].pieces_[p];const auto& q=piece.queries_;
            if(q.hasFullE0PublicationHistory()){
                if(!piece.progress_.e0PublicationAttempted||!piece.progress_.e0PrefixRetired)return false;
                publishedHistory=true;
            }
            if(piece.setupZonesBound_||!q.prepared()||
               !(q.hasFullE0PublicationHistory()?q.canReleaseSourceRetiredWorld(actualWorld):q.canReleaseUnpublishedWorld(actualWorld)))return false;
        }
        if(!publishedHistory)return false;
        for(std::size_t c=0;c<storage_->count;++c)for(std::size_t p=0;p<storage_->collections[c].count_;++p){
            auto& q=storage_->collections[c].pieces_[p].queries_;
            if(!(q.hasFullE0PublicationHistory()?q.releaseSourceRetiredWorld(actualWorld):q.releaseUnpublishedWorld(actualWorld)))std::terminate();
        }
        if(storage_->modelScenes.prepared()&&!storage_->modelScenes.closeAfterNativeRevocation())std::terminate();
        storage_->queryBorrowsReleased=true;storage_->queriesColdPrepared=false;return true;
    }
    // Each next authored occurrence requires the preceding real WATER suffix.
    // Collection advancement still waits for the genuine group constructor.
    // This canonical prefix reaches fresh
    // PSG hierarchy loading, then stops BEFORE material/resource/packet load.
    // A warm cache hit stops before source geometry acquisition instead.
    OriginalWreckConstructorPrefixResult50LA constructOrderedPiecePrefix(
        std::size_t collectionIndex,std::size_t pieceIndex,const OriginalWreckConstructorPrefixBinding50LA& binding)noexcept {
        if(!orderedQueriesColdPrepared())return {};
        return constructPiecePrefixCore(collectionIndex,pieceIndex,binding,false,nullptr,nullptr,nullptr);
    }
private:
    //Fresh source-only represented1659C0 stores. The legacy grouped numeric
    //initializer remains unchanged; pointer publication must interleave here.
    void initializeFreshPiece1659C0(OriginalWreckPieceStorage50LA& piece)noexcept {
        auto& p=piece.piece_;
        const SpringVector4_48N vf0{{0.f,0.f,0.f,1.f}};
        p.authored50=vf0; //1659EC
        p.look60=vf0; //1659F8
        p.up70=vf0; //165A04
        p.previous80=vf0;p.current90=vf0; //165A20 vector-array constructor.
        p.normalA0=vf0; //165A30
        p.impulseC0=vf0; //165A3C; not overwritten by the global.
        const void* actual433B10=storage_->nativeGlobals.pieceVector433B10;
        std::memcpy(p.authored50.v,actual433B10,16); //165A5C
        std::memcpy(p.look60.v,actual433B10,16); //165A78
        std::memcpy(p.up70.v,actual433B10,16); //165A88
        std::memcpy(p.normalA0.v,actual433B10,16); //165A98
        std::memcpy(p.previous80.v,actual433B10,16); //165AA8
        std::memcpy(p.current90.v,actual433B10,16); //165AB4
        p.water3C=0;p.water40=0; //165ABC/165AC4
        piece.actualWaterB0_=nullptr; //165ACC; later165C9C is a separate store.
        piece.sourceModel14C_.emplace(nullptr); //165AD4, before wake/scalar stores.
        p.wake30=1;p.flight34=0;p.flight38=0; //165AE4/165AEC/165AF4
        p.flightDelay120=.5f; //165AFC
        p.restitution110=0.f;p.frictionTorqueScale114=1.f;p.inverseExtent118=1.f;
        p.dragEC=0.f;p.rateDampingF0=0.f;p.waterDragF4=0.f;p.waterRateF8=0.f;p.buoyancyFC=0.f;
        p.baseDrag100=0.f;p.baseRateDamping104=0.f;p.finalRateDamping108=200.f;p.finalDrag10C=50.f;
        p.airborneTime11C=0.f;p.measuredLinearAcceleration124=0.f;p.measuredAngularAcceleration128=0.f;
        p.contactTime12C=0.f;p.changeWindow130=0.f;p.stopSpeed134=10.f;p.stopAngularSpeed138=.5f;
        p.stopLinearAcceleration13C=10.f;p.stopAngularAcceleration140=1.f;
        p.explicitCandidateCount148=0; //165BB4. Unrepresented144/150 pointer lanes are not fabricated.
        piece.effectArrays_.freshRoute_=true; //Native selection, no source store.
        piece.effectArrays_.source160_=nullptr; //2CB4D0
        piece.effectArrays_.source164_=nullptr; //2CB4D4
        piece.dust168_=nullptr;piece.splash16C_=nullptr; //2CB4D8/2CB4DC
        piece.effectArrays_.source170_=nullptr; //2CB4E0
        piece.effects_.dustActive178=0;piece.effects_.dustTime17C=0.f; //2CB4E4/2CB4E8
        //174 intentionally remains unknown/unconsumed until2CB61C.
    }
    OriginalWreckConstructorPrefixResult50LA constructPiecePrefixCore(
        std::size_t collectionIndex,std::size_t pieceIndex,const OriginalWreckConstructorPrefixBinding50LA& binding,
        bool fresh,const std::array<float,4>* position,const std::array<float,4>* look,
        const std::array<float,4>* up)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA out;
        if((fresh&&(!position||!look||!up))||
           !(fresh?freshPieceAdmission(pieceIndex):orderedQueriesColdPrepared())||collectionIndex>=storage_->count||
           (storage_->nativeSourceMode?collectionIndex!=storage_->nativeCollectionCursor:collectionIndex!=0))return out;
        auto& collection=storage_->collections[collectionIndex];
        if(pieceIndex>=collection.count_||!collection.ordered38_||collection.retirementAttempted_)return out;
        auto& piece=collection.pieces_[pieceIndex];auto& progress=piece.progress_;
        if(progress.constructorAttempted){out=progress.prefixResult;out.status=S::stopped;return out;}
        if((!collection.nativeSourceMode_&&collection.ordered38_[pieceIndex]!=&piece)||collection.sourceSlots38_!=pieceIndex||
           collection.completedPieceConstructors_!=pieceIndex)return out;
        if(pieceIndex){
            const auto& previous=collection.pieces_[pieceIndex-1u].progress_;
            if(!collection.constructorAttempted_||previous.pieceRetired||previous.retirementAttempted||
               previous.e8PrefixRetired||previous.e4PrefixRetired||previous.contactSuffixRetired||previous.e0PrefixRetired||
               !previous.appended22C1B8||previous.prefixResult.status!=S::nextPieceRequired||
               previous.prefixResult.sourcePC!=0x22c0a0u)return out;
            OriginalWreckDomainRows50LA live;if(!liveRows(live))return out;
            OriginalWreckComponentDomain50LA current{};
            if(!liveRetirementDomain(live,current))return out;
        }else if(collection.constructorAttempted_&&!collection.nativeConstructorOnly_)return out;
        const auto* vectors=storage_->inventory->constructorVectors();
        if(binding.sessionGeneration!=storage_->sessionGeneration||binding.racerKey!=storage_->racerKey||
           !binding.collectionKey||binding.collectionKey==storage_->racerKey||!binding.pieceComponentKeys||
           binding.pieceComponentKeyCount!=8u||!binding.actualObjectCount423F60||
           !binding.actualAttachmentLock423F58||(!binding.constructorGlobals&&!storage_->nativeGlobals.current())||
           (!binding.actualVector4431C0&&!storage_->nativeGravity4431C0)||
           !binding.actualTypes383408||!vectors||
           !binding.actualWaterB0Field||(*binding.actualWaterB0Field&&!binding.actualWaterGlobals))return out;
        // Reserve the complete selected occurrence before any source mutation.
        // Retained canonical history is never reset or silently truncated.
        const auto existing=storage_->prefixDomain.count;
        const auto collectionRows=pieceIndex?0u:1u;
        const auto prefixCapacity=storage_->prefixComponents.size();
        if(existing>prefixCapacity||
           piece.componentCount_>prefixCapacity-existing||
           collectionRows>prefixCapacity-existing-piece.componentCount_){
            out.status=S::capacity;out.sourcePC=0x22c0a0u;return out;
        }
        if(pieceIndex&&(binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
           binding.constructorGlobals!=storage_->actualConstructorGlobals||
           binding.actualVector4431C0!=storage_->actualGravityVector4431C0||
           binding.actualTypes383408!=storage_->actualTypes383408))return out;
        if(binding.collectionKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&collection.collectionComponent_))||
           storage_->racerKey==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(piece.componentSlots_[1])))return out;
        for(std::size_t i=0;i<8u;++i){
            const auto key=binding.pieceComponentKeys[i];
            if(key!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&piece.components_[i])))return out;
            if(!key||key==binding.collectionKey||key==storage_->racerKey)return out;
            for(std::size_t j=0;j<i;++j)if(key==binding.pieceComponentKeys[j])return out;
        }
        progress.constructorAttempted=true;
        collection.constructorAttempted_=true;
        storage_->actualTypes383408=binding.actualTypes383408;
        storage_->actualConstructorGlobals=binding.constructorGlobals;
        storage_->actualGravityVector4431C0=binding.actualVector4431C0;
        storage_->actualObjectCount423F60=binding.actualObjectCount423F60;
        storage_->actualAttachmentLock423F58=binding.actualAttachmentLock423F58;
        auto& domain=storage_->prefixDomain;
        if(!pieceIndex&&!collection.nativeConstructorOnly_&&!storage_->nativeSourceMode)domain={storage_->sessionGeneration,storage_->prefixComponents.data(),0};
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,domain.count,domain.count!=0};return progress.prefixResult;
        };
        auto initialize=[&](OriginalWreckComponent50LA& component,K kind,std::uint64_t key){
            const auto result=initializeOriginalWreckComponent50LA(component,kind,storage_->sessionGeneration,
                key,true,*binding.actualObjectCount423F60);
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
            storage_->prefixComponents[domain.count++]=&component;return true;
        };
        if(!pieceIndex&&!fresh){
            if(!collection.nativeConstructorOnly_&&
               !initialize(collection.collectionComponent_,K::collection22BF00,binding.collectionKey))
                return finish(S::components,0x22bf0cu);
            collection.nativeConstructorOnly_=false;
            collection.sourceCount30_=storage_->inventory->authoring().collections[collectionIndex].sourceNumberOfParts;
            if(collection.sourceCount30_!=collection.count_)return finish(S::components,0x22c04cu);
            // Native prewarmed array adoption, not a guest allocator result.
            collection.sourceArray38_=collection.ordered38_.get(); //22C08C
        }
        if(!initialize(piece.components_[0],K::piece2CB4B0,binding.pieceComponentKeys[0]))
            return finish(S::components,0x1659ccu);
        if(fresh)initializeFreshPiece1659C0(piece);
        else {
            if(binding.constructorGlobals)initializeOriginalWreckPieceFields50LA(piece.piece_,*binding.constructorGlobals);
            else initializeOriginalWreckPieceFields50LA(piece.piece_,storage_->nativeGlobals);
            piece.effects_.dustActive178=0;piece.effects_.dustTime17C=0.f;
        }
        collection.ordered38_[pieceIndex]=&piece; //Actual22C16C; native unwritten slots remain unread.
        collection.sourceSlots38_=pieceIndex+1u; //22C16C, before setup2CB5A0/165C30.
        std::memcpy(piece.piece_.authored50.v,fresh?position->data():vectors->position434830.data(),16);
        std::memcpy(piece.piece_.look60.v,fresh?look->data():vectors->look434860.data(),16);
        std::memcpy(piece.piece_.up70.v,fresh?up->data():vectors->up434850.data(),16);
        piece.actualWaterB0_=*binding.actualWaterB0Field;piece.actualWaterGlobals_=binding.actualWaterGlobals;
        auto graph=piece.assets_->graph();
        std::array<SceneNodeIndex49P,rr::assets::maxModelNodes49O> rows{};
        for(std::size_t row=0;row<piece.sceneCount_;++row)rows[row]=static_cast<SceneNodeIndex49P>(row);
        OriginalRacerModelSceneBinding50LA modelBinding{storage_->sessionGeneration,
            binding.actualObjectCount423F60,binding.actualAttachmentLock423F58,binding.actualTypes383408,
            piece.assets_->model14C(),rows.data(),static_cast<std::uint32_t>(piece.sceneCount_)};
        auto collectRows=[&](){
            for(std::size_t row=0;row<piece.sceneCount_;++row){
                auto* actual=piece.modelOwner_.wreckRowStorage(static_cast<std::uint32_t>(row));
                if(!actual||!actual->reference00)continue;
                piece.componentSlots_[1u+row]=actual;
                bool present=false;for(std::size_t i=0;i<domain.count;++i)present=present||domain.objects[i]==actual;
                if(!present)storage_->prefixComponents[domain.count++]=actual;
            }
        };
        auto modelResult=piece.modelOwner_.constructDirectRoot2403A0(modelBinding,graph,true);
        collectRows();if(!modelResult.okay())return finish(S::components,modelResult.sourcePC);
        if(storage_->nativeSourceMode)piece.sourceModel14C_=&piece.modelOwner_.model_; //165CBC, canonical Model identity.
        modelResult=piece.modelOwner_.prepareDirectRoot26E934(graph,*piece.assets_->standardModelPayload()->member());
        if(!modelResult.okay())return finish(S::components,modelResult.sourcePC);
        const auto file=piece.assets_->modelFileIdentity();
        rr::assets::OriginalStandardModelFileView50LA source;
        rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
        const auto* resolver=storage_->inventory->modelResolverIdentity();
        if(!resolver||!resolver->modelFile(file,source,diagnostic)||!source.member||!source.bytes||
           *source.bytes!=*piece.assets_->standardModelPayload()->originalBytes())
            return finish(S::modelLoadRequired,0x26e9acu);
        // No source geometry retain/clone is faked. Even the childless warm
        // continuation remains closed until its actual prototype is bound.
        if(source.geometry240110)return finish(S::warmModelRequired,0x26e9f0u);
        modelResult=piece.modelOwner_.loadDirectRootSourceRows28CAC0(graph,piece.assets_->hierarchy());
        collectRows();if(!modelResult.okay())return finish(S::components,modelResult.sourcePC);
        // Next source phase26EE1C is real material acquisition. Model append,
        // authored-position extraction/reset, D0 and D4 follow packet loading.
        return finish(S::modelLoadRequired,0x26ee1cu);
    }
public:
    // Continue ONLY this actual fresh occurrence already stopped at26EE1C.
    // The actual application domain and36B970 prefix are caller-owned inputs;
    // inventory binding pins both resolver and material-table identities.
    OriginalWreckConstructorPrefixResult50LA loadFreshModelOccurrence26EE1C(
        std::size_t collectionIndex,std::size_t pieceIndex,std::uint64_t sessionGeneration,std::uint64_t racerKey,
        rr::assets::OriginalStandardMaterialDomain50LA& application,
        const rr::assets::OriginalStandardMaterialPath50LA& path)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||collectionIndex>=storage_->count)return rejected;
        auto& collection=storage_->collections[collectionIndex];
        if(pieceIndex>=collection.sourceSlots38_||!collection.ordered38_||collection.retirementAttempted_)return rejected;
        auto& piece=collection.pieces_[pieceIndex];auto& progress=piece.progress_;
        if(collection.freshLoadAttempted_&&(!piece.sourceModel14C_||
           *piece.sourceModel14C_!=&piece.modelOwner_.model_))return rejected;
        if(collection.ordered38_[pieceIndex]!=&piece||!progress.constructorAttempted||
           progress.retirementAttempted||progress.pieceRetired||progress.freshModelLoadAttempted||
           sessionGeneration!=storage_->sessionGeneration||racerKey!=storage_->racerKey||
           progress.prefixResult.status!=S::modelLoadRequired||progress.prefixResult.sourcePC!=0x26ee1cu||
           !application.prepared()||!path.actualPrefix36B970||
           application.resolver3E1A70().get()!=storage_->inventory->modelResolverIdentity()||
           application.materials382B38().get()!=storage_->inventory->modelMaterialsIdentity())return rejected;
        if(!piece.modelOwner_.loadedFreshRows28CAC0()||piece.modelMaterials_.started()||piece.modelGeometry_||
           piece.modelOwner_.fields().geometry1B4||piece.modelOwner_.fields().sourceRows1B8||
           piece.modelOwner_.fields().packets1F0)return rejected;
        progress.freshModelLoadAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        auto& fields=piece.modelOwner_.fields();
        const auto& payload=piece.assets_->standardModelPayload();
        // This native suspension boundary must still describe the same fresh
        // cache miss. A changed association cannot switch this loaded hierarchy
        // onto the source warm-clone branch or acquire extra material refs.
        rr::assets::OriginalStandardModelFileView50LA source;
        rr::assets::OriginalStandardTextureDiagnostic50LA sourceDiagnostic;
        if(!application.resolver3E1A70()->modelFile(piece.assets_->modelFileIdentity(),source,sourceDiagnostic)||
           !source.bytes||!source.member||source.geometry240110||
           *source.bytes!=*payload->originalBytes())return finish(S::stopped,0x26ee1cu);
        rr::assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
        const bool acquired=piece.modelMaterials_.acquire26E640(*payload,application.materials382B38(),path,materialDiagnostic);
        // Preserve26E670/26E684 even if the first lookup fails. Allocation
        // identity does not authorize reading an unwritten material slot.
        if(piece.modelMaterials_.started()){
            fields.materialCount1C0=static_cast<std::int32_t>(*piece.modelMaterials_.count1C0());
            fields.materials1C4=piece.modelMaterials_.allocationBase1C4();
        }
        if(!acquired)return finish(S::modelLoadRequired,materialDiagnostic.sourcePC);
        auto modelResult=piece.modelOwner_.loadQuantization240570(*payload->scene());
        if(!modelResult.okay())return finish(S::modelLoadRequired,modelResult.sourcePC);
        try{piece.modelGeometry_=std::make_shared<rr::assets::OriginalStandardModelGeometry50LA>();}
        catch(...){return finish(S::allocation,0x26ee38u);}
        rr::assets::OriginalStandardResourceDiagnostic50LA resourceDiagnostic;
        // Existing typed geometry owner includes240210 payload construction
        // and prototype26EE74. Its successful source reference is real, not
        // shared_ptr::use_count; its internal unsupported-input failure stays
        // a terminal bounded loader failure, never a completed resource.
        if(!piece.modelGeometry_->construct240110(payload,application.resolver3E1A70(),
                piece.assets_->modelFileIdentity(),&piece.modelOwner_,resourceDiagnostic)){
            finish(S::geometryAdmission,0x26ee38u);
            progress.prefixResult.nativeLoaderDiagnosticPC=resourceDiagnostic.sourcePC;
            return progress.prefixResult;
        }
        fields.geometry1B4=piece.modelGeometry_.get();
        rr::assets::OriginalStandardTextureDiagnostic50LA cacheDiagnostic;
        if(!application.resolver3E1A70()->publishGeometry1326A0(piece.assets_->modelFileIdentity(),
                piece.modelGeometry_,cacheDiagnostic))return finish(S::modelLoadRequired,cacheDiagnostic.sourcePC);
        // Source26EE90 closes its transient reader. The retained immutable
        // native payload has no open reader or guest allocation to reproduce.
        modelResult=piece.modelOwner_.allocateSourceRowTable26EE9C();
        if(!modelResult.okay())return finish(S::modelLoadRequired,modelResult.sourcePC);
        const auto packets=piece.modelPackets_.construct240740(piece.modelOwner_,*piece.modelGeometry_,false);
        if(!packets.okay())return finish(S::modelLoadRequired,packets.sourcePC);
        //165CD4 returns before165CF0 appends this exact model. Body/gravity,
        // initial query histories, effects and publication remain unreached.
        return finish(S::modelAppendRequired,0x165cf0u);
    }
    // Continue ONLY this actual occurrence already stopped at the warm26E9F0
    // hit (its PSG geometry is already associated). 26E9F0 publishes1B4,
    // 26E9F4..26EA00 retain104960, 26EA10 prototype=geometry.1C, then (a2=0)
    // 26EA88: 191 bit1 -> 26EA9C virtual98(this,prototype.child184); children
    // -> 26EB10 virtual98(this,prototype); childless -> 26EAC4. Then 26EB44
    // material clone, 26EBBC virtualB0=2406E0, 26EE9C row table, 240740.
    // The prototype is resolved through the geometry's own prototype1C, only
    // among this racer's earlier pieces or the caller's own creators.
    OriginalWreckConstructorPrefixResult50LA loadWarmModelOccurrence26E9F0(
        std::size_t collectionIndex,std::size_t pieceIndex,std::uint64_t sessionGeneration,std::uint64_t racerKey,
        rr::assets::OriginalStandardMaterialDomain50LA& application,
        const OriginalWreckWarmCreator50LA* external,std::size_t externalCount)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||collectionIndex>=storage_->count||(externalCount&&!external))return rejected;
        auto& collection=storage_->collections[collectionIndex];
        if(pieceIndex>=collection.sourceSlots38_||!collection.ordered38_||collection.retirementAttempted_)return rejected;
        auto& piece=collection.pieces_[pieceIndex];auto& progress=piece.progress_;
        if(collection.ordered38_[pieceIndex]!=&piece||!progress.constructorAttempted||
           progress.retirementAttempted||progress.pieceRetired||progress.freshModelLoadAttempted||
           progress.warmModelLoadAttempted||sessionGeneration!=storage_->sessionGeneration||
           racerKey!=storage_->racerKey||progress.prefixResult.status!=S::warmModelRequired||
           progress.prefixResult.sourcePC!=0x26e9f0u||!application.prepared()||
           application.resolver3E1A70().get()!=storage_->inventory->modelResolverIdentity()||
           application.materials382B38().get()!=storage_->inventory->modelMaterialsIdentity())return rejected;
        auto& owner=piece.modelOwner_;auto& fields=owner.fields();
        if(owner.rowsLoaded_||piece.modelMaterials_.started()||piece.modelGeometry_||
           fields.geometry1B4||fields.sourceRows1B8||fields.packets1F0)return rejected;
        // The association must still be the same warm hit at this boundary.
        rr::assets::OriginalStandardModelFileView50LA source;rr::assets::OriginalStandardTextureDiagnostic50LA td;
        if(!application.resolver3E1A70()->modelFile(piece.assets_->modelFileIdentity(),source,td)||
           !source.bytes||!source.member||!source.geometry240110||
           *source.bytes!=*piece.assets_->standardModelPayload()->originalBytes())return rejected;
        const auto* prototype=source.geometry240110->prototype1C();
        const auto* payload=source.geometry240110->payload();
        if(!prototype||prototype==&owner||!payload||!payload->originalBytes()||
           *payload->originalBytes()!=*piece.assets_->standardModelPayload()->originalBytes())return rejected;
        const OriginalRacerModelSceneOwner50LA* protoScene=nullptr;
        const rr::assets::OriginalStandardMaterialReferences50LA* protoMaterials=nullptr;
        SceneGraph49P protoGraph{};
        for(std::size_t ci=0;ci<storage_->count&&!protoScene;++ci){auto& c=storage_->collections[ci];
            for(std::size_t pi=0;pi<c.sourceSlots38_&&!protoScene;++pi){auto& other=c.pieces_[pi];
                if(&other==&piece||&other.modelOwner_!=prototype)continue;
                if(other.modelGeometry_!=source.geometry240110||!other.modelMaterials_.complete())return rejected;
                protoScene=&other.modelOwner_;protoMaterials=&other.modelMaterials_;protoGraph=other.assets_->graph();}}
        for(std::size_t i=0;i<externalCount&&!protoScene;++i)
            if(external[i].scene==prototype){
                if(!external[i].materials||!external[i].materials->complete()||!external[i].graph.nodes)return rejected;
                protoScene=external[i].scene;protoMaterials=external[i].materials;protoGraph=external[i].graph;}
        if(!protoScene)return rejected; //Not a creator this racer holds: no prototype authority.
        // Row capacity: the prototype's rows must equal this piece's authored rows.
        if(protoScene->binding_.sourceRowCount!=piece.sceneCount_||!piece.sceneCount_||
           piece.sceneCount_>rr::assets::maxModelNodes49O)return rejected;
        progress.warmModelLoadAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        auto& domain=storage_->prefixDomain;
        auto collectRows=[&]()->bool {
            for(std::size_t row=0;row<piece.sceneCount_;++row){
                auto* actual=owner.wreckRowStorage(static_cast<std::uint32_t>(row));
                if(!actual||!actual->reference00)continue;
                piece.componentSlots_[1u+row]=actual;
                bool present=false;for(std::size_t i=0;i<domain.count;++i)present=present||domain.objects[i]==actual;
                if(present)continue;
                if(domain.count>=storage_->prefixComponents.size())return false; //Refused before the write.
                storage_->prefixComponents[domain.count++]=actual;
            }
            return true;
        };
        // 26E9F0 publish, 26E9F4..26EA00 retain (signed64, association re-checked inside).
        piece.modelGeometry_=source.geometry240110;source.geometry240110.reset();
        fields.geometry1B4=piece.modelGeometry_.get();
        rr::assets::OriginalStandardResourceDiagnostic50LA rd;
        if(!piece.modelGeometry_->retain104960(&owner,rd))return finish(S::modelLoadRequired,rd.sourcePC?rd.sourcePC:0x26ea00u);
        auto graph=piece.assets_->graph();
        OriginalRacerModelSceneOwner50LA::R rows;
        const bool separatePrototype=(protoScene->model_.flags191&2u)!=0;
        if(!separatePrototype&&piece.sceneCount_==1u)
            rows=owner.loadWarmChildlessRows26EAC4(graph,*protoScene,protoGraph);   //26EAC4
        else rows=owner.loadWarmHierarchy26EA28(graph,*protoScene,protoGraph);      //26EA9C / 26EB10
        if(!collectRows())return finish(S::capacity,0x28e5f0u);
        if(!rows.okay())return finish(S::components,rows.sourcePC);
        rr::assets::OriginalStandardMaterialDiagnostic50LA md;
        const bool cloned=piece.modelMaterials_.clone26EB44(*protoMaterials,md);
        if(piece.modelMaterials_.started()){
            fields.materialCount1C0=static_cast<std::int32_t>(*piece.modelMaterials_.count1C0());
            fields.materials1C4=piece.modelMaterials_.allocationBase1C4();}
        if(!cloned)return finish(S::modelLoadRequired,md.sourcePC?md.sourcePC:0x26eb44u);
        auto step=owner.copyParameters2406E0(*protoScene);                              //26EBBC virtualB0
        if(!step.okay())return finish(S::modelLoadRequired,step.sourcePC);
        step=owner.allocateSourceRowTable26EE9C();                                      //26EBC4 -> 26EE9C
        if(!step.okay())return finish(S::modelLoadRequired,step.sourcePC);
        const auto packets=piece.modelPackets_.construct240740(owner,*piece.modelGeometry_,false);
        if(!packets.okay())return finish(S::modelLoadRequired,packets.sourcePC);
        return finish(S::modelAppendRequired,0x165cf0u); //Same receipt as the fresh26EE1C loader.
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstOrderedPrefix(
        const OriginalWreckConstructorPrefixBinding50LA& binding)noexcept {
        return constructOrderedPiecePrefix(0,0,binding);
    }
    OriginalWreckConstructorPrefixResult50LA constructNextOrderedPrefix(
        const OriginalWreckConstructorPrefixBinding50LA& binding)noexcept {
        if(!sourceIdentityBound()||!storage_->count)return {};
        return constructOrderedPiecePrefix(0,firstCollection().completedPieceConstructors_,binding);
    }
    OriginalWreckConstructorPrefixResult50LA loadFirstFreshModel26EE1C(
        std::uint64_t sessionGeneration,std::uint64_t racerKey,
        rr::assets::OriginalStandardMaterialDomain50LA& application,
        const rr::assets::OriginalStandardMaterialPath50LA& path)noexcept {
        return loadFreshModelOccurrence26EE1C(0,0,sessionGeneration,racerKey,application,path);
    }
    //165CF0..165D8C: append the actual loaded model, then consume its
    // current local translation. No inventory preview substitutes for28B180.
    OriginalWreckConstructorPrefixResult50LA appendOccurrenceModel165CF0(
        std::size_t collectionIndex,std::size_t pieceIndex,
        std::uint64_t sessionGeneration,std::uint64_t racerKey)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        const auto* vectors=storage_->inventory->constructorVectors();
        if(!vectors)return {};
        return appendModelCore165CF0(*selected,sessionGeneration,racerKey,vectors->zero433B10.data());
    }
private:
    OriginalWreckConstructorPrefixResult50LA appendModelCore165CF0(OriginalWreckPieceStorage50LA& selected,
        std::uint64_t sessionGeneration,std::uint64_t racerKey,const void* zeroWords)noexcept {
        auto& progress=selected.progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.modelAppendAttempted||
           sessionGeneration!=storage_->sessionGeneration||racerKey!=storage_->racerKey||
           progress.prefixResult.status!=S::modelAppendRequired||progress.prefixResult.sourcePC!=0x165cf0u||
           !storage_->actualAttachmentLock423F58)return rejected;
        auto& piece=selected;
        if(!zeroWords||!piece.modelOwner_.fields().packets1F0||
           !piece.modelOwner_.fields().geometry1B4||!piece.modelMaterials_.complete())return rejected;
        progress.modelAppendAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,
            *piece.componentSlots_[0],*piece.componentSlots_[1],0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)
            return finish(S::components,appended.sourcePC);
        auto graph=piece.assets_->graph();const auto model=piece.assets_->model14C();
        // Source compares X, then Y, then Z to433B10. W never participates.
        std::array<float,4> zeroValue;std::memcpy(zeroValue.data(),zeroWords,16);
        bool sentinel=true;
        for(unsigned lane=0;lane<3u&&sentinel;++lane)
            sentinel=piece.piece_.authored50.v[lane]==zeroValue[lane];
        if(sentinel){
            //28B180 copies local F0/F4/F8 only, without refreshing world100.
            const auto& local=graph.nodes[static_cast<std::int32_t>(model)].localC0;
            std::memcpy(piece.piece_.authored50.v,local.v+12,3u*sizeof(float));
        }
        SceneDisplacement49P zero;
        std::memcpy(zero.v,zeroWords,sizeof zero.v);
        //28AB90 preserves localFC/basis and performs actual28C4F0/289A20
        // invalidation. The authored50 copy and append survive a rejection.
        if(!setLocalTranslation49P(graph,model,zero))return finish(S::physical,0x165d8cu);
        //165DA8 calls165E90. The separate setup continuation admits actual
        // B4/B8 zone/world identities before entering D0/gravity.
        return finish(S::pieceSetupRequired,0x165e90u);
    }
public:
    // Reached165E90 setup: retain its actual zone inputs, then construct and
    // append D0/D4 in source order. No zone membership is published here.
    OriginalWreckConstructorPrefixResult50LA setupOccurrenceBody165E90(
        std::size_t collectionIndex,std::size_t pieceIndex,
        std::uint64_t sessionGeneration,std::uint64_t racerKey,
        const OriginalWreckSetupZones50LA& zones)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.pieceSetupAttempted||!orderedQueriesColdPrepared()||
           sessionGeneration!=storage_->sessionGeneration||racerKey!=storage_->racerKey||
           progress.prefixResult.status!=S::pieceSetupRequired||progress.prefixResult.sourcePC!=0x165e90u||
           (storage_->nativeSourceMode?(!storage_->nativeGlobals.current()||!storage_->nativeGravity4431C0||
               !zones.nativeVisibilityNode||!zones.nativeCollisionNode):
               (!storage_->actualConstructorGlobals||!storage_->actualGravityVector4431C0))||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           !zones.visibility4C0||!zones.visibilityZoneField||!zones.collision4C4||
           !zones.collisionLifecycle||!zones.collisionZoneField)return rejected;
        auto& piece=*selected;
        const auto& queryWorld=piece.queries_.worldBinding();
        if(zones.collision4C4!=queryWorld.world||zones.collisionLifecycle!=queryWorld.lifecycle||
           &zones.visibility4C0->lifecycle==zones.collisionLifecycle||
           &zones.visibility4C0->portals==zones.collision4C4||
           !*zones.visibilityZoneField||*zones.visibilityZoneField>zones.visibility4C0->lifecycle.zoneCount||
           *zones.visibilityZoneField>zones.visibility4C0->portals.zoneCount||
           !*zones.collisionZoneField||*zones.collisionZoneField>zones.collisionLifecycle->zoneCount||
           *zones.collisionZoneField>zones.collision4C4->zoneCount||piece.setupZonesBound_||
           storage_->prefixDomain.count+2u>storage_->prefixComponents.size())return rejected;
        progress.pieceSetupAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        //141C20 at165EBC is the literal identity return, with no side effect.
        piece.sourceVisibilityNodeB4_=zones.nativeVisibilityNode; //165EC8 actual borrowed node.
        piece.visibilityWorldB4_=zones.visibility4C0;piece.visibilityZoneB4_=*zones.visibilityZoneField;
        piece.sourceCollisionNodeB8_=zones.nativeCollisionNode; //165ED0 actual borrowed node.
        piece.collisionWorldB8_=zones.collision4C4;piece.collisionLifecycleB8_=zones.collisionLifecycle;
        piece.collisionZoneB8_=*zones.collisionZoneField;piece.setupZonesBound_=true;
        const auto body=1u+piece.sceneCount_;
        auto initialize=[&](OriginalWreckComponent50LA& component,K kind){
            const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&component));
            const auto result=initializeOriginalWreckComponent50LA(component,kind,storage_->sessionGeneration,
                key,true,*storage_->actualObjectCount423F60);
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
            storage_->prefixComponents[storage_->prefixDomain.count++]=&component;return true;
        };
        auto append=[&](OriginalWreckComponent50LA& component,std::uint32_t mask){
            return appendOriginalWreckComponent50LA(storage_->prefixDomain,*piece.componentSlots_[0],component,
                mask,*storage_->actualAttachmentLock423F58).status==OriginalWreckComponentStatus50LA::complete;
        };
        auto& d0=*piece.componentSlots_[body];auto& d4=*piece.componentSlots_[body+1u];
        if(!initialize(d0,K::body167920)||
           !(storage_->nativeSourceMode?
               initializeOriginalWreckBody50LA(piece.physical_,d0,storage_->nativeGlobals):
               initializeOriginalWreckBody50LA(piece.physical_,d0,*storage_->actualConstructorGlobals)))
            return finish(S::physical,0x165ee0u);
        if(storage_->nativeSourceMode)piece.sourceBodyD0_=&d0; //165EE8 after167920.
        //D0 slot30 is141C20;1652E0 binds the SAME model cache without a
        //forced refresh, followed by actual320=1 and1653C0(0).
        if(!bindOriginalWreckBodyModel50LA(piece.physical_,piece.assets_->graph(),piece.assets_->model14C()))
            return finish(S::physical,0x165f10u);
        if(!append(d0,0xffffffffu))return finish(S::components,0x165f3cu);
        if(!initialize(d4,K::gravity282670))return finish(S::physical,0x165f58u);
        if(storage_->nativeSourceMode){
            if(!constructOriginalWreckGravity282670(piece.gravity_,d4,storage_->nativeGravity4431C0))
                return finish(S::physical,0x165f58u);
            piece.sourceGravityD4_=&d4; //165F60 before282750/282790.
            if(!bindOriginalWreckGravity282750(piece.gravity_,piece.physical_))
                return finish(S::physical,0x165f70u);
        }else if(!initializeOriginalWreckGravity50LA(piece.gravity_,piece.physical_,d4,
            *storage_->actualGravityVector4431C0))return finish(S::physical,0x165f58u);
        if(!append(d4,0u))return finish(S::components,0x165f94u);
        //Subsequent source filename scratch consumes the already retained
        //authentic COL member. Stop before E0 allocation/construction166018.
        return finish(S::publishedLifetimeRequired,0x166018u);
    }
    // Actual E0 Entity and11B190/11B570 setup, stopping before11BBC0.
    // No model-domain row or source shape allocation is claimed here.
    OriginalWreckConstructorPrefixResult50LA constructOccurrenceE0To11BBC0(
        std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.e0ConstructorAttempted||progress.retirementAttempted||
           progress.prefixResult.status!=S::publishedLifetimeRequired||progress.prefixResult.sourcePC!=0x166018u||
           !storage_->actualObjectCount423F60||!orderedQueriesColdPrepared())return rejected;
        auto& piece=*selected;
        if(storage_->nativeSourceMode&&(!piece.sourceBodyD0_||
           *piece.sourceBodyD0_!=piece.componentSlots_[1u+piece.sceneCount_]||!piece.sourceGravityD4_||
           *piece.sourceGravityD4_!=piece.componentSlots_[2u+piece.sceneCount_]))return rejected;
        if(!piece.queries_.canConstructE0Prefix11B190()||
           storage_->prefixDomain.count>=storage_->prefixComponents.size())return rejected;
        auto& e0=*piece.componentSlots_[3u+piece.sceneCount_];
        progress.e0ConstructorAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        const auto initialized=initializeOriginalWreckComponent50LA(e0,K::collision11B190,
            storage_->sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e0)),
            true,*storage_->actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::components,initialized.sourcePC);
        storage_->prefixComponents[storage_->prefixDomain.count++]=&e0;
        if(!piece.queries_.constructE0Prefix11B190(e0,storage_->nativeSourceMode?&piece.sourceE0_:nullptr))return finish(S::components,0x11b190u);
        return finish(S::queryPublicationRequired,0x166074u);
    }
    // Complete this SAME E0's11BBC0, then11F570, then actual append166098.
    // No DC allocation1660A0 or callback/registration/activation is performed.
    OriginalWreckConstructorPrefixResult50LA publishPrepareAppendOccurrenceE0(
        std::size_t collectionIndex,std::size_t pieceIndex,SpringSceneId48N actualModelIdentity)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.e0PublicationAttempted||progress.e0PrefixRetired||progress.retirementAttempted||
           progress.prefixResult.status!=S::queryPublicationRequired||progress.prefixResult.sourcePC!=0x166074u||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           !original_wreck_component_detail50LA::domain(storage_->prefixDomain))return rejected;
        auto& piece=*selected;auto& e0=*piece.componentSlots_[3u+piece.sceneCount_];
        if(storage_->nativeSourceMode&&(!piece.sourceE0_||*piece.sourceE0_!=&e0))return rejected;
        progress.e0PublicationAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;
        };
        const auto loaded=piece.queries_.publishFullE0Type2(actualModelIdentity);
        if(loaded.status!=OriginalWreckQueryStatus50LA::complete)return finish(S::queryConstructionFailed,loaded.sourcePC);
        const auto prepared=piece.queries_.prepareFullE011F570();
        if(prepared.status!=OriginalWreckQueryStatus50LA::complete)return finish(S::queryConstructionFailed,prepared.sourcePC);
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,*piece.componentSlots_[0],e0,
            0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::queryConstructionFailed,appended.sourcePC);
        return finish(S::contactConstructorRequired,0x1660a0u);
    }
    OriginalWreckConstructorPrefixResult50LA constructOccurrenceContactSuffix(
        std::size_t collectionIndex,std::size_t pieceIndex,
        const OriginalWreckContactConstructorBinding50LA& binding)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.contactSuffixAttempted||progress.e0PrefixRetired||progress.retirementAttempted||
           progress.prefixResult.status!=S::contactConstructorRequired||progress.prefixResult.sourcePC!=0x1660a0u||
           !binding.actualVector443550||!binding.actualVector4435A0||!binding.actualTerrain330Field||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           *storage_->actualAttachmentLock423F58||storage_->prefixDomain.count+2u>storage_->prefixComponents.size())return rejected;
        auto& piece=*selected;auto& p=piece.physical_;
        const auto body=1u+piece.sceneCount_;auto& dc=*piece.componentSlots_[body+3u];
        auto& d8=*piece.componentSlots_[body+4u];auto& root=*piece.componentSlots_[0];
        if(storage_->nativeSourceMode&&(!piece.sourceBodyD0_||*piece.sourceBodyD0_!=piece.componentSlots_[body]||
           !piece.sourceE0_||*piece.sourceE0_!=piece.componentSlots_[body+2u]))return rejected;
        const auto scene=piece.queries_.modelSceneIdentity();
        const auto* type2=piece.queries_.query(0).type2.get();
        if(!type2||!type2->sourceOwners.loaded())return rejected;
        const auto resource=type2->sourceOwners.resource();
        if(!scene.value||!resource.nodes||!resource.rays||!resource.nodeCount||!resource.rayCount||
           piece.pointStack_.empty()||piece.pointStack_.size()<resource.nodeCount+resource.rayCount||
           p.contactConstructorFieldsApplied||!original_wreck_component_detail50LA::domain(storage_->prefixDomain)||
           dc.reference00||d8.reference00||dc.kind!=K::unbound||d8.kind!=K::unbound||
           dc.objectKey||d8.objectKey||dc.parent1C||dc.previous10||dc.next14||dc.firstChild18||
           d8.parent1C||d8.previous10||d8.next14||d8.firstChild18)return rejected;
        progress.contactSuffixAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        auto initialize=[&](OriginalWreckComponent50LA& c,K kind){
            const auto r=initializeOriginalWreckComponent50LA(c,kind,storage_->sessionGeneration,
                static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&c)),true,*storage_->actualObjectCount423F60);
            if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
            storage_->prefixComponents[storage_->prefixDomain.count++]=&c;return true;
        };
        if(!p.pointRecords.selectSource()||!p.records.selectSource())std::terminate();
        if(!initialize(dc,K::contact295A40))return finish(S::contactConstructionFailed,0x1660b4u);
        progress.dcReached=true;
        if(!initializeOriginalWreckContactConstructor295A40_50LA(p,dc,*binding.actualVector4435A0))std::terminate();
        if(storage_->nativeSourceMode)piece.sourceDC_=&dc; //1660BC after295A40.
        p.spring.generatedCapacity88=50; //295C20 before allocations80 then8C
        if(!p.pointRecords.allocate295C20())return finish(S::contactConstructionFailed,0x295c40u);
        if(!p.records.allocate295C20())return finish(S::contactConstructionFailed,0x295c58u);
        p.contactBody30=&p.body; //295C80, before296190 POINT generation
        auto appendPoint=[&](const SpringVector4_48N& point){
            auto& state=p.spring;if(state.pointCount7C>=50u)return;
            for(std::size_t i=0;i<state.pointCount7C;++i){const auto& old=p.pointRecords[i];
                if(old.localPoint10.v[0]==point.v[0]&&old.localPoint10.v[1]==point.v[1]&&
                   old.localPoint10.v[2]==point.v[2]&&old.scene40.value==scene.value)return;}
            auto& next=p.pointRecords[state.pointCount7C];next.localPoint10=point;next.scene40=scene;
            next.flags50=static_cast<std::uint8_t>((next.flags50&~1u)|1u);next.margin48=0.f;
            next.normal20=*binding.actualVector4435A0;next.surfacePoint30=*binding.actualVector4435A0;
            ++state.pointCount7C;
        };
        std::size_t top=0,remaining=resource.nodeCount+resource.rayCount;
        piece.pointStack_[top++]=resource.root;
        while(top){
            if(!remaining--)return finish(S::contactConstructionFailed,0x296308u);
            const auto tag=piece.pointStack_[--top];
            if(tag&type2Leaf49X){const auto ray=static_cast<std::size_t>(tag&~type2Leaf49X);
                if(ray>=resource.rayCount)return finish(S::contactConstructionFailed,0x295fd0u);
                appendPoint(resource.rays[ray].second);
            }else{
                if(tag>=resource.nodeCount||top+2u>piece.pointStack_.size())return finish(S::contactConstructionFailed,0x296308u);
                piece.pointStack_[top++]=resource.nodes[tag].children[1];
                piece.pointStack_[top++]=resource.nodes[tag].children[0];
            }
        }
        applyOriginalWreckContact1660F0_50LA(p);
        auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,root,dc,0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::contactConstructionFailed,0x166164u);
        if(!initialize(d8,K::detector295290))return finish(S::contactConstructionFailed,0x166180u);
        progress.d8Reached=true;auto& d=piece.detector_;
        d.contact30=nullptr;d.terrain34=nullptr;d.query38=nullptr;
        d.center40=*binding.actualVector443550;d.half50=*binding.actualVector443550;
        d.enabled64=0;d.queryEnabled68=1;d.reset6C=1;d.count70=0;d.objects74=nullptr;d.pairs60=nullptr;
        if(storage_->nativeSourceMode)piece.sourceD8_=&d8; //166188 after295290.
        d.contact30=&p;d.terrain34=*binding.actualTerrain330Field;
        if(!d.terrain34)p.generated.pointContacts48=false; //genuine2953D8 null terrain
        d.enabled64=1; //1661B8;2953A0 a3/t0=0 allocates no query or pair array
        if(!piece.queries_.bindFullContact2955F0(dc,d8))return finish(S::contactConstructionFailed,0x2955f0u);
        d.query38=&piece.queries_.query(0);d.constructed=true;
        appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,root,d8,0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::contactConstructionFailed,0x1661d0u);
        return finish(S::e4ConstructorRequired,0x1661d8u);
    }
    // Real E4 Entity11B190,11B570(5),categoryA4, and11B920 type0
    // loader. Stop at1662A0 BEFORE response callback,9C/A0,11F570 or append.
    OriginalWreckConstructorPrefixResult50LA constructOccurrenceE4ToPreparation(
        std::size_t collectionIndex,std::size_t pieceIndex,
        rr::assets::OriginalStandardMaterialDomain50LA& application,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        diagnostic={};
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;diagnostic={};
        if(!sourceIdentityBound()||progress.e4ConstructorAttempted||progress.contactSuffixRetired||progress.retirementAttempted||
           progress.prefixResult.status!=S::e4ConstructorRequired||progress.prefixResult.sourcePC!=0x1661d8u||
           !storage_->actualObjectCount423F60||!application.prepared()||
           application.resolver3E1A70().get()!=storage_->inventory->modelResolverIdentity()||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           storage_->prefixDomain.count>=storage_->prefixComponents.size()||
           !original_wreck_component_detail50LA::domain(storage_->prefixDomain))return rejected;
        auto& piece=*selected;const auto body=1u+piece.sceneCount_;
        auto& e4=*piece.componentSlots_[body+5u];
        if(storage_->nativeSourceMode&&(!piece.sourceDC_||*piece.sourceDC_!=piece.componentSlots_[body+3u]||
           !piece.sourceD8_||*piece.sourceD8_!=piece.componentSlots_[body+4u]))return rejected;
        if(!piece.queries_.canConstructE4Prefix11B190()||e4.reference00||e4.kind!=K::unbound||
           e4.objectKey||e4.parent1C||e4.previous10||e4.next14||e4.firstChild18)return rejected;
        progress.e4ConstructorAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        const auto initialized=initializeOriginalWreckComponent50LA(e4,K::collision11B190,
            storage_->sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e4)),
            true,*storage_->actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::e4ConstructionFailed,initialized.sourcePC);
        storage_->prefixComponents[storage_->prefixDomain.count++]=&e4;
        if(!piece.queries_.constructE4Prefix11B190(e4,storage_->nativeSourceMode?&piece.sourceE4_:nullptr))std::terminate();
        if(!piece.queries_.loadFullE4Type0(application.resolver3E1A70(),diagnostic)){
            const auto pc=piece.queries_.query(1).type0Owners.sourceBoundary();
            return finish(S::e4ConstructionFailed,pc?pc:0x11b920u);
        }
        return finish(S::e4PreparationRequired,0x1662a0u);
    }
    //1662A0 callback/body binding ->11F570/11ED60 -> actual141960 append.
    //Leaves E8 allocation1662E4 and all activation/effect stages unreached.
    OriginalWreckConstructorPrefixResult50LA preparePublishAppendOccurrenceE4(
        std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.e4PublicationAttempted||progress.e4PrefixRetired||progress.contactSuffixRetired||
           progress.retirementAttempted||progress.prefixResult.status!=S::e4PreparationRequired||
           progress.prefixResult.sourcePC!=0x1662a0u||!storage_->actualAttachmentLock423F58||
           *storage_->actualAttachmentLock423F58||!original_wreck_component_detail50LA::domain(storage_->prefixDomain))return rejected;
        auto& piece=*selected;auto& e4=*piece.componentSlots_[6u+piece.sceneCount_];
        if(storage_->nativeSourceMode&&(!piece.sourceE4_||*piece.sourceE4_!=&e4))return rejected;
        if(e4.parent1C||e4.previous10||e4.next14||e4.firstChild18)return rejected;
        progress.e4PublicationAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        const auto prepared=piece.queries_.preparePublishFullE411F570();
        if(prepared.status!=OriginalWreckQueryStatus50LA::complete)return finish(S::e4PreparationFailed,prepared.sourcePC);
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,*piece.componentSlots_[0],e4,
            0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::e4PreparationFailed,appended.sourcePC);
        return finish(S::e8ConstructorRequired,0x1662e4u);
    }
    //E8 append166324 precedes owner/category and11C710. This order matters
    //on allocation failure: the actual reached E8 may already be a child.
    OriginalWreckConstructorPrefixResult50LA constructOccurrenceE8ToMutualFilters(
        std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.e8ConstructorAttempted||progress.e4PrefixRetired||progress.retirementAttempted||
           progress.prefixResult.status!=S::e8ConstructorRequired||progress.prefixResult.sourcePC!=0x1662e4u||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           storage_->prefixDomain.count>=storage_->prefixComponents.size()||
           *storage_->actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max()||
           !original_wreck_component_detail50LA::domain(storage_->prefixDomain))return rejected;
        auto& piece=*selected;auto& e8=*piece.componentSlots_[7u+piece.sceneCount_];
        if(storage_->nativeSourceMode&&(!piece.sourceE4_||
           *piece.sourceE4_!=piece.componentSlots_[6u+piece.sceneCount_]))return rejected;
        if(!piece.queries_.canConstructE8Prefix11B190()||e8.reference00||e8.kind!=K::unbound||e8.objectKey||
           e8.parent1C||e8.previous10||e8.next14||e8.firstChild18)return rejected;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        auto initialized=initializeOriginalWreckComponent50LA(e8,K::collision11B190,storage_->sessionGeneration,
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e8)),true,*storage_->actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)return rejected;
        progress.e8ConstructorAttempted=true;
        storage_->prefixComponents[storage_->prefixDomain.count++]=&e8;
        if(!piece.queries_.constructE8Prefix11B190(e8,storage_->nativeSourceMode?&piece.sourceE8_:nullptr))std::terminate();
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,*piece.componentSlots_[0],e8,
            0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::e8ConstructionFailed,appended.sourcePC);
        const auto loaded=piece.queries_.loadPublishFullE811C710();
        if(loaded.status!=OriginalWreckQueryStatus50LA::complete)return finish(S::e8ConstructionFailed,loaded.sourcePC);
        return finish(S::mutualFiltersRequired,0x166390u);
    }
    //Finish165E90's four ordered source allocations, return through165C30,
    //then perform actual2CB610 derived callback store. Stop before2CB614
    //reads the incoming effect count: null contexts do not justify skipping it.
    OriginalWreckConstructorPrefixResult50LA constructOccurrenceMutualAndDerived(
        std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.mutualConstructionAttempted||progress.e8PrefixRetired||progress.retirementAttempted||
           progress.prefixResult.status!=S::mutualFiltersRequired||progress.prefixResult.sourcePC!=0x166390u||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           !original_wreck_component_detail50LA::domain(storage_->prefixDomain))return rejected;
        auto& piece=*selected;progress.mutualConstructionAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){progress.prefixResult={status,pc,storage_->prefixDomain.count,true};return progress.prefixResult;};
        const auto mutual=piece.queries_.constructFullMutual166390();
        if(mutual.status!=OriginalWreckQueryStatus50LA::complete)return finish(S::mutualFiltersFailed,mutual.sourcePC);
        if(!piece.queries_.bindFullDerived2CB610())return finish(S::mutualFiltersFailed,0x2cb610u);
        return finish(S::effectInputsRequired,0x2cb614u);
    }
    // Actual22C0F8/22C100 incoming count, followed by2CB614/2CB61C.
    // Stop before any effect allocation, including count-zero dust/splash.
    // A genuine null argument selects the source epilogue, but this method
    // does not certify that return, piece append, or effect-route completion.
    OriginalWreckConstructorPrefixResult50LA bindOccurrenceEffectConstructorInputs(
        std::size_t collectionIndex,std::size_t pieceIndex,
        const OriginalWreckPieceEffectInputs50LA& binding)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.retirementAttempted||progress.e8PrefixRetired||
           progress.prefixResult.status!=S::effectInputsRequired||progress.prefixResult.sourcePC!=0x2cb614u||
           !binding.resourceContextField||!binding.particleParentField||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           !original_wreck_component_detail50LA::domain(storage_->prefixDomain))return rejected;
        auto& piece=*selected;
        if(piece.effectInputsCommitted_)return rejected;
        const auto& authored=storage_->inventory->authoring().collections[collectionIndex];
        // The adopted inventory owns this exact occurrence and its PHY words.
        // Missing input addresses are rejected; only their actual values may
        // select either original null branch. No substitute context is made.
        piece.effectInputs_=binding;
        piece.effectResource_=*binding.resourceContextField;
        piece.effectParent_=*binding.particleParentField;
        piece.effects_.count174=authored.pieces[pieceIndex].emitSmoke?authored.maxNumberOfDamageEmitters:0u;
        piece.effectInputsCommitted_=true;
        const bool resourceNull=!piece.effectResource_;
        const bool parentNull=!piece.effectParent_;
        progress.prefixResult={resourceNull||parentNull?S::effectReturnRequired:S::effectConstructorRequired,
            resourceNull?0x2cb8c4u:(parentNull?0x2cb8c8u:0x2cb628u),storage_->prefixDomain.count,true};
        return progress.prefixResult;
    }
    OriginalWreckConstructorPrefixResult50LA constructOccurrencePositiveEffectArrays(
        std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.retirementAttempted||progress.e8PrefixRetired||
           progress.prefixResult.status!=S::effectConstructorRequired||progress.prefixResult.sourcePC!=0x2cb628u||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return rejected;
        auto& piece=*selected;
        if(!piece.effectInputsStillBound()||!piece.effectResource_||!piece.effectParent_||
           piece.effects_.count174>512u||piece.effectArrays_.attempted()||piece.modelOwner_.released())return rejected;
        if(!piece.effects_.count174){
            //2CB62C BLEZ174 ->2CB7E4; delay slot already supplies dust size260.
            // No160/164/170 allocation or smoke/fire loop is reached.
            progress.prefixResult={S::effectDustRequired,0x2cb7e4u,storage_->prefixDomain.count,true};
            return progress.prefixResult;
        }
        const bool ready=piece.effectArrays_.allocateAndFind2CB634(piece.effects_.count174,
            piece.modelOwner_,piece.assets_->graph());
        progress.prefixResult={ready?S::effectChildrenRequired:S::effectArraysFailed,
            piece.effectArrays_.sourceBoundary(),storage_->prefixDomain.count,true};
        return progress.prefixResult;
    }
    //Next actual160[index] child of the selected piece, retained by its external owner. Piece
    //array disposal never owns this emitter or its manager registration.
    OriginalWreckConstructorPrefixResult50LA constructOccurrencePositiveSmoke(
        std::size_t collectionIndex,std::size_t pieceIndex,
        OriginalWreckSmokeOwner50LA& owner,const OriginalWreckSmokeBinding50LA& binding)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.retirementAttempted||progress.e8PrefixRetired||
           (progress.prefixResult.status!=S::effectChildrenRequired&&progress.prefixResult.status!=S::effectNextPairRequired)||
           progress.prefixResult.sourcePC!=0x2cb6ccu)return rejected;
        auto& piece=*selected;
        if(!piece.effectInputsStillBound()||owner.context_!=piece.effectResource_||
           binding.particleParent!=piece.effectParent_||binding.domain==&storage_->prefixDomain||
           binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
           !binding.domain||binding.domain->sessionGeneration!=storage_->sessionGeneration||
           !piece.effectArrays_.smoke160_||piece.effectArrays_.pairsStopped_>=piece.effects_.count174||
           piece.effectArrays_.smokeWritten_!=piece.effectArrays_.pairsStopped_||
           piece.effectArrays_.fireWritten_!=piece.effectArrays_.pairsStopped_||
           piece.effectArrays_.anchorsWritten_!=piece.effects_.count174||!owner.admit(binding))return rejected;
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]->objectKey==binding.componentKey)return rejected;
        // Same source racer7FC forest across pieces, so admission sees prior IDs.
        if(!sameExternalEffectForest(binding))return rejected;
        if(piece.effectArrays_.externalDomain_&&
           (piece.effectArrays_.externalDomain_!=binding.domain||
            piece.effectArrays_.externalStorage_!=binding.domainStorage||
            piece.effectArrays_.externalCapacity_!=binding.domainCapacity))return rejected;
        // All pairs use the very same retained external forest. Earlier children
        // must still be live; their array entries are borrows, not new owners.
        for(std::size_t i=0;i<piece.effectArrays_.pairsStopped_;++i){
            const auto* a=piece.effectArrays_.smoke160_[i];const auto* b=piece.effectArrays_.fire164_[i];
            if(!original_wreck_component_detail50LA::contains(*binding.domain,a)||
               !original_wreck_component_detail50LA::contains(*binding.domain,b)||
               a->parent1C!=binding.particleParent||b->parent1C!=binding.particleParent)return rejected;
        }
        if(!owner.construct19A5E0(binding))return rejected;
        piece.effectArrays_.externalDomain_=binding.domain;
        piece.effectArrays_.externalStorage_=binding.domainStorage;
        piece.effectArrays_.externalCapacity_=binding.domainCapacity;
        piece.effectArrays_.smoke160_[piece.effectArrays_.smokeWritten_++]=&owner.component_; //2CB700, before setup
        const bool ready=owner.setupAppend19A720();
        progress.prefixResult={ready?S::effectFireRequired:S::effectSmokeFailed,
            owner.sourceBoundary(),storage_->prefixDomain.count,true};return progress.prefixResult;
    }
    //Complete164[index], then the two actual source Stop84 calls in smoke/fire
    //order. The source loop advances only after both stops; dust remains a boundary.
    OriginalWreckConstructorPrefixResult50LA constructOccurrencePositiveFireAndStops(
        std::size_t collectionIndex,std::size_t pieceIndex,
        OriginalWreckSmokeOwner50LA& smoke,OriginalWreckFireOwner50LA& fire,
        const OriginalWreckFireBinding50LA& binding)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.retirementAttempted||progress.e8PrefixRetired||
           progress.prefixResult.status!=S::effectFireRequired||progress.prefixResult.sourcePC!=0x2cb720u)return rejected;
        auto& piece=*selected;auto& arrays=piece.effectArrays_;
        if(!piece.effectInputsStillBound()||fire.context_!=piece.effectResource_||
           smoke.context_!=piece.effectResource_||!smoke.appended_||smoke.retired_||
           smoke.component_.reference00!=1u||smoke.component_.kind!=OriginalWreckComponentKind50LA::effect19A5E0||
           arrays.pairsStopped_>=piece.effects_.count174||
           arrays.smokeWritten_!=arrays.pairsStopped_+1u||arrays.fireWritten_!=arrays.pairsStopped_||!arrays.fire164_||
           arrays.smoke160_[arrays.pairsStopped_]!=&smoke.component_||
           binding.domain!=smoke.binding_.domain||binding.domainStorage!=smoke.binding_.domainStorage||
           binding.domainCapacity!=smoke.binding_.domainCapacity||binding.particleParent!=piece.effectParent_||
           binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||
           binding.controlOwnerId==smoke.controls_.id||!fire.admit(binding))return rejected;
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]->objectKey==binding.componentKey)return rejected;
        if(smoke.component_.parent1C!=binding.particleParent||smoke.component_.next14||
           binding.domainStorage[binding.domain->count-1]!=&smoke.component_)return rejected;
        if(!fire.construct15DE00(binding))return rejected;
        arrays.fire164_[arrays.fireWritten_++]=&fire.component_; //2CB758 before setup
        if(!fire.setupAppend15DEF0()){
            progress.prefixResult={S::effectFireFailed,fire.sourceBoundary(),storage_->prefixDomain.count,true};return progress.prefixResult;}
        //Both actual pointers are nonnull. Their slot84 is15AD00, unlike the
        //separate deferred explosion profile. Perform smoke first, then fire.
        stopEffect50BC(smoke.controls_.state);smoke.boundary_=0x2cb798u; //2CB794
        stopEffect50BC(fire.controls_.state);fire.boundary_=0x2cb7c0u; //2CB7BC
        ++arrays.pairsStopped_; //2CB7C8/7CC, offset+=4 and index++
        const bool more=arrays.pairsStopped_<piece.effects_.count174;
        progress.prefixResult={more?S::effectNextPairRequired:S::effectDustRequired,
            more?0x2cb6ccu:0x2cb7e0u,storage_->prefixDomain.count,true};return progress.prefixResult;
    }
    OriginalWreckConstructorPrefixResult50LA constructOccurrencePositiveDust(
        std::size_t collectionIndex,std::size_t pieceIndex,
        OriginalWreckDustOwner50LA& dust,const OriginalWreckDustBinding50LA& binding)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.retirementAttempted||progress.e8PrefixRetired||
           progress.prefixResult.status!=S::effectDustRequired||
           (progress.prefixResult.sourcePC!=0x2cb7e0u&&progress.prefixResult.sourcePC!=0x2cb7e4u))return rejected;
        auto& piece=*selected;auto& arrays=piece.effectArrays_;
        if(!piece.effectInputsStillBound()||dust.context_!=piece.effectResource_||piece.dust168_||
           arrays.pairsStopped_!=piece.effects_.count174||
           arrays.smokeWritten_!=arrays.pairsStopped_||arrays.fireWritten_!=arrays.pairsStopped_||
           (piece.effects_.count174&&(binding.domain!=arrays.externalDomain_||binding.domainStorage!=arrays.externalStorage_||
               binding.domainCapacity!=arrays.externalCapacity_))||binding.particleParent!=piece.effectParent_||
           !sameExternalEffectForest(binding)||
           binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||!dust.admit(binding))return rejected;
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]->objectKey==binding.componentKey)return rejected;
        for(std::size_t i=0;i<arrays.pairsStopped_;++i)
            if(!original_wreck_component_detail50LA::contains(*binding.domain,arrays.smoke160_[i])||
               !original_wreck_component_detail50LA::contains(*binding.domain,arrays.fire164_[i]))return rejected;
        if(piece.effects_.count174){
            const auto* last=arrays.fire164_[arrays.pairsStopped_-1u];
            if(last->parent1C!=binding.particleParent||last->next14||
               binding.domainStorage[binding.domain->count-1u]!=last)return rejected;
        }else if(arrays.attempted_||arrays.smoke160_||arrays.fire164_||arrays.anchors170_||
                 progress.prefixResult.sourcePC!=0x2cb7e4u)return rejected;
        if(!dust.construct19A5E0(binding))return rejected;
        arrays.externalDomain_=binding.domain;arrays.externalStorage_=binding.domainStorage;
        arrays.externalCapacity_=binding.domainCapacity;
        piece.dust168_=&dust.component_; //2CB800, before setup
        if(!dust.setupStop19A720()){
            progress.prefixResult={S::effectDustFailed,dust.sourceBoundary(),storage_->prefixDomain.count,true};return progress.prefixResult;}
        piece.effects_.dustActive178=0;piece.effects_.dustTime17C=0.f; //2CB83C/840
        const bool ready=dust.append141960();
        progress.prefixResult={ready?S::effectSplashRequired:S::effectDustFailed,
            dust.sourceBoundary(),storage_->prefixDomain.count,true};return progress.prefixResult;
    }
    OriginalWreckConstructorPrefixResult50LA constructOccurrencePositiveSplash(
        std::size_t collectionIndex,std::size_t pieceIndex,
        OriginalWreckDustOwner50LA& dust,OriginalWreckSplashOwner50LA& splash,
        const OriginalWreckSplashBinding50LA& binding)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.retirementAttempted||progress.e8PrefixRetired||
           progress.prefixResult.status!=S::effectSplashRequired||progress.prefixResult.sourcePC!=0x2cb858u)return rejected;
        auto& piece=*selected;
        if(!piece.effectInputsStillBound()||splash.context_!=piece.effectResource_||
           dust.context_!=piece.effectResource_||piece.dust168_!=&dust.component_||piece.splash16C_||
           !dust.appended_||dust.retired_||dust.component_.reference00!=1u||
           dust.component_.kind!=OriginalWreckComponentKind50LA::effect19A5E0||
           binding.domain!=dust.binding_.domain||binding.domainStorage!=dust.binding_.domainStorage||
           binding.domainCapacity!=dust.binding_.domainCapacity||binding.particleParent!=piece.effectParent_||
           binding.actualObjectCount423F60!=storage_->actualObjectCount423F60||
           binding.actualAttachmentLock423F58!=storage_->actualAttachmentLock423F58||!splash.admit(binding))return rejected;
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i)
            if(storage_->prefixComponents[i]->objectKey==binding.componentKey)return rejected;
        if(dust.component_.parent1C!=binding.particleParent||dust.component_.next14||
           binding.domainStorage[binding.domain->count-1u]!=&dust.component_)return rejected;
        if(!splash.construct19DBE0(binding))return rejected;
        piece.splash16C_=&splash.component_; //2CB874, before setup
        if(!splash.setupStop19DCD0()){
            progress.prefixResult={S::effectSplashFailed,splash.sourceBoundary(),storage_->prefixDomain.count,true};return progress.prefixResult;}
        const bool ready=splash.append141960();
        progress.prefixResult={ready?S::effectReturnRequired:S::effectSplashFailed,
            splash.sourceBoundary(),storage_->prefixDomain.count,true};return progress.prefixResult;
    }
    // The selected real piece has returned from2CB5A0. This source append is
    // distinct from constructor allocation and never means all pieces exist.
    OriginalWreckConstructorPrefixResult50LA appendOccurrencePiece22C1B8(
        std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.retirementAttempted||progress.e8PrefixRetired||
           progress.appended22C1B8||progress.prefixResult.status!=S::effectReturnRequired||
           (progress.prefixResult.sourcePC!=0x2cb8c4u&&progress.prefixResult.sourcePC!=0x2cb8c8u)||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return rejected;
        return appendPieceCore22C1B8(*selected,collectionIndex,pieceIndex);
    }
private:
    //Fresh selected first/second occurrence: actual2CB5A0 return at2CB8C4
    //and caller reload of SAME collection38[source index] at22C1A8.
    OriginalWreckConstructorPrefixResult50LA advanceFreshPieceCursor22C25C(std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& c=storage_->collections[0];auto& progress=p.progress_;
        const auto* query=p.queries_.storage_.get();
        if(!sourceIdentityBound()||storage_->nativeSourcePrefixRetired||c.retirementAttempted_||
           progress.retirementAttempted||progress.pieceRetired||!progress.appended22C1B8||
           progress.prefixResult.status!=S::stopped||progress.prefixResult.sourcePC!=0x22c25cu||
           !query||!query->freshCollectionAttempted||query->freshCollectionCursor!=2u||query->freshCollectionPC!=0x22c25cu||
           !c.freshPieceIndexKnown_||!c.freshPieceByteOffset_||*c.freshPieceByteOffset_!=4u*pieceIndex||
           c.completedPieceConstructors_!=pieceIndex||
           c.sourceSlots38_!=pieceIndex+1u||!c.count_||!c.sourceArray38_||c.sourceArray38_!=c.ordered38_.get()||
           c.ordered38_[pieceIndex]!=&p||p.components_[0].parent1C!=&c.collectionComponent_||p.components_[0].next14||
           (!pieceIndex&&(c.collectionComponent_.firstChild18!=&p.components_[0]||p.components_[0].previous10))||
           (pieceIndex==1u&&freshLiveSecondSibling(c.pieces_[0])!=&p.components_[0])||
           !p.effectInputsStillBound()||!preflightFreshConstructorOnlyRetirement())return {};
        //Native capacity above is not the source loop comparison operand.
        *c.freshPieceByteOffset_+=4u; //22C25C, BEFORE source s0 increment.
        ++c.completedPieceConstructors_; //22C260, SAME canonical s0.
        const std::uint32_t count30=c.sourceCount30_; //22C268 actual Config-produced field.
        const std::int64_t signedCount=(count30&0x80000000u)?
            static_cast<std::int64_t>(count30)-0x100000000ll:static_cast<std::int64_t>(count30);
        const bool more=static_cast<std::int64_t>(c.completedPieceConstructors_)<signedCount; //22C26C signedSLT.
        progress.prefixResult={more?S::nextPieceRequired:S::groupConstructorRequired,
            more?0x22c0a0u:0x22c278u,storage_->prefixDomain.count,true};
        return progress.prefixResult; //No next Config selection, allocation or group construction.
    }
    OriginalWreckConstructorPrefixResult50LA applyFreshCollectionWater22C21C(
        const std::uint32_t& actualNamedWaterBit,std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& collection=storage_->collections[0];
        if(!sourceIdentityBound()||storage_->nativeSourcePrefixRetired||progress.retirementAttempted||
           !progress.appended22C1B8||progress.prefixResult.status!=S::collectionFiltersRequired||
           progress.prefixResult.sourcePC!=0x22c21cu||collection.completedPieceConstructors_!=pieceIndex||
           collection.sourceSlots38_!=pieceIndex+1u||!collection.sourceArray38_||
           collection.sourceArray38_!=collection.ordered38_.get()||collection.ordered38_[pieceIndex]!=&p||
           p.components_[0].parent1C!=&collection.collectionComponent_||p.components_[0].next14||
           (!pieceIndex&&(p.components_[0].previous10||collection.collectionComponent_.firstChild18!=&p.components_[0]))||
           !p.effectInputsStillBound()||!preflightFreshConstructorOnlyRetirement())return {};
        if(pieceIndex==1u){
            const auto* firstQuery=collection.pieces_[0].queries_.storage_.get();
            if(freshLiveSecondSibling(collection.pieces_[0])!=&p.components_[0]||
               !firstQuery||!firstQuery->freshCollectionAttempted||firstQuery->freshCollectionCursor!=2u||
               firstQuery->freshCollectionPC!=0x22c25cu||firstQuery->freshNamedWaterBit!=&actualNamedWaterBit)return {};
        }
        const auto result=p.queries_.constructFreshCollectionFilters22C21C(actualNamedWaterBit);
        //Index1 native admission refusal has not entered the Query producer;
        //retain the genuine reached caller boundary, not a source PC of zero.
        const auto pc=pieceIndex==1u&&!result.sourcePC?0x22c21cu:result.sourcePC;
        progress.prefixResult={result.status==OriginalWreckQueryStatus50LA::complete?S::stopped:S::collectionFiltersFailed,
            pc,storage_->prefixDomain.count,true};
        return progress.prefixResult; //22C25C BEFORE actual source index/byte-offset advancement.
    }
    OriginalWreckConstructorPrefixResult50LA appendFreshOccurrencePiece22C1B8(std::size_t pieceIndex=0u)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        auto* selected=freshRetirementPiece(pieceIndex);if(!selected)return {};
        auto& p=*selected;auto& progress=p.progress_;auto& c=storage_->collections[0];
        if(!sourceIdentityBound()||storage_->nativeSourcePrefixRetired||c.retirementAttempted_||
           progress.retirementAttempted||progress.modelRetirementAttempted||progress.pieceRetired||
           progress.e0PrefixRetired||progress.contactSuffixRetired||progress.e4PrefixRetired||progress.e8PrefixRetired||
           progress.appended22C1B8||progress.prefixResult.status!=S::stopped||progress.prefixResult.sourcePC!=0x2cb8c4u||
           c.sourceSlots38_!=pieceIndex+1u||c.completedPieceConstructors_!=pieceIndex||!c.sourceArray38_||
           c.sourceArray38_!=c.ordered38_.get()||c.ordered38_[pieceIndex]!=&p||
           (!pieceIndex&&c.collectionComponent_.firstChild18)||
           !p.componentSlots_||p.componentSlots_[0]!=&p.components_[0]||
           p.components_[0].parent1C||p.components_[0].previous10||p.components_[0].next14||
           !p.freshDustOwner_||!p.freshSplashOwner_||p.dust168_!=&p.freshDustOwner_->component_||
           p.splash16C_!=&p.freshSplashOwner_->component_||
           !p.freshDustOwner_->freshReaderComplete_||!p.freshDustOwner_->appended_||
           !p.freshSplashOwner_->freshReaderComplete_||!p.freshSplashOwner_->appended_||
           p.freshSplashOwner_->sourceBoundary()!=0x2cb8c4u||
           p.freshDustOwner_->context_!=p.effectResource_||p.freshSplashOwner_->context_!=p.effectResource_||
           p.freshDustOwner_->binding_.particleParent!=p.effectParent_||
           p.freshSplashOwner_->binding_.particleParent!=p.effectParent_)return {};
        if(pieceIndex==1u){
            const auto& first=c.pieces_[0];const auto* predecessor=first.componentSlots_?first.componentSlots_[0]:nullptr;
            if(!c.freshPieceIndexKnown_||!c.freshPieceByteOffset_||*c.freshPieceByteOffset_!=4u||
               (c.sourceCount30_&0x80000000u)||c.sourceCount30_<=1u||!first.components_||
               predecessor!=&first.components_[0]||!first.progress_.appended22C1B8||
               first.progress_.pieceRetired||first.nativePreBodyRetired_||
               predecessor->reference00!=1u||predecessor->kind!=OriginalWreckComponentKind50LA::piece2CB4B0||
               predecessor->parent1C!=&c.collectionComponent_||predecessor->previous10||predecessor->next14||
               c.collectionComponent_.firstChild18!=predecessor)return {};
        }
        const auto body=1u+p.sceneCount_;
        if(body+3u>=p.componentCount_||!p.sourceBodyD0_||*p.sourceBodyD0_!=p.componentSlots_[body]||
           !p.sourceDC_||*p.sourceDC_!=p.componentSlots_[body+3u]||
           !preflightFreshConstructorOnlyRetirement())return {};
        return appendPieceCore22C1B8(p,0u,pieceIndex);
    }
    //Shared source mutation only. Both callers independently retain their
    //original legacy or fresh source selector/readiness admission above.
    OriginalWreckConstructorPrefixResult50LA appendPieceCore22C1B8(
        OriginalWreckPieceStorage50LA& piece,std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        auto& progress=piece.progress_;
        auto& collection=storage_->collections[collectionIndex];
        auto& object=*piece.componentSlots_[0];auto& container=collection.collectionComponent_;
        if(!piece.effectInputsStillBound()||collection.sourceSlots38_!=pieceIndex+1u||
           !collection.ordered38_||collection.ordered38_[pieceIndex]!=&piece||
           object.kind!=OriginalWreckComponentKind50LA::piece2CB4B0||object.reference00!=1u||
           container.kind!=OriginalWreckComponentKind50LA::collection22BF00||container.reference00!=1u||
           container.parent1C||container.previous10||container.next14||
           !piece.physical_.bodyConstructorFieldsApplied||!piece.physical_.contactConstructorFieldsApplied||
           piece.physical_.contactBody30!=&piece.physical_.body)return rejected;
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,container,object,
            2u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)return rejected;
        progress.appended22C1B8=true;
        piece.physical_.generated.frictionTorqueScaleC4=original_wreck_physical_detail50LA::word(0x3e4ccccdu); //22C1E4
        piece.piece_.changeWindow130=original_wreck_physical_detail50LA::word(0x40000000u); //22C1F4
        piece.physical_.parameters.body.linearVelocityLimitSquared=original_wreck_physical_detail50LA::word(0x49034acfu); //22C208
        // Stop before124160(E0,1) and the actual source-owned WATER arrays.
        // Legacy inline category storage cannot stand in for their allocation.
        progress.prefixResult={S::collectionFiltersRequired,0x22c21cu,storage_->prefixDomain.count,true};
        return progress.prefixResult;
    }
public:
    OriginalWreckConstructorPrefixResult50LA applyOccurrenceCollectionWaterCategories(
        std::size_t collectionIndex,std::size_t pieceIndex,
        const std::uint32_t& actualWater4342A8)noexcept {
        auto* selected=constructionOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!sourceIdentityBound()||progress.retirementAttempted||progress.e8PrefixRetired||
           !progress.appended22C1B8||progress.prefixResult.status!=S::collectionFiltersRequired||
           progress.prefixResult.sourcePC!=0x22c21cu||!actualWater4342A8||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return rejected;
        auto& collection=storage_->collections[collectionIndex];auto& piece=*selected;
        auto& component=*piece.componentSlots_[0];
        if(collection.completedPieceConstructors_!=pieceIndex||!collection.count_||collection.sourceSlots38_!=pieceIndex+1u||
           component.parent1C!=&collection.collectionComponent_||component.next14||
           component.previous10!=(pieceIndex?collection.pieces_[pieceIndex-1u].componentSlots_[0]:nullptr)||
           (!pieceIndex&&collection.collectionComponent_.firstChild18!=&component)||!piece.effectInputsStillBound())return rejected;
        const auto r=piece.queries_.constructFullCollectionFilters22C21C(actualWater4342A8);
        if(r.status==OriginalWreckQueryStatus50LA::binding)return rejected;
        if(r.status!=OriginalWreckQueryStatus50LA::complete){
            progress.prefixResult={S::collectionFiltersFailed,r.sourcePC,storage_->prefixDomain.count,true};return progress.prefixResult;}
        //22C25C/260 advance the real ordered constructor cursor once. The
        //next piece's cold backing is not a completed source constructor.
        collection.completedPieceConstructors_=pieceIndex+1u;
        const bool more=collection.completedPieceConstructors_<collection.count_;
        progress.prefixResult={more?S::nextPieceRequired:S::groupConstructorRequired,
            more?0x22c0a0u:0x22c278u,storage_->prefixDomain.count,true};return progress.prefixResult;
    }
    OriginalWreckConstructorPrefixResult50LA appendFirstModel165CF0(
        std::uint64_t sessionGeneration,std::uint64_t racerKey)noexcept {
        return appendOccurrenceModel165CF0(0,0,sessionGeneration,racerKey);
    }
    OriginalWreckConstructorPrefixResult50LA setupFirstBody165E90(
        std::uint64_t sessionGeneration,std::uint64_t racerKey,
        const OriginalWreckSetupZones50LA& zones)noexcept {
        return setupOccurrenceBody165E90(0,0,sessionGeneration,racerKey,zones);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstE0To11BBC0()noexcept {
        return constructOccurrenceE0To11BBC0(0,0);
    }
    OriginalWreckConstructorPrefixResult50LA publishPrepareAppendFirstE0(SpringSceneId48N actualModelIdentity)noexcept {
        return publishPrepareAppendOccurrenceE0(0,0,actualModelIdentity);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstContactSuffix(
        const OriginalWreckContactConstructorBinding50LA& binding)noexcept {
        return constructOccurrenceContactSuffix(0,0,binding);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstE4ToPreparation(
        rr::assets::OriginalStandardMaterialDomain50LA& application,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        return constructOccurrenceE4ToPreparation(0,0,application,diagnostic);
    }
    OriginalWreckConstructorPrefixResult50LA preparePublishAppendFirstE4()noexcept {
        return preparePublishAppendOccurrenceE4(0,0);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstE8ToMutualFilters()noexcept {
        return constructOccurrenceE8ToMutualFilters(0,0);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstMutualAndDerived()noexcept {
        return constructOccurrenceMutualAndDerived(0,0);
    }
    OriginalWreckConstructorPrefixResult50LA bindFirstEffectConstructorInputs(
        const OriginalWreckPieceEffectInputs50LA& binding)noexcept {
        return bindOccurrenceEffectConstructorInputs(0,0,binding);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstPositiveEffectArrays()noexcept {
        return constructOccurrencePositiveEffectArrays(0,0);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstPositiveSmoke(
        OriginalWreckSmokeOwner50LA& owner,const OriginalWreckSmokeBinding50LA& binding)noexcept {
        return constructOccurrencePositiveSmoke(0,0,owner,binding);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstPositiveFireAndStops(
        OriginalWreckSmokeOwner50LA& smoke,OriginalWreckFireOwner50LA& fire,
        const OriginalWreckFireBinding50LA& binding)noexcept {
        return constructOccurrencePositiveFireAndStops(0,0,smoke,fire,binding);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstPositiveDust(
        OriginalWreckDustOwner50LA& dust,const OriginalWreckDustBinding50LA& binding)noexcept {
        return constructOccurrencePositiveDust(0,0,dust,binding);
    }
    OriginalWreckConstructorPrefixResult50LA constructFirstPositiveSplash(
        OriginalWreckDustOwner50LA& dust,OriginalWreckSplashOwner50LA& splash,
        const OriginalWreckSplashBinding50LA& binding)noexcept {
        return constructOccurrencePositiveSplash(0,0,dust,splash,binding);
    }
    OriginalWreckConstructorPrefixResult50LA appendFirstPiece22C1B8()noexcept {
        return appendOccurrencePiece22C1B8(0,0);
    }
    OriginalWreckConstructorPrefixResult50LA applyFirstCollectionWaterCategories(
        const std::uint32_t& actualWater4342A8)noexcept {
        return applyOccurrenceCollectionWaterCategories(0,0,actualWater4342A8);
    }
    bool retireE8PrefixOccurrence(std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=retirementOccurrence(collectionIndex,pieceIndex);
        if(!selected)return false;
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        if(!sourceIdentityBound()||!progress.e8ConstructorAttempted)return false;
        if(progress.e8PrefixRetired)return true;
        if(progress.retirementAttempted||progress.e4PrefixRetired||!storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           (progress.prefixResult.status!=S::mutualFiltersRequired&&progress.prefixResult.status!=S::e8ConstructionFailed&&
            progress.prefixResult.status!=S::mutualFiltersFailed&&progress.prefixResult.status!=S::effectInputsRequired&&
            progress.prefixResult.status!=S::effectConstructorRequired&&progress.prefixResult.status!=S::effectReturnRequired&&
            progress.prefixResult.status!=S::effectArraysFailed&&progress.prefixResult.status!=S::effectChildrenRequired&&
            progress.prefixResult.status!=S::effectSmokeFailed&&progress.prefixResult.status!=S::effectFireRequired&&
            progress.prefixResult.status!=S::effectFireFailed&&progress.prefixResult.status!=S::effectNextPairRequired&&
            progress.prefixResult.status!=S::effectDustRequired&&
            progress.prefixResult.status!=S::effectDustFailed&&progress.prefixResult.status!=S::effectSplashRequired&&
            progress.prefixResult.status!=S::effectSplashFailed&&progress.prefixResult.status!=S::collectionFiltersRequired&&progress.prefixResult.status!=S::collectionFiltersFailed&&
            progress.prefixResult.status!=S::nextPieceRequired&&progress.prefixResult.status!=S::groupConstructorRequired)||
           !storage_->prefixDomain.count)return false;
        OriginalWreckDomainRows50LA live;if(!liveRows(live))return false;
        OriginalWreckComponentDomain50LA domain{};
        if(!liveRetirementDomain(live,domain))return false;
        auto& piece=*selected;const auto body=1u+piece.sceneCount_;
        auto& root=*piece.componentSlots_[0];auto& e4=*piece.componentSlots_[body+5u];auto& e8=*piece.componentSlots_[body+6u];
        const bool linked=e8.parent1C==&root&&e8.previous10==&e4&&e4.next14==&e8;
        const bool unlinked=!e8.parent1C&&!e8.previous10&&!e4.next14;
        if((!linked&&!unlinked)||e8.next14||!piece.queries_.canRetireFullE811B4A0(e8))return false;
        if(linked)e4.next14=nullptr;e8.next14=nullptr;--e8.reference00;
        if(!piece.queries_.retireFullE811B4A0(e8,*storage_->actualObjectCount423F60))std::terminate();
        progress.e8PrefixRetired=true;return true;
    }
    bool retirePreparedE4Occurrence(std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=retirementOccurrence(collectionIndex,pieceIndex);
        if(!selected)return false;
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        if(!sourceIdentityBound()||!progress.e4PublicationAttempted)return false;
        if(progress.e4PrefixRetired)return true;
        if(progress.retirementAttempted||progress.contactSuffixRetired||
           (progress.e8ConstructorAttempted&&!progress.e8PrefixRetired)||!storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           (progress.prefixResult.status!=S::e8ConstructorRequired&&progress.prefixResult.status!=S::e4PreparationFailed&&
            progress.prefixResult.status!=S::mutualFiltersRequired&&progress.prefixResult.status!=S::e8ConstructionFailed&&
            progress.prefixResult.status!=S::mutualFiltersFailed&&progress.prefixResult.status!=S::effectInputsRequired&&
            progress.prefixResult.status!=S::effectConstructorRequired&&progress.prefixResult.status!=S::effectReturnRequired&&
            progress.prefixResult.status!=S::effectArraysFailed&&progress.prefixResult.status!=S::effectChildrenRequired&&
            progress.prefixResult.status!=S::effectSmokeFailed&&progress.prefixResult.status!=S::effectFireRequired&&
            progress.prefixResult.status!=S::effectFireFailed&&progress.prefixResult.status!=S::effectNextPairRequired&&
            progress.prefixResult.status!=S::effectDustRequired&&
            progress.prefixResult.status!=S::effectDustFailed&&progress.prefixResult.status!=S::effectSplashRequired&&
            progress.prefixResult.status!=S::effectSplashFailed&&progress.prefixResult.status!=S::collectionFiltersRequired&&progress.prefixResult.status!=S::collectionFiltersFailed&&
            progress.prefixResult.status!=S::nextPieceRequired&&progress.prefixResult.status!=S::groupConstructorRequired))return false;
        auto& piece=*selected;const auto body=1u+piece.sceneCount_;
        OriginalWreckDomainRows50LA live;if(!liveRows(live))return false;
        OriginalWreckComponentDomain50LA domain{storage_->sessionGeneration,live.data(),0};
        if(!liveRetirementDomain(live,domain))return false;
        auto& root=*piece.componentSlots_[0];auto& d8=*piece.componentSlots_[body+4u];auto& e4=*piece.componentSlots_[body+5u];
        const bool linked=e4.parent1C==&root&&e4.previous10==&d8&&d8.next14==&e4;
        const bool unlinked=!e4.parent1C&&!e4.previous10&&!d8.next14;
        if((!linked&&!unlinked)||e4.next14||!piece.queries_.canRetireFullE411B4A0(e4))return false;
        if(linked)d8.next14=nullptr; //1424E0 tail unlink; retain source parent/previous words
        e4.next14=nullptr;--e4.reference00; //104970
        if(!piece.queries_.retireFullE411B4A0(e4,*storage_->actualObjectCount423F60))std::terminate();
        progress.e4PrefixRetired=true;return true;
    }
    bool retireE4PrefixOccurrence(std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=retirementOccurrence(collectionIndex,pieceIndex);
        if(!selected)return false;
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        if(!sourceIdentityBound()||!progress.e4ConstructorAttempted||progress.e4PublicationAttempted)return false;
        if(progress.e4PrefixRetired)return true;
        if(progress.retirementAttempted||progress.contactSuffixRetired||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           (progress.prefixResult.status!=S::e4PreparationRequired&&progress.prefixResult.status!=S::e4ConstructionFailed))return false;
        auto& piece=*selected;auto& e4=*piece.componentSlots_[6u+piece.sceneCount_];
        if(!piece.queries_.canRetireE4Prefix11B4A0(e4))return false;
        --e4.reference00; //104970, unlinked leaf; no sibling writes
        if(!piece.queries_.retireE4Prefix11B4A0(e4,*storage_->actualObjectCount423F60))std::terminate();
        progress.e4PrefixRetired=true;return true;
    }
    bool retireContactSuffixOccurrence(std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=retirementOccurrence(collectionIndex,pieceIndex);
        if(!selected)return false;
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        if(!sourceIdentityBound()||!progress.contactSuffixAttempted||!progress.dcReached)return false;
        if(progress.contactSuffixRetired)return true;
        if(progress.e0PrefixRetired||progress.retirementAttempted||
           (progress.e4ConstructorAttempted&&!progress.e4PrefixRetired)||!storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           (progress.prefixResult.status!=S::e4ConstructorRequired&&progress.prefixResult.status!=S::contactConstructionFailed&&
            progress.prefixResult.status!=S::e4PreparationRequired&&progress.prefixResult.status!=S::e4ConstructionFailed&&
            progress.prefixResult.status!=S::e8ConstructorRequired&&progress.prefixResult.status!=S::e4PreparationFailed&&
            progress.prefixResult.status!=S::mutualFiltersRequired&&progress.prefixResult.status!=S::e8ConstructionFailed&&
            progress.prefixResult.status!=S::mutualFiltersFailed&&progress.prefixResult.status!=S::effectInputsRequired&&
            progress.prefixResult.status!=S::effectConstructorRequired&&progress.prefixResult.status!=S::effectReturnRequired&&
            progress.prefixResult.status!=S::effectArraysFailed&&progress.prefixResult.status!=S::effectChildrenRequired&&
            progress.prefixResult.status!=S::effectSmokeFailed&&progress.prefixResult.status!=S::effectFireRequired&&
            progress.prefixResult.status!=S::effectFireFailed&&progress.prefixResult.status!=S::effectNextPairRequired&&
            progress.prefixResult.status!=S::effectDustRequired&&
            progress.prefixResult.status!=S::effectDustFailed&&progress.prefixResult.status!=S::effectSplashRequired&&
            progress.prefixResult.status!=S::effectSplashFailed&&progress.prefixResult.status!=S::collectionFiltersRequired&&progress.prefixResult.status!=S::collectionFiltersFailed&&
            progress.prefixResult.status!=S::nextPieceRequired&&progress.prefixResult.status!=S::groupConstructorRequired)||
           !storage_->prefixDomain.count)return false;
        auto& piece=*selected;const auto body=1u+piece.sceneCount_;
        OriginalWreckDomainRows50LA liveObjects;if(!liveRows(liveObjects))return false;
        OriginalWreckComponentDomain50LA liveDomain{storage_->sessionGeneration,liveObjects.data(),0};
        if(!liveRetirementDomain(liveObjects,liveDomain))return false;
        auto& root=*piece.componentSlots_[0];auto& e0=*piece.componentSlots_[body+2u];
        auto& dc=*piece.componentSlots_[body+3u];auto& d8=*piece.componentSlots_[body+4u];
        if(!piece.queries_.canRetireFullContactSuffix())return false;
        const bool dcLinked=dc.parent1C==&root&&dc.previous10==&e0&&e0.next14==&dc;
        const bool dcUnlinked=!dc.parent1C&&!dc.previous10&&!e0.next14;
        if((!dcLinked&&!dcUnlinked)||dc.reference00!=1u||dc.kind!=K::contact295A40||dc.firstChild18)return false;
        bool d8Linked=false;
        if(progress.d8Reached){
            d8Linked=d8.parent1C==&root&&d8.previous10==&dc&&dc.next14==&d8;
            const bool unlinked=!d8.parent1C&&!d8.previous10&&!dc.next14;
            if((!d8Linked&&!unlinked)||d8.reference00!=1u||d8.kind!=K::detector295290||
               d8.next14||d8.firstChild18||piece.detector_.pairs60)return false;
        }else if(dc.next14)return false;
        // Tail-first1424E0/142590/104970. Neither borrowed DC nor D0 gains
        // or loses a reference merely because a pointer field names it.
        if(progress.d8Reached){
            if(d8Linked){dc.next14=nullptr;}
            d8.next14=nullptr;--d8.reference00;
            d8.kind=K::entity141300;--*storage_->actualObjectCount423F60;
            piece.detector_.constructed=false;++progress.contactSuffixDestroyed;
        }
        if(dcLinked){e0.next14=nullptr;}
        dc.next14=nullptr;--dc.reference00;
        if(!piece.physical_.pointRecords.release295B90()||!piece.physical_.records.release295B90())std::terminate();
        dc.kind=K::entity141300;--*storage_->actualObjectCount423F60;++progress.contactSuffixDestroyed;
        // Source pointers/counts/body and E0 callback words remain stale until
        // E0's own destructor. The inactive full path cannot dispatch them.
        progress.contactSuffixRetired=true;return true;
    }
    bool retirePublishedE0Occurrence(std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=retirementOccurrence(collectionIndex,pieceIndex);
        if(!selected)return false;
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        if(!sourceIdentityBound()||!progress.e0PublicationAttempted||
           (progress.contactSuffixAttempted&&!progress.contactSuffixRetired))return false;
        if(progress.e0PrefixRetired)return true;
        if(progress.retirementAttempted||!storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           (progress.prefixResult.status!=S::contactConstructorRequired&&
            progress.prefixResult.status!=S::queryConstructionFailed&&
            progress.prefixResult.status!=S::e4ConstructorRequired&&progress.prefixResult.status!=S::contactConstructionFailed&&
            progress.prefixResult.status!=S::e4PreparationRequired&&progress.prefixResult.status!=S::e4ConstructionFailed&&
            progress.prefixResult.status!=S::e8ConstructorRequired&&progress.prefixResult.status!=S::e4PreparationFailed&&
            progress.prefixResult.status!=S::mutualFiltersRequired&&progress.prefixResult.status!=S::e8ConstructionFailed&&
            progress.prefixResult.status!=S::mutualFiltersFailed&&progress.prefixResult.status!=S::effectInputsRequired&&
            progress.prefixResult.status!=S::effectConstructorRequired&&progress.prefixResult.status!=S::effectReturnRequired&&
            progress.prefixResult.status!=S::effectArraysFailed&&progress.prefixResult.status!=S::effectChildrenRequired&&
            progress.prefixResult.status!=S::effectSmokeFailed&&progress.prefixResult.status!=S::effectFireRequired&&
            progress.prefixResult.status!=S::effectFireFailed&&progress.prefixResult.status!=S::effectNextPairRequired&&
            progress.prefixResult.status!=S::effectDustRequired&&
            progress.prefixResult.status!=S::effectDustFailed&&progress.prefixResult.status!=S::effectSplashRequired&&
            progress.prefixResult.status!=S::effectSplashFailed&&progress.prefixResult.status!=S::collectionFiltersRequired&&progress.prefixResult.status!=S::collectionFiltersFailed&&
            progress.prefixResult.status!=S::nextPieceRequired&&progress.prefixResult.status!=S::groupConstructorRequired)||
           !storage_->prefixDomain.count)return false;
        auto& piece=*selected;const auto body=1u+piece.sceneCount_;
        OriginalWreckDomainRows50LA live;if(!liveRows(live))return false;
        OriginalWreckComponentDomain50LA domain{storage_->sessionGeneration,live.data(),0};
        if(!liveRetirementDomain(live,domain))return false;
        auto& root=*piece.componentSlots_[0];auto& gravity=*piece.componentSlots_[body+1u];
        auto& e0=*piece.componentSlots_[body+2u];
        const bool appended=e0.parent1C==&root&&e0.previous10==&gravity&&gravity.next14==&e0;
        const bool unlinked=!e0.parent1C&&!e0.previous10&&!gravity.next14;
        if((!appended&&!unlinked)||e0.next14||e0.firstChild18||
           !piece.queries_.canRetireFullE011B4A0(e0))return false;
        //1424E0 tail removal, then142590/104970. Source stale parent/previous
        //links are retained; predecessor.next is genuinely severed first.
        if(appended)gravity.next14=nullptr;
        e0.next14=nullptr;--e0.reference00;
        if(!piece.queries_.retireFullE011B4A0(e0,*storage_->actualObjectCount423F60))std::terminate();
        progress.e0PrefixRetired=true;return true;
    }
    // Unlinked E0 has not reached append166098. Its real104970->11B4A0
    // retirement must precede the earlier D4/D0/model disposal cursor.
    bool retireUnpublishedE0Occurrence(std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=retirementOccurrence(collectionIndex,pieceIndex);
        if(!selected)return false;
        auto& progress=selected->progress_;
        using S=OriginalWreckConstructorPrefixStatus50LA;
        if(!sourceIdentityBound()||!progress.e0ConstructorAttempted)return false;
        if(progress.e0PrefixRetired)return true;
        if(progress.retirementAttempted||
           (progress.prefixResult.status!=S::queryPublicationRequired&&progress.prefixResult.status!=S::queryConstructionFailed)||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           *storage_->actualAttachmentLock423F58)return false;
        auto& piece=*selected;auto& e0=*piece.componentSlots_[3u+piece.sceneCount_];
        if(e0.reference00!=1u||e0.parent1C||e0.firstChild18||e0.previous10||e0.next14||
           !piece.queries_.canRetireE0Prefix11B4A0(e0))return false;
        --e0.reference00;
        if(!piece.queries_.retireE0Prefix11B4A0(e0,*storage_->actualObjectCount423F60))std::terminate();
        progress.e0PrefixRetired=true;return true;
    }
    // Reached142590 child suffix of the complete pre-E0 constructor prefix.
    // This does not pretend the unjoined model destructor or enclosing piece
    // has retired. Original sibling/reference/count writes survive the stop.
    OriginalWreckPrefixRetirementResult50LA retireOrderedPrefixChildrenOccurrence(std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=retirementOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckPrefixRetirementStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckPrefixRetirementResult50LA out;
        if(!sourceIdentityBound()||!storage_->collections[collectionIndex].constructorAttempted_||
           (progress.prefixResult.status!=OriginalWreckConstructorPrefixStatus50LA::publishedLifetimeRequired&&
            !((progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::queryPublicationRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::contactConstructorRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::queryConstructionFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::e4ConstructorRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::e4PreparationRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::e4ConstructionFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::e8ConstructorRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::mutualFiltersRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::mutualFiltersFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectInputsRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectConstructorRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectReturnRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectArraysFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectChildrenRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectSmokeFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectFireRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectFireFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectNextPairRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectDustRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectDustFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectSplashRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::effectSplashFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::collectionFiltersRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::collectionFiltersFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::nextPieceRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::groupConstructorRequired||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::e8ConstructionFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::e4PreparationFailed||
                progress.prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::contactConstructionFailed)&&progress.e0PrefixRetired))||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58)return out;
        if(progress.retirementAttempted)return progress.retirementResult;
        if(*storage_->actualAttachmentLock423F58){out.status=S::attachmentLocked;out.sourcePC=0x1425a8u;return out;}
        auto& piece=*selected;const auto body=1u+piece.sceneCount_;
        auto& root=*piece.componentSlots_[0];auto& model=*piece.componentSlots_[1];
        auto& d0=*piece.componentSlots_[body];auto& gravity=*piece.componentSlots_[body+1u];
        // No generic source destructor is admitted: only this exact private,
        // pre-E4 constructor route has known NULL body28C and the
        // one-target gravity allocation represented by gravity_.
        OriginalWreckDomainRows50LA liveObjects;if(!liveRows(liveObjects)){out.status=S::topology;return out;}
        OriginalWreckComponentDomain50LA liveDomain{storage_->sessionGeneration,liveObjects.data(),0};
        if(!liveRetirementDomain(liveObjects,liveDomain)||
           (progress.appended22C1B8?
               (root.parent1C!=&storage_->collections[collectionIndex].collectionComponent_||
                (root.previous10?(!original_wreck_component_detail50LA::contains(liveDomain,root.previous10)||
                    root.previous10->parent1C!=root.parent1C||root.previous10->next14!=&root):
                    storage_->collections[collectionIndex].collectionComponent_.firstChild18!=&root)):
                (root.parent1C!=nullptr||root.previous10!=nullptr))||
           root.next14||root.firstChild18!=&model||
           model.parent1C!=&root||model.previous10||model.next14!=&d0||
           d0.parent1C!=&root||d0.previous10!=&model||d0.next14!=&gravity||d0.firstChild18||
           gravity.parent1C!=&root||gravity.previous10!=&d0||gravity.next14||gravity.firstChild18||
           d0.kind!=K::body167920||gravity.kind!=K::gravity282670||
           piece.physical_.linkedCount268||piece.gravity_.target50!=&piece.physical_||
           piece.gravity_.count30!=1u||piece.gravity_.capacity34!=1u||
           piece.queries_.modelSceneIdentity().value){out.status=S::topology;return out;}
        progress.retirementAttempted=true;
        if(progress.e0PrefixRetired){out.referencesReleased=1u+progress.contactSuffixDestroyed+(progress.e4PrefixRetired?1u:0u)+(progress.e8PrefixRetired?1u:0u);out.objectsDestroyed=out.referencesReleased;}
        //1424E0 tail recursion first severs predecessor.next and self.next.
        //142590 then repairs adjacent links and104970 decrements exactly once.
        auto releaseLeaf=[&](OriginalWreckComponent50LA& leaf){
            if(leaf.previous10)leaf.previous10->next14=nullptr;
            leaf.next14=nullptr;
            if(leaf.parent1C&&leaf.parent1C->firstChild18==&leaf)
                leaf.parent1C->firstChild18=leaf.next14;
            --leaf.reference00;++out.referencesReleased;
            if(!leaf.reference00){
                //2826D0 frees only its pointer-array backing, not the borrowed
                //body.1679D0->165260 sees actual constructor28C=NULL. Neither
                //route clears canonical numeric fields or stale source links.
                // Their final1413E0 transitions the type and decrements423F60.
                if(leaf.kind==K::body167920)leaf.kind=K::body164ED0;
                // The one-target native gravity slot is embedded, so source
                // pointer-array disposal needs no host heap allocation/free.
                leaf.kind=K::entity141300;--*storage_->actualObjectCount423F60;
                ++out.objectsDestroyed;
            }
        };
        releaseLeaf(gravity);releaseLeaf(d0);
        // The separate loaded-model continuation owns the next actual
        // slot10/slot08 and PSG/cache phases. Piece/collection stay live.
        out.status=S::modelRetirementRequired;out.sourcePC=0x1425fcu;
        progress.retirementResult=out;return out;
    }
    // Continue tail-first142590 only after D4/D0 really retired. This is
    // the actual loaded model240450/26E460/2896D0 path, not null-resource cleanup.
    OriginalWreckPrefixRetirementResult50LA retireLoadedModelOccurrence(std::size_t collectionIndex,std::size_t pieceIndex)noexcept {
        auto* selected=retirementOccurrence(collectionIndex,pieceIndex);
        if(!selected)return {};
        auto& progress=selected->progress_;
        using S=OriginalWreckPrefixRetirementStatus50LA;
        OriginalWreckPrefixRetirementResult50LA rejected;
        if(!sourceIdentityBound()||!progress.retirementAttempted)return rejected;
        if(progress.modelRetirementAttempted)return progress.retirementResult;
        if(progress.retirementResult.status!=S::modelRetirementRequired||
           !storage_->actualAttachmentLock423F58||!storage_->actualObjectCount423F60)return rejected;
        auto& piece=*selected;const auto body=1u+piece.sceneCount_;
        auto& root=*piece.componentSlots_[0];auto& model=*piece.componentSlots_[1];
        if(piece.componentSlots_[body]->reference00||piece.componentSlots_[body+1u]->reference00||
           root.firstChild18!=&model||model.parent1C!=&root||model.previous10||model.next14||
           piece.queries_.modelSceneIdentity().value||!piece.modelGeometry_||
           !piece.modelGeometry_->constructed240110()||!piece.modelMaterials_.complete()||
           !piece.modelPackets_.constructed240740())return rejected;
        OriginalWreckDomainRows50LA completeObjects;if(!liveRows(completeObjects))return rejected;
        OriginalWreckComponentDomain50LA completeDomain{};
        if(!liveRetirementDomain(completeObjects,completeDomain))return rejected;
        auto graph=piece.assets_->graph();
        // The canonical model owner adds its own Entity once. Retired D0/D4
        // and owned PSG rows do not belong in its live outside Entity domain.
        OriginalWreckDomainRows50LA outsideObjects;if(!liveRows(outsideObjects))return rejected;
        OriginalWreckComponentDomain50LA outside{};
        if(!liveRetirementDomain(outsideObjects,outside,&piece))return rejected;
        auto scene=piece.modelOwner_.preflightScene2896D0(graph);
        if(!scene.okay()){rejected.sourcePC=scene.sourcePC;return rejected;}
        scene=piece.modelOwner_.preflightModelReference142590(outside);
        if(!scene.okay()){rejected.sourcePC=scene.sourcePC;return rejected;}
        if(!piece.modelPackets_.preflight240450().okay()||!piece.modelMaterials_.canRelease26E4C0()||
           !piece.modelGeometry_->canReleaseForModel(&piece.modelOwner_))return rejected;
        progress.modelRetirementAttempted=true;
        auto out=progress.retirementResult;
        auto finish=[&](S status,std::uint32_t pc){
            out.status=status;out.sourcePC=pc;progress.retirementResult=out;return out;
        };
        scene=piece.modelOwner_.releaseModelReference142590(outside);
        if(!scene.okay())return finish(S::topology,scene.sourcePC);
        ++out.referencesReleased;
        const auto packets=piece.modelPackets_.release240450();
        if(!packets.okay())return finish(S::modelResource,packets.sourcePC);
        rr::assets::OriginalStandardMaterialDiagnostic50LA material;
        if(!piece.modelMaterials_.release26E4C0(material))return finish(S::modelResource,material.sourcePC);
        rr::assets::OriginalStandardResourceDiagnostic50LA geometry;
        if(!piece.modelGeometry_->release26E598(&piece.modelOwner_,piece.modelMaterials_,geometry))
            return finish(S::modelResource,geometry.sourcePC);
        progress.geometryReferenceReleased=true; //only after this model's real resource104970
        scene=piece.modelOwner_.freeSourceRowTable26E5CC();
        if(!scene.okay())return finish(S::modelResource,scene.sourcePC);
        if(!piece.modelMaterials_.freeAllocation26E5EC())return finish(S::modelResource,0x26e5ecu);
        scene=piece.modelOwner_.releaseSceneBase2896D0(graph);
        if(!scene.okay())return finish(S::topology,scene.sourcePC);
        // Actual2896D0 performs these child reference/count transitions using
        // PSG sibling order; these totals only report the completed operations.
        out.referencesReleased+=piece.sceneCount_-1u;out.objectsDestroyed+=piece.sceneCount_;
        // PieceB4/B8 still borrow their worlds. Model retirement is not the
        // enclosing2CB500/165BD0/1413E0 lifetime boundary; do not clear them.
        return finish(S::pieceRetirementRequired,0x2cb500u);
    }
    bool retireFirstE8Prefix()noexcept {return retireE8PrefixOccurrence(0,0);}
    bool retireFirstPreparedE4()noexcept {return retirePreparedE4Occurrence(0,0);}
    bool retireFirstE4Prefix()noexcept {return retireE4PrefixOccurrence(0,0);}
    bool retireFirstContactSuffix()noexcept {return retireContactSuffixOccurrence(0,0);}
    bool retireFirstPublishedE0()noexcept {return retirePublishedE0Occurrence(0,0);}
    bool retireFirstUnpublishedE0()noexcept {return retireUnpublishedE0Occurrence(0,0);}
    OriginalWreckPrefixRetirementResult50LA retireFirstOrderedPrefixChildren()noexcept {return retireOrderedPrefixChildrenOccurrence(0,0);}
    OriginalWreckPrefixRetirementResult50LA retireFirstLoadedModel()noexcept {return retireLoadedModelOccurrence(0,0);}
    // Actual source piece end, separate from collection22BF50. Earlier
    // child/model retirement remains required; this never marks it complete.
    OriginalWreckPrefixRetirementResult50LA retirePieceOccurrence(
        std::size_t collectionIndex,std::size_t pieceIndex,
        const LiveZoneWorld50EN& actualCollisionWorld)noexcept {
        using S=OriginalWreckPrefixRetirementStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckPrefixRetirementResult50LA rejected;
        if(!sourceIdentityBound()||collectionIndex>=storage_->count)return rejected;
        auto& collection=storage_->collections[collectionIndex];
        if(pieceIndex>=collection.sourceSlots38_||collection.sourceSlots38_>collection.count_)return rejected;
        auto& piece=collection.pieces_[pieceIndex];auto& progress=piece.progress_;
        if(progress.pieceRetired)return progress.retirementResult;
        if(!collection.ordered38_||!progress.modelRetirementAttempted||progress.retirementResult.status!=S::pieceRetirementRequired||
           collection.retirementAttempted_||!storage_->actualObjectCount423F60||!*storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58||
           !orderedQueriesColdPrepared())return rejected;
        auto& object=*piece.componentSlots_[0];auto& container=collection.collectionComponent_;
        if(collection.ordered38_[pieceIndex]!=&piece||!piece.modelOwner_.released()||!piece.modelPackets_.released()||
           !piece.modelMaterials_.released26E4C0()||!piece.modelGeometry_||!progress.geometryReferenceReleased||
           piece.modelGeometry_->holdsModelReference(&piece.modelOwner_)||
           !piece.setupZonesBound_||piece.collisionWorldB8_!=&actualCollisionWorld||object.reference00!=1u||
           container.reference00!=1u||object.kind!=K::piece2CB4B0||container.kind!=K::collection22BF00||
           object.firstChild18||object.next14||(!collection.nativeRacerTailUnlinked_&&(container.parent1C||container.previous10))||container.next14||
           collection.groupComponent_.reference00||(collection.groupComponent_.kind!=K::unbound&&!(collection.nativeGroupRetired_&&collection.groupComponent_.kind==K::entity141300)))return rejected;
        // Source1424E0 reaches later siblings first. A later reached but
        // unfinished/unlinked occurrence must also finish its own lifetime.
        for(std::size_t i=pieceIndex+1u;i<collection.sourceSlots38_;++i){
            const auto& later=collection.pieces_[i];
            if(!later.progress_.pieceRetired||later.componentSlots_[0]->reference00)return rejected;
        }
        // Validate the complete actual collection child chain in authored
        // order, excluding only genuinely retired or never-appended pieces.
        auto* node=container.firstChild18;OriginalWreckComponent50LA* previous=nullptr;
        for(std::size_t i=0;i<collection.sourceSlots38_;++i){
            const auto& member=collection.pieces_[i];
            if(collection.ordered38_[i]!=&member)return rejected;
            if(member.progress_.pieceRetired){if(member.componentSlots_[0]->reference00)return rejected;continue;}
            if(!member.progress_.appended22C1B8)continue;
            const auto* expected=member.componentSlots_[0];
            if(node!=expected||node->parent1C!=&container||node->previous10!=previous||!node->reference00)return rejected;
            previous=node;node=node->next14;
        }
        if(node)return rejected;
        if(progress.appended22C1B8){
            if(object.parent1C!=&container||previous!=&object)return rejected;
        }else if(object.parent1C||object.previous10)return rejected;
        // Only THIS occurrence's actual children must be retired. Live older
        // siblings are not discarded or mistaken for this piece's children.
        for(std::size_t i=1;i<piece.componentCount_;++i)
            if(!piece.componentSlots_[i]||piece.componentSlots_[i]->reference00)return rejected;
        const auto& query=piece.queries_;
        if(!query.prepared()||!(query.hasFullE0PublicationHistory()?
           query.canReleaseSourceRetiredWorld(actualCollisionWorld):query.canReleaseUnpublishedWorld(actualCollisionWorld)))return rejected;
        auto out=progress.retirementResult;
        //142590 children already retired; sever the actual tail's previous14
        //or parent18 before104970. Source parent/previous and24 remain history.
        if(progress.appended22C1B8){
            if(object.previous10)object.previous10->next14=nullptr;
            else container.firstChild18=nullptr;
        }
        --object.reference00;++out.referencesReleased;
        if(piece.effectArrays_.attempted()&&!piece.effectArrays_.release2CB500())std::terminate();
        piece.effectArrays_.externalDomain_=nullptr;piece.effectArrays_.externalStorage_=nullptr;
        object.kind=K::entity141300;--*storage_->actualObjectCount423F60;++out.objectsDestroyed;
        piece.visibilityWorldB4_=nullptr;piece.collisionWorldB8_=nullptr;
        piece.collisionLifecycleB8_=nullptr;piece.setupZonesBound_=false;
        piece.actualWaterB0_=nullptr;piece.actualWaterGlobals_=nullptr;
        piece.effectInputs_={};piece.effectResource_=nullptr;piece.effectParent_=nullptr;
        piece.dust168_=nullptr;piece.splash16C_=nullptr;
        out.status=S::pieceRetired;out.sourcePC=0x141418u;
        progress.retirementResult=out;progress.pieceRetired=true;return out;
    }
    //22BF50 owns only collection38. Every reached piece has already passed
    //its own concrete retirement. Group construction remains gated separately.
    OriginalWreckPrefixRetirementResult50LA retireCollectionOccurrence(std::size_t collectionIndex)noexcept {
        using S=OriginalWreckPrefixRetirementStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckPrefixRetirementResult50LA rejected;
        if(!sourceIdentityBound()||collectionIndex>=storage_->count)return rejected;
        auto& collection=storage_->collections[collectionIndex];
        if(collection.retirementAttempted_)return collection.retirementResult_;
        auto& container=collection.collectionComponent_;
        if(!collection.constructorAttempted_||!collection.sourceSlots38_||collection.sourceSlots38_>collection.count_||
           !collection.ordered38_||container.reference00!=1u||container.kind!=K::collection22BF00||
           container.firstChild18||(!collection.nativeRacerTailUnlinked_&&(container.parent1C||container.previous10))||container.next14||
           collection.groupComponent_.reference00||(collection.groupComponent_.kind!=K::unbound&&!(collection.nativeGroupRetired_&&collection.groupComponent_.kind==K::entity141300))||
           !storage_->actualObjectCount423F60||!*storage_->actualObjectCount423F60||
           !storage_->actualAttachmentLock423F58||*storage_->actualAttachmentLock423F58)return rejected;
        for(std::size_t i=0;i<collection.count_;++i){const auto& piece=collection.pieces_[i];
            if(i<collection.sourceSlots38_&&collection.ordered38_[i]!=&piece)return rejected;
            if(i<collection.sourceSlots38_&&(!piece.progress_.pieceRetired||
               piece.progress_.retirementResult.status!=S::pieceRetired))return rejected;
            for(std::size_t j=0;j<piece.componentCount_;++j){
                const auto* component=piece.componentSlots_[j];
                // Unreached PSG child rows have no Entity allocation yet.
                // Their NULL slots are genuine cold backing, not retired rows.
                if(!component){if(i<collection.sourceSlots38_||j<2u||j>=1u+piece.sceneCount_)return rejected;}
                else if(component->reference00||(i>=collection.sourceSlots38_&&component->kind!=K::unbound))return rejected;
            }
        }
        collection.retirementAttempted_=true;
        --container.reference00;
        collection.ordered38_.reset(); //22BF84, never releases array pointees
        container.kind=K::entity141300;--*storage_->actualObjectCount423F60;
        collection.retirementResult_={S::prefixRetired,0x22bf90u,1u,1u};
        return collection.retirementResult_;
    }
    // Compatibility entry point: compose the two real receipts without
    //overwriting the completed piece receipt with enclosing collection totals.
    OriginalWreckPrefixRetirementResult50LA retireFirstPieceAndCollection(
        const LiveZoneWorld50EN& actualCollisionWorld)noexcept {
        using S=OriginalWreckPrefixRetirementStatus50LA;
        if(!storage_)return {};
        auto& collection=firstCollection();
        if(collection.sourceSlots38_!=1u)return {}; //legacy sole-piece aggregate receipt only
        if(collection.retirementAttempted_){auto out=collection.retirementResult_;
            out.referencesReleased+=firstProgress().retirementResult.referencesReleased;
            out.objectsDestroyed+=firstProgress().retirementResult.objectsDestroyed;return out;}
        const auto piece=retirePieceOccurrence(0u,0u,actualCollisionWorld);
        if(piece.status!=S::pieceRetired)return piece;
        auto out=retireCollectionOccurrence(0u);
        if(out.status!=S::prefixRetired)return piece;
        out.referencesReleased+=piece.referencesReleased;out.objectsDestroyed+=piece.objectsDestroyed;return out;
    }
    bool coldBackingPrepared()const noexcept{return bool(storage_);}
    std::uint64_t sessionGeneration()const noexcept{return storage_?storage_->sessionGeneration:0;}
    std::uint64_t racerKey()const noexcept{return storage_?storage_->racerKey:0;}
    std::size_t plannedCollectionCount()const noexcept{return storage_?storage_->count:0;}
    const OriginalWreckCollectionStorage50LA* collection(std::size_t i)const noexcept {
        return storage_&&i<storage_->count?&storage_->collections[i]:nullptr;
    }
};
} // namespace rr::game
