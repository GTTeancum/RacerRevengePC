#include "original_ui_garage_actors50gn.h"
#include "original_ui_main_session50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_scene_collision50kg.h"
#include "original_ui_garage_static_models50kg.h"
#include "original_ui_garage_renderer_lease50kg.h"
#include "../../../tests/native/original_ui_source_timing50lh.h"
#include "../../../src/game/original_camera_start_effects.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/game/original_racer_animation_cold50la.h"
#include <array>
#include <exception>
#include <cstring>
#include <vector>
#include <utility>

namespace rr::platform {
namespace {
using C=game::OriginalWreckComponent50LA;
using K=game::OriginalWreckComponentKind50LA;
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool fail(D& d,S status,std::uint32_t pc)noexcept {
    d.status=status;d.sourcePC=pc;return false;
}
std::string lower(std::string value) {
    for(auto& c:value)if(c>='A'&&c<='Z')c=static_cast<char>(c+32);
    return value;
}
game::OriginalMotionConfiguration50LA configuration(const assets::OriginalUiDat50KG& source) {
    game::OriginalMotionConfiguration50LA out;
    for(const auto& section:source.sections()){
        auto& target=out[lower(section.name)];
        //158648 returns the first present matching value, not the last one.
        for(const auto& value:section.values)if(value.sourceValuePresent)
            target.emplace(lower(value.key),value.text);
    }
    return out;
}
}

struct OriginalUiGarageActors50GN::Storage {
    struct LoopSoundList {
        C entity;
        std::int32_t count30=0;
        const void* rows34=nullptr; //279604; no positive wrapper array admitted.
        bool constructed=false,readComplete=false,readFalse=false,released=false;
    };
    struct Actor {
        std::uint32_t type00=0; //Actual Toy row discriminator, read before late population.
        std::optional<const C*> auxiliary60; //2D83B0, then2D8FB8/2D8FEC; not Character1B8.
        std::unique_ptr<LoopSoundList> loopSound60; //Stable retired backing, not a live60 after readfalse.
        const assets::OriginalUiGarageActorAsset50KG* sourceToy=nullptr;
        assets::OriginalUiDatSection50KG sourceDescriptor; //Owned early TOY read, survives272800.
        C entity290;
        const C* primary4C=nullptr;
        const C* secondary50=nullptr;
        game::OriginalRacerAnimationCold50LA base;
        game::OriginalGarageCharacterFields50GN fields;
        std::shared_ptr<R> model;
        game::OriginalModelMotionSet50LA motions;
        game::ModelPlayback50KG playback;
        std::array<game::ModelPlaybackMotion50KG,4> descriptors{};
        assets::MotionNodeBindings49O rows{};
        game::ModelPlaybackIk50KG ik{};
        assets::OriginalStandardModelFileId50LA mcfId;
        std::unique_ptr<game::OriginalMotionConfiguration50LA> configuration08;
        std::vector<assets::MotionNodeBinding49O> records1EC;
        std::vector<game::BodyMatrix48G> saved1F4;
        bool entityLive=false,modelEnrolled=false,attached=false,cachePublished=false;
        bool membership=false,initialPose=false,retiring=false;
    };
    OriginalUiGaragePodScene50KG* sourceSceneOwner=nullptr;
    std::shared_ptr<Grant> grant;
    std::shared_ptr<const RootInputs> rootInputs;
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    std::shared_ptr<game::OriginalMotionLibrary50LA> motionLibrary;
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility;
    std::shared_ptr<OriginalUiMainAudio50KG> audio;
    OriginalUiGarageDroidAudio50GN* droids=nullptr;
    std::shared_ptr<assets::OriginalStandardTextureResolver50LA> files;
    OriginalUiGarageModelResources50KG* resources=nullptr; // Session outlives this owner.
    C toyList3C8;
    std::uint32_t toyCount30=0;
    std::array<Actor*,game::garageActorCount50GN> records34{};
    game::OriginalGarageAnimationManagerFields50GN manager49C;
    std::array<Actor,game::garageActorCount50GN> actors;
    game::OriginalGarageSequence50GN sequence;
    game::ModelPlaybackWorkspace50KG work;
    std::shared_ptr<const RuntimeFrame> pendingFrame;
    ::original_ui_source_timing50LH::PublicationView runtimeTiming;
    game::OriginalApplicationAnimationClockView50KG runtimePause;
    std::uint64_t lastRuntimePublication=0;
    RuntimeReceipt runtimeReceipt{};
    C* nextRuntimeChild=nullptr;
    bool runtimeStarted=false,runtimeFailed=false;
    // Minted only after the real callback failed and Sequence's stack guard
    // returned. This records effects, not permission to sample them again.
    bool failedSample10179C=false,failedSampleFrameReleased=false,failedSampleAudioPreflight=false;
    std::uint32_t failedSampleActor=0;
    std::uint64_t failedSamplePublication=0;
    const RuntimeFrame* failedSampleFrameIdentity=nullptr;
    const OriginalUiGaragePageOwner50KG* failedSampleRetirementOwner=nullptr;
    Stage stage=Stage::empty;
    std::size_t ordinal=0,constructed=0,completed=0;
    std::size_t loopSoundDescriptor=0,retiredLoopSoundLists=0;
    bool toyLive=false,managerLive=false,managerPublished=false,failed=false,closed=false;
    bool sourceOrdered=false,managerAttempted=false,toyRetiring=false;
    enum class PageRetirement {none,characters,listReference,descriptors,listBase,toyRetired,complete};
    PageRetirement pageRetirement=PageRetirement::none;
    const OriginalUiGaragePageOwner50KG* pageRetirementOwner=nullptr;
    const OriginalUiGaragePageOwner50KG* failedLateBorrowRetirementOwner=nullptr;
    const OriginalUiGaragePageOwner50KG* failedLoadedPageRetirementOwner=nullptr;
    const OriginalUiGaragePageOwner50KG* failedWattoBindingRetirementOwner=nullptr;
    std::size_t nextRetirementActor=game::garageActorCount50GN;
    std::size_t retiredDescriptors=0;
};

OriginalUiGarageActors50GN::OriginalUiGarageActors50GN()noexcept=default;
OriginalUiGarageActors50GN::~OriginalUiGarageActors50GN() {
    // An implicit vector clear would silently drop actual Entity references.
    // The Session must first close while its grant/Application remain live.
    if(storage_&&!storage_->closed)std::terminate();
}

bool OriginalUiGarageActors50GN::constructToyDescriptors2713F8(const assets::OriginalUiDat50KG& toy,
    std::shared_ptr<Grant> grant,
    OriginalUiGarageModelResources50KG& resources,
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,D& d)noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x2713F8);
    if(!grant||!grant->current()||grant->application()!=resources.application_||
       grant->generation_!=resources.generation_||!resources.issuer_||
       resources.issuer_!=grant->issuer()||!assets||!assets->loaded()||!assets->toyDat())
        return fail(d,S::input,0x2713F8);
    try {
        auto next=std::make_unique<Storage>();
        next->grant=std::move(grant);next->assets=std::move(assets);next->resources=&resources;
        storage_=std::move(next);auto& s=*storage_;
        const auto step=s.grant->constructPrefix(s.toyList3C8,K::toyList2D9B40);
        if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}
        s.toyLive=true;
        const auto* general=toy.section("Animations");
        assets::OriginalUiPageDiagnostic50KG text;std::int32_t count=0;
        if(!general||!assets::originalUiInteger50KG(*general,"NumberOfAnimations",0,count,text,0x2D9CC8)||
           count!=static_cast<std::int32_t>(s.records34.size())){
            s.failed=true;return fail(d,S::source,text.sourcePC?text.sourcePC:0x2D9CD0);
        }
        //2D9D14 publishes count30 before records34; no visibility, motion
        // library, live model, character Entity, manager or audio exists here.
        s.toyCount30=static_cast<std::uint32_t>(count);
        const auto& definitions=game::garageActorDefinitions50GN();
        for(std::size_t i=0;i<s.records34.size();++i){
            const auto* a=s.assets->actor(i);
            const auto* row=toy.section("Animation"+std::to_string(i+1));
            if(!row){s.failed=true;return fail(d,S::source,0x2D8A60);}
            s.actors[i].sourceDescriptor=*row;
            const auto* animationType=row->find("AnimationType");
            if(!animationType||lower(animationType->text)!="keyframed"){
                s.failed=true;return fail(d,S::source,0x2D8A60);
            }
            s.actors[i].type00=1;s.actors[i].auxiliary60=nullptr; //Actual2D83B0 constructor store.
            if(!a||a->ordinal!=i+1||a->mcfName!=definitions[i].mcf||!a->model||
               a->motionCount!=definitions[i].motionCount||a->startingZone!=1||
               a->offset!=std::array<float,3>{}||!a->useLighting||!a->useBlendedKeys||a->useCollision){
                s.failed=true;return fail(d,S::source,0x2D8A60);
            }
            //2D8F9C is reached even when no looping sounds are authored.
            // Retain the actual allocated prefix across any rejected read;
            // constructor-null60 alone is not evidence that this path ran.
            s.loopSoundDescriptor=i;s.stage=Stage::loopSoundPrefix;
            auto& actor=s.actors[i];actor.loopSound60=std::make_unique<Storage::LoopSoundList>();
            auto& loop=*actor.loopSound60;
            auto constructed=s.grant->constructActorLoopSound2795E0(*this,i,s.toyList3C8,loop.entity);
            if(!constructed.okay()){s.failed=true;return fail(d,S::phase,constructed.sourcePC);}
            loop.constructed=true;loop.count30=0;loop.rows34=nullptr; //279600/04.
            actor.auxiliary60=&loop.entity; //2D8FB8, before the actual2796A0 read.
            std::int32_t count30=0;assets::OriginalUiPageDiagnostic50KG read;
            if(!assets::originalUiInteger50KG(actor.sourceDescriptor,"NumberOfLoopingSounds",0,count30,read,0x2796D0)){
                s.failed=true;return fail(d,S::source,read.sourcePC?read.sourcePC:0x2796D0);
            }
            loop.count30=count30;loop.readComplete=true;loop.readFalse=count30==0;
            if(!loop.readFalse){
                //2796F8 would allocate genuine ContinuousAnimObjectSound rows.
                // Keep the exact live prefix; do not invent those emitters.
                s.failed=true;return fail(d,S::source,0x2796F8);
            }
            if(loop.rows34||!actor.auxiliary60||*actor.auxiliary60!=&loop.entity){
                s.failed=true;return fail(d,S::phase,0x2D8FE4);
            }
            const auto retired=s.grant->retireActorLoopSoundReadFalse142590(*this,i,s.toyList3C8,loop.entity);
            if(!retired.okay()){s.failed=true;return fail(d,S::phase,retired.sourcePC);}
            loop.released=true;actor.auxiliary60=nullptr; //Actual2D8FEC, only after279668/1413E0.
            ++s.retiredLoopSoundLists;
            // Native non-Entity TOY descriptor custody pins the exact immutable
            // row. Its live character is constructed only in the late pass.
            s.actors[i].sourceToy=a;s.records34[i]=&s.actors[i];
        }
        s.stage=Stage::toyDescriptors;return true;
    }catch(...){if(storage_)storage_->failed=true;return fail(d,S::allocation,0x2713F8);}
}

