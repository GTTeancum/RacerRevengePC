#include "../../src/assets/tv_camera49e.h"
#include "../../src/game/tv_camera_selection49e.h"
#include "../../evidence/tvCamera49E/loader-source/narrow_oracle49e.h"
#include "../../evidence/tvCamera49E/loader-source/authored_projection49e.h"
#include "../../evidence/tvCamera49E/runtime-source/runtime_oracle49e.h"
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

// Counts successful native hot-call C++ allocations, without failure injection,
// allocator ownership experiments, process RAM measurements or pointer probes.
static std::uint64_t allocations=0;
void* operator new(std::size_t n){if(void* p=std::malloc(n?n:1)){++allocations;return p;}throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}
void operator delete[](void* p,std::size_t) noexcept{std::free(p);}

using namespace rr::assets;
using namespace rr::game;
namespace original=tv_camera49e_oracle;
namespace authored=tv_camera49e_authored;
using U=std::uint32_t;
using I=std::int32_t;
using Vec=TVCameraVector49E;
using Record=TVCameraRecord49E;
using State=TVCameraSelectorState49E;
static std::uint64_t checks=0,loads=0,rejections=0,narrowCases=0,runtimeCases=0,resetCases=0,rngCases=0,hotCalls=0;
static std::uint64_t authoredFrames=0,runtimeInstructions=0,narrowInstructions=0;
static std::array<std::uint64_t,138> narrowVisits{};
static std::array<std::uint64_t,std::size(tv_camera49e_original::selector_words)> selectorVisits{};
static std::array<std::uint64_t,std::size(tv_camera49e_original::reset_words)> resetVisits{};
static std::array<std::uint64_t,std::size(tv_camera49e_original::random_words)> randomVisits{};
static std::array<std::uint64_t,std::size(tv_camera49e_original::seed_words)> seedVisits{};
static void require(bool condition,const char* message){++checks;if(!condition)throw std::runtime_error(message);}
static U bits(float f){U v;std::memcpy(&v,&f,4);return v;}
static float number(U v){float f;std::memcpy(&f,&v,4);return f;}
static I signedWord(U v){I i;std::memcpy(&i,&v,4);return i;}
static double binary64(std::uint64_t v){double d;std::memcpy(&d,&v,8);return d;}
static bool finiteWord(U v){return (v&0x7f800000u)!=0x7f800000u;}
static original::V vectorWords(const Vec& v){return {{bits(v.x),bits(v.y),bits(v.z),bits(v.w)}};}
static std::array<U,18> recordWords(const Record& r){return {{bits(r.position.x),bits(r.position.y),bits(r.position.z),bits(r.position.w),bits(r.look.x),bits(r.look.y),bits(r.look.z),bits(r.look.w),bits(r.up.x),bits(r.up.y),bits(r.up.z),bits(r.up.w),bits(r.stationary),bits(r.max_fov),bits(r.min_fov),bits(r.max_fov_distance),bits(r.min_fov_distance),U(r.initial_zone)}};}
static std::array<U,7> rawWords(const Record& r){const auto& p=r.probabilities;return {{U(p.tv),U(p.follow),U(p.left_side),U(p.right_side),U(p.look_back),U(p.first_person),U(p.blur)}};}
static std::array<U,7> thresholdWords(const Record& r){const auto& p=r.thresholds;return {{bits(p.tv),bits(p.follow),bits(p.left_side),bits(p.right_side),bits(p.look_back),bits(p.first_person),bits(p.blur)}};}
static bool sameRecord(const Record& a,const Record& b){return recordWords(a)==recordWords(b)&&rawWords(a)==rawWords(b)&&thresholdWords(a)==thresholdWords(b)&&a.next_begin==b.next_begin&&a.next_count==b.next_count;}
static bool sameRequirements(const TVCameraRequirements49E& a,const TVCameraRequirements49E& b){return a.cameras==b.cameras&&a.links==b.links&&a.payload_bytes==b.payload_bytes;}
static TVCameraSpan49E<const char> span(const std::string& s){return {s.data(),s.size()};}
struct Fixture {
    std::array<Record,max_tv_cameras49e> cameras{};
    std::array<U,max_tv_camera_links49e> links{};
    TVCameraView49E view;
    TVCameraStorage49E storage(){return {{cameras.data(),cameras.size()},{links.data(),links.size()}};}
};
static void load(Fixture& f,const std::string& text){
    TVCameraDiagnostic49E diagnostic;TVCameraRequirements49E requirements;
    auto before=allocations;const bool inspected=inspectTVCamera49E(span(text),requirements,diagnostic);auto after=allocations;
    require(inspected,"valid camera inspection rejected");require(before==after,"inspection allocated");++hotCalls;
    before=allocations;const bool loaded=loadTVCamera49E(span(text),f.storage(),f.view,diagnostic);after=allocations;
    require(loaded,"valid camera load rejected");require(before==after,"load allocated");++hotCalls;
    require(diagnostic.code==TVCameraError49E::none&&f.view.loaded(),"load status mismatch");
    require(sameRequirements(requirements,f.view.requirements()),"inspect/load requirement mismatch");
    require(requirements.payload_bytes==requirements.cameras*sizeof(Record)+requirements.links*sizeof(U),"payload sizing mismatch");++loads;
}
static std::string oneCamera(const std::string& fields=""){return "[CameraPositions]\nNumberOfCameras=1\n[Camera1]\nNumNextCameras=1\nNextCamera1=1\n"+fields;}
static void reject(Fixture& f,TVCameraSpan49E<const char> text,TVCameraStorage49E storage,TVCameraError49E wanted=TVCameraError49E::none){
    const auto cameras=f.cameras;const auto links=f.links;const auto old=f.view.requirements();const U oldSwitch=bits(f.view.switchEvery());const bool oldLoaded=f.view.loaded();
    TVCameraRequirements49E request{19,23,29};TVCameraDiagnostic49E diagnostic;
    const bool inspection=inspectTVCamera49E(text,request,diagnostic);
    if(!inspection)require(request.cameras==19&&request.links==23&&request.payload_bytes==29,"failed inspection modified requirements");
    const bool result=loadTVCamera49E(text,storage,f.view,diagnostic);require(!result,"malformed/domain camera input accepted");
    require(diagnostic.code!=TVCameraError49E::none,"rejected load has no diagnostic");
    if(wanted!=TVCameraError49E::none)require(diagnostic.code==wanted,"camera error category mismatch");
    require(f.view.loaded()==oldLoaded&&bits(f.view.switchEvery())==oldSwitch&&sameRequirements(f.view.requirements(),old),"failed load changed view values");
    for(std::size_t i=0;i<cameras.size();++i)require(sameRecord(f.cameras[i],cameras[i]),"failed load modified camera values");
    require(f.links==links,"failed load changed link values");++rejections;
}
static void reject(Fixture& f,const std::string& text,TVCameraError49E wanted=TVCameraError49E::none){reject(f,span(text),f.storage(),wanted);}
static std::array<U,7> expectedThresholds(const std::array<U,7>& p){
    std::array<U,7> result{};volatile I start=signedWord(U(100)-p[0]);volatile float running=static_cast<float>(start);result[0]=bits(running);
    for(unsigned i=1;i<6;++i){volatile I source=signedWord(p[i]);volatile float converted=static_cast<float>(source);volatile float difference=running-converted;running=difference;result[i]=bits(running);}
    volatile I blur=signedWord(U(100)-p[6]);volatile float converted=static_cast<float>(blur);result[6]=bits(converted);return result;
}
static void testAuthored(Fixture& f,const std::string& text){
    load(f,text);require(f.view.cameras().size==19&&f.view.links().size==20,"authentic camera/link counts");
    require(bits(f.view.switchEvery())==authored::switch_every_word,"authentic SwitchEvery");
    for(unsigned i=0;i<19;++i){const auto w=recordWords(f.cameras[i]);const auto p=rawWords(f.cameras[i]),t=thresholdWords(f.cameras[i]);
        for(unsigned j=0;j<18;++j)require(w[j]==authored::record_words[i][j],"authentic complete record numeric words");
        for(unsigned j=0;j<7;++j){require(p[j]==authored::raw_probabilities[i][j],"authentic raw probability");require(t[j]==authored::threshold_words[i][j],"authentic ordered probability threshold");}
        require(f.cameras[i].next_begin==authored::rows[i][0]&&f.cameras[i].next_count==authored::rows[i][1],"authentic ordered link row");
        require(f.view.camera(i)!=nullptr&&sameRecord(*f.view.camera(i),f.cameras[i]),"valid camera accessor");
        const auto row=f.view.nextCameras(i);require(row.size==authored::rows[i][1],"valid next-camera accessor size");
        for(unsigned j=0;j<row.size;++j)require(row[j]==authored::links[authored::rows[i][0]+j],"authentic next-camera accessor values");
    }
    for(unsigned i=0;i<20;++i)require(f.links[i]==authored::links[i],"authentic all ordered links");
    require(f.cameras[4].max_fov==25&&f.cameras[4].min_fov==65&&f.cameras[4].max_fov_distance==400&&f.cameras[4].min_fov_distance==1572,"authored reversed bounds were normalized");
    require(f.view.camera(19)==nullptr&&f.view.camera(0xffffffffu)==nullptr&&f.view.nextCameras(19).empty(),"out-of-domain accessor admission");
}
static void testLoader(){
    Fixture f;load(f,oneCamera());const auto& r=f.cameras[0];
    const std::array<U,18> expected{{0,0,0,0x3f800000u,0,0,0,0x3f800000u,0,0,0,0x3f800000u,0,0x428c0000u,0x41200000u,0x43480000u,0x41a00000u,0xffffffffu}};
    require(recordWords(r)==expected,"original camera defaults");require(bits(f.view.switchEvery())==bits(5),"default switch timing");
    require(rawWords(r)==std::array<U,7>{{50,10,10,10,10,0,25}},"original raw probability defaults");
    require(thresholdWords(r)==std::array<U,7>{{bits(50),bits(40),bits(30),bits(20),bits(10),bits(10),bits(75)}},"original threshold defaults");
    require(r.next_begin==0&&r.next_count==1&&f.links[0]==0,"one-based self link conversion");
    load(f,oneCamera("FOV=33.25\nMaxFOV=99\nMinFOV=2\nPosition=-0,+2e0,3F\nLook=0.125,-2.5,3\nUp=9,8,7\nStationary=-4\nInitialZone=-2147483648\n"));
    require(f.cameras[0].max_fov==33.25f&&f.cameras[0].min_fov==33.25f,"FOV override ignored");
    require(bits(f.cameras[0].position.x)==0x80000000u&&f.cameras[0].position.y==2&&f.cameras[0].position.z==3,"vector decimal syntax/signed zero");
    require(vectorWords(f.cameras[0].look)==original::V{{bits(.125f),bits(-2.5f),bits(3),bits(1)}}&&vectorWords(f.cameras[0].up)==original::V{{bits(9),bits(8),bits(7),bits(1)}},"present Look/Up XYZ and W1");
    require(f.cameras[0].initial_zone==std::numeric_limits<I>::min()&&f.cameras[0].stationary==-4,"raw signed fields altered");
    for(const char* fov:{"-1","-1.00000002"}){load(f,oneCamera(std::string("FOV=")+fov+"\nMaxFOV=5\nMinFOV=90\nMaxFOVDistance=-3\nMinFOVDistance=900\nInitialZone=1\n"));require(f.cameras[0].max_fov==5&&f.cameras[0].min_fov==90&&f.cameras[0].max_fov_distance==-3&&f.cameras[0].min_fov_distance==900&&f.cameras[0].initial_zone==1,"converted-minus-one FOV fallback/reversed values/zone");}
    load(f,oneCamera("FOV=-0\nMaxFOV=5\nMinFOV=90\n"));require(bits(f.cameras[0].max_fov)==0x80000000u&&bits(f.cameras[0].min_fov)==0x80000000u,"FOV signed-zero override");
    load(f,oneCamera("FOV=-2\nMaxFOV=not-a-number\nMinFOV=also-ignored\n"));require(f.cameras[0].max_fov==-2&&f.cameras[0].min_fov==-2,"negative nonsentinel FOV must ignore alternate text");
    const std::array<std::array<I,7>,5> probabilities{{{{100,0,0,0,0,0,0}},{{-17,29,-31,101,3,-55,133}},{{2147483647,-2147483647-1,16777217,-16777217,1,2,-2147483647-1}},{{-2147483647-1,2147483647,2147483647,2147483647,2147483647,2147483647,2147483647}},{{0,16777217,1,-16777216,1,-1,-1}}}};
    for(const auto& p:probabilities){std::string fields;const char* names[]={"TV","Follow","LeftSide","RightSide","LookBack","FirstPerson","Blur"};std::array<U,7> raw{};
        for(unsigned i=0;i<7;++i){fields+=std::string(names[i])+"="+std::to_string(p[i])+"\n";raw[i]=U(p[i]);}load(f,oneCamera(fields));require(rawWords(f.cameras[0])==raw,"probabilities clamped or normalized");require(thresholdWords(f.cameras[0])==expectedThresholds(raw),"wrap32/order/binary32 threshold preparation");}
    // Decimal parsing supplies binary64 to the proven integer-only narrowing leaf.
    // These expected binary64 boundaries are independent of native from_chars.
    const std::array<std::pair<const char*,std::uint64_t>,7> finiteTexts{{
        {"1.000000059604644775390625",0x3ff0000010000000ull},
        {"1.000000178813934326171875",0x3ff0000030000000ull},
        {"1.1754943157898259e-38",0x380ffffff0000000ull},
        {"-4.9406564584124654e-324",0x8000000000000001ull},
        {"2.2250738585072014e-308",0x0010000000000000ull},
        {"-1e-5000",0x8000000000000000ull},
        {"3.4028234663852886e38",0x47efffffe0000000ull}}};
    tv49e_source::NarrowOracle leaf;
    for(const auto& entry:finiteTexts){load(f,oneCamera(std::string("Stationary=")+entry.first+"\n"));require(bits(f.cameras[0].stationary)==leaf.convert(entry.second),"decimal-to-original-soft-narrow boundary");}
    load(f,oneCamera("Stationary="+std::string(127,'0')+"1\n"));require(f.cameras[0].stationary==1,"128-byte scalar numeric boundary");
    reject(f,oneCamera("Stationary="+std::string(128,'0')+"1\n"));
    load(f,oneCamera("Position="+std::string(122,'0')+"0,0,0\n"));
    reject(f,oneCamera("Position="+std::string(123,'0')+"0,0,0\n"));
    reject(f,oneCamera()+"NextCamera1=1\n");
    reject(f,oneCamera()+"[CameraPositions]\nNumberOfCameras=1\n");
    reject(f,"[CameraPositions]\nSwitchEvery=5\n");
    std::string embeddedNull=oneCamera();embeddedNull.push_back('\0');reject(f,embeddedNull);
    const std::string reordered="; fixture\r\n[Camera2]\r\nNextCamera2=1\r\nNumNextCameras=2\r\nNextCamera1=1\r\n[CameraPositions]\r\nSwitchEvery=-2e0f ; keep raw\r\nNumberOfCameras=2\r\n[Camera1]\r\nNextCamera1=2\r\nNumNextCameras=1";
    load(f,reordered);require(f.view.switchEvery()==-2&&f.links[0]==1&&f.links[1]==0&&f.links[2]==0,"sections/order/repeated valid target admission");
    for(const auto& bad:std::array<std::string,26>{{"", "CameraPositions]\n", "[CameraPositions\n", "[CameraPositions]\nNumberOfCameras=one\n", "[CameraPositions]\nNumberOfCameras=-1\n", "[CameraPositions]\nNumberOfCameras=2147483648\n", "[CameraPositions]\nNumberOfCameras=2\n[Camera1]\nNumNextCameras=1\nNextCamera1=1\n", "[CameraPositions]\nNumberOfCameras=1\n[Camera1]\n", oneCamera("Unknown=1\n"), oneCamera("MaxFOV=1\nMaxFOV=2\n"), oneCamera()+"[Camera1]\n", oneCamera()+"[Other]\n", oneCamera("Position=1,2\n"), oneCamera("Position=1,2,3,4\n"), oneCamera("Position=1,,3\n"), oneCamera("Stationary=nan\n"), oneCamera("Stationary=inf\n"), oneCamera("Stationary=1e999\n"), oneCamera("Stationary=1e39\n"), oneCamera("Stationary=1.2junk\n"), oneCamera("Stationary=0x1p0\n"), oneCamera("InitialZone=1.5\n"), oneCamera("InitialZone=2147483648\n"), oneCamera("TV=-2147483649\n"), oneCamera("Position=1,2,nan\n"), oneCamera("Stationary=1e\n")}})reject(f,bad);
    for(const char* count:{"0","-1"})reject(f,std::string("[CameraPositions]\nNumberOfCameras=1\n[Camera1]\nNumNextCameras=")+count+"\n");
    for(const char* target:{"0","-1","2","2147483647"})reject(f,std::string("[CameraPositions]\nNumberOfCameras=1\n[Camera1]\nNumNextCameras=1\nNextCamera1=")+target+"\n");
    reject(f,"[CameraPositions]\nNumberOfCameras=1\n[Camera1]\nNumNextCameras=2\nNextCamera1=1\n");
    reject(f,{nullptr,1},f.storage(),TVCameraError49E::null_input);
    const auto good=oneCamera();auto insufficient=f.storage();insufficient.cameras.size=0;reject(f,span(good),insufficient,TVCameraError49E::insufficient_storage);
    insufficient=f.storage();insufficient.links.size=0;reject(f,span(good),insufficient,TVCameraError49E::insufficient_storage);
    auto nullStorage=f.storage();nullStorage.cameras={nullptr,1};reject(f,span(good),nullStorage,TVCameraError49E::null_storage);
    nullStorage=f.storage();nullStorage.links={nullptr,1};reject(f,span(good),nullStorage,TVCameraError49E::null_storage);
    std::string limit="[CameraPositions]\nNumberOfCameras=128\n";
    for(unsigned i=1;i<=128;++i){limit+="[Camera"+std::to_string(i)+"]\nNumNextCameras=8\n";for(unsigned j=1;j<=8;++j)limit+="NextCamera"+std::to_string(j)+"="+std::to_string((i+j-2)%128+1)+"\n";}
    load(f,limit);require(f.view.cameras().size==128&&f.view.links().size==1024,"explicit native count/link maximum");
    reject(f,"[CameraPositions]\nNumberOfCameras=129\n",TVCameraError49E::camera_limit);
    std::string tooManyLinks="[CameraPositions]\nNumberOfCameras=1\n[Camera1]\nNumNextCameras=1025\n";for(unsigned j=1;j<=1025;++j)tooManyLinks+="NextCamera"+std::to_string(j)+"=1\n";reject(f,tooManyLinks,TVCameraError49E::link_limit);
    std::string textLimit=oneCamera()+";";textLimit.resize(max_tv_camera_text_bytes49e,'x');load(f,textLimit);textLimit+='x';reject(f,textLimit,TVCameraError49E::input_limit);
    reject(f,"[CameraPositions]\nNumberOfCameras=0\n",TVCameraError49E::nonpositive_cameras);
    for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){require(std::fesetround(mode)==0,"host rounding mode unavailable");reject(f,good,TVCameraError49E::unsupported_rounding);}require(std::fesetround(FE_TONEAREST)==0,"restore nearest rounding");
}

