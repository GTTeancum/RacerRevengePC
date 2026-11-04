#include "original_camera_session50kx.h"
#include "original_race_world_join50la.h"
#include "ra_cold_aggregate50la.h"
#include "ra_fresh_execution_arena50la.h"
#include "../../src/game/original_loading_manager33050la.h"
#include "original_race_display_lifetime50la.h"
#include <array>
#include "../../src/game/original_native_collision_categories.h"
#include <cstring>
#include "original_source_particle_config_invocation50la.inl"
#include <cstdio>
#include <string>
#include <thread>
#include "original_source_track_native_mount50la.inl"
#include "original_source_world_boundary50la.inl"
#include "original_source_world_light_lifetime50la.inl"
#include "original_source_world_light_descriptor50la.inl"
#include "original_source_world_lights50la.inl"
#include "original_source_world_fog50la.inl"
#include "original_source_world_fog_transfer50la.inl"
#include "original_source_world_visibility_environment50la.inl"
#include "original_source_world_post_light50la.inl"
#include "original_source_world_post_particle_models50la.inl"
#include "original_source_world_standard_model50la.inl"
#include "original_source_world_standard_success50la.inl"
#include "original_source_world_model_observer50la.inl"
#include "original_source_world_standard_attachment50la.inl"
#include "original_source_particle_manager_suffix50la.inl"
#include "original_source_world_zone_row50la.inl"
#include "original_source_world_portals50la.inl"
#include "original_source_world_physics50la.inl"
#include "original_source_world_instances50la.inl"
#include "original_source_world_static_models50la.inl"
#include "original_source_world_physics_wrapper50la.inl"
#include "original_source_world_static_cohort50la.inl"
#include "original_source_world_collision50la.inl"
#include "original_source_world_controllers50la.inl"
#include "original_source_world_objects50la.inl"
#include "original_source_world_terrain50la.inl"
#include "original_source_world_toy_custody50la.inl"
#include "original_source_world_toy50la.inl"
#include "original_source_world_trigger_instance_custody50la.inl"
#include "original_source_world_trigger_action_custody50la.inl"
#include "original_source_world_trigger_custody50la.inl"
#include "original_source_world_trigger50la.inl"
#include "original_source_world_trigger_zero_euler50la.inl"
#include "original_source_world_trigger_instance50la.inl"
#include "original_source_world_trigger_action50la.inl"
#include "original_source_world_trigger_cohort50la.inl"
#include "original_source_world_physicsobjects50la.inl"
#include "original_source_world_cscameras50la.inl"
#include "original_source_world_sunflare_custody50la.inl"
#include "original_source_world_sunflare50la.inl"
#include "original_source_world_sunflare_material50la.inl"
#include "original_source_world_sunflare_packet50la.inl"
#include "original_source_world_sunflare_depth50la.inl"
#include "original_source_world_sunflare_retrieve50la.inl"
#include "original_source_world_sunflare_saturation50la.inl"
#include "original_source_world_particle_count50la.inl"
#include "original_source_world_particle_descriptors50la.inl"
#include "original_source_world_buoys50la.inl"
#include "original_source_world_static_shadow50la.inl"
#include "original_source_world_object_close50la.inl"
#include "original_source_world_audio50la.inl"
#include "original_source_world_ambient_count50la.inl"
#include "original_source_world_ambient50la.inl"
#include "original_source_world_soultree_sound_count50la.inl"
#include "original_source_world_soultree_sound50la.inl"
#include "original_source_world_material_sound_count50la.inl"
#include "original_source_world_material_sound50la.inl"
#include "original_source_world_audio_return50la.inl"
#include "original_source_world_search_node_count50la.inl"
#include "original_source_world_search_nodes50la.inl"
#include "original_source_world_portal_owner50la.inl"
#include "original_source_world_portal_geometry50la.inl"
#include "original_source_world_portal_graph50la.inl"
#include "original_source_world_list_methods50la.inl"
#include "original_source_world_method_node_attachments50la.inl"
#include "original_source_world_method_arrays50la.inl"
#include "original_source_world_default_observer50la.inl"
#include "original_source_world_first_light50la.inl"
#include "original_source_world_typed_light50la.inl"
#include "original_source_world_spotlight_config50la.inl"
#include "original_source_world_spotlight_registration50la.inl"
#include "original_source_world_later_ambient50la.inl"
#include "original_source_world_directional_tail50la.inl"
#include "original_source_world_no_flare_wrapper50la.inl"
#include "original_source_world_second_light50la.inl"
#include "original_source_world_light_population50la.inl"
#include "original_source_world_topology50la.inl"


namespace ra_cold_aggregate50LA {
bool Owner::nativeTrackArchiveCurrent(const original_main_entry50KG::Application& app)const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    try {
    //These are the existing native resource owners, NOT receipts for the
    //source12CEA0/132A20/132E40 cache protocol or World slot30/270550 reset.
    if(sourceTrackStep_!=SourceTrackStep::unreached||sourceTrackFile_||sourceWorldConfig_||
       nativeScbRevoked_||terminal_||closed_||!worldColdConstructed_||!worldReset270550Complete_||
       worldApplication_!=app.entities()||!source_||!source_->sourcesDecoded()||
       !prefix_.sources||!prefix_.staticWorld||!prefix_.staticWorldImages||
       !rosterMountComplete_||rosterMountApplication_!=&app||!rosterMountDomain_||
       !rosterMountDomain_->prepared()||!world_||
       world_->source_!=source_||world_->domain_!=rosterMountDomain_||
       rosterMountDomain_!=app.standardMaterialDomain()||!world_->coldImagesCurrent())return false;
    const auto provider=rosterMountDomain_->archiveProvider3E1A70();
    if(!provider||!provider->sealed()||provider->revision()!=world_->providerRevision_)return false;
    std::size_t matches=0;
    for(std::size_t i=0;i<provider->mountCount();++i){
        a::OriginalStandardArchiveProvider50LA::MountView mount;
        if(!provider->mount(i,mount))continue; //Retired native mount tombstone.
        if(!mount.archiveName||*mount.archiveName!="TRACKS/RA.RES")continue;
        if(!mount.archiveSha256||*mount.archiveSha256!=ra_cold_sources50LA::archiveSha||
           mount.archiveBytes!=4604561u||mount.memberCount!=1010u)return false;
        ++matches;
    }
    return matches==1u;
    }catch(...){return false;} //A stale Application's domain accessor may throw.
}
bool Owner::bindNativeTrackScb34(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1b399c;
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    try {
    if(!nativeTrackArchiveCurrent(app)||nativeTob34_)return false;
    const auto provider=rosterMountDomain_->archiveProvider3E1A70();
    if(!nativeScb34_){
        nativeScb34_.emplace(); //Native parser-role publication BEFORE load.
        worldParser34_=&*nativeScb34_;
        //The existing native entry starts1B399C, AFTER unjoined sprintf
        //1B38DC/F4. Revoke constructor string knowledge rather than pretend
        //the skipped filename producers wrote any particular loaded bytes.
        for(auto& byte:worldText3C_)byte.reset();
        for(auto& byte:worldText140_)byte.reset();
        nativeScb34_->source=source_;nativeScb34_->provider=provider;
    }
    auto& reader=*nativeScb34_;
    if(!worldParser34_||*worldParser34_!=&reader||reader.source!=source_||reader.provider!=provider)return false;
    pc=0x1b39a8; //Actual first1577D0 call; native adapter reads retained bytes.
    const auto* decoded=source_->sourcesDecoded();
    a::OriginalStandardArchiveProvider50LA::MemberView member;
    a::OriginalStandardArchiveProvider50LA::MountView mount;
    if(!decoded||provider->lookup132820("tracks\\ra.scb",member)!=
           a::OriginalStandardArchiveProvider50LA::Lookup::present||
       !provider->mount(member.mount,mount)||!mount.archiveName||
       *mount.archiveName!="TRACKS/RA.RES"||!mount.archiveSha256||
       *mount.archiveSha256!=ra_cold_sources50LA::archiveSha||
       mount.archiveBytes!=4604561u||mount.memberCount!=1010u||
       !member.data||member.size!=ra_cold_sources50LA::sources[0].bytes||
       member.revision!=world_->providerRevision_)return false;
    const auto& bytes=decoded->memberBytes[0];
    const auto& document=decoded->world.parameters;
    if(bytes.size()!=member.size||document.original_bytes!=bytes||
       std::memcmp(member.data,bytes.data(),bytes.size()))return false;
    if(reader.document){
        //Reentry checks the SAME completed observation; it does not publish,
        //decode, reset populated topology, or replay any source load call.
        const bool same=reader.document==&document&&reader.member.data==member.data&&
            reader.member.member==member.member&&reader.member.size==member.size&&
            reader.member.mount==member.mount&&reader.member.ordinal==member.ordinal&&
            reader.member.revision==member.revision&&reader.member.bankGeneration==member.bankGeneration;
        if(same)pc=0x1aa078;
        return same;
    }
    reader.member=member;reader.document=&document;
    pc=0x1aa078;return true; //Before World slot80/270900, NOT1A9EE0 return.
    }catch(...){
        //lookup's implicit std::string construction is fallible even when a
        //particular STL uses small-string storage. Retain any published reader
        //prefix; its document remains null until successful validation.
        return false;
    }
}
}

namespace rr::game {
bool OriginalRaceConstructor50LA::nativeTrackArchiveCurrent(
    const original_main_entry50KG::Application& app)const noexcept {
    return resultsFeedbackThread_==std::this_thread::get_id()&&nativeIntroWorldEntity()&&
        world74_&&world74_->nativeTrackArchiveCurrent(app);
}
bool OriginalRaceConstructor50LA::bindNativeTrackScb34(
    const original_main_entry50KG::Application& app,std::uint32_t& pc)noexcept {
    return resultsFeedbackThread_==std::this_thread::get_id()&&nativeIntroWorldEntity()&&
        world74_&&world74_->bindNativeTrackScb34(app,pc);
}
}

namespace original_main_entry50KG {
bool Application::completeConsumedRaWorldNativePrefix1A9EE0(
    original_camera_session50KX::Session& session,std::uint32_t& pc)const noexcept {
    using Step=rr::game::OriginalRaceConstructor50LA::NativeTrackLoadStep;
    pc=0x1a6cc8;
    //Immutable affinity checks precede every mutable Application/Race borrow.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       !session.onOwnerThread())return false;
    if(session.coldPhase_!=1u)return false;
    const auto race=sourceRace28COwner_;
    //Preserve the existing explicitly unprepared native-only route. This
    //predicate proves same custody plus NEVER-attempted providers and no10C;
    //false currentness alone is never permission to skip a stale/partial load.
    if(consumedRaMusicNativeOnlyNoPreparation(session))
        return race&&race->nativeTrackLoadStep_==Step::unreached;
    if(!consumedRaMusicNative10CCurrent(session))return false;
    if(!race||race->resultsFeedbackThread_!=std::this_thread::get_id()||
       !race->ownsNativeCameraAdapter34(session)||race->profile_!=riderProfile||
       !riderProfile)return false;
    try {
        std::array<char,3> name{};
        pc=0x1a6d00;
        if(!riderProfile->selectedTrackName187330(name)||name!=std::array<char,3>{{'r','a','\0'}})
            return false; //This real retained loader is RA-only, not a numeric track projection.
        pc=0x1a6d48; //Native archive replacement interval, not an EE cache receipt.
        if(race->nativeTrackLoadStep_==Step::unreached){
            //Explicit Windows/Linux adaptation: bounded native progress and
            //the ALREADY authenticated retained provider replace Application330
            //and the EE archive-cache protocol. No source330 null or success is
            //invented; no remount/redecode mutates current material borrowers.
            if(trackDirectory36B640_!="tracks\\"||!race->nativeTrackArchiveCurrent(*this))return false;
            race->nativeTrackName_=name;
            race->nativeTrackLoadStep_=Step::archive;
        }else if(race->nativeTrackName_!=name||!race->nativeTrackArchiveCurrent(*this))return false;
        if(race->nativeTrackLoadStep_==Step::archive){
            //1A9F68..A8 saves min(strlen,259) bytes and a terminator BEFORE
            //the reached2E0 read. Native retry retains that exact preimage.
            const auto nul=modelDirectory36B860_.find('\0');
            const auto sourceLength=nul==std::string::npos?modelDirectory36B860_.size():nul;
            const auto length=sourceLength<259u?sourceLength:259u;
            std::memcpy(race->savedModelDirectory1A9EE0_.data(),modelDirectory36B860_.data(),length);
            race->savedModelDirectory1A9EE0_[length]='\0';
            race->nativeTrackLoadStep_=Step::savedDirectory;
        }
        if(race->nativeTrackLoadStep_==Step::savedDirectory){
            pc=0x1a9fb0;
            //Only the genuine normal-UI startup producer is admitted. Native
            //CLI arguments never become the original predefined-track path.
            if(!sourceUseProfile2E0_||*sourceUseProfile2E0_!=1u)return false;
            trackDirectory36B640_="tracks\\"; //Both original branches set this literal.
            pc=0x1a9fe0;
            if(!riderProfile->selectedTrackName187330(name)||name!=race->nativeTrackName_)return false;
            //World slot30 would clear source publication fields. This native
            //backend already owns populated topology/resources, so lifecycle-
            //checked same-owner adoption REPLACES that reset; it never replays
            //270550 over live4C0/4C4/particle/geometry owners.
            race->nativeTrackLoadStep_=Step::scb;
        }
        if(race->nativeTrackLoadStep_==Step::scb){
            pc=0x1aa050; //Second actual Profile187330 read before slot7C.
            if(!riderProfile->selectedTrackName187330(name)||name!=race->nativeTrackName_||
               !race->bindNativeTrackScb34(*this,pc))return false;
        }
        //The native cursor never republishes SCB after its real temporary-role
        //withdrawal. Reviewed same-owner SceneInfo adoption precedes that cut.
        //Only the represented native-adapted1A9EE0 route may return here.
        //Actual Race30/114770 and1A95B0 remain separate owner obligations.
        if(!race->continueNativeTrackAfterScb(*this,pc))return false;
        pc=0x1a6d98;
        // Native preparation only. The orchestration's bool is not a loader
        // completion token: demand its exact retained terminal cursor AND
        // the completed same-owner tail observation before touching Session.
        if(race->nativeTrackLoadStep_!=Step::nativeAdaptedReturn)return false;
        std::uint32_t tailPC=0;
        if(!race->continueNativeTrackAfterWater(*this,tailPC)||tailPC!=0x1aa508){pc=tailPC;return false;}
        // Source constructor30 is zero, but the complete original reachable
        // writer history is NOT established here. Do not read field30_ as
        // authority, skip114770, or claim1A95B0/174DF0 returned. The native
        // root already owns ONE counted174A30 and its real cut/audio children;
        // prepare those existing owners without allocating another controller.
        const auto& issued=session.nativeTaIssuance_;
        auto& controller=session.coldController_;
        const auto& entity=controller.entity;
        if(session.nativeRaOwnedPreparationAttempted_||session.closed_||session.abandoned_||
           session.coldBound_||session.authoredPhase_||
           session.frameStage_!=original_camera_session50KX::Session::FrameStage::idle||
           !issued||issued->issuer!=this||issued->course!=rr::game::OriginalCameraStartCourse::ra||
           issued->generation!=race->entity_.sessionGeneration||session.generation_!=issued->generation||
           session.actual_.applicationEntities!=entitiesOwner_||
           !session.actual_.applicationAnimation.sameOwner(animationClock())||
           !session.readsBound_||!session.identitiesCurrent()||
           !controller.started||controller.released||controller.issuerGeneration!=issued->generation||
           controller.nativeLoadOperandsPublished||controller.nativeLoadOperandsRevoked||
           controller.nativeScene3COwner||controller.race88||controller.scene3C||
           !race->camera34_||race->cameraNativeAdapter34_!=&session||
           race->nativeTaController48_!=&entity||
           entity.kind!=rr::game::OriginalWreckComponentKind50LA::cameraController174A30||
           entity.reference00!=1||entity.sessionGeneration!=issued->generation||
           entity.objectKey!=session.key(session.controller_)||entity.parent1C||entity.previous10||entity.next14||
           !entitiesOwner_||entitiesOwner_->attachmentLock423F58()||
           !entitiesOwner_->entityAuthority_.canRetire(entity)||
           (race->flags1B0KnownMask_&3u)!=3u||(race->flags1B0KnownValue_&3u)!=1u||
           session.actual_.humanCount34!=1u||race->field8C_!=0u||session.raceMode8C_!=0||
           !session.taCuts_||session.taCuts_->loaded()||session.taCuts_->trackId()!="RA"||
           session.taCuts_->cutCount()!=7u||controller.children64[4]||controller.children64[3]||
           !session.nativeCinematographer70_||session.nativeCinematographer70Published_||
           !session.nativeIntroBackingCurrent()||
           !session.actual_.taIntroAuthoring||session.actual_.taIntroAuthoring->trackId()!="RA"||
           !session.nativeIntroFirstChildCurrent())return false;
        // Native late custody of process330 constructor/resources only. The
        // prefix ends BEFORE114A00; it cannot satisfy1CC900 or load return.
        if(!stageNativeLoading330Prefix(pc))return false;
        session.nativeRaOwnedPreparationAttempted_=true;
        //One-shot SAME-controller source prefix under explicit native adoption.
        //Not proof that the still-unjoined Race30/1A95B0 caller reached here.
        if(!session.beginSameControllerLoad174DF0(race.get(),race->camera34_.get(),race->field8C_,pc))return false;
        std::uint32_t actualWorldCount=0;
        if(!race->readSameControllerWorldCount174E48(session,actualWorldCount,pc)){
            controller.nativeLoadPrefixPC=pc;return false;
        }
        if(!session.publishSameControllerWorldCount174E58(actualWorldCount,pc))return false;
        //Original profile reads occur AFTER raw A8 publication. Do not eagerly
        //require mode/App350 on subtype2/3, or App350 when mode==2.
        std::uint32_t subtype=0,mode=0;std::int32_t process350=0;
        pc=controller.nativeLoadPrefixPC=0x174e6c;
        if(!riderProfile->resultsSubtype11258(subtype)||subtype!=session.actual_.launchKind11258)return false;
        if(subtype==1u){
            pc=controller.nativeLoadPrefixPC=0x174e90;
            const auto* common=riderProfile->commonProfile();
            if(!common||!common->known||common->mode34!=session.actual_.configuration34)return false;
            mode=common->mode34;
            if(mode!=2u){pc=controller.nativeLoadPrefixPC=0x174e9c;process350=session.actual_.application350;}
        }
        if(!session.finishSameControllerProfile174EC0(subtype,mode,process350,pc))return false;
        if(!session.adoptNativeCamera6468(pc))return false;
        if(!session.adoptNativeAttached6CAnd70(pc))return false;
        if(!session.completeNativeControllerChildren177750(pc))return false;
        //Separate explicit native adaptation of the SAME existing first
        //racer, NOT source fallthrough or proof of1A6D98/1A95B0 reachability.
        //No a2/a3 caller operands are invented: this prefix stops before them.
        if(!session.actual_.bank.visit(0,[&](){return session.adoptNativeRacerCamera1750A0(pc);}))return false;
        //Explicit native loader adoption, after the actual same-World water
        //return. It never turns planned cold arrays into source publications.
        if(!session.nativeWreckOwner_||!race->stageNativeFirstWreckLoad(*this,session,*session.nativeWreckOwner_,pc))return false;
        // The actual base receiver's shared+98 now reaches a real detached
        // D48 constructor/publication, but remains pending at2C1170. No load,
        // attachment, Player+9C/D40 ownership or full+98 return is inferred.
        // Native human return/resource scans are not source Player/Config
        // history. No phase5, camera continuation or Restart readiness follows.
        return false;
        //The independent source corridor remains fenced at1A6D98; native
        //backing adoption does not prove1A95B0 or174DF0 returned.
    }catch(...){return false;}
}
}

