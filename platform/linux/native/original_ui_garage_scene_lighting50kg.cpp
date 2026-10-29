#include "../../../src/game/original_world_light_assertion273abc.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <exception>
#include <limits>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
using Vec=std::array<float,4>;
bool stop(D& d,S status,std::uint32_t pc)noexcept{d={};d.status=status;d.sourcePC=pc;return false;}
float fp(std::uint32_t bits)noexcept{float value;std::memcpy(&value,&bits,4);return value;}
std::uint32_t word(float value)noexcept{std::uint32_t bits;std::memcpy(&bits,&value,4);return bits;}
float add(float a,float b)noexcept{volatile float result=a+b;return result;}
float sub(float a,float b)noexcept{volatile float result=a-b;return result;}
float mul(float a,float b)noexcept{volatile float result=a*b;return result;}
float div(float a,float b)noexcept{volatile float result=a/b;return result;}
bool finite(const Vec& value)noexcept{for(float lane:value)if(!std::isfinite(lane))return false;return true;}
bool normalize(Vec& value)noexcept{
    if(!finite(value))return false;
    const float norm=add(add(mul(value[0],value[0]),mul(value[1],value[1])),mul(1,mul(value[2],value[2])));
    if(!std::isfinite(norm)||(norm!=0&&(word(norm)&0x7f800000u)==0))return false;
    float q=fp(0x7f7fffffu);
    if(norm!=0){volatile float root=std::sqrt(norm);q=div(1,root);}
    for(std::size_t i=0;i<3;++i)value[i]=mul(value[i],q);
    return finite(value);
}
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept{
    return a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
OriginalUiGarageSceneLighting50KG::~OriginalUiGarageSceneLighting50KG()noexcept{
    if(!closed_||managerBound_)std::terminate();
    for(const auto& row:lights_)if(row&&row->constructed&&!row->retired)std::terminate();
    for(std::size_t i=0;i<3;++i)if(fogConstructed_[i]&&!fogRetired_[i])std::terminate();
}
bool OriginalUiGarageSceneLighting50KG::identityCurrent()const noexcept{
    return !closed_&&owner_&&owner_->sceneLighting_.get()==this&&owner_->sceneLightingOwnerCurrent273AA0(*this)&&issuer_&&application_&&generation_&&manager_;
}
bool OriginalUiGarageSceneLighting50KG::lightIdentity(const Light& light)const noexcept{
    return light.entity.kind==game::OriginalWreckComponentKind50LA::light146720&&light.entity.sessionGeneration==generation_&&
        light.entity.objectKey==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&light.entity));
}
void OriginalUiGarageSceneLighting50KG::initializeLight146720(std::size_t i)noexcept{
    auto& light=*lights_[i];
    light.range84=fp(game::SourceUiLightMath50KG::range146898());
    light.cone9C=fp(0x3fc90fdbu);light.cone98=div(light.cone9C,2);
}
bool OriginalUiGarageSceneLighting50KG::configureLight146F60(std::size_t i,
    const OriginalUiGarageSceneSetup50KG::LightTemp& source,D& d)noexcept{
    if(i>=2||!lights_[i]||!identityCurrent()||!managerBound_||std::fegetround()!=FE_TONEAREST)
        return stop(d,S::phase,0x146F60);
    auto& light=*lights_[i];Manager::Diagnostic md;
    if(!light.typed||light.configured||source.type00!=(i?2:4)||source.zone20||!finite(source.color04)||
       !(source.flags28&4u)||(source.flags28&0x30u))return stop(d,S::source,0x277D80);
    light.type34=static_cast<std::uint32_t>(source.type00);light.ordinalC8=static_cast<std::int32_t>(i);
    Vec half;for(std::size_t lane=0;lane<4;++lane)half[lane]=div(source.color04[lane],2);
    if(source.type00==4){light.ambient5C=half;light.diffuse3C=light.specular4C={{0,0,0,0}};}
    else{light.diffuse3C=light.specular4C=half;light.ambient5C={{0,0,0,0}};}
    if(!manager_->incrementSceneRevision149300(*this,md))return stop(d,S::phase,md.sourcePC);
    if(!game::SourceUiLightMath50KG::luminance146E60(word(light.diffuse3C[0]),word(light.diffuse3C[1]),
        word(light.diffuse3C[2]),light.luminanceCC))return stop(d,S::source,0x146E60);
    const float maximum=fp(game::SourceUiLightMath50KG::range146898());
    const auto setRange=[&](float range){
        if(!std::isfinite(range))return stop(d,S::source,0x147018);
        if(range==0||range>maximum)range=maximum;
        if(range!=light.range84){light.range84=range;
            if(!manager_->incrementSceneRevision149300(*this,md))return stop(d,S::phase,md.sourcePC);}
        return true;
    };
    if(!setRange(source.range24))return false;
    if(source.position14){const auto& position=*source.position14;
        if(!finite(position))return stop(d,S::source,0x1470B4);
        if(position[0]!=light.positionA0[0]||position[1]!=light.positionA0[1]||position[2]!=light.positionA0[2]){
            light.positionA0=position;light.position6C={{position[0],position[1],position[2]}};
            //147600 has no callback/flare work with actual C0/C4/D0 null.
            if(!manager_->incrementSceneRevision149300(*this,md))return stop(d,S::phase,md.sourcePC);
        }
    }
    if(source.look18){const auto& look=*source.look18;
        if(look[0]!=light.lookB0[0]||look[1]!=light.lookB0[1]||look[2]!=light.lookB0[2]){
            light.lookB0=look;
            if(!normalize(light.lookB0))return stop(d,S::source,0x147200);
            light.look78={{light.lookB0[0],light.lookB0[1],light.lookB0[2]}};
            if(!manager_->incrementSceneRevision149300(*this,md))return stop(d,S::phase,md.sourcePC);
        }
    }
    if(source.attenuation1C){const auto& attenuation=*source.attenuation1C;
        if(!finite(attenuation))return stop(d,S::source,0x147238);
        light.attenuation8C={{attenuation[0],attenuation[1],attenuation[2]}};
        float range=0;
        if(attenuation[2]!=0){
            //147280 MULA,14728C MUL,147294 SUB,147298 MSUB,14729C SQRT.
            //Keep each EE binary32 operation distinct; never host fused FMA.
            const float square=mul(attenuation[1],attenuation[1]);
            const float fourC=mul(4,attenuation[2]);
            const float aMinusTwo=sub(attenuation[0],2);
            const float discriminant=sub(square,mul(fourC,aMinusTwo));
            if(discriminant<0||!std::isfinite(discriminant))return stop(d,S::source,0x14729C);
            volatile float root=std::sqrt(discriminant);
            range=div(add(-attenuation[1],root),mul(2,attenuation[2]));
        }else{
            if(attenuation[1]==0)return stop(d,S::source,0x147378);
            range=div(sub(2,attenuation[0]),attenuation[1]);
        }
        if(!setRange(range))return false;
    }else light.attenuation8C={{1,0,0}};
    light.mode38=3;
    //The lens-flare string passed on146F60's stack is not consumed there.
    //No C0/C4 owner is manufactured from HasLensFlare.277FE4 uses bit4,
    //which is false in this authored Scene; both actual pointers stay null.
    light.configured=true;return true;
}
OriginalUiGarageSceneLighting50KG::C* OriginalUiGarageSceneLighting50KG::fogEntity(std::size_t i)const noexcept{
    return i==0?(fog3EC_?&fog3EC_->entity:nullptr):i==1?(reset3F4_?&reset3F4_->entity:nullptr):
        i==2?(proxy3F0_?&proxy3F0_->entity:nullptr):nullptr;
}
bool OriginalUiGarageSceneLighting50KG::colorWriteCurrentFor(const ::original_main_entry50KG::Application* root)const noexcept{
    if(root!=issuer_||!identityCurrent()||!lightsComplete_||fogStage_!=FogStage::awaitingRenderer||rendererWritePerformed_||
       !fog3EC_||!fog3EC_->helper5C||!fog3EC_->color30||!fog3EC_->start34||!fog3EC_->end38)return false;
    for(std::size_t i=0;i<3;++i)if(!fogConstructed_[i]||fogRetiring_[i]||fogRetired_[i]||!fogEntity(i)||fogEntity(i)->reference00!=1)return false;
    const auto& helper=*fog3EC_->helper5C;
    return !helper.buffer20&&!helper.count24&&!helper.count28&&helper.field2C==-1;
}
bool OriginalUiGaragePodScene50KG::populateLights273AA0(std::shared_ptr<OriginalUiLightManagerOwner50KG> manager,D& d)noexcept{
    d={};
    if(populationStage_!=PopulationStage::visibility||!currentPrefix()||configFailure_||sceneLighting_||retainedLightManager426F60_||!sceneSetup_||
       !sceneSetup_->complete_||!sceneSetup_->lights3A8_||!grant_||!grant_->current()||!manager||!manager->current()||
       manager->issuer_!=grant_->issuer()||!sameOwner(manager->entities_,grant_->application())||
       !sameOwner(manager,manager->issuedOwner_.lock())||std::fegetround()!=FE_TONEAREST)return stop(d,S::phase,0x273AA0);
    auto& source=*sceneSetup_->lights3A8_;
    if(!source.entityLive||source.count30!=2||!source.lights38||!source.zoneCount34||*source.zoneCount34!=1||
       !source.zoneColors3C||numZones34C_!=1)return stop(d,S::source,0x273AA0);
    try{
        sceneLighting_=std::unique_ptr<OriginalUiGarageSceneLighting50KG>(new OriginalUiGarageSceneLighting50KG());
        auto& out=*sceneLighting_;out.owner_=this;out.issuer_=grant_->issuer();out.application_=grant_->application();
        retainedLightManager426F60_=manager;
        out.generation_=grant_->generation_;out.manager_=std::move(manager);
        OriginalUiLightManagerOwner50KG::Diagnostic md;
        if(!out.manager_->bindSceneLights(out,md))return stop(d,S::phase,md.sourcePC);
        for(std::size_t i=0;i<2;++i){auto& wrapper=source.lights38[i];
            //This admitted Garage row bypasses the shared 273ABC assertion guard.
            //Preserve unknown prior history before the native zone check.
            rr::game::world_light_assertion273ABC::invalidateNative();
            if(!wrapper.temp04||wrapper.liveLight00||wrapper.temp04->zone20<0||wrapper.temp04->zone20>=numZones34C_)
                return stop(d,S::source,0x273B14);
            out.lights_[i]=std::make_unique<OriginalUiGarageSceneLighting50KG::Light>();auto& light=*out.lights_[i];
            const auto made=grant_->constructGarageSceneLight273AA0(this,i,light.entity,source.entity);
            if(!made.okay())return stop(d,S::phase,made.sourcePC);
            light.constructed=true;out.initializeLight146720(i);wrapper.liveLight00=&light.entity; //277E28.
            const auto query=out.application_->types383408().lightEmitter146938();
            if(!query.okay())return stop(d,S::phase,query.sourcePC);
            light.query32=query.type;light.typed=true;
            if(!out.configureLight146F60(i,*wrapper.temp04,d))return false;
            if(!out.manager_->appendSceneLight148F30(out,i,md))return stop(d,S::phase,md.sourcePC);
            //2780D8..278148 frees the three owned vectors in source order.
            wrapper.temp04->position14.reset();
            wrapper.temp04->look18.reset();
            wrapper.temp04->attenuation1C.reset();
            wrapper.temp04.reset(); //Before parent141960.
            const auto attached=grant_->attachGarageSceneLight273B98(this,i,light.entity,source.entity);
            if(!attached.okay())return stop(d,S::phase,attached.sourcePC);
            light.attached=true;
        }
        //273BE0 compares ONLY x. Undefined y/z/w must remain unread.
        const auto& x=source.zoneColors3C[0].lanes[0];
        if(!x||!(*x<-255.f))return stop(d,S::source,0x1498A0);
        out.lightsComplete_=true;populationStage_=PopulationStage::lights;return true;
    }catch(...){return stop(d,S::allocation,0x277E0C);}
}
bool OriginalUiGaragePodScene50KG::prepareFog273C30(float farDistance,float nearDistance,
    std::shared_ptr<OriginalUiGarageFogColorLease50KG> lease,D& d)noexcept{
    d={};
    if(populationStage_!=PopulationStage::lights||!currentPrefix()||!sceneLighting_||!sceneLighting_->lightsComplete_||
       !sceneLighting_->identityCurrent()||!sceneSetup_||!grant_||!grant_->current()||std::fegetround()!=FE_TONEAREST)
        return stop(d,S::phase,0x273C30);
    auto& out=*sceneLighting_;
    if(out.fogStage_==OriginalUiGarageSceneLighting50KG::FogStage::empty&&!sceneSetup_->fog3AC_){
        populationStage_=PopulationStage::fog;return true; //273C5C: no FogInfo, no owners.
    }
    const float far=farDistance==-1?768.f:farDistance;
    const float near=nearDistance==-1?64.f:nearDistance;
    if(!std::isfinite(far)||!std::isfinite(near))return stop(d,S::source,0x273D54);
    try{
        if(out.fogStage_==OriginalUiGarageSceneLighting50KG::FogStage::empty){
            out.fogStage_=OriginalUiGarageSceneLighting50KG::FogStage::constructing;
            const auto& info=*sceneSetup_->fog3AC_;
            std::array<std::uint32_t,4> color{};
            for(std::size_t i=0;i<4;++i){const float v=info.color00[i];
                if(!std::isfinite(v)||v<0||v>255)return stop(d,S::source,0x273C78);
                color[i]=static_cast<std::uint32_t>(v);
            }
            out.fog3EC_=std::make_unique<OriginalUiGarageSceneLighting50KG::Fog>();
            auto made=grant_->constructGarageSceneFog273C30(this,0,out.fog3EC_->entity);
            if(!made.okay())return stop(d,S::phase,made.sourcePC);
            out.fogConstructed_[0]=true;
            out.reset3F4_=std::make_unique<OriginalUiGarageSceneLighting50KG::Reset>();
            made=grant_->constructGarageSceneFog273C30(this,1,out.reset3F4_->entity);
            if(!made.okay())return stop(d,S::phase,made.sourcePC);
            out.fogConstructed_[1]=true;
            out.proxy3F0_=std::make_unique<OriginalUiGarageSceneLighting50KG::Proxy>();
            made=grant_->constructGarageSceneFog273C30(this,2,out.proxy3F0_->entity);
            if(!made.okay())return stop(d,S::phase,made.sourcePC);
            out.fogConstructed_[2]=true;
            auto& fog=*out.fog3EC_;fog.far50=far;fog.near54=near;fog.gap58=64;
            fog.helper5C=std::make_unique<OriginalUiGarageSceneLighting50KG::Primitive>();
            fog.color30=(color[3]<<24)|(color[0]<<16)|(color[1]<<8)|color[2]|0xff000000u;
            fog.visibility3C=info.visibility10;fog.haziness40=info.haziness14;
            fog.end38=add(near,mul(info.visibility10,sub(far,near)));
            fog.start34=mul(sub(1,info.haziness14),sub(*fog.end38,64));
            if(!std::isfinite(*fog.end38)||!std::isfinite(*fog.start34))return stop(d,S::source,0x13E89C);
            out.fogStage_=OriginalUiGarageSceneLighting50KG::FogStage::awaitingRenderer;
        }
        if(out.fogStage_!=OriginalUiGarageSceneLighting50KG::FogStage::awaitingRenderer||
           !out.fog3EC_||out.fog3EC_->far50!=far||out.fog3EC_->near54!=near)return stop(d,S::phase,0x13E8A8);
        //Missing actual producer retains the precise populated prefix. A later
        //call with the genuine lease may continue this ONE store, never replay
        //constructors or invent a successful renderer callback.
        if(!lease||!lease->currentOwnedBy(lease,out.issuer_)||!lease->storeScene13E8A8(out))return stop(d,S::phase,0x13E8A8);
        out.renderer_=std::move(lease);out.rendererWritePerformed_=true;out.fog3EC_->dirty60=1;
        //13F890 is141C20 return-this.13F9C0 binds a raw borrow, no retain.
        out.proxy3F0_->fog30=&out.fog3EC_->entity;
        sceneSetup_->fog3AC_.reset(); //273F70/273F7C, after all three initializers.
        out.fogStage_=OriginalUiGarageSceneLighting50KG::FogStage::complete;
        populationStage_=PopulationStage::fog;return true;
    }catch(...){return stop(d,S::allocation,0x273CAC);}
}
bool OriginalUiGaragePodScene50KG::sceneLightingOwnerCurrent273AA0(const OriginalUiGarageSceneLighting50KG& owner)const noexcept {
    if(sceneLighting_.get()!=&owner||owner.owner_!=this||owner.closed_||!grant_||!grant_->current()||
       owner.issuer_!=grant_->issuer()||owner.generation_!=grant_->generation_||!sameOwner(owner.application_,grant_->application())||
       podScene_.kind!=game::OriginalWreckComponentKind50LA::entity141300||podScene_.sessionGeneration!=owner.generation_||
       podScene_.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&podScene_)))return false;
    //The actual Scene ledger persists through its source104970 interval.
    //No dependency on the already-retired SoundGroup4D8, nor a readiness flag.
    return (podScene_.reference00==1&&grant_->podSceneOwnerCurrent(this,podScene_))||
        (podScene_.reference00==0&&grant_->scenePodRetirementCurrent(podScene_));
}
bool OriginalUiGaragePodScene50KG::retireSceneLight146980(std::size_t ordinal,D& d)noexcept {
    d={};
    if(!sceneLighting_||ordinal>=2||!sceneLightingOwnerCurrent273AA0(*sceneLighting_)||!sceneSetup_||!sceneSetup_->lights3A8_)
        return stop(d,S::phase,0x146980);
    auto& out=*sceneLighting_;auto& row=out.lights_[ordinal];
    if(!row||!row->constructed||row->retired)return true;
    auto& light=*row;
    const C* const parent=light.attached?&sceneSetup_->lights3A8_->entity:nullptr;
    if(!out.lightIdentity(light)||!out.managerBound_||!out.manager_||light.fieldC0||light.fieldC4||light.observerD0||light.fieldD8)
        return stop(d,S::phase,0x146980);
    if(!light.retiring){
        const auto ready=grant_->preflightGarageSceneLightRetirement(this,ordinal,light.entity,parent);
        if(!ready.okay())return stop(d,S::phase,ready.sourcePC);
        if(!out.manager_->preflightRemoveSceneLight1491F0(out,ordinal))return stop(d,S::phase,0x1491F0);
        const auto begin=grant_->beginGarageSceneLightRetirement(this,ordinal,light.entity,parent);
        if(!begin.okay())return stop(d,S::phase,begin.sourcePC);
        light.retiring=true;
    }
    OriginalUiLightManagerOwner50KG::Diagnostic md;
    if(!light.derivedRetired){
        if(!out.manager_->removeSceneLight1491F0(out,ordinal,md))return stop(d,S::phase,md.sourcePC);
        light.derivedRetired=true;
    }
    const auto finish=grant_->finishGarageSceneLightRetirement(this,ordinal,light.entity,parent);
    if(!finish.okay())return stop(d,S::phase,finish.sourcePC);
    light.retired=true;light.retiring=false;
    //Wrapper live00 is a raw source-stale address until277C30. Do not give
    //that wrapper a fabricated null store or a second Entity release.
    return true;
}
bool OriginalUiGaragePodScene50KG::retireSceneFog13E700(std::size_t ordinal,D& d)noexcept {
    d={};
    if(!sceneLighting_||ordinal>=3||!sceneLightingOwnerCurrent273AA0(*sceneLighting_))return stop(d,S::phase,0x13E700);
    auto& out=*sceneLighting_;
    if(!out.fogConstructed_[ordinal]||out.fogRetired_[ordinal])return true;
    C* const entity=out.fogEntity(ordinal);if(!entity)return stop(d,S::phase,0x13E700);
    const C* const parent=entity->parent1C; //Typed Grant checks this exact ledger identity.
    if(!out.fogRetiring_[ordinal]){
        const auto ready=grant_->preflightGarageSceneFogRetirement(this,ordinal,*entity,parent);
        if(!ready.okay())return stop(d,S::phase,ready.sourcePC);
        const auto begin=grant_->beginGarageSceneFogRetirement(this,ordinal,*entity,parent);
        if(!begin.okay())return stop(d,S::phase,begin.sourcePC);
        out.fogRetiring_[ordinal]=true;
    }
    if(ordinal==0){
        if(!out.fog3EC_)return stop(d,S::phase,0x13E700);
        if(out.fog3EC_->helper5C){const auto& helper=*out.fog3EC_->helper5C;
            if(!out.fog3EC_->color30||!helper.current(*out.fog3EC_->color30))return stop(d,S::phase,0x133FA0);
            out.fog3EC_->helper5C.reset(); //13E740 before Entity1413E0.
        }
    }
    //Reset13FAD0 and Proxy13FA70 have no derived owned Entity/pointer release.
    const auto finish=grant_->finishGarageSceneFogRetirement(this,ordinal,*entity,parent);
    if(!finish.okay())return stop(d,S::phase,finish.sourcePC);
    out.fogRetired_[ordinal]=true;out.fogRetiring_[ordinal]=false;return true;
}
bool OriginalUiGaragePodScene50KG::finishSceneLightingPayload273AA0(D& d)noexcept {
    d={};if(!sceneLighting_)return true;
    auto& out=*sceneLighting_;
    if(!sceneLightingOwnerCurrent273AA0(out))return stop(d,S::phase,0x2781B0);
    for(const auto& row:out.lights_)if(row&&row->constructed&&!row->retired)return stop(d,S::phase,0x146980);
    for(std::size_t i=0;i<3;++i)if(out.fogConstructed_[i]&&!out.fogRetired_[i])return stop(d,S::phase,0x13E700);
    if(out.managerBound_){OriginalUiLightManagerOwner50KG::Diagnostic md;
        if(!out.manager_||!out.manager_->releaseSceneLightsBorrow(out,md))return stop(d,S::phase,md.sourcePC?md.sourcePC:0x2781B0);
    }
    //Native actual field custody only. Neither registry stale field40 nor the
    //renderer13E8A8 field is restored to UI values by source destruction.
    out.renderer_.reset();out.manager_.reset();out.closed_=true;sceneLighting_.reset();return true;
}

