#include "original_ui_win_draw_contribution50pd.h"
#include "original_ui_material_adapter50kg.h"
#include "original_ui_lit_material_adapter50km.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <utility>

namespace rr::platform {
namespace {
using Error=OriginalUiWinContributionError50PD;using Diagnostic=OriginalUiWinContributionDiagnostic50PD;
using Segment=OriginalUiWinSegment50PD;using Kind=OriginalUiWinMaterialKind50PD;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
// Failure receipts carry the source line to the log (diagnostic only).
bool failAt(int line,Diagnostic& d,Error e,std::uint32_t pc=0)noexcept{d.error=e;if(pc)d.sourcePC=pc;std::fprintf(stderr,"win_draw failure line=%d error=%u pc=%x\n",line,unsigned(e),pc);return false;}
#define fail(...) failAt(__LINE__,__VA_ARGS__)
struct Packet {std::uint32_t firstQuad=0,quadCount=0,material=0,image=0,frame=0,ordinal=0,pc=0;std::uint16_t widget=0;Segment segment=Segment::ordinaryWidgets;};
}
struct OriginalUiWinDrawContribution50PD::Storage {
    std::shared_ptr<OriginalUiWinDrawResources50PD> resources;std::shared_ptr<const MaterialTextureGl50EB> gpu;std::uint64_t publication=0;
    std::vector<game::OriginalUiTextureAnimation50KG> animation;std::vector<std::uint32_t> frames;
    std::vector<assets::Vertex48B> vertices,drainedVertices;std::vector<float> fog,w,drainedFog,drainedW;std::vector<Packet> packets;
    std::vector<assets::OriginalUiMaterialBindingState50KG> history,pendingHistory;std::vector<std::uint32_t> leaf,representatives,order;
    std::vector<OriginalUiSpan50KG> spans;std::vector<OriginalUiNativeSpan50KG> states;OriginalUiNativeDraw50KG draw{};
    std::unique_ptr<game::OriginalUiLitGeometryWorkspace50KM> workspace;
    std::vector<assets::Vertex48B> modelVertices;std::vector<float> modelW,modelFog;
    std::uint64_t frameGeneration=0,attemptedGeneration=0,acknowledgedGeneration=0;
    const VehicleWindowGl50DS* submittedRenderer=nullptr;bool submissionPending=false,historyPoisoned=false;
    OriginalUiWinClock50PD clock{};
    bool active=false,ordinary=false,page=false,pageShown=false,exitText=false,prompts=false,finished=false,failed=false,drained=false;
    std::uint8_t apertureCount=0;
    // 29C490 registry rebuild: (token, pass) order, 249BA0 equivalence redirects.
    bool prepareGroups(Diagnostic& d){
        const auto& bindings=*resources->materials();const auto count=bindings.size();
        std::vector<std::uint32_t> sequence;history.resize(count);leaf.resize(count);
        for(std::size_t i=0;i<count;++i){const auto& b=bindings[i];assets::OriginalUiMaterialEquivalenceKey50KG key;
            assets::OriginalUiMaterialEquivalenceDiagnostic50KG keyDetail;assets::OriginalUiMaterialBindingDiagnostic50KG detail;
            const auto* source=b.images.empty()?nullptr:resources->images()->sourceProvenance(b.images[0]);
            if(!b.registryToken||!assets::prepareOriginalUiMaterialEquivalence50KG(b.pass(),source,key,keyDetail)||!history[i].initialize(key,detail))
                return fail(d,Error::material,keyDetail.sourcePC?keyDetail.sourcePC:detail.sourcePC);
            sequence.push_back(static_cast<std::uint32_t>(i));}
        std::sort(sequence.begin(),sequence.end(),[&](std::uint32_t a,std::uint32_t b){
            if(bindings[a].drawRank!=bindings[b].drawRank)return bindings[a].drawRank<bindings[b].drawRank;
            return bindings[a].registryToken!=bindings[b].registryToken?bindings[a].registryToken<bindings[b].registryToken:bindings[a].pass().pass<bindings[b].pass().pass;});
        struct Group{std::uint32_t binding=0,redirect=unresolved,limit=unresolved;};std::vector<Group> groups;
        std::uint32_t previous=unresolved;
        for(const auto index:sequence){const auto current=static_cast<std::uint32_t>(groups.size());
            for(auto& prior:groups){const auto equal=history[index].compare(history[prior.binding]);
                if(equal==assets::OriginalUiMaterialEquivalence50KG::unsupported)return fail(d,Error::material,0x249BA0);
                if(equal==assets::OriginalUiMaterialEquivalence50KG::equivalent&&current<prior.limit)prior.redirect=current;}
            groups.push_back({index,unresolved,unresolved});
            if(previous!=unresolved&&bindings[previous].registryToken==bindings[index].registryToken){
                if(bindings[index].pass().pass!=bindings[previous].pass().pass+1)return fail(d,Error::material,0x29BED0);
                groups[current-1].limit=current;
            }else if(bindings[index].pass().pass!=1)return fail(d,Error::material,0x29BED0);
            leaf[index]=current;previous=index;}
        for(auto& value:leaf){std::size_t steps=0;while(groups[value].redirect!=unresolved){
            const auto next=groups[value].redirect;if(next<=value||++steps>count)return fail(d,Error::material,0x29C150);value=next;}}
        for(const auto& g:groups)representatives.push_back(g.binding);
        pendingHistory=history;return true;
    }
    bool frameFor(std::uint32_t material,std::uint32_t& frame,Diagnostic& d){
        const auto& b=(*resources->materials())[material];
        if(b.animated&&clock.advanceAnimations&&!game::advanceOriginalUiTextureAnimation50KG(animation[material],clock.milliseconds,frames[material]))return fail(d,Error::clock,0x29AC9C);
        frame=b.animated?frames[material]:0;if(!b.images.empty()&&frame>=b.images.size())return fail(d,Error::clock);return true;
    }
    bool record(const assets::Vertex48B* v,const float* ws,const float* fs,std::uint32_t quads,std::uint32_t material,std::uint32_t frame,
        std::uint32_t pc,Segment segment,std::uint16_t widget,Diagnostic& d){
        const auto& b=(*resources->materials())[material];
        if(!quads)return true;
        if(vertices.size()/4+quads>originalUiMaximumSourceQuads50KM||packets.size()>=originalUiMaximumSpans50KG)return fail(d,Error::capacity);
        for(std::uint32_t i=0;i<quads*4;++i){for(float x:v[i].position)if(!std::isfinite(x))return fail(d,Error::geometry,pc);
            for(float x:v[i].uv)if(!std::isfinite(x))return fail(d,Error::geometry,pc);
            if(v[i].position[2]<0||v[i].position[2]>16777215.f||std::floor(v[i].position[2])!=v[i].position[2]||v[i].color[3]>128)return fail(d,Error::geometry,pc);
            if(ws&&(!std::isfinite(ws[i])||ws[i]<=0))return fail(d,Error::geometry,pc);
            if(fs&&(!std::isfinite(fs[i])||fs[i]<0||fs[i]>255.f/256.f))return fail(d,Error::geometry,pc);}
        const auto first=static_cast<std::uint32_t>(vertices.size()/4),ordinal=static_cast<std::uint32_t>(packets.size());
        vertices.insert(vertices.end(),v,v+quads*4);
        for(std::uint32_t i=0;i<quads*4;++i){w.push_back(ws?ws[i]:1.f);fog.push_back(fs?fs[i]:0.f);}
        packets.push_back({first,quads,material,b.images.empty()?unresolved:b.images[frame],frame,ordinal,pc,widget,segment});return true;
    }
};
OriginalUiWinDrawContribution50PD::OriginalUiWinDrawContribution50PD() noexcept=default;
OriginalUiWinDrawContribution50PD::~OriginalUiWinDrawContribution50PD()=default;
bool OriginalUiWinDrawContribution50PD::bindFrame(std::shared_ptr<OriginalUiWinDrawResources50PD> resources,
    std::shared_ptr<const MaterialTextureGl50EB> gpu,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyBound);
    if(!resources||!resources->loaded()||!gpu||!gpu->initialized()||!gpu->publicationToken()||!resources->textures()||
        gpu->textureCount()!=resources->textures()->textureCount())return fail(d,Error::publication);
    for(std::uint32_t i=0;i<gpu->textureCount();++i){const auto* a=gpu->asset(i);const auto* b=resources->textures()->texture(i);
        if(!a||!b||a->identity!=b->identity)return fail(d,Error::publication);}
    try{
        auto s=std::make_unique<Storage>();s->resources=std::move(resources);
        const auto& materials=*s->resources->materials();s->animation.resize(materials.size());s->frames.resize(materials.size());
        for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&!game::initializeOriginalUiTextureAnimation50KG(s->animation[i],
            materials[i].animation,static_cast<std::uint32_t>(materials[i].images.size())))return fail(d,Error::clock,0x29B950);
        s->workspace=std::make_unique<game::OriginalUiLitGeometryWorkspace50KM>();
        s->modelVertices.resize(game::originalUiLitMaximumTriangles50KM*4u);s->modelW.resize(s->modelVertices.size());s->modelFog.resize(s->modelVertices.size());
        s->vertices.reserve(originalUiMaximumSourceQuads50KM*4u);s->drainedVertices.reserve(originalUiMaximumSourceQuads50KM*4u);
        s->w.reserve(originalUiMaximumSourceQuads50KM*4u);s->fog.reserve(originalUiMaximumSourceQuads50KM*4u);
        s->drainedW.reserve(originalUiMaximumSourceQuads50KM*4u);s->drainedFog.reserve(originalUiMaximumSourceQuads50KM*4u);
        s->packets.reserve(originalUiMaximumSpans50KG);s->spans.reserve(originalUiMaximumSpans50KG);s->states.reserve(originalUiMaximumSpans50KG);
        if(!s->prepareGroups(d))return false;
        s->publication=gpu->publicationToken();s->gpu=std::move(gpu);storage_=std::move(s);return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiWinDrawContribution50PD::begin(const OriginalUiWinClock50PD& clock,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);if(!clock.known)return fail(d,Error::clock);auto& s=*storage_;
    if(!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,Error::publication);
    if(s.submissionPending||s.historyPoisoned)return fail(d,Error::sourceOrder);
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;frame_.terminal_={};
    s.vertices.clear();s.w.clear();s.fog.clear();s.packets.clear();s.drainedVertices.clear();s.drainedW.clear();s.drainedFog.clear();
    s.order.clear();s.spans.clear();s.states.clear();s.clock=clock;s.active=true;
    s.ordinary=s.page=s.pageShown=s.exitText=s.prompts=s.finished=s.failed=s.drained=false;s.apertureCount=0;
    const auto& materials=*s.resources->materials();
    for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
        !game::selectOriginalUiTextureAnimationFrame50KG(s.animation[i],s.frames[i])){s.failed=true;return fail(d,Error::clock,0x29AF90);}
    return true;
}
bool OriginalUiWinDrawContribution50PD::append(const game::OriginalUiOverlayQuad50KG& quad,std::uint32_t material,float z,
    Segment segment,std::uint16_t widget,Diagnostic& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(material>=materials.size()||(materials[material].kind!=Kind::screen&&materials[material].kind!=Kind::untextured)||
       !std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z||quad.sourcePacketColor[3]>128)return fail(d,Error::geometry,quad.sourcePC);
    std::uint32_t frame=0;if(!s.frameFor(material,frame,d))return false;
    constexpr unsigned order[]={0,1,3,2};std::array<assets::Vertex48B,4> v{};
    for(unsigned i=0;i<4;++i){const auto& q=quad.vertices[order[i]];auto& o=v[i];
        o.position[0]=q.x;o.position[1]=q.y;o.position[2]=z;o.uv[0]=q.u;o.uv[1]=q.v;for(unsigned c=0;c<4;++c)o.color[c]=quad.sourcePacketColor[c];}
    return s.record(v.data(),nullptr,nullptr,1,material,frame,quad.sourcePC,segment,widget,d);
}
bool OriginalUiWinDrawContribution50PD::appendText(const game::OriginalUiTextLayout50KG& text,const std::array<std::uint8_t,4>& color,
    std::uint32_t material,std::uint32_t pc,Segment segment,std::uint16_t widget,Diagnostic& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(material>=materials.size()||color[3]>128||text.count>text.quads.size())return fail(d,Error::geometry,pc);
    const auto& b=materials[material];if(b.kind!=Kind::screen||b.images.size()!=1||b.animated)return fail(d,Error::material,pc);
    if(!text.count)return true;
    try{std::vector<assets::Vertex48B> v;v.reserve(std::size_t(text.count)*4u);
        for(std::size_t q=0;q<text.count;++q)for(const auto& g:text.quads[q].vertices){assets::Vertex48B o{};
            o.position[0]=g.x;o.position[1]=g.y;o.position[2]=16777214.f;o.uv[0]=g.u;o.uv[1]=g.v;for(unsigned c=0;c<4;++c)o.color[c]=color[c];v.push_back(o);}
        return s.record(v.data(),nullptr,nullptr,text.count,material,0,pc,segment,widget,d);
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiWinDrawContribution50PD::model(std::uint16_t id,Diagnostic& d) noexcept {
    auto& s=*storage_;auto* model=s.resources->model(id);const auto* surfaces=s.resources->modelSurfaces(id);const auto* lit=s.resources->litContext();
    if(!model||!surfaces||!lit||!model->attached())return fail(d,Error::model,0x1D3590);
    game::ModelPalette49S palette{};
    if(!model->projectPalette(palette,d.scene)||!model->radialFogClear(*s.resources->renderContext(),d.scene))return fail(d,Error::model,d.scene.sourcePC);
    const auto* scene=model->scene();if(!scene||scene->lods.size()!=1||scene->lods[0].surfaces.size()!=surfaces->size())return fail(d,Error::model);
    const auto& materials=*s.resources->materials();
    for(std::uint32_t surface=0;surface<surfaces->size();++surface){
        const auto triangles=static_cast<std::uint32_t>(scene->lods[0].surfaces[surface].indices.size()/3);
        if(!triangles||triangles>game::originalUiLitMaximumTriangles50KM)return fail(d,Error::model,0x240C00);
        for(const auto material:{(*surfaces)[surface].first,(*surfaces)[surface].second}){
            if(material==unresolved)continue;if(material>=materials.size())return fail(d,Error::material);
            const bool specular=materials[material].kind==Kind::specular;
            if(!specular&&materials[material].kind!=Kind::lit)return fail(d,Error::material);
            game::OriginalUiLitSurfaceOutput50KM out{s.modelVertices.data(),s.modelW.data(),s.modelFog.data(),triangles,0,0};
            if(!model->project(surface,specular,palette,*lit,out,*s.workspace,d.scene)||out.slotCount!=triangles)return fail(d,Error::model,d.scene.sourcePC);
            std::uint32_t frame=0;if(!s.frameFor(material,frame,d))return false;
            if(!s.record(s.modelVertices.data(),s.modelW.data(),s.modelFog.data(),out.slotCount,material,frame,0x240C00,Segment::model,id,d))return false;
        }
    }
    return true;
}
bool OriginalUiWinDrawContribution50PD::projectOrdinaryWidgets(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||s.ordinary||s.page||s.finished||s.failed)return fail(d,Error::sourceOrder);
    auto* page=s.resources->page();const auto* order=page->order();
    const auto ok=[&]()->bool{
        for(auto id:*order){d.widgetId=id;const auto state=page->state(id);
            if(state.kind==game::OriginalUiWinWidgetKind50PD::rotate){
                // 1D38B0/1D33C0: a User_Rotate draws only while loaded and shown.
                if(shown&&state.shown&&state.loaded&&!model(id,d))return false;continue;}
            const auto material=s.resources->widgetMaterial(id);
            if(const auto* desc=page->textDescriptor(id)){
                if(desc->textSource==assets::OriginalUiTextSource50KG::dynamic&&!s.resources->prepareDynamicText(id,d.resource))return fail(d,Error::resource,d.resource.sourcePC);
                const auto* geometry=s.resources->textGeometry(id);if(!geometry||desc->depth!=1)return fail(d,Error::geometry);
                for(std::size_t line=0;line<geometry->lines().size();++line){const auto& l=geometry->lines()[line];
                    game::OriginalUiTextPosition50KG position;game::OriginalUiTextLineProjection50KG live;
                    if(!page->projectText(id,line,l.glyphs.fontLineHeight,l.measuredWidth,l.measuredHeight,shown,position,live,d.page)){
                        if(d.page.error==game::OriginalUiWinPageError50PD::hidden)continue;return fail(d,Error::projection,d.page.sourcePC);}
                    if(live.scale!=1||live.rotation!=0||live.packetColor[3]>128||position.positionedX!=l.position.positionedX||
                       position.positionedY!=l.position.positionedY)return fail(d,Error::projection,0x2E0A30);
                    if(!appendText(l.glyphs,live.packetColor,material,0x2E0A30,Segment::ordinaryWidgets,id,d))return false;}
            }else if(page->iconDescriptor(id)){
                if(!shown||!state.shown)continue;const auto* layout=page->iconLayout(id);if(!layout)return fail(d,Error::projection,0x2AC590);
                if(!append(layout->quad,material,layout->packetDepth,Segment::ordinaryWidgets,id,d))return false;}
        }
        return true;}();
    if(!ok){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiWinDrawContribution50PD::projectPage(std::uint32_t career,bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.page||s.finished||s.failed)return fail(d,Error::sourceOrder,0x208AA0);
    s.page=true;s.pageShown=shown;if(!shown)return true;
    const auto* bg=s.resources->background(career);if(!bg){s.failed=true;return fail(d,Error::resource,0x2077F0);}
    // 208AC8..208AF0: 2DF550 on page+4D0/+530/+590/+5F0, depth15 packets.
    for(const auto& tile:bg->tiles)if(!append(tile,bg->material,16777200.f,Segment::page,0,d)){s.failed=true;return false;}
    return true;
}
bool OriginalUiWinDrawContribution50PD::projectExitText(const game::OriginalUiApertureHook50KG& hook,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertureCount||s.prompts||s.exitText||s.failed||s.finished)return fail(d,Error::sourceOrder,0x1D75A4);
    const auto m=s.resources->materialIndex("font/denmrk16top");const auto& materials=*s.resources->materials();
    if(m>=materials.size()||materials[m].images.size()!=1)return fail(d,Error::material);
    game::OriginalUiExitTextProjection50KG live;auto* exit=s.resources->exitText();
    if(!exit||exit->projectRender2E0170(hook,live)!=game::OriginalUiExitTextStatus50KG::ok||!live.glyphs||live.images!=s.resources->images().get()||
       live.imageIndex!=materials[m].images[0]||live.materialIdentity!=materials[m].material.identity||live.assignedDepth!=1||live.glyphs->count>10)
        {s.failed=true;return fail(d,Error::projection,0x2E0170);}
    if(!appendText(*live.glyphs,live.packetColor,m,live.submitPC,Segment::exitText,0,d)){s.failed=true;return false;}
    s.exitText=true;return true;
}
bool OriginalUiWinDrawContribution50PD::projectAperture(std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& rect,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.prompts||s.failed||s.finished||ordinal!=s.apertureCount||ordinal>1||
       rect.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rect.dirty3C)return fail(d,Error::sourceOrder,0x1D798C);
    game::OriginalUiOverlayQuad50KG q;q.materialName="NoTexture";q.authoredDepth=1;q.sourcePC=0x1D798C;q.sourcePacketColor={{0,0,0,63}};
    constexpr unsigned order[]={0,2,1,3};for(unsigned i=0;i<4;++i){const auto& v=rect.vertices[order[i]];q.vertices[i]={v.x,v.y,0,0};}
    if(!append(q,s.resources->materialIndex("ui/textures/notexture"),16777214.f,Segment::aperture,0,d)){s.failed=true;return false;}
    ++s.apertureCount;return true;
}
bool OriginalUiWinDrawContribution50PD::projectPrompts(std::uint32_t mask,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertureCount!=2||s.prompts||s.failed||s.finished)return fail(d,Error::sourceOrder);
    if(mask>1)return fail(d,Error::input,0x1D7A10);
    const auto* prompts=s.resources->prompts();
    if(mask&1u){const auto& p=(*prompts)[0];if(!append(p.quad,p.material,16777214.f,Segment::prompts,0,d)){s.failed=true;return false;}}
    s.prompts=true;return true;
}
bool OriginalUiWinDrawContribution50PD::drain(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||s.failed||s.drained||(s.pageShown&&(!s.prompts||s.apertureCount!=2)))return fail(d,Error::unfinished);
    const auto& materials=*s.resources->materials();
    if(!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,Error::publication);
    if(s.historyPoisoned||s.submissionPending||s.w.size()!=s.vertices.size()||s.fog.size()!=s.vertices.size()||
       s.frameGeneration==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::sourceOrder);
    try{
        s.pendingHistory=s.history;s.order.clear();
        for(std::size_t i=0;i<s.packets.size();++i)if(materials[s.packets[i].material].pass().enabled!=0)s.order.push_back(static_cast<std::uint32_t>(i));
        std::sort(s.order.begin(),s.order.end(),[&](std::uint32_t a,std::uint32_t b){
            const auto ga=s.leaf[s.packets[a].material],gb=s.leaf[s.packets[b].material];
            return ga!=gb?ga<gb:s.packets[a].ordinal<s.packets[b].ordinal;});
        if(s.vertices.size()%4||s.vertices.size()/4>originalUiMaximumSourceQuads50KM)return fail(d,Error::capacity);
        const auto sourceTotal=static_cast<std::uint32_t>(s.vertices.size()/4);std::uint32_t total=0;
        for(const auto i:s.order){const auto& p=s.packets[i];
            if(!p.quadCount||p.firstQuad>=sourceTotal||p.quadCount>sourceTotal-p.firstQuad||p.quadCount>originalUiMaximumSourceQuads50KM-total)return fail(d,Error::capacity);
            total+=p.quadCount;}
        if(!total)return fail(d,Error::unfinished);
        const auto& images=*s.resources->images();const auto textureCount=s.gpu->textureCount();
        std::uint32_t terminalMaterial=unresolved;
        for(const auto i:s.order){const auto& p=s.packets[i];const auto& b=materials[p.material];
            const auto first=static_cast<std::uint32_t>(s.drainedVertices.size()/4);
            OriginalUiMaterialSpan50KG converted;bool okay=false;
            const OriginalUiMaterialSpanBinding50KG range{0,p.quadCount,p.quadCount,p.image,textureCount};
            switch(b.kind){
            case Kind::untextured:okay=convertOriginalUiUntexturedMaterialSpan50KG(b.untextured,{0,p.quadCount,p.quadCount},converted);break;
            case Kind::screen:okay=p.frame<b.screenPolicies.size()&&convertOriginalUiMaterialSpan50KG(b.screenPolicies[p.frame],range,converted);break;
            case Kind::lit:okay=p.frame<b.litPolicies.size()&&convertOriginalUiLitMaterialSpan50KM(b.litPolicies[p.frame],range,converted);break;
            case Kind::specular:okay=convertOriginalUiWinTrophySpecularSpan50PD(b.specular,images,range,converted);break;}
            if(!okay){s.failed=true;return fail(d,Error::material);}
            const auto representative=s.representatives[s.leaf[p.material]];
            assets::OriginalUiMaterialBindingDiagnostic50KG detail;
            if(materials[representative].pass().textureCount&&(!s.resources->sourceImageBank()||
               !s.pendingHistory[representative].recordDrainedBinding(*s.resources->sourceImageBank(),p.image,detail)))return fail(d,Error::material,detail.sourcePC);
            terminalMaterial=representative;
            converted.span.firstQuad=first;
            const auto at=std::size_t(p.firstQuad)*4,n=std::size_t(p.quadCount)*4;
            s.drainedVertices.insert(s.drainedVertices.end(),s.vertices.begin()+at,s.vertices.begin()+at+n);
            s.drainedFog.insert(s.drainedFog.end(),s.fog.begin()+at,s.fog.begin()+at+n);
            s.drainedW.insert(s.drainedW.end(),s.w.begin()+at,s.w.begin()+at+n);
            s.spans.push_back(converted.span);s.states.push_back(converted.native);}
        s.draw={};auto& draw=s.draw.draw;
        draw.textures=s.gpu.get();draw.texturePublication=s.publication;draw.vertices=s.drainedVertices.data();draw.quadCount=total;
        draw.spans=s.spans.data();draw.spanCount=static_cast<std::uint32_t>(s.spans.size());draw.canvasWidth=640;draw.canvasHeight=448;
        // win.dat PageInfo BackgroundColor=0,0,0 (2087E0 publishes page+430).
        const auto* page=s.resources->page()->page();
        for(unsigned c=0;c<3;++c)draw.clearRgba[c]=page->backgroundColor[c]/255.f;draw.clearRgba[3]=1;draw.retainRaceImage=false;
        s.draw.states=s.states.data();s.draw.stateCount=static_cast<std::uint32_t>(s.states.size());
        s.draw.particleFog=s.drainedFog.data();s.draw.particleFogCount=static_cast<std::uint32_t>(s.drainedFog.size());
        s.draw.sourceRasterW=s.drainedW.data();s.draw.sourceRasterWCount=static_cast<std::uint32_t>(s.drainedW.size());
        const auto* render=s.resources->renderContext();for(unsigned c=0;c<3;++c)s.draw.particleFogColor[c]=render->sourceFogRgb[c]/255.f;
        s.draw.opaquePresentationAlphaOnly=false;
        if(terminalMaterial==unresolved)return fail(d,Error::unfinished);
        const auto& tb=materials[terminalMaterial];const auto& tp=tb.pass();
        OriginalUiSourceTerminal50KN terminal{};terminal.canonicalToken=tb.registryToken;terminal.pass=tp.pass;terminal.leaf=s.leaf[terminalMaterial];
        terminal.registryGeneration=1;terminal.bankGeneration=1;
        if(!s.pendingHistory[terminalMaterial].copyEffectiveGs(terminal.gs))return fail(d,Error::material);
        for(std::size_t f=0;f<tp.gs.fields.size();++f){terminal.constructionFieldSourcePC[f]=tp.gs.fields[f].sourcePC;terminal.constructionFieldByteOffset[f]=tp.gs.fields[f].byteOffset;}
        terminal.enabled=tp.enabled;terminal.textureCount=tp.textureCount;terminal.textureCoordinateIndex=tp.textureCoordinateIndex;
        terminal.lodCalculationMethod=tp.lodCalculationMethod;terminal.maximumMipLevel=tp.maximumMipLevel;terminal.magFilter=tp.magFilter;
        terminal.minFilter=tp.minFilter;terminal.lodL=tp.lodL;terminal.lodK=tp.lodK;
        s.drained=true;s.finished=true;frame_.native_=&s.draw;frame_.owner_=this;frame_.frameGeneration_=++s.frameGeneration;frame_.terminal_=terminal;
        return true;
    }catch(...){s.failed=true;frame_.native_=nullptr;frame_.owner_=nullptr;return fail(d,Error::allocation);}
}
const OriginalUiNativeDraw50KG* OriginalUiWinDrawContribution50PD::draw() const noexcept{return storage_&&storage_->drained&&!storage_->failed?&storage_->draw:nullptr;}
const OriginalUiSourceFrame50KM* OriginalUiWinDrawContribution50PD::sourceFrame() const noexcept{
    return storage_&&storage_->drained&&!storage_->failed&&!storage_->historyPoisoned&&!storage_->submissionPending&&frame_.native_?&frame_:nullptr;}
