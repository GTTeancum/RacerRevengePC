#pragma once
#include "native_ta_particle_cold50la_fixture.h"
#include "mc_cold_sources50la.h"
namespace native_ta_particle_cold50LA {
inline bool prepareMc(rr::game::OriginalTrackParticleCold50LA& owner,
    const track_session_assets50IG::SessionAssets& track,
    std::shared_ptr<const mc_cold_sources50LA::Owner> source,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> actual382B38,
    std::uint64_t generation,rr::game::OriginalTrackParticleColdDiagnostic50LA& diagnostic) {
    using runtime_startup50fl::require;
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    require(source&&source->ready()&&track.world&&
        track_session_assets50IG::codeEquals(track.world->code,"MC"),
        "MC particle requires the actual authenticated compact source");
    const auto& actualWorld=source->world();
    const char* archiveName="TRACKS/MC.RES";
    const std::size_t archiveBytes=mc_cold_sources50LA::archiveBytes;
    const char* archiveSha=mc_cold_sources50LA::archiveSha;
    const char* pstSha="027296fc1e953a6a936482bf1f51a6ad91e653bf5c322225e73396a259ed2361";
    require(track.archive&&track.world->container&&track.archiveSha256&&
        std::string(track.world->container)==archiveName&&track.archiveBytes==archiveBytes&&
        std::string(track.archiveSha256)==archiveSha,"MC particle archive profile identity rejected");
    const auto domain=application.standardMaterialDomain();
    require(domain&&domain->prepared()&&actual382B38&&domain->materials382B38()==actual382B38&&
        domain->resolver3E1A70()&&!actual382B38->populationTerminal()&&!actual382B38->demandTerminal()&&
        !actual382B38->coldCollectorCurrent(domain->coldCollector14D440()),
        "Track particle requires the actual cold Application domain before collector publication");
    const auto provider=domain->archiveProvider3E1A70();
    require(provider&&provider->sealed(),"Track particle requires its complete sealed native archive provider");
    std::shared_ptr<const mc_cold_sources50LA::Owner> mountedSource;
    Provider::Diagnostic sourceDiagnostic;
    require(mc_cold_sources50LA::Owner::reuseCanonicalSource(*provider,mountedSource,sourceDiagnostic)&&
        mountedSource.get()==source.get(),"MC particle canonical mount must retain this exact source");
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
    require(source->extractMember(member,prt,error,10770u)&&
        source->extractMember(psmMember,psm,error,5022u)&&
        source->extractMember(pstMember,pst,error,132124u),
        "MC selected original particle closure unavailable");
    require(prt.size()==10770u&&pod_materials50ed_fixture::detail::sha256(prt)==
        "a92a82d85adea560886fc0e6911baede2f11e9448b9ac4365bf7037e9e809020",
        "MC original particle PRT identity rejected");
    require(psm.size()==5022u&&pod_materials50ed_fixture::detail::sha256(psm)==
        "ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1",
        "Original selected-track particle PSM identity rejected");
    require(pst.size()==132124u&&pod_materials50ed_fixture::detail::sha256(pst)==pstSha,
        "MC original particle PST identity rejected");
    const auto matches=[&](const std::string& requested,const std::vector<std::uint8_t>& original){
        Provider::MemberView selected;Provider::MountView mounted;
        return provider->lookup132820(requested,selected)==Provider::Lookup::present&&
            provider->mount(selected.mount,mounted)&&mounted.archiveName&&mounted.archiveSha256&&
            *mounted.archiveName==archiveName&&*mounted.archiveSha256==archiveSha&&
            mounted.archiveBytes==archiveBytes&&mounted.memberCount==mc_cold_sources50LA::archiveEntries&&
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
}
