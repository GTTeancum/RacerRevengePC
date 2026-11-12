#pragma once
// PC-native wreck particle effects (docs/pod-destruction-20261108.md).
//
// Data is the original's: the pod archive's particle\*.emt emitter files and
// the selected track's particle\demoprt.prt atlas (groups/frames) and .pst
// sprite sheet. Original wiring (SLUS_202.68):
//  * racer814 = podExplWreckExplosion.emt (15E800 explosion class) and
//    racer810 = podExplWreckSparks.emt (15F100 sparks class), constructed at
//    212B5C..212C40, started at the body position by 2261B0;
//  * racer818[0..9] = podEmberFire.emt / podEmberSmoke.emt damage emitters,
//    those above 0.4 started at the explosion point racer270;
//  * each wreck group's C0 podWreckSparks / C4 podExplosion restart when a
//    member breaks off (2CC7F8..2CC8E4): the first latched member, one per
//    frame, while members are attached, at the root's position; otherwise
//    both stop. An explosion's groups are deactivated (22CAD8) and never fire;
//  * attached members' damage emitters stay off until their release (2CB8F0
//    starts them from the member's own frame, the frame after the break);
//  * pieces with EmitSmoke own podWreckSmoke / podFire damage emitters.
// Particle laws follow the shared manager (161180): age in ms, Grows adds
// Growth*dt to size and Y, HasVelocity integrates, Gravity subtracts
// 32.2*GravityMultiplier*dt, FramesPerSet animates within the chosen set;
// brightness (62290 render) is 255 for Glows else 64, times 1-age/life when
// Fades, alpha/brightness on the PS2 128 scale; Additive selects ONE blending.
//
// Native, not original: CPU billboards in world space; a private random
// stream (effects never draw from the race's shared 308030 RNG, including the
// 15AD50 emitter-reset draw of a mode-2 group restart); embers launch from the private stream (D5: the source draws their direction,
// gate and life from 308030); piece fire
// burns while the piece moves, piece smoke continues at its minimum rate.
#include "native_wreck_debris_sim50pd.h"
#include <algorithm>
#include <bit>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace native_wreck_effects50PD {

using V3=std::array<float,3>;
enum class Kind : std::uint8_t {explosion,sparks,fire,smoke};
struct Definition {
    bool parsed=false;
    Kind kind=Kind::explosion;
    std::int32_t group=-1;
    float life=1000.f,size=1.f,growth=0.f,rate=500.f,ageAtBirth=0.f;
    V3 velocity{},minRange{},maxRange{},minNoise{},maxNoise{};
    float gravity=3.f;
    bool gravityOn=false,fades=false,additive=false,hasVelocity=false,grows=false,glows=false;
    bool mirrorX=false,mirrorY=false,applySpread=true;
    float framesPerSet=-1.f;
    std::int32_t emitMode=0;
    float delay=0.f,oneShot=0.f,deathTime=0.f,maxDeathTime=0.f;
    float minEmitRate=0.f,maxEmitRate=0.f,particlesToDistance=0.f;
    float minLife=0.f,maxLife=0.f,minSize=0.f,maxSize=0.f;
    float streamMin=0.f,streamMax=0.f,separation=1.5f;
};
struct Frame { float u0=0,v0=0,u1=0,v1=0; };
struct Group { std::int32_t first=0,last=-1; };
struct Atlas {
    std::vector<Group> groups;
    std::vector<Frame> frames; // normalized
    bool ready=false;
};
enum Effect : std::uint8_t {wreckExplosion,wreckSparks,emberFire,emberSmoke,groupExplosion,groupSparks,pieceSmoke,pieceFire,effectCount};
inline constexpr const char* effectFiles[effectCount]={
    "particle/podexplwreckexplosion.emt","particle/podexplwrecksparks.emt","particle/podemberfire.emt",
    "particle/podembersmoke.emt","particle/podexplosion.emt","particle/podwrecksparks.emt",
    "particle/podwrecksmoke.emt","particle/podfire.emt"};
inline constexpr Kind effectKinds[effectCount]={Kind::explosion,Kind::sparks,Kind::fire,Kind::smoke,
    Kind::explosion,Kind::sparks,Kind::smoke,Kind::fire};

