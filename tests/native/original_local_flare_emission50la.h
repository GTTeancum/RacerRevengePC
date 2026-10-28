#pragma once
#include "../../src/game/original_pod_cable_allocation50la.h"
#include "../../src/assets/original_standard_texture_resource50la.h"
#include <cmath>
namespace rr::platform {class VehicleWindowGl50DS;}
namespace original_local_flare50LA {
// The actual four-vertex strip's23CA50 CPU prefix. No racer allocation,
// cable Display pin, FIFO receipt or source callback return is borrowed.
// Only the authenticated World visitor and its synchronous native consumer
// may call this. The visitor owns current Application/thread/frame admission.
class Emission final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class rr::platform::VehicleWindowGl50DS;
    using Strip=rr::game::OriginalPodCableStripCold50LA;
    using Attributes=rr::game::OriginalCableStripAttributes50LA;
    using Table=rr::assets::OriginalStandardMaterialTable50LA;
    using Diagnostic=rr::assets::OriginalStandardMaterialDiagnostic50LA;
    using Binding=rr::assets::OriginalCableEmissionBinding50LA;
    enum class Outcome {blocked,childrenOnly,nativeLoweringRequired};
    struct Vertex {std::array<float,3> position,triple;std::array<float,2> uv;};
    struct Operands {
        std::array<Vertex,4> vertices;
        std::array<float,4> color;
        std::array<std::array<float,4>,4> matrix40,matrix90,matrixE0;
        std::array<float,4> vector130;
        Binding material;
        const Strip* strip=nullptr;
        std::shared_ptr<Table> table;
        // Borrows valid only while the same actual World and accepted frame
        // remain retained. No accepted/native binding state is written here.
    };
    static bool materialCurrent(const Operands& input)noexcept {
        return input.strip&&input.table&&input.strip->primitive30_&&
            input.strip->primitive30_->reference.retained_&&
            !input.strip->primitive30_->reference.released_&&
            input.strip->primitive30_->reference.table_==input.table&&
            input.table->coldCollectorCurrent(input.material.registry_);
    }
    static const rr::assets::OriginalStandardMaterialMapBindingView50LA& map(const Operands& input)noexcept {
        return input.material.maps_[0];
    }
    static bool nativeRasterProfile(const Operands& input)noexcept {
        if(!materialCurrent(input))return false;
        const auto& material=input.material;
        const auto& final=material.effectivePass_;
        const auto& origin=material.originatingPass_;
        if(!origin.parameterCount28||*origin.parameterCount28||!final.parameterCount28||*final.parameterCount28||
           !origin.flags21||!final.flags21||((*origin.flags21|*final.flags21)&4u))return false;
        // Exact bounded native additive profile. Every value comes from the
        // current FINAL map, including TEXA; authored blank fields are not
        // treated as zero or substituted by an assumed template.
        constexpr std::int32_t fields[]={1,0,255,255,0,0,2,0,1,128,0,0,0,0,0,0,1,0,0,1,1,1,1,0,0,0,0,1,6,0,0,0,1,2,0};
        static_assert(sizeof(fields)/sizeof(fields[0])==rr::assets::originalUiGsFieldCount50KG,"review new fields");
        const auto& context=map(input).contextAfter249610;
        if(context.runtime435670)return false;
        for(std::size_t i=0;i<rr::assets::originalUiGsFieldCount50KG;++i)
            if(context.fields[i].value!=fields[i])return false;
        constexpr std::int32_t sampler[]={0,0,1,5,0,-75};
        for(std::size_t i=0;i<6;++i)if(context.sampler[i].value!=sampler[i])return false;
        if(input.color!=std::array<float,4>{{127.f,127.f,127.f,64.f}}||
           input.vector130!=std::array<float,4>{{1.f,1.f,1.f,1.f}})return false;
        for(std::size_t row=0;row<4;++row)for(std::size_t column=0;column<4;++column){
            if(input.matrix40[row][column]!=(row==column?1.f:0.f)||
               input.matrixE0[row][column]!=(row==column?1.f:0.f)||
               input.matrix90[row][column]!=(row==3?input.color[column]:0.f))return false;
        }
        for(const auto& vertex:input.vertices)
            if(vertex.triple!=std::array<float,3>{{0.f,1.f,0.f}})return false;
        return true;
    }
    static bool borrowImage(const Operands& input,std::shared_ptr<const std::vector<std::uint8_t>>& backing,
        const std::string*& member,const std::uint8_t*& data,std::size_t& bytes)noexcept {
        if(!nativeRasterProfile(input))return false;
        const auto* texture=map(input).texture;
        if(!texture||!texture->flagsF8()||!texture->flagsF8KnownMask()||
           !(*texture->flagsF8KnownMask()&1u)||(*texture->flagsF8()&1u))return false;
        return texture->borrowLocalFlarePstImage50LA(backing,member,data,bytes);
    }
    static bool shape(const Strip& strip)noexcept {
        const auto& a=strip.attributes_;
        return strip.entity_.reference00==1&&strip.entity_.parent1C&&
            !strip.entity_.firstChild18&&strip.format3C_==7&&strip.count34_==1&&
            strip.groups38_&&strip.groups38_[0]==4&&strip.field40_&&*strip.field40_==1&&
            strip.primitive30_&&strip.primitive30_->attributes10==&a&&
            strip.primitive30_->vertices20==4&&strip.primitive30_->groups22==1&&
            strip.primitive30_->byte0C==7&&a.vertices_&&a.group_&&
            a.group_->vertexCount==4&&a.group_->batchCount==1&&a.group_->counts&&
            a.group_->counts[0]==4&&a.group_->batches&&a.group_->aliases&&
            a.group_->positions&&a.group_->triples&&a.group_->texcoords&&
            a.groupCounts_&&a.groupCounts_[0]==4&&
            a.group_->positions[0].count==4&&a.group_->triples[0].count==4&&a.group_->texcoords[0].count==4&&
            a.group_->positions[0].values==a.vertices_->positions.data()&&
            a.group_->triples[0].values==a.vertices_->triples.data()&&
            a.group_->texcoords[0].values==a.vertices_->texcoords.data()&&
            a.group_->batches[0].count==4&&
            a.group_->batches[0].positions==a.vertices_->positions.data()&&
            a.group_->batches[0].triples==a.vertices_->triples.data()&&
            a.group_->batches[0].texcoords==a.vertices_->texcoords.data();
    }
    static Outcome capture23CA50(Strip& strip,const std::shared_ptr<Table>& table,
        Operands& output,Diagnostic& d)noexcept {
        using S=rr::assets::OriginalStandardMaterialStatus50LA;
        using Stage=rr::assets::OriginalCableMaterialCallbackStage50LA;
        d={};
        const auto reject=[&](S status,std::uint32_t pc)noexcept {
            d.status=status;d.sourcePC=pc;return Outcome::blocked;
        };
        if(!table||!shape(strip))return reject(S::input,0x23ca50);
        auto& reference=strip.primitive30_->reference;
        if(!reference.retained_||reference.released_||reference.table_!=table)
            return reject(S::reference,0x2467c4);
        if(strip.flags48_&2u)return Outcome::childrenOnly; //Actual no-draw gate.
        auto& a=strip.attributes_;
        if(!a.common_){
            try {a.common_=std::make_unique<Attributes::CommonData>();}
            catch(...){return reject(S::allocation,0x23cb34);}
            d.sourceEffectsCommitted=true;
        }
        a.common_->layout=Attributes::CommonData::Layout::cableFormat7;
        d.sourceEffectsCommitted=true;
        // Genuine14ABD0 supplies field40=1: never borrow global ambient from
        // the different Racer field40=0 route. Preserve dirty consumption.
        if(strip.flags48_&1u){
            if(!strip.color70_)return reject(S::source,0x23cac4);
            a.common_->colorC0()=*strip.color70_;
            strip.flags48_=static_cast<std::uint8_t>(strip.flags48_&~1u);
        }
        Diagnostic callback;
        const auto stage=reference.service23CA50(*table,callback);
        callback.sourceEffectsCommitted=callback.sourceEffectsCommitted||d.sourceEffectsCommitted;
        d=callback;
        if(stage==Stage::blocked)return Outcome::blocked;
        if(stage==Stage::materialReturned)return Outcome::childrenOnly;
        if(!table->inspectLocalFlareEmission24B9B0(reference.material00_,output.material,d)){
            d.sourceEffectsCommitted=true;return Outcome::blocked;
        }
        d.sourceEffectsCommitted=true;
        for(std::size_t i=0;i<4;++i){
            const auto& alias=a.group_->aliases[i];
            if(alias.count!=1||alias.aliases[0].batch||alias.aliases[0].ordinal!=i||
               a.vertices_->positions[i].stripControl!=(i<2?-14:14))return reject(S::source,0x24b9e4);
            auto& vertex=output.vertices[i];
            for(std::size_t k=0;k<3;++k){
                const auto& p=a.vertices_->positions[i].xyz[k];
                const auto& n=a.vertices_->triples[i].components[k];
                if(!p.value||!n.value||!std::isfinite(*p.value)||!std::isfinite(*n.value))
                    return reject(S::arithmetic,0x24b9e4);
                vertex.position[k]=*p.value;vertex.triple[k]=*n.value;
            }
            for(std::size_t k=0;k<2;++k){const auto& uv=a.vertices_->texcoords[i].uv[k];
                if(!uv.value||!std::isfinite(*uv.value))return reject(S::arithmetic,0x24b9e4);
                vertex.uv[k]=*uv.value;
            }
        }
        output.color=a.common_->colorC0();
        for(float component:output.color)if(!std::isfinite(component))return reject(S::arithmetic,0x24b9e4);
        output.matrix40=a.common_->matrix40;output.matrix90=a.common_->matrix90;
        output.matrixE0=a.common_->matrixE0;output.vector130=a.common_->vector130;
        for(const auto* matrix:{&output.matrix40,&output.matrix90,&output.matrixE0})
            for(const auto& row:*matrix)for(float component:row)
                if(!std::isfinite(component))return reject(S::arithmetic,0x24b9e4);
        for(float component:output.vector130)if(!std::isfinite(component))return reject(S::arithmetic,0x24b9e4);
        output.strip=&strip;output.table=table;
        d.sourcePC=0x24b9b0;return Outcome::nativeLoweringRequired;
    }
};
}
