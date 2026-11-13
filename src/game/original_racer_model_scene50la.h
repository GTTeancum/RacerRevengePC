#pragma once
namespace native_racer_model50LA {class Owner;}
namespace rr::platform {class OriginalUiCourseOwnedLifecycle50KG; class OriginalUiGarageStaticModels50KG; class OriginalUiGarageSelectedModel50KG; class OriginalUiGarageModelResource50KG; class OriginalUiGarageVisibility50KG;}
#include "model_bounds49o.h"
#include "collection_bounds48s.h"
#include "chariot_scene49u.h"
#include "original_standard_placement_globals50la.h"
#include "original_application_entities50la.h"
#include "scene_graph49p.h"
#include "vehicle_visual49r_internal.h"
#include "../assets/model_geometry49o.h"
#include "../assets/import_scene48d.h"
#include "../assets/original_standard_texture_resource50la.h"
#include "../assets/original_standard_model_resource50la.h"
#include "../assets/original_source_psg_file50la.h"
#include <new>
#include <array>
#include <cmath>
#include <cstring>
#include <exception>
#include <memory>
#include <limits>
#include <optional>
#include <string>

namespace rr::assets { class OriginalStandardModelGeometry50LA;
class OriginalStandardMaterialReferences50LA;class OriginalStandardMaterialTable50LA;
struct OriginalStandardMaterialPath50LA;struct OriginalStandardMaterialDiagnostic50LA; }
namespace ra_static_model_cold50LA { class Owner; }
namespace rr::game::original_gc_gameplay_lighting_candidate { class Owner; }
namespace rr::game {
class OriginalRacerModelPackets50LA;
class OriginalSourceStandardSmoker50LA;

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
    friend class OriginalUiEntityScope50KG; // Same counted Course construction/retirement authority.
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
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class OriginalWreckRacerStorage50LA;
    friend class OriginalWreckPieceStorage50LA; //Retains its same-model bounds invocation history.
    friend class OriginalRacerColdScenes50LA;
    friend class ::ra_static_model_cold50LA::Owner;
    friend class original_gc_gameplay_lighting_candidate::Owner;
    friend class OriginalRacerModelPackets50LA;
    friend class OriginalSourceStandardSmoker50LA;
    friend class OriginalSourcePhysicsModel50LA;
    friend class OriginalSourcePhysicsBody50LA;
    friend class rr::assets::OriginalSourcePsgFile50LA; //Actual source member1C withdrawal checks same-model observer borrow.
    friend class ::native_racer_model50LA::Owner;
    friend class rr::platform::OriginalUiGarageSelectedModel50KG;
    friend class rr::platform::OriginalUiGarageVisibility50KG;
    friend class rr::platform::OriginalUiGarageStaticModels50KG;
    friend class rr::platform::OriginalUiGarageModelResource50KG;
    friend class rr::platform::OriginalUiCourseOwnedLifecycle50KG;
    friend class OriginalUiEntityScope50KG;
    using S=OriginalRacerModelSceneStatus50LA;
    using R=OriginalRacerModelSceneResult50LA;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using I=SceneNodeIndex49P;
    using O=OriginalRacerSceneObject50LA;
    static constexpr I none=I::none;
    enum class RootMode { separate, direct };
    // Private source27AC2C entry: one fresh canonical model node, BEFORE the
    // actual PSG lookup can supply a row map. Tail capacity is not source data.
    struct SourceConstructor2403A0 {
        bool attempted=false,entityCounted=false,sceneReturned=false,modelReturned=false;
        bool rowsBound=false,noOp141C20Returned=false;
        SceneNode49P* canonicalNodes=nullptr;
        std::uint32_t pc=0,vtable1A8=0,vtable10=0;
        std::uint8_t knownFlags03=0,knownFlags191=0,knownFlags1F4=0;
        bool knownScenePrefix=false,knownConfiguration1A4=false,knownModelFields=false;
    } sourceConstructor_;
    R constructSourceDirectRoot2403A0(const OriginalRacerModelSceneBinding50LA& b,
        const SceneGraph49P& graph,bool sourceA1)noexcept {
        auto& s=sourceConstructor_;
        if(s.attempted||model_.constructed||released_)return {S::phase,0x2403a0u};
        if(!b.sessionGeneration||!b.actualObjectCount423F60||!b.actualAttachmentLock423F58||
           !b.actualTypes383408||b.sourceRowIndices||b.sourceRowCount||
           *b.actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max())
            return {S::binding,0x2403a0u};
        if(!validateSceneGraph49P(graph)||graph.count!=1||b.modelIndex!=static_cast<I>(0)||
           !freshNode(graph,b.modelIndex))return {S::graph,0x28947cu};
        binding_=b;rootMode_=RootMode::direct;s.attempted=true;s.canonicalNodes=graph.nodes;
        //Shared actual2893A0 writes one embeddedEntity/ref1/count once, then
        //four vf0 bounds, links, identityC0/zero100 and the SoultreeObject call.
        //No private authority enrollment is hidden here: the actual caller
        //enrolls this SAME Entity, including a counted registry-failure prefix.
        const auto result=constructScene2893A0(model_,b.modelIndex,graph,sourceA1,K::model2403A0);
        s.pc=result.sourcePC;s.knownFlags03=7u;s.entityCounted=model_.constructed;
        if(s.entityCounted){s.vtable1A8=0x3d3190;s.vtable10=0x3d3198;s.knownScenePrefix=true;}
        if(!result.okay())return result; //Do not undo earlier constructor stores/count.
        s.sceneReturned=true;s.knownFlags191=3u;s.knownConfiguration1A4=true;
        //26E3A0 does NOT retain the embeddedEntity or initialize distance1BC.
        s.vtable1A8=0x3d2390;s.vtable10=0x3d2398;
        model_.flags03=static_cast<std::uint8_t>(model_.flags03|1u); //26E3E4
        model_.flags03=static_cast<std::uint8_t>(model_.flags03|2u); //26E404
        fields_.geometry1B4=nullptr;fields_.field1B0=0;
        model_.flags191=static_cast<std::uint8_t>(model_.flags191&~1u);
        fields_.sourceRows1B8=nullptr;fields_.flags1B2=0;fields_.flags1B3=0; //SH1B2
        fields_.materialCount1C0=0;fields_.materials1C4=nullptr;fields_.field1C8=nullptr;
        fields_.flags1B3=static_cast<std::uint8_t>(fields_.flags1B3|0x10u);
        s.vtable1A8=0x3d1f40;s.vtable10=0x3d1f48;model_.dispatch1A8=K::model2403A0;
        fields_.packets1F0=nullptr;fields_.scale1E0={{1.f,1.f,1.f,1.f}};
        fields_.offset1D0={{0.f,0.f,0.f}};fields_.offsetEnabled1DC=0;
        fields_.flags1F4=static_cast<std::uint8_t>(fields_.flags1F4&~1u);s.knownFlags1F4=1u;
        fields_.field1F8=0.f;fields_.field1FC=0.f;fields_.field200=0.f;
        fields_.flags1F4=static_cast<std::uint8_t>(fields_.flags1F4&~2u);s.knownFlags1F4=3u;
        fields_.field204=0;s.knownModelFields=true;
        //Upper03/191/1F4 bits and radius194/198,distance1BC remain unknown.
        //Their native backing defaults MUST NOT serve as authored values.
        modelConstructorComplete_=true;s.modelReturned=true;s.pc=0x240444;
        return {S::complete,s.pc};
    }
    //26E848's141C20 is only JR RA/MOVE V0,A0, NOT a reference acquisition.
    R sourceNoOp141C20()noexcept {
        auto& s=sourceConstructor_;
        if(!s.modelReturned||s.noOp141C20Returned||released_||referenceReleased_||
           !model_.constructed||model_.entity8.reference00!=1u)return {S::phase,0x141c20u};
        s.noOp141C20Returned=true;return {S::complete,0x141c24u};
    }
    //Caller supplies indices ONLY after actual source resource acquisition.
    //This binds native indices, never constructs/recounts row0 or loads rows.
    R bindSourceDirectRows28CAC0(const SceneGraph49P& graph,const I* indices,
        std::uint32_t count)noexcept {
        auto& s=sourceConstructor_;
        if(!s.modelReturned||s.rowsBound||released_||referenceReleased_||rowsLoaded_||
           binding_.sourceRowCount||binding_.sourceRowIndices||graph.nodes!=s.canonicalNodes||
           !s.noOp141C20Returned||!directRootPrepared_)return {S::phase,0x28cac0u};
        if(!indices||!count||count>rr::assets::maxModelNodes49O||indices[0]!=model_.sceneIndex||
           !validateSceneGraph49P(graph)||!freshNode(graph,model_.sceneIndex))return {S::binding,0x28cb0cu};
        for(std::uint32_t row=0;row<count;++row){
            if(!freshNode(graph,indices[row]))return {S::graph,0x28cb0cu};
            for(std::uint32_t prior=0;prior<row;++prior)
                if(indices[row]==indices[prior])return {S::binding,0x28cb0cu};
        }
        for(std::uint32_t row=0;row<count;++row)indices_[row]=indices[row];
        binding_.sourceRowCount=count;binding_.sourceRowIndices=indices_.data();s.rowsBound=true;
        return {S::complete,0x28cac0u};
    }

