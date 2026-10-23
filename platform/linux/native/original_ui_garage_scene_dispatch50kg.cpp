#include "original_ui_garage_scene_dispatch50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_static_models50kg.h"
#include <algorithm>
#include <exception>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool dispatchFail(D& d,S status,std::uint32_t pc)noexcept {d={};d.status=status;d.sourcePC=pc;return false;}
}
OriginalUiGarageSceneDispatch50KG::~OriginalUiGarageSceneDispatch50KG()noexcept {
    if(collisionZone&&collisionZone->live)std::terminate();
}
bool OriginalUiGaragePodScene50KG::finishSceneRead270900(D& d)noexcept {
    d={};const auto* file=liveSceneConfig34();
    if(sceneTailAttempted_||loadStage_!=LoadStage::statics||!file||!sceneSetup_||
       !staticModels_||!staticModels_->descriptorIdentityCurrent()||numZones34C_!=1||
       !sceneSetup_->zones350_||sceneSetup_->zoneCount_!=1)
        return dispatchFail(d,S::phase,0x270D14);
    sceneTailAttempted_=true;
    try {
        sceneDispatch_=std::unique_ptr<OriginalUiGarageSceneDispatch50KG>(new OriginalUiGarageSceneDispatch50KG());
        auto& out=*sceneDispatch_;out.collisionAttempted=true;
        //1B4EC0 owns a zone-pointer array even when the eventual read declines.
        out.collisionLists3CC=std::make_unique<OriginalUiGarageSceneDispatch50KG::CollisionList*[]>(1);
        out.collisionZone=std::make_unique<OriginalUiGarageSceneDispatch50KG::CollisionList>();
        auto& list=*out.collisionZone;
        const auto made=grant_->constructGarageSceneCollisionList27BB40(this,list.entity); //Actual raw List3CC, not query parent3C4.
        if(!made.okay())return dispatchFail(d,S::phase,made.sourcePC);
        list.live=true;out.collisionLists3CC[0]=&list;
        const auto& zone=sceneSetup_->zones350_[0];
        // Actual Zone1 StaticModels fallback means30 collision descriptors,
        // not absence. Bit0 selects separate source names, NOT enablement.
        if(!zone.collisionCount10C||*zone.collisionCount10C!=30||zone.staticCount108!=30||
           (zone.flags148&5u)!=4u)return dispatchFail(d,S::source,0x27BCA0);
        list.count30=static_cast<std::uint32_t>(*zone.collisionCount10C);
        list.records34=std::make_unique<OriginalUiGarageSceneDispatch50KG::CollisionWrapper*[]>(list.count30);
        assets::OriginalUiPageDiagnostic50KG text;
        for(std::size_t i=0;i<list.count30;++i){
            const auto* row=file->section("Model"+std::to_string(i+1));
            if(!row)return dispatchFail(d,S::source,0x27BD84);
            const auto* type=row->find("StaticType");
            if(type&&type->sourceValuePresent&&type->text!="Standard")return dispatchFail(d,S::source,0x27BFB0);
            list.records[i]=std::make_unique<OriginalUiGarageSceneDispatch50KG::CollisionWrapper>();
            list.records34[i]=list.records[i].get(); //Pod factory1B58E0.
            list.records[i]->temporary08=std::make_unique<OriginalUiGarageSceneDispatch50KG::CollisionDescriptor>();
            auto& descriptor=*list.records[i]->temporary08;
            if(!assets::originalUiTriple50KG(*row,"Position",{},descriptor.position110,text,0x27A304)||
               !assets::originalUiTriple50KG(*row,"LookVector",{0,0,1},descriptor.look120,text,0x27A320)||
               !assets::originalUiTriple50KG(*row,"UpVector",{0,1,0},descriptor.up130,text,0x27A33C)||
               !assets::originalUiBoolean50KG(*row,"UseLighting",true,descriptor.useLighting140,text,0x27A354)||
               !assets::originalUiBoolean50KG(*row,"UseDynamicLighting",false,descriptor.useDynamicLighting144,text,0x27A36C)||
               !assets::originalUiBoolean50KG(*row,"IsSmoker",false,descriptor.isSmoker14C,text,0x27A384)||
               !assets::originalUiTriple50KG(*row,"SmokeOffset",{},descriptor.smokeOffset150,text,0x27A3A0)||
               !assets::originalUiTriple50KG(*row,"SmokeVelocity",{},descriptor.smokeVelocity160,text,0x27A3BC))
                return dispatchFail(d,S::source,text.sourcePC);
            const auto* name=row->find("COL");
            if(!name||!name->sourceValuePresent)name=row->find("SLT"); //1B5470, not failed read.
            const std::string filename=name&&name->sourceValuePresent?name->text:"none";
            const auto dot=filename.find('.');
            descriptor.stem00=filename.substr(0,std::min<std::size_t>(dot==std::string::npos?filename.size():dot,50));
            if(!assets::originalUiInteger50KG(*row,"MaterialID",1,descriptor.material184,text,0x1B54EC))
                return dispatchFail(d,S::source,text.sourcePC);
        }
        list.complete=true;
        //Actual270D24 negative branch after slots84/88/8C. Own all FIVE
        //source fields at the real reset; asset count is not this producer.
        if(!fogInterpolationReset27061C_||fogInterpolationBranch270D24_)
            return dispatchFail(d,S::phase,0x270D1C);
        if(sceneSetup_->perZoneFog4F0_)return dispatchFail(d,S::phase,0x270D2C);
        for(const auto& row:fogInterpolation4DC_)if(row)return dispatchFail(d,S::phase,0x270D24);
        fogInterpolationBranch270D24_=true;
        const auto* environment=file->section("Environment");
        if(!environment)return dispatchFail(d,S::source,0x270E88);
        if(!assets::originalUiNumber50KG(*environment,"AmbientColorFlashMinIdleTime",0,
            out.ambientMinIdle368,text,0x270EA0))return dispatchFail(d,S::source,text.sourcePC);
        // Missing key is the REAL early return270EA8, not a synthesized idle
        // effect. An authored key requires the complete flash timing owner.
        const auto* flash=environment->find("AmbientColorFlashMinIdleTime");
        if(flash&&flash->sourceValuePresent)return dispatchFail(d,S::phase,0x270EB0);
        out.sceneComplete=true;loadStage_=LoadStage::sceneTail;return true;
    }catch(...){return dispatchFail(d,S::allocation,0x1B4EC0);}
}
bool OriginalUiGaragePodScene50KG::prepareToyDispatch271CF0Prefix(D& d)noexcept {
    d={};const auto* file=liveToyConfig34();
    if(toyDispatchAttempted_||loadStage_!=LoadStage::toyOpen||!file||!sceneDispatch_||
       !sceneDispatch_->sceneComplete||sceneDispatch_->terrain3B0)
        return dispatchFail(d,S::phase,0x271CF0);
    toyDispatchAttempted_=true;
    try {
        auto& out=*sceneDispatch_;
        // Slot90=271370 allocates real TerrainInfo276830 before ToyList.
        // Its eight embedded276700 rows survive an absent-section read.
        out.terrain3B0=std::make_unique<OriginalUiGarageSceneDispatch50KG::TerrainInfo>();
        assets::OriginalUiPageDiagnostic50KG text;
        for(std::size_t i=0;i<out.terrain3B0->rows04.size();++i){
            const auto* section=file->section("TerrainZone"+std::to_string(i+1));
            if(!section)continue;
            auto& row=out.terrain3B0->rows04[i];
            if(!assets::originalUiNumber50KG(*section,"SurfaceFriction",1,row.friction00,text,0x276764)||
               !assets::originalUiNumber50KG(*section,"SurfaceDrag",1,row.drag04,text,0x27678C)||
               !assets::originalUiNumber50KG(*section,"SurfaceTraction",1,row.traction08,text,0x2767B4)||
               !assets::originalUiInteger50KG(*section,"GenerateDust",1,row.dust0C,text,0x2767D8)||
               !assets::originalUiInteger50KG(*section,"GenerateDirt",1,row.dirt10,text,0x2767FC))
                return dispatchFail(d,S::source,text.sourcePC);
            ++out.terrain3B0->count00;
        }
        // Any matching section reaches slotBC=272520 and its own producers.
        // Actual Garage TOY has none, so retained eight defaults and count0
        // are the authentic return0 from276880, not a successful empty scene.
        if(out.terrain3B0->count00)return dispatchFail(d,S::phase,0x272520);
        out.terrainComplete=true;loadStage_=LoadStage::toyPrefix;return true;
    }catch(...){return dispatchFail(d,S::allocation,0x271388);}
}
bool OriginalUiGaragePodScene50KG::closeSceneDispatchPartial270900(D& d)noexcept {
    d={};if(!sceneDispatch_)return true;
    auto& out=*sceneDispatch_;
    if(out.closed)return true;
    // Bounded native abort of a detached constructor prefix, NOT normal Page
    // teardown or authority to omit later collision children/model disposal.
    if(out.collisionZone&&out.collisionZone->live){
        auto& list=*out.collisionZone;const auto& c=list.entity;
        if(!grant_||!grant_->current()||c.reference00!=1||c.parent1C||c.firstChild18||
           c.previous10||c.next14||c.descendants24)return dispatchFail(d,S::phase,0x27BB80);
        for(const auto& row:list.records)if(row&&row->liveCollision04)return dispatchFail(d,S::phase,0x27BB80);
        for(auto& row:list.records)row.reset();
        list.records34.reset();list.count30=0;
        const auto retired=grant_->retireEmptyPrefix(list.entity,K::entity141300,nullptr);
        if(!retired.okay())return dispatchFail(d,S::phase,retired.sourcePC);
        list.live=false;
    }
    out.collisionLists3CC.reset();out.collisionZone.reset();out.terrain3B0.reset();
    out.closed=true;configFailure_=true;return true;
}
}
