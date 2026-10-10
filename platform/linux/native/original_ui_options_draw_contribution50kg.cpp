#include "original_ui_options_draw_contribution50kg.h"
#include <cmath>
#include <utility>
namespace rr::platform {
namespace {
using Error=OriginalUiOptionsContributionError50KG;
using Diagnostic=OriginalUiOptionsContributionDiagnostic50KG;
using WidgetStatus=game::OriginalUiOptionsWidgetStatus50KG;
using Segment=OriginalUiOptionsSegment50KG;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(Diagnostic& d,Error e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;return false;}
}
struct OriginalUiOptionsDrawContribution50KG::Storage {
    std::shared_ptr<OriginalUiOptionsDrawResources50KG> resources;
    std::vector<game::OriginalUiTextureAnimation50KG> animation;
    std::vector<std::uint32_t> frames;
    std::vector<assets::Vertex48B> vertices;
    std::vector<OriginalUiOptionsPacket50KG> packets;
    OriginalUiOptionsClock50KG clock;
    bool active=false,ordinary=false,page=false,pageShown=false,prompts=false,top=false,finished=false,failed=false;
    std::uint8_t apertureCount=0;
};
OriginalUiOptionsDrawContribution50KG::OriginalUiOptionsDrawContribution50KG() noexcept=default;
OriginalUiOptionsDrawContribution50KG::~OriginalUiOptionsDrawContribution50KG()=default;
bool OriginalUiOptionsDrawContribution50KG::bind(std::shared_ptr<OriginalUiOptionsDrawResources50KG> resources,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyBound);
    if(!resources||!resources->loaded())return fail(d,Error::notBound);
    try {
        auto s=std::make_unique<Storage>();s->resources=std::move(resources);
        const auto& materials=*s->resources->materials();s->animation.resize(materials.size());s->frames.resize(materials.size());
        for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
            !game::initializeOriginalUiTextureAnimation50KG(s->animation[i],materials[i].animation,
                static_cast<std::uint32_t>(materials[i].images.size())))return fail(d,Error::clock);
        s->vertices.reserve(originalUiMaximumQuads50KG*4u);s->packets.reserve(originalUiMaximumSpans50KG);
        storage_=std::move(s);return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiOptionsDrawContribution50KG::begin(const OriginalUiOptionsClock50KG& clock,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);if(!clock.known)return fail(d,Error::clock);
    auto& s=*storage_;s.clock=clock;s.active=true;s.ordinary=s.page=s.pageShown=s.prompts=s.top=s.finished=s.failed=false;
    s.apertureCount=0;s.vertices.clear();s.packets.clear();
    const auto& materials=*s.resources->materials();
    for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
        !game::selectOriginalUiTextureAnimationFrame50KG(s.animation[i],s.frames[i])) {s.failed=true;return fail(d,Error::clock);}
    return true;
}
bool OriginalUiOptionsDrawContribution50KG::append(const game::OriginalUiOverlayQuad50KG& quad,
    std::uint32_t material,float z,Segment segment,std::uint16_t widgetId,Diagnostic& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(material>=materials.size()||!std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z)return fail(d,Error::geometry);
    if(s.vertices.size()/4>=originalUiMaximumQuads50KG||s.packets.size()>=originalUiMaximumSpans50KG)return fail(d,Error::capacity);
    const auto& binding=materials[material];
    if(binding.animated&&s.clock.advanceAnimations&&
        !game::advanceOriginalUiTextureAnimation50KG(s.animation[material],s.clock.milliseconds,s.frames[material]))return fail(d,Error::clock);
    const auto frame=s.frames[material];if(!binding.images.empty()&&frame>=binding.images.size())return fail(d,Error::clock);
    const auto image=binding.images.empty()?unresolved:binding.images[frame];
    constexpr unsigned order[]={0,1,3,2};std::array<assets::Vertex48B,4> vertices{};
    for(unsigned i=0;i<4;++i) {
        const auto& v=quad.vertices[order[i]];auto& out=vertices[i];
        if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v)||quad.sourcePacketColor[3]>128)
            return fail(d,Error::geometry);
        out.position[0]=v.x;out.position[1]=v.y;out.position[2]=z;out.uv[0]=v.u;out.uv[1]=v.v;
        for(unsigned c=0;c<4;++c)out.color[c]=quad.sourcePacketColor[c];
    }
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
    s.vertices.insert(s.vertices.end(),vertices.begin(),vertices.end());
    s.packets.push_back({first,1,material,image,frame,ordinal,quad.sourcePC,widgetId,segment});return true;
}
bool OriginalUiOptionsDrawContribution50KG::widgets(bool top,bool shown,Diagnostic& d) noexcept {
    auto& s=*storage_;auto* bank=s.resources->widgets();const auto* order=bank->order();if(!order)return fail(d,Error::projection);
    const auto& ids=top?order->onTopWidgets:order->ordinaryWidgets;
    const auto segment=top?Segment::onTopWidgets:Segment::ordinaryWidgets;
    for(auto id:ids) {
        d.widgetId=id;const auto material=s.resources->widgetMaterial(id);
        if(const auto* text=s.resources->textGeometry(id)) {
            if(material>=s.resources->materials()->size())return fail(d,Error::material);
            const auto& binding=(*s.resources->materials())[material];const auto* descriptor=bank->textDescriptor(id);
            if(!descriptor||descriptor->depth!=1||binding.images.size()!=1)return fail(d,Error::geometry);
            for(std::size_t lineIndex=0;lineIndex<text->lines().size();++lineIndex) {
                const auto& line=text->lines()[lineIndex];game::OriginalUiOptionsTextProjection50KG live;
                const auto status=bank->projectText(id,lineIndex,line.glyphs.fontLineHeight,line.measuredWidth,line.measuredHeight,shown,live,d.widgets);
                if(status==WidgetStatus::hidden)continue;if(status!=WidgetStatus::ok)return fail(d,Error::projection);
                if(live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128||
                    live.position.positionedX!=line.position.positionedX||live.position.positionedY!=line.position.positionedY||
                    live.line.text!=&descriptor->lines[lineIndex])return fail(d,Error::projection);
                if(!line.glyphs.count)continue;
                if(s.vertices.size()/4+line.glyphs.count>originalUiMaximumQuads50KG||s.packets.size()>=originalUiMaximumSpans50KG)
                    return fail(d,Error::capacity);
                const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
                for(std::size_t q=0;q<line.glyphs.count;++q)for(const auto& glyph:line.glyphs.quads[q].vertices) {
                    assets::Vertex48B v{};v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=16777214.f;
                    v.uv[0]=glyph.u;v.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)v.color[c]=live.line.packetColor[c];s.vertices.push_back(v);
                }
                s.packets.push_back({first,line.glyphs.count,material,binding.images[0],0,ordinal,0x2E0A30,id,segment});
            }
        } else if(bank->iconDescriptor(id)) {
            game::OriginalUiOptionsIconProjection50KG live;const auto status=bank->projectIcon(id,shown,live,d.widgets);
            if(status==WidgetStatus::hidden)continue;if(status!=WidgetStatus::ok)return fail(d,Error::projection);
            if(material>=s.resources->materials()->size())return fail(d,Error::material);
            const auto& binding=(*s.resources->materials())[material];
            if(binding.images.size()!=1||live.images!=s.resources->images()||live.imageIndex!=binding.images[0])return fail(d,Error::projection);
            if(!append(live.layout.quad,material,live.layout.packetDepth,segment,id,d))return false;
        }
        // Original Movement_Control5 has no geometry.
    }
    return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectOrdinaryWidgets(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||s.ordinary||s.page||s.top||s.finished||s.failed)return fail(d,Error::sourceOrder);
    if(!widgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectPage(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.page||s.top||s.finished||s.failed)return fail(d,Error::sourceOrder);
    s.page=true;s.pageShown=shown;if(!shown)return true;
    const auto background=s.resources->materialIndex("ui/textures/mmbkg");
    for(const auto& q:s.resources->background()->quadrants)
        if(!append(q,background,16777200.f,Segment::page,0,d)){s.failed=true;return false;}
    game::OriginalUiOptionsSelection50KG selection;
    if(!s.resources->selection(selection,d.resource)){s.failed=true;return fail(d,Error::geometry);}
    if(!append(selection.beforeText,s.resources->materialIndex("ui/textures/arrwrght"),16777214.f,Segment::page,selection.textWidgetId,d)||
        !append(selection.afterText,s.resources->materialIndex("ui/textures/arrwleft"),16777214.f,Segment::page,selection.textWidgetId,d))
        {s.failed=true;return false;}
    return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectAperture(std::uint8_t ordinal,
    const game::OriginalUiApertureRectangle50KG& rect,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.top||s.prompts||s.failed||s.finished||ordinal!=s.apertureCount||ordinal>1||
        rect.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rect.dirty3C)return fail(d,Error::sourceOrder,0x1D798C);
    game::OriginalUiOverlayQuad50KG q;q.materialName="NoTexture";q.authoredDepth=1;q.sourcePC=0x1D798C;q.sourcePacketColor={{0,0,0,63}};
    constexpr unsigned order[]={0,2,1,3};
    for(unsigned i=0;i<4;++i){const auto& v=rect.vertices[order[i]];q.vertices[i]={v.x,v.y,0,0};}
    if(!append(q,s.resources->materialIndex("ui/textures/notexture"),16777214.f,Segment::aperture,0,d)){s.failed=true;return false;}
    ++s.apertureCount;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectPrompts(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertureCount!=2||s.prompts||s.top||s.failed||s.finished)return fail(d,Error::sourceOrder);
    const auto mask=s.resources->widgets()->pageInputFlags42C();if(mask!=0&&mask!=3)return fail(d,Error::projection,0x1F9E30);
    constexpr const char* names[]={"ui/textures/icon_x","ui/textures/icon_tri"};constexpr float x[]={484,397};
    constexpr std::uint32_t pc[]={0x1D7A2C,0x1D7A44};
    for(unsigned i=0;i<2;++i)if(mask&(1u<<i)) {
        const auto m=s.resources->materialIndex(names[i]);if(m>=s.resources->materials()->size())return fail(d,Error::material);
        const auto& b=(*s.resources->materials())[m];if(b.images.size()!=1)return fail(d,Error::material);
        const auto* atlas=s.resources->textures()->texture(b.images[0]);if(!atlas)return fail(d,Error::geometry);
        const float w=float(atlas->texture.width),h=float(atlas->texture.height);game::OriginalUiOverlayQuad50KG q;
        q.materialName=names[i];q.authoredDepth=1;q.sourcePC=pc[i];
        q.vertices={{{x[i],378,0,0},{x[i]+w,378,1,0},{x[i],378+h,0,1},{x[i]+w,378+h,1,1}}};
        if(!append(q,m,16777214.f,Segment::prompts,0,d)){s.failed=true;return false;}
    }
    s.prompts=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectOnTopWidgets(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||s.top||s.failed||s.finished||
        (s.pageShown&&(!s.prompts||s.apertureCount!=2)))return fail(d,Error::sourceOrder);
    if(!widgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::finishContribution(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||!s.top||s.failed||s.finished||
        (s.pageShown&&(!s.prompts||s.apertureCount!=2)))return fail(d,Error::unfinished);
    s.finished=true;return true;
}
const std::vector<assets::Vertex48B>* OriginalUiOptionsDrawContribution50KG::vertices() const noexcept {
    return storage_&&storage_->finished&&!storage_->failed?&storage_->vertices:nullptr;
}
const std::vector<OriginalUiOptionsPacket50KG>* OriginalUiOptionsDrawContribution50KG::packets() const noexcept {
    return storage_&&storage_->finished&&!storage_->failed?&storage_->packets:nullptr;
}
const OriginalUiOptionsDrawResources50KG* OriginalUiOptionsDrawContribution50KG::resources() const noexcept {return storage_?storage_->resources.get():nullptr;}
}
