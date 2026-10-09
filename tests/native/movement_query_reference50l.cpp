#include "movement_static_query50l.h"
static std::uint32_t movementFlag50L=0;
static rr::game::Type3QueryStatus48P movementCall50L(const rr::game::Type3QueryInput48P&q,const rr::game::Type3QuerySource48P&s,const rr::game::Type3Candidate48P*c,std::size_t n,const rr::game::Type3QueryWorkspace48P&w,const rr::game::Type3QueryBuffers48P&d,rr::game::Type3QueryResult48P&r){
 const auto outcome=rr::game::queryMovementStaticCandidates50L({true,true},q,s,c,n,w,d,r,movementFlag50L);movementFlag50L=outcome.vehicle7A0;return outcome.status;
}
#define queryStaticType3Bounded48P movementCall50L
// Reuse the retained per-field comparison without running its historical full
// corpus or benchmark. Expectations come from the distinct original121920 path.
#define main retained_type3_test_main50l
#include "type3_query48p.cpp"
#undef main
#undef queryStaticType3Bounded48P
#include "movement_callback50l.h"
int main(int argc,char**argv){try{
 if(argc!=2)return 2;std::fesetround(FE_TONEAREST);std::ifstream in(argv[1]);Json fixture;in>>fixture;
 std::vector<CollisionMesh48I> meshes;for(const auto&m:fixture.at("meshes"))meshes.push_back(mesh(m));std::size_t capacity=0;for(const auto&m:meshes)capacity=std::max(capacity,stackBound(m));
 for(const auto&j:fixture.at("cases")){movementFlag50L=0x12345678;differential(j,meshes,capacity);
  check(movementFlag50L==j.at("expected").at("vehicle7a0").get<std::uint32_t>(),"original20EE10 callback and query composition");
 }
 unsigned rejected=0;
 for(unsigned bad=0;bad<12;++bad){Safety s;s.q.filter.flagsF8|=0x02;s.q.callbackEnabled=false;MovementQueryAdmission50L admission{true,true};
  if(bad==0)admission.callbackIs20EE10=false;if(bad==1)admission.completeCollectedStaticOrder=false;
  if(bad==2)s.q.filter.flagsF8&=~0x02u;if(bad==3)s.q.pairCount=2;if(bad==4)s.q.normalMode=1;
  if(bad==5)s.q.callbackEnabled=true;if(bad==6)s.q.resetSpring=true;if(bad==7)s.q.entry=Type3QueryEntry48P::Dispatcher;
  if(bad==8)s.candidates[0].collision.filter.categoryA4=s.q.dynamicMask;
  if(bad==9)s.candidatePointer=nullptr;if(bad==10)s.count=std::numeric_limits<std::size_t>::max();if(bad==11)s.metadata[0]=123;
  const auto saved=s.saved();watchAllocations=true;const auto outcome=queryMovementStaticCandidates50L(admission,s.q,s.source,s.candidatePointer,s.count,s.workspace,s.destination,s.out,0x89abcdef);watchAllocations=false;
  check(outcome.status!=Type3QueryStatus48P::Complete,"unsupported/invalid movement domain rejected");check(outcome.vehicle7A0==0x89abcdef,"callback owner preserved on rejection");for(const auto&snapshot:saved)snapshot.verify();++rejected;
 }
 check(observedAllocations==0,"no allocations in native query/callback");std::cout<<"{\"cases\":"<<differentialCases<<",\"rejections\":"<<rejected<<",\"checks\":"<<checks<<",\"failures\":"<<failures<<",\"hot_allocations\":"<<observedAllocations<<"}\n";return failures?1:0;
}catch(const std::exception&e){watchAllocations=false;std::cerr<<e.what()<<'\n';return 2;}}
