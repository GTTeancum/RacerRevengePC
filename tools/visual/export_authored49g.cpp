// Headless authored-asset diagnostic exporter. Input cache objects are read-only.
// Build with C++17 and -ffp-contract=off; never use -ffast-math.
#include "../../src/assets/scene_asset48d.h"
#include "../../src/assets/native_asset48b.h"
#include "../../src/assets/world_asset48f.h"
#include "../../third_party/native-json/json.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Authored placement requires finite checks and ordered IEEE arithmetic"
#endif

namespace {
namespace fs = std::filesystem;
namespace assets = rr::assets;
using Json = nlohmann::json;
using Bytes = std::vector<std::uint8_t>;
using Vec3 = std::array<double, 3>;
constexpr const char* taContainer = "TRACKS/TA.RES";
constexpr const char* podContainer = "PODS/POD01/POD01.RES";

void need(bool value, const std::string& reason) {
    if (!value) throw std::runtime_error(reason);
}
std::string normalized(std::string value) {
    for (auto& c : value) {
        if (c == '/') c = '\\';
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + ('a' - 'A'));
    }
    return value;
}
Bytes readBytes(const fs::path& path, std::size_t limit) {
    std::ifstream stream(path, std::ios::binary | std::ios::ate);
    need(stream.is_open(), "Cannot open input: " + path.string());
    const auto length = stream.tellg();
    need(length > 0 && static_cast<std::uintmax_t>(length) <= limit,
         "Input size outside limit: " + path.string());
    Bytes bytes(static_cast<std::size_t>(length));
    stream.seekg(0);
    stream.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    need(stream.good() && stream.peek() == std::char_traits<char>::eof(),
         "Input read failed or size changed: " + path.string());
    return bytes;
}
Json readJson(const fs::path& path, std::size_t limit = 16u * 1024u * 1024u) {
    const auto bytes = readBytes(path, limit);
    return Json::parse(bytes.begin(), bytes.end());
}
std::string keyOf(const Json& row) {
    const auto key = row.at("key").get<std::string>();
    need(key.size() == 64 && key.find_first_not_of("0123456789abcdef") == std::string::npos,
         "Invalid native object key");
    return key;
}
fs::path objectPath(const fs::path& root, const Json& row, const char* field, const char* suffix) {
    const auto relative = row.at(field).get<std::string>();
    need(relative == "cacheobjects/" + keyOf(row) + suffix, "Unexpected selected cache object path: " + relative);
    const auto result = root / relative;
    // Refuse symlink redirection; the selected immutable cache closure is local.
    need(!fs::is_symlink(result) && !fs::is_symlink(root / "cacheobjects"), "Symlink cache input rejected");
    return result;
}
float finiteFloat(double value) {
    need(std::isfinite(value) && std::abs(value) <= std::numeric_limits<float>::max(),
         "Position or UV exceeds finite float32 range");
    return static_cast<float>(value);
}
bool nonidentity(const std::array<double, 16>& matrix) {
    for (std::size_t i = 0; i < matrix.size(); ++i)
        if (matrix[i] != (i % 5 == 0 ? 1.0 : 0.0)) return true;
    return false;
}
void appendVertex(assets::Mesh48B& mesh, const assets::Vertex48B& vertex) {
    need(mesh.vertices.size() < assets::max_mesh_vertices, "Native mesh vertex limit");
    for (unsigned c = 0; c < 3; ++c) {
        need(std::isfinite(vertex.position[c]), "Nonfinite native position");
        if (mesh.vertices.empty()) mesh.bounds.minimum[c] = mesh.bounds.maximum[c] = vertex.position[c];
        else {
            mesh.bounds.minimum[c] = std::min(mesh.bounds.minimum[c], vertex.position[c]);
            mesh.bounds.maximum[c] = std::max(mesh.bounds.maximum[c], vertex.position[c]);
        }
    }
    for (const auto uv : vertex.uv) need(std::isfinite(uv), "Nonfinite native UV");
    mesh.vertices.push_back(vertex);
}
void appendIndices(assets::Mesh48B& mesh, const std::vector<std::uint32_t>& indices,
                   std::uint32_t vertexStart, std::size_t vertexCount) {
    need(indices.size() % 3 == 0 && indices.size() <= assets::max_mesh_indices - mesh.indices.size(),
         "Native mesh triangle/index limit");
    for (const auto index : indices) {
        need(index < vertexCount, "Source mesh index outside vertex range");
        mesh.indices.push_back(vertexStart + index);
    }
}
void u32(Bytes& bytes, std::uint32_t value) {
    for (unsigned c = 0; c < 4; ++c) {
        bytes.push_back(static_cast<std::uint8_t>(value));
        value >>= 8;
    }
}
void f32(Bytes& bytes, float value) {
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559, "IEEE binary32 required");
    need(std::isfinite(value), "Nonfinite serialized float");
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    u32(bytes, bits);
}
Bytes encodeMesh(const assets::Mesh48B& mesh) {
    need(!mesh.vertices.empty() && mesh.vertices.size() <= assets::max_mesh_vertices &&
         !mesh.indices.empty() && mesh.indices.size() <= assets::max_mesh_indices && mesh.indices.size() % 3 == 0,
         "Invalid native mesh counts");
    Bytes bytes{'R','R','M','E','S','H','0','1'};
    bytes.reserve(16 + mesh.vertices.size() * 24 + mesh.indices.size() * 4);
    u32(bytes, static_cast<std::uint32_t>(mesh.vertices.size()));
    u32(bytes, static_cast<std::uint32_t>(mesh.indices.size()));
    for (const auto& vertex : mesh.vertices) {
        for (const auto p : vertex.position) f32(bytes, p);
        for (const auto uv : vertex.uv) f32(bytes, uv);
        bytes.insert(bytes.end(), std::begin(vertex.color), std::end(vertex.color));
    }
    for (const auto index : mesh.indices) {
        need(index < mesh.vertices.size(), "Serialized index outside vertex range");
        u32(bytes, index);
    }
    need(bytes.size() <= assets::max_asset_file_bytes, "Native mesh byte limit");
    return bytes;
}
void verifyRoundtrip(const Bytes& bytes) {
    assets::Mesh48B parsed;
    std::string error;
    const auto accepted = assets::parseMesh48B(bytes.data(), bytes.size(), parsed, error);
    need(accepted, "RRMESH01 roundtrip parse failed: " + error);
    need(encodeMesh(parsed) == bytes, "RRMESH01 roundtrip bytes differ");
}
Json bounds(const assets::Mesh48B& mesh) {
    need(!mesh.vertices.empty(), "Empty mesh has no bounds");
    return Json{{"minimum", {mesh.bounds.minimum[0], mesh.bounds.minimum[1], mesh.bounds.minimum[2]}},
                {"maximum", {mesh.bounds.maximum[0], mesh.bounds.maximum[1], mesh.bounds.maximum[2]}}};
}
Json meshReport(const assets::Mesh48B& mesh, std::size_t surfaces, const char* filename) {
    return Json{{"mesh", filename}, {"vertices", mesh.vertices.size()}, {"indices", mesh.indices.size()},
                {"triangles", mesh.indices.size() / 3}, {"surfaces_lod0", surfaces}, {"bounds", bounds(mesh)},
                {"bytes", 16 + mesh.vertices.size() * 24 + mesh.indices.size() * 4},
                {"all_positions_and_uvs_finite", true}, {"parseMesh48B_roundtrip", true}};
}
struct Scene {
    Json request;
    assets::Scene48D scene;
    assets::Mesh48B bind;
    bool companionVerified = false;
};
Scene loadScene(const fs::path& root, const Json& request) {
    Scene result;
    result.request = request;
    need(request.at("status") == "converted_geometry", "Selected scene is not converted geometry");
    const auto record = readJson(objectPath(root, request, "record", ".json"));
    need(record == request.at("native"), "Selected scene record differs from retained native metadata");
    need(record.at("key") == keyOf(request) && record.at("source_sha256") == request.at("source_sha256"),
         "Selected scene key/source provenance mismatch");
    const auto bytes = readBytes(objectPath(root, request, "rrscene", ".rrscene"), assets::max_scene_bytes48d);
    std::string error;
    const auto accepted = assets::parseSceneAsset48D(bytes, result.scene, error);
    need(accepted, "RRSCENE1 parse failed for " + request.at("member").get<std::string>() + ": " + error);
    const auto& scene = result.scene;
    const auto meshAccepted = assets::parseMesh48B(scene.neutral_lod0.data(), scene.neutral_lod0.size(), result.bind, error);
    need(meshAccepted, "Scene bind mesh parse failed: " + error);
    need(record.at("vertices") == result.bind.vertices.size() && record.at("triangles") == result.bind.indices.size() / 3 &&
         record.at("nodes") == scene.nodes.size() && record.at("lods") == scene.lods.size() &&
         record.at("vertices_all_lods") == scene.vertices_all_lods && record.at("surfaces_all_lods") == scene.surfaces_all_lods &&
         record.at("materials") == Json(scene.materials) && record.at("world_space") == scene.world_space,
         "Parsed scene counters or semantic metadata differ from selected record");
    const auto companion = objectPath(root, request, "rrmesh", ".rrmesh");
    if (fs::exists(companion)) {
        const auto meshBytes = readBytes(companion, assets::max_asset_file_bytes);
        need(meshBytes == scene.neutral_lod0, "Companion RRMESH01 differs from scene neutral LOD0");
        verifyRoundtrip(meshBytes);
        result.companionVerified = true;
    }
    return result;
}
void verifyInstance(const Json& row, const assets::WorldInstance48F& instance) {
    need(row.at("section") == instance.section && row.at("geometry_member") == instance.geometry_member &&
         row.at("zone") == instance.zone && row.at("ordinal") == instance.ordinal &&
         row.at("parameter_block") == instance.parameter_block && row.at("source_block_offset") == instance.source_block_offset &&
         row.at("position").get<Vec3>() == instance.position && row.at("look").get<Vec3>() == instance.look &&
         row.at("up").get<Vec3>() == instance.up && row.at("matrix").get<std::array<double, 16>>() == instance.matrix &&
         row.at("use_lighting") == instance.use_lighting,
         "Selection placement differs from parsed RRWORLD1: " + instance.section);
}
void writeNew(const fs::path& path, const Bytes& bytes) {
    need(!fs::exists(path) && !fs::is_symlink(path), "Refusing to overwrite output: " + path.string());
    std::ofstream stream(path, std::ios::binary);
    need(stream.is_open(), "Cannot create output: " + path.string());
    stream.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    stream.close();
    need(stream.good(), "Cannot finish output: " + path.string());
}
struct Options {
    fs::path selection, output;
    bool translated = false;
    Vec3 translation{};
};
Options options(int argc, char** argv) {
    Options result;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--selection" || arg == "--output-dir") {
            need(i + 1 < argc, "Missing path after " + arg);
            auto& path = arg == "--selection" ? result.selection : result.output;
            need(path.empty(), "Duplicate option: " + arg);
            path = argv[++i];
        } else if (arg == "--pod-translation") {
            need(!result.translated && i + 3 < argc, "Supply --pod-translation X Y Z exactly once");
            result.translated = true;
            for (auto& value : result.translation) {
                const std::string text = argv[++i];
                std::size_t used = 0;
                value = std::stod(text, &used);
                need(used == text.size() && std::isfinite(value), "Pod translation must be three finite numbers");
            }
        } else throw std::runtime_error("Unknown option: " + arg);
    }
    need(!result.selection.empty() && !result.output.empty(), "Both --selection and --output-dir are required");
    return result;
}
Json run(const Options& opts) {
    static_assert(sizeof(double) == 8 && std::numeric_limits<double>::is_iec559, "IEEE binary64 required");
    const auto selectionPath = fs::canonical(opts.selection);
    const auto root = selectionPath.parent_path();
    const auto output = fs::weakly_canonical(fs::absolute(opts.output));
    const auto relative = output.lexically_relative(root / "cacheobjects");
    need(!relative.empty() && *relative.begin() == "..", "Output directory may not be the input cache or inside it");
    need(!fs::exists(output) || (fs::is_directory(output) && fs::is_empty(output)), "Output directory must be new or empty");
    need(!fs::is_symlink(opts.output), "Symlink output directory rejected");
    const auto selection = readJson(selectionPath);
    need(selection.at("schema") == "RR_VISUAL_SELECTION49G_V1", "Unsupported selection schema");
    const auto& worldSelection = selection.at("world");
    need(worldSelection.at("container") == taContainer && normalized(worldSelection.at("member")) == "tracks\\ta.scb",
         "Only the selected original TA world is supported");
    const auto worldRecord = readJson(objectPath(root, worldSelection, "record", ".json"));
    need(worldRecord == worldSelection.at("native"), "World record differs from selected metadata");
    need(worldRecord.at("key") == keyOf(worldSelection) && worldRecord.at("source_sha256") == worldSelection.at("source_sha256"),
         "World key/source provenance mismatch");
    assets::World48F world;
    std::string error;
    const auto worldBytes = readBytes(objectPath(root, worldSelection, "rrworld", ".rrworld"), assets::max_world_asset_bytes48f);
    const auto worldAccepted = assets::parseWorldAsset48F(worldBytes, world, error);
    need(worldAccepted, "RRWORLD1 parse failed: " + error);
    need(world.instances.size() == 318 && worldRecord.at("instances") == world.instances.size() &&
         worldRecord.at("zones") == world.zones.size() && worldRecord.at("portals") == world.portals.size(),
         "Selected TA world counters mismatch");

    const auto& requests = selection.at("scene_requests");
    need(requests.is_array() && requests.size() == 231, "Expected 228 TA and three Pod01 scene requests");
    std::map<std::string, Scene> scenes;
    std::map<std::string, std::string> taNames, podFields;
    Json sourceScenes = Json::array();
    std::size_t companionCount = 0;
    for (const auto& request : requests) {
        const auto key = keyOf(request);
        const auto container = request.at("container").get<std::string>();
        const auto member = normalized(request.at("member").get<std::string>());
        need(normalized(request.at("manifest_member").get<std::string>()) == member, "Manifest/request member mismatch");
        need(container == taContainer || container == podContainer, "Unselected source container");
        auto loaded = loadScene(root, request);
        if (container == taContainer) {
            need(taNames.emplace(member, key).second, "Duplicate TA scene member");
            need(loaded.scene.nodes.size() == 1 && loaded.scene.nodes[0].parent == -1 && !loaded.scene.lods.empty(),
                 "TA scene is outside recovered single-root static subset");
        } else {
            need(podFields.emplace(request.at("setup_field").get<std::string>(), key).second, "Duplicate Pod Setup part");
        }
        Json source = request;
        source["parsed_counts"] = meshReport(loaded.bind, loaded.scene.lods[0].surfaces.size(), "embedded_neutral_lod0");
        source["companion_rrmesh_byte_identical"] = loaded.companionVerified;
        companionCount += loaded.companionVerified ? 1 : 0;
        sourceScenes.push_back(std::move(source));
        need(scenes.emplace(key, std::move(loaded)).second, "Duplicate scene key in selected closure");
    }
    need(taNames.size() == 228 && podFields.size() == 3, "Unexpected selected component counts");
    assets::Mesh48B ta;
    Json instances = Json::array();
    std::vector<bool> accounted(world.instances.size(), false);
    std::map<std::string, std::vector<std::size_t>> uses;
    std::set<std::string> usedSceneNames;
    std::size_t previousIndex = 0, replacedRoots = 0, worldSurfaces = 0;
    const auto& visual = selection.at("visual_instances");
    need(visual.is_array() && visual.size() == 284, "Unexpected selected TA visual instance count");
    for (const auto& row : visual) {
        const auto index = row.at("world_instance_index").get<std::size_t>();
        need(index < world.instances.size() && !accounted[index] && (instances.empty() || index > previousIndex),
             "TA visual selection is not unique RRWORLD order");
        previousIndex = index;
        accounted[index] = true;
        const auto& instance = world.instances[index];
        verifyInstance(row, instance);
        need(row.at("container") == taContainer && row.at("status") == "converted_geometry", "Invalid TA instance selection");
        const auto member = normalized(instance.geometry_member);
        const auto key = keyOf(row);
        need(taNames.at(member) == key, "TA instance geometry linkage mismatch");
        usedSceneNames.insert(member);
        uses[key].push_back(index);
        const auto& scene = scenes.at(key).scene;
        if (scene.world_space)
            need(instance.position == Vec3{0, 0, 0} && instance.look == Vec3{0, 0, 1} && instance.up == Vec3{0, 1, 0},
                 "World-space scene placement is outside recovered identity-pose subset");
        for (const auto value : instance.matrix) need(std::isfinite(value), "Nonfinite world placement matrix");
        const bool replaced = nonidentity(scene.nodes[0].local_matrix);
        replacedRoots += replaced ? 1 : 0;
        Json placed = row;
        placed["saved_nonidentity_root_replaced"] = replaced;
        placed["vertex_start"] = ta.vertices.size();
        placed["index_start"] = ta.indices.size();
        placed["surfaces"] = Json::array();
        for (const auto& surface : scene.lods[0].surfaces) {
            const auto vertexStart = static_cast<std::uint32_t>(ta.vertices.size());
            const auto indexStart = ta.indices.size();
            for (const auto& vertex : surface.vertices) {
                need(vertex.bone == 0, "TA static vertex references an unproven bone");
                assets::Vertex48B transformed{};
                for (unsigned c = 0; c < 3; ++c) {
                    // Match import_service48f.cpp:232-250 exactly: ordered
                    // binary64 products/sums, translation, then ONE float cast.
                    // Saved PSG root bind is REPLACED, never composed here.
                    double value = 0;
                    for (unsigned k = 0; k < 3; ++k) value += vertex.position_local[k] * instance.matrix[k * 4 + c];
                    transformed.position[c] = finiteFloat(value + instance.matrix[12 + c]);
                }
                for (unsigned c = 0; c < 2; ++c) transformed.uv[c] = finiteFloat(vertex.uv[c]);
                for (unsigned c = 0; c < 4; ++c) transformed.color[c] = surface.has_original_colors ? vertex.color[c] : 255;
                appendVertex(ta, transformed);
            }
            appendIndices(ta, surface.indices, vertexStart, surface.vertices.size());
            placed["surfaces"].push_back(Json{{"material_index", surface.material_index},
                {"material", scene.materials.at(surface.material_index)}, {"vertex_start", vertexStart},
                {"vertices", surface.vertices.size()}, {"index_start", indexStart}, {"indices", surface.indices.size()},
                {"has_original_colors", surface.has_original_colors}});
            ++worldSurfaces;
        }
        placed["vertices"] = ta.vertices.size() - placed["vertex_start"].get<std::size_t>();
        placed["indices"] = ta.indices.size() - placed["index_start"].get<std::size_t>();
        instances.push_back(std::move(placed));
    }
    const auto& nondrawn = selection.at("non_drawn_instances");
    need(nondrawn.is_array() && nondrawn.size() == 34, "Unexpected non-drawn reference count");
    previousIndex = 0;
    bool firstNondrawn = true;
    for (const auto& row : nondrawn) {
        const auto index = row.at("world_instance_index").get<std::size_t>();
        need(index < world.instances.size() && !accounted[index] && (firstNondrawn || index > previousIndex),
             "Non-drawn metadata is not disjoint RRWORLD order");
        firstNondrawn = false;
        previousIndex = index;
        verifyInstance(row, world.instances[index]);
        need(row.at("status") == "non_drawn", "Invalid non-drawn reference status");
        accounted[index] = true; // Reference metadata only. No payload is opened.
    }
    need(std::all_of(accounted.begin(), accounted.end(), [](bool v) { return v; }) && usedSceneNames.size() == 228,
         "TA instance selection coverage incomplete");
    for (const auto& request : requests) {
        const auto key = keyOf(request);
        need(request.at("world_instance_indices") == Json(uses[key]), "Scene requested instance provenance mismatch");
    }
    need(ta.vertices.size() == 110196 && ta.indices.size() == 209514 && replacedRoots == 42,
         "Original TA native geometry or root-replacement counts changed");

    const std::array<std::pair<const char*, const char*>, 3> partOrder{{
        {"Chariot", "pods\\pod01\\chariot.psg"},
        {"LeftEngine", "pods\\pod01\\englt1.psg"},
        {"RightEngine", "pods\\pod01\\engrt1.psg"}}};
    assets::Mesh48B pod;
    Json parts = Json::array();
    std::size_t podSurfaces = 0;
    for (const auto& part : partOrder) {
        const auto& source = scenes.at(podFields.at(part.first));
        need(normalized(source.request.at("member").get<std::string>()) == part.second,
             "Pod Setup part differs from original three-part selection");
        const auto vertexStart = static_cast<std::uint32_t>(pod.vertices.size());
        const auto indexStart = pod.indices.size();
        for (const auto& vertex : source.bind.vertices) appendVertex(pod, vertex);
        appendIndices(pod, source.bind.indices, vertexStart, source.bind.vertices.size());
        Json record = source.request;
        record["vertex_start"] = vertexStart;
        record["index_start"] = indexStart;
        record["bind_mesh"] = meshReport(source.bind, source.scene.lods[0].surfaces.size(), "embedded_neutral_lod0");
        record["companion_rrmesh_byte_identical"] = source.companionVerified;
        record["hierarchical_bind_preserved"] = true;
        parts.push_back(std::move(record));
        podSurfaces += source.scene.lods[0].surfaces.size();
    }
    need(pod.vertices.size() == 5755 && pod.indices.size() == 10713, "Original Pod01 bind counts changed");
    auto taBytes = encodeMesh(ta);
    auto podBytes = encodeMesh(pod);
    verifyRoundtrip(taBytes);
    verifyRoundtrip(podBytes);
    Json report{{"schema", "RR_AUTHORED_MESH49G_V1"}, {"success", true},
                {"selection", selectionPath.string()}, {"selection_schema", selection.at("schema")},
                {"source_world", worldSelection}, {"source_scenes", sourceScenes},
                {"source_instances", world.instances.size()}, {"visual_instances", visual.size()},
                {"non_drawn_reference_count", nondrawn.size()}, {"unique_TA_scene_names", usedSceneNames.size()},
                {"companion_rrmesh_verified", companionCount}, {"replaced_root_count", replacedRoots},
                {"world_instances", instances}, {"non_drawn_references", nondrawn},
                {"ta_world", meshReport(ta, worldSurfaces, "ta-world.rrmesh")},
                {"pod_bind", meshReport(pod, podSurfaces, "pod01-bind.rrmesh")},
                {"pod_parts", parts}, {"pod_placed", nullptr},
                {"all_positions_and_uvs_finite", true},
                {"format", {{"magic", "RRMESH01"}, {"header_bytes", 16}, {"vertex_bytes", 24},
                            {"index_bytes", 4}, {"byte_order", "little-endian"}}},
                {"policy", {{"TA_transform", "RRWORLD1 binary64 matrix replaces PSG root; ordered k=0..2 sums, translation, single float32 cast"},
                            {"pod_bind", "Existing validated RRSCENE1 neutral_lod0 preserves original hierarchical bind; three original Pod Setup parts"},
                            {"pod_placement", "Only explicit CLI translation; identity orientation; diagnostic display placement, not recovered race placement"},
                            {"appearance", "Original UV and vertex color values retained; missing original colors become neutral white; no added geometry or animation"},
                            {"non_drawn", "Reference metadata retained only; no nonvisual payload opened"},
                            {"hashes", "Per-file hashes are verified and reported externally"}}}};
    Bytes placedBytes;
    if (opts.translated) {
        assets::Mesh48B placed;
        placed.indices = pod.indices;
        placed.vertices.reserve(pod.vertices.size());
        for (auto vertex : pod.vertices) {
            for (unsigned c = 0; c < 3; ++c)
                vertex.position[c] = finiteFloat(static_cast<double>(vertex.position[c]) + opts.translation[c]);
            appendVertex(placed, vertex);
        }
        placedBytes = encodeMesh(placed);
        verifyRoundtrip(placedBytes);
        report["pod_placed"] = meshReport(placed, podSurfaces, "pod01-placed.rrmesh");
        report["pod_placed"]["translation"] = opts.translation;
        report["pod_placed"]["orientation"] = "identity";
        report["pod_placed"]["placement_source"] = "explicit_cli_diagnostic_translation";
    }
    // Validation and serialization finish before any derived file is created.
    fs::create_directories(output);
    writeNew(output / "ta-world.rrmesh", taBytes);
    writeNew(output / "pod01-bind.rrmesh", podBytes);
    if (opts.translated) writeNew(output / "pod01-placed.rrmesh", placedBytes);
    // Re-read output bytes through the retained native parser as final evidence.
    for (const auto& filename : {"ta-world.rrmesh", "pod01-bind.rrmesh"})
        verifyRoundtrip(readBytes(output / filename, assets::max_asset_file_bytes));
    if (opts.translated) verifyRoundtrip(readBytes(output / "pod01-placed.rrmesh", assets::max_asset_file_bytes));
    const auto text = report.dump(2) + "\n";
    writeNew(output / "export-report.json", Bytes(text.begin(), text.end()));
    return Json{{"success", true}, {"report", (output / "export-report.json").string()},
                {"ta_world", report["ta_world"]}, {"pod_bind", report["pod_bind"]},
                {"pod_placed", report["pod_placed"]}, {"replaced_root_count", replacedRoots}};
}
} // namespace
int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Usage: export_authored49g --selection FILE --output-dir NEW_OR_EMPTY_DIR [--pod-translation X Y Z]\n"
                     "Without --pod-translation, exports TA authored world and Pod01 original bind only.\n"
                     "Explicit translation additionally exports a diagnostic Pod01 placement with identity orientation.\n";
        return 0;
    }
    try {
        std::cout << run(options(argc, argv)).dump(2) << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << Json{{"success", false}, {"error", error.what()}}.dump() << '\n';
        return 1;
    }
}
