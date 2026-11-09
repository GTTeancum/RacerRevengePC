#include "original_ui_garage_watto_owner50kg.h"
#include "original_ui_main_session50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_widget1_owner50kg.h"
#include "original_ui_garage_renderer_lease50kg.h"
#include "../../../src/game/original_camera_start_native.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../src/game/original_ui_user_rotate50kg.h"
#include "original_ui_garage_watto_sound_group50kg.h"
#include "../../../src/game/original_racer_animation_cold50la.h"
#include "../../../src/game/original_motion_library50la.h"
#include <array>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <exception>
#include <limits>
#include <optional>
#include <vector>

namespace rr::platform {
namespace {
using C=game::OriginalWreckComponent50LA;
using K=game::OriginalWreckComponentKind50LA;
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool fail(D& d,S status,std::uint32_t pc)noexcept {d.status=status;d.sourcePC=pc;return false;}
bool retirementRejected(const char* branch,D& d)noexcept {
    std::fprintf(stderr,"original_garage_watto_retirement_rejected branch=%s status=%u pc=%u\n",
        branch,static_cast<unsigned>(d.status),d.sourcePC);
    return false;
}
bool retirementRejected(const char* branch,D& d,S status,std::uint32_t pc)noexcept {
    fail(d,status,pc);return retirementRejected(branch,d);
}
bool responseMotionName1D5320(std::uint32_t argument,std::int32_t side,int seconds,char(&name)[12])noexcept {
    if(argument>3u||(side!=0&&side!=1))return false;
    const bool departing=argument==0u||argument==3u;
    if(departing){
        // Purchase1D58D4 first changes zero to one; both departure paths
        // then clamp below three before formatting the actual wp motion.
        if(argument==0u&&seconds==0)seconds=1;
        if(seconds<3)seconds=3;
    }else if(seconds==5)seconds=4;
    if(seconds<0||seconds>9)return false;
    unsigned n=0;name[n++]='w';if(departing)name[n++]='p';name[n++]='_';
    name[n++]=side?'r':'l';name[n++]='t';name[n++]='_';
    name[n++]=static_cast<char>('0'+seconds);name[n++]='s';name[n]=0;return true;
}
std::string lower(std::string s){for(auto& c:s)if(c>='A'&&c<='Z')c=char(c+32);return s;}
game::OriginalMotionConfiguration50LA configuration(const assets::OriginalUiDat50KG& source){
    game::OriginalMotionConfiguration50LA out;
    for(const auto& section:source.sections())for(const auto& value:section.values)
        if(value.sourceValuePresent)out[lower(section.name)].emplace(lower(value.key),value.text);
    return out;
}
}
struct OriginalUiGarageWattoOwner50KG::Storage {
    using Group=OriginalUiGarageWattoSoundGroup50KG;
    struct Event {std::uint32_t id00=0;Group::Sound* sound04=nullptr;};
    struct Emitter {
        C entity;
        const C* group30=nullptr;
        const OriginalUiMainAudio50KG* manager1D8=nullptr;
        std::array<float,4> previous40{},position170{},velocity180{},vector1A0{};
        std::uint32_t serial50=0,flags158=0;
        std::string name54;
        std::uint8_t flags1D4Low5=0;
        std::uint16_t type1E4=0;
        float distance1D0=0,trigger1DC=0,chance1E0=100,near1B8=0,far1BC=0;
        std::uint32_t word1E8=0,word1EC=0;
        const void* sound1F0=nullptr;
        // The source's stack descriptor does not initialize these words.
        // No reached Watto operation consumes them; do not invent zeroes.
        std::optional<std::uint64_t> word190,word1B0;
        std::optional<std::uint32_t> word1C0;
        std::vector<std::unique_ptr<Event>> events200;
        std::uint32_t capacity208=0,growth20C=0;
        bool live=false,attached=false,retiring=false;
    } emitter;
    std::shared_ptr<Grant> grant;
    std::shared_ptr<const RootInputs> inputs;
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> sceneAssets;
    std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> assets;
    // Acquired only from the genuine later Page entry1FAC08, never by the
    // widget constructor. The current detached Page/Scene prefix cannot bind.
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility;
    OriginalUiGarageModelResources50KG* resources=nullptr;
    std::shared_ptr<assets::OriginalStandardTextureResolver50LA> files;
    C widget;
    const C* manager30=nullptr;
    const C* primary4C0=nullptr;
    std::array<std::int32_t,4> selectionFlags{};
    std::array<std::int32_t,2> wantsJoystick50{};
    std::size_t wantsStored=0;
    bool selected38=false;
    std::array<float,4> position70{{1,1,1,1}},look80{{0,0,0,1}},up90{{0,0,0,1}};
    std::array<std::uint8_t,6> responseCountE0{},responseStartE6{};
    bool responseCategoriesLoaded=false;
    std::optional<std::uint32_t> type34;
    Group::Sound* wingDC=nullptr;
    //Native pending controls retain this same concrete group allocation. No
    //additional source reference or second25-slot registry is introduced.
    std::shared_ptr<Group> group;
    game::OriginalRacerAnimationCold50LA base;
    std::shared_ptr<R> model;
    std::shared_ptr<const assets::OriginalUiGarageModelAsset50KG> modelAsset;
    game::OriginalModelMotionSet50LA motions;
    assets::OriginalUiDat50KG widgets,mcf;
    std::unique_ptr<game::OriginalMotionConfiguration50LA> configuration08;
    assets::OriginalStandardModelFileId50LA mcfId;
    assets::MotionNodeBindings49O rows{};
    game::ModelPlayback50KG capture;
    // Stable descriptors borrow the SAME actual153610/MotionLibrary owners.
    // No second animation, snapshot capture or private asset playback exists.
    std::array<game::ModelPlaybackMotion50KG,32> motionDescriptors{};
    const game::ModelPlaybackMotion50KG* descriptor(const game::OriginalMotionResource50LA* resource)const noexcept {
        for(std::size_t n=0;n<motionDescriptors.size();++n)
            if(motions.at(n)==resource)return &motionDescriptors[n];
        return nullptr;
    }
    const game::OriginalMotionResource50LA* resource(const game::ModelPlaybackMotion50KG* descriptor)const noexcept {
        if(!descriptor)return nullptr;
        for(std::size_t n=0;n<motionDescriptors.size();++n)
            if(&motionDescriptors[n]==descriptor)return motions.at(n);
        return nullptr;
    }
    game::ModelPlaybackWorkspace50KG entryWorkspace;
    game::ModelPlaybackIk50KG entryIk;
    const game::OriginalMotionResource50LA* current64=nullptr;
    const game::OriginalMotionResource50LA* wing68=nullptr;
    std::string motionNameBC;
    const OriginalUiGaragePageOwner50KG* entryPage=nullptr;
    bool entryAttempted=false,entryReturned1D5CE4=false;
    std::uint64_t updateSerial=0;
    const C* updateChild=nullptr;
    bool updateReturned=false,updateFailed=false;
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG updateDiagnostic;
    bool responseFailed=false;
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG responseDiagnostic;
    const OriginalUiGaragePageOwner50KG* activationPage90=nullptr;
    bool activationAttempted90=false,activationReturned1D5C00=false;
    bool normalManagerRetirement=false,modelRetiring=false;
    std::vector<assets::MotionNodeBinding49O> records1EC;
    std::vector<game::BodyMatrix48G> saved1F4;
    std::vector<std::uint32_t> wingMask6C;
    game::OriginalUiGarageWattoFields50KG constructorFields;
    float switchProbabilityD0=.25f,randomProbabilityD4=.25f; //1D4400/4404, not later1D4AA0.
    game::OriginalUiGarageWattoLifecycle50KG lifecycle;
    Stage stage=Stage::empty;
    bool widgetLive=false,widgetRetiring=false,animationEnrolled=false,animationAttached=false,animationRetiring=false;
    //Only actual235520 native enqueue may set this. Queue acceptance is the
    //source constructor contract, never applied wet DSP or entry readiness.
    bool constructorReturned1D46B8=false;
    bool cachePublished=false,modelEnrolled=false,failed=false,closed=false;
};
OriginalUiGarageWattoOwner50KG::OriginalUiGarageWattoOwner50KG()noexcept=default;
OriginalUiGarageWattoOwner50KG::~OriginalUiGarageWattoOwner50KG(){if(storage_&&!storage_->closed)std::terminate();}

bool OriginalUiGarageWattoOwner50KG::prepare1D4510(std::shared_ptr<Grant> grant,
    OriginalUiGarageModelResources50KG& resources,
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> sceneAssets,
    std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> assets,
    std::shared_ptr<const RootInputs> inputs,D& d)noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x1D4510);
    if(!grant||!grant->current()||grant->application()!=resources.application_||
       grant->generation_!=resources.generation_||!resources.issuer_||
       !inputs||inputs->issuer_!=resources.issuer_||inputs->issuer_!=grant->issuer()||inputs->grant_!=grant||
       inputs->grant_.owner_before(grant)||grant.owner_before(inputs->grant_)||
       !inputs->application_||!inputs->application_->known||!inputs->application_->clock101A50||
       !inputs->motions_||!inputs->audio_||!sceneAssets||!sceneAssets->loaded()||
       !assets||!assets->loaded()||!assets->bindings()||assets->bindings()->count!=32||
       !assets->hierarchy()||assets->hierarchy()->nodeCount!=32)return fail(d,S::input,0x1D4510);
    try {
        auto next=std::make_unique<Storage>();next->grant=std::move(grant);
        next->inputs=std::move(inputs);next->sceneAssets=std::move(sceneAssets);next->assets=std::move(assets);
        next->resources=&resources;
        next->files=resources.domain_->resolver3E1A70();
        next->group=std::make_shared<Storage::Group>();
        const auto archive=next->sceneAssets->archive();
        const auto psg=archive->find("UI/Anim/wg.psg");
        if(!psg.identity||psg.identity!=next->assets->geometrySource()||
           next->assets->hierarchy()->data!=psg.data||next->assets->hierarchy()->bytes!=psg.size)
            return fail(d,S::source,0x132820);
        assets::OriginalUiPageDiagnostic50KG text;
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UIGarage/Widgets.Dat",next->widgets,text,
                assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !assets::readOriginalUiDat50KG(*archive,"UI/Anim/wg.mcf",next->mcf,text,
                assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,S::source,0x1577D0);
        const auto* widget=next->widgets.section("Widget 10");
        if(!widget||!widget->find("Type")||widget->find("Type")->text!="Watto"||
           !widget->find("MCFFile")||widget->find("MCFFile")->text!="ui\\anim\\wg.mcf")return fail(d,S::source,0x1D4510);
        constexpr const char* selection[]{"EnableOnSelect","DisableOnSelect","EnableOnDeselect","DisableOnDeselect"};
        for(auto name:selection)if(widget->find(name))return fail(d,S::source,0x2A7830);
        storage_=std::move(next);auto& s=*storage_;
        auto step=s.grant->constructPrefix(s.widget,K::wattoWidget1D4320);
        if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}
        // Manager1CCBA0 invokes widgets+98 at1CCDCC BEFORE Page+88 at1CCDF4.
        // Neither1D4320 nor1D4510 reads a Scene zone or calls28EA20. Keep this
        // real constructor unbound; bit8, child-mask2 and155730 still follow.
        s.widgetLive=true;s.manager30=s.grant->manager();
        s.stage=Stage::widget;
        //1D4564/4590/45BC parse constructor vectors, but do not perform the
        //distinct later1D5BB0 placement or any observer bounds operation.
        std::array<float,3> position{},look{},up{};
        if(!assets::originalUiTriple50KG(*widget,"Position",{},position,text,0x1D4564)||
           !assets::originalUiTriple50KG(*widget,"LookVector",{},look,text,0x1D4590)||
           !assets::originalUiTriple50KG(*widget,"UpVector",{},up,text,0x1D45BC)||
           !s.assets->position()||position!=*s.assets->position()||
           !s.assets->look()||look!=*s.assets->look()||!s.assets->up()||up!=*s.assets->up()){
            s.failed=true;return fail(d,S::source,text.sourcePC?text.sourcePC:0x1D4564);}
        for(std::size_t i=0;i<3;++i){s.position70[i]=position[i];s.look80[i]=look[i];s.up90[i]=up[i];}
        std::string error;
        if(!s.group->constructGroup2350D0(s.grant,s.widget,s.inputs->audio_,error)){
            s.failed=true;return fail(d,S::phase,0x2350D0);}
        s.stage=Stage::audioGroup;
        const auto actual=s.grant->application();
        const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&s.base.entity220_));
        const auto made=s.base.constructFresh1536F0({s.grant->generation_,key,
            &actual->objectCount423F60(),&actual->attachmentLock423F58()});
        if(!made.okay()){s.failed=true;return fail(d,S::phase,made.sourcePC);}
        step=s.grant->enrollConstructedWattoAnimation(s.base);
        if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}
        s.animationEnrolled=true;s.stage=Stage::animation;
        auto& f=s.base.fields_;f.field18C=1;
        const auto provider=resources.domain_->archiveProvider3E1A70();
        const auto winner=[&](const assets::OriginalUiMemberView50KG& expected){
            assets::OriginalStandardArchiveProvider50LA::MemberView found;
            return provider&&provider->sealed()&&expected.identity&&expected.data&&
                provider->lookup132820(expected.identity->name,found)==assets::OriginalStandardArchiveProvider50LA::Lookup::present&&
                found.data&&found.size==expected.size&&!std::memcmp(found.data,expected.data,found.size);
        };
        const auto mcf=archive->find("UI/Anim/wg.mcf");
        if(mcf.identity!=s.mcf.identity()||!winner(mcf)){s.failed=true;return fail(d,S::source,0x132820);}
        assets::OriginalStandardTextureDiagnostic50LA cache;
        const std::vector<std::uint8_t> mcfBytes(mcf.data,mcf.data+mcf.size);
        if(!s.files->bindModelMember(mcf.identity->name,assets::OriginalStandardModelFileKind50LA::mcf,mcfBytes,s.mcfId,cache)){
            s.failed=true;return fail(d,S::source,cache.sourcePC);}
        assets::OriginalStandardModelFileView50LA cached;
        if(!s.files->modelFile(s.mcfId,cached,cache)){s.failed=true;return fail(d,S::source,cache.sourcePC);}
        // A real warm MCF clone is not a new bind-pose construction.
        if(cached.animation1536F0){s.failed=true;return fail(d,S::phase,0x153C20);}
        if(!s.files->publishAnimation1326A0(s.mcfId,&s.base,cache)){s.failed=true;return fail(d,S::phase,cache.sourcePC);}
        s.cachePublished=true;s.stage=Stage::cache;
        s.configuration08=std::make_unique<game::OriginalMotionConfiguration50LA>(configuration(s.mcf));
        f.configuration08=s.configuration08.get();f.definition48=widget->find("MCFFile")->text;
        const auto general=s.configuration08->find("general info");
        if(general==s.configuration08->end()){s.failed=true;return fail(d,S::source,0x153CB8);}
        const auto& section=general->second;
        const auto* slt=game::original_motion_detail50LA::value(section,"sltfile");
        const auto* vut=game::original_motion_detail50LA::value(section,"vutfilename");
        if(!slt||*slt!="wg.slt"||!vut||*vut!="wg.vut"||
           game::original_motion_detail50LA::value(section,"mirfilename")||
           game::original_motion_detail50LA::value(section,"mirroraxis")){
            s.failed=true;return fail(d,S::source,0x153CA0);}
        f.rig138=*slt;f.rigOverride188=0;f.directory190="ui\\anim\\";
        f.model98=*vut;f.motionE8="\\";f.motionCount14=32;
        f.field20=1;f.field24=1;f.field28=1;f.field2C=1;f.field0C=0;
        auto descriptor=std::make_shared<assets::OriginalUiGarageModelAsset50KG>();
        descriptor->archive=archive;descriptor->source=psg;descriptor->hierarchy=*s.assets->hierarchy();
        auto payload=std::make_shared<assets::OriginalStandardModelResource50LA>();
        assets::OriginalStandardResourceDiagnostic50LA resource;
        if(!payload->prepare(psg.identity->name,std::vector<std::uint8_t>(psg.data,psg.data+psg.size),resource)){
            s.failed=true;return fail(d,S::source,resource.sourcePC);}
        descriptor->payload=std::move(payload);s.modelAsset=std::move(descriptor);
        const bool loaded=resources.load(s.modelAsset,R::RootMode::separate,true,s.model,d);
        if(s.model&&s.model->scene().model())f.model04=s.model->scene().model();
        if(!loaded){s.failed=true;return false;}
        step=s.grant->enrollConstructedModel(s.model->scene());
        if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}s.modelEnrolled=true;
        step=s.grant->attachModel(s.base.entity220_,s.model->scene(),0u);
        if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}s.stage=Stage::model;
        const auto graph=s.model->graph();s.rows=*s.assets->bindings();
        for(std::uint32_t row=0;row<s.rows.count;++row){
            const auto original=s.rows.rows[row].sceneNode;
            if(original>=s.modelAsset->payload->nodeCount()){s.failed=true;return fail(d,S::source,0x152EA0);}
            const char* name=s.modelAsset->payload->sourceNodes()[original].name;
            const game::OriginalRacerSceneObject50LA* matched=nullptr;
            for(std::uint32_t n=0;n<s.model->scene().sourceRowCount();++n){const auto* node=s.model->scene().sourceRow(n);
                if(node&&game::original_motion_detail50LA::equal308F00(node->name38,name)){
                    if(matched){s.failed=true;return fail(d,S::source,0x152EA0);}matched=node;}}
            if(!matched||static_cast<std::uint32_t>(matched->sceneIndex)>=graph.count){s.failed=true;return fail(d,S::source,0x152EA0);}
            s.rows.rows[row].sceneNode=static_cast<std::uint8_t>(matched->sceneIndex);
        }
        f.nodeCount1E8=32;s.records1EC.assign(s.rows.rows,s.rows.rows+32);f.allocation1EC=s.records1EC.data();
        assets::OriginalStandardArchiveProvider50LA::MemberView mirror;
        if(provider->lookup132820(f.directory190+f.motionE8,mirror)!=assets::OriginalStandardArchiveProvider50LA::Lookup::absent||
           !winner(archive->find("UI/Anim/wg.vut"))){s.failed=true;return fail(d,S::source,0x153534);}
        f.field1F8=1;
        game::ModelPlaybackDiagnostic50KG playback;
        if(!game::captureModelPlaybackMatrices50KG(s.rows,graph,s.capture,playback)){
            s.failed=true;return fail(d,S::source,playback.sourceStage);}
        s.saved1F4.assign(s.capture.savedMatrices1F4,s.capture.savedMatrices1F4+s.capture.savedMatrixCount1F4);
        f.allocation1F4=s.saved1F4.data();
        game::OriginalMotionDefaults50LA defaults;defaults.directory190=f.directory190;defaults.rotationType28=1;
        const auto resolve=[&](const std::string& requested,game::OriginalAdmittedMotion50LA& out){
            for(std::size_t i=0;i<32;++i){const auto* motion=s.assets->motion(i);
                if(motion&&motion->source.identity&&game::original_motion_detail50LA::equal308F00(requested,motion->source.identity->name)){
                    if(motion->motion.data!=motion->source.data||motion->motion.bytes!=motion->source.size||!winner(motion->source))return false;
                    out.motion=&motion->motion;out.originalMemberName=motion->source.identity->name;return true;}}
            return false;
        };
        const auto motion=s.motions.load153610(*s.inputs->motions_,*s.configuration08,32,1,defaults,resolve);
        if(!motion.okay()){s.failed=true;return fail(d,S::source,motion.sourcePC);}f.motionArray1E4=s.motions.motionArray1E4();
        for(std::size_t n=0;n<s.motionDescriptors.size();++n){
            const auto* resource=s.motions.at(n);
            if(!resource){s.failed=true;return fail(d,S::source,0x153610);}
            s.motionDescriptors[n]={&resource->motion(),resource->fields().loopingB4!=0};
        }
        for(std::uint32_t row=0;row<32;++row)if(!game::setLocalMatrix49P(graph,
            static_cast<game::SceneNodeIndex49P>(s.rows.rows[row].sceneNode),s.saved1F4[row])){
            s.failed=true;return fail(d,S::source,0x1556C0);}
        s.configuration08.reset();s.stage=Stage::motions;
        const auto root=static_cast<std::uint32_t>(s.model->scene().model()->sceneIndex);
        graph.nodes[root].sceneFlags190=static_cast<std::uint8_t>(graph.nodes[root].sceneFlags190|8u);
        step=s.grant->attach(s.widget,s.base.entity220_,2u);
        if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}s.animationAttached=true;
        s.wingMask6C.assign(32,1u);
        for(const char* name:{"L_WING","R_WING","L_WING_TIP","R_WING_TIP"}){
            bool found=false;for(std::uint32_t row=0;row<32;++row)if(!std::strcmp(s.rows.rows[row].name,name)){
                s.wingMask6C[row]=0;found=true;break;}
            if(!found){s.failed=true;return fail(d,S::source,0x156580);}}
        // Second155730 reuses the SAME actual1F4 allocation, after mask setup.
        if(!game::captureModelPlaybackMatrices50KG(s.rows,graph,s.capture,playback)){
            s.failed=true;return fail(d,S::source,playback.sourceStage);}
        for(std::uint32_t row=0;row<32;++row)s.saved1F4[row]=s.capture.savedMatrices1F4[row];
        if(!s.group->appendWing232DB0(error)){
            if(s.group->wingRegistrationPending()){
                s.stage=Stage::wingPending;return fail(d,S::phase,0x26DAD0);}
            s.failed=true;return fail(d,S::phase,s.group->failureSourcePC()?s.group->failureSourcePC():0x232DB0);}
        s.failed=true;return fail(d,S::phase,0x26DAD0); //The private continuation owns this boundary.
    }catch(...){if(storage_)storage_->failed=true;return fail(d,S::allocation,0x1D4510);}
}

