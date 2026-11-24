#pragma once
// Common rewrite (R2, B-C09): the ONE Application-issued course camera Session,
// for every course and every launch (1A6DC0 -> 1A95B0 -> 174DF0/174E58 ->
// 174A30, +174FA4 when the cut count is nonzero). Built from the former GC
// factory (the one with the full checks). Replaces the 12
// createNativeXxCameraSession50LA factories and RA's own world-built Session.
// The public root wrapper below retains authenticated authoring and allocates
// the actual controller/cuts. Root audio and Session::prepare then complete
// 177750/175510/1752D0; immutable custody alone is not admission.
#include "original_ui_main_entry50kg.h"
#include "original_camera_assets50kx.h"
#include "original_race_constructor50la.h"
#include "common_track50cr.h"
#include "common_race_world50cr.h"
#include "common_race_log50cr.h"
#include "stock_course_receipts50cr.h"
#include "../../src/assets/original_ta_cut_authoring_candidate.h"
#include <cstring>
#include <iostream>
#include <memory>
#include <span>
#include <string_view>
#include <thread>

namespace original_main_entry50KG::ta_camera_issuer_detail {
// The race's own field size (148r, same rule as 147c's Binder b4f426c): 1AD520
// loads 1..8 racers (a Time Trial is a solo field, AI 0); the session already
// requires every loaded racer bound (racerBank.count() == raceRacerCount).
inline bool fieldSized(std::size_t racers) noexcept {return racers>=1u&&racers<=8u;}
template<class A,class B> bool sameOwner(const std::shared_ptr<A>& a,
    const std::shared_ptr<B>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
// Native input admission only, without invoking Follow's source reader or
// constructing a camera. The stock CAM catalog is a receipt (the Follow
// reader logs non-stock bytes); admission here is the Follow reader's own
// native text capacity (8 KiB), the same for every pod.
inline bool camIdentity(std::uint32_t,const std::uint8_t* bytes,std::size_t size)noexcept {
    return bytes&&size>0&&size<8192;
}
inline bool camIdentity(std::uint32_t pod,const std::vector<std::uint8_t>& bytes)noexcept {
    return camIdentity(pod,bytes.data(),bytes.size());
}
using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
// The World's TOB member (World+A4.., read by the 1A9EE0 load) is the mounted
// winner of tracks\<code>.tob in the Application's provider: same bytes as the
// Track's TOB document, mounted from the Track's own archive (its SHA, size and
// member count). One rule for every course; no catalogue row is required, so a
// zero-cut course is issued as well.
inline bool trackMember(const Provider& provider,const common_track50CR::Track& track,
    Provider::MemberView& out)noexcept {
    try {
        const auto& name=track.identity.tobMember;
        const auto& tob=track.documents.tob;
        Provider::MountView mount;
        return !name.empty()&&!tob.empty()&&provider.sealed()&&
            provider.lookup132820(name,out)==Provider::Lookup::present&&
            out.member&&*out.member==name&&out.data&&out.size==tob.size()&&
            !std::memcmp(out.data,tob.data(),tob.size())&&
            out.revision==provider.revision()&&out.bankGeneration&&provider.mount(out.mount,mount)&&
            mount.archiveSha256&&*mount.archiveSha256==track.archive.sha256()&&
            mount.archiveBytes==track.archive.bytes()&&mount.memberCount==track.archive.memberCount();
    }catch(...){return false;}
}
// common_load step=06 refusal line (first refusal names step/role/owner R2).
inline std::shared_ptr<original_camera_session50KX::Session> refuse(std::string_view why)noexcept {
    try{common_race_log50CR::load(common_race_log50CR::Step::cameraSession1A95B0,false,0,why);}catch(...){}
    return {};
}
// common_track_data role=cuts: the count 174E58 reads (World+3E4), with the
// stock receipt for a stock course (receipt only, never a size or a gate).
inline void logCuts(const common_track50CR::Track& track,std::uint32_t cuts)noexcept {
    try {
        const auto code=track.code();
        const auto* r=track.source()==common_track50CR::Source::stock?
            stock_course_receipts50CR::find(code,"cuts"):nullptr;
        common_race_log50CR::trackData(common_race_log50CR::Role::cuts,code,track.identity.tobMember,cuts,
            r!=nullptr,r?r->value:0u);
    }catch(...){}
}
}
namespace original_main_entry50KG {
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeCourseCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const rr::game::OriginalRaceConstructor50LA& race,
    const common_race_world50CR::World& world74,
    std::span<const std::uint8_t> tob,
    const common_track50CR::NavigationView& map,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    // 1A6DC0: the race's own counted World (race+74) and its loaded Track.
    const auto* track=world74.trackData();
    if(race28C()!=&race||race.world74_.get()!=&world74||!race.nativeIntroRaceEntity()||
       !common_race_world50CR::introWorldEntity(race)||!track)return d::refuse("race, World or track not current");
    // The one map view (race B8 radar/start map), the same owner the Session borrows.
    if(!map.runtimeValid()||map.role()!=common_track50CR::NavigationRole::map||
       !map.sameOwner(actual.actualRadarMap))return d::refuse("map view not runtime-valid or not the session's");
    // The World's TOB member: the bytes the caller passes, the authored copy and
    // the Track's TOB document are one and the same.
    const auto& trackTob=track->documents.tob;
    if(tob.empty()||tob.size()!=trackTob.size()||std::memcmp(tob.data(),trackTob.data(),tob.size())||
       authored.tob.size()!=tob.size()||std::memcmp(authored.tob.data(),tob.data(),tob.size()))
        return d::refuse("TOB differs from the World's TOB member");
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's race preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       !d::fieldSized(actual.bank.count())||!actual.hostRace.runtimeValid()||
       !(authored.inputs.code==track->identity.code)||
       authored.cam.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size())return d::refuse("Application, owners or authored inputs differ");
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    const auto* worldEntity=common_race_world50CR::introWorldEntity(race);
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race.nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       worldEntity->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,*track,member)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))
        return d::refuse("particles, TOB mount or selected pod differ");
    // 174E58 reads the cut count from World+3E4 (TOB [CSCameras].NumberOfCameras).
    // A positive count needs the course's own derived cut row with that count;
    // zero is the zero-cut controller (174F9C skips 174FA4). 174EA8 then zeroes
    // the cuts for subtypes 2/3 and subtype 1 with P+34 != 2 or App+350 != 0
    // (OriginalCameraStartPrelude::prepare174DF0), so RA Free Play / Time Trial
    // get the zero-cut controller with no course code.
    const auto cuts=world74.cutCount3E4();
    d::logCuts(*track,cuts);
    if(cuts>7u)return d::refuse("cut count above the native 7-cut storage");
    if(cuts){
        const auto* row=rr::assets::originalTrackCutIdentity50LA(track->code());
        if(!row||row->cutCount!=cuts)return d::refuse("no cut row derived from the course archive with the World's cut count");
    }
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;
        pin->code=track->identity.code;pin->race=&race;pin->world=&world74;pin->map=map;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        // 1A95B0 -> 174A30 (+174FA4) here, so the controller exists when the
        // factory returns (one controller per human, race+1B0 low2; zero humans
        // build none). 174E48..174EC4: the World's count unless 174EA8 zeroes it
        // from launch data. A surviving count builds the cut owners from the
        // retained intro authoring; a zero count builds the controller alone.
        const auto effectiveCuts=session->launchZeroesCuts174EA8()?0u:cuts;
        bool built=true;
        if(effectiveCuts)built=bool(session->actual_.taIntroAuthoring)&&constructNativeTaIntroCamera50LA();
        else if(session->actual_.humanCount34){
            const auto fresh=freshNativeTaCameraSession50LA();
            built=fresh&&fresh==session&&session->constructCountedControllerWithoutCuts174A30();
        }
        if(!built){
            // The Session (and any partly built controller) retires with this
            // last owner; the Application keeps no issued Session.
            nativeTaCameraSession50LA_.reset();
            return d::refuse(effectiveCuts?"174A30/174FA4 cut controller construction rejected":
                "174A30 zero-cut controller construction rejected");
        }
        common_race_log50CR::load(common_race_log50CR::Step::cameraSession1A95B0,true,effectiveCuts,"issued");
        return session;
    }catch(...){nativeTaCameraSession50LA_.reset();return d::refuse("native allocation failed");}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::freshNativeTaCameraSession50LA()const noexcept {
    namespace d=ta_camera_issuer_detail;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return {};
    const auto session=nativeTaCameraSession50LA_.lock();
    // CRITICAL: closed/prepared/abandoned rejection precedes ANY dereference of
    // captured root-owned Rig/particle borrows. The root closes particles
    // immediately after Session retirement even while other Session borrowers survive.
    if(!session||session->closed_||session->abandoned_||session->coldBound_||
       session->generation_||session->displayInputs_||session->coldPhase_||session->authoredPhase_||
       session->readsBound_||session->coldController_.started||
       session->nativeTaCutChildPhase_!=original_camera_session50KX::Session::NativeTaCutChildPhase::empty||
       session->frameStage_!=original_camera_session50KX::Session::FrameStage::idle||
       !session->nativeTaIssuance_)return {};
    const auto& pin=*session->nativeTaIssuance_;const auto& actual=session->actual_;
    // The race, its one World and its map view recorded at issuance, still current.
    const auto* race=race28C();
    if(!race||race!=pin.race||!pin.world||race->world74_.get()!=pin.world||
       !race->nativeIntroRaceEntity()||!common_race_world50CR::introWorldEntity(*race)||
       !pin.map.sameOwner(actual.actualRadarMap))return {};
    const auto* track=pin.world->trackData();
    if(!track||!(track->identity.code==pin.code))return {};
    if(pin.issuer!=this||!pin.generation||!standardMaterialDomainOwner_||
       !standardMaterialDomainOwner_->prepared()||!d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       !d::sameOwner(pin.provider,standardMaterialDomainOwner_->archiveProvider3E1A70())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||(*actual.actualRigs)[0].get()!=pin.selectedRig||
       !pin.particles||actual.actualTrackParticles4A0!=pin.particles)return {};
    const auto& particles=*pin.particles;const auto& rig=*pin.selectedRig;
    const auto* pod=original_player_identity50IK::find(rig.podId);
    d::Provider::MemberView member;
    if(!particles.prepared()||particles.generation_!=pin.generation||
       particles.component_.sessionGeneration!=pin.generation||particles.component_.reference00!=1||
       !d::sameOwner(particles.application_,entitiesOwner_)||
       !d::sameOwner(particles.table_,standardMaterialDomainOwner_->materials382B38())||
       !pod||pod->racer60!=pin.selectedPod||actual.racer60[0]!=pin.selectedPod||rig.phy!=pin.phy||
       !d::trackMember(*pin.provider,*track,member)||member.revision!=pin.providerRevision||
       member.bankGeneration!=pin.bankGeneration||member.mount!=pin.mount||member.ordinal!=pin.ordinal||
       member.size!=pin.tob.size()||std::memcmp(member.data,pin.tob.data(),member.size))return {};
    return session;
}
inline bool Application::constructNativeTaController174A30()const noexcept {
    // Shared admission performs the same native freshness and
    // exact Application-issued ownership checks before any constructor effect.
    const auto session=freshNativeTaCameraSession50LA();
    return session&&session->constructColdController174A30(session->nativeTaIssuance_->generation)&&
        session->constructTaOwners174FA4();
}
inline bool Application::retainNativeTaCutAuthoring50LA()const noexcept {
    namespace d=ta_camera_issuer_detail;
    namespace a=rr::assets;
    using Session=original_camera_session50KX::Session;
    const auto session=freshNativeTaCameraSession50LA();
    if(!session||session->nativeTaCutAuthoring_)return false;
    const auto& pin=*session->nativeTaIssuance_;
    // Exact selected-track directory order, not caller-selected members/handles.
    const auto* identity=rr::assets::originalTrackCutIdentity50LA(original_camera_session50KX::introTrackId(pin.code));
    if(!identity||!identity->cutCount||identity->cutCount>7)return false;
    const auto memberCount=static_cast<std::size_t>(identity->cutCount)+1u;
    try {
        std::array<d::Provider::MemberView,8> views{};
        std::array<a::OriginalTaCutMemberInputCandidate,8> inputs{};
        for(std::size_t i=0;i<memberCount;++i){
            auto& v=views[i];
            if(pin.provider->lookup132820(identity->members[i].name,v)!=d::Provider::Lookup::present||
               !v.member||*v.member!=identity->members[i].name||!v.data||v.size!=identity->members[i].size||
               v.ordinal!=identity->members[i].ordinal||v.mount!=pin.mount||
               v.revision!=pin.providerRevision||v.bankGeneration!=pin.bankGeneration)return false;
            inputs[i]={*v.member,v.data,v.size};
        }
        if(views[0].ordinal!=pin.ordinal||views[0].size!=pin.tob.size()||
           std::memcmp(views[0].data,pin.tob.data(),pin.tob.size()))return false;
        // All copied bytes are independently hashed by the existing immutable
        // owner. This performs no V2 floating-point load or process-global read.
        a::OriginalTaCutAuthoringCandidate::Status status;
        auto authoring=a::OriginalTaCutAuthoringCandidate::create(identity->trackId,inputs.data(),memberCount,status);
        if(!authoring)return false;
        auto next=std::make_unique<Session::NativeTaCutAuthoringCustody>();
        next->provider=pin.provider;next->generation=pin.generation;
        next->providerRevision=pin.providerRevision;next->bankGeneration=pin.bankGeneration;
        next->mount=pin.mount;
        for(std::size_t i=0;i<memberCount;++i)
            next->members[i]={*views[i].member,views[i].ordinal,views[i].size};
        next->authoring=std::move(authoring);
        // Retained bytes do not prove a still-active mount. Revalidate the
        // original same-owner admission and each current member after owned
        // allocation/copying, before the sole custody publication.
        const auto current=freshNativeTaCameraSession50LA();
        if(!d::sameOwner(current,session)||session->nativeTaCutAuthoring_)return false;
        for(std::size_t i=0;i<memberCount;++i){
            d::Provider::MemberView currentMember;
            if(pin.provider->lookup132820(identity->members[i].name,currentMember)!=d::Provider::Lookup::present||
               !currentMember.member||*currentMember.member!=identity->members[i].name||
               currentMember.data!=views[i].data||currentMember.size!=views[i].size||
               currentMember.mount!=pin.mount||currentMember.ordinal!=identity->members[i].ordinal||
               currentMember.revision!=pin.providerRevision||
               currentMember.bankGeneration!=pin.bankGeneration)return false;
        }
        session->nativeTaCutAuthoring_=std::move(next);
        return true; // Native immutable custody only, not any source return/ready flag.
    }catch(...){return false;} // Native allocation/lookup failure leaves Session unchanged.
}
inline bool Application::abortUnpublishedNativeTaCutChild()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto session=nativeTaCameraSession50LA_.lock();
    // Abort is valid even after close. It must never run fresh-owner checks
    // that dereference already-retired root Rig/particle borrows.
    if(!session||!session->nativeTaIssuance_||session->nativeTaIssuance_->issuer!=this)return false;
    return session->abortUnpublishedTaCutChildNative();
}
// One consumed root entry: immutable source custody, actual174A30, real74
// constructor allocation. Session next constructs actual audio80 and cue5C.
inline bool Application::constructNativeTaIntroCamera50LA()const noexcept {
    return retainNativeTaCutAuthoring50LA()&&constructNativeTaController174A30();
}
} // namespace original_main_entry50KG
