#include "engine_flare50gv.h"
#include "pod_flame50w.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <new>

static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
using namespace rr::assets;using namespace rr::game;
static std::uint32_t bits(float f){std::uint32_t n;std::memcpy(&n,&f,4);return n;}
static float fp(std::uint32_t n){float f;std::memcpy(&f,&n,4);return f;}
static void require(bool yes,const char*label){if(!yes){std::cerr<<label<<'\n';std::exit(1);}}
int main(int argc,char**argv){
 require(argc==2,"expected Pod01.RES");std::ifstream file(argv[1],std::ios::binary);
 std::vector<std::uint8_t>raw((std::istreambuf_iterator<char>(file)),{}),member;
 ResArchive48C res;DecodeError48C error;require(decodeResource48C(raw,res,error),"resource");
 Scene48D scenes[2];Material48E shared,flames[2];
 auto scene=[&](const char*name,Scene48D&out){require(extractResMember48C(res,name,member,error)&&decodeScene48D(member,out,error),name);};
 auto material=[&](const char*name,Material48E&out){require(extractResMember48C(res,name,member,error)&&decodeMaterial48E(member,out,error),name);};
 scene("pods\\pod01\\englt1.psg",scenes[0]);scene("pods\\pod01\\engrt1.psg",scenes[1]);
 material("textures\\01_1.psm",shared);material("textures\\flame01l.psm",flames[0]);material("textures\\flame01r.psm",flames[1]);
 PodFlameOwners50W owners{};PodFlameError50W coldDiagnostic{};
 require(buildPodFlameOwners50W(scenes[0],scenes[1],shared,flames[0],flames[1],1,16,owners,coldDiagnostic),"actual authoring");
 FlameWorkspace50V binding{};require(bindPreparedFlameModels50V(owners.models,77,flameViews50W(owners),owners.records,4,binding).status==FlameStatus50V::Complete,"actual callback binding");
 std::size_t flameVertices=0,flameTriangles=0;float uvMinimum[2]{1000.f,1000.f},uvMaximum[2]{-1000.f,-1000.f};
 for(unsigned side=0;side<2;++side){
  auto value=[&](const char*name){for(const auto&f:flames[side].passes[0].fields.authored)if(f.name==name)return f.value;return std::string{};};
  require(value("inputColorA")=="0"&&value("inputColorB")=="2"&&value("inputAlphaC")=="2"&&value("inputAlphaD")=="1"&&value("fixedAlphaValue")=="255","authored additive equation");
  require(value("alphaBlendingOnOff")=="1"&&value("zWriteDisabled")=="1"&&value("wrapModeVertical")=="1","authored blend/depth/clamp");
  unsigned surfaces=0;
  for(const auto&s:scenes[side].lods[0].surfaces)if(s.material_index&1u){
   require(s.material_index==1,"retained LOD0 flame slot");++surfaces;flameVertices+=s.vertices.size();flameTriangles+=s.indices.size()/3;
   for(const auto&v:s.vertices)for(unsigned axis=0;axis<2;++axis){uvMinimum[axis]=std::fmin(uvMinimum[axis],static_cast<float>(v.uv[axis]));uvMaximum[axis]=std::fmax(uvMaximum[axis],static_cast<float>(v.uv[axis]));}
  }
  require(surfaces==1,"one retained flame surface each engine");
 }
 VehicleFrameState49W frame{};frame.phase=VehicleFramePhase49W::Ready;frame.noise.sharedRandomState=1;
 BaseTailState50X tail{};tail.flagsACC=tail.flagsAD0=1;
 EngineFlareOutput50GV out{};EngineFlareDiagnostic50GV diag{};
 frame.bodies.main.components.engine.localThrust[0].v[2]=10000.f;
 const auto originalFrame=frame;const auto originalTail=tail;const auto originalOwners=owners;
 const auto hot=allocations;
 require(advanceEngineFlare50GV(frame,tail,owners,0,out,diag),"first render");
 require(allocations==hot,"hot allocation");
 require(!std::memcmp(&frame,&originalFrame,sizeof frame)&&!std::memcmp(&tail,&originalTail,sizeof tail)&&!std::memcmp(&owners,&originalOwners,sizeof owners),"projection mutates canonical input");
 require(out.callbacks==2&&out.randomDraws==2&&fp(out.flameAC4[0])==1500.f&&out.flameAC4[1]==0,"source rising response");
 require(out.uv[0][0]==1.f&&out.uv[0][1]==0.f&&out.uv[0][2]==fp(0x3de353f8u)&&out.uv[0][3]==0.f&&out.uv[0][4]>.54f&&out.uv[0][4]<.65f&&out.uv[0][5]==0.f,"source affine output");
 std::uint32_t random=1;random=random*0x41c64e6du+0x3039u;random=random*0x41c64e6du+0x3039u;
 require(out.sharedRandomState==random,"source RNG2 draws");
 commitEngineFlare50GV(out,frame,tail,owners);
 require(owners.records[0].words[2]==0xffffffffu&&owners.records[1].words[2]==0xffffffffu&&owners.uniforms[0].rows[1][1]==1.f&&owners.uniforms[1].rows[2][0]==0.f,"immutable templates");
 const auto first=out;require(advanceEngineFlare50GV(frame,tail,owners,0,out,diag),"repeat stamp");
 require(out.callbacks==0&&out.randomDraws==0&&out.sharedRandomState==random&&!std::memcmp(out.uv,first.uv,sizeof out.uv),"repeat advances callback");
 require(advanceEngineFlare50GV(frame,tail,owners,1,out,diag)&&fp(out.flameAC4[0])==2775.f,"second rising response");
 commitEngineFlare50GV(out,frame,tail,owners);frame.bodies.main.components.engine.localThrust[0].v[2]=0.f;
 require(advanceEngineFlare50GV(frame,tail,owners,2,out,diag)&&fp(out.flameAC4[0])==2636.25f,"source falling response .05");
 frame.bodies.main.components.aero.lowSpeedDc=true;frame.fields.applicationByte58=2;frame.bodies.main.controls.player.raw.values[0]=.5f;
 require(advanceEngineFlare50GV(frame,tail,owners,3,out,diag)&&out.thrustMagnitude[0]==3000.f&&out.thrustMagnitude[1]==0.f,"left low speed steering flame");
 frame.bodies.main.controls.player.raw.values[0]=-.5f;
 require(advanceEngineFlare50GV(frame,tail,owners,3,out,diag)&&out.thrustMagnitude[0]==0.f&&out.thrustMagnitude[1]==3000.f,"right low speed steering flame");
 frame.bodies.main.components.aero.lowSpeedDc=false;tail.flagsAD0=tail.flagsACC=0;
 frame.bodies.main.components.engine.localThrust[0].v[0]=3000.f;frame.bodies.main.components.engine.localThrust[0].v[1]=4000.f;
 require(advanceEngineFlare50GV(frame,tail,owners,4,out,diag)&&out.thrustMagnitude[0]==5000.f&&out.flameAC4[0]==tail.flameAC4[0],"inactive bypass keeps AC4 and source norm");
 for(unsigned stamp=5;stamp<105;++stamp){require(advanceEngineFlare50GV(frame,tail,owners,stamp,out,diag),"scroll progression");commitEngineFlare50GV(out,frame,tail,owners);require(out.uv[0][2]>=0.f&&out.uv[0][2]<=1.f,"scroll wrap");}
 const auto before=out;frame.bodies.main.components.engine.localThrust[1].v[0]=std::numeric_limits<float>::infinity();
 require(!advanceEngineFlare50GV(frame,tail,owners,105,out,diag)&&!std::memcmp(&before,&out,sizeof out),"transactional nonfinite rejection");
 frame.bodies.main.components.engine.localThrust[1].v[0]=0.f;frame.bodies.main.applicationActive11C=1;
 require(!advanceEngineFlare50GV(frame,tail,owners,105,out,diag)&&diag.error==EngineFlareError50GV::unsupportedState,"unsupported pause rejection");
 frame.bodies.main.applicationActive11C=0;owners.records[3].words[0]=0;
 require(!advanceEngineFlare50GV(frame,tail,owners,105,out,diag)&&diag.error==EngineFlareError50GV::binding,"wrong callback rejection");
 require(allocations==hot,"hot progression allocation");
 std::cout<<"{\"source_geometry\":true,\"lod0_flame_material_index\":1,\"flame_vertices\":"<<flameVertices<<",\"flame_triangles\":"<<flameTriangles<<",\"original_uv_min\":["<<uvMinimum[0]<<','<<uvMinimum[1]<<"],\"original_uv_max\":["<<uvMaximum[0]<<','<<uvMaximum[1]<<"],\"hot_allocations\":0,\"callback_stamps\":true,\"source_rng_draws_per_render\":2,\"rising_falling_steering_controls\":true,\"guest_execution\":false}\n";
}
