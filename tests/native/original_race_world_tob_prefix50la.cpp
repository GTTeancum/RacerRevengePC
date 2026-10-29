#include "../../src/game/original_world_light_assertion273abc.h"
#include "../../src/game/original_selected_array_guards27ed40.h"
#include "../../src/game/original_sunflare_saturation_history50la.h"
#include "../../src/game/original_parameter_packet_history50la.h"
#include "original_camera_session50kx.h"
#include "original_race_display_lifetime50la.h"
#include "original_race_world_join50la.h"
#include "ra_cold_aggregate50la.h"
#include "../../src/assets/track_start50ad.h"
#include "../../platform/linux/native/material_texture_gl50eb.h"
#include "../../platform/linux/native/original_ui_garage_screen_text50kg.h"
#include "../../src/game/original_smoke_numeric50rc.h"
#include "../../platform/linux/native/original_ui_light_manager_owner50kg.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"
#include "../../platform/linux/native/original_ui_garage_scene_setup50kg.h"
#include "original_source_world_cscamera_history50la.h"
#include <cstring>
#include <thread>

namespace {
using Document=rr::assets::ParameterDocument48F;
using Block=rr::assets::ParameterBlock48F;
using Field=rr::assets::ParameterField48F;
bool findUniqueBlock(const Document& doc,const char* name,const Block*& out)noexcept {
    out=nullptr;
    for(const auto& block:doc.blocks)if(block.name==name){if(out)return false;out=&block;}
    return true; //True/null is genuine absence in this whole authenticated document.
}
bool findUniqueField(const Block& block,const char* name,const Field*& out)noexcept {
    out=nullptr;
    for(const auto& field:block.fields)if(field.name==name){if(out)return false;out=&field;}
    return true;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::withdrawNativeScb34_271C10(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa098;
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(!nativeSceneInfo270900Current(app,pc)||nativeTob34_)return false;
    pc=0x271c20;
    if(nativeScb34_){
        const auto& reader=*nativeScb34_;
        if(reader.source!=source_||reader.document!=nativeSceneInfo270900_->document||
           reader.provider!=rosterMountDomain_->archiveProvider3E1A70())return false;
        //Native parser-role destruction/withdrawal corresponding to1573B0
        //then World34 clear at271C34. NOT destruction of the shared decoded
        //World, geometry or SceneInfo roles, and NOT whole-owner close.
        nativeScb34_.reset();worldParser34_=nullptr; //271C34, same publication.
    }
    pc=0x1aa0a0;return true;
}

bool Owner::continueNativeTob1B4330(const original_main_entry50KG::Application& app,
    const rr::platform::OriginalUiRiderProfileOwner50KG& profile,std::uint32_t& pc)noexcept {
    using Next=NativeTobReader34::Next;
    pc=0x1aa0ac;
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    try {
    if(nativeScb34_||!nativeSceneInfo270900Current(app,pc)||!particles_||
       !particles_->decoded_||particles_->closed_||particles_->source_!=source_)return false;
    const auto provider=rosterMountDomain_->archiveProvider3E1A70();
    pc=0x1b43c4;
    if(!nativeTob34_){
        nativeTob34_.emplace(); //Publish the replacement World34 parser role BEFORE load.
        worldParser34_=&*nativeTob34_;
        nativeTob34_->source=source_;nativeTob34_->provider=provider;
        nativeTob34_->authoring=particles_.get();
    }
    auto& reader=*nativeTob34_;
    if(!worldParser34_||*worldParser34_!=&reader||reader.source!=source_||reader.provider!=provider||reader.authoring!=particles_.get())return false;
    pc=0x1b43d0;
    a::OriginalStandardArchiveProvider50LA::MemberView member;
    a::OriginalStandardArchiveProvider50LA::MountView mount;
    if(!provider||provider->lookup132820("tracks\\ra.tob",member)!=
           a::OriginalStandardArchiveProvider50LA::Lookup::present||
       !provider->mount(member.mount,mount)||!mount.archiveName||*mount.archiveName!="TRACKS/RA.RES"||
       !mount.archiveSha256||*mount.archiveSha256!=ra_cold_sources50LA::archiveSha||
       mount.archiveBytes!=4604561u||mount.memberCount!=1010u||!member.data||member.size!=95558u||
       member.revision!=world_->providerRevision_)return false;
    //Reuse the existing actual particle-authoring owner's WHOLE authenticated
    //TOB document. No new decode, selected-row table or geometry/actor copy.
    const auto& bytes=particles_->bytes_[0];const auto& document=particles_->tob_;
    if(bytes.size()!=member.size||document.original_bytes!=bytes||
       std::memcmp(member.data,bytes.data(),bytes.size()))return false;
    if(reader.document){
        if(reader.document!=&document||reader.member.data!=member.data||reader.member.member!=member.member||
           reader.member.size!=member.size||reader.member.mount!=member.mount||reader.member.ordinal!=member.ordinal||
           reader.member.revision!=member.revision||reader.member.bankGeneration!=member.bankGeneration)return false;
    }else{
        if(reader.next!=Next::load)return false;
        reader.member=member;reader.document=&document;reader.next=Next::podDust;
    }
    if(reader.next==Next::complete){
        pc=0x1aa0c0;
        return worldTrackEmitsDust51C_&&*worldTrackEmitsDust51C_==1u&&worldMode524_==2u;
    }
    const Block* block=nullptr;const Field* field=nullptr;
    if(reader.next==Next::podDust){
        pc=0x1b4490;
        if(!findUniqueBlock(document,"podracer",block)||!block||!worldTrackEmitsDust51C_)return false;
        pc=0x1b44b0;
        //Exact supported RA spelling at89989. This is not a general158B30
        //numeric-parser claim or a supplied dust/event flag. Missing/general
        //TOB paths remain unsupported; the genuine RA field is present.
        if(!findUniqueField(*block,"trackemitsdust",field)||!field||field->value!="1"||
           field->value_offset!=89989u||field->value_bytes!=2u)return false;
        worldTrackEmitsDust51C_=1u; //Actual51C BEFORE the reached524 read/store.
        reader.next=Next::podCache;
    }
    if(reader.next==Next::podCache){
        pc=0x1b44c8;
        if(!findUniqueBlock(document,"podracer",block)||!block||
           !findUniqueField(*block,"ipucachesize",field)||!field||field->value!="2"||
           field->value_offset!=89993u||field->value_bytes!=2u)return false;
        worldMode524_=2u; //Same constructor-owned524; not a new native GPU-cache allocation.
        reader.next=Next::rain;
    }
    if(reader.next==Next::rain){
        pc=0x1b44e8;
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection actual;
        if(!profile.courseRaceSnapshot(actual))return false;
        //The actual current Profile byte operand, NOT Session/human count or
        //a remembered source1B0 value. Other modes do not read the Rain block.
        if(((actual.packed1125C>>5u)&3u)==1u){
            pc=0x1b4504;
            if(!findUniqueBlock(document,"rain",block)||block)return false;
            //This exact RA TOB has no Rain section; no World534/Rain object,
            //2D0290 constructor or2D05E0 setup is fabricated.
        }
        reader.next=Next::animations;
    }
    if(reader.next==Next::animations){
        pc=0x1b45a0;
        if(!findUniqueBlock(document,"321_anims",block)||block)return false;
        //RA's whole713-section TOB proves absence. Original1B45A8 therefore
        //makes no528/52C/530 writes/allocations; no invented array/count owner.
        reader.next=Next::complete;
    }
    pc=0x1aa0c0;return true; //Before slotA4/271CF0, NOT full1A9EE0 completion.
    }catch(...){return false;} //Published view and reached scalar stores remain owned.
}
}

namespace rr::game {
bool OriginalRaceConstructor50LA::continueNativeTrackAfterScb(
    const original_main_entry50KG::Application& app,std::uint32_t& pc)noexcept {
    using Step=NativeTrackLoadStep;
    pc=0x1aa078;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!nativeIntroWorldEntity()||
       !world74_||!profile_)return false;
    if(nativeTrackLoadStep_==Step::nativeAdaptedA4||nativeTrackLoadStep_==Step::tobRetired||
       nativeTrackLoadStep_==Step::aub||nativeTrackLoadStep_==Step::aubRetired||
       nativeTrackLoadStep_==Step::nativeAdaptedReturn)return continueNativeTrackAfterTob(app,pc);
    if(nativeTrackLoadStep_==Step::scb){
        if(!world74_->continueNativeSceneInfo270900(app,pc))return false;
        nativeTrackLoadStep_=Step::sceneInfo;
    }
    if(nativeTrackLoadStep_==Step::sceneInfo){
        if(!world74_->withdrawNativeScb34_271C10(app,pc))return false;
        nativeTrackLoadStep_=Step::scbRetired;
    }
    if(nativeTrackLoadStep_!=Step::scbRetired&&nativeTrackLoadStep_!=Step::tob)return false;
    if(!world74_->continueNativeTob1B4330(app,*profile_,pc))return false;
    nativeTrackLoadStep_=Step::tob;
    if(!world74_->continueNativeTerrain271370(app,pc))return false;
    if(!world74_->nativeEmptyAnimations2713E0Complete_){
        if(!world74_->loadNativeEmptyAnimations2713E0(*world74_->nativeTob34_->document,pc))return false;
        world74_->nativeEmptyAnimations2713E0Complete_=true;
    }
    if(!world74_->continueNativeTriggers1B5000(app,pc))return false;
    if(!world74_->continueNativePhysicsObjects2718C0(app,pc))return false;
    pc=0x271d64;
    if(!world74_->runtimeQueries_||!world74_->runtimeQueries_->camera_||
       world74_->runtimeQueries_->camera_->session.get()!=cameraNativeAdapter34_||
       !cameraNativeAdapter34_||!ownsNativeCameraAdapter34(*cameraNativeAdapter34_))return false;
    if(!world74_->continueNativeCsCameras272380(app,*cameraNativeAdapter34_,pc))return false;
    const bool completed=world74_->continueNativeSunFlareRead1B46E0(app,*profile_,pc);
    if(completed)return true;
    if(pc!=0x2c5c34||!loadDisplay_)return false;
    const auto camera=world74_->borrowNativeLoadCamera(world74_,*cameraNativeAdapter34_);
    if(!camera||camera.get()!=cameraNativeAdapter34_||!ownsNativeCameraAdapter34(*camera))return false;
    if(world74_->buildNativeSunFlarePacket2E6060(app,*loadDisplay_,*camera,pc))return true;
    if(pc!=0x2c5c44)return false;
    if(world74_->loadNativeSunFlareResources2C5C44(app,pc))return true;
    if(pc!=0x2c5cb4)return false;
    if(world74_->loadNativeSunFlareDepth2C5CF0(app,*loadDisplay_,*camera,pc))return true;
    if(pc!=0x2720d0)return false;
    if(world74_->loadNativeSunFlareSaturation2720D0(app,pc))return true;
    if(pc!=0x271d88)return false;
    if(world74_->loadNativeParticleDescriptors2722F0(app,pc))return true;
    if(pc!=0x271d98)return false;
    if(world74_->loadNativeEmptyBuoys1B5090(app,pc))return true;
    if(pc!=0x271da8)return false;
    if(world74_->loadNativeStaticModelParams271A00(app,pc))return true;
    if(pc!=0x271db0||!world74_->nativeStaticModelParamEffectsComplete_||
       world74_->nativeStaticModelParamsNext_!=673)return false;
    //271DB0 li v0,1;271DBC returns this represented native-adapted A4 route.
    //No claim that earlier270900/fullWorld source services all ran.
    nativeTrackLoadStep_=Step::nativeAdaptedA4;
    return continueNativeTrackAfterTob(app,pc);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueNativeTerrain271370(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x271cf0;
    if(sourceTerrainPhase_!=SourceTerrainPhase::unreached||
       (nativeTerrain3B0_&&nativeTerrain3B0_->sourceMode))return false;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||nativeScb34_||
       !nativeTob34_||nativeTob34_->next!=NativeTobReader34::Next::complete||
       !nativeTob34_->document||!nativeSceneInfo270900Current(app,pc))return false;
    try {
    pc=0x271d24; //A4's FIRST call: slot90 ->271370. Not the second slot94.
    if(!candidates_||!candidates_->prepared_||candidates_->source_!=source_)return false;
    const auto* decoded=source_->sourcesDecoded();
    if(!decoded||decoded->collisions.size()!=178u||candidates_->candidates_.size()<178u)return false;
    const auto& document=*nativeTob34_->document;
    pc=0x2713a0;
    if(!nativeTerrain3B0_)nativeTerrain3B0_.emplace(); //276830 defaults; no Entity count.
    auto& terrain=*nativeTerrain3B0_;
    if(!terrain.count00){
        for(std::size_t i=0;i<8u;++i){
            pc=0x2768c0;
            const Block* block=nullptr;
            const std::string section="terrainzone"+std::to_string(i+1u);
            if(!findUniqueBlock(document,section.c_str(),block)||!block)return false;
            auto& row=terrain.rows04[i];const Field* field=nullptr;
            //Exact authenticated RA numeric spelling only. Genuine absence
            //writes158B80's1.f default, as in TerrainZone1. Not a general parser.
            const char* floats[]={"surfacefriction","surfacedrag","surfacetraction"};
            float* values[]={&row.friction00,&row.drag04,&row.traction08};
            const std::uint32_t floatPC[]={0x276764,0x27678c,0x2767b4};
            for(unsigned j=0;j<3u;++j){
                pc=floatPC[j];
                if(!findUniqueField(*block,floats[j],field)||(field&&field->value!="1.0"))return false;
                *values[j]=1.f;
            }
            const char* flags[]={"generatedust","generatedirt"};
            std::uint32_t* outputs[]={&row.dust0C,&row.dirt10};
            for(unsigned j=0;j<2u;++j){
                pc=j?0x2767fc:0x2767d8;
                if(!findUniqueField(*block,flags[j],field)||
                   (field&&field->value!="T"&&field->value!="F"))return false;
                *outputs[j]=!field||field->value=="T"?1u:0u;
            }
        }
        pc=0x276924;terrain.count00=8; //Reader counts present sections, not successful scalar reads.
    }
    if(terrain.count00!=8u)return false;
    if(terrain.mappingsComplete){pc=0x271d34;return true;}
    //The native SceneInfo route intentionally has no cold3CC/3D4. Attach
    //only descriptor-name/index metadata to its EXISTING178 COL candidates.
    //Other authored models and triggers remain unknown, not default terrain.
    for(std::size_t ci=0;ci<178u;++ci){
        const auto& collision=decoded->collisions[ci];
        auto& candidate=candidates_->candidates_[ci];
        if(candidate.candidate.collision.geometry.mesh!=&collision.mesh)return false;
        const rr::assets::WorldInstance48F* instance=nullptr;
        for(const auto& in:decoded->world.instances)
            if(in.zone==collision.zone&&in.ordinal==collision.ordinal){if(instance)return false;instance=&in;}
        if(!instance||instance->parameter_block>=decoded->world.parameters.blocks.size()||
           instance->collision_member!=collision.member)return false;
        auto& descriptor=candidates_->terrainDescriptors_[ci];
        if(!descriptor.source){
            const auto& block=decoded->world.parameters.blocks[instance->parameter_block];
            const Field* field=nullptr;
            if(!findUniqueField(block,"slt",field)||!field)return false;
            //27A8C4->309348 is FIRST-dot prefix, capped at50 by27A8CC.
            const auto dot=field->value.find('.');
            const auto end=dot==std::string::npos?field->value.size():dot;
            descriptor.name00=field->value.substr(0,end>50u?50u:end);
            if(descriptor.name00.empty())return false;
            descriptor.source=instance; //27B004 index default is already0.
        }else if(descriptor.source!=instance)return false;
        if(candidate.terrainDescriptor180&&candidate.terrainDescriptor180!=&descriptor.field180)return false;
        candidate.terrainDescriptor180=&descriptor.field180;
    }
    pc=0x2713c8; //slotBC ->272520, only after nonzero276880 return.
    const Block* block=nullptr;const Field* field=nullptr;
    if(!findUniqueBlock(document,"terrainzonecollision",block)||!block||
       !findUniqueField(*block,"numcollisionobjects",field)||!field||field->value!="9")return false;
    for(unsigned i=1;i<=9u;++i){
        pc=0x272590;
        const std::string section="zonecollisionobject"+std::to_string(i);
        if(!findUniqueBlock(document,section.c_str(),block)||!block||
           !findUniqueField(*block,"obj",field)||!field)return false;
        std::string name=field->value;
        //2725BC calls1169D0: LAST dot in OBJ. Distinct from descriptor
        //27A8C4->309348 FIRST-dot prefix, retained on the same candidate owner.
        const auto dot=name.rfind('.');if(dot!=std::string::npos)name.resize(dot);
        pc=0x2725f8;
        if(!findUniqueField(*block,"terrainzone",field)||!field||field->value!="2")return false;
        const std::uint32_t index180=2u-1u; //27260C; no clamping/remapping.
        std::size_t matches=0;
        for(auto& descriptor:candidates_->terrainDescriptors_){
            const auto& descriptorName=descriptor.name00;
            bool equal=name.size()==descriptorName.size();
            //308F00 uses the source ASCII case table (not locale-sensitive).
            for(std::size_t n=0;equal&&n<name.size();++n){
                unsigned char a=static_cast<unsigned char>(name[n]);
                unsigned char b=static_cast<unsigned char>(descriptorName[n]);
                if(a>=128u||b>=128u)return false; //unsupported non-ASCII
                if(a>='A'&&a<='Z')a=static_cast<unsigned char>(a+32u);
                if(b>='A'&&b<='Z')b=static_cast<unsigned char>(b+32u);
                equal=a==b;
            }
            if(equal){pc=0x27264c;descriptor.field180=index180;++matches;}
        }
        //Exact RA provenance supports one match per row. Preserve reached
        //writes if a future unsupported document fails this bounded admission.
        if(matches!=1u)return false;
    }
    terrain.mappingsComplete=true;
    pc=0x271d34;return true; //STOP BEFORE slot94/2713E0. NOT full A4 return.
    }catch(...){return false;}
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueNativeTriggers1B5000(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x271d44;
    if(sourceTriggerAttempted_||nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeEmptyAnimations2713E0Complete_||sourceTerrainPhase_!=SourceTerrainPhase::unreached||
       !nativeTerrain3B0_||nativeTerrain3B0_->sourceMode||!nativeTerrain3B0_->mappingsComplete||
       !nativeTob34_||!nativeTob34_->document||!nativeSceneInfo270900Current(app,pc))return false;
    try {
    if(!triggers_||!triggers_->decoded_||triggers_->source_!=source_||!candidates_||
       candidates_->triggerSource_!=triggers_||!candidates_->triggerActions_||
       candidates_->candidates_.size()!=180u||!runtimeQueries_||runtimeQueries_->candidates_!=candidates_||
       !runtimeQueries_->storage_.get()||runtimeQueries_->triggerQueryActive_)return false;
    auto& actions=*candidates_->triggerActions_;auto& group=actions.group_;
    if(!actions.ready_||actions.source_!=triggers_.get()||actions.lease_||actions.actionFailed_||
       actions.terminal_!=ra_trigger_actions50LA::Status::Complete||!group.ready||group.actionsFailed||
       group.triggers.size()!=2u||group.actions.context!=&actions||!group.actions.preflight||!group.actions.apply)return false;
    const auto hooks=actions.hooks();
    if(group.actions.preflight!=hooks.preflight||group.actions.apply!=hooks.apply||
       actions.racerCount_!=runtimeQueries_->inputs_.collisionRoster.size()||
       actions.playerPrefixCount34_!=runtimeQueries_->inputs_.humanCount)return false;
    for(std::size_t i=0;i<actions.racerCount_;++i)
        if(actions.racers_[i].main690!=runtimeQueries_->bodyIdentities_[i].main||
           actions.racers_[i].secondary!=runtimeQueries_->bodyIdentities_[i].secondary)return false;
    const auto& storage=*runtimeQueries_->storage_.get();
    if(storage.candidates.size()!=180u||storage.candidateZones.size()!=180u)return false;
    const auto& document=*nativeTob34_->document;
    //Existing trigger authoring was authenticated earlier. Reuse it, proving
    //its WHOLE bytes equal this actual temporary World34 document. No decode.
    if(document.original_bytes!=triggers_->tob_||
       triggers_->parameters_.original_bytes!=document.original_bytes)return false;
    const Block* block=nullptr;const Field* field=nullptr;
    //Native supported-data admission; source reader occurs after list publication.
    pc=0x2dc474;
    if(!findUniqueBlock(document,"triggers",block)||!block||
       !findUniqueField(*block,"numberoftriggers",field)||!field||field->value!="2")return false;
    if(!createNativeTriggerList2DC320(candidates_,triggers_,pc))return false;
    auto& list=*nativeTriggerList3D0_;
    if(list.readerEntered){pc=list.readerPC;return list.complete;}
    list.readerEntered=true;
    if(!actions.beginNativeDescriptorLoad())return false;
    struct ReaderStop {decltype(list)& owner;std::uint32_t& pc;
        ~ReaderStop(){owner.readerPC=pc;}} readerStop{list,pc};
    pc=0x2dc4f4;list.count30=2u; //Actual reader value from authenticated field above.
    try{list.descriptors34.reset(new NativeTriggerDescriptor*[list.count30]);}catch(...){}
    pc=0x2dc508; //Actual nullable allocation publication; unwritten rows stay unread.
    for(std::size_t i=0;i<2u;++i){
        pc=0x2dc53c;
        const std::string section="trigger"+std::to_string(i+1u);
        const Block* actionBlock=nullptr;
        if(!findUniqueBlock(document,section.c_str(),block)||!block||
           !findUniqueBlock(document,(section+"action1").c_str(),actionBlock)||!actionBlock)return false;
        const auto& authored=triggers_->sources_[i];auto& state=group.triggers[i];
        pc=0x2dc568;
        try{list.descriptorOwners[i]=std::make_unique<NativeTriggerDescriptor>();}catch(...){}
        //Inline descriptor ctor2DC580..598 establishes00/04/68 null/zero.
        //Geometry64 remains explicit existing native shape adoption, not a new shape.
        pc=0x2dc5b0;
        if(!list.descriptors34)return false; //Retain descriptor allocation before bad outer store.
        list.descriptors34[i]=list.descriptorOwners[i].get();
        ++list.descriptorSlotsPublished;
        if(!list.descriptors34[i])return false;
        auto& descriptor=*list.descriptors34[i];descriptor.authoring=&authored;
        auto& candidate=candidates_->candidates_[178u+i];const auto& collision=candidate.candidate.collision;
        if(!findUniqueField(*block,"name",field)||!field||field->value!=authored.name||
           !findUniqueField(*block,"numberofactions",field)||!field||field->value!="1")return false;
        //Actual2DBE08 reader destination, not a restart-time authored-count substitute.
        pc=0x2dbe08;descriptor.actionCount04=1;
        if(!findUniqueField(*block,"zone",field)||!field||field->value!="4"||authored.zone!=4u||
           !findUniqueField(*actionBlock,"actiontype",field)||!field||field->value!="LostAI"||
           !findUniqueField(*actionBlock,"targetnodeindex",field)||!field||
           field->value!=std::to_string(authored.action.targetNodeIndex)||authored.action.reset)return false;
        //Actual existing action/trigger backend, not a second sphere/action.
        //Source factory1B5E70 maps LostAI→22;1B5DB8 chooses1B5A60 reader.
        //Its Reset default0 and TargetNodeIndex29/104 are already owned here.
        if(storage.candidates[178u+i]!=&candidate||storage.candidateZones[178u+i]!=authored.zone||
           !authored.startEnabled||authored.oneTimeOnly||!authored.ignorePlayer||
           authored.randomTriggerPercent90!=100||actions.originalAiNodeCount_!=113u||
           (authored.action.targetNodeIndex!=(i?104u:29u))||
           candidate.nativeTriggers!=&group||candidate.nativeTriggerIndex!=i||candidate.shapeType!=5u||
           !candidate.raOriginalTriggerSegments||candidate.terrainDescriptor180||
           collision.filter.identity!=ra_trigger_actions50LA::firstTriggerIdentity+i||
           collision.geometry.id!=collision.filter.identity||state.collider.identity!=collision.filter.identity||
           collision.ownerIdentity9C!=0x35000000u+i||
           collision.shapeType!=5u||!collision.hasResponseCallbackE8||collision.filter.categoryA4!=8u||
           state.callbackCount98!=1u||state.randomTriggerPercent90!=100||(state.flags1B8&7u)!=3u||
           group.ignoreColliderCount[i]!=actions.playerPrefixCount34_)return false;
        for(unsigned a=0;a<3u;++a)
            if(collision.sphere.center.v[a]!=authored.sphere.center.v[a])return false;
        if(collision.sphere.radius!=authored.sphere.radius)return false;
        for(std::size_t n=0;n<group.ignoreColliderCount[i];++n)
            if(group.ignoreColliders[i][n]!=actions.racers_[n].main690)return false;
        const auto* membership=runtimeQueries_->triggerMembership_[i].get();
        if(!membership||!membership->state.attached||membership->state.object!=179u+i||
           membership->state.method!=g::ZoneMethod50CL::First||membership->state.lastShapeTag!=5u)return false;
        //Adopt immutable descriptor source and SAME native live state. Only
        //missing lifetime Entity identities are added; no flags/contact reset,
        //callback registration, candidate resize, geometry or zone reattachment.
        if(!countNativeTriggerPair2A0290(i,pc))return false;
        if(list.joined<i+1u)list.joined=i+1u;
        pc=0x2dc0a8;
        try{descriptor.actions68.reset(new NativeActionDescriptor*[descriptor.actionCount04]);}catch(...){}
        pc=0x2dc0b0; //Retain nullable publication; factory still runs before slot store.
        //Authenticated LostAI factory type22. Its owned payload is joined to
        //the SAME prewarmed callback backend, never a second action behavior.
        pc=0x1b5db8;
        try{descriptor.action=std::make_unique<NativeActionDescriptor>();}catch(...){}
        pc=0x1b5dec;if(!descriptor.actions68)return false;
        descriptor.actions68[0]=descriptor.action.get();
        descriptor.actionSlotsPublished=1;
        if(!actions.beginNativeActionDescriptor(i,descriptor.action.get(),pc)){
            if(!descriptor.action)pc=0x2dc1ec;return false;
        }
        //Same immutable TOB common reader: missing SpecificObject uses empty
        //158AD0 fallback; never infer this from the old semantic projection.
        const bool specificObjectAbsent=findUniqueField(*actionBlock,"specificobject",field)&&!field;
        if(!actions.completeNativeActionDescriptor(i,authored.action,pc,specificObjectAbsent))return false;
        descriptor.complete=true;
    }
    if(!canRetireNativeTriggerList3D0())return false; //Prove bounded prefix is ownable/retirable.
    if(!actions.completeNativeDescriptorLoad())return false;
    list.complete=true;
    pc=0x271d54;return true; //BEFORE fourth A4 dispatch/+AC, not full A4 return.
    }catch(...){return false;} //Retain list and every reached counted pair for exact native cleanup.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueNativePhysicsObjects2718C0(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x271d54; //Fourth A4 dispatch/+AC ->2718C0.
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeTriggerList3D0_||!nativeTriggerList3D0_->complete||
       !nativeTob34_||!nativeTob34_->document||!nativeSceneInfo270900Current(app,pc))return false;
    const auto& document=*nativeTob34_->document;
    if(document.blocks.size()!=713u)return false;
    pc=0x2718e0;
    //157D00's missing lookup returns before replacing its selected block.
    //Absence of PhysicsObjects ALONE would not justify the following default.
    //The WHOLE authenticated RA TOB also lacks NumberOfPhysicsObjects in EVERY
    //block, including any retained Trigger2Action1 selection. No second parse.
    for(const auto& block:document.blocks){
        if(block.name=="physicsobjects")return false; //Present path not implemented.
        for(const auto& field:block.fields)
            if(field.name=="numberofphysicsobjects")return false;
    }
    pc=0x2718f8;
    const std::int32_t localCount=0; //158B30(a2=0), field miss ->158B60 default.
    if(localCount>0)return false;
    //271904 ->2719E8 skips ALL4B0/4B4 stores and27A650 allocations.
    //Do not zero, allocate or replace any physics owner, nor infer preexisting
    //World4B0/4B4 values from this local default. This branch has no Entity work.
    //Native bounded no-owner admission must agree with authoritative actual
    //constructor lanes. These are custody checks, NOT added guest reads/stores.
    if(!worldReset270550Complete_||!worldPhysicsObjectCount4B0_||*worldPhysicsObjectCount4B0_||
       !worldPhysicsObjectPointers4B4_||*worldPhysicsObjectPointers4B4_)return false;
    nativePhysicsObjects2718C0Complete_=true;
    pc=0x271d64;return true; //BEFORE fifth A4 dispatch/+B8, NOT complete A4.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueNativeCsCameras272380(const original_main_entry50KG::Application& app,
    const original_camera_session50KX::Session& session,std::uint32_t& pc)noexcept {
    pc=0x271d64; //Fifth A4 call/+B8 ->272380: metadata, NOT camera construction.
    if(sourceCsCameraPhase_!=SourceCsCameraPhase::unreached||
       (nativeCsCameraTable3E4_&&nativeCsCameraTable3E4_->sourceMode)||nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativePhysicsObjects2718C0Complete_||!nativeTob34_||!nativeTob34_->document||
       !nativeSceneInfo270900Current(app,pc)||!runtimeQueries_||!runtimeQueries_->camera_||
       runtimeQueries_->camera_->session.get()!=&session||
       runtimeQueries_->camera_->generation!=worldEntity_.sessionGeneration||
       session.sessionGeneration()!=worldEntity_.sessionGeneration)return false;
    try {
    //This SAME native Session already owns authenticated RA cut authoring,
    //prepared by its existing factory BEFORE phase1. Do not invoke phase2
    //startActions/startPrelude, recreate custody, or allocate camera children.
    const auto* authoring=session.taCutAuthoring();
    if(!authoring||authoring->trackId()!="RA"||authoring->cutCount()!=7u)return false;
    const auto bytes=authoring->originalTobBytes();const auto& document=*nativeTob34_->document;
    if(!bytes.data||bytes.size!=document.original_bytes.size()||
       std::memcmp(bytes.data,document.original_bytes.data(),bytes.size))return false;
    pc=0x2723a0;
    const Block* block=nullptr;const Field* field=nullptr;
    if(!findUniqueBlock(document,"cscameras",block)||!block||
       !findUniqueField(*block,"numberofcameras",field)||!field||field->value!="7")return false;
    if(!nativeCsCameraTable3E4_){
        pc=0x2723cc;
        if(!nativeCsCameraZones3E8_)return false; //Unknown is not null.
        if(*nativeCsCameraZones3E8_){pc=0x2723d8;return false;} //Append/realloc path unjoined.
        pc=0x2723fc;nativeCsCameraCount3E4_=authoring->cutCount(); //BEFORE backing/publication.
        nativeCsCameraTable3E4_.emplace();
        auto& table=*nativeCsCameraTable3E4_;
        table.session=&session;table.authoring=authoring;
        //Existing fixed native storage policy replaces seven-word allocation,
        //not another camera owner. Unknown rows remain unknown on publication.
        pc=0x272414;nativeCsCameraZones3E8_=&table.zones;
    }
    auto& table=*nativeCsCameraTable3E4_;
    if(table.session!=&session||table.authoring!=authoring||
       !nativeCsCameraCount3E4_||*nativeCsCameraCount3E4_!=7u||
       !nativeCsCameraZones3E8_||*nativeCsCameraZones3E8_!=&table.zones||table.next>7u||
       !nativeSceneInfo270900_||nativeSceneInfo270900_->authoredZones34C!=10)return false;
    for(std::size_t i=0;i<7u;++i){
        pc=0x272438;
        const std::string section="cscamera"+std::to_string(i+1u);
        if(!findUniqueBlock(document,section.c_str(),block)||!block||
           !findUniqueField(*block,"initialzone",field)||!field)return false;
        std::uint32_t zone=0;
        if(!authoring->initialZone(static_cast<unsigned>(i+1u),zone)||!zone||
           field->value!=std::to_string(zone))return false;
        pc=0x272458;
        auto& reached=(**nativeCsCameraZones3E8_)[i];
        if(reached&&*reached!=zone)return false;
        reached=zone; //Preserve authored1-based ordinals; NEVER subtract1.
        //This native projection omits the genuine shared first-use interval.
        //Withdraw process history only AFTER the reached row output store.
        source_world_cscamera50LA::History::invalidateNative();
        pc=0x2724b4;
        if(zone>static_cast<std::uint32_t>(nativeSceneInfo270900_->authoredZones34C))return false;
        if(table.next<i+1u)table.next=i+1u;
    }
    //Retry validates the SAME table, never appends another seven cameras.
    //Existing source-authoring consumers stay intact; no frame/path selection,
    //Session,3E4 policy, controllerA8, zone-membership or renderer reset occurs.
    pc=0x271d78;return true; //BEFORE sixth A4 call/+C0, not full A4 return.
    }catch(...){return false;} //Reached row stores remain owned until close/retry.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueNativeSunFlareRead1B46E0(const original_main_entry50KG::Application& app,
    const rr::platform::OriginalUiRiderProfileOwner50KG& profile,std::uint32_t& pc)noexcept {
    pc=0x271d78; //+C0 is SunFlares; +C4, NOT +C0, owns ParticleEmitters.
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeCsCameraTable3E4_||nativeCsCameraTable3E4_->sourceMode||nativeCsCameraTable3E4_->next!=7u||
       !nativeCsCameraCount3E4_||*nativeCsCameraCount3E4_!=7u||
       !nativeCsCameraZones3E8_||*nativeCsCameraZones3E8_!=&nativeCsCameraTable3E4_->zones||
       !nativeTob34_||!nativeTob34_->document||!nativeSceneInfo270900Current(app,pc))return false;
    //The sole caller is the actual Race retained by Application. Its entry
    //guard proves Race.profile_ == Application.riderProfile (+2DC identity).
    //Read the current byte, not launch configuration or cached racer count.
    pc=0x1b46ec;
    const auto* current=profile.commonProfile();
    if(!current||!current->known)return false;
    pc=0x1b46f8;
    const bool mode=((current->playerBits1125C>>5u)&3u)!=1u;
    //1B46E8 initializes a1=1; only humans==1 changes it to0 at1B470C.
    //Both branches tail271DD0. This is NOT a skip or absence condition.
    pc=0x271e30;
    const Block* block=nullptr;const Field* field=nullptr;
    if(!findUniqueBlock(*nativeTob34_->document,"sunflares",block)||!block)return false;
    pc=0x271e48;
    if(!findUniqueField(*block,"numberofsunflares",field)||!field||field->value!="1"||
       field->value_offset!=91475u||field->value_bytes!=2u)return false;
    //Exact authenticated numeric read: local stack+4B4=1. Source has NOT
    //read World4A4 or stored World4A8 yet. Never substitute four fire emitters.
    nativeSunFlareRead271DD0_=NativeSunFlareRead271DD0{mode,1};
    if(!beginNativeSunFlareDomain271E5C(pc))return false;
    try {
    pc=0x271ecc;
    if(!findUniqueBlock(*nativeTob34_->document,"sunflare1",block)||!block)return false;
    NativeSunFlareInputs inputs;inputs.modeA1=mode;
    pc=0x271eec;
    if(!findUniqueField(*block,"texturemap",field)||!field||field->value!="sun_ra.tga")return false;
    inputs.texture=field->value;
    pc=0x271f0c;
    if(!findUniqueField(*block,"retrievematerial",field)||field)return false;
    inputs.retrieveMaterial="flarezretrieve"; //Genuine default literal3B9F78.
    pc=0x271f20;
    if(!findUniqueField(*block,"saturation",field)||!field||field->value!="t")return false;
    inputs.saturation=true;
    pc=0x271f34;
    if(!findUniqueField(*block,"minscale",field)||!field||
       !a::parseTrackNumber50AD(field->value.data(),field->value.size(),inputs.minScale))return false;
    pc=0x271f48;
    if(!findUniqueField(*block,"maxscale",field)||!field||
       !a::parseTrackNumber50AD(field->value.data(),field->value.size(),inputs.maxScale)||
       inputs.minScale!=.5f||inputs.maxScale!=2.f)return false;
    pc=0x271f60;
    if(!findUniqueField(*block,"width",field)||field)return false;
    inputs.width=128;
    pc=0x271f78;
    if(!findUniqueField(*block,"height",field)||field)return false;
    inputs.height=128;
    pc=0x271f98;
    if(!findUniqueField(*block,"vertexcolor",field)||field)return false;
    inputs.vertexColor={{128.f,128.f,128.f,255.f}}; //26FA60 default alpha for3 values.
    pc=0x271fe0;
    if(!findUniqueField(*block,"lightdirection",field)||field)return false;
    const auto triple=[&](const char* name,std::array<float,3>& out){
        const Field* value=nullptr;
        if(!findUniqueField(*block,name,value)||!value)return false;
        std::size_t at=0;
        for(unsigned i=0;i<3;++i){const auto comma=value->value.find(',',at);
            if((i<2)==(comma==std::string::npos))return false;
            const auto end=i<2?comma:value->value.size();
            if(!a::parseTrackNumber50AD(value->value.data()+at,end-at,out[i]))return false;
            at=end+1;
        }
        return true;
    };
    std::array<float,3> position{},target{};
    pc=0x272004;if(!triple("position",position))return false;
    pc=0x272020;if(!triple("targetposition",target))return false;
    //Source three scalar SUB.S, then w=1 at27205C. Normalization belongs
    //to2C5A80 and is deliberately not performed early or replaced by light2.
    for(unsigned i=0;i<3;++i){volatile float difference=target[i]-position[i];inputs.direction[i]=difference;}
    inputs.direction[3]=1.f;
    if(!constructNativeSunFlare2C5730(std::move(inputs),pc))return false;
    return loadNativeSunFlareMaterials2C5A10(app,pc); //Never skips the sprite packet service.
    }catch(...){return false;} //Count/table/object remain owned on reached failures.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::retainNativeSunFlareMaterial245DA0(NativeSunFlareMaterial& handle,const std::string& name,
    const std::shared_ptr<a::OriginalStandardMaterialTable50LA>& table,const std::string& directory,
    std::uint32_t& pc)noexcept {
    pc=0x245da0;
    if(!table||!table->prepared()||directory.size()+name.size()+1u>=256u||name.empty()||
       directory.find('\0')!=std::string::npos||name.find('\0')!=std::string::npos)return false;
    try {
    pc=0x245dd8;
    if(!handle.parameters08)handle.parameters08=std::make_unique<NativeSunFlareParameters>();
    pc=0x245dec;
    if(!handle.primitive04)handle.primitive04=std::make_unique<NativeSunFlarePrimitive>();
    auto& primitive=*handle.primitive04;
    std::string requested=directory;
    if(!requested.empty()&&requested.back()!='\\')requested+='\\';
    requested+=name;
    const auto dot=requested.find_last_of('.'); //299C74->1169D0 LAST dot, not first.
    if(dot!=std::string::npos)requested.resize(dot);
    if(primitive.stored){
        pc=0x299cf8;
        if(primitive.table!=table||primitive.requested!=requested||!primitive.retained)return false;
    }else{
        primitive.table=table;primitive.requested=std::move(requested);
        pc=0x299cac;
        a::OriginalStandardMaterialDiagnostic50LA diagnostic;
        std::int32_t row=-1;const bool found=table->find298C80(primitive.requested,row,diagnostic);
        primitive.material00=row;primitive.stored=true; //299CB4 before shared assertion/retain.
        auto& guard=rr::platform::OriginalUiGarageScreenText50KG::guard8CD8();
        auto& assertion=rr::platform::OriginalUiGarageScreenText50KG::assertion443640();
        if(!guard.load(std::memory_order_acquire)){
            assertion.store(1,std::memory_order_relaxed);guard.store(1,std::memory_order_release);
        }
        pc=0x299cf8;
        if(!found||(assertion.load(std::memory_order_relaxed)&&row==-1))return false;
        pc=0x299d38;
        //Actual retained originating3833F0 ScreenMT; no new template/corpus,
        //FINAL-policy bypass, populate/demand, or row captured from a map.
        if(!table->retainScreen14D590(row,diagnostic))return false;
        primitive.retained=true;
    }
    if(!primitive.configured){
        primitive.type0C=15; //245DFC supplies a2=15 to2465B0.
        primitive.flags0F|=1u; //245E20..2C precedes borrowed parameter publication.
        primitive.parameters10=handle.parameters08.get(); //245E34.
        primitive.flags0F&=static_cast<std::uint8_t>(~1u); //245E44: not owned by primitive.
        primitive.configured=true;
    }
    return canRetireNativeSunFlareMaterial(handle);
    }catch(...){return false;} //Every published allocation/reference remains owned.
}

