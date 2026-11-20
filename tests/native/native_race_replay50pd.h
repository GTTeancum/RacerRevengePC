#pragma once
// Common race recording and the replay behind the original Results.
//
// Original (SLUS_202.68, docs/common-results-20261108.md "Original replay"):
// race78 is a SimpleVCR. Its channels are registered when the racer manager
// loads (1AD9A0 per racer), recording starts at GO (1AFE30 -> 287B60, bit1),
// the VCR samples every 1/15 s (287C90), 1AB520 turns it to playback when the
// Results open (bit1 off, bit2 on, every racer +3C = 1, 229F80 reset), and the
// race update loops it from the start when the recording ends (1A8158).
//
// This module drives that real race78 (src/game/original_simple_vcr50la) with
// the original channel set, read from and written back into the port's racer
// state. Native, documented deviations:
// * Channel backing is reserved inside the VCR (the port has no racer+C50..
//   +C78/+B5C/+C00 words); it is copied from/into VehicleFrameState49W.
// * The port cannot re-simulate the cockpit/engine models against a replayed
//   pod, so the other parentless scene roots are kept as a native track of
//   transforms relative to main190, sampled at the same VCR frame instants
//   and interpolated by the same playback time.
#include "original_race_constructor50la.h"
#include "../../src/game/vehicle_frame49w.h"
#include "../../src/game/scene_graph49p.h"
#include "../../src/game/body_scene48g.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

