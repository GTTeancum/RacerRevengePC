#pragma once
#include "native_racer_bank50hn_fixture.h"
#include "original_racer_animation_setup50la_fixture.h"
#include "../../src/game/original_racer_animation_cold50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/original_racer_cold_scenes50la.h"
#include "../../src/game/original_pod_sphere_owner50la.h"
#include "../../src/game/original_racer_main_body_owner50la.h"
#include "../../src/game/original_racer_gravity_owner50la.h"
#include "../../src/game/original_racer_model_packets50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include "../../src/game/original_motion_library50la.h"
#include "../../src/game/engine_flare50gv.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include <array>
#include <cfenv>
#include <cstring>
#include <cstddef>
#include <new>
#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>

namespace native_racer_model50LA {
namespace g=rr::game;
namespace a=rr::assets;
namespace setup=original_racer_animation_setup50LA;
using Rig=native_racer50HW::Rig;

// One real native model owner, role-bound to right1A4/1B8, left1A8/1BC,
// optional POD02 bottomRight1AC/1C0 or bottomLeft1B0/1C4, or the separate
// chariot1A0/1B4 branch21191C.
// Its numeric model is the existing Rig
// subtree in the sealed bank, never another graph/vehicle. Cold source matrix
// setup happens while Rig installs that subtree. Canonical material/resource
// completion precedes the shared material scan; racer append happens later
// at the actual Session caller.
// This split is native lifetime staging, not full retail210770 chronology.
// The bounded Entity list projects three or five real owners in source-relative
// order, with concrete190/198/194 between the last engine and chariot.
// The no-engine combined model194 branch remains unadmitted.
class Owner final {
    friend class ::native_racer50HW::Cables;
    friend class ::original_cable_selection50LA::Binding;
    // Native lifetime staging of source cold operands, not shadow live fields.
    // Only these methods create them, on this retained model's construction
    // path before Rig normalization/motion/placement. No caller data/ready bit.
    struct ColdStack0F70 final {
        std::array<std::uint32_t,4> shared434630;
        std::array<std::uint32_t,4> stack0F70;
        const g::OriginalRacerSceneObject50LA* rightNullcg=nullptr;
    };
    struct ColdChariotLocal final {
        std::array<std::uint32_t,3> xyz; //28B180 writes no W lane.
        const g::OriginalRacerSceneObject50LA* nullcg=nullptr;
        const Owner* rightOwner=nullptr;
    };
    std::optional<ColdStack0F70> coldStack0F70_;
    std::optional<ColdChariotLocal> coldChariotLocal_;
    void captureCold434630BeforeModel(const g::SceneGraph49P& canonical){
        if(modelRole_!=Rig::right)return;
        failurePC_=0x210bbcu;
        need(!coldStack0F70_&&canonical.nodes==arena_->nodes&&canonical.count==rig_->nodeCount,
            "original cold434630 capture requires the same construction arena");
        ColdStack0F70 captured;
        std::memcpy(captured.shared434630.data(),arena_->visualShared.zero434630.v,16);
        captured.stack0F70=captured.shared434630; //210BCC full vector copy.
        coldStack0F70_=captured;
    }
    void captureColdNullcgAfterModel(const g::SceneGraph49P& canonical){
        if(modelRole_!=Rig::right&&modelRole_!=Rig::chariot)return;
        failurePC_=modelRole_==Rig::right?0x210cc4u:0x2119b8u;
        need(canonical.nodes==arena_->nodes&&canonical.count==rig_->nodeCount&&
            g::validateSceneGraph49P(canonical),"original cold nullcg capture graph mismatch");
        // At this boundary the real model subtree is constructed and restored
        // by1556C0. No fresh PSG read, row0 assumption or later Rig pose is used.
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> own{{&scene_,nullptr,nullptr,nullptr,nullptr}};
        const g::OriginalRacerSceneObject50LA* cg=nullptr;
        need(findBoundsNullcg28C590(scene_,canonical,own,cg)&&cg,
            "original cold retained nullcg lookup stopped");
        const auto index=static_cast<std::uint32_t>(cg->sceneIndex);
        if(modelRole_==Rig::right){
            need(coldStack0F70_&&!coldStack0F70_->rightNullcg,
                "original right nullcg cold input already captured or missing");
            failurePC_=0x210cd8u;
            //28B1A0(relative=null) obtains world XYZ before28AB90 normalization.
            // Existing graph refresh owns the real cache effects, not scratch.
            need(g::refreshSceneWorld49P(canonical,cg->sceneIndex),
                "original right nullcg cold world refresh stopped");
            std::memcpy(&coldStack0F70_->stack0F70[2],&canonical.nodes[index].world100.v[14],4);
            coldStack0F70_->rightNullcg=cg; //210CFC replaces only stack0F78.
        }else{
            const auto* right=peers_[Rig::right];
            need(!coldChariotLocal_&&right&&right->coldStack0F70_&&
                right->coldStack0F70_->rightNullcg&&right->rig_==rig_&&right->arena_==arena_&&
                right->bank_==bank_&&right->slot_==slot_&&right->generation_==generation_&&
                right->application_==application_,"original chariot cold right-owner provenance missing");
            ColdChariotLocal captured;captured.nullcg=cg;captured.rightOwner=right;
            std::memcpy(captured.xyz.data(),&canonical.nodes[index].localC0.v[12],12);
            coldChariotLocal_=captured; //2119C4 captures local XYZ; never world or W.
        }
        // These are retained input operands only. No213380/effect/190/194/IK
        // completion is asserted here. The owning chariot constructor immediately
        // publishes these operands to its bound canonical Fixture destinations.
    }
    static float coldFloat(std::uint32_t word)noexcept {
        float value;std::memcpy(&value,&word,4);return value;
    }
    static float coldLength(const float* xyz){
        need(std::fegetround()==FE_TONEAREST,"original cold length rounding rejected");
        volatile float xx=xyz[0]*xyz[0],yy=xyz[1]*xyz[1];
        volatile float xy=xx+yy,zz=xyz[2]*xyz[2],sum=xy+zz;
        need(std::isfinite(xx)&&std::isfinite(yy)&&std::isfinite(xy)&&
            std::isfinite(zz)&&std::isfinite(sum),"original cold length arithmetic rejected");
        std::uint32_t bits;const float input=sum;std::memcpy(&bits,&input,4);
        if((bits&0x7f800000u)==0)return coldFloat(bits&0x80000000u);
        volatile float result=static_cast<float>(std::sqrt(static_cast<double>(input)));
        need(std::isfinite(result),"original cold SQRT rejected");return result;
    }
    void publishColdTarget2119C4(){
        failurePC_=0x2119c4u;
        need(fixture_ && &fixture_->state==arena_&&modelRole_==Rig::chariot&&coldChariotLocal_,
            "original cold target destination lifetime unavailable");
        auto& length=arena_->fields.length858;
        using Stage=g::OriginalRacerLength85850LA::Stage;
        const auto* right=coldChariotLocal_->rightOwner;
        need(length.stage_==Stage::unwritten&&right&&right->fixture_==fixture_&&right->coldStack0F70_&&
            right->coldStack0F70_->rightNullcg,"original cold stack/destination already consumed or missing");
        need(std::fegetround()==FE_TONEAREST,"original target rounding rejected");
        auto& target=fixture_->secondaryInputs.targetRelative2F0;
        //28B180 writes only XYZ. Preserve the canonical existing2FC bits.
        std::memcpy(target.v,coldChariotLocal_->xyz.data(),12);
        for(unsigned lane=0;lane<3;++lane){
            failurePC_=0x2119d8u+lane*0x10u;
            const float operand=coldFloat(right->coldStack0F70_->stack0F70[lane]);
            volatile float result=target.v[lane]-operand;
            need(std::isfinite(result),"original2119CC target subtraction stopped");
            target.v[lane]=result; //Earlier source XYZ writes survive later failure.
        }
        failurePC_=0x211a0cu;const float result=coldLength(target.v);
        fixture_->parameters.lifecycle.restLength854=result; //211A28
        length.generation_=generation_;length.slot_=slot_;
        length.value_=result;length.stage_=Stage::initial211A2C; //211A2C
    }
    bool attachedFor(const g::OriginalRacerEntityCold50LA& racer)const noexcept {
        return modelReappended_&&racer_==&racer&&animation_.fields_.model04==&scene_&&
            (modelRole_==Rig::right?
                racer.animation1A4==&animation_&&racer.rightModel1B8==&scene_:
             modelRole_==Rig::left?
                racer.animation1A8==&animation_&&racer.leftModel1BC==&scene_:
             modelRole_==Rig::bottomRight?
                racer.animation1AC==&animation_&&racer.bottomRightModel1C0==&scene_:
             modelRole_==Rig::bottomLeft?
                racer.animation1B0==&animation_&&racer.bottomLeftModel1C4==&scene_:
                racer.animation1A0==&animation_&&racer.chariotModel1B4==&scene_);
    }
    //213380 is a canonical material-name pass, not aggregate-bounds production.
    // Reached by finish1538B0 immediately after the actual model alias append.
    // Completion covers only this pass, never intervening effects or late strips.
    g::OriginalRacerModelSceneResult50LA prepareMaterials213380()noexcept {
        using S=g::OriginalRacerModelSceneStatus50LA;
        if(!racer_||closed_||!attachedFor(*racer_)||!racer_->selectionMaterialTable_||
           materials_.table382B38()!=racer_->selectionMaterialTable_.get())return {S::phase,0x213380u};
        auto& table=*racer_->selectionMaterialTable_;const auto& fields=scene_.fields();
        if(fields.materialCount1C0<0||(fields.materialCount1C0&&!fields.materials1C4))
            return {S::binding,0x2133b4u};
        try {
            for(std::int32_t i=0;i<fields.materialCount1C0;++i){
                a::OriginalStandardMaterialRowView50LA row;
                if(!table.row(fields.materials1C4[i],row)||!row.name00)return {S::binding,0x298d90u};
                a::OriginalStandardMaterialDiagnostic50LA d;std::int32_t original=-1;
                if(!table.find298C80(*row.name00,original,d))return {S::binding,0x2133dcu};
                if(row.name00->find("ghost")!=std::string::npos)continue; //309BA8 substring, case-sensitive
                if(row.name00->size()>250u)return {S::binding,0x21340cu};
                const std::string ghost=*row.name00+"ghost"; //Original format "%sghost"
                (void)ghost; //Suffix is not a replacement-material ownership operation.
                if(racer_->mode3C_!=0)return {S::phase,0x213428u};
                //Real normal mode branches213414->213460: no lookup/retain/override.
            }
            return {S::complete,0x213478u};
        }catch(...){return {S::allocation,0x21340cu};}
    }
    // Private2286F0..228724 source cut. No runtime caller is installed here.
    // This owner already binds the actual slot/arena/racer and genuine peers;
    // no caller flag, bounds, matrix or replacement control record is accepted.
    g::OriginalRacerModelSceneResult50LA prepareBounds228704()noexcept {
        using S=g::OriginalRacerModelSceneStatus50LA;
        using R=g::OriginalRacerModelSceneResult50LA;
        if(modelRole_!=Rig::chariot||!bank_||!arena_||!racer_||!finishedFor(*racer_))
            return {S::phase,0x2286f0u};
        try {return bank_->visit(slot_,[&]()->R {
            auto& flags=arena_->bodies.main.controls.player.flagsB9C;
            if(flags&0x80u)return {S::complete,0x228724u}; //Original gate, no repair.
            const auto current=graph();
            if(arena_->sceneNodeCount!=rig_->nodeCount||!g::validateSceneGraph49P(current))
                return {S::graph,0x217440u};
            const std::array<unsigned,5> order{{Rig::chariot,Rig::right,Rig::left,
                Rig::bottomRight,Rig::bottomLeft}};
            const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> models{{
                racer_->chariotModel1B4,racer_->rightModel1B8,racer_->leftModel1BC,
                racer_->bottomRightModel1C0,racer_->bottomLeftModel1C4}};
            const std::array<std::uint32_t,5> lookups{{0x217464u,0x2174e0u,0x21755cu,0x2175d8u,0x217654u}};
            const std::array<std::uint32_t,5> setters{{0x217478u,0x2174f4u,0x217570u,0x2175ecu,0x217668u}};
            for(unsigned n=0;n<order.size();++n){
                if(!models[n])continue; //Actual1B4/1B8/1BC/1C0/1C4 null branch.
                auto* owner=order[n]==modelRole_?this:peers_[order[n]];
                if(!owner||!owner->finishedFor(*racer_)||&owner->scene_!=models[n]||
                   owner->bank_!=bank_||owner->slot_!=slot_||owner->arena_!=arena_||
                   owner->rig_!=rig_||owner->generation_!=generation_||
                   owner->application_!=application_)return {S::binding,lookups[n]};
                const g::OriginalRacerSceneObject50LA* cg=nullptr;
                if(!findBoundsNullcg28C590(*models[n],current,models,cg)||!cg)
                    return {S::sourceRowTable,lookups[n]};
                g::SceneDisplacement49P zero;
                // SAME existing live434630 owner used by49R/49V and banked state.
                std::memcpy(zero.v,arena_->visualShared.zero434630.v,sizeof zero.v);
                if(!g::setLocalTranslation49P(current,cg->sceneIndex,zero))
                    return {S::graph,setters[n]};
                auto& cgNode=current.nodes[static_cast<std::uint32_t>(cg->sceneIndex)];
                cgNode.sceneFlags190=static_cast<std::uint8_t>(cgNode.sceneFlags190&~0x20u);
                // Source loads animation.model04 AFTER translation and flag20.
                // Preserve those mutations if this later ownership boundary fails.
                if(owner->animation_.fields_.model04!=&owner->scene_||!owner->scene_.model())
                    return {S::binding,setters[n]+0x34u};
                const auto root=owner->scene_.model()->sceneIndex;
                const auto index=static_cast<std::uint32_t>(root);
                if(index>=current.count)return {S::graph,setters[n]+0x34u};
                current.nodes[index].sceneFlags190=static_cast<std::uint8_t>(
                    current.nodes[index].sceneFlags190&~0x40u);
                // Already inside the sole Bank visit: never call a visiting wrapper.
                auto result=owner->scene_.publishAggregate28E110(current);
                if(!result.okay())return result;
            }
            flags=static_cast<std::uint8_t>(flags|0x80u); //22870C..228720 only after return.
            return {S::complete,0x228724u};
        });}catch(...){return {S::graph,0x228704u};}
    }
    static bool findBoundsNullcg28C590(const g::OriginalRacerModelSceneOwner50LA& start,
        const g::SceneGraph49P& graph,
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5>& models,
        const g::OriginalRacerSceneObject50LA*& found,const char* wanted="nullcg")noexcept {
        found=nullptr;if(!start.model()||start.released()||!start.sourceRowCount())return false;
        std::size_t remaining=graph.count;
        auto equal=[wanted](const std::string& name)noexcept {
            const auto size=std::strlen(wanted);
            if(name.size()!=size)return false;
            for(std::size_t i=0;i<size;++i){auto c=name[i];if(c>='A'&&c<='Z')c=char(c+32);
                if(c!=wanted[i])return false;}return true;
        };
        auto walk=[&](auto&& self,g::SceneNodeIndex49P index)->bool {
            if(index==g::SceneNodeIndex49P::none)return true;
            const auto n=static_cast<std::uint32_t>(index);
            if(!remaining||n>=graph.count)return false;--remaining;
            const g::OriginalRacerSceneObject50LA* object=nullptr;
            for(const auto* model:models)if(model){
                if(model->released())return false;
                if(model->model()&&model->model()->sceneIndex==index)object=model->model();
                for(std::uint32_t j=0;!object&&j<model->sourceRowCount();++j){
                    const auto* row=model->sourceRow(j);if(row&&row->sceneIndex==index)object=row;}
                if(object)break;
            }
            if(!object||!object->constructed||object->released)return false;
            if(equal(object->name38)){found=object;return true;}
            // Original28C590: self, next sibling188, then first child184.
            if(!self(self,graph.nodes[n].nextSibling188))return false;
            if(found)return true;
            return self(self,graph.nodes[n].firstChild184);
        };
        return walk(walk,start.model()->sceneIndex);
    }
    // Early staging owns these exact allocations until Session publishes them
    // into the actual racer. No scene reference is acquired by28EB90.
    using ColdWristAllocation=g::OriginalRacerWristAllocation50LA;
    struct ColdWrist final {
        enum class Stage {unreached,absent,allocationNull,allocated,constructed};
        Stage stage=Stage::unreached;
        std::unique_ptr<ColdWristAllocation> allocation;
        const g::OriginalRacerSceneObject50LA *shoulder=nullptr,*elbow=nullptr,*wrist=nullptr;
    };
    std::array<ColdWrist,2> coldWrists_;
    void constructColdWrists211A4C(const g::SceneGraph49P& canonical){
        need(modelRole_==Rig::chariot&&coldScenes_.parented_&&
            canonical.nodes==arena_->nodes&&canonical.count==rig_->nodeCount,
            "original optional wrist construction requires actual190/194 and chariot");
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> own{{&scene_,nullptr,nullptr,nullptr,nullptr}};
        auto object=[&](g::SceneNodeIndex49P index)->const g::OriginalRacerSceneObject50LA* {
            if(index==g::SceneNodeIndex49P::none)return nullptr;
            if(scene_.model()->sceneIndex==index)return scene_.model();
            for(std::uint32_t i=0;i<scene_.sourceRowCount();++i){
                const auto* row=scene_.sourceRow(i);if(row&&row->sceneIndex==index)return row;}
            return nullptr;
        };
        for(unsigned i=0;i<2;++i){
            auto& owner=coldWrists_[i];failurePC_=i?0x211ab0u:0x211a4cu;
            need(owner.stage==ColdWrist::Stage::unreached,"original wrist constructor cannot replay");
            need(findBoundsNullcg28C590(scene_,canonical,own,owner.wrist,i?"handrt":"handlt"),
                "original wrist name lookup topology stopped");
            if(!owner.wrist){owner.stage=ColdWrist::Stage::absent;continue;}
            const auto parent=canonical.nodes[static_cast<unsigned>(owner.wrist->sceneIndex)].parent180;
            if(parent==g::SceneNodeIndex49P::none){owner.stage=ColdWrist::Stage::absent;continue;}
            owner.elbow=object(parent);need(owner.elbow,"original wrist parent lacks retained scene identity");
            const auto grandparent=canonical.nodes[static_cast<unsigned>(parent)].parent180;
            if(grandparent==g::SceneNodeIndex49P::none){owner.stage=ColdWrist::Stage::absent;continue;}
            owner.shoulder=object(grandparent);need(owner.shoulder,"original wrist grandparent lacks retained scene identity");
            failurePC_=i?0x211adcu:0x211a78u;
            try{owner.allocation=std::make_unique<ColdWristAllocation>();}
            catch(const std::bad_alloc&){owner.stage=ColdWrist::Stage::allocationNull;continue;}
            owner.stage=ColdWrist::Stage::allocated;
            g::ChariotIkVector49T axis;
            std::memcpy(axis.v,application_->binderTriple434650_.data(),sizeof axis.v);
            if(i){for(unsigned lane=0;lane<3;++lane)axis.v[lane]=-axis.v[lane];axis.v[3]=1.f;}
            const g::ChariotIkChainBinding49T binding{owner.shoulder->sceneIndex,owner.elbow->sceneIndex,
                owner.wrist->sceneIndex,g::SceneNodeIndex49P::none};
            g::ChariotIkWorkspace49T work{};g::ChariotIkDiagnostic49T diagnostic{};
            failurePC_=i?0x211b34u:0x211a9cu;
            need(g::initializeChariotIkRecord50LA(binding,axis,canonical,owner.allocation->record,
                arena_->animationShared.libm,work,diagnostic),"original separate wrist28EB90 numeric boundary stopped");
            owner.stage=ColdWrist::Stage::constructed; //211AA4 or211B3C publication.
        }
    }
    void closeUnpublishedColdWrists()noexcept {
        // Native pre-Session exceptional unwind only. Published allocations
        // have moved to racer208/20C and survive all model/scene child closes.
        for(auto& owner:coldWrists_)owner.allocation.reset();
    }
    runtime_startup50fl::Fixture* fixture_=nullptr; //Exact canonical destination lifetime.
    native_racer50HN::Bank* bank_=nullptr;
    g::VehicleFrameState49W* arena_=nullptr;
    std::size_t slot_=0;
    unsigned modelRole_=Rig::right;
    const Rig* rig_=nullptr;
    // Indexed by the existing five Rig roles; the own-role entry stays null.
    // Stable peers borrow the same bank/graph/domain and own no second model.
    std::array<Owner*,5> peers_{};
    inline static constexpr std::array<unsigned,5> sourceRoles_{{
        Rig::right,Rig::left,Rig::bottomRight,Rig::bottomLeft,Rig::chariot}};
    static unsigned sourceRank(unsigned role)noexcept {
        for(unsigned rank=0;rank<sourceRoles_.size();++rank)
            if(sourceRoles_[rank]==role)return rank;
        return static_cast<unsigned>(sourceRoles_.size());
    }
    std::uint64_t generation_=0;
    std::shared_ptr<g::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<g::OriginalMotionLibrary50LA> motionLibrary_;
    std::shared_ptr<a::OriginalStandardTextureResolver50LA> files_;
    a::OriginalStandardModelFileId50LA mcfId_,psgId_;
    std::string* modelDirectory36B860_=nullptr;
    std::string savedModelDirectory_,psgRequest_;
    bool modelDirectorySaved_=false;
    g::OriginalRacerAnimationCold50LA animation_;
    g::OriginalRacerModelSceneOwner50LA scene_;
    g::OriginalRacerColdScenes50LA coldScenes_;
    std::unique_ptr<g::OriginalRacerMainBodyOwner50LA> body340_;
    bool bodyAllocationAttempted_=false;
    std::unique_ptr<g::OriginalRacerGravityOwner50LA> gravity344_;
    bool gravityAllocationAttempted_=false;
    std::unique_ptr<g::OriginalPodSphereOwner50LA> joint360_;
    bool jointAllocationAttempted_=false;
    std::unique_ptr<setup::Setup> setup_;
    std::unique_ptr<setup::Input> input_;
    std::unique_ptr<setup::Archive> archive_;
    std::unique_ptr<setup::Ini> configuration08_;
    g::OriginalModelMotionSet50LA motions_;
    std::shared_ptr<a::OriginalStandardModelResource50LA> payload_;
    std::shared_ptr<a::OriginalStandardModelGeometry50LA> geometry_;
    a::OriginalStandardMaterialReferences50LA materials_;
    g::OriginalRacerModelPackets50LA packets_;
    g::OriginalRacerEntityCold50LA* racer_=nullptr;
    a::PodFlameOwners50W* bankedFlame_=nullptr;
    bool cachePublished_=false,graphStarted_=false,modelAppended_=false;
    bool resourcesComplete_=false,animationAttached_=false,modelReappended_=false,finished_=false,closed_=false;
    std::uint32_t failurePC_=0;
    static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    g::OriginalWreckComponent50LA& animationEntity()noexcept{return animation_.entity220_;}
    std::array<const g::OriginalRacerModelSceneOwner50LA*,4> peerScenes()const noexcept {
        std::array<const g::OriginalRacerModelSceneOwner50LA*,4> out{};
        std::size_t count=0;
        for(const auto role:sourceRoles_){const auto* peer=peers_[role];
            if(peer&&peer->scene_.constructed2403A0())out[count++]=&peer->scene_;}
        return out;
    }
    void restoreModelDirectory1543A8()noexcept {
        if(modelDirectorySaved_){modelDirectory36B860_->swap(savedModelDirectory_);modelDirectorySaved_=false;}
    }
    g::SceneGraph49P graph()const noexcept{return {arena_->nodes,rig_->nodeCount};}
    g::OriginalWreckComponentDomain50LA outside(
        std::array<g::OriginalWreckComponent50LA*,15>& storage)noexcept {
        std::size_t count=0;storage[count++]=&animationEntity();
        for(auto* peer:peers_)if(peer&&peer->animation_.constructed1536F0())
            storage[count++]=&peer->animationEntity();
        if(racer_){
            storage[count++]=&racer_->racer;storage[count++]=&racer_->overlay240.component;
            storage[count++]=&racer_->particle7FC;storage[count++]=&racer_->detached7F8;
        }
        if(body340_&&body340_->liveEntity())storage[count++]=body340_->liveEntity();
        for(auto* peer:peers_)if(peer&&peer->body340_&&peer->body340_->liveEntity())
            storage[count++]=peer->body340_->liveEntity();
        if(gravity344_&&gravity344_->liveEntity())storage[count++]=gravity344_->liveEntity();
        for(auto* peer:peers_)if(peer&&peer->gravity344_&&peer->gravity344_->liveEntity())
            storage[count++]=peer->gravity344_->liveEntity();
        if(joint360_&&joint360_->liveEntity())storage[count++]=joint360_->liveEntity();
        for(auto* peer:peers_)if(peer&&peer->joint360_&&peer->joint360_->liveEntity())
            storage[count++]=peer->joint360_->liveEntity();
        coldScenes_.addDomain(storage.data(),count);
        for(auto* peer:peers_)if(peer)peer->coldScenes_.addDomain(storage.data(),count);
        // Scene methods add the at-most-five real Model Entity8 addresses privately.
        // No released peer or stale model alias becomes a new live object.
        return {generation_,storage.data(),count};
    }
    bool admitAnimationClose()noexcept {
        // A later partially prepared owner must retire before an earlier
        // animation can consume its surviving model alias, even if the later
        // Session append was never reached. Successful close severs the peer.
        for(const auto* peer:peers_)if(peer&&sourceRank(peer->modelRole_)>sourceRank(modelRole_))
            return false;
        if(!animation_.constructed1536F0())return true;
        const auto& c=animationEntity();
        const auto* next=body340_&&body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::appended?
            body340_->liveEntity():nullptr;
        if(c.reference00!=1u||c.next14!=next||
           (modelAppended_?c.firstChild18!=scene_.modelEntity8():c.firstChild18!=nullptr))return false;
        if(!animationAttached_)return !c.parent1C&&!c.previous10;
        if(!racer_||c.parent1C!=&racer_->racer||c.mask20!=2u)return false;
        auto* previous=&racer_->particle7FC;
        for(const auto role:sourceRoles_){
            if(role==modelRole_)break;
            auto* peer=peers_[role];
            if(!peer){
                if((role==Rig::bottomRight||role==Rig::bottomLeft)&&rig_->modelCount==3u)continue;
                return false;
            }
            if(!peer->finishedFor(*racer_))return false;
            previous=&peer->animationEntity();
        }
        // Source142590/1424E0 closes later animation siblings first. In this
        // bounded projection the reverse is C, BL, BR, L, R (or C, L, R),
        // before each earlier owner consumes its surviving model alias.
        if(modelRole_==Rig::chariot){
            if(coldScenes_.appended_!=3)return false;
            previous=coldScenes_.lastEntity();
        }
        return c.previous10==previous&&previous->next14==&c;
    }
    bool closeOnCurrentGraph()noexcept {
        // Native exceptional unwind must restore the same shared source path.
        restoreModelDirectory1543A8();
        if(closed_)return true;
        if(!application_) {closed_=true;return true;}
        if(application_->attachmentLock423F58())return false;
        try {
            // Admit the complete motion release before ANY scene/model
            // mutation; a failure must leave the animation alias retryable.
            const auto motionAdmission=motions_.preflightRelease153F50();
            if(!motionAdmission.okay()){failurePC_=motionAdmission.sourcePC;return false;}
            if(!admitAnimationClose()){failurePC_=0x142590u;return false;}
            const auto* bodyTail=body340_&&body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::appended?
                body340_->liveEntity():nullptr;
            const auto* gravityTail=gravity344_&&gravity344_->appended()?gravity344_->liveEntity():nullptr;
            const auto* jointTail=joint360_&&joint360_->stage_==g::OriginalPodSphereOwner50LA::Stage::appended?
                joint360_->liveEntity():nullptr;
            if(body340_&&!body340_->preflightClose(racer_?&racer_->racer:nullptr,&animationEntity(),gravityTail)){
                failurePC_=0x142590u;return false;
            }
            if(gravity344_&&!gravity344_->preflightClose(racer_?&racer_->racer:nullptr,bodyTail,jointTail)){
                failurePC_=0x142590u;return false;
            }
            if(joint360_&&!joint360_->preflightClose(racer_?&racer_->racer:nullptr,gravityTail)){
                failurePC_=0x142590u;return false;
            }
            closeUnpublishedColdWrists();
            if(joint360_){
                const auto released=joint360_->close142590(racer_?&racer_->racer:nullptr,gravityTail);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){
                    failurePC_=released.sourcePC;return false;
                }
            }
            if(gravity344_){
                const auto released=gravity344_->close142590(racer_?&racer_->racer:nullptr,bodyTail);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){
                    failurePC_=released.sourcePC;return false;
                }
            }
            if(body340_){
                const auto released=body340_->close142590(racer_?&racer_->racer:nullptr,&animationEntity());
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){
                    failurePC_=released.sourcePC;return false;
                }
            }
            if(scene_.modelEntity8()&&!scene_.released()){
                if(!modelAppended_){
                    const auto r=scene_.closeFreshUnlinked(graph());
                    if(!r.okay()){failurePC_=r.sourcePC;return false;}
                }else{
                    std::array<g::OriginalWreckComponent50LA*,15> storage{};
                    const auto d=outside(storage);
                    const auto peers=peerScenes();
                    const auto admission=scene_.preflightScene2896D0(graph());
                    if(!admission.okay()){failurePC_=admission.sourcePC;return false;}
                    const auto referenceAdmission=scene_.preflightModelReference142590(
                        d,peers[0],peers[1],peers[2],peers[3]);
                    if(!referenceAdmission.okay()){failurePC_=referenceAdmission.sourcePC;return false;}
                    if(!materials_.canRelease26E4C0()||
                       (geometry_&&geometry_->constructed240110()&&
                        !geometry_->canReleaseForModel(&scene_)))return false;
                    if(packets_.constructed240740()&&!packets_.preflight240450().okay())return false;
                    // The bank owns only a borrowed clone pointer. Remove it
                    // before the final26E4C0 release can destroy that material.
                    if(bankedFlame_){g::clearCanonicalEngineFlareBorrows50LA(*bankedFlame_,scene_);
                        bankedFlame_=nullptr;}
                    const auto reference=scene_.releaseModelReference142590(
                        d,peers[0],peers[1],peers[2],peers[3]);
                    if(!reference.okay()){failurePC_=reference.sourcePC;return false;}
                    //142590 reached the model through animation18, but repairs
                    // its CURRENT parent7F8. The old animation alias survives
                    // until the enclosing animation is disposed immediately.
                    if(packets_.constructed240740()&&!packets_.release240450().okay())return false;
                    a::OriginalStandardMaterialDiagnostic50LA md;
                    if(!materials_.release26E4C0(md)){failurePC_=md.sourcePC;return false;}
                    if(geometry_&&geometry_->constructed240110()){a::OriginalStandardResourceDiagnostic50LA rd;
                        if(!geometry_->release26E598(&scene_,materials_,rd)){
                            failurePC_=rd.sourcePC;return false;}}
                    const auto freed=scene_.freeSourceRowTable26E5CC();
                    if(!freed.okay()){failurePC_=freed.sourcePC;return false;}
                    const auto released=scene_.releaseSceneBase2896D0(graph());
                    if(!released.okay()){failurePC_=released.sourcePC;return false;}
                }
            }
            if(animation_.constructed1536F0()){
                auto& c=animationEntity();
                if(c.reference00!=1u||c.next14||
                   (modelReappended_&&c.firstChild18!=scene_.modelEntity8())||
                   (!modelReappended_&&c.firstChild18))return false;
                if(c.previous10)c.previous10->next14=c.next14;
                if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
                --c.reference00; //104970 ->156FF0 ->153F50.
                animation_.fields_.motion18=nullptr;
                if(setup_)setup_->releaseBeforeMotion153FA4();
                const auto motionRelease=motions_.release153F50();
                if(!motionRelease.okay()){failurePC_=motionRelease.sourcePC;return false;}
                animation_.fields_.count208=0;
                if(setup_)setup_->releaseAfterMotion1540C4();
                if(cachePublished_)files_->clearAnimation1327D0(&animation_);
                --application_->objectCount423F60();
                animation_.released_=true;
            }
            const auto coldClosed=coldScenes_.close(graph());
            if(!coldClosed.okay()){failurePC_=coldClosed.sourcePC;return false;}
            // Native partial-construction unwind also releases a still-live
            // temporary config. Completed1538B0 already disposed it1573B0.
            configuration08_.reset();input_.reset();archive_.reset();closed_=true;
            // Native peer bookkeeping only. Source parent/previous/head,
            // racer1A4/1A8/1AC/1B0/1A0 and model1B8/1BC/1C0/1C4/1B4 stay stale.
            for(auto*& peer:peers_)if(peer){peer->peers_[modelRole_]=nullptr;peer=nullptr;}
            return true;
        }catch(...){return false;}
    }
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    void prepareEarly(const Rig& rig,std::size_t slot,std::uint64_t generation,
        native_racer50HN::Bank& bank,runtime_startup50fl::Fixture& live,
        std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<g::OriginalMotionLibrary50LA> motionLibrary,
        std::string& modelDirectory36B860,unsigned modelRole=Rig::right){
        need(!rig_&&generation&&application&&domain&&domain->prepared()&&motionLibrary&&
            (modelRole==Rig::right||modelRole==Rig::left||modelRole==Rig::chariot||
                ((modelRole==Rig::bottomRight||modelRole==Rig::bottomLeft)&&rig.modelCount==5u))&&
            modelRole<rig.modelCount,
            "native original model cold owner binding rejected");
        rig_=&rig;slot_=slot;generation_=generation;bank_=&bank;fixture_=&live;arena_=&live.state;modelRole_=modelRole;
        application_=std::move(application);domain_=std::move(domain);
        modelDirectory36B860_=&modelDirectory36B860;
        files_=domain_->resolver3E1A70();motionLibrary_=std::move(motionLibrary);
        need(bool(files_),"native original model requires shared application file owner");
        a::OriginalStandardTextureDiagnostic50LA fd;
        need(files_->bindModelMember(rig.mcfMembers[modelRole_],a::OriginalStandardModelFileKind50LA::mcf,
                rig.mcf[modelRole_],mcfId_,fd)&&
            files_->bindModelMember(rig.psgMembers[modelRole_],a::OriginalStandardModelFileKind50LA::psg,
                rig.psg[modelRole_],psgId_,fd),"original model authenticated cache binding rejected");
        a::OriginalStandardModelFileView50LA mcf,psg;
        need(files_->modelFile(mcfId_,mcf,fd)&&files_->modelFile(psgId_,psg,fd),
            "original model cache rows disappeared");
        // This useful first packet admits the actual fresh route. A genuine
        // warm result is a boundary, never forcibly cleared or called a miss.
        if(mcf.animation1536F0){failurePC_=0x153c20u;
            throw std::runtime_error("original live MCF warm156300 requires a distinct clone owner: "+
                rig.mcfMembers[modelRole_]+" row="+std::to_string(mcfId_.row));}
        if(psg.geometry240110){failurePC_=0x26ea28u;
            throw std::runtime_error("original live PSG warm26EA28 requires geometry retain/hierarchy clone: "+
                rig.psgMembers[modelRole_]+" row="+std::to_string(psgId_.row));}
        const auto constructed=animation_.constructFresh1536F0({generation_,key(&animation_),
            &application_->objectCount423F60(),&application_->attachmentLock423F58()});
        need(constructed.okay(),"original1536F0 animation constructor failed");
        animation_.fields_.field18C=1;
        need(files_->publishAnimation1326A0(mcfId_,&animation_,fd),"original MCF cache publication failed");
        cachePublished_=true;
        archive_=std::make_unique<setup::Archive>(rig);
        a::OriginalStandardArchiveProvider50LA::Diagnostic providerDiagnostic;
        need(domain_->archiveProvider3E1A70()&&
            archive_->mount(*domain_->archiveProvider3E1A70(),providerDiagnostic),
            "original Rig complete archive mount rejected");
        input_=std::make_unique<setup::Input>(setup::readInput1538B0(rig,modelRole_,*archive_));
        configuration08_=std::make_unique<setup::Ini>(input_->parsedConfiguration);
        animation_.fields_.configuration08=configuration08_.get();
        const auto& c=input_->configuration;
        auto& f=animation_.fields_;
        f.definition48=c.definition48;f.rig138=c.slt138;f.rigOverride188=c.rigOverride188;
        f.directory190=c.directory190;f.model98=c.vut98;f.motionE8=c.mirrorE8;
        f.motionCount14=c.motionCount14;f.field20=static_cast<std::uint32_t>(c.interpolatePosition20);
        f.field24=static_cast<std::uint32_t>(c.interpolateRotation24);
        f.field28=c.rotationType28;f.field2C=c.positionType2C;f.field0C=c.mirrorAxis0C;
    }
