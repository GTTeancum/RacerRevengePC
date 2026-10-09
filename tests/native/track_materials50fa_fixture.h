#pragma once
#include "pod_materials50ed_fixture.h"
#include <cfenv>
#include <cmath>

// Cold retained-data bridge for the exact TA mesh used by the live client.
// It re-derives spans from source RRSCENE1 surfaces and the pinned 49G export,
// verifies every live position/UV/color/index, and loads only real RRTEX bytes.
// No image generation, texture substitution, guessed material assignment,
// original shader emulation, resource decoding or GL calls occur here.
namespace track_materials50fa_fixture {
namespace a = rr::assets;
constexpr std::uint32_t unresolved = a::unresolvedMaterialTexture50EA;
using MipMode = pod_materials50ed_fixture::MipMode;
using Choice = pod_materials50ed_fixture::Choice;
using TextureSelection = pod_materials50ed_fixture::TextureSelection;
// Caller supplies a reviewed original-format decoder. Input is the exact
// source member, authenticated here before the callback. Output must be a
// real decoded source mip/frame serialized as RRTEX001; never fallback pixels.
using SourceTextureDecoder = bool (*)(const char* sourceMember,
    const std::vector<std::uint8_t>& source, std::vector<std::uint8_t>& rrtex,
    std::uint32_t& mipIndex, std::string& error);
struct Request {
    std::filesystem::path root; // Retained visual49G directory.
    std::filesystem::path exportReport; // Matching 49G placed export report.
    Choice choice{}; // Explicit authored pass/slot and missing-texture policy.
    std::size_t coldSourceByteCapacity = 0;
    a::MaterialTextureBudgets50EA textureBudgets{};
    std::filesystem::path sourceArchive; // Optional exact retained TA.RES.
    SourceTextureDecoder decodeSourceTexture = nullptr;
    // Deliberate host display policy: use an actual decoded ATM frame until
    // an animation owner exists. False leaves the two ATM sources unresolved.
    bool allowStaticAnimationFrame = false;
};
struct Selection {
    std::uint64_t assetIdentity = 0;
    std::uint32_t materialIndex = 0, pass = 0, slot = 0;
    std::uint32_t bankSlot = unresolved, textureIndex = unresolved, mipIndex = unresolved;
    std::string sceneKey, materialName, materialKey, program, textureName, textureKey;
    std::string unresolvedReason;
    std::string decodedSourceMember, decodedSourceSha256;
    bool staticAnimationFrame = false;
};
struct Span {
    std::uint32_t indexStart = 0, indexCount = 0, vertexStart = 0, vertexCount = 0;
    std::uint32_t textureIndex = unresolved, materialSelection = unresolved;
    std::uint32_t worldInstanceIndex = 0, surfaceIndex = 0;
    bool hasOriginalColors = false;
};
struct Diagnostic {
    std::uint32_t instance = unresolved, surface = unresolved, material = unresolved;
    std::string message;
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
namespace shared = pod_materials50ed_fixture::detail;
using shared::Bytes; using shared::Json; using shared::Inputs;
using shared::need; using shared::number; using shared::hash; using shared::normalized;
using shared::sha256; using shared::add; using shared::u32;
constexpr const char* ta = "TRACKS/TA.RES";
// Fixed development fixture identity, not a general import format. The files
// are independently retained 49G source selection and exporter evidence.
constexpr const char* selectionSha = "12fad4579276c9fce30abd3009420a8529a033e1d7271ae6b0688927f8adf674";
constexpr const char* exportSha = "ef2bdffbb36f5669c0d84eb9b31e5a3f8f4853b5ab966898bb768efd42eb47c4";
constexpr const char* archiveSha = "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8";
inline bool sameFloat(float x, float y) noexcept { return std::memcmp(&x, &y, sizeof x) == 0; }
inline void resolution(const Json& row, const char* extension, const char* expected) {
    const auto name = row.at("name").get<std::string>();
    need(row.at("container") == ta && shared::name(name), "TA exact resolution scope/name");
    const auto wanted = normalized("textures/" + name + extension);
    need(row.at("requested_member") == wanted && row.at("expected_status") == expected,
        "TA resolution member/type");
    const auto local = number(row, "local_candidates", 10000), global = number(row, "global_candidates", 10000);
    const auto& candidates = row.at("candidates");
    need(candidates.is_array() && local <= global && candidates.size() == (local ? local : global) &&
        row.at("scope") == (local ? "local" : "global"), "TA resolution candidate count/scope");
    std::set<std::string> sources; bool unsupported = false;
    for (const auto& c : candidates) {
        need(normalized(c.at("member")) == wanted && (!local || normalized(c.at("container")) == ta),
            "TA exact local-first candidate");
        const auto source = c.at("source_sha256").get<std::string>();
        need(hash(source), "Candidate source hash"); sources.insert(source);
        unsupported |= c.at("status") != expected;
    }
    if (row.at("status") == "resolved") {
        const auto& selected = row.at("selected");
        need(!candidates.empty() && sources.size() == 1 && !unsupported &&
            std::find(candidates.begin(), candidates.end(), selected) != candidates.end() &&
            selected.at("key") == row.at("key") && hash(row.at("key")), "TA resolved candidate provenance");
    } else {
        const std::string why = row.at("reason");
        need(row.at("status") == "unresolved" &&
            ((candidates.empty() && why == "no_exact_member") ||
             (sources.size() == 1 && unsupported && why == "original_resource_unsupported")),
            "TA unresolved resource provenance");
    }
}
struct SourceScene {
    a::Scene48D scene;
    std::vector<std::uint32_t> materialSelections;
    const Json* request = nullptr;
};
} // namespace detail

class Owner final {
    struct Storage {
        a::MaterialTextureAssets50EA bank;
        std::vector<Selection> selections;
        std::vector<Span> spans;
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
    const std::vector<Span>* spans() const noexcept { return storage_ ? &storage_->spans : nullptr; }
    const std::vector<Selection>* selections() const noexcept { return storage_ ? &storage_->selections : nullptr; }
    const std::vector<TextureSelection>* textures() const noexcept { return storage_ ? &storage_->textures : nullptr; }
    std::size_t coldSourceBytes() const noexcept { return storage_ ? storage_->coldBytes : 0; }