    // Actual cold26EBCC byte cursor. This is NOT a decoded Scene48D or a
    // source numeric FileID emulator; the caller pins/authenticates provider
    // bytes and this exact resolver row before entering this private method.
    struct SourceDirectRows28CAC0 {
        std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver;
        std::shared_ptr<rr::assets::OriginalSourcePsgFile50LA> sourceFile;
        rr::assets::OriginalStandardModelFileId50LA file{};
        const std::vector<std::uint8_t>* bytes=nullptr;
        std::size_t cursor=0;
        bool attempted=false,countStored=false,rowZeroPublished=false;
        bool parentAllocationReturned=false,parentAllocationNonNull=false;
        bool returned=false,parentFreed=false,nativeScratchWithdrawn=false;
        std::optional<std::uint32_t> signature,version,returnCount;
        std::unique_ptr<std::int32_t[]> parents;
        std::array<O*,rr::assets::maxModelNodes49O> publishedRows{};
        std::array<std::array<std::uint8_t,128>,rr::assets::maxModelNodes49O> rawNames{};
        //name,matrix,minXYZ,maxXYZ,radius,parent source reads; raw name retains
        //all128 bytes independently of native std::string representation.
        std::array<std::uint8_t,rr::assets::maxModelNodes49O> readMask{};
        std::array<bool,rr::assets::maxModelNodes49O> allocationReturned{},published{},linked{};
        std::uint32_t pc=0,row=0;
    } sourceDirectRows_;
    R importSourceDirectRows28CAC0(SceneGraph49P& graph,std::uint32_t backingCapacity,
        std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver,
        const rr::assets::OriginalStandardModelFileView50LA& file,
        std::shared_ptr<rr::assets::OriginalSourcePsgFile50LA> sourceFile)noexcept {
        auto& s=sourceDirectRows_;
        if(s.attempted||!sourceConstructor_.modelReturned||!sourceConstructor_.noOp141C20Returned||
           !directRootPrepared_||sourceConstructor_.rowsBound||rowsLoaded_||referenceReleased_||released_||
           binding_.sourceRowCount||binding_.sourceRowIndices||graph.nodes!=sourceConstructor_.canonicalNodes||
           graph.count!=1||backingCapacity!=rr::assets::maxModelNodes49O||!validateSceneGraph49P(graph)||
           !freshNode(graph,model_.sceneIndex))return {S::phase,0x26ebccu};
        rr::assets::OriginalStandardModelFileView50LA current;
        rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
        //Resolver rows preserve provider spelling while26E8D8 lowercases
        //the requested path. Admit only ASCII case differences, not slashes.
        const auto sameMember=[](const std::string& a,const std::string& b){
            if(a.size()!=b.size())return false;
            for(std::size_t i=0;i<a.size();++i){auto x=static_cast<unsigned char>(a[i]);auto y=static_cast<unsigned char>(b[i]);
                if(x&0x80u||y&0x80u)return false;
                if(x>='A'&&x<='Z')x=static_cast<unsigned char>(x+0x20u);
                if(y>='A'&&y<='Z')y=static_cast<unsigned char>(y+0x20u);
                if(x!=y)return false;}
            return true;
        };
        if(!resolver||file.id.resolver!=resolver.get()||!file.member||!file.bytes||file.geometry240110||
           file.kind!=rr::assets::OriginalStandardModelFileKind50LA::psg||!sameMember(model_.name38,*file.member)||
           !resolver->modelFile(file.id,current,diagnostic)||current.member!=file.member||
           current.bytes!=file.bytes||current.geometry240110||current.kind!=file.kind)
            return {S::binding,0x26ebccu};
        if(!sourceFile||!sourceFile->current()||!sourceFile->returnedCold()||
           sourceFile->resolver()!=resolver.get()||sourceFile->nativeFile().resolver!=file.id.resolver||
           sourceFile->nativeFile().row!=file.id.row)return {S::binding,0x26ebccu};
        std::size_t initial=0;if(!sourceFile->cursor12F520(initial,diagnostic)||initial)return {S::binding,0x26ebccu};
        s.resolver=std::move(resolver);s.sourceFile=std::move(sourceFile);s.file=file.id;s.bytes=file.bytes;s.attempted=true;
        const auto at=[&](std::uint32_t pc){s.pc=pc;};
        const auto stop=[&](S status,std::uint32_t pc){at(pc);return R{status,pc};};
        const auto read=[&](void* output,std::size_t count,std::uint32_t pc){
            at(pc);rr::assets::OriginalStandardTextureDiagnostic50LA io;
            const bool okay=s.sourceFile->readExactNative(output,count,s.cursor,io);
            if(!okay&&io.sourcePC)at(io.sourcePC);return okay;
        };
        const auto word=[](const std::uint8_t* p){return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
            (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);};
        std::array<std::uint8_t,4> scalar{};
        if(!read(scalar.data(),4,0x26ebe0))return stop(S::sourceRowTable,s.pc);s.signature=word(scalar.data());
        if(!read(scalar.data(),4,0x26ebfc))return stop(S::sourceRowTable,s.pc);s.version=word(scalar.data());
        //308F00 folds only ASCII A..Z through the original ctype table.
        //Keep raw signature receipt; nonASCII signed-table lookup is unjoined.
        std::uint32_t foldedSignature=0;
        for(unsigned lane=0;lane<4;++lane){auto byte=static_cast<std::uint8_t>(*s.signature>>(8u*lane));
            if(byte&0x80u)return stop(S::hierarchy,0x26ec0c);
            if(byte>='A'&&byte<='Z')byte=static_cast<std::uint8_t>(byte+0x20u);
            foldedSignature|=std::uint32_t(byte)<<(8u*lane);
        }
        const bool instanced=foldedSignature==0x00696770u||foldedSignature==0x00696764u; //pgi/dgi
        if(instanced){at(0x26ec48);fields_.flags1B3=static_cast<std::uint8_t>(fields_.flags1B3|4u);}
        const bool ordinary=foldedSignature==0x00677370u||foldedSignature==0x00677864u; //psg/dxg
        if((!ordinary&&!instanced)||(*s.version&0x80000000u)||*s.version<3u)
            return stop(S::hierarchy,0x26eca0); //Actual assertion path remains unjoined, not loader0.
        if(*s.version!=3u)return stop(S::hierarchy,0x26ed14); //Bounded native layout adapter.
        if(!read(scalar.data(),4,0x28cb04))return stop(S::sourceRowTable,s.pc);
        model_.nodeCount1A0=word(scalar.data());s.countStored=true;
        at(0x28cb0c);s.publishedRows[0]=&model_;s.published[0]=true;s.rowZeroPublished=true;
        const auto count=model_.nodeCount1A0;
        //Source signed loop/allocation domain is broader; refuse BEFORE its
        //unrepresented oversized/zero allocation, retaining count and row0.
        if(!count||count>backingCapacity)return stop(S::sourceRowTable,0x28cb14);
        //Native index mapping is not a PSG read or child constructor. Storage
        //was reserved by the caller, and root address/index never changes.
        for(std::uint32_t row=1;row<count;++row){auto& node=graph.nodes[row];
            node.parent180=node.firstChild184=node.nextSibling188=none;}
        graph.count=count;
        std::array<I,rr::assets::maxModelNodes49O> indices{};
        for(std::uint32_t row=0;row<count;++row)indices[row]=static_cast<I>(row);
        const auto bound=bindSourceDirectRows28CAC0(graph,indices.data(),count);if(!bound.okay())return stop(bound.status,bound.sourcePC);
        directRowsStarted_=true;
        at(0x28cb14);s.parents.reset(new(std::nothrow) std::int32_t[count]);
        s.parentAllocationReturned=true;s.parentAllocationNonNull=bool(s.parents);
        for(std::uint32_t row=0;row<count;++row){s.row=row;
            if(row){
                //Native overflow is bounded before the source child allocator.
                if(*binding_.actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max())
                    return stop(S::entity,0x28d070);
                at(0x28d070);rows_[row].reset(new(std::nothrow) O);s.allocationReturned[row]=true;
                if(rows_[row]){const auto made=constructScene2893A0(*rows_[row],indices_[row],graph,true,K::scene2893A0);
                    if(!made.okay())return stop(made.status,made.sourcePC);}
                at(0x28d090);s.publishedRows[row]=rows_[row].get();s.published[row]=true;
            }
            auto* target=s.publishedRows[row];if(!target)return stop(S::sourceRowTable,0x28cb50);
            if(!read(s.rawNames[row].data(),128,0x28cb68))return stop(S::sourceRowTable,s.pc);s.readMask[row]|=1u;
            std::size_t length=0;while(length<128&&s.rawNames[row][length])++length;
            if(length==128)return stop(S::sourceRowTable,0x28cb68); //Raw write kept; no fabricated terminator.
            try{target->name38.assign(reinterpret_cast<const char*>(s.rawNames[row].data()),length);}
            catch(...){return stop(S::allocation,0x28cb68);}
            std::array<std::uint8_t,64> matrix{};
            if(!read(matrix.data(),64,0x28cb88))return stop(S::sourceRowTable,s.pc);
            for(unsigned lane=0;lane<16;++lane){const auto bits=word(matrix.data()+4*lane);
                std::memcpy(&atGraphNode(graph,target->sceneIndex).localC0.v[lane],&bits,4);}
            s.readMask[row]|=2u;
            at(0x28cbc0);target->flags03=static_cast<std::uint8_t>(target->flags03&~4u);
            at(0x28cbdc);atGraphNode(graph,target->sceneIndex).sceneFlags190|=0x20u;
            at(0x28cbf0);atGraphNode(graph,target->sceneIndex).sceneFlags190|=4u;
            std::array<std::uint8_t,12> vector{};
            if(!read(vector.data(),12,0x28cbfc))return stop(S::sourceRowTable,s.pc);
            for(unsigned lane=0;lane<3;++lane){const auto bits=word(vector.data()+4*lane);std::memcpy(&target->bounds140[lane],&bits,4);}s.readMask[row]|=4u;
            if(!read(vector.data(),12,0x28cc1c))return stop(S::sourceRowTable,s.pc);
            for(unsigned lane=0;lane<3;++lane){const auto bits=word(vector.data()+4*lane);std::memcpy(&target->bounds150[lane],&bits,4);}s.readMask[row]|=8u;
            if(!read(scalar.data(),4,0x28cc3c))return stop(S::sourceRowTable,s.pc);
            {const auto bits=word(scalar.data());float radius;std::memcpy(&radius,&bits,4);target->radius194=radius;}s.readMask[row]|=16u;
            if(!s.parents)return stop(S::allocation,0x28cc58); //Original NULL destination reached only NOW.
            if(!read(scalar.data(),4,0x28cc58))return stop(S::sourceRowTable,s.pc);
            {const auto bits=word(scalar.data());std::memcpy(&s.parents[row],&bits,4);}s.readMask[row]|=32u;
            sourceParents_[row]=s.parents[row];
        }
        for(std::uint32_t row=0;row<count;++row){s.row=row;at(0x28cc90);const auto parent=s.parents[row];
            if(parent==-1){s.linked[row]=true;continue;}
            //Bound malformed/cyclic/forward parent cases at the actual lookup,
            //not before preceding row writes. No native index is a guest ptr.
            if(!row||parent<0||static_cast<std::uint32_t>(parent)>=row)return stop(S::hierarchy,0x28cca0);
            appendScene28BEF0(graph,*s.publishedRows[static_cast<std::uint32_t>(parent)],*s.publishedRows[row]);s.linked[row]=true;
        }
        at(0x28cd28);s.parents.reset();s.parentFreed=true;
        rowsLoaded_=true;s.returnCount=count;s.returned=true;at(0x28cd5c);
        return {S::complete,s.pc}; //Only28CAC0; materials/geometry/cache/fileclose remain caller work.
    }
    static SceneNode49P& atGraphNode(const SceneGraph49P& graph,I i)noexcept{return at(graph,i);}

