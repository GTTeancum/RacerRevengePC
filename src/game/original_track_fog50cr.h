#pragma once
// CR1 common track fog: one reader and one runtime for every course. Courses
// differ only in the SCB data they load (Fog block, ZoneN fog fields), never
// in which code runs. Header-only: no files, no allocation after prepare(),
// no guest memory. Every read is bounds-checked; a value that does not fit is
// refused with a diagnostic before anything is stored.
//
// Original (SLUS_202.68):
//  278430  fog record ctor: clears colour00..0C, vis10=1.0, haz14=0.5, table3D2AA0
//  278570  Fog block reader (table3D2AA0 slot8, called at 270B3C): 157D00 "Fog",
//          158AD0 "ColorRGB" default "undef" -> 26FA60, 158B80 "Visibility" 1.0,
//          158B80 "Haziness" 0.5. Result is World3AC. No Fog block: 270B4C frees
//          it and World3AC stays NULL.
//  26FA60  colour parser: strtok "," + atoi; "undef"/missing R -> 255,255,255,255,
//          missing G -> G,B,A 255, missing B -> B,A 255, missing A -> A 255.
//  275E30  ZoneInfo reader (ZoneInfo is 0x160 bytes, World350[zone48]):
//          276254 "FogColorRGB" default "undef" (undef copies World3AC colour),
//          2762DC "FogVisibility" and 2762F8 "FogHaziness" default to the World3AC
//          values; 276304 278480 compares the six lanes; any difference sets
//          World4F0 bit0 (276314..276328).
//  270D1C  if World4F0 bit0: five 0x14-byte controllers World4DC..4EC
//          {current 1.0, period 0.1, factor 1.0, FLT_MAX, -FLT_MAX}.
//  275630  PodScene preparation, every rendered frame and camera: if World4F0 bit0,
//          278430 stack record, 275990 blend, then Application264==1 steps the five
//          controllers with Application250 (else uses the blend directly), packs
//          RGB and calls 13E830 on World3EC.
//  275990  blend: primary zone (camera34->4->48) weight 1; each node of its +30
//          list whose portal overlaps (231150, radius 200 at 442FE8) adds weight
//          (200-d)/200 of that neighbour zone's fog; normalized by total weight.
//  13E830  fog setter: +30 = rgb|FF000000, +3C vis, +40 haz,
//          end38 = near54 + vis*(far50-near54), start34 = (1-haz)*(end38-min58),
//          +60 = 1, renderer(442F70)+28 = +30 (viewport clear colour, 2B4878).
#include "../assets/import_world48f.h"
#include "../assets/original_parameter_lookup50cr.h"
#include "../assets/track_start50ad.h"
#include "original_camera_start_effects.h"
#include "original_zone_fog_overlap50cr.h"
#include <array>
#include <cfenv>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <ostream>
#include <string>
#include <vector>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Original track fog requires strict finite IEEE binary32 arithmetic"
#endif

