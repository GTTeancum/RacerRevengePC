#pragma once
#include "original_pause_native_resources50la.h"
#include "original_pause_layout50la.h"
#include "original_pause_polygon_geometry50la.h"
#include "original_camera_start_effects.h"
#include "../assets/material_texture_assets50ea.h"
#include <limits>

namespace rr::game {
// Concrete native batch for genuine derived1BB010 or ordinary single-view
// outer draw-tail commands, all sharing one native depth plane.
// Outer1B9220 scheduler and Entity admission remain separate; the concrete
// observation can capture an actual temporary115A60 viewport scope.
class OriginalPauseNativeDraw50LA final {
    friend class original_pause_observation50LA::Owner;
public:
    static constexpr std::size_t maximumMaterials=144,maximumQuads=4096,maximumPackets=144;
    using Resources=OriginalPauseNativeResources50LA;
    struct Quad {std::array<assets::Vertex48B,4> vertices;};
    struct Packet {std::uint32_t first=0,count=0,material=0;std::int32_t leaf=-1;};
    struct Frame {
        const OriginalCameraViewportFields* viewport=nullptr;
        std::uint64_t serial=0,cameraPublication=0;
        std::array<Quad,maximumQuads> quads;
        std::array<Packet,maximumPackets> packets;
        std::uint32_t quadCount=0,packetCount=0;bool prepared=false,outerTail=false;
    };
private:
    std::shared_ptr<OriginalPauseStorage50LA> owner_;
    Resources resources_;
    assets::MaterialTextureAssets50EA pixels_;
    std::array<Resources::Projection,36> policies_;
    std::array<std::uint32_t,maximumMaterials> images_{};
    std::array<std::vector<std::uint32_t>,maximumMaterials> frameImages_;
    std::unique_ptr<Frame> frame_;
    OriginalCameraViewportFields rasterViewport_;
    const OriginalCameraViewportFields* sourceViewport_=nullptr;
    bool ready_=false;
    static bool samePolicy(const Resources::Pass& a,const Resources::Pass& b)noexcept {
        if(a.enabled!=b.enabled||a.hasTexture!=b.hasTexture||a.frames!=b.frames)return false;
        if(!a.enabled)return true;
        if(a.hasTexture&&a.textured.maximumVertexAlpha!=b.textured.maximumVertexAlpha)return false;
        const auto& x=a.canonical;const auto& y=b.canonical;
        if(x.origin.finalLeaf!=y.origin.finalLeaf||x.final.maps!=y.final.maps||x.final.uv!=y.final.uv)return false;
        const auto& xs=a.hasTexture?x.maps[0].contextAfter249610:x.final.state;
        const auto& ys=b.hasTexture?y.maps[0].contextAfter249610:y.final.state;
        for(std::size_t i=0;i<assets::originalUiGsFieldCount50KG;++i)
            if(xs.fields[i].value!=ys.fields[i].value)return false;
        for(std::size_t i=0;i<6;++i)if(xs.sampler[i].value!=ys.sampler[i].value)return false;
        return true;
    }
    bool append(std::size_t reference,const Quad* quads,std::size_t count,Frame& f,Resources::Diagnostic& d){
        if(!count)return true;
        Resources::Projection fresh;
        if(!resources_.prepareEmission(reference,fresh,d)||fresh.passes.size()!=policies_[reference].passes.size())return false;
        for(std::size_t p=0;p<fresh.passes.size();++p){
            const auto& pass=fresh.passes[p];
            if(!samePolicy(pass,policies_[reference].passes[p]))return false;
            if(!pass.enabled)continue;
            if(pass.hasTexture){
                const auto material=reference*4+p;
                bool found=false;
                for(std::size_t i=0;i<pass.frames.size();++i)if(pass.frames[i]==pass.image){
                    images_[material]=frameImages_[material][i];found=true;break;
                }
                if(!found)return false;
            }
            if(f.packetCount>=maximumPackets||count>maximumQuads-f.quadCount)return false;
            const auto first=f.quadCount;
            for(std::size_t q=0;q<count;++q){
                for(const auto& v:quads[q].vertices){
                    if(v.position[2]!=16777214.f)return false;
                    for(float x:v.position)if(!std::isfinite(x))return false;
                    for(float x:v.uv)if(!std::isfinite(x))return false;
                    if(v.color[3]>(pass.hasTexture?pass.textured.maximumVertexAlpha:pass.untextured.maximumVertexAlpha))return false;
                }
                f.quads[f.quadCount++]=quads[q];
            }
            f.packets[f.packetCount++]={first,static_cast<std::uint32_t>(count),
                static_cast<std::uint32_t>(reference*4+p),pass.canonical.origin.finalLeaf};
        }
        return true;
    }
    static bool vertex(const OriginalPauseStorage50LA::PacketVertex& in,assets::Vertex48B& out)noexcept {
        out={};for(std::size_t i=0;i<3;++i)out.position[i]=in.xyz[i];
        out.uv[0]=in.stq[0];out.uv[1]=in.stq[1];
        for(std::size_t i=0;i<4;++i){if(in.rgba[i]>255)return false;out.color[i]=static_cast<std::uint8_t>(in.rgba[i]);}
        return true;
    }
public:
    OriginalPauseNativeDraw50LA()=default;
    OriginalPauseNativeDraw50LA(const OriginalPauseNativeDraw50LA&)=delete;
    OriginalPauseNativeDraw50LA& operator=(const OriginalPauseNativeDraw50LA&)=delete;
    // No public close: a renderer's strong owner survives through GPU retirement.
    bool initialize(std::shared_ptr<OriginalPauseStorage50LA> owner,Resources::Diagnostic& d)noexcept {
        if(owner_||!owner)return false;
        owner_=std::move(owner);
        if(!resources_.initialize(owner_,d))return false;
        try {
            std::vector<assets::MaterialTextureAssetInput50EA> inputs;
            images_.fill(assets::unresolvedMaterialTexture50EA);
            for(std::size_t r=0;r<36;++r){
                if(!resources_.prepare(r,policies_[r],d)||policies_[r].passes.size()>4)return false;
                for(std::size_t p=0;p<policies_[r].passes.size();++p){const auto& pass=policies_[r].passes[p];
                    if(!pass.enabled||!pass.hasTexture)continue;
                    for(const auto& image:pass.frames){
                        if(inputs.size()>=maximumMaterials)return false;
                        const auto index=static_cast<std::uint32_t>(inputs.size());
                        frameImages_[r*4+p].push_back(index);
                        if(image==pass.image)images_[r*4+p]=index;
                        // Immutable native bank ordinal; original child identity
                        // and exact PST extent remain retained in pass.frames.
                        inputs.push_back({inputs.size()+1,&image->decoded.base_rrtex});
                    }
                }
            }
            assets::MaterialTextureAssetsInput50EA input;
            input.textures=inputs.data();input.textureCount=static_cast<std::uint32_t>(inputs.size());
            input.budgets={maximumMaterials,0,assets::maxMaterialTextureSourceBytes50EA,
                assets::maxMaterialTextureRGBABytes50EA,assets::max_texture_dimension};
            assets::MaterialTextureAssetDiagnostic50EA pd;
            if(!pixels_.load(input,pd))return false;
            frame_=std::make_unique<Frame>();ready_=true;return true;
        }catch(...){return false;}
    }
    bool current()const noexcept{return ready_&&resources_.current();}
    const assets::MaterialTextureAssets50EA* pixels()const noexcept{return current()?&pixels_:nullptr;}
    const Resources::Pass* material(std::size_t i)const noexcept {
        return current()&&i<maximumMaterials&&i%4<policies_[i/4].passes.size()?&policies_[i/4].passes[i%4]:nullptr;
    }
    std::uint32_t image(std::size_t i)const noexcept{return i<maximumMaterials?images_[i]:assets::unresolvedMaterialTexture50EA;}
    const std::vector<std::uint32_t>* frameImages(std::size_t i)const noexcept {
        return current()&&i<maximumMaterials?&frameImages_[i]:nullptr;
    }
    const Frame* frame()const noexcept{return current()?frame_.get():nullptr;}
    bool prepareDerived(const OriginalPauseLayout50LA::Commands& commands,
        const OriginalCameraViewportFields& viewport,std::uint64_t publication,Resources::Diagnostic& d)noexcept {
        if(commands.outer()){if(frame_)frame_->prepared=false;return false;}
        return prepareCommands(commands,viewport,publication,d);
    }
    bool prepareOuter(const OriginalPauseLayout50LA::Commands& commands,
        const OriginalCameraViewportFields& viewport,std::uint64_t publication,Resources::Diagnostic& d)noexcept {
        if(!commands.outer()||commands.viewportOverride()){if(frame_)frame_->prepared=false;return false;}
        return prepareCommands(commands,viewport,publication,d);
    }
private:
    bool prepareCommands(const OriginalPauseLayout50LA::Commands& commands,
        const OriginalCameraViewportFields& viewport,std::uint64_t publication,Resources::Diagnostic& d)noexcept {
        if(frame_)frame_->prepared=false;
        if(!current()||commands.owner()!=owner_.get()||!publication||!viewport.prepared||!viewport.matricesReady||
           !viewport.sessionGeneration||!viewport.sceneKey||viewport.x1E0||viewport.y1E4||
           viewport.width1E8!=640||viewport.height1EC!=448||
           viewport.depthNear1F0!=0.f||viewport.depthFar1F4!=1.f||
           frame_->serial==std::numeric_limits<std::uint64_t>::max()||publication<frame_->cameraPublication||
           (sourceViewport_&&(sourceViewport_!=&viewport||rasterViewport_.sceneKey!=viewport.sceneKey||
               rasterViewport_.sessionGeneration!=viewport.sessionGeneration))||
           !owner_->entity_||owner_->entity_->component.sessionGeneration!=viewport.sessionGeneration)return false;
        try {
            auto next=std::make_unique<Frame>();next->cameraPublication=publication;
            next->serial=frame_->serial+1;next->outerTail=commands.outer();
            for(std::size_t c=0;c<commands.count();++c){const auto* command=commands.at(c);if(!command)return false;
                if(command->kind==OriginalPauseLayout50LA::Kind::polygon){
                    OriginalPausePolygonGeometry50LA::Projection source;OriginalPausePolygonGeometry50LA::Diagnostic pd;
                    if(!OriginalPausePolygonGeometry50LA::regenerate2E21D0(*owner_,command->object,0xffffffu,source,pd))return false;
                    for(const auto& pass:policies_[source.reference].passes)if(pass.enabled&&pass.hasTexture)return false;
                    Quad q;constexpr std::size_t order[]{1,0,2,3};
                    for(std::size_t v=0;v<4;++v){auto in=source.packet.vertices[order[v]];
                        in.xyz=in.stq;in.stq={};
                        if(!vertex(in,q.vertices[v]))return false;
                    }
                    if(!append(source.reference,&q,1,*next,d))return false;
                }else if(command->kind==OriginalPauseLayout50LA::Kind::texture){
                    OriginalPauseTextureGeometry50LA::Projection source;OriginalPauseTextureGeometry50LA::Diagnostic td;
                    if(!OriginalPauseTextureGeometry50LA::regenerate2DF550(*owner_,command->object,source,td))return false;
                    Quad q;constexpr std::size_t order[]{1,0,2,3};
                    for(std::size_t v=0;v<4;++v)if(!vertex(source.packet.vertices[order[v]],q.vertices[v]))return false;
                    if(!append(source.reference,&q,1,*next,d))return false;
                }else{
                    if(command->object>owner_->derivedTexts_.size())return false;
                    OriginalPauseTextGeometry50LA::Diagnostic td;
                    if(!OriginalPauseTextGeometry50LA::regenerate2E0170(*owner_,command->object,0xffffffu,td))return false;
                    const auto& text=command->object?owner_->derivedTexts_[command->object-1]:owner_->baseText_;
                    for(std::size_t batch=0;batch<text.active5E;++batch){
                        if(batch>=text.packetCount5D||!text.primitives4[batch]||!text.packets64[batch].vertices||
                           text.primitives4[batch]->packet10!=&text.packets64[batch])return false;
                        const auto count=OriginalPauseTextGeometry50LA::activeVertices(text,batch)/2;
                        if(count>36||count*2>text.packets64[batch].count)return false;
                        std::array<Quad,36> quads;
                        for(std::size_t q=0;q<count;++q){const auto& a=text.packets64[batch].vertices[q*2];
                            const auto& b=text.packets64[batch].vertices[q*2+1];
                            if(a.rgba!=b.rgba||a.xyz[2]!=b.xyz[2])return false;
                            auto& out=quads[q];
                            if(!vertex(a,out.vertices[0])||!vertex(b,out.vertices[2]))return false;
                            out.vertices[1]=out.vertices[0];out.vertices[1].position[0]=b.xyz[0];out.vertices[1].uv[0]=b.stq[0];
                            out.vertices[3]=out.vertices[2];out.vertices[3].position[0]=a.xyz[0];out.vertices[3].uv[0]=a.stq[0];
                        }
                        if(!append(text.primitives4[batch]->reference,quads.data(),count,*next,d))return false;
                    }
                }
            }
            // Copy the actual scoped viewport before115A60 restores it. The stable
            // owned snapshot survives deferred GPU submission; it is not a new camera.
            rasterViewport_=viewport;sourceViewport_=&viewport;next->viewport=&rasterViewport_;
            next->prepared=true;frame_=std::move(next);return true;
        }catch(...){return false;}
    }
};
}
