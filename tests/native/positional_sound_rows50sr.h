#pragma once
// Native fixed-position soul-tree rows from the ONE common AUB reader
// (src/assets/original_positional_sounds50sr.h), for every course owner
// (TB, MC, RC; GB once helper 145a wires it). One place for the native
// admission rules; the stock AUB identity and counts are receipts only.
#include "course_archive_receipt50ci.h"
#include "../../src/assets/original_positional_sounds50sr.h"
#include "../../src/assets/audio_assets50hc.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace positional_sound_rows50SR {
namespace a=rr::assets;
// Largest AUB the reader admits (decodeParameters48F's own input cap).
inline constexpr std::size_t maximumAubBytes=2u*1024u*1024u;
struct Stock {const char* code;const char* member;std::size_t aubBytes;const char* aubSha;
 std::size_t rows;std::size_t pcm;};
// Reads `aub` (nullptr = the course archive holds no AUB member) and projects
// the soul-tree list into `rows[0,live)`. Returns live; 0 = no positional
// owner. Throws std::runtime_error on a refusal. Descriptor must have the
// TB/MC/RC/GB fields name, position[4], nearDistance, farDistance,
// triggerDistance, chance, loop, oneShot, resetWhenInaudible.
// `ambient`: an owner that retains the AUB's ambient list itself (GB) passes
// its store; otherwise an authored ambient list is refused (no scheduler).
template<class Descriptor,std::size_t Capacity>
std::size_t project(const std::vector<std::uint8_t>* aub,const Stock& stock,
    std::array<Descriptor,Capacity>& rows,std::vector<a::AudioRequest50HC>& requests,
    a::OriginalSoundList50SR<a::ambientSoundCapacity50SR>* ambient=nullptr){
 static_assert(Capacity==a::soulTreeSoundCapacity50SR,"owner rows are sized by the common reader capacity");
 const auto site=std::string("positional_")+stock.code;
 auto refuse=[&](const std::string& why){throw std::runtime_error(std::string(stock.code)+" positional sound: "+why);};
 requests.clear();if(ambient)ambient->clear();
 if(!aub){std::cerr<<"positional_sounds site="<<site<<" aub=absent soultree_read=0\n";return 0;}
 course_archive_receipt50CI::memberReceipt(site.c_str(),stock.member,*aub,0,0,stock.aubBytes,stock.aubSha);
 a::ParameterDocument48F doc;a::DecodeError48C de{};
 if(!a::decodeParameters48F(*aub,doc,de))refuse("AUB authoring rejected");
 auto read=std::make_unique<a::OriginalPositionalSounds50SR>();std::string error;
 if(!a::readOriginalPositionalSounds50SR(doc,site.c_str(),*read,error))refuse(error);
 const auto& list=read->soulTree;
 // PORT STAND-IN (flagged): the native owners have no ambient scheduler
 // (0x2793A0 list, CycleTime 0x278D70); an authored ambient list is refused
 // here, not ignored. GB's owner retains its ambient rows itself.
 if(ambient)*ambient=read->ambient;
 else if(read->ambient.live)refuse("ambient sound scheduling is not supported natively (PORT STAND-IN)");
 course_archive_receipt50CI::countReceipt(site.c_str(),"soultree_sounds",list.live,stock.rows);
 for(std::size_t i=0;i<list.live;++i){const auto& s=list.rows[i];auto& row=rows.at(i);
  const auto label="SoultreeSound"+std::to_string(s.ordinal)+" ";
  // PORT STAND-IN (flagged): attached, 2D, stereo or start-disabled soul-tree
  // sounds are not implemented by the fixed-position native adapter.
  if(!s.attachToObject.empty()||s.force2D||s.stereo||s.startDisabled)
   refuse(label+"AttachToObject/Force2D/Stereo/StartDisabled is not supported natively (PORT STAND-IN)");
  // PORT STAND-IN (flagged): the original default Position (0x442F80) is
  // not traced; an omitted Position is refused.
  if(!s.positionAuthored)refuse(label+"omitted Position is not supported natively (PORT STAND-IN)");
  // PORT MAPPING (flagged): the original opens resource+".wav" (308FB8); the
  // port's audio bank holds the disc's VAG file of the same name.
  row=Descriptor{};row.name=s.resource;for(auto& c:row.name)if(c=='\\')c='/';row.name+=".vag";
  for(unsigned k=0;k<3;++k)row.position[k]=s.position[k];
  row.loop=s.looping!=0;row.oneShot=s.oneShot!=0;row.resetWhenInaudible=s.resetWhenInaudible!=0;
  row.nearDistance=s.minDistance;row.farDistance=s.maxDistance;row.triggerDistance=s.oneShotDistance;row.chance=s.randomTriggerPercent;
  if(!(row.nearDistance>=0&&row.nearDistance<row.farDistance&&row.triggerDistance>=0&&row.chance>=0&&row.chance<=100))
   refuse(label+"ranges rejected");
  auto found=std::find_if(requests.begin(),requests.end(),[&](const auto& q){return q.name==row.name;});
  if(found==requests.end())requests.push_back({row.name,row.loop,false,false});
  else if(found->loop!=row.loop)refuse("PCM delivery flags conflict");
 }
 course_archive_receipt50CI::countReceipt(site.c_str(),"soultree_pcm",requests.size(),stock.pcm);
 return list.live;
}
} // namespace positional_sound_rows50SR