    struct SourceMaterials26E640 {
        std::shared_ptr<rr::assets::OriginalStandardMaterialReferences50LA> references;
        const std::string* prefixAddress=nullptr;
        const std::int32_t* published1C4=nullptr;
        bool attempted=false,countStored=false,allocationPublished=false,returned=false,nativeWithdrawn=false;
        std::uint32_t count=0,row=0,namesRead=0,lookupsReturned=0,retainsReturned=0,pc=0;
        std::array<std::uint8_t,64> rawName{};
        std::optional<std::int32_t> lookupReturn;
    } sourceMaterials_;
    R loadSourceMaterials26E640(std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>,
        const rr::assets::OriginalStandardMaterialPath50LA&,rr::assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool preflightSourceMaterialsNative()const noexcept;
    bool releaseSourceMaterialsNative()noexcept;

    struct SourceQuantization240570 {
        bool attempted=false,flagStored=false,returned=false;
        std::uint8_t readMask=0;
        std::array<std::uint32_t,8> words{};
        std::optional<bool> xEqualsOne,yEqualsOne;
        std::uint32_t pc=0;
    } sourceQuantization_;
    R loadSourceQuantization240570()noexcept;

    struct SourceBounds28E230 {
        bool entered=false,recompute=false,rootRefreshed=false,aggregateReturned=false;
        bool warmZeroCount=false; //Source1A0=0, distinct from actual enumerated1B8 count.
        bool finalWorldRefreshed=false,centerStored=false,returned=false;
        std::uint8_t flagsAtEntry=0,aggregateStoreMask=0,halfStoreMask=0;
        std::uint32_t rowCount=0,row=0,pc=0;
        OriginalStandardPlacementGlobals50LA::History profileGuard;
        std::array<std::uint8_t,rr::assets::maxModelNodes49O> rowRefreshes{};
        std::array<std::uint32_t,4> aggregate160{},aggregate170{},center{};
        std::array<std::uint32_t,3> half{};
        std::uint32_t radius198=0,halfWBefore=0;
    } sourceBounds_;
    R produceSourceDirectBounds28E230(const SceneGraph49P&,std::array<float,4>& center,
        std::array<float,4>& half)noexcept;
    bool preflightSourceBoundsHistoryNative()const noexcept;

    //Fresh post-pose28E230 invocation. Earlier28B550/28D670 receipts stay
    //immutable; this occurrence alone owns its new cache/bounds mutations.
    struct SourceObserverBounds28E230 {
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        const SceneNode49P* canonicalNodes=nullptr;
        std::uint32_t count=0,root=0,pc=0;
        bool entered=false,returned=false;
        std::array<SceneNode49P,maxSceneNodes49P> before{},after{};
        std::array<std::uint32_t,4> bounds160Before{},bounds170Before{};
        std::optional<std::uint32_t> radiusBefore;
        std::array<std::uint32_t,4> centerBefore{},halfBefore{},centerAfter{},halfAfter{};
    };
    R produceSourceObserverBounds28E230(const SceneGraph49P&,std::array<std::uint32_t,4>&,
        std::array<std::uint32_t,4>&,SourceObserverBounds28E230&)noexcept;
    bool sourceObserverBoundsHistoryNative(const SceneGraph49P&,const SourceObserverBounds28E230&)const noexcept;
    struct SourceGeometry26EE38 {
        std::unique_ptr<rr::assets::OriginalStandardModelGeometry50LA> pending;
        std::shared_ptr<rr::assets::OriginalStandardModelGeometry50LA> owner;
        bool attempted=false,allocationReturned=false,allocationNonNull=false;
        bool constructorReturned=false,published=false,nativeWithdrawn=false;
        bool loadAttempted=false,lodsReturned=false;
        bool paletteAttempted=false,geometryCallReturned=false;
        bool prototypeAttempted=false,prototypeStored=false;
        rr::assets::OriginalStandardModelGeometry50LA* published1B4=nullptr;
        std::uint32_t pc=0;
    } sourceGeometry_;
    R allocateSourceGeometry26EE38()noexcept;
    R loadSourceGeometry26E170()noexcept;
    R finishSourceGeometry240210()noexcept;
    R publishSourceGeometryPrototype26EE74()noexcept;
    bool preflightSourceGeometryNative()const noexcept;
    bool releaseSourceGeometryNative()noexcept;

    //Native same-occurrence borrow, including partial/NULL observer publication.
    //Only the actual root observer retirement may clear it; no source field.
    //Private Standard S3=0 warm26E820 prefix. No cold stream/close receipt,
    //hierarchy clone, material copy, rowtable or loader return is implied.
    struct SourceWarmGeometry26E9F0 {
        bool attempted=false,registered=false,published=false,retained=false;
        bool prototypeRead=false,nativeLedgerWithdrawn=false,nativeWithdrawn=false;
        std::uint32_t pc=0;
        std::shared_ptr<rr::assets::OriginalSourcePsgFile50LA> file;
        rr::assets::OriginalStandardModelGeometry50LA* geometry=nullptr;
        const OriginalRacerModelSceneOwner50LA* creator=nullptr;
        const OriginalRacerModelSceneOwner50LA* prototype=nullptr;
    } sourceWarmGeometry_;
    R acquireSourceWarmGeometry26E9F0(std::shared_ptr<rr::assets::OriginalSourcePsgFile50LA>)noexcept;
    bool preflightSourceWarmGeometryNative()const noexcept;
    bool withdrawSourceWarmGeometryNative()noexcept;
    bool sourceWarmPrototypeBorrowedNative()const noexcept {
        return sourceDirectRows_.sourceFile&&sourceDirectRows_.sourceFile->hasLiveWarmBorrowersNative();
    }
    bool sourceWarmLifetimeBorrowedNative()const noexcept {
        return (sourceWarmGeometry_.attempted&&!sourceWarmGeometry_.nativeWithdrawn)||
            (sourceWarmReturn_.rowAllocationAttempted&&!sourceWarmReturn_.nativeRowsWithdrawn)||
            sourceWarmPrototypeBorrowedNative();
    }
    struct SourceWarmRows26EA88 {
        bool entered=false,bitRead=false,childRead=false,cloneParentCleared=false;
        bool countSeeded=false,rowZeroStored=false,enumerationReturned=false;
        std::uint8_t bit191=0,copyMask=0,flagStoreMask=0;
        const O* selectedCloneSource=nullptr;
        std::uint32_t count=0,pc=0,radius194=0;
        std::array<std::uint32_t,4> bounds140{},bounds150{};
        std::uint8_t flagsAfter=0;
    } sourceWarmRows_;
    R continueSourceWarmRows26EA88(const SceneGraph49P&,const SceneGraph49P&)noexcept;
    bool preflightSourceWarmRowsNative()const noexcept;
    struct SourceWarmMaterials26EB44 {
        bool entered=false,countStored=false,allocationPublished=false,copyReturned=false;
        bool returned=false,nativeWithdrawn=false;
        std::uint32_t count=0,retained=0,pc=0;
        const std::int32_t* published1C4=nullptr;
        std::shared_ptr<rr::assets::OriginalStandardMaterialReferences50LA> references;
    } sourceWarmMaterials_;
    R copySourceWarmMaterials26EB40()noexcept;
    bool preflightSourceWarmMaterialsNative()const noexcept;
    bool withdrawSourceWarmMaterialsNative()noexcept;
    struct SourceWarmReturn26E820 {
        bool parametersEntered=false,parametersReturned=false;
        std::uint16_t parameterStoreMask=0;
        std::array<std::uint32_t,8> parameterWords{};
        std::uint8_t flags1B2=0,flags1B3=0;
        bool rowAllocationAttempted=false,rowAllocationReturned=false,rowPublished=false;
        bool rowCopied=false,returned=false,nativeRowsWithdrawn=false;
        std::uint32_t count=0,pc=0;
        O** published1B8=nullptr;
        const C* returnedEntity=nullptr;
    } sourceWarmReturn_;
    R copySourceWarmParameters2406E0()noexcept;
    R allocateSourceWarmRowTable26EE9C()noexcept;
    bool preflightSourceWarmReturnNative()const noexcept;
    bool withdrawSourceWarmRowTableNative()noexcept;
    bool preflightSourceWarmRenderable26E820Native()const noexcept;
    struct SourceWarmClone28E5F0 {
        struct Row {
            const O* source=nullptr; //History only after warm native withdrawal.
            bool allocationEntered=false,allocationNonNull=false,constructorReturned=false;
            bool incomingPublished=false,nameCopied=false,entered=false,returned=false;
            std::uint8_t localPairMask=0,flagStoreMask=0,boxStoreMask=0;
            bool radius194Stored=false,radius198Stored=false;
            std::optional<std::uint32_t> copiedRadius194,copiedRadius198; //Store receipt separate from value knownness.
            std::optional<std::uint32_t> siblingSlot,childSlot;
            bool siblingParentStored=false,siblingPreviousStored=false,childParentStored=false;
            bool siblingNullStored=false,childNullStored=false;
        };
        bool entered=false,returned=false,selectedChild=false,nativeForestDetached=false;
        std::uint32_t slotCount=0,pc=0;
        SceneNode49P* canonicalNodes=nullptr;
        std::array<Row,rr::assets::maxModelNodes49O> rows{};
    } sourceWarmClone_;
    R cloneSourceWarmHierarchy28E5F0(SceneGraph49P&,std::uint32_t,const SceneGraph49P&)noexcept;
    bool preflightSourceWarmCloneNative()const noexcept;
    bool preflightSourceWarmForestGraphNative(const SceneGraph49P& graph)const noexcept {
        const auto& h=sourceWarmClone_;
        if(!h.entered||!h.selectedChild)return validateSceneGraph49P(graph);
        if(graph.nodes!=h.canonicalNodes||graph.count!=h.slotCount||
           graph.count>rr::assets::maxModelNodes49O||!preflightSourceWarmCloneNative())return false;
        //Native admission only. Generic graph operations still reject root
        //siblings; the actual source graph is neither normalized nor rewritten.
        std::array<SceneNode49P,rr::assets::maxModelNodes49O> scratch{};
        for(std::uint32_t i=0;i<graph.count;++i)scratch[i]=graph.nodes[i];
        std::array<bool,rr::assets::maxModelNodes49O> incoming{};
        for(std::uint32_t i=0;i<graph.count;++i){const auto& node=graph.nodes[i];
            if(node.parent180!=none||node.nextSibling188==none)continue;
            const auto next=static_cast<std::int32_t>(node.nextSibling188);
            if(next<0||static_cast<std::uint32_t>(next)>=graph.count||incoming[next]||
               graph.nodes[next].parent180!=none||!h.rows[i].siblingSlot||
               *h.rows[i].siblingSlot!=static_cast<std::uint32_t>(next)||
               !h.rows[i].siblingParentStored||!h.rows[i].siblingPreviousStored)return false;
            const auto* owner=object(node.nextSibling188);
            if(!owner||owner->previous18C!=static_cast<I>(i))return false;
            incoming[next]=true;scratch[i].nextSibling188=none;
        }
        for(std::uint32_t i=0;i<graph.count;++i)if(graph.nodes[i].parent180==none){
            const auto* owner=object(static_cast<I>(i));
            if(owner&&owner->constructed&&(owner->previous18C!=none)!=incoming[i])return false;
        }
        return validateSceneGraph49P({scratch.data(),graph.count});
    }
    void withdrawSourceWarmForestLinksNative(const SceneGraph49P& graph)noexcept {
        auto& h=sourceWarmClone_;if(!h.entered||!h.selectedChild||h.nativeForestDetached)return;
        //Exact validated native cancellation, not source28BFC0 on a NULL parent.
        //No fallible operation follows admission before all root links clear.
        for(std::uint32_t i=0;i<graph.count;++i)if(graph.nodes[i].parent180==none){
            graph.nodes[i].nextSibling188=none;
            if(auto* owner=object(static_cast<I>(i)))owner->previous18C=none;
        }
        h.nativeForestDetached=true;
    }
    struct SourceWarmEnumeration28C400 {
        bool entered=false,returned=false,mapBound=false;
        std::uint32_t pc=0;
        std::array<O*,rr::assets::maxModelNodes49O> slots{};
        std::array<bool,rr::assets::maxModelNodes49O> written{};
    } sourceWarmEnumeration_;
    R enumerateSourceWarmRows28C400(const SceneGraph49P&)noexcept;
    bool preflightSourceWarmEnumerationNative()const noexcept;
    const void* sourcePacketBorrower_=nullptr; //Exact source packet occurrence, including unpublished/NULL1F0.
    const void* sourceObserverBorrower_=nullptr;
    const void* sourcePhysicsBodyBorrower_=nullptr; //Raw same-model source288 borrow.
    std::size_t sourceParticleEmitterBorrowers_=0; //Actual source emitter74 leases, separate from guest Entity refs.
    const void* sourceSmokerBorrower_=nullptr; //Actual nullable source model40 borrower; not a guest field.
    // Actual source28B550(Model,NULL,stack), root-only copy route. Entity
    // attachment is distinct from SceneObject.parent180 and is not rejected.
    //Exact NULL-relative, t0=0,t1=1,t2=0 branch. Nested owner supplies
    //actual Base60/70; no public prepared-pose or source completion grant.
    struct SourcePostPoseWorld28B550;
    struct SourceOrientation28A090 {
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        const SceneNode49P* canonicalNodes=nullptr;
        std::uint32_t count=0,root=0,pc=0;
        bool entered=false,guardReturned=false,scratchInitialized=false,inputsCopied=false;
        bool numericReturned=false,parentRead=false,rootMarked=false,rootCleared=false,returned=false;
        std::uint8_t lookKnown=0,upKnown=0,xyzMask=0;
        std::uint16_t matrixMask=0;
        OriginalStandardPlacementGlobals50LA::History guard;
        std::array<std::uint32_t,4> look{},up{},savedXYZ{{0,0,0,0x3f800000u}};
        std::array<std::uint32_t,16> scratch{};
        std::array<SceneNode49P,maxSceneNodes49P> before{},after{};
        std::array<std::uint32_t,maxSceneNodes49P> invalidationOrder{};
        std::uint32_t invalidationCount=0;
    };
    R orientSourceNullRelative28A090(const SceneGraph49P&,const std::uint32_t* look,
        const std::uint32_t* lookKnown,const std::uint32_t* up,const std::uint32_t* upKnown,
        SourceOrientation28A090&)noexcept;
    bool sourceOrientationHistoryNative(const SceneGraph49P&,const SourceOrientation28A090&,
        const SourcePostPoseWorld28B550* laterWorld=nullptr,
        const SourceObserverBounds28E230* laterBounds=nullptr)const noexcept;
    struct SourceTranslation28ADD0 {
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        const SceneNode49P* canonicalNodes=nullptr;
        std::uint32_t count=0,root=0,pc=0;
        bool entered=false,guardReturned=false,parentRead=false,scratchInitialized=false,inputCopied=false;
        bool rootMarked=false,rootCleared=false,returned=false;
        std::uint8_t xyzMask=0,inputKnown=0;
        OriginalStandardPlacementGlobals50LA::History guard;
        std::array<std::uint32_t,4> input{},scratch{{0,0,0,0x3f800000u}};
        std::array<SceneNode49P,maxSceneNodes49P> before{},after{};
        std::array<std::uint32_t,maxSceneNodes49P> invalidationOrder{};
        std::uint32_t invalidationCount=0;
    };
    R translateSourceNullRelative28ADD0(const SceneGraph49P&,const std::uint32_t* input,
        const std::uint32_t* known,SourceTranslation28ADD0&)noexcept;
    bool sourceTranslationHistoryNative(const SceneGraph49P&,const SourceTranslation28ADD0&,
        const SourceOrientation28A090* laterOrientation=nullptr,
        const SourcePostPoseWorld28B550* laterWorld=nullptr,
        const SourceObserverBounds28E230* laterBounds=nullptr)const noexcept;
    struct SourceLocalBounds28D670 {
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        const SceneNode49P* canonicalNodes=nullptr;
        std::uint32_t count=0,root=0,visited=0,pc=0;
        bool entered=false,rootReady=false,seedReturned=false,centerStored=false,halfStored=false,returned=false;
        std::array<SceneNode49P,maxSceneNodes49P> before{},after{};
        std::array<const O*,maxSceneNodes49P> objects{};
        std::array<std::uint32_t,maxSceneNodes49P> order{};
        std::array<std::uint8_t,maxSceneNodes49P> getterCount{};
        std::array<bool,maxSceneNodes49P> worldWritten{};
        std::array<std::array<float,4>,maxSceneNodes49P> center140{},half150{};
        SourceLocalBoundsState49O aggregate;
        std::array<SourceLocalBoundsProjection49O,maxSceneNodes49P> projection{};
        std::array<std::uint8_t,maxSceneNodes49P> projectionMask{};
        std::array<std::uint32_t,4> center{},half{};
    };
    R produceSourceLocalBounds28D670(const SceneGraph49P&,std::array<std::uint32_t,4>& center,
        std::array<std::uint32_t,4>& half,SourceLocalBounds28D670&)noexcept;
    // Private Wreck-only native caller: the actual legacy typed constructor and
    // row table authorize this graph. It does not manufacture source File/PSG
    // history or relax the distinct source-renderable entry above.
    R produceNativeWreckLocalBounds28D670(const SceneGraph49P&,std::array<std::uint32_t,4>& center,
        std::array<std::uint32_t,4>& half,SourceLocalBounds28D670&)noexcept;
    R produceBoundLocalBounds28D670Native(const SceneGraph49P&,std::array<std::uint32_t,4>& center,
        std::array<std::uint32_t,4>& half,SourceLocalBounds28D670&)noexcept;
    bool sourceLocalBoundsHistoryNative(const SceneGraph49P&,const SourceLocalBounds28D670&,
        const SourceTranslation28ADD0* laterTranslation=nullptr,
        const SourceOrientation28A090* laterOrientation=nullptr,
        const SourcePostPoseWorld28B550* laterWorld=nullptr,
        const SourceObserverBounds28E230* laterBounds=nullptr)const noexcept;
    struct SourceWorld28B550 {
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        const void* canonicalNodes=nullptr;
        std::uint32_t graphCount=0,sceneIndex=0;
        bool entered=false,refreshAttempted=false,worldReady=false,outputStored=false,returned=false;
        std::uint8_t flags190Before=0,flags190After=0;
        std::uint16_t refreshWordMask=0,outputWordMask=0;
        std::uint32_t pc=0;
        std::array<std::uint32_t,16> localBefore{},worldBefore{},world{};
    };
    R copySourceWorld28B550(const SceneGraph49P& graph,
        std::array<std::uint32_t,16>& output,SourceWorld28B550& h)noexcept {
        if(h.entered||&output==&h.localBefore||&output==&h.worldBefore||&output==&h.world||
           !preflightSourceRenderableReturn26E820Native()||
           graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))return {S::phase,0x28b550u};
        auto& node=at(graph,model_.sceneIndex);
        // Clean nonroot is outside this private root-only adapter. Dirty
        // nonroot instead retains the actual reached refresh-call boundary.
        if(node.parent180!=none&&(node.sceneFlags190&1u))return {S::graph,0x28b550u};
        h.owner=this;h.canonicalNodes=graph.nodes;h.graphCount=graph.count;
        h.sceneIndex=static_cast<std::uint32_t>(model_.sceneIndex);h.entered=true;
        h.pc=0x28b59cu;h.flags190Before=node.sceneFlags190;h.flags190After=node.sceneFlags190;
        std::memcpy(h.localBefore.data(),node.localC0.v,64);
        std::memcpy(h.worldBefore.data(),node.world100.v,64);
        if(!(node.sceneFlags190&1u)){
            h.refreshAttempted=true;
            if(node.parent180!=none){h.pc=0x28b5b8u;return {S::graph,h.pc};}
            for(unsigned pair=0;pair<8u;++pair){
                for(unsigned lane=0;lane<2u;++lane){
                    const auto i=2u*pair+lane;h.pc=lane?0x28b5f8u:0x28b5f4u;
                    std::memcpy(node.world100.v+i,node.localC0.v+i,4);
                    h.refreshWordMask|=static_cast<std::uint16_t>(1u<<i);
                }
            }
            h.pc=0x28b61cu;
            node.sceneFlags190=static_cast<std::uint8_t>((node.sceneFlags190&~1u)|1u);
            h.flags190After=node.sceneFlags190;
        }
        h.pc=0x28b624u;std::memcpy(h.world.data(),node.world100.v,64);h.worldReady=true;
        for(unsigned pair=0;pair<8u;++pair){
            for(unsigned lane=0;lane<2u;++lane){
                const auto i=2u*pair+lane;h.pc=lane?0x28b63cu:0x28b638u;
                std::memcpy(output.data()+i,node.world100.v+i,4);
                h.outputWordMask|=static_cast<std::uint16_t>(1u<<i);
            }
        }
        h.outputStored=true;h.returned=true;h.pc=0x28b67cu;
        return {S::complete,h.pc}; //Control receipt only, no invented numericV0.
    }
    bool sourceWorldHistoryCurrentNative(const SourceWorld28B550& h)const noexcept {
        if(!h.entered)return !h.owner&&!h.canonicalNodes&&!h.graphCount&&!h.sceneIndex&&
            !h.refreshAttempted&&!h.worldReady&&!h.outputStored&&!h.returned&&
            !h.flags190Before&&!h.flags190After&&!h.refreshWordMask&&!h.outputWordMask&&!h.pc;
        if(h.owner!=this||!h.canonicalNodes||!h.graphCount||h.sceneIndex>=h.graphCount)return false;
        if(!h.returned)return h.pc==0x28b5b8u&&h.refreshAttempted&&!(h.flags190Before&1u)&&
            h.flags190After==h.flags190Before&&!h.refreshWordMask&&!h.outputWordMask&&
            !h.worldReady&&!h.outputStored;
        if(h.pc!=0x28b67cu||!h.worldReady||!h.outputStored||h.outputWordMask!=0xffffu)return false;
        if(h.flags190Before&1u)return !h.refreshAttempted&&!h.refreshWordMask&&
            h.flags190After==h.flags190Before&&h.world==h.worldBefore;
        return h.refreshAttempted&&h.refreshWordMask==0xffffu&&
            h.flags190After==static_cast<std::uint8_t>(h.flags190Before|1u)&&h.world==h.localBefore;
    }

