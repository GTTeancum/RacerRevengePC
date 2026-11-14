#pragma once
#include "track_materials50fa_fixture.h"
#include "../../src/game/original_material_uv_animation50uv.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <cstdint>
#include <exception>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

// One texture-scroll owner for every course. Built from the loaded track's
// own span/selection records (the authored PSM pass fields every course's
// world loader already copies into Selection::sourcePassFields). No course
// flag, file size, checksum or material name is consulted. Replaces the
// stand-ins ma/mb/mc/tb/tc_shield_uv_fixture, sc_lava_uv_fixture and
// mc_shield_render50MC.
//
// Original rules (see original_material_uv_animation50uv.h): a controller
// exists only for a pass with VertexAnimation=unique and uvs_animate true.
// Static world placements of a material pass share its prototype (29B1D0
// with no registration table). Per-model clones (29B050) are made only by an
// explicit model registration; no stock course registers one for a
// UV-animated material, and registration is left to the common track
// animation owner (track-data-gaps\anim) when it lands.
//
// Native bounds (flagged): the renderer's existing boundary admits a
// translation on (a) a single-pass surface and (b) a two-pass surface whose
// both passes scroll, under the policy value still named scLavaTwoPass
// (renderer header rename and shape generalisation ride renderer batch 4/18).
// A surface with one of two passes animated, or more than two passes, is
// refused at load with a log line rather than drawn wrong.
namespace track_uv_animation50uv_fixture {
constexpr std::uint32_t unresolved=0xffffffffu;
// 29C490/29C4E8: the original's shader-pass registration pool is 4096
// entries for the whole game; no track can own more UV prototypes than that.
constexpr std::size_t prototypeCapacity=4096;
constexpr std::size_t spanCapacity=1000000;
struct Diagnostic { std::uint32_t sourceSpanIndex=unresolved; std::string message; };
struct Counts {
    std::uint32_t prototypes=0,spans=0,ignoredNoVertexLeaf=0,vertexLeafWithoutUv=0,noTexGenConsumer=0;
};
class Owner final {
    struct Prototype {
        std::string materialKey; // Selection::materialKey: the material's own identity
        std::uint32_t materialIndex=0,pass=0,materialSelection=0;
        rr::game::OriginalMaterialUv50UV state{};
    };
    struct Storage {
        std::vector<Prototype> prototypes;
        std::vector<std::uint32_t> spanPrototype; // per source span, unresolved = not animated
        std::vector<track_materials50fa_fixture::Span> spans; // copied source ranges
        Counts counts{};
    };
    std::unique_ptr<Storage> storage_;
    bool attempted_=false;
    Prototype* prototype(std::uint32_t span) noexcept {
        if(!storage_||span>=storage_->spanPrototype.size())return nullptr;
        const auto p=storage_->spanPrototype[span];
        return p<storage_->prototypes.size()?&storage_->prototypes[p]:nullptr;
    }
    const Prototype* prototype(std::uint32_t span) const noexcept {
        return const_cast<Owner*>(this)->prototype(span);
    }
    static bool fold(const std::string& a,const char* b) noexcept {
        std::size_t i=0;
        for(;i<a.size()&&b[i];++i){
            char x=a[i],y=b[i];
            if(x>='A'&&x<='Z')x=char(x-'A'+'a');
            if(y>='A'&&y<='Z')y=char(y-'A'+'a');
            if(x!=y)return false;
        }
        return i==a.size()&&!b[i];
    }
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    bool animated(std::uint32_t span)const noexcept{return prototype(span)!=nullptr;}
    Counts counts()const noexcept{return storage_?storage_->counts:Counts{};}
    const rr::game::OriginalMaterialUvRows50UV* rows(std::uint32_t span)const noexcept{
        const auto* p=prototype(span);return p?&p->state.matrix:nullptr;
    }
    // Called only when the span's pass is actually drawn by the renderer
    // (VehicleWindowTrackUvFrame50DS service). Several placements sharing one
    // prototype in the same application tick advance it once (29ACB0).
    bool service(std::uint32_t span,std::uint32_t applicationTimestamp,bool applicationPaused,
        bool callbackAllowsUpdate=true)noexcept{
        auto* p=prototype(span);if(!p)return false;
        return rr::game::serviceOriginalMaterialUv50UV(p->state,applicationTimestamp,
            applicationPaused,callbackAllowsUpdate);
    }
    // Cold join after the common alpha lowering. Geometry must be the exact
    // source range; only the renderer's two existing UV shapes are produced.
    bool bindRenderSpan(std::uint32_t span,rr::platform::VehicleWindowTrackSpan50DS& output)const noexcept{
        const auto* p=prototype(span);if(!p)return false;
        const auto& s=storage_->spans[span];
        if(output.indexStart!=s.indexStart||output.indexCount!=s.indexCount||
           output.vertexStart!=s.vertexStart||output.vertexCount!=s.vertexCount||
           output.sourcePass!=s.sourcePass||output.sourcePassCount!=s.sourcePassCount||
           output.projectionPass||s.projectionPass||
           output.material.textureIndex!=s.textureIndex||
           output.material.mode!=rr::platform::VehicleWindowMaterialMode50DS::requiredTexture||
           output.sourceUvAnimation||output.sourceUvPolicy!=rr::platform::NativeTrackUvPolicy50DS::legacySinglePass)
            return false;
        output.sourceUvAnimation=true;
        // Stand-in name: the renderer's adjacent two-pass UV boundary.
        if(s.sourcePassCount==2)output.sourceUvPolicy=rr::platform::NativeTrackUvPolicy50DS::scLavaTwoPass;
        return true;
    }
    // One attempt. Reads only the loaded track's own records.
    bool load(const std::vector<track_materials50fa_fixture::Span>& spans,
        const std::vector<track_materials50fa_fixture::Selection>& selections,Diagnostic& d)noexcept{
        namespace shared=pod_materials50ed_fixture::detail;
        using shared::need;
        try{
            d={};need(!attempted_,"track UV animation preparation is one-attempt");attempted_=true;
            need(spans.size()<=spanCapacity,"track UV source span capacity");
            auto next=std::make_unique<Storage>();
            next->spanPrototype.assign(spans.size(),unresolved);
            next->spans=spans;
            std::vector<std::uint32_t> selectionPrototype(selections.size(),unresolved);
            std::vector<std::uint8_t> selectionRead(selections.size(),0);
            for(std::uint32_t i=0;i<spans.size();++i){
                d.sourceSpanIndex=i;
                const auto& span=spans[i];
                need(span.materialSelection<selections.size(),"track UV source selection extent");
                const auto& sel=selections[span.materialSelection];
                if(!selectionRead[span.materialSelection]){
                    selectionRead[span.materialSelection]=1;
                    const auto& fields=sel.sourcePassFields;
                    need(fields.is_object()&&fields.contains("authored")&&fields.at("authored").is_array(),
                        "track UV authored pass fields missing");
                    std::vector<std::string> names,values;
                    for(const auto& f:fields.at("authored")){
                        need(f.is_object()&&f.contains("name")&&f.at("name").is_string(),"track UV authored field name");
                        names.push_back(f.at("name").get<std::string>());
                        values.push_back(f.contains("value")&&f.at("value").is_string()?f.at("value").get<std::string>():std::string());
                        need(names.size()<=4096,"track UV authored field count");
                    }
                    std::vector<rr::game::OriginalMaterialUvField50UV> view;
                    view.reserve(names.size());
                    for(std::size_t n=0;n<names.size();++n)view.push_back({names[n],values[n]});
                    rr::game::OriginalMaterialUvConfig50UV config;rr::game::OriginalMaterialUvDiagnostic50UV ud;
                    if(!rr::game::readOriginalMaterialUv50UV(view,config,ud))
                        need(false,std::string("track UV material refused: ")+sel.materialName+" pass "+
                            std::to_string(sel.pass)+" field "+(ud.field?ud.field:"?")+" sourcePC="+std::to_string(ud.sourcePC));
                    using S=rr::game::OriginalMaterialUvStatus50UV;
                    if(ud.status==S::noVertexLeaf){
                        // The original never reads uvs_animate here (SA tred02, TC tcsun).
                        bool authored=false;for(const auto& n:names)authored|=fold(n,"uvs_animate");
                        if(authored)++next->counts.ignoredNoVertexLeaf;
                        continue;
                    }
                    if(ud.status==S::uvsNotAnimated){++next->counts.vertexLeafWithoutUv;continue;}
                    // The rows are consumed only by the TexGen vertex program
                    // (program 43); any other program draws unchanged.
                    if(!fold(sel.program,"DiffuseTexTransCompressed")){++next->counts.noTexGenConsumer;continue;}
                    need(sel.slot==1&&sel.pass>=1&&sel.pass<=sel.passCount,"track UV pass/slot identity");
                    // One controller per material pass, however many scene
                    // selections name that pass (29B1D0 prototype).
                    std::uint32_t found=unresolved;
                    for(std::uint32_t q=0;q<next->prototypes.size();++q)
                        if(next->prototypes[q].materialKey==sel.materialKey&&next->prototypes[q].pass==sel.pass)found=q;
                    if(found==unresolved){
                        need(next->prototypes.size()<prototypeCapacity,"track UV prototype capacity (29C490 pool)");
                        found=static_cast<std::uint32_t>(next->prototypes.size());
                        need(!sel.materialKey.empty(),"track UV material identity missing");
                        next->prototypes.push_back({sel.materialKey,sel.materialIndex,sel.pass,span.materialSelection,
                            rr::game::makeOriginalMaterialUv50UV(config)});
                    }
                    selectionPrototype[span.materialSelection]=found;
                }
                const auto p=selectionPrototype[span.materialSelection];
                if(p==unresolved)continue;
                need(span.sourcePass==sel.pass&&span.sourcePassCount==sel.passCount&&!span.projectionPass&&
                    span.textureIndex==sel.textureIndex&&sel.textureIndex!=track_materials50fa_fixture::unresolved,
                    "track UV span/pass/texture identity");
                need(span.sourcePassCount<=2,"track UV surface with more than two passes has no renderer boundary");
                next->spanPrototype[i]=p;++next->counts.spans;
            }
            // Two-pass surfaces: both passes must scroll and be adjacent (the
            // renderer's only two-pass UV boundary).
            for(std::uint32_t i=0;i<spans.size();++i){
                d.sourceSpanIndex=i;
                const auto& s=spans[i];
                if(s.sourcePassCount!=2||s.sourcePass!=1)continue;
                const bool first=next->spanPrototype[i]!=unresolved;
                const bool second=i+1<spans.size()&&next->spanPrototype[i+1]!=unresolved;
                if(!first&&!second)continue;
                need(first&&second,"track UV two-pass surface animates only one pass (no renderer boundary)");
                const auto& t=spans[i+1];
                need(t.sourcePass==2&&t.sourcePassCount==2&&t.worldInstanceIndex==s.worldInstanceIndex&&
                    t.surfaceIndex==s.surfaceIndex&&t.vertexStart==s.vertexStart&&t.vertexCount==s.vertexCount&&
                    t.indexStart==s.indexStart&&t.indexCount==s.indexCount,"track UV two-pass surface pair differs");
            }
            next->counts.prototypes=static_cast<std::uint32_t>(next->prototypes.size());
            storage_=std::move(next);d={};return true;
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
         catch(...){try{d.message="track UV source association failed";}catch(...){}return false;}
    }
    void log(std::ostream& o)const{
        const auto c=counts();
        o<<"track_uv_animation prototypes="<<c.prototypes<<" spans="<<c.spans
         <<" ignored_no_vertex_leaf="<<c.ignoredNoVertexLeaf<<" vertex_leaf_without_uv="<<c.vertexLeafWithoutUv
         <<" no_texgen_consumer="<<c.noTexGenConsumer<<"\n";
        if(storage_)for(std::size_t i=0;i<storage_->prototypes.size();++i){
            const auto& p=storage_->prototypes[i];
            o<<"track_uv_animation_pass material="<<p.materialIndex<<" pass="<<p.pass
             <<" step_u="<<p.state.stepU<<" step_v="<<p.state.stepV<<" frames="<<p.state.frames64
             <<" looping="<<((p.state.flags68&2u)?1:0)<<" reverse="<<(p.state.flags68&1u)
             <<" while_paused="<<(p.state.animatesWhilePaused18?1:0)<<"\n";
        }
    }
};
}
