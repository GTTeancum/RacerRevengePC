#include "import_scene48d.h"
#include "json.hpp"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using Bytes = std::vector<std::uint8_t>;
using Json = nlohmann::json;
namespace fs = std::filesystem;
unsigned checks = 0, failures = 0;
void expect(bool value, const char* label) {
    ++checks;
    if (!value) { ++failures; if (failures <= 30) std::fprintf(stderr, "FAIL: %s\n", label); }
}
Bytes readFile(const fs::path& path, std::size_t max = 64u * 1024u * 1024u) {
    const auto size = fs::file_size(path);
    if (size > max) throw std::runtime_error("Fixture exceeds file cap: " + path.u8string());
    std::ifstream stream(path, std::ios::binary);
    if (!stream) throw std::runtime_error("Missing fixture: " + path.u8string());
    Bytes data(static_cast<std::size_t>(size));
    if (!data.empty()) stream.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    if (!stream || stream.peek() != std::char_traits<char>::eof()) throw std::runtime_error("Fixture read failed");
    return data;
}
Json json(const fs::path& path) { const auto data = readFile(path); return Json::parse(data.begin(), data.end()); }
void put32(Bytes& bytes, std::size_t at, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) bytes[at + i] = static_cast<std::uint8_t>(value >> (8 * i));
}
std::uint32_t u32(const Bytes& bytes, std::size_t at) {
    return std::uint32_t(bytes[at]) | (std::uint32_t(bytes[at + 1]) << 8) |
        (std::uint32_t(bytes[at + 2]) << 16) | (std::uint32_t(bytes[at + 3]) << 24);
}
template<std::size_t N> bool equalArray(const std::array<double, N>& actual, const Json& expected) {
    if (expected.size() != N) return false;
    for (std::size_t i = 0; i < N; ++i) if (actual[i] != expected[i].get<double>()) return false;
    return true;
}
struct HookState { bool cancel = false; unsigned calls = 0, stop = 0xffffffffu; };
bool cancelled(void* opaque) { return static_cast<HookState*>(opaque)->cancel; }
void progress(void* opaque, const char*, std::uint32_t, std::uint32_t) {
    auto& state = *static_cast<HookState*>(opaque);
    if (++state.calls >= state.stop) state.cancel = true;
}
void compareSemantic(const rr::assets::Scene48D& actual, const Json& reference) {
    expect(actual.nodes.size() == reference.at("nodes").size(), "hierarchy size");
    expect(actual.materials == reference.at("materials").get<std::vector<std::string>>(), "original material table");
    expect(actual.world_space == (reference.at("quantization")[7].get<unsigned>() != 0), "world/local flag");
    expect(actual.vertices_all_lods == reference.at("decoded_vertices_all_lods"), "all-LOD vertex accounting");
    expect(actual.surfaces_all_lods == reference.at("surfaces_all_lods"), "all-LOD surface accounting");
    expect(actual.identity_matrix_palette == !reference.at("matrix_palette").is_null(), "matrix palette presence");
    for (std::size_t i = 0; i < actual.nodes.size(); ++i) {
        const auto& node = actual.nodes[i]; const auto& expected = reference.at("nodes")[i];
        expect(node.name == expected.at("name") && node.parent == expected.at("parent"), "original node identity/parent");
        expect(equalArray(node.local_matrix, expected.at("matrix")), "original local matrix values");
        expect(equalArray(node.bind_matrix, expected.at("bind_matrix")), "composed bind matrix values");
        expect(equalArray(node.stored_bounds, expected.at("bounds_center_half_radius")), "original node bounds");
        const auto& bounds = reference.at("bounds_checks")[i];
        expect(equalArray(node.decoded_min, bounds.at("min")) && equalArray(node.decoded_max, bounds.at("max")), "all-LOD decoded node bounds");
        expect(node.decoded_points == bounds.at("points") && node.max_bounds_error == bounds.at("max_error"), "node bounds reconciliation");
    }
    expect(actual.lods.size() == reference.at("lods").size(), "all original LODs retained");
    for (std::size_t level = 0; level < actual.lods.size(); ++level) {
        const auto& lod = actual.lods[level]; const auto& expectedLod = reference.at("lods")[level];
        expect(lod.cutoff == expectedLod.at("cutoff") && lod.surfaces.size() == expectedLod.at("surfaces").size(), "LOD cutoff/surface count");
        for (std::size_t i = 0; i < lod.surfaces.size(); ++i) {
            const auto& surface = lod.surfaces[i]; const auto& expected = expectedLod.at("surfaces")[i];
            expect(surface.material_index == expected.at("material_index") && surface.component_word == expected.at("component_word"), "surface material/attribute identity");
            expect(surface.uv_size_uninterpreted == expected.at("uv_size"), "original uninterpreted UV-size retained");
            expect(surface.indices == expected.at("indices").get<std::vector<std::uint32_t>>(), "original local triangle indices");
            expect(surface.vertices.size() == expected.at("vertices_local").size(), "original local vertices count");
            bool allVertices = true;
            for (std::size_t j = 0; j < surface.vertices.size(); ++j) {
                const auto& vertex = surface.vertices[j]; const auto& v = expected.at("vertices_local")[j];
                for (unsigned axis = 0; axis < 3; ++axis)
                    allVertices = allVertices && vertex.position_local[axis] == v[axis].get<double>();
                for (unsigned axis = 0; axis < 2; ++axis)
                    allVertices = allVertices && vertex.uv[axis] == v[3 + axis].get<double>();
                allVertices = allVertices && equalArray(vertex.normal_local, expected.at("normals_local")[j]) &&
                    vertex.bone == expected.at("bone_indices")[j].get<unsigned>();
                const auto& color = expected.at("original_colors")[j];
                allVertices = allVertices && surface.has_original_colors == !color.is_null();
                if (!color.is_null()) for (unsigned channel = 0; channel < 4; ++channel)
                    allVertices = allVertices && vertex.color[channel] == color[channel].get<unsigned>();
                else allVertices = allVertices && vertex.color == std::array<std::uint8_t, 4>{};
            }
            expect(allVertices, "every local position/normal/UV/bone/source-color value retained exactly");
            const auto& signs = expected.at("normal_alignment");
            expect(surface.normal_positive == signs.value("positive", 0u) && surface.normal_negative == signs.value("negative", 0u) &&
                surface.normal_zero == signs.value("zero", 0u) && surface.mixed_bone_unchecked == signs.value("mixed_bone_unchecked", 0u),
                "normal diagnostics retained without triangle repair");
            const auto& topology = expected.at("topology");
            expect(surface.source_strips == topology.at("source_strips") && surface.strips.size() == topology.at("packed_strips") &&
                surface.continuations.size() == topology.at("continuations").size(), "split-strip counts exactly reconciled");
            bool allStrips = true;
            for (std::size_t j = 0; j < surface.strips.size(); ++j) {
                const auto& strip = surface.strips[j]; const auto& wanted = topology.at("strip_spans")[j];
                allStrips = allStrips && strip.group == wanted.at("group") && strip.start == wanted.at("start") &&
                    strip.end == wanted.at("end") && strip.vertex_start == wanted.at("vertex_start");
            }
            expect(allStrips, "exact original strip spans");
            bool allContinuations = true;
            for (std::size_t j = 0; j < surface.continuations.size(); ++j) {
                const auto& continuation = surface.continuations[j]; const auto& wanted = topology.at("continuations")[j];
                allContinuations = allContinuations && continuation.previous_strip == wanted.at("previous_strip") &&
                    continuation.next_strip == wanted.at("next_strip") && continuation.previous_group == wanted.at("previous_group") &&
                    continuation.next_group == wanted.at("next_group");
            }
            expect(allContinuations, "exact attribute-matched strip continuations");
        }
    }
}
}

