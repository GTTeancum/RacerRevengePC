#include "zone_registration50cj.h"
#include <cstring>
#include <limits>
#include <initializer_list>
namespace rr::game {namespace{
bool overlap(const void*a,std::size_t n,const void*b,std::size_t m)noexcept{
 const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
 return n&&m&&(x<=y?y-x<n:x-y<m);
}
bool ownerValid(const SearchStorage50BS*p)noexcept{return !p||(reinterpret_cast<std::uintptr_t>(p)%alignof(SearchStorage50BS)==0&&sizeof(*p)<=std::numeric_limits<std::uintptr_t>::max()-reinterpret_cast<std::uintptr_t>(p));}
}
SearchStorageStatus50BS movePreparedRegistration50CJ(ZoneRegistration50CJ&record,
 SearchStorage50BS*nextOwner,std::uint32_t object,ZoneRegistrationScratch50CJ&scratch)noexcept{
 using S=SearchStorageStatus50BS;
 if(!object||(object&3u)||!ownerValid(record.owner)||!ownerValid(nextOwner)||overlap(&record,sizeof record,&scratch,sizeof scratch))return S::InvalidInput;
 for(auto*p:{record.owner,nextOwner})if(p&&(overlap(p,sizeof(*p),&record,sizeof record)||overlap(p,sizeof(*p),&scratch,sizeof scratch)))return S::InvalidInput;
 if(record.owner&&nextOwner&&record.owner!=nextOwner&&overlap(record.owner,sizeof(*record.owner),nextOwner,sizeof(*nextOwner)))return S::InvalidInput;
 if(record.owner){
  if(record.encodedObject!=object||!record.slot||record.slot>1024||record.owner->words[record.slot-1]!=object)return S::InvalidTopology;
  std::uint32_t checked=0;auto status=selectSearchSlot50BS(*record.owner,record.slot,checked);if(status!=S::Complete||checked!=record.slot)return S::InvalidTopology;
 }else if(record.slot||record.encodedObject)return S::InvalidTopology;
 if(record.owner==nextOwner)return S::Complete;
 if(record.owner){std::memcpy(&scratch.previous,record.owner,sizeof scratch.previous);const auto status=removeSearchSlot50BS(scratch.previous,record.slot,object);if(status!=S::Complete)return status;}
 std::uint32_t newSlot=0;
 if(nextOwner){
  std::memcpy(&scratch.next,nextOwner,sizeof scratch.next);
  if(!scratch.next.blockCount){auto status=adoptFirstSearchBlock50BU(scratch.next,newSlot);if(status!=S::Complete)return status;}
  auto status=selectSearchSlot50BS(scratch.next,0,newSlot);if(status!=S::Complete)return status;
  status=insertSearchSlot50BS(scratch.next,newSlot,object);if(status!=S::Complete)return status;
 }
 if(record.owner)std::memcpy(record.owner,&scratch.previous,sizeof scratch.previous);
 if(nextOwner)std::memcpy(nextOwner,&scratch.next,sizeof scratch.next);
 record={nextOwner,newSlot,nextOwner?object:0};return S::Complete;
}
}
