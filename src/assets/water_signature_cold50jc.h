#pragma once
#include "water_authoring50jc.h"
#include "../game/water_height50jc.h"

namespace rr::assets {
struct WaterSignatureCold50JC {
    rr::game::WaterSignature50JC query{};
    float authoredPosition40[3]{};
    float scrollB0=0.f,scrollB4=0.f,speedB8=0.f,speedBC=0.f;
    std::uint32_t frameCount24=0,frame28=0;
    bool bound=false;
};
// Consumed2BE110 ->2BE250 ->2BE050 cold fields, after the real WTT reader
// and each authentic flags0/raw-PST texture loader succeeded. This does not
// construct rendering command buffers, register children or publish origins.
// Frames are already in original filename/index order and must outlive out.
// The 4096-frame ceiling is native admission only, not an original limit.
inline rr::game::WaterHeightResult50JC prepareWaterSignatureCold50JC(
    const WaterAuthoring50JC& authored,std::size_t index,
    const rr::game::WaterTextureFrame50JC* frames,std::size_t frameCount,
    std::uint32_t dimension,std::uint32_t pixelStride,WaterSignatureCold50JC& out) noexcept {
    using namespace rr::game;using namespace water_height_detail50jc;
    if(!authored.bound||index>=static_cast<std::size_t>(authored.general.signatureCount)||
       index>=authored.signatures.size())return fail(Status::Owner,0x2b9b38u,index);
    const auto& a=authored.signatures[index];
    if(!a.sectionPresent||a.frameCount<=0||!frames||
       frameCount!=static_cast<std::size_t>(a.frameCount)||frameCount>4096u||!dimension||
       dimension>0x01000000u||pixelStride!=1u||a.channel<0||a.channel>3)
        return fail(Status::Unsupported,0x2be250u,index);
    for(std::size_t i=0;i<frameCount;++i){const auto& f=frames[i];
        if(!f.pixels.bound||!f.pixels.data||f.rowStride!=dimension||
           std::uint64_t(dimension)*dimension>f.pixels.size)
            return fail(Status::Storage,0x2be358u,i);
    }
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::Arithmetic,0x2be110u,index);
    Math m;const auto width=m.read(a.width[0],0x2be130u),depth=m.read(a.width[1],0x2be134u);
    if(m.badPC)return arithmetic(m);
    if(width<=0.f||depth<=0.f)return fail(Status::Unsupported,0x2be11cu,index);
    std::uint32_t channelCount=0;
    for(std::int32_t i=0;i<authored.general.signatureCount;++i)
        if(authored.signatures[i].channel==a.channel)++channelCount;
    if(!channelCount)return fail(Status::Owner,0x2b9c10u,index);
    WaterSignatureCold50JC staged{};auto& q=staged.query;
    q.dimension04=dimension;q.rowStride08=0;q.pixelStride0C=pixelStride;
    // +08 retains constructor zero. The cold raw-view stride derives +04;
    // only the first real 2BEFA0 update publishes +08 at 2BF040.
    q.extentX10=width;q.extentZ14=depth;
    volatile float inverseX=1.f/width,inverseZ=1.f/depth;
    q.inverseX18=m.read(inverseX,0x2be11cu);q.inverseZ1C=m.read(inverseZ,0x2be13cu);
    volatile float count=static_cast<float>(channelCount);
    volatile float scale=1.f/count;q.scale20=m.read(scale,0x2b9c14u);
    //2BE418/41C exposes frame0 for both pointers before the first time update.
    q.current2C=frames[0].pixels;q.next30=frames[0].pixels;
    q.originXC0=0.f;q.originZC4=0.f; // Actual2BE14C/150 cold writes.
    q.infiniteD4=static_cast<std::uint32_t>(a.tile);
    q.channelD8=static_cast<std::uint32_t>(a.channel);
    q.effect110=static_cast<std::uint32_t>(a.physicsType);
    q.originsPublished2BEBD0=false;
    for(unsigned axis=0;axis<3;++axis)staged.authoredPosition40[axis]=
        m.read(a.position[axis],0x2be118u);
    const auto dimensionFloat=m.integer(static_cast<std::int32_t>(dimension),0x2be3a4u);
    const auto speedX=m.mul(a.velocity[0],width,0x2be160u);
    const auto speedZ=m.mul(a.velocity[1],depth,0x2be1a8u);
    // Initial constructor dimension is1; its first division is exact. The
    // real loaded texture then scales each speed once by its actual dimension.
    volatile float loadedX=speedX/dimensionFloat,loadedZ=speedZ/dimensionFloat;
    staged.speedB8=m.read(loadedX,0x2be3ccu);staged.speedBC=m.read(loadedZ,0x2be410u);
    staged.frameCount24=static_cast<std::uint32_t>(frameCount);staged.frame28=0;
    if(m.badPC)return arithmetic(m);
    q.bound=true;staged.bound=true;out=staged;return fail(Status::Complete,0x2be050u);
}
} // namespace rr::assets
