#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/indexed_resource48c.h"
#include "../../src/assets/original_ipu_decoder.h"
#include "../../src/assets/import_world48f.h"
#include <cfenv>
#include <charconv>
#include <iostream>
#include <string>

// The one sky image loader (common rewrite R6, PLAN B-C14): every course's
// [Environment] cube, read from that course's own archive and SCB. Cube
// geometry is the direct 12A8A0 source constructor, not an invented enclosing
// mesh. Only authored face/tile mask bits produce geometry. Images are genuine
// decoded IPUM bytes (largest mip). No course is named: the stock per-course
// pinned rows (archive/CUB/SCB/tile SHA tables, sky_*_index.inc, the GB
// seven-cube loader) are retired; their values are receipts
// (stock_course_receipts50cr.h, roles sky.* and fog.color.*).
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_cold_sources50LA {class Owner;}
namespace sky50gw_fixture {
namespace a = rr::assets;
constexpr std::uint32_t tileCount = 48, vertexCount = 192, indexCount = 288;
constexpr float halfExtent = 25600.0f, cameraRelativeYOffset = 1024.0f;
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
// CUB format, tile walk, image decode, quad construction and Fog.ColorRGB
// syntax. These are format facts every authored single cube uses, not course
// identity.
inline constexpr std::uint16_t cubMasks[6]{255,255,255,255,65535,0};
inline constexpr std::uint32_t cubSizes[6]{0xaaaa,0xaaaa,0xaaaa,0xaaaa,0xaaaaaaaa,0};
inline constexpr float cubSourcePose[9]{0,0,0,0,0,1,0,1,0};
inline constexpr std::size_t cubHeaderOffset = 99100, cubFirstTileOffset = 99180;
// Caller guarantees cub.size() >= cubFirstTileOffset (by an explicit extent check).
inline void requireCubFormat(const Bytes& cub) {
    need(u32(cub.data()) == 1 && u32(cub.data()+4) == 1 &&
        u32(cub.data()+8) == 16 && u32(cub.data()+12) == cubHeaderOffset, "CUB source table");
    const auto* header = cub.data() + cubHeaderOffset;
    need(u32(header) == 32 && u16(header+4) == 0 && u16(header+6) == 0,
        "CUB exact supported texture/flags");
    for (unsigned face = 0; face < 6; ++face)
        need(u16(header+8+face*2) == cubMasks[face] && u32(header+20+face*4) == cubSizes[face],
            "CUB authored tile mask/sizes");
    for (unsigned i = 0; i < 9; ++i)
        need(f32(header+44+i*4) == cubSourcePose[i], "CUB authored origin/look/up");
}
//128D50/24F2C0: exact tile descriptor followed by four IPUM mips 16..128.
// Walks [tile, tile+available) and returns the bytes the tile occupies;
// largest/largestBytes locate its 128x128 mip. Every read is bounded first.
inline std::size_t walkTile(const std::uint8_t* tile, std::size_t available,
    std::size_t& largest, std::size_t& largestBytes) {
    constexpr std::array<std::uint32_t,11> standardDescriptor{0,32,0,16,4096,4,0,0,0,0,0};
    // Both exact source formats occur in authenticated CUB tiles.
    // Select by source bytes, not course identity.
    constexpr std::array<std::uint32_t,11> alternateDescriptor{0,34,0,16,16,4,0,0,0,0,0};
    need(available>=sizeof(standardDescriptor),"Original sky tile descriptor extent");
    bool standard=true,alternate=true;
    for(std::size_t word=0;word<11;++word){
        const auto value=u32(tile+word*4);
        standard=standard&&value==standardDescriptor[word];
        alternate=alternate&&value==alternateDescriptor[word];
    }
    need(standard||alternate,"Original sky tile descriptor");
    std::size_t at=sizeof(standardDescriptor);largest=0;largestBytes=0;
    for(std::uint32_t dimension=16;dimension<=128;dimension*=2){
        need(at<=available&&available-at>=4,"Original sky mip size extent");
        const auto bytes=u32(tile+at);at+=4;
        need(bytes>=25&&bytes<=65536&&bytes<=available-at,"Original sky mip extent");
        const auto* ipum=tile+at;
        need(std::memcmp(ipum,"ipum",4)==0&&u32(ipum+4)==bytes-8&&
            u16(ipum+8)==dimension&&u16(ipum+10)==dimension&&u32(ipum+12)==1,
            "Original sky mip header");
        largest=at;largestBytes=bytes;at+=bytes;
    }
    return at;
}
// Largest mip -> RRTEX001 128x128 RGBA (65552 bytes).
inline void decodeLargestMip(const std::uint8_t* tile, std::size_t largest,
    std::size_t largestBytes, Bytes& image) {
    Bytes rgba;std::string codecError;
    const bool decoded=a::decodeOriginalIpuRgba(tile+largest,largestBytes,
        128,128,rgba,codecError);
    need(decoded,"Original sky IPUM decode: "+codecError);
    need(rgba.size()==128u*128u*4u,"Original sky RGBA extent");
    image={'R','R','T','E','X','0','0','1',128,0,0,0,128,0,0,0};
    image.insert(image.end(),rgba.begin(),rgba.end());
}
inline void appendTileQuad(a::Mesh48B& mesh, std::vector<Span>& spans,
    std::uint32_t face, std::uint32_t tile, std::uint32_t index) {
    const auto v = static_cast<std::uint32_t>(mesh.vertices.size());
    const auto start = static_cast<std::uint32_t>(mesh.indices.size());
    const auto x = (tile % 4) * 128, y = (tile / 4) * 128;
    // Original12B634 and12B34C:UV=.03125+fraction*.9375.
    constexpr float lo = 0.03125f, hi = 0.96875f;
    mesh.vertices.push_back(vertex(face, x, y, lo, lo));
    mesh.vertices.push_back(vertex(face, x, y+128, lo, hi));
    mesh.vertices.push_back(vertex(face, x+128, y, hi, lo));
    mesh.vertices.push_back(vertex(face, x+128, y+128, hi, hi));
    // Preserve source strip order:top-left,bottom-left,top-right,bottom-right.
    for (auto local : {0u,1u,2u,2u,1u,3u}) mesh.indices.push_back(v+local);
    spans.push_back({start,6,v,4,index,face,tile});
}
inline void closeBounds(a::Mesh48B& mesh) {
    auto& bounds = mesh.bounds;
    for (unsigned axis = 0; axis < 3; ++axis) {
        bounds.minimum[axis] = bounds.maximum[axis] = mesh.vertices.front().position[axis];
        for (const auto& vertex : mesh.vertices) {
            bounds.minimum[axis] = std::min(bounds.minimum[axis], vertex.position[axis]);
            bounds.maximum[axis] = std::max(bounds.maximum[axis], vertex.position[axis]);
        }
    }
}
// "R,G,B", each 1..3 decimal digits <= 255, NUL-terminated. Authenticated GA
// has a two-digit blue channel: never read a fixed three-digit stride.
inline void parseFogRgb(const char* digit, std::array<float,4>& clearRgba) {
    for (unsigned channel = 0; channel < 3; ++channel) {
        unsigned value=0,count=0;
        while(*digit>='0'&&*digit<='9'){
            need(++count<=3u,"Original fog channel width");
            value=value*10u+static_cast<unsigned>(*digit++-'0');
        }
        need(count&&value<=255u&&*digit==(channel<2?',':'\0'),"Original fog channel syntax");
        clearRgba[channel] = static_cast<float>(value)/255.0f;
        if(channel<2)++digit;
    }
    clearRgba[3] = 1.0f;
}
} // namespace detail

