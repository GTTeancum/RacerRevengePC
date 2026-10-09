#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include <cfenv>

// Cold TA-only bridge for original skies\ta.cub. Cube geometry is the direct
// 12A8A0 source constructor, not an invented enclosing mesh. Only authored
// face/tile mask bits produce geometry. Images are genuine decoded IPUM bytes.
namespace sky50gw_fixture {
namespace a = rr::assets;
constexpr std::uint32_t tileCount = 48, vertexCount = 192, indexCount = 288;
constexpr float halfExtent = 25600.0f, cameraRelativeYOffset = 1024.0f;
struct Request {
    std::filesystem::path archive; // Exact retained TRACKS/TA.RES.
    std::filesystem::path textureRoot; // evidence/sky50GW; no runtime codec.
};
struct Span {
    std::uint32_t indexStart = 0, indexCount = 0, vertexStart = 0, vertexCount = 0;
    std::uint32_t textureIndex = 0, face = 0, tile = 0;
};
struct Diagnostic {
    std::string message;
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
namespace shared = pod_materials50ed_fixture::detail;
using shared::need; using shared::u32; using shared::sha256;
using Bytes = std::vector<std::uint8_t>;
struct Entry {
    std::uint32_t face, tile, offset, bytes;
    const char* sourceSha256;
    const char* filename;
    const char* imageSha256;
};
inline constexpr Entry entries[] = {
#include "sky50gw_index.inc"
};
static_assert(sizeof(entries) / sizeof(entries[0]) == tileCount, "Actual authored TA tile count");
inline std::uint16_t u16(const std::uint8_t* p) noexcept {
    return std::uint16_t(p[0]) | std::uint16_t(std::uint16_t(p[1]) << 8);
}
inline float f32(const std::uint8_t* p) noexcept {
    const auto bits = u32(p); float f; std::memcpy(&f, &bits, 4); return f;
}
// 12A978..12AB00, switch table 389020. x/y are original 0..512
// grid coordinates, and every tile is 128 units wide before the 100 scale.
inline a::Vertex48B vertex(std::uint32_t face, std::uint32_t x,
    std::uint32_t y, float u, float v) {
    a::Vertex48B out{};
    const float right = (float(x) - 256.0f) * 100.0f;
    const float up = (256.0f - float(y)) * 100.0f;
    switch (face) {
    case 0: out.position[0] = right; out.position[1] = up; out.position[2] = halfExtent; break;
    case 1: out.position[0] = -halfExtent; out.position[1] = up; out.position[2] = right; break;
    case 2: out.position[0] = -right; out.position[1] = up; out.position[2] = -halfExtent; break;
    case 3: out.position[0] = halfExtent; out.position[1] = up; out.position[2] = -right; break;
    case 4: out.position[0] = right; out.position[1] = halfExtent; out.position[2] = -up; break;
    default: throw std::runtime_error("Unauthored sky face");
    }
    // 274BB0 supplies1024 to288A90;288B00 adds it to current cameraY.
    // Keep camera translation outside this immutable mesh; include only the
    // original relative offset. Caller uses an orientation-only sky view.
    out.position[1] += cameraRelativeYOffset;
    out.uv[0] = u; out.uv[1] = v;
    std::fill(std::begin(out.color), std::end(out.color), std::uint8_t(255));
    return out;
}
} // namespace detail

class Owner final {
    struct Storage {
        a::Mesh48B mesh;
        a::MaterialTextureAssets50EA bank;
        std::vector<Span> spans;
        std::array<float,4> clearRgba{};
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner() = default;
    Owner(const Owner&) = delete;
    Owner& operator=(const Owner&) = delete;
    Owner(Owner&&) = delete;
    Owner& operator=(Owner&&) = delete;
    bool loaded() const noexcept { return bool(storage_); }
    const a::Mesh48B* mesh() const noexcept { return storage_ ? &storage_->mesh : nullptr; }
    const a::MaterialTextureAssets50EA* bank() const noexcept { return storage_ ? &storage_->bank : nullptr; }
    const std::vector<Span>* spans() const noexcept { return storage_ ? &storage_->spans : nullptr; }
    // Exact TA.scb Fog.ColorRGB, also used by the original viewport clear.
    // Four immutable normalized floats; null before successful publication.
    const float* clearRgba() const noexcept { return storage_ ? storage_->clearRgba.data() : nullptr; }

