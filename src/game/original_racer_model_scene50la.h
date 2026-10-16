#pragma once
namespace native_racer_model50LA {class Owner;}
#include "model_bounds49o.h"
#include "original_application_entities50la.h"
#include "scene_graph49p.h"
#include "../assets/model_geometry49o.h"
#include "../assets/import_scene48d.h"
#include <array>
#include <cmath>
#include <cstring>
#include <exception>
#include <memory>
#include <optional>
#include <string>

namespace rr::assets { class OriginalStandardModelGeometry50LA; }
namespace ra_static_model_cold50LA { class Owner; }
namespace rr::game::original_gc_gameplay_lighting_candidate { class Owner; }
namespace rr::game {
class OriginalRacerModelPackets50LA;

enum class OriginalRacerModelSceneStatus50LA : std::uint8_t {
    complete,binding,phase,graph,entity,typeRegistry,hierarchy,allocation,
    attachmentLocked,reference,observer,sourceRowTable
};
struct OriginalRacerModelSceneResult50LA {
    OriginalRacerModelSceneStatus50LA status=OriginalRacerModelSceneStatus50LA::binding;
    std::uint32_t sourcePC=0;
    bool okay()const noexcept{return status==OriginalRacerModelSceneStatus50LA::complete;}
};
struct OriginalRacerModelSceneBinding50LA {
    std::uint64_t sessionGeneration=0;
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
    OriginalEntityTypeRegistry50LA* actualTypes383408=nullptr;
    SceneNodeIndex49P modelIndex=SceneNodeIndex49P::none;
    // Existing separate-root construction requires every row distinct from
    // modelIndex. Explicit direct-root construction requires row0==modelIndex.
    const SceneNodeIndex49P* sourceRowIndices=nullptr;
    std::uint32_t sourceRowCount=0;
};

// Matrices and parent180/firstChild184/nextSibling188 are exclusively in the
// caller's ONE canonical SceneGraph49P, addressed by sceneIndex. Cleanup must
// revisit that racer's actual bank before its arena dies. Only previous18C,
// absent from SceneNode49P, is represented here; there is no shadow forest.
struct OriginalRacerSceneObject50LA final {
private:
    friend class ::ra_static_model_cold50LA::Owner;
    friend class original_gc_gameplay_lighting_candidate::Owner;
    friend class OriginalRacerModelSceneOwner50LA;
    friend class OriginalRacerColdScenes50LA;
    OriginalWreckComponent50LA entity8;
public:
    const OriginalWreckComponent50LA& entity()const noexcept{return entity8;}
    SceneNodeIndex49P sceneIndex=SceneNodeIndex49P::none;
    std::uint16_t field00=0;
    std::uint8_t type02=0xffu,flags03=0;
    std::string name38;
    std::array<float,4> bounds140{},bounds150{},bounds160{},bounds170{};
    SceneNodeIndex49P previous18C=SceneNodeIndex49P::none;
    std::uint8_t flags191=0;
    std::optional<float> radius194; // No2893A0 write;28CC3C produces it.
    std::optional<float> radius198; // Published by the actual aggregate-bounds producer.
    const void* observer19C=nullptr;
    std::uint32_t nodeCount1A0=0;
    const void* configuration1A4=nullptr;
    OriginalWreckComponentKind50LA dispatch1A8=OriginalWreckComponentKind50LA::unbound;
    bool constructed=false,released=false;
};

// Scalar/pointer stores of26E3A0 and2403A0. The geometry/material/packet
// owners are separate objects and must publish/release their actual fields.
// A nonnull borrowed decoded scene is not a geometry1B4 acquisition.
struct OriginalRacerModelFields50LA {
    std::uint8_t field1B0=0,flags1B2=0,flags1B3=0;
    rr::assets::OriginalStandardModelGeometry50LA* geometry1B4=nullptr;
    OriginalRacerSceneObject50LA* const* sourceRows1B8=nullptr;
    std::optional<float> distance1BC; // No invented constructor distance;26F070 writes it.
    std::int32_t materialCount1C0=0;
    const std::int32_t* materials1C4=nullptr;
    const void* field1C8=nullptr;
    std::array<float,3> offset1D0{};
    std::uint32_t offsetEnabled1DC=0;
    std::array<float,4> scale1E0{};
    const OriginalRacerModelPackets50LA* packets1F0=nullptr;
    std::uint8_t flags1F4=0;
    float field1F8=0.f,field1FC=0.f,field200=0.f;
    std::uint32_t field204=0;
};

// Source-only staged2403A0 ->26E3A0 ->2893A0 and the a2=1 PSG hierarchy,
// plus an explicit fresh a2=0 model-as-row0 ownership route.
// Construct against the canonical cold graph BEFORE later
// nullcg normalization, motion sampling, placement and rebase. Passing a
// completed Rig graph is rejected, rather than relabelling its installation.
// Each operation receives the actual canonical graph; snapshot copies are
// not retained here. Stable Rig indices and owner addresses survive a bank
// handoff without establishing that any source initialization already ran.
//
// This is NOT all26E820 or240740: path/cache resolution, geometry1B4,
// materials1C4, quantization and packet1F0 belong to their concrete owners.
// Destruction is deliberately split at the real source call boundaries.
class OriginalRacerModelSceneOwner50LA final {
    friend class OriginalRacerColdScenes50LA;
    friend class ::ra_static_model_cold50LA::Owner;
    friend class original_gc_gameplay_lighting_candidate::Owner;
    friend class OriginalRacerModelPackets50LA;
    friend class ::native_racer_model50LA::Owner;
    using S=OriginalRacerModelSceneStatus50LA;
    using R=OriginalRacerModelSceneResult50LA;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using I=SceneNodeIndex49P;
    using O=OriginalRacerSceneObject50LA;
    static constexpr I none=I::none;
    enum class RootMode { separate, direct };
public:
    OriginalRacerModelSceneOwner50LA()=default;
    OriginalRacerModelSceneOwner50LA(const OriginalRacerModelSceneOwner50LA&)=delete;
    OriginalRacerModelSceneOwner50LA& operator=(const OriginalRacerModelSceneOwner50LA&)=delete;
    OriginalRacerModelSceneOwner50LA(OriginalRacerModelSceneOwner50LA&&)=delete;
    OriginalRacerModelSceneOwner50LA& operator=(OriginalRacerModelSceneOwner50LA&&)=delete;
    ~OriginalRacerModelSceneOwner50LA(){if(model_.constructed&&!released_)std::terminate();}

    R construct2403A0(const OriginalRacerModelSceneBinding50LA& b,
        const SceneGraph49P& graph,bool sourceA1)noexcept {
        return constructModel2403A0(b,graph,sourceA1,RootMode::separate);
    }

