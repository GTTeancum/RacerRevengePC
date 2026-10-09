#include "../../src/assets/impact_authoring49y.h"
#include <atomic>
#include <cstdlib>
#include <new>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cfenv>
static std::atomic<unsigned> allocations{0};
void* operator new(std::size_t n) { ++allocations; if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc(); }
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);} void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);} void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
using namespace rr::assets;
static unsigned checks=0;
static void check(bool b,const char* why){++checks;if(!b)throw std::runtime_error(why);}
static std::uint32_t bits(float f){std::uint32_t v;std::memcpy(&v,&f,4);return v;}
static std::string file(const std::string& p){std::ifstream f(p,std::ios::binary);if(!f)throw std::runtime_error("fixture open");return {std::istreambuf_iterator<char>(f),{}};}
static bool env(const std::string& s,rr::game::StaticImpactTuning49Y& out,PodPhysicsDiagnostic49N& d,EnvironmentImpactProvenance49Y* p=nullptr){const auto before=allocations.load();bool ok=readEnvironmentImpact49Y(s.data(),s.size(),out,d,p);check(before==allocations.load(),"environment allocation");return ok;}
static bool defense(const std::string& s,DefenseAuthoring49Y& out,PodPhysicsDiagnostic49N& d,DefenseProvenance49Y* p=nullptr){const auto before=allocations.load();bool ok=readDefenseAuthoring49Y(s.data(),s.size(),out,d,p);check(before==allocations.load(),"defense allocation");return ok;}
static void rejected(const std::string& s,PodPhysicsError49N code){
 rr::game::StaticImpactTuning49Y out; EnvironmentImpactProvenance49Y p;
 std::memset(&out,0xa5,sizeof(out));std::memset(&p,0x5a,sizeof(p));
 unsigned char old[sizeof(out)],audit[sizeof(p)];std::memcpy(old,&out,sizeof(out));std::memcpy(audit,&p,sizeof(p));PodPhysicsDiagnostic49N d{};
 check(!env(s,out,d,&p),"accepted bad document");check(d.code==code,"wrong error");check(d.byteOffset<=s.size(),"error offset bounds");
 check(!std::memcmp(old,&out,sizeof(out))&&!std::memcmp(audit,&p,sizeof(p)),"rejection was not atomic");
}
static void token(const std::string& s,const ImpactAuthoringToken49Y& t,const char* section,const char* key,const char* value){
 check(s.substr(t.sectionOffset,t.sectionBytes)==section,"section provenance");check(s.substr(t.keyOffset,t.keyBytes)==key,"key provenance");check(s.substr(t.valueOffset,t.valueBytes)==value,"value provenance");
}
int main(int argc,char** argv){try{
 check(argc==2,"usage: impact_authoring49y fixture-directory");const std::string dir=argv[1];
 const auto ta=file(dir+"/ta.tun"),phy=file(dir+"/Pod01.phy");
 rr::game::StaticImpactTuning49Y out{};EnvironmentImpactProvenance49Y p{};PodPhysicsDiagnostic49N d{};
 check(env(ta,out,d,&p),"actual TA.tun");check(d.code==PodPhysicsError49N::none,"success diagnostic");
 const std::uint32_t expected[]={0x44834000,0x4444e000,0x43dc0000,0x432f0000,0x42480000,0x41600000,0x41400000,0x41000000,0x40a00000,0x3f000000};
 const char* names[]={"hitXL","hitL","hitM","hitS","hitXS","pointsXL","pointsL","pointsM","pointsS","pointsXS"};
 const char* values[]={"1050","787.5","440","175","50","14","12","8","5",".5"};
 for(unsigned i=0;i<10;++i){check(bits(i<5?out.thresholds00[i]:out.severities14[i-5])==expected[i],"actual environment bits");token(ta,p.fields[i],"EnvDamage",names[i],values[i]);}
 FinalImpactAuthoring49Y finalImpact{};FinalImpactProvenance49Y finalAudit{};
 const auto finalBefore=allocations.load();const bool finalOk=readFinalImpactAuthoring49Y(ta.data(),ta.size(),finalImpact,d,&finalAudit);
 check(finalOk&&allocations.load()==finalBefore,"actual final impact allocation/parse");
 check(bits(finalImpact.threshold64)==0x3f800000&&bits(finalImpact.increment68)==0,"final impact bits");
 token(ta,finalAudit.fields[0],"PodDamage","FinalDamageThreshold","1.0");token(ta,finalAudit.fields[1],"PodDamage","FinalDamageIncrement","0.0");
 for(unsigned i=0;i<2;++i){auto s=ta;const auto& t=finalAudit.fields[i];s.replace(t.valueOffset,t.valueBytes,"1e2");const auto old=finalImpact;const auto audit=finalAudit;
 check(!readFinalImpactAuthoring49Y(s.data(),s.size(),finalImpact,d,&finalAudit)&&d.code==PodPhysicsError49N::numeric_syntax,"final numeric rejection");
 check(!std::memcmp(&old,&finalImpact,sizeof(old))&&!std::memcmp(&audit,&finalAudit,sizeof(audit)),"final rejection atomic");
 s=ta;s.replace(t.keyOffset,t.keyBytes,"UnknownKey");check(!readFinalImpactAuthoring49Y(s.data(),s.size(),finalImpact,d)&&d.code==PodPhysicsError49N::missing_key,"final missing rejection");
 }
 DefenseAuthoring49Y author{};DefenseProvenance49Y provenance{};
 check(defense(phy,author,d,&provenance),"actual Pod01.phy");
 const DefenseValue49Y fields[]={author.baseDefense,author.maxDefense,author.minDefense,author.maxDefenseBound};
 const std::uint32_t dv[]={0x3f4ccccd,0x3f4ccccd,0,0x3f800000};
 const char* ds[]={"BaseAbilities","MaxAbilities","DamageRepair","DamageRepair"};const char* dk[]={"Defense","Defense","MinDefense","MaxDefense"};
 for(unsigned i=0;i<4;++i){check(fields[i].present&&bits(fields[i].value)==dv[i],"actual defense bits/presence");const auto& t=provenance.fields[i];check(phy.substr(t.sectionOffset,t.sectionBytes)==ds[i]&&phy.substr(t.keyOffset,t.keyBytes)==dk[i],"defense provenance");float v;check(parseImpactNumber49Y(phy.data()+t.valueOffset,t.valueBytes,v)&&bits(v)==dv[i],"defense value provenance");}
 check(defense("[Other]\nignored = garbage\n",author,d,&provenance),"unknown fields");check(!author.baseDefense.present&&!author.maxDefense.present&&!author.minDefense.present&&!author.maxDefenseBound.present,"missing defense must remain absent");
 check(defense("[baseabilities]\ndefense=.5\n",author,d),"case insensitive defense");check(author.baseDefense.present&&bits(author.baseDefense.value)==0x3f000000&&!author.maxDefense.present,"independent presence");
 const auto initial=author;const auto initialAudit=provenance;
 check(!defense("[BaseAbilities]\nDefense=.5\nDefense=.8\n",author,d,&provenance),"duplicate defense");check(!std::memcmp(&initial,&author,sizeof(author))&&!std::memcmp(&initialAudit,&provenance,sizeof(provenance)),"defense atomic failure");
 std::string mixed=ta;for(char& c:mixed)if(c>='A'&&c<='Z')c=char(c+'a'-'A');check(env(mixed,out,d),"ASCII case insensitive");
 std::string lf;for(char c:ta)if(c!='\r')lf+=c;check(env(lf,out,d),"LF input");check(env("; comment\n\t\n"+ta,out,d),"comments and blank lines");
 for(unsigned i=0;i<10;++i){auto s=ta;const auto start=s.rfind('\n',p.fields[i].keyOffset);const auto end=s.find('\n',p.fields[i].valueOffset);s.erase(start==std::string::npos?0:start+1,end-(start==std::string::npos?0:start+1)+1);rejected(s,PodPhysicsError49N::missing_key);
 s=ta;s.insert(p.fields[i].keyOffset,std::string(names[i])+"=1\n");rejected(s,PodPhysicsError49N::duplicate_key);
 s=ta;s.replace(p.fields[i].valueOffset,p.fields[i].valueBytes,"NaN");rejected(s,PodPhysicsError49N::numeric_syntax);}
 rejected("",PodPhysicsError49N::invalid_input);rejected(std::string(32769,'x'),PodPhysicsError49N::text_limit);
 rejected(ta+std::string(1,'\0'),PodPhysicsError49N::invalid_character);rejected(ta+std::string(1,char(-1)),PodPhysicsError49N::invalid_character);
 rejected(ta+std::string(256,'x'),PodPhysicsError49N::line_limit);rejected("value=1\n"+ta,PodPhysicsError49N::syntax);
 rejected(ta+"\n[envdamage]\nx=1\n",PodPhysicsError49N::duplicate_section);
 rejected(ta+"\n[Unknown]\nx=1\nX=2\n",PodPhysicsError49N::duplicate_key);
 rejected(ta+"\n[broken\n",PodPhysicsError49N::syntax);rejected(ta+"\n[A]\nbad_key=1\n",PodPhysicsError49N::syntax);
 std::string many;for(unsigned i=0;i<33;++i)many+="[S"+std::to_string(i)+"]\nx=1\n";rejected(many,PodPhysicsError49N::section_limit);
 many="[S]\n";for(unsigned i=0;i<513;++i)many+="k"+std::to_string(i)+"=1\n";rejected(many,PodPhysicsError49N::key_limit);
 const char* bad[]={".","+.","-.","1.",".1234567","1e2","nan","inf",".5f"," .5",".5 ","--.5","+.5;comment","1000000000"};
 for(const char* s:bad){float v=42;std::uint64_t b=0x12345678;check(!parseImpactNumber49Y(s,std::strlen(s),v,&b),"bad decimal accepted");check(v==42&&b==0x12345678,"decimal atomic failure");}
 std::istringstream fixtures(file(dir+"/decimal.tsv"));std::string line;unsigned count=0;
 while(std::getline(fixtures,line)){std::istringstream in(line);std::string s,ds64,fs32;in>>s>>ds64>>fs32;const auto doubleBits=std::stoull(ds64,nullptr,16);const auto floatBits=std::stoul(fs32,nullptr,16);
  for(int mode:{FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){check(std::fesetround(mode)==0,"round mode");float f=42;std::uint64_t b=0;const auto before=allocations.load();const bool ok=parseImpactNumber49Y(s.data(),s.size(),f,&b);check(before==allocations.load(),"decimal allocation");check(ok&&b==doubleBits&&bits(f)==floatBits,"original-word decimal mismatch");}++count;
 }
 std::fesetround(FE_TONEAREST);check(count>800,"missing numeric corpus");
 std::cout<<"impact_authoring49y PASS "<<checks<<" checks, "<<count<<" original-word decimals, zero parser allocations\n";return 0;
}catch(const std::exception& e){std::cerr<<"impact_authoring49y FAIL after "<<checks<<": "<<e.what()<<'\n';return 1;}}

