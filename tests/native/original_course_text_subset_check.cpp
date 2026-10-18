// Isolated Course text API checker derived from the authentic focused checker.
// Author has not compiled or executed this derived checker.
#include "original_ui_course_page_draw50kg.h"
#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
namespace {
namespace a=rr::assets;
using namespace rr::game;
unsigned checks=0,textCases=0;
void need(bool okay,const std::string& why){++checks;if(!okay)throw std::runtime_error(why);}
std::uint32_t bits(float x){return std::bit_cast<std::uint32_t>(x);}
std::shared_ptr<a::OriginalUiArchive50KG> load(const std::filesystem::path& root,a::OriginalUiArchiveId50KG id){
 const auto* identity=a::originalUiArchiveIdentity50KG(id);need(identity,"archive catalog entry");
 const auto path=root/identity->discPath;
 std::ifstream file(path,std::ios::binary|std::ios::ate);need(bool(file),"open "+path.string());
 const auto size=file.tellg();need(size>=0&&static_cast<std::uint64_t>(size)==identity->bytes,"archive exact byte count "+path.string());
 std::vector<std::uint8_t> bytes(identity->bytes);file.seekg(0);
 file.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));need(bool(file),"read original archive");
 auto archive=std::make_shared<a::OriginalUiArchive50KG>();a::OriginalUiArchiveDiagnostic50KG d;
 need(archive->load(id,bytes,d),"archive/member authentication "+path.string());return archive;
}
bool sameText(const OriginalUiCoursePageTextSnapshot50KG& a,const OriginalUiCoursePageTextSnapshot50KG& b){
 return a.memberOffset==b.memberOffset&&a.capacity==b.capacity&&a.count==b.count&&a.packetCount==b.packetCount&&
 a.formattedByteLength==b.formattedByteLength&&a.measuredWidth==b.measuredWidth&&a.measuredHeight==b.measuredHeight&&
 a.authoredX==b.authoredX&&a.authoredY==b.authoredY&&bits(a.positionedX)==bits(b.positionedX)&&bits(a.positionedY)==bits(b.positionedY)&&
 a.color54==b.color54&&a.alpha58==b.alpha58&&a.text==b.text;
}
bool samePacket(const OriginalUiCoursePagePacket50KG& a,const OriginalUiCoursePagePacket50KG& b){
 if(a.materialName!=b.materialName||a.materialIdentity!=b.materialIdentity||a.sourcePC!=b.sourcePC||a.pageMemberOffset!=b.pageMemberOffset||
 bits(a.minimumTextureResidencyPriority)!=bits(b.minimumTextureResidencyPriority)||a.vertices.size()!=b.vertices.size())return false;
 for(std::size_t i=0;i<a.vertices.size();++i){const auto& x=a.vertices[i];const auto& y=b.vertices[i];
  for(unsigned j=0;j<3;++j)if(bits(x.position[j])!=bits(y.position[j]))return false;
  for(unsigned j=0;j<2;++j)if(bits(x.uv[j])!=bits(y.uv[j]))return false;
  for(unsigned j=0;j<4;++j)if(x.color[j]!=y.color[j])return false;}
 return true;
}
void text(const std::filesystem::path& root,const std::shared_ptr<a::OriginalUiArchive50KG>& course){
 const auto loc=load(root,a::OriginalUiArchiveId50KG::localization),fonts=load(root,a::OriginalUiArchiveId50KG::fonts),popup=load(root,a::OriginalUiArchiveId50KG::controllerPopup);
 a::OriginalUiEnglish50KG english;a::OriginalUiLocalizationDiagnostic50KG ld;need(a::readOriginalUiEnglish50KG(*loc,english,ld),"actual English localization");
 a::OriginalUiImageSource50KG font,overlay;font.identity=1;font.member=fonts->member(3);font.cel=fonts->member(2);font.pstBytes=font.member.size;
 overlay.identity=2;overlay.member=popup->member(3);overlay.pstBytes=overlay.member.size;
 auto images=std::make_shared<a::OriginalUiScreenAssets50KG>();a::OriginalUiScreenAssetDiagnostic50KG sd;
 need(images->load({font,overlay},sd),"real Denmrk16 and overlay1 native decode");
 OriginalUiCoursePageDraw50KG page;OriginalUiCoursePageDrawDiagnostic50KG d;
 need(!page.setLapCount(1,d)&&d.error==OriginalUiCoursePageDrawError50KG::notPrepared,"unprepared text rejected");
 need(page.prepare(course,english,images,0,1,16777215,d),"actual embedded Course page prepare");
 need(english.at(126)&&english.at(133)&&english.at(134),"real duration and singular/plural entries");
 const auto expectedText=[&](unsigned laps){return english.at(126)->text+"  "+std::to_string(laps)+" "+english.at(laps==1?133:134)->text;};
 need((*page.texts())[1].text==expectedText(3),"cold default remains3");
 const auto initialPackets=*page.packets();const auto initialTexts=*page.texts();
 for(unsigned laps=1;laps<=25;++laps){
  need(page.setLapCount(laps,d),"set genuine lap text");const auto& snapshot=(*page.texts())[1];const auto& packet=(*page.packets())[3];
  need(snapshot.text==expectedText(laps)&&snapshot.memberOffset==0xB30&&snapshot.capacity==60&&snapshot.authoredY==147,"singular/plural source format and member");
  need(snapshot.count==snapshot.text.size()&&snapshot.formattedByteLength==expectedText(laps).size()&&snapshot.packetCount==(snapshot.count+35u)/36u,"source text counts");
  need(packet.sourcePC==0x1DB258&&packet.pageMemberOffset==0xB30&&packet.materialIdentity==initialPackets[3].materialIdentity&&!packet.vertices.empty(),"text keeps genuine packet/font material provenance");
  need(sameText((*page.texts())[0],initialTexts[0]),"event row text unchanged");
  for(unsigned i:{0u,1u,2u,4u,5u})need(samePacket((*page.packets())[i],initialPackets[i]),"unrelated page packets unchanged");
  const auto p=*page.packets();const auto t=*page.texts();need(page.setLapCount(laps,d),"repeated same count accepted");
  for(std::size_t i=0;i<p.size();++i)need(samePacket(p[i],(*page.packets())[i]),"same-count packet idempotence");
  for(std::size_t i=0;i<t.size();++i)need(sameText(t[i],(*page.texts())[i]),"same-count text idempotence");++textCases;
 }
 for(unsigned laps:{0u,26u,31u,std::numeric_limits<unsigned>::max()}){
  const auto p=*page.packets();const auto t=*page.texts();need(!page.setLapCount(laps,d)&&d.error==OriginalUiCoursePageDrawError50KG::geometry&&d.sourcePC==0x1DAF78,"bad lap text rejected");
  for(std::size_t i=0;i<p.size();++i)need(samePacket(p[i],(*page.packets())[i]),"invalid count packet atomicity");
  for(std::size_t i=0;i<t.size();++i)need(sameText(t[i],(*page.texts())[i]),"invalid count text atomicity");
 }
 need(page.setLapCount(3,d)&&sameText((*page.texts())[1],initialTexts[1])&&samePacket((*page.packets())[3],initialPackets[3]),"restore original3 text and geometry exactly");
}
}
int main(int argc,char** argv){try{need(argc==2,"usage: text_subset ORIGINAL_DISC_ROOT");const std::filesystem::path root=argv[1];auto archive=load(root,a::OriginalUiArchiveId50KG::course);text(root,archive);std::cout<<"TEXT_SUBSET_PASS counts="<<textCases<<" assertions="<<checks<<"\n";return 0;}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
