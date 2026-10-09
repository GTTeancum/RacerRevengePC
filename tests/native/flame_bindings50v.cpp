#include "flame_bindings50v.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <new>
using namespace rr::game;
static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream file(argv[1]);nlohmann::json fixture;file>>fixture;unsigned checks=0,rejections=0,maxWrites=0;
for(const auto&row:fixture["cases"]){FlameModel50V models[4]{};std::vector<FlameMaterial50V>materials;std::vector<FlamePass50V>passes;std::vector<FlameInstance50V>instances;std::vector<std::uint32_t>references=row["references"].get<std::vector<std::uint32_t>>();std::vector<FlameRecord50V>records;
for(unsigned i=0;i<4;++i){const auto&m=row["models"][i];models[i]={m[0],m[1],m[2],m[3]};}for(const auto&m:row["materials"])materials.push_back({m[0],m[1]});for(const auto&p:row["passes"])passes.push_back({p[0],p[1],p[2],p[3].get<unsigned>()!=0});for(const auto&i:row["instances"])instances.push_back({i[0],i[1]});for(const auto&rec:row["records"]){FlameRecord50V out{};for(unsigned j=0;j<8;++j)out.words[j]=rec[j];records.push_back(out);}
FlameViews50V view{materials.data(),materials.size(),passes.data(),passes.size(),instances.data(),instances.size(),references.data(),references.size()};FlameWorkspace50V work{};auto before=records;const auto cold=allocations;const auto result=bindPreparedFlameModels50V(models,row["owner"].get<unsigned>(),view,records.data(),records.size(),work);if(allocations!=cold||result.status!=FlameStatus50V::Complete||result.writes!=row["writes"].size())return 1;++checks;if(result.writes>maxWrites)maxWrites=result.writes;
for(unsigned i=0;i<records.size();++i)for(unsigned j=0;j<8;++j){if(records[i].words[j]!=row["expected"][i][j].get<unsigned>()){std::cerr<<"record "<<i<<" word "<<j<<'\n';return 1;}++checks;}
for(unsigned i=0;i<result.writes;++i){if(work.writes[i].record!=row["writes"][i][0].get<unsigned>()||work.writes[i].callbackTag!=row["writes"][i][1].get<unsigned>()||work.writes[i].ownerId!=row["writes"][i][2].get<unsigned>())return 1;++checks;}
records=before;passes[1].instanceCount=0;auto rejected=bindPreparedFlameModels50V(models,row["owner"],view,records.data(),records.size(),work);if(rejected.status!=FlameStatus50V::NeedsColdInstance||std::memcmp(records.data(),before.data(),records.size()*sizeof(records[0])))return 1;++rejections;passes[1].instanceCount=4;
passes[2].instanceCount=0;rejected=bindPreparedFlameModels50V(models,row["owner"],view,records.data(),records.size(),work);if(rejected.status!=FlameStatus50V::NeedsColdInstance||std::memcmp(records.data(),before.data(),records.size()*sizeof(records[0])))return 1;++rejections;passes[2].instanceCount=4;
if(!passes[0].instanceTablePresent){auto filter=models[0].flameFilter;models[0].flameFilter=0;rejected=bindPreparedFlameModels50V(models,row["owner"],view,records.data(),records.size(),work);if(rejected.status!=FlameStatus50V::UnsupportedDiagnostic||std::memcmp(records.data(),before.data(),records.size()*sizeof(records[0])))return 1;++rejections;models[0].flameFilter=filter;}

references[0]=99;rejected=bindPreparedFlameModels50V(models,row["owner"],view,records.data(),records.size(),work);if(rejected.status!=FlameStatus50V::InvalidBinding||std::memcmp(records.data(),before.data(),records.size()*sizeof(records[0])))return 1;++rejections;references[0]=0;
rejected=bindPreparedFlameModels50V(models,row["owner"],view,work.next,records.size(),work);if(rejected.status!=FlameStatus50V::Storage)return 1;++rejections;
rejected=bindPreparedFlameModels50V(models,row["owner"],view,records.data(),129,work);if(rejected.status!=FlameStatus50V::Limit)return 1;++rejections;
}
std::cout<<"{\"original_cases\":"<<fixture["cases"].size()<<",\"checks\":"<<checks<<",\"rejections\":"<<rejections<<",\"max_writes\":"<<maxWrites<<",\"hot_allocations\":0}\n";}
