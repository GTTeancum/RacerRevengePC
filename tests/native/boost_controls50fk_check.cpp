#include "boost_controls50fk_fixture.h"
#include "../../src/game/advancement_selectors49k.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
using namespace rr::game;
static unsigned checks=0;
static void check(bool value,const char* why){++checks;if(!value)throw std::runtime_error(why);}
static std::uint32_t bits(float value){std::uint32_t out;std::memcpy(&out,&value,4);return out;}
int main(int argc,char** argv){try{
    check(argc==2,"usage: boost_controls50fk_check Pod01.phy");
    check(std::fesetround(FE_TONEAREST)==0,"rounding");
    std::ifstream file(argv[1],std::ios::binary);check(bool(file),"PHY open");
    const std::string text{std::istreambuf_iterator<char>(file),{}};
    AuthoredSelectionContext49N selection{AuthoredClass49N::A,false,true,false,false,{}};
    AuthoredBinding49N authored{};authored.usedAuthoredClassValues=true;
    authored.selectedClass=AuthoredClass49N::A;authored.engineDifference5C=1000.f;
    BoostParameters50FK p{};float cf8=0.f;
    check(boost50fk_fixture::bind(text.data(),text.size(),selection,authored,p,cf8),"authored class cooling");
    check(bits(cf8)==0x3bbac711u&&bits(p.heatRate88)==0x3e2e147bu,"source cooling and constructor constants");
    BoostInput50FK in{.016f,0.f,0.f,0.f,1,0,2,123,false};
    BoostState50FK state{},out{};BoostDiagnostic50FK d{};
    auto step=[&]{check(advancePlayerBoost50FK(in,p,state,out,d),"native boost step");state=out;};
    step();check(state.vehicle64==0&&state.heat7C==0,"held without thrust does not start");
    in.engine70=1;in.engine74=1;step();
    check(state.vehicle64==1&&state.vehicle68==0&&state.duration6C==.016f&&state.heat7C>0,"held starts native boost");
    check(selectEngineFlags49K(state.vehicle64,in.flagsB9C).engine34==1,"native engine selector consumes state1");
    in.engine70=0;in.engine74=0;step();
    check(state.vehicle64==1&&state.vehicle68==1,"zero-thrust held retains source state");
    in.flagsB9C=0;step();check(state.vehicle64==0&&state.duration6C==0,"release clears state and duration");
    in.flagsB9C=3;in.engine70=1;in.engine74=1;step();
    check(state.vehicle64==0,"repair flag inhibits boost");
    in.flagsB9C=1;state.heat7C=.999f;step();
    check(state.vehicle64==2&&state.heat7C==1,"source heat clamps into state2");
    check(selectEngineFlags49K(state.vehicle64,in.flagsB9C).engine34==1,"source state2 remains boosted for49Z damage");
    in.flagsB9C=0;step();check(state.vehicle64==0&&state.heat7C<1,"release begins cooling");
    state.heat7C=.5f;in.profile34=1;in.profile10FC0=1;step();
    check(state.heat7C==0,"source profile heat bypass");
    in.profile34=2;state.heat7C=.5f;in.cachedSpeed34=1000.f;step();
    check(state.heat7C<.5f&&state.heat7C>0,"source speed-squared cooling");
    const auto saved=out;in.d40Present=true;
    check(!advancePlayerBoost50FK(in,p,state,out,d)&&d.error==BoostError50FK::AudioOwner&&
        std::memcmp(&saved,&out,sizeof out)==0,"present audio owner rejects atomically");
    in.d40Present=false;in.cachedSpeed34=std::numeric_limits<float>::infinity();
    check(!advancePlayerBoost50FK(in,p,state,out,d)&&std::memcmp(&saved,&out,sizeof out)==0,"nonfinite rejects atomically");
    authored.usedAuthoredClassValues=false;
    check(boost50fk_fixture::bind(text.data(),text.size(),selection,authored,p,cf8)&&bits(cf8)!=0x3bbac711u,"distinct interpolated cooling branch");
    selection.applyProfileOverwrite=true;
    check(!boost50fk_fixture::bind(text.data(),text.size(),selection,authored,p,cf8),"unowned profile overwrite rejected");
    std::cout<<"boost_controls50fk PASS "<<checks<<" focused checks; no game launch or guest execution\n";
    return 0;
}catch(const std::exception& e){std::cerr<<"boost_controls50fk FAIL "<<checks<<": "<<e.what()<<'\n';return 1;}}
