#pragma once
#include "original_ui_main_entry50kg.h"
#include "track_session_assets50ig_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/original_standard_material_source50la.h"
#include <cstring>
#include "rc_cold_sources50la.h"
#include "tb_cold_sources50la.h"

namespace native_ta_particle_cold50LA {
// Actual native cold load for admitted TA/MA/SA/GA/MB/SC/TC callers. This consumes the
// already authenticated SCB and exact selected-track archive in the SAME
// Application material domain, before its caller performs material population.
// No recovery-time decoder execution, alternate domain or source fallback.
inline bool prepare(rr::game::OriginalTrackParticleCold50LA& owner,
    const track_session_assets50IG::SessionAssets& track,
    const rr::assets::World48F& actualWorld,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> actual382B38,
    std::uint64_t generation,rr::game::OriginalTrackParticleColdDiagnostic50LA& diagnostic) {
    using runtime_startup50fl::require;
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    const bool ta=track.world&&track_session_assets50IG::codeEquals(track.world->code,"TA");
    const bool ma=track.world&&track_session_assets50IG::codeEquals(track.world->code,"MA");
    const bool sa=track.world&&track_session_assets50IG::codeEquals(track.world->code,"SA");
    const bool ga=track.world&&track_session_assets50IG::codeEquals(track.world->code,"GA");
    const bool mb=track.world&&track_session_assets50IG::codeEquals(track.world->code,"MB");
    const bool sc=track.world&&track_session_assets50IG::codeEquals(track.world->code,"SC");
    const bool tc=track.world&&track_session_assets50IG::codeEquals(track.world->code,"TC");
    require(ta||ma||sa||ga||mb||sc||tc,"native particle cold load requires actual TA, MA, SA, GA, MB, SC or TC track");
    const char* archiveName=tc?"TRACKS/TC.RES":sc?"TRACKS/SC.RES":mb?"TRACKS/MB.RES":ta?"TRACKS/TA.RES":ma?"TRACKS/MA.RES":sa?"TRACKS/SA.RES":"TRACKS/GA.RES";
    const std::size_t archiveBytes=tc?3607868u:sc?2974353u:mb?4421779u:ta?3334562u:ma?3691592u:sa?3786696u:2834942u;
    const char* archiveSha=tc?"7b5486f4029dabe137919127c879faef92b8bd2fb9df5c0d1a1d122e92731490":sc?"8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d":mb?"6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca":ta?"b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8":
        ma?"2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228":
        sa?"34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a":
           "063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9";
    const char* pstSha=tc?"9fe4c6d8c9399d50cbfd4b82c4b420b3584ee7af97f5eae4316302c4bd342a9c":sc?"75a36310131d44eb1111c58bd372099b20f6b24cf9846746f5408055a2dcedc9":mb?"f5ea0c3c3de3084af5ffb8c46f71f26604b7ebc1a8f6ebc2c25cf670a906a6ba":ta?"08363cd9b2ebb557f2c8c673caa9084f2042721afbb112d5bbc8b5e08a4c49ba":
        ma?"d9506d37883d481d52f917e1d99d4bb57406c0f10191c5e233e91190d282909e":
        sa?"41e8f989e3c5b875be8dd85e38a6dfcb33372fad34eebed62e8134be55b26ff1":
           "05256d6b6cfcb5392cc38d4a2d5ce789ef2ea87fa10331d3597105be37b76b6a";
    require(track.archive&&track.world->container&&track.archiveSha256&&
        std::string(track.world->container)==archiveName&&track.archiveBytes==archiveBytes&&
        std::string(track.archiveSha256)==archiveSha,"Track particle archive profile identity rejected");
    const auto domain=application.standardMaterialDomain();
    require(domain&&domain->prepared()&&actual382B38&&domain->materials382B38()==actual382B38&&
        domain->resolver3E1A70()&&!actual382B38->populationTerminal()&&!actual382B38->demandTerminal()&&
        !actual382B38->coldCollectorCurrent(domain->coldCollector14D440()),
        "Track particle requires the actual cold Application domain before collector publication");
    const auto provider=domain->archiveProvider3E1A70();
    require(provider&&provider->sealed(),"Track particle requires its complete sealed native archive provider");
    const auto providerRevision=provider->revision();
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
    require(*particleName=="demoprt.tga"&&member=="particle\\demoprt.prt",
        "Actual SCB-selected particle member identity rejected");
    // Same bounded fresh-cold guard as the real RA particle stage: never
    // adopt another manager's retained/populated canonical demoprt material.
    std::int32_t particleRow=-1;rr::assets::OriginalStandardMaterialRowView50LA material;
    require(actual382B38->find298C80("particle\\demoprt",particleRow,diagnostic.material)&&
        actual382B38->row(particleRow,material)&&material.references08&&
        *material.references08==0&&!material.object04,
        "Track particle requires fresh canonical demoprt reference ownership");
    const std::string psmMember=member.substr(0,member.size()-4)+".psm";
    const std::string pstMember=member.substr(0,member.size()-4)+".pst";
    const auto bytes=runtime_startup50fl::readBytes(track.archive);
    require(bytes.size()==archiveBytes&&pod_materials50ed_fixture::detail::sha256(bytes)==archiveSha,
        "Track particle archive identity rejected");
    rr::assets::ResArchive48C archive;rr::assets::DecodeError48C error;
    rr::assets::DecodeLimits48C limits;limits.resource_file_bytes=archiveBytes;
    limits.resource_decoded_bytes=64u*1024u*1024u;limits.resource_entries=4096;
    std::vector<std::uint8_t> prt,psm,pst;
    require(rr::assets::decodeResource48C(bytes,archive,error,nullptr,limits)&&
        rr::assets::extractResMember48C(archive,member,prt,error)&&
        rr::assets::extractResMember48C(archive,psmMember,psm,error)&&
        rr::assets::extractResMember48C(archive,pstMember,pst,error),
        "SCB-selected original track PRT/PSM/PST closure unavailable");
    require(prt.size()==(mb?10310u:9210u)&&pod_materials50ed_fixture::detail::sha256(prt)==
        (mb?"98a7d3c28ed61d1e8c9fb90a9dd433119e5e2574f648308271d4a2f4a0bd7a2a":
            "d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f"),
        "Original selected-track PRT identity rejected");
    require(psm.size()==5022u&&pod_materials50ed_fixture::detail::sha256(psm)==
        "ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1",
        "Original selected-track particle PSM identity rejected");
    require(!pst.empty()&&pod_materials50ed_fixture::detail::sha256(pst)==pstSha,
        "Original selected-track particle PST identity rejected");
    if(ga)require(archive.entries.size()==970u&&archive.decoded.size()==26099712u&&pst.size()==132124u,
        "GA original particle archive/member extent rejected");
    if(mb)require(archive.entries.size()==487u&&archive.decoded.size()==16392192u&&pst.size()==132124u,
        "MB original particle archive/member extent rejected");
    if(sc)require(archive.entries.size()==984u&&archive.decoded.size()==26714112u&&pst.size()==132124u,
        "SC original particle archive/member extent rejected");
    if(tc)require(archive.entries.size()==509u&&archive.decoded.size()==16048128u&&pst.size()==132124u,
        "TC original particle archive/member extent rejected");
    const auto matches=[&](const std::string& requested,const std::vector<std::uint8_t>& original){
        Provider::MemberView selected;Provider::MountView mounted;
        return provider->lookup132820(requested,selected)==Provider::Lookup::present&&
            provider->mount(selected.mount,mounted)&&mounted.archiveName&&mounted.archiveSha256&&
            *mounted.archiveName==archiveName&&*mounted.archiveSha256==archiveSha&&
            mounted.archiveBytes==archiveBytes&&mounted.memberCount==archive.entries.size()&&
            selected.member&&*selected.member==requested&&selected.data&&
            selected.revision==providerRevision&&selected.size==original.size()&&
            !std::memcmp(selected.data,original.data(),original.size());
    };
    require(matches(member,prt)&&matches(psmMember,psm)&&matches(pstMember,pst)&&
        provider->sealed()&&provider->revision()==providerRevision,
        "Track particle closure differs from the same selected mounted archive");
    auto materialSource=std::make_shared<rr::assets::OriginalStandardMaterialSource50LA>();
    require(materialSource->prepare(psmMember,psm,diagnostic.material.document),
        "Original selected-track particle PSM parsing rejected");
    // Same domain owns the exact PSM/PST beyond these temporary source vectors.
    // The actual particle owner acquires its canonical reference before the
    // caller's14D370/29E340 population. Failure retains real partial effects.
    require(domain->appendOriginalSources({materialSource},{{pstMember,pst}},diagnostic.material),
        "Original selected-track particle canonical source append rejected");
    const bool prepared=application.withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){
        return owner.prepareNativeTrack(actualDirectory,*particleName,prt,application.entities(),
            std::move(actual382B38),generation,diagnostic);
    });
    if(!prepared)return false;
    require(provider->sealed()&&provider->revision()==providerRevision&&owner.material150()&&
        owner.material150()->table382B38()==domain->materials382B38().get(),
        "Track particle construction changed its canonical source ownership");
    return true;
}
// RC compact-source path; all legacy cold paths above remain unchanged.
inline bool prepareRc(rr::game::OriginalTrackParticleCold50LA& owner,
    const track_session_assets50IG::SessionAssets& track,
    std::shared_ptr<const rc_cold_sources50LA::Owner> source,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> actual382B38,
    std::uint64_t generation,rr::game::OriginalTrackParticleColdDiagnostic50LA& diagnostic) {
    using runtime_startup50fl::require;
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    require(source&&source->ready()&&track.world&&
        track_session_assets50IG::codeEquals(track.world->code,"RC"),
        "RC particle requires the actual authenticated compact source");
    const auto& actualWorld=source->world();
    const char* archiveName="TRACKS/RC.RES";
    const std::size_t archiveBytes=rc_cold_sources50LA::archiveBytes;
    const char* archiveSha=rc_cold_sources50LA::archiveSha;
    const char* pstSha="730d836d0decb8c5460b1318ec90ef82d88f342fbd92fc81580c190b5c09f019";
    require(track.archive&&track.world->container&&track.archiveSha256&&
        std::string(track.world->container)==archiveName&&track.archiveBytes==archiveBytes&&
        std::string(track.archiveSha256)==archiveSha,"RC particle archive profile identity rejected");
    const auto domain=application.standardMaterialDomain();
    require(domain&&domain->prepared()&&actual382B38&&domain->materials382B38()==actual382B38&&
        domain->resolver3E1A70()&&!actual382B38->populationTerminal()&&!actual382B38->demandTerminal()&&
        !actual382B38->coldCollectorCurrent(domain->coldCollector14D440()),
        "Track particle requires the actual cold Application domain before collector publication");
    const auto provider=domain->archiveProvider3E1A70();
    require(provider&&provider->sealed(),"Track particle requires its complete sealed native archive provider");
    std::shared_ptr<const rc_cold_sources50LA::Owner> mountedSource;
    Provider::Diagnostic sourceDiagnostic;
    require(rc_cold_sources50LA::Owner::reuseCanonicalSource(*provider,mountedSource,sourceDiagnostic)&&
        mountedSource.get()==source.get(),"RC particle canonical mount must retain this exact source");
    const auto providerRevision=provider->revision();
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
    require(*particleName=="demoprt.tga"&&member=="particle\\demoprt.prt",
        "Actual SCB-selected particle member identity rejected");
    // Same bounded fresh-cold guard as the real RA particle stage: never
    // adopt another manager's retained/populated canonical demoprt material.
    std::int32_t particleRow=-1;rr::assets::OriginalStandardMaterialRowView50LA material;
    require(actual382B38->find298C80("particle\\demoprt",particleRow,diagnostic.material)&&
        actual382B38->row(particleRow,material)&&material.references08&&
        *material.references08==0&&!material.object04,
        "Track particle requires fresh canonical demoprt reference ownership");
    const std::string psmMember=member.substr(0,member.size()-4)+".psm";
    const std::string pstMember=member.substr(0,member.size()-4)+".pst";
    rr::assets::DecodeError48C error;
    std::vector<std::uint8_t> prt,psm,pst;
    require(source->extractMember(member,prt,error,7671u)&&
        source->extractMember(psmMember,psm,error,5022u)&&
        source->extractMember(pstMember,pst,error,132124u),
        "RC selected original particle closure unavailable");
    require(prt.size()==7671u&&pod_materials50ed_fixture::detail::sha256(prt)==
        "9ee08f9801aeae21750eb106a2892f1399a6ab13f41f754d1d66b698c1428958",
        "RC original particle PRT identity rejected");
    require(psm.size()==5022u&&pod_materials50ed_fixture::detail::sha256(psm)==
        "ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1",
        "Original selected-track particle PSM identity rejected");
    require(pst.size()==132124u&&pod_materials50ed_fixture::detail::sha256(pst)==pstSha,
        "RC original particle PST identity rejected");
    const auto matches=[&](const std::string& requested,const std::vector<std::uint8_t>& original){
        Provider::MemberView selected;Provider::MountView mounted;
        return provider->lookup132820(requested,selected)==Provider::Lookup::present&&
            provider->mount(selected.mount,mounted)&&mounted.archiveName&&mounted.archiveSha256&&
            *mounted.archiveName==archiveName&&*mounted.archiveSha256==archiveSha&&
            mounted.archiveBytes==archiveBytes&&mounted.memberCount==rc_cold_sources50LA::archiveEntries&&
            selected.member&&*selected.member==requested&&selected.data&&
            selected.revision==providerRevision&&selected.size==original.size()&&
            !std::memcmp(selected.data,original.data(),original.size());
    };
    require(matches(member,prt)&&matches(psmMember,psm)&&matches(pstMember,pst)&&
        provider->sealed()&&provider->revision()==providerRevision,
        "Track particle closure differs from the same selected mounted archive");
    auto materialSource=std::make_shared<rr::assets::OriginalStandardMaterialSource50LA>();
    require(materialSource->prepare(psmMember,psm,diagnostic.material.document),
        "Original selected-track particle PSM parsing rejected");
    // Same domain owns the exact PSM/PST beyond these temporary source vectors.
    // The actual particle owner acquires its canonical reference before the
    // caller's14D370/29E340 population. Failure retains real partial effects.
    require(domain->appendOriginalSources({materialSource},{{pstMember,pst}},diagnostic.material),
        "Original selected-track particle canonical source append rejected");
    const bool prepared=application.withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){
        return owner.prepareNativeTrack(actualDirectory,*particleName,prt,application.entities(),
            std::move(actual382B38),generation,diagnostic);
    });
    if(!prepared)return false;
    require(provider->sealed()&&provider->revision()==providerRevision&&owner.material150()&&
        owner.material150()->table382B38()==domain->materials382B38().get(),
        "Track particle construction changed its canonical source ownership");
    return true;
}
// TB compact-source path; all legacy cold paths above remain unchanged.
inline bool prepareTb(rr::game::OriginalTrackParticleCold50LA& owner,
    const track_session_assets50IG::SessionAssets& track,
    std::shared_ptr<const tb_cold_sources50LA::Owner> source,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> actual382B38,
    std::uint64_t generation,rr::game::OriginalTrackParticleColdDiagnostic50LA& diagnostic) {
    using runtime_startup50fl::require;
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    require(source&&source->ready()&&track.world&&
        track_session_assets50IG::codeEquals(track.world->code,"TB"),
        "TB particle requires the actual authenticated compact source");
    const auto& actualWorld=source->world();
    const char* archiveName="TRACKS/TB.RES";
    const std::size_t archiveBytes=tb_cold_sources50LA::archiveBytes;
    const char* archiveSha=tb_cold_sources50LA::archiveSha;
    const char* pstSha="9266d01d81b65f5be72e15380eaf251d34af713dfa7875cadffaa5eb81c1183a";
    require(track.archive&&track.world->container&&track.archiveSha256&&
        std::string(track.world->container)==archiveName&&track.archiveBytes==archiveBytes&&
        std::string(track.archiveSha256)==archiveSha,"TB particle archive profile identity rejected");
    const auto domain=application.standardMaterialDomain();
    require(domain&&domain->prepared()&&actual382B38&&domain->materials382B38()==actual382B38&&
        domain->resolver3E1A70()&&!actual382B38->populationTerminal()&&!actual382B38->demandTerminal()&&
        !actual382B38->coldCollectorCurrent(domain->coldCollector14D440()),
        "Track particle requires the actual cold Application domain before collector publication");
    const auto provider=domain->archiveProvider3E1A70();
    require(provider&&provider->sealed(),"Track particle requires its complete sealed native archive provider");
    std::shared_ptr<const tb_cold_sources50LA::Owner> mountedSource;
    Provider::Diagnostic sourceDiagnostic;
    require(tb_cold_sources50LA::Owner::reuseCanonicalSource(*provider,mountedSource,sourceDiagnostic)&&
        mountedSource.get()==source.get(),"TB particle canonical mount must retain this exact source");
    const auto providerRevision=provider->revision();
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
    require(*particleName=="demoprt.tga"&&member=="particle\\demoprt.prt",
        "Actual SCB-selected particle member identity rejected");
    // Same bounded fresh-cold guard as the real RA particle stage: never
    // adopt another manager's retained/populated canonical demoprt material.
    std::int32_t particleRow=-1;rr::assets::OriginalStandardMaterialRowView50LA material;
    require(actual382B38->find298C80("particle\\demoprt",particleRow,diagnostic.material)&&
        actual382B38->row(particleRow,material)&&material.references08&&
        *material.references08==0&&!material.object04,
        "Track particle requires fresh canonical demoprt reference ownership");
    const std::string psmMember=member.substr(0,member.size()-4)+".psm";
    const std::string pstMember=member.substr(0,member.size()-4)+".pst";
    rr::assets::DecodeError48C error;
    std::vector<std::uint8_t> prt,psm,pst;
    require(source->extractMember(member,prt,error,9210u)&&
        source->extractMember(psmMember,psm,error,5022u)&&
        source->extractMember(pstMember,pst,error,132124u),
        "TB selected original particle closure unavailable");
    require(prt.size()==9210u&&pod_materials50ed_fixture::detail::sha256(prt)==
        "d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f",
        "TB original particle PRT identity rejected");
    require(psm.size()==5022u&&pod_materials50ed_fixture::detail::sha256(psm)==
        "ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1",
        "Original selected-track particle PSM identity rejected");
    require(pst.size()==132124u&&pod_materials50ed_fixture::detail::sha256(pst)==pstSha,
        "TB original particle PST identity rejected");
    const auto matches=[&](const std::string& requested,const std::vector<std::uint8_t>& original){
        Provider::MemberView selected;Provider::MountView mounted;
        return provider->lookup132820(requested,selected)==Provider::Lookup::present&&
            provider->mount(selected.mount,mounted)&&mounted.archiveName&&mounted.archiveSha256&&
            *mounted.archiveName==archiveName&&*mounted.archiveSha256==archiveSha&&
            mounted.archiveBytes==archiveBytes&&mounted.memberCount==tb_cold_sources50LA::archiveEntries&&
            selected.member&&*selected.member==requested&&selected.data&&
            selected.revision==providerRevision&&selected.size==original.size()&&
            !std::memcmp(selected.data,original.data(),original.size());
    };
    require(matches(member,prt)&&matches(psmMember,psm)&&matches(pstMember,pst)&&
        provider->sealed()&&provider->revision()==providerRevision,
        "Track particle closure differs from the same selected mounted archive");
    auto materialSource=std::make_shared<rr::assets::OriginalStandardMaterialSource50LA>();
    require(materialSource->prepare(psmMember,psm,diagnostic.material.document),
        "Original selected-track particle PSM parsing rejected");
    // Same domain owns the exact PSM/PST beyond these temporary source vectors.
    // The actual particle owner acquires its canonical reference before the
    // caller's14D370/29E340 population. Failure retains real partial effects.
    require(domain->appendOriginalSources({materialSource},{{pstMember,pst}},diagnostic.material),
        "Original selected-track particle canonical source append rejected");
    const bool prepared=application.withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){
        return owner.prepareNativeTrack(actualDirectory,*particleName,prt,application.entities(),
            std::move(actual382B38),generation,diagnostic);
    });
    if(!prepared)return false;
    require(provider->sealed()&&provider->revision()==providerRevision&&owner.material150()&&
        owner.material150()->table382B38()==domain->materials382B38().get(),
        "Track particle construction changed its canonical source ownership");
    return true;
}
} // namespace native_ta_particle_cold50LA
