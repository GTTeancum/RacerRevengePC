#include "../../src/game/camera_frame49f.h"
#include "../../evidence/cameraView49F/tv-oracle/tv_view_oracle49f.h"
#include "../../tools/gameplay/camera_view_oracle49f.h"
#include "../../evidence/tvCamera49E/loader-source/authored_projection49e.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

// Successful native hot-call allocations only. No allocator failure injection,
// ownership/lifetime probes, address faults, misalignment or invalid EE memory.
static std::uint64_t allocations=0;
void* operator new(std::size_t n){if(void* p=std::malloc(n?n:1)){++allocations;return p;}throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}
void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
using namespace rr::assets;
using namespace rr::game;
namespace tv=tv_view49f_oracle;
namespace cv=rr::test49f;
namespace authored=tv_camera49e_authored;
using U=std::uint32_t;using I=std::int32_t;using Vec=TVCameraVector49E;
using Selected=TVCameraSelectorState49E;using Update=TVCameraUpdateState49F;
using Input=TVCameraUpdateInput49F;using Output=TVCameraUpdateOutput49F;
using Scene=CameraSceneState49F;using View=CameraViewOutput49F;
static std::uint64_t checks=0,tvCases=0,viewCases=0,frameCases=0,rejections=0,hotCalls=0;
static std::uint64_t authoredFrames=0,trajectoryFrames=0,shakeCases=0,contextCases=0,randomDraws=0;
static std::uint64_t tvInstructions=0,consumerInstructions=0;
static const char* stage="startup";
static void require(bool b,const char* s){++checks;if(!b)throw std::runtime_error(s);}
static U bits(float f){U w;std::memcpy(&w,&f,4);return w;}
static float number(U w){float f;std::memcpy(&f,&w,4);return f;}
static tv::V words(const Vec& v){return {{bits(v.x),bits(v.y),bits(v.z),bits(v.w)}};}
static cv::Vec4 values(const Vec& v){return {{v.x,v.y,v.z,v.w}};}
static Vec vector(const cv::Vec4& v){return {v[0],v[1],v[2],v[3]};}
template<std::size_t N>static void equalFloats(const std::array<float,N>& a,const std::array<float,N>& b,const char* why){for(unsigned i=0;i<N;++i)require(bits(a[i])==bits(b[i]),why);}
static void equalVec(const Vec& a,const Vec& b,const char* why){require(words(a)==words(b),why);}
template<class T>static std::array<unsigned char,sizeof(T)> snapshot(const T& t){std::array<unsigned char,sizeof(T)> b{};std::memcpy(b.data(),&t,sizeof(T));return b;}
template<std::size_t N>static unsigned covered(const std::array<std::uint64_t,N>& a){unsigned n=0;for(auto x:a)n+=x!=0;return n;}
#define TV_BLOCKS(X) X(update) X(base) X(target) X(position) X(fov) X(scene) X(local) X(construct) X(rng)
#define TV_VISITS(N) static std::array<std::uint64_t,std::size(tv_view49f_original::N##_words)> tv_##N{};
TV_BLOCKS(TV_VISITS)
#undef TV_VISITS
#define CV_BLOCKS(X) X(scene_set) X(look_target) X(view) X(projection) X(roll) X(tan) X(sincos) X(zero_matrix) X(multiply)
#define CV_VISITS(N) static std::array<std::uint64_t,std::size(cv::frozen::N)> cv_##N{};
CV_BLOCKS(CV_VISITS)
#undef CV_VISITS
static void recordTV(const tv::Result& r){for(U pc:r.executed){++tvInstructions;
#define TV_HIT(N) if(pc>=tv_view49f_original::N##_start&&pc<tv_view49f_original::N##_start+4*tv_##N.size()){++tv_##N[(pc-tv_view49f_original::N##_start)/4];continue;}
TV_BLOCKS(TV_HIT)
#undef TV_HIT
require(false,"TV oracle executed outside frozen numeric closure");}randomDraws+=r.randomCalls;}
static void recordConsumer(U pc){++consumerInstructions;
#define CV_HIT(N) if(pc>=cv::frozen::N##_start&&pc<cv::frozen::N##_start+4*cv_##N.size()){++cv_##N[(pc-cv::frozen::N##_start)/4];return;}
CV_BLOCKS(CV_HIT)
#undef CV_HIT
require(false,"consumer oracle executed outside frozen numeric closure");}
struct Fixture {
    std::array<TVCameraRecord49E,max_tv_cameras49e> records{};
    std::array<U,max_tv_camera_links49e> links{};TVCameraView49E view;
    std::array<tv::Record,max_tv_cameras49e> source{};
    void load(const std::string& text,bool authentic=false){
        TVCameraDiagnostic49E d;require(loadTVCamera49E({text.data(),text.size()},{{records.data(),records.size()},{links.data(),links.size()}},view,d),"fixture loader rejected");
        for(unsigned i=0;i<view.cameras().size;++i){const auto& r=records[i];auto& w=source[i].words;
            const auto p=words(r.position),l=words(r.look),u=words(r.up);
            for(unsigned k=0;k<4;++k){w[k]=p[k];w[k+4]=l[k];w[k+8]=u[k];}
            w[12]=bits(r.stationary);w[13]=bits(r.max_fov);w[14]=bits(r.min_fov);w[15]=bits(r.max_fov_distance);w[16]=bits(r.min_fov_distance);w[17]=U(r.initial_zone);w[18]=0x6a000048;w[19]=0x6a00004c;
            if(authentic)for(unsigned k=0;k<18;++k){require(w[k]==authored::record_words[i][k],"TA complete authored numeric record mismatch");w[k]=authored::record_words[i][k];}
        }
    }
};
static std::string oneCamera(const std::string& fields=""){return "[CameraPositions]\nNumberOfCameras=1\n[Camera1]\nNumNextCameras=1\nNextCamera1=1\n"+fields;}
static Selected selected(const Fixture& f,unsigned index=0){Selected s;s.current40=I(index);s.previous44=77;s.hold90=.37f;s.positionA0=f.records[index].position;s.filterCurrent00=f.records[index].max_fov;return s;}
static Output output(){Output o;o.mode=-73;o.position={11,13,17,19};o.target={23,29,31,37};o.look={41,43,47,53};o.up={59,61,67,71};o.fov=79;return o;}
static Input input(){Input i;i.resolvedPlayerPosition={100,20,70,7};i.bodyVelocity={90,-3,7,11};i.targetYOffset=5;i.elapsed=.016f;return i;}
static tv::State sourceState(const Selected& s,const Update& u){tv::State t;t.current40=s.current40;t.previous44=s.previous44;t.stationary80=s.stationary80;t.hold90=bits(s.hold90);t.positionA0=words(s.positionA0);t.fieldC0=bits(s.fieldC0);t.fieldC4=bits(s.fieldC4);t.fieldC8=bits(s.fieldC8);t.wordCC=s.wordCC;t.sourceD0=s.sourceD0;t.filterWords[0]=bits(s.filterCurrent00);t.filterWords[1]=bits(u.filterTau04);t.filterWords[2]=bits(u.filterBlend08);t.rawFov04=bits(u.rawFov04);t.cameraPosition10=words(u.cameraPosition10);t.playerPosition60=words(u.playerPosition60);t.playerVelocity70=words(u.playerVelocity70);t.shakeVectorB0=words(u.shakeVectorB0);return t;}
static tv::Output sourceOutput(const Output& o){tv::Output t;t.mode=o.mode;t.position=words(o.position);t.target=words(o.target);t.look=words(o.look);t.up=words(o.up);t.fov=bits(o.fov);return t;}
static tv::Input sourceInput(const Input& i){tv::Input t;t.resolvedPlayerPosition=words(i.resolvedPlayerPosition);t.bodyVelocity=words(i.bodyVelocity);t.targetYOffset=bits(i.targetYOffset);t.elapsed=bits(i.elapsed);for(const auto& s:i.sceneSamples)t.scene.push_back({s.inactive40,words(s.localPosition),words(s.velocity)});return t;}
static tv::Result originalTV(const Fixture& f,const Selected& s,const Update& u,const Input& i,const TVCameraRandom49E& r,const Output& o){auto t=tv::update(f.source.data(),f.view.cameras().size,sourceState(s,u),sourceInput(i),sourceOutput(o),r.raw);recordTV(t);return t;}
static void equalTV(const Selected& s,const Update& u,const Output& o,const TVCameraRandom49E& r,const tv::Result& want){require(tv::sameState(sourceState(s,u),want.state),"TV full state differs from original words");require(tv::sameOutput(sourceOutput(o),want.output),"TV full output differs from original words");require(r.raw==want.rngRaw,"TV shared RNG differs from original words");require(want.randomCalls==0||want.randomCalls==3,"TV original RNG count outside zero or three");}
static tv::Result compareTV(const Fixture& f,Selected& s,Update& u,const Input& i,TVCameraRandom49E& r,Output& o){auto want=originalTV(f,s,u,i,r,o);auto before=allocations;const bool ok=updateTVCamera49F(f.view,s,u,i,r,o);auto after=allocations;require(ok,"finite TV primitive rejected");require(before==after,"successful TV primitive allocated");++hotCalls;++tvCases;equalTV(s,u,o,r,want);return want;}
static View originalView(const Scene& prior,const Output& o){cv::Scene s;s.position=values(o.position);s.direction=values(prior.look);s.up=values(prior.up);s.roll=prior.rollRadians;s.fov=prior.fovDegrees;
    const auto p=values(o.position),look=values(o.look),up=values(o.mode==0?o.up:prior.up),target=values(o.target);
    cv::sceneSet(s,&p,o.mode==0?&look:nullptr,&up,&prior.rollRadians,&o.fov);
    View v;v.targetAccepted=o.mode!=0&&cv::lookTarget(s,target);v.fovWritten=o.fov!=0;v.position=vector(s.position);v.sceneAfter=prior;v.sceneAfter.look=vector(s.direction);v.sceneAfter.up=vector(s.up);v.sceneAfter.fovDegrees=s.fov;
    v.view=cv::view(s.position,s.direction,s.up,s.roll);volatile float angle=number(0x3c8efa34)*s.fov;volatile float inverse=1.0f/prior.aspect1F8;
    v.projection=cv::projection(prior.nearPlane,prior.farPlane,angle,inverse);v.combined=cv::multiply(v.view,v.projection);return v;
}
static void equalScene(const Scene& a,const Scene& b){equalVec(a.look,b.look,"scene Look differs");equalVec(a.up,b.up,"scene Up differs");require(bits(a.fovDegrees)==bits(b.fovDegrees)&&bits(a.aspect1F8)==bits(b.aspect1F8)&&bits(a.nearPlane)==bits(b.nearPlane)&&bits(a.farPlane)==bits(b.farPlane)&&bits(a.rollRadians)==bits(b.rollRadians),"scene scalar words differ");}
static void equalView(const View& a,const View& b){equalScene(a.sceneAfter,b.sceneAfter);equalVec(a.position,b.position,"view position payload differs");equalFloats(a.view,b.view,"view matrix exact word differs");equalFloats(a.projection,b.projection,"projection matrix exact word differs");equalFloats(a.combined,b.combined,"combined matrix exact word differs");require(a.targetAccepted==b.targetAccepted&&a.fovWritten==b.fovWritten,"scene consumer flags differ");}
static View compareView(const Scene& s,const Output& o){const auto want=originalView(s,o);View got;auto before=allocations;const bool ok=composeCameraView49F(s,o,got);auto after=allocations;require(ok,"finite consumer rejected");require(before==after,"successful consumer allocated");++hotCalls;++viewCases;equalView(got,want);return got;}
static tv::Result compareFrame(const Fixture& f,Selected& s,Update& u,const Input& i,TVCameraRandom49E& r,Scene& scene,Output& live,View& view){
    auto ps=s;auto pu=u;auto pr=r;auto po=live;po.look=scene.look;po.up=scene.up;
    const auto want=compareTV(f,ps,pu,i,pr,po);const auto pv=compareView(scene,po);
    auto before=allocations;const bool ok=updateTVCameraView49F(f.view,s,u,i,r,scene,live,view);auto after=allocations;
    require(ok,"finite composed frame rejected");require(before==after,"successful composed frame allocated");++hotCalls;++frameCases;
    equalTV(s,u,live,r,want);equalView(view,pv);equalScene(scene,pv.sceneAfter);return want;
}
static void rejectFrame(const Fixture& f,Selected s,Update u,Input i,Scene scene=Scene{},bool oracleFailure=false,CameraViewContext49F context={}){
    TVCameraRandom49E r{0x13579bdf};auto o=output();View v;v.position={101,103,107,109};v.targetAccepted=true;v.fovWritten=true;v.view.fill(113);v.projection.fill(127);v.combined.fill(131);
    const auto ss=snapshot(s);const auto su=snapshot(u);const auto sr=snapshot(r);const auto so=snapshot(o);const auto sc=snapshot(scene);const auto sv=snapshot(v);
    if(oracleFailure){bool rejected=false;try{(void)originalTV(f,s,u,i,r,o);}catch(const tv::DomainError&){rejected=true;}require(rejected,"finite TV domain fixture did not reach expected original unsupported arithmetic");require(!updateTVCamera49F(f.view,s,u,i,r,o),"TV primitive admitted unsupported arithmetic");require(snapshot(s)==ss&&snapshot(u)==su&&snapshot(r)==sr&&snapshot(o)==so,"failed TV primitive changed state/output/RNG");}
    require(!updateTVCameraView49F(f.view,s,u,i,r,scene,o,v,context),"unsupported composed frame accepted");
    require(snapshot(s)==ss&&snapshot(u)==su&&snapshot(r)==sr&&snapshot(o)==so&&snapshot(scene)==sc&&snapshot(v)==sv,"failed whole frame changed state/output/RNG");++rejections;
}
static void rejectView(Scene s,Output o){View v;v.position={137,139,149,151};v.view.fill(157);v.projection.fill(163);v.combined.fill(167);const auto old=snapshot(v);require(!composeCameraView49F(s,o,v),"unsupported consumer accepted");require(snapshot(v)==old,"failed consumer changed output");++rejections;}

