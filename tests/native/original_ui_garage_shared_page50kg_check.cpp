#include "original_ui_garage_shared_page50kg.h"
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
// CPU trace fixture only, NOT a production hook/renderer capability. No source
// owner, session or registry admission is minted by this isolated exercise.
struct Trace {
    bool available=true;unsigned hooks=0,rectangles=0;game::OriginalUiApertureHook50KG last{};
    static bool ready(void* p)noexcept{return static_cast<Trace*>(p)->available;}
    static void hook(void* p,const game::OriginalUiApertureHook50KG& event)noexcept{auto& t=*static_cast<Trace*>(p);++t.hooks;t.last=event;}
    static void rectangle(void* p,std::uint8_t,const game::OriginalUiApertureRectangle50KG&)noexcept{++static_cast<Trace*>(p)->rectangles;}
    game::OriginalUiApertureCallbacks50KG callbacks(){return {this,ready,hook,rectangle};}
};
using AS=game::OriginalUiApertureStatus50KG;using Phase=game::OriginalUiTransitionPhase50KG;using Life=game::OriginalUiTransitionLifecycle50KG;
bool ok(AS s){return s==AS::ok||s==AS::noChange;}
void regress(const std::string& root,const platform::OriginalUiGarageSharedPageDrawOwner50KG& resources,
    const assets::OriginalUiEnglish50KG& english){
    using A=assets::OriginalUiArchiveId50KG;
    for(auto screen:{4u,5u,6u,9u,11u,12u}){
        auto source=archive(root,screen==4?A::mainMenu:screen==5?A::rider:screen==6?A::course:screen==9?A::tournament:screen==11?A::hallOfFame:A::options);
        const auto count=screen==12?6u:screen==5?2u:1u;
        for(unsigned page=1;page<=count;++page){game::OriginalUiApertureOwner50KG a;Trace trace;game::OriginalUiApertureDiagnostic50KG d;AS r=AS::notBound;
            if(screen==4)r=a.bindMain(source,trace.callbacks(),d);
            else if(screen==5)r=a.bindRider(source,page,trace.callbacks(),d);
            else if(screen==6)r=a.bindCourse(source,trace.callbacks(),d);
            else if(screen==9)r=a.bindTournament(source,trace.callbacks(),d);
            else if(screen==11)r=a.bindHallOfFame(source,trace.callbacks(),d);
            else if(page==1)r=a.bindOptionsMenu(source,trace.callbacks(),d);
            else if(page<=3)r=a.bindControls(source,page,trace.callbacks(),d);
            else if(page==4)r=a.bindGameOptions(source,trace.callbacks(),d);
            else if(page==5)r=a.bindMemoryCard(source,trace.callbacks(),d);
            else r=a.bindCredits(source,trace.callbacks(),d);
            check(r==AS::ok,"existing route bind regression");
            check(a.saveGarageUpdateDelta38(.5f)==AS::invalidPhase&&!a.garageAperturePredicate(Phase::entry),"Garage API leaked across route");
            if(screen==4)r=a.saveMainUpdateDelta38(1);
            else if(screen==5)r=a.saveRiderUpdateDelta38(1);
            else if(screen==6)r=a.saveCourseUpdateDelta38(1);
            else if(screen==9)r=a.saveTournamentUpdateDelta38(1);
            else if(screen==11)r=a.saveHallOfFameUpdateDelta38(1);
            else if(page==1)r=a.saveOptionsMenuUpdateDelta38(1);
            else if(page<=3)r=a.saveControlsUpdateDelta38(1);
            else if(page==4)r=a.saveGameOptionsUpdateDelta38(1);
            else if(page==5)r=a.saveMemoryCardUpdateDelta38(1);
            else r=a.saveCreditsUpdateDelta38(1);
            check(r==AS::ok&&ok(a.beginTransition7C(Phase::entry)),"existing entry regression");
            if(screen==6)check(ok(a.beginCourseEntryAtRender48(Phase::entry)),"Course entry gate");
            if(screen==9)check(ok(a.beginTournamentEntryAtRender48(Phase::entry,0)),"Tournament entry gate");
            check(a.render48(Life::waitPageTransition)==AS::ok&&a.render48(Life::waitPageTransition)==AS::ok,"existing open render");
            check(a.snapshot().state==game::OriginalUiApertureState50KG::open&&a.pollCompletion({std::uint8_t(screen),std::uint8_t(page)},Phase::entry)==game::OriginalUiTransitionCompletion50KG::complete,"existing completion regression");
            check(ok(a.beginTransition7C(Phase::exit))&&a.render48(Life::waitScreenExit)==AS::ok&&a.render48(Life::waitScreenExit)==AS::ok,"existing close render");
            check(a.snapshot().state==game::OriginalUiApertureState50KG::closed,"existing closed state");
            game::OriginalUiExitText50KG exit;game::OriginalUiExitTextDiagnostic50KG e;
            check(exit.prepare({std::uint8_t(screen),std::uint8_t(page)},source,english,resources.images(),resources.materials()->at(0).image,e)==game::OriginalUiExitTextStatus50KG::ok,"existing exit-text capacity/identity regression");
            check(exit.snapshot().capacity5B==10&&*exit.text()==english.at(89)->text,"existing exit-text content regression");
        }
    }
}
}
int main(int argc,char** argv){try{
    check(argc==2,"usage: garage-shared-check ORIGINAL_DISC_ROOT");using A=assets::OriginalUiArchiveId50KG;
    auto garage=archive(argv[1],A::garage),app=archive(argv[1],A::appStart),fonts=archive(argv[1],A::fonts),overlays=archive(argv[1],A::overlays),loc=archive(argv[1],A::localization);
    assets::OriginalUiEnglish50KG english;assets::OriginalUiLocalizationDiagnostic50KG ld;check(assets::readOriginalUiEnglish50KG(*loc,english,ld),"English");
    auto page=std::make_shared<assets::OriginalUiGaragePageData50KG>();assets::OriginalUiGaragePageDiagnostic50KG gd;check(page->load(garage,gd),"Garage page");
    auto panel=std::make_shared<platform::OriginalUiGaragePanel50KG>();platform::OriginalUiGaragePanelDiagnostic50KG pd;
    platform::OriginalUiGaragePanelColdInput50KG pc;pc.page=page;pc.appStart=app;pc.fonts=fonts;pc.english=&english;pc.runtime.overrideKnown=true;
    pc.video={std::make_shared<const int>(1),640,448,32,0xffffff,0x31}; // Cold layout fixture, not live Video admission.
    check(panel->load(pc,pd),"authentic retained panel");
    platform::OriginalUiGarageSharedColdInput50KG in{panel,app,overlays,&english,pc.runtime};
    platform::OriginalUiGarageSharedPageDrawOwner50KG s;platform::OriginalUiGarageSharedDiagnostic50KG d;
    auto bad=in;bad.panel.reset();check(!s.load(bad,d)&&!s.loaded(),"missing shared panel accepted");
    bad=in;bad.overlays=garage;check(!s.load(bad,d)&&!s.loaded(),"local archive passed as overlays");
    bad=in;bad.runtime.override435670=true;check(!s.load(bad,d)&&!s.loaded(),"unsupported material override accepted");
    if(!s.load(in,d)){std::cerr<<"error="<<unsigned(d.error)<<" pc="<<std::hex<<d.sourcePC<<std::dec<<" material="<<d.material<<" detail="<<unsigned(d.materialDetail.error)<<" field="<<d.materialDetail.field<<" prompt="<<unsigned(d.prompt.error)<<" bank="<<unsigned(d.bank.error)<<" exit="<<unsigned(d.exitText.status)<<'\n';throw std::runtime_error("authentic shared load");}
    check(s.loaded()&&s.panel()==panel.get()&&s.images()->textures()->textureCount()==18&&s.materials()->size()==6,"retained shared population");
    const auto& ms=*s.materials();const auto& ps=*s.prompts();
    check(ms[1].authored.identity==garage->find("ui/textures/notexture.psm").identity&&ms[1].image==assets::unresolvedMaterialTexture50EA&&
        ms[1].untextured.depthMethod==1&&ms[1].untextured.blend==assets::OriginalUiMaterialBlend50KG::opaque,"real NoTexture policy");
    constexpr float xs[]{484,380,285,160};constexpr const char* names[]{"icon_x","icon_tri","icon_sqr","icon_cir"};
    for(unsigned i=0;i<4;++i){const auto& p=ps[i];const auto& m=ms[i+2];const auto& owner=i<2?*overlays:*garage;
        check(m.prompt&&m.prompt->loaded()&&m.authored.passes.empty()&&!m.authored.identity,"prompt fabricated authored PSM");
        check(m.prompt->pageIdentity()==page->pageDat()->identity()&&m.prompt->textureIdentity()==owner.find(std::string("ui/textures/")+names[i]+".pst").identity,"prompt cross-archive provenance");
        check(m.prompt->images()==s.images().get()&&m.prompt->imageIndex()==m.image&&m.prompt->creationPC()==0x2475C0&&m.prompt->templateOwner()->identity==app->find("AppStart/ScreenMT.psm").identity,"prompt real fallback context");
        check(m.textured.depthMethod==2&&m.textured.depthWrite&&m.textured.requiresSeparateAlphaStore&&m.textured.blend==assets::OriginalUiMaterialBlend50KG::sourceAlpha,"prompt original source-alpha/depth");
        check(p.mask==(1u<<i)&&p.objectOffset==0x2A0+i*0x60&&p.quad.vertices[0].x==xs[i]&&p.quad.vertices[0].y==378&&p.quad.vertices[3].x==xs[i]+32&&p.quad.vertices[3].y==410&&p.quad.vertices[3].u==1&&p.quad.vertices[3].v==1,"full32px prompt area/source UV");
        check(p.quad.sourcePacketColor==std::array<std::uint8_t,4>{{127,127,127,63}},"source prompt channels changed");
    }
    check(ps[1].positionPC==0x1FA8EC&&ps[0].quad.vertices[3].x==panel->descriptor(1)->x&&ps[1].quad.vertices[3].x==panel->descriptor(2)->x,"original full-width prompt/label boundary overlap regression");
    assets::OriginalUiGaragePromptFallback50KG wrong;assets::OriginalUiGaragePromptDiagnostic50KG wd;
    check(!wrong.load(garage,app,overlays,2,pc.runtime,s.images(),ps[3].image,wd)&&!wrong.loaded(),"cross prompt image accepted");
    check(!wrong.load(garage,app,overlays,4,pc.runtime,s.images(),ps[0].image,wd)&&!wrong.loaded(),"unknown prompt accepted");
    // Same-looking square in OVERLAYS is a different source and must not substitute local Garage.
    auto other=std::make_shared<assets::OriginalUiScreenAssets50KG>();const auto square=overlays->find("ui/textures/icon_sqr.pst");assets::OriginalUiScreenAssetDiagnostic50KG image;
    check(other->load({{1,square,0,square.size,{}}},image),"shared square fixture");
    check(!wrong.load(garage,app,overlays,2,pc.runtime,other,0,wd)&&!wrong.loaded(),"identical-looking shared square alias accepted");
    auto* exit=s.exitText();check(exit&&exit->prepared()&&exit->snapshot().route.screen==13&&exit->snapshot().capacity5B==10&&*exit->text()==english.at(89)->text,"real Garage exit text");
    game::OriginalUiExitTextProjection50KG projected;game::OriginalUiApertureHook50KG event;
    check(exit->projectRender2E0170(event,projected)==game::OriginalUiExitTextStatus50KG::invalidRenderBoundary,"invented exit boundary accepted");
    // Bounded exit-leaf event fixture only; live Garage delayed close is not implemented here.
    event.kind=game::OriginalUiApertureHookKind50KG::screenExitObject440;event.callPC=0x1D75A4;event.targetPC=0x2E0170;
    event.observed.bound=true;event.observed.state=game::OriginalUiApertureState50KG::closing;event.observed.top284=230;event.observed.bottom28C=218;
    check(exit->projectRender2E0170(event,projected)==game::OriginalUiExitTextStatus50KG::ok&&projected.packetColor==std::array<std::uint8_t,4>{{102,102,102,63}}&&projected.glyphs->count<=10,"exit original glyph/color/capacity");
    check(exit->snapshot().positionedX20==481&&exit->snapshot().positionedY24==384,"exit original position");
    game::OriginalUiApertureOwner50KG a;Trace trace;game::OriginalUiApertureDiagnostic50KG ad;
    check(!s.projectAfterAperture(a,3,0,d),"unbound aperture accepted");
    check(a.bindGarage(fonts,trace.callbacks(),ad)==AS::invalidArchive,"nonGarage aperture identity accepted");
    trace.available=false;check(a.bindGarage(garage,trace.callbacks(),ad)==AS::unavailable,"missing native hooks accepted");trace.available=true;
    check(a.bindGarage(garage,trace.callbacks(),ad)==AS::ok,"Garage shared aperture bind");
    check(a.config()->openSize274==69&&a.config()->time4B0==.5f&&a.config()->startClosed&&a.snapshot().speed4B8==310&&a.snapshot().top284==224&&a.snapshot().bottom28C==224&&!a.snapshot().inputEnabled268,"actual Garage config/defaults");
    check(a.render48(Life::waitPageTransition)==AS::missingUpdateDelta&&!s.projectAfterAperture(a,3,0,d),"invented update sample accepted");
    check(a.saveMainUpdateDelta38(.5f)==AS::invalidPhase&&a.saveGarageUpdateDelta38(std::numeric_limits<float>::quiet_NaN())==AS::invalidDelta,"cross update/NaN accepted");
    check(a.saveGarageUpdateDelta38(.5f)==AS::ok&&a.beginTransition7C(Phase::entry)==AS::ok&&trace.last.kind==game::OriginalUiApertureHookKind50KG::openingAC&&trace.last.targetPC==0x1FAF10,"entry actual Garage callback");
    check(a.render48(Life::waitPageTransition)==AS::ok&&a.snapshot().top284==69&&a.snapshot().bottom28C==379&&a.snapshot().state==game::OriginalUiApertureState50KG::opening,"endpoint pre-step semantics");
    check(a.render48(Life::waitPageTransition)==AS::ok&&a.snapshot().inputEnabled268&&trace.last.targetPC==0x1FAF70&&a.garageAperturePredicate(Phase::entry),"opened actual Garage callback");
    check(a.pollCompletion({13,1},Phase::entry)==game::OriginalUiTransitionCompletion50KG::unavailable,"shared-only Garage completion admitted");
    const auto hooks=trace.hooks;check(a.beginTransition7C(Phase::exit)==AS::noChange&&a.snapshot().state==game::OriginalUiApertureState50KG::open&&trace.hooks==hooks,"Garage EXIT7C must stay no-op");
    for(std::uint32_t mask=0;mask<16;++mask){check(s.projectAfterAperture(a,mask,0,d),"source tail projection");unsigned count=2;for(unsigned i=0;i<4;++i)if(mask&(1u<<i))++count;
        check(s.packetCount()==count&&s.packets()[0].objectOffset==0x424&&s.packets()[1].objectOffset==0x428,"apertures before prompts");
        check(s.packets()[0].quad->vertices[3].y==69&&s.packets()[1].quad->vertices[0].y==379&&s.packets()[1].quad->vertices[3].y==448&&s.packets()[0].packetZ==16777214,"actual aperture geometry/depth");
        check(s.packets()[0].quad->sourcePacketColor==std::array<std::uint8_t,4>{{0,0,0,63}},"aperture source alpha lost");
        unsigned n=2;for(unsigned i=0;i<4;++i)if(mask&(1u<<i))check(s.packets()[n++].objectOffset==0x2A0+0x60*i,"prompt order changed");}
    check(s.projectAfterAperture(a,15,7,d)&&s.packetCount()==2,"source420 suppresses only prompts");
    check(!s.projectAfterAperture(a,16,0,d)&&s.packetCount()==2,"invalid mask changed previous packets");
    regress(argv[1],s,english);
    check(!s.load(in,d)&&d.error==platform::OriginalUiGarageSharedError50KG::alreadyLoaded,"reload replaced resources");
    std::weak_ptr<const platform::OriginalUiGaragePanel50KG> retained=panel;panel.reset();in.panel.reset();check(!retained.expired()&&s.panel()->video()->lifetime,"shared panel/video lifetime lost");
    std::cout<<"PASS Garage shared source resources:4 authentic32x32 prompts,6 material bindings,18-image combined bank,real exit capacity10,Garage aperture69/0.5/Closed,all16 masks/source420,correct triangle380/full-width label boundaries,12 existing route regressions. No Garage transition/Watto/scene/fire/collector/source-frame readiness.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
