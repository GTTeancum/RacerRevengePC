#pragma once
#include "pod_materials50ed_fixture.h"
#include "track_source_fixture.h"
#include "mb_original_world50lm.h"
#include "tc_original_world50lm.h"
#include "rc_cold_sources50la.h"
#include "tb_cold_sources50la.h"
#include "gb_cold_sources50la.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/original_ipu_decoder.h"
#include <cfenv>

// Cold authenticated bridge for original TA/GC/GA/MA/SA/RA/MB/TC sky members. Cube geometry is the direct
// 12A8A0 source constructor, not an invented enclosing mesh. Only authored
// face/tile mask bits produce geometry. Images are genuine decoded IPUM bytes.
namespace sky50gw_fixture {
namespace a = rr::assets;
constexpr std::uint32_t tileCount = 48, vertexCount = 192, indexCount = 288;
constexpr float halfExtent = 25600.0f, cameraRelativeYOffset = 1024.0f;
struct Request {
    std::filesystem::path archive; // Exact archive for the explicitly selected track.
    std::filesystem::path textureRoot; // Optional explicit predecoded compatibility input.
    track_source_fixture::Track track = track_source_fixture::Track::ta;
    // Cold-only borrow from the SAME retained RA aggregate; never retained by this owner.
    // Both CUB and SCB are authenticated below, with no second archive decode.
    const a::ResArchive48C* retainedArchive = nullptr;
    bool decodeOriginalImages = false; // Cold in-process decode from this same authenticated CUB.
    // Typed cold borrow from the loaded authenticated MB world. The pointer and
    // its archive are used only during load(); no borrowed storage is retained.
    // Naked retainedArchive stays RA-only, and both borrows cannot be supplied.
    const mb_original_world50LM::Owner* retainedMbWorld = nullptr;
    // Appended typed TC cold borrow. Its own authenticated archive contains
    // byte-identical ta.cub; source identity and zone selection remain TC.
    const tc_original_world50LM::Owner* retainedTcWorld = nullptr;
    // Synchronous cold borrow only; no archive decode or retained source copy.
    const rc_cold_sources50LA::Owner* retainedRcSource = nullptr;
    // Appended typed synchronous borrow from TB's authentic indexed source.
    const tb_cold_sources50LA::Owner* retainedTbSource = nullptr;
    std::shared_ptr<const gb_cold_sources50LA::Owner> retainedGbSource; // Seven internal GB cubes, same retained source.
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
inline constexpr Entry gcEntries[] = {
#include "sky_gc_index.inc"
};
static_assert(sizeof(gcEntries) / sizeof(gcEntries[0]) == tileCount, "Actual authored GC tile count");
#if __has_include("sky_ga_index.inc")
inline constexpr Entry gaEntries[] = {
#include "sky_ga_index.inc"
};
static_assert(sizeof(gaEntries) / sizeof(gaEntries[0]) == tileCount, "Actual authored GA tile count");
#endif
#if __has_include("sky_ma_index.inc")
inline constexpr Entry maEntries[] = {
#include "sky_ma_index.inc"
};
static_assert(sizeof(maEntries)/sizeof(maEntries[0]) == tileCount, "Actual authored MA tile count");
#endif
#if __has_include("sky_sa_index.inc")
inline constexpr Entry saEntries[] = {
#include "sky_sa_index.inc"
};
static_assert(sizeof(saEntries)/sizeof(saEntries[0]) == tileCount, "Actual authored SA tile count");
#endif
#if __has_include("sky_ra_index.inc")
inline constexpr Entry raEntries[] = {
#include "sky_ra_index.inc"
};
static_assert(sizeof(raEntries)/sizeof(raEntries[0]) == tileCount, "Actual authored RA tile count");
#endif
#if __has_include("sky_mb_index.inc")
inline constexpr Entry mbEntries[] = {
#include "sky_mb_index.inc"
};
static_assert(sizeof(mbEntries)/sizeof(mbEntries[0]) == tileCount, "Actual authored MB tile count");
#endif
inline constexpr Entry rcEntries[] = {
#include "sky_rc_index.inc"
};
static_assert(sizeof(rcEntries)/sizeof(rcEntries[0]) == tileCount, "Actual authored RC tile count");
inline constexpr Entry tbEntries[] = {
#include "sky_tb_index.inc"
};
static_assert(sizeof(tbEntries)/sizeof(tbEntries[0]) == tileCount, "Actual authored TB tile count");
struct Source {
    std::size_t archiveBytes, decodedCapacity, cubBytes, sceneBytes;
    std::uint32_t archiveEntries;
    const char* archiveSha; const char* cubMember; const char* cubSha;
    const char* sceneMember; const char* sceneSha;
    std::size_t fogRgbOffset;
    const char* fogRgb;
    const Entry* tiles;
};
inline const Source& source(track_source_fixture::Track track) {
    static const Source ta{3334562,16u*1024u*1024u,164187,130511,383,
        "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8",
        "skies\\ta.cub","27e43db810442e9fd90fda8551e6804ab68d48e8398992a1b2997945e88d68f4",
        "tracks\\ta.scb","c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d",
        9928,"175,182,207",entries};
    static const Source gc{4205067,32u*1024u*1024u,184132,359830,1190,
        "69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",
        "skies\\sky21.cub","676a6601d962d66210d80f0ca97f2d6ad7a7b6eb43d7ba55c164a4792d6b392c",
        "tracks\\gc.scb","d47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d",
        27716,"151,160,177",gcEntries};
#if __has_include("sky_ga_index.inc")
    static const Source ga{2834942,32u*1024u*1024u,193885,292650,970,
        "063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9",
        "skies\\ga.cub","a7a39f4efab6ff032293846f90429d4235cfc3ec9d16aea178b59108faf67141",
        "tracks\\ga.scb","ad38b0d47efdda92e36bf36f9b380f7f3111698cb068532068a19229097de5d7",
        21908,"117,107,66",gaEntries};
#endif
#if __has_include("sky_ma_index.inc")
    static const Source ma{3691592,32u*1024u*1024u,227791,404992,485,
        "2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228",
        "skies\\ma.cub","c8bc416fca9f89351a76b7acd23195d661783fde6dcd3d9050f535caf8beae61",
        "tracks\\ma.scb","5766af3e7a0bba0644058ae6f31eab802cadd9be7cc71911c349f603c7ca873b",
        29699,"166,173,184",maEntries};
#endif
#if __has_include("sky_sa_index.inc")
    static const Source sa{3786696,32u*1024u*1024u,242613,813441,1065,
        "34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a",
        "skies\\sullust.cub","0cb2c1389889318ec806edafe7779ae74fde5d4dc1fc2040e3a4e36da6cfd61d",
        "tracks\\sa.scb","dd38f087f0438b2e1279651fe3b4f525b053c90d34cf5365d6bf4ec6c7d1d394",
        58084,"18,21,22",saEntries};
#endif
#if __has_include("sky_ra_index.inc")
    static const Source ra{4604561,32u*1024u*1024u,161825,540795,1010,
        "1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2",
        "skies\\ra.cub","c497927d2fe78de6f2f9bbf9e319ac0ea2b4939e020df45a4ae570e5abb73960",
        "tracks\\ra.scb","57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81",
        38633,"180,180,165",raEntries};
#endif
#if __has_include("sky_mb_index.inc")
    static const Source mb{4421779,16u*1024u*1024u,210066,621304,487,
        "6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca",
        "skies\\mb.cub","31cff4858fd5132ab838102877e21c2d14077e12e1a86fae77dbbf9587efcdd0",
        "tracks\\mb.scb","e64bd3c7085bd92ef0e6486bbfc2fcfed7aae4462858dcb73a5ef3c893233148",
        37044,"177,201,230",mbEntries};
#endif
    static const Source tc{3607868,16u*1024u*1024u,164187,232284,509,
        "7b5486f4029dabe137919127c879faef92b8bd2fb9df5c0d1a1d122e92731490",
        "skies\\ta.cub","27e43db810442e9fd90fda8551e6804ab68d48e8398992a1b2997945e88d68f4",
        "tracks\\tc.scb","795bbbf9882baf233b9d67f777d0bb1e36506dda0f1ad7e71a303faae3993d24",
        17753,"85,91,102",entries};
    static const Source rc{4655141,32u*1024u*1024u,167607,468918,1121,
        "ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b",
        "skies\\rcsky.cub","3f8a28b1d91d127638cb8e70cb996107968e135a0e9704bfba4cd98931697436",
        "tracks\\rc.scb","1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a",
        35404,"0,0,0",rcEntries};
    static const Source tb{3749557,32u*1024u*1024u,184730,448107,1465,
        "1188206d7b1f64910c119192829fc357b9d4a9f021d64e8a2b7561f2c113a09b",
        "skies\\tb.cub","fe992b1ccf89f28fa1f47ebe1ad1958e9f0e459a408e9341edf732ae95d95330",
        "tracks\\tb.scb","c3536c6e76dc4d6836ee154aabc80f5e73bd80e85189b841538b3a3ce53e8184",
        32324,"95,71,79",tbEntries};
    switch (track) {
    case track_source_fixture::Track::tb: return tb;
    case track_source_fixture::Track::rc: return rc;
    case track_source_fixture::Track::ta: return ta;
    case track_source_fixture::Track::tc: return tc;
    case track_source_fixture::Track::gc: return gc;
    case track_source_fixture::Track::ga:
#if __has_include("sky_ga_index.inc")
        return ga;
#else
        throw std::runtime_error("GA sky bytes have not been decoded and pinned by authorized cold preparation");
#endif
    case track_source_fixture::Track::ma:
#if __has_include("sky_ma_index.inc")
        return ma;
#else
        throw std::runtime_error("MA sky bytes have not been cold-prepared and pinned");
#endif
    case track_source_fixture::Track::sa:
#if __has_include("sky_sa_index.inc")
        return sa;
#else
        throw std::runtime_error("SA sky bytes have not been cold-prepared and pinned");
#endif
    case track_source_fixture::Track::ra:
#if __has_include("sky_ra_index.inc")
        return ra;
#else
        throw std::runtime_error("RA sky bytes have not been cold-prepared and pinned");
#endif
    case track_source_fixture::Track::mb:
#if __has_include("sky_mb_index.inc")
        return mb;
#else
        throw std::runtime_error("MB sky bytes have not been cold-decoded and pinned");
#endif
    default: throw std::runtime_error("Unsupported original sky track identity");
    }
}
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
    std::shared_ptr<const gb_cold_sources50LA::Owner> gbSource_;
    bool loadGb(const Request&,Diagnostic&) noexcept;
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
    // Exact selected SCB Fog.ColorRGB, also used by the original viewport clear.
    // Four immutable normalized floats; null before successful publication.
    const float* clearRgba() const noexcept { return storage_ ? storage_->clearRgba.data() : nullptr; }