namespace original_camera_session50KX {
bool Session::completeCountedRaceWorldNativePrefix1A9EE0(std::uint32_t& pc)noexcept {
    pc=0x1a6cc8;
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_||nativeTaIssuance_->course!=OriginalCameraStartCourse::ra)return true;
    std::uint32_t pause=0;
    return nativeTaIssuance_->issuer&&actual_.applicationAnimation.readPause11C(pause)&&
        nativeTaIssuance_->issuer->completeConsumedRaWorldNativePrefix1A9EE0(*this,pc);
}
}

namespace rr::game {
bool OriginalRaceConstructor50LA::readSameControllerWorldCount174E48(
    const original_camera_session50KX::Session& session,std::uint32_t& count,std::uint32_t& pc)const noexcept {
    pc=0x174e48;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!world74_||!application_||
       !ownsNativeCameraAdapter34(session)||world74_->terminal_||world74_->closed_||
       world74_->worldApplication_!=application_||
       world74_->worldEntity_.sessionGeneration!=entity_.sessionGeneration||
       !world74_->nativeCsCameraCount3E4_||!world74_->nativeCsCameraTable3E4_||
       !world74_->nativeCsCameraZones3E8_)return false;
    const auto& table=*world74_->nativeCsCameraTable3E4_;
    if(table.sourceMode||table.session!=&session||table.authoring!=session.taCutAuthoring()||
       table.next!=table.zones.size()||*world74_->nativeCsCameraZones3E8_!=&table.zones)return false;
    //Read actual World lane published2723FC; authoring alone is NOT this value.
    count=*world74_->nativeCsCameraCount3E4_;return true;
}
}

// Separate native cold inventory/query lifecycle. These hooks do not execute
//2129C0's source allocations or publish racer374, collection38, or any pieceD0.
namespace original_camera_session50KX {
bool Session::stageNativeRetainedWreckCold()noexcept {
    return !nativeWreckOwner_||nativeWreckOwner_->stageNativeRetainedWreckCold(*this);
}
bool Session::retireNativePlayerSpeechCold()noexcept {
    return !nativeWreckOwner_||nativeWreckOwner_->retireNativePlayerSpeechCold(*this);
}
bool Session::retireNativeRetainedWreckCold()noexcept {
    return !nativeWreckOwner_||nativeWreckOwner_->retireNativeRetainedWreckCold(*this);
}
}
namespace ra_source_camera50LA {
bool OwnershipBridge::stageNativeRetainedWreckCold(original_camera_session50KX::Session& session)noexcept {
    try {
    if(sourceIncomingZonesAttempted_||sourceIncomingZonesWithdrawing_||sourceWreckConstructorAttempted_)return false; //Distinct fresh chronology cannot enter legacy Follow adapter.
    if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent()||closed_||terminal_||
       cameraPrepareTerminal_||nativeWreckRetired_||!storage_.get()||!follow_||!queriesConstructed_||
       session.actual_.followQueries.get()!=follow_.get()||!rosterResources_||!rosterResources_->wreckBacking_||
       !session.readsBound_||!session.identitiesCurrent()||session.closed_||session.abandoned_||
       !camera_->generation||session.generation_!=camera_->generation)return false;
    if(nativeWreckAttempted_)return nativeWreckPrepared_;
    nativeWreckAttempted_=true;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !backing.modelSourcesBound()||follow_->attempted_||follow_->ready_||follow_->closed_||follow_->synced_)
            return false;
        std::array<const g::OriginalWreckComponent50LA*,8> racers{};
        for(std::size_t i=0;i<session.actual_.bank.count();++i){
            const auto& racer=session.racerEntities_[i];
            if(!racer.prepared()||racer.racer.objectKey!=session.racerKey(i))return false;
            racers[i]=&racer.racer;
        }
        std::string error;
        if(!backing.bindActualRacers(racers,session.actual_.bank.count(),session.generation_,error))return false;
        const auto* rows=backing.reservations();
        if(!rows||rows->empty()||rows->size()!=inputs_.wreck.size())return false;
        return session.actual_.bank.visit(0,[&](){
        // Real current native owner field, corroborated by its named registry;
        // not a copied guest-global/category value or a synthetic field.
        const auto& category=follow_->main_.dynamicMask;
        if(category!=g::OriginalNativeCollisionCategories::physicsBody||
           follow_->mainFilter_.categoryA4!=category)return false;
        std::vector<g::OriginalWreckPieceQuerySlots50LA> slots;slots.reserve(rows->size());
        g::OriginalWreckQueryWorld50LA common{};
        for(std::size_t i=0;i<rows->size();++i){
            const auto& row=(*rows)[i];const auto& admitted=inputs_.wreck[i];
            if(row.queries!=admitted.queries||row.assets!=admitted.assets||row.identities!=admitted.identities)return false;
            g::OriginalWreckQueryWorld50LA world{};
            if(!follow_->bindWreckQueryWorldCold(i,*row.queries,*row.assets,row.identities,category,world))return false;
            // The same bridge supplies every common lane. Only its actual
            // reserved refs/identities vary per occurrence.
            if(world.world!=&storage_.get()->world||world.lifecycle!=&storage_.get()->lifecycle)return false;
            slots.push_back({world.refs,world.identities});
            if(!i){common=world;common.refs={};common.identities={};}
        }
        if(!backing.prepareOrderedQueries(common,slots,error))return false;
        nativeWreckPrepared_=true;return true;
        });
    }catch(...){return false;} // Exact owners and any successful prefix retained.
}
bool OwnershipBridge::retireNativeRetainedWreckCold(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent())return false;
    if(!nativeWreckAttempted_||nativeWreckRetired_)return true;
    if(!storage_.get()||!follow_||!rosterResources_||!rosterResources_->wreckBacking_||
       session.actual_.followQueries.get()!=follow_.get())return false;
    auto& backing=*rosterResources_->wreckBacking_;
    // Close the exact Follow while its retained wreck aliases still live.
    // Never bypass real source memberships; close rejects any attached tail.
    try {if(!session.actual_.bank.visit(0,[&](){return follow_->close();}))return false;}
    catch(...){return false;}
    if(!backing.retireNativeSourcePrefix(storage_.get()->world))return false;
    if(!backing.retireNativeColdQueries(&storage_.get()->world))return false;
    // Any actual query publications have already passed their concrete source
    // destructor. The untouched later pieces remain cold-only cancellation.
    // Revoke all Follow aliases before destroying even the first piece owner.
    for(std::size_t i=0;i<follow_->wreck_.size();++i)follow_->wreck_[i]={};
    if(!backing.releaseNativeColdBacking())return false;
    inputs_.wreck.clear();
    if(nativeWreckZoneOwner_){
        if(!nativeWreckZoneOwner_->releaseNativeWreckFirstZones(this))return false;
        nativeWreckZoneOwner_=nullptr;
    }
    nativeWreckRetired_=true;return true;
}
}

namespace rr::game {
bool OriginalRaceConstructor50LA::stageNativeFirstWreckLoad(
    const original_main_entry50KG::Application& app,original_camera_session50KX::Session& session,
    ra_source_camera50LA::OwnershipBridge& bridge,std::uint32_t& pc)noexcept {
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!ownsNativeCameraAdapter34(session)||
       !world74_||!world74_->source_||!world74_->source_->sourcesDecoded())return false;
    std::uint32_t tail=0;
    if(!continueNativeTrackAfterWater(app,tail)||tail!=0x1aa508||
       nativeWater70State_!=NativeWater70State::published||!nativeWater70Object_||
       nativeWater70Domain_.expired())return false;
    const auto& heights=world74_->source_->sourcesDecoded()->height.sourceDomain;
    //The same native Race70 copy, not merely an immutable no-water descriptor,
    //authorizes this private210804 alias before the2129C0 load sequence.
    if(!world74_->worldWater3FC_||*world74_->worldWater3FC_||!world74_->nativeWaterNone274570_)return false;
    const auto state=app.state;
    if(!state||!bridge.nativeFirstWreckApplicationCurrent(session,app))return false;
    // Synchronous borrow of this SAME live App state. No copied seed/facade,
    // no retained raw pointer, and failure never rolls back consumed draws.
    return bridge.stageNativeFirstWreckLoad(session,*this,*world74_,heights,app.materialDirectory36B970(),
        app.particleDirectory36B530(),*world74_->worldWater3FC_,state->rawRandom308030,pc);
}
}
namespace ra_source_camera50LA {
bool OwnershipBridge::nativeFirstWreckApplicationCurrent(
    const original_camera_session50KX::Session& session,
    const original_main_entry50KG::Application& app)const noexcept {
    const auto state=app.state;
    return state&&app.entities()==session.actual_.applicationEntities&&
        session.actual_.arena.borrowedNoise50KG==&state->sharedNoise308030;
}
//Synchronous adapter to the same native frame/control owners. This does not
//upgrade the cold Entity's base dispatch to Player/AI or claim their loaders.
bool OwnershipBridge::nativeRacerInputOwnerCurrent(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model)const noexcept {
    const auto pin=bankLifetime_.lock();
    if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||session.closed_||session.abandoned_||terminal_||closed_||
       cameraPrepareTerminal_||source348Arena_||!pin||!bankIdentity_||pin->identity!=bankIdentity_||
       bankIdentity_->raWorldLifetime_.get()!=pin.get()||!bankIdentity_->sealed_||bankIdentity_->retired_||
       bankIdentity_->visitDepth_!=1u||(!bankIdentity_->visiting_&&!bankIdentity_->readBlocked_)||
       bankIdentity_->sourceQueryPending()||model.slot_>=session.racerEntities_.size()||
       bankIdentity_->currentSlot_!=model.slot_||&session.actual_.bank!=bankIdentity_||
       !actual_||!gridArena_||!gridInputsRegistered(*gridArena_)||sharedOwnerLease_||sharedRoleViews_||
       nativeFifoPins_||triggerQueryActive_||sharedEffectsStopped_||!nativeWreckPrepared_||nativeWreckRetired_||
       !session.actual_.actualChariotModels||(*session.actual_.actualChariotModels)[model.slot_].get()!=&model||
       model.bank_!=bankIdentity_||!model.fixture_||
       &model.fixture_->parameters!=&gridArena_->parameters||model.arena_!=&actual_->frame||
       &session.actual_.arena!=&actual_->frame||!model.application_||
       model.application_!=session.actual_.applicationEntities||
       model.handlingRoute_!=native_racer_model50LA::Owner::HandlingRoute::legacy||
       model.generation_!=session.generation_)return false;
    const auto& racer=session.racerEntities_[model.slot_];
    return model.racer_==&racer&&model.attachedFor(racer)&&racer.prepared()&&!racer.nativeWreckWithdrawn_&&
        racer.racer.sessionGeneration==session.generation_&&racer.body340==model.body340_.get()&&
        racer.body340&&racer.body340->canonicalNumericAddress()==&actual_->frame.bodies.main&&
        registeredRange(&actual_->frame.fields.vehicle58,sizeof(actual_->frame.fields.vehicle58))&&
        registeredRange(&gridArena_->parameters.bodies.main,sizeof(gridArena_->parameters.bodies.main));
}
bool OwnershipBridge::nativeHandlingInputOwnerCurrent(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model)const noexcept {
    return model.nativePhysicalComplete_&&model.nativeRepulsorComplete_&&
        nativeRacerInputOwnerCurrent(session,model);
}
bool OwnershipBridge::readNativeHandlingTrack187330(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model,const g::OriginalRaceConstructor50LA& race,
    std::array<char,3>& out)const noexcept {
    //The ordinary ownsNativeCameraAdapter34 gate requires an idle Bank.
    //This getter instead runs inside the already-authenticated mutable visit.
    return nativeHandlingInputOwnerCurrent(session,model)&&race.cameraNativeAdapter34_==&session&&
        race.nativeCameraAdapter34LifetimeCurrent(session)&&race.world74_&&
        race.world74_->runtimeQueries_.get()==this&&!race.world74_->terminal_&&!race.world74_->closed_&&
        camera_->display&&race.readSourceTrackName187330(out);
}
bool OwnershipBridge::readNativeHandlingConfiguration215434(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model,std::uint32_t& out)const noexcept {
    if(!nativeHandlingInputOwnerCurrent(session,model)||model.slot_!=0u||
       session.actual_.humanCount34!=1u||actual_->frame.fields.vehicle58!=0u||
       actual_->frame.fields.nativeAiOwnersBound50IV||!session.actual_.nativeHumanPlayer0)return false;
    //Native PlayerEventState is the SAME owner used by48Z/49A dispatch, not
    //a copied cold selection. It is explicitly not a2107E8 source receipt.
    const auto index=session.actual_.nativeHumanPlayer0->source5C;
    if(index>=g::controlPresetSlots49D)return false;
    out=session.actual_.preferences.slots[index].configuration11040;return true;
}
bool OwnershipBridge::stageNativeFirstWreckLoad(original_camera_session50KX::Session& session,
    const g::OriginalRaceConstructor50LA& race,ra_pod_scene50LA::Owner& world,const g::UndertrackPhysicsDomain50EW& heights,const std::string& directory,
    const std::string& actualParticleDirectory36B530,const void* actualWorld3FC,std::uint32_t& actualRawRandom,std::uint32_t& pc)noexcept {
    try {
        if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
           session.nativeWreckOwner_!=this||!cameraBankCurrent()||closed_||terminal_||cameraPrepareTerminal_||
           !nativeWreckPrepared_||nativeWreckRetired_||!session.identitiesCurrent()||
           !session.nativeRacerCamera1750A0_.prefixComplete||session.nativeRacerCamera1750A0_.revoked||
           session.nativeRacerCamera1750A0_.pc!=0x1750f8||session.actual_.actualHeightDomain!=&heights||
           session.actual_.actualMaterialDirectory36B970!=&directory||!rosterResources_||
           !rosterResources_->wreckBacking_||!follow_||follow_->attempted_||follow_->closed_)return false;
        return session.actual_.bank.visit(0,[&](){
            auto& backing=*rosterResources_->wreckBacking_;
            if(!session.actual_.actualChariotModels)return false;
            auto* nativeModel=(*session.actual_.actualChariotModels)[0].get();
            auto* phy=nativeModel?nativePhyOwnerCurrent(session,*nativeModel):nullptr;
            auto& actualRacer=session.racerEntities_[0];
            if(!phy||actualRacer.nativePhyLoad_)return false;
            if(session.actual_.humanCount34&&!bindNativePlayerLoad1A0848(session,*nativeModel))return false;
            //Forward bind to the genuine pre-existing cold occurrence. Earlier
            //Wreck inventory reads remain explicitly native-adapted; no fake
            //210A48 open or2129AC selection history is manufactured here.
            actualRacer.nativePhyLoad_=phy;
            if(!backing.loadNativeFirstRacerSource(actualRacer,heights,directory,actualWorld3FC,pc))return false;
            g::OriginalWreckSetupZones50LA zones;g::CollectionRef50BX visibility=0,collision=0;
            if(nativeWreckZoneOwner_||!world.retainNativeWreckFirstZones(this,
                session.racerEntities_[0].racer.sessionGeneration,zones,visibility,collision))return false;
            nativeWreckZoneOwner_=&world;
            // Any setup rejection retains this exact borrow and constructor
            // prefix until real source-aware retirement; never retry/replay.
            if(!backing.setupNativeBody(0,zones,pc))return false;
            if(!backing.loadNativeQueryChildren(0,pc))return false;
            //Actual retained Application directory, not an authored filename
            //guess or a copied source value. Preserve earlier query effects if
            //this native resource-lookup admission cannot be established.
            if(actualParticleDirectory36B530!="particle\\")return false;
            if(!world.retainNativeWreckParticles(this,session.actual_.actualTrackParticles4A0))return false;
            //Existing immutable HOST category namespace, not a fabricated guest
            //4342A8 word or an independent literal category table.
            const auto& water=g::OriginalNativeCollisionCategories::bindings[5];
            if(std::strcmp(water.name,"WATER")||water.bit!=g::OriginalNativeCollisionCategories::water)return false;
            if(!backing.loadNativeEffects(0,*nativeWreckParticles_,water.bit,pc))return false;
            if(!backing.loadNativeRemainingCollectionPieces(zones,heights,directory,*nativeWreckParticles_,water.bit,pc))return false;
            if(!backing.loadNativeCollectionGroup(pc))return false;
            if(!backing.deactivateNativeCollectionLoad(pc)||!backing.appendNativeCollectionToRacer(pc))return false;
            //Actual same selected Bank690/694 owners, not Wreck replacements.
            //Native immutable projection refresh is admitted before each source
            //list mutation; no active collector/role view may retain old words.
            if(sharedOwnerLease_||sharedRoleViews_||nativeFifoPins_||triggerQueryActive_||sharedEffectsStopped_||
               !actual_||!actual_->recovery||!storage_.get()||!storage_.get()->sharedRoster_)return false;
            std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> rows{};
            if(!sharedColdRowsCurrent(0,rows)||rows[0]->canonicalFilter!=&actual_->mainFilter||
               rows[3]->canonicalFilter!=&actual_->recovery->podFilter||
               rows[3]->bodyA0!=session.racerEntities_[0].body340||!rows[3]->bodyA0||
               rows[0]->collision!=rows[3]->collision||!bodyIdentities_[0].collision||
               rows[0]->collision!=bodyIdentities_[0].collision||
               rows[0]->type2Resource!=&bodyIdentities_[0].collision->main()||
               actual_->main.resource!=rows[0]->type2Resource||
               !rows[0]->identity||!rows[3]->identity||rows[0]->identity==rows[3]->identity)return false;
            const auto actual694=rows[3]->identity;
            auto appendMain690=[&](std::uint32_t actualE4)->bool {
                auto& row=*rows[0];auto& canonical=actual_->mainFilter;auto& s=*storage_.get();
                if(!actualE4||!row.coldFilter||!row.coldFilter->matches(canonical)||
                   row.canonicalFilter!=&canonical||row.identity!=canonical.identity||!row.reservedRef||
                   row.reservedRef>s.count.objects||row.effectRevision==std::numeric_limits<std::uint64_t>::max())return false;
                auto& object=s.objects[row.reservedRef-1u];const bool published=object.identity!=0;
                if(published){const auto& prior=row.coldFilter->view;
                    if(object.identity!=row.identity||object.filter.categoryA4!=prior.categoryA4||
                       object.filter.acceptMaskB8!=prior.acceptMaskB8||object.filter.flags31!=prior.flags31)return false;
                    for(unsigned i=0;i<2u;++i){const auto& a=object.filter.identities[i];const auto& b=prior.identities[i];
                        if(a.values!=b.values||a.capacity!=b.capacity||a.count!=b.count||a.mode!=b.mode)return false;}
                    const auto& a=object.filter.categories;const auto& b=prior.categories;
                    if(a.values!=b.values||a.capacity!=b.capacity||a.count!=b.count||a.mode!=b.mode)return false;
                }
                const auto& words=canonical.identityLists[0].identities;
                if(std::find(words.begin(),words.end(),actualE4)!=words.end())return true; //123EE4 duplicate branch
                //Prepare native projection and realloc lowering without changing
                //the one actual Bank filter. A rejection retains prior E0 work.
                auto nextCanonical=canonical;auto& nextWords=nextCanonical.identityLists[0].identities;
                const auto hole=std::find(nextWords.begin(),nextWords.end(),0u);
                const bool fillHole=hole!=nextWords.end();const auto holeIndex=static_cast<std::size_t>(hole-nextWords.begin());
                if(fillHole)*hole=actualE4;else {
                    if(nextWords.size()>=static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()))return false;
                    nextWords.push_back(actualE4);
                }
                auto nextProjection=std::make_unique<const storage::Storage::SharedColdFilter>(nextCanonical);
                if(fillHole)canonical.identityLists[0].identities[holeIndex]=actualE4; //123F30, original C0 pointer unchanged
                else canonical.identityLists[0].identities.swap(nextWords); //123F5C..7C native realloc lowering
                row.coldFilter=std::move(nextProjection);
                if(published)object.filter=row.coldFilter->view; //same actual published collector owner only
                ++row.effectRevision;return true;
            };
            for(;;){
                if(!backing.bindNativeRacerExclusions(actual694,appendMain690,pc))return false;
                if(pc==0x212b50u){
                    if(!backing.loadNativeRacer814810(pc))return false;
                    if(!backing.loadNativeRacer818(actualParticleDirectory36B530,actualRawRandom,pc))return false;
                    if(!rosterResources_->rigs_[0])return false;
                    //Same real forward parser owner and retained PHY. End
                    //this reader lease before any nested handling borrower.
                    auto* model=nativeModel;
                    if(!model||model->actualRig()!=rosterResources_->rigs_[0].get()||
                       nativePhyOwnerCurrent(session,*model)!=phy||actualRacer.nativePhyLoad_!=phy)return false;
                    {
                        auto reader=phy->borrow(model->nativePhyContext_,rosterResources_->rigs_[0]->phy);
                        if(!reader||!backing.beginNativeHandling2146(reader,pc))return false;
                    }
                    const bool physical=model->continueNativePhysical214710(session.racerEntities_[0],
                        rosterResources_->rigs_[0]->phy,pc);
                    backing.sourcePC_=pc; //Same reached continuation, including a retained failed prefix.
                    if(!physical)return false;
                    const bool repulsor=model->continueNativeRepulsor214894(session.racerEntities_[0],
                        rosterResources_->rigs_[0]->phy,*this,pc);
                    backing.sourcePC_=pc;
                    if(!repulsor)return false;
                    const bool handling=model->continueNativeAeroEngine214954(session.racerEntities_[0],
                        rosterResources_->rigs_[0]->phy,session,race,*this,pc);
                    backing.sourcePC_=pc;
                    if(!handling)return false;
                    const bool animation=model->continueNativeAnimationContact2154D8(session.racerEntities_[0],
                        rosterResources_->rigs_[0]->phy,session,*this,pc);
                    backing.sourcePC_=pc;
                    if(!animation||!model->nativeHandlingStack_||!nativeHandlingInputOwnerCurrent(session,*model))return false;
                    //Same collection cohort, Config selection and2146 stack.
                    //Each22C310 finishes before its own22C610; no reordered
                    //whole-roster pass or reconstructed temporary bodies.
                    bool wreck=false;
                    {
                        auto reader=phy->borrow(model->nativePhyContext_,rosterResources_->rigs_[0]->phy);
                        if(!reader||nativePhyOwnerCurrent(session,*model)!=reader.owner())return false;
                        wreck=backing.continueNativeWreck215AF8(session.racerEntities_[0],reader,
                            model->nativeHandlingStack_->gravity47C,pc);
                    }
                    backing.sourcePC_=model->failurePC_=session.racerEntities_[0].nativePostEmberPC_=pc;
                    if(!wreck||pc!=0x215bb8u)return false;
                    model->nativeWreckTuningComplete_=true;
                    const bool returned=model->continueNativeHandling215BB8(session.racerEntities_[0],
                        rosterResources_->rigs_[0]->phy,session,race,*this,pc);
                    backing.sourcePC_=pc;
                    if(!returned||pc!=0x216034u)return false;
                    //The current native Cables adapter is the retained owner
                    //of the later source allocations, not a replacement254.
                    //Stay in this exact mutable Bank visit: never enter the
                    //inactive wrapper that reparses PHY or nests a visit.
                    if(!nativeHandlingInputOwnerCurrent(session,*model)||
                       session.actual_.actualCables!=&camera_->cables||
                       !camera_->cablePins[0]||!camera_->cables[0]||
                       session.actual_.racer60[0]!=session.identity_[0].pod60){
                        pc=backing.sourcePC_=model->failurePC_=
                            session.racerEntities_[0].nativePostEmberPC_=0x212df0u;
                        return false;
                    }
                    const bool outer=camera_->cables[0]->continueNativeOuter212DF0(
                        session.racerEntities_[0],*model,session.actual_.applicationEntities,
                        session.actual_.racer60[0],pc);
                    //Retain every reached native/source effect, including a
                    //failed partial material/strip/cable owner. No retry of
                    //the one-shot handling or loader prefix is authorized.
                    backing.sourcePC_=model->failurePC_=
                        session.racerEntities_[0].nativePostEmberPC_=pc;
                    if(!outer||pc!=0x212e10u||
                       !camera_->cables[0]->nativeOuterReturned_||
                       camera_->cables[0]->nativeOuterResult_.sourcePC!=pc||
                       !nativeHandlingInputOwnerCurrent(session,*model))return false;
                    const bool spin=model->continueNativeSpin212ED0(session.racerEntities_[0],pc);
                    backing.sourcePC_=model->failurePC_=session.racerEntities_[0].nativePostEmberPC_=pc;
                    if(!spin||pc!=0x212e18u)return false;
                    const bool tail=continueNativeOuterStorage212E18(session,*model,pc);
                    backing.sourcePC_=model->failurePC_=session.racerEntities_[0].nativePostEmberPC_=pc;
                    // Only real native close permits the typed outer return.
                    // The forward human caller now consumes the SAME current
                    // Aero steering. Keep outer PCs separate from caller PCs.
                    if(!tail)return false;
                    if(!session.actual_.humanCount34)return true;
                    if(!continueNativePlayerReturn1A0850(session,*model,pc))return false;
                    if(!continueNativePlayerResources1AE104(session,*model,race,pc))return false;
                    return continueNativeBaseRacerSpeech22A1D0(session,*model,race,pc);
                }
                if(pc!=0x2129e0u||!backing.loadNativeNextCollectionPrefix(heights,directory,pc)||
                   !backing.setupNativeBody(0,zones,pc)||!backing.loadNativeQueryChildren(0,pc)||
                   !backing.loadNativeEffects(0,*nativeWreckParticles_,water.bit,pc)||
                   !backing.loadNativeRemainingCollectionPieces(zones,heights,directory,*nativeWreckParticles_,water.bit,pc)||
                   !backing.loadNativeCollectionGroup(pc)||!backing.deactivateNativeCollectionLoad(pc)||
                   !backing.appendNativeCollectionToRacer(pc))return false;
            }
        });
    }catch(...){return false;}
}
}

