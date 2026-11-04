#pragma once
#include "original_wreck_rig_inventory50la.h"
#include "original_racer_wreck_config50la.h"
#include "../../src/game/original_racer_world_borrow50la.h"
#include "ra_native_wreck_effect_children50la.h"
#include "../../src/game/original_wreck_racer_storage50la.h"
#include "../../src/game/original_application_entities50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include <array>
#include <limits>
#include <memory>
#include <thread>
#include <vector>

namespace ra_source_camera50LA { class OwnershipBridge; }
namespace ra_cold_roster50LA {
// Native cold backing only. Real racer identity and query backing are adopted
// through the same retained bridge; no source racer370/374, source constructor
// or camera/body publication is manufactured.
// Model sources remain unbound until the actual retained provider mount seals.
class WreckBacking final {
    friend class ::ra_source_camera50LA::OwnershipBridge;
public:
    struct Reservation {
        rr::game::OriginalWreckQueries50LA* queries=nullptr;
        const rr::game::OriginalWreckAssets50LA* assets=nullptr;
        std::array<std::uint32_t,3> identities{};
    };
private:
    // Pins precede borrowers so all retained partial prefixes die first.
    std::shared_ptr<rr::game::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> domain_;
    std::array<std::unique_ptr<rr::game::OriginalWreckAssetInventory50LA>,8> inventories_;
    std::array<std::unique_ptr<rr::game::OriginalWreckRacerStorage50LA>,8> storage_;
    std::vector<Reservation> reservations_;
    struct Span {std::size_t begin=0,count=0;};
    std::array<Span,8> spans_{};
    std::size_t racerCount_=0;
    bool identityAttempted_=false,identitiesBound_=false;
    bool queryAttempted_=false,queriesPrepared_=false,retiring_=false,queriesRetired_=false;
    std::array<const rr::game::OriginalWreckComponent50LA*,8> racers_{};
    std::uint64_t generation_=0;
    const std::thread::id thread_=std::this_thread::get_id();
    bool attempted_=false,complete_=false,closed_=false;
    bool modelBindingAttempted_=false,modelSourcesBound_=false;
    std::string failure_;
    //Native custody for the second reached loop iteration. The first iteration's
    //selectedDamage and strings remain at their original addresses under its live
    //effect context; this is not another collection cursor or source count.
    struct FreshSecondPiecePrefix {
        std::optional<std::string> pieceKeyE0,smokeKeyF0,pieceName100,modelName210;
        std::optional<std::uint32_t> emitSmoke31C,selectedDamage;
        bool attempted=false,complete=false,bodyAttempted=false,bodyComplete=false,e0Attempted=false,e0Complete=false;
        bool e0LoadAttempted=false,e0LoadComplete=false,e0PrepareAttempted=false,e0PrepareComplete=false;
        bool dcAttempted=false,dcComplete=false,d8Attempted=false,d8Complete=false;
        bool e4Attempted=false,e4Complete=false,e4PrepareAttempted=false,e4PrepareComplete=false;
        bool e8Attempted=false,e8Complete=false;
        bool mutualAttempted=false,mutualComplete=false;
        bool effectInputsAttempted=false,effectInputsComplete=false;
        bool arraysAttempted=false,arraysComplete=false;
        bool initialChildAttempted=false,initialChildComplete=false;
        bool initialReaderAttempted=false,initialReaderComplete=false;
        bool initialCallerAttempted=false,initialCallerComplete=false;
        bool successorAttempted=false,successorComplete=false;
        bool successorReaderAttempted=false,successorReaderComplete=false;
        bool successorCallerAttempted=false,successorCallerComplete=false;
        bool firstPairStopAttempted=false,firstPairStopped=false;
        bool remainingPairsAttempted=false,remainingPairsComplete=false;
        bool positiveDustAttempted=false,positiveDustComplete=false;
        bool positiveDustReaderAttempted=false,positiveDustReaderComplete=false;
        bool positiveSplashAttempted=false,positiveSplashComplete=false;
        bool positiveSplashReaderAttempted=false,positiveSplashReaderComplete=false;
        bool pieceAppendAttempted=false,pieceAppendComplete=false;
        bool waterAttempted=false,waterComplete=false;
        bool pieceCursorAttempted=false,pieceCursorComplete=false;
    };
    struct FreshSourcePrefix {
        rr::game::NativeRacerWreckConfigReader50LA config;
        rr::game::OriginalRacerEntityCold50LA* racer=nullptr;
        std::optional<std::string> partKeyFD0,partNameFE0;
        std::shared_ptr<const rr::game::OriginalRacerWorldBorrow50LA> collectionWorld;
        std::optional<std::string> pieceKeyE0,smokeKeyF0,pieceName100,modelName210;
        std::optional<std::uint32_t> maxDamage318,emitSmoke31C,selectedDamage;
        std::unique_ptr<NativeWreckEffectChildren> effects;
        bool mutualAttempted=false,mutualComplete=false,effectInputsAttempted=false,effectInputsComplete=false;
        FreshSecondPiecePrefix secondPiece;
        bool groupConstructorAttempted=false,groupConstructorComplete=false;
        bool groupPrimaryAttempted=false,groupPrimaryComplete=false;
        bool groupContextAttempted=false,groupContextComplete=false;
        bool groupSparksAttempted=false,groupSparksComplete=false;
        bool groupSparksSetupAttempted=false,groupSparksSetupComplete=false;
        bool groupSparksConfigAttempted=false,groupSparksConfigComplete=false;
        bool pieceCursorAttempted=false,pieceCursorComplete=false;
        bool waterAttempted=false,waterComplete=false;
        bool pieceAppendAttempted=false,pieceAppendComplete=false;
        bool splashAttempted=false,splashComplete=false,splashReadAttempted=false,splashReadComplete=false;
        bool dustAttempted=false,dustComplete=false,dustReadAttempted=false,dustReadComplete=false;
        bool firstPairStopAttempted=false,firstPairStopped=false,remainingPairsAttempted=false,remainingPairsComplete=false;
        bool fireAttempted=false,fireComplete=false,fireReadAttempted=false,fireReadComplete=false;
        bool arraysAttempted=false,arraysComplete=false,smokeAttempted=false,smokeComplete=false,smokeReadAttempted=false,smokeReadComplete=false;
        bool pieceAttempted=false,pieceComplete=false,bodyAttempted=false,bodyComplete=false,e0Attempted=false,e0Complete=false,e0LoadAttempted=false,e0LoadComplete=false,e0PrepareAttempted=false,e0PrepareComplete=false,dcAttempted=false,dcComplete=false,d8Attempted=false,d8Complete=false,e4Attempted=false,e4Complete=false,e4PrepareAttempted=false,e4PrepareComplete=false,e8Attempted=false,e8Complete=false;
        std::uint32_t pc=0x2129a4u;
        bool attempted=false,complete=false,withdrawn=false;
    };
    std::array<FreshSourcePrefix,8> freshSources_{};
    bool freshSourceSelected_=false,freshSourceRetired_=false;
    bool sourceAttempted_=false,sourceWithdrawn_=false;
    std::uint32_t sourcePC_=0x2129c0;
    rr::game::OriginalRacerEntityCold50LA* sourceRacer_=nullptr;
    std::unique_ptr<NativeWreckEffectChildren> nativeEffects_;