bool OriginalUiGaragePodScene50KG::retirePageFog13E700(const OriginalUiGaragePageOwner50KG& page,
    std::size_t ordinal,D& d)noexcept {
    d={};if(ordinal>=3||!grant_||!grant_->current()||page.scene4C0_!=this||page.sceneBacking_.get()!=this||
       !sceneLighting_||!sceneLightingOwnerCurrent273AA0(*sceneLighting_))return stop(d,S::phase,0x1425B8);
    auto& out=*sceneLighting_;auto* entity=out.fogEntity(ordinal);
    if(!entity||!out.fogConstructed_[ordinal])return stop(d,S::phase,0x13E700);
    if(out.fogRetired_[ordinal])return true;
    if(!out.fogRetiring_[ordinal]&&!grant_->garagePageSceneTailCurrent(page,*this,ordinal+2,*entity))
        return stop(d,S::phase,0x1425B8);
    return retireSceneFog13E700(ordinal,d);
}
bool OriginalUiGaragePodScene50KG::retirePageLightList2781B0(const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!grant_||!grant_->current()||page.scene4C0_!=this||page.sceneBacking_.get()!=this||
       !sceneSetup_||!sceneSetup_->lights3A8_)return stop(d,S::phase,0x2781B0);
    auto& list=*sceneSetup_->lights3A8_;if(list.pageRetired)return true;
    if(!sceneLighting_||!sceneLightingOwnerCurrent273AA0(*sceneLighting_))return stop(d,S::phase,0x2781B0);
    if(!list.pageRetiring){
        if(!list.entityLive||list.count30!=2||!list.lights38||
           !grant_->garagePageSceneTailCurrent(page,*this,1,list.entity))return stop(d,S::phase,0x1425B8);
        //142590 visits the actual last Light first. Wrapper live00 is still
        //the source stale address after that child releases its last ref.
        for(std::size_t i=2;i;--i){
            const auto& light=sceneLighting_->lights_[i-1];
            if(!light||!light->constructed||list.lights38[i-1].liveLight00!=&light->entity)
                return stop(d,S::phase,0x146980);
            if(!retireSceneLight146980(i-1,d))return false;
        }
        const auto begun=grant_->beginGaragePageLightList142590(page,*this,list.entity);
        if(!begun.okay())return stop(d,S::phase,begun.sourcePC);
        list.pageRetiring=true;
    }
    //2781E8 uses2E8750/277C30 in reverse wrapper order.277C30 never
    //releases live00: it destroys only its actual temp04 and owned vectors.
    if(list.lights38){
        for(std::size_t i=2;i;--i){auto& row=list.lights38[i-1];
            if(row.temp04){row.temp04->position14.reset();row.temp04->look18.reset();
                row.temp04->attenuation1C.reset();row.temp04.reset();}
        }
        list.lights38.reset(); //2781F0 stores source38 null.
    }
    list.zoneColors3C.reset(); //278200/278208, before Entity base.
    const auto finished=grant_->finishGaragePageLightList1413E0(page,*this,list.entity);
    if(!finished.okay())return stop(d,S::phase,finished.sourcePC);
    list.entityLive=false;list.pageRetiring=false;list.pageRetired=true;
    //count30 and zoneCount34 remain source-stale. The later Scene270120
    //global manager zone release is NOT performed by this Page callback.
    return true;
}

}

