#pragma once
#include "mc_cold_sources50la.h"
#include "sky50gw_fixture.h"
#include <thread>

namespace mc_sky50MC {
namespace a=rr::assets;
struct Tile {std::uint32_t face,tile,offset,bytes;const char* sha;};
inline constexpr std::array<Tile,48> tiles{{
#include "mc_sky_index.inc"
}};
// Immutable source texture/geometry owner, not a renderer admission. Borrowers
// must retain this exact owner through the final GPU upload/draw fence.
class Owner final {
 struct Storage {
  a::Mesh48B mesh;a::MaterialTextureAssets50EA bank;
  std::vector<sky50gw_fixture::Span> spans;
  std::array<float,4> clear{{100.f/255.f,73.f/255.f,81.f/255.f,1.f}};
 };
 std::shared_ptr<const mc_cold_sources50LA::Owner> source_;
 std::unique_ptr<Storage> storage_;
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> source,sky50gw_fixture::Diagnostic& d)noexcept {
  using namespace sky50gw_fixture::detail;
  d={};if(thread_!=std::this_thread::get_id()||attempted_){d.message="MC sky requires fresh same-thread owner";return false;}attempted_=true;
  try {
   need(source&&source->ready()&&std::fegetround()==FE_TONEAREST,"MC sky actual source/rounding unavailable");
   const auto& scene=source->world().parameters.original_bytes;
   need(scene.size()==187711&&sha256(scene)=="26f5d637533ee9840e916e02d5cef7b06d68352f1b7e0b11a48bf050d73df51e","MC sky actual SCB identity differs");
   Bytes cub;a::DecodeError48C error;
   need(source->archive().extractMember("skies\\mcset4.cub",cub,error,194342)&&cub.size()==194342&&
    sha256(cub)=="ff3eb5a3e3480a04fe3bf3a85ab2d9113803f6fbd2b5301be43a2ff471a64383","MC sky original CUB differs");
   need(u32(cub.data())==1&&u32(cub.data()+4)==1&&u32(cub.data()+8)==16&&u32(cub.data()+12)==99100,
    "MC single-cube palette/table differs");
   const auto* h=cub.data()+99100;
   need(u32(h)==32&&u16(h+4)==0&&u16(h+6)==0,"MC sky format differs");
   constexpr std::array<float,9> pose{{0,0,0,0,0,1,0,1,0}};
   constexpr std::array<std::uint16_t,6> masks{{255,255,255,255,65535,0}};
   for(unsigned i=0;i<9;++i)need(f32(h+44+4*i)==pose[i],"MC sky pose differs");
   for(unsigned face=0;face<6;++face){std::uint32_t code=0;
    for(unsigned tile=0;tile<16;++tile)if(masks[face]&(1u<<tile))code|=2u<<(2*tile);
    need(u16(h+8+2*face)==masks[face]&&u32(h+20+4*face)==code,"MC sky authored masks/codes differ");}
   auto next=std::make_unique<Storage>();next->mesh.vertices.reserve(192);next->mesh.indices.reserve(288);next->spans.reserve(48);
   std::array<Bytes,48> images;std::array<a::MaterialTextureAssetInput50EA,48> textures{};
   std::array<a::MaterialTextureSlotInput50EA,48> slots{};std::size_t at=99180;
   for(std::size_t i=0;i<tiles.size();++i){const auto& e=tiles[i];
    need(e.offset==at&&at<=cub.size()&&e.bytes<=cub.size()-at,"MC sky tile extent differs");
    Bytes raw(cub.begin()+at,cub.begin()+at+e.bytes);need(sha256(raw)==e.sha,"MC sky tile identity differs");
    constexpr std::array<std::uint32_t,11> descriptor{{0,32,0,16,4096,4,0,0,0,0,0}};
    for(unsigned j=0;j<11;++j)need(u32(raw.data()+4*j)==descriptor[j],"MC sky tile descriptor differs");
    std::size_t cursor=44,largest=0,largestBytes=0;
    for(unsigned dimension=16;dimension<=128;dimension*=2){
     need(cursor+4<=raw.size(),"MC sky mip size missing");const auto bytes=u32(raw.data()+cursor);cursor+=4;
     need(bytes>=25&&bytes<=65536&&bytes<=raw.size()-cursor,"MC sky mip extent differs");const auto* ipum=raw.data()+cursor;
     need(std::memcmp(ipum,"ipum",4)==0&&u32(ipum+4)==bytes-8&&u16(ipum+8)==dimension&&u16(ipum+10)==dimension&&u32(ipum+12)==1,"MC sky IPUM header differs");
     largest=cursor;largestBytes=bytes;cursor+=bytes;
    }
    need(cursor==raw.size(),"MC sky tile closure differs");Bytes rgba;std::string error;
    need(a::decodeOriginalIpuRgba(raw.data()+largest,largestBytes,128,128,rgba,error),"MC sky original IPUM decode: "+error);
    need(rgba.size()==65536,"MC sky decoded pixel extent differs");
    images[i]={'R','R','T','E','X','0','0','1',128,0,0,0,128,0,0,0};images[i].insert(images[i].end(),rgba.begin(),rgba.end());
    textures[i]={static_cast<std::uint32_t>(i),&images[i]};slots[i]={{0,static_cast<std::uint32_t>(i),1,1},static_cast<std::uint32_t>(i),a::MaterialTextureRequirement50EA::required};
    const auto v=static_cast<std::uint32_t>(next->mesh.vertices.size()),start=static_cast<std::uint32_t>(next->mesh.indices.size());
    const auto x=(e.tile%4)*128,y=(e.tile/4)*128;constexpr float lo=.03125f,hi=.96875f;
    next->mesh.vertices.push_back(vertex(e.face,x,y,lo,lo));next->mesh.vertices.push_back(vertex(e.face,x,y+128,lo,hi));
    next->mesh.vertices.push_back(vertex(e.face,x+128,y,hi,lo));next->mesh.vertices.push_back(vertex(e.face,x+128,y+128,hi,hi));
    for(auto local:{0u,1u,2u,2u,1u,3u})next->mesh.indices.push_back(v+local);
    next->spans.push_back({start,6,v,4,static_cast<std::uint32_t>(i),e.face,e.tile});at+=e.bytes;
   }
   need(at==cub.size()&&next->mesh.vertices.size()==192&&next->mesh.indices.size()==288,"MC complete sky closure differs");
   for(unsigned axis=0;axis<3;++axis){auto& bounds=next->mesh.bounds;bounds.minimum[axis]=bounds.maximum[axis]=next->mesh.vertices.front().position[axis];
    for(const auto& v:next->mesh.vertices){bounds.minimum[axis]=std::min(bounds.minimum[axis],v.position[axis]);bounds.maximum[axis]=std::max(bounds.maximum[axis],v.position[axis]);}}
   const a::MaterialTextureAssetsInput50EA input{textures.data(),48,slots.data(),48,{48,48,48u*65552u,48u*65536u,128}};
   need(next->bank.load(input,d.bank),"MC sky texture bank: "+d.bank.parserMessage);
   source_=std::move(source);storage_=std::move(next);return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
  catch(...){try{d.message="MC sky unexpected preparation failure";}catch(...){}return false;}
 }
 // Source payload accounting only; caller includes allocator/control overhead.
 std::size_t retainedPayloadBytes()const noexcept{return storage_?
  storage_->mesh.vertices.capacity()*sizeof(a::Vertex48B)+storage_->mesh.indices.capacity()*sizeof(std::uint32_t)+
  storage_->spans.capacity()*sizeof(sky50gw_fixture::Span)+storage_->bank.sourceByteCount()+storage_->bank.rgbaByteCount():0;}
 bool loaded()const noexcept{return thread_==std::this_thread::get_id()&&storage_&&source_&&source_->ready();}
 const mc_cold_sources50LA::Owner* sourceIdentity()const noexcept{return loaded()?source_.get():nullptr;}
 const a::Mesh48B* mesh()const noexcept{return loaded()?&storage_->mesh:nullptr;}
 const a::MaterialTextureAssets50EA* bank()const noexcept{return loaded()?&storage_->bank:nullptr;}
 const std::vector<sky50gw_fixture::Span>* spans()const noexcept{return loaded()?&storage_->spans:nullptr;}
 const float* clearRgba()const noexcept{return loaded()?storage_->clear.data():nullptr;}
};
}
