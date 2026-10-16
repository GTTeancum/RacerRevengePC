#pragma once
// Private native CPU numeric/layout kernel for Speed/Boost189450. This is
// not a resource owner or evidence of renderer admission/display completion.
#include "original_ui_text_position50kg.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rr::game::speed_boost_detail50LA {
inline float bits(std::uint32_t word)noexcept {float f;std::memcpy(&f,&word,4);return f;}
inline float add(float a,float b)noexcept {volatile float v=a+b;return v;}
inline float sub(float a,float b)noexcept {volatile float v=a-b;return v;}
inline float mul(float a,float b)noexcept {volatile float v=a*b;return v;}
inline float div(float a,float b)noexcept {volatile float v=a/b;return v;}
struct Texture {
    float x=0,y=0,width=0,height=0,u0=0,v0=0,u1=1,v1=1;
    float atlasWidth=0,atlasHeight=0;
    std::uint8_t dirty=0;
};
struct Text {
    float x=0,y=0;
    std::array<char,41> bytes{};
    std::uint8_t length=0,dirty=0;
    std::uint32_t color=0,justification=0;
    std::int16_t alpha=255;
};
//2E2198/2E21A0 initialize source kind2 Z/W to1 before its first draw.
struct Vertex {float x=0,y=0,z=1,w=1,u=0,v=0;};
struct Triangle {
    // Separate genuine source vertices and native primitive aliases. Outer
    // layout/heat writes both. Only initial dirty transformation quantizes XY.
    std::array<Vertex,3> vertices{},packet{};
    std::array<float,4> delta{{0,0,0,1}};
    bool dirty=true;
    void position(std::size_t i,float x,float y)noexcept {
        vertices[i].x=x;vertices[i].y=y;
        packet[i].x=x;packet[i].y=y;packet[i].z=vertices[i].z;packet[i].w=vertices[i].w;
    }
    void uv(std::size_t i,float u,float v)noexcept {
        vertices[i].u=packet[i].u=u;vertices[i].v=packet[i].v=v;
    }
    bool prepareKind2Angle0(std::uint32_t actualVideoDepth60)noexcept {
        if(actualVideoDepth60!=16777215u)return false;
        if(!dirty)return true;
        auto next=packet;
        const float z=sub(static_cast<float>(actualVideoDepth60),1.f);
        for(std::size_t i=0;i<3;++i){
            std::int32_t x=0,y=0;
            if(!roundOriginalUiTextMetric50KG(vertices[i].x,1.f,x)||
               !roundOriginalUiTextMetric50KG(vertices[i].y,1.f,y))return false;
            next[i]=vertices[i];next[i].x=static_cast<float>(x);next[i].y=static_cast<float>(y);next[i].z=z;
        }
        for(auto& vertex:vertices)vertex.z=z;
        packet=next;dirty=false;return true;
    }
};
struct Submission {
    enum class Kind:std::uint8_t {texture,triangle};
    Kind kind=Kind::texture;std::uint8_t index=0;
};
struct State {
    std::array<Texture,15> textures{};
    std::array<Text,2> texts{};
    Triangle triangle;
    float cachedSpeed30=0;
    std::int32_t raceMode6CC=0,formatMode6E0=0;
    State()noexcept {
        texts[0].color=0xfffdff67;texts[0].justification=4;
        texts[1].color=0xff669bcc;texts[1].justification=0;
        triangle.uv(0,0,0);triangle.uv(1,0,1);triangle.uv(2,1,0);
    }
    // Called by the real loader only after resolving each canonical atlas.
    bool initializeTexture(std::size_t i,std::int32_t width,std::int32_t height)noexcept {
        if(i>=15||width<=0||height<=0)return false;
        auto& t=textures[i];t.atlasWidth=static_cast<float>(width);t.atlasHeight=static_cast<float>(height);
        static constexpr float widths[]={28,40,40,20,123,48,123,16,16,16,16,16,16,16,16};
        static constexpr float heights[]={28,40,40,20,32,57,22,16,16,16,16,16,16,16,16};
        t.width=widths[i];t.height=heights[i];t.dirty|=1;
        const float rw=div(1.f,t.atlasWidth),rh=div(1.f,t.atlasHeight);
        switch(i){
        case 0:t.u0=0;t.u1=mul(28,rw);t.v0=mul(42,rh);t.v1=mul(70,rh);break;
        case 1:t.u0=0;t.u1=mul(40,rw);t.v0=0;t.v1=mul(40,rh);break;
        case 2:t.u0=mul(40,rw);t.u1=mul(80,rw);t.v0=0;t.v1=mul(40,rh);break;
        case 3:t.u0=bits(0x3eec0000);t.u1=bits(0x3f1e0000);t.v0=bits(0x3ecd4fdf);t.v1=bits(0x3f100000);break;
        case 4:t.u0=0;t.u1=mul(124,rw);t.v0=mul(73,rh);t.v1=mul(105,rh);break;
        case 5:t.u0=mul(82,rw);t.u1=mul(128,rw);t.v0=mul(14,rh);t.v1=mul(72,rh);break;
        case 6:t.u0=0;t.u1=bits(0x3f780000);t.v0=bits(0x3f5170a4);t.v1=bits(0x3f7e0000);break;
        default:t.u0=t.v0=0;t.u1=t.v1=1;break;
        }
        return true;
    }
    bool layout18AD90(std::uint32_t oneBasedView)noexcept {
        if(oneBasedView<1||oneBasedView>2)return false;
        const float ox=raceMode6CC==0||raceMode6CC==1?0.f:oneBasedView==1?-300.f:-320.f;
        const float oy=raceMode6CC==1?(oneBasedView==1?-197.f:-224.f):0.f;
        static constexpr float xs[]={575,570,570,442,462,563,462,447,463,479,495,511,527,543,559};
        static constexpr float ys[]={352,345,345,395,362,358,393,398,398,398,398,398,398,398,397};
        for(std::size_t i=0;i<15;++i){auto& t=textures[i];const float x=add(xs[i],ox),y=add(ys[i],oy);
            if(t.x!=x||t.y!=y){t.x=x;t.y=y;t.dirty|=1;}}
        triangle.position(0,add(575,ox),add(367,oy));
        triangle.position(1,add(575,ox),add(428,oy));
        triangle.position(2,add(640,ox),add(367,oy));
        triangle.delta={{sub(triangle.vertices[2].x,triangle.vertices[1].x),
            sub(triangle.vertices[2].y,triangle.vertices[1].y),
            sub(triangle.vertices[2].z,triangle.vertices[1].z),1.f}};
        constexpr float tx[]={559,456},ty[]={362,398};
        for(std::size_t i=0;i<2;++i){auto& t=texts[i];const float x=add(tx[i],ox),y=add(ty[i],oy);
            if(t.x!=x||t.y!=y){t.x=x;t.y=y;t.dirty|=3;}}
        return true;
    }
    bool prepareSelected18BBB0(float actualBodySpeed34,std::uint32_t oneBasedView)noexcept {
        if(!std::isfinite(actualBodySpeed34))return false;
        const float speed=mul(bits(0x3f2e8bc1),actualBodySpeed34);
        if(!std::isfinite(speed))return false;
        cachedSpeed30=speed;return layout18AD90(oneBasedView);
    }
    bool assignText(std::size_t index,const char* bytes,std::size_t length)noexcept {
        if(index>=2||!bytes||length>40)return false;
        auto& t=texts[index];
        if(t.length==length&&(!length||!std::memcmp(t.bytes.data(),bytes,length)))return true;
        if(length)std::memcpy(t.bytes.data(),bytes,length);
        t.bytes[length]=0;t.length=static_cast<std::uint8_t>(length);t.dirty|=3;return true;
    }
    bool prepareNumeric18C320()noexcept {
        // The actual constructor's ordinary mode only. No debug input or
        // substituted Application FPS/counter state is introduced here.
        if(formatMode6E0!=0)return false;
        std::int32_t value=0;
        if(!roundOriginalUiTextMetric50KG(cachedSpeed30,1.f,value))return false;
        const bool negative=value<0;
        std::uint32_t magnitude=negative?0u-static_cast<std::uint32_t>(value):static_cast<std::uint32_t>(value);
        std::array<char,12> reversed{},formatted{};std::size_t digits=0,length=0;
        do {reversed[digits++]=static_cast<char>('0'+magnitude%10u);magnitude/=10u;}while(magnitude);
        const auto width=digits+(negative?1u:0u);
        for(std::size_t i=width;i<3;++i)formatted[length++]=' ';
        if(negative)formatted[length++]='-';
        while(digits)formatted[length++]=reversed[--digits];
        return assignText(0,formatted.data(),length);
    }
    // The caller must submit each item in order, preparing its genuine owned
    // primitive at that moment. No batching/sorting/readiness is certified here.
    bool heatSequence18BDF0(float actualHeat7C,std::uint32_t actualBoost64,
        bool actualShowSpeed10FA4,std::uint32_t actualBlinkD8,
        std::array<Submission,15>& output,std::size_t& count)noexcept {
        // Existing genuine Boost50FK clamps7C into this finite source domain.
        if(!std::isfinite(actualHeat7C)||actualHeat7C<0||actualHeat7C>1||actualBoost64>2)return false;
        for(const auto& t:textures)if(!(t.atlasWidth>0))return false;
        std::int32_t n=0;if(!roundOriginalUiTextMetric50KG(mul(10.f,actualHeat7C),1.f,n))return false;
        std::int32_t terminal=n+7;const float step=bits(0x3dcccccd);
        float fraction=terminal>=15?div(sub(actualHeat7C,mul(8.f,step)),mul(2.f,step)):
            div(sub(actualHeat7C,mul(static_cast<float>(n),step)),step);
        fraction=std::min(1.f,fraction);terminal=std::min(15,terminal);
        std::array<Submission,15> next{};std::size_t used=0;
        const auto texture=[&](std::uint8_t i){next[used++]={Submission::Kind::texture,i};};
        texture(0);texture(3);if(actualShowSpeed10FA4)texture(4);texture(5);texture(6);
        for(std::int32_t i=7;i<=terminal;++i){
            if(i==15){
                const auto& base=triangle.vertices[1];
                triangle.position(2,add(base.x,mul(fraction,triangle.delta[0])),add(base.y,mul(fraction,triangle.delta[1])));
                triangle.vertices[2].z=triangle.packet[2].z=add(base.z,mul(fraction,triangle.delta[2]));
                triangle.uv(2,fraction,sub(1.f,fraction));next[used++]={Submission::Kind::triangle,0};
            }else {auto& t=textures[static_cast<std::size_t>(i)];const float u=i==terminal?fraction:1.f;
                t.u1=u;const float width=mul(u,t.atlasWidth);if(t.width!=width){t.width=width;t.dirty|=1;}
                texture(static_cast<std::uint8_t>(i));}
        }
        // Source duplicates icon0 when idle; do not deduplicate submissions.
        texture(actualBoost64==0?0:actualBoost64==1||actualBlinkD8?1:2);
        output=next;count=used;return true;
    }
};
} // namespace rr::game::speed_boost_detail50LA
