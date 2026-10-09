#include "zone_search_views50cg.h"
#include "search_cold50bu.h"
#include <cstdio>
using namespace rr::game;
int main(){
 SearchStorage50BS populated{},empty{};std::uint32_t slot=0;
 if(initializeColdSearchStorage50BU(populated,1024)!=SearchStorageStatus50BS::Complete||initializeColdSearchStorage50BU(empty,1024)!=SearchStorageStatus50BS::Complete||adoptFirstSearchBlock50BU(populated,slot)!=SearchStorageStatus50BS::Complete||insertSearchSlot50BS(populated,slot,4)!=SearchStorageStatus50BS::Complete)return 1;
 ZoneSearchBinding50CG bindings[]={{&populated,&empty},{&empty,&populated}};
 CollectionTree50BX methods[4]{};CollectionZone50BX zones[2]{};SearchListBlock50BR blocks[4]{};
 if(prepareZoneSearchViews50CG(bindings,2,{methods,4,zones,2,blocks,4})!=SearchStorageStatus50BS::Complete)return 1;
 if(zones[0].firstMethod!=1||zones[1].secondMethod!=4||methods[0].searchList.blocks[0].words[0]!=4||methods[1].searchList.blockCount)return 1;
 std::puts("50CG smoke passed: populated/empty methods preserve explicit zone order");
}
