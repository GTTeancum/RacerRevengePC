#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_actors50gn.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_renderer_lease50kg.h"
#include <cfenv>
#include <limits>
#include <cmath>
#include "original_ui_garage_static_models50kg.h"
#include "original_ui_garage_scene_collision50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "original_ui_garage_shared_page50kg.h"
#include "original_ui_rider_profile50kg.h"
#include "../../../src/game/original_ui_garage_state50kg.h"
#include "../../../src/game/original_shared_rng50iu.h"
#include "../../../src/game/original_camera_scene_tick.h"
#include "../../../src/game/original_camera_start_effects.h"
#include <bit>
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <exception>
#include <new>
#include <cstring>
namespace rr::platform {namespace {
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
OriginalUiGaragePageOwner50KG::OriginalUiGaragePageOwner50KG()noexcept=default;
OriginalUiGaragePageOwner50KG::~OriginalUiGaragePageOwner50KG()noexcept {
    // Never destroy a counted Page, source-last-ref Scene, or canonical
    // embedded primitives by a host reverse-member destructor. In particular,
    // a failed shared loader leaves the exact owner retained at the Root.
    if(entityLive_||sceneBacking_||retiredSceneBacking_||quadCount_||textCount_||router_||controlConstructionCount_||groupConstructionCount_)std::terminate();
}
bool OriginalUiGaragePageOwner50KG::fail(Diagnostic& d,Status status,std::uint32_t pc)const noexcept {
    d.status=status;d.sourcePC=pc;d.sourceEffectsCommitted=entityLive_;return false;
}
bool OriginalUiGaragePageOwner50KG::inputsCurrent()const noexcept {
    return ownerThread_==std::this_thread::get_id()&&issuer_&&grant_&&grant_->current()&&
        grant_->issuer()==issuer_&&sameOwner(authority_,grant_->application())&&
        sameOwner(authority_,issuer_->entities())&&widgets_&&fog_&&
        widgets_->issuer_==issuer_&&sameOwner(widgets_->authority_,authority_)&&
        sameOwner(widgets_->grant_,grant_)&&sameOwner(widgets_->fog_,fog_)&&
        sameOwner(fog_->grant_,grant_)&&fog_->currentAttached()&&
        (entryPhase_==EntryPhase::notEntered?
            (activationPhase_==ActivationPhase::notActivated?widgets_->currentColdComplete():
                widgets_->pageActivationCustodyCurrent(*this)):widgets_->pageEntryCustodyCurrent(*this));
}
void OriginalUiGaragePageOwner50KG::constructQuad2DEF40(std::uint32_t offset)noexcept {
    if(quadCount_>=quads_.size())std::terminate();auto& q=quads_[quadCount_++];
    if(!q.construct2DEF40(offset))std::terminate();
}
void OriginalUiGaragePageOwner50KG::constructText2E02F0(TextStorage& storage)noexcept {
    if(storage.value)std::terminate();
    storage.value=::new(static_cast<void*>(storage.bytes.data())) Text;
    ++textCount_;
}
bool OriginalUiGaragePageOwner50KG::prepare1F9FF0(const RootInputs& in,Diagnostic& d)noexcept {
    d={};if(stage_!=Stage::empty||entityLive_||ownerThread_!=std::this_thread::get_id())
        return fail(d,Status::phase,0x1D1C14);
    issuer_=in.issuer();authority_=in.authority();grant_=in.grant();widgets_=in.widgets();fog_=in.fog();
    stage_=Stage::bound;
    if(!inputsCurrent())return fail(d,Status::input,0x1D1C14);
    //1F9FF0 ->1D5EA0 ->2A5C70 ->141300, exactly ONE counted prefix.
    d.scope=grant_->constructGaragePage1F9FF0(this,widgets_.get(),fog_.get(),entity_);
    if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    entityLive_=true;stage_=Stage::entity;vtable_=0x3D3C60;
    base_.staticWidgetCount13C=base_.controlCount140=0;base_.controls144=nullptr;
    base_.router134=nullptr;base_.groupCount148=0;base_.groups14C=nullptr;base_.pageNumber138=0;
    base_.extra150=nullptr;base_.backgroundTexture154=nullptr;base_.moviePlayer158=nullptr;
    base_.moviePlayer158=nullptr;base_.moviePathFirstByte15C=0;base_.field260=base_.field264=0;
    base_.input268=1;stage_=Stage::baseConstructed;
    //1D5EA0 constructs the4 inline prompts, then the actual ScreenText440.
    vtable_=0x3D16C0;
    for(std::uint32_t i=0;i<4;++i)constructQuad2DEF40(0x2A0+0x60*i);
    constructText2E02F0(sharedText440_);
    shared_.openSize274=224;shared_.time4B0=0;shared_.top27C=0;
    shared_.left288=0;shared_.left278=0;shared_.right290=640;shared_.right280=640;
    shared_.bottom294=448;shared_.savedDelta298=0;shared_.field420=0;
    shared_.field42C=3;shared_.field434=0;shared_.popup438=nullptr;
    base_.input268=0;shared_.camera4BC=nullptr;stage_=Stage::sharedConstructed;
    //1F9FF0 owns2 tabs,1 background,120 notches,ScreenText3340,
    //then33F0 and3450. Unknown424/428 and aperture state remain unknown.
    vtable_=0x3D0A10;
    for(std::uint32_t i=0;i<2;++i)constructQuad2DEF40(0x520+0x60*i);
    constructQuad2DEF40(0x5E0);
    for(std::uint32_t i=0;i<120;++i)constructQuad2DEF40(0x640+0x60*i);
    constructText2E02F0(costText3340_);
    constructQuad2DEF40(0x33F0);constructQuad2DEF40(0x3450);
    scene4C0_=nullptr;panelOffset4D8_=140;field4E4_=0;watto33B0_=nullptr;selected4C4_=nullptr;
    timer34B4_.fill(0);enabled34C8_={1,1};
    for(std::size_t i=0;i<6;++i){pairFirst_[i]=50;pairSecond_[i]=300;}
    stage_=Stage::constructed;pendingPC_=0x1FA2EC;d.sourceEffectsCommitted=true;
    d.sourcePC=0x1FA168;return true;
}
bool OriginalUiGaragePageOwner50KG::currentConstructor()const noexcept {
    if(!entityLive_||entityRefReleased_||entityCountReleased_||!inputsCurrent()||
       !(managerAttached1D1C40_?grant_->garageAttachedPageCurrent(*this,entity_):
            grant_->garagePageCurrent(this,entity_))||quadCount_!=129||textCount_!=2||
       !sharedText440_.value||!costText3340_.value||vtable_!=0x3D0A10)return false;
    return stage_==Stage::constructed||stage_==Stage::sharedManagerBound||stage_==Stage::sharedDatLoaded||stage_==Stage::sharedApertureLoading||stage_==Stage::sharedAperturesLoaded||
        stage_==Stage::sharedCameraConstructed||stage_==Stage::sharedCameraViewportPending||stage_==Stage::sharedCameraCallbackPending||stage_==Stage::sharedCameraObserverPending||stage_==Stage::sharedCameraPoseSet||stage_==Stage::sharedPromptsLoading||stage_==Stage::sharedPromptsLoaded||stage_==Stage::sharedTextLoading||stage_==Stage::sharedTextLoaded||stage_==Stage::sharedBackgroundLoaded||stage_==Stage::sharedRouterLoading||stage_==Stage::sharedCollectionsLoading||stage_==Stage::sharedControlsLoading||stage_==Stage::sharedGroupsLoading||
        stage_==Stage::sharedLoaderReturned||stage_==Stage::sceneConstructing||stage_==Stage::sceneConstructed||
        stage_==Stage::wattoBorrowed||stage_==Stage::costTextLoading||stage_==Stage::costTextLoaded||
        stage_==Stage::backgroundLoading||stage_==Stage::backgroundLoaded||
        stage_==Stage::presentationLoading||stage_==Stage::presentationLoaded;
}
bool OriginalUiGaragePageOwner50KG::retainSharedColdResources(std::shared_ptr<const PageData> data,
    std::shared_ptr<const OriginalUiGarageSharedPageDrawOwner50KG> draw,Diagnostic& d)noexcept {
    d={};if(!currentConstructor()||sharedMetadata_||sharedDraw_)return fail(d,Status::phase,0x1FA2EC);
    if(!data||!data->loaded()||!data->pageDat()||!data->page()||
       !sameOwner(data->archive(),widgets_->archive_)||
       data->page()->identity!=data->pageDat()->identity()||
       (draw&&(!draw->loaded()||!draw->panel()||draw->panel()->pageData()!=data.get())))
        return fail(d,Status::input,0x1D63B0);
    sharedMetadata_=std::move(data);sharedDraw_=std::move(draw);
    // Only pin genuine predecoded inputs. No constructor/load stage advances,
    // no packet material/glyph is smuggled into the canonical inline objects.
    d.sourcePC=0x1FA2EC;d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGaragePageOwner50KG::retainManagerSearch1D7B3C(
    std::shared_ptr<OriginalUiGarageManagerSearch50KG> search,Diagnostic& d)noexcept {
    d={};if(!currentConstructor()||stage_!=Stage::constructed||managerSearch_||!search||!search->current()||
       search->issuer_!=issuer_||!sameOwner(search->grant_,grant_)||!sameOwner(search->application_,authority_))
        return fail(d,Status::input,0x1D7B3C);
    //Input ownership only. No observer, source Camera34 or membership is created.
    managerSearch_=std::move(search);d.sourcePC=0x1D7B3C;return true;
}
bool OriginalUiGaragePageOwner50KG::beginSharedLoad1D63B0(Diagnostic& d)noexcept {
    d={};if(currentConstructor()&&stage_==Stage::sharedCameraViewportPending)
        return continueSharedCameraViewport114A00(d);
    if(currentConstructor()&&stage_==Stage::sharedCameraCallbackPending)
        return continueSharedCameraConstructor2361C0(d);
    if(currentConstructor()&&stage_==Stage::sharedCameraObserverPending)
        return continueSharedCameraObserver114770(d);
    if(currentConstructor()&&(stage_==Stage::sharedCameraPoseSet||stage_==Stage::sharedPromptsLoading))
        return continueSharedPrompts1D682C(d);
    if(currentConstructor()&&stage_==Stage::sharedPromptsLoaded)
        return continueSharedText1D6AEC(d);
    if(currentConstructor()&&stage_==Stage::sharedTextLoaded)
        return continueSharedBackground1D6CC4(d);
    if(currentConstructor()&&stage_==Stage::sharedBackgroundLoaded)
        return continueSharedCollections1D6DAC(d);
    if(!currentConstructor()||stage_!=Stage::constructed)
        return fail(d,Status::phase,0x1FA2EC);
    // The source a3 is this SAME Garage manager, not a detached Scene or
    // a metadata surrogate. Both source stores precede temporary DAT157320.
    shared_.manager270=grant_->manager();base_.manager130=grant_->manager();
    stage_=Stage::sharedManagerBound;pendingPC_=0x1D63E0;
    try {
        if(!sharedMetadata_||!sharedMetadata_->loaded()||!sameOwner(sharedMetadata_->archive(),widgets_->archive_))
            return fail(d,Status::input,0x1D63E0);
        //Actual loader-owned parsed DAT, not a borrowed success bit. Resolve
        //the source filename from the authenticated same-manager page index.
        assets::OriginalUiPageIndex50KG index;
        if(!assets::readOriginalUiPageIndex50KG(*sharedMetadata_->uiDat(),index,d.page)||index.pageFiles.size()!=1)
            return fail(d,Status::input,0x1D63E8);
        loaderDat_=std::make_unique<assets::OriginalUiDat50KG>(); //157320 custody.
        sourcePageFile_="UI/UIGarage/"+index.pageFiles[0];
        const auto dot=sourcePageFile_.find_last_of('.');
        if(dot==std::string::npos)return fail(d,Status::input,0x1D6454);
        const auto binary=sourcePageFile_.substr(0,dot)+".bpb";
        //This reached Garage branch falls back from genuinely absent BPB.
        //A present binary is not silently replaced with textual DAT.
        if(widgets_->archive_->find(binary).identity)return fail(d,Status::sharedLoaderPending,0x1577D0);
        if(!assets::readOriginalUiDat50KG(*widgets_->archive_,sourcePageFile_,*loaderDat_,d.page)||
           loaderDat_->identity()!=sharedMetadata_->pageDat()->identity())
            return fail(d,Status::input,0x1D648C);
        stage_=Stage::sharedDatLoaded;pendingPC_=0x1D64C4;
        const auto* managerName=grant_->borrowManagerName194();
        if(!managerName)return fail(d,Status::sharedLoaderPending,pendingPC_);
        name30_.emplace(managerName); //1D64C4 real source manager194 borrow.
        const auto* info=loaderDat_->section("PageInfo");
        if(!info)return fail(d,Status::input,0x1D64D4);
        if(!assets::originalUiInteger50KG(*info,"OpenSize",224,shared_.openSize274,d.page,0x1D64EC)||
           !assets::originalUiNumber50KG(*info,"Time",1,shared_.time4B0,d.page,0x1D6508))
            return fail(d,Status::input,d.page.sourcePC);
        std::string start="Closed";
        if(const auto* value=info->find("Start"))start=value->text;
        //1D7430: source compares the string with "Closed", then writes its
        //state and exact top284/bottom28C. Do not borrow projected aperture state.
        if(start!="Closed"){
            base_.input268=1;shared_.top284=shared_.openSize274;
            shared_.bottom28C=448-shared_.openSize274;shared_.state4B4=0;
        }else{shared_.top284=224;shared_.bottom28C=224;shared_.state4B4=2;}
        bool hasPopup=false;
        if(!assets::originalUiBoolean50KG(*info,"HasPopup",false,hasPopup,d.page,0x1D6584))
            return fail(d,Status::input,d.page.sourcePC);
        if(hasPopup)return fail(d,Status::sharedLoaderPending,0x1D6598);
        for(std::size_t ordinal=0;ordinal<2;++ordinal){
            //Each construction,publication,load and color loop finishes
            //before the next source aperture is allocated.
            apertures_[ordinal]=std::make_unique<Aperture>();
            shared_.apertures424[ordinal]=apertures_[ordinal].get();
            stage_=Stage::sharedApertureLoading;pendingPC_=0x1D66F8;
            if(!loadAperture1D66F8(ordinal,d))return false;
            auto& aperture=*apertures_[ordinal];
            for(std::size_t v=0;v<4;++v){auto& vertex=aperture.vertices00[v];
                if(vertex.aliases10!=1||!vertex.rgba1C[0]||vertex.rgba1C[1])
                    return fail(d,Status::input,0x1D6750);
                vertex.rgba1C[0]->words[3]=63; //1D6758; no second alias exists.
                vertex.rgba1C[0]->words[0]=vertex.rgba1C[0]->words[1]=vertex.rgba1C[0]->words[2]=0; //133F20 RGB only.
            }
        }
        stage_=Stage::sharedAperturesLoaded;pendingPC_=0x1D6804;
        if(std::fegetround()!=FE_TONEAREST||shared_.time4B0<=0||!std::isfinite(shared_.time4B0))
            return fail(d,Status::input,pendingPC_);
        volatile float extent=float(224-shared_.openSize274),speed=extent/shared_.time4B0;
        shared_.speed4B8=speed; //1D6808, no caller-supplied geometry/timing.
        return beginSharedCamera1D7A90(d);
    }catch(...){return fail(d,Status::allocation,pendingPC_);}

}
OriginalUiGaragePageOwner50KG::PolygonAssertion& OriginalUiGaragePageOwner50KG::polygonCount8F38()noexcept {static PolygonAssertion value;return value;}
OriginalUiGaragePageOwner50KG::PolygonAssertion& OriginalUiGaragePageOwner50KG::polygonLimit8F3C()noexcept {static PolygonAssertion value;return value;}
OriginalUiGaragePageOwner50KG::PolygonAssertion& OriginalUiGaragePageOwner50KG::polygonQuads8F44()noexcept {static PolygonAssertion value;return value;}
OriginalUiGaragePageOwner50KG::PolygonAssertion& OriginalUiGaragePageOwner50KG::polygonAliases8F64()noexcept {static PolygonAssertion value;return value;}
bool OriginalUiGaragePageOwner50KG::loadAperture1D66F8(std::size_t ordinal,Diagnostic& d)noexcept {
    if(ordinal>=2||!apertures_[ordinal]||(ordinal==0&&apertures_[1])||stage_!=Stage::sharedApertureLoading)
        return fail(d,Status::phase,0x1D66F8);
    auto& p=*apertures_[ordinal];p.type0C=1;p.count04=p.count08=4;
    const auto firstUse=[](PolygonAssertion& s)noexcept {
        if(!s.guard.load(std::memory_order_acquire)){s.word.store(1,std::memory_order_relaxed);s.guard.store(1,std::memory_order_release);}
    };
    firstUse(polygonCount8F38());firstUse(polygonLimit8F3C());firstUse(polygonQuads8F44());
    //The three source assertions pass for the hardwired1D66F0(type1,count4)
    //input. No caller-provided geometry or skipped source phase is admitted.
    try {
        p.vertices00=std::make_unique<Aperture::Vertex[]>(4); //2E2190 each.
        p.owns38=1;p.dirty3C=1;p.count34=p.count08;
        p.count48=1;p.capacity4C=1;p.count34=1;
        p.packets44=std::make_unique<Aperture::Descriptor[]>(1);
        auto* packet=static_cast<Quad::Qword*>(::operator new[](18*sizeof(Quad::Qword),std::align_val_t{64}));
        p.packets44[0].allocation00.reset(packet);
        for(std::size_t i=0;i<18;++i)std::construct_at(packet+i); //2E1FF8 zero288.
        auto& descriptor=p.packets44[0];
        //Native source-RGBA backing and live aliases are the observable
        //contract. These headers are retained state, never guest execution,
        //a DMA submission or a renderer-ready proof.
        packet[1].words={0x11000000,0x03000000,0x02000200,0x6C0E8000};
        descriptor.vif04=packet+1;
        packet[2].words={0x8004,0x30064000,0x512,0};
        descriptor.gif08=packet+2;descriptor.tag0C=packet+3;
        descriptor.sourcePrimitive=12;descriptor.sourceVertices=4;
        std::size_t cursor=4;
        for(std::size_t v=0;v<4;++v){auto& vertex=p.vertices00[v];
            firstUse(polygonAliases8F64());
            if(vertex.aliases10>=2)return fail(d,Status::input,0x2E2C68);
            const auto slot=vertex.aliases10;
            vertex.rgba1C[slot]=packet+cursor++; //All source channels start0.
            vertex.stq14[slot]=packet+cursor++;
            packet[cursor].words={0,0,0x3F800000,0};vertex.xyz24[slot]=packet+cursor++;
            ++vertex.aliases10;
        }
        packet[cursor].words={0x140000B0,0x10000000,0,0};
        packet[0].words={0x30000011,0,0,0};descriptor.dmaData4=packet+1;
        descriptor.pointersKnown=true; //descriptor+10 remains genuinely unknown.
        p.primitives30=std::make_unique<std::unique_ptr<Quad::Primitive>[]>(1);
        Quad::RootInputs inputs(issuer_,issuer_->standardMaterialDomainOwner_,issuer_->materialContextOwner_,
            issuer_->videoOwner_,widgets_->archive_,&issuer_->materialDirectory36B970_);
        if(!Quad::loadAperturePrimitive2E20F0(inputs,p.primitives30[0],d.quad)){
            pendingPC_=d.quad.sourcePC;return fail(d,Status::sharedLoaderPending,pendingPC_);
        }
        p.primitives30[0]->packet10=packet; //2E2144 after real numeric retain.
        d.sourcePC=0x2E2180;d.sourceEffectsCommitted=true;return true;
    }catch(...){return fail(d,Status::allocation,pendingPC_);}
}
bool OriginalUiGaragePageOwner50KG::beginSharedCamera1D7A90(Diagnostic& d)noexcept {
    if(stage_!=Stage::sharedAperturesLoaded||sharedCamera_||shared_.camera4BC)
        return fail(d,Status::phase,0x1D7A90);
    d.camera=issuer_->stageFrontendCameraCold(sharedCamera_);
    if(!d.camera.okay())return fail(d,Status::sharedLoaderPending,d.camera.sourcePC);
    shared_.camera4BC=sharedCamera_.get();stage_=Stage::sharedCameraConstructed; //1D7B08.
    d.camera=issuer_->stageFrontendCameraViewportPrefix1149D4(*sharedCamera_);
    if(!d.camera.okay())return fail(d,Status::sharedLoaderPending,d.camera.sourcePC);
    stage_=Stage::sharedCameraViewportPending;pendingPC_=0x114A00;
    return continueSharedCameraViewport114A00(d);
}
bool OriginalUiGaragePageOwner50KG::continueSharedCameraViewport114A00(Diagnostic& d)noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    if(!currentConstructor()||stage_!=Stage::sharedCameraViewportPending||!sharedCamera_||
       shared_.camera4BC!=sharedCamera_.get()||sharedCamera_->issuer_!=issuer_||
       !sameOwner(sharedCamera_->entities_,authority_)||!sharedCamera_->onAffinity()||
       !sharedCamera_->actualEntitiesAlive())return fail(d,Status::input,0x114A00);
    auto& camera=*sharedCamera_;
    if(!sharedCameraDisplay_){
        const auto live=issuer_->raceDisplayOwner50LA_.lock();
        if(!live||live->application_!=issuer_||!live->garageFirstViewportCurrent50KG(d.renderer))
            return fail(d,Status::sharedLoaderPending,0x114A0C);
        if(camera.phase_!=Camera::Phase::beforeViewport114A00||!camera.coldRetirementPreflight().okay())
            return fail(d,Status::phase,0x114A00);
        sharedCameraDisplay_=live;live->pin();sharedCameraDisplayPinned_=true;
        //114A10..1C read the actual initialized Display field exactly once.
        const auto count=live->sourceBufferCount0C_+std::uint32_t{1};
        camera.fields_.count210=count;camera.fields_.count20C=count;
        camera.phase_=Camera::Phase::insideViewport114A00;
    }
    auto& display=*sharedCameraDisplay_;
    if(!sharedCameraDisplayPinned_||!display.garageFirstViewportCurrent50KG(d.renderer)||
       camera.phase_!=Camera::Phase::insideViewport114A00)
        return fail(d,Status::sharedLoaderPending,0x114A38);
    //Actual2458C0 is two ordered drains. Page's source a2=0 excludes the
    //packet/scissor branch245B14..C64. Known-null source Scene0 excludes the
    //zoom328/32C stores; a prior/unknown selection is never treated as null.
    if(sharedViewportDrainPhase_==0){
        std::int32_t returned=0;
        const auto serviced=game::serviceOriginalRenderer244D60(display.dispatch_,true,
            display.renderer_,d.renderer,returned,game::OriginalCameraDrainPolicy::initializedContext);
        if(serviced.status!=game::OriginalCameraStartStatus::complete)
            return fail(d,Status::sharedLoaderPending,serviced.sourcePC);
        sharedViewportDrainPhase_=1;
    }
    if(sharedViewportDrainPhase_==1){
        if(!display.renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted)
            return fail(d,Status::sharedLoaderPending,0x245910);
        sharedViewportDrainPhase_=2;
    }
    camera.phase_=Camera::Phase::afterViewport114A00;
    stage_=Stage::sharedCameraCallbackPending;pendingPC_=0x1149E0;
    return continueSharedCameraConstructor2361C0(d);
}
bool OriginalUiGaragePageOwner50KG::continueSharedCameraConstructor2361C0(Diagnostic& d)noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    if(!currentConstructor()||stage_!=Stage::sharedCameraCallbackPending||!sharedCamera_||
       !sharedCameraDisplay_||!sharedCameraDisplayPinned_||sharedViewportDrainPhase_!=2||
       sharedCamera_->issuer_!=issuer_||!sharedCamera_->actualEntitiesAlive())
        return fail(d,Status::phase,0x1149E0);
    auto& camera=*sharedCamera_;auto& f=camera.fields_;auto& display=*sharedCameraDisplay_;
    const auto fp=[](std::uint32_t word)noexcept{return Camera::floatFromBits(word);};
    const auto words=[](const std::array<float,16>& input)noexcept {
        Camera::Words16 output{};for(std::size_t i=0;i<16;++i)output[i]=Camera::floatBits(input[i]);return output;
    };
    const auto vector=[&](const Camera::Words4& v)noexcept {
        return assets::TVCameraVector49E{fp(v[0]),fp(v[1]),fp(v[2]),fp(v[3])};
    };
    game::CameraSceneState49F scene;
    scene.look=vector(f.look190);scene.up=vector(f.up1A0);scene.fovDegrees=fp(f.fov1D0);
    scene.rollRadians=fp(f.roll1D4);scene.aspect1F8=fp(f.aspect1F8);
    scene.nearPlane=fp(f.near1FC);scene.farPlane=fp(f.far200);
    while(sharedCameraCallbackCount_<2){
        if(camera.phase_!=Camera::Phase::afterViewport114A00)
            return fail(d,Status::phase,sharedCameraCallbackCount_?0x11471C:0x1149E0);
        game::CameraViewOutput49F core;float focal=0,angle=0;
        auto r=game::rebuildOriginalSceneCore115350(scene,vector(f.position180),
            static_cast<std::int32_t>(f.viewport1E0[2]),core,focal);
        if(r.status!=game::OriginalCameraStartStatus::complete)return fail(d,Status::sharedLoaderPending,r.sourcePC);
        r=game::computeOriginalGuardAngle2371D0(focal,angle);
        if(r.status!=game::OriginalCameraStartStatus::complete)return fail(d,Status::sharedLoaderPending,r.sourcePC);
        //1149E0 and11471C each invoke the actual236DD0 callback.115350's
        //independent core is known. Bounds318..324 are not written until the
        //following236214 suffix, so dependent140/250/290/2D0/334/338 remain
        //unknown here. No observer/child/render reads them in this closed cut.
        f.viewC0=words(core.view);f.projection80=words(core.projection);
        f.combined100=words(core.combined);f.focal1D8=Camera::floatBits(focal);f.coreKnown=true;
        f.screen140Known=false;f.combinedScreen250Known=false;
        f.guardProjection290Known=false;f.guardCombined2D0Known=false;
        f.guardAngle330=Camera::floatBits(angle);++sharedCameraCallbackCount_;
    }
    if(camera.phase_==Camera::Phase::afterViewport114A00){
        if(!(camera.entity_.flags29&1u)||!display.garageFirstViewportCurrent50KG(d.renderer))
            return fail(d,Status::sharedLoaderPending,0x11473C);
        if(display.sourceWidth4_!=static_cast<std::int32_t>(f.fullWidth204)||
           display.sourceHeight8_!=static_cast<std::int32_t>(f.fullHeight208))
            return fail(d,Status::sharedLoaderPending,0x26F978); //Real resize not represented.
        display.sourceSelectedCamera0_=sharedCamera_; //26F98C/998 actual retained Camera selection.
        camera.phase_=Camera::Phase::selectedBeforeBounds236214;
    }
    if(camera.phase_!=Camera::Phase::selectedBeforeBounds236214||
       !sameOwner(display.sourceSelectedCamera0_,sharedCamera_)||!display.sourceCurrentCamera0Known_)
        return fail(d,Status::phase,0x236214);
    game::OriginalCameraViewportFields numeric;
    const auto prepared=game::prepareOriginalRaceViewport2361C0(numeric,camera.generation_,camera.key_,scene);
    if(prepared.status!=game::OriginalCameraStartStatus::complete)
        return fail(d,Status::sharedLoaderPending,prepared.sourcePC);
    //Reuse the recovered numeric suffix, but publish into this canonical owner
    //in source order. The temporary arithmetic struct is not a Camera or lease.
    f.depth310=Camera::floatBits(numeric.depth310);f.depth314=Camera::floatBits(numeric.depth314);
    f.left318=Camera::floatBits(numeric.left318);f.right31C=Camera::floatBits(numeric.right31C);
    f.top320=Camera::floatBits(numeric.top320);f.bottom324=Camera::floatBits(numeric.bottom324);
    f.aspect1F8=Camera::floatBits(scene.aspect1F8);f.matrix140=words(numeric.screen140);f.screen140Known=true;
    f.combinedScreen250=words(numeric.combinedScreen250);f.combinedScreen250Known=true;
    f.guardCombined2D0=words(numeric.guardCombined2D0);f.guardCombined2D0Known=true;
    f.guardX334=Camera::floatBits(numeric.guardX334);f.guardY338=Camera::floatBits(numeric.guardY338);
    //290 remains unknown from the early callbacks. Do not replace it with80;
    //the later authored114F90/236DD0 operation must produce it for real.
    camera.phase_=Camera::Phase::viewportInitialized2361C0;
    stage_=Stage::sharedCameraObserverPending;pendingPC_=0x114770;
    return continueSharedCameraObserver114770(d);
}
bool OriginalUiGaragePageOwner50KG::continueSharedCameraObserver114770(Diagnostic& d)noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    if(!currentConstructor()||stage_!=Stage::sharedCameraObserverPending||!managerSearch_||
       !sharedCamera_||sharedCamera_->phase_!=Camera::Phase::viewportInitialized2361C0||
       !sharedCameraDisplay_||!sameOwner(sharedCameraDisplay_->sourceSelectedCamera0_,sharedCamera_)||
       !sharedCameraDisplay_->sourceCurrentCamera0Known_||!loaderDat_)
        return fail(d,Status::sharedLoaderPending,0x1D7B3C);
    auto& camera=*sharedCamera_;auto& f=camera.fields_;
    OriginalUiGarageManagerSearch50KG::Diagnostic search;
    //Nonnull34 may be a retained allocation prefix, not completed membership.
    if(!managerSearch_->registerCamera114770(sharedCamera_,search))
        return fail(d,Status::sharedLoaderPending,search.sourcePC);
    //The source setter114F90 does NOT rebuild236DD0. Parse into its real
    //temporary input vectors, then retain only the source pose/counter writes.
    const auto* fields=loaderDat_->section("PageInfo");if(!fields)return fail(d,Status::input,0x1D7B74);
    std::array<float,3> position{},look{},up{};float fov=0,roll=0;
    if(!assets::originalUiTriple50KG(*fields,"CameraPosition1",{{0,0,0}},position,d.page,0x1D7B74)||
       !assets::originalUiTriple50KG(*fields,"CameraLook1",{{0,-1,0}},look,d.page,0x1D7BB0)||
       !assets::originalUiTriple50KG(*fields,"CameraUp1",{{0,0,1}},up,d.page,0x1D7BE8)||
       !assets::originalUiNumber50KG(*fields,"CameraFOV1",35,fov,d.page,0x1D7C10)||
       !assets::originalUiNumber50KG(*fields,"CameraRoll1",0,roll,d.page,0x1D7C34))
        return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
    game::CameraSceneState49F prior;
    prior.fovDegrees=Camera::floatFromBits(f.fov1D0);prior.aspect1F8=Camera::floatFromBits(f.aspect1F8);
    prior.nearPlane=Camera::floatFromBits(f.near1FC);prior.farPlane=Camera::floatFromBits(f.far200);prior.rollRadians=roll;
    game::TVCameraUpdateOutput49F input;input.mode=0;input.position={position[0],position[1],position[2],1};
    input.look={look[0],look[1],look[2],1};input.up={up[0],up[1],up[2],1};input.fov=fov;
    game::CameraViewOutput49F normalized;
    if(!game::composeCameraView49F(prior,input,normalized))return fail(d,Status::sharedLoaderPending,0x114F90);
    const auto words=[](const assets::TVCameraVector49E& v)noexcept {
        return Camera::Words4{{Camera::floatBits(v.x),Camera::floatBits(v.y),Camera::floatBits(v.z),Camera::floatBits(v.w)}};
    };
    f.position180=words(input.position);f.look190=words(normalized.sceneAfter.look);f.up1A0=words(normalized.sceneAfter.up);
    f.roll1D4=Camera::floatBits(roll);if(fov!=0)f.fov1D0=Camera::floatBits(fov);
    f.count210=sharedCameraDisplay_->sourceBufferCount0C_+std::uint32_t{1}; //115050, NOT20C.
    camera.phase_=Camera::Phase::poseSet114F90;
    //Core matrices retain the previous constructor pose;290 is still unknown.
    //Only a later real114AE0/236DD0 update may publish the authored matrices.
    stage_=Stage::sharedCameraPoseSet;pendingPC_=0x1D682C;
    return fail(d,Status::sharedLoaderPending,pendingPC_);
}
bool OriginalUiGaragePageOwner50KG::continueSharedPrompts1D682C(Diagnostic& d)noexcept {
    d={};if(!currentConstructor()||(stage_!=Stage::sharedCameraPoseSet&&stage_!=Stage::sharedPromptsLoading)||
       !sharedCamera_||!managerSearch_||!managerSearch_->cameraRegistrationCurrent())return fail(d,Status::phase,0x1D682C);
    stage_=Stage::sharedPromptsLoading;
    constexpr std::array<std::uint32_t,4> loads{{0x1D682C,0x1D68DC,0x1D6994,0x1D6A4C}};
    constexpr std::array<std::uint32_t,4> x{{0x43F20000,0x43C68000,0x438E8000,0x43200000}};
    Quad::RootInputs inputs(issuer_,issuer_->standardMaterialDomainOwner_,issuer_->materialContextOwner_,issuer_->videoOwner_,widgets_->archive_,&issuer_->materialDirectory36B970_);
    while(sharedPromptCount_<4){
        auto& q=quads_[sharedPromptCount_];pendingPC_=loads[sharedPromptCount_];
        if(!q.load2DF0B0(inputs,d.quad)){
            // Receipt of the reached call, not an inference from failed_. The
            // exact native partial Primitive is retained, without a packet.
            if(!sharedPromptCount_&&d.quad.status==Quad::Status::missingPsmFallbackPending&&
               d.quad.sourcePC==0x29B638){
                failedFirstPrompt29B638_=true;
                failedPromptGeneration_=grant_->generation_;failedPromptEpoch_=grant_->epoch_;
            }
            return fail(d,Status::sharedLoaderPending,d.quad.sourcePC);
        }
        const auto px=std::bit_cast<float>(x[sharedPromptCount_]);const auto py=std::bit_cast<float>(0x43BD0000u);
        if(q.position10[0]!=px||q.position10[1]!=py){
            q.position10[0]=px;q.position10[1]=py;q.flags5CValue|=1;q.flags5CKnownMask|=1;
            //Source position setter only marks transformed geometry dirty.
            //It does not rewrite the packet until the actual later Quad update.
            if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}
        }
        ++sharedPromptCount_;
    }
    stage_=Stage::sharedPromptsLoaded;pendingPC_=0x1D6AEC;
    return continueSharedText1D6AEC(d);
}
bool OriginalUiGaragePageOwner50KG::continueSharedText1D6AEC(Diagnostic& d)noexcept {
    d={};if(!currentConstructor()||stage_!=Stage::sharedPromptsLoaded||sharedPromptCount_!=4||!sharedText440_.value)
        return fail(d,Status::phase,0x1D6AEC);
    const auto fonts=issuer_->fonts328Owner_;if(!fonts)return fail(d,Status::sharedLoaderPending,0x1D6AFC);
    const auto font=fonts->find282210(assets::OriginalApplicationFonts50LA::Name::frontendDenmrk16);
    const auto localization=issuer_->sourceLocalization32COwner_;
    if(!localization)return fail(d,Status::sharedLoaderPending,0x1D6B1C);
    assets::OriginalApplicationLocalizationDiagnostic50KG ld;
    if(!sharedText32C_&&!issuer_->borrowLocalizationText18AC60(*localization,89,sharedText32C_,ld))
        return fail(d,Status::sharedLoaderPending,ld.sourcePC);
    std::string_view text;
    if(!assets::OriginalApplicationLocalization50KG::SelectedText::BorrowReader::readCurrent(sharedText32C_,*issuer_,*localization,89,text))
        return fail(d,Status::sharedLoaderPending,0x1D6BA8);
    stage_=Stage::sharedTextLoading;pendingPC_=0x1D6BB0;
    auto& t=*sharedText440_.value;
    if(!t.loadColdSource2DFD90(*issuer_,issuer_->standardMaterialDomainOwner_,issuer_->materialContextOwner_,fonts,font,widgets_->archive_,
        issuer_->materialDirectory36B970_,issuer_->fontDirectory36B420_,
        assets::OriginalApplicationFonts50LA::Name::frontendDenmrk16,"denmrk16Top",10,false,d.text,text))
        return fail(d,Status::sharedLoaderPending,d.text.sourcePC);
    sharedText32C_.reset(); //The actual initial text was copied, no surviving source32C pointer.
    if(t.color54_!=0x00CCCCCC){
        for(std::size_t g=0;g<t.capacity5B_;++g)for(auto& vertex:t.glyphsC_[g].vertices)
            for(std::size_t a=0;a<vertex.aliases10;++a){
                auto& rgba=vertex.rgba1C[a]->words;rgba[0]=rgba[1]=rgba[2]=102;
            }
        t.color54_=0x00CCCCCC;
    }
    constexpr float px=481.0f,py=384.0f;
    if(t.vector10_[0]!=px||t.vector10_[1]!=py){t.vector10_[0]=px;t.vector10_[1]=py;t.flags5A_|=3;}
    stage_=Stage::sharedTextLoaded;pendingPC_=0x1D6CC4;
    return continueSharedBackground1D6CC4(d);
}
bool OriginalUiGaragePageOwner50KG::continueSharedBackground1D6CC4(Diagnostic& d)noexcept {
    d={};if(!currentConstructor()||stage_!=Stage::sharedTextLoaded||!loaderDat_||!sharedCameraDisplay_||
       !sharedCameraDisplay_->frontendOwnerCurrent50KG()||!sharedCameraDisplay_->sourceCurrentCamera0Known_||
       !sameOwner(sharedCameraDisplay_->sourceSelectedCamera0_,sharedCamera_))return fail(d,Status::phase,0x1D6CC4);
    sharedCameraDisplay_->sourceClearMode2C_=0; //1D6CEC, BEFORE158AD0's Background read.
    const auto* fields=loaderDat_->section("PageInfo");if(!fields)return fail(d,Status::input,0x1D6CE8);
    const auto* value=fields->find("Background");const std::string_view background=value?std::string_view(value->text):std::string_view("*INVALID*");
    std::array<float,3> rgb{};std::int32_t useVideo=0;
    if(!assets::originalUiTriple50KG(*fields,"BackgroundColor",{{0,255,0}},rgb,d.page,0x1D6D1C))
        return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
    //The authenticated Garage authored value is exactly0,0,0. No host
    //float-to-integer rule is substituted for other source CVT.W.S inputs.
    if(rgb[0]!=0||rgb[1]!=0||rgb[2]!=0)return fail(d,Status::sharedLoaderPending,0x1D6D38);
    shared_.backgroundColor430=0; //1D6D70 after the actual three known conversions.
    if(!assets::originalUiInteger50KG(*fields,"UseVideoAsTexture",0,useVideo,d.page,0x1D6D74))
        return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
    if(background!="*INVALID*"||useVideo!=0||base_.backgroundTexture154||base_.moviePlayer158)
        return fail(d,Status::sharedLoaderPending,0x1D6D8C);
    //1D6080 selects the non-.pss call.2A6490's '*' branch neither creates
    //Texture154 nor Movie158. The preceding scratch sprintf has no surviving
    //consumer on this branch, so no invented guest directory is required.
    base_.backgroundTexture154=nullptr;base_.moviePlayer158=nullptr; //2A6634/38.
    if(!assets::originalUiInteger50KG(*fields,"PageNo",0,base_.pageNumber138,d.page,0x1D6DA4))
        return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
    stage_=Stage::sharedBackgroundLoaded;pendingPC_=0x1D6DAC;
    return continueSharedCollections1D6DAC(d);
}
bool OriginalUiGaragePageOwner50KG::appendRouterWidget2A71A0(
    C& widget,const std::array<std::int32_t,2>& wants,Diagnostic& d) {
    if(!router_||!router_->attached)return fail(d,Status::phase,0x2A71A0);
    for(std::size_t port=0;port<2;++port){
        if(!router_->headKnown[port]||!router_->membershipKnown[port]||!router_->heads[port])
            return fail(d,Status::phase,0x2A71D4);
        if(!wants[port])continue;
        auto& head=*router_->heads[port];pendingPC_=head.first0?0x2A7220:0x2A71F8;
        auto node=std::make_unique<RouterNode>();node->widget0=&widget;
        //Only raw borrows:2A71A0 never calls AddRef or changes a Widget/Sound.
        auto* raw=node.get();
        if(head.first0){if(!head.last4)return fail(d,Status::phase,0x2A7228);head.last4->next8=std::move(node);}
        else head.first0=std::move(node);
        head.last4=raw;
    }
    //The original additionally reads Widget58/5C/60/64/68/6C. Some of these
    //are unwritten or non-input aliases. Those source-only memberships are
    //NOT six known-empty lists. No native input outside ports0/1 is admitted.
    //Nodes hold no retained refs; each distinct head is independent and their
    //destructor2A7010 merely frees nodes. Omission cannot affect observed ports.
    for(std::size_t port=2;port<8;++port)router_->membershipKnown[port]=false;
    ++router_->visitedWidgets;return true;
}
bool OriginalUiGaragePageOwner50KG::loadControl1D7180(std::size_t ordinal,Diagnostic& d) {
    if(ordinal>=controls_.size()||ordinal!=controlConstructionCount_||controls_[ordinal]||
       !loaderDat_||!router_||!router_->attached)return fail(d,Status::phase,0x1D7180);
    pendingPC_=0x1D71A8;controls_[ordinal]=std::make_unique<PageControl>();auto& c=*controls_[ordinal];
    d.scope=grant_->constructGaragePageControl1D5D10(*this,ordinal,c.entity);
    if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    c.constructed=true;++controlConstructionCount_;c.sourceVtable=0x3D0060;
    //2A5850 initializes30/34/38/3C before the concrete vtable store.
    pendingPC_=0x1D71D8;const auto* fields=loaderDat_->section("Control "+std::to_string(ordinal));
    c.sectionPresent30=fields?1:0; //Actual157D00 boolean return, NOT a DAT pointer.
    if(fields){
        pendingPC_=0x1D720C;
        if(!assets::originalUiInteger50KG(*fields,"NumberOfWidgets",0,c.count34,d.page,pendingPC_))
            return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
        if(c.count34<0||c.count34>22)return fail(d,Status::sharedLoaderPending,0x1D7218);
        if(c.count34){
            pendingPC_=0x1D7228;c.widgets38.reset(new C*[static_cast<std::size_t>(c.count34)]);
            for(std::int32_t i=0;i<c.count34;++i){
                pendingPC_=0x1D7260;std::int32_t id=0;
                if(!assets::originalUiInteger50KG(*fields,"WidgetID"+std::to_string(i+1),0,id,d.page,pendingPC_))
                    return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
                C* widget=nullptr;std::array<std::int32_t,2> wants{};
                if(id<1||!widgets_->borrowPageWidget1D7284(static_cast<std::size_t>(id),widget,wants))
                    return fail(d,Status::sharedLoaderPending,0x1D7284);
                c.widgets38[static_cast<std::size_t>(i)]=widget;++c.widgetsKnown; //1D7288 before router.
                if(widget&&!appendRouterWidget2A71A0(*widget,wants,d))return false;
            }
        }
    }
    c.loaded=true;controlTable_[ordinal]=&c;controlSlotsKnown_[ordinal]=true; //1D6EEC.
    return true;
}
bool OriginalUiGaragePageOwner50KG::loadGroup1D6F08(std::size_t ordinal,Diagnostic& d) {
    if(ordinal>=groups_.size()||ordinal!=groupConstructionCount_||groups_[ordinal]||
       controlConstructionCount_!=controls_.size()||!loaderDat_)return fail(d,Status::phase,0x1D6F08);
    pendingPC_=0x1D6F08;groups_[ordinal]=std::make_unique<PageGroup>();auto& g=*groups_[ordinal];
    d.scope=grant_->constructGaragePageGroup2A4B70(*this,ordinal,g.entity);
    if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    g.constructed=true;++groupConstructionCount_;g.sourceVtable=0x3D3BE0;
    groupTable_[ordinal]=&g;groupSlotsKnown_[ordinal]=true; //1D6F40 before group reads.
    g.enabled30=1; //1D6F5C;2A4B70 itself leaves30 and3C unwritten.
    pendingPC_=0x1D6F64;const auto* fields=loaderDat_->section("Group "+std::to_string(ordinal));
    const assets::OriginalUiDatSection50KG missing{};const auto& row=fields?*fields:missing;
    if(!assets::originalUiInteger50KG(row,"NumberOfControls",0,g.count34,d.page,0x1D6F7C))
        return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
    if(g.count34<0||g.count34>8)return fail(d,Status::sharedLoaderPending,0x1D6F88);
    if(g.count34){
        pendingPC_=0x1D6F98;g.controls38.reset(new PageControl*[static_cast<std::size_t>(g.count34)]);
        for(std::int32_t i=0;i<g.count34;++i){
            pendingPC_=0x1D6FCC;std::int32_t id=-1;
            if(!assets::originalUiInteger50KG(row,"ControlID"+std::to_string(i+1),-1,id,d.page,pendingPC_))
                return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
            if(id<-1||id>=8||(id>=0&&!controlSlotsKnown_[static_cast<std::size_t>(id)]))
                return fail(d,Status::sharedLoaderPending,0x1D6FFC);
            g.controls38[static_cast<std::size_t>(i)]=id<0?nullptr:controlTable_[static_cast<std::size_t>(id)];
            ++g.controlsKnown;
        }
        g.selection3C=1;g.selected40=g.controls38[0]; //1D7034/50.
    }else{g.controls38.reset();g.selection3C=0;g.selected40=nullptr;}
    pendingPC_=0x1D7070;
    if(!assets::originalUiInteger50KG(row,"NumberOfAssociations",0,g.associatedCount44,d.page,pendingPC_))
        return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
    if(g.associatedCount44<0||g.associatedCount44>8)return fail(d,Status::sharedLoaderPending,0x1D707C);
    if(g.associatedCount44){
        pendingPC_=0x1D708C;g.associated48.reset(new PageControl*[static_cast<std::size_t>(g.associatedCount44)]);
        for(std::int32_t i=0;i<g.associatedCount44;++i){
            pendingPC_=0x1D70C0;std::int32_t id=0;
            if(!assets::originalUiInteger50KG(row,"AssociationID"+std::to_string(i+1),0,id,d.page,pendingPC_))
                return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
            if(id<0||id>=8||!controlSlotsKnown_[static_cast<std::size_t>(id)])
                return fail(d,Status::sharedLoaderPending,0x1D70E8);
            g.associated48[static_cast<std::size_t>(i)]=controlTable_[static_cast<std::size_t>(id)];
            ++g.associatedKnown;
        }
    }else g.associated48.reset();
    bool keep=true;pendingPC_=0x1D7120;
    if(!assets::originalUiBoolean50KG(row,"KeepEnabled",true,keep,d.page,pendingPC_))
        return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
    g.keepEnabled4C=keep?1:0;g.loaded=true;return true;
}
bool OriginalUiGaragePageOwner50KG::continueSharedCollections1D6DAC(Diagnostic& d)noexcept {
    d={};if(!currentConstructor()||stage_!=Stage::sharedBackgroundLoaded||!loaderDat_||router_||
       !managerSearch_||!managerSearch_->cameraRegistrationCurrent())return fail(d,Status::phase,0x1D6DAC);
    stage_=Stage::sharedRouterLoading;pendingPC_=0x1D6DAC;
    try{
        router_=std::make_unique<PageRouter>();
        d.scope=grant_->constructGaragePageRouter2A6FD0(*this,router_->entity);
        if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
        router_->constructed=true;router_->sourceVtable=0x3D3D00;
        router_->page34=&entity_;router_->manager30=*shared_.manager270; //2A7114/18.
        pendingPC_=0x2A7120;router_->table38.reset(new RouterHead*[8]);
        for(std::size_t i=0;i<8;++i){
            pendingPC_=0x2A7134;router_->heads[i]=std::make_unique<RouterHead>();
            router_->table38[i]=router_->heads[i].get();router_->headKnown[i]=true;
            router_->membershipKnown[i]=true; //Genuinely empty immediately after2A7100.
        }
        base_.router134=router_.get(); //1D6DD8.
        d.scope=grant_->attachGaragePageRouter1D6DE4(*this,router_->entity);
        if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
        router_->attached=true;stage_=Stage::sharedCollectionsLoading;
        const auto* fields=loaderDat_->section("PageInfo");if(!fields)return fail(d,Status::input,0x1D6DFC);
        std::int32_t count=0;pendingPC_=0x1D6DFC;
        if(!assets::originalUiInteger50KG(*fields,"NumberOfStaticWidgets",0,count,d.page,pendingPC_))
            return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
        base_.staticWidgetCount13C=static_cast<std::uint32_t>(count);
        if(count!=0)return fail(d,Status::sharedLoaderPending,pendingPC_);
        pendingPC_=0x1D6E14;
        if(!assets::originalUiInteger50KG(*fields,"NumberOfControls",0,count,d.page,pendingPC_))
            return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
        base_.controlCount140=static_cast<std::uint32_t>(count);
        if(count!=7)return fail(d,Status::sharedLoaderPending,pendingPC_);
        pendingPC_=0x1D6E24;controlTable_.reset(new PageControl*[8]);base_.controls144=controlTable_.get();
        pendingPC_=0x1D6E40;
        if(!assets::originalUiInteger50KG(*fields,"NumberOfGroups",0,count,d.page,pendingPC_))
            return fail(d,Status::sharedLoaderPending,d.page.sourcePC);
        base_.groupCount148=static_cast<std::uint32_t>(count);
        if(count!=2)return fail(d,Status::sharedLoaderPending,pendingPC_);
        pendingPC_=0x1D6E50;groupTable_.reset(new PageGroup*[3]);base_.groups14C=groupTable_.get();
        pendingPC_=0x1D6E5C;extraTable_.reset(new ExtraHead*[2]);base_.extra150=extraTable_.get();
        for(std::size_t i=0;i<2;++i){pendingPC_=0x1D6E70;extraHeads_[i]=std::make_unique<ExtraHead>();extraTable_[i]=extraHeads_[i].get();}
        stage_=Stage::sharedControlsLoading;
        for(std::size_t i=0;i<8;++i)if(!loadControl1D7180(i,d))return false;
        stage_=Stage::sharedGroupsLoading;
        for(std::size_t i=0;i<3;++i)if(!loadGroup1D6F08(i,d))return false;
        if(shared_.popup438)return fail(d,Status::sharedLoaderPending,0x1D7148);
        //141960(NULL,-1) is the reached, genuine no-op. No popup was created.
        if(!grant_->garagePageCollectionsCurrent(*this))return fail(d,Status::scope,0x1D7154);
        //Close the actual native parsed-DAT lifetime after its final consumer.
        //Archive/provider owners remain shared; no unrelated archive is closed.
        pendingPC_=0x1D7154;loaderDat_.reset();loaderDatClosed1573B0_=true;
        collectionsLoaded_=true;stage_=Stage::sharedLoaderReturned;pendingPC_=0x1FA2F4;
        if(!sharedCollectionsCurrent())return fail(d,Status::phase,0x1D715C);
        d.sourcePC=0x1D715C;d.sourceEffectsCommitted=true;return true;
    }catch(const std::bad_alloc&){return fail(d,Status::allocation,pendingPC_);}
    catch(...){return fail(d,Status::sharedLoaderPending,pendingPC_);}
}
bool OriginalUiGaragePageOwner50KG::sharedCollectionsCurrent()const noexcept {
    return sharedCollectionsPayloadCurrent()&&grant_->garagePageCollectionsCurrent(*this);
}
bool OriginalUiGaragePageOwner50KG::sharedCollectionsPayloadCurrent()const noexcept {
    //The exact retained payload survives the following Scene/Camera appends.
    //The construction-only Scope query additionally requires Router.next14=0;
    //keep that admission strict instead of falsely using it after1B48A0.
    if(!collectionsLoaded_||!loaderDatClosed1573B0_||loaderDat_||!currentConstructor()||
       !router_||!router_->constructed||!router_->attached||
       base_.router134!=router_.get()||!router_->page34||*router_->page34!=&entity_||
       !router_->manager30||*router_->manager30!=grant_->manager()||!router_->table38||
       base_.controls144!=controlTable_.get()||base_.groups14C!=groupTable_.get()||
       base_.extra150!=extraTable_.get()||controlConstructionCount_!=8||groupConstructionCount_!=3||
       !controlTable_||!groupTable_||!extraTable_)return false;
    for(std::size_t i=0;i<8;++i){
        if(!router_->headKnown[i]||!router_->heads[i]||router_->table38[i]!=router_->heads[i].get())return false;
        if(i<2&&!router_->membershipKnown[i])return false;
        if(!controlSlotsKnown_[i]||!controls_[i]||controlTable_[i]!=controls_[i].get()||!controls_[i]->loaded||
           controls_[i]->widgetsKnown!=static_cast<std::size_t>(controls_[i]->count34))return false;
    }
    for(std::size_t i=0;i<3;++i){
        if(!groupSlotsKnown_[i]||!groups_[i]||groupTable_[i]!=groups_[i].get()||!groups_[i]->loaded||
           groups_[i]->controlsKnown!=static_cast<std::size_t>(groups_[i]->count34)||
           groups_[i]->associatedKnown!=static_cast<std::size_t>(groups_[i]->associatedCount44))return false;
    }
    for(std::size_t i=0;i<2;++i)if(!extraHeads_[i]||extraTable_[i]!=extraHeads_[i].get())return false;
    return true;
}
bool OriginalUiGaragePageOwner50KG::constructScene1FA308(
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,
    std::shared_ptr<OriginalUiMainAudio50KG> main,OriginalUiGarageDroidAudio50GN* droids,
    std::shared_ptr<OriginalAudioPendingWaves50KG> pending,Diagnostic& d)noexcept {
    d={};if(!currentConstructor())return fail(d,Status::input,0x1FA308);
    if(stage_!=Stage::sharedLoaderReturned)
        return fail(d,Status::sharedLoaderPending,pendingPC_?pendingPC_:0x1FA2EC);
    if(!sharedCollectionsCurrent())return fail(d,Status::sharedLoaderPending,0x1D715C);
    if(sceneBacking_||retiredSceneBacking_||scene4C0_||!droids)return fail(d,Status::phase,0x1FA308);
    try{sceneBacking_.reset(new OriginalUiGaragePodScene50KG);}
    catch(...){return fail(d,Status::allocation,0x1FA2F4);}
    stage_=Stage::sceneConstructing;std::string error;
    if(!sceneBacking_->construct1B2870(grant_,std::move(assets),std::move(main),*droids,std::move(pending),error))
        return fail(d,Status::scenePending,0x1FA308);
    // Source1FA310 owns the returned reference. The backing never moves to
    // Session, which may only borrow this very object while Page retains it.
    scene4C0_=sceneBacking_.get();stage_=Stage::sceneConstructed;pendingPC_=0x1FA318;
    d.sourcePC=0x1FA310;d.sourceEffectsCommitted=true;return true;
}
OriginalUiGaragePodScene50KG* OriginalUiGaragePageOwner50KG::borrowScene4C0()const noexcept {
    if(!currentConstructor()||(stage_!=Stage::sceneConstructed&&stage_!=Stage::wattoBorrowed&&
       stage_!=Stage::costTextLoading&&stage_!=Stage::costTextLoaded&&
       stage_!=Stage::backgroundLoading&&stage_!=Stage::backgroundLoaded&&
       stage_!=Stage::presentationLoading&&stage_!=Stage::presentationLoaded)||!scene4C0_||
       scene4C0_!=sceneBacking_.get()||!sameOwner(scene4C0_->grant_,grant_)||
       !scene4C0_->currentPrefix())return nullptr;
    return scene4C0_;
}
bool OriginalUiGaragePageOwner50KG::continuePageSpecific1FA318(Diagnostic& d)noexcept {
    d={};if(stage_==Stage::backgroundLoaded)return continuePresentation1FA4DC(d);
    if(!borrowScene4C0()||stage_!=Stage::sceneConstructed||watto33B0_)
        return fail(d,Status::phase,0x1FA318);
    //1CDAF0(10) reads the ACTUAL retained manager154 table. No constructed
    //but unpublished Watto, clone, empty slot, or caller pointer is accepted.
    const auto& row=widgets_->rows_[9];
    if(!row.watto||!widgets_->ownsWattoRow(row.watto.get())||
       !widgets_->slotsKnown_[9]||!widgets_->managerTable154_||
       widgets_->managerTable154_[9]!=row.wattoEntity||!row.wattoEntity)
        return fail(d,Status::input,0x1FA318);
    watto33B0_=row.watto.get();stage_=Stage::wattoBorrowed;pendingPC_=0x1FA334;
    //This source call LOOKS UP denmrk16 in the existing Application328.
    //It does not construct a registry, load another font, or choose a fallback.
    const auto registry=issuer_->fonts328Owner_;
    if(!registry)return fail(d,Status::input,0x1FA334);
    const auto font=registry->find282210(assets::OriginalApplicationFonts50LA::Name::frontendDenmrk16);
    if(!font)return fail(d,Status::input,0x1FA334);
    stage_=Stage::costTextLoading;pendingPC_=0x1FA360;
    auto& text=*costText3340_.value;
    if(!text.loadCost1FA360(*issuer_,issuer_->standardMaterialDomainOwner_,issuer_->materialContextOwner_,
        registry,font,widgets_->archive_,issuer_->materialDirectory36B970_,issuer_->fontDirectory36B420_,d.text))
        return fail(d,Status::scenePending,d.text.sourcePC?d.text.sourcePC:0x1FA360);
    if(!text.coldInputsCurrent())return fail(d,Status::input,0x1FA368);
    //The freshly loaded position and justification already equal source
    //0,0 and the actual3C4738 default. The equality branches do not dirty them.
    if(text.vector10_[0]!=0||text.vector10_[1]!=0||text.justification50_!=0)
        return fail(d,Status::input,0x1FA368);
    if(text.depth48_!=12){text.depth48_=12;text.flags5A_|=2;} //1FA438..450.
    if(text.color54_!=0x00CCCCCCu){
        for(std::size_t i=0;i<text.capacity5B_;++i)for(auto& vertex:text.glyphsC_[i].vertices){
            //133F20 affects RGB only. Source1D8850 alpha remains63.
            auto* rgba=vertex.rgba1C[0];if(!rgba)std::terminate();
            rgba->words[0]=rgba->words[1]=rgba->words[2]=102;
        }
        text.color54_=0x00CCCCCCu; //1FA4C0, after every vertex pair.
    }
    stage_=Stage::costTextLoaded;pendingPC_=0x1FA4D4;
    //Actual background5E0.Load("podstat2") owns its primitive and numeric
    //canonical material retain, not the immutable panel packet projection.
    stage_=Stage::backgroundLoading;
    Quad::RootInputs quadInputs(issuer_,issuer_->standardMaterialDomainOwner_,issuer_->materialContextOwner_,
        issuer_->videoOwner_,widgets_->archive_,&issuer_->materialDirectory36B970_);
    if(!quads_[6].load2DF0B0(quadInputs,d.quad)){
        pendingPC_=d.quad.sourcePC;return fail(d,Status::scenePending,pendingPC_);
    }
    stage_=Stage::backgroundLoaded;pendingPC_=0x1FA4DC;
    return continuePresentation1FA4DC(d);
}
bool OriginalUiGaragePageOwner50KG::continuePresentation1FA4DC(Diagnostic& d)noexcept {
    d={};if(!borrowScene4C0()||stage_!=Stage::backgroundLoaded||!sharedCollectionsCurrent()||
       presentationStep_||quads_[6].stage_!=Quad::Stage::loaded||!quads_[6].resource58)
        return fail(d,Status::phase,0x1FA4DC);
    Quad::RootInputs inputs(issuer_,issuer_->standardMaterialDomainOwner_,issuer_->materialContextOwner_,
        issuer_->videoOwner_,widgets_->archive_,&issuer_->materialDirectory36B970_);
    if(!Quad::inputsCurrent(inputs))return fail(d,Status::input,0x1FA4DC);
    stage_=Stage::presentationLoading;pendingPC_=0x1FA4DC;
    //Source setters change the actual descriptor and dirty bits. They do not
    //pretend the later2DF600 geometry transform or a render submission ran.
    const auto dimension=[](Quad& q,std::size_t component,float value)noexcept {
        if(q.uv30[component]!=value){q.uv30[component]=value;q.flags5CValue|=1;q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}}
    };
    const auto packetFloat=[](Quad& q,std::size_t qword,std::size_t component,float value)noexcept {
        q.resource58[qword].words[component]=std::bit_cast<std::uint32_t>(value);
    };
    auto& background=quads_[6];background.field40=15; //1FA4F8 writes unconditionally.
    volatile float z=float(background.video_->packetZMaximum)-15.0f;
    for(std::size_t i=0;i<4;++i)packetFloat(background,5+3*i,2,z); //1FA528..540.
    dimension(background,1,640.0f);presentationStep_=1;
    auto& tab0=quads_[4];pendingPC_=0x1FA5B4;
    if(!tab0.load2DF0B0(inputs,d.quad))return fail(d,Status::scenePending,d.quad.sourcePC);
    dimension(tab0,2,18.0f);volatile float tab0T=18.0f*tab0.scale50;
    packetFloat(tab0,15,1,tab0T);packetFloat(tab0,12,1,tab0T); //1FA64C/65C before next load.
    presentationStep_=2;auto& tab1=quads_[5];pendingPC_=0x1FA658;
    if(!tab1.load2DF0B0(inputs,d.quad))return fail(d,Status::scenePending,d.quad.sourcePC);
    dimension(tab1,2,18.0f);dimension(tab1,1,234.0f);
    volatile float tab1S=20.0f*tab1.scale4C,tab1T=18.0f*tab1.scale50;
    packetFloat(tab1,12,0,tab1S);packetFloat(tab1,6,0,tab1S);
    packetFloat(tab1,15,1,tab1T);packetFloat(tab1,12,1,tab1T);presentationStep_=3;
    for(std::size_t i=0;i<120;++i){
        auto& notch=quads_[7+i];pendingPC_=0x1FA7A0;
        if(!notch.load2DF0B0(inputs,d.quad))return fail(d,Status::scenePending,d.quad.sourcePC);
        dimension(notch,1,5.0f);dimension(notch,2,10.0f);++presentationNotchesLoaded_;
    }
    presentationStep_=4;auto& back=quads_[1];
    if(back.position10[0]!=380.0f||back.position10[1]!=378.0f){
        back.position10[0]=380.0f;back.position10[1]=378.0f;back.flags5CValue|=1;back.flags5CKnownMask|=1;
        if(back.uv30[3]!=0){back.flags5CValue|=2;back.flags5CKnownMask|=2;}
    }
    for(std::size_t i=0;i<2;++i){
        auto& arrow=quads_[127+i];pendingPC_=i?0x1FA9E4:0x1FA950;
        if(!arrow.load2DF0B0(inputs,d.quad))return fail(d,Status::scenePending,d.quad.sourcePC);
        //Actual unique ATM prototype is retained by the canonical material.
        //No animation tick or flattened/constant-frame stand-in is issued.
        if(arrow.color44!=0x00ffffffu){
            for(std::size_t v=0;v<4;++v){auto& rgba=arrow.resource58[4+3*v].words;rgba[0]=rgba[1]=rgba[2]=127;}
            arrow.color44=0x00ffffffu;
        }
        ++presentationArrowsLoaded_;
    }
    presentationStep_=5;stage_=Stage::presentationLoaded;pendingPC_=0x1FB0F0;
    return preflightSceneLoad1FB0F0(d);
}
bool OriginalUiGaragePageOwner50KG::preflightSceneLoad1FB0F0(Diagnostic& d)const noexcept {
    d={};if(!borrowScene4C0()||!sharedCollectionsCurrent())return fail(d,Status::sceneOrder,0x1FB0F0);
    if(!watto33B0_)return fail(d,Status::scenePending,0x1FA318);
    if(!costText3340_.value||costText3340_.value->phase_!=Text::Phase::coldLoaded||
       !costText3340_.value->coldInputsCurrent())return fail(d,Status::scenePending,0x1FA360);
    if(stage_!=Stage::presentationLoaded||presentationStep_!=5||presentationNotchesLoaded_!=120||presentationArrowsLoaded_!=2)
        return fail(d,Status::scenePending,pendingPC_?pendingPC_:0x1FA4DC);
    Quad::RootInputs inputs(issuer_,issuer_->standardMaterialDomainOwner_,issuer_->materialContextOwner_,
        issuer_->videoOwner_,widgets_->archive_,&issuer_->materialDirectory36B970_);
    if(!Quad::inputsCurrent(inputs))return fail(d,Status::input,0x1FB0F0);
    for(const auto& q:quads_)if(q.stage_!=Quad::Stage::loaded||!q.material54||!q.resource58||
        q.material54!=q.primitiveOwner_.get()||q.resource58!=q.packetOwner_.get()||
        q.material54->packet10!=q.resource58||!q.material54->materialRetained||
        !sameOwner(q.domain_,issuer_->standardMaterialDomainOwner_)||
        !sameOwner(q.context_,issuer_->materialContextOwner_)||!sameOwner(q.video_,issuer_->videoOwner_))
            return fail(d,Status::scenePending,0x1FB0F0);
    d.sourcePC=0x1FB0F0;return true; //Only the actual next Scene-load call is admitted.
}
bool OriginalUiGaragePageOwner50KG::preflightSceneAttachment1B48A0(Diagnostic& d)const noexcept {
    d={};if(!borrowScene4C0()||!scene4C0_->sourceLoadReturned1FB59C())
        return fail(d,Status::sceneOrder,0x1FB5DC);
    if(!scenePageBorrowCaptured_){
        std::array<C*,10> actual{};assets::OriginalStandardMaterialDiagnostic50LA sd;
        if(!scene4C0_->borrowPageChildren1B48A0(*this,actual,sd))
            return fail(d,Status::scenePending,sd.sourcePC);
    }else{
        if(scenePageAttachCursor_>scenePageChildren_.size())return fail(d,Status::phase,0x1B48A0);
        for(std::size_t i=0;i<scenePageChildren_.size();++i){const auto* c=scenePageChildren_[i];
            if(!c)return fail(d,Status::phase,0x1B48A0);
            if(i<scenePageAttachCursor_){
                if(!grant_->garageScenePageChildCurrent(*this,*scene4C0_,i,*c))
                    return fail(d,Status::phase,0x1B48A0);
            }else if(c->reference00!=1||c->parent1C||c->previous10||c->next14||c->mask20)
                return fail(d,Status::phase,0x1B48A0);
        }
    }
    //Admission to this concrete append pass only. It neither attaches Scene
    //itself nor certifies Camera1FB620, later1B4CA0, input or Page readiness.
    d.sourceEffectsCommitted=scenePageAttachCursor_!=0;d.sourcePC=0x1B48A0;return true;
}
bool OriginalUiGaragePageOwner50KG::sceneAttachmentPrefixCurrent1B48A0()const noexcept {
    if(!currentConstructor()||!scene4C0_||sceneBacking_.get()!=scene4C0_||!scenePageBorrowCaptured_||
       scenePageAttachCursor_!=scenePageChildren_.size()||!grant_||!grant_->current())return false;
    for(std::size_t i=0;i<scenePageChildren_.size();++i)if(!scenePageChildren_[i]||
       !grant_->garageScenePageChildCurrent(*this,*scene4C0_,i,*scenePageChildren_[i]))return false;
    return true;
}
bool OriginalUiGaragePageOwner50KG::cameraRegistrationCurrent1FB608()const noexcept {
    return scene4C0_&&sceneBacking_.get()==scene4C0_&&scene4C0_->visibility_&&sharedCamera_&&
        scene4C0_->visibility_->pageCameraRegistrationCurrent(*this,*sharedCamera_);
}
bool OriginalUiGaragePageOwner50KG::continueSceneAttachments1FB5C0(Diagnostic& d)noexcept {
    d={};if(failedSceneAttachment1B4A38_)return fail(d,Status::scope,0x1B4A38);
    if(!currentConstructor()||!scene4C0_||sceneBacking_.get()!=scene4C0_||
       !scene4C0_->sourceLoadReturned1FB59C()||cameraHandoffPhase_||!managerSearch_||!sharedCamera_)
        return fail(d,Status::scenePending,0x1FB5C0);
    if(!scenePageBorrowCaptured_){
        assets::OriginalStandardMaterialDiagnostic50LA sd;
        if(!scene4C0_->borrowPageChildren1B48A0(*this,scenePageChildren_,sd))
            return fail(d,Status::scenePending,sd.sourcePC);
        scenePageBorrowCaptured_=true;
    }
    constexpr std::array<std::uint32_t,10> masks{{0xffffffffu,2u,0xffffffffu,0xffffffffu,
        0xffffffffu,2u,2u,0xffffffffu,0xffffffffu,0xffffffffu}};
    for(;scenePageAttachCursor_<scenePageChildren_.size();++scenePageAttachCursor_){
        const auto i=scenePageAttachCursor_;
        for(std::size_t j=0;j<i;++j)if(!grant_->garageScenePageChildCurrent(*this,*scene4C0_,j,*scenePageChildren_[j]))
            return fail(d,Status::phase,0x1B48A0);
        if(!i){assets::OriginalStandardMaterialDiagnostic50LA vd;
            if(!scene4C0_->visibility_->attachPage4BC1B48D4(*this,*scene4C0_,vd))
                return fail(d,Status::scenePending,vd.sourcePC);
        }else{
            d.scope=grant_->attachGarageScenePageChild1B48A0(*this,*scene4C0_,i,*scenePageChildren_[i],masks[i]);
            if(!d.scope.okay()){
                // Only the observed pre-attach ordinal6 rejection mints this
                // terminal native exception receipt. Earlier/later failures
                // and partially applied attaches are deliberately excluded.
                if(i==6&&d.scope.sourcePC==0x1B4A38){
                    const auto receipt=grant_->noteGaragePageAttachmentFailure1B4A38(*this,*scene4C0_,scenePageChildren_);
                    if(receipt.okay()){
                        failedSceneAttachment1B4A38_=true;
                        failedSceneAttachmentGeneration_=grant_->generation_;
                        failedSceneAttachmentEpoch_=grant_->epoch_;
                    }
                }
                return fail(d,Status::scope,d.scope.sourcePC);
            }
        }
    }
    if(!sceneAttachmentPrefixCurrent1B48A0())return fail(d,Status::phase,0x1B4C84);
    d.sourceEffectsCommitted=true;d.sourcePC=0x1B4C84;return true;
}
bool OriginalUiGaragePageOwner50KG::continueCameraHandoff1FB5E4(Diagnostic& d)noexcept {
    d={};if(!sceneAttachmentPrefixCurrent1B48A0()||!sharedCamera_||!managerSearch_||
       shared_.camera4BC!=sharedCamera_.get())return fail(d,Status::scenePending,0x1FB5E4);
    if(!cameraHandoffPhase_){
        OriginalUiGarageManagerSearch50KG::Diagnostic sd;
        if(!managerSearch_->unregisterCamera1148D0(sharedCamera_,sd))return fail(d,Status::scenePending,sd.sourcePC);
        cameraHandoffPhase_=1; //Real manager row removed,34 cleared;38 retained.
    }
    if(cameraHandoffPhase_==1){
        assets::OriginalStandardMaterialDiagnostic50LA vd;
        if(!scene4C0_->visibility_->registerPageCamera1FB608(*this,sharedCamera_,vd))
            return fail(d,Status::scenePending,vd.sourcePC);
        cameraHandoffPhase_=2;
    }
    if(cameraHandoffPhase_==2){
        d.scope=grant_->attachGaragePageCamera1FB620(*this,*sharedCamera_);
        if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
        cameraHandoffPhase_=3;
    }
    if(!grant_->garagePageCameraCurrent(*this,*sharedCamera_)||!cameraRegistrationCurrent1FB608())
        return fail(d,Status::phase,0x1FB620);
    //This is ONLY the concrete Camera handoff. Source1B86D0 and the later
    //1B4CA0 Page/Scene children still follow, so this is not full Page readiness.
    d.sourceEffectsCommitted=true;d.sourcePC=0x1FB620;return true;
}
bool OriginalUiGaragePageOwner50KG::continueLateSceneAttachments1FB648(
    OriginalUiSharedDialogs50KG& dialogs,Diagnostic& d)noexcept {
    d={};if(!sceneAttachmentPrefixCurrent1B48A0()||cameraHandoffPhase_!=3||!sharedCamera_||
       !grant_->garagePageCameraCurrent(*this,*sharedCamera_)||!cameraRegistrationCurrent1FB608())
        return fail(d,Status::scenePending,0x1FB628);
    if(!sceneLateBorrowCaptured_){
        assets::OriginalStandardMaterialDiagnostic50LA sd;
        if(!scene4C0_->borrowLatePageChildren1B4CA0(*this,sceneLateChildren_,sd)){
            // Retain the exact reached failure before shared-dialog reappend
            // or any late child attachment. This is not Page load completion.
            if(sd.status==assets::OriginalStandardMaterialStatus50LA::phase&&sd.sourcePC==0x1B4D24){
                failedLateBorrow1B4D24_=true;
                failedLateBorrowGeneration_=grant_->generation_;
                failedLateBorrowEpoch_=grant_->epoch_;
            }
            return fail(d,Status::scenePending,sd.sourcePC);
        }
        sceneLateBorrowCaptured_=true;
    }
    //Actual shared470 then47C timer/reset/detach/reappend precedes1B4CA0.
    //A receipt belongs to this exact Page/grant/root, not a caller ready bit.
    if(!dialogs.reorderCompleteFor1B872C(*this)){
        OriginalUiSharedDialogs50KG::Diagnostic shared;
        if(!dialogs.reappend1B86D0(*this,shared))return fail(d,Status::scenePending,0x1B86D0);
    }
    if(!dialogs.reorderCompleteFor1B872C(*this))return fail(d,Status::phase,0x1B872C);
    constexpr std::array<std::uint32_t,3> masks{{0x402u,0u,0u}};
    for(;sceneLateAttachCursor_<sceneLateChildren_.size();++sceneLateAttachCursor_){
        const auto i=sceneLateAttachCursor_;
        for(std::size_t j=0;j<i;++j)if(!grant_->garageLateScenePageChildCurrent(*this,*scene4C0_,j,*sceneLateChildren_[j]))
            return fail(d,Status::phase,0x1B4CA0);
        d.scope=grant_->attachGarageLateScenePageChild1B4CA0(*this,*scene4C0_,i,*sceneLateChildren_[i],masks[i]);
        if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    }
    //No renderer callbacks, successful Page admission, wet playback or fire
    //readiness follow merely from these actual source child attachments.
    d.sourceEffectsCommitted=true;d.sourcePC=0x1B4DB8;return true;
}
bool OriginalUiGaragePageOwner50KG::lateSceneAttachmentsCurrent1FB64C()const noexcept {
    if(!sceneAttachmentPrefixCurrent1B48A0()||cameraHandoffPhase_!=3||!sharedCamera_||
       !grant_->garagePageCameraCurrent(*this,*sharedCamera_)||!cameraRegistrationCurrent1FB608()||
       !sceneLateBorrowCaptured_||sceneLateAttachCursor_!=sceneLateChildren_.size()||sceneLateRetiredCount_)
        return false;
    for(std::size_t i=0;i<sceneLateChildren_.size();++i)if(!sceneLateChildren_[i]||
       !grant_->garageLateScenePageChildCurrent(*this,*scene4C0_,i,*sceneLateChildren_[i]))return false;
    return true;
}
bool OriginalUiGaragePageOwner50KG::selectedRegistrationCurrent1FB67C()const noexcept {
    if(!loadApplication344_||!scene4C0_||!scene4C0_->visibility_||!managerSearch_||!managerSearch_->current())
        return false;
    //1FAAA0's nonzero branch skips1FB660, including its Page4C4 write.
    if(*loadApplication344_!=0)return !loadSelectedOwner_&&!selected4C4_&&!loadSelectedObserver_&&!loadSelectedZone_;
    if(!loadSelectedOwner_||!sameOwner(loadSelectedOwner_,managerSearch_->selected_)||
       !loadSelectedOwner_->current()||!loadSelectedObserver_||!loadSelectedZone_||
       !scene4C0_->visibility_->current()||scene4C0_->visibility_->primaryZone4C0()!=loadSelectedZone_)
        return false;
    const auto* resource=loadSelectedOwner_->model();
    const auto* model=resource?resource->scene().model():nullptr;
    return model&&selected4C4_==managerSearch_->selectedEntity_&&
        selected4C4_==resource->scene().modelEntity8()&&model->observer19C==loadSelectedObserver_;
}
bool OriginalUiGaragePageOwner50KG::continueLoadReturn1FAA88(
    OriginalUiSharedDialogs50KG& dialogs,Diagnostic& d)noexcept {
    d={};
    if(!borrowScene4C0()||!scene4C0_->sourceLoadReturned1FB59C()||!sharedCollectionsPayloadCurrent()||
       !managerSearch_||!managerSearch_->current()||!shared_.manager270||
       *shared_.manager270!=grant_->manager()||managerAttached1D1C40_)
        return fail(d,Status::sceneOrder,0x1FAA88);
    if(loadReturnPhase_==LoadReturnPhase::returned){
        if(!sourceLoadReturned1FAAC8())return fail(d,Status::phase,0x1FAAC8);
        d.sourceEffectsCommitted=true;d.sourcePC=0x1FAAC8;return true;
    }
    if(loadReturnPhase_==LoadReturnPhase::forestPending){
        pendingPC_=0x1FAA88;
        if(!sceneAttachmentPrefixCurrent1B48A0()&&!continueSceneAttachments1FB5C0(d)){
            pendingPC_=d.sourcePC;return false;
        }
        if(cameraHandoffPhase_!=3&&!continueCameraHandoff1FB5E4(d)){
            pendingPC_=d.sourcePC;return false;
        }
        if(!lateSceneAttachmentsCurrent1FB64C()&&!continueLateSceneAttachments1FB648(dialogs,d)){
            pendingPC_=d.sourcePC;return false;
        }
        if(!lateSceneAttachmentsCurrent1FB64C()||!dialogs.reorderCompleteFor1B872C(*this))
            return fail(d,Status::phase,0x1FB64C);
        loadReturnPhase_=LoadReturnPhase::application344Pending;pendingPC_=0x1FAA98;
    }
    if(!lateSceneAttachmentsCurrent1FB64C()||!dialogs.reorderCompleteFor1B872C(*this))
        return fail(d,Status::phase,0x1FB64C);
    if(loadReturnPhase_==LoadReturnPhase::application344Pending){
        std::int32_t actual344=0;
        //The actual Application constructor/argv producer owns this branch.
        //Neither a fresh profile, host argv inference nor a caller flag does.
        if(!issuer_->observeGarageLoaderApplication344(*this,actual344))
            return fail(d,Status::loaderApplicationPending,0x1FAA98);
        loadApplication344_=actual344;
        loadReturnPhase_=actual344?LoadReturnPhase::resourcesPending:LoadReturnPhase::selectedPending;
        pendingPC_=actual344?0x1FAAC0:0x1FAAAC;
    }
    if(loadReturnPhase_==LoadReturnPhase::selectedFailed||loadReturnPhase_==LoadReturnPhase::resourcesFailed){
        d.loadResources=loadFailure_;
        return fail(d,loadReturnPhase_==LoadReturnPhase::selectedFailed?Status::loaderSelectedPending:
            Status::loaderResourcesPending,pendingPC_);
    }
    if(loadReturnPhase_==LoadReturnPhase::selectedPending){
        if(!loadApplication344_||*loadApplication344_||!managerSearch_->selected_||
           !managerSearch_->selected_->current()||selected4C4_||loadSelectedOwner_||
           !scene4C0_->visibility_||!scene4C0_->visibility_->current())
            return fail(d,Status::loaderSelectedPending,0x1FAAAC);
        const auto* resource=managerSearch_->selected_->model();
        if(!resource||resource->scene().modelEntity8()!=managerSearch_->selectedEntity_||
           !scene4C0_->visibility_->primaryZone4C0())return fail(d,Status::loaderSelectedPending,0x1FB66C);
        loadSelectedOwner_=managerSearch_->selected_;
        selected4C4_=managerSearch_->selectedEntity_; //1FB668 precedes the28E8A0 call.
        loadSelectedZone_=scene4C0_->visibility_->primaryZone4C0();
        pendingPC_=0x1FB67C;
        if(!scene4C0_->visibility_->attachSelected1FB67C(*loadSelectedOwner_,d.loadResources)){
            //28E8A0 may already have published observer19C before a bounds/
            //membership failure. Retain that exact owner; never reissue the
            //whole call or manufacture a successful observer on retry.
            loadFailure_=d.loadResources;loadReturnPhase_=LoadReturnPhase::selectedFailed;
            pendingPC_=d.loadResources.sourcePC?d.loadResources.sourcePC:0x1FB67C;
            return fail(d,Status::loaderSelectedPending,pendingPC_);
        }
        loadSelectedObserver_=resource->scene().model()->observer19C;
        if(!selectedRegistrationCurrent1FB67C()){
            loadReturnPhase_=LoadReturnPhase::selectedFailed;pendingPC_=0x1FB684;
            loadFailure_={};loadFailure_.status=assets::OriginalStandardMaterialStatus50LA::phase;
            loadFailure_.sourcePC=pendingPC_;d.loadResources=loadFailure_;
            return fail(d,Status::loaderSelectedPending,pendingPC_);
        }
        loadReturnPhase_=LoadReturnPhase::resourcesPending;pendingPC_=0x1FAAC0;
    }
    if(loadReturnPhase_==LoadReturnPhase::resourcesPending){
        if(!selectedRegistrationCurrent1FB67C())return fail(d,Status::phase,0x1FAAB4);
        const auto& domain=issuer_->standardMaterialDomainOwner_;
        const auto& context=issuer_->materialContextOwner_;
        if(!domain||!domain->prepared()||!context||!context->live()||
           context->materialApplication()!=issuer_->materialApplication||
           !context->applicationAnimationClock().sameOwner(issuer_->animationClock())||
           !domain->materials382B38()->screenApplicationMatches(*issuer_->materialApplication))
            return fail(d,Status::loaderResourcesPending,0x2A4B3C);
        //Actual UIGarage vtable3CF5A0+A4 is2A4B30, not Page1FAAF0.
        //Reuse the ONE canonical domain's14D370(materials),29E340(textures),
        //14D440(collector) sequence. No Video/text service or entry callback
        //is substituted. A failed aggregated call preserves partial effects
        //and its exact nested diagnostic, and is not silently replayed.
        pendingPC_=0x2A4B3C;
        if(!domain->populate(context,d.loadResources)){
            loadFailure_=d.loadResources;loadReturnPhase_=LoadReturnPhase::resourcesFailed;
            pendingPC_=d.loadResources.sourcePC?d.loadResources.sourcePC:
                d.loadResources.texture.sourcePC?d.loadResources.texture.sourcePC:0x2A4B3C;
            return fail(d,Status::loaderResourcesPending,pendingPC_);
        }
        loadCollector_=domain->coldCollector14D440();
        if(!domain->materials382B38()->coldCollectorCurrent(loadCollector_)){
            loadReturnPhase_=LoadReturnPhase::resourcesFailed;pendingPC_=0x2A4B54;
            loadFailure_={};loadFailure_.status=assets::OriginalStandardMaterialStatus50LA::phase;
            loadFailure_.sourcePC=pendingPC_;d.loadResources=loadFailure_;
            return fail(d,Status::loaderResourcesPending,pendingPC_);
        }
        loadReturnPhase_=LoadReturnPhase::returned;pendingPC_=0;
    }
    if(!sourceLoadReturned1FAAC8())return fail(d,Status::phase,0x1FAAC8);
    d.sourceEffectsCommitted=true;d.sourcePC=0x1FAAC8;return true;
}
bool OriginalUiGaragePageOwner50KG::sourceLoadReturned1FAAC8()const noexcept {
    return loadReturnPhase_==LoadReturnPhase::returned&&currentConstructor()&&
        sharedCollectionsPayloadCurrent()&&scene4C0_&&scene4C0_->sourceLoadReturned1FB59C()&&
        lateSceneAttachmentsCurrent1FB64C()&&
        loadApplication344_.has_value()&&selectedRegistrationCurrent1FB67C()&&
        issuer_->standardMaterialDomainOwner_;
    //The returned phase records the completed first2A4B30. A later genuine
    //manager1CCE74→2A4B30 rebuild must not erase this historical return.
    //Only continueLoadReturn authenticates that first current collector.
}
bool OriginalUiGaragePageOwner50KG::attachToManager1D1C40(Diagnostic& d)noexcept {
    d={};if(managerAttached1D1C40_||!sourceLoadReturned1FAAC8())return fail(d,Status::phase,0x1D1C40);
    //The loader returns the Page at1FAAC8. Only the enclosing manager's
    //following141960 call publishes it in the manager Entity forest.
    d.scope=grant_->attachCompletedGaragePage1D1C40(*this,entity_);
    if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    managerAttached1D1C40_=true;d.sourceEffectsCommitted=true;d.sourcePC=0x1D1C40;return true;
}
bool OriginalUiGaragePageOwner50KG::activationWidgetCallCurrent(std::size_t row)const noexcept {
    //Independent of Page/Widget current predicates: a partial Watto90 call
    //asks this while the actual source-ordered placement is in progress.
    if(activationPhase_!=ActivationPhase::widgetsPending||entryPhase_!=EntryPhase::notEntered||
       !managerAttached1D1C40_||ownerThread_!=std::this_thread::get_id()||!grant_||!grant_->current()||
       grant_->issuer()!=issuer_||!sameOwner(authority_,grant_->application())||
       !grant_->garageAttachedPageCurrent(*this,entity_)||!widgets_||row>=widgets_->rows_.size()||
       !widgets_->slotsKnown_[row]||!widgets_->managerTable154_||activationGroup_>=groups_.size()||
       activationPass_>1||!groupTable_||!groupSlotsKnown_[activationGroup_]||
       !groups_[activationGroup_]||groupTable_[activationGroup_]!=groups_[activationGroup_].get())return false;
    const auto& group=*groups_[activationGroup_];
    if(!group.loaded||group.retirementStarted||group.payloadRetired)return false;
    const auto count=activationPass_?group.associatedKnown:group.controlsKnown;
    const auto* table=activationPass_?group.associated48.get():group.controls38.get();
    if(!table||activationControl_>=count)return false;
    const auto* control=table[activationControl_];
    if(!control||!control->loaded||control->retirementStarted||control->payloadRetired||
       !control->widgets38||activationWidget_>=control->widgetsKnown)return false;
    bool retained=false;for(std::size_t i=0;i<controls_.size();++i)
        if(controlSlotsKnown_[i]&&controls_[i].get()==control&&controlTable_&&controlTable_[i]==control)retained=true;
    return retained&&control->widgets38[activationWidget_]==widgets_->managerTable154_[row];
}
bool OriginalUiGaragePageOwner50KG::continueActivation1D7390(Diagnostic& d)noexcept {
    d={};
    if(activationFailed_){d.widget=activationFailure_;return fail(d,Status::activationWidgetsPending,pendingPC_);}
    if(!managerAttached1D1C40_||!sourceLoadReturned1FAAC8()||!sharedCollectionsPayloadCurrent()||
       entryPhase_!=EntryPhase::notEntered)return fail(d,Status::phase,0x2A3C44);
    if(activationPhase_==ActivationPhase::notActivated){
        activationPhase_=ActivationPhase::showPending;pendingPC_=0x2A6680;
    }
    if(activationPhase_==ActivationPhase::showPending){
        //Page slot24 is141630, but its actual children may override24. Scope
        //must dispatch the recovered concrete forest, never just set all bits.
        d.scope=grant_->activateGaragePageForest141630(*this,entity_);
        if(!d.scope.okay())return fail(d,Status::activationShowPending,d.scope.sourcePC);
        activationPhase_=ActivationPhase::resetPending;pendingPC_=0x2A668C;
    }
    if(activationPhase_==ActivationPhase::resetPending){
        base_.field264=1; //2A668C after the actual recursive show returns.
        for(std::size_t i=0;i<extraHeads_.size();++i){
            if(!extraTable_||!extraHeads_[i]||extraTable_[i]!=extraHeads_[i].get())
                return fail(d,Status::phase,0x2A66AC);
            extraHeads_[i]->first0=-1; //2A66B0.
            extraHeads_[i]->last4=nullptr; //2A66C8, including the final delay slot.
        }
        activationPhase_=ActivationPhase::widgetsPending;pendingPC_=0x2A66EC;
    }
    if(activationPhase_==ActivationPhase::widgetsPending){
        if(base_.groupCount148+std::uint32_t{1}!=groups_.size())return fail(d,Status::phase,0x2A6700);
        while(activationGroup_<groups_.size()){
            if(!groupTable_||!groupSlotsKnown_[activationGroup_]||
               groupTable_[activationGroup_]!=groups_[activationGroup_].get())return fail(d,Status::phase,0x2A66E0);
            auto* group=groupTable_[activationGroup_];
            if(!group){++activationGroup_;activationPass_=0;activationControl_=activationWidget_=0;continue;}
            if(!group->loaded||group->retirementStarted||group->payloadRetired)return fail(d,Status::phase,0x2A66EC);
            while(activationPass_<2){
                const auto count=activationPass_?group->associatedKnown:group->controlsKnown;
                auto* const table=activationPass_?group->associated48.get():group->controls38.get();
                if(count&&!table)return fail(d,Status::phase,activationPass_?0x2A5808:0x2A57C8);
                while(activationControl_<count){
                    auto* const control=table[activationControl_];
                    if(!control){++activationControl_;activationWidget_=0;continue;}
                    bool retained=false;for(std::size_t i=0;i<controls_.size();++i)
                        if(controlSlotsKnown_[i]&&controls_[i].get()==control&&controlTable_&&controlTable_[i]==control)retained=true;
                    if(!retained||!control->loaded||control->retirementStarted||control->payloadRetired||
                       control->count34<0||control->widgetsKnown!=static_cast<std::size_t>(control->count34)||
                       (control->widgetsKnown&&!control->widgets38))return fail(d,Status::phase,0x2A5BF0);
                    while(activationWidget_<control->widgetsKnown){
                        const auto* target=control->widgets38[activationWidget_];
                        if(!target){++activationWidget_;continue;}
                        std::size_t row=0;for(;row<widgets_->rows_.size();++row)
                            if(widgets_->slotsKnown_[row]&&widgets_->managerTable154_[row]==target)break;
                        pendingPC_=0x2A5C30;
                        if(row==widgets_->rows_.size()||!activationWidgetCallCurrent(row))
                            return fail(d,Status::activationWidgetsPending,pendingPC_);
                        if(!widgets_->activatePageWidget90(*this,row,d.widget)){
                            if(d.widget.sourcePC)pendingPC_=d.widget.sourcePC;
                            if(row==9&&watto33B0_){OriginalUiGarageWattoOwner50KG::Observation after;
                                if(watto33B0_->inspect(after)&&after.failed){activationFailed_=true;activationFailure_=d.widget;}}
                            return fail(d,Status::activationWidgetsPending,pendingPC_);
                        }
                        ++activationWidget_;
                    }
                    ++activationControl_;activationWidget_=0;
                }
                ++activationPass_;activationControl_=activationWidget_=0;
            }
            ++activationGroup_;activationPass_=0;
        }
        activationPhase_=ActivationPhase::moviePending;pendingPC_=0x2A6710;
    }
    if(activationPhase_==ActivationPhase::moviePending){
        //The real shared loader proved '*INVALID*', no Movie158 and no path.
        //Do not claim the positive movie constructor/start callbacks returned.
        if(base_.moviePathFirstByte15C||base_.moviePlayer158)
            return fail(d,Status::activationMoviePending,base_.moviePlayer158?0x2A6770:0x2A6728);
        activationPhase_=ActivationPhase::displayPending;pendingPC_=0x2A67C0;
    }
    if(activationPhase_==ActivationPhase::displayPending){
        if(base_.backgroundTexture154||base_.moviePlayer158||!sharedCameraDisplay_||
           !sameOwner(sharedCameraDisplay_,issuer_->raceDisplayOwner50LA_.lock())||
           !sharedCameraDisplay_->sourceFogFieldLifetimeCurrent())
            return fail(d,Status::activationDisplayPending,0x2A67E8);
        sharedCameraDisplay_->sourceClearMode2C_=1; //Actual no texture/movie branch.
        activationPhase_=ActivationPhase::returned;pendingPC_=0;
    }
    if(!sourceActivationReturned2A680C())return fail(d,Status::phase,0x2A680C);
    d.sourceEffectsCommitted=true;d.sourcePC=0x2A680C;return true;
}
bool OriginalUiGaragePageOwner50KG::sourceActivationReturned2A680C()const noexcept {
    return activationPhase_==ActivationPhase::returned&&!activationFailed_&&managerAttached1D1C40_&&
        base_.field264==1&&activationGroup_==groups_.size()&&sourceLoadReturned1FAAC8();
}
bool OriginalUiGaragePageOwner50KG::entryProfileCurrent()const noexcept {
    //Independent of Page/Widget live predicates: those predicates call this
    //observation while authenticating a real partially applied appearance.
    if(!entryProfile_||ownerThread_!=std::this_thread::get_id()||!issuer_||!issuer_->profile||
       !sameOwner(entryProfile_->owner,issuer_->riderProfile)||
       entryProfile_->owner->commonProfile()!=issuer_->profile.get())return false;
    OriginalUiRiderProfileOwner50KG::GarageProfileSnapshot actual;
    if(!entryProfile_->owner->selectedGarageProfileSnapshot(actual))return false;
    const auto& saved=*entryProfile_;
    if(actual.career.stamp.generation!=saved.generation||actual.career.stamp.mode34!=saved.mode||
       actual.career.career100C0!=saved.career100C0||actual.career.unspent2194!=saved.unspent2194||
       actual.colors.accent17FA00!=saved.primaryColor||actual.colors.secondary17FA10!=saved.secondaryColor)
        return false;
    for(std::size_t i=0;i<actual.current1920.size();++i)
        if(std::bit_cast<std::uint32_t>(actual.current1920[i])!=
           std::bit_cast<std::uint32_t>(saved.current1920[i]))return false;
    return true;
}
bool OriginalUiGaragePageOwner50KG::continueEntryAppearance1FBAB0(Diagnostic& d)noexcept {
    if(entryPhase_!=EntryPhase::appearancePending||!entryProfileCurrent()||!currentConstructor()||
       !managerAttached1D1C40_||!widgets_||entryAppearanceStep_>18)
        return fail(d,Status::entryAppearancePending,0x1FBAB0);
    //These are source manager154 zero-based indices, not textual labels.
    constexpr std::array<std::size_t,13> primaryRows{{2,16,17,18,19,20,21,10,11,12,13,14,15}};
    constexpr std::array<std::uint32_t,13> primaryPC{{0x1FBAEC,0x1FBB00,0x1FBB14,0x1FBB28,
        0x1FBB3C,0x1FBB50,0x1FBB64,0x1FBB78,0x1FBB8C,0x1FBBA0,0x1FBBB4,0x1FBBC8,0x1FBBDC}};
    while(entryAppearanceStep_<13){
        pendingPC_=primaryPC[entryAppearanceStep_];
        if(!widgets_->setPageEntryColor2AA7F0(*this,primaryRows[entryAppearanceStep_],
            entryProfile_->primaryColor,d.widget)){
            if(d.widget.sourcePC)pendingPC_=d.widget.sourcePC;return fail(d,Status::entryAppearancePending,pendingPC_);
        }
        ++entryAppearanceStep_;
    }
    //1FBBE4..1FBDB4 writes existing2DF0B0 packets only if the full color
    //changed: R across four vertices, then G, then B, then descriptor color.
    constexpr std::array<std::size_t,3> quads{{6,4,5}};
    constexpr std::array<std::uint32_t,3> quadPC{{0x1FBBE4,0x1FBC80,0x1FBD1C}};
    constexpr std::array<std::size_t,4> rgbaQwords{{13,10,7,4}};
    while(entryAppearanceStep_<16){
        const auto index=entryAppearanceStep_-13;auto& q=quads_[quads[index]];
        pendingPC_=quadPC[index];
        if(q.stage_!=Quad::Stage::loaded||!q.resource58||q.resource58!=q.packetOwner_.get()||
           !q.material54||q.material54!=q.primitiveOwner_.get()||q.material54->packet10!=q.resource58)
            return fail(d,Status::entryAppearancePending,pendingPC_);
        const auto color=entryProfile_->primaryColor;
        if(q.color44!=color){
            for(std::size_t component=0;component<3;++component){
                const auto half=((color>>(16-8*component))&0xffu)>>1;
                for(const auto qword:rgbaQwords)q.resource58[qword].words[component]=half;
            }
            q.color44=color;
        }
        ++entryAppearanceStep_;
    }
    while(entryAppearanceStep_<18){
        const auto row=entryAppearanceStep_-16;pendingPC_=row?0x1FBDEC:0x1FBDD8;
        if(!widgets_->setPageEntryColor2AA7F0(*this,row,entryProfile_->secondaryColor,d.widget)){
            if(d.widget.sourcePC)pendingPC_=d.widget.sourcePC;return fail(d,Status::entryAppearancePending,pendingPC_);
        }
        ++entryAppearanceStep_;
    }
    entryPhase_=EntryPhase::statPending;pendingPC_=0x1FAB80;return true;
}
bool OriginalUiGaragePageOwner50KG::wattoEntryBindingCurrent1FAC08(
    const OriginalUiGarageWattoOwner50KG& owner)const noexcept {
    //No Page.inputsCurrent call here: Widget's retained row9 may itself use
    //this exact owner during the following1D5C40 operation.
    if(!managerAttached1D1C40_||!grant_||!grant_->current()||grant_->issuer()!=issuer_||
       !sameOwner(authority_,grant_->application())||!grant_->garageAttachedPageCurrent(*this,entity_)||
       entryPhase_<EntryPhase::wattoActivationPending||
       !entryProfileCurrent()||!widgets_||!widgets_->ownsWattoRow(&owner)||watto33B0_!=&owner||
       widgets_->rows_[9].watto.get()!=&owner||!widgets_->slotsKnown_[9]||!widgets_->managerTable154_||
       widgets_->managerTable154_[9]!=widgets_->rows_[9].wattoEntity||
       !scene4C0_||sceneBacking_.get()!=scene4C0_||!scene4C0_->visibility_||
       !scene4C0_->visibility_->current()||scene4C0_->visibility_->primaryZone4C0()!=entryWattoZone_||
       !entryWattoModel_||!entryWattoObserver_)return false;
    OriginalUiGarageWattoOwner50KG::Observation actual;
    if(!owner.inspect(actual)||actual.failed||actual.widget!=widgets_->rows_[9].wattoEntity||
       actual.model!=entryWattoModel_||!actual.model->current())return false;
    const auto* model=actual.model->scene().model();
    return model&&model->observer19C==entryWattoObserver_;
}
bool OriginalUiGaragePageOwner50KG::entryWidgetCallCurrent(
    std::size_t row,std::uint32_t operation,std::int32_t argument)const noexcept {
    //A callback derives its exact target from the currently reached source
    //group/control walk. Never recurse through Widget custody or Page inputs.
    if(!managerAttached1D1C40_||!grant_||!grant_->current()||grant_->issuer()!=issuer_||
       !sameOwner(authority_,grant_->application())||!grant_->garageAttachedPageCurrent(*this,entity_)||
       !entryProfileCurrent()||!widgets_||row>=widgets_->rows_.size()||
       !widgets_->managerTable154_||!widgets_->slotsKnown_[row])return false;
    if(entryPhase_==EntryPhase::returned)return runtimeWidgetCallCurrent(row,operation,argument);
    const C* expected=nullptr;const PageControl* control=nullptr;
    if(entryPhase_==EntryPhase::groupDisablePending||entryPhase_==EntryPhase::groupEnablePending){
        const std::size_t group=entryPhase_==EntryPhase::groupDisablePending?1:2;
        const auto value=group==1?0:1;
        if(operation!=0x2A7A00||argument!=value||!entryGroupStored_||!entryControlStored_||
           !groups_[group]||!groupSlotsKnown_[group]||!groupTable_||groupTable_[group]!=groups_[group].get())return false;
        const auto& g=*groups_[group];
        if(!g.loaded||g.retirementStarted||!g.enabled30||*g.enabled30!=value||entryGroupPass_>1)return false;
        if(!entryGroupPass_){
            if(entryGroupControl_>=g.controlsKnown||!g.controls38)return false;
            control=g.controls38[entryGroupControl_];
        }else{
            if(entryGroupControl_>=g.associatedKnown||!g.associated48)return false;
            control=g.associated48[entryGroupControl_];
        }
        if(!control||control->sectionPresent30!=value)return false;
    }else if(entryPhase_==EntryPhase::groupSelectionPending){
        if(argument||!groups_[2]||!groupTable_||groupTable_[2]!=groups_[2].get())return false;
        const auto& g=*groups_[2];
        if(!g.loaded||g.retirementStarted||!g.selection3C||*g.selection3C!=1||
           !g.controls38||!g.controlsKnown||g.selected40!=g.controls38[0])return false;
        if(entrySelectionStep_==1){
            if(operation!=0x7C||entryGroupControl_>=g.controlsKnown)return false;
            control=g.controls38[entryGroupControl_];
        }else if(entrySelectionStep_==2){
            if(operation!=0x78)return false;
            control=g.selected40;
        }else return false;
    }else if(entryPhase_==EntryPhase::explicitDeselectPending){
        if(operation!=0x7C||argument||entryExplicitWidget_>=6||row!=10+entryExplicitWidget_)return false;
        expected=widgets_->managerTable154_[row];
    }else return false;
    if(control){
        bool retained=false;for(std::size_t i=0;i<controls_.size();++i)
            if(controls_[i].get()==control&&controlSlotsKnown_[i]&&controlTable_&&controlTable_[i]==control)retained=true;
        if(!retained||!control->loaded||control->retirementStarted||!control->widgets38||
           entryControlWidget_>=control->widgetsKnown)return false;
        expected=control->widgets38[entryControlWidget_];
    }
    return expected&&expected==widgets_->managerTable154_[row];
}
bool OriginalUiGaragePageOwner50KG::continueEntryControls1FAC24(Diagnostic& d)noexcept {
    const auto retainedControl=[&](const PageControl* value)noexcept {
        if(!value||!value->loaded||value->retirementStarted||value->payloadRetired||
           value->count34<0||value->widgetsKnown!=static_cast<std::size_t>(value->count34)||
           (value->widgetsKnown&&!value->widgets38))return false;
        for(std::size_t i=0;i<controls_.size();++i)
            if(controlSlotsKnown_[i]&&controls_[i].get()==value&&controlTable_&&controlTable_[i]==value)return true;
        return false;
    };
    const auto call=[&](const C* target,std::uint32_t operation,std::int32_t argument)noexcept {
        if(!target||!widgets_->managerTable154_)return fail(d,Status::entryControlsPending,pendingPC_);
        std::size_t row=0;for(;row<widgets_->rows_.size();++row)
            if(widgets_->slotsKnown_[row]&&widgets_->managerTable154_[row]==target)break;
        if(row==widgets_->rows_.size()||!entryWidgetCallCurrent(row,operation,argument))
            return fail(d,Status::entryControlsPending,pendingPC_);
        if(!widgets_->applyPageEntryWidgetCall(*this,row,operation,argument,d.widget)){
            if(d.widget.sourcePC)pendingPC_=d.widget.sourcePC;
            return fail(d,Status::entryControlsPending,pendingPC_);
        }
        return true;
    };
    if(entryPhase_==EntryPhase::controlsPending){
        shared_.field42C=0; //1FAC24 precedes either group operation.
        entryPhase_=EntryPhase::groupDisablePending;pendingPC_=0x1FAC30;
    }
    while(entryPhase_==EntryPhase::groupDisablePending||entryPhase_==EntryPhase::groupEnablePending){
        const std::size_t group=entryPhase_==EntryPhase::groupDisablePending?1:2;
        const std::int32_t value=group==1?0:1;
        if(!groups_[group]||!groupTable_||groupTable_[group]!=groups_[group].get())
            return fail(d,Status::entryControlsPending,pendingPC_);
        auto& g=*groups_[group];
        if(!g.loaded||g.retirementStarted||g.payloadRetired)
            return fail(d,Status::entryControlsPending,pendingPC_);
        if(!entryGroupStored_){g.enabled30=value;entryGroupStored_=true;} //2A501C.
        while(entryGroupPass_<2){
            const auto count=entryGroupPass_?g.associatedKnown:g.controlsKnown;
            auto* const table=entryGroupPass_?g.associated48.get():g.controls38.get();
            if(count&&!table)return fail(d,Status::entryControlsPending,entryGroupPass_?0x2A5074:0x2A5030);
            while(entryGroupControl_<count){
                auto* control=table[entryGroupControl_];
                if(control){
                    if(!retainedControl(control))return fail(d,Status::entryControlsPending,0x2A5B60);
                    //2A5B7C writes the SAME source control30 which the loader
                    //called sectionPresent30, not a second enabled shadow.
                    if(!entryControlStored_){control->sectionPresent30=value;entryControlStored_=true;}
                    while(entryControlWidget_<control->widgetsKnown){
                        pendingPC_=0x2A5BA4;const auto* widget=control->widgets38[entryControlWidget_];
                        if(widget&&!call(widget,0x2A7A00,value))return false;
                        ++entryControlWidget_;
                    }
                }
                ++entryGroupControl_;entryControlWidget_=0;entryControlStored_=false;
            }
            ++entryGroupPass_;entryGroupControl_=0;
        }
        entryGroupPass_=0;entryGroupControl_=0;entryControlWidget_=0;entryGroupStored_=false;
        entryPhase_=group==1?EntryPhase::groupEnablePending:EntryPhase::groupSelectionPending;
        pendingPC_=group==1?0x1FAC40:0x1FAC50;
    }
    if(entryPhase_==EntryPhase::groupSelectionPending){
        if(!groups_[2]||!groupTable_||groupTable_[2]!=groups_[2].get())
            return fail(d,Status::entryControlsPending,0x2A6BC0);
        auto& g=*groups_[2];
        if(!g.loaded||g.retirementStarted||g.payloadRetired)
            return fail(d,Status::entryControlsPending,0x2A4F50);
        if(!g.count34){entrySelectionStep_=3;}
        if(!entrySelectionStep_){
            if(!g.controls38||!g.controlsKnown)return fail(d,Status::entryControlsPending,0x2A4F70);
            g.selection3C=1;g.selected40=g.controls38[0]; //2A4F70,2A4F90.
            entrySelectionStep_=1;
        }
        if(entrySelectionStep_==1){
            while(entryGroupControl_<g.controlsKnown){
                auto* control=g.controls38[entryGroupControl_];
                if(control&&!retainedControl(control))return fail(d,Status::entryControlsPending,0x2A5A70);
                if(control)while(entryControlWidget_<control->widgetsKnown){
                    pendingPC_=0x2A5AB0;const auto* widget=control->widgets38[entryControlWidget_];
                    if(widget&&!call(widget,0x7C,0))return false;
                    ++entryControlWidget_;
                }
                ++entryGroupControl_;entryControlWidget_=0;
            }
            entrySelectionStep_=2;entryControlWidget_=0;
        }
        if(entrySelectionStep_==2){
            if(g.selected40&&!retainedControl(g.selected40))return fail(d,Status::entryControlsPending,0x2A5A00);
            if(g.selected40)while(entryControlWidget_<g.selected40->widgetsKnown){
                pendingPC_=0x2A5A38;
                //Unlike2A5A70, source2A5A00 has no null-widget skip.
                if(!call(g.selected40->widgets38[entryControlWidget_],0x78,0))return false;
                ++entryControlWidget_;
            }
            entrySelectionStep_=3;
        }
        entryPhase_=EntryPhase::explicitDeselectPending;pendingPC_=0x1FAC6C;
    }
    if(entryPhase_==EntryPhase::explicitDeselectPending){
        constexpr std::array<std::uint32_t,6> pcs{0x1FAC6C,0x1FAC88,0x1FACA4,0x1FACC0,0x1FACDC,0x1FACF8};
        while(entryExplicitWidget_<6){
            pendingPC_=pcs[entryExplicitWidget_];
            if(!call(widgets_->managerTable154_[10+entryExplicitWidget_],0x7C,0))return false;
            ++entryExplicitWidget_;
        }
        entryPhase_=EntryPhase::randomFirstPending;pendingPC_=0x1FAD00;
    }
    return true;
}
bool OriginalUiGaragePageOwner50KG::continueEntryCamera1156E0(Diagnostic& d)noexcept {
    if(entryPhase_!=EntryPhase::cameraActivationPending||!sharedCamera_||
       shared_.camera4BC!=sharedCamera_.get()||!grant_->garagePageCameraCurrent(*this,*sharedCamera_)||
       !sharedCameraDisplayPinned_||!sharedCameraDisplay_||sharedCameraDisplay_->application_!=issuer_||
       !sharedCameraDisplay_->frontendOwnerCurrent50KG()||!sharedCameraDisplay_->sourceCurrentCamera0Known_)
        return fail(d,Status::entryCameraPending,pendingPC_);
    auto& camera=*sharedCamera_;auto& display=*sharedCameraDisplay_;
    if(!entryCameraStep_){
        pendingPC_=0x141578;camera.entity_.flags29=static_cast<std::uint8_t>((camera.entity_.flags29&~1u)|1u);
        pendingPC_=0x14158C;camera.entity_.flags29=static_cast<std::uint8_t>((camera.entity_.flags29&~2u)|2u);
        entryCameraStep_=1;
    }
    if(entryCameraStep_==1){
        pendingPC_=0x1414CC;
        //Camera+2C is1414C0. This actual retained Camera has no Entity
        //children; never pretend the nonempty recursive walk completed.
        if(camera.entity_.firstChild18)return fail(d,Status::entryCameraPending,pendingPC_);
        entryCameraStep_=2;
    }
    if(entryCameraStep_==2){
        pendingPC_=0x26F948;display.sourceSelectedCamera0_=sharedCamera_;
        entryCameraStep_=3;
    }
    if(entryCameraStep_==3){
        pendingPC_=0x26F94C;
        if(!sameOwner(display.sourceSelectedCamera0_,sharedCamera_))
            return fail(d,Status::entryCameraPending,0x26F94C);
        //Actual26F940 compares dimensions after its selection store. The
        //unrepresented resize114920 remains closed with that store retained.
        if(display.sourceWidth4_!=static_cast<std::int32_t>(camera.fields_.fullWidth204)||
           display.sourceHeight8_!=static_cast<std::int32_t>(camera.fields_.fullHeight208)){
            pendingPC_=0x26F978;
            return fail(d,Status::entryCameraPending,0x26F978);
        }
        entryCameraStep_=4;
    }
    if(entryCameraStep_==4){
        pendingPC_=0x11570C;
        //This is the real243CB4 Display buffer count, also consumed by the
        //earlier114A10/115050. It is not a frame number or an invented default.
        if(!display.sourceBufferCount0CInitialized_)
            return fail(d,Status::entryCameraPending,pendingPC_);
        camera.fields_.count210=display.sourceBufferCount0C_+std::uint32_t{1}; //115714.
        entryCameraStep_=5;
    }
    entryPhase_=EntryPhase::cameraSlotsPending;pendingPC_=0x1FADC0;return true;
}
bool OriginalUiGaragePageOwner50KG::entryCameraActivationReturned115724()const noexcept {
    //The Root's1401B0 writer authenticates this exact completed call, never
    //a caller boolean or a replacement array of nominal Camera handles.
    return entryPhase_==EntryPhase::cameraSlotsPending&&entryCameraStep_==5&&
        managerAttached1D1C40_&&grant_&&grant_->current()&&grant_->issuer()==issuer_&&
        sameOwner(authority_,grant_->application())&&grant_->garageAttachedPageCurrent(*this,entity_)&&
        entryProfileCurrent()&&sharedCamera_&&shared_.camera4BC==sharedCamera_.get()&&
        grant_->garagePageCameraCurrent(*this,*sharedCamera_)&&
        (sharedCamera_->entity_.flags29&3u)==3u&&!sharedCamera_->entity_.firstChild18&&
        sharedCameraDisplayPinned_&&sharedCameraDisplay_&&sharedCameraDisplay_->application_==issuer_&&
        sharedCameraDisplay_->frontendOwnerCurrent50KG()&&sharedCameraDisplay_->sourceCurrentCamera0Known_&&
        sharedCameraDisplay_->sourceBufferCount0CInitialized_&&
        sameOwner(sharedCameraDisplay_->sourceSelectedCamera0_,sharedCamera_)&&
        sharedCamera_->fields_.count210==sharedCameraDisplay_->sourceBufferCount0C_+std::uint32_t{1};
}
bool OriginalUiGaragePageOwner50KG::sourceEntryReturned1FAE00()const noexcept {
    return entryPhase_==EntryPhase::returned&&managerAttached1D1C40_&&
        sourceLoadReturned1FAAC8()&&entryProfileCurrent()&&entryAppearanceStep_==18&&
        entryStatCount_==6&&entryExplicitWidget_==6&&entryCameraStep_==5&&
        watto33B0_&&wattoEntryBindingCurrent1FAC08(*watto33B0_);
}
bool OriginalUiGaragePageOwner50KG::continueEntry1FAAF0(Diagnostic& d)noexcept {
    d={};
    //A failed real observer/motion call retains its exact failure, before
    //general custody checks which correctly reject the damaged live owner.
    if(entryPhase_==EntryPhase::wattoBindingFailed||entryPhase_==EntryPhase::wattoActivationFailed){
        d.loadResources=entryWattoFailure_;return fail(d,Status::entryWattoPending,pendingPC_);
    }
    if(!sourceActivationReturned2A680C())return fail(d,Status::activationShowPending,0x2A3C44);
    if(!managerAttached1D1C40_||!sourceLoadReturned1FAAC8()||!sharedCollectionsPayloadCurrent())
        return fail(d,Status::phase,0x1FAAF0);
    if(entryPhase_==EntryPhase::notEntered){
        //These are genuine entry stores. In particular4E4 is NOT reset.
        information4DC_=0;entryField4E0_=0; //1FAB08,1FAB0C.
        enabled34C8_[0]=0;enabled34C8_[1]=0; //1FAB10,1FAB14.
        entryPhase_=EntryPhase::profilePending;pendingPC_=0x1FAB38;
    }
    if(entryPhase_==EntryPhase::profilePending){
        if(!issuer_->riderProfile||!issuer_->profile||
           issuer_->riderProfile->commonProfile()!=issuer_->profile.get())
            return fail(d,Status::entryProfilePending,0x1FAB30);
        OriginalUiRiderProfileOwner50KG::GarageProfileSnapshot actual;
        if(!issuer_->riderProfile->selectedGarageProfileSnapshot(actual))
            return fail(d,Status::entryProfilePending,0x1FAB38);
        //This native SceneAssets factory authenticates19E only. Do not recolor
        //or publish statistics for another career while borrowing that model.
        //The original -1/packed fallback is not manufactured by this API.
        pendingPC_=0x1FAB5C;
        if(actual.career.career100C0!=19||actual.career.stamp.mode34!=2)
            return fail(d,Status::entryProfilePending,0x1FAB5C);
        EntryProfile held;held.owner=issuer_->riderProfile;
        held.generation=actual.career.stamp.generation;held.mode=actual.career.stamp.mode34;
        held.career100C0=actual.career.career100C0;held.current1920=actual.current1920;
        held.unspent2194=actual.career.unspent2194;held.primaryColor=actual.colors.accent17FA00;
        held.secondaryColor=actual.colors.secondary17FA10;entryProfile_=std::move(held);
        entryPhase_=EntryPhase::appearancePending;pendingPC_=0x1FAB5C;
    }
    if(!entryProfileCurrent())return fail(d,Status::entryProfilePending,pendingPC_);
    if(entryPhase_==EntryPhase::appearancePending&&!continueEntryAppearance1FBAB0(d))return false;
    if(entryPhase_==EntryPhase::statPending){
        for(;entryStatCount_<6;++entryStatCount_){
            pendingPC_=0x1FAB80;std::int32_t actual=0;
            if(!game::convertOriginalUiGarageStat50KG(entryProfile_->current1920[entryStatCount_],actual))
                return fail(d,Status::entryNumbersPending,0x188D70);
            current4E8_[entryStatCount_]=actual; //Actual188D70 return stored at1FAB8C.
        }
        entryPhase_=EntryPhase::deltaPending;pendingPC_=0x1FABA8;
    }
    if(entryPhase_==EntryPhase::deltaPending){
        for(std::size_t pair=0;pair<3;++pair){delta500_[2*pair]=0;delta500_[2*pair+1]=0;}
        entryPhase_=EntryPhase::cashPending;pendingPC_=0x1FABE8;
    }
    if(entryPhase_==EntryPhase::cashPending){
        available34B0_=std::bit_cast<std::int32_t>(entryProfile_->unspent2194); //1FABF0 delay slot.
        entryPhase_=EntryPhase::divisibility5Pending;pendingPC_=0x1FABEC;
    }
    const auto sourceDigitSum=[](std::int32_t amount)noexcept {
        std::int32_t sum=0;for(;amount>0;amount/=10)sum+=amount%10;return sum;
    };
    if(entryPhase_==EntryPhase::divisibility5Pending){
        enabled34C8_[1]=sourceDigitSum(*available34B0_)%5==0?1u:0u; //1FAE10..88.
        entryPhase_=EntryPhase::divisibility4Pending;pendingPC_=0x1FABF4;
    }
    if(entryPhase_==EntryPhase::divisibility4Pending){
        enabled34C8_[0]=sourceDigitSum(*available34B0_)%4==0?1u:0u; //1FAE90..1FAF08.
        entryPhase_=EntryPhase::wattoBindingPending;pendingPC_=0x1FAC08;
    }
    if(entryPhase_==EntryPhase::wattoBindingFailed){
        d.loadResources=entryWattoFailure_;return fail(d,Status::entryWattoPending,pendingPC_);
    }
    if(entryPhase_==EntryPhase::wattoBindingPending){
        OriginalUiGarageWattoOwner50KG::Observation actual;
        if(!watto33B0_||!widgets_->ownsWattoRow(watto33B0_)||
           !watto33B0_->inspect(actual)||actual.failed||!actual.model||!actual.model->current()||
           actual.widget!=widgets_->rows_[9].wattoEntity||!scene4C0_->visibility_||
           !scene4C0_->visibility_->current()||!scene4C0_->visibility_->primaryZone4C0())
            return fail(d,Status::entryWattoPending,0x1FAC08);
        entryWattoModel_=actual.model;entryWattoZone_=scene4C0_->visibility_->primaryZone4C0();
        //1D481C remove precedes1D4830 mode1 attach. The existing real owner
        //performs both; failures retain its partial observer and are not replayed.
        if(!watto33B0_->bindPageVisibility1FAC08(*this,scene4C0_->visibility_,d.loadResources)||
           !scene4C0_->visibility_->attachWatto1D4800(*watto33B0_,d.loadResources)){
            retainWattoBindingFailure1D4800(d.loadResources);
            entryPhase_=EntryPhase::wattoBindingFailed;entryWattoFailure_=d.loadResources;
            pendingPC_=d.loadResources.sourcePC?d.loadResources.sourcePC:0x1D4830;
            return fail(d,Status::entryWattoPending,pendingPC_);
        }
        entryWattoObserver_=entryWattoModel_->scene().model()->observer19C;
        entryPhase_=EntryPhase::wattoActivationPending;pendingPC_=0x1FAC1C;
    }
    if(entryPhase_==EntryPhase::wattoActivationPending){
        if(!wattoEntryBindingCurrent1FAC08(*watto33B0_))return fail(d,Status::entryWattoPending,0x1FAC08);
        entryWattoActivationStarted_=true;
        if(!watto33B0_->enterPage1FAC1C(*this,d.loadResources)){
            OriginalUiGarageWattoOwner50KG::Observation after;
            if(watto33B0_->inspect(after)&&after.failed){
                entryPhase_=EntryPhase::wattoActivationFailed;entryWattoFailure_=d.loadResources;
            }
            pendingPC_=d.loadResources.sourcePC?d.loadResources.sourcePC:0x1FAC1C;
            return fail(d,Status::entryWattoPending,pendingPC_);
        }
        entryPhase_=EntryPhase::controlsPending;pendingPC_=0x1FAC24;
    }
    if(entryPhase_>=EntryPhase::controlsPending&&entryPhase_<=EntryPhase::explicitDeselectPending)
        if(!continueEntryControls1FAC24(d))return false;
    if(entryPhase_==EntryPhase::randomFirstPending){
        if(!issuer_->state||!issuer_->state->known)return fail(d,Status::entryRandomPending,0x1FAD00);
        const auto raw=game::drawOriginalSharedRng50IU(issuer_->state->sharedNoise308030);
        //Explicit binary32 instruction boundaries, no fused multiplication/add.
        const volatile float unit=static_cast<float>(raw)*std::bit_cast<float>(0x30000000u);
        const volatile float scaled=5.0f*unit;const volatile float first=8.0f+scaled;
        const volatile float second=2.5f+first;
        timer34B4_[0]=std::bit_cast<std::uint32_t>(static_cast<float>(first)); //1FAD44.
        timer34B4_[1]=std::bit_cast<std::uint32_t>(static_cast<float>(second)); //1FAD4C delay slot.
        entryPhase_=EntryPhase::randomSecondPending;pendingPC_=0x1FAD48;
    }
    if(entryPhase_==EntryPhase::randomSecondPending){
        if(!issuer_->state||!issuer_->state->known)return fail(d,Status::entryRandomPending,0x1FAD48);
        const auto raw=game::drawOriginalSharedRng50IU(issuer_->state->sharedNoise308030);
        const volatile float unit=static_cast<float>(raw)*std::bit_cast<float>(0x30000000u);
        const volatile float scaled=5.0f*unit;const volatile float value=8.0f+scaled;
        timer34B4_[2]=std::bit_cast<std::uint32_t>(static_cast<float>(value)); //1FAD84.
        entryPhase_=EntryPhase::cameraActivationPending;pendingPC_=0x1FAD9C;
    }
    if(entryPhase_==EntryPhase::cameraActivationPending&&!continueEntryCamera1156E0(d))return false;
    if(entryPhase_==EntryPhase::cameraSlotsPending){
        if(!issuer_->rewriteGarageCameraSlots1401B0(*this))
            return fail(d,Status::entryCameraPending,0x1FADC0);
        entryPhase_=EntryPhase::popupPending;pendingPC_=0x1FADDC;
    }
    if(entryPhase_==EntryPhase::popupPending){
        if(!issuer_->state||!issuer_->state->sharedDialogs)
            return fail(d,Status::entryPopupPending,0x1FADD8);
        auto& dialogs=*issuer_->state->sharedDialogs;
        if(!dialogs.reorderCompleteFor1B872C(*this)||!sameOwner(dialogs.application_.lock(),issuer_->state)||
           dialogs.inputs_.root!=issuer_||!dialogs.controller47C_.loaded)
            return fail(d,Status::entryPopupPending,0x1FADDC);
        //Actual retained47C state. No popup is attached, started or rendered.
        issuer_->state->controllerPopupState.result3D8=0;
        entryPhase_=EntryPhase::backgroundPending;pendingPC_=0x1FADE8;
    }
    if(entryPhase_==EntryPhase::backgroundPending){
        if(!shared_.backgroundColor430||!sharedCameraDisplayPinned_||!sharedCameraDisplay_||
           sharedCameraDisplay_->application_!=issuer_||!sharedCameraDisplay_->frontendOwnerCurrent50KG())
            return fail(d,Status::entryBackgroundPending,0x1FADE8);
        sharedCameraDisplay_->sourceFogColor28_=*shared_.backgroundColor430;
        sharedCameraDisplay_->sourceFogColor28Known_=true; //Actual1FADE8 first writer.
        entryPhase_=EntryPhase::returned;pendingPC_=0x1FAE00;
    }
    if(!sourceEntryReturned1FAE00())return fail(d,Status::phase,pendingPC_);
    d.sourcePC=0x1FAE00;d.sourceEffectsCommitted=true;return true;
}

