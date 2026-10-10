#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr { namespace assets {
constexpr std::uint32_t max_mesh_vertices = 1000000;
constexpr std::uint32_t max_mesh_indices = 3000000;
constexpr std::uint32_t max_texture_dimension = 4096;
constexpr std::size_t max_asset_file_bytes = 64u * 1024u * 1024u + 16u;

struct Vertex48B {
    float position[3];
    float uv[2];
    std::uint8_t color[4];
};
static_assert(sizeof(Vertex48B) == 24, "Native inspection vertex layout");
struct Bounds48B { float minimum[3]; float maximum[3]; };
struct Mesh48B {
    std::vector<Vertex48B> vertices;
    std::vector<std::uint32_t> indices;
    Bounds48B bounds{};
};
struct Texture48B {
    std::uint32_t width = 0, height = 0;
    std::vector<std::uint8_t> rgba;
};

// Parsing is transactional: a rejected input leaves the previous asset intact.
// Serialized integers/floats are decoded explicitly; no host struct is read.
bool parseMesh48B(const std::uint8_t*, std::size_t, Mesh48B&, std::string& error);
bool parseTexture48B(const std::uint8_t*, std::size_t, Texture48B&, std::string& error);
bool loadMesh48B(const char* path, Mesh48B&, std::string& error);
bool loadTexture48B(const char* path, Texture48B&, std::string& error);
} }
