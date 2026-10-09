#include "native_asset48b.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <utility>

namespace rr { namespace assets {
namespace {
std::uint32_t readU32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
        (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
float readF32(const std::uint8_t* p) {
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
                  "Asset contract needs IEEE754 binary32");
    const std::uint32_t bits = readU32(p);
    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}
bool fail(std::string& error, const char* reason) { error = reason; return false; }
bool loadFile(const char* path, std::vector<std::uint8_t>& bytes, std::string& error) {
    if (!path || !*path) return fail(error, "empty asset path");
    std::FILE* file = nullptr;
#ifdef _MSC_VER
    if (fopen_s(&file, path, "rb") != 0) file = nullptr;
#else
    file = std::fopen(path, "rb");
#endif
    if (!file) return fail(error, "cannot open asset");
    if (std::fseek(file, 0, SEEK_END) != 0) {
        std::fclose(file); return fail(error, "cannot seek asset");
    }
    const long length = std::ftell(file);
    if (length < 16 || static_cast<unsigned long>(length) > max_asset_file_bytes) {
        std::fclose(file); return fail(error, "asset file length outside bounds");
    }
    if (std::fseek(file, 0, SEEK_SET) != 0) {
        std::fclose(file); return fail(error, "cannot rewind asset");
    }
    bytes.resize(static_cast<std::size_t>(length));
    const std::size_t read = std::fread(bytes.data(), 1, bytes.size(), file);
    const int extra = std::fgetc(file);
    const bool ioError = std::ferror(file) != 0;
    const int closeResult = std::fclose(file);
    if (read != bytes.size() || extra != EOF || ioError || closeResult != 0)
        return fail(error, "asset read failed or file changed size");
    return true;
}
}

bool parseMesh48B(const std::uint8_t* bytes, std::size_t size,
                  Mesh48B& output, std::string& error) {
    error.clear();
    if (!bytes || size < 16 || size > max_asset_file_bytes)
        return fail(error, "mesh file length outside bounds");
    if (std::memcmp(bytes, "RRMESH01", 8) != 0) return fail(error, "mesh magic/version mismatch");
    const std::uint32_t vertexCount = readU32(bytes + 8), indexCount = readU32(bytes + 12);
    if (!vertexCount || vertexCount > max_mesh_vertices)
        return fail(error, "mesh vertex count outside bounds");
    if (!indexCount || indexCount > max_mesh_indices || indexCount % 3)
        return fail(error, "mesh triangle index count outside bounds");
    const std::uint64_t expected = 16ull + 24ull * vertexCount + 4ull * indexCount;
    if (expected != size) return fail(error, "mesh exact file length mismatch");
    // Check every untrusted value before constructing native arrays.
    for (std::uint32_t i = 0; i < vertexCount; ++i) {
        const std::uint8_t* vertex = bytes + 16 + std::size_t(i) * 24;
        for (unsigned j = 0; j < 5; ++j)
            if (!std::isfinite(readF32(vertex + j * 4)))
                return fail(error, "mesh contains non-finite position or UV");
    }
    const std::uint8_t* indices = bytes + 16 + std::size_t(vertexCount) * 24;
    for (std::uint32_t i = 0; i < indexCount; ++i)
        if (readU32(indices + std::size_t(i) * 4) >= vertexCount)
            return fail(error, "mesh index outside vertex array");
    Mesh48B parsed;
    parsed.vertices.resize(vertexCount);
    parsed.indices.resize(indexCount);
    for (std::uint32_t i = 0; i < vertexCount; ++i) {
        const std::uint8_t* source = bytes + 16 + std::size_t(i) * 24;
        Vertex48B& vertex = parsed.vertices[i];
        for (unsigned j = 0; j < 3; ++j) {
            vertex.position[j] = readF32(source + j * 4);
            if (i == 0 || vertex.position[j] < parsed.bounds.minimum[j])
                parsed.bounds.minimum[j] = vertex.position[j];
            if (i == 0 || vertex.position[j] > parsed.bounds.maximum[j])
                parsed.bounds.maximum[j] = vertex.position[j];
        }
        vertex.uv[0] = readF32(source + 12); vertex.uv[1] = readF32(source + 16);
        std::memcpy(vertex.color, source + 20, 4);
    }
    for (std::uint32_t i = 0; i < indexCount; ++i)
        parsed.indices[i] = readU32(indices + std::size_t(i) * 4);
    output = std::move(parsed);
    return true;
}

bool parseTexture48B(const std::uint8_t* bytes, std::size_t size,
                     Texture48B& output, std::string& error) {
    error.clear();
    if (!bytes || size < 16 || size > max_asset_file_bytes)
        return fail(error, "texture file length outside bounds");
    if (std::memcmp(bytes, "RRTEX001", 8) != 0) return fail(error, "texture magic/version mismatch");
    const std::uint32_t width = readU32(bytes + 8), height = readU32(bytes + 12);
    if (!width || !height || width > max_texture_dimension || height > max_texture_dimension)
        return fail(error, "texture dimensions outside bounds");
    const std::uint64_t pixelBytes = 4ull * width * height;
    if (16ull + pixelBytes != size) return fail(error, "texture exact file length mismatch");
    Texture48B parsed;
    parsed.width = width; parsed.height = height;
    parsed.rgba.assign(bytes + 16, bytes + size);
    output = std::move(parsed);
    return true;
}

bool loadMesh48B(const char* path, Mesh48B& mesh, std::string& error) {
    std::vector<std::uint8_t> bytes;
    return loadFile(path, bytes, error) && parseMesh48B(bytes.data(), bytes.size(), mesh, error);
}
bool loadTexture48B(const char* path, Texture48B& texture, std::string& error) {
    std::vector<std::uint8_t> bytes;
    return loadFile(path, bytes, error) && parseTexture48B(bytes.data(), bytes.size(), texture, error);
}
} }
