#pragma once
#include "../../../src/game/original_video_clear_packet25cad0_native50la.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <new>
#include <optional>
#include <utility>

namespace rr::platform {
//Bounded packet-to-draw decoder and CPU raster kernel for the ACTUAL immutable
//25CAD0 native packet owner. Only its exact clear vocabulary is supported, not
//a generalized sprite or DMA/GIF renderer. No arbitrary byte span, caller color,
//geometry or
//GPU/IRQ success receipt is accepted. This is an explicit native interpretation,
//not original GS framebuffer reconstruction, swizzled VRAM, or source execution.
class OriginalVideoPacketCpuRenderer50LA final {
public:
    enum class Status : std::uint8_t {
        malformedPacket,unsupportedState,unsupportedGeometry,decoded,
        policyRequired,unsupportedPolicy,allocation,pixelsProduced
    };
    enum class FbaPolicy : std::uint8_t {unspecified,zero};
    enum class ScanmaskPolicy : std::uint8_t {unspecified,zero};
    struct NativePolicies {
        //Neither register is written by25CAD0. Default is deliberately NOT0.
        FbaPolicy fba=FbaPolicy::unspecified;
        ScanmaskPolicy scanmask=ScanmaskPolicy::unspecified;
    };
    struct Rectangle {std::uint32_t left=0,top=0,right=0,bottom=0;};
    //Decoder-produced numeric draw, reusable by another explicitly owned native
    //backend. Its copies certify only this supported packet decoding, never a
    //source caller, framebuffer identity, live renderer target or mode readiness.
    class DecodedDraw final {
        friend class OriginalVideoPacketCpuRenderer50LA;
        std::uint32_t width_=0,height_=0;
        Rectangle rectangle_;
        std::array<std::uint8_t,4> rgba_{};
        DecodedDraw()noexcept=default;
    public:
        DecodedDraw(const DecodedDraw&)=default;
        DecodedDraw& operator=(const DecodedDraw&)=default;
        std::uint32_t width()const noexcept{return width_;}
        std::uint32_t height()const noexcept{return height_;}
        const Rectangle& rectangle()const noexcept{return rectangle_;}
        const std::array<std::uint8_t,4>& rgba()const noexcept{return rgba_;}
        static constexpr bool sourceFbaKnown()noexcept{return false;}
        static constexpr bool sourceScanmaskKnown()noexcept{return false;}
    };
    struct DecodeResult {
        Status status=Status::malformedPacket;
        std::optional<DecodedDraw> draw;
    };
    class Image final {
        friend class OriginalVideoPacketCpuRenderer50LA;
        DecodedDraw draw_;
        NativePolicies policies_;
        std::unique_ptr<std::uint8_t[]> rgba_;
        std::size_t rowBytes_=0,sizeBytes_=0;
        Image(const DecodedDraw& draw,NativePolicies policies)noexcept:draw_(draw),policies_(policies){}
    public:
        ~Image()noexcept=default;
        Image(const Image&)=delete;
        Image& operator=(const Image&)=delete;
        std::uint32_t width()const noexcept{return draw_.width();}
        std::uint32_t height()const noexcept{return draw_.height();}
        std::size_t rowBytes()const noexcept{return rowBytes_;}
        std::size_t sizeBytes()const noexcept{return sizeBytes_;}
        const std::uint8_t* data()const noexcept{return rgba_.get();}
        const DecodedDraw& draw()const noexcept{return draw_;}
        NativePolicies nativePolicies()const noexcept{return policies_;}
        //Independent, immutable, top-to-bottom row-major straight RGBA8.
        //No native window/GL target, GS address, depth image or submission lease.
    };
    struct RasterResult {
        Status status=Status::malformedPacket;
        std::unique_ptr<const Image> image;
    };
private:
    static std::uint32_t word(const std::uint8_t* bytes,std::size_t qword,std::size_t lane)noexcept {
        std::uint32_t value=0;const auto p=qword*16+lane*4;
        for(unsigned i=0;i<4;++i)value|=std::uint32_t(bytes[p+i])<<(8*i);
        return value;
    }
public:
    static DecodeResult decode(const rr::game::OriginalVideoClearPacket25CAD0Native50LA& packet)noexcept {
        if(packet.sizeBytes()!=208)return {};
        const auto* bytes=packet.data();
        //Strict supported packet grammar: every byte is consumed and pinned
        //except qword9's RGBA data word. Constants authenticate only this
        //bounded CPU draw vocabulary, NOT ownership of inherited GS state.
        static constexpr std::uint32_t expected[13][4]={
            {0x1000000b,0,0,0}, //DMA CNT, QWC11.
            {0x0000800a,0x10000000,0x0000000e,0}, //GIF PACKED A+D, EOP,NLOOP10,NREG1.
            {0x00000001,0x00000000,0x1a,0}, //PRMODECONT:PRIM supplies attributes.
            {0x30000000,0x00000001,0x4e,0}, //ZBUF:depth writes masked.
            {0x00100000,0x00000000,0x4c,0}, //FRAME:FBW16,PSMCT32,write mask0.
            {0x00006000,0x00006000,0x18,0}, //XYOFFSET1.
            {0x03ff0000,0x03ff0000,0x40,0}, //SCISSOR1, inclusive endpoints.
            {0x00030013,0x00000000,0x47,0}, //TEST1:alpha/depth always,DATEoff.
            {0x00000006,0x00000000,0x00,0}, //PRIM:sprite,untextured,unblended,context0.
            {0x00000000,0x3f800000,0x01,0}, //RGBAQ:only lane0 varies;Q=1.0.
            {0x60006000,0x00000000,0x04,0}, //XYZF2, not XYZ2.
            {0xa000a000,0x00000000,0x04,0},
            {0x70000000,0,0,0} //DMA END,QWC0.
        };
        for(std::size_t q=0;q<13;++q)for(std::size_t lane=0;lane<4;++lane){
            if(q==9&&lane==0)continue;
            if(word(bytes,q,lane)!=expected[q][lane])return {Status::unsupportedState,{}};
        }
        const auto frame=word(bytes,4,0),offsetX=word(bytes,5,0)&0xffffu,offsetY=word(bytes,5,1)&0xffffu;
        const auto first=word(bytes,10,0),last=word(bytes,11,0);
        const std::int32_t x0=std::int32_t(first&0xffffu)-std::int32_t(offsetX);
        const std::int32_t y0=std::int32_t(first>>16)-std::int32_t(offsetY);
        const std::int32_t x1=std::int32_t(last&0xffffu)-std::int32_t(offsetX);
        const std::int32_t y1=std::int32_t(last>>16)-std::int32_t(offsetY);
        //Exact16.4, integer-aligned subset. Native pixel-center coverage is
        //half-open; fractional edges would need a separately stated contract.
        if(x0<0||y0<0||x1<=x0||y1<=y0||x0%16||y0%16||x1%16||y1%16)
            return {Status::unsupportedGeometry,{}};
        const auto scissorX=word(bytes,6,0),scissorY=word(bytes,6,1);
        const auto left=scissorX&0x7ffu,right=((scissorX>>16)&0x7ffu)+1;
        const auto top=scissorY&0x7ffu,bottom=((scissorY>>16)&0x7ffu)+1;
        DecodedDraw draw;
        draw.width_=((frame>>16)&0x3fu)*64;draw.height_=bottom;
        if(!draw.width_||!draw.height_||right<left||bottom<top)return {Status::unsupportedGeometry,{}};
        draw.rectangle_={std::max(left,std::uint32_t(x0/16)),std::max(top,std::uint32_t(y0/16)),
            std::min({right,std::uint32_t(x1/16),draw.width_}),
            std::min({bottom,std::uint32_t(y1/16),draw.height_})};
        if(draw.rectangle_.left>=draw.rectangle_.right||draw.rectangle_.top>=draw.rectangle_.bottom)
            return {Status::unsupportedGeometry,{}};
        const auto rgba=word(bytes,9,0);
        for(unsigned i=0;i<4;++i)draw.rgba_[i]=std::uint8_t(rgba>>(8*i));
        return {Status::decoded,draw};
    }
    static RasterResult rasterize(const DecodedDraw& draw,NativePolicies policies)noexcept {
        if(policies.fba==FbaPolicy::unspecified||policies.scanmask==ScanmaskPolicy::unspecified)
            return {Status::policyRequired,{}};
        if(policies.fba!=FbaPolicy::zero||policies.scanmask!=ScanmaskPolicy::zero)
            return {Status::unsupportedPolicy,{}};
        const auto& rectangle=draw.rectangle_;
        //No destination surface is borrowed or fabricated. The admitted packet
        //covers this ENTIRE independent image, so no unknown old pixels or
        //depth are needed. Partial coverage requires a real destination owner.
        if(!draw.width_||!draw.height_||rectangle.left||rectangle.top||
           rectangle.right!=draw.width_||rectangle.bottom!=draw.height_)
            return {Status::unsupportedGeometry,{}};
        if(std::size_t(draw.width_)>std::numeric_limits<std::size_t>::max()/4)
            return {Status::unsupportedGeometry,{}};
        const std::size_t rowBytes=std::size_t(draw.width_)*4;
        if(std::size_t(draw.height_)>std::numeric_limits<std::size_t>::max()/rowBytes)
            return {Status::unsupportedGeometry,{}};
        const std::size_t byteCount=rowBytes*draw.height_;
        std::unique_ptr<Image> image(new(std::nothrow) Image(draw,policies));
        if(!image)return {Status::allocation,{}};
        image->rgba_.reset(new(std::nothrow) std::uint8_t[byteCount]);
        if(!image->rgba_)return {Status::allocation,{}};
        //Native FBA0 preserves packet alpha exactly;SCANMSK0 writes every row.
        //PRIM/TEST/ZBUF pin no texture,fog,blend,destination test or depth write.
        //No normalized-alpha, premultiplication, sRGB conversion or VRAM swizzle.
        for(std::uint32_t y=rectangle.top;y<rectangle.bottom;++y){
            auto* row=image->rgba_.get()+std::size_t(y)*rowBytes;
            for(std::uint32_t x=rectangle.left;x<rectangle.right;++x)
                for(unsigned c=0;c<4;++c)row[std::size_t(x)*4+c]=draw.rgba_[c];
        }
        image->rowBytes_=rowBytes;image->sizeBytes_=byteCount;
        return {Status::pixelsProduced,std::move(image)};
    }
    static RasterResult renderPacket(const rr::game::OriginalVideoClearPacket25CAD0Native50LA& packet,
        NativePolicies policies)noexcept {
        auto decoded=decode(packet);
        if(decoded.status!=Status::decoded||!decoded.draw)return {decoded.status,{}};
        return rasterize(*decoded.draw,policies);
    }
};
}