bool OriginalUiGarageWattoOwner50KG::continueWingRegistration1D464C(
    std::shared_ptr<OriginalAudioPendingWaves50KG> waves,D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x26DAD0);auto& s=*storage_;
    if(s.closed||s.failed||s.stage!=Stage::wingPending||!s.widgetLive||s.widget.parent1C||
       s.visibility||s.primary4C0||s.wingDC||!s.group||!s.group->wingRegistrationPending())
        return fail(d,S::phase,0x26DAD0);
    try {
        std::string error;
        if(!s.group->registerPendingWing2332A0(std::move(waves),error))return fail(d,S::phase,0x26DAD0);
        s.wingDC=s.group->members4C_[0]; //Actual returned232DB0 pointer, stored1D464C.
        if(!s.group->storeWingVolume233830(error)){s.failed=true;return fail(d,S::phase,0x1D4658);}
        assets::OriginalUiPageDiagnostic50KG text;const auto* widget=s.widgets.section("Widget 10");
        if(!widget||!assets::originalUiNumber50KG(*widget,"RandomMotionPct",.12f,s.randomProbabilityD4,text,0x1D4AD0)||
           !assets::originalUiNumber50KG(*widget,"SwitchSidesPct",.12f,s.switchProbabilityD0,text,0x1D4AF0)||
           s.randomProbabilityD4!=s.assets->randomMotionProbability()||
           s.switchProbabilityD0!=s.assets->switchSideProbability()){
            s.failed=true;return fail(d,S::source,text.sourcePC?text.sourcePC:0x1D4AA0);}
        if(!constructEmitter1D4850(d))return false;
        s.type34=15; //Factory1CD6AC a1=15, retained s2, store1D4678.
        s.stage=Stage::flushPending;
        return fail(d,S::phase,0x232EC0); //Root must perform the actual pending flush.
    }catch(...){s.failed=true;return fail(d,S::allocation,0x1D464C);}
}
bool OriginalUiGarageWattoOwner50KG::pendingFlush232EC0(
    std::shared_ptr<OriginalAudioPendingWaves50KG> waves)const noexcept {
    return storage_&&storage_->stage==Stage::flushPending&&!storage_->constructorReturned1D46B8&&
        constructorSubtreeCurrent()&&storage_->group->wingFlushPending232EC0(waves);
}
bool OriginalUiGarageWattoOwner50KG::continueAfterFlush1D4688(
    std::shared_ptr<OriginalAudioPendingWaves50KG> waves,D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x232EC0);auto& s=*storage_;
    if((s.stage!=Stage::flushPending&&s.stage!=Stage::reverbPending)||s.constructorReturned1D46B8||
       !constructorSubtreeCurrent()||!s.group->wingResolved26D9F0(waves))return fail(d,S::phase,0x232EC0);
    if(s.stage==Stage::reverbPending)return fail(d,S::phase,0x235520);
    //Source232EC0 returned only after actual latest Sound publication and
    //pending-count clearance. No caller bool, predecode, or early233E90 stands in.
    s.stage=Stage::reverbPending;std::string error;
    if(!s.group->configureReverb235520(error))return fail(d,S::phase,0x235520);
    //235520 is the final source call. After genuine enqueue, only infallible
    //local return-state publication remains. Wet DSP is still unapplied.
    s.constructorReturned1D46B8=true;s.stage=Stage::prepared;return true;
}