    // Serial cold load, transactional and retryable after failure. Existing
    // publication rejects reload without invalidating views. No references to
    // the caller mesh survive this call. Strict IEEE arithmetic is required.
    bool load(const Request& request, const a::Mesh48B& mesh, Diagnostic& d) noexcept {
        using namespace detail;
        try {
            d = {};
            need(!storage_, "TA materials already loaded");
            need(std::fegetround() == FE_TONEAREST, "TA parity requires FE_TONEAREST");
            need(!request.root.empty() && !request.exportReport.empty(), "Explicit retained TA paths required");
            need(request.coldSourceByteCapacity > 0 &&
                request.coldSourceByteCapacity <= pod_materials50ed_fixture::maxColdSourceBytes,
                "Explicit bounded TA cold source capacity required");
            const auto& choice = request.choice;
            // This pinned corpus has exactly one authored pass and map in
            // every TA material. Do not invent an absent channel policy.
            need(choice.pass == 1 && choice.slot == 1 &&
                (choice.requirement == a::MaterialTextureRequirement50EA::required ||
                 choice.requirement == a::MaterialTextureRequirement50EA::optionalUntextured) &&
                (choice.mipMode == MipMode::exactIndex || choice.mipMode == MipMode::largestRetained) &&
                choice.mipIndex <= 6, "Explicit supported TA pass/slot/mip policy required");
            const auto& budget = request.textureBudgets;
            need(budget.textureCapacity <= a::maxMaterialTextureAssets50EA &&
                budget.slotCapacity <= a::maxMaterialTextureSlots50EA &&
                budget.sourceByteCapacity <= a::maxMaterialTextureSourceBytes50EA &&
                budget.rgbaByteCapacity <= a::maxMaterialTextureRGBABytes50EA &&
                budget.dimensionCapacity <= a::max_texture_dimension, "Invalid TA texture budgets");
            need(mesh.vertices.size() == 110196 && mesh.indices.size() == 209514, "Exact TA mesh extent required");
            Inputs inputs; inputs.root = std::filesystem::canonical(request.root);
            inputs.capacity = request.coldSourceByteCapacity;
            need(!std::filesystem::is_symlink(inputs.root / "cacheobjects"), "TA cache directory symlink rejected");
            const auto selectedBytes = inputs.read(inputs.root / "asset-selection.json", 4u * 1024u * 1024u);
            const auto reportBytes = inputs.read(request.exportReport, 4u * 1024u * 1024u);
            need(sha256(selectedBytes) == selectionSha && sha256(reportBytes) == exportSha,
                "TA retained selection/export identity mismatch");
            const auto selected = Json::parse(selectedBytes.begin(), selectedBytes.end());
            const auto report = Json::parse(reportBytes.begin(), reportBytes.end());
            need(request.sourceArchive.empty() == (request.decodeSourceTexture == nullptr),
                "TA source archive and decoder must be supplied together");
            a::ResArchive48C archive;
            if (request.decodeSourceTexture) {
                const auto archiveBytes = inputs.read(request.sourceArchive, 4u * 1024u * 1024u);
                need(sha256(archiveBytes) == archiveSha, "TA original archive identity mismatch");
                a::DecodeLimits48C limits; limits.resource_file_bytes = 4u * 1024u * 1024u;
                limits.resource_decoded_bytes = 16u * 1024u * 1024u; limits.resource_entries = 512;
                a::DecodeError48C error;
                need(a::decodeResource48C(archiveBytes, archive, error, nullptr, limits),
                    "TA original archive decode: " + error.code);
            }
            need(selected.at("schema") == "RR_VISUAL_SELECTION49G_V1" && selected.at("status") == "complete" &&
                report.at("schema") == "RR_AUTHORED_MESH49G_V1" && report.at("success") == true,
                "TA selection/export schema/status");
            for (const auto& row : selected.at("objects")) {
                const auto path = row.at("path").get<std::string>();
                need(shared::safeObjectPath(path) && hash(row.at("sha256")) &&
                    inputs.objects.emplace(path, row).second, "TA selected object path/hash/duplicate");
            }
            std::map<std::string, const Json*> materials, textureRows, sceneRows;
            for (const auto& row : selected.at("material_resolution")) if (row.at("container") == ta) {
                resolution(row, ".psm", "converted_material");
                need(materials.emplace(row.at("name"), &row).second, "Duplicate TA material association");
            }
            for (const auto& row : selected.at("texture_resolution")) if (row.at("container") == ta) {
                resolution(row, ".pst", "converted_texture");
                need(textureRows.emplace(row.at("name"), &row).second, "Duplicate TA texture association");
            }
            for (const auto& row : selected.at("scene_requests")) if (row.at("container") == ta)
                need(sceneRows.emplace(row.at("key"), &row).second, "Duplicate TA source scene");
            need(materials.size() == 29 && sceneRows.size() == 228, "TA retained material/scene extent");
            const auto& instances = report.at("world_instances");
            need(instances.size() == 284 && selected.at("visual_instances").size() == instances.size(),
                "TA source world instance extent");
            auto next = std::make_unique<Storage>(); next->spans.reserve(353);
            std::map<std::string, SourceScene> scenes;
            std::map<std::pair<std::string, std::uint32_t>, std::uint32_t> textureIndexes;
            // One callback invocation per original unresolved resource, even
            // when many scene materials use that source. Failed decodes retain
            // their precise reason and never synthesize a bank texture.
            struct DecodedSource { std::uint32_t texture = unresolved, mip = unresolved; std::string why; };
            std::map<std::string, DecodedSource> decodedSources;
            std::vector<Bytes> bytes;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            std::size_t rgbaBytes = 0, textureSourceBytes = 0;
            std::uint32_t nextVertex = 0, nextIndex = 0;
            for (std::uint32_t instanceIndex = 0; instanceIndex < instances.size(); ++instanceIndex) {
                d.instance = instanceIndex;
                const auto& instance = instances.at(instanceIndex);
                const auto& selectedInstance = selected.at("visual_instances").at(instanceIndex);
                for (auto it = selectedInstance.begin(); it != selectedInstance.end(); ++it)
                    need(instance.at(it.key()) == it.value(), "TA exporter differs from selected source placement");
                const std::string key = instance.at("key");
                auto found = scenes.find(key);
                if (found == scenes.end()) {
                    const auto rowFound = sceneRows.find(key);
                    need(rowFound != sceneRows.end(), "TA instance scene is unselected");
                    const auto& row = *rowFound->second;
                    const auto& record = inputs.record(key);
                    need(row.at("status") == "converted_geometry" && record == row.at("native") &&
                        record.at("source_sha256") == row.at("source_sha256") &&
                        row.at("rrscene") == "cacheobjects/" + key + ".rrscene", "TA scene provenance");
                    const auto sourceBytes = inputs.selected(row.at("rrscene"), a::max_scene_bytes48d);
                    need(record.at("scene_sha256") == sha256(sourceBytes), "TA scene record hash mismatch");
                    SourceScene source; source.request = &row; std::string why;
                    need(a::parseSceneAsset48D(sourceBytes, source.scene, why), "TA source scene parse: " + why);
                    const auto& scene = source.scene;
                    need(scene.nodes.size() == 1 && !scene.identity_matrix_palette &&
                        record.at("materials") == Json(scene.materials), "TA single-root source scene/material subset");
                    const auto assetIdentity = static_cast<std::uint64_t>(scenes.size()) + 1;
                    for (std::uint32_t mi = 0; mi < scene.materials.size(); ++mi) {
                        d.material = mi;
                        need(slots.size() < budget.slotCapacity, "TA material slot budget");
                        Selection out; out.assetIdentity = assetIdentity; out.sceneKey = key;
                        out.materialIndex = mi; out.materialName = scene.materials[mi];
                        out.pass = choice.pass; out.slot = choice.slot;
                        out.bankSlot = static_cast<std::uint32_t>(slots.size());
                        const auto material = materials.find(out.materialName);
                        need(material != materials.end(), "TA exact scene material association absent");
                        const auto& mr = *material->second;
                        need(mr.at("status") == "resolved", "TA material unresolved");
                        out.materialKey = mr.at("key").get<std::string>();
                        const auto& materialRecord = inputs.record(out.materialKey);
                        need(materialRecord.at("type") == "material" &&
                            materialRecord.at("converter") == "rr-native-material-48E-v1" &&
                            materialRecord.at("source_sha256") == mr.at("selected").at("source_sha256"),
                            "TA material source record mismatch");
                        bool requester = false;
                        for (const auto& r : mr.at("requested_by"))
                            if (r.at("scene_key") == key && r.at("scene_member") == row.at("member")) requester = true;
                        need(requester, "TA scene did not request this material");
                        const auto& passes = materialRecord.at("passes");
                        need(passes.size() == 1 && passes.at(0).at("pass") == 1 &&
                            passes.at(0).at("textures").size() == 1 && passes.at(0).at("textures").at(0).at("slot") == 1,
                            "TA material is outside proven single-pass/single-map corpus");
                        out.program = passes.at(0).at("program").get<std::string>();
                        out.textureName = passes.at(0).at("textures").at(0).at("name").get<std::string>();
                        const auto texture = textureRows.find(out.textureName);
                        need(texture != textureRows.end(), "TA exact material texture association absent");
                        const auto& tr = *texture->second; requester = false;
                        for (const auto& r : tr.at("requested_by"))
                            if (r.at("material_key") == out.materialKey && r.at("material_name") == out.materialName &&
                                r.at("pass") == 1 && r.at("slot") == 1 && r.at("program") == out.program) requester = true;
                        need(requester, "TA material did not request this texture slot");
                        if (tr.at("status") != "resolved") {
                            out.unresolvedReason = tr.at("reason").get<std::string>();
                            const Json* source = nullptr;
                            if (request.decodeSourceTexture && out.unresolvedReason == "original_resource_unsupported") {
                                // The retained resolver's exact local PST is
                                // still authoritative; no global substitute.
                                const auto& candidates = tr.at("candidates");
                                if (candidates.size() == 1 && candidates.at(0).at("container") == ta)
                                    source = &candidates.at(0);
                            } else if (request.decodeSourceTexture && request.allowStaticAnimationFrame &&
                                out.unresolvedReason == "no_exact_member") {
                                // Only the two independently retained animated
                                // TA resources have a proven first-frame route.
                                const std::string member = out.textureName == "arrowred" ? "textures/arrowred.atm" :
                                    out.textureName == "crowd" ? "textures/crowd.atm" : "";
                                if (!member.empty()) for (const auto& candidate : tr.at("non_substitutable_same_stem"))
                                    if (candidate.at("container") == ta && normalized(candidate.at("member")) == normalized(member)) {
                                        need(!source, "Ambiguous TA animated source"); source = &candidate;
                                    }
                            }
                            if (source) {
                                out.decodedSourceMember = source->at("member").get<std::string>();
                                out.decodedSourceSha256 = source->at("source_sha256").get<std::string>();
                                out.staticAnimationFrame = out.unresolvedReason == "no_exact_member";
                                const auto sourceKey = normalized(out.decodedSourceMember);
                                auto decoded = decodedSources.find(sourceKey);
                                if (decoded == decodedSources.end()) {
                                    Bytes original; a::DecodeError48C error;
                                    need(a::extractResMember48C(archive, out.decodedSourceMember, original, error, 1024u * 1024u),
                                        "TA exact original texture extraction: " + error.code);
                                    need(original.size() == number(*source, "source_bytes", 1024u * 1024u) &&
                                        sha256(original) == out.decodedSourceSha256, "TA original texture member hash/extent mismatch");
                                    DecodedSource result; Bytes rrtex;
                                    if (request.decodeSourceTexture(out.decodedSourceMember.c_str(), original, rrtex, result.mip, result.why)) {
                                        need(result.mip <= 6 && rrtex.size() >= 16 && std::memcmp(rrtex.data(), "RRTEX001", 8) == 0,
                                            "TA source decoder output framing/mip");
                                        const auto width = u32(rrtex.data() + 8), height = u32(rrtex.data() + 12);
                                        need(width && height && width <= budget.dimensionCapacity && height <= budget.dimensionCapacity,
                                            "TA decoded source texture dimension budget");
                                        const auto rgba = std::size_t(width) * height * 4;
                                        need(rrtex.size() == rgba + 16 && bytes.size() < budget.textureCapacity,
                                            "TA decoded source texture extent/count budget");
                                        add(rgbaBytes, rgba, budget.rgbaByteCapacity, "TA decoded source RGBA budget");
                                        add(textureSourceBytes, rrtex.size(), budget.sourceByteCapacity, "TA decoded source RRTEX budget");
                                        TextureSelection meta; meta.key = out.decodedSourceSha256; meta.mipIndex = result.mip;
                                        meta.bankIndex = static_cast<std::uint32_t>(bytes.size()); meta.identity = std::uint64_t(meta.bankIndex) + 1;
                                        meta.width = width; meta.height = height;
                                        meta.path = request.sourceArchive.generic_string() + "::" + out.decodedSourceMember;
                                        meta.sha256 = sha256(rrtex); result.texture = meta.bankIndex; result.why.clear();
                                        next->textures.push_back(std::move(meta)); bytes.push_back(std::move(rrtex));
                                    } else {
                                        need(!result.why.empty(), "TA source decoder failure requires an explicit reason");
                                        result.why = "source_decoder_unsupported: " + result.why;
                                    }
                                    decoded = decodedSources.emplace(sourceKey, std::move(result)).first;
                                }
                                out.textureIndex = decoded->second.texture; out.mipIndex = decoded->second.mip;
                                out.unresolvedReason = decoded->second.why;
                                if (out.textureIndex != unresolved) out.textureKey = out.decodedSourceSha256;
                            }
                        }
                        else {
                            out.textureKey = tr.at("key").get<std::string>();
                            const auto& textureRecord = inputs.record(out.textureKey);
                            need(textureRecord == tr.at("native") && textureRecord.at("type") == "texture" &&
                                textureRecord.at("converter") == "rr-native-texture-48E-v1" &&
                                textureRecord.at("source_sha256") == tr.at("selected").at("source_sha256"),
                                "TA texture source record mismatch");
                            const auto& mips = textureRecord.at("mips");
                            need(!mips.empty() && mips.size() <= 7, "TA retained mip extent");
                            const Json* picked = nullptr;
                            std::uint32_t mipIndex = 0, lastW = 0, lastH = 0;
                            for (const auto& mip : mips) {
                                const auto index = number(mip, "index", 6);
                                const auto w = number(mip, "width", 1024), h = number(mip, "height", 1024);
                                need(index == mipIndex++ && w >= 16 && h >= 16 &&
                                    (!index || (w == lastW * 2 && h == lastH * 2)) && hash(mip.at("sha256")),
                                    "TA mip source order/dimensions");
                                lastW = static_cast<std::uint32_t>(w); lastH = static_cast<std::uint32_t>(h);
                                const auto path = "cacheobjects/" + out.textureKey + "-mip" + std::to_string(index) + ".rrtex";
                                const auto object = inputs.objects.find(path);
                                if (object != inputs.objects.end() && object->second.at("extracted") == true &&
                                    (choice.mipMode == MipMode::largestRetained || index == choice.mipIndex)) picked = &mip;
                            }
                            if (!picked) out.unresolvedReason = "requested_mip_not_retained";
                            else {
                                out.mipIndex = static_cast<std::uint32_t>(number(*picked, "index", 6));
                                const auto textureKey = std::make_pair(out.textureKey, out.mipIndex);
                                const auto existing = textureIndexes.find(textureKey);
                                if (existing != textureIndexes.end()) out.textureIndex = existing->second;
                                else {
                                    need(bytes.size() < budget.textureCapacity, "TA texture count budget");
                                    TextureSelection meta; meta.key = out.textureKey; meta.mipIndex = out.mipIndex;
                                    meta.bankIndex = static_cast<std::uint32_t>(bytes.size()); meta.identity = std::uint64_t(meta.bankIndex) + 1;
                                    meta.width = static_cast<std::uint32_t>(number(*picked, "width", 1024));
                                    meta.height = static_cast<std::uint32_t>(number(*picked, "height", 1024));
                                    need(meta.width <= budget.dimensionCapacity && meta.height <= budget.dimensionCapacity,
                                        "TA texture dimension budget");
                                    const auto rgba = std::size_t(meta.width) * meta.height * 4;
                                    add(rgbaBytes, rgba, budget.rgbaByteCapacity, "TA RGBA byte budget");
                                    add(textureSourceBytes, rgba + 16, budget.sourceByteCapacity, "TA texture source byte budget");
                                    meta.path = "cacheobjects/" + out.textureKey + "-mip" + std::to_string(out.mipIndex) + ".rrtex";
                                    meta.sha256 = picked->at("sha256").get<std::string>();
                                    auto retained = inputs.selected(meta.path, rgba + 16);
                                    need(retained.size() == rgba + 16 && std::memcmp(retained.data(), "RRTEX001", 8) == 0 &&
                                        u32(retained.data() + 8) == meta.width && u32(retained.data() + 12) == meta.height &&
                                        sha256(retained) == meta.sha256, "TA actual texture framing/hash");
                                    out.textureIndex = meta.bankIndex; textureIndexes.emplace(textureKey, meta.bankIndex);
                                    next->textures.push_back(std::move(meta)); bytes.push_back(std::move(retained));
                                }
                            }
                        }
                        need(out.textureIndex != unresolved || choice.requirement == a::MaterialTextureRequirement50EA::optionalUntextured,
                            "Required TA texture unresolved: " + out.textureName + ": " + out.unresolvedReason);
                        const auto textureIdentity = out.textureIndex == unresolved ?
                            ((std::uint64_t(1) << 63) | out.bankSlot) : std::uint64_t(out.textureIndex) + 1;
                        slots.push_back({{out.assetIdentity, mi, choice.pass, choice.slot}, textureIdentity, choice.requirement});
                        source.materialSelections.push_back(static_cast<std::uint32_t>(next->selections.size()));
                        next->selections.push_back(std::move(out));
                    }
                    found = scenes.emplace(key, std::move(source)).first;
                }
                const auto& source = found->second;
                const auto& scene = source.scene;
                need(instance.at("container") == ta &&
                    normalized(instance.at("geometry_member")) == normalized(source.request->at("member")),
                    "TA instance geometry/source mismatch");
                const auto vb = static_cast<std::uint32_t>(number(instance, "vertex_start", a::max_mesh_vertices));
                const auto ib = static_cast<std::uint32_t>(number(instance, "index_start", a::max_mesh_indices));
                need(vb == nextVertex && ib == nextIndex && instance.at("surfaces").size() == scene.spans_lod0.size(),
                    "TA source instance offset/surface order mismatch");
                const auto matrix = instance.at("matrix").get<std::array<double, 16>>();
                for (auto v : matrix) need(std::isfinite(v), "TA source placement nonfinite");
                for (std::uint32_t si = 0; si < scene.spans_lod0.size(); ++si) {
                    d.surface = si;
                    const auto& span = scene.spans_lod0[si]; const auto& surface = scene.lods.at(0).surfaces.at(si);
                    Span out; out.indexStart = ib + span.index_start; out.indexCount = span.index_count;
                    out.vertexStart = vb + span.vertex_start; out.vertexCount = span.vertex_count;
                    out.materialSelection = source.materialSelections.at(span.material_index);
                    out.textureIndex = next->selections.at(out.materialSelection).textureIndex;
                    out.worldInstanceIndex = static_cast<std::uint32_t>(number(instance, "world_instance_index", 317));
                    out.surfaceIndex = si; out.hasOriginalColors = span.has_original_colors;
                    need(out.vertexStart == nextVertex && out.indexStart == nextIndex && out.vertexCount && out.indexCount &&
                        out.indexCount % 3 == 0 && out.vertexCount <= mesh.vertices.size() - std::min<std::size_t>(nextVertex, mesh.vertices.size()) &&
                        out.indexCount <= mesh.indices.size() - std::min<std::size_t>(nextIndex, mesh.indices.size()),
                        "TA source span order/extent mismatch");
                    const auto& recorded = instance.at("surfaces").at(si);
                    need(recorded.at("material") == scene.materials.at(span.material_index) &&
                        recorded.at("material_index") == span.material_index && recorded.at("vertex_start") == out.vertexStart &&
                        recorded.at("index_start") == out.indexStart && recorded.at("vertices") == out.vertexCount &&
                        recorded.at("indices") == out.indexCount && recorded.at("has_original_colors") == out.hasOriginalColors,
                        "TA exporter source material/span mismatch");
                    for (std::size_t vi = 0; vi < surface.vertices.size(); ++vi) {
                        const auto& authored = surface.vertices[vi]; const auto& live = mesh.vertices[out.vertexStart + vi];
                        need(authored.bone == 0, "TA source vertex bone unsupported");
                        for (unsigned c = 0; c < 2; ++c)
                            need(sameFloat(live.uv[c], static_cast<float>(authored.uv[c])), "TA source UV mismatch");
                        for (unsigned c = 0; c < 4; ++c)
                            need(live.color[c] == (surface.has_original_colors ? authored.color[c] : 255), "TA source color mismatch");
                        for (unsigned c = 0; c < 3; ++c) {
                            double position = 0;
                            for (unsigned q = 0; q < 3; ++q) position += authored.position_local[q] * matrix[q * 4 + c];
                            position += matrix[12 + c];
                            need(std::isfinite(position) && sameFloat(live.position[c], static_cast<float>(position)),
                                "TA source position/placement mismatch");
                        }
                    }
                    for (std::size_t ii = 0; ii < surface.indices.size(); ++ii)
                        need(mesh.indices[out.indexStart + ii] == out.vertexStart + surface.indices[ii], "TA source index order mismatch");
                    next->spans.push_back(out); nextVertex += out.vertexCount; nextIndex += out.indexCount;
                }
            }
            need(next->spans.size() == 353 && nextVertex == mesh.vertices.size() && nextIndex == mesh.indices.size(),
                "TA full source span coverage mismatch");
            std::vector<a::MaterialTextureAssetInput50EA> textureInput; textureInput.reserve(bytes.size());
            for (std::uint32_t i = 0; i < bytes.size(); ++i) textureInput.push_back({std::uint64_t(i) + 1, &bytes[i]});
            const a::MaterialTextureAssetsInput50EA bankInput{textureInput.data(), static_cast<std::uint32_t>(textureInput.size()),
                slots.data(), static_cast<std::uint32_t>(slots.size()), budget};
            need(next->bank.load(bankInput, d.bank), "TA real texture bank rejected");
            next->coldBytes = inputs.sourceBytes; storage_ = std::move(next);
            d.instance = d.surface = d.material = unresolved; return true;
        } catch (const std::exception& e) { try { d.message = e.what(); } catch (...) {} return false; }
        catch (...) { try { d.message = "Unexpected TA cold material failure"; } catch (...) {} return false; }
    }
};
} // namespace track_materials50fa_fixture
