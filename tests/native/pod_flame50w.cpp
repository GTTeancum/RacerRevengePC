#include "pod_flame50w.h"
#include <fstream>
#include <iterator>
#include <iostream>
#include <cstring>
#include <new>
#include <cstdlib>
#include <type_traits>
#include <utility>
using namespace rr::assets;using namespace rr::game;
static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream file(argv[1],std::ios::binary);std::vector<std::uint8_t>raw((std::istreambuf_iterator<char>(file)),{});ResArchive48C res;DecodeError48C error;if(!decodeResource48C(raw,res,error))return 1;
Scene48D left,right;Material48E shared,flameLeft,flameRight;std::vector<std::uint8_t>member;
if(!extractResMember48C(res,"pods\\pod01\\englt1.psg",member,error)||!decodeScene48D(member,left,error))return 1;
if(!extractResMember48C(res,"pods\\pod01\\engrt1.psg",member,error)||!decodeScene48D(member,right,error))return 1;
if(!extractResMember48C(res,"textures\\01_1.psm",member,error)||!decodeMaterial48E(member,shared,error))return 1;
if(!extractResMember48C(res,"textures\\flame01l.psm",member,error)||!decodeMaterial48E(member,flameLeft,error))return 1;
if(!extractResMember48C(res,"textures\\flame01r.psm",member,error)||!decodeMaterial48E(member,flameRight,error))return 1;
PodFlameOwners50W owners{};PodFlameError50W diag{};unsigned checks=0,rejects=0;
if(!buildPodFlameOwners50W(left,right,shared,flameLeft,flameRight,1,16,owners,diag)){std::cerr<<"cold error "<<unsigned(diag)<<'\n';return 1;}++checks;
for(unsigned u=0;u<4;++u)for(unsigned row=0;row<3;++row)for(unsigned c=0;c<4;++c){if(owners.uniforms[u].rows[row][c]!=(row==c?1.f:0.f))return 1;++checks;}
auto original=owners;FlameWorkspace50V workspace{};const auto view=flameViews50W(owners);const auto cold=allocations;auto bound=bindPreparedFlameModels50V(owners.models,77,view,owners.records,4,workspace);if(allocations!=cold||bound.status!=FlameStatus50V::Complete||bound.writes!=8)return 1;++checks;
if(owners.records[2].words[0]!=0x20f110||owners.records[3].words[0]!=0x20ee90||owners.records[2].words[1]!=77||owners.records[3].words[1]!=77||owners.records[0].words[0]||owners.records[1].words[0])return 1;++checks;
for(unsigned i=0;i<4;++i)for(unsigned k=2;k<8;++k){if(owners.records[i].words[k]!=original.records[i].words[k])return 1;++checks;}
owners.uniforms[2].rows[0][0]=2;if(owners.uniforms[0].rows[0][0]!=1||owners.uniforms[1].rows[0][0]!=1||owners.uniforms[3].rows[0][0]!=1)return 1;++checks;owners=original;
auto reject=[&](const Scene48D&l,const Material48E&fl,std::uint32_t a,std::uint32_t b){auto before=owners;if(buildPodFlameOwners50W(l,right,shared,fl,flameRight,a,b,owners,diag)||std::memcmp(&before,&owners,sizeof owners))return false;++rejects;return true;};
if(!reject(left,flameLeft,0,16)||!reject(left,flameLeft,1,1))return 1;
auto badScene=left;badScene.materials[1]="unknown";if(!reject(badScene,flameLeft,1,16))return 1;
auto badFlame=flameLeft;badFlame.passes[0].microprogram="other";if(!reject(left,badFlame,1,16))return 1;
badFlame=flameLeft;for(auto&f:badFlame.passes[0].fields.authored)if(f.name=="VertexAnimation")f.value="shared";if(!reject(left,badFlame,1,16))return 1;
badFlame=flameLeft;for(auto&f:badFlame.passes[0].fields.authored)if(f.name=="TexGenMatrixRow1")f.value="1,0,0";if(!reject(left,badFlame,1,16))return 1;
// Release all original cold parse ownership; fixed runtime state must survive.
auto release=[](auto&object){using T=typename std::decay<decltype(object)>::type;T empty;using std::swap;swap(object,empty);};
release(raw);release(res);release(left);release(right);release(shared);release(flameLeft);release(flameRight);release(member);
if(raw.capacity()||member.capacity()||res.entries.capacity()||res.blocks.capacity()||res.decoded.capacity()||left.nodes.capacity()||left.materials.capacity()||left.lods.capacity()||right.nodes.capacity()||right.materials.capacity()||right.lods.capacity()||shared.passes.capacity()||flameLeft.passes.capacity()||flameRight.passes.capacity())return 1;
const auto live=flameViews50W(owners);const auto hot=allocations;bound=bindPreparedFlameModels50V(owners.models,77,live,owners.records,4,workspace);if(allocations!=hot||bound.status!=FlameStatus50V::Complete||bound.writes!=8)return 1;++checks;
std::cout<<"{\"checks\":"<<checks<<",\"rejections\":"<<rejects<<",\"hot_allocations\":0,\"cold_inputs_released_before_rebind\":true,\"binding_writes\":8,\"owner_bytes\":"<<sizeof owners<<"}\n";}