namespace rr::game {
// decodeWorld48F caps NumZones at 256 (WorldLimits48F::zones); the visibility
// world the camera walks holds at most 30 zone keys.
inline constexpr std::size_t originalTrackFogZoneLimit50CR=256;
// Camera region-list records read per frame; storage capacity is portals+1.
inline constexpr std::size_t originalTrackFogOverlapLimit50CR=128;

struct OriginalFogDescriptor278430_50CR {
    std::array<float,4> color00{{0.f,0.f,0.f,0.f}};
    float visibility10=1.0f,haziness14=0.5f;
};
struct OriginalTrackFogZone50CR {
    OriginalFogDescriptor278430_50CR fog{};   // ZoneInfo+12C..+140
    bool colorAuthored=false,visibilityAuthored=false,hazinessAuthored=false;
    bool differs=false;                       // 278480 said "not equal"
};
struct OriginalTrackFog50CR {
    std::size_t scbBytes=0,fogBlockOffset=0;
    OriginalFogDescriptor278430_50CR global3AC{};
    bool colorAuthored=false,visibilityAuthored=false,hazinessAuthored=false;
    std::vector<OriginalTrackFogZone50CR> zones350; // index = zone48
    bool zoneFogDiffers4F0=false;                   // World4F0 bit0
};
enum class OriginalTrackFogStatus50CR : std::uint8_t {
    complete,noFogBlock,duplicateBlock,duplicateField,valueLength,number,color,range,
    zoneCount,zoneBlock,arithmetic,state,cameraZone,overlapCapacity
};
inline const char* originalTrackFogStatusName50CR(OriginalTrackFogStatus50CR s) noexcept {
    using FogS50CR=OriginalTrackFogStatus50CR;
    switch(s){
    case FogS50CR::complete:return "complete";case FogS50CR::noFogBlock:return "no_fog_block";
    case FogS50CR::duplicateBlock:return "duplicate_block";case FogS50CR::duplicateField:return "duplicate_field";
    case FogS50CR::valueLength:return "value_length";case FogS50CR::number:return "number";case FogS50CR::color:return "color";
    case FogS50CR::range:return "range";case FogS50CR::zoneCount:return "zone_count";case FogS50CR::zoneBlock:return "zone_block";
    case FogS50CR::arithmetic:return "arithmetic";case FogS50CR::state:return "state";case FogS50CR::cameraZone:return "camera_zone";
    case FogS50CR::overlapCapacity:return "overlap_capacity";
    }
    return "unknown";
}
struct OriginalTrackFogDiagnostic50CR {
    OriginalTrackFogStatus50CR status=OriginalTrackFogStatus50CR::complete;
    std::uint32_t sourcePC=0;
    std::size_t sourceOffset=0;
    std::uint32_t zone=0; // one-based ZoneN, 0 = global Fog block
};

namespace track_fog50cr_detail {
using FogD50CR=OriginalTrackFogDiagnostic50CR;using FogS50CR=OriginalTrackFogStatus50CR;
inline float fogAdd(float a,float b) noexcept {volatile float r=a+b;return r;}
inline float fogSub(float a,float b) noexcept {volatile float r=a-b;return r;}
inline float fogMul(float a,float b) noexcept {volatile float r=a*b;return r;}
inline float fogDiv(float a,float b) noexcept {volatile float r=a/b;return r;}
inline float fogBits(std::uint32_t word) noexcept {float f;std::memcpy(&f,&word,4);return f;}
inline bool fogFail(FogD50CR& d,FogS50CR s,std::uint32_t pc,std::size_t offset,std::uint32_t zone=0) noexcept {
    d={s,pc,offset,zone};return false;
}
inline bool fogFraction(float v) noexcept {return std::isfinite(v)&&v>=0.f&&v<=1.f;}
// 157D00 selection. A repeated label is refused, never resolved by order.
inline bool fogBlock(const assets::ParameterDocument48F& doc,const char* name,
    const assets::ParameterBlock48F*& out,FogD50CR& d,std::uint32_t pc) noexcept {
    out=nullptr;
    for(const auto& b:doc.blocks)if(b.name==name){
        if(out)return fogFail(d,FogS50CR::duplicateBlock,pc,b.offset);
        out=&b;
    }
    return true;
}
// 158240 lookup inside the selected block only.
inline bool fogField(const assets::ParameterBlock48F& b,const char* name,
    const assets::ParameterField48F*& out,FogD50CR& d,std::uint32_t pc,std::uint32_t zone) noexcept {
    // 1583AC: the shared original search; a duplicated label resolves as the original does (B8).
    (void)d;(void)pc;(void)zone;
    out=assets::packedField1583AC(b,name);
    return true;
}
// 158B80 number. Same rule as the Garage DAT reader: an omitted integral zero
// (".5", "-.25") is admitted; parseTrackNumber50AD does the exact decimal
// conversion and holds at most 13 bytes, so the source text is at most 12.
inline bool fogNumber(const assets::ParameterField48F& f,float& out,FogD50CR& d,std::uint32_t pc,
    std::uint32_t zone) noexcept {
    const auto& t=f.value;const std::size_t n=t.size();
    if(!n||n>12)return fogFail(d,FogS50CR::valueLength,pc,f.value_offset,zone);
    char buffer[14]{};std::size_t at=0,i=0;
    if(t[0]=='-'||t[0]=='+')buffer[at++]=t[i++];
    if(i<n&&t[i]=='.')buffer[at++]='0';
    for(;i<n;++i){
        if(at>=sizeof buffer-1)return fogFail(d,FogS50CR::valueLength,pc,f.value_offset,zone);
        buffer[at++]=t[i];
    }
    float value=0.f;
    if(!assets::parseTrackNumber50AD(buffer,at,value))return fogFail(d,FogS50CR::number,pc,f.value_offset,zone);
    out=value;return true;
}
// One atoi token: optional blanks, 1..3 decimal digits, optional blanks.
// Anything else is refused rather than guessed (atoi would read a prefix).
inline bool fogChannel(const std::string& t,std::size_t first,std::size_t last,int& out) noexcept {
    while(first<last&&(t[first]==' '||t[first]=='\t'))++first;
    while(last>first&&(t[last-1]==' '||t[last-1]=='\t'))--last;
    if(first==last||last-first>3)return false;
    int v=0;
    for(std::size_t i=first;i<last;++i){
        if(t[i]<'0'||t[i]>'9')return false;
        v=v*10+(t[i]-'0');
    }
    out=v;return true;
}
// 26FA60. Text buffer at 275F8C/2785B8 is 0x100 bytes, so longer text is refused.
inline bool fogColor(const std::string& text,std::size_t offset,std::array<float,4>& out,FogD50CR& d,
    std::uint32_t pc,std::uint32_t zone) noexcept {
    if(text.size()>255)return fogFail(d,FogS50CR::valueLength,pc,offset,zone);
    std::array<float,4> v{{255.f,255.f,255.f,255.f}};
    std::size_t at=0;
    const auto next=[&](std::size_t& first,std::size_t& last) noexcept -> bool {
        while(at<text.size()&&text[at]==',')++at; // strtok skips leading delimiters
        if(at>=text.size())return false;
        first=at;while(at<text.size()&&text[at]!=',')++at;last=at;return true;
    };
    std::size_t first=0,last=0;
    if(!next(first,last)||text.compare(first,last-first,"undef")==0){out=v;return true;}
    for(unsigned k=0;k<4;++k){
        if(k&&!next(first,last))break; // 26FAB0/26FB28/26FB9C/26FC08: rest stays 255
        int value=0;
        if(!fogChannel(text,first,last,value))return fogFail(d,FogS50CR::color,pc,offset+first,zone);
        if(value>255)return fogFail(d,FogS50CR::range,pc,offset+first,zone);
        v[k]=static_cast<float>(value); // 302E30 atoi, cvt.s.w
    }
    out=v;return true;
}
// 278480: EQ.S on six lanes (colour 00..0C, +10, +14); +0 equals -0.
inline bool fogSameDescriptor(const OriginalFogDescriptor278430_50CR& a,
    const OriginalFogDescriptor278430_50CR& b) noexcept {
    for(unsigned k=0;k<4;++k)if(!(a.color00[k]==b.color00[k]))return false;
    return a.visibility10==b.visibility10&&a.haziness14==b.haziness14;
}
// cvt.w.s on the EE FPU always rounds toward zero; the static_cast does the same.
inline bool fogPackRgb(float r,float g,float b,std::uint32_t& out) noexcept {
    const float c[3]={r,g,b};std::uint32_t lane[3]{};
    for(unsigned k=0;k<3;++k){
        if(!std::isfinite(c[k])||c[k]<0.f||c[k]>255.f)return false;
        lane[k]=static_cast<std::uint32_t>(static_cast<std::int32_t>(c[k]));
    }
    out=(lane[0]<<16)|(lane[1]<<8)|lane[2];return true;
}
inline void fogChannels(std::uint32_t color30,unsigned rgb[3]) noexcept {
    rgb[0]=(color30>>16)&0xffu;rgb[1]=(color30>>8)&0xffu;rgb[2]=color30&0xffu;
}
} // namespace track_fog50cr_detail

// 278570 + 270B58..270BF8/275E30 fog lanes, over the track's own decoded SCB.
// Transactional: out is written only on success. Allocates the zone table once.
inline bool readOriginalTrackFog50CR(const assets::World48F& world,OriginalTrackFog50CR& out,
    OriginalTrackFogDiagnostic50CR& d) {
    using namespace track_fog50cr_detail;
    d={};
    const auto& doc=world.parameters;
    // A document decoded with its duplicate labels kept (MC) is admitted: every
    // block and field read below refuses an ambiguous name on its own.
    if(doc.version!=1u)return fogFail(d,FogS50CR::state,0x278570,0);
    OriginalTrackFog50CR next;next.scbBytes=doc.original_bytes.size();
    const assets::ParameterBlock48F* fog=nullptr;
    if(!fogBlock(doc,"fog",fog,d,0x278590))return false;
    // Without a Fog block World3AC stays NULL and 275E30 has no defaults to copy.
    if(!fog)return fogFail(d,FogS50CR::noFogBlock,0x278598,0);
    if(fog->offset>=doc.original_bytes.size())return fogFail(d,FogS50CR::state,0x278590,fog->offset);
    next.fogBlockOffset=fog->offset;
    const assets::ParameterField48F* f=nullptr;
    if(!fogField(*fog,"colorrgb",f,d,0x2785c0,0))return false;
    next.colorAuthored=f!=nullptr;
    if(!fogColor(f?f->value:std::string("undef"),f?f->value_offset:fog->offset,
        next.global3AC.color00,d,0x2785cc,0))return false;
    if(!fogField(*fog,"visibility",f,d,0x278608,0))return false;
    next.visibilityAuthored=f!=nullptr;
    if(f&&!fogNumber(*f,next.global3AC.visibility10,d,0x278608,0))return false;
    if(!fogField(*fog,"haziness",f,d,0x278624,0))return false;
    next.hazinessAuthored=f!=nullptr;
    if(f&&!fogNumber(*f,next.global3AC.haziness14,d,0x278624,0))return false;
    // Native admission: 13E830 interpolates between near and far; a fraction
    // outside [0,1] would put the fog outside that range. Refused, not clamped.
    if(!fogFraction(next.global3AC.visibility10)||!fogFraction(next.global3AC.haziness14))
        return fogFail(d,FogS50CR::range,0x278608,fog->offset);

    const std::size_t count=world.zones.size(); // World34C
    if(!count||count>originalTrackFogZoneLimit50CR)return fogFail(d,FogS50CR::zoneCount,0x270b58,0);
    next.zones350.resize(count);
    for(std::size_t i=0;i<count;++i){
        const auto zone=static_cast<std::uint32_t>(i+1);
        const auto& z=world.zones[i];
        if(z.zone!=zone||z.parameter_block>=doc.blocks.size())
            return fogFail(d,FogS50CR::zoneBlock,0x275e78,0,zone);
        const auto& b=doc.blocks[z.parameter_block];
        if(b.name!="zone"+std::to_string(zone))return fogFail(d,FogS50CR::zoneBlock,0x275e78,b.offset,zone);
        auto& r=next.zones350[i];
        if(!fogField(b,"fogcolorrgb",f,d,0x276254,zone))return false;
        if(!f||f->value=="undef")r.fog.color00=next.global3AC.color00; //27627C..276298
        else{
            r.colorAuthored=true;
            if(!fogColor(f->value,f->value_offset,r.fog.color00,d,0x2762a0,zone))return false;
        }
        r.fog.visibility10=next.global3AC.visibility10; //2762C8..2762D8 default = World3AC+10
        if(!fogField(b,"fogvisibility",f,d,0x2762dc,zone))return false;
        if(f){r.visibilityAuthored=true;if(!fogNumber(*f,r.fog.visibility10,d,0x2762dc,zone))return false;}
        r.fog.haziness14=next.global3AC.haziness14;     //2762E4..2762F4 default = World3AC+14
        if(!fogField(b,"foghaziness",f,d,0x2762f8,zone))return false;
        if(f){r.hazinessAuthored=true;if(!fogNumber(*f,r.fog.haziness14,d,0x2762f8,zone))return false;}
        if(!fogFraction(r.fog.visibility10)||!fogFraction(r.fog.haziness14))
            return fogFail(d,FogS50CR::range,0x2762f8,b.offset,zone);
        r.differs=!fogSameDescriptor(next.global3AC,r.fog);
        if(r.differs)next.zoneFogDiffers4F0=true; //276328
    }
    out=std::move(next);return true;
}

// 13E830 on an already ranged World3EC (far50/near54/min58 set by 13E920 or
// restoreOriginalRaceFog1A98DC). Transactional; refuses unranged storage.
inline bool setOriginalRaceFog13E830_50CR(OriginalRaceFogFields& fog,std::uint32_t rgb,
    float visibility,float haziness) noexcept {
    using namespace track_fog50cr_detail;
    if(std::fegetround()!=FE_TONEAREST)return false;
    if(fog.dirty60!=1u||!std::isfinite(fog.far50)||!std::isfinite(fog.near54)||
       !std::isfinite(fog.minimum58)||!(fog.far50>fog.near54))return false;
    if(rgb>0x00ffffffu||!fogFraction(visibility)||!fogFraction(haziness))return false;
    auto n=fog;
    n.color30=rgb|0xff000000u;                          //13E840..13E84C
    n.authoredFraction3C=visibility;                    //13E850
    n.authoredFraction40=haziness;                      //13E854
    const float span=fogSub(n.far50,n.near54);             //13E874
    n.end38=fogAdd(n.near54,fogMul(n.authoredFraction3C,span)); //13E87C..13E884
    n.start34=fogMul(fogSub(1.0f,n.authoredFraction40),fogSub(n.end38,n.minimum58)); //13E890..13E89C
    if(!std::isfinite(n.end38)||!std::isfinite(n.start34)||n.start34<0.f||!(n.end38>n.start34))return false;
    n.dirty60=1u;                                       //13E8B0
    fog=n;return true;
}
inline bool sameOriginalRaceFogFields50CR(const OriginalRaceFogFields& a,const OriginalRaceFogFields& b) noexcept {
    return a.color30==b.color30&&a.start34==b.start34&&a.end38==b.end38&&
        a.authoredFraction3C==b.authoredFraction3C&&a.authoredFraction40==b.authoredFraction40&&
        a.far50==b.far50&&a.near54==b.near54&&a.minimum58==b.minimum58&&a.dirty60==b.dirty60;
}
// World3EC as the track load leaves it: 273C64..273CB0 packs the World3AC
// colour, 13E780 -> 13E830 forces alpha FF, and the PodRaceEvent load passes
// far 4000 / near bits 44A6A99A / base 64 (1AA158..1AA178), which
// restoreOriginalRaceFog1A98DC applies with the same 13E830 arithmetic.
inline bool loadOriginalRaceFogFields50CR(const OriginalTrackFog50CR& fog,OriginalRaceFogFields& out,
    OriginalTrackFogDiagnostic50CR& d) noexcept {
    using namespace track_fog50cr_detail;
    std::uint32_t rgb=0;
    const auto& c=fog.global3AC.color00;
    if(!fogPackRgb(c[0],c[1],c[2],rgb))return fogFail(d,FogS50CR::color,0x273c64,fog.fogBlockOffset);
    OriginalRaceFogFields next{};
    next.color30=0xff000000u|rgb;
    next.authoredFraction3C=fog.global3AC.visibility10;
    next.authoredFraction40=fog.global3AC.haziness14;
    std::uint32_t renderer28=0;
    if(restoreOriginalRaceFog1A98DC(next,renderer28).status!=OriginalCameraStartStatus::complete||
       renderer28!=next.color30||!(next.end38>next.start34)||next.start34<0.f)
        return fogFail(d,FogS50CR::arithmetic,0x13e830,fog.fogBlockOffset);
    out=next;return true;
}

struct OriginalTrackFogFrame50CR {
    bool zoneChanged=false,interpolated=false;
    std::uint32_t previousZone48=0,zone48=0;
    std::size_t regionRecords=0,contributing=0;
    float weight=0.f;
    std::array<float,3> targetRgb{};float targetVisibility=0.f,targetHaziness=0.f;
    std::uint32_t appliedRgb=0;float appliedVisibility=0.f,appliedHaziness=0.f;
    std::uint64_t frame=0;
};

// The ONE track fog owner (common_track50CR::Track::roles.fog, step 05.05 273C30),
// every course. Owns the zone table (World350 fog lanes), the five controllers
// (World4DC..4EC), the Application250 sample and the World3EC fields the track
// load leaves (273C64/13E830), which the camera session (restore 1A98DC,
// display, per-zone 275630) and the renderer borrow. Replaces the GB/MC/RA fog
// owners. prepareFrame275630 writes only the World3EC storage it is handed.
class OriginalTrackFogRuntime50CR final {
    struct Controller {
        float current00=1.0f;
        float period04=track_fog50cr_detail::fogBits(0x3dcccccdu); // 0.1
        float factor08=1.0f;
        float maximum0C=std::numeric_limits<float>::max();
        float minimum10=-std::numeric_limits<float>::max();
    };
    OriginalTrackFog50CR fog_{};
    std::array<Controller,5> controllers_{}; // 4DC vis, 4E0 haz, 4E4 R, 4E8 G, 4EC B
    std::chrono::steady_clock::time_point last250_{};
    std::uint64_t frames_=0;
    std::uint32_t lastZone48_=0;
    OriginalRaceFogFields fields3EC_{};
    bool prepared_=false,controllersBuilt_=false,clockStarted_=false,haveZone_=false;
public:
    OriginalTrackFogRuntime50CR()=default;
    OriginalTrackFogRuntime50CR(const OriginalTrackFogRuntime50CR&)=delete;
    OriginalTrackFogRuntime50CR& operator=(const OriginalTrackFogRuntime50CR&)=delete;

