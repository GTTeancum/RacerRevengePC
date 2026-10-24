#pragma once
// PRIVATE, UNACTIVATED SOURCE CANDIDATE. Not compiled or executed.
// Exact original TA authoring bytes only. This is not child282860, a Camera,
// a live resource mount, an Entity, a readiness receipt or VU-normalized arrays.
// No I/O, runtime registration, native asset packing, fixtures or integration.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>
#include <vector>

namespace rr::assets::ta_cut_authoring_candidate_detail {
// SHA-256 helper retained verbatim (apart from inline linkage) from frozen
// src/game/original_camera_start_authoring.cpp. It authenticates actual owned
// bytes here; the caller supplies no digest, admitted flag or readiness token.
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
inline bool hashMatches(const std::uint8_t* data, std::size_t size,
    const char* expected) noexcept {
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
inline std::uint32_t word(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
        (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
}

namespace rr::assets {
struct OriginalTaCutMemberInputCandidate {
    // Resolved original member identity, not a guessed runtime request path.
    // Input must be readable and stable during create(); no aliases retained.
    std::string_view originalMember;
    const std::uint8_t* bytes = nullptr;
    std::size_t size = 0;
};
struct OriginalTaCutFrameBitsCandidate {
    std::array<std::uint32_t,2> frameWords{};
    std::uint32_t nodeWord = 0;
    // Preserve all64 original bytes, including nonzero tail after "camera".
    std::array<std::uint8_t,64> nameBytes{};
    // File order, never fabricated/normalized/interpolated: vector0, vector1,
    // position, scalar. Binary32 words remain exact without host arithmetic.
    std::array<std::uint32_t,10> payloadWords{};
};
enum class OriginalTaCutAuthoringStatusCandidate : std::uint8_t {
    none, input, identity, layout, allocation
};
class OriginalTaCutAuthoringCandidate final {
public:
    using Input = OriginalTaCutMemberInputCandidate;
    using Status = OriginalTaCutAuthoringStatusCandidate;
    struct Bytes { const std::uint8_t* data = nullptr; std::size_t size = 0; };
    OriginalTaCutAuthoringCandidate(const OriginalTaCutAuthoringCandidate&) = delete;
    OriginalTaCutAuthoringCandidate& operator=(const OriginalTaCutAuthoringCandidate&) = delete;
    OriginalTaCutAuthoringCandidate(OriginalTaCutAuthoringCandidate&&) = delete;
    OriginalTaCutAuthoringCandidate& operator=(OriginalTaCutAuthoringCandidate&&) = delete;
    ~OriginalTaCutAuthoringCandidate() = default;

    // Atomically return immutable owned source authoring only. Expected order:
    // exact TA.tob, TA01.vub, TA02.vub, TA03.vub resolved from original TA.RES.
    // This independently checks member bytes, NOT a live archive provider.
    static std::unique_ptr<const OriginalTaCutAuthoringCandidate> create(
        const std::array<Input,4>& input, Status& status) noexcept {
        using namespace ta_cut_authoring_candidate_detail;
        status = Status::input;
        for (std::size_t i=0; i<4; ++i) {
            if (!input[i].bytes || input[i].size != sizes_[i] ||
                input[i].originalMember != members_[i]) return nullptr;
        }
        try {
            auto next = std::unique_ptr<OriginalTaCutAuthoringCandidate>(
                new OriginalTaCutAuthoringCandidate);
            for (std::size_t i=0; i<4; ++i) {
                next->bytes_[i].assign(input[i].bytes,input[i].bytes+input[i].size);
                const auto& owned = next->bytes_[i];
                if (!hashMatches(owned.data(),owned.size(),digests_[i])) {
                    status = Status::identity; return nullptr;
                }
            }
            // TOB is exact-hash pinned; offsets are original serialized value
            // locations, not copied scene state or an invented fallback zone.
            const auto& tob = next->bytes_[0];
            if (tob[3256] != '3' || tob[3257] != 0) {
                status = Status::layout; return nullptr;
            }
            for (std::size_t cut=0; cut<3; ++cut) {
                const auto& owned = next->bytes_[cut+1]; const auto* p = owned.data();
                const auto frames = frameCounts_[cut];
                if (word(p)!=1 || word(p+4)!=frames || word(p+8)!=1 ||
                    owned.size()!=12u+std::size_t(frames)*116u ||
                    tob[zoneOffsets_[cut]]!=zoneAscii_[cut] ||
                    tob[zoneOffsets_[cut]+1]!=0) {
                    status = Status::layout; return nullptr;
                }
                for (std::uint32_t frame=0; frame<frames; ++frame) {
                    const auto* row=p+12u+std::size_t(frame)*116u;
                    if (word(row)!=frame || word(row+4)!=1 || word(row+8)!=0 ||
                        std::memcmp(row+12,"camera",7)!=0) {
                        status = Status::layout; return nullptr;
                    }
                    // Raw exact byte identities already authenticate the
                    // nonzero name tails. Never discard or interpret them.
                    for (std::size_t lane=0; lane<10; ++lane) {
                        if ((word(row+76+lane*4)&0x7f800000u)==0x7f800000u) {
                            status = Status::layout; return nullptr;
                        }
                    }
                }
            }
            status = Status::none;
            return std::unique_ptr<const OriginalTaCutAuthoringCandidate>(next.release());
        } catch (...) { status = Status::allocation; return nullptr; }
    }
    // All public cut ordinals are original1..3; source frame indices are0-based.
    bool frameBits(unsigned cutOrdinal, std::uint32_t frame,
        OriginalTaCutFrameBitsCandidate& output) const noexcept {
        using ta_cut_authoring_candidate_detail::word;
        if (cutOrdinal<1 || cutOrdinal>3 || frame>=frameCounts_[cutOrdinal-1])
            return false;
        const auto* row=bytes_[cutOrdinal].data()+12u+std::size_t(frame)*116u;
        OriginalTaCutFrameBitsCandidate next;
        next.frameWords={word(row),word(row+4)}; next.nodeWord=word(row+8);
        std::memcpy(next.nameBytes.data(),row+12,64);
        for (std::size_t i=0; i<10; ++i) next.payloadWords[i]=word(row+76+i*4);
        output=next; return true;
    }
    std::uint32_t frameCount(unsigned cutOrdinal) const noexcept {
        return cutOrdinal>=1 && cutOrdinal<=3 ? frameCounts_[cutOrdinal-1] : 0;
    }
    bool initialZone(unsigned cutOrdinal, std::uint32_t& output) const noexcept {
        if (cutOrdinal<1 || cutOrdinal>3) return false;
        output=std::uint32_t(bytes_[0][zoneOffsets_[cutOrdinal-1]]-'0');
        return true;
    }
    Bytes originalTobBytes() const noexcept {return {bytes_[0].data(),bytes_[0].size()};}
    Bytes originalVubBytes(unsigned cutOrdinal) const noexcept {
        if (cutOrdinal<1 || cutOrdinal>3) return {};
        const auto& b=bytes_[cutOrdinal]; return {b.data(),b.size()};
    }
private:
    OriginalTaCutAuthoringCandidate() = default;
    std::array<std::vector<std::uint8_t>,4> bytes_;
    inline static constexpr std::array<const char*,4> members_{{
        "tracks\\ta.tob","tracks\\ta01.vub","tracks\\ta02.vub","tracks\\ta03.vub"}};
    inline static constexpr std::array<std::size_t,4> sizes_{{13887,53488,17528,17528}};
    inline static constexpr std::array<const char*,4> digests_{{
        "11e4c4909bf866fee626977c970d9907d4f191c29eeff32bddd162f173916f6b",
        "ad0a75b4e37e7e21f3fa29162c00dc4bc482b2f2d04066e5255edbd49a41147c",
        "ba15e426727250fdf7d25a0c7378d5323624a7504eb79cd9792eadd85f079e05",
        "fe7f6b12b3ddcb695f39b47d0a4e9cb89847e9639461d0b8ca03d4c05ed78456"}};
    inline static constexpr std::array<std::uint32_t,3> frameCounts_{{461,151,151}};
    inline static constexpr std::array<std::size_t,3> zoneOffsets_{{3109,3158,3207}};
    inline static constexpr std::array<std::uint8_t,3> zoneAscii_{{'9','5','6'}};
};
}
