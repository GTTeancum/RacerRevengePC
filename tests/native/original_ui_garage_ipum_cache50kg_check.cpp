#include "../../src/assets/original_ui_ipum_texture_cache50kg.h"
#include "../../src/assets/original_ui_screen_assets50kg.h"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
namespace a=rr::assets;
std::size_t checks=0;
void check(bool okay,const char* why){++checks;if(!okay){std::cerr<<"FAIL "<<why<<"\n";std::exit(1);}}
std::vector<std::uint8_t> read(const std::filesystem::path& path){std::ifstream file(path,std::ios::binary);check(bool(file),"source file opens");return {std::istreambuf_iterator<char>(file),{}};}
std::shared_ptr<a::OriginalUiArchive50KG> archive(a::OriginalUiArchiveId50KG id,const std::filesystem::path& path){
 auto owner=std::make_shared<a::OriginalUiArchive50KG>();a::OriginalUiArchiveDiagnostic50KG d;
 check(owner->load(id,read(path),d),"genuine archive authenticates");return owner;
}
int main(int argc,char** argv){
 check(argc==3,"disc and independently generated native-pixel fixture paths");const std::filesystem::path disc=argv[1],pixels=argv[2];
 auto garage=archive(a::OriginalUiArchiveId50KG::garage,disc/"UI/UIGARAGE/UIGARAGE.RES");
 auto pod=archive(a::OriginalUiArchiveId50KG::garagePod19,disc/"UI/UIGARAGE/19E.RES");
 using Cache=a::OriginalUiIpumTextureCache50KG;a::OriginalUiIpumCacheDiagnostic50KG d;
 check(Cache::additionalCatalogSourceCount()==29,"existing Rider29 catalog unchanged");
 check(Cache::garageCatalogSourceCount()==11&&!Cache::garageSourceMember(11)&&!Cache::garageSourceArchive(11),"fixed genuine Garage11 catalog only");
 auto cache=std::make_unique<Cache>();
 const auto gs=garage->find("ui/textures/spcgarge.pst"),ps=pod->find("ui/textures/spcgarge.pst");
 check(gs.identity!=ps.identity&&gs.data!=ps.data&&gs.size==ps.size,"equal spcgarge bytes retain distinct original member owners");
 check(!cache->loadGarageOriginal(8,pod,d)&&d.error==a::OriginalUiIpumCacheError50KG::archive&&!cache->loaded(),"wrong archive cannot authorize same-name same-byte source");
 check(!cache->loadGarageOriginal(11,garage,d)&&!cache->loaded(),"unknown source index rejects without publication");
 a::TextureLimits48E limited;limited.input_bytes=1;
 check(!cache->loadGarageOriginal(8,garage,d,nullptr,limited)&&d.error==a::OriginalUiIpumCacheError50KG::limit&&!cache->loaded(),"source budget failure publishes nothing");
 a::DecodeHooks48C cancelled;cancelled.cancelled=[](void*){return true;};
 check(!cache->loadGarageOriginal(8,garage,d,&cancelled)&&d.error==a::OriginalUiIpumCacheError50KG::cancelled&&!cache->loaded(),"cancelled native decode publishes nothing");
 check(cache->loadGarageOriginal(8,garage,d),"native spcgarge source decodes and output hashes authenticate");
 check(!cache->textureFor(ps,0,ps.size),"Garage texture cannot alias the unadmitted19E member");
 const auto* first=cache->textureFor(gs,0,gs.size);
 check(first&&!cache->loadGarageOriginal(8,garage,d)&&d.error==a::OriginalUiIpumCacheError50KG::alreadyLoaded&&cache->textureFor(gs,0,gs.size)==first,"reload preserves original immutable texture");
 std::vector<a::OriginalUiImageSource50KG> sources;
 std::size_t mipTotal=0;
 for(std::size_t i=0;i<11;++i){const auto sourceArchive=i==10?pod:garage;
  check(Cache::garageSourceArchive(i)==sourceArchive->identity(),"catalog actual archive identity");
  if(i!=8)check(cache->loadGarageOriginal(i,sourceArchive,d),"all reached genuine Garage IPUM members decode");
  const auto source=sourceArchive->find(Cache::garageSourceMember(i));const auto* texture=cache->textureFor(source,0,source.size);const auto* evidence=cache->provenanceFor(source,0,source.size);
  check(texture&&evidence&&texture->kind==2&&evidence->decodedInProcess&&!evidence->hardwarePixelParityVerified,"actual typed native output with truthful parity limit");
  check(evidence->sourceArchiveSha256==sourceArchive->identity()->sha256&&evidence->sourcePstSha256==source.identity->sha256,"provenance remains exact source/archive, not shared name");
  check(texture->mips.size()==evidence->mipCount&&evidence->mipCount>0,"all original mips retained");mipTotal+=evidence->mipCount;
  for(std::size_t m=0;m<texture->mips.size();++m){const auto expected=read(pixels/(std::to_string(i)+"-"+std::to_string(m)+".rrtex"));const auto& actual=texture->mips[m];
   check(expected.size()==actual.rgba.size()+16&&std::equal(actual.rgba.begin(),actual.rgba.end(),expected.begin()+16),"every mip equals actual native source decode fixture");
   check(evidence->mips[m].yuv420pSha256.empty()&&!evidence->mips[m].rgbaSha256.empty(),"RGBA-only native adapter does not invent unobserved YUV hash");
  }
  auto fake=source;fake.data+=1;
  check(!cache->textureFor(fake,0,source.size)&&!cache->textureFor(source,1,source.size-1),"foreign byte range and subrange cannot borrow cache identity");
  a::OriginalUiImageSource50KG image;image.identity=i+1;image.member=source;image.pstBytes=source.size;sources.push_back(image);
 }
 check(mipTotal==47,"exact47 reached archive-specific mip allocations retained");
 check(cache->textureFor(gs,0,gs.size)!=cache->textureFor(ps,0,ps.size),"separate archive texture ownership despite equal original pixels");
 a::OriginalUiScreenAssets50KG images;a::OriginalUiScreenAssetDiagnostic50KG imageDiagnostic;
 check(images.load(sources,imageDiagnostic,cache.get()),"real screen image owner admits complete native cache outputs");
 cache.reset();garage.reset();pod.reset();
 check(images.loaded()&&images.textures()&&images.decodedTexture(10)&&images.ipumProvenance(10)->decodedInProcess&&images.sourceProvenance(10)->memberIdentity==sources[10].member.identity,"copied immutable image owner survives actual cache/archive release");
 std::cout<<"PASS "<<checks<<" Garage native IPUM cache checks\n";
}
