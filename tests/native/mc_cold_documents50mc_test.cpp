// Draft parent-owned cold checker. Not compiled or run by preparation worker.
#include "mc_cold_documents50mc.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iterator>
namespace c=mc_cold_documents50MC;
static void need(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
static std::vector<std::uint8_t> read(const char* path){
 std::ifstream f(path,std::ios::binary);need(bool(f),"open original MC.RES");
 return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
static const c::a::ParameterBlock48F& block(const c::a::ParameterDocument48F& p,const char* name){
 for(const auto& b:p.blocks)if(b.name==name)return b;throw std::runtime_error(name);
}
int main(int argc,char** argv){try{
 need(argc==2,"usage: mc-cold-documents ORIGINAL-MC.RES");
 constexpr std::size_t budget=32u*1024u*1024u;
 c::g::SourceLibmState49T libm; c::Diagnostic d;
 for(unsigned mode=0;mode!=3;++mode){
  auto bytes=read(argv[1]);c::Owner rejected;
  if(mode==2)bytes.back()^=1;
  need(!rejected.decodeOwned(std::move(bytes),libm,mode==0?0:mode==1?budget+1:budget,d),"negative identity/budget accepted");
  need(!rejected.documentsReady()&&!rejected.gameplayReady()&&!d.mapGatesCommitted&&!d.aiGatesCommitted,"negative prepared data");
 }
 c::Owner source;auto bytes=read(argv[1]);
 if(!source.decodeOwned(std::move(bytes),libm,budget,d)){
  std::cerr<<"phase="<<int(d.phase)<<" message="<<d.message<<"\n";return 2;
 }
 need(source.documentsReady()&&!source.gameplayReady(),"source-only readiness");
 need(source.scb().preservesDuplicateLabels&&!source.tob().preservesDuplicateLabels,"distinct label policies");
 const auto& environment=block(source.scb(),"environment");
 unsigned colors=0,types=0;for(const auto& f:environment.fields){
  if(f.name=="type"){need((types==0&&f.value=="Ambient"&&f.entry_offset==14842)||(types==1&&f.value=="Directional"&&f.entry_offset==14885),"lost environment type provenance");++types;}
  if(f.name=="colorrgb"){need((colors==0&&f.value=="63,54,78"&&f.entry_offset==14068)||(colors==1&&f.value=="191,172,183"&&f.entry_offset==14111),"lost environment color provenance");++colors;}
 }
 need(colors==2&&types==2,"environment duplicate count");
 unsigned missingUp=0,missingLighting=0,models=0;
 for(const auto& b:source.scb().blocks){
  if(b.name.compare(0,4,"zone")||b.name.find("-model")==std::string::npos)continue;
  ++models;bool up=false,lighting=false;for(const auto& f:b.fields){up|=f.name=="upvector";lighting|=f.name=="uselighting";}
  missingUp+=!up;missingLighting+=!lighting;
 }
 need(models==458&&missingUp==32&&missingLighting==43,"missing placement fields fabricated or changed");
 const auto* n=source.coldNavigation();need(n&&n->map.track.count==173&&n->ai.track.count==173,"complete cold navigation");
 need(n->map.gatesPrepared&&n->ai.gatesPrepared&&!n->map.indicesPrepared&&!n->ai.indicesPrepared,"unearned runtime indices");
 need(n->mapMetadata.bound&&n->aiMetadata.bound,"full navigation metadata");
 c::a::World48F forbidden;c::a::DecodeError48C error;
 need(!c::a::decodeWorld48F(source.scb().original_bytes,forbidden,error),"strict World guard weakened");
 std::vector<std::uint8_t> empty;need(!source.decodeOwned(std::move(empty),libm,budget,d)&&source.documentsReady(),"one-attempt invariant");
 std::cout<<"MC cold documents and unindexed gates only; no gameplay admission\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