bool OriginalUiGarageActors50GN::toyDescriptorsCurrent()const noexcept {
    if(!storage_)return false;
    const auto& s=*storage_;
    if(s.closed||s.failed||s.toyRetiring||!s.toyLive||!s.grant||!s.grant->current()||
       !s.assets||!s.assets->loaded()||!s.resources||s.toyCount30!=s.actors.size()||
       s.retiredLoopSoundLists!=s.actors.size()||
       s.toyList3C8.kind!=K::toyList2D9B40||s.toyList3C8.reference00!=1||
       s.toyList3C8.sessionGeneration!=s.grant->generation_||
       s.toyList3C8.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&s.toyList3C8))||
       s.toyList3C8.parent1C||s.toyList3C8.previous10||s.toyList3C8.next14||s.toyList3C8.mask20)
        return false;
    for(std::size_t i=0;i<s.records34.size();++i)
        if(s.records34[i]!=&s.actors[i]||s.actors[i].sourceToy!=s.assets->actor(i)||
           !s.actors[i].loopSound60||!s.actors[i].loopSound60->constructed||
           !s.actors[i].loopSound60->readComplete||!s.actors[i].loopSound60->readFalse||
           !s.actors[i].loopSound60->released||s.actors[i].loopSound60->entity.reference00||
           !s.actors[i].auxiliary60.has_value()||*s.actors[i].auxiliary60)return false;
    return true;
}

bool OriginalUiGarageActors50GN::prepareToyList2713F8(OriginalUiGaragePodScene50KG& scene,
    std::shared_ptr<Grant> grant,OriginalUiGarageModelResources50KG& resources,
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,D& d)noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x2713F8);
    if(!scene.currentPrefix()||scene.grant_!=grant||scene.assets_!=assets||scene.actorOwner3C8_||
       scene.visibilityAttempted_||scene.visibility_||!scene.staticModels_||
       scene.staticModels_->resources_.get()!=&resources||
       !scene.staticModels_->belongsTo(scene,grant,assets)||
       !scene.staticModels_->descriptorIdentityCurrent()||scene.staticModels_->populationAttempted_)
        return fail(d,S::phase,0x2713F8);
    const auto* config=scene.liveToyConfig34();
    if(!config||scene.loadStage_!=OriginalUiGaragePodScene50KG::LoadStage::toyPrefix)
        return fail(d,S::phase,0x2713F8);
    const bool prepared=constructToyDescriptors2713F8(*config,std::move(grant),resources,std::move(assets),d);
    if(prepared)scene.loadStage_=OriginalUiGaragePodScene50KG::LoadStage::toyDescriptors;
    if(storage_){storage_->sourceSceneOwner=&scene;scene.actorOwner3C8_=this;}
    return prepared;
}

bool OriginalUiGarageActors50GN::constructManager274324(std::shared_ptr<const RootInputs> inputs,
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility,D& d)noexcept {
    d={};if(!toyDescriptorsCurrent())return fail(d,S::phase,0x2742F0);
    auto& s=*storage_;
    if(s.stage!=Stage::toyDescriptors||s.managerAttempted||s.managerLive||s.visibility||
       !inputs||inputs->issuer_!=s.resources->issuer_||inputs->issuer_!=s.grant->issuer()||
       inputs->grant_!=s.grant||!inputs->application_||!inputs->application_->known||
       !inputs->motions_||!inputs->audio_||!visibility||
       !visibility->matchesColdConstruction(s.grant,s.assets)||
       !s.grant->characterShared()||!s.grant->sphereShared())return fail(d,S::input,0x2742F0);
    auto* droids=inputs->audio_->garageDroidAudioCold();
    if(!droids||!droids->sourceConstructionPrefixCurrent())return fail(d,S::phase,0x278F60);
    s.rootInputs=std::move(inputs);s.application=s.rootInputs->application_;
    s.motionLibrary=s.rootInputs->motions_;s.audio=s.rootInputs->audio_;
    s.droids=droids;s.files=s.resources->domain_->resolver3E1A70();
    s.visibility=std::move(visibility);s.managerAttempted=true;
    auto step=s.grant->constructPrefix(s.manager49C.entity,K::animationManager101330);
    if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}
    s.managerLive=true;
    step=s.grant->publishAnimationManager(s.manager49C);
    if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}
    s.managerPublished=true;s.stage=Stage::owners;
    //101380 has NOT run. The ten constructors/interleaved suffixes precede
    // manager49C.toys38=&toyList3C8. No Page/renderer readiness is issued.
    return true;
}

bool OriginalUiGarageActors50GN::prepareAnimationManager2742F0(
    std::shared_ptr<const RootInputs> inputs,
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility,D& d)noexcept {
    d={};if(!storage_||!storage_->sourceSceneOwner)return fail(d,S::phase,0x2742F0);
    const auto& s=*storage_;const auto& scene=*s.sourceSceneOwner;
    if(!scene.currentPrefix()||scene.populationStage_!=OriginalUiGaragePodScene50KG::PopulationStage::collisions||
       !scene.sceneCollision_||!scene.sceneCollision_->current2743B0(scene)||
       scene.grant_!=s.grant||scene.assets_!=s.assets||
       scene.actorOwner3C8_!=this||scene.visibility_!=visibility||!visibility||visibility->sourceSceneOwner_!=&scene||
       !inputs||inputs->audio_!=scene.main_||!inputs->audio_||
       inputs->audio_.owner_before(scene.main_)||scene.main_.owner_before(inputs->audio_)||
       inputs->audio_->garageDroidAudioCold()!=scene.backend_||!scene.backend_||
       !scene.staticModels_||!scene.staticModels_->belongsTo(scene,s.grant,s.assets)||
       !scene.staticModels_->populationCurrent())return fail(d,S::phase,0x274160);
    if(!constructManager274324(std::move(inputs),std::move(visibility),d))return false;
    storage_->sourceOrdered=true;return true;
}

bool OriginalUiGarageActors50GN::finishPodSceneActorScan1B40B0(
    OriginalUiGaragePodScene50KG& scene,D& d)noexcept {
    d={};if(!scenePopulationCurrent2742F0(scene)||scene.actorOverrideScan1B40B0_||
       !scene.worldList500_||scene.capacity504_!=10||scene.growth508_!=100000||scene.count50C_||
       scene.interpolation510_||scene.interpolation514_)return fail(d,S::phase,0x1B40B0);
    const auto& s=*storage_;
    if(s.toyCount30!=game::garageActorCount50GN)return fail(d,S::phase,0x1B415C);
    for(std::size_t i=0;i<s.toyCount30;++i){const auto* row=s.records34[i];
        if(row!=&s.actors[i])return fail(d,S::phase,0x1B4104);
        if(row->type00!=1)continue; //Source1B410C exact discriminator branch.
        const auto& animation=row->base.fields_;
        if(!animation.model04||!row->model||animation.model04!=row->model->scene().model())
            return fail(d,S::phase,0x1B4114);
        // The actual retained MCF reader wrote rig138 (not an asset-derived
        // guessed filename). Source3090E8 compares it case-insensitively.
        constexpr char cameraName[]="camdroid.slt";
        bool matches=animation.rig138.size()==sizeof(cameraName)-1;
        for(std::size_t c=0;matches&&c<animation.rig138.size();++c){
            char actual=animation.rig138[c];if(actual>='A'&&actual<='Z')actual=static_cast<char>(actual+32);
            matches=actual==cameraName[c];
        }
        if(matches)return fail(d,S::source,0x1B4140); //Populated interpolation branch not yet owned.
    }
    // All ten reached actual rows fail the camdroid comparison.1B417C
    // bypasses BOTH allocations at510/514; the real constructor list remains.
    for(std::size_t i=0;i<scene.capacity504_;++i)if(scene.worldList500_[i])
        return fail(d,S::phase,0x1B4178);
    scene.actorOverrideScan1B40B0_=true;return true;
}

bool OriginalUiGarageActors50GN::borrowPageEntities1B4A68(const OriginalUiGaragePodScene50KG& scene,
    game::OriginalWreckComponent50LA*& manager,game::OriginalWreckComponent50LA*& toys)noexcept {
    manager=toys=nullptr;if(!scenePopulationCurrent2742F0(scene))return false;
    auto& s=*storage_;
    if(scene.actorOwner3C8_!=this||s.manager49C.toys38!=&s.toyList3C8||
       s.grant->animationManager()!=&s.manager49C||s.manager49C.entity.parent1C||s.toyList3C8.parent1C)
        return false;
    manager=&s.manager49C.entity;toys=&s.toyList3C8;return true;
}
bool OriginalUiGarageActors50GN::scenePopulationCurrent2742F0(
    const OriginalUiGaragePodScene50KG& scene)const noexcept {
    if(!storage_)return false;
    const auto& s=*storage_;
    if(!s.sourceOrdered||s.sourceSceneOwner!=&scene||s.failed||s.closed||
       s.stage!=Stage::complete||s.constructed!=game::garageActorCount50GN||
       s.completed!=game::garageActorCount50GN||!s.toyLive||!s.managerLive||!s.managerPublished||
       !s.visibility||s.visibility!=scene.visibility_||s.grant!=scene.grant_)return false;
    for(const auto& a:s.actors)if(!a.entityLive||!a.modelEnrolled||!a.attached||!a.membership||
        !a.initialPose||a.retiring||!a.model)return false;
    return true;
}

bool OriginalUiGarageActors50GN::prepareOwners2713F8(std::shared_ptr<Grant> grant,
    OriginalUiGarageModelResources50KG& resources,
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,
    std::shared_ptr<const RootInputs> inputs,
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility,D& d)noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x2713F8);
    if(!grant||!grant->current()||!inputs||!inputs->issuer_||inputs->issuer_!=resources.issuer_||
       inputs->issuer_!=grant->issuer()||inputs->grant_!=grant||!inputs->application_||
       !inputs->application_->known||!inputs->motions_||!inputs->audio_||
       !grant->characterShared()||!grant->sphereShared()||
       !visibility||!visibility->matchesColdConstruction(grant,assets))return fail(d,S::input,0x2713F8);
    const auto* droids=inputs->audio_->garageDroidAudioCold();
    if(!droids||!droids->sourceConstructionPrefixCurrent())return fail(d,S::phase,0x278F60);
    if(!assets||!assets->toyDat())return fail(d,S::input,0x2713F8);
    const auto* componentConfig=assets->toyDat();
    if(!constructToyDescriptors2713F8(*componentConfig,std::move(grant),resources,std::move(assets),d))return false;
    // Explicit detached component compatibility, never sourceOrdered custody.
    return constructManager274324(std::move(inputs),std::move(visibility),d);
}

