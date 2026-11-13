// PodLoad: the original per-racer pod load 2129C0 (210770 caller), run for
// every slot in the 1AE280 manager order, on every course that reaches it.
// D:\Programming\GitHub\RacerRevengePC\work\scratch\pod-load-all\PLAN.md 3.1/3.2.
// Track-side inputs come only through original_pod_load50pl.h.
#include "original_camera_session50kx.h"
#include "original_race_world_join50la.h"
#include "ra_cold_aggregate50la.h"
#include "ra_fresh_execution_arena50la.h"
#include "original_pod_load50pl.h"
#include "../../src/game/original_native_collision_categories.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>

namespace original_pod_load50PL {
// Provider backed by the track's source scene owner (its source-loaded 4C0/4C4
// search nodes and World4A0 particle owner). One implementation per kind of
// track data, chosen by what the loaded track owns, not by course name.
class SceneTrackData50PL final : public TrackDataProvider50PL {
public:
    SceneTrackData50PL(ra_pod_scene50LA::Owner& scene,const void* water)noexcept:scene_(scene),water_(water){}
    bool retainWreckZones(ra_source_camera50LA::OwnershipBridge& borrower,std::uint64_t generation,
        rr::game::OriginalWreckSetupZones50LA& zones,ZoneRef50PL& visibility,ZoneRef50PL& collision)noexcept override {
        if(borrower.nativeWreckZoneOwner_)return false;
        rr::game::CollectionRef50BX v=0,c=0;
        if(!borrower.podSceneRetainZones(scene_,generation,zones,v,c))return false;
        visibility=v;collision=c;borrower.nativeWreckZoneOwner_=&scene_;return true;
    }
    bool releaseWreckZones(ra_source_camera50LA::OwnershipBridge& borrower)noexcept override {
        if(borrower.nativeWreckZoneOwner_!=&scene_)return false;
        if(!borrower.podSceneReleaseZones(scene_))return false;
        borrower.nativeWreckZoneOwner_=nullptr;return true;
    }
    bool retainWreckParticles(ra_source_camera50LA::OwnershipBridge& borrower,
        rr::game::OriginalTrackParticleCold50LA* actual4A0)noexcept override {
        return borrower.podSceneRetainParticles(scene_,actual4A0);
    }
    const void* water3FC()const noexcept override{return water_;}
private:
    ra_pod_scene50LA::Owner& scene_;
    const void* water_=nullptr;
};
}

