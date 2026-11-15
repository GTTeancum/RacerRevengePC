#pragma once
// One rule for every course, pod and menu: before a 14D370/29E340 population
// pair, the material domain admits the PSM (and its texture closure) of every
// table row that already holds references but has no loaded object yet.
//
// Original (SLUS_202.68): 14D370 visits every row with refs>0 and no object and
// 29B628 opens "<name>.psm" from the mounted file system at that moment. The
// port's material domain constructs only from admitted, authenticated sources,
// so the caller admits them from the same sealed mount provider immediately
// before the pair. Keyed on table state only (refs and object), never on a
// course, material or texture name.
//
// A row whose PSM is genuinely absent from the mounts is left for 29B628's own
// missing-file handling inside construction (which refuses what the port has
// not established). Any other lookup or document failure refuses before the
// domain is touched; the append itself is transactional (domain).
#include "original_racer_material_sources50la_fixture.h"
#include <cstdio>

namespace original_pending_material_sources50LA {
namespace s=original_racer_material_sources50LA;
struct Result {
    std::size_t pendingRows=0,absentRows=0,materials=0,textures=0;
};
inline bool collect(const rr::assets::OriginalStandardMaterialTable50LA& table,
    const s::Provider& provider,s::Materials& materials,s::Textures& textures,
    Result& result,s::Diagnostic& d)noexcept {
    d={};result={};
    if(!provider.sealed())return s::detail::fail(d,s::Status::unsealed,0x132820);
    if(!table.prepared()||table.populationTerminal()||table.demandTerminal())
        return s::detail::fail(d,s::Status::reference,0x14D370);
    const auto revision=provider.revision();
    try {
        auto nextMaterials=materials;auto nextTextures=textures;
        const auto count=table.rowCount();
        for(std::size_t i=0;i<count;++i){
            rr::assets::OriginalStandardMaterialRowView50LA row;
            if(!table.row(static_cast<std::int32_t>(i),row)||!row.name00||!row.references08)
                return s::detail::fail(d,s::Status::reference,0x14D3A0);
            if(*row.references08<=0||row.object04)continue; //14D3A0..3B8 gates.
            ++result.pendingRows;
            std::fprintf(stderr,"pending_material_row row=%zu name=%s refs=%d\n",i,row.name00->c_str(),int(*row.references08));
            s::Provider::MemberView view;
            const auto found=provider.lookup132820(s::detail::withExtension(*row.name00,".psm"),view);
            if(found==s::Provider::Lookup::unknown)return s::detail::fail(d,s::Status::unsealed,0x132820);
            if(found==s::Provider::Lookup::absent){++result.absentRows;continue;}
            if(!s::detail::material(provider,*row.name00,nextMaterials,nextTextures,d))return false;
        }
        if(!provider.sealed()||provider.revision()!=revision)return s::detail::fail(d,s::Status::unsealed,0x132820);
        result.materials=nextMaterials.size()-materials.size();
        result.textures=nextTextures.size()-textures.size();
        materials.swap(nextMaterials);textures.swap(nextTextures);d.status=s::Status::complete;return true;
    }catch(...){return s::detail::fail(d,s::Status::allocation,0x29B628);}
}
}
