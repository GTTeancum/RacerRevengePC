// Host check for helper 145g tasks 2/3 (no game run): stock TRACKS\TA.RES through the
// common trigger reader (course_trigger_sources50ct.h), the common trigger-action owner
// (ga_trigger_actions_fixture.h) and the common track-animation runtime. Checks:
//  - TA's 13 triggers, what is carried (LostAI Reset trigger 13 is not), triggers 3/4
//    Animation -> rows 1/2 at 40 percent, IgnoreTrigger "ANIMATION" ignores nothing;
//  - the 2A00A0 chance draw through dispatchNativeTriggerContact50GA on the shared RNG
//    (MC's 2A013C..230 helper), admitting about 40 percent of first contacts;
//  - a fired trigger starts its barge (track_animation_started by=trigger3/4);
//  - 2702E0: raceReset re-stops both barges at frame 0 and clears the fired bits.
// Build: vs.cmd build_check.py ; run: ta_trigger_check.exe "C:\Games\Star Wars Racer Revenge\TRACKS"
#include "../../../tests/native/course_trigger_sources50ct.h"
#include "../../../tests/native/ga_trigger_actions_fixture.h"
#include "../../../tests/native/track_animation_runtime50an.h"
#include "../../../src/assets/import_decode48c.h"
#include <fstream>
#include <iostream>

