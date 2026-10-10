// Cold original-RES importer. No guest execution, recoloring, or mesh substitution.
#include "assets/import_decode48c.h"
#include "assets/import_scene48d.h"
#include "assets/import_texture48e.h"
#include "assets/scene_asset48d.h"
#include "assets/native_asset48b.h"
#include "assets/pod_physics49n.h"
#include "native-json/json.hpp"
#include <algorithm>
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
static std::string normal(std::string s){for(auto& c:s){if(c=='\\')c='/';if(c>='A'&&c<='Z')c+=32;}return s;}
static Bytes read(const fs::path& p){std::ifstream f(p,std::ios::binary|std::ios::ate);need(bool(f),"Cannot read "+p.string());auto n=f.tellg();need(n>0&&n<=64*1024*1024,"Resource outside bounded size");Bytes b(static_cast<std::size_t>(n));f.seekg(0);f.read(reinterpret_cast<char*>(b.data()),b.size());need(bool(f),"Short resource read");return b;}
static void write(const fs::path& p,const Bytes& b){need(!fs::exists(p),"Refusing overwrite "+p.string());std::ofstream f(p,std::ios::binary);f.write(reinterpret_cast<const char*>(b.data()),b.size());f.close();need(bool(f),"Cannot write "+p.string());}
static void writeJ(const fs::path& p,const J& j){auto s=j.dump(2)+"\n";write(p,Bytes(s.begin(),s.end()));}
static J fields(const a::MaterialFields48E& f){J r={{"authored",J::array()},{"unspecified",J::array()}};for(auto p:{&f.authored,&f.unspecified})for(auto& x:*p)r[p==&f.authored?"authored":"unspecified"].push_back({{"name",x.name},{"value",x.value},{"source_offset",x.source_offset}});return r;}
int main(int argc,char** argv){try{
 need(argc==3,"Usage: import_original_pods PODxx.RES NEW_OUTPUT_DIRECTORY");const fs::path input=argv[1],out=argv[2];need(!fs::exists(out),"Output directory must be new");
 a::ResArchive48C res;a::DecodeError48C e;const auto raw=read(input);need(a::decodeResource48C(raw,res,e),"RES decode: "+e.code+" "+e.detail);fs::create_directories(out);
 J manifest={{"schema","RR_ORIGINAL_POD_IMPORT_V1"},{"source_container",input.filename().string()},{"source_container_bytes",raw.size()},{"members",J::array()}};
 for(std::size_t i=0;i<res.entries.size();++i){const auto& r=res.entries[i];const auto ext=fs::path(normal(r.name)).extension().string();
  if(ext!=".psg"&&ext!=".psm"&&ext!=".pst"&&ext!=".phy"&&ext!=".slt"&&ext!=".mcf"&&ext!=".mot"&&ext!=".vut"&&ext!=".aub")continue;
  Bytes b;need(a::extractResMember48C(res,r.name,b,e),"Extract "+r.name+": "+e.code);const std::string stem=std::to_string(i);const auto source=stem+"-original"+ext;write(out/source,b);
  J item={{"source_member",r.name},{"source_index",i},{"logical_offset",r.logical_offset},{"source_bytes",r.bytes},{"source_file",source},{"kind",ext.substr(1)}};
  if(ext==".psg") {a::Scene48D scene;if(a::decodeScene48D(b,scene,e)){Bytes native;std::string error;need(a::encodeSceneAsset48D(scene,native,error),"Scene encode "+error);a::Scene48D check;need(a::parseSceneAsset48D(native,check,error)&&check.neutral_lod0==scene.neutral_lod0,"Scene round trip "+error);write(out/(stem+".rrscene"),native);write(out/(stem+".rrmesh"),scene.neutral_lod0);a::Mesh48B mesh;need(a::parseMesh48B(scene.neutral_lod0.data(),scene.neutral_lod0.size(),mesh,error),"Mesh roundtrip "+error);item.update({{"status","converted_geometry"},{"rrscene",stem+".rrscene"},{"rrmesh",stem+".rrmesh"},{"nodes",scene.nodes.size()},{"vertices",mesh.vertices.size()},{"indices",mesh.indices.size()},{"materials",scene.materials},{"lods",scene.lods.size()},{"world_space",scene.world_space},{"node_names",J::array()},{"spans",J::array()}});for(const auto& n:scene.nodes)item["node_names"].push_back(n.name);for(const auto& s:scene.spans_lod0)item["spans"].push_back({{"material_index",s.material_index},{"vertex_start",s.vertex_start},{"vertex_count",s.vertex_count},{"index_start",s.index_start},{"index_count",s.index_count}});
   item["bounds"]={{"minimum",{mesh.bounds.minimum[0],mesh.bounds.minimum[1],mesh.bounds.minimum[2]}},{"maximum",{mesh.bounds.maximum[0],mesh.bounds.maximum[1],mesh.bounds.maximum[2]}}};
  }else item.update({{"status","unsupported_geometry"},{"error",e.code},{"detail",e.detail}});
  }else if(ext==".pst"){a::Texture48E t;if(a::decodeTexture48E(b,t,e)){write(out/(stem+".rrtex"),t.base_rrtex);item.update({{"status","converted_texture"},{"rrtex",stem+".rrtex"},{"width",t.mips.back().width},{"height",t.mips.back().height},{"mips",t.mips.size()}});}else item.update({{"status","unsupported_texture"},{"error",e.code},{"detail",e.detail}});
  }else if(ext==".psm"){a::Material48E m;if(a::decodeMaterial48E(b,m,e)){item.update({{"status","converted_material"},{"general",fields(m.general)},{"passes",J::array()}});for(const auto& p:m.passes){J pass={{"pass",p.pass},{"program",p.microprogram},{"texture_count_defaulted",p.texture_count_defaulted},{"fields",fields(p.fields)},{"textures",J::array()}};for(const auto& t:p.textures)pass["textures"].push_back({{"slot",t.slot},{"name",t.name}});item["passes"].push_back(pass);}}else item.update({{"status","unsupported_material"},{"error",e.code},{"detail",e.detail}});
  }else if(ext==".phy"){rr::game::AuthoredParameters49N p{};a::PodPhysicsDiagnostic49N d{};const bool ok=a::readPodPhysics49N(reinterpret_cast<const char*>(b.data()),b.size(),p,d);item["native_physics_49n_accepted"]=ok;if(!ok)item["native_physics_49n_error"]={{"code",static_cast<unsigned>(d.code)},{"offset",d.byteOffset}};item["status"]="original_source_retained";
  }else item["status"]="original_source_retained";
  manifest["members"].push_back(item);
 }
 writeJ(out/"import.json",manifest);std::cout<<manifest.dump()<<'\n';return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