int main(int argc, char** argv) {
    using namespace rr::assets;
    try {
        if (argc != 2) throw std::runtime_error("usage: racer_native_scene_check PROJECT_ROOT");
        const fs::path root = fs::u8path(argv[1]);
        DecodeError48C error;
        Bytes mutationFixture, hierarchyFixture, splitFixture;
        Json splitReference;
        Scene48D mutationScene;
        for (const std::string label : {"TA", "POD01"}) {
            const fs::path folder = root / "evidence/assets48C" / label;
            const Json inventory = json(folder / "inventory.json");
            const fs::path resourcePath = root / "work/native/project/disc" /
                (label == "TA" ? "TRACKS/TA.RES" : "PODS/POD01/POD01.RES");
            ResArchive48C archive;
            if (!decodeResource48C(readFile(resourcePath), archive, error)) throw std::runtime_error(error.code + ": " + error.detail);
            unsigned accepted = 0, rejected = 0;
            for (const auto& entry : inventory.at("accepted_candidates")) {
                Bytes bytes;
                if (!extractResMember48C(archive, entry.at("name"), bytes, error)) throw std::runtime_error(error.code);
                Scene48D scene;
                if (!decodeScene48D(bytes, scene, error))
                    throw std::runtime_error(entry.at("name").get<std::string>() + ": " + error.code + ": " + error.detail);
                const fs::path filename = fs::u8path(entry.at("output").get<std::string>()).filename();
                const auto frozen = readFile(folder / filename);
                expect(scene.neutral_lod0 == frozen, "neutral bind LOD0 byte-identical to Python48C");
                auto referenceName = filename; referenceName.replace_extension(".json");
                const auto reference = json(folder / referenceName).at("scene");
                compareSemantic(scene, reference);
                expect(scene.spans_lod0.size() == entry.at("surface_spans_lod0").size(), "neutral export material span count");
                bool spansMatch = true;
                for (std::size_t i = 0; i < scene.spans_lod0.size(); ++i) {
                    const auto& span = scene.spans_lod0[i]; const auto& expected = entry.at("surface_spans_lod0")[i];
                    spansMatch = spansMatch && span.material_index == expected.at("material_index") &&
                        span.vertex_start == expected.at("vertex_start") && span.vertex_count == expected.at("vertex_count") &&
                        span.index_start == expected.at("index_start") && span.index_count == expected.at("index_count") &&
                        span.has_original_colors == expected.at("original_vertex_colors");
                }
                expect(spansMatch, "neutral export material spans retained exactly");
                if (label == "TA" && mutationFixture.empty()) { mutationFixture = bytes; mutationScene = scene; }
                if (label == "POD01" && hierarchyFixture.empty() && scene.nodes.size() > 1) hierarchyFixture = bytes;
                if (splitFixture.empty() && entry.at("continuations").get<unsigned>() > 0) { splitFixture = bytes; splitReference = reference; }
                ++accepted;
            }
            for (const auto& entry : inventory.at("rejected")) {
                Bytes bytes;
                if (!extractResMember48C(archive, entry.at("name"), bytes, error)) throw std::runtime_error(error.code);
                Scene48D scene;
                expect(!decodeScene48D(bytes, scene, error) && error.kind == DecodeKind48C::unsupported, "Python unsupported original remains unsupported");
                expect(error.code == "scene.attribute_command", "unsupported original has named attribute-format error");
                ++rejected;
            }
            expect(accepted == (label == "TA" ? 228u : 13u) && rejected == (label == "TA" ? 2u : 0u), "full original inventory coverage matches frozen evidence");
            std::printf("scene48d %s accepted=%u rejected=%u exact_python48C=1\n", label.c_str(), accepted, rejected);
        }
        const Bytes baseline = mutationScene.neutral_lod0;
        auto reject = [&](const Bytes& data, const char* label) {
            expect(!decodeScene48D(data, mutationScene, error) && error.kind != DecodeKind48C::none && !error.code.empty(), label);
            expect(mutationScene.neutral_lod0 == baseline, "rejected scene leaves previous output intact");
        };
        for (std::size_t length : {std::size_t(0), std::size_t(8), std::size_t(12), std::size_t(139), std::size_t(235),
                std::size_t(239), std::size_t(335), std::size_t(367), mutationFixture.size() - 1})
            reject(Bytes(mutationFixture.begin(), mutationFixture.begin() + length), "scene structural truncation");
        auto changed = mutationFixture; put32(changed, 4, 5); reject(changed, "scene version not silently substituted");
        changed = mutationFixture; put32(changed, 8, 257); reject(changed, "hierarchy allocation cap");
        changed = mutationFixture; put32(changed, 232, 0); reject(changed, "self-cycle parent rejected");
        expect(error.code == "scene.parent_order", "parent-order rejection code");
        changed = mutationFixture; put32(changed, 140, 0x7f800000u); reject(changed, "nonfinite hierarchy transform rejected");
        changed = mutationFixture; put32(changed, 152, 0x3f800000u); reject(changed, "projective hierarchy transform unsupported");
        changed = mutationFixture; put32(changed, 216, 0xbf800000u); reject(changed, "negative hierarchy half extent");
        changed = mutationFixture; put32(changed, 236, 4097); reject(changed, "material table allocation cap");
        changed = mutationFixture; put32(changed, 304, 0); reject(changed, "zero quantization rejected");
        changed = mutationFixture; put32(changed, 336, 2); reject(changed, "unknown LOD layout unsupported");
        changed = mutationFixture; put32(changed, 344, 4097); reject(changed, "surface allocation cap");
        changed = mutationFixture; put32(changed, 348, 1); reject(changed, "surface material index range");
        changed = mutationFixture; put32(changed, 364, 0xfffffff0u); reject(changed, "surface byte extent overflow");
        changed = mutationFixture; changed[398] = 0; changed[399] = 0; reject(changed, "zero bone/ADC rejected");
        changed = mutationFixture; changed[398] = 224; changed[399] = 255; reject(changed, "bone outside hierarchy rejected");
        changed = mutationFixture; put32(changed, 204, 0); reject(changed, "decoded local bounds checked independently");
        changed = mutationFixture; changed.push_back(0); reject(changed, "single-object suffix rejected");
        changed = hierarchyFixture; put32(changed, 12 + 224 + 220, 1); reject(changed, "child self-cycle rejected");
        changed = hierarchyFixture; changed[changed.size() - 5] ^= 1; reject(changed, "matrix palette padding/IDs checked");
        changed = hierarchyFixture; put32(changed, changed.size() - 4, 255); reject(changed, "matrix palette count checked");
        bool alteredContinuation = false;
        for (const auto& lod : splitReference.at("lods")) for (const auto& surface : lod.at("surfaces")) {
            if (alteredContinuation || surface.at("topology").at("continuations").empty()) continue;
            const unsigned group = surface.at("topology").at("continuations")[0].at("next_group");
            // Locate the next group's UV payload in the independently frozen command trace.
            unsigned currentGroup = 0; bool inGroup = false;
            for (const auto& command : surface.at("topology").at("commands")) {
                const std::string word = command.at("word");
                if (word.substr(0, 2) == "6c") { if (inGroup) ++currentGroup; inGroup = true; }
                if (currentGroup == group && word.substr(0, 2) == "65") {
                    changed = splitFixture; const std::size_t at = command.at("offset").get<std::size_t>() + 4;
                    changed[at] ^= 1; reject(changed, "continuation requires matching UV as well as position");
                    expect(error.code == "scene.strip_continuations", "mismatched continuation is not silently joined");
                    alteredContinuation = true; break;
                }
            }
        }
        expect(alteredContinuation, "real split-strip fixture exercised");
        SceneLimits48D limits; limits.input_bytes = mutationFixture.size() - 1;
        expect(!decodeScene48D(mutationFixture, mutationScene, error, nullptr, limits) && error.kind == DecodeKind48C::limit, "custom scene byte limit");
        limits = {}; limits.vertices_all_lods = 1;
        expect(!decodeScene48D(mutationFixture, mutationScene, error, nullptr, limits) && error.code == "scene.total_vertices", "all-LOD cumulative vertex limit");
        HookState state; DecodeHooks48C hooks{&state, cancelled, progress}; state.cancel = true;
        expect(!decodeScene48D(mutationFixture, mutationScene, error, &hooks) && error.kind == DecodeKind48C::cancelled, "initial cancellation");
        state = {}; state.stop = 2;
        expect(!decodeScene48D(mutationFixture, mutationScene, error, &hooks) && error.kind == DecodeKind48C::cancelled && mutationScene.neutral_lod0 == baseline,
            "progress cancellation preserves previous scene");
        std::printf("native-scene48d pointer_bits=%u checks=%u failures=%u\n", unsigned(sizeof(void*) * 8), checks, failures);
        return failures ? 1 : 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "Scene fixture failure: %s\n", error.what()); return 2;
    }
}
