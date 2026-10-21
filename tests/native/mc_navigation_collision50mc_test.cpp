// Parent-owned draft checker: NOT compiled or executed by preparation worker.
#include "mc_navigation50mc.h"
#include "mc_collision_sources50mc.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
namespace c=mc_cold_sources50LA;
static void need(bool v,const char* why){if(!v)throw std::runtime_error(why);}
static std::vector<std::uint8_t> read(const char* path){std::ifstream f(path,std::ios::binary);need(bool(f),"open MC.RES");return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};}
int main(int argc,char** argv){try{
 need(argc==2,"usage: mc-navigation-collision ORIGINAL-MC.RES");constexpr std::size_t budget=32u*1024u*1024u;
 c::g::SourceLibmState49T libm;c::Diagnostic d;
 auto source=std::make_shared<c::Owner>();
 if(!source->decodeOwned(read(argv[1]),libm,budget,d)){std::cerr<<"phase="<<int(d.phase)<<" "<<d.message<<"\n";return 2;}
 need(source->ready()&&!source->gameplayReady(),"cold source only");
 need(source->world().zones.size()==28&&source->world().portals.size()==28&&source->world().instances.size()==458,"complete World");
 {auto map=source->map(),ai=source->ai();
  need(map.gatesPrepared()&&ai.gatesPrepared()&&map.count()==173&&ai.count()==173,"both complete gates");
  need(map.role()==c::g::NavigationRole50MC::Map&&ai.role()==c::g::NavigationRole50MC::Ai,"distinct role types");
  need(map.nodes()!=ai.nodes()&&map.track().nodes()!=ai.track().nodes(),"Map/AI distinct storage");
  need(!map.runtimeValid()&&!ai.runtimeValid()&&!map.nodeIndex78()&&!ai.nodeIndex78(),"no cold runtime upgrade");
  need(source->mapMetadata().bound()&&source->aiMetadata().bound(),"complete metadata");
  need(!source->close(),"source closed with view borrows");
 }
 c::a::World48F forbidden;c::a::DecodeError48C error;
 need(!c::a::decodeWorld48F(source->scb().original_bytes,forbidden,error),"strict shared World weakened");
 mc_navigation50MC::Owner navigation;mc_navigation50MC::Diagnostic nd;
 need(navigation.prepare(source,nd)&&!navigation.runtimeReady(),"navigation staged without authority");
 need(!navigation.bindWorld(c::g::McCollisionWorldLease{},nd)&&!navigation.runtimeReady(),"empty lease admitted");
 need(!navigation.map().gatesPrepared()&&navigation.close(),"failed final issuer published view");
 auto collision=std::make_shared<mc_collision_sources50MC::Owner>();mc_collision_sources50MC::Diagnostic cd;
 if(!collision->prepare(source,budget,cd)){std::cerr<<"mesh phase="<<int(cd.phase)<<" row="<<cd.placement<<" "<<cd.message<<"\n";return 3;}
 need(collision->meshesReady()&&!collision->placementsReady()&&!collision->gameplayReady(),"mesh-only publication");
 {auto borrow=collision->borrow();need(bool(borrow)&&borrow.sourceIdentity()==source.get(),"same-source collision borrow");
  for(std::size_t i=0;i<89;++i)need(borrow.mesh(i)&&!borrow.collider(i),"mesh exists without fabricated placement");
  need(!borrow.mesh(89)&&!collision->close()&&!source->close(),"borrow/close fence");
 }
 const auto retained=cd.retainedBytes,peak=cd.maximumReservation;
 need(collision->close(),"collision fenced teardown");
 {
  c::a::OriginalStandardArchiveProvider50LA provider;c::a::OriginalStandardArchiveProvider50LA::Diagnostic pd;
  need(!source->mountCanonicalArchive(provider,0,pd)&&provider.mountCount()==0&&provider.revision()==0,"mount budget reject mutated provider");
  need(source->mountCanonicalArchive(provider,source->canonicalMountReservationBytes(),pd)&&provider.mountCount()==1&&!provider.sealed(),"canonical source mount");
  need(provider.sealNativeMounts(pd),"seal canonical namespace");const auto revision=provider.revision();
  need(source->mountCanonicalArchive(provider,0,pd)&&provider.revision()==revision&&provider.sealed(),"same-source reuse changed seal/revision");
  std::shared_ptr<const c::Owner> reused;need(c::Owner::reuseCanonicalSource(provider,reused,pd)&&reused.get()==source.get(),"canonical source retagged/copied");
  need(!source->close(),"source retired while canonical mount retained");
  c::a::OriginalStandardArchiveProvider50LA::MemberView member;
  need(provider.lookup132820("tracks\\mc.scb",member)==c::a::OriginalStandardArchiveProvider50LA::Lookup::present&&member.size==187711,"MC canonical member lookup");
 }
 need(source->close(),"source fenced teardown");
 std::cout<<"MC173 Map +173 Ai cold views; all89 COL0 meshes; no runtime aggregate authority. mesh-retained="<<retained<<" reservation="<<peak<<"\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