    // Standard27ABC0 passes a2=0 to240740/26E820: source row0 is this
    // actual model, not another SceneObject. Later rows still own allocations.
    // sourceA1 is2403A0's Entity-reference choice, independent of loader a2.
    R constructDirectRoot2403A0(const OriginalRacerModelSceneBinding50LA& b,
        const SceneGraph49P& graph,bool sourceA1)noexcept {
        return constructModel2403A0(b,graph,sourceA1,RootMode::direct);
    }
private:
    R constructModel2403A0(const OriginalRacerModelSceneBinding50LA& b,
        const SceneGraph49P& graph,bool sourceA1,RootMode mode)noexcept {
        if(model_.constructed||released_)return {S::phase,0x2403a0u};
        if(!b.sessionGeneration||!b.actualObjectCount423F60||!b.actualAttachmentLock423F58||
           !b.actualTypes383408||!b.sourceRowIndices||!b.sourceRowCount||
           b.sourceRowCount>rr::assets::maxModelNodes49O)return {S::binding,0x2403a0u};
        if(!validateSceneGraph49P(graph)||!freshNode(graph,b.modelIndex))return {S::graph,0x28947cu};
        if(mode==RootMode::direct&&b.sourceRowIndices[0]!=b.modelIndex)
            return {S::binding,0x28cb0cu};
        for(std::uint32_t row=0;row<b.sourceRowCount;++row){
            const auto index=b.sourceRowIndices[row];
            if((index==b.modelIndex&&!(mode==RootMode::direct&&row==0))||
               !freshNode(graph,index))return {S::graph,0x28947cu};
            for(std::uint32_t prior=0;prior<row;++prior)
                if(index==b.sourceRowIndices[prior])return {S::binding,0x28cb0cu};
        }
        binding_=b;rootMode_=mode;
        for(std::uint32_t row=0;row<b.sourceRowCount;++row)indices_[row]=b.sourceRowIndices[row];
        // Own the native index map, not the caller's temporary pointer array.
        binding_.sourceRowIndices=indices_.data();
        auto result=constructScene2893A0(model_,b.modelIndex,graph,sourceA1,K::model2403A0);
        if(!result.okay())return result;
        //26E3A0: the constructor changes dispatch identity and represented
        // byte03 bits0/1; it does not increment a second Entity reference.
        model_.flags03=static_cast<std::uint8_t>(model_.flags03|3u);
        fields_.geometry1B4=nullptr;fields_.field1B0=0;
        model_.flags191=static_cast<std::uint8_t>(model_.flags191&~1u);
        fields_.sourceRows1B8=nullptr;fields_.flags1B2=0;fields_.flags1B3=0;
        fields_.materialCount1C0=0;fields_.materials1C4=nullptr;fields_.field1C8=nullptr;
        fields_.flags1B3=static_cast<std::uint8_t>(fields_.flags1B3|0x10u);
        //2403A0's own stores. The upper six1F4 bits are not source-cleared.
        model_.dispatch1A8=K::model2403A0;
        fields_.packets1F0=nullptr;fields_.scale1E0={{1.f,1.f,1.f,1.f}};
        fields_.offset1D0={{0.f,0.f,0.f}};fields_.offsetEnabled1DC=0;
        fields_.flags1F4=static_cast<std::uint8_t>(fields_.flags1F4&~1u);
        fields_.field1F8=0.f;fields_.field1FC=0.f;fields_.field200=0.f;
        fields_.flags1F4=static_cast<std::uint8_t>(fields_.flags1F4&~2u);
        fields_.field204=0;modelConstructorComplete_=true;
        return {S::complete,0x240444u};
    }
public:
    // Caller supplies the actual26E934 resolved PSG name after its source
    // directory/lowercase/extension handling. No cache resolution is inferred.
    R constructSeparateRoot26E944(const SceneGraph49P& graph,const std::string& resolvedPsgMember)noexcept {
        if(rootMode_!=RootMode::separate||!modelConstructorComplete_||rows_[0]||referenceReleased_||released_)
            return {S::phase,0x26e944u};
        if(resolvedPsgMember.empty()||resolvedPsgMember.size()>260||
           resolvedPsgMember.find('\0')!=std::string::npos)return {S::binding,0x26e934u};
        if(!validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex)||!freshNode(graph,indices_[0])||
           at(graph,model_.sceneIndex).firstChild184!=none)return {S::graph,0x26e988u};
        std::unique_ptr<O> root;
        std::string name;
        try{root=std::make_unique<O>();name=resolvedPsgMember;}
        catch(...){return {S::allocation,0x26e964u};}
        model_.name38.swap(name); //26E934 strcpy, before the distinct root.
        model_.flags191=static_cast<std::uint8_t>(model_.flags191|2u);
        rows_[0]=std::move(root);
        const auto result=constructScene2893A0(*rows_[0],indices_[0],graph,true,K::scene2893A0);
        if(!result.okay())return result;
        appendScene28BEF0(graph,model_,*rows_[0]);
        return {S::complete,0x26e98cu};
    }

    // Same resolved-name store, but a2=0 skips26E94C..26E98C: no separate
    // root constructor, reference, attachment, or model191 bit1 publication.
    R prepareDirectRoot26E934(const SceneGraph49P& graph,const std::string& resolvedPsgMember)noexcept {
        if(rootMode_!=RootMode::direct||!modelConstructorComplete_||directRootPrepared_||
           referenceReleased_||released_)return {S::phase,0x26e934u};
        if(resolvedPsgMember.empty()||resolvedPsgMember.size()>260||
           resolvedPsgMember.find('\0')!=std::string::npos)return {S::binding,0x26e934u};
        if(!validateSceneGraph49P(graph)||!freshNode(graph,model_.sceneIndex))
            return {S::graph,0x28cb0cu};
        std::string name;
        try{name=resolvedPsgMember;}catch(...){return {S::allocation,0x26e934u};}
        model_.name38.swap(name);directRootPrepared_=true;
        return {S::complete,0x26e934u};
    }

