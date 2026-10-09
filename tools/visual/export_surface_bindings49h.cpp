// Offline 49H diagnostic binding manifest. All selected inputs remain read-only.
// Build with C++17, -fno-fast-math -ffp-contract=off. No renderer or original shader.
#include "../../src/assets/scene_asset48d.h"
#include "../../src/assets/native_asset48b.h"
#include "../../third_party/native-json/json.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "49H parity requires finite ordered arithmetic"
#endif

namespace {
namespace fs = std::filesystem;
namespace assets = rr::assets;
using Json = nlohmann::json;
using Bytes = std::vector<std::uint8_t>;
constexpr const char* ta = "TRACKS/TA.RES";
constexpr const char* pod = "PODS/POD01/POD01.RES";
void need(bool ok, const std::string& text) { if (!ok) throw std::runtime_error(text); }

// Standard SHA-256 used only for immutable offline input/output provenance.
// Independent Python hashlib controls cover selected payloads, canonical
// metadata and multi-megabyte meshes in test_surface_bindings49h.py.
std::uint32_t rotr(std::uint32_t x, unsigned n) { return (x >> n) | (x << (32 - n)); }
std::string sha256(const Bytes& input) {
    static const std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    std::array<std::uint32_t,8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    Bytes data(input);
    const auto bits = static_cast<std::uint64_t>(data.size()) * 8;
    data.push_back(0x80);
    while (data.size() % 64 != 56) data.push_back(0);
    for (int i = 7; i >= 0; --i) data.push_back(static_cast<std::uint8_t>(bits >> (i * 8)));
    for (std::size_t at = 0; at < data.size(); at += 64) {
        std::uint32_t w[64]{};
        for (unsigned i = 0; i < 16; ++i)
            for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | data[at+i*4+j];
        for (unsigned i = 16; i < 64; ++i) {
            const auto s0 = rotr(w[i-15],7) ^ rotr(w[i-15],18) ^ (w[i-15] >> 3);
            const auto s1 = rotr(w[i-2],17) ^ rotr(w[i-2],19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        auto v = h;
        for (unsigned i = 0; i < 64; ++i) {
            const auto s1 = rotr(v[4],6) ^ rotr(v[4],11) ^ rotr(v[4],25);
            const auto ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
            const auto t1 = v[7] + s1 + ch + k[i] + w[i];
            const auto s0 = rotr(v[0],2) ^ rotr(v[0],13) ^ rotr(v[0],22);
            const auto maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
            for (unsigned j = 7; j > 0; --j) v[j] = v[j-1];
            v[4] += t1; v[0] = t1 + s0 + maj;
        }
        for (unsigned i = 0; i < 8; ++i) h[i] += v[i];
    }
    std::ostringstream stream;
    stream << std::hex << std::setfill('0');
    for (const auto word : h) stream << std::setw(8) << word;
    return stream.str();
}
std::string sha256(const std::string& text) { return sha256(Bytes(text.begin(), text.end())); }
bool hash(const std::string& text) {
    return text.size() == 64 && text.find_first_not_of("0123456789abcdef") == std::string::npos;
}
std::string normalized(std::string text) {
    for (auto& c : text) { if (c == '\\') c = '/'; if (c >= 'a' && c <= 'z') c -= 'a' - 'A'; }
    return text;
}
std::uint32_t number(const Json& value, const char* field, std::uint32_t limit) {
    const auto& n = value.at(field);
    need(n.is_number_unsigned() && n.get<std::uint64_t>() <= limit, std::string("Invalid scalar: ") + field);
    return n.get<std::uint32_t>();
}
Bytes read(const fs::path& path, std::size_t limit = 16u * 1024u * 1024u) {
    need(!fs::is_symlink(path), "Symlink input rejected: " + path.string());
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    need(file.is_open(), "Cannot open input: " + path.string());
    const auto size = file.tellg();
    need(size > 0 && static_cast<std::uintmax_t>(size) <= limit, "Input byte limit");
    Bytes bytes(static_cast<std::size_t>(size)); file.seekg(0);
    file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    need(file.good() && file.peek() == std::char_traits<char>::eof(), "Input read/extent failure");
    return bytes;
}
Json parse(const Bytes& bytes) { return Json::parse(bytes.begin(), bytes.end()); }
std::string outputRelative(const fs::path& file, const fs::path& out) {
    return fs::relative(file, out.parent_path()).generic_string();
}
bool sameFloat(float a, float b) { return std::memcmp(&a, &b, sizeof(float)) == 0; }
struct Inputs {
    fs::path root;
    std::map<std::string,Json> objects;
    std::set<std::string> verified;
    std::uint64_t verifiedBytes = 0;
    Bytes selected(const std::string& path) {
        const auto found = objects.find(path);
        need(found != objects.end(), "Unselected object path: " + path);
        const auto bytes = read(root / path, assets::max_scene_bytes48d);
        need(found->second.at("bytes") == bytes.size() && found->second.at("sha256") == sha256(bytes),
             "Selected object hash/extent mismatch: " + path);
        if (verified.insert(path).second) verifiedBytes += bytes.size();
        return bytes;
    }
    Json record(const std::string& key) {
        need(hash(key), "Invalid object key");
        auto result = parse(selected("cacheobjects/" + key + ".json"));
        auto fields = result;
        const auto digest = fields.at("record_sha256").get<std::string>(); fields.erase("record_sha256");
        need(result.at("key") == key && hash(digest) && sha256(fields.dump()) == digest,
             "Native canonical record hash mismatch");
        const auto source = result.at("source_sha256").get<std::string>();
        need(hash(source) && sha256(result.at("converter").get<std::string>() + "\n" + source) == key,
             "Native content key mismatch");
        return result;
    }
};
using BindingMap = std::map<std::pair<std::string,std::string>,Json>;
std::pair<std::string,std::string> association(const Json& row) {
    return {normalized(row.at("container")), normalized(row.at("name"))};
}
void resolution(const Json& row, const char* extension, const char* expected) {
    const auto wanted = normalized("textures/" + row.at("name").get<std::string>() + extension);
    need(row.at("requested_member") == wanted && row.at("expected_status") == expected,
         "Resolution requested name/type mismatch");
    need(row.at("candidates").is_array(), "Resolution candidate array");
    const auto local = number(row, "local_candidates", 100000);
    const auto global = number(row, "global_candidates", 100000);
    need(row.at("scope") == (local ? "local" : "global") && local <= global &&
         row.at("candidates").size() == (local ? local : global), "Resolution scope/count mismatch");
    std::set<std::string> sources;
    bool unsupported = false;
    for (const auto& c : row.at("candidates")) {
        need(normalized(c.at("member")) == wanted, "Resolution candidate member mismatch");
        if (local) need(normalized(c.at("container")) == normalized(row.at("container")), "Local candidate container mismatch");
        sources.insert(c.at("source_sha256")); unsupported |= c.at("status") != expected;
    }
    if (row.at("status") == "resolved") {
        need(!row.at("candidates").empty() && sources.size() == 1 && !unsupported,
             "Resolved candidate ambiguity/unsupported status");
        const auto& selected = row.at("selected");
        need(std::find(row.at("candidates").begin(), row.at("candidates").end(), selected) != row.at("candidates").end()
             && selected.at("key") == row.at("key"), "Selected resolution candidate mismatch");
    } else {
        need(row.at("status") == "unresolved", "Unknown resolution status");
        const std::string reason = row.at("reason");
        need((row.at("candidates").empty() && reason == "no_exact_member") ||
             (sources.size() == 1 && unsupported && reason == "original_resource_unsupported"),
             "Unproven neutral fallback reason");
    }
}
struct Scene { Json request; assets::Scene48D native; assets::Mesh48B bind; };
struct Mesh { fs::path path; Bytes bytes; assets::Mesh48B native; Json color; };
struct Options { fs::path selection, report, output; };
Options options(int argc, char** argv) {
    Options result;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        need(i + 1 < argc, "Missing option value");
        fs::path* target = arg == "--selection" ? &result.selection : arg == "--export-report" ? &result.report :
                          arg == "--output" ? &result.output : nullptr;
        need(target && target->empty(), "Unknown or duplicate option: " + arg); *target = argv[++i];
    }
    need(!result.selection.empty() && !result.report.empty() && !result.output.empty(),
         "Required: --selection FILE --export-report FILE --output FILE");
    return result;
}
Json run(const Options& opts) {
    const auto selectionPath = fs::canonical(opts.selection), reportPath = fs::canonical(opts.report);
    const auto output = fs::weakly_canonical(fs::absolute(opts.output));
    need(!fs::exists(output) && !fs::is_symlink(opts.output), "Output must be new");
    need(fs::is_directory(output.parent_path()), "Output parent directory must exist");
    for (const auto& inputRoot : {selectionPath.parent_path(), reportPath.parent_path()}) {
        const auto rel = output.lexically_relative(inputRoot);
        need(!rel.empty() && *rel.begin() == "..", "Output must not be in immutable source directory");
    }
    const auto selectionBytes = read(selectionPath), reportBytes = read(reportPath);
    const auto selection = parse(selectionBytes), report = parse(reportBytes);
    need(selection.at("schema") == "RR_VISUAL_SELECTION49G_V1" && selection.at("status") == "complete" &&
         report.at("schema") == "RR_AUTHORED_MESH49G_V1" && report.at("success") == true, "Input schema/status mismatch");
    Inputs inputs{selectionPath.parent_path(), {}, {}, 0};
    need(!fs::is_symlink(inputs.root / "cacheobjects"), "Cache directory symlink rejected");
    need(selection.at("objects").is_array() && selection.at("objects").size() == 859, "Expected selected 859-object closure");
    std::uint64_t declaredSelectedBytes = 0;
    for (const auto& row : selection.at("objects")) {
        const auto path = row.at("path").get<std::string>();
        const fs::path p(path);
        const auto suffix = p.extension().string();
        const auto filename = p.filename().string();
        const bool plain = (suffix == ".json" || suffix == ".rrscene" || suffix == ".rrmesh" || suffix == ".rrworld") && filename.size() == 64 + suffix.size();
        const bool mip = suffix == ".rrtex" && filename.size() == 75 && filename.substr(64, 4) == "-mip" && filename[68] >= '0' && filename[68] <= '6';
        need(p.parent_path() == "cacheobjects" && hash(filename.substr(0,64)) && (plain || mip),
             "Nonvisual or unsafe selected path");
        need(hash(row.at("sha256")) && inputs.objects.emplace(path, row).second, "Duplicate selected object/hash");
        declaredSelectedBytes += number(row, "bytes", static_cast<std::uint32_t>(assets::max_scene_bytes48d));
        need(declaredSelectedBytes <= 32u * 1024u * 1024u, "Selected aggregate byte limit");
    }
    for (const auto& pair : inputs.objects) inputs.selected(pair.first);
    BindingMap textureResolution, materials;
    for (const auto& row : selection.at("texture_resolution")) {
        resolution(row, ".pst", "converted_texture");
        need(textureResolution.emplace(association(row), row).second, "Duplicate texture association");
    }
    Json textures = Json::array();
    std::set<std::string> addedTextures;
    for (const auto& row : selection.at("material_resolution")) {
        resolution(row, ".psm", "converted_material");
        need(row.at("status") == "resolved", "Unresolved material outside selected scope");
        const std::string key = row.at("key");
        const auto record = inputs.record(key);
        need(row.at("record") == "cacheobjects/" + key + ".json" && record.at("type") == "material" &&
             record.at("converter") == "rr-native-material-48E-v1" && record.at("source_sha256") == row.at("selected").at("source_sha256"),
             "Material record identity mismatch");
        const auto& passes = record.at("passes");
        need(passes.is_array() && !passes.empty() && passes.size() <= 8, "Material pass count");
        unsigned passNumber = 0;
        Json ignored = Json::array();
        for (const auto& pass : passes) {
            need(number(pass, "pass", 8) == ++passNumber && pass.at("program").is_string(), "Material pass/program ordering");
            unsigned slotNumber = 0;
            need(pass.at("textures").is_array() && !pass.at("textures").empty() && pass.at("textures").size() <= 2, "Material slot count");
            for (const auto& slot : pass.at("textures")) {
                const std::string name = slot.at("name");
                need(number(slot, "slot", 2) == ++slotNumber && !name.empty() &&
                     name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") == std::string::npos,
                     "Material texture slot/name ordering");
                if (passNumber != 1 || slotNumber != 1)
                    ignored.push_back({{"pass", passNumber}, {"slot", slotNumber}, {"name", name}, {"program", pass.at("program")}});
            }
        }
        const auto& pass = passes.at(0);
        const auto textureName = pass.at("textures").at(0).at("name").get<std::string>();
        const auto found = textureResolution.find({normalized(row.at("container")), normalized(textureName)});
        need(found != textureResolution.end(), "Missing selected first-pass texture association");
        const auto& resolved = found->second;
        bool requester = false;
        for (const auto& r : resolved.at("requested_by"))
            if (r.at("material_key") == key && r.at("material_name") == row.at("name") && r.at("pass") == 1 && r.at("slot") == 1 && r.at("program") == pass.at("program")) requester = true;
        need(requester, "Texture association requester mismatch");
        Json binding{{"material_key", key}, {"material", row.at("name")}, {"material_container", row.at("container")},
                     {"material_record_sha256", record.at("record_sha256")}, {"pass", 1}, {"slot", 1},
                     {"program_unexecuted", pass.at("program")}, {"texture_name", textureName},
                     {"texture_id", nullptr}, {"status", "neutral_fallback"}, {"fallback_reason", nullptr},
                     {"additional_passes_maps_unexecuted", ignored}};
        if (resolved.at("status") != "resolved") binding["fallback_reason"] = resolved.at("reason");
        else {
            const std::string textureKey = resolved.at("key");
            const auto textureRecord = inputs.record(textureKey);
            need(textureRecord == resolved.at("native") && textureRecord.at("type") == "texture" &&
                 textureRecord.at("converter") == "rr-native-texture-48E-v1" &&
                 textureRecord.at("source_sha256") == resolved.at("selected").at("source_sha256"), "Texture record identity mismatch");
            const auto& mips = textureRecord.at("mips");
            need(mips.is_array() && !mips.empty() && mips.size() <= 7, "Texture mip count");
            unsigned mi = 0, lastW = 0, lastH = 0;
            for (const auto& mip : mips) {
                const auto width = number(mip, "width", 1024), height = number(mip, "height", 1024);
                need(number(mip, "index", 6) == mi && width >= 16 && height >= 16 &&
                     (!mi || (width == lastW * 2 && height == lastH * 2)), "Ascending texture mip metadata");
                lastW = width; lastH = height; ++mi;
            }
            const auto& mip = mips.back();
            const auto rel = "cacheobjects/" + textureKey + "-mip" + std::to_string(mi - 1) + ".rrtex";
            const auto bytes = inputs.selected(rel);
            assets::Texture48B native; std::string error;
            need(assets::parseTexture48B(bytes.data(), bytes.size(), native, error), "Native texture parse: " + error);
            need(native.width == lastW && native.height == lastH && sha256(bytes) == mip.at("sha256"), "Largest texture hash/dimensions mismatch");
            binding["texture_id"] = textureKey; binding["status"] = "textured";
            if (addedTextures.insert(textureKey).second)
                textures.push_back({{"id", textureKey}, {"path", outputRelative(inputs.root / rel, output)},
                    {"sha256", sha256(bytes)}, {"mip_index", mi - 1}, {"width", native.width}, {"height", native.height},
                    {"rgba_bytes", native.rgba.size()}, {"record_sha256", textureRecord.at("record_sha256")},
                    {"source_container", resolved.at("selected").at("container")}, {"source_member", resolved.at("selected").at("member")},
                    {"source_sha256", textureRecord.at("source_sha256")}});
        }
        need(materials.emplace(association(row), std::move(binding)).second, "Duplicate material association");
    }
    need(materials.size() == 35, "Expected 35 material associations");
    std::map<std::string,Scene> scenes;
    for (const auto& row : selection.at("scene_requests")) {
        const std::string key = row.at("key");
        need(row.at("status") == "converted_geometry" && (row.at("container") == ta || row.at("container") == pod), "Selected scene scope");
        const auto record = inputs.record(key);
        need(record == row.at("native") && record.at("source_sha256") == row.at("source_sha256"), "Scene record identity");
        const auto sceneRel = "cacheobjects/" + key + ".rrscene", meshRel = "cacheobjects/" + key + ".rrmesh";
        need(row.at("rrscene") == sceneRel && row.at("rrmesh") == meshRel && row.at("record") == "cacheobjects/" + key + ".json", "Scene selected paths");
        const auto bytes = inputs.selected(sceneRel);
        Scene s; s.request = row; std::string error;
        need(sha256(bytes) == record.at("scene_sha256") && assets::parseSceneAsset48D(bytes, s.native, error), "Native scene parse/hash: " + error);
        const auto companion = inputs.selected(meshRel);
        need(companion == s.native.neutral_lod0 && sha256(companion) == record.at("mesh_sha256") &&
             assets::parseMesh48B(companion.data(), companion.size(), s.bind, error), "Scene companion parity/parse");
        need(record.at("materials") == Json(s.native.materials) && record.at("vertices") == s.bind.vertices.size() &&
             record.at("triangles") == s.bind.indices.size() / 3, "Scene semantic metadata parity");
        need(scenes.emplace(key, std::move(s)).second, "Duplicate selected scene key");
    }
    need(scenes.size() == 231, "Expected 231 selected scenes");
    std::map<std::string,Mesh> meshes;
    Json meshRows = Json::array();
    for (const auto& id : {std::string("TA"), std::string("POD01")}) {
        const auto& description = report.at(id == "TA" ? "ta_world" : "pod_placed");
        const std::string filename = id == "TA" ? "ta-world.rrmesh" : "pod01-placed.rrmesh";
        need(description.at("mesh") == filename, "Expected unchanged 49G mesh filename");
        Mesh m; m.path = reportPath.parent_path() / filename; m.bytes = read(m.path, assets::max_asset_file_bytes);
        m.color = id == "TA" ? Json{0.57,0.63,0.68,1.0} : Json{0.97,0.58,0.15,1.0};
        std::string error;
        need(assets::parseMesh48B(m.bytes.data(), m.bytes.size(), m.native, error), "Placed mesh parse: " + error);
        need(description.at("vertices") == m.native.vertices.size() && description.at("indices") == m.native.indices.size()
             && description.at("bytes") == m.bytes.size(), "Placed mesh description mismatch");
        meshRows.push_back({{"id", id}, {"path", outputRelative(m.path, output)}, {"sha256", sha256(m.bytes)},
            {"vertices", m.native.vertices.size()}, {"indices", m.native.indices.size()},
            {"triangles", m.native.indices.size()/3}, {"fallback_rgba", m.color}});
        meshes.emplace(id, std::move(m));
    }
    need(report.at("world_instances").size() == 284 && selection.at("visual_instances").size() == 284 &&
         report.at("pod_parts").size() == 3, "Selected instance/part count");
    need(report.at("pod_placed").at("orientation") == "identity", "Pod diagnostic orientation");
    const auto translation = report.at("pod_placed").at("translation").get<std::array<double,3>>();
    for (const auto value : translation) need(std::isfinite(value), "Finite Pod translation required");
    Json spans = Json::array();
    std::set<std::string> activeTextures;
    std::map<std::string,std::uint32_t> nextV, nextI;
    auto append = [&](const Json& instance, const std::string& id) {
        const std::string key = instance.at("key");
        const auto found = scenes.find(key); need(found != scenes.end(), "Instance scene key missing");
        const auto& s = found->second;
        const auto& target = meshes.at(id);
        need(instance.at("container") == (id == "TA" ? ta : pod) && instance.at("container") == s.request.at("container") &&
             normalized(instance.at(id == "TA" ? "geometry_member" : "member")) == normalized(s.request.at("member")), "Instance exact scene member mismatch");
        const auto vb = number(instance, "vertex_start", assets::max_mesh_vertices);
        const auto ib = number(instance, "index_start", assets::max_mesh_indices);
        need(vb == nextV[id] && ib == nextI[id], "Instance offset discontinuity");
        if (id == "TA") need(instance.at("surfaces").size() == s.native.spans_lod0.size(), "TA surface count mismatch");
        const auto matrix = id == "TA" ? instance.at("matrix").get<std::array<double,16>>() : std::array<double,16>{};
        for (std::size_t j = 0; j < s.native.spans_lod0.size(); ++j) {
            const auto& span = s.native.spans_lod0[j]; const auto& surf = s.native.lods.at(0).surfaces.at(j);
            const auto vstart = vb + span.vertex_start, istart = ib + span.index_start;
            need(vstart == nextV[id] && istart == nextI[id] &&
                 span.vertex_count <= target.native.vertices.size() - std::min<std::size_t>(vstart, target.native.vertices.size()) &&
                 span.index_count <= target.native.indices.size() - std::min<std::size_t>(istart, target.native.indices.size()), "Surface extent/discontinuity");
            const auto& name = s.native.materials.at(span.material_index);
            if (id == "TA") {
                const auto& r = instance.at("surfaces").at(j);
                need(r.at("material") == name && r.at("material_index") == span.material_index && r.at("vertex_start") == vstart &&
                     r.at("index_start") == istart && r.at("vertices") == span.vertex_count && r.at("indices") == span.index_count &&
                     r.at("has_original_colors") == span.has_original_colors, "TA reported material/span mismatch");
            }
            for (std::size_t vi = 0; vi < surf.vertices.size(); ++vi) {
                const auto& actual = target.native.vertices.at(vstart + vi);
                const auto& authored = surf.vertices[vi];
                const auto& bound = s.bind.vertices.at(span.vertex_start + vi);
                for (unsigned c = 0; c < 2; ++c)
                    need(sameFloat(actual.uv[c], static_cast<float>(authored.uv[c])), "UV association parity mismatch");
                for (unsigned c = 0; c < 4; ++c)
                    need(actual.color[c] == (surf.has_original_colors ? authored.color[c] : 255), "Source color parity mismatch");
                for (unsigned c = 0; c < 3; ++c) {
                    double value = 0;
                    if (id == "TA") {
                        need(authored.bone == 0, "Unproven TA bone");
                        for (unsigned q = 0; q < 3; ++q) value += authored.position_local[q] * matrix[q*4+c];
                        value += matrix[12+c];
                    } else value = static_cast<double>(bound.position[c]) + translation[c];
                    need(std::isfinite(value) && sameFloat(actual.position[c], static_cast<float>(value)), "49G position parity mismatch");
                }
            }
            for (std::size_t ii = 0; ii < surf.indices.size(); ++ii)
                need(target.native.indices.at(istart+ii) == vstart + surf.indices[ii], "Ordered surface index parity mismatch");
            const auto material = materials.find({normalized(instance.at("container")), normalized(name)});
            need(material != materials.end(), "Missing material association");
            Json row = material->second;
            row.update({{"draw_order", spans.size()}, {"mesh_id", id}, {"index_start", istart}, {"index_count", span.index_count},
                        {"vertex_start", vstart}, {"vertex_count", span.vertex_count}, {"scene_key", key},
                        {"scene_member", s.request.at("member")}, {"surface_index", j}, {"material_index", span.material_index},
                        {"instance", instance.at(id == "TA" ? "section" : "setup_field")},
                        {"fallback_rgba", target.color}, {"has_original_colors", span.has_original_colors}});
            row["world_instance_index"] = id == "TA" ? instance.at("world_instance_index") : Json(nullptr);
            if (!row.at("texture_id").is_null()) activeTextures.insert(row.at("texture_id"));
            spans.push_back(std::move(row)); nextV[id] += span.vertex_count; nextI[id] += span.index_count;
        }
    };
    for (std::size_t i = 0; i < 284; ++i) {
        const auto& instance = report.at("world_instances").at(i);
        const auto& selected = selection.at("visual_instances").at(i);
        for (auto it = selected.begin(); it != selected.end(); ++it) need(instance.at(it.key()) == it.value(), "49G instance differs from selected placement");
        append(instance, "TA");
    }
    need(spans.size() == 353, "Expected 353 TA surface spans");
    const std::array<std::string,3> partNames{{"Chariot","LeftEngine","RightEngine"}};
    for (std::size_t i = 0; i < 3; ++i) {
        const auto& instance = report.at("pod_parts").at(i);
        need(instance.at("setup_field") == partNames[i], "Original Pod part order mismatch");
        const auto& request = scenes.at(instance.at("key").get<std::string>()).request;
        need(request.at("setup_field") == partNames[i], "Selected Pod part association mismatch");
        append(instance, "POD01");
    }
    need(spans.size() == 360 && activeTextures.size() == 26 && activeTextures == addedTextures, "Active surface/texture coverage mismatch");
    for (const auto& m : meshes) need(nextV[m.first] == m.second.native.vertices.size() && nextI[m.first] == m.second.native.indices.size(),
                                     "Not every placed vertex/index covered exactly once");
    Json summary{{"total", {{"surfaces", 0}, {"vertices", 0}, {"indices", 0}, {"triangles", 0}}},
                 {"textured", {{"surfaces", 0}, {"vertices", 0}, {"indices", 0}, {"triangles", 0}}},
                 {"fallback", {{"surfaces", 0}, {"vertices", 0}, {"indices", 0}, {"triangles", 0}}}};
    for (const auto& row : spans) for (const auto& key : {std::string("total"), row.at("texture_id").is_null() ? std::string("fallback") : std::string("textured")}) {
        auto& s = summary[key];
        s["surfaces"] = s["surfaces"].get<unsigned>() + 1;
        s["vertices"] = s["vertices"].get<unsigned>() + row.at("vertex_count").get<unsigned>();
        s["indices"] = s["indices"].get<unsigned>() + row.at("index_count").get<unsigned>();
        s["triangles"] = s["triangles"].get<unsigned>() + row.at("index_count").get<unsigned>() / 3;
    }
    std::uint64_t textureBytes = 0, meshBytes = 0;
    for (const auto& t : textures) textureBytes += t.at("rgba_bytes").get<std::uint64_t>();
    for (const auto& m : meshes) meshBytes += m.second.bytes.size() - 16;
    summary["texture_upload_bytes"] = textureBytes; summary["mesh_upload_bytes"] = meshBytes;
    summary["unique_textures"] = textures.size(); summary["ta_spans"] = 353; summary["pod_spans"] = 7;
    need(summary.at("textured").at("surfaces") == 286 && summary.at("fallback").at("surfaces") == 74 &&
         textureBytes == 3940352 && meshBytes == 3663732, "Independent assessment coverage/cost differs");
    Json result{{"schema", "textured49h.bindings.v1"}, {"meshes", meshRows}, {"textures", textures}, {"spans", spans}, {"summary", summary},
        {"provenance", {{"selection_path", outputRelative(selectionPath, output)}, {"selection_sha256", sha256(selectionBytes)},
                        {"export_report_path", outputRelative(reportPath, output)}, {"export_report_sha256", sha256(reportBytes)},
                        {"selected_objects_verified", inputs.verified.size()}, {"selected_bytes_verified", inputs.verifiedBytes},
                        {"limits", {{"selection_json_bytes", 16u * 1024u * 1024u}, {"export_report_json_bytes", 16u * 1024u * 1024u},
                                    {"selected_aggregate_bytes", 32u * 1024u * 1024u}, {"individual_selected_object_bytes", assets::max_scene_bytes48d},
                                    {"individual_placed_mesh_bytes", assets::max_asset_file_bytes}, {"selected_object_count", 859},
                                    {"parsed_scene_count", 231}, {"output_spans", 360}}},
                        {"validated_scene_count", scenes.size()}, {"collision_payload_reads", 0},
                        {"mesh_geometry_uv_color_index_parity", true}}},
        {"policy", {{"purpose", "Authored base-color diagnostic; no original material program is implemented."},
                    {"texture", "First authored pass and slot 1; only largest cached original mip; RGBA8_UNORM; no UV flip or rescaling."},
                    {"sampling", "Port of existing diagnostic linear repeat sampler, largest mip only; not original PSM state conformance."},
                    {"appearance", "Texture RGB multiplied by original vertex RGB; opaque alpha; unresolved texture uses unchanged 49G per-mesh neutral diagnostic color."},
                    {"order", "TA RRWORLD instance and LOD0 surface order, then Chariot/LeftEngine/RightEngine LOD0 order; every index exactly once."},
                    {"not_executed", "Original microprograms, additional passes/maps, alpha blending/testing, specular effects, animated textures/UVs, lighting, original sampler semantics."}}}};
    std::ofstream file(output, std::ios::binary | std::ios::out);
    need(file.is_open(), "Cannot create output"); const auto text = result.dump(2) + "\n";
    file.write(text.data(), static_cast<std::streamsize>(text.size())); file.close(); need(file.good(), "Output write failure");
    return Json{{"success", true}, {"output", output.string()}, {"summary", summary}, {"sha256", sha256(text)}};
}
} // namespace
int main(int argc, char** argv) {
    try { std::cout << run(options(argc, argv)).dump(2) << '\n'; return 0; }
    catch (const std::exception& error) { std::cerr << "49H binding export failed: " << error.what() << '\n'; return 1; }
}
