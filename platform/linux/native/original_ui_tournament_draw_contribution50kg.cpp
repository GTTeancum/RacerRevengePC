#include "original_ui_tournament_draw_contribution50kg.h"
#include "original_ui_tournament_prompt_placement50kg.h"
#include "../../../src/assets/original_ui_material_equivalence50kg.h"
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <optional>
#include <utility>
#include "vehicle_window_gl50ds.h"
namespace rr::platform {
namespace {
using D=OriginalUiTournamentContributionDiagnostic50KG;using E=OriginalUiTournamentContributionError50KG;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(D& d,E e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;return false;}
char fold(char c) noexcept{return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
    return i==a.size()&&!b[i];
}
bool samplerSame(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b) noexcept {
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
}
enum class RegistrationKind : std::uint8_t {existingStatic,ledgerDynamic};
using LiveMaterial=game::OriginalUiRiderLiveMaterial50KG;
using Lifetime=game::OriginalUiRiderMaterialLifetime50KG;
using MaterialRow=game::OriginalUiRiderMaterialRow50KG;
bool compatible(const assets::OriginalUiMaterialPass50KG& a,
    const assets::OriginalUiMaterialPass50KG& b) noexcept {
    return a.pass==b.pass&&a.identity==b.identity&&a.inheritedTemplate==b.inheritedTemplate&&
        a.provenance.origin==b.provenance.origin&&a.provenance.archive==b.provenance.archive&&
        a.provenance.fallbackTexture==b.provenance.fallbackTexture;
}
struct Binding {
    std::string registeredName;
    const assets::OriginalUiMaterialPass50KG* pass=nullptr;
    const assets::OriginalUiImageProvenance50KG* provenance=nullptr;
    std::uint32_t image=unresolved,leaf=unresolved,registration=unresolved;
    MaterialTextureSampler50EB resourceSampler{};
    std::optional<OriginalUiMaterialSpan50KG> prototype; //Absent means resources only; never a default draw policy.
    // Real PSM registry existence, independent of cold sampler/image policy.
    // False bindings can be uploaded but cannot redirect or receive packets.
    bool sourceRegistered=false;
    RegistrationKind kind=RegistrationKind::existingStatic;
    MaterialRow row{};
    std::shared_ptr<LiveMaterial> live;
};
struct Packet {std::uint32_t first=0,count=0,binding=0,ordinal=0;};
struct Registration {
    std::uint32_t rank=0,pass=0;
    const assets::OriginalUiMaterialPass50KG* source=nullptr;
    assets::OriginalUiMaterialBindingState50KG state;
    RegistrationKind kind=RegistrationKind::existingStatic;
    const Lifetime* ledger=nullptr;
    MaterialRow row{};
    std::uint64_t epoch=0;
    std::shared_ptr<LiveMaterial> live;
};
struct MaterialObservation {
    MaterialRow row{};std::uint64_t epoch=0;std::uint32_t owners=0;
};
struct PendingLiveMaterial {
    std::shared_ptr<LiveMaterial> live;
    std::vector<assets::OriginalUiMaterialBindingState50KG> states;
};
bool material(const assets::OriginalUiMaterialPass50KG& pass,const assets::OriginalUiScreenAssets50KG& images,
    std::uint32_t index,OriginalUiMaterialSpan50KG& output,D& d) noexcept {
    const auto* textures=images.textures();if(!textures)return fail(d,E::bank);
    if(pass.textureCount==0){assets::OriginalUiNativeUntexturedMaterial50KG policy{};
        return assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,policy,d.materialDetail)&&
            convertOriginalUiUntexturedMaterialSpan50KG(policy,{0,1,1},output);}
    const auto* texture=images.decodedTexture(index);if(!texture)return fail(d,E::material);
    const OriginalUiMaterialSpanBinding50KG range{0,1,1,index,textures->textureCount()};
    if(same(pass.microprogram,"ScreenObject")){assets::OriginalUiNativeMaterial50KG policy{};
        return assets::prepareOriginalUiNativeMaterial50KG(pass,*texture,policy,d.materialDetail)&&
            convertOriginalUiMaterialSpan50KG(policy,range,output);}
    if(same(pass.textureName,"02_shdw")||same(pass.textureName,"03_shdw")||same(pass.textureName,"04_shdw")||
       same(pass.textureName,"07_shdw")||same(pass.textureName,"10_shdw")||same(pass.textureName,"11_shdw")||
       same(pass.textureName,"12_shdw")||same(pass.textureName,"15_shdw")||
       same(pass.textureName,"19_shdw")||same(pass.textureName,"20_shdw")||
       same(pass.textureName,"09_shdw")||same(pass.textureName,"23_shdw")||same(pass.textureName,"24_shdw")||
       same(pass.textureName,"01_shdw")||same(pass.textureName,"08_shdw")||same(pass.textureName,"14_shdw")||same(pass.textureName,"16_shdw")||same(pass.textureName,"17_shdw")||
       same(pass.textureName,"18_shdw")||same(pass.textureName,"21_shdw")||same(pass.textureName,"22_shdw")||same(pass.textureName,"25_shdw")){assets::OriginalUiRider19ShadowMaterial50KM policy{};
        return assets::prepareOriginalUiTournamentPodShadowMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19ShadowMaterialSpan50KM(policy,images,range,output);}
    if(same(pass.microprogram,"SpecularMapCompressed")){assets::OriginalUiRider19SpecularMaterial50KM policy{};
        return assets::prepareOriginalUiTournamentPodSpecularMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19SpecularMaterialSpan50KM(policy,images,range,output);}
    if(pass.gs[assets::OriginalUiGsField50KG::onePassAntiAliasingOnOff].value==1){
        assets::OriginalUiRider19IpumMaterial50KM policy{};
        return assets::prepareOriginalUiAa1IpumMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19IpumMaterialSpan50KM(policy,images,range,output);}
    if(texture->kind==2){assets::OriginalUiRider19IpumMaterial50KM policy{};
        return assets::prepareOriginalUiRider19IpumMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19IpumMaterialSpan50KM(policy,images,range,output);}
    if(same(pass.textureName,"17_3")){assets::OriginalUiRider17CutoutMaterial50KM policy{};
        return assets::prepareOriginalUiTournamentPodCutoutMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider17CutoutMaterialSpan50KM(policy,images,range,output);}
    if(texture->mips.size()>1){
        if(same(pass.textureName,"pod_wndw")||same(pass.textureName,"12c_4")){assets::OriginalUiRiderWindowMaterial50KM policy{};
            return assets::prepareOriginalUiTournamentPodWindowMaterial50KM(pass,images,index,policy,d.materialDetail)&&
                convertOriginalUiRiderWindowMaterialSpan50KM(policy,images,range,output);}
        assets::OriginalUiRider19BodyMaterial50KM policy{};
        return assets::prepareOriginalUiTournamentPodBodyMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19BodyMaterialSpan50KM(policy,images,range,output);
    }
    assets::OriginalUiLitMaterial50KM policy{};
    return assets::prepareOriginalUiLitMaterial50KM(pass,*texture,policy,d.materialDetail)&&
        convertOriginalUiLitMaterialSpan50KM(policy,range,output);
}
}
struct OriginalUiTournamentDrawContribution50KG::Storage {
    std::shared_ptr<OriginalUiTournamentWidgetResources50KG> resources;
    std::shared_ptr<OriginalUiTournamentCareerResources50KG> career;
    std::shared_ptr<OriginalUiTournamentWidgets50KG> widgets;
    std::shared_ptr<game::OriginalUiTournamentPodOwner50KG> backend;
    std::shared_ptr<game::OriginalUiTournamentCameraOwner50KG> camera;
    const game::OriginalUiTournamentCameraFrame50KG* cameraFrame=nullptr;
    std::shared_ptr<const game::OriginalUiTransitionOwner50KG> transition;
    const game::OriginalUiApertureOwner50KG* aperture=nullptr;
    game::OriginalUiApertureSnapshot50KG apertureAtBegin{};
    bool exitExpected=false,exitProjected=false;
    std::uint32_t widgetOwner=0,careerOwner=0,modelOwner=0,binderOwner=0;
    game::OriginalUiTextureAnimation50KG markerAnimation{};
    std::uint32_t markerControllerStamp=0xffffffffu;bool markerAnimatesWhilePaused=false;
    bool markerMutatedInFrame=false,markerTerminal=false;
    game::OriginalUiLitGeometryWorkspace50KM workspace{};
    game::OriginalUiTextLayout50KG textScratch{};
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;
    std::uint64_t bankGeneration=0,publication=0,frameGeneration=0;
    std::uint64_t registryGeneration=0;
    std::uint64_t acknowledgedGeneration=0,attemptedGeneration=0;
    const VehicleWindowGl50DS* submittedRenderer=nullptr;
    std::optional<std::uint64_t> retiredRendererGeneration;
    std::uint64_t lifecycleSequence=0,groupingFrontier=0;
    bool awaitingGroupRecord=false,groupsValid=false;
    std::vector<MaterialObservation> materialObservations;
    std::vector<PendingLiveMaterial> pendingLive;
    std::vector<Binding> bindings;
    std::vector<Registration> history,pendingHistory;
    std::vector<std::uint32_t> representatives;
    std::vector<MaterialTextureSampler50EB> samplers;
    std::vector<assets::Vertex48B> vertices,orderedVertices,scratchVertices;
    std::vector<float> w,fog,orderedW,orderedFog,scratchW,scratchFog;
    std::vector<Packet> packets;
    std::vector<std::uint32_t> order;
    std::vector<OriginalUiSpan50KG> spans;
    std::vector<OriginalUiNativeSpan50KG> states;
    game::OriginalUiRiderRenderContext50KG render{};
    OriginalUiTournamentDrawClock50KG clock{};
    OriginalUiNativeDraw50KG draw{};
    bool active=false,ordinary=false,page=false,pageShown=false,prompts=false,top=false,finished=false,failed=false;
    bool submissionPending=false,historyPoisoned=false;
    std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG> predecessor;
    bool predecessorRetirementIssued=false;
    std::uint8_t apertures=0;
    std::uint32_t find(const assets::OriginalUiMaterialPass50KG* pass,std::uint32_t image)const noexcept {
        for(std::size_t i=0;i<bindings.size();++i)if(bindings[i].sourceRegistered&&bindings[i].leaf!=unresolved&&bindings[i].pass==pass&&bindings[i].image==image)
            return drawablePacket(static_cast<std::uint32_t>(i))?static_cast<std::uint32_t>(i):unresolved;
        return unresolved;
    }
    bool drawablePacket(std::uint32_t index)const noexcept {
        if(index>=bindings.size()||!bindings[index].prototype)return false;
        const auto leaf=bindings[index].leaf;
        return leaf<representatives.size()&&representatives[leaf]<bindings.size()&&
            bindings[representatives[leaf]].sourceRegistered&&bool(bindings[representatives[leaf]].prototype);
    }
    bool capacity(std::size_t quads)const noexcept {
        return quads<=originalUiMaximumSourceQuads50KM&&vertices.size()/4<=originalUiMaximumSourceQuads50KM-quads&&packets.size()<originalUiMaximumSpans50KG;
    }
    bool add(const std::string& name,const assets::OriginalUiMaterialPass50KG& pass,
        const assets::OriginalUiScreenAssets50KG& source,std::uint32_t image,
        const assets::OriginalUiImageBankMapping50KG* map,std::vector<Binding>& out,D& d,
        RegistrationKind kind=RegistrationKind::existingStatic)const {
        Binding b;b.registeredName=name;b.pass=&pass;b.kind=kind;
        b.sourceRegistered=kind==RegistrationKind::existingStatic;
        if(kind==RegistrationKind::ledgerDynamic&&!backend->canonicalMaterialRow(name,b.row))return fail(d,E::schedule,0x298C80);
        if(pass.textureCount){
            const auto* from=source.sourceProvenance(image);
            const auto* to=map?bank->images()->sourceProvenance(map->combinedIndex):nullptr;
            const auto* sourceImage=source.textures()?source.textures()->texture(image):nullptr;
            const auto* combinedImage=map?bank->images()->textures()->texture(map->combinedIndex):nullptr;
            if(!from||!to||!sourceImage||!combinedImage||from->memberIdentity!=to->memberIdentity||
                from->celIdentity!=to->celIdentity||from->pstOffset!=to->pstOffset||from->pstBytes!=to->pstBytes||
                map->sourceIdentity!=sourceImage->identity||map->combinedIdentity!=combinedImage->identity)return fail(d,E::bank);
            b.image=map->combinedIndex;b.provenance=to;
        }else if(map||image!=unresolved)return fail(d,E::material);
        OriginalUiMaterialSpan50KG drawable;
        if(!material(pass,*bank->images(),b.image,drawable,d))return fail(d,E::material);
        b.resourceSampler=drawable.native.sampler;b.prototype=std::move(drawable);
        out.push_back(std::move(b));return true;
    }
    // Defined below after source-backed pass key preparation. This always
    // resolves actual registry/pass barriers, never an arbitrary caller rank.
    bool schedule(std::vector<Binding>&,const std::vector<Registration>&,
        std::vector<Registration>&,std::vector<std::uint32_t>&,D&)const;
    bool registrationCurrent(const Registration& record)const noexcept {
        if(!record.state.initialized())return false;
        if(record.kind==RegistrationKind::existingStatic)return !record.ledger&&!record.row.prepared()&&!record.epoch&&!record.live;
        const auto& ledger=backend->materialLifetime();game::OriginalUiRiderMaterialRowState50KG state;
        return record.ledger==&ledger&&record.live&&record.live->ledger==&ledger&&record.epoch&&
            record.live->row.same(record.row)&&record.live->epoch==record.epoch&&
            ledger.rowState(record.row,state)&&state.ownerPresence&&state.epoch==record.epoch&&
            backend->materialInstance(record.row,record.epoch)==record.live&&
            record.pass&&record.pass<=record.live->sources.size()&&
            record.live->sources.size()==record.live->states.size()&&
            record.source&&record.live->sources[record.pass-1]&&
            compatible(*record.source,*record.live->sources[record.pass-1]);
    }
};