static void testAuthored(Fixture& f,const std::string& text){stage="authored";f.load(text,true);require(f.view.cameras().size==19&&f.view.links().size==20,"TA authored camera/link counts");require(bits(f.view.switchEvery())==authored::switch_every_word,"TA SwitchEvery");
    for(unsigned c=0;c<19;++c){const auto links=f.view.nextCameras(c);require(links.size==authored::rows[c][1],"TA ordered branch count");for(unsigned n=0;n<links.size;++n)require(links[n]==authored::links[authored::rows[c][0]+n],"TA ordered branch target");
        for(unsigned mode=0;mode<2;++mode){auto s=selected(f,c);s.stationary80=I(mode);Update u;u.playerPosition60.w=17;TVCameraRandom49E r{c+1};Scene scene;scene.rollRadians=.013f*float(c);auto o=output();View v;
            for(unsigned n=0;n<8;++n){auto i=input();const auto& p=f.records[c].position;const auto& q=f.records[(c+1)%19].position;const float t=float(n)/7; i.resolvedPlayerPosition={p.x+(q.x-p.x)*t,p.y+(q.y-p.y)*t+20,p.z+(q.z-p.z)*t,number(0x7fc12345)};i.targetYOffset=9;i.elapsed=.03f;compareFrame(f,s,u,i,r,scene,o,v);require(bits(u.playerPosition60.w)==bits(17)&&bits(o.target.w)==(mode!=0?bits(37):bits(17)),"body getter incorrectly consumed input W");++authoredFrames;}
        }
    }
    // Selection is deliberately explicit and outside the 49F frame operation.
    // No game TV/follow chooser or its extra RNG calls are synthesized here.
    for(unsigned branch:{4u,5u}){auto s=selected(f,3);s.hold90=0;Update u;Scene scene;auto o=output();View v;TVCameraRandom49E r{0xffffffff};TVCameraSelection49E selection;
        auto i=input();i.resolvedPlayerPosition=f.records[branch].position;auto before=allocations;bool ok=selectTVCamera49E(f.view,s,i.resolvedPlayerPosition,0,r,selection);auto after=allocations;require(ok&&before==after&&s.current40==I(branch),"explicit TA first 4-to-5/6 selection failed");++hotCalls;
        i.resolvedPlayerPosition.y+=30;compareFrame(f,s,u,i,r,scene,o,v);++trajectoryFrames;
    }
    Selected s;Update u;Scene scene;auto o=output();View v;TVCameraRandom49E r{1};TVCameraSelection49E selection;
    for(unsigned target=0;target<=19;++target){const auto c=target%19;const auto p=f.records[c].position;unsigned tries=0;do{auto before=allocations;bool ok=selectTVCamera49E(f.view,s,p,.2f,r,selection);auto after=allocations;require(ok&&before==after,"explicit authored trajectory selection rejected/allocated");++hotCalls;auto i=input();i.resolvedPlayerPosition=p;i.resolvedPlayerPosition.y+=25;i.elapsed=.2f;compareFrame(f,s,u,i,r,scene,o,v);++trajectoryFrames;++tries;}while(s.current40!=I(c)&&tries<8);require(s.current40==I(c),"TA trajectory failed to reach authored camera");}
    require(s.current40==0&&s.previous44==18,"TA 19-to-1 loop not reached");
    // The game selector reads local XYZ; the TV body getter resolves world XYZ.
    // Deliberately separate those coordinates and prove 49F never reselects.
    {Selected localSelected;Update worldState;TVCameraRandom49E sharedRandom{73};TVCameraSelection49E picked;const Vec selectorLocalPosition=f.records[0].position;auto count=allocations;const bool ok=selectTVCamera49E(f.view,localSelected,selectorLocalPosition,0,sharedRandom,picked);auto end=allocations;require(ok&&count==end&&localSelected.current40==0,"local-pose explicit selection failed");++hotCalls;auto worldInput=input();worldInput.resolvedPlayerPosition=f.records[14].position;worldInput.resolvedPlayerPosition.y+=250;Scene worldScene;auto live=output();View matrices;const I selectedBefore=localSelected.current40;const U rngBefore=sharedRandom.raw;compareFrame(f,localSelected,worldState,worldInput,sharedRandom,worldScene,live,matrices);require(localSelected.current40==selectedBefore&&sharedRandom.raw==rngBefore,"49F secretly selected a camera or consumed game-mode RNG");require(live.target.x==worldInput.resolvedPlayerPosition.x&&live.target.z==worldInput.resolvedPlayerPosition.z&&live.target.x!=selectorLocalPosition.x,"local selector position substituted for resolved world target");++trajectoryFrames;}
}

