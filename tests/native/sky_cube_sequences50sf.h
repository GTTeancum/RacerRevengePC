#pragma once
// Sky cube sequences (the original "lightning flash" overlay cubes), for any
// course whose sky data authors them. Nothing here names a course: a sky CUB
// with more than one internal cube plus a companion `.acb` text file next to
// it drives this; a course without the `.acb` member keeps the base cube only,
// exactly as before (the original 1299A0 returns before allocating anything).
//
// Original chain (SLUS_202.68, DrawableCube, "Cubedraw.cpp"):
//  1295B0  loads every internal cube of the CUB into the cube table (+3C, count
//          +7D8), current cube +38 = cubes[0], then calls 1299A0.
//  1299A0  derives "<cube path with extension replaced>.acb" (strrchr '.', else
//          append), opens it as text config (157320/1577D0); missing file =
//          no sequences. [Sequences] NumSequences (158B30, default 0; <=0 = none),
//          then per [Sequence%d] (40-byte rows at +4C, count +48):
//            +00 StartFrame-1  (158B80 default 0)
//            +04 EndFrame-1    (158B80 default 0)
//            +08 int(+04 - +00)
//            +0C FramesPerSecond (default 0)
//            +10 IdleTime        (default 0)
//            +14 RandomStartChance (158B30 default 100)
//            +18 elapsed = BeginTime (default 0)
//            +1C check accumulator = 1.0
//            +20 frame offset = 0
//            +24 bit0 active = 0
//          [CubeMapping] Face%dMapping (faces 1..6) = up to 16 comma-separated
//          panel ids per face (0 = none), one per tile, stored in the 6x16 table
//          of 0x14-byte panel records at +50 (+0 id, +4 count, +8.. up to 3
//          sequence indices). For each nonzero panel: [Panel%d] SequenceList =
//          comma-separated 1-based sequence numbers (assert count < 3, assert
//          number <= NumSequences), and every cube StartFrame-1 .. EndFrame-2 must
//          hold that face/tile ("Missing panel" assert).
//  12BB50  SkyCube update (288B00 slot 38, dt = the object-system frame time):
//          every inactive sequence's elapsed += dt; if none is active, the first
//          sequence (row order) with elapsed > IdleTime whose once-per-second
//          check comes due (check >= 1: reset to 0 and roll, else check += dt)
//          rolls one shared percentile 0..100 (lazy 80A8/3D6B30 101A50 seed, 308030
//          draw, "GetRandomNumber" Defs.h) and starts when percentile <
//          RandomStartChance (active, current +44 = row; only one at a time).
//          While one is active: frame += dt * FramesPerSecond; once frame >
//          (EndFrame-1)-(StartFrame-1) it ends (frame = 0, elapsed = 0, inactive).
//  12C540  GetTextureMap(face, tile): a tile whose panel lists the active current
//          sequence takes cubes[int(frame + StartFrame-1)]'s face/tile texture
//          (asserted < cube count), falling back to the current (base) cube's
//          texture when that cube lacks the tile; every other tile keeps the base.
//          Drawing (12BE50..12C3A0) is unchanged: same base-cube tile gates (+138
//          visible, +54 authored), same geometry, opaque; only the texture swaps.
//
// What this owner does: reads the .acb and the overlay cubes' tiles (decoded
// exactly like the base cube), runs the 12BB50 timer, and answers 12C540 per
// base sky span. It does not touch the base sky owner or its bank. Owned by the
// sky role (original_track_sky50kx_fixture.h); renderer: request.sky.sequenceTextures
// and frame.skySpanImages (vehicle_window_gl50ds.h).
//
// PORT bounds (flagged): the original asserts (and halts) where this owner
// refuses the flash data with a logged reason and keeps the base sky:
// SequenceList longer than 3, sequence number outside 1..NumSequences, a
// referenced cube outside the CUB, a "Missing panel", Face%dMapping longer than
// 16 ids, a missing [Panel%d] section. The original reads cubes[-1] for a
// StartFrame below 1 and writes past tile 15 for a longer mapping; both refused.
// Native ceilings: 64 sequences, 1,024 overlay tiles (64 MiB RGBA).
// 158B80 decimal conversion: strtod then binary32 (302E10 rounding not
// reproduced for non-representable decimals; every stock value is an integer).
#include "sky50gw_fixture.h"
#include "../../src/assets/original_parameter_lookup50cr.h"
#include "../../src/game/shared_random_owner50id.h"
#include "../../platform/linux/native/material_texture_gl50eb.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace sky_cube_sequences50SF {
namespace a=rr::assets;namespace g=rr::game;namespace sky=sky50gw_fixture;
inline constexpr std::uint32_t maxSequences=64,maxOverlayTiles=1024,noImage=0xffffffffu;
struct Diagnostic {
    std::string member,message;
    a::DecodeError48C decode{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
// 1299A0 40-byte row, field for field.
struct Sequence {
    float start=0.f,end=0.f;          // +00/+04: authored frame - 1
    std::int32_t frames=0;            // +08
    float framesPerSecond=0.f,idle=0.f;// +0C/+10
    std::int32_t chance=100;          // +14
    float elapsed=0.f,check=1.f,frame=0.f; // +18/+1C/+20
    bool active=false;                // +24 bit0
};
// +50 panel record (0x14 bytes): id, count, up to 3 zero-based sequence rows.
struct Panel {std::int32_t id=0;std::uint32_t count=0;std::array<std::int32_t,3> sequences{};};

namespace detail {
using sky::detail::need;using sky::detail::u32;using sky::detail::u16;using sky::detail::f32;
inline char fold(char c)noexcept{return c>='A'&&c<='Z'?static_cast<char>(c-'A'+'a'):c;}
inline bool sameFolded(std::string_view x,std::string_view y)noexcept{
    if(x.size()!=y.size())return false;
    for(std::size_t i=0;i<x.size();++i)if(fold(x[i])!=fold(y[i]))return false;return true;
}
// Text config subset the 1299A0 getters reach: 12EE90 lines, 159440 whitespace
// removal outside brackets/quotes, '[' headers (duplicates refused, 1576C8),
// ';' / empty lines skipped (1580EC), 158240 first "key=value" whose key matches
// case-insensitively (308F00); an empty value or no '=' is skipped (158800);
// at most one leading/trailing quote removed.
struct Text {
    struct Section {std::string name;std::vector<std::string> lines;};
    std::vector<Section> sections;
    void parse(const std::vector<std::uint8_t>& bytes){
        std::string raw;Section* current=nullptr;
        const auto flush=[&]{
            std::string clean;bool quoted=false;int brackets=0;
            for(char c:raw){
                if(c=='[')++brackets;else if(c==']')--brackets;else if(c=='"')quoted=!quoted;
                const bool white=c==' '||c=='\t'||c=='\r'||c=='\n';
                if(white&&brackets<=0&&!quoted)continue;
                clean.push_back(c);
            }
            need(clean.size()<1024,"Sky sequence text line longer than 1023 bytes");
            if(!clean.empty()&&clean[0]=='['){
                const auto close=clean.find(']');
                need(close!=std::string::npos,"Sky sequence section header unterminated");
                const auto name=clean.substr(1,close-1);
                for(const auto& s:sections)need(!sameFolded(s.name,name),"Sky sequence section duplicated: "+name);
                sections.push_back({name,{}});current=&sections.back();
            }else if(current&&!clean.empty()&&clean[0]!=';')current->lines.push_back(clean);
            raw.clear();
        };
        for(auto b:bytes){need(b!=0,"Sky sequence text holds a NUL byte");
            if(b=='\n')flush();else raw.push_back(static_cast<char>(b));}
        flush();
    }
    const Section* section(std::string_view name)const noexcept{
        for(const auto& s:sections)if(sameFolded(s.name,name))return &s;return nullptr;
    }
    static bool field(const Section* s,std::string_view key,std::string& value){
        if(!s)return false;
        for(const auto& line:s->lines){
            const auto eq=line.find('=');if(eq==std::string::npos||eq+1>=line.size())continue;
            if(!sameFolded(std::string_view(line).substr(0,eq),key))continue;
            std::string v=line.substr(eq+1);
            if(!v.empty()&&v.front()=='"')v.erase(0,1);
            if(!v.empty()&&v.back()=='"')v.pop_back();
            value=v;return true;
        }
        return false;
    }
    // 158B30 (158920 rules) / 158B80 with the caller's default.
    static std::int32_t integer(const Section* s,std::string_view key,std::int32_t fallback){
        std::string v;return field(s,key,v)?a::integer158920(v):fallback;
    }
    static float number(const Section* s,std::string_view key,float fallback){
        std::string v;if(!field(s,key,v))return fallback;
        const double parsed=std::strtod(v.c_str(),nullptr);
        need(std::isfinite(parsed),"Sky sequence number not finite: "+std::string(key));
        return static_cast<float>(parsed);
    }
    // 29FF50/29FFA0 over ",": empty tokens are not produced.
    static std::vector<std::string> tokens(const std::string& v){
        std::vector<std::string> out;std::string t;
        for(char c:v){if(c==','){if(!t.empty())out.push_back(t);t.clear();}else t.push_back(c);}
        if(!t.empty())out.push_back(t);return out;
    }
};
// 302E30 atoi.
inline std::int32_t atoi302E30(const std::string& t)noexcept{return static_cast<std::int32_t>(std::strtol(t.c_str(),nullptr,10));}
// 1299A0 name rule: replace from the last '.', else append.
inline std::string acbMember(const std::string& cubMember){
    const auto dot=cubMember.rfind('.');
    return (dot==std::string::npos?cubMember:cubMember.substr(0,dot))+".acb";
}
} // namespace detail

class Owner final {
    struct Storage {
        std::string acbMember;
        std::uint32_t internalCubes=0;
        std::vector<Sequence> sequences;
        std::array<std::array<Panel,16>,6> panels{};
        // Per internal cube, face, tile: overlay bank image (cube 0 = base: noImage).
        std::vector<std::array<std::array<std::uint32_t,16>,6>> tileImage;
        std::vector<std::array<std::uint16_t,6>> masks;
        a::MaterialTextureAssets50EA bank;
        // Base sky span -> (face, tile), and the per-span selection this frame.
        std::vector<std::pair<std::uint32_t,std::uint32_t>> spanTiles;
        std::vector<std::uint32_t> spanImage;   // overlay image or noImage (base)
        std::int32_t current=0;                 // +44
        std::uint64_t fired=0,steps=0,flashSteps=0; // flashSteps: steps leaving a span on a sequence image
    };
    std::unique_ptr<Storage> storage_;
    rr::platform::MaterialTextureGl50EB gpu_;
    bool absent_=false;
    // 275854..275964: only the camera-zone-selected sky cube child is updated
    // (flags29 bit0 gates 141ED0). Set from the frame's sky selection.
    bool selected_=true;
    std::uint64_t loggedStep_=0;
public:
    void setSelected(bool selected)noexcept{selected_=selected;}
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // True when the course's sky authors sequences and they loaded.
    bool loaded()const noexcept{return bool(storage_);}
    // True when the course has no .acb member (the original's no-sequence route).
    bool absent()const noexcept{return absent_;}
    std::uint32_t overlays()const noexcept{return storage_&&storage_->internalCubes?storage_->internalCubes-1u:0u;}
    std::uint64_t fired()const noexcept{return storage_?storage_->fired:0u;}
    std::size_t sequenceCount()const noexcept{return storage_?storage_->sequences.size():0u;}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    // Per base span this frame: overlay image index in bank(), or noImage (base texture).
    const std::vector<std::uint32_t>* spanImages()const noexcept{return storage_?&storage_->spanImage:nullptr;}

    // extract(member,bytes,DecodeError48C&) reads one member of the course's own
    // archive (the same reader the base sky load used). base = the loaded base sky.
    template<class Extract> bool load(Extract&& extract,const sky::Owner& base,Diagnostic& d)noexcept{
        using namespace detail;
        try{
            d={};absent_=false;
            need(!storage_,"Sky sequences already loaded");
            need(base.loaded()&&base.cubeMember()&&base.spans(),"Sky sequences need the loaded base sky");
            const auto member=acbMember(*base.cubeMember());d.member=member;
            std::vector<std::uint8_t> acb;a::DecodeError48C error;
            if(!extract(member,acb,error)){
                if(error.code=="res.member_missing"){absent_=true;return true;} // 1577D0 failure: no sequences.
                d.decode=error;need(false,"Sky sequence member "+member+": "+error.detail);
            }
            Text text;text.parse(acb);
            const auto* list=text.section("Sequences");
            const auto count=Text::integer(list,"NumSequences",0);
            if(count<=0){absent_=true;return true;} // 129A6C: no sequence rows, base only.
            need(static_cast<std::uint32_t>(count)<=maxSequences,"Sky sequences above the native ceiling");
            const auto cubes=base.internalCubes();
            need(cubes>=2,"Sky sequences authored but the CUB holds only the base cube");
            auto next=std::make_unique<Storage>();auto& s=*next;
            s.acbMember=member;s.internalCubes=cubes;
            s.sequences.resize(static_cast<std::size_t>(count));
            for(std::int32_t i=0;i<count;++i){
                const auto* sec=text.section("Sequence"+std::to_string(i+1));
                auto& q=s.sequences[static_cast<std::size_t>(i)];
                q.start=Text::number(sec,"StartFrame",0.f)-1.f;
                q.end=Text::number(sec,"EndFrame",0.f)-1.f;
                q.frames=static_cast<std::int32_t>(q.end-q.start);
                q.framesPerSecond=Text::number(sec,"FramesPerSecond",0.f);
                q.idle=Text::number(sec,"IdleTime",0.f);
                q.chance=Text::integer(sec,"RandomStartChance",100);
                q.elapsed=Text::number(sec,"BeginTime",0.f);
                q.check=1.f;q.frame=0.f;q.active=false;
                // PORT bound: the frame-to-cube index int(frame+start) must stay in the CUB.
                // (An EndFrame before StartFrame is legal: the sequence ends on its first step.)
                need(q.start>=0.f&&static_cast<std::uint32_t>(q.end>q.start?q.end:q.start)<cubes,
                    "Sky sequence "+std::to_string(i+1)+" frames outside the CUB's cubes");
            }
            // Overlay cubes 1..n-1, decoded like the base cube (1289C0/1290E0/128D50).
            std::vector<std::uint8_t> cub;
            need(extract(*base.cubeMember(),cub,error),"Sky sequence CUB re-read: "+error.detail);
            need(cub.size()==base.cubeBytes()&&cub.size()>=12u+4u*cubes&&u32(cub.data()+4)==cubes,"Sky sequence CUB differs from the base load");
            s.tileImage.assign(cubes,{});s.masks.assign(cubes,{});
            for(auto& c:s.tileImage)for(auto& f:c)f.fill(noImage);
            std::vector<std::vector<std::uint8_t>> images;
            for(std::uint32_t cube=0;cube<cubes;++cube){
                const std::size_t header=u32(cub.data()+12+4*cube);
                const std::size_t end=cube+1<cubes?u32(cub.data()+12+4*(cube+1)):cub.size();
                need(header>=12u+4u*cubes&&header+80u<=end&&end<=cub.size(),"Sky sequence cube extent");
                const auto* h=cub.data()+header;
                need(u32(h)==32&&u16(h+4)==0&&u16(h+6)==0,"Sky sequence cube texture/flags");
                for(unsigned i=0;i<9;++i)need(f32(h+44+i*4)==sky::detail::cubSourcePose[i],"Sky sequence cube origin/look/up");
                std::size_t at=header+80u;
                for(std::uint32_t face=0;face<6;++face){
                    const auto mask=u16(h+8+face*2);s.masks[cube][face]=mask;
                    std::uint32_t sizes=0;for(unsigned t=0;t<16;++t)if(mask&(1u<<t))sizes|=2u<<(t*2);
                    need(u32(h+20+face*4)==sizes,"Sky sequence cube tile size codes (128 only)");
                    for(std::uint32_t tile=0;tile<16;++tile){
                        if(!(mask&(1u<<tile)))continue;
                        need(at<=end,"Sky sequence tile offset");
                        std::size_t largest=0,largestBytes=0;
                        const auto bytes=sky::detail::walkTile(cub.data()+at,end-at,largest,largestBytes);
                        if(cube){
                            need(images.size()<maxOverlayTiles,"Sky sequence overlay tiles above the native ceiling");
                            images.emplace_back();
                            sky::detail::decodeLargestMip(cub.data()+at,largest,largestBytes,images.back());
                            s.tileImage[cube][face][tile]=static_cast<std::uint32_t>(images.size()-1);
                        }
                        at+=bytes;
                    }
                }
                need(at==end,"Sky sequence cube tile closure");
            }
            need(!images.empty(),"Sky sequence overlay cubes hold no tile");
            // [CubeMapping] Face%dMapping -> panel ids.
            const auto* mapping=text.section("CubeMapping");
            for(std::uint32_t face=0;face<6;++face){
                std::string v;Text::field(mapping,"Face"+std::to_string(face+1)+"Mapping",v);
                const auto ids=Text::tokens(v);
                need(ids.size()<=16,"Sky sequence Face"+std::to_string(face+1)+"Mapping longer than 16 tiles");
                for(std::size_t tile=0;tile<ids.size();++tile)s.panels[face][tile].id=atoi302E30(ids[tile]);
            }
            // [Panel%d] SequenceList, then the "Missing panel" check.
            for(std::uint32_t face=0;face<6;++face)for(std::uint32_t tile=0;tile<16;++tile){
                auto& panel=s.panels[face][tile];if(!panel.id)continue;
                const auto name="Panel"+std::to_string(panel.id);
                const auto* sec=text.section(name);need(sec!=nullptr,"Sky sequence ["+name+"] missing");
                std::string v;Text::field(sec,"SequenceList",v);panel.count=0;
                for(const auto& t:Text::tokens(v)){
                    need(panel.count<3,name+" - SequenceList too high");
                    const auto row=atoi302E30(t)-1;
                    need(row>=0&&row<count,"PanelID "+std::to_string(panel.id)+" sequence number outside the sequences");
                    const auto& q=s.sequences[static_cast<std::size_t>(row)];
                    for(auto c=static_cast<std::int32_t>(q.start);static_cast<float>(c)<q.end;++c){
                        const auto cube=static_cast<std::uint32_t>(c);
                        need(cube<cubes&&(s.masks[cube][face]&(1u<<tile)),"Missing panel (face "+std::to_string(face+1)+
                            " panel "+std::to_string(tile+1)+" frame "+std::to_string(c+1)+" sequence "+std::to_string(row+1)+")");
                    }
                    panel.sequences[panel.count++]=row;
                }
            }
            // Overlay bank, same framing as the base sky bank.
            const auto n=static_cast<std::uint32_t>(images.size());
            std::vector<a::MaterialTextureAssetInput50EA> textures(n);
            std::vector<a::MaterialTextureSlotInput50EA> slots(n);
            for(std::uint32_t i=0;i<n;++i){
                textures[i]={i,&images[i]};
                slots[i]={{0,i,1,1},i,a::MaterialTextureRequirement50EA::required};
            }
            const a::MaterialTextureAssetsInput50EA input{textures.data(),n,slots.data(),n,{n,n,n*65552u,n*65536u,128}};
            need(s.bank.load(input,d.bank),"Sky sequence texture bank: "+d.bank.parserMessage);
            for(const auto& span:*base.spans()){
                need(span.face<6&&span.tile<16,"Sky sequence base span tile");
                s.spanTiles.push_back({span.face,span.tile});
            }
            s.spanImage.assign(s.spanTiles.size(),noImage);
            storage_=std::move(next);return true;
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}
            std::cerr<<"sky_flash refused member="<<d.member<<" reason="<<d.message<<'\n';return false;}
        catch(...){try{d.message="Sky sequence allocation failure";}catch(...){}return false;}
    }

    // 12BB50 with dt; then the 12C540 answer for every base span. paused: the
    // caller's world-children pause rule (PORT: same as the track animations).
    // timestamp: the 101A50 value used only if the shared seed is still zero.
    bool update(float dt,bool paused,std::uint32_t timestamp,std::uint32_t& raw,
        g::SharedRandomOwner50ID& random,Diagnostic& d)noexcept{
        d={};if(!storage_)return true;
        if(!std::isfinite(dt)||dt<0.f){d.message="Sky sequence update dt";return false;}
        auto& s=*storage_;
        if(!paused&&selected_){
            ++s.steps;
            bool none=true;
            for(auto& q:s.sequences){if(q.active)none=false;else q.elapsed+=dt;}
            if(none){
                for(std::size_t i=0;i<s.sequences.size();++i){
                    auto& q=s.sequences[i];
                    if(q.elapsed<=q.idle)continue;
                    if(q.check<1.f){q.check+=dt;continue;}
                    q.check=0.f;
                    // Shared percentile (same steps as the 101860 track-animation draw).
                    if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
                    if(!random.seed.seed3D6B30){random.seed.seed3D6B30=timestamp;raw=timestamp;}
                    raw=raw*0x41c64e6du+0x3039u;volatile float converted=static_cast<float>(raw&0x7fffffffu);
                    volatile float unit=converted*0x1p-31f;volatile float percent=101.f*unit;
                    if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
                    const auto selected=static_cast<std::int32_t>(percent);
                    if(random.range.rangeCheck3D6B38&&(selected<0||selected>=101)){d.message="assumption violated in GetRandomNumber";return false;}
                    if(selected<q.chance){q.active=true;s.current=static_cast<std::int32_t>(i);++s.fired;break;}
                }
            }else{
                auto& q=s.sequences[static_cast<std::size_t>(s.current)];
                q.frame+=dt*q.framesPerSecond;
                if(!(q.frame<=q.end-q.start)){q.frame=0.f;q.elapsed=0.f;q.active=false;}
            }
        }
        // 12C540 per base span.
        const auto& q=s.sequences[static_cast<std::size_t>(s.current)];
        bool anyFlash=false;
        for(std::size_t i=0;i<s.spanTiles.size();++i){
            const auto [face,tile]=s.spanTiles[i];const auto& panel=s.panels[face][tile];
            std::uint32_t image=noImage;
            if(panel.id&&q.active)for(std::uint32_t k=0;k<panel.count;++k){
                if(panel.sequences[k]!=s.current)continue;
                const auto cube=static_cast<std::int32_t>(q.frame+q.start);
                if(cube<0||static_cast<std::uint32_t>(cube)>=s.internalCubes){d.message="DrawableCube::GetTextureMap LpCubes index error";return false;}
                image=s.tileImage[static_cast<std::size_t>(cube)][face][tile]; // noImage = cube lacks the tile: base.
                break;
            }
            s.spanImage[i]=image;anyFlash=anyFlash||image!=noImage;
        }
        if(anyFlash&&!paused&&selected_)++s.flashSteps;
        return true;
    }
    // Upload the overlay bank with the base sky's sampling (linear, clamp-to-edge).
    bool createGpu(const rr::platform::MaterialTextureGlRequest50EB& caller,
        rr::platform::MaterialTextureGlDiagnostic50EB& d)noexcept{
        namespace p=rr::platform;
        if(!storage_)return false;
        try{
            const auto n=storage_->bank.textureCount();
            std::vector<p::MaterialTextureSampler50EB> samplers(n,
                {p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
                 p::MaterialTextureWrap50EB::clampToEdge,p::MaterialTextureWrap50EB::clampToEdge});
            auto request=caller;request.samplers=samplers.data();request.samplerCount=n;
            request.maximumGpuBytes=n*128u*128u*4u;
            return gpu_.create(request,storage_->bank,d);
        }catch(...){return false;}
    }
    bool bindRequest(rr::platform::VehicleWindowRequest50DS& request)const noexcept{
        if(!storage_||!gpu_.initialized()||!request.sky.textures)return false;
        request.sky.sequenceTextures=&gpu_;return true;
    }
    // After update(): this frame's per-span images (borrowed through submit).
    bool bindFrame(rr::platform::VehicleWindowFrame50DS& frame)const noexcept{
        if(!storage_||!gpu_.initialized())return false;
        static_assert(noImage==rr::platform::vehicleWindowSkyBaseImage50DS);
        frame.skySpanImages=storage_->spanImage.data();
        frame.skySpanImageCount=static_cast<std::uint32_t>(storage_->spanImage.size());return true;
    }
    bool ownsResources()const noexcept{return gpu_.ownsResources();}
    bool close(rr::platform::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
    // sky_flash course=<code> overlays=<n> fired=<count>, every `every` update steps.
    void logPeriodic(std::string_view course,std::uint64_t every=600){
        if(!storage_||!every||!storage_->steps||storage_->steps%every||storage_->steps==loggedStep_)return;
        loggedStep_=storage_->steps;
        std::cerr<<"sky_flash course="<<course<<" overlays="<<overlays()<<" fired="<<fired()<<" flash_steps="<<storage_->flashSteps<<" steps="<<storage_->steps<<'\n';
    }
    void logLoad(std::string_view course)const{
        std::cerr<<"sky_flash course="<<course<<" overlays="<<overlays()<<" sequences="<<sequenceCount()
            <<" overlay_tiles="<<(storage_?storage_->bank.textureCount():0u)<<" acb="<<(storage_?"loaded":(absent_?"absent":"none"))<<'\n';
    }
};
} // namespace sky_cube_sequences50SF
