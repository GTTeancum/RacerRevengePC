// UNVERIFIED reconstruction. Rebuild/run; prior lost logs do not certify this file.
#include "native_racer_contact_bank50ht_fixture.h"
#include "../../src/assets/spring_authoring50f.h"
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <new>
namespace {bool watch=false;std::size_t allocations=0,checks=0;
void need(bool ok,const char* why){++checks;if(!ok){std::cerr<<why<<'\n';std::exit(1);}}
std::vector<std::uint8_t> read(const char* path){std::ifstream f(path,std::ios::binary);need(bool(f),path);return {std::istreambuf_iterator<char>(f),{}};}}
void* operator new(std::size_t n){if(watch)++allocations;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void* p)noexcept{std::free(p);}void operator delete(void* p,std::size_t)noexcept{std::free(p);}
using namespace rr::game;namespace c=native_racer_contact50HT;namespace a=rr::assets;
BodyMatrix48G pose(float x,float z){BodyMatrix48G m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=x;m.v[14]=z;return m;}
int main(){
 need(std::fesetround(FE_TONEAREST)==0,"rounding");
 const auto raw=read("work/native/project/disc/PODS/POD01/POD01.RES");
 a::ResArchive48C archive;a::DecodeError48C de;std::vector<std::uint8_t> col;
 need(a::decodeResource48C(raw,archive,de)&&a::extractResMember48C(archive,"Pods\\Pod01\\podboxm.col",col,de),"actual podboxm");
 auto u32=[](const std::uint8_t* p){std::uint32_t n;std::memcpy(&n,p,4);return n;};
 const auto nr=u32(col.data()+80),nn=u32(col.data()+84);
 std::vector<Type2Node49X> nodes(nn);std::vector<Type2Ray49X> rays(nr);
 std::vector<Type2OpaqueEndpointWords49X> opaque(nr,{0x3f800000,0x3f800000});
 std::vector<std::uint32_t> topology(nr+nn);Type2ResourceView49X shape{};
 need(decodeType2Resource49X(col.data(),col.size(),opaque.data(),opaque.size(),nodes.data(),nn,rays.data(),nr,{topology.data(),topology.size()},shape)==Type2ResourceStatus49X::Complete,"actual query decode");
 c::Owner owner;std::string error;need(owner.prepare(shape,error),error.c_str());
 need(owner.targetMesh().vertices.size()==16&&owner.targetMesh().faces.size()==28,"actual target topology");
 std::cout<<"geometry rays="<<nr<<" nodes="<<nn<<" target root half="<<owner.targetMesh().nodes[0].half[0]<<','<<owner.targetMesh().nodes[0].half[1]<<','<<owner.targetMesh().nodes[0].half[2]<<'\n';
 c::Body input[4]{},output[4]{};
 for(unsigned i=0;i<4;++i){auto& b=input[i].physical;b.identity=100+i;b.body.state.scene=pose(1000.f*i,0);b.body.state.body.orientation.v[0]=1;
  b.body.parameters.mass=100;b.body.parameters.inverseMass=.01f;
  for(unsigned k=0;k<3;++k)b.body.parameters.inverseInertiaColumns[k].v[k]=.001f;}
 const auto phy=read("work/impactAuthoring49Y/Pod01.phy");
 a::SpringCollisionAuthoring50F authored{};a::PodPhysicsDiagnostic49N diagnostic{};
 SpringGeneratedParameters48K settings{};settings.impact54=true;settings.friction50=true;settings.impact.disableLeverArms5C=true;
 settings.impact.correctPenetration64=true;settings.impact.normalForceScale98=.97f;settings.impact.positionCorrectionScaleCC=.2f;
 SpringResponseParameters48L response{};response.maxSpeed94=10000;
 need(a::readSpringCollision50F(reinterpret_cast<const char*>(phy.data()),phy.size(),authored,diagnostic)&&a::applySpringCollision50F(authored,100,settings,response),"actual authored restitution");
 const c::Pair order[]={{0,1},{1,0},{0,2},{2,0},{0,3},{3,0},{1,2},{2,1},{1,3},{3,1},{2,3},{3,2}};
 c::Receipt receipt{};
 watch=true;auto status=owner.run(input,4,order,12,settings,response,output,receipt);watch=false;
 need(status==c::Status::Complete&&receipt.geometryPairs==0&&!std::memcmp(input,output,sizeof input),"separated racers unchanged");
 bool found=false;float hitX=0,hitZ=0;std::size_t foundRecords=0;
 // Search synthetic placements of AUTHORED meshes, not generated fake shapes.
 for(float x=2;x<100&&!found;x+=2)for(float z=-80;z<=80&&!found;z+=4){
  input[1].physical.body.state.scene=pose(x,z);input[0].physical.body.state.body.velocity={{100,0,0}};
  input[1].physical.body.state.body.velocity={{0,0,0}};
  watch=true;status=owner.run(input,4,order,12,settings,response,output,receipt);watch=false;
  need(status==c::Status::Complete,"actual mesh pair query");
  if(receipt.activeRecords&&receipt.suppressedPairs==1&&output[1].physical.body.state.body.velocity.v[0]>0){found=true;hitX=x;hitZ=z;foundRecords=receipt.generatedRecords;}
 }
 need(found,"authored pod shapes produce moving two-body contacts");
 need(receipt.respondedPairs==1&&receipt.suppressedPairs==1,"reciprocal source gate applies response once");
 need(receipt.contacts[0].count20C==1&&receipt.contacts[1].count20C==1,"both original contact callbacks");
 need(output[0].physical.body.state.body.velocity.v[0]<100&&output[1].physical.body.state.body.velocity.v[0]>0,"both bodies receive opposite impulse");
 const float sum=output[0].physical.body.state.body.velocity.v[0]+output[1].physical.body.state.body.velocity.v[0];
 need(std::fabs(sum-100)<.001f,"equal mass linear momentum preserved");
 for(unsigned i=0;i<4;++i)need(!std::memcmp(&input[i].physical.body.state.scene,&output[i].physical.body.state.scene,sizeof(BodyMatrix48G)),"no teleport or scene mutation");
 need(!std::memcmp(&input[2],&output[2],2*sizeof(c::Body)),"unrelated bodies unchanged");
 c::Body onePairOutput[4]{};c::Receipt onePairReceipt{};const c::Pair onePair[]={{0,1}};
 watch=true;status=owner.run(input,4,onePair,1,settings,response,onePairOutput,onePairReceipt);watch=false;
 need(status==c::Status::Complete&&onePairReceipt.respondedPairs==1&&
      !std::memcmp(onePairOutput,output,sizeof output),"one pair equals reciprocal-suppressed full order");
 auto noImpact=settings;noImpact.impact54=false;noImpact.friction50=false;c::Body noImpactOutput[4]{};c::Receipt noImpactReceipt{};
 watch=true;status=owner.run(input,4,onePair,1,noImpact,response,noImpactOutput,noImpactReceipt);watch=false;
 c::Body noImpactExpected[4]{};std::memcpy(noImpactExpected,input,sizeof input);
 auto& processedExpected=noImpactExpected[1].physical.body.state;
 processedExpected.callbackState238=1;
 processedExpected.processedContactPartners23C[0]={input[0].physical.identity};
 need(status==c::Status::Complete&&noImpactReceipt.activeRecords>0&&
      noImpactReceipt.contacts[0].count20C==1&&noImpactReceipt.contacts[1].count20C==1&&
      !std::memcmp(noImpactExpected,noImpactOutput,sizeof noImpactExpected),
      "zero impulse preserves dynamics but commits authentic asymmetric1654C0 callback");
 // Persisted processed identities survive another run until actual integration
 // clears count238. Reciprocal geometry remains real, with no response replay.
 const c::Pair reverseOnly[]={{1,0}};c::Body pendingOutput[4]{};c::Receipt pendingReceipt{};
 watch=true;status=owner.run(noImpactOutput,4,reverseOnly,1,noImpact,response,pendingOutput,pendingReceipt);watch=false;
 need(status==c::Status::Complete&&pendingReceipt.geometryPairs==1&&pendingReceipt.suppressedPairs==1&&
      pendingReceipt.respondedPairs==0&&!std::memcmp(noImpactOutput,pendingOutput,sizeof pendingOutput),
      "next contact transaction consumes canonical pending238/23C");
 watch=true;status=owner.run(noImpactOutput,4,nullptr,0,noImpact,response,pendingOutput,pendingReceipt);watch=false;
 need(status==c::Status::Complete&&pendingReceipt.testedPairs==0&&
      !std::memcmp(noImpactOutput,pendingOutput,sizeof pendingOutput),"zero-pair pass preserves pending processed owner");
 c::Body four[4]{},fourOutput[4]{};std::memcpy(four,input,sizeof four);
 four[2].physical.body.state.scene=pose(-6,-12);four[3].physical.body.state.scene=pose(6,-12);
 four[2].physical.body.state.body.velocity={{30,0,10}};four[3].physical.body.state.body.velocity={{-20,0,5}};
 c::Receipt fourReceipt{};watch=true;status=owner.run(four,4,order,12,settings,response,fourOutput,fourReceipt);watch=false;
 need(status==c::Status::Complete&&fourReceipt.respondedPairs>=3,"four actual simultaneous racer contacts");
 for(unsigned axis=0;axis<3;++axis){float before=0,after=0;
  for(unsigned i=0;i<4;++i){before+=four[i].physical.body.state.body.velocity.v[axis];after+=fourOutput[i].physical.body.state.body.velocity.v[axis];}
  need(std::isfinite(after)&&std::fabs(after-before)<.002f,"four racer source impulses preserve total momentum");}
 std::cout<<"four racer responses="<<fourReceipt.respondedPairs<<" suppressed="<<fourReceipt.suppressedPairs<<" records="<<fourReceipt.generatedRecords<<'\n';
 VehicleFrameState49W arena{};VehicleFrameBindings49W bindings{};
 bindings.lifecycle.main190=static_cast<SceneNodeIndex49P>(0);
 VehicleFrameParameters49W parameters{};parameters.bodies.main.body=four[0].physical.body.parameters;
 parameters.bodies.main.centerOffset=four[0].physical.body.localCenter;
 arena.phase=VehicleFramePhase49W::Ready;
 need(!arena.fields.flagB9D0,"fresh bank arena has constructor-clear B9D bit0");
 const std::uint8_t bankB9C[4]={0x00u,0x01u,0x80u,0xffu};
 const bool bankB9D[4]={false,true,true,true};
 native_racer50HN::Bank bank;bank.owner(arena);bank.seal(4);
 for(std::size_t i=0;i<4;++i)bank.visit(i,[&](){arena.bodies.main.components.root=four[i].physical.body.state;
  arena.nodes[0].world100=four[i].physical.body.state.scene;
  arena.bodies.secondary.root.scene=pose(123.f+static_cast<float>(i),456);
  arena.bodies.main.controls.player.flagsB9C=bankB9C[i];arena.fields.flagB9D0=bankB9D[i];});
 const auto b9cRead=bank.bindRead(arena.bodies.main.controls.player.flagsB9C);
 const auto b9dRead=bank.bindRead(arena.fields.flagB9D0);
 for(std::size_t i=0;i<4;++i){
  need(bank.read(i,b9cRead)==bankB9C[i],"bank retains each independent B9C byte");
  need(bank.read(i,b9dRead)==bankB9D[i],"bank retains each independent B9D bit");
 }
 c::Receipt bankReceipt{};watch=true;
 status=c::applyAtWorldBoundary(bank,owner,arena,bindings,parameters,{{100,101,102,103}},{{0,0,0,0}},order,12,settings,response,bankReceipt);
 watch=false;
 need(status==c::Status::Complete&&bankReceipt.respondedPairs==fourReceipt.respondedPairs,"whole bank contact pass");
 for(std::size_t i=0;i<4;++i)bank.visit(i,[&](){
  need(arena.bodies.main.controls.player.flagsB9C==bankB9C[i]&&arena.fields.flagB9D0==bankB9D[i],"contact pass and bank switching preserve independent flags");
  need(!std::memcmp(&arena.bodies.main.components.root,&fourOutput[i].physical.body.state,sizeof(RootBodyState48H)),"all bank impulse results match standalone pass");
  const auto untouched=pose(123.f+static_cast<float>(i),456);
  need(!std::memcmp(&arena.bodies.secondary.root.scene,&untouched,sizeof untouched),"secondary body untouched");
  need(!std::memcmp(&arena.nodes[0].world100,&four[i].physical.body.state.scene,sizeof(BodyMatrix48G)),"bank scene untouched");
 });
 RootBodyState48H stoppedBefore{};bank.visit(3,[&](){arena.phase=VehicleFramePhase49W::Post354;
  stoppedBefore=arena.bodies.main.components.root;});bank.stop(3);
 watch=true;status=c::applyAtWorldBoundary(bank,owner,arena,bindings,parameters,{{100,101,102,103}},{{0,0,0,0}},onePair,1,settings,response,bankReceipt);watch=false;
 need(status==c::Status::Complete,"excluded non-Ready racer does not abort participants");
 bank.visit(3,[&](){need(arena.phase==VehicleFramePhase49W::Post354&&
  !std::memcmp(&stoppedBefore,&arena.bodies.main.components.root,sizeof stoppedBefore),"excluded failed racer unchanged");
  need(arena.bodies.main.controls.player.flagsB9C==bankB9C[3]&&arena.fields.flagB9D0==bankB9D[3],"excluded failed racer preserves both flag owners");});
 // Exercise independent scalar changes through the same established bank;
 // these are storage inputs, not claims of original gameplay producers.
 bank.visit(1,[&](){arena.bodies.main.controls.player.flagsB9C=0x5au;});
 need(bank.read(1,b9cRead)==0x5au&&bank.read(1,b9dRead),"banked B9C update preserves set B9D");
 bank.visit(2,[&](){arena.fields.flagB9D0=false;});
 need(bank.read(2,b9cRead)==bankB9C[2]&&!bank.read(2,b9dRead),"banked B9D update preserves B9C");
 need(bank.read(0,b9cRead)==bankB9C[0]&&bank.read(0,b9dRead)==bankB9D[0]&&
      bank.read(3,b9cRead)==bankB9C[3]&&bank.read(3,b9dRead)==bankB9D[3],"flag updates do not leak into other racer slots");
 std::array<c::Body,8> eight{},eightOutput{};std::array<c::Pair,56> eightOrder{};
 std::size_t eightPairCount=0;
 for(std::size_t i=0;i<8;++i){eight[i]=four[i%4];eight[i].physical.identity=static_cast<std::uint32_t>(200+i);
  if(i>=4)eight[i].physical.body.state.scene.v[12]+=200.f;
  for(std::size_t j=0;j<8;++j)if(i!=j)eightOrder[eightPairCount++]={i,j};}
 c::Receipt eightReceipt{};watch=true;
 status=owner.run(eight.data(),eight.size(),eightOrder.data(),eightPairCount,settings,response,eightOutput.data(),eightReceipt);watch=false;
 need(status==c::Status::Complete&&eightReceipt.testedPairs==56&&eightReceipt.respondedPairs>=6,"eight native racers complete ordered contacts");
 for(std::size_t i=0;i<8;++i)need(eightReceipt.contacts[i].count20C>0&&eightReceipt.contacts[i].count20C<=7,"every eight-racer participant has contacts within original ten-slot list");
 for(unsigned axis=0;axis<3;++axis){float before=0,after=0;
  for(unsigned i=0;i<8;++i){before+=eight[i].physical.body.state.body.velocity.v[axis];after+=eightOutput[i].physical.body.state.body.velocity.v[axis];}
  need(std::isfinite(after)&&std::fabs(after-before)<.004f,"eight racer source impulses preserve momentum");}
 std::cout<<"eight racer responses="<<eightReceipt.respondedPairs<<" suppressed="<<eightReceipt.suppressedPairs<<" records="<<eightReceipt.generatedRecords<<'\n';
 const auto previous=receipt;const auto saved=output[0];const c::Pair bad[]={{0,4}};
 need(owner.run(input,4,bad,1,settings,response,output,receipt)==c::Status::InvalidInput&&
      !std::memcmp(&previous,&receipt,sizeof receipt)&&!std::memcmp(&saved,&output[0],sizeof saved),"invalid pair transactional");
 c::ProcessedList seen;need(!seen.contains(9)&&seen.append(9)&&seen.contains(9)&&seen.append(9)&&seen.storage.count20C==1,"source processed list duplicate");
 need(allocations==0,"zero hot allocations");
 std::cout<<"{\"checks\":"<<checks<<",\"contact_x\":"<<hitX<<",\"contact_z\":"<<hitZ<<",\"generated_records\":"<<foundRecords
          <<",\"hot_allocations\":"<<allocations<<",\"guest_execution\":false,\"scope\":\"actual Pod01 geometry and native two-body response; host-ordered main-body seam\"}\n";
}
