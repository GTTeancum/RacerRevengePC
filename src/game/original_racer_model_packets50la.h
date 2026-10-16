#pragma once
#include "original_racer_model_scene50la.h"
#include "model_palette49s.h"
#include "original_racer_light_selection50km.h"
#include "original_racer_model_view50la.h"
#include "../assets/original_standard_model_resource50la.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <exception>
#include <memory>
#include <vector>

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
    std::size_t modelMatrixIndex()const noexcept{return extraMatrixCount;}
};

class OriginalRacerModelPackets50LA final {
    using S=OriginalRacerModelPacketStatus50LA;
    using R=OriginalRacerModelPacketResult50LA;
public:
    OriginalRacerModelPackets50LA()=default;
    OriginalRacerModelPackets50LA(const OriginalRacerModelPackets50LA&)=delete;
    OriginalRacerModelPackets50LA& operator=(const OriginalRacerModelPackets50LA&)=delete;
    ~OriginalRacerModelPackets50LA(){if(constructed_&&!released_)std::terminate();}

    // The caller has really completed26E820, including resource1B4 and row
    // table1B8. This consumes the actual palette20, not Scene48D's identity
    // palette shorthand or a caller-provided packet count. No Entity retain
    // or material acquisition occurs here.
    R construct240740(OriginalRacerModelSceneOwner50LA& model,
        const rr::assets::OriginalStandardModelGeometry50LA& geometry,bool sourceA3)noexcept {
        if(constructed_||released_||!model.constructed2403A0()||!model.rowsLoaded_||model.released_||
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
        matricesPrepared_=true;
        return {S::complete,0x241598u};
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
    OriginalRacerModelSceneOwner50LA* model_=nullptr;
    const rr::assets::OriginalStandardModelGeometry50LA* geometry_=nullptr;
    std::unique_ptr<OriginalRacerModelPacket50LA[]> packets_;
    std::size_t count_=0;
    bool hasPalette20_=false,constructed_=false,released_=false,matricesPrepared_=false;
};
} // namespace rr::game