    //One genuine post-orientation28B550 occurrence, independent of earlier
    //initialization getters. Full graph snapshots pin unchanged child caches.
    struct SourcePostPoseWorld28B550 {
        bool entered=false,returned=false;
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        const SceneNode49P* canonicalNodes=nullptr;
        std::uint32_t count=0,root=0,pc=0;
        SourceWorld28B550 getter;
        std::array<SceneNode49P,maxSceneNodes49P> before{},after{};
    };
    R copySourcePostPoseWorld28B550(const SceneGraph49P& graph,
        std::array<std::uint32_t,16>& output,SourcePostPoseWorld28B550& h)noexcept {
        if(h.entered||!preflightSourceRenderableReturn26E820Native()||
           graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
           !graph.count||graph.count>maxSceneNodes49P||!validateSceneGraph49P(graph)||
           !inRange(graph,model_.sceneIndex)||at(graph,model_.sceneIndex).parent180!=none)
            return {S::phase,0x28b550u};
        h.entered=true;h.owner=this;h.canonicalNodes=graph.nodes;h.count=graph.count;
        h.root=static_cast<std::uint32_t>(model_.sceneIndex);
        for(std::uint32_t i=0;i<h.count;++i)h.before[i]=h.after[i]=graph.nodes[i];
        const auto r=copySourceWorld28B550(graph,output,h.getter);h.pc=r.sourcePC;
        for(std::uint32_t i=0;i<h.count;++i)h.after[i]=graph.nodes[i];
        h.returned=r.okay();return r;
    }
    bool sourcePostPoseWorldHistoryNative(const SceneGraph49P& graph,
        const SourcePostPoseWorld28B550& h,const SourceObserverBounds28E230* laterBounds=nullptr)const noexcept {
        if(laterBounds&&(!h.returned||!laterBounds->entered||!sourceObserverBoundsHistoryNative(graph,*laterBounds)))return false;
        if(!h.entered)return !h.owner&&!h.canonicalNodes&&!h.count&&!h.root&&!h.pc&&!h.returned&&!h.getter.entered;
        if(h.owner!=this||h.canonicalNodes!=graph.nodes||h.count!=graph.count||!h.count||
           h.count>maxSceneNodes49P||h.root>=h.count||h.root!=static_cast<std::uint32_t>(model_.sceneIndex)||
           !validateSceneGraph49P(graph)||h.before[h.root].parent180!=none)return false;
        const auto& g=h.getter;
        if(!g.entered){if(h.returned||h.pc!=0x28b550u)return false;}
        else if(!g.returned||!sourceWorldHistoryCurrentNative(g)||g.owner!=this||g.canonicalNodes!=graph.nodes||
           g.graphCount!=h.count||g.sceneIndex!=h.root||h.returned!=g.returned||h.pc!=g.pc||
           g.flags190Before!=h.before[h.root].sceneFlags190||
           std::memcmp(g.localBefore.data(),h.before[h.root].localC0.v,64)||
           std::memcmp(g.worldBefore.data(),h.before[h.root].world100.v,64))return false;
        for(std::uint32_t i=0;i<h.count;++i){
            const auto& b=h.before[i];const auto& a=h.after[i];const auto& c=laterBounds?laterBounds->before[i]:graph.nodes[i];
            if(b.parent180!=a.parent180||b.firstChild184!=a.firstChild184||b.nextSibling188!=a.nextSibling188||
               c.parent180!=a.parent180||c.firstChild184!=a.firstChild184||c.nextSibling188!=a.nextSibling188||
               std::memcmp(b.localC0.v,a.localC0.v,64)||std::memcmp(c.localC0.v,a.localC0.v,64)||
               std::memcmp(c.world100.v,a.world100.v,64)||c.sceneFlags190!=a.sceneFlags190)return false;
            const bool refreshed=i==h.root&&g.entered&&g.returned&&g.refreshAttempted;
            if(a.sceneFlags190!=(refreshed?std::uint8_t(b.sceneFlags190|1u):b.sceneFlags190)||
               std::memcmp(a.world100.v,refreshed?b.localC0.v:b.world100.v,64))return false;
            if(i==h.root&&g.entered&&(g.flags190After!=a.sceneFlags190||
               (g.worldReady&&std::memcmp(g.world.data(),a.world100.v,64))))return false;
        }
        return true;
    }

