#pragma once
#include "../../src/assets/material_texture_assets50ea.h"
#include "../../src/assets/vehicle_draw_assets50cr.h"
#include "../../src/assets/scene_asset48d.h"
#include "../../third_party/native-json/json.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Development-only, cold retained-data adapter. No source resource decoding,
// guessed filename lookup, original shader execution, renderer or GL calls.
namespace pod_materials50ed_fixture {
namespace a = rr::assets;
constexpr std::size_t maxColdSourceBytes = 32u * 1024u * 1024u;
constexpr std::uint32_t unresolved = a::unresolvedMaterialTexture50EA;
enum class MipMode : std::uint8_t { unspecified, exactIndex, largestRetained };
struct Choice {
    // Exact native numbers, not vector offsets. Zero is not an authored pass
    // or slot in these Material48E records and is rejected, even if optional.
    std::uint32_t pass = 0, slot = 0;
    a::MaterialTextureRequirement50EA requirement = a::MaterialTextureRequirement50EA::required;
    MipMode mipMode = MipMode::unspecified;
    std::uint32_t mipIndex = 0;
};
struct Request {
    std::filesystem::path root; // Contains asset-selection.json and cacheobjects.
    // Dense native material-index order: chariot, left engine, right engine.
    std::array<std::vector<Choice>, 3> choices;
    std::size_t coldSourceByteCapacity = 0;
    a::MaterialTextureBudgets50EA textureBudgets{};
};
struct Selection {
    std::uint32_t materialIndex = 0, pass = 0, slot = 0;
    std::uint32_t bankSlot = unresolved, textureIndex = unresolved, mipIndex = unresolved;
    std::uint64_t assetIdentity = 0, textureIdentity = 0;
    a::MaterialTextureRequirement50EA requirement = a::MaterialTextureRequirement50EA::required;
    std::string materialName, materialKey, program, textureName, textureKey;
    std::string unresolvedReason;
};
struct AuthoredSlot {
    std::uint32_t component = 0, materialIndex = 0, pass = 0, slot = 0;
    bool selectedForDisplay = false, resolvedInSelection = false;
    std::string materialName, materialKey, program, textureName, textureKey, unresolvedReason;
};
struct TextureSelection {
    std::uint64_t identity = 0;
    std::uint32_t bankIndex = unresolved, mipIndex = unresolved, width = 0, height = 0;
    std::string key, path, sha256;
};
struct Diagnostic {
    std::uint32_t component = 3, material = unresolved, pass = 0, slot = 0;
    std::string message;
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
namespace fs = std::filesystem;
using Json = nlohmann::json;
using Bytes = std::vector<std::uint8_t>;
constexpr const char* pod = "PODS/POD01/POD01.RES";
constexpr const char* sceneKeys[3] = {
    "cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5",
    "210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3",
    "f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60"};
constexpr const char* sceneMembers[3] = {
    "pods\\pod01\\chariot.psg", "pods\\pod01\\englt1.psg", "pods\\pod01\\engrt1.psg"};
inline void need(bool ok, const std::string& why) { if (!ok) throw std::runtime_error(why); }
inline bool hash(const std::string& s) {
    return s.size() == 64 && s.find_first_not_of("0123456789abcdef") == std::string::npos;
}
inline bool name(const std::string& s) {
    return !s.empty() && s.size() <= 128 &&
        s.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") == std::string::npos;
}
inline std::string normalized(std::string s) {
    for (auto& c : s) { if (c == '\\') c = '/'; if (c >= 'a' && c <= 'z') c -= 'a' - 'A'; }
    return s;
}
inline std::uint64_t number(const Json& j, const char* key, std::uint64_t cap) {
    const auto& n = j.at(key);
    need(n.is_number_unsigned() && n.get<std::uint64_t>() <= cap, std::string("Invalid scalar: ") + key);
    return n.get<std::uint64_t>();
}
inline void add(std::size_t& total, std::size_t amount, std::size_t cap, const char* why) {
    need(total <= cap && amount <= cap - total, why); total += amount;
}
inline std::uint32_t rotr(std::uint32_t x, unsigned n) { return (x >> n) | (x << (32 - n)); }
inline std::string sha256(const Bytes& input) {
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
inline std::string sha256(const std::string& text) { return sha256(Bytes(text.begin(), text.end())); }
inline bool safeObjectPath(const std::string& path) {
    if (path.compare(0, 13, "cacheobjects/") != 0) return false;
    const auto filename = path.substr(13);
    if (filename.size() < 64 || !hash(filename.substr(0, 64))) return false;
    const auto suffix = filename.substr(64);
    return suffix == ".json" || suffix == ".rrscene" || suffix == ".rrmesh" || suffix == ".rrworld" ||
        (suffix.size() == 11 && suffix.compare(0, 4, "-mip") == 0 &&
         suffix[4] >= '0' && suffix[4] <= '6' && suffix.substr(5) == ".rrtex");
}
struct Inputs {
    fs::path root;
    std::size_t capacity = 0, sourceBytes = 0;
    std::map<std::string, Json> objects, records;
    Bytes read(const fs::path& path, std::size_t limit) {
        need(!fs::is_symlink(path), "Symlink input rejected: " + path.string());
        std::ifstream f(path, std::ios::binary | std::ios::ate);
        need(f.is_open(), "Cannot open retained input: " + path.string());
        const auto size = f.tellg();
        need(size > 0 && static_cast<std::uintmax_t>(size) <= limit, "Retained input byte limit");
        add(sourceBytes, static_cast<std::size_t>(size), capacity, "Cold source byte budget");
        Bytes b(static_cast<std::size_t>(size)); f.seekg(0);
        f.read(reinterpret_cast<char*>(b.data()), static_cast<std::streamsize>(b.size()));
        need(f.good() && f.peek() == std::char_traits<char>::eof(), "Retained input read/extent failure");
        return b;
    }
    const Json& object(const std::string& path) const {
        need(safeObjectPath(path), "Unsafe retained cache path");
        const auto it = objects.find(path);
        need(it != objects.end(), "Object absent from selected closure: " + path);
        return it->second;
    }
    Bytes selected(const std::string& path, std::size_t limit) {
        const auto& row = object(path);
        need(row.at("extracted") == true, "Selected object was not retained: " + path);
        const auto b = read(root / path, limit);
        need(number(row, "bytes", maxColdSourceBytes) == b.size() && row.at("sha256") == sha256(b),
            "Selected object hash/extent mismatch: " + path);
        return b;
    }
    const Json& record(const std::string& key) {
        need(hash(key), "Invalid retained content key");
        const auto known = records.find(key); if (known != records.end()) return known->second;
        const auto b = selected("cacheobjects/" + key + ".json", 64u * 1024u);
        auto j = Json::parse(b.begin(), b.end()); auto fields = j;
        const auto digest = fields.at("record_sha256").get<std::string>(); fields.erase("record_sha256");
        need(j.at("key") == key && hash(digest) && sha256(fields.dump()) == digest,
            "Native canonical record hash mismatch");
        const auto source = j.at("source_sha256").get<std::string>();
        need(hash(source) && sha256(j.at("converter").get<std::string>() + "\n" + source) == key,
            "Native content key mismatch");
        return records.emplace(key, std::move(j)).first->second;
    }
};
// Same retained 49G/48E schema and provenance checks as 49H, narrowed to the
// exact Pod container. A reported cross-container resolution is rejected.
inline void resolution(const Json& row, const char* extension, const char* status) {
    const auto n = row.at("name").get<std::string>();
    need(name(n) && row.at("container") == pod, "Wrong Pod resolution scope/name");
    const auto wanted = normalized("textures/" + n + extension);
    need(row.at("requested_member") == wanted && row.at("expected_status") == status,
        "Resolution requested member/status mismatch");
    const auto& candidates = row.at("candidates");
    const auto local = number(row, "local_candidates", 4096), global = number(row, "global_candidates", 4096);
    need(candidates.is_array() && local <= global && candidates.size() == (local ? local : global) &&
        row.at("scope") == (local ? "local" : "global"), "Resolution candidate scope/count mismatch");
    std::set<std::string> sources; bool unsupported = false;
    for (const auto& c : candidates) {
        need(normalized(c.at("member")) == wanted, "Resolution candidate exact member mismatch");
        need(normalized(c.at("container")) == pod, "Cross-container candidate rejected");
        const auto source = c.at("source_sha256").get<std::string>();
        need(hash(source), "Invalid candidate source hash");
        sources.insert(source); unsupported |= c.at("status") != status;
    }
    if (row.at("status") == "resolved") {
        need(local && sources.size() == 1 && !unsupported, "Unsupported/nonlocal/ambiguous resolution");
        const auto& selected = row.at("selected");
        need(std::find(candidates.begin(), candidates.end(), selected) != candidates.end() &&
            selected.at("key") == row.at("key") && hash(row.at("key")), "Selected candidate mismatch");
    } else {
        need(row.at("status") == "unresolved", "Unknown resolution status");
        const std::string why = row.at("reason");
        need((candidates.empty() && why == "no_exact_member") ||
            (sources.size() == 1 && unsupported && why == "original_resource_unsupported"),
            "Unproven unresolved resource");
    }
}
inline std::size_t sceneSize(const a::Scene48D& scene) {
    std::size_t size = 96;
    for (const auto& n : scene.nodes) add(size, 320 + n.name.size(), maxColdSourceBytes, "Scene byte bound");
    for (const auto& m : scene.materials) add(size, 4 + m.size(), maxColdSourceBytes, "Scene byte bound");
    for (const auto& lod : scene.lods) {
        add(size, 12, maxColdSourceBytes, "Scene byte bound");
        for (const auto& s : lod.surfaces) {
            need(s.vertices.size() <= 1000000 && s.indices.size() <= 3000000, "Scene counts");
            add(size, 24 + s.vertices.size() * 70 + s.indices.size() * 4, maxColdSourceBytes, "Scene byte bound");
        }
    }
    add(size, 8 + scene.spans_lod0.size() * 24, maxColdSourceBytes, "Scene byte bound");
    add(size, scene.neutral_lod0.size(), maxColdSourceBytes, "Scene byte bound");
    return size;
}
inline std::uint32_t u32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
        (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
} // namespace detail

class Owner final {
    struct Storage {
        a::MaterialTextureAssets50EA bank;
        std::array<std::vector<Selection>, 3> component;
        std::vector<AuthoredSlot> authored;
        std::vector<TextureSelection> textures;
        std::size_t coldBytes = 0;
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner() = default;
    Owner(const Owner&) = delete;
    Owner& operator=(const Owner&) = delete;
    Owner(Owner&&) = delete;
    Owner& operator=(Owner&&) = delete;
    bool loaded() const noexcept { return bool(storage_); }
    const a::MaterialTextureAssets50EA* bank() const noexcept { return storage_ ? &storage_->bank : nullptr; }
    const std::vector<Selection>* component(unsigned c) const noexcept {
        return storage_ && c < 3 ? &storage_->component[c] : nullptr;
    }
    const std::vector<AuthoredSlot>* authoredSlots() const noexcept { return storage_ ? &storage_->authored : nullptr; }
    const std::vector<TextureSelection>* textures() const noexcept { return storage_ ? &storage_->textures : nullptr; }
    std::size_t coldSourceBytes() const noexcept { return storage_ ? storage_->coldBytes : 0; }

    // Load once, all-or-nothing. Error leaves an empty owner retryable. A loaded
    // owner rejects reload without invalidating borrowed bank/metadata views.
    // The caller keeps input choices and 50CR scenes unchanged during this call.
    bool load(const Request& request, const a::VehicleDrawAssets50CR& draw, Diagnostic& d) noexcept {
        using namespace detail;
        try {
            d = {};
            need(!storage_, "Pod materials already loaded");
            need(draw.loaded() && !request.root.empty(), "Loaded 50CR assets and selected root required");
            need(request.coldSourceByteCapacity > 0 && request.coldSourceByteCapacity <= maxColdSourceBytes,
                "Explicit cold source budget required");
            const auto& budget = request.textureBudgets;
            need(budget.textureCapacity <= a::maxMaterialTextureAssets50EA &&
                budget.slotCapacity <= a::maxMaterialTextureSlots50EA &&
                budget.sourceByteCapacity <= a::maxMaterialTextureSourceBytes50EA &&
                budget.rgbaByteCapacity <= a::maxMaterialTextureRGBABytes50EA &&
                budget.dimensionCapacity <= a::max_texture_dimension, "Invalid texture bank budgets");
            Inputs inputs; inputs.root = fs::canonical(request.root); inputs.capacity = request.coldSourceByteCapacity;
            need(!fs::is_symlink(inputs.root / "cacheobjects"), "Cache directory symlink rejected");
            const auto selectionBytes = inputs.read(inputs.root / "asset-selection.json", 4u * 1024u * 1024u);
            const auto selection = Json::parse(selectionBytes.begin(), selectionBytes.end());
            need(selection.at("schema") == "RR_VISUAL_SELECTION49G_V1" && selection.at("status") == "complete",
                "Retained selection schema/status mismatch");
            const auto& objects = selection.at("objects");
            need(objects.is_array() && objects.size() <= 4096, "Selected object table bound");
            for (const auto& row : objects) {
                const auto path = row.at("path").get<std::string>();
                need(safeObjectPath(path) && hash(row.at("sha256")) && row.at("extracted").is_boolean(),
                    "Unsafe selected object path/hash/retention flag");
                number(row, "bytes", maxColdSourceBytes);
                need(inputs.objects.emplace(path, row).second, "Duplicate selected object path");
            }
            std::map<std::string, const Json*> materials, textures;
            for (const auto* table : {"material_resolution", "texture_resolution"}) {
                const auto& rows = selection.at(table);
                need(rows.is_array() && rows.size() <= 4096, "Resolution table bound");
                const bool material = std::string(table) == "material_resolution";
                for (const auto& row : rows) if (row.at("container") == pod) {
                    resolution(row, material ? ".psm" : ".pst", material ? "converted_material" : "converted_texture");
                    auto& index = material ? materials : textures;
                    need(index.emplace(row.at("name").get<std::string>(), &row).second, "Duplicate exact Pod association");
                }
            }
            auto next = std::make_unique<Storage>();
            std::array<const Json*, 3> scenes{};
            const auto& sceneRows = selection.at("scene_requests");
            need(sceneRows.is_array() && sceneRows.size() <= 4096, "Scene request table bound");
            for (const auto& row : sceneRows) if (row.at("container") == pod) {
                unsigned c = 0; while (c < 3 && row.at("key") != sceneKeys[c]) ++c;
                need(c < 3 && !scenes[c] && row.at("member") == sceneMembers[c], "Unexpected/duplicate Pod scene");
                scenes[c] = &row;
            }
            std::size_t slotCount = 0;
            for (unsigned c = 0; c < 3; ++c) {
                d.component = c;
                need(scenes[c], "Missing exact Pod component scene");
                const auto& row = *scenes[c];
                const auto* actual = draw.scene(static_cast<rr::game::VehicleDrawComponent50CM>(c));
                need(actual && actual->materials.size() <= 4096 && request.choices[c].size() == actual->materials.size(),
                    "Choices must cover every original material index");
                add(slotCount, actual->materials.size(), budget.slotCapacity, "Selected slot budget");
                const std::string key = sceneKeys[c], recordPath = "cacheobjects/" + key + ".json";
                const std::string scenePath = "cacheobjects/" + key + ".rrscene";
                const auto& record = inputs.record(key);
                need(row.at("status") == "converted_geometry" && row.at("record") == recordPath &&
                    row.at("rrscene") == scenePath && row.at("native") == record &&
                    record.at("converter") == "rr-native-scene-48D-v1" &&
                    row.at("source_sha256") == record.at("source_sha256") && record.at("materials") == Json(actual->materials),
                    "Actual 50CR scene material provenance mismatch");
                // Serialize the immutable loaded 50CR scene through the existing
                // RRSCENE1 writer, with byte budget checked before allocation.
                // This also rejects same-sized substitute geometry/materials.
                const auto expected = sceneSize(*actual);
                add(inputs.sourceBytes, expected, inputs.capacity, "Cold source scene byte budget");
                Bytes encoded; std::string why;
                need(a::encodeSceneAsset48D(*actual, encoded, why), "Loaded scene re-encoding rejected: " + why);
                const auto& object = inputs.object(scenePath);
                need(encoded.size() == expected && object.at("bytes") == encoded.size() &&
                    object.at("sha256") == sha256(encoded) && record.at("scene_sha256") == object.at("sha256"),
                    "Loaded 50CR scene differs from exact retained component");
                next->component[c].reserve(actual->materials.size());
            }
            std::vector<Bytes> bytes;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            bytes.reserve(slotCount); slots.reserve(slotCount);
            std::map<std::pair<std::string, std::uint32_t>, std::uint32_t> textureIndexes;
            std::size_t textureSourceBytes = 0, rgbaBytes = 0;
            for (unsigned c = 0; c < 3; ++c) {
                const auto* actual = draw.scene(static_cast<rr::game::VehicleDrawComponent50CM>(c));
                for (std::uint32_t mi = 0; mi < actual->materials.size(); ++mi) {
                    const auto& choice = request.choices[c][mi];
                    d.component = c; d.material = mi; d.pass = choice.pass; d.slot = choice.slot;
                    need(choice.pass > 0 && choice.pass <= 8 && choice.slot > 0 && choice.slot <= 2 &&
                        (choice.requirement == a::MaterialTextureRequirement50EA::required ||
                         choice.requirement == a::MaterialTextureRequirement50EA::optionalUntextured) &&
                        (choice.mipMode == MipMode::exactIndex || choice.mipMode == MipMode::largestRetained) &&
                        choice.mipIndex <= 6, "Explicit native pass/slot, requirement and mip choice required");
                    Selection out; out.materialIndex = mi; out.assetIdentity = c + 1;
                    out.pass = choice.pass; out.slot = choice.slot; out.requirement = choice.requirement;
                    out.materialName = actual->materials[mi]; out.bankSlot = static_cast<std::uint32_t>(slots.size());
                    // Absent resource identities cannot collide with dense present identities.
                    out.textureIdentity = (std::uint64_t(1) << 63) | out.bankSlot;
                    const auto material = materials.find(out.materialName);
                    need(material != materials.end(), "Missing exact scene-to-material association");
                    const auto& mr = *material->second;
                    need(mr.at("status") == "resolved", "Unresolved material record");
                    out.materialKey = mr.at("key").get<std::string>();
                    const auto& record = inputs.record(out.materialKey);
                    need(mr.at("record") == "cacheobjects/" + out.materialKey + ".json" &&
                        record.at("type") == "material" && record.at("converter") == "rr-native-material-48E-v1" &&
                        record.at("source_sha256") == mr.at("selected").at("source_sha256"), "Material record identity mismatch");
                    std::size_t uses = 0;
                    need(mr.at("requested_by").is_array(), "Material requester array");
                    for (const auto& r : mr.at("requested_by")) if (r.at("scene_key") == sceneKeys[c]) {
                        need(r.at("scene_member") == sceneMembers[c], "Material requester exact member mismatch"); ++uses;
                    }
                    need(uses == static_cast<std::size_t>(std::count(actual->materials.begin(), actual->materials.end(), out.materialName)),
                        "Material requester multiplicity mismatch");
                    const auto& passes = record.at("passes");
                    need(passes.is_array() && !passes.empty() && passes.size() <= 8, "Material pass count");
                    const Json* chosenTexture = nullptr;
                    std::uint32_t passNumber = 0;
                    for (const auto& pass : passes) {
                        need(number(pass, "pass", 8) == ++passNumber && pass.at("program").is_string(), "Native material pass ordering");
                        const auto& maps = pass.at("textures");
                        need(maps.is_array() && !maps.empty() && maps.size() <= 2, "Native material map count");
                        std::uint32_t slotNumber = 0;
                        for (const auto& map : maps) {
                            need(number(map, "slot", 2) == ++slotNumber && name(map.at("name")), "Native material map ordering/name");
                            const auto texture = textures.find(map.at("name").get<std::string>());
                            need(texture != textures.end(), "Missing exact material-to-texture association");
                            const auto& tr = *texture->second; unsigned requesters = 0;
                            need(tr.at("requested_by").is_array(), "Texture requester array");
                            for (const auto& r : tr.at("requested_by"))
                                if (r.at("material_key") == out.materialKey && r.at("material_name") == out.materialName &&
                                    r.at("pass") == passNumber && r.at("slot") == slotNumber && r.at("program") == pass.at("program")) ++requesters;
                            need(requesters == 1, "Texture requester exact pass/slot/program mismatch");
                            AuthoredSlot authored; authored.component = c; authored.materialIndex = mi;
                            authored.pass = passNumber; authored.slot = slotNumber;
                            authored.materialName = out.materialName; authored.materialKey = out.materialKey;
                            authored.program = pass.at("program").get<std::string>(); authored.textureName = map.at("name").get<std::string>();
                            authored.selectedForDisplay = choice.pass == passNumber && choice.slot == slotNumber;
                            authored.resolvedInSelection = tr.at("status") == "resolved";
                            if (authored.resolvedInSelection) authored.textureKey = tr.at("key").get<std::string>();
                            else authored.unresolvedReason = tr.at("reason").get<std::string>();
                            if (authored.selectedForDisplay) {
                                chosenTexture = &tr; out.program = authored.program; out.textureName = authored.textureName;
                                out.textureKey = authored.textureKey; out.unresolvedReason = authored.unresolvedReason;
                            }
                            next->authored.push_back(std::move(authored));
                        }
                    }
                    if (!chosenTexture) out.unresolvedReason = "native_pass_or_slot_absent";
                    if (chosenTexture && chosenTexture->at("status") == "resolved") {
                        const auto& tr = *chosenTexture; const auto& record = inputs.record(out.textureKey);
                        need(record == tr.at("native") && record.at("type") == "texture" &&
                            record.at("converter") == "rr-native-texture-48E-v1" &&
                            record.at("source_sha256") == tr.at("selected").at("source_sha256"), "Texture record identity mismatch");
                        const auto& mips = record.at("mips");
                        need(mips.is_array() && !mips.empty() && mips.size() <= 7, "Retained mip count");
                        const Json* selectedMip = nullptr; std::uint64_t largest = 0;
                        std::uint32_t mipNumber = 0, lastWidth = 0, lastHeight = 0;
                        for (const auto& mip : mips) {
                            const auto index = number(mip, "index", 6);
                            const auto w = static_cast<std::uint32_t>(number(mip, "width", 1024));
                            const auto h = static_cast<std::uint32_t>(number(mip, "height", 1024));
                            need(index == mipNumber++ && w >= 16 && h >= 16 &&
                                (!index || (w == lastWidth * 2 && h == lastHeight * 2)) && hash(mip.at("sha256")), "Retained mip ordering/metadata");
                            lastWidth = w; lastHeight = h;
                            const std::string path = "cacheobjects/" + out.textureKey + "-mip" + std::to_string(index) + ".rrtex";
                            const auto found = inputs.objects.find(path);
                            const bool retained = found != inputs.objects.end() && found->second.at("extracted") == true;
                            if (choice.mipMode == MipMode::exactIndex && index == choice.mipIndex && retained) selectedMip = &mip;
                            if (choice.mipMode == MipMode::largestRetained && retained && std::uint64_t(w) * h > largest) {
                                largest = std::uint64_t(w) * h; selectedMip = &mip;
                            }
                        }
                        if (!selectedMip) out.unresolvedReason = "requested_mip_not_retained";
                        else {
                            out.mipIndex = static_cast<std::uint32_t>(number(*selectedMip, "index", 6));
                            const auto key = std::make_pair(out.textureKey, out.mipIndex);
                            const auto known = textureIndexes.find(key);
                            if (known != textureIndexes.end()) out.textureIndex = known->second;
                            else {
                                need(bytes.size() < budget.textureCapacity, "Texture count budget");
                                TextureSelection meta; meta.key = out.textureKey; meta.mipIndex = out.mipIndex;
                                meta.bankIndex = static_cast<std::uint32_t>(bytes.size()); meta.identity = std::uint64_t(meta.bankIndex) + 1;
                                meta.width = static_cast<std::uint32_t>(number(*selectedMip, "width", 1024));
                                meta.height = static_cast<std::uint32_t>(number(*selectedMip, "height", 1024));
                                need(meta.width <= budget.dimensionCapacity && meta.height <= budget.dimensionCapacity, "Texture dimension budget");
                                const auto rgba = std::size_t(meta.width) * meta.height * 4;
                                add(rgbaBytes, rgba, budget.rgbaByteCapacity, "Decoded RGBA byte budget");
                                add(textureSourceBytes, rgba + 16, budget.sourceByteCapacity, "RRTEX source byte budget");
                                meta.path = "cacheobjects/" + out.textureKey + "-mip" + std::to_string(out.mipIndex) + ".rrtex";
                                meta.sha256 = selectedMip->at("sha256").get<std::string>();
                                auto retained = inputs.selected(meta.path, rgba + 16);
                                need(retained.size() == rgba + 16 && std::memcmp(retained.data(), "RRTEX001", 8) == 0 &&
                                    u32(retained.data() + 8) == meta.width && u32(retained.data() + 12) == meta.height &&
                                    meta.sha256 == sha256(retained), "Retained exact RRTEX framing/hash mismatch");
                                out.textureIndex = meta.bankIndex; textureIndexes.emplace(key, meta.bankIndex);
                                next->textures.push_back(std::move(meta)); bytes.push_back(std::move(retained));
                            }
                            out.textureIdentity = std::uint64_t(out.textureIndex) + 1;
                        }
                    }
                    need(out.textureIndex != unresolved || choice.requirement == a::MaterialTextureRequirement50EA::optionalUntextured,
                        "Required displayed slot unresolved: " + out.materialName + ": " + out.unresolvedReason);
                    slots.push_back({{out.assetIdentity, mi, choice.pass, choice.slot}, out.textureIdentity, choice.requirement});
                    next->component[c].push_back(std::move(out));
                }
            }
            std::vector<a::MaterialTextureAssetInput50EA> texturesInput; texturesInput.reserve(bytes.size());
            for (std::uint32_t i = 0; i < bytes.size(); ++i) texturesInput.push_back({std::uint64_t(i) + 1, &bytes[i]});
            const a::MaterialTextureAssetsInput50EA input{texturesInput.data(), static_cast<std::uint32_t>(texturesInput.size()),
                slots.data(), static_cast<std::uint32_t>(slots.size()), budget};
            need(next->bank.load(input, d.bank), "50EA retained texture bank rejected");
            next->coldBytes = inputs.sourceBytes;
            storage_ = std::move(next); d.component = 3; d.material = unresolved; d.pass = d.slot = 0;
            return true;
        } catch (const std::exception& e) { try { d.message = e.what(); } catch (...) {} return false; }
        catch (...) { try { d.message = "Unexpected cold Pod material failure"; } catch (...) {} return false; }
    }
};
} // namespace pod_materials50ed_fixture
