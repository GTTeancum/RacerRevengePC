#pragma once
// Common track-animation owner for one course (ANIM.md piece 5, the one owner the race world holds):
// the course archive's own TOB -> reader -> source -> runtime. Absent [Animations] (or
// NumberOfAnimations 0) = no owner (the original deletes its DynamicModels container, 2713E0).
// Logs one line per course and one per row (muted logs-only tests read them).
#include "track_animation_runtime50an.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/dlc_files50dl.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace track_animation_course50AN {
namespace a=rr::assets;namespace g=rr::game;namespace rt=track_animation_runtime50AN;namespace src=track_animation_source50AN;
struct Loaded {std::shared_ptr<rt::Owner> runtime;src::Extract extract;};
// archivePath: the course RES; code: the course code (log label and tracks\<code>.tob member);
// zoneCount: the loaded world's zones. Throws std::runtime_error with a plain message on refusal.
inline Loaded load(const std::filesystem::path& archivePath,const char* code,std::size_t zoneCount,
    g::SourceLibmState49T& libm,g::ModelMotionSharedState49T& shared){
 Loaded out;std::string label=code?code:"??";std::string lower=label;for(auto& c:lower)if(c>='A'&&c<='Z')c=char(c-'A'+'a');
 auto archive=std::make_shared<a::ResArchive48C>();
 {std::ifstream f(rr::assets::dlc50DL::host(archivePath),std::ios::binary); // through the DLC overlay, as every course-archive open
  if(!f)throw std::runtime_error("track animation course archive unreadable");
  const a::DecodeLimits48C limits{};std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
  if(bytes.size()>limits.resource_file_bytes)throw std::runtime_error("track animation course archive above the RES limit");
  a::DecodeError48C e{};if(!a::decodeResource48C(bytes,*archive,e,nullptr,limits))throw std::runtime_error("track animation course archive rejected: "+e.code);}
 std::vector<std::uint8_t> tob;a::DecodeError48C e{};
 if(!a::extractResMember48C(*archive,"tracks\\"+lower+".tob",tob,e)||tob.empty())throw std::runtime_error("track animation course TOB missing");
 a::ParameterDocument48F document;a::WorldLimits48F wl{};wl.retainDuplicateLabels=true; // the reader refuses duplicates it consumes
 if(!a::decodeParameters48F(tob,document,e,nullptr,wl))throw std::runtime_error("track animation course TOB rejected: "+e.code);
 out.extract=[archive](const std::string& name,std::vector<std::uint8_t>& bytes,std::size_t limit){
  a::DecodeError48C x{};return a::extractResMember48C(*archive,name,bytes,x,limit);};
 auto source=std::make_shared<src::Owner>();src::Diagnostic sd{};
 if(!source->load(document,static_cast<std::uint32_t>(zoneCount),out.extract,libm,sd)){
  std::cerr<<"track_animations course="<<label<<" refused: "<<sd.message<<" row="<<sd.row<<" member="<<sd.member<<'\n';
  throw std::runtime_error("track animation source rejected: "+sd.message);}
 if(!source->rowCount()){std::cerr<<"track_animations course="<<label<<" table=0 owner=absent\n";out.extract=nullptr;return out;}
 auto runtime=std::make_shared<rt::Owner>();rt::Diagnostic d{};
 if(!runtime->create(source,shared,label,d)){
  std::cerr<<"track_animations course="<<label<<" create refused: "<<d.message<<" row="<<(d.row+1)<<'\n';
  throw std::runtime_error("track animation runtime rejected: "+d.message);}
 runtime->logTable(std::cerr);runtime->retainExtract(out.extract);out.runtime=std::move(runtime);return out;
}
} // namespace track_animation_course50AN
