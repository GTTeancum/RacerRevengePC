// Common rewrite R7: the 1A9EE0 sub-steps for particles (05.06), materials
// (05.08), water (05.10) and the AUB (05.12), for every course (PLAN B-C15,
// B-C16, B-C17; declarations frozen in common_track50cr.h).
//
// Each step reads only the race's Track (its archive, decoded SCB/TOB/AUB and
// the role owners published before it), sizes what it stores from counts read
// from those files, publishes its owner into Track::roles, logs one common_load
// line plus its common_track_data lines, and refuses (Diagnostic) rather than
// guessing. None branches on the course code; the code is data (log lines,
// receipt lookup). Stock values are receipts only (stock_course_receipts50cr.h).
// The one branch on data is A9: water exists only when the SCB Environment
// names a WaterFile other than "none" (274570 none / 2745D4 water factory).
#include "common_track50cr.h"
#include "common_race_log50cr.h"
#include "stock_course_receipts50cr.h"
#include "original_ui_main_entry50kg.h"
#include "native_ta_particle_cold50la_fixture.h"
#include "track_particle_emitters50cr.h"
#include "material_alpha50gx_fixture.h"
#include "course_world_texture50st.h"
#include "single_pass_world50lm.h"            // R3: single_pass_world50LM::Owner (roles.staticWorld)
#include "water_resources50jc_fixture.h"
#include "native_water_runtime50jc_fixture.h"
#include "material_audio_provider50hi_fixture.h"
#include "positional_sound_rows50sr.h"
#include "runtime_asset_paths50lb.h"
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/track_height_descriptor50jb.h"
#include "../../src/assets/material_texture_assets50ea.h"
#include "../../src/assets/original_ui_material50kg.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"
#include <exception>
#include <memory>
#include <string>
#include <string_view>