private:
    void admitPeer(const Owner& other)const {
        need(&other!=this&&rig_&&rig_==other.rig_&&
            slot_==other.slot_&&bank_==other.bank_&&arena_==other.arena_&&
            generation_==other.generation_&&application_==other.application_&&
            domain_==other.domain_&&files_==other.files_&&motionLibrary_==other.motionLibrary_&&
            modelDirectory36B860_==other.modelDirectory36B860_&&modelRole_!=other.modelRole_&&
            animation_.constructed1536F0()&&other.animation_.constructed1536F0()&&
            !graphStarted_&&!other.graphStarted_&&!racer_&&!other.racer_&&!closed_&&!other.closed_,
            "original model peer binding rejected");
        const auto rows=rig_->hierarchy[modelRole_].nodeCount;
        const auto peerRows=rig_->hierarchy[other.modelRole_].nodeCount;
        need(rows&&peerRows&&rows<=a::maxModelNodes49O&&peerRows<=a::maxModelNodes49O&&
            rig_->roots[modelRole_]!=rig_->roots[other.modelRole_],
            "original model peers require distinct canonical model ranges");
        for(std::uint32_t row=0;row<rows;++row){
            const auto node=rig_->sourceNodes[modelRole_][row];
            need(node!=rig_->roots[other.modelRole_],"original model source row aliases peer root");
            for(std::uint32_t peerRow=0;peerRow<peerRows;++peerRow)
                need(node!=rig_->sourceNodes[other.modelRole_][peerRow],
                    "original model source rows overlap");
        }
        for(std::uint32_t row=0;row<peerRows;++row)
            need(rig_->roots[modelRole_]!=rig_->sourceNodes[other.modelRole_][row],
                "original model root aliases peer source row");
    }
    bool graphOrderAdmitted()const noexcept {
        if(rig_->modelCount==5u)
            for(const auto role:sourceRoles_)if(role!=modelRole_&&!peers_[role])return false;
        for(const auto* peer:peers_)if(peer){
            if(peer->modelDirectorySaved_)return false;
            const bool earlier=sourceRank(peer->modelRole_)<sourceRank(modelRole_);
            if(earlier){if(!peer->setup_||!peer->setup_->restored1556C0())return false;}
            else if(peer->graphStarted_)return false;
        }
        return true;
    }
    bool attachmentOrderAdmitted(const g::OriginalRacerEntityCold50LA& racer)const noexcept {
        for(const auto role:sourceRoles_){
            if(role==modelRole_)continue;
            const auto* peer=peers_[role];
            if(!peer){
                if((role==Rig::bottomRight||role==Rig::bottomLeft)&&rig_->modelCount==3u)continue;
                // Preserve the upper right/left staging before the optional
                // separate-chariot join, but never omit an earlier owner.
                if(rig_->modelCount==3u&&sourceRank(role)>sourceRank(modelRole_))continue;
                return false;
            }
            if(sourceRank(role)<sourceRank(modelRole_)){
                if(!peer->finishedFor(racer))return false;
            }else if(peer->animationAttached_)return false;
        }
        return true;
    }
