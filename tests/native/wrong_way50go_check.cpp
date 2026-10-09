#include "runtime_allocations50fl.h"
#include "wrong_way_progress50go.h"
#include <memory>
#include <fstream>
#include <iterator>
#include <iostream>
#include <cfenv>
#include <cstring>
int main(int argc,char**argv){try{
 if(argc!=2)return 2;std::fesetround(FE_TONEAREST);
 using namespace rr::game;
 std::ifstream f(argv[1],std::ios::binary);std::string text((std::istreambuf_iterator<char>(f)),{});
 if(!f||text.empty())throw std::runtime_error("actual map missing");
 auto scratch=std::make_unique<TrackNavigationScratch50EU>();auto map=std::make_unique<TrackNavigation50EU>();
 SourceLibmState49T libm{};TrackNavigationDiagnostic50EU nd{};
 if(!readTrackNavigation50EU(text.data(),text.size(),libm,*scratch,*map,nd))throw std::runtime_error("actual map decode");
 LapProgressTrack50GF markers{};LapProgressDiagnostic50GF ld{};
 if(!prepareLapProgressTrack50GF(*scratch,markers,ld))throw std::runtime_error("actual markers");
 WrongWayProgressInput50GO input{};input.config58=2;input.vehicleC8C=0;input.vehicleC88=1;
 for(unsigned i=0;i<3;++i)input.localForwardE0[i]=map->track.nodes[1].position[i]-map->track.nodes[0].position[i];
 WrongWayProgressState50GO state{2,0xa7};WrongWayProgressContinuation50GO pending{};
 WrongWayProgressReceipt50GO receipt{};WrongWayProgressDiagnostic50GO diag{};unsigned checks=0;
 auto check=[&](bool value){++checks;if(!value)throw std::runtime_error("wrong-way focused check failed");};
 const auto before=::allocations;
 check(beginWrongWayProgress50GO(map->track,markers,input,state,pending,receipt,diag));
 check(receipt.directionEvaluated&&!receipt.wrongWay);check(state.flags10CC==0xa5&&state.vehicleFF8==2);
 check(pending.pending.operation==WrongWayHudOperation50GO::None);
 for(float& v:input.localForwardE0)v=-v;
 check(beginWrongWayProgress50GO(map->track,markers,input,state,pending,receipt,diag));
 check(receipt.directionEvaluated&&receipt.wrongWay);check(state.flags10CC==0xa7&&state.vehicleFF8==2);
 check(pending.pending.operation==WrongWayHudOperation50GO::None);
 input.vehicle40=1;
 check(beginWrongWayProgress50GO(map->track,markers,input,state,pending,receipt,diag));
 check(!receipt.directionEvaluated&&!receipt.wrongWay&&state.flags10CC==0xa5);
 input.vehicle40=0;input.application34C=1;
 check(beginWrongWayProgress50GO(map->track,markers,input,state,pending,receipt,diag));
 check(receipt.markerCursorFilled&&state.vehicleFF8==markers.progressCount48);
 input.application34C=0;input.config58=3;state.flags10CC=0xa7;state.vehicleFF8=1;
 check(beginWrongWayProgress50GO(map->track,markers,input,state,pending,receipt,diag));
 check(!receipt.warningBranchReached&&!receipt.directionEvaluated&&state.flags10CC==0xa5&&state.vehicleFF8==1);
 check(::allocations==before);
 std::cout<<"PASS wrong-way actual-track focused checks="<<checks<<" hot_cpp_allocations="<<(::allocations-before)<<'\n';return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
