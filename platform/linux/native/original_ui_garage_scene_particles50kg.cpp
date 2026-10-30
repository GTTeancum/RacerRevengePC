#include "original_ui_garage_scene_emitters50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_scene_setup50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <new>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool stop(D& d,S status,std::uint32_t pc)noexcept{d={};d.status=status;d.sourcePC=pc;return false;}
float fp(std::uint32_t bits)noexcept{float v;std::memcpy(&v,&bits,4);return v;}
float div32(float a,float b)noexcept{volatile float v=a/b;return v;}
float mul32(float a,float b)noexcept{volatile float v=a*b;return v;}
float add32(float a,float b)noexcept{volatile float v=a+b;return v;}
}
void OriginalUiGarageSceneParticles50KG::ScratchDelete::operator()(std::uint8_t* p)const noexcept {
    ::operator delete[](p,std::align_val_t(64));
}
OriginalUiGarageSceneParticles50KG::~OriginalUiGarageSceneParticles50KG()noexcept {
    if(entityLive||retiring)std::terminate();
}
bool OriginalUiGaragePodScene50KG::prepareParticleManager1B4DC0(D& d)noexcept {
    d={};if(!currentPrefix()||populationStage_!=PopulationStage::fog||sceneParticles_||
       !sceneSetup_||!sceneSetup_->complete_||!sceneSetup_->environment3A4_||!visibility_||
       !visibility_->current()||!configResources_||!nativeApplication_||std::fegetround()!=FE_TONEAREST)
        return stop(d,S::phase,0x1B4DC0);
    const auto& env=*sceneSetup_->environment3A4_;
    const char* particleName=env.particle41C.get();
    if(!particleName||!*particleName||std::strcmp(particleName,"none")==0)return stop(d,S::source,0x1B4E08);
    const auto app=grant_->application();const auto* host=grant_->issuer();
    if(!app||!host||host->entities()!=app||!configResources_->domain_||!configResources_->domain_->prepared()||
       !configResources_->context_||!configResources_->context_->live())
        return stop(d,S::phase,0x1B4E10);
    // The genuine application global decides whether source slot100 runs.
    if(!app->visibility443A98_){
        useEnvironment35C_=1;
        game::original_world_environment50KG::SpatialBounds bounds;
        bounds.terrainWidth520=env.width520;bounds.terrainBreadth524=env.breadth524;
        bounds.terrainWidthScale528=env.widthScale528;bounds.terrainBreadthScale52C=env.breadthScale52C;
        bounds.border530=env.border530;bounds.xMin534=env.xmin534;bounds.xMax538=env.xmax538;
        bounds.zMin53C=env.zmin53C;bounds.zMax540=env.zmax540;
        if(!visibility_->prepareEnvironment272FC0(bounds,d))return false;
    }else if(!visibility_->environmentCurrent272FC0())return stop(d,S::phase,0x1B4E10);
    std::unique_ptr<OriginalUiGarageSceneConfig50KG> atlasConfig;
    const auto fail=[&](S status,std::uint32_t pc){
        if(atlasConfig){atlasConfig->close1573B0();atlasConfig.reset();}
        return stop(d,status,pc);
    };
    try {
        sceneParticles_=std::make_unique<OriginalUiGarageSceneParticles50KG>();auto& p=*sceneParticles_;
        const auto entity=grant_->constructGarageSceneParticleManager1B4DC0(this,p.entity);
        if(!entity.okay())return fail(S::phase,entity.sourcePC);
        p.entityLive=true;
        //160B10/160E00 retained CPU state. Primitive packet storage remains
        //actually null until a future real renderer producer allocates it.
        p.capacity130=5000;p.auxiliary1C0=std::make_unique<OriginalUiGarageSceneParticles50KG::Auxiliary>();
        auto& auxiliary=*p.auxiliary1C0;auxiliary.field20=fp(0x3C95E9E2u);
        auxiliary.inverseDimensions70={{div32(32767,90),div32(32767,35),div32(32767,90),1}};
        p.depthCount118=p.capacity130/2;p.buckets11C.resize(static_cast<std::size_t>(p.depthCount118));
        std::int32_t previousDepth=0;
        if(!app->writeParticleDepth160E78(p.depthCount118,previousDepth))return fail(S::phase,0x160E78);
        p.depthGlobalBefore=previousDepth;p.depthGlobalAfter=p.depthCount118;
        p.depthWritten=true; //This existing Garage occurrence, never a Racer source-call receipt.
        p.scratchBytes=static_cast<std::size_t>(p.capacity130+p.capacity130/12+1)*64u+16u;
        p.scratch15C.reset(static_cast<std::uint8_t*>(::operator new[](p.scratchBytes,std::align_val_t(64))));
        std::memset(p.scratch15C.get(),0,p.scratchBytes);
        auto authored=std::make_shared<game::OriginalUiGarageParticleAssets50KG>();
        assets::OriginalUiPageDiagnostic50KG text;
        if(authored->load(assets_,text)!=game::OriginalGcSprayStatus50KG::complete)
            return fail(S::source,text.sourcePC);
        p.assetsOwner=std::move(authored);
        auto provider=configResources_->domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return fail(S::phase,0x132820);
        std::string stem=particleName;
        const auto dot=stem.find_last_of('.');if(dot!=std::string::npos)stem.resize(dot);
        const std::string material=host->particleDirectory36B530()+stem;
        atlasConfig.reset(new OriginalUiGarageSceneConfig50KG);
        if(atlasConfig->open1577D0(provider,assets_->archive(),material+".prt",text)!=
            OriginalUiGarageSceneConfig50KG::OpenResult::opened||!atlasConfig->parsed())
            return fail(S::source,text.sourcePC?text.sourcePC:0x161820);
        p.numeric=std::make_unique<game::OriginalUiGarageParticles50KG>();
        const auto prepared=p.numeric->prepareManager160E00(p.assetsOwner,atlasConfig->readBuffer_);
        if(!prepared.okay())return fail(S::source,prepared.sourcePC);
        //1618A8 preserves the actual live Config's scalar alongside the same
        //atlas owner; the numeric parser is not a replacement file/config.
        const auto* general=atlasConfig->parsed()->section("General Info");
        if(!general||!assets::originalUiNumber50KG(*general,"GlowScalar",255,p.glowScalar148,text,0x1618A8))
            return fail(S::source,0x1618A8);
        atlasConfig->close1573B0();atlasConfig.reset(); //161AAC/stack1573B0
        auxiliary.numericChild=p.numeric->manager()->auxiliaryChild();
        if(!auxiliary.numericChild||!auxiliary.numericChild->constructed||auxiliary.numericChild->enabled18)
            return fail(S::phase,0x162F14);
        std::vector<std::uint8_t> psm,pst;
        const auto readOriginal=[&](const std::string& name,std::vector<std::uint8_t>& bytes){
            assets::OriginalStandardArchiveProvider50LA::MemberView selected;
            const auto original=assets_->archive()->find(name);
            if(!original.identity||!original.data||provider->lookup132820(name,selected)!=
                assets::OriginalStandardArchiveProvider50LA::Lookup::present||!selected.data||
                selected.size!=original.size||std::memcmp(selected.data,original.data,original.size))return false;
            bytes.assign(selected.data,selected.data+selected.size);return true;
        };
        if(!readOriginal(material+".psm",psm)||!readOriginal(material+".pst",pst))return fail(S::source,0x132820);
        auto source=std::make_shared<assets::OriginalStandardMaterialSource50LA>();
        if(!source->prepare(material+".psm",psm,text))return fail(S::source,text.sourcePC);
        if(!configResources_->domain_->appendOriginalSources({source},{{material+".pst",pst}},d))return false;
        p.material150=std::make_unique<assets::OriginalStandardParticleMaterialReference50LA>();
        if(!host->withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){
            return p.material150->acquireTrack299BE0(configResources_->domain_->materials382B38(),
                actualDirectory,particleName,d);
        }))return false;
        p.field144=1;
        p.quadPositions90={{{{-1,1,0}},{{-1,-1,0}},{{1,1,0}},{{1,-1,0}}}};
        // Source inline13E3F0 normalizer: distinct binary32 square/add,
        //sqrt, reciprocal and multiply. W remains the vector's source1.
        for(std::size_t i=0;i<4;++i){auto& normal=p.normals50[i];
            normal={{p.quadPositions90[i][0],p.quadPositions90[i][1],.25f,1}};
            const float square=add32(add32(mul32(normal[0],normal[0]),mul32(normal[1],normal[1])),mul32(normal[2],normal[2]));
            volatile float length=std::sqrt(square);const float inverse=div32(1,length);
            for(unsigned axis=0;axis<3;++axis)normal[axis]=mul32(normal[axis],inverse);
        }
        p.prepared=true;populationStage_=PopulationStage::particleManager;return true;
    }catch(...){return fail(S::allocation,0x160E00);}
}
bool OriginalUiGaragePodScene50KG::updatePageParticleManager161180(
    const OriginalUiGaragePageOwner50KG& page,float dt,D& d)noexcept {
    d={};if(!sceneParticles_||!grant_||page.grant_!=grant_||page.scene4C0_!=this||
       page.sceneBacking_.get()!=this||!page.updatePublication_||page.updateFailed_||page.updateReturned_||
       page.updatedPageChildren_!=10||page.updateChild_!=&sceneParticles_->entity||
       !grant_->garageScenePageChildCurrent(page,*this,9,sceneParticles_->entity))
        return stop(d,S::phase,0x161180);
    auto& p=*sceneParticles_;auto app=grant_->application();const auto* host=grant_->issuer();
    if(!app||page.authority_!=app||!host||page.issuer_!=host||host->entities()!=app||
       !p.entityLive||!p.prepared||p.retiring||p.retired||p.updateFailed||p.nativeGarageFifoPins_||
       page.updatePublication_<=p.updatePublication||(p.updatePublication&&!p.updateReturned)||
       !p.numeric||!p.numeric->prepared()||!p.numeric->manager()||!p.material150||
       !configResources_||!configResources_->domain_||
       p.material150->table382B38()!=configResources_->domain_->materials382B38().get())
        return stop(d,S::phase,0x161180);
    p.updatePublication=page.updatePublication_;p.updateReturned=false;p.updateSourcePC=0x1611A4;
    //Actual Application11C is read here, before material resolution. Neither
    //Renderer state nor a fresh source-pause default is accepted in its place.
    if(host->animationPaused()){
        p.updateReturned=true;p.updateSourcePC=0x161778;d.sourcePC=0x161778;return true;
    }
    bool committed=false;
    const auto failed=[&](std::uint32_t pc)noexcept {
        p.updateFailed=true;p.updateSourcePC=pc;
        if(d.status==S::complete)d.status=S::source;
        d.sourcePC=pc;d.sourceEffectsCommitted=d.sourceEffectsCommitted||committed;return false;
    };
    p.updateSourcePC=0x1611DC;
    if(!p.material150->selectGarage1611DC(d))return failed(d.sourcePC);
    committed=true; //Actual29BFF0 selection/demand call returned.
    if(!p.field158){
        std::int32_t width=0,height=0;
        const auto& textures=configResources_->domain_->textures383400();
        if(!textures)return failed(0x161204);
        if(!p.material150->garageDimensions161220(*textures,width,height,d))return failed(d.sourcePC);
        //Original CVT.S.W produces the actual divisors; no image-size guess,
        //second atlas or constructor158 overwrite is used.
        volatile float sourceWidth=static_cast<float>(width),sourceHeight=static_cast<float>(height);
        p.updateSourcePC=0x161234;
        const auto normalized=p.numeric->normalizeAtlas161234(sourceWidth,sourceHeight);
        if(normalized!=game::OriginalGcParticleStatus50KG::complete)return failed(0x161234);
        p.field158=1; //1612F4 only after all canonical atlas lanes were stored.
    }
    p.updateSourcePC=0x1612F8;
    //This is the retained real auxiliary, not a forged absent branch. Its
    //authentic162F14 enabled18 is zero. An enabled helper needs its actual
    //virtual update; do not step particles or initialize later globals early.
    if(!p.auxiliary1C0||!p.auxiliary1C0->numericChild||
       p.auxiliary1C0->numericChild!=p.numeric->manager()->auxiliaryChild()||
       !p.auxiliary1C0->numericChild->constructed)return failed(0x1612F8);
    if(p.auxiliary1C0->numericChild->enabled18)return failed(0x161318);
    if(!app->particleUpdateGuard84D4_){
        app->particleDiagnostic4339D0_=-1;app->particleUpdateGuard84D4_=1;
    }
    if(!app->particleUpdateGuard84D8_){
        app->particleDiagnostic4339D8_=-1;app->particleUpdateGuard84D8_=1;
    }
    p.updateSourcePC=0x161364;
    //Canonical numeric count/order/slots live once inside this manager. Do
    //not mirror count128 into the old constructor-only outer used128 word.
    const auto advanced=p.numeric->stepPool(dt,false);
    if(!advanced.okay())return failed(0x161364);
    p.updateSourcePC=0x1616F8;
    if(app->diagnostic14_)return failed(0x161700); //Actual diagnostic owner not joined.
    //161180 has no inherited141ED0 call. Page may now advance next14.
    p.updateReturned=true;p.updateSourcePC=0x161778;
    d.sourcePC=0x161778;d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGaragePodScene50KG::populateInstances274080(D& d)noexcept {
    d={};if(!currentPrefix()||populationStage_!=PopulationStage::particleManager||!sceneSetup_||
       !nativeApplication_||!sceneSetup_->instanceMaster_||
       nativeApplication_->instanceMaster444078!=sceneSetup_->instanceMaster_||
       !sceneSetup_->instanceMaster_->records00_||sceneSetup_->instanceCount3E0_!=0||
       sceneSetup_->instanceDescriptors3DC_)return stop(d,S::phase,0x274080);
    // Authored count0 genuinely bypasses2C06A0/2C07B0/2BFDA0. The reached
    //global InstanceMaster and capacity remain retained across this phase.
    populationStage_=PopulationStage::instances;return true;
}
bool OriginalUiGaragePodScene50KG::retireParticleManager160C70(D& d)noexcept {
    d={};
    if(!sceneParticles_)return true;
    auto& p=*sceneParticles_;
    if(p.retired)return true;
    if(p.nativeGarageFifoPins_)return stop(d,S::phase,0x160C70);
    if((sceneEmitters_&&!sceneEmitters_->retired)||
       (p.numeric&&!p.numeric->emitterRowsRetired15A720()))return stop(d,S::phase,0x160C70);
    if(!p.retiring){
        const auto preflight=grant_->preflightGarageSceneParticleManager1B4DC0(this,p.entity);
        if(!preflight.okay())return stop(d,S::phase,preflight.sourcePC);
        const auto begun=grant_->beginGarageSceneParticleManager1B4DC0(this,p.entity,p.entity.parent1C);
        if(!begun.okay())return stop(d,S::phase,begun.sourcePC);
        p.retiring=true;
    }
    p.prepared=false;p.numeric.reset(); //Native aggregate owns source pool/atlas.
    //160D38 releases150 BEFORE160D68 frees its borrowed scratch15C.
    //Only this exact zero-pin, ref0 owner phase authorizes the non-owning10.
    if(p.material150&&!p.material150->authorizeGarageRelease160D38(p,p.scratch15C.get()))
        return stop(d,S::phase,0x160D38);
    p.material150.reset();
    p.scratch15C.reset();p.scratchPayload04=nullptr; //Actual160D70 source null store.
    std::vector<OriginalUiGarageSceneParticles50KG::DepthBucket>{}.swap(p.buckets11C);
    if(p.auxiliary1C0){p.auxiliary1C0->numericChild=nullptr;--p.auxiliary1C0->reference00;p.auxiliary1C0.reset();}
    const auto releasePacket=[&](auto& primitive)noexcept {
        if(primitive.released133FA0)return true;
        if(primitive.buffer20){
            if(!primitive.ownedBuffer20||primitive.buffer20!=primitive.ownedBuffer20->words.data())
                return false;
            primitive.ownedBuffer20.reset(); //Actual133FA0; source does not null its dying20.
        }else if(primitive.ownedBuffer20)return false;
        primitive.released133FA0=true;return true;
    };
    if(!releasePacket(p.primitive190)||!releasePacket(p.primitive160))return stop(d,S::phase,0x133FA0);
    p.leaf14COwner.reset();p.leaf14CReceipt={};
    const auto finished=grant_->finishGarageSceneParticleManager1B4DC0(this,p.entity);
    if(!finished.okay())return stop(d,S::phase,finished.sourcePC);
    p.entityLive=false;p.retiring=false;p.retired=true;return true;
}
bool OriginalUiGaragePodScene50KG::retirePageParticleManager160C70(const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!grant_||!grant_->current()||page.scene4C0_!=this||page.sceneBacking_.get()!=this||!sceneParticles_)
        return stop(d,S::phase,0x160C70);
    const auto& owner=*sceneParticles_;if(owner.retired)return true;
    if(!owner.retiring&&!grant_->garagePageSceneTailCurrent(page,*this,9,owner.entity))
        return stop(d,S::phase,0x1425B8);
    return retireParticleManager160C70(d);
}

}
