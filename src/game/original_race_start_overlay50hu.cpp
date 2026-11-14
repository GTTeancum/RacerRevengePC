#include "original_race_start_overlay50hu.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <new>
#include <string>
#include <utility>
#include <vector>

namespace rr::game {
namespace {
using A=assets::OriginalUiArchiveId50KG;
using E=OriginalRaceStartOverlayDiagnostic50HU::Error;
bool genuine(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,A id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
bool fail(OriginalRaceStartOverlayDiagnostic50HU& d,E e,std::uint32_t pc,std::size_t texture=0) noexcept {
    d.error=e;d.sourcePC=pc;d.texture=texture;return false;
}
// Strict binary32 steps, as the other 50LA/50KG HUD owners do.
float sub(float a,float b) noexcept {volatile float v=a-b;return v;}
float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
float add(float a,float b) noexcept {volatile float v=a+b;return v;}
float div(float a,float b) noexcept {volatile float v=a/b;return v;}
// 1976E0 layout constants (top-left, before the view-mode offset).
// x: 343 = 0x43AB8000, 286 = 0x438F0000, 230 = 0x43660000, 255 = 0x437F0000.
// y: 121 = 0x42F20000, 125 = 0x42FA0000, 30 = 0x41F00000.
constexpr float layoutX[8]={343,343,286,286,230,230,255,255};
constexpr float layoutY[8]={121,121,125,125,121,121,30,30};
// 197040 initial positions: identical except "3"/"3h" at 229 (0x43650000).
constexpr float loadX[8]={343,343,286,286,229,229,255,255};
// 197C10 order and once-flags: "3" (+1C0/+220, flag 344, event 3),
// "2" (+100/+160, 348, event 2), "1" (+40/+A0, 34C, event 1).
struct Digit {std::uint8_t plain,lit;float threshold;std::uint32_t event;std::uint32_t pc;};
constexpr Digit digits[3]={{4,5,3.f,3u,0x197da4u},{2,3,2.f,2u,0x197ee0u},{0,1,1.f,1u,0x19801cu}};
}
struct OriginalRaceStartOverlay50HU::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,overlays;
    assets::OriginalUiMaterialTemplates50KG templates;
    assets::OriginalUiScreenAssets50KG images;
    std::array<assets::OriginalUiMaterial50KG,textureCount> materials;
    struct Texture {
        float x10=0,y14=0;        // ScreenTexture +10/+14 (top-left)
        float width34=0,height38=0; // 2DF0B0 with f12=f13=0: decoded texture size
        std::int16_t alpha48=255; // cached alpha written by 197C10 (+268 etc.)
    };
    std::array<Texture,textureCount> textures{};
    // 196F20: age340 = 0.75 (3F400000), once-flags 344/348/34C/350 = 0.
    float age340=0.75f;
    std::array<std::uint32_t,4> once{};
    std::uint32_t viewMode354=0;
    OriginalRaceStartOverlayDraw50HU draw{};
    bool loaded=false,attached=false;
};
OriginalRaceStartOverlay50HU::OriginalRaceStartOverlay50HU() noexcept:storage_(new(std::nothrow) Storage){}
OriginalRaceStartOverlay50HU::~OriginalRaceStartOverlay50HU()=default;
bool OriginalRaceStartOverlay50HU::loaded() const noexcept{return storage_&&storage_->loaded;}
bool OriginalRaceStartOverlay50HU::attached() const noexcept{return loaded()&&storage_->attached;}
float OriginalRaceStartOverlay50HU::age340() const noexcept{return storage_?storage_->age340:-1.f;}
const OriginalRaceStartOverlayDraw50HU& OriginalRaceStartOverlay50HU::nativeDraw() const noexcept {
    static const OriginalRaceStartOverlayDraw50HU none{};return storage_?storage_->draw:none;
}
const assets::OriginalUiScreenAssets50KG* OriginalRaceStartOverlay50HU::images() const noexcept {
    return loaded()?&storage_->images:nullptr;
}
const assets::OriginalUiMaterial50KG* OriginalRaceStartOverlay50HU::material(std::size_t i) const noexcept {
    return loaded()&&i<textureCount?&storage_->materials[i]:nullptr;
}
bool OriginalRaceStartOverlay50HU::load197040(std::shared_ptr<const assets::OriginalUiArchive50KG> app,
    std::shared_ptr<const assets::OriginalUiArchive50KG> overlays,
    const assets::OriginalUiMaterialRuntime50KG& runtime,OriginalRaceStartOverlayDiagnostic50HU& d) noexcept {
    d={};
    if(!storage_)return fail(d,E::allocation,0x196f20);
    if(storage_->loaded)return fail(d,E::alreadyLoaded,0x197040);
    if(!genuine(app,A::appStart)||!genuine(overlays,A::overlays))return fail(d,E::archive,0x197040);
    try {
        // Build a fresh owner; publish it only after every texture, material
        // and image succeeded. The 196F20 fields start at their defaults.
        auto n=std::make_unique<Storage>();auto& s=*n;
        s.appStart=std::move(app);s.overlays=std::move(overlays);
        if(!s.templates.load(*s.appStart,d.material))return fail(d,E::material,0x2df10c);
        std::vector<assets::OriginalUiImageSource50KG> sources;sources.reserve(textureCount);
        for(std::size_t i=0;i<textureCount;++i){
            const auto image=static_cast<assets::OriginalRaceStartOverlayImage50HU>(i);
            if(!assets::readOriginalRaceStartOverlayFallbackMaterial50HU(*s.overlays,image,s.templates,
                assets::OriginalUiTemplate50KG::screenMt,runtime,s.materials[i],d.material))
                return fail(d,E::material,0x2475c0,i);
            const auto pst=s.overlays->find(std::string("ui\\textures\\")+
                assets::originalRaceStartOverlayImageName50HU(image)+".pst");
            if(!pst.identity||!pst.data||pst.size!=pst.identity->bytes)return fail(d,E::images,0x2df0b0,i);
            sources.push_back({0x19704000u+i,pst,0,pst.size,{}});
        }
        if(!s.images.load(sources,d.images))return fail(d,E::images,0x2df0b0,d.images.image);
        const auto* bank=s.images.textures();
        if(!bank||bank->textureCount()!=textureCount)return fail(d,E::images,0x2df0b0);
        for(std::size_t i=0;i<textureCount;++i){const auto* t=bank->texture(static_cast<std::uint32_t>(i));
            // 2DF180..2DF1C4: +34/+38 are the texture's own integer width/height.
            if(!t||!t->texture.width||!t->texture.height||t->texture.width>1024||t->texture.height>1024)
                return fail(d,E::images,0x2df1b0,i);
            s.textures[i].width34=static_cast<float>(t->texture.width);
            s.textures[i].height38=static_cast<float>(t->texture.height);
            s.textures[i].x10=loadX[i];s.textures[i].y14=layoutY[i];s.textures[i].alpha48=255;
        }
        s.loaded=true;storage_=std::move(n);
        std::cerr<<"race_start_overlay load197040 textures=8 sizes=";
        for(std::size_t i=0;i<textureCount;++i)
            std::cerr<<(i?",":"")<<storage_->textures[i].width34<<'x'<<storage_->textures[i].height38;
        std::cerr<<'\n';
        return true;
    }catch(...){return fail(d,E::allocation,0x197040);}
}
namespace {
// 1976E0(this, oneBasedView). Exact branch structure:
//   +354 == 0: (dx,dy) = (0,0)
//   +354 == 1: dx = 0; dy = view==1 ? 109 (0x42DA0000) : -115 (0xC2E60000)
//   otherwise: dy = 109; dx = view==1 ? 0 : -320 (0xC3A00000)
void layoutOffsets(std::uint32_t mode,std::uint32_t view,float& dx,float& dy) noexcept {
    if(!mode){dx=0;dy=0;return;}
    if(mode==1){dx=0;dy=view==1?109.f:-115.f;return;}
    dy=109.f;dx=view==1?0.f:-320.f;
}
}
bool OriginalRaceStartOverlay50HU::attach1976B0(std::uint32_t viewMode) noexcept {
    if(!loaded()||storage_->attached)return false;
    auto& s=*storage_;s.viewMode354=viewMode;s.attached=true;
    float dx=0,dy=0;layoutOffsets(s.viewMode354,1,dx,dy);
    for(std::size_t i=0;i<textureCount;++i){s.textures[i].x10=add(layoutX[i],dx);s.textures[i].y14=add(layoutY[i],dy);}
    std::cerr<<"race_start_overlay attach1976B0 view_mode="<<viewMode<<'\n';
    return true;
}
void OriginalRaceStartOverlay50HU::reset1983A0() noexcept {
    if(!storage_)return;
    storage_->age340=0.75f;storage_->once={};
}
bool OriginalRaceStartOverlay50HU::update198190(const OriginalRaceStartOverlayInputs50HU& in,float dt) noexcept {
    if(!attached()||!std::isfinite(dt)||dt<0||dt>1)return false;
    auto& s=*storage_;
    // 198190: flags58 bit1 (racing) gates the age; age < 0 stops the clock.
    if((in.flags58&2u)&&!(s.age340<0.f))s.age340=sub(s.age340,dt);
    return true;
}
bool OriginalRaceStartOverlay50HU::draw198300(const OriginalRaceStartOverlayInputs50HU& in) noexcept {
    if(!attached()||!std::isfinite(in.countdown34))return false;
    auto& s=*storage_;auto& out=s.draw;
    const auto serial=out.serial+1;out={};out.serial=serial;
    // 1982C0: the second view draws only when the race has two views.
    const std::uint32_t views=in.raceFlags1B0&3u;
    if(in.drawnView268==1&&views<2)return true;
    if(in.drawnView268>1)return false;
    // 198328..19836C: age >= 0, flags58 bit2 (counting), race+29 bit2 clear.
    if(s.age340<0.f||!(in.flags58&4u)||(in.raceEntityFlags29&4u))return true;
    // 198270: 1976E0(this, Application+268 + 1) while age >= 0.
    float dx=0,dy=0;layoutOffsets(s.viewMode354,in.drawnView268+1,dx,dy);
    for(std::size_t i=0;i<textureCount;++i){s.textures[i].x10=add(layoutX[i],dx);s.textures[i].y14=add(layoutY[i],dy);}
    // 197C10.
    float f=1.f;if(s.age340<=0.75f)f=div(s.age340,0.75f);
    float rise=mul(100.f,sub(1.f,f));if(s.viewMode354)rise=0.f;
    const float scaled=mul(255.f,f);
    if(!std::isfinite(scaled))return false;
    const auto alpha=static_cast<std::int16_t>(static_cast<std::int32_t>(scaled)); //310F38 truncation.
    const auto emit=[&](std::uint8_t index,bool lit){
        auto& t=s.textures[index];
        if(lit){if(t.alpha48!=alpha)t.alpha48=alpha;
            const float y=sub(t.y14,rise);if(t.y14!=y)t.y14=y;}
        auto& sprite=out.sprites[out.count++];
        sprite.x=t.x10;sprite.y=t.y14;sprite.width=t.width34;sprite.height=t.height38;
        // 2DF390..2DF3D8: RGB = colour>>1 (255 -> 127), alpha = +48 >> 2.
        sprite.color={{127,127,127,static_cast<std::uint8_t>((t.alpha48>>2)&0xff)}};
        sprite.texture=index;
    };
    const auto sound=[&](std::size_t once,std::uint32_t event){
        if(s.once[once])return;
        out.sounds[out.soundCount++]=event;s.once[once]=1;
    };
    for(std::size_t i=0;i<3;++i){const auto& g=digits[i];
        const bool lit=in.countdown34<g.threshold;
        emit(lit?g.lit:g.plain,lit);
        if(lit){sound(i,g.event);out.lit=g.event;}
    }
    const bool go=(in.flags58&2u)!=0;
    emit(go?7:6,go);
    if(go){sound(3,4);out.lit=4;} //198150.
    out.drawn=true;
    return true;
}
}
