// Cold original-RES importer. No guest execution, recoloring, or mesh substitution.
#include "assets/import_decode48c.h"
#include "assets/import_scene48d.h"
#include "assets/import_texture48e.h"
#include "assets/scene_asset48d.h"
#include "assets/native_asset48b.h"
#include "assets/pod_physics49n.h"
#include "ma_compressed_scene_admission.h"
#include "ra_compressed_scene_admission.h"
#include "ra_tga_import.h"
#include "native-json/json.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
namespace fs=std::filesystem;
namespace a=rr::assets;
using J=nlohmann::json;
using Bytes=std::vector<std::uint8_t>;
static void need(bool p,const std::string& why){if(!p)throw std::runtime_error(why);}
// Hash the actual raw bytes, not a filename or an admission pin. Reuse the
// native SHA-256 block transform already used by MA/RA admission; the padding
// and hex order mirror ma_compressed_scene::detail::hashMatches.
static std::string sourceSha256(const Bytes& bytes) {
 std::array<std::uint32_t,8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
  0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
 std::size_t at=0;
 for(;bytes.size()-at>=64;at+=64)
  ma_compressed_scene::detail::shaBlock(bytes.data()+at,h);
 std::array<std::uint8_t,128> tail{};
 const auto remainder=bytes.size()-at;
 if(remainder)std::memcpy(tail.data(),bytes.data()+at,remainder);
 tail[remainder]=0x80;
 const std::size_t padded=remainder<56?64:128;
 const auto bits=static_cast<std::uint64_t>(bytes.size())*8u;
 for(unsigned i=0;i<8;++i)tail[padded-1-i]=static_cast<std::uint8_t>(bits>>(i*8));
 ma_compressed_scene::detail::shaBlock(tail.data(),h);
 if(padded==128)ma_compressed_scene::detail::shaBlock(tail.data()+64,h);
 constexpr char hex[]="0123456789abcdef";
 std::string result(64,'0');
 for(unsigned i=0;i<64;++i)result[i]=hex[(h[i/8]>>((7u-i%8)*4))&15u];
 return result;
}
static std::string normal(std::string s){for(auto& c:s){if(c=='\\')c='/';if(c>='A'&&c<='Z')c+=32;}return s;}
// Retain only this POD's four original speech catalogs, never arbitrary INIs.
static bool originalPodSpeechIni(const fs::path& input,const std::string& member){
 const auto pod=normal(input.stem().string()),name=normal(member);
 if(pod.size()!=5||pod.substr(0,3)!="pod"||pod[3]<'0'||pod[3]>'9'||pod[4]<'0'||pod[4]>'9')return false;
 const auto prefix="audio/speech/"+pod+"/";
 for(const auto category:{"curses","taunts","special","sounds"})if(name==prefix+category+".ini")return true;
 return false;
}
static Bytes read(const fs::path& p){std::ifstream f(p,std::ios::binary|std::ios::ate);need(bool(f),"Cannot read "+p.string());auto n=f.tellg();need(n>0&&n<=64*1024*1024,"Resource outside bounded size");Bytes b(static_cast<std::size_t>(n));f.seekg(0);f.read(reinterpret_cast<char*>(b.data()),b.size());need(bool(f),"Short resource read");return b;}
static void write(const fs::path& p,const Bytes& b){need(!fs::exists(p),"Refusing overwrite "+p.string());std::ofstream f(p,std::ios::binary);f.write(reinterpret_cast<const char*>(b.data()),b.size());f.close();need(bool(f),"Cannot write "+p.string());}
static void writeJ(const fs::path& p,const J& j){auto s=j.dump(2)+"\n";write(p,Bytes(s.begin(),s.end()));}
static J fields(const a::MaterialFields48E& f){J r={{"authored",J::array()},{"unspecified",J::array()}};for(auto p:{&f.authored,&f.unspecified})for(auto& x:*p)r[p==&f.authored?"authored":"unspecified"].push_back({{"name",x.name},{"value",x.value},{"source_offset",x.source_offset}});return r;}
int main(int argc,char** argv){try{
 const bool maCompressed=argc==4&&std::string(argv[3])=="--authenticated-ma-compressed";
 const bool raCompressed=argc==4&&std::string(argv[3])=="--authenticated-ra-compressed";
 need(argc==3||maCompressed||raCompressed,"Usage: import_original_pods RESOURCE.RES NEW_OUTPUT_DIRECTORY [--authenticated-ma-compressed|--authenticated-ra-compressed]");const fs::path input=argv[1],out=argv[2];need(!fs::exists(out),"Output directory must be new");
 a::ResArchive48C res;a::DecodeError48C e;const auto raw=read(input);need(a::decodeResource48C(raw,res,e),"RES decode: "+e.code+" "+e.detail);
 ma_compressed_scene::Admission maAdmission;
 ra_compressed_scene::Admission raAdmission;
 if(maCompressed)maAdmission.authenticate(raw,res);
 if(raCompressed){
  raAdmission.authenticate(raw,res);
  ra_tga_import::authenticate(raw,res,sourceSha256);
 }
 const auto containerSha256=sourceSha256(raw);
 fs::create_directories(out);
 J manifest={{"schema","RR_ORIGINAL_POD_IMPORT_V1"},{"source_container",input.filename().string()},{"source_container_bytes",raw.size()},{"source_container_sha256",containerSha256},{"members",J::array()}};
 if(maCompressed)manifest["compressed_position_admission"]="Exact authenticated MA archive, 21 pinned static PSGs, actual pinned one-pass PrelitCompressed PSMs";
 if(raCompressed)manifest["compressed_position_admission"]="Exact authenticated RA archive, 583 pinned single-node PSGs, actual pinned one-pass PrelitCompressed/DiffuseCompressed PSMs";
 for(std::size_t i=0;i<res.entries.size();++i){const auto& r=res.entries[i];const auto ext=fs::path(normal(r.name)).extension().string();
  // Exact RA authentication above bounds this to the original 1010 members.
  // Retain ATM/TGA and world/collision/effect sources with their actual hashes;
  // unconverted kinds remain original_source_retained, never native assets.
  // MA and default imports retain their existing member selection.
  if(!raCompressed&&(ext!=".psg"&&ext!=".psm"&&ext!=".pst"&&ext!=".phy"&&ext!=".slt"&&ext!=".mcf"&&ext!=".mot"&&ext!=".vut"&&ext!=".aub"&&!originalPodSpeechIni(input,r.name)))continue;
  Bytes b;need(a::extractResMember48C(res,r.name,b,e),"Extract "+r.name+": "+e.code);const std::string stem=std::to_string(i);const auto source=stem+"-original"+ext;write(out/source,b);
  J item={{"source_member",r.name},{"source_index",i},{"logical_offset",r.logical_offset},{"source_bytes",r.bytes},{"source_sha256",sourceSha256(b)},{"source_file",source},{"kind",ext.substr(1)}};
  if(ext==".psg") {a::Scene48D scene;const auto sceneLimits=raCompressed?raAdmission.limitsFor(r,b):
   (maCompressed?maAdmission.limitsFor(r,b):a::SceneLimits48D{});if(a::decodeScene48D(b,scene,e,nullptr,sceneLimits)){Bytes native;std::string error;need(a::encodeSceneAsset48D(scene,native,error),"Scene encode "+error);a::Scene48D check;need(a::parseSceneAsset48D(native,check,error)&&check.neutral_lod0==scene.neutral_lod0,"Scene round trip "+error);write(out/(stem+".rrscene"),native);write(out/(stem+".rrmesh"),scene.neutral_lod0);a::Mesh48B mesh;need(a::parseMesh48B(scene.neutral_lod0.data(),scene.neutral_lod0.size(),mesh,error),"Mesh roundtrip "+error);item.update({{"status","converted_geometry"},{"rrscene",stem+".rrscene"},{"rrmesh",stem+".rrmesh"},{"nodes",scene.nodes.size()},{"vertices",mesh.vertices.size()},{"indices",mesh.indices.size()},{"materials",scene.materials},{"lods",scene.lods.size()},{"world_space",scene.world_space},{"node_names",J::array()},{"spans",J::array()}});for(const auto& n:scene.nodes)item["node_names"].push_back(n.name);for(const auto& s:scene.spans_lod0)item["spans"].push_back({{"material_index",s.material_index},{"vertex_start",s.vertex_start},{"vertex_count",s.vertex_count},{"index_start",s.index_start},{"index_count",s.index_count}});
   item["bounds"]={{"minimum",{mesh.bounds.minimum[0],mesh.bounds.minimum[1],mesh.bounds.minimum[2]}},{"maximum",{mesh.bounds.maximum[0],mesh.bounds.maximum[1],mesh.bounds.maximum[2]}}};
  }else item.update({{"status","unsupported_geometry"},{"error",e.code},{"detail",e.detail}});
  }else if(raCompressed&&ext==".tga"&&ra_tga_import::find(r.name)){
   const auto& pin=*ra_tga_import::find(r.name);
   const auto mips=ra_tga_import::convert(pin,b,item["source_sha256"].get<std::string>());
   item.update({{"status","converted_texture"},{"source_format",pin.depth==24?"tga_bgr24":"tga_bgra32"},
    {"converter","rr-original-tga-50LA-v1"},{"width",pin.dimension},{"height",pin.dimension},
    {"mips",mips.size()},{"native_mips",J::array()}});
   for(std::size_t level=0;level<mips.size();++level){
    const auto& mip=mips[level];Bytes encoded;
    need(a::encodeTextureLevel48E(mip,encoded,e),"Native RA TGA mip encode: "+e.code+" "+e.detail);
    const auto filename=stem+"-mip"+std::to_string(level)+".rrtex";
    const auto sha256=sourceSha256(encoded);write(out/filename,encoded);
    item["native_mips"].push_back({{"index",level},{"width",mip.width},{"height",mip.height},
     {"rrtex",filename},{"rrtex_bytes",encoded.size()},{"rrtex_sha256",sha256}});
    if(level+1==mips.size())item.update({{"rrtex",filename},
     {"rrtex_bytes",encoded.size()},{"rrtex_sha256",sha256}});
   }
  }else if(ext==".pst"){
   a::Texture48E t;
   if(a::decodeTexture48E(b,t,e)){
    write(out/(stem+".rrtex"),t.base_rrtex);
    item.update({{"status","converted_texture"},{"rrtex",stem+".rrtex"},{"width",t.mips.back().width},{"height",t.mips.back().height},{"mips",t.mips.size()},{"texture_kind",t.kind}});
    // Preserve kind0 through the established native packed-four-bit decoder.
    // Each level retains original palette alpha and row order; this does not
    // reinterpret packed nibbles as kind1 byte indexes or normalize pixels.
    if(t.kind==0){
     item["native_mips"]=J::array();
     for(std::size_t level=0;level<t.mips.size();++level){
      const auto& mip=t.mips[level];Bytes encoded;
      need(a::encodeTextureLevel48E(mip,encoded,e),"Native kind0 mip encode: "+e.code+" "+e.detail);
      a::Texture48B check;std::string why;
      need(a::parseTexture48B(encoded.data(),encoded.size(),check,why)&&check.width==mip.width&&
       check.height==mip.height&&check.rgba==mip.rgba,"Native kind0 mip round trip: "+why);
      if(level+1==t.mips.size())need(encoded==t.base_rrtex,"Native kind0 base mip identity");
      const auto filename=stem+"-mip"+std::to_string(level)+".rrtex";write(out/filename,encoded);
      item["native_mips"].push_back({{"index",level},{"width",mip.width},{"height",mip.height},
       {"source_header_offset",mip.source_header_offset},{"source_indices_offset",mip.source_indices_offset},{"rrtex",filename}});
     }
    }
   }else item.update({{"status","unsupported_texture"},{"error",e.code},{"detail",e.detail}});
  }else if(ext==".psm"){a::Material48E m;if(a::decodeMaterial48E(b,m,e)){item.update({{"status","converted_material"},{"general",fields(m.general)},{"passes",J::array()}});for(const auto& p:m.passes){J pass={{"pass",p.pass},{"program",p.microprogram},{"texture_count_defaulted",p.texture_count_defaulted},{"fields",fields(p.fields)},{"textures",J::array()}};for(const auto& t:p.textures)pass["textures"].push_back({{"slot",t.slot},{"name",t.name}});item["passes"].push_back(pass);}}else item.update({{"status","unsupported_material"},{"error",e.code},{"detail",e.detail}});
  }else if(ext==".phy"){rr::game::AuthoredParameters49N p{};a::PodPhysicsDiagnostic49N d{};const bool ok=a::readPodPhysics49N(reinterpret_cast<const char*>(b.data()),b.size(),p,d);item["native_physics_49n_accepted"]=ok;if(!ok)item["native_physics_49n_error"]={{"code",static_cast<unsigned>(d.code)},{"offset",d.byteOffset}};item["status"]="original_source_retained";
  }else item["status"]="original_source_retained";
  manifest["members"].push_back(item);
 }
 writeJ(out/"import.json",manifest);std::cout<<manifest.dump()<<'\n';return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