bool OriginalUiGarageActors50GN::constructNext2D9060(D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x2D9060);
    auto& s=*storage_;
    if(s.closed||s.failed||s.stage!=Stage::owners||s.ordinal>=s.actors.size()||
       s.completed!=s.ordinal||!s.grant->current()||
       s.grant->animationManager()!=&s.manager49C)return fail(d,S::phase,0x2D9060);
    auto& a=s.actors[s.ordinal];
    a.primary4C=s.visibility->primaryZone4C0();a.secondary50=s.visibility->secondaryZone4C4();
    if(!a.primary4C||!a.secondary50||a.primary4C==a.secondary50){s.failed=true;return fail(d,S::phase,0x2D915C);}
    const auto step=s.grant->constructPrefix(a.entity290,K::character11A750);
    if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}
    a.entityLive=true;++s.constructed;
    const auto actual=s.grant->application();
    const auto constructed=a.base.constructCharacterBase1536F0({s.grant->generation_,a.entity290.objectKey,
        &actual->objectCount423F60(),&actual->attachmentLock423F58()},a.entity290);
    if(!constructed.okay()){s.failed=true;return fail(d,S::phase,constructed.sourcePC);}
    auto& f=a.base.fields_;
    f.dispatch21C=K::character11A750;f.entity220Adjustment30=0; //11A7D0.
    game::initializeOriginalGarageCharacterFields11A75050GN(a.fields);
    a.playback.stopped10=true;s.stage=Stage::character;
    f.field18C=1; //1538C8, before MCF lookup/publication;11A93C a3=1.
    try {
        if(a.sourceToy!=s.assets->actor(s.ordinal)){s.failed=true;return fail(d,S::source,0x2D9060);}
        const auto& source=*a.sourceToy;
        const auto member=s.assets->archive()->find("UI/Anim/"+source.mcfName);
        if(!member.identity||member.identity!=source.mcf.identity()||!member.data||
           member.size!=member.identity->bytes){s.failed=true;return fail(d,S::source,0x153998);}
        const auto provider=s.resources->domain_->archiveProvider3E1A70();
        const auto winner=[&](const assets::OriginalUiMemberView50KG& expected) {
            assets::OriginalStandardArchiveProvider50LA::MemberView actual;
            return provider&&provider->sealed()&&expected.identity&&expected.data&&
                provider->lookup132820(expected.identity->name,actual)==
                    assets::OriginalStandardArchiveProvider50LA::Lookup::present&&actual.data&&
                actual.size==expected.size&&!std::memcmp(actual.data,expected.data,actual.size);
        };
        if(!winner(member)){s.failed=true;return fail(d,S::source,0x132820);}
        const std::vector<std::uint8_t> bytes(member.data,member.data+member.size);
        assets::OriginalStandardTextureDiagnostic50LA cache;
        if(!s.files->bindModelMember(member.identity->name,assets::OriginalStandardModelFileKind50LA::mcf,
            bytes,a.mcfId,cache)){s.failed=true;return fail(d,S::source,cache.sourcePC);}
        assets::OriginalStandardModelFileView50LA found;
        if(!s.files->modelFile(a.mcfId,found,cache)){s.failed=true;return fail(d,S::source,cache.sourcePC);}
        // Unique Garage MCF names make the ordinary fresh page cold. A real
        //live cache hit is still156300, NOT permission to reread a bind pose.
        //That distinct prototype clone remains unavailable and is never erased.
        if(found.animation1536F0){s.failed=true;return fail(d,S::phase,0x153C20);}
        if(!s.files->publishAnimation1326A0(a.mcfId,&a.base,cache)){
            s.failed=true;return fail(d,S::phase,cache.sourcePC);}
        a.cachePublished=true;s.stage=Stage::animationCache;
        a.configuration08=std::make_unique<game::OriginalMotionConfiguration50LA>(configuration(source.mcf));
        f.configuration08=a.configuration08.get();f.definition48=member.identity->name;
        const auto general=a.configuration08->find("general info");
        if(general==a.configuration08->end()){s.failed=true;return fail(d,S::source,0x153CB8);}
        const auto& section=general->second;
        const auto* slt=game::original_motion_detail50LA::value(section,"sltfile");
        const auto* vut=game::original_motion_detail50LA::value(section,"vutfilename");
        if(!slt||!vut||game::original_motion_detail50LA::value(section,"mirfilename")||
           game::original_motion_detail50LA::value(section,"mirroraxis")){
            s.failed=true;return fail(d,S::source,0x153CA0);}
        f.rig138=*slt;f.rigOverride188=0;f.directory190="ui\\anim\\";
        f.model98=*vut;f.motionE8="\\";f.motionCount14=static_cast<std::int32_t>(source.motionCount);
        f.field20=1;f.field24=1;f.field28=1;f.field2C=1;f.field0C=0;
        const bool loaded=s.resources->load(source.model,R::RootMode::separate,true,a.model,d);
        if(a.model&&a.model->scene().model())f.model04=a.model->scene().model();
        if(!loaded){s.failed=true;return false;}
        auto step2=s.grant->enrollConstructedModel(a.model->scene());
        if(!step2.okay()){s.failed=true;return fail(d,S::phase,step2.sourcePC);}
        a.modelEnrolled=true;
        step2=s.grant->attachModel(a.entity290,a.model->scene(),0u);
        if(!step2.okay()){s.failed=true;return fail(d,S::phase,step2.sourcePC);}
        s.stage=Stage::model;
        const auto graph=a.model->graph();const auto* model=a.model->scene().model();
        // Map the actual current hierarchy by its authenticated unique source
        //names. Warm PSG cloning uses enumeration order, not necessarily PSG
        //row ordinal. The mutable locals remain those copied from its actual
        //live geometry prototype, including the previous actor's initial pose.
        a.rows=source.bindings;
        if(!a.rows.count||a.rows.count!=source.model->payload->nodeCount()){
            s.failed=true;return fail(d,S::source,0x152EA0);}
        for(std::uint32_t row=0;row<a.rows.count;++row){
            const auto original=a.rows.rows[row].sceneNode;
            if(original>=source.model->payload->nodeCount()){s.failed=true;return fail(d,S::source,0x152EA0);}
            const auto* name=source.model->payload->sourceNodes()[original].name;
            const game::OriginalRacerSceneObject50LA* matched=nullptr;
            for(std::uint32_t j=0;j<a.model->scene().sourceRowCount();++j){const auto* node=a.model->scene().sourceRow(j);
                if(node&&game::original_motion_detail50LA::equal308F00(node->name38,name)){
                    if(matched){s.failed=true;return fail(d,S::source,0x152EA0);}matched=node;
                }
            }
            if(!matched||static_cast<std::int32_t>(matched->sceneIndex)<0||
               static_cast<std::uint32_t>(matched->sceneIndex)>=graph.count){
                s.failed=true;return fail(d,S::source,0x152EA0);}
            a.rows.rows[row].sceneNode=static_cast<std::uint8_t>(matched->sceneIndex);
        }
        f.nodeCount1E8=static_cast<std::int32_t>(a.rows.count); //15435C precedes allocation154364.
        a.records1EC.assign(a.rows.rows,a.rows.rows+a.rows.count);
        f.allocation1EC=a.records1EC.data();
        // Exact absent MIR lookup: default MIRFilename is one backslash,
        //concatenated after the actual ui\anim\ directory, not "none".
        assets::OriginalStandardArchiveProvider50LA::MemberView mirror;
        if(s.resources->domain_->archiveProvider3E1A70()->lookup132820(f.directory190+f.motionE8,mirror)!=
           assets::OriginalStandardArchiveProvider50LA::Lookup::absent){
            s.failed=true;return fail(d,S::source,0x153534);}
        if(!winner(source.vut)){s.failed=true;return fail(d,S::source,0x132820);}
        f.field1F8=1; // Real retained opened VUT produced these renamed rows.
        game::ModelPlaybackDiagnostic50KG playback;
        if(!game::captureModelPlaybackMatrices50KG(a.rows,graph,a.playback,playback)){
            s.failed=true;return fail(d,S::source,playback.sourceStage);}
        a.saved1F4.assign(a.playback.savedMatrices1F4,a.playback.savedMatrices1F4+a.playback.savedMatrixCount1F4);
        f.allocation1F4=a.saved1F4.data();
        game::OriginalMotionDefaults50LA defaults;defaults.directory190=f.directory190;defaults.rotationType28=1;
        const auto resolve=[&](const std::string& requested,game::OriginalAdmittedMotion50LA& out) {
            for(std::uint32_t j=0;j<source.motionCount;++j){const auto& motion=source.motions[j];
                if(motion.source.identity&&game::original_motion_detail50LA::equal308F00(requested,motion.source.identity->name)){
                    if(motion.motion.data!=motion.source.data||motion.motion.bytes!=motion.source.size||
                       !winner(motion.source))return false;
                    out.motion=&motion.motion;out.originalMemberName=motion.source.identity->name;return true;
                }
            }
            return false;
        };
        const auto motionResult=a.motions.load153610(*s.motionLibrary,*a.configuration08,
            f.motionCount14,1,defaults,resolve);
        if(!motionResult.okay()){s.failed=true;return fail(d,S::source,motionResult.sourcePC);}
        f.motionArray1E4=a.motions.motionArray1E4();
        for(std::uint32_t row=0;row<a.rows.count;++row)
            if(!game::setLocalMatrix49P(graph,static_cast<game::SceneNodeIndex49P>(a.rows.rows[row].sceneNode),a.saved1F4[row])){
                s.failed=true;return fail(d,S::source,0x1556C0);}
        //1573B0 releases the temporary actual config; source08 stays stale.
        a.configuration08.reset();s.stage=Stage::motion;
        a.fields.firstChild284=graph.nodes[static_cast<std::uint32_t>(model->sceneIndex)].firstChild184; //11A950.
        if(a.fields.firstChild284==game::SceneNodeIndex49P::none){s.failed=true;return fail(d,S::source,0x11A950);}
        auto& root=graph.nodes[static_cast<std::uint32_t>(model->sceneIndex)];
        root.sceneFlags190=static_cast<std::uint8_t>(root.sceneFlags190|0x18u);
        const auto* toy=&a.sourceDescriptor;
        if(!toy||toy->find("LookVector")||toy->find("UpVector")||toy->find("Yaw")||toy->find("Pitch")||toy->find("Roll")){
            s.failed=true;return fail(d,S::source,0x2D92D0);}
        // All authored angles and Offset are exactly zero.157100's polynomial
        //gives +Z/+Y;289EC0(a3=0,t0=1,t1=0) yields this exact homogeneous pose.
        //It does not call source acos or alter the retained application libm.
        const game::SceneDisplacement49P position{{0.f,0.f,0.f,1.f}};
        const game::BodyMatrix48G pose{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
        if(!game::setLocalTranslation49P(graph,model->sceneIndex,position)||
           !game::setLocalMatrix49P(graph,model->sceneIndex,pose)){
            s.failed=true;return fail(d,S::source,0x2D9348);}
        s.stage=Stage::placement;
        if(!s.visibility->attachActor2D9350(s.ordinal,a.model,d)){s.failed=true;return false;}
        a.membership=true;s.stage=Stage::membership;
        //2D9384 uses actual library lookup, no extra153610/D8 retain.
        const auto& def=game::garageActorDefinitions50GN()[s.ordinal];
        for(std::uint32_t j=0;j<source.motionCount;++j){
            const auto* motion=s.motionLibrary->find1523F0(def.motionNames[j]);
            if(!motion||motion!=a.motions.at(j)){s.failed=true;return fail(d,S::source,0x2D939C);}
            a.descriptors[j]={&motion->motion(),motion->fields().loopingB4!=0};
        }
        step2=s.grant->attach(s.toyList3C8,a.entity290,2u);
        if(!step2.okay()){s.failed=true;return fail(d,S::phase,step2.sourcePC);}
        a.attached=true;
        return continueInitial2D946C(d);
    }catch(...){s.failed=true;return fail(d,S::allocation,0x1538B0);}
}