struct Particle {
    V3 position{},velocity{};
    float age=0,life=1,size=1,growth=0,stream=0;
    std::int32_t frame=0;
    std::uint8_t effect=0;
    bool mirrorX=false,mirrorY=false;
};
struct Emitter {
    std::uint8_t effect=0;
    V3 position{},previous{};
    float age=0,carry=0;
    std::int32_t piece=-1; // sim piece index it follows, or -1
    std::int32_t group=-1; // sim wreck group whose C0/C4 this is, or -1
    std::uint32_t counter=0;
    bool alive=false;
    // 19BA50 PodEmberParticleEmitter: moves itself (19BF30) and carries its
    // 330 podEmberSmoke child, which follows the ember's previous point.
    bool ballistic=false;
    V3 velocity{},smokePosition{},smokePrevious{};
    float life=0,smokeCarry=0;
};
inline constexpr std::size_t maxParticles=4096,maxEmitters=192;
struct State {
    std::array<Definition,effectCount> definitions{};
    Atlas atlas;
    std::vector<Particle> particles;
    std::vector<Emitter> emitters;
    std::uint32_t random=0x2545f491u;
    std::uint64_t spawned=0,dropped=0,steps=0,embersLaunched=0;
    bool cold=false;
};
inline State& state() noexcept { static State owner; return owner; }

// ---- original text parsers -------------------------------------------------
namespace parse {
inline std::string trim(std::string s){
    const auto a=s.find_first_not_of(" \t\r\n");if(a==std::string::npos)return {};
    const auto b=s.find_last_not_of(" \t\r\n");return s.substr(a,b-a+1);
}
inline std::string lower(std::string s){for(char& c:s)if(c>='A'&&c<='Z')c=char(c-'A'+'a');return s;}
struct Entry { std::string section,key,value; };
inline std::vector<Entry> ini(const std::vector<std::uint8_t>& bytes){
    std::vector<Entry> out;std::string section,line;
    auto flush=[&](){
        const auto t=trim(line);line.clear();
        if(t.empty()||t[0]==';'||t[0]==':')return;
        if(t.front()=='['){const auto e=t.find(']');section=lower(trim(t.substr(1,e==std::string::npos?std::string::npos:e-1)));return;}
        const auto eq=t.find('=');if(eq==std::string::npos)return;
        auto value=t.substr(eq+1);const auto comment=value.find(';');if(comment!=std::string::npos)value=value.substr(0,comment);
        out.push_back({section,lower(trim(t.substr(0,eq))),trim(value)});
    };
    for(const auto b:bytes){if(b=='\n'){flush();continue;}line.push_back(char(b));}
    flush();return out;
}
inline const std::string* find(const std::vector<Entry>& e,const char* section,const char* key){
    for(const auto& x:e)if(x.section==section&&x.key==key)return &x.value;
    return nullptr;
}
inline void number(const std::vector<Entry>& e,const char* s,const char* k,float& out){
    if(const auto* v=find(e,s,k))out=std::strtof(v->c_str(),nullptr);
}
inline void integer(const std::vector<Entry>& e,const char* s,const char* k,std::int32_t& out){
    if(const auto* v=find(e,s,k))out=static_cast<std::int32_t>(std::strtol(v->c_str(),nullptr,10));
}
inline void boolean(const std::vector<Entry>& e,const char* s,const char* k,bool& out){
    if(const auto* v=find(e,s,k))out=!v->empty()&&(v->front()=='T'||v->front()=='t'||v->front()=='1');
}
inline void triple(const std::vector<Entry>& e,const char* s,const char* k,V3& out){
    if(const auto* v=find(e,s,k)){const char* p=v->c_str();char* end=nullptr;
        for(auto& f:out){f=std::strtof(p,&end);if(end==p)break;p=end;while(*p==','||*p==' ')++p;}}
}
}

