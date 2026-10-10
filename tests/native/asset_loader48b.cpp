#include "native_asset48b.h"
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace {
unsigned checks = 0, failures = 0;
void expect(bool value, const char* description) {
    ++checks;
    if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", description); }
}
void appendU32(std::vector<std::uint8_t>& bytes, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) bytes.push_back(static_cast<std::uint8_t>(value >> (i * 8)));
}
void setU32(std::vector<std::uint8_t>& bytes, std::size_t offset, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) bytes[offset + i] = static_cast<std::uint8_t>(value >> (i * 8));
}
void appendFloat(std::vector<std::uint8_t>& bytes, float value) {
    std::uint32_t bits;
    std::memcpy(&bits, &value, 4);
    appendU32(bytes, bits);
}
std::vector<std::uint8_t> triangle() {
    std::vector<std::uint8_t> bytes{'R','R','M','E','S','H','0','1'};
    appendU32(bytes, 3); appendU32(bytes, 3);
    const float data[3][5] = {{10,2,30,-2,0.5f}, {12,2,30,4,0}, {10,4,30,0,1}};
    for (unsigned i = 0; i < 3; ++i) {
        for (float value : data[i]) appendFloat(bytes, value);
        bytes.push_back(static_cast<std::uint8_t>(3 + i)); bytes.push_back(129);
        bytes.push_back(255); bytes.push_back(0);
    }
    appendU32(bytes, 2); appendU32(bytes, 0); appendU32(bytes, 1);
    return bytes;
}
std::vector<std::uint8_t> texture() {
    std::vector<std::uint8_t> bytes{'R','R','T','E','X','0','0','1'};
    appendU32(bytes, 2); appendU32(bytes, 2);
    for (unsigned i = 0; i < 16; ++i) bytes.push_back(static_cast<std::uint8_t>(i * 13));
    return bytes;
}
}
int main() {
    using namespace rr::assets;
    std::string error;
    Mesh48B mesh;
    const auto original = triangle();
    auto bytes = original;
    expect(parseMesh48B(bytes.data(), bytes.size(), mesh, error), "valid mesh");
    expect(error.empty(), "successful parse clears error");
    expect(bytes == original, "loader does not modify source bytes");
    expect(mesh.vertices.size() == 3 && mesh.indices.size() == 3, "counts retained");
    expect(mesh.vertices[0].position[0] == 10 && mesh.vertices[2].position[1] == 4, "original positions retained without recentering");
    expect(mesh.vertices[0].uv[0] == -2 && mesh.vertices[1].uv[0] == 4, "UVs outside zero-one range retained");
    expect(mesh.vertices[0].color[1] == 129 && mesh.vertices[0].color[3] == 0, "RGBA bytes retained");
    expect(mesh.indices[0] == 2 && mesh.indices[1] == 0 && mesh.indices[2] == 1, "triangle winding retained");
    expect(mesh.bounds.minimum[0] == 10 && mesh.bounds.maximum[0] == 12 && mesh.bounds.maximum[2] == 30, "bounds from original positions");
    auto rejectedMesh = [&](const std::vector<std::uint8_t>& candidate, const char* why) {
        const bool accepted = parseMesh48B(candidate.data(), candidate.size(), mesh, error);
        expect(!accepted && !error.empty(), why);
        expect(mesh.vertices.size() == 3 && mesh.indices[0] == 2 && mesh.vertices[0].position[0] == 10,
               "failed mesh parse preserves previous output");
    };
    for (std::size_t length = 0; length < original.size(); ++length)
        rejectedMesh(std::vector<std::uint8_t>(original.begin(), original.begin() + length), "each truncation rejected");
    bytes = original; bytes.push_back(0); rejectedMesh(bytes, "trailing bytes rejected");
    bytes = original; bytes[7] = '2'; rejectedMesh(bytes, "unknown mesh version rejected");
    bytes = original; setU32(bytes, 8, 0); rejectedMesh(bytes, "zero vertices rejected");
    bytes = original; setU32(bytes, 8, max_mesh_vertices + 1); rejectedMesh(bytes, "large vertex count rejected before allocation");
    bytes = original; setU32(bytes, 8, UINT32_MAX); rejectedMesh(bytes, "overflowing vertex arithmetic rejected");
    bytes = original; setU32(bytes, 12, max_mesh_indices + 1); rejectedMesh(bytes, "large index count rejected before allocation");
    bytes = original; setU32(bytes, 12, 0); rejectedMesh(bytes, "zero triangles rejected");
    bytes = original; setU32(bytes, 12, 2); rejectedMesh(bytes, "partial triangle rejected");
    for (unsigned i = 0; i < 3; ++i) {
        bytes = original; setU32(bytes, 88 + i * 4, 3); rejectedMesh(bytes, "every out of bounds index rejected");
        bytes = original; setU32(bytes, 88 + i * 4, UINT32_MAX); rejectedMesh(bytes, "maximum index rejected");
    }
    for (unsigned vertex = 0; vertex < 3; ++vertex)
        for (unsigned field = 0; field < 5; ++field)
            for (std::uint32_t value : {0x7fc00000u, 0x7f800000u, 0xff800000u}) {
                bytes = original; setU32(bytes, 16 + vertex * 24 + field * 4, value);
                rejectedMesh(bytes, "all non-finite vertex fields rejected");
            }
    expect(!parseMesh48B(nullptr, original.size(), mesh, error), "null mesh input rejected");
    bytes = original; setU32(bytes, 16, 0x7f7fffffu);
    Mesh48B largeFinite;
    expect(parseMesh48B(bytes.data(), bytes.size(), largeFinite, error), "finite float maximum parses; renderer validates camera separately");
    bytes = original; setU32(bytes, 88, 0); setU32(bytes, 92, 0);
    Mesh48B degenerate;
    expect(parseMesh48B(bytes.data(), bytes.size(), degenerate, error), "degenerate triangle retained as source topology");

    Texture48B image;
    const auto originalImage = texture();
    expect(parseTexture48B(originalImage.data(), originalImage.size(), image, error), "valid texture");
    expect(image.width == 2 && image.height == 2 && image.rgba.size() == 16, "texture shape retained");
    expect(image.rgba[0] == 0 && image.rgba[15] == 195, "texture pixels unmodified");
    auto rejectedTexture = [&](const std::vector<std::uint8_t>& candidate, const char* why) {
        expect(!parseTexture48B(candidate.data(), candidate.size(), image, error) && !error.empty(), why);
        expect(image.width == 2 && image.rgba.size() == 16 && image.rgba[15] == 195, "failed texture parse preserves output");
    };
    for (std::size_t length = 0; length < originalImage.size(); ++length)
        rejectedTexture(std::vector<std::uint8_t>(originalImage.begin(), originalImage.begin() + length), "every texture truncation rejected");
    bytes = originalImage; bytes.push_back(0); rejectedTexture(bytes, "texture trailing data rejected");
    bytes = originalImage; bytes[0] = 0; rejectedTexture(bytes, "texture magic rejected");
    for (unsigned offset : {8u, 12u}) {
        bytes = originalImage; setU32(bytes, offset, 0); rejectedTexture(bytes, "zero texture dimension rejected");
        bytes = originalImage; setU32(bytes, offset, max_texture_dimension + 1); rejectedTexture(bytes, "oversize texture dimension rejected");
        bytes = originalImage; setU32(bytes, offset, UINT32_MAX); rejectedTexture(bytes, "overflow texture dimension rejected");
    }
    expect(!parseTexture48B(nullptr, originalImage.size(), image, error), "null texture input rejected");
    std::printf("native-asset48b pointer_bits=%u checks=%u failures=%u\n", unsigned(sizeof(void*) * 8), checks, failures);
    return failures ? 1 : 0;
}