    //Distinct source occurrences for Fog loader15FF6C/15FF7C. These private
    //root-only adapters own neither the model nor its graph. The caller must
    //authenticate its same wrapper/model74 lease before each invocation.
    //Only the ready and dirty-parentless routes are admitted. Root siblings,
    //nonnull graph parents, relative getters and observer work stay bounded.
    struct SourcePosition28B1A0 {
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        const SceneNode49P* canonicalNodes=nullptr;
        std::uint32_t graphCount=0,sceneIndex=0,pc=0;
        SceneNodeIndex49P firstChild184=none;
        bool entered=false,refreshAttempted=false,worldReady=false,returned=false;
        std::uint8_t flags190Before=0,flags190After=0,outputMask=0;
        std::uint16_t refreshWordMask=0;
        std::array<std::uint32_t,16> localBefore{},worldBefore{},world{};
        std::array<std::uint32_t,3> output{}; //No emitterW or source scratchW.
    };
    struct SourceDirection28B9B0 {
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        const SceneNode49P* canonicalNodes=nullptr;
        std::uint32_t graphCount=0,sceneIndex=0,pc=0;
        SceneNodeIndex49P firstChild184=none;
        bool entered=false,refreshAttempted=false,worldReady=false;
        bool arithmeticEntered=false,arithmeticReturned=false,returned=false;
        std::uint8_t flags190Before=0,flags190After=0,outputMask=0,arithmeticMask=0;
        std::uint16_t refreshWordMask=0;
        std::array<std::uint32_t,16> localBefore{},worldBefore{},world{};
        std::array<std::uint32_t,4> input{}; //Actual full LQC2; W is copy-only.
        std::array<std::uint32_t,3> output{}; //Original caller ignores stack6C.
    };
    //Structural retained-history checks never dereference owner/canonicalNodes.
    //They remain usable after native borrow withdrawal, but are not live owner
    //authentication and do not re-execute arithmetic to fabricate receipts.
    template<class H> static bool sourceFogCacheHistoryNative(const H& h)noexcept {
        if(!h.entered){
            if(h.owner||h.canonicalNodes||h.graphCount||h.sceneIndex||h.pc||
               h.firstChild184!=none||h.refreshAttempted||h.worldReady||h.returned||
               h.flags190Before||h.flags190After||h.outputMask||h.refreshWordMask)return false;
            for(auto v:h.localBefore)if(v)return false;
            for(auto v:h.worldBefore)if(v)return false;
            for(auto v:h.world)if(v)return false;
            for(auto v:h.output)if(v)return false;
            return true;
        }
        if(!h.owner||!h.canonicalNodes||!h.graphCount||
           h.graphCount>rr::assets::maxModelNodes49O||h.sceneIndex>=h.graphCount||
           !h.worldReady||!h.pc)return false;
        if(h.firstChild184!=none&&(static_cast<std::int32_t>(h.firstChild184)<0||
           static_cast<std::uint32_t>(h.firstChild184)>=h.graphCount))return false;
        if(h.flags190Before&1u){
            if(h.refreshAttempted||h.refreshWordMask||
               h.flags190After!=h.flags190Before||h.world!=h.worldBefore)return false;
        }else if(!h.refreshAttempted||h.refreshWordMask!=0xffffu||
                 h.flags190After!=static_cast<std::uint8_t>(h.flags190Before|1u)||
                 h.world!=h.localBefore)return false;
        return true;
    }
    static bool sourcePositionHistoryNative(const SourcePosition28B1A0& h)noexcept {
        if(!sourceFogCacheHistoryNative(h))return false;
        if(!h.entered)return true;
        //This root-copy/copy-only leaf has no failure after admission. XYZ
        //stores remain independently recorded at28B268/270/27C.
        if(!h.returned||h.pc!=0x28b2acu||h.outputMask!=7u)return false;
        for(unsigned lane=0;lane<3;++lane)if(h.output[lane]!=h.world[12+lane])return false;
        return true;
    }
    static bool sourceDirectionHistoryNative(const SourceDirection28B9B0& h)noexcept {
        if(!sourceFogCacheHistoryNative(h))return false;
        if(!h.entered){
            if(h.arithmeticEntered||h.arithmeticReturned||h.arithmeticMask)return false;
            for(auto v:h.input)if(v)return false;
            return true;
        }
        if(!h.arithmeticEntered)return false;
        if(h.returned)return h.arithmeticReturned&&h.arithmeticMask==15u&&
            h.outputMask==7u&&h.pc==0x28baa4u;
        if(h.arithmeticReturned||h.outputMask)return false;
        for(auto v:h.output)if(v)return false;
        return (h.pc==0x28ba74u&&h.arithmeticMask==0u)||
            (h.pc==0x28ba78u&&h.arithmeticMask==1u)||
            (h.pc==0x28ba7cu&&h.arithmeticMask==3u)||
            (h.pc==0x28ba80u&&h.arithmeticMask==7u);
    }
    static bool sourceFogTransformHistoryNative(const SourcePosition28B1A0& position,
        const SourceDirection28B9B0& direction)noexcept {
        if(!sourcePositionHistoryNative(position)||!sourceDirectionHistoryNative(direction))return false;
        if(!direction.entered)return true;
        //Retained evidence only. No callback or mutation separates the getters.
        return position.returned&&direction.owner==position.owner&&
            direction.canonicalNodes==position.canonicalNodes&&direction.graphCount==position.graphCount&&
            direction.sceneIndex==position.sceneIndex&&direction.firstChild184==position.firstChild184&&
            !direction.refreshAttempted&&direction.flags190Before==position.flags190After&&
            direction.localBefore==position.localBefore&&direction.worldBefore==position.world;
    }
    R copySourcePosition28B1A0(const SceneGraph49P& graph,
        std::array<std::uint32_t,3>& output,SourcePosition28B1A0& h)noexcept {
        if(!sourcePositionHistoryNative(h)||h.entered||&output==&h.output||
           !preflightSourceRenderableReturn26E820Native()||
           graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
           !graph.count||graph.count>rr::assets::maxModelNodes49O||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))return {S::phase,0x28b1a0u};
        auto& node=at(graph,model_.sceneIndex);
        if(node.parent180!=none)return {S::graph,0x28b1a0u};
        h.owner=this;h.canonicalNodes=graph.nodes;h.graphCount=graph.count;
        h.sceneIndex=static_cast<std::uint32_t>(model_.sceneIndex);h.entered=true;
        h.firstChild184=node.firstChild184;
        h.pc=0x28b1e0u;h.flags190Before=h.flags190After=node.sceneFlags190;
        std::memcpy(h.localBefore.data(),node.localC0.v,64);
        std::memcpy(h.worldBefore.data(),node.world100.v,64);
        if(!(node.sceneFlags190&1u)){
            h.refreshAttempted=true;
            //28B224..248: eight pairs copied as raw words, no numeric gate.
            for(unsigned pair=0;pair<8;++pair)for(unsigned lane=0;lane<2;++lane){
                const auto i=2u*pair+lane;h.pc=lane?0x28b23cu:0x28b238u;
                std::memcpy(node.world100.v+i,node.localC0.v+i,4);
                h.refreshWordMask|=static_cast<std::uint16_t>(1u<<i);
            }
            h.pc=0x28b260u;node.sceneFlags190=static_cast<std::uint8_t>(node.sceneFlags190|1u);
            h.flags190After=node.sceneFlags190;
        }
        h.worldReady=true;std::memcpy(h.world.data(),node.world100.v,64);
        constexpr std::uint32_t stores[]{0x28b268u,0x28b270u,0x28b27cu};
        for(unsigned lane=0;lane<3;++lane){
            h.pc=stores[lane];std::memcpy(&h.output[lane],node.world100.v+12+lane,4);
            output[lane]=h.output[lane];h.outputMask|=static_cast<std::uint8_t>(1u<<lane);
        }
        //Relative=NULL at28B278. No inverse call, no fourth-lane output store.
        h.pc=0x28b2acu;h.returned=true;return {S::complete,h.pc};
    }
    R transformSourceDirection28B9B0(const SceneGraph49P& graph,
        const std::array<std::uint32_t,4>& input,std::array<std::uint32_t,3>& output,
        SourceDirection28B9B0& h)noexcept {
        if(!sourceDirectionHistoryNative(h)||h.entered||&input==&h.input||&output==&h.output||
           !preflightSourceRenderableReturn26E820Native()||
           graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
           !graph.count||graph.count>rr::assets::maxModelNodes49O||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))return {S::phase,0x28b9b0u};
        auto& node=at(graph,model_.sceneIndex);
        if(node.parent180!=none)return {S::graph,0x28b9b0u};
        h.owner=this;h.canonicalNodes=graph.nodes;h.graphCount=graph.count;
        h.sceneIndex=static_cast<std::uint32_t>(model_.sceneIndex);h.entered=true;
        h.firstChild184=node.firstChild184;
        h.pc=0x28b9d8u;h.flags190Before=h.flags190After=node.sceneFlags190;
        std::memcpy(h.localBefore.data(),node.localC0.v,64);
        std::memcpy(h.worldBefore.data(),node.world100.v,64);
        if(!(node.sceneFlags190&1u)){
            h.refreshAttempted=true;
            for(unsigned pair=0;pair<8;++pair)for(unsigned lane=0;lane<2;++lane){
                const auto i=2u*pair+lane;h.pc=lane?0x28ba34u:0x28ba30u;
                std::memcpy(node.world100.v+i,node.localC0.v+i,4);
                h.refreshWordMask|=static_cast<std::uint16_t>(1u<<i);
            }
            h.pc=0x28ba58u;node.sceneFlags190=static_cast<std::uint8_t>(node.sceneFlags190|1u);
            h.flags190After=node.sceneFlags190;
        }
        h.pc=0x28ba60u;h.worldReady=true;std::memcpy(h.world.data(),node.world100.v,64);
        h.pc=0x28ba70u;h.input=input;
        VisualVector49R in{};std::memcpy(in.v,input.data(),16);
        std::array<float,3> candidate{};h.arithmeticEntered=true;
        if(!detail::cleanMatrixDirectionXYZPrefix49U(node.world100,in,candidate,h.pc)){
            h.arithmeticMask=h.pc==0x28ba80u?7u:(h.pc==0x28ba7cu?3u:(h.pc==0x28ba78u?1u:0u));
            return {S::graph,h.pc}; //The earlier real cache writes survive.
        }
        h.arithmeticReturned=true;h.arithmeticMask=15u;
        //Original28BA8C stores four lanes, but this loader's projection retains
        //only XYZ. There is deliberately no outputW value or knownness claim.
        h.pc=0x28ba8cu;std::memcpy(h.output.data(),candidate.data(),12);
        output=h.output;h.outputMask=7u;
        h.pc=0x28baa4u;h.returned=true;return {S::complete,h.pc};
    }
    bool sourceFogTransformGraphCurrentNative(const SceneGraph49P& graph,
        const SourcePosition28B1A0& position,const SourceDirection28B9B0& direction)const noexcept {
        if(!sourceFogTransformHistoryNative(position,direction))return false;
        //An admission refusal leaves no source graph receipt to authenticate.
        //The outer caller still checks the actual same-wrapper model lease.
        if(!position.entered)return true;
        if(!preflightSourceRenderableReturn26E820Native()||
           graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
           !graph.count||graph.count>rr::assets::maxModelNodes49O||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))return false;
        const auto& node=at(graph,model_.sceneIndex);
        if(node.parent180!=none)return false;
        const auto current=[&](const auto& h)noexcept {
            return h.owner==this&&h.canonicalNodes==graph.nodes&&h.graphCount==graph.count&&
                h.sceneIndex==static_cast<std::uint32_t>(model_.sceneIndex)&&
                h.firstChild184==node.firstChild184&&h.flags190After==node.sceneFlags190&&
                !std::memcmp(h.localBefore.data(),node.localC0.v,64)&&
                !std::memcmp(h.world.data(),node.world100.v,64);
        };
        if(!current(position))return false;
        return !direction.entered||current(direction);
    }

    struct SourcePoint28BAB0 {
        const OriginalRacerModelSceneOwner50LA* owner=nullptr;
        bool entered=false,refreshAttempted=false,worldReady=false,arithmeticReturned=false,outputStored=false,returned=false;
        std::uint8_t flags190Before=0;std::uint32_t pc=0;
        std::array<std::uint32_t,4> input{},output{};
        std::array<std::uint32_t,16> world{};
    };
    R transformSourcePoint28BAB0(const SceneGraph49P& graph,const std::array<std::uint32_t,4>& input,
        std::array<std::uint32_t,4>& output,SourcePoint28BAB0& h)noexcept {
        if(h.entered||&input==&output||!preflightSourceRenderableReturn26E820Native()||
           graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))return {S::phase,0x28bab0u};
        h.entered=true;h.owner=this;h.input=input;
        auto& node=at(graph,model_.sceneIndex);h.pc=0x28bad8;h.flags190Before=node.sceneFlags190;
        if(!(node.sceneFlags190&1u)){
            h.refreshAttempted=true;h.pc=node.parent180==none?0x28bb24u:0x28baf4u;
            //Actual same graph lazy caches, not a prepared world copy. Earlier
            //ancestor writes survive a later bounded native refresh refusal.
            if(!refreshSceneWorld49P(graph,model_.sceneIndex))return {S::graph,h.pc};
        }
        h.pc=0x28bb60;h.worldReady=true;std::memcpy(h.world.data(),node.world100.v,64);
        VisualVector49R in{},candidate{};std::memcpy(in.v,input.data(),16);
        detail::SceneVectorContext49U scratch{};
        h.pc=0x28bb78;
        //Existing exact operation-sequence native finite/nearest adapter:
        //inputW and matrixW lanes are not consumed; source outputW is1.
        //No claim of EE/VU exceptional behavior or general bit parity.
        if(!detail::cleanMatrixPointPrefix49U(node.world100,in,scratch,candidate))return {S::graph,h.pc};
        h.arithmeticReturned=true;std::memcpy(h.output.data(),candidate.v,16);
        h.pc=0x28bb94;output=h.output;h.outputStored=true;
        h.pc=0x28bbac;h.returned=true;return {S::complete,h.pc};
    }
    bool sourcePointHistoryCurrentNative(const SourcePoint28BAB0& h)const noexcept {
        if(!h.entered)return !h.owner&&!h.refreshAttempted&&!h.worldReady&&!h.arithmeticReturned&&!h.outputStored&&!h.returned&&!h.pc;
        if(h.owner!=this||!h.pc||(h.worldReady&&!h.refreshAttempted&&!(h.flags190Before&1u))||
           (h.arithmeticReturned&&!h.worldReady)||(h.outputStored&&!h.arithmeticReturned))return false;
        return !h.returned||(h.outputStored&&h.output[3]==0x3f800000u&&h.pc==0x28bbacu);
    }

    struct SourceRowTable26EE9C {
        bool attempted=false,allocationReturned=false,published=false,copied=false,returned=false,nativeWithdrawn=false;
        std::uint32_t count=0,pc=0;
        const rr::assets::OriginalSourcePsgFile50LA* closedFile=nullptr;
        std::int32_t ignoredCloseResult=0;
        O* const* published1B8=nullptr;
        const C* returnedEntityV0=nullptr;
    } sourceRowTable_;
    R allocateClosedSourceRowTable26EE9C()noexcept;
    bool preflightSourceRowTableNative()const noexcept;
    bool withdrawSourceRowTableNative()noexcept;
    bool preflightSourceRenderableReturn26E820Native()const noexcept;
    const C* sourceRenderableEntity26E820Native()const noexcept;
    struct SourcePacketPaletteBorrow240210 {
        const void* descriptor20=nullptr;
        std::optional<std::uint32_t> count00;
        const std::uint32_t* sizes04=nullptr;
    };
    bool borrowSourcePacketPalette240210Native(const rr::assets::OriginalStandardModelGeometry50LA*,
        SourcePacketPaletteBorrow240210&)const noexcept;

    // Only the allocating wreck owner can capture this stable cold identity.
    // It is not evidence that2403A0 or a PSG row constructor has executed.
    C* wreckModelStorage()noexcept{return &model_.entity8;}
    C* wreckRowStorage(std::uint32_t row)noexcept {
        auto* object=sourceRowObject(row);return object?&object->entity8:nullptr;
    }

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
        // Native constructor custody only, never sourceConstructor_ receipts.
        // The retained graph allocation must outlive this exact model owner.
        nativeModelNodes_=graph.nodes;nativeModelNodeCount_=graph.count;
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
    // Actual warm virtual98=28E5D0 ->9C=28E5F0 hierarchy copy.
    // This is NOT re-reading PSG locals: a retained cache prototype may already
    // contain sampled motion/placement. Copy its actual local/bounds/flags and
    // allocate siblings BEFORE children, then enumerate28C400 into actual1B8
    // order. The genuine geometry104960/material26EB90 owners run separately.
    // Bounded source branches: separate->separate26EA28, non-childless
    // direct->direct26EB10, and26EA9C: a direct (a2=0) user whose prototype is
    // a separate-root model (191 bit1), e.g. a wreck piece warm-hitting the
    // racer's own chariot. 26EA9C runs virtual98(this,prototype.child184): the
    // source root is the prototype's distinct root row, the destination is this
    // model's row0, and no1A0 copy follows (only the a2=1 path26EA44 copies it).
    // Childless direct->direct remains its exact API above.
    R loadWarmHierarchy26EA28(const SceneGraph49P& graph,
        const OriginalRacerModelSceneOwner50LA& prototype,
        const SceneGraph49P& prototypeGraph)noexcept {
        const bool selectedChild26EA9C=rootMode_==RootMode::direct&&
            prototype.rootMode_==RootMode::separate&&(prototype.model_.flags191&2u);
        if(!modelConstructorComplete_||rowsLoaded_||referenceReleased_||released_||
           &prototype==this||(rootMode_!=prototype.rootMode_&&!selectedChild26EA9C)||!prototype.rowsLoaded_||
           prototype.referenceReleased_||prototype.released_||(!selectedChild26EA9C&&binding_.sourceRowCount<2)||
           binding_.sourceRowCount!=prototype.binding_.sourceRowCount||
           binding_.sessionGeneration!=prototype.binding_.sessionGeneration||
           binding_.actualObjectCount423F60!=prototype.binding_.actualObjectCount423F60||
           binding_.actualTypes383408!=prototype.binding_.actualTypes383408||
           !validateSceneGraph49P(graph)||!validateSceneGraph49P(prototypeGraph))
            return {S::phase,0x26ea28u};
        auto* root=sourceRowObject(0);const auto* priorRoot=prototype.sourceRowObject(0);
        if(!root||!priorRoot||!root->constructed||!priorRoot->constructed||
           at(graph,root->sceneIndex).firstChild184!=none||at(graph,root->sceneIndex).nextSibling188!=none||
           at(prototypeGraph,priorRoot->sceneIndex).nextSibling188!=none||
           (rootMode_==RootMode::separate?
              (at(graph,root->sceneIndex).parent180!=model_.sceneIndex||!(prototype.model_.flags191&2u)):
              (!directRootPrepared_||directRowsStarted_||at(graph,model_.sceneIndex).parent180!=none||
               (!selectedChild26EA9C&&(prototype.model_.flags191&2u)))))return {S::hierarchy,0x28e5d0u};
        //28C400 appends ALL immediate children, then their children in source
        // sibling order, recursively repeating the same two-level procedure.
        std::array<const O*,rr::assets::maxModelNodes49O> originals{};
        originals[0]=priorRoot;std::uint32_t count=1;
        const auto enumerate=[&](auto&& self,const O* parent)->bool {
            auto c=at(prototypeGraph,parent->sceneIndex).firstChild184;
            for(auto n=c;n!=none;n=at(prototypeGraph,n).nextSibling188){
                const auto* o=prototype.object(n);if(!o||count==binding_.sourceRowCount)return false;
                for(std::uint32_t i=0;i<count;++i)if(originals[i]==o)return false;
                originals[count++]=o;
            }
            for(auto n=c;n!=none;n=at(prototypeGraph,n).nextSibling188){
                auto g=at(prototypeGraph,n).firstChild184;
                for(auto k=g;k!=none;k=at(prototypeGraph,k).nextSibling188){
                    const auto* o=prototype.object(k);if(!o||count==binding_.sourceRowCount)return false;
                    for(std::uint32_t i=0;i<count;++i)if(originals[i]==o)return false;
                    originals[count++]=o;
                }
                for(auto k=g;k!=none;k=at(prototypeGraph,k).nextSibling188)
                    if(!self(self,prototype.object(k)))return false;
            }
            return true;
        };
        if(!enumerate(enumerate,priorRoot)||count!=binding_.sourceRowCount)return {S::hierarchy,0x28c400u};
        std::array<std::unique_ptr<O>,rr::assets::maxModelNodes49O> backing;
        std::array<std::string,rr::assets::maxModelNodes49O> names;
        std::array<std::int32_t,rr::assets::maxModelNodes49O> parents{};
        const auto rowFor=[&](I index)->std::int32_t {
            for(std::uint32_t i=0;i<count;++i)if(originals[i]->sceneIndex==index)return static_cast<std::int32_t>(i);
            return -1;
        };
        try {
            for(std::uint32_t i=0;i<count;++i){
                const auto* o=originals[i];if(!o->constructed||o->released||o->observer19C||o->configuration1A4)
                    return {S::hierarchy,0x28e610u};
                names[i]=o->name38;parents[i]=i?rowFor(at(prototypeGraph,o->sceneIndex).parent180):-1;
                if(i){if(parents[i]<0||!freshNode(graph,indices_[i]))return {S::hierarchy,0x28e7a8u};
                    backing[i]=std::make_unique<O>();}
            }
        }catch(...){return {S::allocation,0x28d060u};}
        if(rootMode_==RootMode::direct)directRowsStarted_=true;
        // Source allocation visitation: current, next-sibling subtree, child.
        // Backing is preallocated above; only these reached constructors count.
        const auto clone=[&](auto&& self,std::uint32_t i)->R {
            if(i){rows_[i]=std::move(backing[i]);auto r=constructScene2893A0(*rows_[i],indices_[i],graph,true,K::scene2893A0);
                if(!r.okay())return r;}
            auto& dst=*sourceRowObject(i);const auto& src=*originals[i];
            auto& dn=at(graph,dst.sceneIndex);const auto& sn=at(prototypeGraph,src.sceneIndex);
            dst.name38.swap(names[i]);dn.localC0=sn.localC0;
            dn.sceneFlags190=static_cast<std::uint8_t>((dn.sceneFlags190&~(1u|4u|32u|64u))|(sn.sceneFlags190&(4u|32u|64u)));
            dst.flags03=static_cast<std::uint8_t>((dst.flags03&~4u)|(src.flags03&4u));
            dst.bounds140=src.bounds140;dst.bounds150=src.bounds150;dst.radius194=src.radius194;
            dst.bounds160=src.bounds160;dst.bounds170=src.bounds170;dst.radius198=src.radius198;
            sourceParents_[i]=parents[i];
            if(sn.nextSibling188!=none){const auto k=rowFor(sn.nextSibling188);if(k<0)return {S::hierarchy,0x28e764u};
                auto r=self(self,static_cast<std::uint32_t>(k));if(!r.okay())return r;}
            if(sn.firstChild184!=none){const auto k=rowFor(sn.firstChild184);if(k<0)return {S::hierarchy,0x28e7b0u};
                auto r=self(self,static_cast<std::uint32_t>(k));if(!r.okay())return r;}
            return {S::complete,0x28e7e0u};
        };
        const auto result=clone(clone,0);if(!result.okay())return result;
        for(std::uint32_t i=1;i<count;++i)appendScene28BEF0(graph,*sourceRowObject(static_cast<std::uint32_t>(parents[i])),*sourceRowObject(i));
        //26EA40 restores the old parent after28E5D0 cleared it. Native root
        // retained that same parent throughout; no observer/entity attach ran.
        if(rootMode_==RootMode::separate)model_.nodeCount1A0=prototype.model_.nodeCount1A0;
        warmHierarchyRows_=true;rowsLoaded_=true;return {S::complete,0x26eb3cu};
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
    bool loadedFreshRows28CAC0()const noexcept{return rowsLoaded_&&!warmChildlessRows_&&!warmHierarchyRows_&&!released_;}
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

    // Character11A750 derives the1536F0 prefix, but owns a distinct actual
    // Entity dispatch kind. Do not weaken appendToAnimation's exact guard.
    R appendToCharacter154348(const OriginalWreckComponentDomain50LA& outside,
        C& character)noexcept {
        if(!modelConstructorComplete_||!rowsLoaded_||referenceReleased_||released_||
           character.kind!=K::character11A750)return {S::phase,0x154348u};
        std::array<C*,maxOriginalWreckComponents50LA> list{};
        OriginalWreckComponentDomain50LA joined{};
        if(!joinDomain(outside,list,joined,nullptr,nullptr,nullptr,nullptr))return {S::entity,0x154348u};
        const auto result=appendOriginalWreckComponent50LA(joined,character,model_.entity8,0,
            *binding_.actualAttachmentLock423F58);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)
            return {result.status==OriginalWreckComponentStatus50LA::attachmentLocked?
                S::attachmentLocked:S::entity,result.sourcePC};
        publishedToAnimation_=true;return {S::complete,0x154348u};
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
            if(!source||source==&model_)continue;
            const auto& object=*source;
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

    C& uiScopeModelEntity8()noexcept{return model_.entity8;}

    // The actual Application authority may contain several constructor
    // generations. Its private synchronous View is the only issuer of this
    // path; never rewrite ancestors into a single-generation model domain.
    template<class ActualAuthorityView>
    R preflightModelReference142590InAuthority(const ActualAuthorityView& actual)const noexcept {
        if(sourceParticleEmitterBorrowers_||sourcePhysicsBodyBorrower_||sourcePacketBorrower_||sourceWarmLifetimeBorrowedNative())return {S::phase,0x142590u};
        if(!model_.constructed||referenceReleased_||released_)return {S::phase,0x142590u};
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x1425a8u};
        const auto& c=model_.entity8;
        if(!domain(actual)||!contains(actual,&c)||c.reference00!=1u||
           c.kind!=K::model2403A0||c.firstChild18)return {S::entity,0x1425b8u};
        if(c.previous10&&(!contains(actual,c.previous10)||c.previous10->next14!=&c||
           c.previous10->parent1C!=c.parent1C))return {S::entity,0x142604u};
        if(c.next14&&(!contains(actual,c.next14)||c.next14->previous10!=&c||
           c.next14->parent1C!=c.parent1C))return {S::entity,0x14261cu};
        if(c.parent1C){if(!contains(actual,c.parent1C)||
           (!c.previous10&&c.parent1C->firstChild18!=&c))return {S::entity,0x14262cu};}
        else if(c.previous10||c.next14)return {S::entity,0x14262cu};
        return {S::complete,0x142590u};
    }
    // Only the scope calls immediately after actual authority preflight and
    // enrollment retirement in the same serial operation. No public token.
    R releasePreflightedModelReference142590()noexcept {
        if(sourceParticleEmitterBorrowers_||sourcePhysicsBodyBorrower_||sourcePacketBorrower_||sourceWarmLifetimeBorrowedNative())return {S::phase,0x142590u};
        if(!model_.constructed||referenceReleased_||released_||
           *binding_.actualAttachmentLock423F58||model_.entity8.reference00!=1u)
            return {S::phase,0x142590u};
        auto& c=model_.entity8;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        --c.reference00;referenceReleased_=true;return {S::complete,0x142654u};
    }