static void testFovAndModes(){stage="FOV and modes";
    const std::array<std::string,6> variants{{"MaxFOV=70\nMinFOV=20\nMaxFOVDistance=200\nMinFOVDistance=20\n","MaxFOV=25\nMinFOV=65\nMaxFOVDistance=400\nMinFOVDistance=1572\n","MaxFOV=80\nMinFOV=10\nMaxFOVDistance=20\nMinFOVDistance=200\n","FOV=43\nMaxFOVDistance=20\nMinFOVDistance=20\n","MaxFOV=70\nMinFOV=20\nMaxFOVDistance=20\nMinFOVDistance=20\n","FOV=-15\n"}};
    for(unsigned v=0;v<variants.size();++v){Fixture f;f.load(oneCamera("Position=10,20,30\nLook=2,3,4\nUp=0,7,1\n"+variants[v]));
        for(I mode:{0,1,-3})for(float distance:{0.f,10.f,20.f,55.f,200.f,500.f,2000.f})for(float dt:{-.02f,-0.f,0.f,.016f,.4f,.8f}){
            auto s=selected(f);s.stationary80=mode;s.filterCurrent00=61;Update u;u.playerPosition60.w=13;auto i=input();i.elapsed=dt;i.resolvedPlayerPosition={10+distance,400,30,19};Scene scene;auto o=output();View view;TVCameraRandom49E r{17};
            if(v==4&&mode==0){rejectFrame(f,s,u,i,scene,true);continue;}
            compareFrame(f,s,u,i,r,scene,o,view);require((o.mode==0)==(mode!=0),"stationary flag does not select actual output mode");
            require(words(u.cameraPosition10)==words(f.records[0].position),"base camera state changed by consumer");
            if(dt==0)require(bits(o.fov)==bits(61),"zero-dt FOV filter changed current");
            if(v==1)require(bits(u.rawFov04)==bits(25),"authored reversed FOV source clamp changed");
            if(dt==.016f&&u.rawFov04!=61)require(bits(u.rawFov04)!=bits(o.fov),"raw FOV overwritten by filtered output");
        }
    }
    // Horizontal source FOV distance ignores target Y, while target preserves it.
    Fixture f;f.load(oneCamera("MaxFOV=70\nMinFOV=20\nMaxFOVDistance=200\nMinFOVDistance=20\n"));
    U raw=0;for(float y:{0.f,10000.f,-10000.f}){auto s=selected(f);Update u;auto i=input();i.resolvedPlayerPosition={100,y,0,1};TVCameraRandom49E r;auto o=output();compareTV(f,s,u,i,r,o);if(y==0)raw=bits(u.rawFov04);else require(raw==bits(u.rawFov04),"FOV incorrectly used Y distance");}
}

