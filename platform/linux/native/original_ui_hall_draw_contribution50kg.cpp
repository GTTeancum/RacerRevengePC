#include "original_ui_hall_draw_contribution50kg.h"
#include "original_ui_hall_addon50dl.h"
#include "original_ui_hall_background_adapter50kg.h"
#include "original_ui_material_adapter50kg.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include <limits>
#include <algorithm>
#include <cmath>
#include <utility>
namespace rr::platform {
namespace {
using Error=OriginalUiHallContributionError50KG;
using Diagnostic=OriginalUiHallContributionDiagnostic50KG;

using Segment=OriginalUiHallSegment50KG;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(Diagnostic& d,Error e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;return false;}
// PORT EXTENSION (DLC add-on course, 148r): the one per-frame record
// (original_ui_hall_addon50dl.h). Owner thread only.
thread_local OriginalUiHallAddonFrame50DL* hallAddonFrame=nullptr;
}
void setOriginalUiHallAddonFrame50DL(OriginalUiHallAddonFrame50DL* frame) noexcept {hallAddonFrame=frame;}
OriginalUiHallAddonFrame50DL* originalUiHallAddonFrame50DL() noexcept {return hallAddonFrame;}
struct OriginalUiHallDrawContribution50KG::Storage {
    std::shared_ptr<OriginalUiHallDrawResources50KG> resources;
    std::vector<game::OriginalUiTextureAnimation50KG> animation;
    std::vector<std::uint32_t> frames;
    std::vector<assets::Vertex48B> vertices;
    std::vector<OriginalUiHallPacket50KG> packets;
    std::vector<float> fog,drainedFog,w,drainedW;
    std::vector<assets::OriginalUiMaterialBindingState50KG> history,pendingHistory;
    std::vector<std::uint32_t> leaf,representatives;
    std::uint64_t frameGeneration=0,attemptedGeneration=0,acknowledgedGeneration=0;
    const VehicleWindowGl50DS* submittedRenderer=nullptr;
    bool submissionPending=false,historyPoisoned=false;
    bool prepareGroups(Diagnostic& d){
        const auto& bindings=*resources->materials();const auto count=bindings.size();
        std::vector<std::uint32_t> sequence;sequence.reserve(count);history.resize(count);leaf.resize(count);
        for(std::size_t i=0;i<count;++i){const auto& b=bindings[i];
            if(b.material.passes.size()!=1)return fail(d,Error::material);
            const auto& pass=b.material.passes[0];assets::OriginalUiMaterialEquivalenceKey50KG key;
            assets::OriginalUiMaterialEquivalenceDiagnostic50KG keyDetail;assets::OriginalUiMaterialBindingDiagnostic50KG detail;
            const auto* source=b.images.empty()?nullptr:resources->images()->sourceProvenance(b.images[0]);
            if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,source,key,keyDetail)||!history[i].initialize(key,detail))
                return fail(d,Error::material,keyDetail.sourcePC?keyDetail.sourcePC:detail.sourcePC);
            sequence.push_back(static_cast<std::uint32_t>(i));
        }
        std::sort(sequence.begin(),sequence.end(),[&](std::uint32_t a,std::uint32_t b){
            return bindings[a].registryToken!=bindings[b].registryToken?bindings[a].registryToken<bindings[b].registryToken:
                bindings[a].material.passes[0].pass<bindings[b].material.passes[0].pass;
        });
        struct Group {std::uint32_t binding=0,redirect=unresolved,limit=unresolved;};
        std::vector<Group> groups;groups.reserve(count);
        std::uint32_t previous=unresolved;
        for(const auto index:sequence){const auto current=static_cast<std::uint32_t>(groups.size());
            for(auto& prior:groups){const auto equal=history[index].compare(history[prior.binding]);
                if(equal==assets::OriginalUiMaterialEquivalence50KG::unsupported)return fail(d,Error::material,0x249BA0);
                if(equal==assets::OriginalUiMaterialEquivalence50KG::equivalent&&current<prior.limit)prior.redirect=current;}
            groups.push_back({index,unresolved,unresolved});
            if(previous!=unresolved&&bindings[previous].registryToken==bindings[index].registryToken){
                if(bindings[index].material.passes[0].pass!=bindings[previous].material.passes[0].pass+1)return fail(d,Error::material,0x29BED0);
                groups[current-1].limit=current;
            }else if(bindings[index].material.passes[0].pass!=1)return fail(d,Error::material,0x29BED0);
            leaf[index]=current;previous=index;
        }
        for(auto& value:leaf){std::size_t steps=0;while(groups[value].redirect!=unresolved){
            const auto next=groups[value].redirect;if(next<=value||++steps>count)return fail(d,Error::material,0x29C150);value=next;}}
        for(const auto& group:groups)representatives.push_back(group.binding);
        pendingHistory=history;return true;
    }
    std::vector<assets::Vertex48B> drainedVertices;
    std::vector<std::uint32_t> order;
    std::vector<OriginalUiSpan50KG> spans;
    std::vector<OriginalUiNativeSpan50KG> states;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;
    std::uint64_t publication=0;
    OriginalUiNativeDraw50KG draw{};
    bool drained=false,exitText=false;
    OriginalUiHallClock50KG clock;
    std::uint32_t planet=0;
    bool active=false,ordinary=false,page=false,pageShown=false,prompts=false,top=false,finished=false,failed=false;
    std::uint8_t apertureCount=0;
};
OriginalUiHallDrawContribution50KG::OriginalUiHallDrawContribution50KG() noexcept=default;
OriginalUiHallDrawContribution50KG::~OriginalUiHallDrawContribution50KG()=default;
bool OriginalUiHallDrawContribution50KG::bind(std::shared_ptr<OriginalUiHallDrawResources50KG> resources,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyBound);
    if(!resources||!resources->loaded())return fail(d,Error::notBound);
    try {
        auto s=std::make_unique<Storage>();s->resources=std::move(resources);
        const auto& materials=*s->resources->materials();s->animation.resize(materials.size());s->frames.resize(materials.size());
        for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
            !game::initializeOriginalUiTextureAnimation50KG(s->animation[i],materials[i].animation,
                static_cast<std::uint32_t>(materials[i].images.size())))return fail(d,Error::clock);
        s->vertices.reserve(originalUiMaximumSourceQuads50KM*4u);s->packets.reserve(originalUiMaximumSpans50KG);
        s->fog.reserve(originalUiMaximumSourceQuads50KM*4u);s->drainedFog.reserve(originalUiMaximumSourceQuads50KM*4u);
        s->drainedVertices.reserve(originalUiMaximumSourceQuads50KM*4u);s->order.reserve(originalUiMaximumSpans50KG);
        s->spans.reserve(originalUiMaximumSpans50KG);s->states.reserve(originalUiMaximumSpans50KG);
        s->w.reserve(originalUiMaximumSourceQuads50KM*4u);s->drainedW.reserve(originalUiMaximumSourceQuads50KM*4u);
        if(!s->prepareGroups(d))return false;
        storage_=std::move(s);return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiHallDrawContribution50KG::bindFrame(std::shared_ptr<OriginalUiHallDrawResources50KG> resources,
    std::shared_ptr<const MaterialTextureGl50EB> gpu,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyBound);
    if(!resources||!resources->loaded()||!gpu||!gpu->initialized()||!gpu->publicationToken()||
        gpu->textureCount()!=resources->textures()->textureCount())return fail(d,Error::publication);
    for(std::uint32_t i=0;i<gpu->textureCount();++i) {
        const auto* a=gpu->asset(i);const auto* b=resources->textures()->texture(i);
        if(!a||!b||a->identity!=b->identity)return fail(d,Error::publication);
    }
    if(!bind(std::move(resources),d))return false;
    storage_->publication=gpu->publicationToken();storage_->gpu=std::move(gpu);return true;
}
bool OriginalUiHallDrawContribution50KG::begin(const OriginalUiHallClock50KG& clock,std::uint32_t planet,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);if(!clock.known)return fail(d,Error::clock);
    auto& s=*storage_;if(s.gpu&&(!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication))return fail(d,Error::publication);
    if(s.submissionPending||s.historyPoisoned)return fail(d,Error::sourceOrder);
    if(s.resources->backgroundImage(planet)==unresolved)return fail(d,Error::input,0x1E3EF0);
    const auto image=s.resources->currentBackgroundImage();OriginalUiSpan50KG span;OriginalUiNativeSpan50KG state;
    if(image!=unresolved&&!prepareOriginalUiHallBackgroundSpan50KG(*s.resources->images(),image,span,state))return fail(d,Error::material,0x2A6E90);
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;frame_.terminal_={};
    s.w.clear();s.drainedW.clear();s.planet=planet;
    s.drained=s.exitText=false;s.fog.clear();s.drainedFog.clear();s.drainedVertices.clear();s.order.clear();s.spans.clear();s.states.clear();
    s.clock=clock;s.active=true;s.ordinary=s.page=s.pageShown=s.prompts=s.top=s.finished=s.failed=false;
    s.apertureCount=0;s.vertices.clear();s.packets.clear();
    const auto& materials=*s.resources->materials();
    for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
        !game::selectOriginalUiTextureAnimationFrame50KG(s.animation[i],s.frames[i])) {s.failed=true;return fail(d,Error::clock);}
    // Root +44 at 140738 precedes +48 at 140788. Hall raw 2A6E90 reads
    // retained page+154 before 1E3EF0 changes it, so the first frame has no raw
    // packet and later frames use the previous shown page render's selection.
    if(image==unresolved)return true;
    // Source25D198 supplies the one-texel initial UV; terminal UV has no inset.
    constexpr float xy[4][2]={{0,0},{640,0},{640,448},{0,448}};
    for(unsigned i=0;i<4;++i){assets::Vertex48B v{};
        v.position[0]=xy[i][0];v.position[1]=xy[i][1];v.position[2]=0;
        v.uv[0]=(i==1||i==2)?1.f:1.f/640;v.uv[1]=i>=2?1.f:1.f/448;
        v.color[0]=v.color[1]=v.color[2]=128;v.color[3]=64;
        s.vertices.push_back(v);s.fog.push_back(0.f);s.w.push_back(1.f);}
    s.packets.push_back({0,1,unresolved,image,0,0,0x2A6E90,0,Segment::rawBackground});return true;
}
bool OriginalUiHallDrawContribution50KG::append(const game::OriginalUiOverlayQuad50KG& quad,
    std::uint32_t material,float z,Segment segment,std::uint16_t widgetId,Diagnostic& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(material>=materials.size()||!std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z)return fail(d,Error::geometry);
    if(s.vertices.size()/4>=originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)return fail(d,Error::capacity);
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
    s.vertices.insert(s.vertices.end(),vertices.begin(),vertices.end());s.fog.insert(s.fog.end(),4,0.f);s.w.insert(s.w.end(),4,1.f);
    s.packets.push_back({first,1,material,image,frame,ordinal,quad.sourcePC,widgetId,segment});return true;
}
bool OriginalUiHallDrawContribution50KG::appendText(const game::OriginalUiTextLayout50KG& text,
    const std::array<std::uint8_t,4>& color,std::uint32_t material,std::uint32_t pc,Segment segment,std::uint16_t id,Diagnostic& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(material>=materials.size()||color[3]>128||text.count>text.quads.size())return fail(d,Error::geometry,pc);
    const auto& binding=materials[material];if(binding.images.size()!=1||binding.animated)return fail(d,Error::material,pc);
    if(!text.count)return true;
    if(s.vertices.size()/4+text.count>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)
        return fail(d,Error::capacity,pc);
    for(std::size_t q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices)
        if(!std::isfinite(glyph.x)||!std::isfinite(glyph.y)||!std::isfinite(glyph.u)||!std::isfinite(glyph.v))return fail(d,Error::geometry,pc);
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
    for(std::size_t q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices){assets::Vertex48B v{};
        v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=16777214.f;
        v.uv[0]=glyph.u;v.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)v.color[c]=color[c];
        s.vertices.push_back(v);s.fog.push_back(0.f);s.w.push_back(1.f);}
    s.packets.push_back({first,text.count,material,binding.images[0],0,ordinal,pc,id,segment});return true;
}
bool OriginalUiHallDrawContribution50KG::widgets(bool top,bool shown,Diagnostic& d) noexcept {
    auto& s=*storage_;auto* bank=s.resources->widgets();const auto* order=bank->order(top);if(!order)return fail(d,Error::projection);
    const auto segment=top?Segment::onTopWidgets:Segment::ordinaryWidgets;
    auto* addon=originalUiHallAddonFrame50DL();
    const auto addonName=[&](std::uint16_t id,bool visible,Segment where,OriginalUiHallAddonFrame50DL& frame,Diagnostic& dd)->bool{
        const auto* descriptor=bank->textDescriptor(id);const auto material=s.resources->widgetMaterial(id);
        if(!descriptor||descriptor->depth!=1||material>=s.resources->materials()->size())return fail(dd,Error::geometry);
        const auto& binding=(*s.resources->materials())[material];
        if(binding.images.size()!=1||binding.animated)return fail(dd,Error::material);
        const auto* font=s.resources->images()->font(binding.images[0]);
        const auto* atlas=s.resources->textures()->texture(binding.images[0]);
        if(!font||!atlas)return fail(dd,Error::resource);
        const auto& text=*frame.name;
        game::OriginalUiTextInput50KG input;input.font=font;input.atlas=&atlas->texture;
        input.text=reinterpret_cast<const std::uint8_t*>(text.data());input.length=text.size();
        game::OriginalUiTextLayout50KG glyphs;game::OriginalUiTextDiagnostic50KG quiet;
        std::int32_t width=0,height=0;
        if(text.empty()||!game::layoutOriginalUiText50KG(input,glyphs,quiet)||
           !game::roundOriginalUiTextMetric50KG(glyphs.widthBeforeScaleAndRounding,1,width)||
           !game::roundOriginalUiTextMetric50KG(glyphs.firstGlyphHeightBeforeScaleAndRounding,1,height)){++frame.nameSkipped;return true;}
        game::OriginalUiHallTextProjection50KG live;
        if(!bank->projectText(id,0,glyphs.fontLineHeight,width,height,visible,live,dd.widgets)){
            if(dd.widgets.error==game::OriginalUiHallWidgetError50KG::hidden){dd.widgets={};return true;}
            return fail(dd,Error::projection);}
        if(live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128){++frame.nameSkipped;return true;}
        input.localX=live.position.positionedX;input.localY=live.position.positionedY;
        if(!game::layoutOriginalUiText50KG(input,glyphs,quiet)){++frame.nameSkipped;return true;}
        if(!appendText(glyphs,live.line.packetColor,material,0x2E0A30,where,id,dd))return false;
        ++frame.nameDrawn;return true;
    };
    for(auto id:*order){d.widgetId=id;const auto material=s.resources->widgetMaterial(id);
        // PORT EXTENSION (DLC add-on course, 148r): the add-on name in place of
        // the anchor's course-name line 0. A name the font cannot lay out is
        // skipped (counted), never replaced by another text.
        if(addon&&addon->name&&id==addon->anchorName&&s.resources->textGeometry(id)){
            if(!addonName(id,shown,segment,*addon,d))return false;continue;}
        if(const auto* text=s.resources->textGeometry(id)){
            const auto* descriptor=bank->textDescriptor(id);if(!descriptor||descriptor->depth!=1)return fail(d,Error::geometry);
            for(std::size_t lineIndex=0;lineIndex<text->lines().size();++lineIndex){
                const auto* line=s.resources->textGeometryLine(id,lineIndex);if(!line)return fail(d,Error::geometry);
                game::OriginalUiHallTextProjection50KG live;
                if(!bank->projectText(id,lineIndex,line->glyphs.fontLineHeight,line->measuredWidth,line->measuredHeight,shown,live,d.widgets)){
                    if(d.widgets.error==game::OriginalUiHallWidgetError50KG::hidden)continue;return fail(d,Error::projection);}
                if(live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128||
                    live.position.positionedX!=line->position.positionedX||live.position.positionedY!=line->position.positionedY||
                    lineIndex>=descriptor->lines.size()||live.line.text!=&descriptor->lines[lineIndex])return fail(d,Error::projection);
                if(!appendText(line->glyphs,live.line.packetColor,material,0x2E0A30,segment,id,d))return false;
            }
        }else if(bank->iconDescriptor(id)){
            game::OriginalUiHallIconProjection50KG live;
            if(!bank->projectIcon(id,shown,live,d.widgets)){
                if(d.widgets.error==game::OriginalUiHallWidgetError50KG::hidden)continue;return fail(d,Error::projection);}
            if(material>=s.resources->materials()->size())return fail(d,Error::material);
            const auto& binding=(*s.resources->materials())[material];
            if(binding.images.empty()||live.images!=s.resources->images()||live.imageIndex!=binding.images[0])return fail(d,Error::projection);
            if(!append(live.layout.quad,material,live.layout.packetDepth,segment,id,d))return false;
        }
        // Movement_Control5 has no geometry; empty sentinels never draw.
    }
    return true;
}
bool OriginalUiHallDrawContribution50KG::projectOrdinaryWidgets(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||s.ordinary||s.page||s.top||s.finished||s.failed)return fail(d,Error::sourceOrder);
    if(!widgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiHallDrawContribution50KG::projectPage(const OriginalUiHallPageInput50KG& input,bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.page||s.top||s.finished||s.failed||input.planetIndex!=s.planet)return fail(d,Error::sourceOrder,0x1E3EF0);
    s.page=true;s.pageShown=shown;if(!shown)return true;
    if(!s.resources->projectPage(input,d.resource)){s.failed=true;return fail(d,Error::resource,d.resource.sourcePC);}
    const auto count=s.resources->pagePacketCount();const auto* packets=s.resources->pagePackets();
    if(!packets||count!=(input.showPlanetHeader?37u:36u)){s.failed=true;return fail(d,Error::geometry,0x1E17D0);}
    for(std::size_t i=0;i<count;++i){const auto& p=packets[i];
        if(bool(p.quad)==bool(p.text)){s.failed=true;return fail(d,Error::geometry,p.sourcePC);}
        if(p.quad){if(i>=9||!append(*p.quad,p.materialIndex,16777214.f,Segment::page,0,d)){s.failed=true;return false;}}
        else if(i<9||!appendText(*p.text,p.packetColor,p.materialIndex,p.sourcePC,Segment::page,0,d)){s.failed=true;return false;}
    }
    return true;
}
bool OriginalUiHallDrawContribution50KG::projectExitText(const game::OriginalUiApertureHook50KG& hook,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertureCount||s.prompts||s.top||s.exitText||s.failed||s.finished)
        return fail(d,Error::sourceOrder,0x1D75A4);
    // Source capacity10 upper-bounds the real glyph packet before the owner
    // performs RenderText preparation. This is not an invented loading label.
    if(s.vertices.size()/4+10>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)
        return fail(d,Error::capacity);
    const auto m=s.resources->materialIndex("font/denmrk16top");
    if(m>=s.resources->materials()->size())return fail(d,Error::material);
    const auto& binding=(*s.resources->materials())[m];
    if(binding.images.size()!=1)return fail(d,Error::material);
    game::OriginalUiExitTextProjection50KG live;
    auto* exitText=s.resources->exitText();
    if(!exitText||exitText->projectRender2E0170(hook,live)!=game::OriginalUiExitTextStatus50KG::ok||
        !live.glyphs||live.images!=s.resources->images()||live.imageIndex!=binding.images[0]||
        live.materialIdentity!=binding.material.identity||live.assignedDepth!=1||live.packetColor[3]>128||live.glyphs->count>10)
        {s.failed=true;return fail(d,Error::projection,0x2E0170);}
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
    for(std::size_t q=0;q<live.glyphs->count;++q)for(const auto& glyph:live.glyphs->quads[q].vertices) {
        assets::Vertex48B v{};v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=16777214.f;
        v.uv[0]=glyph.u;v.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)v.color[c]=live.packetColor[c];
        s.vertices.push_back(v);s.fog.push_back(0.f);s.w.push_back(1.f);
    }
    if(live.glyphs->count)s.packets.push_back({first,live.glyphs->count,m,live.imageIndex,0,
        static_cast<std::uint32_t>(s.packets.size()),live.submitPC,0,Segment::exitText});
    s.exitText=true;return true;
}
bool OriginalUiHallDrawContribution50KG::projectAperture(std::uint8_t ordinal,
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
bool OriginalUiHallDrawContribution50KG::projectPrompts(std::uint32_t mask,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertureCount!=2||s.prompts||s.top||s.failed||s.finished)return fail(d,Error::sourceOrder);
    // 1E4870: mode5 stores 42C=2 (Triangle/Back); every other mode stores 1
    // (Cross/confirm). 1D7A10 draws +2A0 for bit0 and +300 for bit1 at the
    // base page defaults (1D6874: 484,378; tri 397,378). Hall never moves them.
    if(mask!=0&&mask!=1&&mask!=2)return fail(d,Error::input,0x1D7A44);
    if(mask&1){const auto m=s.resources->materialIndex("ui/textures/icon_x");
        if(m>=s.resources->materials()->size())return fail(d,Error::material);
        const auto& b=(*s.resources->materials())[m];if(b.images.size()!=1)return fail(d,Error::material);
        const auto* atlas=s.resources->textures()->texture(b.images[0]);if(!atlas)return fail(d,Error::geometry);
        const float w=float(atlas->texture.width),h=float(atlas->texture.height);game::OriginalUiOverlayQuad50KG q;
        q.materialName="icon_x";q.authoredDepth=1;q.sourcePC=0x1D7A2C;
        q.vertices={{{484,378,0,0},{484+w,378,1,0},{484,378+h,0,1},{484+w,378+h,1,1}}};
        if(!append(q,m,16777214.f,Segment::prompts,0,d)){s.failed=true;return false;}}
    if(mask&2){const auto m=s.resources->materialIndex("ui/textures/icon_tri");
        if(m>=s.resources->materials()->size())return fail(d,Error::material);
        const auto& b=(*s.resources->materials())[m];if(b.images.size()!=1)return fail(d,Error::material);
        const auto* atlas=s.resources->textures()->texture(b.images[0]);if(!atlas)return fail(d,Error::geometry);
        const float w=float(atlas->texture.width),h=float(atlas->texture.height);game::OriginalUiOverlayQuad50KG q;
        q.materialName="icon_tri";q.authoredDepth=1;q.sourcePC=0x1D7A44;
        q.vertices={{{397,378,0,0},{397+w,378,1,0},{397,378+h,0,1},{397+w,378+h,1,1}}};
        if(!append(q,m,16777214.f,Segment::prompts,0,d)){s.failed=true;return false;}}
    s.prompts=true;return true;
}
bool OriginalUiHallDrawContribution50KG::projectOnTopWidgets(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||s.top||s.failed||s.finished||
        (s.pageShown&&(!s.prompts||s.apertureCount!=2)))return fail(d,Error::sourceOrder);
    if(!widgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}
