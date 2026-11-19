// Host-only static check (no game, no window) for helper 145g:
//  1. every stock TRACKS\*.RES through the common track-animation reader/source/runtime:
//     per course table count and per row mcf / zone / collision / start rule against
//     work\scratch\track-data-gaps\anim\anim_table.txt (ANIM.md table b);
//  2. TB / TC / GB / MC: the retired per-course owners (copies in old\) against the common
//     runtime, node world matrices of every row at 12 times over 6.4 s with the triggered
//     rows fired at 1 s, plus the shared RNG word after every step (one draw per tick);
//  3. the common visual sphere (track_animation_sphere50an.h) against the retired TB/GB/MC
//     sphere leaves at every compared time.
// Usage: pose_check.exe <TRACKS dir> <anim_table.txt>
#include "../../../../tests/native/track_animation_runtime50an.h"
#include "../../../../tests/native/track_animation_sphere50an.h"
#include "../../../../src/assets/import_decode48c.h"
#define private public
#define protected public
#include "old/tb_door_runtime50tb.h"
#include "old/tb_door_sphere50tb.h"
#include "old/tc_gate_runtime50tc.h"
#include "old/gb_animation_runtime50gb.h"
#include "old/gb_animation_sphere50gb.h"
#include "old/mc_animation_runtime50mc.h"
#include "old/mc_animation_sphere50mc.h"
#undef private
#undef protected
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>