namespace native_race_replay50PD {
using rr::game::OriginalRaceConstructor50LA;
using rr::game::OriginalSimpleVCR50LA;
using rr::game::OriginalSimpleVCRDiagnostic50LA;
constexpr std::size_t maxRacers=8,maxRoots=12;
// Words per racer: 0..3 C60 quaternion, 4..6 C50 position, 7..9 C70 velocity,
// 10 B5C, 11 B60, 12 B68. Bytes per racer: C00..C06.
constexpr std::size_t racerWords=13,racerBytes=7;
struct RacerTraits {bool ai=false,configurationEntry=false,extra1C0=false,extra1C4=false;};
using Matrix=std::array<float,16>;
struct RootSample {float time=0;std::array<Matrix,maxRoots> relative{};};
struct RacerTrack {
    std::array<std::int32_t,maxRoots> roots{};std::size_t rootCount=0;
    std::vector<RootSample> samples;
};

inline Matrix toArray(const rr::game::BodyMatrix48G& m)noexcept{Matrix a;std::memcpy(a.data(),m.v,64);return a;}
inline rr::game::BodyMatrix48G toBody(const Matrix& a)noexcept{rr::game::BodyMatrix48G m;std::memcpy(m.v,a.data(),64);return m;}
// Row-vector convention (world = local * parentWorld): (a*b)[i][j].
inline Matrix multiply(const Matrix& a,const Matrix& b)noexcept{
    Matrix r{};
    for(int i=0;i<4;++i)for(int j=0;j<4;++j){
        float sum=0;for(int k=0;k<4;++k)sum+=a[i*4+k]*b[k*4+j];r[i*4+j]=sum;
    }
    return r;
}
// Affine inverse: rows 0..2 basis (fourth lanes 0), row 3 translation.
inline bool inverseAffine(const Matrix& m,Matrix& out)noexcept{
    const float a=m[0],b=m[1],c=m[2],d=m[4],e=m[5],f=m[6],g=m[8],h=m[9],i=m[10];
    const float A=e*i-f*h,B=-(d*i-f*g),C=d*h-e*g;
    const float det=a*A+b*B+c*C;
    if(!(std::fabs(det)>1e-12f)||!std::isfinite(det))return false;
    const float s=1.f/det;
    Matrix r{};
    r[0]=A*s;r[1]=-(b*i-c*h)*s;r[2]=(b*f-c*e)*s;
    r[4]=B*s;r[5]=(a*i-c*g)*s;r[6]=-(a*f-c*d)*s;
    r[8]=C*s;r[9]=-(a*h-b*g)*s;r[10]=(a*e-b*d)*s;
    const float tx=m[12],ty=m[13],tz=m[14];
    for(int j=0;j<3;++j)r[12+j]=-(tx*r[0+j]+ty*r[4+j]+tz*r[8+j]);
    r[15]=1.f;out=r;return true;
}

class Owner final {
public:
    enum class Phase:std::uint8_t {idle,bound,recording,replay};
    struct Stats {
        std::uint32_t captures=0,nativeSamples=0,applied=0,loops=0,rejected=0,lapRestarts=0;
        const char* lastRejection=nullptr;
    };
    // A course without the RA world join (its race uses the native owners).
    // Every consumed race records race78 (RA's world74_ race included).
    static bool commonRace(const OriginalRaceConstructor50LA&)noexcept{return true;}
    static OriginalSimpleVCR50LA::State vcrState(const OriginalRaceConstructor50LA& race)noexcept{
        return race.vcr78Source_?race.vcr78Source_->state():OriginalSimpleVCR50LA::State{};
    }
    Phase phase()const noexcept{return phase_;}
    const Stats& stats()const noexcept{return stats_;}
    std::size_t racers()const noexcept{return count_;}
    void reset()noexcept{
        raceWeak_.reset();race_=nullptr;phase_=Phase::idle;count_=0;words_=nullptr;bytes_=nullptr;
        for(auto& t:tracks_){t.rootCount=0;t.samples.clear();}
        stats_={};lastWrites_=0;state44_={};
    }
    // In-place 1AC8E0 (race1B0 bit4 clear; every course and pod): the restarted race
    // records again from its own GO 1AFE30 (287B60 then bit1, startRecording), as the
    // original does at every GO. The binding (race78 channels, traits, roots) is the
    // same race's and stays; only the previous recording's native samples are dropped.
    // Idle (never bound) stays idle. Refused when race78 is no longer published.
    bool rearm1AC8E0(const char*& why)noexcept{
        why=nullptr;
        if(phase_==Phase::idle)return true;
        if(!current(why))return false;
        for(auto& t:tracks_)t.samples.clear();
        pending_={};captured_.fill(false);state44_={};lastWrites_=0;
        phase_=Phase::bound;return true;
    }
    // 1AD9A0 for every racer, in roster order, after race78 and Race90 exist
    // and before the first VCR update (bit4 closes registration).
    bool bind(const std::shared_ptr<OriginalRaceConstructor50LA>& race,const RacerTraits* traits,
        std::size_t count,const char*& why)noexcept{
        why=nullptr;
        if(phase_!=Phase::idle){why="replay already bound";return false;}
        if(!race||!traits||!count||count>maxRacers){why="replay bind requires the race and 1..8 racers";return false;}
        auto* vcr=race->vcr78Source_;
        if(!vcr||race->vcr78Owner_.get()!=vcr||!vcr->belongsToRace(*race)){why="race78 SimpleVCR is not published";return false;}
        std::uint32_t* words=nullptr;std::uint8_t* bytes=nullptr;
        if(!vcr->reserveNativeChannelBacking(count*racerWords,count*racerBytes,words,bytes)){
            why="race78 native channel backing unavailable";return false;
        }
        for(std::size_t r=0;r<count;++r){
            auto* w=words+r*racerWords;auto* b=bytes+r*racerBytes;const auto& t=traits[r];
            const auto add=[&](std::uint32_t* p,std::uint32_t interp,float scale){
                return vcr->addChannel2878F0(p,nullptr,interp,scale,1u)>=0;};
            bool ok=true;
            for(int k=0;k<4;++k)ok=ok&&add(w+k,3u,5000.f);      // C60..C6C
            for(int k=0;k<3;++k)ok=ok&&add(w+4+k,0u,1.f);       // C50..C58
            for(int k=0;k<3;++k)ok=ok&&add(w+7+k,0u,1.f);       // C70..C78
            if(t.ai)ok=ok&&add(w+10,0u,10.f);                    // B5C (racer58 == 1)
            else if(t.configurationEntry)ok=ok&&add(w+12,0u,10.f)&&add(w+11,0u,10.f); // B68, B60
            else ok=ok&&add(w+10,0u,10.f);                       // B5C
            for(int k=0;k<5;++k)ok=ok&&vcr->addByteChannel287A30(b+k,nullptr,1u)>=0; // C00..C04
            if(t.extra1C0)ok=ok&&vcr->addByteChannel287A30(b+6,nullptr,1u)>=0;       // C06
            if(t.extra1C4)ok=ok&&vcr->addByteChannel287A30(b+5,nullptr,1u)>=0;       // C05
            if(!ok){why="race78 rejected a racer channel (capacity/bit4)";return false;}
        }
        raceWeak_=race;race_=race.get();words_=words;bytes_=bytes;count_=count;phase_=Phase::bound;
        for(auto& t:tracks_){t.rootCount=0;t.samples.clear();}
        return true;
    }
    // 1AFE30 at GO: 287B60 then bit1 (non-subtype-3 path).
    bool startRecording(const char*& why)noexcept{
        why=nullptr;auto* vcr=current(why);if(!vcr)return false;
        if(phase_!=Phase::bound){why="recording requires a freshly bound replay";return false;}
        OriginalSimpleVCRDiagnostic50LA d;
        if(!vcr->startRecord1A80C8(*race_,d)){why=d.operation?d.operation:"287B60 rejected";return false;}
        for(auto& t:tracks_)t.samples.clear();captured_.fill(false);
        // race1B0 bit3 is not touched here: only 1A7C18 clears it, after 2AEE90 accepted
        // the race fade-in (the one 1A7BD0 owner, on every course).
        phase_=Phase::recording;lastWrites_=0;return true;
    }
    // 1A7F40..1A80EC (race update, non-subtype-3, B4 flags58 bits3..4 = 1 or 3,
    // i.e. a human lap event, and the association head 3C[0]+48 bit0 clear):
    // 229F20 every racer, race1A8 += 1, 287B60, bit1. The recording restarts
    // at each human lap, so the Results replay holds the human's last lap.
    // 229F20's C38/C40 snapshot is not represented: the port's replay pose
    // comes from the first played frame (apply), not from a 216A30 placement.
    bool restartRecordingAtLap1A7F40(const char*& why)noexcept{
        why=nullptr;auto* vcr=current(why);if(!vcr)return false;
        if(phase_!=Phase::recording){why="lap restart outside recording";return false;}
        race_->field1A8_+=1u;                    // 1A80BC..1A80C4 (ADDIU wraps)
        OriginalSimpleVCRDiagnostic50LA d;
        if(!vcr->startRecord1A80C8(*race_,d)){why=d.operation?d.operation:"287B60 rejected";return false;}
        for(auto& t:tracks_)t.samples.clear();captured_.fill(false);lastWrites_=0;++stats_.lapRestarts;
        return true;
    }
    // 229F80's C00..C06 = 0 on a restored racer. The original channels write
    // straight into racer C00..C06, so the store also clears what 229770 reads
    // until playback writes the next frame; here that is the channel backing.
    bool clearRacerBytes229F80(std::size_t slot)noexcept{
        if(phase_!=Phase::replay||!bytes_||slot>=count_)return false;
        std::memset(bytes_+slot*racerBytes,0,racerBytes);state44_[slot]=0;return true;
    }
    std::uint32_t race1A8()const noexcept{return race_&&!raceWeak_.expired()?race_->field1A8_:0u;}
    // 229770's racer+44 (C02 & 0xF) of the last applied frame; 228620 compares
    // it with +40 to re-run the recorded wreck (2289..: 3C != 0 branches).
    std::uint32_t replayState44(std::size_t slot)const noexcept{return slot<state44_.size()?state44_[slot]:0u;}
    // 1AB520 waiting branch: race1B0 bit4 (Results replay) before 1AC8E0.
    bool markResultsReplay1AB520()noexcept{
        if(!race_||raceWeak_.expired())return false;
        race_->flags1B0KnownValue_=std::uint8_t(race_->flags1B0KnownValue_|0x10u);
        race_->flags1B0KnownMask_=std::uint8_t(race_->flags1B0KnownMask_|0x10u);return true;
    }
    // True when the next 287C90 will write a VCR frame: staging must be current.
    bool captureDue()const noexcept{
        if(phase_!=Phase::recording||!race_||raceWeak_.expired())return false;
        const auto* vcr=race_->vcr78Source_;return vcr&&vcr->recordSampleDue();
    }
    // Inside a bank visit of this racer slot. Copies the original channel
    // sources (port fields of racer+C50..C78/B5C../C00..) into staging and
    // snapshots every other parentless scene root relative to main190.
    bool capture(std::size_t slot,const rr::game::VehicleFrameState49W& s,
        const rr::game::VehicleFrameBindings49W& b)noexcept{
        {const char* why=nullptr;if(!current(why))return reject(why);}
        if(phase_!=Phase::recording||slot>=count_)return reject("capture outside recording");
        const auto main=static_cast<std::uint32_t>(b.lifecycle.main190);
        if(main>=s.sceneNodeCount||s.sceneNodeCount>rr::game::maxSceneNodes49P)return reject("capture scene unavailable");
        // Worlds on a private copy: the lazy cache of the live state is untouched.
        std::array<rr::game::SceneNode49P,rr::game::maxSceneNodes49P> scratch;
        std::memcpy(scratch.data(),s.nodes,s.sceneNodeCount*sizeof(rr::game::SceneNode49P));
        const rr::game::SceneGraph49P graph{scratch.data(),s.sceneNodeCount};
        if(!rr::game::refreshSceneWorld49P(graph,b.lifecycle.main190))return reject("capture main190 world");
        const auto& world=scratch[main].world100;
        rr::game::Quaternion48C q{};
        if(!rr::game::matrixToQuaternion48G(world,q))return reject("capture main190 quaternion");
        auto* w=words_+slot*racerWords;
        std::memcpy(w,q.v,16);std::memcpy(w+4,world.v+12,12);
        std::memcpy(w+7,s.bodies.main.components.root.body.velocity.v,12);
        std::memcpy(w+10,&s.bodies.main.controls.bound.control.shaping.targets.values[0],4);
        std::memcpy(w+11,&s.bodies.main.controls.bound.control.shaping.targets.values[1],4);
        std::memcpy(w+12,&s.bodies.main.controls.bound.control.shaping.targets.values[3],4);
        std::memcpy(bytes_+slot*racerBytes,s.packedC00,racerBytes);
        // Native root track.
        auto& track=tracks_[slot];
        if(!track.rootCount){
            for(std::uint32_t n=0;n<s.sceneNodeCount&&track.rootCount<maxRoots;++n)
                if(n!=main&&s.nodes[n].parent180==rr::game::SceneNodeIndex49P::none)
                    track.roots[track.rootCount++]=static_cast<std::int32_t>(n);
        }
        Matrix inverse;if(!inverseAffine(toArray(world),inverse))return reject("capture main190 inverse");
        pending_[slot].time=0;
        for(std::size_t k=0;k<track.rootCount;++k){
            const auto node=static_cast<rr::game::SceneNodeIndex49P>(track.roots[k]);
            if(!rr::game::refreshSceneWorld49P(graph,node))return reject("capture root world");
            pending_[slot].relative[k]=multiply(toArray(scratch[track.roots[k]].world100),inverse);
        }
        captured_[slot]=true;++stats_.captures;return true;
    }
    // One 287C90 for this fixed step. While recording, Race90 advances as in
    // 1A7F48 and a written VCR frame also commits the native root samples.
    bool step(float dt,bool paused11C,const char*& why)noexcept{
        why=nullptr;auto* vcr=current(why);if(!vcr)return false;
        if(phase_==Phase::recording&&!paused11C){
            float time=0;std::memcpy(&time,&race_->field90_,4);time+=dt;std::memcpy(&race_->field90_,&time,4);
        }
        OriginalSimpleVCRDiagnostic50LA d;
        if(!vcr->update287C90(dt,paused11C,d)){why=d.operation?d.operation:"287C90 rejected";return false;}
        const auto st=vcr->state();
        const auto writes=st.keyframes+st.deltas;
        if(phase_==Phase::recording&&writes!=lastWrites_){
            lastWrites_=writes;
            for(std::size_t r=0;r<count_;++r){
                if(!captured_[r])continue;
                auto sample=pending_[r];sample.time=st.time30;
                try{tracks_[r].samples.push_back(sample);}catch(...){why="native root track allocation";return false;}
                ++stats_.nativeSamples;captured_[r]=false;
            }
        }
        return true;
    }
    // 1ABB2C..1ABB68: bit1 off, bit2 on. Every racer enters replay (+3C = 1).
    bool beginReplay(const char*& why)noexcept{
        why=nullptr;auto* vcr=current(why);if(!vcr)return false;
        if(phase_!=Phase::recording){why="replay requires a recorded race";return false;}
        OriginalSimpleVCRDiagnostic50LA d;
        if(!vcr->setPlayback(true,d)){why=d.operation;return false;}
        phase_=Phase::replay;return true;
    }
    // 1A8158: the playback reached the end of the recording (bit5).
    bool ended()const noexcept{
        if(phase_!=Phase::replay||!race_||raceWeak_.expired()||!race_->vcr78Source_)return false;
        return race_->vcr78Source_->state().ended;
    }
    // 1A81B8..1A81D8: 287BE0 then bit2 (loop from the start).
    bool loop(const char*& why)noexcept{
        why=nullptr;auto* vcr=current(why);if(!vcr)return false;
        OriginalSimpleVCRDiagnostic50LA d;
        if(!vcr->reset287BE0(d)||!vcr->setPlayback(false,d)){why=d.operation?d.operation:"loop rejected";return false;}
        ++stats_.loops;return true;
    }
    float playTime()const noexcept{
        return race_&&!raceWeak_.expired()&&race_->vcr78Source_?race_->vcr78Source_->state().play34:0.f;
    }
    // Inside a bank visit: the played channels into the port racer, as 229770
    // and the replay pose. No physics, AI, input or frame phase is touched.
    bool apply(std::size_t slot,rr::game::VehicleFrameState49W& s,
        const rr::game::VehicleFrameBindings49W& b)noexcept{
        {const char* why=nullptr;if(!current(why))return reject(why);}
        if(phase_!=Phase::replay||slot>=count_)return reject("apply outside replay");
        const auto main=static_cast<std::uint32_t>(b.lifecycle.main190);
        const auto secondary=static_cast<std::uint32_t>(b.lifecycle.physical19C);
        if(main>=s.sceneNodeCount||secondary>=s.sceneNodeCount)return reject("apply scene unavailable");
        const auto* w=words_+slot*racerWords;const auto* p=bytes_+slot*racerBytes;
        rr::game::Quaternion48C q{};std::memcpy(q.v,w,16);
        rr::game::BodyMatrix48G world{};
        if(!rr::game::quaternionToMatrix48G(q,world)){
            static int reported=0; //read-only diagnostic: the played words that refused
            if(reported<3){++reported;const auto st=race_->vcr78Source_->state();
                std::fprintf(stderr,"race_replay quaternion_refused slot=%zu q=%g,%g,%g,%g pos=%g,%g,%g play=%g time30=%g keyframes=%llu deltas=%llu ended=%d\n",
                    slot,q.v[0],q.v[1],q.v[2],q.v[3],
                    *reinterpret_cast<const float*>(w+4),*reinterpret_cast<const float*>(w+5),*reinterpret_cast<const float*>(w+6),
                    st.play34,st.time30,(unsigned long long)st.keyframes,(unsigned long long)st.deltas,int(st.ended));}
            return reject("apply quaternion");
        }
        std::memcpy(world.v+12,w+4,12);world.v[15]=1.f;
        const rr::game::SceneGraph49P graph{s.nodes,s.sceneNodeCount};
        if(!rr::game::setParentlessWorldMatrix49P(graph,b.lifecycle.main190,world))return reject("apply main190");
        // Native root track at the same playback time.
        auto& track=tracks_[slot];
        if(!track.samples.empty()&&track.rootCount){
            const float t=playTime();
            std::size_t hi=0;while(hi<track.samples.size()&&track.samples[hi].time<=t)++hi;
            const auto& a=track.samples[hi?hi-1:0];const auto& c=track.samples[hi<track.samples.size()?hi:track.samples.size()-1];
            float f=0;if(c.time>a.time)f=std::clamp((t-a.time)/(c.time-a.time),0.f,1.f);
            const auto mainWorld=toArray(world);
            for(std::size_t k=0;k<track.rootCount;++k){
                Matrix rel;for(int e=0;e<16;++e)rel[e]=a.relative[k][e]+f*(c.relative[k][e]-a.relative[k][e]);
                rel[3]=rel[7]=rel[11]=0.f;rel[15]=1.f;
                const auto node=static_cast<rr::game::SceneNodeIndex49P>(track.roots[k]);
                if(!rr::game::setParentlessWorldMatrix49P(graph,node,toBody(multiply(rel,mainWorld))))
                    return reject("apply root");
            }
        }
        // Every node: cables, Binder and flame read cached worlds directly.
        for(std::uint32_t n=0;n<s.sceneNodeCount;++n)
            if(!rr::game::refreshSceneWorld49P(graph,static_cast<rr::game::SceneNodeIndex49P>(n)))
                return reject("apply refresh");
        // runtime_startup50fl::project: body scenes and scene flags from nodes.
        auto& m=s.bodies.main;auto& z=s.bodies.secondary;
        m.components.root.scene=s.nodes[main].world100;z.root.scene=s.nodes[secondary].world100;
        s.bodies.mainSphere.sceneFlags190=(s.bodies.mainSphere.sceneFlags190&0xffffff00u)|s.nodes[main].sceneFlags190;
        z.sceneFlags190=(z.sceneFlags190&0xffffff00u)|s.nodes[secondary].sceneFlags190;
        for(auto* root:{&m.components.root,&z.root}){
            if(!rr::game::matrixToQuaternion48G(root->scene,root->body.orientation))return reject("apply body orientation");
            std::memcpy(root->body.position.v,root->scene.v+12,12);
        }
        std::memcpy(m.components.root.body.velocity.v,w+7,12);
        auto& targets=m.controls.bound.control.shaping.targets.values;
        std::memcpy(&targets[0],w+10,4);std::memcpy(&targets[1],w+11,4);std::memcpy(&targets[3],w+12,4);
        // 229770.
        std::memcpy(s.packedC00,p,racerBytes);
        state44_[slot]=p[2]&0xfu;                 // 229AD0..229ADC
        const auto code=[&](unsigned shift){return float((p[0]>>shift)&3u)*0.33f;};
        m.components.engine.smoothedDrive[0]=code(0);m.components.engine.smoothedDrive[1]=code(2);
        m.components.bindings.aeroDrive[0]=code(4);m.components.bindings.aeroDrive[1]=code(6);
        m.controls.player.actionB80=float((p[1]>>7)&1u);
        m.controls.player.flagsB9C=static_cast<std::uint8_t>((m.controls.player.flagsB9C&~3u)|((p[2]>>5)&1u)|(((p[2]>>6)&1u)<<1));
        if((p[1]>>6)&1u){for(auto& v:s.fields.impactBA0.v)v=0.f;}
        else for(unsigned k=0;k<3;++k){
            float v=float((p[1]>>(2*k))&3u)*20.f-40.f;if(!(v<0.f))v+=20.f;s.fields.impactBA0.v[k]=v;
        }
        s.fields.damage7D4=float(p[3])*(1.f/255.f);s.fields.damage7D0=float(p[4])*(1.f/255.f);
        s.fields.damage7DC=float(p[5])*(1.f/255.f);s.fields.damage7D8=float(p[6])*(1.f/255.f);
        ++stats_.applied;return true;
    }
private:
    OriginalSimpleVCR50LA* current(const char*& why)noexcept{
        if(raceWeak_.expired())race_=nullptr;
        if(!race_||!race_->vcr78Source_||race_->vcr78Owner_.get()!=race_->vcr78Source_||
           !race_->vcr78Source_->belongsToRace(*race_)){why="race78 SimpleVCR no longer published";return nullptr;}
        return race_->vcr78Source_;
    }
    bool reject(const char* why)noexcept{++stats_.rejected;stats_.lastRejection=why;return false;}
    std::weak_ptr<OriginalRaceConstructor50LA> raceWeak_;OriginalRaceConstructor50LA* race_=nullptr;
    Phase phase_=Phase::idle;std::size_t count_=0;
    std::uint32_t* words_=nullptr;std::uint8_t* bytes_=nullptr;
    std::array<RacerTrack,maxRacers> tracks_{};
    std::array<RootSample,maxRacers> pending_{};std::array<bool,maxRacers> captured_{};
    std::array<std::uint32_t,maxRacers> state44_{};
    std::uint32_t lastWrites_=0;Stats stats_{};
};
inline Owner& owner()noexcept{static Owner o;return o;}
}