bool Owner::loadNativeSunFlareMaterials2C5A10(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x2720c8;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!nativeSunFlares_||nativeSunFlares_->sourceMode||
       !nativeSunFlares_->owner||!canRetireNativeSunFlare()||!world_||
       world_->domain_!=rosterMountDomain_||!world_->context_||!world_->context_->live()||
       !rosterMountDomain_||app.standardMaterialDomain()!=rosterMountDomain_||
       !nativeSceneInfo270900_||nativeSceneInfo270900_->authoredZones34C!=10)return false;
    auto& object=*nativeSunFlares_->owner;
    using Load=NativeSunFlare::Load;
    const auto context=world_->context_;const auto table=rosterMountDomain_->materials382B38();
    if(!table||!context->materialApplication()||
       !table->screenApplicationMatches(*context->materialApplication())||
       !context->applicationAnimationClock().sameOwner(app.animationClock()))return false;
    try {
    const auto& directory=app.materialDirectory36B970();
    if(object.context&&object.context!=context)return false;
    if(object.directory36B970&&*object.directory36B970!=directory)return false;
    if(!object.context)object.context=context;
    if(!object.directory36B970)object.directory36B970=directory;
    if(object.nextLoad==Load::unreached){
        pc=0x2c5a6c;object.zones30=static_cast<std::uint32_t>(nativeSceneInfo270900_->authoredZones34C);
        pc=0x2c5a80;
        if(std::fegetround()!=FE_TONEAREST)return false;
        g::original_smoke_numeric_detail50RC::Math math;
        const std::array<float,3> input{{object.inputs.direction[0],object.inputs.direction[1],object.inputs.direction[2]}};
        const auto normal=g::original_smoke_numeric_detail50RC::normalize13E3F0(input,math);
        if(math.badPC){pc=math.badPC;return false;}
        pc=0x2c5a94;object.direction50={{normal[0],normal[1],normal[2],1.f}};
        object.minScale34=object.inputs.minScale;object.maxScale38=object.inputs.maxScale;
        object.flags98KnownValue=static_cast<std::uint8_t>((object.flags98KnownValue&~4u)|(object.inputs.modeA1?4u:0u));
        object.nextLoad=Load::fields;
    }
    if(object.nextLoad==Load::fields){
        pc=0x2c5ab8;
        auto sprite=std::make_unique<NativeSunFlareSprite>(); //2E5E60->245CF0, uncounted.
        object.sprite64=std::move(sprite); //2C5AD0, BEFORE material loader.
        object.nextLoad=Load::spritePublished;
    }
    if(!object.sprite64)return false;
    auto& sprite=*object.sprite64;
    if(object.nextLoad==Load::spritePublished){
        pc=0x2c5ae0; //Sprite slot+C is2E5EE0->245DA0, not texture fallback.
        if(!retainNativeSunFlareMaterial245DA0(sprite.material,object.inputs.texture,table,directory,pc))return false;
        object.nextLoad=Load::spriteLoaded;
    }
    if(object.nextLoad==Load::spriteLoaded){
        pc=0x2c5aec;sprite.scale1C=sprite.scale20=*object.maxScale38;
        pc=0x2c5af8;sprite.alpha48=0;
        for(auto& color:sprite.colors38)color&=0x00ffffffu; //2E5F60 all four corners.
        pc=0x2c5b04;
        const auto* video=context->videoIdentity();
        if(!video||!context->live()||video->packetZMaximum<10)return false;
        sprite.depth34=video->packetZMaximum-10; //Actual443A60+60, not a supplied constant.
        //Source float->integer->byte conversions; admitted VertexColor has
        //exact128 components, all four source calls use alpha255, independent
        //of the separate sprite.alpha48 zero written above.
        if(object.inputs.vertexColor!=std::array<float,4>{{128.f,128.f,128.f,255.f}})return false;
        for(auto& color:sprite.colors38)color=0xff808080u;
        pc=0x2c5c18;sprite.width14=object.inputs.width;sprite.height18=object.inputs.height;
        pc=0x2c5c28;sprite.x0C=0;sprite.y10=0;
        object.nextLoad=Load::spriteConfigured;
    }
    pc=0x2c5c34;
    return false; //Mandatory2E6060 packet service NEXT; no fake loader/+C0/A4 return.
    }catch(...){return false;}
}
}

