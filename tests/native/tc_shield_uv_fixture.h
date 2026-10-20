#pragma once
#include "tc_original_world50lm.h"
#include "../../src/game/original_tc_shield_uv.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>
#include <exception>
#include <memory>
#include <string>

// Actual TC shield material prototype, shared by its nine static placements.
// TC's sole active dynamic registration is the separately owned tc_gate PSG.
// TCSUN omits VertexAnimation and the original factory returns a null leaf;
// its authored UV velocities do not install a service or create a prototype.
namespace tc_shield_uv_fixture {
constexpr const char* materialKey="7b96457eb7f6a8d0e9e16834ec7fd0f1d658e641fb2e083be713122318febd89";
struct Diagnostic { std::uint32_t sourceSpanIndex=0xffffffffu; std::string message; };
struct Binding {
    std::uint32_t sourceSpanIndex=0;
    track_materials50fa_fixture::Span source{};
};
class Owner final {
    struct Storage {
        rr::game::OriginalTcShieldUv prototype{};
        std::array<Binding,9> bindings{};
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
    const std::array<Binding,9>* bindings()const noexcept{return storage_?&storage_->bindings:nullptr;}
    const rr::game::OriginalTcShieldUv* state(std::uint32_t span)const noexcept {
        return binding(span)?&storage_->prototype:nullptr;
    }
    const rr::game::OriginalTcShieldUvRows* rows(std::uint32_t span)const noexcept {
        const auto* s=state(span);return s?&s->matrix:nullptr;
    }
    bool service(std::uint32_t span,std::uint32_t applicationTimestamp,bool applicationPaused,
        bool callbackAllowsUpdate=true)noexcept {
        if(!binding(span))return false;
        return rr::game::serviceOriginalTcShieldUv(storage_->prototype,
            applicationTimestamp,applicationPaused,callbackAllowsUpdate);
    }
    // This is a pure cold join after selectTc alpha lowering. Only the existing
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
    bool load(const tc_original_world50LM::Owner& world,Diagnostic& d)noexcept {
        namespace shared=pod_materials50ed_fixture::detail;
        using shared::need;
        try {
            d={};need(!attempted_,"TC UV preparation is one-attempt");attempted_=true;
            need(world.loaded()&&world.archive()&&world.spans()&&world.selections(),"TC UV requires its loaded original world");
            rr::assets::DecodeError48C decode;std::vector<std::uint8_t> tob;
            need(rr::assets::extractResMember48C(*world.archive(),"tracks\\tc.tob",tob,decode)&&
                tob.size()==25512&&shared::sha256(tob)=="e080f96e2657e1331ac1f829faa880a5e984ebf1c5fab393daa6539534dd0573",
                "TC UV requires its exact sole-gate-animation TOB");
            const auto& spans=*world.spans();const auto& selections=*world.selections();
            need(spans.size()==738&&selections.size()==69,"TC UV actual world extent differs");
            auto next=std::make_unique<Storage>();std::size_t count=0,sunCount=0;
            std::uint32_t selected=0xffffffffu,totalVertices=0,totalIndices=0;
            for(std::uint32_t i=0;i<spans.size();++i) {
                d.sourceSpanIndex=i;const auto& span=spans[i];
                need(span.materialSelection<selections.size(),"TC UV source selection extent");
                const auto& s=selections[span.materialSelection];
                const auto name=shared::normalized(s.materialName);
                if(name=="TCSUN"){++sunCount;continue;}
                if(name!="TC_SHIELD")continue;
                need(s.track==track_source_fixture::Track::tc&&s.materialKey==materialKey&&s.assetIdentity==0x5443&&
                    s.pass==1&&s.passCount==1&&s.slot==1&&
                    shared::normalized(s.program)=="DIFFUSETEXTRANSCOMPRESSED"&&
                    shared::normalized(s.textureName)=="TC_SHIELD"&&!s.projectedPrelit&&!s.staticAnimationFrame&&
                    !s.clampU&&!s.clampV&&s.textureIndex!=track_materials50fa_fixture::unresolved&&
                    shared::normalized(s.decodedSourceMember)=="TEXTURES/TC_SHIELD.PST"&&
                    s.decodedSourceSha256=="5dafb1b7425f3367997b408ae23e22d59bc40e32ca3b1ad6b1aa1c3ce807cee3"&&
                    span.sourcePass==1&&span.sourcePassCount==1&&!span.projectionPass&&span.textureIndex==s.textureIndex&&
                    ((span.vertexCount==4&&span.indexCount==6)||(span.vertexCount==12&&span.indexCount==18)),
                    "TC UV exact original material/pass/image/geometry identity differs");
                need(selected==0xffffffffu||selected==span.materialSelection,"TC UV duplicate material prototype");
                selected=span.materialSelection;
                need(count<next->bindings.size(),"TC UV original binding extent exceeded");
                for(std::size_t j=0;j<count;++j)need(next->bindings[j].source.worldInstanceIndex!=span.worldInstanceIndex,
                    "TC UV requires nine distinct original static placements");
                next->bindings[count++]={i,span};totalVertices+=span.vertexCount;totalIndices+=span.indexCount;
            }
            need(count==9&&sunCount==9&&selected!=0xffffffffu&&totalVertices==60&&totalIndices==90,
                "TC UV requires one shield prototype/nine shield placements and nine static sun placements");
            storage_=std::move(next);d={};return true;
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
         catch(...){try{d.message="TC UV source association failed";}catch(...){}return false;}
    }
};
}
