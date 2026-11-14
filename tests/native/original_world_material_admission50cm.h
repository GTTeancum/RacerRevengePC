#pragma once
// One world-material pass admission for every course. Every per-course world
// loader asks this header; none of them may key on a course, material or texture
// name (repo CLAUDE.md, common path).
//
// Original rule (SLUS_202.68):
// - Only TotalShaderPasses passes exist (29B718 reads the count; later
//   [ShaderPass_N] sections are dead text). decodeMaterial48E already honours it.
// - A pass's microProgramName is looked up in the program table 382B70 by
//   298C80: first match, ASCII case-insensitive (compare 308F00). The table is
//   filled once by 1505C0; the world programs below are rows 33 ProjectedPrelit
//   (150A1C, name 3A56C0), 38 DiffuseCompressed (150ABC, 3A5720), 39
//   PrelitCompressed (150ADC, 3A5740), 40 EnvironmentMapCompressed (150AFC,
//   3A5760) and 43 DiffuseTexTransCompressed (150B5C, 3A57C0). The same table
//   and lookup serve every course, pod and menu; nothing reads the material,
//   texture or course name. A name not in the table takes the StringTable.cpp
//   "GetIndex: Couldn't find the string" assert path (298D24..298D5C) and -1.
//   See also src/assets/original_standard_program_table50la.{h,cpp}.
//
// Port boundary (flagged stand-in, keyed on program data only, never on names):
// the native world path draws one texture map per pass (slot 1), at most two
// passes per surface (TrackProjectedUv holds two UV pairs), and only these five
// programs. ProjectedPrelit is drawn only as an all-projected pair; an
// environment map only as a later pass of a non-projected material. This is
// the union of what the per-course loaders admitted before, minus the names.
// Anything else is refused, not approximated.
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/material_texture_assets50ea.h"
#include "../../src/assets/native_asset48b.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <cstddef>
#include <string>