namespace rr::platform {
bool OriginalUiGarageSceneLighting50KG::updateLight147650(
    const OriginalUiGaragePageOwner50KG& page,C& node,float dt,std::uint64_t serial,D& d)noexcept {
    d={};
    if(!serial||!std::isfinite(dt)||dt<0||!identityCurrent()||!managerBound_||
       !manager_->sourceMutationAvailable()||page.scene4C0_!=owner_||page.sceneBacking_.get()!=owner_||
       page.authority_!=application_||page.updatePublication_!=serial||page.updateFailed_||
       page.updateChild_!=page.scenePageChildren_[1]||node.parent1C!=page.updateChild_)
        return stop(d,S::phase,0x147650);
    Light* actual=nullptr;
    for(const auto& row:lights_)if(row && &row->entity==&node){actual=row.get();break;}
    if(!actual||!lightIdentity(*actual)||!actual->constructed||!actual->configured||!actual->registered||
       !actual->attached||actual->retiring||actual->retired||
       (node.flags29&0x0du)!=1u||!(node.mask20&2u)||actual->updateFailed||serial<=actual->updatePublication)
        return stop(d,S::phase,0x147650);
    auto& light=*actual;light.updatePublication=serial;light.updateReturned=false;
    auto failed=[&](std::uint32_t pc)noexcept{light.updateFailed=true;return stop(d,S::source,pc);};
    //14766C: these two actual authored Lights have no pattern ownerD8.
    //A future flashing light must execute its own146C40 color publication.
    if(light.fieldD8)return failed(0x14766C);
    //100008 cleared BSS;147A14/18 publish the shared first-use values once.
    //No LightManager/Page retirement clears this actual Application history.
    Manager::Diagnostic shared;
    if(!manager_->serviceSceneLightShared147A00(*this,node,shared))return failed(shared.sourcePC);
    //147EB0 is the actual inherited141ED0 call. These concrete Lights have
    //no Entity children; no camera scratch path or renderer draw is invented.
    if(node.firstChild18||(node.descendants24&2u))return failed(0x147EB0);
    light.updateReturned=true;d.sourcePC=0x147EC8;return true;
}
}