    // Fresh28CAC0 only. Warm26EA28/26EA50 clone is a different source path
    // and cannot be represented by calling this routine on cached row data.
    R loadSourceRows28CAC0(const SceneGraph49P& graph,
        const rr::assets::PsgHierarchyView49O& hierarchy)noexcept {
        if(rootMode_!=RootMode::separate)return {S::phase,0x28cac0u};
        return loadFreshRows28CAC0(graph,hierarchy);
    }
    R loadDirectRootSourceRows28CAC0(const SceneGraph49P& graph,
        const rr::assets::PsgHierarchyView49O& hierarchy)noexcept {
        if(rootMode_!=RootMode::direct||!directRootPrepared_||directRowsStarted_)
            return {S::phase,0x28cac0u};
        return loadFreshRows28CAC0(graph,hierarchy);
    }
private:
    // Private static-owner continuation of26EAC4..26EB3C. Only the genuine
    // a2=0, prototype191.bit1=0, child184=null branch is admitted. This is
    // not the virtual hierarchy clone.28C400 leaves caller's root-only table
    // count at1, without writing constructor nodeCount1A0 (which stays0).
    R loadWarmChildlessRows26EAC4(const SceneGraph49P& graph,
        const OriginalRacerModelSceneOwner50LA& prototype,
        const SceneGraph49P& prototypeGraph)noexcept {
        if(rootMode_!=RootMode::direct||!directRootPrepared_||directRowsStarted_||
           rowsLoaded_||referenceReleased_||released_||&prototype==this||
           binding_.sourceRowCount!=1||prototype.rootMode_!=RootMode::direct||
           !prototype.rowsLoaded_||prototype.released_||prototype.referenceReleased_||
           prototype.binding_.sourceRowCount!=1||
           binding_.sessionGeneration!=prototype.binding_.sessionGeneration||
           binding_.actualObjectCount423F60!=prototype.binding_.actualObjectCount423F60||
           binding_.actualTypes383408!=prototype.binding_.actualTypes383408)
            return {S::phase,0x26eac4u};
        if(!validateSceneGraph49P(graph)||!validateSceneGraph49P(prototypeGraph)||
           !freshNode(graph,model_.sceneIndex)||
           !inRange(prototypeGraph,prototype.model_.sceneIndex)||
           at(graph,model_.sceneIndex).firstChild184!=none||
           at(prototypeGraph,prototype.model_.sceneIndex).firstChild184!=none||
           (prototype.model_.flags191&2u)||!prototype.model_.radius194||
           model_.nodeCount1A0!=0||indices_[0]!=model_.sceneIndex)
            return {S::hierarchy,0x26eab8u};
        directRowsStarted_=true;
        model_.bounds140=prototype.model_.bounds140;
        model_.bounds150=prototype.model_.bounds150;
        model_.radius194=prototype.model_.radius194;
        auto& node=at(graph,model_.sceneIndex);
        node.sceneFlags190=static_cast<std::uint8_t>(node.sceneFlags190|0x24u);
        sourceParents_[0]=-1;
        warmChildlessRows_=true;rowsLoaded_=true;
        return {S::complete,0x26eb3cu};
    }
    R loadFreshRows28CAC0(const SceneGraph49P& graph,
        const rr::assets::PsgHierarchyView49O& hierarchy)noexcept {
        auto* root=sourceRowObject(0);
        if(!root||!root->constructed||rowsLoaded_||referenceReleased_||released_)
            return {S::phase,0x28cac0u};
        if(hierarchy.nodeCount!=binding_.sourceRowCount||!hierarchy.data||!hierarchy.bytes||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex)||!inRange(graph,indices_[0])||
           (rootMode_==RootMode::separate?
               (at(graph,model_.sceneIndex).firstChild184!=indices_[0]||
                at(graph,indices_[0]).parent180!=model_.sceneIndex):
               (indices_[0]!=model_.sceneIndex||!freshNode(graph,model_.sceneIndex)))||
           at(graph,indices_[0]).firstChild184!=none||at(graph,indices_[0]).nextSibling188!=none)
            return {S::hierarchy,0x28cb04u};
        // Admission/native backing allocation is fallible before source
        // mutation. Backing is not a constructed/count-bearing source object.
        std::array<rr::assets::PsgNode49O,rr::assets::maxModelNodes49O> input{};
        std::array<std::unique_ptr<O>,rr::assets::maxModelNodes49O> backing;
        std::array<std::string,rr::assets::maxModelNodes49O> names;
        try{
            for(std::uint32_t row=0;row<hierarchy.nodeCount;++row){
                if(!rr::assets::readPsgNode49O(hierarchy,row,input[row])||
                   (row==0?input[row].parent!=-1:input[row].parent<0||
                    static_cast<std::uint32_t>(input[row].parent)>=row))return {S::hierarchy,0x28cc90u};
                names[row]=input[row].name;
                if(row){if(!freshNode(graph,indices_[row]))return {S::graph,0x28cb44u};
                    backing[row]=std::make_unique<O>();}
            }
        }catch(...){return {S::allocation,0x28d070u};}
        // Once source construction starts, a failed direct load must retire
        // its constructed prefix, never overwrite it with a second attempt.
        if(rootMode_==RootMode::direct)directRowsStarted_=true;
        root->nodeCount1A0=hierarchy.nodeCount; //28CB04; row0 is never reconstructed.
        for(std::uint32_t row=0;row<hierarchy.nodeCount;++row){
            if(row){
                rows_[row]=std::move(backing[row]);
                const auto result=constructScene2893A0(*rows_[row],indices_[row],graph,true,K::scene2893A0);
                if(!result.okay())return result; // Serial registry failure remains explicit.
            }
            auto& target=*sourceRowObject(row);const auto& source=input[row];
            target.name38.swap(names[row]); //28CB68
            std::memcpy(at(graph,target.sceneIndex).localC0.v,source.local,sizeof source.local); //28CB88
            target.flags03=static_cast<std::uint8_t>(target.flags03&~4u);
            at(graph,target.sceneIndex).sceneFlags190=static_cast<std::uint8_t>(
                at(graph,target.sceneIndex).sceneFlags190|0x20u);
            at(graph,target.sceneIndex).sceneFlags190=static_cast<std::uint8_t>(
                at(graph,target.sceneIndex).sceneFlags190|4u);
            std::memcpy(target.bounds140.data(),source.bounds,3*sizeof(float));
            std::memcpy(target.bounds150.data(),source.bounds+3,3*sizeof(float));
            target.radius194=source.bounds[6];sourceParents_[row]=source.parent;
        }
        // Row0 remains the existing model (a2=0) or its distinct26E988 child.
        // Original row0 parent=-1 causes the exact28CC98 skip. This bounded
        // loader admits only later rows with an earlier source-row parent.
        for(std::uint32_t row=1;row<hierarchy.nodeCount;++row)
            appendScene28BEF0(graph,*sourceRowObject(static_cast<std::uint32_t>(sourceParents_[row])),
                *sourceRowObject(row));
        if(rootMode_==RootMode::separate)model_.nodeCount1A0=hierarchy.nodeCount; //26ED38
        rowsLoaded_=true;
        return {S::complete,rootMode_==RootMode::separate?0x26ed38u:0x26ed48u};
    }
