#include "grid_center50ac.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <cfenv>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=3)return 2;std::fesetround(FE_TONEAREST);static_assert(sizeof(GridCenterResult50AC)==36);nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned cases=0,checks=0;
for(const auto&row:j["cases"]){GridCenterInput50AC in{};for(unsigned i=0;i<4;++i){in.current[i]=row["current"][i];in.previous[i]=row["previous"][i];}in.rank=row["rank"];in.nextNode=1;GridCenterResult50AC out{};auto count=allocations;if(!prepareGridCenter50AC(in,out)||allocations!=count)return 1;std::uint32_t words[9];std::memcpy(words,&out,36);for(unsigned i=0;i<9;++i){if(words[i]!=row["expected"][i]){std::cerr<<cases<<" word "<<i<<" actual "<<std::hex<<words[i]<<" expected "<<row["expected"][i]<<'\n';return 1;}++checks;}++cases;}
nlohmann::json poses;std::ifstream pf(argv[2]);pf>>poses;unsigned poseCases=0,poseChecks=0;for(const auto&row:poses["cases"]){const auto&g=row["grid"];GridCenterInput50AC in{};for(unsigned i=0;i<4;++i){in.current[i]=g["current"][i];in.previous[i]=g["previous"][i];}in.rank=g["rank"];in.nextNode=1;auto raw=row["lift"].get<std::uint32_t>();float lift;std::memcpy(&lift,&raw,4);BodyMatrix48G out{};const auto n=allocations;if(!prepareCenterPose50AC(in,lift,{{0,1,0}},out)||allocations!=n)return 1;std::uint32_t words[16];std::memcpy(words,out.v,64);for(unsigned i=0;i<16;++i){if(words[i]!=row["matrix"][i]){std::cerr<<"pose "<<poseCases<<" word "<<i<<" actual "<<std::hex<<words[i]<<" expected "<<row["matrix"][i]<<'\n';return 1;}++poseChecks;}++poseCases;}
unsigned rejected=0;for(unsigned bad=0;bad<5;++bad){GridCenterInput50AC in{};in.rank=1;in.current[2]=0x3f800000u;if(bad<3)in.rank=bad==0?0:bad==1?2:7;if(bad==3)in.current[2]=0;if(bad==4)in.current[0]=0x7fc00000u;GridCenterResult50AC out{};std::memset(&out,0xa5,sizeof out);const auto saved=out;auto count=allocations;if(prepareGridCenter50AC(in,out)||allocations!=count||std::memcmp(&out,&saved,sizeof out))return 1;++rejected;}
std::cout<<nlohmann::json{{"pose_cases",poseCases},{"pose_word_checks",poseChecks},{"cases",cases},{"word_checks",checks},{"rejections",rejected},{"hot_allocations",0},{"result_bytes",sizeof(GridCenterResult50AC)}}.dump()<<'\n';}