namespace ra_pod_scene50LA {
bool Owner::retainNativeWreckFirstZones(ra_source_camera50LA::OwnershipBridge* borrower,
    std::uint64_t generation,g::OriginalWreckSetupZones50LA& out,
    g::CollectionRef50BX& visibility,g::CollectionRef50BX& collision)noexcept {
    if(!borrower||nativeWreckZoneBorrower_||runtimeQueries_.get()!=borrower||
       !topologyConstructed_||topologyClosed_||!identitiesCurrent()||
       generation!=sceneGeneration_||!sceneApplication_||sceneApplication_->attachmentLock423F58()||
       !sourceVisibilityPublished_||!sourceCollisionPublished_)return false;
    const auto* v=(*sourceVisibility4C0_)[0];const auto* c=(*sourceCollision4C4_)[0];
    if(v!=&visibility4C0_[0]||c!=&collision4C4_[0]||!v->attached||!c->attached||
       v->entity.reference00!=1u||c->entity.reference00!=1u||
       v->entity.kind!=g::OriginalWreckComponentKind50LA::searchNode27F120||
       c->entity.kind!=g::OriginalWreckComponentKind50LA::searchNode27F120||
       v->entity.objectKey!=key(&v->entity)||c->entity.objectKey!=key(&c->entity)||
       v->entity.sessionGeneration!=generation||c->entity.sessionGeneration!=generation||
       v->entity.parent1C!=&container4BC_||c->entity.parent1C!=&container4BC_||
       v->collectionBacking!=visibilityRows_||c->collectionBacking!=collisionRows_||
       visibilityRows_==collisionRows_||v->ordinal48!=0u||c->ordinal48!=0u)return false;
    auto* vis=lighting_->nativeWreckVisibility165E90(this,visibilityRows_);
    auto* collisionOwner=borrower->storage_.get();
    if(!vis||!collisionOwner||&vis->lifecycle==&collisionOwner->lifecycle||
       &vis->portals==&collisionOwner->world||!vis->lifecycle.zoneCount||!vis->portals.zoneCount||
       !collisionOwner->lifecycle.zoneCount||!collisionOwner->world.zoneCount)return false;
    //1AE0C8/1AE488 load actual4C0[0]/4C4[0], forwarded unchanged through
    //1A0570/16E450 ->210770 ->22BFD0 ->165E90. Native registry refs encode
    //SearchNode.ordinal48+1; these temporaries are NOT guest pointer fields.
    visibility=v->ordinal48+1u;collision=c->ordinal48+1u;
    out.visibility4C0=vis;out.visibilityZoneField=&visibility;
    out.collision4C4=&collisionOwner->world;out.collisionLifecycle=&collisionOwner->lifecycle;
    out.collisionZoneField=&collision;out.nativeVisibilityNode=v;out.nativeCollisionNode=c;
    nativeWreckVisibilityNode_=v;nativeWreckCollisionNode_=c;nativeWreckZoneBorrower_=borrower;
    return true;
}
bool Owner::releaseNativeWreckFirstZones(ra_source_camera50LA::OwnershipBridge* borrower)noexcept {
    if(!borrower||nativeWreckZoneBorrower_!=borrower||runtimeQueries_.get()!=borrower||
       !sourceTopologyCurrent(true)||nativeWreckVisibilityNode_!=(*sourceVisibility4C0_)[0]||
       nativeWreckCollisionNode_!=(*sourceCollision4C4_)[0])return false;
    //Caller has retired real external effects, piece/query borrowers and the
    //native facade backing first. Direct Track close was blocked meanwhile.
    if(borrower->nativeWreckParticles_){
        if(borrower->nativeWreckParticles_!=nativeParticleManager4A0_||!trackParticles_||
           trackParticles_.get()!=nativeParticleManager4A0_||
           trackParticles_->nativeWreckBorrower_!=borrower)return false;
        trackParticles_->nativeWreckBorrower_=nullptr;borrower->nativeWreckParticles_=nullptr;
    }
    nativeWreckZoneBorrower_=nullptr;nativeWreckVisibilityNode_=nullptr;nativeWreckCollisionNode_=nullptr;
    return true;
}
}

namespace ra_pod_scene50LA {
bool Owner::retainNativeWreckParticles(ra_source_camera50LA::OwnershipBridge* borrower,
    g::OriginalTrackParticleCold50LA* actual)noexcept {
    if(!borrower||nativeWreckZoneBorrower_!=borrower||runtimeQueries_.get()!=borrower||
       borrower->nativeWreckZoneOwner_!=this||borrower->nativeWreckParticles_||
       !trackParticles_||actual!=trackParticles_.get()||actual!=nativeParticleManager4A0_||
       actual->nativeWreckBorrower_||!sceneApplication_||!lighting_||
       !actual->matchesNativeResources160E00(sceneApplication_.get(),actual->table_.get(),
           sceneGeneration_,actual->particles()))return false;
    //1AA57C reads genuine World4A0. Same counted owner/numeric pool as the
    //earlier racer210890 producer, not a new effect manager or NULL projection.
    actual->nativeWreckBorrower_=borrower;borrower->nativeWreckParticles_=actual;return true;
}
}


// Distinct source-loading segment. The existing native observation route above
// is unchanged and never supplies a completed cache/reset/Config receipt here.
namespace ra_cold_aggregate50LA {
const std::string* Owner::sourceParticleDirectory36B530(const g::OriginalApplicationEntities50LA* expected)const noexcept {
    try {
    //Durable SAME invocation identity, not the reset-only270550 entry predicate.
    //The SCB Config may genuinely be closed and World34 replaced by TOB.
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||nativeScbRevoked_||
       !worldColdConstructed_||worldEntity_.reference00!=1u||!sourceLoadRace_||!sourceLoadApplication_||
       !sourceLoadRace_->nativeIntroRaceEntity()||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       !expected||worldApplication_.get()!=expected||sourceLoadApplication_->entities()!=worldApplication_||
       sourceLoadReset270550Phase_!=SourceLoadReset270550Phase::returned||
       sourceTrackStep_==SourceTrackStep::unreached||sourceTrackStep_==SourceTrackStep::terminal||
       sourceTrackStep_==SourceTrackStep::nativeClosed||!sourceConfig34Published_||!sourceWorldConfig_||
       !sourceTrackFile_||sourceTrackFile_->thread_!=std::this_thread::get_id()||sourceTrackFile_->nativeClosed_||!sourceTrackFile_->joined_||
       !sourceTrackMountComplete_||sourceTrackMountClosed_||sourceTrackMountApplication_!=sourceLoadApplication_||
       !source_||!source_->sourceTrackInput_||sourceTrackMountSource_!=source_||
       sourceTrackFile_->archive_!=source_->sourceTrackInput_||!sourceTrackMountDomain_||!sourceTrackMountDomain_->prepared()||
       sourceTrackMountLease_.closed_||sourceTrackMountLease_.thread_!=std::this_thread::get_id()||
       !sourceTrackMountLease_.count_||!sourceTrackMountLease_.provider_||!sourceTrackMountLease_.provider_->sealed()||
       sourceTrackMountLease_.provider_!=sourceTrackMountDomain_->archiveProvider3E1A70()||
       sourceTrackFile_->provider_!=sourceTrackMountLease_.provider_||
       sourceTrackFile_->resolver_!=sourceTrackMountDomain_->resolver3E1A70())return nullptr;
    const auto& config=*sourceWorldConfig_;
    if(config.thread_!=std::this_thread::get_id())return nullptr;
    if(sourceSceneConfigNativeWithdrawn_){
        if(!sourceSceneConfigCloseReturned_||!config.sourceClosed_||!config.nativeClosed_||
           config.source_||config.file_||config.attemptRetained_)return nullptr;
    }else{
        if(sourceSceneConfigCloseAttempted_||config.sourceClosed_||config.nativeClosed_||
           config.file_!=sourceTrackFile_||!config.source_||!worldParser34_||
           *worldParser34_!=config.source_.get()||
           !sourceTrackFile_->current(config,config.source_->file00))return nullptr;
    }
    return &sourceLoadApplication_->particleDirectory36B530();
    }catch(...){return nullptr;} //Native accessor refusal preserves the earlier smoke prefix.
}
bool Owner::sourceWorldLoadInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&sourceLoadRace_&&sourceLoadApplication_&&
        !terminal_&&!closed_&&!nativeScbRevoked_&&worldColdConstructed_&&
        sourceLoadRace_->nativeIntroRaceEntity()&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        worldApplication_==sourceLoadApplication_->entities()&&
        sourceTrackStep_==SourceTrackStep::reset;
}
bool Owner::abortSourceTrackNative(std::uint32_t& pc)noexcept {
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||constructorRacerWorldBorrows_||nativeWreckZoneBorrower_||sourceParticleInvocationBorrowers_||sourceFogTransferBorrowers_)return false;
    //Actual emitter70 borrows World4A0: withdraw all source model/smoker
    //borrowers before native particle teardown. No source call order changes.
    if(!abortSourcePostParticleModelsNative(pc))return false;
    if(!abortSourceParticleConfigNative(pc)||!abortSourceParticleStageNative(pc)||!abortSourceFogTransferNative(pc))return false;
    if(sourceTrackStep_==SourceTrackStep::nativeClosed)return true;
    if(!preflightSourceWorldTopologyNative()||!preflightSourceWorldMaterialAssignmentsNative()||!preflightSourceWorldAudioNative()||!preflightSourceWorldStaticModelParamsNative()||!preflightSourceWorldBuoysNative()||!preflightSourceWorldParticleCountNative()||!preflightSourceWorldSunFlareNative()||!preflightSourceWorldCsCamerasNative()||!preflightSourceWorldBoundaryNative()||
       !abortSourceWorldTopologyNative(pc)||!abortSourceWorldMaterialAssignmentsNative(pc)||!abortSourceWorldMaterialSoundsNative(pc)||!abortSourceWorldSoulTreeSoundsNative(pc)||!abortSourceWorldAmbientNative(pc)||!abortSourceWorldStaticModelParamsNative(pc)||!abortSourceWorldBuoysNative(pc)||!abortSourceWorldParticleDescriptorsNative(pc)||!abortSourceWorldSunFlareNative(pc)||!abortSourceWorldCsCamerasNative(pc)||!abortSourceWorldTriggerNative(pc)||
       !abortSourceWorldToyNative(pc)||
       !abortSourceWorldTerrainNative(pc)||
       !abortSourceWorldControllersNative(pc)||
       !abortSourceWorldCollisionNative(pc)||
       !abortSourceWorldStaticModelsNative(pc)||
       !abortSourceWorldInstanceMasterNative(pc)||
       !abortSourceWorldPhysicsNative(pc)||
       !abortSourceWorldPortalsNative(pc)||
       !abortSourceWorldZonesNative(pc)||
       !abortSourceWorldFogNative(pc)||
       !abortSourceWorldLightsNative(pc)||
       !abortSourceEnvironmentNative(pc))return false;
    if(!abortSourceWorldBoundaryNative(pc))return false;
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!abortSourceWorldAudioNative(pc))return false;
    //The later TOB occurrence withdraws before the old SCB tombstone and
    //shared Track root. No old-wrapper cleanup can close a foreign occurrence.
    if(sourceWorldObjectConfig_){
        if(!sourceWorldObjectConfig_->abortNative(d)){pc=d.sourcePC;return false;}
        if(sourceObjectConfig34Published_)worldParser34_.reset(); //Native unknown, not guestNULL.
        sourceWorldObjectConfig_.reset();sourceObjectPhase_=SourceObjectPhase::nativeClosed;
    }
    if(sourceWorldConfig_&&!sourceWorldConfig_->abortNative(d)){pc=d.sourcePC;return false;}
    // Config occurrence withdrawal precedes bank/device retirement. The actual
    // World34 source history is not assigned a synthetic source NULL by abort.
    if(sourceConfig34Published_)worldParser34_.reset(); // knowledge withdrawn, NOT source NULL
    sourceWorldConfig_.reset();
    if(sourceTrackFile_&&!sourceTrackFile_->closeNative(d)){pc=d.sourcePC;return false;}
    sourceTrackFile_.reset();
    a::OriginalStandardMaterialDiagnostic50LA nativeMount;
    if(!closeSourceTrackNativeProvider(nativeMount))return false;
    sourceLoadRace_=nullptr;sourceLoadApplication_=nullptr;
    sourceTrackStep_=SourceTrackStep::nativeClosed;return true;
}
bool Owner::loadSourceWorld1B3880(std::uint32_t& pc)noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    if(nativeScbThread_!=std::this_thread::get_id()||sourceTrackStep_!=SourceTrackStep::filename||
       sourceLoadReset270550Phase_!=SourceLoadReset270550Phase::returned||
       !sourceWorldConfig_||!sourceTrackFile_||nativeScb34_||nativeTob34_||
       !worldParser34_||*worldParser34_!=nullptr)return false;
    const auto stop=[&](std::uint32_t at)noexcept {pc=sourceTrackPC_=at;sourceTrackStep_=SourceTrackStep::terminal;return false;};
    // Derived1B3880 exact admitted tracks\ra path:13DA50 produces empty
    // drive, tracks\ path, ra stem, empty extension. Only actual sprintf bytes
    // (including NUL) are written; untouched tails retain prior source history.
    static constexpr char directory[]="tracks\\";
    static constexpr char name[]="ra";
    for(std::size_t i=0;i<sizeof(directory);++i)worldText3C_[i]=std::uint8_t(directory[i]);
    sourceTrackPC_=0x1b38dc;
    for(std::size_t i=0;i<sizeof(name);++i)worldText140_[i]=std::uint8_t(name[i]);
    sourceTrackPC_=0x1b38f4;
    if(!sourceWorldConfig_->allocate1B3984())return stop(0x1b3984);
    worldParser34_=sourceWorldConfig_->source_.get(); // unconditional1B399C includingNULL
    sourceConfig34Published_=true;
    sourceTrackStep_=SourceTrackStep::config;
    a::OriginalStandardTextureDiagnostic50LA d;
    auto returned=sourceWorldConfig_->open1577D0("tracks\\ra.scb",d);
    if(returned==Config::Return::bounded)return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    if(returned==Config::Return::zero){
        returned=sourceWorldConfig_->open1577D0("tracks\\ra.scn",d);
        if(returned==Config::Return::bounded)return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
        if(returned==Config::Return::one)return stop(0x1b3a4c); // actual optional binary-export global unjoined
        if(returned==Config::Return::zero){
            if(!sourceWorldConfig_->close1573B0(d))return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
            worldParser34_=nullptr; // actual1B39DC, unlike native abort
            return stop(0x1b39e0); // before unjoined source assertion/logging
        }
    }
    sourceTrackStep_=SourceTrackStep::opened;sourceTrackPC_=pc=0x1b3aac;
    return continueSourceWorldLog1B3AAC(pc); // explicit native diagnostic adaptation, below
}
}

