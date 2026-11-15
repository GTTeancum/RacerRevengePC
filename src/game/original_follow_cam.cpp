#include "original_follow_camera.h"
#include <array>
#include <cstring>
#include <iostream>
namespace rr::game {
namespace {
struct CamIdentity {std::int32_t pod;std::size_t bytes;const char* sha256;};
#include "original_follow_cam_catalog.inc"
// Reused unchanged SHA256 steps from original_ui_archive50kg.cpp, with its
// cancellation-only wrapper removed. This authenticates data, not guest code.
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
OriginalFollowResult loadOriginalFollowCam(std::int32_t pod,std::int32_t layout,
    const std::uint8_t* bytes,std::size_t byteCount,
    OriginalFollowPreset* storage,std::size_t capacity,OriginalFollowPresetBank& out) noexcept {
    using Status=OriginalFollowStatus;
    // One rule for every pod's loose PODnn.CAM: the original reads whatever the
    // disc holds (Config 1577D0). The stock catalog is a receipt; the bytes are
    // admitted by the native capacity/syntax checks below, the same for all.
    if(!bytes||!byteCount)return {Status::authoring,0x2867d4,0};
    {const CamIdentity* id=nullptr;for(const auto& c:catalog)if(c.pod==pod){id=&c;break;}
     const bool stock=id&&byteCount==id->bytes&&hashMatches(bytes,byteCount,id->sha256);
     if(!stock){try{std::cerr<<"pod_cam_identity pod="<<pod<<" bytes="<<byteCount<<" stock=0\n";}catch(...){}}}
    // The stock originals contain only comments, bracket sections and plain
    // key=value ASCII tokens. No guessed binaryTEXTCI or broader INI grammar.
    std::array<char,8192> text{};
    if(byteCount>=text.size())return {Status::capacity,0x1577d0,0};
    std::memcpy(text.data(),bytes,byteCount);
    std::array<OriginalFollowAuthoredSection,32> sections{};
    std::array<std::array<OriginalFollowAuthoredValue,32>,32> values{};
    std::size_t count=0,at=0;
    while(at<byteCount){
        const std::size_t start=at;while(at<byteCount&&text[at]!='\n')++at;
        const std::size_t end=at;if(at<byteCount)++at;
        std::size_t cleaned=start;
        for(std::size_t i=start;i<end;++i)if(text[i]!=' '&&text[i]!='\t'&&text[i]!='\r')text[cleaned++]=text[i];
        text[cleaned]='\0';char* line=text.data()+start;
        if(!*line||*line==';')continue;
        if(*line=='['){
            const std::size_t length=std::strlen(line);
            if(length<3||line[length-1]!=']'||count==sections.size())return {Status::authoring,0x157d00,count};
            line[length-1]='\0';sections[count]={line+1,values[count].data(),0};++count;continue;
        }
        if(!count)return {Status::authoring,0x158240,0};
        char* equal=std::strchr(line,'=');if(!equal||equal==line||!equal[1])return {Status::authoring,0x158240,count-1};
        *equal='\0';auto& sec=sections[count-1];if(sec.count==32)return {Status::capacity,0x158240,count-1};
        values[count-1][sec.count++]={line,equal+1,std::strlen(equal+1)};
    }
    OriginalFollowCamSource source;
    source.memberIdentity=0x43414d0000000000ull|std::uint32_t(pod);
    source.pod60=pod;source.layout118=layout;source.sections=sections.data();source.sectionCount=count;
    return readOriginalFollowPresets(source,storage,capacity,out);
}
}
