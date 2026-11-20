#pragma once
// Common rewrite (R5, PLAN B-C13 / 2.2 "animation, ATM, UV"): the ONE animated-
// texture (ATM) reader of the race path, for every course. Replaces the 21
// per-course *_atm_{animation,window}_fixture.h owners, their generated
// *_atm_*_index.inc pin tables and ra_atm_authoring50la.h.
//
// What it reads is the course's own authored data, nothing else:
//   * each static-world material pass whose texture is an .ATM (the selection
//     the single-pass decoder marked staticAnimationFrame, R3's staticWorld),
//     with its authored pass fields: TextureAnimation, animatedMapInitialFrame,
//     animatedMapFramesPerSecond, animatedMapLooping, animatedMapReverseLoopOnEnd,
//     animatedMapLoopOnEndToFrame, wrapModeHorizontal/Vertical (29B950 defaults);
//   * the .ATM member itself (102230 storage 0: u32 revision 1, u32 count,
//     u32 selector 0, then `count` consecutive PST streams of kind 0/1 indexed
//     or kind 2 IPUM), every frame decoded natively to RRTEX at cold;
//   * the spans that draw those selections (one animation row per span).
// The playback is the admitted original 29A020 forward-loop subset
// (rr::game::advanceAuthoredTextureAnimation). PORT STAND-IN (flagged): a pass
// authoring reverse-loop, a LoopOnEndToFrame other than 0, non-looping, or a
// layered (multi-pass) placement is refused with its reason, not mis-played.
//
// No course code, archive SHA, member size or frame count is pinned: the stock
// counts are receipts (stock_course_receipts50CR "atm.sources" / "atm.frames" /
// "atm.animations"), logged by logTrackData. Storage is sized from the counts
// read here; the allocation ceilings below are logged guards, not sizes.
#include "common_track50cr.h"
#include "stock_course_receipts50cr.h"
#include "track_materials50fa_fixture.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/original_ipu_decoder.h"
#include "../../src/assets/material_texture_assets50ea.h"
#include "../../src/game/authored_texture_animation.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace track_atm50CR {
namespace a=rr::assets;namespace p=rr::platform;namespace ct=common_track50CR;namespace tm=track_materials50fa_fixture;
// PORT STAND-IN (flagged): allocation guards against a corrupt file, logged on
// refusal. 64 frames per ATM is the 29A020/102230 subset's own bound
// (advanceAuthoredTextureAnimation refuses more).
struct Ceilings {
 std::uint32_t framesPerAtm=64,frames=4096,animations=65536;
 std::size_t atmBytes=8u*1024u*1024u,rgbaBytes=256u*1024u*1024u;
};
struct Diagnostic {
 std::string member,message;
 std::uint32_t selection=a::unresolvedMaterialTexture50EA,frame=a::unresolvedMaterialTexture50EA;
 a::DecodeError48C decode{};
 a::MaterialTextureAssetDiagnostic50EA bank{};
};
struct Animation {
 std::uint32_t sourceSpanIndex=0,worldInstanceIndex=0,materialSelection=0;
 std::uint32_t sourceTextureIndex=a::unresolvedMaterialTexture50EA;
 std::uint32_t firstTextureIndex=0,frameCount=0,initialFrame=0;
 float framesPerSecond=0.f;
 bool clampU=false,clampV=false;
};
struct Sampling {bool clampU=false,clampV=false;};
struct Counts {std::uint32_t sources=0,frames=0,animations=0;};

namespace detail {
inline void need(bool ok,const std::string& why){if(!ok)throw std::runtime_error(why);}
inline std::uint32_t u32(const std::uint8_t* b)noexcept{
 return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);}
