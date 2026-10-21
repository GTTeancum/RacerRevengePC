// Included after sky50gw_fixture::Owner. Authenticates/decompresses every GB
// internal cube payload. Only the authentic complete base cube is submitted;
// lightning/ambient-flash chronology is not invented from sparse cube order.
namespace sky50gw_fixture {
namespace gb_detail50GB {
struct Entry {std::uint32_t cube,face,tile,offset,bytes;const char* sha;const char* filename;const char* imageSha;};
inline constexpr std::array<Entry,90> entries{{
#include "sky_gb_index.inc"
}};
inline constexpr std::array<std::uint32_t,8> offsets{{99124,193365,213138,235955,259119,280357,295223,304110}};
inline constexpr std::array<std::array<std::uint16_t,6>,7> masks{{
 {{255,255,255,255,65535,0}},{{240,96,0,48,0,0}},{{240,96,0,48,0,0}},
 {{240,96,0,48,0,0}},{{240,96,0,48,0,0}},{{240,0,0,48,0,0}},{{96,0,0,48,0,0}}
}};
}
inline bool Owner::loadGb(const Request& request,Diagnostic& d) noexcept {
 using namespace detail;namespace gb=gb_detail50GB;
 try{
  d={};need(!storage_,"GB sky assets already loaded");
  need(request.retainedGbSource&&request.retainedGbSource->ready()&&request.decodeOriginalImages&&
   !request.retainedArchive&&!request.retainedMbWorld&&!request.retainedTcWorld&&!request.retainedRcSource&&!request.retainedTbSource,
   "GB sky requires its sole actual compact source and original image decoding");
  need(std::fegetround()==FE_TONEAREST,"GB sky geometry requires FE_TONEAREST");
  const auto& scene=request.retainedGbSource->world().parameters.original_bytes;
  need(scene.size()==401867u&&sha256(scene)=="ed0eb494f8c91df5264e120bad3780016f67f297f259572d48a3e7a56aad27bb"&&
   std::memcmp(scene.data()+28581,"31,28,55",9)==0,"GB actual scene/fog identity differs");
  Bytes cub;a::DecodeError48C error;
  need(request.retainedGbSource->extractMember("skies\\gb.cub",cub,error,304110u)&&cub.size()==304110u&&
   sha256(cub)=="6b97ffd55d8aad11819d2696cc2366bf0303a42ebc65219b9ef4f4214a94a59c","GB exact CUB rejected");
  need(u32(cub.data())==1&&u32(cub.data()+4)==7&&u32(cub.data()+8)==40,"GB palette/cube table differs");
  for(unsigned i=0;i<7;++i)need(u32(cub.data()+12+4*i)==gb::offsets[i],"GB internal cube offset differs");
  need(u32(cub.data()+40)==0&&u32(cub.data()+44)==161&&u32(cub.data()+48)==0,"GB source palette framing differs");
  auto next=std::make_unique<Storage>();next->clearRgba={{31.f/255.f,28.f/255.f,55.f/255.f,1.f}};
  next->mesh.vertices.reserve(vertexCount);next->mesh.indices.reserve(indexCount);next->spans.reserve(tileCount);
  std::array<Bytes,90> images;std::array<a::MaterialTextureAssetInput50EA,90> textures{};
  std::array<a::MaterialTextureSlotInput50EA,90> slots{};std::size_t ordinal=0;
  for(unsigned cube=0;cube<7;++cube){
   const auto* h=cub.data()+gb::offsets[cube];need(u32(h)==32&&u16(h+4)==0&&u16(h+6)==0,"GB cube format differs");
   constexpr std::array<float,9> pose{{0,0,0,0,0,1,0,1,0}};
   for(unsigned j=0;j<9;++j)need(f32(h+44+4*j)==pose[j],"GB source cube pose differs");
   std::size_t at=gb::offsets[cube]+80;
   for(unsigned face=0;face<6;++face){
    const auto mask=gb::masks[cube][face];need(u16(h+8+face*2)==mask,"GB source tile mask differs");
    std::uint32_t packedSizes=0;for(unsigned tile=0;tile<16;++tile)if(mask&(1u<<tile))packedSizes|=2u<<(tile*2);
    need(u32(h+20+face*4)==packedSizes,"GB source tile size codes differ");
    for(unsigned tile=0;tile<16;++tile){if(!(mask&(1u<<tile)))continue;
     need(ordinal<gb::entries.size(),"GB tile count overflow");const auto& e=gb::entries[ordinal];
     need(e.cube==cube&&e.face==face&&e.tile==tile&&e.offset==at&&e.bytes<=gb::offsets[cube+1]-at,"GB tile extent differs");
     Bytes source(cub.begin()+at,cub.begin()+at+e.bytes);need(sha256(source)==e.sha,"GB tile identity differs");
     constexpr std::array<std::uint32_t,11> descriptor{{0,32,0,16,4096,4,0,0,0,0,0}};
     for(unsigned j=0;j<11;++j)need(u32(source.data()+4*j)==descriptor[j],"GB tile descriptor differs");
     std::size_t cursor=44,largest=0,largestBytes=0;
     for(unsigned dimension=16;dimension<=128;dimension*=2){
      need(cursor+4<=source.size(),"GB mip size truncated");const auto bytes=u32(source.data()+cursor);cursor+=4;
      need(bytes>=25&&bytes<=65536&&bytes<=source.size()-cursor,"GB mip extent differs");const auto* ipum=source.data()+cursor;
      need(std::memcmp(ipum,"ipum",4)==0&&u32(ipum+4)==bytes-8&&u16(ipum+8)==dimension&&u16(ipum+10)==dimension&&u32(ipum+12)==1,"GB mip header differs");
      largest=cursor;largestBytes=bytes;cursor+=bytes;
     }
     need(cursor==source.size(),"GB mip closure differs");Bytes rgba;std::string codecError;
     need(a::decodeOriginalIpuRgba(source.data()+largest,largestBytes,128,128,rgba,codecError),"GB original IPUM decode: "+codecError);
     need(rgba.size()==65536,"GB source image extent differs");images[ordinal]={'R','R','T','E','X','0','0','1',128,0,0,0,128,0,0,0};
     images[ordinal].insert(images[ordinal].end(),rgba.begin(),rgba.end());need(sha256(images[ordinal])==e.imageSha,"GB genuine decoded image differs");
     textures[ordinal]={static_cast<std::uint32_t>(ordinal),&images[ordinal]};
     slots[ordinal]={{0,static_cast<std::uint32_t>(ordinal),1,1},static_cast<std::uint32_t>(ordinal),a::MaterialTextureRequirement50EA::required};
     if(cube==0){const auto v=static_cast<std::uint32_t>(next->mesh.vertices.size()),start=static_cast<std::uint32_t>(next->mesh.indices.size());
      const auto x=(tile%4)*128,y=(tile/4)*128;constexpr float lo=.03125f,hi=.96875f;
      next->mesh.vertices.push_back(vertex(face,x,y,lo,lo));next->mesh.vertices.push_back(vertex(face,x,y+128,lo,hi));
      next->mesh.vertices.push_back(vertex(face,x+128,y,hi,lo));next->mesh.vertices.push_back(vertex(face,x+128,y+128,hi,hi));
      for(auto local:{0u,1u,2u,2u,1u,3u})next->mesh.indices.push_back(v+local);
      next->spans.push_back({start,6,v,4,static_cast<std::uint32_t>(ordinal),face,tile});
     }
     ++ordinal;at+=e.bytes;
    }
   }
   need(at==gb::offsets[cube+1],"GB internal cube source closure differs");
  }
  need(ordinal==90&&next->spans.size()==48&&next->mesh.vertices.size()==192&&next->mesh.indices.size()==288,"GB complete/base cube counts differ");
  for(unsigned axis=0;axis<3;++axis){auto& bounds=next->mesh.bounds;bounds.minimum[axis]=bounds.maximum[axis]=next->mesh.vertices.front().position[axis];
   for(const auto& v:next->mesh.vertices){bounds.minimum[axis]=std::min(bounds.minimum[axis],v.position[axis]);bounds.maximum[axis]=std::max(bounds.maximum[axis],v.position[axis]);}}
  const a::MaterialTextureAssetsInput50EA input{textures.data(),90,slots.data(),90,{90,90,90u*65552u,90u*65536u,128}};
  need(next->bank.load(input,d.bank),"GB authentic sky bank: "+d.bank.parserMessage);
  gbSource_=request.retainedGbSource;storage_=std::move(next);return true;
 }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
 catch(...){try{d.message="Unexpected GB sky preparation failure";}catch(...){}return false;}
}
}
