// TEST ONLY. Bounded supplied-counter -> recovered timing -> numeric consumer
// diagnostic. No OS clock, rendering, full game loop, world/visibility/physics
// queries, IRQ model, game-mode chooser, or opaque callback implementation.
#include "../../src/game/clock_quantization49j.h"
#include "../../src/game/control_shaping48v.h"
#include "../../src/game/camera_frame49f.h"
#include "../../evidence/clockQuantization49J/conversion-source/conversion_oracle49j.h"
#include "../../evidence/tvCamera49E/loader-source/authored_projection49e.h"
#include "../../tools/gameplay/input_shaping_oracle48v.h"
#include "../../tools/gameplay/camera_view_oracle49f.h"
#include "../../evidence/cameraView49F/tv-oracle/tv_view_oracle49f.h"
#include "frame_timing_oracle49i.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>

static std::uint64_t allocations=0;
void* operator new(std::size_t n){if(void* p=std::malloc(n?n:1)){++allocations;return p;}throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((noinline))
#endif
static void releaseAllocation(void* p) noexcept{std::free(p);}
void operator delete(void* p) noexcept{releaseAllocation(p);}
void operator delete[](void* p) noexcept{releaseAllocation(p);}
void operator delete(void* p,std::size_t) noexcept{releaseAllocation(p);}
void operator delete[](void* p,std::size_t) noexcept{releaseAllocation(p);}
using namespace rr::game;
using namespace rr::assets;
using U=std::uint32_t;using U64=std::uint64_t;using Vec=TVCameraVector49E;
namespace co=clock49j_conversion_oracle;
namespace fo=frame49i_oracle;
namespace tv=tv_view49f_oracle;
namespace cv=rr::test49f;
static U64 checks=0,hotCalls=0,inputInstructions=0,clockInstructions=0,cameraInstructions=0;
static const char* stage="initialization";
static void require(bool condition,const char* why){++checks;if(!condition)throw std::runtime_error(why);}
static U bits(float x){U word;std::memcpy(&word,&x,4);return word;}
static float number(U word){float x;std::memcpy(&x,&word,4);return x;}
static tv::V words(const Vec& v){return {{bits(v.x),bits(v.y),bits(v.z),bits(v.w)}};}
static cv::Vec4 values(const Vec& v){return {{v.x,v.y,v.z,v.w}};}
static Vec vector(const cv::Vec4& v){return {v[0],v[1],v[2],v[3]};}
template<class T>static bool same(const T& a,const T& b){return std::memcmp(&a,&b,sizeof(T))==0;}
template<std::size_t N>static void equalFloats(const std::array<float,N>& a,const std::array<float,N>& b,const char* why){for(unsigned k=0;k<N;++k)require(bits(a[k])==bits(b[k]),why);}
static void hot(U64 before){require(allocations==before,"native hot call allocated");++hotCalls;}
static fo::State sourceFrame(const FrameTimingState49I& s){return {s.current230,s.previous234,bits(s.raw250),bits(s.lastDispatched23C),s.phase22C,s.active11C,s.pending120};}

struct CameraFixture {
    std::array<TVCameraRecord49E,max_tv_cameras49e> records{};
    std::array<U,max_tv_camera_links49e> links{};
    std::array<tv::Record,max_tv_cameras49e> source{};
    TVCameraView49E view;
    void load(const char* path){
        std::ifstream file(path,std::ios::binary);require(bool(file),"cannot open authentic TA TV-camera configuration");
        const std::string text{std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
        TVCameraDiagnostic49E diagnostic;
        const auto count=allocations;
        const bool ok=loadTVCamera49E({text.data(),text.size()},{{records.data(),records.size()},{links.data(),links.size()}},view,diagnostic);
        hot(count);require(ok,"authentic TA configuration rejected");
        require(view.cameras().size==19&&view.links().size==20,"authentic TA camera/link counts differ");
        for(unsigned i=0;i<19;++i){
            const auto& r=records[i];auto& w=source[i].words;
            const auto p=words(r.position),l=words(r.look),u=words(r.up);
            for(unsigned k=0;k<4;++k){w[k]=p[k];w[k+4]=l[k];w[k+8]=u[k];}
            w[12]=bits(r.stationary);w[13]=bits(r.max_fov);w[14]=bits(r.min_fov);
            w[15]=bits(r.max_fov_distance);w[16]=bits(r.min_fov_distance);w[17]=U(r.initial_zone);
            w[18]=0x6a000048;w[19]=0x6a00004c;
            for(unsigned k=0;k<18;++k)require(w[k]==tv_camera49e_authored::record_words[i][k],"authentic camera record differs from retained source projection");
        }
        const auto& c=records[15];
        require(c.max_fov==75&&c.min_fov==45&&c.max_fov_distance==1750&&c.min_fov_distance==5&&c.probabilities.tv==70,"Camera16 sensitivity fixture changed");
    }
};
struct Branch {
    FrameTimingState49I frame{0,0,0,0,1,0,0};
    ControlInputStep48V controls{};
    TVCameraSelectorState49E selected{};
    TVCameraUpdateState49F update{};
    TVCameraRandom49E rng{1};
    CameraSceneState49F scene{};
    TVCameraUpdateOutput49F camera{};
    CameraViewOutput49F matrices{};
};
static void initialize(const CameraFixture& fixture,Branch& branch){
    // Explicit request for TV diagnostics. The authentic TV=70 does not choose
    // the mode here. One shared RNG is supplied at selection entry, seed=1.
    TVCameraSelection49E selection;
    const auto count=allocations;
    const bool ok=selectTVCamera49E(fixture.view,branch.selected,fixture.records[15].position,0,branch.rng,selection);
    hot(count);
    require(ok&&selection.changed&&branch.selected.current40==15&&branch.selected.stationary80==0,"Camera16 explicit initial selection failed");
    require(branch.selected.filterCurrent00==75&&branch.rng.raw==0x41c67ea6u,"selection initializer or shared RNG changed");
    // Aspect=1 is an explicit supplied projection fixture, not a recovered
    // viewport/global initializer. Default near/far/roll are source defaults.
    branch.scene.aspect1F8=1;
}
static tv::State sourceCamera(const Branch& b){
    const auto& s=b.selected;const auto& u=b.update;tv::State out;
    out.current40=s.current40;out.previous44=s.previous44;out.stationary80=s.stationary80;out.hold90=bits(s.hold90);out.positionA0=words(s.positionA0);
    out.fieldC0=bits(s.fieldC0);out.fieldC4=bits(s.fieldC4);out.fieldC8=bits(s.fieldC8);out.wordCC=s.wordCC;out.sourceD0=s.sourceD0;
    out.filterWords[0]=bits(s.filterCurrent00);out.filterWords[1]=bits(u.filterTau04);out.filterWords[2]=bits(u.filterBlend08);
    out.rawFov04=bits(u.rawFov04);out.cameraPosition10=words(u.cameraPosition10);out.playerPosition60=words(u.playerPosition60);out.playerVelocity70=words(u.playerVelocity70);out.shakeVectorB0=words(u.shakeVectorB0);return out;
}
static tv::Output sourceOutput(const TVCameraUpdateOutput49F& o){tv::Output out;out.mode=o.mode;out.position=words(o.position);out.target=words(o.target);out.look=words(o.look);out.up=words(o.up);out.fov=bits(o.fov);return out;}
static CameraViewOutput49F sourceView(const CameraSceneState49F& prior,const TVCameraUpdateOutput49F& o){
    cv::Scene scene;scene.position=values(o.position);scene.direction=values(prior.look);scene.up=values(prior.up);scene.roll=prior.rollRadians;scene.fov=prior.fovDegrees;
    const auto p=values(o.position),look=values(o.look),up=values(o.mode==0?o.up:prior.up),target=values(o.target);
    cv::sceneSet(scene,&p,o.mode==0?&look:nullptr,&up,&prior.rollRadians,&o.fov);
    CameraViewOutput49F out;out.targetAccepted=o.mode!=0&&cv::lookTarget(scene,target);out.fovWritten=o.fov!=0;out.position=vector(scene.position);out.sceneAfter=prior;
    out.sceneAfter.look=vector(scene.direction);out.sceneAfter.up=vector(scene.up);out.sceneAfter.fovDegrees=scene.fov;
    out.view=cv::view(scene.position,scene.direction,scene.up,scene.roll);
    volatile float angle=number(0x3c8efa34u)*scene.fov;volatile float inverse=1.0f/prior.aspect1F8;
    out.projection=cv::projection(prior.nearPlane,prior.farPlane,angle,inverse);out.combined=cv::multiply(out.view,out.projection);return out;
}
static void cameraStep(const CameraFixture& fixture,Branch& b,float elapsed){
    const auto position=fixture.records[15].position;
    // Identity parent is the supplied fixture assumption. Local scene XYZ and
    // resolved player XYZ agree here only because that parent is identity.
    const Vec player{position.x+100,position.y-20,position.z+600,1};
    const std::array<TVCameraSceneSample49F,1> completeScene{{{0,player,{100,0,0,1}}}};
    const TVCameraUpdateInput49F input{player,{100,0,0,1},0,elapsed,{completeScene.data(),completeScene.size()}};
    auto copiedOutput=b.camera;copiedOutput.look=b.scene.look;copiedOutput.up=b.scene.up;
    tv::Input oracleInput;oracleInput.resolvedPlayerPosition=words(player);oracleInput.bodyVelocity=words(input.bodyVelocity);oracleInput.elapsed=bits(elapsed);oracleInput.targetYOffset=0;
    oracleInput.scene.push_back({0,words(player),words(input.bodyVelocity)});
    const auto expected=tv::update(fixture.source.data(),fixture.view.cameras().size,sourceCamera(b),oracleInput,sourceOutput(copiedOutput),b.rng.raw);
    cameraInstructions+=expected.executed.size();
    const auto priorScene=b.scene;const auto priorRng=b.rng.raw;const auto count=allocations;
    const bool ok=updateTVCameraView49F(fixture.view,b.selected,b.update,input,b.rng,b.scene,b.camera,b.matrices,{0,0});
    hot(count);require(ok,"connected numeric TV view rejected");
    require(tv::sameState(sourceCamera(b),expected.state)&&tv::sameOutput(sourceOutput(b.camera),expected.output)&&b.rng.raw==expected.rngRaw,"connected TV state differs from original words");
    require(expected.randomCalls==0&&b.rng.raw==priorRng&&b.selected.wordCC==0&&b.selected.sourceD0==-1,"below-170 scene fixture triggered shake or RNG");
    const auto expectedView=sourceView(priorScene,b.camera);
    equalFloats(b.matrices.view,expectedView.view,"connected view matrix differs from original words");
    equalFloats(b.matrices.projection,expectedView.projection,"connected projection differs from original words");
    equalFloats(b.matrices.combined,expectedView.combined,"connected combined matrix differs from original words");
    require(words(b.scene.look)==words(expectedView.sceneAfter.look)&&words(b.scene.up)==words(expectedView.sceneAfter.up)&&bits(b.scene.fovDegrees)==bits(expectedView.sceneAfter.fovDegrees),"connected scene state differs from original words");
    require(b.matrices.targetAccepted==expectedView.targetAccepted&&b.matrices.fovWritten==expectedView.fovWritten,"connected view flags differ");
    require(b.camera.mode==1&&b.selected.current40==15,"numeric TV update reselected camera or mode");
}
static ControlChannels48B suppliedChannels(unsigned n){
    // Numerical channel fixture only: no action/button/device mapping inferred.
    const float sign=(n/7)%2?-1.0f:1.0f;
    return {{sign,(n/11)%2?.625f:-.625f,(n/5)%2?-.75f:.75f,(n/13)%2?1.f:-1.f}};
}
static void inputStep(Branch& b,const ControlChannels48B& channels,float elapsed){
    const ControlRateLimits48B rates{4,3};original48v::Machine original;
    const auto expected=original.run(channels,b.controls.shaping.smoothedChannel0D28,0,b.controls.filtered,rates,elapsed);
    inputInstructions+=original.steps;ControlInputStep48V next{};const auto count=allocations;
    const bool ok=stepControlInputs48V(channels,b.controls.shaping.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,b.controls.filtered,rates,elapsed,next);
    hot(count);require(ok&&same(next,expected),"connected input differs from original words");b.controls=next;
}
// This is an explicit caller-owned phase fixture. Supplied stage observations
// exercise the recovered ordering, without implementing or claiming to invoke
// manager128080, pre-dispatch245030/global action, root callback, or267BE0.
// The input/camera calls below are diagnostic numeric consumers, not callbacks.
static bool consumePrepared(const CameraFixture& fixture,Branch& b,const FrameElapsed49I& elapsed,unsigned n){
    const U capturedFlagsAfterSuppliedManager=(n!=0&&n%37==0)?8u:0u;
    // A later supplied flag value differs at selected boundaries. The captured
    // decision remains authoritative, and no cap is recomputed after begin.
    const U laterSuppliedFlags=(n%17==0)?capturedFlagsAfterSuppliedManager^8u:capturedFlagsAfterSuppliedManager;
    (void)laterSuppliedFlags;
    const auto before=sourceFrame(b.frame);auto expected=before;
    const auto beforeFiltered=b.controls.filtered;const U beforeFov=bits(b.selected.filterCurrent00);
    const auto count=allocations;const auto entry=enterFrameSimulation49I(b.frame,capturedFlagsAfterSuppliedManager,elapsed.elapsedSeconds);hot(count);
    const bool ready=(capturedFlagsAfterSuppliedManager&8u)==0;
    require((entry==FrameSimulationEntry49I::Ready)==ready,"captured manager flags changed entry decision");
    if(ready){
        fo::enter(expected,bits(elapsed.elapsedSeconds));require(same(sourceFrame(b.frame),expected),"connected phase entry differs from original words");
        inputStep(b,suppliedChannels(n),elapsed.elapsedSeconds);cameraStep(fixture,b,elapsed.elapsedSeconds);
        if(elapsed.elapsedSeconds==0)require(same(b.controls.filtered,beforeFiltered)&&bits(b.camera.fov)==beforeFov,"zero elapsed advanced rate filter or camera FOV");
        // Supplied root-return boundary, not an emulated callback or schedule.
        const auto old=allocations;leaveFrameSimulation49I(b.frame);hot(old);fo::leave(expected);
    }else require(entry==FrameSimulationEntry49I::Skipped,"fixture elapsed unexpectedly rejected");
    require(same(sourceFrame(b.frame),expected),"connected return/skip phase differs");
    // Explicit supplied tail-stage completion. No opaque effects are inferred.
    const auto old=allocations;finishFrameTiming49I(b.frame);hot(old);fo::finish(expected);
    require(same(sourceFrame(b.frame),expected),"connected live tail differs from original words");return ready;
}
static U idealTimestamp(U64 ticks){
    // TEST-ONLY mathematical floor(ticks/576), with final uint32 saturation.
    // This deliberately omits source float quantization. It is not an OS clock
    // and never enters production code. All other fixtures and consumers match.
    const U64 ms=ticks/576u;return ms>0xffffffffull?0xffffffffu:U(ms);
}
struct Scenario {const char* name;U64 initialTicks;unsigned frames;U cap;unsigned kind;bool synthetic;};
static constexpr std::array<Scenario,13> scenarios{{
    {"zero_epoch_bootstrap",0,32,0,0,false},
    {"frequent_u16_wrap",100000,96,0,1,false},
    {"repeated_sample_pending_zero_stall",100000,64,0,2,false},
    {"multi_wrap_stall_cap_006",100000,64,0,3,false},
    {"multi_wrap_stall_cap_003333",100000,64,1,3,false},
    {"long_near_2pow24",(U64{1}<<24)-600,128,0,4,false},
    {"long_near_2pow33",(U64{1}<<33)-40000,128,0,5,false},
    {"long_near_2pow40_cap_006",(U64{1}<<40)-400000,128,0,5,false},
    {"long_near_2pow40_cap_003333",(U64{1}<<40)-400000,128,1,5,false},
    {"cross_timestamp_saturation",2473901031424ull-16385,64,0,6,false},
    {"synthetic_u64_accumulator_wrap",~U64{0}-10000,8,0,5,true},
    {"synthetic_large_pending_signed_wrap",100000,8,0,7,true},
    {"synthetic_helper_double_rounding",9007200865353727ull,3,0,8,true}
}};
static U increment(const Scenario& s,unsigned n){
    if(n==0)return 0;
    if(s.kind==0){constexpr U seq[]={0,1,574,1,576,1,9216,0};return seq[(n-1)%8];}
    if(s.kind==1)return 48000+(n%3)*576;
    if(s.kind==2)return n>=12&&n<=24?0:9216;
    if(s.kind==3)return n==9?7*65536+12345:n==29?1152000:n==30?0:9216;
    if(s.kind==4)return 575;
    if(s.kind==6)return n==1?16384:n==2?1:n==3?0:9216;
    if(s.kind==8)return 0;
    return 9216+(n%3)*19;
}
struct Summary {
    U64 rows=0,ready=0,wraps=0,multiWraps=0,zeroDelta=0,sourceCaps=0,idealCaps=0,saturated=0;
    U64 stampDifferences=0,elapsedDifferences=0,inputDifferences=0,fovDifferences=0,projectionDifferences=0;
    double maxInputDifference=0,maxFovDifference=0,maxProjectionDifference=0;
    U64 firstSaturatedTicks=0,hash=14695981039346656037ull;
};
static void hashWord(U64& hash,U64 word,unsigned bytes=4){for(unsigned k=0;k<bytes;++k){hash^=(word>>(k*8))&255u;hash*=1099511628211ull;}}
struct Row {
    U64 ticks=0;U sample=0,pending=0,delta=0,rounded=0,scaled=0,sourceStamp=0,idealStamp=0,ready=0,capturedFlags=0,laterFlags=0;
    FrameElapsed49I sourceElapsed{},idealElapsed{};float sourceRaw=0,idealRaw=0;
    Branch source{},ideal{};
};
static void hashBranch(U64& h,const Branch& b){
    for(float f:b.controls.filtered.values)hashWord(h,bits(f));
    hashWord(h,bits(b.controls.shaping.smoothedChannel0D28));
    for(float f:b.controls.shaping.targets.values)hashWord(h,bits(f));
    hashWord(h,bits(b.update.rawFov04));hashWord(h,bits(b.update.filterBlend08));hashWord(h,bits(b.camera.fov));hashWord(h,U(b.camera.mode));hashWord(h,b.rng.raw);
    for(const auto* v:{&b.camera.position,&b.camera.target,&b.scene.look,&b.scene.up})for(U w:words(*v))hashWord(h,w);
    for(const auto* m:{&b.matrices.view,&b.matrices.projection,&b.matrices.combined})for(float f:*m)hashWord(h,bits(f));
    hashWord(h,b.frame.phase22C);hashWord(h,bits(b.frame.lastDispatched23C));
}
static void hashRow(Summary& out,const Row& row){
    hashWord(out.hash,row.ticks,8);for(U x:{row.sample,row.pending,row.delta,row.rounded,row.scaled,row.sourceStamp,row.idealStamp,row.ready,row.capturedFlags,row.laterFlags,row.sourceElapsed.deltaMilliseconds,row.idealElapsed.deltaMilliseconds,bits(row.sourceElapsed.elapsedSeconds),bits(row.idealElapsed.elapsedSeconds),bits(row.sourceRaw),bits(row.idealRaw)})hashWord(out.hash,x);
    hashBranch(out.hash,row.source);hashBranch(out.hash,row.ideal);
}
static void traceHeader(std::ostream& out){
    out<<"scenario\tsynthetic\tframe\tticks_u64\tsample_u16\tpending_u32\tdelta_counter_hex\trounded_tick_hex\tscaled_ms_hex\tsource_stamp\tideal_stamp\tsource_delta_ms\tideal_delta_ms\tsource_elapsed_hex\tideal_elapsed_hex\tsource_raw_hex\tideal_raw_hex\tcap_word\troot_ready\tcaptured_flags_after_manager\tlater_supplied_flags";
    for(const char* label:{"source","ideal"}){
        for(unsigned k=0;k<4;++k)out<<'\t'<<label<<"_input"<<k<<"_hex";
        out<<'\t'<<label<<"_smoothed0_hex";
        for(unsigned k=0;k<4;++k)out<<'\t'<<label<<"_target"<<k<<"_hex";
        for(const char* field:{"raw_fov","fov_blend","fov","mode","rng"})out<<'\t'<<label<<'_'<<field<<"_hex";
        for(const char* v:{"position","target","scene_look","scene_up"})for(const char* axis:{"x","y","z","w"})out<<'\t'<<label<<'_'<<v<<'_'<<axis<<"_hex";
        for(const char* matrix:{"view","projection","combined"})for(unsigned k=0;k<16;++k)out<<'\t'<<label<<'_'<<matrix<<'_'<<k<<"_hex";
        out<<'\t'<<label<<"_phase_hex\t"<<label<<"_last_dispatch_hex";
    }out<<'\n';
}
static void hex(std::ostream& out,U word){out<<'\t'<<std::hex<<std::setw(8)<<std::setfill('0')<<word<<std::dec<<std::setfill(' ');}
static void traceBranch(std::ostream& out,const Branch& b){
    for(float f:b.controls.filtered.values)hex(out,bits(f));
    hex(out,bits(b.controls.shaping.smoothedChannel0D28));
    for(float f:b.controls.shaping.targets.values)hex(out,bits(f));
    for(U word:{bits(b.update.rawFov04),bits(b.update.filterBlend08),bits(b.camera.fov),U(b.camera.mode),b.rng.raw})hex(out,word);
    for(const auto* v:{&b.camera.position,&b.camera.target,&b.scene.look,&b.scene.up})for(U word:words(*v))hex(out,word);
    for(const auto* m:{&b.matrices.view,&b.matrices.projection,&b.matrices.combined})for(float f:*m)hex(out,bits(f));
    hex(out,b.frame.phase22C);hex(out,bits(b.frame.lastDispatched23C));
}
static void traceRow(std::ostream& out,const Scenario& s,unsigned n,const Row& r){
    out<<s.name<<'\t'<<s.synthetic<<'\t'<<n<<'\t'<<r.ticks<<'\t'<<r.sample<<'\t'<<r.pending;hex(out,r.delta);hex(out,r.rounded);hex(out,r.scaled);
    out<<'\t'<<r.sourceStamp<<'\t'<<r.idealStamp<<'\t'<<r.sourceElapsed.deltaMilliseconds<<'\t'<<r.idealElapsed.deltaMilliseconds;
    hex(out,bits(r.sourceElapsed.elapsedSeconds));hex(out,bits(r.idealElapsed.elapsedSeconds));hex(out,bits(r.sourceRaw));hex(out,bits(r.idealRaw));out<<'\t'<<s.cap<<'\t'<<r.ready<<'\t'<<r.capturedFlags<<'\t'<<r.laterFlags;
    traceBranch(out,r.source);traceBranch(out,r.ideal);out<<'\n';
}
static Summary run(const CameraFixture& fixture,const Scenario& s,std::ostream* trace){
    stage=s.name;ClockCounterState49J clock{s.initialTicks,0,65000};Branch source,ideal;initialize(fixture,source);initialize(fixture,ideal);Summary result;
    for(unsigned n=0;n<s.frames;++n){
        Row row;const U step=increment(s,n);const U timerTotal=U(clock.previousCount)+step;
        row.sample=timerTotal&65535u;row.pending=timerTotal>>16;
        if(s.kind==7&&n){constexpr U pending[]={0x8000,0xffff,0x10000,0x10001,0xffffffff,0x7fff,0};row.pending=pending[n-1];row.sample=n%2?clock.previousCount:U(clock.previousCount-1)&65535u;}
        clock.pendingOverflows=row.pending;const U priorCount=clock.previousCount;const U64 priorTicks=clock.accumulatedTicks;
        U effectivePending=row.pending;if(row.sample<priorCount&&!effectivePending)effectivePending=1;
        const U expectedDelta=U(row.sample-priorCount)+U(effectivePending<<16);
        const U64 signedDelta=(expectedDelta&0x80000000u)?(0xffffffff00000000ull|expectedDelta):expectedDelta;
        const U64 expectedTicks=priorTicks+signedDelta;
        auto expectedFrame=sourceFrame(source.frame);co::Oracle clockOracle;const auto wantedClock=clockOracle.quantize(expectedTicks,std::uint16_t(row.sample));clockInstructions+=clockOracle.steps;
        const auto wantedElapsed=fo::begin(expectedFrame,wantedClock.uint32timestamp,s.cap);
        ClockFrameOutput49J produced{};const auto count=allocations;
        const bool ok=beginFrameFromTimerSample49J(clock,source.frame,std::uint16_t(row.sample),s.cap,produced);hot(count);
        require(ok&&produced.deltaCounterBits==expectedDelta&&clock.accumulatedTicks==expectedTicks&&clock.pendingOverflows==0&&clock.previousCount==row.sample,"connected timer accumulation differs");
        require(produced.quantization.roundedTickBits==wantedClock.roundedTickBits&&produced.quantization.scaledMillisecondBits==wantedClock.scaledMillisecondBits&&produced.quantization.uint32timestamp==wantedClock.uint32timestamp,"connected source quantization differs from original words");
        require(same(sourceFrame(source.frame),expectedFrame)&&produced.elapsed.deltaMilliseconds==wantedElapsed.deltaMilliseconds&&bits(produced.elapsed.elapsedSeconds)==wantedElapsed.elapsedBits,"connected 49I begin differs from original words");
        row.ticks=clock.accumulatedTicks;row.delta=produced.deltaCounterBits;row.rounded=produced.quantization.roundedTickBits;row.scaled=produced.quantization.scaledMillisecondBits;row.sourceStamp=produced.quantization.uint32timestamp;row.idealStamp=idealTimestamp(row.ticks);row.sourceElapsed=produced.elapsed;row.sourceRaw=source.frame.raw250;
        auto expectedIdeal=sourceFrame(ideal.frame);const auto wantedIdeal=fo::begin(expectedIdeal,row.idealStamp,s.cap);const auto old=allocations;
        const bool idealOk=beginFrameTiming49I(ideal.frame,row.idealStamp,s.cap,row.idealElapsed);hot(old);row.idealRaw=ideal.frame.raw250;
        require(idealOk&&same(sourceFrame(ideal.frame),expectedIdeal)&&row.idealElapsed.deltaMilliseconds==wantedIdeal.deltaMilliseconds&&bits(row.idealElapsed.elapsedSeconds)==wantedIdeal.elapsedBits,"test-only ideal comparator changed 49I behavior");
        row.capturedFlags=(n!=0&&n%37==0)?8u:0u;row.laterFlags=(n%17==0)?row.capturedFlags^8u:row.capturedFlags;
        row.ready=consumePrepared(fixture,source,row.sourceElapsed,n)?1u:0u;require(row.ready==U(consumePrepared(fixture,ideal,row.idealElapsed,n)),"comparison fixture diverged in supplied flags");row.source=source;row.ideal=ideal;
        ++result.rows;result.ready+=row.ready;result.wraps+=row.sample<priorCount;result.multiWraps+=row.pending>1;result.zeroDelta+=row.sourceElapsed.deltaMilliseconds==0;
        const U cap=s.cap?0x3d08850au:0x3d75c28fu;result.sourceCaps+=bits(row.sourceElapsed.elapsedSeconds)==cap;result.idealCaps+=bits(row.idealElapsed.elapsedSeconds)==cap;
        if(row.sourceStamp==0xffffffffu){if(!result.saturated)result.firstSaturatedTicks=row.ticks;++result.saturated;}
        result.stampDifferences+=row.sourceStamp!=row.idealStamp;result.elapsedDifferences+=bits(row.sourceElapsed.elapsedSeconds)!=bits(row.idealElapsed.elapsedSeconds);
        bool inputDiff=false,projectionDiff=false;
        for(unsigned k=0;k<4;++k){const float a=source.controls.filtered.values[k],b=ideal.controls.filtered.values[k];inputDiff|=bits(a)!=bits(b);result.maxInputDifference=std::max(result.maxInputDifference,std::fabs(double(a)-double(b)));}
        for(unsigned k=0;k<16;++k){const float a=source.matrices.projection[k],b=ideal.matrices.projection[k];projectionDiff|=bits(a)!=bits(b);result.maxProjectionDifference=std::max(result.maxProjectionDifference,std::fabs(double(a)-double(b)));}
        result.inputDifferences+=inputDiff;result.fovDifferences+=bits(source.camera.fov)!=bits(ideal.camera.fov);result.projectionDifferences+=projectionDiff;result.maxFovDifference=std::max(result.maxFovDifference,std::fabs(double(source.camera.fov)-double(ideal.camera.fov)));
        equalFloats(source.matrices.view,ideal.matrices.view,"identical supplied pose unexpectedly changed view orientation between clocks");
        require(source.rng.raw==ideal.rng.raw&&bits(source.update.rawFov04)==bits(ideal.update.rawFov04),"clock comparison changed RNG or raw camera FOV fixture");
        if(s.kind==2&&n>=12&&n<=24)require(row.delta==0&&row.sourceElapsed.deltaMilliseconds==0,"repeated sample/pending0 stall inferred missing time");
        if(s.kind==3&&(n==9||n==29))require(bits(row.sourceElapsed.elapsedSeconds)==cap,"multi-wrap stall failed original upper cap");
        if(s.kind==3&&n==31)require(row.sourceElapsed.deltaMilliseconds==16,"post-stall frame invented catch-up");
        if(s.kind==6&&n==1)require(row.ticks==2473901031423ull&&row.sourceStamp==0xfffffe00u&&row.scaled==0x4f7ffffeu,"last unsaturated tick differs");
        if(s.kind==6&&n==2)require(row.ticks==2473901031424ull&&row.sourceStamp==0xffffffffu&&row.scaled==0x4f800000u&&row.sourceElapsed.deltaMilliseconds==511,"first saturated tick or 511ms jump differs");
        if(s.kind==6&&n>2)require(row.sourceElapsed.deltaMilliseconds==0,"saturated timestamp advanced instead of freezing");
        if(s.kind==8)require(row.rounded==0x5a000002u&&row.sourceStamp==0xffffffffu&&row.idealStamp==0xffffffffu,"rare helper counterexample differs or escaped saturation");
        hashRow(result,row);if(trace)traceRow(*trace,s,n,row);
    }
    return result;
}
static void summary(std::ostream& out,const std::array<Summary,scenarios.size()>& all){
    out<<std::setprecision(12)<<"{\n  \"schema\": \"clock-timing-connected49j-v1\",\n  \"scope\": \"Supplied timer samples and phase observations; recovered source numeric input and Camera16 TV view. No OS clock, rendered images, integrated gameplay, opaque callbacks, IRQ atomicity, or catch-up.\",\n  \"comparison\": \"TEST ONLY floor(accumulatedTicks / 576), saturated to uint32, then the identical 49I and consumer fixtures.\",\n  \"camera\": {\"authored_index\":15,\"name\":\"Camera16\",\"max_fov\":75,\"min_fov\":45,\"max_distance\":1750,\"min_distance\":5,\"authored_tv_probability\":70,\"mode\":\"explicit TV diagnostic\",\"parent\":\"supplied identity\",\"resolved_offset\":[100,-20,600],\"velocity\":[100,0,0],\"complete_local_scene_entries\":1,\"shared_rng_seed\":1,\"rng_after_selection\":1103527590,\"aspect_fixture\":1},\n  \"trace_float_format\": \"exact IEEE binary32 words in hexadecimal; view/projection/combined each use original flat 16-word row-vector storage\",\n  \"repeatable\": true,\n  \"native_hot_call_allocations\": 0,\n  \"scenarios\": [\n";
    for(unsigned k=0;k<all.size();++k){const auto& r=all[k];const auto& s=scenarios[k];
        out<<"    {\"name\":\""<<s.name<<"\",\"synthetic_edge\":"<<(s.synthetic?"true":"false")<<",\"initial_ticks\":"<<s.initialTicks<<",\"frames\":"<<r.rows<<",\"ready_frames\":"<<r.ready<<",\"sample_wraps\":"<<r.wraps<<",\"multi_wrap_feeds\":"<<r.multiWraps<<",\"zero_elapsed_frames\":"<<r.zeroDelta<<",\"source_capped_frames\":"<<r.sourceCaps<<",\"ideal_capped_frames\":"<<r.idealCaps<<",\"saturated_frames\":"<<r.saturated<<",\"first_saturated_ticks\":"<<r.firstSaturatedTicks<<",\"timestamp_difference_frames\":"<<r.stampDifferences<<",\"elapsed_difference_frames\":"<<r.elapsedDifferences<<",\"input_difference_frames\":"<<r.inputDifferences<<",\"fov_difference_frames\":"<<r.fovDifferences<<",\"projection_difference_frames\":"<<r.projectionDifferences<<",\"max_abs_input_difference\":"<<r.maxInputDifference<<",\"max_abs_fov_difference\":"<<r.maxFovDifference<<",\"max_abs_projection_difference\":"<<r.maxProjectionDifference<<",\"numeric_fnv1a64\":\""<<std::hex<<std::setw(16)<<std::setfill('0')<<r.hash<<std::dec<<std::setfill(' ')<<"\"}"<<(k+1==all.size()?"\n":",\n");
    }out<<"  ]\n}\n";
}
int main(int argc,char** argv){
    try{
        require(argc==1||argc==2||argc==3,"usage: clock_timing_diagnostics49j [TA_CAMERA_PATH [OUTPUT_PREFIX]]");
        require(std::fesetround(FE_TONEAREST)==0,"nearest-even unavailable");
        const U64 helperCounterexample=9007200865353727ull;
        // A host uint64-to-float cast can double-round at x87's intermediate
        // precision on IA32. This comparator requires one nearest-even rounding,
        // independently of the host conversion ABI and the source helper.
        const U directIdealRounded=fo::detail::roundDyadic(helperCounterexample,0);
        require(directIdealRounded==0x5a000001u,"test-only direct integer rounding counterexample changed");
        CameraFixture fixture;fixture.load(argc>1?argv[1]:"evidence/tvCamera49E/authored/taTVCam.pos");
        std::ofstream trace;if(argc==3){trace.open(std::string(argv[2])+".trace.tsv");require(bool(trace),"cannot open trace output");traceHeader(trace);}
        std::array<Summary,scenarios.size()> result{};
        for(unsigned k=0;k<scenarios.size();++k){result[k]=run(fixture,scenarios[k],argc==3?&trace:nullptr);const auto repeat=run(fixture,scenarios[k],nullptr);require(result[k].hash==repeat.hash,"repeated connected numeric trace differs");}
        require(result[1].wraps>60,"frequent-wrap coverage too small");require(result[2].zeroDelta>=14,"stall coverage too small");
        require(result[5].stampDifferences&&result[5].elapsedDifferences,"2^24 precision boundary not exercised");
        require(result[6].elapsedDifferences&&result[6].inputDifferences&&result[6].fovDifferences,"2^33 connected clock sensitivity missing");
        require(result[7].zeroDelta>80&&result[7].sourceCaps&&result[7].maxFovDifference>1&&result[7].maxInputDifference>.1,"2^40 coarsening failed to affect connected consumers");
        require(result[9].saturated&&result[9].elapsedDifferences&&result[9].inputDifferences,"saturation crossing did not reach consumers");
        require(!result[12].elapsedDifferences&&!result[12].inputDifferences&&!result[12].fovDifferences,"saturated helper counterexample fabricated downstream timing effects");
        if(argc==3){trace.close();require(bool(trace),"trace write failed");std::ofstream out(std::string(argv[2])+".summary.json");require(bool(out),"cannot open summary output");summary(out,result);out.close();require(bool(out),"summary write failed");}
        U64 frames=0;for(const auto& r:result)frames+=r.rows;
        std::cout<<"clock_timing_diagnostics49j: "<<checks<<" checks passed; "<<scenarios.size()<<" scenarios, "<<frames<<" paired frames repeated identically; "<<hotCalls<<" allocation-free native calls; "<<clockInstructions<<" clock, "<<inputInstructions<<" input, "<<cameraInstructions<<" TV original instructions; complete actual matrix traces; no OS clock or invented catch-up\n";return 0;
    }catch(const std::exception& e){std::cerr<<"clock_timing_diagnostics49j FAILED at "<<stage<<": "<<e.what()<<'\n';return 1;}
}