bool OriginalUiGarageWattoOwner50KG::visibilityBinding(VisibilityBinding& out)const noexcept {
    out={};if(!storage_||storage_->closed||!storage_->model||!storage_->modelEnrolled)return false;
    const auto& s=*storage_;out.grant=s.grant;out.model=s.model;
    out.sceneAssets=s.sceneAssets;out.assets=s.assets;out.visibility=s.visibility;
    out.widget=&s.widget;out.animation220=s.base.entity220();
    if(s.base.fields_.model04)out.animationModel04=*s.base.fields_.model04;
    out.primary4C0=s.primary4C0;return true;
}

bool OriginalUiGarageWattoOwner50KG::bindPageVisibility1FAC08(
    const OriginalUiGaragePageOwner50KG& page,
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility,D& d)noexcept {
    d={};d.field="Watto same-Page visibility binding required";
    if(!storage_)return fail(d,S::phase,0x1FAC08);auto& s=*storage_;
    if(s.closed||s.failed||s.normalManagerRetirement||s.stage!=Stage::prepared||
       !s.constructorReturned1D46B8||!s.activationAttempted90||!s.activationReturned1D5C00||
       s.activationPage90!=&page||s.entryAttempted||s.entryReturned1D5CE4||s.entryPage||
       s.visibility||s.primary4C0||!s.grant||!s.grant->current()||!s.inputs||
       !s.model||!s.model->scene().model()||s.model->scene().model()->observer19C||
       !subtreeCurrent(false))return fail(d,S::phase,0x1FAC08);
    if(page.grant_!=s.grant||page.grant_.owner_before(s.grant)||s.grant.owner_before(page.grant_)||
       page.issuer_!=s.inputs->issuer_||s.grant->issuer()!=page.issuer_||
       !s.grant->garageAttachedPageCurrent(page,page.entity_)||
       page.activationPhase_!=OriginalUiGaragePageOwner50KG::ActivationPhase::returned||
       page.entryPhase_!=OriginalUiGaragePageOwner50KG::EntryPhase::wattoBindingPending||
       !page.entryProfileCurrent()||!page.sourceLoadReturned1FAAC8()||
       page.watto33B0_!=this||!page.widgets_||!page.widgets_->ownsWattoRow(this)||
       page.widgets_->rows_[9].wattoEntity!=&s.widget||page.entryWattoModel_!=s.model.get()||
       page.entryWattoObserver_||!page.scene4C0_||page.sceneBacking_.get()!=page.scene4C0_||
       !visibility||visibility->sourceSceneOwner_!=page.scene4C0_||
       !visibility->matchesColdConstruction(s.grant,s.sceneAssets)||!page.entryWattoZone_||
       visibility->primaryZone4C0()!=page.entryWattoZone_)
        return fail(d,S::phase,0x1FAC08);
    //1FABFC..1FAC0C supplies this actual Page's Scene primary to Watto.
    //Retain native custody only; unchanged1D481C/1D4830 below still performs
    //the real remove/attach. Do not call subtreeCurrent between these stores
    //and attach: the observer does not exist yet. Partial failure keeps them.
    s.visibility=std::move(visibility);s.primary4C0=page.entryWattoZone_;
    d={};return true;
}

bool OriginalUiGarageWattoOwner50KG::observerUpdateCall1D4DD0(
    const OriginalUiGarageVisibility50KG& visibility,
    const OriginalUiGaragePageOwner50KG*& page,std::uint64_t& publication)const noexcept {
    page=nullptr;publication=0;
    if(!storage_)return false;
    const auto& s=*storage_;
    if(s.closed||s.failed||s.normalManagerRetirement||s.modelRetiring||
       s.stage!=Stage::entered||!s.entryReturned1D5CE4||!s.entryPage||
       !s.activationReturned1D5C00||s.activationPage90!=s.entryPage||
       !s.grant||!s.grant->current()||!s.inputs||!s.inputs->issuer_||
       s.inputs->grant_!=s.grant||s.grant->issuer()!=s.inputs->issuer_||
       !s.updateSerial||s.updateReturned||s.updateFailed||s.updateChild||
       s.visibility.get()!=&visibility||!s.primary4C0||!s.model||!s.model->current()||
       s.model->nativeGarageFifoPins_||!s.current64||!s.wing68||
       s.capture.current18||s.base.fields_.motion18||!s.base.fields_.model04||
       *s.base.fields_.model04!=s.model->scene().model()||
       s.entryPage->issuer_!=s.inputs->issuer_||s.entryPage->grant_!=s.grant||
       s.entryPage->entryPhase_!=OriginalUiGaragePageOwner50KG::EntryPhase::returned||
       s.entryPage->scene4C0_!=visibility.sourceSceneOwner_||
       !s.entryPage->wattoEntryBindingCurrent1FAC08(*this)||
       (s.widget.flags29&0x0du)!=1u||!(s.widget.mask20&2u))return false;
    ::original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!s.inputs->expectedTiming_.readPrepared(sample,serial)||serial!=s.updateSerial)return false;
    //Only advance1D4B10 publishes updateSerial for this prepared owner. Its
    //actual155A34 masked-motion return leaves both motion18 views null until
    //1D4DD0 calls28EA20;1D4DF4 restores them afterward. No permission is cached.
    page=s.entryPage;publication=serial;return true;
}

bool OriginalUiGarageWattoOwner50KG::normalModelObserverRetirementCurrent(
    const OriginalUiGarageVisibility50KG& visibility,const R& model,
    const OriginalUiGaragePageOwner50KG* page)const noexcept {
    if(!storage_||!page)return false;const auto& s=*storage_;
    if(s.closed||!s.normalManagerRetirement||!s.modelRetiring||!s.modelEnrolled||
       s.model.get()!=&model||s.visibility.get()!=&visibility||!s.grant||!s.grant->current()||
       !s.grant->garageManagerPageRetired1413E0(page)||
       !s.entryReturned1D5CE4||s.entryPage!=page||!s.activationReturned1D5C00||s.activationPage90!=page||
       !s.widgetLive||s.widgetRetiring||!s.animationEnrolled||!s.animationAttached||s.animationRetiring||
       !s.base.constructed_||s.base.released_||!s.inputs||s.inputs->grant_!=s.grant||
       s.inputs->issuer_!=s.grant->issuer())return false;
    const auto* animation=s.base.entity220();const auto* entity=model.scene().modelEntity8();
    return animation&&entity&&s.widget.parent1C==s.grant->manager()&&s.widget.reference00==1&&
        animation->parent1C==&s.widget&&animation->reference00==1&&
        entity->parent1C==animation&&!entity->reference00&&model.referenceReleased142590()&&
        s.base.fields_.model04&&*s.base.fields_.model04==model.scene().model();
}

bool OriginalUiGarageWattoOwner50KG::constructorSubtreeCurrent()const noexcept {
    return subtreeCurrent(true);
}
bool OriginalUiGarageWattoOwner50KG::subtreeCurrent(bool constructorOnly)const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    if(!storage_)return false;const auto& s=*storage_;
    if((s.failed&&(constructorOnly||(!s.entryAttempted&&!s.activationAttempted90)||!s.constructorReturned1D46B8))||
       s.closed||!s.type34||*s.type34!=15||!s.responseCategoriesLoaded||
       !s.grant||!s.grant->current()||!s.inputs||s.inputs->grant_!=s.grant||
       s.inputs->grant_.owner_before(s.grant)||s.grant.owner_before(s.inputs->grant_)||
       !s.resources||s.resources->application_!=s.grant->application()||
       s.resources->generation_!=s.grant->generation_||!s.widgetLive||s.widgetRetiring||
       !s.group||!s.group->sourceSoundsComplete()||!s.animationEnrolled||
       !s.animationAttached||s.animationRetiring||!s.modelEnrolled||!s.model||!s.model->current()||
       !s.cachePublished||!s.emitter.live||!s.emitter.attached||s.emitter.retiring||
       s.emitter.events200.size()!=40||s.wingMask6C.size()!=32||s.rows.count!=32||
       s.records1EC.size()!=32||s.saved1F4.size()!=32||
       (constructorOnly&&(s.visibility||s.primary4C0||s.entryAttempted||s.activationAttempted90))||
       s.wingDC!=s.group->members4C_[0])return false;
    const auto* manager=s.grant->manager();const auto* model=s.model->scene().model();
    const auto* modelEntity=s.model->scene().modelEntity8();
    const auto* animation=s.base.entity220();const auto* group=s.group->entity();
    const auto current=[&](const C* entity,K kind,const C* parent,std::uint32_t mask){
        return entity&&entity->kind==kind&&entity->reference00==1&&
            entity->sessionGeneration==s.grant->generation_&&
            entity->objectKey==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(entity))&&
            entity->parent1C==parent&&entity->mask20==mask;
    };
    const auto* parent=s.widget.parent1C;
    if(!manager||s.manager30!=manager||(parent&&parent!=manager)||
       !current(&s.widget,K::wattoWidget1D4320,parent,parent?0xffffffffu:0u)||
       (!parent&&(s.widget.previous10||s.widget.next14))||
       !current(group,K::soundGroup2350D0,&s.widget,0xffffffffu)||
       !current(animation,K::animation1536F0,&s.widget,2u)||
       !current(modelEntity,K::model2403A0,animation,0u)||
       !current(&s.emitter.entity,K::wattoEmitter2921C0,&s.widget,0xffffffffu)||
       s.widget.firstChild18!=group||group->previous10||group->next14!=animation||group->firstChild18||
       animation->previous10!=group||animation->next14!=&s.emitter.entity||animation->firstChild18!=modelEntity||
       modelEntity->previous10||modelEntity->next14||
       s.emitter.entity.previous10!=animation||s.emitter.entity.next14||s.emitter.entity.firstChild18||
       !model||!s.base.fields_.model04||*s.base.fields_.model04!=model)return false;
    if(constructorOnly){if(model->observer19C)return false;}
    else if(s.visibility){
        if(!s.primary4C0||!model->observer19C||
           !s.visibility->liveOrExpiredWattoObserverCurrent(*this))return false;
    }else if(s.primary4C0||model->observer19C)return false;
    // sourceSoundsComplete() above already performed the full same-Group
    // registry/ref/41-Sound/voice proof. Everything since that check is a
    // readonly identity/link/resource observation on this owner thread, not a
    // source callback or mutation. Validate every actual event and both member
    // pointers without recursively repeating that whole proof40 more times.
    // This local grouping never survives return or an actual source effect.
    for(std::size_t i=0;i<s.emitter.events200.size();++i){const auto& event=s.emitter.events200[i];
        if(!event||event->id00!=i+1u||
           !s.group->responseMemberIdentityAfterCurrent(event->id00,event->sound04))return false;
    }
    return true;
}
bool OriginalUiGarageWattoOwner50KG::constructorPublication(ColdPublication& out)const noexcept {
    out={};if(!storage_||!storage_->constructorReturned1D46B8||storage_->stage!=Stage::prepared||
       !constructorSubtreeCurrent()||!storage_->group->reverbEnqueueCurrent())return false;auto& s=*storage_;
    out.grant=s.grant;out.widget=&s.widget;out.animation220=s.base.entity220();
    out.modelEntity8=s.model->scene().modelEntity8();out.soundGroup=s.group->entity();
    out.responseEmitter=&s.emitter.entity;return true;
}

