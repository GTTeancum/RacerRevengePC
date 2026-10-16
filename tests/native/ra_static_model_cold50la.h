#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/original_racer_model_packets50la.h"
#include "../../src/game/original_ui_user_rotate50kg.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "../../src/game/model_bounds49o.h"
#include "../../src/assets/original_standard_model_resource50la.h"
#include "../../src/assets/original_standard_material_table50la.h"
#include <limits>
#include <vector>

namespace ra_static_world50LA {class Owner;}
namespace ra_static_model_cold50LA {
namespace g=rr::game;namespace a=rr::assets;
// This owns real cold construction prefixes. The source calls28E8A0 before
//27AE38 appends the model Entity to StaticModels. The explicit observer stage
// below performs these in source order against the SAME visibility lifecycle.
// No selected191, traversal or renderer capability is issued.
// Explicit continuation owns genuine CPU packets and authored placement.
class Owner final {
    friend class ra_static_world50LA::Owner;
    using C=g::OriginalWreckComponent50LA;
    using K=g::OriginalWreckComponentKind50LA;
    using I=g::SceneNodeIndex49P;
public:
    struct Diagnostic {
        const char* failure=nullptr;
        std::size_t placement=0,parents=0,models=0,unresolvedSlots=0;
        g::OriginalRacerModelSceneResult50LA scene{};
        a::GeometryDiagnostic49O hierarchy{};
        a::OriginalStandardResourceDiagnostic50LA resource{};
        a::OriginalStandardTextureDiagnostic50LA cache{};
        g::OriginalRacerModelPacketResult50LA packets{};
        g::LiveZoneResult50EN visibility{};
    };
    struct Counts {std::size_t parents=0,models=0,unresolvedSlots=0;};
    // Supported cold operations, not per-instance successful-stage receipts.
    static constexpr bool observerRegistrationAdmitted=true;
    static constexpr bool modelEntityAppendAdmitted=true;
    static constexpr bool selectedQueuesAdmitted=false;
private:
    struct Input {
        std::size_t worldInstance=0;
        std::shared_ptr<const a::OriginalStandardModelResource50LA> payload;
        a::OriginalStandardMaterialReferences50LA* materials=nullptr;
    };
    struct Descriptor {
        const a::WorldInstance48F* source=nullptr;
        std::array<float,3> position{},look{},up{};
        bool useDynamicLighting144=false;
        std::string modelName180;
    };
    struct StandardWrapper {
        enum class Dispatch { unbound,standard3D26F0 } dispatch=Dispatch::unbound;
        std::uint32_t type00=0;
        g::OriginalRacerModelSceneOwner50LA* model8=nullptr; // Alias, not a second owner.
        std::unique_ptr<Descriptor> descriptor10;
    };
    struct Observer {
        g::CollectionRef50BX object=0;
        std::vector<g::CollectionZoneLink50BX> links;
        std::vector<g::LiveZoneEntry50EN> entries;
        g::LiveZoneMembership50EN membership;
    };
    struct Model {
        Input source;
        // One canonical graph per independent source model, never a copied
        // raster matrix or an alias of another placement sharing this PSG.
        g::SceneNode49P node{};
        g::OriginalRacerModelSceneOwner50LA scene;
        StandardWrapper wrapper;
        std::shared_ptr<a::OriginalStandardModelGeometry50LA> geometry;
        g::OriginalRacerModelPackets50LA packets;
        bool resourceRetained=false,referenceReleased=false,rowTableFreed=false,placed=false;
        std::size_t parent=0,slot=0;
        bool rows=false;
        std::unique_ptr<Observer> observer;
        bool parentAttached=false;
        g::SceneGraph49P graph()noexcept{return {&node,1};}
    };
    struct Slot {
        std::size_t worldInstance=0;
        Model* model=nullptr;
        // Null represents an explicitly unconstructed geometry-less prefix,
        // not a successful source loader or a skipped visibility registration.
    };
    struct Parent {
        C entity;
        std::uint32_t zone=0,count30=0;
        std::vector<Slot> entries34;
        bool constructed=false,released=false;
    };
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::shared_ptr<g::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<a::OriginalStandardMaterialTable50LA> materials_;
    std::vector<std::unique_ptr<Parent>> parents_;
    std::vector<std::unique_ptr<Model>> models_;
    std::uint64_t generation_=0;
    std::size_t unresolved_=0;
    bool attempted_=false,constructed_=false,closed_=false;
    bool resourcesAttempted_=false,resourcesComplete_=false;
    std::shared_ptr<a::OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> visibility_;
    bool observersAttempted_=false,observersComplete_=false;
    Owner()=default;
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    static bool reject(Diagnostic& d,const char* why)noexcept{d.failure=why;return false;}
    bool prepare(std::shared_ptr<const ra_cold_sources50LA::Owner> source,
        std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::shared_ptr<a::OriginalStandardMaterialTable50LA> materials,
        std::uint64_t generation,const std::vector<Input>& inputs,Diagnostic& d)noexcept {
        d={};if(attempted_||closed_)return reject(d,"static model prefix already attempted");
        attempted_=true;
        if(!source||!source->sourcesDecoded()||!application||!application->application10()||
           !materials||!generation||application->attachmentLock423F58())
            return reject(d,"same live source/Application/material owners required");
        source_=std::move(source);application_=std::move(application);materials_=std::move(materials);
        generation_=generation;
        try {
            const auto& world=source_->sourcesDecoded()->world;
            // Native backing admission/allocation precedes counted constructors.
            parents_.reserve(world.zones.size());models_.reserve(inputs.size());
            std::size_t wi=0;
            for(std::size_t z=0;z<world.zones.size();++z){const auto& authored=world.zones[z];
                if(authored.zone!=z+1)return reject(d,"source zone order differs");
                auto p=std::make_unique<Parent>();p->zone=authored.zone;
                p->entries34.resize(authored.models);
                for(std::size_t j=0;j<p->entries34.size();++j,++wi){
                    if(wi>=world.instances.size()||world.instances[wi].zone!=authored.zone||
                       world.instances[wi].ordinal!=j+1)return reject(d,"source StaticModels entry order differs");
                    p->entries34[j].worldInstance=wi;
                }
                parents_.push_back(std::move(p));
            }
            if(wi!=world.instances.size()||inputs.size()>wi)return reject(d,"source StaticModels count differs");
            for(std::size_t i=0;i<inputs.size();++i){const auto& input=inputs[i];d.placement=i;
                if(input.worldInstance>=world.instances.size()||(i&&input.worldInstance<=inputs[i-1].worldInstance)||
                   !input.payload||!input.payload->prepared()||input.payload->nodeCount()!=1||
                   !input.payload->originalBytes()||!input.payload->member()||!input.payload->scene()||
                   !input.materials||!input.materials->complete()||
                   input.materials->table382B38()!=materials_.get())return reject(d,"actual placement resource/material prefix missing");
                const auto& instance=world.instances[input.worldInstance];
                if(instance.parameter_block>=world.parameters.blocks.size())return reject(d,"source model descriptor absent");
                bool typeSeen=false;
                for(const auto& f:world.parameters.blocks[instance.parameter_block].fields)if(f.name=="statictype"){
                    //27B834 StaticType, default Standard. Never reinterpret
                    // PhysicsObject or the source's unknown-type fallback.
                    if(typeSeen||f.value!="Standard")return reject(d,"unadmitted actual static wrapper type");
                    typeSeen=true;
                }
                if(!instance.zone||instance.zone>parents_.size()||!instance.ordinal||
                   instance.ordinal>parents_[instance.zone-1]->entries34.size())return reject(d,"source slot outside parent");
                auto m=std::make_unique<Model>();m->source=input;m->parent=instance.zone-1;m->slot=instance.ordinal-1;
                //27BA04..27BAC4 Standard wrapper construction, followed by
                // retained native descriptor authoring (27AB90). No model alias
                // is published until its genuine2403A0 constructor succeeds.
                m->wrapper.dispatch=StandardWrapper::Dispatch::standard3D26F0;m->wrapper.type00=1;
                m->wrapper.descriptor10=std::make_unique<Descriptor>();
                auto& descriptor=*m->wrapper.descriptor10;descriptor.source=&instance;
                for(unsigned axis=0;axis<3;++axis){
                    const double values[]={instance.position[axis],instance.look[axis],instance.up[axis]};
                    float* destinations[]={&descriptor.position[axis],&descriptor.look[axis],&descriptor.up[axis]};
                    for(unsigned v=0;v<3;++v){const float f=static_cast<float>(values[v]);
                        if(!std::isfinite(f)||double(f)!=values[v])return reject(d,"original descriptor scalar is not binary32");
                        *destinations[v]=f;
                    }
                }
                bool dynamicSeen=false;
                for(const auto& f:world.parameters.blocks[instance.parameter_block].fields)if(f.name=="usedynamiclighting"){
                    if(dynamicSeen||(f.value!="T"&&f.value!="F"))return reject(d,"unsupported original UseDynamicLighting");
                    descriptor.useDynamicLighting144=f.value=="T";dynamicSeen=true;
                }
                bool sltSeen=false;
                for(const auto& f:world.parameters.blocks[instance.parameter_block].fields)if(f.name=="slt"){
                    if(sltSeen)return reject(d,"duplicate source SLT descriptor");sltSeen=true;
                    const auto dot=f.value.find('.');
                    //27A524 strcspn("."), cap50; this admitted corpus must not
                    // silently resolve a different file through truncation.
                    if(dot==std::string::npos||!dot||dot>50)return reject(d,"unsupported original SLT stem");
                    descriptor.modelName180=f.value.substr(0,dot)+".slt"; //27AC14 sprintf.
                }
                if(!sltSeen)return reject(d,"actual SLT descriptor missing");
                auto& slot=parents_[m->parent]->entries34[m->slot];
                if(slot.model||slot.worldInstance!=input.worldInstance)return reject(d,"duplicate actual parent slot");
                slot.model=m.get();models_.push_back(std::move(m));
            }
            unresolved_=world.instances.size()-models_.size();
            const auto additional=parents_.size()+models_.size();
            if(additional>std::numeric_limits<std::uint32_t>::max()-application_->objectCount423F60())
                return reject(d,"Application object count cannot admit this cold prefix");
            //271150 passes a1=1 to27B610. It constructs a genuine Entity,
            // installs StaticModels dispatch, and clears count30/entries34.
            // The ordered native array above is backing, not source publication.
            for(auto& p:parents_){
                const auto r=g::initializeOriginalWreckComponent50LA(p->entity,K::staticModels27B610,
                    generation_,key(&p->entity),true,application_->objectCount423F60());
                if(r.status!=g::OriginalWreckComponentStatus50LA::complete)return reject(d,"StaticModels Entity construction failed");
                p->constructed=true;++d.parents;
                //27B780 supplies this authored count. Native slots preserve
                //27B7A4's ordering; they do not impersonate constructed
                //StandardStaticModel/PhysicsObjectStaticModel wrapper tables.
                p->count30=static_cast<std::uint32_t>(p->entries34.size());
            }
            for(std::size_t i=0;i<models_.size();++i){auto& m=*models_[i];d.placement=i;
                const I root=static_cast<I>(0);
                g::OriginalRacerModelSceneBinding50LA binding;
                binding.sessionGeneration=generation_;
                binding.actualObjectCount423F60=&application_->objectCount423F60();
                binding.actualAttachmentLock423F58=&application_->attachmentLock423F58();
                binding.actualTypes383408=&application_->types383408();
                binding.modelIndex=root;binding.sourceRowIndices=&root;binding.sourceRowCount=1;
                //27AC30 a1=1;27AC44 loader a2=0: the model IS PSG row0.
                d.scene=m.scene.constructDirectRoot2403A0(binding,m.graph(),true);
                if(!d.scene.okay())return reject(d,"actual direct-root2403A0 prefix failed");
                m.wrapper.model8=&m.scene; //27AC38, not ownership transfer.
                d.scene=m.scene.prepareDirectRoot26E934(m.graph(),*m.source.payload->member());
                if(!d.scene.okay())return reject(d,"actual model name prefix failed");
                const auto& bytes=*m.source.payload->originalBytes();a::PsgHierarchyView49O hierarchy{};
                if(!a::readPsgHierarchy49O(bytes.data(),bytes.size(),hierarchy,d.hierarchy)||hierarchy.nodeCount!=1)
                    return reject(d,"retained original PSG hierarchy rejected");
                // Cache lookup in the resource continuation chooses fresh28CAC0
                // or the actual warm childless26EAC4 branch. Never pre-load
                // fresh rows before that choice (nodeCount1A0 differs).
                ++d.models;
                // Resource/placement continuation is a separate explicit stage.
            }
            d.unresolvedSlots=unresolved_;constructed_=true;return true;
        }catch(...){return reject(d,"cold static allocation failed; counted prefixes retained for close");}
    }
#include "ra_static_model_resources50la.inl"
#include "ra_static_model_observers50la.inl"
    bool canClose()const noexcept {
        if(closed_)return true;
        if(!application_)return parents_.empty()&&models_.empty();
        if(application_->attachmentLock423F58())return false;
        if(visibility_&&!visibility_->raStaticOwnerIdle(this))return false;
        std::size_t counted=0;
        for(const auto& p:parents_)if(p&&p->constructed&&!p->released){const auto& e=p->entity;
            if(e.reference00!=1||e.kind!=K::staticModels27B610||e.sessionGeneration!=generation_||
               e.parent1C||e.previous10||e.next14||e.mask20||e.descendants24||!parentTopology(*p))return false;
            ++counted;
        }
        for(const auto& m:models_)if(m&&m->scene.model()&&!m->scene.released()){
            const auto* object=m->scene.model();const auto& e=object->entity();
            const auto& fields=m->scene.fields();
            if((!m->parentAttached&&(e.parent1C||e.previous10||e.next14))||e.firstChild18||
               e.reference00!=(m->referenceReleased?0u:1u)||
               fields.field1C8||
               object->observer19C!=m->observer.get()||object->configuration1A4||
               !m->scene.preflightRetainedScene2896D0({const_cast<g::SceneNode49P*>(&m->node),1},m->observer.get()).okay())return false;
            if(m->observer){const auto& o=*m->observer;
                if(!visibility_||!o.object||o.membership.storage.links!=o.links.data()||
                   o.membership.storage.entries!=o.entries.data()||o.membership.storage.capacity!=o.links.size()||
                   o.links.size()!=o.entries.size()||
                   (o.membership.attached&&(o.membership.object!=o.object||
                    o.membership.method!=g::ZoneMethod50CL::First)))return false;
            }
            ++counted;
        }
        if(!resourcesCanClose())return false;
        return counted<=application_->objectCount423F60();
    }
    bool close()noexcept {
        if(closed_)return true;if(!canClose())return false;
        if(visibility_&&!visibility_->beginRaStaticAccess(this,true))return false;
        const auto failClose=[&](){if(visibility_)visibility_->endRaStaticAccess(this);return false;};
        // Private incomplete-prefix recovery, not the attached27B650 path.
        // Constructed packets do execute their actual240450 retirement leaf.
        //27AAD0 destroys wrapper metadata, not the model8 alias. This owner
        // retires real linked/unlinked model prefixes before releasing backing.
        for(auto it=models_.rbegin();it!=models_.rend();++it)if(*it){auto& m=**it;
            if(resourcesAttempted_){if(!closeResources(m))return failClose();}
            else if(!m.scene.closeFreshUnlinked(m.graph()).okay())return failClose();
        }
        if(visibility_&&!visibility_->releaseRaStaticOwner(this))return failClose();
        visibility_.reset();
        models_.clear();
        for(auto& p:parents_)if(p&&p->constructed&&!p->released){
            for(auto& slot:p->entries34)slot.model=nullptr;
            p->entries34.clear();
            --p->entity.reference00; // Private root's actual104970 reference.
            --application_->objectCount423F60(); // Empty base1413E0.
            p->released=true;
        }
        parents_.clear();resolver_.reset();materials_.reset();source_.reset();application_.reset();closed_=true;return true;
    }
public:
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    Counts counts()const noexcept {
        return constructed_&&!closed_?Counts{parents_.size(),models_.size(),unresolved_}:Counts{};
    }
    const g::OriginalRacerModelSceneOwner50LA* model(std::size_t placement)const noexcept {
        return constructed_&&!closed_&&placement<models_.size()?&models_[placement]->scene:nullptr;
    }
    const g::SceneNode49P* sourceRoot(std::size_t placement)const noexcept {
        return model(placement)?&models_[placement]->node:nullptr;
    }
    bool resourcesAndPlacementConstructed()const noexcept{return resourcesComplete_&&!closed_;}
    const g::OriginalRacerModelPackets50LA* coldPackets(std::size_t placement)const noexcept {
        return resourcesAndPlacementConstructed()&&placement<models_.size()?&models_[placement]->packets:nullptr;
    }
    const C* parent(std::size_t zoneIndex)const noexcept {
        return constructed_&&!closed_&&zoneIndex<parents_.size()?&parents_[zoneIndex]->entity:nullptr;
    }
    // Exact authored slot order, including unresolved slots; no selected list.
    bool parentSlot(std::size_t zoneIndex,std::size_t ordinalIndex,std::size_t& worldInstance,
        const g::OriginalRacerModelSceneOwner50LA*& modelOut)const noexcept {
        if(!constructed_||closed_||zoneIndex>=parents_.size()||ordinalIndex>=parents_[zoneIndex]->entries34.size())return false;
        const auto& slot=parents_[zoneIndex]->entries34[ordinalIndex];worldInstance=slot.worldInstance;
        modelOut=slot.model?&slot.model->scene:nullptr;return true;
    }
};
}