static void testShake(){stage="scene shake";Fixture f;f.load(oneCamera("Position=10,20,30\nFOV=60\nLook=0,0,1\nUp=0,1,0\n"));
    std::array<TVCameraSceneSample49F,5> samples{{{1,{number(0x7fc12345),0,0,1},{number(0x7fc54321),0,0,1}},{0,{500,20,30,1},{340,0,0,1}},{0,{10,20,30,1},{100,0,0,1}},{0,{10,20,30,1},{340,0,0,1}},{0,{10,20,30,1},{510,0,0,1}}}};
    auto s=selected(f);Update u;auto i=input();i.resolvedPlayerPosition={9000,7000,8000,1};i.sceneSamples={samples.data(),samples.size()};i.elapsed=.1f;TVCameraRandom49E r{1};Scene scene;auto o=output();View v;
    auto first=compareFrame(f,s,u,i,r,scene,o,v);++shakeCases;require(s.sourceD0==3&&first.randomCalls==3,"complete ordered scene did not select first eligible original index");require(words(u.cameraPosition10)==words(f.records[0].position)&&words(o.position)!=words(u.cameraPosition10),"shake changed base state or missed output");require(o.target.x==9000&&o.target.y==7005&&o.target.z==8000,"resolved actor world XYZ confused with local scene position");
    const auto phase=u.shakeVectorB0;auto second=compareFrame(f,s,u,i,r,scene,o,v);++shakeCases;require(s.sourceD0==4&&second.randomCalls==0&&bits(u.shakeVectorB0.x)==0&&bits(u.shakeVectorB0.y)==bits(phase.y)&&bits(u.shakeVectorB0.z)==bits(phase.z),"skip-current/next eligible restore phase incorrect");
    // Preserve complete entries, deactivate later sources instead of compacting.
    samples[4].inactive40=1;samples[3].inactive40=1;i.elapsed=.2f;
    auto third=compareFrame(f,s,u,i,r,scene,o,v);++shakeCases;require(third.randomCalls==3,"shake alternating random phase did not draw three");i.elapsed=.2f;
    auto fourth=compareFrame(f,s,u,i,r,scene,o,v);++shakeCases;require(fourth.randomCalls==0&&s.wordCC==0&&s.sourceD0==-1&&s.fieldC0==0&&s.fieldC4==0&&s.fieldC8==0,"shake expiry/reset words mismatch");
    // No scene trigger: inactive, out-of-radius, low speed, or current index.
    for(unsigned reason=0;reason<4;++reason){TVCameraSceneSample49F sample;sample.localPosition={10,20,30,1};sample.velocity={340,0,0,1};auto t=selected(f);if(reason==0)sample.inactive40=1;if(reason==1)sample.localPosition.x=66;if(reason==2)sample.velocity.x=169;if(reason==3)t.sourceD0=0;Update uu;auto ii=input();ii.sceneSamples={&sample,1};TVCameraRandom49E rr{37};auto oo=output();auto got=compareTV(f,t,uu,ii,rr,oo);require(got.randomCalls==0&&rr.raw==37&&t.wordCC==0,"ineligible scene entry triggered shake");++shakeCases;}
    // Boundary zero intensity is an original divide-by-zero when B0.x==0.
    for(unsigned reason=0;reason<2;++reason){TVCameraSceneSample49F sample;sample.localPosition={reason==0?65.f:10.f,20,30,1};sample.velocity={reason==0?340.f:170.f,0,0,1};auto t=selected(f);Update uu;auto ii=input();ii.sceneSamples={&sample,1};rejectFrame(f,t,uu,ii,Scene{},true);++shakeCases;
        // Existing nonzero-X phase takes the finite restore branch before expiry.
        uu.shakeVectorB0={2,3,4,5};TVCameraRandom49E rr{41};auto oo=output();auto got=compareTV(f,t,uu,ii,rr,oo);require(got.randomCalls==0&&t.wordCC==0&&t.sourceD0==-1,"zero-intensity restore branch should expire finitely");++shakeCases;
    }
    // Signed nonzero phase restores saved A0 (which is a separate source
    // field), and only clears B0.x. Exactly equal elapsed/duration expires.
    {auto t=selected(f);t.positionA0={-70,80,90,-0.f};t.wordCC=1;t.fieldC0=.5f;t.fieldC4=.25f;t.fieldC8=7;t.sourceD0=3;Update uu;uu.shakeVectorB0={-2,3,4,-0.f};auto ii=input();ii.elapsed=.25f;TVCameraRandom49E rr{73};auto oo=output();const auto a=compareTV(f,t,uu,ii,rr,oo);require(a.randomCalls==0&&rr.raw==73,"negative nonzero shake X did not restore without RNG");require(words(oo.position)==words(t.positionA0)&&words(oo.position)!=words(uu.cameraPosition10),"restore must copy saved A0 rather than raw selected camera position");require(bits(uu.shakeVectorB0.x)==0&&uu.shakeVectorB0.y==3&&uu.shakeVectorB0.z==4&&bits(uu.shakeVectorB0.w)==0x80000000u,"restore changed more than shake X");require(t.wordCC==0&&t.fieldC0==0&&t.fieldC4==0&&t.fieldC8==0&&t.sourceD0==-1,"exact elapsed equals duration did not expire");++shakeCases;}
    // A first RNG output of exactly zero makes shake X zero, so the next
    // update draws again rather than inventing an unconditional phase toggle.
    {auto t=selected(f);t.wordCC=1;t.fieldC0=.45f;t.fieldC8=14;Update uu;auto ii=input();ii.elapsed=.01f;TVCameraRandom49E rr{0xfc77a683u};auto oo=output();const auto a=compareTV(f,t,uu,ii,rr,oo);require(a.randomCalls==3&&bits(uu.shakeVectorB0.x)==0,"zero random X phase anchor");const auto b=compareTV(f,t,uu,ii,rr,oo);require(b.randomCalls==3,"zero-X shake was incorrectly forced to alternate");shakeCases+=2;}
    // Duration/fade and native state may be negative; preserve original ordering.
    for(float elapsed:{-.2f,0.f,.01f,.5f}){auto t=selected(f);t.wordCC=1;t.fieldC0=.45f;t.fieldC4=.1f;t.fieldC8=9;t.sourceD0=4;Update uu;auto ii=input();ii.elapsed=elapsed;TVCameraRandom49E rr{0xffffffff};auto oo=output();compareTV(f,t,uu,ii,rr,oo);++shakeCases;}
}

