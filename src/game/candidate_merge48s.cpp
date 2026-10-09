#include "candidate_merge48s.h"
#include <limits>

namespace rr::game {
namespace {
struct Span { std::uintptr_t first=0,last=0; };
bool span(const void* p,std::size_t count,std::size_t size,std::size_t alignment,
          Span& out) noexcept {
    if(!count){out={};return true;}
    const auto start=reinterpret_cast<std::uintptr_t>(p);
    if(!p||start%alignment||count>static_cast<std::size_t>(
        std::numeric_limits<std::ptrdiff_t>::max())/size)return false;
    const auto bytes=count*size;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-start)return false;
    out={start,start+bytes};return true;
}
bool overlaps(Span a,Span b) noexcept {
    return a.first<a.last&&b.first<b.last&&a.first<b.last&&b.first<a.last;
}
std::size_t copied(std::int32_t count,std::size_t available) noexcept {
    return count<=0?0:(static_cast<std::size_t>(count)<available?
        static_cast<std::size_t>(count):available);
}
}
CandidateMergeStatus48S mergeCandidates48S(const CandidateMergeInput48S& input,
    const CandidateMergeBuffer48S& scratch,const CandidateMergeBuffer48S& destination,
    CandidateMergeResult48S& result) noexcept {
    Span reads[5],writes[3];
    // Validate descriptor addresses before reading descriptor members.
    if(!span(&input,1,sizeof(input),alignof(CandidateMergeInput48S),reads[0])||
       !span(&scratch,1,sizeof(scratch),alignof(CandidateMergeBuffer48S),reads[1])||
       !span(&destination,1,sizeof(destination),alignof(CandidateMergeBuffer48S),reads[2])||
       !span(&result,1,sizeof(result),alignof(CandidateMergeResult48S),writes[2]))
        return CandidateMergeStatus48S::InvalidInput;
    if(!span(input.first.identities,input.first.backingCount,sizeof(std::uint32_t),
             alignof(std::uint32_t),reads[3])||
       !span(input.second.identities,input.second.backingCount,sizeof(std::uint32_t),
             alignof(std::uint32_t),reads[4])||
       !span(scratch.identities,scratch.capacity,sizeof(std::uint32_t),
             alignof(std::uint32_t),writes[0])||
       !span(destination.identities,destination.capacity,sizeof(std::uint32_t),
             alignof(std::uint32_t),writes[1]))
        return CandidateMergeStatus48S::InvalidInput;
    for(std::size_t i=0;i<3;++i){
        for(const auto r:reads)if(overlaps(writes[i],r))
            return CandidateMergeStatus48S::InvalidInput;
        for(std::size_t j=i+1;j<3;++j)if(overlaps(writes[i],writes[j]))
            return CandidateMergeStatus48S::InvalidInput;
    }
    const auto a=copied(input.first.count,CandidateMergeCapacity48S);
    const auto b=copied(input.second.count,CandidateMergeCapacity48S-a);
    if(input.first.backingCount<a||input.second.backingCount<b)
        return CandidateMergeStatus48S::InsufficientBacking;
    if(scratch.capacity<a+b||destination.capacity<a+b)
        return CandidateMergeStatus48S::InsufficientCapacity;
    for(std::size_t i=0;i<a;++i)scratch.identities[i]=input.first.identities[i];
    for(std::size_t i=0;i<b;++i)scratch.identities[a+i]=input.second.identities[i];
    for(std::size_t i=0;i<a+b;++i)destination.identities[i]=scratch.identities[i];
    result.count=static_cast<std::int32_t>(a+b);
    return CandidateMergeStatus48S::Complete;
}
}
