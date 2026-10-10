#include "vehicle_static_query50h.h"
using namespace rr::game;
Type2StaticStatus49X preparedMainQuery50H(const Type2StaticQuery49X&,
 const Type2StaticCandidate49X*,std::size_t,const Type2StaticInitial49X&,
 const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,Type2StaticResult49X&) noexcept;
#define produceStaticType2Contacts49X preparedMainQuery50H
#define main retained_type2_main
#include "static_type2_producer49x.cpp"
#undef main
#undef produceStaticType2Contacts49X
namespace {unsigned preparedCalls=0,boundaryRejects=0;
std::uint64_t digest50H(const Type2StaticDestination49X&d){std::uint64_t h=1469598103934665603ull;
 auto add=[&](const void*p,std::size_t n){auto*b=static_cast<const unsigned char*>(p);for(std::size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}};
 add(d.pairs,d.pairCapacity*sizeof(*d.pairs));add(d.metadata,d.metadataCapacity*sizeof(*d.metadata));add(d.records,d.recordCapacity*sizeof(*d.records));add(d.effects,d.effectCapacity*sizeof(*d.effects));return h;
}}
Type2StaticStatus49X preparedMainQuery50H(const Type2StaticQuery49X&q,
 const Type2StaticCandidate49X*c,std::size_t n,const Type2StaticInitial49X&i,
 const Type2StaticWorkspace49X&w,const Type2StaticDestination49X&d,Type2StaticResult49X&r) noexcept {
 VehicleFrameState49W frame{};frame.phase=VehicleFramePhase49W::PostWorld;
 frame.nodes[0].world100=q.worldMatrix;frame.nodes[0].sceneFlags190=1;frame.bodies.main.components.root.scene=q.worldMatrix;
 VehicleFrameBindings49W bindings{};bindings.lifecycle.main190=static_cast<SceneNodeIndex49P>(0);
 MainStaticDetectorAdmission50H admission{true,true,true,false,true,false,q.resetRecordsBefore,true,2,0,bindings.lifecycle.main190};
 Type2StaticQuery49X stale;std::memcpy(&stale,&q,sizeof stale);
 stale.worldMatrix.v[12]+=100000;stale.sphere.center.v[0]+=100000;stale.sphere.radius=0;stale.resetRecordsBefore=!q.resetRecordsBefore;
 const auto savedFrame=frame;Type2StaticResult49X savedResult;std::memcpy(&savedResult,&r,sizeof r);const auto savedDigest=digest50H(d);
 for(unsigned test=0;test<10;++test){auto a=admission;auto f=frame;auto destination=d;auto expected=Type2StaticStatus49X::UnsupportedScope;auto query=stale;auto resource=*q.resource;
  if(test==0){f.phase=VehicleFramePhase49W::Ready;expected=Type2StaticStatus49X::InvalidInput;}
  if(test==1)a.registrationE0=1;
  if(test==2)a.pointContactsEnabled=true;
  if(test==3)a.pairsPresent=true;
  if(test==4)f.nodes[0].sceneFlags190=0;
  if(test==5){f.bodies.main.components.root.scene.v[12]+=1;expected=Type2StaticStatus49X::InvalidInput;}
  if(test==6){destination.effects=reinterpret_cast<TerrainCandidateEffects48J*>(&f.nodes[0]);destination.effectCapacity=1;expected=Type2StaticStatus49X::InvalidInput;}
  if(test==7){query.resource=reinterpret_cast<const Type2ResourceView49X*>(reinterpret_cast<const char*>(q.resource)+1);expected=Type2StaticStatus49X::InvalidInput;}
  if(test==8){resource.nodes=reinterpret_cast<const Type2Node49X*>(reinterpret_cast<const char*>(resource.nodes)+1);query.resource=&resource;expected=Type2StaticStatus49X::InvalidInput;}
  if(test==9){resource.nodeCount=std::numeric_limits<std::size_t>::max();query.resource=&resource;expected=Type2StaticStatus49X::InvalidInput;}
  const auto status=queryMainStaticVehicle50H(f,bindings,a,query,c,n,i,w,destination,r);
  if(status!=expected||std::memcmp(&r,&savedResult,sizeof r)||digest50H(d)!=savedDigest)std::abort();++boundaryRejects;
 }
 const auto status=queryMainStaticVehicle50H(frame,bindings,admission,stale,c,n,i,w,d,r);
 if(std::memcmp(&frame,&savedFrame,sizeof frame))std::abort();++preparedCalls;return status;
}
int main(int argc,char**argv){const int result=retained_type2_main(argc,argv);
 std::cout<<"{\"prepared_main_queries\":"<<preparedCalls<<",\"boundary_rejections\":"<<boundaryRejects<<",\"hot_allocations\":"<<allocations<<"}\n";
 return result||!preparedCalls||allocations?1:0;
}
