#include "collision_mesh48i.h"
#include "collision_runtime48h.h"
#include "import_collision48g.h"
#include "../../tools/assets/collision_placement_fixtures48i.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>

using namespace rr::assets;
namespace fs = std::filesystem;
using Bytes = std::vector<std::uint8_t>;
static std::size_t checks = 0;
static void require(bool value, const char* why) {
    ++checks;
    if (!value) throw std::runtime_error(why);
}
template<class T> static bool exact(const T& a, const T& b) {
    return std::memcmp(&a, &b, sizeof(T)) == 0;
}
static Bytes read(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("fixture open");
    return Bytes(std::istreambuf_iterator<char>(file), {});
}
static CollisionMesh48I sentinel() {
    CollisionMesh48I m;
    m.reverse_mode = 999;
    m.vertices.push_back({1, 2, 3});
    return m;
}
static bool unchanged(const CollisionMesh48I& m) {
    return m.reverse_mode == 999 && m.vertices.size() == 1 &&
        m.vertices[0] == std::array<float, 3>{1, 2, 3} &&
        m.vertex_normals.empty() && m.nodes.empty() && m.faces.empty();
}
static void placementChecks() {
    for (const auto& words : placement_fixtures48i) {
        float f[57]; std::memcpy(f, words, sizeof(f));
        std::array<float, 3> position, look, up;
        std::array<float, 16> local, placement, world;
        std::copy_n(f, 3, position.begin()); std::copy_n(f + 3, 3, look.begin()); std::copy_n(f + 6, 3, up.begin());
        std::copy_n(f + 9, 16, local.begin());
        require(placementCollision48I(position, look, up, placement), "original placement accepted");
        require(std::memcmp(placement.data(), f + 25, 64) == 0, "original-word placement exact");
        require(composeCollisionWorld48I(local, placement, world), "original composition accepted");
        require(std::memcmp(world.data(), f + 41, 64) == 0, "original-word composition exact");
        auto alias = local;
        require(composeCollisionWorld48I(alias, placement, alias) && exact(alias, world), "local alias composition");
        alias = placement;
        require(composeCollisionWorld48I(local, alias, alias) && exact(alias, world), "placement alias composition");
    }
    std::array<float, 16> output{}; output[0] = 123; const auto saved = output;
    require(!placementCollision48I({0, 0, 0}, {0, 0, 0}, {0, 1, 0}, output) && output == saved, "zero axis rejection");
    require(!placementCollision48I({0, 0, 0}, {0, 2, 0}, {0, 1, 0}, output) && output == saved, "parallel axes rejection");
    require(!placementCollision48I({INFINITY, 0, 0}, {0, 0, 1}, {0, 1, 0}, output) && output == saved, "nonfinite placement rejection");
    require(!composeCollisionWorld48I(saved, saved, output) && output == saved, "nonaffine composition rejection");
}
int main(int argc, char** argv) {
    try {
        require(argc == 3, "root and report arguments");
        placementChecks();
        std::size_t files = 0, faces = 0, nodes = 0, cacheBytes = 0, runtimeBytes = 0;
        std::vector<fs::path> paths;
        for (const auto& entry : fs::directory_iterator(fs::path(argv[1]) / "evidence/importCollision48G"))
            if (entry.path().extension() == ".col") paths.push_back(entry.path());
        std::sort(paths.begin(), paths.end());
        for (const auto& path : paths) {
            auto original = read(path);
            Collision48G source;
            DecodeError48C decodeError;
            std::string error;
            require(decodeCollision48G(original, source, decodeError), "source fixture decode");
            Bytes cache;
            require(encodeCollisionAsset48G(source, cache, error), "fixture cache encoding");
            CollisionMesh48I mesh;
            require(loadCollisionMesh48I(cache, mesh, error) && error.empty(), "native cache load");
            cacheBytes += cache.size();
            std::fill(cache.begin(), cache.end(), std::uint8_t{0xcd});
            require(exact(mesh.local_matrix, source.matrix) && mesh.reverse_mode == source.reverse_mode,
                    "source transform and reverse flag retained after input destruction");
            require(mesh.vertices.size() == source.vertices.size() && mesh.vertex_normals.size() == source.vertex_normals.size() &&
                    mesh.nodes.size() == source.nodes.size() && mesh.faces.size() == source.faces.size(), "all record counts");
            for (std::size_t i = 0; i < mesh.vertices.size(); ++i)
                require(exact(mesh.vertices[i], source.vertices[i]), "ordered float vertices");
            for (std::size_t i = 0; i < mesh.vertex_normals.size(); ++i)
                require(exact(mesh.vertex_normals[i], source.vertex_normals[i]), "unquantized vertex normals");
            for (std::size_t i = 0; i < mesh.nodes.size(); ++i) {
                const auto& a = mesh.nodes[i]; const auto& b = source.nodes[i];
                require(exact(a.volume, b.volume) && exact(a.center, b.center) && exact(a.half, b.half) && a.children == b.children,
                        "node bounds volume and ordered children");
            }
            for (std::size_t i = 0; i < mesh.faces.size(); ++i) {
                const auto& a = mesh.faces[i]; const auto& b = source.faces[i];
                std::array<std::int16_t, 3> packed;
                require(packCollisionFaceNormal48H(b.normal, packed), "previously original-word-verified packing");
                require(exact(a.d, b.d) && a.normal == packed && a.vertices[0] == b.vertices[0] &&
                        a.vertices[1] == b.vertices[1] && a.vertices[2] == b.vertices[2], "packed plane and ordered triangle indices");
            }
            ++files; faces += mesh.faces.size(); nodes += mesh.nodes.size();
            runtimeBytes += (mesh.vertices.size() + mesh.vertex_normals.size()) * 12 + mesh.nodes.size() * 36 + mesh.faces.size() * 16 + 68;
            if (files == 1) {
                require(encodeCollisionAsset48G(source, cache, error), "mutation cache reset");
                for (std::size_t end : {std::size_t(0), std::size_t(131), cache.size() - 1}) {
                    Bytes cut(cache.begin(), cache.begin() + end); auto out = sentinel();
                    require(!loadCollisionMesh48I(cut, out, error) && !error.empty() && unchanged(out), "truncated input is transactional");
                }
                cache.back() ^= 1; auto out = sentinel();
                require(!loadCollisionMesh48I(cache, out, error) && unchanged(out), "provenance mismatch is transactional");
                const float extreme = std::numeric_limits<float>::max();
                std::memcpy(original.data() + source.faces_offset + 4, &extreme, 4);
                require(decodeCollision48G(original, source, decodeError) && encodeCollisionAsset48G(source, cache, error), "finite overflow fixture");
                require(!loadCollisionMesh48I(cache, out, error) && error == "collision.runtime_normal_overflow" && unchanged(out), "runtime packing overflow is transactional");
            }
        }
        require(files == 38 && faces == 14586, "all original TA collision fixtures tested");
        std::ofstream report(argv[2]);
        report << "{\n  \"passed\": true,\n  \"files\": " << files << ",\n  \"faces\": " << faces
            << ",\n  \"nodes\": " << nodes << ",\n  \"cache_bytes\": " << cacheBytes << ",\n  \"runtime_record_bytes\": " << runtimeBytes
            << ",\n  \"memory_scope\": \"Typed record payload; excludes vector control blocks, capacity and load-time staging\",\n  \"checks\": " << checks << "\n}\n";
        require(bool(report), "report write");
        std::cout << checks << " native collision mesh checks passed; " << files << " assets; " << faces << " faces; " << runtimeBytes << " runtime record bytes\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL after " << checks << ": " << error.what() << '\n';
        return 1;
    }
}