    // Transactional, bounded cold load. No fallback textures and no runtime
    // filesystem lookups, allocations or codec work after success. Retain
    // owner through all dependent GPU uploads. The mesh already includesY+1024.
    bool load(const Request& request, Diagnostic& d) noexcept {
        using namespace detail;
        if(request.track==track_source_fixture::Track::gb)return loadGb(request,d);
        if(request.retainedGbSource){d.message="GB source cannot supply another track sky";return false;}
        try {
            d = {};
            need(!storage_, "Sky assets already loaded");
            need(std::fegetround() == FE_TONEAREST, "Sky geometry requires FE_TONEAREST");
            need(unsigned(bool(request.retainedArchive))+unsigned(bool(request.retainedMbWorld))+
                unsigned(bool(request.retainedTcWorld))+unsigned(bool(request.retainedRcSource))+
                unsigned(bool(request.retainedTbSource))<=1u, "Ambiguous retained sky owners");
            need(!request.retainedArchive || request.track == track_source_fixture::Track::ra, "Retained sky archive requires explicit RA identity");
            const a::ResArchive48C* retainedArchive=request.retainedArchive;
            if(request.retainedMbWorld){
                need(request.track==track_source_fixture::Track::mb && request.retainedMbWorld->loaded() &&
                    request.retainedMbWorld->archive(), "Retained MB sky requires the actual loaded MB world owner");
                retainedArchive=request.retainedMbWorld->archive();
                need(retainedArchive->entries.size()==487u && retainedArchive->decoded.size()==16392192u,
                    "Retained MB archive shape differs");
            }
            if(request.retainedTcWorld){
                need(request.track==track_source_fixture::Track::tc && request.retainedTcWorld->loaded() &&
                    request.retainedTcWorld->archive(), "Retained TC sky requires the actual loaded TC world owner");
                retainedArchive=request.retainedTcWorld->archive();
                need(retainedArchive->entries.size()==509u && retainedArchive->decoded.size()==16048128u,
                    "Retained TC archive shape differs");
            }
            need(request.track!=track_source_fixture::Track::rc||request.retainedRcSource,
                "RC sky requires the actual retained compact source");
            need(!request.retainedRcSource||(request.track==track_source_fixture::Track::rc&&
                request.retainedRcSource->ready()&&request.decodeOriginalImages),
                "RC sky requires its ready source and original image decoding");
            need(request.track!=track_source_fixture::Track::tb||request.retainedTbSource,
                "TB sky requires the actual retained compact source");
            need(!request.retainedTbSource||(request.track==track_source_fixture::Track::tb&&
                request.retainedTbSource->ready()&&request.decodeOriginalImages),
                "TB sky requires its ready source and original image decoding");
            need((!request.archive.empty() || retainedArchive || request.retainedRcSource || request.retainedTbSource) &&
                (request.decodeOriginalImages || !request.textureRoot.empty()), "Explicit sky source paths required");
            need(request.decodeOriginalImages || !std::filesystem::is_symlink(request.textureRoot), "Sky texture root symlink rejected");
            const auto& selected = source(request.track);
            shared::Inputs input; input.capacity = 8u * 1024u * 1024u;
            a::ResArchive48C decodedArchive; a::DecodeError48C error;
            if (!retainedArchive && !request.retainedRcSource && !request.retainedTbSource) {
                auto archiveBytes = input.read(request.archive, selected.archiveBytes);
                need(archiveBytes.size() == selected.archiveBytes && sha256(archiveBytes) == selected.archiveSha, "Original sky archive identity");
                a::DecodeLimits48C limits;
                limits.resource_file_bytes = selected.archiveBytes;
                limits.resource_decoded_bytes = selected.decodedCapacity;
                limits.resource_entries = selected.archiveEntries;
                limits.resource_name_bytes = 128;
                need(a::decodeResource48C(archiveBytes, decodedArchive, error, nullptr, limits),
                    "Original sky archive decode: " + error.detail);
            }
            const auto& archive = retainedArchive ? *retainedArchive : decodedArchive;
            Bytes cub;
            need(request.retainedTbSource?
                request.retainedTbSource->extractMember(selected.cubMember,cub,error,selected.cubBytes):
                request.retainedRcSource?
                request.retainedRcSource->extractMember(selected.cubMember,cub,error,selected.cubBytes):
                a::extractResMember48C(archive, selected.cubMember, cub, error, selected.cubBytes),
                "Original sky member extraction: " + error.detail);
            need(cub.size() == selected.cubBytes && sha256(cub) == selected.cubSha, "Original CUB identity");
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
            Bytes extractedSceneParameters;
            if(!request.retainedRcSource&&!request.retainedTbSource)need(a::extractResMember48C(archive, selected.sceneMember,
                extractedSceneParameters,error,selected.sceneBytes),
                "Original sky clear parameters extraction: " + error.detail);
            const auto& sceneParameters=request.retainedTbSource?
                request.retainedTbSource->world().parameters.original_bytes:request.retainedRcSource?
                request.retainedRcSource->world().parameters.original_bytes:extractedSceneParameters;
            need(sceneParameters.size() == selected.sceneBytes && sha256(sceneParameters) == selected.sceneSha,
                "Original scene parameter identity");
            // Exact BINARY1 Fog.ColorRGB field, verified by the existing
            // reversible parameter decoder. Full member authentication makes
            // this a bounded fixed fixture field, not a permissive text parser.
            const auto fogRgbOffset = selected.fogRgbOffset;
            const auto colorBytes=std::strlen(selected.fogRgb)+1u;
            need(fogRgbOffset<=sceneParameters.size() && colorBytes<=sceneParameters.size()-fogRgbOffset &&
                std::memcmp(sceneParameters.data()+fogRgbOffset, selected.fogRgb, colorBytes) == 0,
                "Original Fog.ColorRGB field");
            // Authenticated GA has a two-digit blue channel. Never read a
            // fixed three-digit stride through the next field's value bytes.
            const char* digit=selected.fogRgb;
            for (unsigned channel = 0; channel < 3; ++channel) {
                unsigned value=0,count=0;
                while(*digit>='0'&&*digit<='9'){
                    need(++count<=3u,"Original fog channel width");
                    value=value*10u+static_cast<unsigned>(*digit++-'0');
                }
                need(count&&value<=255u&&*digit==(channel<2?',':'\0'),"Original fog channel syntax");
                next->clearRgba[channel] = static_cast<float>(value)/255.0f;
                if(channel<2)++digit;
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
                const auto& e = selected.tiles[index];
                need(e.face == face && e.tile == tile && e.offset == offset && e.bytes <= cub.size()-offset,
                    "Sky source tile sequence/extent");
                Bytes source(cub.begin()+offset, cub.begin()+offset+e.bytes);
                need(sha256(source) == e.sourceSha256, "Sky source tile identity");
                offset += e.bytes;
                if(request.decodeOriginalImages){
                    //128D50/24F2C0: exact tile descriptor followed by four IPUM mips.
                    constexpr std::array<std::uint32_t,11> standardDescriptor{0,32,0,16,4096,4,0,0,0,0,0};
                    // Exact descriptor from all48 authenticated Sullust CUB tiles.
                    // Keep source/output image pins and four IPUM mip checks below.
                    constexpr std::array<std::uint32_t,11> saDescriptor{0,34,0,16,16,4,0,0,0,0,0};
                    const auto& descriptor=request.track==track_source_fixture::Track::sa?
                        saDescriptor:standardDescriptor;
                    need(source.size()>=sizeof(descriptor),"Original sky tile descriptor extent");
                    for(std::size_t word=0;word<11;++word)
                        need(u32(source.data()+word*4)==descriptor[word],"Original sky tile descriptor");
                    std::size_t at=sizeof(descriptor),largest=0,largestBytes=0;
                    for(std::uint32_t dimension=16;dimension<=128;dimension*=2){
                        need(at<=source.size()&&source.size()-at>=4,"Original sky mip size extent");
                        const auto bytes=u32(source.data()+at);at+=4;
                        need(bytes>=25&&bytes<=65536&&bytes<=source.size()-at,"Original sky mip extent");
                        const auto* ipum=source.data()+at;
                        need(std::memcmp(ipum,"ipum",4)==0&&u32(ipum+4)==bytes-8&&
                            u16(ipum+8)==dimension&&u16(ipum+10)==dimension&&u32(ipum+12)==1,
                            "Original sky mip header");
                        largest=at;largestBytes=bytes;at+=bytes;
                    }
                    need(at==source.size(),"Original sky tile mip closure");
                    Bytes rgba;std::string codecError;
                    const bool decoded=a::decodeOriginalIpuRgba(source.data()+largest,largestBytes,
                        128,128,rgba,codecError);
                    need(decoded,"Original sky IPUM decode: "+codecError);
                    need(rgba.size()==128u*128u*4u,"Original sky RGBA extent");
                    images[index]={'R','R','T','E','X','0','0','1',128,0,0,0,128,0,0,0};
                    images[index].insert(images[index].end(),rgba.begin(),rgba.end());
                }else images[index] = input.read(request.textureRoot / e.filename, 65552);
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

#include "gb_sky_loader50gb.inl"