public:
    R preflightModelReference142590(const OriginalWreckComponentDomain50LA& outside,
        const OriginalRacerModelSceneOwner50LA* sibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* secondSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* thirdSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* fourthSibling=nullptr)const noexcept {
        using namespace original_wreck_component_detail50LA;
        if(sourceParticleEmitterBorrowers_||sourcePhysicsBodyBorrower_||sourcePacketBorrower_||sourceWarmLifetimeBorrowedNative())return {S::phase,0x142590u};
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
        if(sourceParticleEmitterBorrowers_||sourcePhysicsBodyBorrower_||sourcePacketBorrower_||sourceWarmLifetimeBorrowedNative())return {S::phase,0x142590u};
        if(sourceRowTable_.attempted&&!sourceRowTable_.nativeWithdrawn)return {S::phase,0x26e5ccu};
        if(!referenceReleased_||released_||rowTableFreed_)return {S::phase,0x26e5ccu};
        rowTable_.reset();rowTableFreed_=true;
        //26E5D8 does not clear model1B8. Its stale value is not dereferenced
        // again during this disposal sequence. Native ownership is separate.
        return {S::complete,0x26e5dcu};
    }

    // Only2896D0, after upstream240450 packets and26E460 material/resource
    // release and array frees. This method never claims those owners ran.
    R releaseSceneBase2896D0(const SceneGraph49P& graph)noexcept {
        if(sourceParticleEmitterBorrowers_||sourcePhysicsBodyBorrower_||sourcePacketBorrower_||sourceWarmLifetimeBorrowedNative())return {S::phase,0x142590u};
        if(sourceRowTable_.attempted&&!sourceRowTable_.nativeWithdrawn)return {S::phase,0x26e5ccu};
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
private:
    // Native cancellation of a genuine private model prefix, not an original
    // error branch. Storage separately admits Entity links and concrete payload
    // owners before releaseModelReference142590. Unlike closeFreshUnlinked this
    // permits acquired payloads and the actual piece->model Entity attachment;
    // neither attachment nor payload completion is inferred from scene rows.
    R preflightNativePartialScene2896D0(const SceneGraph49P& graph,const void* expectedObserver=nullptr,
        const C* expectedSourceChild=nullptr)const noexcept {
        if(released_||!model_.constructed||model_.released||publishedToAnimation_||
           model_.observer19C!=expectedObserver||model_.configuration1A4||fields_.field1C8)
            return {S::phase,0x2896d0u};
        const auto& c=model_.entity8;
        if(c.reference00!=(referenceReleased_?0u:1u)||c.firstChild18!=expectedSourceChild||
           c.kind!=K::model2403A0||c.sessionGeneration!=binding_.sessionGeneration)
            return {S::entity,0x142590u};
        if(!binding_.actualObjectCount423F60||!binding_.actualAttachmentLock423F58||
           !preflightSourceWarmForestGraphNative(graph)||!inRange(graph,model_.sceneIndex)||
           at(graph,model_.sceneIndex).parent180!=none||
           (at(graph,model_.sceneIndex).nextSibling188!=none&&!sourceWarmClone_.selectedChild)||model_.previous18C!=none)
            return {S::binding,0x289704u};
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x1425a8u};
        std::size_t count=1;
        // rows_ contains only separately allocated rows; direct row0 is model_.
        // Construction can stop before the hierarchy-link pass or row table.
        for(const auto& row:rows_)if(row&&row->constructed){
            const auto& entity=row->entity8;
            if(row->released||entity.reference00!=1u||entity.kind!=K::scene2893A0||
               entity.sessionGeneration!=binding_.sessionGeneration||entity.parent1C||
               entity.previous10||entity.next14||entity.firstChild18||row->observer19C||
               row->configuration1A4||!inRange(graph,row->sceneIndex))return {S::entity,0x28972cu};
            ++count;
        }
        if(*binding_.actualObjectCount423F60<count)return {S::reference,0x1413e0u};
        std::size_t seen=0;
        if(!admitSceneSubtree(graph,model_,seen,count))return {S::graph,0x289744u};
        for(const auto& row:rows_)if(row&&row->constructed&&
            at(graph,row->sceneIndex).parent180==none){
            if((!sourceWarmClone_.selectedChild&&(row->previous18C!=none||at(graph,row->sceneIndex).nextSibling188!=none))||
               !admitSceneSubtree(graph,*row,seen,count))return {S::graph,0x289744u};
        }
        if(seen!=count)return {S::graph,0x289744u};
        return {S::complete,0x2896d0u};
    }
    // Only after the real142590 last-reference drop,240450/26E460 payload
    // disposal,26E5CC row-table free and26E5EC material-array free. Native
    // backing owners enforce those payload phases; stale source aliases remain.
    R releaseNativePartialSceneBase2896D0(const SceneGraph49P& graph)noexcept {
        if(sourceParticleEmitterBorrowers_||sourcePhysicsBodyBorrower_||sourcePacketBorrower_||sourceWarmLifetimeBorrowedNative())return {S::phase,0x142590u};
        if(sourceRowTable_.attempted&&!sourceRowTable_.nativeWithdrawn)return {S::phase,0x26e5ccu};
        if(!referenceReleased_||rowTable_||!rowTableFreed_)return {S::phase,0x2896d0u};
        const auto admitted=preflightNativePartialScene2896D0(graph);
        if(!admitted.okay())return admitted;
        withdrawSourceWarmForestLinksNative(graph);
        // Detached constructed rows never entered the model's184 forest.
        // Retire each genuine root once, retaining2896D0 order within it.
        for(auto& row:rows_)if(row&&row->constructed&&!row->released&&
            at(graph,row->sceneIndex).parent180==none){
            --row->entity8.reference00;destroyScene2896D0(graph,*row);
        }
        destroyScene2896D0(graph,model_);released_=true;
        return {S::complete,0x2897acu};
    }

    R preflightFreshUnlinkedNative(const SceneGraph49P& graph)const noexcept {
        if(sourceParticleEmitterBorrowers_||sourcePhysicsBodyBorrower_||sourcePacketBorrower_||sourceWarmLifetimeBorrowedNative())return {S::phase,0x142590u};
        if(sourceRowTable_.attempted&&!sourceRowTable_.nativeWithdrawn)return {S::phase,0x26e5ccu};
        if(released_||!model_.constructed)return {S::complete,0};
        const auto& c=model_.entity8;
        if(publishedToAnimation_||fields_.geometry1B4||fields_.materials1C4||fields_.packets1F0||
           fields_.field1C8||model_.observer19C||
           c.parent1C||c.previous10||c.next14||c.firstChild18||referenceReleased_||c.reference00!=1u)
            return {S::phase,0x240450u};
        if(!binding_.actualObjectCount423F60||!binding_.actualAttachmentLock423F58||
           !preflightSourceWarmForestGraphNative(graph)||!inRange(graph,model_.sceneIndex)||
           at(graph,model_.sceneIndex).parent180!=none||(at(graph,model_.sceneIndex).nextSibling188!=none&&!sourceWarmClone_.selectedChild))
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
            if((!sourceWarmClone_.selectedChild&&(row->previous18C!=none||at(graph,row->sceneIndex).nextSibling188!=none))||
               !admitSceneSubtree(graph,*row,seen,count))return {S::graph,0x289744u};
        }
        if(seen!=count)return {S::graph,0x289744u};
        return {S::complete,0};
    }
