#pragma once
// ONE [ParticleEmitters] reader for every course TOB (SLUS_202.68). Every course
// owner that places scene particle emitters reads its descriptor table here; no
// course keeps its own pinned parse. Courses differ only in the TOB they hand in.
//
// Original, traced in work\scratch\dis_all.txt:
//  27DE00 ParticleEmitters::Load: 157D00 selects [ParticleEmitters]; a miss logs
//         "No Particle Emitters defined." and loads nothing. 158B30 reads
//         NumberOfEmitters (default 0). For i=1..N it formats "ParticleEmitter%d"
//         and selects it; a missing block logs "%s not defined." and is SKIPPED
//         (this list checks the 157D00 result, unlike the sound lists). The end
//         logs "Read %d particle emitters out of a total of %d."
//  27C600 descriptor fields: StartingZone (int, 0), TypeID (int, 0),
//         AttachToSoultree (string, default "NONE"). strcmp (3090E8) with "NONE":
//         equal -> Position (158C00, default zero 442F80), instances forced 1;
//         otherwise ChildObject (string, "NONE"), OffsetFromSoultree (vector,
//         zero), NumberOfInstances (int, 1). Then EmitterFilename (string,
//         "NONE") and StartOn (int, 1).
//  27C7B0 emitter load: EmitterFilename equal to "NONE" (308F00, case-insensitive)
//         -> "Unable to load particle emitter because no .emt filename is
//         specified." and the emitter is not loaded; NumberOfInstances <= 0 ->
//         "No need to load an emitter with no instances." (27CBF4), not loaded;
//         TypeID dispatch 0,1,2,8,9,11,12 (27C898..27C8E4); any other value ->
//         "Invalid particle type ID - %d" (27CBC4), not loaded.
// Integer fields follow 158B30 (strtol base 0, integer prefix, end ignored).
// Vector/decimal text uses the decimal conversion the course owners already
// used (std::from_chars double, rounded to float), so stock values stay
// bit-identical. PORT STAND-IN (flagged): text that conversion cannot read is
// refused instead of being read as a partial value.
//
// Storage is sized from the authored NumberOfEmitters (guard 4096) with a live count;
// a TOB that names more emitters than the store holds is refused with a log line
// before any row is written.
#include "import_world48f.h"
#include <array>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace rr::assets::particle_emitters50PE {
// Allocation guard only (stock maximum MC 40): the table is sized from the authored
// NumberOfEmitters (27DE00 allocates that count); a count above the guard is refused before any write.
inline constexpr std::size_t capacity=4096;
enum class Type:std::int32_t {Smoke=0,Dust=1,Spark=2,Fire=8,Explosion=9,Firework=11,Fog=12};
inline bool validType(std::int32_t t)noexcept{return t==0||t==1||t==2||t==8||t==9||t==11||t==12;}
enum class Unloaded:std::uint8_t {None,NoFilename,NoInstances,InvalidType};
struct Emitter {
 const ParameterBlock48F* block=nullptr;   // borrowed from the caller's document
 std::uint32_t ordinal=0;                  // N of ParticleEmitterN (1-based)
 std::int32_t startingZone=0,typeId=0,instances=1,startOn=1;
 std::string attachToSoulTree="NONE",childObject="NONE",filename="NONE";
 bool attached=false;                      // AttachToSoultree != "NONE" (3090E8)
 std::array<float,3> positionOrOffset{};   // Position, or OffsetFromSoultree when attached
 bool authoredStartOn=false,authoredInstances=false,authoredPositionOrOffset=false;
 Unloaded unloaded=Unloaded::None;         // 27C7B0 result: None = loaded
 bool loaded()const noexcept{return unloaded==Unloaded::None;}
};
struct Table {
 std::vector<Emitter> rows;   // sized once from NumberOfEmitters
 std::size_t authored=0;   // NumberOfEmitters
 std::size_t live=0;       // found ParticleEmitterN blocks, rows [0,live)
 std::size_t loadedCount=0,attachedCount=0,missing=0;
 bool absent=true;         // no [ParticleEmitters] block
 const Emitter* row(std::size_t i)const noexcept{return i<live?&rows[i]:nullptr;}
};
namespace detail {
inline const ParameterBlock48F* select(const ParameterDocument48F& p,const std::string& name)noexcept{
 for(const auto& b:p.blocks)if(b.name==name)return &b;return nullptr;   // labels are lowercase in the document
}
// 158648 first-present lookup.
inline const std::string* find(const ParameterBlock48F& b,const char* key)noexcept{
 for(const auto& f:b.fields)if(f.name==key)return &f.value;return nullptr;
}
inline bool sameNoCase(const std::string& a,const char* b)noexcept{
 const auto n=std::strlen(b);if(a.size()!=n)return false;
 for(std::size_t i=0;i<n;++i){auto x=a[i],y=b[i];if(x>='A'&&x<='Z')x=char(x+32);if(y>='A'&&y<='Z')y=char(y+32);if(x!=y)return false;}
 return true;
}
// 158B30 -> 158920: first character T / F (309538, n=1), then "ON" / "OFF"
// (308F00), then first character Y / N, else strtol base 0 (30AE70) with the
// end pointer ignored. So StartOn "T" reads 1 and "F" reads 0.
inline std::int32_t integer158B30(const std::string& text)noexcept{
 const char c=text.empty()?'\0':text[0];
 if(c=='T'||c=='t')return 1;if(c=='F'||c=='f')return 0;
 if(sameNoCase(text,"ON"))return 1;if(sameNoCase(text,"OFF"))return 0;
 if(c=='Y'||c=='y')return 1;if(c=='N'||c=='n')return 0;
 return static_cast<std::int32_t>(std::strtol(text.c_str(),nullptr,0));
}
inline bool decimal(const std::string& text,float& out)noexcept{
 double v=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),v);
 if(r.ec!=std::errc{}||r.ptr!=text.data()+text.size()||!std::isfinite(v))return false;
 out=static_cast<float>(v);return std::isfinite(out);
}
inline bool vector(const std::string& text,std::array<float,3>& out)noexcept{
 std::size_t begin=0;
 for(unsigned i=0;i<3;++i){const auto end=text.find(',',begin);
  if((end==std::string::npos)!=(i==2))return false;
  if(!decimal(text.substr(begin,end==std::string::npos?std::string::npos:end-begin),out[i]))return false;begin=end+1;
 }
 return true;
}
inline const char* typeName(std::int32_t t)noexcept{
 switch(t){case 0:return "Smoke";case 1:return "Dust";case 2:return "Spark";case 8:return "Fire";
  case 9:return "Explosion";case 11:return "Firework";case 12:return "Fog";default:return "Invalid";}
}
} // namespace detail
// Reads the table; `p` must outlive `out` (rows borrow its blocks). Returns
// false (and logs "particle_emitters site=.. refused: ..") only when the data
// does not fit the store or a value cannot be read; `out` is then empty.
// Absent block / 0 emitters returns true with live==0: no emitter owner.
inline bool read(const ParameterDocument48F& p,const char* site,Table& out){
 out=Table{};const char* s=site?site:"?";
 auto refuse=[&](const char* why,std::size_t n){
  std::cerr<<"particle_emitters site="<<s<<" emitter="<<n<<" refused: "<<why<<"\n";out=Table{};return false;};
 const auto* list=detail::select(p,"particleemitters");
 if(!list){std::cerr<<"particle_emitters site="<<s<<" authored=0 read=0 absent=1\n";return true;}
 Table next;next.absent=false;
 const auto* count=detail::find(*list,"numberofemitters");
 const auto authored=count?detail::integer158B30(*count):0;
 if(authored<0)return refuse("NumberOfEmitters below zero",0);
 if(static_cast<std::size_t>(authored)>capacity)return refuse("NumberOfEmitters exceeds the emitter store capacity",0);
 next.authored=static_cast<std::size_t>(authored);
 next.rows.assign(next.authored,Emitter{});
 std::string types;
 for(std::size_t i=0;i<next.authored;++i){
  const auto label="particleemitter"+std::to_string(i+1);const auto* b=detail::select(p,label);
  if(!b){++next.missing;std::cerr<<"particle_emitters site="<<s<<" "<<label<<" not defined (skipped, 27DF00)\n";continue;}
  if(next.live>=next.rows.size())return refuse("emitter rows exceed the authored count",i+1);
  auto& e=next.rows[next.live];e=Emitter{};e.block=b;e.ordinal=static_cast<std::uint32_t>(i+1);
  if(const auto* v=detail::find(*b,"startingzone"))e.startingZone=detail::integer158B30(*v);
  if(const auto* v=detail::find(*b,"typeid"))e.typeId=detail::integer158B30(*v);
  if(const auto* v=detail::find(*b,"attachtosoultree"))e.attachToSoulTree=*v;
  e.attached=e.attachToSoulTree!="NONE";
  if(!e.attached){
   if(const auto* v=detail::find(*b,"position")){e.authoredPositionOrOffset=true;if(!detail::vector(*v,e.positionOrOffset))return refuse("Position text",i+1);}
   e.instances=1;
  }else{
   if(const auto* v=detail::find(*b,"childobject"))e.childObject=*v;
   if(const auto* v=detail::find(*b,"offsetfromsoultree")){e.authoredPositionOrOffset=true;if(!detail::vector(*v,e.positionOrOffset))return refuse("OffsetFromSoultree text",i+1);}
   if(const auto* v=detail::find(*b,"numberofinstances")){e.authoredInstances=true;e.instances=detail::integer158B30(*v);}
  }
  if(const auto* v=detail::find(*b,"emitterfilename"))e.filename=*v;
  if(const auto* v=detail::find(*b,"starton")){e.authoredStartOn=true;e.startOn=detail::integer158B30(*v);}
  // 27C7B0 load order: filename, instances, type.
  if(detail::sameNoCase(e.filename,"NONE")){e.unloaded=Unloaded::NoFilename;
   std::cerr<<"particle_emitters site="<<s<<" "<<label<<" not loaded: no .emt filename\n";}
  else if(e.instances<=0){e.unloaded=Unloaded::NoInstances;
   std::cerr<<"particle_emitters site="<<s<<" "<<label<<" not loaded: no instances\n";}
  else if(!validType(e.typeId)){e.unloaded=Unloaded::InvalidType;
   std::cerr<<"particle_emitters site="<<s<<" "<<label<<" not loaded: invalid particle type ID "<<e.typeId<<"\n";}
  if(e.loaded()){++next.loadedCount;if(e.attached)++next.attachedCount;}
  if(next.live)types+=',';types+=detail::typeName(e.typeId);
  ++next.live;
 }
 std::cerr<<"particle_emitters site="<<s<<" authored="<<next.authored<<" read="<<next.live<<" loaded="<<next.loadedCount
          <<" attached="<<next.attachedCount<<" types="<<(next.live?types:std::string("-"))<<"\n";
 out=std::move(next);return true;
}
} // namespace rr::assets::particle_emitters50PE
