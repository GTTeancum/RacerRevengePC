#include "vehicle_candidates50ca.h"
#include "candidate_collection_fixture50bx.h"
#include <fstream>
#include <iostream>
using namespace collection_test50bx;
int main(int argc,char**argv){
 if(argc!=2)return 2;
 Json data;std::ifstream(argv[1])>>data;
 const auto& j=data.at("cases").at(303);
 Fixture f(j);
 // Prepared smoke world: both ordered methods point at the explicit list.
 for(auto& z:f.zones)z.secondMethod=z.firstMethod;
 Storage second=f.final;
 std::uint32_t temporary[64]{},merged[64]{};
 VehicleCandidateWorkspace50CA w{f.work,f.destination,second.buffers(),{temporary,64},{merged,64}};
 const auto r=collectVehicleCandidates50CA(f.in,f.initial,w);
 if(r.phase!=VehicleCandidateResult50CA::Phase::Complete){std::cerr<<"phase "<<int(r.phase)<<" status "<<int(r.collectionStatus)<<'\n';return 1;}
 const auto wanted=std::min(64,r.first.count+r.second.count);
 if(r.count!=wanted)return 1;
 for(int k=0;k<r.count;++k)if(merged[k]!=(k<r.first.count?f.final.ids[k]:second.ids[k-r.first.count]))return 1;
 auto bad=w;bad.second.objects=f.source.objects.data();
 if(collectVehicleCandidates50CA(f.in,f.initial,bad).phase!=VehicleCandidateResult50CA::Phase::Admission)return 1;
 std::cout<<"50CA smoke passed: first="<<r.first.count<<" second="<<r.second.count<<" merged="<<r.count<<"; source-alias rejected\n";
}
