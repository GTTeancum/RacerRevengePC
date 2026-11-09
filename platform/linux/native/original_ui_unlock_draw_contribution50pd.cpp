#include "original_ui_unlock_draw_contribution50pd.h"
#include "original_ui_material_adapter50kg.h"
#include "original_ui_unlock_model_material50pd.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>

namespace rr::platform {
namespace {
using Error=OriginalUiUnlockContributionError50PD;using Diagnostic=OriginalUiUnlockContributionDiagnostic50PD;
using Segment=OriginalUiUnlockSegment50PD;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(Diagnostic& d,Error e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;return false;}
bool unlockGalleryIdentity(const assets::OriginalUiMemberIdentity50KG* member) noexcept {
    if(!member)return false;
    const auto* archive=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::unlock);if(!archive)return false;
    bool canonical=false;for(std::size_t i=0;i<archive->memberCount;++i)canonical|=assets::originalUiMemberIdentity50KG(archive->id,i)==member;
    if(!canonical||!member->name)return false;
    for(const char* name:{"ui\\textures\\artpod01.pst","ui\\textures\\artpil01.pst","ui\\textures\\artpla01.pst"})
        if(std::strcmp(member->name,name)==0)return true;
    return false;
}
}
bool prepareOriginalUiUnlockBackgroundSpan50PD(const assets::OriginalUiScreenAssets50KG& images,std::uint32_t image,
    OriginalUiSpan50KG& span,OriginalUiNativeSpan50KG& state) noexcept {
    const auto* source=images.sourceProvenance(image);const auto* decoded=images.decodedTexture(image);
    const auto* bank=images.textures();const auto* texture=bank?bank->texture(image):nullptr;
    if(!source||!decoded||!texture||!source->memberIdentity||source->celIdentity||source->pstOffset||
       source->pstBytes!=287772||decoded->kind!=1||decoded->mips.size()!=1||
       texture->texture.width!=640||texture->texture.height!=448||!unlockGalleryIdentity(source->memberIdentity))return false;
    // Same raw 25CC60 packet policy as the Hall raw background path.
    OriginalUiSpan50KG s;s.firstQuad=0;s.quadCount=1;s.textureIndex=image;
    s.blend=OriginalUiBlend50KG::sourceAlpha;s.colorScale=255.f/128.f;s.alphaScale=(255.f/128.f)*(255.f/128.f);
    s.materialMode=OriginalUiMaterialMode50KG::texturedModulate;
    OriginalUiNativeSpan50KG n;n.depth=OriginalUiDepth50KG::always;n.depthWrite=false;
    n.sampler.minification=MaterialTextureFilter50EB::linear;n.sampler.magnification=MaterialTextureFilter50EB::linear;
    n.sampler.s=MaterialTextureWrap50EB::clampToEdge;n.sampler.t=MaterialTextureWrap50EB::clampToEdge;
    span=s;state=n;return true;
}
struct OriginalUiUnlockDrawContribution50PD::Storage {
    std::shared_ptr<OriginalUiUnlockDrawResources50PD> resources;
    std::vector<assets::Vertex48B> vertices,drainedVertices;
    std::vector<OriginalUiUnlockPacket50PD> packets;
    std::vector<float> fog,drainedFog,w,drainedW;
    std::vector<assets::OriginalUiMaterialBindingState50KG> history,pendingHistory;
    std::vector<std::uint32_t> leaf,representatives,order;
    std::vector<OriginalUiSpan50KG> spans;std::vector<OriginalUiNativeSpan50KG> states;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;std::uint64_t publication=0;
    OriginalUiNativeDraw50KG draw{};
    std::uint64_t frameGeneration=0,attemptedGeneration=0,acknowledgedGeneration=0;
    const VehicleWindowGl50DS* submittedRenderer=nullptr;
    bool submissionPending=false,historyPoisoned=false;
    OriginalUiUnlockClock50PD clock;
    // 3D pass: long-lived lit-geometry workspace and per-surface scratch.
    std::unique_ptr<game::OriginalUiLitGeometryWorkspace50KM> workspace;
    std::vector<assets::Vertex48B> scratchVertices;std::vector<float> scratchW,scratchFog;
    const game::OriginalUiUnlockCameraFrame50PD* camera=nullptr;
    bool drained=false,exitText=false,active=false,ordinary=false,page=false,pageShown=false,prompts=false,top=false,finished=false,failed=false;
    std::uint8_t apertureCount=0;
    // Same canonical registry-token group construction as the Hall composer.
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
                bindings[a].material.passes[0].pass<bindings[b].material.passes[0].pass;});
        struct Group {std::uint32_t binding=0,redirect=unresolved,limit=unresolved;};
        std::vector<Group> groups;groups.reserve(count);std::uint32_t previous=unresolved;
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
};
OriginalUiUnlockDrawContribution50PD::OriginalUiUnlockDrawContribution50PD() noexcept=default;
OriginalUiUnlockDrawContribution50PD::~OriginalUiUnlockDrawContribution50PD()=default;
bool OriginalUiUnlockDrawContribution50PD::bind(std::shared_ptr<OriginalUiUnlockDrawResources50PD> resources,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyBound);
    if(!resources||!resources->loaded())return fail(d,Error::notBound);
    try {
        auto s=std::make_unique<Storage>();s->resources=std::move(resources);
        for(const auto& m:*s->resources->materials())if(m.animated)return fail(d,Error::clock); // no animated UIUnlock material
        s->vertices.reserve(originalUiMaximumSourceQuads50KM*4u);s->packets.reserve(originalUiMaximumSpans50KG);
        s->fog.reserve(originalUiMaximumSourceQuads50KM*4u);s->drainedFog.reserve(originalUiMaximumSourceQuads50KM*4u);
        s->drainedVertices.reserve(originalUiMaximumSourceQuads50KM*4u);s->order.reserve(originalUiMaximumSpans50KG);
        s->spans.reserve(originalUiMaximumSpans50KG);s->states.reserve(originalUiMaximumSpans50KG);
        s->w.reserve(originalUiMaximumSourceQuads50KM*4u);s->drainedW.reserve(originalUiMaximumSourceQuads50KM*4u);
        if(!s->prepareGroups(d))return false;
        s->workspace=std::make_unique<game::OriginalUiLitGeometryWorkspace50KM>();
        if(!s->resources->camera()||!s->resources->camera()->frame()||!s->resources->models())return fail(d,Error::resource);
        storage_=std::move(s);return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiUnlockDrawContribution50PD::bindFrame(std::shared_ptr<OriginalUiUnlockDrawResources50PD> resources,
    std::shared_ptr<const MaterialTextureGl50EB> gpu,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyBound);
    if(!resources||!resources->loaded()||!gpu||!gpu->initialized()||!gpu->publicationToken()||
       gpu->textureCount()!=resources->textures()->textureCount())return fail(d,Error::publication);
    for(std::uint32_t i=0;i<gpu->textureCount();++i){const auto* a=gpu->asset(i);const auto* b=resources->textures()->texture(i);
        if(!a||!b||a->identity!=b->identity)return fail(d,Error::publication);}
    if(!bind(std::move(resources),d))return false;
    storage_->publication=gpu->publicationToken();storage_->gpu=std::move(gpu);return true;
}
bool OriginalUiUnlockDrawContribution50PD::begin(const OriginalUiUnlockClock50PD& clock,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);if(!clock.known)return fail(d,Error::clock);
    auto& s=*storage_;if(s.gpu&&(!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication))return fail(d,Error::publication);
    if(s.submissionPending||s.historyPoisoned)return fail(d,Error::sourceOrder);
    const auto image=s.resources->currentBackgroundImage();OriginalUiSpan50KG span;OriginalUiNativeSpan50KG state;
    if(image!=unresolved&&!prepareOriginalUiUnlockBackgroundSpan50PD(*s.resources->images(),image,span,state))return fail(d,Error::material,0x2A6E90);
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;frame_.terminal_={};
    s.w.clear();s.drainedW.clear();s.fog.clear();s.drainedFog.clear();s.drainedVertices.clear();s.order.clear();s.spans.clear();s.states.clear();
    s.drained=s.exitText=false;s.clock=clock;s.active=true;
    s.ordinary=s.page=s.pageShown=s.prompts=s.top=s.finished=s.failed=false;s.apertureCount=0;s.vertices.clear();s.packets.clear();
    // Visibility 27F650 membership/guard cameras for this frame's model traversal.
    s.camera=s.resources->camera()->frame();
    {game::OriginalUiUnlockModelDiagnostic50PD md;if(!s.camera||!s.resources->models()->beginFrame(*s.camera,md)){s.failed=true;return fail(d,Error::geometry,md.sourcePC);}}
    // Root +44 (2A6E90) draws the retained page+154 texture before +48.
    if(image==unresolved)return true;
    constexpr float xy[4][2]={{0,0},{640,0},{640,448},{0,448}};
    for(unsigned i=0;i<4;++i){assets::Vertex48B v{};
        v.position[0]=xy[i][0];v.position[1]=xy[i][1];v.position[2]=0;
        v.uv[0]=(i==1||i==2)?1.f:1.f/640;v.uv[1]=i>=2?1.f:1.f/448;
        v.color[0]=v.color[1]=v.color[2]=128;v.color[3]=64;
        s.vertices.push_back(v);s.fog.push_back(0.f);s.w.push_back(1.f);}
    s.packets.push_back({0,1,unresolved,image,0,0,0x2A6E90,0,Segment::rawBackground});return true;
}
bool OriginalUiUnlockDrawContribution50PD::append(const game::OriginalUiOverlayQuad50KG& quad,std::uint32_t material,float z,
    Segment segment,std::uint16_t widgetId,Diagnostic& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(material>=materials.size()||!std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z)return fail(d,Error::geometry);
    if(s.vertices.size()/4>=originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)return fail(d,Error::capacity);
    const auto& binding=materials[material];if(binding.animated)return fail(d,Error::clock);
    const auto image=binding.images.empty()?unresolved:binding.images[0];
    constexpr unsigned order[]={0,1,3,2};std::array<assets::Vertex48B,4> vertices{};
    for(unsigned i=0;i<4;++i){const auto& v=quad.vertices[order[i]];auto& out=vertices[i];
        if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v)||quad.sourcePacketColor[3]>128)
            return fail(d,Error::geometry);
        out.position[0]=v.x;out.position[1]=v.y;out.position[2]=z;out.uv[0]=v.u;out.uv[1]=v.v;
        for(unsigned c=0;c<4;++c)out.color[c]=quad.sourcePacketColor[c];}
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
    s.vertices.insert(s.vertices.end(),vertices.begin(),vertices.end());s.fog.insert(s.fog.end(),4,0.f);s.w.insert(s.w.end(),4,1.f);
    s.packets.push_back({first,1,material,image,0,ordinal,quad.sourcePC,widgetId,segment});return true;
}
bool OriginalUiUnlockDrawContribution50PD::appendText(const game::OriginalUiTextLayout50KG& text,const std::array<std::uint8_t,4>& color,
    float z,std::uint32_t material,std::uint32_t pc,Segment segment,std::uint16_t id,Diagnostic& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(material>=materials.size()||color[3]>128||text.count>text.quads.size()||!std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z)
        return fail(d,Error::geometry,pc);
    const auto& binding=materials[material];if(binding.images.size()!=1||binding.animated)return fail(d,Error::material,pc);
    if(!text.count)return true;
    if(s.vertices.size()/4+text.count>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)return fail(d,Error::capacity,pc);
    for(std::size_t q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices)
        if(!std::isfinite(glyph.x)||!std::isfinite(glyph.y)||!std::isfinite(glyph.u)||!std::isfinite(glyph.v))return fail(d,Error::geometry,pc);
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
    for(std::size_t q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices){assets::Vertex48B v{};
        v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=z;v.uv[0]=glyph.u;v.uv[1]=glyph.v;
        for(unsigned c=0;c<4;++c)v.color[c]=color[c];
        s.vertices.push_back(v);s.fog.push_back(0.f);s.w.push_back(1.f);}
    s.packets.push_back({first,text.count,material,binding.images[0],0,ordinal,pc,id,segment});return true;
}
bool OriginalUiUnlockDrawContribution50PD::widgets(bool top,bool shown,Diagnostic& d) noexcept {
    auto& s=*storage_;auto* bank=s.resources->widgets();const auto* order=bank->order(top);if(!order)return fail(d,Error::projection);
    const auto segment=top?Segment::onTopWidgets:Segment::ordinaryWidgets;
    for(auto id:*order){d.widgetId=id;const auto material=s.resources->widgetMaterial(id);
        if(const auto* descriptor=bank->textDescriptor(id)){
            const auto* text=s.resources->textGeometry(id);
            // A dynamic widget has no geometry until its first 2A9FF0.
            if(!text){if(bank->dynamicText(id))continue;return fail(d,Error::geometry);}
            float z=0;if(!s.resources->textPacketDepth(id,z))return fail(d,Error::geometry);
            for(std::size_t lineIndex=0;lineIndex<text->lines().size();++lineIndex){
                const auto& line=text->lines()[lineIndex];
                game::OriginalUiUnlockTextProjection50PD live;
                if(!bank->projectText(id,lineIndex,line.glyphs.fontLineHeight,line.measuredWidth,line.measuredHeight,shown,live,d.widgets)){
                    if(d.widgets.error==game::OriginalUiUnlockWidgetError50PD::hidden)continue;return fail(d,Error::projection);}
                if(live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128||
                   live.position.positionedX!=line.position.positionedX||live.position.positionedY!=line.position.positionedY||
                   lineIndex>=descriptor->lines.size()||!live.text)return fail(d,Error::projection);
                if(!appendText(line.glyphs,live.line.packetColor,z,material,0x2E0A30,segment,id,d))return false;
            }
        }else if(bank->iconDescriptor(id)){
            game::OriginalUiUnlockIconProjection50PD live;
            if(!bank->projectIcon(id,shown,live,d.widgets)){
                if(d.widgets.error==game::OriginalUiUnlockWidgetError50PD::hidden)continue;return fail(d,Error::projection);}
            if(material>=s.resources->materials()->size())return fail(d,Error::material);
            const auto& binding=(*s.resources->materials())[material];
            if(binding.images.empty()||live.images!=s.resources->images()||live.imageIndex!=binding.images[0])return fail(d,Error::projection);
            if(!append(live.layout.quad,material,live.layout.packetDepth,segment,id,d))return false;
        }
        else if(s.resources->models()->dynamicWidget(id)){
            // User_Rotate pod / Soultree_Character pilot, in widget traversal order.
            if(shown&&!model(id,d))return false;
        }
        // Empty: no geometry.
    }
    return true;
}
bool OriginalUiUnlockDrawContribution50PD::model(std::uint16_t id,Diagnostic& d) noexcept {
    auto& s=*storage_;auto& models=*s.resources->models();if(!models.loaded(id))return true;
    game::OriginalUiUnlockModelDiagnostic50PD md;game::OriginalUiUnlockModelPacket50PD selected;
    if(!s.camera||!models.prepare(id,*s.camera,selected,md))return fail(d,Error::projection,md.sourcePC);
    if(selected.emitsGeometry){
        const auto& packet=selected.packet;const auto* materials=selected.materials;
        if(!packet.scene||!materials||packet.scene->lods.size()!=1)return fail(d,Error::geometry);
        const auto& surfaces=packet.scene->lods[0].surfaces;
        for(std::uint32_t surface=0;surface<surfaces.size();++surface){
            const auto m=surfaces[surface].material_index;
            if(m>=materials->materialCount()||surfaces[surface].indices.size()%3)return fail(d,Error::geometry);
            const auto count=surfaces[surface].indices.size()/3;if(!count)continue;
            if(count>originalUiMaximumSourceQuads50KM)return fail(d,Error::capacity);
            s.scratchVertices.resize(count*4);s.scratchW.resize(count*4);s.scratchFog.resize(count*4);
            for(std::uint32_t p=0;p<materials->passCount(m);++p){
                const auto* pass=materials->pass(m,p);if(!pass)return fail(d,Error::material);
                const auto binding=s.resources->modelBinding(materials,m,p);
                const auto image=pass->imageIndex==unresolved?unresolved:s.resources->modelImage(materials,pass->imageIndex);
                if(binding==unresolved||(pass->imageIndex!=unresolved&&image==unresolved))return fail(d,Error::material,0x24BD7C);
                game::OriginalUiLitSurfaceOutput50KM projected{s.scratchVertices.data(),s.scratchW.data(),s.scratchFog.data(),static_cast<std::uint32_t>(count),0,0};
                const bool colored=surfaces[surface].has_original_colors&&selected.characterAssets&&selected.characterAssets->scene()==packet.scene;
                const auto status=colored?
                    game::projectOriginalUiUnlockColoredSurface50PD(*selected.characterAssets,0,surface,p,pass->source.microprogram,
                        packet.palette,packet.context,packet.fog.computeRadialFog1B2,projected,*s.workspace):
                    unlock_model_material50pd::same(pass->source.microprogram,"SpecularMapCompressed")?
                    game::projectOriginalUiSpecularMapCompressedSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,projected,*s.workspace):
                    game::projectOriginalUiLitSceneSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,packet.fog.computeRadialFog1B2,projected,*s.workspace);
                if(status!=game::OriginalUiLitGeometryStatus50KM::complete||projected.materialIndex!=m)return fail(d,Error::geometry,0x24BD7C);
                if(!projected.slotCount)continue;
                if(s.vertices.size()/4+projected.slotCount>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)
                    return fail(d,Error::capacity);
                const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);const auto n=std::size_t(projected.slotCount)*4;
                s.vertices.insert(s.vertices.end(),s.scratchVertices.begin(),s.scratchVertices.begin()+n);
                s.w.insert(s.w.end(),s.scratchW.begin(),s.scratchW.begin()+n);s.fog.insert(s.fog.end(),s.scratchFog.begin(),s.scratchFog.begin()+n);
                s.packets.push_back({first,projected.slotCount,binding,image,0,static_cast<std::uint32_t>(s.packets.size()),0x24BD7C,id,Segment::ordinaryWidgets});
            }
        }
    }
    // Entity children: both binder helpers run even after a culled parent.
    for(std::size_t pair=0;pair<2;++pair)if(models.binderPresent(id,pair)){
        game::OriginalUiUnlockBinderPacket50PD packet;
        if(!models.projectBinder(id,pair,*s.camera,s.clock.milliseconds,s.clock.advanceAnimations,packet,md))return fail(d,Error::projection,md.sourcePC);
        const auto binding=s.resources->binderBinding();const auto image=s.resources->binderImage(packet.image.imageIndex);
        if(binding==unresolved||image==unresolved)return fail(d,Error::material,0x1D4240);
        if(s.vertices.size()/4+2>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)return fail(d,Error::capacity);
        const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);const auto& g=packet.geometry;
        s.vertices.insert(s.vertices.end(),g.vertices.begin(),g.vertices.end());
        s.w.insert(s.w.end(),g.rasterW.begin(),g.rasterW.end());s.fog.insert(s.fog.end(),g.fogFactors.begin(),g.fogFactors.end());
        s.packets.push_back({first,2,binding,image,packet.image.imageIndex,static_cast<std::uint32_t>(s.packets.size()),0x1D4240,id,Segment::ordinaryWidgets});
    }
    return true;
}
bool OriginalUiUnlockDrawContribution50PD::projectOrdinaryWidgets(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||s.ordinary||s.page||s.top||s.finished||s.failed)return fail(d,Error::sourceOrder);
    if(!widgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiUnlockDrawContribution50PD::projectPage(std::int32_t pod,bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.page||s.top||s.finished||s.failed||pod<-1||pod>26)return fail(d,Error::sourceOrder,0x20B710);
    s.page=true;s.pageShown=shown;if(!shown||pod==-1)return true;
    // 20C9B8..20C9E8: page+4C0+pod*180+i*60 -> 2DF550, i=0..3. A pod whose
    // sprites the init never built is not drawn (no substitute image).
    const auto* sprites=s.resources->sprites(pod);if(!sprites)return true;
    for(const auto& quad:sprites->quadrants)
        if(!append(quad,sprites->material,16777200.f,Segment::page,0,d)){s.failed=true;return false;} // depth15
    return true;
}
bool OriginalUiUnlockDrawContribution50PD::projectExitText(const game::OriginalUiApertureHook50KG& hook,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertureCount||s.prompts||s.top||s.exitText||s.failed||s.finished)return fail(d,Error::sourceOrder,0x1D75A4);
    if(s.vertices.size()/4+10>originalUiMaximumSourceQuads50KM||s.packets.size()>=originalUiMaximumSpans50KG)return fail(d,Error::capacity);
    const auto m=s.resources->materialIndex("font/denmrk16top");
    if(m>=s.resources->materials()->size())return fail(d,Error::material);
    const auto& binding=(*s.resources->materials())[m];if(binding.images.size()!=1)return fail(d,Error::material);
    game::OriginalUiExitTextProjection50KG live;auto* exitText=s.resources->exitText();
    if(!exitText||exitText->projectRender2E0170(hook,live)!=game::OriginalUiExitTextStatus50KG::ok||
       !live.glyphs||live.images!=s.resources->images()||live.imageIndex!=binding.images[0]||
       live.materialIdentity!=binding.material.identity||live.assignedDepth!=1||live.packetColor[3]>128||live.glyphs->count>10)
        {s.failed=true;return fail(d,Error::projection,0x2E0170);}
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
    for(std::size_t q=0;q<live.glyphs->count;++q)for(const auto& glyph:live.glyphs->quads[q].vertices){
        assets::Vertex48B v{};v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=16777214.f;
        v.uv[0]=glyph.u;v.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)v.color[c]=live.packetColor[c];
        s.vertices.push_back(v);s.fog.push_back(0.f);s.w.push_back(1.f);}
    if(live.glyphs->count)s.packets.push_back({first,live.glyphs->count,m,live.imageIndex,0,
        static_cast<std::uint32_t>(s.packets.size()),live.submitPC,0,Segment::exitText});
    s.exitText=true;return true;
}
bool OriginalUiUnlockDrawContribution50PD::projectAperture(std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& rect,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.top||s.prompts||s.failed||s.finished||ordinal!=s.apertureCount||ordinal>1||
       rect.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rect.dirty3C)return fail(d,Error::sourceOrder,0x1D798C);
    game::OriginalUiOverlayQuad50KG q;q.materialName="NoTexture";q.authoredDepth=1;q.sourcePC=0x1D798C;q.sourcePacketColor={{0,0,0,63}};
    constexpr unsigned order[]={0,2,1,3};
    for(unsigned i=0;i<4;++i){const auto& v=rect.vertices[order[i]];q.vertices[i]={v.x,v.y,0,0};}
    if(!append(q,s.resources->materialIndex("ui/textures/notexture"),16777214.f,Segment::aperture,0,d)){s.failed=true;return false;}
    ++s.apertureCount;return true;
}
bool OriginalUiUnlockDrawContribution50PD::projectPrompts(std::uint32_t mask,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertureCount!=2||s.prompts||s.top||s.failed||s.finished)return fail(d,Error::sourceOrder);
    if(mask>1)return fail(d,Error::input,0x1D7A10); // UIUnlock writes only 0 (20AE5C/20B42C) or 1 (20B404)
    const auto* prompts=s.resources->prompts();if(!prompts)return fail(d,Error::resource);
    for(const auto& p:*prompts)if(mask&p.mask)
        if(!append(p.quad,p.material,16777214.f,Segment::prompts,0,d)){s.failed=true;return false;}
    s.prompts=true;return true;
}
bool OriginalUiUnlockDrawContribution50PD::projectOnTopWidgets(bool shown,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||s.top||s.failed||s.finished||(s.pageShown&&(!s.prompts||s.apertureCount!=2)))return fail(d,Error::sourceOrder);
    if(!widgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}