bool OriginalUiGarageActors50GN::continueInitial2D946C(D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x2D946C);
    auto& s=*storage_;
    if(s.closed||s.failed||s.stage!=Stage::membership||s.ordinal>=s.actors.size()||!s.grant->current())
        return fail(d,S::phase,0x2D946C);
    auto& a=s.actors[s.ordinal];
    if(!a.membership||!a.attached||!a.model||!a.model->current())return fail(d,S::phase,0x2D946C);
    game::GarageActorBinding50GN binding;
    binding.resources=&a.motions;binding.playback=&a.playback;binding.rows=&a.rows;
    binding.graph=a.model->graph();binding.ik=&a.ik;
    for(std::size_t j=0;j<a.descriptors.size();++j)binding.motions[j]=&a.descriptors[j];
    game::GarageSequenceDiagnostic50GN sequence;
    if(!s.sequence.bindNextInitialPose(s.ordinal,binding,s.application->modelMotion,
        s.application->modelMode0Ik,s.work,sequence)){
        s.failed=true;return fail(d,S::source,sequence.sourcePC);}
    a.initialPose=true;s.stage=Stage::initialPose;
    auto& f=a.base.fields_;f.motion18=a.motions.at(0);f.time34=a.playback.time34;f.field10=1;
    if(!s.visibility->updateActor28EA20(s.ordinal,*a.model,d)){s.failed=true;return false;}
    s.stage=Stage::soundRows;
    std::string error;
    if(!s.droids->appendActorSoundRows278F60(s.ordinal,error)){s.failed=true;return fail(d,S::phase,0x278F60);}
    ++s.completed;++s.ordinal;
    if(s.completed==s.actors.size()){
        s.manager49C.toys38=&s.toyList3C8; // Actual101380, only after all10.
        s.stage=Stage::complete;
    }else s.stage=Stage::owners;
    return true;
}

OriginalUiGarageActors50GN::RootInputs::RootInputs(
    const ::original_main_entry50KG::Application* issuer,std::shared_ptr<Grant> grant,
    std::shared_ptr<OriginalUiMainApplication50KG> application,
    std::shared_ptr<game::OriginalMotionLibrary50LA> motions,std::shared_ptr<OriginalUiMainAudio50KG> audio,
    game::OriginalApplicationAnimationClockView50KG pause,
    ::original_ui_source_timing50LH::PublicationView timing)noexcept
    :issuer_(issuer),grant_(std::move(grant)),application_(std::move(application)),
     motions_(std::move(motions)),audio_(std::move(audio)),expectedPause_(std::move(pause)),
     expectedTiming_(std::move(timing)),expectedClock101A50_(application_?application_->clock101A50:nullptr){}

OriginalUiGarageActors50GN::RuntimeFrame::RuntimeFrame(std::shared_ptr<const RootInputs> roots,
    std::shared_ptr<OriginalUiGarageRendererLease50KG> renderer)noexcept
    :roots_(std::move(roots)),renderer_(std::move(renderer)) {
    if(!roots_)return;
    pause_=roots_->expectedPause_;timing_=roots_->expectedTiming_;
    ::original_ui_source_timing50LH::Sample sample;
    if(!timing_.readPrepared(sample,publication_))publication_=0;
}

bool OriginalUiGarageActors50GN::currentRuntimeFrame(const RuntimeFrame& frame,
    float& dt,std::uint32_t& pause)const noexcept {
    if(!storage_)return false;
    const auto& s=*storage_;
    if(s.closed||s.failed||s.runtimeFailed||!s.sourceOrdered||!s.sourceSceneOwner||
       s.stage!=Stage::complete||!s.grant->current()||
       frame.roots_!=s.rootInputs||!frame.publication_||
       !frame.pause_.sameOwner(s.rootInputs->expectedPause_)||
       !frame.timing_.sameOwner(s.rootInputs->expectedTiming_)||
       !s.rootInputs->expectedClock101A50_||s.application->clock101A50!=s.rootInputs->expectedClock101A50_||
       s.grant->issuer()!=s.rootInputs->issuer_||s.grant->animationManager()!=&s.manager49C||
       !frame.pause_.readPause11C(pause))return false;
    if(s.runtimeStarted&&(!s.runtimeTiming.sameOwner(frame.timing_)||!s.runtimePause.sameOwner(frame.pause_)))return false;
    ::original_ui_source_timing50LH::Sample sample;std::uint64_t publication=0;
    if(!frame.timing_.readPrepared(sample,publication)||publication!=frame.publication_)return false;
    // This is the producer's already capped23C/f12. Never substitute raw250.
    dt=sample.updateSeconds;return true;
}

bool OriginalUiGarageActors50GN::readTimestamp101A50(void* context,std::uint32_t& out)noexcept {
    auto& self=*static_cast<OriginalUiGarageActors50GN*>(context);
    auto& s=*self.storage_;
    const auto& expected=s.rootInputs->expectedClock101A50_;
    return expected&&s.application->clock101A50==expected&&game::sampleHostTimestamp50ID(*expected,out);
}
void OriginalUiGarageActors50GN::publishPlaybackFields()noexcept {
    auto& s=*storage_;
    for(auto& a:s.actors){auto& f=a.base.fields_;const auto& p=a.playback;
        auto resource=[&](const game::ModelPlaybackMotion50KG* descriptor)->const void* {
            if(!descriptor)return nullptr;
            for(std::size_t i=0;i<a.descriptors.size();++i)if(descriptor==&a.descriptors[i])return a.motions.at(i);
            return nullptr;
        };
        f.field10=p.stopped10?1u:0u;f.field30=p.blending30?1u:0u;
        f.motion18=resource(p.current18);f.motion1C=resource(p.previous1C);
        f.time34=p.time34;f.time38=p.oldTime38;
        // blend3C/40/44 are canonical retained ModelPlayback fields. The cold
        // semantic prefix does not manufacture a second copy of those lanes.
    }
}
bool OriginalUiGarageActors50GN::afterSample11AD80(void* context,std::size_t ordinal,
    const game::GarageActorBinding50GN& binding,game::GarageSequenceDiagnostic50GN& diagnostic)noexcept {
    auto& self=*static_cast<OriginalUiGarageActors50GN*>(context);auto& s=*self.storage_;
    if(ordinal>=s.actors.size())return false;
    auto& a=s.actors[ordinal];
    if(binding.playback!=&a.playback||binding.resources!=&a.motions||!a.model||!a.model->current())return false;
    self.publishPlaybackFields();
    // All authenticated Garage MOTs use the normal (non-loop wrap)11AD80
    // branch. Collider280 is absent for every authored UseCollision=F actor.
    if(a.playback.current18&&a.playback.current18->looping){
        diagnostic.error=game::GarageSequenceError50GN::state;diagnostic.sourcePC=0x11ADC0;return false;
    }
    D bounds;
    if(!s.visibility->updateActor28EA20(ordinal,*a.model,bounds)){
        diagnostic.error=game::GarageSequenceError50GN::postSample;diagnostic.sourcePC=bounds.sourcePC;return false;
    }
    // Actual old220, before the later ToyList11AAE0 traversal. No sound replay
    // or group elapsed service is inferred from an animation callback.
    // Observe the actual pre-play refusal without changing its admission.
    // A poisoned or partly published native voice is a different failure.
    const bool prePlayRefused=!s.droids->ready()&&!s.droids->poisoned_;
    const auto beforeAudioRevision=s.droids->revision_;
    if(!s.droids->afterSample(ordinal,a.playback.time34,a.playback.stopped10,a.fields.position220)){
        s.failedSampleAudioPreflight=prePlayRefused&&!s.droids->poisoned_&&
            s.droids->revision_==beforeAudioRevision;
        diagnostic.error=game::GarageSequenceError50GN::postSample;diagnostic.sourcePC=0x10179C;return false;
    }
    return true;
}

bool OriginalUiGarageActors50GN::serviceManager101860(std::shared_ptr<const RuntimeFrame> frame,
    RuntimeReceipt& receipt,D& d)noexcept {
    d={};receipt={};float dt=0;std::uint32_t pause=0;
    if(!frame||!currentRuntimeFrame(*frame,dt,pause))return fail(d,S::phase,0x101860);
    auto& s=*storage_;
    if(s.pendingFrame||frame->publication_<=s.lastRuntimePublication||
       s.manager49C.toys38!=&s.toyList3C8||s.toyCount30!=s.actors.size())return fail(d,S::phase,0x101860);
    // Validate retained identities, not a replacement population or copied RNG.
    for(std::size_t i=0;i<s.actors.size();++i){const auto& a=s.actors[i];
        if(s.records34[i]!=&a||!a.entityLive||!a.attached||a.entity290.parent1C!=&s.toyList3C8||
           !a.model||!a.model->current())return fail(d,S::phase,0x1019B8);
    }
    //The actual Sound/transport owner is required here, NOT later menu/wet
    //readiness. Source playback is still attempted only by afterSample and
    //its real native wet-voice guard; silent predecessors must not be skipped.
    if(!pause&&(s.audio->nativeDroidSampleTransport()!=s.droids||!s.application->clock101A50))return fail(d,S::phase,0x101860);
    s.runtimeTiming=frame->timing_;s.runtimePause=frame->pause_;s.pendingFrame=std::move(frame);
    s.runtimeStarted=true;s.runtimeReceipt={};s.runtimeReceipt.publication=s.pendingFrame->publication_;
    std::array<bool,game::garageActorCount50GN> enabled{};
    for(std::size_t i=0;i<enabled.size();++i)enabled[i]=(s.actors[i].entity290.flags29&1u)!=0;
    game::GarageSequenceDiagnostic50GN sequence;
    const bool completed=s.sequence.serviceManager101860(s.manager49C,pause!=0,dt,enabled,
        &readTimestamp101A50,this,s.application->sharedNoise308030,s.application->sharedRandomSelectors,
        s.application->modelMotion,s.application->modelMode0Ik,s.work,&afterSample11AD80,this,
        s.runtimeReceipt.manager,sequence);
    publishPlaybackFields();receipt=s.runtimeReceipt;
    if(!completed){
        s.runtimeFailed=true;
        //10179C is emitted only AFTER the actual model sample and Observer
        //update returned and Droid afterSample refused. The RAII servicing
        //guard has returned; preserve every playback/RNG/receipt write.
        if(s.failedSampleAudioPreflight&&sequence.error==game::GarageSequenceError50GN::postSample&&sequence.sourcePC==0x10179C&&
           sequence.actor<s.actors.size()&&s.sequence.failed_&&!s.sequence.servicing_&&
           s.runtimeReceipt.manager.actors[sequence.actor].sampled&&!s.lastRuntimePublication&&
           !s.runtimeReceipt.managerComplete&&!s.runtimeReceipt.toyListComplete&&
           !s.nextRuntimeChild&&s.pendingFrame){
            s.failedSampleActor=sequence.actor;s.failedSamplePublication=s.pendingFrame->publication_;
            s.failedSampleFrameIdentity=s.pendingFrame.get();s.failedSample10179C=true;
        }
        return fail(d,S::source,sequence.sourcePC);
    }
    //101A24 invokes the manager's own141ED0 only on the unpaused path. This
    //actual owner has no children; foreign additions require a real dispatcher.
    if(!pause&&(s.manager49C.entity.firstChild18||(s.manager49C.entity.descendants24&2u))){
        s.runtimeFailed=true;return fail(d,S::phase,0x101A24);
    }
    s.runtimeReceipt.managerComplete=true;
    s.nextRuntimeChild=(s.toyList3C8.descendants24&2u)?s.toyList3C8.firstChild18:nullptr;
    receipt=s.runtimeReceipt;return true;
}