public:
    R closeFreshUnlinked(const SceneGraph49P& graph)noexcept {
        const auto admitted=preflightFreshUnlinkedNative(graph);if(!admitted.okay())return admitted;
        if(released_||!model_.constructed)return {S::complete,0};
        if(sourceDirectRows_.attempted&&!sourceDirectRows_.nativeScratchWithdrawn){
            //Native cancellation of the synchronous byte adapter, not source
            //28CD28 or File131040. Historical read/publication receipts survive.
            sourceDirectRows_.parents.reset();sourceDirectRows_.bytes=nullptr;
            sourceDirectRows_.resolver.reset();sourceDirectRows_.sourceFile.reset();sourceDirectRows_.nativeScratchWithdrawn=true;
        }
        auto& c=model_.entity8;
        withdrawSourceWarmForestLinksNative(graph);
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
    // Normal Garage destructor continuation. Public/null-observer cleanup
    //is deliberately unchanged. The concrete resource owner reaches these
    //only after240450/26E460 payloads and the real Entity last-reference drop.
    bool garageSceneChildrenRetired_=false;
    R retireGarageSceneChildren2896D0(const SceneGraph49P& graph,const void* observer)noexcept {
        if(!referenceReleased_||released_||rowTable_||!rowTableFreed_||garageSceneChildrenRetired_)
            return {S::phase,0x2896D0u};
        const auto check=preflightRetainedScene2896D0(graph,observer);
        if(!check.okay())return check;
        model_.dispatch1A8=K::scene2893A0;
        auto& node=at(graph,model_.sceneIndex);
        //Preflight proves this real root has no scene-parent. Child order is
        //the source next-after-first loop, not reverse allocation order.
        while(node.firstChild184!=none&&at(graph,node.firstChild184).nextSibling188!=none){
            auto& next=*object(at(graph,node.firstChild184).nextSibling188);
            --next.entity8.reference00;destroyScene2896D0(graph,next);
        }
        if(node.firstChild184!=none){
            auto& first=*object(node.firstChild184);
            --first.entity8.reference00;destroyScene2896D0(graph,first);
        }
        garageSceneChildrenRetired_=true;return {S::complete,0x289760u};
    }
    R finishGarageSceneBase1413E0(const SceneGraph49P& graph,const void* retiredObserver)noexcept {
        if(!garageSceneChildrenRetired_||!referenceReleased_||released_||rowTable_||
           !rowTableFreed_||!model_.constructed||model_.released||model_.entity8.reference00||
           model_.observer19C!=retiredObserver||model_.configuration1A4||
           !binding_.actualObjectCount423F60||!*binding_.actualObjectCount423F60||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))return {S::phase,0x289778u};
        const auto& node=at(graph,model_.sceneIndex);
        if(node.parent180!=none||node.nextSibling188!=none||node.firstChild184!=none)
            return {S::graph,0x289778u};
        //The caller already performed280920. Source never clears19C here.
        --*binding_.actualObjectCount423F60;model_.released=true;released_=true;
        return {S::complete,0x2897ACu};
    }

    // Garage a2=0 aliases source row0 to this model. Cold28CB04 writes1A0=1;
    // warm childless26EAC4 leaves the constructor1A0=0. Both retain one1B8
    // source row. Cold26F6F8 visits that SAME root world once after the local
    // box seed; warm26F70C visits zero rows. Never multiply row0 local again.
    R publishDirectRootBounds28E110(const SceneGraph49P& graph)noexcept {
        if(!constructed2403A0()||rootMode_!=RootMode::direct||!rowsLoaded_||referenceReleased_||
           binding_.sourceRowCount!=1||(!loadedFreshRows28CAC0()&&!warmChildlessRows_)||
           model_.nodeCount1A0!=(warmChildlessRows_?0u:1u)||!rowTable_||
           fields_.sourceRows1B8!=rowTable_.get()||rowTable_[0]!=&model_||sourceRowObject(0)!=&model_||
           !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))return {S::phase,0x28e110u};
        auto& root=at(graph,model_.sceneIndex);const auto flags=root.sceneFlags190;
        if(flags&0x10u)return {S::phase,0x26f4c0u};
        const bool recompute=(flags&0x80u)&&((flags&8u)||!(flags&0x40u));
        if(!recompute){
            if((flags&0x80u)&&!(flags&1u)&&!refreshSceneWorld49P(graph,model_.sceneIndex))return {S::graph,0x2897b0u};
            return model_.radius198?R{S::complete,0x28e218u}:R{S::phase,0x28e1f8u};
        }
        OriginalStandardPlacementGlobals50LA::boundsNativeRouteUnknown();
        if(!(flags&4u)||!refreshSceneWorld49P(graph,model_.sceneIndex))return {S::graph,0x26f608u};
        ModelBoundsContext49O context{};context.rootWorld100=root.world100;context.rootSceneFlags190=root.sceneFlags190;
        for(unsigned axis=0;axis<3;++axis){context.rootCenter140.v[axis]=model_.bounds140[axis];
            context.rootHalfExtent150.v[axis]=model_.bounds150[axis];}
        ModelBoundsWorkspace49O scratch{};ModelBoundsResult49O result{};
        if(warmChildlessRows_){
            if(!computeModelBounds49O(nullptr,0,context,scratch,result))return {S::graph,0x26f718u};
        }else {
            ModelBoundsNode49O row{};row.local=root.localC0;row.parent=-1;row.sceneFlags190=root.sceneFlags190;
            row.center140=context.rootCenter140;row.halfExtent150=context.rootHalfExtent150;
            if(!computeDirectRootModelBounds50KG(&row,1,context,scratch,result))return {S::graph,0x26f6f8u};
        }
        std::memcpy(model_.bounds160.data(),result.center160,sizeof result.center160);
        std::memcpy(model_.bounds170.data(),result.halfExtent170,sizeof result.halfExtent170);
        model_.radius198=result.radius198;root.sceneFlags190=result.rootSceneFlags190;
        return {S::complete,0x26f844u};
    }

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
        OriginalStandardPlacementGlobals50LA::boundsNativeRouteUnknown();
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
    //Actual direct Model virtual90=26F4B0, NOT the enclosing28E110 dirty
    //dispatch gate. Native adapter explicitly omits profiling-only gp-74A0 /
    //442F68 writes (and refuses the distinct bit10 sphere branch). It does
    //not claim those source globals are NULL/initialized or replay wrapper
    //effects. Finite current-transform projection admission is read-only.
    R publishDirectBox26F4B0NativeProfileAdapter(const SceneGraph49P& graph)noexcept {
        if(!constructed2403A0()||!rowsLoaded_||referenceReleased_||rootMode_!=RootMode::separate||
           warmChildlessRows_||!validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex)||
           !rowTable_||fields_.sourceRows1B8!=rowTable_.get()||model_.nodeCount1A0!=binding_.sourceRowCount||
           !model_.nodeCount1A0||model_.nodeCount1A0>maxModelBoundsNodes49O||
           at(graph,model_.sceneIndex).sceneFlags190&0x10u)return {S::phase,0x26f4c0u};
        OriginalStandardPlacementGlobals50LA::boundsNativeRouteUnknown();
        std::array<SceneNode49P,maxSceneNodes49P> admittedNodes{};
        for(std::uint32_t i=0;i<graph.count;++i)admittedNodes[i]=graph.nodes[i];
        SceneGraph49P admitted{admittedNodes.data(),graph.count};
        if(!refreshSceneWorld49P(admitted,model_.sceneIndex))return {S::graph,0x26f608u};
        std::array<ModelBoundsNode49O,maxModelBoundsNodes49O> rows{};
        ModelBoundsContext49O context{};const auto& root=at(admitted,model_.sceneIndex);
        context.rootWorld100=root.world100;context.rootSceneFlags190=root.sceneFlags190;
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
            auto& node=at(admitted,row->sceneIndex);
            if(expected==none||node.parent180!=expected)return {S::hierarchy,0x28daf0u};
            rows[i].local=node.localC0;rows[i].parent=parent;rows[i].sceneFlags190=node.sceneFlags190;
            for(unsigned k=0;k<3;++k){rows[i].center140.v[k]=row->bounds140[k];
                rows[i].halfExtent150.v[k]=row->bounds150[k];}
            //28DAF0's flag04-false return has no scene-cache writes.
            if((node.sceneFlags190&4u)&&!refreshSceneWorld49P(admitted,row->sceneIndex))
                return {S::graph,0x28db9cu};
        }
        ModelBoundsWorkspace49O workspace{};ModelBoundsResult49O output{};
        if(!computeModelBounds49O(rows.data(),model_.nodeCount1A0,context,workspace,output))
            return {S::graph,0x28daf0u};
        //The reused numerical adapter regenerates row worlds. Prove those
        //are the SAME actual lazy-cache results, not permission to replace a
        //different independently-ready world100 with a new transform.
        for(std::uint32_t i=0;i<model_.nodeCount1A0;++i)if(rows[i].sceneFlags190&4u){
            if(std::memcmp(workspace.world[i].v,at(admitted,sourceRowObject(i)->sceneIndex).world100.v,
                sizeof workspace.world[i].v))return {S::graph,0x28daf0u};
        }
        //Only now admit the represented direct source call on actual storage.
        //Its profiler adapter has no gameplay callback. Root refresh precedes
        //the real source1B8 traversal; eligible rows execute their point and
        //three axis refreshes in original order. No28E110 dirty skip exists.
        if(!refreshSceneWorld49P(graph,model_.sceneIndex))return {S::graph,0x26f608u};
        for(std::uint32_t i=0;i<model_.nodeCount1A0;++i)if(rows[i].sceneFlags190&4u){
            for(unsigned getter=0;getter<4;++getter)
                if(!refreshSceneWorld49P(graph,sourceRowObject(i)->sceneIndex))return {S::graph,0x28db9cu};
        }
        std::memcpy(model_.bounds160.data(),output.center160,sizeof output.center160); //26F780
        std::memcpy(model_.bounds170.data(),output.halfExtent170,sizeof output.halfExtent170); //26F7E8
        model_.radius198=output.radius198; //26F828 delay store, even forzero.
        if(output.radius198>0.f)at(graph,model_.sceneIndex).sceneFlags190=
            static_cast<std::uint8_t>((at(graph,model_.sceneIndex).sceneFlags190&~0x40u)|0x40u); //26F840
        return {S::complete,0x26f854u};
    }