bool OriginalUiUnlockDrawContribution50PD::drain(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notBound);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||!s.top||s.failed||s.drained||(s.pageShown&&(!s.prompts||s.apertureCount!=2)))return fail(d,Error::unfinished);
    const auto& materials=*s.resources->materials();
    if(!s.gpu||!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,Error::publication);
    if(s.historyPoisoned||s.submissionPending||s.w.size()!=s.vertices.size()||s.fog.size()!=s.vertices.size()||
       s.frameGeneration==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::sourceOrder);
    try {
        s.pendingHistory=s.history;s.order.clear();
        for(std::size_t i=0;i<s.packets.size();++i)
            if(s.packets[i].materialIndex==unresolved||materials[s.packets[i].materialIndex].material.passes[0].enabled!=0)
                s.order.push_back(static_cast<std::uint32_t>(i));
        std::sort(s.order.begin(),s.order.end(),[&](std::uint32_t a,std::uint32_t b){
            const auto ga=s.packets[a].materialIndex==unresolved?0:s.leaf[s.packets[a].materialIndex]+1;
            const auto gb=s.packets[b].materialIndex==unresolved?0:s.leaf[s.packets[b].materialIndex]+1;
            return ga!=gb?ga<gb:s.packets[a].sourceOrdinal<s.packets[b].sourceOrdinal;});
        if(s.vertices.size()%4||s.vertices.size()/4>originalUiMaximumSourceQuads50KM)return fail(d,Error::capacity);
        const auto sourceTotal=static_cast<std::uint32_t>(s.vertices.size()/4);std::uint32_t total=0;
        for(const auto i:s.order){const auto& p=s.packets[i];
            if(!p.quadCount||p.firstQuad>=sourceTotal||p.quadCount>sourceTotal-p.firstQuad||p.quadCount>originalUiMaximumSourceQuads50KM-total)
                return fail(d,Error::capacity);
            total+=p.quadCount;}
        if(!total)return fail(d,Error::unfinished);
        s.drainedVertices.clear();s.drainedFog.clear();s.drainedW.clear();s.spans.clear();s.states.clear();
        std::uint32_t terminalMaterial=unresolved;
        for(const auto i:s.order){
            const auto& p=s.packets[i];const auto first=static_cast<std::uint32_t>(s.drainedVertices.size()/4);
            OriginalUiMaterialSpan50KG converted;bool okay=false;
            if(p.materialIndex==unresolved){
                okay=p.segment==Segment::rawBackground&&first==0&&p.quadCount==1&&
                    prepareOriginalUiUnlockBackgroundSpan50PD(*s.resources->images(),p.imageIndex,converted.span,converted.native);
            }else{
                const auto& material=materials[p.materialIndex];
                const OriginalUiMaterialSpanBinding50KG range{0,p.quadCount,p.quadCount,p.imageIndex,s.gpu->textureCount()};
                if(material.model){
                    // Rider pass dispatch with the packet's actual (frame) image.
                    assets::OriginalUiMaterialDiagnostic50KG detail;
                    okay=convertOriginalUiUnlockModelPass50PD(material.material.passes[0],*s.resources->images(),
                        material.images.empty()?unresolved:p.imageIndex,p.quadCount,converted,detail);
                }else
                okay=material.images.empty()?convertOriginalUiUntexturedMaterialSpan50KG(material.untextured,{0,p.quadCount,p.quadCount},converted):
                    (!material.policies.empty()&&convertOriginalUiMaterialSpan50KG(material.policies[0],range,converted));
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
        // Authenticated unlock.dat PageInfo BackgroundColor=0,0,0.
        const auto* page=s.resources->page()->page();
        for(unsigned c=0;c<3;++c)s.draw.draw.clearRgba[c]=page->backgroundColor[c]/255.f;
        s.draw.draw.clearRgba[3]=1;s.draw.draw.retainRaceImage=false;
        s.draw.states=s.states.data();s.draw.stateCount=static_cast<std::uint32_t>(s.states.size());
        s.draw.particleFog=s.drainedFog.data();s.draw.particleFogCount=static_cast<std::uint32_t>(s.drainedFog.size());
        s.draw.sourceRasterW=s.drainedW.data();s.draw.sourceRasterWCount=static_cast<std::uint32_t>(s.drainedW.size());
        s.draw.opaquePresentationAlphaOnly=false;
        if(terminalMaterial==unresolved)return fail(d,Error::unfinished);
        const auto& terminalBinding=materials[terminalMaterial];const auto& terminalPass=terminalBinding.material.passes[0];
        OriginalUiSourceTerminal50KN terminal{};terminal.canonicalToken=terminalBinding.registryToken;
        terminal.pass=terminalPass.pass;terminal.leaf=s.leaf[terminalMaterial];
        // One cold bank publication and one group construction for this owner.
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
const OriginalUiNativeDraw50KG* OriginalUiUnlockDrawContribution50PD::draw() const noexcept {
    return storage_&&storage_->drained&&!storage_->failed?&storage_->draw:nullptr;
}
const OriginalUiSourceFrame50KM* OriginalUiUnlockDrawContribution50PD::sourceFrame() const noexcept {
    return storage_&&storage_->drained&&!storage_->failed&&!storage_->historyPoisoned&&!storage_->submissionPending&&frame_.native_?&frame_:nullptr;
}
bool OriginalUiUnlockDrawContribution50PD::sourceTeardownPending() const noexcept {return storage_&&storage_->submittedRenderer;}
bool OriginalUiUnlockDrawContribution50PD::retireSourceRenderer(VehicleWindowGl50DS& renderer,Diagnostic& d) noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.submissionPending||(s.submittedRenderer&&s.submittedRenderer!=&renderer))return fail(d,Error::sourceOrder);
    if(s.submittedRenderer){VehicleWindowDiagnostic50DS detail;if(!renderer.retireOriginalUiSourceOwner(*this,detail))return fail(d,Error::publication);}
    s.submittedRenderer=nullptr;return true;
}
bool OriginalUiUnlockDrawContribution50PD::beginSourceSubmission(std::uint64_t generation,const VehicleWindowGl50DS& renderer) const noexcept {
    if(!storage_||!sourceFrame()||generation!=frame_.frameGeneration_)return false;auto& s=*storage_;
    if(s.submissionPending||s.historyPoisoned||(s.submittedRenderer&&s.submittedRenderer!=&renderer)||
       generation<=s.attemptedGeneration||generation<=s.acknowledgedGeneration||s.pendingHistory.size()!=s.history.size())return false;
    s.submittedRenderer=&renderer;s.attemptedGeneration=generation;s.submissionPending=true;return true;
}
void OriginalUiUnlockDrawContribution50PD::finishSourceSubmission(std::uint64_t generation,bool accepted,bool started) const noexcept {
    if(!storage_)return;auto& s=*storage_;
    if(!s.submissionPending||generation!=frame_.frameGeneration_){s.historyPoisoned=true;return;}
    s.submissionPending=false;
    if(accepted){if(s.pendingHistory.size()!=s.history.size()){s.historyPoisoned=true;return;}s.history.swap(s.pendingHistory);s.acknowledgedGeneration=generation;}
    else if(started)s.historyPoisoned=true;
}
const std::vector<OriginalUiUnlockPacket50PD>* OriginalUiUnlockDrawContribution50PD::packets() const noexcept {
    return storage_&&storage_->finished&&!storage_->failed?&storage_->packets:nullptr;
}
const OriginalUiUnlockDrawResources50PD* OriginalUiUnlockDrawContribution50PD::resources() const noexcept {return storage_?storage_->resources.get():nullptr;}
}