namespace ra_source_camera50LA {
namespace g=rr::game;

// The loaded track's source scene owner supplies its own 4C0/4C4, 4A0 and
// Race70 water through the provider; nothing here is course-specific.
bool OwnershipBridge::loadPodRosterFromScene2129C0(original_camera_session50KX::Session& session,
    const g::OriginalRaceConstructor50LA& race,::ra_pod_scene50LA::Owner& scene,const void* water3FC,
    const g::UndertrackPhysicsDomain50EW& heights,const std::string& directory,
    const std::string& actualParticleDirectory36B530,std::uint32_t& actualRawRandom,std::uint32_t& pc)noexcept {
    try {
        if(podTrackStorage_)return false;
        podTrackStorage_=std::make_unique<original_pod_load50PL::SceneTrackData50PL>(scene,water3FC);
    }catch(...){return false;}
    return loadPodRoster2129C0(session,race,*podTrackStorage_,heights,directory,actualParticleDirectory36B530,actualRawRandom,pc);
}
// The scene owner admits only its bridge as borrower (friend); these forward
// the provider's calls unchanged.
bool OwnershipBridge::podSceneRetainZones(::ra_pod_scene50LA::Owner& scene,std::uint64_t generation,
    g::OriginalWreckSetupZones50LA& zones,g::CollectionRef50BX& visibility,g::CollectionRef50BX& collision)noexcept {
    return scene.retainNativeWreckFirstZones(this,generation,zones,visibility,collision);
}
bool OwnershipBridge::podSceneReleaseZones(::ra_pod_scene50LA::Owner& scene)noexcept {
    return scene.releaseNativeWreckFirstZones(this);
}
bool OwnershipBridge::podSceneRetainParticles(::ra_pod_scene50LA::Owner& scene,g::OriginalTrackParticleCold50LA* actual)noexcept {
    return scene.retainNativeWreckParticles(this,actual);
}
// 1AE280: every racer's 2129C0 in manager order. The 4C0[0]/4C4[0] borrow is
// taken once for the whole roster (1AE0C8/1AE488 pass the same rows; 1AE4B0)
// and the particle owner once. A slot that stops leaves every later slot
// unreached, as the original manager would never reach them.
bool OwnershipBridge::loadPodRoster2129C0(original_camera_session50KX::Session& session,
    const g::OriginalRaceConstructor50LA& race,original_pod_load50PL::TrackDataProvider50PL& track,
    const g::UndertrackPhysicsDomain50EW& heights,const std::string& directory,
    const std::string& actualParticleDirectory36B530,std::uint32_t& actualRawRandom,std::uint32_t& pc)noexcept {
    try {
        if(podRosterAttempted_||podZonesHeld_||!session.onOwnerThread()||!rosterResources_||
           !rosterResources_->wreckBacking_)return false;
        podRosterAttempted_=true;
        auto& backing=*rosterResources_->wreckBacking_;
        const std::size_t racers=session.actual_.bank.count();
        const std::size_t humans=session.actual_.humanCount34;
        if(!racers||racers>session.racerEntities_.size()||backing.nativeRacerCount()!=racers||humans>racers)return false;
        std::size_t pieces=0;for(std::size_t i=0;i<racers;++i)pieces+=backing.nativeSlotPieces(i);
        if(!track.retainWreckZones(*this,session.racerEntities_[0].racer.sessionGeneration,
            podZones_,podVisibility_,podCollision_))return false;
        // The zone fields 165E90 reads are the roster-wide temporaries kept here.
        podZones_.visibilityZoneField=&podVisibility_;podZones_.collisionZoneField=&podCollision_;
        podZonesHeld_=true;podTrack_=&track;
        if(!track.retainWreckParticles(*this,session.actual_.actualTrackParticles4A0))return false;
        std::size_t loaded=0;
        for(std::size_t ordinal=0;ordinal<racers;++ordinal){
            std::size_t slot=0;
            if(!original_pod_load50PL::managerOrderSlot50PL(ordinal,humans,racers,slot))return false;
            std::array<g::OriginalWreckWarmCreator50LA,4> warm{};std::size_t warmCount=0;
            if(!podWarmCreators(session,slot,warm,warmCount)||!backing.bindNativeWarmCreators(slot,warm.data(),warmCount))return false;
            const bool ok=loadPodRacer2129C0(slot,session,race,podZones_,heights,directory,
                actualParticleDirectory36B530,track.water3FC(),actualRawRandom,pc);
            backing.releaseNativeWarmCreators(slot);
            std::size_t c=0,p=0;unsigned st=0;std::uint32_t cursor=0;
            backing.nativeWreckCursor(slot,c,p,st,cursor);
            if(!ok){
                std::fprintf(stderr,"pod_load_stopped slot=%zu human=%d pc=%x collection=%zu piece=%zu status=%u",
                    slot,int(slot<humans),unsigned(pc),c,p,st);std::fputc(10,stderr);
                // Read-only diagnostic of this racer's actual child chain (212AAC tail rule).
                const auto& r=session.racerEntities_[slot];
                std::fprintf(stderr,"pod_load_racer_chain slot=%zu kind=%u ref=%llu parent=%d prev=%d next=%d",slot,
                    unsigned(r.racer.kind),(unsigned long long)r.racer.reference00,int(bool(r.racer.parent1C)),
                    int(bool(r.racer.previous10)),int(bool(r.racer.next14)));
                unsigned k=0;for(const auto* x=r.racer.firstChild18;x&&k<64;x=x->next14,++k)
                    std::fprintf(stderr," [%u kind=%u ref=%llu own=%d 7fc=%d]",k,unsigned(x->kind),
                        (unsigned long long)x->reference00,int(x->parent1C==&r.racer),int(x==&r.particle7FC));
                std::fputc(10,stderr);return false;
            }
            ++loaded;
            const auto* lane=podExclusions_.lane(slot);
            std::fprintf(stderr,"pod_load_racer slot=%zu human=%d pieces=%zu lane690=%zu/%zu pc=%x%s",slot,int(slot<humans),
                backing.nativeSlotPieces(slot),lane?lane->count:0u,lane?lane->capacity:0u,unsigned(pc),
                podAiHandlingStopped_[slot]?" handling=native_standin":"");
            std::fputc(10,stderr);
        }
        std::fprintf(stderr,"pod_load_complete racers=%zu pieces=%zu",loaded,pieces);std::fputc(10,stderr);
        return loaded==racers;
    }catch(...){return false;}
}
// This racer's own model scenes, the only creators a warm 26E9F0 hit may name
// besides its earlier wreck pieces. Borrowed for the duration of its load.
bool OwnershipBridge::podWarmCreators(original_camera_session50KX::Session& session,std::size_t slot,
    std::array<g::OriginalWreckWarmCreator50LA,4>& out,std::size_t& count)const noexcept {
    count=0;out={};
    if(slot>=8u)return false;
    for(const auto* models:{session.actual_.actualChariotModels,session.actual_.actualRightModels,
        session.actual_.actualLeftModels,session.actual_.actualBottomLeftModels}){
        if(!models)continue;
        const auto* m=(*models)[slot].get();if(!m)continue;
        const auto* scene=m->modelScene();const auto* materials=m->materialReferences();
        if(!scene||!materials)continue;
        if(count>=out.size())return false;
        out[count++]={scene,materials,m->graph()};
    }
    return true;
}

bool OwnershipBridge::loadPodRacer2129C0(std::size_t slot,original_camera_session50KX::Session& session,
    const g::OriginalRaceConstructor50LA& race,const g::OriginalWreckSetupZones50LA& zones,
    const g::UndertrackPhysicsDomain50EW& heights,const std::string& directory,
    const std::string& actualParticleDirectory36B530,const void* actualWorld3FC,std::uint32_t& actualRawRandom,std::uint32_t& pc)noexcept {
    try {
        if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
           session.nativeWreckOwner_!=this||!cameraBankCurrent()||closed_||terminal_||cameraPrepareTerminal_||
           !nativeWreckPrepared_||nativeWreckRetired_||!session.identitiesCurrent()||
           !session.nativeRacerCamera1750A0_.prefixComplete||session.nativeRacerCamera1750A0_.revoked||
           session.nativeRacerCamera1750A0_.pc!=0x1750f8||session.actual_.actualHeightDomain!=&heights||
           session.actual_.actualMaterialDirectory36B970!=&directory||!rosterResources_||
           !rosterResources_->wreckBacking_||!follow_||follow_->attempted_||follow_->closed_||
           slot>=session.actual_.bank.count()||slot>=session.racerEntities_.size())return false;
        return session.actual_.bank.visit(slot,[&](){
            auto& backing=*rosterResources_->wreckBacking_;
            if(!session.actual_.actualChariotModels)return false;
            auto* nativeModel=(*session.actual_.actualChariotModels)[slot].get();
            auto* phy=nativeModel?nativePhyOwnerCurrent(session,*nativeModel):nullptr;
            auto& actualRacer=session.racerEntities_[slot];
            if(!phy||actualRacer.nativePhyLoad_)return false;
            const bool human=slot<session.actual_.humanCount34;
            if(human&&!bindNativePlayerLoad1A0848(session,*nativeModel))return false;
            //Forward bind to the genuine pre-existing cold occurrence. Earlier
            //Wreck inventory reads remain explicitly native-adapted; no fake
            //210A48 open or2129AC selection history is manufactured here.
            actualRacer.nativePhyLoad_=phy;
            if(!backing.loadNativeFirstRacerSource(slot,actualRacer,heights,directory,actualWorld3FC,pc))return false;
            // 4C0[0]/4C4[0]: the one roster-wide borrow taken by loadPodRoster2129C0 (1AE4B0).
            if(!podZonesHeld_||!nativeWreckParticles_)return false;
            // Any setup rejection retains this exact borrow and constructor
            // prefix until real source-aware retirement; never retry/replay.
            if(!backing.setupNativeBody(slot,0,zones,pc))return false;
            if(!backing.loadNativeQueryChildren(slot,0,pc))return false;
            //Actual retained Application directory, not an authored filename
            //guess or a copied source value. Preserve earlier query effects if
            //this native resource-lookup admission cannot be established.
            if(actualParticleDirectory36B530!="particle\\")return false;
            //Existing immutable HOST category namespace, not a fabricated guest
            //4342A8 word or an independent literal category table.
            const auto& water=g::OriginalNativeCollisionCategories::bindings[5];
            if(std::strcmp(water.name,"WATER")||water.bit!=g::OriginalNativeCollisionCategories::water)return false;
            if(!backing.loadNativeEffects(slot,0,*nativeWreckParticles_,water.bit,pc))return false;
            if(!backing.loadNativeRemainingCollectionPieces(slot,zones,heights,directory,*nativeWreckParticles_,water.bit,pc))return false;
            if(!backing.loadNativeCollectionGroup(slot,pc))return false;
            if(!backing.deactivateNativeCollectionLoad(slot,pc)||!backing.appendNativeCollectionToRacer(slot,pc))return false;
            //Actual same selected Bank690/694 owners, not Wreck replacements.
            //Native immutable projection refresh is admitted before each source
            //list mutation; no active collector/role view may retain old words.
            auto refuse=[&](int why){std::fprintf(stderr,"pod_load_exclusion_refused slot=%zu reason=%d",slot,why);std::fputc(10,stderr);return false;};
            if(sharedOwnerLease_||sharedRoleViews_||nativeFifoPins_||triggerQueryActive_||sharedEffectsStopped_)return refuse(1);
            if(!actual_)return refuse(2);if(!actual_->recovery)return refuse(21);
            // This racer's own 690/694 owners, read inside its own Bank visit:
            // the live main filter (identity and banked list 0) and the 694
            // pod filter. Nothing here writes them; 212ABC's 690 additions go
            // to this racer's fixed lane (podExclusions_), never the live list.
            const auto& main690=actual_->mainFilter;const auto& pod694=actual_->recovery->podFilter;
            if(session.actual_.bank.currentSlot_!=slot)return refuse(3);
            if(!main690.identity||main690.identity!=bodyIdentities_[slot].main)return refuse(4);
            if(!pod694.identity||pod694.identity!=actual_->recovery->globals.identity[0]||
               pod694.identity==main690.identity)return refuse(5);
            const auto& list0=main690.identityLists[0];
            if(list0.mode!=1||list0.identities.empty())return refuse(6);
            const auto actual694=pod694.identity;
            if(!podExclusions_.lane(slot)){
                if(!podExclusions_.size(slot,main690.identity,actual694,list0.identities.data(),
                    list0.identities.size(),backing.nativeSlotPieces(slot)))return refuse(7);
                std::fprintf(stderr,"pod_load_exclusion_lane slot=%zu base=%zu capacity=%zu own694=%08x shared694=%zu",
                    slot,list0.identities.size(),podExclusions_.lane(slot)->capacity,unsigned(actual694),
                    podExclusions_.shared694(slot));std::fputc(10,stderr);
            }else if(podExclusions_.lane(slot)->own694!=actual694||podExclusions_.lane(slot)->own690!=main690.identity)return refuse(8);
            auto appendMain690=[&](std::uint32_t actualE4)->bool {
                if(!podExclusions_.append690(slot,actualE4))return refuse(20);
                return true;
            };            for(;;){
                if(!backing.bindNativeRacerExclusions(slot,actual694,appendMain690,pc))return refuse(100);
                if(pc==0x212b50u){
                    if(!backing.loadNativeRacer814810(slot,pc))return false;
                    if(!backing.loadNativeRacer818(slot,actualParticleDirectory36B530,actualRawRandom,pc))return false;
                    // The 2129C0 tail is the same code for human and AI (16E5F4 calls
                    // 210770 -> 2129C0 exactly as 1A0848 does). The only differences
                    // are data: +58 (vehicle58) makes the AI skip 215434/214FC4, and
                    // the engine step already branches on it.
                    if(!rosterResources_->rigs_[slot])return false;
                    //Same real forward parser owner and retained PHY. End
                    //this reader lease before any nested handling borrower.
                    auto* model=nativeModel;
                    if(!model||model->actualRig()!=rosterResources_->rigs_[slot].get()||
                       nativePhyOwnerCurrent(session,*model)!=phy||actualRacer.nativePhyLoad_!=phy)return false;
                    {
                        auto reader=phy->borrow(model->nativePhyContext_,rosterResources_->rigs_[slot]->phy);
                        if(!reader||!backing.beginNativeHandling2146(slot,reader,pc))return false;
                    }
                    const bool physical=model->continueNativePhysical214710(session.racerEntities_[slot],
                        rosterResources_->rigs_[slot]->phy,pc);
                    backing.native_[slot].pc=pc; //Same reached continuation, including a retained failed prefix.
                    if(!physical)return false;
                    const bool repulsor=model->continueNativeRepulsor214894(session.racerEntities_[slot],
                        rosterResources_->rigs_[slot]->phy,*this,pc);
                    backing.native_[slot].pc=pc;
                    if(!repulsor)return false;
                    const bool handling=model->continueNativeAeroEngine214954(session.racerEntities_[slot],
                        rosterResources_->rigs_[slot]->phy,session,race,*this,pc);
                    backing.native_[slot].pc=pc;
                    if(!handling)return false;
                    const bool animation=model->continueNativeAnimationContact2154D8(session.racerEntities_[slot],
                        rosterResources_->rigs_[slot]->phy,session,*this,pc);
                    backing.native_[slot].pc=pc;
                    if(!animation||!model->nativeHandlingStack_||!nativeHandlingInputOwnerCurrent(session,*model))return false;
                    //Same collection cohort, Config selection and2146 stack.
                    //Each22C310 finishes before its own22C610; no reordered
                    //whole-roster pass or reconstructed temporary bodies.
                    bool wreck=false;
                    {
                        auto reader=phy->borrow(model->nativePhyContext_,rosterResources_->rigs_[slot]->phy);
                        if(!reader||nativePhyOwnerCurrent(session,*model)!=reader.owner())return false;
                        wreck=backing.continueNativeWreck215AF8(slot,session.racerEntities_[slot],reader,
                            model->nativeHandlingStack_->gravity47C,pc);
                    }
                    backing.native_[slot].pc=model->failurePC_=session.racerEntities_[slot].nativePostEmberPC_=pc;
                    if(!wreck||pc!=0x215bb8u)return false;
                    model->nativeWreckTuningComplete_=true;
                    const bool returned=model->continueNativeHandling215BB8(session.racerEntities_[slot],
                        rosterResources_->rigs_[slot]->phy,session,race,*this,pc);
                    backing.native_[slot].pc=pc;
                    if(!returned||pc!=0x216034u)return false;
                    //The current native Cables adapter is the retained owner
                    //of the later source allocations, not a replacement254.
                    //Stay in this exact mutable Bank visit: never enter the
                    //inactive wrapper that reparses PHY or nests a visit.
                    if(!nativeHandlingInputOwnerCurrent(session,*model)||
                       session.actual_.actualCables!=&camera_->cables||
                       !camera_->cablePins[slot]||!camera_->cables[slot]||
                       session.actual_.racer60[slot]!=session.identity_[slot].pod60){
                        pc=backing.native_[slot].pc=model->failurePC_=
                            session.racerEntities_[slot].nativePostEmberPC_=0x212df0u;
                        return false;
                    }
                    const bool outer=camera_->cables[slot]->continueNativeOuter212DF0(
                        session.racerEntities_[slot],*model,session.actual_.applicationEntities,
                        session.actual_.racer60[slot],pc);
                    //Retain every reached native/source effect, including a
                    //failed partial material/strip/cable owner. No retry of
                    //the one-shot handling or loader prefix is authorized.
                    backing.native_[slot].pc=model->failurePC_=
                        session.racerEntities_[slot].nativePostEmberPC_=pc;
                    if(!outer||pc!=0x212e10u||
                       !camera_->cables[slot]->nativeOuterReturned_||
                       camera_->cables[slot]->nativeOuterResult_.sourcePC!=pc||
                       !nativeHandlingInputOwnerCurrent(session,*model))return false;
                    const bool spin=model->continueNativeSpin212ED0(session.racerEntities_[slot],pc);
                    backing.native_[slot].pc=model->failurePC_=session.racerEntities_[slot].nativePostEmberPC_=pc;
                    if(!spin||pc!=0x212e18u)return false;
                    const bool tail=continueNativeOuterStorage212E18(session,*model,pc);
                    backing.native_[slot].pc=model->failurePC_=session.racerEntities_[slot].nativePostEmberPC_=pc;
                    // Only real native close permits the typed outer return.
                    // The forward human caller now consumes the SAME current
                    // Aero steering. Keep outer PCs separate from caller PCs.
                    if(!tail)return false;
                    if(!human){
                        // 16E5FC returns with no further work; 1AE3CC continues at 1AE4CC
                        // with the same 1AA5A0 resource pair the human uses (14D370/29E340).
                        rr::assets::OriginalStandardMaterialDiagnostic50LA resources{};
                        if(!race.populateNativeReturnedPlayerResources1AA5A0(resources))return false;
                        // STAND-IN (flagged): from 1AE4D8 on (AI+98 = 173970: 22A1D0 speech,
                        // sound object 16DA00/103FD0), the D44 row, 2C2230 and 2169C0 grid
                        // placement stay with the native AI audio owner and roster cold.
                        pc=backing.native_[slot].pc=0x1ae4d8u;
                        podAiHandlingStopped_[slot]=true;return true;
                    }
                    if(!continueNativePlayerReturn1A0850(session,*model,pc))return false;
                    if(!continueNativePlayerResources1AE104(session,*model,race,pc))return false;
                    return continueNativeBaseRacerSpeech22A1D0(session,*model,race,pc);
                }
                // Next [Wreckage] PartN (2129E0 loop). Each refusal names its step.
                auto step=[&](int at,bool ok){
                    if(!ok){std::fprintf(stderr,"pod_load_collection_step_refused slot=%zu step=%d pc=%x",slot,at,unsigned(pc));std::fputc(10,stderr);}
                    return ok;};
                if(!step(0,pc==0x2129e0u)||!step(1,backing.loadNativeNextCollectionPrefix(slot,heights,directory,pc))||
                   !step(2,backing.setupNativeBody(slot,0,zones,pc))||!step(3,backing.loadNativeQueryChildren(slot,0,pc))||
                   !step(4,backing.loadNativeEffects(slot,0,*nativeWreckParticles_,water.bit,pc))||
                   !step(5,backing.loadNativeRemainingCollectionPieces(slot,zones,heights,directory,*nativeWreckParticles_,water.bit,pc))||
                   !step(6,backing.loadNativeCollectionGroup(slot,pc))||!step(7,backing.deactivateNativeCollectionLoad(slot,pc))||
                   !step(8,backing.appendNativeCollectionToRacer(slot,pc)))return false;
            }
        });
    }catch(...){return false;}
}
}