namespace ra_cold_aggregate50LA {
std::shared_ptr<original_camera_session50KX::Session> Owner::borrowNativeLoadCamera(
    const std::shared_ptr<Owner>& self,const original_camera_session50KX::Session& expected)noexcept {
    //Dedicated private in-load lifetime pin; NEVER weakens ready()-requiring
    //public gameplay borrow. The caller/getter prove reached selected Camera.
    if(self.get()!=this||nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       terminal_||closed_||!runtimeQueries_||!runtimeQueries_->cameraBankCurrent()||
       !runtimeQueries_->camera_||runtimeQueries_->camera_->session.get()!=&expected||
       expected.sessionGeneration()!=worldEntity_.sessionGeneration||
       runtimeCameraBorrows_==std::numeric_limits<std::size_t>::max())return {};
    try {
        ++runtimeCameraBorrows_;
        return std::shared_ptr<original_camera_session50KX::Session>(
            runtimeQueries_->camera_->session.get(),[self](original_camera_session50KX::Session*)noexcept{
                --self->runtimeCameraBorrows_;
            });
    }catch(...){return {};} //shared_ptr invokes supplied deleter on allocation failure.
}
bool Owner::buildNativeSunFlarePacket2E6060(const original_main_entry50KG::Application& app,
    const original_main_entry50KG::RaceDisplayLifetime50LA& display,
    const original_camera_session50KX::Session& camera,std::uint32_t& pc)noexcept {
    pc=0x2c5c34;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeSunFlares_||nativeSunFlares_->sourceMode||!nativeSunFlares_->owner||!canRetireNativeSunFlare())return false;
    auto& object=*nativeSunFlares_->owner;
    using Load=NativeSunFlare::Load;
    if(object.nextLoad<Load::spriteConfigured)return false;
    if(!object.context||!object.context->live()||!object.sprite64)return false;
    auto& sprite=*object.sprite64;
    if(!sprite.material.parameters08||!sprite.material.primitive04||
       !sprite.material.primitive04->configured)return false;
    auto& parameter=*sprite.material.parameters08;
    if(object.nextLoad>=Load::spritePacket){pc=0x2c5c44;return false;} //Do not rebuild/restore old camera observation.
    pc=0x2e6060;
    if(std::fegetround()!=FE_TONEAREST)return false;
    pc=0x2e60f0;
    const auto* video=object.context->videoIdentity();std::uint64_t revision=0;
    std::array<std::int32_t,4> rectangle{};
    if(!video||display.application_!=&app||display.video_.get()!=video||
       !runtimeCameraBorrows_||!runtimeQueries_||!runtimeQueries_->camera_||
       runtimeQueries_->camera_->session.get()!=&camera||
       !display.readColdGameplayViewport115990(camera,rectangle,revision))return false;
    //The ACTUAL phase1 sequence already selected this nonnull Camera before
    //the World load.115990 borrows its existing1E0/1E4/1E8/1EC fields.
    //No phase2 assumption, default-null branch or Video-offset substitute.
    pc=0x2e6114;
    if(rectangle!=std::array<std::int32_t,4>{{0,0,640,448}})return false;
    //Exact2E611C..619C CVT.S.W, .5*extent,2048-half-origin,CVT.W.S,<<4.
    const auto x=(2048-(rectangle[2]-rectangle[0])/2-rectangle[0])*16;
    const auto y=(2048-(rectangle[3]-rectangle[1])/2-rectangle[1])*16;
    if(!sprite.x0C||!sprite.y10||*sprite.x0C||*sprite.y10||
       sprite.width14!=128u||sprite.height18!=128u||sprite.scale1C!=2.f||sprite.scale20!=2.f||
       sprite.u24!=0.f||sprite.u2C!=0.f||sprite.v28!=1.f||sprite.v30!=1.f||sprite.alpha48||
       sprite.depth34!=video->packetZMaximum-10||sprite.colors38!=
           std::array<std::uint32_t,4>{{0xff808080u,0xff808080u,0xff808080u,0xff808080u}}||
       video->width!=640||video->height!=448||x<0||y<0||x>65535-4096||y>65535-4096)return false;
    //Admitted exact scalar CVT.W.S(0+128*2+0.5)=256; source shifts16ths.
    const auto left=static_cast<std::uint32_t>(x),top=static_cast<std::uint32_t>(y);
    const auto right=left+4096u,bottom=top+4096u;
    pc=0x2e6218; //134000(a1=1,a2=128): SAME actual133F80 helper.
    rr::game::OriginalParameterPacketHistory50LA::invalidateNative134xxx();
    if(!parameter.packetStarted){
        for(auto& word:parameter.unknown00To1C)word=0;
        parameter.field2C=1;parameter.count24=128;parameter.packetStarted=true;
        parameter.packetKind=NativeSunFlareParameters::PacketKind::sprite;
    }
    try {
        if(!parameter.packet)parameter.packet=std::unique_ptr<NativeSunFlarePacket>(new NativeSunFlarePacket);
    }catch(...){return false;} //Retain pre-allocation reset/capacity stores.
    parameter.buffer20=parameter.packet->words.data();parameter.count28=0;
    auto& words=parameter.packet->words;
    const auto put=[&](std::uint32_t word){words[parameter.count28++]=word;};
    const auto quad=[&](std::uint32_t a,std::uint32_t b,std::uint32_t c,std::uint32_t d){put(a);put(b);put(c);put(d);};
    //Private CPU adaptation of serialized134990/1345D0/1344E0/134470/
    //134650 leaves. Temporary source writer scratch becomes bounded local
    //indices; no second renderer, global selection, DMA or submission occurs.
    put(0x11000000u);put(0x03000000u);put(0x02000000u);
    put(0x6c098320u); //mode1,UNPACK9QW,to0x320;134650 fills placeholder.
    quad(0x00008001u,0x80000000u,0xeeeeeeeeu,0x00eeeeeeu); //2E6258..63A4 GIFtag.
    quad(0,0,0x3f,0);quad(0x11e,0,0,0); //2E63B0..DC.
    quad(0x00100010u,0,3,0); //First UV: ((0*128)+1)<<4 in both lanes.
    quad(0x00808080u,0x3f800000u,1,0); //RGB, alpha48/4=0, Q=1.
    quad(left|(top<<16),static_cast<std::uint32_t>(sprite.depth34),5,0);
    quad(0x08000800u,0,3,0); //Second UV:1*128<<4.
    quad(0x00808080u,0x3f800000u,1,0);
    quad(right|(bottom<<16),static_cast<std::uint32_t>(sprite.depth34),5,0);
    put(0x14000000u); //2E660C after134650 ends UNPACK.
    if(parameter.count28!=41u)std::terminate();
    words[41]=words[42]=words[43]=0; //1342B0 padding does NOT change count28.
    parameter.unknown00To1C[0]=0x3000000bu; //44DWORD /4 =>11 QW.
    parameter.unknown00To1C[1].reset(); //Native pointer relocation, never guessed guest address.
    parameter.nativePayload04=parameter.buffer20;
    parameter.packetComplete=true;
    //Source2E661C sees genuine call a1=0 and skips2F5E50. No renderer queue
    //or texture demand is fabricated. Serial callback-free build preserved
    //the actual Display observation; keep exact check before publishing return.
    std::uint64_t after=0;std::array<std::int32_t,4> afterRectangle{};
    if(!display.readColdGameplayViewport115990(camera,afterRectangle,after)||
       after!=revision||afterRectangle!=rectangle)return false;
    object.nextLoad=Load::spritePacket;
    pc=0x2c5c44;return false; //BEFORE embedded flarezretrieve245DA0, not full loader/A4.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::loadNativeSunFlareResources2C5C44(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x2c5c44;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!nativeSunFlares_||nativeSunFlares_->sourceMode||
       !nativeSunFlares_->owner||!canRetireNativeSunFlare()||!world_||
       world_->domain_!=rosterMountDomain_||!world_->context_||!world_->context_->live()||
       !rosterMountDomain_||app.standardMaterialDomain()!=rosterMountDomain_)return false;
    auto& o=*nativeSunFlares_->owner;using Load=NativeSunFlare::Load;
    const auto context=world_->context_;const auto table=rosterMountDomain_->materials382B38();
    if(o.nextLoad<Load::spritePacket||o.context!=context||!o.directory36B970||
       *o.directory36B970!=app.materialDirectory36B970()||!table||!context->materialApplication()||
       !table->screenApplicationMatches(*context->materialApplication())||
       !context->applicationAnimationClock().sameOwner(app.animationClock())||o.zones30!=10)return false;
    try {
        if(o.nextLoad==Load::spritePacket){
            o.callback74=&o; //2C5C48 delay slot BEFORE245DA0; retained on failure.
            if(!retainNativeSunFlareMaterial245DA0(o.material68,o.inputs.retrieveMaterial,
                table,*o.directory36B970,pc))return false;
            o.nextLoad=Load::retrieveLoaded;
        }
        if(o.nextLoad==Load::retrieveLoaded){
            pc=0x2c5c54;auto& b=*o.material68.parameters08;
            rr::game::OriginalParameterPacketHistory50LA::invalidateNative134xxx();
            if(!b.packetStarted){
                for(auto& word:b.unknown00To1C)word=0;
                b.field2C=1;b.count24=128;b.packetStarted=true;
                b.packetKind=NativeSunFlareParameters::PacketKind::retrieve;
            }
            if(!b.packet)b.packet=std::unique_ptr<NativeSunFlarePacket>(new NativeSunFlarePacket);
            b.buffer20=b.packet->words.data();
            pc=0x2c5c60;b.packet->words[0]=0;b.count28=1; //134990(a1=0).
            pc=0x2c5c68;b.packet->words[1]=b.packet->words[2]=b.packet->words[3]=0;
            b.unknown00To1C[0]=0x30000001u;b.unknown00To1C[1].reset();
            b.nativePayload04=b.buffer20;b.packetComplete=true;
            o.nextLoad=Load::retrievePacket;
        }
        if(o.nextLoad==Load::retrievePacket){
            pc=0x2c5c7c;
            auto& callback=o.material68.primitive04->optional04;
            if(!callback)callback=std::make_unique<NativeSunFlareCallback>(); //299B78 allocation.
            callback->sourceFunction=0x2c5610;callback->self=&o; //299B88 then299B90.
            //Source callback identity/custody only; no invented readback dispatch.
            o.nextLoad=Load::callbackBound;
        }
        if(o.nextLoad==Load::callbackBound){
            pc=0x2c5c88;
            o.colors44=std::unique_ptr<std::array<float,4>[]>(new std::array<float,4>[o.zones30]);
            o.nextLoad=Load::colors44Allocated; //2C5C90 publication, contents uninitialized.
        }
        if(o.nextLoad==Load::colors44Allocated){
            pc=0x2c5c98;
            o.colors48=std::unique_ptr<std::array<float,4>[]>(new std::array<float,4>[o.zones30]);
            o.nextLoad=Load::colors48Allocated; //2C5CAC delay publication BEFORE305500.
        }
        if(o.nextLoad==Load::colors48Allocated){
            pc=0x2c5ca8;
            o.buffer78=std::unique_ptr<NativeSunFlareReadback>(new NativeSunFlareReadback);
            o.nextLoad=Load::readbackAllocated; //2C5CB0, aligned64/1024B, no fake clear.
        }
        pc=0x2c5cb4;return false; //BEFORE2C5CF0 actual-camera matrix/depth calculation.
    }catch(...){return false;} //Retain every earlier source-order publication/reference.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::loadNativeSunFlareDepth2C5CF0(const original_main_entry50KG::Application& app,
    const original_main_entry50KG::RaceDisplayLifetime50LA& display,
    const original_camera_session50KX::Session& camera,std::uint32_t& pc)noexcept {
    pc=0x2c5cb4;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!nativeSunFlares_||nativeSunFlares_->sourceMode||
       !nativeSunFlares_->owner||!canRetireNativeSunFlare())return false;
    auto& o=*nativeSunFlares_->owner;using Load=NativeSunFlare::Load;
    if(o.nextLoad<Load::readbackAllocated)return false;
    if(o.nextLoad>=Load::depthCommitted){pc=0x2720d0;return false;}
    pc=0x2c5cf0;
    if(!o.context||!o.context->live()||!o.sprite64||!o.colors44||!o.colors48||!o.buffer78||
       !o.material68.primitive04||!o.material68.primitive04->optional04||o.depth40||
       std::fegetround()!=FE_TONEAREST)return false;
    const auto* video=o.context->videoIdentity();std::uint64_t revision=0;
    std::array<std::int32_t,4> rectangle{};
    if(!video||display.application_!=&app||display.video_.get()!=video||
       !runtimeCameraBorrows_||!runtimeQueries_||!runtimeQueries_->camera_||
       runtimeQueries_->camera_->session.get()!=&camera||
       !display.readColdGameplayViewport115990(camera,rectangle,revision))return false;
    const auto& viewport=camera.viewport();const auto& matrices=camera.view();
    if(!viewport.prepared||!viewport.matricesReady||viewport.sessionGeneration!=worldEntity_.sessionGeneration||
       rectangle!=std::array<std::int32_t,4>{{0,0,640,448}})return false;
    //Existing selected Session owns80 projection,C0 view,140 screen,180
    //position and190 look. Never normalize/rebuild/advance that owner here.
    //This bounded native binary32 model reuses existing strict math leaves;
    //FE_TONEAREST is native admission, NOT a claim about original FCSR.
    using Mat=std::array<float,16>;
    namespace numeric=g::original_smoke_numeric_detail50RC;
    numeric::Math m;
    Mat projection=matrices.projection; //2C5D38..54 exact local copy.
    projection[10]=numeric::bits(0x3f800054u); //2C5D64.
    projection[14]=-projection[10]; //2C5D70 NEG.S,2C5D74 store.
    const auto product=[&](const Mat& x,const Mat& y,std::uint32_t firstPC){
        Mat result;
        for(std::size_t row=0;row<4;++row)for(std::size_t lane=0;lane<4;++lane){
            constexpr std::uint32_t rowOffset[]{0,20,40,56};
            const auto at=firstPC+rowOffset[row];
            auto v=m.mul(y[lane],x[row*4],at);
            v=m.add(v,m.mul(y[4+lane],x[row*4+1],at+4),at+4);
            v=m.add(v,m.mul(y[8+lane],x[row*4+2],at+8),at+8);
            result[row*4+lane]=m.add(v,m.mul(y[12+lane],x[row*4+3],at+12),at+12);
        }
        return result;
    };
    const auto combined=product(matrices.view,projection,0x2c5da0);
    const auto screen=product(combined,viewport.screen140,0x2c5e24);
    const auto& look=matrices.sceneAfter.look;const auto& position=matrices.position;
    const std::array<float,3> direction{{look.x,look.y,look.z}},origin{{position.x,position.y,position.z}};
    std::array<float,3> farPoint{},scaled{},point{};
    constexpr std::uint32_t farPC[]{0x2c5ea0,0x2c5ea4,0x2c5eac};
    constexpr std::uint32_t scalePC[]{0x2c5ec8,0x2c5ed0,0x2c5ed8};
    for(unsigned i=0;i<3;++i)farPoint[i]=m.mul(numeric::bits(0x46c80000u),direction[i],farPC[i]);
    for(unsigned i=0;i<3;++i)scaled[i]=m.mul(numeric::bits(0x3f3504f3u),farPoint[i],scalePC[i]);
    //Source ADD.S PCs are1C,20,28 (24 is a store).
    constexpr std::uint32_t addPC[]{0x2c5f1c,0x2c5f20,0x2c5f28};
    for(unsigned i=0;i<3;++i)point[i]=m.add(origin[i],scaled[i],addPC[i]);
    std::array<float,4> transformed{};
    const auto forcedW=m.add(1.f,0.f,0x14eb50); //14EAE0 ignores inputW, forces1.
    for(unsigned lane=0;lane<4;++lane){
        auto v=m.mul(screen[lane],point[0],0x14eb54);
        v=m.add(v,m.mul(screen[4+lane],point[1],0x14eb58),0x14eb58);
        v=m.add(v,m.mul(screen[8+lane],point[2],0x14eb5c),0x14eb5c);
        transformed[lane]=m.add(v,m.mul(screen[12+lane],forcedW,0x14eb60),0x14eb60);
    }
    const auto depth=m.div(transformed[2],transformed[3],0x2c5f4c);
    const auto integerDepth=m.cvtWord(depth,0x2c5f50); //Existing finite signed truncation contract.
    if(m.badPC){pc=m.badPC;return false;}
    std::array<std::int32_t,4> afterRectangle{};std::uint64_t afterRevision=0;
    if(!display.readColdGameplayViewport115990(camera,afterRectangle,afterRevision)||
       afterRectangle!=rectangle||afterRevision!=revision)return false;
    pc=0x2c5f5c;o.depth40=integerDepth; //First reached write to actual flare40.
    o.nextLoad=Load::depthCommitted;
    pc=0x2720d0;return false; //2C5A10 returned; BEFORE authored saturation branch.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::loadNativeSunFlareSaturation2720D0(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x2720d0;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!nativeSunFlares_||nativeSunFlares_->sourceMode||
       !nativeSunFlares_->owner||!canRetireNativeSunFlare()||!nativeSceneInfo270900Current(app,pc)||
       !nativeTob34_||!nativeTob34_->document)return false;
    auto& o=*nativeSunFlares_->owner;using Load=NativeSunFlare::Load;
    if(o.nextLoad!=Load::depthCommitted&&o.nextLoad!=Load::saturationComplete)return false;
    if(o.nextLoad==Load::saturationComplete){pc=0x271d88;return false;}
    if(!o.inputs.saturation||!o.depth40||!o.colors44||!o.colors48||o.zones30!=10||
       !nativeSunFlares_->count4A8||*nativeSunFlares_->count4A8!=1||
       !nativeSunFlarePointers4A4_||!*nativeSunFlarePointers4A4_||
       *nativeSunFlarePointers4A4_!=nativeSunFlares_->pointerBacking.get()||
       (*nativeSunFlarePointers4A4_)[0]!=&o)return false;
    const Block* block=nullptr;const Field* field=nullptr;
    if(!findUniqueBlock(*nativeTob34_->document,"sunflare1",block)||!block)return false;
    pc=0x2720ec;
    if(!findUniqueField(*block,"saturationmincolor",field)||field)return false;
    pc=0x272108;
    if(!findUniqueField(*block,"saturationmaxcolor",field)||field)return false;
    //Authenticated exact RA per-zone colors.26FA60 parses four integer
    //components then CVT.S.W; all admitted values are exactly representable.
    //Zone9 intentionally has min200 > max160; preserve, never repair/clamp.
    constexpr std::int32_t minimum[]{70,70,160,160,160,160,160,70,200,160};
    constexpr std::int32_t maximum[]{70,160,200,200,200,200,200,160,160,200};
    try {
        for(;o.colorRowsInitialized<o.zones30;){
            const auto row=o.colorRowsInitialized;
            pc=0x2721e0;
            const auto minName=std::string("saturationmincolorforzone")+std::to_string(row+1);
            pc=0x2721f4;
            if(!findUniqueField(*block,minName.c_str(),field)||!field||
               field->value!=std::string("0,0,0,")+std::to_string(minimum[row]))return false;
            const std::array<float,4> minColor{{0,0,0,static_cast<float>(minimum[row])}};
            pc=0x27223c;
            const auto maxName=std::string("saturationmaxcolorforzone")+std::to_string(row+1);
            pc=0x272250;
            if(!findUniqueField(*block,maxName.c_str(),field)||!field||
               field->value!=std::string("0,0,0,")+std::to_string(maximum[row]))return false;
            const std::array<float,4> maxColor{{0,0,0,static_cast<float>(maximum[row])}};
            pc=0x27229c;
            //2C6008..1C source bounds condition is true for actual row0..9.
            //Private bounded native leaf, no fake shared assertion execution.
            rr::game::OriginalSunFlareSaturationHistory50LA::invalidateNative2C5F80();
            o.flags98KnownValue=static_cast<std::uint8_t>((o.flags98KnownValue&~2u)|2u); //2C6060.
            for(unsigned lane=0;lane<4;++lane)o.colors44[row][lane]=minColor[lane]; //2C6070..88.
            for(unsigned lane=0;lane<4;++lane)o.colors48[row][lane]=maxColor[lane]; //2C6098..B0.
            ++o.colorRowsInitialized; //2C5F80 returned then2722A4 increments zone.
        }
    }catch(...){return false;} //Earlier initialized rows remain owned/known, never replayed.
    o.colorsInitialized=true;o.nextLoad=Load::saturationComplete;
    //One authored flare:2722B8 increments flareindex1,2722C4 exits,2722E8
    //returns271DD0 through1B46E0. Only+C0 completed; no World/A4 success.
    pc=0x271d88;return false; //BEFORE actual+C4 ParticleEmitter call2722F0.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::canRetireNativeParticleList3D8()const noexcept {
    if(sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::unreached)return preflightSourceWorldParticleCountNative();
    if(!nativeParticleList3D8_)return true;
    if(!worldApplication_||worldApplication_->attachmentLock423F58()||!particles_||particles_->closed_)return false;
    const auto& l=*nativeParticleList3D8_;const auto& e=l.entity;
    if(!l.counted||l.actualOwner!=particles_.get()||(l.count30!=0&&l.count30!=4)||
       l.published>4||l.loaded>l.published||(l.published&&!l.pointers34)||
       (l.pointers34&&l.count30!=4)||(l.complete&&l.loaded!=4)||l.servicesCompleted>l.loaded||
       (l.serviceComplete&&l.servicesCompleted!=4)||
       e.category28!=0xff||(e.flags29&15u)!=3u||e.nativeFlags29||e.parent1C||e.previous10||e.next14||
       e.mask20||!canRetireNativeParticleEntity274370(e,false))return false;
    const Entity* previous=nullptr;const Entity* first=nullptr;std::size_t enrolled=0;
    for(std::size_t i=0;i<4;++i){
        if(i>=l.published){if(l.rows[i])return false;continue;}
        const auto& row=l.rows[i];const auto& owned=particles_->emitters_[i];const auto& c=owned.entity;
        if(!row||l.pointers34[i]!=row.get()||owned.retired||
           c.reference00!=1||c.sessionGeneration!=worldEntity_.sessionGeneration||c.objectKey!=key(&c)||
           c.kind!=g::OriginalWreckComponentKind50LA::effect15DE00||c.firstChild18||c.descendants24||
           (row->emitterPublished&&(!row->emitterPointers00||row->emitterPointers00[0]!=&c))||
           (row->unknown08&&*row->unknown08!=8)||
           (row->unknown0C&&*row->unknown0C!=1)||(row->unknown10&&*row->unknown10!=1)||
           (row->emitterEnrolled!=owned.enrolled)||
           (row->emitterEnrolled&&!canRetireNativeParticleEntity274370(c,true))||
           (row->configured&&!owned.loaderComplete)||(row->started&&!row->configured)||
           (row->enabled&&!row->started)||(row->attached&&(!row->enabled||!row->emitterEnrolled))||
           (row->descriptorRetired&&(!row->attached||row->descriptor04))||
           (i<l.servicesCompleted&&!row->descriptorRetired))return false;
        if(i<l.loaded&&!row->descriptorRetired&&(!row->descriptor04||!row->descriptor04->loaded||
           row->descriptor04->actualAttachment!=&particles_->attachments_[i]))return false;
        if(row->descriptor04&&row->descriptor04->actualAttachment&&
           row->descriptor04->actualAttachment!=&particles_->attachments_[i])return false;
        if(row->emitterEnrolled)++enrolled;
        if(owned.attachment74&&owned.attachment74!=row->attachment74)return false;
        if(row->attached){
            if(c.parent1C!=&e||c.previous10!=previous||c.mask20!=0x402u||
               (previous&&previous->next14!=&c))return false;
            if(!first)first=&c;previous=&c;
        }else if(c.parent1C||c.previous10||c.next14||c.mask20)return false;
    }
    return worldApplication_->objectCount423F60()>=enrolled+1u&&
        e.firstChild18==first&&(!previous||!previous->next14)&&e.descendants24==(first?0x402u:0u);
}
bool Owner::retireNativeParticleList3D8()noexcept {
    if(sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::unreached){std::uint32_t pc=0;return abortSourceWorldParticleDescriptorsNative(pc);}
    if(!canRetireNativeParticleList3D8())return false;
    if(!nativeParticleList3D8_)return true;
    auto& l=*nativeParticleList3D8_;
    //142590 first releases the child chain through1424E0 in reverse sibling
    //order. Actual source15DE60 owns no forced-set array for this exact EMT.
    //No external forest walk occurs between this preflight and list release.
    for(std::size_t n=l.published;n;){const auto i=--n;auto& row=*l.rows[i];
        auto& emitter=particles_->emitters_[i];auto& e=emitter.entity;
        if(!row.emitterEnrolled)continue;
        if(!releaseNativeParticleEntity142590(e,true,row.attached?&l.entity:nullptr,
            row.attached?e.previous10:nullptr))std::terminate();
        // Unattached enrolled failures are native partial-construction cleanup,
        // not an extra source list child or fabricated source loader return.
        row.attached=false;row.emitterEnrolled=false;emitter.enrolled=false;emitter.retired=true;
        emitter.manager70=nullptr;emitter.numericManager70=nullptr;emitter.attachment74=nullptr;
        row.attachment74=nullptr;
    }
    //27DCD0 ascending row descriptor, raw pointer array, row; then34.
    //Raw pointer arrays own NO additional emitter references/counts.
    for(std::size_t i=0;i<l.published;++i){
        l.rows[i]->descriptor04.reset();l.rows[i]->emitterPointers00.reset();
        l.rows[i].reset();l.pointers34[i]=nullptr;
    }
    l.pointers34.reset();
    if(!releaseNativeParticleEntity142590(l.entity,false,nullptr,nullptr))std::terminate();
    l.counted=false;nativeParticleList3D8_.reset();return true;
}
bool Owner::loadNativeParticleDescriptors2722F0(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    if(sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::unreached)return false;
    pc=0x271d88;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeSunFlares_||nativeSunFlares_->sourceMode||!nativeSunFlares_->owner||
       nativeSunFlares_->owner->nextLoad!=NativeSunFlare::Load::saturationComplete||
       !nativeTob34_||!nativeTob34_->document||!nativeSceneInfo270900Current(app,pc)||
       !particles_||!particles_->decoded_||particles_->closed_||particles_->source_!=source_||
       particles_->application_!=worldApplication_||particles_->generation_!=worldEntity_.sessionGeneration||
       particles_->constructed_!=4||!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    try {
        if(!nativeParticleList3D8_){
            //Actual first entry of this NEW native metadata domain. Existing
            //particle owner/controls remain intact; no source270550 reset claim.
            pc=0x272308;
            auto list=std::make_unique<NativeParticleList3D8>();
            if(!constructNativeParticleList27DC9C(list->entity,pc))return false;
            list->counted=true;list->actualOwner=particles_.get();
            pc=0x272324;nativeParticleList3D8_=std::move(list); //World3D8 publication before reader.
        }
        if(!canRetireNativeParticleList3D8())return false;
        auto& l=*nativeParticleList3D8_;
        if(l.complete){pc=0x271d98;return false;}
        const Block* block=nullptr;const Field* field=nullptr;
        pc=0x27de30;
        if(!findUniqueBlock(*nativeTob34_->document,"particleemitters",block)||!block)return false;
        pc=0x27de64;
        if(!findUniqueField(*block,"numberofemitters",field)||!field||field->value!="4")return false;
        if(!l.pointers34){
            pc=0x27deb0;l.count30=4; //Count store before pointer-array allocation.
            pc=0x27deb8;
            l.pointers34=std::unique_ptr<NativeParticleRow27DE00*[]>(new NativeParticleRow27DE00*[4]);
            //Unpublished slots are deliberately unknown/unread, not fake source-null stores.
        }
        for(;l.loaded<4;){
            const auto i=l.loaded;
            pc=0x27dee8;
            const auto name=std::string("particleemitter")+std::to_string(i+1);
            if(!findUniqueBlock(*nativeTob34_->document,name.c_str(),block)||!block)return false;
            if(!l.rows[i]){
                pc=0x27df14;auto row=std::make_unique<NativeParticleRow27DE00>();
                l.rows[i]=std::move(row);pc=0x27df40;l.pointers34[i]=l.rows[i].get();++l.published;
            }
            auto& row=*l.rows[i];
            if(!row.descriptor04){
                pc=0x27c768;auto descriptor=std::make_unique<NativeParticleDescriptor27C430>();
                pc=0x27c780;row.descriptor04=std::move(descriptor);
            }
            auto& d=*row.descriptor04;const auto& actual=particles_->attachments_[i];
            //Same authenticated existing native attachment, not a shadow emitter.
            if(actual.zone!=1||actual.type!=8||actual.instances!=1||actual.ordinal!=366+i||
               actual.offset50!=std::array<float,4>{{0,15,0,1}})return false;
            pc=0x27c628;
            if(!findUniqueField(*block,"startingzone",field)||!field||field->value!="1")return false;
            d.startingZone00=1;
            pc=0x27c640;
            if(!findUniqueField(*block,"typeid",field)||!field||field->value!="8")return false;
            d.type04=8;
            pc=0x27c660;
            if(!findUniqueField(*block,"attachtosoultree",field)||!field||field->value!=actual.soulTree)return false;
            d.attach08=field->value; //27C678 non-NONE branch, genuine authored name.
            pc=0x27c6c0;
            if(!findUniqueField(*block,"childobject",field)||field)return false;
            d.child10C="NONE"; //Source default3BB488, not guessed missing binding.
            pc=0x27c6dc;
            if(!findUniqueField(*block,"offsetfromsoultree",field)||!field||field->value!="0.0,15.0,0.0")return false;
            d.offset320=std::array<float,3>{{actual.offset50[0],actual.offset50[1],actual.offset50[2]}};
            //27C6DC->158C00 changes onlyXYZ. Preserve unknown sourceW32C;
            //same native emitter's establishedW1 remains untouched/unreplayed.
            pc=0x27c6f4;
            if(!findUniqueField(*block,"numberofinstances",field)||!field||field->value!="1")return false;
            d.instances330=1;
            pc=0x27c714;
            if(!findUniqueField(*block,"emitterfilename",field)||!field||field->value!="rafire.emt")return false;
            d.emitter210=field->value;
            pc=0x27c72c;
            if(!findUniqueField(*block,"starton",field)||field)return false;
            d.startOn334=1;d.actualAttachment=&actual;d.loaded=true;
            ++l.loaded; //27C600/27C750 return,27DF60/6C advance source rows.
        }
        l.complete=true;
        pc=0x271d98;return false; //27DE00 returns1;27233C skips delete; +C4 only.
    }catch(...){return false;} //Owned source-order allocations/stores survive bounded retry/retirement.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::loadNativeEmptyBuoys1B5090(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    if(sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::unreached)return false;
    pc=0x271d98;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeParticleList3D8_||nativeParticleList3D8_->sourceMode||!nativeParticleList3D8_->complete||
       !nativeTob34_||!nativeTob34_->document||!nativeSceneInfo270900Current(app,pc))return false;
    if(nativeBuoyCount538_){if(*nativeBuoyCount538_!=0)return false;pc=0x271da8;return false;}
    pc=0x1b50c8;
    //157D00 failed selection retains prior section. Prove field absence in
    //EVERY authenticated section too, rather than assume a fresh reader.
    for(const auto& block:nativeTob34_->document->blocks){
        if(block.name=="buoys")return false;
        for(const auto& field:block.fields)if(field.name=="numberofbuoys")return false;
    }
    pc=0x1b50e0;nativeBuoyCount538_=0; //Actual158B30 default0 writes World538.
    //1B50EC sees0 and skips allocation/table53C entirely. Do not initialize,
    //reset or claim a known-null53C; no source constructor/reset receipt.
    pc=0x271da8;return false; //1B5330 return; BEFORE final+B0 StaticModelParams.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::loadNativeStaticModelParams271A00(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    if(sourceStaticModelParamPhase_!=SourceStaticModelParamPhase::unreached)return false;
    pc=0x271da8;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeBuoyCount538_||*nativeBuoyCount538_||!nativeTob34_||!nativeTob34_->document||
       !nativeSceneInfo270900Current(app,pc)||!world_||world_->closed_||world_->source_!=source_||
       !world_->context_||!world_->context_->live()||!world_->modelPrefixes_)return false;
    auto& owner=*world_->modelPrefixes_;
    if(owner.closed_||!owner.constructed_||owner.source_!=source_||owner.application_!=worldApplication_||
       owner.generation_!=worldEntity_.sessionGeneration||!owner.canClose()||
       owner.parents_.size()!=10||owner.models_.size()!=1085||owner.unresolved_!=176||
       world_->collisionOnly_.size()!=176||nativeStaticModelParamsNext_>673)return false;
    if(nativeStaticModelParamEffectsComplete_){
        if(nativeStaticModelParamsNext_!=673)return false;
        pc=0x271db0;return false; //No fullA4 success ahead of separate epilogue join.
    }
    const auto* source=source_?source_->sourcesDecoded():nullptr;
    if(!source||source->world.instances.size()!=1261||source->world.zones.size()!=10)return false;
    try {
        const auto fold=[](std::string& text){
            for(char& c:text){const auto u=static_cast<unsigned char>(c);
                if(u>=128u||!u)return false;if(c>='A'&&c<='Z')c=static_cast<char>(c+('a'-'A'));}
            return true;
        };
        const Block* block=nullptr;const Field* field=nullptr;
        pc=0x271a34;
        if(!findUniqueBlock(*nativeTob34_->document,"staticmodelparams",block)||!block)return false;
        pc=0x271a4c;
        if(!findUniqueField(*block,"numberofstaticmodelparams",field)||!field||field->value!="673")return false;
        //NATIVE admission preflight over the same immutable authenticated data,
        //not receipts that source read all rows before its ordered writes.
        std::array<std::string,673> parameterNames;
        for(std::size_t i=0;i<673;++i){
            const auto section=std::string("model")+std::to_string(i+1);
            if(!findUniqueBlock(*nativeTob34_->document,section.c_str(),block)||!block||
               !findUniqueField(*block,"slt",field)||!field)return false;
            auto name=field->value;
            if(name.empty()||name.size()>=256||!fold(name))return false;
            const auto dot=name.find_last_of('.'); //271AA0->1169D0 LAST dot.
            if(dot!=std::string::npos)name.resize(dot);
            if(name.empty())return false;
            parameterNames[i]=std::move(name);
            if(!findUniqueField(*block,"allowshadowreceiving",field)||!field||field->value!="F")return false;
            //158920 recognizes F and writes0; no atoi or boolean guess/default1.
        }
        using Descriptor=ra_static_model_cold50LA::Owner::Descriptor;
        struct SlotView {Descriptor* actual=nullptr;std::string name00;};
        std::vector<SlotView> slots;slots.reserve(1261);
        std::size_t unknown=0,actual=0,worldIndex=0;
        for(std::size_t zone=0;zone<10;++zone){
            const auto& parent=owner.parents_[zone];
            if(!parent||!parent->constructed||parent->released||parent->zone!=zone+1||
               parent->entries34.size()!=source->world.zones[zone].models||
               parent->count30!=parent->entries34.size())return false;
            for(std::size_t ordinal=0;ordinal<parent->entries34.size();++ordinal,++worldIndex){
                const auto& slot=parent->entries34[ordinal];
                if(slot.worldInstance!=worldIndex||worldIndex>=source->world.instances.size())return false;
                const auto& instance=source->world.instances[worldIndex];
                if(instance.zone!=zone+1||instance.ordinal!=ordinal+1||
                   instance.parameter_block>=source->world.parameters.blocks.size())return false;
                const auto& sourceBlock=source->world.parameters.blocks[instance.parameter_block];
                if(!findUniqueField(sourceBlock,"slt",field)||!field)return false;
                auto stem=field->value;const auto first=stem.find('.');
                stem.resize(std::min<std::size_t>(first==std::string::npos?stem.size():first,50));
                if(stem.empty()||!fold(stem))return false; //27A524 FIRST dot,cap50;308F00 ASCII.
                if(!findUniqueField(sourceBlock,"statictype",field)||(field&&field->value!="Standard"))return false;
                if(slot.model){
                    auto& model=*slot.model;auto& wrapper=model.wrapper;
                    if(model.source.worldInstance!=worldIndex||model.parent!=zone||model.slot!=ordinal||
                       wrapper.type00!=1||wrapper.dispatch!=decltype(wrapper.dispatch)::standard3D26F0||
                       !wrapper.descriptor10||wrapper.descriptor10->source!=&instance||
                       wrapper.model8!=&model.scene||!model.scene.model()||model.scene.released())return false;
                    auto nativeName=wrapper.descriptor10->modelName180;
                    if(!fold(nativeName)||nativeName!=stem+".slt")return false;
                    slots.push_back({wrapper.descriptor10.get(),std::move(stem)});++actual;
                }else{
                    //Unknown retail presence/type, NOT a proven null source wrapper.
                    //Tie exact unresolved parent slot to existing collision-only owner.
                    const ra_static_world50LA::Owner::CollisionOnly* collision=nullptr;
                    for(const auto& candidate:world_->collisionOnly_)if(candidate.worldInstance==worldIndex){
                        if(collision)return false;collision=&candidate;}
                    if(!collision||collision->zone!=instance.zone||collision->ordinal!=instance.ordinal||
                       collision->absentGeometry!=instance.geometry_member||
                       collision->presentCollision!=instance.collision_member||
                       instance.geometry_member!="geometry\\"+stem+".psg")return false;
                    for(const auto& parameter:parameterNames)if(parameter==stem)return false;
                    //Every possible authored type1 descriptor is nonmatching;
                    //other type/null also cannot reach271B4C. Source path before
                    //that store only reads/compares: bounded no-effect adaptation.
                    slots.push_back({nullptr,std::move(stem)});++unknown;
                }
            }
        }
        if(worldIndex!=1261||actual!=1085||unknown!=176)return false;
        std::size_t stores=0,affected=0;
        for(const auto& slot:slots)if(slot.actual){
            std::size_t matches=0;for(const auto& name:parameterNames)if(slot.name00==name)++matches;
            stores+=matches;if(matches)++affected;
        }
        if(stores!=1049||affected!=1047)return false; //Exact authenticated RA intersection.
        //All fallible allocations/proofs precede new stores. Exact673 source
        //row order retained, including duplicate names, then zone/slot order.
        for(;nativeStaticModelParamsNext_<673;++nativeStaticModelParamsNext_){
            pc=0x271ae0;const auto& name=parameterNames[nativeStaticModelParamsNext_];
            for(auto& slot:slots)if(slot.actual&&slot.name00==name){
                pc=0x271b4c;slot.actual->allowShadowReceiving148=0;
            }
        }
        nativeStaticModelParamEffectsComplete_=true;
        pc=0x271db0;return false; //271A00 metadata effects complete; BEFORE A4 true-return.
    }catch(...){return false;}
}
}

namespace rr::game {
bool OriginalRaceConstructor50LA::continueNativeTrackAfterTob(
    const original_main_entry50KG::Application& app,std::uint32_t& pc)noexcept {
    using Step=NativeTrackLoadStep;
    pc=0x1aa0e0;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!nativeIntroWorldEntity()||!world74_||
       !profile_||nativeTrackName_!=std::array<char,3>{{'r','a','\0'}}||!nativeTrackArchiveCurrent(app))return false;
    if(nativeTrackLoadStep_==Step::nativeAdaptedReturn)return continueNativeTrackAfterWater(app,pc);
    if(nativeTrackLoadStep_==Step::aubRetired){
        if(!world74_->nativeAubRoleRetired272930_||world74_->nativeAub38_)return false;
        if(world74_->continueNativeZoneServices2729E0(app,pc))return true;
        if(pc!=0x1aa400)return false;
        return continueNativeTrackAfterWater(app,pc); //No reader republication.
    }
    if(nativeTrackLoadStep_==Step::nativeAdaptedA4){
        if(!world74_->withdrawNativeTob34_272800(app,pc))return false;
        nativeTrackLoadStep_=Step::tobRetired;
    }
    if(nativeTrackLoadStep_!=Step::tobRetired&&nativeTrackLoadStep_!=Step::aub)return false;
    if(!world74_->bindNativeAub38_1B4720(app,pc))return false;
    nativeTrackLoadStep_=Step::aub;
    if(world74_->loadNativeEmptyAmbient271470(app,pc))return true;
    if(pc!=0x2714d8)return false;
    if(world74_->loadNativeSoulTreeSoundDescriptors2714D8(app,pc))return true;
    if(pc!=0x271530)return false;
    if(world74_->loadNativeMaterialSoundDescriptors271530(app,pc))return true;
    if(pc!=0x2715e4||!world74_->completeNativeAudio2728E0(app,pc))return false;
    if(!world74_->withdrawNativeAub38_272930(app,pc))return false;
    nativeTrackLoadStep_=Step::aubRetired;
    if(world74_->continueNativeZoneServices2729E0(app,pc))return true;
    if(pc!=0x1aa400)return false;
    return continueNativeTrackAfterWater(app,pc);

}
}
namespace ra_cold_aggregate50LA {
bool Owner::withdrawNativeTob34_272800(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa0e0;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||nativeScb34_||nativeAub38_||
       !nativeSceneInfo270900Current(app,pc)||!nativeStaticModelParamEffectsComplete_||
       nativeStaticModelParamsNext_!=673||!particles_||particles_->closed_)return false;
    pc=0x27281c;
    if(nativeTob34_){
        const auto& reader=*nativeTob34_;
        if(reader.next!=NativeTobReader34::Next::complete||reader.source!=source_||
           reader.provider!=rosterMountDomain_->archiveProvider3E1A70()||
           reader.authoring!=particles_.get()||reader.document!=&particles_->tob_)return false;
        //Native reader-role destruction corresponding to1573B0 then272824
        //World34 clear. Shared authenticated TOB/particle/world backing survives.
        nativeTob34_.reset();worldParser34_=nullptr;
    }
    pc=0x1aa0f4;return true;
}
bool Owner::bindNativeAub38_1B4720(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa0f4;
    if(sourceAudioPhase_!=SourceAudioPhase::unreached||nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||nativeAubRoleRetired272930_||nativeScb34_||nativeTob34_||
       !nativeStaticModelParamEffectsComplete_||!nativeSceneInfo270900Current(app,pc)||
       !audio_||!audio_->decoded_||audio_->source_!=source_)return false;
    try {
        const auto provider=rosterMountDomain_->archiveProvider3E1A70();
        pc=0x1b47ac;
        if(!nativeAub38_){
            nativeAub38_.emplace(); //Native parser-role publication BEFORE1577D0.
            nativeAub38_->source=source_;nativeAub38_->provider=provider;nativeAub38_->authoring=audio_.get();
        }
        auto& reader=*nativeAub38_;
        if(reader.source!=source_||reader.provider!=provider||reader.authoring!=audio_.get())return false;
        //1B4740 formats%s%s.aud;1B4758 LAST-dot replacement with.aub.
        //Same RA track/provider admission, primary authenticated member only.
        pc=0x1b47b8;
        a::OriginalStandardArchiveProvider50LA::MemberView member;
        a::OriginalStandardArchiveProvider50LA::MountView mount;
        if(!provider||provider->lookup132820("tracks\\ra.aub",member)!=
               a::OriginalStandardArchiveProvider50LA::Lookup::present||
           !provider->mount(member.mount,mount)||!mount.archiveName||*mount.archiveName!="TRACKS/RA.RES"||
           !mount.archiveSha256||*mount.archiveSha256!=ra_cold_sources50LA::archiveSha||
           mount.archiveBytes!=4604561u||mount.memberCount!=1010u||!member.member||!member.data||member.size!=22511u||
           member.revision!=world_->providerRevision_)return false;
        const auto& bytes=audio_->bytes_[0];const auto& document=audio_->aub_;
        if(bytes.size()!=member.size||document.original_bytes!=bytes||
           std::memcmp(member.data,bytes.data(),bytes.size()))return false;
        if(reader.document&&(reader.document!=&document||reader.memberName!=*member.member||
           reader.member.size!=member.size||reader.member.mount!=member.mount||reader.member.ordinal!=member.ordinal||
           reader.member.revision!=member.revision||reader.member.bankGeneration!=member.bankGeneration))return false;
        //Equal exact originating identity/extents/bytes; allocation addresses
        //are not cross-provider provenance. No second parse/copy or Sound owner.
        reader.memberName=*member.member;reader.member=member;reader.document=&document;
        //Primary load succeeded:1B47C0 branches1B485C; no AUD fallback,
        //434318 editor export,1573B0(otherWorld34) or fake source side effect.
        pc=0x1aa108;return true; //1B4870 returns1, caller takes+E8 branch next.
    }catch(...){return false;} //Published empty role retained on failure; backing untouched.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::loadNativeEmptyAmbient271470(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa114;
    if(sourceAmbientPhase_!=SourceAmbientPhase::unreached||nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeAub38_||!nativeAub38_->document||!audio_||!audio_->decoded_||
       nativeAub38_->authoring!=audio_.get()||nativeAub38_->document!=&audio_->aub_||
       nativeAub38_->source!=source_||!nativeSceneInfo270900Current(app,pc)||
       !worldColdConstructed_||!worldApplication_||worldApplication_->attachmentLock423F58()||
       nativeEmptyAmbient3B4_)return false;
    if(nativeEmptyAmbient271470Complete_){pc=0x2714d8;return false;}
    //2728E0 observes actual World38, dispatches+EC=1B46D0->271470.
    //Whole authenticated74-section AUB admission, not a source parser call.
    //42 SoulTreeSound rows are a DIFFERENT subsequent list (World3B8).
    const auto& document=*nativeAub38_->document;
    const Block* sounds=nullptr;const Field* field=nullptr;
    if(document.blocks.size()!=74||!findUniqueBlock(document,"sounds",sounds)||!sounds||
       !findUniqueField(*sounds,"numambientsounds",field)||field)return false;
    for(const auto& block:document.blocks)
        for(const auto& f:block.fields)if(f.name=="numambientsounds")return false;
    auto& count=worldApplication_->objectCount423F60();
    pc=0x271480;
    if(!reserveNativeSoundListCustody())return false;
    std::unique_ptr<NativeEmptyAmbient3B4> list;
    try{list=std::make_unique<NativeEmptyAmbient3B4>();}catch(...){return false;}
    auto& entity=list->entity;
    if(!canEnrollNativeSoundListCustody(entity))return false;
    pc=0x2792ec; //2792E0(a1=1)->141300, actual Application count and authority.
    constructNativeSoundListCustody(entity);
    //279300 count30=0,279304 pointer34=null; publication BEFORE+74 reader.
    nativeEmptyAmbient3B4_=std::move(list);pc=0x27149c;
    pc=0x2793e0;nativeEmptyAmbient3B4_->count30=0; //Sounds exists, missing NumAmbientSounds defaults0.
    //2793F8 false ->2714C8 slot10 final release. No array or Sound constructed.
    pc=0x2714c8;
    if(entity.reference00!=1||entity.category28!=0xff||(entity.flags29&15u)!=3u||
       entity.nativeFlags29||entity.parent1C||entity.previous10||entity.next14||
       entity.firstChild18||entity.mask20||entity.descendants24||!count||
       nativeEmptyAmbient3B4_->count30||nativeEmptyAmbient3B4_->rows34||
       !canRetireNativeSoundListCustody(entity))std::terminate();
    //279320 sees null34;279368->1413E0 then listfree. No fallible operation
    //after publication, so this balanced empty leaf cannot strand a counted owner.
    retireNativeSoundListCustody(entity);
    --entity.reference00;--count;
    nativeEmptyAmbient3B4_.reset();pc=0x2714d0; //Real bounded World3B4 clear.
    nativeEmptyAmbient271470Complete_=true;
    pc=0x2714d8;return false; //BEFORE distinct42-row SoulTreeSound list constructor.
}
}