    // Transactional, bounded cold load. No fallback textures and no runtime
    // filesystem lookups, allocations or codec work after success. Retain
    // owner through all dependent GPU uploads. The mesh already includesY+1024.
    bool load(const Request& request, Diagnostic& d) noexcept {
        using namespace detail;
        try {
            d = {};
            need(!storage_, "Sky assets already loaded");
            need(std::fegetround() == FE_TONEAREST, "Sky geometry requires FE_TONEAREST");
            need(!request.archive.empty() && !request.textureRoot.empty(), "Explicit sky source paths required");
            need(!std::filesystem::is_symlink(request.textureRoot), "Sky texture root symlink rejected");
            shared::Inputs input; input.capacity = 8u * 1024u * 1024u;
            auto archiveBytes = input.read(request.archive, 3334562);
            need(archiveBytes.size() == 3334562 && sha256(archiveBytes) ==
                "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8", "Original sky archive identity");
            a::ResArchive48C archive; a::DecodeError48C error;
            a::DecodeLimits48C limits;
            limits.resource_file_bytes = 3334562;
            limits.resource_decoded_bytes = 16u * 1024u * 1024u;
            limits.resource_entries = 383;
            limits.resource_name_bytes = 128;
            need(a::decodeResource48C(archiveBytes, archive, error, nullptr, limits),
                "Original sky archive decode: " + error.detail);
            Bytes cub;
            need(a::extractResMember48C(archive, "skies\\ta.cub", cub, error, 164187),
                "Original sky member extraction: " + error.detail);
            need(cub.size() == 164187 && sha256(cub) ==
                "27e43db810442e9fd90fda8551e6804ab68d48e8398992a1b2997945e88d68f4", "Original CUB identity");
            need(u32(cub.data()) == 1 && u32(cub.data()+4) == 1 &&
                u32(cub.data()+8) == 16 && u32(cub.data()+12) == 99100, "CUB source table");
            const auto* header = cub.data() + 99100;
            need(u32(header) == 32 && u16(header+4) == 0 && u16(header+6) == 0,
                "CUB exact supported texture/flags");
            constexpr std::uint16_t masks[6]{255,255,255,255,65535,0};
            constexpr std::uint32_t sizes[6]{0xaaaa,0xaaaa,0xaaaa,0xaaaa,0xaaaaaaaa,0};
            constexpr float sourcePose[9]{0,0,0,0,0,1,0,1,0};
            for (unsigned face = 0; face < 6; ++face)
                need(u16(header+8+face*2) == masks[face] && u32(header+20+face*4) == sizes[face],
                    "CUB authored tile mask/sizes");
            for (unsigned i = 0; i < 9; ++i)
                need(f32(header+44+i*4) == sourcePose[i], "CUB authored origin/look/up");

            auto next = std::make_unique<Storage>();
            Bytes sceneParameters;
            need(a::extractResMember48C(archive, "tracks\\ta.scb", sceneParameters, error, 130511),
                "Original sky clear parameters extraction: " + error.detail);
            need(sceneParameters.size() == 130511 && sha256(sceneParameters) ==
                "c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d",
                "Original TA scene parameter identity");
            // Exact BINARY1 Fog.ColorRGB field, verified by the existing
            // reversible parameter decoder. Full member authentication makes
            // this a bounded fixed fixture field, not a permissive text parser.
            constexpr std::size_t fogRgbOffset = 9928;
            constexpr char fogRgb[] = "175,182,207";
            need(std::memcmp(sceneParameters.data()+fogRgbOffset, fogRgb, sizeof fogRgb) == 0,
                "Original TA Fog.ColorRGB field");
            for (unsigned channel = 0; channel < 3; ++channel) {
                const auto* digits = sceneParameters.data()+fogRgbOffset+channel*4;
                const unsigned value = (digits[0]-'0')*100u+(digits[1]-'0')*10u+(digits[2]-'0');
                next->clearRgba[channel] = static_cast<float>(value)/255.0f;
            }
            next->clearRgba[3] = 1.0f;
            next->mesh.vertices.reserve(vertexCount); next->mesh.indices.reserve(indexCount);
            next->spans.reserve(tileCount);
            std::array<Bytes, tileCount> images;
            std::array<a::MaterialTextureAssetInput50EA, tileCount> textures{};
            std::array<a::MaterialTextureSlotInput50EA, tileCount> slots{};
            std::size_t offset = 99180; std::uint32_t index = 0;
            for (std::uint32_t face = 0; face < 6; ++face) for (std::uint32_t tile = 0; tile < 16; ++tile) {
                if (!(masks[face] & (1u << tile))) continue;
                need(index < tileCount, "Sky tile count bound");
                const auto& e = entries[index];
                need(e.face == face && e.tile == tile && e.offset == offset && e.bytes <= cub.size()-offset,
                    "Sky source tile sequence/extent");
                Bytes source(cub.begin()+offset, cub.begin()+offset+e.bytes);
                need(sha256(source) == e.sourceSha256, "Sky source tile identity");
                offset += e.bytes;
                images[index] = input.read(request.textureRoot / e.filename, 65552);
                need(images[index].size() == 65552 && sha256(images[index]) == e.imageSha256 &&
                    std::memcmp(images[index].data(), "RRTEX001", 8) == 0 &&
                    u32(images[index].data()+8) == 128 && u32(images[index].data()+12) == 128,
                    "Genuine decoded sky image identity/dimensions");
                textures[index] = {index, &images[index]};
                slots[index] = {{0, index, 1, 1}, index, a::MaterialTextureRequirement50EA::required};
                const auto v = static_cast<std::uint32_t>(next->mesh.vertices.size());
                const auto start = static_cast<std::uint32_t>(next->mesh.indices.size());
                const auto x = (tile % 4) * 128, y = (tile / 4) * 128;
                // Original12B634 and12B34C:UV=.03125+fraction*.9375.
                constexpr float lo = 0.03125f, hi = 0.96875f;
                next->mesh.vertices.push_back(vertex(face, x, y, lo, lo));
                next->mesh.vertices.push_back(vertex(face, x, y+128, lo, hi));
                next->mesh.vertices.push_back(vertex(face, x+128, y, hi, lo));
                next->mesh.vertices.push_back(vertex(face, x+128, y+128, hi, hi));
                // Preserve source strip order:top-left,bottom-left,top-right,bottom-right.
                for (auto local : {0u,1u,2u,2u,1u,3u}) next->mesh.indices.push_back(v+local);
                next->spans.push_back({start,6,v,4,index,face,tile});
                ++index;
            }
            need(index == tileCount && offset == cub.size(), "Complete CUB source tile closure");
            auto& bounds = next->mesh.bounds;
            for (unsigned axis = 0; axis < 3; ++axis) {
                bounds.minimum[axis] = bounds.maximum[axis] = next->mesh.vertices.front().position[axis];
                for (const auto& vertex : next->mesh.vertices) {
                    bounds.minimum[axis] = std::min(bounds.minimum[axis], vertex.position[axis]);
                    bounds.maximum[axis] = std::max(bounds.maximum[axis], vertex.position[axis]);
                }
            }
            const a::MaterialTextureAssetsInput50EA bankInput{textures.data(), tileCount, slots.data(), tileCount,
                {tileCount,tileCount,3146496,3145728,128}};
            need(next->bank.load(bankInput, d.bank), "Sky texture bank: " + d.bank.parserMessage);
            storage_ = std::move(next); return true;
        } catch (const std::exception& e) { try { d.message = e.what(); } catch (...) {} return false; }
        catch (...) { try { d.message = "Unexpected cold sky load failure"; } catch (...) {} return false; }
    }
};
} // namespace sky50gw_fixture