namespace a=rr::assets;namespace g=rr::game;namespace rt=track_animation_runtime50AN;namespace srcns=track_animation_source50AN;
static std::vector<std::uint8_t> readFile(const std::string& p){std::ifstream f(p,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
struct Expected{std::string mcf;int zone=0;bool collision=false;std::string start;};
static std::map<std::string,std::vector<Expected>> parseTable(const std::string& path){
 std::map<std::string,std::vector<Expected>> out;std::ifstream f(path);std::string line,code;
 const std::regex head(R"(^== (\w\w) )"),row(R"(^\s+#(\d+) KeyFramed mcf=(\S+) zone=(\d+) \S+ coll=(\d) .*\| start=(trigger T(\d+)|race start|never))");
 std::smatch m;
 while(std::getline(f,line)){
  if(std::regex_search(line,m,head)){code=m[1];out[code];continue;}
  if(std::regex_search(line,m,row)){Expected e;e.mcf=m[2];e.zone=std::stoi(m[3]);e.collision=m[4]=="1";
   const std::string s=m[5];e.start=s.rfind("trigger",0)==0?"trigger"+std::string(m[6]):s=="race start"?"race_start":"never";out[code].push_back(e);}
 }
 return out;
}
static int failures=0;
struct NewCourse {
 a::ResArchive48C archive;a::ParameterDocument48F doc;std::shared_ptr<srcns::Owner> source;std::unique_ptr<rt::Owner> runtime;
 g::SourceLibmState49T libm{};g::ModelMotionSharedState49T shared{};g::SharedRandomOwner50ID random{};std::uint32_t raw=0;
 bool load(const std::string& path,const char* code){
  const auto bytes=readFile(path);a::DecodeError48C error{};
  if(!a::decodeResource48C(bytes,archive,error))return false;
  std::vector<std::uint8_t> tob;std::string lower=code;for(auto& c:lower)c=char(c-'A'+'a');
  if(!a::extractResMember48C(archive,"tracks\\"+lower+".tob",tob,error))return false;
  a::WorldLimits48F wl{};wl.retainDuplicateLabels=true;if(!a::decodeParameters48F(tob,doc,error,nullptr,wl))return false;
  const srcns::Extract extract=[this](const std::string& name,std::vector<std::uint8_t>& out,std::size_t limit){a::DecodeError48C e{};return a::extractResMember48C(archive,name,out,e,limit);};
  source=std::make_shared<srcns::Owner>();srcns::Diagnostic sd{};
  if(!source->load(doc,0,extract,libm,sd)){std::cout<<code<<" FAIL source: "<<sd.message<<'\n';return false;}
  if(!source->rowCount())return true;
  runtime=std::make_unique<rt::Owner>();rt::Diagnostic d{};
  if(!runtime->create(source,shared,code,d)){std::cout<<code<<" FAIL create: "<<d.message<<'\n';return false;}
  return true;
 }
 void fireTriggers(){for(const auto& x:source->table().actions)if(x.kind==a::TrackAnimationActionKind50AN::animation)runtime->fire(x.target,x.trigger);}
 bool tick(std::uint32_t timestamp){rt::Diagnostic d{};
  if(!runtime->tick(.016f,false,timestamp,raw,random,shared,d)||!runtime->prepareDraw(d)){std::cout<<"new tick FAIL "<<d.message<<'\n';return false;}return true;}
 std::vector<g::BodyMatrix48G> worlds(std::size_t row){auto nodes=runtime->state(row).nodes;std::vector<g::BodyMatrix48G> out;
  const g::SceneGraph49P graph{nodes.data(),static_cast<std::uint32_t>(nodes.size())};
  for(std::uint32_t i=0;i<nodes.size();++i){g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(i));out.push_back(nodes[i].world100);}return out;}
 // The binding's visual sphere for one row (common leaf).
 bool sphere(std::size_t row,track_animation_sphere50AN::Result& out){
  auto nodes=runtime->state(row).nodes;const g::SceneGraph49P graph{nodes.data(),static_cast<std::uint32_t>(nodes.size())};
  const auto& h=source->row(row).hierarchy;std::vector<track_animation_sphere50AN::Row> rows(h.nodeCount);
  g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(0));
  for(std::uint32_t r=0;r<h.nodeCount;++r){a::PsgNode49O o{};a::readPsgNode49O(h,r,o);g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(r+1));
   rows[r].world=nodes[r+1].world100;for(unsigned k=0;k<3;++k)rows[r].center[k]=o.bounds[k];rows[r].radius=o.bounds[6];}
  return track_animation_sphere50AN::compute(nodes[0].world100,rows.data(),rows.size(),out);
 }
};
static float matrixDiff(const g::BodyMatrix48G& x,const g::BodyMatrix48G& y){float m=0;for(int k=0;k<16;++k)m=std::max(m,std::fabs(x.v[k]-y.v[k]));return m;}
template<class Nodes>static float nodesDiff(Nodes nodes,std::size_t count,const std::vector<g::BodyMatrix48G>& fresh){
 const g::SceneGraph49P graph{nodes.data(),static_cast<std::uint32_t>(count)};float m=0;
 if(fresh.size()!=count)return 1e30f;
 for(std::uint32_t i=0;i<count;++i){g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(i));m=std::max(m,matrixDiff(nodes[i].world100,fresh[i]));}
 return m;
}
static const std::vector<int> checkSteps{1,30,61,62,63,80,100,150,200,250,300,400};
static void report(const char* code,float worst,std::uint32_t rawOld,std::uint32_t rawNew,float sphereWorst){
 if(rawOld!=rawNew){std::cout<<code<<" shared RNG differs old="<<rawOld<<" new="<<rawNew<<'\n';++failures;}
 else std::cout<<code<<" shared RNG identical after 400 ticks ("<<rawNew<<")\n";
 if(worst!=0.f){std::cout<<code<<" POSE DIFFERS (worst "<<worst<<")\n";++failures;}else std::cout<<code<<" node worlds identical to the old owner at every compared time\n";
 if(sphereWorst>=0.f){if(sphereWorst!=0.f){std::cout<<code<<" VISUAL SPHERE DIFFERS (worst "<<sphereWorst<<")\n";++failures;}else std::cout<<code<<" visual spheres identical to the old leaf\n";}
}
static constexpr std::uint32_t timestamp=0x1234567u;
static constexpr std::size_t budget=48u*1024u*1024u;

