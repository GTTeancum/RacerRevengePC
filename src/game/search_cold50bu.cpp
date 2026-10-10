#include "search_cold50bu.h"
namespace rr::game{
SearchStorageStatus50BS initializeColdSearchStorage50BU(SearchStorage50BS&s,std::uint32_t capacity)noexcept{
 if(!capacity||capacity>1024)return SearchStorageStatus50BS::InvalidInput;
 s.perBlock=capacity;s.blockCount=0;s.lastUsed=capacity;s.freeHead=0;return SearchStorageStatus50BS::Complete;
}
SearchStorageStatus50BS adoptFirstSearchBlock50BU(SearchStorage50BS&s,std::uint32_t&selected)noexcept{
 auto a=reinterpret_cast<std::uintptr_t>(&s),b=reinterpret_cast<std::uintptr_t>(&selected);
 if((b>=a&&b-a<sizeof s)||!s.perBlock||s.perBlock>1024||s.blockCount||s.freeHead||s.lastUsed!=s.perBlock)return SearchStorageStatus50BS::InvalidInput;
 s.blockCount=1;s.lastUsed=0;selected=1;return SearchStorageStatus50BS::Complete;
}
}