static void testConsumer(){stage="consumer numeric leaves";Scene s;s.look={2,3,4,9};s.up={0,4,0,11};s.aspect1F8=.75f;s.nearPlane=.25f;s.farPlane=7000;auto o=output();o.position={3,4,5,7};o.fov=77;
    for(I mode:{0,1,-1,17})for(Vec target:std::array<Vec,6>{{{3,4,5,13},{3,8,5,13},{3,-8,5,13},{3,4,50,13},{-8,2,9,13},{3.00001f,400,5,13}}})for(float roll:{0.f,-0.f,.3f,-.3f,2.3f,-2.3f,3.1415927f}){s.rollRadians=roll;o.mode=mode;o.target=target;o.look={2,3,4,9};o.up={0,4,1,11};auto v=compareView(s,o);require(v.targetAccepted==(mode!=0&&(target.x!=o.position.x||target.y!=o.position.y||target.z!=o.position.z)),"target acceptance branch mismatch");}
    for(float fov:{0.f,-0.f,1.f,10.f,40.f,77.f,120.f,180.f,-45.f,-180.f,-270.f,-360.f,270.f,360.f,179.99998f,180.00002f})for(float aspect:{.25f,1.f,2.f,-1.f}){s.rollRadians=0;s.aspect1F8=aspect;o.mode=0;o.look={0,0,1,1};o.up={0,1,0,1};o.fov=fov;auto v=compareView(s,o);require(v.fovWritten==(fov!=0),"zero FOV retain flag");if(fov==0)require(bits(v.sceneAfter.fovDegrees)==bits(s.fovDegrees),"zero FOV overwrote prior");require(bits(v.projection[11])==bits(1)&&bits(v.projection[15])==0,"projection homogeneous convention");require(bits(v.projection[14])==bits(-v.projection[10]*s.nearPlane),"projection near/depth coefficient");}
    s=Scene{};o=output();o.mode=0;o.position={0,0,0,1};o.look={0,0,1,1};o.up={0,1,0,1};o.fov=60;
    for(float a:{-3.1415927f,-2.3f,-1.5707964f,-.5f,-0.f,0.f,.3f,1.5707964f,2.3f,3.1415927f}){s.rollRadians=a;auto v=compareView(s,o);const auto sc=cv::sincosSource(-a);const auto rm=cv::rollMatrix(-a);if(a!=0){require(v.view[0]==rm[0]&&v.view[1]==rm[1]&&v.view[4]==rm[4]&&v.view[5]==rm[5],"roll leaf row-vector sign/composition mismatch");}require(bits(rm[0])==bits(sc[1])&&bits(rm[1])==bits(-sc[0])&&bits(rm[4])==bits(sc[0]),"sincos polynomial/roll entries mismatch");volatile float radians=number(0x3c8efa34)*60.f;volatile float half=.5f*radians;const float t=cv::tanSource(half);volatile float h=1.f/t;require(bits(v.projection[0])==bits(h),"source tan/projection exact scale");}
    // Original trig folds once; it does not apply full periodic reduction.
    {Scene wide;wide.rollRadians=number(0x40fdb225u);auto supplied=output();supplied.mode=0;supplied.position={3,4,5,1};supplied.look={0,0,1,1};supplied.up={0,1,0,1};supplied.fov=60;const auto v=compareView(wide,supplied);require(bits(v.view[0])==0x3e45e828u,"original single-fold polynomial coefficient witness");}
    for(Vec zero:std::array<Vec,3>{{{0,0,0,5},{-0.f,0,-0.f,7},{number(1),-number(1),0,11}}}){o.look=zero;o.up=zero;s.rollRadians=0;compareView(s,o);}
    // Only copied/unconsumed W lanes are payloads. Normalized W is source W+0.
    for(U w:{0x80000000u,0x7fc12345u,0x7f800000u,0xff800000u}){s=Scene{};s.look.w=number(w);o=output();o.mode=1;o.position={3,4,5,number(w)};o.target={3,4,5,number(w)};o.look={number(0x7fc54321),0,0,number(w)};o.up={number(0x7fc76543),0,0,number(w)};View v;auto n=allocations;const bool ok=composeCameraView49F(s,o,v);auto end=allocations;require(ok&&n==end,"unconsumed consumer W rejected/allocated");++hotCalls;require(bits(v.position.w)==w&&bits(v.sceneAfter.look.w)==w,"consumer true-copy W not bit preserved");
        o.mode=0;o.look={0,0,1,number(w)};o.up={0,1,0,number(w)};if(w==0x80000000u){auto result=compareView(Scene{},o);require(bits(result.sceneAfter.look.w)==0&&bits(result.sceneAfter.up.w)==0,"normalized signed-zero W missed original W+0");}else rejectView(Scene{},o);
    }
}