    bool prepare(const assets::World48F& world,OriginalTrackFogDiagnostic50CR& d) {
        if(prepared_)return track_fog50cr_detail::fogFail(d,OriginalTrackFogStatus50CR::state,0x1a9ee0,0);
        OriginalTrackFog50CR next;
        if(!readOriginalTrackFog50CR(world,next,d))return false;
        OriginalRaceFogFields loaded{};
        if(!loadOriginalRaceFogFields50CR(next,loaded,d))return false;
        fog_=std::move(next);fields3EC_=loaded;
        controllersBuilt_=fog_.zoneFogDiffers4F0; //270D1C..270E7C, defaults above
        prepared_=true;return true;
    }
    bool prepared() const noexcept {return prepared_;}
    // World3EC as the track load left it (read-only), and the one mutable
    // binding the camera session's restore/display/per-zone steps write.
    const OriginalRaceFogFields* fields3EC() const noexcept {return prepared_?&fields3EC_:nullptr;}
    OriginalRaceFogFields* cameraFields3EC() noexcept {return prepared_?&fields3EC_:nullptr;}
    const OriginalTrackFog50CR& fog() const noexcept {return fog_;}
    // 27563C..275644: the per-frame zone path runs only when World4F0 bit0 is set.
    bool zoneFogActive() const noexcept {return prepared_&&fog_.zoneFogDiffers4F0&&controllersBuilt_;}
    bool loadFields(OriginalRaceFogFields& out,OriginalTrackFogDiagnostic50CR& d) const noexcept {
        if(!prepared_)return track_fog50cr_detail::fogFail(d,OriginalTrackFogStatus50CR::state,0x273c30,0);
        return loadOriginalRaceFogFields50CR(fog_,out,d);
    }
    // Application250: uncapped seconds between updates; the first is 0 (140D70).
    float sampleElapsed250() noexcept {
        const auto now=std::chrono::steady_clock::now();
        float seconds=0.f;
        if(clockStarted_&&now>last250_)seconds=std::chrono::duration<float>(now-last250_).count();
        last250_=now;clockStarted_=true;
        return std::isfinite(seconds)&&seconds>0.f?seconds:0.f;
    }
    // 275630..275850 for one camera. viewports264 is Application+264.
    bool prepareFrame275630(std::uint32_t viewports264,float elapsed250,std::uint32_t primaryZone48,
        const OriginalZoneFogOverlap50CR* overlaps,std::size_t count,OriginalRaceFogFields& fog3EC,
        OriginalTrackFogFrame50CR& out,OriginalTrackFogDiagnostic50CR& d) noexcept {
        using namespace track_fog50cr_detail;
        d={};
        if(!zoneFogActive()||!viewports264)return fogFail(d,FogS50CR::state,0x27563c,0);
        if(!std::isfinite(elapsed250)||elapsed250<0.f)return fogFail(d,FogS50CR::arithmetic,0x27567c,0);
        if(count>originalTrackFogOverlapLimit50CR||(count&&!overlaps))return fogFail(d,FogS50CR::overlapCapacity,0x275a3c,count);
        const std::size_t zones=fog_.zones350.size();
        if(primaryZone48>=zones)return fogFail(d,FogS50CR::cameraZone,0x275a18,primaryZone48);
        OriginalTrackFogFrame50CR f;f.frame=++frames_;f.zone48=primaryZone48;f.regionRecords=count;
        f.previousZone48=lastZone48_;f.zoneChanged=!haveZone_||lastZone48_!=primaryZone48;
        // 275990
        const float radius=originalZoneFogRadius275990_50CR;
        const auto& primary=fog_.zones350[primaryZone48].fog;          //2759F0..275A48
        float rgb[3]={primary.color00[0],primary.color00[1],primary.color00[2]};
        float visibility=primary.visibility10,haziness=primary.haziness14,weight=1.0f;
        for(std::size_t i=0;i<count;++i){
            const auto& o=overlaps[i];
            if(o.zone48>=zones)return fogFail(d,FogS50CR::cameraZone,0x275a88,o.zone48);
            if(!o.overlaps)continue;                                  //275A64
            if(!std::isfinite(o.planeDistance7C))return fogFail(d,FogS50CR::arithmetic,0x275a74,i);
            const float w=fogDiv(fogSub(radius,o.planeDistance7C),radius);  //275A84..275A8C
            const auto& z=fog_.zones350[o.zone48].fog;
            weight=fogAdd(weight,w);                                     //275AAC
            for(unsigned k=0;k<3;++k)rgb[k]=fogAdd(rgb[k],fogMul(w,z.color00[k])); //275AB0..275ADC
            visibility=fogAdd(visibility,fogMul(w,z.visibility10));        //275AEC/275AF4
            haziness=fogAdd(haziness,fogMul(w,z.haziness14));              //275AF0/275AF8
            ++f.contributing;
        }
        if(!std::isfinite(weight)||!(weight>0.f))return fogFail(d,FogS50CR::arithmetic,0x275b1c,0);
        for(unsigned k=0;k<3;++k)f.targetRgb[k]=fogDiv(rgb[k],weight); //275B20..275B40
        f.targetVisibility=fogDiv(visibility,weight);                   //275B1C
        f.targetHaziness=fogDiv(haziness,weight);                       //275B2C
        f.weight=weight;
        float vis=f.targetVisibility,haz=f.targetHaziness,r=f.targetRgb[0],g=f.targetRgb[1],b=f.targetRgb[2];
        auto staged=controllers_;
        if(viewports264==1u){
            //275678..2757EC: min.s(App250,period), factor=min/period, current+=factor*(target-current)
            const float target[5]={vis,haz,r,g,b};
            for(unsigned i=0;i<5;++i){
                auto& c=staged[i];
                const float m=elapsed250<c.period04?elapsed250:c.period04;
                c.factor08=fogDiv(m,c.period04);
                c.current00=fogAdd(c.current00,fogMul(c.factor08,fogSub(target[i],c.current00)));
                if(!std::isfinite(c.current00))return fogFail(d,FogS50CR::arithmetic,0x2756c0+0x44u*i,i);
            }
            vis=staged[0].current00;haz=staged[1].current00;
            r=staged[2].current00;g=staged[3].current00;b=staged[4].current00;
            f.interpolated=true;
        }
        std::uint32_t packed=0;
        if(!fogPackRgb(r,g,b,packed))return fogFail(d,FogS50CR::range,0x2757a8,0);  //2757A8..275800 / 275824..275850
        auto fog=fog3EC;
        if(!setOriginalRaceFog13E830_50CR(fog,packed,vis,haz))return fogFail(d,FogS50CR::arithmetic,0x13e830,0);
        f.appliedRgb=packed;f.appliedVisibility=vis;f.appliedHaziness=haz;
        controllers_=staged;fog3EC=fog;lastZone48_=primaryZone48;haveZone_=true;out=f;
        return true;
    }
    // One line per course, then one per zone that authors any fog field.
    void logPrepared(std::ostream& o,const OriginalRaceFogFields& loaded,bool worldFogDrawn) const {
        unsigned rgb[3];track_fog50cr_detail::fogChannels(loaded.color30,rgb);
        std::size_t authored=0,differ=0;
        for(const auto& z:fog_.zones350){
            if(z.colorAuthored||z.visibilityAuthored||z.hazinessAuthored)++authored;
            if(z.differs)++differ;
        }
        o<<"track_fog source=scb scb_bytes="<<fog_.scbBytes<<" fog_block="<<fog_.fogBlockOffset
         <<" color="<<rgb[0]<<','<<rgb[1]<<','<<rgb[2]<<(fog_.colorAuthored?"":"(default)")
         <<" visibility="<<fog_.global3AC.visibility10<<(fog_.visibilityAuthored?"":"(default)")
         <<" haziness="<<fog_.global3AC.haziness14<<(fog_.hazinessAuthored?"":"(default)")
         <<" start="<<loaded.start34<<" end="<<loaded.end38
         <<" zones="<<fog_.zones350.size()<<" zone_fog_authored="<<authored<<" zone_fog_differs="<<differ
         <<" zone_fog_active="<<(zoneFogActive()?1:0)<<" world_fog_drawn="<<(worldFogDrawn?1:0)<<'\n';
        for(std::size_t i=0;i<fog_.zones350.size();++i){
            const auto& z=fog_.zones350[i];
            if(!(z.colorAuthored||z.visibilityAuthored||z.hazinessAuthored))continue;
            o<<"track_fog_zone zone="<<i+1<<" color="<<z.fog.color00[0]<<','<<z.fog.color00[1]<<','<<z.fog.color00[2]
             <<" visibility="<<z.fog.visibility10<<" haziness="<<z.fog.haziness14
             <<" authored="<<(z.colorAuthored?"c":"")<<(z.visibilityAuthored?"v":"")<<(z.hazinessAuthored?"h":"")
             <<" differs="<<(z.differs?1:0)<<'\n';
        }
    }
    static void logFrame(std::ostream& o,const OriginalTrackFogFrame50CR& f,const OriginalRaceFogFields& fog) {
        o<<"track_fog_zone_change frame="<<f.frame<<" zone="<<(f.frame==1?0u:f.previousZone48+1u)<<"->"<<f.zone48+1u
         <<" region_records="<<f.regionRecords<<" contributing="<<f.contributing<<" weight="<<f.weight
         <<" target="<<f.targetRgb[0]<<','<<f.targetRgb[1]<<','<<f.targetRgb[2]
         <<'/'<<f.targetVisibility<<'/'<<f.targetHaziness
         <<" applied="<<((f.appliedRgb>>16)&0xffu)<<','<<((f.appliedRgb>>8)&0xffu)<<','<<(f.appliedRgb&0xffu)
         <<'/'<<f.appliedVisibility<<'/'<<f.appliedHaziness
         <<" interpolated="<<(f.interpolated?1:0)<<" start="<<fog.start34<<" end="<<fog.end38<<'\n';
    }
};
} // namespace rr::game