bool OriginalUiGarageWattoOwner50KG::managerRowPublication(ColdPublication& out)const noexcept {
    out={};if(!storage_||!storage_->constructorReturned1D46B8||
       (storage_->stage!=Stage::prepared&&storage_->stage!=Stage::entered)||!subtreeCurrent(false))return false;
    auto& s=*storage_;
    //The actual235520 return is historical. A later native queue consumption
    //does not unconstruct this live Widget or replay its constructor.
    out.grant=s.grant;out.widget=&s.widget;out.animation220=s.base.entity220();
    out.modelEntity8=s.model->scene().modelEntity8();out.soundGroup=s.group->entity();
    out.responseEmitter=&s.emitter.entity;return true;
}

OriginalUiGarageWattoOwner50KG::C* OriginalUiGarageWattoOwner50KG::borrowChildDraw1D4E60(
    const OriginalUiGaragePageOwner50KG& page,const C& receiver)const noexcept {
    if(!storage_)return nullptr;auto& s=*storage_;
    if(&receiver!=&s.widget||s.failed||s.closed||s.updateFailed||s.responseFailed||
       s.normalManagerRetirement||s.stage!=Stage::entered||!s.entryReturned1D5CE4||
       s.entryPage!=&page||!s.activationReturned1D5C00||s.activationPage90!=&page||
       !s.inputs||!s.inputs->issuer_||!s.inputs->application_||!s.inputs->application_->known||
       !subtreeCurrent(false)||!page.wattoEntryBindingCurrent1FAC08(*this)||
       !s.visibility||!s.visibility->current()||(s.widget.flags29&9u)!=1u||!(s.widget.mask20&8u))return nullptr;
    // The source draw does not test flags29 bit2. Group and Emitter retain
    // real draw-mask children and still need141DD8 service before+48. The
    // Animation's model child is mask0; Camera already owns model collection.
    return &s.widget;
}

bool OriginalUiGarageWattoOwner50KG::borrowPageWants50(
    const C* expected,std::array<std::int32_t,2>& out)const noexcept {
    if(!constructorSubtreeCurrent()||!storage_->constructorReturned1D46B8||storage_->wantsStored!=2||
       expected!=&storage_->widget||storage_->widget.parent1C!=storage_->grant->manager())return false;
    out=storage_->wantsJoystick50;return true;
}
bool OriginalUiGarageWattoOwner50KG::storeWants1CD8F4(std::size_t index,bool value,D& d)noexcept {
    d={};ColdPublication publication;
    if(!constructorPublication(publication)||index>=2||storage_->wantsStored!=index||
       publication.widget->parent1C)return fail(d,S::phase,0x1CD8F4);
    storage_->wantsJoystick50[index]=value?1:0;++storage_->wantsStored;return true;
}

bool OriginalUiGarageWattoOwner50KG::constructEmitter1D4850(D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x1D4850);auto& s=*storage_;auto& e=s.emitter;
    if(s.closed||s.failed||s.stage!=Stage::wingPending||e.live||!s.group||!s.group->current()||
       !s.group->wingRegistrationComplete_||!s.wingDC||s.wingDC!=s.group->members4C_[0]||
       !s.wingDC->source||s.wingDC->source->volume38!=17||!s.animationAttached)
        return fail(d,S::phase,0x1D4850);
    auto* shared=s.grant->emitterShared();
    if(!shared||!s.inputs->audio_){s.failed=true;return fail(d,S::input,0x290F10);}
    try {
        assets::OriginalUiDat50KG audio;assets::OriginalUiPageDiagnostic50KG text;
        const auto member=s.sceneAssets->archive()->find("audio/UIWatto.ini");
        const auto provider=s.resources->domain_->archiveProvider3E1A70();
        assets::OriginalStandardArchiveProvider50LA::MemberView winner;
        if(!member.identity||member.identity!=s.assets->audioSource()||!provider||!provider->sealed()||
           provider->lookup132820(member.identity->name,winner)!=assets::OriginalStandardArchiveProvider50LA::Lookup::present||
           !winner.data||winner.size!=member.size||std::memcmp(winner.data,member.data,member.size)){
            s.failed=true;return fail(d,S::source,0x132820);}
        if(!assets::readOriginalUiDat50KG(*s.sceneAssets->archive(),"audio/UIWatto.ini",audio,text)||
           audio.identity()!=s.assets->audioSource()){
            s.failed=true;return fail(d,S::source,0x1D4850);}
        const auto* responses=audio.section("WattoResponses");
        constexpr const char* keys[]{"NumberOfSliderResponses","NumberOfPurchaseResponses","NumberOfNoPurchaseResponses",
            "NumberOfNoMoneyResponses","NumberOfRandomResponses","NumberOfTransitionResponses"};
        constexpr std::int32_t expected[]{2,13,1,2,17,5};
        for(std::size_t i=0;i<6;++i){std::int32_t count=0;
            if(!responses||!assets::originalUiInteger50KG(*responses,keys[i],0,count,text,0x1D489C+static_cast<std::uint32_t>(i)*0x20u)||count!=expected[i]){
                s.failed=true;return fail(d,S::source,text.sourcePC?text.sourcePC:0x1D489C);}
            s.responseCountE0[i]=static_cast<std::uint8_t>(count);
        }
        s.responseStartE6[0]=1;
        for(std::size_t i=1;i<6;++i)s.responseStartE6[i]=static_cast<std::uint8_t>(s.responseStartE6[i-1]+s.responseCountE0[i-1]);
        s.responseCategoriesLoaded=true;
        auto step=s.grant->constructPrefix(e.entity,K::wattoEmitter2921C0);
        if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}e.live=true;
        //290F10 consumes the SAME process lazy assertion and serial. The
        //non-null actual root audio manager proves its reached assertion.
        if(!shared->guard8C5C){shared->assertion434C0=1;shared->guard8C5C=1;}
        e.manager1D8=s.inputs->audio_.get();e.group30=s.group->entity();
        const float largest=std::numeric_limits<float>::max();
        e.previous40={{largest,largest,largest,1.f}};e.position170=e.previous40;
        e.velocity180={};e.vector1A0={};e.flags158=0;e.distance1D0=0;
        e.trigger1DC=0;e.chance1E0=100;e.name54.clear();e.type1E4=6;e.flags1D4Low5=0;
        e.serial50=shared->serial434B8++;e.word1E8=0;e.word1EC=1;e.sound1F0=nullptr;
        e.previous40=shared->origin43470;
        e.events200.reserve(1);e.capacity208=1;e.growth20C=16; //294090(1,16).
        //292300->2911E0 exact reached active/configured branch. Source stack
        //descriptor produces these vectors/ranges; untouched words remain unknown.
        if(!(e.entity.flags29&1u)||!e.manager1D8){s.failed=true;return fail(d,S::phase,0x29123C);}
        e.position170={{0,0,0,1}};e.velocity180={{0,0,0,1}};e.vector1A0={{0,0,0,1}};
        e.flags158=2;e.trigger1DC=20;e.chance1E0=100;e.near1B8=0;e.far1BC=450;
        for(std::uint32_t id=1;id<=40;++id){
            //292754 allocates the event wrapper BEFORE calling Sound factory.
            auto event=std::make_unique<Storage::Event>();event->id00=id;
            std::string error;event->sound04=s.group->appendResponse232DB0(id,*s.assets,error);
            if(!event->sound04){s.failed=true;return fail(d,S::phase,
                s.group->failureSourcePC()?s.group->failureSourcePC():0x232DB0);}
            if(e.events200.size()==e.capacity208){
                e.events200.reserve(e.capacity208+e.growth20C);e.capacity208+=e.growth20C;}
            e.events200.push_back(std::move(event));
        }
        step=s.grant->attach(s.widget,e.entity,0xffffffffu);
        if(!step.okay()){s.failed=true;return fail(d,S::phase,step.sourcePC);}e.attached=true;
        s.stage=Stage::sounds;
        return true; //Local1573B0 configuration destruction precedes1D4678.
    }catch(...){s.failed=true;return fail(d,S::allocation,0x2921C0);}
}

