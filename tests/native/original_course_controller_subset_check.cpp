// Proposal-only focused checker. Its author has neither compiled nor run it.
// Positive cases use authenticated original archives and real controller phase
// APIs. Negative fixtures deliberately corrupt copies of that typed state.
// No guest execution, live gameplay, profile mutation or fake owner admissions.
#include "original_ui_course_widget_controller50kg.h"
#include <array>
#include <bit>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
namespace a=rr::assets;
using namespace rr::game;
using Status=OriginalUiCourseWidgetControllerStatus50KG;
using Plan=OriginalUiCourseWidgetPlan50KG;
using Nav=OriginalUiNavigation50KG;
using Profile=OriginalUiCourseWidgetProfile50KG;
using Op=OriginalUiControlOperation50KG;
using Step=OriginalUiCourseWidgetStep50KG;
using StepKind=OriginalUiCourseWidgetStepKind50KG;
unsigned checks=0,rowCases=0,lapCases=0,negativeCases=0,courseCases=0,routeCases=0,textCases=0;
void need(bool okay,const std::string& why){++checks;if(!okay)throw std::runtime_error(why);}
std::uint32_t bits(float x){return std::bit_cast<std::uint32_t>(x);}
std::shared_ptr<a::OriginalUiArchive50KG> load(const std::filesystem::path& root,a::OriginalUiArchiveId50KG id){
 const auto* identity=a::originalUiArchiveIdentity50KG(id);need(identity,"archive catalog entry");
 const auto path=root/identity->discPath;
 std::ifstream file(path,std::ios::binary|std::ios::ate);need(bool(file),"open "+path.string());
 const auto size=file.tellg();need(size>=0&&static_cast<std::uint64_t>(size)==identity->bytes,"archive exact byte count "+path.string());
 std::vector<std::uint8_t> bytes(identity->bytes);file.seekg(0);
 file.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));need(bool(file),"read original archive");
 auto archive=std::make_shared<a::OriginalUiArchive50KG>();a::OriginalUiArchiveDiagnostic50KG d;
 need(archive->load(id,bytes,d),"archive/member authentication "+path.string());return archive;
}
Profile profile(unsigned course=1){
 Profile p;p.course700=course;p.subtype70C=1;p.flags11018=4;p.flags1125C=0x23;
 p.courseRecords100C4={{0,11,4,8,7,1,5,9,12,6,10,3,2}};return p;
}
bool sameNav(const Nav& x,const Nav& y){
 if(x.page!=y.page||x.controllerGroup!=y.controllerGroup)return false;
 for(std::size_t i=0;i<x.controls.size();++i){const auto& a=x.controls[i];const auto& b=y.controls[i];
  if(a.enabled!=b.enabled||a.blocked!=b.blocked||a.selected!=b.selected||a.shown!=b.shown)return false;}
 for(std::size_t i=0;i<x.groups.size();++i)if(x.groups[i].enabled!=y.groups[i].enabled||x.groups[i].selectedSlot!=y.groups[i].selectedSlot)return false;
 return true;
}
bool sameStep(const Step& a,const Step& b){return a.kind==b.kind&&a.widget==b.widget&&a.operation==b.operation&&a.value==b.value&&a.sourcePC==b.sourcePC&&a.argument==b.argument;}
bool samePlan(const Plan& a,const Plan& b){
 if(!sameNav(a.navigation,b.navigation)||a.count!=b.count||a.writesPromptMask42C!=b.writesPromptMask42C||a.promptMask42C!=b.promptMask42C)return false;
 for(std::size_t i=0;i<a.steps.size();++i)if(!sameStep(a.steps[i],b.steps[i]))return false;return true;
}
bool sameLap(const OriginalUiCourseLapMove50KG& a,const OriginalUiCourseLapMove50KG& b){
 return a.laps14EC==b.laps14EC&&a.sourcePC==b.sourcePC&&a.audioPC==b.audioPC&&a.effectId==b.effectId&&
  bits(a.repeatRight14D4)==bits(b.repeatRight14D4)&&bits(a.repeatLeft14D8)==bits(b.repeatLeft14D8);
}
Plan sentinelPlan(const Nav& n){
 Plan p;p.navigation=n;p.count=3;p.writesPromptMask42C=true;p.promptMask42C=0x12345678;
 for(std::size_t i=0;i<p.steps.size();++i)p.steps[i]={StepKind::promptMask42C,static_cast<std::uint16_t>(i),Op::reset,true,0xCA000000u+static_cast<std::uint32_t>(i),77};
 return p;
}
Nav opened(OriginalUiCourseWidgetController50KG& controller,const Profile& p){
 Nav n;need(controller.initialNavigation(n),"initial authenticated navigation");
 for(const auto phase:{OriginalUiCourseWidgetPhase50KG::requestedPage90,OriginalUiCourseWidgetPhase50KG::entry74,
                      OriginalUiCourseWidgetPhase50KG::openingAC,OriginalUiCourseWidgetPhase50KG::openedB0}){
  Plan plan;need(controller.phase(n,phase,p,plan)==Status::ok,"real controller phase");n=plan.navigation;}
 need(n.controllerGroup[0]==1&&n.controllerGroup[1]==-1,"opened real course focus");
 need(n.groups[2].selectedSlot==1&&n.groups[3].selectedSlot==0,"subtype1 and real empty group3");return n;
}
// Independent source-order oracle: expand every authenticated membership,
// preserving duplicates. No candidate moveRow/moveLaps call is used here.
void expectedControl(Plan& p,int control,Op op,std::uint32_t pc,bool value=false){
 if(control==-1)return;need(control>=0&&static_cast<std::size_t>(control)<p.navigation.page->controls.size(),"oracle control id");
 for(auto widget:p.navigation.page->controls[static_cast<std::size_t>(control)].widgets){
  need(p.count<p.steps.size(),"oracle capacity");p.steps[p.count++]={StepKind::widget,widget,op,value,pc,0};}
 auto& s=p.navigation.controls[static_cast<std::size_t>(control)];
 switch(op){case Op::select:s.selected=true;break;case Op::deselect:case Op::clearSelection:s.selected=false;break;
 case Op::show:s.shown=true;break;case Op::hide:s.shown=false;break;case Op::setEnabled:s.enabled=value;break;case Op::reset:break;}
}
Plan expectedRow(const Nav& n,unsigned target){
 Plan result;result.navigation=n;const auto& old=n.page->groups[static_cast<std::size_t>(n.controllerGroup[0])];
 for(auto id:old.associations)expectedControl(result,id,Op::deselect,0x2A4E78);
 for(auto id:old.controls)expectedControl(result,id,old.keepEnabled?Op::clearSelection:Op::deselect,old.keepEnabled?0x2A4F10:0x2A4EC4);
 const auto& next=n.page->groups[target];for(auto id:next.associations)expectedControl(result,id,Op::select,0x2A4DFC);
 if(!next.controls.empty()){
  for(auto id:next.controls)expectedControl(result,id,Op::deselect,0x2A4FA8);
  expectedControl(result,next.controls[n.groups[target].selectedSlot-1],Op::select,0x2A4FD4);}
 result.navigation.controllerGroup[0]=static_cast<std::int16_t>(target);return result;
}
void rows(OriginalUiCourseWidgetController50KG& c,const Profile& p,Nav& row3){
 const auto start=opened(c,p);need(start.page->groups[3].controls.empty()&&!start.page->groups[3].associations.empty(),"authored association-only group3");
 need(start.groups[4].enabled,"real opened Confirm/Back group4 enabled");
 const unsigned downTargets[]={2,3,1},upTargets[]={3,2,1};
 for(bool down:{false,true}){
  auto n=start;for(unsigned step=0;step<3;++step){
   const auto before=n;const auto target=down?downTargets[step]:upTargets[step];auto expected=expectedRow(before,target);auto got=sentinelPlan(before);
   need(c.moveRow(n,down,p,got)==Status::ok,"exact row move status");need(sameNav(n,before),"row input immutable");
   need(samePlan(got,expected),"authored row callback order/state/PC exact");
   need(got.navigation.groups[3].selectedSlot==0,"empty group remains slot0");
   need(got.navigation.controllerGroup[0]>=1&&got.navigation.controllerGroup[0]<=3,"no group4/5 focus");
   if(target==3){row3=got.navigation;for(std::size_t i=0;i<got.count;++i)need(got.steps[i].sourcePC!=0x2A4FA8&&got.steps[i].sourcePC!=0x2A4FD4,"no empty-group selected-control callback");}
   n=got.navigation;++rowCases;
  }
  need(n.controllerGroup[0]==1,"three moves restore course focus");
 }
 // Deliberate adversarial state: even an enabled group5 must not enter the
 // explicit Course1/2/3 cycle. No metadata or owner identity is fabricated.
 auto extra=start;extra.groups[5].enabled=true;
 for(bool down:{false,true}){Plan got;need(c.moveRow(extra,down,p,got)==Status::ok,"enabled non-row group does not block row move");
  need(samePlan(got,expectedRow(extra,down?2:3)),"groups4/5 excluded when both enabled");}
}
void laps(OriginalUiCourseWidgetController50KG& c,const Profile& p,const Nav& row3){
 for(unsigned old=1;old<=25;++old)for(bool forward:{false,true}){
  auto before=row3;OriginalUiCourseLapMove50KG got;
  need(c.moveLaps(row3,old,forward,p,got)==Status::ok,"lap move admitted");
  need(got.laps14EC==(forward?(old==25?1:old+1):(old==1?25:old-1)),"source1..25 lap wrap");
  need(got.sourcePC==(forward?0x1DF5C4u:0x1DF310u)&&got.audioPC==(forward?0x1DF64Cu:0x1DF398u)&&got.effectId==(forward?4u:3u),"lap source/audio provenance");
  need(bits(got.repeatRight14D4)==(forward?0x3F266666u:0u)&&bits(got.repeatLeft14D8)==(forward?0u:0x3F266666u),"exact source0.65 timer bits/opposite zero");
  need(sameNav(row3,before),"lap move leaves all navigation immutable");++lapCases;
 }
}
void negatives(OriginalUiCourseWidgetController50KG& c,const Profile& p,const Nav& row3){
 const auto start=opened(c,p);const auto ps=sentinelPlan(start);
 const OriginalUiCourseLapMove50KG ls{777,0x12345678,0x87654321,91,7.0f,9.0f};
 auto badRow=[&](const Nav& n,const Profile& pr,Status expected){for(bool down:{false,true}){auto out=ps;const auto before=n;
  need(c.moveRow(n,down,pr,out)==expected&&samePlan(out,ps)&&sameNav(n,before),"invalid row atomic/status");++negativeCases;}};
 auto badLap=[&](const Nav& n,const Profile& pr,unsigned count,Status expected){for(bool f:{false,true}){auto out=ls;const auto before=n;
  need(c.moveLaps(n,count,f,pr,out)==expected&&sameLap(out,ls)&&sameNav(n,before),"invalid lap atomic/status");++negativeCases;}};
 OriginalUiCourseWidgetController50KG unbound;
 auto out=ps;need(unbound.moveRow(start,true,p,out)==Status::notBound&&samePlan(out,ps),"unbound row atomic");
 auto lo=ls;need(unbound.moveLaps(row3,3,true,p,lo)==Status::notBound&&sameLap(lo,ls),"unbound lap atomic");
 auto n=start;n.page=nullptr;badRow(n,p,Status::page);n=row3;n.page=nullptr;badLap(n,p,3,Status::page);
 for(int group:{-1,0,4,5,6}){n=start;n.controllerGroup[0]=static_cast<std::int16_t>(group);badRow(n,p,Status::invalidNavigation);}
 for(int group:{-1,0,1,2,4,5}){n=row3;n.controllerGroup[0]=static_cast<std::int16_t>(group);badLap(n,p,3,Status::invalidNavigation);}
 n=start;n.controllerGroup[1]=1;badRow(n,p,Status::invalidNavigation);n=row3;n.controllerGroup[1]=1;badLap(n,p,3,Status::invalidNavigation);
 for(unsigned g=1;g<=3;++g){n=start;n.groups[g].enabled=false;badRow(n,p,Status::invalidNavigation);
  n=row3;n.groups[g].enabled=false;badLap(n,p,3,Status::invalidNavigation);}
 for(unsigned slot:{0u,5u,65535u}){n=start;n.groups[1].selectedSlot=static_cast<std::uint16_t>(slot);badRow(n,p,Status::invalidNavigation);
  n=row3;n.groups[1].selectedSlot=static_cast<std::uint16_t>(slot);badLap(n,p,3,Status::invalidNavigation);}
 for(unsigned slot:{0u,2u,3u}){n=start;n.groups[2].selectedSlot=static_cast<std::uint16_t>(slot);badRow(n,p,Status::invalidNavigation);
  n=row3;n.groups[2].selectedSlot=static_cast<std::uint16_t>(slot);badLap(n,p,3,Status::invalidNavigation);}
 n=start;n.groups[3].selectedSlot=1;badRow(n,p,Status::invalidNavigation);n=row3;n.groups[3].selectedSlot=1;badLap(n,p,3,Status::invalidNavigation);
 for(unsigned value:{0u,26u,31u,std::numeric_limits<unsigned>::max()})badLap(row3,p,value,Status::invalidNavigation);
 std::vector<Profile> wrong;
 for(unsigned mode:{1u,2u,3u}){auto x=p;x.mode34=mode;wrong.push_back(x);}
 for(unsigned subtype:{0u,2u,3u}){auto x=p;x.subtype70C=subtype;wrong.push_back(x);}
 for(unsigned course:{0u,5u}){auto x=p;x.course700=course;wrong.push_back(x);}
 {auto x=p;x.flags100BC=1;wrong.push_back(x);} {auto x=p;x.flags11018=3;wrong.push_back(x);}
 {auto x=p;x.flags1125C=0x40;wrong.push_back(x);} {auto x=p;x.courseRecords100C4[0]=12;wrong.push_back(x);}
 for(const auto& pr:wrong){badRow(start,pr,Status::unsupportedProfile);badLap(row3,pr,3,Status::unsupportedProfile);}
}
void courses(OriginalUiCourseWidgetController50KG& c){
 for(unsigned course=1;course<=4;++course)for(bool forward:{false,true}){
  const auto p=profile(course);auto n=opened(c,p);const auto before=n;
  Plan expected;expected.navigation=n;OriginalUiCommands50KG commands;
  need(moveOriginalUiControl50KG(expected.navigation,1,forward,commands)==OriginalUiNavigationStatus50KG::ok,"existing generic course control leaf");
  for(std::size_t i=0;i<commands.count;++i){const auto& x=commands.values[i];expectedControl(expected,x.controlId,x.operation,x.sourcePC,x.value);}
  const auto selected=expected.navigation.groups[1].selectedSlot;
  expectedControl(expected,n.page->groups[1].controls[selected-1],Op::reset,0x2A5A38);
  Plan got;need(c.moveCourse(n,forward,p,got)==Status::ok&&samePlan(got,expected)&&sameNav(n,before),"existing course move and reset callback unchanged");
  need(selected==(forward?(course==4?1:course+1):(course==1?4:course-1)),"existing four unlocked authored course order");++courseCases;
 }
 const auto p=profile();auto n=opened(c,p);Plan moved;need(c.moveRow(n,true,p,moved)==Status::ok,"event focus setup");
 const auto sentinel=sentinelPlan(n);auto out=sentinel;
 need(c.moveCourse(moved.navigation,true,p,out)==Status::invalidNavigation&&samePlan(out,sentinel),"event row cannot change course");
}
}
int main(int argc,char** argv){try{need(argc==2,"usage: controller_subset ORIGINAL_DISC_ROOT");auto archive=load(argv[1],a::OriginalUiArchiveId50KG::course);OriginalUiCourseWidgetController50KG controller;a::OriginalUiPageDiagnostic50KG d;need(controller.bind(archive,d)==Status::ok,"actual authenticated controller");const auto p=profile();Nav row3;rows(controller,p,row3);laps(controller,p,row3);negatives(controller,p,row3);courses(controller);std::cout<<"CONTROLLER_SUBSET_PASS rows="<<rowCases<<" laps="<<lapCases<<" negatives="<<negativeCases<<" courses="<<courseCases<<" assertions="<<checks<<"\n";return 0;}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