bool OriginalUiGaragePageOwner50KG::retireLateSceneChildren142590(Diagnostic& d)noexcept {
    d={};if(!currentConstructor()||!scene4C0_||sceneBacking_.get()!=scene4C0_||
       !sceneLateBorrowCaptured_||sceneLateAttachCursor_!=sceneLateChildren_.size())
        return fail(d,Status::childRetirementPending,0x1425B8);
    while(sceneLateRetiredCount_<sceneLateChildren_.size()){
        const auto ordinal=sceneLateChildren_.size()-1-sceneLateRetiredCount_;
        assets::OriginalStandardMaterialDiagnostic50LA sd;
        const bool done=ordinal?scene4C0_->retirePageMaterialSoundList(*this,ordinal,sd):
            scene4C0_->retirePageParticleList27DCD0(*this,sd);
        if(!done)return fail(d,Status::childRetirementPending,sd.sourcePC);
        ++sceneLateRetiredCount_;
    }
    //Camera and the earlier ten Scene children still follow tail-first.
    d.sourceEffectsCommitted=true;d.sourcePC=0x1425B8;return true;
}
bool OriginalUiGaragePageOwner50KG::retireCameraChild231310(Diagnostic& d)noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    d={};if(!currentConstructor()||!sharedCamera_||shared_.camera4BC!=sharedCamera_.get()||
       !scene4C0_||sceneBacking_.get()!=scene4C0_||!scene4C0_->visibility_||
       !sharedCameraDisplayPinned_||!sharedCameraDisplay_||sharedCameraDisplay_->application_!=issuer_||
       !sharedCameraDisplay_->frontendOwnerCurrent50KG()||!sharedCameraDisplay_->sourceCurrentCamera0Known_)
        return fail(d,Status::childRetirementPending,0x231310);
    auto& camera=*sharedCamera_;
    if(!cameraRetirementStep_){
        if(camera.phase_!=Camera::Phase::pageAttached1FB620||!camera.nativeRegistrationIdentityCurrent()||
           camera.entity_.parent1C!=&entity_||camera.entity_.next14||camera.entity_.firstChild18||
           !scene4C0_->visibility_->pageCameraRegistrationCurrent(*this,camera))
            return fail(d,Status::childRetirementPending,0x1425B8);
        d.scope=grant_->beginGaragePageCameraRetirement142590(*this,camera);
        if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
        if(camera.phase_!=Camera::Phase::pageRetiring1155C0||camera.enrolled_||camera.entity_.reference00)
            return fail(d,Status::phase,0x104970);
        camera.ancestryTable_=0x3D1C10; //231334, before base destructor.
        camera.ancestryTable_=0x3CCA00; //1155E4.
        cameraRetirementStep_=1;
    }
    if(cameraRetirementStep_==1){
        assets::OriginalStandardMaterialDiagnostic50LA vd;
        if(!scene4C0_->visibility_->retirePageCameraObserver1155F4(*this,camera,vd))
            return fail(d,Status::childRetirementPending,vd.sourcePC);
        cameraRetirementStep_=2;
    }
    if(cameraRetirementStep_==2){
        //1144F4 genuinely initialized224 to null. None of the reached native
        //viewport/pose operations allocates it. Positive13D9F0 stays closed.
        if(camera.fields_.resource224)return fail(d,Status::childRetirementPending,0x13D9F0);
        cameraRetirementStep_=3;
    }
    if(cameraRetirementStep_==3){
        if(!camera.retireApplicationCameraSlots115620())return fail(d,Status::childRetirementPending,0x115620);
        cameraRetirementStep_=4;
    }
    if(cameraRetirementStep_==4){
        if(!sharedCameraDisplay_->retireSelectedCamera115690(camera))
            return fail(d,Status::rendererRetirementPending,0x115690);
        cameraRetirementStep_=5;
    }
    if(cameraRetirementStep_==5){
        d.scope=grant_->finishGaragePageCameraRetirement1413E0(*this,camera);
        if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
        if(camera.phase_!=Camera::Phase::released)return fail(d,Status::phase,0x1413E0);
        cameraRetirementStep_=6;
    }
    //Page4BC and Camera34/38 are not nulled by231310/1155C0. Retained native
    //backing safely carries those source observations through later List free.
    d.sourceEffectsCommitted=true;d.sourcePC=0x231358;return true;
}
bool OriginalUiGaragePageOwner50KG::retireSceneChildren142590(Diagnostic& d)noexcept {
    d={};if(!currentConstructor()||!scene4C0_||sceneBacking_.get()!=scene4C0_||
       !scenePageBorrowCaptured_||scenePageAttachCursor_!=scenePageChildren_.size())
        return fail(d,Status::childRetirementPending,0x1425B8);
    if(!retireLateSceneChildren142590(d)||!retireCameraChild231310(d))return false;
    return retireInitialSceneChildren142590(d);
}
bool OriginalUiGaragePageOwner50KG::actorSampleFailureCurrent()const noexcept {
    if(!issuer_||!grant_||!entityLive_||entityRefReleased_||entityCountReleased_||
       !updateFailed_||updateSourcePC_!=0x10179C||!updatePublication_||updateReturned_||
       completedUpdateFrames_||lastCompletedUpdatePublication_||updatedPageChildren_!=8||
       !scene4C0_||sceneBacking_.get()!=scene4C0_||scenePageAttachCursor_!=10||sceneLateAttachCursor_!=3||
       !scenePageBorrowCaptured_||!sceneLateBorrowCaptured_||cameraHandoffPhase_!=3||
       updateChild_!=scenePageChildren_[7]||!managerAttached1D1C40_||
       stage_!=Stage::presentationLoaded||entryPhase_!=EntryPhase::returned)return false;
    const auto owner=issuer_->garageCustodyOwner_;
    return owner&&owner->page_.get()==this&&owner->actorSamplePage_==this&&
        owner->normalRetirementPhase_==OriginalUiGarageSession50KG::NormalRetirementPhase::managerDetached&&
        owner->actorSampleFailureRetirementCurrent(*issuer_);
}
bool OriginalUiGaragePageOwner50KG::retireInitialSceneChildren142590(Diagnostic& d)noexcept {
    d={};
    // Shared only after the actual Camera tail's final source count release.
    // Each callback below still authenticates the real count10 Scope ledger.
    if(!currentConstructor()||!scene4C0_||sceneBacking_.get()!=scene4C0_||
       !scenePageBorrowCaptured_||scenePageAttachCursor_!=scenePageChildren_.size()||
       sceneInitialRetiredCount_>scenePageChildren_.size()||cameraRetirementStep_!=6||
       !sharedCamera_||shared_.camera4BC!=sharedCamera_.get()||
       sharedCamera_->phase_!=game::OriginalFrontendCameraCold50LA::Phase::released||
       sharedCamera_->enrolled_||sharedCamera_->entity_.reference00)
        return fail(d,Status::childRetirementPending,0x1425B8);
    while(sceneInitialRetiredCount_<scenePageChildren_.size()){
        const auto ordinal=scenePageChildren_.size()-1-sceneInitialRetiredCount_;
        assets::OriginalStandardMaterialDiagnostic50LA sd;bool done=false;
        switch(ordinal){
        case 9:done=scene4C0_->retirePageParticleManager160C70(*this,sd);break;
        case 8:if(actorSampleFailureCurrent()){
            done=scene4C0_->actorOwner3C8_&&scene4C0_->actorOwner3C8_->retireFailedSampleToyList142590(*this,sd);break;
        }
        done=scene4C0_->actorOwner3C8_&&
            (wattoBindingFailureCurrent()?scene4C0_->actorOwner3C8_->retireFailedWattoBindingToyList142590(*this,sd):
             (loadedPageHideFailureCurrent()?scene4C0_->actorOwner3C8_->retireFailedLoadedPageToyList142590(*this,sd):
             lateSceneBorrowFailureCurrent()?scene4C0_->actorOwner3C8_->retireFailedLateBorrowToyList142590(*this,sd):
                scene4C0_->actorOwner3C8_->retirePageToyList142590(*this,sd)));break;
        case 7:if(actorSampleFailureCurrent()){
            done=scene4C0_->actorOwner3C8_&&scene4C0_->actorOwner3C8_->retireFailedSampleAnimationManager101FE0(*this,sd);break;
        }
        done=scene4C0_->actorOwner3C8_&&
            (wattoBindingFailureCurrent()?scene4C0_->actorOwner3C8_->retireFailedWattoBindingAnimationManager101FE0(*this,sd):
             (loadedPageHideFailureCurrent()?scene4C0_->actorOwner3C8_->retireFailedLoadedPageAnimationManager101FE0(*this,sd):
             lateSceneBorrowFailureCurrent()?scene4C0_->actorOwner3C8_->retireFailedLateBorrowAnimationManager101FE0(*this,sd):
                scene4C0_->actorOwner3C8_->retirePageAnimationManager101FE0(*this,sd)));break;
        case 6:done=scene4C0_->sceneCollision_&&scene4C0_->sceneCollision_->retirePageRawList27BB80(*this,sd);break;
        case 5:done=scene4C0_->staticModels_&&scene4C0_->staticModels_->retirePage27B650(*this,sd);break;
        case 4:case 3:case 2:done=scene4C0_->retirePageFog13E700(*this,ordinal-2,sd);break;
        case 1:done=scene4C0_->retirePageLightList2781B0(*this,sd);break;
        case 0:done=scene4C0_->visibility_&&scene4C0_->visibility_->retirePageSearchEntities142590(*this,sd);break;
        default:break;
        }
        if(!done)return fail(d,Status::childRetirementPending,sd.sourcePC?sd.sourcePC:0x1425B8);
        ++sceneInitialRetiredCount_;
    }
    d.sourceEffectsCommitted=true;d.sourcePC=0x1425B8;return true;
}
bool OriginalUiGaragePageOwner50KG::beginRetirement142590(Diagnostic& d)noexcept {
    d={};
    //Native continuation after an actual last-reference dispatch never
    //replays142590 or its already completed child callbacks.
    if(stage_==Stage::pageRefReleased)return continuePayloadRetirement1FA170(d);
    if(!currentConstructor())return fail(d,Status::phase,0x142590);
    if(scenePageAttachCursor_){
        if(scenePageAttachCursor_!=scenePageChildren_.size()||sceneLateAttachCursor_!=sceneLateChildren_.size())
            return fail(d,Status::childRetirementPending,0x1425B8);
        if(!retireSceneChildren142590(d))return false;
    }
    if(sharedCamera_&&sharedCamera_->phase_!=game::OriginalFrontendCameraCold50LA::Phase::released)
        return fail(d,Status::childRetirementPending,0x1FB620);
    if(router_&&(router_->retirementStarted||entity_.firstChild18==&router_->entity)){
        if(!retireRouter2A7010(d))return false;
    }
    if(entity_.firstChild18||router_)return fail(d,Status::childRetirementPending,0x1425B8);
    // A Scene child forest being empty does not prove PAGE child retirement.
    // No Scene ref release is inferred from a detached Scene or backend flag.
    d.scope=grant_->beginGaragePageRetirement142590(this,entity_);
    if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    entityRefReleased_=true;stage_=Stage::pageRefReleased;
    //The real Scene call precedes every embedded Page payload. Its retained
    //continuation stops at any unavailable actual owner without skipping it.
    pendingPC_=scene4C0_?0x1FA1B4:0x2DEFE0;
    return continuePayloadRetirement1FA170(d);
}
bool OriginalUiGaragePageOwner50KG::drainEmbedded2DEFE0(std::uint8_t& phase,Diagnostic& d,
 std::uint32_t firstPC,std::uint32_t secondPC)noexcept {
    if(phase>2||!grant_||!grant_->garagePageRetirementCurrent(*this))return fail(d,Status::phase,firstPC);
    if(!sharedCameraDisplay_){
        const auto display=issuer_->raceDisplayOwner50LA_.lock();
        if(!display||display->application_!=issuer_||!display->frontendOwnerCurrent50KG()||!display->canRetain())
            return fail(d,Status::rendererRetirementPending,firstPC);
        sharedCameraDisplay_=display;display->pin();sharedCameraDisplayPinned_=true;
    }
    auto& display=*sharedCameraDisplay_;
    if(!sharedCameraDisplayPinned_||display.application_!=issuer_||!display.frontendOwnerCurrent50KG()||!display.canRetain())
        return fail(d,Status::rendererRetirementPending,firstPC);
    if(!phase){
        std::int32_t actualV0=0;
        const auto r=game::serviceOriginalRenderer244D60(display.dispatch_,true,display.renderer_,d.renderer,
            actualV0,game::OriginalCameraDrainPolicy::initializedContext);
        if(r.status!=game::OriginalCameraStartStatus::complete)return fail(d,Status::rendererRetirementPending,r.sourcePC);
        phase=1;
    }
    if(phase==1){
        //Unconditional second source slot24 call. A failed completion keeps
        //phase1, so a retry never repeats the already completed slot20 call.
        if(!display.renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted)
            return fail(d,Status::rendererRetirementPending,secondPC);
        phase=2;
    }
    return true;
}
bool OriginalUiGaragePageOwner50KG::retireQuad2DEFE0(std::size_t index,Diagnostic& d)noexcept {
    if(index>=quads_.size()||index+1!=quadCount_||!quads_[index].constructed)
        return fail(d,Status::phase,0x2DEFE0);
    auto& q=quads_[index];
    if(!q.preflightRetirement2DEFE0(d.quad))return fail(d,Status::rendererRetirementPending,d.quad.sourcePC);
    if(!drainEmbedded2DEFE0(q.retirementDrainPhase_,d,0x2DF018,0x2DF030))return false;
    if(!q.retireResources2DF038(d.quad))return fail(d,Status::rendererRetirementPending,d.quad.sourcePC);
    --quadCount_;return true;
}
bool OriginalUiGaragePageOwner50KG::retireAperture2E1980(std::size_t ordinal,Diagnostic& d)noexcept {
    if(ordinal>=2||ordinal!=retiredApertures_||!shared_.apertures424[ordinal].has_value())
        return fail(d,Status::phase,0x1FA23C);
    if(*shared_.apertures424[ordinal]!=apertures_[ordinal].get())return fail(d,Status::phase,0x1FA23C);
    if(!apertures_[ordinal]){++retiredApertures_;return true;} //A genuine stored null only.
    auto& p=*apertures_[ordinal];
    //Only the actually reached one-packet/four-vertex polygon layout exists.
    //Its unsupported/partially unknown alternatives are not silently freed.
    if(p.count34!=1||p.count48!=1||p.capacity4C!=1||!p.primitives30||!p.packets44||
       p.count04!=4||p.count08!=4||p.owns38!=1||!p.vertices00||
       !Quad::preflightPrimitive246260(p.primitives30[0],d.quad))
        return fail(d,Status::rendererRetirementPending,d.quad.sourcePC?d.quad.sourcePC:0x2E19D4);
    if(!drainEmbedded2DEFE0(apertureRetirementDrains_[ordinal],d,0x2E19B4,0x2E19CC))return false;
    if(!Quad::preflightPrimitive246260(p.primitives30[0],d.quad))
        return fail(d,Status::rendererRetirementPending,d.quad.sourcePC);
    if(p.nativeGarageFifoPins_||p.nativeMenuWarningPins_)return fail(d,Status::rendererRetirementPending,0x2E1A08);
    Quad::releasePrimitive246260(p.primitives30[0]); //2E1A08 BEFORE packet backing.
    p.primitives30.reset(); //2E1A28.
    p.packets44[0].allocation00.reset();p.packets44.reset(); //2E1A5C then2E1A80.
    p.vertices00.reset(); //2E1A9C:2DFC10 has no per-vertex payload/ref side effect.
    apertures_[ordinal].reset(); //2E1AB4. Source424/428 is NOT cleared.
    ++retiredApertures_;return true;
}
bool OriginalUiGaragePageOwner50KG::retireScene1FA1B4(Diagnostic& d)noexcept {
    if(stage_!=Stage::pageRefReleased||!entityRefReleased_||entityCountReleased_||
       !scene4C0_||!sceneBacking_||retiredSceneBacking_||sceneBacking_.get()!=scene4C0_||
       !grant_||!grant_->garagePageRetirementCurrent(*this))
        return fail(d,Status::phase,0x1FA1B4);
    auto& scene=*sceneBacking_;using R=OriginalUiGaragePodScene50KG::RetirementStage;
    assets::OriginalStandardMaterialDiagnostic50LA sd;std::string error;
    if(scene.retirementStage_==R::none){
        const auto& entry=issuer_->garageCustodyOwner_;
        const bool failedPartial=failedSceneRetirement_==FailedSceneRetirement::payloadRetiring;
        const bool failedLate=failedLateBorrowRetirement_==FailedLateBorrowRetirement::payloadRetiring;
        const bool failedLoaded=failedLoadedHideRetirement_==FailedLoadedHideRetirement::payloadRetiring;
        const bool failedBinding=failedWattoBindingRetirement_==FailedWattoBindingRetirement::payloadRetiring;
        if(sceneReferenceReleased_||(static_cast<unsigned>(failedPartial)+static_cast<unsigned>(failedLate)+
           static_cast<unsigned>(failedLoaded)+static_cast<unsigned>(failedBinding)>1)||(failedBinding?
           (!entry||!scene.beginFailedWattoBindingRetirement1FA1B4(*this,*entry,error)):(failedLoaded?
           (!entry||!scene.beginFailedLoadedHideRetirement1FA1B4(*this,*entry,error)):
           (failedLate?(!entry||!scene.beginFailedLateBorrowRetirement1FA1B4(*this,*entry,error)):
            (failedPartial?(!entry||!scene.beginFailedPartialRetirement1FA1B4(*this,*entry,error)):
             !scene.beginRetirementAt1FA1B4(*this,error))))))
            return fail(d,Status::scenePending,0x1FA1B4);
        sceneReferenceReleased_=true;
    }
    if(!sceneReferenceReleased_)return fail(d,Status::phase,0x1FA1B4);
    while(scene.retirementStage_!=R::complete){
        bool done=false;sd={};
        switch(scene.retirementStage_){
        case R::lastReference:case R::interpolation510Released:case R::interpolation514Released:
        case R::buoysReleased:case R::animationArraysReleased:done=scene.retirePodScenePrefix1B2920(sd);break;
        case R::worldListReleased:done=scene.retireConfig34At26FDB8(sd);break;
        case R::configClosed:{
            if(failedWattoBindingRetirement_==FailedWattoBindingRetirement::payloadRetiring){
                const auto& entry=issuer_->garageCustodyOwner_;
                done=entry&&scene.failedWattoBindingRetirementOwner_==this&&!scene.failedPageRetirementOwner_&&
                    !scene.failedLateBorrowRetirementOwner_&&!scene.failedLoadedHideRetirementOwner_&&
                    scene.retireUnresolvedSoundGroupForWattoBindingFailure(*entry,error);
            }else if(failedLoadedHideRetirement_==FailedLoadedHideRetirement::payloadRetiring){
                const auto& entry=issuer_->garageCustodyOwner_;
                done=entry&&scene.failedLoadedHideRetirementOwner_==this&&!scene.failedPageRetirementOwner_&&
                    !scene.failedLateBorrowRetirementOwner_&&
                    scene.retireUnresolvedSoundGroupForLoadedPageFailure(*entry,error);
            }else if(failedLateBorrowRetirement_==FailedLateBorrowRetirement::payloadRetiring){
                const auto& entry=issuer_->garageCustodyOwner_;
                done=entry&&scene.failedLateBorrowRetirementOwner_==this&&!scene.failedPageRetirementOwner_&&
                    scene.retireUnresolvedSoundGroupForLateBorrowFailure(*entry,error);
            }else if(failedSceneRetirement_==FailedSceneRetirement::payloadRetiring){
                const auto& entry=issuer_->garageCustodyOwner_;
                done=entry&&scene.failedPageRetirementOwner_==this&&
                    scene.retireUnresolvedSoundGroupForEntryFailure(*entry,error);
            }else done=scene.retireSoundGroup26FDD4(error);
            sd.sourcePC=0x26FDD4;break;
        }
        case R::groupReleased:done=scene.retireStartData26FDEC(sd);break;
        case R::startDataReleased:done=scene.retireZones26FE08(sd);break;
        case R::zonesReleased:done=scene.retirePortalArray26FE14(sd);break;
        case R::portalArrayReleased:done=scene.retireEnvironment276B30(sd);break;
        case R::environmentReleased:done=scene.retireFogDescriptor26FE58(sd);break;
        case R::fogDescriptorReleased:done=scene.retireTerrainInfo26FE70(sd);break;
        case R::terrainInfoReleased:done=scene.retireStaticPointerArray26FE88(sd);break;
        case R::staticPointerArrayReleased:done=scene.retirePhysicsPointerArray26FEA0(sd);break;
        case R::physicsPointerArrayReleased:done=scene.retirePhysicsManager26FEC8(sd);break;
        case R::physicsManagerReleased:done=scene.retireZoneLists27C030(sd);break;
        case R::zoneListsReleased:done=scene.retireCurrentInstanceMaster26FEF8(sd);break;
        case R::instanceMasterReleased:done=scene.retirePrimarySearchPointers26FF14(sd);break;
        case R::primarySearchPointersReleased:done=scene.retireSecondarySearchPointers26FF2C(sd);break;
        case R::secondarySearchPointersReleased:done=scene.retireCollisionPointers26FF44(sd);break;
        case R::collisionPointersReleased:done=scene.clearEnvironmentGlobal26FF54(sd);break;
        case R::environmentGlobalCleared:done=scene.retireSkyPointerArray26FF64(sd);break;
        case R::skyPointerArrayReleased:done=scene.retireSearchListPayloads26FF6C(sd);break;
        case R::searchListPayloadsReleased:done=scene.retireCameraZonePointers2700F8(sd);break;
        case R::cameraZonePointersReleased:done=scene.retireSunFlarePointers270110(sd);break;
        case R::sunFlarePointersReleased:done=scene.retireLightManagerZones270120(sd);break;
        case R::lightManagerZonesReleased:done=scene.retireDefaultZoneObserver270138(sd);break;
        case R::defaultZoneObserverReleased:done=scene.retireAuralRegistrations27015C(sd);break;
        case R::auralRegistrationsCleared:done=scene.retireFogInterpolationSlots270164(sd);break;
        case R::fogInterpolationSlotsReleased:done=scene.finishSceneRetirement2701CC(sd);break;
        default:return fail(d,Status::phase,0x1FA1B4);
        }
        if(!done)return fail(d,Status::scenePending,sd.sourcePC?sd.sourcePC:0x1FA1B4);
    }
    if(scene.sceneLive_||scene.groupLive_||scene.podScene_.reference00||scene.podScene_.firstChild18)
        return fail(d,Status::scenePending,0x1FA1BC);
    //1FA1BC is reached only after the actual typed Scene base/count release.
    //Other native consumers may retain tombstone backing; no guest ref is
    //kept alive or fabricated by that native storage lifetime.
    scene4C0_=nullptr;retiredSceneBacking_=std::move(sceneBacking_);pendingPC_=0x1FA1C4;
    d.sourceEffectsCommitted=true;d.sourcePC=0x1FA1BC;return true;
}
bool OriginalUiGaragePageOwner50KG::continuePayloadRetirement1FA170(Diagnostic& d)noexcept {
    d={};if(stage_!=Stage::pageRefReleased||!entityRefReleased_||entityCountReleased_||
       !grant_||!grant_->garagePageRetirementCurrent(*this))return fail(d,Status::phase,0x1FA170);
    if(scene4C0_||sceneBacking_){
        pendingPC_=0x1FA1B4;if(!retireScene1FA1B4(d))return false;
    }
    if(payloadRetirementStep_==0){pendingPC_=0x1FA1C4;if(!retireQuad2DEFE0(128,d))return false;++payloadRetirementStep_;}
    if(payloadRetirementStep_==1){pendingPC_=0x1FA1D0;if(!retireQuad2DEFE0(127,d))return false;++payloadRetirementStep_;}
    if(payloadRetirementStep_==2){
        pendingPC_=0x1FA1DC;if(!costText3340_.value||!costText3340_.value->retire2E1340(d.text))
            return fail(d,Status::rendererRetirementPending,d.text.sourcePC?d.text.sourcePC:pendingPC_);
        costText3340_.value->~Text();costText3340_.value=nullptr;--textCount_;++payloadRetirementStep_;
    }
    if(payloadRetirementStep_==3){
        while(retiredNotches_<120){pendingPC_=0x1FA1F4;if(!retireQuad2DEFE0(126-retiredNotches_,d))return false;++retiredNotches_;}
        ++payloadRetirementStep_;
    }
    if(payloadRetirementStep_==4){pendingPC_=0x1FA200;if(!retireQuad2DEFE0(6,d))return false;++payloadRetirementStep_;}
    if(payloadRetirementStep_==5){
        while(retiredTabs_<2){pendingPC_=0x1FA218;if(!retireQuad2DEFE0(5-retiredTabs_,d))return false;++retiredTabs_;}
        ++payloadRetirementStep_;
    }
    if(payloadRetirementStep_==6){
        vtable_=0x3D16C0; //1FA230 BEFORE the shared-base payload.
        while(retiredApertures_<2){pendingPC_=0x1FA248;if(!retireAperture2E1980(retiredApertures_,d))return false;}
        ++payloadRetirementStep_;
    }
    if(payloadRetirementStep_==7){
        pendingPC_=0x1FA264;if(!sharedText440_.value||!sharedText440_.value->retire2E1340(d.text))
            return fail(d,Status::rendererRetirementPending,d.text.sourcePC?d.text.sourcePC:pendingPC_);
        sharedText440_.value->~Text();sharedText440_.value=nullptr;--textCount_;++payloadRetirementStep_;
    }
    if(payloadRetirementStep_==8){
        while(retiredPrompts_<4){pendingPC_=0x1FA27C;if(!retireQuad2DEFE0(3-retiredPrompts_,d))return false;++retiredPrompts_;}
        ++payloadRetirementStep_;
    }
    pendingPC_=0x2A5CF0;return continueBaseRetirement2A5CF0(d);
}
bool OriginalUiGaragePageOwner50KG::retireRouter2A7010(Diagnostic& d)noexcept {
    if(!router_||!currentConstructor()||!router_->constructed)
        return fail(d,Status::phase,0x2A7010);
    auto& r=*router_;
    if(!r.retirementStarted){
        if(entity_.firstChild18!=&r.entity||r.entity.parent1C!=&entity_||
           r.entity.previous10||r.entity.next14||r.entity.firstChild18||!r.attached||!r.table38)
            return fail(d,Status::childRetirementPending,0x1425B8);
        for(std::size_t i=0;i<8;++i)if(!r.headKnown[i]||!r.heads[i]||r.table38[i]!=r.heads[i].get())
            return fail(d,Status::phase,0x2A7050);
        d.scope=grant_->beginGaragePageRouterRetirement142590(*this,r.entity);
        if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
        r.retirementStarted=true;r.attached=false;r.sourceVtable=0x3D3D00;
    }
    //Only this final actual Page child is handled here. Other scene-list or
    //Camera children must reach their own concrete callbacks first.
    while(r.retiredHeads<8){
        auto& head=r.heads[r.retiredHeads];if(!head)return fail(d,Status::phase,0x2A7058);
        //Native ownership moves model source next8 traversal, not stores into
        //the source head. Nodes never dereference/release their raw Widget0.
        while(head->first0){auto node=std::move(head->first0);head->first0=std::move(node->next8);node.reset();}
        head.reset();++r.retiredHeads; //2A708C, ports ascending.
    }
    if(r.table38)r.table38.reset(); //2A70A4 free,2A70B8 explicit source38=null.
    d.scope=grant_->finishGaragePageRouterRetirement1413E0(*this,r.entity);
    if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    r.constructed=false;router_.reset(); //2A70CC. Source Page134 is not cleared.
    return true;
}
bool OriginalUiGaragePageOwner50KG::continueBaseRetirement2A5CF0(Diagnostic& d)noexcept {
    if(stage_!=Stage::pageRefReleased||payloadRetirementStep_!=9||quadCount_||textCount_||
       router_||scene4C0_||sceneBacking_||!grant_||!grant_->garagePageRetirementCurrent(*this))
        return fail(d,Status::phase,0x2A5CF0);
    if(failedSharedRetirement_==FailedSharedRetirement::payloadRetiring)
        return finishFailedSharedBase2A5CF0(d);
    if(!baseRetirementStep_){
        vtable_=0x3D3C60; //2A5D1C.
        if(base_.backgroundTexture154)return fail(d,Status::childRetirementPending,0x2A5D34);
        if(!collectionsLoaded_||!loaderDatClosed1573B0_||base_.controlCount140!=7||base_.groupCount148!=2||
           !controlTable_||base_.controls144!=controlTable_.get()||!groupTable_||base_.groups14C!=groupTable_.get()||
           !extraTable_||base_.extra150!=extraTable_.get())return fail(d,Status::phase,0x2A5D48);
        baseRetirementStep_=1;
    }
    if(baseRetirementStep_==1){
        while(retiredControls_<8){
            const auto i=retiredControls_;auto& owned=controls_[i];
            if(!controlSlotsKnown_[i]||!owned||controlTable_[i]!=owned.get()||!owned->constructed)
                return fail(d,Status::phase,0x2A5D48);
            auto& c=*owned;
            if(!c.retirementStarted){
                d.scope=grant_->beginGaragePageControlRetirement142590(*this,i,c.entity);
                if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
                c.retirementStarted=true;c.sourceVtable=0x3D0060; //1D5D74.
            }
            if(!c.payloadRetired){c.sourceVtable=0x3D3B60;c.widgets38.reset();c.payloadRetired=true;} //2A58D4, no Widget refs.
            d.scope=grant_->finishGaragePageControlRetirement1413E0(*this,i,c.entity);
            if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
            c.constructed=false;owned.reset(); //1D5D90 only AFTER source count release.
            controlTable_[i]=nullptr; //2A5D80 is a real source store.
            --controlConstructionCount_;++retiredControls_;
        }
        controlTable_.reset(); //2A5DA4. Source Page144 remains stale, not nulled.
        baseRetirementStep_=2;
    }
    if(baseRetirementStep_==2){
        while(retiredGroups_<3){
            const auto i=retiredGroups_;auto& owned=groups_[i];
            if(!groupSlotsKnown_[i]||!owned||groupTable_[i]!=owned.get()||!owned->constructed)
                return fail(d,Status::phase,0x2A5DB8);
            auto& g=*owned;
            if(!g.retirementStarted){
                d.scope=grant_->beginGaragePageGroupRetirement142590(*this,i,g.entity);
                if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
                g.retirementStarted=true;g.sourceVtable=0x3D3BE0;
            }
            if(!g.payloadRetired){
                //These are raw Control borrows. The already-destroyed Control
                //objects are never dereferenced or reference-released here.
                g.controls38.reset();g.associated48.reset();g.payloadRetired=true; //2A4C04/0C,1C/24.
            }
            d.scope=grant_->finishGaragePageGroupRetirement1413E0(*this,i,g.entity);
            if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
            g.constructed=false;owned.reset(); //2A4C44. Source table slot remains stale.
            --groupConstructionCount_;++retiredGroups_;
        }
        groupTable_.reset(); //2A5E04. No source14C=null store.
        baseRetirementStep_=3;
    }
    if(baseRetirementStep_==3){
        while(retiredExtraHeads_<2){
            const auto i=retiredExtraHeads_;
            if(!extraTable_||extraTable_[i]!=extraHeads_[i].get()||!extraHeads_[i])return fail(d,Status::phase,0x2A5E1C);
            extraHeads_[i].reset();++retiredExtraHeads_; //2A5E28; table entries are not cleared.
        }
        extraTable_.reset();baseRetirementStep_=4; //2A5E4C; source150 remains stale.
    }
    //The NORMAL source route attaches Camera at1FB620, so its real callback
    //must already have run in the Page forest. A detached constructor-only
    //Camera does not establish that fact. Do not destroy it late or null the
    //Display's selectedCamera0 merely to make Page cleanup succeed.
    if(sharedCamera_&&(sharedCamera_->phase_!=game::OriginalFrontendCameraCold50LA::Phase::released||
       sharedCamera_->enrolled_||sharedCamera_->entity_.reference00))
        return fail(d,Status::childRetirementPending,0x1FB620);
    d.scope=grant_->finishGaragePageRetirement1413E0(*this,entity_);
    if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    entityCountReleased_=true;entityLive_=false;stage_=Stage::pagePayloadRetired;pendingPC_=0;
    //Only native lifetime borrows are dropped after the actual final count.
    //The Display retains its own selected-Camera identity, even if retired.
    sharedCamera_.reset();managerSearch_.reset();sharedText32C_.reset();sharedDraw_.reset();sharedMetadata_.reset();
    if(sharedCameraDisplayPinned_){sharedCameraDisplay_->unpin();sharedCameraDisplayPinned_=false;}
    sharedCameraDisplay_.reset();
    if(failedSceneRetirement_==FailedSceneRetirement::payloadRetiring){
        if(!grant_->failedPartialGaragePageRetired1413E0(*this))std::terminate();
        failedSceneRetirement_=FailedSceneRetirement::returned;
    }
    if(failedWattoBindingRetirement_==FailedWattoBindingRetirement::payloadRetiring){
        if(!grant_->failedWattoBindingGaragePageRetired1413E0(*this))std::terminate();
        failedWattoBindingRetirement_=FailedWattoBindingRetirement::returned;
    }
    if(failedLoadedHideRetirement_==FailedLoadedHideRetirement::payloadRetiring){
        if(!grant_->failedLoadedHideGaragePageRetired1413E0(*this))std::terminate();
        failedLoadedHideRetirement_=FailedLoadedHideRetirement::returned;
    }
    if(failedLateBorrowRetirement_==FailedLateBorrowRetirement::payloadRetiring){
        if(!grant_->failedLateBorrowGaragePageRetired1413E0(*this))std::terminate();
        failedLateBorrowRetirement_=FailedLateBorrowRetirement::returned;
    }
    d.sourcePC=0x1FA2AC;d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGaragePageOwner50KG::failedPartialCameraRetiredCurrent()const noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    return partialSceneAttachmentFailureCurrent()&&sharedCamera_&&sharedCameraDisplay_&&
        sharedCameraDisplayPinned_&&managerSearch_&&managerSearch_->current()&&
        !managerSearch_->camera_&&!managerSearch_->cameraObserver_&&
        sharedCamera_->failedLoaderRetirementStep_==6&&sharedCamera_->phase_==Camera::Phase::released&&
        !sharedCamera_->enrolled_&&!sharedCamera_->entity_.reference00&&
        !sharedCamera_->entity_.firstChild18&&sharedCamera_->fields_.parent38==&managerSearch_->node_;
}
bool OriginalUiGaragePageOwner50KG::unboundWattoFailureOperandsCurrent()const noexcept {
    if(!watto33B0_||!widgets_||!widgets_->ownsWattoRow(watto33B0_)||
       !entryWattoModel_||!entryWattoZone_||entryWattoObserver_)return false;
    OriginalUiGarageWattoOwner50KG::VisibilityBinding binding;
    return watto33B0_->visibilityBinding(binding)&&!binding.visibility&&!binding.primary4C0&&
        binding.model.get()==entryWattoModel_&&binding.model->scene().model()&&
        !binding.model->scene().model()->observer19C&&binding.widget==widgets_->rows_[9].wattoEntity;
}
void OriginalUiGaragePageOwner50KG::retainWattoBindingFailure1D4800(
    const assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
    // Native failure provenance only, after the actual readonly rejection.
    // A later failure after retaining the binding/Observer cannot mint this.
    if(failedWattoBinding1D4800_||d.status!=assets::OriginalStandardMaterialStatus50LA::input||
       d.sourcePC!=0x1D4800||d.sourceEffectsCommitted||
       entryPhase_!=EntryPhase::wattoBindingPending||activationPhase_!=ActivationPhase::returned||
       !grant_||!grant_->current()||!managerAttached1D1C40_||!sourceLoadReturned1FAAC8()||
       !entryProfileCurrent()||entryAppearanceStep_!=18||entryStatCount_!=6||
       entryWattoActivationStarted_||entryGroupStored_||entryControlStored_||entryGroupPass_||
       entryGroupControl_||entryControlWidget_||entrySelectionStep_||entryExplicitWidget_||entryCameraStep_||
       !unboundWattoFailureOperandsCurrent())return;
    failedWattoBindingGeneration_=grant_->generation_;failedWattoBindingEpoch_=grant_->epoch_;
    failedWattoBinding1D4800_=true;
}
bool OriginalUiGaragePageOwner50KG::wattoBindingFailureCurrent()const noexcept {
    return wattoBindingFailureOwnerCurrent()&&!entityRefReleased_&&stage_==Stage::presentationLoaded&&
        currentConstructor();
}
bool OriginalUiGaragePageOwner50KG::wattoBindingFailureOwnerCurrent()const noexcept {
    // Actual pre-bind entry failure history is independent of mutable child payloads.
    // The live-ref1 sibling predicate remains unchanged and cannot authorize this tail.
    if(!failedWattoBinding1D4800_||failedLoadedPageHide1415A0_||failedSceneAttachment1B4A38_||failedLateBorrow1B4D24_||ownerThread_!=std::this_thread::get_id()||
       !issuer_||!grant_||failedWattoBindingGeneration_!=grant_->generation_||
       failedWattoBindingEpoch_!=grant_->epoch_||grant_->issuer()!=issuer_||
       !sameOwner(authority_,grant_->application())||!sameOwner(authority_,issuer_->entities())||
       !managerAttached1D1C40_||!scenePageBorrowCaptured_||scenePageAttachCursor_!=10||cameraHandoffPhase_!=3||
       !sceneLateBorrowCaptured_||sceneLateAttachCursor_!=3||
       activationPhase_!=ActivationPhase::returned||entryPhase_!=EntryPhase::wattoBindingFailed||
       entryWattoFailure_.status!=assets::OriginalStandardMaterialStatus50LA::input||
       entryWattoFailure_.sourcePC!=0x1D4800||entryWattoFailure_.sourceEffectsCommitted||
       !entryProfile_||entryAppearanceStep_!=18||entryStatCount_!=6||entryWattoActivationStarted_||
       entryGroupStored_||entryControlStored_||entryGroupPass_||entryGroupControl_||entryControlWidget_||
       entrySelectionStep_||entryExplicitWidget_||entryCameraStep_||!unboundWattoFailureOperandsCurrent()||
       loadReturnPhase_!=LoadReturnPhase::returned||updatePublication_||drawPublication_||inputSerial_||
       !entityLive_||entityCountReleased_)return false;
    for(const auto* child:sceneLateChildren_)if(!child)return false;
    const auto* scene=scene4C0_?scene4C0_:retiredSceneBacking_.get();
    if(!scene)return false;
    if(scene4C0_){
        if(sceneBacking_.get()!=scene4C0_||retiredSceneBacking_)return false;
    }else if(sceneBacking_||!sceneReferenceReleased_||
             failedWattoBindingRetirement_!=FailedWattoBindingRetirement::payloadRetiring||
             scene->failedWattoBindingRetirementOwner_!=this||scene->failedPageRetirementOwner_||scene->failedLateBorrowRetirementOwner_||scene->failedLoadedHideRetirementOwner_||scene->sceneLive_||
             scene->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete)return false;
    const auto& owner=issuer_->garageCustodyOwner_;
    if(!owner||owner->page_.get()!=this||owner->podScene_!=scene||!owner->failed_||
       owner->entryManagerGeneration_!=failedWattoBindingGeneration_||owner->grant_.get()!=grant_.get())return false;
    return (!entityRefReleased_&&stage_==Stage::presentationLoaded&&grant_->garageAttachedPageCurrent(*this,entity_))||
        (entityRefReleased_&&stage_==Stage::pageRefReleased&&grant_->garagePageRetirementCurrent(*this));
}
bool OriginalUiGaragePageOwner50KG::preflightFailedWattoBinding1D4800(Diagnostic& d)const noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    d={};
    if(!wattoBindingFailureCurrent()||failedWattoBindingRetirement_!=FailedWattoBindingRetirement::notStarted||
       !scene4C0_||!scene4C0_->sourceLoadReturned1FB59C()||sceneLateRetiredCount_||
       sceneInitialRetiredCount_||cameraRetirementStep_||entityRefReleased_||
       payloadRetirementStep_||baseRetirementStep_||!sharedCamera_||
       sharedCamera_->phase_!=Camera::Phase::pageAttached1FB620||
       !sharedCamera_->nativeRegistrationIdentityCurrent()||
       !sharedCameraDisplay_||!sharedCameraDisplayPinned_||
       !sharedCameraDisplay_->frontendOwnerCurrent50KG()||
       !scene4C0_->visibility_||
       !scene4C0_->visibility_->pageCameraRegistrationCurrent(*this,*sharedCamera_))
        return fail(d,Status::phase,0x1D4800);
    const auto& entry=issuer_->garageCustodyOwner_;
    if(!entry||!scene4C0_->canRetireUnresolvedSoundGroupForWattoBindingFailure(*entry,false))
        return fail(d,Status::scenePending,0x235150);
    d.sourcePC=0x1D4800;return true;
}
bool OriginalUiGaragePageOwner50KG::retireFailedWattoBinding1D4800(Diagnostic& d)noexcept {
    using P=FailedWattoBindingRetirement;d={};
    if(failedWattoBindingRetirement_==P::returned){
        if(!grant_||!grant_->failedWattoBindingGaragePageRetired1413E0(*this)||!entityCountReleased_||
           entityLive_||stage_!=Stage::pagePayloadRetired||!retiredSceneBacking_||
           retiredSceneBacking_->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
           retiredSceneBacking_->failedWattoBindingRetirementOwner_!=this)
            return fail(d,Status::phase,0x1FA2AC);
        d.sourcePC=0x1FA2AC;d.sourceEffectsCommitted=true;return true;
    }
    if(!wattoBindingFailureOwnerCurrent())return fail(d,Status::phase,0x1D4800);
    if(failedWattoBindingRetirement_==P::payloadRetiring)return continuePayloadRetirement1FA170(d);
    if(failedWattoBindingRetirement_==P::notStarted){
        if(!preflightFailedWattoBinding1D4800(d))return false;
        failedWattoBindingRetirement_=P::childrenRetiring;
    }
    if(failedWattoBindingRetirement_==P::childrenRetiring){
        if(!retireSceneChildren142590(d))return false;
        if(sceneLateRetiredCount_!=3||sceneInitialRetiredCount_!=10||cameraRetirementStep_!=6)
            return fail(d,Status::phase,0x1413E0);
        failedWattoBindingRetirement_=P::routerRetiring;
    }
    if(failedWattoBindingRetirement_==P::routerRetiring){
        if(!retireRouter2A7010(d))return false;
        const auto step=grant_->beginGarageWattoBindingPageRetirement142590(*this,*scene4C0_,entity_);
        if(!step.okay())return fail(d,Status::childRetirementPending,step.sourcePC);
        entityRefReleased_=true;stage_=Stage::pageRefReleased;pendingPC_=0x1FA1B4;
        failedWattoBindingRetirement_=P::payloadRetiring;
    }
    return continuePayloadRetirement1FA170(d);
}
bool OriginalUiGaragePageOwner50KG::loadedPageHideFailureCurrent()const noexcept {
    return loadedHideFailureOwnerCurrent()&&!entityRefReleased_&&stage_==Stage::presentationLoaded&&
        currentConstructor();
}
bool OriginalUiGaragePageOwner50KG::loadedHideFailureOwnerCurrent()const noexcept {
    // The actual failed hide preflight receipt is independent of mutable child payloads.
    // The live-ref1 sibling predicate remains unchanged and cannot authorize this tail.
    if(!failedLoadedPageHide1415A0_||failedSceneAttachment1B4A38_||failedLateBorrow1B4D24_||ownerThread_!=std::this_thread::get_id()||
       !issuer_||!grant_||failedLoadedPageHideGeneration_!=grant_->generation_||
       failedLoadedPageHideEpoch_!=grant_->epoch_||grant_->issuer()!=issuer_||
       !sameOwner(authority_,grant_->application())||!sameOwner(authority_,issuer_->entities())||
       !managerAttached1D1C40_||!scenePageBorrowCaptured_||scenePageAttachCursor_!=10||cameraHandoffPhase_!=3||
       !sceneLateBorrowCaptured_||sceneLateAttachCursor_!=3||
       activationPhase_!=ActivationPhase::notActivated||entryPhase_!=EntryPhase::notEntered||
       loadReturnPhase_!=LoadReturnPhase::returned||updatePublication_||drawPublication_||inputSerial_||
       !entityLive_||entityCountReleased_)return false;
    for(const auto* child:sceneLateChildren_)if(!child)return false;
    const auto* scene=scene4C0_?scene4C0_:retiredSceneBacking_.get();
    if(!scene)return false;
    if(scene4C0_){
        if(sceneBacking_.get()!=scene4C0_||retiredSceneBacking_)return false;
    }else if(sceneBacking_||!sceneReferenceReleased_||
             failedLoadedHideRetirement_!=FailedLoadedHideRetirement::payloadRetiring||
             scene->failedLoadedHideRetirementOwner_!=this||scene->failedPageRetirementOwner_||scene->failedLateBorrowRetirementOwner_||scene->sceneLive_||
             scene->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete)return false;
    const auto& owner=issuer_->garageCustodyOwner_;
    if(!owner||owner->page_.get()!=this||owner->podScene_!=scene||!owner->failed_||
       owner->entryManagerGeneration_!=failedLoadedPageHideGeneration_||owner->grant_.get()!=grant_.get())return false;
    return (!entityRefReleased_&&stage_==Stage::presentationLoaded&&grant_->garageAttachedPageCurrent(*this,entity_))||
        (entityRefReleased_&&stage_==Stage::pageRefReleased&&grant_->garagePageRetirementCurrent(*this));
}
bool OriginalUiGaragePageOwner50KG::lateBorrowFailureOwnerCurrent()const noexcept {
    // The actual failed1B4D24 receipt is independent of mutable Camera/child payloads.
    // The live-ref1 sibling predicate remains unchanged and cannot authorize this tail.
    if(!failedLateBorrow1B4D24_||failedSceneAttachment1B4A38_||ownerThread_!=std::this_thread::get_id()||
       !issuer_||!grant_||failedLateBorrowGeneration_!=grant_->generation_||
       failedLateBorrowEpoch_!=grant_->epoch_||grant_->issuer()!=issuer_||
       !sameOwner(authority_,grant_->application())||!sameOwner(authority_,issuer_->entities())||
       managerAttached1D1C40_||!scenePageBorrowCaptured_||scenePageAttachCursor_!=10||cameraHandoffPhase_!=3||
       sceneLateBorrowCaptured_||sceneLateAttachCursor_||sceneLateRetiredCount_||
       activationPhase_!=ActivationPhase::notActivated||entryPhase_!=EntryPhase::notEntered||
       loadReturnPhase_!=LoadReturnPhase::forestPending||updatePublication_||drawPublication_||inputSerial_||
       !entityLive_||entityCountReleased_)return false;
    for(const auto* child:sceneLateChildren_)if(child)return false;
    const auto* scene=scene4C0_?scene4C0_:retiredSceneBacking_.get();
    if(!scene)return false;
    if(scene4C0_){
        if(sceneBacking_.get()!=scene4C0_||retiredSceneBacking_)return false;
    }else if(sceneBacking_||!sceneReferenceReleased_||
             failedLateBorrowRetirement_!=FailedLateBorrowRetirement::payloadRetiring||
             scene->failedLateBorrowRetirementOwner_!=this||scene->failedPageRetirementOwner_||scene->sceneLive_||
             scene->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete)return false;
    const auto& owner=issuer_->garageCustodyOwner_;
    if(!owner||owner->page_.get()!=this||owner->podScene_!=scene||!owner->failed_||
       owner->entryManagerGeneration_!=failedLateBorrowGeneration_||owner->grant_.get()!=grant_.get())return false;
    return (!entityRefReleased_&&stage_==Stage::presentationLoaded&&grant_->garagePageCurrent(this,entity_))||
        (entityRefReleased_&&stage_==Stage::pageRefReleased&&grant_->garagePageRetirementCurrent(*this));
}
bool OriginalUiGaragePageOwner50KG::preflightFailedLateBorrow1B4D24(Diagnostic& d)const noexcept {
    d={};
    if(!lateBorrowFailureOwnerCurrent()||failedLateBorrowRetirement_!=FailedLateBorrowRetirement::notStarted||
       !lateSceneBorrowFailureCurrent()||!currentConstructor()||!scene4C0_||
       !scene4C0_->sourceLoadReturned1FB59C()||sceneInitialRetiredCount_||cameraRetirementStep_||
       entityRefReleased_||payloadRetirementStep_||baseRetirementStep_)
        return fail(d,Status::phase,0x1B4D24);
    if(!preflightFailedLateBorrowCamera231310(d))return false;
    const auto& entry=issuer_->garageCustodyOwner_;
    if(!entry||!scene4C0_->canRetireUnresolvedSoundGroupForLateBorrowFailure(*entry,false))
        return fail(d,Status::scenePending,0x235150);
    d.sourcePC=0x1B4D24;return true;
}
bool OriginalUiGaragePageOwner50KG::preflightFailedLoadedHide1415A0(Diagnostic& d)const noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    d={};
    if(!loadedPageHideFailureCurrent()||failedLoadedHideRetirement_!=FailedLoadedHideRetirement::notStarted||
       !scene4C0_||!scene4C0_->sourceLoadReturned1FB59C()||sceneLateRetiredCount_||
       sceneInitialRetiredCount_||cameraRetirementStep_||entityRefReleased_||
       payloadRetirementStep_||baseRetirementStep_||!sharedCamera_||
       sharedCamera_->phase_!=Camera::Phase::pageAttached1FB620||
       !sharedCamera_->nativeRegistrationIdentityCurrent()||
       !sharedCameraDisplay_||!sharedCameraDisplayPinned_||
       !sharedCameraDisplay_->frontendOwnerCurrent50KG()||
       !scene4C0_->visibility_||
       !scene4C0_->visibility_->pageCameraRegistrationCurrent(*this,*sharedCamera_))
        return fail(d,Status::phase,0x1415A0);
    const auto& entry=issuer_->garageCustodyOwner_;
    if(!entry||!scene4C0_->canRetireUnresolvedSoundGroupForLoadedPageFailure(*entry,false))
        return fail(d,Status::scenePending,0x235150);
    d.sourcePC=0x1415A0;return true;
}
bool OriginalUiGaragePageOwner50KG::retireFailedLoadedHide1415A0(Diagnostic& d)noexcept {
    using P=FailedLoadedHideRetirement;d={};
    if(failedLoadedHideRetirement_==P::returned){
        if(!grant_||!grant_->failedLoadedHideGaragePageRetired1413E0(*this)||!entityCountReleased_||
           entityLive_||stage_!=Stage::pagePayloadRetired||!retiredSceneBacking_||
           retiredSceneBacking_->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
           retiredSceneBacking_->failedLoadedHideRetirementOwner_!=this)
            return fail(d,Status::phase,0x1FA2AC);
        d.sourcePC=0x1FA2AC;d.sourceEffectsCommitted=true;return true;
    }
    if(!loadedHideFailureOwnerCurrent())return fail(d,Status::phase,0x1415A0);
    if(failedLoadedHideRetirement_==P::payloadRetiring)return continuePayloadRetirement1FA170(d);
    if(failedLoadedHideRetirement_==P::notStarted){
        if(!preflightFailedLoadedHide1415A0(d))return false;
        failedLoadedHideRetirement_=P::childrenRetiring;
    }
    if(failedLoadedHideRetirement_==P::childrenRetiring){
        if(!retireSceneChildren142590(d))return false;
        if(sceneLateRetiredCount_!=3||sceneInitialRetiredCount_!=10||cameraRetirementStep_!=6)
            return fail(d,Status::phase,0x1413E0);
        failedLoadedHideRetirement_=P::routerRetiring;
    }
    if(failedLoadedHideRetirement_==P::routerRetiring){
        if(!retireRouter2A7010(d))return false;
        const auto step=grant_->beginGarageLoadedHidePageRetirement142590(*this,*scene4C0_,entity_);
        if(!step.okay())return fail(d,Status::childRetirementPending,step.sourcePC);
        entityRefReleased_=true;stage_=Stage::pageRefReleased;pendingPC_=0x1FA1B4;
        failedLoadedHideRetirement_=P::payloadRetiring;
    }
    return continuePayloadRetirement1FA170(d);
}
bool OriginalUiGaragePageOwner50KG::retireFailedLateBorrow1B4D24(Diagnostic& d)noexcept {
    using P=FailedLateBorrowRetirement;d={};
    if(failedLateBorrowRetirement_==P::returned){
        if(!grant_||!grant_->failedLateBorrowGaragePageRetired1413E0(*this)||!entityCountReleased_||
           entityLive_||stage_!=Stage::pagePayloadRetired||!retiredSceneBacking_||
           retiredSceneBacking_->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
           retiredSceneBacking_->failedLateBorrowRetirementOwner_!=this)
            return fail(d,Status::phase,0x1FA2AC);
        d.sourcePC=0x1FA2AC;d.sourceEffectsCommitted=true;return true;
    }
    if(!lateBorrowFailureOwnerCurrent())return fail(d,Status::phase,0x1B4D24);
    if(failedLateBorrowRetirement_==P::payloadRetiring)return continuePayloadRetirement1FA170(d);
    if(failedLateBorrowRetirement_==P::notStarted){
        if(!preflightFailedLateBorrow1B4D24(d))return false;
        failedLateBorrowRetirement_=P::initialRetiring;
    }
    if(failedLateBorrowRetirement_==P::initialRetiring){
        if(!retireFailedLateBorrowInitialChildren142590(d))return false;
        if(sceneInitialRetiredCount_!=10||cameraRetirementStep_!=6)
            return fail(d,Status::phase,0x1413E0);
        failedLateBorrowRetirement_=P::detachedAudioRetiring;
    }
    if(failedLateBorrowRetirement_==P::detachedAudioRetiring){
        if(!retireFailedLateBorrowDetachedAudio271470(d))return false;
        failedLateBorrowRetirement_=P::routerRetiring;
    }
    if(failedLateBorrowRetirement_==P::routerRetiring){
        if(!retireRouter2A7010(d))return false;
        const auto step=grant_->beginGarageLateBorrowPageRetirement142590(*this,*scene4C0_,entity_);
        if(!step.okay())return fail(d,Status::childRetirementPending,step.sourcePC);
        entityRefReleased_=true;stage_=Stage::pageRefReleased;pendingPC_=0x1FA1B4;
        failedLateBorrowRetirement_=P::payloadRetiring;
    }
    return continuePayloadRetirement1FA170(d);
}
bool OriginalUiGaragePageOwner50KG::partialSceneAttachmentFailureCurrent()const noexcept {
    // Immutable failure identity survives real raw-list/model destruction.
    // Never call current2743B0 or require their old ref1/payload state here.
    if(!failedSceneAttachment1B4A38_||ownerThread_!=std::this_thread::get_id()||!issuer_||!grant_||
       failedSceneAttachmentGeneration_!=grant_->generation_||failedSceneAttachmentEpoch_!=grant_->epoch_||
       grant_->issuer()!=issuer_||!sameOwner(authority_,grant_->application())||
       !sameOwner(authority_,issuer_->entities())||managerAttached1D1C40_||
       !scenePageBorrowCaptured_||scenePageAttachCursor_!=6||sceneLateAttachCursor_||cameraHandoffPhase_||
       activationPhase_!=ActivationPhase::notActivated||entryPhase_!=EntryPhase::notEntered||
       loadReturnPhase_!=LoadReturnPhase::forestPending||updatePublication_||drawPublication_||inputSerial_||
       !entityLive_||entityCountReleased_)return false;
    const auto* scene=scene4C0_?scene4C0_:retiredSceneBacking_.get();
    if(!scene)return false;
    if(scene4C0_){
        if(sceneBacking_.get()!=scene4C0_||retiredSceneBacking_)return false;
    }else if(sceneBacking_||!sceneReferenceReleased_||
             failedSceneRetirement_!=FailedSceneRetirement::payloadRetiring||
             scene->failedPageRetirementOwner_!=this||scene->sceneLive_||
             scene->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete)return false;
    const auto& owner=issuer_->garageCustodyOwner_;
    if(!owner||owner->page_.get()!=this||owner->podScene_!=scene||!owner->failed_||
       owner->entryManagerGeneration_!=failedSceneAttachmentGeneration_||owner->grant_.get()!=grant_.get())return false;
    return (!entityRefReleased_&&stage_==Stage::presentationLoaded&&grant_->garagePageCurrent(this,entity_))||
        (entityRefReleased_&&stage_==Stage::pageRefReleased&&grant_->garagePageRetirementCurrent(*this));
}
bool OriginalUiGaragePageOwner50KG::preflightFailedSceneAttachment(Diagnostic& d)const noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    d={};
    if(!partialSceneAttachmentFailureCurrent()||failedSceneRetirement_!=FailedSceneRetirement::notStarted||
       !currentConstructor()||!grant_->garagePartialPageFailureCurrent(*this,*scene4C0_)||
       !scene4C0_->sourceLoadReturned1FB59C()||!scene4C0_->sceneCollision_||!scene4C0_->staticModels_||
       entityRefReleased_||payloadRetirementStep_||baseRetirementStep_||cameraRetirementStep_||
       !sharedCamera_||shared_.camera4BC!=sharedCamera_.get()||!managerSearch_||!managerSearch_->current()||
       sharedCamera_->phase_!=Camera::Phase::poseSet114F90||sharedCamera_->failedLoaderRetirementStep_||
       managerSearch_->camera_!=sharedCamera_||!managerSearch_->cameraRegistrationCurrent()||
       sharedCamera_->fields_.parent38!=&managerSearch_->node_||!sharedCameraDisplayPinned_)
        return fail(d,Status::phase,0x1B4A38);
    for(std::size_t i=0;i<6;++i)if(!scenePageChildren_[i]||
        !grant_->garageScenePageChildCurrent(*this,*scene4C0_,i,*scenePageChildren_[i]))
        return fail(d,Status::phase,0x1B4A38);
    for(std::size_t i=6;i<scenePageChildren_.size();++i){const auto* c=scenePageChildren_[i];
        if(!c||c->reference00!=1||c->parent1C||c->previous10||c->next14||c->mask20)
            return fail(d,Status::phase,0x1B4A38);
    }
    assets::OriginalStandardMaterialDiagnostic50LA collision;
    if(!scene4C0_->sceneCollision_->preflightFailedRawList27BB80(*this,collision))
        return fail(d,Status::childRetirementPending,collision.sourcePC);
    if(!preflightFailedPartialCamera231310(d))return false;
    const auto& entry=issuer_->garageCustodyOwner_;
    if(!entry||!scene4C0_->canRetireUnresolvedSoundGroupForEntryFailure(*entry,false))
        return fail(d,Status::scenePending,0x235150);
    d.sourcePC=0x27BB80;return true;
}
bool OriginalUiGaragePageOwner50KG::retireFailedSceneAttachmentPrefix(Diagnostic& d)noexcept {
    using P=FailedSceneRetirement;d={};
    if(failedSceneRetirement_==P::returned){
        if(!grant_||!grant_->failedPartialGaragePageRetired1413E0(*this)||!entityCountReleased_||
           entityLive_||stage_!=Stage::pagePayloadRetired||!retiredSceneBacking_||
           retiredSceneBacking_->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete)
            return fail(d,Status::phase,0x1FA2AC);
        d.sourcePC=0x1FA2AC;d.sourceEffectsCommitted=true;return true;
    }
    if(!partialSceneAttachmentFailureCurrent())return fail(d,Status::phase,0x1B4A38);
    if(failedSceneRetirement_==P::payloadRetiring)return continuePayloadRetirement1FA170(d);
    if(failedSceneRetirement_==P::notStarted){
        if(!preflightFailedSceneAttachment(d))return false;
        failedSceneRetirement_=P::rawRetiring;
    }
    assets::OriginalStandardMaterialDiagnostic50LA child;
    if(failedSceneRetirement_==P::rawRetiring){
        if(!scene4C0_->sceneCollision_->retireFailedRawList27BB80(*this,child))
            return fail(d,Status::childRetirementPending,child.sourcePC);
        failedSceneRetirement_=P::rawRetired;
    }
    if(!scene4C0_->sceneCollision_->failedRawListRetired27BB80(*this))
        return fail(d,Status::phase,0x27BB80);
    if(failedSceneRetirement_==P::rawRetired)failedSceneRetirement_=P::staticPending;
    if(failedSceneRetirement_==P::staticPending){
        if(!scene4C0_->staticModels_->retireFailedPageStatic27B650(*this,child))
            return fail(d,Status::childRetirementPending,child.sourcePC);
        failedSceneRetirement_=P::staticRetired;failedAttachedRetiredCount_=1;
    }
    while(failedAttachedRetiredCount_<5){
        const auto ordinal=5-failedAttachedRetiredCount_;bool returned=false;
        if(ordinal>=2)returned=scene4C0_->retirePageFog13E700(*this,ordinal-2,child);
        else returned=scene4C0_->retirePageLightList2781B0(*this,child);
        if(!returned)return fail(d,Status::childRetirementPending,child.sourcePC?child.sourcePC:0x1425B8);
        ++failedAttachedRetiredCount_;
    }
    if(failedSceneRetirement_==P::staticRetired)failedSceneRetirement_=P::detachedEmitters;
    if(failedSceneRetirement_==P::detachedEmitters){
        //Native exception-only ownership unwind: this actual List was never
        //attached as late Page slot0. Retire its real children before dropping
        //their live Actor-model/pool borrows, without inventing a Page callback.
        if(!scene4C0_->retireSceneParticleList27DCD0(child))
            return fail(d,Status::childRetirementPending,child.sourcePC);
        failedSceneRetirement_=P::detachedActors;
    }
    if(failedSceneRetirement_==P::detachedActors){
        const auto& owner=issuer_->garageCustodyOwner_;
        if(!owner||owner->page_.get()!=this||!owner->actors_||
           (scene4C0_->actorOwner3C8_&&scene4C0_->actorOwner3C8_!=owner->actors_.get()))
            return fail(d,Status::phase,0x2D9C20);
        if(!owner->actors_->closeCold142590(child))return fail(d,Status::childRetirementPending,child.sourcePC);
        if(scene4C0_->actorOwner3C8_)return fail(d,Status::phase,0x2D9C20);
        failedSceneRetirement_=P::detachedParticles;
    }
    if(failedSceneRetirement_==P::detachedParticles){
        if(!scene4C0_->retireParticleManager160C70(child))
            return fail(d,Status::childRetirementPending,child.sourcePC);
        failedSceneRetirement_=P::cameraRetiring;
    }
    if(failedSceneRetirement_==P::cameraRetiring){
        //The actual Camera still belongs to manager Search; no Scene Camera
        //registration ever occurred. Use its matching failed-manager lifetime.
        if(!retireFailedPartialCamera231310(d))return false;
        failedSceneRetirement_=P::detachedOwnersRetired;
    }
    if(failedSceneRetirement_==P::detachedOwnersRetired){
        assets::OriginalStandardMaterialDiagnostic50LA child;
        //Actual descriptors are detached, never fabricated late Page slots.
        //The native failure wrapper retains an in-flight ref0/count receipt.
        if(!scene4C0_->retireFailedPageMaterialSoundList(*this,2,child)||
           !scene4C0_->retireFailedPageMaterialSoundList(*this,1,child)){
            d.loadResources=child;return fail(d,Status::childRetirementPending,child.sourcePC);
        }
        failedSceneRetirement_=P::searchRetiring;
    }
    if(failedSceneRetirement_==P::searchRetiring){
        if(!scene4C0_->visibility_->retireFailedPageSearchEntities142590(*this,d.loadResources))
            return fail(d,Status::childRetirementPending,d.loadResources.sourcePC);
        failedAttachedRetiredCount_=6;failedSceneRetirement_=P::searchRetired;
    }
    if(failedSceneRetirement_==P::searchRetired){
        if(failedAttachedRetiredCount_!=6||!scene4C0_->visibility_->failedPageSearchEntitiesRetired142590(*this))
            return fail(d,Status::childRetirementPending,0x141340);
        failedSceneRetirement_=P::routerRetiring;
    }
    if(failedSceneRetirement_==P::routerRetiring){
        if(router_&&!retireRouter2A7010(d))return false;
        if(router_||entity_.firstChild18||!failedPartialCameraRetiredCurrent())
            return fail(d,Status::childRetirementPending,0x1425B8);
        d.scope=grant_->beginGaragePageRetirement142590(this,entity_);
        if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
        entityRefReleased_=true;stage_=Stage::pageRefReleased;pendingPC_=0x1FA1B4;
        failedSceneRetirement_=P::payloadRetiring;
    }
    return continuePayloadRetirement1FA170(d);
}
bool OriginalUiGaragePageOwner50KG::sharedPromptFailurePrefixCurrent()const noexcept {
    if(!failedFirstPrompt29B638_||ownerThread_!=std::this_thread::get_id()||!issuer_||!grant_||
       failedPromptGeneration_!=grant_->generation_||failedPromptEpoch_!=grant_->epoch_||
       grant_->issuer()!=issuer_||!sameOwner(authority_,grant_->application())||
       !sameOwner(authority_,issuer_->entities())||managerAttached1D1C40_||
       scene4C0_||sceneBacking_||retiredSceneBacking_||watto33B0_||selected4C4_||
       router_||base_.router134||collectionsLoaded_||loaderDatClosed1573B0_||
       controlConstructionCount_||groupConstructionCount_||controlTable_||groupTable_||extraTable_||
       base_.controlCount140||base_.groupCount148||base_.controls144||base_.groups14C||base_.extra150||
       base_.backgroundTexture154||base_.moviePlayer158||shared_.popup438||
       sharedPromptCount_||activationPhase_!=ActivationPhase::notActivated||
       entryPhase_!=EntryPhase::notEntered||loadReturnPhase_!=LoadReturnPhase::forestPending||
       updatePublication_||drawPublication_||inputSerial_||scenePageAttachCursor_||sceneLateAttachCursor_)
        return false;
    for(const auto& owner:controls_)if(owner)return false;
    for(const auto& owner:groups_)if(owner)return false;
    for(const auto& owner:extraHeads_)if(owner)return false;
    return entityLive_&&!entityCountReleased_&&
        ((!entityRefReleased_&&stage_==Stage::sharedPromptsLoading&&
          grant_->garagePageCurrent(this,entity_))||
         (entityRefReleased_&&stage_==Stage::pageRefReleased&&
          grant_->garagePageRetirementCurrent(*this)));
}
bool OriginalUiGaragePageOwner50KG::preflightFailedSharedPrompt(Diagnostic& d)const noexcept {
    d={};
    if(!sharedPromptFailurePrefixCurrent()||failedSharedRetirement_!=FailedSharedRetirement::notStarted||
       !currentConstructor()||pendingPC_!=0x1D682C||!loaderDat_||failedLoaderDatReleased_||
       quadCount_!=129||textCount_!=2||!sharedText440_.value||!costText3340_.value||
       payloadRetirementStep_||baseRetirementStep_||retiredPrompts_||retiredApertures_||
       retiredNotches_||retiredTabs_||entity_.firstChild18||entity_.parent1C||entity_.previous10||entity_.next14)
        return fail(d,Status::phase,0x29B638);
    const auto& prompt=quads_[0];
    if(prompt.stage_!=Quad::Stage::failed||prompt.pendingPC_!=0x29B638||
       !prompt.material54||!prompt.material54->materialRetained||prompt.resource58||prompt.packetOwner_)
        return fail(d,Status::phase,0x29B638);
    for(std::size_t i=0;i<quads_.size();++i){
        const auto& q=quads_[i];
        if(i&&(q.stage_!=Quad::Stage::constructed||q.material54||q.resource58))
            return fail(d,Status::phase,0x2DEFE0);
        if(!q.preflightRetirement2DEFE0(d.quad))return fail(d,Status::rendererRetirementPending,d.quad.sourcePC);
    }
    if(!sharedText440_.value->preflightRetirement2E1340(d.text)||
       !costText3340_.value->preflightRetirement2E1340(d.text))
        return fail(d,Status::rendererRetirementPending,d.text.sourcePC);
    // Check actual three retained Primitive releases together. Equal rows do
    // not make the two independently retained Apertures a single reference.
    std::array<const Quad::Primitive*,3> primitives{{prompt.material54,nullptr,nullptr}};
    for(std::size_t i=0;i<apertures_.size();++i){
        const auto& owner=apertures_[i];
        if(!owner||!shared_.apertures424[i]||*shared_.apertures424[i]!=owner.get())
            return fail(d,Status::phase,0x2E1980);
        const auto& a=*owner;
        if(a.type0C!=1||a.count34!=1||a.count48!=1||a.capacity4C!=1||!a.primitives30||
           !a.packets44||a.count04!=4||a.count08!=4||a.owns38!=1||!a.vertices00||
           a.nativeGarageFifoPins_||a.nativeMenuWarningPins_||!a.packets44[0].allocation00||
           !Quad::preflightPrimitive246260(a.primitives30[0],d.quad))
            return fail(d,Status::rendererRetirementPending,d.quad.sourcePC?d.quad.sourcePC:0x2E1980);
        primitives[i+1]=a.primitives30[0].get();
    }
    for(const auto* primitive:primitives){
        if(!primitive||!primitive->materialRetained||!primitive->domain)return fail(d,Status::phase,0x14D500);
        std::int32_t releases=0;
        for(const auto* other:primitives)
            if(other&&other->domain.get()==primitive->domain.get()&&other->material00==primitive->material00)++releases;
        assets::OriginalStandardMaterialRowView50LA row;
        if(!primitive->domain->materials382B38()->row(primitive->material00,row)||
           !row.references08||*row.references08<releases)return fail(d,Status::rendererRetirementPending,0x14D500);
    }
    return preflightFailedSharedCamera231310(d);
}
bool OriginalUiGaragePageOwner50KG::retireFailedSharedPrompt(Diagnostic& d)noexcept {
    using P=FailedSharedRetirement;
    d={};
    if(failedSharedRetirement_==P::returned){
        if(stage_!=Stage::pagePayloadRetired||entityLive_||!entityCountReleased_)
            return fail(d,Status::phase,0x1413E0);
        d.sourcePC=0x1FA2AC;return true;
    }
    if(failedSharedRetirement_==P::notStarted){
        if(!preflightFailedSharedPrompt(d))return false;
        failedSharedRetirement_=P::cameraRetiring;
    }
    if(failedSharedRetirement_==P::cameraRetiring){
        if(!retireFailedSharedCamera231310(d))return false;
        failedSharedRetirement_=P::cameraRetired;
    }
    if(failedSharedRetirement_==P::cameraRetired){
        if(!sharedPromptFailurePrefixCurrent()||entityRefReleased_)
            return fail(d,Status::phase,0x142590);
        failedSharedRetirement_=P::payloadRetiring;
    }
    if(failedSharedRetirement_!=P::payloadRetiring)return fail(d,Status::phase,0x1FA170);
    // Existing Page last-reference and embedded payload order, including every
    // real renderer drain. Its normal loaded-collection tail is not fabricated.
    return beginRetirement142590(d);
}
bool OriginalUiGaragePageOwner50KG::finishFailedSharedBase2A5CF0(Diagnostic& d)noexcept {
    if(!sharedPromptFailurePrefixCurrent()||failedSharedRetirement_!=FailedSharedRetirement::payloadRetiring||
       stage_!=Stage::pageRefReleased||payloadRetirementStep_!=9||quadCount_||textCount_||
       sharedText440_.value||costText3340_.value||retiredApertures_!=2||retiredPrompts_!=4||
       retiredNotches_!=120||retiredTabs_!=2||apertures_[0]||apertures_[1]||
       !sharedCamera_||sharedCamera_->phase_!=game::OriginalFrontendCameraCold50LA::Phase::released||
       sharedCamera_->enrolled_||sharedCamera_->entity_.reference00||
       (failedLoaderDatReleased_?bool(loaderDat_):!loaderDat_))return fail(d,Status::phase,0x2A5CF0);
    // Actual constructor null/count-zero branches. No Controls, Groups, extra
    // heads or background resource ever existed, and no loaded receipt is set.
    vtable_=0x3D3C60;
    if(!failedLoaderDatReleased_){
        loaderDat_.reset();failedLoaderDatReleased_=true;
        // Native parser custody of an interrupted call, NOT source1573B0 return.
    }
    d.scope=grant_->finishGaragePageRetirement1413E0(*this,entity_);
    if(!d.scope.okay())return fail(d,Status::scope,d.scope.sourcePC);
    entityCountReleased_=true;entityLive_=false;stage_=Stage::pagePayloadRetired;pendingPC_=0;
    sharedCamera_.reset();managerSearch_.reset();sharedText32C_.reset();sharedDraw_.reset();sharedMetadata_.reset();
    if(sharedCameraDisplayPinned_){sharedCameraDisplay_->unpin();sharedCameraDisplayPinned_=false;}
    sharedCameraDisplay_.reset();failedSharedRetirement_=FailedSharedRetirement::returned;
    d.sourcePC=0x1FA2AC;d.sourceEffectsCommitted=true;return true;
}
OriginalUiGaragePageOwner50KG::Observation OriginalUiGaragePageOwner50KG::inspect()const noexcept {
    Observation o;o.stage=stage_;
    o.activationPhase=activationPhase_;o.activationGroup=activationGroup_;o.activationPass=activationPass_;
    o.activationControl=activationControl_;o.activationWidget=activationWidget_;
    o.sourceActivationReturned=sourceActivationReturned2A680C();
    o.controlEntities=controlConstructionCount_;o.groupEntities=groupConstructionCount_;
    o.loaderDatClosed=loaderDatClosed1573B0_;o.sharedLoaderReturned=sharedCollectionsPayloadCurrent();
    if(router_){o.eventRouter134=router_->constructed?&router_->entity:nullptr;o.routerMembershipKnown=router_->membershipKnown;
        for(const auto& head:router_->heads)if(head)++o.routerHeads;}
o.entity=entityLive_?&entity_:nullptr;
    o.managerBorrowsKnown=base_.manager130.has_value()&&shared_.manager270.has_value();
    if(base_.manager130)o.manager130=*base_.manager130;if(shared_.manager270)o.manager270=*shared_.manager270;
    o.scene4C0=scene4C0_;o.constructedQuads=quadCount_;o.constructedTexts=textCount_;
    o.sourceVtable=vtable_;o.pendingPC=pendingPC_;o.constructorReturned=quadCount_==129&&textCount_==2;
    o.loadReturnPhase=loadReturnPhase_;o.application344Known=loadApplication344_.has_value();
    if(loadApplication344_)o.application344=*loadApplication344_;
    o.selected4C4=selected4C4_;o.selectedObserverAttached=loadSelectedObserver_&&selectedRegistrationCurrent1FB67C();
    o.entryPhase=entryPhase_;o.entryAppearanceStep=entryAppearanceStep_;o.entryStatCount=entryStatCount_;
    o.entryGroupPass=entryGroupPass_;o.entryGroupControl=entryGroupControl_;o.entryControlWidget=entryControlWidget_;
    o.entrySelectionStep=entrySelectionStep_;o.entryExplicitWidget=entryExplicitWidget_;o.entryCameraStep=entryCameraStep_;
    o.sourceEntryReturned=sourceEntryReturned1FAE00();
    o.entryProfileKnown=entryProfile_.has_value();if(entryProfile_)o.entryCareer=entryProfile_->career100C0;
    o.information4DC=information4DC_;o.field4E0=entryField4E0_;o.current4E8=current4E8_;
    o.delta500=delta500_;o.available34B0=available34B0_;
    o.wattoObserverAttached=watto33B0_&&wattoEntryBindingCurrent1FAC08(*watto33B0_);
    o.sourceLoadReturned=sourceLoadReturned1FAAC8();o.managerAttached=managerAttached1D1C40_;
    o.aperturePointersKnown=shared_.apertures424[0].has_value()&&shared_.apertures424[1].has_value();
    o.sceneReferenceReleased=sceneReferenceReleased_;o.pageReferenceReleased=entityRefReleased_;
    o.pageEntityCountReleased=entityCountReleased_;o.openSize274=shared_.openSize274;
    o.left278=shared_.left278;o.right280=shared_.right280;o.top27C=shared_.top27C;
    o.left288=shared_.left288;o.right290=shared_.right290;o.bottom294=shared_.bottom294;
    o.input268=static_cast<std::int32_t>(base_.input268);o.savedDelta298=shared_.savedDelta298;
    o.time4B0=shared_.time4B0;o.panelOffset4D8=panelOffset4D8_;
    o.updatePublication=updatePublication_;o.updateSourcePC=updateSourcePC_;
    o.updatedPageChildren=updatedPageChildren_;o.updatedLights=updatedLights_;
    o.updateFailed=updateFailed_;o.updateReturned=updateReturned_;
    o.pairFirst=pairFirst_;o.pairSecond=pairSecond_;o.timer34B4=timer34B4_;o.enabled34C8=enabled34C8_;return o;
}
bool OriginalUiGaragePageOwner50KG::inspectQuad(std::size_t index,QuadObservation& out)const noexcept {
    if(index>=quadCount_||!quads_[index].constructed)return false;const auto& q=quads_[index];
    QuadObservation o;o.objectOffset=q.offset;o.position10=q.position10;o.size20=q.size20;o.uv30=q.uv30;
    o.field40=q.field40;o.scale4C=q.scale4C;o.scale50=q.scale50;o.color44=q.color44;o.alpha48=q.alpha48;
    o.flags5CValue=q.flags5CValue;o.flags5CKnownMask=q.flags5CKnownMask;
    o.material54Null=q.material54==nullptr;o.resource58Null=q.resource58==nullptr;out=o;return true;
}
}