bool OriginalUiGarageWattoOwner50KG::retirementCustody(
    std::shared_ptr<Grant>& grant,const C*& widget,bool& closed)const noexcept {
    grant.reset();widget=nullptr;closed=false;if(!storage_)return false;const auto& s=*storage_;
    if(!s.grant||!s.grant->current()||!s.inputs||s.inputs->grant_!=s.grant||
       s.inputs->grant_.owner_before(s.grant)||s.grant.owner_before(s.inputs->grant_)||
       s.inputs->issuer_!=s.grant->issuer()||s.widget.kind!=K::wattoWidget1D4320||
       s.widget.sessionGeneration!=s.grant->generation_||
       s.widget.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&s.widget))||
       s.manager30!=s.grant->manager()||
       s.widget.reference00!=((s.closed||s.widgetRetiring)?0u:1u))return false;
    grant=s.grant;widget=&s.widget;closed=s.closed;return true;
}
bool OriginalUiGarageWattoOwner50KG::preflightColdRetirement142590(D& d)const noexcept {
    return preflightRetirement142590(false,d);
}
bool OriginalUiGarageWattoOwner50KG::preflightManagerRetirement142590(D& d)const noexcept {
    return preflightRetirement142590(true,d);
}
bool OriginalUiGarageWattoOwner50KG::preflightRetirement142590(bool normal,D& d)const noexcept {
    d={};if(!storage_||storage_->closed)return true;const auto& s=*storage_;
    if(!s.grant||!s.grant->current()||(s.normalManagerRetirement&&!normal))return retirementRejected("preflight_authority",d,S::phase,0x142590);
    if(normal&&!s.grant->garageManagerPageRetired1413E0())return fail(d,S::phase,0x1FA170);
    if(!normal&&(s.visibility||s.primary4C0))return fail(d,S::phase,0x1FAC08);
    if(s.widgetLive&&!s.widgetRetiring){
        const auto* parent=s.widget.parent1C;
        if((parent&&parent!=s.grant->manager())||s.widget.reference00!=1||
           (!parent&&(s.widget.previous10||s.widget.next14))||
           (parent&&(s.widget.previous10?(s.widget.previous10->next14!=&s.widget||s.widget.previous10->parent1C!=parent):
               parent->firstChild18!=&s.widget))||
           (s.widget.next14&&(s.widget.next14->previous10!=&s.widget||s.widget.next14->parent1C!=parent)))
            return retirementRejected("preflight_outer_links",d,S::phase,0x142590);
    }
    const void* observer=s.model&&s.model->scene().model()?s.model->scene().model()->observer19C:nullptr;
    if(!s.visibility&&(s.primary4C0||observer))return fail(d,S::phase,0x28EA90);
    if(normal&&observer&&s.model&&!s.model->closed_&&
       !s.visibility->preflightWattoObserver28976C(*this))return fail(d,S::phase,0x28976C);
    if(s.group&&!s.group->preflightPendingRetirement235150())return fail(d,S::phase,0x235150);
    if(s.group&&s.group->constructed_&&!s.group->retired_){const auto& g=*s.group;
        if(!g.grant_||g.grant_!=s.grant||g.grant_.owner_before(s.grant)||s.grant.owner_before(g.grant_)||
           !g.audio_||!g.audio_->current()||g.audio_->sourceGroup_!=&g||!g.members4C_||
           g.count48_>g.capacity54_||g.count48_>g.audio_->soundCount||
           !g.grant_->preflightWattoSoundGroupRetirement(g.entity_,g.attached_?g.widget_:nullptr,g.slot40_).okay()||
           !g.audio_->canRevise(1)||!g.audio_->mixer_->canRecordControlChanges(g.count48_+g.audio_->instanceCount))
            return fail(d,S::phase,0x235150);
        for(std::size_t i=0;i<g.count48_;++i)if(!g.members4C_[i]||!g.audio_->sourceSoundCurrent(*g.members4C_[i]))
            return fail(d,S::phase,0x2353D0);
    }
    if(s.emitter.live&&!s.emitter.retiring){
        const auto emitter=s.grant->preflightWattoEmitterRetirement(s.emitter.entity,s.emitter.attached?&s.widget:nullptr);
        if(!emitter.okay())return retirementRejected("preflight_emitter",d,S::phase,emitter.sourcePC);
    }
    if(s.base.constructed_&&!s.base.released_){
        const auto motion=s.motions.preflightRelease153F50();
        if(!motion.okay())return fail(d,S::phase,motion.sourcePC);
        if(s.cachePublished){assets::OriginalStandardModelFileView50LA cached;
            assets::OriginalStandardTextureDiagnostic50LA cache;
            if(!s.files||!s.files->modelFile(s.mcfId,cached,cache)||cached.animation1536F0!=&s.base)
                return fail(d,S::phase,0x1540E0);}
        if(s.model&&!s.model->closed_&&s.model->pageRetirement_==R::PageRetirement::none&&
           !s.model->canRetireResources(observer))return fail(d,S::phase,0x240450);
    }
    return true;
}

bool OriginalUiGarageWattoOwner50KG::closeChildren142590(D& d)noexcept {
    return closeChildren142590(false,d);
}
bool OriginalUiGarageWattoOwner50KG::closeManagerChildren142590(D& d)noexcept {
    return closeChildren142590(true,d);
}
bool OriginalUiGarageWattoOwner50KG::closeChildren142590(bool normal,D& d)noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(!preflightRetirement142590(normal,d))return false;
    if(normal)s.normalManagerRetirement=true;
    // The manager retires later rows before row10, and row10 before rows1..9.
    // Do not destroy this subtree while an attached later sibling still lives.
    if(s.widgetLive&&!s.widgetRetiring){
        const auto* parent=s.widget.parent1C;
        if((parent&&parent!=s.grant->manager())||s.widget.next14||
           (!parent&&s.widget.previous10)||s.widget.reference00!=1||
           (parent&&(s.widget.previous10?s.widget.previous10->next14!=&s.widget:
               parent->firstChild18!=&s.widget)))return retirementRejected("children_outer_tail",d,S::phase,0x142590);
    }
    // Widget construction precedes Scene construction, so a null owner/zone
    // and null19C are the authentic unbound state, not missing membership to
    // repair. An unexpected observer without its retained owner is corruption;
    // refuse before retiring any child rather than dereferencing a null owner.
    if(!s.visibility&&(s.primary4C0||(s.model&&s.model->scene().model()&&
       s.model->scene().model()->observer19C)))return fail(d,S::phase,0x28EA90);
    // A still-pending representative pins this whole owner. Refuse before
    // destroying even the first child; never invent a clear/flush on failure.
    if(s.group&&!s.group->preflightPendingRetirement235150())
        return fail(d,S::phase,0x235150);
    // The source third child, when reached, dies BEFORE animation and group.
    if(s.emitter.live){
        auto& e=s.emitter;
        if(!e.retiring){
            const auto begin=s.grant->beginWattoEmitterRetirement(e.entity,e.attached?&s.widget:nullptr);
            if(!begin.okay())return retirementRejected("emitter_begin",d,S::phase,begin.sourcePC);
            e.retiring=true;
        }
        //292220 frees only event wrapper allocations; group retains Sounds.
        for(auto& event:e.events200)event.reset();
        std::vector<std::unique_ptr<Storage::Event>>().swap(e.events200);
        const auto finish=s.grant->finishWattoEmitterRetirement(e.entity);
        if(!finish.okay())return retirementRejected("emitter_finish",d,S::phase,finish.sourcePC);
        e.live=false;e.attached=false;e.retiring=false;
    }
    if(s.base.constructed_&&!s.base.released_){
        if(!s.motions.preflightRelease153F50().okay())return fail(d,S::phase,0x153FF4);
        if(s.cachePublished){assets::OriginalStandardModelFileView50LA cached;
            assets::OriginalStandardTextureDiagnostic50LA cache;
            if(!s.files->modelFile(s.mcfId,cached,cache)||cached.animation1536F0!=&s.base)
                return fail(d,S::phase,0x1540E0);}
        if(s.model&&!s.model->closed_){
            if(!s.modelEnrolled){if(!s.model->closeUnpublished(d))return false;}
            else if(normal){
                if(!s.modelRetiring){
                    const auto* entity=s.model->scene().modelEntity8();
                    if(!entity||entity->parent1C!=s.base.entity220()||entity->next14||entity->previous10||
                       entity->firstChild18||entity->reference00!=1||
                       s.model->pageRetirement_!=R::PageRetirement::none)
                        return retirementRejected("model_child",d,S::phase,0x142590);
                    const auto begin=s.grant->retireModel(s.model->scene());
                    if(!begin.okay())return retirementRejected("model_begin",d,S::phase,begin.sourcePC);
                    s.modelRetiring=true;
                }
                if(s.model->pageRetirement_<=R::PageRetirement::payloadsReleased&&
                   !s.model->retirePagePayloads240450(d))return retirementRejected("model_payloads",d);
                if(s.model->pageRetirement_==R::PageRetirement::payloadsReleased&&
                   !s.model->retirePageSceneChildren2896D0(d))return retirementRejected("model_scene_children",d);
                if(s.model->pageRetirement_==R::PageRetirement::childrenReleased){
                    if(s.model->pageObserverIdentity_){
                        if(!s.visibility||!s.visibility->retireWattoObserver28976C(*this,d))return retirementRejected("model_observer",d);
                    }else s.model->pageRetirement_=R::PageRetirement::observerReleased; //289760 actual null19C branch.
                }
                if(s.model->pageRetirement_==R::PageRetirement::observerReleased&&
                   !s.model->finishPageSceneBase1413E0(d))return retirementRejected("model_base",d);
                if(s.model->pageRetirement_!=R::PageRetirement::complete)return fail(d,S::phase,0x1413E0);
                s.modelEnrolled=false;s.modelRetiring=false;
            }else {
                const void* observer=s.model->scene().model()->observer19C;
                if(!s.model->canRetireResources(observer))return fail(d,S::phase,0x240450);
                //Detached constructor abort never acquired a Scene Observer.
                //Normal manager destruction above must NOT call28EA80.
                if(s.visibility||observer)return fail(d,S::phase,0x28EA90);
                if(!s.model->canRetireResources(nullptr))return fail(d,S::phase,0x240450);
                const auto retired=s.grant->retireModel(s.model->scene());
                if(!retired.okay())return fail(d,S::phase,retired.sourcePC);
                s.modelEnrolled=false;if(!s.model->releaseAfterEntity142590(d))return false;
            }
        }
        if(!s.animationEnrolled){
            const auto fresh=s.base.releaseFresh153F50();
            if(!fresh.okay())return fail(d,S::phase,fresh.sourcePC);
        }else {
            if(!s.animationRetiring){
                const auto begin=s.grant->beginWattoAnimationRetirement(s.base.entity220_,s.animationAttached?&s.widget:nullptr);
                if(!begin.okay())return retirementRejected("animation_begin",d,S::phase,begin.sourcePC);
                s.animationRetiring=true;
            }
            auto& f=s.base.fields_;
            // Genuine153F50(1), after reference0 and BEFORE Entity1413E0.
            f.dispatch21C=K::animation1536F0;f.entity220Adjustment30=0;f.motion18=nullptr;
            std::vector<game::BodyMatrix48G>().swap(s.saved1F4);
            const auto motion=s.motions.release153F50();
            if(!motion.okay())return fail(d,S::phase,motion.sourcePC);
            f.count208=0;std::vector<assets::MotionNodeBinding49O>().swap(s.records1EC);
            if(s.cachePublished){s.files->clearAnimation1327D0(&s.base);s.cachePublished=false;}
            s.configuration08.reset();
            const auto finish=s.grant->finishWattoAnimationRetirement(s.base.entity220_);
            if(!finish.okay())return retirementRejected("animation_finish",d,S::phase,finish.sourcePC);
            s.base.released_=true;s.animationEnrolled=false;s.animationAttached=false;s.animationRetiring=false;
            // Loaded153F50(1), configuration release and counted1413E0 have
            // all completed. Fresh/unpublished and interrupted paths do not mint this.
            s.files->recordGarageFinalMcfRelease153F50(s.mcfId,s.base);
        }
    }
    std::string error;
    if(s.group&&!s.group->closeGroup235150(error))return fail(d,S::phase,0x235150);
    return true;
}
bool OriginalUiGarageWattoOwner50KG::closeCold142590(D& d)noexcept {
    return close142590(false,d);
}
bool OriginalUiGarageWattoOwner50KG::closeManager142590(D& d)noexcept {
    return close142590(true,d);
}
bool OriginalUiGarageWattoOwner50KG::close142590(bool normal,D& d)noexcept {
    d={};if(!storage_||storage_->closed)return true;auto& s=*storage_;
    if(!closeChildren142590(normal,d))return false;
    if(s.widgetLive){
        if(!s.widgetRetiring){
            const auto begin=s.grant->beginWattoWidgetRetirement(s.widget,s.widget.parent1C);
            if(!begin.okay())return retirementRejected("widget_begin",d,S::phase,begin.sourcePC);
            s.widgetRetiring=true;
        }
        std::vector<std::uint32_t>().swap(s.wingMask6C); //1D44B4, after104970.
        const auto finish=s.grant->finishWattoWidgetRetirement(s.widget);
        if(!finish.okay())return retirementRejected("widget_finish",d,S::phase,finish.sourcePC);
        s.widgetLive=false;s.widgetRetiring=false;
    }
    s.closed=true;s.stage=Stage::closed;
    s.visibility.reset(); //Only native custody, after actual model Observer/destructor.
    return true;
}
bool OriginalUiGarageWattoOwner50KG::activatePageWidget90(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageWidget1Owner50KG& widgets,D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x1D5BB0);auto& s=*storage_;
    if(!widgets.pageWattoShowCurrent90(page,*this)||!s.grant||!s.grant->current()||
       s.closed||s.failed||s.normalManagerRetirement||!s.constructorReturned1D46B8||
       !subtreeCurrent(false))return fail(d,S::phase,0x2A79E0);
    if(s.activationReturned1D5C00)
        return s.activationPage90==&page?true:fail(d,S::phase,0x1D5C00);
    if(s.activationAttempted90||s.entryAttempted||s.stage!=Stage::prepared||s.visibility||
       s.primary4C0||s.model->scene().model()->observer19C)return fail(d,S::phase,0x1D5BB0);
    const auto graph=s.model->graph();const auto root=s.model->scene().model()->sceneIndex;
    const std::array<float,3> position{{s.position70[0],s.position70[1],s.position70[2]}};
    const std::array<float,3> look{{s.look80[0],s.look80[1],s.look80[2]}};
    const std::array<float,3> up{{s.up90[0],s.up90[1],s.up90[2]}};
    game::BodyMatrix48G pose;
    if(!game::originalUiUserRotatePose50KG(position,look,up,pose))return fail(d,S::source,0x289EC0);
    //The caller has already performed the real2A79E0→virtual24 recursive
    //show. Placement is later and operates on this animation's actual model04.
    s.activationAttempted90=true;s.activationPage90=&page;
    const game::SceneDisplacement49P translation{{s.position70[0],s.position70[1],s.position70[2],s.position70[3]}};
    if(!game::setLocalTranslation49P(graph,root,translation)){
        s.failed=true;return fail(d,S::source,0x28AB90);}
    if(!game::setLocalMatrix49P(graph,root,pose)){
        s.failed=true;return fail(d,S::source,0x289EC0);}
    s.activationReturned1D5C00=true;return true;
}

