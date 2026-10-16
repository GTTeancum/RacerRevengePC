#pragma once
// Source-only Damage18C550 numeric subset. UNTESTED; no owner, resource facet,
// Bank access, renderer admission or production caller. Original SLUS SHA256
// c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
#include <array>
#include <cstddef>
#include <cmath>
#include <cstdint>
#include <initializer_list>

namespace rr::game::damage_detail50LA {
inline float add(float a,float b)noexcept {volatile float r=a+b;return r;}
inline float sub(float a,float b)noexcept {volatile float r=a-b;return r;}
inline float mul(float a,float b)noexcept {volatile float r=a*b;return r;}
inline float div(float a,float b)noexcept {volatile float r=a/b;return r;}
struct AuthoredTexture {
    std::uint16_t offset;
    float width,height,u0Pixels,u1Pixels,v0Pixels,v1Pixels;
};
// All32 are separate ScreenTexture references to genuine dmgradar, never one
// shared mutable sprite. Array index0..11 = +50;12..21 = +4D0;22..31 = +890.
// Pixel coordinates multiply each actual texture's reciprocal dimensions.
inline constexpr std::array<AuthoredTexture,32> authored{{
    {0x050,62,29,1,63,51,80}, {0x0b0,62,29,63,1,51,80},
    {0x110,88,48,1,89,1,49}, {0x170,20,19,47,67,82,101},
    {0x1d0,18,17,47,65,104,121}, {0x230,20,19,67,47,82,101},
    {0x290,24,28,69,93,82,110}, {0x2f0,30,15,15,45,101,116},
    {0x350,24,28,93,69,82,110}, {0x3b0,30,37,94,124,82,119},
    {0x410,42,16,2,44,83,99}, {0x470,30,37,124,94,82,119},
    {0x4d0,6,15,117,123,56,71}, {0x530,5,15,112,117,56,71},
    {0x590,5,15,107,112,56,71}, {0x5f0,6,17,101,107,56,73},
    {0x650,5,17,96,101,56,73}, {0x6b0,5,17,91,96,56,73},
    {0x710,6,20,85,91,56,76}, {0x770,5,20,80,85,56,76},
    {0x7d0,5,20,75,80,56,76}, {0x830,5,20,70,75,56,76},
    {0x890,6,15,123,117,56,71}, {0x8f0,5,15,117,112,56,71},
    {0x950,5,15,112,107,56,71}, {0x9b0,6,17,107,101,56,73},
    {0xa10,5,17,101,96,56,73}, {0xa70,5,17,96,91,56,73},
    {0xad0,6,20,91,85,56,76}, {0xb30,5,20,85,80,56,76},
    {0xb90,5,20,80,75,56,76}, {0xbf0,5,20,75,70,56,76}
}};
// Actual2DF0B0 call order, including the three loader loops (18CA38,
// 18D030,18D628), is not constructor/array order.
inline constexpr std::array<std::uint8_t,32> loadOrder{{
    12,22,13,23,14,24,15,25,16,26,17,27,18,28,19,29,20,30,21,31,
    0,1,2,3,6,9,4,7,10,5,8,11
}};
// Same indexing as authored; repeated PCs identify actual loader loops.
inline constexpr std::array<std::uint32_t,32> loadCallerPC{{
    0x18d930,0x18daa4,0x18dc1c,0x18dd84,0x18e1dc,0x18e644,
    0x18def4,0x18e354,0x18e7bc,0x18e068,0x18e4cc,0x18e934,
    0x18c748,0x18ca38,0x18ca38,0x18cd40,0x18d030,0x18d030,
    0x18d338,0x18d628,0x18d628,0x18d628,
    0x18c8bc,0x18cbcc,0x18cbcc,0x18ceb4,0x18d1c4,0x18d1c4,
    0x18d4ac,0x18d7bc,0x18d7bc,0x18d7bc
}};
struct Texture {
    float x=0,y=0,width=0,height=0,u0=0,u1=1,v0=0,v1=1,rotation=0;
    std::uint8_t dirty=0;
    std::uint32_t color44=0x00ffffff;
    std::int16_t alpha48=255;
    std::array<std::uint8_t,4> packetColor{{127,127,127,63}};
    void geometryChanged()noexcept {
        dirty=static_cast<std::uint8_t>((dirty&~1u)|1u);
        if(rotation!=0)dirty=static_cast<std::uint8_t>((dirty&~2u)|2u);
    }
    void position(float px,float py)noexcept {
        if(x==px&&y==py)return;
        x=px;y=py;geometryChanged();
    }
    void dimensions(float w,float h)noexcept {
        if(width!=w){width=w;geometryChanged();}
        if(height!=h){height=h;geometryChanged();}
    }
    // Source inline setters update four primitive color lanes directly.
    // They do not mark geometry dirty or fabricate a draw receipt.
    void color(std::uint32_t rgb)noexcept {
        if(color44==rgb)return;
        color44=rgb;
        packetColor[0]=static_cast<std::uint8_t>(((rgb>>16)&255u)>>1);
        packetColor[1]=static_cast<std::uint8_t>(((rgb>>8)&255u)>>1);
        packetColor[2]=static_cast<std::uint8_t>((rgb&255u)>>1);
    }
    void alpha(std::uint8_t a)noexcept {
        if(alpha48==a)return;
        alpha48=a;packetColor[3]=static_cast<std::uint8_t>(a>>2);
    }
};
struct State {
    std::array<Texture,32> textures{};
    //18C5E0..18C620: per-view health initialized1; source update stores dt40.
    std::array<float,2> health30{{1,1}},health38{{1,1}};
    float elapsed40=0;
    std::int32_t raceModeC58=0;
    // Loader dimensions/UV subset18C748..18EAA4. These dimensions must come
    // from each actual resolved canonical atlas, not guessed128 constants.
    bool initializeTexture(std::size_t i,std::int32_t atlasWidth,
        std::int32_t atlasHeight)noexcept {
        if(i>=textures.size()||atlasWidth<=0||atlasHeight<=0)return false;
        auto& t=textures[i];const auto& a=authored[i];
        t.dimensions(a.width,a.height);
        const float rw=div(1.f,static_cast<float>(atlasWidth));
        const float rh=div(1.f,static_cast<float>(atlasHeight));
        t.u0=mul(a.u0Pixels,rw);t.u1=mul(a.u1Pixels,rw);
        t.v0=mul(a.v0Pixels,rh);t.v1=mul(a.v1Pixels,rh);
        return true;
    }
    //18EAD0..18F450. Source callers use one-based view; no viewport scaling
    // or arbitrary layout choice is inserted here.
    bool layout18EAD0(std::uint32_t oneBasedView)noexcept {
        if(oneBasedView<1||oneBasedView>2)return false;
        const float ox=raceModeC58==0||raceModeC58==1?0.f:
            oneBasedView==1?0.f:-30.f;
        const float oy=raceModeC58==1?(oneBasedView==1?-199.f:-224.f):0.f;
        float lx=42.f,rx=151.f;const float y=add(355.f,oy);
        for(int i=9;i>=0;--i){
            textures[12+i].position(add(lx,ox),y);
            textures[22+i].position(add(rx,ox),y);
            rx=sub(rx,(i==7||i==4||i==1)?6.f:5.f);
            lx=add(lx,(i==6||i==3)?6.f:5.f);
        }
        constexpr std::array<float,12> xs{{36,100,55,77,90,101,65,84,109,53,77,115}};
        constexpr std::array<float,12> ys{{350,350,368,371,376,371,373,390,373,375,401,375}};
        constexpr std::array<std::uint8_t,12> order{{0,1,2,3,6,9,4,7,10,5,8,11}};
        for(auto i:order)textures[i].position(add(xs[i],ox),add(ys[i],oy));
        return true;
    }
    //18F4D0..18F590, finite nonnegative damage domain. This projects health
    // into HUD-owned state; it never edits source damage. The caller supplies
    // the actual1AF330(Application268) racer's four values, not host HUD bars.
    bool predraw18F4D0(std::uint32_t view,float damage7D4,float damage7DC,
        float damage7D0,float damage7D8)noexcept {
        if(view>=2)return false;
        for(float v:{damage7D4,damage7DC,damage7D0,damage7D8})
            if(!std::isfinite(v)||v<0)return false;
        //Above1 is admitted: source MIN.S bounds the selected maximum,
        //then SUB.S derives HUD health. Never clamp the input fields.
        const float a=damage7D4<damage7DC?damage7DC:damage7D4;
        const float b=damage7D0<damage7D8?damage7D8:damage7D0;
        health30[view]=sub(1.f,a>1.f?1.f:a);
        health38[view]=sub(1.f,b>1.f?1.f:b);
        return layout18EAD0(view+1);
    }
    struct DrawSequence {std::array<std::uint8_t,32> textureIndices{};std::size_t count=0;};
    //18F680..18FF7F leaf subset after its real outer race/view/visibility gates.
    // Caller owns those gates and actual view-state40==0. B9C bit1 tint is
    // required in addition to four damage values and nine signed10A8 slots.
    // Output order is source submission order, not permission to render.
    bool leaf18F680(std::uint32_t view,std::uint8_t flagsB9C,
        const std::array<std::int32_t,9>& attachment10A8,DrawSequence& out)noexcept {
        if(view>=2)return false;
        const float h0=health30[view],h1=health38[view];
        if(!std::isfinite(h0)||!std::isfinite(h1)||h0<0||h0>1||h1<0||h1>1)return false;
        const float scaled0=mul(10.f,h0),scaled1=mul(10.f,h1);
        // EE CVT.W.S truncates; helper310F38 unsigned conversion is also
        // truncation for the admitted finite nonnegative fractional alpha.
        const auto n0=static_cast<std::uint32_t>(scaled0);
        const auto n1=static_cast<std::uint32_t>(scaled1);
        const float f0=sub(scaled0,static_cast<float>(n0));
        const float f1=sub(scaled1,static_cast<float>(n1));
        const auto tint=[&](std::uint32_t n)noexcept {
            return (flagsB9C&2u)?0x006a9dd4u:
                n>=6?0x0065c96eu:n>=3?0x00c4d868u:0x00ca6060u;
        };
        DrawSequence next;
        const auto emit=[&](std::uint8_t i)noexcept {next.textureIndices[next.count++]=i;};
        emit(0);emit(1);emit(2);
        // Source completes both full-segment loops before either partial.
        for(std::uint32_t i=0;i<n0;++i){auto& t=textures[12+i];t.color(tint(n0));t.alpha(255);emit(static_cast<std::uint8_t>(12+i));}
        for(std::uint32_t i=0;i<n1;++i){auto& t=textures[22+i];t.color(tint(n1));t.alpha(255);emit(static_cast<std::uint8_t>(22+i));}
        if(n0<10){auto& t=textures[12+n0];t.color(tint(n0));
            t.alpha(static_cast<std::uint8_t>(static_cast<std::uint32_t>(mul(255.f,f0))&255u));
            emit(static_cast<std::uint8_t>(12+n0));}
        if(n1<10){auto& t=textures[22+n1];t.color(tint(n1));
            t.alpha(static_cast<std::uint8_t>(static_cast<std::uint32_t>(mul(255.f,f1))&255u));
            emit(static_cast<std::uint8_t>(22+n1));}
        for(std::size_t i=0;i<9;++i)if(attachment10A8[i]!=-1)emit(static_cast<std::uint8_t>(3+i));
        out=next;return true;
    }
};
} // namespace rr::game::damage_detail50LA

