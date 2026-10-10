#include "track_collision_tags50an.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <new>
using namespace rr::assets;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned cases=0;for(const auto&row:j["cases"]){auto name=row["name"].get<std::string>();TrackCollisionMasks50AN m{row["masks"][0],row["masks"][1],row["masks"][2],row["masks"][3]};std::uint32_t out=0xdeadbeef;auto count=allocations;if(!classifyTrackCollision50AN(name.data(),name.size(),m,out)||out!=row["category"]||allocations!=count)return 1;++cases;}unsigned rejected=0;for(unsigned k=0;k<4;++k){char buf[129]{};for(auto&c:buf)c='a';if(k==0)buf[1]='\0';if(k==1)buf[1]=char(0xff);std::uint32_t out=0xdeadbeef;if(classifyTrackCollision50AN(k==3?nullptr:buf,k==2?128:4,{},out)||out!=0xdeadbeef)return 1;++rejected;}
std::cout<<nlohmann::json{{"cases",cases},{"rejected",rejected},{"hot_allocations",0},{"scratch_bytes",128}}.dump()<<'\n';}
