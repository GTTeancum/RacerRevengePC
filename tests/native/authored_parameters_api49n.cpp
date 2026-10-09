#include "../../src/game/authored_parameters49n.h"
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <initializer_list>
#include <type_traits>

using namespace rr::game;
using K=AuthoredKey49N;
static unsigned checks=0,hotCalls=0;
static std::size_t allocations=0;
void* operator new(std::size_t n){++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
static void require(bool okay,const char* message){++checks;if(!okay){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
static std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
static float fp(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
static float multiply(float a,float b){volatile float v=a*b;return v;}
static float subtract(float a,float b){volatile float v=a-b;return v;}
template<class F> static bool hot(F function){const auto before=allocations;const bool okay=function();require(allocations==before,"native allocation");++hotCalls;return okay;}
static AuthoredValue49N& value(AuthoredParameters49N& p,K key){return p.values[static_cast<std::size_t>(key)];}
static AuthoredParameters49N fixture(){
    // Explicit numeric fixture matching the known Pod01 mapped values. Actual
    // PHY byte parsing/source conversion is tested separately, not simulated.
    const float values[]={
        1000,1,1,1,6,10,
        8475,8475,8475,3750,9000,900,900,900,500,900,
        1800,1600,1,1,110,100,200,25,
        0,0,.02f,25000,-10000,-1000,-2000,100,-5000,
        500,8000,.00005f,-70000,-40,-5000,40,-20,-2000,300,
        -8000,-8000,-8000,-5000,-8000,0,
        .9f,.9f,1,1,1,1,250,1,450,1,4,3
    };
    static_assert(sizeof(values)/sizeof(*values)==authoredKeyCount49N);
    AuthoredParameters49N raw{};
    for(std::size_t i=0;i<authoredKeyCount49N;++i)raw.values[i]={values[i],true};
    raw.leftEnginePosition={{{-.3f,0,0}},true};raw.rightEnginePosition={{{.3f,0,0}},true};
    raw.tuning={true,{1,true},{0,true},{0,true},{0,true},{0,true}};
    return raw;
}
static AuthoredBinding49N prior(){
    AuthoredBinding49N p{};
    p.main.body.mass=13;p.main.body.inverseMass=.125f;
    p.main.body.linearAccelerationLimitSquared=19;p.main.body.angularAccelerationLimitSquared=23;
    p.main.body.linearVelocityLimitSquared=29;p.main.body.angularVelocityLimitSquared=31;
    p.main.body.translationEnabled[0]=true;p.main.body.rotationEnabled[2]=true;
    p.main.aero.velocityDamping130={{.01f,.02f,.03f}};
    p.main.centerOffset={{1,2,3}};p.main.probe204=37;p.main.probeLocal140[3]=41;
    p.main.effective434260[1]=43;p.main.effective4342C8=47;p.main.effective434290={{53,59,61}};
    p.main.effectiveImpactZero434630={{67,71,73}};p.main.gravityForceX=79;p.main.gravityForceZ=83;
    p.controls.scale4346B8=.625f;p.controls.bias4346C0=.125f;
    return p;
}
static bool required(K k,bool useValues,bool mode,bool profile){
    switch(k){
    case K::MaxThrustA:case K::TheoreticalTopSpeedA:case K::SpoilerYawMomentA:return useValues;
    case K::MaxThrustB:case K::MaxThrustC:case K::TheoreticalTopSpeedB:case K::TheoreticalTopSpeedC:
    case K::SpoilerYawMomentB:case K::SpoilerYawMomentC:return false;
    case K::MinMaxThrust:case K::MaxMaxThrust:case K::MinTheoreticalTopSpeed:case K::MaxTheoreticalTopSpeed:
    case K::MinSpoilerYawMoment:case K::MaxSpoilerYawMoment:return !useValues||profile;
    case K::SpoilerYawMomentOffset:return (!useValues&&!mode)||profile;
    case K::BaseAbilityMaxThrust:case K::MaxAbilityMaxThrust:case K::BaseAbilityTheoreticalTopSpeed:
    case K::MaxAbilityTheoreticalTopSpeed:case K::BaseAbilitySpoilerYawMoment:case K::MaxAbilitySpoilerYawMoment:return !useValues;
    default:return true;
    }
}
int main(){
    require(std::fesetround(FE_TONEAREST)==0,"rounding setup");
    static_assert(std::is_trivially_copyable_v<AuthoredParameters49N>);
    static_assert(std::is_trivially_copyable_v<AuthoredBinding49N>);
    const auto initial=prior();auto raw=fixture();AuthoredBinding49N out{};
    AuthoredSelectionContext49N c{AuthoredClass49N::A,false,false,false,false,{}};
    require(hot([&]{return bindAuthoredParameters49N(raw,c,initial,out);}),"base bind");
    require(out.main.engine.maxThrust==8475&&out.main.aero.steering100==-8000,"class values");
    require(bits(out.gravity378)==bits(multiply(6,fp(0x4200cccd)))&&bits(out.airborneGravity37C)==bits(multiply(10,fp(0x4200cccd))),"gravity conversion");
    require(bits(out.main.engine.theoreticalTopSpeed)==bits(multiply(900,fp(0x3fbbbb84))),"speed conversion");
    require(bits(out.engineDifference5C)==bits(subtract(out.main.engine.theoreticalTopSpeed,multiply(100,fp(0x3fbbbb84)))),"final5C");
    require(out.controls.speed1B84==250&&out.controls.speed2B8C==450&&out.rates.x==4&&out.rates.y==3,"joystick remains authored units");
    require(out.controls.scale4346B8==initial.controls.scale4346B8&&out.controls.bias4346C0==initial.controls.bias4346C0,"control globals preserved");
    require(std::memcmp(&out.main.aero.velocityDamping130,&initial.main.aero.velocityDamping130,sizeof(Vector3_48C))==0,"unwritten damping preserved");
    require(out.main.body.linearAccelerationLimitSquared==19&&out.main.body.rotationEnabled[2]&&out.main.centerOffset.v[2]==3&&out.main.gravityForceX==79,"unwritten main preserved");
    require(bits(out.inverseInertia140.v[1])==0x80000000u&&bits(out.inverseInertia140.v[4])==0x80000000u,"inverse signed cofactors");
    require(bits(out.inverseInertia140.v[15])==0&&out.inertia100.v[15]==1,"inverse full fourth lane");
    auto alias=initial;require(hot([&]{return bindAuthoredParameters49N(raw,c,alias,alias);}),"bind alias");
    require(std::memcmp(&alias,&out,sizeof out)==0,"alias words");
    struct ClassCase{const char* text;AuthoredClass49N expected;};
    const ClassCase classes[]={{"ta",AuthoredClass49N::A},{"tb",AuthoredClass49N::B},{"tc",AuthoredClass49N::C},{"sb",AuthoredClass49N::C},
        {"ra",AuthoredClass49N::B},{"sc",AuthoredClass49N::B},{"ba",AuthoredClass49N::A},{"SB",AuthoredClass49N::C},{"rab",AuthoredClass49N::A},{"",AuthoredClass49N::C}};
    for(const auto& x:classes){AuthoredClass49N result{};require(hot([&]{return selectAuthoredClass49N(x.text,std::strlen(x.text),result);}),"track selection");require(result==x.expected,"track class");}
    auto classOut=AuthoredClass49N::B;require(!selectAuthoredClass49N(nullptr,1,classOut)&&classOut==AuthoredClass49N::B,"null selection atomic");
    require(!selectAuthoredClass49N("a\0b",3,classOut)&&classOut==AuthoredClass49N::B,"embedded null atomic");
    require(!selectAuthoredClass49N("a",256,classOut),"bounded length before read");
    raw.tuning.useAValues={1,true};raw.tuning.useBValues={1,true};raw.tuning.useCValues={1,true};
    value(raw,K::MaxThrustA).value=100;value(raw,K::MaxThrustB).value=200;value(raw,K::MaxThrustC).value=300;
    require(bindAuthoredParameters49N(raw,c,initial,out)&&out.selectedClass==AuthoredClass49N::C&&out.main.engine.maxThrust==300,"C override wins");
    raw.tuning.useCValues.value=2;require(bindAuthoredParameters49N(raw,c,initial,out)&&out.selectedClass==AuthoredClass49N::B,"overrides require exactly1");
    raw.tuning.sectionPresent=false;require(bindAuthoredParameters49N(raw,c,initial,out)&&out.selectedClass==AuthoredClass49N::A&&out.usedAuthoredClassValues,"absent Tuning default");
    raw=fixture();raw.tuning.useValues.present=false;require(bindAuthoredParameters49N(raw,c,initial,out)&&out.usedAuthoredClassValues,"missing UseValues default");
    raw=fixture();c.configurationEntryNonzero=true;c.applyProfileOverwrite=true;c.profileAbilitiesPresent=true;c.profile={.2f,1.5f,.25f};
    require(bindAuthoredParameters49N(raw,c,initial,out),"profile and configuration");
    require(out.main.engine.maxThrust==4800&&out.main.aero.steering100==-5750,"profile selected values");
    require(out.main.engine.torqueBasis[0].v[1]==4000&&out.main.engine.torqueBasis[1].v[1]==-4000,"pre-profile torque persists");
    require(bits(out.main.engine.theoreticalTopSpeed)==bits(multiply(fp(0x3fbbbb84),1100)),"speed profile intentionally unclamped");
    c.profile={2,0,-1};require(bindAuthoredParameters49N(raw,c,initial,out)&&out.main.engine.maxThrust==9000&&out.main.aero.steering100==-5000,"increasing and decreasing clamp ends");
    raw.leftEnginePosition.present=false;raw.rightEnginePosition.value.v[0]=std::numeric_limits<float>::quiet_NaN();
    require(bindAuthoredParameters49N(raw,c,initial,out),"overwritten raw vectors not demanded");
    c.mode58Nonzero=true;require(!bindAuthoredParameters49N(raw,c,initial,out),"live authored vectors demanded");
    raw=fixture();c={AuthoredClass49N::A,false,false,false,false,{}};
    value(raw,K::Weight).value=-1000;require(bindAuthoredParameters49N(raw,c,initial,out),"negative mass setter path");
    require(out.main.body.mass==initial.main.body.mass&&out.main.body.inverseMass==initial.main.body.inverseMass&&!out.massSetterWrote,"nonpositive mass preserves");
    CoupledBodyParameters49M nonpositiveRecipient{},nonpositiveProjected{};
    nonpositiveRecipient.main.body.mass=151;nonpositiveRecipient.main.body.inverseMass=157;
    require(projectAuthoredCoupledParameters49N(out,nonpositiveRecipient,nonpositiveProjected)&&nonpositiveProjected.main.body.mass==151&&nonpositiveProjected.main.body.inverseMass==157,"nonpositive projection preserves recipient mass");
    value(raw,K::Weight).value=0;require(bindAuthoredParameters49N(raw,c,initial,out),"singular original inverse");
    for(float f:out.inverseInertia140.v)require(bits(f)==0,"singular inverse all positive zero");

    // Demand matrix covers active/ignored values on all tuning/profile/mode
    // routes. Missing lookup and NaN are different admission failures.
    for(unsigned use=0;use<2;++use)for(unsigned mode=0;mode<2;++mode)for(unsigned profile=0;profile<2;++profile){
        raw=fixture();raw.tuning.useValues.value=static_cast<std::int32_t>(use);
        c={AuthoredClass49N::A,mode!=0,false,profile!=0,true,{.4f,.5f,.6f}};
        for(std::size_t i=0;i<authoredKeyCount49N;++i){
            const bool demanded=required(static_cast<K>(i),use!=0,mode!=0,profile!=0);
            for(unsigned invalid=0;invalid<2;++invalid){auto changed=raw;
                if(invalid)changed.values[i].value=std::numeric_limits<float>::quiet_NaN();else changed.values[i].present=false;
                out=initial;const auto before=out;const bool okay=hot([&]{return bindAuthoredParameters49N(changed,c,initial,out);});
                require(okay==!demanded,"key demand");if(!okay)require(std::memcmp(&before,&out,sizeof out)==0,"key rejection atomic");
            }
        }
    }
    raw=fixture();c={AuthoredClass49N::A,false,false,true,false,{}};out=initial;
    require(!bindAuthoredParameters49N(raw,c,initial,out)&&std::memcmp(&initial,&out,sizeof out)==0,"profile absence atomic");
    c.applyProfileOverwrite=false;c.profile={std::numeric_limits<float>::quiet_NaN(),0,0};
    require(bindAuthoredParameters49N(raw,c,initial,out),"inactive profile ignored");
    auto broken=raw;value(broken,K::Weight).value=std::numeric_limits<float>::max();out=initial;
    require(!bindAuthoredParameters49N(broken,c,initial,out)&&std::memcmp(&initial,&out,sizeof out)==0,"overflow atomic");
    c.inferredClass=static_cast<AuthoredClass49N>(0);require(!bindAuthoredParameters49N(raw,c,initial,out),"invalid class");
    c.inferredClass=AuthoredClass49N::A;require(bindAuthoredParameters49N(raw,c,initial,out),"projection fixture");
    CoupledBodyParameters49M before{};before.main=initial.main;before.main.body.mass=101;before.mainSphere.centerOffsetW=103;
    before.main.aero.velocityDamping130={{107,109,113}};before.secondary.body.mass=127;before.secondary.centerOffset={{131,137,139}};
    CoupledBodyParameters49M projected{};
    require(hot([&]{return projectAuthoredCoupledParameters49N(out,before,projected);}),"typed49M projection");
    require(projected.main.body.mass==out.main.body.mass&&projected.main.engine.maxThrust==8475,"projected numerical body and engine");
    require(std::memcmp(&before.secondary,&projected.secondary,sizeof before.secondary)==0,"secondary preserved bytewise");
    require(projected.main.aero.velocityDamping130.v[2]==113&&projected.mainSphere.centerOffsetW==103,"recipient unresolved values preserved");
    for(float f:projected.mainSphere.inverseInertiaExtra)require(bits(f)==0,"all supplemental inverse lanes transferred");
    auto same=before;require(projectAuthoredCoupledParameters49N(out,same,same)&&std::memcmp(&same,&projected,sizeof same)==0,"projection alias");
    auto inconsistent=out;inconsistent.main.body.inverseInertiaColumns[0].v[0]=std::numeric_limits<float>::quiet_NaN();
    require(projectAuthoredCoupledParameters49N(inconsistent,before,same)&&std::memcmp(&same,&projected,sizeof same)==0,"full inverse is canonical projection source");
    SubstepSelectionContext49K oldSelection{1,2,3,4,5,6},selection{};
    SecondaryInputs49M oldSecondary{};oldSecondary.targetRelative2F0={{7,8,9,10}};oldSecondary.effective4346C8=11;SecondaryInputs49M secondary{};
    require(hot([&]{return projectAuthoredGravity49N(out,oldSelection,oldSecondary,selection,secondary);}),"gravity projection");
    require(selection.vehicle378==out.gravity378&&secondary.gravity37C==out.airborneGravity37C,"gravity shared source");
    require(selection.effective434730==3&&selection.vehicle64==5&&secondary.targetRelative2F0.v[3]==10&&secondary.effective4346C8==11,"gravity context preserved");
    auto badBinding=out;badBinding.main.engine.maxThrust=std::numeric_limits<float>::infinity();same=before;
    require(!projectAuthoredCoupledParameters49N(badBinding,before,same)&&std::memcmp(&same,&before,sizeof same)==0,"projection atomic nonfinite");
    badBinding=out;badBinding.gravity378=std::numeric_limits<float>::quiet_NaN();selection=oldSelection;secondary=oldSecondary;
    require(!projectAuthoredGravity49N(badBinding,oldSelection,oldSecondary,selection,secondary)&&std::memcmp(&selection,&oldSelection,sizeof selection)==0&&std::memcmp(&secondary,&oldSecondary,sizeof secondary)==0,"gravity atomic nonfinite");
    for(int rounding:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){
        require(std::fesetround(rounding)==0,"rounding change");out=initial;same=before;
        require(!bindAuthoredParameters49N(raw,c,initial,out)&&std::memcmp(&out,&initial,sizeof out)==0,"bind rounding atomic");
        require(!projectAuthoredCoupledParameters49N(initial,before,same)&&std::memcmp(&same,&before,sizeof same)==0,"projection rounding atomic");
        require(!projectAuthoredGravity49N(initial,oldSelection,oldSecondary,selection,secondary),"gravity rounding gate");
    }
    require(std::fesetround(FE_TONEAREST)==0,"rounding restore");
    std::printf("authored_parameters_api49n: %u checks, %u allocation-free calls passed\n",checks,hotCalls);
}