class Owner final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    // Native source proof of the published images: which archive member and
    // which tile extents produced them (the race World's sky chain re-binds the
    // same bytes, 274AE0 -> 1295B0). Filled for every course by the archive
    // overload of loadFromCourseData. The stock SHA pins are retired: the tile
    // and image digests stay null (receipts are logged by the sky reader).
    // `source` is the former RA cold-source pin; the common load leaves it null
    // (the Track's archive lease keeps the bytes alive).
    struct NativeRaSourceProof {
        enum class DecodeRoute { originalIpuLargestMip, authenticatedPredecoded };
        struct Tile {
            std::uint32_t face=0,tile=0,offset=0,bytes=0;
            const char* sourceSha256=nullptr;
            const char* imageSha256=nullptr;
            const a::MaterialTextureAsset50EA* image=nullptr;
        };
        std::shared_ptr<const ra_cold_sources50LA::Owner> source;
        const a::ResArchive48C* archive=nullptr;
        const std::string* member=nullptr;
        const std::uint8_t* memberData=nullptr;
        std::size_t memberOrdinal=0,memberOffset=0,memberBytes=0;
        const char* memberSha256=nullptr;
        DecodeRoute decodeRoute=DecodeRoute::originalIpuLargestMip;
        std::array<Tile,tileCount> tiles{};
        std::size_t consumedTiles=0;
        const a::Mesh48B* mesh=nullptr;
        const a::MaterialTextureAssets50EA* bank=nullptr;
        const std::vector<Span>* spans=nullptr;
        // Native original-producer proof only: no FileManager first-provider,
        // guest file handle, all-mip decode, source texture refs or GPU receipt.
    };
    struct Storage {
        a::Mesh48B mesh;
        a::MaterialTextureAssets50EA bank;
        std::vector<Span> spans;
        std::array<float,4> clearRgba{};
        std::string cubeMember,cubeSha256;
        std::size_t cubeBytes=0;
        std::uint32_t internalCubes=0;
        std::unique_ptr<NativeRaSourceProof> nativeRaProof;
    };
    std::unique_ptr<Storage> storage_;
    // One body for both archive kinds; extract(member,bytes,error) reads one
    // named member of the course's own archive (sky_course_data50gw.inl).
    template<class Extract> bool loadCourseDataWith(Extract&& extract,const a::World48F& world,Diagnostic& d) noexcept;
