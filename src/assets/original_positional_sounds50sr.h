#pragma once
// ONE reader for the course AUB's [Sounds] soul-tree and ambient sound lists,
// every course (TA..RA, converted tracks). It follows the original scene
// sound load (SLUS_202.68):
//  - 0x279910 SoultreeSounds::Load: 157D00 select [Sounds]; 158B30
//    NumSoultreeSounds (default 0; 0 = no list, return 0); for i=1..N
//    sprintf "SoultreeSound%d" (308C90) and 157D00 WITHOUT checking the
//    result, then the descriptor's own load (vtable+8).
//  - 0x2793A0 AmbientSounds::Load: same with NumAmbientSounds / "AmbientSound%d".
//  - 157D00 miss keeps the previous selection (15821C): the descriptor then
//    re-reads the previously selected block, i.e. a duplicate of the previous
//    sound, or [Sounds] itself for a miss before any sound block (which has no
//    SoundResourceFile, so that load fails).
//  - 0x2787B0 base sound fields: SoundResourceFile (158AD0 string; absent =
//    load fails, 0x278E00 then drops the object), strcat ".wav" (308FB8, string
//    at 0x3BAE08), Position (158C00, default vector 0x442F80), Looping,
//    OneShot, ResetWhenInaudible, Force2D, Stereo (158B30, default 0),
//    OneShotDistance (158B80, default 20.0), RandomTriggerPercent (default
//    100.0), MaxDistance, MinDistance (default 0.0), Priority (158B30, default 10).
//  - 0x278B80 soul-tree extras: AttachToObject (string), StartDisabled (int 0).
//  - 0x278D70 ambient extra: CycleTime (158B80, default 0.0).
//  - 158B30 -> 158920 integer text: first char T -> 1, F -> 0, "ON"/"OFF",
//    first char Y -> 1, N -> 0, else strtol base 0 prefix (30AE70).
// 0x271600 (the same [Sounds] block's NumMaterials / DefaultMaterial /
// CollisionObjectsMaterial%d) is the material audio reader, not this one.
//
// Capacity, not a stock count: the stores hold the largest stock lists (SC 199
// soul-tree sounds; GB 4 ambient sounds) with the live count; a larger list is
// refused (bounds-checked before any write). This is the ONE place these
// capacities are defined. Each native owner bounds or refuses what it does
// not implement itself.
#include "import_world48f.h"
#include "track_start50ad.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace rr::assets {
inline constexpr std::size_t soulTreeSoundCapacity50SR=199,ambientSoundCapacity50SR=4;
struct OriginalSound50SR {
 std::uint32_t ordinal=0;               // authored 1-based index i
 const ParameterBlock48F* block=nullptr;// block actually read (selection)
 bool repeatedSelection=false;          // 157D00 missed: previous selection re-read
 std::string resource;                  // SoundResourceFile as authored
 std::string file;                      // resource + ".wav" (308FB8)
 std::array<float,3> position{};bool positionAuthored=false;
 std::int32_t looping=0,oneShot=0,resetWhenInaudible=0,force2D=0,stereo=0,priority=10;
 float oneShotDistance=20.f,randomTriggerPercent=100.f,maxDistance=0.f,minDistance=0.f;
 std::string attachToObject;std::int32_t startDisabled=0; // soul-tree (0x278B80)
 float cycleTime=0.f;                                      // ambient (0x278D70)
};
template<std::size_t Capacity> struct OriginalSoundList50SR {
 std::uint32_t authored=0;   // NumSoultreeSounds / NumAmbientSounds
 std::size_t live=0;         // loaded descriptors in rows[0,live)
 std::size_t dropped=0,repeated=0;
 std::array<OriginalSound50SR,Capacity> rows{};
 static constexpr std::size_t capacity()noexcept{return Capacity;}
 void clear()noexcept{authored=0;live=0;dropped=repeated=0;}
};
struct OriginalPositionalSounds50SR {
 bool soundsBlock=false;
 OriginalSoundList50SR<soulTreeSoundCapacity50SR> soulTree;
 OriginalSoundList50SR<ambientSoundCapacity50SR> ambient;
 // Large (capacity rows): hold it on the heap, not the stack.
};
namespace positional_sounds50SR_detail {
inline const ParameterBlock48F* find(const ParameterDocument48F& d,const std::string& name)noexcept{
 for(const auto& b:d.blocks)if(b.name==name)return &b;return nullptr;
}
// 158240: the first field with this key in the selected block.
inline const std::string* field(const ParameterBlock48F* b,const char* key)noexcept{
 if(!b)return nullptr;for(const auto& f:b->fields)if(f.name==key)return &f.value;return nullptr;
}
inline int folded(char c)noexcept{return c>='a'&&c<='z'?c-'a'+'A':c;}
inline bool same(const std::string& a,const char* b)noexcept{
 std::size_t i=0;for(;b[i];++i)if(i>=a.size()||folded(a[i])!=folded(b[i]))return false;return i==a.size();
}
// 158920.
inline std::int32_t integer158920(const std::string& t)noexcept{
 if(!t.empty()&&folded(t[0])=='T')return 1;if(!t.empty()&&folded(t[0])=='F')return 0;
 if(same(t,"ON"))return 1;if(same(t,"OFF"))return 0;
 if(!t.empty()&&folded(t[0])=='Y')return 1;if(!t.empty()&&folded(t[0])=='N')return 0;
 return static_cast<std::int32_t>(std::strtol(t.c_str(),nullptr,0));
}
inline std::int32_t integer(const ParameterBlock48F* b,const char* key,std::int32_t fallback)noexcept{
 const auto* v=field(b,key);return v?integer158920(*v):fallback;
}
// 158A70 (atof + narrowing). PORT STAND-IN (flagged): the port's bounded
// decimal route (parseTrackNumber50AD) is used and a text it cannot read is
// refused, not prefix-read.
inline bool number(const ParameterBlock48F* b,const char* key,float fallback,float& out)noexcept{
 const auto* v=field(b,key);if(!v){out=fallback;return true;}
 return parseTrackNumber50AD(v->data(),v->size(),out)&&std::isfinite(out);
}
inline bool vector(const std::string& t,std::array<float,3>& out)noexcept{
 std::size_t begin=0;
 for(unsigned axis=0;axis<3;++axis){const auto end=t.find(',',begin);
  if((axis<2)!=(end!=std::string::npos))return false;
  const auto last=end==std::string::npos?t.size():end;
  if(!parseTrackNumber50AD(t.data()+begin,last-begin,out[axis])||!std::isfinite(out[axis]))return false;begin=last+1;}
 return true;
}
// 0x2787B0 (+0x278B80 / +0x278D70). Returns false when the original load
// fails (no SoundResourceFile): the wrapper drops it. Throws nothing; a field
// text the port cannot read sets `bad`.
inline bool load(const ParameterBlock48F* b,bool soulTree,OriginalSound50SR& s,const char*& bad)noexcept{
 const auto* resource=field(b,"soundresourcefile");if(!resource)return false;
 s.resource=*resource;s.file=*resource+".wav";
 if(const auto* p=field(b,"position")){
  if(!vector(*p,s.position)){bad="Position";return true;}s.positionAuthored=true;
 }
 s.looping=integer(b,"looping",0);s.oneShot=integer(b,"oneshot",0);s.resetWhenInaudible=integer(b,"resetwheninaudible",0);
 s.force2D=integer(b,"force2d",0);s.stereo=integer(b,"stereo",0);
 if(!number(b,"oneshotdistance",20.f,s.oneShotDistance)){bad="OneShotDistance";return true;}
 if(!number(b,"randomtriggerpercent",100.f,s.randomTriggerPercent)){bad="RandomTriggerPercent";return true;}
 if(!number(b,"maxdistance",0.f,s.maxDistance)){bad="MaxDistance";return true;}
 if(!number(b,"mindistance",0.f,s.minDistance)){bad="MinDistance";return true;}
 s.priority=integer(b,"priority",10);
 if(soulTree){if(const auto* a=field(b,"attachtoobject"))s.attachToObject=*a;s.startDisabled=integer(b,"startdisabled",0);}
 else if(!number(b,"cycletime",0.f,s.cycleTime)){bad="CycleTime";return true;}
 return true;
}
template<std::size_t C> bool list(const ParameterDocument48F& d,const ParameterBlock48F* sounds,const char* countKey,
    const char* prefix,bool soulTree,OriginalSoundList50SR<C>& out,std::string& error){
 out.clear();
 const auto n=integer(sounds,countKey,0);
 if(n<=0)return true;
 out.authored=static_cast<std::uint32_t>(n);
 const ParameterBlock48F* selected=sounds; // [Sounds] stays selected until a hit
 for(std::uint32_t i=1;i<=out.authored;++i){
  const auto* hit=find(d,prefix+std::to_string(i));
  const bool repeated=!hit;if(hit)selected=hit;
  OriginalSound50SR s;s.ordinal=i;s.block=selected;s.repeatedSelection=repeated;const char* bad=nullptr;
  if(!load(selected,soulTree,s,bad)){++out.dropped;continue;}
  if(bad){error=std::string(prefix)+std::to_string(i)+" "+bad+" text rejected";return false;}
  // Bounds check before the write: a list above the store is refused.
  if(out.live>=out.rows.size()){error=std::string(countKey)+" exceeds the native sound store capacity";return false;}
  out.repeated+=repeated;out.rows[out.live++]=std::move(s);
 }
 return true;
}
}
// Reads both lists. `site` names the owner for the one log line. Absent
// [Sounds] block or zero counts = empty lists (no positional owner).
// PORT STAND-IN (flagged): with [Sounds] absent the original's 158B30 would
// read the count from whatever block was selected before (untraced); the port
// reads no list.
inline bool readOriginalPositionalSounds50SR(const ParameterDocument48F& d,const char* site,
    OriginalPositionalSounds50SR& out,std::string& error){
 namespace x=positional_sounds50SR_detail;
 out.soundsBlock=false;out.soulTree.clear();out.ambient.clear();error.clear();
 const auto* sounds=x::find(d,"sounds");out.soundsBlock=sounds!=nullptr;
 const bool ok=sounds&&
  x::list(d,sounds,"numsoultreesounds","soultreesound",true,out.soulTree,error)&&
  x::list(d,sounds,"numambientsounds","ambientsound",false,out.ambient,error);
 if(sounds&&!ok){
  std::cerr<<"positional_sounds site="<<(site?site:"?")<<" refused: "<<error<<"\n";
  out.soulTree.clear();out.ambient.clear();return false;
 }
 // Distinct PCM files of the loaded soul-tree descriptors (authored order).
 std::size_t pcm=0;
 for(std::size_t i=0;i<out.soulTree.live;++i){bool seen=false;
  for(std::size_t j=0;j<i;++j)if(out.soulTree.rows[j].file==out.soulTree.rows[i].file)seen=true;pcm+=!seen;}
 std::cerr<<"positional_sounds site="<<(site?site:"?")<<" soultree_authored="<<out.soulTree.authored
          <<" soultree_read="<<out.soulTree.live<<" soultree_repeated="<<out.soulTree.repeated
          <<" soultree_dropped="<<out.soulTree.dropped<<" ambient_authored="<<out.ambient.authored
          <<" ambient_read="<<out.ambient.live<<" pcm="<<pcm<<"\n";
 return true;
}
} // namespace rr::assets
