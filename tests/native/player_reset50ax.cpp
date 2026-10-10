#include "player_reset50ax.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned checks=0;for(const auto&row:j["cases"]){PlayerResetState50AX s{row["pre"][1],row["pre"][2],row["pre"][6]};std::uint32_t owner=row["owner_pre"];auto before=allocations;resetPlayerEnd50AX(s,row["cf8"],row["present"].get<bool>()?reinterpret_cast<std::uint8_t*>(&owner):nullptr);if(before!=allocations||s.vehicle64!=row["post"][1]||s.vehicle68!=row["post"][2]||s.vehicle78!=row["post"][6]||owner!=row["owner_post"])return 1;checks+=5;for(unsigned i:{0u,3u,4u,5u,7u}){if(row["pre"][i]!=row["post"][i])return 1;++checks;}}
std::cout<<nlohmann::json{{"cases",j["cases"].size()},{"checks",checks},{"hot_allocations",0},{"state_bytes",sizeof(PlayerResetState50AX)}}.dump()<<'\n';}