public:
    // Reached only after the real resource/material path has completed its
    // earlier source operations. Owns the actual model1B8 row-pointer table.
    R allocateSourceRowTable26EE9C()noexcept {
        if(!rowsLoaded_||rowTable_||rowTableFreed_||referenceReleased_||released_)
            return {S::phase,0x26ee9cu};
        std::unique_ptr<O*[]> next;
        try{next=std::make_unique<O*[]>(binding_.sourceRowCount);}
        catch(...){return {S::allocation,0x26ee9cu};}
        for(std::uint32_t row=0;row<binding_.sourceRowCount;++row)next[row]=sourceRowObject(row);
        rowTable_=std::move(next);fields_.sourceRows1B8=rowTable_.get();
        return {S::complete,0x26eeb4u};
    }

    bool constructed2403A0()const noexcept{return modelConstructorComplete_&&!released_;}
    bool loadedFreshRows28CAC0()const noexcept{return rowsLoaded_&&!warmChildlessRows_&&!released_;}
    bool loadedQuantization240570()const noexcept{return quantizationLoaded_&&!released_;}
    bool released()const noexcept{return released_;}
    const C* modelEntity8()const noexcept{return model_.constructed?&model_.entity8:nullptr;}
    const O* model()const noexcept{return model_.constructed?&model_:nullptr;}
    OriginalRacerModelFields50LA& fields()noexcept{return fields_;}
    const OriginalRacerModelFields50LA& fields()const noexcept{return fields_;}
    std::uint32_t sourceRowCount()const noexcept{return rowsLoaded_?binding_.sourceRowCount:0u;}
    const O* sourceRow(std::uint32_t row)const noexcept{return sourceRowObject(row);}

    //26EE24 virtualAC is240570 (3D1FEC), not later culling240C00.
    // Scene48D retained each source binary32 exactly in a double. This copies
    // the original eight scalar read results without decoding again.
    R loadQuantization240570(const rr::assets::Scene48D& authenticatedScene)noexcept {
        if(!rowsLoaded_||quantizationLoaded_||referenceReleased_||released_)
            return {S::phase,0x240570u};
        if(authenticatedScene.nodes.size()!=binding_.sourceRowCount)return {S::binding,0x240570u};
        std::array<float,7> source{};
        for(unsigned i=0;i<source.size();++i){
            source[i]=static_cast<float>(authenticatedScene.quantization[i]);
            if(!std::isfinite(source[i])||static_cast<double>(source[i])!=authenticatedScene.quantization[i]||
               (i<4&&source[i]<=0.f))return {S::binding,0x240598u};
        }
        std::memcpy(fields_.scale1E0.data(),source.data(),4*sizeof(float));
        std::memcpy(fields_.offset1D0.data(),source.data()+4,3*sizeof(float));
        fields_.offsetEnabled1DC=authenticatedScene.world_space?1u:0u;
        const bool nonunit=fields_.scale1E0[0]!=1.f||fields_.scale1E0[1]!=1.f;
        fields_.flags1B2=static_cast<std::uint8_t>((fields_.flags1B2&~0x40u)|(nonunit?0x40u:0u));
        quantizationLoaded_=true;
        return {S::complete,0x2406c8u};
    }

    // Warm26EBBC virtualB0=2406E0: source parameter copies are separate from
    // hierarchy clone/geometry retain and do not establish either operation.
    R copyParameters2406E0(const OriginalRacerModelSceneOwner50LA& prototype)noexcept {
        if(!modelConstructorComplete_||quantizationLoaded_||referenceReleased_||released_||
           &prototype==this||!prototype.loadedQuantization240570())return {S::phase,0x2406e0u};
        fields_.scale1E0=prototype.fields_.scale1E0;
        fields_.offset1D0=prototype.fields_.offset1D0;
        fields_.offsetEnabled1DC=prototype.fields_.offsetEnabled1DC;
        fields_.flags1B2=prototype.fields_.flags1B2;fields_.flags1B3=prototype.fields_.flags1B3;
        quantizationLoaded_=true;
        return {S::complete,0x240730u};
    }

    //152EA0 changes canonical source names before copying the1EC record.
    // VUT's later optional alias changes1EC only, never these SceneObjects.
    R uppercaseSourceNames152EA0()noexcept {
        if(!rowsLoaded_||referenceReleased_||released_)return {S::phase,0x152ea0u};
        for(std::uint32_t row=0;row<binding_.sourceRowCount;++row)
            for(char& ch:sourceRowObject(row)->name38)if(ch>='a'&&ch<='z')ch=static_cast<char>(ch-'a'+'A');
        return {S::complete,0x152ea0u};
    }

    // Narrow join: the caller provides the real surrounding Entity domain,
    // excluding this private model; this method adds its canonical address.
    R appendToAnimation154348(const OriginalWreckComponentDomain50LA& outside,
        C& animation,const OriginalRacerModelSceneOwner50LA* sibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* secondSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* thirdSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* fourthSibling=nullptr)noexcept {
        if(!modelConstructorComplete_||!rowsLoaded_||referenceReleased_||released_||
           animation.kind!=K::animation1536F0)return {S::phase,0x154348u};
        std::array<C*,maxOriginalWreckComponents50LA> list{};
        OriginalWreckComponentDomain50LA joined{};
        if(!joinDomain(outside,list,joined,sibling,secondSibling,thirdSibling,fourthSibling))
            return {S::entity,0x154348u};
        const auto result=appendOriginalWreckComponent50LA(joined,animation,model_.entity8,0,
            *binding_.actualAttachmentLock423F58);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)
            return {result.status==OriginalWreckComponentStatus50LA::attachmentLocked?
                S::attachmentLocked:S::entity,result.sourcePC};
        publishedToAnimation_=true;
        return {S::complete,0x154348u};
    }

    // The bounded second141960 at210CA4/210F68/21126C/211568/211998. Preserve the old
    // animation18 alias while rewriting THIS canonical Entity's current
    // parent/list/mask. There is no detach and no added reference.
    R reappendToDetached7F8210CA4(const OriginalWreckComponentDomain50LA& outside,
        C& animation,C& detached7F8,const OriginalRacerModelSceneOwner50LA* sibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* secondSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* thirdSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* fourthSibling=nullptr)noexcept {
        return reappendToDetached7F8(outside,animation,detached7F8,
            sibling,secondSibling,thirdSibling,fourthSibling,0x210ca4u,0x210ca8u);
    }
    R reappendToDetached7F8210F68(const OriginalWreckComponentDomain50LA& outside,
        C& animation,C& detached7F8,const OriginalRacerModelSceneOwner50LA* sibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* secondSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* thirdSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* fourthSibling=nullptr)noexcept {
        return reappendToDetached7F8(outside,animation,detached7F8,
            sibling,secondSibling,thirdSibling,fourthSibling,0x210f68u,0x210f6cu);
    }
    R reappendToDetached7F821126C(const OriginalWreckComponentDomain50LA& outside,
        C& animation,C& detached7F8,const OriginalRacerModelSceneOwner50LA* right,
        const OriginalRacerModelSceneOwner50LA* left,
        const OriginalRacerModelSceneOwner50LA* bottomLeft=nullptr,
        const OriginalRacerModelSceneOwner50LA* chariot=nullptr)noexcept {
        if(!admitDetachedPrefix(detached7F8,{{right,left,nullptr,nullptr}},2u))
            return {S::entity,0x21126cu};
        return reappendToDetached7F8(outside,animation,detached7F8,
            right,left,bottomLeft,chariot,0x21126cu,0x211270u);
    }
    R reappendToDetached7F8211568(const OriginalWreckComponentDomain50LA& outside,
        C& animation,C& detached7F8,const OriginalRacerModelSceneOwner50LA* right,
        const OriginalRacerModelSceneOwner50LA* left,
        const OriginalRacerModelSceneOwner50LA* bottomRight,
        const OriginalRacerModelSceneOwner50LA* chariot=nullptr)noexcept {
        if(!admitDetachedPrefix(detached7F8,{{right,left,bottomRight,nullptr}},3u))
            return {S::entity,0x211568u};
        return reappendToDetached7F8(outside,animation,detached7F8,
            right,left,bottomRight,chariot,0x211568u,0x21156cu);
    }
    R reappendToDetached7F8211998(const OriginalWreckComponentDomain50LA& outside,
        C& animation,C& detached7F8,const OriginalRacerModelSceneOwner50LA* right,
        const OriginalRacerModelSceneOwner50LA* left,
        const OriginalRacerModelSceneOwner50LA* bottomRight=nullptr,
        const OriginalRacerModelSceneOwner50LA* bottomLeft=nullptr)noexcept {
        // This bounded separate-chariot join has both actual engine models.
        // Optional POD02 lowers are an entire pair in their source order.
        // Combined-model2118B8 remains a separate, unadmitted owner.
        if(!right||!left||right==left||right==this||left==this||
           bool(bottomRight)!=bool(bottomLeft))
            return {S::binding,0x211998u};
        if(!admitDetachedPrefix(detached7F8,{{right,left,bottomRight,bottomLeft}},bottomRight?4u:2u))
            return {S::entity,0x211998u};
        return reappendToDetached7F8(outside,animation,detached7F8,
            right,left,bottomRight,bottomLeft,0x211998u,0x21199cu);
    }