static void compareTB(const std::string& dir){
 const std::string path=dir+"\\TB.RES";NewCourse n;if(!n.load(path,"TB")||!n.runtime){++failures;return;}
 g::SourceLibmState49T libm{};g::ModelMotionSharedState49T shared{};g::SharedRandomOwner50ID random{};std::uint32_t raw=0;
 auto cold=std::make_shared<tb_cold_sources50LA::Owner>();tb_cold_sources50LA::Diagnostic cd{};
 if(!cold->decodeOwned(readFile(path),libm,budget,cd)){std::cout<<"TB old cold FAIL "<<cd.message<<'\n';++failures;return;}
 auto src=std::make_shared<tb_door_motion50TB::SourceOwner>();tb_door_motion50TB::Diagnostic sd{};
 if(!src->decode(cold,libm,tb_door_motion50TB::SourceOwner::maximumAdditionalPayloadBytes(),sd)){std::cout<<"TB old source FAIL "<<sd.message<<'\n';++failures;return;}
 tb_door_runtime50TB::Owner old;tb_door_runtime50TB::Diagnostic d{};
 if(!old.beginPrepare(src,shared,tb_door_runtime50TB::Owner::requiredPayloadBytes(),d)){std::cout<<"TB old prepare FAIL "<<d.message<<'\n';++failures;return;}
 old.binding_=reinterpret_cast<const tb_door_binding50TB::Binding*>(&old); // host check: stand-in issuer
 if(!old.prepareCollisionAfterVisual(d)){std::cout<<"TB old collision FAIL "<<d.message<<'\n';++failures;old.binding_=nullptr;return;}
 old.collisionState_.registrationE0=1;
 if(!old.completeInitialSample(shared,d)){std::cout<<"TB old sample FAIL "<<d.message<<'\n';++failures;old.binding_=nullptr;return;}
 old.publicationReady_=true;old.posePending_=false;
 float worst=nodesDiff(old.nodes_,old.nodes_.size(),n.worlds(0)),sphereWorst=0;
 for(int step=1;step<=checkSteps.back();++step){
  if(step==62){old.activateSequence(0);n.fireTriggers();}
  if(!old.service(.016f,timestamp,raw,random,shared,d)){std::cout<<"TB old service FAIL "<<d.message<<'\n';++failures;break;}old.posePending_=false;
  if(!n.tick(timestamp)){++failures;break;}
  if(std::find(checkSteps.begin(),checkSteps.end(),step)==checkSteps.end())continue;
  const float m=nodesDiff(old.nodes_,old.nodes_.size(),n.worlds(0));worst=std::max(worst,m);
  // old visual sphere leaf vs the common leaf
  auto nodes=old.nodes_;const g::SceneGraph49P graph{nodes.data(),static_cast<std::uint32_t>(nodes.size())};std::array<tb_door_sphere50TB::Row,3> rows{};
  for(std::uint32_t i=0;i<4;++i)g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(i));
  for(std::uint32_t i=0;i<3;++i){a::PsgNode49O o{};a::readPsgNode49O(old.source_->hierarchy(),i,o);rows[i].world=nodes[i+1].world100;for(unsigned k=0;k<3;++k)rows[i].center[k]=o.bounds[k];rows[i].radius=o.bounds[6];}
  tb_door_sphere50TB::Result os{};track_animation_sphere50AN::Result ns{};
  if(!tb_door_sphere50TB::compute(nodes[0].world100,rows,os)||!n.sphere(0,ns)){std::cout<<"TB sphere FAIL\n";++failures;}
  else{float s=std::fabs(os.radius198-ns.radius198);for(int k=0;k<3;++k)s=std::max(s,std::fabs(os.center160[k]-ns.center160[k]));sphereWorst=std::max(sphereWorst,s);}
  std::cout<<"TB pose compare at step "<<step<<" ("<<step*0.016f<<" s): max |dworld| = "<<m<<" old stopped="<<old.sequence_.stopped10<<" new stopped="<<n.runtime->state(0).stopped10<<'\n';
 }
 old.binding_=nullptr;report("TB",worst,raw,n.raw,sphereWorst);
}
static void compareTC(const std::string& dir){
 const std::string path=dir+"\\TC.RES";NewCourse n;if(!n.load(path,"TC")||!n.runtime){++failures;return;}
 g::SourceLibmState49T libm{};g::ModelMotionSharedState49T shared{};g::SharedRandomOwner50ID random{};std::uint32_t raw=0;
 auto cold=std::make_shared<tc_cold_sources50LA::Owner>();tc_cold_sources50LA::Diagnostic cd{};
 if(!cold->decode(readFile(path),libm,budget,cd)){std::cout<<"TC old cold FAIL "<<cd.message<<'\n';++failures;return;}
 auto src=std::make_shared<tc_gate_motion50TC::SourceOwner>();tc_gate_motion50TC::Diagnostic sd{};
 if(!src->decode(cold,sd)){std::cout<<"TC old source FAIL "<<sd.message<<'\n';++failures;return;}
 tc_gate_runtime50TC::Owner old;tc_gate_runtime50TC::Diagnostic d{};
 if(!old.beginPrepare(src,shared,budget,d)){std::cout<<"TC old prepare FAIL "<<d.message<<'\n';++failures;return;}
 if(!old.completeInitialSample(shared,d)){std::cout<<"TC old sample FAIL "<<d.message<<'\n';++failures;return;}
 float worst=nodesDiff(old.nodes_,old.nodes_.size(),n.worlds(0));
 for(int step=1;step<=checkSteps.back();++step){
  if(step==62){old.activateSequence(0);n.fireTriggers();}
  if(!old.service(.016f,timestamp,raw,random,shared,d)){std::cout<<"TC old service FAIL "<<d.message<<'\n';++failures;break;}
  if(!n.tick(timestamp)){++failures;break;}
  if(std::find(checkSteps.begin(),checkSteps.end(),step)==checkSteps.end())continue;
  const float m=nodesDiff(old.nodes_,old.nodes_.size(),n.worlds(0));worst=std::max(worst,m);
  std::cout<<"TC pose compare at step "<<step<<" ("<<step*0.016f<<" s): max |dworld| = "<<m<<'\n';
 }
 report("TC",worst,raw,n.raw,-1.f);
}
template<class T>
static void compareMulti(const std::string& dir,const char* code,std::vector<std::size_t> triggered){
 const std::string path=dir+"\\"+code+".RES";NewCourse n;if(!n.load(path,code)||!n.runtime){++failures;return;}
 g::SourceLibmState49T libm{};g::ModelMotionSharedState49T shared{};g::SharedRandomOwner50ID random{};std::uint32_t raw=0;
 using Cold=typename T::Cold;using Source=typename T::Source;using Runtime=typename T::Runtime;using Sphere=typename T::Sphere;
 auto cold=std::make_shared<Cold>();typename T::ColdDiagnostic cd{};
 if(!cold->decodeOwned(readFile(path),libm,std::min<std::size_t>(budget,32u*1024u*1024u),cd)){std::cout<<code<<" old cold FAIL "<<cd.message<<'\n';++failures;return;}
 auto src=std::make_shared<Source>();typename T::SourceDiagnostic sd{};bool decoded=false;
 if constexpr(requires{src->decode(cold,libm,Source::maximumAdditionalPayloadBytes(),sd);})decoded=src->decode(cold,libm,Source::maximumAdditionalPayloadBytes(),sd);
 else decoded=src->decode(cold,Source::maximumAdditionalPayloadBytes(),sd);
 if(!decoded){std::cout<<code<<" old source FAIL "<<sd.message<<'\n';++failures;return;}
 Runtime old;typename T::RuntimeDiagnostic d{};
 if(!old.beginPrepare(src,shared,Runtime::requiredPayloadBytes(),d)){std::cout<<code<<" old prepare FAIL "<<d.message<<'\n';++failures;return;}
 old.binding_=reinterpret_cast<decltype(old.binding_)>(&old);
 const auto rows=old.state_.size();
 if(rows!=n.runtime->rowCount()){std::cout<<code<<" ROW COUNT DIFFERS old="<<rows<<" new="<<n.runtime->rowCount()<<'\n';++failures;old.binding_=nullptr;return;}
 for(std::size_t i=0;i<rows;++i){old.state_[i].visualPrepared=true;
  if(!old.prepareCollisionAfterVisual(i,d)||!old.initialSample(i,shared,d)){std::cout<<code<<" old row "<<i<<" initial FAIL "<<d.message<<'\n';++failures;old.binding_=nullptr;return;}}
 old.publicationReady_=true;
 auto diffAll=[&](){float m=0;for(std::size_t i=0;i<rows;++i)m=std::max(m,nodesDiff(old.state_[i].nodes,old.source(i).graphCount,n.worlds(i)));return m;};
 float worst=diffAll(),sphereWorst=0;
 for(int step=1;step<=checkSteps.back();++step){
  if(step==62){for(auto i:triggered)old.activateSequence(i);n.fireTriggers();}
  if(!old.serviceRandom(timestamp,raw,random,d)){++failures;break;}
  for(std::size_t i=0;i<rows;++i){bool entered=false;if(!old.advance(i,.016f,shared,entered,d)){std::cout<<code<<" old advance FAIL "<<d.message<<'\n';++failures;break;}}
  if(!n.tick(timestamp)){++failures;break;}
  if(std::find(checkSteps.begin(),checkSteps.end(),step)==checkSteps.end())continue;
  const float m=diffAll();worst=std::max(worst,m);
  for(std::size_t i=0;i<rows;++i){
   auto nodes=old.state_[i].nodes;const auto count=old.source(i).graphCount;const g::SceneGraph49P graph{nodes.data(),static_cast<std::uint32_t>(count)};
   for(std::uint32_t k=0;k<count;++k)g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(k));
   const auto& h=old.source(i).hierarchy;std::vector<typename Sphere::Row> srows(h.nodeCount);
   for(std::uint32_t r=0;r<h.nodeCount;++r){a::PsgNode49O o{};a::readPsgNode49O(h,r,o);srows[r].world=nodes[r+1].world100;for(unsigned k=0;k<3;++k)srows[r].center[k]=o.bounds[k];srows[r].radius=o.bounds[6];}
   typename Sphere::Result os{};track_animation_sphere50AN::Result ns{};
   if(!Sphere::compute(nodes[0].world100,srows.data(),srows.size(),os)||!n.sphere(i,ns)){std::cout<<code<<" sphere FAIL row "<<i<<'\n';++failures;continue;}
   float s=std::fabs(os.radius198-ns.radius198);for(int k=0;k<3;++k)s=std::max(s,std::fabs(os.center160[k]-ns.center160[k]));sphereWorst=std::max(sphereWorst,s);
  }
  std::cout<<code<<" pose compare at step "<<step<<" ("<<step*0.016f<<" s): max |dworld| = "<<m<<'\n';
 }
 old.binding_=nullptr;report(code,worst,raw,n.raw,sphereWorst);
}
struct GbSphere{using Row=gb_animation_sphere50GB::Row;using Result=gb_animation_sphere50GB::Result;
 static bool compute(const g::BodyMatrix48G& r,const Row* rows,std::size_t n,Result& o){return gb_animation_sphere50GB::compute(r,rows,n,o);}};