bool OriginalUiWinDrawContribution50PD::sourceTeardownPending() const noexcept{return storage_&&storage_->submittedRenderer;}
bool OriginalUiWinDrawContribution50PD::retireSourceRenderer(VehicleWindowGl50DS& renderer,Diagnostic& d) noexcept{
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.submissionPending||(s.submittedRenderer&&s.submittedRenderer!=&renderer))return fail(d,Error::sourceOrder);
    if(s.submittedRenderer){VehicleWindowDiagnostic50DS detail;if(!renderer.retireOriginalUiSourceOwner(*this,detail))return fail(d,Error::publication);}
    s.submittedRenderer=nullptr;return true;
}
bool OriginalUiWinDrawContribution50PD::beginSourceSubmission(std::uint64_t generation,const VehicleWindowGl50DS& renderer) const noexcept{
    if(!storage_||!sourceFrame()||generation!=frame_.frameGeneration_)return false;auto& s=*storage_;
    if(s.submissionPending||s.historyPoisoned||(s.submittedRenderer&&s.submittedRenderer!=&renderer)||
       generation<=s.attemptedGeneration||generation<=s.acknowledgedGeneration||s.pendingHistory.size()!=s.history.size())return false;
    s.submittedRenderer=&renderer;s.attemptedGeneration=generation;s.submissionPending=true;return true;
}
void OriginalUiWinDrawContribution50PD::finishSourceSubmission(std::uint64_t generation,bool accepted,bool started) const noexcept{
    if(!storage_)return;auto& s=*storage_;
    if(!s.submissionPending||generation!=frame_.frameGeneration_){s.historyPoisoned=true;return;}
    s.submissionPending=false;
    if(accepted){if(s.pendingHistory.size()!=s.history.size()){s.historyPoisoned=true;return;}s.history.swap(s.pendingHistory);s.acknowledgedGeneration=generation;}
    else if(started)s.historyPoisoned=true;
}
}
