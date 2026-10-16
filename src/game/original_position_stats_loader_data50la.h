#pragma once
// Authored190190 loader operands only. No resource allocation, live field
// mutation, host layout or claim that full PositionStats loading has occurred.
#include <array>
#include <cstdint>
namespace rr::game::position_stats_loader50LA {
struct Texture {
    std::uint32_t offset,callerPC;
    const char* material;
    float width34,height38;
    //Exact raw stores relative to the buffer at ScreenTexture+58. No host
    //corner ordering is inferred. Descending endpoints are not sorted/remapped.
    std::uint32_t buffer60_C0,buffer90_F0,buffer64_94,bufferC4_F4;
};
inline constexpr std::array<Texture,9> fixedTextures{{
    {0x270,0x1901dc,"stats",32,19,0x3f800000,0x3f4a0000,0x3f4c0000,0x3f700000},
    {0x2d0,0x190318,"stats",5,19,0x3f000000,0x3f480000,0x3f4c0000,0x3f700000},
    {0x330,0x190454,"stats",32,27,0x00000000,0x3e780000,0x3f4c0000,0x3f800000},
    {0x390,0x19058c,"stats",32,27,0x3e800000,0x3efc0000,0x3f4c0000,0x3f800000},
    {0x3f0,0x1906c8,"stats",54,19,0x3f000000,0x3f480000,0x3f4c0000,0x3f700000},
    {0x450,0x190804,"stats",100,101,0x00000000,0x3f460000,0x3c000000,0x3f4a0000},
    {0x4b0,0x190940,"maptop",16,16,0x3f600000,0x3f7e0000,0x00000000,0x3df00000},
    {0x510,0x190a78,"stats",32,19,0x3f800000,0x3f4a0000,0x3f4c0000,0x3f700000},
    {0x570,0x190bb4,"stats",129,19,0x3f480000,0x3f000000,0x3f4c0000,0x3f700000}
}};
inline constexpr Texture mapTexture{0x5d0,0x190cfc,"maptop",8,8,
    0x3f740000,0x3f7e0000,0x3e000000,0x3e300000};
inline constexpr std::uint32_t mapTextureCount=118,mapTextureStride=0x60;
inline constexpr std::uint32_t textCount=5,textBase=0x40,textStride=0x70,textCapacity=40;
inline constexpr const char* font="eurostileboldbold13ingame";
inline constexpr std::uint32_t textCallerPC=0x190eb0,textRGB=0xffe5d96c;
//All five initial strings point at source3A8F20's NUL byte. Text1 field40=1.7;
//text4 justification50=4 (source3C4758). Others retain constructor defaults.
inline constexpr std::uint32_t text1Field40Bits=0x3fd9999a;
inline constexpr std::uint32_t text4Justification=4;
}