struct McSphere{using Row=mc_animation_sphere50MC::Row;using Result=mc_animation_sphere50MC::Result;
 static bool compute(const g::BodyMatrix48G& r,const Row* rows,std::size_t n,Result& o){return mc_animation_sphere50MC::compute(r,rows,n,o);}};

struct GbTraits{using Cold=gb_cold_sources50LA::Owner;using ColdDiagnostic=gb_cold_sources50LA::Diagnostic;using Source=gb_animation_source50GB::Owner;
 using SourceDiagnostic=gb_animation_source50GB::Diagnostic;using Runtime=gb_animation_runtime50GB::Owner;using RuntimeDiagnostic=gb_animation_runtime50GB::Diagnostic;using Sphere=GbSphere;};
struct McTraits{using Cold=mc_cold_sources50LA::Owner;using ColdDiagnostic=mc_cold_sources50LA::Diagnostic;using Source=mc_animation_source50MC::Owner;
 using SourceDiagnostic=mc_animation_source50MC::Diagnostic;using Runtime=mc_animation_runtime50MC::Owner;using RuntimeDiagnostic=mc_animation_runtime50MC::Diagnostic;using Sphere=McSphere;};
int main(int argc,char** argv){
 if(argc<3){std::cerr<<"usage: pose_check <TRACKS dir> <anim_table.txt>\n";return 2;}
 const std::string dir=argv[1];const auto expected=parseTable(argv[2]);
 const char* codes[]={"TA","TB","TC","SA","SC","MA","MB","MC","GA","GB","GC","RA","RC"};
 for(const char* code:codes){
  NewCourse n;if(!n.load(dir+"\\"+code+".RES",code)){std::cout<<code<<" FAIL load\n";++failures;continue;}
  const auto& exp=expected.at(code);
  if(!n.runtime){std::cout<<code<<" table=0 -> no owner (absent)"<<(exp.empty()?" OK":" MISMATCH")<<'\n';if(!exp.empty())++failures;continue;}
  n.runtime->logTable(std::cout);
  {std::size_t colored=0,surfaces=0;for(std::size_t i=0;i<n.source->rowCount();++i)for(const auto& s:n.source->row(i).scene.lods[0].surfaces){++surfaces;colored+=s.has_original_colors?1:0;}
   std::cout<<code<<" surfaces="<<surfaces<<" with_source_vertex_colours="<<colored<<std::endl;}
  bool ok=n.source->rowCount()==exp.size();
  for(std::size_t i=0;i<n.source->rowCount()&&i<exp.size();++i){const auto& r=n.source->row(i);const auto& s=n.runtime->state(i);
   std::string start="never";for(const auto& x:n.source->table().actions)if(x.kind==a::TrackAnimationActionKind50AN::animation&&x.target==i){start="trigger"+std::to_string(x.trigger);break;}
   if(start=="never"&&!s.active90.empty()&&s.active90[0])start="race_start";
   const bool rowOk=srcns::detail::iequal(r.authored.mcfStem,exp[i].mcf)&&int(r.authored.startingZone+1)==exp[i].zone&&s.hasCollision==exp[i].collision&&start==exp[i].start;
   if(!rowOk){std::cout<<"  MISMATCH row "<<(i+1)<<'\n';ok=false;}
  }
  std::cout<<code<<" table compare "<<(ok?"OK":"MISMATCH")<<'\n';if(!ok)++failures;
 }
 compareTB(dir);
 compareTC(dir);
 compareMulti<GbTraits>(dir,"GB",{0,2});
 compareMulti<McTraits>(dir,"MC",{0});
 std::cout<<"failures="<<failures<<'\n';return failures?1:0;
}
