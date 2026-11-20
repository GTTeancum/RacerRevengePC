#pragma once
#include "track_materials50fa_fixture.h"
#include "../../src/assets/world_normals50hk.h"

// Cold bridge for authenticated selected-track normals omitted by RRMESH01.
// It does not regenerate normals, weld positions, smooth edges, modify source
// RGB/UV/positions/indices, or claim original PS2 lighting program parity.
namespace track_normals50hk_fixture {
struct Diagnostic {
    std::uint32_t scenes=0, instances=0, surfaces=0, normals=0, zeroNormals=0;
    std::size_t sourceBytes=0;
    std::string message;
};
inline bool load(const std::filesystem::path& root,
    const std::filesystem::path& exportReport, const rr::assets::Mesh48B& mesh,
    std::vector<rr::assets::WorldNormal50HK>& output, Diagnostic& diagnostic,
    track_source_fixture::Track track = track_source_fixture::Track::ta) noexcept {
    namespace a=rr::assets;
    namespace d=track_materials50fa_fixture::detail;
    try {
        diagnostic={};
        const auto& profile=track_visual_profile_fixture::get(track);
        d::need(std::fegetround()==FE_TONEAREST, "Track normal bridge requires FE_TONEAREST");
        d::need(mesh.vertices.size()==profile.vertices && mesh.indices.size()==profile.indices,
            "Track normal bridge requires exact retained mesh extent");
        d::Inputs inputs; inputs.root=std::filesystem::canonical(root);
        inputs.capacity=32u*1024u*1024u;
        d::need(!std::filesystem::is_symlink(inputs.root/"cacheobjects"), "Track normal cache symlink rejected");
        const auto selectionBytes=inputs.read(inputs.root/"asset-selection.json",4u*1024u*1024u);
        const auto reportBytes=inputs.read(exportReport,4u*1024u*1024u);
        d::need(d::sha256(selectionBytes)==profile.selectionSha && d::sha256(reportBytes)==profile.exportSha,
            "Track normal selection/export identity mismatch");
        const auto selection=d::Json::parse(selectionBytes.begin(),selectionBytes.end());
        const auto report=d::Json::parse(reportBytes.begin(),reportBytes.end());
        for (const auto& row:selection.at("objects")) {
            const auto path=row.at("path").get<std::string>();
            d::need(d::shared::safeObjectPath(path) && d::hash(row.at("sha256")) &&
                inputs.objects.emplace(path,row).second,"Track normal source path/hash/duplicate");
        }
        std::map<std::string,const d::Json*> sources;
        for (const auto& row:selection.at("scene_requests")) if (row.at("container")==profile.container)
            d::need(sources.emplace(row.at("key"),&row).second,"Track normal duplicate scene");
        d::need(sources.size()==profile.scenes && report.at("world_instances").size()==profile.instances,
            "Track normal source count mismatch");
        std::map<std::string,a::Scene48D> scenes;
        std::vector<a::WorldNormal50HK> next; next.reserve(mesh.vertices.size());
        std::uint32_t nextIndex=0;
        for (const auto& instance:report.at("world_instances")) {
            const std::string key=instance.at("key");
            auto found=scenes.find(key);
            if (found==scenes.end()) {
                const auto source=sources.find(key);
                d::need(source!=sources.end(),"Track normal instance scene unselected");
                const auto& row=*source->second;
                const auto& record=inputs.record(key);
                d::need(row.at("status")=="converted_geometry" && record==row.at("native") &&
                    record.at("source_sha256")==row.at("source_sha256") &&
                    row.at("rrscene")=="cacheobjects/"+key+".rrscene", "Track normal scene provenance");
                const auto bytes=inputs.selected(row.at("rrscene"),a::max_scene_bytes48d);
                d::need(record.at("scene_sha256")==d::sha256(bytes),"Track normal source hash mismatch");
                a::Scene48D scene; std::string why;
                d::need(a::parseSceneAsset48D(bytes,scene,why),"Track normal source parse: "+why);
                d::need(scene.nodes.size()==1 && !scene.identity_matrix_palette && !scene.lods.empty(),
                    "Track normal single-root source subset");
                found=scenes.emplace(key,std::move(scene)).first;
            }
            const auto& scene=found->second;
            auto matrix=instance.at("matrix").get<std::array<double,16>>();
            if ((track==track_source_fixture::Track::gc||track==track_source_fixture::Track::ga||
                 track==track_source_fixture::Track::ma||track==track_source_fixture::Track::sa||track==track_source_fixture::Track::course) && scene.world_space)
                matrix={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
            if ((track==track_source_fixture::Track::gc||track==track_source_fixture::Track::ga||
                 track==track_source_fixture::Track::ma||track==track_source_fixture::Track::sa||track==track_source_fixture::Track::course))
                d::need(instance.at("visual_matrix")==d::Json(matrix) &&
                    instance.at("world_space_root_bypassed")==scene.world_space,
                    "Original world-space normal root policy mismatch");
            const auto vertexBase=next.size();
            const auto indexBase=nextIndex;
            d::need(instance.at("vertex_start")==vertexBase && instance.at("index_start")==indexBase &&
                instance.at("surfaces").size()==scene.spans_lod0.size() &&
                scene.lods[0].surfaces.size()==scene.spans_lod0.size(),"Track normal source instance order");
            for (std::size_t si=0; si<scene.spans_lod0.size(); ++si) {
                const auto& span=scene.spans_lod0[si]; const auto& surface=scene.lods[0].surfaces[si];
                d::need(vertexBase+span.vertex_start==next.size() && indexBase+span.index_start==nextIndex &&
                    span.vertex_count==surface.vertices.size() && span.index_count==surface.indices.size() &&
                    surface.vertices.size()<=mesh.vertices.size()-next.size() &&
                    surface.indices.size()<=mesh.indices.size()-nextIndex,"Track normal source surface order/extent");
                const auto surfaceBase=next.size();
                for (const auto& authored:surface.vertices) {
                    const auto& live=mesh.vertices[next.size()];
                    d::need(authored.bone==0,"Track normal source bone unsupported");
                    for (unsigned axis=0; axis<3; ++axis) {
                        double position=0;
                        for (unsigned k=0;k<3;++k) position+=authored.position_local[k]*matrix[k*4+axis];
                        position+=matrix[12+axis];
                        d::need(std::isfinite(position) && d::sameFloat(live.position[axis],static_cast<float>(position)),
                            "Track normal source placement mismatch");
                    }
                    for (unsigned axis=0;axis<2;++axis)
                        d::need(d::sameFloat(live.uv[axis],static_cast<float>(authored.uv[axis])),"Track normal source UV mismatch");
                    for (unsigned axis=0;axis<4;++axis)
                        d::need(live.color[axis]==(surface.has_original_colors?authored.color[axis]:255),"Track normal source color mismatch");
                    a::WorldNormal50HK normal;
                    d::need(a::transformWorldNormal50HK(authored.normal_local,matrix,normal),"Track source normal transform failed");
                    if (normal.xyz[0]==0 && normal.xyz[1]==0 && normal.xyz[2]==0) ++diagnostic.zeroNormals;
                    next.push_back(normal);
                }
                for (const auto index:surface.indices) {
                    d::need(index<surface.vertices.size() && mesh.indices[nextIndex]==surfaceBase+index,
                        "Track normal source index order mismatch");
                    ++nextIndex;
                }
                ++diagnostic.surfaces;
            }
            ++diagnostic.instances;
        }
        d::need(next.size()==mesh.vertices.size() && nextIndex==mesh.indices.size() &&
            diagnostic.surfaces==profile.surfaces,"Track normal full mesh coverage");
        diagnostic.scenes=static_cast<std::uint32_t>(scenes.size());
        diagnostic.normals=static_cast<std::uint32_t>(next.size());
        diagnostic.sourceBytes=inputs.sourceBytes;
        output=std::move(next);
        return true;
    } catch (const std::exception& e) { try {diagnostic.message=e.what();} catch (...) {} return false; }
    catch (...) { try {diagnostic.message="Unexpected track source normal failure";} catch (...) {} return false; }
}
} // namespace track_normals50hk_fixture
