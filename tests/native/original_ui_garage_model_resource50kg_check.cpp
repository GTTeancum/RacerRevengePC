// Focused authentic lower-leaf and actual production resource/selected check.
// Explicit test-only Application/Video/Session apertures are not production
// host, visibility, populated material-pass or renderer capabilities.
#include "../../platform/linux/native/original_ui_garage_selected_model50kg.h"
#include <bit>
#include "../../src/game/original_ui_entity_scope50kg.h"
#include <atomic>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <new>
#include <stdexcept>
#include <execinfo.h>
static std::atomic<bool> failNextAllocation{false};
static std::atomic<long> failAllocationAt{-1};
static std::atomic<unsigned> ownerChecks{0};
void* operator new(std::size_t n){if(failNextAllocation.exchange(false))throw std::bad_alloc();
    const auto at=failAllocationAt.load();if(at>=0&&failAllocationAt.fetch_sub(1)==0)throw std::bad_alloc();if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
namespace native_racer_model50LA {
class Owner {
public:
    static void count(rr::game::OriginalRacerModelSceneOwner50LA& scene,std::uint32_t value){scene.model_.nodeCount1A0=value;}
    static auto& entity(rr::game::OriginalRacerModelSceneOwner50LA& scene){return scene.uiScopeModelEntity8();}
    static auto warm(rr::game::OriginalRacerModelSceneOwner50LA& out,const rr::game::SceneGraph49P& graph,
        const rr::game::OriginalRacerModelSceneOwner50LA& prototype,const rr::game::SceneGraph49P& prior){
        return out.loadWarmHierarchy26EA28(graph,prototype,prior);
    }
};
}
// Isolated issuer/Session aperture: actual ApplicationEntities, authority,
// constructors and release execute; this is NOT a full production Application.
namespace original_main_entry50KG {
struct Application {
    static std::shared_ptr<rr::assets::OriginalStandardTextureLifetime50LA> textures(){
        return std::shared_ptr<rr::assets::OriginalStandardTextureLifetime50LA>(new rr::assets::OriginalStandardTextureLifetime50LA());
    }
    std::unique_ptr<rr::platform::OriginalUiGarageModelResources50KG> resources(
        std::shared_ptr<rr::game::OriginalApplicationEntities50LA> app,
        std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context){
        return std::unique_ptr<rr::platform::OriginalUiGarageModelResources50KG>(
            new rr::platform::OriginalUiGarageModelResources50KG(this,std::move(app),std::move(domain),std::move(context),77));
    }
    std::shared_ptr<rr::game::OriginalUiEntityScope50KG> make(std::shared_ptr<rr::game::OriginalApplicationEntities50LA> app){
        std::shared_ptr<rr::game::OriginalUiEntityScope50KG> out;
        if(!rr::game::OriginalUiEntityScope50KG::construct(this,std::move(app),out).okay())std::abort();
        return out;
    }
};
// Explicit isolated test host aperture. This is NOT the production Video or
// evidence that a native display/current GL context exists. All other context
// backing and the called production resource code remain genuine.
struct Video {
    bool testLive=true;
    static bool live(const Video& v)noexcept{return v.testLive;}
    static std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context(
        std::shared_ptr<Video> video,std::shared_ptr<const rr::assets::OriginalUiMaterialApplication50LA> app,
        std::shared_ptr<const rr::assets::OriginalUiMaterialRuntime50KG> runtime){
        return std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA>(
            new rr::assets::OriginalStandardMaterialContext50LA(std::move(video),std::move(app),
                std::move(runtime),Application::textures(),&Video::live,{}));
    }
};
}
namespace rr::platform {
class OriginalUiGarageSession50KG {
public:
    using Scope=game::OriginalUiEntityScope50KG;using Grant=Scope::GarageConstructionGrant;
    static std::shared_ptr<Grant> grant(const std::shared_ptr<Scope>& scope,std::size_t count=1){
        if(!scope->prepareGarageManager(77).okay())std::abort();
        std::shared_ptr<Grant> grant;
        if(!scope->issueGarageConstruction(scope,77,grant).okay()||!grant->reservePopulation(count).okay())std::abort();
        return grant;
    }
    static bool enroll(Grant& grant,game::OriginalRacerModelSceneOwner50LA& model){return grant.enrollConstructedModel(model).okay();}
    static bool attach(Grant& grant,game::OriginalRacerModelSceneOwner50LA& model){
        auto* manager=grant.manager();return manager&&grant.attachModel(*manager,model,2).okay();
    }
    static bool retire(Grant& grant,game::OriginalRacerModelSceneOwner50LA& model){return grant.retireModel(model).okay();}
    using Resource=OriginalUiGarageModelResource50KG;
    static bool load(OriginalUiGarageModelResources50KG& resources,
        std::shared_ptr<const assets::OriginalUiGarageModelAsset50KG> source,Resource::RootMode mode,
        bool lighting,std::shared_ptr<Resource>& out,assets::OriginalStandardMaterialDiagnostic50LA& d){
        return resources.load(std::move(source),mode,lighting,out,d);
    }
    static bool close(Resource& resource,assets::OriginalStandardMaterialDiagnostic50LA& d){return resource.closeUnpublished(d);}
    static bool prepare(OriginalUiGarageSelectedModel50KG& selected,std::shared_ptr<Grant> grant,
        OriginalUiGarageModelResources50KG& resources,std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,
        assets::OriginalStandardMaterialDiagnostic50LA& d){return selected.prepare1D1CD0(std::move(grant),resources,std::move(assets),d);}
    static std::shared_ptr<Resource> retained(OriginalUiGarageSelectedModel50KG& selected){return selected.model_;}
    static bool bounds(OriginalUiGarageSelectedModel50KG& selected,assets::OriginalStandardMaterialDiagnostic50LA& d){return selected.publishBounds28E110(d);}
    static bool close(OriginalUiGarageSelectedModel50KG& selected,assets::OriginalStandardMaterialDiagnostic50LA& d){return selected.close1D18D0(d);}
    static void close(Scope& scope){if(!scope.releaseGarageManager(77).okay()||!scope.close().okay())std::abort();}
};
}
namespace {
namespace a=rr::assets;namespace g=rr::game;
using I=g::SceneNodeIndex49P;
void check(bool ok,const char* why){++ownerChecks;if(!ok){std::cerr<<why<<'\n';std::abort();}}
std::shared_ptr<a::OriginalUiArchive50KG> archive(const std::string& root,a::OriginalUiArchiveId50KG id){
    const auto* identity=a::originalUiArchiveIdentity50KG(id);check(identity,"catalog");
    std::ifstream f(root+"/"+identity->discPath,std::ios::binary|std::ios::ate);check(bool(f),"file");
    std::vector<std::uint8_t> bytes(identity->bytes);f.seekg(0);f.read(reinterpret_cast<char*>(bytes.data()),bytes.size());check(bool(f),"read");
    auto out=std::make_shared<a::OriginalUiArchive50KG>();a::OriginalUiArchiveDiagnostic50KG d;
    check(out->loadOwned(id,std::move(bytes),d),"archive hash");return out;
}
struct Model {
    std::array<g::SceneNode49P,a::maxModelNodes49O+1> nodes{};
    std::array<I,a::maxModelNodes49O> rows{};
    g::OriginalRacerModelSceneOwner50LA scene;
    a::OriginalStandardMaterialReferences50LA materials;
    std::uint32_t count=0;
    g::SceneGraph49P graph(){return {nodes.data(),count+1};}
    void construct(g::OriginalApplicationEntities50LA& app,const a::OriginalUiGarageModelAsset50KG& asset,
        g::OriginalApplicationEntities50LA* counter=nullptr,g::OriginalApplicationEntities50LA* lock=nullptr,g::OriginalApplicationEntities50LA* types=nullptr){
        count=asset.payload->nodeCount();for(unsigned i=0;i<count;++i)rows[i]=static_cast<I>(i+1);
        g::OriginalRacerModelSceneBinding50LA b{77,&(counter?counter: &app)->objectCount423F60(),&(lock?lock: &app)->attachmentLock423F58(),&(types?types: &app)->types383408(),static_cast<I>(0),rows.data(),count};
        check(scene.construct2403A0(b,graph(),true).okay(),"model2403A0");
        check(scene.constructSeparateRoot26E944(graph(),*asset.payload->member()).okay(),"separate root");
    }
    void release(a::OriginalStandardModelGeometry50LA& geometry,bool referenceAlreadyReleased=false){
        g::OriginalWreckComponentDomain50LA detached{77,nullptr,0};
        check(scene.preflightScene2896D0(graph()).okay(),"scene preflight");
        if(!referenceAlreadyReleased)check(scene.releaseModelReference142590(detached).okay(),"real model reference release");
        a::OriginalStandardMaterialDiagnostic50LA md;check(materials.release26E4C0(md),"zero material loop");
        a::OriginalStandardResourceDiagnostic50LA rd;check(geometry.release26E598(&scene,materials,rd),"real geometry release");
        check(scene.freeSourceRowTable26E5CC().okay(),"row free");
        check(scene.releaseSceneBase2896D0(graph()).okay(),"scene destruction");
    }
};
void run(const std::string& root){
    auto garage=archive(root,a::OriginalUiArchiveId50KG::garage),pod=archive(root,a::OriginalUiArchiveId50KG::garagePod19);
    a::OriginalUiGarageSceneAssets50KG assets;a::OriginalUiGarageSceneAssetDiagnostic50KG ad;
    check(assets.load(garage,pod,ad),"authentic scene assets");const auto source=assets.actor(0)->model;
    auto applicationPin=std::make_shared<g::OriginalApplicationEntities50LA>();auto& application=*applicationPin;const auto base=application.objectCount423F60();
    Model prototype;prototype.construct(application,*source);
    check(prototype.scene.loadSourceRows28CAC0(prototype.graph(),source->hierarchy).okay(),"cold rows");
    check(prototype.scene.loadQuantization240570(*source->scene()).okay(),"quantization");
    check(prototype.scene.allocateSourceRowTable26EE9C().okay(),"prototype row table");
    auto resolver=std::make_shared<a::OriginalStandardTextureResolver50LA>();a::OriginalStandardTextureDiagnostic50LA cache;
    a::OriginalStandardModelFileId50LA file;
    check(resolver->bindModelMember(*source->payload->member(),a::OriginalStandardModelFileKind50LA::psg,*source->payload->originalBytes(),file,cache),"file binding");
    auto geometry=std::make_shared<a::OriginalStandardModelGeometry50LA>();a::OriginalStandardResourceDiagnostic50LA rd;
    check(geometry->construct240110(source->payload,resolver,file,&prototype.scene,rd),"geometry constructor");
    prototype.scene.fields().geometry1B4=geometry.get();check(resolver->publishGeometry1326A0(file,geometry,cache),"geometry publish");
    // The existing prototype's actual current local is not its PSG bind local.
    prototype.nodes[3].localC0.v[12]=99.25f;const auto changedName=prototype.scene.sourceRow(2)->name38;
    const auto coldCount=application.objectCount423F60();
    Model failed;failed.construct(application,*source);
    check(geometry->retain104960(&failed.scene,rd),"pre-clone warm retain");failed.scene.fields().geometry1B4=geometry.get();
    const auto prefixCount=application.objectCount423F60();
    failNextAllocation=true;
    const auto allocation=native_racer_model50LA::Owner::warm(failed.scene,failed.graph(),prototype.scene,prototype.graph());
    check(!allocation.okay()&&allocation.status==g::OriginalRacerModelSceneStatus50LA::allocation,"allocation failure not retained");
    check(application.objectCount423F60()==prefixCount&&!failed.materials.started()&&*geometry->references00()==2,"partial warm effects changed");
    // Guard must reject an inconsistent nonzero actual model count even with
    // untouched material object, then accept only its real constructor zero.
    failed.scene.fields().materialCount1C0=1;
    check(!geometry->release26E598(&failed.scene,failed.materials,rd)&&*geometry->references00()==2,"nonzero unretired materials accepted");
    failed.scene.fields().materialCount1C0=0;failed.release(*geometry);
    check(application.objectCount423F60()==coldCount&&*geometry->references00()==1,"partial warm leaked count/reference");
    Model warm;warm.construct(application,*source);check(geometry->retain104960(&warm.scene,rd),"warm retain");warm.scene.fields().geometry1B4=geometry.get();
    check(native_racer_model50LA::Owner::warm(warm.scene,warm.graph(),prototype.scene,prototype.graph()).okay(),"authentic warm clone");
    check(warm.scene.sourceRowCount()==18&&warm.scene.model()->nodeCount1A0==18&&!warm.scene.loadedFreshRows28CAC0(),"warm incorrectly relabeled fresh");
    unsigned changed=0;
    for(unsigned i=0;i<warm.scene.sourceRowCount();++i){const auto* row=warm.scene.sourceRow(i);
        if(row->name38==changedName){check(warm.nodes[static_cast<unsigned>(row->sceneIndex)].localC0.v[12]==99.25f,"warm reread bind pose");++changed;}
        check(!row->observer19C&&row->entity().reference00==1,"clone copied observer/reference");}
    check(changed==1,"source name mapping");
    check(warm.scene.copyParameters2406E0(prototype.scene).okay(),"warm parameter copy");
    check(warm.scene.allocateSourceRowTable26EE9C().okay(),"warm row table");
    check(!geometry->canReleaseForModel(&prototype.scene),"prototype retired before warm user");
    original_main_entry50KG::Application issuer;auto scope=issuer.make(applicationPin);
    using Session=rr::platform::OriginalUiGarageSession50KG;auto grant=Session::grant(scope);
    g::OriginalApplicationEntities50LA foreign;
    for(unsigned mismatch=0;mismatch<3;++mismatch){Model wrong;
        wrong.construct(application,*source,mismatch==0?&foreign:nullptr,mismatch==1?&foreign:nullptr,mismatch==2?&foreign:nullptr);
        check(!Session::enroll(*grant,wrong.scene),"foreign Application service pointer accepted");
        check(wrong.scene.closeFreshUnlinked(wrong.graph()).okay(),"foreign detached prefix cleanup");}
    check(Session::enroll(*grant,warm.scene)&&Session::attach(*grant,warm.scene),"real mixed-generation enrollment/manager attachment");
    const auto* e=warm.scene.modelEntity8();const auto* actualParent=e->parent1C;
    check(actualParent&&e->mask20==2&&e->reference00==1,"selected-model Entity source mask/reference");
    check(Session::retire(*grant,warm.scene)&&e->reference00==0&&e->parent1C==actualParent&&actualParent->firstChild18==nullptr,
        "142590 failed unlink or incorrectly performed141A70");
    warm.release(*geometry,true);Session::close(*scope);grant.reset();scope.reset();prototype.release(*geometry);
    check(application.objectCount423F60()==base&&*geometry->references00()==0,"final count/cache references");
    std::cout<<"AUTHENTIC pd18 cold/warm hierarchy, sampled local copy, pre-material allocation failure and exact zero-material geometry cleanup passed.\n";
}
void runProductionOwners(const std::string& root){
    using Session=rr::platform::OriginalUiGarageSession50KG;
    using Resource=rr::platform::OriginalUiGarageModelResource50KG;
    using Mode=Resource::RootMode;
    using D=a::OriginalStandardMaterialDiagnostic50LA;
    auto garage=archive(root,a::OriginalUiArchiveId50KG::garage);
    auto pod=archive(root,a::OriginalUiArchiveId50KG::garagePod19);
    auto appStart=archive(root,a::OriginalUiArchiveId50KG::appStart);
    auto assets=std::make_shared<a::OriginalUiGarageSceneAssets50KG>();a::OriginalUiGarageSceneAssetDiagnostic50KG ad;
    check(assets->load(garage,pod,ad),"production input assets");
    auto app=std::make_shared<g::OriginalApplicationEntities50LA>();const auto base=app->objectCount423F60();
    auto materialApp=std::make_shared<a::OriginalUiMaterialApplication50LA>();a::OriginalUiMaterialDiagnostic50KG ui;
    check(materialApp->prepareFreshApplication(appStart,ui),"real APPSTART material application");
    auto domain=std::make_shared<a::OriginalStandardMaterialDomain50LA>();D d;
    check(domain->prepareFresh(materialApp,d),"real canonical material domain");
    const auto provider=domain->archiveProvider3E1A70();a::OriginalStandardArchiveProvider50LA::Diagnostic pd;
    check(provider->mountAuthenticatedUi(garage,pd)&&provider->mountAuthenticatedUi(pod,pd)&&provider->sealNativeMounts(pd),"real complete original mounts");
    a::OriginalStandardTextureDiagnostic50LA td;
    check(domain->resolver3E1A70()->bindProvider3E1A70(provider,td),"shared provider/cache binding");
    a::OriginalStandardArchiveProvider50LA::MemberView allocationFreeLookup;
    failAllocationAt=0;
    const auto present=provider->lookup132820(*assets->selectedPod()->model->payload->member(),allocationFreeLookup);
    const bool caseEqual=a::OriginalStandardArchiveProvider50LA::sameName("textures\\specmap5.tga","TEXTURES\\SPECMAP5.TGA");
    const bool slashUnequal=!a::OriginalStandardArchiveProvider50LA::sameName("textures/specmap5.tga","textures\\specmap5.tga");
    const bool noAllocation=failAllocationAt.load()==0;failAllocationAt=-1;
    check(present==a::OriginalStandardArchiveProvider50LA::Lookup::present&&allocationFreeLookup.data&&
        caseEqual&&slashUnequal&&noAllocation,"literal provider comparison allocated or changed ASCII/slash lookup");
    auto runtime=std::make_shared<a::OriginalUiMaterialRuntime50KG>();runtime->overrideKnown=true;
    auto video=std::make_shared<original_main_entry50KG::Video>();
    auto context=original_main_entry50KG::Video::context(video,materialApp,runtime);
    original_main_entry50KG::Application issuer;auto scope=issuer.make(app);auto grant=Session::grant(scope,2);
    auto resources=issuer.resources(app,domain,context);
    const auto prefix=app->objectCount423F60();
    std::int32_t selectedRow=-1,actorRow=-1;
    check(domain->materials382B38()->find298C80("ui\\textures\\19e_1",selectedRow,d),"selected canonical first row");
    check(domain->materials382B38()->find298C80("ui\\textures\\pd1",actorRow,d),"actor canonical first row");
    const auto refs=[&](std::int32_t row){a::OriginalStandardMaterialRowView50LA view;
        check(domain->materials382B38()->row(row,view)&&view.references08,"real material row view");return *view.references08;};
    check(refs(selectedRow)==0&&refs(actorRow)==0,"genuine fresh canonical counts");
    // A source view, hierarchy and payload must all belong to the same exact
    // authenticated PSG. No identity-only admission of substituted geometry.
    for(unsigned mismatch=0;mismatch<3;++mismatch){
        auto altered=std::make_shared<a::OriginalUiGarageModelAsset50KG>(*assets->selectedPod()->model);
        if(mismatch==0)altered->payload=assets->actor(0)->model->payload;
        if(mismatch==1)altered->hierarchy=assets->actor(0)->model->hierarchy;
        if(mismatch==2)altered->archive=garage;
        std::shared_ptr<Resource> rejected;
        check(!Session::load(*resources,altered,Mode::direct,false,rejected,d),"substituted model descriptor admitted");
        check(rejected&&Session::close(*rejected,d)&&app->objectCount423F60()==prefix,"source rejection leaked prefix");
    }
    rr::platform::OriginalUiGarageSelectedModel50KG selected;
    if(!Session::prepare(selected,grant,*resources,assets,d)){
        std::cerr<<"selected prepare status="<<unsigned(d.status)<<" pc="<<std::hex<<d.sourcePC<<std::dec<<'\n';std::abort();}
    check(selected.current()&&selected.model(),"real selected19E preparation");
    auto selectedResource=Session::retained(selected);
    const auto* model=selected.model();const auto* rootModel=model->scene().model();
    check(model->scene().sourceRowCount()==1&&rootModel->nodeCount1A0==1&&model->scene().fields().sourceRows1B8,
        "selected genuine direct root count/table");
    check(model->scene().sourceRow(0)==rootModel&&model->graph().count==1,"selected root alias or graph duplicated");
    const auto* entity=model->scene().modelEntity8();const auto* parent=entity->parent1C;
    check(parent&&entity->mask20==2&&entity->reference00==1&&app->objectCount423F60()==prefix+1,"selected real manager Entity attachment/count");
    const auto& local=model->graph().nodes[0].localC0;
    const std::array<std::uint32_t,3> position{{0x4278ab14u,0x4141c3edu,0x423dd6d4u}};
    for(unsigned axis=0;axis<3;++axis)check(std::bit_cast<std::uint32_t>(local.v[12+axis])==position[axis],"selected authored translation bits");
    for(unsigned i=0;i<12;++i)check(local.v[i]==((i==0||i==5||i==10)?1.f:0.f),"selected authored +Z/+Y basis");
    check(local.v[15]==1.f&&model->scene().fields().offsetEnabled1DC==0,"selected root fourth lane/offset");
    check(model->packets().count()==1&&model->packets().packet(0)->extraMatrixCount==0&&
        model->packets().packet(0)->sourcePaletteRows.empty()&&model->packets().packet(0)->matrixSlots.size()==1&&
        !(model->scene().fields().flags1B3&1u),"selected zero-palette/local-light-off packet");
    check(model->materials().complete()&&*model->materials().handle1C4(0)==selectedRow&&refs(selectedRow)==1,
        "selected real canonical material reference");
    native_racer_model50LA::Owner::count(selectedResource->scene(),0);
    check(!Session::bounds(selected,d),"cold provenance accepted constructor-zero count");
    native_racer_model50LA::Owner::count(selectedResource->scene(),1);
    check(Session::bounds(selected,d)&&rootModel->radius198&&rootModel->nodeCount1A0==1,"actual selected direct-root bounds");
    // The loaded header pose must not become a second child transform.
    for(unsigned axis=0;axis<3;++axis)check(model->graph().nodes[0].world100.v[12+axis]==local.v[12+axis],"selected doubled transform");
    a::OriginalStandardModelFileView50LA cached;
    check(domain->resolver3E1A70()->lookupModel132820(*model->source()->payload->member(),a::OriginalStandardModelFileKind50LA::psg,cached,td)&&
        cached.geometry240110&&cached.geometry240110.get()==model->geometry()&&cached.geometry240110->prototype1C()==&model->scene(),"selected actual cache/prototype identity");
    auto geometry=cached.geometry240110;
    rr::platform::OriginalUiGarageSelectedModel50KG selectedWarm;
    check(Session::prepare(selectedWarm,grant,*resources,assets,d),"actual resources.load warm childless19E");
    auto warmSelected=Session::retained(selectedWarm);
    check(warmSelected->current()&&warmSelected->geometry()==geometry.get()&&*geometry->references00()==2&&refs(selectedRow)==2,
        "warm selected geometry/material references");
    check(warmSelected->scene().model()->nodeCount1A0==0&&warmSelected->scene().sourceRowCount()==1&&
        !warmSelected->scene().loadedFreshRows28CAC0(),"warm childless distinct path");
    check(!Session::close(selected,d)&&selected.current()&&entity->reference00==1&&refs(selectedRow)==2,
        "prototype retired before warm model");
    native_racer_model50LA::Owner::count(warmSelected->scene(),1);
    check(!Session::bounds(selectedWarm,d),"warm provenance accepted cold loaded count");
    native_racer_model50LA::Owner::count(warmSelected->scene(),0);
    check(Session::bounds(selectedWarm,d)&&warmSelected->scene().model()->nodeCount1A0==0,
        "warm selected zero-row bounds branch");
    for(unsigned axis=0;axis<3;++axis)check(std::bit_cast<std::uint32_t>(warmSelected->graph().nodes[0].world100.v[12+axis])==position[axis],
        "warm selected exact overwritten placement");
    check(Session::close(selectedWarm,d)&&*geometry->references00()==1&&refs(selectedRow)==1,"warm childless full resource retirement");
    check(Session::close(selected,d)&&!selected.current()&&entity->reference00==0&&entity->parent1C==parent&&
        parent->firstChild18==nullptr&&*geometry->references00()==0&&refs(selectedRow)==0,"selected real ordered retirement");
    check(domain->resolver3E1A70()->lookupModel132820(*assets->selectedPod()->model->payload->member(),a::OriginalStandardModelFileKind50LA::psg,cached,td)&&
        !cached.geometry240110,"selected actual cache clear1327D0");
    check(app->objectCount423F60()==prefix,"selected final Application count");
    std::shared_ptr<Resource> actor,actorWarm;
    check(Session::load(*resources,assets->actor(0)->model,Mode::separate,true,actor,d),"actual resources.load cold actor");
    check(actor->scene().sourceRowCount()==18&&actor->scene().model()->nodeCount1A0==18&&refs(actorRow)==3,"actor real hierarchy/material slots");
    const auto sourceIndex=actor->scene().sourceRow(2)->sceneIndex;actor->graph().nodes[static_cast<unsigned>(sourceIndex)].localC0.v[12]=99.25f;
    const auto changedName=actor->scene().sourceRow(2)->name38;
    check(Session::load(*resources,assets->actor(0)->model,Mode::separate,true,actorWarm,d),"actual resources.load warm actor");
    check(actorWarm->geometry()==actor->geometry()&&*actor->geometry()->references00()==2&&refs(actorRow)==6,"actor warm real material/cache references");
    unsigned copied=0;
    for(unsigned i=0;i<actorWarm->scene().sourceRowCount();++i){const auto* row=actorWarm->scene().sourceRow(i);
        if(row->name38==changedName){check(actorWarm->graph().nodes[static_cast<unsigned>(row->sceneIndex)].localC0.v[12]==99.25f,"production warm lost sampled local");++copied;}}
    check(copied==1&&!Session::close(*actor,d),"production warm prototype guard");
    check(Session::close(*actorWarm,d)&&Session::close(*actor,d)&&refs(actorRow)==0&&app->objectCount423F60()==prefix,"production actor cleanup");
    // Sweep real allocation sites in production selected direct cold load,
    // including resource cache/packet phases. Retain and retire each prefix.
    unsigned failures=0,successes=0;
    for(long allocation=0;allocation<40;++allocation){
        auto population=issuer.resources(app,domain,context);std::shared_ptr<Resource> partial;
        std::cerr<<"allocation sweep "<<allocation<<'\n';
        failAllocationAt=allocation;const bool okay=Session::load(*population,assets->selectedPod()->model,Mode::direct,false,partial,d);
        failAllocationAt=-1;
        if(okay)++successes;else ++failures;
        if(partial&&!Session::close(*partial,d)){std::cerr<<"partial cleanup at allocation="<<allocation<<" pc="<<std::hex<<d.sourcePC<<std::dec<<'\n';std::abort();}
        check(app->objectCount423F60()==prefix&&refs(selectedRow)==0,"production failure sweep leaked source ownership");
    }
    check(failures>10&&successes>0,"production sweep did not reach actual failures and completion");
    Session::close(*scope);grant.reset();scope.reset();
    check(app->objectCount423F60()==base,"production final Application count");
    std::cout<<"AUTHENTIC production resources.load/prepare + selected19E: cold/warm direct and actor hierarchy, canonical source material refs, shared geometry cache, exact placement/bounds, manager mask2, ordered retirement, "
        <<failures<<" allocation failures / "<<successes<<" completed loads. Checks="<<ownerChecks.load()<<".\n";
    std::cout<<"Test-only Video/Application/Session admission aperture. No real native Video, populated material passes, renderer, Page, normal-boot argc or aggregate admission claimed.\n";
}

}
int main(int argc,char** argv){std::cout<<std::unitbuf;std::set_terminate([](){void* trace[64];auto count=backtrace(trace,64);backtrace_symbols_fd(trace,count,2);std::abort();});if(argc!=2)return 2;run(argv[1]);runProductionOwners(argv[1]);return 0;}
