#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/model_bindings49o.h"
#include "../../src/assets/world_normals50hk.h"
#include "../../src/game/model_motion49t.h"
#include "../../src/game/source_acos49t.h"
#include "../../src/game/shared_random_owner50id.h"
#include "../../src/game/rigid_vertices49s.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

// Original SA ten-owner, one-motion/one-sequence world animation route.
// All ten TOB entries explicitly disable collision. No COL owner, collision
// registration, invented VUT, legacy conversion or replacement timer exists.
// Rendering retains the existing explicit native rigid-vertex/normal policy;
// neither this adapter nor ShortAxis49T claims original PS2 VU raster parity.
namespace sa_animated_world {
namespace a=rr::assets;namespace g=rr::game;
using Index=g::SceneNodeIndex49P;
inline constexpr std::size_t ownerCount=10,vertexCount=5623,indexCapacity=16869,surfaceCount=21;
inline constexpr std::size_t maxSourceNodes=14,maxRootInclusiveNodes=15,materialCount=8;
inline constexpr std::size_t archiveBytes=3786696,decodedBytes=29638656,archiveEntries=1065;
inline constexpr const char* archiveSha="34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a";
struct Span {
 std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0,material=0;
 std::uint32_t owner=0,sourceMaterial=0; // Authored owner/material, independent of global span ordinal.
};
struct Sequence {
 bool active90=true,loop88=true;
 std::int32_t nextSequenceA0=-1,nextMotionA8=-1;
 std::uint32_t stopped10=1;float current34=0.f,elapsedB0=0.f;
};
struct PlaybackShared {std::uint8_t guard844C=0;std::uint32_t value433468=0;};
using ServiceShared=g::SharedRandomOwner50ID;
struct State {std::array<g::SceneNode49P,maxRootInclusiveNodes> nodes{};Sequence sequence{};};
struct Source {
 std::vector<std::uint8_t> psg,vut,mot,mcf;
 a::PsgHierarchyView49O hierarchy{};a::MotionNodeBindings49O bindings{};
 a::MotionView49O motion{};a::Scene48D scene{};
 std::vector<g::RigidVertex49S> vertices;
 std::vector<std::array<double,3>> normals;
 std::vector<g::RigidPosition49S> projected;
 std::uint32_t startingZone=0,vertexStart=0,graphCount=0;
 std::uint32_t skippedBlankVutLines=0;
 bool motionLoops=true,useCollision=false,useLighting=true;
};
struct Diagnostic {std::string message;std::size_t owner=0;g::ModelMotionDiagnostic49T motion{};};
namespace detail {
inline void need(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
inline g::BodyMatrix48G identity(){g::BodyMatrix48G m{};for(unsigned i=0;i<16;i+=5)m.v[i]=1.f;return m;}
inline float add(float a,float b){volatile float x=a+b;need(std::isfinite(x),"SA animation time addition");return x;}
inline float sub(float a,float b){volatile float x=a-b;need(std::isfinite(x),"SA animation time subtraction");return x;}
inline const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
 const a::ParameterBlock48F* found=nullptr;
 for(const auto& b:p.blocks)if(b.name==name){need(!found,"SA duplicate animation block");found=&b;}
 need(found,"SA animation authored block absent");return *found;
}
inline const std::string* optional(const a::ParameterBlock48F& b,const char* name){const std::string* found=nullptr;for(const auto& f:b.fields)if(f.name==name){need(!found,"SA duplicate animation field");found=&f.value;}return found;}
inline const std::string& field(const a::ParameterBlock48F& b,const char* name){const auto* f=optional(b,name);need(f,"SA animation consumed field absent");return *f;}
inline bool mcfLine(const std::vector<std::uint8_t>& bytes,const std::string& wanted){
 const std::string text(bytes.begin(),bytes.end());std::size_t at=0;
 while(at<text.size()){auto end=text.find('\n',at);if(end==std::string::npos)end=text.size();auto stop=end;if(stop>at&&text[stop-1]=='\r')--stop;
  if(text.substr(at,stop-at)==wanted)return true;at=end+1;
 }return false;
}
struct Resource {const char* name;const char* sha;std::size_t bytes;};
struct Definition {
 const char* name;const char* geometry;std::uint32_t zone,nodes,vertices,surfaces,frames,durationBits,rotationType;
 Resource psg,vut,mot,mcf;
};
// BEGIN AUTHENTICATED LITERAL RESOURCE TABLE
inline constexpr std::array<Definition,ownerCount> definitions{{
 {"robotred01","robotred01",7,14,1927,3,151,0x41700000u,0,
  {"anim\\robotred01.psg","8bc768e4b01fb3bc8fc2082fc1a0fcd159fa311fd7904ed94006f8d8fa1f33fe",52564},
  {"anim\\robotred01.vut","473ebfb6cc276d587ad9a00e279f365ca0305df52fc8375d18bb656eb2279c62",438},
  {"anim\\robotred01.mot","bd80647eb7e39620299aa3798f2b192cfbebcc37c70215e6ef5efc9fd8bae0a0",23100},
  {"anim\\robotred01.mcf","fe7c201e416b11265c11f9a1280d128cd281627f6a1733ef046118b99e44fb16",199}},
 {"slide02","slide02",6,2,4,1,151,0x41700000u,0,
  {"anim\\slide02.psg","3c05d55ef69f6d4f50ad542dbd782245b4eccce7c7205c73d428c549304b356d",736},
  {"anim\\slide02.vut","7f49122d9a75111129ae42d234259268b1d226523af14c89f60ea51f601ebfe0",34},
  {"anim\\slide02.mot","83ae0150736bbe765f782473aba66d1622d84f7bdb43bcc82d0ed2faada3e926",7292},
  {"anim\\slide02.mcf","7a246558dbbaa315af01b56e89ef16824d7d4491cee53c55b71c17d6708e3634",187}},
 {"conrobo01","conrobo01",4,4,252,3,86,0x41080000u,0,
  {"anim\\conrobo01.psg","c880140696a831152b65314553c5b06add987ce9584e9f55f911a3f34c94fb1a",7688},
  {"anim\\conrobo01.vut","3f95a94e15ecefdb7c3d05eecafb5c2127d39281bcb701ff359b64d9e5be0add",86},
  {"anim\\conrobo01.mot","4f24ede78a9d58a89324fc168ce4dc312f9acee48879d52f7447fe3cf05f5360",6260},
  {"anim\\conrobo01.mcf","ebd69d4a7c0ac81b26d23a62e311ecc5acf5bc4ad813ca0db277d315f5645744",195}},
 {"conrobo06","conrobo06",4,3,204,2,86,0x41080000u,0,
  {"anim\\conrobo06.psg","99f3cf3e63b809b3faa80bba104bd7c6d3a862ee69b97aa9611d9e781b149f75",6132},
  {"anim\\conrobo06.vut","badd21c051ad7360086c90a10b6c35a232a81ac4d73bd0fe09249579f3318474",65},
  {"anim\\conrobo06.mot","d707ab91519b49f347285b2925fdf69df2e03e62f17cf55b8a388919a2e3fb20",2116},
  {"anim\\conrobo06.mcf","0f43065bfef04341a52054e8292e8a10ac56886fc54b2cf6fe5f7d56b7e026d0",195}},
 {"conrobobox01","conrobobox01",4,2,206,1,86,0x41080000u,0,
  {"anim\\conrobobox01.psg","9c03cefba5dee57fbfbe13b7ba33e017420a187f237a698c0b6694151a1d3869",5872},
  {"anim\\conrobobox01.vut","5ffb2a2a354c73e44dc5073101775fd6f18cb45764744058e341dd89cd144761",44},
  {"anim\\conrobobox01.mot","43a5207ba840d9e17fa8679c728fdd1a700643dade7b737ef93eaaf68b44c1a5",4172},
  {"anim\\conrobobox01.mcf","2cdb856f76bc604f931a1510aba7717351322c5f7864c9b086d0118b5bdb8234",207}},
 {"conrobobox02","conrobobox01",4,2,206,1,86,0x41080000u,0,
  {"anim\\conrobobox01.psg","9c03cefba5dee57fbfbe13b7ba33e017420a187f237a698c0b6694151a1d3869",5872},
  {"anim\\conrobobox01.vut","5ffb2a2a354c73e44dc5073101775fd6f18cb45764744058e341dd89cd144761",44},
  {"anim\\conrobobox02.mot","eb58b680950a3cf8760bd0c3a87c814dededece3a67e407c9eafdaf84a7034c2",4172},
  {"anim\\conrobobox02.mcf","e7dffa650ce4cf41d307c46068f89295a67f25b470a9f66d03a66dd694bd8150",207}},
 {"conrobobox03","conrobobox01",4,2,206,1,86,0x41080000u,0,
  {"anim\\conrobobox01.psg","9c03cefba5dee57fbfbe13b7ba33e017420a187f237a698c0b6694151a1d3869",5872},
  {"anim\\conrobobox01.vut","5ffb2a2a354c73e44dc5073101775fd6f18cb45764744058e341dd89cd144761",44},
  {"anim\\conrobobox03.mot","8d2eed43c1312adbcfd730582b5a603b042ec293115b7ab9db6b67c1d9b4971a",4172},
  {"anim\\conrobobox03.mcf","297296676a98a699a2228bf30e7825f9f60b0eb26b50bb705c131a23811a3da3",207}},
 {"conrobobox04","conrobobox01",4,2,206,1,86,0x41080000u,0,
  {"anim\\conrobobox01.psg","9c03cefba5dee57fbfbe13b7ba33e017420a187f237a698c0b6694151a1d3869",5872},
  {"anim\\conrobobox01.vut","5ffb2a2a354c73e44dc5073101775fd6f18cb45764744058e341dd89cd144761",44},
  {"anim\\conrobobox04.mot","bbf499d8eab51953a08c029a1de182a449f6c0b73b0205b570e227032b5123ab",4172},
  {"anim\\conrobobox04.mcf","804e517619207ea50227e7b3d8cadbfb676f18826250a96b4f52e1cd0dbc5f69",207}},
 {"lazerarm01","lazerarm01",4,5,1206,4,81,0x40aaaaabu,1,
  {"anim\\lazerarm01.psg","b6e2d5c304b25bd3d476b4f86bee18b3b0afe0da0a648161958c2a5cf12f4fd6",22624},
  {"anim\\lazerarm01.vut","368a25648edc14ced7be16685ad4536416a38b736d856eec3a4ee0b88c5eaa92",113},
  {"anim\\lazerarm01.mot","68304e554bb25de73bb6d4afffaea0dc326853f74aac4dfd18018d6b3d94f6e8",4928},
  {"anim\\lazerarm01.mcf","91892d04d0321f5110e218c26f65641869465db76ea292feabde2d85af911b1e",223}},
 {"lazerarm08","lazerarm08",4,5,1206,4,81,0x40aaaaabu,1,
  {"anim\\lazerarm08.psg","3176e2f5143da6d6d472180f293ab3c6071b7fb3647bbb12ae7071145c64472d",22624},
  {"anim\\lazerarm08.vut","1318181da26d5fb537bae1150f3f8a8fe09e5913a7bd0d59fe0b45ba12f1d566",113},
  {"anim\\lazerarm08.mot","124398a8533627c908ee66d022cd903cb8dfaf78db57ccaa16fc4a9c3d8dfd98",4928},
  {"anim\\lazerarm08.mcf","68d30c02fd27cad052cd755572043685dd2b6d12f5ccaf96872af1f6b31e7b35",223}},
}};
inline constexpr std::array<Resource,materialCount> materialResources{{
 {"textures\\robotred01.psm","1399a9d5036eeb256519aa158d3e08ed57d55ef732db3292be26cc667f73967b",264},
 {"textures\\robotred02.psm","94bb99f58efd0906a63483c4498b0de6fc2c0b4e7e0a48bdf0c29cd1ce96e2db",264},
 {"textures\\robox.psm","6f94d54318721016e29c6b78c03210eb5b692ca0240bef663eb9036a75e0a995",259},
 {"textures\\slide02.psm","2d297d770aa1093673ba655520ffe2ed473635be045296723b158cea9b50a016",261},
 {"textures\\larobo02.psm","7aa25119b4fe537cdaaa017e29688a22574b2fdd28de99a3b772d1b210a6a7a4",262},
 {"textures\\larobo01.psm","267402e7b6343ebb4cbc37917c871efb005dd1c0389b10174d43540406bf8a54",262},
 {"textures\\larobo03.psm","5bb71a3a03f0071c9ebdcff765502e3614d4c62bff2916b66080776583062770",262},
 {"textures\\lazerrobo01.psm","bb00825e7998cb3f0aca74c83f192b5baf0fbe9b635bfb4ee8e57f33e52945ab",265},
}};
inline constexpr std::array<const char*,materialCount> materialNames{{"robotred01","robotred02","robox","slide02","larobo02","larobo01","larobo03","lazerrobo01"}};
// END AUTHENTICATED LITERAL RESOURCE TABLE
inline void install(const Source& source,State& state){
 for(auto& n:state.nodes){n={};n.localC0=n.world100=identity();}
 state.nodes[0].sceneFlags190=0x98; // Fresh root80,2D9214 sets08/10. SA offset is explicitly zero.
 for(std::uint32_t i=0;i<source.hierarchy.nodeCount;++i){a::PsgNode49O original{};
  need(a::readPsgNode49O(source.hierarchy,i,original),"SA original PSG hierarchy row rejected");
  const auto target=i+1,parent=original.parent<0?0u:static_cast<unsigned>(original.parent)+1;
  auto& n=state.nodes[target];std::memcpy(n.localC0.v,original.local,sizeof original.local);
  n.sceneFlags190=0xa4;n.parent180=static_cast<Index>(parent);
  auto& first=state.nodes[parent].firstChild184;
  if(first==Index::none)first=static_cast<Index>(target);
  else{auto previous=static_cast<unsigned>(first);while(state.nodes[previous].nextSibling188!=Index::none)previous=static_cast<unsigned>(state.nodes[previous].nextSibling188);state.nodes[previous].nextSibling188=static_cast<Index>(target);}
 }
 need(g::validateSceneGraph49P({state.nodes.data(),source.graphCount}),"SA animation source graph rejected");
}
inline void lazy(PlaybackShared& shared){if(!shared.guard844C){shared.value433468=0xffffffffu;shared.guard844C=1;}}
inline bool step(const Source& source,Sequence& q,float delta){
 need(std::isfinite(delta)&&delta>=0.f,"SA animation invalid source delta");
 if(q.nextSequenceA0==-1){q.nextSequenceA0=0;
  if(!q.active90)q.nextSequenceA0=q.nextMotionA8=-1;
  else{++q.nextMotionA8;
   if(q.nextMotionA8>=1){if(!q.loop88){q.active90=false;--q.nextMotionA8;q.nextSequenceA0=-1;}else q.nextMotionA8=0;}
  }
 }
 //1015EC current/next motion pointers are equal for each authored one-motion
 // sequence. AC=0, so the near-end cross-motion blend/restart route is absent.
 if(q.stopped10&&q.active90){q.nextSequenceA0=-1;q.elapsedB0=0.f;q.stopped10=0;q.current34=0.f;}
 if(q.stopped10)return false;
 q.elapsedB0=add(q.elapsedB0,delta);q.current34=add(q.current34,delta);
 //151160 is strict > duration.154528..154534 subtracts ONCE, not modulo,
 // preserving a large source delta and resulting position extrapolation.
 if(q.current34>source.motion.duration&&source.motionLoops)q.current34=sub(q.current34,source.motion.duration);
 return true;
}
}
class Owner final {
 std::array<Source,ownerCount> source_{};
 std::array<State,ownerCount> state_{},pending_{};
 std::array<std::vector<std::uint8_t>,materialCount> psm_{};
 a::Mesh48B mesh_{};std::vector<a::Vertex48B> nextVertices_;
 std::vector<a::WorldNormal50HK> normals_,nextNormals_;
 std::array<Span,surfaceCount> spans_{};
 g::ModelMotionWorkspace49T workspace_{};PlaybackShared playback_{};
 bool attempted_=false,ready_=false;
 bool project(std::size_t owner,State& state,std::vector<a::Vertex48B>& vertices,std::vector<a::WorldNormal50HK>& normals){
  auto& source=source_[owner];const g::SceneGraph49P graph{state.nodes.data(),source.graphCount};
  std::array<g::BodyMatrix48G,maxSourceNodes> palette{};
  for(std::uint32_t i=0;i<source.hierarchy.nodeCount;++i){if(!g::refreshSceneWorld49P(graph,static_cast<Index>(i+1)))return false;palette[i]=state.nodes[i+1].world100;}
  g::RigidVertexDiagnostic49S diagnostic{};
  if(!g::projectRigidVertices49S({source.vertices.data(),static_cast<std::uint32_t>(source.vertices.size())},
      {palette.data(),source.hierarchy.nodeCount},{source.projected.data(),static_cast<std::uint32_t>(source.projected.size())},diagnostic))return false;
  for(std::size_t i=0;i<source.vertices.size();++i){
   std::memcpy(vertices[source.vertexStart+i].position,source.projected[i].v,sizeof source.projected[i].v);
   const auto& m=palette[source.vertices[i].paletteIndex];std::array<double,16> matrix{};
   for(unsigned n=0;n<16;++n)matrix[n]=m.v[n];
   if(!a::transformWorldNormal50HK(source.normals[i],matrix,normals[source.vertexStart+i]))return false;
  }return true;
 }
 void updateBounds()noexcept{
  if(mesh_.vertices.empty())return;
  for(unsigned axis=0;axis<3;++axis){mesh_.bounds.minimum[axis]=mesh_.bounds.maximum[axis]=mesh_.vertices.front().position[axis];
   for(const auto& v:mesh_.vertices){mesh_.bounds.minimum[axis]=std::min(mesh_.bounds.minimum[axis],v.position[axis]);mesh_.bounds.maximum[axis]=std::max(mesh_.bounds.maximum[axis],v.position[axis]);}
  }
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool load(const std::filesystem::path& archivePath,g::SourceLibmState49T& libm,g::ModelMotionSharedState49T& shared,Diagnostic& d){
  (void)libm; // Every original SA MOT is already -2; no legacy quaternion conversion.
  d={};if(attempted_){d.message="SA animated world requires a fresh stable owner";return false;}attempted_=true;
  try{using namespace detail;namespace f=pod_materials50ed_fixture::detail;
   f::Inputs input;input.capacity=archiveBytes;const auto bytes=input.read(archivePath,archiveBytes);
   need(bytes.size()==archiveBytes&&f::sha256(bytes)==archiveSha,"SA animated original archive identity");
   a::ResArchive48C archive;a::DecodeError48C error{};a::DecodeLimits48C limits{};
   limits.resource_file_bytes=archiveBytes;limits.resource_decoded_bytes=decodedBytes;limits.resource_entries=archiveEntries;
   need(a::decodeResource48C(bytes,archive,error,nullptr,limits),"SA animated original archive decode");
   need(archive.entries.size()==archiveEntries&&archive.decoded.size()==decodedBytes,"SA animated original archive extent");
   auto member=[&](const Resource& r){std::vector<std::uint8_t> out;need(a::extractResMember48C(archive,r.name,out,error),"SA animation member missing");need(out.size()==r.bytes&&f::sha256(out)==r.sha,"SA animation original member identity");return out;};
   const auto tob=member({"tracks\\sa.tob","b2b53df201ddfc5bd0283cd50911c6e94937f3633bba33b864da13557f415542",13314});
   a::ParameterDocument48F params;need(a::decodeParameters48F(tob,params,error),"SA original TOB decode");
   need(field(block(params,"animations"),"numberofanimations")=="10","SA original animated owner count");
   for(std::size_t i=0;i<materialCount;++i){psm_[i]=member(materialResources[i]);a::Material48E psm;
    need(a::decodeMaterial48E(psm_[i],psm,error)&&psm.passes.size()==1,"SA original animation material decode");
    const auto& pass=psm.passes.front();need(pass.pass==1&&pass.microprogram=="DiffuseCompressed"&&pass.textures.size()==1&&pass.textures[0].slot==1&&pass.textures[0].name==materialNames[i],"SA animation compressed material proof");
   }
   auto nextShared=shared;mesh_.vertices.reserve(vertexCount);mesh_.indices.reserve(indexCapacity);normals_.resize(vertexCount);
   std::size_t spanIndex=0;
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;auto& source=source_[i];const auto& definition=definitions[i];const std::string name=definition.name;const auto firstSpan=spanIndex;
    source.psg=member(definition.psg);source.vut=member(definition.vut);source.mot=member(definition.mot);source.mcf=member(definition.mcf);
    // MCFs02/03/04 deliberately select conrobobox01 PSG/VUT and distinct MOTs.
    // These literals are independently SHA-pinned; missing resources never
    // become identity bindings, a same-name guess or a dummy geometry owner.
    need(mcfLine(source.mcf,"ContentDirectory=anim\\")&&mcfLine(source.mcf,"SLTFile="+std::string(definition.geometry)+".slt")&&
      mcfLine(source.mcf,"VUTfilename="+std::string(definition.geometry)+".Vut")&&mcfLine(source.mcf,"NAME="+name)&&
      mcfLine(source.mcf,"NumberOfMotions=1")&&mcfLine(source.mcf,"Looping=yes"),"SA original MCF resource binding differs");
    const auto& authored=block(params,"animation"+std::to_string(i+1));
    need(field(authored,"animationtype")=="KeyFramed"&&field(authored,"mcf")==name+".mcf"&&field(authored,"motiontoplay1")==name&&
      field(authored,"numberofmotions")=="1"&&field(authored,"numberinsequence1")=="1"&&field(authored,"motionsequence1")=="1"&&
      field(authored,"useblendedkeys")=="T"&&field(authored,"offset")=="0.0,0.0,0.0","SA authored single-motion scope");
    need(!optional(authored,"numberinsequence")&&!optional(authored,"lookvector")&&!optional(authored,"upvector")&&
      !optional(authored,"yaw")&&!optional(authored,"pitch")&&!optional(authored,"roll"),"SA animation unjoined authoring");
    need(field(authored,"startingzone")==std::to_string(definition.zone)&&field(authored,"loopsequence1")=="T"&&
      field(authored,"sequenceactive1")=="T"&&field(authored,"onetimeonly")=="T"&&field(authored,"uselighting")=="T"&&
      field(authored,"usecollision")=="F","SA original animation activation/collision differs");
    source.startingZone=definition.zone;source.vertexStart=static_cast<std::uint32_t>(mesh_.vertices.size());
    a::GeometryDiagnostic49O geometry{};a::MotionBindingDiagnostic49O binding{};
    need(a::readPsgHierarchy49O(source.psg.data(),source.psg.size(),source.hierarchy,geometry)&&source.hierarchy.nodeCount==definition.nodes,"SA original PSG hierarchy");
    source.graphCount=source.hierarchy.nodeCount+1;
    std::size_t bindingBytes=source.vut.size();
    if(i==3){
     // Only SHA-pinned conrobo06 ends in an additional CRLF blank line.
     //12F234 rewrites that CRLF as LF.153228 tokenizes with '=,' (which does
     // not strip LF),1543FC finds no matching name,153248 sees FF,15325C
     // skips every rename/flag store. Preserve original bytes and expose the
     // skipped diagnostic count; pass only the three actual binding records
     // to strict49O. This does not globally admit arbitrary blank VUT input.
     need(bindingBytes==65&&source.vut[bindingBytes-4]=='\r'&&source.vut[bindingBytes-3]=='\n'&&
       source.vut[bindingBytes-2]=='\r'&&source.vut[bindingBytes-1]=='\n',"SA original conrobo06 blank line changed");
     bindingBytes-=2;source.skippedBlankVutLines=1;
    }
    need(a::readMotionNodeBindings49O(source.hierarchy,reinterpret_cast<const char*>(source.vut.data()),bindingBytes,source.bindings,binding),"SA actual VUT bindings");
    for(std::uint32_t row=0;row<source.bindings.count;++row)++source.bindings.rows[row].sceneNode;
    need(a::readMot49O(source.mot.data(),source.mot.size(),source.motion,geometry),"SA actual original MOT view");
    std::uint32_t durationBits=0;std::memcpy(&durationBits,&source.motion.duration,sizeof durationBits);
    need(source.motion.frameCount==definition.frames&&durationBits==definition.durationBits&&source.motion.rotationType==definition.rotationType,"SA original MOT time/rotation extent");
    // Runtime MOT clocks differ from MCF FrameCount/FPS. Retain original
    //15/8.5/5.33333349 durations and ShortAxis keys; never retime to MCF.
    a::SceneLimits48D sceneLimits{};sceneLimits.allow_signed32_quantized_positions=true;
    need(a::decodeScene48D(source.psg,source.scene,error,nullptr,sceneLimits),"SA animated PSG visual decoder");
    need(source.scene.nodes.size()==definition.nodes&&source.scene.lods.size()==1&&!source.scene.world_space&&source.scene.identity_matrix_palette&&
      source.scene.lods[0].surfaces.size()==definition.surfaces&&source.scene.materials.size()==definition.surfaces,"SA animated rigid palette/surface scope");
    for(const auto& surface:source.scene.lods[0].surfaces){need(spanIndex<spans_.size()&&surface.material_index<source.scene.materials.size(),"SA animated surface ceiling");
     need(surface.material_index==spanIndex-firstSpan,"SA authored material order");
     const auto& material=source.scene.materials[surface.material_index];bool known=false;for(const auto* expected:materialNames)known=known||material==expected;
     need(known&&surface.component_word==0x107&&!surface.has_original_colors,"SA original compressed/no-RGB surface");
     auto& span=spans_[spanIndex];span={static_cast<std::uint32_t>(mesh_.vertices.size()),static_cast<std::uint32_t>(surface.vertices.size()),
      static_cast<std::uint32_t>(mesh_.indices.size()),static_cast<std::uint32_t>(surface.indices.size()),static_cast<std::uint32_t>(spanIndex),static_cast<std::uint32_t>(i),surface.material_index};
     for(const auto& original:surface.vertices){need(original.bone<source.hierarchy.nodeCount,"SA animated vertex palette bound");
      g::RigidVertex49S v{};for(unsigned n=0;n<3;++n)v.positionLocal[n]=original.position_local[n];v.paletteIndex=original.bone;source.vertices.push_back(v);source.normals.push_back(original.normal_local);
      a::Vertex48B shown{};for(unsigned n=0;n<2;++n)shown.uv[n]=static_cast<float>(original.uv[n]);
      // Explicit existing host diffuse compatibility: absent source RGB is
      // neutral255; original PSM/PST and normals remain required separately.
      for(unsigned n=0;n<4;++n)shown.color[n]=255;mesh_.vertices.push_back(shown);
     }
     for(const auto index:surface.indices){need(index<surface.vertices.size(),"SA animated local triangle index");mesh_.indices.push_back(span.vertexStart+index);}++spanIndex;
    }
    need(source.vertices.size()==definition.vertices,"SA animated source vertex extent");source.projected.resize(source.vertices.size());install(source,state_[i]);
    lazy(playback_);need(g::sampleModelMotionTime50IG(source.motion,source.bindings,0.f,{state_[i].nodes.data(),source.graphCount},nextShared,workspace_,d.motion),"SA original cold animation pose");
    need(project(i,state_[i],mesh_.vertices,normals_),"SA original animated world projection");
   }
   need(mesh_.vertices.size()==vertexCount&&!mesh_.indices.empty()&&mesh_.indices.size()<=indexCapacity&&spanIndex==surfaceCount,"SA animated aggregate extent");
   nextVertices_=mesh_.vertices;nextNormals_=normals_;pending_=state_;updateBounds();shared=nextShared;ready_=true;d={};return true;
  }catch(const std::exception& e){d.message=e.what();return false;}
 }
 bool advance(float delta,g::ModelMotionSharedState49T& shared,Diagnostic& d){
  d={};if(!ready_){d.message="SA animated world is not ready";return false;}
  try{pending_=state_;auto nextShared=shared;auto playback=playback_;
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;auto& source=source_[i];auto& state=pending_[i];
    if(detail::step(source,state.sequence,delta)){detail::lazy(playback);
     detail::need(g::sampleModelMotionTime50IG(source.motion,source.bindings,state.sequence.current34,{state.nodes.data(),source.graphCount},nextShared,workspace_,d.motion),"SA source-time animation rejected");
    }
    detail::need(project(i,state,nextVertices_,nextNormals_),"SA animated live projection rejected");
   }
   state_=pending_;shared=nextShared;playback_=playback;
   std::copy(nextVertices_.begin(),nextVertices_.end(),mesh_.vertices.begin());std::copy(nextNormals_.begin(),nextNormals_.end(),normals_.begin());updateBounds();return true;
  }catch(const std::exception& e){d.message=e.what();return false;}
 }
 //101860 world service takes ONE shared RNG draw before iterating all owners.
 // Do not call this once per animation or seed a private animation RNG.
 // Retains GA's explicit transactional failure staging, rather than claiming
 // original partial RNG/motion mutation prefixes when native admission fails.
 bool service(float delta,std::uint32_t sourceTimestamp,std::uint32_t& sharedRaw,ServiceShared& lazy,g::ModelMotionSharedState49T& shared,Diagnostic& d){
  auto next=lazy;auto raw=sharedRaw;
  if(!next.seed.lazy80A8){next.seed.seed3D6B30=0;next.seed.lazy80A8=true;}
  if(!next.seed.seed3D6B30){next.seed.seed3D6B30=sourceTimestamp;raw=sourceTimestamp;}
  raw=raw*0x41c64e6du+0x3039u;
  volatile float converted=static_cast<float>(raw&0x7fffffffu);volatile float unit=converted*0x1p-31f;volatile float percent=101.f*unit;
  if(!next.range.lazy80AC){next.range.rangeCheck3D6B38=1;next.range.lazy80AC=true;}
  if(!std::isfinite(percent)||percent<0.f||percent>=101.f){d={};d.message="SA animation source RNG range";return false;}
  if(!advance(delta,shared,d))return false;sharedRaw=raw;lazy=next;return true;
 }
 const a::Mesh48B& mesh()const noexcept{return mesh_;}
 const std::vector<a::WorldNormal50HK>& normals()const noexcept{return normals_;}
 const std::array<Span,surfaceCount>& spans()const noexcept{return spans_;}
 const Source& source(std::size_t i)const{return source_.at(i);}
 const State& state(std::size_t i)const{return state_.at(i);}
 bool ready()const noexcept{return ready_;}
};
} // namespace sa_animated_world
