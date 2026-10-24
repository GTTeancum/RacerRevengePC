#pragma once
// Native single-Session allocation/custody seam only. The source174A30
// constructor entry below is private and UNCALLED. No source loader/clock,
// attachment, audio publication, renderer callback or admission is supplied.
#include "original_ui_main_entry50kg.h"
#include "original_camera_assets50kx.h"
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
using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
inline bool taMember(const Provider& provider,Provider::MemberView& out)noexcept {
    try {
    Provider::MountView mount;
    return provider.sealed()&&provider.lookup132820("tracks\\ta.tob",out)==Provider::Lookup::present&&
        out.member&&*out.member=="tracks\\ta.tob"&&out.data&&out.size==13887u&&
        out.revision==provider.revision()&&out.bankGeneration&&provider.mount(out.mount,mount)&&
        mount.archiveName&&*mount.archiveName=="TRACKS/TA.RES"&&mount.archiveSha256&&
        *mount.archiveSha256=="b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8"&&
        mount.archiveBytes==3334562u&&mount.memberCount==383u&&
        rr::assets::ta_cut_authoring_candidate_detail::hashMatches(out.data,out.size,
            "11e4c4909bf866fee626977c970d9907d4f191c29eeff32bddd162f173916f6b");
    }catch(...){return false;} // std::string lookup argument allocation is native-only.
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
        pin->selectedRig=&rig;pin->particles=actual.actualTrackParticles4A0;pin->selectedPod=pod->racer60;
        pin->cam=authored.cam;pin->tob=authored.tob;pin->phy=rig.phy;
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
       !d::taMember(*pin.provider,member)||member.revision!=pin.providerRevision||
       member.bankGeneration!=pin.bankGeneration||member.mount!=pin.mount||member.ordinal!=pin.ordinal||
       member.size!=pin.tob.size()||std::memcmp(member.data,pin.tob.data(),member.size))return {};
    return session;
}
inline bool Application::constructNativeTaController174A30()const noexcept {
    // Still UNCALLED. Shared admission performs the same V8 freshness and
    // exact Application-issued ownership checks before any constructor effect.
    const auto session=freshNativeTaCameraSession50LA();
    return session&&session->constructColdController174A30(session->nativeTaIssuance_->generation);
}
inline bool Application::retainNativeTaCutAuthoring50LA()const noexcept {
    namespace d=ta_camera_issuer_detail;
    namespace a=rr::assets;
    using Session=original_camera_session50KX::Session;
    const auto session=freshNativeTaCameraSession50LA();
    if(!session||session->nativeTaCutAuthoring_)return false;
    const auto& pin=*session->nativeTaIssuance_;
    // Exact original TA directory order, not caller-selected members/handles.
    constexpr std::array<const char*,4> names{{
        "tracks\\ta.tob","tracks\\ta01.vub","tracks\\ta02.vub","tracks\\ta03.vub"}};
    constexpr std::array<std::size_t,4> ordinals{{0,376,377,378}};
    constexpr std::array<std::size_t,4> sizes{{13887,53488,17528,17528}};
    try {
        std::array<d::Provider::MemberView,4> views{};
        std::array<a::OriginalTaCutMemberInputCandidate,4> inputs{};
        for(std::size_t i=0;i<views.size();++i){
            auto& v=views[i];
            if(pin.provider->lookup132820(names[i],v)!=d::Provider::Lookup::present||
               !v.member||*v.member!=names[i]||!v.data||v.size!=sizes[i]||
               v.ordinal!=ordinals[i]||v.mount!=pin.mount||
               v.revision!=pin.providerRevision||v.bankGeneration!=pin.bankGeneration)return false;
            inputs[i]={*v.member,v.data,v.size};
        }
        if(views[0].ordinal!=pin.ordinal||views[0].size!=pin.tob.size()||
           std::memcmp(views[0].data,pin.tob.data(),pin.tob.size()))return false;
        // All copied bytes are independently hashed by the existing immutable
        // owner. This performs no V2 floating-point load or process-global read.
        a::OriginalTaCutAuthoringCandidate::Status status;
        auto authoring=a::OriginalTaCutAuthoringCandidate::create(inputs,status);
        if(!authoring)return false;
        auto next=std::make_unique<Session::NativeTaCutAuthoringCustody>();
        next->provider=pin.provider;next->generation=pin.generation;
        next->providerRevision=pin.providerRevision;next->bankGeneration=pin.bankGeneration;
        next->mount=pin.mount;
        for(std::size_t i=0;i<views.size();++i)
            next->members[i]={*views[i].member,views[i].ordinal,views[i].size};
        next->authoring=std::move(authoring);
        // Retained bytes do not prove a still-active mount. Revalidate the
        // original same-owner admission and each current member after owned
        // allocation/copying, before the sole custody publication.
        const auto current=freshNativeTaCameraSession50LA();
        if(!d::sameOwner(current,session)||session->nativeTaCutAuthoring_)return false;
        for(std::size_t i=0;i<views.size();++i){
            d::Provider::MemberView currentMember;
            if(pin.provider->lookup132820(names[i],currentMember)!=d::Provider::Lookup::present||
               !currentMember.member||*currentMember.member!=names[i]||
               currentMember.data!=views[i].data||currentMember.size!=views[i].size||
               currentMember.mount!=pin.mount||currentMember.ordinal!=ordinals[i]||
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
    return session&&session->constructUnpublishedTaCutChild282860();
}
inline bool Application::abortUnpublishedNativeTaCutChild()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto session=nativeTaCameraSession50LA_.lock();
    // Abort is valid even after close. It must never run fresh-owner checks
    // that dereference already-retired root Rig/particle borrows.
    if(!session||!session->nativeTaIssuance_||session->nativeTaIssuance_->issuer!=this)return false;
    return session->abortUnpublishedTaCutChildNative();
}
} // namespace original_main_entry50KG