private:
    bool admitDetachedPrefix(const C& detached7F8,
        const std::array<const OriginalRacerModelSceneOwner50LA*,4>& prefix,
        std::size_t count)const noexcept {
        if(count<2u||count>prefix.size())return false;
        const C* previous=nullptr;
        for(std::size_t i=0;i<count;++i){
            const auto* peer=prefix[i];
            if(!peer||peer==this||!peer->reappended7F8_||peer->referenceReleased_||peer->released_)
                return false;
            for(std::size_t j=0;j<i;++j)if(prefix[j]==peer)return false;
            const auto& c=peer->model_.entity8;
            if(c.parent1C!=&detached7F8||c.previous10!=previous||
               (previous?previous->next14!=&c:detached7F8.firstChild18!=&c))return false;
            previous=&c;
        }
        return previous&&!previous->next14;
    }
    R reappendToDetached7F8(const OriginalWreckComponentDomain50LA& outside,
        C& animation,C& detached7F8,const OriginalRacerModelSceneOwner50LA* sibling,
        const OriginalRacerModelSceneOwner50LA* secondSibling,
        const OriginalRacerModelSceneOwner50LA* thirdSibling,
        const OriginalRacerModelSceneOwner50LA* fourthSibling,
        std::uint32_t callPC,std::uint32_t returnPC)noexcept {
        using namespace original_wreck_component_detail50LA;
        if(!publishedToAnimation_||reappended7F8_||referenceReleased_||released_)
            return {S::phase,callPC};
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x14197cu};
        std::array<C*,maxOriginalWreckComponents50LA> list{};
        OriginalWreckComponentDomain50LA joined{};
        if(!joinDomain(outside,list,joined,sibling,secondSibling,thirdSibling,fourthSibling)||
           !contains(joined,&animation)||!contains(joined,&detached7F8))
            return {S::entity,0x141984u};
        auto& c=model_.entity8;
        if(animation.kind!=K::animation1536F0||animation.firstChild18!=&c||c.parent1C!=&animation||
           c.previous10||c.next14||c.reference00!=1u||detached7F8.parent1C||detached7F8.previous10||
           detached7F8.next14||detached7F8.kind!=K::entity141300)return {S::entity,0x14198cu};
        C* tail=nullptr;std::size_t left=joined.count;
        for(auto* p=detached7F8.firstChild18;p;p=p->next14){
            if(!left--||!contains(joined,p)||p==&c||p->parent1C!=&detached7F8||p->previous10!=tail)
                return {S::entity,0x1419a8u};
            tail=p;
        }
        if(tail)tail->next14=&c;else detached7F8.firstChild18=&c;
        c.previous10=tail;c.parent1C=&detached7F8;c.mask20=0x1008u;
        detached7F8.descendants24|=c.descendants24|c.mask20;
        reappended7F8_=true;
        return {S::complete,returnPC};
    }
public:
    // Preflight all scene ownership before142590 or any240450/26E460 free.
    // There are no new Entity links to PSG rows: their ownership is184/188.
    R preflightScene2896D0(const SceneGraph49P& graph)const noexcept {
        return preflightRetainedScene2896D0(graph,nullptr);
    }
private:
    // The static owner preflights its own typed observer before the later
    // destructor removes it. Public callers still require null19C.
    R preflightRetainedScene2896D0(const SceneGraph49P& graph,const void* observer)const noexcept {
        if(released_)return {S::complete,0x2897acu};
        if(!model_.constructed||!binding_.actualObjectCount423F60||!binding_.actualAttachmentLock423F58)
            return {S::binding,0x2896d0u};
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x1425a8u};
        if(!validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex)||
           at(graph,model_.sceneIndex).parent180!=none||at(graph,model_.sceneIndex).nextSibling188!=none||
           model_.previous18C!=none)return {S::graph,0x289704u};
        if(model_.observer19C!=observer||model_.configuration1A4)return {S::observer,0x289760u};
        if(model_.entity8.reference00!=(referenceReleased_?0u:1u))return {S::reference,0x104970u};
        std::size_t owned=1;
        for(std::uint32_t row=0;row<binding_.sourceRowCount;++row){
            const auto* source=sourceRowObject(row);
            // Aliased row0 already received model preflight and its one count.
            if(!source||source==&model_)continue;const auto& object=*source;
            if(!object.constructed||object.released||!inRange(graph,object.sceneIndex)||
               object.observer19C||object.configuration1A4)return {S::observer,0x289760u};
            const auto& c=object.entity8;
            if(c.reference00!=1u||c.kind!=K::scene2893A0||c.sessionGeneration!=binding_.sessionGeneration||
               c.firstChild18||c.parent1C||c.previous10||c.next14)return {S::entity,0x28972cu};
            ++owned;
        }
        std::size_t seen=0;
        if(!admitSceneSubtree(graph,model_,seen,owned)||seen!=owned)return {S::graph,0x289744u};
        return {S::complete,0x2896d0u};
    }

