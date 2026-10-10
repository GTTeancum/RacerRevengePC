#include "search_storage50bs.h"
namespace rr::game{namespace{
using S=SearchStorageStatus50BS;
S validate(const SearchStorage50BS&s,std::uint32_t&used)noexcept{
 if(!s.perBlock||s.perBlock>1024||s.blockCount>1024/s.perBlock||s.lastUsed>s.perBlock||(!s.blockCount&&s.lastUsed!=s.perBlock))return S::InvalidInput;
 used=s.blockCount?(s.blockCount-1)*s.perBlock+s.lastUsed:0;
 std::uint32_t seen[32]{};auto ref=s.freeHead;
 while(ref){if(ref>used||(seen[(ref-1)/32]&(1u<<((ref-1)%32))))return S::InvalidTopology;auto w=s.words[ref-1];if((w&3)!=2)return S::InvalidTopology;seen[(ref-1)/32]|=1u<<((ref-1)%32);ref=w>>2;}
 for(unsigned i=0;i<used;++i)if(!(seen[i/32]&(1u<<(i%32)))&&(!s.words[i]||(s.words[i]&3)))return S::InvalidTopology;
 return S::Complete;
}
bool outputAliases(const SearchStorage50BS&s,const std::uint32_t&v)noexcept{auto a=reinterpret_cast<std::uintptr_t>(&s),b=reinterpret_cast<std::uintptr_t>(&v);return b>=a&&b-a<sizeof s;}
}
SearchStorageStatus50BS selectSearchSlot50BS(const SearchStorage50BS&s,std::uint32_t previous,std::uint32_t&selected)noexcept{
 if(outputAliases(s,selected))return S::InvalidInput;std::uint32_t used;auto status=validate(s,used);if(status!=S::Complete)return status;
 if(previous){if(previous>used)return S::InvalidInput;selected=previous;return S::Complete;}
 if(s.freeHead){selected=s.freeHead;return S::Complete;}
 if(s.lastUsed==s.perBlock)return S::GrowthBoundary;selected=used+1;return S::Complete;
}
SearchStorageStatus50BS insertSearchSlot50BS(SearchStorage50BS&s,std::uint32_t slot,std::uint32_t identity)noexcept{
 if(!identity||(identity&3))return S::InvalidInput;std::uint32_t used;auto status=validate(s,used);if(status!=S::Complete)return status;
 if(!slot||slot>1024)return S::InvalidInput;
 if(s.freeHead&&slot==s.freeHead)s.freeHead=s.words[slot-1]>>2;
 else{if(s.freeHead||s.lastUsed==s.perBlock||slot!=used+1)return S::InvalidInput;++s.lastUsed;}
 s.words[slot-1]=identity;return S::Complete;
}
SearchStorageStatus50BS removeSearchSlot50BS(SearchStorage50BS&s,std::uint32_t slot,std::uint32_t identity)noexcept{
 if(!identity||(identity&3))return S::InvalidInput;std::uint32_t used;auto status=validate(s,used);if(status!=S::Complete)return status;
 if(!slot||slot>used||s.words[slot-1]!=identity)return S::InvalidInput;
 s.words[slot-1]=(s.freeHead<<2)|2;s.freeHead=slot;return S::Complete;
}
}
