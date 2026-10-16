#include "original_camera_start_effects.h"
#include <cstring>
namespace rr::game { namespace {
std::uint32_t rotateRight(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
// Standard SHA-256, fixed stack workspace; no member-sized temporary copies.
// Constants are the SHA-256 constants also retained in audio_assets50hc.cpp.
void shaBlock(const std::uint8_t* block, std::array<std::uint32_t, 8>& h) noexcept {
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
bool hashMatches(const std::uint8_t* data, std::size_t size,
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
}
bool OriginalCourseStartActions::prepare(OriginalCameraStartCourse course,const std::uint8_t* bytes,std::size_t size) noexcept {
    if(authenticated_||!bytes)return false;
    const char* sha=nullptr;std::size_t exact=0;
    if(course==OriginalCameraStartCourse::gc){exact=9623;sha="e677f80d12da2c2225c8ba030e2a29e3a358c52bcfd4f9c981b79c3c53b72454";}
    else if(course==OriginalCameraStartCourse::ga){exact=9689;sha="e96450990e23c97f6d686707a183789b08d45857187d5c8463960cb323e75aa3";}
    else if(course==OriginalCameraStartCourse::ta){exact=13887;sha="11e4c4909bf866fee626977c970d9907d4f191c29eeff32bddd162f173916f6b";}
    else if(course==OriginalCameraStartCourse::ma){exact=32691;sha="4a3bcecce43a53b3ec3ac28df9c9672a61e825a2987c11bbd0ab055b29f7da55";}
    else if(course==OriginalCameraStartCourse::sa){exact=13314;sha="b2b53df201ddfc5bd0283cd50911c6e94937f3633bba33b864da13557f415542";}
    else if(course==OriginalCameraStartCourse::ra){exact=95558;sha="7336c6bab40265adb5e82aec8ed3ee66669c19b26a597cfacedddfca4be48524";}
    else if(course==OriginalCameraStartCourse::rc){exact=124930;sha="71b1f9609c0a6ce84d71ff69355d9cfcbc8771eb7ddb8876473b4fddd6e23e6d";}
    if(!sha||size!=exact||!hashMatches(bytes,size,sha))return false;
    // Independent static section-table inspection of these exact unchanged
    // originals proves no321_anims section. No generic section fallback here.
    // RA tracks\ra.tob:713 sections; RC tracks\rc.tob:907 sections.
    // CSCameras count bytes at TOB offsets15496/19491 are respectively7/5.
    // Exact [CSCameras].NumberOfCameras: GC4, GA0, TA3, MA4, SA5, RA7, RC5.
    // Source174DF0 derives controllerA8 from this genuine track3E4 value.
    sourceSceneCutCount3E4_=course==OriginalCameraStartCourse::ta?3:
        (course==OriginalCameraStartCourse::gc||course==OriginalCameraStartCourse::ma)?4:
        (course==OriginalCameraStartCourse::sa||course==OriginalCameraStartCourse::rc)?5:
        course==OriginalCameraStartCourse::ra?7:0;
    course_=course;authenticated_=true;return true;
}
}
