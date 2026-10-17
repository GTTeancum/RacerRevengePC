#pragma once
#include "original_position_stats_resources50la.h"
#include "../../src/game/original_position_stats_hud_draw50la.h"
#include "../../platform/linux/native/original_radar_overlay_material_adapter50la.h"
#include <limits>
namespace rr::game {
class OriginalPositionStatsHud50LA final:public OriginalPositionStatsNativeOwner50LA {
    using R=OriginalPositionStatsResources50LA;
    using D=OriginalPositionStatsResourceDiagnostic50LA;
    std::weak_ptr<R> resources_;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> pixels_;
    assets::OriginalStandardMaterialCollectorReceipt50LA collector_{};
    std::array<assets::OriginalUiMaterial50KG,3> materials_{};
    std::array<std::int32_t,3> rows_{{-1,-1,-1}},leaves_{{-1,-1,-1}};
    assets::OriginalUiNativeUntexturedMaterial50KG radarPolicy_{};
    std::array<std::int16_t,2> demandTokens_{{-1,-1}};
    std::uint64_t providerRevision_=0;
    OriginalPositionStatsNativeDraw50LA draw_{};
    bool cold_=false;
    static bool fail(D& d,std::uint32_t pc)noexcept {d.error=D::Error::material;d.sourcePC=pc;return false;}
    bool demand191AF0(const original_main_entry50KG::Application& application,R& r,D& d)noexcept {
        try {
            const auto table=r.domain_->textures383400();
            assets::OriginalStandardTextureResource50LA* texture=nullptr;
            //191B00: actual ScreenTexture+270 getter2DF8A0, then191B28.
            if(!table->resolve298E60(demandTokens_[0],texture,d.material.texture)||
               texture!=r.fixed_[0].atlas)return fail(d,0x2df8a0);
            bool written=false;
            if(!texture->demandPositionStats191AF0(0x191B28u,written,d.material.texture))return fail(d,0x191b28);
            //191B3C: same Application328 registry lookup282210. Registry was
            // created by real190E64 staging and survives serial display close.
            const auto font=application.fonts328()->find282210(assets::OriginalApplicationFonts50LA::Name::eurostile13);
            if(font){
                if(font!=r.texts_[0].font4C||
                   !table->resolve298E60(demandTokens_[1],texture,d.material.texture)||
                   texture!=font->texture281F90())return fail(d,0x281f90);
                if(!texture->demandPositionStats191AF0(0x191B88u,written,d.material.texture))return fail(d,0x191b88);
            }
            //191B90's141C30 finds no children in this genuinely detached
            // owner. Native scheduling does not claim global HUD traversal.
            return true;
        }catch(...){return fail(d,0x191af0);}
    }
    bool appendTextureObject(R::Texture& t,std::uint32_t material,D& d)noexcept {
        if(!t.buffer58||t.buffer58->count!=4||t.field3C!=0||(t.flags5C&2u)||t.field30!=1||
           draw_.quadCount==draw_.maximumQuads||draw_.packetCount==draw_.maximumPackets)return fail(d,0x2df550);
        if(t.flags5C&1u){
            for(unsigned v=0;v<4;++v){auto& p=t.buffer58->vertices[v];
                p.xyz[0]=t.x10+((v&1)?t.width34:0.f);p.xyz[1]=t.y14+((v&2)?t.height38:0.f);}
            t.flags5C=std::uint8_t(t.flags5C&~1u);
        }
        // Preserve the original strip's TR-BL diagonal with native fan indices.
        auto& q=draw_.quads[draw_.quadCount];constexpr unsigned order[]={1,0,2,3};
        for(unsigned v=0;v<4;++v){const auto& p=t.buffer58->vertices[order[v]];
            if(!std::isfinite(p.xyz[0])||!std::isfinite(p.xyz[1])||p.xyz[2]!=16777214.f)return fail(d,0x2df694);
            q.vertices[v]={p.xyz[0],p.xyz[1],p.stq[0],p.stq[1]};
            for(unsigned c=0;c<4;++c){if(p.rgba[c]>255)return fail(d,0x2df550);q.packetColors[v][c]=std::uint8_t(p.rgba[c]);}}
        q.depth=16777214.f;draw_.packets[draw_.packetCount++]={draw_.quadCount++,1,material,leaves_[material]};return true;
    }
    bool appendTexture(R& r,std::size_t index,D& d)noexcept {
        return appendTextureObject(r.fixed_[index],index==6?1u:0u,d);
    }
    bool appendText(R& r,std::size_t index,D& d)noexcept {
        const auto& t=r.texts_[index];
        for(std::size_t b=0;b<t.active5E;++b){const auto& packet=t.packets64[b];const auto count=packet.activeVertices/2;
            if(!count)continue;
            if(packet.activeVertices%2||draw_.packetCount==draw_.maximumPackets||count>draw_.maximumQuads-draw_.quadCount)return fail(d,0x2e0f50);
            const auto first=draw_.quadCount;
            for(std::size_t i=0;i<count;++i){const auto& a=packet.vertices[i*2];const auto& z=packet.vertices[i*2+1];auto& q=draw_.quads[draw_.quadCount++];
                q.vertices={{{a.xyz[0],a.xyz[1],a.stq[0],a.stq[1]},
                    {z.xyz[0],a.xyz[1],z.stq[0],a.stq[1]},
                    {z.xyz[0],z.xyz[1],z.stq[0],z.stq[1]},
                    {a.xyz[0],z.xyz[1],a.stq[0],z.stq[1]}}};
                if(a.xyz[2]!=16777214.f||z.xyz[2]!=a.xyz[2])return fail(d,0x2e1240);
                q.depth=a.xyz[2];
                for(unsigned v=0;v<4;++v)for(unsigned c=0;c<4;++c){
                    // Original sprite packet color is constant across a glyph.
                    if(a.rgba[c]!=z.rgba[c]||a.rgba[c]>255)return fail(d,0x2e03f0);
                    q.packetColors[v][c]=std::uint8_t(a.rgba[c]);}
            }
            draw_.packets[draw_.packetCount++]={first,std::uint32_t(count),2,leaves_[2]};
        }return true;
    }
public:
    bool initialize(const std::shared_ptr<R>& r,std::shared_ptr<const assets::OriginalUiScreenAssets50KG> pixels,D& d)noexcept {
        d={};if(cold_||!r||!r->observationDisplay()||!pixels||!pixels->loaded()||!pixels->textures()||pixels->textures()->textureCount()!=2)return fail(d,0x190190);
        try {
            collector_=r->domain_->coldCollector14D440();const auto table=r->domain_->materials382B38();
            const auto provider=r->domain_->archiveProvider3E1A70();
            if(!provider||!provider->sealed()||!table->coldCollectorCurrent(collector_))return fail(d,0x14d440);
            providerRevision_=provider->revision();
            const char* paths[]={"ui\\textures\\stats.pst","font\\eurostileboldbold13ingame.pst"};
            for(unsigned i=0;i<2;++i){const auto member=r->overlays_->find(paths[i]);const auto* p=pixels->sourceProvenance(i);
                const auto* a=i?r->texts_[0].font4C->texture281F90():r->fixed_[0].atlas;
                const auto* tex=pixels->textures()->texture(i);
                if(!p||!a||!tex||!pixels->decodedTexture(i)||p->memberIdentity!=member.identity||p->pstOffset||p->pstBytes!=member.size||
                   !R::sameBytes(a->originalBytes(),member)||!a->sourceWidth14()||!a->sourceHeight18()||
                   tex->texture.width!=std::uint32_t(*a->sourceWidth14())||tex->texture.height!=std::uint32_t(*a->sourceHeight18())||
                   p->celIdentity!=(i?r->texts_[0].font4C->celIdentity():nullptr))return fail(d,0x2df180);
            }
            const char* demandNames[]={"ui\\textures\\stats","font\\eurostileboldbold13ingame"};
            for(unsigned i=0;i<2;++i){assets::OriginalStandardTextureResource50LA* texture=nullptr;
                if(!r->domain_->textures383400()->find(demandNames[i],demandTokens_[i])||
                   !r->domain_->textures383400()->resolve298E60(demandTokens_[i],texture,d.material.texture)||
                   texture!=(i?r->texts_[0].font4C->texture281F90():r->fixed_[0].atlas))return fail(d,0x191af0);
            }
            rows_={{r->fixed_[0].primitive54->material.row,r->fixed_[6].primitive54->material.row,r->texts_[0].primitives4[0]->material.row}};
            const char* names[]={"ui\\textures\\stats.psm","ui\\textures\\maptop.psm","font\\eurostileboldbold13ingame.psm"};
            for(unsigned i=0;i<3;++i){assets::OriginalStandardMaterialMapBindingView50LA map;
                if(!table->prepareColdScreenPrototypeMap249610(collector_,rows_[i],0,0,map,d.material)||
                   map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||map.origin.finalLeaf<0||
                   map.texture!=(i==2?r->texts_[0].font4C->texture281F90():r->fixed_[i==1?6:0].atlas))return fail(d,0x249610);
                assets::OriginalUiMaterialDiagnostic50KG md;
                if(!assets::readOriginalUiMaterial50KG(*r->overlays_,names[i],r->context_->materialApplication()->templates(),
                    assets::OriginalUiTemplate50KG::screenMt,r->context_->materialRuntime(),materials_[i],md)||materials_[i].passes.size()!=1)return fail(d,0x247110);
                const assets::OriginalStandardMaterialObject50LA* canonical=nullptr;
                assets::OriginalStandardMaterialPassView50LA pass;
                if(!table->resolve298E60(rows_[i],canonical,d.material)||!canonical||!canonical->pass4C(0,pass)||
                   !pass.state30||pass.state30->contextCount()!=1)return fail(d,0x298e60);
                const auto& parsed=materials_[i].passes[0];
                for(std::size_t f=0;f<assets::originalUiGsFieldCount50KG;++f)
                    if(parsed.gs.fields[f].value!=map.contextAfter249610.fields[f].value||parsed.gs.fields[f].value!=pass.state30->fields[f].value)return fail(d,0x2476a0);
                const std::int32_t sampler[]={parsed.lodCalculationMethod,parsed.maximumMipLevel,
                    parsed.magFilter,parsed.minFilter,parsed.lodL,parsed.lodK};
                for(unsigned f=0;f<6;++f)if(sampler[f]!=pass.state30->sampler[f].value)return fail(d,0x247f38);
                leaves_[i]=map.origin.finalLeaf;
            }
            assets::OriginalRadarOverlayNativePass50LA radar;
            if(!r->primitives_->elements_[0].primitives30[0]->material.prepareNativeZeroMap(collector_,radar,d.material)||!radar.enabled)return fail(d,0x2e21d0);
            radarPolicy_=radar.policy;pixels_=std::move(pixels);resources_=r;cold_=true;return true;
        }catch(...){return fail(d,0x190190);}
    }
    bool nativeCpuResourcesPrepared()const noexcept override {
        const auto r=resources_.lock();if(!cold_||!r||!r->observationDisplay()||!r->context_||!r->context_->live())return false;
        const auto provider=r->domain_->archiveProvider3E1A70();
        return provider&&provider->sealed()&&provider->revision()==providerRevision_&&r->domain_->materials382B38()->coldCollectorCurrent(collector_);
    }
    const assets::OriginalUiScreenAssets50KG* nativePixels()const noexcept override{return pixels_.get();}
    const assets::OriginalUiMaterial50KG* nativeMaterial(std::size_t i)const noexcept override{return nativeCpuResourcesPrepared()&&i<3?&materials_[i]:nullptr;}
    std::uint32_t nativeImage(std::size_t i)const noexcept override{return i==2?1:0;}
    const assets::OriginalUiNativeUntexturedMaterial50KG* nativeRadarPolicy()const noexcept override{return nativeCpuResourcesPrepared()?&radarPolicy_:nullptr;}
    const OriginalPositionStatsNativeDraw50LA& nativeDraw()const noexcept override{return draw_;}
    void invalidateNativeDraw()noexcept{draw_.prepared=false;}
    bool prepareNativeDisplay(const original_main_entry50KG::Application& application,
        const original_camera_session50KX::Session& camera,const native_ai50IV::Session& manager,D& d)noexcept {
        d={};draw_.prepared=false;draw_.quadCount=draw_.packetCount=0;
        const auto r=resources_.lock();OriginalPositionStatsNumbers50LA observation;
        if(!nativeCpuResourcesPrepared()||!r||!application.capturePositionStatsNumbers50LA(*r,camera,manager,observation,d)||
           !application.preparePositionStatsText50LA(*r,camera,manager,d)||
           !application.preparePositionStatsRadar50LA(*r,camera,manager,d))return false;
        const auto& viewport=camera.viewport();
        if(!viewport.prepared||!viewport.matricesReady||viewport.sessionGeneration!=observation.sessionGeneration||
           viewport.width1E8!=640||viewport.height1EC!=448||viewport.x1E0||viewport.y1E4||
           !observation.cameraPublication||observation.cameraPublication<draw_.cameraPublication||
           draw_.serial==std::numeric_limits<std::uint64_t>::max())return fail(d,0x191be0);
        draw_.viewport=&viewport;draw_.cameraPublication=observation.cameraPublication;++draw_.serial;
        bool visible=false;position_stats_detail50LA::Visibility preference;
        if(!position_stats_detail50LA::ownLeafVisible191BE0(observation.viewIndex,observation.raceFlags1B0,
               observation.raceEntityFlags29,observation.uiFlags1E4,visible)||
           !position_stats_detail50LA::preferenceVisibility17FB90(observation.preference10F9C,
               observation.viewState40,observation.subtype11258,preference))return fail(d,0x191be0);
        if(!visible){draw_.prepared=true;return true;}
        // Revalidate the originating retained references against the current
        // canonical FINAL leaves at every submitted native frame.
        const auto table=r->domain_->materials382B38();
        for(unsigned i=0;i<3;++i){assets::OriginalStandardMaterialMapBindingView50LA map;
            if(!table->prepareColdScreenPrototypeMap249610(collector_,rows_[i],0,0,map,d.material)||
               map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||map.origin.finalLeaf!=leaves_[i]||
               map.texture!=(i==2?r->texts_[0].font4C->texture281F90():r->fixed_[i==1?6:0].atlas))return fail(d,0x249610);
            for(std::size_t f=0;f<assets::originalUiGsFieldCount50KG;++f)
                if(materials_[i].passes[0].gs.fields[f].value!=map.contextAfter249610.fields[f].value)return fail(d,0x249610);
        }
        // Native eligible-display service, like TimeStats194E20 and Damage18F470.
        // Preserve stats-before-font source writes and every committed prefix.
        if(!demand191AF0(application,*r,d))return false;
        if(preference.position)for(unsigned i=0;i<5;++i)if(!appendTexture(*r,i,d))return false;
        if(preference.map&&!appendTexture(*r,5,d))return false;
        if(preference.countCD0&&(!appendTexture(*r,7,d)||!appendTexture(*r,8,d)))return false;
        if(preference.map){
            for(std::size_t e=0;e<r->radarRoute_.completedElements;++e){auto& element=r->primitives_->elements_[e];
                for(std::size_t p=0;p<element.capacity4C;++p){const auto& packet=element.packets44[p];
                    assets::OriginalRadarOverlayNativePass50LA material;
                    if(!element.primitives30[p]->material.prepareNativeZeroMap(collector_,material,d.material))return fail(d,0x2e21d0);
                    if(!material.enabled)continue;
                    if(packet.activeCount<4)continue;
                    const auto count=packet.activeCount/2-1;
                    if(packet.activeCount%2||count>draw_.maximumQuads-draw_.quadCount||draw_.packetCount==draw_.maximumPackets)return fail(d,0x2e21d0);
                    const auto first=draw_.quadCount;
                    for(std::size_t v=0;v+3<packet.activeCount;v+=2){auto& quad=draw_.quads[draw_.quadCount++];
                        const std::size_t order[]={v+1,v,v+2,v+3};
                        unsigned firstCorner=0;
                        if(v){
                            // The preceding strip quad already validated these shared endpoints.
                            const auto& previous=draw_.quads[draw_.quadCount-2];
                            quad.vertices[0]=previous.vertices[3];quad.vertices[1]=previous.vertices[2];
                            quad.packetColors[0]=previous.packetColors[3];quad.packetColors[1]=previous.packetColors[2];
                            firstCorner=2;
                        }
                        for(unsigned corner=firstCorner;corner<4;++corner){const auto& source=packet.vertices[order[corner]];assets::Vertex48B vertex;
                            if(!platform::convertOriginalRadarOverlayDepthIndependentVertex50LA(material,source.stq,source.rgba,vertex))return fail(d,0x2e21d0);
                            quad.vertices[corner]={vertex.position[0],vertex.position[1],0,0};
                            for(unsigned c=0;c<4;++c)quad.packetColors[corner][c]=vertex.color[c];}
                        quad.depth=0;
                    }
                    radarPolicy_=material.policy;
                    draw_.packets[draw_.packetCount++]={first,std::uint32_t(count),3,material.origin.finalLeaf};
                }
            }
            for(std::size_t marker=0;marker<r->radarMarkerCount_;++marker){
                const auto index=r->radarMarkerIndices_[marker];
                if(index>=r->map_.size()||!appendTextureObject(r->map_[index],1,d))return false;
            }
            if(!appendTexture(*r,6,d))return false;
        }
        if(preference.position)for(auto i:{0u,2u,1u})if(!appendText(*r,i,d))return false;
        if(preference.countCD0&&(!appendText(*r,3,d)||!appendText(*r,4,d)))return false;
        draw_.prepared=true;return true;
    }
};
}