namespace ra_cold_aggregate50LA {
bool Owner::loadSourceTrackSegment1A6D48(const rr::game::OriginalRaceConstructor50LA& race,
    const original_main_entry50KG::Application& app,const std::string& modelDirectory,
    std::string& trackDirectory,std::uint32_t& pc)noexcept {
    pc=0; // Native admission precedes this private source segment.
    if(nativeScbThread_!=std::this_thread::get_id()||sourceTrackNativeAttempted_||
       sourceTrackStep_!=SourceTrackStep::unreached||sourceLoadRace_||sourceLoadApplication_||
       terminal_||closed_||nativeScbRevoked_||!worldColdConstructed_||
       race.nativeIntroWorldEntity()!=&worldEntity_||worldApplication_!=app.entities()||
       !source_||!source_->sourcesDecoded()||!source_->sourceTrackInput_||
       worldAttempted_||worldImagesAttempted_||nativeScb34_||nativeTob34_||
       trackDirectory!="tracks\\")return false;
    std::array<char,3> name{};
    if(!race.readSourceTrackName187330(name)||name!=std::array<char,3>{{'r','a',0}})return false;
    sourceTrackNativeAttempted_=true;
    std::string nextTrackDirectory;
    try {
        nextTrackDirectory="tracks\\";
        sourceTrackFile_.reset(new a::OriginalRaTrackSourceFile50LA);
        sourceWorldConfig_.reset(new rr::platform::OriginalWorldSourceConfig50LA);
    }catch(...){return false;} // retained wrapper custody, no source allocator claim
    if(!sourceWorldConfig_->bindNative(sourceTrackFile_))return false;
    a::OriginalStandardMaterialDiagnostic50LA mount;
    if(!prepareSourceTrackNativeProvider(app,mount))return false;
    const auto domain=sourceTrackMountDomain_;
    if(!domain||!domain->resolver3E1A70()||!domain->archiveProvider3E1A70())return false;
    sourceLoadRace_=&race;sourceLoadApplication_=&app;
    const auto stop=[&](std::uint32_t at)noexcept {pc=sourceTrackPC_=at;sourceTrackStep_=SourceTrackStep::terminal;return false;};
    a::OriginalStandardTextureDiagnostic50LA d;
    sourceTrackStep_=SourceTrackStep::mount;sourceTrackPC_=pc=0x1a6d48;
    if(!sourceTrackFile_->mount12CEA0(domain->resolver3E1A70(),domain->archiveProvider3E1A70(),
        source_->sourceTrackInput_,d))return stop(d.sourcePC?d.sourcePC:0x1a6d50);
    sourceTrackStep_=SourceTrackStep::archiveOnly;sourceTrackPC_=pc=0x1a6d64;
    if(!sourceTrackFile_->enterArchiveOnly132A20(d))return stop(d.sourcePC?d.sourcePC:0x1a6d64);
    sourceTrackStep_=SourceTrackStep::cache;sourceTrackPC_=pc=0x1a6d78;
    if(!sourceTrackFile_->resizeCache132E40(d))return stop(d.sourcePC?d.sourcePC:0x1a6d78);
    // Actual1A9EE0 saved-directory prefix. No source getter/native decoded view
    // is substituted for these bytes; truncate exactly to259 plus terminator.
    sourceTrackStep_=SourceTrackStep::savedDirectory;
    const auto nul=modelDirectory.find('\0');
    const auto length=std::min<std::size_t>(259,nul==std::string::npos?modelDirectory.size():nul);
    std::memcpy(sourceSavedModelDirectory_.data(),modelDirectory.data(),length);
    sourceSavedModelDirectory_[length]=0;sourceSavedDirectoryBytes_=length+1;
    // Actual normal-profile2E0 branch was admitted by Application, not supplied
    // by a generic native mode. The literal directory allocation is preadmitted.
    trackDirectory.swap(nextTrackDirectory);sourceTrackPC_=0x1a9fd0;
    if(!race.readSourceTrackName187330(name)||name!=std::array<char,3>{{'r','a',0}})return stop(0x1a9fe0);
    sourceTrackStep_=SourceTrackStep::reset;
    sourceLoadReset270550Phase_=SourceLoadReset270550Phase::invoked1AA040;
    sourceLoadReset270550PC_=sourceTrackPC_=pc=0x1aa040;
    if(!resetColdLoadWorld270550(pc))return stop(pc);
    sourceTrackPC_=pc=0x1aa050;
    if(!race.readSourceTrackName187330(name)||name!=std::array<char,3>{{'r','a',0}})return stop(0x1aa050);
    sourceTrackStep_=SourceTrackStep::filename;
    return loadSourceWorld1B3880(pc);
}
}
namespace rr::game {
bool OriginalRaceConstructor50LA::loadSourceTrackSegment1A6D48(
    const original_main_entry50KG::Application& app,const std::string& modelDirectory,
    std::string& trackDirectory,std::uint32_t& pc)noexcept {
    pc=0;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!world74_||!counted_||rolledBack_||
       application_!=app.entities()||profile_!=app.riderProfile||!nativeIntroRaceEntity()||
       nativeTrackLoadStep_!=NativeTrackLoadStep::unreached||cameraNativeAdapter34_||
       !loadingCamera34_.suffixReturned||loadingCamera34_.terminal||
       loadingCamera34_.stopPC!=0x1a9598||loadingCamera34_.arrayWithdrawn||
       !loadingCameraSourceNullSuffixCurrent())return false;
    return world74_->loadSourceTrackSegment1A6D48(*this,app,modelDirectory,trackDirectory,pc);
}
}
namespace original_main_entry50KG {
bool Application::loadRetainedSourceTrackSegment1A6CF4(std::uint32_t& pc)const noexcept {
    pc=0; // This private segment has no upstream caller: music1A3C00 is unjoined.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!sourceRace28COwner_||
       raceConstructorColdOwner50LA_.lock()!=sourceRace28COwner_||!riderProfile||
       !sourceUseProfile2E0_||*sourceUseProfile2E0_!=1u||!sourceLoading330Owner_)return false;
    const auto& loader=*sourceLoading330Owner_;
    // Exact constructor/initializer-produced null-mode branch of1A6CD4. No
    // progress display or nonzero30 branch is accepted from native observations.
    if(!loader.current()||loader.issuer_!=this||!loader.managerInitialized_||
       !loader.slot330Published_||loader.retiring_||loader.retired_||loader.mode30_!=0u)return false;
    try {
        return sourceRace28COwner_->loadSourceTrackSegment1A6D48(*this,
            modelDirectory36B860_,trackDirectory36B640_,pc);
    }catch(...){return false;}
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueSourceWorldLog1B3AAC(std::uint32_t& pc)noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||
       sourceTrackStep_!=SourceTrackStep::opened||sourceTrackPC_!=0x1b3aac||
       sourceWorldLogAttempted_||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!sourceConfig34Published_||
       !sourceWorldConfig_||!sourceWorldConfig_->source_||!sourceTrackFile_||
       sourceWorldConfig_->file_!=sourceTrackFile_||
       sourceWorldConfig_->returned_!=Config::Return::one||
       !worldParser34_||*worldParser34_!=sourceWorldConfig_->source_.get()||
       !sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00))return false;
    // Original literal3ABB00 has exactly two %s conversions, in this order:
    // World244 then World140. Read ONLY known bytes through the actual NUL;
    // unused source-unwritten suffix bytes are never scanned or zero-filled.
    std::array<char,1024> output;std::size_t size=0;
    const auto literal=[&](const char* text)noexcept {
        while(*text){if(size==output.size())return false;output[size++]=*text++;}return true;};
    const auto sourceString=[&](const auto& text)noexcept {
        for(const auto& byte:text){if(!byte)return false;if(*byte==0)return true;
            if(size==output.size())return false;output[size++]=static_cast<char>(*byte);}
        return false;
    };
    if(!literal("\n\n***** Begin Load for Scene ")||!sourceString(worldText244_)||
       !literal(" (")||!sourceString(worldText140_)||!literal(".scn) *****\n\n"))return false;
    // Native I/O adapter, NOT an emulation of EE reent38544C->stdout+8 or
    // vfprintf30B4F0. It performs one real host stdout write without forcing a
    // flush, keeps partial bytes/error, and never fabricates successful output.
    //1336C0's integer return is propagated by1B3880 but unused at1AA078, so a
    // native short write/error does NOT invent a gameplay failure branch.
    sourceWorldLogAttempted_=true;sourceWorldLogRequested_=size;
    sourceWorldLogWritten_=std::fwrite(output.data(),1,size,stdout);
    sourceWorldLogNativeError_=sourceWorldLogWritten_!=size||std::ferror(stdout)!=0;
    sourceTrackStep_=SourceTrackStep::filenameReturned;sourceTrackPC_=pc=0x1aa078;
    return continueSourceSceneInfo270900(pc); // Actual sameWorld slot80 dispatch, below.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldBoundaryInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceSceneInfoAttempted_&&
        sourceSceneInfoPhase_==SourceSceneInfoPhase::boundary&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
