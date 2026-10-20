// Standalone pure candidate test. Requires the exact original Course archive.
// No fake metadata identity, owner token, runtime launch, or profile writes.
#include "original_ui_course_mode2_entry50kg.h"
#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace {
using namespace rr::game;
void check(bool value,const char* message){if(!value)throw std::runtime_error(message);}
bool same(const OriginalUiCourseMode2EntryPlan50KG& a,const OriginalUiCourseMode2EntryPlan50KG& b){
 return a.courseSlot14E4==b.courseSlot14E4&&a.subtypeSlot14F4==b.subtypeSlot14F4&&a.tier14E8==b.tier14E8&&
 a.selectedControl.controlId==b.selectedControl.controlId&&a.selectedControl.course44==b.selectedControl.course44&&
 a.selectedControl.planet40==b.selectedControl.planet40&&a.planetProfileChange.field==b.planetProfileChange.field&&
 a.planetProfileChange.operation==b.planetProfileChange.operation&&a.planetProfileChange.value==b.planetProfileChange.value&&
 a.planetProfileChange.replaceMask==b.planetProfileChange.replaceMask&&a.planetProfileChange.sourcePC==b.planetProfileChange.sourcePC&&a.sourcePC==b.sourcePC;
}
void test(const OriginalUiCourseSelectionData50KG& data){
 constexpr std::array<std::uint32_t,13> order{{0,11,4,8,7,1,5,9,12,6,10,3,2}};
 OriginalUiCourseMode2EntryPlan50KG sentinel;sentinel.courseSlot14E4=99;sentinel.subtypeSlot14F4=88;
 sentinel.tier14E8=77;sentinel.sourcePC=0xabcdefu;sentinel.selectedControl={66,55,44};
 OriginalUiCourseMode2EntryInput50KG in;in.mode34=2;in.career100C0=19;
 for(std::uint32_t career=0;career<27;++career)for(unsigned raw=0;raw<256;++raw){
  in.career100C0=career;in.progress219C=static_cast<std::uint8_t>(raw);
  auto out=sentinel;const auto before=in;
  check(planOriginalUiCourseMode2Entry50KG(data,in,out)==OriginalUiCourseSelectionStatus50KG::ok,"mode2 projection rejected");
  auto slot=(raw%32u)+1u;if(slot>13u)slot=13u;
  check(out.courseSlot14E4==slot&&out.subtypeSlot14F4==1,"source slot/subtype mismatch");
  check(out.tier14E8==(slot<=4?1u:(slot<=8?2u:3u)),"source tier mismatch");
  OriginalUiCourseControl50KG control;check(data.control(static_cast<std::uint16_t>(slot),control),"real control unavailable");
  check(control.course44==order[slot-1]&&out.selectedControl.controlId==control.controlId&&
      out.selectedControl.course44==control.course44&&out.selectedControl.planet40==control.planet40,"authored control mismatch");
  const auto& p=out.planetProfileChange;
  check(p.field==OriginalUiProfileField50KG::rider2Bits1125E&&p.operation==OriginalUiProfileWrite50KG::maskedByte&&
      p.replaceMask==0xe0u&&p.value==((control.planet40&7u)<<5)&&p.sourcePC==0x1da618u,"planet write mismatch");
  for(unsigned old=0;old<256;++old)check((((old&~p.replaceMask)|(p.value&p.replaceMask))&31u)==(old&31u),"unrelated planet byte lanes changed");
  check(in.mode34==before.mode34&&in.career100C0==before.career100C0&&in.progress219C==before.progress219C,"input changed");
 }
 in={2,19,0};
 for(const auto mode:{0u,1u,3u,0xffffffffu}){in.mode34=mode;auto out=sentinel;
  check(planOriginalUiCourseMode2Entry50KG(data,in,out)==OriginalUiCourseSelectionStatus50KG::unsupportedMode&&same(out,sentinel),"wrong-mode failure changed output");}
 in.mode34=2;
 for(const auto career:{27u,28u,0xffffffffu}){in.career100C0=career;auto out=sentinel;
  check(planOriginalUiCourseMode2Entry50KG(data,in,out)==OriginalUiCourseSelectionStatus50KG::invalidSelection&&same(out,sentinel),"bad-career failure changed output");}
 in.career100C0=19;OriginalUiCourseSelectionData50KG unbound;auto out=sentinel;
 check(planOriginalUiCourseMode2Entry50KG(unbound,in,out)==OriginalUiCourseSelectionStatus50KG::invalidPage&&same(out,sentinel),"unbound metadata accepted or changed output");
 // Existing mode0/1 planner remains separate, including remembered lanes.
 // Vary unrelated mode's lanes; use actual authenticated controls throughout.
 for(unsigned mode=0;mode<2;++mode)for(unsigned slot=1;slot<=13;++slot){
  OriginalUiCourseRememberedProfile50KG p;p.ownerReady=true;p.mode34=mode;
  p.course700=slot;p.course710=slot;p.subtype70C=1;
  p.laps704=7;p.entrants708=6;p.laps718=9;p.entrants714=5;
  OriginalUiCourseEntryPrefix50KG old;
  check(planOriginalUiCourseEntryPrefix50KG(data,p,old)==OriginalUiCourseSelectionStatus50KG::ok,"existing mode0/1 rejected");
  check(old.courseSlot14E4==slot&&old.subtypeSlot14F4==1&&old.laps14EC==(mode?9u:7u)&&old.entrants14F0==(mode?5u:6u),"existing mode0/1 lanes changed");
  p.mode34=2;
  check(planOriginalUiCourseEntryPrefix50KG(data,p,old)==OriginalUiCourseSelectionStatus50KG::unsupportedMode,"old runtime-facing leaf accidentally admitted mode2");
 }
 // No generation, ownerReady, laps or entrants occur in mode2 plan/input.
 // This test therefore cannot create or validate authority. Future private
 // snapshot/commit identity tests belong to that owner, not this scalar leaf.
}
}
int main(int argc,char** argv){
 try {
  check(argc==2,"usage: mode2-entry-test /path/to/original/COURSE.RES");
  const auto* identity=rr::assets::originalUiArchiveIdentity50KG(rr::assets::OriginalUiArchiveId50KG::course);
  check(identity,"Course catalog missing");
  std::ifstream file(argv[1],std::ios::binary|std::ios::ate);check(bool(file),"cannot open supplied original Course archive");
  const auto length=file.tellg();check(length>=0&&static_cast<std::uint64_t>(length)==identity->bytes,"Course archive size mismatch");
  std::vector<std::uint8_t> bytes(identity->bytes);file.seekg(0);
  file.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));check(bool(file),"Course archive read failed");
  auto archive=std::make_shared<rr::assets::OriginalUiArchive50KG>();rr::assets::OriginalUiArchiveDiagnostic50KG ad;
  check(archive->load(rr::assets::OriginalUiArchiveId50KG::course,bytes,ad),"Course archive/member authentication failed");
  OriginalUiCourseSelectionData50KG data;OriginalUiCourseSelectionDiagnostic50KG d;
  check(data.load(archive,d),"authenticated Course metadata load failed");test(data);
  std::cout<<"Candidate pure checks passed against authenticated Course metadata; no gameplay, progression or authorization tested.\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