static void testNarrow(){
    tv49e_source::NarrowOracle oracle;
    const auto check=[&](std::uint64_t b){
        if(((b>>52)&2047)==2047)return;
        const U expected=oracle.convert(b);float output=number(0x42f60000u);const auto before=allocations;
        const bool ok=convertTVCameraNumber49E(binary64(b),output);const auto after=allocations;
        require(ok==finiteWord(expected),"soft narrowing finite-result admission");
        require(bits(output)==(ok?expected:0x42f60000u),"original-word soft narrowing differential/atomicity");
        if(ok){require(before==after,"soft narrowing allocated");++hotCalls;}
        for(unsigned i=0;i<narrowVisits.size();++i){narrowVisits[i]+=oracle.hits[i];}narrowInstructions+=oracle.steps;++narrowCases;
    };
    for(std::uint64_t exponent=0;exponent<2047;++exponent)for(std::uint64_t sign=0;sign<2;++sign)
        for(std::uint64_t fraction:std::array<std::uint64_t,17>{{0,1,0x1fffff,0x200000,0x200001,0xfffffff,0x10000000,0x10000001,0x17fffffff,0x180000000,0x180000001,0x7ffffffffffff,0x8000000000000,0xffffff0000000,0xffffffffffffd,0xffffffffffffe,0xfffffffffffff}})
            check((sign<<63)|(exponent<<52)|fraction);
    std::uint64_t random=0x987654321abcdef0ull;
    for(unsigned i=0;i<250000;++i){random^=random<<13;random^=random>>7;random^=random<<17;check(random);}
    for(int mode:{FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){
        require(std::fesetround(mode)==0,"soft conversion rounding mode unavailable");
        for(std::uint64_t b:std::array<std::uint64_t,14>{{0,0x8000000000000000ull,1,0x8000000000000001ull,0x0010000000000000ull,0x380fffffe0000000ull,0x380ffffff0000000ull,0x36a0000000000000ull,0x3ff0000010000000ull,0x3ff0000030000000ull,0xbff0000030000000ull,0x47efffffe0000000ull,0x47effffff0000000ull,0x7fefffffffffffffull}})check(b);
        for(std::uint64_t b:std::array<std::uint64_t,6>{{0x7ff0000000000000ull,0xfff0000000000000ull,0x7ff8000000000000ull,0x7ff0000000000001ull,0xfff8000000000000ull,0xffffffffffffffffull}}){float output=number(0x80000000u);require(!convertTVCameraNumber49E(binary64(b),output)&&bits(output)==0x80000000u,"nonfinite narrowing changed output");++rejections;}
    }
    require(std::fesetround(FE_TONEAREST)==0,"restore narrowing rounding");
    require(oracle.convert(0x380ffffff0000000ull)==0x007fffffu,"original truncating float-underflow anchor");
    require(oracle.convert(0x8000000000000001ull)==0x80000000u,"original signed binary64 subnormal zero anchor");
    require(oracle.convert(0x3ff0000010000000ull)==0x3f800000u&&oracle.convert(0x3ff0000030000000ull)==0x3f800002u,"normal ties-to-even anchors");
}
static original::State originalState(const State& n){
    original::State s;s.current=n.current40;s.previous=n.previous44;s.stationary=U(n.stationary80);s.holdBits=bits(n.hold90);s.cameraPosition=vectorWords(n.positionA0);
    s.fieldC0=bits(n.fieldC0);s.fieldC4=bits(n.fieldC4);s.fieldC8=bits(n.fieldC8);s.wordCC=n.wordCC;s.sourceD0=n.sourceD0;s.filterWords[0]=bits(n.filterCurrent00);return s;
}
static void sameState(const State& n,const original::State& s){
    const auto got=originalState(n);require(got.current==s.current&&got.previous==s.previous,"selector current/previous differential");
    require(got.stationary==s.stationary&&got.holdBits==s.holdBits,"selector stationary/hold differential");
    require(got.cameraPosition==s.cameraPosition,"selector complete position differential");
    require(got.fieldC0==s.fieldC0&&got.fieldC4==s.fieldC4&&got.fieldC8==s.fieldC8&&got.wordCC==s.wordCC&&got.sourceD0==s.sourceD0,"selector all source reset fields differential");
    require(got.filterWords==s.filterWords,"selector filter-current/reset preservation differential");
}
static void accumulate(const original::Result& r){
    using namespace tv_camera49e_original;
    for(U pc:r.executed){++runtimeInstructions;
        if(pc>=selector_start&&pc<selector_start+4*selectorVisits.size())++selectorVisits[(pc-selector_start)/4];
        else if(pc>=reset_start&&pc<reset_start+4*resetVisits.size())++resetVisits[(pc-reset_start)/4];
        else if(pc>=random_start&&pc<random_start+4*randomVisits.size())++randomVisits[(pc-random_start)/4];
        else if(pc>=seed_start&&pc<seed_start+4*seedVisits.size())++seedVisits[(pc-seed_start)/4];
    }
}
struct SourceView {
    std::array<original::Record,max_tv_cameras49e> records{};
    std::array<original::LinkRow,max_tv_cameras49e> rows{};
    std::array<U,max_tv_camera_links49e> links{};
    std::size_t count=0,linkCount=0;
    original::View view()const{return {records.data(),count,rows.data(),links.data(),linkCount};}
    explicit SourceView(const Fixture& f,bool realAuthored=false){
        count=f.view.cameras().size;linkCount=f.view.links().size;
        for(std::size_t i=0;i<count;++i){const auto words=recordWords(f.cameras[i]);for(unsigned j=0;j<18;++j)records[i].words[j]=realAuthored?authored::record_words[i][j]:words[j];
            records[i].words[18]=0x6a000048u;records[i].words[19]=0x6a00004cu;
            rows[i]=realAuthored?original::LinkRow{authored::rows[i][0],authored::rows[i][1]}:original::LinkRow{f.cameras[i].next_begin,f.cameras[i].next_count};}
        for(std::size_t i=0;i<linkCount;++i)links[i]=realAuthored?authored::links[i]:f.links[i];
    }
};
static State sentry(){State s;s.current40=-1;s.previous44=71;s.stationary80=-19;s.hold90=.125f;s.positionA0={19,23,29,-0.0f};s.fieldC0=31;s.fieldC4=-37;s.fieldC8=41;s.wordCC=0xfedcba98u;s.sourceD0=43;s.filterCurrent00=-47;return s;}
static original::Result compareSelect(const Fixture& f,State& state,const Vec& player,float elapsed,TVCameraRandom49E& rng,bool realAuthored=false){
    SourceView source(f,realAuthored);TVCameraSelection49E output{{-101,103,-107,-0.0f},true};
    const auto expected=original::select(source.view(),originalState(state),vectorWords(player),bits(elapsed),vectorWords(output.position),rng.raw);
    const auto before=allocations;const bool ok=selectTVCamera49E(f.view,state,player,elapsed,rng,output);const auto after=allocations;
    require(ok,"supported finite native selector rejected");require(before==after,"selector allocated");++hotCalls;
    sameState(state,expected.state);require(vectorWords(output.position)==expected.output,"selector returned vec4 differential");
    require(output.changed==(expected.resultWord!=0)&&rng.raw==expected.rngRaw,"selector return/RNG word differential");
    require(expected.randomCalls==unsigned(output.changed),"RNG consumed other than exactly once on change");accumulate(expected);++runtimeCases;return expected;
}
static void loadSynthetic(Fixture& f){load(f,"[CameraPositions]\nNumberOfCameras=3\nSwitchEvery=999\n[Camera1]\nPosition=10,0,0\nStationary=25\nMaxFOV=71\nNumNextCameras=2\nNextCamera1=2\nNextCamera2=3\n[Camera2]\nPosition=-1,0,0\nStationary=75\nMaxFOV=72\nNumNextCameras=1\nNextCamera1=1\n[Camera3]\nPosition=1,0,0\nStationary=100\nMaxFOV=73\nNumNextCameras=1\nNextCamera1=1\n");}
static U testRandom=0x49e2d30du;
static U nextTest(){testRandom^=testRandom<<13;testRandom^=testRandom>>17;testRandom^=testRandom<<5;return testRandom;}
static float moderateFloat(){const U sign=nextTest()&0x80000000u;const U exp=(110u+nextTest()%30u)<<23;return number(sign|exp|(nextTest()&0x7fffffu));}
static void testResetAndRandom(){
    const State fresh;require(fresh.current40==-1&&fresh.previous44==0&&fresh.stationary80==0&&vectorWords(fresh.positionA0)==original::V{{0,0,0,0x3f800000u}},"proven constructor defaults/current/previous/position");
    const std::array<U,12> seeds{{0,1,2,0x7fffffffu,0x80000000u,0xfffffffeu,0xffffffffu,0xfc77a683u,0xeb3191e8u,0x8dbdbb1eu,0x7c77a683u,0x0dbdbb1eu}};
    for(U seed:seeds){TVCameraRandom49E rng{nextTest()};const auto seeded=original::seed(rng.raw,seed);const auto before=allocations;seedTVCameraRandom49E(rng,seed);const auto after=allocations;require(before==after&&rng.raw==seeded.rngRaw,"seed exact original raw word");accumulate(seeded);++rngCases;++hotCalls;
        for(unsigned step=0;step<128;++step){const auto expected=original::random(rng.raw);const auto count=allocations;const U result=nextTVCameraRandom49E(rng);const auto end=allocations;require(count==end&&result==expected.resultWord&&rng.raw==expected.rngRaw,"LCG wrap/mask original-word differential");require((result&0x80000000u)==0,"LCG returned sign bit");accumulate(expected);++rngCases;++hotCalls;}
    }
    for(I current:{-2147483647-1,-17,-1,0,1,2,2147483647}){auto s=sentry();s.current40=current;s.hold90=number(nextTest());const auto expected=original::reset(originalState(s),0xabcdef01u,{{1,2,3,4}});const auto before=allocations;resetTVCameraSelector49E(s);const auto after=allocations;
        require(before==after,"reset allocated");sameState(s,expected.state);require(s.current40==0&&bits(s.hold90)==0x3f19999au&&expected.rngRaw==0xabcdef01u&&expected.output==original::V{{1,2,3,4}}&&expected.randomCalls==0,"reset exact two fields/no RNG");accumulate(expected);++resetCases;++hotCalls;}
}
static void testRuntime(const Fixture& authoredFixture){
    Fixture f;loadSynthetic(f);
    for(int mode:{FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){
        require(std::fesetround(mode)==0,"runtime rounding mode unavailable");
        for(unsigned target=0;target<19;++target){State s=sentry();s.current40=-17;TVCameraRandom49E rng{target};compareSelect(authoredFixture,s,authoredFixture.cameras[target].position,0,rng,true);require(s.current40==I(target)&&s.previous44==-17,"authored initial nearest selection");++authoredFrames;}
        {State s=sentry();TVCameraRandom49E rng{1};compareSelect(authoredFixture,s,authoredFixture.cameras[0].position,0,rng,true);++authoredFrames;
            for(unsigned target=1;target<=19;++target){const unsigned chosen=target%19;const auto p=authoredFixture.cameras[chosen].position;unsigned calls=0;
                while(s.current40!=I(chosen)&&calls<8){compareSelect(authoredFixture,s,p,.2f,rng,true);++calls;++authoredFrames;}require(s.current40==I(chosen),"authored complete path including19-to1 failed");}
            require(s.previous44==18&&s.current40==0,"authored cyclic wrap transition");}
        for(unsigned branch:{4u,5u}){State s=sentry();s.current40=3;s.hold90=0;TVCameraRandom49E rng{0xffffffffu};compareSelect(authoredFixture,s,authoredFixture.cameras[branch].position,0,rng,true);require(s.current40==I(branch),"authored camera4 branch5/6");++authoredFrames;}
        {State s=sentry();TVCameraRandom49E rng{1};compareSelect(f,s,{0,0,0,1},0,rng);require(s.current40==1,"initial equal-distance tie did not keep first");}
        {State s=sentry();s.current40=0;s.hold90=0;TVCameraRandom49E rng{1};compareSelect(f,s,{0,0,0,1},0,rng);require(s.current40==1,"ordered link tie did not keep first");}
        {State s=sentry();s.current40=0;s.hold90=0;TVCameraRandom49E rng{1};compareSelect(f,s,{5.5f,0,0,1},0,rng);require(s.current40==2,"current/link tie did not choose link");}
        {State s=sentry();s.current40=0;s.hold90=.6f;TVCameraRandom49E rng{1};const U seed=rng.raw;compareSelect(f,s,{-1,0,0,1},1,rng);require(s.current40==0&&s.hold90<0&&rng.raw==seed,"positive hold crossing switched on same call");compareSelect(f,s,{-1,0,0,1},0,rng);require(s.current40==1&&bits(s.hold90)==0x3f19999au,"crossed hold did not switch next call");}
        {State s=sentry();s.current40=0;s.hold90=.6f;const auto old=originalState(s);TVCameraRandom49E rng{1};compareSelect(f,s,{10,0,0,1},100,rng);require(bits(s.hold90)==old.holdBits&&rng.raw==1&&s.fieldC0==31&&s.filterCurrent00==-47&&s.previous44==71,"unchanged desired must preserve timer/source/filter/RNG");compareSelect(f,s,{-1,0,0,1},-.25f,rng);require(s.hold90>.6f&&s.current40==0&&rng.raw==1,"negative elapsed was clamped");}
        for(U seed:{0u,1u,0xffffffffu,0xfc77a683u,0x8dbdbb1eu}){
            volatile I word=I((seed*0x41c64e6du+0x3039u)&0x7fffffffu);volatile float converted=static_cast<float>(word);volatile float scaled=0x1p-31f*converted;volatile float percentage=100.0f*scaled;
            for(float threshold:{std::nextafter(float(percentage),-std::numeric_limits<float>::infinity()),float(percentage),std::nextafter(float(percentage),std::numeric_limits<float>::infinity()),-1.0f,101.0f}){
                f.cameras[1].stationary=threshold;State s=sentry();TVCameraRandom49E rng{seed};compareSelect(f,s,{-1,0,0,1},0,rng);require(s.stationary80==(percentage<threshold?1:0),"stationary strict threshold or percentage rounding");}
        }
        f.cameras[1].stationary=75;
        for(unsigned iteration=0;iteration<256;++iteration){
            for(unsigned c=0;c<3;++c){f.cameras[c].position={moderateFloat(),moderateFloat(),moderateFloat(),number(0x80000000u|(c+1))};f.cameras[c].max_fov=moderateFloat();f.cameras[c].stationary=moderateFloat();}
            State s=sentry();s.current40=iteration%4==0?-7:I(iteration%3);s.hold90=(iteration%3==0)?0:moderateFloat();TVCameraRandom49E rng{nextTest()};Vec player{moderateFloat(),moderateFloat(),moderateFloat(),1};compareSelect(f,s,player,moderateFloat(),rng);
        }
        // Directed rounding exposes the original player-minus-camera operand order.
        f.cameras[0].position={1,0,0,1};f.cameras[1].position={-.8f,0,0,1};f.cameras[2].position={10,10,10,1};
        {State s=sentry();TVCameraRandom49E rng{1};compareSelect(f,s,{.1f,0,0,1},0,rng);}
        // Exact source order separates rounded-equal candidates under nearest.
        f.cameras[0].position={1,0x1p-12f,0x1p-12f,1};f.cameras[1].position={0x1p-12f,0x1p-12f,1,1};f.cameras[2].position={10,10,10,1};
        {State s=sentry();TVCameraRandom49E rng{1};compareSelect(f,s,{0,0,0,1},0,rng);if(mode==FE_TONEAREST)require(s.current40==0,"distance accumulation was reassociated");}
        for(U coordinate:{0u,0x80000000u,1u,0x80000001u,0x007fffffu,0x00800000u,0x00800001u,0x3f800001u,0x5e800000u}){
            for(unsigned c=0;c<3;++c){f.cameras[c].position={number(coordinate),number(coordinate^0x80000000u),0,number(0x80000000u)};f.cameras[c].max_fov=70;f.cameras[c].stationary=0;}
            State s=sentry();TVCameraRandom49E rng{1};compareSelect(f,s,{0,0,0,1},0,rng);
        }
        require(std::fesetround(FE_TONEAREST)==0,"restore runtime rounding between load fixtures");loadSynthetic(f);
    }
}
static void rejectedSelection(Fixture& f,State s,Vec player,float elapsed){
    const auto old=originalState(s);TVCameraRandom49E rng{0xcafebabeu};TVCameraSelection49E output{{71,-73,79,-0.0f},true};const auto out=vectorWords(output.position);
    require(!selectTVCamera49E(f.view,s,player,elapsed,rng,output),"unsupported native selector values accepted");sameState(s,old);
    require(rng.raw==0xcafebabeu&&vectorWords(output.position)==out&&output.changed,"rejected selector changed RNG/output");++rejections;
}
static void testRuntimeAdmission(){
    Fixture f;State s=sentry();rejectedSelection(f,s,{0,0,0,1},0);loadSynthetic(f);
    for(I current:{3,127,2147483647}){s=sentry();s.current40=current;rejectedSelection(f,s,{0,0,0,1},0);}
    for(U word:{0x7f800000u,0xff800000u,0x7fc12345u}){
        for(unsigned field=0;field<3;++field){Vec p{0,0,0,1};if(field==0)p.x=number(word);else if(field==1)p.y=number(word);else p.z=number(word);rejectedSelection(f,sentry(),p,0);}
        rejectedSelection(f,sentry(),{0,0,0,1},number(word));
        s=sentry();s.current40=0;s.hold90=number(word);rejectedSelection(f,s,{-1,0,0,1},0);
        const auto saved=f.cameras[1];f.cameras[1].max_fov=number(word);rejectedSelection(f,sentry(),{-1,0,0,1},0);f.cameras[1]=saved;
        f.cameras[1].stationary=number(word);rejectedSelection(f,sentry(),{-1,0,0,1},0);f.cameras[1]=saved;
        for(unsigned field=0;field<3;++field){if(field==0)f.cameras[1].position.x=number(word);else if(field==1)f.cameras[1].position.y=number(word);else f.cameras[1].position.z=number(word);rejectedSelection(f,sentry(),{0,0,0,1},0);f.cameras[1]=saved;}
    }
    const auto saved=f.cameras[1];
    for(Vec position:std::array<Vec,3>{{{1e20f,0,0,1},{1.4e19f,1.4e19f,0,1},{std::numeric_limits<float>::max(),0,0,1}}}){f.cameras[1].position=position;rejectedSelection(f,sentry(),{0,0,0,1},0);}
    f.cameras[1].position={std::numeric_limits<float>::max(),0,0,1};rejectedSelection(f,sentry(),{-std::numeric_limits<float>::max(),0,0,1},0);f.cameras[1]=saved;
    s=sentry();s.current40=0;s.hold90=std::numeric_limits<float>::max();rejectedSelection(f,s,{-1,0,0,1},-std::numeric_limits<float>::max());
    // Unused numeric fields are intentionally not admitted by a global scan.
    s=sentry();s.current40=1;s.hold90=number(0x7fc12345u);s.filterCurrent00=number(0x7fc54321u);f.cameras[1].max_fov=number(0x7fc00123u);f.cameras[1].stationary=number(0x7fc00456u);f.cameras[2].position.x=number(0x7fc00789u);
    const auto old=originalState(s);TVCameraRandom49E rng{1};TVCameraSelection49E output;
    const auto before=allocations;const bool ok=selectTVCamera49E(f.view,s,{-1,0,0,1},0,rng,output);const auto after=allocations;
    require(ok&&before==after&&!output.changed&&rng.raw==1,"unconsumed native fields incorrectly rejected or allocated");
    auto wanted=old;wanted.cameraPosition=vectorWords(f.cameras[1].position);sameState(s,wanted);++hotCalls;
}
template<std::size_t N> static unsigned covered(const std::array<std::uint64_t,N>& visits){unsigned n=0;for(auto v:visits)n+=v!=0;return n;}
int main(int argc,char** argv){
    try{
        require(std::numeric_limits<float>::is_iec559&&sizeof(float)==4&&sizeof(double)==8,"IEEE numeric model required");require(std::fesetround(FE_TONEAREST)==0,"nearest rounding required");
        const char* path=argc>1?argv[1]:"evidence/tvCamera49E/authored/taTVCam.pos";std::ifstream input(path,std::ios::binary);require(bool(input),"authored camera fixture unavailable");const std::string text((std::istreambuf_iterator<char>(input)),{});
        Fixture fixture;testAuthored(fixture,text);testLoader();testNarrow();testResetAndRandom();testRuntime(fixture);testRuntimeAdmission();
        require(covered(narrowVisits)==119,"finite narrowing original-word coverage changed");
        require(covered(selectorVisits)==201,"reachable selector original-word coverage incomplete");
        require(covered(resetVisits)==resetVisits.size()&&covered(randomVisits)==randomVisits.size()&&covered(seedVisits)==seedVisits.size(),"reset/random/seed word coverage incomplete");
        std::cout<<"{\"status\":\"pass\",\"checks\":"<<checks<<",\"cases\":{\"loads\":"<<loads<<",\"rejected_native_values\":"<<rejections<<",\"narrow\":"<<narrowCases<<",\"selector\":"<<runtimeCases<<",\"reset\":"<<resetCases<<",\"rng\":"<<rngCases<<",\"authored_trajectory_frames\":"<<authoredFrames<<"},\"hot_calls\":"<<hotCalls<<",\"hot_allocations\":0,\"word_coverage\":{\"narrow\":["<<covered(narrowVisits)<<","<<narrowVisits.size()<<"],\"selector\":["<<covered(selectorVisits)<<","<<selectorVisits.size()<<"],\"reset\":["<<covered(resetVisits)<<","<<resetVisits.size()<<"],\"rng\":["<<covered(randomVisits)<<","<<randomVisits.size()<<"],\"seed\":["<<covered(seedVisits)<<","<<seedVisits.size()<<"]},\"instructions\":{\"narrow\":"<<narrowInstructions<<",\"runtime\":"<<runtimeInstructions<<"},\"proof\":{\"runtime\":\"strict host binary32 instruction model, not universal PS2 silicon MADD proof\",\"runtime_rounding_modes\":4,\"ingestion_rounding\":\"nearest only\",\"soft_narrowing_rounding_modes\":4,\"finite_boundaries\":\"signed zero, binary64 subnormal flush, binary32 underflow truncation, normal ties, finite overflow rejection, distance arithmetic admission\",\"authored_records\":19,\"authored_links\":20,\"original_invalid_memory_executions\":0}}\n";
        return 0;
    }catch(const std::exception& e){std::fesetround(FE_TONEAREST);std::cerr<<"tv_camera49e: "<<e.what()<<" (checks="<<checks<<", loads="<<loads<<", narrow="<<narrowCases<<", selector="<<runtimeCases<<")\n";return 1;}
}