namespace rr::game {
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::GarageConstructionGrant::attachGaragePageCamera1FB620(
    PageOwner& page,OriginalFrontendCameraCold50LA& camera)noexcept {
    using F=OriginalFrontendCameraCold50LA;
    if(!current()||page.grant_.get()!=this||page.sharedCamera_.get()!=&camera||page.cameraHandoffPhase_!=2||
       !page.sceneAttachmentPrefixCurrent1B48A0()||!page.cameraRegistrationCurrent1FB608()||
       scope_->pageCameraOwner_||scope_->pageCameraEntity_||scope_->pageCameraRetiring_||scope_->pageCameraRetired_||
       scope_->pageSceneAttachmentCount_!=scope_->pageSceneAttachments_.size()||
       camera.issuer_!=scope_->issuer_||camera.entities_!=scope_->application_||
       camera.entities_.owner_before(scope_->application_)||scope_->application_.owner_before(camera.entities_)||
       !camera.nativeRegistrationIdentityCurrent()||camera.phase_!=F::Phase::poseSet114F90)
        return {Status::phase,0x1FB620};
    auto& c=camera.entity_;auto* const pageNode=scope_->garagePage_;
    auto* const tail=scope_->pageSceneAttachments_.back().node;
    if(!pageNode||!scope_->garagePageCurrent(generation_,&page,*pageNode)||!tail||tail->next14||
       tail->parent1C!=pageNode||c.kind!=K::camera236180||c.parent1C||c.previous10||c.next14||
       c.firstChild18||c.descendants24||c.mask20)return {Status::topology,0x1FB620};
    const auto added=scope_->application_->entityAuthority_.attachChildren(*pageNode,c,0xffffffffu,0);
    if(added.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,added.sourcePC};
    scope_->pageCameraOwner_=&camera;scope_->pageCameraEntity_=&c;
    camera.phase_=F::Phase::pageAttached1FB620;return {Status::complete,0x1FB620};
}
bool OriginalUiEntityScope50KG::GarageConstructionGrant::garagePageCameraCurrent(
    const PageOwner& page,const OriginalFrontendCameraCold50LA& camera)const noexcept {
    if(!current()||page.grant_.get()!=this||page.sharedCamera_.get()!=&camera||
       scope_->pageCameraOwner_!=&camera||scope_->pageCameraEntity_!=&camera.entity_||
       scope_->pageCameraRetiring_||scope_->pageCameraRetired_||!scope_->garagePage_||
       !scope_->garagePageOwnedCurrent(generation_,&page,*scope_->garagePage_)||
       camera.issuer_!=scope_->issuer_||camera.entities_!=scope_->application_||
       !camera.nativeRegistrationIdentityCurrent()||camera.phase_!=OriginalFrontendCameraCold50LA::Phase::pageAttached1FB620)
        return false;
    const auto& c=camera.entity_;
    return c.kind==K::camera236180&&c.mask20==0xffffffffu&&c.parent1C==scope_->garagePage_&&
        scope_->linked(c,scope_->garagePage_);
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::GarageConstructionGrant::beginGaragePageCameraRetirement142590(
    PageOwner& page,OriginalFrontendCameraCold50LA& camera)noexcept {
    if(!garagePageCameraCurrent(page,camera)||page.cameraRetirementStep_||!page.cameraRegistrationCurrent1FB608()||
       camera.entity_.next14||camera.entity_.firstChild18||!scope_->application_->objectCount423F60_)
        return {Status::topology,0x1425B8};
    auto& c=camera.entity_;
    //1425B8 repairs live immediate links but leaves the dying object's own
    //parent/previous/next observations intact. Do not call141A70 detach.
    if(c.previous10)c.previous10->next14=c.next14;
    if(c.next14)c.next14->previous10=c.previous10;
    if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
    if(!scope_->application_->entityAuthority_.retire(c))std::terminate();
    --c.reference00;camera.enrolled_=false;
    camera.phase_=OriginalFrontendCameraCold50LA::Phase::pageRetiring1155C0;
    scope_->pageCameraRetiring_=true;return {Status::complete,0x104970};
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::GarageConstructionGrant::finishGaragePageCameraRetirement1413E0(
    PageOwner& page,OriginalFrontendCameraCold50LA& camera)noexcept {
    if(!current()||page.grant_.get()!=this||page.sharedCamera_.get()!=&camera||page.cameraRetirementStep_!=5||
       scope_->pageCameraOwner_!=&camera||scope_->pageCameraEntity_!=&camera.entity_||
       !scope_->pageCameraRetiring_||scope_->pageCameraRetired_||!scope_->garagePage_||
       !scope_->garagePageOwnedCurrent(generation_,&page,*scope_->garagePage_)||
       camera.issuer_!=scope_->issuer_||camera.entities_!=scope_->application_||!camera.onAffinity()||
       !camera.actualEntitiesAlive()||camera.enrolled_||
       camera.phase_!=OriginalFrontendCameraCold50LA::Phase::pageRetiring1155C0)
        return {Status::phase,0x1413E0};
    const auto& c=camera.entity_;
    if(c.kind!=K::camera236180||c.reference00||c.firstChild18||c.parent1C!=scope_->garagePage_||
       c.objectKey!=camera.key_||c.sessionGeneration!=camera.generation_||
       contains(scope_->application_->entityAuthority_.view(),&c)||!scope_->application_->objectCount423F60_)
        return {Status::reference,0x1413E0};
    --scope_->application_->objectCount423F60_;camera.phase_=OriginalFrontendCameraCold50LA::Phase::released;
    scope_->pageCameraRetiring_=false;scope_->pageCameraRetired_=true;
    return {Status::complete,0x1413E0};
}
}

namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::updateSharedCamera2363B0(float dt,Diagnostic& d)noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    if(!sharedCamera_||!scene4C0_||!scene4C0_->visibility_||!issuer_||!grant_||
       !updatePublication_||updateFailed_||updateReturned_||updatedPageChildren_!=11||
       updateChild_!=&sharedCamera_->entity_||!std::isfinite(dt)||dt<0||
       !grant_->garageAttachedPageCurrent(*this,entity_)||
       !scene4C0_->visibility_->pageCameraRegistrationCurrent(*this,*sharedCamera_))
        return fail(d,Status::runtimePending,0x2363B0);
    auto& camera=*sharedCamera_;auto& f=camera.fields_;
    const auto stop=[&](std::uint32_t pc)noexcept {
        camera.updateFailed_=true;camera.updateSourcePC_=pc;
        return fail(d,Status::runtimePending,pc);
    };
    if(camera.phase_!=Camera::Phase::pageAttached1FB620||camera.issuer_!=issuer_||
       !sameOwner(camera.entities_,authority_)||!camera.nativeRegistrationIdentityCurrent()||
       camera.entity_.parent1C!=&entity_||camera.updateFailed_||
       updatePublication_<=camera.updatePublication_||
       (camera.updatePublication_&&!camera.updateReturned_)||
       !f.coreKnown||!f.screen140Known||std::fegetround()!=FE_TONEAREST)
        return fail(d,Status::runtimePending,0x2363B0);
    camera.updatePublication_=updatePublication_;camera.updateReturned_=false;camera.updateSourcePC_=0x2363D4;
    //The actual process-owned Application bit, not window state, Source
    //Camera selection or a fresh per-Page default, selects this branch.
    if(issuer_->sourceDebugCamera228Bit2_)return stop(0x2363E8);
    const auto fp=[](std::uint32_t w)noexcept{return Camera::floatFromBits(w);};
    const auto vector=[&](const Camera::Words4& w)noexcept {
        return assets::TVCameraVector49E{fp(w[0]),fp(w[1]),fp(w[2]),fp(w[3])};
    };
    const auto words4=[](const assets::TVCameraVector49E& v)noexcept {
        return Camera::Words4{{Camera::floatBits(v.x),Camera::floatBits(v.y),
            Camera::floatBits(v.z),Camera::floatBits(v.w)}};
    };
    const auto words16=[](const std::array<float,16>& v)noexcept {
        Camera::Words16 w{};for(std::size_t i=0;i<w.size();++i)w[i]=Camera::floatBits(v[i]);return w;
    };
    //Arithmetic projection of this actual Camera, not bindFresh or an
    //independently advancing scene. Preserve both histories authored before
    //entry and both actual countdowns.114AE0 does not divide movement by dt.
    game::OriginalCameraSceneTick tick;
    tick.bound=true;tick.sessionGeneration=camera.generation_;tick.sceneKey=camera.key_;
    tick.previousPosition1B0=vector(f.previousPosition1B0);tick.previousLook1C0=vector(f.previousLook1C0);
    tick.distance240=fp(f.movement240);tick.angle244=fp(f.angular244);
    tick.countdown20C=f.count20C;tick.countdown210=f.count210;
    camera.updateSourcePC_=0x114AE0;
    const auto history=game::advanceOriginalCameraSceneTick114AE0(tick,camera.generation_,camera.key_,
        vector(f.position180),vector(f.look190),camera.entity_.flags29);
    using Tick=game::OriginalCameraSceneTickStatus;
    if(history!=Tick::needsMatrixVisibilityAndChildren&&history!=Tick::skippedBySceneFlag)
        return stop(0x114AE0);
    f.movement240=Camera::floatBits(tick.distance240);f.previousPosition1B0=words4(tick.previousPosition1B0);
    f.angular244=Camera::floatBits(tick.angle244);f.previousLook1C0=words4(tick.previousLook1C0);
    f.count210=tick.countdown210;f.count20C=tick.countdown20C;
    if(history==Tick::skippedBySceneFlag){camera.updateReturned_=true;camera.updateSourcePC_=0x114C74;return true;}
    //Actual virtual78 is236DD0. Borrow all current fields, including real
    //post2361C0 bounds/zoom/screen matrix. Do not use race defaults or replay
    //114F90; its earlier pose stores deliberately left these caches old.
    game::OriginalCameraViewportFields viewport;
    viewport.sessionGeneration=camera.generation_;viewport.sceneKey=camera.key_;viewport.prepared=true;
    viewport.fullWidth204=std::bit_cast<std::int32_t>(f.fullWidth204);
    viewport.fullHeight208=std::bit_cast<std::int32_t>(f.fullHeight208);viewport.aspect1DC=fp(f.aspect1DC);
    viewport.x1E0=std::bit_cast<std::int32_t>(f.viewport1E0[0]);
    viewport.y1E4=std::bit_cast<std::int32_t>(f.viewport1E0[1]);
    viewport.width1E8=std::bit_cast<std::int32_t>(f.viewport1E0[2]);
    viewport.height1EC=std::bit_cast<std::int32_t>(f.viewport1E0[3]);
    viewport.depthNear1F0=fp(f.viewport1E0[4]);viewport.depthFar1F4=fp(f.viewport1E0[5]);
    viewport.left318=fp(f.left318);viewport.right31C=fp(f.right31C);
    viewport.top320=fp(f.top320);viewport.bottom324=fp(f.bottom324);
    viewport.zoom328=fp(f.zoom328);viewport.zoom32C=fp(f.zoom32C);
    viewport.depth310=fp(f.depth310);viewport.depth314=fp(f.depth314);
    for(std::size_t i=0;i<16;++i)viewport.screen140[i]=fp(f.matrix140[i]);
    game::CameraSceneState49F scene;scene.look=vector(f.look190);scene.up=vector(f.up1A0);
    scene.fovDegrees=fp(f.fov1D0);scene.rollRadians=fp(f.roll1D4);scene.aspect1F8=fp(f.aspect1F8);
    scene.nearPlane=fp(f.near1FC);scene.farPlane=fp(f.far200);
    game::CameraViewOutput49F core;camera.updateSourcePC_=0x236DD0;
    const auto matrix=game::rebuildOriginalRaceScene236DD0(viewport,scene,vector(f.position180),core);
    if(matrix.status!=game::OriginalCameraStartStatus::complete)return stop(matrix.sourcePC);
    f.viewC0=words16(core.view);f.projection80=words16(core.projection);f.combined100=words16(core.combined);
    f.focal1D8=Camera::floatBits(viewport.focalPixels1D8);f.coreKnown=true;
    f.matrix140=words16(viewport.screen140);f.screen140Known=true;
    f.guardAngle330=Camera::floatBits(viewport.guardAngle330);
    f.guardX334=Camera::floatBits(viewport.guardX334);f.guardY338=Camera::floatBits(viewport.guardY338);
    f.combinedScreen250=words16(viewport.combinedScreen250);f.combinedScreen250Known=true;
    f.guardProjection290=words16(viewport.guardProjection290);f.guardProjection290Known=true;
    f.guardCombined2D0=words16(viewport.guardCombined2D0);f.guardCombined2D0Known=true;
    camera.updateSourcePC_=0x114C44;
    if(!scene4C0_->visibility_->updatePageCameraObserver114C44(*this,camera,d.loadResources))
        return stop(d.loadResources.sourcePC);
    camera.updateSourcePC_=0x141ED0;
    //Canonical Camera remains childless. A changed child forest is not
    //silently declared traversed after a matrix or Observer success.
    if(camera.entity_.firstChild18||(camera.entity_.descendants24&2u))return stop(0x141ED0);
    camera.updateReturned_=true;camera.updateSourcePC_=0x236A70;return true;
}
bool OriginalUiGaragePageOwner50KG::continueUpdate1FB6A0(
    const OriginalUiGarageSession50KG& session,OriginalUiGarageRendererLease50KG& renderer,
    Diagnostic& d)noexcept {
    d={};
    if(updateFailed_)return fail(d,Status::runtimePending,updateSourcePC_);
    //A later publication is admitted only after the enclosing real Draw/tail
    //rearms this native cursor. Source payloads/timers are never reconstructed.
    if(updatePublication_||!sourceEntryReturned1FAE00()||!managerAttached1D1C40_||
       !grant_->garageAttachedPageCurrent(*this,entity_)||session.page_.get()!=this||
       !session.sameRoot(*issuer_)||session.grant_!=grant_||session.podScene_!=scene4C0_||
       !scene4C0_||sceneBacking_.get()!=scene4C0_||!session.actors_||!session.runtimeFrame_||
       session.runtimeManagerChild_!=&entity_||session.runtimePhase_!=OriginalUiGarageSession50KG::RuntimePhase::managerChildren||
       session.managerRestart17C_||session.managerLifecycleCallbackPending_||
       (session.managerLifecycle16C_!=0&&session.managerLifecycle16C_!=2&&
        session.managerLifecycle16C_!=3&&session.managerLifecycle16C_!=4&&session.managerLifecycle16C_!=9)||
       !shared_.manager270||*shared_.manager270!=grant_->manager()||entity_.parent1C!=grant_->manager()||
       !renderer.currentFor(issuer_)||std::fegetround()!=FE_TONEAREST)
        return fail(d,Status::runtimePending,0x1FB6A0);
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    float dt=0;std::uint32_t pause=0;
    if(!session.entryTiming_.readPrepared(sample,serial)||
       !session.actors_->currentRuntimeFrame(*session.runtimeFrame_,dt,pause)||
       dt!=sample.updateSeconds||serial<=lastCompletedUpdatePublication_||
       completedUpdateFrames_==std::numeric_limits<std::uint64_t>::max()||
       !std::isfinite(dt)||dt<0||dt>std::bit_cast<float>(0x3d75c28fu))
        return fail(d,Status::runtimePending,0x140AC0);
    if(!router_||!sharedCamera_||!scene4C0_->sceneLighting_||scenePageAttachCursor_!=10||
       sceneLateAttachCursor_!=3||entity_.firstChild18!=&router_->entity)
        return fail(d,Status::runtimePending,0x141ED0);
    //Initial values retain the authenticated entry bounds. Later frames use
    //the same genuinely mutated timers, never an independent TimedActions copy.
    for(std::size_t i=0;i<3;++i){const float old=std::bit_cast<float>(timer34B4_[i]);
        const float low=i==1?10.5f:8.f,high=i==1?15.5f:13.f;
        if(!std::isfinite(old)||(!completedUpdateFrames_&&(old<low||old>high)))return fail(d,Status::runtimePending,0x1FB6A0);
    }
    updatePublication_=serial;updateSourcePC_=0x1FB6A0;
    auto stop=[&](std::uint32_t pc)noexcept {
        updateFailed_=true;updateSourcePC_=pc;d.sourceEffectsCommitted=true;
        return fail(d,Status::runtimePending,pc);
    };
    for(std::size_t i=0;i<3;++i){
        if(!enabled34C8_[i==2?1:0])continue;
        const float old=std::bit_cast<float>(timer34B4_[i]);
        if(old<=0)continue;
        volatile float decremented=old-dt;
        timer34B4_[i]=std::bit_cast<std::uint32_t>(static_cast<float>(decremented));
        //Retain the real subtraction if a future bounded producer changes.
        //Never zero the timer before actual actor+emitter callbacks return.
        d.sourceEffectsCommitted=true;
        if(decremented<=0&&!runPositiveTimer1FB708(session,i,d))return stop(d.sourcePC);
    }
    //Exact1FBA14/20: ONLY16C==3 or4 calls the panel transition.
    if((session.managerLifecycle16C_==3||session.managerLifecycle16C_==4)&&
       !updatePanel1FE860(session,dt,d))return stop(d.sourcePC);
    const float speech=std::bit_cast<float>(timer34B4_[4]);
    if(!std::isfinite(speech)||!entryField4E0_)return stop(0x1FBA38);
    if(speech>0){
        volatile float decremented=speech-dt;
        timer34B4_[4]=std::bit_cast<std::uint32_t>(static_cast<float>(decremented));
        d.sourceEffectsCommitted=true;
        if(*entryField4E0_&&decremented<=0&&!stopSpeechTimer2924A0(session,d))return stop(d.sourcePC);
    }
    shared_.savedDelta298=dt; //1FBA94, the2A6BB0 call's delay-slot store.
    updateSourcePC_=0x141ED0;updateChild_=(entity_.descendants24&2u)?entity_.firstChild18:nullptr;
    auto eligible=[](const C& c)noexcept{return (c.flags29&0x0du)==1u&&(c.mask20&2u);};
    auto service=[&]()noexcept {
        game::OriginalCameraStartResult result;
        if(renderer.serviceEntity141F3C(d.renderer,result))return true;
        return stop(result.sourcePC?result.sourcePC:0x141F3C);
    };
    //An inherited callback skips noneligible direct children; it does not
    //recursively update every descendant. Unknown eligible children stop.
    auto inheritedNoEligible=[&](const C& c)noexcept {
        if(!(c.descendants24&2u))return true;
        for(const C* n=c.firstChild18;n;n=n->next14){
            if(n->parent1C!=&c)return stop(0x141F00);
            if(eligible(*n)){if(!service())return false;return stop(0x141F60);}
        }
        return true;
    };
    while(updateChild_){
        auto* child=updateChild_;
        C* expected=nullptr;
        if(updatedPageChildren_==0)expected=&router_->entity;
        else if(updatedPageChildren_<=10)expected=scenePageChildren_[updatedPageChildren_-1];
        else if(updatedPageChildren_==11)expected=&sharedCamera_->entity_;
        else if(updatedPageChildren_<15)expected=sceneLateChildren_[updatedPageChildren_-12];
        if(child!=expected||child->parent1C!=&entity_)return stop(0x141F00);
        if(eligible(*child)){
            if(!service())return false;
            if(child==&router_->entity||child==scenePageChildren_[0]||
               child==scenePageChildren_[3]||child==scenePageChildren_[4]||
               child==scenePageChildren_[5]||child==scenePageChildren_[6]){
                if(!inheritedNoEligible(*child))return false;
            }else if(child==scenePageChildren_[1]){
                auto& lighting=*scene4C0_->sceneLighting_;
                //2781B0's live LightList inherits141ED0; actual two children
                //are visited in forward intrusive order, not registry order.
                for(C* light=(child->descendants24&2u)?child->firstChild18:nullptr;light;){
                    if(light->parent1C!=child)return stop(0x141F00);
                    if(eligible(*light)){
                        if(!service())return false;
                        if(!lighting.updateLight147650(*this,*light,dt,serial,d.loadResources))
                            return stop(d.loadResources.sourcePC);
                        ++updatedLights_;
                    }
                    light=light->next14; //After147650 returns.
                }
            }else if(child==scenePageChildren_[2]){
                //Actual Fog+38=13E9B0 is a literal return1, not traversal.
            }else if(child==scenePageChildren_[7]){
                OriginalUiGarageActors50GN::RuntimeReceipt receipt;
                if(!session.actors_->serviceManager101860(session.runtimeFrame_,receipt,d.loadResources))
                    return stop(d.loadResources.sourcePC);
            }else if(child==scenePageChildren_[8]){
                OriginalUiGarageActors50GN::RuntimeReceipt receipt;
                if(!session.actors_->continueToyList141ED0(session.runtimeFrame_,receipt,d.loadResources))
                    return stop(d.loadResources.sourcePC);
            }else if(child==scenePageChildren_[9]){
                if(!scene4C0_->updatePageParticleManager161180(*this,dt,d.loadResources))
                    return stop(d.loadResources.sourcePC);
            }
            else if(child==&sharedCamera_->entity_){
                if(!updateSharedCamera2363B0(dt,d))return stop(d.sourcePC);
            }
            else if(child==sceneLateChildren_[0]){
                if(!scene4C0_->updatePageEmitters27DFC0(*this,dt,renderer,d.renderer,d.loadResources))
                    return stop(d.loadResources.sourcePC);
            }
            else return stop(0x141F60);
        }
        ++updatedPageChildren_;updateChild_=child->next14; //Only after callback return.
    }
    updateReturned_=true;updateSourcePC_=0x1FBAAC;
    lastCompletedUpdatePublication_=serial;++completedUpdateFrames_;
    d.sourcePC=updateSourcePC_;d.sourceEffectsCommitted=true;return true;
}
}