inline Definition readDefinition(const std::vector<std::uint8_t>& bytes,Kind kind){
    using namespace parse;const auto e=ini(bytes);Definition d;d.kind=kind;
    const char* g="general info";const char* p="properties";
    integer(e,g,"particlegroup",d.group);number(e,g,"particlelife",d.life);number(e,g,"initialsize",d.size);
    number(e,g,"growth",d.growth);number(e,g,"emitrate",d.rate);number(e,g,"ageatbirth",d.ageAtBirth);
    triple(e,g,"velocity",d.velocity);triple(e,g,"minrange",d.minRange);triple(e,g,"maxrange",d.maxRange);
    triple(e,g,"minimumvelocitynoise",d.minNoise);triple(e,g,"maximumvelocitynoise",d.maxNoise);
    number(e,g,"gravitymultiplier",d.gravity);integer(e,g,"emitmode",d.emitMode);
    number(e,g,"delaytime",d.delay);number(e,g,"oneshottime",d.oneShot);number(e,g,"mindeathtime",d.deathTime);
    number(e,g,"maxdeathtime",d.maxDeathTime);
    number(e,g,"minimumemitrate",d.minEmitRate);number(e,g,"maximumemitrate",d.maxEmitRate);
    number(e,g,"particlestodistanceratio",d.particlesToDistance);
    number(e,g,"minimumlife",d.minLife);number(e,g,"maximumlife",d.maxLife);
    number(e,g,"minimuminitialsize",d.minSize);number(e,g,"maximuminitialsize",d.maxSize);
    number(e,g,"minstreamlength",d.streamMin);number(e,g,"maxstreamlength",d.streamMax);
    number(e,g,"particleseparationfactor",d.separation);boolean(e,g,"applyspreadtovelocity",d.applySpread);
    boolean(e,p,"fades",d.fades);boolean(e,p,"additive",d.additive);boolean(e,p,"hasvelocity",d.hasVelocity);
    boolean(e,p,"grows",d.grows);boolean(e,p,"glows",d.glows);boolean(e,p,"gravity",d.gravityOn);
    boolean(e,p,"randommirroraboutx",d.mirrorX);boolean(e,p,"randommirrorabouty",d.mirrorY);
    number(e,"animation","framesperset",d.framesPerSet);
    d.parsed=d.group>=0&&d.life>0.f;return d;
}
inline Atlas readAtlas(const std::vector<std::uint8_t>& prt,float width,float height){
    using namespace parse;const auto e=ini(prt);Atlas a;
    std::int32_t groups=0,frames=0;integer(e,"general info","numgroups",groups);integer(e,"general info","numframes",frames);
    if(groups<=0||frames<=0||groups>256||frames>4096||width<=0||height<=0)return a;
    a.groups.resize(std::size_t(groups));a.frames.resize(std::size_t(frames));
    for(std::int32_t i=0;i<groups;++i){const auto s="group"+std::to_string(i);
        integer(e,s.c_str(),"firstframe",a.groups[std::size_t(i)].first);integer(e,s.c_str(),"lastframe",a.groups[std::size_t(i)].last);}
    for(std::int32_t i=0;i<frames;++i){const auto s="frame"+std::to_string(i);float l=0,r=0,t=0,b=0;
        number(e,s.c_str(),"uvleft",l);number(e,s.c_str(),"uvright",r);number(e,s.c_str(),"uvtop",t);number(e,s.c_str(),"uvbottom",b);
        a.frames[std::size_t(i)]={l/width,t/height,r/width,b/height};}
    a.ready=true;return a;
}

