#include "optional_probe_effects50p.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <limits>
using namespace rr::game;
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream in(argv[1]);nlohmann::json data;in>>data;unsigned checks=0;
for(const auto&row:data["cases"]){OptionalProbeOwners50P owner{};owner.optional7FC=row["owner"];owner.manager51C=row["manager"];owner.effect808[0]=row["ids"][0];owner.effect808[1]=row["ids"][1];auto word=row["velocity"].get<std::uint32_t>();float z;std::memcpy(&z,&word,4);
const auto out=inspectOptionalProbeEffects50P(owner,row["probe104"],row["probe118"],z);if(static_cast<unsigned>(out.status)!=row["status"].get<unsigned>()||out.count!=row["calls"].size())return 1;++checks;
for(unsigned i=0;i<out.count;++i){const auto&expected=row["calls"][i];if(static_cast<unsigned>(out.events[i].kind)!=expected["kind"]||out.events[i].sourcePC!=expected["pc"]||out.events[i].target!=expected["target"])return 1;++checks;}}
OptionalProbeOwners50P owner{1,1,{0,11}};float z=0;if(inspectOptionalProbeEffects50P(owner,0,0,z).status!=OptionalProbeStatus50P::Binding)return 1;++checks;
owner.effect808[0]=10;z=std::numeric_limits<float>::quiet_NaN();if(inspectOptionalProbeEffects50P(owner,1,0,z).status!=OptionalProbeStatus50P::Arithmetic)return 1;++checks;
std::cout<<"{\"original_cases\":9,\"checks\":"<<checks<<"}\n";}
