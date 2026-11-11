#pragma once
// Bounded immutable TA/RA authoring. This source-only change is untested.
// Exact original authoring bytes only. This is not child282860, a Camera,
// a live resource mount, an Entity, a readiness receipt or VU-normalized arrays.
// No I/O, runtime registration, native asset packing or fabricated fixtures.
#include "original_track_cut_identity50la.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
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
inline bool decimalValue(const std::vector<std::uint8_t>& bytes,std::size_t at,
    std::uint32_t& output) noexcept {
    if(at>=bytes.size()||bytes[at]<'0'||bytes[at]>'9')return false;
    std::uint32_t value=0;
    for(unsigned digits=0;digits<10&&at<bytes.size();++digits,++at){
        const auto c=bytes[at];
        if(c<'0'||c>'9')return false;
        const auto digit=static_cast<std::uint32_t>(c-'0');
        if(value>(std::numeric_limits<std::uint32_t>::max()-digit)/10u)return false;
        value=value*10u+digit;
        if(at+1<bytes.size()&&bytes[at+1]==0){output=value;return true;}
    }
    return false;
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

    // Compatibility entry: the historical four-member overload remains TA.
    static std::unique_ptr<const OriginalTaCutAuthoringCandidate> create(
        const std::array<Input,4>& input, Status& status) noexcept {
        return create("TA",input.data(),input.size(),status);
    }
    // Atomically return immutable owned original authoring only. The order is
    // the selected track's TOB followed by its exact numbered VUB members.
    // This checks owned bytes independently; it does not certify a live mount.
    static std::unique_ptr<const OriginalTaCutAuthoringCandidate> create(
        std::string_view trackId,const Input* input,std::size_t memberCount,Status& status) noexcept {
        using namespace ta_cut_authoring_candidate_detail;
        status = Status::input;
        if(!input||(trackId!="TA"&&trackId!="RA"&&trackId!="GC"&&trackId!="SA"&&trackId!="MA"&&trackId!="GA"&&trackId!="MB"&&trackId!="SC"&&trackId!="GB"&&trackId!="RC"&&trackId!="TC"))return nullptr;
        const auto* identity=originalTrackCutIdentity50LA(trackId);
        if(!identity||identity->cutCount!=((trackId=="TA"||trackId=="SC"||trackId=="TC")?3u:trackId=="RA"?7u:(trackId=="SA"||trackId=="GA"||trackId=="MB"||trackId=="RC")?5u:4u)||
           memberCount!=static_cast<std::size_t>(identity->cutCount)+1u)return nullptr;
        for (std::size_t i=0; i<memberCount; ++i) {
            const auto& member=identity->members[i];
            if (!input[i].bytes || input[i].size != member.size ||
                input[i].originalMember != member.name) return nullptr;
        }
        try {
            auto next = std::unique_ptr<OriginalTaCutAuthoringCandidate>(
                new OriginalTaCutAuthoringCandidate(*identity));
            for (std::size_t i=0; i<memberCount; ++i) {
                next->bytes_[i].assign(input[i].bytes,input[i].bytes+input[i].size);
                const auto& owned = next->bytes_[i];
                if (!hashMatches(owned.data(),owned.size(),identity->members[i].sha256)) {
                    status = Status::identity; return nullptr;
                }
            }
            // Exact-hash-pinned TOB offsets are serialized source values,
            // not copied Scene state or an invented fallback initial zone.
            const auto& tob = next->bytes_[0];std::uint32_t cuts=0;
            if (!decimalValue(tob,identity->cutCountOffset,cuts)||cuts!=identity->cutCount) {
                status = Status::layout; return nullptr;
            }
            for (std::size_t cut=0; cut<identity->cutCount; ++cut) {
                const auto& owned = next->bytes_[cut+1]; const auto* p = owned.data();
                const auto frames = identity->frames[cut];std::uint32_t zone=0;
                if (!frames||owned.size()<12u||word(p)!=1 || word(p+4)!=frames || word(p+8)!=1 ||
                    owned.size()!=12u+std::size_t(frames)*116u ||
                    !decimalValue(tob,identity->zoneOffsets[cut],zone)||zone!=identity->zones[cut]) {
                    status = Status::layout; return nullptr;
                }
                next->initialZones_[cut]=zone;
                for (std::uint32_t frame=0; frame<frames; ++frame) {
                    const auto* row=p+12u+std::size_t(frame)*116u;
                    if (word(row)!=frame || word(row+4)!=1 || word(row+8)!=0 ||
                        std::memcmp(row+12,"camera",7)!=0) {
                        status = Status::layout; return nullptr;
                    }
                    // Raw identities authenticate all64 name bytes, including
                    // nonzero tails. Never discard or reinterpret those bytes.
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
    std::string_view trackId() const noexcept {return identity_->trackId;}
    unsigned cutCount() const noexcept {return identity_->cutCount;}
    // Cut ordinals are original1..cutCount(); source frame indices are0-based.
    bool frameBits(unsigned cutOrdinal, std::uint32_t frame,
        OriginalTaCutFrameBitsCandidate& output) const noexcept {
        using ta_cut_authoring_candidate_detail::word;
        if (cutOrdinal<1 || cutOrdinal>cutCount() || frame>=identity_->frames[cutOrdinal-1])
            return false;
        const auto* row=bytes_[cutOrdinal].data()+12u+std::size_t(frame)*116u;
        OriginalTaCutFrameBitsCandidate next;
        next.frameWords={word(row),word(row+4)}; next.nodeWord=word(row+8);
        std::memcpy(next.nameBytes.data(),row+12,64);
        for (std::size_t i=0; i<10; ++i) next.payloadWords[i]=word(row+76+i*4);
        output=next; return true;
    }
    std::uint32_t frameCount(unsigned cutOrdinal) const noexcept {
        return cutOrdinal>=1 && cutOrdinal<=cutCount() ? identity_->frames[cutOrdinal-1] : 0;
    }
    bool initialZone(unsigned cutOrdinal, std::uint32_t& output) const noexcept {
        if (cutOrdinal<1 || cutOrdinal>cutCount()) return false;
        output=initialZones_[cutOrdinal-1];
        return true;
    }
    Bytes originalTobBytes() const noexcept {return {bytes_[0].data(),bytes_[0].size()};}
    Bytes originalVubBytes(unsigned cutOrdinal) const noexcept {
        if (cutOrdinal<1 || cutOrdinal>cutCount()) return {};
        const auto& b=bytes_[cutOrdinal]; return {b.data(),b.size()};
    }
private:
    explicit OriginalTaCutAuthoringCandidate(const OriginalTrackCutIdentity50LA& identity) noexcept
        :identity_(&identity){}
    const OriginalTrackCutIdentity50LA* const identity_;
    std::array<std::vector<std::uint8_t>,8> bytes_;
    std::array<std::uint32_t,7> initialZones_{};
};
}