// Cold: definitions from one racer archive (every pod archive carries the
// same ten effect members), atlas from the track. Reserves all storage.
template<class Rig>
inline bool loadCold(const Rig& rig,const std::vector<std::uint8_t>& prt,float atlasWidth,float atlasHeight,std::string& log){
    auto& s=state();s.cold=false;
    std::size_t parsed=0;
    for(std::size_t i=0;i<effectCount;++i){
        s.definitions[i]={};
        if(!rig.originalEntry(effectFiles[i]))continue;
        s.definitions[i]=readDefinition(rig.originalMember(effectFiles[i]),effectKinds[i]);parsed+=s.definitions[i].parsed;
    }
    s.atlas=readAtlas(prt,atlasWidth,atlasHeight);
    s.particles.clear();s.particles.reserve(maxParticles);s.emitters.clear();s.emitters.reserve(maxEmitters);
    s.cold=parsed&&s.atlas.ready;
    log="definitions="+std::to_string(parsed)+"/"+std::to_string(effectCount)+" atlas_groups="+
        std::to_string(s.atlas.groups.size())+" atlas_frames="+std::to_string(s.atlas.frames.size());
    for(std::size_t i=0;i<effectCount;++i){const auto& d=s.definitions[i];
        log+=" ["+std::to_string(i)+" g="+std::to_string(d.group)+" life="+std::to_string(int(d.life))+
            " rate="+std::to_string(d.rate)+(d.additive?" add":"")+"]";}
    return s.cold;
}
inline void reset() noexcept { auto& s=state();s.particles.clear();s.emitters.clear();s.spawned=s.dropped=s.steps=0; }

inline float unit(State& s) noexcept { s.random=s.random*1664525u+1013904223u;return float(s.random>>8)*(1.f/16777216.f); }
inline float lerp(float a,float b,float t) noexcept { return a+(b-a)*t; }

inline void start(Effect effect,const V3& position,std::int32_t piece=-1,std::int32_t group=-1) noexcept {
    auto& s=state();if(!s.cold||!s.definitions[effect].parsed)return;
    if(s.emitters.size()>=maxEmitters){++s.dropped;return;}
    Emitter e;e.effect=effect;e.position=e.previous=position;e.piece=piece;e.group=group;e.alive=true;s.emitters.push_back(e);
}
inline std::uint64_t groupRestarts=0;
// 2CC7F8..2CC8E4 per group frame, and the released members' 2CB8F0 damage
// emitters (podWreckSmoke/podFire) from the frame after their break.
inline void groupEffects() noexcept {
    auto& s=state();auto& d=native_wreck_debris50PD::sim();
    const auto& cold=native_wreck_debris50PD::cold();
    for(std::size_t i=0;i<d.pieces.size();++i){
        auto& p=d.pieces[i];
        if(p.effectsStarted||p.attached||!p.releasedStep||p.releasedStep>=d.steps||p.racer>=cold.racers.size())continue;
        const auto& racer=cold.racers[p.racer];if(p.index>=racer.pieces.size())continue;
        p.effectsStarted=true;
        if(!racer.pieces[p.index].emitSmoke)continue;
        const V3 at{p.position.x,p.position.y,p.position.z};
        start(pieceSmoke,at,std::int32_t(i));start(pieceFire,at,std::int32_t(i));
        std::cerr<<"piece_fx start piece="<<p.racer<<'.'<<p.index<<" step="<<d.steps<<" released="<<p.releasedStep<<'\n';
    }
    if(!s.definitions[groupSparks].parsed||!s.definitions[groupExplosion].parsed)return; // 2CC7F8: both must exist
    for(std::size_t gi=0;gi<d.groupCount;++gi){
        auto& g=d.groups[gi];if(!g.active||g.root>=d.pieces.size())continue;
        const auto& root=d.pieces[g.root];if(root.retired)continue;
        std::int32_t latched=-1;
        if(g.attached38)for(std::size_t i=0;i<g.capacity34;++i)if(g.latchB0[i]){latched=std::int32_t(i);break;}
        // Stop84 both (or, before a restart, the 88 reset of the running ones).
        for(auto& e:s.emitters)if(e.alive&&e.group==std::int32_t(gi))e.alive=false;
        if(latched<0)continue;
        g.latchB0[std::size_t(latched)]=false;
        const V3 at{root.position.x,root.position.y,root.position.z};
        start(groupSparks,at,std::int32_t(g.root),std::int32_t(gi));start(groupExplosion,at,std::int32_t(g.root),std::int32_t(gi));
        ++groupRestarts;
        std::cerr<<"group_fx slot="<<g.racer<<" c="<<g.collection+1<<" restart member="<<latched<<" step="<<d.steps
            <<" attached38="<<g.attached38<<'\n';
    }
}

