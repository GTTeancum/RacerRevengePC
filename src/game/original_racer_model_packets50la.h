#pragma once
#include "original_racer_model_scene50la.h"
#include "model_palette49s.h"
#include "original_racer_light_selection50km.h"
#include "original_racer_model_view50la.h"
#include "original_racer_diffuse_clip50la.h"
#include "../assets/original_standard_model_resource50la.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <exception>
#include <memory>
#include <optional>
#include <new>
#include <vector>

namespace rr::platform {class OriginalUiGarageModelResource50KG; class OriginalUiGarageDrawContribution50KG; class OriginalUiGarageModelLowering50KG; class OriginalUiGarageVisibility50KG;}
namespace rr::game {
enum class OriginalRacerModelPacketStatus50LA : std::uint8_t {
    complete,phase,geometry,palette,numeric,allocation
};
struct OriginalRacerModelPacketResult50LA {
    OriginalRacerModelPacketStatus50LA status=OriginalRacerModelPacketStatus50LA::phase;
    std::uint32_t sourcePC=0;
    bool okay()const noexcept{return status==OriginalRacerModelPacketStatus50LA::complete;}
};

// Native CPU projection of the actual2407E0/240A10 allocation payload.
// It contains only scalar values and matrix slots, with real source palette
// row identities. It contains no VIF/DMA words, guest pointer or packet for a
// renderer. The zero matrix payload is the source memset result, not identity
// transforms or proof that a draw/update method populated the matrices.
struct OriginalRacerModelPacket50LA {
    std::array<float,4> scale40{}; //1E0/1E4/1E8, then binary32 reciprocal1E0.
    std::uint32_t extraMatrixCount=0; //240740 a3:0 or the actual two slots.
    std::vector<std::uint8_t> sourcePaletteRows;
    // extra slots (if any), one model slot, then each real palette-bone slot.
    std::vector<BodyMatrix48G> matrixSlots;
    // Only241280's two distance-dependent bits are populated by the current
    // native view producer. Other clip/LOD flags and complete packet20 remain
    // outside this field; in particular this is not a fabricated full word.
    std::uint16_t distanceFlags20=0;
    // Full source20 is absent until the real Garage241280 matrix callback.
    // It is captured before the later1B3 cache-bit store, never reconstructed.
    std::optional<std::uint16_t> garageSourceFlags20;
    std::size_t modelMatrixIndex()const noexcept{return extraMatrixCount;}
};

class OriginalRacerModelPackets50LA final {
    friend class rr::platform::OriginalUiGarageModelResource50KG;
    friend class rr::platform::OriginalUiGarageDrawContribution50KG;
    friend class rr::platform::OriginalUiGarageModelLowering50KG;
    friend class rr::platform::OriginalUiGarageVisibility50KG;
    using S=OriginalRacerModelPacketStatus50LA;
    using R=OriginalRacerModelPacketResult50LA;
public:
    OriginalRacerModelPackets50LA()=default;
    OriginalRacerModelPackets50LA(const OriginalRacerModelPackets50LA&)=delete;
    OriginalRacerModelPackets50LA& operator=(const OriginalRacerModelPackets50LA&)=delete;
    ~OriginalRacerModelPackets50LA(){if((constructed_&&!released_)||
        (sourcePackets_.entered&&!sourcePackets_.nativeWithdrawn))std::terminate();}

