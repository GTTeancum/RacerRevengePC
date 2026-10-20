#pragma once
#include "tb_original_world50lm.h"
#include "../../src/game/original_tb_shield_uv.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>
#include <exception>
#include <memory>
#include <string>

// Actual TB shield prototype and forty authenticated static placements.
namespace tb_shield_uv_fixture {
constexpr const char* materialKey="7b96457eb7f6a8d0e9e16834ec7fd0f1d658e641fb2e083be713122318febd89";
struct Diagnostic { std::uint32_t sourceSpanIndex=0xffffffffu; std::string message; };
struct Binding {
    std::uint32_t sourceSpanIndex=0;
    track_materials50fa_fixture::Span source{};
};
class Owner final {
    struct Storage {
        std::shared_ptr<const tb_original_world50LM::Owner> retainedWorld;
        rr::game::OriginalTbShieldUv prototype{};
        std::array<Binding,40> bindings{};
    };
    std::unique_ptr<Storage> storage_;
    bool attempted_=false;
    const Binding* binding(std::uint32_t span)const noexcept {
        if(storage_)for(const auto& b:storage_->bindings)if(b.sourceSpanIndex==span)return &b;
        return nullptr;
    }
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    const std::array<Binding,40>* bindings()const noexcept{return storage_?&storage_->bindings:nullptr;}
    const rr::game::OriginalTbShieldUv* state(std::uint32_t span)const noexcept {
        return binding(span)?&storage_->prototype:nullptr;
    }
    const rr::game::OriginalTbShieldUvRows* rows(std::uint32_t span)const noexcept {
        const auto* s=state(span);return s?&s->matrix:nullptr;
    }
    bool service(std::uint32_t span,std::uint32_t applicationTimestamp,bool applicationPaused,
        bool callbackAllowsUpdate=true)noexcept {
        if(!binding(span))return false;
        return rr::game::serviceOriginalTbShieldUv(storage_->prototype,
            applicationTimestamp,applicationPaused,callbackAllowsUpdate);
    }
    // This is a pure cold join after selectTb alpha lowering. Only the existing
    // single textured, unprojected UV path is used; no renderer guard changes.
    bool bindRenderSpan(std::uint32_t span,rr::platform::VehicleWindowTrackSpan50DS& output)const noexcept {
        const auto* b=binding(span);if(!b)return false;const auto& s=b->source;
        const auto& m=output.material;
        if(output.indexStart!=s.indexStart||output.indexCount!=s.indexCount||
           output.vertexStart!=s.vertexStart||output.vertexCount!=s.vertexCount||
           output.sourcePass!=1||output.sourcePassCount!=1||output.projectionPass||
           m.textureIndex!=s.textureIndex||m.mode!=rr::platform::VehicleWindowMaterialMode50DS::requiredTexture||
           m.blend!=rr::platform::VehicleWindowBlend50DS::additive||m.depthWrite||!m.depthEqual||
           m.alphaScale!=(255.f/128.f)*(255.f/128.f)||m.fixedBlendAlpha||m.unlit||m.sourcePrelit||
           m.sourceDiffuseCompressed50LA||m.sourceSpecularPair50LA||
           !output.sourceAlphaTest||output.sourceAlphaReference!=0||output.discardZeroAlpha||
           !output.sourceFogKnown||!output.sourceFogEnabled||
           output.sourceUvAnimation||output.sourceUvPolicy!=rr::platform::NativeTrackUvPolicy50DS::legacySinglePass)return false;
        output.sourceUvAnimation=true;return true;
    }
    // Typed synchronous source borrow. Geometry/image association is copied;
    // no caller-made selection or source storage pointer survives publication.
    // One attempt; preserve reached failure and already published state.
    bool load(std::shared_ptr<const tb_original_world50LM::Owner> retainedWorld,Diagnostic& d)noexcept {
        namespace shared=pod_materials50ed_fixture::detail;
        using shared::need;
        try {
            d={};need(!attempted_,"TB UV preparation is one-attempt");attempted_=true;
            need(retainedWorld&&retainedWorld->loaded(),"TB UV requires retained world");
            const auto& world=*retainedWorld;
            need(world.loaded()&&world.archive()&&world.spans()&&world.selections(),"TB UV requires its loaded original world");
            need(world.sourceOwner()&&world.sourceOwner()->originalTob().size()==106139&&
                shared::sha256(world.sourceOwner()->originalTob())=="12c908874736ea91dcd6d3f8af97413a0cfe3bcd87cfce0798e8ff86fe5fb0bc","TB UV own original TOB identity");
            const auto& spans=*world.spans();const auto& selections=*world.selections();
            need(spans.size()==1376&&selections.size()==613,"TB UV actual world extent differs");
            auto next=std::make_unique<Storage>();std::size_t count=0;
            next->retainedWorld=std::move(retainedWorld);
            std::uint32_t selected=0xffffffffu,totalVertices=0,totalIndices=0;
            for(std::uint32_t i=0;i<spans.size();++i) {
                d.sourceSpanIndex=i;const auto& span=spans[i];
                need(span.materialSelection<selections.size(),"TB UV source selection extent");
                const auto& s=selections[span.materialSelection];
                const auto name=shared::normalized(s.materialName);
                if(name!="TC_SHIELD")continue;
                need(s.track==track_source_fixture::Track::tb&&s.materialKey==materialKey&&s.assetIdentity==0x5442&&
                    s.pass==1&&s.passCount==1&&s.slot==1&&
                    shared::normalized(s.program)=="DIFFUSETEXTRANSCOMPRESSED"&&
                    shared::normalized(s.textureName)=="TC_SHIELD"&&!s.projectedPrelit&&!s.staticAnimationFrame&&
                    !s.clampU&&!s.clampV&&s.textureIndex!=track_materials50fa_fixture::unresolved&&
                    shared::normalized(s.decodedSourceMember)=="TEXTURES/TC_SHIELD.PST"&&
                    s.decodedSourceSha256=="5dafb1b7425f3367997b408ae23e22d59bc40e32ca3b1ad6b1aa1c3ce807cee3"&&
                    span.sourcePass==1&&span.sourcePassCount==1&&!span.projectionPass&&span.textureIndex==s.textureIndex&&
                    (span.vertexCount==4&&span.indexCount==6),
                    "TB UV exact original material/pass/image/geometry identity differs");
                need(selected==0xffffffffu||selected==span.materialSelection,"TB UV duplicate material prototype");
                selected=span.materialSelection;
                need(count<next->bindings.size(),"TB UV original binding extent exceeded");
                for(std::size_t j=0;j<count;++j)need(next->bindings[j].source.worldInstanceIndex!=span.worldInstanceIndex,
                    "TB UV requires forty distinct original static placements");
                next->bindings[count++]={i,span};totalVertices+=span.vertexCount;totalIndices+=span.indexCount;
            }
            need(count==40&&selected!=0xffffffffu&&totalVertices==160&&totalIndices==240,
                "TB UV requires one shield prototype/forty distinct shield placements");
            storage_=std::move(next);d={};return true;
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
         catch(...){try{d.message="TB UV source association failed";}catch(...){}return false;}
    }
};
}