bool OriginalUiGarageActors50GN::continueToyList141ED0(std::shared_ptr<const RuntimeFrame> frame,
    RuntimeReceipt& receipt,D& d)noexcept {
    d={};float dt=0;std::uint32_t pause=0;
    if(!frame||!currentRuntimeFrame(*frame,dt,pause))return fail(d,S::phase,0x141ED0);
    auto& s=*storage_;receipt=s.runtimeReceipt;
    if(!s.pendingFrame||!s.runtimeReceipt.managerComplete||
       frame->publication_!=s.pendingFrame->publication_||!frame->timing_.sameOwner(s.pendingFrame->timing_)||
       !frame->pause_.sameOwner(s.pendingFrame->pause_))return fail(d,S::phase,0x141ED0);
    // Source does NOT skip ToyList/character work merely because manager pause
    // was nonzero. dt0 reciprocal semantics remain unavailable, not zeroed.
    while(s.nextRuntimeChild){
        if(s.runtimeReceipt.visitedCharacters>=s.actors.size()){
            s.runtimeFailed=true;return fail(d,S::phase,0x141F08);
        }
        C* child=s.nextRuntimeChild;Storage::Actor* actor=nullptr;
        for(auto& a:s.actors)if(&a.entity290==child){actor=&a;break;}
        if(!actor||child->parent1C!=&s.toyList3C8){s.runtimeFailed=true;return fail(d,S::phase,0x141F08);}
        if(game::originalWreckChildEligible50LA(*child)){
            if(!frame->renderer_||!frame->renderer_->currentFor(s.rootInputs->issuer_)){
                s.runtimeReceipt.pendingRenderer=true;receipt=s.runtimeReceipt;return fail(d,S::phase,0x141F3C);
            }
            VehicleWindowDiagnostic50DS rendererDiagnostic;game::OriginalCameraStartResult rendererResult;
            if(!frame->renderer_->serviceEntity141F3C(rendererDiagnostic,rendererResult)){
                s.runtimeFailed=true;receipt=s.runtimeReceipt;return fail(d,S::phase,rendererResult.sourcePC);
            }
            s.runtimeReceipt.pendingRenderer=false;
            game::OriginalGarageCharacterDiagnostic50GN character;
            if(!game::updateOriginalGarageCharacter11AAE050GN(dt,*s.grant->characterShared(),
                actor->model->graph(),actor->fields,character)){
                s.runtimeFailed=true;receipt=s.runtimeReceipt;return fail(d,S::source,character.sourcePC);
            }
            // Character's actual model child mask0 gives a genuine141ED0 early
            // return. Do not bypass a newly introduced eligible descendant.
            if(child->descendants24&2u){s.runtimeFailed=true;receipt=s.runtimeReceipt;return fail(d,S::phase,0x11ACB8);}
            ++s.runtimeReceipt.updatedCharacters;
        }
        ++s.runtimeReceipt.visitedCharacters;
        // Exact141F68: read next14 AFTER the child's completed callback.
        s.nextRuntimeChild=child->next14;
    }
    s.runtimeReceipt.toyListComplete=true;s.runtimeReceipt.pendingRenderer=false;
    s.lastRuntimePublication=s.pendingFrame->publication_;s.pendingFrame.reset();
    receipt=s.runtimeReceipt;return true;
}

bool OriginalUiGarageActors50GN::trigger101C10(const char* actor,const char* sequence,D& d)noexcept {
    d={};if(!storage_||storage_->closed||storage_->failed||storage_->runtimeFailed||
        storage_->pendingFrame||storage_->stage!=Stage::complete||!storage_->grant->current())
        return fail(d,S::phase,0x101C10);
    game::GarageSequenceDiagnostic50GN diagnostic;
    if(!storage_->sequence.trigger(actor,sequence,diagnostic))return fail(d,S::source,diagnostic.sourcePC);
    return true;
}

bool OriginalUiGarageActors50GN::failedSampleFrameCurrent(
    const RuntimeFrame& frame,std::uint64_t publication)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    if(!s.failedSample10179C||!s.failedSampleAudioPreflight||!s.runtimeStarted||!s.runtimeFailed||s.failed||
       !s.sequence.failed_||s.sequence.servicing_||!publication||s.lastRuntimePublication||
       s.failedSamplePublication!=publication||s.failedSampleFrameIdentity!=&frame||
       s.runtimeReceipt.publication!=publication||s.runtimeReceipt.managerComplete||
       s.runtimeReceipt.toyListComplete||s.runtimeReceipt.visitedCharacters||
       s.runtimeReceipt.updatedCharacters||s.runtimeReceipt.pendingRenderer||s.nextRuntimeChild||
       s.failedSampleActor>=s.actors.size()||!s.runtimeReceipt.manager.drew||
       !s.runtimeReceipt.manager.actors[s.failedSampleActor].sampled||
       frame.publication_!=publication||frame.roots_!=s.rootInputs||!s.rootInputs||
       !frame.pause_.sameOwner(s.runtimePause)||!frame.timing_.sameOwner(s.runtimeTiming)||
       !frame.timing_.sameOwner(s.rootInputs->expectedTiming_)||!frame.renderer_||
       !frame.renderer_->currentFor(s.rootInputs->issuer_))return false;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!frame.timing_.readPrepared(sample,serial)||serial!=publication)return false;
    if(s.failedSampleFrameReleased)return failedSampleFrameReleasedCurrent(publication)&&!s.pendingFrame;
    return !s.closed&&s.managerLive&&s.managerPublished&&s.pendingFrame.get()==&frame&&
        s.sourceOrdered&&s.sourceSceneOwner;
}
bool OriginalUiGarageActors50GN::failedSampleFrameReleasedCurrent(std::uint64_t publication)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    return s.failedSample10179C&&s.failedSampleFrameReleased&&s.failedSamplePublication==publication&&
        s.failedSampleFrameIdentity&&s.failedSampleRetirementOwner&&s.runtimeFailed&&s.sequence.failed_&&
        !s.sequence.servicing_&&!s.pendingFrame&&!s.nextRuntimeChild&&s.closed&&!s.managerLive&&
        !s.toyLive&&!s.toyRetiring&&!s.manager49C.entity.reference00&&!s.toyList3C8.reference00&&
        s.pageRetirement==Storage::PageRetirement::complete&&s.stage==Stage::pageManagerRetired;
}
bool OriginalUiGarageActors50GN::failedSampleRetirementCurrent(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    if(!storage_||!page.actorSampleFailureCurrent())return false;const auto& s=*storage_;
    if(s.failedLateBorrowRetirementOwner||s.failedLoadedPageRetirementOwner||s.failedWattoBindingRetirementOwner||
       !s.failedSample10179C||s.failedSampleRetirementOwner!=&page||!s.runtimeFailed||
       !s.sequence.failed_||s.sequence.servicing_||s.nextRuntimeChild||
       s.failedSamplePublication!=page.updatePublication_||!s.failedSampleFrameIdentity)return false;
    if(s.failedSampleFrameReleased)return failedSampleFrameReleasedCurrent(s.failedSamplePublication);
    return s.pendingFrame&&s.pendingFrame.get()==s.failedSampleFrameIdentity&&
        failedSampleFrameCurrent(*s.pendingFrame,s.failedSamplePublication);
}
bool OriginalUiGarageActors50GN::retireFailedSampleToyList142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_||!page.actorSampleFailureCurrent())return fail(d,S::phase,0x10179C);
    auto& s=*storage_;
    if(s.failedLateBorrowRetirementOwner||s.failedLoadedPageRetirementOwner||s.failedWattoBindingRetirementOwner||
       (s.failedSampleRetirementOwner&&s.failedSampleRetirementOwner!=&page)||
       (!s.failedSampleRetirementOwner&&s.pageRetirement!=Storage::PageRetirement::none)||
       !s.pendingFrame||!failedSampleFrameCurrent(*s.pendingFrame,page.updatePublication_))
        return fail(d,S::phase,0x10179C);
    s.failedSampleRetirementOwner=&page;
    return retireAttachedToyList142590(page,d);
}
bool OriginalUiGarageActors50GN::retireFailedSampleAnimationManager101FE0(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!failedSampleRetirementCurrent(page))return fail(d,S::phase,0x101FE0);
    return retireAttachedAnimationManager101FE0(page,d);
}

bool OriginalUiGarageActors50GN::pageActorModelRetirementCurrent(
    const OriginalUiGaragePageOwner50KG& page,std::size_t ordinal,const R& model)const noexcept {
    if(!storage_||ordinal>=game::garageActorCount50GN)return false;
    const auto& s=*storage_;
    if(s.closed||s.failed||(s.runtimeFailed&&!failedSampleRetirementCurrent(page))||!s.sourceOrdered||!s.sourceSceneOwner||
       s.sourceSceneOwner->actorOwner3C8_!=this||s.pageRetirementOwner!=&page||
       s.pageRetirement!=Storage::PageRetirement::characters||s.nextRetirementActor!=ordinal+1||
       !s.toyLive||s.toyRetiring||!s.managerLive||!s.managerPublished||!s.grant||
       !s.grant->garagePageSceneTailCurrent(page,*s.sourceSceneOwner,8,s.toyList3C8))return false;
    const auto& actor=s.actors[ordinal];const auto* child=model.scene().modelEntity8();
    if(s.records34[ordinal]!=&actor||!actor.sourceToy||actor.sourceToy!=s.assets->actor(ordinal)||
       actor.model.get()!=&model||!actor.base.fields_.model04.has_value()||
       *actor.base.fields_.model04!=model.scene().model()||model.source()!=actor.sourceToy->model.get()||
       !actor.entityLive||!actor.attached||actor.retiring||
       !actor.membership||!actor.initialPose||actor.entity290.reference00!=1||
       actor.entity290.parent1C!=&s.toyList3C8||actor.entity290.next14||
       actor.entity290.previous10!=(ordinal?&s.actors[ordinal-1].entity290:nullptr)||
       actor.entity290.mask20!=2u||!child||child->parent1C!=&actor.entity290||
       child->next14||child->previous10||child->firstChild18||child->mask20)return false;
    for(std::size_t i=ordinal+1;i<s.actors.size();++i)
        if(s.actors[i].entityLive||s.actors[i].retiring||s.actors[i].attached||
           !s.actors[i].model||!s.actors[i].model->closed_||!s.actors[i].base.released_)
            return false;
    return true;
}