inline std::string normalized(std::string s){for(auto& c:s){if(c=='\\')c='/';else if(c>='A'&&c<='Z')c=char(c-'A'+'a');}return s;}
// Authored pass field (selection.sourcePassFields {"authored":[{name,value}]}), 29B950 default when absent.
inline std::string field(const tm::Selection& s,const char* name,const char* fallback){
 const auto& f=s.sourcePassFields;std::string value=fallback;bool found=false;
 if(!f.is_object()||!f.contains("authored")||!f.at("authored").is_array())return value;
 for(const auto& row:f.at("authored")){
  if(!row.is_object()||!row.contains("name")||!row.contains("value")||row.at("name")!=name)continue;
  need(!found&&row.at("value").is_string(),std::string("repeated or non-text ATM pass field ")+name);
  value=row.at("value").get<std::string>();found=true;
 }
 return value;
}
inline float number(const std::string& text){
 std::istringstream in(text);in.imbue(std::locale::classic());float v=0;in>>v;
 need(bool(in)&&std::isfinite(v),"ATM pass field is not a finite number");in>>std::ws;need(in.eof(),"ATM pass field suffix");return v;
}
inline bool power(std::uint32_t n)noexcept{return n>=16&&n<=1024&&!(n&(n-1));}
struct Frame {std::size_t offset=0,bytes=0;std::uint32_t kind=0,mips=0,width=0,height=0;};
// 102230 storage 0: walk every PST stream by its own headers (no size division,
// no search). Kind 0/1: 12-byte header, 64/1024-byte palette, per mip 16-byte
// header + texels/2 or texels. Kind 2: 12-byte header, per mip 20-byte header +
// an ipum payload. The walk must consume the member exactly.
inline std::vector<Frame> walk(const std::vector<std::uint8_t>& atm,std::uint32_t frameCeiling){
 const auto range=[&](std::size_t at,std::size_t n){return at<=atm.size()&&n<=atm.size()-at;};
 const auto word=[&](std::size_t at){need(range(at,4),"truncated ATM");return u32(atm.data()+at);};
 need(word(0)==1,"ATM revision is not 1");
 const auto count=word(4);need(count>=1&&count<=frameCeiling,"ATM frame count outside 1..frame ceiling");
 need(word(8)==0,"ATM selector nonzero (separate TGA frame path, not implemented)");
 std::vector<Frame> frames;frames.reserve(count);std::size_t at=12;
 for(std::uint32_t f=0;f<count;++f){
  Frame fr;fr.offset=at;need(range(at,12),"truncated ATM PST header");
  need((word(at)&0xffffu)==1u,"ATM PST revision is not 1");
  fr.kind=word(at+4);fr.mips=word(at+8);need(fr.mips>=1&&fr.mips<=7,"ATM PST mip count outside 1..7");
  std::uint32_t pw=0,ph=0;
  if(fr.kind==0||fr.kind==1){
   at+=12+(fr.kind==0?64u:1024u);need(range(at,0),"truncated ATM PST palette");
   for(std::uint32_t m=0;m<fr.mips;++m){
    const auto h=word(at),w=word(at+4);
    need(power(w)&&power(h)&&!word(at+8)&&!word(at+12)&&(!m||(w==pw*2&&h==ph*2)),"ATM PST mip extent/order");
    const std::size_t texels=std::size_t(w)*h,bytes=fr.kind==0?texels/2:texels;
    need(range(at+16,bytes),"truncated ATM PST pixels");at+=16+bytes;pw=w;ph=h;
   }
  }else if(fr.kind==2){
   at+=12;
   for(std::uint32_t m=0;m<fr.mips;++m){
    need(range(at,20),"truncated ATM IPUM mip header");
    const auto h=word(at),w=word(at+4),bytes=word(at+16);
    need(power(w)&&power(h)&&!word(at+8)&&!word(at+12)&&(!m||(w==pw*2&&h==ph*2))&&bytes>=25&&range(at+20,bytes),
        "ATM IPUM mip extent/order");
    at+=20+bytes;pw=w;ph=h;
   }
  }else need(false,"ATM PST kind outside 0..2");
  fr.bytes=at-fr.offset;fr.width=pw;fr.height=ph;frames.push_back(fr);
 }
 need(at==atm.size(),"ATM bytes after the authored frame count");
 return frames;
}
// One frame to RRTEX001 (largest stored mip): kind 0/1 through decodeTexture48E,
// kind 2 through the native IPUM decoder.
inline std::vector<std::uint8_t> decode(const std::vector<std::uint8_t>& atm,const Frame& fr,a::DecodeError48C& e){
 std::vector<std::uint8_t> pst(atm.begin()+static_cast<std::ptrdiff_t>(fr.offset),atm.begin()+static_cast<std::ptrdiff_t>(fr.offset+fr.bytes));
 std::vector<std::uint8_t> out;
 if(fr.kind==2){
  std::size_t at=12,payload=0,payloadBytes=0;
  for(std::uint32_t m=0;m<fr.mips;++m){const auto bytes=u32(pst.data()+at+16);payload=at+20;payloadBytes=bytes;at=payload+bytes;}
  need(!std::memcmp(pst.data()+payload,"ipum",4),"ATM IPUM payload tag");
  a::TextureMip48E mip;mip.width=fr.width;mip.height=fr.height;std::string why;
  need(a::decodeOriginalIpuRgba(pst.data()+payload,payloadBytes,fr.width,fr.height,mip.rgba,why),"ATM IPUM decode: "+why);
  need(a::encodeTextureLevel48E(mip,out,e),"ATM IPUM output framing");
 }else{
  a::Texture48E t;
  need(a::decodeTexture48E(pst,t,e)&&t.mips.size()==fr.mips&&t.mips.back().width==fr.width&&t.mips.back().height==fr.height,
      "ATM indexed frame decode/identity");
  out=std::move(t.base_rrtex);
 }
 need(out.size()==std::size_t(fr.width)*fr.height*4+16&&!std::memcmp(out.data(),"RRTEX001",8)&&
     u32(out.data()+8)==fr.width&&u32(out.data()+12)==fr.height,"ATM decoded frame framing");
 return out;
}
} // namespace detail

