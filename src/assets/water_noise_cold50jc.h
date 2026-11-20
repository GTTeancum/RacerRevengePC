#pragma once
#include "import_decode48c.h"
#include "../game/water_height50jc.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rr::assets {
// Identity metadata only, never an embedded texture or a generated substitute.
// Caller must authenticate the actual GC archive/member before this reader.
inline constexpr char gcWaterNoiseMember50JC[] = "tracks\\baseDisplacement.tga";
inline constexpr char gcWaterNoiseSha25650JC[] =
    "07e69a2bfc92e646788d2ddb0e6112be2b6f52ccb8ef7b84fd64fc889fc84be9";
inline constexpr std::size_t gcWaterNoiseFileBytes50JC = 1068;

struct WaterNoiseTexture50JC {
    std::array<std::uint8_t,16u*16u*4u> rgba{};
    std::uint32_t dimension=0,rowStride=0,pixelStride=0;
    std::size_t sourcePixels=0,sourceFileBytes=0;
    bool bound=false;
};

// Bounded actual-GC subset: type2, ID0, no color map, origin(0,0),16x16,
//32-bit BGRA, descriptor8. Header/extent validation is NOT authentication;
// the input must be the caller's authenticated original member, identified
// above. No disk access, resource extraction, fallback noise, or importer run.
//29EE40 reads pixels,29EF14 swaps B/R,29F060..29F170 reverses bottom-origin
// rows.24E0E0 does nothing for8888;16C7E0->16A530 copies64 bytes per row.
// Flags0 produce one mip. Key00FF00FF is unused by the8888->8888 copy path.
// Alpha is copied verbatim, never keyed, normalized, or halved. Trailer bytes
// are not pixels; the original read stops after the1024-byte image plane.
// Failure preserves output. Input is borrowed only for this call.
inline bool readWaterNoiseTexture50JC(const std::vector<std::uint8_t>& input,
    WaterNoiseTexture50JC& output,DecodeError48C& diagnostic) {
    diagnostic={};
    auto fail=[&](DecodeKind48C kind,const char* code,std::size_t offset,
                  const char* detail) {
        diagnostic={kind,code,offset,detail};return false;
    };
    // The member's size is not a gate (stock GC is gcWaterNoiseFileBytes50JC, a receipt): the
    // header and the pixel-plane extent below admit it. PORT STAND-IN (flagged): the reader's
    // fixed 16x16 BGRA32 plane is its capacity; another authored shape is refused by shape.
    if(input.size()<18u||input.size()>64u*1024u)
        return fail(DecodeKind48C::unsupported,"water.noise_extent",input.size(),
            "baseDisplacement member outside the TGA header/extent bound");
    if(input[0]!=0||input[1]!=0||input[2]!=2)
        return fail(DecodeKind48C::unsupported,"water.noise_tga_kind",0,
            "Only original ID-free uncompressed true-color TGA is admitted");
    for(std::size_t i=3;i<12;++i)if(input[i]!=0)
        return fail(DecodeKind48C::unsupported,"water.noise_tga_header",i,
            "Color-map and image-origin metadata differ from the admitted GC member");
    const auto width=std::uint32_t(input[12])|(std::uint32_t(input[13])<<8u);
    const auto height=std::uint32_t(input[14])|(std::uint32_t(input[15])<<8u);
    if(width!=16||height!=16||input[16]!=32||input[17]!=8)
        return fail(DecodeKind48C::unsupported,"water.noise_tga_shape",12,
            "Only16x16 BGRA32 descriptor8 is source-bound in this reader");
    constexpr std::size_t firstPixel=18,rowBytes=16u*4u,planeBytes=16u*rowBytes;
    if(firstPixel>input.size()||planeBytes>input.size()-firstPixel)
        return fail(DecodeKind48C::malformed,"water.noise_tga_pixels",firstPixel,
            "Original uncompressed pixel plane exceeds its supplied member");
    WaterNoiseTexture50JC staged{};
    for(std::size_t y=0;y<16;++y)for(std::size_t x=0;x<16;++x){
        const auto source=firstPixel+(15u-y)*rowBytes+x*4u;
        const auto destination=y*rowBytes+x*4u;
        staged.rgba[destination]=input[source+2u];
        staged.rgba[destination+1u]=input[source+1u];
        staged.rgba[destination+2u]=input[source];
        staged.rgba[destination+3u]=input[source+3u];
    }
    staged.dimension=width;staged.rowStride=rowBytes;staged.pixelStride=4;
    staged.sourcePixels=firstPixel;staged.sourceFileBytes=input.size();
    staged.bound=true;output=staged;return true;
}

// Only the non-null noise-owner branch of2B1040. Call after the actual enabled
// companion load2B2510..2B256C succeeded; this helper never invents absence or
// changes EnableBaseDisplacement. worldWidthHeight443EE0 is the actual WTT
// owner, not a GC constant. Texture storage must remain alive/unmoved while
// provider.noise5C is borrowed. It is shared world state, not per-racer state.
// Updates only the four represented noise fields after all checks; it does
// NOT bind signaturesC4, buckets, globals, or provider.sourceStateBound.
inline rr::game::WaterHeightResult50JC prepareWaterNoiseCold50JC(
    const WaterNoiseTexture50JC& texture,float worldWidthHeight443EE0,
    rr::game::WaterHeightProvider50JC& provider) noexcept {
    using namespace rr::game;using namespace water_height_detail50jc;
    if(!texture.bound)return fail(Status::Owner,0x2b105cu);
    if(texture.dimension!=16||texture.rowStride!=64||texture.pixelStride!=4||
       texture.sourcePixels!=18||texture.sourceFileBytes!=gcWaterNoiseFileBytes50JC)
        return fail(Status::Unsupported,0x2b1088u);
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::Arithmetic,0x2b10c0u);
    Math m;
    const auto worldWidth=m.read(worldWidthHeight443EE0,0x2b10b8u);
    if(m.badPC)return arithmetic(m);
    if(worldWidth<=0.f)return fail(Status::Unsupported,0x2b10c0u);
    const auto dimension=texture.rowStride/texture.pixelStride; //2B10A0
    const auto dimensionFloat=m.integer(static_cast<std::int32_t>(dimension),0x2b10bcu);
    volatile float divided=dimensionFloat/worldWidth;
    const auto coordinateScale=m.read(divided,0x2b10c0u);
    if(m.badPC)return arithmetic(m);
    provider.noise5C={texture.rgba.data(),texture.rgba.size(),true};
    provider.noiseDimensionAC=static_cast<std::int32_t>(dimension);
    provider.noisePixelStrideCC=static_cast<std::int32_t>(texture.pixelStride);
    provider.noiseCoordinateScaleBC=coordinateScale;
    provider.noisePresence=WaterNoisePresence50JC::Present;
    return fail(Status::Complete,0x2b10c4u);
}
} // namespace rr::assets
