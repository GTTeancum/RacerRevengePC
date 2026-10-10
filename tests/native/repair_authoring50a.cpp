#include "../../src/assets/repair_authoring50a.h"
#include <atomic>
#include <cfenv>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <new>
#include <sstream>
#include <stdexcept>
#include <string>
using namespace rr::assets;
static std::atomic<unsigned> allocations{0};static unsigned checks=0;
void* operator new(std::size_t n){++allocations;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
static void check(bool b,const char* why){++checks;if(!b)throw std::runtime_error(why);}
static std::uint32_t bits(const float& f){std::uint32_t b;std::memcpy(&b,&f,4);return b;}
static std::string file(const std::string& p){std::ifstream f(p,std::ios::binary);if(!f)throw std::runtime_error("fixture open");return {std::istreambuf_iterator<char>(f),{}};}
static bool read(const std::string& s,RepairAuthoring50A& a,PodPhysicsDiagnostic49N& d,RepairProvenance50A* p=nullptr){const auto before=allocations.load();const bool ok=readRepairAuthoring50A(s.data(),s.size(),a,d,p);check(allocations.load()==before,"parser allocated");return ok;}
static void reject(const std::string& s,PodPhysicsError49N expected){
 RepairAuthoring50A a;RepairProvenance50A p;std::memset(&a,0xa5,sizeof a);std::memset(&p,0x5a,sizeof p);
 unsigned char ab[sizeof a],pb[sizeof p];std::memcpy(ab,&a,sizeof a);std::memcpy(pb,&p,sizeof p);PodPhysicsDiagnostic49N d{};
 check(!read(s,a,d,&p),"invalid accepted");check(d.code==expected&&d.byteOffset<=s.size(),"diagnostic");check(!std::memcmp(ab,&a,sizeof a)&&!std::memcmp(pb,&p,sizeof p),"failure not atomic");
}
int main(int argc,char** argv){try{
 check(argc==2,"usage: repair_authoring50a fixture-directory");const std::string dir=argv[1],phy=file(dir+"/Pod01.phy");const auto original=phy;
 RepairAuthoring50A a{};RepairProvenance50A p{};PodPhysicsDiagnostic49N d{};
 check(read(phy,a,d,&p)&&d.code==PodPhysicsError49N::none,"real Pod01 parse");check(phy==original,"source bytes changed");
 const char* sections[]={"BaseAbilities","MaxAbilities","DamageRepair","DamageRepair","DamageRepair","DamageRepair","DamageRepair"};
 const char* keys[]={"BaseRepairAmount","BaseRepairAmount","MinBaseRepairAmount","MaxBaseRepairAmount","BaseRepairAmountA","BaseRepairAmountB","BaseRepairAmountC"};
 const std::uint32_t expected[]={0x3f800000,0x3f800000,0x3bb43958,0x3c656042,0x3c54fdf4,0x3c54fdf4,0x3c54fdf4};
 std::istringstream authored(file(dir+"/authored.tsv"));unsigned records=0;std::size_t index,sectionAt,keyAt,valueAt,valueBytes;std::string hex;
 while(authored>>index>>sectionAt>>keyAt>>valueAt>>valueBytes>>hex){check(index<7,"source fixture index");const auto& t=p.fields[index];
  check(a.values[index].present&&bits(a.values[index].value)==std::stoul(hex,nullptr,16)&&bits(a.values[index].value)==expected[index],"original authored bits");
  check(t.sectionOffset==sectionAt&&t.keyOffset==keyAt&&t.valueOffset==valueAt&&t.valueBytes==valueBytes,"original token offsets");
  check(phy.substr(t.sectionOffset,t.sectionBytes)==sections[index]&&phy.substr(t.keyOffset,t.keyBytes)==keys[index],"section/key provenance");++records;
 }
 check(records==7,"all authored records");
 for(unsigned i=0;i<7;++i){auto s=phy;const auto& t=p.fields[i];s.replace(t.keyOffset,t.keyBytes,"UnconsumedRepairKey");RepairAuthoring50A missing{};RepairProvenance50A missingP{};
  check(read(s,missing,d,&missingP),"missing field rejected");check(!missing.values[i].present&&bits(missing.values[i].value)==0,"missing default invented");
  const ImpactAuthoringToken49Y empty{};check(!std::memcmp(&missingP.fields[i],&empty,sizeof empty),"absent provenance");
  for(unsigned j=0;j<7;++j)if(j!=i)check(missing.values[j].present&&bits(missing.values[j].value)==expected[j],"missing affected another key");
  s=phy;s.insert(t.keyOffset,std::string(keys[i])+"=1\n");reject(s,PodPhysicsError49N::duplicate_key);
  s=phy;s.replace(t.valueOffset,t.valueBytes,"NaN");reject(s,PodPhysicsError49N::numeric_syntax);
 }
 for(unsigned mask=0;mask<128;++mask){std::string s="; all fields independent\n";
  for(unsigned section=0;section<3;++section){s+="["+std::string(sections[section])+"]\n";for(unsigned i=0;i<7;++i)if((mask&(1u<<i))&&std::string(sections[i])==sections[section])s+=std::string(keys[i])+"="+std::to_string(i+1)+"\n";}
  RepairAuthoring50A m{};RepairProvenance50A mp{};check(read(s,m,d,&mp),"presence grid parse");
  for(unsigned i=0;i<7;++i){check(m.values[i].present==bool(mask&(1u<<i)),"presence grid flag");check(m.values[i].present?m.values[i].value==float(i+1):bits(m.values[i].value)==0,"presence grid value");}
 }
 std::istringstream decimals(file(dir+"/decimal.tsv"));std::string token,d64,f32;unsigned numeric=0;
 while(decimals>>token>>d64>>f32){for(unsigned key=0;key<7;++key){const std::string s="["+std::string(sections[key])+"]\n"+keys[key]+"="+token+"\n";
  for(int mode:{FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){check(std::fesetround(mode)==0,"set rounding");RepairAuthoring50A r{};check(read(s,r,d),"original numeric token rejected");check(r.values[key].present&&bits(r.values[key].value)==std::stoul(f32,nullptr,16),"original decimal output bits");}
 }++numeric;}
 check(numeric==14,"numeric fixtures");std::fesetround(FE_TONEAREST);
 std::string lower=phy;for(char& c:lower)if(c>='A'&&c<='Z')c=char(c+'a'-'A');check(read(lower,a,d),"ASCII case folding");
 std::string lf;for(char c:phy)if(c!='\r')lf+=c;check(read(lf,a,d),"LF form");
 check(read("; no authored repair fields\n",a,d),"comment-only admission");for(const auto& v:a.values)check(!v.present&&bits(v.value)==0,"all absent");
 reject("",PodPhysicsError49N::invalid_input);reject(std::string(32769,'x'),PodPhysicsError49N::text_limit);
 reject(phy+std::string(1,'\0'),PodPhysicsError49N::invalid_character);reject(phy+std::string(1,char(-1)),PodPhysicsError49N::invalid_character);
 reject(phy+"\n"+std::string(256,'x'),PodPhysicsError49N::line_limit);reject("key=1\n",PodPhysicsError49N::syntax);
 reject("[DamageRepair]\nBaseRepairAmountA=\n",PodPhysicsError49N::syntax);
 reject("[Bad]\nbad_key=1\n",PodPhysicsError49N::syntax);reject("[bad\n",PodPhysicsError49N::syntax);
 reject("[Unknown]\nx=1\nX=2\n",PodPhysicsError49N::duplicate_key);reject("[Unknown]\nx=1\n[UNKNOWN]\ny=1\n",PodPhysicsError49N::duplicate_section);
 std::string many;for(unsigned i=0;i<33;++i)many+="[S"+std::to_string(i)+"]\nx=1\n";reject(many,PodPhysicsError49N::section_limit);
 many="[S]\n";for(unsigned i=0;i<513;++i)many+="k"+std::to_string(i)+"=1\n";reject(many,PodPhysicsError49N::key_limit);
 for(const char* bad:{".","+." ,"1.",".1234567","1e2","inf",".5;inline","--.5","1000000000"})reject(std::string("[BaseAbilities]\nBaseRepairAmount=")+bad+"\n",PodPhysicsError49N::numeric_syntax);
 std::cout<<"repair_authoring50a PASS "<<checks<<" checks, 7 original authored values, 128 presence combinations, zero parser allocations\n";return 0;
}catch(const std::exception& e){std::cerr<<"repair_authoring50a FAIL after "<<checks<<": "<<e.what()<<'\n';return 1;}}