namespace {
//Exact retained RA numeric subset only: unsigned integral decimal, optional.0.
//All admitted values<=65535 are exactly representable in binary32, independent
//of FCSR rounding. Not a replacement for general original atof/310440 semantics.
bool raSoundExactInteger(const std::string& text,std::size_t at,std::size_t length,
    bool dotZero,float& out)noexcept {
    if(at>text.size()||length>text.size()-at||!length)return false;
    if(dotZero){if(length<3||text[at+length-2]!='.'||text[at+length-1]!='0')return false;length-=2;}
    std::uint32_t n=0;
    for(std::size_t i=0;i<length;++i){const char c=text[at+i];
        if(c<'0'||c>'9'||n>6553u)return false;
        n=n*10u+std::uint32_t(c-'0');if(n>65535u)return false;
    }
    out=static_cast<float>(n);return true;
}
bool raSoundScalar(const Block& block,const char* name,float fallback,float& out)noexcept {
    const Field* f=nullptr;if(!findUniqueField(block,name,f))return false;
    if(!f){out=fallback;return true;}
    return raSoundExactInteger(f->value,0,f->value.size(),false,out);
}
bool raSoundBool(const Block& block,const char* name,std::int32_t& out)noexcept {
    const Field* f=nullptr;if(!findUniqueField(block,name,f))return false;
    if(!f){out=0;return true;}
    if(f->value=="T"){out=1;return true;}if(f->value=="F"){out=0;return true;}return false;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::canRetireNativeSoulTreeSounds3B8()const noexcept {
    if(nativeSoulTreeSounds3B8_&&nativeSoulTreeSounds3B8_->sourceMode)return preflightSourceWorldSoulTreeSoundsNative();
    if(!nativeSoulTreeSounds3B8_)return true;
    if(!worldApplication_||worldApplication_->attachmentLock423F58()||!audio_||!audio_->decoded_)return false;
    const auto& l=*nativeSoulTreeSounds3B8_;const auto& e=l.entity;
    if(!l.counted||l.actualOwner!=audio_.get()||(l.count30!=0&&l.count30!=42)||l.loaded>42||
       (l.rows34&&l.count30!=42)||(l.loaded&&!l.rows34)||(l.complete&&l.loaded!=42)||
       e.kind!=g::OriginalWreckComponentKind50LA::entity141300||e.reference00!=1||
       e.objectKey!=key(&e)||e.sessionGeneration!=worldEntity_.sessionGeneration||
       e.category28!=0xff||(e.flags29&15u)!=3u||e.nativeFlags29||e.parent1C||e.firstChild18||
       e.previous10||e.next14||e.mask20||e.descendants24||!worldApplication_->objectCount423F60()||
       !canRetireNativeSoundListCustody(e))return false;
    if(l.rows34)for(std::size_t i=0;i<42;++i){const auto& row=l.rows34[i];
        if(row.runtime04)return false;
        if(i<l.loaded&&(!row.descriptor08||!row.descriptor08->loaded))return false;
        if(row.descriptor08&&row.descriptor08->actualSource&&
           row.descriptor08->actualSource!=audio_->ambient_[i])return false;
    }
    return true;
}
bool Owner::retireNativeSoulTreeSounds3B8()noexcept {
    if(sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached){std::uint32_t pc=0;return abortSourceWorldSoulTreeSoundsNative(pc);}
    if(!canRetireNativeSoulTreeSounds3B8())return false;
    if(!nativeSoulTreeSounds3B8_)return true;
    auto& l=*nativeSoulTreeSounds3B8_;
    //Native partial-load custody extension: retail279890 frees the raw row
    //array without visiting unconsumed descriptors. Here278E80 has not consumed
    //them, so explicitly dispose those allocations first; do NOT claim original
    //full-runtime cleanup or destroy any Sound/manager/stream we do not own.
    if(l.rows34)for(std::size_t i=0;i<42;++i)l.rows34[i].descriptor08.reset();
    l.rows34.reset(); //2798CC rawarray release, then2798D8->1413E0.
    retireNativeSoundListCustody(l.entity);
    --l.entity.reference00;--worldApplication_->objectCount423F60();l.counted=false;
    nativeSoulTreeSounds3B8_.reset();return true;
}
bool Owner::loadNativeSoulTreeSoundDescriptors2714D8(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    if(sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached||
       (nativeSoulTreeSounds3B8_&&nativeSoulTreeSounds3B8_->sourceMode))return false;
    pc=0x2714d8;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!nativeEmptyAmbient271470Complete_||
       !nativeAub38_||!audio_||!audio_->decoded_||nativeAub38_->authoring!=audio_.get()||
       nativeAub38_->document!=&audio_->aub_||audio_->source_!=source_||
       !nativeSceneInfo270900Current(app,pc)||!worldColdConstructed_||!worldApplication_||
       worldApplication_->attachmentLock423F58())return false;
    try {
        if(!nativeSoulTreeSounds3B8_){
            auto list=std::make_unique<NativeSoulTreeSoundList3B8>();
            if(!reserveNativeSoundListCustody()||!canEnrollNativeSoundListCustody(list->entity))return false;
            pc=0x27985c;
            constructNativeSoundListCustody(list->entity);
            list->counted=true;list->actualOwner=audio_.get();
            pc=0x2714f4;nativeSoulTreeSounds3B8_=std::move(list); //Publication BEFORE reader279910.
        }
        if(!canRetireNativeSoulTreeSounds3B8())return false;
        auto& l=*nativeSoulTreeSounds3B8_;
        if(l.complete){pc=0x271530;return false;}
        const auto& document=*nativeAub38_->document;const Block* block=nullptr;const Field* field=nullptr;
        pc=0x279938;
        if(!findUniqueBlock(document,"sounds",block)||!block)return false;
        pc=0x279950;
        if(!findUniqueField(*block,"numsoultreesounds",field)||!field||field->value!="42")return false;
        l.count30=42; //158B30 count store BEFORE allocation.
        if(!l.rows34){
            pc=0x279978;auto rows=std::make_unique<NativeSoulTreeSoundRow278DD0[]>(42);
            //Native typed array replaces12-byte guest rows+16-byte array header;
            //all42 constructors complete BEFORE27999C pointer34 publication.
            pc=0x27999c;l.rows34=std::move(rows);
        }
        while(l.loaded<42){
            const auto i=l.loaded;pc=0x2799c4;
            const auto name=std::string("soultreesound")+std::to_string(i+1);
            if(!findUniqueBlock(document,name.c_str(),block)||!block||block!=audio_->ambient_[i])return false;
            auto& row=l.rows34[i];
            if(!row.descriptor08){
                pc=0x278e18;auto descriptor=std::make_unique<NativeSoulTreeSoundDescriptor278C00>();
                pc=0x278e30;row.descriptor08=std::move(descriptor);
            }
            auto& d=*row.descriptor08;d.actualSource=block;
            pc=0x2787e0;
            if(!findUniqueField(*block,"soundresourcefile",field)||!field||field->value.empty()||
               field->value.size()+4u>=0x104u)return false;
            d.resource00=field->value;pc=0x278800;d.resource00+=".wav"; //Name only, no lookup or playback.
            pc=0x27881c;
            if(!findUniqueField(*block,"position",field)||!field)return false;
            std::array<float,3> xyz{};std::size_t at=0;
            for(std::size_t axis=0;axis<3;++axis){
                const auto comma=field->value.find(',',at);
                if((axis<2&&comma==std::string::npos)||(axis==2&&comma!=std::string::npos))return false;
                const auto end=axis<2?comma:field->value.size();
                if(!raSoundExactInteger(field->value,at,end-at,true,xyz[axis]))return false;at=end+1;
            }
            d.position110=xyz; //158C00 storesXYZ only. Unknown globalW11C survives.
            pc=0x278834;if(!raSoundBool(*block,"looping",d.looping128))return false;
            pc=0x27884c;if(!raSoundBool(*block,"oneshot",d.oneShot12C))return false;
            pc=0x278864;if(!raSoundBool(*block,"resetwheninaudible",d.reset130))return false;
            pc=0x27887c;if(!findUniqueField(*block,"force2d",field)||field)return false;d.force2D134=0;
            pc=0x278894;if(!findUniqueField(*block,"stereo",field)||field)return false;d.stereo138=0;
            pc=0x2788b0;if(!raSoundScalar(*block,"oneshotdistance",20.f,d.oneShotDistance13C))return false;
            pc=0x2788cc;if(!raSoundScalar(*block,"randomtriggerpercent",100.f,d.randomTriggerPercent140))return false;
            pc=0x2788e4;if(!raSoundScalar(*block,"maxdistance",0.f,d.max120))return false;
            pc=0x2788fc;if(!raSoundScalar(*block,"mindistance",0.f,d.min124))return false;
            pc=0x278914;if(!findUniqueField(*block,"priority",field)||field)return false;d.priority1C0=10;
            const auto bits=[](float value)noexcept {std::uint32_t word;std::memcpy(&word,&value,4);return word;};
            pc=0x278924;
            for(std::size_t axis=0;axis<3;++axis)d.state150[4+axis]=bits(xyz[axis]);
            d.state150[7]=d.positionW11C; //Full128-bit copy propagates unknownW, not zero.
            pc=0x27892c;d.state150[22]=bits(d.min124); //1A8
            pc=0x278934;d.state150[23]=bits(d.max120); //1AC
            pc=0x278d44;if(!findUniqueField(*block,"attachtoobject",field)||field)return false;
            d.attach1D0="body";d.loaded=true;++l.loaded;
        }
        l.complete=true;pc=0x271530;return false; //279A00true; next MaterialCollisionSound list not yet reached.
    }catch(...){return false;} //Source-order counted/list/array/descriptors retained for retry or explicit retirement.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::canRetireNativeMaterialSounds3BC()const noexcept {
    if(sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached)return preflightSourceWorldMaterialSoundsNative();
    for(std::size_t kind=0;kind<2;++kind){
        if(!nativeMaterialSounds3BC_[kind])continue;
        if(!worldApplication_||worldApplication_->attachmentLock423F58()||!audio_||!audio_->decoded_)return false;
        const auto& l=*nativeMaterialSounds3BC_[kind];const auto& e=l.entity;
        if(!l.counted||l.drive!=(kind==1)||l.actualOwner!=audio_.get()||l.actualCatalog!=&audio_->materials_||
           (l.count30!=0&&l.count30!=12)||l.loaded>12||(l.rows34&&l.count30!=12)||
           (l.loaded&&!l.rows34)||(l.complete&&l.loaded!=12)||
           e.kind!=g::OriginalWreckComponentKind50LA::entity141300||e.reference00!=1||
           e.objectKey!=key(&e)||e.sessionGeneration!=worldEntity_.sessionGeneration||e.category28!=0xff||
           (e.flags29&15u)!=3u||e.nativeFlags29||e.parent1C||e.firstChild18||e.previous10||e.next14||
           e.mask20||e.descendants24||!worldApplication_->objectCount423F60()||
           !canRetireNativeSoundListCustody(e))return false;
        if(l.rows34)for(std::size_t i=0;i<12;++i){const auto& row=l.rows34[i];
            const auto* name=kind?&audio_->materials_.drive[i]:&audio_->materials_.collision[i];
            if(row.actualCatalogName&&row.actualCatalogName!=name)return false;
            if(row.actualDrive&&(!kind||row.actualDrive!=&audio_->materials_.parameters.drive[i]))return false;
            if(i<l.loaded&&(!row.loaded||!row.actualSource||row.actualCatalogName!=name||
               (kind&&!row.actualDrive)))return false;
        }
    }
    return true;
}
bool Owner::retireNativeMaterialSounds3BC()noexcept {
    if(sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached){std::uint32_t pc=0;return abortSourceWorldMaterialSoundsNative(pc);}
    if(!canRetireNativeMaterialSounds3BC())return false;
    for(auto& owner:nativeMaterialSounds3BC_){if(!owner)continue;auto& l=*owner;
        //279AE0/279DD0 release rowarray then1413E0 then list. Native strings
        //are descriptor storage only, not audio handles or counted child objects.
        l.rows34.reset();
        retireNativeSoundListCustody(l.entity);
        --l.entity.reference00;--worldApplication_->objectCount423F60();l.counted=false;owner.reset();
    }
    return true;
}
bool Owner::loadNativeMaterialSoundDescriptors271530(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    if(sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached||
       (nativeMaterialSounds3BC_[0]&&nativeMaterialSounds3BC_[0]->sourceMode)||
       (nativeMaterialSounds3BC_[1]&&nativeMaterialSounds3BC_[1]->sourceMode))return false;
    if(sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached||
       (nativeSoulTreeSounds3B8_&&nativeSoulTreeSounds3B8_->sourceMode))return false;
    pc=0x271530;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeSoulTreeSounds3B8_||!nativeSoulTreeSounds3B8_->complete||!nativeAub38_||
       !audio_||!audio_->decoded_||nativeAub38_->authoring!=audio_.get()||
       nativeAub38_->document!=&audio_->aub_||audio_->source_!=source_||
       !nativeSceneInfo270900Current(app,pc)||!worldColdConstructed_||!worldApplication_||
       worldApplication_->attachmentLock423F58())return false;
    try {
        const auto& catalog=audio_->materials_;const auto& document=*nativeAub38_->document;
        if(catalog.parameters.rowCount!=12||catalog.parameters.driveRowCount!=12)return false;
        for(std::size_t kind=0;kind<2;++kind){const bool drive=kind==1;
            pc=drive?0x271588:0x271530;
            if(!nativeMaterialSounds3BC_[kind]){
                auto list=std::make_unique<NativeMaterialSoundList>();
                list->drive=drive;list->sourceVtable08=drive?0x3d2780:0x3d2800;
                if(!reserveNativeSoundListCustody()||!canEnrollNativeSoundListCustody(list->entity))return false;
                pc=drive?0x279d9c:0x279aac;
                constructNativeSoundListCustody(list->entity);
                list->counted=true;list->actualOwner=audio_.get();list->actualCatalog=&catalog;
                pc=drive?0x2715a4:0x27154c;nativeMaterialSounds3BC_[kind]=std::move(list);
            }
            if(!canRetireNativeMaterialSounds3BC())return false;
            auto& l=*nativeMaterialSounds3BC_[kind];if(l.complete)continue;
            const Block* block=nullptr;const Field* field=nullptr;
            pc=drive?0x279e78:0x279b88;
            if(!findUniqueBlock(document,"sounds",block)||!block)return false;
            pc=drive?0x279e90:0x279ba0;
            if(!findUniqueField(*block,drive?"nummaterialdriveonsounds":"nummaterialcollisionsounds",field)||
               !field||field->value!="12")return false;
            l.count30=12;
            if(!l.rows34){
                pc=drive?0x279eb8:0x279bd0;
                auto rows=std::make_unique<NativeMaterialSoundDescriptor[]>(12);
                //Guest278650 common1D0 rows versus278950 derived1F0 drive rows.
                //Typed native array/header adaptation, constructors precede publication.
                if(drive)for(std::size_t i=0;i<12;++i)rows[i].sourceVtable1C4=0x3d2a80;
                pc=drive?0x279edc:0x279bf4;l.rows34=std::move(rows);
            }
            while(l.loaded<12){const auto i=l.loaded;
                pc=drive?0x279f04:0x279c1c;
                const auto name=std::string(drive?"materialdriveonsound":"materialcollisionsound")+std::to_string(i+1);
                if(!findUniqueBlock(document,name.c_str(),block)||!block||
                   (drive?catalog.driveSectionKeys[i]:catalog.collisionSectionKeys[i])!=i+1)return false;
                auto& d=l.rows34[i];d.actualSource=block;
                pc=0x2787e0;
                if(!findUniqueField(*block,"soundresourcefile",field)||!field||field->value.empty()||
                   field->value.size()+4u>=0x104u)return false;
                //Join authentic source identity to SAME existing native catalog;
                //catalog uses slash-normalized.vag while original descriptor uses.wav.
                auto nativeName=field->value;for(auto& c:nativeName)if(c=='\\')c='/';nativeName+=".vag";
                const auto& actualName=drive?catalog.drive[i]:catalog.collision[i];
                if(nativeName!=actualName)return false;d.actualCatalogName=&actualName;
                d.resource00=field->value;pc=0x278800;d.resource00+=".wav";
                //All24 actual rows omit common parameters. Absence is checked
                //on each exact retained block, never supplied as a zero substitute.
                pc=0x27881c;if(!findUniqueField(*block,"position",field)||field)return false;
                //DefaultXYZ from unknown current442F80; existingW also unknown.
                pc=0x278834;if(!findUniqueField(*block,"looping",field)||field)return false;d.looping128=0;
                pc=0x27884c;if(!findUniqueField(*block,"oneshot",field)||field)return false;d.oneShot12C=0;
                pc=0x278864;if(!findUniqueField(*block,"resetwheninaudible",field)||field)return false;d.reset130=0;
                pc=0x27887c;if(!findUniqueField(*block,"force2d",field)||field)return false;d.force2D134=0;
                pc=0x278894;if(!findUniqueField(*block,"stereo",field)||field)return false;d.stereo138=0;
                pc=0x2788b0;if(!findUniqueField(*block,"oneshotdistance",field)||field)return false;d.oneShotDistance13C=20.f;
                pc=0x2788cc;if(!findUniqueField(*block,"randomtriggerpercent",field)||field)return false;d.randomTriggerPercent140=100.f;
                pc=0x2788e4;if(!findUniqueField(*block,"maxdistance",field)||field)return false;d.max120=0;
                pc=0x2788fc;if(!findUniqueField(*block,"mindistance",field)||field)return false;d.min124=0;
                pc=0x278914;if(!findUniqueField(*block,"priority",field)||field)return false;d.priority1C0=10;
                pc=0x278924;for(std::size_t axis=0;axis<4;++axis)d.state150[4+axis]=d.position110[axis];
                pc=0x27892c;d.state150[22]=0u;pc=0x278934;d.state150[23]=0u;
                if(drive){
                    const auto& actual=catalog.parameters.drive[i];
                    constexpr const char* keys[5]={"materialsamplespeed","materialminvolume","materialminspeed",
                        "materialmaxspeed","materialresonantfreqspeed"};
                    constexpr const char* texts[5]={"100","0","1","60","100"};
                    constexpr std::int32_t values[5]={100,0,1,60,100};
                    constexpr std::uint32_t sites[5]={0x2789dc,0x2789f4,0x278a0c,0x278a24,0x278a3c};
                    const std::int32_t actualValues[5]={actual.sampleSpeed,actual.minVolume,actual.minSpeed,
                        actual.maxSpeed,actual.resonantSpeed};
                    for(std::size_t n=0;n<5;++n){pc=sites[n];
                        if(!findUniqueField(*block,keys[n],field)||!field||field->value!=texts[n]||
                           actualValues[n]!=values[n])return false;d.drive1D0[n]=actualValues[n];
                    }
                    pc=0x278a54;
                    if(!findUniqueField(*block,"materialfreqslope",field)||!field||field->value!="0.75"||
                       actual.frequencySlope!=0.75f)return false;
                    d.slope1E4=actual.frequencySlope;d.actualDrive=&actual;
                    //Actual catalog basePitch is separate native provider data,
                    //not part of these six source fields and is never copied.
                }
                d.loaded=true;++l.loaded;
            }
            l.complete=true; //Each original reader independently returns1.
        }
        pc=0x2715e4;return false; //BEFORE virtual+F0=271600 material assignments.
    }catch(...){return false;} //Retain source-order counted/list/array prefix for retry/retirement.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::completeNativeAudio2728E0(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    if(sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached||
       (nativeMaterialSounds3BC_[0]&&nativeMaterialSounds3BC_[0]->sourceMode)||
       (nativeMaterialSounds3BC_[1]&&nativeMaterialSounds3BC_[1]->sourceMode))return false;
    if(sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached||
       (nativeSoulTreeSounds3B8_&&nativeSoulTreeSounds3B8_->sourceMode))return false;
    pc=0x2715e4;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||nativeAubRoleRetired272930_||
       !nativeAub38_||!audio_||!audio_->decoded_||nativeAub38_->authoring!=audio_.get()||
       nativeAub38_->document!=&audio_->aub_||!nativeSceneInfo270900Current(app,pc)||
       !nativeMaterialSounds3BC_[0]||!nativeMaterialSounds3BC_[0]->complete||
       !nativeMaterialSounds3BC_[1]||!nativeMaterialSounds3BC_[1]->complete)return false;
    if(!nativeMaterialAssignments271600_)nativeMaterialAssignments271600_.emplace();
    auto& locals=*nativeMaterialAssignments271600_;
    if(!locals.complete){
        const auto& document=*nativeAub38_->document;const Block* sounds=nullptr;const Block* list=nullptr;
        const Field* field=nullptr;
        pc=0x27163c;if(!findUniqueBlock(document,"sounds",sounds)||!sounds)return false;
        pc=0x271654;
        if(!findUniqueField(*sounds,"nummaterials",field)||!field||field->value!="1"||
           audio_->materials_.materialCount!=1)return false;
        locals.materialCount284=1;
        pc=0x27166c;
        if(!findUniqueField(*sounds,"defaultmaterial",field)||!field||field->value!="1"||
           audio_->materials_.defaultMaterial!=1)return false;
        locals.defaultMaterial288=1;
        //Outer1<=NumMaterials enters exactly material1. Failed157D00 keeps
        //the actual previously selected Sounds; do not assume a fresh/null block.
        pc=0x271694;
        if(!findUniqueBlock(document,"collisionobjectsmaterial1",list)||list)return false;
        pc=0x2716ac;
        if(!findUniqueField(*sounds,"numcollisionobjects",field)||field||
           !audio_->materials_.orderedObjectMaterials.empty())return false;
        locals.objectCount28C=0;
        //Inner1<objectCount is false. No descriptor184 read/write, including
        //DefaultMaterial fallback. Out-of-range authoredMaterial2 is untouched.
        //Outer increments2 then2>1 exits271808. No candidate enumeration needed.
        locals.complete=true;
    }
    //271828 returns+F0;2715F4 returns+EC;272918 writes true then272920 returns+E8.
    pc=0x1aa128;return true;
}
bool Owner::withdrawNativeAub38_272930(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa128;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeMaterialAssignments271600_||!nativeMaterialAssignments271600_->complete||
       !nativeSceneInfo270900Current(app,pc)||nativeScb34_||nativeTob34_)return false;
    if(nativeAubRoleRetired272930_){if(nativeAub38_)return false;pc=0x1aa13c;return true;}
    if(!nativeAub38_||!audio_||!audio_->decoded_||nativeAub38_->authoring!=audio_.get()||
       nativeAub38_->document!=&audio_->aub_||nativeAub38_->source!=source_||
       nativeAub38_->provider!=rosterMountDomain_->archiveProvider3E1A70())return false;
    pc=0x27294c;nativeAub38_.reset(); //1573B0 destruction of native reader ROLE only.
    //Exact source272954 is sw zero,34(s0), NOT38. Old World34 already absent;
    //native role invalidation prevents dangling use, but is NOT a claim that
    //retail cleared38 or that immutable audio document backing was destroyed.
    nativeAubRoleRetired272930_=true;pc=0x1aa13c;return true;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueNativeZoneServices2729E0(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa13c;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!nativeAubRoleRetired272930_||
       nativeAub38_||!nativeSceneInfo270900Current(app,pc)||!source_||
       !nativeSceneRoles270900Current(*source_,pc)||!lighting_||
       !nativeSceneInfo270900_||nativeSceneInfo270900_->authoredZones34C!=10||
       sceneApplication_!=worldApplication_||sceneGeneration_!=worldEntity_.sessionGeneration)return false;
    //Explicit native already-populated topology adoption replaces destructive
    //272A14/1C and4C0/4C4 reallocation. SAME real counted container and twenty
    //zone nodes/backing aliases; never duplicate, reset membership or recount.
    //Their native construction already ran; do NOT call that a fresh retail
    //2729E0 execution or advance past later zone-method services yet.
    if(container4BC_.kind!=g::OriginalWreckComponentKind50LA::entity141300||
       !container4BC_.reference00||!topologyConstructed_||topologyClosed_||!canCloseTopology())return false;
    for(std::size_t i=0;i<10;++i){const auto& visible=visibility4C0_[i];const auto& collision=collision4C4_[i];
        if(!visible.constructed||!visible.attached||!collision.constructed||!collision.attached||
           visible.collectionBacking!=&visibilityRows_[i]||collision.collectionBacking!=&collisionRows_[i]||
           visible.ordinal48!=i||collision.ordinal48!=i)return false;
    }
    nativeZoneTopologyPrefix272A48_=true;
    using Manager=rr::platform::OriginalUiLightManagerOwner50KG;Manager::Diagnostic diagnostic;
    pc=0x272a48;
    if(!Manager::bindRaWorldZones272A48(app,*this,lighting_,nativeRaLightZones_,diagnostic)){
        pc=diagnostic.sourcePC;return false;
    }
    if(!continueNativeZoneExclusions272A58(*source_,pc))return false;
    while(nativePortalNext_<nativePortals_.size()){
        const auto ordinal=nativePortalNext_;
        if(!continueNativePortal230450(*source_,ordinal,pc)||
           !continueNativePortalWorldLinks272B80(ordinal,pc))return false;
        auto& portal=*nativePortals_[ordinal];
        if(portal.worldLinksPublished!=4u||!canCloseTopology())return false;
        const auto& zones=portal.authored->zones;
        if(!nativeRaLightZones_->linkRaPortal148D00(*this,ordinal,portal.entity,container4BC_,
            zones[0]-1u,zones[1]-1u,diagnostic)){pc=diagnostic.sourcePC;return false;}
        portal.lightLinksComplete=true;++nativePortalNext_; //272C60/68/6C/70.
    }
    if(!continueNativeMethodArrays272C78(*source_,pc))return false;
    while(nativeMethodZoneNext_<10u){const auto z=nativeMethodZoneNext_;
        if(!prepareNativeMethodRow272E80(z,pc)||
           !continueNativeVisibilityList(*source_,z*2u,pc)||
           !continueNativeCollisionList(*source_,z*2u,pc)||
           !continueNativeVisibilityList(*source_,z*2u+1u,pc)||
           !continueNativeCollisionList(*source_,z*2u+1u,pc))return false;
        ++nativeMethodZoneNext_; //272F14, only AFTER both actual method calls.
    }
    if(!continueNativeEnvironmentAliases272FC0(*source_,pc))return false;
    if(!continueNativeDefaultObserver272F48(pc))return false;
    //272F74's v0=1 and272FAC epilogue complete the represented native-adapted
    //+FC route. This is NOT proof of unrepresented270900 or full1A9EE0.
    if(!adoptNativeLightWrappers273AA0(*source_,pc))return false;
    if(!nativeRaLightZones_||!continueNativeAmbientLight(*this,*nativeRaLightZones_,pc))return false;
    if(!continueNativeDirectionalLight(*this,*source_,*nativeRaLightZones_,pc))return false;
    //The exact same source Root material service/context, not a map capture or
    //SunFlare ScreenMT substitution.14AB30 uses DefaultMT through150450.
    if(!world_||world_->domain_!=rosterMountDomain_||!world_->context_||!world_->context_->live()||
       !rosterMountDomain_||app.standardMaterialDomain()!=rosterMountDomain_)return false;
    const auto context=world_->context_;const auto table=rosterMountDomain_->materials382B38();
    if(!table||!context->materialApplication()||!table->screenApplicationMatches(*context->materialApplication())||
       !context->applicationAnimationClock().sameOwner(app.animationClock()))return false;
    while(nativePointNext_<nativePoints_.size()){
        const auto index=nativePointNext_;
        if(!continueNativeLocalPrimary(index,*this,*source_,*nativeRaLightZones_,pc)||
           !continueNativeLocalFlare(index,table,app.materialDirectory36B970(),pc)||
           !continueNativeLocalFlareObserver(index,pc)||!completeNativeLocalPoint(index,pc))return false;
        ++nativePointNext_; //Only after actual source row attachment273B98.
    }
    while(nativeRemainderNext_<nativeRemainder_.size()){
        if(!continueNativeRemainingLight(nativeRemainderNext_,*this,*source_,*nativeRaLightZones_,pc))return false;
        ++nativeRemainderNext_; //Source order includes Ambient173..175.
    }
    if(!nativeLightList3A8_||nativeLightList3A8_->colorsLoaded!=10u)return false;
    while(nativeDirectionalColorNext_<10u){const auto zone=nativeDirectionalColorNext_;
        const auto& color=nativeLightList3A8_->colors3C[zone];
        pc=0x273bdc;if(!color.lanes[0])return false;
        //Source273BE0 uses x<-255, including authored-256 sentinel.
        if(!(*color.lanes[0]<-255.f)){
            rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic detail;
            if(!nativeRaLightZones_->applyRaDirectionalColor1498A0(*this,zone,detail)){pc=detail.sourcePC;return false;}
        }
        ++nativeDirectionalColorNext_;
    }
    //273C24 return1 closes the represented native-adapted+10C route only.
    return continueNativeFog273C30(app,pc); //No fullWorld claim.

}
bool Owner::continueNativeFog273C30(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa158;
    if(nativeFogDescriptor3AC_&&nativeFogDescriptor3AC_->sourceMode)return false;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!source_||
       !nativeSceneInfo270900Current(app,pc)||nativeDirectionalColorNext_!=10u||
       nativeRemainderNext_!=168u||!runtimeQueries_||!worldApplication_||
       runtimeCameraBorrows_||runtimeWorldBorrows_)return false;
    g::OriginalRaceFogFields* actual=nullptr;
    pc=0x273c30;
    if(!runtimeQueries_->readNativeColdFog273C30(*worldApplication_,*source_,
        worldEntity_.sessionGeneration,actual)||!actual)return false;
    if(nativeFogLoadComplete_){
        if(nativeFogDescriptor3AC_||!nativeFogCustodyCurrent(actual))return false;
        return continueNativeParticleManager1B4DC0(app,pc);
    }
    if(nativeFogLoadAttempted_)return false; //Failed partial effects never replay.
    const auto* decoded=source_->sourcesDecoded();if(!decoded)return false;
    const auto& doc=decoded->world.parameters;
    const rr::assets::ParameterBlock48F* block=nullptr;
    for(const auto& candidate:doc.blocks)if(candidate.name=="fog"){
        if(block)return false;block=&candidate;
    }
    if(doc.original_bytes.size()!=540795u||!block||block->offset!=38500u||block->fields.size()!=3u)return false;
    const auto& c=block->fields[0];const auto& h=block->fields[1];const auto& v=block->fields[2];
    if(c.name!="colorrgb"||c.value!="180,180,165"||c.value_offset!=38633u||
       h.name!="haziness"||h.value!=".5"||h.value_offset!=38648u||
       v.name!="visibility"||v.value!=".9"||v.value_offset!=38645u)return false;
    // Actual authored Fog is present. This is a new native immutable-authoring
    // custody, NOT a receipt for earlier270B10/278570 or a null3AC reset.
    nativeFogLoadAttempted_=true;
    try {
        nativeFogDescriptor3AC_=std::make_unique<NativeFogDescriptor3AC>();
        nativeFogDescriptor3AC_->source=source_;nativeFogDescriptor3AC_->block=block;
        // Actual native color already comes from these integral180/180/165/255
        // lanes; their source CVT.W.S outputs are exact. Existing range values
        // were computed before Session allocation and restored by coldViewport.
        // Adopt them, never replay arithmetic or claim original FCSR semantics.
        pc=0x273cac;
        if(!constructNativeFog273CAC(*actual,pc)||!allocateNativeFogBuffer13E780(pc))return false;
        pc=0x13e99c;
        if(!runtimeQueries_->publishNativeColdFog13E99C(*worldApplication_,*source_,
            worldEntity_.sessionGeneration,*actual))return false;
        if(!initializeNativeFogOff13F890(pc)||!bindNativeFogOn13F9C0(pc))return false;
        //273F70/7C withdraws this actual native descriptor role only. No other
        //authenticated source bytes, native fog owner or camera state retires.
        pc=0x273f70;nativeFogDescriptor3AC_.reset();nativeFogLoadComplete_=true;
        return continueNativeParticleManager1B4DC0(app,pc); //273F94 return1; continue actual+114.
    }catch(...){return false;} //All reached allocations/counts remain owned.
}
bool Owner::continueNativeParticleManager1B4DC0(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    try {
    pc=0x1aa180;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!source_||
       !nativeFogLoadComplete_||!nativeSceneInfo270900Current(app,pc)||!runtimeQueries_||
       !runtimeQueries_->camera_||runtimeQueries_->camera_->particles!=trackParticles_||
       runtimeQueries_->camera_->generation!=trackParticleGeneration_||
       runtimeCameraBorrows_||runtimeWorldBorrows_||!particles_||particles_->source_!=source_||
       trackParticleHostApplication_!=&app||trackParticleApplication_!=worldApplication_||
       trackParticleGeneration_!=worldEntity_.sessionGeneration||!trackParticleDomain_||
       trackParticleDomain_!=app.standardMaterialDomain()||!world_||world_->domain_!=trackParticleDomain_||
       !world_->context_||!world_->context_->live()||!trackParticles_||!trackParticles_->prepared()||
       trackParticles_->material150()->table382B38()!=trackParticleDomain_->materials382B38().get())return false;
    g::OriginalRaceFogFields* actualFog=nullptr;
    if(!runtimeQueries_->readNativeColdFog273C30(*worldApplication_,*source_,
        worldEntity_.sessionGeneration,actualFog)||!nativeFogCustodyCurrent(actualFog))return false;
    const auto* prt=particles_->originalMember(2);if(!prt)return false;
    if(!completeNativeParticleManager1B4DC0(*source_,*prt,pc))return false;
    //1B4E74's successful pointer result follows the represented native-adapted
    //160E00 route, not prepared()==return. Caller resumes at1AA194, before
    //the actual Application330/+30 control branch and World+118.
    return continueNativeInstances1AA194(app,pc);
    }catch(...){return false;} //Retain reached ownership; never rely on string SSO.
}
bool Owner::continueNativeInstances1AA194(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa194;
    try {
        if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||!source_||
           !nativeParticleManagerComplete_||nativeParticleManager4A0_!=trackParticles_.get()||
           !nativeSceneInfo270900Current(app,pc)||!app.state||!runtimeQueries_||
           runtimeCameraBorrows_||runtimeWorldBorrows_)return false;
        g::OriginalRaceFogFields* fog=nullptr;
        if(!runtimeQueries_->readNativeColdFog273C30(*worldApplication_,*source_,
            worldEntity_.sessionGeneration,fog)||!nativeFogCustodyCurrent(fog))return false;
        if(!nativeGeometryDirectory1AA1DC_){
            //Same explicit native bounded-progress policy as the earlier
            //1A9EE0 prefix replaces Application330. Neither unknown330/+30
            //nor source1CC900 execution is invented. Real directory store
            //below DOES target the existing Application namespace owner.
            pc=0x1aa1c0;app.modelDirectory36B860()="geometry\\";
            nativeGeometryDirectory1AA1DC_=true;
        }else if(app.modelDirectory36B860()!=(nativeCollisionAnimationDirectory1AA2A0_?"anim\\":"geometry\\"))return false;
        if(!completeNativeEmptyInstances274080(*source_,app.state,pc))return false;
        return continueNativeStandardModels274160(app,pc);
    }catch(...){return false;} //Retain actual directory/owner effects; no rollback or reset.
}
bool Owner::retireNativeRaLightZones148110()noexcept {
    if(!nativeRaLightZones_)return true;
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic diagnostic;
    for(std::size_t i=nativeRemainder_.size();i>0;--i)
        if(!retireNativeRemainingLight(i-1u,*this,*nativeRaLightZones_))return false;
    nativeRemainderNext_=0;nativeDirectionalColorNext_=0;
    //Native partial-load retirement in reverse authored order preserves the
    //source149214 decrement-only registry prefix; no arbitrary compaction.
    for(std::size_t i=nativePoints_.size();i>0;--i){
        if(!retireNativeLocalFlare(i-1u)||!retireNativeLocalPrimary(i-1u,*this,*nativeRaLightZones_))return false;
    }
    nativePointNext_=0;
    if(!retireNativeDirectionalLight(*this,*nativeRaLightZones_))return false;
    if(!retireNativeAmbientLight(*this,*nativeRaLightZones_))return false;
    if(!nativeRaLightZones_->releaseRaZones148110(*this,diagnostic))return false;
    nativeRaLightZones_.reset();return true;
}
}