public:
    R preflightModelReference142590(const OriginalWreckComponentDomain50LA& outside,
        const OriginalRacerModelSceneOwner50LA* sibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* secondSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* thirdSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* fourthSibling=nullptr)const noexcept {
        using namespace original_wreck_component_detail50LA;
        if(!model_.constructed||referenceReleased_||released_)return {S::phase,0x142590u};
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x1425a8u};
        std::array<C*,maxOriginalWreckComponents50LA> list{};
        OriginalWreckComponentDomain50LA domain{};
        if(!joinDomain(outside,list,domain,sibling,secondSibling,thirdSibling,fourthSibling))
            return {S::entity,0x142590u};
        const auto& c=model_.entity8;
        if(!original_wreck_component_detail50LA::domain(domain)||!contains(domain,&c)||
           c.reference00!=1u||c.kind!=K::model2403A0||c.firstChild18||c.next14)
            return {S::entity,0x1425b8u};
        if(c.previous10&&(!contains(domain,c.previous10)||c.previous10->next14!=&c||
           c.previous10->parent1C!=c.parent1C))return {S::entity,0x142604u};
        if(c.parent1C){
            if(!contains(domain,c.parent1C)||(!c.previous10&&c.parent1C->firstChild18!=&c))
                return {S::entity,0x14262cu};
        }else if(c.previous10)return {S::entity,0x14262cu};
        return {S::complete,0x142590u};
    }

    // Caller first preflights scene and concrete resource cleanup. Source
    //142590 repairs CURRENT parent1C, preserving the old animation18 alias.
    // No retain,141A70 detach or invented pointer clearing is performed.
    R releaseModelReference142590(const OriginalWreckComponentDomain50LA& domain,
        const OriginalRacerModelSceneOwner50LA* sibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* secondSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* thirdSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* fourthSibling=nullptr)noexcept {
        const auto result=preflightModelReference142590(domain,sibling,secondSibling,thirdSibling,fourthSibling);
        if(!result.okay())return result;
        auto& c=model_.entity8;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        --c.reference00;referenceReleased_=true;
        // The resulting zero dispatches241920 ->240450. Its packet/material/
        // resource operations must occur before releaseSceneBase2896D0 below.
        return {S::complete,0x142654u};
    }

    R freeSourceRowTable26E5CC()noexcept {
        if(!referenceReleased_||released_||rowTableFreed_)return {S::phase,0x26e5ccu};
        rowTable_.reset();rowTableFreed_=true;
        //26E5D8 does not clear model1B8. Its stale value is not dereferenced
        // again during this disposal sequence. Native ownership is separate.
        return {S::complete,0x26e5dcu};
    }

    // Only2896D0, after upstream240450 packets and26E460 material/resource
    // release and array frees. This method never claims those owners ran.
    R releaseSceneBase2896D0(const SceneGraph49P& graph)noexcept {
        if(released_)return {S::complete,0x2897acu};
        if(!referenceReleased_)return {S::phase,0x2896d0u};
        if(rowTable_)return {S::sourceRowTable,0x26e5ccu};
        const auto result=preflightScene2896D0(graph);if(!result.okay())return result;
        destroyScene2896D0(graph,model_);released_=true;
        return {S::complete,0x2897acu};
    }

    // Native exception recovery for the still-private early stage. This is
    // not a new original error branch or a loaded-model destructor claim.
    // All genuine constructed Entities receive their concrete destruction;
    // unconstructed preflight backing never contributes to423F60.
    R closeFreshUnlinked(const SceneGraph49P& graph)noexcept {
        if(released_||!model_.constructed)return {S::complete,0};
        auto& c=model_.entity8;
        if(publishedToAnimation_||fields_.geometry1B4||fields_.materials1C4||fields_.packets1F0||
           fields_.field1C8||model_.observer19C||
           c.parent1C||c.previous10||c.next14||c.firstChild18||referenceReleased_||c.reference00!=1u)
            return {S::phase,0x240450u};
        if(!binding_.actualObjectCount423F60||!binding_.actualAttachmentLock423F58||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex)||
           at(graph,model_.sceneIndex).parent180!=none||at(graph,model_.sceneIndex).nextSibling188!=none)
            return {S::binding,0x142590u};
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x1425a8u};
        std::size_t count=1;
        // rows_ holds only allocated objects. Direct row0 lives in model_,
        // so prefix recovery and forest retirement must not visit it twice.
        for(auto& row:rows_)if(row&&row->constructed&&!row->released){
            const auto& entity=row->entity8;
            if(entity.reference00!=1u||entity.parent1C||entity.previous10||entity.next14||
               entity.firstChild18||row->observer19C||!inRange(graph,row->sceneIndex))return {S::entity,0x28972cu};
            ++count;
        }
        // Validate all constructed roots before mutation, including rows
        // left unlinked if construction stopped before28CC90's link pass.
        std::size_t seen=0;
        if(!admitSceneSubtree(graph,model_,seen,count))return {S::graph,0x289744u};
        for(const auto& row:rows_)if(row&&row->constructed&&!row->released&&
            at(graph,row->sceneIndex).parent180==none){
            if(row->previous18C!=none||at(graph,row->sceneIndex).nextSibling188!=none||
               !admitSceneSubtree(graph,*row,seen,count))return {S::graph,0x289744u};
        }
        if(seen!=count)return {S::graph,0x289744u};
        // A registry failure during row construction can leave a counted
        // row outside184, before the subsequent source hierarchy-link pass.
        for(auto& row:rows_)if(row&&row->constructed&&!row->released&&
            at(graph,row->sceneIndex).parent180==none){
            --row->entity8.reference00;destroyScene2896D0(graph,*row);
        }
        --c.reference00;referenceReleased_=true;
        rowTable_.reset();rowTableFreed_=true;
        destroyScene2896D0(graph,model_);released_=true;
        return {S::complete,0x2897acu};
    }
