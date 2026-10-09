#include "type2_bounds50by.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <cfenv>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
float fp(std::uint32_t w){float f;std::memcpy(&f,&w,4);return f;}
int main(int argc,char**argv){if(argc!=3)return 1;std::fesetround(FE_TONEAREST);nlohmann::json j;std::ifstream f(argv[1]);f>>j;std::ifstream cf(argv[2]);nlohmann::json corpus;cf>>corpus;struct Owned{std::vector<Type2Node49X> nodes;std::vector<Type2Ray49X> rays;};Owned actual[2];for(unsigned k=0;k<2;++k){auto& shape=corpus["shapes"][5+k];for(auto& n:shape["nodes"]){Type2Node49X value{};value.volume=n["volume"];value.radius=n["radius"];value.center=n["center"].get<std::array<float,3>>();value.half=n["half"].get<std::array<float,3>>();value.children[0]=n["children"][0];value.children[1]=n["children"][1];actual[k].nodes.push_back(value);}for(auto& values:shape["rays"]){Type2Ray49X ray{};for(unsigned i=0;i<4;++i){ray.first.v[i]=fp(values[i]);ray.second.v[i]=fp(values[i+4]);}actual[k].rays.push_back(ray);}}unsigned cases=0,checks=0,rejected=0;for(auto&row:j["cases"]){Type2Node49X node{};Type2Ray49X ray{};for(unsigned i=0;i<3;++i){node.center[i]=fp(row["center"][i]);node.half[i]=fp(row["half"][i]);}for(unsigned i=0;i<4;++i){ray.first.v[i]=fp(row["ray"][i]);ray.second.v[i]=fp(row["ray"][i+4]);}Type2ResourceView49X view{};if(row["root_present"]){auto& owned=actual[row["source_shape"]=="podboxm.col"?0:1];view.nodes=owned.nodes.data();view.nodeCount=owned.nodes.size();view.rays=owned.rays.data();view.rayCount=owned.rays.size();view.root=0;}else{view.root=type2Leaf49X;view.rays=&ray;view.rayCount=1;}Type2BoundsInput50BY in{};in.resource=&view;for(unsigned i=0;i<16;++i)in.preparedWorld.v[i]=fp(row["matrix"][i]);for(unsigned i=0;i<4;++i)in.cachedCenterWords[i]=row["cache"][i];in.cachedRadiusWord=row["cache"][4];CollectionBoundsResult48S out{};auto n=allocations;
 if(prepareType2Bounds50BY(in,out)!=CollectionBoundsStatus48S::Complete||allocations!=n)return 1;auto expected=row["expected"].get<std::vector<std::uint32_t>>();static_assert(sizeof out==48);if(std::memcmp(&out,expected.data(),48)){std::uint32_t got[12];std::memcpy(got,&out,48);for(unsigned i=0;i<12;++i)if(got[i]!=expected[i])std::cerr<<"case "<<cases<<" word "<<i<<" got "<<got[i]<<" expected "<<expected[i]<<'\n';return 1;}checks+=3;
 auto reject=[&](){auto saved=out;auto before=allocations;auto status=prepareType2Bounds50BY(in,out);if(status==CollectionBoundsStatus48S::Complete||std::memcmp(&out,&saved,sizeof out)||allocations!=before)return false;++rejected;checks+=3;return true;};
 auto root=view.root;view.root=static_cast<std::uint16_t>(view.nodeCount?view.nodeCount:1);if(!reject())return 1;view.root=root;auto matrix=in.preparedWorld.v[0];in.preparedWorld.v[0]=fp(0x7fc01234);if(!reject())return 1;in.preparedWorld.v[0]=matrix;
 auto savedView=view;view.nodes=reinterpret_cast<const Type2Node49X*>(&out);view.nodeCount=1;view.root=0;if(!reject())return 1;view=savedView;in.resource=nullptr;if(!reject())return 1;in.resource=&view;
 std::fesetround(FE_DOWNWARD);bool roundingRejected=reject();std::fesetround(FE_TONEAREST);if(!roundingRejected)return 1;++cases;
 }
 std::cout<<nlohmann::json{{"actual_pod01_query_resources",2},{"original_cases",cases},{"checks",checks},{"negative_controls",rejected},{"hot_allocations",0},{"output_bytes",sizeof(CollectionBoundsResult48S)}}.dump()<<'\n';}