namespace ra_pod_scene50LA {
bool Owner::continueNativeZoneExclusions272A58(const ra_cold_sources50LA::Owner& source,
    std::uint32_t& pc)noexcept {
    pc=0x272a58;
    if(!nativeSceneRoles270900Current(source,pc)||!lighting_)return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
    try{
        while(nativeZoneExclusionsNext_<10u){const auto z=nativeZoneExclusionsNext_;
            const auto* actual=lighting_->nativeRaZoneExclusions272B08(this,decoded->world,visibilityRows_,z,pc);
            if(!actual||(actual->exclusionCount!=6u&&actual->exclusionCount!=7u))return false;
            const auto& sourceZone=decoded->world.zones[z];
            if(sourceZone.zone!=z+1u||sourceZone.parameter_block>=decoded->world.parameters.blocks.size())return false;
            const auto& block=decoded->world.parameters.blocks[sourceZone.parameter_block];
            const Field* field=nullptr;
            if(block.name!=std::string("zone")+std::to_string(z+1u)||
               !findUniqueField(block,"exclusionlist",field)||!field||field->value.empty()||field->value.size()>=128u)return false;
            //Exact retained source labels converted by2763E8/3F4 to zero-based
            //ordinals. Validate SAME actual canonical ordered metadata, no sort
            //or deduplication and no separate runtime visibility collection.
            std::size_t at=0,count=0;
            while(at<field->value.size()){
                std::uint32_t label=0;const auto start=at;
                while(at<field->value.size()&&field->value[at]>='0'&&field->value[at]<='9'){
                    label=label*10u+std::uint32_t(field->value[at++]-'0');if(label>10u)return false;
                }
                if(at==start||!label||count>=actual->exclusionCount||actual->exclusions[count]!=label-1u)return false;
                ++count;if(at==field->value.size())break;
                if(field->value[at++]!=','||at==field->value.size())return false;
            }
            if(count!=actual->exclusionCount)return false;
            auto& node=visibility4C0_[z];
            if(node.exclusionSource&&node.exclusionSource!=actual)return false;
            //Existing native constructors/ordinal stores were adopted at+FC
            //prefix. Only this newly reached3C/40 source-owned allocation is new.
            node.exclusionSource=actual;
            pc=0x27f538;node.exclusionCount40=actual->exclusionCount;
            if(!node.exclusions3C){
                pc=0x27f534;auto rows=std::unique_ptr<std::uint32_t[]>(new std::uint32_t[actual->exclusionCount]);
                pc=0x27f540;node.exclusions3C=std::move(rows);
            }
            //27F550..570 exact ordered DWORD copy. No fallible operation after
            //publication; source exclusion array never replaces collectionBacking.
            for(std::size_t i=0;i<actual->exclusionCount;++i)node.exclusions3C[i]=actual->exclusions[i];
            node.exclusionsComplete=true;++nativeZoneExclusionsNext_;
        }
        pc=0x272b3c;return true;
    }catch(...){return false;} //Known40 may precede failed allocation; retain honest partial metadata.
}
}

namespace ra_pod_scene50LA {
bool Owner::canRetireNativePortals()const noexcept {
    if(sourceTopologyMode_)return false; //Dedicated source Portal custody/preflight owns this family.
    if(nativePortalNext_>nativePortals_.size())return false;
    bool gap=false;
    for(std::size_t i=0;i<nativePortals_.size();++i){const auto& owner=nativePortals_[i];
        if(!owner){if(i<nativePortalNext_)return false;gap=true;continue;}
        if(gap||i>nativePortalNext_||!sceneApplication_||sceneApplication_->attachmentLock423F58())return false;
        const auto& p=*owner;const auto& e=p.entity;
        if(!p.counted||e.reference00!=1||e.kind!=g::OriginalWreckComponentKind50LA::entity141300||
           e.objectKey!=key(&e)||e.sessionGeneration!=sceneGeneration_||e.category28!=0xff||
           (e.flags29&15u)!=3u||e.nativeFlags29||e.firstChild18||e.mask20||e.descendants24||
           (p.attached?(e.parent1C!=&container4BC_||nativeTopologyAuthorityEnrolled_!=domainCount_):
                       (e.parent1C||e.previous10||e.next14))||
           !sceneApplication_->objectCount423F60()||!sceneApplication_->entityAuthority_.canRetire(e))return false;
        for(const auto& w:p.vertexW)if(w)return false;
        if(p.loaded&&(!p.geometry||!p.authored||!p.viewport434940||!p.maximumDiagonalC0))return false;
        if(p.lightLinksComplete&&(!p.attached||p.worldLinksPublished!=4u))return false;
        if(i<nativePortalNext_&&!p.lightLinksComplete)return false;
    }
    return true;
}
void Owner::retireNativePortal(std::size_t ordinal)noexcept {
    auto& owner=nativePortals_[ordinal];if(!owner)return;
    auto& p=*owner;
    if(p.sourceMode)std::terminate(); //Never retire source authority through the legacy sceneApplication lane.
    if(p.attached||p.worldLinksPublished||p.entity.parent1C||p.entity.previous10||p.entity.next14||
       !sceneApplication_->entityAuthority_.retire(p.entity))std::terminate();
    --p.entity.reference00;--sceneApplication_->objectCount423F60(); //2303F0->1413E0.
    p.geometry=nullptr;p.authored=nullptr;p.viewport434940=nullptr;owner.reset();
}
bool Owner::continueNativePortal230450(const ra_cold_sources50LA::Owner& source,
    std::size_t ordinal,std::uint32_t& pc)noexcept {
    if(sourceTopologyMode_||(nativePortalParameters3A0_&&nativePortalParameters3A0_->sourceMode))return false;
    if(!nativeSceneRoles270900Current(source,pc)||nativeZoneExclusionsNext_!=10u||
       ordinal!=nativePortalNext_||ordinal>=nativePortals_.size()||!canRetireNativePortals())return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
    const auto& world=decoded->world;
    pc=0x272b28;if(world.portals.size()!=nativePortals_.size())return false;
    //Genuine first native join of immutable decoded portal parameters. This
    //does NOT backfill a receipt for original270C64 allocation/reset. The
    //role alone retires at272C78; actual immutable backing remains owned.
    if(!nativePortalParameters3A0_){
        if(ordinal)return false;
        nativePortalParameters3A0_.emplace(NativePortalParameterRole3A0{world.portals.data(),world.portals.size(),false});
    }
    if(nativePortalParameters3A0_->retired||nativePortalParameters3A0_->rows!=world.portals.data()||
       nativePortalParameters3A0_->count!=world.portals.size())return false;
    const auto* geometry=lighting_->nativeRaPortal230450(this,world,visibilityRows_,sceneApplication_.get(),ordinal,pc);
    if(!geometry)return false;
    const auto& authored=world.portals[ordinal];
    if(!authored.zones[0]||authored.zones[0]>10u||!authored.zones[1]||authored.zones[1]>10u||
       authored.zones[0]==authored.zones[1])return false;
    try {
        auto& owner=nativePortals_[ordinal];
        if(!owner){
            pc=0x272b3c;auto portal=std::make_unique<NativePortal230370>();
            auto& count=sceneApplication_->objectCount423F60();auto& authority=sceneApplication_->entityAuthority_;
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
               !authority.canEnroll(portal->entity,key(&portal->entity)))return false;
            pc=0x23037c;
            if(g::initializeOriginalWreckComponent50LA(portal->entity,g::OriginalWreckComponentKind50LA::entity141300,
                sceneGeneration_,key(&portal->entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
                !authority.enroll(portal->entity))std::terminate();
            portal->counted=true;owner=std::move(portal);
        }
        auto& p=*owner;
        if(p.loaded){if(p.geometry!=geometry||p.authored!=&authored)return false;pc=0x272b80;return true;}
        //Adopt SAME actual earlier-prepared visibility geometry/quad, no replay.
        //Source full-vector copy has unknown W; scalar math consumes XYZ only.
        pc=0x230468;p.geometry=geometry;p.authored=&authored;
        pc=0x230484;p.viewport434940=&sceneApplication_->portalViewport434940_;
        pc=0x230558;float diagonal=0;
        if(g::portalMaximumDiagonal230580_50EO(*geometry,diagonal)!=g::PortalGeometryStatus50EO::Complete)return false;
        p.maximumDiagonalC0=diagonal;p.loaded=true;pc=0x230568;return true;
    }catch(...){return false;}
}
bool Owner::canRetireNativePortalWorldLinks()const noexcept {
    if(sourceTopologyMode_)return false; //Exact source chain withdrawal has a separate ledger.
    for(const auto& owner:nativePortals_)if(owner){const auto& p=*owner;
        if(p.worldLinksPublished>4u||(p.worldLinksPublished&&(!p.attached||!p.authored)))return false;
        if(p.authored&&(!p.authored->zones[0]||p.authored->zones[0]>10u||
           !p.authored->zones[1]||p.authored->zones[1]>10u||p.authored->zones[0]==p.authored->zones[1]))return false;
    }
    //27F3F8 prepends. Validate exact reverse-source chain for EACH real node,
    //including exact portal/neighbor identity, no arbitrary extra records.
    for(std::size_t z=0;z<10u;++z)for(bool collision:{false,true}){
        const auto& node=collision?collision4C4_[z]:visibility4C0_[z];auto* link=node.portalLinks30.get();
        for(std::size_t i=nativePortals_.size();i-- >0;){const auto& owned=nativePortals_[i];if(!owned)continue;
            const auto& p=*owned;if(!p.worldLinksPublished)continue;
            const auto a=p.authored->zones[0]-1u,b=p.authored->zones[1]-1u;
            for(std::size_t stage=p.worldLinksPublished;stage-- >0;){
                const bool stageCollision=(stage&1u)!=0;const auto from=stage<2u?a:b,to=stage<2u?b:a;
                if(stageCollision!=collision||from!=z)continue;
                const auto& neighbor=collision?collision4C4_[to]:visibility4C0_[to];
                if(!link||link->neighbor04!=&neighbor.entity||link->portal08!=&p.entity||
                   link->field0C||link->field10!=0x7f7fffff)return false;
                link=link->next00.get();
            }
        }
        if(link)return false;
    }
    return !environment443108_.portalLinks30;
}
void Owner::retireNativePortalWorldLinks()noexcept {
    if(!canRetireNativePortalWorldLinks())std::terminate();
    //Native adopted portal-after-nodes order: revoke raw borrows BEFORE any
    //Portal dies, then later ordinary node exclusions/aliases. No dereference.
    for(auto& node:visibility4C0_)node.portalLinks30.reset();
    for(auto& node:collision4C4_)node.portalLinks30.reset();
    for(auto& portal:nativePortals_)if(portal)portal->worldLinksPublished=0;
}
bool Owner::continueNativePortalWorldLinks272B80(std::size_t ordinal,std::uint32_t& pc)noexcept {
    pc=0x272b80;
    if(ordinal!=nativePortalNext_||ordinal>=nativePortals_.size()||!nativePortals_[ordinal]||
       !nativePortals_[ordinal]->loaded||!canCloseTopology())return false;
    auto& p=*nativePortals_[ordinal];auto& authority=sceneApplication_->entityAuthority_;
    if(nativeTopologyAuthorityEnrolled_<domainCount_){
        //Only the FIRST attach completes missing authority of existing native
        //topology. Prove complete closed forest before enrollment; no recount.
        if(ordinal)return false;
        const std::size_t expected=1u+visibility4C0_.size()+collision4C4_.size()+
            std::size_t(environment443108_.constructed);
        if(domainCount_!=expected||container4BC_.parent1C)return false;
        const auto member=[&](const Entity* node)noexcept {
            if(!node)return true;
            for(std::size_t i=0;i<domainCount_;++i)if(domain_[i]==node)return true;
            return false;
        };
        for(std::size_t i=0;i<domainCount_;++i){const auto* node=domain_[i];
            if(!node||!member(node->parent1C)||!member(node->previous10)||
               !member(node->next14)||!member(node->firstChild18))return false;
            for(std::size_t j=0;j<i;++j)if(domain_[j]==node)return false;
        }
        if(!authority.reserveAdditional(domainCount_-nativeTopologyAuthorityEnrolled_))return false;
        while(nativeTopologyAuthorityEnrolled_<domainCount_){auto& node=*domain_[nativeTopologyAuthorityEnrolled_];
            if(!authority.canEnroll(node,key(&node))){
                while(nativeTopologyAuthorityEnrolled_)
                    if(!authority.retire(*domain_[--nativeTopologyAuthorityEnrolled_]))std::terminate();
                return false; //No partial global forest; monotonic serial stays.
            }
            if(!authority.enroll(node))std::terminate();++nativeTopologyAuthorityEnrolled_;
        }
    }
    if(!p.attached){
        const auto result=authority.attachChildren(container4BC_,p.entity,0,sceneApplication_->attachmentLock423F58());
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete){pc=result.sourcePC;return false;}
        p.attached=true;
    }
    const auto a=p.authored->zones[0]-1u,b=p.authored->zones[1]-1u;
    SearchNode* from[4]={&visibility4C0_[a],&collision4C4_[a],&visibility4C0_[b],&collision4C4_[b]};
    SearchNode* to[4]={&visibility4C0_[b],&collision4C4_[b],&visibility4C0_[a],&collision4C4_[a]};
    constexpr std::uint32_t calls[4]={0x272bc8,0x272bec,0x272c10,0x272c34};
    try{
        while(p.worldLinksPublished<4u){const auto i=p.worldLinksPublished;pc=calls[i];
            auto link=std::make_unique<NativePortalLink27F3B0>();
            link->neighbor04=&to[i]->entity;link->portal08=&p.entity;
            link->next00=std::move(from[i]->portalLinks30);from[i]->portalLinks30=std::move(link);
            ++p.worldLinksPublished;
        }
        pc=0x272c54;return true;
    }catch(...){return false;}
}
}
namespace ra_pod_scene50LA {
bool Owner::nativeMethodArraysCurrent()const noexcept {
    if(sourceTopologyMode_||(nativePortalParameters3A0_&&nativePortalParameters3A0_->sourceMode))return false;
    if(nativePortalParameters3A0_){const auto& role=*nativePortalParameters3A0_;
        if(role.count!=10u||bool(role.rows)==role.retired||(role.retired&&nativePortalNext_!=10u))return false;
        if(!role.retired&&nativePortals_[0]&&nativePortals_[0]->authored&&role.rows!=nativePortals_[0]->authored)return false;
    }
    if((nativeMethods4CC_&&!nativeMethods4C8_)||(nativeMethods4D0_&&!nativeMethods4CC_)||
       (nativeMethods4D4_&&!nativeMethods4D0_)||nativeMethodZoneNext_>10u||nativeMethodRowsInitialized_>10u||
       nativeMethodZoneNext_>nativeMethodRowsInitialized_||nativeMethodRowsInitialized_>nativeMethodZoneNext_+1u)return false;
    if(nativeMethods4C8_&&(!nativePortalParameters3A0_||!nativePortalParameters3A0_->retired))return false;
    if(nativeMethodRowsInitialized_&&!nativeMethods4D4_)return false;
    if(environment443108_.nativeList34||environment443108_.nativeList38||!nativeCollisionListsCurrent())return false;
    const auto* storage=runtimeQueries_?runtimeQueries_->storage_.get():nullptr;
    for(std::size_t z=0;z<10u;++z){
        const auto& v=visibility4C0_[z];const auto& c=collision4C4_[z];
        if(z>=nativeMethodRowsInitialized_){
            if(v.nativeList34||v.nativeList38||c.nativeList34||c.nativeList38)return false;
            continue; //Source-unwritten array slots are NEVER read.
        }
        NativeListMethodCustody* methods[4]={nativeMethods4C8_[z],nativeMethods4CC_[z],nativeMethods4D0_[z],nativeMethods4D4_[z]};
        NativeListMethodCustody* aliases[4]={v.nativeList34,c.nativeList34,v.nativeList38,c.nativeList38};
        bool previousComplete=true;
        for(std::size_t stage=0;stage<4u;++stage){const bool collision=(stage&1u)!=0;const auto index=z*2u+stage/2u;
            const auto* method=methods[stage];const auto* alias=aliases[stage];
            const bool pending=collision&&storage&&storage->raSourceLists_&&
                storage->raSourceLists_->methods[index].constructorAttempted&&!storage->raSourceLists_->methods[index].retired;
            if(!previousComplete&&(method||alias||pending))return false;
            if(!method){if(alias)return false;previousComplete=false;continue;}
            if(!collision&&(!lighting_||!lighting_->raVisibilityListCustodyCurrent(this,method)))return false;
            if(method->ordinal_!=index||!method->arrayReference_||method->nodeReference_!=bool(alias)||
               (alias&&alias!=method))return false;
            previousComplete=bool(alias);
        }
        if(z<nativeMethodZoneNext_&&!previousComplete)return false;
    }
    return true;
}
bool Owner::continueNativeMethodArrays272C78(const ra_cold_sources50LA::Owner& source,std::uint32_t& pc)noexcept {
    pc=0x272c78;
    if(nativePortalNext_!=10u||!nativeSceneRoles270900Current(source,pc)||!nativeMethodArraysCurrent()||
       !nativePortalParameters3A0_)return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded||decoded->world.zones.size()!=10u)return false;
    for(const auto& portal:nativePortals_)if(!portal||!portal->lightLinksComplete)return false;
    auto& role=*nativePortalParameters3A0_;
    if(!role.retired){
        if(role.rows!=decoded->world.portals.data()||role.count!=decoded->world.portals.size())return false;
        role.rows=nullptr;role.retired=true; //Native borrower withdrawal, NOT2E8AE0 free.
    }
    try{
        if(!nativeMethods4C8_){pc=0x272e3c;auto rows=std::unique_ptr<NativeListMethodCustody*[]>(new NativeListMethodCustody*[10]);pc=0x272e44;nativeMethods4C8_=std::move(rows);}
        if(!nativeMethods4CC_){pc=0x272e4c;auto rows=std::unique_ptr<NativeListMethodCustody*[]>(new NativeListMethodCustody*[10]);pc=0x272e54;nativeMethods4CC_=std::move(rows);}
        if(!nativeMethods4D0_){pc=0x272e5c;auto rows=std::unique_ptr<NativeListMethodCustody*[]>(new NativeListMethodCustody*[10]);pc=0x272e64;nativeMethods4D0_=std::move(rows);}
        if(!nativeMethods4D4_){pc=0x272e6c;auto rows=std::unique_ptr<NativeListMethodCustody*[]>(new NativeListMethodCustody*[10]);pc=0x272e74;nativeMethods4D4_=std::move(rows);}
        pc=0x272e80;return true;
    }catch(...){return false;}
}
bool Owner::prepareNativeMethodRow272E80(std::size_t z,std::uint32_t& pc)noexcept {
    pc=0x272e80;
    if(z!=nativeMethodZoneNext_||z>=10u||!canCloseTopology()||!nativeMethods4D4_)return false;
    if(nativeMethodRowsInitialized_==z+1u)return true;
    if(nativeMethodRowsInitialized_!=z)return false;
    const auto& v=visibility4C0_[z];const auto& c=collision4C4_[z];
    if(!v.attached||!c.attached||v.entity.parent1C!=&container4BC_||c.entity.parent1C!=&container4BC_||
       v.entity.mask20!=0x400u||c.entity.mask20)return false;
    //Adopt SAME existing native attachments; no source141960 replay/reorder.
    pc=0x272ec8;nativeMethods4C8_[z]=nullptr;
    pc=0x272ed4;nativeMethods4CC_[z]=nullptr;
    pc=0x272ee0;nativeMethods4D0_[z]=nullptr;
    pc=0x272eec;nativeMethods4D4_[z]=nullptr;
    ++nativeMethodRowsInitialized_;pc=0x272ef8;return true;
}
bool Owner::continueNativeVisibilityList(const ra_cold_sources50LA::Owner& source,std::size_t index,std::uint32_t& pc)noexcept {
    pc=(index&1u)?0x272f0c:0x272ef8;
    const auto z=index/2u;const bool dynamic=(index&1u)!=0;
    if(index>=20u||z!=nativeMethodZoneNext_||nativeMethodRowsInitialized_!=z+1u||
       !nativeSceneRoles270900Current(source,pc)||!nativeMethodArraysCurrent())return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
    auto& method=dynamic?nativeMethods4D0_[z]:nativeMethods4C8_[z];
    auto& alias=dynamic?visibility4C0_[z].nativeList38:visibility4C0_[z].nativeList34;
    if(!method){
        auto* actual=lighting_->adoptRaVisibilityList(this,decoded->world,visibilityRows_,sceneApplication_.get(),index,pc);
        if(!actual)return false;method=actual; //Actual arraypublication before nodealias.
    }
    if(!alias){if(!lighting_->retainRaVisibilityListNode(this,method,pc))return false;alias=method;}
    pc=dynamic?0x273854:0x2733c4;return true;
}
bool Owner::nativeCollisionListsCurrent()const noexcept {
    const auto* s=runtimeQueries_?runtimeQueries_->storage_.get():nullptr;
    if(!s||!s->raSourceLists_){
        for(std::size_t z=0;z<10u;++z){
            if(collision4C4_[z].nativeList34||collision4C4_[z].nativeList38)return false;
            if(z<nativeMethodRowsInitialized_&&((nativeMethods4CC_&&nativeMethods4CC_[z])||(nativeMethods4D4_&&nativeMethods4D4_[z])))return false;
        }
        return true;
    }
    const auto& domain=*s->raSourceLists_;
    if(domain.world!=this||s->application_!=sceneApplication_||s->count.zones!=10u||s->registries.size()!=20u||
       s->trees.size()!=20u||s->lifecycle.storage.views.methods!=s->trees.data())return false;
    for(std::size_t index=0;index<20u;++index){const auto z=index/2u;const bool dynamic=(index&1u)!=0;
        const auto& m=domain.methods[index];const auto& c=m.custody;
        auto* published=z<nativeMethodRowsInitialized_?(dynamic?(nativeMethods4D4_?nativeMethods4D4_[z]:nullptr):
            (nativeMethods4CC_?nativeMethods4CC_[z]:nullptr)):nullptr;
        const auto* alias=dynamic?collision4C4_[z].nativeList38:collision4C4_[z].nativeList34;
        if(!m.constructorAttempted||m.retired){
            if(c.reference00_||m.selected1C||published||alias)return false;
            continue;
        }
        if(z>=nativeMethodRowsInitialized_||!c.begun_||c.world_!=this||c.application_!=sceneApplication_.get()||
           c.method_!=&s->trees[index]||c.registry_!=&s->registries[index]||c.selectionOwner_!=&m||c.ordinal_!=index||
           c.reference00_!=std::uint64_t(c.arrayReference_)+std::uint64_t(c.nodeReference_)||!c.reference00_||
           !c.arrayReference_||c.usage10_!=(c.nodeAliasWasAcquired_?(dynamic?2u:1u):0u)||
           (c.nodeReference_&&!c.nodeAliasWasAcquired_)||m.count18||m.growth20!=30u||m.capacity24!=30u||
           !m.owned28Bit0||(m.type2D&&!m.type2C)||m.complete!=bool(m.selected1C&&m.type2C&&m.type2D&&!m.typeLookupFailed)||
           m.published!=bool(published)||(published&&published!=&c)||c.nodeReference_!=bool(alias)||
           (alias&&alias!=&c)||(m.published&&!m.complete)||(c.nodeReference_&&!m.published))return false;
    }
    return true;
}
bool Owner::continueNativeCollisionList(const ra_cold_sources50LA::Owner& source,std::size_t index,std::uint32_t& pc)noexcept {
    const auto z=index/2u;const bool dynamic=(index&1u)!=0;pc=dynamic?0x273854:0x2733c4;
    if(index>=20u||z!=nativeMethodZoneNext_||nativeMethodRowsInitialized_!=z+1u||
       !nativeSceneRoles270900Current(source,pc)||!nativeMethodArraysCurrent()||
       !(dynamic?visibility4C0_[z].nativeList38:visibility4C0_[z].nativeList34))return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
    const auto selection=rr::assets::readZoneSearchSelection50CN(decoded->world,z);
    if(selection.status!=rr::assets::ZoneSearchReadStatus50CN::Complete||
       (dynamic?selection.selection.visibilityDynamic114:selection.selection.collisionDynamic11C)!=rr::assets::ZoneSearchKind50CN::List)return false;
    auto* s=runtimeQueries_->storage_.get();
    if(!s||s->application_!=sceneApplication_||s->count.zones!=10u||s->registries.size()!=20u||s->trees.size()!=20u||
       (dynamic?s->zones[z].secondMethod:s->zones[z].firstMethod)!=index+1u||
       s->lifecycle.storage.views.methods!=s->trees.data())return false;
    const auto& registry=s->registries[index];const auto& tree=s->trees[index];
    if(registry.perBlock!=0x400u||registry.blockCount>1u||registry.lastUsed>0x400u||
       tree.method!=g::CollectionTree50BX::Method::SearchList||tree.searchList.perBlock30!=0x400||
       tree.searchList.blockCount!=static_cast<std::int32_t>(registry.blockCount)||
       tree.searchList.lastUsed38!=static_cast<std::int32_t>(registry.lastUsed)||
       (registry.blockCount&&(!tree.searchList.blocks||tree.searchList.blockCapacity<2u||
        tree.searchList.blocks[0].words!=registry.words||tree.searchList.blocks[0].capacity!=0x400u)))return false;
    try{
        if(!s->raSourceLists_){
            s->raSourceLists_=std::make_unique<track_runtime_storage50IG::Storage::RaSourceListDomain>();s->raSourceLists_->world=this;
        }
        auto& m=s->raSourceLists_->methods[index];auto& c=m.custody;
        if(m.retired)return false;
        if(!m.constructorAttempted){
            pc=dynamic?0x273890:0x273400;
            rr::game::selected_array_guards27ED40::invalidateNative();
            c.world_=this;c.application_=sceneApplication_.get();c.method_=&s->trees[index];c.registry_=&s->registries[index];
            c.selectionOwner_=&m;c.ordinal_=index;c.reference00_=1;c.arrayReference_=true;c.begun_=true;m.constructorAttempted=true;
            pc=0x27ee38;m.selected1C.reset(new(std::nothrow) const Entity*[30]);
            m.growth20=30;m.capacity24=30;m.owned28Bit0=true;
            pc=0x27ea28;auto tag=sceneApplication_->types383408_.findOrAppend2A31E0("SoultreeObject");
            if(!tag.okay()){m.typeLookupFailed=true;pc=tag.sourcePC;return false;}m.type2C=tag.type;
            pc=0x27ea44;tag=sceneApplication_->types383408_.findOrAppend2A31E0("SearchableGameObject");
            if(!tag.okay()){m.typeLookupFailed=true;pc=tag.sourcePC;return false;}m.type2D=tag.type;m.complete=bool(m.selected1C);
        }
        if(!m.complete){pc=m.typeLookupFailed?0x27ea44:0x27ee38;return false;}
        auto& array=dynamic?nativeMethods4D4_[z]:nativeMethods4CC_[z];
        auto& alias=dynamic?collision4C4_[z].nativeList38:collision4C4_[z].nativeList34;
        if(!m.published){pc=dynamic?0x2738a4:0x273414;array=&c;m.published=true;}
        if(!c.nodeReference_){pc=dynamic?0x2738bc:0x27342c;c.usage10_=dynamic?2u:1u;
            c.nodeReference_=true;c.nodeAliasWasAcquired_=true;++c.reference00_;alias=&c;}
        pc=dynamic?0x273a90:0x273600;return true;
    }catch(...){return false;}
}
bool Owner::releaseNativeCollisionListReference(std::size_t index,bool nodeAlias)noexcept {
    auto* s=runtimeQueries_?runtimeQueries_->storage_.get():nullptr;
    if(!s||!s->raSourceLists_)return !nodeAlias;
    if(index>=20u||!nativeCollisionListsCurrent())return false;
    auto& domain=*s->raSourceLists_;auto& m=domain.methods[index];auto& c=m.custody;
    if(!m.constructorAttempted||m.retired)return !nodeAlias;
    if(nodeAlias){if(!c.nodeReference_)return false;c.nodeReference_=false;}
    else{if(!c.arrayReference_||c.nodeReference_)return false;c.arrayReference_=false;}
    --c.reference00_;
    if(!c.reference00_){
        m.selected1C.reset();m.retired=true;m.complete=false;m.published=false;
        c.world_=nullptr;c.application_=nullptr;c.method_=nullptr;c.registry_=nullptr;c.selectionOwner_=nullptr;
        bool any=false;for(const auto& other:domain.methods)if(other.custody.reference00_){any=true;break;}
        if(!any)s->raSourceLists_.reset();
    }
    return true;
}
}

