#include "original_ui_main_entry50kg.h"
#include "ra_cold_aggregate50la.h"
#include <cstdint>
#include <cstring>
#include <limits>
#include <thread>

namespace {
using Block=rr::assets::ParameterBlock48F;
using Field=rr::assets::ParameterField48F;
const Field* uniqueField(const Block& block,const char* name,bool& unambiguous)noexcept {
    const Field* found=nullptr;
    for(const auto& field:block.fields)if(field.name==name){
        if(found){unambiguous=false;return nullptr;}found=&field;
    }
    return found;
}
const Block* uniqueBlock(const rr::assets::ParameterDocument48F& doc,const char* name)noexcept {
    const Block* found=nullptr;
    for(const auto& block:doc.blocks)if(block.name==name){if(found)return nullptr;found=&block;}
    return found;
}
// Narrow native read of the authenticated decimal spellings already accepted
// by this RA decoder. Missing158B30 keys store the ORIGINAL caller default;
// malformed/unsupported spellings reject rather than pretend retail atoi ran.
bool integer158B30(const Block& block,const char* name,std::int32_t fallback,std::int32_t& out)noexcept {
    bool unique=true;const auto* field=uniqueField(block,name,unique);if(!unique)return false;
    if(!field){out=fallback;return true;}
    const auto& text=field->value;if(text.empty())return false;
    std::size_t i=0;const bool negative=text[0]=='-';
    if(negative||text[0]=='+')++i;if(i==text.size())return false;
    std::uint64_t magnitude=0;
    const std::uint64_t limit=negative?2147483648ull:2147483647ull;
    for(;i<text.size();++i){const auto c=text[i];
        if(c<'0'||c>'9'||magnitude>(limit-unsigned(c-'0'))/10u)return false;
        magnitude=magnitude*10u+unsigned(c-'0');
    }
    out=negative?static_cast<std::int32_t>(-static_cast<std::int64_t>(magnitude)):
        static_cast<std::int32_t>(magnitude);return true;
}
}