    // The caller has really completed26E820, including resource1B4 and row
    // table1B8. This consumes the actual palette20, not Scene48D's identity
    // palette shorthand or a caller-provided packet count. No Entity retain
    // or material acquisition occurs here.
    R construct240740(OriginalRacerModelSceneOwner50LA& model,
        const rr::assets::OriginalStandardModelGeometry50LA& geometry,bool sourceA3)noexcept {
        if(sourcePackets_.entered||constructed_||released_||!model.constructed2403A0()||!model.rowsLoaded_||model.released_||
           !model.loadedQuantization240570()||
           model.fields().packets1F0||!model.fields().sourceRows1B8)
            return {S::phase,0x240740u};
        if(model.fields().geometry1B4!=&geometry||!geometry.constructed240110()||
           !geometry.payload()||geometry.payload()->nodeCount()!=model.sourceRowCount())
            return {S::geometry,0x240784u};
        const bool palette=geometry.hasPalette20();
        const auto count=palette?geometry.paletteCount20():1u;
        if(!count||count>rr::assets::maxModelNodes49O)return {S::palette,0x24081cu};
        const auto& fields=model.fields();
        if(std::fegetround()!=FE_TONEAREST)return {S::numeric,0x2408f0u};
        for(unsigned lane=0;lane<3;++lane)
            if(!std::isfinite(fields.scale1E0[lane]))return {S::numeric,0x2408c4u};
        if(fields.scale1E0[0]==0.f)return {S::numeric,0x2408f0u};
        volatile float reciprocal=1.f/fields.scale1E0[0];
        if(!std::isfinite(reciprocal))return {S::numeric,0x2408f0u};
        if(!palette&&fields.offsetEnabled1DC)
            for(float value:fields.offset1D0)if(!std::isfinite(value))return {S::numeric,0x240b40u};
        std::unique_ptr<OriginalRacerModelPacket50LA[]> next;
        try{
            next=std::make_unique<OriginalRacerModelPacket50LA[]>(count);
            for(std::size_t packet=0;packet<count;++packet){
                auto& out=next[packet];out.extraMatrixCount=sourceA3?2u:0u;
                const auto* rows=palette?geometry.palette20(packet):nullptr;
                if(palette){
                    if(!rows||rows->empty()||rows->size()>model.sourceRowCount())return {S::palette,0x240850u};
                    for(auto row:*rows)if(row>=model.sourceRowCount())return {S::palette,0x240850u};
                    out.sourcePaletteRows=*rows;
                }
                out.matrixSlots.resize(out.extraMatrixCount+1u+out.sourcePaletteRows.size());
                // Native storage may have padding; explicitly write every
                // source payload float rather than relying on an allocator.
                for(auto& matrix:out.matrixSlots)std::memset(matrix.v,0,sizeof matrix.v);
                std::memcpy(out.scale40.data(),fields.scale1E0.data(),3*sizeof(float));
                out.scale40[3]=reciprocal;
                // Only240A10 has this conditional root-matrix override.
                //2407E0 palette storage remains source-zero at construction.
                if(!palette&&fields.offsetEnabled1DC){
                    auto& matrix=out.matrixSlots[out.modelMatrixIndex()];
                    matrix.v[0]=matrix.v[5]=matrix.v[10]=matrix.v[15]=1.f;
                    std::memcpy(matrix.v+12,fields.offset1D0.data(),3*sizeof(float));
                }
            }
        }catch(...){return {S::allocation,palette?0x240874u:0x240a44u};}
        model_=&model;geometry_=&geometry;hasPalette20_=palette;count_=count;
        packets_=std::move(next);constructed_=true;
        //24076C..780 and2407A4/7B4. The field owns this typed native facet;
        // no binary-compatible console packet is exposed.
        model.fields().flags1B3=static_cast<std::uint8_t>((model.fields().flags1B3&~1u)|(sourceA3?1u:0u));
        model.fields().packets1F0=this;
        return {S::complete,0x2407c4u};
    }

    // Reached15432C always passes a3=1. Explicit diagnostic overload keeps
    // the source choice fixed for the animation-owner join.
    bool construct240740(OriginalRacerModelSceneOwner50LA& model,
        const rr::assets::OriginalStandardModelGeometry50LA& geometry,R& diagnostic)noexcept {
        diagnostic=construct240740(model,geometry,true);return diagnostic.okay();
    }