OriginalUiTournamentDrawContribution50KG::OriginalUiTournamentDrawContribution50KG() noexcept=default;
OriginalUiTournamentDrawContribution50KG::~OriginalUiTournamentDrawContribution50KG(){
    drain();
    // Keep every exact lifetime pin on an unresolved actual renderer borrow.
    // The owning Session must retry retirement, never destruct after failure.
    if(storage_&&storage_->submittedRenderer)(void)storage_.release();
}
bool OriginalUiTournamentDrawContribution50KG::staticRows(const OriginalUiTournamentWidgetResources50KG& widgets,
 const OriginalUiTournamentCareerResources50KG& career,std::vector<MaterialRow>& out,D& d) noexcept {
 d={};if(!widgets.loaded()||!career.loaded()||!widgets.bankInput())return fail(d,E::resource);
 const assets::OriginalUiArchive50KG* app=nullptr;
 for(const auto& a:widgets.bankInput()->archives)if(a&&a->identity()==assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::appStart))app=a.get();
 if(!app)return fail(d,E::resource);
 try {std::vector<MaterialRow> next;game::OriginalUiRiderMaterialLifetimeDiagnostic50KG detail;
  const auto add=[&](const std::string& name){MaterialRow row;if(!MaterialRow::prepare(*app,name,row,detail))return false;
   for(const auto& old:next)if(old.same(row))return true;next.push_back(row);return true;};
  for(const auto& m:*widgets.materials())if(!add(m.registeredName))return fail(d,E::schedule,detail.sourcePC);
  for(const auto& m:*career.materials())if(!add(m.registeredName))return fail(d,E::schedule,detail.sourcePC);
  std::sort(next.begin(),next.end(),[](const auto& a,const auto& b){return a.rank()<b.rank();});out=std::move(next);return true;
 }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiTournamentDrawContribution50KG::bind(std::shared_ptr<OriginalUiTournamentWidgetResources50KG> resources,
 std::shared_ptr<OriginalUiTournamentCareerResources50KG> career,std::shared_ptr<OriginalUiTournamentWidgets50KG> widgets,
 std::shared_ptr<game::OriginalUiTournamentPodOwner50KG> pod,std::shared_ptr<game::OriginalUiTournamentCameraOwner50KG> camera,
 std::shared_ptr<const game::OriginalUiTransitionOwner50KG> transition,const game::OriginalUiApertureOwner50KG* aperture,D& d) noexcept {
 d={};if(storage_)return fail(d,E::alreadyBound);
 if(!resources||!resources->loaded()||!career||!career->loaded()||!widgets||widgets->poisoned()||!pod||!pod->podAssets()||
    pod->podAssets().get()!=career->pod()||!camera||!camera->archive()||!transition||!aperture||!aperture->snapshot().bound)
    return fail(d,E::input);
 try {auto s=std::make_unique<Storage>();s->resources=std::move(resources);s->career=std::move(career);s->widgets=std::move(widgets);
  s->backend=std::move(pod);s->camera=std::move(camera);s->transition=std::move(transition);s->aperture=aperture;
  for(auto* v:{&s->vertices,&s->orderedVertices,&s->scratchVertices})v->reserve(originalUiMaximumSourceQuads50KM*4u);
  for(auto* v:{&s->w,&s->fog,&s->orderedW,&s->orderedFog,&s->scratchW,&s->scratchFog})v->reserve(originalUiMaximumSourceQuads50KM*4u);
  s->packets.reserve(originalUiMaximumSpans50KG);s->order.reserve(originalUiMaximumSpans50KG);
  s->spans.reserve(originalUiMaximumSpans50KG);s->states.reserve(originalUiMaximumSpans50KG);
  storage_=std::move(s);
  if(!storage_->backend->bindMaterialConsumer(this,this,[](void* self,bool rebuild) noexcept {
      D detail;return static_cast<OriginalUiTournamentDrawContribution50KG*>(self)->consumeMaterials(rebuild,detail);})){
      storage_.reset();return fail(d,E::phase);}
  return true;
 }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiTournamentDrawContribution50KG::prepareBank(D& d) noexcept {
 d={};if(!storage_||storage_->bank||storage_->active)return fail(d,E::phase);auto& s=*storage_;
 try {assets::OriginalUiImageBankInput50KG inputs;
  const auto addInput=[&](const assets::OriginalUiImageBankInput50KG* input,std::uint32_t& first){
   if(!input)return false;first=static_cast<std::uint32_t>(inputs.owners.size());
   for(const auto& a:input->archives){bool found=false;for(const auto& b:inputs.archives)if(a==b)found=true;if(!found)inputs.archives.push_back(a);}
   inputs.owners.insert(inputs.owners.end(),input->owners.begin(),input->owners.end());
   if(input->ipumCache){if(inputs.ipumCache&&inputs.ipumCache!=input->ipumCache)return false;inputs.ipumCache=input->ipumCache;}return true;};
  if(!addInput(s.resources->bankInput(),s.widgetOwner)||!addInput(s.career->bankInput(),s.careerOwner)||
     !addInput(s.backend->bankInput(),s.modelOwner)||s.backend->bankInput()->owners.size()!=2)return fail(d,E::bank);
  s.binderOwner=s.modelOwner+1;
  auto bank=std::make_shared<assets::OriginalUiImageBank50KG>();if(!bank->load(inputs,d.bank))return fail(d,E::bank);
  s.bank=bank;struct Rollback{Storage& s;bool done=false;~Rollback(){if(!done)s.bank.reset();}} rollback{s};
  std::vector<Binding> bindings;
  for(const auto& m:*s.resources->materials()){
   if(m.source.passes.size()!=1)return fail(d,E::material);
   if(!s.add(m.registeredName,m.source.passes[0],*s.resources->images(),m.image,
       m.image==unresolved?nullptr:bank->mapping(s.widgetOwner,m.image),bindings,d))return false;
  }
  const auto& careers=*s.career->materials();for(std::uint32_t i=0;i<careers.size();++i){const auto& m=careers[i];
   if(m.source.passes.size()!=1||!s.add(m.registeredName,m.source.passes[0],*s.career->images(),i,
      bank->mapping(s.careerOwner,i),bindings,d))return fail(d,E::material);
  }
  const auto materials=s.backend->materials();const auto pod=s.backend->podAssets();const auto* scene=pod?pod->scene():nullptr;
  if(!materials||!scene||materials->materialCount()!=scene->materials.size())return fail(d,E::model);
  for(std::uint32_t m=0;m<materials->materialCount();++m)for(std::uint32_t p=0;p<materials->passCount(m);++p){
   const auto* pass=materials->pass(m,p);if(!pass||!s.add("ui/textures/"+scene->materials[m],pass->source,*materials->images(),pass->imageIndex,
      pass->imageIndex==unresolved?nullptr:bank->mapping(s.modelOwner,pass->imageIndex),bindings,d,RegistrationKind::ledgerDynamic))return fail(d,E::material);
  }
  const auto binder=s.backend->binderAssets();if(!binder||!binder->images()||!binder->images()->textures()||!binder->pass())return fail(d,E::resource);
  for(std::uint32_t i=0;i<binder->images()->textures()->textureCount();++i)
   if(!s.add("ui/textures/binder",*binder->pass(),*binder->images(),i,bank->mapping(s.binderOwner,i),bindings,d,RegistrationKind::ledgerDynamic))return false;
  std::vector<Registration> history;std::vector<std::uint32_t> representatives;
  if(!s.schedule(bindings,{},history,representatives,d))return false;
  std::vector<MaterialTextureSampler50EB> samplers(bank->images()->textures()->textureCount());std::vector<bool> set(samplers.size(),false);
  for(const auto& binding:bindings)if(binding.image!=unresolved){if(binding.image>=samplers.size()||
      (set[binding.image]&&!samplerSame(samplers[binding.image],binding.resourceSampler)))return fail(d,E::material);
    set[binding.image]=true;samplers[binding.image]=binding.resourceSampler;}
  for(bool value:set)if(!value)return fail(d,E::material);
  assets::OriginalUiPageDiagnostic50KG animationDetail;
  if(!assets::originalUiBoolean50KG((*s.career->materials())[15].source.passes[0].authored,"animatesWhilePaused",false,
     s.markerAnimatesWhilePaused,animationDetail,0x29B950))return fail(d,E::clock,0x29B950);
  if(!s.career->claimNativeMarker(this)||!s.career->nativeMarkerConfig(this)||
     !game::initializeOriginalUiTextureAnimation50KG(s.markerAnimation,*s.career->nativeMarkerConfig(this),9))return fail(d,E::clock);
  s.bindings=std::move(bindings);s.samplers=std::move(samplers);s.history=std::move(history);s.representatives=std::move(representatives);
  s.bankGeneration=1;s.groupsValid=true;s.registryGeneration=s.backend->registryGeneration();rollback.done=true;return true;
 }catch(...){return fail(d,E::allocation);}
}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiTournamentDrawContribution50KG::bank()const noexcept{return storage_?storage_->bank:nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiTournamentDrawContribution50KG::samplers()const noexcept{return storage_&&storage_->bank?&storage_->samplers:nullptr;}
bool OriginalUiTournamentDrawContribution50KG::bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB> gpu,D& d) noexcept {
 d={};if(!storage_||storage_->active||frame_.native_||storage_->gpu||storage_->submissionPending||storage_->submittedRenderer||!bank()||!gpu||!gpu->initialized()||!gpu->publicationToken()||gpu->riderSparse())return fail(d,E::publication);
 const auto* textures=bank()->images()->textures();if(gpu->textureCount()!=textures->textureCount())return fail(d,E::publication);
 for(std::uint32_t i=0;i<gpu->textureCount();++i){const auto* a=gpu->asset(i);const auto* b=textures->texture(i);
  if(!a||!b||!a->name||a->identity!=b->identity)return fail(d,E::publication);}
 storage_->gpu=std::move(gpu);storage_->publication=storage_->gpu->publicationToken();return true;
}
bool OriginalUiTournamentDrawContribution50KG::begin(const OriginalUiTournamentDrawClock50KG& clock,
 const game::OriginalUiTournamentCameraFrame50KG& camera,D& d) noexcept {
 d={};if(!storage_||!clock.known)return fail(d,E::input);auto& s=*storage_;
 game::OriginalApplicationAnimationClockSample50KG actual;
 if(!clock.applicationAnimation.read(actual)||actual.current230!=clock.milliseconds||camera.applicationCurrent230()!=actual.current230||
    clock.advanceAnimations!=(!actual.pause11C))return fail(d,E::clock);
 if(s.active||frame_.native_||s.historyPoisoned||s.markerTerminal||s.submissionPending||!s.groupsValid||s.awaitingGroupRecord||!s.career->current()||
    s.lifecycleSequence!=s.backend->materialLifetime().sequence()||s.registryGeneration!=s.backend->registryGeneration()||
    !s.camera->isCurrent(camera)||!s.bank||!s.gpu||!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,E::publication);
 const auto life=s.transition->snapshot();const auto aperture=s.aperture->snapshot();
 if(!life.bound||life.current.screen!=9||life.current.page!=1||!aperture.bound||!s.widgets->dynamicComplete()||s.widgets->poisoned())return fail(d,E::phase);
 if(!s.backend->beginFrame())return fail(d,E::phase);
 s.clock=clock;s.cameraFrame=&camera;s.render=camera.renderContext();s.apertureAtBegin=aperture;s.exitProjected=false;
 s.exitExpected=life.lifecycle==game::OriginalUiTransitionLifecycle50KG::waitScreenExit&&
    aperture.state==game::OriginalUiApertureState50KG::closing&&aperture.top284>=224&&aperture.bottom28C<=224;
 s.active=true;s.markerMutatedInFrame=false;s.ordinary=s.page=s.pageShown=s.prompts=s.top=s.finished=s.failed=false;s.apertures=0;
 s.vertices.clear();s.w.clear();s.fog.clear();s.packets.clear();s.order.clear();s.spans.clear();s.states.clear();return true;
}
bool OriginalUiTournamentDrawContribution50KG::append(const game::OriginalUiOverlayQuad50KG& quad,
 const assets::OriginalUiMaterialPass50KG* pass,std::uint32_t image,float z,D& d) noexcept {
 auto& s=*storage_;if(!pass||!std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z||quad.sourcePacketColor[3]>128)return fail(d,E::geometry);
 const auto binding=s.find(pass,image);if(binding==unresolved)return fail(d,E::material);if(!s.capacity(1))return fail(d,E::capacity);
 std::array<assets::Vertex48B,4> vertices{};constexpr unsigned order[]={0,1,3,2};
 for(unsigned i=0;i<4;++i){const auto& v=quad.vertices[order[i]];if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v))return fail(d,E::geometry);
  auto& o=vertices[i];o.position[0]=v.x;o.position[1]=v.y;o.position[2]=z;o.uv[0]=v.u;o.uv[1]=v.v;for(unsigned c=0;c<4;++c)o.color[c]=quad.sourcePacketColor[c];}
 const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);s.vertices.insert(s.vertices.end(),vertices.begin(),vertices.end());
 s.w.insert(s.w.end(),4,1);s.fog.insert(s.fog.end(),4,0);s.packets.push_back({first,1,binding,static_cast<std::uint32_t>(s.packets.size())});return true;
}
bool OriginalUiTournamentDrawContribution50KG::appendText(const game::OriginalUiTextLayout50KG& text,const std::array<std::uint8_t,4>& color,
 const assets::OriginalUiMaterialPass50KG* pass,std::uint32_t image,D& d) noexcept {
 auto& s=*storage_;if(text.count>text.quads.size()||color[3]>128)return fail(d,E::geometry);const auto b=s.find(pass,image);
 if(b==unresolved)return fail(d,E::material);if(!text.count)return true;if(!s.capacity(text.count))return fail(d,E::capacity);
 for(std::size_t q=0;q<text.count;++q)for(const auto& v:text.quads[q].vertices)
  if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v))return fail(d,E::geometry);
 const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
 for(std::size_t q=0;q<text.count;++q)for(const auto& v:text.quads[q].vertices){assets::Vertex48B o{};
  o.position[0]=v.x;o.position[1]=v.y;o.position[2]=16777214.f;o.uv[0]=v.u;o.uv[1]=v.v;for(unsigned c=0;c<4;++c)o.color[c]=color[c];s.vertices.push_back(o);}
 s.w.insert(s.w.end(),text.count*4,1);s.fog.insert(s.fog.end(),text.count*4,0);s.packets.push_back({first,text.count,b,static_cast<std::uint32_t>(s.packets.size())});return true;
}
bool OriginalUiTournamentDrawContribution50KG::model(std::uint16_t id,D& d) noexcept {
 auto& s=*storage_;if(id!=s.backend->widgetId()||!s.backend->attached()||!s.backend->shown())return fail(d,E::model);
 game::OriginalUiTournamentPodPacket50KG selected;
 if(!s.cameraFrame||!s.backend->prepareSelected(*s.camera,*s.cameraFrame,selected,d.pod))return fail(d,E::model);
 if(selected.emitsGeometry){const auto& packet=selected.model;const auto materials=s.backend->materials();
  if(!packet.scene||!materials||packet.scene->lods.size()!=1)return fail(d,E::geometry);
  const auto& surfaces=packet.scene->lods[0].surfaces;
  for(std::uint32_t surface=0;surface<surfaces.size();++surface){const auto m=surfaces[surface].material_index;
   if(m>=materials->materialCount()||surfaces[surface].indices.size()%3)return fail(d,E::geometry);
   const auto count=surfaces[surface].indices.size()/3;if(!count)continue;if(count>originalUiMaximumSourceQuads50KM)return fail(d,E::capacity);
   s.scratchVertices.resize(count*4);s.scratchW.resize(count*4);s.scratchFog.resize(count*4);
   for(std::uint32_t p=0;p<materials->passCount(m);++p){d.material=m;d.pass=p+1;const auto* material=materials->pass(m,p);if(!material)return fail(d,E::material);
    const auto* map=material->imageIndex==unresolved?nullptr:s.bank->mapping(s.modelOwner,material->imageIndex);
    if(material->imageIndex!=unresolved&&!map)return fail(d,E::bank);
    const auto binding=s.find(&material->source,map?map->combinedIndex:unresolved);if(binding==unresolved)return fail(d,E::material);
    game::OriginalUiLitSurfaceOutput50KM projected{s.scratchVertices.data(),s.scratchW.data(),s.scratchFog.data(),static_cast<std::uint32_t>(count),0,0};
    const auto status=same(material->source.microprogram,"SpecularMapCompressed")?
      game::projectOriginalUiSpecularMapCompressedSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,projected,s.workspace):
      game::projectOriginalUiLitSceneSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,packet.fog.computeRadialFog1B2,projected,s.workspace);
    if(status!=game::OriginalUiLitGeometryStatus50KM::complete||projected.materialIndex!=m)return fail(d,E::geometry);
    if(!s.capacity(projected.slotCount))return fail(d,E::capacity);if(!projected.slotCount)continue;
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);const auto n=std::size_t(projected.slotCount)*4;
    s.vertices.insert(s.vertices.end(),s.scratchVertices.begin(),s.scratchVertices.begin()+n);s.w.insert(s.w.end(),s.scratchW.begin(),s.scratchW.begin()+n);
    s.fog.insert(s.fog.end(),s.scratchFog.begin(),s.scratchFog.begin()+n);s.packets.push_back({first,projected.slotCount,binding,static_cast<std::uint32_t>(s.packets.size())});
   }
  }
 }
 // Genuine helper children run even when selected.emitsGeometry is false.
 for(std::size_t pair=0;pair<2;++pair)if(s.backend->binderPresent(pair)){
  game::OriginalUiTournamentBinderPacket50KG packet;
  if(!s.backend->projectBinder(pair,*s.camera,*s.cameraFrame,s.clock.milliseconds,s.clock.advanceAnimations,packet,d.pod))return fail(d,E::model);
  const auto* map=s.bank->mapping(s.binderOwner,packet.image.imageIndex);if(!map)return fail(d,E::bank);
  const auto binding=s.find(packet.image.pass,map->combinedIndex);if(binding==unresolved)return fail(d,E::material);if(!s.capacity(2))return fail(d,E::capacity);
  const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);const auto& p=packet.geometry;
  s.vertices.insert(s.vertices.end(),p.vertices.begin(),p.vertices.end());s.w.insert(s.w.end(),p.rasterW.begin(),p.rasterW.end());s.fog.insert(s.fog.end(),p.fogFactors.begin(),p.fogFactors.end());
  s.packets.push_back({first,2,binding,static_cast<std::uint32_t>(s.packets.size())});
 }
 return true;
}
bool OriginalUiTournamentDrawContribution50KG::widgets(bool top,bool shown,D& d) noexcept {
 auto& s=*storage_;const auto* order=s.widgets->order(top);if(!order)return fail(d,E::widget);const auto& materials=*s.resources->materials();
 for(auto id:*order){d.widget=id;const auto state=s.widgets->state(id);if(!shown||!state.shown)continue;
  if(state.kind==OriginalUiTournamentWidgetKind50KG::userRotate){if(!model(id,d))return false;continue;}
  const auto m=s.resources->widgetMaterial(id);
  if(const auto* descriptor=s.widgets->textDescriptor(id)){
   const auto* geometry=s.widgets->textGeometry(id);if(!geometry||m>=materials.size()||descriptor->depth!=1)return fail(d,E::resource);
   const auto& material=materials[m];const auto* map=s.bank->mapping(s.widgetOwner,material.image);if(!map)return fail(d,E::bank);
   for(std::size_t line=0;line<geometry->lines().size();++line){const auto& g=geometry->lines()[line];OriginalUiTournamentTextProjection50KG live;
    if(!s.widgets->projectText(id,line,g.glyphs.fontLineHeight,g.measuredWidth,g.measuredHeight,shown,live,d.widgetDetail)){
     // The source callback was reached. A stopped TextOverlay legitimately
     // emits nothing even when its widget remains shown (reset does not restart it).
     if(d.widgetDetail.error==OriginalUiTournamentWidgetsError50KG::text&&
        d.widgetDetail.text==game::OriginalUiTextWidgetStatus50KG::hidden)continue;
     return fail(d,E::widget);
    }
    if(live.descriptor!=descriptor||live.line.scale!=1||live.line.rotation!=0||live.position.positionedX!=g.position.positionedX||
       live.position.positionedY!=g.position.positionedY)return fail(d,E::geometry);
    if(!appendText(g.glyphs,live.line.packetColor,&material.source.passes[0],map->combinedIndex,d))return false;
   }
  }else if(state.kind==OriginalUiTournamentWidgetKind50KG::icon){OriginalUiTournamentIconProjection50KG live;
   if(!s.widgets->projectIcon(id,shown,live,d.widgetDetail)||m>=materials.size()||live.images!=s.resources->images().get()||live.imageIndex!=materials[m].image)return fail(d,E::widget);
   const auto* map=s.bank->mapping(s.widgetOwner,live.imageIndex);if(!map||!append(live.layout.quad,&materials[m].source.passes[0],map->combinedIndex,live.layout.packetDepth,d))return false;
  }
 }
 return true;
}
bool OriginalUiTournamentDrawContribution50KG::projectOrdinaryWidgets(bool shown,D& d) noexcept {
 d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;if(!s.active||s.ordinary||s.page||s.failed)return fail(d,E::phase);
 if(!widgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiTournamentDrawContribution50KG::projectCareer(std::uint8_t popup,bool shown,D& d) noexcept {
 d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;if(!s.active||!s.ordinary||s.page||s.failed)return fail(d,E::phase);
 s.page=true;s.pageShown=shown;if(!shown)return true;
 // This fresh-career owner has the real hidden popup. A shown popup requires
 // its actual dispatch owner, not silently omitted popup geometry.
 if(popup&1u){s.failed=true;return fail(d,E::phase,0x1F2730);}
 struct Guard{Storage& s;bool done=false;~Guard(){if(!done){s.failed=true;if(s.markerMutatedInFrame)s.markerTerminal=true;}}}guard{s};
 const auto* bg=s.career->backgroundPackets(popup);const auto* portrait=s.career->portraitPacket(popup);
 const auto* stats=s.career->statPackets(popup);const auto* medals=s.career->medalPackets(popup);const auto* numbers=s.career->numbers();
 if(!bg||!portrait||!stats||!medals||!numbers)return fail(d,E::resource);
 const auto appendPacket=[&](const OriginalUiTournamentMedalPacket50KG& p){const auto* map=s.bank->mapping(s.careerOwner,p.image);
  return map&&p.image<s.career->materials()->size()&&append(p.sprite,&(*s.career->materials())[p.image].source.passes[0],map->combinedIndex,16777215.f-p.sprite.authoredDepth,d);};
 for(const auto& p:*bg)if(!appendPacket(p))return false;
 if(!appendPacket(*portrait))return false;
 for(const auto& p:*stats)if(!appendPacket(p))return false;
 for(const auto& p:*medals)if(!appendPacket(p))return false;
 //2DF550..2DF864 prepares the ScreenTexture geometry BEFORE246760 calls
 //24A740/29AC90. Build the exact fixed64x64 source vertices first; only then
 //service this retained unique prototype and enqueue the selected-frame bind.
 const auto* geometry=s.career->nativeMarkerPacket(this,0);if(!geometry||!s.capacity(1))return fail(d,E::capacity);
 std::array<assets::Vertex48B,4> markerVertices{};constexpr unsigned corner[]{0,1,3,2};
 for(unsigned i=0;i<4;++i){const auto& v=geometry->sprite.vertices[corner[i]];auto& out=markerVertices[i];
  if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v))return fail(d,E::geometry);
  out.position[0]=v.x;out.position[1]=v.y;out.position[2]=16777215.f-geometry->sprite.authoredDepth;
  out.uv[0]=v.u;out.uv[1]=v.v;for(unsigned c=0;c<4;++c)out.color[c]=geometry->sprite.sourcePacketColor[c];
 }
 game::OriginalApplicationAnimationClockSample50KG entry,leaf,leafCommit,controllerCommit;std::uint32_t frame=0;
 if(!s.clock.applicationAnimation.read(entry))return fail(d,E::clock,0x29ACA8);
 if(s.markerControllerStamp!=entry.current230&&(!entry.pause11C||s.markerAnimatesWhilePaused)){
  if(!s.clock.applicationAnimation.read(leaf))return fail(d,E::clock,0x29A118);
  auto next=s.markerAnimation;
  if(!game::advanceOriginalUiTextureAnimation50KG(next,leaf.current230,frame))return fail(d,E::clock,0x29A020);
  // This is the source leaf's first actual mutation. A later failure may not
  // replay this service through a reset/retry; keep the changed state intact.
  s.markerAnimation=next;s.markerMutatedInFrame=true;
  if(!s.career->nativeMarkerPacket(this,frame))return fail(d,E::clock,0x29A020);
  if(!s.clock.applicationAnimation.read(leafCommit))return fail(d,E::clock,0x29A16C);
  s.markerAnimation.previousMilliseconds=leafCommit.current230;
  if(!s.clock.applicationAnimation.read(controllerCommit))return fail(d,E::clock,0x29AD48);
  s.markerControllerStamp=controllerCommit.current230;
 }else if(!game::selectOriginalUiTextureAnimationFrame50KG(s.markerAnimation,frame))return fail(d,E::clock,0x29AF90);
 const auto* marker=s.career->nativeMarkerPacket(this,frame);const auto* markerMap=marker?s.bank->mapping(s.careerOwner,marker->image):nullptr;
 const auto markerBinding=markerMap?s.find(&(*s.career->materials())[marker->image].source.passes[0],markerMap->combinedIndex):unresolved;
 if(markerBinding==unresolved)return fail(d,E::material,0x24B278);
 const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
 s.vertices.insert(s.vertices.end(),markerVertices.begin(),markerVertices.end());s.w.insert(s.w.end(),4,1);s.fog.insert(s.fog.end(),4,0);
 s.packets.push_back({first,1,markerBinding,static_cast<std::uint32_t>(s.packets.size())});
 const auto* map=s.bank->mapping(s.careerOwner,0);if(!map)return fail(d,E::bank);
 for(const auto& n:*numbers)if(!appendText(n.glyphs,game::originalUiTextPacketColor50KG(n.color54,n.alpha58),
    &(*s.career->materials())[0].source.passes[0],map->combinedIndex,d))return false;
 guard.done=true;return true;
}
bool OriginalUiTournamentDrawContribution50KG::projectExitText(const game::OriginalUiApertureHook50KG& hook,D& d) noexcept {
 d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
 if(!s.active||!s.page||!s.pageShown||s.apertures||s.prompts||s.top||s.failed||s.exitProjected||!s.exitExpected||
    hook.observed.state!=s.apertureAtBegin.state||hook.observed.top284!=s.apertureAtBegin.top284||hook.observed.bottom28C!=s.apertureAtBegin.bottom28C||!s.capacity(10))return fail(d,E::phase,0x1D75A4);
 auto* exit=s.resources->exitText();game::OriginalUiExitTextProjection50KG live;const auto& material=(*s.resources->materials())[0];
 const auto* map=s.bank->mapping(s.widgetOwner,material.image);
 if(!exit||!map||exit->projectRender2E0170(hook,live)!=game::OriginalUiExitTextStatus50KG::ok||!live.glyphs||
    live.images!=s.resources->images().get()||live.imageIndex!=material.image||live.materialIdentity!=material.source.identity||
    live.assignedDepth!=1||live.glyphs->count>10||!appendText(*live.glyphs,live.packetColor,&material.source.passes[0],map->combinedIndex,d)){
    s.failed=true;return fail(d,E::geometry,0x2E0170);}
 s.exitProjected=true;return true;
}
bool OriginalUiTournamentDrawContribution50KG::projectAperture(std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& rect,D& d) noexcept {
 d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;if(!s.active||!s.page||!s.pageShown||s.prompts||s.top||s.failed||
   ordinal!=s.apertures||ordinal>1||rect.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rect.dirty3C||(s.exitExpected&&!s.exitProjected))return fail(d,E::phase);
 game::OriginalUiOverlayQuad50KG q;q.authoredDepth=1;q.sourcePC=0x1D798C;q.sourcePacketColor={{0,0,0,63}};
 constexpr unsigned order[]={0,2,1,3};for(unsigned i=0;i<4;++i){const auto& v=rect.vertices[order[i]];q.vertices[i]={v.x,v.y,0,0};}
 if(!append(q,&(*s.resources->materials())[11].source.passes[0],unresolved,16777214.f,d)){s.failed=true;return false;}++s.apertures;return true;
}
bool OriginalUiTournamentDrawContribution50KG::projectPrompts(std::uint32_t mask,D& d) noexcept {
 d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;if(!s.active||!s.page||!s.pageShown||s.apertures!=2||s.prompts||s.top||s.failed||(mask&~15u))return fail(d,E::phase);
 // Shared1D7A2C..74 renders cross,triangle,square,circle in source order.
 constexpr unsigned order[]{0,1,2,3};
 for(auto i:order){OriginalUiTournamentPromptPlacement50KG placement;
  if(!originalUiTournamentPromptPlacement50KG(i,placement))return fail(d,E::geometry);
  if(!(mask&placement.mask))continue;const auto& m=(*s.resources->materials())[7+i];const auto* image=s.resources->images()->textures()->texture(m.image);
  const auto* map=s.bank->mapping(s.widgetOwner,m.image);if(!image||!map)return fail(d,E::bank);
  game::OriginalUiOverlayQuad50KG q;q.sourcePC=placement.drawPC;const float w=float(image->texture.width),h=float(image->texture.height);
  q.vertices={{{placement.x,placement.y,0,0},{placement.x+w,placement.y,1,0},{placement.x,placement.y+h,0,1},{placement.x+w,placement.y+h,1,1}}};
  if(!append(q,&m.source.passes[0],map->combinedIndex,16777214.f,d)){s.failed=true;return false;}}
 s.prompts=true;return true;
}
bool OriginalUiTournamentDrawContribution50KG::projectOnTopWidgets(bool shown,D& d) noexcept {
 d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;if(!s.active||!s.ordinary||!s.page||s.top||s.failed||(s.pageShown&&(!s.prompts||s.apertures!=2)))return fail(d,E::phase);
 if(!widgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}
bool OriginalUiTournamentDrawContribution50KG::sourceTeardownPending()const noexcept{return storage_&&storage_->submittedRenderer;}
bool OriginalUiTournamentDrawContribution50KG::retireSourceRenderer(VehicleWindowGl50DS& renderer,D& d) noexcept {
 d={};if(!storage_)return true;auto& s=*storage_;if(s.submissionPending||(s.submittedRenderer&&s.submittedRenderer!=&renderer))return fail(d,E::phase);
 if(s.submittedRenderer){VehicleWindowDiagnostic50DS detail;if(!renderer.retireOriginalUiSourceOwner(*this,detail))return fail(d,E::publication);}
 if(!s.backend->acceptSourceRetirement(this,s.attemptedGeneration,s.backend->materialLifetime().sequence()))return fail(d,E::phase);
 s.retiredRendererGeneration=s.attemptedGeneration;s.submittedRenderer=nullptr;return true;
}

std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG>
OriginalUiTournamentDrawContribution50KG::predecessorInventory(D& d)const noexcept {
 d={};if(!storage_)return {};auto& s=*storage_;
 if(s.predecessor)return s.predecessor;
 if(!s.bank||!s.backend->attached()||s.historyPoisoned||s.submissionPending||!s.groupsValid||s.awaitingGroupRecord||
    s.lifecycleSequence!=s.backend->materialLifetime().sequence()||s.registryGeneration!=s.backend->registryGeneration()){
    fail(d,E::phase);return {};}
 try {auto next=std::shared_ptr<game::OriginalUiTournamentPredecessorInventory50KG>(new game::OriginalUiTournamentPredecessorInventory50KG);
  next->creator_=this;next->bank_=s.bank;next->bankGeneration_=s.bankGeneration;next->registryGeneration_=s.registryGeneration;next->materialSequence_=s.lifecycleSequence;
  for(const auto& binding:s.bindings){if(!binding.sourceRegistered||!binding.pass||binding.registration>=s.history.size()||
       !s.registrationCurrent(s.history[binding.registration])){fail(d,E::schedule);return {};}
   MaterialRow row;if(!s.backend->canonicalMaterialRow(binding.registeredName,row)){fail(d,E::schedule);return {};}
   std::size_t index=0;for(;index<next->passes_.size();++index)if(next->passes_[index].row.same(row)&&next->passes_[index].source.pass==binding.pass->pass)break;
   if(index==next->passes_.size()){game::OriginalUiTournamentPredecessorPass50KG pass;pass.row=row;pass.registeredName=binding.registeredName;pass.source=*binding.pass;next->passes_.push_back(std::move(pass));}
   if(binding.image!=unresolved){const auto* provenance=s.bank->images()->sourceProvenance(binding.image);
    if(!provenance||!binding.pass->textureCount){fail(d,E::bank);return {};}next->passes_[index].images.push_back(*provenance);
   }else if(binding.pass->textureCount){fail(d,E::bank);return {};}
  }
  std::sort(next->passes_.begin(),next->passes_.end(),[](const auto& a,const auto& b){return a.row.rank()!=b.row.rank()?a.row.rank()<b.row.rank():a.source.pass<b.source.pass;});
  if(next->passes_.empty()){fail(d,E::schedule);return {};}
  s.predecessor=std::move(next);return s.predecessor;
 }catch(...){fail(d,E::allocation);return {};}
}
bool OriginalUiTournamentDrawContribution50KG::retireUnsubmittedSource(D& d) noexcept {
 d={};if(!storage_)return true;auto& s=*storage_;
 if(s.active||frame_.native_||s.cameraFrame||s.submissionPending||s.submittedRenderer||
    (s.attemptedGeneration&&(!s.retiredRendererGeneration||*s.retiredRendererGeneration!=s.attemptedGeneration)))return fail(d,E::phase);
 // A retry may reuse ONLY this owner's completed real renderer retirement.
 return s.backend->acceptSourceRetirement(this,s.attemptedGeneration,s.backend->materialLifetime().sequence())?true:fail(d,E::phase);
}
bool OriginalUiTournamentDrawContribution50KG::retireStaticRegistrations(D& d) noexcept {
 d={};if(!storage_)return true;auto& s=*storage_;const auto& lifetime=s.backend->materialLifetime();
 if(s.active||frame_.native_||s.cameraFrame||s.submissionPending||s.submittedRenderer||s.historyPoisoned||
    !s.backend->teardownComplete()||lifetime.hasOwners()||lifetime.pendingEvents()||lifetime.populationActive()||
    lifetime.sequence()!=s.lifecycleSequence||lifetime.sequence()!=lifetime.acknowledgedSequence())return fail(d,E::phase);
 // Dynamic rows must already have passed exact journal retirement. This is
 // the concrete destruction of this collector's remaining static registrations,
 // not a predicate that overlooks them. Immutable source/bank inventory stays pinned.
 for(const auto& record:s.history)if(record.kind!=RegistrationKind::existingStatic||record.live||record.ledger||record.epoch)return fail(d,E::schedule);
 for(const auto& binding:s.bindings)if(binding.live||(binding.kind==RegistrationKind::ledgerDynamic&&binding.sourceRegistered))return fail(d,E::schedule);
 for(const auto& observation:s.materialObservations)if(observation.owners||observation.epoch)return fail(d,E::schedule);
 for(auto& binding:s.bindings){binding.sourceRegistered=false;binding.registration=unresolved;binding.leaf=unresolved;binding.live.reset();}
 s.history.clear();s.pendingHistory.clear();s.pendingLive.clear();s.representatives.clear();s.materialObservations.clear();
 s.groupsValid=false;s.awaitingGroupRecord=false;s.groupingFrontier=0;return true;
}
bool OriginalUiTournamentDrawContribution50KG::retirementComplete()const noexcept {
 if(!storage_)return false;const auto& s=*storage_;const auto& lifetime=s.backend->materialLifetime();
 for(const auto& binding:s.bindings)if(binding.sourceRegistered||binding.live||binding.registration!=unresolved||binding.leaf!=unresolved)return false;
 return !s.active&&!frame_.native_&&!s.cameraFrame&&!s.submissionPending&&!s.submittedRenderer&&!s.historyPoisoned&&
   s.history.empty()&&s.pendingHistory.empty()&&s.pendingLive.empty()&&s.representatives.empty()&&s.materialObservations.empty()&&
   !s.groupsValid&&!s.awaitingGroupRecord&&s.backend->teardownComplete()&&!lifetime.hasOwners()&&!lifetime.pendingEvents()&&!lifetime.populationActive()&&
   lifetime.sequence()==s.lifecycleSequence&&lifetime.sequence()==lifetime.acknowledgedSequence();
}
bool OriginalUiTournamentDrawContribution50KG::retirePredecessorInventory(
 std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG> inventory,
 game::OriginalUiTournamentPredecessorRetirement50KG& output,D& d)const noexcept {
 d={};if(!storage_||!retirementComplete()||!inventory||storage_->predecessor!=inventory||inventory->creator_!=this||
   storage_->predecessorRetirementIssued||output.inventory_||output.creator_||output.consumed_||
   inventory->bank_!=storage_->bank||inventory->bankGeneration_!=storage_->bankGeneration||
   inventory->registryGeneration_>storage_->registryGeneration||inventory->materialSequence_>storage_->lifecycleSequence)return fail(d,E::phase);
 output.inventory_=std::move(inventory);output.creator_=this;output.materialSequence_=storage_->lifecycleSequence;
 output.submissionWatermark_=storage_->attemptedGeneration;storage_->predecessorRetirementIssued=true;return true;
}
bool OriginalUiTournamentDrawContribution50KG::consumeMaterials(bool rebuild,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(s.active||frame_.native_||s.submissionPending||
       (rebuild&&s.historyPoisoned))return fail(d,E::phase);
    const auto& ledger=s.backend->materialLifetime();const auto frontier=ledger.sequence();
    if(!s.bank){
        // A never-prepared cold candidate owns no registry state to consume.
        // This is not permission to skip even one real source journal event.
        if(rebuild||frontier||ledger.acknowledgedSequence()||ledger.pendingEvents()||ledger.hasOwners()||ledger.populationActive()||
           s.lifecycleSequence||s.submittedRenderer||s.attemptedGeneration||s.acknowledgedGeneration||s.cameraFrame||
           !s.bindings.empty()||!s.history.empty()||!s.pendingHistory.empty()||!s.pendingLive.empty()||
           !s.materialObservations.empty()||!s.representatives.empty()||s.groupsValid||s.awaitingGroupRecord)return fail(d,E::phase);
        return true;
    }
    if(frontier<s.lifecycleSequence||ledger.acknowledgedSequence()>s.lifecycleSequence)return fail(d,E::schedule,0x14D370);
    if(rebuild&&(s.awaitingGroupRecord||frontier==s.lifecycleSequence))return fail(d,E::schedule,0x14D440);
    try {
        // No source effect can be undone here. Stage only this consumer's
        // projection; failed validation leaves its cursor and state unchanged.
        auto history=s.history;auto bindings=s.bindings;auto observations=s.materialObservations;
        auto representatives=s.representatives;
        bool identityChanged=false,awaiting=s.awaitingGroupRecord,groupAccepted=false;
        std::uint64_t populationBoundary=0;
        for(auto sequence=s.lifecycleSequence;sequence<frontier;){
            game::OriginalUiRiderMaterialEvent50KG event;
            if(!ledger.event(++sequence,event)||event.sequence!=sequence)return fail(d,E::schedule,0x14D370);
            using Kind=game::OriginalUiRiderMaterialEventKind50KG;
            if(event.kind==Kind::populationBoundary){
                if(awaiting||event.row.prepared())return fail(d,E::schedule,0x14D370);
                for(const auto& observation:observations)if(observation.owners&&!observation.epoch)return fail(d,E::schedule,0x14D370);
                populationBoundary=sequence;continue;
            }
            if(event.kind==Kind::groupRebuilt){
                if(rebuild||!awaiting||sequence!=s.groupingFrontier+1||groupAccepted)return fail(d,E::schedule,0x14D440);
                awaiting=false;groupAccepted=true;continue;
            }
            if(!event.row.prepared())return fail(d,E::schedule,0x298C80);
            bool admitted=false;
            for(const auto& binding:bindings)if(binding.kind==RegistrationKind::ledgerDynamic&&binding.row.same(event.row)){admitted=true;break;}
            if(!admitted)return fail(d,E::schedule,0x298C80);
            std::size_t index=0;for(;index<observations.size();++index)if(observations[index].row.same(event.row))break;
            if(index==observations.size())observations.push_back({event.row,0,0});
            auto& observation=observations[index];
            if(event.kind==Kind::ownerAcquired){
                if(!event.ownerSerial||!event.sourceSlotOccurrences||observation.owners==std::numeric_limits<std::uint32_t>::max()||
                   event.ownerPresenceAfter!=observation.owners+1||event.epoch!=observation.epoch)return fail(d,E::schedule,0x26E7D8);
                observation.owners=event.ownerPresenceAfter;
            }else if(event.kind==Kind::ownerReleased){
                if(!event.ownerSerial||!event.sourceSlotOccurrences||!observation.owners||
                   event.ownerPresenceAfter!=observation.owners-1||event.epoch!=observation.epoch)return fail(d,E::schedule,0x26EB90);
                observation.owners=event.ownerPresenceAfter;
            }else if(event.kind==Kind::rowPopulated){
                if(!observation.owners||observation.epoch||!event.epoch||event.ownerPresenceAfter!=observation.owners)return fail(d,E::schedule,0x14D370);
                const auto live=s.backend->materialInstance(event.row,event.epoch);
                if(!live||live->ledger!=&ledger||!live->row.same(event.row)||live->epoch!=event.epoch||
                   live->sources.empty()||live->sources.size()!=live->states.size())return fail(d,E::schedule,0x14D370);
                for(std::size_t pass=0;pass<live->sources.size();++pass){const auto* source=live->sources[pass];
                    if(!source||source->pass!=pass+1||!live->states[pass].initialized()||live->states[pass].bound())return fail(d,E::schedule,0x14D370);
                    bool alias=false;
                    for(const auto& binding:bindings)if(binding.kind==RegistrationKind::ledgerDynamic&&binding.row.same(event.row)&&
                       binding.pass&&binding.pass->pass==source->pass){
                        if(!compatible(*source,*binding.pass))return fail(d,E::schedule,0x14D370);alias=true;
                    }
                    if(!alias)return fail(d,E::schedule,0x14D370);
                    for(const auto& record:history)if(record.rank==event.row.rank()&&record.pass==source->pass)return fail(d,E::schedule,0x14D370);
                    Registration record;record.rank=event.row.rank();record.pass=source->pass;record.source=source;
                    record.kind=RegistrationKind::ledgerDynamic;record.ledger=&ledger;record.row=event.row;record.epoch=event.epoch;
                    record.live=live;record.state=live->states[pass];history.push_back(std::move(record));
                }
                observation.epoch=event.epoch;identityChanged=true;
            }else if(event.kind==Kind::rowRetired){
                if(observation.owners||!observation.epoch||observation.epoch!=event.epoch||event.ownerPresenceAfter||
                   !event.retirement.row().same(event.row)||event.retirement.epoch()!=event.epoch||
                   event.retirement.sequence()!=sequence||!ledger.retirementCurrent(event.retirement))return fail(d,E::schedule,0x14D440);
                const auto retired=s.backend->materialInstance(event.row,event.epoch);
                if(!retired||retired->ledger!=&ledger||!retired->row.same(event.row)||retired->epoch!=event.epoch)
                    return fail(d,E::schedule,0x14D440);
                std::size_t removed=0;
                for(auto i=history.begin();i!=history.end();){
                    if(i->kind==RegistrationKind::ledgerDynamic&&i->ledger==&ledger&&i->row.same(event.row)&&i->epoch==event.epoch){
                        i=history.erase(i);++removed;
                    }else ++i;
                }
                if(!removed||removed!=retired->sources.size())return fail(d,E::schedule,0x14D440);
                observation.epoch=0;identityChanged=true;
            }else return fail(d,E::schedule,0x14D370);
        }
        for(const auto& observation:observations){game::OriginalUiRiderMaterialRowState50KG state;
            if(!ledger.rowState(observation.row,state)||state.ownerPresence!=observation.owners||state.epoch!=observation.epoch)
                return fail(d,E::schedule,0x14D370);
        }
        for(std::size_t i=0;i<bindings.size();++i){auto& binding=bindings[i];
            const auto& old=s.bindings[i];std::uint32_t rank=unresolved;
            if(binding.kind==RegistrationKind::ledgerDynamic){
                binding.live=s.backend->liveMaterial(binding.registeredName);binding.sourceRegistered=bool(binding.live);
                rank=binding.row.rank();
                if(binding.live){const auto& live=*binding.live;
                    if(live.ledger!=&ledger||!live.row.same(binding.row)||!live.epoch||!binding.pass||!binding.pass->pass||
                       binding.pass->pass>live.sources.size()||live.sources.size()!=live.states.size()||
                       !live.sources[binding.pass->pass-1]||!compatible(*binding.pass,*live.sources[binding.pass->pass-1]))return fail(d,E::schedule,0x14D370);
                }
                if(old.live!=binding.live){
                    // An absent or changed handle requires the old exact epoch
                    // to have disappeared through an authenticated retirement.
                    if(old.live)for(const auto& record:history)if(record.kind==RegistrationKind::ledgerDynamic&&
                       record.ledger==old.live->ledger&&record.row.same(old.live->row)&&record.epoch==old.live->epoch)
                        return fail(d,E::schedule,0x14D440);
                    identityChanged=true;
                }
            }else{
                if(old.registration>=s.history.size()||s.history[old.registration].kind!=RegistrationKind::existingStatic)return fail(d,E::schedule);
                rank=s.history[old.registration].rank;
            }
            binding.registration=unresolved;
            if(!binding.sourceRegistered){binding.leaf=unresolved;continue;}
            for(std::size_t registration=0;registration<history.size();++registration){const auto& record=history[registration];
                if(record.kind!=binding.kind||record.rank!=rank||record.pass!=binding.pass->pass)continue;
                if(binding.kind==RegistrationKind::ledgerDynamic&&
                   (record.ledger!=&ledger||!record.row.same(binding.row)||record.epoch!=binding.live->epoch||record.live!=binding.live))continue;
                if(!record.source||!compatible(*record.source,*binding.pass)||!s.registrationCurrent(record))return fail(d,E::schedule);
                binding.registration=static_cast<std::uint32_t>(registration);break;
            }
            if(binding.registration==unresolved)return fail(d,E::schedule,0x14D370);
        }
        bool valid=s.groupsValid;
        if(rebuild){
            if(populationBoundary!=frontier||awaiting)return fail(d,E::schedule,0x14D440);
            std::vector<Registration> scheduled;std::vector<std::uint32_t> grouped;
            if(!s.schedule(bindings,history,scheduled,grouped,d))return false;
            history=std::move(scheduled);representatives=std::move(grouped);valid=true;awaiting=true;
        }else if(identityChanged){
            // Retirement is not14D440. Removing a redirect target cannot leave
            // old indices pointing at another compacted registration. Preserve
            // retained states but invalidate scheduling until a genuine rebuild.
            representatives.clear();for(auto& binding:bindings)binding.leaf=unresolved;valid=false;
        }
        s.history.swap(history);s.bindings.swap(bindings);s.materialObservations.swap(observations);
        s.representatives.swap(representatives);s.pendingHistory.clear();s.pendingLive.clear();
        s.lifecycleSequence=frontier;s.groupsValid=valid;s.awaitingGroupRecord=awaiting;
        if(rebuild)s.groupingFrontier=frontier;
        if(groupAccepted)s.registryGeneration=s.backend->registryGeneration();
        return true;
    }catch(...){return fail(d,E::allocation);}
}
std::uint64_t OriginalUiTournamentDrawContribution50KG::materialSequence()const noexcept {
    return storage_?storage_->lifecycleSequence:0;
}
std::uint64_t OriginalUiTournamentDrawContribution50KG::sourceSubmissionWatermark()const noexcept {
    return storage_?storage_->attemptedGeneration:0;
}
bool OriginalUiTournamentDrawContribution50KG::Storage::schedule(std::vector<Binding>& input,
    const std::vector<Registration>& previous,std::vector<Registration>& output,
    std::vector<std::uint32_t>& outputRepresentatives,D& d)const {
    // The cold resources pin the same authenticated APPSTART used by every
    // actual pass. Tokenization is source269B20: only space/CR/LF delimit.
    assets::OriginalUiMemberView50KG table{};
    const auto* inputs=resources->bankInput();
    if(!inputs||historyPoisoned||submissionPending)return fail(d,E::schedule,0x29BEA0);
    for(const auto& archive:inputs->archives)if(archive&&archive->loaded()&&
       archive->identity()==assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::appStart)) {
        for(std::size_t i=0;i<archive->memberCount();++i){const auto member=archive->member(i);
            if(member.identity&&same(std::string(member.identity->name),"AppStart/Mat_Stbl.txt"))table=member;
        }
    }
    if(!table.identity||!table.data||table.size!=70962)return fail(d,E::schedule);
    std::vector<std::uint32_t> ranks(input.size(),unresolved),sequence;sequence.reserve(input.size());
    std::size_t cursor=0;std::uint32_t ordinal=0;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    while(cursor<table.size){
        while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;
        if(cursor==table.size)break;
        const auto start=cursor;while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        for(std::size_t i=0;i<input.size();++i){const auto& name=input[i].registeredName;
            if(name.size()!=cursor-start)continue;std::size_t j=0;
            for(;j<name.size()&&fold(name[j])==fold(static_cast<char>(table.data[start+j]));++j){}
            if(j!=name.size())continue;
            if(ranks[i]!=unresolved){
                // Original298C80 returns the first matching row;299CAC
                // retains that token. The authenticated window and Mars
                // duplicates never create an owner or replace the first rank.
                const bool window=same(name,"ui/textures/pod_wndw")&&ranks[i]==2490&&
                    ordinal==3221&&start==70650;
                const bool mars=same(name,"ui/textures/08c_3")&&ranks[i]==2386&&
                    ordinal==2387&&start==55274;
                if(!window&&!mars)return fail(d,E::schedule,0x298C80);
                continue;
            }
            ranks[i]=ordinal;
        }
        ++ordinal;
    }
    if(ordinal!=3235)return fail(d,E::schedule);
    for(std::size_t i=0;i<input.size();++i){
        if(ranks[i]==unresolved||!input[i].pass)return fail(d,E::schedule);
        if(input[i].kind==RegistrationKind::ledgerDynamic&&
           (!input[i].row.prepared()||input[i].row.rank()!=ranks[i]||input[i].row.tableIdentity()!=table.identity))return fail(d,E::schedule);
        for(std::size_t j=0;j<i;++j)if(ranks[j]==ranks[i]&&input[j].kind!=input[i].kind)return fail(d,E::schedule,0x14D370);
        if(input[i].sourceRegistered)sequence.push_back(static_cast<std::uint32_t>(i));
    }
    std::sort(sequence.begin(),sequence.end(),[&](std::uint32_t a,std::uint32_t b){
        if(ranks[a]!=ranks[b])return ranks[a]<ranks[b];
        if(input[a].pass->pass!=input[b].pass->pass)return input[a].pass->pass<input[b].pass->pass;
        return a<b;
    });
    struct Group {
        std::uint32_t binding=0,redirect=unresolved,limit=unresolved,rank=0,registration=unresolved;
    };
    std::vector<Group> groups;groups.reserve(sequence.size());
    auto nextHistory=previous;
    std::uint32_t previousRank=unresolved,previousPass=0,previousGroup=unresolved;
    for(const auto index:sequence){auto& binding=input[index];const auto& pass=*binding.pass;
        // A repeated PSG slot or ATM frame is a reference to an existing
        // registry material/pass, never a second 29BED0 registration.
        if(previousRank==ranks[index]&&previousPass==pass.pass){
            const auto& original=input[groups[previousGroup].binding];
            if(binding.kind!=original.kind||!compatible(pass,*original.pass)||
               (binding.kind==RegistrationKind::ledgerDynamic&&binding.live!=original.live))return fail(d,E::schedule);
            binding.leaf=previousGroup;binding.registration=groups[previousGroup].registration;continue;
        }
        if((previousRank!=ranks[index]&&pass.pass!=1)||
           (previousRank==ranks[index]&&pass.pass!=previousPass+1))return fail(d,E::schedule,0x29BED0);
        Group next;next.binding=index;next.rank=ranks[index];
        for(std::size_t i=0;i<nextHistory.size();++i){const auto& record=nextHistory[i];
            if(record.rank!=ranks[index]||record.pass!=pass.pass)continue;
            if(record.kind!=binding.kind)return fail(d,E::schedule);
            if(binding.kind==RegistrationKind::ledgerDynamic&&
               (!binding.live||record.ledger!=binding.live->ledger||!record.row.same(binding.row)||
                record.epoch!=binding.live->epoch))continue;
            if(!record.source||!compatible(*record.source,pass)||!registrationCurrent(record))return fail(d,E::schedule);
            next.registration=static_cast<std::uint32_t>(i);break;
        }
        if(next.registration==unresolved){
            // Dynamic states were constructed by the population issuer and
            // copied when its exact rowPopulated event was accepted. Scheduling
            // is never permitted to manufacture a replacement dynamic state.
            if(binding.kind!=RegistrationKind::existingStatic)return fail(d,E::schedule,0x14D370);
            assets::OriginalUiMaterialEquivalenceKey50KG key;assets::OriginalUiMaterialEquivalenceDiagnostic50KG detail;
            if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,binding.provenance,key,detail))return fail(d,E::schedule,detail.sourcePC);
            Registration record;record.rank=ranks[index];record.pass=pass.pass;record.source=&pass;
            assets::OriginalUiMaterialBindingDiagnostic50KG bindingDetail;
            if(!record.state.initialize(key,bindingDetail))return fail(d,E::schedule,bindingDetail.sourcePC);
            next.registration=static_cast<std::uint32_t>(nextHistory.size());nextHistory.push_back(std::move(record));
        }
        const auto current=static_cast<std::uint32_t>(groups.size());
        //29C490 examines every earlier registration, including redirected ones.
        for(auto& prior:groups){
            const auto equal=nextHistory[next.registration].state.compare(nextHistory[prior.registration].state);
            if(equal==assets::OriginalUiMaterialEquivalence50KG::unsupported)return fail(d,E::schedule,0x249BA0);
            if(equal==assets::OriginalUiMaterialEquivalence50KG::equivalent&&current<prior.limit)prior.redirect=current;
        }
        groups.push_back(std::move(next));
        //29BED0 writes the previous pass's dependency barrier after adding
        // this pass. The earlier redirect check intentionally precedes it.
        if(previousRank==ranks[index])groups[previousGroup].limit=current;
        binding.leaf=current;binding.registration=groups[current].registration;previousRank=ranks[index];previousPass=pass.pass;previousGroup=current;
    }
    for(auto& binding:input){
        if(!binding.sourceRegistered){binding.leaf=unresolved;binding.registration=unresolved;continue;}
        std::size_t visits=0;
        while(binding.leaf<groups.size()&&groups[binding.leaf].redirect!=unresolved){
            const auto next=groups[binding.leaf].redirect;
            if(next<=binding.leaf||++visits>groups.size())return fail(d,E::schedule,0x29C150);
            binding.leaf=next;
        }
        if(binding.leaf>=groups.size())return fail(d,E::schedule);
    }
    // Only consumeMaterials may remove an authenticated retired epoch before
    // this call. Every remaining static or dynamic registration must survive.
    for(std::size_t i=0;i<previous.size();++i){bool present=false;
        for(const auto& group:groups)if(group.registration==i){present=true;break;}
        if(!present)return fail(d,E::schedule,0x14D440);
    }
    std::vector<std::uint32_t> nextRepresentatives;nextRepresentatives.reserve(groups.size());
    for(const auto& group:groups)nextRepresentatives.push_back(group.binding);
    output=std::move(nextHistory);outputRepresentatives=std::move(nextRepresentatives);return true;
}