class Owner final {
 struct Storage {
  a::MaterialTextureAssets50EA bank;
  std::vector<Animation> animations;
  std::vector<rr::game::AuthoredTextureAnimation> state,scratchState;
  std::vector<std::uint32_t> selectedFrames,scratchFrames;
  std::vector<Sampling> sampling;
  Counts counts;
 };
 std::unique_ptr<Storage> storage_;
 p::MaterialTextureGl50EB gpu_;
 std::vector<p::VehicleWindowTrackAnimation50DS> bindings_;
 bool attempted_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool loaded()const noexcept{return bool(storage_);}
 bool ownsResources()const noexcept{return gpu_.ownsResources();}
 const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
 const std::vector<Animation>* animations()const noexcept{return storage_?&storage_->animations:nullptr;}
 const std::vector<std::uint32_t>* selectedFrames()const noexcept{return storage_?&storage_->selectedFrames:nullptr;}
 const std::vector<Sampling>* sampling()const noexcept{return storage_?&storage_->sampling:nullptr;}
 Counts counts()const noexcept{return storage_?storage_->counts:Counts{};}
 const p::MaterialTextureGl50EB& gpu()const noexcept{return gpu_;}
 const std::vector<p::VehicleWindowTrackAnimation50DS>& bindings()const noexcept{return bindings_;}

