#pragma once
#include "original_ui_main_entry50kg.h"
#include "track_session_assets50ig_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/assets/import_world48f.h"

namespace native_ta_particle_cold50LA {
// Actual native cold load for admitted TA/SA callers. This consumes the
// already authenticated SCB and exact selected-track archive;
// no recovery-time decoder, fabricated null4A0, or GC authoring substitution.
inline bool prepare(rr::game::OriginalTrackParticleCold50LA& owner,
    const track_session_assets50IG::SessionAssets& track,
    const rr::assets::World48F& actualWorld,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> actual382B38,
    std::uint64_t generation,rr::game::OriginalTrackParticleColdDiagnostic50LA& diagnostic) {
    using runtime_startup50fl::require;
    const bool ta=track.world&&track_session_assets50IG::codeEquals(track.world->code,"TA");
    const bool sa=track.world&&track_session_assets50IG::codeEquals(track.world->code,"SA");
    require(ta||sa,"native particle cold load requires actual TA or SA track");
    if(sa)require(track.archiveBytes==3786696u&&track.archiveSha256&&
        std::string(track.archiveSha256)=="34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a",
        "SA particle archive profile identity rejected");
    const auto sameName=[](const std::string& a,const char* b){
        std::size_t i=0;
        for(;i<a.size()&&b[i];++i){const auto lower=[](char c){return c>='A'&&c<='Z'?char(c+('a'-'A')):c;};
            if(lower(a[i])!=lower(b[i]))return false;}
        return i==a.size()&&!b[i];
    };
    const std::string* particleName=nullptr;
    for(const auto& block:actualWorld.parameters.blocks){
        if(!sameName(block.name,"Environment"))continue;
        for(const auto& field:block.fields){
            if(!sameName(field.name,"ParticleTextureMapName"))continue;
            require(!particleName,"duplicate SCB ParticleTextureMapName");particleName=&field.value;
        }
    }
    require(particleName&&!particleName->empty()&&particleName->size()<128&&
        particleName->find_first_of("/\\")==std::string::npos,
        "Original track particle atlas name is unavailable");
    //1617A0 uses36B530+the original SCB name and replaces its last extension.
    //It does not choose a material from the PRT's separate TextureMap field.
    std::string member=application.particleDirectory36B530()+*particleName;
    const auto dot=member.find_last_of('.');
    if(dot!=std::string::npos)member.resize(dot);
    member+=".prt";
    // Retained SA SCB selects this exact atlas through the actual Application
    // directory. Do not substitute the PRT TextureMap or another track's data.
    if(sa)require(*particleName=="demoprt.tga"&&member=="particle\\demoprt.prt",
        "SA SCB-selected particle member identity rejected");
    const auto bytes=runtime_startup50fl::readBytes(track.archive);
    require(bytes.size()==track.archiveBytes&&
        pod_materials50ed_fixture::detail::sha256(bytes)==track.archiveSha256,
        "Track particle archive identity rejected");
    rr::assets::ResArchive48C archive;rr::assets::DecodeError48C error;
    rr::assets::DecodeLimits48C limits;limits.resource_file_bytes=track.archiveBytes;
    limits.resource_decoded_bytes=64u*1024u*1024u;limits.resource_entries=4096;
    std::vector<std::uint8_t> prt;
    require(rr::assets::decodeResource48C(bytes,archive,error,nullptr,limits)&&
        rr::assets::extractResMember48C(archive,member,prt,error),
        "SCB-selected original track PRT member unavailable");
    // Full48F retained SA member: offset909312,9210 bytes. The native atlas
    // parser consumes these unchanged bytes; no offline particle cache is needed.
    if(sa)require(prt.size()==9210u&&pod_materials50ed_fixture::detail::sha256(prt)==
        "d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f",
        "SA original PRT identity rejected");
    return application.withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){
        return owner.prepareNativeTrack(actualDirectory,*particleName,prt,application.entities(),
            std::move(actual382B38),generation,diagnostic);
    });
}
} // namespace native_ta_particle_cold50LA
