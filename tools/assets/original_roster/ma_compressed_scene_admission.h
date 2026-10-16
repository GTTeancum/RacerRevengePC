#pragma once
// Cold preparation only: exact original MA compressed-position admission.
// Default callers receive the existing strict V4_16 limits. No source execution.
#include "assets/import_decode48c.h"
#include "assets/import_scene48d.h"
#include "assets/import_texture48e.h"
#include <array>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace ma_compressed_scene {
namespace detail {
using Bytes = std::vector<std::uint8_t>;
struct Identity { const char* name; std::size_t bytes; const char* sha256; };
// Raw identities from the authenticated original MA.RES intake. These are
// source-admission pins, not geometry, colors, decoded output or a substitute.
constexpr Identity wideScenes[] = {
    {"geometry/gate_column.psg", 8656, "c06d66c986127d6e9ff9cf32071efb8bc0d619528bcf89cea447ac34b19780d4"},
    {"geometry/gate_wall.psg", 95708, "1041ca49017ec73edeeee616de859238a31695194841152b5cb799b768ba0fff"},
    {"geometry/gate_wall-deco.psg", 20816, "45e0ecabd5374d9cc2ccd563ff771f862b79e92248f8fddfbf90b9dfecbedd56"},
    {"geometry/stadium_stands.psg", 33492, "48541365d91880f036dee06e97bd8878f62aa9ec4ab82e7f515a36e4d6f20c0e"},
    {"geometry/steps02.psg", 30280, "369b32056da517515b22ea2ca6dcc586327e7ec4104e563b678f58be2c1a1c5a"},
    {"geometry/arch_a.psg", 15584, "19fa5dbfa82a789f19ff0dabb68f63f252cb70e7f3b0a3c1fa9bebc1bc6dad97"},
    {"geometry/arch_d.psg", 19088, "f47c4eee670a6fb0a2f1d15358e01adc3363d6ee7cbba06b479c624a9e99b973"},
    {"geometry/bonerail_a.psg", 93200, "7d7c2b99cfb1e2d2ee1cbdf145f2f0a826396c47f4dc5f58410152aa174a492d"},
    {"geometry/shell_road-support-lt.psg", 16064, "5d068b0e3512245d2b8e6d9b5ef0fc179df08a56b27a5c5140826b2d66c14780"},
    {"geometry/shell_road-support-rt.psg", 16288, "b27c609ef5a559de379ccb3a5428872f04530774746861e2c80d44b42cf94b58"},
    {"geometry/shellbld_railcap.psg", 66672, "ad644cf244a5b1ab10cd909a33fdb82747061fd675f830a5e0ea34699254f9e3"},
    {"geometry/hoh_bone-a.psg", 11856, "91f7875e0554c994e89826c53f4dd137fd5ec5ba224c67ca80eaf4c03859eb44"},
    {"geometry/hoh_wall-shell.psg", 5264, "b1e627e3f4902399ca501189518fbce56cf63f248ee0c9c2f4efde1125a6b122"},
    {"geometry/hoh_wall-shell-light.psg", 800, "a5d887d607a3297b9c31b844cf66927d227f956ebf47268d5d95da5db1b911a9"},
    {"geometry/hoh_wall-shell-lightcone.psg", 1792, "6d4557a07a9c2a9bf76106d85aec89446fc5f308124ccc86760913230ff0b483"},
    {"geometry/statue_lightbeam.psg", 1024, "9b1c9077f9e2b4cf091c2ea9e73a0cd73986cc261f91bb956888cf6b69715384"},
    {"geometry/hoh_exit.psg", 83588, "85e49158e5593450b1a4e8153bc4e78f35eec05dd96b7580d47de47c3ed777d0"},
    {"geometry/hoh_floor02.psg", 976, "ea82540b139302167ca4f9b782ab2019b2fe4b839d877870d6e9553725f5dda0"},
    {"geometry/steps01.psg", 109192, "a12de94fb36a1dd738f601a623bb003676980ab531c15529cc81c041bb4319b3"},
    {"geometry/terrace.psg", 129784, "03622500493d1ba6f2a80ac7c3c1ddf0f525182ec5decb43f7b3c10ae1c428c3"},
    {"geometry/hoh_entrance.psg", 106404, "d9c3fdf1e25f0a1098cb02adfe29a96f8acf38d71412afa427a29c7d8c8f6da4"},
};
constexpr Identity compressedMaterials[] = {
    {"textures/bld_ledge-b.psm", 264, "e838d19113250a592321dffdfb105e7a026d4c573f4bf26a35b6ecb61e8a1027"},
    {"textures/bonerim_a.psm", 262, "a2da9d57716475df6527af6a6d7e737cf623bb62af70b6f7a240463eac59f4ab"},
    {"textures/bridge_split-cap-trim.psm", 274, "fc32f26de789bc881d169ce9e1462a84999bb7e6d947717d8d9a6e09a0a9d114"},
    {"textures/calciumroad.psm", 264, "8caeed1570c4abfabf6e6dbf148d7d54db2187f2130f7f229612b0eae368fb08"},
    {"textures/gen_shell.psm", 262, "c55e3086b6146c4343764b867f7e46952862e9642f7b5c4c2741789f4163dd27"},
    {"textures/hoh_road.psm", 261, "0cc5706b95c51eaaa06309f90860bc85747fa8b18edbe3492271e945ce1d0858"},
    {"textures/hoh_shells-lightbeam.psm", 2595, "5176024c3106bee11aef9b483d75ccb2d25b9d88732c499d44122ffad192fccc"},
    {"textures/hoh_shells.psm", 263, "9b943b463dd260451de3b4c791290e3acbf43b5aa64beb193046ef472942e9fb"},
    {"textures/lightbright.psm", 264, "a82d8843d921c1d613238652b5676ba7d5982541e86d55d3fd073da2b1d4b400"},
    {"textures/mc_coral10.psm", 263, "544fc71044298eba443eb0e19ed49becadc3c95d8908a359da588b8ea8717304"},
    {"textures/stands_seats.psm", 265, "86b8a91cee95513e48b661ebe9d3eb744e22e6a0715864764d6def58f0eb633b"},
    {"textures/tentacle.psm", 261, "96a6d1a713e4b30d7bd24daeb74ec894b0fed74ee3372dda8a2d10f37fc85ebd"},
    {"textures/ter_fount-side.psm", 267, "9584113dd300d0311f5f2d69370450a0afb5fb96835b40dba4b6ad57cb89160e"},
    {"textures/ter_fount-top.psm", 266, "97f9f6a90b6050590693458b84bda24181868d559a36c640fc41192e2a70882a"},
    {"textures/terrace_ledge-tiles.psm", 272, "e720a6010485675039932be25dbc52539ea5967f414332fae946c50f840b5267"},
    {"textures/terrace_slope-tiles.psm", 272, "b88abc3f6e07c2bd21be25c213d5bec593ccf38252809bbed61a129da0121a04"},
    {"textures/wall_scales-arch.psm", 269, "72e0c18f30e4c1783a1aeb63048b9635ebb70a066d9ba76c17f0ea1040bbdf9d"},
    {"textures/wall_shell-plates.psm", 270, "238caac95f2c9dc77ee9ebcff8b881bbf6780fbaced554dc970bfef8bcc325f2"},
};
static_assert(sizeof(wideScenes) / sizeof(wideScenes[0]) == 21, "Exact MA static wide-position source set");

inline void require(bool good, const std::string& reason) {
    if (!good) throw std::runtime_error("MA compressed admission: " + reason);
}
inline std::string normalized(std::string name) {
    for (char& c : name) {
        if (c == '\\') c = '/';
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + ('a' - 'A'));
    }
    return name;
}
// Standard SHA-256 steps reused from original_ui_archive50kg.cpp. This cold
// admission has no cancellation callback and never modifies input bytes.
inline std::uint32_t rotateRight(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
// Standard SHA-256, fixed stack workspace; no member-sized temporary copies.
// Constants are the SHA-256 constants also retained in audio_assets50hc.cpp.
inline void shaBlock(const std::uint8_t* block, std::array<std::uint32_t, 8>& h) noexcept {
    constexpr std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::uint32_t w[64]{};
    for (unsigned i = 0; i < 16; ++i)
        for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | block[i * 4 + j];
    for (unsigned i = 16; i < 64; ++i) {
        const auto s0 = rotateRight(w[i-15],7) ^ rotateRight(w[i-15],18) ^ (w[i-15] >> 3);
        const auto s1 = rotateRight(w[i-2],17) ^ rotateRight(w[i-2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    auto a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], f=h[5], g=h[6], v=h[7];
    for (unsigned i = 0; i < 64; ++i) {
        const auto s1 = rotateRight(e,6) ^ rotateRight(e,11) ^ rotateRight(e,25);
        const auto ch = (e & f) ^ (~e & g);
        const auto t1 = v + s1 + ch + k[i] + w[i];
        const auto s0 = rotateRight(a,2) ^ rotateRight(a,13) ^ rotateRight(a,22);
        const auto maj = (a & b) ^ (a & c) ^ (b & c);
        const auto t2 = s0 + maj;
        v=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=v;
}
inline bool hashMatches(const std::uint8_t* data, std::size_t size, const char* expected) noexcept {
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; size - at >= 64; at += 64) {
        shaBlock(data + at, h);
    }
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = size - at;
    if (remainder) std::memcpy(tail.data(), data + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(size) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u])
            return false;
    return true;
}
inline void requireIdentity(const Bytes& bytes, const Identity& identity) {
    require(bytes.size() == identity.bytes &&
        hashMatches(bytes.data(), bytes.size(), identity.sha256),
        std::string("source identity differs: ") + identity.name);
}
template<std::size_t N>
inline const Identity* findIdentity(const Identity (&identities)[N], const std::string& name) {
    for (const auto& identity : identities) if (name == identity.name) return &identity;
    return nullptr;
}
inline Bytes sourceMember(const rr::assets::ResArchive48C& archive, const Identity& identity) {
    const rr::assets::ResourceEntry48C* found = nullptr;
    for (const auto& entry : archive.entries) if (normalized(entry.name) == identity.name) {
        require(!found, std::string("ambiguous source member: ") + identity.name);
        found = &entry;
    }
    require(found != nullptr, std::string("missing source member: ") + identity.name);
    Bytes bytes;
    rr::assets::DecodeError48C error;
    require(rr::assets::extractResMember48C(archive, found->name, bytes, error),
        std::string("source extraction failed: ") + identity.name + " " + error.code);
    requireIdentity(bytes, identity);
    return bytes;
}
inline std::uint32_t word(const Bytes& bytes, std::size_t at) {
    require(at <= bytes.size() && bytes.size() - at >= 4, "truncated source material table");
    return std::uint32_t(bytes[at]) | (std::uint32_t(bytes[at + 1]) << 8) |
        (std::uint32_t(bytes[at + 2]) << 16) | (std::uint32_t(bytes[at + 3]) << 24);
}
inline void requireCompressedMaterials(const rr::assets::ResArchive48C& archive, const Bytes& scene) {
    // Exact pinned inputs are all PSG3 with one 224-byte node. This reads only
    // the actual source material name table; geometry stays with decodeScene48D.
    require(word(scene, 0) == 0x00677370 && word(scene, 4) == 3 && word(scene, 8) == 1,
        "wide-position source must be the pinned single-node PSG3 form");
    constexpr std::size_t countAt = 12 + 224;
    const auto count = word(scene, countAt);
    require(count >= 1 && count <= 6 && scene.size() >= countAt + 4 + count * 64u,
        "source material table extent differs");
    for (std::uint32_t i = 0; i < count; ++i) {
        const auto begin = countAt + 4 + std::size_t(i) * 64;
        std::size_t length = 0;
        while (length < 64 && scene[begin + length]) {
            require(scene[begin + length] < 128, "source material name is not ASCII");
            ++length;
        }
        require(length > 0 && length < 64, "invalid source material name");
        const std::string name(scene.begin() + begin, scene.begin() + begin + length);
        const auto path = "textures/" + normalized(name) + ".psm";
        const auto* identity = findIdentity(compressedMaterials, path);
        require(identity != nullptr, "unproven compressed source material: " + path);
        const auto bytes = sourceMember(archive, *identity);
        rr::assets::Material48E material;
        rr::assets::DecodeError48C error;
        require(rr::assets::decodeMaterial48E(bytes, material, error),
            "source material decode failed: " + path + " " + error.code);
        require(material.passes.size() == 1 &&
            normalized(material.passes.front().microprogram) == "prelitcompressed",
            "source material does not select the proven compressed program: " + path);
    }
}
} // namespace detail

class Admission {
    bool authenticated_ = false;
public:
    void authenticate(const detail::Bytes& originalArchive, const rr::assets::ResArchive48C& archive) {
        authenticated_ = false;
        constexpr detail::Identity original{"TRACKS/MA.RES", 3691592,
            "2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228"};
        detail::requireIdentity(originalArchive, original);
        detail::require(archive.entries.size() == 485 && archive.decoded.size() == 15532032,
            "original MA resource extent differs");
        // Authenticate all opt-in dependencies before creating any output files.
        for (const auto& identity : detail::wideScenes) {
            const auto bytes = detail::sourceMember(archive, identity);
            detail::requireCompressedMaterials(archive, bytes);
        }
        authenticated_ = true;
    }
    rr::assets::SceneLimits48D limitsFor(const rr::assets::ResourceEntry48C& member,
        const detail::Bytes& bytes) const {
        rr::assets::SceneLimits48D limits;
        if (!authenticated_) return limits;
        const auto* identity = detail::findIdentity(detail::wideScenes, detail::normalized(member.name));
        if (!identity) return limits;
        detail::requireIdentity(bytes, *identity);
        limits.allow_signed32_quantized_positions = true;
        return limits;
    }
};
} // namespace ma_compressed_scene