namespace ra_pod_scene50LA {
bool Owner::nativeSceneRoles270900Current(const ra_cold_sources50LA::Owner& source,
    std::uint32_t& pc)const noexcept {
    pc=0x270a50;
    if(!worldApplication_||worldApplication_->nativeOwnerThread_!=std::this_thread::get_id()||
       !worldColdConstructed_||!worldApplication_->application10()||worldApplication_->attachmentLock423F58()||
       worldEntity_.reference00!=1u||worldEntity_.objectKey!=key(&worldEntity_)||
       !worldApplication_->entityAuthority_.canRetire(worldEntity_)||
       sceneApplication_!=worldApplication_||!sceneGeneration_||sceneGeneration_!=worldEntity_.sessionGeneration||
       !topologyConstructed_||topologyClosed_||!identitiesCurrent()||!canCloseTopology())return false;
    const auto* decoded=source.sourcesDecoded();
    if(!decoded||!runtimeQueries_||runtimeQueries_->terminal_||runtimeQueries_->closed_||
       !runtimeQueries_->worldOwnersRegistered_||!runtimeQueries_->storageConstructed_||
       !runtimeQueries_->candidates_||runtimeQueries_->candidates_->decodedSource()!=decoded||
       runtimeQueries_->freshLighting_!=lighting_||!runtimeQueries_->storage_.get())return false;
    const auto& storage=*runtimeQueries_->storage_.get();
    if(!storage.borrowedWorldLease||storage.zones.data()!=collisionRows_||storage.count.zones!=10u||
       storage.world.portalHeads!=decoded->liveWorld.portalHeads||
       storage.world.portalLinks!=decoded->liveWorld.portalLinks||
       storage.world.portals!=decoded->liveWorld.portals||storage.world.zoneCount!=10u||
       storage.world.portalCount!=10u||storage.world.portalLinkCount!=20u)return false;
    for(std::size_t z=0;z<10u;++z){const auto& visible=visibility4C0_[z];const auto& collision=collision4C4_[z];
        if(!visible.constructed||!visible.attached||!collision.constructed||!collision.attached||
           visible.ordinal48!=z||collision.ordinal48!=z||
           visible.collectionBacking!=&visibilityRows_[z]||collision.collectionBacking!=&collisionRows_[z])return false;
    }
    if(!lighting_->nativeSceneRoles270900Current(this,decoded->world,visibilityRows_,pc))return false;
    pc=0x270cdc;
    // The original3D4/3CC cold-prefix builders are UNCALLED on this live
    // native route. Their absence is not guest-null knowledge or success.
    // Adopt the actual runtime Storage below; never construct late duplicates.
    if(worldPhysics3D4_||worldCollisionObjects3CC_||worldCollisionResources_||
       !runtimeQueries_->queriesConstructed_||!runtimeQueries_->actual_||!runtimeQueries_->follow_||
       !runtimeQueries_->cameraBankCurrent())return false;
    //27B834 defaults StaticType to Standard. The source27C110 PhysicsObject
    //index covers ALL1261 authored models; it is not the178 COL registry.
    const auto& world=decoded->world;
    if(world.instances.size()!=1261u||world.zones.size()!=10u)return false;
    std::array<std::uint32_t,10> ordinals{};
    for(std::size_t z=0;z<10u;++z)
        if(world.zones[z].zone!=z+1u||!world.zones[z].models)return false;
    for(const auto& instance:world.instances){
        if(!instance.zone||instance.zone>10u||instance.parameter_block>=world.parameters.blocks.size()||
           instance.ordinal!=++ordinals[instance.zone-1u])return false;
        const auto& block=world.parameters.blocks[instance.parameter_block];bool unique=true;
        const auto* type=uniqueField(block,"statictype",unique);
        if(block.name!=instance.section||!unique||(type&&type->value!="Standard"))return false;
    }
    for(std::size_t z=0;z<10u;++z)if(ordinals[z]!=world.zones[z].models)return false;
    pc=0x270cf4;
    //Concrete native collision provider: SAME178 prepared source meshes,
    //same candidate addresses and their genuine4C4 static registrations.
    //Additive TOB triggers are kept distinct, never counted as PhysicsObjects.
    const auto* bindings=runtimeQueries_->candidates_->staticBindings();
    if(!bindings||decoded->collisions.size()!=178u||bindings->size()<178u||
       storage.count.terrain!=bindings->size()||storage.candidates.size()!=bindings->size()||
       storage.candidateZones.size()!=bindings->size()||storage.candidateMethods.size()!=bindings->size()||
       storage.registrations.size()<bindings->size()||storage.registries.size()<20u||
       storage.lifecycle.zoneCount!=10u||storage.lifecycle.objectCount<bindings->size()||
       storage.lifecycle.storage.registries!=storage.registries.data()||
       storage.lifecycle.storage.registrations!=storage.registrations.data())return false;
    for(std::size_t i=0;i<178u;++i){const auto& binding=(*bindings)[i];const auto& original=decoded->collisions[i];
        if(binding.animated||binding.trigger||binding.originalOwner!=i||!binding.candidate||
           storage.candidates[i]!=binding.candidate||storage.candidateZones[i]!=original.zone||
           binding.method!=g::ZoneMethod50CL::First||storage.candidateMethods[i]!=binding.method||
           !original.zone||original.zone>10u)return false;
        const auto& collision=binding.candidate->candidate.collision;
        if(collision.geometry.mesh!=&original.mesh||!binding.candidate->nodeRadii||
           binding.candidate->nodeRadiusCount!=original.mesh.nodes.size()||
           std::memcmp(collision.geometry.worldMatrix.v,original.instance.world.v,sizeof(original.instance.world.v)))return false;
        const auto& registration=storage.registrations[i];
        const auto* expected=&storage.registries[(original.zone-1u)*2u];
        if(registration.owner!=expected||!registration.slot||registration.slot>1024u||
           registration.encodedObject!=(i+1u)*4u||expected->blockCount!=1u||
           !expected->perBlock||expected->perBlock>1024u||registration.slot>expected->lastUsed||
           expected->words[registration.slot-1u]!=registration.encodedObject)return false;
    }
    pc=0x270d14;
    //Actual later1B4E20 has ALREADY overwritten the earlier default0. Never
    //replay270A1C over this live field or replace the populated environment.
    if(!particlePredecessorAttempted_||useEnvironment35C_!=1u||!environmentMethod_||
       !environment443108_.constructed||!environment443108_.attached||
       sceneApplication_->environment443108_!=&environment443108_.entity||
       sceneApplication_->visibility443A98_!=environmentMethod_.get()||
       !method34Retained_||!method38Retained_||!constructorReferenceReleased_)return false;
    pc=0x270fb0;return true;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueNativeSceneInfo270900(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x270900;
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    //After adoption, normal271C10 may withdraw the temporary parser34 role.
    //The real immutable source and live services remain owned by this World.
    if(nativeSceneInfo270900_&&nativeSceneInfo270900_->nativeRolesAccepted)
        return nativeSceneInfo270900Current(app,pc);
    if(!bindNativeTrackScb34(app,pc)||!nativeScb34_||!nativeScb34_->document)return false;
    const auto* decoded=source_->sourcesDecoded();const auto& doc=decoded->world.parameters;
    pc=0x2709a4;
    if(!nativeSceneInfo270900_){
        const auto* info=uniqueBlock(doc,"sceneinfo");
        if(!info||decoded->world.scene_info_block>=doc.blocks.size()||
           info!=&doc.blocks[decoded->world.scene_info_block])return false;
        nativeSceneInfo270900_.emplace();
        nativeSceneInfo270900_->document=&doc;nativeSceneInfo270900_->sceneInfo=info;
    }
    auto& read=*nativeSceneInfo270900_;using Read=NativeSceneInfo270900::Read;
    if(read.document!=&doc||read.sceneInfo!=uniqueBlock(doc,"sceneinfo"))return false;
    if(read.next==Read::sceneName){
        pc=0x2709bc;bool unique=true;const auto* name=uniqueField(*read.sceneInfo,"scenename",unique);
        //158240(-1) copies actual source text. Our immutable alias admits the
        //real bounded RA spelling only, never inventing a missing name/default.
        if(!unique||!name||name->value.empty()||name->value.size()>=260u||
           name->value.find('\0')!=std::string::npos)return false;
        read.name244=name;read.next=Read::portals;
    }
    if(read.next==Read::portals){
        pc=0x2709d4;
        if(!integer158B30(*read.sceneInfo,"numportals",0,read.authoredPortals358))return false;
        read.next=Read::zones;
    }
    if(read.next==Read::zones){
        pc=0x270a00;
        if(!integer158B30(*read.sceneInfo,"numzones",1,read.authoredZones34C))return false;
        read.next=Read::environmentNode;
    }
    if(read.next==Read::environmentNode){
        pc=0x270a2c;
        if(!integer158B30(*read.sceneInfo,"useenvironmentnode",0,read.authoredUseEnvironment35C))return false;
        read.next=Read::roles;
    }
    pc=0x270a50;
    if(read.authoredPortals358!=10||read.authoredZones34C!=10||read.authoredUseEnvironment35C!=0||
       decoded->world.zones.size()!=10u||decoded->world.portals.size()!=10u||
       !nativeSceneRoles270900Current(*source_,pc))return false;
    pc=0x270e9c;
    const auto* environment=uniqueBlock(doc,"environment");bool unique=true;
    if(!environment||uniqueField(*environment,"ambientcolorflashminidletime",unique)||!unique)return false;
    //Actual absent158B80 key writes default0 then270EA8 takes its normal
    //epilogue. No flash state is reset here: the existing native lighting
    //owner already embodies this authenticated absent-key branch.
    read.physicsCollisionBackend=runtimeQueries_->storage_.get();
    read.environmentVisibilityBackend=lighting_.get();
    read.nativeRolesAccepted=true;
    pc=0x1aa08c;return true; //EXPLICIT native post-slot80, NOT source270900.
}
bool Owner::nativeSceneInfo270900Current(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)const noexcept {
    pc=0x270900;
    if(nativeScbThread_!=std::this_thread::get_id()||!nativeTrackArchiveCurrent(app)||
       !nativeSceneInfo270900_||!nativeSceneInfo270900_->nativeRolesAccepted)return false;
    const auto* decoded=source_->sourcesDecoded();const auto& doc=decoded->world.parameters;
    const auto& read=*nativeSceneInfo270900_;
    if(read.document!=&doc||read.sceneInfo!=uniqueBlock(doc,"sceneinfo")||!read.sceneInfo||
       read.next!=NativeSceneInfo270900::Read::roles||!runtimeQueries_||
       read.physicsCollisionBackend!=runtimeQueries_->storage_.get()||
       read.environmentVisibilityBackend!=lighting_.get())return false;
    bool unique=true;
    if(read.name244!=uniqueField(*read.sceneInfo,"scenename",unique)||!unique||!read.name244)return false;
    std::int32_t portals=0,zones=1,environmentNode=0;
    if(!integer158B30(*read.sceneInfo,"numportals",0,portals)||portals!=read.authoredPortals358||
       !integer158B30(*read.sceneInfo,"numzones",1,zones)||zones!=read.authoredZones34C||
       !integer158B30(*read.sceneInfo,"useenvironmentnode",0,environmentNode)||
       environmentNode!=read.authoredUseEnvironment35C||portals!=10||zones!=10||environmentNode!=0)return false;
    const auto* environment=uniqueBlock(doc,"environment");unique=true;
    if(!environment||uniqueField(*environment,"ambientcolorflashminidletime",unique)||!unique||
       !nativeSceneRoles270900Current(*source_,pc))return false;
    pc=0x1aa08c;return true;
}
}
