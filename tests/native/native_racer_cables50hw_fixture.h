#pragma once
namespace original_cable_selection50LA { class Binding; }
namespace racer_cold_visibility50CR { class Retirement; } // W0 round 2: one retirement for every course
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace ra_cold_aggregate50LA {class Owner;}
// Cold original-roster cable and flame adapters. Scene endpoints always come
// from the selected racer's own PSG; shared-looking source bytes are verified
// from that racer's retained RES members before being decoded.
#include "native_racer_rig50hw_fixture.h"
#include "source_cable_texture50fc_fixture.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/pod_flame50w.h"
#include <cstdio>
#include "../../src/game/pod_cables50fd.h"
#include "../../src/game/original_pod_cable_selection_state50la.h"
#include <map>
#include <limits>
#include "../../src/game/original_racer_strip_owners50la.h"

namespace native_racer_model50LA { class Owner; }

namespace native_racer50HW {
namespace visual_detail {
inline std::vector<std::uint8_t> original(const Rig& rig,const std::string& member){
    if(rig.directOriginal)return rig.originalMember(member);
    const auto wanted=rig_detail::lower(member);const J* selected=nullptr;
    for(const auto& row:rig.manifest.at("members"))
        if(rig_detail::lower(row.at("source_member").get<std::string>())==wanted){
            rig_detail::need(!selected,"Roster duplicate original visual member");selected=&row;}
    rig_detail::need(selected,"Roster missing original visual member: "+member);
    const std::filesystem::path path=selected->at("source_file").get<std::string>();
    rig_detail::need(!path.is_absolute()&&path==path.filename(),"Roster unsafe visual member path");
    auto bytes=f::readBytes(rig.directory/path);
    rig_detail::need(bytes.size()==selected->at("source_bytes").get<std::size_t>()&&
        pod_materials50ed_fixture::detail::sha256(bytes)==selected->at("source_sha256").get<std::string>(),
        "Roster original visual member identity differs: "+member);
    return bytes;
}
inline a::Material48E material(const Rig& rig,const std::string& name){
    const auto bytes=original(rig,"textures/"+rig_detail::lower(name)+".psm");
    a::Material48E out;a::DecodeError48C diagnostic{};
    rig_detail::need(a::decodeMaterial48E(bytes,out,diagnostic),"Roster original visual material decode");
    return out;
}
}

class Cables final {
    friend class ::original_cable_selection50LA::Binding;
    friend class ::racer_cold_visibility50CR::Retirement;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    friend class ::ra_cold_aggregate50LA::Owner;
    std::uint32_t nativeFifoPins_=0,cameraPins_=0;
    std::unique_ptr<g::OriginalRacerStripOwners50LA> lateStrips_;
    // Private native-only, one-shot outer-loader continuation. The sole caller
    // authenticates the active Bank.visit(0), same Session racer60 and pin.
    bool continueNativeOuter212DF0(g::OriginalRacerEntityCold50LA&,
        const native_racer_model50LA::Owner&,
        std::shared_ptr<g::OriginalApplicationEntities50LA>,std::uint32_t actualRacer60,
        std::uint32_t& pc)noexcept;
    bool nativeOuterAttempted_=false,nativeOuterReturned_=false;
    bool nativeOuterCablesAllocationAttempted_=false;
    g::OriginalPodCableAllocationResult50LA nativeOuterResult_{};
    class SourceCameraPin final {
        friend class Cables;
        Cables* owner_;
        explicit SourceCameraPin(Cables& owner):owner_(&owner){++owner.cameraPins_;}
    public:
        SourceCameraPin(const SourceCameraPin&)=delete;
        ~SourceCameraPin(){if(!owner_->cameraPins_)std::terminate();--owner_->cameraPins_;}
    };
    std::unique_ptr<SourceCameraPin> retainSourceCamera(const Rig& actualRig) {
        if(sourceRig_!=&actualRig||cameraPins_||nativeFifoPins_)return {};
        // Reuse only native holders whose actual source lifetimes have ended.
        // No live backlink or queued lease is erased. New173D1C starts50=null.
        auto* allocation=selectionState_.allocation_.get();
        if(allocation&&(!allocation->closed_||allocation->racer_||allocation->application_||allocation->fifoPins_))return {};
        if(lateStrips_&&(!lateStrips_->closed_||lateStrips_->racer_||lateStrips_->app_))return {};
        if(allocation){selectionState_.allocation_.reset();selectionState_.membership50_=0;}
        lateStrips_.reset();
        nativeOuterAttempted_=nativeOuterReturned_=nativeOuterCablesAllocationAttempted_=false;
        nativeOuterResult_={};
        return std::unique_ptr<SourceCameraPin>(new SourceCameraPin(*this));
    }
    const Rig* sourceRig_=nullptr;
    a::MaterialTextureAssets50EA bank_;
    g::PodCableBindings50FD bindings_{};
    // Actual211B60..211D54 pointer aliases, independent of mesh cableCount.
    std::array<Index,4> selectionFirst_{{Index::none,Index::none,Index::none,Index::none}};
    std::array<Index,4> selectionSecond_{{Index::none,Index::none,Index::none,Index::none}};
    // Actual vehicle254 PodCables owner:173D1C initializes50 to null;
    //174360 caches the actual7F0 identity and216AC8 clears the same field.
    // Native stable membership token replaces the original pointer, never a
    // collision-zone reference. This per-racer owner lives outside bank swaps.
    g::OriginalPodCableSelectionState50LA selectionState_;
public:
    Cables(const Rig& rig,const g::VehicleFrameState49W& state,
        const g::VehicleFrameBindings49W& frame){
        using rig_detail::need;
        need(g::validVehicleSceneCount49W(state,frame)&&state.sceneNodeCount==rig.nodeCount&&
            frame.lifecycle.model1B4==rig.roots[Rig::chariot]&&
            frame.lifecycle.left1BC==rig.roots[Rig::left]&&frame.lifecycle.right1B8==rig.roots[Rig::right],
            "Roster cable graph identity differs");
        const bool bottom=rig.modelCount==5;
        need((rig.modelCount==3||bottom)&&
            frame.visual.extra1C4Present==bottom&&frame.visual.extra1C0Present==bottom&&
            frame.visual.bottomLeft1C4==(bottom?rig.roots[Rig::bottomLeft]:Index::none)&&
            frame.visual.bottomRight1C0==(bottom?rig.roots[Rig::bottomRight]:Index::none),
            "Roster cable optional graph identity differs");
        for(unsigned model=0;model<rig.modelCount;++model)for(unsigned n=0;n<rig.hierarchy[model].nodeCount;++n){
            a::PsgNode49O source{};need(a::readPsgNode49O(rig.hierarchy[model],n,source),"Roster cable source node");
            const auto parent=source.parent<0?rig.roots[model]:rig.sourceNodes[model][source.parent];
            need(state.nodes[static_cast<unsigned>(rig.sourceNodes[model][n])].parent180==parent,
                "Roster cable source/current topology differs");
        }
        const char* ordinary[]={"chariotcableleft1","chariotcableright1"};
        const char* display[]={"drawcableleft1","drawcableright1"};
        const Index lifecycle[]={frame.lifecycle.leftCable1E0,frame.lifecycle.rightCable1E4};
        for(unsigned side=0;side<2;++side){
            const auto endpoint=rig.find(Rig::chariot,ordinary[side]);
            need(endpoint!=Index::none&&endpoint==lifecycle[side],"Roster cable lifecycle source binding");
            bindings_.chariot[side]=rig.find(Rig::chariot,display[side]);
            if(bindings_.chariot[side]==Index::none)bindings_.chariot[side]=endpoint;
        }
        bindings_.engine[0]=rig.find(Rig::left,"enginecableleft1");
        bindings_.engine[1]=rig.find(Rig::right,"enginecableright1");
        need(bindings_.engine[0]!=Index::none&&bindings_.engine[1]!=Index::none,"Roster cable engine anchors unavailable");
        if(bottom){
            bindings_.cableCount=4;
            bindings_.engine[2]=rig.find(Rig::bottomLeft,"enginecableleft2");
            bindings_.engine[3]=rig.find(Rig::bottomRight,"enginecableright2");
            need(bindings_.engine[2]!=Index::none&&bindings_.engine[3]!=Index::none,
                "Roster bottom cable engine anchors unavailable");
            const char* secondaryDisplay[]={"drawcableleft2","drawcableright2"};
            for(unsigned side=0;side<2;++side){
                bindings_.chariot[side+2]=rig.find(Rig::chariot,secondaryDisplay[side]);
                //211D0C/211D50 fall back to ordinary chariotcableleft1/right1,
                // not the primary draw node and not chariotcableleft2/right2.
                if(bindings_.chariot[side+2]==Index::none)bindings_.chariot[side+2]=lifecycle[side];
            }
        }
        const auto psm=visual_detail::original(rig,"textures/cable.psm");
        const auto pst=visual_detail::original(rig,"textures/cable.pst");
        // All eight actual RES members share these authenticated originals.
        // Decode this racer's exact bytes; do not read Pod01 as a fallback.
        // The stock cable.psm digest is a log-only receipt; the material is admitted by its
        // own structure below (StripOptFogCode, one map "cable"), for every pod.
        if(pod_materials50ed_fixture::detail::sha256(psm)!=
            "701039c97a0f4b74a65e90fdb1e386b40cbbb17bcedb2347196884234d0eee33")
            std::fprintf(stderr,"common_pod_file member=textures/cable.psm bytes=%zu receipt=differs\n",psm.size());
        a::Material48E material;a::DecodeError48C decode{};
        need(a::decodeMaterial48E(psm,material,decode),"Roster cable material decode");
        need(material.passes.size()==1&&material.passes[0].pass==1&&
            material.passes[0].microprogram=="StripOptFogCode"&&
            material.passes[0].textures.size()==1&&material.passes[0].textures[0].slot==1&&
            material.passes[0].textures[0].name=="cable","Roster cable source shader/texture binding");
        std::vector<std::uint8_t> texture;std::string textureError;
        need(source_cable_texture50fc_fixture::decodeCableTexture50FC(pst,texture,textureError),
            textureError.empty()?"Roster cable original texture decode":textureError);
        constexpr std::uint64_t identity=0x50fd00000001ull;
        const a::MaterialTextureAssetInput50EA textureInput{identity,&texture};
        const a::MaterialTextureSlotInput50EA slot{{identity,0,1,1},identity,a::MaterialTextureRequirement50EA::required};
        const a::MaterialTextureAssetsInput50EA input{&textureInput,1,&slot,1,{1,1,64u*1024u,64u*1024u,256}};
        a::MaterialTextureAssetDiagnostic50EA diagnostic{};
        need(bank_.load(input,diagnostic),"Roster cable immutable texture bank");
        selectionFirst_[0]=bindings_.chariot[0];selectionFirst_[1]=bindings_.chariot[1];
        selectionSecond_[0]=bindings_.engine[0];selectionSecond_[1]=bindings_.engine[1];
        for(unsigned side=0;side<2;++side){
            const unsigned primary=side?Rig::right:Rig::left;
            const unsigned lower=side?Rig::bottomRight:Rig::bottomLeft;
            const char* engine=side?"enginecableright2":"enginecableleft2";
            const char* draw=side?"drawcableright2":"drawcableleft2";
            selectionSecond_[side+2]=rig.find(bottom?lower:primary,engine);
            if(bottom||selectionSecond_[side+2]!=Index::none){
                selectionFirst_[side+2]=rig.find(Rig::chariot,draw);
                // Only the actual bottom-model branch has this fallback.
                if(bottom&&selectionFirst_[side+2]==Index::none)selectionFirst_[side+2]=lifecycle[side];
            }
        }
        sourceRig_=&rig;
    }
    ~Cables()noexcept {
        // Retained allocation alone must never outlive its persistent Cables
        // owner and raw racer backlink. Guard before any member destruction.
        if(nativeFifoPins_||cameraPins_)std::terminate();
        if(selectionState_.allocation_&&selectionState_.allocation_->close().status!=
           g::OriginalPodCableAllocationStatus50LA::closed)std::terminate();
        if(lateStrips_&&lateStrips_->close().status!=g::OriginalPodCableAllocationStatus50LA::closed)
            std::terminate();
    }
    Cables(const Cables&)=delete;Cables& operator=(const Cables&)=delete;
    Cables(Cables&&)=delete;Cables& operator=(Cables&&)=delete;
    const a::MaterialTextureAssets50EA* bank()const noexcept{return &bank_;}
    const g::PodCableBindings50FD& bindings()const noexcept{return bindings_;}
    std::uint32_t& visibility50() noexcept{return selectionState_.membership50_;}
    const g::OriginalPodCableSelectionState50LA& selectionState()const noexcept{return selectionState_;}
    // Inactive source-only prerequisite. Definition is in the explicit cold
    // adapter; ordinary mesh construction never invokes it. Close before the
    // containing racer/Application, while actual owners remain alive.
    g::OriginalPodCableAllocationResult50LA beginLateStripPrefix(
        g::OriginalRacerEntityCold50LA&,const native_racer_model50LA::Owner&,
        std::shared_ptr<g::OriginalApplicationEntities50LA>)noexcept;
    g::OriginalPodCableAllocationResult50LA beginSelectionAllocationPrefix(
        g::OriginalRacerEntityCold50LA&,const native_racer_model50LA::Owner& actualChariot,
        std::shared_ptr<g::OriginalApplicationEntities50LA>)noexcept;
    g::OriginalPodCableAllocationResult50LA closeSelectionAllocationPrefix()noexcept;
};

// Defined against the generic source-selected builder in pod_flame50w. The
// decoded material table is temporary; PodFlameOwners50W owns all retained POD
// uniforms/bindings and is safe to register in the racer snapshot bank.
inline a::PodFlameOwners50W buildColdFlames(const Rig& rig,
    const a::VehicleDrawAssets50CR& draw,const g::VehicleFrameBindings49W& bindings){
    rig_detail::need(draw.componentCount()==rig.modelCount&&rig.modelCount>=3&&rig.modelCount<=5,
        "Roster flame model count differs from actual draw assets");
    const unsigned modelCount=rig.modelCount-1;
    const unsigned rigModels[]={Rig::left,Rig::right,Rig::bottomLeft,Rig::bottomRight};
    const Index roots[]={bindings.visual.left1BC,bindings.visual.right1B8,
        bindings.visual.bottomLeft1C4,bindings.visual.bottomRight1C0};
    std::map<std::string,a::Material48E> decoded;
    std::vector<const a::Material48E*> materials[a::podFlameModelCapacity50W];
    a::PodFlameModelInput50W input[a::podFlameModelCapacity50W]{};
    for(unsigned model=0;model<modelCount;++model){
        const auto* scene=draw.scene(static_cast<g::VehicleDrawComponent50CM>(model+1));
        rig_detail::need(scene&&roots[model]!=Index::none&&roots[model]==rig.roots[rigModels[model]],
            "Roster flame model/source identity differs");
        materials[model].reserve(scene->materials.size());
        for(const auto& name:scene->materials){
            const auto identity=rig_detail::lower(name);
            auto found=decoded.find(identity);
            if(found==decoded.end())found=decoded.emplace(identity,visual_detail::material(rig,name)).first;
            materials[model].push_back(&found->second);
        }
        input[model]={scene,&materials[model],static_cast<unsigned>(roots[model])+1};
    }
    a::PodFlameOwners50W result{};a::PodFlameError50W diagnostic{};
    rig_detail::need(a::buildPodFlameOwners50W(input,modelCount,result,diagnostic),
        "Roster source-selected per-model flame owners rejected");
    return result;
}
}
