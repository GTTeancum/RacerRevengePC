#include "original_ui_main_entry50kg.h"
#include "ra_cold_aggregate50la.h"
#include "original_world_cold_prefix50la.h"
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
bool Owner::constructLivePhysicsIndex270CDC(std::shared_ptr<const ra_cold_sources50LA::Owner> source,
    std::uint32_t& pc)noexcept {
    pc=livePhysicsIndexPC_;
    if(livePhysicsIndexAttempted_)return livePhysicsIndexComplete_;
    if(!worldApplication_||worldApplication_->nativeOwnerThread_!=std::this_thread::get_id()||
       !worldColdConstructed_||worldApplication_->attachmentLock423F58()||worldPhysics3D4_||
       !source||!g::OriginalWorldPhysicsIndex50LA::standardOnlySource(*source)||
       !worldZones34C_||*worldZones34C_!=source->sourcesDecoded()->world.zones.size())return false;
    livePhysicsIndexAttempted_=true;
    //270CC4 allocator ->27C010 constructor ->270CDC publication, BEFORE
    //27C110. This is the LOAD producer, not a late allocation at Restart.
    try{worldPhysics3D4_.reset(new g::OriginalWorldPhysicsIndex50LA);}catch(...){worldPhysics3D4_.reset();}
    pc=livePhysicsIndexPC_=0x270cdc;
    pc=livePhysicsIndexPC_=0x270ce8;
    if(!worldPhysics3D4_)return false; //Published null, cannot invoke27C110.
    if(!worldPhysics3D4_->prepare27C110(std::move(source))){
        pc=livePhysicsIndexPC_=worldPhysics3D4_->preparePC_;return false;
    }
    livePhysicsIndexComplete_=true;pc=livePhysicsIndexPC_=0x270cec;return true;
}
bool Owner::resetLiveWorldPhysics2702E0(
    const std::shared_ptr<g::OriginalApplicationEntities50LA>& expected,
    std::uint64_t generation,std::uint32_t& pc)noexcept {
    pc=0x27030c;
    if(!expected||expected->nativeOwnerThread_!=std::this_thread::get_id())return false;
    if(restartWorldPhysicsAttempted_){pc=restartWorldPhysicsPC_;return false;}
    restartWorldPhysicsAttempted_=true;
    if(worldApplication_!=expected||!worldColdConstructed_||topologyClosed_||
       worldEntity_.sessionGeneration!=generation||!generation||worldEntity_.reference00!=1||
       expected->attachmentLock423F58()||!expected->entityAuthority_.canRetire(worldEntity_)||
       !livePhysicsIndexComplete_||!worldPhysics3D4_)return false;
    if(!worldPhysics3D4_->reset27C330(pc)){restartWorldPhysicsPC_=pc;return false;}
    pc=restartWorldPhysicsPC_=0x270320;
    //Only actual fresh2705E0 establishes null. The admitted empty-Toy path
    //returns274308 before27432C, so it does not overwrite this same lane.
    if(!worldReset270550Complete_||!worldToyManager49C_)return false;
    if(*worldToyManager49C_){pc=restartWorldPhysicsPC_=0x27032c;return false;}
    pc=restartWorldPhysicsPC_=0x270334;
    if(!nativeTriggerList3D0_)return false; //Loaded owner required, no guessed null.
    if(!resetNativeTriggerList2DCC70(expected,generation,pc)){
        restartWorldPhysicsPC_=pc;return false;
    }
    auto& list=*nativeTriggerList3D0_;
    restartTriggerIndexS3_=0;restartTriggerOffsetS1_=0; //270350/358.
    for(;;){
        pc=restartWorldPhysicsPC_=0x270508;restartTriggerCount30_=list.count30;
        restartActionIndexS4_=0; //270518 branch delay, including final exit.
        if(static_cast<std::int32_t>(restartTriggerIndexS3_)>=static_cast<std::int32_t>(*restartTriggerCount30_))break;
        restartActionOffsetS0_=0; //270364 delay slot.
        for(;;){
            pc=restartWorldPhysicsPC_=0x2704ec;
            if(!nativeTriggerDescriptorCurrent(restartTriggerIndexS3_))return false;
            restartTriggerDescriptor_=list.descriptors34[restartTriggerIndexS3_];
            pc=restartWorldPhysicsPC_=0x2704f0;
            restartActionCount04_=restartTriggerDescriptor_->actionCount04;
            if(static_cast<std::int32_t>(restartActionIndexS4_)>=*restartActionCount04_)break;
            pc=restartWorldPhysicsPC_=0x270368;
            if(restartActionIndexS4_>=restartTriggerDescriptor_->actionSlotsPublished||
               !restartTriggerDescriptor_->actions68)return false;
            const auto* action=restartTriggerDescriptor_->actions68[restartActionIndexS4_];
            if(!action)return false;
            pc=restartWorldPhysicsPC_=0x270378;restartActionType00_=action->type00;
            if(*restartActionType00_==1){pc=restartWorldPhysicsPC_=0x270384;return false;}
            //Actual branch reloads same descriptor68/type00 before comparing10.
            pc=restartWorldPhysicsPC_=0x270438;
            action=list.descriptors34[restartTriggerIndexS3_]->actions68[restartActionIndexS4_];
            if(!action)return false;restartActionType00_=action->type00;
            if(*restartActionType00_==10){pc=restartWorldPhysicsPC_=0x270444;return false;}
            //BNE likely27043C takes22 skip; delay incrementsS0 and target4DC
            //incrementsS4. No LostAI invocation, C80 write, callback/contact reset.
            restartActionOffsetS0_+=4u;restartActionIndexS4_+=1u;
        }
        restartTriggerOffsetS1_+=4u;restartTriggerIndexS3_+=1u; //270500/504.
    }
    pc=restartWorldPhysicsPC_=0x270548;restartWorldPhysicsReturned_=true;return true;
}