inline void emitOne(State& s,Emitter& e,const Definition& d,float distance,std::int32_t effect=-1) noexcept {
    if(s.particles.size()>=maxParticles){++s.dropped;return;}
    const auto* g=d.group>=0&&std::size_t(d.group)<s.atlas.groups.size()?&s.atlas.groups[std::size_t(d.group)]:nullptr;
    if(!g||g->last<g->first)return;
    Particle p;p.effect=effect>=0?static_cast<std::uint8_t>(effect):e.effect;++e.counter;
    const float rx=unit(s),ry=unit(s),rz=unit(s);
    p.position=e.position;p.size=d.size;p.growth=d.grows?d.growth:0.f;p.life=d.life;
    const std::int32_t frames=g->last-g->first+1;
    std::int32_t set=0;
    if(d.framesPerSet>1.f){const auto sets=std::max<std::int32_t>(1,frames/std::int32_t(d.framesPerSet));set=std::min(sets-1,std::int32_t(rz*float(sets)));}
    p.frame=d.framesPerSet>1.f?g->first+set*std::int32_t(d.framesPerSet):g->first+std::min(frames-1,std::int32_t(rz*float(frames)));
    switch(d.kind){
    case Kind::explosion: // 15EB08..15EF20
        p.position[0]+=lerp(d.minRange[0],d.maxRange[0],rx);p.position[2]+=lerp(d.minRange[2],d.maxRange[2],rz);
        p.velocity=d.velocity;if(d.applySpread)p.velocity[1]+=d.velocity[1]*ry;
        p.life=d.life-.125f*d.life*float(e.counter&3u);break;
    case Kind::sparks:
        p.velocity={d.velocity[0]+lerp(d.minRange[0],d.maxRange[0],rx),d.velocity[1]+lerp(d.minRange[1],d.maxRange[1],ry),
            d.velocity[2]+lerp(d.minRange[2],d.maxRange[2],rz)};
        p.life=d.life*lerp(.75f,1.f,unit(s));p.stream=lerp(d.streamMin,d.streamMax,unit(s))*d.separation*d.size;break;
    case Kind::fire:
        p.position[0]+=lerp(d.minRange[0],d.maxRange[0],rx);p.position[1]+=lerp(d.minRange[1],d.maxRange[1],ry);
        p.position[2]+=lerp(d.minRange[2],d.maxRange[2],unit(s));p.velocity=d.velocity;break;
    case Kind::smoke:{
        const float t=unit(s);
        p.position[0]+=lerp(d.minRange[0],d.maxRange[0],rx)+(e.previous[0]-e.position[0])*t;
        p.position[1]+=(e.previous[1]-e.position[1])*t;
        p.position[2]+=lerp(d.minRange[2],d.maxRange[2],rz)+(e.previous[2]-e.position[2])*t;
        for(unsigned k=0;k<3;++k)p.velocity[k]=d.velocity[k]+lerp(d.minNoise[k],d.maxNoise[k],unit(s));
        if(d.maxLife>d.minLife&&d.minLife>0.f)p.life=lerp(d.minLife,d.maxLife,unit(s));
        if(d.maxSize>d.minSize&&d.minSize>0.f)p.size=lerp(d.minSize,d.maxSize,unit(s));
        (void)distance;break;}
    }
    // 19C520..19C528: an ember drops its fire at its previous point with the
    // constant zero velocity 434F50.
    if(e.ballistic&&d.kind==Kind::fire){p.position=e.previous;p.velocity={0.f,0.f,0.f};}
    p.age=d.ageAtBirth*p.life;
    p.mirrorX=d.mirrorX&&unit(s)>.5f;p.mirrorY=d.mirrorY&&unit(s)>.5f;
    s.particles.push_back(p);++s.spawned;
}