 // Whether the static world's selections author any ATM pass (absent = no ATM owner).
 static bool authored(const std::vector<tm::Selection>& selections)noexcept{
  for(const auto& s:selections)if(s.staticAnimationFrame)return true;return false;
 }
 // Cold, one attempt. `world` is the Track's static world (roles.staticWorld:
 // spans(), selections(), bank()); its ATM members come from the Track's archive.
 template<class StaticWorld>
 bool loadSource(const ct::Track& track,const StaticWorld& world,Diagnostic& d,const Ceilings& ceilings=Ceilings{})noexcept {
  using detail::need;
  try{
   d={};need(!attempted_,"ATM owner is one-attempt");attempted_=true;
   const std::vector<tm::Selection>* selections=world.selections();const std::vector<tm::Span>* spans=world.spans();
   const a::MaterialTextureAssets50EA* staticBank=world.bank();
   need(selections&&spans&&staticBank,"static world selections/spans/bank unavailable");
   auto next=std::make_unique<Storage>();
   // One source per distinct .ATM member, in first-use order.
   struct Src {std::string member;std::vector<std::uint8_t> bytes;std::vector<detail::Frame> frames;std::uint32_t first=0;};
   std::vector<Src> sources;std::map<std::string,std::size_t> byMember;
   std::vector<std::uint32_t> selectionSource(selections->size(),a::unresolvedMaterialTexture50EA);
   std::vector<Animation> definitions(selections->size());
   std::size_t rgba=0;std::uint32_t frames=0;
   for(std::uint32_t si=0;si<selections->size();++si){
    const auto& s=(*selections)[si];if(!s.staticAnimationFrame)continue;d.selection=si;d.member=s.decodedSourceMember;
    const auto key=detail::normalized(s.decodedSourceMember);need(!key.empty(),"ATM selection names no source member");
    auto found=byMember.find(key);
    if(found==byMember.end()){
     Src src;src.member=s.decodedSourceMember;ct::Diagnostic md;
     need(track.archive.member(src.member,src.bytes,ceilings.atmBytes,md),"ATM member unreadable: "+md.reason);
     src.frames=detail::walk(src.bytes,ceilings.framesPerAtm);src.first=frames;
     need(src.frames.size()<=ceilings.frames-frames,"ATM frames above the frame ceiling");
     frames+=static_cast<std::uint32_t>(src.frames.size());
     for(const auto& f:src.frames){const auto b=std::size_t(f.width)*f.height*4;need(b<=ceilings.rgbaBytes-rgba,"ATM decoded RGBA above the ceiling");rgba+=b;}
     found=byMember.emplace(key,sources.size()).first;sources.push_back(std::move(src));
    }
    const auto& src=sources[found->second];
    // 29B950 defaults; the admitted 29A020 forward-loop subset.
    need(detail::field(s,"TextureAnimation","none")=="unique","ATM pass TextureAnimation other than unique");
    const auto initial=detail::number(detail::field(s,"animatedMapInitialFrame","0"));
    const auto fps=detail::number(detail::field(s,"animatedMapFramesPerSecond","1"));
    need(detail::field(s,"animatedMapLooping","1")=="1"&&detail::field(s,"animatedMapReverseLoopOnEnd","0")=="0"&&
        detail::number(detail::field(s,"animatedMapLoopOnEndToFrame","-1"))==0.f,
        "ATM pass outside the forward-loop subset (looping, no reverse, LoopOnEndToFrame 0)");
    need(initial>=0.f&&initial==std::floor(initial)&&initial<float(src.frames.size())&&fps>0.f&&fps<=60.f,"ATM pass initial frame/rate");
    auto& def=definitions[si];def.firstTextureIndex=src.first;def.frameCount=static_cast<std::uint32_t>(src.frames.size());
    def.initialFrame=static_cast<std::uint32_t>(initial);def.framesPerSecond=fps;def.clampU=s.clampU;def.clampV=s.clampV;
    selectionSource[si]=static_cast<std::uint32_t>(found->second);
   }
   d.selection=a::unresolvedMaterialTexture50EA;
   need(!sources.empty(),"no ATM pass authored (caller creates no ATM owner then)");
   // Decode every frame once, natively.
   std::vector<std::vector<std::uint8_t>> native(frames);std::vector<a::MaterialTextureAssetInput50EA> images(frames);
   next->sampling.resize(frames);std::vector<bool> samplerSet(frames,false);
   for(const auto& src:sources){d.member=src.member;
    for(std::uint32_t f=0;f<src.frames.size();++f){d.frame=f;const auto index=src.first+f;
     native[index]=detail::decode(src.bytes,src.frames[f],d.decode);images[index]={std::uint64_t(index)+1,&native[index]};}
   }
   d.frame=a::unresolvedMaterialTexture50EA;
   for(std::uint32_t si=0;si<selections->size();++si){if(selectionSource[si]==a::unresolvedMaterialTexture50EA)continue;
    const auto& def=definitions[si];
    for(std::uint32_t f=0;f<def.frameCount;++f){const auto i=def.firstTextureIndex+f;
     need(!samplerSet[i]||(next->sampling[i].clampU==def.clampU&&next->sampling[i].clampV==def.clampV),"shared ATM frames with different wrap policies");
     next->sampling[i]={def.clampU,def.clampV};samplerSet[i]=true;}
   }
   // One animation row per span drawing an ATM selection (the live one-pass subset).
   for(std::uint32_t si=0;si<spans->size();++si){const auto& span=(*spans)[si];
    if(span.materialSelection>=selections->size())continue;
    if(selectionSource[span.materialSelection]==a::unresolvedMaterialTexture50EA)continue;
    const auto& sel=(*selections)[span.materialSelection];
    need(sel.passCount==1&&span.sourcePass==1&&span.sourcePassCount==1&&span.projectionPass==0&&span.textureIndex==sel.textureIndex,
        "placed ATM span outside the single-pass live subset (layered ATM placement)");
    need(next->animations.size()<ceilings.animations,"ATM placed spans above the animation ceiling");
    auto row=definitions[span.materialSelection];row.sourceSpanIndex=si;row.worldInstanceIndex=span.worldInstanceIndex;
    row.materialSelection=span.materialSelection;row.sourceTextureIndex=sel.textureIndex;next->animations.push_back(row);
   }
   const auto n=next->animations.size();
   next->state.resize(n);next->scratchState.resize(n);next->selectedFrames.resize(n);next->scratchFrames.resize(n);
   for(std::size_t i=0;i<n;++i){next->state[i]={float(next->animations[i].initialFrame),0,false};next->selectedFrames[i]=next->animations[i].initialFrame;}
   const a::MaterialTextureBudgets50EA budget{frames,0,rgba+std::size_t(frames)*16,rgba,1024};
   const a::MaterialTextureAssetsInput50EA input{images.data(),frames,nullptr,0,budget};
   need(next->bank.load(input,d.bank),"ATM frame bank rejected");
   next->counts={static_cast<std::uint32_t>(sources.size()),frames,static_cast<std::uint32_t>(n)};
   std::cerr<<"track_atm course="<<track.code()<<" sources="<<sources.size()<<" frames="<<frames<<" animations="<<n<<'\n';
   storage_=std::move(next);return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}
   std::cerr<<"track_atm course="<<track.code()<<" refused member="<<d.member<<" reason="<<d.message<<'\n';return false;}
  catch(...){try{d.message="ATM allocation failure";}catch(...){}return false;}
 }
 bool createGpu(const p::MaterialTextureGlRequest50EB& caller,p::MaterialTextureGlDiagnostic50EB& d)noexcept {
  if(!storage_)return false;
  try{
   const auto& policies=storage_->sampling;
   std::vector<p::MaterialTextureSampler50EB> samplers(policies.size());
   for(std::size_t i=0;i<policies.size();++i)
    samplers[i]={p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
     policies[i].clampU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
     policies[i].clampV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat};
   std::vector<p::VehicleWindowTrackAnimation50DS> bindings;bindings.reserve(storage_->animations.size());
   for(const auto& an:storage_->animations){
    if(an.firstTextureIndex>policies.size()||an.frameCount>policies.size()-an.firstTextureIndex)return false;
    bindings.push_back({an.sourceSpanIndex,an.sourceTextureIndex,an.firstTextureIndex,an.frameCount});
   }
   auto request=caller;request.samplers=samplers.data();request.samplerCount=static_cast<std::uint32_t>(samplers.size());
   std::size_t bytes=0;for(std::uint32_t i=0;i<storage_->bank.textureCount();++i)if(const auto* t=storage_->bank.texture(i))bytes+=t->texture.rgba.size();
   request.maximumGpuBytes=bytes;
   if(!gpu_.create(request,storage_->bank,d))return false;
   bindings_=std::move(bindings);return true;
  }catch(...){return false;}
 }
 bool bindRequest(p::VehicleWindowRequest50DS& request)const noexcept {
  if(!storage_||!gpu_.initialized())return false;
  request.trackAnimationTextures=&gpu_;request.trackAnimations=bindings_.data();
  request.trackAnimationCount=static_cast<std::uint32_t>(bindings_.size());return true;
 }
 // Common accepted-world timestamp only; transactional (all rows or none).
 bool advance(std::uint32_t timestamp)noexcept {
  if(!storage_)return false;auto& s=*storage_;
  for(std::size_t i=0;i<s.animations.size();++i){
   s.scratchState[i]=s.state[i];s.scratchFrames[i]=s.selectedFrames[i];const auto& an=s.animations[i];
   if(!rr::game::advanceAuthoredTextureAnimation(s.scratchState[i],timestamp,an.framesPerSecond,an.frameCount,s.scratchFrames[i]))return false;
  }
  for(std::size_t i=0;i<s.animations.size();++i){s.state[i]=s.scratchState[i];s.selectedFrames[i]=s.scratchFrames[i];}
  return true;
 }
 bool bindFrame(p::VehicleWindowFrame50DS& frame)const noexcept {
  if(!storage_||!gpu_.initialized())return false;
  frame.trackAnimationFrames=storage_->selectedFrames.data();
  frame.trackAnimationFrameCount=static_cast<std::uint32_t>(storage_->selectedFrames.size());return true;
 }
 // Race start (fresh and 1AC8E0 restart): every row back to its authored initial frame.
 void reset()noexcept {
  if(!storage_)return;
  for(std::size_t i=0;i<storage_->animations.size();++i){
   storage_->state[i]={float(storage_->animations[i].initialFrame),0,false};storage_->selectedFrames[i]=storage_->animations[i].initialFrame;}
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
 // common_track_data atm.sources / atm.frames / atm.animations (receipts for a stock course).
 void logTrackData(std::string_view course,common_race_log50CR::Source source)const{
  namespace L=common_race_log50CR;const auto c=counts();const bool stock=source==L::Source::stock;
  const auto row=[&](const char* role,std::uint64_t v){const auto* r=stock?stock_course_receipts50CR::find(course,role):nullptr;
   L::trackDataNamed(role,course,"",v,r!=nullptr,r?r->value:0u);};
  row("atm.sources",c.sources);row("atm.frames",c.frames);row("atm.animations",c.animations);
 }
};
} // namespace track_atm50CR
