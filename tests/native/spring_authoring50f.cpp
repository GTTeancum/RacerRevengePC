#include "../../src/assets/spring_authoring50f.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <limits>
using namespace rr::assets;
using namespace rr::game;
using J=nlohmann::json;
static bool hot=false;static unsigned allocations=0,checks=0;
void* operator new(std::size_t n){if(hot)++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void*p) noexcept{std::free(p);}void operator delete[](void*p) noexcept{std::free(p);}
void operator delete(void*p,std::size_t) noexcept{std::free(p);}void operator delete[](void*p,std::size_t) noexcept{std::free(p);}
void need(bool b,const char*s){++checks;if(!b)throw std::runtime_error(s);}
float fp(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
std::string read(const char*p){std::ifstream f(p,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char**argv){try{
 need(argc==3,"PHY and fixture paths required");const auto text=read(argv[1]);J fixture=J::parse(read(argv[2]));
 SpringCollisionAuthoring50F authored{};SpringCollisionProvenance50F provenance{};PodPhysicsDiagnostic49N diagnostic{};
 hot=true;const bool ok=readSpringCollision50F(text.data(),text.size(),authored,diagnostic,&provenance);hot=false;need(ok,"actual PHY parsed");
 const float expected[11]={1,0,0,1,.3f,.1f,0,0,0,0,0};
 for(unsigned i=0;i<11;++i){need(bits(authored.values[i])==bits(expected[i]),"actual authored bits");const auto&t=provenance.fields[i];need(text.substr(t.sectionOffset,t.sectionBytes)=="Collisions","section provenance");need(text.substr(t.keyOffset,t.keyBytes)==fixture["keys"][i]["key"],"key provenance");float f;need(parseImpactNumber49Y(text.data()+t.valueOffset,t.valueBytes,f)&&bits(f)==bits(authored.values[i]),"token exact parse");}
 for(unsigned i=0;i<11;++i){auto missing=text;const auto&t=provenance.fields[i];const auto begin=missing.rfind('\n',t.keyOffset);const auto end=missing.find('\n',t.valueOffset);missing.erase(begin+1,end-begin);SpringCollisionAuthoring50F out;SpringCollisionProvenance50F audit;std::memset(&out,0xa5,sizeof out);std::memset(&audit,0x5a,sizeof audit);auto saved=out;auto savedAudit=audit;hot=true;const auto parsed=readSpringCollision50F(missing.data(),missing.size(),out,diagnostic,&audit);hot=false;need(!parsed&&diagnostic.code==PodPhysicsError49N::missing_key,"missing key rejected");need(!std::memcmp(&out,&saved,sizeof out)&&!std::memcmp(&audit,&savedAudit,sizeof audit),"atomic parse failure");}
 for(const std::string bad:{text+"\n[Collisions]\nX=1\n",text+"\nBadKey=1\nBadKey=2\n",std::string("[Collisions]\nSpring=nan\n")}){SpringCollisionAuthoring50F out{};hot=true;const bool parsed=readSpringCollision50F(bad.data(),bad.size(),out,diagnostic);hot=false;need(!parsed,"invalid text rejected");}
 unsigned count=0;
 for(const auto& row:fixture["cases"]){++count;SpringCollisionAuthoring50F a;for(unsigned k=0;k<11;++k)a.values[k]=fp(row["values"][k]);
  SpringGeneratedParameters48K p{};SpringResponseParameters48L r{};p.impact.normalForceScale98=.97f;p.impact.positionCorrectionScaleCC=.2f;p.impact54=true;p.friction50=true;p.impact.disableLeverArms5C=true;
  const std::uint32_t opaque=0x7f800123u;std::memcpy(&p.impact.worldCenter110.v[0],&opaque,4);for(unsigned k=0;k<10;++k)p.frictionByMaterial[k]=float(k+1)/16;
  r.maxSpeed94=10000;r.secondaryAtLimit60=true;SpringGeneratedParameters48K before;SpringResponseParameters48L beforeR;std::memcpy(&before,&p,sizeof p);std::memcpy(&beforeR,&r,sizeof r);
  hot=true;const bool applied=applySpringCollision50F(a,fp(row["mass"]),p,r);hot=false;need(applied,"setter complete");
  const auto&e=row["expected"];for(auto pair:{std::pair<float,unsigned>{r.maxPenetration90,0x90},{r.stiffness9C,0x9c},{r.dampingA0,0xa0},{r.impactStiffnessA4,0xa4},{r.impactDampingA8,0xa8},{p.impact.restitutionB8,0xb8},{p.impact.restitutionBC,0xbc},{p.impact.restitutionC0,0xc0},{p.frictionByMaterial[0],0xd8}})need(bits(pair.first)==e[std::to_string(pair.second)].get<std::uint32_t>(),"original setter bits");
  need(bits(p.impact.forceTorqueScaleB4)==bits(a.values[8])&&bits(p.impact.angularImpulseScaleB0)==bits(a.values[9])&&bits(p.frictionTorqueScaleC4)==bits(a.values[10]),"direct loader fields");
  SpringGeneratedParameters48K restored;std::memcpy(&restored,&p,sizeof p);restored.impact.forceTorqueScaleB4=before.impact.forceTorqueScaleB4;restored.impact.angularImpulseScaleB0=before.impact.angularImpulseScaleB0;restored.frictionTorqueScaleC4=before.frictionTorqueScaleC4;restored.impact.restitutionB8=before.impact.restitutionB8;restored.impact.restitutionBC=before.impact.restitutionBC;restored.impact.restitutionC0=before.impact.restitutionC0;restored.frictionByMaterial[0]=before.frictionByMaterial[0];need(!std::memcmp(&restored,&before,sizeof before),"unconsumed settings preserved");need(r.maxSpeed94==beforeR.maxSpeed94&&r.secondaryAtLimit60==beforeR.secondaryAtLimit60,"unwritten response fields preserved");
  SpringGeneratedParameters48K saved;SpringResponseParameters48L savedR;std::memcpy(&saved,&p,sizeof p);std::memcpy(&savedR,&r,sizeof r);hot=true;const bool invalid=applySpringCollision50F(a,std::numeric_limits<float>::infinity(),p,r);hot=false;need(!invalid&&!std::memcmp(&saved,&p,sizeof p)&&!std::memcmp(&savedR,&r,sizeof r),"invalid arithmetic atomic");
 }
 need(allocations==0,"zero allocation");std::cout<<J{{"setter_cases",count},{"checks",checks},{"hot_allocations",allocations}}.dump()<<'\n';return 0;
}catch(const std::exception&e){hot=false;std::cerr<<e.what()<<'\n';return 1;}}