static U randomState=0x49f9e123;
static U randomWord(){randomState^=randomState<<13;randomState^=randomState>>17;randomState^=randomState<<5;return randomState;}
static float sample(float scale=1){return (float(randomWord()%200001)-100000.f)*(scale/100000.f);}
static void testRandomized(const Fixture& f){stage="randomized finite differential";
    for(unsigned n=0;n<384;++n){Scene scene;scene.look={sample(),sample(),sample(),7};scene.up={sample(),sample(),sample(),11};scene.rollRadians=sample(3.14f);scene.fovDegrees=60+sample(20);scene.aspect1F8=.25f+std::fabs(sample(2));scene.nearPlane=.01f+std::fabs(sample(5));scene.farPlane=10000+sample(9000);auto o=output();o.mode=I(n%3)-1;o.position={sample(1000),sample(1000),sample(1000),13};o.target={sample(1000),sample(1000),sample(1000),17};o.look={sample(),sample(),sample(),19};o.up={sample(),sample(),sample(),23};o.fov=50+sample(30);compareView(scene,o);}
    for(unsigned n=0;n<192;++n){auto s=selected(f,n%19);s.stationary80=I(n%3==0);s.filterCurrent00=50+sample(20);Update u;u.playerPosition60.w=31;u.filterTau04=.1f+std::fabs(sample(.8f));auto i=input();const auto p=f.records[n%19].position;i.resolvedPlayerPosition={p.x+sample(1500),p.y+sample(200),p.z+sample(1500),37};i.targetYOffset=sample(20);i.elapsed=sample(.05f);i.bodyVelocity={sample(400),sample(400),sample(400),41};std::array<TVCameraSceneSample49F,3> samples{{{1,{0,0,0,1},{0,0,0,1}},{0,{p.x+sample(100),p.y,p.z,1},{170+std::fabs(sample(400)),0,0,1}},{0,{p.x,p.y,p.z,1},{169,0,0,1}}}};i.sceneSamples={samples.data(),samples.size()};TVCameraRandom49E rng{randomWord()};Scene scene;scene.rollRadians=sample(3);scene.aspect1F8=.5f+std::fabs(sample());auto o=output();View v;compareFrame(f,s,u,i,rng,scene,o,v);}
}