    static rr::game::OriginalWreckInventoryEndpointPolicy50LA endpointPolicy() noexcept {
        // COL contains XYZ only. This explicit HOST opaque-W policy matches
        // the native disc bootstrap; it is not guest heap/file evidence.
        return {nullptr,[](void*,const rr::game::OriginalWreckCollectionAuthoring50LA&,
            const rr::game::OriginalWreckPieceAuthoring50LA&,std::size_t index,
            rr::game::Type2OpaqueEndpointWords49X& out) noexcept {
            if(index>=32768u)return false;
            out={0x80000000u+static_cast<std::uint32_t>(index),
                 0x7fc10000u+static_cast<std::uint32_t>(index)};
            return true;
        }};
    }
    // Called only after the real Race70 copy of same World's admitted NONE
    // provider. Native backing adoption does not emulate guest malloc failure.
    bool loadNativeFirstRacerSource(rr::game::OriginalRacerEntityCold50LA& racer,
        const rr::game::UndertrackPhysicsDomain50EW& heights,
        const std::string& materialDirectory,const void* actualWorld3FC,std::uint32_t& pc)noexcept {
        using namespace rr::game;using S=OriginalWreckConstructorPrefixStatus50LA;
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||freshSourceSelected_||sourceAttempted_||
           !queriesPrepared_||!identitiesBound_||!application_||application_->released_||
           !domain_||!modelSourcesBound_||!storage_[0]||racers_[0]!=&racer.racer||
           racer.nativeWreckLoadAttempted_||racer.sourceTerrain330_||racer.sourceWater334_||actualWorld3FC||
           !racer.prepared()||racer.racer.sessionGeneration!=generation_||
           heights.water334!=UndertrackHeightOwner50EW::Absent||heights.waterProvider334)
            return false;
        sourceAttempted_=true;sourceRacer_=&racer;racer.nativeWreckLoadAttempted_=true;
        try {
            //Actual210804 alias under this private same-Race70 native load
            //adaptation; not permission to replay the full210770 constructor.
            //2107FC precedes210804. The caller supplied the SAME current
            //World3FC, admitted null by actual constructor+Water NONE branch.
            racer.sourceTerrain330_.emplace(static_cast<const OriginalWreckTerrainProvider50LA*>(actualWorld3FC));
            racer.sourceWater334_.emplace(heights.waterProvider334);
            auto& owner=*storage_[0];auto& app=*application_;
            OriginalWreckConstructorGlobalViews50LA globals;
            globals.bodyVector433AD0=app.bodyOrigin433AD0_.data();
            globals.pieceVector433B10=app.wreckAssetVectors_.zero433B10.data();
            globals.reset.vector433B80=app.bodyReset433B80_.data();
            globals.reset.quaternion382B60=app.bodyQuaternion382B60_.data();
            if(!owner.bindNativeConstructorGlobalViews(globals,app.gravityOrigin4431C0_.data()))return false;
            std::uint32_t count=0;
            pc=sourcePC_=0x2129c0;
            if(!owner.nativeWreckCount158920(count))return false;
            racer.sourceWreckCount370_=count;
            pc=sourcePC_=0x2129cc;
            const auto* array=owner.nativeArray374Backing();if(!array)return false;
            pc=sourcePC_=0x2129d4;racer.sourceWreck374_=array;racer.sourceWreck374Published_=true;
            pc=sourcePC_=0x212a2c;
            if(!owner.constructNativeCollection22BF00(0,app.objectCount423F60(),
                app.attachmentLock423F58(),app.types383408()))return false;
            pc=sourcePC_=0x212a4c;
            if(!owner.publishNativeCollection212A4C(0))return false;
            racer.sourceWreck374Slots_=1;
            pc=sourcePC_=0x212a90;
            auto result=owner.loadNativeOccurrencePiece22BFD0(0,&*racer.sourceWater334_,heights.waterGlobals);
            pc=sourcePC_=result.sourcePC;
            if(result.status!=S::modelLoadRequired||result.sourcePC!=0x26ee1cu)return false;
            result=owner.loadFreshModelOccurrence26EE1C(0,0,generation_,racer.racer.objectKey,*domain_,{&materialDirectory});
            pc=sourcePC_=result.sourcePC;if(result.status!=S::modelAppendRequired)return false;
            result=owner.appendOccurrenceModel165CF0(0,0,generation_,racer.racer.objectKey);
            pc=sourcePC_=result.sourcePC;
            //165E90 still requires actual B4/B8 world AND zone-field owners.
            //No chosen host zone, source D0, or camera286D20 is fabricated.
            return result.status==S::pieceSetupRequired&&result.sourcePC==0x165e90u;
        }catch(...){return false;}
    }
    bool loadNativeNextCollectionPrefix(const rr::game::UndertrackPhysicsDomain50EW& heights,
        const std::string& materialDirectory,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||sourcePC_!=0x2129e0u||
           !sourceRacer_||sourceRacer_->nativeWreckWithdrawn_||!storage_[0]||!application_||application_->released_||
           !domain_||!sourceRacer_->sourceWater334_||!sourceRacer_->sourceWreckCount370_||
           !storage_[0]->nativeSource374AliasCurrent(sourceRacer_->sourceWreck374_,
               *sourceRacer_->sourceWreckCount370_,sourceRacer_->sourceWreck374Slots_))return false;
        try {
            auto& owner=*storage_[0];auto& app=*application_;const auto ci=owner.nativeCollectionOrdinal()+1u;
            if(ci!=sourceRacer_->sourceWreck374Slots_||ci>=*sourceRacer_->sourceWreckCount370_)return false;
            pc=sourcePC_=0x212a2cu;
            if(!owner.constructNativeCollection22BF00(ci,app.objectCount423F60(),app.attachmentLock423F58(),app.types383408()))return false;
            pc=sourcePC_=0x212a4cu;if(!owner.publishNativeCollection212A4C(ci))return false;
            ++sourceRacer_->sourceWreck374Slots_; //only the actual new source store
            pc=sourcePC_=0x212a90u;
            auto r=owner.loadNativeOccurrencePiece22BFD0(0,&*sourceRacer_->sourceWater334_,heights.waterGlobals);
            pc=sourcePC_=r.sourcePC;if(r.status!=S::modelLoadRequired||r.sourcePC!=0x26ee1cu)return false;
            r=owner.loadFreshModelOccurrence26EE1C(ci,0,generation_,sourceRacer_->racer.objectKey,*domain_,{&materialDirectory});
            pc=sourcePC_=r.sourcePC;if(r.status!=S::modelAppendRequired)return false;
            r=owner.appendOccurrenceModel165CF0(ci,0,generation_,sourceRacer_->racer.objectKey);
            pc=sourcePC_=r.sourcePC;return r.status==S::pieceSetupRequired&&r.sourcePC==0x165e90u;
        }catch(...){return false;}
    }
    bool setupNativeBody(std::size_t pieceIndex,const rr::game::OriginalWreckSetupZones50LA& zones,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           !sourceAttempted_||sourcePC_!=0x165e90||!sourceRacer_||!storage_[0]||
           sourceRacer_->nativeWreckWithdrawn_||!sourceRacer_->sourceWreckCount370_||
           !storage_[0]->nativeSource374AliasCurrent(sourceRacer_->sourceWreck374_,
               *sourceRacer_->sourceWreckCount370_,sourceRacer_->sourceWreck374Slots_))return false;
        const auto r=storage_[0]->setupOccurrenceBody165E90(storage_[0]->nativeCollectionOrdinal(),pieceIndex,generation_,sourceRacer_->racer.objectKey,zones);
        pc=sourcePC_=r.sourcePC;
        return r.status==S::publishedLifetimeRequired&&r.sourcePC==0x166018;
    }
    bool loadNativeQueryChildren(std::size_t pieceIndex,std::uint32_t& pc)noexcept {
        using namespace rr::game;using S=OriginalWreckConstructorPrefixStatus50LA;
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           !sourceAttempted_||sourcePC_!=0x166018||!sourceRacer_||!storage_[0]||!application_||
           application_->released_||!domain_||!modelSourcesBound_||sourceRacer_->nativeWreckWithdrawn_||
           !sourceRacer_->sourceTerrain330_||*sourceRacer_->sourceTerrain330_)return false;
        auto& owner=*storage_[0];
        auto result=owner.constructOccurrenceE0To11BBC0(owner.nativeCollectionOrdinal(),pieceIndex);pc=sourcePC_=result.sourcePC;
        if(result.status!=S::queryPublicationRequired)return false;
        result=owner.publishNativeOccurrenceE0(pieceIndex);pc=sourcePC_=result.sourcePC;
        if(result.status!=S::contactConstructorRequired)return false;
        result=owner.constructNativeOccurrenceContact(pieceIndex,application_->detectorOrigin443550_.data(),
            application_->springZero4435A0_.data(),&*sourceRacer_->sourceTerrain330_);
        pc=sourcePC_=result.sourcePC;if(result.status!=S::e4ConstructorRequired)return false;
        rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
        result=owner.constructOccurrenceE4ToPreparation(owner.nativeCollectionOrdinal(),pieceIndex,*domain_,diagnostic);
        pc=sourcePC_=result.sourcePC;if(result.status!=S::e4PreparationRequired)return false;
        result=owner.preparePublishAppendOccurrenceE4(owner.nativeCollectionOrdinal(),pieceIndex);pc=sourcePC_=result.sourcePC;
        if(result.status!=S::e8ConstructorRequired)return false;
        result=owner.constructOccurrenceE8ToMutualFilters(owner.nativeCollectionOrdinal(),pieceIndex);pc=sourcePC_=result.sourcePC;
        return result.status==S::mutualFiltersRequired&&result.sourcePC==0x166390;
    }
    bool loadNativeEffects(std::size_t pieceIndex,rr::game::OriginalTrackParticleCold50LA& manager,
        const std::uint32_t& water,std::uint32_t& pc)noexcept {
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           !sourceRacer_||!storage_[0]||sourcePC_!=0x166390||!application_||application_->released_||
           !domain_||!domain_->archiveProvider3E1A70()||
           sourceRacer_->particles4A0_!=manager.particles()||!sourceRacer_->sourceParticle7FC_||
           *sourceRacer_->sourceParticle7FC_!=&sourceRacer_->particle7FC||!manager.material150()||
           manager.material150()->table382B38()!=domain_->materials382B38().get())return false;
        try {
            if(!nativeEffects_)nativeEffects_=std::make_unique<NativeWreckEffectChildren>();
            const bool result=nativeEffects_->load(*storage_[0],*application_,*sourceRacer_,manager,&*sourceRacer_->sourceParticle7FC_,
                *domain_->archiveProvider3E1A70(),water,pieceIndex,pc);
            sourcePC_=pc;return result;
        }catch(...){return false;}
    }
    // Continue only the genuine22C25C/260 cursor after the previous WATER
    // suffix. No next collection/group or alternate primary body is inferred.
    bool loadNativeRemainingCollectionPieces(const rr::game::OriginalWreckSetupZones50LA& zones,
        const rr::game::UndertrackPhysicsDomain50EW& heights,const std::string& materialDirectory,
        rr::game::OriginalTrackParticleCold50LA& manager,const std::uint32_t& water,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           !sourceRacer_||!storage_[0]||!domain_||!nativeEffects_||!sourceRacer_->sourceWater334_||
           sourceRacer_->nativeWreckWithdrawn_)return false;
        try {
            auto& owner=*storage_[0];
            while(sourcePC_==0x22c0a0u){
                const auto* collection=owner.collection(owner.nativeCollectionOrdinal());if(!collection)return false;
                const auto pi=collection->completedPieceConstructorCount();
                if(!pi||pi>=collection->plannedPieceCount()||
                   collection->reachedSourcePieceSlotCount()!=pi)return false;
                auto r=owner.loadNativeOccurrencePiece22BFD0(pi,&*sourceRacer_->sourceWater334_,heights.waterGlobals);
                pc=sourcePC_=r.sourcePC;if(r.status!=S::modelLoadRequired||r.sourcePC!=0x26ee1cu)return false;
                r=owner.loadFreshModelOccurrence26EE1C(owner.nativeCollectionOrdinal(),pi,generation_,sourceRacer_->racer.objectKey,*domain_,{&materialDirectory});
                pc=sourcePC_=r.sourcePC;if(r.status!=S::modelAppendRequired)return false;
                r=owner.appendOccurrenceModel165CF0(owner.nativeCollectionOrdinal(),pi,generation_,sourceRacer_->racer.objectKey);
                pc=sourcePC_=r.sourcePC;if(r.status!=S::pieceSetupRequired)return false;
                if(!setupNativeBody(pi,zones,pc)||!loadNativeQueryChildren(pi,pc)||
                   !loadNativeEffects(pi,manager,water,pc))return false;
            }
            return sourcePC_==0x22c278u; //group remains required, never ready
        }catch(...){return false;}
    }
    bool loadNativeCollectionGroup(std::uint32_t& pc)noexcept {
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           sourcePC_!=0x22c278u||!sourceRacer_||!storage_[0]||!nativeEffects_||
           !application_||application_->released_||!domain_||!domain_->archiveProvider3E1A70()||
           sourceRacer_->nativeWreckWithdrawn_)return false;
        if(!storage_[0]->constructNativeGroup22C278(application_->groupOrigin4441F0_.data(),pc)){
            sourcePC_=pc;return false;
        }
        const bool done=nativeEffects_->loadGroup(*storage_[0],*application_,*domain_->archiveProvider3E1A70(),pc);
        sourcePC_=pc;return done;
    }
    bool deactivateNativeCollectionLoad(std::uint32_t& pc)noexcept {
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           sourcePC_!=0x22c2c8u||!sourceRacer_||sourceRacer_->nativeWreckWithdrawn_||
           !storage_[0]||!nativeEffects_)return false;
        const bool done=nativeEffects_->deactivateLoad22CE10(*storage_[0],pc);
        sourcePC_=pc;return done;
    }
    bool appendNativeCollectionToRacer(std::uint32_t& pc)noexcept {
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           sourcePC_!=0x212a98u||!sourceRacer_||sourceRacer_->nativeWreckWithdrawn_||!storage_[0]||
           !sourceRacer_->prepared()||!application_||application_->released_||
           sourceRacer_->actualCounter423F60()!=&application_->objectCount423F60()||
           sourceRacer_->actualLock423F58()!=&application_->attachmentLock423F58())return false;
        const bool done=storage_[0]->appendNativeCollection212AAC(sourceRacer_->racer,pc);sourcePC_=pc;return done;
    }
    template<class AppendMain690> bool bindNativeRacerExclusions(std::uint32_t actual694,
        AppendMain690&& appendMain690,std::uint32_t& pc)noexcept {
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           sourcePC_!=0x212abcu||!sourceRacer_||sourceRacer_->nativeWreckWithdrawn_||!storage_[0])return false;
        try {
            auto& owner=*storage_[0];const auto* c=owner.collection(owner.nativeCollectionOrdinal());
            if(!c||c->reachedSourcePieceSlotCount()!=c->completedPieceConstructorCount())return false;
            for(std::size_t pi=0;pi<c->reachedSourcePieceSlotCount();++pi){std::uint32_t actualE4=0;
                if(!owner.appendNativeOwnPodExclusion(pi,actual694,actualE4,pc)){sourcePC_=pc;return false;}
                sourcePC_=pc;
                if(!appendMain690(actualE4))return false;
                if(!owner.commitNativeMain690Exclusion(pi,pc)){sourcePC_=pc;return false;}
                sourcePC_=pc;
            }
            return sourcePC_==0x2129e0u||sourcePC_==0x212b50u;
        }catch(...){return false;}
    }
    bool continueNativeWreck215AF8(rr::game::OriginalRacerEntityCold50LA& actualRacer,
        rr::assets::NativePhyLoad50LA::Reader& phy,
        std::optional<float>& gravity47C,std::uint32_t& pc)noexcept {
        if(pc!=0x215af8u||sourcePC_!=pc||actualRacer.nativePostEmberPC_!=pc||
           !phy||!actualRacer.nativePhyLoad_||phy.owner()!=actualRacer.nativePhyLoad_)return false;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||
           !sourceAttempted_||freshSourceSelected_||!identitiesBound_||!modelSourcesBound_||
           !actualRacer.prepared()||actualRacer.racer.sessionGeneration!=generation_||racers_[0]!=&actualRacer.racer||
           sourceRacer_!=&actualRacer||actualRacer.nativeWreckWithdrawn_||!storage_[0]||
           !application_||application_->released_||!domain_||!actualRacer.sourceWreckCount370_||
           !storage_[0]->nativeSource374AliasCurrent(actualRacer.sourceWreck374_,
               *actualRacer.sourceWreckCount370_,actualRacer.sourceWreck374Slots_))return false;
        const bool done=storage_[0]->continueNativeTuning215AF8(phy.cursor(),gravity47C,pc);
        sourcePC_=pc;return done;
    }
    bool loadNativeRacer814810(std::uint32_t& pc)noexcept {
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||sourcePC_!=0x212b50u||
           !sourceRacer_||sourceRacer_->nativeWreckWithdrawn_||!storage_[0]||!nativeEffects_||
           !application_||application_->released_||!domain_||!domain_->archiveProvider3E1A70()||
           !sourceRacer_->sourceParticle7FC_||*sourceRacer_->sourceParticle7FC_!=&sourceRacer_->particle7FC||
           sourceRacer_->sourceRacerExplosion814_||sourceRacer_->sourceRacerSparks810_)return false;
        auto publish=[&](bool explosion,rr::game::OriginalWreckComponent50LA* actual){
            auto& alias=explosion?sourceRacer_->sourceRacerExplosion814_:sourceRacer_->sourceRacerSparks810_;
            if(!actual||alias)return false;alias=actual;return true;
        };
        const bool done=nativeEffects_->loadRacer814810(*storage_[0],*application_,*domain_->archiveProvider3E1A70(),publish,pc);
        sourcePC_=pc;return done;
    }
    bool loadNativeRacer818(const std::string& directory,std::uint32_t& actualRaw,std::uint32_t& pc)noexcept{
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||sourcePC_!=0x212c44u||
           !sourceRacer_||sourceRacer_->nativeWreckWithdrawn_||!storage_[0]||!nativeEffects_||
           !application_||application_->released_||!domain_||!domain_->archiveProvider3E1A70()||
           !sourceRacer_->slots818_||sourceRacer_->nativeSource818Published_||
           !sourceRacer_->sourceParticle7FC_||*sourceRacer_->sourceParticle7FC_!=&sourceRacer_->particle7FC)return false;
        for(const auto* slot:*sourceRacer_->slots818_)if(slot)return false;
        auto publish=[&](std::size_t i,rr::game::OriginalWreckComponent50LA* actual){
            if(!actual||i!=sourceRacer_->nativeSource818Published_||i>=sourceRacer_->slots818_->size()||
               (*sourceRacer_->slots818_)[i])return false;
            (*sourceRacer_->slots818_)[i]=actual;++sourceRacer_->nativeSource818Published_;return true;
        };
        const bool done=nativeEffects_->loadRacer818(*storage_[0],*application_,*domain_->archiveProvider3E1A70(),directory,actualRaw,publish,pc);
        sourcePC_=pc;return done;
    }
    bool beginNativeHandling2146(rr::assets::NativePhyLoad50LA::Reader& phy,std::uint32_t& pc)noexcept{
        pc=sourcePC_;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||sourceWithdrawn_||sourcePC_!=0x212dc8u||
           !sourceRacer_||sourceRacer_->nativeWreckWithdrawn_||!nativeEffects_||!nativeEffects_->emberComplete_||
           !application_||application_->released_||!sourceRacer_->slots818_||
           !nativeEffects_->emberAliasesCurrent(sourceRacer_->slots818_->data(),sourceRacer_->nativeSource818Published_))return false;
        const bool done=sourceRacer_->beginNativeHandling2146(phy,pc);sourcePC_=pc;return done;
    }
    bool retireNativeSourcePrefix(const rr::game::LiveZoneWorld50EN& world)noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(!sourceAttempted_||sourceWithdrawn_)return true;
        if(!sourceRacer_||!storage_[0]||!application_||application_->released_||
           sourceRacer_->racer.sessionGeneration!=generation_||sourceRacer_->racer.reference00!=1u||
           sourceRacer_->racer.objectKey!=storage_[0]->racerKey()||
           &sourceRacer_->racer!=racers_[0]||!sourceRacer_->nativeWreckLoadAttempted_)return false;
        if(sourceRacer_->sourceWreck374Published_&&(!sourceRacer_->sourceWreckCount370_||
           !storage_[0]->nativeSource374AliasCurrent(sourceRacer_->sourceWreck374_,
               *sourceRacer_->sourceWreckCount370_,sourceRacer_->sourceWreck374Slots_)))return false;
        if(nativeEffects_&&!nativeEffects_->racerTailAliasesCurrent(sourceRacer_->sourceRacerExplosion814_,
            sourceRacer_->sourceRacerSparks810_))return false;
        if(nativeEffects_&&(!sourceRacer_->slots818_||!nativeEffects_->emberAliasesCurrent(
            sourceRacer_->slots818_->data(),sourceRacer_->nativeSource818Published_)))return false;
        //Once212AAC made this the real Racer tail, source1424E0 retires
        //collection BEFORE earlier7FC children. Keep effect native backing,
        //model attachment history and manager facade alive across that order.
        if(nativeEffects_&&!nativeEffects_->retireDetachedTail(*storage_[0]))return false;
        if(!storage_[0]->retireNativeCollectionPieceSourcePrefix(world))return false;
        if(nativeEffects_&&!nativeEffects_->retireExternal())return false;
        //Withdraw borrow admission, not source words: preserve370/374 and slot
        //history exactly. No future consumer may dereference its dying array.
        sourceRacer_->nativeWreckWithdrawn_=true;sourceRacer_=nullptr;
        sourceWithdrawn_=true;return true;
    }
    //Fresh source chronology is distinct from the late native Follow adapter.
    //Identity adoption is native custody only; no query preparation or330/334
    //republication occurs. A failed bind remains one-way and must withdraw.
    bool selectFreshSourceIdentities(
        const std::array<const rr::game::OriginalWreckComponent50LA*,8>& racers,
        std::size_t count,std::uint64_t generation,std::string& error)noexcept {
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||freshSourceSelected_||
           sourceAttempted_||queryAttempted_||identityAttempted_)return false;
        freshSourceSelected_=true;
        return bindActualRacers(racers,count,generation,error);
    }
    bool loadFreshSourceFirstCollection2129A4(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::vector<std::uint8_t>& actualPhy,std::uint32_t& pc)noexcept {
        pc=0x2129a4u;
        if(thread_!=std::this_thread::get_id()||!freshSourceSelected_||freshSourceRetired_||
           closed_||retiring_||!identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||
           slot>=racerCount_||!storage_[slot]||racers_[slot]!=&racer.racer||
           !application_||application_->released_||!racer.prepared()||
           racer.racer.sessionGeneration!=generation_||racer.nativeWreckLoadAttempted_||racer.nativeWreckWithdrawn_||
           !racer.sourceTerrain330_.has_value()||!racer.sourceWater334_.has_value()||
           racer.sourceWreck374Published_||racer.sourceWreck374_||racer.sourceWreck374Slots_)return false;
        auto& state=freshSources_[slot];if(state.attempted||state.withdrawn)return false;
        auto& owner=*storage_[slot];
        const auto* inventory=owner.storage_?owner.storage_->inventory.get():nullptr;
        const auto* retained=inventory?inventory->retainedSourcePhy():nullptr;
        if(!retained||*retained!=actualPhy)return false;
        //Native retained identity before fallible reader/admission; no future
        //route can silently adopt this prefix or replay earlier source writes.
        state.attempted=true;state.racer=&racer;racer.nativeWreckLoadAttempted_=true;
        try {
            if(!state.config.bindNative(*retained))return false;
            pc=state.pc=0x2129acu;
            if(!state.config.select157D00("Wreckage"))return false;
            pc=state.pc=0x2129c0u;
            if(!state.config.readInteger158920("NumberOfParts",racer.sourceWreckCount370_))return false;
            pc=state.pc=0x2129c8u;
            if(!racer.sourceWreckCount370_||!owner.bindFreshSourceCount158920(racer.racer,*racer.sourceWreckCount370_))return false;
            pc=state.pc=0x2129ccu;
            //Explicit pre-reserved native allocation policy, never literal
            //guest malloc failure: source374 remains absent until this store.
            const auto* array=owner.nativeArray374Backing();if(!array)return false;
            pc=state.pc=0x2129d4u;
            racer.sourceWreck374_=array;racer.sourceWreck374Published_=true;
            //The actual positive count selects the first loop iteration. No
            //unwritten374 element is read, and later ordinals stay unknown.
            pc=state.pc=0x2129ecu;state.partKeyFD0="Part1";
            pc=state.pc=0x2129fcu;if(!state.config.select157D00("Wreckage"))return false;
            pc=state.pc=0x212a10u;
            if(!state.config.read158240(*state.partKeyFD0,state.partNameFE0,0x104u))return false;
            //Part-name destination may remain unknown on a missing key. The
            //22BF00 does not consume it; the separate22BFD0 prefix does.
            pc=state.pc=0x212a18u;
            pc=state.pc=0x212a2cu;
            if(!owner.constructFreshFirstCollection22BF00(application_->objectCount423F60(),
                application_->attachmentLock423F58(),application_->types383408()))return false;
            pc=state.pc=0x212a4cu;if(!owner.publishFreshFirstCollection212A4C())return false;
            racer.sourceWreck374Slots_=owner.freshSourcePublished374();
            state.complete=true;pc=state.pc=0x212a50u;return true;
        }catch(...){pc=state.pc;return false;} //Prior source stores/owners retained; no replay.
    }
    bool loadFreshSourceFirstPiece212A50(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::shared_ptr<const rr::game::OriginalRacerWorldBorrow50LA>& world,
        const std::string& materialDirectory,std::uint32_t& pc)noexcept {
        using namespace rr::game;using S=OriginalWreckConstructorPrefixStatus50LA;
        pc=0x212a50u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||!domain_||
           slot>=racerCount_||!storage_[slot]||racers_[slot]!=&racer.racer||
           !application_||application_->released_||!world||!world->current()||world->generation()!=generation_||
           !racer.prepared()||racer.nativeWreckWithdrawn_||!racer.nativeWreckLoadAttempted_||
           !racer.sourceTerrain330_||!racer.sourceWater334_||*racer.sourceWater334_||
           !racer.sourceWreckCount370_)return false;
        auto& state=freshSources_[slot];auto& owner=*storage_[slot];
        if(!state.complete||state.pc!=0x212a50u||state.racer!=&racer||state.withdrawn||state.pieceAttempted||
           !owner.freshSource374AliasCurrent(racer.sourceWreck374_,*racer.sourceWreckCount370_,racer.sourceWreck374Slots_))return false;
        const auto* actualWorld=world->actualCollectionWorld40();if(!actualWorld)return false;
        state.pieceAttempted=true;state.collectionWorld=world; //Same lease before any source40/model effects.
        try {
            auto& app=*application_;
            OriginalWreckConstructorGlobalViews50LA globals;
            globals.bodyVector433AD0=app.bodyOrigin433AD0_.data();
            globals.pieceVector433B10=app.wreckAssetVectors_.zero433B10.data();
            globals.reset.vector433B80=app.bodyReset433B80_.data();
            globals.reset.quaternion382B60=app.bodyQuaternion382B60_.data();
            pc=state.pc=0x22c02cu;
            if(!owner.beginFreshCollectionLoad22C02C(*actualWorld,globals,app.gravityOrigin4431C0_.data()))return false;
            //22C02C is the157D00 call delay: the real World40 write is already
            //visible when selected-section or later native parsing refuses.
            pc=state.pc=0x22c028u;
            if(!state.partNameFE0||!state.config.select157D00(*state.partNameFE0))return false;
            pc=state.pc=0x22c04cu;
            auto* count=owner.freshCollectionCount30Field158B30();if(!count)return false;
            if(!state.config.readInteger158B30("NumberOfParts",0u,*count))return false;
            pc=state.pc=0x22c064u;
            if(!state.config.readInteger158B30("MaxNumberOfDamageEmitters",4u,state.maxDamage318))return false;
            pc=state.pc=0x22c07cu;
            if(static_cast<std::int32_t>(*count)<=0)return false; //Nonpositive return branch outside this bounded slice.
            pc=state.pc=0x22c08cu;
            if(!owner.publishFreshCollectionArray22C08C())return false;
            pc=state.pc=0x22c0a0u;if(!state.config.select157D00(*state.partNameFE0))return false;
            pc=state.pc=0x22c0b4u;state.pieceKeyE0="Part1";
            pc=state.pc=0x22c0c8u;state.smokeKeyF0="EmitSmoke1";
            pc=state.pc=0x22c0dcu;
            if(!state.config.read158240(*state.pieceKeyE0,state.pieceName100,0x104u))return false;
            pc=state.pc=0x22c0ecu;
            if(!state.config.readInteger158920(*state.smokeKeyF0,state.emitSmoke31C))return false;
            pc=state.pc=0x22c0f4u;if(!state.emitSmoke31C)return false;
            state.selectedDamage=0u;
            if(*state.emitSmoke31C){
                pc=state.pc=0x22c100u;if(!state.maxDamage318)return false;state.selectedDamage=*state.maxDamage318;
            }
            //This local is NOT piece174:2CB61C stores174 only AFTER165C30
            //returns. The pre-body/model prefix must leave174 unknown.
            pc=state.pc=0x22c120u;
            if(!state.pieceName100||!racer.sourcePodName1E0_)return false;
            state.modelName210=std::string("Pods\\")+racer.sourcePodName1E0_->data()+"\\"+*state.pieceName100+".slt";
            pc=state.pc=0x22c13cu;
            auto result=owner.constructFreshFirstPiece2CB4B0(*state.modelName210,
                app.wreckAssetVectors_.position434830,app.wreckAssetVectors_.look434860,app.wreckAssetVectors_.up434850,
                &*racer.sourceWater334_,nullptr);
            pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
            if(result.status!=S::modelLoadRequired||result.sourcePC!=0x26ee1cu)return false;
            result=owner.loadFreshModelOccurrence26EE1C(0,0,generation_,racer.racer.objectKey,*domain_,{&materialDirectory});
            pc=state.pc=result.sourcePC?result.sourcePC:state.pc;if(result.status!=S::modelAppendRequired)return false;
            result=owner.appendFreshOccurrenceModel165CF0();
            pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
            if(result.status!=S::pieceSetupRequired||result.sourcePC!=0x165e90u)return false;
            state.pieceComplete=true;return true;
        }catch(...){pc=state.pc;return false;}
    }
    bool loadFreshSourceSecondBody165E90(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const rr::game::OriginalWreckSetupZones50LA& zones,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x165e90u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        auto& second=state.secondPiece;
        if(!second.complete||state.pc!=0x165e90u||state.racer!=&racer||state.withdrawn||second.bodyAttempted||
           !state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||
           !state.collectionWorld||!state.collectionWorld->current()||!second.modelName210||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!state.collectionWorld->readIncoming210770(generation_,&application_->objectCount423F60(),v,c)||
           v!=racer.binding_.actualIncomingVisibility4C0||c!=racer.binding_.actualIncomingCollision4C4||
           zones.nativeVisibilityNode!=v||zones.nativeCollisionNode!=c)return false;
        second.bodyAttempted=true;
        const auto result=storage_[slot]->setupFreshOccurrenceBody165E90(zones,*second.modelName210,1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::publishedLifetimeRequired||result.sourcePC!=0x166018u)return false;
        second.bodyComplete=true;return true;
    }
    bool loadFreshSourceFirstBody165E90(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const rr::game::OriginalWreckSetupZones50LA& zones,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x165e90u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_)return false;
        auto& state=freshSources_[slot];
        if(!state.pieceComplete||state.pc!=0x165e90u||state.racer!=&racer||state.withdrawn||state.bodyAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||!state.modelName210||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get())return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!state.collectionWorld->readIncoming210770(generation_,&application_->objectCount423F60(),v,c)||
           v!=racer.binding_.actualIncomingVisibility4C0||c!=racer.binding_.actualIncomingCollision4C4||
           zones.nativeVisibilityNode!=v||zones.nativeCollisionNode!=c)return false;
        state.bodyAttempted=true;
        const auto result=storage_[slot]->setupFreshOccurrenceBody165E90(zones,*state.modelName210);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::publishedLifetimeRequired||result.sourcePC!=0x166018u)return false;
        state.bodyComplete=true;return true;
    }
    bool freshSecondE0Reservation166018(std::size_t slot,const rr::game::OriginalRacerEntityCold50LA& racer,
        std::size_t& nativeOrdinal)const noexcept {
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        const auto& state=freshSources_[slot];const auto& second=state.secondPiece;const auto& span=spans_[slot];
        if(!second.bodyComplete||second.e0Attempted||state.pc!=0x166018u||state.racer!=&racer||state.withdrawn||
           !state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        std::size_t index=0;
        rr::game::OriginalWreckRacerStorage50LA::NativeColdReservation actual;
        if(!storage_[slot]->freshSecondE0Reservation166018(index,actual)||index!=1u||index>=span.count||
           span.begin>=reservations_.size()||index>=reservations_.size()-span.begin)return false;
        const auto ordinal=span.begin+index;const auto& row=reservations_[ordinal];
        if(row.queries!=actual.queries||row.assets!=actual.assets)return false;
        nativeOrdinal=ordinal;return true; //Native flattened reserve index, not guest field publication.
    }
    bool loadFreshSourceSecondE0166018(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const rr::game::OriginalWreckQueryWorld50LA& world,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x166018u;
        std::size_t ordinal=0;if(!freshSecondE0Reservation166018(slot,racer,ordinal)||
           world.identities!=reservations_[ordinal].identities)return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        const void* v=nullptr;const void* c=nullptr;
        if(!state.collectionWorld->readIncoming210770(generation_,&application_->objectCount423F60(),v,c)||
           v!=racer.binding_.actualIncomingVisibility4C0||c!=racer.binding_.actualIncomingCollision4C4)return false;
        second.e0Attempted=true;
        const auto result=storage_[slot]->constructFreshOccurrenceE0To11BBC0(world,application_->types383408(),1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::queryPublicationRequired||pc!=0x166074u)return false;
        second.e0Complete=true;return true;
    }
    bool loadFreshSourceFirstE0166018(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const rr::game::OriginalWreckQueryWorld50LA& world,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x166018u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_)return false;
        auto& state=freshSources_[slot];const auto& span=spans_[slot];
        if(!state.bodyComplete||state.pc!=0x166018u||state.racer!=&racer||state.withdrawn||state.e0Attempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!span.count||
           span.begin>=reservations_.size()||world.identities!=reservations_[span.begin].identities)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!state.collectionWorld->readIncoming210770(generation_,&application_->objectCount423F60(),v,c)||
           v!=racer.binding_.actualIncomingVisibility4C0||c!=racer.binding_.actualIncomingCollision4C4)return false;
        state.e0Attempted=true;
        const auto result=storage_[slot]->constructFreshOccurrenceE0To11BBC0(world,application_->types383408());
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::queryPublicationRequired||result.sourcePC!=0x166074u)return false;
        state.e0Complete=true;return true;
    }
    bool loadFreshSourceSecondE0Shape166074(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x166074u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.e0Complete||state.pc!=0x166074u||state.racer!=&racer||state.withdrawn||second.e0LoadAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.e0LoadAttempted=true;
        rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
        const auto result=storage_[slot]->loadFreshOccurrenceE0NativeTransport166074(domain_->resolver3E1A70(),
            application_->observeCollisionDirectory36B0F0(),application_->collisionName3E0730_,diagnostic,1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::e0PreparationRequired||result.sourcePC!=0x16607cu)return false;
        second.e0LoadComplete=true;return true;
    }
    bool prepareAppendFreshSourceSecondE016607C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x16607cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.e0LoadComplete||state.pc!=0x16607cu||state.racer!=&racer||state.withdrawn||second.e0PrepareAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.e0PrepareAttempted=true;
        const auto result=storage_[slot]->prepareAppendFreshE016607C(1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::contactConstructorRequired||result.sourcePC!=0x1660a0u)return false;
        second.e0PrepareComplete=true;return true;
    }
    bool loadFreshSourceFirstE0Shape166074(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x166074u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.e0Complete||state.pc!=0x166074u||state.racer!=&racer||state.withdrawn||state.e0LoadAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.e0LoadAttempted=true;
        rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
        const auto result=storage_[slot]->loadFreshOccurrenceE0NativeTransport166074(domain_->resolver3E1A70(),
            application_->observeCollisionDirectory36B0F0(),application_->collisionName3E0730_,diagnostic);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::e0PreparationRequired||result.sourcePC!=0x16607cu)return false;
        state.e0LoadComplete=true;return true;
    }
    bool prepareAppendFreshSourceFirstE016607C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x16607cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.e0LoadComplete||state.pc!=0x16607cu||state.racer!=&racer||state.withdrawn||state.e0PrepareAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.e0PrepareAttempted=true;
        const auto result=storage_[slot]->prepareAppendFreshE016607C();
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::contactConstructorRequired||result.sourcePC!=0x1660a0u)return false;
        state.e0PrepareComplete=true;return true;
    }
    bool constructFreshSourceSecondDC1660A0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x1660a0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.e0PrepareComplete||state.pc!=0x1660a0u||state.racer!=&racer||state.withdrawn||second.dcAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.dcAttempted=true;
        const auto result=storage_[slot]->constructFreshContactDC1660A0(application_->springZero4435A0_,1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::detectorConstructorRequired||result.sourcePC!=0x16616cu)return false;
        second.dcComplete=true;return true;
    }
    bool constructFreshSourceSecondD816616C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x16616cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.dcComplete||state.pc!=0x16616cu||state.racer!=&racer||state.withdrawn||second.d8Attempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.d8Attempted=true;
        if(!racer.sourceTerrain330_)return false;
        const auto result=storage_[slot]->constructFreshDetectorD816616C(application_->detectorOrigin443550_,
            application_->wreckAssetVectors_.zero433B10,&*racer.sourceTerrain330_,1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::e4ConstructorRequired||result.sourcePC!=0x1661d8u)return false;
        second.d8Complete=true;return true;
    }
    bool constructFreshSourceFirstDC1660A0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x1660a0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.e0PrepareComplete||state.pc!=0x1660a0u||state.racer!=&racer||state.withdrawn||state.dcAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.dcAttempted=true;
        const auto result=storage_[slot]->constructFreshContactDC1660A0(application_->springZero4435A0_);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::detectorConstructorRequired||result.sourcePC!=0x16616cu)return false;
        state.dcComplete=true;return true;
    }
    bool constructFreshSourceFirstD816616C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x16616cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.dcComplete||state.pc!=0x16616cu||state.racer!=&racer||state.withdrawn||state.d8Attempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.d8Attempted=true;
        if(!racer.sourceTerrain330_)return false;
        const auto result=storage_[slot]->constructFreshDetectorD816616C(application_->detectorOrigin443550_,
            application_->wreckAssetVectors_.zero433B10,&*racer.sourceTerrain330_);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::e4ConstructorRequired||result.sourcePC!=0x1661d8u)return false;
        state.d8Complete=true;return true;
    }
    bool constructLoadFreshSourceSecondE41661D8(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x1661d8u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.d8Complete||state.pc!=0x1661d8u||state.racer!=&racer||state.withdrawn||second.e4Attempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.e4Attempted=true;
        if(!second.modelName210)return false;
        rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
        const auto result=storage_[slot]->constructLoadFreshE41661D8(*second.modelName210,application_->types383408(),
            domain_->resolver3E1A70(),application_->observeCollisionDirectory36B0F0(),application_->collisionName3E0730_,diagnostic,1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::e4NativeLoadComplete||result.sourcePC!=0x16629cu)return false;
        second.e4Complete=true;return true;
    }
    bool prepareAppendFreshSourceSecondE416629C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x16629cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.e4Complete||state.pc!=0x16629cu||state.racer!=&racer||state.withdrawn||second.e4PrepareAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.e4PrepareAttempted=true;
        const auto result=storage_[slot]->prepareAppendFreshE416629C(1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::e8ConstructorRequired||result.sourcePC!=0x1662e4u)return false;
        second.e4PrepareComplete=true;return true;
    }
    bool constructLoadFreshSourceFirstE41661D8(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x1661d8u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.d8Complete||state.pc!=0x1661d8u||state.racer!=&racer||state.withdrawn||state.e4Attempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.e4Attempted=true;
        if(!state.modelName210)return false;
        rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
        const auto result=storage_[slot]->constructLoadFreshE41661D8(*state.modelName210,application_->types383408(),
            domain_->resolver3E1A70(),application_->observeCollisionDirectory36B0F0(),application_->collisionName3E0730_,diagnostic);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::e4NativeLoadComplete||result.sourcePC!=0x16629cu)return false;
        state.e4Complete=true;return true;
    }
    bool prepareAppendFreshSourceFirstE416629C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x16629cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.e4Complete||state.pc!=0x16629cu||state.racer!=&racer||state.withdrawn||state.e4PrepareAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.e4PrepareAttempted=true;
        const auto result=storage_[slot]->prepareAppendFreshE416629C();
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::e8ConstructorRequired||result.sourcePC!=0x1662e4u)return false;
        state.e4PrepareComplete=true;return true;
    }
    bool bindFreshSourceSecondEffectInputs2CB618(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        rr::game::OriginalTrackParticleCold50LA& manager,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb618u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.mutualComplete||state.pc!=0x2cb618u||state.racer!=&racer||state.withdrawn||second.effectInputsAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        if(racer.particles4A0_!=manager.particles()||!racer.sourceParticle7FC_||
           *racer.sourceParticle7FC_!=&racer.particle7FC)return false;
        rr::game::OriginalWreckPieceEffectInputs50LA binding;
        if(!state.effects->borrowFreshSharedInputs2CB618(racer,manager,&*racer.sourceParticle7FC_,binding))return false;
        second.effectInputsAttempted=true;
        const auto result=storage_[slot]->bindFreshEffectInputs2CB618(binding,1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::effectConstructorRequired||result.sourcePC!=0x2cb628u)return false;
        second.effectInputsComplete=true;return true;
    }
    bool constructFreshSourceSecondMutual166390(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x166390u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.e8Complete||state.pc!=0x166390u||state.racer!=&racer||state.withdrawn||second.mutualAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.mutualAttempted=true;
        const auto result=storage_[slot]->constructFreshMutualAndCount166390(second.selectedDamage,1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::effectInputsRequired||result.sourcePC!=0x2cb618u)return false;
        second.mutualComplete=true;return true;
    }
    bool constructFreshSourceSecondE81662E4(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x1662e4u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!state.effects||!state.effects->canRetireFreshInputContext()||!second.e4PrepareComplete||state.pc!=0x1662e4u||state.racer!=&racer||state.withdrawn||second.e8Attempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.e8Attempted=true;
        const auto result=storage_[slot]->constructFreshE8To166390(application_->types383408(),1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::mutualFiltersRequired||result.sourcePC!=0x166390u)return false;
        second.e8Complete=true;return true;
    }
    bool constructFreshSourceFirstE81662E4(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x1662e4u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.e4PrepareComplete||state.pc!=0x1662e4u||state.racer!=&racer||state.withdrawn||state.e8Attempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.e8Attempted=true;
        const auto result=storage_[slot]->constructFreshE8To166390(application_->types383408());
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::mutualFiltersRequired||result.sourcePC!=0x166390u)return false;
        state.e8Complete=true;return true;
    }
    bool stopFreshSourceSecondFirstPair2CB778(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb778u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.successorCallerComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb778u||state.racer!=&racer||state.withdrawn||second.firstPairStopAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.firstPairStopAttempted=true;
        const bool done=state.effects->stopFreshSecondFirstPair(*storage_[slot],pc);
        state.pc=pc;
        if(!done||(pc!=0x2cb6ccu&&pc!=0x2cb7e0u))return false;
        second.firstPairStopped=true;return true;
    }
    bool stopFreshSourceFirstPair2CB778(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb778u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.fireReadComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb778u||state.racer!=&racer||state.withdrawn||state.firstPairStopAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.firstPairStopAttempted=true;
        const bool done=state.effects->stopFreshFirstPair(*storage_[slot],pc);
        state.pc=pc;
        if(!done||(pc!=0x2cb6ccu&&pc!=0x2cb7e0u))return false;
        state.firstPairStopped=true;return true;
    }
    bool loadFreshSourceSecondPiece22C0A0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string& materialDirectory,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x22c0a0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70()||
           !racer.sourceTerrain330_||!racer.sourceWater334_||*racer.sourceWater334_||!racer.sourceWreckCount370_)return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;auto& owner=*storage_[slot];
        if(!state.pieceCursorComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x22c0a0u||state.racer!=&racer||state.withdrawn||second.attempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get()||
           !owner.freshSource374AliasCurrent(racer.sourceWreck374_,*racer.sourceWreckCount370_,racer.sourceWreck374Slots_))return false;
        std::size_t index=0;std::uint32_t count=0,offset=0;
        if(!owner.freshSourcePieceCursor(index,count,offset)||index!=1u||offset!=4u||
           (count&0x80000000u)||count<=index)return false;
        second.attempted=true;
        try {
            //The reached next-iteration branch selects the SAME Config section.
            //Neither the first iteration's field borrows nor its stage ledgers reset.
            pc=state.pc=0x22c0a0u;
            if(!state.partNameFE0||!state.config.select157D00(*state.partNameFE0))return false;
            pc=state.pc=0x22c0b4u;second.pieceKeyE0=std::string("Part")+std::to_string(index+1u);
            pc=state.pc=0x22c0c8u;second.smokeKeyF0=std::string("EmitSmoke")+std::to_string(index+1u);
            pc=state.pc=0x22c0dcu;
            if(!state.config.read158240(*second.pieceKeyE0,second.pieceName100,0x104u))return false;
            pc=state.pc=0x22c0ecu;
            if(!state.config.readInteger158920(*second.smokeKeyF0,second.emitSmoke31C))return false;
            pc=state.pc=0x22c0f4u;if(!second.emitSmoke31C)return false;
            pc=state.pc=0x22c0fcu;second.selectedDamage=0u;
            if(*second.emitSmoke31C){
                pc=state.pc=0x22c100u;if(!state.maxDamage318)return false;second.selectedDamage=*state.maxDamage318;
            }
            //174 remains unproduced until the later real2CB61C return path.
            pc=state.pc=0x22c120u;
            if(!second.pieceName100||!racer.sourcePodName1E0_)return false;
            second.modelName210=std::string("Pods\\")+racer.sourcePodName1E0_->data()+"\\"+*second.pieceName100+".slt";
            auto& app=*application_;pc=state.pc=0x22c13cu;
            auto result=owner.constructFreshSecondPiece2CB4B0(*second.modelName210,
                app.wreckAssetVectors_.position434830,app.wreckAssetVectors_.look434860,app.wreckAssetVectors_.up434850,
                &*racer.sourceWater334_,nullptr);
            pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
            //A genuine warm26E9F0 or any partial constructor cut stays a refusal;
            //the cold model loader is never replayed to manufacture a warm result.
            if(result.status!=S::modelLoadRequired||result.sourcePC!=0x26ee1cu)return false;
            result=owner.loadFreshModelOccurrence26EE1C(0,index,generation_,racer.racer.objectKey,*domain_,{&materialDirectory});
            pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
            if(result.status!=S::modelAppendRequired)return false;
            result=owner.appendFreshOccurrenceModel165CF0(index);
            pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
            if(result.status!=S::pieceSetupRequired||pc!=0x165e90u)return false;
            second.complete=true;return true;
        }catch(...){pc=state.pc;return false;}
    }
    bool advanceFreshSourceSecondPieceCursor22C25C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x22c25cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!second.waterComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x22c25cu||state.racer!=&racer||state.withdrawn||second.pieceCursorAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.pieceCursorAttempted=true;
        const bool done=state.effects->advanceFreshSecondPieceCursor(*storage_[slot],pc);
        state.pc=pc;
        if(!done||(pc!=0x22c0a0u&&pc!=0x22c278u))return false;
        second.pieceCursorComplete=true;return true;
    }
    bool beginFreshSourceGroupSparksConfig15F2F0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x15f2f0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.groupSparksSetupComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x15f2f0u||state.racer!=&racer||state.withdrawn||state.groupSparksConfigAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.groupSparksConfigAttempted=true;
        const bool done=state.effects->beginFreshGroupSparksConfig(*storage_[slot],pc);
        state.pc=pc;
        if(!done||pc!=0x15ae2cu)return false;
        state.groupSparksConfigComplete=true;return true;
    }
    bool setupFreshSourceGroupSparks2CC0D4(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cc0d4u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.groupSparksComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cc0d4u||state.racer!=&racer||state.withdrawn||state.groupSparksSetupAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.groupSparksSetupAttempted=true;
        const bool done=state.effects->setupFreshGroupSparks(*storage_[slot],actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||(pc!=0x15f2f0u&&pc!=0x2cc124u))return false;
        state.groupSparksSetupComplete=true;return true;
    }
    bool constructFreshSourceGroupSparks2CC0B4(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cc0b4u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.groupContextComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cc0b4u||state.racer!=&racer||state.withdrawn||state.groupSparksAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.groupSparksAttempted=true;
        const bool done=state.effects->constructFreshGroupSparks(*storage_[slot],*application_,pc);
        state.pc=pc;
        if(!done||pc!=0x2cc0d4u)return false;
        state.groupSparksComplete=true;return true;
    }
    bool consumeFreshSourceGroupContext2CC088(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cc088u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.groupPrimaryComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cc088u||state.racer!=&racer||state.withdrawn||state.groupContextAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.groupContextAttempted=true;
        const bool done=state.effects->consumeFreshGroupContext(*storage_[slot],pc);
        state.pc=pc;
        if(!done||pc!=0x2cc0b4u)return false;
        state.groupContextComplete=true;return true;
    }
    bool bindFreshSourceGroupPrimary22C298(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x22c298u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.groupConstructorComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x22c298u||state.racer!=&racer||state.withdrawn||state.groupPrimaryAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.groupPrimaryAttempted=true;
        const bool done=state.effects->bindFreshGroupPrimary(*storage_[slot],pc);
        state.pc=pc;
        if(!done||pc!=0x2cc088u)return false;
        state.groupPrimaryComplete=true;return true;
    }
    bool constructFreshSourceGroup22C278(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::array<std::uint32_t,4>& actual4441F0,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x22c278u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.pieceCursorComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x22c278u||state.racer!=&racer||state.withdrawn||state.groupConstructorAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.groupConstructorAttempted=true;
        const bool done=state.effects->constructFreshGroup(*storage_[slot],actual4441F0,pc);
        state.pc=pc;
        if(!done||pc!=0x22c298u)return false;
        state.groupConstructorComplete=true;return true;
    }
    bool advanceFreshSourcePieceCursor22C25C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x22c25cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.waterComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x22c25cu||state.racer!=&racer||state.withdrawn||state.pieceCursorAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.pieceCursorAttempted=true;
        const bool done=state.effects->advanceFreshPieceCursor(*storage_[slot],pc);
        state.pc=pc;
        if(!done||(pc!=0x22c0a0u&&pc!=0x22c278u))return false;
        state.pieceCursorComplete=true;return true;
    }
    bool applyFreshSourceSecondWater22C21C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::uint32_t& actualNamedWaterBit,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x22c21cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!second.pieceAppendComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x22c21cu||state.racer!=&racer||state.withdrawn||second.waterAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.waterAttempted=true;
        const bool done=state.effects->applyFreshSecondWater(*storage_[slot],actualNamedWaterBit,pc);
        state.pc=pc;
        if(!done||pc!=0x22c25cu)return false;
        second.waterComplete=true;return true;
    }
    bool applyFreshSourceWater22C21C(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::uint32_t& actualNamedWaterBit,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x22c21cu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.pieceAppendComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x22c21cu||state.racer!=&racer||state.withdrawn||state.waterAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.waterAttempted=true;
        const bool done=state.effects->applyFreshWater(*storage_[slot],actualNamedWaterBit,pc);
        state.pc=pc;
        if(!done||pc!=0x22c25cu)return false;
        state.waterComplete=true;return true;
    }
    bool appendFreshSourceSecondPiece22C1B8(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb8c4u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!(second.positiveSplashReaderComplete||second.successorCallerComplete)||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb8c4u||state.racer!=&racer||state.withdrawn||second.pieceAppendAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.pieceAppendAttempted=true;
        const bool done=state.effects->appendFreshSecondPiece(*storage_[slot],pc);
        state.pc=pc;
        if(!done||pc!=0x22c21cu)return false;
        second.pieceAppendComplete=true;return true;
    }
    bool appendFreshSourcePiece22C1B8(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb8c4u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.splashReadComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb8c4u||state.racer!=&racer||state.withdrawn||state.pieceAppendAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.pieceAppendAttempted=true;
        const bool done=state.effects->appendFreshPiece(*storage_[slot],pc);
        state.pc=pc;
        if(!done||pc!=0x22c21cu)return false;
        state.pieceAppendComplete=true;return true;
    }
    bool readStopAppendFreshSourceSplash19DDC8(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x19ddc8u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.splashComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x19ddc8u||state.racer!=&racer||state.withdrawn||state.splashReadAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.splashReadAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->readStopAppendFreshSplash(*storage_[slot],*provider,pc);
        state.pc=pc;
        if(!done||pc!=0x2cb8c4u)return false;
        state.splashReadComplete=true;return true;
    }
    bool readStopAppendFreshSourceDust19A7D0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x19a7d0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.dustComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x19a7d0u||state.racer!=&racer||state.withdrawn||state.dustReadAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.dustReadAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->readStopAppendFreshDust(*storage_[slot],*provider,pc);
        state.pc=pc;
        if(!done||pc!=0x2cb854u)return false;
        state.dustReadComplete=true;return true;
    }
    bool readAppendFreshSourceFire15E024(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x15e024u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.fireComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x15e024u||state.racer!=&racer||state.withdrawn||state.fireReadAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.fireReadAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->readAppendFreshFire(*storage_[slot],*provider,pc);
        state.pc=pc;
        if(!done||pc!=0x2cb778u)return false;
        state.fireReadComplete=true;return true;
    }
    bool completeFreshSourceSecondInitialChild19A7F8(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x19a7f8u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.initialReaderComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x19a7f8u||state.racer!=&racer||state.withdrawn||second.initialCallerAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.initialCallerAttempted=true;
        const bool done=state.effects->completeFreshSecondInitialChild(*storage_[slot],pc);
        state.pc=pc;
        if(!done||(pc!=0x2cb720u&&pc!=0x2cb854u))return false;
        second.initialCallerComplete=true;return true;
    }
    bool completeFreshSourceSecondSuccessorChild(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(pc!=0x15e0b0u&&pc!=0x19de7cu)return false;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.successorReaderComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=pc||state.racer!=&racer||state.withdrawn||second.successorCallerAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.successorCallerAttempted=true;
        const bool done=state.effects->completeFreshSecondSuccessorChild(*storage_[slot],pc);
        state.pc=pc;
        if(!done||(pc!=0x2cb778u&&pc!=0x2cb8c4u))return false;
        second.successorCallerComplete=true;return true;
    }
    bool readFreshSourceSecondSuccessorChild(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(pc!=0x15e024u&&pc!=0x19ddc8u)return false;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.successorComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=pc||state.racer!=&racer||state.withdrawn||second.successorReaderAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.successorReaderAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->readFreshSecondSuccessorChild(*storage_[slot],*provider,pc);
        state.pc=pc;
        if(!done||(pc!=0x15e0b0u&&pc!=0x19de7cu))return false;
        second.successorReaderComplete=true;return true;
    }
    bool readStopAppendFreshSourceSecondPositiveDust19A7D0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x19a7d0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.positiveDustComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x19a7d0u||state.racer!=&racer||state.withdrawn||second.positiveDustReaderAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.positiveDustReaderAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->readStopAppendFreshSecondPositiveDust(*storage_[slot],*provider,pc);
        state.pc=pc;
        if(!done||pc!=0x2cb854u)return false;
        second.positiveDustReaderComplete=true;return true;
    }
    bool readStopAppendFreshSourceSecondPositiveSplash19DDC8(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x19ddc8u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.positiveSplashComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x19ddc8u||state.racer!=&racer||state.withdrawn||second.positiveSplashReaderAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.positiveSplashReaderAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->readStopAppendFreshSecondPositiveSplash(*storage_[slot],*provider,pc);
        state.pc=pc;
        if(!done||pc!=0x2cb8c4u)return false;
        second.positiveSplashReaderComplete=true;return true;
    }
    bool readFreshSourceSecondInitialChild19A7D0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x19a7d0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.initialChildComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x19a7d0u||state.racer!=&racer||state.withdrawn||second.initialReaderAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.initialReaderAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->readFreshSecondInitialChild(*storage_[slot],*provider,pc);
        state.pc=pc;
        if(!done||pc!=0x19a7f8u)return false;
        second.initialReaderComplete=true;return true;
    }
    bool readAppendFreshSourceSmoke19A7D0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x19a7d0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.smokeComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x19a7d0u||state.racer!=&racer||state.withdrawn||state.smokeReadAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.smokeReadAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->readAppendFreshSmoke(*storage_[slot],*provider,pc);
        state.pc=pc;
        if(!done||pc!=0x2cb720u)return false;
        state.smokeReadComplete=true;return true;
    }
    bool continueFreshSourceSecondPairs2CB6CC(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb6ccu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!second.firstPairStopped||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb6ccu||state.racer!=&racer||state.withdrawn||second.remainingPairsAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.remainingPairsAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->continueFreshSecondPairs(*storage_[slot],*application_,*provider,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x2cb7e0u)return false;
        second.remainingPairsComplete=true;return true;
    }
    bool continueFreshSourcePairs2CB6CC(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb6ccu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.firstPairStopped||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb6ccu||state.racer!=&racer||state.withdrawn||state.remainingPairsAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.remainingPairsAttempted=true;
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const bool done=state.effects->continueFreshPairs(*storage_[slot],*application_,*provider,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x2cb7e0u)return false;
        state.remainingPairsComplete=true;return true;
    }
    bool constructFreshSourceFire2CB720(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb720u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.smokeReadComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb720u||state.racer!=&racer||state.withdrawn||state.fireAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.fireAttempted=true;
        const bool done=state.effects->constructFreshFire(*storage_[slot],*application_,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x15e024u)return false;
        state.fireComplete=true;return true;
    }
    bool constructFreshSourceSplash2CB854(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::array<std::uint32_t,4>* actual433F50,const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb854u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.dustReadComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=pc||state.racer!=&racer||state.withdrawn||state.splashAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.splashAttempted=true;
        const bool done=state.effects->constructFreshSplash(*storage_[slot],*application_,actual433F50,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x19ddc8u)return false;
        state.splashComplete=true;return true;
    }
    bool constructFreshSourceSecondPositiveSplash(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::array<std::uint32_t,4>* actual433F50,const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb854u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.positiveDustReaderComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=pc||state.racer!=&racer||state.withdrawn||second.positiveSplashAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.positiveSplashAttempted=true;
        const bool done=state.effects->constructFreshSecondPositiveSplash(*storage_[slot],*application_,actual433F50,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x19ddc8u)return false;
        second.positiveSplashComplete=true;return true;
    }
    bool constructFreshSourceSecondSuccessorChild(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::array<std::uint32_t,4>* actual433F50,const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(pc!=0x2cb720u&&pc!=0x2cb854u)return false;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.initialCallerComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=pc||state.racer!=&racer||state.withdrawn||second.successorAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.successorAttempted=true;
        const bool done=state.effects->constructFreshSecondSuccessorChild(*storage_[slot],*application_,actual433F50,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||(pc!=0x15e024u&&pc!=0x19ddc8u))return false;
        second.successorComplete=true;return true;
    }
    bool constructFreshSourceSecondPositiveDust(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb7e0u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.firstPairStopped||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=pc||state.racer!=&racer||state.withdrawn||second.positiveDustAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.positiveDustAttempted=true;
        const bool done=state.effects->constructFreshSecondPositiveDust(*storage_[slot],*application_,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x19a7d0u)return false;
        second.positiveDustComplete=true;return true;
    }
    bool constructFreshSourceSecondInitialChild(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(pc!=0x2cb6ccu&&pc!=0x2cb7e4u)return false;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.arraysComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=pc||state.racer!=&racer||state.withdrawn||second.initialChildAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.initialChildAttempted=true;
        const bool done=state.effects->constructFreshSecondInitialChild(*storage_[slot],*application_,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x19a7d0u)return false;
        second.initialChildComplete=true;return true;
    }
    bool constructFreshSourceDust2CB7E0(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(pc!=0x2cb7e0u&&pc!=0x2cb7e4u)return false;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.arraysComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=pc||state.racer!=&racer||state.withdrawn||state.dustAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.dustAttempted=true;
        const bool done=state.effects->constructFreshDust(*storage_[slot],*application_,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x19a7d0u)return false;
        state.dustComplete=true;return true;
    }
    bool constructFreshSourceSmoke2CB6CC(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb6ccu;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.arraysComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb6ccu||state.racer!=&racer||state.withdrawn||state.smokeAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.smokeAttempted=true;
        const bool done=state.effects->constructFreshSmoke(*storage_[slot],*application_,actualDirectory36B530,pc);
        state.pc=pc;
        if(!done||pc!=0x19a7d0u)return false;
        state.smokeComplete=true;return true;
    }
    bool constructFreshSourceSecondArrays2CB628(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb628u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||sourceAttempted_||queryAttempted_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];auto& second=state.secondPiece;
        if(!state.pieceCursorComplete||!second.effectInputsComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb628u||state.racer!=&racer||state.withdrawn||second.arraysAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        second.arraysAttempted=true;
        const auto result=storage_[slot]->constructFreshEffectArrays2CB628(1u);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(!((result.status==S::effectChildrenRequired&&result.sourcePC==0x2cb6ccu)||
             (result.status==S::effectDustRequired&&result.sourcePC==0x2cb7e4u)))return false;
        second.arraysComplete=true;return true;
    }
    bool constructFreshSourceArrays2CB628(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb628u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.effectInputsComplete||!state.effects||!state.effects->freshInputsOnly_||
           !state.effects->canRetireFreshInputContext()||state.pc!=0x2cb628u||state.racer!=&racer||state.withdrawn||state.arraysAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.arraysAttempted=true;
        const auto result=storage_[slot]->constructFreshEffectArrays2CB628();
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(!((result.status==S::effectChildrenRequired&&result.sourcePC==0x2cb6ccu)||
             (result.status==S::effectDustRequired&&result.sourcePC==0x2cb7e4u)))return false;
        state.arraysComplete=true;return true;
    }
    bool constructFreshSourceMutual166390(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x166390u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           !identitiesBound_||!modelSourcesBound_||slot>=racerCount_||!storage_[slot]||
           racers_[slot]!=&racer.racer||!racer.prepared()||racer.nativeWreckWithdrawn_||
           !application_||application_->released_||!domain_||!domain_->prepared()||!domain_->resolver3E1A70())return false;
        auto& state=freshSources_[slot];
        if(!state.e8Complete||state.pc!=0x166390u||state.racer!=&racer||state.withdrawn||state.mutualAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get()||!inventories_[slot]||
           inventories_[slot]->modelResolverIdentity()!=domain_->resolver3E1A70().get())return false;
        state.mutualAttempted=true;
        const auto result=storage_[slot]->constructFreshMutualAndCount166390(state.selectedDamage);
        pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
        if(result.status!=S::effectInputsRequired||result.sourcePC!=0x2cb618u)return false;
        state.mutualComplete=true;return true;
    }
    bool bindFreshSourceEffectInputs2CB618(std::size_t slot,rr::game::OriginalRacerEntityCold50LA& racer,
        rr::game::OriginalTrackParticleCold50LA& manager,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb618u;
        if(thread_!=std::this_thread::get_id()||closed_||retiring_||!freshSourceSelected_||freshSourceRetired_||
           slot>=racerCount_||!storage_[slot]||racers_[slot]!=&racer.racer||!racer.prepared()||
           racer.nativeWreckWithdrawn_||!application_||application_->released_)return false;
        auto& state=freshSources_[slot];
        if(!state.mutualComplete||state.pc!=0x2cb618u||state.racer!=&racer||state.withdrawn||state.effectInputsAttempted||
           !state.collectionWorld||!state.collectionWorld->current()||
           racer.binding_.actualWorld2107FC!=state.collectionWorld.get())return false;
        state.effectInputsAttempted=true;
        //Original210770 t2 is the SAME argument retained at210890;7FC is an
        //actual source field from210A0C, never an optional authoring projection.
        if(racer.particles4A0_!=manager.particles()||!racer.sourceParticle7FC_||
           *racer.sourceParticle7FC_!=&racer.particle7FC)return false;
        try {
            state.effects=std::make_unique<NativeWreckEffectChildren>();
            if(!state.effects->bindFreshInputContext(racer,manager,&*racer.sourceParticle7FC_))return false;
            const auto result=storage_[slot]->bindFreshEffectInputs2CB618(
                {&state.effects->sourceContext_,state.effects->sourceParentField_});
            pc=state.pc=result.sourcePC?result.sourcePC:state.pc;
            if(result.status!=S::effectConstructorRequired||result.sourcePC!=0x2cb628u)return false;
            state.effectInputsComplete=true;return true;
        }catch(...){return false;}
    }
    bool retireFreshSourceConstructors()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(!freshSourceSelected_||freshSourceRetired_)return true;
        if(closed_||sourceAttempted_||queryAttempted_||!application_||application_->released_)return false;
        //Whole prefix preflight before the first count/reference drop.
        //Fresh E0 may own its prepared Type2 payload and appended Entity tail,
        //but no world registration. Retire all reached effects before World pins.
        for(std::size_t slot=0;slot<racerCount_;++slot){
            const auto& state=freshSources_[slot];const auto& owner=storage_[slot];
            if(!owner||!owner->preflightFreshConstructorOnlyRetirement()||
               (state.effects&&!state.effects->canRetireFreshInputContext()))return false;
            if(!state.attempted)continue;
            const auto* racer=state.racer;
            if(!racer||&racer->racer!=racers_[slot]||racer->racer.reference00!=1u||
               racer->racer.sessionGeneration!=generation_||!racer->nativeWreckLoadAttempted_)return false;
            if(racer->sourceWreck374Published_&&(!racer->sourceWreckCount370_||
               !owner->freshSource374AliasCurrent(racer->sourceWreck374_,*racer->sourceWreckCount370_,
                    racer->sourceWreck374Slots_)))return false;
        }
        //All external and piece owners were preflighted above. An unappended
        //smoke prefix retires before its borrowed160 array and base Model.
        for(std::size_t slot=racerCount_;slot-->0;)
            if(freshSources_[slot].effects&&!freshSources_[slot].effects->retireFreshExternalPrefix())return false;
        for(std::size_t slot=racerCount_;slot-->0;)
            if(!storage_[slot]->retireFreshConstructorOnly22BF50())return false;
        for(auto& state:freshSources_)if(state.effects&&!state.effects->retireFreshInputContext())return false;
        for(auto& state:freshSources_){
            if(state.racer)state.racer->nativeWreckWithdrawn_=true;
            state.racer=nullptr; //Native borrow ends; source370/374 words remain untouched.
            state.config.withdrawNative();
            state.collectionWorld.reset(); //After actual model/piece/collection retirement only.
            state.withdrawn=true;
        }
        freshSourceRetired_=true;return true;
    }
    bool bindActualRacers(
        const std::array<const rr::game::OriginalWreckComponent50LA*,8>& racers,
        std::size_t count,std::uint64_t actualGeneration,std::string& error) noexcept {
        try {
            if(thread_!=std::this_thread::get_id()){error="wreck identity owner thread differs";return false;}
            if(closed_||retiring_||identityAttempted_){error="wreck identity adoption unavailable";return false;}
            identityAttempted_=true;
            if(!complete_||!modelSourcesBound_||count!=racerCount_||!actualGeneration||
               !application_||application_->released_)
                throw std::runtime_error("wreck actual racer identity prerequisites absent");
            for(std::size_t i=0;i<count;++i){
                const auto* racer=racers[i];
                if(!storage_[i]||!racer||racer->sessionGeneration!=actualGeneration||!racer->objectKey||
                   racer->kind!=rr::game::OriginalWreckComponentKind50LA::racer27E130||racer->reference00!=1)
                    throw std::runtime_error("wreck actual racer component differs");
                for(std::size_t j=0;j<i;++j)if(racers[j]==racer||racers[j]->objectKey==racer->objectKey)
                    throw std::runtime_error("wreck duplicate racer component identity");
            }
            racers_=racers;generation_=actualGeneration;
            for(std::size_t i=0;i<count;++i)
                if(!storage_[i]->bindNativeColdIdentity(*racers_[i]))
                    throw std::runtime_error("wreck actual identity bind rejected; prefix retained");
            identitiesBound_=true;error.clear();return true;
        }catch(const std::exception& e){try{failure_=e.what();error=failure_;}catch(...){}return false;}
        catch(...){try{failure_="wreck actual identity adoption failed";error=failure_;}catch(...){}return false;}
    }
    // Bridge supplies its one authentic common world and the exact reserved
    // per-occurrence triples. No authored count is used as a live source array.
    bool prepareOrderedQueries(const rr::game::OriginalWreckQueryWorld50LA& common,
        const std::vector<rr::game::OriginalWreckPieceQuerySlots50LA>& slots,
        std::string& error) noexcept {
        try {
            if(thread_!=std::this_thread::get_id()){error="wreck query owner thread differs";return false;}
            if(closed_||retiring_||freshSourceSelected_||queryAttempted_){error="wreck query preparation unavailable";return false;}
            queryAttempted_=true;
            if(!identitiesBound_||!modelSourcesBound_||!application_||application_->released_||
               slots.size()!=reservations_.size()||
               !common.world||common.modelScenes||common.groupPieceCapacity!=1u)
                throw std::runtime_error("wreck ordered world prerequisites absent");
            for(unsigned q=0;q<3;++q)if(common.refs[q]||common.identities[q])
                throw std::runtime_error("wreck common world contains occurrence slots");
            for(std::size_t i=0;i<slots.size();++i){
                if(slots[i].identities!=reservations_[i].identities)
                    throw std::runtime_error("wreck reserved occurrence identity differs");
                for(unsigned q=0;q<3;++q){
                    if(!slots[i].refs[q]||slots[i].refs[q]>common.objectCount)
                        throw std::runtime_error("wreck actual occurrence reference absent");
                    for(std::size_t j=0;j<=i;++j)for(unsigned k=0;k<3;++k){
                        if(j==i&&k>=q)break;
                        if(slots[j].refs[k]==slots[i].refs[q])
                            throw std::runtime_error("wreck repeated occurrence reference");
                    }
                }
            }
            for(std::size_t i=0;i<racerCount_;++i){
                const auto& span=spans_[i];const auto* racer=racers_[i];
                if(!racer||racer->sessionGeneration!=generation_||
                   racer->objectKey!=storage_[i]->racerKey()||racer->reference00!=1||
                   racer->kind!=rr::game::OriginalWreckComponentKind50LA::racer27E130||
                   !span.count||span.begin>slots.size()||span.count>slots.size()-span.begin)
                    throw std::runtime_error("wreck racer lifetime or reservation span differs");
                if(storage_[i]->prepareOrderedQueriesCold(common,slots.data()+span.begin,span.count).status!=
                    rr::game::OriginalWreckOrderedQueryStatus50LA::coldPrepared)
                    throw std::runtime_error("wreck ordered cold query preparation rejected; prefix retained");
            }
            queriesPrepared_=true;error.clear();return true;
        }catch(const std::exception& e){try{failure_=e.what();error=failure_;}catch(...){}return false;}
        catch(...){try{failure_="wreck ordered query preparation failed";error=failure_;}catch(...){}return false;}
    }
    // Two-phase native cancellation. Follow must close while these rows live;
    // its aliases are revoked between this phase and backing destruction.
    bool retireNativeColdQueries(const rr::game::LiveZoneWorld50EN* world) noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(closed_||queriesRetired_)return true;
        retiring_=true; // A rejected preflight cannot permit another adoption.
        for(const auto& owner:storage_)if(owner&&!owner->canRetireNativeColdQueries(world))return false;
        for(auto& owner:storage_)if(owner&&!owner->retireNativeColdQueries(world))return false;
        queriesRetired_=true;return true;
    }
    bool releaseNativeColdBacking() noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(closed_)return true;
        if(!queriesRetired_||(sourceAttempted_&&!sourceWithdrawn_)||(freshSourceSelected_&&!freshSourceRetired_))return false;
        for(const auto& owner:storage_)if(owner&&!owner->canReleaseNativeColdBacking())return false;
        closed_=true;reservations_.clear();racers_.fill(nullptr);
        for(auto& owner:storage_)owner.reset();
        nativeEffects_.reset(); //All external refs and piece array borrows genuinely retired.
        for(auto& state:freshSources_)state.effects.reset(); //After every piece field borrow is destroyed.
        for(auto& inventory:inventories_)inventory.reset();
        return true;
    }
    // Only the real world owner invokes this after its canClose check and
    // all unpublished query withdrawals. Preflight every alias before any
    // destruction; future source-bound owners require genuine source cleanup.
    bool releaseUnboundColdReservations() noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(closed_)return true;
        if(identityAttempted_||queryAttempted_||retiring_)return false;
        for(const auto& row:reservations_)
            if(!row.queries||row.queries->prepared()||row.queries->constructed())return false;
        for(const auto& owner:storage_)if(owner&&
            (owner->sessionGeneration()!=0||owner->racerKey()!=0))return false;
        closed_=true;
        reservations_.clear();
        for(auto& owner:storage_)owner.reset();
        for(auto& inventory:inventories_)inventory.reset();
        return true;
    }
