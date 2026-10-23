// Cold-only checks against actual originals. Never creates a route/session,
// renderer, mocked readiness token, profile, purchase or filesystem asset copy.
#include "original_ui_garage_page50kg.h"
#include "original_ui_overlay_text50kg.h"
#include "original_ui_atm50kg.h"
#include "import_texture48e.h"
#include <array>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace {
using namespace rr::assets;
using Id=OriginalUiArchiveId50KG;
void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
std::vector<std::uint8_t> read(const std::string& root,Id id){
    const auto* identity=originalUiArchiveIdentity50KG(id);check(identity,"missing catalog identity");
    std::ifstream file(root+"/"+identity->discPath,std::ios::binary|std::ios::ate);
    check(bool(file),"cannot open original archive");const auto length=file.tellg();
    check(length>=0&&static_cast<std::uint64_t>(length)==identity->bytes,"archive input size mismatch");
    std::vector<std::uint8_t> result(identity->bytes);file.seekg(0);
    file.read(reinterpret_cast<char*>(result.data()),static_cast<std::streamsize>(result.size()));
    check(bool(file),"original archive read failed");return result;
}
struct Cancel {unsigned calls=0,at=0;};
bool cancel(void* context){auto& s=*static_cast<Cancel*>(context);return ++s.calls>=s.at;}
std::shared_ptr<OriginalUiArchive50KG> load(const std::string& root,Id id){
    auto archive=std::make_shared<OriginalUiArchive50KG>();auto bytes=read(root,id);
    OriginalUiArchiveDiagnostic50KG d;
    check(archive->loadOwned(id,std::move(bytes),d),"archive/member authentication failed");
    check(bytes.empty()&&archive->loaded()&&d.status==OriginalUiArchiveStatus50KG::none,"owned publication mismatch");
    const auto* identity=originalUiArchiveIdentity50KG(id);
    check(archive->identity()==identity&&archive->memberCount()==identity->memberCount,"archive identity mismatch");
    for(std::size_t i=0;i<archive->memberCount();++i){
        const auto member=archive->member(i);
        check(member.identity==originalUiMemberIdentity50KG(id,i)&&member.data&&member.size==member.identity->bytes,"member order mismatch");
        check(archive->find(member.identity->name).identity==member.identity,"member lookup mismatch");
    }
    check(!archive->member(identity->memberCount).identity&&!archive->find("garage.dat").identity,"member bounds or basename fallback");
    return archive;
}
void failureTests(const std::string& root){
    auto original=read(root,Id::garage);auto changed=original;changed[changed.size()/2]^=1;
    OriginalUiArchive50KG archive;OriginalUiArchiveDiagnostic50KG d;
    check(!archive.loadOwned(Id::garage,std::move(changed),d)&&!archive.loaded()&&
          d.status==OriginalUiArchiveStatus50KG::archiveIdentity&&changed.size()==original.size(),"hash failure published or transferred input");
    check(!archive.load(Id::garagePod19,original,d)&&d.status==OriginalUiArchiveStatus50KG::archiveIdentity,"wrong archive accepted");
    changed=original;changed.pop_back();
    check(!archive.load(Id::garage,changed,d)&&!archive.loaded(),"truncated archive accepted");
    for(const auto at:{1u,40u,310u}){
        Cancel state{0,at};DecodeHooks48C hooks{&state,&cancel,nullptr};changed=original;
        check(!archive.loadOwned(Id::garage,std::move(changed),d,&hooks)&&!archive.loaded()&&
              d.status==OriginalUiArchiveStatus50KG::cancelled&&changed==original,"cancelled load published or changed input");
    }
    check(archive.load(Id::garage,original,d),"failed cold load was not retryable");
    const auto before=archive.member(58);
    check(!archive.load(Id::garage,original,d)&&d.status==OriginalUiArchiveStatus50KG::alreadyLoaded&&
          archive.member(58).data==before.data,"reload replaced immutable archive");
}
void textures(const OriginalUiArchive50KG& archive){
    struct Expected{const char* name;unsigned size,kind;};
    for(const auto& row:std::array<Expected,3>{{
            {"ui/textures/garagetab.pst",32,0},{"ui/textures/notch.pst",16,1},{"ui/textures/podstat2.pst",128,1}}}){
        const auto m=archive.find(row.name);check(m.identity&&m.data,"missing original Garage texture");
        Texture48E texture;DecodeError48C d;std::vector<std::uint8_t> bytes(m.data,m.data+m.size);
        check(decodeTexture48E(bytes,texture,d)&&texture.kind==row.kind&&texture.mips.size()==1&&
              texture.mips[0].width==row.size&&texture.mips[0].height==row.size&&
              texture.mips[0].rgba.size()==row.size*row.size*4,"original Garage texture decode mismatch");
    }
    for(const char* name:{"ui/textures/arrwleftsm.atm","ui/textures/arrwrghtsm.atm"}){
        const auto m=archive.find(name);OriginalUiAtm50KG atm;DecodeError48C d;
        check(m.identity&&splitOriginalUiAtm50KG(m.data,m.size,atm,d)&&atm.frames.size()==4,"Garage arrow frame count mismatch");
        for(const auto& frame:atm.frames){
            check(frame.kind==0&&frame.width==16&&frame.height==16&&frame.mipCount==1&&
                  frame.pstOffset<=m.size&&frame.pstBytes<=m.size-frame.pstOffset,"Garage arrow extent mismatch");
            Texture48E texture;std::vector<std::uint8_t> bytes(m.data+frame.pstOffset,m.data+frame.pstOffset+frame.pstBytes);
            check(decodeTexture48E(bytes,texture,d)&&texture.mips.size()==1&&texture.mips[0].rgba.size()==1024,"Garage arrow pixel decode mismatch");
        }
    }
}
void pageTests(const std::string& root){
    auto archive=load(root,Id::garage);auto pod=load(root,Id::garagePod19);
    check(pod->memberCount()==4&&pod->find("ui/geometry/19e.psg").size==136544,"selected19E original missing");
    check(archive->find("ui/uigarage/garage.dat").identity==archive->member(58).identity,"case/slash lookup mismatch");
    OriginalUiGaragePageData50KG data;OriginalUiGaragePageDiagnostic50KG d;
    check(!data.load({},d)&&d.status==OriginalUiGaragePageStatus50KG::archive&&!data.loaded()&&!data.page()&&!data.camera(),"empty archive accepted");
    check(!data.load(pod,d)&&d.status==OriginalUiGaragePageStatus50KG::archive&&!data.widgetsDat(),"wrong authenticated archive accepted");
    check(data.load(archive,d)&&data.loaded(),"Garage page decode failed");
    const auto* page=data.page();check(page&&page->pageNo==1&&page->widgetCount==22&&page->groups.size()==3&&page->controls.size()==8,"Garage page shape mismatch");
    check(page->openSize==69&&page->openTime==0.5f&&page->start=="Closed"&&!page->hasPopup&&
          page->background=="*INVALID*"&&page->backgroundColor==std::array<float,3>{{0,0,0}},"Garage aperture/background mismatch");
    check(page->groups[0].controls==std::vector<std::int16_t>{0}&&page->groups[1].controls==std::vector<std::int16_t>{1}&&
          page->groups[2].controls==std::vector<std::int16_t>({2,3,4,5,6,7}),"Garage group membership mismatch");
    for(const auto& group:page->groups)check(group.keepEnabled&&group.associations.empty(),"Garage group properties mismatch");
    check(page->controls[0].widgets==std::vector<std::uint16_t>({8,10})&&
          page->controls[1].widgets==std::vector<std::uint16_t>({1,2,3,7}),"Garage zero/base control mismatch");
    for(unsigned i=2;i<=7;++i)check(page->controls[i].present&&
        page->controls[i].widgets==std::vector<std::uint16_t>({std::uint16_t(i+9),std::uint16_t(i+15)}),"Garage statistic control mismatch");
    const auto* camera=data.camera();check(camera&&camera->identity==archive->member(58).identity&&
        camera->position==std::array<float,3>{{19.879841f,13.712481f,102.906265f}}&&
        camera->look==std::array<float,3>{{0.650447f,0,-0.759552f}}&&camera->up==std::array<float,3>{{0,1,0}}&&
        camera->fovDegrees==30&&camera->rollRadians==0,"authored camera changed");
    const auto* movement=data.movement();check(movement&&movement->widgetId==6&&movement->mappings[0]==-2&&movement->mappings[1]==-1&&
          movement->wantsJoystickInput==std::array<bool,2>{{true,true}}&&!movement->onTop,"authored movement changed");
    check(data.wattoMcf().identity==archive->member(22).identity&&data.wattoMcf().size==2814,"Watto MCF reference mismatch");
    const auto* widgets=data.widgetsDat();auto loc=load(root,Id::localization);OriginalUiEnglish50KG english;OriginalUiLocalizationDiagnostic50KG ld;
    check(readOriginalUiEnglish50KG(*loc,english,ld),"original English decode failed");
    unsigned localized=0,dynamic=0;
    for(unsigned i=1;i<=22;++i){
        const auto* section=widgets->section("Widget "+std::to_string(i));check(section,"missing authored widget");
        const auto* type=section->find("Type");check(type,"missing widget type");
        if(type->text!="Overlay_Text")continue;
        OriginalUiOverlayText50KG text;OriginalUiPageDiagnostic50KG td;
        check(readOriginalUiGarageOverlayText50KG(*widgets,static_cast<std::uint16_t>(i),&english,text,td),"Garage original text decode failed");
        check(text.identity==widgets->identity()&&text.widgetId==i&&text.alpha==255,"Garage text provenance or alpha changed");
        if(text.textSource==OriginalUiTextSource50KG::localized){++localized;check(!text.lines.empty()&&!text.lines[0].empty(),"localized label replaced with empty text");}
        if(text.textSource==OriginalUiTextSource50KG::dynamic){++dynamic;check(text.dynamicLineLength==40&&text.lines.size()==(i==8?2u:4u),"dynamic text capacity mismatch");}
        if(i>=17){
            check(section->find("Color")->text=="104,104,104,104"&&text.color==0xff686868&&text.highlightColor==0xffcccccc&&
                  text.x==360&&text.y==270+18*static_cast<int>(i-17),"source-prefix stat label mismatch");
            OriginalUiOverlayText50KG sentinel;sentinel.textId="unchanged";
            check(!readOriginalUiOverlayText50KG(*widgets,static_cast<std::uint16_t>(i),&english,sentinel,td)&&
                  sentinel.textId=="unchanged","generic strict parser changed or failure mutated output");
        }
    }
    check(localized==10&&dynamic==7,"Garage overlay text population mismatch");
    OriginalUiOverlayText50KG sentinel;sentinel.textId="unchanged";OriginalUiPageDiagnostic50KG td;
    check(!readOriginalUiGarageOverlayText50KG(*data.pageDat(),1,&english,sentinel,td)&&sentinel.textId=="unchanged","Garage adapter accepted wrong catalog member");
    check(!data.load(archive,d)&&d.status==OriginalUiGaragePageStatus50KG::alreadyLoaded&&data.page()==page,"reload replaced page owner");
    textures(*archive);std::weak_ptr<OriginalUiArchive50KG> retained=archive;archive.reset();
    check(!retained.expired()&&data.archive()&&data.wattoMcf().data&&data.page()==page,"page did not pin authenticated backing");
}
}
int main(int argc,char** argv){
    try{
        check(argc==2,"usage: garage-resource-check /path/to/original/disc");
        check(static_cast<unsigned>(Id::hallOfFame)==13&&static_cast<unsigned>(Id::garage)==14&&static_cast<unsigned>(Id::garagePod19)==15,"archive ordinals changed");
        check(!originalUiArchiveIdentity50KG(static_cast<Id>(16))&&!originalUiMemberIdentity50KG(Id::garage,170)&&
              !originalUiMemberIdentity50KG(Id::garagePod19,4),"catalog bounds failed");
        check(originalUiArchiveIdentity50KG(Id::garage)->firstMember==1306&&originalUiArchiveIdentity50KG(Id::garagePod19)->firstMember==1476,"catalog prefix changed");
        failureTests(argv[1]);pageTests(argv[1]);
        std::cout<<"Garage cold archive/page/text/indexed-texture checks passed using authenticated originals. No route, live scene, Watto animation, audio, GPU or profile readiness is claimed.\n";return 0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