bool OriginalUiGarageWattoOwner50KG::enterPage1FAC1C(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_)return fail(d,S::phase,0x1FAC1C);auto& s=*storage_;
    if(!page.wattoEntryBindingCurrent1FAC08(*this)||!s.grant||!s.grant->current()||
       s.closed||s.failed||s.normalManagerRetirement||!s.constructorReturned1D46B8||
       !s.activationReturned1D5C00||s.activationPage90!=&page||
       !s.inputs||!s.inputs->application_||!s.inputs->motions_||!subtreeCurrent(false))
        return fail(d,S::phase,0x1FAC08);
    if(s.entryReturned1D5CE4){
        if(s.entryPage!=&page||s.stage!=Stage::entered||!s.selected38||!s.current64||!s.wing68||
           s.capture.current18!=s.descriptor(s.current64)||s.base.fields_.motion18!=s.current64)
            return fail(d,S::phase,0x1D5CE4);
        return true;
    }
    if(s.entryAttempted||s.stage!=Stage::prepared||s.capture.current18||s.base.fields_.motion18||
       s.base.fields_.field204||s.base.fields_.count208||s.base.fields_.allocation20C||
       s.base.fields_.field214||s.base.fields_.allocation218)return fail(d,S::phase,0x1D5C40);
    //Authored flags are all zero. Therefore2A78C0 writes selected38 only;
    //neither its EnableOnSelect nor DisableOnSelect child show/hide executes.
    for(auto flag:s.selectionFlags)if(flag)return fail(d,S::source,0x2A78C0);
    const auto* wing=s.inputs->motions_->find1523F0("wings_01");
    const auto* entry=s.inputs->motions_->find1523F0("wg_ent");
    bool ownsWing=false,ownsEntry=false;
    for(std::size_t i=0;i<static_cast<std::size_t>(s.motions.count14());++i){
        ownsWing=ownsWing||s.motions.at(i)==wing;ownsEntry=ownsEntry||s.motions.at(i)==entry;
    }
    if(!wing||!entry||!ownsWing||!ownsEntry||s.capture.savedMatrixCount1F4!=s.rows.count)
        return fail(d,S::source,0x1523F0);
    try{s.motionNameBC.reserve(12);}catch(...){return fail(d,S::allocation,0x1D5C40);}
    s.entryAttempted=true;s.entryPage=&page;
    s.selected38=true; //2A78C0 at1D5C54. Actual already-bound Observer is untouched.
    s.motionNameBC="wings_01";s.wing68=s.inputs->motions_->find1523F0(s.motionNameBC);
    s.motionNameBC="wg_ent";s.current64=s.inputs->motions_->find1523F0(s.motionNameBC);
    auto& application=*s.inputs->application_;
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG motion;
    const bool entered=game::enterOriginalGarageWattoMotion1D5CA850KG(*s.descriptor(s.current64),
        s.rows,s.model->graph(),s.entryIk,s.capture,application.modelMotion,
        application.modelMode0Ik,s.entryWorkspace,s.constructorFields,motion);
    //These are the same animation owner's canonical source fields, not an
    //independently seeded playback. Preserve155800 even if154480 is rejected.
    if(s.capture.current18==s.descriptor(s.current64))s.base.fields_.motion18=s.current64;
    s.base.fields_.field10=s.capture.stopped10?1u:0u;
    s.base.fields_.field30=s.capture.blending30?1u:0u;
    s.base.fields_.time34=s.capture.time34;
    if(!entered){s.failed=true;return fail(d,S::phase,motion.sourcePC);}
    s.entryReturned1D5CE4=true;s.stage=Stage::entered;return true;
}

