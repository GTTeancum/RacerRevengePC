// Reconstructed from retained review probe after workspace reset.
// UNVERIFIED until built and run against the restored49X implementation.
// Finite cold-admitted inputs must not commit nonfinite world output.
#include "static_type2_producer49x.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <array>
using namespace rr::game;
namespace {
BodyMatrix48G identity(){BodyMatrix48G m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1.f;return m;}
}
int main(){
 Type2Ray49X ray[2]={{{{1.e29f,1.f,0.f,1.f}},{{1.e29f,-1.f,0.f,1.f}}},{{{1.e29f,1.f,0.f,1.f}},{{1.e29f,-1.f,0.f,1.f}}}};
 Type2Node49X qnode{};qnode.volume=2.f;qnode.radius=1.7320508f;qnode.children[0]=0x8000;qnode.children[1]=0x8001;qnode.center={1.e29f,0,0};qnode.half={1,1,1};
 Type2ResourceView49X shape{};shape.localMatrix=identity();shape.rays=ray;shape.rayCount=2;shape.root=0;shape.nodes=&qnode;shape.nodeCount=1;shape.maxDepth=1;
 std::uint32_t qt[3];std::size_t qdepth=99;
 const auto qstatus=validateType2Resource49X(shape,{qt,3},qdepth);
 rr::assets::CollisionMesh48I mesh{};mesh.local_matrix={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
 mesh.vertices={{{1.e29f-2.e22f,0.f,-1.e-20f}},{{1.e29f,0.f,1.e-20f}},{{1.e29f+2.e22f,0.f,-1.e-20f}}};
 rr::assets::CollisionMeshFace48I face{};face.vertices={0,1,2};face.normal={0,32767,0};mesh.faces={face,face};
 rr::assets::CollisionMeshNode48I node{};node.volume=1;node.center={1.e29f,0,0};node.half={1.e18f,1.f,1.e-20f};node.children={rr::assets::collision_mesh_leaf48i,rr::assets::collision_mesh_leaf48i|1};mesh.nodes={node};
 float radius[1];std::uint32_t mt[3];std::size_t md=0;
 const auto ms=prepareStaticTargetRadii49X(mesh,radius,1,{mt,3},md);
 CollisionFilter48J qf{};qf.identity=100;qf.categoryA4=4;qf.acceptMaskB8=0xffffffff;qf.flagsF8=4;qf.flags31=1;qf.categoryModeD4=1;
 Type2StaticQuery49X q{};q.resource=&shape;q.worldMatrix=identity();q.worldMatrix.v[0]=1.e-10f;q.filter=&qf;q.sphere={{{0,0,0}},100};q.collisionTypeTag=7;q.groupTypeTag=9;q.callbackEnabled=false;
 Type2StaticCandidate49X c{};c.nodeRadii=radius;c.nodeRadiusCount=1;c.maxDepth=md;
 auto& cc=c.candidate.collision;cc.filter=qf;cc.filter.identity=10;cc.geometry={&mesh,identity(),10};cc.geometry.worldMatrix.v[0]=1.e10f;cc.sphere=q.sphere;cc.typeTag=7;
 SpringCumulativePair48N ip[2]{},wp[2]{},dp[2]{};Type2PairMetadata49X im[2]{},wm[2]{},dm[2]{};
 TerrainCandidateEffects48J we[1]{},de[1]{};Type2WorkItem49X stack[3]{};
 Type2StaticInitial49X initial{};initial.pairs=ip;initial.metadata=im;initial.pairSlots=2;
 Type2StaticWorkspace49X w{};w.pairs=wp;w.pairCapacity=2;w.metadata=wm;w.metadataCapacity=2;w.effects=we;w.effectCapacity=1;w.stack=stack;w.stackCapacity=3;
 Type2StaticDestination49X d{};d.pairs=dp;d.pairCapacity=2;d.metadata=dm;d.metadataCapacity=2;d.effects=de;d.effectCapacity=1;
 Type2StaticResult49X out{};out.selected.surfacePoint.v[0]=-123.f;out.pairCount=98765;out.hasGeometryHit=true;
 std::memset(dp,0xa5,sizeof dp);std::memset(dm,0xa5,sizeof dm);std::memset(de,0xa5,sizeof de);
 std::array<unsigned char,sizeof out> beforeOut;std::array<unsigned char,sizeof dp> beforePair;
 std::array<unsigned char,sizeof dm> beforeMeta;std::array<unsigned char,sizeof de> beforeEffect;
 std::memcpy(beforeOut.data(),&out,sizeof out);std::memcpy(beforePair.data(),dp,sizeof dp);
 std::memcpy(beforeMeta.data(),dm,sizeof dm);std::memcpy(beforeEffect.data(),de,sizeof de);
 const auto status=produceStaticType2Contacts49X(q,&c,1,initial,w,d,out);
 const bool unchanged=std::memcmp(beforeOut.data(),&out,sizeof out)==0&&std::memcmp(beforePair.data(),dp,sizeof dp)==0&&std::memcmp(beforeMeta.data(),dm,sizeof dm)==0&&std::memcmp(beforeEffect.data(),de,sizeof de)==0;
 const bool pass=qstatus==Type2ResourceStatus49X::Complete&&ms==Type2ResourceStatus49X::Complete&&status==Type2StaticStatus49X::InvalidInput&&unchanged;
 std::cout<<(pass?"PASS":"FAIL")<<" finite world-output overflow: cold_query="<<unsigned(qstatus)<<" cold_mesh="<<unsigned(ms)<<" producer="<<unsigned(status)<<" destinations_unchanged="<<unchanged<<"\n";
 return pass?0:1;
}
