#pragma once
#include "sc_original_world50lm.h"
#include "../../src/game/original_sc_lava_uv.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>
#include <exception>
#include <memory>
#include <string>

// Native render-boundary projection of the exact SC lava material. Two pass
// prototypes remain separate even though both sample the same original image.
// The authenticated SC TOB has no model-animation registrations. This owner
// neither invents clones from VertexAnimation=unique nor exposes a fake table.
namespace sc_lava_uv_fixture {
constexpr const char* materialKey="43cf6e8c8a2b26c5db88f99b2f65247a76d09f01875978d2c638e39af305ad33";
struct Diagnostic { std::uint32_t sourceSpanIndex=0xffffffffu; std::string message; };
struct Binding {
    std::uint32_t sourceSpanIndex=0;
    track_materials50fa_fixture::Span source{};
};
class Owner final {
    struct Storage {
        std::array<rr::game::OriginalScLavaUv,2> prototypes{};
        std::array<Binding,8> bindings{};
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
    const std::array<Binding,8>* bindings()const noexcept{return storage_?&storage_->bindings:nullptr;}
    const rr::game::OriginalScLavaUv* state(std::uint32_t span)const noexcept {
        const auto* b=binding(span);return b?&storage_->prototypes[b->source.sourcePass-1]:nullptr;
    }
    const rr::game::OriginalScLavaUvRows* rows(std::uint32_t span)const noexcept {
        const auto* s=state(span);return s?&s->matrix:nullptr;
    }
    bool service(std::uint32_t span,std::uint32_t applicationTimestamp,bool applicationPaused,
        bool callbackAllowsUpdate=true)noexcept {
        const auto* b=binding(span);if(!b)return false;
        return rr::game::serviceOriginalScLavaUv(storage_->prototypes[b->source.sourcePass-1],
            b->source.sourcePass,applicationTimestamp,applicationPaused,callbackAllowsUpdate);
    }
    // Cold lowering consumes the exact source range and texture. The renderer
    // subsequently checks both neighboring policies before any GL publication.
    bool bindRenderSpan(std::uint32_t span,rr::platform::VehicleWindowTrackSpan50DS& output)const noexcept {
        const auto* b=binding(span);if(!b)return false;const auto& s=b->source;
        if(output.indexStart!=s.indexStart||output.indexCount!=s.indexCount||
           output.vertexStart!=s.vertexStart||output.vertexCount!=s.vertexCount||
           output.sourcePass!=s.sourcePass||output.sourcePassCount!=2||output.projectionPass||
           output.material.textureIndex!=s.textureIndex||
           output.material.mode!=rr::platform::VehicleWindowMaterialMode50DS::requiredTexture||
           output.sourceUvAnimation||output.sourceUvPolicy!=rr::platform::NativeTrackUvPolicy50DS::legacySinglePass)return false;
        output.sourceUvAnimation=true;
        output.sourceUvPolicy=rr::platform::NativeTrackUvPolicy50DS::scLavaTwoPass;return true;
    }
    // Synchronous borrow only. The authentic loaded SC world is the sole input;
    // exact material/pass/geometry values are copied, no caller-made selection
    // table or borrowed storage pointer survives publication. One attempt.
    bool load(const sc_original_world50LM::Owner& world,Diagnostic& d)noexcept {
        namespace shared=pod_materials50ed_fixture::detail;
        using shared::need;
        try {
            d={};need(!attempted_,"SC UV preparation is one-attempt");attempted_=true;
            need(world.loaded()&&world.archive()&&world.spans()&&world.selections(),"SC UV requires its loaded original world");
            rr::assets::DecodeError48C decode;std::vector<std::uint8_t> tob;
            need(rr::assets::extractResMember48C(*world.archive(),"tracks\\sc.tob",tob,decode)&&
                tob.size()==64047&&shared::sha256(tob)=="473351310a098008a477f9e72a6fc7e03d8bb808adcd79f5d810f90aa332b727",
                "SC UV requires its exact no-model-animation-registration TOB");
            const auto& spans=*world.spans();const auto& selections=*world.selections();
            need(spans.size()==4095&&selections.size()==82,"SC UV actual world extent differs");
            auto next=std::make_unique<Storage>();std::size_t count=0;
            std::array<std::uint32_t,2> passSelection{0xffffffffu,0xffffffffu};
            for(std::uint32_t i=0;i<spans.size();++i) {
                d.sourceSpanIndex=i;const auto& span=spans[i];
                need(span.materialSelection<selections.size(),"SC UV source selection extent");
                const auto& s=selections[span.materialSelection];
                if(shared::normalized(s.materialName)!="LAVAJOLD03")continue;
                need(s.track==track_source_fixture::Track::sc&&s.materialKey==materialKey&&s.assetIdentity==0x5343&&
                    s.pass>=1&&s.pass<=2&&s.passCount==2&&s.slot==1&&
                    shared::normalized(s.program)=="DIFFUSETEXTRANSCOMPRESSED"&&
                    shared::normalized(s.textureName)=="LAVAJOLD03"&&!s.projectedPrelit&&!s.staticAnimationFrame&&
                    !s.clampU&&!s.clampV&&s.textureIndex!=track_materials50fa_fixture::unresolved&&
                    span.sourcePass==s.pass&&span.sourcePassCount==2&&!span.projectionPass&&span.textureIndex==s.textureIndex,
                    "SC UV exact original material/pass/image identity differs");
                auto& selected=passSelection[s.pass-1];
                need(selected==0xffffffffu||selected==span.materialSelection,"SC UV duplicate pass prototype");
                selected=span.materialSelection;
                need(count<next->bindings.size(),"SC UV original binding extent exceeded");
                next->bindings[count++]={i,span};
            }
            need(count==8&&passSelection[0]!=0xffffffffu&&passSelection[1]!=0xffffffffu&&
                passSelection[0]!=passSelection[1],"SC UV requires two distinct pass prototypes and four complete surfaces");
            for(std::size_t i=0;i<8;i+=2) {
                const auto& a=next->bindings[i];const auto& b=next->bindings[i+1];
                need(a.source.sourcePass==1&&b.source.sourcePass==2&&b.sourceSpanIndex==a.sourceSpanIndex+1&&
                    a.source.worldInstanceIndex==b.source.worldInstanceIndex&&a.source.surfaceIndex==b.source.surfaceIndex&&
                    a.source.textureIndex==b.source.textureIndex&&a.source.vertexStart==b.source.vertexStart&&
                    a.source.vertexCount==b.source.vertexCount&&a.source.indexStart==b.source.indexStart&&a.source.indexCount==b.source.indexCount,
                    "SC UV original adjacent pass pair differs");
            }
            storage_=std::move(next);d={};return true;
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
         catch(...){try{d.message="SC UV source association failed";}catch(...){}return false;}
    }
};
}