bool OriginalUiGarageWattoOwner50KG::enter1D4800(game::OriginalUiGarageWattoReceipt50KG& r,
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG& d)noexcept {
    r={};d={};d.error=game::OriginalUiGarageWattoLifecycleError50KG::preflight;
    //1FAC08 is later than all22 widget constructors, Page/Scene construction,
    // and the loader flush. Only the real Page4C0 -> Scene4C0[0] path may issue
    // its zone for1D481C removal and1D4830 mode1 reattachment. No cold prefix
    // substitutes for that owner, and1D5C40 numeric entry is not this bind.
    //This legacy unscoped entry has no Page capability and stays closed.
    //The actual Page now calls Visibility::attachWatto1D4800, then the distinct
    //enterPage1FAC1C above. Constructor success cannot substitute either call.
    d.sourcePC=0x1FAC08;return false;
}
bool OriginalUiGarageWattoOwner50KG::stopPageSpeech2924A0(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};
    if(!storage_)return fail(d,S::phase,0x2924A0);
    auto& s=*storage_;
    if(s.failed||s.closed||s.normalManagerRetirement||s.stage!=Stage::entered||
       !s.entryReturned1D5CE4||s.entryPage!=&page||!s.inputs||!s.inputs->application_||
       !s.inputs->application_->known||!s.group||!s.group->audio_||
       s.group->main_.get()!=s.inputs->audio_.get()||!subtreeCurrent(false)||
       !page.wattoEntryBindingCurrent1FAC08(*this)||!page.speechStopPending_||
       !page.updatePublication_||page.speechStopPublication_!=page.updatePublication_||
       page.updateFailed_||page.updateReturned_||!s.emitter.live||s.emitter.retiring||
       !s.group->audio_->canExecute(0,true))return fail(d,S::phase,0x2924A0);
    // subtreeCurrent authenticates each actual emitter+200 row and its
    // Sound04. These forty response rows exclude the separate wing Sound.
    //292850 tail-calls Sound+20(sound,0); no motion/ready/exit/timer mutation.
    d.sourceEffectsCommitted=true;
    if(!s.group->audio_->stopAllResponses())return fail(d,S::source,0x292850);
    d.sourcePC=0x2924A0;return true;
}
bool OriginalUiGarageWattoOwner50KG::responseDuration1D5320(
    std::uint32_t event,int& seconds)const noexcept {
    if(!storage_||!storage_->group||!storage_->group->audio_)return false;
    const auto& s=*storage_;const OriginalUiGarageWattoAudio50KG::Sound* sound=nullptr;
    for(const auto& row:s.emitter.events200)
        if(row&&row->id00==event){sound=row->sound04;break;}
    float metadataDuration=0.f;
    if(!sound||!s.group->audio_->sourceSoundCurrent(*sound)||!sound->source||
       !s.group->audio_->queryDuration(event,metadataDuration))return false;
    const float value=sound->source->duration20;
    if(!std::isfinite(value)||value<=0.f||value>=10.f||
       std::bit_cast<std::uint32_t>(value)!=std::bit_cast<std::uint32_t>(metadataDuration))return false;
    // Established bounded native CVT.W.S truncating contract. Actual source
    // words do not independently establish the guest FCSR rounding mode.
    seconds=static_cast<int>(value);return true;
}
bool OriginalUiGarageWattoOwner50KG::remainingMotion1D5C10(
    const OriginalUiGaragePageOwner50KG& page,float& remaining,D& d)const noexcept {
    d={};remaining=0;
    const auto reject=[&](std::uint32_t pc)noexcept{return fail(d,S::phase,pc);};
    if(!storage_)return reject(0x1D5C10);const auto& s=*storage_;
    if(s.failed||s.closed||s.normalManagerRetirement||s.stage!=Stage::entered||
       !s.entryReturned1D5CE4||s.entryPage!=&page||!s.inputs||!subtreeCurrent(false)||
       !page.wattoEntryBindingCurrent1FAC08(*this)||!s.inputs->motions_||
       s.base.fields_.motion18!=s.current64||s.base.fields_.time34!=s.capture.time34)
        return reject(0x1D5C10);
    if(!s.current64){d.sourcePC=0x1D5C30;return true;}
    if(!s.inputs->motions_->owns(s.current64)||s.capture.current18!=s.descriptor(s.current64)||
       !std::isfinite(s.current64->fields().durationD0)||!std::isfinite(s.base.fields_.time34))
        return reject(0x1D5C24);
    // Exact remaining duration, NOT normalized time or a completion receipt.
    volatile float value=s.current64->fields().durationD0-s.base.fields_.time34;
    if(!std::isfinite(value))return reject(0x1D5C2C);
    remaining=value;d.sourcePC=0x1D5C30;return true;
}
bool OriginalUiGarageWattoOwner50KG::canRespondPage1D5320(
    const OriginalUiGaragePageOwner50KG& page,std::uint32_t argument,
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG& d)const noexcept {
    using E=game::OriginalUiGarageWattoLifecycleError50KG;
    d={};auto reject=[&](E error,std::uint32_t pc)noexcept {d.error=error;d.sourcePC=pc;return false;};
    // Only the retained Page transaction can call this private receiver:
    // purchase0, no-money1, slider2, and no-purchase3 are source arguments,
    // never a caller-selected sound ID or a separate native sound group.
    if(argument>3u)return reject(E::input,0x1D5320);
    if(!storage_)return reject(E::unprepared,0x1D5320);const auto& s=*storage_;
    if(s.responseFailed){d=s.responseDiagnostic;return false;}
    if(s.failed||s.closed||s.normalManagerRetirement||s.stage!=Stage::entered||
       !s.entryReturned1D5CE4||s.entryPage!=&page||!s.inputs||!s.inputs->application_||
       !s.inputs->issuer_||!s.inputs->application_->known||!subtreeCurrent(false)||
       !page.wattoEntryBindingCurrent1FAC08(*this)||!s.visibility||!s.visibility->current()||
       s.group->main_.get()!=s.inputs->audio_.get())return reject(E::preflight,0x1D5320);
    // Actual1D5374/1D5584 no-op is earlier than any range draw/audio/duration
    // or playback preflight. Purchase/NoPurchase are NOT readyA4 gated.
    if((argument==1u||argument==2u)&&!s.constructorFields.readyA4)return true;
    const auto& fields=s.constructorFields;const auto& base=s.base.fields_;
    if(std::fegetround()!=FE_TONEAREST||!s.inputs->application_->clock101A50||
       !s.inputs->motions_||!s.current64||!s.descriptor(s.current64)||
       s.capture.current18!=s.descriptor(s.current64)||base.motion18!=s.current64||
       (s.capture.previous1C&&!s.resource(s.capture.previous1C))||
       base.motion1C!=s.resource(s.capture.previous1C)||
       base.field10!=(s.capture.stopped10?1u:0u)||base.field30!=(s.capture.blending30?1u:0u)||
       base.time34!=s.capture.time34||base.time38!=s.capture.oldTime38||
       !std::isfinite(s.capture.time34)||s.capture.time34<0.f||
       !std::isfinite(s.capture.oldTime38)||!std::isfinite(s.capture.time34+1.f)||
       (fields.sideA8!=0&&fields.sideA8!=1)||s.motionNameBC.capacity()<12||
       !s.group->audio_->canExecute(1u,argument==0u||argument==3u))return reject(E::preflight,0x1D5320);
    for(std::size_t n=0;n<s.motionDescriptors.size();++n){
        const auto* motion=s.motions.at(n);const auto& descriptor=s.motionDescriptors[n];
        if(!motion||!s.inputs->motions_->owns(motion)||descriptor.motion!=&motion->motion()||
           descriptor.looping!=(motion->fields().loopingB4!=0))return reject(E::motion,0x1523F0);
    }
    constexpr std::array<std::size_t,4> categories{{1u,3u,0u,2u}};
    const std::size_t category=categories[argument];
    const std::uint32_t count=s.responseCountE0[category],first=s.responseStartE6[category];
    constexpr std::array<std::uint32_t,4> counts{{13u,2u,2u,1u}},starts{{3u,17u,1u,16u}};
    if(count!=counts[argument]||first!=starts[argument])return reject(E::input,0x1D5320);
    for(std::uint32_t event=first;event<first+count;++event){
        int seconds=0;char name[12]{};
        if(!responseDuration1D5320(event,seconds)||
           !responseMotionName1D5320(argument,fields.sideA8,seconds,name)||
           !s.descriptor(s.inputs->motions_->find1523F0(name)))return reject(E::audio,0x292510);
    }
    // Source virtualA0 only requests a manager transition and retains this
    // Page/Scene/Watto. It is not a license to retire any of these owners.
    return true;
}
bool OriginalUiGarageWattoOwner50KG::respondPage1D5320(
    const OriginalUiGaragePageOwner50KG& page,std::uint32_t argument,
    game::OriginalUiGarageWattoReceipt50KG& r,
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG& d)noexcept {
    using E=game::OriginalUiGarageWattoLifecycleError50KG;
    r={};d={};if(!canRespondPage1D5320(page,argument,d))return false;
    auto& s=*storage_;auto& fields=s.constructorFields;
    if((argument==1u||argument==2u)&&!fields.readyA4)return true;
    auto& application=*s.inputs->application_;auto& random=application.sharedRandomSelectors;
    r.effectsStarted=true;r.eligible=true;
    auto stop=[&](E error,std::uint32_t pc)noexcept {
        d.error=error;d.sourcePC=pc;s.responseFailed=true;s.responseDiagnostic=d;s.failed=true;return false;
    };
    auto positiveFloatInteger=[](std::uint32_t value)noexcept {
        unsigned shift=0;for(auto n=value;n>=0x01000000u;n>>=1)++shift;
        return shift?value&~((std::uint32_t{1}<<shift)-1u):value;
    };
    if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
    if(!random.seed.seed3D6B30){
        std::uint32_t timestamp=0;
        if(!game::sampleHostTimestamp50ID(*application.clock101A50,timestamp))return stop(E::clock,0x101A50);
        random.seed.seed3D6B30=timestamp;game::seedOriginalSharedRng50IU(application.sharedNoise308030,timestamp);
        r.seeded=true;
    }
    constexpr std::array<std::size_t,4> categories{{1u,3u,0u,2u}};
    const std::size_t category=categories[argument];
    const std::uint32_t count=s.responseCountE0[category],first=s.responseStartE6[category];
    const auto raw=game::drawOriginalSharedRng50IU(application.sharedNoise308030);++r.draws308030;
    const auto selected=static_cast<std::uint32_t>((std::uint64_t{positiveFloatInteger(raw)}*count)>>31);
    if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
    if(selected>=count)return stop(E::range,0x1D5320);
    const std::uint32_t event=first+selected;r.event=event;
    if(argument==1u||argument==2u){
        const auto chance=game::drawOriginalSharedRng50IU(application.sharedNoise308030);++r.draws308030;
        const float unit=static_cast<float>(positiveFloatInteger(chance))*0x1p-31f;
        // Exact float-to-double widening at311038; source compares to0.25.
        // A rejected choice still consumed BOTH shared draws and lazy effects.
        if(!(static_cast<double>(unit)<0.25))return true;
    }
    int seconds=0;
    if(argument==0u||argument==3u){
        if(!s.group->audio_->stopAllResponses())return stop(E::audio,0x2924A0);
        if(!s.group->audio_->playResponse(event,0,0))return stop(E::audio,0x292360);
        //292510/Sound+20 occurs AFTER the real play for purchase0 and case3.
        if(!responseDuration1D5320(event,seconds))return stop(E::audio,0x292510);
    }else{
        if(!responseDuration1D5320(event,seconds))return stop(E::audio,0x292510);
        if(!s.group->audio_->playResponse(event,0,0))return stop(E::audio,0x292360);
    }
    char name[12]{};
    if(!responseMotionName1D5320(argument,fields.sideA8,seconds,name))return stop(E::motion,0x1D5320);
    s.motionNameBC=name;fields.idleCooldownF4=2.f;fields.readyA4=false;
    if(argument==0u||argument==3u)fields.exitA0=true;
    s.current64=s.inputs->motions_->find1523F0(s.motionNameBC);
    const auto* descriptor=s.descriptor(s.current64);
    if(!s.current64||!descriptor)return stop(E::motion,0x1523F0);
    // Response's common tail has no Tick434510 assertion initialization,
    // observer/wing sample or clock advancement. Blend is exactly1 second.
    if(!game::transitionModelPlayback50KG(*descriptor,1.f,s.rows,s.model->graph(),s.capture,d.playback))
        return stop(E::motion,0x155830);
    auto& base=s.base.fields_;
    base.field10=s.capture.stopped10?1u:0u;base.field30=s.capture.blending30?1u:0u;
    base.motion18=s.resource(s.capture.current18);base.motion1C=s.resource(s.capture.previous1C);
    base.time34=s.capture.time34;base.time38=s.capture.oldTime38;
    r.accepted=true;return true;
}
bool OriginalUiGarageWattoOwner50KG::advance1D4B10(
    const OriginalUiGaragePageOwner50KG& page,
    const ::original_ui_source_timing50LH::PublicationView& timing,std::uint64_t serial,
    OriginalUiGarageRendererLease50KG& renderer,game::OriginalUiGarageWattoReceipt50KG& r,
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG& d)noexcept {
    using E=game::OriginalUiGarageWattoLifecycleError50KG;
    r={};d={};
    auto reject=[&](E error,std::uint32_t pc)noexcept {d.error=error;d.sourcePC=pc;return false;};
    if(!storage_)return reject(E::unprepared,0x1D4B10);
    auto& s=*storage_;
    if(s.updateFailed){d=s.updateDiagnostic;return false;}
    ::original_ui_source_timing50LH::Sample sample;std::uint64_t actualSerial=0;
    if(s.failed||s.closed||s.stage!=Stage::entered||!s.entryReturned1D5CE4||
       s.entryPage!=&page||!s.inputs||!s.inputs->application_||!s.inputs->issuer_||
       !s.inputs->application_->known||!s.inputs->application_->clock101A50||
       !timing.sameOwner(s.inputs->expectedTiming_)||!timing.readPrepared(sample,actualSerial)||
       !serial||serial!=actualSerial||serial<=s.updateSerial||
       !renderer.currentFor(s.inputs->issuer_)||!subtreeCurrent(false)||
       !page.wattoEntryBindingCurrent1FAC08(*this)||!s.visibility||!s.visibility->current()||
       s.group->main_.get()!=s.inputs->audio_.get()||
       (s.widget.flags29&0x0du)!=1u||!(s.widget.mask20&2u))
        return reject(E::preflight,0x1D4B10);
    const float dt=sample.updateSeconds;
    auto& application=*s.inputs->application_;
    auto& fields=s.constructorFields;
    auto& base=s.base.fields_;
    const auto* currentDescriptor=s.descriptor(s.current64);
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(dt)||dt<0.f||!std::isfinite(5.f*dt)||
       !s.inputs->motions_||!s.current64||!s.wing68||!currentDescriptor||
       s.wing68!=s.inputs->motions_->find1523F0("wings_01")||
       s.capture.current18!=currentDescriptor||base.motion18!=s.current64||
       (s.capture.previous1C&&!s.resource(s.capture.previous1C))||
       base.motion1C!=s.resource(s.capture.previous1C)||
       base.field10!=(s.capture.stopped10?1u:0u)||base.field30!=(s.capture.blending30?1u:0u)||
       base.time34!=s.capture.time34||base.time38!=s.capture.oldTime38||
       !std::isfinite(s.capture.time34)||!std::isfinite(s.capture.oldTime38)||
       !std::isfinite(s.capture.blendEnd3C)||!std::isfinite(s.capture.blendStart40)||
       !std::isfinite(s.capture.blendDuration44)||(s.capture.blending30&&s.capture.blendDuration44<=0.f)||
       !std::isfinite(s.current64->motion().duration)||s.current64->motion().duration<=0.f||
       !std::isfinite(fields.sideCooldownF8)||!std::isfinite(fields.idleCooldownF4)||
       !std::isfinite(fields.wingPhaseF0)||fields.wingPhaseF0<0.f||fields.wingPhaseF0>1.f||
       (fields.sideA8!=0&&fields.sideA8!=1)||s.motionNameBC.capacity()<12||
       !std::isfinite(s.switchProbabilityD0)||s.switchProbabilityD0<0.f||s.switchProbabilityD0>1.f||
       !std::isfinite(s.randomProbabilityD4)||s.randomProbabilityD4<0.f||s.randomProbabilityD4>1.f||
       application.wattoMotionAssertion434510>1u||
       (!application.wattoMotionGuard86B0&&application.wattoMotionAssertion434510!=0u))
        return reject(E::preflight,0x1D4B10);
    for(std::size_t n=0;n<s.motionDescriptors.size();++n){
        const auto* resource=s.motions.at(n);const auto& descriptor=s.motionDescriptors[n];
        if(!resource||!s.inputs->motions_->owns(resource)||
           s.inputs->motions_->find1523F0(resource->fields().name00)!=resource||
           descriptor.motion!=&resource->motion()||descriptor.looping!=(resource->fields().loopingB4!=0))
            return reject(E::preflight,0x1523F0);
    }
    const float ratio=s.capture.time34/s.current64->motion().duration;
    if(!std::isfinite(ratio))return reject(E::preflight,0x1D4B10);
    const bool eligible=ratio>0.8f&&!fields.exitA0;
    const bool mayRespond=eligible&&fields.idleCooldownF4<=0.f;
    volatile float wingDelta=5.f*dt;volatile float prospectiveWing=fields.wingPhaseF0+wingDelta;
    if(!std::isfinite(prospectiveWing)||!s.group->audio_->canExecute(
       std::size_t(mayRespond)+std::size_t(prospectiveWing>1.f),mayRespond))
        return reject(E::preflight,0x292360);
    // Resolve the actual292510 emitter rows, not PCM duration or an array
    // ordinal masquerading as a source Sound. The exact SourceSound+20 was
    // already published from authenticated metadata by104730/233060.
    auto duration=[&](std::uint32_t event,int& seconds)noexcept {
        const OriginalUiGarageWattoAudio50KG::Sound* sound=nullptr;
        for(const auto& row:s.emitter.events200)
            if(row&&row->id00==event){sound=row->sound04;break;}
        float metadataDuration=0.f;
        if(!sound||!s.group->audio_->sourceSoundCurrent(*sound)||!sound->source||
           !s.group->audio_->queryDuration(event,metadataDuration))return false;
        const float value=sound->source->duration20;
        if(!std::isfinite(value)||value<=0.f||value>=10.f||
           std::bit_cast<std::uint32_t>(value)!=std::bit_cast<std::uint32_t>(metadataDuration))return false;
        // Same bounded truncating conversion contract as the existing native
        // Rider/Watto math. ELF proves CVT.W.S, not an independent FCSR setup.
        seconds=static_cast<int>(value);return true;
    };
    auto speechName=[&](int seconds,char(&name)[9])noexcept {
        if(seconds==5)seconds=4; //actual1D5270, not a duration rewrite.
        if(seconds<0||seconds>9)return false;
        name[0]='w';name[1]='_';name[2]=fields.sideA8?'r':'l';name[3]='t';
        name[4]='_';name[5]=static_cast<char>('0'+seconds);name[6]='s';name[7]=0;
        return true;
    };
    if(eligible){
        for(const char* name:{"w_rt_br2","w_lt_br2","w_rt_lt_1","w_lt_rt_1"})
            if(!s.descriptor(s.inputs->motions_->find1523F0(name)))return reject(E::motion,0x1523F0);
        if(mayRespond)for(std::uint32_t event=19;event<=35;++event){
            int seconds=0;char name[9]{};
            if(!duration(event,seconds)||!speechName(seconds,name)||
               !s.descriptor(s.inputs->motions_->find1523F0(name)))return reject(E::audio,0x292510);
        }
    }
    for(auto old:fields.historyFC)if(old!=-1&&(old<19||old>35))return reject(E::preflight,0x1D5130);
    // Every allocation and backend capacity check precedes the first source
    // effect. Later failure retains the actual prefix and cannot replay a draw,
    // stop/play, shared lazy store or motion transition on another frame.
    s.updateSerial=serial;s.updateReturned=false;s.updateChild=nullptr;r.effectsStarted=true;r.eligible=eligible;
    auto stop=[&](E error,std::uint32_t pc)noexcept {
        d.error=error;d.sourcePC=pc;s.updateDiagnostic=d;s.updateFailed=true;s.failed=true;return false;
    };
    // Explicit positive CVT.S.W truncation contract, independent of host
    // integer-to-float rounding. No claim to recover an unobserved guest FCSR.
    auto positiveFloatInteger=[](std::uint32_t value)noexcept {
        unsigned shift=0;for(auto n=value;n>=0x01000000u;n>>=1)++shift;
        return shift?value&~((std::uint32_t{1}<<shift)-1u):value;
    };
    auto probability=[&]()noexcept {
        // Direct308030: unlike event selection this has NO lazy reseeding.
        ++r.draws308030;
        return static_cast<float>(positiveFloatInteger(game::drawOriginalSharedRng50IU(
            application.sharedNoise308030)))*0x1p-31f;
    };
    auto choice=[&](std::uint32_t first,std::uint32_t count,std::uint32_t& event)noexcept {
        auto& random=application.sharedRandomSelectors;
        if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
        if(!random.seed.seed3D6B30){
            std::uint32_t timestamp=0;
            if(!game::sampleHostTimestamp50ID(*application.clock101A50,timestamp))return stop(E::clock,0x101A50);
            random.seed.seed3D6B30=timestamp;game::seedOriginalSharedRng50IU(application.sharedNoise308030,timestamp);
            r.seeded=true;
        }
        const auto raw=game::drawOriginalSharedRng50IU(application.sharedNoise308030);++r.draws308030;
        const auto selected=static_cast<std::uint32_t>((std::uint64_t{positiveFloatInteger(raw)}*count)>>31);
        if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
        if(selected>=count)return stop(E::range,0x1D4F84);
        event=first+selected;r.event=event;return true;
    };
    const bool oldReady=fields.readyA4;
    if(fields.sideCooldownF8>=0.f){volatile float remaining=fields.sideCooldownF8-dt;
        fields.sideCooldownF8=remaining;}
    if(eligible){
        volatile float blend=.25f*s.current64->motion().duration;
        fields.readyA4=true;bool selected=false;
        if(fields.idleCooldownF4<=0.f){
            if(probability()<s.randomProbabilityD4){
                if(!s.group->audio_->stopAllResponses())return stop(E::audio,0x2924A0);
                std::uint32_t event=0,attempts=0;bool collision=false;
                do{
                    if(!choice(s.responseStartE6[4],s.responseCountE0[4],event))return false;
                    ++attempts;collision=false;
                    for(auto old:fields.historyFC)if(old==static_cast<std::int32_t>(event))collision=true;
                }while(collision&&attempts<s.responseCountE0[4]);
                // Source uses the oldest history row even if the LAST draw
                // became unique exactly at the attempt limit.
                if(attempts>=s.responseCountE0[4])event=static_cast<std::uint32_t>(fields.historyFC[4]);
                for(std::size_t n=4;n>0;--n)fields.historyFC[n]=fields.historyFC[n-1];
                fields.historyFC[0]=static_cast<std::int32_t>(event);r.event=event;
                int seconds=0;if(!duration(event,seconds))return stop(E::audio,0x292510);
                if(!s.group->audio_->playResponse(event,0,0))return stop(E::audio,0x292360);
                fields.lastRandomEventEC=static_cast<std::uint8_t>(event);
                char name[9]{};if(!speechName(seconds,name))return stop(E::motion,0x1D5270);
                s.motionNameBC=name;fields.idleCooldownF4=2.f;fields.readyA4=false;selected=true;r.accepted=true;
            }else if(fields.sideCooldownF8<=0.f&&probability()<s.switchProbabilityD0){
                if(fields.sideA8){fields.sideA8=0;s.motionNameBC="w_rt_lt_1";}
                else{fields.sideA8=1;s.motionNameBC="w_lt_rt_1";}
                fields.sideCooldownF8=30.f;
                std::uint32_t event=0;
                if(!choice(s.responseStartE6[5],s.responseCountE0[5],event))return false;
                if(!s.group->audio_->playResponse(event,0,0))return stop(E::audio,0x292360);
                fields.idleCooldownF4=2.f;fields.readyA4=false;selected=true;r.accepted=true;
            }
        }
        if(fields.readyA4&&!oldReady){
            s.motionNameBC=fields.sideA8?"w_rt_br2":"w_lt_br2";fields.idleCooldownF4=2.f;selected=true;
        }
        if(selected){
            s.current64=s.inputs->motions_->find1523F0(s.motionNameBC);
            if(!application.wattoMotionGuard86B0){
                application.wattoMotionAssertion434510=1;application.wattoMotionGuard86B0=true;
            }
            const auto* descriptor=s.descriptor(s.current64);
            if(!s.current64||!descriptor)return stop(E::motion,0x1D4D2C);
            if(!game::transitionModelPlayback50KG(*descriptor,blend,s.rows,s.model->graph(),s.capture,d.playback))
                return stop(E::motion,0x155830);
            //155830 has returned even if a later sample/observer fails.
            base.field10=s.capture.stopped10?1u:0u;base.field30=s.capture.blending30?1u:0u;
            base.motion18=s.resource(s.capture.current18);base.motion1C=s.resource(s.capture.previous1C);
            base.time34=s.capture.time34;base.time38=s.capture.oldTime38;
        }else{volatile float remaining=fields.idleCooldownF4-dt;fields.idleCooldownF4=remaining;}
    }
    if(!game::advanceModelPlayback50KG(dt,s.rows,s.model->graph(),s.entryIk,s.capture,
        application.modelMotion,application.modelMode0Ik,s.entryWorkspace,d.playback))
        return stop(E::motion,0x154480);
    base.field10=s.capture.stopped10?1u:0u;base.field30=s.capture.blending30?1u:0u;
    base.motion18=s.current64;base.motion1C=s.resource(s.capture.previous1C);
    base.time34=s.capture.time34;base.time38=s.capture.oldTime38;
    game::ModelMotionMask50KG mask;mask.count=static_cast<std::uint32_t>(s.wingMask6C.size());
    for(std::uint32_t i=0;i<mask.count;++i)mask.rows[i]=s.wingMask6C[i];
    game::ModelMotionState49T overlayState{1};
    if(!game::sampleModelMotionMasked50KG(s.wing68->motion(),s.rows,mask,fields.wingPhaseF0,
        s.model->graph(),overlayState,application.modelMotion,s.entryWorkspace.motion,d.overlay))
        return stop(E::motion,0x1559F0);
    s.capture.current18=nullptr;base.motion18=nullptr; //155A34, before observer.
    D observer;
    if(!s.visibility->updateWatto28EA20(*this,observer))return stop(E::observer,observer.sourcePC);
    s.capture.current18=s.descriptor(s.current64);base.motion18=s.current64;
    volatile float phaseDelta=5.f*dt;volatile float nextPhase=fields.wingPhaseF0+phaseDelta;
    fields.wingPhaseF0=nextPhase;
    if(fields.wingPhaseF0>1.f){
        fields.wingPhaseF0=0.f;
        // Actual retained wing Sound, after any selected response. No private
        // dry publication or source-history clear; prefix remains on failure.
        if(!s.group->audio_->playWing(2))return stop(E::audio,0x233410);
    }
    // The real inherited141ED0 selects Group, Animation, Emitter in that
    // order. Their virtual38 targets are235720,141ED0,141ED0 respectively.
    // Animation is NOT sampled again; Emitter has no spatial/audio callback.
    auto childEligible=[](const C& node)noexcept {return (node.flags29&0x0du)==1u&&(node.mask20&2u);};
    if(s.widget.descendants24&2u)s.updateChild=s.widget.firstChild18;
    while(s.updateChild){
        const C* child=s.updateChild;
        if(child->parent1C!=&s.widget)return stop(E::base,0x141F00);
        if(childEligible(*child)){
            VehicleWindowDiagnostic50DS rd;game::OriginalCameraStartResult result;
            if(!renderer.serviceEntity141F3C(rd,result))return stop(E::base,result.sourcePC);
            if(child==s.group->entity()){
                if(!s.group->serviceGroup235720(dt))return stop(E::base,0x235720);
            }else if(child==s.base.entity220()){
                // Exact sole model child has mask0. Even a retained aggregate
                // descendant bit cannot authorize a nonexistent model tick.
                if(child->descendants24&2u)for(auto* model=child->firstChild18;model;model=model->next14)
                    if(model!=s.model->scene().modelEntity8()||model->parent1C!=child||childEligible(*model))
                        return stop(E::base,0x141F60);
            }else if(child==&s.emitter.entity){
                if((child->descendants24&2u)&&child->firstChild18)return stop(E::base,0x141F60);
            }else return stop(E::base,0x141F60);
        }
        s.updateChild=child->next14; //Actual141F68, only after return.
    }
    s.updateReturned=true;return true;
}
bool OriginalUiGarageWattoOwner50KG::inspect(Observation& out)const noexcept {
    out={};if(!storage_)return false;const auto& s=*storage_;
    out.stage=s.stage;out.widget=s.widgetLive?&s.widget:nullptr;
    out.animation220=s.base.entity220();out.animationFields=s.base.fields();
    out.soundGroup=s.group?s.group->entity():nullptr;
    out.responseEmitter=s.emitter.live?&s.emitter.entity:nullptr;
    out.model=s.model.get();out.fields=s.lifecycle.fields()?s.lifecycle.fields():&s.constructorFields;
    out.selected38=s.selected38;out.failed=s.failed;return true;
}
}