private:
    OriginalRacerModelSceneBinding50LA binding_{};
    const SceneNode49P* nativeModelNodes_=nullptr;
    std::uint32_t nativeModelNodeCount_=0;
    O model_;
    OriginalRacerModelFields50LA fields_;
    std::array<I,rr::assets::maxModelNodes49O> indices_{};
    std::array<std::int32_t,rr::assets::maxModelNodes49O> sourceParents_{};
    // Separate mode owns rows0..N-1; direct mode leaves rows_[0] empty.
    std::array<std::unique_ptr<O>,rr::assets::maxModelNodes49O> rows_;
    std::unique_ptr<O*[]> rowTable_;
    RootMode rootMode_=RootMode::separate;
    bool directRootPrepared_=false,directRowsStarted_=false,warmChildlessRows_=false,warmHierarchyRows_=false;
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
        return joinDomain(outside,list.data(),list.size(),joined,sibling,secondSibling,thirdSibling,fourthSibling);
    }
    // Caller-sized rows (a racer wreck chain sizes them from its real count).
    // Every write below is within `capacity`, checked first.
    bool joinDomain(const OriginalWreckComponentDomain50LA& outside,
        C** listRows,std::size_t capacity,
        OriginalWreckComponentDomain50LA& joined,
        const OriginalRacerModelSceneOwner50LA* sibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* secondSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* thirdSibling=nullptr,
        const OriginalRacerModelSceneOwner50LA* fourthSibling=nullptr)const noexcept {
        using namespace original_wreck_component_detail50LA;
        if(!listRows||!capacity)return false;
        struct Rows{C** p;std::size_t n;std::size_t size()const noexcept{return n;}
            C*& operator[](std::size_t i)const noexcept{return p[i];}C** data()const noexcept{return p;}} list{listRows,capacity};
        const std::array<const OriginalRacerModelSceneOwner50LA*,4> peers{{
            sibling,secondSibling,thirdSibling,fourthSibling}};
        std::size_t added=1u;bool ended=false;
        for(std::size_t i=0;i<peers.size();++i){
            if(!peers[i]){ended=true;continue;}
            if(ended)return false;
            for(std::size_t j=0;j<i;++j)if(peers[j]==peers[i])return false;
            ++added;
        }
        if(outside.sessionGeneration!=binding_.sessionGeneration||added>list.size()||outside.count>list.size()-added||
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

#include "original_racer_source_bounds50la.inl"

#include "original_racer_source_local_bounds50la.inl"

#include "original_racer_source_translation50la.inl"

#include "original_racer_source_orientation50la.inl"

#include "original_racer_source_observer_bounds50la.inl"