namespace ra_pod_scene50LA {
bool Owner::adoptNativeLightWrappers273AA0(const ra_cold_sources50LA::Owner& source,std::uint32_t& pc)noexcept {
    pc=0x1aa150;
    if(nativeLightList3A8_&&nativeLightList3A8_->sourceMode)return false;
    if(!nativeDefaultObserver443148_||!nativeDefaultObserver443148_->published||
       !nativeDefaultObserverCurrent()||!nativeSceneRoles270900Current(source,pc)||
       !canCloseTopology())return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
    //New native metadata custody at actual+10C, NOT retrospective278290 or
    //270ABC construction receipt. Existing canonical render lights stay intact.
    try {
        if(!nativeLightList3A8_){
            auto list=std::make_unique<NativeLightList3A8>();auto& authority=sceneApplication_->entityAuthority_;
            auto& count=sceneApplication_->objectCount423F60();
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
               !authority.canEnroll(list->entity,key(&list->entity)))return false;
            if(g::initializeOriginalWreckComponent50LA(list->entity,g::OriginalWreckComponentKind50LA::entity141300,
                sceneGeneration_,key(&list->entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
               !authority.enroll(list->entity))std::terminate();
            list->counted=true;list->canonical=lighting_.get();nativeLightList3A8_=std::move(list);
        }
        auto& l=*nativeLightList3A8_;
        const Block* lights=nullptr;const Field* field=nullptr;
        if(!findUniqueBlock(decoded->world.parameters,"lights",lights)||!lights||
           !findUniqueField(*lights,"numberoflights",field)||!field||field->value!="185")return false;
        l.zoneCount34=10u;
        if(!l.colors3C)l.colors3C=std::unique_ptr<NativeLightList3A8::Color[]>(new NativeLightList3A8::Color[10]);
        for(;l.colorsLoaded<10u;++l.colorsLoaded){const auto z=l.colorsLoaded;
            const auto key="directionallightcolorforzone"+std::to_string(z+1u);
            if(!findUniqueField(*lights,key.c_str(),field))return false;
            auto& color=l.colors3C[z];
            if(z==0u||z==1u||z==7u){
                if(!field||field->value!="20,20,20")return false;
                //26FA60 integer text parser: absent fourth component defaults
                //255 at26FC10/14, then four stores. No unknown-W substitution.
                color.lanes={20.f,20.f,20.f,255.f};
            }else{if(field)return false;color.lanes[0]=-256.f;} //2783B4 writes ONLY x.
        }
        l.count30=185u;
        if(!l.wrappers38)l.wrappers38=std::unique_ptr<NativeLightWrapper277C10[]>(new NativeLightWrapper277C10[185]);
        for(;l.loaded<185u;++l.loaded){
            g::original_gc_gameplay_lighting_candidate::RaAuthoredLightIdentity identity;
            if(!lighting_->nativeRaLightIdentity(this,decoded->world,visibilityRows_,l.loaded,identity,pc))return false;
            //An admitted legacy row bypasses the original per-row guard.
            //Rejected calls and empty row loops do not taint source history.
            g::world_light_assertion273ABC::invalidateNative();
            l.wrappers38[l.loaded].descriptor04=identity;
        }
        if(!canRetireNativeLightWrappers())return false;
        pc=0x273b74;return true;
    }catch(...){return false;}
}
}

namespace ra_pod_scene50LA {
bool Owner::nativeAmbientLightCurrent(const Entity* actual)const noexcept {
    if(!nativeAmbientLight_||!nativeLightList3A8_||!sceneApplication_||!actual)return false;
    const auto& l=*nativeAmbientLight_;const auto& e=l.entity;
    if(actual!=&e||!l.constructed||e.kind!=g::OriginalWreckComponentKind50LA::light146720||
       e.objectKey!=key(&e)||e.sessionGeneration!=sceneGeneration_||e.category28!=0xff||
       (e.flags29&15u)!=3u||e.nativeFlags29||e.reference00!=(l.retiring?0u:1u)||
       e.firstChild18||e.previous10||e.descendants24||
       e.next14!=(nativeDirectionalLight_&&nativeDirectionalLight_->attached?&nativeDirectionalLight_->entity:nullptr)||
       e.parent1C!=(l.attached?&nativeLightList3A8_->entity:nullptr)||e.mask20!=(l.attached?2u:0u)||
       (!l.retiring&&!sceneApplication_->entityAuthority_.canRetire(e))||
       l.query30||l.flags33||l.knownFlags33!=7u||l.mode38!=3u||
       l.fieldC0||l.fieldC4||l.observerD0||l.fieldD4||l.fieldD8||
       l.positionA0!=sceneApplication_->lightVector426F10_||
       l.range84!=g::SourceUiLightMath50KG::range146898())return false;
    if(l.typed&&l.query32==0xff)return false;
    if(l.colorsWritten&&(l.type34!=4u||l.ordinalC8||
       l.ambient5C!=std::array<float,4>{{50,50,50,127.5f}}||
       l.diffuse3C!=std::array<float,4>{}||l.specular4C!=std::array<float,4>{}))return false;
    return (!l.revisionWritten||l.colorsWritten)&&(!l.configured||(l.revisionWritten&&!l.luminanceCC))&&
        (!l.registered||l.configured)&&(!l.attached||l.registered);
}
bool Owner::continueNativeAmbientLight(const ::ra_cold_aggregate50LA::Owner& world,
    rr::platform::OriginalUiLightManagerOwner50KG& manager,std::uint32_t& pc)noexcept {
    pc=0x273b74;
    if(!nativeLightList3A8_||nativeLightList3A8_->loaded!=185u||!canCloseTopology())return false;
    auto& list=*nativeLightList3A8_;auto& row=list.wrappers38[0];
    if(!row.descriptor04||row.descriptor04->ordinal!=1u||row.descriptor04->type00!=4u||
       row.descriptor04->zone20||row.descriptor04->localPrimitive)return false;
    const auto* b=row.descriptor04->retainedSource;const Field* field=nullptr;
    if(!b||!findUniqueField(*b,"colorrgb",field)||!field||field->value!="100,100,100"||b->fields.size()!=2u)return false;
    try {
        if(!nativeAmbientLight_){
            pc=0x277e0c;auto light=std::make_unique<NativeRaLight146720>();
            auto& authority=sceneApplication_->entityAuthority_;auto& count=sceneApplication_->objectCount423F60();
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
               !authority.canEnroll(light->entity,key(&light->entity)))return false;
            pc=0x277e20;
            if(g::initializeOriginalWreckComponent50LA(light->entity,g::OriginalWreckComponentKind50LA::light146720,
                sceneGeneration_,key(&light->entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
               !authority.enroll(light->entity))std::terminate();
            light->constructed=true;light->range84=g::SourceUiLightMath50KG::range146898();
            light->positionA0=sceneApplication_->lightVector426F10_;nativeAmbientLight_=std::move(light);
        }
        auto& l=*nativeAmbientLight_;
        if(!nativeAmbientLightCurrent(&l.entity)||l.retiring)return false;
        if(!l.typed){pc=0x146950;const auto type=sceneApplication_->types383408().lightEmitter146938();
            if(!type.okay())return false;l.query32=type.type;l.typed=true;
            pc=0x277e28;row.live00=&l.entity;
        }
        if(!l.colorsWritten){
            //141C20 returns this; no hidden Entity side effect. Exact halves,
            //including source26FA60 missing alpha255. No FCSR inference.
            pc=0x146fc0;l.type34=4;l.ordinalC8=0;
            l.ambient5C={{50,50,50,127.5f}};l.diffuse3C={};l.specular4C={};l.colorsWritten=true;
        }
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic diagnostic;
        if(!l.revisionWritten){pc=0x149300;
            if(!manager.incrementRaAmbient149300(world,l.entity,diagnostic)){pc=diagnostic.sourcePC;return false;}
            l.revisionWritten=true;
        }
        if(!l.configured){
            pc=0x146e60;if(!g::SourceUiLightMath50KG::luminance146E60(0,0,0,l.luminanceCC))return false;
            //First Ambient reader277508 skips vectors; range0 maps to unchanged
            //constructor maximum, no extra revision. Null attenuation ->1,0,0.
            l.configured=true;
        }
        if(!l.registered){pc=0x277fcc;
            if(!manager.appendRaAmbient148F30(world,l.entity,diagnostic)){pc=diagnostic.sourcePC;return false;}
            l.registered=true;
        }
        //Descriptor remains immutable provenance, not a retail F8 temp leak:
        //its optional native borrower is NOT an owned277220 allocation.
        if(!l.attached){pc=0x273b98;
            const auto attached=sceneApplication_->entityAuthority_.attachChildren(list.entity,l.entity,2u,
                sceneApplication_->attachmentLock423F58());
            if(attached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=attached.sourcePC;return false;}
            l.attached=true;
        }
        pc=0x273ba0;return true;
    }catch(...){return false;}
}
bool Owner::retireNativeAmbientLight(const ::ra_cold_aggregate50LA::Owner& world,
    rr::platform::OriginalUiLightManagerOwner50KG& manager)noexcept {
    if(!nativeAmbientLight_)return true;
    auto& l=*nativeAmbientLight_;if(!nativeAmbientLightCurrent(&l.entity)||l.retiring)return false;
    //Explicit native tail-first partial retirement before148110 zones. Do NOT
    //infer an unproven retail World3A8 position. Source146980 still calls actual
    //manager removal even when construction stopped before registration.
    if(l.attached){
        nativeLightList3A8_->entity.firstChild18=nullptr;nativeLightList3A8_->entity.descendants24=0;
        l.entity.parent1C=nullptr;l.entity.mask20=0;l.attached=false;
    }
    if(!sceneApplication_->entityAuthority_.retire(l.entity))std::terminate();
    --l.entity.reference00;l.retiring=true;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic diagnostic;
    if(!manager.removeRaAmbient1491F0(world,l.entity,diagnostic))std::terminate();
    --sceneApplication_->objectCount423F60();nativeLightList3A8_->wrappers38[0].live00=nullptr;
    nativeAmbientLight_.reset();return true;
}
}

namespace ra_pod_scene50LA {
bool Owner::nativeDirectionalLightCurrent(const Entity* actual)const noexcept {
    if(!nativeDirectionalLight_||!nativeDirectionalInputs_||!nativeLightList3A8_||!nativeAmbientLight_||
       !sceneApplication_||!actual)return false;
    const auto& l=*nativeDirectionalLight_;const auto& e=l.entity;const auto& input=*nativeDirectionalInputs_;
    const auto bits=[](float v){std::uint32_t out;std::memcpy(&out,&v,4);return out;};
    if(actual!=&e||!l.constructed||e.kind!=g::OriginalWreckComponentKind50LA::light146720||
       e.objectKey!=key(&e)||e.sessionGeneration!=sceneGeneration_||e.category28!=0xffu||
       (e.flags29&15u)!=3u||e.nativeFlags29||e.reference00!=(l.retiring?0u:1u)||e.firstChild18||e.descendants24||
       e.next14!=(nativePoints_[0].light&&nativePoints_[0].light->attached?&nativePoints_[0].light->entity:nullptr)||
       e.previous10!=(l.attached?&nativeAmbientLight_->entity:nullptr)||
       e.parent1C!=(l.attached?&nativeLightList3A8_->entity:nullptr)||e.mask20!=(l.attached?2u:0u)||
       (!l.retiring&&!sceneApplication_->entityAuthority_.canRetire(e))||l.query30||l.flags33||l.knownFlags33!=7u||
       l.mode38!=3u||l.fieldC0||l.fieldC4||l.observerD0||l.fieldD4||l.fieldD8||(l.typed&&l.query32==0xffu))return false;
    if(l.colorsWritten&&(l.type34!=2u||l.ordinalC8!=1||l.diffuse3C!=std::array<float,4>{{103,103,92.5f,127.5f}}||
       l.specular4C!=l.diffuse3C||l.ambient5C!=std::array<float,4>{}))return false;
    for(std::size_t i=0;i<4;++i){
        if(l.positionA0[i]!=(l.positionWritten?bits(input.positionA0[i]):sceneApplication_->lightVector426F10_[i]))return false;
        if(l.lookB0[i]!=(l.lookWritten?bits(input.lookB0[i]):(i==3u?0x3f800000u:0u)))return false;
    }
    if(l.positionWritten)for(std::size_t i=0;i<3;++i)if(l.position6C[i]!=input.positionA0[i])return false;
    if(l.lookWritten&&l.look78!=input.look78)return false;
    if(l.range84!=(l.rangeWritten?bits(input.range84):g::SourceUiLightMath50KG::range146898())||
       (l.rangeWritten&&l.attenuation8C!=input.attenuation8C))return false;
    return (!l.revisionWritten||l.colorsWritten)&&(!l.positionWritten||l.revisionWritten)&&
        (!l.positionRevision||l.positionWritten)&&(!l.lookWritten||l.positionRevision)&&
        (!l.lookRevision||l.lookWritten)&&(!l.rangeWritten||l.lookRevision)&&(!l.rangeRevision||l.rangeWritten)&&
        (!l.configured||l.rangeRevision)&&(!l.registered||l.configured)&&(!l.attached||l.registered);
}
bool Owner::continueNativeDirectionalLight(const ::ra_cold_aggregate50LA::Owner& world,
    const ra_cold_sources50LA::Owner& source,rr::platform::OriginalUiLightManagerOwner50KG& manager,
    std::uint32_t& pc)noexcept {
    pc=0x273b74;
    if(!nativeAmbientLight_||!nativeAmbientLight_->attached||!nativeLightList3A8_||
       nativeLightList3A8_->loaded!=185u||!canCloseTopology())return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
    auto& row=nativeLightList3A8_->wrappers38[1];
    if(!row.descriptor04||row.descriptor04->ordinal!=2u||row.descriptor04->type00!=2u||
       row.descriptor04->zone20||row.descriptor04->localPrimitive)return false;
    try {
        if(!nativeDirectionalInputs_){g::original_gc_gameplay_lighting_candidate::RaDirectionalLightInputs input;
            if(!lighting_->nativeRaDirectionalInputs(this,decoded->world,visibilityRows_,input,pc))return false;
            nativeDirectionalInputs_=input;
        }
        const auto& input=*nativeDirectionalInputs_;
        std::uint32_t nativeRangeBits=0;std::memcpy(&nativeRangeBits,&input.range84,4);
        if(!nativeRangeBits||nativeRangeBits>=g::SourceUiLightMath50KG::range146898())return false;
        if(!nativeDirectionalLight_){pc=0x277e0c;auto light=std::make_unique<NativeRaLight146720>();
            auto& authority=sceneApplication_->entityAuthority_;auto& count=sceneApplication_->objectCount423F60();
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
               !authority.canEnroll(light->entity,key(&light->entity)))return false;
            if(g::initializeOriginalWreckComponent50LA(light->entity,g::OriginalWreckComponentKind50LA::light146720,
                sceneGeneration_,key(&light->entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
               !authority.enroll(light->entity))std::terminate();
            light->constructed=true;light->range84=g::SourceUiLightMath50KG::range146898();
            light->positionA0=sceneApplication_->lightVector426F10_;nativeDirectionalLight_=std::move(light);
        }
        auto& l=*nativeDirectionalLight_;if(!nativeDirectionalLightCurrent(&l.entity)||l.retiring)return false;
        const auto bits=[](float v){std::uint32_t out;std::memcpy(&out,&v,4);return out;};
        if(!l.typed){pc=0x146950;const auto type=sceneApplication_->types383408().lightEmitter146938();
            if(!type.okay())return false;l.query32=type.type;l.typed=true;row.live00=&l.entity;}
        if(!l.colorsWritten){l.type34=2;l.ordinalC8=1;l.diffuse3C=l.specular4C={{103,103,92.5f,127.5f}};
            l.ambient5C={};l.colorsWritten=true;}
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic diagnostic;
        const auto revise=[&](std::uint32_t site,bool& done){if(done)return true;pc=site;
            if(!manager.incrementRaDirectional149300(world,l.entity,site,diagnostic)){pc=diagnostic.sourcePC;return false;}
            done=true;return true;};
        if(!revise(0x146e30,l.revisionWritten))return false;
        pc=0x146e60;if(!g::SourceUiLightMath50KG::luminance146E60(0x42ce0000u,0x42ce0000u,0x42b90000u,l.luminanceCC))return false;
        //Range0 keeps constructor maximum. Actual nonzero position changes;
        //C0/C4/D0 are null, so147600 and optional callbacks have no resources.
        if(!l.positionWritten){for(std::size_t i=0;i<4;++i)l.positionA0[i]=bits(input.positionA0[i]);
            for(std::size_t i=0;i<3;++i)l.position6C[i]=input.positionA0[i];l.positionWritten=true;}
        if(!revise(0x147184,l.positionRevision))return false;
        //Same canonical bounded native pose math, not EE/VU equivalence.
        if(!l.lookWritten){for(std::size_t i=0;i<4;++i)l.lookB0[i]=bits(input.lookB0[i]);
            l.look78=input.look78;l.lookWritten=true;}
        if(!revise(0x147230,l.lookRevision))return false;
        if(!l.rangeWritten){l.attenuation8C=input.attenuation8C;l.range84=bits(input.range84);l.rangeWritten=true;}
        if(!revise(0x147354,l.rangeRevision))return false;
        l.configured=true;
        if(!l.registered){pc=0x277fcc;
            if(!manager.appendRaDirectional148F30(world,l.entity,diagnostic)){pc=diagnostic.sourcePC;return false;}
            l.registered=true;}
        //HasLensFlare bit3 passes unused146F60 stack arg; actual bit4/5
        //HasLightFlare/HasFlasher absent -> false. Do NOT invent lens resources.
        if(!l.attached){pc=0x273b98;const auto attached=sceneApplication_->entityAuthority_.attachChildren(
            nativeLightList3A8_->entity,l.entity,2u,sceneApplication_->attachmentLock423F58());
            if(attached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=attached.sourcePC;return false;}l.attached=true;}
        pc=0x273ba0;return true;
    }catch(...){return false;}
}
bool Owner::retireNativeDirectionalLight(const ::ra_cold_aggregate50LA::Owner& world,
    rr::platform::OriginalUiLightManagerOwner50KG& manager)noexcept {
    if(!nativeDirectionalLight_){nativeDirectionalInputs_.reset();return true;}
    auto& l=*nativeDirectionalLight_;if(!nativeDirectionalLightCurrent(&l.entity)||l.retiring)return false;
    if(l.attached){nativeAmbientLight_->entity.next14=nullptr;l.entity.previous10=l.entity.parent1C=nullptr;
        l.entity.mask20=0;l.attached=false;} //Parent still has actual Ambient mask2.
    if(!sceneApplication_->entityAuthority_.retire(l.entity))std::terminate();
    --l.entity.reference00;l.retiring=true;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic diagnostic;
    if(!manager.removeRaDirectional1491F0(world,l.entity,diagnostic))std::terminate();
    --sceneApplication_->objectCount423F60();nativeLightList3A8_->wrappers38[1].live00=nullptr;
    nativeDirectionalLight_.reset();nativeDirectionalInputs_.reset();return true;
}
}

namespace ra_pod_scene50LA {
bool Owner::nativeLocalLightCurrent(const Entity* actual)const noexcept {
    for(const auto& p:nativeRemainder_)if(p.light && &p.light->entity==actual)return nativeRemainingLightCurrent(actual);
    std::size_t index=0;while(index<nativePoints_.size()&&(!nativePoints_[index].light||&nativePoints_[index].light->entity!=actual))++index;
    if(index>=nativePoints_.size())return false;
    const auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    if(!nativeLocalLight_||!nativeLocalInputs_||!nativeLightList3A8_||!sceneApplication_)return false;
    const auto& l=*nativeLocalLight_;const auto& e=l.entity;const auto& input=*nativeLocalInputs_;
    if(l.attached&&((index&&!nativePoints_[index-1u].light)||(!index&&!nativeDirectionalLight_)))return false;
    const auto bits=[](float v){std::uint32_t out;std::memcpy(&out,&v,4);return out;};
    if(actual!=&e||!l.constructed||e.kind!=g::OriginalWreckComponentKind50LA::light146720||
       e.objectKey!=key(&e)||e.sessionGeneration!=sceneGeneration_||e.category28!=0xffu||
       (e.flags29&15u)!=3u||e.nativeFlags29||e.reference00!=(l.retiring?0u:1u)||
       e.firstChild18!=(nativeLocalFlare_&&nativeLocalFlare_->lightAttached?&nativeLocalFlare_->entity:nullptr)||
       e.descendants24!=(nativeLocalFlare_&&nativeLocalFlare_->lightAttached?nativeLocalFlare_->entity.descendants24:0u)||
       e.parent1C!=(l.attached?&nativeLightList3A8_->entity:nullptr)||
       e.previous10!=(l.attached?(index?&nativePoints_[index-1u].light->entity:&nativeDirectionalLight_->entity):nullptr)||
       e.next14!=(index+1u<nativePoints_.size()?
           (nativePoints_[index+1u].light&&nativePoints_[index+1u].light->attached?&nativePoints_[index+1u].light->entity:nullptr):
           (nativeRemainder_[0].light&&nativeRemainder_[0].light->attached?&nativeRemainder_[0].light->entity:nullptr))||e.mask20!=(l.attached?2u:0u)||(!l.retiring&&!sceneApplication_->entityAuthority_.canRetire(e))||
       l.query30||l.flags33||l.knownFlags33!=7u||(l.typed&&l.query32==0xffu)||l.fieldC0||
       l.fieldC4!=(nativeLocalFlare_&&nativeLocalFlare_->lightPublished?&nativeLocalFlare_->entity:retiredLocalFlareIdentity_)||l.fieldD8||
       l.mode38!=(l.configured?1u:3u)||l.lookWritten||l.lookRevision)return false;
    if(l.colorsWritten&&(l.type34!=1u||l.ordinalC8!=static_cast<std::int32_t>(index+2u)||l.diffuse3C!=std::array<float,4>{{127.5f,127.5f,127.5f,127.5f}}||
       l.specular4C!=l.diffuse3C||l.ambient5C!=std::array<float,4>{}))return false;
    for(std::size_t i=0;i<4;++i)if(l.positionA0[i]!=(l.positionWritten?bits(input.positionA0[i]):sceneApplication_->lightVector426F10_[i])||
       l.lookB0[i]!=(i==3u?0x3f800000u:0u))return false;
    for(std::size_t i=0;i<3;++i)if(l.positionWritten&&l.position6C[i]!=input.positionA0[i])return false;
    if(l.look78!=std::array<float,3>{{1,0,0}}||l.range84!=(l.rangeWritten?bits(input.range84):g::SourceUiLightMath50KG::range146898())||
       (l.rangeWritten&&l.attenuation8C!=input.attenuation8C))return false;
    if(l.observerOwner){const auto& o=*l.observerOwner;
        if(o.field0C||o.field10||o.unknown18To1C[0]||o.unknown18To1C[1]||
           (o.sharedConstructed&&!sceneApplication_->garageObserverShared_.owns(&o))||
           l.observerD0!=(o.sharedConstructed?&o:nullptr))return false;
        std::size_t visited=0;
        for(auto* link=o.next00;link;link=link->next00){
            if(++visited>10u||!sceneApplication_->garageObserverShared_.ownsLink(&o,link)||
               !link->child04||!link->method08||!link->portal0C||!link->slot14)return false;
        }
        if((o.portalLoopStarted&&!o.primaryRegistered)||(o.portalLoopComplete&&(!o.portalLoopStarted||o.portalCursor||o.pending))||
           (l.registered&&!o.portalLoopComplete)||(!o.portalLoopStarted&&(o.portalCursor||o.cursorMethod||o.pending||o.next00)))return false;
        if(o.pending&&(!o.pending->child||!o.pending->method||!o.pending->portal||!o.pending->slot||!o.portalCursor))return false;
        if(o.primaryRegistered){if(!o.sharedConstructed||!o.child04||!o.method08||!o.slot14||
            l.fieldD4!=o.child04||o.center20!=l.positionA0)return false;}
        else if(o.child04||o.method08||o.slot14||o.center20!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})return false;
    }else if(l.observerD0||(!l.retiring&&(l.fieldD4||l.registered)))return false;
    return (!l.revisionWritten||l.colorsWritten)&&(!l.positionWritten||l.revisionWritten)&&
        (!l.positionRevision||l.positionWritten)&&(!l.rangeWritten||l.positionRevision)&&
        (!l.rangeRevision||l.rangeWritten)&&(!l.configured||l.rangeRevision);
}
bool Owner::continueNativeLocalPrimary(std::size_t index,const ::ra_cold_aggregate50LA::Owner& world,
    const ra_cold_sources50LA::Owner& source,rr::platform::OriginalUiLightManagerOwner50KG& manager,
    std::uint32_t& pc)noexcept {
    if(index>=nativePoints_.size())return false;
    auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    pc=0x273b74;
    if(!nativeDirectionalLight_||!nativeDirectionalLight_->attached||!nativeLightList3A8_||
       nativeLightList3A8_->loaded!=185u||!canCloseTopology())return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
    auto& row=nativeLightList3A8_->wrappers38[index+2u];
    if(!row.descriptor04||row.descriptor04->ordinal!=index+3u||row.descriptor04->type00!=1u||
       row.descriptor04->zone20!=(index<5u?1u:7u)||!row.descriptor04->localPrimitive)return false;
    try {
        if(!nativeLocalInputs_){g::original_gc_gameplay_lighting_candidate::RaDirectionalLightInputs input;
            if(!lighting_->nativeRaLocalInputs(this,decoded->world,visibilityRows_,index+2u,input,pc))return false;nativeLocalInputs_=input;}
        const auto& input=*nativeLocalInputs_;const auto bits=[](float v){std::uint32_t out;std::memcpy(&out,&v,4);return out;};
        if(!bits(input.range84)||bits(input.range84)>=g::SourceUiLightMath50KG::range146898())return false;
        if(!nativeLocalLight_){auto l=std::make_unique<NativeRaLight146720>();
            auto& authority=sceneApplication_->entityAuthority_;auto& count=sceneApplication_->objectCount423F60();
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||!authority.canEnroll(l->entity,key(&l->entity)))return false;
            if(g::initializeOriginalWreckComponent50LA(l->entity,g::OriginalWreckComponentKind50LA::light146720,
                sceneGeneration_,key(&l->entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||!authority.enroll(l->entity))std::terminate();
            l->constructed=true;l->range84=g::SourceUiLightMath50KG::range146898();l->positionA0=sceneApplication_->lightVector426F10_;
            nativeLocalLight_=std::move(l);}
        auto& l=*nativeLocalLight_;if(!nativeLocalLightCurrent(&l.entity)||l.retiring)return false;
        if(!l.typed){pc=0x146950;const auto type=sceneApplication_->types383408().lightEmitter146938();
            if(!type.okay())return false;l.query32=type.type;l.typed=true;row.live00=&l.entity;}
        if(!l.colorsWritten){l.type34=1;l.ordinalC8=static_cast<std::int32_t>(index+2u);l.diffuse3C=l.specular4C={{127.5f,127.5f,127.5f,127.5f}};l.ambient5C={};l.colorsWritten=true;}
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
        const auto revise=[&](std::uint32_t site,bool& done){if(done)return true;pc=site;
            if(!manager.incrementRaLocal149300(world,l.entity,site,d)){pc=d.sourcePC;return false;}done=true;return true;};
        if(!revise(0x146e30,l.revisionWritten))return false;
        pc=0x146e60;if(!g::SourceUiLightMath50KG::luminance146E60(0x42ff0000u,0x42ff0000u,0x42ff0000u,l.luminanceCC))return false;
        if(!l.positionWritten){for(std::size_t i=0;i<4;++i)l.positionA0[i]=bits(input.positionA0[i]);
            for(std::size_t i=0;i<3;++i)l.position6C[i]=input.positionA0[i];l.positionWritten=true;}
        if(!revise(0x147184,l.positionRevision))return false;
        //Absent look stays constructorB0/(+X)78, no147230 revision.
        if(!l.rangeWritten){l.attenuation8C=input.attenuation8C;l.range84=bits(input.range84);l.rangeWritten=true;}
        if(!revise(0x147354,l.rangeRevision))return false;
        l.mode38=1;l.configured=true;
        if(!l.registered){
            pc=0x149080;if(!manager.bindRaLocalPrimary2809C0(world,l.entity,d)){pc=d.sourcePC;return false;}
            if(!manager.continueRaLocalPortals280BF0(world,l.entity,d)){pc=d.sourcePC;return false;}
            if(!manager.appendRaLocal148F30(world,l.entity,d)){pc=d.sourcePC;return false;}
            l.registered=true;
        }
        pc=0x277fe4;return true;
    }catch(...){return false;}
}
bool Owner::retireNativeLocalPrimary(std::size_t index,const ::ra_cold_aggregate50LA::Owner& world,
    rr::platform::OriginalUiLightManagerOwner50KG& manager)noexcept {
    if(index>=nativePoints_.size())return false;
    auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    if(!nativeLocalLight_){nativeLocalInputs_.reset();return true;}
    auto& l=*nativeLocalLight_;if(!nativeLocalLightCurrent(&l.entity)||l.retiring)return false;
    if(l.attached){auto* previous=index?&nativePoints_[index-1u].light->entity:&nativeDirectionalLight_->entity;
        previous->next14=nullptr;l.entity.previous10=l.entity.parent1C=nullptr;
        l.entity.mask20=0;l.attached=false;nativeLightList3A8_->entity.descendants24=2u|nativePointDescendants();}
    if(!sceneApplication_->entityAuthority_.retire(l.entity))std::terminate();--l.entity.reference00;l.retiring=true;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
    if(!manager.removeRaLocalPrimary1491F0(world,l.entity,d))std::terminate();
    --sceneApplication_->objectCount423F60();nativeLightList3A8_->wrappers38[index+2u].live00=nullptr;
    nativeLocalLight_.reset();nativeLocalInputs_.reset();retiredLocalFlareIdentity_=nullptr;return true;
}
}

namespace ra_pod_scene50LA {
bool Owner::nativeLocalFlareCurrentAt(std::size_t index)const noexcept {
    if(index>=nativePoints_.size())return false;
    const auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    if(!nativeLocalFlare_)return true;
    if(!sceneApplication_||!nativeLocalLight_||!nativeLocalLight_->registered||!nativeLocalInputs_)return false;
    const auto& f=*nativeLocalFlare_;const auto& e=f.entity;
    if(f.stripAttached&&!f.strip90)return false;
    if(!f.entityConstructed||!f.table||!f.table->prepared()||e.kind!=g::OriginalWreckComponentKind50LA::lightFlare14AA10||
       e.objectKey!=key(&e)||e.sessionGeneration!=sceneGeneration_||e.reference00!=1u||e.category28!=0xffu||
       (e.flags29&15u)!=3u||e.nativeFlags29||e.parent1C!=(f.lightAttached?&nativeLocalLight_->entity:nullptr)||
       e.previous10||e.next14||e.mask20||
       e.firstChild18!=(f.stripAttached?&f.strip90->entity_:nullptr)||e.descendants24!=(f.stripAttached?8u:0u)||
       !sceneApplication_->entityAuthority_.canRetire(e)||f.query30||f.knownFlags33!=7u||
       f.flags33!=(f.typed?3u:0u)||(f.typed&&f.query32==0xffu)||f.constructed!=f.typed||
       f.packedColor!=0xffffffffu||f.turning9C!=1u||(f.inputsWritten&&!f.constructed)||
       bool(f.size98)!=f.inputsWritten||(f.size98&&*f.size98!=30.f))return false;
    const auto bits=[](float value){std::uint32_t word;std::memcpy(&word,&value,4);return word;};
    for(std::size_t i=0;i<5u;++i)for(std::size_t k=0;k<4u;++k){
        const auto expected=i==0u&&f.inputsWritten?bits(nativeLocalInputs_->positionA0[k]):(k==3u?0x3f800000u:0u);
        if(i&&f.cornersWritten){float value;std::memcpy(&value,&f.vectors40To80[i][k],4);
            if(!std::isfinite(value))return false;
        }else if(f.vectors40To80[i][k]!=expected)return false;
    }
    if(f.lightAttached&&(!f.lightPublished||!f.observerOwner||!f.observerOwner->portalLoopComplete))return false;
    if(f.visibilityIdentity){
        const auto* o=f.observerOwner.get();
        if(!lighting_->raFlareIdentityCurrent(this,f.entity,f.visibilityIdentity,
            o&&o->primaryRegistered?o->method08:nullptr,o&&o->primaryRegistered?o->slot14:0u))return false;
    }else if(f.observerOwner||f.observer94)return false;
    if(f.observerOwner){const auto& o=*f.observerOwner;
        if(o.next00||o.field0C||o.field10||o.unknown18To1C[0]||o.unknown18To1C[1]||
           (o.sharedConstructed&&!sceneApplication_->garageObserverShared_.owns(&o))||
           f.observer94!=(o.sharedConstructed?&o:nullptr)||
           (o.portalLoopComplete&&(!o.portalLoopStarted||o.portalCursor)))return false;
        if(o.primaryRegistered){if(!o.sharedConstructed||o.child04!=&visibility4C0_[index<5u?1u:7u].entity||
            o.method08!=visibility4C0_[index<5u?1u:7u].nativeList34||!o.slot14||o.center20!=f.vectors40To80[0])return false;
        }else if(o.child04||o.method08||o.slot14||o.portalLoopStarted||o.portalCursor)return false;
    }else if(f.observer94)return false;
    if((f.stripPublished&&!f.stripConstructed)||(f.stripAttached&&!f.stripPublished)||
       (f.setupComplete&&(!f.setupStarted||!f.stripAttached))||(f.setupStarted&&!f.stripPublished))return false;
    if(f.strip90){const auto& strip=*f.strip90;const auto& s=strip.entity_;
        if(!f.stripConstructed||s.kind!=g::OriginalWreckComponentKind50LA::strip23BCA0||s.objectKey!=key(&s)||
           s.sessionGeneration!=sceneGeneration_||s.reference00!=1u||s.category28!=0xffu||(s.flags29&15u)!=3u||
           s.nativeFlags29||s.firstChild18||s.previous10||s.next14||s.descendants24||
           s.parent1C!=(f.stripAttached?&f.entity:nullptr)||s.mask20!=(f.stripAttached?8u:0u)||
           !sceneApplication_->entityAuthority_.canRetire(s))return false;
        if(strip.primitive30_&&!strip.primitive30_->reference.canRelease299AC0())return false;
        if(f.setupComplete&&(!strip.primitive30_||!strip.primitive30_->reference.retained()||
           strip.format3C_!=7u||strip.count34_!=1u||strip.field40_!=1u||!strip.groups38_||strip.groups38_[0]!=4u||
           strip.color44_!=0xffffffffu||strip.color70_!=std::array<float,4>{{127.f,127.f,127.f,64.f}}||
           strip.primitive30_->byte0C!=7u||strip.primitive30_->attributes10!=&strip.attributes_||
           strip.primitive30_->vertices20!=4u||strip.primitive30_->groups22!=1u))return false;
    }else if(f.stripConstructed||f.stripPublished||f.stripAttached||f.setupStarted)return false;
    return true;
}
bool Owner::continueNativeLocalFlare(std::size_t index,const std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>& table,
    const std::string& directory,std::uint32_t& pc)noexcept {
    if(index>=nativePoints_.size())return false;
    auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    pc=0x277fe4;
    if(!sceneApplication_||!nativeLocalLight_||!nativeLocalLight_->registered||!nativeLocalInputs_||
       !nativeLightList3A8_||!nativeLocalFlareCurrent()||!table||!table->prepared())return false;
    const auto& row=nativeLightList3A8_->wrappers38[index+2u];
    if(!row.descriptor04||!row.descriptor04->retainedSource||row.live00!=&nativeLocalLight_->entity)return false;
    const auto& authored=*row.descriptor04->retainedSource;const Field* field=nullptr;
    const auto exact=[&](const char* name,const char* value){return findUniqueField(authored,name,field)&&field&&field->value==value;};
    if(!exact("haslightflare","1")||!exact("lightflarecolorrgb","255,255,255")||
       !exact("lightflaresize","30.000000")||!exact("lightflaretexturemapname","flare1")||
       !findUniqueField(authored,"lightflareturning",field)||field)return false;
    try {
        if(!nativeLocalFlare_){
            auto owned=std::make_unique<NativeLocalFlare>();owned->table=table;owned->directory=directory;
            auto& f=*owned;auto& count=sceneApplication_->objectCount423F60();auto& authority=sceneApplication_->entityAuthority_;
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
               !authority.canEnroll(f.entity,key(&f.entity)))return false;
            pc=0x278050;
            if(g::initializeOriginalWreckComponent50LA(f.entity,g::OriginalWreckComponentKind50LA::lightFlare14AA10,
                sceneGeneration_,key(&f.entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
               !authority.enroll(f.entity))std::terminate();
            f.entityConstructed=true;nativeLocalFlare_=std::move(owned);
        }
        auto& f=*nativeLocalFlare_;
        if(f.table!=table||f.directory!=directory)return false;
        if(!f.typed){pc=0x27e1d4;const auto type=sceneApplication_->types383408().searchableGameObject27E1BC();
            if(!type.okay())return false;f.query32=type.type;f.typed=true;f.flags33=3u;f.constructed=true;}
        if(!f.inputsWritten){
            //Four CVT.W.S inputs255 are exactintegral under either truncation or
            //nearest; no sourceFCSR claim.26FA60 W255, packedFFFFFFFF.
            f.packedColor=0xffffffffu;f.size98=30.f;f.turning9C=1u;
            for(std::size_t i=0;i<4u;++i)std::memcpy(&f.vectors40To80[0][i],&nativeLocalInputs_->positionA0[i],4);
            f.inputsWritten=true;
        }
        if(!setupNativeLocalFlareStrip(f,table,directory,pc))return false;
        pc=0x278098;return nativeLocalFlareCurrent();
    }catch(...){return false;}
}
bool Owner::retireNativeLocalFlare(std::size_t index)noexcept {
    if(index>=nativePoints_.size())return false;
    auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    if(!nativeLocalFlare_)return true;
    if(!nativeLocalFlareCurrent()||sceneApplication_->attachmentLock423F58())return false;
    auto& f=*nativeLocalFlare_;auto& authority=sceneApplication_->entityAuthority_;
    //Actual142590 child-first semantics for the admitted subtree. A failed
    //unattached strip remains native abort custody, not a retail null-safe call.
    if(f.strip90){auto& strip=*f.strip90;auto& e=strip.entity_;
        if(f.stripAttached){f.entity.firstChild18=nullptr;f.entity.descendants24=0;
            e.parent1C=nullptr;e.mask20=0;f.stripAttached=false;}
        if(!authority.retire(e))std::terminate();--e.reference00;
        strip.attributes_.release23BCF0();strip.groups38_.reset();strip.primitive30_.reset();
        --sceneApplication_->objectCount423F60();f.strip90.reset();
    }
    std::uint32_t pc=0;
    if(f.observerOwner){auto& o=*f.observerOwner;
        if(o.primaryRegistered&&!lighting_->releaseRaFlarePrimary(this,f.entity,f.visibilityIdentity,pc))std::terminate();
        //No source280BF0 secondary link is admitted on exact Light3/radius0.
        //An allocated-before-primary abort skips retail null08 dereference.
        if(o.sharedConstructed&&!sceneApplication_->garageObserverShared_.release280920(&o))std::terminate();
        f.observerOwner.reset();f.observer94=nullptr; //14B9EC after280920.
    }
    if(f.visibilityIdentity&&!lighting_->releaseRaFlareIdentity(this,f.entity,f.visibilityIdentity,pc))std::terminate();
    f.visibilityIdentity=0;
    if(f.lightAttached){nativeLocalLight_->entity.firstChild18=nullptr;nativeLocalLight_->entity.descendants24=0;
        if(nativeLocalLight_->attached)nativeLightList3A8_->entity.descendants24=2u|nativePointDescendants();
        f.entity.parent1C=nullptr;f.lightAttached=false;}
    if(f.lightPublished)retiredLocalFlareIdentity_=&f.entity; //146980 does not clearC4; never dereference afterfree.
    if(!authority.retire(f.entity))std::terminate();--f.entity.reference00;
    --sceneApplication_->objectCount423F60();nativeLocalFlare_.reset();return true;
}
}

// Draft member definition, included in existing World implementation TU.
// Caller owns/authenticates actual Light3 flare Entity/input/context/global vectors.
namespace ra_pod_scene50LA {
bool Owner::setupNativeLocalFlareStrip(NativeLocalFlare& flare,
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table,
    const std::string& directory,std::uint32_t& pc)noexcept {
    using Strip=g::OriginalPodCableStripCold50LA;
    pc=0x14ab90;
    if(!sceneApplication_||flare.packedColor!=0xffffffffu||!table||!table->prepared()||
       !sceneApplication_->entityAuthority_.canRetire(flare.entity)||
       flare.entity.reference00!=1u||flare.entity.sessionGeneration!=sceneGeneration_||
       sceneApplication_->attachmentLock423F58())return false;
    if(flare.setupComplete){pc=0x14ace4;return true;} //Caller validates complete currentness.
    if(flare.setupStarted){pc=flare.failurePC;return false;} //Never replay partial source allocation/init.
    const auto fail=[&]()noexcept {flare.failurePC=pc;return false;};
    try {
        if(!flare.strip90){
            auto strip=std::make_unique<Strip>();
            auto& authority=sceneApplication_->entityAuthority_;
            auto& count=sceneApplication_->objectCount423F60();
            pc=0x14aba4;
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
               !authority.canEnroll(strip->entity_,key(&strip->entity_)))return fail();
            if(g::initializeOriginalWreckComponent50LA(strip->entity_,
                g::OriginalWreckComponentKind50LA::strip23BCA0,sceneGeneration_,key(&strip->entity_),
                true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
               !authority.enroll(strip->entity_))std::terminate();
            strip->flags48_=1; //150378/390; other source constructor fields retain typed defaults.
            flare.stripConstructed=true;flare.strip90=std::move(strip);flare.stripPublished=true; //14ABAC.
        }
        if(!flare.stripConstructed||!flare.stripPublished||flare.stripAttached)return fail();
        auto& strip=*flare.strip90;
        if(!sceneApplication_->entityAuthority_.canRetire(strip.entity_)||strip.entity_.parent1C||
           strip.entity_.firstChild18||strip.entity_.previous10||strip.entity_.next14||strip.entity_.mask20||
           strip.entity_.descendants24||flare.entity.firstChild18)return fail();
        //14ABD0 ->23BEB0 ->150450. All subsequent failures retain this exact prefix.
        flare.setupStarted=true;
        strip.format3C_=7;strip.count34_=1;
        pc=0x150498;strip.groups38_.reset(new std::uint32_t[1]);
        strip.field40_=1; //Actual14ABD4 a9 copied by1504A4, not cable caller's zero.
        strip.color44_=flare.packedColor;strip.flags48_|=1u;
        strip.color70_=std::array<float,4>{{127.f,127.f,127.f,64.f}};
        //23D750 shifts each integer byte right1 BEFORE float conversion.
        strip.groups38_[0]=4; //382B30 raw integerword4, copied by1504EC.
        pc=0x1504f0;strip.primitive30_=std::make_unique<Strip::Primitive>();
        rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
        pc=0x299be0;
        if(!strip.primitive30_->reference.acquireNamed299BE0(table,directory,"flare1",diagnostic)){
            pc=diagnostic.sourcePC;return fail();
        }
        strip.primitive30_->byte0C=7; //2465CC follows actual DefaultMT retain.
        strip.attributes_.constructQuad23BEB0(7,pc);
        strip.primitive30_->attributes10=&strip.attributes_;
        strip.primitive30_->flags0F&=static_cast<std::uint8_t>(~1u); //Borrowed actual attributes.
        strip.primitive30_->groups22=1;strip.primitive30_->vertices20=4;
        pc=0x14abe0;
        const auto attached=sceneApplication_->entityAuthority_.attachChildren(flare.entity,strip.entity_,8u,
            sceneApplication_->attachmentLock423F58());
        if(attached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=attached.sourcePC;return fail();}
        flare.stripAttached=true;
        auto& attributes=strip.attributes_;
        attributes.setTexcoord23C800(0,0.f,0.f);attributes.setTexcoord23C800(1,0.f,1.f);
        attributes.setTexcoord23C800(2,1.f,0.f);attributes.setTexcoord23C800(3,1.f,1.f);
        //14AC74..C4: for each vertex triple first, then position. XYZ-only
        //setters preserve stripControl; W is not a fourth vertex position lane.
        for(std::uint32_t i=0;i<4u;++i){
            for(std::size_t k=0;k<3u;++k){float value;
                std::memcpy(&value,&sceneApplication_->flareTriple427020_[k],sizeof value);
                attributes.vertices_->triples[i].components[k]={value,0xffu};}
            for(std::size_t k=0;k<3u;++k){float value;
                std::memcpy(&value,&sceneApplication_->flarePosition427000_[k],sizeof value);
                attributes.vertices_->positions[i].xyz[k]={value,0xffu};}
        }
        flare.setupComplete=true;flare.failurePC=0;pc=0x14ace4;return true;
    }catch(...){return fail();}
}
}

namespace ra_pod_scene50LA {
bool Owner::nativeLocalFlareIdentityCurrent(const Entity* identity)const noexcept {
    const auto authored=nativeLocalFlareAuthoredIndex(identity);if(!authored)return false;
    const auto index=*authored-2u;
    if(index>=nativePoints_.size())return false;
    const auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    //A nonrecursive identity/authority leaf. In particular it does NOT inspect
    //the strip resource or call the canonical registry's currentness predicate:
    //14B9B0 removes membership after142590 has already retired the strip child.
    if(!sceneApplication_||!lighting_||!nativeLocalFlare_||!nativeLocalLight_||
       !nativeLocalLight_->registered||nativeLocalLight_->retiring)return false;
    const auto& f=*nativeLocalFlare_;const auto& e=f.entity;
    return identity==&e&&f.entityConstructed&&f.constructed&&f.typed&&
        e.kind==g::OriginalWreckComponentKind50LA::lightFlare14AA10&&
        e.objectKey==key(&e)&&e.sessionGeneration==sceneGeneration_&&
        e.reference00==1u&&e.category28==0xffu&&(e.flags29&15u)==3u&&!e.nativeFlags29&&
        !e.previous10&&!e.next14&&!e.mask20&&
        e.parent1C==(f.lightAttached?&nativeLocalLight_->entity:nullptr)&&
        !f.query30&&f.query32!=0xffu&&f.knownFlags33==7u&&f.flags33==3u&&
        sceneApplication_->entityAuthority_.canRetire(e);
}
bool Owner::continueNativeLocalFlareObserver(std::size_t index,std::uint32_t& pc)noexcept {
    if(index>=nativePoints_.size())return false;
    auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    pc=0x278098;
    if(!nativeLocalFlare_||!nativeLocalFlareCurrent()||!canCloseTopology()||
       !nativeLocalFlare_->setupComplete||!nativeLocalFlare_->inputsWritten)return false;
    auto& f=*nativeLocalFlare_;auto& zone=visibility4C0_[index<5u?1u:7u];
    if(!zone.constructed||!zone.attached||zone.ordinal48!=(index<5u?1u:7u)||!zone.nativeList34||
       !nativeLightList3A8_||!nativeLightList3A8_->wrappers38[index+2u].descriptor04||
       nativeLightList3A8_->wrappers38[index+2u].descriptor04->zone20!=(index<5u?1u:7u))return false;
    //Native identity enrollment extends the SAME canonical registry domain;
    //it is not a source List constructor or a replay of existing members.
    if(!lighting_->bindRaFlareIdentity(this,f.entity,f.query32,f.flags33,f.visibilityIdentity,pc))return false;
    try {
        pc=0x14ba90;
        if(!f.observerOwner)f.observerOwner=std::make_unique<NativeLocalFlareObserver>();
        auto& o=*f.observerOwner;
        if(!o.sharedConstructed){pc=0x14baa0;
            if(!sceneApplication_->garageObserverShared_.construct280890(&o))return false;
            o.sharedConstructed=true;
        }
        f.observer94=&o; //14BAB0, before the actual2809C0 service.
        if(!o.primaryRegistered){pc=0x14bad0;
            std::uint32_t slot=0;
            if(!lighting_->registerRaFlarePrimary(this,zone.nativeList34,f.entity,f.visibilityIdentity,slot,pc))return false;
            o.child04=&zone.entity;o.method08=zone.nativeList34;o.slot14=slot;
            o.center20=f.vectors40To80[0];o.primaryRegistered=true; //280B90..BBC.
        }
        if(!o.portalLoopStarted){o.portalCursor=zone.portalLinks30.get();o.portalLoopStarted=true;}
        while(o.portalCursor){
            const NativePortalLink27F3B0* edge=nullptr;
            for(auto* p=zone.portalLinks30.get();p;p=p->next00.get())if(p==o.portalCursor){edge=p;break;}
            pc=0x280c40;if(!edge||!edge->neighbor04||!edge->portal08)return false;
            const NativePortal230370* portal=nullptr;
            for(const auto& p:nativePortals_)if(p && &p->entity==edge->portal08)portal=p.get();
            if(!portal||!portal->loaded||!portal->geometry)return false;
            g::Vector3_48C center{};for(std::size_t i=0;i<3u;++i)std::memcpy(&center.v[i],&o.center20[i],4);
            bool overlaps=false;float distance=0;
            pc=0x231150;
            if(g::overlapPortal231150_50EO(*portal->geometry,center,0.f,overlaps,distance)!=
               g::PortalGeometryStatus50EO::Complete)return false;
            //14BAC0 supplies actual radius+0. The authenticated Light3 misses
            //both incident portals. Do not substitute its LightEmitter range.
            //An unsupported hit remains before280C60 distance writes and any
            //secondary method/member/shared105720 allocation; retain primary.
            if(overlaps){pc=0x280c60;return false;}
            o.portalCursor=edge->next00.get();
        }
        o.portalLoopComplete=true;pc=0x14bb54;return nativeLocalFlareCurrent();
    }catch(...){return false;}
}
bool Owner::completeNativeLocalPoint(std::size_t index,std::uint32_t& pc)noexcept {
    if(index>=nativePoints_.size())return false;
    auto& point=nativePoints_[index];auto& nativeLocalLight_=point.light;auto& nativeLocalFlare_=point.flare;
    auto& nativeLocalInputs_=point.inputs;auto& retiredLocalFlareIdentity_=point.retiredFlareIdentity;
    pc=0x2780a4;
    if(!nativeLocalFlare_||!nativeLocalFlareCurrent()||!nativeLocalLight_||
       !nativeLocalLightCurrent(&nativeLocalLight_->entity)||!nativeLightList3A8_||
       !nativeLocalFlare_->observerOwner||!nativeLocalFlare_->observerOwner->portalLoopComplete)return false;
    auto& f=*nativeLocalFlare_;auto& l=*nativeLocalLight_;
    const auto& row=nativeLightList3A8_->wrappers38[index+2u];
    if(!row.descriptor04||!row.descriptor04->retainedSource||row.live00!=&l.entity)return false;
    const Field* flasher=nullptr;
    if(!findUniqueField(*row.descriptor04->retainedSource,"hasflasher",flasher)||flasher)return false;
    if(!f.lightPublished){
        if(l.fieldC4)return false;
        l.fieldC4=&f.entity;f.lightPublished=true; //146AF4 before child attachment.
    }
    if(!f.lightAttached){pc=0x146afc;
        const auto attached=sceneApplication_->entityAuthority_.attachChildren(l.entity,f.entity,0u,
            sceneApplication_->attachmentLock423F58());
        if(attached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=attached.sourcePC;return false;}
        f.lightAttached=true;
    }
    //2780AC bit5 is absent in this complete authored row: no146B10 flasher.
    //As for prior rows, retain immutable descriptor provenance; it is not an
    //owned277220 allocation and does not claim guest2780D8..278148 frees.
    if(!l.attached){pc=0x273b98;
        const auto attached=sceneApplication_->entityAuthority_.attachChildren(nativeLightList3A8_->entity,l.entity,2u,
            sceneApplication_->attachmentLock423F58());
        if(attached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=attached.sourcePC;return false;}
        l.attached=true;
    }
    pc=0x273ba0;return nativeLocalFlareCurrent()&&nativeLocalLightCurrent(&l.entity);
}
}

namespace ra_pod_scene50LA {
std::optional<std::size_t> Owner::nativeLocalZoneIndex(const Entity* entity)const noexcept {
    if(!entity||!nativeLightList3A8_||!nativeLightList3A8_->wrappers38)return std::nullopt;
    for(std::size_t i=0;i<nativePoints_.size();++i){const auto& p=nativePoints_[i];
        if(!p.light||&p.light->entity!=entity)continue;
        const auto& row=nativeLightList3A8_->wrappers38[i+2u];
        if(!row.descriptor04||row.descriptor04->ordinal!=i+3u||row.descriptor04->type00!=1u||
           row.descriptor04->zone20!=(i<5u?1u:7u))return std::nullopt;
        return row.descriptor04->zone20;
    }
    for(std::size_t i=0;i<nativeRemainder_.size();++i){const auto& p=nativeRemainder_[i];
        if(!p.light||&p.light->entity!=entity)continue;const auto authored=i+17u;
        const auto& row=nativeLightList3A8_->wrappers38[authored];
        const auto type=authored>=172u&&authored<=174u?4u:3u;
        if(!row.descriptor04||row.descriptor04->ordinal!=authored+1u||row.descriptor04->type00!=type||
           row.descriptor04->zone20>=10u)return std::nullopt;
        return row.descriptor04->zone20;
    }return std::nullopt;
}
std::optional<std::size_t> Owner::nativeLocalFlareAuthoredIndex(const Entity* entity)const noexcept {
    if(!entity)return std::nullopt;
    for(std::size_t i=0;i<nativePoints_.size();++i){const auto& p=nativePoints_[i];
        if(p.flare && &p.flare->entity==entity&&p.light&&p.light->ordinalC8==static_cast<std::int32_t>(i+2u)&&
           nativeLocalZoneIndex(&p.light->entity))return i+2u;
    }return std::nullopt;
}
std::optional<std::size_t> Owner::nativeLocalFlareZoneIndex(const Entity* entity)const noexcept {
    const auto i=nativeLocalFlareAuthoredIndex(entity);if(!i)return std::nullopt;
    return nativeLocalZoneIndex(&nativePoints_[*i-2u].light->entity);
}
}

namespace ra_pod_scene50LA {
std::optional<std::size_t> Owner::nativeLocalLightAuthoredIndex(const Entity* entity)const noexcept {
    if(!nativeLocalZoneIndex(entity))return std::nullopt;
    for(std::size_t i=0;i<nativePoints_.size();++i)
        if(nativePoints_[i].light && &nativePoints_[i].light->entity==entity)return i+2u;
    for(std::size_t i=0;i<nativeRemainder_.size();++i)
        if(nativeRemainder_[i].light && &nativeRemainder_[i].light->entity==entity)return i+17u;
    return std::nullopt;
}
}

namespace ra_pod_scene50LA {
bool Owner::nativeRemainingLightCurrent(const Entity* actual)const noexcept {
    std::size_t index=0;
    while(index<nativeRemainder_.size()&&(!nativeRemainder_[index].light||&nativeRemainder_[index].light->entity!=actual))++index;
    if(index==nativeRemainder_.size()||!sceneApplication_||!nativeLightList3A8_||!nativeRemainder_[index].inputs)return false;
    const auto& owner=nativeRemainder_[index];const auto& l=*owner.light;const auto& e=l.entity;
    const auto& input=*owner.inputs;const auto authored=index+17u;const bool ambient=input.type==4u;
    const auto* previous=index?(nativeRemainder_[index-1u].light?&nativeRemainder_[index-1u].light->entity:nullptr):
        (nativePoints_.back().light?&nativePoints_.back().light->entity:nullptr);
    const auto* next=index+1u<nativeRemainder_.size()&&nativeRemainder_[index+1u].light&&nativeRemainder_[index+1u].light->attached?
        &nativeRemainder_[index+1u].light->entity:nullptr;
    if((l.attached&&!previous)||!l.constructed||e.kind!=g::OriginalWreckComponentKind50LA::light146720||
       e.objectKey!=key(&e)||e.sessionGeneration!=sceneGeneration_||e.reference00!=(l.retiring?0u:1u)||e.category28!=0xffu||
       (e.flags29&15u)!=3u||e.nativeFlags29||e.firstChild18||e.descendants24||
       e.parent1C!=(l.attached?&nativeLightList3A8_->entity:nullptr)||e.previous10!=(l.attached?previous:nullptr)||e.next14!=next||
       e.mask20!=(l.attached?2u:0u)||(!l.retiring&&!sceneApplication_->entityAuthority_.canRetire(e))||
       l.query30||l.flags33||l.knownFlags33!=7u||(l.typed&&l.query32==0xffu)||l.fieldC0||l.fieldC4||l.fieldD8||l.mode38!=3u||
       input.type!=(authored>=172u&&authored<=174u?4u:3u)||input.zone>=10u)return false;
    const auto bits=[](float v){std::uint32_t word;std::memcpy(&word,&v,4);return word;};
    if(l.colorsWritten&&(l.type34!=input.type||l.ordinalC8!=static_cast<std::int32_t>(authored)||
       l.diffuse3C!=input.diffuse||l.specular4C!=input.specular||l.ambient5C!=input.ambient))return false;
    if(!owner.luminanceWritten&&l.luminanceCC!=0xbf800000u)return false;
    if(ambient){
        if(l.positionWritten||l.positionRevision||l.lookWritten||l.lookRevision||l.rangeWritten||l.rangeRevision||owner.conesWritten||
           l.positionA0!=sceneApplication_->lightVector426F10_||l.position6C!=std::array<float,3>{}||
           l.lookB0!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}}||l.look78!=std::array<float,3>{{1,0,0}}||
           l.attenuation8C!=std::array<float,3>{{1,0,0}}||l.range84!=g::SourceUiLightMath50KG::range146898()||
           l.cone98!=0x3f490fdbu||l.cone9C!=0x3fc90fdbu||l.observerOwner||l.observerD0||l.fieldD4||
           (owner.luminanceWritten&&l.luminanceCC))return false;
    }else{
        for(std::size_t k=0;k<4;++k)if(l.positionA0[k]!=(l.positionWritten?bits(input.pose.positionA0[k]):sceneApplication_->lightVector426F10_[k])||
           l.lookB0[k]!=(l.lookWritten?bits(input.pose.lookB0[k]):(k==3u?0x3f800000u:0u)))return false;
        for(std::size_t k=0;k<3;++k)if(l.position6C[k]!=(l.positionWritten?input.pose.positionA0[k]:0.f)||
           l.look78[k]!=(l.lookWritten?input.pose.look78[k]:(k?0.f:1.f)))return false;
        if(l.range84!=(l.rangeWritten?bits(input.pose.range84):g::SourceUiLightMath50KG::range146898())||
           l.attenuation8C!=(l.rangeWritten?input.pose.attenuation8C:std::array<float,3>{{1,0,0}})||
           l.cone98!=(owner.conesWritten?input.cone98:0x3f490fdbu)||l.cone9C!=(owner.conesWritten?input.cone9C:0x3fc90fdbu)||
           (l.lookWritten&&!input.lookRevision)||(l.positionWritten&&!owner.luminanceWritten)||
           (l.positionRevision&&!l.positionWritten)||(l.lookWritten&&!l.positionRevision)||(l.lookRevision&&!l.lookWritten)||
           (l.rangeWritten&&!(input.lookRevision?l.lookRevision:l.positionRevision))||(l.rangeRevision&&!l.rangeWritten)||
           (l.configured&&!l.rangeRevision)||(owner.conesWritten&&!l.configured)||(l.registered&&!owner.conesWritten))return false;
        if(l.observerOwner){const auto& o=*l.observerOwner;
            if(o.field0C||o.field10||o.unknown18To1C[0]||o.unknown18To1C[1]||
               (o.sharedConstructed&&!sceneApplication_->garageObserverShared_.owns(&o))||
               l.observerD0!=(o.sharedConstructed?&o:nullptr))return false;
            std::size_t visited=0;for(auto* link=o.next00;link;link=link->next00)
                if(++visited>20u||!sceneApplication_->garageObserverShared_.ownsLink(&o,link)||
                   !link->child04||!link->method08||!link->portal0C||!link->slot14)return false;
            if((o.portalLoopStarted&&!o.primaryRegistered)||(o.portalLoopComplete&&(!o.portalLoopStarted||o.portalCursor||o.pending))||
               (l.registered&&!o.portalLoopComplete)||(!o.portalLoopStarted&&(o.portalCursor||o.cursorMethod||o.pending||o.next00)))return false;
            if(o.pending&&(!o.pending->child||!o.pending->method||!o.pending->portal||!o.pending->slot||!o.portalCursor))return false;
            if(o.primaryRegistered){if(!o.sharedConstructed||!o.child04||!o.method08||!o.slot14||
                l.fieldD4!=o.child04||o.center20!=l.positionA0)return false;}
            else if(o.child04||o.method08||o.slot14||o.center20!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})return false;
        }else if(l.observerD0||(!l.retiring&&(l.fieldD4||l.registered)))return false;
    }
    return (!l.revisionWritten||l.colorsWritten)&&(!owner.luminanceWritten||l.revisionWritten)&&
        (!l.configured||owner.luminanceWritten)&&(!l.registered||l.configured)&&(!l.attached||l.registered);
}
bool Owner::continueNativeRemainingLight(std::size_t index,const ::ra_cold_aggregate50LA::Owner& world,
    const ra_cold_sources50LA::Owner& source,rr::platform::OriginalUiLightManagerOwner50KG& manager,std::uint32_t& pc)noexcept {
    pc=0x273b74;
    if(index>=nativeRemainder_.size()||nativePointNext_!=15u||!nativeLightList3A8_||nativeLightList3A8_->loaded!=185u||
       !canCloseTopology()||(index&&(!nativeRemainder_[index-1u].light||!nativeRemainder_[index-1u].light->attached)))return false;
    auto& owner=nativeRemainder_[index];const auto authored=index+17u;
    auto& row=nativeLightList3A8_->wrappers38[authored];const auto* decoded=source.sourcesDecoded();
    if(!decoded||!row.descriptor04||row.descriptor04->ordinal!=authored+1u)return false;
    try {
        if(!owner.inputs){g::original_gc_gameplay_lighting_candidate::RaRemainingLightInputs input;
            if(!lighting_->nativeRaRemainingInputs(this,decoded->world,visibilityRows_,authored,input,pc))return false;
            owner.inputs=input;
        }
        const auto& input=*owner.inputs;const bool ambient=input.type==4u;
        if(!owner.light){pc=0x277e20;auto light=std::make_unique<NativeRaLight146720>();
            auto& authority=sceneApplication_->entityAuthority_;auto& count=sceneApplication_->objectCount423F60();
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||!authority.canEnroll(light->entity,key(&light->entity)))return false;
            if(g::initializeOriginalWreckComponent50LA(light->entity,g::OriginalWreckComponentKind50LA::light146720,
                sceneGeneration_,key(&light->entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||!authority.enroll(light->entity))std::terminate();
            light->constructed=true;light->range84=g::SourceUiLightMath50KG::range146898();light->positionA0=sceneApplication_->lightVector426F10_;
            owner.light=std::move(light);
        }
        auto& l=*owner.light;if(!nativeRemainingLightCurrent(&l.entity)||l.retiring)return false;
        if(!l.typed){pc=0x146950;const auto type=sceneApplication_->types383408().lightEmitter146938();
            if(!type.okay())return false;l.query32=type.type;l.typed=true;row.live00=&l.entity;
        }
        if(!l.colorsWritten){l.type34=input.type;l.ordinalC8=static_cast<std::int32_t>(authored);
            l.diffuse3C=input.diffuse;l.specular4C=input.specular;l.ambient5C=input.ambient;l.colorsWritten=true;
        }
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
        if(!l.revisionWritten){pc=0x146e30;
            const bool okay=ambient?manager.incrementRaLaterAmbient149300(world,l.entity,d):manager.incrementRaLocal149300(world,l.entity,pc,d);
            if(!okay){pc=d.sourcePC;return false;}l.revisionWritten=true;
        }
        const auto bits=[](float v){std::uint32_t word;std::memcpy(&word,&v,4);return word;};
        if(!owner.luminanceWritten){pc=0x146e60;
            const bool okay=ambient?g::SourceUiLightMath50KG::luminance146E60(0,0,0,l.luminanceCC):
                g::SourceUiLightMath50KG::luminanceRaRemaining146E60(bits(l.diffuse3C[0]),bits(l.diffuse3C[1]),bits(l.diffuse3C[2]),l.luminanceCC);
            if(!okay)return false;owner.luminanceWritten=true;
        }
        if(!ambient){
            if(!l.positionWritten){for(std::size_t k=0;k<4;++k)l.positionA0[k]=bits(input.pose.positionA0[k]);
                for(std::size_t k=0;k<3;++k)l.position6C[k]=input.pose.positionA0[k];l.positionWritten=true;}
            if(!l.positionRevision){pc=0x147184;if(!manager.incrementRaLocal149300(world,l.entity,pc,d)){pc=d.sourcePC;return false;}l.positionRevision=true;}
            if(input.lookRevision){
                if(!l.lookWritten){for(std::size_t k=0;k<4;++k)l.lookB0[k]=bits(input.pose.lookB0[k]);l.look78=input.pose.look78;l.lookWritten=true;}
                if(!l.lookRevision){pc=0x147230;if(!manager.incrementRaLocal149300(world,l.entity,pc,d)){pc=d.sourcePC;return false;}l.lookRevision=true;}
            }
            if(!l.rangeWritten){l.range84=bits(input.pose.range84);l.attenuation8C=input.pose.attenuation8C;l.rangeWritten=true;}
            if(!l.rangeRevision){pc=0x147354;if(!manager.incrementRaLocal149300(world,l.entity,pc,d)){pc=d.sourcePC;return false;}l.rangeRevision=true;}
        }
        l.configured=true; //Source type3/4 leaves constructor mode38=3, not Point's1.
        if(!ambient&&!owner.conesWritten){pc=0x146f30;l.cone98=input.cone98;l.cone9C=input.cone9C;owner.conesWritten=true;}
        if(!l.registered){
            if(ambient){pc=0x277fcc;if(!manager.appendRaLaterAmbient148F30(world,l.entity,d)){pc=d.sourcePC;return false;}}
            else{pc=0x149080;if(!manager.bindRaLocalPrimary2809C0(world,l.entity,d)||
                 !manager.continueRaLocalPortals280BF0(world,l.entity,d)||!manager.appendRaLocal148F30(world,l.entity,d)){pc=d.sourcePC;return false;}}
            l.registered=true;
        }
        //Whole selected source row has no HasLightFlare/HasFlasher. Retained
        //descriptor is immutable provenance, not a guest temporary allocation.
        if(!l.attached){pc=0x273b98;
            const auto attached=sceneApplication_->entityAuthority_.attachChildren(nativeLightList3A8_->entity,l.entity,2u,
                sceneApplication_->attachmentLock423F58());
            if(attached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=attached.sourcePC;return false;}l.attached=true;
        }
        pc=0x273ba0;return nativeRemainingLightCurrent(&l.entity);
    }catch(...){return false;}
}
bool Owner::retireNativeRemainingLight(std::size_t index,const ::ra_cold_aggregate50LA::Owner& world,
    rr::platform::OriginalUiLightManagerOwner50KG& manager)noexcept {
    if(index>=nativeRemainder_.size())return false;auto& owner=nativeRemainder_[index];
    if(!owner.light){owner.inputs.reset();owner.luminanceWritten=owner.conesWritten=false;return true;}
    auto& l=*owner.light;if(!nativeRemainingLightCurrent(&l.entity)||l.retiring||l.entity.next14)return false;
    if(l.attached){auto* previous=index?&nativeRemainder_[index-1u].light->entity:&nativePoints_.back().light->entity;
        previous->next14=nullptr;l.entity.previous10=l.entity.parent1C=nullptr;l.entity.mask20=0;l.attached=false;
        nativeLightList3A8_->entity.descendants24=2u|nativePointDescendants();
    }
    if(!sceneApplication_->entityAuthority_.retire(l.entity))std::terminate();--l.entity.reference00;l.retiring=true;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
    if(!manager.removeRaLocalPrimary1491F0(world,l.entity,d))std::terminate();
    --sceneApplication_->objectCount423F60();nativeLightList3A8_->wrappers38[index+17u].live00=nullptr;
    owner.light.reset();owner.inputs.reset();owner.luminanceWritten=owner.conesWritten=false;return true;
}
}

#include "original_world_fog_custody50la.inl"

namespace ra_pod_scene50LA {
bool Owner::completeNativeParticleManager1B4DC0(const ra_cold_sources50LA::Owner& source,
    const std::vector<std::uint8_t>& authenticatedPrt,std::uint32_t& pc)noexcept {
    try {
    pc=0x1b4dc0;
    if(!nativeSceneRoles270900Current(source,pc)||!lighting_||!trackParticles_||
       !trackParticles_->prepared()||trackParticles_->application_!=sceneApplication_||
       trackParticles_->generation_!=sceneGeneration_||!trackParticles_->table_)return false;
    const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
    const auto* selector=lighting_->nativeRaParticleSelector1B4DC0(this,decoded->world,visibilityRows_,pc);
    if(!selector||*selector!="demoprt.tga")return false;
    //Actual nonnull application visibility takes1B4E14 directly. The earlier
    //native topology producer is real and MUST NOT be replayed here.
    pc=0x1b4e10;
    if(!nativeEnvironmentAliases273174_||!environmentMethod_||
       sceneApplication_->visibility443A98_!=environmentMethod_.get()||
       sceneApplication_->environment443108_!=&environment443108_.entity||
       !environmentMethod_->constructorStateOnly()||!environment443108_.attached)return false;
    const auto* component=trackParticles_->component();
    if(!component||component->kind!=g::OriginalWreckComponentKind50LA::particleManager160B10||
       component->reference00!=1u||component->sessionGeneration!=sceneGeneration_||
       component->objectKey!=key(component)||component->parent1C||component->previous10||
       component->next14||component->firstChild18||component->mask20||component->descendants24||
       trackParticles_->capacity130()!=5000||trackParticles_->depthBucketCount118()!=2500)return false;
    if(nativeParticleManager4A0_&&nativeParticleManager4A0_!=trackParticles_.get())return false;
    if(nativeParticleTexture41C_&&nativeParticleTexture41C_!=selector)return false;
    if(nativeParticleManagerComplete_){
        if(!nativeParticleManager4A0_||!trackParticles_->matchesNativeResources160E00(
            sceneApplication_.get(),trackParticles_->table_.get(),sceneGeneration_,trackParticles_->particles()))return false;
        pc=0x1b4eac;return true;
    }
    if(nativeParticleManagerAttempted_)return false;
    //Authenticate retained original PRT and real authored GlowScalar. This
    //read-only parser does not recreate the numeric atlas or pretend its
    //earlier native Config adaptation was a guest1577D0 execution receipt.
    std::vector<rr::assets::OriginalUiDatSection50KG> document;
    if(!g::readOriginalGcParticleText50KG(authenticatedPrt,9210u,
       "d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f",document))return false;
    const rr::assets::OriginalUiDatSection50KG* info=nullptr;
    for(const auto& section:document)if(section.name=="General Info"){
        if(info)return false;info=&section;
    }
    if(!info)return false;
    const auto* glow=info->find("GlowScalar");const auto* texture=info->find("TextureMap");
    const auto* frames=info->find("NumFrames");const auto* groups=info->find("NumGroups");
    if(!glow||glow->text!="128.0"||!texture||texture->text!=*selector||
       !frames||frames->text!="129"||!groups||groups->text!="9"||
       !trackParticles_->particles()->atlasTextureMapName()||
       *trackParticles_->particles()->atlasTextureMapName()!=*selector)return false;
    //1B4E54 publishes the SAME existing counted owner. Actual native early
    //pool/material ordering is adopted, not a second160B10/count or reset.
    nativeParticleManagerAttempted_=true;
    nativeParticleTexture41C_=selector;nativeParticleManager4A0_=trackParticles_.get();
    g::OriginalTrackParticleColdDiagnostic50LA detail;
    if(!trackParticles_->completeNativeResources160E00(128.f,detail)){pc=detail.sourcePC;return false;}
    nativeParticleManagerComplete_=true;pc=0x1b4eac;return true;
    }catch(...){return false;} //Retain reached ownership; never rely on string SSO.
}
}

namespace ra_pod_scene50LA {
bool Owner::completeNativeEmptyInstances274080(const ra_cold_sources50LA::Owner& source,
    const std::shared_ptr<rr::platform::OriginalUiMainApplication50KG>& application,
    std::uint32_t& pc)noexcept {
    pc=0x274080;
    try {
        if(!application||(nativeEmptyInstances3DC_&&nativeEmptyInstances3DC_->sourceMode)||
           !nativeSceneRoles270900Current(source,pc))return false;
        const auto* decoded=source.sourcesDecoded();if(!decoded)return false;
        const auto& document=decoded->world.parameters;
        if(document.original_bytes.size()!=540795u||document.blocks.size()!=1472u)return false;
        std::size_t fields=0;
        for(const auto& block:document.blocks){
            if(block.name=="instancemasters")return false;
            for(const auto& field:block.fields){
                ++fields;
                //Absent157D00 may retain prior selection. Complete document
                //absence of the count key proves158B30 default0 regardless.
                if(field.name=="numberofinstancemasters")return false;
            }
        }
        if(fields!=7922u)return false;
        if(nativeEmptyInstances3DC_){
            const auto& current=*nativeEmptyInstances3DC_;
            if(!current.complete||current.application!=application||!current.published||
               application->instanceMaster444078!=current.master||!canRetireNativeInstanceMaster())return false;
            pc=0x274158;return true;
        }
        nativeEmptyInstances3DC_.emplace();auto& role=*nativeEmptyInstances3DC_;
        role.application=application;
        //The earlier native World adoption omitted270FD0. At this actual
        //continuation, add its missing REAL resource in the same native global
        //domain. Constructor publishes no callback; native atomic construction
        //initializes count10 before global publication, unlike2BFD24/28.
        //This native resource-construction order remains distinct from retail;
        //the separate World3DC lane must come from fresh270550, not this domain.
        pc=0x270fe8;
        role.master=std::shared_ptr<rr::platform::OriginalUiGarageInstanceMaster50KG>(
            new rr::platform::OriginalUiGarageInstanceMaster50KG());
        const auto previous=application->instanceMaster444078;
        if(previous)application->overwrittenInstanceMasters.push_back(previous);
        pc=0x2bfd24;application->instanceMaster444078=role.master;role.published=true;
        //Actual absent158B30 default writes source3E0 AFTER global publication.
        //No allocation/store3DC occurs on this zero-count load branch: it
        //must retain the actual earlier fresh2705B4 known-null store.
        pc=0x271028;nativeInstanceCount3E0_=0;
        pc=0x274110;
        if(!nativeInstanceCount3E0_||*nativeInstanceCount3E0_)return false;
        pc=0x274120;
        if(!nativeInstanceDescriptors3DC_||*nativeInstanceDescriptors3DC_)return false;
        if(!canRetireNativeInstanceMaster())return false;
        //Zero loop skips2C06A0/2C07B0/141960/2BFDA0. Native descriptor role is
        //genuinely empty so274120..13C has no allocation to free or clear.
        role.complete=true;pc=0x274158;return true;
    }catch(...){return false;} //Pending owner/history/global effects remain owned; no retry replay.
}
bool Owner::canRetireNativeInstanceMaster()const noexcept {
    if(!nativeEmptyInstances3DC_)return true;
    const auto& role=*nativeEmptyInstances3DC_;
    if(role.sourceMode){
        //Native preflight only: accepts authentic nullable/partial source
        //construction, never grants legacy completed-instance readiness.
        if(!role.application||role.sourceWorld!=&worldEntity_||
           role.sourceGeneration!=worldEntity_.sessionGeneration||!role.sourceGeneration||
           role.sourceThread!=std::this_thread::get_id()||
           !nativeInstanceDescriptors3DC_||*nativeInstanceDescriptors3DC_)return false;
        if(role.complete&&(!role.sourceCallerPublished||!nativeInstanceCount3E0_||*nativeInstanceCount3E0_))return false;
        if(!role.master)return !role.sourceConstructorPublished&&
            (!role.sourceCallerPublished||role.sourceOuterReturned);
        const auto& m=*role.master;
        if(!m.sourceMode_||!m.sourceArrayReturned_||m.capacity04_!=10||m.growth08_!=100000||m.used0C_||
           (m.sourceCount10Known_&&m.count10_)||
           (role.sourceConstructorPublished&&!m.sourceCount10Known_)||
           (role.sourceCallerPublished&&!role.sourceConstructorPublished))return false;
        if(!role.published)return !role.sourceConstructorPublished&&!role.sourceCallerPublished;
        if(role.application->instanceMaster444078==role.master)return true;
        std::size_t retained=0;
        for(const auto& old:role.application->overwrittenInstanceMasters)if(old==role.master)++retained;
        return retained==1u;
    }
    if(!role.application||!nativeInstanceCount3E0_||*nativeInstanceCount3E0_||
       !nativeInstanceDescriptors3DC_||*nativeInstanceDescriptors3DC_)return false;
    if(!role.master)return !role.published&&!role.complete;
    const auto& master=*role.master;
    if(!master.records00_||master.capacity04_!=10||master.growth08_!=100000||
       master.used0C_||master.count10_)return false;
    if(!role.published)return !role.complete;
    if(role.application->instanceMaster444078==role.master)return true;
    //Another actual scene may have overwritten the shared global. Never
    //destroy that current owner or free the displaced backing retained there.
    std::size_t retained=0;
    for(const auto& old:role.application->overwrittenInstanceMasters)if(old==role.master)++retained;
    return retained==1u;
}
bool Owner::retireNativeInstanceMaster()noexcept {
    if(!canRetireNativeInstanceMaster())return false;
    if(!nativeEmptyInstances3DC_)return true;
    auto& role=*nativeEmptyInstances3DC_;
    if(role.master){
        if(role.published&&role.application->instanceMaster444078==role.master){
            //The actual current global points to OUR zero-record owner.
            //2BFD68 clear precedes raw backing free; no stale alias target.
            role.application->instanceMaster444078.reset();role.master->close2BFD40();
        }else if(!role.published){
            role.master->close2BFD40(); //Explicit native unpublished-failure cleanup only.
        }
        //If displaced, immutable existing history retains its real backing.
        //This native local cleanup does not run a foreign current-global dtor.
        role.master.reset();
    }
    nativeEmptyInstances3DC_.reset();return true;
}
}

namespace ra_pod_scene50LA {
bool Owner::adoptNativeStaticZone2741C0(const ra_cold_sources50LA::Owner& source,
    const ra_static_model_cold50LA::Owner* models,std::size_t zone,std::uint32_t& pc)noexcept {
    pc=0x27419c;
    if(!nativeSceneRoles270900Current(source,pc)||!lighting_||zone>=10u)return false;
    const auto* data=source.sourcesDecoded();if(!data||data->world.zones.size()!=10u)return false;
    const auto& z=data->world.zones[zone];
    if(z.zone!=zone+1u||z.parameter_block>=data->world.parameters.blocks.size())return false;
    const Field* value=nullptr;
    if(!findUniqueField(data->world.parameters.blocks[z.parameter_block],"environmentnodevisible",value)||value)
        return false;
    //275EF4 absent-key default1 populates source zone148 bit1.2741B8/BC
    //DSLL32(30),DSRL32(31) extract precisely bit1, not descriptor148 bit0.
    //Existing native projection already owns this value. No second node44.
    if(!lighting_->nativeStaticZone2741C0Current(this,models,data->world,visibilityRows_,zone,pc))return false;
    nativeModelZone354_=static_cast<std::uint32_t>(zone);return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::continueNativeStandardModels274160(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa1f4;
    try {
        if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||nativeStandardServiceTerminal_||
           !nativeGeometryDirectory1AA1DC_||!nativeEmptyInstances3DC_||!nativeEmptyInstances3DC_->complete||app.modelDirectory36B860()!=(nativeCollisionAnimationDirectory1AA2A0_?"anim\\":"geometry\\")||
           !nativeSceneInfo270900Current(app,pc)||!world_||world_->closed_||world_->source_!=source_||
           !world_->rendererPublication_||!world_->imagePublication_||!world_->modelPrefixes_||
           !world_->domain_||!world_->context_||!world_->context_->live()||
           runtimeCameraBorrows_||runtimeWorldBorrows_||!runtimeQueries_)return false;
        g::OriginalRaceFogFields* fog=nullptr;
        if(!runtimeQueries_->readNativeColdFog273C30(*worldApplication_,*source_,
            worldEntity_.sessionGeneration,fog)||!nativeFogCustodyCurrent(fog))return false;
        const auto display=runtimeQueries_->nativeColdModelDisplay274160(*worldApplication_,*source_,
            worldEntity_.sessionGeneration);
        const auto* images=world_->imagePublication_;
        std::array<std::int32_t,4> coldRectangle{};std::uint64_t coldSelectionRevision=0;
        if(!display||display->application_!=&app||!runtimeQueries_->camera_||
           !runtimeQueries_->camera_->session||
           world_->rendererPublication_!=&display->renderer_||
           !display->readColdGameplayViewport115990(*runtimeQueries_->camera_->session,coldRectangle,coldSelectionRevision)||
           images->raStaticWorldOwner()!=world_.get()||!images->publicationToken()||
           !images->validBorrow(&display->video_->host,&original_main_entry50KG::RaceDisplayLifetime50LA::current,
               images->publicationToken()))return false;
        //Both pre-existing publications remain retained. Only descriptor/name/
        //sourceflags metadata changes; geometry/texture/buffer pointers do not.
        auto& owner=*world_->modelPrefixes_;const auto* data=source_->sourcesDecoded();
        const auto& provider=world_->domain_->archiveProvider3E1A70();
        const auto& resolver=world_->domain_->resolver3E1A70();
        if(!data||data->world.instances.size()!=1261u||data->world.zones.size()!=10u||
           !provider||!provider->sealed()||provider->revision()!=world_->providerRevision_||!resolver||
           !owner.canClose()||owner.closed_||owner.resolver_!=resolver||owner.source_!=source_||owner.application_!=worldApplication_||
           owner.generation_!=worldEntity_.sessionGeneration||owner.parents_.size()!=10u||
           owner.models_.size()!=1085u||owner.unresolved_!=176u||world_->collisionOnly_.size()!=176u||
           nativeStandardZoneNext_>10u)return false;
        if(nativeStandardServiceComplete_)return continueNativeFinishLine1B3B80(app,pc);
        //Terminal native continuation: source27B930 ignores callback errors,
        //but native exceptions/unsupported custody must never replay effects.
        nativeStandardServiceTerminal_=true;
        while(nativeStandardZoneNext_<10u){const auto zone=nativeStandardZoneNext_;
            const auto& parent=owner.parents_[zone];
            if(!parent||!parent->constructed||parent->released||parent->zone!=zone+1u||
               parent->count30!=parent->entries34.size()||
               parent->count30!=data->world.zones[zone].models||!parent->count30||
               !adoptNativeStaticZone2741C0(*source_,&owner,zone,pc))return false;
            for(;nativeStandardSlotNext_<parent->entries34.size();++nativeStandardSlotNext_){
                const auto ordinal=nativeStandardSlotNext_;const auto& slot=parent->entries34[ordinal];
                if(slot.worldInstance>=data->world.instances.size())return false;
                const auto& instance=data->world.instances[slot.worldInstance];
                if(instance.zone!=zone+1u||instance.ordinal!=ordinal+1u||
                   instance.parameter_block>=data->world.parameters.blocks.size())return false;
                ra_static_model_cold50LA::Owner::Diagnostic diagnostic;
                pc=0x27ab60;
                if(!owner.prepareNativeStandardSlot27AB60(zone,ordinal,diagnostic))return false;
                std::string absent;
                if(!slot.model){
                    const ra_static_world50LA::Owner::CollisionOnly* actual=nullptr;
                    for(const auto& c:world_->collisionOnly_)if(c.worldInstance==slot.worldInstance){
                        if(actual)return false;actual=&c;}
                    if(!actual||actual->zone!=instance.zone||actual->ordinal!=instance.ordinal||
                       actual->absentGeometry!=instance.geometry_member||actual->presentCollision!=instance.collision_member)
                        return false;
                    const Field* slt=nullptr;
                    if(!findUniqueField(data->world.parameters.blocks[instance.parameter_block],"slt",slt)||!slt)return false;
                    auto stem=slt->value;const auto first=stem.find('.');
                    stem.resize(std::min<std::size_t>(first==std::string::npos?stem.size():first,50u));
                    if(stem.empty())return false;
                    for(char& c:stem){if(static_cast<unsigned char>(c)>=128u||!c)return false;
                        if(c>='A'&&c<='Z')c=static_cast<char>(c+'a'-'A');}
                    absent=std::string("geometry\\")+stem+".psg";
                    if(absent!=instance.geometry_member)return false;
                    //Actual unretired filename/cache rows BEFORE mounted-bank
                    //absence. Other lookup errors are NOT successful misses.
                    rr::assets::OriginalStandardModelFileView50LA file;
                    rr::assets::OriginalStandardTextureDiagnostic50LA lookup;
                    rr::assets::OriginalStandardArchiveProvider50LA::MemberView member;
                    pc=0x132820;
                    if(resolver->lookupModel132820(absent,rr::assets::OriginalStandardModelFileKind50LA::psg,file,lookup)||
                       lookup.status!=rr::assets::OriginalStandardTextureStatus50LA::unresolved||
                       lookup.sourcePC!=0x13294cu||
                       provider->lookup132820(absent,member)!=rr::assets::OriginalStandardArchiveProvider50LA::Lookup::absent)
                        return false;
                    //Explicit native mounted-namespace transport adaptation.
                    //Guest+170 fallback/cursors/diagnostics remain unrepresented.
                }
                pc=0x27abc0;const auto beforeCount=worldApplication_->objectCount423F60();
                if(!owner.completeNativeStandardSlot27ABC0(zone,ordinal,absent,diagnostic)||
                   worldApplication_->objectCount423F60()!=beforeCount)return false;
                if(slot.model)++nativeStandardVisuals_;else ++nativeStandardNegativeLoads_;
            }
            //Actual+98=274250 examines this SAME list. All authored default
            //Standard wrappers now genuinely exist/type1; no type2 callback.
            pc=0x274250;
            for(std::size_t i=0;i<parent->entries34.size();++i)
                if(!owner.nativeStandardSlotCurrent27ABC0(zone,i))return false;
            nativeStandardSlotNext_=0;++nativeStandardZoneNext_;
        }
        if(nativeStandardVisuals_!=1085u||nativeStandardNegativeLoads_!=176u||
           !nativeModelZone354_||*nativeModelZone354_!=9u||!owner.canClose())return false;
        nativeStandardServiceComplete_=true;nativeStandardServiceTerminal_=false;
        return continueNativeFinishLine1B3B80(app,pc);
    }catch(...){nativeStandardServiceTerminal_=true;return false;}
}
}

namespace original_main_entry50KG {
bool RaceDisplayLifetime50LA::readColdGameplayViewport115990(
    const original_camera_session50KX::Session& camera,
    std::array<std::int32_t,4>& rectangle,std::uint64_t& revision)const noexcept {
    //The post-bind115990 getter intentionally requires coldBound_. This new
    //private caller is ONLY actual phase1 before that bind; do not relax it.
    if(!gameplayDisplayIdentityCurrent50LA()||
       selectedGameplayCamera0_!=&camera||!gameplaySelectionRevision_||
       gameplaySelectionRevision_==std::numeric_limits<std::uint64_t>::max())return false;
    const auto selected=camera.selectedGameplayDisplay_.lock();
    if(selected.get()!=this||!selected->applicationLifetime_.sameOwner(applicationLifetime_)||
       camera.closed_||camera.abandoned_||camera.coldBound_||camera.coldPhase_!=1u||
       !camera.coldViewport_.complete()||!camera.coldCameraSelected26F940_||
       camera.frameStage_!=original_camera_session50KX::Session::FrameStage::idle||
       camera.publicationCount_||camera.fogPreparationRevision_||camera.fogPreparation_.owner||
       !camera.displayInputs_||&camera.displayInputs_->renderer!=&renderer_||
       &camera.displayInputs_->rendererDispatch!=&dispatch_||camera.display_.rendererDispatch!=&dispatch_||
       camera.actual_.applicationEntities!=application_->entitiesOwner_||
       !camera.actual_.applicationAnimation.sameOwner(applicationLifetime_)||
       camera.display_.rendererCurrentScene0!=&camera.viewport_||!camera.viewport_.prepared||
       !camera.viewport_.matricesReady||camera.viewport_.sessionGeneration!=camera.generation_||
       camera.viewport_.sceneKey!=camera.key(camera.scene_)||
       camera.viewport_.x1E0||camera.viewport_.y1E4||camera.viewport_.width1E8!=640||camera.viewport_.height1EC!=448||
       camera.viewport_.fullWidth204!=640||camera.viewport_.fullHeight208!=448||
       camera.displayInputs_->rendererWidth4!=640||camera.displayInputs_->rendererHeight8!=448)return false;
    rr::platform::VehicleWindowDiagnostic50DS diagnostic;
    if(!renderer_.idleForRaceReset50LA(diagnostic))return false;
    rectangle={{camera.viewport_.x1E0,camera.viewport_.y1E4,camera.viewport_.width1E8,camera.viewport_.height1EC}};
    revision=gameplaySelectionRevision_;return true; //No selection/reset or stored borrowed pointer.
}
}

#include "original_world_finishline_custody50la.inl"

#include "original_world_collision_service50la.inl"

#include "original_world_collision_observer50la.inl"

#include "original_world_sound_list_custody50la.inl"

#include "original_world_particle_custody50la.inl"
#include "original_world_particle_services50la.inl"

#include "original_world_trigger_observer50la.inl"
#include "original_world_trigger_services50la.inl"

namespace original_main_entry50KG {
bool RaceDisplayLifetime50LA::nativeSkyMetadataCurrent129280(
    const original_camera_session50KX::Session& camera,const rr::assets::Mesh48B& mesh,
    const rr::assets::MaterialTextureAssets50EA& bank)const noexcept {
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    if(!readColdGameplayViewport115990(camera,rectangle,revision))return false;
    rr::platform::VehicleWindowDiagnostic50DS diagnostic;
    return renderer_.raSkySourceCurrent129280(mesh,bank,this,diagnostic);
}
}
#include "original_world_sky_prefix50la.inl"

#include "original_world_sky_textures50la.inl"

#include "original_world_sky_command_custody50la.inl"
#include "original_world_sky_geometry_custody50la.inl"
#include "original_world_sky_tail50la.inl"
#include "original_world_water_none50la.inl"

#include "original_race_world_native_tail50la.inl"