namespace original_world_material_admission50CM {
enum class Program : unsigned char {
    unknown,prelitCompressed,diffuseCompressed,diffuseTexTransCompressed,projectedPrelit,environmentMapCompressed
};
inline constexpr std::size_t maxWorldPasses=2;
namespace detail {
// 308F00: ASCII case fold, whole-string equality.
inline bool same308F00(const std::string& a,const char* b) noexcept {
    std::size_t i=0;
    for(;i<a.size()&&b[i];++i){
        char x=a[i],y=b[i];
        if(x>='A'&&x<='Z')x=char(x+('a'-'A'));
        if(y>='A'&&y<='Z')y=char(y+('a'-'A'));
        if(x!=y)return false;
    }
    return i==a.size()&&!b[i];
}
}
inline Program program(const std::string& name) noexcept {
    struct Row{const char* name;Program program;};
    static constexpr Row rows[]={
        {"PrelitCompressed",Program::prelitCompressed},
        {"DiffuseCompressed",Program::diffuseCompressed},
        {"DiffuseTexTransCompressed",Program::diffuseTexTransCompressed},
        {"ProjectedPrelit",Program::projectedPrelit},
        {"EnvironmentMapCompressed",Program::environmentMapCompressed},
    };
    for(const auto& row:rows)if(detail::same308F00(name,row.name))return row.program;
    return Program::unknown;
}
inline bool projected(const rr::assets::MaterialPass48E& pass) noexcept {
    return program(pass.microprogram)==Program::projectedPrelit;
}
inline bool materialPassCount(const rr::assets::Material48E& material) noexcept {
    return !material.passes.empty()&&material.passes.size()<=maxWorldPasses;
}
// Pass pi of an already decoded material.
inline bool pass(const rr::assets::Material48E& material,std::size_t pi) noexcept {
    if(!materialPassCount(material)||pi>=material.passes.size())return false;
    const auto& source=material.passes[pi];
    if(source.pass!=pi+1||source.textures.size()!=1||source.textures[0].slot!=1)return false;
    bool anyProjected=false,allProjected=true;
    for(const auto& other:material.passes){const bool p=projected(other);anyProjected=anyProjected||p;allProjected=allProjected&&p;}
    switch(program(source.microprogram)){
    case Program::prelitCompressed:
    case Program::diffuseCompressed:
    case Program::diffuseTexTransCompressed: return !anyProjected;
    case Program::projectedPrelit: return material.passes.size()==2&&allProjected;
    case Program::environmentMapCompressed: return pi>=1&&!anyProjected;
    case Program::unknown: break;
    }
    return false;
}

// ---- C: identity and closure receipts, capacity bounds, unpinned images ----
// The original hashes nothing and pins no counts: it opens TRACKS\XX.RES by the
// course table's name and decodes what is there. Stock identities and stock
// counts survive only as receipt flags (stock=1|0) in the log, never as gates.
// Every bound below is a native capacity or a shared resource limit, checked
// before the write it protects, never a stock count.

// Shared resource limits (import_decode48c.h defaults): file 64 MiB, decoded
// 128 MiB, 10000 entries. The file limit is sized from the actual file.
inline bool archiveFits(std::size_t fileBytes) noexcept {
    return fileBytes&&fileBytes<=rr::assets::DecodeLimits48C{}.resource_file_bytes;
}
inline rr::assets::DecodeLimits48C archiveLimits(std::size_t fileBytes) noexcept {
    rr::assets::DecodeLimits48C limits;limits.resource_file_bytes=fileBytes;return limits;
}
// Native capacities. Images: the material texture bank hard ceiling.
// Selections and spans: the renderer caps maxVehicleWindowMaterials50DS and
// maxVehicleWindowTrackSpans50DS (platform/linux/native/vehicle_window_gl50ds.h:68-69),
// restated here so this test header does not pull in the renderer header.
inline constexpr std::uint32_t maxWorldMaterials=rr::assets::maxMaterialTextureAssets50EA;
inline constexpr std::uint32_t maxWorldImages=rr::assets::maxMaterialTextureAssets50EA;
inline constexpr std::uint32_t maxWorldSelections=4096;
inline constexpr std::size_t maxWorldSpans=65536;
static_assert(maxWorldSelections<=rr::assets::maxMaterialTextureSlots50EA,"world selections exceed bank slots");

// One receipt line per load. Logs only.
inline void identityReceipt(const char* course,bool stock,const std::string& archiveSha,std::size_t archiveBytes,
    std::size_t decodedBytes,std::size_t entries,const std::string& scbSha,std::size_t scbBytes) noexcept {
    std::fprintf(stderr,"world_identity course=%s stock=%d archive_sha=%s bytes=%llu decoded=%llu entries=%llu scb_sha=%s scb_bytes=%llu\n",
        course?course:"?",stock?1:0,archiveSha.empty()?"-":archiveSha.c_str(),
        static_cast<unsigned long long>(archiveBytes),static_cast<unsigned long long>(decodedBytes),
        static_cast<unsigned long long>(entries),scbSha.empty()?"-":scbSha.c_str(),static_cast<unsigned long long>(scbBytes));
}
inline void closureReceipt(const char* course,bool stock,std::size_t materials,std::size_t selections,
    std::size_t spans,std::size_t images,std::size_t vertices) noexcept {
    std::fprintf(stderr,"world_closure course=%s stock=%d materials=%llu selections=%llu spans=%llu images=%llu vertices=%llu\n",
        course?course:"?",stock?1:0,static_cast<unsigned long long>(materials),static_cast<unsigned long long>(selections),
        static_cast<unsigned long long>(spans),static_cast<unsigned long long>(images),static_cast<unsigned long long>(vertices));
}
// Closure as bounds and invariants, replacing the stock-count equalities.
inline bool closureBounds(std::size_t scenes,std::size_t placements,std::size_t collisionOnly,std::size_t surfaces,
    std::size_t instances,std::size_t materials,std::size_t selections,std::size_t secondPasses,std::size_t spans,
    std::size_t vertices,std::size_t indices,std::size_t images,std::size_t pinnedImages,std::size_t runtimeImages) noexcept {
    return placements<=instances&&collisionOnly<=instances-placements&&scenes<=placements&&
        materials<=maxWorldMaterials&&selections<=maxWorldSelections&&images<=maxWorldImages&&images<=selections&&
        spans<=maxWorldSpans&&surfaces<=spans&&secondPasses<=spans-surfaces&&
        vertices<=rr::assets::max_mesh_vertices&&indices<=rr::assets::max_mesh_indices&&
        pinnedImages<=images&&runtimeImages==images-pinnedImages;
}
// "disc:<member>:<source sha>" -> source sha (the loaders build this key).
inline std::string keySha(const std::string& key) {
    const auto at=key.rfind(':');return at==std::string::npos?std::string():key.substr(at+1);
}
// Unpinned image: extent from the decoded RRTEX001 envelope, bounded to the
// native 1..1024 texel range before any size arithmetic.
inline bool decodedExtent(const std::vector<std::uint8_t>& output,std::uint32_t& w,std::uint32_t& h) noexcept {
    if(output.size()<16||std::memcmp(output.data(),"RRTEX001",8))return false;
    const auto u32=[&](std::size_t at){return std::uint32_t(output[at])|(std::uint32_t(output[at+1])<<8)|
        (std::uint32_t(output[at+2])<<16)|(std::uint32_t(output[at+3])<<24);};
    w=u32(8);h=u32(12);
    return w&&h&&w<=1024&&h<=1024&&output.size()==std::size_t(w)*h*4+16;
}
} // namespace original_world_material_admission50CM
// Short name for header-side code included inside loader namespaces.
namespace wma50CM=original_world_material_admission50CM;
