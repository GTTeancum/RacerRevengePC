#include "packed_vehicle_tail50r.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cfenv>
#include <cstdlib>
#include <new>
#include <limits>
static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}
using namespace rr::game;
float raw(const nlohmann::json&v){auto b=v.get<std::uint32_t>();float f;std::memcpy(&f,&b,4);return f;}
int main(int argc,char**argv){if(argc!=2)return 2;std::fesetround(FE_TONEAREST);std::ifstream file(argv[1]);nlohmann::json fixture;file>>fixture;unsigned checks=0;
for(const auto&row:fixture["cases"]){PackedVehicleInput50R in{};for(unsigned i=0;i<4;++i){in.drive[i]=raw(row["drive"][i]);in.damage7D0[i]=raw(row["damage"][i]);}for(unsigned i=0;i<3;++i)in.impactBA0[i]=raw(row["impact"][i]);in.filteredB70=raw(row["filtered"][0]);in.filteredB78=raw(row["filtered"][1]);in.actionB80=raw(row["action"]);in.vehicle58=row["vehicle58"];in.configurationEntryNonzero=row["configured"].get<unsigned>()!=0;in.flagsB9C=row["flags"];in.state40=row["state"];
const auto before=in;const auto cold=allocations;auto out=packVehicleTail50R(in);if(allocations!=cold||std::memcmp(&in,&before,sizeof in))return 1;if(!out.complete)return 1;for(unsigned i=0;i<7;++i){if(out.bytesC00[i]!=row["bytes"][i].get<unsigned>()){std::cerr<<"case "<<checks/7<<" byte "<<i<<" got "<<unsigned(out.bytesC00[i])<<" expected "<<row["bytes"][i]<<'\n';return 1;}++checks;}}
unsigned negatives=0;PackedVehicleInput50R input{};
input.actionB80=std::numeric_limits<float>::quiet_NaN();input.configurationEntryNonzero=true;input.filteredB70=0;input.filteredB78=input.actionB80;
if(!packVehicleTail50R(input).complete)return 1;++negatives; // skipped action and second comparison
input.filteredB70=1;if(packVehicleTail50R(input).complete)return 1;++negatives;
input.configurationEntryNonzero=false;if(packVehicleTail50R(input).complete)return 1;++negatives;
input={};input.drive[2]=std::numeric_limits<float>::infinity();if(packVehicleTail50R(input).complete)return 1;++negatives;
input={};input.impactBA0[0]=std::numeric_limits<float>::max();if(packVehicleTail50R(input).complete)return 1;++negatives;
input={};std::fesetround(FE_DOWNWARD);if(packVehicleTail50R(input).complete)return 1;++negatives;std::fesetround(FE_TONEAREST);
std::cout<<"{\"original_cases\":"<<fixture["cases"].size()<<",\"byte_checks\":"<<checks<<",\"admission_controls\":"<<negatives<<",\"hot_allocations\":0}\n";}