// One world step (the debris step's 0.016 s). Emitters that follow a sim
// piece take its position; particles age and move by the manager laws.
inline void step(float dt) noexcept {
    auto& s=state();if(!s.cold)return;++s.steps;
    groupEffects();
    const auto& pieces=native_wreck_debris50PD::sim().pieces;
    for(auto& e:s.emitters){
        if(!e.alive)continue;
        const auto& d=s.definitions[e.effect];
        if(e.ballistic){
            // 19BF74..19BFF4: alive while 340 <= 344 (age within its life),
            // then Stop84 and virtual18.
            if(!(e.age<=e.life)){e.alive=false;continue;}
            e.age+=dt;
            // 19C01C..19C098: 40 = 30, 30 += 60*dt, then 60.y -= 96.6f*dt.
            const float fall=std::bit_cast<float>(0x42c13334u)*dt;
            e.previous=e.position;
            for(unsigned k=0;k<3;++k)e.position[k]+=e.velocity[k]*dt;
            e.velocity[1]-=fall;
            e.carry+=d.rate*dt;const float fires=std::floor(e.carry);e.carry-=fires;
            for(std::int32_t n=0;n<std::int32_t(fires);++n)emitOne(s,e,d,0.f);
            // 19C0A8..19C188: the smoke child takes the ember's previous point
            // through three filters of time constant 0.005 s (gain
            // min(dt,0.005)/0.005, i.e. 1 at the world step), then 15ACB0.
            const auto& smoke=s.definitions[emberSmoke];
            if(smoke.parsed){
                const float gain=std::min(dt,.005f)/.005f;
                e.smokePrevious=e.smokePosition;
                for(unsigned k=0;k<3;++k)e.smokePosition[k]+=gain*(e.previous[k]-e.smokePosition[k]);
                const float dx=e.smokePosition[0]-e.smokePrevious[0],dy=e.smokePosition[1]-e.smokePrevious[1],
                    dz=e.smokePosition[2]-e.smokePrevious[2];
                float rate=smoke.rate+(dt>0.f?std::sqrt(dx*dx+dy*dy+dz*dz)/dt*smoke.particlesToDistance:0.f);
                if(smoke.maxEmitRate>0.f)rate=std::clamp(rate,smoke.minEmitRate,smoke.maxEmitRate);
                e.smokeCarry+=rate*dt;const float puffs=std::floor(e.smokeCarry);e.smokeCarry-=puffs;
                Emitter child=e;child.ballistic=false;child.effect=emberSmoke;
                child.position=e.smokePosition;child.previous=e.smokePrevious;
                for(std::int32_t n=0;n<std::int32_t(puffs);++n)emitOne(s,child,smoke,0.f);
            }
            continue;
        }
        e.previous=e.position;bool moving=true;
        if(e.piece>=0){
            if(std::size_t(e.piece)>=pieces.size()||pieces[std::size_t(e.piece)].retired){e.alive=false;continue;}
            const auto& p=pieces[std::size_t(e.piece)];e.position={p.position.x,p.position.y,p.position.z};moving=!p.settled;
        }
        const float t0=e.age;e.age+=dt;
        float count=0.f;
        switch(d.kind){
        case Kind::explosion:case Kind::sparks:{
            const float end=d.delay+(d.emitMode==1?d.oneShot:dt);
            if(t0<d.delay){if(e.age<d.delay)break;}
            if(t0>=end){e.alive=false;break;}
            count=std::max(1.f,std::floor(.5f+d.rate*dt));break;}
        case Kind::fire:
            if(e.piece>=0?!moving:(d.deathTime>0.f&&e.age>d.deathTime)){if(e.piece<0)e.alive=false;break;}
            count=d.rate*dt;break;
        case Kind::smoke:{
            if(e.piece<0&&e.age>std::max(d.deathTime,s.definitions[emberFire].deathTime)){e.alive=false;break;}
            const float dx=e.position[0]-e.previous[0],dy=e.position[1]-e.previous[1],dz=e.position[2]-e.previous[2];
            const float distance=std::sqrt(dx*dx+dy*dy+dz*dz);
            float rate=d.rate+(dt>0.f?distance/dt*d.particlesToDistance:0.f);
            if(d.maxEmitRate>0.f)rate=std::clamp(rate,d.minEmitRate,d.maxEmitRate);
            count=rate*dt;break;}
        }
        if(d.kind==Kind::fire||d.kind==Kind::smoke){e.carry+=count;count=std::floor(e.carry);e.carry-=count;}
        for(std::int32_t n=0;n<std::int32_t(count);++n)emitOne(s,e,d,0.f);
    }
    s.emitters.erase(std::remove_if(s.emitters.begin(),s.emitters.end(),[](const Emitter& e){return !e.alive;}),s.emitters.end());
    const float milliseconds=1000.f*dt;
    for(std::size_t i=0;i<s.particles.size();){
        auto& p=s.particles[i];p.age+=milliseconds;
        if(p.age>p.life){p=s.particles.back();s.particles.pop_back();continue;}
        const auto& d=s.definitions[p.effect];
        if(d.grows){p.size=std::max(0.f,p.size+p.growth*dt);p.position[1]+=p.growth*dt;}
        if(d.hasVelocity)for(unsigned k=0;k<3;++k)p.position[k]+=p.velocity[k]*dt;
        if(d.gravityOn)p.velocity[1]-=32.2f*d.gravity*dt;
        if(d.framesPerSet>1.f&&p.life>0.f){
            const auto& g=s.atlas.groups[std::size_t(d.group)];
            const auto perSet=std::int32_t(d.framesPerSet);
            const auto set=(p.frame-g.first)/perSet;
            auto phase=std::int32_t(p.age/(p.life/d.framesPerSet));if(phase>0)--phase;
            phase=std::clamp<std::int32_t>(phase,0,perSet-1);
            p.frame=std::min(g.last,g.first+set*perSet+phase);
        }
        ++i;
    }
}