bool OriginalUiGarageActors50GN::failedLateBorrowActorAudioCurrent(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    if(!storage_||storage_->failedLoadedPageRetirementOwner||storage_->failedWattoBindingRetirementOwner||!page.lateSceneBorrowFailureCurrent())return false;
    const auto& s=*storage_;const auto* scene=page.scene4C0_;
    return scene&&page.sceneBacking_.get()==scene&&page.grant_==s.grant&&
        !page.grant_.owner_before(s.grant)&&!s.grant.owner_before(page.grant_)&&
        s.failedLateBorrowRetirementOwner==&page&&s.audio&&s.audio==scene->main_&&
        s.droids&&s.droids==scene->backend_&&
        (s.sourceSceneOwner==scene||s.pageRetirement==Storage::PageRetirement::complete)&&
        s.droids->preflightLateBorrowActorRetirement2353D0(page,*scene);
}

bool OriginalUiGarageActors50GN::failedLoadedPageActorAudioCurrent(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    if(!storage_||storage_->failedLateBorrowRetirementOwner||storage_->failedWattoBindingRetirementOwner||!page.loadedPageHideFailureCurrent())return false;
    const auto& s=*storage_;const auto* scene=page.scene4C0_;
    return scene&&page.sceneBacking_.get()==scene&&page.grant_==s.grant&&
        !page.grant_.owner_before(s.grant)&&!s.grant.owner_before(page.grant_)&&
        s.failedLoadedPageRetirementOwner==&page&&s.audio&&s.audio==scene->main_&&
        s.droids&&s.droids==scene->backend_&&
        (s.sourceSceneOwner==scene||s.pageRetirement==Storage::PageRetirement::complete)&&
        s.droids->preflightLoadedPageActorRetirement2353D0(page,*scene);
}

bool OriginalUiGarageActors50GN::failedWattoBindingActorAudioCurrent(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    if(!storage_||storage_->failedLateBorrowRetirementOwner||storage_->failedLoadedPageRetirementOwner||!page.wattoBindingFailureCurrent())return false;
    const auto& s=*storage_;const auto* scene=page.scene4C0_;
    return scene&&page.sceneBacking_.get()==scene&&page.grant_==s.grant&&
        !page.grant_.owner_before(s.grant)&&!s.grant.owner_before(page.grant_)&&
        s.failedWattoBindingRetirementOwner==&page&&s.audio&&s.audio==scene->main_&&
        s.droids&&s.droids==scene->backend_&&
        (s.sourceSceneOwner==scene||s.pageRetirement==Storage::PageRetirement::complete)&&
        s.droids->preflightWattoBindingActorRetirement2353D0(page,*scene);
}

bool OriginalUiGarageActors50GN::retirePageToyList142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_||storage_->failedSample10179C||storage_->failedLateBorrowRetirementOwner||storage_->failedLoadedPageRetirementOwner||storage_->failedWattoBindingRetirementOwner)
        return fail(d,S::phase,0x142590);
    return retireAttachedToyList142590(page,d);
}

bool OriginalUiGarageActors50GN::retireFailedLateBorrowToyList142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_||storage_->failedLoadedPageRetirementOwner||storage_->failedWattoBindingRetirementOwner||!page.lateSceneBorrowFailureCurrent())return fail(d,S::phase,0x142590);
    auto& s=*storage_;const auto* scene=page.scene4C0_;
    if(!scene||page.sceneBacking_.get()!=scene||s.sourceSceneOwner!=scene||
       !s.droids||s.droids!=scene->backend_||!s.audio||s.audio!=scene->main_||
       !s.droids->preflightLateBorrowActorRetirement2353D0(page,*scene)||
       (s.failedLateBorrowRetirementOwner&&s.failedLateBorrowRetirementOwner!=&page)||
       (!s.failedLateBorrowRetirementOwner&&s.pageRetirement!=Storage::PageRetirement::none))
        return fail(d,S::phase,0x142590);
    // A failure-specific invocation receipt, not a normal Page-entry receipt.
    // No Sound, voice, callback, or model field is changed by this publication.
    s.failedLateBorrowRetirementOwner=&page;
    return retireAttachedToyList142590(page,d);
}

bool OriginalUiGarageActors50GN::retireFailedLoadedPageToyList142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_||storage_->failedLateBorrowRetirementOwner||storage_->failedWattoBindingRetirementOwner||!page.loadedPageHideFailureCurrent())return fail(d,S::phase,0x142590);
    auto& s=*storage_;const auto* scene=page.scene4C0_;
    if(!scene||page.sceneBacking_.get()!=scene||s.sourceSceneOwner!=scene||
       !s.droids||s.droids!=scene->backend_||!s.audio||s.audio!=scene->main_||
       !s.droids->preflightLoadedPageActorRetirement2353D0(page,*scene)||
       (s.failedLoadedPageRetirementOwner&&s.failedLoadedPageRetirementOwner!=&page)||
       (!s.failedLoadedPageRetirementOwner&&s.pageRetirement!=Storage::PageRetirement::none))
        return fail(d,S::phase,0x142590);
    // An exact failed-loaded-Page invocation, not GUI77 or normal Page entry.
    // No Sound, voice, callback, or model field is changed by this publication.
    s.failedLoadedPageRetirementOwner=&page;
    return retireAttachedToyList142590(page,d);
}

bool OriginalUiGarageActors50GN::retireFailedWattoBindingToyList142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_||storage_->failedLateBorrowRetirementOwner||storage_->failedLoadedPageRetirementOwner||!page.wattoBindingFailureCurrent())return fail(d,S::phase,0x142590);
    auto& s=*storage_;const auto* scene=page.scene4C0_;
    if(!scene||page.sceneBacking_.get()!=scene||s.sourceSceneOwner!=scene||
       !s.droids||s.droids!=scene->backend_||!s.audio||s.audio!=scene->main_||
       !s.droids->preflightWattoBindingActorRetirement2353D0(page,*scene)||
       (s.failedWattoBindingRetirementOwner&&s.failedWattoBindingRetirementOwner!=&page)||
       (!s.failedWattoBindingRetirementOwner&&s.pageRetirement!=Storage::PageRetirement::none))
        return fail(d,S::phase,0x142590);
    // An exact unbound Watto-entry invocation, not another failed or normal Page entry.
    // No Sound, voice, callback, or model field is changed by this publication.
    s.failedWattoBindingRetirementOwner=&page;
    return retireAttachedToyList142590(page,d);
}

