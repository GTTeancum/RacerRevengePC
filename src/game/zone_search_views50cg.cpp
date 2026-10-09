#include "zone_search_views50cg.h"
#include <limits>
namespace rr::game {namespace{
struct Span{std::uintptr_t a=0,b=0;};
template<class T>bool span(const T*p,std::size_t n,Span&s)noexcept{
 s={};if(!n)return true;
 if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
 auto a=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;s={a,a+bytes};return true;
}
bool overlap(Span a,Span b)noexcept{return a.a<b.b&&b.a<a.b;}
}
SearchStorageStatus50BS prepareZoneSearchViews50CG(const ZoneSearchBinding50CG*bindings,
 std::size_t count,const ZoneSearchViews50CG&out)noexcept{
 using S=SearchStorageStatus50BS;Span writes[3]{},input{},description{};
 if(count>std::numeric_limits<CollectionRef50BX>::max()/2||count>out.zoneCapacity||2*count>out.methodCapacity)return S::InvalidInput;
 if(!span(bindings,count,input)||!span(&out,1,description)||!span(out.methods,out.methodCapacity,writes[0])||!span(out.zones,out.zoneCapacity,writes[1])||!span(out.blocks,out.blockCapacity,writes[2]))return S::InvalidInput;
 for(unsigned i=0;i<3;++i){if(overlap(writes[i],input)||overlap(writes[i],description))return S::InvalidInput;for(unsigned j=0;j<i;++j)if(overlap(writes[i],writes[j]))return S::InvalidInput;}
 std::size_t needed=0;
 for(std::size_t z=0;z<count;++z)for(const auto*registry:{bindings[z].first,bindings[z].second}){
  Span source{};if(!span(registry,1,source))return S::InvalidInput;for(auto w:writes)if(overlap(w,source))return S::InvalidInput;
  std::uint32_t slot=0;const auto status=selectSearchSlot50BS(*registry,0,slot);
  if(status!=S::Complete&&status!=S::GrowthBoundary)return status;
  const auto n=registry->blockCount?static_cast<std::size_t>(registry->blockCount)+1:0;
  if(n>out.blockCapacity-needed)return S::InvalidInput;
  needed+=n;
 }
 std::size_t used=0;
 for(std::size_t z=0;z<count;++z){
  const SearchStorage50BS*registries[]={bindings[z].first,bindings[z].second};
  for(unsigned side=0;side<2;++side){const auto&registry=*registries[side];const auto n=registry.blockCount?static_cast<std::size_t>(registry.blockCount)+1:0;
   CollectionTree50BX method{};method.method=CollectionTree50BX::Method::SearchList;
   const auto status=prepareSearchView50BT(registry,n?out.blocks+used:nullptr,n,method.searchList);if(status!=S::Complete)return status;
   out.methods[z*2+side]=method;used+=n;
  }
  out.zones[z]={static_cast<CollectionRef50BX>(z*2+1),static_cast<CollectionRef50BX>(z*2+2)};
 }
 return S::Complete;
}
}
