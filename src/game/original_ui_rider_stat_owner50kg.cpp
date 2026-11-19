#include "original_ui_rider_stat_owner50kg.h"
#include "original_ui_text_position50kg.h"
#include "../assets/dlc_registry50dl.h"
#include <cmath>
#include <utility>
namespace rr::game {
namespace {
using D=OriginalUiRiderStatOwnerDiagnostic50KG;using E=D::Error;using A=assets::OriginalUiRiderStatAsset50KG;
bool fail(D& d,E e) noexcept {d.error=e;return false;}
float add(float a,float b) noexcept {volatile float v=a+b;return v;}
float sub(float a,float b) noexcept {volatile float v=a-b;return v;}
float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
bool count(float f,std::int32_t& n) noexcept {
    if(!std::isfinite(f)||!roundOriginalUiTextMetric50KG(add(mul(20.f,f),.5f),1.f,n))return false;
    if(n>20)n=20;return n>=0;
}
struct Sprite {
    std::uint32_t offset=0,color=0xffffff;
    const assets::OriginalUiRiderStatResource50KG* resource=nullptr;
    float width=0,height=0;
};
}
struct OriginalUiRiderStatOwner50KG::Storage {
    std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> assets;
    std::vector<Sprite> sprites;
    OriginalUiTextureAnimation50KG marker;
    std::array<std::uint32_t,6> labelColor{{0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff}};
    std::uint32_t rider=0;
    bool appearance=false;
    Sprite* find(std::uint32_t offset) noexcept {for(auto& s:sprites)if(s.offset==offset)return &s;return nullptr;}
};
OriginalUiRiderStatOwner50KG::OriginalUiRiderStatOwner50KG() noexcept=default;
OriginalUiRiderStatOwner50KG::~OriginalUiRiderStatOwner50KG()=default;
bool OriginalUiRiderStatOwner50KG::bind(std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> assets,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound);if(!assets||!assets->loaded())return fail(d,E::resource);
    try {
        auto s=std::make_unique<Storage>();s->assets=std::move(assets);s->sprites.reserve(198);
        const auto addSprite=[&](std::uint32_t off,const assets::OriginalUiRiderStatResource50KG* r)->bool {
            if(!r||r->images.empty())return false;
            const auto images=s->assets->images();const auto* bank=images?images->textures():nullptr;
            const auto* image=bank?bank->texture(r->images[0]):nullptr;
            if(!image)return false;
            // Every texture visited by1ECBF0 is an authenticated standalone
            // one-mip PST.24E4A0 clamps all resident-level choices to count-1,
            // hence its texture+10 floor cannot change the selected level0.
            // Keep this proof explicit instead of inventing material draw order.
            if(!r->animated){const auto* decoded=images->decodedTexture(r->images[0]);
                if(!decoded||decoded->mips.size()!=1)return false;}
            Sprite p;p.offset=off;p.resource=r;
            p.width=float(image->texture.width);p.height=float(image->texture.height);
            if(!(p.width>0&&p.height>0))return false;s->sprites.push_back(p);return true;
        };
        for(unsigned i=0;i<13;++i){
            if(!addSprite(0x540+i*0x60,s->assets->resource(A::medalOutline)))return fail(d,E::resource);
            for(unsigned medal=0;medal<3;++medal)
                if(!addSprite(0xa20+i*0x120+medal*0x60,s->assets->resource(static_cast<A>(unsigned(A::medalGold)+medal))))return fail(d,E::resource);
        }
        if(!addSprite(0x18c0,s->assets->resource(A::careerMarker))||!addSprite(0x2340,s->assets->resource(A::panelLeft))||
            !addSprite(0x23a0,s->assets->resource(A::panelRight)))return fail(d,E::resource);
        for(unsigned rider=1;rider<=26;++rider)if(const auto* portrait=s->assets->portrait(static_cast<std::uint8_t>(rider)))
            if(!addSprite(0x1920+rider*0x60,portrait))return fail(d,E::resource);
        for(unsigned row=0;row<6;++row)for(unsigned n=0;n<20;++n){
            if(!addSprite(0x2400+row*0x780+n*0x60,s->assets->resource(A::notch)))return fail(d,E::resource);
            s->sprites.back().width=5;s->sprites.back().height=10;
        }
        const auto* marker=s->assets->resource(A::careerMarker);
        if(!marker||!marker->animated||marker->material.passes.size()!=1)return fail(d,E::resource);
        const auto& section=marker->material.passes[0].authored;const auto* mode=section.find("TextureAnimation");
        if(!mode||mode->text!="unique")return fail(d,E::resource);
        OriginalUiTextureAnimationConfig50KG config;std::int32_t looping=1,reverse=0,end=-1;
        if(!assets::originalUiNumber50KG(section,"animatedMapInitialFrame",0,config.initialFrame,d.parse,0x29b950)||
            !assets::originalUiNumber50KG(section,"animatedMapFramesPerSecond",1,config.framesPerSecond,d.parse,0x29b950)||
            !assets::originalUiInteger50KG(section,"animatedMapLooping",1,looping,d.parse,0x29b950)||
            !assets::originalUiInteger50KG(section,"animatedMapReverseLoopOnEnd",0,reverse,d.parse,0x29b950)||
            !assets::originalUiInteger50KG(section,"animatedMapLoopOnEndToFrame",-1,end,d.parse,0x29b950))return fail(d,E::resource);
        config.looping=looping!=0;config.reverseLoopOnEnd=reverse!=0;config.loopOnEndToFrame=end;
        if(!initializeOriginalUiTextureAnimation50KG(s->marker,config,static_cast<std::uint32_t>(marker->images.size())))return fail(d,E::resource);
        storage_=std::move(s);return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderStatOwner50KG::applyAppearance(const OriginalUiRiderStatInput50KG& in,D& d) noexcept {
    // PORT EXTENSION (DLC): a registry add-on number has no original portrait object.
    const bool addon=rr::assets::dlc50DL::addonNumberRange(in.selectedRider);
    d={};if(!storage_||!in.profileReady||in.selectedRider!=in.observedRider||
        (!addon&&(in.selectedRider>26||!storage_->assets->portrait(static_cast<std::uint8_t>(in.selectedRider)))))return fail(d,E::input);
    std::array<std::int32_t,6> current{},maximum{};
    for(unsigned i=0;i<6;++i)if(!count(in.current1920[i],current[i])||!count(in.maximum0408[i],maximum[i]))return fail(d,E::arithmetic);
    auto& s=*storage_;const auto color=in.accentColor17FA00;
    s.find(0x2340)->color=color;s.find(0x23a0)->color=color;s.labelColor.fill(color);
    for(unsigned i=0;i<13;++i)s.find(0x540+i*0x60)->color=color;
    for(unsigned row=0;row<6;++row)for(std::int32_t n=0;n<maximum[row];++n)
        s.find(0x2400+row*0x780+unsigned(n)*0x60)->color=n<current[row]?color:0x333333u;
    s.rider=in.selectedRider;s.appearance=true;return true;
}
bool OriginalUiRiderStatOwner50KG::project(const OriginalUiRiderStatInput50KG& in,const OriginalUiRiderStatClock50KG& clock,
    OriginalUiRiderStatFrame50KG& output,D& d) noexcept {
    d={};if(!storage_||!storage_->appearance||storage_->rider!=in.selectedRider)return fail(d,E::appearance);
    if(!clock.known)return fail(d,E::clock);
    OriginalUiRiderStatProjection50KG projection;if(!projectOriginalUiRiderStats50KG(in,projection))return fail(d,E::input);
    try {
        auto& s=*storage_;auto states=s.sprites;auto marker=s.marker;std::uint32_t markerFrame=0;
        if(clock.advanceAnimations){if(!advanceOriginalUiTextureAnimation50KG(marker,clock.milliseconds,markerFrame))return fail(d,E::clock);}
        else if(!selectOriginalUiTextureAnimationFrame50KG(marker,markerFrame))return fail(d,E::clock);
        OriginalUiRiderStatFrame50KG frame;frame.packets.reserve(projection.count);
        frame.lockedWidgetX=projection.lockedWidgetX;frame.lockedWidgetY=projection.lockedWidgetY;
        const auto images=s.assets->images();const auto* fontResource=s.assets->resource(A::labelFont);
        const auto fontImage=fontResource->images[0];const auto* font=images->font(fontImage);
        const auto* atlas=images->textures()->texture(fontImage);
        if(!font||!atlas)return fail(d,E::resource);
        for(unsigned index=0;index<projection.count;++index){const auto& draw=projection.draws[index];d.objectOffset=draw.objectOffset;
            OriginalUiRiderStatPacket50KG packet;packet.objectOffset=draw.objectOffset;packet.sourcePC=draw.sourcePC;
            if(draw.kind==OriginalUiRiderStatKind50KG::label){
                const auto label=unsigned(draw.localizationIndex-200);if(label>=6)return fail(d,E::input);
                const auto* text=s.assets->label(static_cast<std::uint8_t>(label));if(!text)return fail(d,E::resource);
                OriginalUiTextInput50KG textInput;textInput.font=font;textInput.atlas=&atlas->texture;
                textInput.text=reinterpret_cast<const std::uint8_t*>(text->data());textInput.length=text->size();
                auto& glyphs=frame.labels[label];if(!layoutOriginalUiText50KG(textInput,glyphs,d.text))return fail(d,E::text);
                std::int32_t width=0,x=0;
                if(!roundOriginalUiTextMetric50KG(glyphs.widthBeforeScaleAndRounding,1,width)||
                    !roundOriginalUiTextMetric50KG(sub(draw.x,float(width)),1,x))return fail(d,E::arithmetic);
                // Exact right justification2E1534: integer X, unrounded Y.
                textInput.localX=float(x);textInput.localY=draw.y;
                if(!layoutOriginalUiText50KG(textInput,glyphs,d.text))return fail(d,E::text);
                frame.labelColors[label]=originalUiTextPacketColor50KG(s.labelColor[label],255);
                packet.resource=fontResource;packet.imageIndex=fontImage;packet.labelIndex=static_cast<std::int8_t>(label);
            }else{
                Sprite* state=nullptr;for(auto& p:states)if(p.offset==draw.objectOffset){state=&p;break;}
                if(!state)return fail(d,E::resource);
                if(draw.widthOverride!=0)state->width=draw.widthOverride;if(draw.heightOverride!=0)state->height=draw.heightOverride;
                packet.resource=state->resource;packet.imageIndex=state->resource->images[state->resource->animated?markerFrame:0];
                auto& q=packet.sprite;
                q.materialName=state->resource->registeredName.c_str();q.authoredDepth=1;q.sourcePC=draw.sourcePC;
                q.sourcePacketColor=originalUiTextPacketColor50KG(state->color,255);
                const float right=add(draw.x,state->width),bottom=add(draw.y,state->height);
                if(!std::isfinite(right)||!std::isfinite(bottom))return fail(d,E::arithmetic);
                q.vertices={{{draw.x,draw.y,0,0},{right,draw.y,1,0},{draw.x,bottom,0,1},{right,bottom,1,1}}};
            }
            frame.packets.push_back(packet);
        }
        s.sprites=std::move(states);s.marker=marker;output=std::move(frame);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
}
