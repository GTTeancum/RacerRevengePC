#include "reset_box50aj.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <cfenv>
#include <limits>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;std::fesetround(FE_TONEAREST);nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned cases=0,hits=0;
for(const auto&row:j["cases"]){std::uint32_t u[30];for(unsigned i=0;i<30;++i)u[i]=row["input"][i];SpringVector4_48N p{},q{};BodyMatrix48G m{};Vector3_48C c{},h{};std::memcpy(p.v,u,16);std::memcpy(q.v,u+4,16);std::memcpy(m.v,u+8,64);std::memcpy(c.v,u+24,12);std::memcpy(h.v,u+27,12);auto n=allocations;auto status=resetSegmentBox50AJ(p,q,m,c,h);if(allocations!=n||status==Type3NodeStatus48P::InvalidInput||unsigned(status==Type3NodeStatus48P::Overlap)!=row["overlap"]){std::cerr<<"case "<<cases<<" status "<<int(status)<<'\n';return 1;}hits+=status==Type3NodeStatus48P::Overlap;++cases;}
unsigned rejected=0;for(unsigned k=0;k<5;++k){SpringVector4_48N p{},q{};BodyMatrix48G m{};Vector3_48C c{},h{};float nan=std::numeric_limits<float>::quiet_NaN();if(k==0)p.v[1]=nan;if(k==1)q.v[0]=nan;if(k==2)m.v[12]=nan;if(k==3)c.v[2]=nan;if(k==4)h.v[2]=-1;if(resetSegmentBox50AJ(p,q,m,c,h)!=Type3NodeStatus48P::InvalidInput)return 1;++rejected;}
std::cout<<nlohmann::json{{"cases",cases},{"overlaps",hits},{"rejected",rejected},{"hot_allocations",0}}.dump()<<'\n';}
