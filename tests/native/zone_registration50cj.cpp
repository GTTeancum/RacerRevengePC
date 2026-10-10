#include "zone_registration50cj.h"
#include <cstdio>
using namespace rr::game;
int main(){SearchStorage50BS a{},b{};ZoneRegistrationScratch50CJ scratch{};ZoneRegistration50CJ r{};
 if(initializeColdSearchStorage50BU(a,1024)!=SearchStorageStatus50BS::Complete||initializeColdSearchStorage50BU(b,1024)!=SearchStorageStatus50BS::Complete)return 1;
 if(movePreparedRegistration50CJ(r,&a,4,scratch)!=SearchStorageStatus50BS::Complete||r.owner!=&a||a.words[0]!=4)return 1;
 if(movePreparedRegistration50CJ(r,&b,4,scratch)!=SearchStorageStatus50BS::Complete||r.owner!=&b||b.words[0]!=4||!(a.words[0]&2))return 1;
 if(movePreparedRegistration50CJ(r,nullptr,4,scratch)!=SearchStorageStatus50BS::Complete||r.owner||!(b.words[0]&2))return 1;
 std::puts("50CJ smoke passed: register, transfer, unregister");}