namespace rr::platform {
OriginalUiGarageSceneLighting50KG* OriginalUiGaragePageOwner50KG::borrowRenderFog13EA10(const C& node)const noexcept {
    if(!grant_||!grant_->garageAttachedPageCurrent(*this,entity_)||!scene4C0_||!scene4C0_->sceneLighting_)return nullptr;
    auto& light=*scene4C0_->sceneLighting_;
    if(!light.identityCurrent()||light.closed_||!light.fog3EC_||!light.proxy3F0_||!light.reset3F4_||
       !light.fog3EC_->color30||!light.fog3EC_->start34||!light.fog3EC_->end38||
       !light.fog3EC_->helper5C||light.proxy3F0_->fog30!=&light.fog3EC_->entity||
       node.parent1C!=&entity_||light.fog3EC_->entity.parent1C!=&entity_||
       !light.fog3EC_->helper5C->current(*light.fog3EC_->color30))return nullptr;
    for(std::size_t i=0;i<3;++i)if(&node==light.fogEntity(i)&&light.fogConstructed_[i]&&!light.fogRetiring_[i]&&!light.fogRetired_[i])return &light;
    return nullptr;
}
bool OriginalUiGaragePageOwner50KG::renderFogEnd13E9C0(const C& node,float& end)const noexcept {
    if(!grant_||!grant_->garageAttachedPageCurrent(*this,entity_)||!scene4C0_||
       !scene4C0_->sceneLighting_)return false;
    const auto& light=*scene4C0_->sceneLighting_;
    if(!light.identityCurrent()||light.closed_||!light.fog3EC_||!light.proxy3F0_||
       !light.fog3EC_->end38||light.proxy3F0_->fog30!=&light.fog3EC_->entity)return false;
    if(&node!=&light.fog3EC_->entity&& &node!=&light.proxy3F0_->entity)return false;
    if(node.parent1C!=&entity_||light.fog3EC_->entity.parent1C!=&entity_)return false;
    end=*light.fog3EC_->end38;return true;
}
std::shared_ptr<OriginalUiGarageVisibility50KG> OriginalUiGaragePageOwner50KG::renderVisibility114D28()const noexcept {
    if(!grant_||!grant_->garageAttachedPageCurrent(*this,entity_)||!scene4C0_||!scene4C0_->visibility_)return {};
    return scene4C0_->visibility_;
}
bool OriginalUiGaragePageOwner50KG::renderEmptyPool162870(C& node,std::uint64_t serial,std::uint32_t& pc)noexcept {
    pc=0x162870;
    if(!grant_||!grant_->garageAttachedPageCurrent(*this,entity_)||!scene4C0_||
       !scene4C0_->sceneParticles_)return false;
    auto& pool=*scene4C0_->sceneParticles_;
    if(&node!=&pool.entity||node.parent1C!=&entity_||!pool.entityLive||!pool.prepared||
       pool.retiring||pool.retired||!pool.numeric||!pool.numeric->managerPrepared160E00()||
       !pool.numeric->manager()||!pool.updateReturned||pool.updateFailed||pool.updatePublication!=serial)return false;
    if(!serial||pool.projectionPublication==serial)return false;
    pool.projectionPublication=serial;pool.projectionSourcePC=0x162880;
    pool.field14C=0; //162880, BEFORE reading the actual retained numeric128.
    pc=pool.projectionSourcePC=0x162898;
    if(pool.numeric->manager()->particleCount()!=0)return false;
    pc=pool.projectionSourcePC=0x162D1C;
    return true; //Exact empty first-pass return, not a particle draw receipt.
}
}