bool OriginalUiHallDrawContribution50KG::drain(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||!s.prompts||!s.top||s.failed||s.drained||s.apertureCount!=2)return fail(d,Error::unfinished);
    const auto& materials=*s.resources->materials();
    if(!s.gpu||!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,Error::publication);
    if(s.historyPoisoned||s.submissionPending||s.w.size()!=s.vertices.size()||s.fog.size()!=s.vertices.size()||
        s.frameGeneration==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::sourceOrder);
    try {
    s.pendingHistory=s.history;
    s.order.clear();
    for(std::size_t i=0;i<s.packets.size();++i)
        if(s.packets[i].materialIndex==unresolved||materials[s.packets[i].materialIndex].material.passes[0].enabled!=0)s.order.push_back(static_cast<std::uint32_t>(i));
    std::sort(s.order.begin(),s.order.end(),[&](std::uint32_t a,std::uint32_t b){
        // Raw25CC60 dispatch precedes all deferred canonical ScreenObject
        // groups. It is not a fabricated material or registry token.
        const auto ga=s.packets[a].materialIndex==unresolved?0:s.leaf[s.packets[a].materialIndex]+1;
        const auto gb=s.packets[b].materialIndex==unresolved?0:s.leaf[s.packets[b].materialIndex]+1;
        return ga!=gb?ga<gb:s.packets[a].sourceOrdinal<s.packets[b].sourceOrdinal;
    });
    // Like the Rider source-frame owner, validate the complete large frame,
    // then convert each material in its local bounded span and rebase it.
    // The legacy material converter's 8192 limit is not a whole source-frame limit.
    if(s.vertices.size()%4||s.vertices.size()/4>originalUiMaximumSourceQuads50KM)
        return fail(d,Error::capacity);
    const auto sourceTotal=static_cast<std::uint32_t>(s.vertices.size()/4);
    std::uint32_t total=0;
    for(const auto i:s.order){const auto& p=s.packets[i];
        if(!p.quadCount||p.firstQuad>=sourceTotal||p.quadCount>sourceTotal-p.firstQuad||
            p.quadCount>originalUiMaximumSourceQuads50KM-total)return fail(d,Error::capacity);
        total+=p.quadCount;
    }
    if(!total)return fail(d,Error::unfinished);
    s.drainedVertices.clear();s.drainedFog.clear();s.drainedW.clear();s.spans.clear();s.states.clear();
    std::uint32_t terminalMaterial=unresolved;
    for(const auto i:s.order){
        const auto& p=s.packets[i];
        const auto first=static_cast<std::uint32_t>(s.drainedVertices.size()/4);
        OriginalUiMaterialSpan50KG converted;bool okay=false;
        if(p.materialIndex==unresolved){
            okay=p.segment==Segment::rawBackground&&first==0&&p.quadCount==1&&
                prepareOriginalUiHallBackgroundSpan50KG(*s.resources->images(),p.imageIndex,converted.span,converted.native);
        }else{
            const auto& material=materials[p.materialIndex];
            const OriginalUiMaterialSpanBinding50KG range{0,p.quadCount,p.quadCount,p.imageIndex,s.gpu->textureCount()};
            okay=material.images.empty()?convertOriginalUiUntexturedMaterialSpan50KG(material.untextured,{0,p.quadCount,p.quadCount},converted):
                (p.frameIndex<material.policies.size()&&convertOriginalUiMaterialSpan50KG(material.policies[p.frameIndex],range,converted));
            const auto representative=s.representatives[s.leaf[p.materialIndex]];
            const auto& source=materials[representative].material.passes[0];
            assets::OriginalUiMaterialBindingDiagnostic50KG detail;
            if(source.textureCount&&(!s.resources->sourceImageBank()||
                !s.pendingHistory[representative].recordDrainedBinding(*s.resources->sourceImageBank(),p.imageIndex,detail)))
                return fail(d,Error::material,detail.sourcePC);
            terminalMaterial=representative;
        }
        if(!okay){s.failed=true;return fail(d,Error::material);}
        converted.span.firstQuad=first;
        const auto begin=s.vertices.begin()+std::size_t(p.firstQuad)*4;
        s.drainedVertices.insert(s.drainedVertices.end(),begin,begin+std::size_t(p.quadCount)*4);
        const auto fogBegin=s.fog.begin()+std::size_t(p.firstQuad)*4;
        s.drainedFog.insert(s.drainedFog.end(),fogBegin,fogBegin+std::size_t(p.quadCount)*4);
        const auto wBegin=s.w.begin()+std::size_t(p.firstQuad)*4;
        s.drainedW.insert(s.drainedW.end(),wBegin,wBegin+std::size_t(p.quadCount)*4);
        s.spans.push_back(converted.span);s.states.push_back(converted.native);
    }
    s.draw={};
    s.draw.draw.textures=s.gpu.get();s.draw.draw.texturePublication=s.publication;
    s.draw.draw.vertices=s.drainedVertices.data();s.draw.draw.quadCount=total;
    s.draw.draw.spans=s.spans.data();s.draw.draw.spanCount=static_cast<std::uint32_t>(s.spans.size());
    s.draw.draw.canvasWidth=640;s.draw.draw.canvasHeight=448;
    // Actual authenticated Hall PageInfo black, underneath raw planet image.
    const auto* page=s.resources->widgets()->page();
    for(unsigned c=0;c<3;++c)s.draw.draw.clearRgba[c]=page->backgroundColor[c]/255.f;
    s.draw.draw.clearRgba[3]=1;s.draw.draw.retainRaceImage=false;
    s.draw.states=s.states.data();s.draw.stateCount=static_cast<std::uint32_t>(s.states.size());
    s.draw.particleFog=s.drainedFog.data();s.draw.particleFogCount=static_cast<std::uint32_t>(s.drainedFog.size());
    s.draw.sourceRasterW=s.drainedW.data();s.draw.sourceRasterWCount=static_cast<std::uint32_t>(s.drainedW.size());
    s.draw.opaquePresentationAlphaOnly=false;
    // A raw-background-only partial contribution cannot obtain this seal.
    // The terminal is the actual last nonempty canonical material leaf.
    if(terminalMaterial==unresolved)return fail(d,Error::unfinished);
    const auto& terminalBinding=materials[terminalMaterial];const auto& terminalPass=terminalBinding.material.passes[0];
    OriginalUiSourceTerminal50KN terminal{};terminal.canonicalToken=terminalBinding.registryToken;
    terminal.pass=terminalPass.pass;terminal.leaf=s.leaf[terminalMaterial];
    // This immutable Hall owner has one successful cold bank publication
    // and one source group construction. Neither is rebuilt on course changes;
    // the separate frameGeneration records each real accepted submission.
    terminal.registryGeneration=1;terminal.bankGeneration=1;
    if(!s.pendingHistory[terminalMaterial].copyEffectiveGs(terminal.gs))return fail(d,Error::material);
    for(std::size_t f=0;f<terminalPass.gs.fields.size();++f){terminal.constructionFieldSourcePC[f]=terminalPass.gs.fields[f].sourcePC;
        terminal.constructionFieldByteOffset[f]=terminalPass.gs.fields[f].byteOffset;}
    terminal.enabled=terminalPass.enabled;terminal.textureCount=terminalPass.textureCount;terminal.textureCoordinateIndex=terminalPass.textureCoordinateIndex;
    terminal.lodCalculationMethod=terminalPass.lodCalculationMethod;terminal.maximumMipLevel=terminalPass.maximumMipLevel;
    terminal.magFilter=terminalPass.magFilter;terminal.minFilter=terminalPass.minFilter;terminal.lodL=terminalPass.lodL;terminal.lodK=terminalPass.lodK;
    s.drained=true;s.finished=true;frame_.native_=&s.draw;frame_.owner_=this;frame_.frameGeneration_=++s.frameGeneration;frame_.terminal_=terminal;
    return true;
    }catch(...){s.failed=true;frame_.native_=nullptr;frame_.owner_=nullptr;return fail(d,Error::allocation);}
}
const OriginalUiNativeDraw50KG* OriginalUiHallDrawContribution50KG::draw() const noexcept {
    return storage_&&storage_->drained&&!storage_->failed?&storage_->draw:nullptr;
}
const OriginalUiSourceFrame50KM* OriginalUiHallDrawContribution50KG::sourceFrame() const noexcept {
    return storage_&&storage_->drained&&!storage_->failed&&!storage_->historyPoisoned&&!storage_->submissionPending&&frame_.native_?&frame_:nullptr;
}
bool OriginalUiHallDrawContribution50KG::sourceTeardownPending() const noexcept {return storage_&&storage_->submittedRenderer;}
bool OriginalUiHallDrawContribution50KG::retireSourceRenderer(VehicleWindowGl50DS& renderer,Diagnostic& d) noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.submissionPending||(s.submittedRenderer&&s.submittedRenderer!=&renderer))return fail(d,Error::sourceOrder);
    if(s.submittedRenderer){VehicleWindowDiagnostic50DS detail;
        if(!renderer.retireOriginalUiSourceOwner(*this,detail))return fail(d,Error::publication);}
    s.submittedRenderer=nullptr;return true;
}
bool OriginalUiHallDrawContribution50KG::beginSourceSubmission(std::uint64_t generation,const VehicleWindowGl50DS& renderer) const noexcept {
    if(!storage_||!sourceFrame()||generation!=frame_.frameGeneration_)return false;auto& s=*storage_;
    if(s.submissionPending||s.historyPoisoned||(s.submittedRenderer&&s.submittedRenderer!=&renderer)||
        generation<=s.attemptedGeneration||generation<=s.acknowledgedGeneration||s.pendingHistory.size()!=s.history.size())return false;
    s.submittedRenderer=&renderer;s.attemptedGeneration=generation;s.submissionPending=true;return true;
}
void OriginalUiHallDrawContribution50KG::finishSourceSubmission(std::uint64_t generation,bool accepted,bool started) const noexcept {
    if(!storage_)return;auto& s=*storage_;
    if(!s.submissionPending||generation!=frame_.frameGeneration_){s.historyPoisoned=true;return;}
    s.submissionPending=false;
    if(accepted){if(s.pendingHistory.size()!=s.history.size()){s.historyPoisoned=true;return;}
        s.history.swap(s.pendingHistory);s.acknowledgedGeneration=generation;}
    else if(started)s.historyPoisoned=true;
}
bool OriginalUiHallDrawContribution50KG::finishContribution(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||!s.top||s.failed||s.finished||
        (s.pageShown&&(!s.prompts||s.apertureCount!=2)))return fail(d,Error::unfinished);
    s.finished=true;return true;
}
const std::vector<assets::Vertex48B>* OriginalUiHallDrawContribution50KG::vertices() const noexcept {
    return storage_&&storage_->finished&&!storage_->failed?&storage_->vertices:nullptr;
}
const std::vector<OriginalUiHallPacket50KG>* OriginalUiHallDrawContribution50KG::packets() const noexcept {
    return storage_&&storage_->finished&&!storage_->failed?&storage_->packets:nullptr;
}
const OriginalUiHallDrawResources50KG* OriginalUiHallDrawContribution50KG::resources() const noexcept {return storage_?storage_->resources.get():nullptr;}
}