bool OriginalUiTournamentDrawContribution50KG::finish(D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||!s.top||s.failed||s.finished||
       (s.pageShown&&(!s.prompts||s.apertures!=2||s.exitExpected!=s.exitProjected)))return fail(d,E::unfinished);
    if(!bank()||!s.gpu||s.gpu->publicationToken()!=s.publication||
       s.vertices.size()!=s.w.size()||s.vertices.size()!=s.fog.size()||s.vertices.size()%4||
       s.vertices.empty()||s.vertices.size()/4>originalUiMaximumSourceQuads50KM||
       s.packets.size()>originalUiMaximumSpans50KG){s.failed=true;return fail(d,E::bank);}
    try {
        s.order.clear();
        // Complete actual frame coverage, before binding-history effects or GL.
        // Actual ATM images, all model passes and binder/stat/widget packets
        // are checked individually, not only material representatives.
        for(const auto& packet:s.packets){
            if(!s.drawablePacket(packet.binding)){s.failed=true;return fail(d,E::material);}
            if(packet.binding>=s.bindings.size())return fail(d,E::publication);
            const auto index=s.bindings[packet.binding].image;
            if(index==unresolved)continue;
            const auto* cpu=s.bank->images()->textures()->texture(index);
            const auto* gpu=s.gpu->asset(index);
            if(!cpu||!gpu||!gpu->name||cpu->identity!=gpu->identity){s.failed=true;return fail(d,E::publication);}
        }
        for(std::size_t i=0;i<s.packets.size();++i){const auto& p=s.packets[i];
            if(p.binding>=s.bindings.size()||!s.bindings[p.binding].sourceRegistered||s.bindings[p.binding].leaf==unresolved||!p.count||
               p.first>=s.vertices.size()/4||p.count>s.vertices.size()/4-p.first){s.failed=true;return fail(d,E::schedule);}
            s.order.push_back(static_cast<std::uint32_t>(i));
        }
        //24B7B0 ascending groups,29C150 FIFO within the final redirect leaf.
        std::sort(s.order.begin(),s.order.end(),[&](std::uint32_t a,std::uint32_t b){
            const auto left=s.bindings[s.packets[a].binding].leaf,right=s.bindings[s.packets[b].binding].leaf;
            return left!=right?left<right:s.packets[a].ordinal<s.packets[b].ordinal;
        });
        s.orderedVertices.clear();s.orderedW.clear();s.orderedFog.clear();s.spans.clear();s.states.clear();
        for(const auto index:s.order){const auto& packet=s.packets[index];const auto& binding=s.bindings[packet.binding];
            auto span=binding.prototype->span;span.firstQuad=static_cast<std::uint32_t>(s.orderedVertices.size()/4);span.quadCount=packet.count;
            const auto start=std::size_t(packet.first)*4,count=std::size_t(packet.count)*4;
            s.orderedVertices.insert(s.orderedVertices.end(),s.vertices.begin()+start,s.vertices.begin()+start+count);
            s.orderedW.insert(s.orderedW.end(),s.w.begin()+start,s.w.begin()+start+count);
            s.orderedFog.insert(s.orderedFog.end(),s.fog.begin()+start,s.fog.begin()+start+count);
            s.spans.push_back(span);s.states.push_back(binding.prototype->native);
        }
        if(s.orderedVertices.size()!=s.vertices.size()||s.frameGeneration==std::numeric_limits<std::uint64_t>::max()){
            s.failed=true;return fail(d,E::capacity);
        }
        // Propose native binding updates before sealing. Only actual source
        // acceptance may commit them; a composed or rejected frame is no bind.
        s.pendingHistory=s.history;s.pendingLive.clear();
        OriginalUiSourceTerminal50KN terminal{};
        std::uint32_t terminalLeaf=unresolved;
        for(const auto index:s.order){const auto leaf=s.bindings[s.packets[index].binding].leaf;
            if(leaf>=s.representatives.size()||s.representatives[leaf]>=s.bindings.size()){
                s.failed=true;return fail(d,E::schedule);
            }
            const auto& representative=s.bindings[s.representatives[leaf]];
            if(representative.registration>=s.pendingHistory.size()) {s.failed=true;return fail(d,E::schedule);}
            if(representative.pass->textureCount){
                assets::OriginalUiMaterialBindingDiagnostic50KG bindingDetail;
                // Native spans retain each actual selected ATM frame. The
                // representative records the last real bind in its FIFO; the
                // source unique-animation rule excludes merging regardless.
                const auto actualImage=s.bindings[s.packets[index].binding].image;
                if(!s.pendingHistory[representative.registration].state.recordDrainedBinding(*s.bank,actualImage,bindingDetail)){
                    s.failed=true;return fail(d,E::schedule,bindingDetail.sourcePC);
                }
            }
            // initialize() admits only a prepared key; recordDrainedBinding
            // preserves it. Keep each packet's state rejection without copying
            // arrays that a later actual drain would immediately overwrite.
            if(!s.pendingHistory[representative.registration].state.initialized()){
                s.failed=true;return fail(d,E::schedule);
            }
            terminalLeaf=leaf;
        }
        if(terminalLeaf!=unresolved){
            const auto& representative=s.bindings[s.representatives[terminalLeaf]];
            // The actual nonempty drain sequence selects the representative;
            // never use the highest registered row or the packet's prototype.
            const auto& source=*representative.pass;
            const auto& registration=s.pendingHistory[representative.registration];
            terminal.canonicalToken=registration.rank;terminal.pass=registration.pass;terminal.leaf=terminalLeaf;
            terminal.registryGeneration=s.registryGeneration;terminal.bankGeneration=s.bankGeneration;
            for(std::size_t field=0;field<source.gs.fields.size();++field){
                terminal.constructionFieldSourcePC[field]=source.gs.fields[field].sourcePC;
                terminal.constructionFieldByteOffset[field]=source.gs.fields[field].byteOffset;
            }
            terminal.enabled=source.enabled;terminal.textureCount=source.textureCount;
            terminal.textureCoordinateIndex=source.textureCoordinateIndex;
            terminal.lodCalculationMethod=source.lodCalculationMethod;terminal.maximumMipLevel=source.maximumMipLevel;
            terminal.magFilter=source.magFilter;terminal.minFilter=source.minFilter;terminal.lodL=source.lodL;terminal.lodK=source.lodK;
            // All binding updates have completed, including the final actual
            // image's IPUM TEXA mutation. Copy the final effective state once.
            if(!registration.state.copyEffectiveGs(terminal.gs)){
                s.failed=true;return fail(d,E::schedule);
            }
        }
        // Prepare duplicate accepted-state vectors while allocation can still
        // reject this proposed frame. finishSourceSubmission only swaps these
        // vectors into the exact issuer handles after real source acceptance.
        for(const auto& record:s.pendingHistory){
            if(!s.registrationCurrent(record)){s.failed=true;return fail(d,E::schedule,0x14D370);}
            if(record.kind!=RegistrationKind::ledgerDynamic)continue;
            std::size_t index=0;for(;index<s.pendingLive.size();++index)if(s.pendingLive[index].live==record.live)break;
            if(index==s.pendingLive.size())s.pendingLive.push_back({record.live,record.live->states});
            if(!record.pass||record.pass>s.pendingLive[index].states.size()){
                s.failed=true;return fail(d,E::schedule,0x14D370);
            }
            s.pendingLive[index].states[record.pass-1]=record.state;
        }
        const auto* page=s.resources->page();
        if(!page){s.failed=true;return fail(d,E::resource);}
        s.draw={};s.draw.draw.textures=s.gpu.get();s.draw.draw.texturePublication=s.publication;
        s.draw.draw.vertices=s.orderedVertices.data();s.draw.draw.quadCount=static_cast<std::uint32_t>(s.orderedVertices.size()/4);
        s.draw.draw.spans=s.spans.data();s.draw.draw.spanCount=static_cast<std::uint32_t>(s.spans.size());
        s.draw.draw.canvasWidth=640;s.draw.draw.canvasHeight=448;
        for(unsigned i=0;i<3;++i){s.draw.draw.clearRgba[i]=page->backgroundColor[i]/255.f;s.draw.particleFogColor[i]=s.render.sourceFogRgb[i]/255.f;}
        // The sealed renderer performs the actual raw-alpha128 source clear;
        // clearRgba alpha remains the established native presentation contract.
        s.draw.draw.clearRgba[3]=1;s.draw.draw.retainRaceImage=false;
        s.draw.states=s.states.data();s.draw.stateCount=static_cast<std::uint32_t>(s.states.size());
        s.draw.sourceRasterW=s.orderedW.data();s.draw.sourceRasterWCount=static_cast<std::uint32_t>(s.orderedW.size());
        s.draw.particleFog=s.orderedFog.data();s.draw.particleFogCount=static_cast<std::uint32_t>(s.orderedFog.size());
        s.draw.opaquePresentationAlphaOnly=false;
        s.finished=true;
        frame_.native_=&s.draw;frame_.owner_=this;frame_.frameGeneration_=++s.frameGeneration;frame_.terminal_=terminal;

        return true;
    }catch(...){s.failed=true;return fail(d,E::allocation);}
}
const OriginalUiSourceFrame50KM* OriginalUiTournamentDrawContribution50KG::frame()const noexcept {
    return storage_&&storage_->active&&storage_->finished&&!storage_->failed&&
        !storage_->historyPoisoned&&!storage_->markerTerminal&&!storage_->submissionPending&&storage_->groupsValid&&!storage_->awaitingGroupRecord&&
        storage_->lifecycleSequence==storage_->backend->materialLifetime().sequence()&&
        storage_->registryGeneration==storage_->backend->registryGeneration()&&frame_.native_?&frame_:nullptr;
}
void OriginalUiTournamentDrawContribution50KG::drain() noexcept {
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;frame_.terminal_={};
    frame_.riderSession_=nullptr;frame_.riderTextures_=nullptr;frame_.riderResidencySerial_=0;
    if(!storage_)return;auto& s=*storage_;
    if(s.submissionPending)s.historyPoisoned=true;
    // Retain a terminal source-service failure even after CPU drain. A
    // successfully completed frame may advance normally; an abandoned or
    // failed post-service frame cannot be replayed by begin(). This latch is
    // separate from binding-history corruption, so genuine teardown still runs.
    if(s.active&&s.markerMutatedInFrame&&(!s.finished||s.failed))s.markerTerminal=true;
    if(s.active)s.backend->endFrame();
    // All camera-dependent data was flattened by value into vertices/W/fog,
    // then into ordered streams. No source frame or renderer stores a camera
    // pointer. This retires ONLY the CPU issuance after the pod borrow ended;
    // submittedRenderer and all source material/image leases remain unchanged.
    if(s.cameraFrame){if(s.camera->retireFrame(*s.cameraFrame))s.cameraFrame=nullptr;else s.historyPoisoned=true;}
    s.active=false;s.finished=false;s.draw={};s.pendingHistory.clear();s.pendingLive.clear();
}
bool OriginalUiTournamentDrawContribution50KG::beginSourceSubmission(std::uint64_t generation,const VehicleWindowGl50DS& renderer)const noexcept {
    if(!storage_||!frame()||generation!=frame_.frameGeneration_)return false;
    auto& s=*storage_;if((s.submittedRenderer&&s.submittedRenderer!=&renderer)||s.submissionPending||s.historyPoisoned||generation<=s.acknowledgedGeneration||
       generation<=s.attemptedGeneration||s.pendingHistory.size()!=s.history.size())return false;
    for(const auto& record:s.pendingHistory)if(!s.registrationCurrent(record))return false;
    if(!s.backend->sourceSubmissionStarted(this,generation))return false;
    s.submittedRenderer=&renderer;s.retiredRendererGeneration.reset();s.attemptedGeneration=generation;s.submissionPending=true;return true;
}
void OriginalUiTournamentDrawContribution50KG::finishSourceSubmission(std::uint64_t generation,bool accepted,bool started)const noexcept {
    if(!storage_)return;auto& s=*storage_;
    if(!s.submissionPending||generation!=frame_.frameGeneration_){s.historyPoisoned=true;return;}
    s.submissionPending=false;
    if(accepted){
        // Validate the whole transaction before the first nonallocating swap.
        // Synchronous backend submission ownership forbids source destruction
        // in between; an invariant failure poisons rather than partially commits.
        if(s.lifecycleSequence!=s.backend->materialLifetime().sequence()||s.pendingHistory.size()!=s.history.size()){
            s.historyPoisoned=true;return;
        }
        for(const auto& record:s.pendingHistory)if(!s.registrationCurrent(record)){s.historyPoisoned=true;return;}
        for(const auto& pending:s.pendingLive){
            if(!pending.live||pending.live->states.size()!=pending.states.size()) {s.historyPoisoned=true;return;}
            for(const auto& state:pending.states)if(!state.initialized()){s.historyPoisoned=true;return;}
        }
        for(auto& pending:s.pendingLive)pending.live->states.swap(pending.states);
        s.history.swap(s.pendingHistory);s.acknowledgedGeneration=generation;s.pendingHistory.clear();s.pendingLive.clear();
    }else{
        s.pendingLive.clear();if(started)s.historyPoisoned=true;
    }
}
} // namespace rr::platform