public:
    // Course-neutral single-cube load from an already decoded course archive
    // and the World48F decoded from that archive's SCB (the Track retains both;
    // neither is retained here). Defined in sky_course_data50gw.inl. No identity
    // pins: member names, tile extents and clear colour all come from the data;
    // CUB format is still enforced.
    bool loadFromCourseData(const a::ResArchive48C& archive,const a::World48F& world,
        Diagnostic& d) noexcept;
    // Same rule for a course held as an indexed (compact) archive.
    bool loadFromCourseData(const a::IndexedResArchive48C& archive,const a::World48F& world,
        Diagnostic& d) noexcept;
    // Same rule over any member reader: extract(member,bytes,DecodeError48C&).
    template<class Extract> bool loadFromCourseMembers(Extract&& extract,const a::World48F& world,
        Diagnostic& d) noexcept {return loadCourseDataWith(std::forward<Extract>(extract),world,d);}
    Owner() = default;
    Owner(const Owner&) = delete;
    Owner& operator=(const Owner&) = delete;
    Owner(Owner&&) = delete;
    Owner& operator=(Owner&&) = delete;
    bool loaded() const noexcept { return bool(storage_); }
    const a::Mesh48B* mesh() const noexcept { return storage_ ? &storage_->mesh : nullptr; }
    const a::MaterialTextureAssets50EA* bank() const noexcept { return storage_ ? &storage_->bank : nullptr; }
    const std::vector<Span>* spans() const noexcept { return storage_ ? &storage_->spans : nullptr; }
    // The SCB [Fog] ColorRGB, also used by the original viewport clear.
    // Four immutable normalized floats; null before successful publication.
    const float* clearRgba() const noexcept { return storage_ ? storage_->clearRgba.data() : nullptr; }
    // The CUB member read (skies\<CubeFile1>), its bytes and its internal cube count.
    const std::string* cubeMember() const noexcept { return storage_ ? &storage_->cubeMember : nullptr; }
    std::size_t cubeBytes() const noexcept { return storage_ ? storage_->cubeBytes : 0u; }
    std::string cubeSha256() const { return storage_ ? storage_->cubeSha256 : std::string(); }
    std::uint32_t internalCubes() const noexcept { return storage_ ? storage_->internalCubes : 0u; }
};
} // namespace sky50gw_fixture

#include "sky_course_data50gw.inl"
