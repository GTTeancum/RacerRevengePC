#pragma once
// One common texture-scroll (uvs_animate) controller for every material pass
// of every course. Replaces the six SHA-pinned per-course owners
// (MA/MB/MC/TB/TC shield, SC lava) with the original's own data rule.
//
// Original (SLUS_202.68), every address read in the disassembly:
//  29A310  VertexAnimationData ctor (vtable 3D3750). Only a pass whose
//          VertexAnimation leaf exists reaches it, so uvs_animate is read
//          ONLY when VertexAnimation=unique. 29A354 158B30("uvs_animate",0);
//          true -> 2E8D90(0x70) -> 29A490 into +08. Otherwise +08 stays NULL
//          and the authored u/v keys are never read (SA tred02, TC tcsun).
//  29A490  UV data ctor (0x70 bytes): +00..+2C step rows, +30..+5C inverse
//          rows, +60 counter, +64 uv_frames, +68 flags (bit0 ReverseLoopOnEnd,
//          bit1 Looping, bit2 running backwards). Keys and defaults:
//          u_center 0, v_center 0, uv_angular_rate 0 (only when authored:
//          311038/301F50/302580 -> cos/sin), u_scale 1, v_scale 1, u_shear 0,
//          v_shear 0, u_velocity 0, v_velocity 0 (158B80 numbers: 302E10
//          atof -> 310440 double->float), uv_frames 1, uv_Looping 0,
//          uv_ReverseLoopOnEnd 0 (158B30). Step translation row:
//          +20 = basis term - u_velocity (29A68C..6B0), +24 = ... - v_velocity.
//  29AB30  animatesWhilePaused (158B30, default 0) into the outer leaf +18.
//  29AC90  per service: app+230 == leaf+08 -> return (same tick); app+11C
//          (paused) && !leaf+18 -> return without stamping; optional callback
//          (+00/+04); vertex leaf vfunc -> 246CC0 -> 29A910 when +08 != NULL;
//          29AD48..58 stamps leaf+08 = app+230 even after a false callback.
//  29A910  counter +60 == 0 -> write identity into the material's three
//          TexGen rows. Else rows = rows x (bit2 ? inverse : step), then each
//          translation component with |t| > 5 (strict, 29A9DC/29AA20) loses
//          its truncated integer part (cvt.w.s). w lanes cleared. -> 29A840.
//  29A840  counter: bit2 ? --c : ++c. If Looping: if Reverse: c <= 0 clears
//          bit2; c >= frames sets bit2 and c = frames - 2. Else c >= frames
//          -> c = 0. Without Looping the counter only counts.
//
// Units: texture-coordinate units added per serviced application tick (one
// step per distinct app+230 value at which the pass is drawn), not per
// second. A pass that is not drawn is not serviced and does not catch up.
//
// Native admission (bounds, not claims about the original):
//  * identity basis only (uv_angular_rate 0, u/v_scale 1, u/v_shear 0): the
//    renderer's existing UV boundary passes a translation only. No stock
//    material authors anything else. Others are refused, never approximated.
//  * numbers: full decimal text -> binary64 (std::from_chars, correctly
//    rounded like atof) -> binary32 round-to-nearest (310440). Trailing junk
//    that atof would ignore is refused.
//  * velocities must be finite with |v| <= 5 so one step cannot jump a wrap.
//  * uv_frames 1..65535.
#include <cfenv>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>

