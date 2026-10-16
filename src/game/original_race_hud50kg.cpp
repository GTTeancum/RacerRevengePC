#include "original_race_hud50kg.h"
#include "original_ui_text50kg.h"
#include "original_ui_text_position50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>

namespace rr::game {
namespace {
using A=assets::OriginalUiArchiveId50KG;
using E=OriginalRaceHudDiagnostic50KG::Error;
using Text=OriginalRaceHudText50KG;
using Gleam=OriginalRaceHudGleam50KG;
bool genuine(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,A id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
bool fail(OriginalRaceHudDiagnostic50KG& d,E e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
void stop(Text& t) noexcept {t.enabled70=0;t.indefinite88=0;t.hold84=-1;t.lockedCC=0;}
void setColor(Text& t,std::uint32_t c) noexcept {if(!t.lockedCC){t.color170=c;t.colorDirty180=true;}}
void setPosition(Text& t,std::int32_t x,std::int32_t y) noexcept {
    if(t.lockedCC)return;
    t.positionDuration=0;t.positionElapsed=0;t.positionTarget[0]=x;t.positionTarget[1]=y;
    t.positionFrom[0]=t.position[0];t.positionFrom[1]=t.position[1];
    t.position=t.positionTarget; //29DB9C four-word copy, zero duration.
}
void setText(Text& t,const char* p,std::size_t n) noexcept {
    if(t.lockedCC)return;
    n=std::min<std::size_t>(n,40);
    if(n==t.textLength&&!std::memcmp(t.text.data(),p,n))return;
    std::memcpy(t.text.data(),p,n);t.text[n]=0;t.textLength=static_cast<std::uint8_t>(n);
}
// Exact29DA40 arguments shared by22AE70/22B170/22B370/22B760. The
// delayed fade forces+CC=1 even though the caller's initial lock argument is0.
void setupLap(Text& t,float scale,float alphaDuration) noexcept {
    if(t.lockedCC)return;
    t.lockedCC=0;t.enabled70=1;t.blinkRemaining78=0;t.blinkPeriod7C=0;t.blinkOn80=1;
    t.hold84=2.5f;t.indefinite88=0;t.alphaTarget94=230;t.alpha174=230;t.alphaDirty184=true;
    t.scaleTargetA8=scale;t.scale178=scale;t.scaleDirty188=true;
    t.rotationTargetB4=0;t.rotation17C=0;t.rotationDirty18C=true;
    t.secondDelayD0=1;t.secondAlphaD4=0;t.alphaDurationA0=alphaDuration;
    t.secondScaleD8=scale;t.scaleDurationB0=0;t.secondRotationDC=0;t.rotationDurationC0=0;t.lockedCC=1;
}
bool time189000(float seconds,std::array<char,9>& text) noexcept {
    if(!std::isfinite(seconds))return false;
    if(seconds<=0||seconds>=21474836.0f){std::memcpy(text.data(),"--:--.--",9);return true;}
    const float scaled=seconds*100.f;const float rounded=.5f+scaled;
    if(!std::isfinite(rounded)||rounded>=2147483648.f)return false;
    const auto ticks=static_cast<std::int32_t>(rounded);
    const auto minutes=std::min(ticks/6000,99),secs=(ticks/100)%60,hundredths=ticks%100;
    text={{char('0'+minutes/10),char('0'+minutes%10),':',char('0'+secs/10),char('0'+secs%10),'.',
        char('0'+hundredths/10),char('0'+hundredths%10),0}};return true;
}
}
struct OriginalRaceHud50KG::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,overlays,localization;
    assets::OriginalUiMaterialTemplates50KG templates;
    assets::OriginalUiScreenAssets50KG images;
    assets::OriginalUiEnglish50KG english;
    //0 gleam14,1 in-game16,2 gleam30,3 explosion16,4 explosion30,
    //5 gleam bright,6 gleam clear. These are real parsed material objects.
    std::array<assets::OriginalUiMaterial50KG,7> materials;
    struct View {
        std::array<Text,7> records12C;
        Text status17C,time164;
        std::array<Text,2> explosions16C;
        Gleam gleam184,recordGleam194;
    };
    std::array<std::unique_ptr<View>,2> views;
    std::uint32_t count=0;
    std::int32_t rendererWidth=0,rendererHeight=0;
    bool constructed=false;
    Text* at(std::uint32_t v,std::uint32_t offset) noexcept {
        if(v>=count||!views[v])return nullptr;
        auto& w=*views[v];
        if(offset>=0x12c&&offset<=0x144&&(offset&3u)==0)return &w.records12C[(offset-0x12c)/4];
        if(offset==0x17c)return &w.status17C;
        if(offset==0x164)return &w.time164;
        if(offset==0x16c||offset==0x170)return &w.explosions16C[(offset-0x16c)/4];
        return nullptr;
    }
    bool width(const Text& t,std::int32_t& out) const noexcept {
        const auto* font=images.font(t.fontIndex);const auto* texture=images.textures()->texture(t.fontIndex);
        if(!font||!texture)return false;
        OriginalUiTextLayout50KG layout;OriginalUiTextDiagnostic50KG d;
        OriginalUiTextInput50KG in{font,&texture->texture,reinterpret_cast<const std::uint8_t*>(t.text.data()),t.textLength,0,0};
        return layoutOriginalUiText50KG(in,layout,d)&&roundOriginalUiTextMetric50KG(layout.widthBeforeScaleAndRounding,t.screenScale40,out);
    }
};
OriginalRaceHud50KG::OriginalRaceHud50KG() noexcept=default;
OriginalRaceHud50KG::~OriginalRaceHud50KG()=default;
bool OriginalRaceHud50KG::load(std::shared_ptr<const assets::OriginalUiArchive50KG> app,
    std::shared_ptr<const assets::OriginalUiArchive50KG> overlays,
    std::shared_ptr<const assets::OriginalUiArchive50KG> loc,
    const assets::OriginalUiMaterialRuntime50KG& runtime,OriginalRaceHudDiagnostic50KG& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded,0x1a8cc4);
    if(!genuine(app,A::appStart)||!genuine(overlays,A::overlays)||!genuine(loc,A::localization))return fail(d,E::archive,0x1a8cc4);
    try {
        auto n=std::make_unique<Storage>();n->appStart=std::move(app);n->overlays=std::move(overlays);n->localization=std::move(loc);
        if(!n->templates.load(*n->appStart,d.material))return fail(d,E::material,0x29ca40);
        const char* names[]={"font/denmarkbold14gleam.psm",nullptr,"font/denmarkbold30gleam.psm",
            "font/denmarkbold16explosion.psm","font/denmarkbold30explosion.psm","ui/textures/gleam.psm","ui/textures/gleamclear.psm"};
        for(std::size_t i=0;i<n->materials.size();++i){
            const bool okay=i==1?assets::readOriginalRaceHudFontFallbackMaterial50KG(*n->overlays,n->templates,runtime,n->materials[i],d.material):
                assets::readOriginalUiMaterial50KG(*n->overlays,names[i],n->templates,assets::OriginalUiTemplate50KG::screenMt,runtime,n->materials[i],d.material);
            if(!okay)return fail(d,E::material,i<5?0x29ca40:0x2e6770);
        }
        const char* fonts[]={"denmarkbold14ingame","denmarkbold16ingame","denmarkbold30ingame"};
        std::vector<assets::OriginalUiImageSource50KG> sources;
        for(unsigned i=0;i<3;++i){
            const std::string base=std::string("font/")+fonts[i];const auto pst=n->overlays->find(base+".pst"),cel=n->overlays->find(base+".cel");
            sources.push_back({0x1a8cc400u+i,pst,0,pst.size,cel});
        }
        // Both gleam ScreenTextures resolve the same authentic gleam atlas.
        const auto gleam=n->overlays->find("ui/textures/gleam.pst");sources.push_back({0x1a8cc403u,gleam,0,gleam.size,{}});
        if(!n->images.load(sources,d.images))return fail(d,E::images,0x29ca40);
        if(!assets::readOriginalUiEnglish50KG(*n->localization,n->english,d.localization))return fail(d,E::localization,0x22b2dc);
        if(!n->english.at(463)||!n->english.at(464))return fail(d,E::localization,0x22b2dc);
        storage_=std::move(n);return true;
    }catch(...){return fail(d,E::allocation,0x1a8cc4);}
}
bool OriginalRaceHud50KG::resourcesLoaded() const noexcept{return bool(storage_);}
bool OriginalRaceHud50KG::loaded() const noexcept{return storage_&&storage_->constructed;}
std::uint32_t OriginalRaceHud50KG::viewCount() const noexcept{return loaded()?storage_->count:0;}
bool OriginalRaceHud50KG::construct1A8CC4(std::uint8_t flags,
    const std::array<const OriginalCameraViewportFields*,2>& views,std::int32_t width,std::int32_t height) noexcept {
    if(!storage_||storage_->constructed||width<=0||height<=0)return false;
    const auto count=flags&3u;if(!count||count>2)return false;
    for(unsigned i=0;i<2;++i){
        if(i>=count){if(views[i])return false;continue;}
        const auto* v=views[i];if(!v||!v->prepared||!v->sceneKey||!v->sessionGeneration||v->width1E8<=0||v->height1EC<=0)return false;
    }
    try {
        std::array<std::unique_ptr<Storage::View>,2> next;
        for(unsigned i=0;i<count;++i)next[i]=std::make_unique<Storage::View>();
        auto init=[&](Text& t,unsigned view,std::uint8_t font,std::uint8_t material){
            t.viewport=views[view];t.fontIndex=font;t.materialIndex=material;
            t.viewportScaleX=static_cast<float>(width)/static_cast<float>(views[view]->width1E8);
            t.viewportScaleY=static_cast<float>(height)/static_cast<float>(views[view]->height1EC);
            t.position[0]=static_cast<std::int32_t>(502.f/t.viewportScaleX);
            t.position[1]=static_cast<std::int32_t>(315.f/t.viewportScaleY);
            t.position[2]=t.position[0]+128;t.position[3]=t.position[1]+16;
            //29DC60(1) after29CA40, exact original constant3C4740.
            t.screenJustification50=1;
        };
        // Preserve construction order: all seven/view first, then four/view.
        for(unsigned i=0;i<count;++i)for(auto& t:next[i]->records12C)init(t,i,0,0);
        for(unsigned i=0;i<count;++i){auto& v=*next[i];init(v.status17C,i,1,1);init(v.time164,i,2,2);
            init(v.explosions16C[0],i,1,3);init(v.explosions16C[1],i,2,4);
            for(auto* g:{&v.gleam184,&v.recordGleam194}){g->viewport=views[i];
                for(auto& c:g->brightColor)c={{127,127,127,63}};
                for(auto& c:g->clearColor)c={{0,0,0,0}};
            }
        }
        storage_->views=std::move(next);storage_->count=count;storage_->rendererWidth=width;storage_->rendererHeight=height;
        storage_->constructed=true;return true;
    }catch(...){return false;}
}
const OriginalRaceHudText50KG* OriginalRaceHud50KG::textAt(std::uint32_t view,std::uint32_t offset) const noexcept {
    return loaded()?storage_->at(view,offset):nullptr;
}
const OriginalRaceHudGleam50KG* OriginalRaceHud50KG::gleamAt(std::uint32_t view,std::uint32_t offset) const noexcept {
    if(!loaded()||view>=storage_->count)return nullptr;
    if(offset==0x184)return &storage_->views[view]->gleam184;
    if(offset==0x194)return &storage_->views[view]->recordGleam194;
    return nullptr;
}
LapHudFields50ID OriginalRaceHud50KG::lapFields(std::uint32_t view) const noexcept {
    if(!loaded()||view>=storage_->count)return {};
    return {reinterpret_cast<std::uintptr_t>(textAt(view,0x134)),reinterpret_cast<std::uintptr_t>(textAt(view,0x130)),
        reinterpret_cast<std::uintptr_t>(textAt(view,0x164)),true};
}
const assets::OriginalUiScreenAssets50KG* OriginalRaceHud50KG::images() const noexcept{return storage_?&storage_->images:nullptr;}
const assets::OriginalUiMaterial50KG* OriginalRaceHud50KG::material(std::size_t i) const noexcept {
    return storage_&&i<storage_->materials.size()?&storage_->materials[i]:nullptr;
}
assets::OriginalUiMemberView50KG OriginalRaceHud50KG::materialRegistry() const noexcept {
    try { return storage_?storage_->appStart->find("AppStart/Mat_Stbl.txt"):assets::OriginalUiMemberView50KG{}; }
    catch (...) { return {}; }
}
bool OriginalRaceHud50KG::stopAll1AD230() noexcept {
    if(!loaded())return false;
    for(unsigned view=0;view<2;++view){
        if(!storage_->views[view])continue;auto& v=*storage_->views[view];
        stop(v.status17C);stop(v.time164);
        for(unsigned i=0;i<7;++i){stop(v.records12C[i]);if(i<2)stop(v.explosions16C[i]);}
    }
    return true;
}
bool OriginalRaceHud50KG::applyLapCompletion(std::uint32_t view,std::uint32_t group34,
    std::uint8_t split,const LapCompletionRequest50GG& request) noexcept {
    if(!loaded()||view>=storage_->count||request.query)return false;
    const bool lap=request.kind==LapCompletionCall50GG::LapRecord22B170;
    const bool race=request.kind==LapCompletionCall50GG::RaceRecord22B370;
    const bool last=request.kind==LapCompletionCall50GG::LastLap22B760;
    if(!lap&&!race&&!last)return false;
    auto& v=*storage_->views[view];
    const bool one=group34==1,vertical=(split&2u)!=0;
    const std::int32_t x=one||!vertical?320:160;
    // Preflight all bounded string/metric work before the source setters.
    std::array<char,512> label{};std::size_t n=0;
    if(!last){
        std::array<char,9> time{};if(!time189000(request.argumentF12,time))return false;
        const auto* localized=storage_->english.at(lap?463:464);
        if(!localized||localized->text.size()+10>label.size())return false;
        n=localized->text.size();std::memcpy(label.data(),localized->text.data(),n);label[n++]=' ';
        std::memcpy(label.data()+n,time.data(),8);n+=8;
    }
    Text& t=last?v.time164:v.records12C[lap?2:1];
    Text& banner=last?t:v.records12C[0];
    // Width reads occur after SetText but before any source draw commits the
    // new scale. Preserve locked-setter suppression and old ScreenText scale.
    Text prospective=banner;
    setText(prospective,last?"FINAL LAP!":"NEW RECORD!",last?10u:11u);
    std::int32_t measured=0;if(!storage_->width(prospective,measured))return false;
    if(last){setColor(t,0xff7dcd6c);setPosition(t,x,one?120:vertical?180:90);
        setText(t,"FINAL LAP!",10);setupLap(t,1,1.5f);
    }else{
        setColor(t,0xff669bcc);setPosition(t,x,one?(lap?380:364):vertical?(lap?310:294):(lap?180:164));
        setText(t,label.data(),n);setupLap(t,.75f,1);
        // Unconditional22AE70 tail, including its independently locked banner.
        setColor(banner,0xff669bcc);setPosition(banner,x,one?346:vertical?276:146);
        setText(banner,"NEW RECORD!",11);setupLap(banner,1.2f,1);
    }
    auto& g=last?v.gleam184:v.recordGleam194;
    const float half=static_cast<float>(measured/2); // signed integer /2 first.
    const float delta=(last?1.15f:1.3f)*half;g.x104=static_cast<float>(x)-delta;
    g.y100=last?(one?90.f:vertical?150.f:60.f):(one?316.f:vertical?246.f:116.f);
    g.width108=30;g.height10C=last?50.f:100.f;
    g.travel110=static_cast<float>(1.2*static_cast<double>(measured)); // source libgcc binary64 multiply.
    g.offset114=0;g.duration120=.75f;g.angle118=.523598730564117431640625f; //3F060A91.
    if(!g.active11C){g.active11C=1;g.elapsed124=0;} //2E6940 does not restart an active gleam.
    return true;
}
namespace {
void advanceText(Text& t,float dt,std::int32_t rendererWidth,std::int32_t rendererHeight) noexcept {
    t.viewportScaleX=static_cast<float>(rendererWidth)/static_cast<float>(t.viewport->width1E8);
    t.viewportScaleY=static_cast<float>(rendererHeight)/static_cast<float>(t.viewport->height1EC);
    t.colorDirty180=t.alphaDirty184=t.scaleDirty188=t.rotationDirty18C=false;
    if(t.enabled70){
        if(t.hold84>=0){t.hold84=t.hold84-dt;}
        else if(!t.indefinite88){stop(t);}
        if(t.enabled70){
            t.blinkRemaining78=t.blinkRemaining78-dt;
            if(t.blinkRemaining78<=0&&t.blinkPeriod7C>0){t.blinkOn80=!t.blinkOn80;t.blinkRemaining78=t.blinkPeriod7C;}
        }
    }else t.lockedCC=0;
    const auto alpha=static_cast<std::uint16_t>(t.screenAlpha58)&255u;
    if(t.alphaTarget94!=static_cast<std::int32_t>(alpha)){
        if(t.alphaElapsed9C>=t.alphaDurationA0)t.alpha174=static_cast<float>(t.alphaTarget94);
        else{
            const float ratio=t.alphaElapsed9C/t.alphaDurationA0;
            const float difference=static_cast<float>(t.alphaTarget94-t.alphaFrom90);
            const float scaled=difference*ratio;
            t.alphaIntermediate98=t.alphaFrom90+static_cast<std::int32_t>(scaled);
            t.alpha174=static_cast<float>(t.alphaIntermediate98);t.alphaElapsed9C=t.alphaElapsed9C+dt;
        }
        t.alphaDirty184=true;
    }
    if(t.scaleTargetA8!=t.screenScale40){
        if(t.scaleElapsedAC>=t.scaleDurationB0)t.scale178=t.scaleTargetA8;
        else{const float ratio=t.scaleElapsedAC/t.scaleDurationB0;const float difference=t.scaleTargetA8-t.scaleFromA4;
            const float scaled=difference*ratio;t.scale178=t.scaleFromA4+scaled;t.scaleElapsedAC=t.scaleElapsedAC+dt;}
        t.scaleDirty188=true;
    }
    if(t.rotationTargetB4!=t.screenRotation44){
        if(t.rotationElapsedBC>=t.rotationDurationC0)t.rotation17C=t.rotationTargetB4;
        else{const float ratio=t.rotationElapsedBC/t.rotationDurationC0;const float difference=t.rotationTargetB4-t.rotationFromB8;
            const float scaled=difference*ratio;t.rotation17C=t.rotationFromB8+scaled;t.rotationElapsedBC=t.rotationElapsedBC+dt;}
        t.rotationDirty18C=true;
    }
    // The transition occurs on the next source update AFTER crossing below0.
    // It does not clear+CC; the hold's eventual29DB10 owns unlock.
    if(t.secondDelayD0>=0)t.secondDelayD0=t.secondDelayD0-dt;
    else if(t.secondDelayD0!=-1){
        t.secondDelayD0=-1;t.alphaTarget94=std::clamp(t.secondAlphaD4,0,255);
        t.alphaFrom90=static_cast<std::int32_t>(alpha);t.alphaElapsed9C=0;
        t.scaleTargetA8=std::max(t.secondScaleD8,0.f);t.scaleFromA4=t.screenScale40;t.scaleElapsedAC=0;
        t.rotationTargetB4=t.secondRotationDC;t.rotationFromB8=t.screenRotation44;t.rotationElapsedBC=0;
    }
}
}
bool OriginalRaceHud50KG::advanceAnimations29CDA0(float dt) noexcept {
    if(!loaded()||!std::isfinite(dt)||dt<0||dt>1)return false;
    for(unsigned i=0;i<storage_->count;++i){const auto* v=storage_->views[i]->status17C.viewport;
        if(!v||v->width1E8<=0||v->height1EC<=0)return false;}
    for(unsigned i=0;i<storage_->count;++i){auto& v=*storage_->views[i];
        for(auto& t:v.records12C)advanceText(t,dt,storage_->rendererWidth,storage_->rendererHeight);
        advanceText(v.status17C,dt,storage_->rendererWidth,storage_->rendererHeight);
        advanceText(v.time164,dt,storage_->rendererWidth,storage_->rendererHeight);
        for(auto& t:v.explosions16C)advanceText(t,dt,storage_->rendererWidth,storage_->rendererHeight);
        for(auto* g:{&v.gleam184,&v.recordGleam194})if(g->active11C){
            g->elapsed124=g->elapsed124+dt;if(g->elapsed124>g->duration120)g->active11C=0; //2E6E50.
        }
    }
    return true;
}
bool OriginalRaceHud50KG::commitNativePresentation(std::uint32_t view,
    const OriginalCameraViewportFields& viewport) noexcept {
    if(!loaded()||view>=storage_->count||storage_->views[view]->status17C.viewport!=&viewport)return false;
    auto& v=*storage_->views[view];
    const auto commit=[](Text& t){
        if(!t.enabled70||!t.blinkOn80)return;
        // The viewport-owned29D464 path applies these values unconditionally;
        // the dirty-bit-only branch belongs to overlays without a viewport.
        t.screenColor54=t.color170;t.screenAlpha58=static_cast<std::int16_t>(t.alpha174);
        t.screenScale40=t.scale178;t.screenRotation44=t.rotation17C;
        //29D638..29D644 copies the overlay's live justification before2E0170.
        //The constructor's direct29DC60(1) is only the initial ScreenText value.
        t.screenJustification50=t.justification74;
    };
    for(auto& t:v.records12C)commit(t);commit(v.status17C);commit(v.time164);
    for(auto& t:v.explosions16C)commit(t);
    return true;
}

}
