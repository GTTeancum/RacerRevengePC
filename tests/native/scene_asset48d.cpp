#include "scene_asset48d.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
using namespace rr::assets;
using Bytes = std::vector<std::uint8_t>;
unsigned checks = 0;
void expect(bool ok, const std::string& text) { ++checks; if (!ok) throw std::runtime_error(text); }
Bytes read(const std::string& p) {
    std::ifstream f(p, std::ios::binary); if (!f) throw std::runtime_error("Missing fixture " + p);
    return Bytes(std::istreambuf_iterator<char>(f), {});
}
int main(int argc, char** argv) {
    try {
        expect(argc == 2, "root required"); std::string root = argv[1], error;
        unsigned accepted = 0; Bytes sample;
        for (const char* path : {"TRACKS/TA.RES", "PODS/POD01/POD01.RES"}) {
            ResArchive48C archive; DecodeError48C de;
            expect(decodeResource48C(read(root + "/work/native/project/disc/" + path), archive, de), de.code);
            for (const auto& entry : archive.entries) {
                if (entry.name.size() < 4 || entry.name.substr(entry.name.size() - 4) != ".psg") continue;
                Bytes source(archive.decoded.begin() + entry.logical_offset, archive.decoded.begin() + entry.logical_offset + entry.bytes);
                Scene48D scene;
                if (!decodeScene48D(source, scene, de)) { expect(de.kind == DecodeKind48C::unsupported, de.code); continue; }
                Bytes binary, again; Scene48D loaded;
                expect(encodeSceneAsset48D(scene, binary, error), entry.name + ": " + error);
                expect(parseSceneAsset48D(binary, loaded, error), entry.name + ": " + error);
                expect(encodeSceneAsset48D(loaded, again, error) && binary == again, "Canonical scene round trip");
                expect(scene.neutral_lod0 == loaded.neutral_lod0 && scene.materials == loaded.materials, "Compatibility/material preservation");
                expect(scene.nodes.size() == loaded.nodes.size() && scene.lods.size() == loaded.lods.size(), "Hierarchy/LOD preservation");
                expect(scene.quantization == loaded.quantization && scene.world_space == loaded.world_space &&
                    scene.identity_matrix_palette == loaded.identity_matrix_palette, "Scene coordinate semantics");
                for (std::size_t n = 0; n < scene.nodes.size(); ++n) {
                    const auto& a = scene.nodes[n]; const auto& b = loaded.nodes[n];
                    expect(a.name == b.name && a.parent == b.parent && a.local_matrix == b.local_matrix &&
                        a.bind_matrix == b.bind_matrix && a.stored_bounds == b.stored_bounds, "Exact node semantics");
                }
                for (std::size_t l = 0; l < scene.lods.size(); ++l) {
                    expect(scene.lods[l].cutoff == loaded.lods[l].cutoff && scene.lods[l].surfaces.size() == loaded.lods[l].surfaces.size(), "Exact LOD semantics");
                    for (std::size_t s = 0; s < scene.lods[l].surfaces.size(); ++s) {
                        const auto& a = scene.lods[l].surfaces[s]; const auto& b = loaded.lods[l].surfaces[s];
                        expect(a.material_index == b.material_index && a.has_original_colors == b.has_original_colors &&
                            a.uv_size_uninterpreted == b.uv_size_uninterpreted && a.indices == b.indices && a.vertices.size() == b.vertices.size(), "Exact surface semantics");
                        for (std::size_t v = 0; v < a.vertices.size(); ++v) {
                            const auto& av = a.vertices[v]; const auto& bv = b.vertices[v];
                            expect(av.position_local == bv.position_local && av.normal_local == bv.normal_local &&
                                av.uv == bv.uv && av.bone == bv.bone && av.color == bv.color, "Exact vertex semantics");
                        }
                    }
                }
                if (sample.empty() && scene.nodes.size() > 1) sample = binary;
                ++accepted;
            }
        }
        expect(accepted == 241, "Expected 228 TA and 13 POD01 scenes");
        expect(!sample.empty(), "Multinode fixture missing");
        Scene48D output; output.materials = {"transaction sentinel"};
        for (auto size : {std::size_t(0), std::size_t(8), std::size_t(95), sample.size() / 2, sample.size() - 1}) {
            Bytes bad(sample.begin(), sample.begin() + size);
            expect(!parseSceneAsset48D(bad, output, error), "Truncation accepted");
            expect(output.materials == std::vector<std::string>{"transaction sentinel"}, "Rejected parse changed output");
        }
        for (auto offset : {0u, 8u, 12u, 20u, 24u, 28u, 32u, 36u}) {
            Bytes bad = sample; for (unsigned i = 0; i < 4; ++i) bad[offset + i] = 255;
            expect(!parseSceneAsset48D(bad, output, error), "Invalid header accepted");
        }
        Bytes bad = sample; bad.push_back(0); expect(!parseSceneAsset48D(bad, output, error), "Trailing byte accepted");
        bad = sample; bad.back() ^= 1; expect(!parseSceneAsset48D(bad, output, error), "Compatibility index corruption accepted");
        Scene48D parsed; expect(parseSceneAsset48D(sample, parsed, error), error);
        auto contradictory = parsed; contradictory.nodes[0].local_matrix[12] += 100;
        Bytes invalid;
        expect(!encodeSceneAsset48D(contradictory, invalid, error), "Contradictory bind hierarchy accepted");
        contradictory = parsed;
        for (auto& n : contradictory.nodes) n.stored_bounds[0] += 100000;
        expect(!encodeSceneAsset48D(contradictory, invalid, error), "Contradictory bounds accepted");
        parsed.lods[0].surfaces[0].vertices[0].bone = 65535;
        Bytes untouched{1, 2, 3}; expect(!encodeSceneAsset48D(parsed, untouched, error) && untouched == Bytes({1,2,3}), "Invalid encode transaction");
        std::cout << checks << " scene cache checks passed; " << accepted << " original scenes\n"; return 0;
    } catch (const std::exception& e) { std::cerr << "FAIL: " << e.what() << " after " << checks << " checks\n"; return 1; }
}