private:
    // Actual28E110 -> virtual90=26F4B0, only from the retained native owner.
    // No caller bounds/flags or publication receipt. Separate racer rows only.
    R publishAggregate28E110(const SceneGraph49P& graph)noexcept {
        if(!constructed2403A0()||!rowsLoaded_||referenceReleased_||!validateSceneGraph49P(graph)||
           !inRange(graph,model_.sceneIndex))return {S::phase,0x28e110u};
        auto& root=at(graph,model_.sceneIndex);
        const auto flags=root.sceneFlags190;
        const bool recompute=(flags&0x80u)&&((flags&8u)||!(flags&0x40u));
        if(!recompute){
            if((flags&0x80u)&&!(flags&1u)&&!refreshSceneWorld49P(graph,model_.sceneIndex))
                return {S::graph,0x2897b0u};
            return model_.radius198?R{S::complete,0x28e218u}:R{S::phase,0x28e1f8u};
        }
        if(rootMode_!=RootMode::separate||warmChildlessRows_||!rowTable_||
           fields_.sourceRows1B8!=rowTable_.get()||model_.nodeCount1A0!=binding_.sourceRowCount||
           !model_.nodeCount1A0||model_.nodeCount1A0>maxModelBoundsNodes49O||flags&0x10u)
            return {S::phase,0x26f4c0u}; //Sphere/direct-root route remains distinct.
        if(!refreshSceneWorld49P(graph,model_.sceneIndex))return {S::graph,0x26f608u};
        std::array<ModelBoundsNode49O,maxModelBoundsNodes49O> rows{};
        ModelBoundsContext49O context{};context.rootWorld100=root.world100;
        context.rootSceneFlags190=root.sceneFlags190;
        for(unsigned k=0;k<3;++k){context.rootCenter140.v[k]=model_.bounds140[k];
            context.rootHalfExtent150.v[k]=model_.bounds150[k];}
        for(std::uint32_t i=0;i<model_.nodeCount1A0;++i){
            const auto* row=sourceRowObject(i);
            if(!row||rowTable_[i]!=row||!row->constructed||row->released||!inRange(graph,row->sceneIndex))
                return {S::sourceRowTable,0x26f6e0u};
            const auto parent=sourceParents_[i];
            if(parent < -1)return {S::hierarchy,0x28daf0u};
            const auto expected=parent<0?model_.sceneIndex:
                (parent<static_cast<std::int32_t>(i)?sourceRowObject(std::uint32_t(parent))->sceneIndex:none);
            const auto& node=at(graph,row->sceneIndex);
            if(expected==none||node.parent180!=expected)return {S::hierarchy,0x28daf0u};
            rows[i].local=node.localC0;rows[i].parent=parent;rows[i].sceneFlags190=node.sceneFlags190;
            for(unsigned k=0;k<3;++k){rows[i].center140.v[k]=row->bounds140[k];
                rows[i].halfExtent150.v[k]=row->bounds150[k];}
        }
        ModelBoundsWorkspace49O workspace{};ModelBoundsResult49O output{};
        if(!computeModelBounds49O(rows.data(),model_.nodeCount1A0,context,workspace,output))
            return {S::graph,0x28daf0u};
        // Bounds publish only after real current-transform union succeeds.
        std::memcpy(model_.bounds160.data(),output.center160,sizeof output.center160);
        std::memcpy(model_.bounds170.data(),output.halfExtent170,sizeof output.halfExtent170);
        model_.radius198=output.radius198;root.sceneFlags190=output.rootSceneFlags190;
        return {S::complete,0x26f844u};
    }
