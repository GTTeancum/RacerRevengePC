// Host-only static check (no game, no window): runs the common track-animation reader, source
// owner and runtime tick over every stock TRACKS\*.RES for a few simulated seconds.
//  1. per course: table count, per row created / collision / start rule, compared with
//     work\scratch\track-data-gaps\anim\anim_table.txt (ANIM.md table b);
//  2. for courses whose old per-course owner can be called on the host (GC, SA, GA load from
//     the archive path), the drawn vertex positions at several times against that owner.
// Usage: anim_check.exe <TRACKS dir> <anim_table.txt>
#include "../../../../tests/native/track_animation_runtime50an.h"
#include "../../../../tests/native/track_animation_materials50an.h"
#include "old/gc_animated_world50ig_fixture.h"  // the retired per-course owner, kept here for the pose comparison
#include "old/sa_animated_world_fixture.h"  // the retired per-course owner, kept here for the pose comparison
#include "old/ga_animated_world_fixture.h"  // the retired per-course owner, kept here for the pose comparison
#include "../../../../src/assets/import_decode48c.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>

// Host check: no DLC overlay (stock paths unchanged).
namespace rr::assets::dlc50DL{std::filesystem::path host(const std::filesystem::path& p){return p;}}
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
   const std::string s=m[5];e.start=s.rfind("trigger",0)==0?"trigger"+std::string(m[6]):s=="race start"?"race_start":"never";
   out[code].push_back(e);}
 }
 return out;
}
static bool detail_lower_eq(const std::string& x,const std::string& y){return srcns::detail::iequal(x,y);}
static float maxDiff(const a::Mesh48B& x,const a::Mesh48B& y){
 if(x.vertices.size()!=y.vertices.size())return 1e30f;float m=0;
 for(std::size_t i=0;i<x.vertices.size();++i)for(int k=0;k<3;++k)m=std::max(m,std::fabs(x.vertices[i].position[k]-y.vertices[i].position[k]));
 return m;
}
int main(int argc,char** argv){
 if(argc<3){std::cerr<<"usage: anim_check <TRACKS dir> <anim_table.txt>\n";return 2;}
 const std::string dir=argv[1];const auto expected=parseTable(argv[2]);
 const char* codes[]={"TA","TB","TC","SA","SC","MA","MB","MC","GA","GB","GC","RA","RC"};
 int failures=0,total=0;
 for(const char* code:codes){
  const std::string path=dir+"\\"+code+".RES";const auto bytes=readFile(path);
  a::ResArchive48C archive;a::DecodeError48C error{};
  if(!a::decodeResource48C(bytes,archive,error)){std::cout<<code<<" FAIL archive decode\n";++failures;continue;}
  std::vector<std::uint8_t> tob;std::string lower=code;for(auto& c:lower)c=char(c-'A'+'a');
  if(!a::extractResMember48C(archive,"tracks\\"+lower+".tob",tob,error)){std::cout<<code<<" FAIL tob\n";++failures;continue;}
  a::ParameterDocument48F doc;a::WorldLimits48F wl{};wl.retainDuplicateLabels=true;if(!a::decodeParameters48F(tob,doc,error,nullptr,wl)){std::cout<<code<<" FAIL tob decode\n";++failures;continue;}
  const srcns::Extract extract=[&](const std::string& name,std::vector<std::uint8_t>& out,std::size_t limit){
   a::DecodeError48C e{};return a::extractResMember48C(archive,name,out,e,limit);};
  // Fresh shared state per course (the session owns these).
  g::SourceLibmState49T libm{};g::ModelMotionSharedState49T shared{};g::SharedRandomOwner50ID random{};std::uint32_t raw=0;
  auto source=std::make_shared<srcns::Owner>();srcns::Diagnostic sd{};
  if(!source->load(doc,0,extract,libm,sd)){std::cout<<code<<" FAIL source: "<<sd.message<<" row="<<sd.row<<" member="<<sd.member<<'\n';++failures;continue;}
  const auto& exp=expected.at(code);
  if(!source->rowCount()){std::cout<<code<<" table=0 -> no owner (absent)"<<(exp.empty()?" OK":" MISMATCH")<<'\n';if(!exp.empty())++failures;continue;}
  rt::Owner runtime;rt::Diagnostic d{};
  if(!runtime.create(source,shared,code,d)){std::cout<<code<<" FAIL create: "<<d.message<<" row="<<d.row<<'\n';++failures;continue;}
  runtime.logTable(std::cout);
  {track_animation_materials50AN::Owner materials;track_animation_materials50AN::Diagnostic md{};
   if(!materials.load(runtime,extract,md)){std::cout<<code<<" MATERIALS FAIL: "<<md.message<<" span="<<md.span<<" member="<<md.member<<'\n';++failures;}
   else{std::cout<<code<<" materials="<<materials.materialCount()<<" textures="<<materials.textureCount()<<" spans="<<runtime.spans().size()<<'\n';
    for(std::size_t m=0;m<materials.materialCount();++m){const auto& x=materials.material(m);
     std::cout<<"   material "<<x.name<<" program="<<x.program<<" map="<<x.texture<<" fields=";for(const auto& f:x.fields.authored)std::cout<<f.name<<"="<<f.value<<";";std::cout<<'\n';}}}
  bool ok=source->rowCount()==exp.size();
  for(std::size_t i=0;i<source->rowCount()&&i<exp.size();++i){const auto& r=source->row(i);const auto& s=runtime.state(i);
   std::string start="never";for(const auto& x:source->table().actions)if(x.kind==a::TrackAnimationActionKind50AN::animation&&x.target==i){start="trigger"+std::to_string(x.trigger);break;}
   if(start=="never"&&!s.active90.empty()&&s.active90[0])start="race_start";
   const bool rowOk=detail_lower_eq(r.authored.mcfStem,exp[i].mcf)&&int(r.authored.startingZone+1)==exp[i].zone&&s.hasCollision==exp[i].collision&&start==exp[i].start;
   if(!rowOk){std::cout<<"  MISMATCH row "<<(i+1)<<" got mcf="<<r.authored.mcfStem<<" zone="<<(r.authored.startingZone+1)<<" coll="<<s.hasCollision<<" start="<<start
     <<" expected mcf="<<exp[i].mcf<<" zone="<<exp[i].zone<<" coll="<<exp[i].collision<<" start="<<exp[i].start<<'\n';ok=false;}
  }
  std::cout<<code<<" table compare "<<(ok?"OK":"MISMATCH")<<'\n';if(!ok)++failures;++total;
  // Simulate 6 s at the port's .016 step; fire every Animation-action target at t=1 s.
  const std::uint32_t timestamp=0x1234567u;
  for(int step=0;step<375;++step){
   if(step==62)for(const auto& x:source->table().actions)if(x.kind==a::TrackAnimationActionKind50AN::animation)runtime.fire(x.target,x.trigger);
   if(!runtime.tick(.016f,false,timestamp,raw,random,shared,d)){std::cout<<code<<" FAIL tick "<<step<<": "<<d.message<<" row="<<d.row<<'\n';++failures;break;}
   if(!runtime.prepareDraw(d)){std::cout<<code<<" FAIL draw: "<<d.message<<'\n';++failures;break;}
  }
  for(std::size_t i=0;i<runtime.rowCount();++i){const auto& s=runtime.state(i);
   std::cout<<"  row "<<(i+1)<<" after 6s: stopped="<<s.stopped10<<" time="<<s.time34<<" started="<<s.everStarted<<" at_tick="<<s.startedAtTick<<" by="<<s.startedByTrigger<<'\n';}
 }
 // ---- PORT EXTENSIONS (a)-(d) on TA row 1 (N64-only abilities; stock never calls them) ----
 {const std::string path=dir+"\\TA.RES";const auto bytes=readFile(path);a::ResArchive48C archive;a::DecodeError48C error{};a::decodeResource48C(bytes,archive,error);
  std::vector<std::uint8_t> tob;a::extractResMember48C(archive,"tracks\\ta.tob",tob,error);a::ParameterDocument48F doc;a::WorldLimits48F wl{};wl.retainDuplicateLabels=true;a::decodeParameters48F(tob,doc,error,nullptr,wl);
  const srcns::Extract extract=[&](const std::string& name,std::vector<std::uint8_t>& out,std::size_t limit){a::DecodeError48C e{};return a::extractResMember48C(archive,name,out,e,limit);};
  g::SourceLibmState49T libm{};g::ModelMotionSharedState49T shared{};g::SharedRandomOwner50ID random{};std::uint32_t raw=0;
  auto source=std::make_shared<srcns::Owner>();srcns::Diagnostic sd{};source->load(doc,0,extract,libm,sd);
  rt::Owner r;rt::Diagnostic d{};r.create(source,shared,"TA",d);rt::Control& c=r;
  auto step=[&](int n){for(int k=0;k<n;++k){r.tick(.016f,false,1u,raw,random,shared,d);r.prepareDraw(d);}};
  auto ok=[&](bool v,const char* what){std::cout<<"  extension "<<what<<": "<<(v?"OK":"FAIL")<<'\n';if(!v)++failures;};
  const float duration=source->motion(r.state(0).motion18).view.duration;
  ok(c.setTime(0,4.f)&&r.state(0).time34==4.f,"(a) set-time 4 s");
  ok(c.setRate(0,2.f,false,true),"(a) rate +2 accepted");step(200);
  ok(r.state(0).atEnd&&r.state(0).time34==duration,"(a) stop at end holds the end pose (rate +2)");
  ok(c.setRate(0,-3.f,false,true),"(a) rate -3 accepted");step(300);
  ok(r.state(0).atEnd&&r.state(0).time34==0.f,"(a) negative rate stops at time 0");
  ok(c.setRate(0,1.f,true,false),"(a) looping rate");step(600);ok(!r.state(0).atEnd&&r.state(0).time34>=0.f&&r.state(0).time34<=duration,"(a) looping stays inside the motion");
  ok(c.clearRate(0),"(a) clear rate");
  ok(c.hide(0,true)&&!r.state(0).collisionWanted&&!r.state(0).enabled29,"(b) hide unregisters collision");step(1);
  {const auto& m=r.mesh();const auto s0=r.state(0).vertexStart;bool degenerate=true;for(std::uint32_t k=1;k<r.state(0).vertexCount;++k)for(int q=0;q<3;++q)degenerate=degenerate&&m.vertices[s0+k].position[q]==m.vertices[s0].position[q];ok(degenerate,"(b) hidden row not drawn");}
  ok(c.show(0)&&r.state(0).collisionWanted&&r.state(0).enabled29,"(b) show re-registers");
  g::BodyMatrix48G scaled{};for(unsigned k=0;k<16;k+=5)scaled.v[k]=2.f;scaled.v[15]=1.f;scaled.v[12]=100.f;
  ok(c.setTransformOverride(0,&scaled)&&r.state(0).overrideScaled&&!r.state(0).collisionWanted,"(c) scaled override drops collision");step(1);
  ok(c.setTransformOverride(0,nullptr)&&!r.state(0).transformOverride&&r.state(0).collisionWanted,"(c) override cleared");
  ok(source->row(0).authored.startHidden==false,"(c) StartHidden absent on stock TA");
  ok(c.setAlpha(0,.5f)&&r.state(0).alpha==.5f,"(d) alpha kept");step(1);ok(c.setAlpha(0,1.f),"(d) alpha restored");
  ok(c.findReference("none")==1,"2DA190 last-match ReferenceName rule");
 }
 // ---- pose comparison against the old per-course owners that load from the archive path ----
 auto compare=[&](const char* code,auto& old,auto fireOld,auto od,const std::vector<int>& checkSteps){
  const std::string path=dir+"\\"+code+".RES";
  g::SourceLibmState49T libmOld{},libmNew{};g::ModelMotionSharedState49T sharedOld{},sharedNew{};
  g::SharedRandomOwner50ID randomOld{},randomNew{};std::uint32_t rawOld=0,rawNew=0;
  if(!old->load(path,libmOld,sharedOld,od)){std::cout<<code<<" old owner load FAIL: "<<od.message<<'\n';++failures;return;}
  const auto bytes=readFile(path);a::ResArchive48C archive;a::DecodeError48C error{};a::decodeResource48C(bytes,archive,error);
  std::vector<std::uint8_t> tob;std::string lower=code;for(auto& c:lower)c=char(c-'A'+'a');a::extractResMember48C(archive,"tracks\\"+lower+".tob",tob,error);
  a::ParameterDocument48F doc;a::WorldLimits48F wl{};wl.retainDuplicateLabels=true;a::decodeParameters48F(tob,doc,error,nullptr,wl);
  const srcns::Extract extract=[&](const std::string& name,std::vector<std::uint8_t>& out,std::size_t limit){a::DecodeError48C e{};return a::extractResMember48C(archive,name,out,e,limit);};
  auto source=std::make_shared<srcns::Owner>();srcns::Diagnostic sd{};source->load(doc,0,extract,libmNew,sd);
  rt::Owner runtime;rt::Diagnostic d{};if(!runtime.create(source,sharedNew,code,d)){std::cout<<code<<" new create FAIL "<<d.message<<'\n';++failures;return;}
  float worst=0;const std::uint32_t timestamp=0x1234567u;
  std::cout<<code<<" pose compare at step 0: max |dpos| = "<<maxDiff(old->mesh(),runtime.mesh())<<'\n';
  for(int step=1;step<=checkSteps.back();++step){
   if(step==62){fireOld(*old);for(const auto& x:source->table().actions)if(x.kind==a::TrackAnimationActionKind50AN::animation)runtime.fire(x.target,x.trigger);}
   if(!old->service(.016f,timestamp,rawOld,randomOld,sharedOld,od)){std::cout<<code<<" old service FAIL "<<od.message<<'\n';++failures;return;}
   if(!runtime.tick(.016f,false,timestamp,rawNew,randomNew,sharedNew,d)||!runtime.prepareDraw(d)){std::cout<<code<<" new tick FAIL "<<d.message<<'\n';++failures;return;}
   if(std::find(checkSteps.begin(),checkSteps.end(),step)!=checkSteps.end()){const float m=maxDiff(old->mesh(),runtime.mesh());worst=std::max(worst,m);
    std::cout<<code<<" pose compare at step "<<step<<" ("<<step*0.016f<<" s): max |dpos| = "<<m<<'\n';}
  }
  if(rawOld!=rawNew){std::cout<<code<<" shared RNG differs old="<<rawOld<<" new="<<rawNew<<'\n';++failures;}
  if(worst!=0.f){std::cout<<code<<" POSE DIFFERS (worst "<<worst<<")\n";++failures;}else std::cout<<code<<" poses identical to the old owner\n";
 };
 const std::vector<int> steps{1,30,61,63,80,100,150,200,300,400};
 {auto old=std::make_unique<gc_animated_world50IG::Owner>();compare("GC",old,[](gc_animated_world50IG::Owner& o){o.activateSequence(0);},gc_animated_world50IG::Diagnostic{},steps);}
 {auto old=std::make_unique<sa_animated_world::Owner>();compare("SA",old,[](sa_animated_world::Owner&){},sa_animated_world::Diagnostic{},steps);}
 {auto old=std::make_unique<ga_animated_world::Owner>();compare("GA",old,[](ga_animated_world::Owner& o){o.activateSequence(0);o.activateSequence(1);},ga_animated_world::Diagnostic{},steps);}
 std::cout<<"courses with rows checked="<<total<<" failures="<<failures<<'\n';
 return failures?1:0;
}