bool Owner::modelParents3C4Current(const ra_static_model_cold50LA::Owner* expected,bool complete)const noexcept {
    if(expected&&expected->sourceMode_){
        //Exclusive actual2710F0 partial route. Never completed legacy metadata.
        if(complete||sourceModelOwner3C4_!=expected||expected->sourceWorld3C4Borrower_!=this||
           !sourceModels3C4_||*sourceModels3C4_!=sourceModelParents3C4_.get()||
           expected->application_!=worldApplication_||expected->generation_!=worldEntity_.sessionGeneration||
           expected->closed_||!expected->canCloseSourceNative()||sourceModelParentsPublished3C4_>10||
           modelParents3C4Complete_)return false;
        for(std::size_t z=0;z<sourceModelParentsPublished3C4_;++z){
            if(!sourceModelParents3C4_||!expected->sourceGroupOuterReturned_[z])return false;
            const auto* parent=z<expected->parents_.size()?expected->parents_[z].get():nullptr;
            if(sourceModelParents3C4_[z]!=(parent?&parent->entity:nullptr))return false;
        }
        return true;
    }
    if(!expected||sourceModelOwner3C4_!=expected||expected->sourceWorld3C4Borrower_!=this||
       !sourceModels3C4_||*sourceModels3C4_!=sourceModelParents3C4_.get()||
       expected->closed_||!expected->constructed_||expected->application_!=worldApplication_||
       expected->generation_!=worldEntity_.sessionGeneration||expected->parents_.size()!=10u||
       sourceModelParentsPublished3C4_>10u)return false;
    for(std::size_t z=0;z<sourceModelParentsPublished3C4_;++z){
        const auto& parent=expected->parents_[z];
        if(!sourceModelParents3C4_||!parent||sourceModelParents3C4_[z]!=&parent->entity||
           !parent->constructed||parent->released||parent->zone!=z+1u||
           parent->entity.reference00!=1u||parent->entity.sessionGeneration!=worldEntity_.sessionGeneration||
           parent->count30!=parent->entries34.size())return false;
    }
    return !complete||(modelParents3C4Complete_&&sourceModelParentsPublished3C4_==10u);
}
bool Owner::withdrawModelParents3C4()noexcept {
    if(sourceModelOwner3C4_){
        if(!modelParents3C4Current(sourceModelOwner3C4_,false))return false;
        //Revoke source array BEFORE any Parent payload/reference retirement.
        if(sourceModelOwner3C4_->sourceMode_)sourceModels3C4_.reset(); //Native source-mode knowledge withdrawal.
        else sourceModels3C4_=nullptr; //Preserve existing native projection policy.
        sourceModelOwner3C4_->sourceWorld3C4Borrower_=nullptr;sourceModelOwner3C4_=nullptr;
    }else if(sourceModels3C4_&&*sourceModels3C4_)return false;
    sourceModelParents3C4_.reset();sourceModelParentsPublished3C4_=0;
    modelParents3C4Complete_=false;return true;
}

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
    //3D4 now has its own actual270CDC/27C110 LOAD owner. The separate3CC
    //collision backend remains the existing native Storage, never duplicated.
    //Before its producer, null3D4 is unproduced, not source-null knowledge.
    if((worldPhysics3D4_&&(!livePhysicsIndexComplete_||
          !worldPhysics3D4_->currentForSource(source)))||
       worldCollisionObjects3CC_||worldCollisionResources_||
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
bool Owner::publishNativeModelParents2710F0(std::uint32_t& pc)noexcept {
    pc=modelParents3C4PC_;
    if(modelParents3C4Attempted_)return modelParents3C4Complete_&&world_&&
        modelParents3C4Current(world_->modelPrefixes_.get());
    if(!worldColdConstructed_||!worldApplication_||!source_||!source_->sourcesDecoded()||
       !sourceModels3C4_||*sourceModels3C4_||sourceModelParents3C4_||sourceModelOwner3C4_||
       !world_||world_->closed_||world_->source_!=source_||!world_->modelPrefixes_)return false;
    auto& owner=*world_->modelPrefixes_;const auto& authored=source_->sourcesDecoded()->world;
    if(owner.sourceMode_||owner.closed_||!owner.constructed_||!owner.canClose()||owner.source_!=source_||
       owner.application_!=worldApplication_||owner.generation_!=worldEntity_.sessionGeneration||
       owner.sourceWorld3C4Borrower_||owner.parents_.size()!=10u||authored.zones.size()!=10u)return false;
    //Immutable same-provider custody, not27B610/27B780 replay over counted
    //parents. Each authoritative source-pointer publication follows below.
    for(std::size_t z=0;z<10u;++z){const auto& parent=owner.parents_[z];
        if(!parent||!parent->constructed||parent->released||parent->zone!=z+1u||
           parent->entity.reference00!=1u||parent->entity.sessionGeneration!=owner.generation_||
           parent->count30!=authored.zones[z].models||parent->entries34.size()!=parent->count30)return false;
    }
    modelParents3C4Attempted_=true;
    try{sourceModelParents3C4_.reset(new Entity*[10]);}catch(...){sourceModelParents3C4_.reset();}
    sourceModels3C4_=sourceModelParents3C4_.get(); //27112C, even null.
    sourceModelOwner3C4_=&owner;owner.sourceWorld3C4Borrower_=this;
    pc=modelParents3C4PC_=0x27116c;
    if(!sourceModelParents3C4_)return false; //No fabricated empty array.
    for(std::size_t z=0;z<10u;++z){
        sourceModelParents3C4_[z]=&owner.parents_[z]->entity; //27116C same actual parent.
        ++sourceModelParentsPublished3C4_;
    }
    modelParents3C4Complete_=true;pc=modelParents3C4PC_=0x270d0c;return true; //Native post-slot only, not27121C return.
}

bool Owner::continueNativeSceneInfo270900(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x270900;
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(livePhysicsIndexAttempted_&&!livePhysicsIndexComplete_){pc=livePhysicsIndexPC_;return false;}
    if(modelParents3C4Attempted_&&!modelParents3C4Complete_){pc=modelParents3C4PC_;return false;}
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
        for(std::size_t i=0;i<name->value.size();++i)worldText244_[i]=static_cast<std::uint8_t>(name->value[i]);
        worldText244_[name->value.size()]=0; //Bounded actual158240 write; tail bytes remain unknown.
        read.name244=name;read.next=Read::portals;
    }
    if(read.next==Read::portals){
        pc=0x2709d4;
        if(!integer158B30(*read.sceneInfo,"numportals",0,read.authoredPortals358))return false;
        worldPortals358_=static_cast<std::uint32_t>(read.authoredPortals358);
        read.next=Read::zones;
    }
    if(read.next==Read::zones){
        pc=0x270a00;
        if(!integer158B30(*read.sceneInfo,"numzones",1,read.authoredZones34C))return false;
        worldZones34C_=static_cast<std::uint32_t>(read.authoredZones34C);
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
    //After authentic source/native custody, issue the actual independent3D4
    //constructor/publication before publishing SceneInfo acceptance. Native
    //collision/visibility roles remain existing owners; no270550 replay.
    if(!constructLivePhysicsIndex270CDC(source_,pc))return false;
    //Source270D04 slot88=2710F0 supplies this role. Explicit native
    //adoption binds existing counted parents before downstream readers; it
    //does not certify prior270FD0 or full2710F0 callback execution.
    if(!publishNativeModelParents2710F0(pc))return false;
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
       !nativeSceneInfo270900_||!nativeSceneInfo270900_->nativeRolesAccepted||
       !livePhysicsIndexComplete_||!worldPhysics3D4_||!world_||
       !modelParents3C4Current(world_->modelPrefixes_.get()))return false;
    const auto* decoded=source_->sourcesDecoded();const auto& doc=decoded->world.parameters;
    const auto& read=*nativeSceneInfo270900_;
    if(read.document!=&doc||read.sceneInfo!=uniqueBlock(doc,"sceneinfo")||!read.sceneInfo||
       read.next!=NativeSceneInfo270900::Read::roles||!runtimeQueries_||
       read.physicsCollisionBackend!=runtimeQueries_->storage_.get()||
       read.environmentVisibilityBackend!=lighting_.get())return false;
    if(!worldZones34C_||*worldZones34C_!=static_cast<std::uint32_t>(read.authoredZones34C)||
       !worldPortals358_||*worldPortals358_!=static_cast<std::uint32_t>(read.authoredPortals358))return false;
    if(!read.name244||read.name244->value.size()>=worldText244_.size())return false;
    for(std::size_t i=0;i<read.name244->value.size();++i)
        if(!worldText244_[i]||*worldText244_[i]!=static_cast<std::uint8_t>(read.name244->value[i]))return false;
    if(!worldText244_[read.name244->value.size()]||*worldText244_[read.name244->value.size()])return false;
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