    bool constructed240740()const noexcept{return constructed_&&!released_;}
    bool released()const noexcept{return released_;}
    std::uint16_t compressionFlag40()const noexcept {return constructed240740()&&model_?model_->fields().flags1B2&0x40u:0u;}
    std::size_t count()const noexcept{return constructed240740()?count_:0u;}
    const OriginalRacerModelPacket50LA* packet(std::size_t i)const noexcept {
        return constructed240740()&&i<count_?&packets_[i]:nullptr;
    }
    // Matrix-producing portion241488..241598 for the one real palette
    // admitted by the current racer draw path.49S already evaluated the
    // current graphics worlds in source order; consume that same result,
    // never bind locals or a later physics pose. The selected light values
    // come from the caller's actual draw lease under its native light policy.
    // This does not assert original14A0A0 cache parity or fill packet flags:
    // distance1BC/radius198/fog/LOD and global submission remain separate.
    // Serial preparation storage, not an accepted-frame receipt. Failure may
    // change slots; consumers use them only after success and copy their final
    // output before another preparation or240450. No allocation or callback.
    R preparePaletteMatrices241280(const ModelPaletteInput49S& input,
        const ModelPalette49S& worlds,
        const original_racer_lighting50KM::LightMatrices& lights)noexcept {
        geometryRevision_=0;matricesPrepared_=false;
        if(!constructed240740()||!model_||model_->fields().packets1F0!=this||
           input.phase!=ModelPalettePhase49S::selectedRenderPreparation||
           input.suppliedState!=ModelPaletteSceneState49S::completeCurrentScene||
           !(input.modelFlags191&1u)||input.modelRoot!=model_->model()->sceneIndex)
            return {S::phase,0x2412acu};
        if(!hasPalette20_||count_!=1u||!input.sourceNodes||!input.paletteIds||
           input.sourceNodeCount!=model_->sourceRowCount()||
           worlds.entryCount!=input.paletteCount)
            return {S::palette,0x24141cu};
        auto& out=packets_[0];
        const auto& fields=model_->fields();
        if(!(fields.flags1B3&1u)||out.extraMatrixCount!=2u||
           out.sourcePaletteRows.size()!=worlds.entryCount||
           out.matrixSlots.size()!=3u+worlds.entryCount)
            return {S::palette,0x241488u};
        for(unsigned row=0;row<4;++row){
            std::memcpy(out.matrixSlots[0].v+4u*row,lights.directions[row].data(),4u*sizeof(float));
            std::memcpy(out.matrixSlots[1].v+4u*row,lights.diffuse[row].data(),4u*sizeof(float));
        }
        //2413D8..241418 overrides only the diffuse ambient row.204 is a
        // float payload despite the native field's uint32_t representation.
        if(fields.flags1F4&1u){
            out.matrixSlots[1].v[12]=fields.field1F8;
            out.matrixSlots[1].v[13]=fields.field1FC;
            out.matrixSlots[1].v[14]=fields.field200;
        }
        if(fields.flags1F4&2u)
            std::memcpy(out.matrixSlots[1].v+15,&fields.field204,sizeof(float));
        std::memcpy(out.matrixSlots[2].v,worlds.rootWorld.v,sizeof worlds.rootWorld.v);
        for(std::uint32_t i=0;i<worlds.entryCount;++i){
            const auto id=out.sourcePaletteRows[i];
            const auto* row=model_->sourceRow(id);
            if(id>=input.sourceNodeCount||input.paletteIds[i]!=id||!row||
               row->sceneIndex!=input.sourceNodes[id])return {S::palette,0x241540u};
            std::memcpy(out.matrixSlots[3u+i].v,worlds.nodeWorld[i].v,sizeof worlds.nodeWorld[i].v);
        }
        if(matrixRevision_==std::numeric_limits<std::uint64_t>::max())return {S::phase,0x241598u};
        ++matrixRevision_;geometryRevision_=0;matricesPrepared_=true;
        return {S::complete,0x241598u};
    }
    // Commit the actual successful post-motion draw's source geometry. This
    // receipt remains paired with these exact palette matrices while stopped.
    bool retainDrawGeometry50LA(const rr::assets::Scene48D& semantic,const BodyMatrix48G& world,
        const OriginalRacerAggregateBounds50LA& bounds,std::uint64_t& revision)noexcept {
        if(!constructed240740()||!matricesPrepared_||!matrixRevision_||count_!=1||
           !geometry_||!geometry_->payload()||!geometry_->payload()->scene()||
           semantic.nodes.size()!=model_->sourceRowCount()||
           semantic.quantization!=geometry_->payload()->scene()->quantization||
           std::memcmp(world.v,packets_[0].matrixSlots[2].v,sizeof world.v))return false;
        // The visual and model resource are separate decodes of the same PSG.
        // Bind their exact semantic payload once, never just a node count/scale.
        if(retainedSemantic_&&retainedSemantic_!=&semantic)return false;
        if(!retainedSemantic_){
            const auto& original=*geometry_->payload()->scene();
            if(semantic.materials!=original.materials||semantic.world_space!=original.world_space||
               semantic.identity_matrix_palette!=original.identity_matrix_palette||
               semantic.lods.size()!=original.lods.size())return false;
            for(std::size_t i=0;i<semantic.nodes.size();++i){
                const auto& a=semantic.nodes[i];const auto& b=original.nodes[i];
                if(a.name!=b.name||a.parent!=b.parent||a.local_matrix!=b.local_matrix||
                   a.bind_matrix!=b.bind_matrix||a.stored_bounds!=b.stored_bounds)return false;
            }
            for(std::size_t l=0;l<semantic.lods.size();++l){
                const auto& a=semantic.lods[l];const auto& b=original.lods[l];
                if(a.cutoff!=b.cutoff||a.surfaces.size()!=b.surfaces.size())return false;
                for(std::size_t j=0;j<a.surfaces.size();++j){
                    const auto& x=a.surfaces[j];const auto& y=b.surfaces[j];
                    if(x.material_index!=y.material_index||x.component_word!=y.component_word||
                       x.has_original_colors!=y.has_original_colors||x.groups!=y.groups||
                       x.indices!=y.indices||x.vertices.size()!=y.vertices.size()||
                       x.strips.size()!=y.strips.size())return false;
                    for(std::size_t v=0;v<x.vertices.size();++v){
                        const auto& c=x.vertices[v];const auto& d=y.vertices[v];
                        if(c.position_local!=d.position_local||c.normal_local!=d.normal_local||
                           c.uv!=d.uv||c.color!=d.color||c.bone!=d.bone)return false;
                    }
                    for(std::size_t t=0;t<x.strips.size();++t){
                        const auto& c=x.strips[t];const auto& d=y.strips[t];
                        if(c.group!=d.group||c.start!=d.start||c.end!=d.end||c.vertex_start!=d.vertex_start)return false;
                    }
                }
            }
        }
        for(unsigned i=0;i<4;++i)if(!std::isfinite(bounds.center160[i])||
            !std::isfinite(bounds.halfExtent170[i])||bounds.halfExtent170[i]<0.f)return false;
        if(!std::isfinite(bounds.radius198)||bounds.radius198<0.f)return false;
        retainedSemantic_=&semantic;retainedWorld_=world;retainedBounds_=bounds;
        geometryRevision_=matrixRevision_;revision=geometryRevision_;return true;
    }
    bool prepareDiffuseSelection240C00(const rr::assets::Scene48D& semantic,std::uint64_t revision,
        const BodyMatrix48G& world,const OriginalRacerAggregateBounds50LA& bounds,
        const std::array<float,16>& combined,const std::array<float,16>& guard,
        OriginalRacerDiffuseClip50LA& output)noexcept {
        if(!constructed240740()||!matricesPrepared_||!model_||model_->fields().packets1F0!=this||
           model_->model_.observer19C||!revision||revision!=geometryRevision_||revision!=matrixRevision_||
           &semantic!=retainedSemantic_||std::memcmp(world.v,retainedWorld_.v,sizeof world.v)||
           bounds.center160!=retainedBounds_.center160||bounds.halfExtent170!=retainedBounds_.halfExtent170||
           bounds.radius198!=retainedBounds_.radius198)return false;
        OriginalRacerDiffuseClip50LA next;
        if(model_->model_.flags03&4u){model_->model_.flags191&=static_cast<std::uint8_t>(~1u);next.disabled=true;output=next;return true;}
        if(!projectOriginalRacerDiffuseClip50LA(world,bounds,combined,guard,next))return false;
        // These are the actual original selected/clip fields, not draw-ready
        // flags. Rejected models remain rejected independently of bit04.
        model_->model_.flags191=static_cast<std::uint8_t>((model_->model_.flags191&~1u)|(next.selected?1u:0u));
        auto& f=model_->fields();
        if(next.selected)f.flags1B2=static_cast<std::uint8_t>((f.flags1B2&~5u)|(next.remainingMask?5u:0u));
        else f.flags1B2|=4u;
        output=next;return true;
    }
    // Current-camera portion of26F070 and241320..24146C for the SAME
    // retained multi-node model. Called for every presentation, including a
    // stopped racer's retained geometry. Bounds are the actual49O output
    // copied with that successful geometry; the light/matrix slots stay put.
    // Mutates only preparation state, not accepted texture/renderer history.
    // Full240C00 observer clipping, LOD and packet20/1B3.08 timing are not
    // inferred from this independent distance/fog/map-2 subset.
    R prepareDistanceFlags241280(const OriginalRacerAggregateBounds50LA& bounds,
        const std::array<float,3>& localPosition,const std::array<float,3>& cameraPosition,
        float fogStartB0,const OriginalRacerModelRenderGlobals50LA& globals,
        OriginalRacerModelDistanceFlags50LA& output)noexcept {
        if(!constructed240740()||!matricesPrepared_||!model_||
           model_->fields().packets1F0!=this||model_->model_.nodeCount1A0<=1u||
           !hasPalette20_||count_!=1u)return {S::phase,0x26f090u};
        if(!std::isfinite(bounds.radius198)||bounds.radius198<0.f||!std::isfinite(fogStartB0))
            return {S::numeric,0x24133cu};
        for(unsigned lane=0;lane<4;++lane)
            if(!std::isfinite(bounds.center160[lane])||!std::isfinite(bounds.halfExtent170[lane]))
                return {S::numeric,0x26f0e0u};
        float distance=0.f;
        if(!projectOriginalRacerDistance26F070(bounds.center160,localPosition,cameraPosition,distance))
            return {S::numeric,0x26f190u};
        volatile float farEdge=distance+bounds.radius198;
        if(!std::isfinite(farEdge))return {S::numeric,0x24133cu};
        auto& fields=model_->fields();
        // Retain actual aggregate values in their canonical model facet;
        // these are not renderer display bounds or an estimated root radius.
        model_->model_.bounds160=bounds.center160;
        model_->model_.bounds170=bounds.halfExtent170;
        model_->model_.radius198=bounds.radius198;
        fields.distance1BC=distance;
        fields.flags1B2=static_cast<std::uint8_t>((fields.flags1B2&~2u)|
            (farEdge<fogStartB0?0u:2u));
        fields.flags1B3=static_cast<std::uint8_t>((fields.flags1B3&~2u)|
            (distance<=globals.secondMapDistance382FA0()?0u:2u));
        auto& packet=packets_[0];
        packet.distanceFlags20=static_cast<std::uint16_t>((fields.flags1B2&2u)|
            (static_cast<std::uint16_t>(fields.flags1B3&2u)<<8));
        output={distance,packet.distanceFlags20};
        return {S::complete,0x24146cu};
    }
    R preflight240450()const noexcept {
        if(released_)return {S::complete,0x240520u};
        if(!constructed_||!model_||!geometry_||!packets_||model_->fields().packets1F0!=this||
           model_->fields().geometry1B4!=geometry_)return {S::phase,0x240490u};
        if(!geometry_->constructed240110()||geometry_->hasPalette20()!=hasPalette20_||
           (hasPalette20_&&geometry_->paletteCount20()!=count_))return {S::geometry,0x24049cu};
        return {S::complete,0x240450u};
    }
    //240450 only, before26E460 releases the resource it reads to determine
    // the actual palette count. The source leaves1F0's pointer value stale;
    // native released_ prevents reuse without inventing a source null store.
    R release240450()noexcept {
        const auto ready=preflight240450();if(!ready.okay()||released_)return ready;
        if(hasPalette20_){
            for(std::size_t i=0;i<count_;++i){
                std::vector<BodyMatrix48G>().swap(packets_[i].matrixSlots);
                std::vector<std::uint8_t>().swap(packets_[i].sourcePaletteRows);
            }
        }
        packets_.reset();released_=true;
        return {S::complete,0x240520u};
    }
private:
    // Actual one-node/no20 Garage branch. Private to its real retained model
    // and draw owner. This is only241280's numeric packet preparation, not a
    // visibility selection, queue admission or accepted-frame capability.
    R prepareGarageDirectMatrix241684(const SceneGraph49P& graph,
        const std::array<float,3>& cameraPosition,float fogStartB0)noexcept {
        matricesPrepared_=false;geometryRevision_=0;
        if(!constructed240740()||!model_||!geometry_||hasPalette20_||count_!=1||
           model_->fields().packets1F0!=this||model_->sourceRowCount()!=1||
           (!model_->loadedFreshRows28CAC0()&&!model_->warmChildlessRows_)||
           model_->model_.nodeCount1A0!=(model_->warmChildlessRows_?0u:1u)||!(model_->model_.flags191&1u)||
           (model_->fields().flags1B3&1u)||model_->fields().offsetEnabled1DC||
           packets_[0].extraMatrixCount||packets_[0].matrixSlots.size()!=1||
           !packets_[0].sourcePaletteRows.empty())return {S::phase,0x2415e0u};
        const auto bounds=model_->publishDirectRootBounds28E110(graph);
        if(!bounds.okay())return {S::phase,bounds.sourcePC};
        const auto index=static_cast<std::size_t>(model_->model_.sceneIndex);
        if(index>=graph.count||!refreshSceneWorld49P(graph,model_->model_.sceneIndex))return {S::phase,0x2897b0u};
        const auto& root=graph.nodes[index];
        const std::array<float,3> local{{root.localC0.v[12],root.localC0.v[13],root.localC0.v[14]}};
        float distance=0;
        if(!std::isfinite(fogStartB0)||!model_->model_.radius198||
           !projectOriginalRacerDistance26F070(model_->model_.bounds160,local,cameraPosition,distance))return {S::numeric,0x26f190u};
        volatile float farEdge=distance+*model_->model_.radius198;
        if(!std::isfinite(farEdge)||matrixRevision_==std::numeric_limits<std::uint64_t>::max())return {S::numeric,0x24133cu};
        auto& fields=model_->fields();fields.distance1BC=distance;
        fields.flags1B2=static_cast<std::uint8_t>((fields.flags1B2&~2u)|(farEdge<fogStartB0?0u:2u));
        //241390 skips24141C..24146C: no palette second-map distance test.
        // Keep the actual previous1B3 byte; do not manufacture a map2 gate.
        directFlags20_=static_cast<std::uint16_t>(fields.flags1B2)|(static_cast<std::uint16_t>(fields.flags1B3)<<8);
        packets_[0].distanceFlags20=static_cast<std::uint16_t>(directFlags20_&0x0202u);
        std::memcpy(packets_[0].matrixSlots[0].v,root.world100.v,sizeof root.world100.v);
        ++matrixRevision_;matricesPrepared_=true;return {S::complete,0x2416a0u};
    }
    // Private reached241280 producer. The owning Visibility callback supplies
    // the same retained model graph, actual14A0A0 light result and live Display
    // B0/process382FA0 values. This is only the matrix/flag prefix: no material
    // cache, FIFO append, Entity children or complete-frame receipt is implied.
    R prepareGaragePackets241280(SceneGraph49P graph,
        const original_racer_lighting50KM::LightMatrices* lights,
        float fogStartB0,float secondMapDistance382FA0)noexcept {
        matricesPrepared_=false;geometryRevision_=0;
        if(!constructed240740()||!model_||!geometry_||!packets_||!count_||
           model_->fields().packets1F0!=this||model_->fields().geometry1B4!=geometry_||
           !model_->fields().sourceRows1B8||!(model_->model_.flags191&1u)||
           !model_->fields().distance1BC||std::fegetround()!=FE_TONEAREST)
            return {S::phase,0x2412acu};
        const auto index=static_cast<std::size_t>(model_->model_.sceneIndex);
        if(index>=graph.count||!refreshSceneWorld49P(graph,model_->model_.sceneIndex))
            return {S::phase,0x2897b0u};
        auto& fields=model_->fields();
        const auto radius=model_->model_.nodeCount1A0==1u?model_->model_.radius194:model_->model_.radius198;
        if(!radius||!std::isfinite(*radius)||!std::isfinite(*fields.distance1BC)||
           !std::isfinite(fogStartB0)||!std::isfinite(secondMapDistance382FA0)||
           matrixRevision_==std::numeric_limits<std::uint64_t>::max())return {S::numeric,0x24133cu};
        const bool extra=(fields.flags1B3&1u)!=0;
        if(extra&&!lights)return {S::phase,hasPalette20_?0x2413c8u:0x24161cu};
        if(hasPalette20_!=(geometry_->hasPalette20())||count_!=(hasPalette20_?geometry_->paletteCount20():1u))
            return {S::palette,0x24141cu};
        for(std::size_t i=0;i<count_;++i){
            const auto& packet=packets_[i];
            if(packet.extraMatrixCount!=(extra?2u:0u)||
               packet.matrixSlots.size()!=packet.extraMatrixCount+1u+packet.sourcePaletteRows.size())
                return {S::palette,0x241488u};
            if(hasPalette20_){
                const auto* original=geometry_->palette20(i);
                if(!original||*original!=packet.sourcePaletteRows)return {S::palette,0x24150cu};
                for(auto id:packet.sourcePaletteRows)
                    if(id>=model_->sourceRowCount())return {S::palette,0x241540u};
            }else if(!packet.sourcePaletteRows.empty())return {S::palette,0x2415e0u};
        }
        volatile float farEdge=*fields.distance1BC+*radius;
        if(!std::isfinite(farEdge))return {S::numeric,0x24133cu};
        fields.flags1B2=static_cast<std::uint8_t>((fields.flags1B2&~2u)|(farEdge<fogStartB0?0u:2u));
        if(hasPalette20_)fields.flags1B3=static_cast<std::uint8_t>((fields.flags1B3&~2u)|
            (secondMapDistance382FA0<*fields.distance1BC?2u:0u));
        const auto sourceFlags=static_cast<std::uint16_t>(fields.flags1B2)|
            (static_cast<std::uint16_t>(fields.flags1B3)<<8);
        for(std::size_t i=0;i<count_;++i){
            auto& packet=packets_[i];packet.garageSourceFlags20=sourceFlags;
            packet.distanceFlags20=static_cast<std::uint16_t>(sourceFlags&0x0202u);
            if(extra){
                for(unsigned row=0;row<4;++row){
                    std::memcpy(packet.matrixSlots[0].v+4u*row,lights->directions[row].data(),4u*sizeof(float));
                    std::memcpy(packet.matrixSlots[1].v+4u*row,lights->diffuse[row].data(),4u*sizeof(float));
                }
                if(fields.flags1F4&1u){packet.matrixSlots[1].v[12]=fields.field1F8;
                    packet.matrixSlots[1].v[13]=fields.field1FC;packet.matrixSlots[1].v[14]=fields.field200;}
                if(fields.flags1F4&2u)std::memcpy(packet.matrixSlots[1].v+15,&fields.field204,sizeof(float));
            }
            if(hasPalette20_||!fields.offsetEnabled1DC)
                std::memcpy(packet.matrixSlots[packet.modelMatrixIndex()].v,graph.nodes[index].world100.v,sizeof graph.nodes[index].world100.v);
            for(std::size_t row=0;row<packet.sourcePaletteRows.size();++row){
                const auto* actual=model_->sourceRow(packet.sourcePaletteRows[row]);
                //241548's actual null branch retains the prior slot. It does
                // not substitute an identity/bind matrix or erase provenance.
                if(!actual)continue;
                if(static_cast<std::size_t>(actual->sceneIndex)>=graph.count||
                   !refreshSceneWorld49P(graph,actual->sceneIndex))return {S::phase,0x2897b0u};
                std::memcpy(packet.matrixSlots[packet.modelMatrixIndex()+1u+row].v,
                    graph.nodes[static_cast<std::size_t>(actual->sceneIndex)].world100.v,sizeof graph.nodes[static_cast<std::size_t>(actual->sceneIndex)].world100.v);
            }
        }
        ++matrixRevision_;matricesPrepared_=true;
        // The no-palette cache scan2416A4..241750 is performed by the actual
        // collector caller, not replaced here with a null1C8 or a ready bit.
        return {S::complete,hasPalette20_?0x2415b4u:0x2416a0u};
    }
#include "original_racer_source_packets50la.inl"
    std::uint16_t directFlags20_=0;
    OriginalRacerModelSceneOwner50LA* model_=nullptr;
    const rr::assets::OriginalStandardModelGeometry50LA* geometry_=nullptr;
    std::unique_ptr<OriginalRacerModelPacket50LA[]> packets_;
    std::size_t count_=0;
    bool hasPalette20_=false,constructed_=false,released_=false,matricesPrepared_=false;
    std::uint64_t matrixRevision_=0,geometryRevision_=0;
    const rr::assets::Scene48D* retainedSemantic_=nullptr;
    BodyMatrix48G retainedWorld_{};
    OriginalRacerAggregateBounds50LA retainedBounds_{};
};
} // namespace rr::game