bool Owner::continueSourceSceneInfo270900(std::uint32_t& pc)noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||sourceSceneInfoAttempted_||
       sourceTrackStep_!=SourceTrackStep::filenameReturned||sourceTrackPC_!=0x1aa078||
       !sourceWorldLogAttempted_||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||sourceLoadApplication_->entities()!=worldApplication_||
       !sourceConfig34Published_||!sourceWorldConfig_||!sourceWorldConfig_->source_||!sourceTrackFile_||
       sourceWorldConfig_->file_!=sourceTrackFile_||sourceWorldConfig_->returned_!=Config::Return::one||
       !worldParser34_||*worldParser34_!=sourceWorldConfig_->source_.get()||
       !sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00))return false;
    sourceSceneInfoAttempted_=true;sourceTrackStep_=SourceTrackStep::sceneInfo;
    const auto stop=[&](std::uint32_t at)noexcept {pc=sourceTrackPC_=at;sourceTrackStep_=SourceTrackStep::terminal;return false;};
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;std::int32_t value=0;
    // Most-derived3CF110 slot80 is genuinely270900. Actual retained Config34
    // is non-NULL; no source NULL/error assertion branch is skipped by a guess.
    sourceSceneInfoPhase_=SourceSceneInfoPhase::select;sourceTrackPC_=pc=0x2709a4;
    if(!sourceWorldConfig_->selectSceneInfo157D00(returned,d))
        return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    // Selector's actual v0 is ignored at2709AC, not converted into load success.
    sourceSceneInfoPhase_=SourceSceneInfoPhase::name;sourceTrackPC_=pc=0x2709bc;
    if(!sourceWorldConfig_->readSceneName158240(worldText244_,returned,d))
        return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    // Original2709BC uses actual World244, max=-1, and ignores getter v0.
    // Binary missing leaves it untouched; text missing writes NUL; text found
    // copies only the genuine value prefix. Unknown suffix remains unknown.
    const auto warn=[&](std::size_t slot,const char* text,std::uint32_t at)noexcept {
        sourceTrackPC_=pc=at;auto& receipt=sourceSceneWarnings_[slot];
        if(receipt.attempted)return false;
        receipt.requested=std::strlen(text);receipt.attempted=true;
        receipt.written=std::fwrite(text,1,receipt.requested,stdout);
        receipt.error=receipt.written!=receipt.requested||std::ferror(stdout)!=0;
        return true; // Explicit native logging; source caller ignores stdio return.
    };
    sourceSceneInfoPhase_=SourceSceneInfoPhase::portals;sourceTrackPC_=pc=0x2709d4;
    if(!sourceWorldConfig_->readSceneInfoInteger158B30("NumPortals",0,value,returned,d))
        return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    worldPortals358_=static_cast<std::uint32_t>(value); // Same actual158B30 target.
    if(!returned&&!warn(0,"Number of portals not specified.  Using 0.\n",0x2709e8))return stop(pc);
    sourceSceneInfoPhase_=SourceSceneInfoPhase::zones;sourceTrackPC_=pc=0x270a00;
    if(!sourceWorldConfig_->readSceneInfoInteger158B30("NumZones",1,value,returned,d))
        return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    worldZones34C_=static_cast<std::uint32_t>(value);
    if(!returned&&!warn(1,"Number of zones not specified.  Using 1.\n",0x270a14))return stop(pc);
    sourceSceneInfoPhase_=SourceSceneInfoPhase::environment;sourceTrackPC_=pc=0x270a2c;
    if(!sourceWorldConfig_->readSceneInfoInteger158B30("UseEnvironmentNode",0,value,returned,d))
        return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    useEnvironment35C_=static_cast<std::uint32_t>(value);
    if(!returned&&!warn(2,"UseEnvironmentNode not specified.  Using F.\n",0x270a40))return stop(pc);
    sourceSceneInfoPhase_=SourceSceneInfoPhase::boundary;sourceTrackPC_=pc=0x270a50;
    // Most-derived F8 dispatch is272970. Its actual0/1 return is ignored by
    //270A58; helper bool distinguishes completed call from retained bound.
    if(!allocateLoadSourceWorldBoundary272970(pc))return stop(pc);
    sourceSceneInfoPhase_=SourceSceneInfoPhase::returnedPrefix;sourceTrackPC_=pc=0x270a58;
    return continueSourceEnvironment270A58(pc); // Same actual270900 caller, not a readiness setter.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueSourceEnvironment270A58(std::uint32_t& pc)noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    using Environment=g::original_world_environment50KG::Owner;
    using EnvironmentDiagnostic=g::original_world_environment50KG::Diagnostic;
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||
       sourceTrackStep_!=SourceTrackStep::sceneInfo||!sourceSceneInfoAttempted_||
       sourceSceneInfoPhase_!=SourceSceneInfoPhase::returnedPrefix||pc!=0x270a58||
       sourceEnvironmentPhase_!=SourceEnvironmentPhase::unreached||sourceEnvironment3A4_||
       !nativeEnvironment3A4_||*nativeEnvironment3A4_!=nullptr||
       !worldZones34C_||*worldZones34C_!=10||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!sourceConfig34Published_||
       !sourceWorldConfig_||!sourceWorldConfig_->source_||!sourceTrackFile_||
       sourceWorldConfig_->file_!=sourceTrackFile_||sourceWorldConfig_->returned_!=Config::Return::one||
       !worldParser34_||*worldParser34_!=sourceWorldConfig_->source_.get()||
       !sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00))return false;
    sourceEnvironmentPhase_=SourceEnvironmentPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept {
        sourceEnvironmentPhase_=SourceEnvironmentPhase::bounded;
        sourceTrackStep_=SourceTrackStep::terminal;sourceTrackPC_=pc=at;return false;
    };
    EnvironmentDiagnostic d;
    // PIMPL capacity is native preadmission. A refusal here produces no guest
    // allocation return or World3A4 store; PC0 distinguishes that refusal.
    if(!Environment::allocateSource270A58(worldEntity_,*sourceWorldConfig_,sourceEnvironment3A4_,d))
        return stop(d.sourcePC);
    nativeEnvironment3A4_=sourceEnvironment3A4_.get(); // Unconditional270A70, including NULL.
    sourceEnvironmentPublished_=true;sourceTrackPC_=pc=0x270a7c;
    if(!sourceEnvironment3A4_)return stop(0x270a7c); // First actual NULL dereference.
    bool returned=false;
    if(!sourceEnvironment3A4_->loadSource276C00(worldEntity_,*sourceWorldConfig_,
            static_cast<std::int32_t>(*worldZones34C_),returned,d))return stop(d.sourcePC);
    //270A8C tests the real loader return, not the native helper's bounded status.
    if(!returned){
        sourceTrackPC_=pc=0x270a98;
        if(!sourceEnvironment3A4_->retireSource276B30(1,d))return stop(d.sourcePC);
        sourceEnvironment3A4_.reset();
        nativeEnvironment3A4_=nullptr; // Actual caller270AA0 only on returned zero.
        sourceEnvironmentPhase_=SourceEnvironmentPhase::returnedZero;
    }else sourceEnvironmentPhase_=SourceEnvironmentPhase::returnedOne;
    sourceTrackPC_=pc=0x270aa8;
    if(!loadSourceWorldLights270AA8(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldFog270B14(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldZones270B58(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldPortals270BF8(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldPhysics270CC4(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldInstanceMaster270FD0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldStaticModels2710F0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceWorldStaticModels27B89C(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldCollisionObjects1B4EC0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldControllers270D1C(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!finishSourceWorld270E80(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    if(!closeSourceSceneConfig271C10(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    if(!loadSourceWorldObjects1B4330(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    //Actual1AA0B8 compares the genuine vA0 return with1. The zero branch
    //skips vA4/vE0; preserve that distinct next boundary rather than invoke it.
    if(!sourceObjectReturnValue_)return false;
    if(*sourceObjectReturnValue_!=1){sourceTrackPC_=pc=0x1aa0e8;return true;}
    if(!loadSourceWorldTerrain271370(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldToy2713E0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldTriggers1B5000(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceWorldTrigger2DBF98(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceTriggerInstance2DBFE0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceWorldTriggerAction2DC024(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceWorldTriggerList2DC5D0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceWorldPhysicsObjects2718C0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldCsCameras272380(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldSunFlare1B46E0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceSunFlareMaterial2C5AE0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceSunFlarePacket2C5AE8(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceSunFlareRetrieve2C5C3C(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!continueSourceSunFlareSaturation2720D0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldParticleDescriptors2722F0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldBuoys1B4890(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldStaticModelParams271A00(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!completeSourceWorldObjects271DB0(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!closeSourceObjectConfig272800(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldAudio1B4720(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    //Actual1AA0FC/100 compares the returned scalar to1. Zero skips vE8/F4;
    //retain that exact next caller boundary without inventing an Ambient call.
    if(!sourceAudioReturnValue_){sourceTrackStep_=SourceTrackStep::terminal;return false;}
    if(*sourceAudioReturnValue_!=1){pc=sourceTrackPC_=0x1aa130;return true;}
    pc=sourceTrackPC_=0x1aa114; //Actual most-derived vE8→vEC dispatch.
    if(!loadSourceWorldAmbient271470(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldSoulTreeSounds2714D8(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldMaterialSounds271530(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldMaterialAssignments271600(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!closeSourceAudioConfig272930(pc)){
        sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;
    }
    sourceTrackPC_=pc;
    if(!loadSourceWorldTopology2729E0(pc)){sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;}
    sourceTrackPC_=pc;
    if(!continueSourceWorldSearchNodes272A50(pc)){sourceTrackPC_=pc;sourceTopologyPhase_=SourceTopologyPhase::bounded;sourceTrackStep_=SourceTrackStep::terminal;return false;}
    sourceTopologyPhase_=SourceTopologyPhase::nodesReturned;
    sourceTrackPC_=sourceTopologyPC_=pc;
    if(!continueSourceWorldPortalGraph272B28(pc)){sourceTrackPC_=sourceTopologyPC_=pc;sourceTopologyPhase_=SourceTopologyPhase::bounded;sourceTrackStep_=SourceTrackStep::terminal;return false;}
    sourceTopologyPhase_=SourceTopologyPhase::portalsReturned;
    sourceTrackPC_=sourceTopologyPC_=pc;
    if(!continueSourceWorldMethodArrays272C90(pc)){sourceTrackPC_=sourceTopologyPC_=pc;sourceTopologyPhase_=SourceTopologyPhase::bounded;sourceTrackStep_=SourceTrackStep::terminal;return false;}
    sourceTopologyPhase_=SourceTopologyPhase::methodsReturned;
    sourceTrackPC_=sourceTopologyPC_=pc;
    if(!continueSourceDefaultObserver272F28(pc)){sourceTrackPC_=sourceTopologyPC_=pc;sourceTopologyPhase_=SourceTopologyPhase::bounded;sourceTrackStep_=SourceTrackStep::terminal;return false;}
    sourceTopologyPhase_=SourceTopologyPhase::observerReturned;
    sourceTrackPC_=sourceTopologyPC_=pc;
    if(!continueSourceLightPopulation1AA144(pc)){sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;}
    sourceTrackPC_=pc;
    if(!continueSourceFogAndParticle1AA158(pc)){sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;}
    sourceTrackPC_=pc;
    if(!continueSourcePostParticleModels1AA194(pc)){sourceTrackPC_=pc;sourceTrackStep_=SourceTrackStep::terminal;return false;}
    return true; //Only represented source prefix, never whole World readiness.
}
bool Owner::abortSourceEnvironmentNative(std::uint32_t& pc)noexcept {
    if(sourceParticleInvocationBorrowers_||sourceParticleConfig_||sourceFogTransferBorrowers_)return false;
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceEnvironmentPhase_==SourceEnvironmentPhase::nativeClosed)return !sourceEnvironment3A4_;
    if(sourceEnvironmentPhase_==SourceEnvironmentPhase::unreached)return !sourceEnvironment3A4_;
    if(sourceEnvironmentPublished_&&(!nativeEnvironment3A4_||
            *nativeEnvironment3A4_!=sourceEnvironment3A4_.get()))return false;
    g::original_world_environment50KG::Diagnostic d;
    if(sourceEnvironment3A4_){
        if(!sourceEnvironment3A4_->preflightSourceNative()||
           !sourceEnvironment3A4_->abortSourceNative(d)){pc=d.sourcePC;return false;}
    }
    // Withdraw typed knowledge before freeing the backing. This is NOT a guest
    // NULL assignment; genuine returned-zero NULL was already stored above.
    if(sourceEnvironmentPublished_)nativeEnvironment3A4_.reset();
    sourceEnvironment3A4_.reset();sourceEnvironmentPhase_=SourceEnvironmentPhase::nativeClosed;
    return true;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldLightsInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270aa8&&
        sourceSceneInfoAttempted_&&sourceSceneInfoPhase_==SourceSceneInfoPhase::returnedPrefix&&
        (sourceEnvironmentPhase_==SourceEnvironmentPhase::returnedOne||
         sourceEnvironmentPhase_==SourceEnvironmentPhase::returnedZero)&&
        sourceEnvironmentPublished_&&nativeEnvironment3A4_&&
        *nativeEnvironment3A4_==sourceEnvironment3A4_.get()&&
        !nativeLightList3A8_&&sourceLight3A8_&&*sourceLight3A8_==nullptr&&
        worldZones34C_&&*worldZones34C_==10&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldFogInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270b14&&
        sourceSceneInfoAttempted_&&sourceSceneInfoPhase_==SourceSceneInfoPhase::returnedPrefix&&
        (sourceLightsPhase_==SourceLightsPhase::returnedOne||sourceLightsPhase_==SourceLightsPhase::returnedZero)&&
        sourceLight3A8_&&*sourceLight3A8_==nativeLightList3A8_.get()&&
        !nativeFogDescriptor3AC_&&sourceFog3AC_&&*sourceFog3AC_==nullptr&&
        worldZones34C_&&*worldZones34C_==10&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldZonesInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270b58&&
        sourceSceneInfoAttempted_&&sourceSceneInfoPhase_==SourceSceneInfoPhase::returnedPrefix&&
        (sourceFogPhase_==SourceFogPhase::returnedOne||sourceFogPhase_==SourceFogPhase::returnedZero)&&
        sourceFog3AC_&&*sourceFog3AC_==nativeFogDescriptor3AC_.get()&&
        worldZoneInfo350_&&*worldZoneInfo350_==nullptr&&
        worldZones34C_&&*worldZones34C_==10&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldPortalsInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270bf8&&
        sourceSceneInfoAttempted_&&sourceSceneInfoPhase_==SourceSceneInfoPhase::returnedPrefix&&
        sourceZonesPhase_==SourceZonesPhase::returned&&sourceZonesLoaded_==10&&
        sourceZones350_&&worldZoneInfo350_&&*worldZoneInfo350_==sourceZones350_.get()&&
        !nativePortalParameters3A0_&&sourcePortal3A0_&&*sourcePortal3A0_==nullptr&&
        worldPortals358_&&*worldPortals358_==10&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldPhysicsInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270cc4&&
        sourceSceneInfoAttempted_&&sourceSceneInfoPhase_==SourceSceneInfoPhase::returnedPrefix&&
        sourcePortalsPhase_==SourcePortalsPhase::returned&&sourcePortalsLoaded_==10&&
        nativePortalParameters3A0_&&nativePortalParameters3A0_->sourceMode&&
        nativePortalParameters3A0_->sourcePublished&&sourcePortal3A0_&&
        *sourcePortal3A0_==nativePortalParameters3A0_->sourceArray.get()&&*sourcePortal3A0_&&
        !worldPhysics3D4_&&!livePhysicsIndexAttempted_&&!livePhysicsIndexComplete_&&
        worldZones34C_&&*worldZones34C_==10&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldInstanceMasterInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270cec&&
        sourceSceneInfoAttempted_&&sourceSceneInfoPhase_==SourceSceneInfoPhase::returnedPrefix&&
        sourcePhysicsPhase_==SourcePhysicsPhase::returned&&sourcePhysicsPrepareReturned_&&
        worldPhysics3D4_&&sourcePhysics3D4_&&*sourcePhysics3D4_==worldPhysics3D4_.get()&&
        !nativeEmptyInstances3DC_&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadApplication_->state&&sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldStaticModelsInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270cfc&&
        sourceSceneInfoAttempted_&&sourceSceneInfoPhase_==SourceSceneInfoPhase::returnedPrefix&&
        sourceInstancePhase_==SourceInstancePhase::returned&&
        nativeEmptyInstances3DC_&&nativeEmptyInstances3DC_->sourceMode&&nativeEmptyInstances3DC_->complete&&
        sourcePhysicsPhase_==SourcePhysicsPhase::returned&&sourcePhysicsPrepareReturned_&&
        worldPhysics3D4_&&sourcePhysics3D4_&&*sourcePhysics3D4_==worldPhysics3D4_.get()&&
        sourceZonesPhase_==SourceZonesPhase::returned&&sourceZonesLoaded_==10&&
        sourceZones350_&&worldZoneInfo350_&&*worldZoneInfo350_==sourceZones350_.get()&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadApplication_->state&&
        nativeEmptyInstances3DC_->application==sourceLoadApplication_->state&&
        nativeEmptyInstances3DC_->sourceCallerPublished&&
        sourceLoadApplication_->state->instanceMaster444078==nativeEmptyInstances3DC_->master&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_source_camera50LA {
bool OwnershipBridge::bindFreshSourceIncomingZones(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!session.constructorOnly_||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent()||terminal_||closed_||cameraPrepareTerminal_||
       session.closed_||session.abandoned_||sourceIncomingZonesAttempted_||sourceIncomingZonesWithdrawing_||nativeWreckAttempted_||
       nativeWreckZoneOwner_||follow_||queriesConstructed_||!session.constructorWorld_||
       !session.actual_.applicationEntities||session.generation_!=camera_->generation)return false;
    const auto fresh=constructorArena_.lock();
    if(!fresh||!fresh->currentNative()||bankIdentity_!=&fresh->bank_||!actual_||&actual_->frame!=&fresh->live_.state)return false;
    const void* v=nullptr;const void* c=nullptr;
    const auto& lease=*session.constructorWorld_;
    if(!lease.readIncoming210770(session.generation_,
        &session.actual_.applicationEntities->objectCount423F60(),v,c))return false;
    auto* world=lease.actualOwner_;if(!world)return false;
    sourceIncomingZonesAttempted_=true;
    g::OriginalWreckSetupZones50LA zones;
    if(!world->retainNativeWreckFirstZones(this,session.generation_,zones,
        sourceIncomingVisibilityOrdinal_,sourceIncomingCollisionOrdinal_))return false;
    nativeWreckZoneOwner_=world; //Retained before any later refusal/210770 effects.
    if(zones.nativeVisibilityNode!=v||zones.nativeCollisionNode!=c||!zones.visibility4C0||
       !zones.collision4C4||!zones.collisionLifecycle)return false;
    sourceIncomingVisibility_=v;sourceIncomingCollision_=c;
    sourceIncomingVisibilityWorld_=zones.visibility4C0;
    sourceIncomingCollisionWorld_=zones.collision4C4;sourceIncomingCollisionLifecycle_=zones.collisionLifecycle;
    sourceIncomingZonesReady_=true;return true;
}
bool OwnershipBridge::withdrawFreshSourceIncomingZones(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!session.constructorOnly_||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent())return false;
    sourceIncomingZonesWithdrawing_=true; //One-way admission revocation BEFORE fallible source cleanup.
    return true;
}
bool OwnershipBridge::retireFreshSourceIncomingZones(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!session.constructorOnly_||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent())return false;
    if(!sourceIncomingZonesAttempted_||sourceIncomingZonesWithdrawn_)return true;
    if(sourceWreckConstructorAttempted_&&!sourceWreckConstructorRetired_)return false;
    //Every actual Racer/model prefix has already retired. No copied ready flag
    //stands in for its count/reference teardown; failed earlier cleanup retains this borrow.
    for(std::size_t i=0;i<session.actual_.bank.count();++i)
        if(!session.racerEntities_[i].released())return false;
    if(nativeWreckZoneOwner_){
        if(!nativeWreckZoneOwner_->releaseNativeWreckFirstZones(this))return false;
        nativeWreckZoneOwner_=nullptr;
    }
    sourceIncomingZonesWithdrawn_=true;sourceIncomingZonesReady_=false;return true;
}
bool OwnershipBridge::loadFreshSourceWreckConfig2129A4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    const std::vector<std::uint8_t>& phy,std::uint32_t& pc)noexcept {
    pc=0x2129a4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||nativeWreckPrepared_||follow_||queriesConstructed_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_||
           !rosterResources_->rigs_[slot]||rosterResources_->rigs_[slot]->phy!=phy)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.constructorWorld_->current()||
           session.generation_!=camera_->generation||slot>=session.actual_.bank.count()||
           &session.racerEntities_[slot]!=&racer||racer.racer.objectKey!=session.racerKey(slot)||
           racer.racer.sessionGeneration!=session.generation_)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||!backing.modelSourcesBound())return false;
        if(!sourceWreckConstructorAttempted_){
            sourceWreckConstructorAttempted_=true; //Custody before any per-owner identity mutation.
            std::array<const g::OriginalWreckComponent50LA*,8> racers{};
            for(std::size_t i=0;i<session.actual_.bank.count();++i){
                const auto& actual=session.racerEntities_[i];
                if(!actual.prepared()||actual.racer.objectKey!=session.racerKey(i)||
                   actual.racer.sessionGeneration!=session.generation_)return false;
                racers[i]=&actual.racer;
            }
            std::string error;
            if(!backing.selectFreshSourceIdentities(racers,session.actual_.bank.count(),session.generation_,error))return false;
        }
        return backing.loadFreshSourceFirstCollection2129A4(slot,racer,phy,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckPiece212A50(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x212a50u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||!sourceWreckConstructorAttempted_||
           sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||nativeWreckPrepared_||follow_||queriesConstructed_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.constructorWorld_->current()||
           !session.actual_.actualMaterialDirectory36B970||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||!backing.modelSourcesBound())return false;
        return backing.loadFreshSourceFirstPiece212A50(slot,racer,session.constructorWorld_,
            *session.actual_.actualMaterialDirectory36B970,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckSecondBody165E90(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x165e90u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        g::OriginalWreckSetupZones50LA zones;
        zones.visibility4C0=sourceIncomingVisibilityWorld_;
        zones.visibilityZoneField=&sourceIncomingVisibilityOrdinal_;
        zones.collision4C4=sourceIncomingCollisionWorld_;zones.collisionLifecycle=sourceIncomingCollisionLifecycle_;
        zones.collisionZoneField=&sourceIncomingCollisionOrdinal_;
        zones.nativeVisibilityNode=v;zones.nativeCollisionNode=c;
        return backing.loadFreshSourceSecondBody165E90(slot,racer,zones,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckBody165E90(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x165e90u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        g::OriginalWreckSetupZones50LA zones;
        zones.visibility4C0=sourceIncomingVisibilityWorld_;
        zones.visibilityZoneField=&sourceIncomingVisibilityOrdinal_;
        zones.collision4C4=sourceIncomingCollisionWorld_;zones.collisionLifecycle=sourceIncomingCollisionLifecycle_;
        zones.collisionZoneField=&sourceIncomingCollisionOrdinal_;
        zones.nativeVisibilityNode=v;zones.nativeCollisionNode=c;
        return backing.loadFreshSourceFirstBody165E90(slot,racer,zones,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckSecondE0166018(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166018u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        std::size_t ordinal=0;
        if(!backing.freshSecondE0Reservation166018(slot,racer,ordinal))return false;
        const auto* rows=backing.reservations();
        if(!rows||ordinal>=rows->size()||rows->size()!=inputs_.wreck.size())return false;
        const auto& row=(*rows)[ordinal];const auto& reserved=inputs_.wreck[ordinal];
        if(row.queries!=reserved.queries||row.assets!=reserved.assets||row.identities!=reserved.identities)return false;
        auto& actual=*storage_.get();auto& app=*session.actual_.applicationEntities;
        const auto& physics=app.nativeCollisionCategoryBindings_[1];
        if(std::string_view(physics.name)!="PHYSICSBODY"||!physics.bit||!actual.count.wreckFirstQuery||
           ordinal>=actual.count.wreckReferences/3u)return false;
        g::OriginalWreckQueryWorld50LA world;
        world.lifecycle=&actual.lifecycle;world.world=&actual.world;world.workspace=&actual.membershipWork;
        world.objects=actual.objects.data();world.states=actual.source.data();world.objectCount=actual.count.objects;
        world.identities=row.identities;
        for(unsigned q=0;q<3;++q)world.refs[q]=static_cast<g::CollectionRef50BX>(actual.count.wreckFirstQuery+ordinal*3u+q);
        //Constructor-only metadata borrows the same reserved registry. Future
        //Follow copies/geometry/workspaces do not exist and are not fabricated.
        world.physicsBodyCategory3E0718=&physics.bit;
        world.sentinel3E02C0=&original_follow_query_bridge::QueryBridge::sentinel3E02C0;
        world.globalValid3E0730=&app.collisionGlobalValid3E0730_;
        return backing.loadFreshSourceSecondE0166018(slot,racer,world,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckE0166018(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166018u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        const auto& span=backing.spans_[slot];const auto* rows=backing.reservations();
        if(!rows||!span.count||span.begin>=rows->size()||rows->size()!=inputs_.wreck.size())return false;
        const auto& row=(*rows)[span.begin];const auto& reserved=inputs_.wreck[span.begin];
        if(row.queries!=reserved.queries||row.assets!=reserved.assets||row.identities!=reserved.identities)return false;
        auto& actual=*storage_.get();auto& app=*session.actual_.applicationEntities;
        const auto& physics=app.nativeCollisionCategoryBindings_[1];
        if(std::string_view(physics.name)!="PHYSICSBODY"||!physics.bit||!actual.count.wreckFirstQuery||
           span.begin>=actual.count.wreckReferences/3u)return false;
        g::OriginalWreckQueryWorld50LA world;
        world.lifecycle=&actual.lifecycle;world.world=&actual.world;world.workspace=&actual.membershipWork;
        world.objects=actual.objects.data();world.states=actual.source.data();world.objectCount=actual.count.objects;
        world.identities=row.identities;
        for(unsigned q=0;q<3;++q)world.refs[q]=static_cast<g::CollectionRef50BX>(actual.count.wreckFirstQuery+span.begin*3u+q);
        //Constructor-only metadata borrows the same reserved registry. Future
        //Follow copies/geometry/workspaces do not exist and are not fabricated.
        world.physicsBodyCategory3E0718=&physics.bit;
        world.sentinel3E02C0=&original_follow_query_bridge::QueryBridge::sentinel3E02C0;
        world.globalValid3E0730=&app.collisionGlobalValid3E0730_;
        return backing.loadFreshSourceFirstE0166018(slot,racer,world,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckSecondE0Shape166074(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166074u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.loadFreshSourceSecondE0Shape166074(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::prepareAppendFreshSourceWreckSecondE016607C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16607cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.prepareAppendFreshSourceSecondE016607C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckE0Shape166074(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166074u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.loadFreshSourceFirstE0Shape166074(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::prepareAppendFreshSourceWreckE016607C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16607cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.prepareAppendFreshSourceFirstE016607C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondDC1660A0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1660a0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceSecondDC1660A0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondD816616C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16616cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceSecondD816616C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckDC1660A0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1660a0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceFirstDC1660A0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckD816616C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16616cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceFirstD816616C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructLoadFreshSourceWreckSecondE41661D8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1661d8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructLoadFreshSourceSecondE41661D8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::prepareAppendFreshSourceWreckSecondE416629C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16629cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.prepareAppendFreshSourceSecondE416629C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructLoadFreshSourceWreckE41661D8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1661d8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructLoadFreshSourceFirstE41661D8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::prepareAppendFreshSourceWreckE416629C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16629cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.prepareAppendFreshSourceFirstE416629C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::bindFreshSourceWreckSecondEffectInputs2CB618(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb618u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        //The first piece already acquired this exact World-issued manager pin.
        //Do not replace it or construct an alternate context/domain for piece1.
        auto* manager=nativeWreckZoneOwner_->nativeParticleManager4A0_;
        if(!manager||nativeWreckParticles_!=manager||!manager->prepared()||!manager->particles())return false;
        return backing.bindFreshSourceSecondEffectInputs2CB618(slot,racer,*manager,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondMutual166390(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166390u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceSecondMutual166390(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondE81662E4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1662e4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceSecondE81662E4(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckE81662E4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1662e4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceFirstE81662E4(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::stopFreshSourceWreckSecondFirstPair2CB778(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb778u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.stopFreshSourceSecondFirstPair2CB778(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::stopFreshSourceWreckFirstPair2CB778(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb778u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.stopFreshSourceFirstPair2CB778(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckSecondPiece22C0A0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c0a0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        if(!session.actual_.actualMaterialDirectory36B970)return false;
        return backing.loadFreshSourceSecondPiece22C0A0(slot,racer,*session.actual_.actualMaterialDirectory36B970,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::advanceFreshSourceWreckSecondPieceCursor22C25C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c25cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.advanceFreshSourceSecondPieceCursor22C25C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::setupFreshSourceWreckGroupSparks2CC0D4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cc0d4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.setupFreshSourceGroupSparks2CC0D4(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::beginFreshSourceWreckGroupSparksConfig15F2F0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x15f2f0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.beginFreshSourceGroupSparksConfig15F2F0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckGroupSparks2CC0B4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cc0b4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.constructFreshSourceGroupSparks2CC0B4(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::consumeFreshSourceWreckGroupContext2CC088(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cc088u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.consumeFreshSourceGroupContext2CC088(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::bindFreshSourceWreckGroupPrimary22C298(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c298u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.bindFreshSourceGroupPrimary22C298(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckGroup22C278(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c278u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.constructFreshSourceGroup22C278(slot,racer,session.actual_.applicationEntities->groupOrigin4441F0_,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::advanceFreshSourceWreckPieceCursor22C25C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c25cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.advanceFreshSourcePieceCursor22C25C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::applyFreshSourceWreckSecondWater22C21C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c21cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        const auto& water=session.actual_.applicationEntities->nativeCollisionCategoryBindings_[5];
        if(std::string_view(water.name)!="WATER"||!water.bit)return false;
        //Same App HOST namespace used by existing track/queries; not guest4342A8 numeric bits.
        return backing.applyFreshSourceSecondWater22C21C(slot,racer,water.bit,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::applyFreshSourceWreckWater22C21C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c21cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        const auto& water=session.actual_.applicationEntities->nativeCollisionCategoryBindings_[5];
        if(std::string_view(water.name)!="WATER"||!water.bit)return false;
        //Same App HOST namespace used by existing track/queries; not guest4342A8 numeric bits.
        return backing.applyFreshSourceWater22C21C(slot,racer,water.bit,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::appendFreshSourceWreckSecondPiece22C1B8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb8c4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.appendFreshSourceSecondPiece22C1B8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::appendFreshSourceWreckPiece22C1B8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb8c4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.appendFreshSourcePiece22C1B8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readStopAppendFreshSourceWreckSplash19DDC8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19ddc8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readStopAppendFreshSourceSplash19DDC8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readStopAppendFreshSourceWreckDust19A7D0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7d0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readStopAppendFreshSourceDust19A7D0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readAppendFreshSourceWreckFire15E024(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x15e024u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readAppendFreshSourceFire15E024(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::completeFreshSourceWreckSecondInitialChild19A7F8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7f8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.completeFreshSourceSecondInitialChild19A7F8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::completeFreshSourceWreckSecondSuccessorChild(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x15e0b0u&&pc!=0x19de7cu)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.completeFreshSourceSecondSuccessorChild(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readFreshSourceWreckSecondSuccessorChild(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x15e024u&&pc!=0x19ddc8u)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readFreshSourceSecondSuccessorChild(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readStopAppendFreshSourceWreckSecondPositiveDust19A7D0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7d0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readStopAppendFreshSourceSecondPositiveDust19A7D0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readStopAppendFreshSourceWreckSecondPositiveSplash19DDC8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19ddc8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readStopAppendFreshSourceSecondPositiveSplash19DDC8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readFreshSourceWreckSecondInitialChild19A7D0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7d0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readFreshSourceSecondInitialChild19A7D0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readAppendFreshSourceWreckSmoke19A7D0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7d0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readAppendFreshSourceSmoke19A7D0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::continueFreshSourceWreckSecondPairs2CB6CC(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb6ccu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed only by each actual smoke/fire setup after its reached manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.continueFreshSourceSecondPairs2CB6CC(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::continueFreshSourceWreckPairs2CB6CC(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb6ccu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed only by each actual smoke/fire setup after its reached manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.continueFreshSourcePairs2CB6CC(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckFire2CB720(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb720u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by15DFDC AFTER the actual fire constructor/manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.constructFreshSourceFire2CB720(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondPositiveSplash(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb854u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed only by the positive Splash setup AFTER constructor/manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.constructFreshSourceSecondPositiveSplash(slot,racer,
            &session.actual_.applicationEntities->particleDefault433F50_,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondSuccessorChild(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x2cb720u&&pc!=0x2cb854u)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed only by the selected Fire/Splash setup AFTER constructor/manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.constructFreshSourceSecondSuccessorChild(slot,racer,
            &session.actual_.applicationEntities->particleDefault433F50_,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSplash2CB854(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb854u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19DD80 AFTER the actual splash constructor/manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.constructFreshSourceSplash2CB854(slot,racer,
            &session.actual_.applicationEntities->particleDefault433F50_,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondPositiveDust(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb7e0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19A788 AFTER the actual selected child constructor/manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.constructFreshSourceSecondPositiveDust(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondInitialChild(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x2cb6ccu&&pc!=0x2cb7e4u)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19A788 AFTER the actual selected child constructor/manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.constructFreshSourceSecondInitialChild(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckDust2CB7E0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x2cb7e0u&&pc!=0x2cb7e4u)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19A788 AFTER the actual dust constructor/manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.constructFreshSourceDust2CB7E0(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSmoke2CB6CC(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb6ccu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19A788 AFTER the actual smoke constructor/manager stores.
        const std::string* directory=nullptr;
        auto* world=session.constructorWorld_->actualOwner_;
        if(world)directory=world->sourceParticleDirectory36B530(session.actual_.applicationEntities.get());
        return backing.constructFreshSourceSmoke2CB6CC(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondArrays2CB628(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb628u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.constructFreshSourceSecondArrays2CB628(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckArrays2CB628(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb628u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->nativeParticleManager4A0_||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.constructFreshSourceArrays2CB628(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckMutual166390(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166390u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           session.constructorWorld_->actualOwner_!=nativeWreckZoneOwner_||
           nativeWreckZoneOwner_->nativeWreckZoneBorrower_!=this||
           nativeWreckZoneOwner_->nativeWreckVisibilityNode_!=sourceIncomingVisibility_||
           nativeWreckZoneOwner_->nativeWreckCollisionNode_!=sourceIncomingCollision_||
           !nativeWreckZoneOwner_->sourceTopologyCurrent(true)||!storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        if(!backing.constructFreshSourceMutual166390(slot,racer,pc))return false;
        //Only now at2CB618 consume the actual incoming Track4A0 context.
        //1AA57C -> wrapper chain ->210770 t2; same object as Racer210890.
        auto* manager=nativeWreckZoneOwner_->nativeParticleManager4A0_;
        if(!manager||!manager->prepared()||!manager->particles())return false;
        if(nativeWreckParticles_){
            if(nativeWreckParticles_!=manager)return false; //Same private World-issued pin; release clears both sides.
        }else if(!nativeWreckZoneOwner_->retainNativeWreckParticles(this,manager))return false;
        return backing.bindFreshSourceEffectInputs2CB618(slot,racer,*manager,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::retireFreshSourceWreckConstructors(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!session.constructorOnly_||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent())return false;
    if(!sourceWreckConstructorAttempted_||sourceWreckConstructorRetired_)return true;
    if(closed_||follow_||queriesConstructed_||nativeWreckAttempted_||!rosterResources_||!rosterResources_->wreckBacking_)return false;
    sourceWreckConstructorWithdrawing_=true; //One-way; a rejected cleanup cannot reopen construction.
    auto& backing=*rosterResources_->wreckBacking_;
    if(!backing.retireFreshSourceConstructors())return false;
    //There are no prepared/active query aliases on this route. Still perform
    //the same two-phase native cancellation before their retained backing dies.
    for(const auto& row:inputs_.wreck)if(!row.queries||row.queries->prepared()||row.queries->constructed())return false;
    if(!backing.retireNativeColdQueries(nullptr))return false;
    inputs_.wreck.clear(); //Withdraw actual Bridge borrows BEFORE backing destruction.
    if(!backing.releaseNativeColdBacking())return false;
    sourceWreckConstructorRetired_=true;return true;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldStaticCohortInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x27b89c&&
        sourceStaticPhase_==SourceStaticPhase::prefixComplete&&sourceStaticOwner_&&
        sourceModelOwner3C4_==sourceStaticOwner_.get()&&modelParents3C4Current(sourceStaticOwner_.get(),false)&&
        sourceStaticZoneIndex_==0&&sourceStaticModelIndex_==0&&sourceStaticPhysicsCount_==0&&
        sourceStaticSucceededZones_==0&&sourceStaticZonePointerOffset_==0&&sourceStaticZoneByteOffset_==0&&
        sourceStaticSlotByteOffset_==0&&sourceStaticZoneArgIndex_==0&&sourceStaticResolvedType_&&*sourceStaticResolvedType_==1&&
        sourceStaticPhysicsArg_==worldPhysics3D4_.get()&&sourceStaticConfigArg_==sourceWorldConfig_.get()&&
        sourceStaticZoneArg_==sourceZones350_.get()&&preflightSourceWorldStaticModelsNative()&&
        sourceInstancePhase_==SourceInstancePhase::returned&&nativeEmptyInstances3DC_&&
        nativeEmptyInstances3DC_->sourceMode&&nativeEmptyInstances3DC_->complete&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadApplication_->state&&
        nativeEmptyInstances3DC_->application==sourceLoadApplication_->state&&
        sourceLoadApplication_->state->instanceMaster444078==nativeEmptyInstances3DC_->master&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceWorldConfig_->sourceFirstStaticModelConfigCurrent()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldCollisionInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270d0c&&
        sourceStaticPhase_==SourceStaticPhase::returned&&sourceStaticOwner_&&
        sourceStaticOwner_->sourceAllDescriptorsReturned_&&sourceStaticZoneIndex_==10&&
        sourceStaticSucceededZones_==10&&sourceModelParentsPublished3C4_==10&&
        sourceModelOwner3C4_==sourceStaticOwner_.get()&&
        modelParents3C4Current(sourceStaticOwner_.get(),false)&&preflightSourceWorldStaticModelsNative()&&
        sourcePhysicsPhase_==SourcePhysicsPhase::returned&&sourcePhysicsPrepareReturned_&&
        worldPhysics3D4_&&sourcePhysics3D4_&&*sourcePhysics3D4_==worldPhysics3D4_.get()&&
        sourceZonesPhase_==SourceZonesPhase::returned&&sourceZonesLoaded_==10&&sourceZones350_&&
        worldZoneInfo350_&&*worldZoneInfo350_==sourceZones350_.get()&&
        !worldCollisionObjects3CC_&&!worldCollisionResources_&&!nativeCollisionModelBorrow_&&
        !nativeCollisionPrefixEntered_&&!nativeCollisionServiceTerminal_&&!nativeCollisionServiceComplete_&&
        sourceInstancePhase_==SourceInstancePhase::returned&&nativeEmptyInstances3DC_&&
        nativeEmptyInstances3DC_->sourceMode&&nativeEmptyInstances3DC_->complete&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadApplication_->state&&
        nativeEmptyInstances3DC_->application==sourceLoadApplication_->state&&
        nativeEmptyInstances3DC_->sourceCallerPublished&&
        sourceLoadApplication_->state->instanceMaster444078==nativeEmptyInstances3DC_->master&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceWorldConfig_->sourceStaticModelNextSelection_==1261&&
        sourceWorldConfig_->sourceStaticModelSelected_&&*sourceWorldConfig_->sourceStaticModelSelected_==1260&&
        sourceWorldConfig_->sourceStaticModelConfigCurrent()&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldTailInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::sceneInfo&&sourceTrackPC_==0x270d1c&&
        sourceSceneInfoPhase_==SourceSceneInfoPhase::returnedPrefix&&
        sourceCollisionPhase_==SourceCollisionPhase::returned&&sourceCollisionObjectsAttempted_&&
        sourceCollisionZoneIndex_==10&&sourceCollisionSucceeded_==10&&
        worldCollisionObjects3CC_&&sourceCollisionObjects3CC_&&
        *sourceCollisionObjects3CC_==worldCollisionObjects3CC_->sourceArray3CC_.get()&&
        worldCollisionObjects3CC_->sourceArrayKnown_==10&&preflightSourceWorldCollisionNative()&&
        sourceStaticPhase_==SourceStaticPhase::returned&&preflightSourceWorldStaticModelsNative()&&
        sourcePhysicsPhase_==SourcePhysicsPhase::returned&&sourcePhysicsPrepareReturned_&&
        worldPhysics3D4_&&sourcePhysics3D4_&&*sourcePhysics3D4_==worldPhysics3D4_.get()&&
        sourceZonesPhase_==SourceZonesPhase::returned&&sourceZonesLoaded_==10&&sourceZones350_&&
        worldZoneInfo350_&&*worldZoneInfo350_==sourceZones350_.get()&&
        sourceInstancePhase_==SourceInstancePhase::returned&&nativeEmptyInstances3DC_&&
        nativeEmptyInstances3DC_->sourceMode&&nativeEmptyInstances3DC_->complete&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadApplication_->state&&
        nativeEmptyInstances3DC_->application==sourceLoadApplication_->state&&
        nativeEmptyInstances3DC_->sourceCallerPublished&&
        sourceLoadApplication_->state->instanceMaster444078==nativeEmptyInstances3DC_->master&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceConfig34Published_&&
        sourceWorldConfig_&&sourceWorldConfig_->source_&&sourceTrackFile_&&
        sourceWorldConfig_->file_==sourceTrackFile_&&sourceWorldConfig_->returned_==Config::Return::one&&
        worldParser34_&&*worldParser34_==sourceWorldConfig_->source_.get()&&
        sourceWorldConfig_->sourceCollisionDescriptorNextSelection_==1261&&
        sourceWorldConfig_->sourceCollisionDescriptorSelected_&&*sourceWorldConfig_->sourceCollisionDescriptorSelected_==1260&&
        sourceWorldConfig_->sourceCollisionDescriptorConfigCurrent()&&!sourceAmbientFlashMinimum368_&&
        sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00);
}
bool Owner::finishSourceWorld270E80(std::uint32_t& pc)noexcept {
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||
       sourceTrackStep_!=SourceTrackStep::sceneInfo||sourceTrackPC_!=0x270e80||
       sourceControllerPhase_!=SourceControllerPhase::returnedControllers||
       sourceSceneInfoPhase_!=SourceSceneInfoPhase::returnedPrefix||
       sourceCollisionPhase_!=SourceCollisionPhase::returned||!preflightSourceWorldControllersNative()||
       !preflightSourceWorldCollisionNative()||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||sourceLoadApplication_->entities()!=worldApplication_||
       !sourceWorldConfig_||!sourceWorldConfig_->source_||!sourceTrackFile_||
       sourceWorldConfig_->file_!=sourceTrackFile_||!worldParser34_||
       *worldParser34_!=sourceWorldConfig_->source_.get()||sourceAmbientFlashMinimum368_||
       !sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00))return false;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceTrackPC_=at;sourceTrackStep_=SourceTrackStep::terminal;return false;};
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    pc=0x270e88;
    if(!sourceWorldConfig_->selectSourceWorldTailEnvironment157D00(returned,d))
        return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    //Actual selector return is ignored. An unexpected missing section would
    //make the following getter search retained collision data; this branch is
    //explicitly bounded, not relabeled as a guest failure or cache reset.
    if(!returned)return stop(0x270ea0);
    pc=0x270ea0;
    if(!sourceWorldConfig_->readSourceAmbientFlashMinimum158B80(sourceAmbientFlashMinimum368_,returned,d))
        return stop(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    if(returned)return stop(0x270eb0); //Unjoined positive flash setup, never fabricated absent.
    if(!sourceAmbientFlashMinimum368_||*sourceAmbientFlashMinimum368_!=0)return stop(0x270ea8);
    //Real missing getter0 takes270EA8; delay270EAC writes v0=1. No390 flag,
    //other flash scalars/colors or360 state is written on this taken branch.
    sourceSceneInfoPhase_=SourceSceneInfoPhase::returnedOne;
    sourceTrackStep_=SourceTrackStep::sceneInfoReturned;
    pc=sourceTrackPC_=0x1aa08c;return true; //True270900 return1; before actual caller v9C.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldTerrainInvocationCurrent()const noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::objectsReturned&&sourceTrackPC_==0x1aa0b4&&
        sourceObjectPhase_==SourceObjectPhase::returnedOne&&sourceObjectReturnValue_&&*sourceObjectReturnValue_==1&&
        sourceSceneConfigCloseReturned_&&sourceSceneConfigNativeWithdrawn_&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadApplication_->state&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&sourceLoadApplication_->entities()==worldApplication_&&
        worldColdConstructed_&&worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&
        sourceObjectConfig34Published_&&sourceWorldObjectConfig_&&sourceWorldObjectConfig_->source_&&
        sourceTrackFile_&&sourceWorldObjectConfig_->file_==sourceTrackFile_&&
        worldParser34_&&*worldParser34_==sourceWorldObjectConfig_->source_.get()&&
        sourceWorldObjectConfig_->objectSectionPhase_==Config::ObjectSectionPhase::animationsMissing&&
        sourceWorldObjectConfig_->objectPodReads_==2&&sourceWorldObjectConfig_->sourceObjectPodCurrent()&&
        sourceTrackFile_->current(*sourceWorldObjectConfig_,sourceWorldObjectConfig_->source_->file00)&&
        sourceCollisionPhase_==SourceCollisionPhase::returned&&sourceCollisionZoneIndex_==10&&
        sourceCollisionSucceeded_==10&&worldCollisionObjects3CC_&&sourceCollisionObjects3CC_&&
        *sourceCollisionObjects3CC_==worldCollisionObjects3CC_->sourceArray3CC_.get()&&
        worldCollisionObjects3CC_->sourceArrayKnown_==10&&preflightSourceWorldCollisionNative()&&
        sourceZonesPhase_==SourceZonesPhase::returned&&sourceZonesLoaded_==10&&sourceZones350_&&
        worldZoneInfo350_&&*worldZoneInfo350_==sourceZones350_.get();
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldToyInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::objectsReturned&&sourceTrackPC_==0x271d2c&&
        sourceObjectPhase_==SourceObjectPhase::returnedOne&&sourceObjectReturnValue_&&*sourceObjectReturnValue_==1&&
        sourceTerrainPhase_==SourceTerrainPhase::returned&&nativeTerrain3B0_&&nativeTerrain3B0_->sourceMode&&
        nativeTerrain3B0_->sourceLoaderReturned&&nativeTerrain3B0_->sourceRowsSelected==8&&
        nativeTerrain3B0_->sourceMappingsCompleted==9&&preflightSourceWorldTerrainNative()&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadApplication_->state&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&sourceLoadApplication_->entities()==worldApplication_&&
        worldColdConstructed_&&worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&
        sourceObjectConfig34Published_&&sourceWorldObjectConfig_&&sourceWorldObjectConfig_->source_&&
        sourceTrackFile_&&sourceWorldObjectConfig_->file_==sourceTrackFile_&&
        worldParser34_&&*worldParser34_==sourceWorldObjectConfig_->source_.get()&&
        sourceWorldObjectConfig_->sourceTerrainNextSelection_==18&&sourceWorldObjectConfig_->sourceTerrainSelected_&&
        *sourceWorldObjectConfig_->sourceTerrainSelected_==17&&sourceWorldObjectConfig_->sourceTerrainConfigCurrent()&&
        sourceTrackFile_->current(*sourceWorldObjectConfig_,sourceWorldObjectConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldTriggerInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!terminal_&&!closed_&&
        sourceTrackStep_==SourceTrackStep::objectsReturned&&sourceTrackPC_==0x271d3c&&
        sourceObjectPhase_==SourceObjectPhase::returnedOne&&sourceObjectReturnValue_&&*sourceObjectReturnValue_==1&&
        sourceToyPhase_==SourceToyPhase::returned&&sourceToyLoaderReturnedZero_&&sourceToyPublished_&&
        sourceToy3C8_&&!*sourceToy3C8_&&!nativeEmptyToyList3C8_&&preflightSourceWorldToyNative()&&
        sourceTerrainPhase_==SourceTerrainPhase::returned&&nativeTerrain3B0_&&nativeTerrain3B0_->sourceMode&&
        nativeTerrain3B0_->sourceMappingsCompleted==9&&preflightSourceWorldTerrainNative()&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadApplication_->state&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&sourceLoadApplication_->entities()==worldApplication_&&
        worldColdConstructed_&&worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&
        sourceObjectConfig34Published_&&sourceWorldObjectConfig_&&sourceWorldObjectConfig_->source_&&
        sourceTrackFile_&&sourceWorldObjectConfig_->file_==sourceTrackFile_&&
        worldParser34_&&*worldParser34_==sourceWorldObjectConfig_->source_.get()&&
        sourceWorldObjectConfig_->sourceToyAnimationsMissingCurrent()&&
        sourceTrackFile_->current(*sourceWorldObjectConfig_,sourceWorldObjectConfig_->source_->file00);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldCsCamerasInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTrackPC_==0x271d5c&&sourcePhysicsObjectsAttempted_&&sourcePhysicsObjectsReturned_&&
        sourcePhysicsObjectStack14C_&&*sourcePhysicsObjectStack14C_==0&&
        sourceTriggerPhase_==SourceTriggerPhase::listReturned&&sourceTriggerListReturn_&&*sourceTriggerListReturn_==1&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&worldParser34_&&sourceWorldObjectConfig_&&
        sourceWorldObjectConfig_->source_&&*worldParser34_==sourceWorldObjectConfig_->source_.get()&&
        sourceTriggerConfigArg_==sourceWorldObjectConfig_.get()&&
        sourceWorldObjectConfig_->sourcePhysicsObjectCountReturned_&&
        sourceWorldObjectConfig_->sourcePhysicsObjectsMissingCurrent()&&
        !nativeCsCameraTable3E4_&&nativeCsCameraCount3E4_&&*nativeCsCameraCount3E4_==0&&
        nativeCsCameraZones3E8_&&!*nativeCsCameraZones3E8_&&
        worldZones34C_&&*worldZones34C_==10&&preflightSourceWorldTriggerNative();
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldSunFlareInvocationCurrent()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceTrackPC_!=0x271d6c||sourceCsCameraPhase_!=SourceCsCameraPhase::returned||
       sourceCsCameraPC_!=0x271d6c||!preflightSourceWorldCsCamerasNative()||
       !nativeCsCameraTable3E4_||!nativeCsCameraTable3E4_->sourceMode||
       !nativeCsCameraTable3E4_->sourceRows||nativeCsCameraTable3E4_->next!=7||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||!sourceWorldObjectConfig_||
       !sourceWorldObjectConfig_->source_||*worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       !sourceWorldObjectConfig_->sourceCsCameraConfigCurrent()||
       sourceWorldObjectConfig_->sourceCsCameraNextSelection_!=8||
       !sourceWorldObjectConfig_->sourceCsCameraSelected_||*sourceWorldObjectConfig_->sourceCsCameraSelected_!=7||
       !worldZones34C_||*worldZones34C_!=10||nativeSunFlares_||nativeSunFlareRead271DD0_||
       !nativeSunFlarePointers4A4_||*nativeSunFlarePointers4A4_)return false;
    for(const auto& zone:*nativeCsCameraTable3E4_->sourceRows)if(!zone)return false;
    return true;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldSunFlareMaterialInvocationCurrent()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceTrackPC_!=0x2c5ae0||sourceSunFlarePC_!=0x2c5ae0||
       sourceSunFlarePhase_!=SourceSunFlarePhase::materialPrefix||
       !preflightSourceWorldSunFlareNative()||!sourceSunFlareStack_||
       !sourceSunFlareStack_->textureKnown||!nativeSunFlares_||!nativeSunFlares_->sourceMode||
       nativeSunFlares_->sourceNativeClosed||!nativeSunFlares_->owner||
       !nativeSunFlares_->owner->sourceConstructorReturned||
       !nativeSunFlares_->owner->sourceSpritePublished||!nativeSunFlares_->owner->sprite64||
       !sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||
       *worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       !sourceWorldObjectConfig_->sourceSunFlareConfigCurrent()||
       sourceWorldObjectConfig_->sourceSunFlareNextSelection_!=2||
       !sourceWorldObjectConfig_->sourceSunFlareSelected_||
       *sourceWorldObjectConfig_->sourceSunFlareSelected_!=1||
       !sourceTrackMountComplete_||sourceTrackMountClosed_||
       sourceTrackMountApplication_!=sourceLoadApplication_||sourceTrackMountSource_!=source_||
       !sourceTrackMountDomain_||sourceTrackMountLease_.closed_||
       sourceTrackMountLease_.provider_!=sourceTrackMountDomain_->archiveProvider3E1A70())return false;
    // These owners already exist. No constructor/context/Video is synthesized.
    const auto& domain=sourceLoadApplication_->standardMaterialDomainOwner_;
    if(domain!=sourceTrackMountDomain_||!domain->prepared()||
       !sourceLoadApplication_->materialApplication)return false;
    const auto& table=domain->materials382B38();
    return table&&table->sourceNames17CAD4Current()&&
        table->screenApplicationMatches(*sourceLoadApplication_->materialApplication);
}
}

namespace original_main_entry50KG {
bool RaceDisplayLifetime50LA::readSourceLoadingViewport115990(
    const std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& camera,
    std::array<std::int32_t,4>& out)const noexcept {
    using Camera=rr::game::OriginalFrontendCameraCold50LA;
    if(!ownerThread()||!application_||!frontendOwnerCurrent50KG()||!frontendCreated_||
       retiring_||!sourceCurrentCamera0Known_||!camera||sourceSelectedCamera0_!=camera||
       sourceSelectedCamera0_.owner_before(camera)||camera.owner_before(sourceSelectedCamera0_)||
       selectedGameplayCamera0_||camera->phase_!=Camera::Phase::viewportInitialized2361C0||
       !camera->unpublishedAbortCurrent(application_,application_->entities(),true))return false;
    // Actual115990 load/store sequence, including wrapping ADDU. This consumes
    // the same selected counted Camera fields, not diagnostic knownWord data.
    const auto& v=camera->fields_.viewport1E0;
    const auto put=[&](std::size_t i,std::uint32_t bits)noexcept {std::memcpy(&out[i],&bits,4);};
    put(0,v[0]);             //115994
    put(2,v[0]+v[2]);        //1159A4
    put(1,v[1]);             //1159AC
    put(3,v[1]+v[3]);        //1159C0 return delay slot
    return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::readSourceSunFlareVideo60(std::int32_t& out)const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceLoadApplication_||!sourceLoadRace_||
       sourceLoadApplication_->entities()!=worldApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_)return false;
    const auto video=sourceLoadApplication_->videoOwner_; //Never invokes lazy videoForSession.
    const auto display=sourceLoadRace_->loadDisplay_;
    if(!video||!display||display->application_!=sourceLoadApplication_||display->video_!=video||
       sourceLoadApplication_->raceDisplayOwner50LA_.lock()!=display||
       !display->frontendOwnerCurrent50KG()||!display->canRetain()||!display->frontendCreated_)return false;
    // Native lexical pin only. No source add-ref or context/service operation.
    display->pin();out=video->packetZMaximum;display->unpin(); //Actual Video60 consumer.
    return true;
}
bool Owner::readSourceSunFlareViewport115990(std::array<std::int32_t,4>& out)const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceLoadApplication_||!sourceLoadRace_||
       sourceLoadApplication_->entities()!=worldApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_)return false;
    const auto& n=sourceLoadRace_->loadingCamera34_;
    if(!n.entered||!n.returned||!n.selected||!n.pinned||n.terminal||
       !n.suffixReturned||!n.arrayPublished||n.arrayWithdrawn||n.selectionWithdrawn||n.cameraDisposed||
       sourceLoadRace_->cameraNativeAdapter34_)return false;
    const auto display=sourceLoadRace_->loadDisplay_;
    const auto video=sourceLoadApplication_->videoOwner_;
    const auto camera=sourceLoadRace_->camera34_;
    if(!display||!video||!camera||display->application_!=sourceLoadApplication_||display->video_!=video||
       sourceLoadApplication_->raceDisplayOwner50LA_.lock()!=display||
       !display->canRetain()||!display->sourceCurrentCamera0Known_||display->sourceSelectedCamera0_!=camera||
       display->selectedGameplayCamera0_)return false;
    // This synchronous source reader has no callback or service. No Camera
    // pointer escapes in the packet; only four produced scalar words do.
    display->pin();const bool read=display->readSourceLoadingViewport115990(camera,out);display->unpin();
    return read;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldSunFlarePacketInvocationCurrent()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceTrackPC_!=0x2c5ae8||sourceSunFlarePC_!=0x2c5ae8||
       sourceSunFlarePhase_!=SourceSunFlarePhase::materialReturned||
       !preflightSourceWorldSunFlareNative()||!sourceSunFlareStack_||
       !sourceSunFlareStack_->textureKnown||!nativeSunFlares_||!nativeSunFlares_->sourceMode||
       nativeSunFlares_->sourceNativeClosed||!nativeSunFlares_->owner||
       !nativeSunFlares_->owner->sourceConstructorReturned||
       !nativeSunFlares_->owner->sourceSpritePublished||!nativeSunFlares_->owner->sprite64||
       !sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||
       *worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       !sourceWorldObjectConfig_->sourceSunFlareConfigCurrent()||
       sourceWorldObjectConfig_->sourceSunFlareNextSelection_!=2||
       !sourceWorldObjectConfig_->sourceSunFlareSelected_||
       *sourceWorldObjectConfig_->sourceSunFlareSelected_!=1||
       !sourceTrackMountComplete_||sourceTrackMountClosed_||
       sourceTrackMountApplication_!=sourceLoadApplication_||sourceTrackMountSource_!=source_||
       !sourceTrackMountDomain_||sourceTrackMountLease_.closed_||
       sourceTrackMountLease_.provider_!=sourceTrackMountDomain_->archiveProvider3E1A70())return false;
    // These owners already exist. No constructor/context/Video is synthesized.
    const auto& domain=sourceLoadApplication_->standardMaterialDomainOwner_;
    if(domain!=sourceTrackMountDomain_||!domain->prepared()||
       !sourceLoadApplication_->materialApplication)return false;
    const auto& table=domain->materials382B38();
    const auto& m=nativeSunFlares_->owner->sprite64->material;
    return table&&table->sourceNames17CAD4Current()&&
        table->screenApplicationMatches(*sourceLoadApplication_->materialApplication)&&
        m.sourceMode&&!m.sourceNativeClosed&&m.sourceTable==table&&
        m.sourceApplication==sourceLoadApplication_->materialApplication&&
        m.sourceParametersPublished&&m.sourceParameters08&&
        m.sourcePrimitivePublished&&m.sourcePrimitive04&&m.primitive04&&
        *m.sourcePrimitive04==m.primitive04.get()&&m.primitive04->configured&&
        m.primitive04->retained&&!m.primitive04->sourceReferenceReleased;
}
}

namespace original_main_entry50KG {
bool RaceDisplayLifetime50LA::sourceLoadingCameraPresence2C5CF0(
    const std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& camera)const noexcept {
    using Camera=rr::game::OriginalFrontendCameraCold50LA;
    return ownerThread()&&application_&&frontendOwnerCurrent50KG()&&frontendCreated_&&
        !retiring_&&sourceCurrentCamera0Known_&&camera&&sourceSelectedCamera0_==camera&&
        !sourceSelectedCamera0_.owner_before(camera)&&!camera.owner_before(sourceSelectedCamera0_)&&
        !selectedGameplayCamera0_&&camera->issuer_==application_&&
        camera->entities_==application_->entities()&&
        camera->phase_==Camera::Phase::viewportInitialized2361C0&&
        camera->nativeRegistrationIdentityCurrent()&&camera->entity_.reference00==1;
}
bool RaceDisplayLifetime50LA::readSourceLoadingCameraWords2C5CF0(
    const std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& camera,
    std::uint16_t offset,std::uint32_t count,std::uint32_t* out)const noexcept {
    using Camera=rr::game::OriginalFrontendCameraCold50LA;
    if(!out||!((count==16&&(offset==0x80||offset==0xc0||offset==0x140))||
              (count==3&&(offset==0x180||offset==0x190))))return false;
    if(!ownerThread()||!application_||!frontendOwnerCurrent50KG()||!frontendCreated_||
       retiring_||!sourceCurrentCamera0Known_||!camera||sourceSelectedCamera0_!=camera||
       sourceSelectedCamera0_.owner_before(camera)||camera.owner_before(sourceSelectedCamera0_)||
       selectedGameplayCamera0_||camera->phase_!=Camera::Phase::viewportInitialized2361C0||
       !camera->unpublishedAbortCurrent(application_,application_->entities(),true)||
       !camera->fields_.coreKnown)return false;
    const auto& f=camera->fields_;
    const std::uint32_t* words=offset==0x80?f.projection80.data():
        offset==0xc0?f.viewC0.data():offset==0x140?f.matrix140.data():
        offset==0x180?f.position180.data():f.look190.data();
    // Source copies each actual retained word in address order. No diagnostic
    // snapshot, numerical reconstruction or future Session fields substitute.
    for(std::uint32_t i=0;i<count;++i)out[i]=words[i];
    return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::readSourceSunFlareCameraWords2C5CF0(
    std::uint16_t offset,std::uint32_t count,std::uint32_t* out)const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceLoadApplication_||!sourceLoadRace_||
       sourceLoadApplication_->entities()!=worldApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_)return false;
    const auto& n=sourceLoadRace_->loadingCamera34_;
    if(!n.entered||!n.returned||!n.selected||!n.pinned||n.terminal||
       !n.suffixReturned||!n.arrayPublished||n.arrayWithdrawn||n.selectionWithdrawn||n.cameraDisposed||
       sourceLoadRace_->cameraNativeAdapter34_)return false;
    const auto display=sourceLoadRace_->loadDisplay_;
    const auto video=sourceLoadApplication_->videoOwner_;
    const auto camera=sourceLoadRace_->camera34_;
    if(!display||!video||!camera||display->application_!=sourceLoadApplication_||display->video_!=video||
       sourceLoadApplication_->raceDisplayOwner50LA_.lock()!=display||
       !display->canRetain()||!display->sourceCurrentCamera0Known_||display->sourceSelectedCamera0_!=camera||
       display->selectedGameplayCamera0_)return false;
    // This synchronous source reader has no callback or service. No Camera
    // pointer escapes in the packet; only the specifically requested produced scalar words do.
    display->pin();const bool read=display->readSourceLoadingCameraWords2C5CF0(camera,offset,count,out);display->unpin();
    return read;
}
bool Owner::sourceSunFlareSelectedCameraPresent2C5CF0()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceLoadApplication_||!sourceLoadRace_||
       sourceLoadApplication_->entities()!=worldApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_)return false;
    const auto& n=sourceLoadRace_->loadingCamera34_;
    if(!n.entered||!n.returned||!n.selected||!n.pinned||n.terminal||
       !n.suffixReturned||!n.arrayPublished||n.arrayWithdrawn||n.selectionWithdrawn||n.cameraDisposed||
       sourceLoadRace_->cameraNativeAdapter34_)return false;
    const auto display=sourceLoadRace_->loadDisplay_;
    const auto video=sourceLoadApplication_->videoOwner_;
    const auto camera=sourceLoadRace_->camera34_;
    if(!display||!video||!camera||display->application_!=sourceLoadApplication_||display->video_!=video||
       sourceLoadApplication_->raceDisplayOwner50LA_.lock()!=display||
       !display->canRetain()||!display->sourceCurrentCamera0Known_||display->sourceSelectedCamera0_!=camera||
       display->selectedGameplayCamera0_)return false;
    // This synchronous source reader has no callback or service. No Camera
    // pointer escapes in the packet; no matrix or vector word is read at this presence boundary.
    display->pin();const bool read=display->sourceLoadingCameraPresence2C5CF0(camera);display->unpin();
    return read;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldSunFlareRetrieveInvocationCurrent()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceTrackPC_!=0x2c5c3c||sourceSunFlarePC_!=0x2c5c3c||
       sourceSunFlarePhase_!=SourceSunFlarePhase::packetReturned||
       !preflightSourceWorldSunFlareNative()||!sourceSunFlareStack_||
       !sourceSunFlareStack_->textureKnown||!nativeSunFlares_||!nativeSunFlares_->sourceMode||
       nativeSunFlares_->sourceNativeClosed||!nativeSunFlares_->owner||
       !nativeSunFlares_->owner->sourceConstructorReturned||
       !nativeSunFlares_->owner->sourceSpritePublished||!nativeSunFlares_->owner->sprite64||
       !sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||
       *worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       !sourceWorldObjectConfig_->sourceSunFlareConfigCurrent()||
       sourceWorldObjectConfig_->sourceSunFlareNextSelection_!=2||
       !sourceWorldObjectConfig_->sourceSunFlareSelected_||
       *sourceWorldObjectConfig_->sourceSunFlareSelected_!=1||
       !sourceTrackMountComplete_||sourceTrackMountClosed_||
       sourceTrackMountApplication_!=sourceLoadApplication_||sourceTrackMountSource_!=source_||
       !sourceTrackMountDomain_||sourceTrackMountLease_.closed_||
       sourceTrackMountLease_.provider_!=sourceTrackMountDomain_->archiveProvider3E1A70())return false;
    // These owners already exist. No constructor/context/Video is synthesized.
    const auto& domain=sourceLoadApplication_->standardMaterialDomainOwner_;
    if(domain!=sourceTrackMountDomain_||!domain->prepared()||
       !sourceLoadApplication_->materialApplication)return false;
    const auto& table=domain->materials382B38();
    const auto& m=nativeSunFlares_->owner->sprite64->material;
    return table&&table->sourceNames17CAD4Current()&&
        table->screenApplicationMatches(*sourceLoadApplication_->materialApplication)&&
        m.sourceMode&&!m.sourceNativeClosed&&m.sourceTable==table&&
        m.sourceApplication==sourceLoadApplication_->materialApplication&&
        m.sourceParametersPublished&&m.sourceParameters08&&
        m.sourcePrimitivePublished&&m.sourcePrimitive04&&m.primitive04&&
        *m.sourcePrimitive04==m.primitive04.get()&&m.primitive04->configured&&
        m.primitive04->retained&&!m.primitive04->sourceReferenceReleased;
}
bool Owner::sourceWorldSunFlareSaturationInvocationCurrent()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceTrackPC_!=0x2720d0||sourceSunFlarePC_!=0x2720d0||
       sourceSunFlarePhase_!=SourceSunFlarePhase::depthReturned||
       !preflightSourceWorldSunFlareNative()||!sourceSunFlareStack_||
       !sourceSunFlareStack_->textureKnown||!sourceSunFlareStack_->saturation4A8||
       !worldZones34C_||*worldZones34C_!=10||
       !sourceSunFlareIndex_||*sourceSunFlareIndex_!=0||
       !sourceSunFlareLimit_||*sourceSunFlareLimit_!=1||
       !sourceSunFlareDepthFrame_||!sourceSunFlareDepthFrame_->returned||!nativeSunFlares_||!nativeSunFlares_->sourceMode||
       nativeSunFlares_->sourceNativeClosed||!nativeSunFlares_->owner||
       !nativeSunFlares_->owner->sourceConstructorReturned||
       !nativeSunFlares_->owner->sourceZonesStored||nativeSunFlares_->owner->zones30!=10||
       !nativeSunFlares_->owner->sourceDepthStored||!nativeSunFlares_->owner->depth40||
       !nativeSunFlares_->owner->sourceSpritePublished||!nativeSunFlares_->owner->sprite64||
       !sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||
       *worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       !sourceWorldObjectConfig_->sourceSunFlareConfigCurrent()||
       sourceWorldObjectConfig_->sourceSunFlareNextSelection_!=2||
       !sourceWorldObjectConfig_->sourceSunFlareSelected_||
       *sourceWorldObjectConfig_->sourceSunFlareSelected_!=1||
       !sourceTrackMountComplete_||sourceTrackMountClosed_||
       sourceTrackMountApplication_!=sourceLoadApplication_||sourceTrackMountSource_!=source_||
       !sourceTrackMountDomain_||sourceTrackMountLease_.closed_||
       sourceTrackMountLease_.provider_!=sourceTrackMountDomain_->archiveProvider3E1A70())return false;
    // These owners already exist. No constructor/context/Video is synthesized.
    const auto& domain=sourceLoadApplication_->standardMaterialDomainOwner_;
    if(domain!=sourceTrackMountDomain_||!domain->prepared()||
       !sourceLoadApplication_->materialApplication)return false;
    const auto& table=domain->materials382B38();
    const auto& m=nativeSunFlares_->owner->sprite64->material;
    return table&&table->sourceNames17CAD4Current()&&
        table->screenApplicationMatches(*sourceLoadApplication_->materialApplication)&&
        m.sourceMode&&!m.sourceNativeClosed&&m.sourceTable==table&&
        m.sourceApplication==sourceLoadApplication_->materialApplication&&
        m.sourceParametersPublished&&m.sourceParameters08&&
        m.sourcePrimitivePublished&&m.sourcePrimitive04&&m.primitive04&&
        *m.sourcePrimitive04==m.primitive04.get()&&m.primitive04->configured&&
        m.primitive04->retained&&!m.primitive04->sourceReferenceReleased;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldParticleDescriptorsInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTrackPC_==0x271d80&&sourceSunFlarePC_==0x271d80&&
        sourceSunFlarePhase_==SourceSunFlarePhase::saturationReturned&&
        preflightSourceWorldSunFlareNative()&&sourceSunFlareIndex_&&sourceSunFlareLimit_&&
        *sourceSunFlareIndex_==1&&*sourceSunFlareLimit_==1&&
        worldColdConstructed_&&worldApplication_&&!worldApplication_->attachmentLock423F58()&&
        worldEntity_.reference00==1&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&
        worldParser34_&&sourceWorldObjectConfig_&&sourceWorldObjectConfig_->source_&&
        *worldParser34_==sourceWorldObjectConfig_->source_.get()&&
        sourceWorldObjectConfig_->sourceSunFlareConfigCurrent()&&
        sourceWorldObjectConfig_->sourceSunFlareNextSelection_==2&&
        sourceWorldObjectConfig_->sourceSunFlareSelected_&&
        *sourceWorldObjectConfig_->sourceSunFlareSelected_==1&&
        sourceParticleList3D8_&&!*sourceParticleList3D8_&&!nativeParticleList3D8_&&
        sourceTrackMountComplete_&&!sourceTrackMountClosed_&&
        sourceTrackMountApplication_==sourceLoadApplication_&&sourceTrackMountSource_==source_&&
        sourceTrackMountDomain_&&!sourceTrackMountLease_.closed_&&
        sourceTrackMountLease_.provider_==sourceTrackMountDomain_->archiveProvider3E1A70();
}
}

namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldParticleCountNative()const noexcept {
    //All aggregate-owned source audio/Particle count lanes derive only after exact local
    //authority preflight. Neither local helper calls this combined floor.
    if(!preflightSourceWorldTopologyNative()||!preflightSourceWorldParticleDescriptorsNative()||!preflightSourceWorldAmbientNative()||!preflightSourceWorldSoulTreeSoundsNative()||!preflightSourceWorldMaterialSoundsNative())return false;
    std::size_t extra=0;bool hasSourceOwner=false;
    std::size_t sourceModels=0;
    if(!preflightSourceStandardModelsNative(sourceModels))return false;
    if(sourceModels){hasSourceOwner=true;extra=sourceModels;}
    std::size_t particleManagerCount=0;
    if(!sourceParticleStageCountedNative(particleManagerCount))return false;
    if(sourceParticleAllocationAttempted_){hasSourceOwner=true;
        if(particleManagerCount>std::numeric_limits<std::size_t>::max()-extra)return false;extra+=particleManagerCount;}
    if(sourceVisibilityEnvironment_.node.counted){hasSourceOwner=true;++extra;}
    if(sourceFirstLight_){hasSourceOwner=true;if(sourceFirstLightCounted_)++extra;}
    if(sourceSecondLight_){hasSourceOwner=true;if(sourceSecondLightCounted_)++extra;}

    if(nativeParticleList3D8_&&nativeParticleList3D8_->sourceMode){
        hasSourceOwner=true;if(nativeParticleList3D8_->counted)++extra;
        for(const auto& row:nativeParticleList3D8_->rows)if(row)for(const auto& emitter:row->sourceEmitters)
            if(emitter&&emitter->counted){if(extra==std::numeric_limits<std::size_t>::max())return false;++extra;}
    }
    if(nativeEmptyAmbient3B4_&&nativeEmptyAmbient3B4_->sourceMode){
        hasSourceOwner=true;if(nativeEmptyAmbient3B4_->sourceCounted){
            if(extra==std::numeric_limits<std::size_t>::max())return false;++extra;
        }
    }
    if(nativeSoulTreeSounds3B8_&&nativeSoulTreeSounds3B8_->sourceMode){
        hasSourceOwner=true;if(nativeSoulTreeSounds3B8_->counted){
            if(extra==std::numeric_limits<std::size_t>::max())return false;++extra;
        }
    }
    for(const auto& list:nativeMaterialSounds3BC_)if(list&&list->sourceMode){
        hasSourceOwner=true;if(list->counted){
            if(extra==std::numeric_limits<std::size_t>::max())return false;++extra;
        }
    }
    if(sourceTopologyPhase_!=SourceTopologyPhase::unreached&&sourceTopologyPhase_!=SourceTopologyPhase::nativeClosed){
        hasSourceOwner=true;std::size_t topology=sourceTopologyContainerCounted_?1u:0u,manager=0;
        if(sourceTopologyLightManager_&&!sourceTopologyLightManager_->sourceRaZonesCounted(*this,manager))return false;
        if(manager>std::numeric_limits<std::size_t>::max()-topology)return false;topology+=manager;
        std::size_t nodes=0;if(!sourceWorldSearchNodesCounted(nodes)||nodes>std::numeric_limits<std::size_t>::max()-topology)return false;topology+=nodes;
        std::size_t portals=0;if(!sourceWorldPortalGraphCounted(portals)||portals>std::numeric_limits<std::size_t>::max()-topology)return false;topology+=portals;
        if(topology>std::numeric_limits<std::size_t>::max()-extra)return false;extra+=topology;
    }
    return !hasSourceOwner||canRetireNativeWorldPrefix(nullptr,extra);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldBuoysInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTrackPC_==0x271d90&&sourceParticleDescriptorPC_==0x271d90&&
        sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::returned&&
        preflightSourceWorldParticleCountNative()&&nativeParticleList3D8_&&
        nativeParticleList3D8_->sourceMode&&nativeParticleList3D8_->sourceConstructorReturned&&
        nativeParticleList3D8_->counted&&nativeParticleList3D8_->count30==4&&
        sourceParticleDescriptorSuccessS1_&&*sourceParticleDescriptorSuccessS1_==4&&
        sourceParticleDescriptorRowS0_&&*sourceParticleDescriptorRowS0_==4&&
        sourceLoadRace_&&sourceLoadApplication_&&worldColdConstructed_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&
        sourceWorldObjectConfig_&&sourceWorldObjectConfig_->source_&&worldParser34_&&
        *worldParser34_==sourceWorldObjectConfig_->source_.get()&&
        sourceParticleDescriptorConfigArg_==sourceWorldObjectConfig_.get()&&
        sourceWorldObjectConfig_->sourceParticleDescriptorConfigCurrent()&&
        sourceWorldObjectConfig_->sourceParticleDescriptorNextSelection_==5&&
        sourceWorldObjectConfig_->sourceParticleDescriptorSelected_&&
        *sourceWorldObjectConfig_->sourceParticleDescriptorSelected_==4&&
        sourceTrackMountComplete_&&!sourceTrackMountClosed_&&
        sourceTrackMountApplication_==sourceLoadApplication_&&sourceTrackMountSource_==source_&&
        sourceTrackMountDomain_&&!sourceTrackMountLease_.closed_&&
        sourceTrackMountLease_.provider_==sourceTrackMountDomain_->archiveProvider3E1A70();
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldStaticModelParamsInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTrackPC_==0x271da0&&sourceBuoysPC_==0x271da0&&
        sourceBuoysPhase_==SourceBuoysPhase::returned&&preflightSourceWorldBuoysNative()&&
        nativeBuoyCount538_&&*nativeBuoyCount538_==0&&
        sourceStaticPhase_==SourceStaticPhase::returned&&sourceStaticOwner_&&
        sourceStaticOwner_->sourceAllDescriptorsReturned_&&sourceStaticZoneIndex_==10&&
        sourceStaticSucceededZones_==10&&sourceModelParentsPublished3C4_==10&&
        sourceModelOwner3C4_==sourceStaticOwner_.get()&&
        modelParents3C4Current(sourceStaticOwner_.get(),false)&&preflightSourceWorldStaticModelsNative()&&
        worldZones34C_&&*worldZones34C_==10&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&
        sourceWorldObjectConfig_&&sourceWorldObjectConfig_->source_&&worldParser34_&&
        *worldParser34_==sourceWorldObjectConfig_->source_.get()&&
        sourceWorldObjectConfig_->sourceBuoysMissingCurrent()&&
        sourceWorldObjectConfig_->sourceBuoyCountReturned_&&
        sourceTrackMountComplete_&&!sourceTrackMountClosed_&&
        sourceTrackMountApplication_==sourceLoadApplication_&&sourceTrackMountSource_==source_&&
        sourceTrackMountDomain_&&!sourceTrackMountLease_.closed_&&
        sourceTrackMountLease_.provider_==sourceTrackMountDomain_->archiveProvider3E1A70();
}
bool Owner::completeSourceWorldObjects271DB0(std::uint32_t& pc)noexcept {
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceTrackPC_!=0x271db0||sourceStaticModelParamPC_!=0x271db0||
       sourceStaticModelParamPhase_!=SourceStaticModelParamPhase::returned||
       !preflightSourceWorldStaticModelParamsNative()||sourceTobA4V0_||sourceTobA4Returned_)return false;
    //Actual outer return scalar. The caller ignores it; no readiness flag or
    //legacy source/native gate is changed by this private return history.
    pc=sourceTrackPC_=0x271db0;sourceTobA4V0_=1;
    pc=sourceTrackPC_=0x1aa0d4;sourceTobA4Returned_=true;
    return true; //Before next vE0 dispatch at1AA0DC/1AA0E0.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldAmbientInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTrackStep_==SourceTrackStep::audioReturned&&sourceTrackPC_==0x1aa114&&
        sourceAudioPhase_==SourceAudioPhase::returnedOne&&sourceAudioPC_==0x1aa0fc&&
        sourceAudioReturnValue_&&*sourceAudioReturnValue_==1&&sourceAudioConfig38Published_&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceWorldAudioConfig_&&sourceWorldAudioConfig_->source_&&
        sourceAudioParser38_&&*sourceAudioParser38_==sourceWorldAudioConfig_->source_.get()&&
        sourceTrackFile_&&sourceWorldAudioConfig_->file_==sourceTrackFile_&&
        sourceWorldAudioConfig_->sourceAudioConfigCurrent()&&
        sourceTrackFile_->current(*sourceWorldAudioConfig_,sourceWorldAudioConfig_->source_->file00)&&
        preflightSourceWorldParticleCountNative();
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldSoulTreeSoundsInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTrackStep_==SourceTrackStep::audioReturned&&sourceTrackPC_==0x2714d8&&
        sourceAmbientPhase_==SourceAmbientPhase::returnedZero&&sourceAmbientPC_==0x2714d8&&
        sourceAmbientDestructorReturned_&&sourceAmbientLoaderReturned_&&sourceAmbientGetterReturned_&&
        sourceAmbient3B4_&&!*sourceAmbient3B4_&&!nativeEmptyAmbient3B4_&&
        sourceAudioPhase_==SourceAudioPhase::returnedOne&&sourceAudioReturnValue_&&*sourceAudioReturnValue_==1&&
        sourceAudioConfig38Published_&&sourceWorldAudioConfig_&&sourceWorldAudioConfig_->source_&&
        sourceAmbientConfigArg_==sourceWorldAudioConfig_.get()&&sourceAudioParser38_&&
        *sourceAudioParser38_==sourceWorldAudioConfig_->source_.get()&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceTrackFile_&&
        sourceWorldAudioConfig_->file_==sourceTrackFile_&&sourceWorldAudioConfig_->sourceAmbientCountReturned_&&
        sourceWorldAudioConfig_->sourceAmbientSoundsConfigCurrent()&&
        sourceTrackFile_->current(*sourceWorldAudioConfig_,sourceWorldAudioConfig_->source_->file00)&&
        preflightSourceWorldParticleCountNative();
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldMaterialSoundsInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTrackStep_==SourceTrackStep::audioReturned&&sourceTrackPC_==0x271530&&
        sourceSoulTreeSoundPhase_==SourceSoulTreeSoundPhase::returned&&sourceSoulTreeSoundPC_==0x271530&&
        nativeSoulTreeSounds3B8_&&nativeSoulTreeSounds3B8_->sourceMode&&nativeSoulTreeSounds3B8_->sourceLoaded==42&&
        sourceSoulTreeSounds3B8_&&*sourceSoulTreeSounds3B8_==nativeSoulTreeSounds3B8_.get()&&
        !nativeMaterialAssignments271600_&&!nativeMaterialSounds3BC_[0]&&!nativeMaterialSounds3BC_[1]&&
        sourceAudioPhase_==SourceAudioPhase::returnedOne&&sourceAudioReturnValue_&&*sourceAudioReturnValue_==1&&
        sourceAudioConfig38Published_&&sourceWorldAudioConfig_&&sourceWorldAudioConfig_->source_&&
        sourceSoulTreeSoundConfigArg_==sourceWorldAudioConfig_.get()&&sourceAudioParser38_&&
        *sourceAudioParser38_==sourceWorldAudioConfig_->source_.get()&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceTrackFile_&&
        sourceWorldAudioConfig_->file_==sourceTrackFile_&&sourceWorldAudioConfig_->sourceSoulTreeSoundConfigCurrent()&&
        sourceWorldAudioConfig_->sourceSoulTreeSoundNextSelection_==43&&
        sourceWorldAudioConfig_->sourceSoulTreeSoundSelected_&&*sourceWorldAudioConfig_->sourceSoulTreeSoundSelected_==42&&
        sourceTrackFile_->current(*sourceWorldAudioConfig_,sourceWorldAudioConfig_->source_->file00)&&
        preflightSourceWorldParticleCountNative();
}
}

//Private lighting/World access isolation. Definitions live in the existing TU
//that already requires complete ra_pod_scene50LA::Owner. Each operation is the
//original direct member access/call, not cached data, validation or readiness.
namespace rr::game::original_gc_gameplay_lighting_candidate {
std::optional<original_world_environment50KG::Owner*>& Owner::podEnvironment3A4(::ra_pod_scene50LA::Owner& world)noexcept {return world.nativeEnvironment3A4_;}
const std::optional<original_world_environment50KG::Owner*>& Owner::podEnvironment3A4(const ::ra_pod_scene50LA::Owner& world)noexcept {return world.nativeEnvironment3A4_;}
const std::shared_ptr<OriginalApplicationEntities50LA>& Owner::podApplication(const ::ra_pod_scene50LA::Owner& world)noexcept {return world.worldApplication_;}
bool Owner::podColdConstructed(const ::ra_pod_scene50LA::Owner& world)noexcept {return world.worldColdConstructed_;}
bool Owner::podTopologyClosed(const ::ra_pod_scene50LA::Owner& world)noexcept {return world.topologyClosed_;}
const std::shared_ptr<Owner>& Owner::podLighting(const ::ra_pod_scene50LA::Owner& world)noexcept {return world.lighting_;}
bool Owner::podFlareIdentityCurrent(const ::ra_pod_scene50LA::Owner& world,const OriginalWreckComponent50LA* entity)noexcept {return world.nativeLocalFlareIdentityCurrent(entity);}
std::optional<std::size_t> Owner::podFlareZoneIndex(const ::ra_pod_scene50LA::Owner& world,const OriginalWreckComponent50LA* entity)noexcept {return world.nativeLocalFlareZoneIndex(entity);}
std::optional<std::size_t> Owner::podFlareAuthoredIndex(const ::ra_pod_scene50LA::Owner& world,const OriginalWreckComponent50LA* entity)noexcept {return world.nativeLocalFlareAuthoredIndex(entity);}
}

namespace ra_cold_aggregate50LA {
bool Owner::readSourceWorldRows1AE0C8(const original_main_entry50KG::Application& app,
    std::uint64_t generation,const void*& visibility,const void*& collision)const noexcept {
    // No call to retirement/count preflight here: its live-borrower rejection
    // remains intact and must not make an already issued reader self-invalid.
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !app.enabled||sourceLoadApplication_!=&app||!sourceLoadRace_||
       !worldApplication_||app.entities()!=worldApplication_||worldApplication_->attachmentLock423F58()||
       !worldColdConstructed_||!worldReset270550Complete_||!generation||
       worldEntity_.sessionGeneration!=generation||worldEntity_.reference00!=1||
       worldEntity_.kind!=g::OriginalWreckComponentKind50LA::entity141300||worldEntity_.objectKey!=key(&worldEntity_)||
       !sourceTopologyMode_||sourceTopologyWithdrawn_||topologyClosed_||
       !sourceTopologyContainer_||!sourceTopologyContainerCounted_||!sourceTopologyContainerEnrolled_||
       !sourceTopologyContainerPublished_||!sourceContainer4BC_||*sourceContainer4BC_!=sourceTopologyContainer_.get()||
       !preflightSourcePortalContainerNative(*sourceTopologyContainer_,true,true)||
       !sourceTopologyVisibilityPublished_||!sourceTopologyCollisionPublished_||
       !sourceVisibility4C0_||!sourceCollision4C4_||!sourceVisibilitySlots4C0_||!sourceCollisionSlots4C4_||
       *sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get()||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
       !sourceVisibilityPublished_||!sourceCollisionPublished_||sourceMethodNodesDetaching_)return false;
    const auto& v=sourceSearchNodes_[0][0];const auto& c=sourceSearchNodes_[1][0];
    if(!v.owner||!c.owner||v.withdrawn||c.withdrawn||!v.published||!c.published||
       !v.counted||!c.counted||!v.enrolled||!c.enrolled||!v.ordinalKnown||!c.ordinalKnown||
       v.constructorMask!=511||c.constructorMask!=511||
       (*sourceVisibility4C0_)[0]!=v.owner.get()||(*sourceCollision4C4_)[0]!=c.owner.get()||
       !v.owner->attached||!c.owner->attached||
       !preflightSourceSearchNodeNative(v,0,false)||!preflightSourceSearchNodeNative(c,0,true))return false;
    visibility=v.owner.get();collision=c.owner.get();return true;
    // Available nodes are NOT evidence that World load returned or1AA580 ran.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::readSourceWorldFields2107FC(const original_main_entry50KG::Application& app,
    std::uint64_t generation,const void*& terrain,const void*& water,
    const rr::game::OriginalRaceConstructor50LA*& race)const noexcept {
    const void *v=nullptr,*c=nullptr;
    if(!readSourceWorldRows1AE0C8(app,generation,v,c)||
       sourceLoadReset270550Phase_!=SourceLoadReset270550Phase::returned||
       !worldWater3FC_.has_value()||!worldWater400_.has_value())return false;
    // Actual2705CC/2705D0 known-field publications, read through at2107FC/210800.
    // No reinterpretation of native WaterDomain, no synthetic NULL from absence,
    // no assertion that either water loader or whole World load has returned.
    terrain=*worldWater3FC_;water=*worldWater400_;race=sourceLoadRace_;return true;
}
}
