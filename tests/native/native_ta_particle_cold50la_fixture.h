#pragma once
// ONE track particle manager load for every course (PLAN B-C16, step 05.06).
// Original 1B4DC0 builds the track's particle manager; 1617A0 forms the atlas
// member from 36B530 ("particle\\") + the SCB Environment
// ParticleTextureMapName with its last extension replaced (.prt/.psm/.pst), and
// 299BE0 acquires the canonical particle material. Nothing here names a course
// or a member: the SCB names the atlas (stock: demoprt.tga), and the stock
// PRT/PSM/PST bytes are receipts (stock_course_receipts50cr.h particles.*).
// The per-course RC/TB/GB compact-source paths and the MC path are retired.
#include "original_ui_main_entry50kg.h"
#include "common_track50cr.h"
#include "runtime_startup50fl_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/dlc_course_entries50dl.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/original_standard_material_source50la.h"
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace native_ta_particle_cold50LA {
// Largest PRT/PSM/PST member the reader copies (the shared course-archive
// member cap; a larger member is refused by capacity, never by identity).
inline constexpr std::size_t maximumMemberBytes=8u*1024u*1024u;

// The SCB Environment ParticleTextureMapName (the one field 1617A0 reads).
// Returns nullptr when the SCB names none; refuses a duplicate.
inline const std::string* particleTextureMapName(const rr::assets::World48F& world){
    const auto sameName=[](const std::string& a,const char* b){
        std::size_t i=0;
        for(;i<a.size()&&b[i];++i){const auto lower=[](char c){return c>='A'&&c<='Z'?char(c+('a'-'A')):c;};
            if(lower(a[i])!=lower(b[i]))return false;}
        return i==a.size()&&!b[i];
    };
    const std::string* name=nullptr;
    for(const auto& block:world.parameters.blocks){
        if(!sameName(block.name,"Environment"))continue;
        for(const auto& field:block.fields){
            if(!sameName(field.name,"ParticleTextureMapName"))continue;
            runtime_startup50fl::require(!name,"duplicate SCB ParticleTextureMapName");name=&field.value;
        }
    }
    return name;
}

// The PRT/PSM/PST member names 1617A0 forms from the SCB atlas name.
struct Members {std::string prt,psm,pst;};
inline Members atlasMembers(const original_main_entry50KG::Application& application,const std::string& particleName){
    //1617A0 uses36B530+the original SCB name and replaces its last extension.
    //It does not choose a material from the PRT's separate TextureMap field.
    std::string stem=application.particleDirectory36B530()+particleName;
    const auto dot=stem.find_last_of('.');
    if(dot!=std::string::npos)stem.resize(dot);
    return {stem+".prt",stem+".psm",stem+".pst"};
}

// 1B4DC0 for the race's Track. Consumes the Track's decoded SCB and archive in
// the SAME Application material domain, before the caller's 14D370/29E340
// material population (05.08). No recovery-time decoder, alternate domain or
// source fallback. Throws std::runtime_error on a refusal (the 05.06 step
// turns it into a common_load refusal); returns the owner's own result.
inline bool prepare(rr::game::OriginalTrackParticleCold50LA& owner,
    const common_track50CR::Track& track,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> actual382B38,
    std::uint64_t generation,rr::game::OriginalTrackParticleColdDiagnostic50LA& diagnostic) {
    using runtime_startup50fl::require;
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    require(track.archive.opened()&&track.archive.decoded(),"Track particle requires the race's opened course archive");
    const auto domain=application.standardMaterialDomain();
    require(domain&&domain->prepared()&&actual382B38&&domain->materials382B38()==actual382B38&&
        domain->resolver3E1A70()&&!actual382B38->populationTerminal()&&!actual382B38->demandTerminal()&&
        !actual382B38->coldCollectorCurrent(domain->coldCollector14D440()),
        "Track particle requires the actual cold Application domain before collector publication");
    const auto provider=domain->archiveProvider3E1A70();
    require(provider&&provider->sealed(),"Track particle requires its complete sealed native archive provider");
    const auto providerRevision=provider->revision();
    const auto* particleName=particleTextureMapName(track.documents.world);
    require(particleName&&!particleName->empty()&&particleName->size()<128&&
        particleName->find_first_of("/\\")==std::string::npos,
        "Original track particle atlas name is unavailable");
    const auto members=atlasMembers(application,*particleName);
    // Same bounded fresh-cold guard as the original particle stage: never
    // adopt another manager's retained/populated canonical particle material.
    std::int32_t particleRow=-1;rr::assets::OriginalStandardMaterialRowView50LA material;
    require(actual382B38->find298C80(members.prt.substr(0,members.prt.size()-4),particleRow,diagnostic.material)&&
        actual382B38->row(particleRow,material)&&material.references08&&
        *material.references08==0&&!material.object04,
        "Track particle requires fresh canonical particle material reference ownership");
    // inherit.particles (add-on row only; PORT EXTENSION, NOT ORIGINAL): an
    // add-on archive without the particle closure names a stock course whose
    // own TRACKS/<CODE>.RES supplies it (the registry row says so). The
    // archive owner (R3, Archive::open) mounts that closure beside the course
    // archive in the same provider namespace; a stock course reads its own.
    rr::game::dlc50DL::AddonCourseLoad addon;
    const bool inherited=track.source()==common_track50CR::Source::registry&&
        rr::game::dlc50DL::addonCourseLoadByCode(std::string(track.code()).c_str(),addon)&&addon.particlesInherited;
    std::vector<std::uint8_t> prt,psm,pst;
    if(inherited){
        require(!addon.particleArchive.empty(),"inherited particle archive unnamed");
        // readBytes resolves the repository-relative disc path (runtime_asset_paths50LB).
        const auto inheritedBytes=runtime_startup50fl::readBytes("work/native/project/disc/"+addon.particleArchive);
        rr::assets::ResArchive48C inheritedArchive;rr::assets::DecodeError48C error;
        require(rr::assets::decodeResource48C(inheritedBytes,inheritedArchive,error),
            "inherited particle archive decode rejected");
        require(rr::assets::extractResMember48C(inheritedArchive,members.prt,prt,error)&&
            rr::assets::extractResMember48C(inheritedArchive,members.psm,psm,error)&&
            rr::assets::extractResMember48C(inheritedArchive,members.pst,pst,error),
            "inherited particle PRT/PSM/PST closure unavailable");
    }else{
        common_track50CR::Diagnostic member;
        require(track.archive.member(members.prt,prt,maximumMemberBytes,member)&&
            track.archive.member(members.psm,psm,maximumMemberBytes,member)&&
            track.archive.member(members.pst,pst,maximumMemberBytes,member),
            "SCB-selected original track PRT/PSM/PST closure unavailable");
    }
    require(!prt.empty()&&!psm.empty()&&!pst.empty(),"Original selected-track particle closure empty");
    {
        std::string code(track.code());
        for(auto& c:code)if(c>='A'&&c<='Z')c=static_cast<char>(c+('a'-'A'));
        std::cerr<<"course_route_particles code="<<code<<" source="
                 <<(inherited?"inherited:"+addon.particleArchive:std::string("own"))<<" prt="<<members.prt<<'\n';
    }
    // The provider must serve exactly these bytes under these names (the same
    // mounted course archive, or the inherited closure mounted beside it).
    const auto matches=[&](const std::string& requested,const std::vector<std::uint8_t>& original){
        Provider::MemberView selected;
        return provider->lookup132820(requested,selected)==Provider::Lookup::present&&
            selected.member&&Provider::sameName(*selected.member,requested)&&selected.data&&
            selected.revision==providerRevision&&selected.size==original.size()&&
            !std::memcmp(selected.data,original.data(),original.size());
    };
    const bool prtMatch=matches(members.prt,prt),psmMatch=matches(members.psm,psm),pstMatch=matches(members.pst,pst);
    if(!(prtMatch&&psmMatch&&pstMatch))std::cerr<<"course_route_particle_closure code="<<track.code()
        <<" inherited="<<(inherited?1:0)<<" prt="<<prtMatch<<" psm="<<psmMatch<<" pst="<<pstMatch<<'\n';
    require(prtMatch&&psmMatch&&pstMatch&&provider->sealed()&&provider->revision()==providerRevision,
        "Track particle closure differs from the same mounted archive");
    auto materialSource=std::make_shared<rr::assets::OriginalStandardMaterialSource50LA>();
    require(materialSource->prepare(members.psm,psm,diagnostic.material.document),
        "Original selected-track particle PSM parsing rejected");
    // Same domain owns the exact PSM/PST beyond these temporary source vectors.
    // The particle owner acquires its canonical reference before the caller's
    // 14D370/29E340 population. Failure retains real partial effects.
    const bool appended=domain->appendOriginalSources({materialSource},{{members.pst,pst}},diagnostic.material);
    if(!appended)std::cerr<<"course_route_particle_append code="<<track.code()<<" inherited="<<(inherited?1:0)
        <<" status="<<unsigned(diagnostic.material.status)<<" pc="<<std::hex<<diagnostic.material.sourcePC<<std::dec<<'\n';
    require(appended,"Original selected-track particle canonical source append rejected");
    const bool prepared=application.withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){
        return owner.prepareNativeTrack(actualDirectory,*particleName,prt,application.entities(),
            std::move(actual382B38),generation,diagnostic);
    });
    if(!prepared)return false;
    // The same PSM/PST bytes, retained on the manager for the one particle
    // material owner (continuation: particleMaterials.load(particlePsm, particlePst)).
    require(owner.retainAtlasMaterialSources(std::move(psm),std::move(pst)),
        "Track particle PSM/PST retention rejected");
    require(provider->sealed()&&provider->revision()==providerRevision&&owner.material150()&&
        owner.material150()->table382B38()==domain->materials382B38().get(),
        "Track particle construction changed its canonical source ownership");
    return true;
}
} // namespace native_ta_particle_cold50LA