private:
    OriginalRacerModelSceneBinding50LA binding_{};
    O model_;
    OriginalRacerModelFields50LA fields_;
    std::array<I,rr::assets::maxModelNodes49O> indices_{};
    std::array<std::int32_t,rr::assets::maxModelNodes49O> sourceParents_{};
    // Separate mode owns rows0..N-1; direct mode leaves rows_[0] empty.
    std::array<std::unique_ptr<O>,rr::assets::maxModelNodes49O> rows_;
    std::unique_ptr<O*[]> rowTable_;
    RootMode rootMode_=RootMode::separate;
    bool directRootPrepared_=false,directRowsStarted_=false,warmChildlessRows_=false;
    bool modelConstructorComplete_=false,rowsLoaded_=false,referenceReleased_=false;
    bool rowTableFreed_=false,released_=false;
    bool publishedToAnimation_=false,reappended7F8_=false;
    bool quantizationLoaded_=false;

    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    static bool inRange(const SceneGraph49P& graph,I i)noexcept {
        const auto n=static_cast<std::int32_t>(i);
        return graph.nodes&&n>=0&&static_cast<std::uint32_t>(n)<graph.count;
    }
    static SceneNode49P& at(const SceneGraph49P& graph,I i)noexcept {
        return graph.nodes[static_cast<std::uint32_t>(i)];
    }
    static bool freshNode(const SceneGraph49P& graph,I i)noexcept {
        return inRange(graph,i)&&at(graph,i).parent180==none&&
            at(graph,i).firstChild184==none&&at(graph,i).nextSibling188==none;
    }
    O* sourceRowObject(std::uint32_t row)noexcept {
        if(row>=binding_.sourceRowCount)return nullptr;
        if(rootMode_==RootMode::direct&&row==0)return model_.constructed?&model_:nullptr;
        return rows_[row].get();
    }
    const O* sourceRowObject(std::uint32_t row)const noexcept {
        if(row>=binding_.sourceRowCount)return nullptr;
        if(rootMode_==RootMode::direct&&row==0)return model_.constructed?&model_:nullptr;
        return rows_[row].get();
    }
    O* object(I i)noexcept {
        if(model_.sceneIndex==i)return &model_;
        for(std::uint32_t row=0;row<binding_.sourceRowCount;++row){
            auto* source=sourceRowObject(row);if(source&&source->sceneIndex==i)return source;
        }
        return nullptr;
    }
    const O* object(I i)const noexcept {
        if(model_.sceneIndex==i)return &model_;
        for(std::uint32_t row=0;row<binding_.sourceRowCount;++row){
            const auto* source=sourceRowObject(row);if(source&&source->sceneIndex==i)return source;
        }
        return nullptr;
    }
    R constructScene2893A0(O& target,I index,const SceneGraph49P& graph,bool sourceA1,K kind)noexcept {
        return constructStandaloneScene2893A0(binding_,target,index,graph,sourceA1,kind);
    }
    static R constructStandaloneScene2893A0(const OriginalRacerModelSceneBinding50LA& binding,
        O& target,I index,const SceneGraph49P& graph,bool sourceA1,K kind)noexcept {
        target.field00=0;target.type02=0xffu;
        target.flags03=static_cast<std::uint8_t>(target.flags03&~7u);
        const auto entity=initializeOriginalWreckComponent50LA(target.entity8,kind,
            binding.sessionGeneration,key(&target.entity8),sourceA1,*binding.actualObjectCount423F60);
        if(entity.status!=OriginalWreckComponentStatus50LA::complete)return {S::entity,entity.sourcePC};
        target.constructed=true;target.sceneIndex=index;target.dispatch1A8=K::scene2893A0;
        //289448/454/460/46C: SQC2 vf0 is(0,0,0,1), not a zero vector.
        target.bounds140=target.bounds150=target.bounds160=target.bounds170={{0.f,0.f,0.f,1.f}};
        auto& node=at(graph,index);node.parent180=node.firstChild184=node.nextSibling188=none;
        target.previous18C=none;
        std::memset(node.localC0.v,0,sizeof node.localC0.v);
        node.localC0.v[15]=1.f;node.localC0.v[10]=1.f;
        node.localC0.v[5]=1.f;node.localC0.v[0]=1.f;
        std::memset(node.world100.v,0,sizeof node.world100.v);
        node.sceneFlags190=0x80u;
        target.observer19C=nullptr;target.name38.clear();target.nodeCount1A0=0;
        const auto type=binding.actualTypes383408->findOrAppend2A31E0("SoultreeObject");
        if(!type.okay())return {S::typeRegistry,0x289654u};
        target.type02=type.type;target.entity8.category28=type.type;
        target.flags191=static_cast<std::uint8_t>((target.flags191&~3u)|1u);
        target.configuration1A4=nullptr;
        return {S::complete,0x2896c0u};
    }
    void appendScene28BEF0(const SceneGraph49P& graph,O& parent,O& child)noexcept {
        auto& p=at(graph,parent.sceneIndex);auto& c=at(graph,child.sceneIndex);
        c.parent180=parent.sceneIndex;
        if(p.firstChild184==none)p.firstChild184=child.sceneIndex;
        else{auto tail=p.firstChild184;
            while(at(graph,tail).nextSibling188!=none)tail=at(graph,tail).nextSibling188;
            at(graph,tail).nextSibling188=child.sceneIndex;child.previous18C=tail;}
    }
    bool admitSceneSubtree(const SceneGraph49P& graph,const O& root,
        std::size_t& seen,std::size_t limit)const noexcept {
        if(++seen>limit||root.released)return false;
        auto child=at(graph,root.sceneIndex).firstChild184;I previous=none;
        while(child!=none){
            const auto* next=object(child);
            if(!next||next==&model_||next->previous18C!=previous||
               at(graph,child).parent180!=root.sceneIndex||!admitSceneSubtree(graph,*next,seen,limit))return false;
            previous=child;child=at(graph,child).nextSibling188;
        }
        return true;
    }
    static void invalidate289A20(const SceneGraph49P& graph,I index)noexcept {
        auto& node=at(graph,index);
        if(node.nextSibling188!=none)invalidate289A20(graph,node.nextSibling188);
        if(!(node.sceneFlags190&1u))return;
        node.sceneFlags190=static_cast<std::uint8_t>(node.sceneFlags190&~1u);
        if(node.firstChild184!=none)invalidate289A20(graph,node.firstChild184);
    }
    // Shared source prefix: mark child/ancestors before invalidating the
    // ready child subtree, while the original parent links still exist.
    static void prepareDetachScene28BFC0(const SceneGraph49P& graph,const O& child)noexcept {
        auto& node=at(graph,child.sceneIndex);
        for(auto p=child.sceneIndex;p!=none;p=at(graph,p).parent180)
            at(graph,p).sceneFlags190=static_cast<std::uint8_t>(at(graph,p).sceneFlags190|0x80u);
        if(node.sceneFlags190&1u){
            node.sceneFlags190=static_cast<std::uint8_t>(node.sceneFlags190&~1u);
            if(node.firstChild184!=none)invalidate289A20(graph,node.firstChild184);
        }
    }
    void detachScene28BFC0(const SceneGraph49P& graph,O& child)noexcept {
        prepareDetachScene28BFC0(graph,child);
        auto& node=at(graph,child.sceneIndex);
        auto& parent=at(graph,node.parent180);
        if(parent.firstChild184==child.sceneIndex)parent.firstChild184=node.nextSibling188;
        if(node.nextSibling188!=none)object(node.nextSibling188)->previous18C=child.previous18C;
        if(child.previous18C!=none)at(graph,child.previous18C).nextSibling188=node.nextSibling188;
        node.parent180=none;node.nextSibling188=none;child.previous18C=none;
    }
    void destroyScene2896D0(const SceneGraph49P& graph,O& owner)noexcept {
        owner.dispatch1A8=K::scene2893A0;
        auto& node=at(graph,owner.sceneIndex);
        if(node.parent180!=none)detachScene28BFC0(graph,owner); //289710
        if(node.firstChild184!=none){
            //289744 repeatedly releases the current first child's NEXT
            // sibling, and then releases the first child. This is not a
            // reverse-array loop and is independent of Entity14 sibling links.
            while(at(graph,node.firstChild184).nextSibling188!=none){
                auto& next=*object(at(graph,node.firstChild184).nextSibling188);
                --next.entity8.reference00;destroyScene2896D0(graph,next);
            }
            auto& first=*object(node.firstChild184);
            --first.entity8.reference00;destroyScene2896D0(graph,first);
        }
        // Observer19C is admitted null; no unjoined280920 destructor is run.
        // Each concrete scene destructor reaches its own1413E0 exactly once.
        --*binding_.actualObjectCount423F60;owner.released=true;
    }
    bool joinDomain(const OriginalWreckComponentDomain50LA& outside,
        std::array<C*,maxOriginalWreckComponents50LA>& list,
        OriginalWreckComponentDomain50LA& joined,
        const OriginalRacerModelSceneOwner50LA* sibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* secondSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* thirdSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* fourthSibling=nullptr)const noexcept {
        using namespace original_wreck_component_detail50LA;
        const std::array<const OriginalRacerModelSceneOwner50LA*,4> peers{{
            sibling,secondSibling,thirdSibling,fourthSibling}};
        std::size_t added=1u;bool ended=false;
        for(std::size_t i=0;i<peers.size();++i){
            if(!peers[i]){ended=true;continue;}
            if(ended)return false;
            for(std::size_t j=0;j<i;++j)if(peers[j]==peers[i])return false;
            ++added;
        }
        if(outside.sessionGeneration!=binding_.sessionGeneration||outside.count>list.size()-added||
           (outside.count&&!original_wreck_component_detail50LA::domain(outside)))return false;
        // At most four bounded real peers supply their private canonical Entity8. This
        // borrow adds no object, reference, alias repair or mutable accessor.
        for(const auto* peer:peers)if(peer&&(peer==this||!peer->model_.constructed||peer->released_||
           peer->referenceReleased_||peer->binding_.sessionGeneration!=binding_.sessionGeneration||
           peer->binding_.actualObjectCount423F60!=binding_.actualObjectCount423F60||
           peer->binding_.actualAttachmentLock423F58!=binding_.actualAttachmentLock423F58||
           peer->binding_.actualTypes383408!=binding_.actualTypes383408))return false;
        for(std::size_t i=0;i<outside.count;++i){
            if(outside.objects[i]==&model_.entity8)return false;
            for(const auto* peer:peers)if(peer&&outside.objects[i]==&peer->model_.entity8)return false;
            list[i]=outside.objects[i];
        }
        // Mutability is confined to this owner's explicit joining methods;
        // no mutable Entity pointer is exposed by its public interface.
        list[outside.count]=const_cast<C*>(&model_.entity8);
        std::size_t at=outside.count+1u;
        for(const auto* peer:peers)if(peer)list[at++]=const_cast<C*>(&peer->model_.entity8);
        joined={binding_.sessionGeneration,list.data(),outside.count+added};
        return original_wreck_component_detail50LA::domain(joined);
    }
};
} // namespace rr::game
