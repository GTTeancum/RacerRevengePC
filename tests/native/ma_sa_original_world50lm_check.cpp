// Parent-operated cold check; author has not compiled/executed it.
#include "ma_sa_original_world50lm.h"
#include "sky50gw_fixture.h"
#include "original_track_sky50kx_fixture.h"
#include <fstream>
#include <iostream>
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
using Bytes=std::vector<std::uint8_t>;
void need(bool v,const std::string& why){if(!v)throw std::runtime_error(why);}
Bytes read(const std::filesystem::path& p){
 std::ifstream f(p,std::ios::binary|std::ios::ate);need(bool(f),"archive open");auto size=f.tellg();
 need(size>0&&size<=8*1024*1024,"archive bound");Bytes b(static_cast<std::size_t>(size));f.seekg(0);
 f.read(reinterpret_cast<char*>(b.data()),b.size());need(bool(f),"exact archive read");return b;
}
template<class Owner,class Diagnostic>
void check(const std::filesystem::path& root,const char* code,track_source_fixture::Track track,
 std::size_t decoded,std::size_t entries,std::size_t scbSize,std::size_t pins,std::size_t images){
 const auto path=root/"TRACKS"/(std::string(code)+".RES");auto source=read(path);
 a::ResArchive48C archive;a::DecodeError48C error;a::DecodeLimits48C limits;
 limits.resource_decoded_bytes=decoded;limits.resource_entries=entries;
 need(a::decodeResource48C(source,archive,error,nullptr,limits),"archive decode: "+error.code);
 Bytes scb;need(a::extractResMember48C(archive,"tracks\\"+std::string(code)+".scb",scb,error,scbSize),"SCB extract");
 a::World48F world;need(a::decodeWorld48F(scb,world,error),"SCB decode");
 // Mutable typed caches must be checked against their authenticated source.
 auto changed=world;need(!changed.instances.empty(),"world instance");changed.instances.front().position[0]+=1;
 Owner rejected;Diagnostic d;need(!rejected.load(source,changed,d)&&!rejected.loaded()&&!rejected.mesh(),"typed mutation rejected transactionally");
 auto corrupt=source;corrupt.back()^=1;
 need(!rejected.load(corrupt,world,d)&&!rejected.loaded(),"archive mutation rejected transactionally");
 Owner owner;const bool ok=owner.load(source,world,d);
 if(!ok)std::cerr<<code<<" stage="<<d.stage<<" member="<<d.member<<" reason="<<d.message<<'\n';
 need(ok,"original static owner load");need(d.independentlyPinnedImages==pins&&d.runtimeIdentityImages==images-pins,"honest image pin accounting");
 need(owner.mesh()&&owner.normals()&&owner.normals()->size()==owner.mesh()->vertices.size()&&owner.bank()&&owner.archive(),"complete source publication");
 for(const auto& s:*owner.selections())need(s.track==track&&s.pass==1&&s.passCount==1&&s.slot==1&&!s.decodedSourceMember.empty()&&!s.decodedSourceSha256.empty(),"source material selection");
 const auto* mesh=owner.mesh();need(!owner.load(source,world,d)&&owner.mesh()==mesh,"reload preserves owner");
 sky50gw_fixture::Request request;request.archive=path;request.track=track;request.decodeOriginalImages=true;
 sky50gw_fixture::Owner sky;sky50gw_fixture::Diagnostic sd;
 const bool skyOk=sky.load(request,sd);if(!skyOk)std::cerr<<sd.message<<'\n';need(skyOk,"original sky pixels");
 need(sky.mesh()->vertices.size()==192&&sky.mesh()->indices.size()==288&&sky.spans()->size()==48,"48 original sky tiles");
 original_track_sky50KX::Owner skySelection;std::string why;need(skySelection.prepare(world,track,why),"source sky enable: "+why);
 for(std::uint32_t z=0;z<world.zones.size();++z){bool enabled=false;need(skySelection.select(z,enabled),"source camera-zone sky selector");}
 std::cout<<code<<" original static owner, source mutation rejection and original sky PASS; "<<pins<<" independent static image pins / "<<images<<" images\n";
}
int main(int argc,char** argv){try{
 need(argc==2,"usage: check ORIGINAL_DISC_DIRECTORY");need(std::fegetround()==FE_TONEAREST,"rounding mode");
 check<ma_original_world50LM::Owner,ma_original_world50LM::Diagnostic>(argv[1],"MA",track_source_fixture::Track::ma,15532032,485,404992,49,80);
 check<common_route_world50LM::Owner,common_route_world50LM::Diagnostic>(argv[1],"SA",track_source_fixture::Track::sa,29638656,1065,813441,7,104);
 return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
