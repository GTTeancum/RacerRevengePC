#include "original_gc_pod_dust50kg.h"
#include "original_smoke_numeric50rc.h"
#include "water_wake50jc.h"
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
OriginalGcSprayReceipt50KG receipt(S status,std::uint32_t pc) noexcept {
    OriginalGcSprayReceipt50KG r;r.status=status;r.sourcePC=pc;return r;
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
bool hashMatches(const std::uint8_t* data, std::size_t size,
    const char* expected) noexcept {
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; size - at >= 64; at += 64) {
        shaBlock(data + at, h);
    }
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = size - at;
    if (remainder) std::memcpy(tail.data(), data + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(size) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u])
            return false;
    return true;
}
// Identity is checked before interpreting even a single authoring field.
// Text tokenizer above also accepts the original colon-prefixed annotations;
// the stricter scene PRT reader cannot consume those authentic EMT lines.
bool authentic(const std::vector<std::uint8_t>& b,OriginalGcPodDustMember50KG member) noexcept {
    constexpr std::size_t sizes[]={3237,3236,2906,2908,3232,3237};
    constexpr const char* hashes[]={
        "0e18ac3a94b8d4abaec38f9288a96b39b800ac58b17ba499ffc33f247995bb66",
        "9e5b26cf362e261239a0c6f7ff499f4dea285862286aa8dae65611529e2b5f7f",
        "03f835faf0501dd2a0761f5d1e1996d44bb634fa636242ff94341beaec7008c0",
        "2e89d1d1c6d44804dc25687d837699ab3207e8a800ee3756875e5760226957e2",
        "02ae885a7fa05b2d1370c4d0148bfbc51614a340ea04abe3098812a89ab33d9b",
        "f559ef3678be818686c576ffc58eb7bbae7f30dc07f271bfe264b2960ccda914"};
    const auto i=static_cast<unsigned>(member);
    return i<6&&b.size()==sizes[i]&&hashMatches(b.data(),b.size(),hashes[i]);
}
} // namespace
OriginalGcSprayStatus50KG readOriginalGcPodDustDefinition50KG(
    const std::vector<std::uint8_t>& input,OriginalGcPodDustMember50KG member,
    OriginalGcPodDustDefinition50KG& output,D& d) noexcept {
    OriginalGcPodDustSourceMetadata50KG ignored;
    return readOriginalGcPodDustSourceDefinition50KG(input,member,output,ignored,d);
}
OriginalGcSprayStatus50KG readOriginalGcPodDustSourceDefinition50KG(
    const std::vector<std::uint8_t>& input,OriginalGcPodDustMember50KG member,
    OriginalGcPodDustDefinition50KG& output,OriginalGcPodDustSourceMetadata50KG& metadata,D& d) noexcept {
    d={};if(!authentic(input,member))return S::assetIdentity;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    try {
        std::vector<Section> doc;if(!sections(input,doc,d))return S::authoredData;
        const Section absent{};
        const auto& general=section(doc,"General Info",absent);
        const auto& properties=section(doc,"Properties",absent);
        const auto& animation=section(doc,"Animation",absent);
        OriginalGcPodDustDefinition50KG f;f.member=member;
        auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){
            return assets::originalUiNumber50KG(s,key,fallback,value,d,pc);};
        auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){
            return assets::originalUiInteger50KG(s,key,fallback,value,d,pc);};
        auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){
            return assets::originalUiTriple50KG(general,key,{},value,d,pc);};
        //19B07C calls the common authoring reader before any derived field.
        if(!i(general,"ParticleGroup",-1,f.particleGroup1B8,0x15aee0)||
           !n(general,"ParticleLife",1000,f.particleLife17C,0x15af70)||
           !n(general,"InitialSize",1,f.initialSize184,0x15af8c)||
           !n(general,"Growth",0,f.growth180,0x15afa4)||
           !n(general,"EmitRate",500,f.emitRate188,0x15afc0)||
           !n(general,"AgeAtBirth",0,f.ageAtBirth1A0,0x15afd8)||
           !v("Velocity",f.velocity60,0x15b004)||
           !n(general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
           !n(general,"GravityMultiplier",3,f.gravityMultiplier,0x15b080)||
           !n(general,"Restitution",bits(0x3f0ccccdu),f.restitution,0x15b0a0))return S::authoredData;
        const auto timing=readOriginalGcCommonEmitterTiming50KG(input,f.timing,d);
        if(timing!=S::complete)return timing;
        OriginalGcPodDustSourceMetadata50KG source;
        source.rawAgeAtBirth=f.ageAtBirth1A0;
        auto present=[&](const char* key){for(const auto& value:general.values)
            if(same(value.key,key))return value.sourceValuePresent;return false;};
        source.velocityPresent=present("Velocity");
        source.minimumNoisePresent=present("MinimumVelocityNoise");
        source.maximumNoisePresent=present("MaximumVelocityNoise");
        Math m;f.ageAtBirth1A0=m.mul(f.ageAtBirth1A0,f.particleLife17C,0x15aff8);
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        const char* keys[]={"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
        const std::uint32_t masks[]={2,4,8,0x1000,0x2000,1,0x20,0x800};
        const std::uint32_t pcs[]={0x15b1e8,0x15b200,0x15b218,0x15b230,0x15b248,0x15b260,0x15b278,0x15b290};
        for(unsigned k=0;k<8;++k){bool value=k==7;
            if(!assets::originalUiBoolean50KG(properties,keys[k],k==7,value,d,pcs[k]))return S::authoredData;
            if(k==7)value=!value;if(value)f.particleFlags198|=masks[k];
        }
        if(!assets::originalUiBoolean50KG(properties,"RandomMirrorAboutX",false,f.randomMirrorX,d,0x15b2a8)||
           !assets::originalUiBoolean50KG(properties,"RandomMirrorAboutY",false,f.randomMirrorY,d,0x15b2c0)||
           !n(animation,"FramesPerSet",-1,f.framesPerSet,0x15b490))return S::authoredData;
        if(f.groundLevel!=std::numeric_limits<float>::max())f.particleFlags198|=0x200u;
        // The unused1/0 at15B4B0 has no consumer for the actual FramesPerSet0
        // members. Its flag400 is clear. Preserve that inactive admission.
        f.inverseFramesPerSet=f.framesPerSet==0?0:m.div(1,f.framesPerSet,0x15b4b0);
        if(f.framesPerSet>1)f.particleFlags198|=0x400u;
        std::int32_t forced=0;
        if(!i(animation,"NumberOfForcedSets",0,forced,0x15b4c0))return S::authoredData;
        source.forcedCount=forced;
        if(forced>0){d.sourcePC=0x15b508;return S::unsupported;}
        // Actual19B060 read order and defaults, rather than the EMT comments.
        if(!n(general,"MinimumPercentLife",.5f,f.minimumPercentLife21C,0x19b0a8)||
           !n(general,"MinimumPercentLifeConvergance",1,f.minimumPercentLifeConvergance220,0x19b168)||
           !n(general,"MinimumInitialSize",0,f.minimumInitialSize208,0x19b208)||
           !n(general,"MaximumInitialSize",5,f.maximumInitialSize20C,0x19b2ac)||
           !n(general,"InitialSizeToSpeedRatio",bits(0x3c23d70au),f.initialSizeToSpeedRatio210,0x19b354)||
           !n(general,"MinimumLife",500,f.minimumLife214,0x19b3f8)||
           !n(general,"MaximumLife",3500,f.maximumLife218,0x19b4a0)||
           !n(general,"LifeToSpeedRatio",5,f.lifeToSpeedRatio224,0x19b544)||
           !n(general,"MinimumEmitRate",0,f.minimumEmitRate22C,0x19b5e4)||
           !n(general,"MaximumEmitRate",10,f.maximumEmitRate230,0x19b688)||
           !n(general,"ParticlesToDistanceRatio",bits(0x3c23d70au),f.particlesToDistanceRatio234,0x19b730)||
           !n(general,"ParticlesToSpeedRatio",0,f.particlesToSpeedRatio238,0x19b7d0)||
           !n(general,"InitialPercentAge",0,f.initialPercentAge228,0x19b870)||
           !n(general,"MinimumEmissionSpeed",0,f.minimumEmissionSpeed23C,0x19b910)||
           !v("MinimumVelocityNoise",f.minimumVelocityNoise240,0x19b9b4)||
           !v("MaximumVelocityNoise",f.maximumVelocityNoise250,0x19b9d0))return S::authoredData;
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        // All six authenticated source members are in these asserted domains.
        // Keep bad authored values visible rather than silently clamp them.
        if(f.particleGroup1B8<0||f.minimumPercentLife21C<0||f.minimumPercentLife21C>1||
           f.minimumPercentLifeConvergance220<0||f.minimumInitialSize208<0||f.maximumInitialSize20C<0||
           f.initialSizeToSpeedRatio210<0||f.minimumLife214<0||f.maximumLife218<0||
           f.lifeToSpeedRatio224<0||f.minimumEmitRate22C<0||f.maximumEmitRate230<0||
           f.particlesToDistanceRatio234<0||f.particlesToSpeedRatio238<0||f.initialPercentAge228<0||
           f.minimumEmissionSpeed23C<0){d.sourcePC=0x19b128;return S::authoredData;}
        f.parsed=true;output=f;metadata=source;return S::complete;
    }catch(...){return S::allocation;}
}
OriginalGcSprayReceipt50KG bindOriginalGcPodDust50KG(
    OriginalGcPodDustRuntime50KG& r,const OriginalGcPodDustDefinition50KG& f,
    OriginalGcParticleManager50KG& manager,const EffectOwner50BE& owner) noexcept {
    if(r.bound)return receipt(S::alreadyPrepared,0x19a720);
    if(!f.parsed||!manager.prepared())return receipt(S::unprepared,0x19a744);
    if(!owner.id||owner.profile!=EffectProfile50BE::Particle19A5E0||
       owner.mode1D8!=f.timing.mode1D8||owner.mode1D8>1||
       (owner.state.flags1D4&7u)!=1u||owner.state.intensity204!=0x3f800000u)
        return receipt(S::unsupported,0x19a618);
    // Bind only a genuine fresh50BJ control owner, already loaded from this EMT.
    for(unsigned k=0;k<3;++k)if(bits(owner.state.velocity60[k])!=f.velocity60[k])
        return receipt(S::unsupported,0x15b004);
    const auto initial=owner.mode1D8==1?f.timing.maxTimeOn1EC:0.f;
    if(bits(owner.duration1E4)!=initial)return receipt(S::unsupported,0x15b0d8);
    const auto* group=manager.group(f.particleGroup1B8);
    if(!group)return receipt(S::unsupported,0x19b9e8);
    OriginalGcParticleEmitterState50KG state;
    state.particleGroup=f.particleGroup1B8;state.framesPerSet=f.framesPerSet;
    state.inverseFramesPerSet=f.inverseFramesPerSet;
    state.gravity=f.gravityMultiplier;state.bouncePlane=f.groundLevel;state.bounceFactor=f.restitution;
    const auto handle=manager.registerEmitter(state);
    if(!handle)return receipt(S::capacity,0x19a790);
    r.remainder200=0;r.currentFrame190=r.firstFrame1B0=group->first;r.lastFrame1B4=group->last;
    r.managerOwnerId=handle;r.canonicalOwnerId=owner.id;r.member=f.member;
    r.managerIdentity=&manager;r.bound=true;r.failed=false;
    return receipt(S::complete,0x19ba28);
}
OriginalGcSprayReceipt50KG advanceOriginalGcPodDustAfterCommon50KG(
    OriginalGcPodDustRuntime50KG& runtime,const OriginalGcPodDustDefinition50KG& f,
    OriginalGcParticleManager50KG& manager,EffectOwner50BE& owner,float dt,
    std::uint32_t& raw,SharedRandomOwner50ID& shared,HostMonotonicClock50ID& clock) noexcept {
    if(runtime.failed)return runtime.stoppedAt;
    if(!runtime.bound||!f.parsed||!manager.prepared())return receipt(S::unprepared,0x19a870);
    if(runtime.managerIdentity!=&manager||owner.id!=runtime.canonicalOwnerId||
       owner.profile!=EffectProfile50BE::Particle19A5E0||f.member!=runtime.member||
       owner.mode1D8!=f.timing.mode1D8||!manager.emitter(runtime.managerOwnerId))
        return receipt(S::unsupported,0x19a870);
    return original_smoke_numeric_detail50RC::emit<0x19a870u>(runtime,f,manager,owner.state,dt,raw,shared,clock);
}

} // namespace rr::game