public:
    // Preserve the original two-engine admission. Chariot joins separately
    // only after both exact engine peers have formed this stable pair.
    void bindSibling(Owner& other){
        need((modelRole_==Rig::right&&other.modelRole_==Rig::left)||
             (modelRole_==Rig::left&&other.modelRole_==Rig::right),
            "original engine sibling roles rejected");
        for(const auto* peer:peers_)need(!peer,"original engine already has a peer");
        for(const auto* peer:other.peers_)need(!peer,"original engine peer already bound");
        admitPeer(other);
        peers_[other.modelRole_]=&other;other.peers_[modelRole_]=this;
    }
    void bindChariotSiblings(Owner& right,Owner& left,
        Owner* bottomRight=nullptr,Owner* bottomLeft=nullptr){
        need(rig_&&modelRole_==Rig::chariot&&right.modelRole_==Rig::right&&left.modelRole_==Rig::left&&
            right.peers_[Rig::left]==&left&&left.peers_[Rig::right]==&right&&
            bool(bottomRight)==bool(bottomLeft)&&
            (bottomRight?rig_->modelCount==5u:rig_->modelCount==3u),
            "original separate chariot requires its actual engine pair");
        for(unsigned role=0;role<peers_.size();++role){
            need(right.peers_[role]==(role==Rig::left?&left:nullptr)&&
                 left.peers_[role]==(role==Rig::right?&right:nullptr),
                "original engine pair already has additional peers");
        }
        for(const auto* peer:peers_)need(!peer,"original chariot peers already bound");
        if(bottomRight){
            need(bottomRight->modelRole_==Rig::bottomRight&&bottomLeft->modelRole_==Rig::bottomLeft,
                "original lower engine peer roles rejected");
            for(const auto* peer:bottomRight->peers_)need(!peer,"original bottom-right peers already bound");
            for(const auto* peer:bottomLeft->peers_)need(!peer,"original bottom-left peers already bound");
        }
        const std::array<Owner*,5> owners{{&right,&left,bottomRight,bottomLeft,this}};
        // All pairwise identity/domain/range admissions precede mutation.
        // Five owners borrow one another; none owns or clones another model.
        for(std::size_t i=0;i<owners.size();++i)if(owners[i])
            for(std::size_t j=0;j<i;++j)if(owners[j])owners[i]->admitPeer(*owners[j]);
        for(auto* owner:owners)if(owner)
            for(auto* peer:owners)if(peer&&peer!=owner)owner->peers_[peer->modelRole_]=peer;
    }
    // Called only by the actual Rig::install before its generic model stores.
    // Other Rig models retain their existing cold implementation.
    void constructGraph(const g::SceneGraph49P& canonical){
        failurePC_=0x154140u;
        need(rig_&&input_&&!graphStarted_&&canonical.nodes==arena_->nodes&&
            canonical.count==rig_->nodeCount&&graphOrderAdmitted()&&
            (modelRole_!=Rig::chariot||(peers_[Rig::right]&&peers_[Rig::left])),
            "original model graph construction order rejected");
        captureCold434630BeforeModel(canonical);
        const auto& c=input_->configuration;
        if(!c.rigOverride188){
            savedModelDirectory_=*modelDirectory36B860_;
            modelDirectory36B860_->clear();modelDirectorySaved_=true;
        }
        //154140's real prefix state feeds26E820; it remains cleared until
        //152EA0/MIR/VUT finish, then restores before153610 motion setup.
        psgRequest_=*modelDirectory36B860_;
        if(!psgRequest_.empty()&&psgRequest_.back()!='\\')psgRequest_+='\\';
        psgRequest_+=c.rigOverride188?c.slt138:c.directory190+c.slt138;
        psgRequest_=setup::detail::canonical(std::move(psgRequest_));
        const auto dot=psgRequest_.find_last_of('.');
        psgRequest_=psgRequest_.substr(0,dot)+".psg";
        a::OriginalStandardTextureDiagnostic50LA fd;
        a::OriginalStandardModelFileView50LA selected;
        need(files_->lookupModel132820(psgRequest_,a::OriginalStandardModelFileKind50LA::psg,selected,fd)&&
            selected.id.resolver==psgId_.resolver&&selected.id.row==psgId_.row,
            "original source PSG request differs from authenticated Rig member");
        graphStarted_=true;
        const g::OriginalRacerModelSceneBinding50LA binding{generation_,
            &application_->objectCount423F60(),&application_->attachmentLock423F58(),
            &application_->types383408(),rig_->roots[modelRole_],rig_->sourceNodes[modelRole_],
            rig_->hierarchy[modelRole_].nodeCount};
        if(modelRole_==Rig::chariot){
            const auto result=coldScenes_.construct(binding,canonical,rig_->main,rig_->hidden,rig_->container);
            failurePC_=result.sourcePC;
            need(result.okay(),"original190/198/194 SceneObject construction failed");
        }
        need(scene_.construct2403A0(binding,canonical,(animationEntity().flags29&1u)!=0).okay(),
            "original2403A0 model constructor failed");
        animation_.fields_.model04=&scene_;
        need(scene_.constructSeparateRoot26E944(canonical,psgRequest_).okay()&&
            scene_.loadSourceRows28CAC0(canonical,rig_->hierarchy[modelRole_]).okay(),
            "original separate-root PSG model construction failed");
        std::array<g::OriginalWreckComponent50LA*,15> storage{};
        const auto peers=peerScenes();
        need(scene_.appendToAnimation154348(outside(storage),animationEntity(),
            peers[0],peers[1],peers[2],peers[3]).okay(),
            "original154348 model append failed");
        modelAppended_=true;
    }
    void prepareAnimation(const g::SceneGraph49P& canonical){
        failurePC_=0x154350u;
        need(modelAppended_&&input_&&!setup_,"original animation setup order rejected");
        setup_=std::make_unique<setup::Setup>();setup_->prepareTables154140(std::move(*input_));input_.reset();
        need(scene_.uppercaseSourceNames152EA0().okay(),"original scene name enumeration failed");
        auto& f=animation_.fields_;const auto& c=setup_->configuration();
        f.nodeCount1E8=static_cast<std::int32_t>(setup_->records1EC().size());
        f.allocation1EC=setup_->records1EC().data();
        if(setup_->mirrorOpened153450())f.allocation1F0=setup_->mirrorRows1F0().data();
        f.field1F8=setup_->field1F8();
        restoreModelDirectory1543A8();
        failurePC_=0x155730u;
        setup_->capture155730(canonical);f.allocation1F4=setup_->savedMatrices1F4().data();
        const g::OriginalMotionDefaults50LA defaults{c.directory190,c.interpolatePosition20,
            c.interpolateRotation24,c.rotationType28,c.positionType2C};
        failurePC_=0x153610u;
        const auto loaded=motions_.load153610(*motionLibrary_,setup_->parsedConfiguration(),c.motionCount14,
            f.field1E0,defaults,[&](const std::string& requested,g::OriginalAdmittedMotion50LA& out){
                const auto source=archive_->lookup(requested);
                const auto sourceName=archive_->originalMemberName(requested);
                if(!source||!sourceName)return false;
                for(unsigned slot=0;slot<Rig::MotionCount;++slot)
                    if(g::original_motion_detail50LA::equal308F00(*sourceName,rig_->motMembers[slot])&&
                       *source==rig_->mot[slot]&&rig_->motions[slot].data==rig_->mot[slot].data()){
                        out={&rig_->motions[slot],*sourceName};return true;}
                // No incomplete retained-manifest miss is called source absence.
                return false;
            });
        need(loaded.okay(),"original153610 admitted MOT ownership failed");
        f.motion18=motions_.current18();f.motionArray1E4=motions_.motionArray1E4();
        failurePC_=0x1556c0u;setup_->restore1556C0(canonical);
        archive_.reset();
        // Rig invokes this before its normalization, motion and root placement.
        // Cold snapshots stay inside the same owner across delayed Session join.
        captureColdNullcgAfterModel(canonical);
        if(modelRole_==Rig::chariot){
            publishColdTarget2119C4();
            const auto result=coldScenes_.parent211A34(canonical);failurePC_=result.sourcePC;
            need(result.okay(),"original190/194 scene parenting failed");
            constructColdWrists211A4C(canonical);
        }
    }
    // Immutable numeric adapter only: allocation/publication remains above.
    // Solver targets are resolved separately and never gate source allocation.
    void bindColdWrists(g::ChariotIkBindings49T& bindings,g::ChariotIkPair49T& records,
        const g::ChariotIkPair49T*& borrowed)const {
        need(!closed_,"original wrist adapter cannot read retired scene borrows");
        const g::ChariotIkChainBinding49T absent{g::SceneNodeIndex49P::none,g::SceneNodeIndex49P::none,
            g::SceneNodeIndex49P::none,g::SceneNodeIndex49P::none};
        bindings={absent,absent};borrowed=nullptr;
        for(unsigned i=0;i<2;++i){
            const auto& owner=coldWrists_[i];
            need(owner.stage!=ColdWrist::Stage::unreached&&owner.stage!=ColdWrist::Stage::allocated,
                "original wrist constructor did not reach terminal optional boundary");
            if(owner.stage!=ColdWrist::Stage::constructed)continue;
            const auto* allocation=owner.allocation?owner.allocation.get():
                racer_&&!racer_->released()?racer_->wristAllocations_[i].get():nullptr;
            need(allocation&&owner.wrist&&!owner.wrist->released,
                "original wrist allocation or retained scene lifetime ended");
            const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> own{{&scene_,nullptr,nullptr,nullptr,nullptr}};
            const g::OriginalRacerSceneObject50LA* target=nullptr;
            need(findBoundsNullcg28C590(scene_,graph(),own,target,i?"r_hand_dummy":"l_hand_dummy"),
                "original retained wrist target lookup topology stopped");
            auto& b=i?bindings.right:bindings.left;
            b={owner.shoulder->sceneIndex,owner.elbow->sceneIndex,owner.wrist->sceneIndex,
                target?target->sceneIndex:g::SceneNodeIndex49P::none};
            (i?records.right:records.left)=allocation->record;borrowed=&records;
        }
    }
    // Actual Rig pre-motion normalization, after retained cold capture. Uses
    //28AB90 so a world cache read for stack0F70 cannot survive the local write.
    void normalizeColdEngineBeforeMotion(const g::SceneGraph49P& canonical){
        failurePC_=0x28ab90u;
        need(modelRole_!=Rig::chariot&&setup_&&!resourcesComplete_&&
            canonical.nodes==arena_->nodes&&canonical.count==rig_->nodeCount&&
            g::validateSceneGraph49P(canonical),"original cold engine normalization boundary rejected");
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> own{{&scene_,nullptr,nullptr,nullptr,nullptr}};
        const g::OriginalRacerSceneObject50LA* cg=nullptr;
        need(findBoundsNullcg28C590(scene_,canonical,own,cg)&&cg,
            "original cold engine normalization retained nullcg missing");
        if(modelRole_==Rig::right)need(coldStack0F70_&&coldStack0F70_->rightNullcg==cg,
            "original right normalization must follow its exact cold world capture");
        g::SceneDisplacement49P zero;
        std::memcpy(zero.v,arena_->visualShared.zero434630.v,sizeof zero.v);
        need(g::setLocalTranslation49P(canonical,cg->sceneIndex,zero),
            "original28AB90 cold engine normalization stopped");
    }
    // Called by this SAME Rig after its canonical scene install, before any
    // PHY handling working copy. Represented340 and344 constructor writes
    // are admitted; probe348/collision owners remain separate cuts.
    void constructColdMain211F6C(const g::SceneGraph49P& canonical){
        need(modelRole_==Rig::chariot&&fixture_ && &fixture_->state==arena_&&
            !bodyAllocationAttempted_&&!body340_&&canonical.nodes==arena_->nodes&&
            canonical.count==rig_->nodeCount&&coldScenes_.initialized_==3&&
            coldScenes_.objects_[0]&&coldScenes_.objects_[0]->sceneIndex==rig_->main&&
            fixture_->bindings.lifecycle.main190==rig_->main,
            "original main340 construction destination rejected");
        bodyAllocationAttempted_=true;failurePC_=0x211f6cu;
        body340_=std::make_unique<g::OriginalRacerMainBodyOwner50LA>();
        const auto constructed=body340_->construct167920(*application_,generation_,
            fixture_->state,fixture_->parameters,fixture_->bindings);
        failurePC_=constructed.sourcePC;
        need(constructed.status==g::OriginalWreckComponentStatus50LA::complete,
            "original main340 constructor stopped");
        const auto bound=body340_->bindMain211FB0(*coldScenes_.objects_[0]);
        failurePC_=bound.sourcePC;
        need(bound.status==g::OriginalWreckComponentStatus50LA::complete,
            "original main340 scene288 binding stopped");
        need(!gravityAllocationAttempted_&&!gravity344_,"original gravity344 allocation cannot replay");
        gravityAllocationAttempted_=true;failurePC_=0x211ff8u;
        gravity344_=std::make_unique<g::OriginalRacerGravityOwner50LA>();
        const auto gravityConstructed=gravity344_->construct282670(*application_,generation_,
            fixture_->state.bodies.main,fixture_->parameters.bodies.main);
        failurePC_=gravityConstructed.sourcePC;
        need(gravityConstructed.status==g::OriginalWreckComponentStatus50LA::complete,
            "original gravity344 constructor stopped");
        const auto gravityTargets=gravity344_->allocateOne282750();
        failurePC_=gravityTargets.sourcePC;
        need(gravityTargets.status==g::OriginalWreckComponentStatus50LA::complete,
            "original gravity344 target-array allocation stopped");
    }
    // Join the existing successful numeric PHY transaction's actual165340
    //100 result to this SAME body owner. The no-fail live state/parameter copy
    //immediately follows; a failed handling transaction never reaches here.
    //This does not claim the intervening2146C0 Entity consumers are present.
    void commitMainInertia21488C(runtime_startup50fl::Fixture& actual,
        const g::AuthoredBinding49N& binding){
        need(&actual==fixture_&&modelRole_==Rig::chariot&&body340_&&
            body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::sceneBound&&
            body340_->canonicalNumericAddress()==&actual.state.bodies.main&&
            body340_->parameters_==&actual.parameters.bodies.main,
            "original main340 handling commit destination rejected");
        std::memcpy(body340_->inertia100_.v,binding.inertia100.v,sizeof binding.inertia100.v);
    }
    // Handling uses a native working copy, but this exact Fixture and its
    // canonical target/854/858 remain the retained source owners. No new visit.
    void preserveColdTargetForHandling(runtime_startup50fl::Fixture& actual,
        g::AuthoredGeometryBinding49O& projection)const {
        using Stage=g::OriginalRacerLength85850LA::Stage;
        need(&actual==fixture_ && &actual.state==arena_&&modelRole_==Rig::chariot&&
            coldChariotLocal_&&arena_->fields.length858.stage_==Stage::initial211A2C&&
            arena_->fields.length858.generation_==generation_&&arena_->fields.length858.slot_==slot_,
            "original cold handling destination or stage replaced");
        projection.targetRelative2F0=actual.secondaryInputs.targetRelative2F0;
        // This transient projection carries the real target into the existing
        // mass/inertia binder; its derived target/858 are never authoritative.
    }
    void setupColdJoint2126(runtime_startup50fl::Fixture& actual,
        const g::AuthoredGeometryBinding49O& geometry,const g::PodSphereBody49L& main,
        g::PodSphereJoint49L& joint){
        g::AuthoredGeometryBinding49O admitted=geometry;
        preserveColdTargetForHandling(actual,admitted);
        auto& length=arena_->fields.length858;
        using Stage=g::OriginalRacerLength85850LA::Stage;
        // Source2126B0/BC writes anchor300 XYZ=(0,0,3); retained original
        // constructor provenance supplies W through the existing binding.
        need(geometry.secondaryAnchor300.v[0]==0.f&&geometry.secondaryAnchor300.v[1]==0.f&&
            geometry.secondaryAnchor300.v[2]==3.f&&geometry.secondaryAnchor300.v[3]==1.f,
            "original2126B0 anchor binding differs");
        auto& sourceJoint=actual.state.bodies.joint;
        need(modelRole_==Rig::chariot&&body340_&&
            body340_->canonicalNumericAddress()==&actual.state.bodies.main&&
            !jointAllocationAttempted_&&!joint360_,
            "original joint360 allocation cannot replay");
        jointAllocationAttempted_=true;failurePC_=0x21266cu;
        joint360_=std::make_unique<g::OriginalPodSphereOwner50LA>();
        const auto constructed=joint360_->construct1B6200(*application_,generation_,sourceJoint);
        failurePC_=constructed.sourcePC;
        need(constructed.status==g::OriginalWreckComponentStatus50LA::complete,
            "original joint360 Entity/numeric construction stopped");
        failurePC_=0x2126d0u;const float anchorLength=coldLength(geometry.secondaryAnchor300.v);
        volatile float result=actual.parameters.lifecycle.restLength854-anchorLength;
        need(std::isfinite(result),"original2126F0 length subtraction stopped");
        length.value_=result;length.stage_=Stage::adjusted2126F4;
        g::PodSphereBody49L secondary{};secondary.inverseMass=geometry.inverseMass270;
        std::memcpy(secondary.inverseInertia.v,geometry.inverseInertia140.v,sizeof secondary.inverseInertia.v);
        // The retained Entity owns this SAME numeric joint; source A/B
        // operations continue here. Session stages the real alias/append later.
        failurePC_=0x212700u;need(g::setupPodSphereA49L(sourceJoint,secondary,geometry.secondaryAnchor300),
            "original212700 joint A setup stopped");
        g::PodSphereVector49L zero;std::memcpy(zero.v,actual.state.visualShared.zero434630.v,16);
        failurePC_=0x212718u;
        need(g::setupPodSphereB49L(sourceJoint,&main,zero,length.value_),"original212718 joint B setup stopped");
        length.stage_=Stage::consumed212718; //Joint4C is a clamped consumer, never858.
        joint=sourceJoint; //Successful handling working copy keeps the same owner writes.
    }
    const setup::Setup& animationSetup()const {need(bool(setup_),"original model setup missing");return *setup_;}
    const g::OriginalRacerAnimationCold50LA& animation()const noexcept{return animation_;}
    const g::OriginalRacerAnimationCold50LA& animation1A4()const {
        need(modelRole_==Rig::right,"original animation1A4 requires the actual right role");return animation_;
    }
    const g::OriginalRacerAnimationCold50LA& animation1A8()const {
        need(modelRole_==Rig::left,"original animation1A8 requires the actual left role");return animation_;
    }
    const g::OriginalRacerAnimationCold50LA& animation1AC()const {
        need(modelRole_==Rig::bottomRight,"original animation1AC requires the actual bottom-right role");return animation_;
    }
    const g::OriginalRacerAnimationCold50LA& animation1B0()const {
        need(modelRole_==Rig::bottomLeft,"original animation1B0 requires the actual bottom-left role");return animation_;
    }
    const g::OriginalRacerAnimationCold50LA& animation1A0()const {
        need(modelRole_==Rig::chariot,"original animation1A0 requires the actual chariot role");return animation_;
    }
    unsigned modelRole()const noexcept{return modelRole_;}
    const Rig* actualRig()const noexcept{return rig_;}
    std::uint64_t sessionGeneration()const noexcept{return generation_;}
    std::uint32_t failurePC()const noexcept{return failurePC_;}
    bool finished1538B0()const noexcept{return finished_&&!closed_;}
    bool resourcesComplete1538B0()const noexcept{return resourcesComplete_&&!closed_;}
    const g::OriginalRacerModelSceneOwner50LA* modelScene()const noexcept {
        return resourcesComplete1538B0()?&scene_:nullptr;
    }
    g::OriginalRacerModelPackets50LA* modelPackets()noexcept {
        return resourcesComplete1538B0()?&packets_:nullptr;
    }
    const a::OriginalStandardModelGeometry50LA* modelGeometry()const noexcept {
        return resourcesComplete1538B0()?geometry_.get():nullptr;
    }
    const a::OriginalStandardMaterialReferences50LA* materialReferences()const noexcept {
        return resourcesComplete1538B0()?&materials_:nullptr;
    }
    const a::OriginalStandardMaterialDomain50LA* materialDomain()const noexcept{return domain_.get();}
    // Caller is visiting this exact installed racer slot. The stable arena
    // address is retained only so ordered close can clear its banked borrow.
    void bindRendererMaterialFlame(a::PodFlameOwners50W& actualFlame){
        need((modelRole_==Rig::right||modelRole_==Rig::left||
            modelRole_==Rig::bottomRight||modelRole_==Rig::bottomLeft)&&
            resourcesComplete1538B0()&&!bankedFlame_,"original renderer flame join order rejected");
        // Rig order is L,R,C,BL,BR; the real flame model ordinals are L,R,BL,BR.
        const unsigned flameModel=modelRole_==Rig::left?0u:modelRole_==Rig::right?1u:
            modelRole_==Rig::bottomLeft?2u:3u;
        g::EngineFlareDiagnostic50GV diagnostic;
        need(g::bindCanonicalEngineFlare50LA(actualFlame,scene_,materials_,flameModel,diagnostic),
            "original engine canonical animation join rejected");
        bankedFlame_=&actualFlame;
    }
    bool finishedFor(const g::OriginalRacerEntityCold50LA& racer)const noexcept {
        return finished1538B0()&&attachedFor(racer);
    }
    bool close()noexcept {
        if(closed_)return true;
        if(!bank_||!graphStarted_)return closeOnCurrentGraph();
        try{return bank_->visit(slot_,[&](){return closeOnCurrentGraph();});}
        catch(...){return false;}
    }
    // Complete resource/material ownership before the actual shared14D370
    // scan and GPU publication. Numeric graph installation is never replayed.
    void completeResources1538B0(const std::string& directory){
        failurePC_=0x26e820u;
        need(!resourcesComplete_&&!finished_&&!closed_&&setup_&&setup_->restored1556C0()&&modelAppended_,
            "original model resource completion order rejected");
        a::OriginalStandardTextureDiagnostic50LA fd;
        a::OriginalStandardModelFileView50LA source;
        need(files_->modelFile(psgId_,source,fd)&&source.member&&source.bytes,
            "original PSG cache changed during cold model staging");
        if(source.geometry240110){failurePC_=0x26ea28u;
            throw std::runtime_error("original live PSG warm26EA28 appeared before resource completion: "+
                *source.member+" row="+std::to_string(psgId_.row));}
        payload_=std::make_shared<a::OriginalStandardModelResource50LA>();
        a::OriginalStandardResourceDiagnostic50LA rd;
        need(payload_->prepare(*source.member,*source.bytes,rd),"original PSG geometry resource rejected");
        a::OriginalStandardMaterialDiagnostic50LA md;
        failurePC_=0x26e640u;
        need(materials_.acquire26E640(*payload_,domain_->materials382B38(),{&directory},md),
            "original racer canonical material references rejected");
        auto& fields=scene_.fields();
        fields.materialCount1C0=static_cast<std::int32_t>(*materials_.count1C0());
        fields.materials1C4=materials_.handle1C4(0);
        failurePC_=0x240570u;
        need(scene_.loadQuantization240570(*payload_->scene()).okay(),"original model quantization rejected");
        geometry_=std::make_shared<a::OriginalStandardModelGeometry50LA>();
        failurePC_=0x240110u;
        need(geometry_->construct240110(payload_,files_,psgId_,&scene_,rd),
            "original240110 geometry constructor failed");
        fields.geometry1B4=geometry_.get();
        need(files_->publishGeometry1326A0(psgId_,geometry_,fd),"original PSG geometry cache publication failed");
        need(scene_.allocateSourceRowTable26EE9C().okay(),"original model source-row allocation failed");
        failurePC_=0x240740u;
        need(packets_.construct240740(scene_,*geometry_,true).okay(),"original model CPU palette storage failed");
        // This config08 identity was real while1538B0 consumed it.1573B0
        // disposes storage without writing a source null back into08.
        configuration08_.reset();
        resourcesComplete_=true;failurePC_=0;
    }
    // Actual Session caller retains the original Entity append/alias order.
    // Resource completion happened once before the shared population scan.
    void finish1538B0(g::OriginalRacerEntityCold50LA& racer,const std::string& directory){
        (void)directory;
        const bool right=modelRole_==Rig::right;
        const bool left=modelRole_==Rig::left;
        const bool bottomRight=modelRole_==Rig::bottomRight;
        const bool bottomLeft=modelRole_==Rig::bottomLeft;
        const auto animationPC=right?0x210c7cu:left?0x210f40u:
            bottomRight?0x211244u:bottomLeft?0x211540u:0x211970u;
        const auto modelPC=right?0x210ca4u:left?0x210f68u:
            bottomRight?0x21126cu:bottomLeft?0x211568u:0x211998u;
        failurePC_=right?0x210c48u:left?0x210f0cu:
            bottomRight?0x211210u:bottomLeft?0x21150cu:0x21193cu;
        auto& animationSlot=right?racer.animation1A4:left?racer.animation1A8:
            bottomRight?racer.animation1AC:bottomLeft?racer.animation1B0:racer.animation1A0;
        auto& modelSlot=right?racer.rightModel1B8:left?racer.leftModel1BC:
            bottomRight?racer.bottomRightModel1C0:bottomLeft?racer.bottomLeftModel1C4:racer.chariotModel1B4;
        const auto* rightPeer=peers_[Rig::right];const auto* leftPeer=peers_[Rig::left];
        const auto* bottomRightPeer=peers_[Rig::bottomRight];
        const auto* bottomLeftPeer=peers_[Rig::bottomLeft];
        const auto* chariotPeer=peers_[Rig::chariot];
        need(resourcesComplete1538B0()&&!finished_&&racer.postOverlayConstructed210A20()&&
            racer.racer.sessionGeneration==generation_&&!animationSlot&&!modelSlot&&
            (!racer_||racer_==&racer)&&attachmentOrderAdmitted(racer),
            "original model Session attachment order rejected");
        std::array<g::OriginalWreckComponent50LA*,15> storage{};
        racer_=&racer;
        // The same early-constructed1536F0 identity reaches its real Session
        // slot. This is native staging, not a replayed constructor or cache hit.
        const auto d=outside(storage);
        if(modelRole_==Rig::chariot){
            need(!racer.scene190&&!racer.scene198&&!racer.scene194,
                "original retained scene aliases already published");
            racer.scene190=coldScenes_.objects_[0].get();
            racer.scene198=coldScenes_.objects_[1].get();
            racer.scene194=coldScenes_.objects_[2].get();
            need(!racer.wrist208&&!racer.wrist20C&&!racer.wristAllocations_[0]&&!racer.wristAllocations_[1],
                "original racer wrist allocation ownership already published");
            for(const auto& owner:coldWrists_)
                need(owner.stage==ColdWrist::Stage::constructed||owner.stage==ColdWrist::Stage::absent||
                    owner.stage==ColdWrist::Stage::allocationNull,"original wrist construction not complete");
            // Transfer SAME allocations, never reconstruct/copy the source
            // records. Their scene borrows need not outlive plain2E8B00 frees.
            for(unsigned i=0;i<2;++i)racer.wristAllocations_[i]=std::move(coldWrists_[i].allocation);
            racer.wrist208=racer.wristAllocations_[0]?&racer.wristAllocations_[0]->record:nullptr;
            racer.wrist20C=racer.wristAllocations_[1]?&racer.wristAllocations_[1]->record:nullptr;
            const auto result=coldScenes_.append(d,racer.racer);failurePC_=result.sourcePC;
            need(result.okay(),"original190/198/194 Entity append failed");
        }
        animationSlot=&animation_;
        failurePC_=animationPC;
        const auto attached=g::appendOriginalWreckComponent50LA(d,racer.racer,animationEntity(),2u,
            application_->attachmentLock423F58());
        need(attached.status==g::OriginalWreckComponentStatus50LA::complete,
            "original210C7C/210F40/211244/211540/211970 animation append failed");
        animationAttached_=true;
        modelSlot=&scene_; //210C8C/210F50/211254/211550/211980: SAME model04, not model+8.
        failurePC_=modelPC;
        const auto peers=peerScenes();
        const auto reappended=right?
            scene_.reappendToDetached7F8210CA4(d,animationEntity(),racer.detached7F8,
                peers[0],peers[1],peers[2],peers[3]):
            left?scene_.reappendToDetached7F8210F68(d,animationEntity(),racer.detached7F8,
                peers[0],peers[1],peers[2],peers[3]):
            bottomRight?scene_.reappendToDetached7F821126C(d,animationEntity(),racer.detached7F8,
                &rightPeer->scene_,&leftPeer->scene_,&bottomLeftPeer->scene_,&chariotPeer->scene_):
            bottomLeft?scene_.reappendToDetached7F8211568(d,animationEntity(),racer.detached7F8,
                &rightPeer->scene_,&leftPeer->scene_,&bottomRightPeer->scene_,&chariotPeer->scene_):
            scene_.reappendToDetached7F8211998(d,animationEntity(),racer.detached7F8,
                &rightPeer->scene_,&leftPeer->scene_,bottomRightPeer?&bottomRightPeer->scene_:nullptr,
                bottomLeftPeer?&bottomLeftPeer->scene_:nullptr);
        need(reappended.okay(),"original210CA4/210F68/21126C/211568/211998 model alias append failed");
        modelReappended_=true;
        // Same concrete source caller after210CA4/210F68/21126C/211568/211998.
        // Do not admit the native completed join until its material pass returns.
        // On failure the real aliases/material-prefix state remain for close;
        // no automatic append replay or later-effect completion is supplied.
        failurePC_=right?0x210cb4u:left?0x210f78u:
            bottomRight?0x21127cu:bottomLeft?0x211578u:0x2119a8u;
        const auto materialPass=prepareMaterials213380();
        if(!materialPass.okay()){
            failurePC_=materialPass.sourcePC;
            throw std::runtime_error("original213380 canonical material pass stopped");
        }
        if(modelRole_==Rig::chariot){
            failurePC_=0x211f88u;
            need(body340_&&!racer.body340&&
                body340_->canonicalNumericAddress()==&fixture_->state.bodies.main&&
                body340_->scene288()==racer.scene190,
                "original main340 canonical owner publication rejected");
            racer.body340=body340_.get();
            const auto bodyAppended=body340_->append211FA8(d,racer.racer);
            failurePC_=bodyAppended.sourcePC;
            need(bodyAppended.status==g::OriginalWreckComponentStatus50LA::complete,
                "original main340 Entity append stopped");
            failurePC_=0x212014u;
            need(gravity344_&&!racer.gravity344&&
                gravity344_->canonicalAccelerationAddress()==&fixture_->state.bodies.main.selectedGravity,
                "original gravity344 canonical owner publication rejected");
            racer.gravity344=gravity344_.get();
            const auto gravityAppended=gravity344_->append212034(d,racer.racer);
            failurePC_=gravityAppended.sourcePC;
            need(gravityAppended.status==g::OriginalWreckComponentStatus50LA::complete,
                "original gravity344 Entity append stopped");
            const auto gravityBound=gravity344_->bindTarget212048(*body340_);
            failurePC_=gravityBound.sourcePC;
            need(gravityBound.status==g::OriginalWreckComponentStatus50LA::complete,
                "original gravity344 main target binding stopped");
            failurePC_=0x212688u;
            need(joint360_&&!racer.joint360&&
                joint360_->canonicalNumericAddress()==&fixture_->state.bodies.joint,
                "original joint360 canonical owner publication rejected");
            racer.joint360=joint360_.get();
            const auto result=joint360_->append2126A8(d,racer.racer);failurePC_=result.sourcePC;
            need(result.status==g::OriginalWreckComponentStatus50LA::complete,
                "original joint360 Entity append stopped");
        }
        finished_=true;failurePC_=0;
    }
};
} //namespace native_racer_model50LA