static void testAdmission(){stage="atomic finite admission";Fixture f;f.load(oneCamera("Position=10,20,30\nMaxFOV=70\nMinFOV=20\nMaxFOVDistance=200\nMinFOVDistance=20\n"));auto s=selected(f);Update u;auto i=input();
    Fixture unloaded;rejectFrame(unloaded,s,u,i);for(I index:{-1,1,std::numeric_limits<I>::max()}){auto q=s;q.current40=index;rejectFrame(f,q,u,i);}
    for(U w:{0x7f800000u,0xff800000u,0x7fc12345u}){float bad=number(w);auto ii=i;ii.elapsed=bad;rejectFrame(f,s,u,ii);ii=i;ii.targetYOffset=bad;rejectFrame(f,s,u,ii,Scene{},true);
        for(unsigned k=0;k<3;++k){ii=i;if(k==0)ii.resolvedPlayerPosition.x=bad;if(k==1)ii.resolvedPlayerPosition.y=bad;if(k==2)ii.resolvedPlayerPosition.z=bad;rejectFrame(f,s,u,ii,Scene{},true);}
        auto uu=u;uu.filterTau04=bad;rejectFrame(f,s,uu,i,Scene{},true);auto ss=s;ss.filterCurrent00=bad;rejectFrame(f,ss,u,i,Scene{},true);
        TVCameraSceneSample49F sample;sample.localPosition=f.records[0].position;sample.velocity={bad,0,0,1};ii=i;ii.sceneSamples={&sample,1};rejectFrame(f,s,u,ii,Scene{},true);sample.velocity={340,0,0,1};sample.localPosition.x=bad;rejectFrame(f,s,u,ii,Scene{},true);
        Scene scene;scene.aspect1F8=bad;rejectFrame(f,s,u,i,scene);scene=Scene{};scene.rollRadians=bad;rejectFrame(f,s,u,i,scene);scene=Scene{};scene.nearPlane=bad;rejectFrame(f,s,u,i,scene);scene=Scene{};scene.farPlane=bad;rejectFrame(f,s,u,i,scene);
        auto o=output();o.position.x=bad;rejectView(Scene{},o);o=output();o.mode=0;o.look.x=bad;rejectView(Scene{},o);o=output();o.mode=0;o.up.x=bad;rejectView(Scene{},o);o=output();o.fov=bad;rejectView(Scene{},o);
    }
    {auto uu=u;uu.filterTau04=0;rejectFrame(f,s,uu,i,Scene{},true);uu=u;uu.filterTau04=number(1);auto ii=i;ii.elapsed=-1;rejectFrame(f,s,uu,ii,Scene{},true);}
    {auto ii=i;ii.resolvedPlayerPosition.x=1e20f;rejectFrame(f,s,u,ii,Scene{},true);ii=i;ii.resolvedPlayerPosition.y=std::numeric_limits<float>::max();ii.targetYOffset=std::numeric_limits<float>::max();rejectFrame(f,s,u,ii,Scene{},true);}
    {auto ss=s;ss.wordCC=1;ss.fieldC0=0;ss.fieldC8=7;rejectFrame(f,ss,u,i,Scene{},true);ss.fieldC0=.4f;auto uu=u;uu.shakeVectorB0.x=number(0x7fc12345);rejectFrame(f,ss,uu,i,Scene{},true);}
    // TV succeeds and consumes three RNG words; consumer failure still rolls all back.
    {TVCameraSceneSample49F sample;sample.localPosition=f.records[0].position;sample.velocity={340,0,0,1};auto ii=i;ii.sceneSamples={&sample,1};auto ss=s;auto uu=u;auto oo=output();TVCameraRandom49E rr{31};const auto succeeded=compareTV(f,ss,uu,ii,rr,oo);require(succeeded.randomCalls==3,"late consumer-failure fixture failed to reach successful RNG-mutating TV stage");Scene scene;scene.aspect1F8=0;rejectFrame(f,s,u,ii,scene);}
    for(unsigned kind=0;kind<6;++kind){Scene scene;if(kind==0)scene.aspect1F8=0;if(kind==1)scene.farPlane=0;if(kind==2)scene.nearPlane=scene.farPlane;if(kind==3)scene.up={1e20f,0,0,1};if(kind==4)scene.rollRadians=1e20f;if(kind==5)scene.aspect1F8=number(1);rejectFrame(f,s,u,i,scene);}
    {auto o=output();o.fov=0;Scene scene;scene.fovDegrees=0;rejectView(scene,o);o=output();o.mode=0;o.look={1e20f,0,0,1};rejectView(Scene{},o);o=output();o.mode=1;o.position={-std::numeric_limits<float>::max(),0,0,1};o.target={std::numeric_limits<float>::max(),0,0,1};rejectView(Scene{},o);}
    for(int rounding:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){require(std::fesetround(rounding)==0,"rounding mode unavailable");rejectFrame(f,s,u,i);rejectView(Scene{},output());auto ss=s;auto uu=u;TVCameraRandom49E rr{19};auto oo=output();const auto oldS=snapshot(ss);const auto oldU=snapshot(uu);const auto oldR=snapshot(rr);const auto oldO=snapshot(oo);require(!updateTVCamera49F(f.view,ss,uu,i,rr,oo),"TV primitive accepted unsupported rounding");require(snapshot(ss)==oldS&&snapshot(uu)==oldU&&snapshot(rr)==oldR&&snapshot(oo)==oldO,"rounding rejection changed TV primitive state");++rejections;require(std::fesetround(FE_TONEAREST)==0,"nearest rounding restore failed");}
    for(U badW:{0x7f800000u,0xff800000u,0x7fc13579u}){Scene scene;scene.up.w=number(badW);auto oo=output();oo.mode=1;oo.position={1,2,3,7};oo.target={10,20,30,11};rejectView(scene,oo);rejectFrame(f,s,u,i,scene);}
    // Arithmetic-unconsumed state and copied 128-bit lanes remain bitwise payloads.
    {Fixture fixed;fixed.load(oneCamera("FOV=60\nPosition=1,2,3\nLook=0,0,1\nUp=0,1,0\n"));auto ss=selected(fixed);ss.stationary80=1;ss.hold90=number(0x7fc11111);ss.fieldC0=number(0x7fc22222);ss.fieldC4=number(0x7fc33333);ss.fieldC8=number(0x7fc44444);Update uu;uu.rawFov04=number(0x7fc55555);uu.playerPosition60.w=number(0x7fc66666);uu.playerVelocity70={number(0x7fc77777),0,0,number(0x7fc88888)};uu.shakeVectorB0={number(0x7fc99999),0,0,number(0x7fcaaaaa)};auto ii=input();ii.resolvedPlayerPosition={number(0x7fcbbbbb),5,number(0x7fcccccc),number(0x7fcddddd)};ii.bodyVelocity={number(0x7fceeeee),number(0x7fcfffff),number(0x7fc12345),number(0x80000000)};Scene scene;auto oo=output();oo.target={number(0x7fc54321),0,0,number(0x7fc56789)};View vv;TVCameraRandom49E rr{97};compareFrame(fixed,ss,uu,ii,rr,scene,oo,vv);require(bits(uu.playerPosition60.w)==0x7fc66666&&words(uu.playerVelocity70)==words(ii.bodyVelocity)&&bits(oo.target.x)==0x7fc54321,"unconsumed TV payload words were changed");}
}

