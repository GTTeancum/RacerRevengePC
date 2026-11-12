#pragma once
// Same-Application TA Session issuance and consumed source constructor entry.
// The public root wrapper below retains authenticated authoring and allocates
// the actual controller/three cuts. Root audio and Session::prepare
// then complete177750/175510/1752D0; immutable custody alone is not admission.
#include "mc_camera_assets50mc.h"
#include "original_ui_main_entry50kg.h"
#include "original_camera_assets50kx.h"
#include "original_race_constructor50la.h"
#include "../../src/assets/original_ta_cut_authoring_candidate.h"
#include <cstring>
#include <memory>
#include <thread>

namespace original_main_entry50KG::ta_camera_issuer_detail {
template<class A,class B> bool sameOwner(const std::shared_ptr<A>& a,
    const std::shared_ptr<B>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
// Reuse the exact original CAM identity catalog, without invoking Follow's
// source reader or constructing a camera. This is native input admission only.
struct CamIdentity {std::int32_t pod;std::size_t bytes;const char* sha256;};
#include "../../src/game/original_follow_cam_catalog.inc"
inline bool camIdentity(std::uint32_t pod,const std::vector<std::uint8_t>& bytes)noexcept {
    for(const auto& row:catalog)if(row.pod>=0&&static_cast<std::uint32_t>(row.pod)==pod)
        return bytes.size()==row.bytes&&rr::assets::ta_cut_authoring_candidate_detail::hashMatches(
            bytes.data(),bytes.size(),row.sha256);
    return false;
}
inline bool camIdentity(std::uint32_t pod,const std::uint8_t* bytes,std::size_t size)noexcept {
    for(const auto& row:catalog)if(row.pod>=0&&static_cast<std::uint32_t>(row.pod)==pod)
        return bytes&&size==row.bytes&&rr::assets::ta_cut_authoring_candidate_detail::hashMatches(bytes,size,row.sha256);
    return false;
}
using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
inline bool trackMember(const Provider& provider,rr::game::OriginalCameraStartCourse course,
    Provider::MemberView& out)noexcept {
    const auto* identity=rr::assets::originalTrackCutIdentity50LA(original_camera_session50KX::introTrackId(course));
    if(!identity)return false;
    const auto& expected=identity->members[0];
    try {
        Provider::MountView mount;
        return provider.sealed()&&provider.lookup132820(expected.name,out)==Provider::Lookup::present&&
            out.member&&*out.member==expected.name&&out.data&&out.size==expected.size&&out.ordinal==expected.ordinal&&
            out.revision==provider.revision()&&out.bankGeneration&&provider.mount(out.mount,mount)&&
            mount.archiveName&&*mount.archiveName==identity->archiveName&&mount.archiveSha256&&
            *mount.archiveSha256==identity->archiveSha256&&mount.archiveBytes==identity->archiveBytes&&
            mount.memberCount==identity->memberCount&&
            rr::assets::ta_cut_authoring_candidate_detail::hashMatches(out.data,out.size,expected.sha256);
    }catch(...){return false;}
}
inline bool taMember(const Provider& provider,Provider::MemberView& out)noexcept {
    return trackMember(provider,rr::game::OriginalCameraStartCourse::ta,out);
}

}
namespace original_main_entry50KG {
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeTaCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected TA preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::ta||authored.inputs.mcSourceIdentity||
       authored.cam.empty()||authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size())return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::taMember(*provider,member)||
       member.size!=authored.tob.size()||std::memcmp(member.data,authored.tob.data(),member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::ta;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeGcCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    if(!race||!race->nativeGcWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       !actual.actualRadarMapB8||!actual.actualRadarMapB8->gatesPrepared||!actual.actualRadarMapB8->indicesPrepared||
       actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapGb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected GC preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::gc||authored.inputs.mcSourceIdentity||
       authored.cam.empty()||authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size())return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::gc,member)||
       member.size!=authored.tob.size()||std::memcmp(member.data,authored.tob.data(),member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::gc;
        pin->gcRace=race;pin->gcWorld=race->nativeGcWorldBacking();pin->gcMap=actual.actualRadarMapB8;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeGaCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    if(!race||!race->nativeGaWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       !actual.actualRadarMapB8||!actual.actualRadarMapB8->gatesPrepared||!actual.actualRadarMapB8->indicesPrepared||
       actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapGb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected GA preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::ga||authored.inputs.mcSourceIdentity||
       authored.cam.empty()||authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size())return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::ga,member)||
       member.size!=authored.tob.size()||std::memcmp(member.data,authored.tob.data(),member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::ga;
        pin->gaRace=race;pin->gaWorld=race->nativeGaWorldBacking();pin->gaMap=actual.actualRadarMapB8;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeSaCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    if(!race||!race->nativeSaWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       !actual.actualRadarMapB8||!actual.actualRadarMapB8->gatesPrepared||!actual.actualRadarMapB8->indicesPrepared||
       actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapGb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected SA preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::sa||authored.inputs.mcSourceIdentity||
       authored.cam.empty()||authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size())return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::sa,member)||
       member.size!=authored.tob.size()||std::memcmp(member.data,authored.tob.data(),member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::sa;
        pin->saRace=race;pin->saWorld=race->nativeSaWorldBacking();pin->saMap=actual.actualRadarMapB8;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeMaCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    if(!race||!race->nativeMaWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       !actual.actualRadarMapB8||!actual.actualRadarMapB8->gatesPrepared||!actual.actualRadarMapB8->indicesPrepared||
       actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapGb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected MA preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::ma||authored.inputs.mcSourceIdentity||
       authored.cam.empty()||authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size())return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::ma,member)||
       member.size!=authored.tob.size()||std::memcmp(member.data,authored.tob.data(),member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::ma;
        pin->maRace=race;pin->maWorld=race->nativeMaWorldBacking();pin->maMap=actual.actualRadarMapB8;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeMbCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    // MB keeps its retained map as a view: never the B8 pointer.
    if(!race||!race->nativeMbWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       actual.actualRadarMapB8||!actual.actualRadarMapMb.gatesPrepared()||!actual.actualRadarMapMb.indicesPrepared()||
       actual.actualRadarMapMb.role()!=rr::game::NavigationRole50MB::Map||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapGb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected MB preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::mb||authored.inputs.mcSourceIdentity||
       authored.cam.empty()||authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size())return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::mb,member)||
       member.size!=authored.tob.size()||std::memcmp(member.data,authored.tob.data(),member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::mb;
        pin->mbRace=race;pin->mbWorld=race->nativeMbWorldBacking();pin->mbMap=actual.actualRadarMapMb;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeScCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    if(!race||!race->nativeScWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       !actual.actualRadarMapB8||!actual.actualRadarMapB8->gatesPrepared||!actual.actualRadarMapB8->indicesPrepared||
       actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapGb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected GC preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::sc||authored.inputs.mcSourceIdentity||
       authored.cam.empty()||authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size())return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::sc,member)||
       member.size!=authored.tob.size()||std::memcmp(member.data,authored.tob.data(),member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::sc;
        pin->scRace=race;pin->scWorld=race->nativeScWorldBacking();pin->scMap=actual.actualRadarMapB8;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeGbCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    // GB keeps its retained map as a view: never the B8 pointer.
    if(!race||!race->nativeGbWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       actual.actualRadarMapB8||!actual.actualRadarMapGb.gatesPrepared()||!actual.actualRadarMapGb.indicesPrepared()||
       actual.actualRadarMapGb.role()!=rr::game::NavigationRole50GB::Map||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected GB preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::gb||authored.inputs.mcSourceIdentity||
       // GB's TOB stays borrowed from the retained GB cold source (loadGb leaves tob empty).
       authored.cam.empty()||!authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||!authored.inputs.tob||authored.inputs.tobBytes!=20617u)return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::gb,member)||
       member.size!=authored.inputs.tobBytes||std::memcmp(member.data,authored.inputs.tob,member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::gb;
        pin->gbRace=race;pin->gbWorld=race->nativeGbWorldBacking();pin->gbMap=actual.actualRadarMapGb;
        pin->cam=authored.cam;pin->tob.assign(authored.inputs.tob,authored.inputs.tob+authored.inputs.tobBytes);pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeTbCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    // TB keeps its retained map as a view: never the B8 pointer.
    if(!race||!race->nativeTbWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       actual.actualRadarMapB8||!actual.actualRadarMapTb.gatesPrepared()||!actual.actualRadarMapTb.indicesPrepared()||
       actual.actualRadarMapTb.role()!=rr::game::NavigationRole50TB::Map||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapGb.gatesPrepared()||actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected TB preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::tb||authored.inputs.mcSourceIdentity||
       // TB's TOB stays borrowed from the retained TB cold source (loadTb leaves tob empty).
       authored.cam.empty()||!authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||!authored.inputs.tob||authored.inputs.tobBytes!=106139u)return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::tb,member)||
       member.size!=authored.inputs.tobBytes||std::memcmp(member.data,authored.inputs.tob,member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::tb;
        pin->tbRace=race;pin->tbWorld=race->nativeTbWorldBacking();pin->tbMap=actual.actualRadarMapTb;
        pin->cam=authored.cam;pin->tob.assign(authored.inputs.tob,authored.inputs.tob+authored.inputs.tobBytes);pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeMcCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const mc_camera_assets50MC::Owner& assets)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    const auto* in=assets.inputs();
    if(!in||!assets.prepared()||!in->mcSourceIdentity||assets.sourceIdentity()!=in->mcSourceIdentity)return {};
    // MC keeps its retained map as a view: never the B8 pointer.
    if(!race||!race->nativeMcWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       actual.actualRadarMapB8||!actual.actualRadarMapMc.gatesPrepared()||!actual.actualRadarMapMc.indicesPrepared()||
       actual.actualRadarMapMc.role()!=rr::game::NavigationRole50MC::Map||actual.actualRadarMapMc.count()!=173u||
       actual.actualRadarMapMc.sourceIdentity()!=in->mcSourceIdentity||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapGb.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected MC preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       // MC's CAM is owned by its assets owner; its TOB is borrowed from the retained MC cold source.
       in->course!=rr::game::OriginalCameraStartCourse::mc||!in->cam||!in->camBytes||
       !in->tob||in->tobBytes!=186642u)return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::mc,member)||
       member.size!=in->tobBytes||std::memcmp(member.data,in->tob,member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       in->phy!=reinterpret_cast<const char*>(rig.phy.data())||
       in->phyBytes!=rig.phy.size()||in->phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,in->cam,in->camBytes)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::mc;
        pin->mcRace=race;pin->mcWorld=race->nativeMcWorldBacking();pin->mcMap=actual.actualRadarMapMc;pin->mcSource=in->mcSourceIdentity;
        pin->cam.assign(in->cam,in->cam+in->camBytes);pin->tob.assign(in->tob,in->tob+in->tobBytes);pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeRcCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    // RC uses its retained B8 navigation map (as SC/GC), no map view.
    if(!race||!race->nativeRcWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       !actual.actualRadarMapB8||!actual.actualRadarMapB8->gatesPrepared||!actual.actualRadarMapB8->indicesPrepared||
       actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapTc.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapGb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected RC preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::rc||authored.inputs.mcSourceIdentity||
       // RC's TOB stays borrowed from the retained RC cold source (loadRc leaves tob empty).
       authored.cam.empty()||!authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||!authored.inputs.tob||authored.inputs.tobBytes!=124930u)return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::rc,member)||
       member.size!=authored.inputs.tobBytes||std::memcmp(member.data,authored.inputs.tob,member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::rc;
        pin->rcRace=race;pin->rcWorld=race->nativeRcWorldBacking();pin->rcMap=actual.actualRadarMapB8;
        pin->cam=authored.cam;pin->tob.assign(authored.inputs.tob,authored.inputs.tob+authored.inputs.tobBytes);pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::createNativeTcCameraSession50LA(
    original_camera_session50KX::ActualOwners actual,
    const original_camera_session50KX::AuthoredStorage& authored)const noexcept {
    namespace c=original_camera_session50KX;
    namespace d=ta_camera_issuer_detail;
    const auto* race=race28C();
    // TC keeps its retained map as a view: never the B8 pointer.
    if(!race||!race->nativeTcWorldBacking()||!race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
       actual.actualRadarMapB8||!actual.actualRadarMapTc.gatesPrepared()||!actual.actualRadarMapTc.indicesPrepared()||
       actual.actualRadarMapTc.role()!=rr::game::NavigationRole50TC::Map||actual.actualRadarMapGb.gatesPrepared()||
       actual.actualRadarMapTb.gatesPrepared()||actual.actualRadarMapMb.gatesPrepared()||actual.actualRadarMapMc.gatesPrepared())return {};
    // No lazy domain creation: only the actual already-retained Application
    // domain/provider used by this root's selected TC preparation is eligible.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!nativeTaCameraSession50LA_.expired()||
       !enabled||!entitiesOwner_||!entitiesOwner_->application10()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !d::sameOwner(actual.applicationEntities,entitiesOwner_)||
       !d::sameOwner(actual.applicationMaterials,standardMaterialDomainOwner_->materials382B38())||
       actual.actualMaterialDirectory36B970!=&materialDirectory36B970_||materialDirectory36B970_!="textures\\"||
       !actual.applicationAnimation.sameOwner(animationClock())||
       &actual.applicationClock!=animationClockBackend_.get()||
       !actual.actualRigs||!(*actual.actualRigs)[0]||!actual.actualTrackParticles4A0||
       actual.bank.count()!=8||!actual.hostRace.runtimeValid()||
       authored.inputs.course!=rr::game::OriginalCameraStartCourse::tc||authored.inputs.mcSourceIdentity||
       // TC's TOB is the authored storage's own authenticated copy (as SC).
       authored.cam.empty()||authored.tob.empty()||authored.inputs.cam!=authored.cam.data()||
       authored.inputs.camBytes!=authored.cam.size()||authored.inputs.tob!=authored.tob.data()||
       authored.inputs.tobBytes!=authored.tob.size()||authored.tob.size()!=25512u)return {};
    const auto& particles=*actual.actualTrackParticles4A0;
    const auto& rig=*(*actual.actualRigs)[0];
    const auto* pod=original_player_identity50IK::find(rig.podId);
    const auto& table=standardMaterialDomainOwner_->materials382B38();
    const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    d::Provider::MemberView member;
    if(!particles.prepared()||!particles.generation_||!particles.entityConstructed_||
       race->nativeIntroRaceEntity()->sessionGeneration!=particles.generation_||
       race->nativeIntroWorldEntity()->sessionGeneration!=particles.generation_||
       !d::sameOwner(particles.application_,entitiesOwner_)||!d::sameOwner(particles.table_,table)||
       particles.component_.reference00!=1||particles.component_.sessionGeneration!=particles.generation_||
       particles.component_.kind!=rr::game::OriginalWreckComponentKind50LA::particleManager160B10||
       table->populationTerminal()||table->demandTerminal()||!provider||!d::trackMember(*provider,rr::game::OriginalCameraStartCourse::tc,member)||
       member.size!=authored.inputs.tobBytes||std::memcmp(member.data,authored.inputs.tob,member.size)||
       !pod||pod->racer60!=actual.racer60[0]||rig.phy.empty()||rig.phy.size()>1024u*1024u||
       authored.inputs.phy!=reinterpret_cast<const char*>(rig.phy.data())||
       authored.inputs.phyBytes!=rig.phy.size()||authored.inputs.phyIdentity!=
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&rig.phy))||
       !d::camIdentity(pod->racer60,authored.cam)||
       !rr::assets::ta_cut_authoring_candidate_detail::hashMatches(rig.phy.data(),rig.phy.size(),pod->phySha256))return {};
    try {
        // Reserve/copy immutable input custody before allocating the ONE Session.
        // No source constructor, registry, counter, RNG or renderer effect occurs.
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=this;pin->provider=provider;pin->generation=particles.generation_;
        pin->providerRevision=member.revision;pin->bankGeneration=member.bankGeneration;
        pin->mount=member.mount;pin->ordinal=member.ordinal;
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;pin->course=rr::game::OriginalCameraStartCourse::tc;
        pin->tcRace=race;pin->tcWorld=race->nativeTcWorldBacking();pin->tcMap=actual.actualRadarMapTc;
        pin->cam=authored.cam;pin->tob.assign(authored.inputs.tob,authored.inputs.tob+authored.inputs.tobBytes);pin->phy=rig.phy;
        auto session=std::make_shared<c::Session>(actual);
        session->nativeTaIssuance_=std::move(pin);
        // Publication only after all fallible native allocation. Weak ownership
        // cannot extend root Rig/particle lifetime or create an Application cycle.
        nativeTaCameraSession50LA_=session;
        return session;
    }catch(...){return {};}
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::freshNativeTaCameraSession50LA()const noexcept {
    namespace d=ta_camera_issuer_detail;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return {};
    const auto session=nativeTaCameraSession50LA_.lock();
    // CRITICAL: closed/prepared/abandoned rejection precedes ANY dereference of
    // captured root-owned Rig/particle borrows. TA closes particles immediately
    // after Session retirement even while other shared Session borrowers survive.
    if(!session||session->closed_||session->abandoned_||session->coldBound_||
       session->generation_||session->displayInputs_||session->coldPhase_||session->authoredPhase_||
       session->readsBound_||session->coldController_.started||
       session->nativeTaCutChildPhase_!=original_camera_session50KX::Session::NativeTaCutChildPhase::empty||
       session->frameStage_!=original_camera_session50KX::Session::FrameStage::idle||
       !session->nativeTaIssuance_)return {};
    const auto& pin=*session->nativeTaIssuance_;const auto& actual=session->actual_;
    if(pin.course==rr::game::OriginalCameraStartCourse::gc){
        const auto* race=race28C();
        if(!race||race!=pin.gcRace||!pin.gcWorld||race->nativeGcWorldBacking()!=pin.gcWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
           !pin.gcMap||actual.actualRadarMapB8!=pin.gcMap)return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::ga){
        const auto* race=race28C();
        if(!race||race!=pin.gaRace||!pin.gaWorld||race->nativeGaWorldBacking()!=pin.gaWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
           !pin.gaMap||actual.actualRadarMapB8!=pin.gaMap)return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::sa){
        const auto* race=race28C();
        if(!race||race!=pin.saRace||!pin.saWorld||race->nativeSaWorldBacking()!=pin.saWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
           !pin.saMap||actual.actualRadarMapB8!=pin.saMap)return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::ma){
        const auto* race=race28C();
        if(!race||race!=pin.maRace||!pin.maWorld||race->nativeMaWorldBacking()!=pin.maWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
           !pin.maMap||actual.actualRadarMapB8!=pin.maMap)return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::sc){
        const auto* race=race28C();
        if(!race||race!=pin.scRace||!pin.scWorld||race->nativeScWorldBacking()!=pin.scWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
           !pin.scMap||actual.actualRadarMapB8!=pin.scMap)return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::gb){
        const auto* race=race28C();
        if(!race||race!=pin.gbRace||!pin.gbWorld||race->nativeGbWorldBacking()!=pin.gbWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||actual.actualRadarMapB8||
           !pin.gbMap.sameOwner(actual.actualRadarMapGb))return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::tb){
        const auto* race=race28C();
        if(!race||race!=pin.tbRace||!pin.tbWorld||race->nativeTbWorldBacking()!=pin.tbWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||actual.actualRadarMapB8||
           !pin.tbMap.sameOwner(actual.actualRadarMapTb))return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::mc){
        const auto* race=race28C();
        if(!race||race!=pin.mcRace||!pin.mcWorld||race->nativeMcWorldBacking()!=pin.mcWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||actual.actualRadarMapB8||
           !pin.mcMap.sameOwner(actual.actualRadarMapMc)||!pin.mcSource||
           pin.mcSource!=actual.actualRadarMapMc.sourceIdentity())return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::rc){
        const auto* race=race28C();
        if(!race||race!=pin.rcRace||!pin.rcWorld||race->nativeRcWorldBacking()!=pin.rcWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
           !pin.rcMap||actual.actualRadarMapB8!=pin.rcMap)return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::tc){
        const auto* race=race28C();
        if(!race||race!=pin.tcRace||!pin.tcWorld||race->nativeTcWorldBacking()!=pin.tcWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||actual.actualRadarMapB8||
           !pin.tcMap.sameOwner(actual.actualRadarMapTc))return {};
    }
    if(pin.course==rr::game::OriginalCameraStartCourse::mb){
        const auto* race=race28C();
        if(!race||race!=pin.mbRace||!pin.mbWorld||race->nativeMbWorldBacking()!=pin.mbWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||actual.actualRadarMapB8||
           !pin.mbMap.sameOwner(actual.actualRadarMapMb))return {};
    }
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
       !d::trackMember(*pin.provider,pin.course,member)||member.revision!=pin.providerRevision||
       member.bankGeneration!=pin.bankGeneration||member.mount!=pin.mount||member.ordinal!=pin.ordinal||
       member.size!=pin.tob.size()||std::memcmp(member.data,pin.tob.data(),member.size))return {};
    return session;
}
inline std::shared_ptr<original_camera_session50KX::Session> Application::resourceNativeTaCameraSession50LA()const noexcept {
    namespace d=ta_camera_issuer_detail;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return {};
    const auto session=nativeTaCameraSession50LA_.lock();
    // Separate constructed-child admission; the original fresh helper is unchanged.
    // CRITICAL: closed/prepared/abandoned rejection precedes ANY dereference of
    // captured root-owned Rig/particle borrows. TA closes particles immediately
    // after Session retirement even while other shared Session borrowers survive.
    if(!session||session->closed_||session->abandoned_||session->coldBound_||
       session->generation_||session->displayInputs_||session->coldPhase_||session->authoredPhase_||
       session->readsBound_||session->coldController_.started||
       session->nativeTaCutChildPhase_!=original_camera_session50KX::Session::NativeTaCutChildPhase::constructed||
       !session->nativeUnpublishedTaCutChild_||!session->nativeTaCutAuthoring_||
       !session->nativeTaCutAuthoring_->authoring||
       session->nativeUnpublishedTaCutChild_->resourcePhase!=
           original_camera_session50KX::Session::NativeUnpublishedTaCutChild::ResourcePhase::unloaded||
       session->frameStage_!=original_camera_session50KX::Session::FrameStage::idle||
       !session->nativeTaIssuance_)return {};
    const auto& pin=*session->nativeTaIssuance_;const auto& actual=session->actual_;
    // This optional historical probe owns only TA01's461-frame reservation.
    // Production GC uses its distinct dynamic four-child175510 owner instead.
    if(pin.course!=rr::game::OriginalCameraStartCourse::ta)return {};
    if(pin.course==rr::game::OriginalCameraStartCourse::gc){
        const auto* race=race28C();
        if(!race||race!=pin.gcRace||!pin.gcWorld||race->nativeGcWorldBacking()!=pin.gcWorld||
           !race->nativeIntroRaceEntity()||!race->nativeIntroWorldEntity()||
           !pin.gcMap||actual.actualRadarMapB8!=pin.gcMap)return {};
    }
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
       !d::trackMember(*pin.provider,pin.course,member)||member.revision!=pin.providerRevision||
       member.bankGeneration!=pin.bankGeneration||member.mount!=pin.mount||member.ordinal!=pin.ordinal||
       member.size!=pin.tob.size()||std::memcmp(member.data,pin.tob.data(),member.size))return {};
    const auto& custody=*session->nativeTaCutAuthoring_;
    if(!d::sameOwner(custody.provider,pin.provider)||custody.generation!=pin.generation||
       custody.providerRevision!=pin.providerRevision||custody.bankGeneration!=pin.bankGeneration||
       custody.mount!=pin.mount)return {};
    // Retained exact bytes alone are not a current mount/epoch certificate.
    const auto* identity=rr::assets::originalTrackCutIdentity50LA(original_camera_session50KX::introTrackId(pin.course));
    if(!identity||!identity->cutCount||identity->cutCount>7)return {};
    const auto memberCount=static_cast<std::size_t>(identity->cutCount)+1u;
    try {
        for(std::size_t i=0;i<memberCount;++i){
            const auto& retained=custody.members[i];
            const auto bytes=i?custody.authoring->originalVubBytes(static_cast<unsigned>(i)):
                custody.authoring->originalTobBytes();
            d::Provider::MemberView current;
            if(retained.name!=identity->members[i].name||retained.ordinal!=identity->members[i].ordinal||retained.bytes!=identity->members[i].size||
               !bytes.data||bytes.size!=identity->members[i].size||
               custody.provider->lookup132820(identity->members[i].name,current)!=d::Provider::Lookup::present||
               !current.member||*current.member!=identity->members[i].name||!current.data||current.size!=identity->members[i].size||
               current.ordinal!=identity->members[i].ordinal||current.mount!=pin.mount||
               current.revision!=pin.providerRevision||current.bankGeneration!=pin.bankGeneration||
               std::memcmp(current.data,bytes.data,bytes.size))return {};
        }
    }catch(...){return {};}
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
    const auto* identity=rr::assets::originalTrackCutIdentity50LA(original_camera_session50KX::introTrackId(pin.course));
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
inline bool Application::constructUnpublishedNativeTaCutChild282860()const noexcept {
    // Authentic same-Application fresh Session admission, before touching the
    // actual process owner. Unloaded282860 consumes no VUB or numeric receipt.
    const auto session=freshNativeTaCameraSession50LA();
    return session&&session->nativeTaIssuance_->course==rr::game::OriginalCameraStartCourse::ta&&
        session->constructUnpublishedTaCutChild282860();
}
inline bool Application::deriveUnpublishedNativeTa01Resources()const noexcept {
    namespace d=ta_camera_issuer_detail;
    namespace r=rr::game::original_ta_cut_resource_candidate_detail;
    // UNCALLED. Fixed native TA01 association is NOT a source74 index or a
    // reached175510 call. The one constructed child is never replaced.
    const auto session=resourceNativeTaCameraSession50LA();
    if(!session||!r::arithmeticAdmitted())return false;
    auto* const child=session->nativeUnpublishedTaCutChild_.get();
    auto reserved=r::reserveTa01();
    if(!reserved)return false; //Native pre-effect reservation failure only.
    // Allocation is followed by current same-owner/epoch/closed-state admission
    // again. Every failure here frees local backing, not the actual child.
    const auto current=resourceNativeTaCameraSession50LA();
    if(!d::sameOwner(current,session)||current->nativeUnpublishedTaCutChild_.get()!=child)return false;
    const auto& actualProcess=current->actual_.applicationEntities;
    if(!actualProcess||!actualProcess->application10()||
       actualProcess->nativeOwnerThread_!=std::this_thread::get_id())return false;
    return child->deriveReservedTa01(*current->nativeTaCutAuthoring_->authoring,
        actualProcess->cutOrigin443200_,std::move(reserved));
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
