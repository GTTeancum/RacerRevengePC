#include "runtime_allocations50fl.h"
#include "lap_drive50gj_fixture.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <cfenv>
int main(int argc,char**argv){try{
 if(argc!=2)return 2;
 std::fesetround(FE_TONEAREST);
 std::ifstream f(argv[1],std::ios::binary);std::string text((std::istreambuf_iterator<char>(f)),{});
 if(!f||text.empty())throw std::runtime_error("actual map text missing");
 using namespace rr::game;
 auto scratch=std::make_unique<TrackNavigationScratch50EU>();
 auto map=std::make_unique<TrackNavigation50EU>();SourceLibmState49T libm{};TrackNavigationDiagnostic50EU nd{};
 if(!readTrackNavigation50EU(text.data(),text.size(),libm,*scratch,*map,nd))throw std::runtime_error("actual map reader failed");
 lap_drive50GJ::Owner owner;lap_drive50GJ::Continuity continuity;VehicleFrameFields49W fields{};
 owner.continuity=&continuity;owner.fields=&fields;
 if(!prepareLapProgressTrack50GF(*scratch,owner.track,owner.diagnostic))throw std::runtime_error("markers missing");
 initializePlayerLapRaceFields50GF(continuity.state);initializePlayerLapCursor50GF(continuity.state);
 resetUndertrackPlayerPrefix50ET(continuity.prefix);std::string error;
 if(!lap_sound50GH::prepareDisabledFromPod01(owner.sound,owner.provider,error))throw std::runtime_error(error);
 const auto before=::allocations;unsigned checks=0;
 auto require=[&](bool b){++checks;if(!b)throw std::runtime_error("joined lap check failed");};
 require(owner.crossing(true,0,0));require(continuity.state.vehicleCC8==0);
 for(unsigned i=0;i<owner.track.progressCount48;++i){const auto n=owner.track.progressNodes4C[i];require(owner.crossing(true,n,n));}
 require(continuity.state.vehicleFF8==owner.track.progressCount48);
 require(owner.crossing(true,0,0));require(continuity.state.vehicleCC8==1);
 require(continuity.state.vehicleFF8==0);require(owner.completedCallbacks==1);
 require(owner.unavailableSounds==1);require(!owner.pending.pending.requested);
 require(owner.crossing(true,0,0));require(continuity.state.vehicleCC8==1);
 require(::allocations==before);
 std::cout<<"PASS joined actual-marker lap boundary checks="<<checks<<" progress_nodes="<<owner.track.progressCount48
          <<" completed="<<continuity.state.vehicleCC8<<" unavailable_calls="<<owner.unavailableSounds
          <<" hot_cpp_allocations="<<(::allocations-before)<<"\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