public:
    WreckBacking()=default;
    WreckBacking(const WreckBacking&)=delete;
    WreckBacking& operator=(const WreckBacking&)=delete;
    bool prepare(std::shared_ptr<rr::game::OriginalApplicationEntities50LA> app,
        std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> domain,
        const std::array<std::unique_ptr<native_racer50HW::Rig>,8>& rigs,
        const std::array<std::vector<std::uint8_t>,8>& retainedRes,
        std::size_t racerCount,std::string& error) noexcept {
        try {
            if(thread_!=std::this_thread::get_id()) {error="wreck backing owner thread differs";return false;}
            if(closed_||retiring_) {error="wreck cold backing retired";return false;}
            if(attempted_) {error=failure_.empty()?"wreck backing preparation already attempted":failure_;return false;}
            attempted_=true;
            application_=std::move(app);domain_=std::move(domain);
            if(!application_||!racerCount||racerCount>8u||
                application_->nativeOwnerThread_!=thread_||!application_->constructed_||application_->released_)
                throw std::runtime_error("actual wreck application/roster absent");
            racerCount_=racerCount;
            using I=rr::game::OriginalWreckInventoryStatus50LA;
            const auto endpoints=endpointPolicy();
            std::size_t occurrence=0;
            for(std::size_t slot=0;slot<racerCount;++slot){
                if(!rigs[slot])throw std::runtime_error("actual wreck Rig absent");
                inventories_[slot]=std::make_unique<rr::game::OriginalWreckAssetInventory50LA>();
                auto& inventory=*inventories_[slot];
                if(original_wreck_rig50LA::inventoryRetainedRig(*rigs[slot],retainedRes[slot],
                    application_->wreckAssetVectors_,inventory,error).status!=I::inventoried)
                    throw std::runtime_error(error.empty()?"wreck retained inventory rejected":error);
                while(!inventory.complete()){
                    const auto loaded=inventory.loadNextCold(endpoints,error);
                    if(loaded.status!=I::pieceLoaded&&loaded.status!=I::complete)
                        throw std::runtime_error(error.empty()?"wreck cold occurrence load rejected":error);
                }
                storage_[slot]=std::make_unique<rr::game::OriginalWreckRacerStorage50LA>();
                if(storage_[slot]->adoptNativeColdInventory(inventories_[slot]).status!=
                    rr::game::OriginalWreckRacerStorageStatus50LA::coldBacking)
                    throw std::runtime_error("wreck native cold backing adoption rejected");
                std::vector<rr::game::OriginalWreckRacerStorage50LA::NativeColdReservation> rows;
                if(!storage_[slot]->nativeColdReservations(rows))
                    throw std::runtime_error("wreck native reservation extraction rejected");
                spans_[slot]={reservations_.size(),rows.size()};
                for(const auto& row:rows){
                    constexpr std::uint32_t first=0xe1000000u;
                    if(occurrence>(std::numeric_limits<std::uint32_t>::max()-first-2u)/3u)
                        throw std::runtime_error("wreck native identity range exhausted");
                    if(!row.queries||!row.assets)throw std::runtime_error("wreck reservation owner absent");
                    const auto base=first+3u*static_cast<std::uint32_t>(occurrence);
                    // Explicit host metadata only; actual world admission still
                    // validates the complete namespace and exact owner addresses.
                    reservations_.push_back({row.queries,row.assets,{base,base+1u,base+2u}});
                    ++occurrence;
                }
            }
            complete_=true;error.clear();return true;
        }catch(const std::exception& e){try{failure_=e.what();error=failure_;}catch(...){}return false;}
        catch(...){try{failure_="wreck cold backing preparation failed";error=failure_;}catch(...){}return false;}
    }
    // Invoked only after the real provider sealNativeMounts completion. The
    // earlier native capacity reservation cannot invent resolver admission.
    bool bindMountedSources(std::shared_ptr<rr::game::OriginalApplicationEntities50LA> app,
        std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> domain,
        std::string& error) noexcept {
        try {
            if(thread_!=std::this_thread::get_id()) {error="wreck binding owner thread differs";return false;}
            if(closed_||retiring_) {error="wreck cold backing retired";return false;}
            if(modelBindingAttempted_) {error=failure_.empty()?"wreck model binding already attempted":failure_;return false;}
            modelBindingAttempted_=true;
            if(!complete_||!app||app.get()!=application_.get()||
                app->nativeOwnerThread_!=thread_||!app->constructed_||app->released_||
                !domain||(domain_&&domain_.get()!=domain.get())||!domain->prepared())
                throw std::runtime_error("actual mounted wreck application/domain differs");
            domain_=std::move(domain);
            for(auto& owner:storage_)if(owner){
                rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
                if(owner->bindNativeColdModelSources(*domain_,diagnostic).status!=
                    rr::game::OriginalWreckInventoryStatus50LA::modelSourcesBound)
                    throw std::runtime_error("wreck mounted same-domain source binding rejected");
            }
            modelSourcesBound_=true;error.clear();return true;
        }catch(const std::exception& e){try{failure_=e.what();error=failure_;}catch(...){}return false;}
        catch(...){try{failure_="wreck mounted source binding failed";error=failure_;}catch(...){}return false;}
    }
    bool modelSourcesBound()const noexcept {
        return thread_==std::this_thread::get_id()&&!closed_&&!retiring_&&complete_&&modelSourcesBound_;
    }
    const std::vector<Reservation>* reservations()const noexcept {
        return thread_==std::this_thread::get_id()&&!closed_&&!retiring_&&complete_?&reservations_:nullptr;
    }
    const rr::game::OriginalApplicationEntities50LA* applicationIdentity()const noexcept{return application_.get();}
    const rr::assets::OriginalStandardMaterialDomain50LA* domainIdentity()const noexcept{return domain_.get();}
};
} // namespace ra_cold_roster50LA