namespace rr::game {

struct OriginalMaterialUvRows50UV {
    float rows[3][4]{{1.f,0.f,0.f,0.f},{0.f,1.f,0.f,0.f},{0.f,0.f,1.f,0.f}};
};
// One authored pass field (key and value text as the PSM reader stored them).
struct OriginalMaterialUvField50UV { std::string_view name, value; };

enum class OriginalMaterialUvStatus50UV : std::uint8_t {
    ok, noVertexLeaf, uvsNotAnimated, syntax, duplicate, unsupportedBasis, range
};
struct OriginalMaterialUvDiagnostic50UV {
    OriginalMaterialUvStatus50UV status=OriginalMaterialUvStatus50UV::ok;
    std::uint32_t sourcePC=0;
    const char* field=nullptr;
};

struct OriginalMaterialUvConfig50UV {
    float uCenter=0.f,vCenter=0.f,angularRate=0.f,uScale=1.f,vScale=1.f,uShear=0.f,vShear=0.f;
    float uVelocity=0.f,vVelocity=0.f;
    std::int32_t frames=1;
    bool looping=false,reverseLoopOnEnd=false,animatesWhilePaused=false;
};

// Controller state: the material pass's three TexGen rows (29A910 writes the
// pass rows themselves), the 0x70-byte UV data words used, and the outer
// leaf timestamp. One per material pass (29B1D0 prototype); model clones made
// by 29B050 registration copy everything except the timestamp (29AB50).
struct OriginalMaterialUv50UV {
    OriginalMaterialUvRows50UV matrix{};
    float stepU=0.f,stepV=0.f;       // +20/+24: 0 - velocity (identity basis)
    std::int32_t counter60=0,frames64=1;
    std::uint8_t flags68=0;           // bit0 reverse, bit1 looping, bit2 backwards
    bool animatesWhilePaused18=false;
    std::uint32_t controllerTimestamp08=0xffffffffu;
};

namespace material_uv50uv_detail {
inline char fold(char c) noexcept {return c>='A'&&c<='Z'?char(c-'A'+'a'):c;}
inline bool same(std::string_view a,std::string_view b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;
    return true;
}
inline std::string_view trim(std::string_view s) noexcept {
    while(!s.empty()&&(s.front()==' '||s.front()=='\t'||s.front()=='\r'))s.remove_prefix(1);
    while(!s.empty()&&(s.back()==' '||s.back()=='\t'||s.back()=='\r'||s.back()==';'))s.remove_suffix(1);
    return s;
}
inline bool fail(OriginalMaterialUvDiagnostic50UV& d,OriginalMaterialUvStatus50UV s,
    std::uint32_t pc,const char* field) noexcept {d.status=s;d.sourcePC=pc;d.field=field;return false;}
// Finds at most one authored field. Duplicates are refused (the PSM reader
// keeps the first; a converted track must not rely on that).
template<class Fields>
inline bool find(const Fields& fields,const char* key,std::string_view& out,bool& present,
    OriginalMaterialUvDiagnostic50UV& d,std::uint32_t pc) {
    present=false;
    for(const OriginalMaterialUvField50UV& f:fields)if(same(trim(f.name),key)){
        if(present)return fail(d,OriginalMaterialUvStatus50UV::duplicate,pc,key);
        out=trim(f.value);present=true;
    }
    return true;
}
// 158B80: atof (302E10) then double->float (310440).
inline bool number(std::string_view text,float& out) noexcept {
    if(text.empty()||text.size()>64)return false;
    if(text.front()=='+')text.remove_prefix(1);
    double value=0;
    const auto r=std::from_chars(text.data(),text.data()+text.size(),value,std::chars_format::general);
    if(r.ec!=std::errc{}||r.ptr!=text.data()+text.size()||!std::isfinite(value))return false;
    if(std::fabs(value)>double(std::numeric_limits<float>::max()))return false;
    volatile float narrowed=static_cast<float>(value);
    out=narrowed;return std::isfinite(out);
}
// 158B30 -> 158920: T/F, Y/N first letter, ON/OFF, else base-0 integer.
inline bool integer(std::string_view text,std::int32_t& out) noexcept {
    if(text.empty()||text.size()>16)return false;
    bool negative=false;
    if(text.front()=='-'||text.front()=='+'){negative=text.front()=='-';text.remove_prefix(1);}
    int base=10;
    if(text.size()>2&&text[0]=='0'&&(text[1]=='x'||text[1]=='X')){base=16;text.remove_prefix(2);}
    else if(text.size()>1&&text[0]=='0'){base=8;text.remove_prefix(1);}
    std::int64_t v=0;
    const auto r=std::from_chars(text.data(),text.data()+text.size(),v,base);
    if(r.ec!=std::errc{}||r.ptr!=text.data()+text.size())return false;
    if(negative)v=-v;
    if(v<std::numeric_limits<std::int32_t>::min()||v>std::numeric_limits<std::int32_t>::max())return false;
    out=static_cast<std::int32_t>(v);return true;
}
inline bool boolean(std::string_view text,bool& out) noexcept {
    if(text.empty())return false;
    const char c=fold(text.front());
    if(c=='t'||c=='y'){out=true;return true;}
    if(c=='f'||c=='n'){out=false;return true;}
    if(same(text,"on")){out=true;return true;}
    if(same(text,"off")){out=false;return true;}
    std::int32_t n=0;if(!integer(text,n))return false;
    out=n!=0;return true;
}
template<class Fields>
inline bool readNumber(const Fields& f,const char* key,float fallback,float& out,
    OriginalMaterialUvDiagnostic50UV& d,std::uint32_t pc) {
    std::string_view v;bool present=false;
    if(!find(f,key,v,present,d,pc))return false;
    if(!present){out=fallback;return true;}
    if(!number(v,out))return fail(d,OriginalMaterialUvStatus50UV::syntax,pc,key);
    return true;
}
template<class Fields>
inline bool readBoolean(const Fields& f,const char* key,bool fallback,bool& out,
    OriginalMaterialUvDiagnostic50UV& d,std::uint32_t pc) {
    std::string_view v;bool present=false;
    if(!find(f,key,v,present,d,pc))return false;
    if(!present){out=fallback;return true;}
    if(!boolean(v,out))return fail(d,OriginalMaterialUvStatus50UV::syntax,pc,key);
    return true;
}
template<class Fields>
inline bool readInteger(const Fields& f,const char* key,std::int32_t fallback,std::int32_t& out,
    OriginalMaterialUvDiagnostic50UV& d,std::uint32_t pc) {
    std::string_view v;bool present=false;
    if(!find(f,key,v,present,d,pc))return false;
    if(!present){out=fallback;return true;}
    bool b=false;
    if(integer(v,out))return true;
    if(boolean(v,b)){out=b?1:0;return true;} // 158B30 accepts T/F for every int key
    return fail(d,OriginalMaterialUvStatus50UV::syntax,pc,key);
}
// 29A9DC..29AA60: strict |t| > 5, cvt.w.s truncation, one subtract.
inline float wrap(float t) noexcept {
    if(!(std::fabs(t)>5.f))return t;
    volatile float integral=static_cast<float>(static_cast<std::int32_t>(t));
    volatile float reduced=t-integral;
    return reduced;
}
inline bool valid(const OriginalMaterialUv50UV& s) noexcept {
    const auto& m=s.matrix.rows;
    return m[0][0]==1.f&&m[0][1]==0.f&&m[0][2]==0.f&&m[0][3]==0.f&&
        m[1][0]==0.f&&m[1][1]==1.f&&m[1][2]==0.f&&m[1][3]==0.f&&
        std::isfinite(m[2][0])&&std::fabs(m[2][0])<=10.f&&
        std::isfinite(m[2][1])&&std::fabs(m[2][1])<=10.f&&
        m[2][2]==1.f&&m[2][3]==0.f&&std::isfinite(s.stepU)&&std::isfinite(s.stepV)&&
        std::fabs(s.stepU)<=5.f&&std::fabs(s.stepV)<=5.f&&
        s.frames64>=1&&s.frames64<=65535&&(s.flags68&~7u)==0;
}
} // namespace material_uv50uv_detail

// Reads one pass. Returns true with status ok when a controller exists,
// true with noVertexLeaf / uvsNotAnimated when the original builds none
// (not an error: the keys are simply never read), false on refusal.
template<class Fields>
inline bool readOriginalMaterialUv50UV(const Fields& fields,OriginalMaterialUvConfig50UV& out,
    OriginalMaterialUvDiagnostic50UV& d) {
    using namespace material_uv50uv_detail;
    d={};
    std::string_view leaf;bool present=false;
    if(!find(fields,"VertexAnimation",leaf,present,d,0x29B890))return false;
    if(!present||same(leaf,"none")){d.status=OriginalMaterialUvStatus50UV::noVertexLeaf;return true;}
    if(!same(leaf,"unique"))return fail(d,OriginalMaterialUvStatus50UV::syntax,0x29B890,"VertexAnimation");
    bool animate=false;
    if(!readBoolean(fields,"uvs_animate",false,animate,d,0x29A354))return false;
    OriginalMaterialUvConfig50UV c;
    if(!readBoolean(fields,"animatesWhilePaused",false,c.animatesWhilePaused,d,0x29AB30))return false;
    if(!animate){out=c;d.status=OriginalMaterialUvStatus50UV::uvsNotAnimated;return true;}
    if(!readNumber(fields,"u_center",0.f,c.uCenter,d,0x29A508)||
       !readNumber(fields,"v_center",0.f,c.vCenter,d,0x29A524)||
       !readNumber(fields,"uv_angular_rate",0.f,c.angularRate,d,0x29A53C)||
       !readNumber(fields,"u_scale",1.f,c.uScale,d,0x29A598)||
       !readNumber(fields,"v_scale",1.f,c.vScale,d,0x29A5B4)||
       !readNumber(fields,"u_shear",0.f,c.uShear,d,0x29A5CC)||
       !readNumber(fields,"v_shear",0.f,c.vShear,d,0x29A5E4)||
       !readNumber(fields,"u_velocity",0.f,c.uVelocity,d,0x29A684)||
       !readNumber(fields,"v_velocity",0.f,c.vVelocity,d,0x29A6AC)||
       !readInteger(fields,"uv_frames",1,c.frames,d,0x29A6D4)||
       !readBoolean(fields,"uv_Looping",false,c.looping,d,0x29A6EC)||
       !readBoolean(fields,"uv_ReverseLoopOnEnd",false,c.reverseLoopOnEnd,d,0x29A724))return false;
    // 29A544: the angle only enters when authored; an authored 0 is cos 1 / sin 0.
    if(c.angularRate!=0.f||c.uScale!=1.f||c.vScale!=1.f||c.uShear!=0.f||c.vShear!=0.f)
        return fail(d,OriginalMaterialUvStatus50UV::unsupportedBasis,0x29A490,"identity UV basis");
    if(!std::isfinite(c.uVelocity)||!std::isfinite(c.vVelocity)||
       std::fabs(c.uVelocity)>5.f||std::fabs(c.vVelocity)>5.f)
        return fail(d,OriginalMaterialUvStatus50UV::range,0x29A684,"u_velocity/v_velocity");
    if(c.frames<1||c.frames>65535)return fail(d,OriginalMaterialUvStatus50UV::range,0x29A6D4,"uv_frames");
    out=c;d.status=OriginalMaterialUvStatus50UV::ok;return true;
}

// 29A490 for the admitted identity basis. Rows start as the pass's authored
// TexGen rows in the original; the first service overwrites them with
// identity (29A918), so starting from identity is visibly the same.
inline OriginalMaterialUv50UV makeOriginalMaterialUv50UV(const OriginalMaterialUvConfig50UV& c) noexcept {
    OriginalMaterialUv50UV s;
    volatile float u=0.f-c.uVelocity,v=0.f-c.vVelocity; // 29A68C..29A6D8
    s.stepU=u;s.stepV=v;s.frames64=c.frames;
    s.flags68=static_cast<std::uint8_t>((c.reverseLoopOnEnd?1u:0u)|(c.looping?2u:0u)); // bit2 clear
    s.animatesWhilePaused18=c.animatesWhilePaused;
    return s;
}
// 29AB50: clone keeps rows, counter and flags; resets only the timestamp.
inline OriginalMaterialUv50UV cloneOriginalMaterialUv50UV(const OriginalMaterialUv50UV& p) noexcept {
    auto c=p;c.controllerTimestamp08=0xffffffffu;return c;
}

// 29AC90 -> 246CC0 -> 29A910 -> 29A840. Transactional: state changes only on
// success. Returns false only for an invalid state or rounding mode.
inline bool serviceOriginalMaterialUv50UV(OriginalMaterialUv50UV& state,
    std::uint32_t applicationTimestamp230,bool applicationPaused11C,
    bool callbackAllowsUpdate=true) noexcept {
    using namespace material_uv50uv_detail;
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559);
    if(std::fegetround()!=FE_TONEAREST||!valid(state))return false;
    if(applicationTimestamp230==state.controllerTimestamp08)return true;   //29ACB0
    if(applicationPaused11C&&!state.animatesWhilePaused18)return true;      //29ACB8..29ACC8
    auto next=state;
    if(callbackAllowsUpdate){
        if(next.counter60==0)next.matrix=OriginalMaterialUvRows50UV{};     //29A918..29A940
        else{
            // Identity basis: rows x step adds the step translation once
            // (inverse step at +50/+54 is the negated translation, 29A7C8..DC).
            const bool backwards=(next.flags68&4u)!=0;
            const float du=backwards?-next.stepU:next.stepU;
            const float dv=backwards?-next.stepV:next.stepV;
            volatile float tu=next.matrix.rows[2][0]+du;
            volatile float tv=next.matrix.rows[2][1]+dv;
            next.matrix.rows[2][0]=wrap(tu);
            next.matrix.rows[2][1]=wrap(tv);
        }
        // 29A840
        next.counter60+= (next.flags68&4u)?-1:1;
        if(next.flags68&2u){
            if(next.flags68&1u){
                if(next.counter60<=0)next.flags68=static_cast<std::uint8_t>(next.flags68&~4u);
                else if(next.counter60>=next.frames64){
                    next.flags68=static_cast<std::uint8_t>(next.flags68|4u);
                    next.counter60=next.frames64-2;
                }
            }else if(next.counter60>=next.frames64)next.counter60=0;
        }else if(next.counter60==std::numeric_limits<std::int32_t>::max()){
            // Without Looping the original word counts forever (addiu wraps
            // after ~2^31 ticks, ~1.1 years at 60 Hz). Hold instead of
            // overflowing a signed int; flagged native bound.
            next.counter60=std::numeric_limits<std::int32_t>::max()-1;
        }
    }
    next.controllerTimestamp08=applicationTimestamp230;                      //29AD48..58
    if(!valid(next))return false;
    state=next;return true;
}

} // namespace rr::game