static void testContext(const Fixture& f){
    stage="caller context flags";
    const auto initialSelected=selected(f);const Update initialUpdate;const Scene initialScene;const auto initialOutput=output();
    auto i=input();TVCameraSceneSample49F sample;sample.localPosition=initialSelected.positionA0;sample.velocity={340,0,0,1};i.sceneSamples={&sample,1};
    // This ordinary frame genuinely changes all numeric stages and draws RNG,
    // so rejecting context cannot be implemented by quietly skipping matrices.
    auto expectedSelected=initialSelected;auto expectedUpdate=initialUpdate;auto expectedScene=initialScene;auto expectedOutput=initialOutput;TVCameraRandom49E expectedRandom{0x13579bdf};View expectedView;
    const auto expected=compareFrame(f,expectedSelected,expectedUpdate,i,expectedRandom,expectedScene,expectedOutput,expectedView);
    require(expected.randomCalls==3,"context fixture must reach RNG-mutating ordinary TV update");
    for(CameraViewContext49F context:std::array<CameraViewContext49F,6>{{{4,0},{0,4},{4,4},{0xff,0xfb},{0xfb,0xff},{0xff,0xff}}}){
        rejectFrame(f,initialSelected,initialUpdate,i,initialScene,false,context);++contextCases;
    }
    // Bit2 is the only admitted-context restriction. Other bits are carried as
    // caller context and must not change the recovered ordinary numeric frame.
    for(CameraViewContext49F context:std::array<CameraViewContext49F,9>{{{0,0},{1,2},{2,1},{8,0},{0,8},{0x80,0x40},{0xfb,0},{0,0xfb},{0xfb,0xfb}}}){
        auto s=initialSelected;auto u=initialUpdate;auto scene=initialScene;auto o=initialOutput;TVCameraRandom49E r{0x13579bdf};View v;
        const auto before=allocations;const bool ok=updateTVCameraView49F(f.view,s,u,i,r,scene,o,v,context);const auto after=allocations;
        require(ok,"bit2-clear caller context rejected");require(before==after,"successful context-qualified frame allocated");++hotCalls;++frameCases;++contextCases;
        equalTV(s,u,o,r,expected);equalScene(scene,expectedScene);equalView(v,expectedView);
    }
}

int main(int argc,char** argv){try{require(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,"binary32 required");require(std::fesetround(FE_TONEAREST)==0,"nearest-even host rounding required");
    // The oracle hook records executed frozen instructions, not native lines.
    cv::instructionTrace49F=recordConsumer;
    const char* path=argc>1?argv[1]:"evidence/tvCamera49E/authored/taTVCam.pos";std::ifstream in(path,std::ios::binary);require(bool(in),"authentic TA POS fixture missing");const std::string text((std::istreambuf_iterator<char>(in)),{});Fixture f;testAuthored(f,text);testFovAndModes();testShake();testConsumer();testRandomized(f);testAdmission();testContext(f);
    require(covered(tv_update)==152&&covered(tv_base)==104&&covered(tv_target)==16&&covered(tv_position)==9&&covered(tv_fov)==57&&covered(tv_scene)==108&&covered(tv_local)==7&&covered(tv_construct)==7,"TV admitted original-word coverage unexpectedly incomplete");require(covered(tv_rng)==tv_rng.size(),"RNG leaf coverage incomplete");require(covered(cv_view)==cv_view.size()&&covered(cv_projection)==cv_projection.size()&&covered(cv_roll)==cv_roll.size()&&covered(cv_zero_matrix)==cv_zero_matrix.size()&&covered(cv_multiply)==cv_multiply.size()&&covered(cv_scene_set)==cv_scene_set.size()&&covered(cv_look_target)==117&&covered(cv_tan)>=95&&covered(cv_sincos)==87,"consumer original-word coverage incomplete");
    std::cout<<"{\"status\":\"pass\",\"checks\":"<<checks<<",\"cases\":{\"tv_word_differential\":"<<tvCases<<",\"consumer_word_differential\":"<<viewCases<<",\"composed_frames\":"<<frameCases<<",\"authored_frames\":"<<authoredFrames<<",\"explicit_selection_trajectory_frames\":"<<trajectoryFrames<<",\"shake_directed\":"<<shakeCases<<",\"caller_context_flags\":"<<contextCases<<",\"native_atomic_rejections\":"<<rejections<<"},\"hot_calls\":"<<hotCalls<<",\"hot_allocations\":0,\"oracle_rng_draws\":"<<randomDraws<<",\"word_coverage\":{\"tv\":{";
    bool comma=false;
#define PRINT_TV(N) if(comma)std::cout<<',';comma=true;std::cout<<"\""#N"\":["<<covered(tv_##N)<<','<<tv_##N.size()<<']';
    TV_BLOCKS(PRINT_TV)
#undef PRINT_TV
    std::cout<<"},\"consumer\":{";comma=false;
#define PRINT_CV(N) if(comma)std::cout<<',';comma=true;std::cout<<"\""#N"\":["<<covered(cv_##N)<<','<<cv_##N.size()<<']';
    CV_BLOCKS(PRINT_CV)
#undef PRINT_CV
    std::cout<<"}},\"instructions\":{\"tv\":"<<tvInstructions<<",\"consumer\":"<<consumerInstructions<<"},\"proof\":{\"floating_model\":\"strict host binary32 instruction steps, not universal PS2 silicon proof\",\"rounding_modes\":1,\"unexecuted_live_branch\":\"five retained/source-reviewed tangent exact-cosine-zero saturation words; no witness in bounded FOV-neighbor scan\",\"authored_cameras\":19,\"authored_links\":20,\"boundary\":\"already-selected body-attached TV through actual core view/projection before platform rendering\",\"game_mode_or_extra_rng_simulated\":false,\"original_invalid_memory_executions\":0}}\n";return 0;
}catch(const std::exception& e){std::cerr<<"camera_frame49f: "<<e.what()<<" (stage="<<stage<<", checks="<<checks<<", tv="<<tvCases<<", view="<<viewCases<<", frames="<<frameCases<<")\n";return 1;}}
