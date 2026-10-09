#include "base_pose_history50y.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <cfenv>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;std::fesetround(FE_TONEAREST);static_assert(sizeof(BasePoseHistoryState50Y)==176*4);nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned checks=0,cases=0;for(const auto&row:j["cases"]){BasePoseHistoryState50Y state{};auto before=row["before"].get<std::vector<std::uint32_t>>();auto expected=row["expected"].get<std::vector<std::uint32_t>>();std::memcpy(&state,before.data(),sizeof state);BasePoseHistoryInput50Y in{};auto matrix=row["matrix"].get<std::vector<std::uint32_t>>();std::memcpy(&in.readyWorld100,matrix.data(),64);for(unsigned i=0;i<4;++i){in.velocity70[i]=row["velocity"][i];in.positionAt228B98[i]=row["local_position"][i];}in.outerDeltaBits=row["dt"];in.diagnosticsAbsent=in.worldReady=true;const auto a=allocations;auto out=advanceBasePoseHistory50Y(state,in);if(allocations!=a||out.status!=BasePoseHistoryStatus50Y::Complete)return 1;std::uint32_t words[176];std::memcpy(words,&out.state,sizeof words);for(unsigned i=0;i<176;++i){if(words[i]!=expected[i]){std::cerr<<"case "<<cases<<" word "<<i<<" actual "<<std::hex<<words[i]<<" expected "<<expected[i]<<'\n';return 1;}++checks;}++cases;}
unsigned rejections=0;
for(unsigned bad=0;bad<6;++bad){BasePoseHistoryState50Y state{};BasePoseHistoryInput50Y in{};in.readyWorld100.v[0]=in.readyWorld100.v[5]=in.readyWorld100.v[10]=in.readyWorld100.v[15]=1;in.diagnosticsAbsent=in.worldReady=true;
if(bad==0)in.diagnosticsAbsent=false;if(bad==1)in.worldReady=false;if(bad==2)state.currentFD0=20;if(bad==3)state.nextFD4=20;if(bad==4)state.timerD2C=0x7fc00000u;if(bad==5){state.timerD2C=0x3f800000u;in.outerDeltaBits=0x7fc00000u;}
const auto n=allocations;auto out=advanceBasePoseHistory50Y(state,in);if(out.status==BasePoseHistoryStatus50Y::Complete||allocations!=n||std::memcmp(&state,&out.state,sizeof state))return 1;++rejections;}
std::cout<<nlohmann::json{{"rejections",rejections},{"cases",cases},{"word_checks",checks},{"hot_allocations",0},{"state_bytes",sizeof(BasePoseHistoryState50Y)}}.dump()<<'\n';}