bool OriginalUiGarageActors50GN::retireAttachedToyList142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x142590);
    auto& s=*storage_;using Phase=Storage::PageRetirement;
    if(s.pageRetirement==Phase::toyRetired||s.pageRetirement==Phase::complete)return s.pageRetirementOwner==&page&&
        !s.toyLive&&!s.toyRetiring&&!s.toyList3C8.reference00&&!s.toyList3C8.firstChild18;
    if(s.closed||s.failed||(s.runtimeFailed&&!failedSampleRetirementCurrent(page))||!s.sourceOrdered||!s.sourceSceneOwner||
       s.sourceSceneOwner->actorOwner3C8_!=this||!s.sourceSceneOwner->sourceLoadReturned1FB59C()||
       s.sourceSceneOwner->grant_!=s.grant||s.sourceSceneOwner->assets_!=s.assets||
       !s.grant||!s.grant->current()||!s.visibility||s.visibility!=s.sourceSceneOwner->visibility_||
       !s.rootInputs||!s.audio||s.audio!=s.rootInputs->audio_||s.audio!=s.sourceSceneOwner->main_||
       !s.droids||s.droids!=s.sourceSceneOwner->backend_||
       s.droids->sourceSceneGroup_!=s.sourceSceneOwner||
       (s.failedLateBorrowRetirementOwner?!failedLateBorrowActorAudioCurrent(page):
        s.failedLoadedPageRetirementOwner?!failedLoadedPageActorAudioCurrent(page):
        s.failedWattoBindingRetirementOwner?!failedWattoBindingActorAudioCurrent(page):
           (s.droids!=s.audio->garageDroidAudioCold()||
            !s.droids->preflightSourceSceneRetirement2353D0(*s.sourceSceneOwner)))||
       !s.application||!s.application->known||!s.motionLibrary||!s.files||
       (s.pendingFrame&&!failedSampleRetirementCurrent(page))||s.nextRuntimeChild||s.sequence.servicing_||!s.managerLive||!s.managerPublished||
       s.grant->animationManager()!=&s.manager49C||s.manager49C.toys38!=&s.toyList3C8||
       (s.pageRetirementOwner&&s.pageRetirementOwner!=&page))return fail(d,S::phase,0x142590);
    // The native source callbacks are quiescent. Do not erase a pending tick,
    // stop a Sound, or reset Main's mixer to manufacture a teardown predecessor.
    if(s.pageRetirement<=Phase::listReference){
        if(!s.toyLive||s.toyRetiring||
           !s.grant->garagePageSceneTailCurrent(page,*s.sourceSceneOwner,8,s.toyList3C8))
            return fail(d,S::phase,0x1425B8);
    }else if(!s.toyRetiring||!s.grant->garagePageToyListBaseRetirementCurrent(
        page,*s.sourceSceneOwner,s.toyList3C8))return fail(d,S::phase,0x2D9B80);
    if(s.pageRetirement==Phase::none){
        if(s.stage!=Stage::complete||(s.failedSampleRetirementOwner?
           (!failedSampleRetirementCurrent(page)||!s.sequence.bound_||s.sequence.initialPoseCount_!=s.actors.size()):!s.sequence.ready())||s.constructed!=s.actors.size()||s.completed!=s.actors.size()||
           s.toyCount30!=s.records34.size()||s.retiredLoopSoundLists!=s.actors.size()||
           s.toyList3C8.kind!=K::toyList2D9B40||
           s.toyList3C8.mask20!=0xffffffffu)return fail(d,S::phase,0x2D9B80);
        for(std::size_t i=0;i<s.actors.size();++i){const auto& actor=s.actors[i];
            if(s.records34[i]!=&actor||actor.type00!=1||actor.sourceToy!=s.assets->actor(i)||
               !actor.entityLive||!actor.modelEnrolled||!actor.attached||!actor.membership||
               !actor.initialPose||actor.retiring||!actor.model||!actor.model->current()||
               !actor.base.constructed_||actor.base.released_||actor.configuration08||
               !actor.loopSound60||!actor.loopSound60->released||!actor.loopSound60->readFalse||
               !actor.auxiliary60.has_value()||*actor.auxiliary60)
                return fail(d,S::phase,0x2D9060);
        }
        s.pageRetirementOwner=&page;s.pageRetirement=Phase::characters;s.stage=Stage::pageCharacters;
    }
    if(s.pageRetirement==Phase::characters){
        while(s.nextRetirementActor){const auto ordinal=s.nextRetirementActor-1;
            auto& actor=s.actors[ordinal];auto& model=*actor.model;
            if(!actor.entityLive||!actor.attached||actor.entity290.next14||
               actor.entity290.parent1C!=&s.toyList3C8||
               actor.entity290.previous10!=(ordinal?&s.actors[ordinal-1].entity290:nullptr))
                return fail(d,S::phase,0x1425D8);
            if(!actor.retiring){
                auto& f=actor.base.fields_;
                if(actor.base.released_||!actor.motions.preflightRelease153F50().okay()||
                   !actor.cachePublished||f.self00!=&actor.entity290||f.dispatch21C!=K::character11A750||
                   f.entity220Adjustment30||
                   f.allocation1EC!=actor.records1EC.data()||f.allocation1F4!=actor.saved1F4.data()||
                   f.motionArray1E4!=actor.motions.motionArray1E4()||
                   f.allocation1F0||f.allocation1FC||f.allocation200||f.allocation20C||f.count208||
                   f.allocation210||f.allocation218||actor.configuration08)
                    return fail(d,S::phase,0x153F50);
                assets::OriginalStandardModelFileView50LA cached;
                assets::OriginalStandardTextureDiagnostic50LA cache;
                if(!s.files->modelFile(actor.mcfId,cached,cache)||cached.animation1536F0!=&actor.base)
                    return fail(d,S::phase,0x1540E0);
                if(!model.closed_){
                    if(!pageActorModelRetirementCurrent(page,ordinal,model))return fail(d,S::phase,0x142590);
                    if(!model.referenceReleased142590()){
                        if(!actor.modelEnrolled||!s.visibility->preflightPageActorObserver28976C(page,*this,ordinal,model)||
                           !model.canRetireResources(model.scene().model()->observer19C))
                            return fail(d,S::phase,0x240450);
                        const auto step=s.grant->retireModel(model.scene_);
                        if(!step.okay())return fail(d,S::phase,step.sourcePC);
                        actor.modelEnrolled=false;
                    }else if(actor.modelEnrolled||model.scene().modelEntity8()->reference00)
                        return fail(d,S::phase,0x104970);
                    if(model.pageRetirement_<=R::PageRetirement::payloadsReleased&&
                       !model.retirePagePayloads240450(d))return false;
                    if(model.pageRetirement_==R::PageRetirement::payloadsReleased&&
                       !model.retirePageSceneChildren2896D0(d))return false;
                    if(model.pageRetirement_==R::PageRetirement::childrenReleased&&
                       !s.visibility->retirePageActorObserver28976C(page,*this,ordinal,model,d))return false;
                    if(model.pageRetirement_==R::PageRetirement::observerReleased&&
                       !model.finishPageSceneBase1413E0(d))return false;
                }
                if(!model.closed_||model.pageRetirement_!=R::PageRetirement::complete||
                   actor.modelEnrolled||actor.entity290.firstChild18)return fail(d,S::phase,0x289778);
                // Only now does Character290 take its last reference. Its
                // model was genuinely destroyed, not merely unregistered.
                const auto step=s.grant->beginCharacterRetirement(actor.entity290,&s.toyList3C8);
                if(!step.okay())return fail(d,S::phase,step.sourcePC);
                actor.retiring=true;
            }
            if(!actor.base.released_){auto& f=actor.base.fields_;
                //11A888 calls153F50(a1=0): no second Entity220 destructor.
                f.dispatch21C=K::animation1536F0;f.entity220Adjustment30=-0x70;
                f.motion18=nullptr;
                std::vector<game::BodyMatrix48G>().swap(actor.saved1F4); //153FC4.
                const auto motion=actor.motions.release153F50();
                if(!motion.okay())return fail(d,S::phase,motion.sourcePC);
                f.count208=0; //154094 even on the actual null20C branch.
                std::vector<assets::MotionNodeBinding49O>().swap(actor.records1EC); //1540D0.
                s.files->clearAnimation1327D0(&actor.base);actor.cachePublished=false;
                //Source doesn't null freed1E4/1EC/1F4 or model04. The retained
                //native backing is a tombstone, never permission to sample it.
                actor.base.released_=true;
            }
            const auto finish=s.grant->finishCharacterRetirement(actor.entity290);
            if(!finish.okay())return fail(d,S::phase,finish.sourcePC);
            actor.entityLive=false;actor.retiring=false;actor.attached=false;
            // Actual11A89C/1413E0 has returned after153F50(0), including
            // motion/array/cache teardown. Retain proof, not freed source fields.
            s.files->recordGarageFinalMcfRelease153F50(actor.mcfId,actor.base);
            //membership remains source-attached in the dead SearchList row.
            --s.nextRetirementActor;
        }
        if(s.toyList3C8.firstChild18)return fail(d,S::phase,0x1425B8);
        s.pageRetirement=Phase::listReference;
    }
    if(s.pageRetirement==Phase::listReference){
        const auto step=s.grant->beginGaragePageToyListRetirement142590(page,*s.sourceSceneOwner,s.toyList3C8);
        if(!step.okay())return fail(d,S::phase,step.sourcePC);
        s.toyRetiring=true;s.pageRetirement=Phase::descriptors;s.stage=Stage::pageToyDescriptors;
    }
    if(s.pageRetirement==Phase::descriptors){
        //2D9BC8 visits descriptor rows forward AFTER all Character Entities.
        //2D87D0/2D83C0 free descriptor-owned arrays, not Scene4D8 Sound owners.
        while(s.retiredDescriptors<s.records34.size()){
            const auto i=s.retiredDescriptors;auto& actor=s.actors[i];
            if(s.records34[i]!=&actor||actor.entityLive||actor.retiring||!actor.base.released_||
               !actor.model||!actor.model->closed_)return fail(d,S::phase,0x2D9BEC);
            //2D83C0 reaches null60 from actual2D8FEC after this descriptor's
            //2795E0/readfalse/142590 lifetime, not constructor-only absence.
            if(!actor.auxiliary60.has_value()||*actor.auxiliary60)return fail(d,S::phase,0x2D8460);
            actor.sourceToy=nullptr;actor.sourceDescriptor={};
            s.sequence.bindings_[i]={}; //Native borrowed descriptor payload.
            ++s.retiredDescriptors;
        }
        s.sequence.bound_=false;s.records34.fill(nullptr); //2D9C10/18; count30 survives.
        s.pageRetirement=Phase::listBase;
    }
    if(s.pageRetirement==Phase::listBase){
        const auto step=s.grant->finishGaragePageToyListRetirement1413E0(page,*s.sourceSceneOwner,s.toyList3C8);
        if(!step.okay())return fail(d,S::phase,step.sourcePC);
        s.toyLive=false;s.toyRetiring=false;s.pageRetirement=Phase::toyRetired;s.stage=Stage::pageToyRetired;
    }
    return true;
}

bool OriginalUiGarageActors50GN::retirePageAnimationManager101FE0(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_||storage_->failedSample10179C||storage_->failedLateBorrowRetirementOwner||storage_->failedLoadedPageRetirementOwner||storage_->failedWattoBindingRetirementOwner)
        return fail(d,S::phase,0x101FE0);
    return retireAttachedAnimationManager101FE0(page,d);
}

bool OriginalUiGarageActors50GN::retireFailedLateBorrowAnimationManager101FE0(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!failedLateBorrowActorAudioCurrent(page))return fail(d,S::phase,0x101FE0);
    return retireAttachedAnimationManager101FE0(page,d);
}

bool OriginalUiGarageActors50GN::retireFailedLoadedPageAnimationManager101FE0(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!failedLoadedPageActorAudioCurrent(page))return fail(d,S::phase,0x101FE0);
    return retireAttachedAnimationManager101FE0(page,d);
}

bool OriginalUiGarageActors50GN::retireFailedWattoBindingAnimationManager101FE0(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!failedWattoBindingActorAudioCurrent(page))return fail(d,S::phase,0x101FE0);
    return retireAttachedAnimationManager101FE0(page,d);
}

bool OriginalUiGarageActors50GN::retireAttachedAnimationManager101FE0(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x101FE0);
    auto& s=*storage_;
    if(s.pageRetirement==Storage::PageRetirement::complete)
        return s.closed&&s.pageRetirementOwner==&page&&!s.managerLive&&!s.manager49C.entity.reference00;
    if(s.pageRetirement!=Storage::PageRetirement::toyRetired||s.pageRetirementOwner!=&page||
       !s.sourceSceneOwner||s.sourceSceneOwner->actorOwner3C8_!=this||!s.grant||!s.grant->current()||
       s.toyLive||s.toyRetiring||s.toyList3C8.reference00||s.toyList3C8.firstChild18||
       !s.managerLive||!s.managerPublished||s.manager49C.toys38!=&s.toyList3C8||
       !s.audio||s.audio!=s.sourceSceneOwner->main_||!s.droids||s.droids!=s.sourceSceneOwner->backend_||
       (s.failedLateBorrowRetirementOwner?!failedLateBorrowActorAudioCurrent(page):
        s.failedLoadedPageRetirementOwner?!failedLoadedPageActorAudioCurrent(page):
        s.failedWattoBindingRetirementOwner?!failedWattoBindingActorAudioCurrent(page):
           !s.droids->preflightSourceSceneRetirement2353D0(*s.sourceSceneOwner))||(s.pendingFrame&&!failedSampleRetirementCurrent(page))||s.nextRuntimeChild)
        return fail(d,S::phase,0x101FE0);
    const auto step=s.grant->retireGaragePageAnimationManager101FE0(page,*s.sourceSceneOwner,s.manager49C,s.toyList3C8);
    if(!step.okay())return fail(d,S::phase,step.sourcePC);
    //101FE0 does not clear3DF400 or manager38. Only native liveness and the
    //Scene's native Actor borrow end here. Main, Sound samples and voices live
    //on until the separately ordered Scene Group235150 destructor.
    s.managerLive=false;s.sourceSceneOwner->actorOwner3C8_=nullptr;s.sourceSceneOwner=nullptr;
    s.sourceOrdered=false;s.closed=true;s.pageRetirement=Storage::PageRetirement::complete;
    s.stage=Stage::pageManagerRetired;
    if(s.failedSampleRetirementOwner){
        // Native failed-call borrow ends only after actual101FE0/count release.
        // Session retains the same Frame/Display until terminal owner cleanup.
        s.pendingFrame.reset();s.failedSampleFrameReleased=true;
    }
    return true;
}

