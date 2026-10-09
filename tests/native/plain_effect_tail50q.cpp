#include "plain_effect_tail50q.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <new>
#include <cstring>
using namespace rr::game;
static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream in(argv[1]);nlohmann::json data;in>>data;unsigned checks=0;
for(const auto&row:data["cases"]){PlainEffectOwners50Q owners{};for(unsigned i=0;i<12;++i){owners.slots[i].id=row["ids"][i];owners.slots[i].flags1D4=row["flags"][i];}
const auto before=owners;const auto cold=allocations;const auto out=inspectPlainEffectTail50Q(owners);if(allocations!=cold||std::memcmp(&owners,&before,sizeof owners))return 1;++checks;
if(out.status!=PlainEffectStatus50Q::Complete||out.count!=row["calls"].size())return 1;++checks;
for(unsigned i=0;i<out.count;++i){if(out.events[i].kind!=ImpactEffectKind50B::Stop84||out.events[i].target!=row["calls"][i]["target"].get<unsigned>()||out.events[i].sourcePC!=row["calls"][i]["pc"].get<unsigned>())return 1;++checks;}}
PlainEffectOwners50Q o{};o.attachment334=1;auto r=inspectPlainEffectTail50Q(o);if(r.status!=PlainEffectStatus50Q::UnsupportedAttached||r.count)return 1;++checks;
o.attachment334=0;o.slots[0]={1,2};o.slots[11]={1,2};r=inspectPlainEffectTail50Q(o);if(r.status!=PlainEffectStatus50Q::AliasedOwners||r.count)return 1;++checks;
std::cout<<"{\"original_cases\":"<<data["cases"].size()<<",\"checks\":"<<checks<<",\"hot_allocations\":0}\n";}