// Destroy handlers: 2261B0 (explode) starts 814/810 at the body and the
// embers at the explosion point; EmitSmoke pieces get smoke and fire, except
// attached group members (theirs start at release). No group sparks or
// explosion here: an explosion's groups are deactivated (22CAD8, 2CC9C0) and
// a wreck's fire only on a break (groupEffects).
inline void destroyed(std::size_t slot,bool explode,const V3& body,const V3& explosion) noexcept {
    auto& s=state();if(!s.cold)return;
    if(explode){
        start(wreckExplosion,body);start(wreckSparks,body);
        // 818[0..9] (2263A4..2263EC): entries whose 370 > 0.4 launch from 270
        // (19C780): 60 = 360 * 117.333f (80 mph), 360 = (2r-1, r, 2r-1) drawn
        // at construction (19BE48..19BED8), 370 = r (19CC24), life 344 =
        // lerp(MinDeathTime, MaxDeathTime, r) (19C834..19C854).
        const auto& fire=s.definitions[emberFire];
        if(fire.parsed)for(unsigned i=0;i<10;++i){
            const float dx=2.f*unit(s)-1.f,dy=unit(s),dz=2.f*unit(s)-1.f,gate=unit(s);
            if(!(gate>.4f))continue;
            if(s.emitters.size()>=maxEmitters){++s.dropped;continue;}
            const float speed=std::bit_cast<float>(0x42eaaa65u);
            const float maximum=fire.maxDeathTime>0.f?fire.maxDeathTime:fire.deathTime;
            Emitter e;e.effect=emberFire;e.alive=true;e.ballistic=true;
            e.position=e.previous=e.smokePosition=e.smokePrevious=explosion;
            e.velocity={dx*speed,dy*speed,dz*speed};
            e.life=lerp(fire.deathTime,maximum,unit(s));
            s.emitters.push_back(e);++s.embersLaunched;
        }
    }
    const auto& cold=native_wreck_debris50PD::cold();
    auto& pieces=native_wreck_debris50PD::sim().pieces;
    if(slot>=cold.racers.size())return;
    const auto& racer=cold.racers[slot];
    for(std::size_t i=0;i<pieces.size();++i){
        auto& p=pieces[i];if(p.racer!=slot||p.index>=racer.pieces.size()||p.effectsStarted||p.attached)continue;
        const auto& piece=racer.pieces[p.index];const V3 at{p.position.x,p.position.y,p.position.z};
        if(piece.emitSmoke){start(pieceSmoke,at,std::int32_t(i));start(pieceFire,at,std::int32_t(i));}
        p.effectsStarted=true;
    }
}

