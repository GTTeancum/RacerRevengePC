#include "original_camera_session50kx.h"
#include "original_race_world_join50la.h"
#include "ra_cold_aggregate50la.h"
#include <array>
#include <cstring>
#include <string>
#include <thread>

namespace ra_cold_aggregate50LA {
bool Owner::nativeTrackArchiveCurrent(const original_main_entry50KG::Application& app)const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    try {
    //These are the existing native resource owners, NOT receipts for the
    //source12CEA0/132A20/132E40 cache protocol or World slot30/270550 reset.
    if(nativeScbRevoked_||terminal_||closed_||!worldColdConstructed_||
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
        nativeScb34_->source=source_;nativeScb34_->provider=provider;
    }
    auto& reader=*nativeScb34_;
    if(reader.source!=source_||reader.provider!=provider)return false;
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
        pc=0x1a6d98;return false; //BEFORE actualRace30 read, never skip into next Sessionphase.
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
