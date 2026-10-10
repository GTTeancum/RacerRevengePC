#include "float_unsigned50r.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream in(argv[1]);nlohmann::json data;in>>data;unsigned checks=0;
for(const auto&row:data["cases"]){auto raw=row[0].get<std::uint32_t>();if(rr::game::sourceFloatUnsigned50R(raw)!=row[1].get<std::uint32_t>())return 1;++checks;}
std::cout<<"{\"original_cases\":"<<checks<<"}\n";}
