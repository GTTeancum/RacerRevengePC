#pragma once
#include "original_ui_main_entry50kg.h"
#include "ra_cold_aggregate50la.h"
#include <cstring>
#include "../../src/assets/original_track_cut_identity50la.h"

namespace ra_cold_aggregate50LA {
inline bool Owner::constructRuntimeSourceCamera(
    const original_main_entry50KG::Application& application,
    const ra_source_camera50LA::CameraCallerOwners& actual,
    const std::vector<std::uint8_t>& selectedOriginalCam,
    original_camera_session50KX::DisplayInputs& display,Diagnostic& d)noexcept {
    namespace c=original_camera_session50KX;
    d={};d.stage=Stage::sourceCamera;
    if(terminal_||closed_||!prefix_.runtimeQueryWorldOwners||!prefix_.lighting||
       !prefix_.trackParticleManager||!prefix_.staticWorldImages||!runtimeQueries_||
       runtimeQueries_->cameraAttempted_)
        return fail(d,"RA camera requires its existing registered runtime/light/particle/image owners");
    auto& runtime=*runtimeQueries_;
    runtime.cameraAttempted_=true;
    try {
        const auto domain=application.standardMaterialDomain();
        const auto entities=application.entities();
        const auto* source=source_?source_->sourcesDecoded():nullptr;
        if(runtime.terminal_||!runtime.queriesConstructed_||!runtime.worldOwnersRegistered_||
           &actual.bank!=runtime.bankIdentity_||!runtime.cameraBankCurrent()||
           !runtime.actual_||&actual.arena!=&runtime.actual_->frame||
           (runtime.gridArena_&&(&actual.bindings!=&runtime.gridArena_->bindings||
               &actual.ai!=&runtime.gridArena_->ai||&actual.poseHistory!=&runtime.gridArena_->history||
               &actual.actualRigs!=&runtime.gridArena_->rigs||
               actual.humanCount34!=runtime.gridArena_->humanCount))||
           !source||!lighting_||!lighting_->world()||!trackParticles_||!trackParticles_->prepared()||
           !trackParticleGeneration_||trackParticleHostApplication_!=&application||
           trackParticleDomain_!=domain||trackParticleApplication_!=entities||
           !domain||domain!=world_->domain_||!entities||!world_->context_||!world_->context_->live()||
           world_->context_->materialApplication()!=application.materialApplication||
           trackParticles_->material150()->table382B38()!=domain->materials382B38().get()||
           !domain->materials382B38()->coldCollectorCurrent(world_->imageCollector_)||
           domain->archiveProvider3E1A70()->revision()!=world_->providerRevision_||
           display.trackFog3EC||selectedOriginalCam.empty()||!actual.actualRigs[0]||actual.actualRigs[0]->phy.empty()||
           bool(actual.actualBottomRightModels)!=bool(actual.actualBottomLeftModels))
            return fail(d,"RA camera Bank/Application/context/manager or selected rig ownership differs");
        for(std::size_t i=0;i<actual.bank.count();++i){
            const auto* rig=actual.actualRigs[i].get();
            if(!rig||!actual.actualCables[i]||actual.actualCables[i]->sourceRig_!=rig||!actual.actualRightModels[i]||!actual.actualLeftModels[i]||!actual.actualChariotModels[i]||
               rig->podId.size()!=5u||rig->podId.compare(0,3,"POD")||
               rig->podId[3]<'0'||rig->podId[3]>'9'||rig->podId[4]<'0'||rig->podId[4]>'9'||
               static_cast<std::uint32_t>((rig->podId[3]-'0')*10+rig->podId[4]-'0')!=actual.racer60[i])
                return fail(d,"RA camera requires the actual active rig/model identities");
        }
        // Already authenticated from this same retained RA archive by the
        // particle authoring owner. Do not re-read another SessionAssets route.
        const auto* tob=particles_->originalMember(0);
        if(!tob)return fail(d,"same-source authenticated RA TOB unavailable");
        a::OriginalStandardArchiveProvider50LA::MemberView selected;
        a::OriginalStandardArchiveProvider50LA::MountView mount;
        const auto& provider=domain->archiveProvider3E1A70();
        if(provider->lookup132820("tracks\\ra.tob",selected)!=a::OriginalStandardArchiveProvider50LA::Lookup::present||
           !provider->mount(selected.mount,mount)||!mount.archiveSha256||
           *mount.archiveSha256!=ra_cold_sources50LA::archiveSha||mount.archiveBytes!=4604561u||
           mount.memberCount!=1010u||!selected.data||selected.size!=tob->size()||
           selected.revision!=world_->providerRevision_||std::memcmp(selected.data,tob->data(),tob->size()))
            return fail(d,"RA camera TOB differs from its actual mounted source");
        // Publish private storage before constructing a Session, so any later
        // exception keeps its real effects and authoring backing for cleanup.
        runtime.camera_.reset(new ra_source_camera50LA::OwnershipBridge::CameraBinding());
        auto& camera=*runtime.camera_;
        camera.lighting=lighting_;camera.particles=trackParticles_;
        for(std::size_t i=0;i<actual.bank.count();++i){
            camera.cablePins[i]=actual.actualCables[i]->retainSourceCamera(*actual.actualRigs[i]);
            if(!camera.cablePins[i])return fail(d,"actual persistent Cables camera lifetime unavailable");
            camera.cables[i]=actual.actualCables[i].get();
        }
        camera.cam=selectedOriginalCam;camera.tob=*tob;
        camera.selectedRig=actual.actualRigs[0].get();camera.phySnapshot=camera.selectedRig->phy;
        camera.display=&display;camera.generation=trackParticleGeneration_;
        if(!camera.fog.prepare(source_))return fail(d,"actual retained RA fog authoring rejected");
        camera.fogAddress=&camera.fog.fields_;
        display.trackFog3EC=camera.fogAddress; // Same retained field used by Session restore and fog preparation.
        const auto& phy=camera.selectedRig->phy;
        camera.authored={camera.cam.data(),camera.cam.size(),reinterpret_cast<const char*>(phy.data()),phy.size(),
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&phy)),
            camera.tob.data(),camera.tob.size(),g::OriginalCameraStartCourse::ra};
        auto& storage=*runtime.storage_.get();
        camera.session.reset(new c::Session(c::ActualOwners{
            actual.bank,actual.arena,actual.bindings,actual.baseTail,actual.ai,actual.racerTiming,
            actual.all8Manager,actual.hostRace,actual.sharedRandom,actual.attachedGlobals,actual.applicationClock,
            actual.preferences,actual.launchKind11258,actual.configuration34,actual.application350,
            *camera.lighting,storage.lifecycle,storage.mainMembership.state,actual.blur,*runtime.follow_,
            actual.poseHistory,actual.boost,actual.cycleAudio,actual.racer60,actual.humanCount34,actual.race94,
            actual.humanHeld,actual.raceHud,actual.deathDebris,entities,domain->materials382B38(),
            &application.materialDirectory36B970(),&actual.actualRigs,camera.particles.get(),
            &source->height.sourceDomain,&actual.actualRightModels,&actual.actualLeftModels,
            &actual.actualChariotModels,actual.actualBottomRightModels,actual.actualBottomLeftModels,
            application.animationClock(),&camera.cables,&source->map,{},{},{},{},{},
            actual.startMusicContext,actual.startMusicRequest,actual.recordManager40,
            actual.introAudio,actual.introAuthoring,actual.introMusicFadeRequest}));
        if(actual.introAuthoring&&!runtime.constructCurrentRaIntro174FA4(application))
            return fail(d,"same-owner RA authored intro constructor rejected");
        // No prepare, pose, viewport publication, RNG/clock sampling or
        // source-ready flag. Only an actual retained Session now exists.
        return true;
    }catch(...){return fail(d,"RA camera construction failed; actual private prefix retained for cleanup");}
}
} // namespace ra_cold_aggregate50LA

