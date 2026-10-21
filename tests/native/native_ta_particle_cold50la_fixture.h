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
#include "gb_cold_sources50la.h"

namespace native_ta_particle_cold50LA {
// Shared native cold load for an authenticated selected-track capability. It consumes the
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
    // Source selection and availability are separate from this shared cold
    // capability. Authenticate the descriptor against the retained catalog;
    // caller-supplied hashes alone must never nominate another archive.
    const char* selectionError=nullptr;
    const auto* canonical=track_session_assets50IG::select(
        track.world?track.world->code:nullptr,selectionError);
    require(canonical&&track.world==canonical->world&&track.archive&&
        track.archiveSha256&&canonical->archiveSha256&&
        track.archiveBytes==canonical->archiveBytes&&
        std::string(track.archiveSha256)==canonical->archiveSha256,
        "Track particle canonical selected archive descriptor rejected");
    const char* archiveName=canonical->world->container;
    const auto archiveBytes=canonical->archiveBytes;
    const char* archiveSha=canonical->archiveSha256;
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
    const auto selectedParticleName=[&](const rr::assets::World48F& world){
        const std::string* name=nullptr;
        for(const auto& block:world.parameters.blocks){
            if(!sameName(block.name,"Environment"))continue;
            for(const auto& field:block.fields){
                if(!sameName(field.name,"ParticleTextureMapName"))continue;
                require(!name,"duplicate SCB ParticleTextureMapName");name=&field.value;
            }
        }
        return name;
    };
    const auto* particleName=selectedParticleName(actualWorld);
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
    // Verify the selected field against decoded authenticated SCB bytes,
    // never silently substitute a conflicting mutable caller world.
    std::vector<std::uint8_t> scb;rr::assets::World48F sourceWorld;
    require(rr::assets::extractResMember48C(archive,canonical->world->worldMember,scb,error)&&
        rr::assets::decodeWorld48F(scb,sourceWorld,error)&&
        actualWorld.parameters.original_bytes==scb,
        "Track particle world differs from the selected original SCB");
    const auto* originalParticleName=selectedParticleName(sourceWorld);
    require(originalParticleName&&*originalParticleName==*particleName,
        "Track particle parsed selection differs from authenticated SCB");
    // These are the two already-supported original atlas layouts, not
    // course identities. The numerical constructor retains its own bounds.
    const auto prtSha=pod_materials50ed_fixture::detail::sha256(prt);
    require((prt.size()==9210u&&prtSha==
            "d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f")||
        (prt.size()==10310u&&prtSha==
            "98a7d3c28ed61d1e8c9fb90a9dd433119e5e2574f648308271d4a2f4a0bd7a2a"),
        "Original selected-track PRT supported layout rejected");
    require(psm.size()==5022u&&pod_materials50ed_fixture::detail::sha256(psm)==
        "ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1",
        "Original selected-track particle PSM identity rejected");
    // PST pixels belong to this exact authenticated whole archive. Preserve
    // them byte-for-byte in the same mounted provider and canonical domain;
    // no another-course image, exported pixels or independent lookup exists.
    require(!pst.empty(),"Original selected-track particle PST missing");
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
// GB compact-source path; all legacy cold paths above remain unchanged.
inline bool prepareGb(rr::game::OriginalTrackParticleCold50LA& owner,
    const track_session_assets50IG::SessionAssets& track,
    std::shared_ptr<const gb_cold_sources50LA::Owner> source,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> actual382B38,
    std::uint64_t generation,rr::game::OriginalTrackParticleColdDiagnostic50LA& diagnostic) {
    using runtime_startup50fl::require;
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    require(source&&source->ready()&&track.world&&
        track_session_assets50IG::codeEquals(track.world->code,"GB"),
        "GB particle requires the actual authenticated compact source");
    const auto& actualWorld=source->world();
    const char* archiveName="TRACKS/GB.RES";
    const std::size_t archiveBytes=gb_cold_sources50LA::archiveBytes;
    const char* archiveSha=gb_cold_sources50LA::archiveSha;
    const char* pstSha="e00d1321525795ed19676514f733ea3b4027925f2b1367afd45b189c4bee2642";
    require(track.archive&&track.world->container&&track.archiveSha256&&
        std::string(track.world->container)==archiveName&&track.archiveBytes==archiveBytes&&
        std::string(track.archiveSha256)==archiveSha,"GB particle archive profile identity rejected");
    const auto domain=application.standardMaterialDomain();
    require(domain&&domain->prepared()&&actual382B38&&domain->materials382B38()==actual382B38&&
        domain->resolver3E1A70()&&!actual382B38->populationTerminal()&&!actual382B38->demandTerminal()&&
        !actual382B38->coldCollectorCurrent(domain->coldCollector14D440()),
        "Track particle requires the actual cold Application domain before collector publication");
    const auto provider=domain->archiveProvider3E1A70();
    require(provider&&provider->sealed(),"Track particle requires its complete sealed native archive provider");
    std::shared_ptr<const gb_cold_sources50LA::Owner> mountedSource;
    Provider::Diagnostic sourceDiagnostic;
    require(gb_cold_sources50LA::Owner::reuseCanonicalSource(*provider,mountedSource,sourceDiagnostic)&&
        mountedSource.get()==source.get(),"GB particle canonical mount must retain this exact source");
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
    require(source->extractMember(member,prt,error,9385u)&&
        source->extractMember(psmMember,psm,error,5022u)&&
        source->extractMember(pstMember,pst,error,132124u),
        "GB selected original particle closure unavailable");
    require(prt.size()==9385u&&pod_materials50ed_fixture::detail::sha256(prt)==
        "fc95eee7487a6ae0c00e50228cf3667f9978f74770fe913fe95310a35c59f15c",
        "GB original particle PRT identity rejected");
    require(psm.size()==5022u&&pod_materials50ed_fixture::detail::sha256(psm)==
        "ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1",
        "Original selected-track particle PSM identity rejected");
    require(pst.size()==132124u&&pod_materials50ed_fixture::detail::sha256(pst)==pstSha,
        "GB original particle PST identity rejected");
    const auto matches=[&](const std::string& requested,const std::vector<std::uint8_t>& original){
        Provider::MemberView selected;Provider::MountView mounted;
        return provider->lookup132820(requested,selected)==Provider::Lookup::present&&
            provider->mount(selected.mount,mounted)&&mounted.archiveName&&mounted.archiveSha256&&
            *mounted.archiveName==archiveName&&*mounted.archiveSha256==archiveSha&&
            mounted.archiveBytes==archiveBytes&&mounted.memberCount==gb_cold_sources50LA::archiveEntries&&
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