namespace rr::assets::dlc50DL{std::filesystem::path host(const std::filesystem::path& p){return p;}}
namespace a=rr::assets;namespace g=rr::game;namespace rt=track_animation_runtime50AN;namespace srcns=track_animation_source50AN;
static int failures=0;
static void check(bool ok,const std::string& what){std::cout<<(ok?"OK   ":"FAIL ")<<what<<'\n';if(!ok)++failures;}
static std::vector<std::uint8_t> readFile(const std::string& p){std::ifstream f(p,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
static const char* kind(course_trigger_sources50CT::Action k){
 switch(k){case course_trigger_sources50CT::Action::LostAI:return "LostAI";case course_trigger_sources50CT::Action::Animation:return "Animation";default:return "PodSound";}}

int main(int argc,char** argv){
 if(argc<2){std::cerr<<"usage: ta_trigger_check <TRACKS dir>\n";return 2;}
 const std::string path=std::string(argv[1])+"\\TA.RES";
 // ---- reader ----
 course_trigger_sources50CT::Owner triggers;std::string error;
 course_trigger_sources50CT::Support support;support.randomPercent=support.ignoreAi=support.reset=true;
 check(triggers.load(path,course_trigger_sources50CT::stockTa,64,1024,error,support),"TA trigger reader: "+error);
 if(failures)return 1;
 check(triggers.liveCount()==13,"TA live triggers = 13 (got "+std::to_string(triggers.liveCount())+")");
 check(triggers.stockTob(),"TA TOB is the stock receipt");
 for(std::size_t i=0;i<triggers.liveCount();++i){const auto& s=triggers.source(i);
  std::cout<<"  trigger"<<(i+1)<<" name="<<s.name<<" zone="<<s.zone<<" shape="<<(s.shapeType==6?"box":"sphere")<<" percent="<<s.randomTriggerPercent
           <<" ignore_player="<<s.ignorePlayer<<" ignore_ai="<<s.ignoreAi<<" ignore_animations="<<s.ignoreAnimations<<" one_time="<<s.oneTimeOnly;
  for(std::uint32_t k=0;k<s.actionCount;++k){const auto& x=s.actions[k];std::cout<<" action"<<(k+1)<<"="<<kind(x.kind);
   if(x.kind==course_trigger_sources50CT::Action::LostAI)std::cout<<"(node "<<x.targetNodeIndex<<(x.reset?", Reset":"")<<")";
   if(x.kind==course_trigger_sources50CT::Action::Animation)std::cout<<"(row "<<(x.animationIndex+1)<<")";}
  std::cout<<'\n';}
 for(int t:{2,3}){const auto& s=triggers.source(t);
  check(s.actionCount==1&&s.actions[0].kind==course_trigger_sources50CT::Action::Animation&&s.actions[0].animationIndex==unsigned(t-2)&&
        s.randomTriggerPercent==40&&!s.ignorePlayer&&!s.ignoreAi&&!s.ignoreAnimations,"trigger"+std::to_string(t+1)+" Animation row "+std::to_string(t-1)+", 40 percent, ignores nothing");}
 check(triggers.source(12).actions[0].reset,"trigger13 authors LostAI Reset");
 // ---- animation runtime (the 145d common owner) ----
 const auto bytes=readFile(path);a::ResArchive48C archive;a::DecodeError48C de{};
 check(a::decodeResource48C(bytes,archive,de),"TA archive decode");
 std::vector<std::uint8_t> tob;check(a::extractResMember48C(archive,"tracks\\ta.tob",tob,de),"TA TOB member");
 a::ParameterDocument48F doc;a::WorldLimits48F wl{};wl.retainDuplicateLabels=true;check(a::decodeParameters48F(tob,doc,de,nullptr,wl),"TA TOB decode");
 const srcns::Extract extract=[&](const std::string& name,std::vector<std::uint8_t>& out,std::size_t limit){a::DecodeError48C e{};return a::extractResMember48C(archive,name,out,e,limit);};
 g::SourceLibmState49T libm{};g::ModelMotionSharedState49T shared{};g::SharedRandomOwner50ID animRandom{};std::uint32_t animRaw=0;
 auto source=std::make_shared<srcns::Owner>();srcns::Diagnostic sd{};check(source->load(doc,0,extract,libm,sd),"TA animation source: "+sd.message);
 auto runtime=std::make_shared<rt::Owner>();rt::Diagnostic d{};check(runtime->create(source,shared,"TA",d),"TA animation runtime: "+d.message);
 if(failures)return 1;
 check(runtime->rowCount()==2&&runtime->isTriggerTarget(0)&&runtime->isTriggerTarget(1),"TA 2 rows, both Animation-action targets");
 // ---- common trigger-action owner ----
 g::SharedRandomOwner50ID random{};std::uint32_t raw=0x12345678u;g::HostMonotonicClock50ID clock{};
 random.seed.lazy80A8=true;random.seed.seed3D6B30=raw; // seeded already (as after the first draw of a session)
 ga_trigger_actions::Owner actions;
 check(actions.bindChance(triggers.code(),{&raw,&random,&clock}),"bindChance");
 std::array<ga_trigger_actions::RacerIdentities,ga_trigger_actions::racers> ids{};
 for(std::uint32_t i=0;i<8;++i)ids[i]={0x100u+i*2,0x101u+i*2};
 check(actions.prepare(triggers,runtime.get(),ids,200,nullptr,true,1u,8),"common owner prepare: "+actions.error());
 auto& group=actions.nativeGroup();
 check((group.triggers[12].flags29&1u)==0,"trigger13 (LostAI Reset) registered disabled (not carried)");
 check((group.triggers[2].flags29&1u)&&(group.triggers[3].flags29&1u)&&group.triggers[2].randomTriggerPercent90==40,"triggers 3/4 enabled at 40 percent");
 check(group.ignoreColliderCount[2]==0&&group.ignoreColliderCount[3]==0&&group.ignoreColliderCount[0]==1,"ignore lists: triggers 3/4 none, trigger1 the player");
 // ---- 2A00A0 chance through the live dispatcher ----
 // Statistics on a copy: one first contact per trial on trigger 3 (fresh state each time).
 std::size_t admitted=0,trials=2000,draws0=0;
 {for(std::size_t n=0;n<trials;++n){auto copy=group;copy.actions={&admitted,[](void*,const g::TriggerActionCall50IN&)noexcept{return true;},
    [](void* p,const g::TriggerActionCall50IN&)noexcept{++*static_cast<std::size_t*>(p);}};
   const auto r=g::dispatchNativeTriggerContact50GA(copy,2,0x102u);
   if(r.status!=g::TriggerEntryStatus50IN::Complete||!r.randomConsumed){check(false,"chance dispatch status");break;}
   draws0+=copy.chanceDraws;}}
 const double rate=double(admitted)/double(trials);
 std::cout<<"  chance: "<<admitted<<"/"<<trials<<" admitted ("<<rate*100.<<" percent), draws="<<draws0<<'\n';
 check(draws0==trials&&rate>0.35&&rate<0.45,"trigger3 admits about 40 percent of first contacts, one shared-RNG draw each");
 // The same random sequence as MC's helper: two independent draws from the same seed agree.
 {std::uint32_t r1=7u,r2=7u;g::SharedRandomOwner50ID s1{},s2{};s1.seed.lazy80A8=s2.seed.lazy80A8=true;s1.seed.seed3D6B30=s2.seed.seed3D6B30=7u;
  g::OriginalTriggerChanceReceipt50MC c1{},c2{};g::drawOriginalTriggerChance50MC({&r1,&s1,&clock},c1);
  r2=r2*0x41c64e6du+0x3039u;check(c1.rawAfter==r2&&c1.percent==g::originalTriggerPercent50MC(r2&0x7fffffffu),"draw = 308030 LCG step and 2A013C percent");}
 // ---- live: dispatch until trigger 3 and 4 fire, then tick the barges ----
 bool fired3=false,fired4=false;
 for(int n=0;n<400&&!(fired3&&fired4);++n){
  for(std::size_t t:{std::size_t(2),std::size_t(3)}){
   auto& s=group.triggers[t];if(s.flags1B8&8u){(t==2?fired3:fired4)=true;continue;}
   // The racer's synchronous query lease, as frame_physics50du.cpp opens it.
   std::uint32_t c80=0;const ga_trigger_actions::ActionLease lease{0x102u,&c80,nullptr,0};
   if(!actions.beginQuery(lease)){check(false,"query lease");n=400;break;}
   const auto r=g::dispatchNativeTriggerContact50GA(group,t,0x102u);
   const bool ended=actions.endQuery();
   if(r.status!=g::TriggerEntryStatus50IN::Complete||!ended){check(false,"live dispatch status");n=400;break;}
   // The contact did not admit: as the racer stays then leaves, the trigger's
   // next service clears touched (exit). Model leaving: clear touched bits.
   if(!(s.flags1B8&8u))s.flags1B8=static_cast<std::uint8_t>(s.flags1B8&~0x30u);
  }
 }
 check(fired3&&fired4,"triggers 3 and 4 fire (by chance) and call Control::fire");
 for(int k=0;k<120;++k){if(!runtime->tick(.016f,false,1u,animRaw,animRandom,shared,d)){check(false,"tick: "+d.message);break;}}
 check(runtime->state(0).everStarted&&runtime->state(0).startedByTrigger==3&&runtime->state(1).everStarted&&runtime->state(1).startedByTrigger==4,
       "barges started by trigger3 / trigger4");
 check(runtime->state(0).time34>0.f,"barge 1 sailing (time > 0)");
 // ---- 2702E0 ----
 check(runtime->raceReset(d),"raceReset (101D10 + 101B20): "+d.message);
 check(actions.raceReset()==13,"trigger manager 2DCC70 over 13 triggers");
 check(!(group.triggers[2].flags1B8&8u)&&!(group.triggers[3].flags1B8&8u),"fired bits cleared");
 for(int r=0;r<2;++r)std::cout<<"  after reset row"<<(r+1)<<": stopped="<<runtime->state(r).stopped10<<" time="<<runtime->state(r).time34
     <<" active="<<int(runtime->state(r).active90[0])<<" saved="<<int(runtime->state(r).saved8C[0])<<"\n";
 // 101DA0 clamps the reset time to 1e-4 (the pose is sampled there, as the original).
 check(runtime->state(0).stopped10&&runtime->state(0).time34<=1e-4f&&!runtime->state(0).active90[0]&&
       runtime->state(1).stopped10&&runtime->state(1).time34<=1e-4f&&!runtime->state(1).active90[0],"both barges stopped at their start pose waiting for their triggers again");
 for(int k=0;k<60;++k)runtime->tick(.016f,false,1u,animRaw,animRandom,shared,d);
 check(runtime->state(0).stopped10&&runtime->state(0).time34<=1e-4f&&runtime->state(1).stopped10,"barges still waiting after 60 ticks");
 std::cout<<(failures?"FAILURES ":"ALL OK ")<<failures<<'\n';
 return failures?1:0;
}