bool OriginalUiGarageActors50GN::closeCold142590(D& d)noexcept {
    d={};if(!storage_)return true;
    auto& s=*storage_;if(s.closed)return true;
    if(!s.grant||!s.grant->current()||s.pageRetirement!=Storage::PageRetirement::none||
       s.toyList3C8.parent1C||s.manager49C.entity.parent1C)
        return fail(d,S::phase,0x142590);
    // An unrepresented positive/read-failed2796A0 prefix remains owned.
    // Never clear descriptor60 or free its backing as cold rollback.
    for(const auto& actor:s.actors)if(actor.loopSound60&&
        actor.loopSound60->constructed&&!actor.loopSound60->released)
        return fail(d,S::phase,0x2796F8);
    s.pendingFrame.reset();s.nextRuntimeChild=nullptr;
    //142590 destroys sibling characters tail-first. Each actual model child
    //dies BEFORE the character's last-reference/destructor sequence. This also
    //keeps each warm PSG user ahead of its live geometry prototype on unwind.
    for(std::size_t i=s.actors.size();i>0;--i){auto& a=s.actors[i-1];
        if(!a.entityLive)continue;
        if(!a.motions.preflightRelease153F50().okay())return fail(d,S::phase,0x153FF4);
        if(a.cachePublished){
            assets::OriginalStandardModelFileView50LA cached;
            assets::OriginalStandardTextureDiagnostic50LA cache;
            if(!s.files->modelFile(a.mcfId,cached,cache)||cached.animation1536F0!=&a.base)
                return fail(d,S::phase,0x1540E0);
        }
        if(a.model&&!a.model->closed_){
            if(!a.modelEnrolled){
                if(a.model->referenceReleased142590()){
                    if(!a.model->releaseAfterEntity142590(d))return false;
                }else if(!a.model->closeUnpublished(d))return false;
            }else {
                const auto* model=a.model->scene().model();
                const void* observer=model?model->observer19C:nullptr;
                if(!a.model->canRetireResources(observer))return fail(d,S::phase,0x240450);
                if(a.membership||observer){
                    if(!s.visibility->removeActor28EA80(i-1,*a.model,d))return false;
                    a.membership=false;
                }
                if(!a.model->canRetireResources(nullptr))return fail(d,S::phase,0x240450);
                const auto step=s.grant->retireModel(a.model->scene());
                if(!step.okay())return fail(d,S::phase,step.sourcePC);
                a.modelEnrolled=false;
                if(!a.model->releaseAfterEntity142590(d))return false;
            }
        }
        const C* parent=a.attached?&s.toyList3C8:nullptr;
        if(!a.retiring){
            auto step=s.grant->preflightCharacterRetirement(a.entity290,parent);
            if(!step.okay())return fail(d,S::phase,step.sourcePC);
            step=s.grant->beginCharacterRetirement(a.entity290,parent);
            if(!step.okay())return fail(d,S::phase,step.sourcePC);
            a.retiring=true;
        }
        if(a.base.constructed_&&!a.base.released_){
            auto& f=a.base.fields_;
            //11A888->153F50(a1=0), after104970 and before11A89C->1413E0.
            f.dispatch21C=K::animation1536F0;f.entity220Adjustment30=-0x70;
            f.motion18=nullptr;
            std::vector<game::BodyMatrix48G>().swap(a.saved1F4); //153FB8.
            const auto motion=a.motions.release153F50();
            if(!motion.okay())return fail(d,S::phase,motion.sourcePC);
            f.count208=0;
            std::vector<assets::MotionNodeBinding49O>().swap(a.records1EC); //1540C4.
            if(a.cachePublished){s.files->clearAnimation1327D0(&a.base);a.cachePublished=false;}
            a.configuration08.reset(); // Native failed-load cleanup, if needed.
            a.base.released_=true;
        }
        const auto finish=s.grant->finishCharacterRetirement(a.entity290);
        if(!finish.okay())return fail(d,S::phase,finish.sourcePC);
        a.entityLive=false;a.retiring=false;a.attached=false;
        // Even a partial-load owner qualifies only after this complete real
        // last-reference callback, never merely because1327D0 ran.
        s.files->recordGarageFinalMcfRelease153F50(a.mcfId,a.base);
    }
    if(s.toyLive){
        if(!s.toyRetiring){
            const auto step=s.grant->beginToyListRetirement(s.toyList3C8);
            if(!step.okay())return fail(d,S::phase,step.sourcePC);
            s.toyRetiring=true;
        }
        //2D9B80 disposes non-Entity TOY descriptors and records34 after the
        // last reference but BEFORE1413E0 decrements the actual object count.
        for(auto& actor:s.actors){actor.sourceToy=nullptr;actor.sourceDescriptor={};}
        s.records34.fill(nullptr);
        const auto step=s.grant->finishToyListRetirement(s.toyList3C8);
        if(!step.okay())return fail(d,S::phase,step.sourcePC);
        s.toyLive=false;s.toyRetiring=false;
    }
    if(s.managerLive){
        const auto step=s.grant->retireEmptyPrefix(s.manager49C.entity,K::animationManager101330,nullptr);
        if(!step.okay())return fail(d,S::phase,step.sourcePC);
        s.managerLive=false;
    }
    // grant retirement clears native liveness only. Source3DF400 intentionally
    //keeps manager49C's address, exactly as101FE0 does. Never write it null.
    if(s.sourceSceneOwner){
        if(s.sourceSceneOwner->actorOwner3C8_!=this)return fail(d,S::phase,0x2D9C20);
        s.sourceSceneOwner->actorOwner3C8_=nullptr;s.sourceSceneOwner=nullptr;
    }
    s.sourceOrdered=false;s.closed=true;s.stage=Stage::closed;return true;
}

bool OriginalUiGarageActors50GN::inspect(Observation& out)const noexcept {
    out={};if(!storage_)return false;
    const auto& s=*storage_;out.stage=s.stage;out.actor=s.ordinal;
    out.constructedCharacters=s.constructed;out.completedActors=s.completed;
    out.failed=s.failed;out.sourceManagerPublished=s.managerPublished;out.sourceOrdered=s.sourceOrdered;
    out.toyList3C8=&s.toyList3C8;out.manager49C=&s.manager49C;
    out.loopSoundDescriptor=s.loopSoundDescriptor;out.retiredLoopSoundLists=s.retiredLoopSoundLists;
    if(s.loopSoundDescriptor<s.actors.size()&&s.actors[s.loopSoundDescriptor].loopSound60){
        const auto& loop=*s.actors[s.loopSoundDescriptor].loopSound60;
        out.loopSoundEntity=&loop.entity;out.loopSoundCount30=loop.count30;
        out.loopSoundConstructed=loop.constructed;out.loopSoundReadComplete=loop.readComplete;
        out.loopSoundReadFalse=loop.readFalse;out.loopSoundReleased=loop.released;
    }
    if(s.ordinal<s.actors.size()&&s.actors[s.ordinal].entityLive){
        const auto& a=s.actors[s.ordinal];out.character290=&a.entity290;
        out.fields=&a.fields;out.model=a.model.get();
    }
    return true;
}
// Append INSIDE namespace rr::platform in original_ui_garage_actors50gn.cpp,
//after its Storage definition. Uses newest frozen suffix type00 and complete
//sourceSceneOwner/current2742F0 identity. No actor source file is replaced here.
bool OriginalUiGaragePodScene50KG::resolveEmitterActor27CFF8(const char* mcf,
    const char* child,std::shared_ptr<OriginalUiGarageModelResource50KG>& output,
    game::SceneNodeIndex49P& selected,std::uint32_t& sourceRow,D& d)noexcept {
    d={};output.reset();selected=game::SceneNodeIndex49P::none;sourceRow=0;
    const auto stopped=[&](std::uint32_t pc){d.status=S::phase;d.sourcePC=pc;return false;};
    const auto sameName=[](const char* a,const char* b)noexcept {
        if(!a||!b)return false;
        while(*a&&*b){unsigned char x=static_cast<unsigned char>(*a++),y=static_cast<unsigned char>(*b++);
            if(x>='a'&&x<='z')x=static_cast<unsigned char>(x-'a'+'A');
            if(y>='a'&&y<='z')y=static_cast<unsigned char>(y-'a'+'A');
            if(x!=y)return false;}
        return !*a&&!*b;
    };
    if(!mcf||!child||!currentPrefix()||!staticModels_||!staticModels_->populationCurrent()||
       !actorOwner3C8_||!actorOwner3C8_->scenePopulationCurrent2742F0(*this)||
       !actorOverrideScan1B40B0_)return stopped(0x27CFF8);
    //27CC40 visits each actual zone list and its source count30/records34.
    //Authenticated Garage has ONE zone list. Actual root name38, not asset
    //filename or a caller-supplied absence flag, controls the first-match test.
    for(std::size_t i=0;i<staticModels_->count30_;++i){
        const auto* wrapper=staticModels_->entries34_[i];
        if(!wrapper||wrapper!=staticModels_->wrappers_[i].get()||!wrapper->model8||
           wrapper->model8!=staticModels_->models_[i].get()||!wrapper->model8->current())
            return stopped(0x27CC70);
        const auto* root=wrapper->model8->scene().model();
        if(!root||!root->constructed||root->released)return stopped(0x27CC88);
        if(sameName(root->name38.c_str(),mcf))return stopped(0x27CCA0); //Not an admitted Garage branch.
    }
    auto& storage=*actorOwner3C8_->storage_;
    for(std::size_t index=0;index<storage.toyCount30;++index){
        auto* actor=storage.records34[index];
        if(actor!=&storage.actors[index])return stopped(0x27D008);
        if(actor->type00!=1)continue;
        const auto* animation=actor->base.fields();
        if(!animation||!animation->model04||!actor->model||!actor->model->current()||
           animation->model04!=actor->model->scene().model()||!actor->sourceToy||
           actor->sourceToy!=assets_->actor(index)||actor->model->source()!=actor->sourceToy->model.get())
            return stopped(0x27D030);
        const auto& definition=animation->definition48;
        const auto slash=definition.find_last_of("/\\");
        const char* filename=definition.c_str()+(slash==std::string::npos?0:slash+1);
        if(!sameName(filename,mcf))continue; //13DA50 + source308F00.
        auto found=actor->fields.firstChild284; //NOT the outer model root.
        const auto graph=actor->model->graph();
        if(!game::validateSceneGraph49P(graph))return stopped(0x27D074);
        const auto objectAt=[&](game::SceneNodeIndex49P node)noexcept -> const game::OriginalRacerSceneObject50LA* {
            for(std::uint32_t row=0;row<actor->model->scene().sourceRowCount();++row){
                const auto* object=actor->model->scene().sourceRow(row);
                if(object&&object->sceneIndex==node&&object->constructed&&!object->released)return object;
            }
            return nullptr;
        };
        if(!objectAt(found))return stopped(0x27D074);
        if(!sameName(child,"none")){
            //Exact28C590 order: self, nextSibling188 subtree, firstChild184
            //subtree. Unlike a generic name-map, source first-match survives.
            std::array<game::SceneNodeIndex49P,game::maxSceneNodes49P> pending{};
            std::size_t used=0,visits=0;pending[used++]=found;found=game::SceneNodeIndex49P::none;
            while(used){const auto current=pending[--used];
                const auto integer=static_cast<std::int32_t>(current);
                if(integer<0||static_cast<std::uint32_t>(integer)>=graph.count||++visits>graph.count)
                    return stopped(0x28C590);
                const auto* object=objectAt(current);if(!object)return stopped(0x28C590);
                if(sameName(object->name38.c_str(),child)){found=current;break;}
                const auto& node=graph.nodes[integer];
                if(node.firstChild184!=game::SceneNodeIndex49P::none){
                    if(used>=pending.size())return stopped(0x28C590);pending[used++]=node.firstChild184;}
                if(node.nextSibling188!=game::SceneNodeIndex49P::none){
                    if(used>=pending.size())return stopped(0x28C590);pending[used++]=node.nextSibling188;}
            }
            if(found==game::SceneNodeIndex49P::none)return stopped(0x27D098);
        }
        //Native source indices[] were bound when the REAL model constructed,
        //including warm hierarchy cloning. Never infer PSG index from node-1.
        bool mapped=false;
        for(std::uint32_t original=0;original<actor->sourceToy->model->payload->nodeCount();++original)
            if(actor->model->indices_[original]==found){
                assets::PsgNode49O source{};
                if(!assets::readPsgNode49O(actor->sourceToy->model->hierarchy,original,source)||
                   !sameName(source.name,objectAt(found)->name38.c_str()))return stopped(0x27D098);
                sourceRow=original;mapped=true;break;
            }
        if(!mapped)return stopped(0x27D098);
        output=actor->model;selected=found;return true;
    }
    return stopped(0x27D700); //Never replace a required node with null.
}


}