namespace ra_source_camera50LA {
inline bool OwnershipBridge::constructCurrentRaIntro174FA4(
    const original_main_entry50KG::Application& application)noexcept {
    namespace c=original_camera_session50KX;namespace a=rr::assets;
    // This private method is reached only through this retained RA world's
    // actual factory. Never adopt an externally constructed or shared TA Session.
    if(terminal_||closed_||cameraPrepareTerminal_||!camera_||!camera_->session||
       !cameraBankCurrent()||!camera_->generation||!camera_->display||
       !freshLighting_||camera_->lighting!=freshLighting_||!camera_->fog.current()||
       camera_->display->trackFog3EC!=&camera_->fog.fields_)return false;
    auto& session=*camera_->session;auto& actual=session.actual_;
    const auto domain=application.standardMaterialDomain();
    const auto provider=domain?domain->archiveProvider3E1A70():nullptr;
    const auto* identity=a::originalTrackCutIdentity50LA("RA");
    if(!identity||identity->cutCount!=7||!provider||!provider->sealed()||
       session.nativeTaIssuance_||session.nativeTaCutAuthoring_||session.generation_||
       session.coldBound_||session.closed_||session.coldController_.started||
       camera_->authored.course!=g::OriginalCameraStartCourse::ra||
       actual.configuration34!=2||actual.launchKind11258!=1||!actual.taIntroAudio||
       !actual.taIntroAuthoring||actual.taIntroAuthoring->trackId()!="RA"||
       actual.taIntroAuthoring->providerIdentity()!=provider.get()||
       actual.applicationEntities!=application.entities()||
       actual.applicationMaterials!=domain->materials382B38()||
       !actual.applicationAnimation.sameOwner(application.animationClock())||
       !actual.startMusicRequest||!actual.introMusicFadeRequest)return false;
    try {
        using P=a::OriginalStandardArchiveProvider50LA;
        std::array<P::MemberView,8> views{};
        std::array<a::OriginalTaCutMemberInputCandidate,8> inputs{};
        for(std::size_t i=0;i<views.size();++i){const auto& expected=identity->members[i];auto& v=views[i];
            P::MountView mount;
            if(provider->lookup132820(expected.name,v)!=P::Lookup::present||!v.member||
               *v.member!=expected.name||!v.data||v.size!=expected.size||v.ordinal!=expected.ordinal||
               !v.bankGeneration||v.revision!=provider->revision()||!provider->mount(v.mount,mount)||
               !mount.archiveName||*mount.archiveName!=identity->archiveName||!mount.archiveSha256||
               *mount.archiveSha256!=identity->archiveSha256||mount.archiveBytes!=identity->archiveBytes||
               mount.memberCount!=identity->memberCount||
               (i&&(v.mount!=views[0].mount||v.bankGeneration!=views[0].bankGeneration)))return false;
            inputs[i]={*v.member,v.data,v.size};
        }
        if(views[0].size!=camera_->tob.size()||std::memcmp(views[0].data,camera_->tob.data(),views[0].size))return false;
        a::OriginalTaCutAuthoringCandidate::Status status;
        auto authored=a::OriginalTaCutAuthoringCandidate::create("RA",inputs.data(),inputs.size(),status);
        if(!authored||authored->cutCount()!=7||authored->trackId()!="RA")return false;
        auto pin=std::make_unique<c::Session::NativeTaIssuance>();
        pin->issuer=&application;pin->provider=provider;pin->generation=camera_->generation;
        pin->providerRevision=views[0].revision;pin->bankGeneration=views[0].bankGeneration;
        pin->mount=views[0].mount;pin->ordinal=views[0].ordinal;pin->course=g::OriginalCameraStartCourse::ra;
        pin->selectedRig=camera_->selectedRig;pin->particles=camera_->particles.get();pin->selectedPod=actual.racer60[0];
        pin->cam=camera_->cam;pin->tob=camera_->tob;pin->phy=camera_->phySnapshot;
        auto custody=std::make_unique<c::Session::NativeTaCutAuthoringCustody>();
        custody->provider=provider;custody->generation=pin->generation;custody->providerRevision=pin->providerRevision;
        custody->bankGeneration=pin->bankGeneration;custody->mount=pin->mount;
        for(std::size_t i=0;i<views.size();++i)custody->members[i]={*views[i].member,views[i].ordinal,views[i].size};
        custody->authoring=std::move(authored);
        // Recheck the actual mounted winners after copying, before publishing
        // custody or performing any174A30 source count/Entity constructor effect.
        for(std::size_t i=0;i<views.size();++i){P::MemberView current;
            if(provider->lookup132820(identity->members[i].name,current)!=P::Lookup::present||
               current.data!=views[i].data||current.size!=views[i].size||current.mount!=pin->mount||
               current.ordinal!=views[i].ordinal||current.revision!=pin->providerRevision||
               current.bankGeneration!=pin->bankGeneration)return false;
        }
        session.nativeTaIssuance_=std::move(pin);session.nativeTaCutAuthoring_=std::move(custody);
        return session.constructColdController174A30(camera_->generation)&&session.constructTaOwners174FA4();
    }catch(...){return false;} // Published source prefixes remain owned for actual close.
}
} // namespace ra_source_camera50LA
