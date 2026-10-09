#include "candidate_handles50cb.h"
#include <cstring>
#include <limits>
namespace rr::game {namespace{
struct Span{std::uintptr_t b=0,e=0;};
template<class T>bool span(const T*p,std::size_t n,Span&s)noexcept{
 s={};if(!n)return true;
 if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
 auto b=reinterpret_cast<std::uintptr_t>(p),z=n*sizeof(T);
 if(z>std::numeric_limits<std::uintptr_t>::max()-b)return false;
 s={b,b+z};return true;
}
bool overlap(Span a,Span b)noexcept{return a.b<b.e&&b.b<a.e;}
}
CandidateHandleStatus50CB resolveCandidateHandles50CB(
 const std::uint32_t*ids,std::size_t count,const CandidateHandle50CB*owners,std::size_t ownerCount,
 std::size_t*scratch,std::size_t scratchCapacity,std::size_t*destination,std::size_t destinationCapacity)noexcept{
 using S=CandidateHandleStatus50CB;Span input{},table{},work{},output{};
 if(!span(ids,count,input)||!span(owners,ownerCount,table)||!span(scratch,scratchCapacity,work)||!span(destination,destinationCapacity,output)||
 overlap(work,output)||overlap(work,input)||overlap(work,table)||overlap(output,input)||overlap(output,table))return S::InvalidInput;
 if(scratchCapacity<count||destinationCapacity<count)return S::InsufficientCapacity;
 for(std::size_t i=0;i<ownerCount;++i){if(!owners[i].identity)return S::InvalidInput;
  if(i&&owners[i].identity==owners[i-1].identity)return S::DuplicateIdentity;
  if(i&&owners[i].identity<owners[i-1].identity)return S::InvalidInput;
 }
 for(std::size_t i=0;i<count;++i){std::size_t low=0,high=ownerCount;
  while(low<high){const auto mid=low+(high-low)/2;if(owners[mid].identity<ids[i])low=mid+1;else high=mid;}
  const auto j=low;
  if(j==ownerCount||owners[j].identity!=ids[i])return S::UnknownIdentity;
  scratch[i]=owners[j].index;
 }
 if(count)std::memcpy(destination,scratch,count*sizeof(std::size_t));
 return S::Complete;
}
}