// Camera-facing quads: blended (back to front) first, then additive.
struct Camera { V3 position{},right{},up{}; };
inline Camera camera(const V3& position,const V3& target,const V3& upHint) noexcept {
    auto norm=[](V3 v){const float n=std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);return n>0.f?V3{v[0]/n,v[1]/n,v[2]/n}:V3{0,0,0};};
    auto cross=[](V3 a,V3 b){return V3{a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]};};
    const V3 forward=norm({target[0]-position[0],target[1]-position[1],target[2]-position[2]});
    V3 right=norm(cross(forward,upHint));if(right==V3{0,0,0})right={1,0,0};
    return {position,right,cross(right,forward)};
}
template<class Vertex>
inline std::uint32_t build(const Camera& c,Vertex* out,std::uint32_t capacity,std::uint32_t& blended) noexcept {
    auto& s=state();blended=0;if(!s.cold||!out)return 0;
    static std::array<std::pair<float,std::uint32_t>,maxParticles> order{};
    std::uint32_t blendedCount=0;
    for(std::uint32_t i=0;i<s.particles.size()&&i<maxParticles;++i){
        const auto& p=s.particles[i];
        const float dx=p.position[0]-c.position[0],dy=p.position[1]-c.position[1],dz=p.position[2]-c.position[2];
        if(!s.definitions[p.effect].additive)order[blendedCount++]={dx*dx+dy*dy+dz*dz,i};
    }
    std::sort(order.begin(),order.begin()+blendedCount,[](const auto& a,const auto& b){return a.first>b.first;});
    std::uint32_t quads=0;
    auto emit=[&](const Particle& p){
        if(quads>=capacity)return;
        const auto& d=s.definitions[p.effect];
        if(p.frame<0||std::size_t(p.frame)>=s.atlas.frames.size())return;
        auto f=s.atlas.frames[std::size_t(p.frame)];
        if(p.mirrorX)std::swap(f.u0,f.u1);
        if(p.mirrorY)std::swap(f.v0,f.v1);
        float brightness=d.glows?255.f:64.f;
        if(d.fades&&p.life>0.f)brightness*=std::max(0.f,1.f-p.age/p.life);
        const auto alpha=static_cast<std::uint8_t>(std::clamp(brightness,0.f,255.f));
        V3 a=c.right,b=c.up;float ha=p.size,hb=p.size;
        if(d.kind==Kind::sparks){
            const float n=std::sqrt(p.velocity[0]*p.velocity[0]+p.velocity[1]*p.velocity[1]+p.velocity[2]*p.velocity[2]);
            if(n>1e-3f){
                const V3 axis{p.velocity[0]/n,p.velocity[1]/n,p.velocity[2]/n};
                const V3 view{p.position[0]-c.position[0],p.position[1]-c.position[1],p.position[2]-c.position[2]};
                V3 side{axis[1]*view[2]-axis[2]*view[1],axis[2]*view[0]-axis[0]*view[2],axis[0]*view[1]-axis[1]*view[0]};
                const float m=std::sqrt(side[0]*side[0]+side[1]*side[1]+side[2]*side[2]);
                if(m>1e-3f){a={side[0]/m,side[1]/m,side[2]/m};b=axis;hb=std::max(p.size,.5f*p.stream);}
            }
        }
        const float corners[4][2]={{-1,1},{-1,-1},{1,1},{1,-1}}; // TL, BL, TR, BR
        const float uvs[4][2]={{f.u0,f.v0},{f.u0,f.v1},{f.u1,f.v0},{f.u1,f.v1}};
        for(unsigned k=0;k<4;++k){
            auto& v=out[quads*4+k];
            for(unsigned j=0;j<3;++j)v.position[j]=p.position[j]+a[j]*ha*corners[k][0]+b[j]*hb*corners[k][1];
            v.uv[0]=uvs[k][0];v.uv[1]=uvs[k][1];
            v.color[0]=v.color[1]=v.color[2]=128;v.color[3]=alpha;
        }
        ++quads;
    };
    for(std::uint32_t n=0;n<blendedCount;++n)emit(s.particles[order[n].second]);
    blended=quads;
    for(std::uint32_t i=0;i<s.particles.size()&&i<maxParticles;++i)
        if(s.definitions[s.particles[i].effect].additive)emit(s.particles[i]);
    return quads;
}

} // namespace native_wreck_effects50PD
