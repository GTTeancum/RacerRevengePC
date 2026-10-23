#include "original_ui_garage_panel50kg.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
using namespace rr;
namespace {
void check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
std::shared_ptr<assets::OriginalUiArchive50KG> archive(const std::string& root,assets::OriginalUiArchiveId50KG id){
    auto a=std::make_shared<assets::OriginalUiArchive50KG>();const auto* identity=assets::originalUiArchiveIdentity50KG(id);
    check(identity,"archive identity");std::ifstream f(root+"/"+identity->discPath,std::ios::binary);
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(f)),{});assets::OriginalUiArchiveDiagnostic50KG d;
    check(bool(f)&&a->loadOwned(id,std::move(bytes),d),"archive authentication");return a;
}
void report(const platform::OriginalUiGaragePanelDiagnostic50KG& d){
    std::cerr<<"panel error="<<unsigned(d.error)<<" widget="<<d.widget<<" material="<<d.material<<" image="<<d.image
        <<" pc="<<std::hex<<d.sourcePC<<std::dec<<" material-detail="<<unsigned(d.materialDetail.error)<<" field="<<d.materialDetail.field
        <<" fallback="<<unsigned(d.fallback.error)<<" geometry="<<unsigned(d.geometry.error)<<" image="<<unsigned(d.imageDetail.error)<<"\n";
}
}
int main(int argc,char** argv){try {
    check(argc==2,"usage: garage-panel-check ORIGINAL_DISC_ROOT");using A=assets::OriginalUiArchiveId50KG;
    platform::OriginalUiGaragePanelColdInput50KG in;auto garage=archive(argv[1],A::garage);
    in.appStart=archive(argv[1],A::appStart);in.fonts=archive(argv[1],A::fonts);
    auto loc=archive(argv[1],A::localization);assets::OriginalUiEnglish50KG english;assets::OriginalUiLocalizationDiagnostic50KG ld;
    check(assets::readOriginalUiEnglish50KG(*loc,english,ld),"English source");in.english=&english;in.runtime.overrideKnown=true;
    auto page=std::make_shared<assets::OriginalUiGaragePageData50KG>();assets::OriginalUiGaragePageDiagnostic50KG pd;
    check(page->load(garage,pd),"Garage page");in.page=page;
    // Cold layout fixture, never a native Video/GL/source collector capability.
    in.video={std::make_shared<const int>(1),640,448,32,0xffffff,0x31};
    platform::OriginalUiGaragePanel50KG owner;platform::OriginalUiGaragePanelDiagnostic50KG d;
    auto invalid=in;invalid.video.lifetime.reset();check(!owner.load(invalid,d)&&!owner.loaded(),"missing shared video admitted");
    invalid=in;invalid.fonts=garage;check(!owner.load(invalid,d)&&!owner.loaded(),"wrong font archive admitted");
    invalid=in;invalid.runtime.override435670=true;check(!owner.load(invalid,d)&&!owner.loaded(),"unsupported runtime override admitted");
    if(!owner.load(in,d)){report(d);throw std::runtime_error("authentic panel load");}
    check(owner.loaded()&&owner.ordinaryOrder()->size()==19&&owner.materials()->size()==9&&owner.images()->textures()->textureCount()==14,"retained population");
    for(auto id:{4u,5u,9u,6u,10u})check(!owner.descriptor(id)&&!owner.widget(id),"nontext surrogate manufactured");
    check(!owner.applyTextCallback(10,game::OriginalUiControlOperation50KG::show,true,d)&&d.error==platform::OriginalUiGaragePanelError50KG::requiredJoin,"Watto silently omitted");
    for(std::uint16_t id=1;id<=22;++id)if(const auto* t=owner.descriptor(id)){
        check(t->identity==page->widgetsDat()->identity(),"text provenance");
        if(t->textSource==assets::OriginalUiTextSource50KG::dynamic)check(t->dynamicLineLength==40&&t->lines.size()==(id==8?2:4),"dynamic capacity");
        check(!owner.widget(id)->shown(),"cold widget shown");
    }
    check(owner.descriptor(3)->lines[0]=="WATTO'S GARAGE"&&owner.descriptor(17)->lines[0]=="ACCELERATION","authentic localized labels");
    const auto& materials=*owner.materials();
    for(auto i:{4u,6u}){const auto& m=materials[i];check(m.fallback&&m.fallback->loaded()&&m.authored.passes.empty()&&!m.authored.identity,"fallback pretended authored PSM");
        check(m.fallback->templateOwner()->identity==in.appStart->find("AppStart/ScreenMT.psm").identity&&m.fallback->creationPC()==0x2475C0,"canonical template source");
        check(m.policies.size()==1&&m.policies[0].depthMethod==2&&m.policies[0].depthWrite&&m.policies[0].requiresSeparateAlphaStore,"fallback source policy");
        check(m.fallback->textureIdentity()==garage->find(i==4?"ui/textures/podstat2.pst":"ui/textures/notch.pst").identity,"fallback actual texture");}
    assets::OriginalUiGaragePanelFallback50KG badFallback;assets::OriginalUiGaragePanelFallbackDiagnostic50KG fd;
    check(!badFallback.load(garage,in.appStart,assets::OriginalUiGaragePanelFallbackKind50KG::podstat2,in.runtime,owner.images(),materials[6].images[0],fd)&&!badFallback.loaded(),"fallback cross-image spoof");
    for(auto i:{7u,8u})check(materials[i].animated&&materials[i].images.size()==4&&materials[i].animation.framesPerSecond==16&&materials[i].animation.reverseLoopOnEnd,"original unique arrows");
    check(!owner.advanceArrows({},d)&&d.error==platform::OriginalUiGaragePanelError50KG::clock,"invented application clock");
    game::OriginalUiGarageState50KG state;state.known=true;state.base=state.current4E8={{4,5,4,4,4,3}};state.maximum={{18,19,18,18,18,17}};
    // Numeric fresh fixture only; the independent production profile owner
    // remains responsible for selected-career identity/admission in a session.
    platform::OriginalUiGaragePanelInput50KG input{&state,true,0,true,0xff4080c0,0xffa0c0e0};
    check(!owner.projectFreshPanel(input,d)&&!owner.packetCount(),"missing source theme accepted");
    check(owner.setEntryTheme(input.primaryA8,input.secondaryAC,d),"source entry theme");
    check(owner.widget(3)->widgetColor()==input.primaryA8&&owner.widget(1)->widgetColor()==input.secondaryAC&&owner.widget(8)->widgetColor()==0xffcccccc,"source theme targets");
    for(std::uint32_t selected=0;selected<6;++selected){state.selected4E4=selected;
        if(!owner.projectFreshPanel(input,d)){report(d);throw std::runtime_error("fresh panel projection");}
        check(owner.packetCount()==114,"only maximum108 notches drawn plus6 panel/text/arrows");
        const auto* packets=owner.packets();check(packets[0].objectOffset==0x5E0&&packets[0].quad->vertices[0].y==251&&packets[0].quad->vertices[3].x==640&&packets[0].packetZ==float(0xffffff)-15,"panel exact geometry/depth");
        check(packets[1].quad->vertices[0].x==374&&packets[1].quad->vertices[3].x==406&&packets[1].quad->vertices[3].v==18.f/32.f,"first cropped tab");
        check(packets[2].quad->vertices[0].x==406&&packets[2].quad->vertices[3].x==640&&packets[2].quad->vertices[3].u==20.f/32.f,"source second234-wide tab");
        for(std::size_t r=0;r<6;++r)for(std::size_t j=0;j<20;++j){const auto* q=owner.notch(r,j);check(q&&q->vertices[0].x==190+7*j&&q->vertices[0].y==271+18*r&&q->vertices[3].x==195+7*j&&q->vertices[3].y==281+18*r,"all120 retained notch slots");
            const auto expected=j<std::size_t(state.current4E8[r])?(r==selected?std::array<std::uint8_t,4>{{102,102,102,63}}:std::array<std::uint8_t,4>{{32,64,96,63}}):std::array<std::uint8_t,4>{{25,25,25,50}};
            check(q->sourcePacketColor==expected,"original notch color/alpha");}
        const auto expectedCost=std::to_string(50*(state.current4E8[selected]+1)+300)+" T";check(*owner.costText()==expectedCost,"source next-price format");
        check(packets[111].objectOffset==0x3340&&packets[111].text&&packets[111].packetZ==float(0xffffff)-12&&packets[112].objectOffset==0x33F0&&packets[113].objectOffset==0x3450,"cost and arrow source order");
        check(packets[112].quad->vertices[0].x==172&&packets[113].quad->vertices[0].x==329&&packets[113].quad->vertices[0].y==270+18*selected,"source arrows");
        check(owner.liveText(8)->at(0)=="Available Truguts: 0"&&owner.liveText(8)->at(1).empty(),"source cash format/capacity");
        for(std::uint16_t r=0;r<6;++r)check(owner.widget(11+r)->shown()==(r==selected),"selected description only");
        check(owner.liveText(11+selected)->at(0)=="Buy:","source description prefix");
    }
    check(owner.liveText(11)->at(1)=="Dual 20"&&owner.liveText(11)->at(2)=="PCX Injector"&&owner.liveText(12)->at(1)=="Plug 2"&&owner.liveText(12)->at(2)=="Thrust Coil","original description lines");
    const auto before=*owner.costText();const auto* ptr=owner.packets();state.delta500[0]=1;check(!owner.projectFreshPanel(input,d)&&owner.packets()==ptr&&*owner.costText()==before&&owner.packetCount()==114,"purchase state mutated panel");state.delta500[0]=0;
    input.panelOffset4D8=140;check(owner.projectFreshPanel(input,d)&&owner.packets()[0].quad->vertices[0].y==391,"entry Y retained");
    input.panelOffset4D8=std::numeric_limits<float>::quiet_NaN();check(!owner.projectFreshPanel(input,d),"NaN panel offset");input.panelOffset4D8=0;check(owner.projectFreshPanel(input,d),"restore source offset");
    check(owner.applyTextCallback(17,game::OriginalUiControlOperation50KG::show,true,d)&&owner.applyTextCallback(17,game::OriginalUiControlOperation50KG::select,true,d)&&owner.updateText(17,true,d),"label source callbacks");
    platform::OriginalUiGarageWidgetTextProjection50KG textProjection;check(owner.projectText(17,0,true,textProjection,d),"label real font projection");
    check(textProjection.geometry->position.authoredLineX==170&&textProjection.geometry->position.authoredLineY==268&&textProjection.line.packetColor==std::array<std::uint8_t,4>{{102,102,102,63}},"render-relocated label and highlight");
    state.base.fill(20);state.current4E8.fill(20);state.maximum.fill(20);check(owner.projectFreshPanel(input,d)&&owner.packetCount()==126&&*owner.costText()=="MAX","full120 capacity and MAX");
    for(std::uint16_t i=11;i<=16;++i)check(owner.widget(i)->shown()==(i==11+state.selected4E4),"literal upgraded-versus-maximum description branch");
    state.base.fill(0);state.current4E8.fill(0);state.maximum.fill(0);check(owner.projectFreshPanel(input,d)&&owner.packetCount()==6,"zero maximum draws no notches");
    check(!owner.load(in,d)&&d.error==platform::OriginalUiGaragePanelError50KG::alreadyLoaded,"reload replaced owner");
    std::weak_ptr<const assets::OriginalUiArchive50KG> pin=garage;garage.reset();page.reset();in.page.reset();check(!pin.expired()&&owner.pageData()->wattoMcf().data,"original resource lifetime");
    std::cout<<"PASS authenticated Garage2D:17 text owners,19 genuine widget traversal entries including required Watto,9 material bindings,14 exact images,120 retained notch slots,114 fresh packets,126 maximum packets. No GL/session/route/Watto/scene/shared-page/canonical-collector readiness.\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