namespace common_track50CR {
namespace {
namespace receipts=stock_course_receipts50CR;

// One common_track_data line for a base role; the receipt is looked up only
// for a stock course (add-on courses print receipt=none). Never sizes anything.
void trackData(const Track& t,log::Role role,std::string_view file,std::uint64_t value){
    const auto* r=t.source()==Source::stock?receipts::find(t.code(),log::roleName(role)):nullptr;
    log::trackData(role,t.code(),file,value,r!=nullptr,r?r->value:0);
}
// The same for a dotted sub-role ("aub.collision_rows").
void trackDataNamed(const Track& t,std::string_view role,std::string_view file,std::uint64_t value){
    const auto* r=t.source()==Source::stock?receipts::find(t.code(),role):nullptr;
    log::trackDataNamed(role,t.code(),file,value,r!=nullptr,r?r->value:0);
}
// A hash receipt is logged as a common_track_data line whose value is 1 when
// the stock digest matches (receipt=match), 0 when it differs (differs(1)).
void hashData(const Track& t,std::string_view role,std::string_view file,const std::string& sha){
    const auto* r=t.source()==Source::stock?receipts::findHash(t.code(),role):nullptr;
    log::trackDataNamed(role,t.code(),file,r?(r->sha256==sha?1u:0u):0u,r!=nullptr,1u);
}
bool refuse(Diagnostic& d,log::Step step,std::uint32_t pc,std::string why){
    log::load(step,false,d.value,why,nullptr,pc);
    d.refuse(step,pc,std::move(why));
    return false;
}
std::string what(const std::exception& e){try{return e.what();}catch(...){return "exception";}}

// The one application-lifetime water BSS (443BB0 swell clock, 8D38 lazy guard,
// 443BB8 sentinel): host-side memory owned here, shared by every race's water
// runtime like the original's one global; never game memory. A new race's
// runtime is initialized only after the previous one is released.
native_water_runtime50JC::Application& waterApplication443BB0(){
    static native_water_runtime50JC::Application application;
    return application;
}
} // namespace

// ------------------------------------------------------------------ 05.06
// 1B4DC0 particle manager + 274370 services over the 27DE00 reader.
bool prepareParticles1B4DC0(Track& t,const LoadInputs& in,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::particles1B4DC0;
    d.value=0;
    try{
        if(t.roles.particles||t.roles.emitters)return refuse(d,step,0x1B4DC0u,"particle manager already published");
        if(!in.application)return refuse(d,step,0x1B4DC0u,"particle manager needs the Application's material domain (none in this load)");
        if(!in.materials382B38||!in.generation)return refuse(d,step,0x1B4DC0u,"particle manager needs the race's 382B38 table and generation");
        auto owner=std::make_shared<rr::game::OriginalTrackParticleCold50LA>();
        rr::game::OriginalTrackParticleColdDiagnostic50LA pd;
        if(!native_ta_particle_cold50LA::prepare(*owner,t,*in.application,in.materials382B38,in.generation,pd))
            return refuse(d,step,pd.sourcePC?pd.sourcePC:0x1B4DC0u,
                "track particle manager rejected status="+std::to_string(static_cast<unsigned>(pd.status)));
        const auto capacity=static_cast<std::uint64_t>(owner->capacity130());
        t.roles.particles=std::move(owner);
        d.value=capacity;
        // 274370 over the TOB [ParticleEmitters] table (27DE00/27C600/27C7B0): the
        // ONE ground-effects owner (track_particle_emitters50cr.h). It reads the rows,
        // EMT definitions and SoulTree placements, reserves the 15AEA0 registry in the
        // manager above and emits by each row's TypeID. It is published whenever the
        // manager exists, also with zero rows, so the one manager advance (161180)
        // runs every frame on every course. It logs particles.emitters/.attachments/
        // .definitions itself.
        {
            const auto& process=in.application->state;
            if(!process||!process->clock101A50)return refuse(d,step,0x27DE00u,"ground effects need the Application process and its 101A50 clock");
            track_particle_emitters50CR::Inputs inputs;
            inputs.manager=t.roles.particles;inputs.process=process;inputs.clock=process->clock101A50;
            // PORT STAND-IN (flagged): LoadInputs carries no process particle-globals
            // pointer this round (w0-requestsound3-E.txt 1); the owner keeps its own
            // TypeID 11 speed scale until it does.
            inputs.globals=nullptr;
            inputs.generation=in.generation;
            auto emitters=std::make_shared<track_particle_emitters50CR::Owner>();
            Diagnostic ed;
            if(!emitters->prepare(t,inputs,ed))
                return refuse(d,step,ed.pc?ed.pc:0x27DE00u,"ground effects: "+ed.reason);
            t.counts.particleEmitters=static_cast<std::uint32_t>(emitters->emitterCount());
            t.counts.particleAttachments=static_cast<std::uint32_t>(emitters->attachedCount());
            t.roles.emitters=std::move(emitters);
        }
        const auto* name=native_ta_particle_cold50LA::particleTextureMapName(t.documents.world);
        trackDataNamed(t,"particles.manager_capacity",name?*name:std::string_view{},capacity);
        trackDataNamed(t,"particles.depth_buckets",name?*name:std::string_view{},
            static_cast<std::uint64_t>(t.roles.particles->depthBucketCount118()));
        trackData(t,log::Role::particles,name?*name:std::string_view{},t.counts.particleEmitters);
        log::load(step,true,capacity);
        return true;
    }catch(const std::exception& e){return refuse(d,step,0x1B4DC0u,what(e));}
     catch(...){return refuse(d,step,0x1B4DC0u,"particle manager exception");}
}

// ------------------------------------------------------------------ 05.08
// 274160 materials, 14D370/29E340 textures. The one decoder is the static
// world's single-pass owner (05.07, R3); this step runs the one material rule
// (selectMaSa, with authored AA1) over every pass of every selection at cold,
// so a pass the native renderer cannot represent refuses the load here, and
// logs the one texture budget.
bool readMaterials274160(Track& t,const LoadInputs& in,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::materials274160;
    d.value=0;
    try{
        const auto* world=t.roles.staticWorld.get();
        if(!world||!world->loaded())return refuse(d,step,0x274160u,"static world (05.07) not loaded");
        const auto* selections=world->selections();const auto* materials=world->materials();const auto* bank=world->bank();
        if(!selections||!materials||!bank)return refuse(d,step,0x274160u,"static world material tables unavailable");
        // The APPSTART DefaultMT template the 2476A0 rule inherits from (the one
        // application-lifetime APPSTART owner; never replaced on a race restart).
        if(!in.application)return refuse(d,step,0x2476A0u,"material rule needs the Application's APPSTART DefaultMT template (none in this load)");
        const auto appStart=in.application->appStart(runtime_asset_paths50LB::resolve("work/native/project/disc"));
        if(!appStart)return refuse(d,step,0x2476A0u,"APPSTART archive unavailable for the DefaultMT template");
        rr::assets::OriginalUiMaterialTemplates50KG templates;rr::assets::OriginalUiMaterialDiagnostic50KG md;
        if(!templates.load(*appStart,md))return refuse(d,step,0x2476A0u,"APPSTART DefaultMT template rejected");
        std::uint64_t secondPasses=0,aa1Passes=0,blended=0,alphaTested=0,destinationAlpha=0;
        for(std::size_t i=0;i<selections->size();++i){
            const auto& selection=(*selections)[i];
            material_alpha50gx_fixture::Policy policy;
            if(!world->alpha(i,templates,in.application->materialRuntime,policy))
                return refuse(d,step,0x2476A0u,"material pass outside the one material rule: selection="+std::to_string(i)+
                    " material="+selection.materialName+" pass="+std::to_string(selection.pass)+"/"+
                    std::to_string(selection.passCount)+" program="+selection.program);
            secondPasses+=selection.pass==2;aa1Passes+=policy.onePassAa1;
            blended+=policy.blend!=material_alpha50gx_fixture::Blend::opaque;alphaTested+=policy.sourceAlphaTest;
            destinationAlpha+=policy.gsDestinationAlphaPass!=0;
        }
        t.counts.materials=static_cast<std::uint32_t>(materials->size());
        t.counts.textures=bank->textureCount();
        // One texture budget (course_world_texture50st.h): sized from what the
        // course loaded, checked against the one ceiling, logged.
        const auto budgets=course_world_texture50ST::textureBudgets50ST();
        log::storage("texture_bank",bank->textureCount(),bank->rgbaByteCount(),budgets.rgbaByteCapacity);
        log::storage("texture_sources",bank->textureCount(),bank->sourceByteCount(),budgets.sourceByteCapacity);
        if(bank->textureCount()>budgets.textureCapacity||bank->rgbaByteCount()>budgets.rgbaByteCapacity||
           bank->sourceByteCount()>budgets.sourceByteCapacity){
            log::storageRefused("texture_bank",bank->rgbaByteCount(),budgets.rgbaByteCapacity,0x29E340u);
            return refuse(d,step,0x29E340u,"texture bank above the one texture budget ceiling");
        }
        d.value=t.counts.materials;
        trackData(t,log::Role::materials,t.identity.scbMember,t.counts.materials);
        trackDataNamed(t,"materials.count",t.identity.scbMember,t.counts.materials);
        trackDataNamed(t,"materials.selections",t.identity.scbMember,selections->size());
        trackDataNamed(t,"materials.second_passes",t.identity.scbMember,secondPasses);
        trackDataNamed(t,"materials.textures",t.identity.scbMember,t.counts.textures);
        trackDataNamed(t,"storage.texture_rgba_bytes",t.identity.scbMember,bank->rgbaByteCount());
        // AA1 is authored data on each pass (PLAN B-C15): every course now
        // applies it where it is authored (no receipt row; PLAN 5.6 expects
        // TA/TC/MA/MB/MC/SA/SC to gain it).
        trackDataNamed(t,"materials.aa1_passes",t.identity.scbMember,aa1Passes);
        trackDataNamed(t,"materials.blended_passes",t.identity.scbMember,blended);
        trackDataNamed(t,"materials.alpha_tested_passes",t.identity.scbMember,alphaTested);
        trackDataNamed(t,"materials.destination_alpha_passes",t.identity.scbMember,destinationAlpha);
        log::load(step,true,t.counts.materials);
        return true;
    }catch(const std::exception& e){return refuse(d,step,0x274160u,what(e));}
     catch(...){return refuse(d,step,0x274160u,"material step exception");}
}

// ------------------------------------------------------------------ 05.10
// 274570 (no water) / 2745D4 (water factory), A9: the original branches on
// whether the SCB Environment WaterFile names water ("none" = no water, exact
// case-sensitive compare 2745B0).
bool prepareWater274570(Track& t,const LoadInputs& in,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::water274570;
    (void)in;d.value=0;
    try{
        if(t.roles.water||t.roles.waterResources)return refuse(d,step,0x274570u,"water already published");
        rr::assets::TrackHeightDescriptor50JB descriptor{};
        const auto read=rr::assets::readTrackHeightDescriptor50JB(t.documents.world.parameters,descriptor);
        if(read!=rr::assets::TrackHeightReadStatus50JB::Complete)
            return refuse(d,step,0x2771D4u,"SCB Environment WaterFile unreadable status="+
                std::to_string(static_cast<unsigned>(read)));
        const std::string water=descriptor.waterFile110.value.data();
        if(water=="none"){
            // 274570: no water. Nothing is created (A9); roles stay null.
            t.counts.water=false;t.documents.waterName.clear();t.documents.waterLogicalOffset=0;
            trackDataNamed(t,"water.present",t.identity.scbMember,0);
            trackData(t,log::Role::water,t.identity.scbMember,0);
            log::load(step,true,0,"274570 none");
            return true;
        }
        // 2745D4: the WaterFile resolves beside the scene member (1B38DC).
        const auto* archive=t.archive.decoded();
        if(!archive)return refuse(d,step,0x2745D4u,"course archive not decoded");
        const auto slash=t.identity.scbMember.find_last_of("/\\");
        if(slash==std::string::npos)return refuse(d,step,0x1B38DCu,"track scene directory unavailable");
        const std::string name=t.identity.scbMember.substr(0,slash+1)+water;
        std::vector<std::uint8_t> wtt;water_resources50JC::Member identity;std::string error;
        if(!water_resources50JC::member(*archive,name,wtt,identity,error))return refuse(d,step,0x2745D4u,error);
        std::shared_ptr<water_resources50JC::Owner> resources;
        if(!water_resources50JC::prepare(*archive,t.archive.sha256().c_str(),identity.name,identity.logicalOffset,wtt,resources,error))
            return refuse(d,step,0x2745D4u,error);
        auto runtime=std::make_shared<native_water_runtime50JC::Owner>();
        // One native view (the original 274610..640 viewport-mode0 layout).
        runtime->initialize(waterApplication443BB0(),resources,native_water_runtime50JC::Policy::OneNativeView);
        t.counts.water=true;t.documents.waterName=identity.name;t.documents.waterLogicalOffset=identity.logicalOffset;
        const auto signatures=static_cast<std::uint64_t>(resources->signatures.size());
        t.roles.waterResources=std::move(resources);t.roles.water=std::move(runtime);
        d.value=signatures;
        trackDataNamed(t,"water.present",identity.name,1);
        trackData(t,log::Role::water,identity.name,signatures);
        trackDataNamed(t,"water.noise.bytes",t.roles.waterResources->noiseMember.name,t.roles.waterResources->noiseMember.sourceBytes);
        hashData(t,"water.noise.sha256",t.roles.waterResources->noiseMember.name,t.roles.waterResources->noiseMember.sha256);
        log::load(step,true,signatures,"2745D4 "+identity.name);
        return true;
    }catch(const std::exception& e){return refuse(d,step,0x2745D4u,what(e));}
     catch(...){return refuse(d,step,0x2745D4u,"water step exception");}
}

// ------------------------------------------------------------------ 05.12
// 271600/271674..271804 AUB material audio (+279B60/279E50 rows) and the
// [Sounds] positional lists (279910/2793A0) from the Track's own AUB.
bool readAub271600(Track& t,const LoadInputs& in,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::aub271600;
    d.value=0;
    try{
        if(t.roles.audio||t.roles.sounds)return refuse(d,step,0x271600u,"AUB owners already published");
        if(!in.mixer||!in.audioGroups)return refuse(d,step,0x271600u,"AUB owners need the race mixer and audio groups");
        if(t.documents.aub.empty()){
            // The AUB member is read once, at its own size (bounded by the
            // decoder's own input cap), and kept with the Track's documents.
            Diagnostic member;
            if(!t.archive.member(t.identity.aubMember,t.documents.aub,2u*1024u*1024u,member))
                return refuse(d,step,0x271600u,"course AUB member unavailable: "+t.identity.aubMember+
                    (member.reason.empty()?std::string{}:" ("+member.reason+")"));
        }
        const auto& aub=t.documents.aub;
        trackDataNamed(t,"aub.member_bytes",t.identity.aubMember,aub.size());
        hashData(t,"aub.sha256",t.identity.aubMember,pod_materials50ed_fixture::detail::sha256(aub));
        auto provider=std::make_shared<material_audio50HI::Provider>();std::string error;
        if(!provider->prepareTrack(aub,t.code(),in.audioRoot,*in.mixer,material_audio50HI::baseFirstVoice,error))
            return refuse(d,step,0x271600u,"material audio: "+error);
        t.counts.aubRows=static_cast<std::uint32_t>(provider->collisionRowCount()+provider->driveRowCount());
        trackData(t,log::Role::aub,t.identity.aubMember,t.counts.aubRows);
        trackDataNamed(t,"aub.collision_rows",t.identity.aubMember,provider->collisionRowCount());
        trackDataNamed(t,"aub.drive_rows",t.identity.aubMember,provider->driveRowCount());
        trackDataNamed(t,"aub.materials",t.identity.aubMember,provider->materialCount());
        trackDataNamed(t,"aub.unique_clips",t.identity.aubMember,provider->uniqueClipCount());
        auto sounds=std::make_shared<positional_sound_rows50SR::Owner>();
        if(!sounds->prepare(t,*provider,in.audioRoot,*in.mixer,*in.audioGroups,error))
            return refuse(d,step,0x279910u,"positional sounds: "+error);
        t.counts.positionalSounds=static_cast<std::uint32_t>(sounds->descriptorCount());
        trackDataNamed(t,"sound.positional.rows",t.identity.aubMember,sounds->descriptorCount());
        trackDataNamed(t,"sound.positional.ambient",t.identity.aubMember,sounds->ambientCount());
        t.roles.audio=std::move(provider);t.roles.sounds=std::move(sounds);
        d.value=t.counts.aubRows;
        log::load(step,true,t.counts.aubRows);
        return true;
    }catch(const std::exception& e){return refuse(d,step,0x271600u,what(e));}
     catch(...){return refuse(d,step,0x271600u,"AUB step exception");}
}

} // namespace common_track50CR
