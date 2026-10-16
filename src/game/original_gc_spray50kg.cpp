#include "original_gc_spray50kg.h"
#include "water_wake50jc.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <utility>

namespace rr::game {
namespace {
using S=OriginalGcSprayStatus50KG;
using D=assets::OriginalUiPageDiagnostic50KG;
using E=assets::OriginalUiPageError50KG;
using Section=assets::OriginalUiDatSection50KG;
using Math=water_wake_detail50jc::Math;
float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
void put(std::uint32_t& u,float f) noexcept {std::memcpy(&u,&f,4);}
bool good(float f) noexcept {return std::isfinite(f)&&std::fpclassify(f)!=FP_SUBNORMAL;}
OriginalGcSprayReceipt50KG receipt(S s,std::uint32_t pc) noexcept {
    OriginalGcSprayReceipt50KG r;r.status=s;r.sourcePC=pc;return r;
}
char lower(char c) noexcept{return c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const char* b) noexcept {
    const auto n=std::strlen(b);if(a.size()!=n)return false;
    for(std::size_t i=0;i<n;++i)if(lower(a[i])!=lower(b[i]))return false;return true;
}
bool space(char c) noexcept{return c==' '||c=='\t'||c=='\r';}
// Bounded source159440 cleaning and158648 first-present lookup. Unknown keys
// and duplicates remain in source order; quoted empty differs from missing.
// This is text authoring only, not a BPB/archive decoder or runtime interpreter.
bool sections(const std::vector<std::uint8_t>& bytes,std::vector<Section>& out,D& d) {
    auto fail=[&](E e,std::size_t at){d={e,at,0x159440u};return false;};
    if(bytes.empty()||bytes.size()>32768)return fail(E::capacity,0);
    auto size=bytes.size();if(bytes.back()==0)--size;
    for(std::size_t i=0;i<size;++i)if((bytes[i]<32&&bytes[i]!=9&&bytes[i]!=10&&bytes[i]!=13)||bytes[i]>126)
        return fail(E::unsupportedEncoding,i);
    for(std::size_t at=0;at<size;){
        const auto start=at;while(at<size&&bytes[at]!='\n')++at;auto end=at;
        if(at<size){++at;if(end>start&&bytes[end-1]=='\r')--end;}
        if(end-start>2048)return fail(E::capacity,start);
        auto first=start;while(first<end&&space(char(bytes[first])))++first;
        if(first==end||bytes[first]==';'||bytes[first]==':')continue;
        // Colon-prefixed EMT annotations cannot match any consumed source key.
        //158724/158800 skips a line without an '=' value; colon key/value
        // annotations also fail the requested-key comparison at15880C.
        const bool header=bytes[first]=='[';bool quoted=false;int brackets=0;
        std::string line;std::vector<std::size_t> offsets;
        for(auto i=start;i<end;++i){const char c=char(bytes[i]);
            if(header&&c==';'&&!quoted&&brackets==0&&!line.empty()&&line.back()==']')break;
            if(c=='"')quoted=!quoted;else if(c=='[')++brackets;else if(c==']')--brackets;
            if(brackets<0||brackets>16)return fail(E::textSyntax,i);
            if(space(c)&&!quoted&&brackets==0)continue;
            line.push_back(c);offsets.push_back(i);
        }
        if(line.empty()||line[0]==';')continue;
        if(brackets)return fail(E::textSyntax,start);
        if(line[0]=='['){
            if(line.size()<3||line.back()!=']')return fail(E::textSyntax,start);
            auto name=line.substr(1,line.size()-2);
            if(name.size()>127||name.find_first_of("[]=\"")!=std::string::npos)return fail(E::textSyntax,start);
            for(const auto& s:out)if(same(s.name,name.c_str()))return fail(E::duplicateSection,start);
            if(out.size()==64)return fail(E::capacity,start);
            out.push_back({std::move(name),offsets[1],{}});continue;
        }
        const auto equal=line.find('=');
        if(equal==std::string::npos)continue; //158800: no value token.
        if(out.empty())continue; //No current requested source block yet.
        if(!equal||equal>127)return fail(E::textSyntax,start);
        auto key=line.substr(0,equal);if(key.find_first_of("[];\"")!=std::string::npos)return fail(E::textSyntax,start);
        auto va=equal+1,vb=line.size();const bool present=va<vb;
        if(va<vb&&line[va]=='"')++va;if(va<vb&&line[vb-1]=='"')--vb;
        if(out.back().values.size()==256)return fail(E::capacity,start);
        const auto valueOffset=va<offsets.size()?offsets[va]:offsets[equal]+1;
        out.back().values.push_back({std::move(key),line.substr(va,vb-va),offsets[0],valueOffset,present});
    }
    return !out.empty()||fail(E::textSyntax,0);
}
const Section& section(const std::vector<Section>& doc,const char* name,const Section& absent) noexcept {
    for(const auto& s:doc)if(same(s.name,name))return s;return absent;
}

// SHA-256 block operation shared in form with original_ui_archive50kg.cpp.
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
bool authentic(const std::vector<std::uint8_t>& b) noexcept {
    if(b.size()!=1027)return false;
    std::array<std::uint32_t,8> h{{0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
        0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u}};
    std::size_t at=0;for(;b.size()-at>=64;at+=64)shaBlock(b.data()+at,h);
    std::array<std::uint8_t,128> tail{};const auto n=b.size()-at;
    std::memcpy(tail.data(),b.data()+at,n);tail[n]=0x80;
    const std::size_t padded=n<56?64:128;const std::uint64_t length=b.size()*8u;
    for(unsigned i=0;i<8;++i)tail[padded-1-i]=std::uint8_t(length>>(i*8u));
    shaBlock(tail.data(),h);if(padded==128)shaBlock(tail.data()+64,h);
    constexpr std::array<std::uint32_t,8> expected{{0x61ed9636u,0xb3b0db17u,
        0xfe60ed9cu,0x2a07d45du,0x962436f4u,0xe217c9c4u,0xf1c58148u,0xc912bf17u}};
    return h==expected;
}
} // namespace
OriginalGcSprayStatus50KG readOriginalGcSprayDefinition50KG(
    const std::vector<std::uint8_t>& input,OriginalGcSprayDefinition50KG& output,D& d) noexcept {
    d={};if(!authentic(input))return S::assetIdentity;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    try{
        std::vector<Section> doc;if(!sections(input,doc,d))return S::authoredData;
        const Section absent{};const auto& general=section(doc,"General Info",absent);
        const auto& properties=section(doc,"Properties",absent);
        const auto& animation=section(doc,"Animation",absent);
        OriginalGcSprayDefinition50KG f;f.groundLevel=std::numeric_limits<float>::max();
        auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){
            return assets::originalUiNumber50KG(s,key,fallback,value,d,pc);};
        auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){
            return assets::originalUiInteger50KG(s,key,fallback,value,d,pc);};
        auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){
            return assets::originalUiTriple50KG(general,key,{},value,d,pc);};
        if(!v("MinRange",f.minRange,0x19da70)||!v("MaxRange",f.maxRange,0x19da8c)||
           !v("InitialPositionOffset",f.positionOffset,0x19daa8)||
           !n(general,"MinStreamHeight",1,f.minStreamHeight,0x19dac4)||
           !n(general,"MaxStreamHeight",3,f.maxStreamHeight,0x19dae0)||
           !n(general,"SizeModifier",.5f,f.sizeModifier,0x19dafc)||
           !n(general,"FillerLifeModifier",bits(0x3ecccccdu),f.fillerLifeModifier,0x19db1c)||
           !n(general,"FillerSizeModifier",bits(0x3f4ccccdu),f.fillerSizeModifier,0x19db3c)||
           !n(general,"FillerVelocityModifier",.5f,f.fillerVelocityModifier,0x19db58)||
           !n(general,"SideVelocityModifier",2.5f,f.sideVelocityModifier,0x19db74)||
           !i(general,"ParticleGroup",-1,f.particleGroup,0x15aee0)||
           !n(general,"ParticleLife",1000,f.particleLife,0x15af70)||
           !n(general,"InitialSize",1,f.initialSize,0x15af8c)||
           !n(general,"Growth",0,f.growth,0x15afa4)||
           !n(general,"EmitRate",500,f.emitRate,0x15afc0)||
           !n(general,"AgeAtBirth",0,f.ageAtBirth,0x15afd8)||
           !v("Velocity",f.velocity,0x15b004)||
           !i(general,"EmitMode",0,f.emitMode,0x15b01c)||
           !n(general,"DelayTime",0,f.delayTime,0x15b03c)||
           !n(general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
           !n(general,"GravityMultiplier",3,f.gravityMultiplier,0x15b080)||
           !n(general,"Restitution",bits(0x3f0ccccdu),f.restitution,0x15b0a0))return S::authoredData;
        Math m;f.ageAtBirth=m.mul(f.ageAtBirth,f.particleLife,0x15aff8);
        if(f.emitMode==1){if(!n(general,"OneShotTime",0,f.maxTimeOn,0x15b0c8))return S::authoredData;}
        else if(f.emitMode==2){
            if(!n(general,"RandomMinTimeOn",0,f.minTimeOn,0x15b0f8)||
               !n(general,"RandomMaxTimeOn",0,f.maxTimeOn,0x15b110)||
               !n(general,"RandomMinTimeOff",0,f.minTimeOff,0x15b128)||
               !n(general,"RandomMinTimeOff",0,f.minTimeOff,0x15b140))return S::authoredData;
            // Retail repeats the SAME key and field at15B138/13C. No source
            // read of RandomMaxTimeOff occurs; constructor1F4 remains zero.
        }else if(f.emitMode==3){
            if(!n(general,"TimedTimeOn",0,f.maxTimeOn,0x15b1a0)||
               !n(general,"TimedTimeOff",0,f.maxTimeOff,0x15b1b8))return S::authoredData;
        }else if(f.emitMode!=0){d.sourcePC=0x15a8ac;return S::unsupported;}
        const char* keys[]={"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
        const std::uint32_t masks[]={2,4,8,0x1000,0x2000,1,0x20,0x800};
        const std::uint32_t pcs[]={0x15b1e8,0x15b200,0x15b218,0x15b230,0x15b248,0x15b260,0x15b278,0x15b290};
        for(unsigned k=0;k<8;++k){bool value=k==7;
            if(!assets::originalUiBoolean50KG(properties,keys[k],k==7,value,d,pcs[k]))return S::authoredData;
            if(k==7)value=!value;if(value)f.particleFlags|=masks[k];
        }
        if(!assets::originalUiBoolean50KG(properties,"RandomMirrorAboutX",false,f.randomMirrorX,d,0x15b2a8)||
           !assets::originalUiBoolean50KG(properties,"RandomMirrorAboutY",false,f.randomMirrorY,d,0x15b2c0)||
           !n(animation,"FramesPerSet",-1,f.framesPerSet,0x15b490))return S::authoredData;
        if(f.groundLevel!=std::numeric_limits<float>::max())f.particleFlags|=0x200;
        // The source stores1/0 for FramesPerSet0, but flag400 is clear. The
        // native unused inverse remains0; active animation requires a divisor.
        f.inverseFramesPerSet=f.framesPerSet==0?0:m.div(1,f.framesPerSet,0x15b4b0);
        if(f.framesPerSet>1)f.particleFlags|=0x400;
        std::int32_t count=0;if(!i(animation,"NumberOfForcedSets",0,count,0x15b4c0))return S::authoredData;
        if(count>256){d.sourcePC=0x15b530;return S::capacity;}
        for(std::int32_t k=0;k<count;++k){std::int32_t set=-1;
            const auto name="ForcedSet"+std::to_string(k+1);
            if(!i(animation,name.c_str(),-1,set,0x15b568))return S::authoredData;
            if(set==std::numeric_limits<std::int32_t>::min()){d.sourcePC=0x15b584;return S::unsupported;}
            f.forcedSets.push_back(set-1);
        }
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        output=std::move(f);return S::complete;
    }catch(...){return S::allocation;}
}
OriginalGcSprayStatus50KG readOriginalGcCommonEmitterTiming50KG(
    const std::vector<std::uint8_t>& input,OriginalGcCommonEmitterTiming50KG& out,D& d) noexcept {
    d={};try{
        std::vector<Section> doc;if(!sections(input,doc,d))return S::authoredData;
        const Section absent{};const auto& general=section(doc,"General Info",absent);
        OriginalGcCommonEmitterTiming50KG next;std::int32_t mode=0;
        auto n=[&](const char* key,float& value,std::uint32_t pc){
            return assets::originalUiNumber50KG(general,key,0,value,d,pc);};
        if(!assets::originalUiInteger50KG(general,"EmitMode",0,mode,d,0x15b01c)||
           !n("DelayTime",next.delayTime1E0,0x15b03c))return S::authoredData;
        if(mode<0||mode>3){d.sourcePC=0x15a8ac;return S::unsupported;}
        next.mode1D8=static_cast<std::uint32_t>(mode);
        if(mode==1){if(!n("OneShotTime",next.maxTimeOn1EC,0x15b0c8))return S::authoredData;}
        else if(mode==2){
            if(!n("RandomMinTimeOn",next.minTimeOn1E8,0x15b0f8)||
               !n("RandomMaxTimeOn",next.maxTimeOn1EC,0x15b110)||
               !n("RandomMinTimeOff",next.minTimeOff1F0,0x15b128)||
               !n("RandomMinTimeOff",next.minTimeOff1F0,0x15b140))return S::authoredData;
        }else if(mode==3){
            if(!n("TimedTimeOn",next.maxTimeOn1EC,0x15b1a0)||
               !n("TimedTimeOff",next.maxTimeOff1F4,0x15b1b8))return S::authoredData;
        }
        out=next;return S::complete;
    }catch(...){return S::allocation;}
}
OriginalGcSprayReceipt50KG OriginalGcSprayOwner50KG::load(
    OriginalGcParticleManager50KG& manager,const std::vector<std::uint8_t>& emt,
    const std::array<float,3>& direction,std::uint32_t& raw) noexcept {
    if(loaded())return receipt(S::alreadyPrepared,0x19cc50);
    if(!manager.prepared())return receipt(S::unprepared,0x19cd74);
    for(float f:direction)if(!good(f))return receipt(S::arithmetic,0x19ce88);
    OriginalGcSprayDefinition50KG f;D d;
    const auto read=readOriginalGcSprayDefinition50KG(emt,f,d);
    if(read!=S::complete)return receipt(read,d.sourcePC?d.sourcePC:0x19ce38);
    const auto* group=manager.group(f.particleGroup);
    if(!group)return receipt(S::authoredData,0x19db98);
    if(!f.forcedSets.empty())return receipt(S::unsupported,0x15b508);
    // Finite active source divisors. No clamping, substitute lifetimes, or
    // replacement particles when an authored value falls outside admission.
    if(f.particleLife<=0||f.initialSize<=0||f.emitRate<0||f.fillerLifeModifier<=0)
        return receipt(S::unsupported,0x19cfa0);
    OriginalGcParticleEmitterState50KG state;
    state.particleGroup=f.particleGroup;state.framesPerSet=f.framesPerSet;
    state.inverseFramesPerSet=f.inverseFramesPerSet;
    state.gravity=f.gravityMultiplier;state.bouncePlane=f.groundLevel;state.bounceFactor=f.restitution;
    // Registration provides the canonical manager's actual owner48 handle.
    const auto id=manager.registerEmitter(state);if(!id)return receipt(S::capacity,0x19cd70);
    definition_=std::move(f);sideDirection_=direction;manager_=&manager;ownerId_=id;
    controls_={};controls_.flags1D4=1;
    controls_.position30[3]=controls_.previous40[3]=controls_.velocity60[3]=0x3f800000u;
    for(unsigned lane=0;lane<3;++lane)put(controls_.velocity60[lane],definition_.velocity[lane]);
    firstFrame_=currentFrame_=group->first;lastFrame_=group->last;
    delay1DC_=definition_.delayTime;put(duration1E4_,definition_.maxTimeOn);carriedSize18C_=0;
    auto r=receipt(S::complete,0x19ceb0);
    if(definition_.emitMode==2){Math m;const auto random=drawOriginalGcParticleRandom50KG(raw);++r.randomDraws;
        const auto span=m.sub(definition_.maxTimeOn,definition_.minTimeOn,0x15b170);
        const auto initial=m.add(definition_.minTimeOn,m.mul(span,random,0x15b174),0x15b178);
        if(m.badPC){r.status=S::arithmetic;r.sourcePC=m.badPC;}else put(duration1E4_,initial);
    }
    return r;
}
namespace {
float root(float x,Math& m,std::uint32_t pc) noexcept {
    m.read(x,pc);if(x<0){if(!m.badPC)m.badPC=pc;return 0;}
    volatile float y=std::sqrt(x);return m.read(y,pc);
}
float squared(const std::array<float,3>& v,Math& m,std::uint32_t pc) noexcept {
    const auto x=m.mul(v[0],v[0],pc),y=m.mul(v[1],v[1],pc+4);
    const auto a=m.add(x,y,pc+8);return m.add(a,m.mul(v[2],v[2],pc+12),pc+12);
}
}
OriginalGcSprayReceipt50KG advanceOriginalGcCommonEmitter50KG(
    EffectControlState50BC& c,const OriginalGcCommonEmitterTiming50KG& timing,
    float& delay1DC,std::uint32_t& durationWord,bool attachment74Present,
    float dt,bool paused,std::uint32_t& raw,
    const OriginalGcCommonEmitterDispatch50KG* calls,
    const OriginalGcCommonAttachment50KG* attachment) noexcept {
    auto r=receipt(S::complete,0x15abf4);
    //15A7B0 pause and15A7D0 first-update gates precede arithmetic/attachment.
    if(paused){r.commonSkipped=true;r.sourcePC=0x15a7c8;return r;}
    if(c.flags1D4&1u){c.flags1D4=std::uint8_t(c.flags1D4&0xfeu);
        std::memcpy(c.previous40,c.position30,16);r.commonSkipped=true;r.sourcePC=0x15a800;return r;}
    if(attachment74Present){
        if(!attachment||!attachment->graph)return receipt(S::unsupported,0x15a810);
        const auto& graph=*attachment->graph;
        const auto node=static_cast<std::int32_t>(attachment->node);
        if(node<0||static_cast<std::uint32_t>(node)>=graph.count||!validateSceneGraph49P(graph))
            return receipt(S::storage,0x28b1a0);
        //15A818 copies all four canonical words before the actual scene query.
        std::memcpy(c.previous40,c.position30,16);
        if(!refreshSceneWorld49P(graph,attachment->node))return receipt(S::arithmetic,0x28b1fcu);
        const auto& world=graph.nodes[node].world100;
        //28B264..278 writes XYZ only. Existing position W is preserved.
        for(unsigned k=0;k<3;++k)put(c.position30[k],world.v[12+k]);
        Math transform;std::array<float,3> rotated{};
        //28BA74..7C: world.X*offset.X, then Y, then Z; no translation or W.
        for(unsigned k=0;k<3;++k){const auto x=transform.mul(world.v[k],attachment->offset50[0],0x28ba74);
            const auto y=transform.mul(world.v[4+k],attachment->offset50[1],0x28ba78);
            const auto xy=transform.add(x,y,0x28ba78);
            const auto z=transform.mul(world.v[8+k],attachment->offset50[2],0x28ba7c);
            rotated[k]=transform.add(xy,z,0x28ba7c);}
        if(transform.badPC)return receipt(S::arithmetic,transform.badPC);
        const std::uint32_t addPC[]={0x15a840,0x15a850,0x15a860};
        for(unsigned k=0;k<3;++k){const auto position=transform.add(bits(c.position30[k]),rotated[k],addPC[k]);
            if(transform.badPC)return receipt(S::arithmetic,transform.badPC);
            put(c.position30[k],position);}
    }
    float duration=bits(durationWord);
    if(std::fegetround()!=FE_TONEAREST)return receipt(S::arithmetic,0x15a7a0);
    Math m;m.read(dt,0x15a7c4);m.read(delay1DC,0x15a868);
    auto failMath=[&](){r.status=S::arithmetic;r.sourcePC=m.badPC;return r;};
    if(m.badPC)return failMath();
    auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
    auto inactive=[&](){
        const auto timer=m.add(bits(c.word19C),dt,0x15aba8);
        if(m.badPC)return failMath();put(c.word19C,timer);
        std::memcpy(c.previous40,c.position30,16);c.flags1D4=std::uint8_t(c.flags1D4&0xfbu);
        r.commonSkipped=true;r.sourcePC=0x15abc8;return r;
    };
    auto skip=[&](std::uint32_t pc){r.commonSkipped=true;r.sourcePC=pc;return r;};
    bool commonModeReturnedZero=false;
    if(delay1DC>0){
        if(c.flags1D4&2u){const auto delay=m.sub(delay1DC,dt,0x15a898);
            if(m.badPC)return failMath();delay1DC=delay;}
        return skip(0x15a8a4);
    }
    if(timing.mode1D8>3)return receipt(S::unsupported,0x15a8ac);
    if(timing.mode1D8==2||timing.mode1D8==3)m.read(duration,0x15a960);
    if(m.badPC)return failMath();
    if(timing.mode1D8==1){
        if(!(c.flags1D4&2u))return inactive();
        m.read(duration,0x15a8d0);if(m.badPC)return failMath();
        if(duration<=0){
            if(!calls||!calls->stop84||!calls->stop84(calls->context,c))return receipt(S::unsupported,0x15a8fc);
            return skip(0x15a904);}
        const auto nextDuration=m.sub(duration,dt,0x15a8e4);
        if(m.badPC)return failMath();duration=nextDuration;put(durationWord,duration);commonModeReturnedZero=true;
    }else if(timing.mode1D8==2||timing.mode1D8==3){
        const bool started=(c.flags1D4&2u)!=0;
        if(duration>0){
            const auto subtractPc=timing.mode1D8==2?(started?0x15a974u:0x15a9f4u):(started?0x15aac4u:0x15ab14u);
            const auto nextDuration=m.sub(duration,dt,subtractPc);
            if(m.badPC)return failMath();duration=nextDuration;put(durationWord,duration);
            if(duration<=0){
                if(timing.mode1D8==2){
                    const float unit=draw();
                    const auto lo=started?timing.minTimeOff1F0:timing.minTimeOn1E8;
                    const auto hi=started?timing.maxTimeOff1F4:timing.maxTimeOn1EC;
                    const auto pc=started?0x15a9b0u:0x15aa30u;
                    const auto span=m.sub(hi,lo,pc);
                    const auto duration2=m.add(lo,m.mul(span,unit,pc+4),pc+8);
                    if(m.badPC)return failMath();duration=duration2;put(durationWord,duration);
                }else {duration=started?timing.maxTimeOff1F4:timing.maxTimeOn1EC;put(durationWord,duration);}
                if(started){
                    if(!calls||!calls->stop84||!calls->stop84(calls->context,c)){
                        r.status=S::unsupported;r.sourcePC=timing.mode1D8==2?0x15a9c8u:0x15aae8u;return r;}
                    return skip(timing.mode1D8==2?0x15a9d0u:0x15aaf0u);}
                if(!calls||!calls->start80||!calls->start80(calls->context,c)){
                    r.status=S::unsupported;r.sourcePC=timing.mode1D8==2?0x15aa48u:0x15ab38u;return r;}
                c.word19C=0;commonModeReturnedZero=true;
            }else if(!started)return inactive();
            else commonModeReturnedZero=true;
        }
    }
    if(!commonModeReturnedZero){
        if(!(c.flags1D4&2u))return inactive();
        c.flags1D4=std::uint8_t(c.flags1D4|4u); //15ABDC..EC; unchanged if already set.
    }
    return r;
}
OriginalGcSprayReceipt50KG OriginalGcSprayOwner50KG::update(float dt,bool paused,
    std::uint32_t& raw,SharedRandomOwner50ID& shared,HostMonotonicClock50ID& clock) noexcept {
    if(!loaded())return receipt(S::unprepared,0x19cf08);
    const auto& f=definition_;
    const OriginalGcCommonEmitterTiming50KG timing{static_cast<std::uint32_t>(f.emitMode),
        f.delayTime,f.minTimeOn,f.maxTimeOn,f.minTimeOff,f.maxTimeOff};
    const OriginalGcCommonEmitterDispatch50KG calls{nullptr,
        [](void*,EffectControlState50BC& s){startEffect50BC(s);return true;},
        [](void*,EffectControlState50BC& s){stopEffect50BC(s);return true;}};
    auto r=advanceOriginalGcCommonEmitter50KG(controls_,timing,delay1DC_,duration1E4_,false,dt,paused,raw,&calls);
    if(!r.okay()||r.commonSkipped)return r;
    r.sourcePC=0x19d9e4;
    auto& c=controls_;Math m;
    auto failMath=[&](){r.status=S::arithmetic;r.sourcePC=m.badPC;return r;};
    auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
    // Common15A7A0 returned0: derived19CED0 now uses the same controls30/40.
    std::array<float,3> delta{},position{},base{},side{};
    for(unsigned k=0;k<3;++k){position[k]=bits(c.position30[k]);
        delta[k]=m.sub(position[k],bits(c.previous40[k]),0x19cf3cu+8u*k);}
    const auto distance=root(squared(delta,m,0x19cf70),m,0x19cf80);
    const auto demand=m.div(m.mul(distance,f.emitRate,0x19cf9c),f.initialSize,0x19cfa0);
    if(m.badPC)return failMath();
    std::int32_t count=0;
    if(demand>1){count=m.cvtWord(demand,0x19cfbc);if(m.badPC)return failMath();c.intensity204=0;}
    else{
        const auto accumulated=m.add(bits(c.intensity204),demand,0x19cfd0);
        if(m.badPC)return failMath();put(c.intensity204,accumulated);
        if(accumulated>1){count=m.cvtWord(accumulated,0x19cfe8);if(m.badPC)return failMath();c.intensity204=0;}
    }
    std::array<float,3> step{};
    // count0 computes1/0 and dead products in the source before testing the
    // loop. No result is stored in an owner or particle. Keep count0 and the
    // accumulator exactly, omit ONLY the unobservable exceptional temporary,
    // and still perform base/side arithmetic reached before the loop test.
    if(count){const auto inverse=m.div(1,m.integer(count,0x19cffc),0x19d018);
        for(unsigned k=0;k<3;++k)step[k]=m.mul(inverse,delta[k],0x19d01cu+4u*k);}
    else r.deadZeroCountReciprocal=true;
    for(unsigned k=0;k<3;++k)base[k]=m.add(position[k],f.positionOffset[k],0x19d058u+4u*k);
    //13E440: vector displacement CROSS loader-supplied side vector.
    side[0]=m.sub(m.mul(delta[1],sideDirection_[2],0x13e454),m.mul(delta[2],sideDirection_[1],0x13e458),0x13e458);
    side[1]=m.sub(m.mul(delta[2],sideDirection_[0],0x13e454),m.mul(delta[0],sideDirection_[2],0x13e458),0x13e458);
    side[2]=m.sub(m.mul(delta[0],sideDirection_[1],0x13e454),m.mul(delta[1],sideDirection_[0],0x13e458),0x13e458);
    const auto square=squared(side,m,0x19d0b0);
    if(square==0)side={};else{
        const auto inverse=m.div(1,root(square,m,0x19d0ec),0x19d0ec);
        for(unsigned k=0;k<3;++k)side[k]=m.mul(side[k],inverse,0x19d0fcu+12u*k);
    }
    for(unsigned k=0;k<3;++k)side[k]=m.mul(side[k],f.sideVelocityModifier,0x19d124u+12u*k);
    if(m.badPC)return failMath();
    auto full=[&](std::uint32_t pc){carriedSize18C_=f.initialSize;c.word19C=0;
        r.poolFull=true;r.sourcePC=pc;return r;};
    auto frame=[&](OriginalGcParticle50KG& p,bool filler)->bool {
        if(!seedOriginalGcParticleRandom50KG(raw,shared,clock)){
            r.status=S::clock;r.sourcePC=filler?0x19d538u:0x19d830u;return false;}
        const auto wide=std::int64_t(lastFrame_)-std::int64_t(firstFrame_)+1;
        if(wide<1||wide>std::numeric_limits<std::int32_t>::max()){
            r.status=S::storage;r.sourcePC=filler?0x19d55cu:0x19d854u;return false;}
        const float span=m.integer(static_cast<std::int32_t>(wide),filler?0x19d568u:0x19d860u);
        const auto unit=draw();const auto selectedOffset=m.cvtWord(m.mul(span,unit,filler?0x19d588u:0x19d880u),filler?0x19d58cu:0x19d884u);
        if(m.badPC){r.status=S::arithmetic;r.sourcePC=m.badPC;return false;}
        const auto wideSelected=std::int64_t(firstFrame_)+selectedOffset;
        if(wideSelected<std::numeric_limits<std::int32_t>::min()||wideSelected>std::numeric_limits<std::int32_t>::max()){
            r.status=S::arithmetic;r.sourcePC=filler?0x19d598u:0x19d890u;return false;}
        const auto selected=static_cast<std::int32_t>(wideSelected);
        if(!checkOriginalGcParticleRandomRange50KG(selected,firstFrame_,lastFrame_,shared)){
            r.status=S::sourceRangeAssertion;r.sourcePC=filler?0x19d5ecu:0x19d8e4u;return false;}
        p.frame=selected;return true;
    };
    for(std::int32_t stream=0;stream<count;++stream){
        // Five pre-capacity draws per stream, three spread, growth, height.
        const float x=draw(),y=draw(),z=draw();
        const float unit[3]={x,y,z};std::array<float,3> spread{},origin{},velocity{};
        const auto index=m.integer(stream,0x19d1d8);
        const auto total=m.integer(count,0x19d1ec);
        const auto fraction=m.div(index,total,0x19d1fc);
        const auto sizeFactor=std::max(bits(0x3e4ccccdu),fraction); //MAX.S19D23C.
        for(unsigned k=0;k<3;++k){
            const auto pc=0x19d1f0u+32u*k;
            const auto span=m.sub(f.maxRange[k],f.minRange[k],pc);
            spread[k]=m.add(f.minRange[k],m.mul(unit[k],span,pc+4),pc+8);
            origin[k]=m.add(base[k],m.mul(index,step[k],0x19d270u+4u*k),0x19d29cu+4u*k);
        }
        if(m.badPC)return failMath();
        const float growthUnit=draw();
        const auto growth=m.mul(f.growth,growthUnit,0x19d2f4);
        const auto size=m.mul(f.initialSize,sizeFactor,0x19d300);
        const auto inverseLife=m.div(1,f.particleLife,0x19d318);
        for(unsigned k=0;k<3;++k){
            const auto sum=m.add(bits(c.velocity60[k]),spread[k],0x19d32cu+4u*k);
            velocity[k]=m.add(sum,side[k],0x19d358u+12u*k);
        }
        std::uint32_t flags=f.particleFlags;
        if(x>.5f&&f.randomMirrorX)flags|=0x4000u;else flags&=~0x4000u;
        if(y>.5f&&f.randomMirrorY)flags|=0x8000u;else flags&=~0x8000u;
        if(m.badPC)return failMath();
        const float heightUnit=draw();
        const auto heightSpan=m.sub(f.maxStreamHeight,f.minStreamHeight,0x19d458);
        const auto height=m.add(f.minStreamHeight,m.mul(heightSpan,heightUnit,0x19d45c),0x19d460);
        const auto streamCount=m.cvtWord(m.add(.5f,height,0x19d464),0x19d468);
        if(m.badPC)return failMath();
        auto* p=manager_->nextParticle();if(!p)return full(0x19d6e8);
        // Filler record19D4A8..19D6E4. Reused particle fields20/24/4C are not
        // reset: source writes only the represented emitter fields below.
        p->ageMilliseconds=0;
        const auto halfway=m.sub(index,.5f,0x19d4a0);
        std::array<float,3> fillerPosition{};
        for(unsigned k=0;k<3;++k)fillerPosition[k]=m.add(base[k],m.mul(halfway,step[k],0x19d4b8u+4u*k),0x19d4e4u+4u*k);
        if(m.badPC)return failMath();p->position=fillerPosition;p->positionW=1;
        if(!frame(*p,true))return r;
        p->growth=growth;if(growth==0)p->flags&=~8u;
        const auto fillerSize=m.add(size,m.mul(f.growth,f.fillerSizeModifier,0x19d664),0x19d668);
        if(m.badPC)return failMath();p->size=fillerSize;p->ownerId=ownerId_;p->initialSize=p->size;
        const auto life=m.mul(f.particleLife,f.fillerLifeModifier,0x19d684);
        if(m.badPC)return failMath();p->lifeMilliseconds=life;
        const auto inverse=m.div(1,life,0x19d690);if(m.badPC)return failMath();p->inverseLife=inverse;
        std::array<float,3> fillerVelocity{};
        for(unsigned k=0;k<3;++k)fillerVelocity[k]=m.mul(f.fillerVelocityModifier,velocity[k],0x19d6a8u+4u*k);
        if(m.badPC)return failMath();p->velocity=fillerVelocity;p->velocityW=1;p->flags=flags;
        // The late flags assignment intentionally overwrites the temporary
        // growth0 bit clear above, exactly as19D6CC/D0 does.
        if(!manager_->commitParticle(ownerId_)){r.status=S::storage;r.sourcePC=0x19d6e4;return r;}++r.spawned;
        for(std::int32_t vertical=0;vertical<streamCount;++vertical){
            const auto ratio=m.div(m.integer(vertical,0x19d70c),m.integer(streamCount,0x19d71c),0x19d724);
            //19D728..40 computes this unused vector before the capacity test.
            for(unsigned k=0;k<3;++k)m.mul(ratio,velocity[k],0x19d728u+8u*k);
            const auto complement=m.sub(1,ratio,0x19d760);
            if(m.badPC)return failMath();p=manager_->nextParticle();if(!p)return full(0x19d9ac);
            p->ageMilliseconds=0;std::array<float,3> streamPosition{};
            for(unsigned k=0;k<3;++k){
                const auto first=m.mul(ratio,velocity[k],0x19d78cu+4u*k);
                const auto second=m.mul(ratio,first,0x19d7acu+4u*k);
                streamPosition[k]=m.add(origin[k],second,0x19d7d8u+4u*k);
            }
            if(m.badPC)return failMath();p->position=streamPosition;p->positionW=1;
            if(!frame(*p,false))return r;
            const auto streamGrowth=m.add(growth,m.mul(growth,ratio,0x19d924),0x19d928);
            if(m.badPC)return failMath();p->growth=streamGrowth;if(streamGrowth==0)p->flags&=~8u;
            const auto sizePart=m.mul(growth,complement,0x19d95c);
            const auto modified=m.mul(f.sizeModifier,sizePart,0x19d960);
            const auto streamSize=m.add(size,modified,0x19d964);
            if(m.badPC)return failMath();p->size=streamSize;p->ownerId=ownerId_;p->initialSize=p->size;
            p->lifeMilliseconds=f.particleLife;p->inverseLife=inverseLife;
            p->velocity=velocity;p->velocityW=1;p->flags=flags;
            if(!manager_->commitParticle(ownerId_)){r.status=S::storage;r.sourcePC=0x19d9a8;return r;}++r.spawned;
        }
    }
    return r;
}
} // namespace rr::game
