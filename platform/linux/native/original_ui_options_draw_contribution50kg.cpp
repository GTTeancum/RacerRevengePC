#include "original_ui_options_draw_contribution50kg.h"
#include "original_ui_options_particle_adapter50kg.h"
#include "../../../src/game/original_ui_controller_popup_geometry50kg.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include <limits>
#include <algorithm>
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
    std::shared_ptr<OriginalUiControllerPopupResources50KG> popup;
    std::array<std::uint32_t,5> popupIndices{};
    std::array<assets::OriginalUiMaterialBindingState50KG,5> popupPendingHistory;
    std::uint64_t popupRevision=0;
    bool popupLeased=false;
    bool popupVisited=false,memoryCardVisited=false,directVideoPresent=false;
    OriginalUiDirectVideo50KG directVideo{};
    std::vector<game::OriginalUiTextureAnimation50KG> animation;
    std::vector<std::uint32_t> frames;
    std::vector<assets::Vertex48B> vertices;
    game::OriginalUiLitGeometryWorkspace50KM modelWorkspace;
    std::vector<assets::Vertex48B> modelVertices;
    std::vector<float> modelW,modelFog;
    std::vector<OriginalUiOptionsPacket50KG> packets;
    std::vector<float> fog,drainedFog,w,drainedW;
    std::vector<assets::OriginalUiMaterialBindingState50KG> history,pendingHistory;
    std::vector<std::uint32_t> leaf,representatives;
    std::uint64_t frameGeneration=0,attemptedGeneration=0,acknowledgedGeneration=0;
    const VehicleWindowGl50DS* submittedRenderer=nullptr;
    bool submissionPending=false,historyPoisoned=false;
    bool prepareGroups(Diagnostic& d){
        const auto& bindings=*resources->materials();const auto count=bindings.size();
        popup=resources->controllerPopup();if(!popup)return fail(d,Error::resource);popupIndices.fill(unresolved);
        history.clear();pendingHistory.clear();leaf.clear();representatives.clear();
        std::vector<std::uint32_t> sequence;sequence.reserve(count);history.resize(count);leaf.resize(count);
        for(std::size_t i=0;i<count;++i){const auto& b=bindings[i];
            if(b.material.passes.size()!=1)return fail(d,Error::material);
            const auto& pass=b.material.passes[0];assets::OriginalUiMaterialEquivalenceKey50KG key;
            assets::OriginalUiMaterialEquivalenceDiagnostic50KG keyDetail;assets::OriginalUiMaterialBindingDiagnostic50KG detail;
            const auto* source=b.images.empty()?nullptr:resources->images()->sourceProvenance(b.images[0]);
            if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,source,key,keyDetail)||!history[i].initialize(key,detail))
                return fail(d,Error::material,keyDetail.sourcePC?keyDetail.sourcePC:detail.sourcePC);
            for(std::size_t role=0;role<popupIndices.size();++role)if(b.registryToken==OriginalUiControllerPopupResources50KG::registryTokens[role]){
                if(popupIndices[role]!=unresolved||!popup->copyHistory(b.registryToken,key,history[i],d.resource.controllerPopup))
                    return fail(d,Error::material,d.resource.controllerPopup.sourcePC);
                popupIndices[role]=static_cast<std::uint32_t>(i);
            }
            sequence.push_back(static_cast<std::uint32_t>(i));
        }
        for(auto index:popupIndices)if(index==unresolved)return fail(d,Error::material);
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
    std::array<float,3> fogColor{};
    bool scene=false,drained=false,exitText=false;
    OriginalUiOptionsClock50KG clock;
    std::uint8_t sourcePage=1;
    bool active=false,ordinary=false,page=false,pageShown=false,prompts=false,top=false,finished=false,failed=false;
    std::uint8_t apertureCount=0,controlsBackgroundCount=0;
};
OriginalUiOptionsDrawContribution50KG::OriginalUiOptionsDrawContribution50KG() noexcept=default;
OriginalUiOptionsDrawContribution50KG::~OriginalUiOptionsDrawContribution50KG(){
    if(storage_&&storage_->popup)storage_->popup->release(this,!storage_->submissionPending&&!storage_->submittedRenderer);
}
bool OriginalUiOptionsDrawContribution50KG::bind(std::shared_ptr<OriginalUiOptionsDrawResources50KG> resources,Diagnostic& d) noexcept {
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
        s->modelVertices.resize(game::originalUiLitMaximumTriangles50KM*4u);
        s->modelW.resize(game::originalUiLitMaximumTriangles50KM*4u);s->modelFog.resize(game::originalUiLitMaximumTriangles50KM*4u);
        s->popup=s->resources->controllerPopup();if(!s->popup)return fail(d,Error::resource);
        storage_=std::move(s);return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiOptionsDrawContribution50KG::bindFrame(std::shared_ptr<OriginalUiOptionsDrawResources50KG> resources,
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
bool OriginalUiOptionsDrawContribution50KG::begin(const OriginalUiOptionsClock50KG& clock,Diagnostic& d) noexcept {
    return beginPage(1,clock,d);
}
bool OriginalUiOptionsDrawContribution50KG::beginPage(std::uint8_t sourcePage,const OriginalUiOptionsClock50KG& clock,Diagnostic& d) noexcept {
    if(sourcePage<1||sourcePage>6){d={};return fail(d,Error::input);}
    d={};if(!storage_)return fail(d,Error::notBound);if(!clock.known)return fail(d,Error::clock);
    auto& s=*storage_;if(s.gpu&&(!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication))return fail(d,Error::publication);
    if(s.submissionPending||s.historyPoisoned)return fail(d,Error::sourceOrder);
    if(!s.popupLeased){try{if(!s.prepareGroups(d)||!s.popup->acquire(this,s.popupRevision))return fail(d,Error::sourceOrder);
        s.popupLeased=true;}catch(...){return fail(d,Error::allocation);}}
    if(!s.popup->current(this,s.popupRevision))return fail(d,Error::sourceOrder);
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;frame_.terminal_={};
    frame_.directVideoPresent_=false;frame_.directVideo_={};frame_.directVideoCapability_={};
    s.w.clear();s.drainedW.clear();s.sourcePage=sourcePage;
    s.popupVisited=false;s.memoryCardVisited=false;s.directVideoPresent=false;s.directVideo={};s.scene=s.drained=s.exitText=false;s.fog.clear();s.drainedFog.clear();s.drainedVertices.clear();s.order.clear();s.spans.clear();s.states.clear();s.clock=clock;s.active=true;s.ordinary=s.page=s.pageShown=s.prompts=s.top=s.finished=s.failed=false;
    s.apertureCount=s.controlsBackgroundCount=0;s.vertices.clear();s.packets.clear();
    const auto& materials=*s.resources->materials();
    for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
        !game::selectOriginalUiTextureAnimationFrame50KG(s.animation[i],s.frames[i])) {s.failed=true;return fail(d,Error::clock);}
    return true;
}
bool OriginalUiOptionsDrawContribution50KG::append(const game::OriginalUiOverlayQuad50KG& quad,
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
bool OriginalUiOptionsDrawContribution50KG::widgets(bool top,bool shown,Diagnostic& d) noexcept {
    auto& s=*storage_;auto* bank=s.resources->widgets();const auto* order=bank->order();if(!order)return fail(d,Error::projection);
    const auto& ids=top?order->onTopWidgets:order->ordinaryWidgets;
    const auto segment=top?Segment::onTopWidgets:Segment::ordinaryWidgets;
    for(auto id:ids) {
        d.widgetId=id;const auto material=s.resources->widgetMaterial(id);
        if(bank->widgetState(id).kind==game::OriginalUiOptionsWidgetKind50KG::model){
            if(id!=36||top||!controlsModel(shown,d))return fail(d,Error::geometry,d.sourcePC);
        } else if(const auto* text=s.resources->textGeometry(id)) {
            if(material>=s.resources->materials()->size())return fail(d,Error::material);
            const auto& binding=(*s.resources->materials())[material];const auto* descriptor=bank->textDescriptor(id);
            if(!descriptor||descriptor->depth!=1||binding.images.size()!=1)return fail(d,Error::geometry);
            for(std::size_t lineIndex=0;lineIndex<text->lines().size();++lineIndex) {
                const auto* liveGeometry=s.resources->textGeometryLine(id,lineIndex);if(!liveGeometry)return fail(d,Error::geometry);
                const auto& line=*liveGeometry;game::OriginalUiOptionsTextProjection50KG live;
                const auto status=bank->projectText(id,lineIndex,line.glyphs.fontLineHeight,line.measuredWidth,line.measuredHeight,shown,live,d.widgets);
                if(status==WidgetStatus::hidden)continue;if(status!=WidgetStatus::ok)return fail(d,Error::projection);
                if(live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128||
                    live.position.positionedX!=line.position.positionedX||live.position.positionedY!=line.position.positionedY||
                    live.line.text!=(descriptor->textSource==assets::OriginalUiTextSource50KG::dynamic?bank->dynamicText(id,lineIndex):&descriptor->lines[lineIndex]))return fail(d,Error::projection);
                if(!line.glyphs.count)continue;
                if(s.vertices.size()/4+line.glyphs.count>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)
                    return fail(d,Error::capacity);
                const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
                for(std::size_t q=0;q<line.glyphs.count;++q)for(const auto& glyph:line.glyphs.quads[q].vertices) {
                    assets::Vertex48B v{};v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=16777214.f;
                    v.uv[0]=glyph.u;v.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)v.color[c]=live.line.packetColor[c];s.vertices.push_back(v);s.fog.push_back(0.f);s.w.push_back(1.f);
                }
                s.packets.push_back({first,line.glyphs.count,material,binding.images[0],0,ordinal,0x2E0A30,id,segment});
            }
        } else if(bank->iconDescriptor(id)) {
            game::OriginalUiOptionsIconProjection50KG live;const auto status=bank->projectIcon(id,shown,live,d.widgets);
            if(status==WidgetStatus::hidden)continue;if(status!=WidgetStatus::ok)return fail(d,Error::projection);
            if(material>=s.resources->materials()->size())return fail(d,Error::material);
            const auto& binding=(*s.resources->materials())[material];
            if(binding.images.empty()||live.images!=s.resources->images()||live.imageIndex!=binding.images[0])return fail(d,Error::projection);
            if(!append(live.layout.quad,material,live.layout.packetDepth,segment,id,d))return false;
        }
        // Original Movement_Control5 has no geometry.
    }
    return true;
}
bool OriginalUiOptionsDrawContribution50KG::controlsModel(bool ancestorsShown,Diagnostic& d) noexcept {
    auto& s=*storage_;const auto widget=s.resources->widgets()->widgetState(36);
    if(!ancestorsShown||!widget.shown)return true;
    game::OriginalUiControlsModelPacket50KG packet;
    if(!s.resources->prepareControlsModelPacket(packet,d.resource))return fail(d,Error::resource,d.resource.sourcePC);
    if(!s.resources->currentControlsModelSelection(packet.selection))return fail(d,Error::sourceOrder,0x240C00);
    if(!packet.emitsGeometry)return true;
    // A single real LOD leaves no LOD-selection substitution. Any other
    // authored population requires its actual selector before admission.
    if(!packet.scene||packet.scene->lods.size()!=1||packet.sourceLod!=0||!packet.selection.selected())return fail(d,Error::geometry,0x241280);
    const auto& surfaces=packet.scene->lods[packet.sourceLod].surfaces;const auto& bindings=*s.resources->materials();
    for(std::uint32_t surface=0;surface<surfaces.size();++surface){
        const auto& source=surfaces[surface];const auto triangles=source.indices.size()/3;
        if(source.material_index>=packet.scene->materials.size()||source.indices.size()%3)return fail(d,Error::geometry,0x241280);
        if(!triangles)continue;
        if(triangles>game::originalUiLitMaximumTriangles50KM)return fail(d,Error::capacity,0x241280);
        const auto& sourceName=packet.scene->materials[source.material_index];
        // Both real PSM passes are retained. ps2_red alone has one pass;
        // the cold resource owner rejects any other authored pass population.
        for(std::uint32_t pass=1;pass<=2;++pass){
            const auto material=s.resources->controlsMaterial(sourceName,pass);
            if(material==unresolved){if(pass==1)return fail(d,Error::material);break;}
            if(material>=bindings.size())return fail(d,Error::material);
            const auto& binding=bindings[material];
            if(!binding.model||binding.images.size()!=1||binding.material.passes.size()!=1)return fail(d,Error::material);
            game::OriginalUiLitSurfaceOutput50KM output{s.modelVertices.data(),s.modelW.data(),s.modelFog.data(),
                game::originalUiLitMaximumTriangles50KM,0,0};
            const auto status=binding.specular?
                game::projectOriginalUiSpecularMapCompressedSurface50KM(*packet.scene,packet.sourceLod,surface,packet.palette,packet.context,output,s.modelWorkspace):
                game::projectOriginalUiLitSceneSurface50KM(*packet.scene,packet.sourceLod,surface,packet.palette,packet.context,packet.computeRadialFog,output,s.modelWorkspace);
            if(status!=game::OriginalUiLitGeometryStatus50KM::complete||output.materialIndex!=source.material_index||output.slotCount!=triangles)
                return fail(d,Error::geometry,binding.specular?0x33B8C0:0x331E00);
            if(s.vertices.size()/4+output.slotCount>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)
                return fail(d,Error::capacity);
            const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
            const auto count=std::size_t(output.slotCount)*4;
            s.vertices.insert(s.vertices.end(),s.modelVertices.begin(),s.modelVertices.begin()+count);
            s.w.insert(s.w.end(),s.modelW.begin(),s.modelW.begin()+count);s.fog.insert(s.fog.end(),s.modelFog.begin(),s.modelFog.begin()+count);
            s.packets.push_back({first,output.slotCount,material,binding.images[0],0,ordinal,0x241280,36,Segment::ordinaryWidgets});
        }
    }
    return s.resources->currentControlsModelSelection(packet.selection)||fail(d,Error::sourceOrder,0x240C00);
}
bool OriginalUiOptionsDrawContribution50KG::projectControllerRequiredPopup(const game::OriginalUiControllerPopupRender50KG& render,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||s.popupVisited||s.ordinary||s.page||s.top||s.scene||s.finished||s.failed||
       !s.popup->current(this,s.popupRevision))return fail(d,Error::sourceOrder,0x2064C0);
    if(s.sourcePage!=1&&(render.drawDim||render.drawMessage||render.drawConfirm||render.drawCancel||render.drawStrips))
        return fail(d,Error::input,0x2064C0);
    if(!s.popup->project(render,d.resource.controllerPopup)){s.failed=true;return fail(d,Error::projection,d.resource.controllerPopup.sourcePC);}
    const auto* geometry=s.popup->geometry();if(!geometry)return fail(d,Error::resource);
    for(std::size_t i=0;i<geometry->packetCount();++i){const auto& packet=geometry->packets()[i];
        const auto role=static_cast<std::size_t>(packet.material);if(role>=s.popupIndices.size()){s.failed=true;return fail(d,Error::material);}
        const auto material=s.popupIndices[role];
        if(bool(packet.quad)==bool(packet.text)){s.failed=true;return fail(d,Error::geometry,packet.sourcePC);}
        if(packet.quad){if(!append(*packet.quad,material,packet.packetDepth,Segment::controllerPopup,0,d)){s.failed=true;return false;}continue;}
        const auto& text=*packet.text;const auto& binding=(*s.resources->materials())[material];
        if(binding.images.size()!=1||binding.animated||text.count>text.quads.size()||packet.packetColor[3]>128||packet.packetDepth!=16777214.f){
            s.failed=true;return fail(d,Error::geometry,packet.sourcePC);}
        if(!text.count)continue;
        if(s.vertices.size()/4+text.count>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG){s.failed=true;return fail(d,Error::capacity);}
        for(std::size_t q=0;q<text.count;++q)for(const auto& v:text.quads[q].vertices)
            if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v)){s.failed=true;return fail(d,Error::geometry,packet.sourcePC);}
        const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
        for(std::size_t q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices){assets::Vertex48B v{};
            v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=packet.packetDepth;v.uv[0]=glyph.u;v.uv[1]=glyph.v;
            for(unsigned c=0;c<4;++c)v.color[c]=packet.packetColor[c];s.vertices.push_back(v);s.fog.push_back(0);s.w.push_back(1);}
        s.packets.push_back({first,text.count,material,binding.images[0],0,ordinal,packet.sourcePC,0,Segment::controllerPopup});
    }
    s.popupVisited=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectOrdinaryWidgets(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.popupVisited||s.ordinary||s.page||s.top||s.finished||s.failed)return fail(d,Error::sourceOrder);
    if(!widgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectPage(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.page||s.top||s.finished||s.failed)return fail(d,Error::sourceOrder);
    if(shown&&(s.sourcePage==5||s.sourcePage==6))return fail(d,Error::input,0x1F8990);
    s.page=true;s.pageShown=shown;if(!shown)return true;
    // Controls1F3CE0 interleaves background markers, labels/preview and the
    // aperture through its real controller effect stream. No menu arrows here.
    if(s.sourcePage==2||s.sourcePage==3)return true;
    const auto background=s.resources->materialIndex("ui/textures/mmbkg");
    const auto* bg=s.sourcePage==4?s.resources->gameOptionsBackground():s.resources->background();
    if(!bg){s.failed=true;return fail(d,Error::geometry);}
    for(const auto& q:bg->quadrants)
        if(!append(q,background,16777200.f,Segment::page,0,d)){s.failed=true;return false;}
    if(s.sourcePage==4&&s.resources->widgets()->refreshGameOptionsVolumeText(d.widgets)!=WidgetStatus::ok){s.failed=true;return fail(d,Error::projection);}
    game::OriginalUiOptionsSelection50KG selection;
    if(!(s.sourcePage==4?s.resources->gameOptionsSelection(selection,d.resource):s.resources->selection(selection,d.resource))){s.failed=true;return fail(d,Error::geometry);}
    if(!append(selection.beforeText,s.resources->materialIndex("ui/textures/arrwrght"),16777214.f,Segment::page,selection.textWidgetId,d)||
        !append(selection.afterText,s.resources->materialIndex("ui/textures/arrwleft"),16777214.f,Segment::page,selection.textWidgetId,d))
        {s.failed=true;return false;}
    return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectCreditsPage(std::uint32_t phase170,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||s.sourcePage!=6||!s.ordinary||s.page||s.top||s.finished||s.failed)return fail(d,Error::sourceOrder,0x1F8990);
    auto* bank=s.resources->widgets();const auto* controller=bank->creditsController();const auto* state=bank->creditsState();
    game::OriginalUiCreditsRender50KG render;
    if(!controller||!state||controller->render(*state,phase170,render)!=game::OriginalUiOptionsMenuStatus50KG::ok)
        return fail(d,Error::projection,0x1F8990);
    const auto* bg=s.resources->creditsBackground();
    const auto* patch=s.resources->creditsLogo(OriginalUiCreditsLogo50KG::permanentPatch);
    const auto* column=s.resources->creditsLogo(OriginalUiCreditsLogo50KG::permanentColumn);
    if(!bg||!patch||!column||!render.drawBackground||!render.drawPermanentLogos||!render.renderAperture)
        return fail(d,Error::resource,0x1F8990);
    s.page=true;s.pageShown=true;
    const auto background=s.resources->materialIndex("ui/textures/mmbkg"),lec=s.resources->materialIndex("ui/textures/logolec");
    for(const auto& q:bg->quadrants)
        if(!append(q,background,16777200.f,Segment::page,0,d)){s.failed=true;return false;}
    if(!append(*patch,lec,16777200.f,Segment::page,0,d)||!append(*column,lec,16777200.f,Segment::page,0,d))
        {s.failed=true;return false;}
    if(render.drawCredits){
        if(bank->setCreditsCardAlpha(render.controlId,render.cardAlpha,d.widgets)!=WidgetStatus::ok)
            {s.failed=true;return fail(d,Error::projection,0x1F8B4C);}
        const auto logo=[&](OriginalUiCreditsLogo50KG kind,std::uint8_t alpha,std::uint32_t pc) noexcept {
            const auto* original=s.resources->creditsLogo(kind);if(!original)return fail(d,Error::resource,pc);
            auto q=*original;q.sourcePacketColor=game::originalUiTextPacketColor50KG(0x00ffffff,alpha);q.sourcePC=pc;
            return append(q,s.resources->materialIndex(q.materialName),16777200.f,Segment::page,0,d);
        };
        const auto boundary=controller->config()->lecBeginControl7DC;
        const bool fadingRain=(render.selectedSlot==1&&state->phase7D4==0)||
            (render.selectedSlot+1==boundary&&state->phase7D4==2);
        const bool fadingLec=(render.selectedSlot==boundary&&state->phase7D4==0)||
            (render.selectedSlot==state->navigation.page->groups[1].controls.size()&&state->phase7D4>=2);
        if((render.drawRainLogo&&!logo(OriginalUiCreditsLogo50KG::rain,render.rainAlpha,fadingRain?0x1F8BFC:0x1F8C80))||
            (render.drawLecLogo&&!logo(OriginalUiCreditsLogo50KG::lec,render.lecAlpha,fadingLec?0x1F8D20:0x1F8D98)))
            {s.failed=true;return false;}
    }
    return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectMemoryCardPage(const game::OriginalUiMemoryCardRender50KG& r,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||s.sourcePage!=5||!s.ordinary||s.page||s.top||s.finished||s.failed)
        return fail(d,Error::sourceOrder,0x1F6DE0);
    if(!r.renderAperture||r.sourcePC!=0x1F6DE0||
        s.resources->widgets()->preflightMemoryCardPlan(r.page,d.widgets)!=WidgetStatus::ok)
        return fail(d,Error::projection,0x1F6DE0);
    if(s.resources->widgets()->commitMemoryCardPlan(r.page,d.widgets)!=WidgetStatus::ok){
        s.failed=true;return fail(d,Error::projection,0x1F6DE0);}
    s.page=true;s.pageShown=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectMemoryCardDialog(const game::OriginalUiMemoryCardRender50KG& r,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||s.sourcePage!=5||!s.page||!s.pageShown||s.apertureCount!=2||!s.prompts||
        s.memoryCardVisited||s.top||s.finished||s.failed)return fail(d,Error::sourceOrder,0x1D79BC);
    auto* owner=s.resources->memoryCardResources();
    if(!owner||!owner->project(r,d.resource.memoryCard))return fail(d,Error::resource,0x1FFFD0);
    // This command precedes the later material-group drain, not a guessed
    // material span. Preserve its reached ownership even when source RGBA=0.
    if(r.requiresDirectVideoSprite){
        if(!s.resources->memoryCardReady()||r.directSetupPC!=0x200C88||
            r.directSpritePC!=0x200C98||r.directRestorePC!=0x200CB0)
            return fail(d,Error::resource,0x200C98);
        s.directVideo.test=0x31001;s.directVideo.prim=0x46;s.directVideo.alpha=r.directAlpha;
        s.directVideo.rgba=r.state.color3E0;s.directVideo.width=640;s.directVideo.height=448;
        s.directVideo.entryPC=0x25B1A0;s.directVideo.producerPC=0x25E610;s.directVideo.submitPC=0x244A40;
        if(!validOriginalUiDirectVideo50KG(s.directVideo))return fail(d,Error::resource,0x25E610);
        s.directVideoPresent=true;
    }
    const auto* geometry=owner->geometry();if(!geometry)return fail(d,Error::resource,0x1FFFD0);
    for(std::size_t i=0;i<geometry->stripCount();++i)
        if(!append(geometry->strips()[i],owner->overlayMaterial(),16777214.f,Segment::memoryCardDialog,0,d)){
            s.failed=true;return false;}
    s.memoryCardVisited=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectControlsBackground(std::uint8_t ordinal,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||(s.sourcePage!=2&&s.sourcePage!=3)||s.top||s.apertureCount||s.prompts||
        s.failed||s.finished||ordinal!=s.controlsBackgroundCount||ordinal>3)return fail(d,Error::sourceOrder,0x1F3D28);
    const auto* background=s.resources->controlsBackground();if(!background)return fail(d,Error::resource);
    if(!append(background->quadrants[ordinal],s.resources->materialIndex("ui/textures/mmbkg"),16777200.f,Segment::page,0,d)){
        s.failed=true;return false;}
    ++s.controlsBackgroundCount;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectExitText(const game::OriginalUiApertureHook50KG& hook,Diagnostic& d) noexcept {
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
    auto* exitText=s.sourcePage==5?s.resources->memoryCardExitText():s.sourcePage==6?s.resources->creditsExitText():s.sourcePage==4?s.resources->gameOptionsExitText():
        (s.sourcePage==2||s.sourcePage==3)?s.resources->controlsExitText(s.sourcePage-2):s.resources->exitText();
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
bool OriginalUiOptionsDrawContribution50KG::projectAperture(std::uint8_t ordinal,
    const game::OriginalUiApertureRectangle50KG& rect,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.top||s.prompts||s.failed||s.finished||ordinal!=s.apertureCount||ordinal>1||
        rect.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rect.dirty3C||
        ((s.sourcePage==2||s.sourcePage==3)&&s.controlsBackgroundCount!=4))return fail(d,Error::sourceOrder,0x1D798C);
    game::OriginalUiOverlayQuad50KG q;q.materialName="NoTexture";q.authoredDepth=1;q.sourcePC=0x1D798C;q.sourcePacketColor={{0,0,0,63}};
    constexpr unsigned order[]={0,2,1,3};
    for(unsigned i=0;i<4;++i){const auto& v=rect.vertices[order[i]];q.vertices[i]={v.x,v.y,0,0};}
    if(!append(q,s.resources->materialIndex("ui/textures/notexture"),16777214.f,Segment::aperture,0,d)){s.failed=true;return false;}
    ++s.apertureCount;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectPrompts(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertureCount!=2||s.prompts||s.top||s.failed||s.finished)return fail(d,Error::sourceOrder);
    const auto mask=s.resources->widgets()->pageInputFlags42C();
    if(mask!=0&&mask!=((s.sourcePage==5||s.sourcePage==6)?2u:(s.sourcePage==2||s.sourcePage==3)?15u:s.sourcePage==4?7u:3u))return fail(d,Error::projection,s.sourcePage==6?0x1F8800:s.sourcePage==4?0x1F5D30:0x1F9E30);
    constexpr const char* names[]={"ui/textures/icon_x","ui/textures/icon_tri","ui/textures/icon_sqr","ui/textures/icon_cir"};
    constexpr float menuX[]={484,397,0,0},gameX[]={514,410,280,0},controlsX[]={514,410,158,22};
    const auto* x=(s.sourcePage==2||s.sourcePage==3)?controlsX:s.sourcePage==4?gameX:menuX;
    constexpr std::uint32_t pc[]={0x1D7A2C,0x1D7A44,0x1D7A5C,0x1D7A74};
    for(unsigned i=0;i<((s.sourcePage==2||s.sourcePage==3)?4u:s.sourcePage==4?3u:2u);++i)if(mask&(1u<<i)) {
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
        (s.pageShown&&(!s.prompts||s.apertureCount!=2))||(s.sourcePage==5&&!s.memoryCardVisited))return fail(d,Error::sourceOrder);
    if(!widgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::projectParticles(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.top||s.scene||s.failed||s.drained)return fail(d,Error::sourceOrder);
    auto& fire=*s.resources->fire();const auto& scene=*s.resources->scene();
    const auto& renderState=*s.resources->particleRenderState();
    const auto* definition=scene.definition();const auto* material=scene.particleMaterial();
    if(!scene.prepared()||!definition||!material||material->identity()!=s.resources->particleMaterial()->identity)
        return fail(d,Error::resource);
    // The scene fog entity runs after the manager fog entity and rewrites the
    // source camera near/far before ParticleCode sees its constants. Consume
    // that concrete prepared state, including exact packed FOGCOL conversion.
    for(unsigned c=0;c<3;++c){
        const auto value=renderState.fogColor[c];
        if(!std::isfinite(value)||value<0||value>1)return fail(d,Error::input);
        s.fogColor[c]=value;
    }
    const auto& camera=renderState.particleCamera;const auto& constants=renderState.constants;
    const game::OriginalUiMainParticleView50KG view{{camera.combinedSourceScreen250[2],camera.combinedSourceScreen250[6],
        camera.combinedSourceScreen250[10],camera.combinedSourceScreen250[14]},camera.focalPixels1D8};
    d.fire=fire.preparePackets(view);
    if(d.fire!=game::OriginalUiMainFireStatus50KG::complete){s.failed=true;return fail(d,Error::projection);}
    const auto count=fire.packetCount();const auto* source=fire.packets();
    if(count>game::originalUiMainParticleCapacity50KG||(count&&!source)||
        s.vertices.size()/4+count>originalUiMaximumSourceQuads50KM)return fail(d,Error::capacity);
    // Each source run is at most12 and chooses ONE of the two material states.
    // Header validation occurs before projecting any of this batch.
    for(std::size_t start=0;start<count;){
        const auto signedCount=source[start].signedRunCount;
        if(signedCount==0||signedCount<-12||signedCount>12)return fail(d,Error::input);
        const auto n=static_cast<std::size_t>(signedCount<0?-signedCount:signedCount);
        if(n>count-start)return fail(d,Error::input);
        for(std::size_t j=1;j<n;++j)if(source[start+j].signedRunCount)return fail(d,Error::input);
        start+=n;
    }
    for(std::size_t start=0;start<count;){
        const auto signedCount=source[start].signedRunCount;
        const auto n=static_cast<std::size_t>(signedCount<0?-signedCount:signedCount);
        const std::uint8_t state=signedCount<0?1:0;
        const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
        for(std::size_t j=0;j<n;++j){
            game::OriginalUiMainParticleQuad50KG quad;
            d.particle=game::projectOriginalUiMainParticleQuad50KG(source[start+j],state,camera,constants,quad);
            if(d.particle!=game::OriginalUiMainParticleGeometryStatus50KG::complete){s.failed=true;return fail(d,Error::projection);}
            if(!quad.visible)continue;
            s.vertices.insert(s.vertices.end(),quad.vertices.begin(),quad.vertices.end());
            s.fog.insert(s.fog.end(),quad.fogFactors.begin(),quad.fogFactors.end());s.w.insert(s.w.end(),4,1.f);
        }
        const auto quads=static_cast<std::uint32_t>(s.vertices.size()/4)-first;
        if(quads){
            if(s.packets.size()>=originalUiMaximumSpans50KG){s.failed=true;return fail(d,Error::capacity);}
            s.packets.push_back({first,quads,unresolved,s.resources->particleImageIndex(),state,static_cast<std::uint32_t>(s.packets.size()),
                0x162248,0,Segment::particles});
        }
        start+=n;
    }
    s.scene=true;return true;
}
bool OriginalUiOptionsDrawContribution50KG::drain(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.popupVisited||!s.ordinary||!s.page||!s.prompts||!s.top||!s.scene||s.failed||s.drained||s.apertureCount!=2||(s.sourcePage==5&&!s.memoryCardVisited))return fail(d,Error::unfinished);
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
        // ParticleCode token2902 precedes every admitted Options material and
        // cannot merge with these distinct ScreenObject/Diffuse identities.
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
        if(p.materialIndex==unresolved)okay=convertOriginalUiOptionsParticleSpan50KG(*s.resources->particleMaterial(),
            static_cast<std::uint8_t>(p.frameIndex),{0,p.quadCount,p.quadCount,p.imageIndex,s.gpu->textureCount()},converted);
        else {
            const auto& material=materials[p.materialIndex];
            const OriginalUiMaterialSpanBinding50KG range{0,p.quadCount,p.quadCount,p.imageIndex,s.gpu->textureCount()};
            okay=material.model?(material.specular?
                convertOriginalUiControlsSpecularSpan50KG(material.controlsSpecular,*s.resources->images(),range,converted):
                convertOriginalUiLitMaterialSpan50KM(material.lit,range,converted)):
                material.images.empty()?convertOriginalUiUntexturedMaterialSpan50KG(material.untextured,{0,p.quadCount,p.quadCount},converted):
                convertOriginalUiMaterialSpan50KG(material.policies[p.frameIndex],range,converted);
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
    // Actual Options PageInfo background, retained in the authenticated page owner.
    // Scene tail has already emitted the final shared FOGCOL. The earlier
    // Widget36 manager-blue fog word is provenance, not a per-model GS write;
    // its actual palette fog factors and this final drain color stay separate.
    const auto* page=s.sourcePage==5?s.resources->widgets()->memoryCardPage():s.sourcePage==6?s.resources->widgets()->creditsPage():s.sourcePage==4?s.resources->widgets()->gameOptionsPage():
        (s.sourcePage==2||s.sourcePage==3)?s.resources->widgets()->controlsPage():s.resources->widgets()->page();
    for(unsigned c=0;c<3;++c)s.draw.draw.clearRgba[c]=page->backgroundColor[c]/255.f;
    s.draw.draw.clearRgba[3]=1;s.draw.draw.retainRaceImage=false;
    s.draw.states=s.states.data();s.draw.stateCount=static_cast<std::uint32_t>(s.states.size());
    s.draw.particleFog=s.drainedFog.data();s.draw.particleFogCount=static_cast<std::uint32_t>(s.drainedFog.size());
    for(unsigned c=0;c<3;++c)s.draw.particleFogColor[c]=s.fogColor[c];
    s.draw.sourceRasterW=s.drainedW.data();s.draw.sourceRasterWCount=static_cast<std::uint32_t>(s.drainedW.size());
    s.draw.opaquePresentationAlphaOnly=false;
    // Every accepted Options frame contains an actual non-particle terminal
    // group. A particle-only partial contribution cannot obtain this seal.
    if(terminalMaterial==unresolved)return fail(d,Error::unfinished);
    const auto& terminalBinding=materials[terminalMaterial];const auto& terminalPass=terminalBinding.material.passes[0];
    OriginalUiSourceTerminal50KN terminal{};terminal.canonicalToken=terminalBinding.registryToken;
    terminal.pass=terminalPass.pass;terminal.leaf=s.leaf[terminalMaterial];
    // This immutable Options owner has one successful cold bank publication
    // and one source group construction. Neither is rebuilt on page changes;
    // the separate frameGeneration records each real accepted submission.
    terminal.registryGeneration=1;terminal.bankGeneration=1;
    if(!s.pendingHistory[terminalMaterial].copyEffectiveGs(terminal.gs))return fail(d,Error::material);
    for(std::size_t f=0;f<terminalPass.gs.fields.size();++f){terminal.constructionFieldSourcePC[f]=terminalPass.gs.fields[f].sourcePC;
        terminal.constructionFieldByteOffset[f]=terminalPass.gs.fields[f].byteOffset;}
    terminal.enabled=terminalPass.enabled;terminal.textureCount=terminalPass.textureCount;terminal.textureCoordinateIndex=terminalPass.textureCoordinateIndex;
    terminal.lodCalculationMethod=terminalPass.lodCalculationMethod;terminal.maximumMipLevel=terminalPass.maximumMipLevel;
    terminal.magFilter=terminalPass.magFilter;terminal.minFilter=terminalPass.minFilter;terminal.lodL=terminalPass.lodL;terminal.lodK=terminalPass.lodK;
    for(std::size_t i=0;i<s.popupIndices.size();++i)s.popupPendingHistory[i]=s.pendingHistory[s.popupIndices[i]];
    if(s.directVideoPresent&&(!s.resources->memoryCardReady()||!validOriginalUiDirectVideo50KG(s.directVideo)))
        return fail(d,Error::resource,0x25E610);
    frame_.directVideoPresent_=s.directVideoPresent;
    frame_.directVideo_=s.directVideoPresent?s.directVideo:OriginalUiDirectVideo50KG{};
    frame_.directVideoCapability_=s.directVideoPresent?s.resources->video()->directVideoCapability:OriginalUiDirectVideoCapability50KG{};
    s.drained=true;s.finished=true;frame_.native_=&s.draw;frame_.owner_=this;frame_.frameGeneration_=++s.frameGeneration;frame_.terminal_=terminal;
    return true;
    }catch(...){s.failed=true;frame_.native_=nullptr;frame_.owner_=nullptr;return fail(d,Error::allocation);}
}
const OriginalUiNativeDraw50KG* OriginalUiOptionsDrawContribution50KG::draw() const noexcept {
    return storage_&&storage_->drained&&!storage_->failed?&storage_->draw:nullptr;
}
const OriginalUiSourceFrame50KM* OriginalUiOptionsDrawContribution50KG::sourceFrame() const noexcept {
    return storage_&&storage_->drained&&!storage_->failed&&!storage_->historyPoisoned&&!storage_->submissionPending&&
        storage_->popupLeased&&storage_->popup->current(this,storage_->popupRevision)&&frame_.native_?&frame_:nullptr;
}
bool OriginalUiOptionsDrawContribution50KG::sourceTeardownPending() const noexcept {return storage_&&storage_->submittedRenderer;}
bool OriginalUiOptionsDrawContribution50KG::retireSourceRenderer(VehicleWindowGl50DS& renderer,Diagnostic& d) noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.submissionPending||(s.submittedRenderer&&s.submittedRenderer!=&renderer))return fail(d,Error::sourceOrder);
    if(s.submittedRenderer){VehicleWindowDiagnostic50DS detail;
        if(!renderer.retireOriginalUiSourceOwner(*this,detail))return fail(d,Error::publication);}
    s.submittedRenderer=nullptr;s.popup->release(this,true);s.popupLeased=false;
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;return true;
}
bool OriginalUiOptionsDrawContribution50KG::beginSourceSubmission(std::uint64_t generation,const VehicleWindowGl50DS& renderer) const noexcept {
    if(!storage_||!sourceFrame()||generation!=frame_.frameGeneration_)return false;auto& s=*storage_;
    if(s.submissionPending||s.historyPoisoned||(s.submittedRenderer&&s.submittedRenderer!=&renderer)||
        generation<=s.attemptedGeneration||generation<=s.acknowledgedGeneration||s.pendingHistory.size()!=s.history.size())return false;
    s.submittedRenderer=&renderer;s.attemptedGeneration=generation;s.submissionPending=true;return true;
}
void OriginalUiOptionsDrawContribution50KG::finishSourceSubmission(std::uint64_t generation,bool accepted,bool started) const noexcept {
    if(!storage_)return;auto& s=*storage_;
    if(!s.submissionPending||generation!=frame_.frameGeneration_){s.historyPoisoned=true;s.popup->poison(this);return;}
    s.submissionPending=false;
    if(accepted){if(s.pendingHistory.size()!=s.history.size()||!s.popup->commit(this,s.popupRevision,s.popupPendingHistory)){
            s.historyPoisoned=true;s.popup->poison(this);return;}
        s.history.swap(s.pendingHistory);s.acknowledgedGeneration=generation;}
    else if(started){s.historyPoisoned=true;s.popup->poison(this);}
}
bool OriginalUiOptionsDrawContribution50KG::finishContribution(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.popupVisited||!s.ordinary||!s.page||!s.top||!s.scene||s.failed||s.finished||
        (s.pageShown&&(!s.prompts||s.apertureCount!=2))||(s.sourcePage==5&&!s.memoryCardVisited))return fail(d,Error::unfinished);
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
