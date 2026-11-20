#include "original_ui_rider_draw_contribution50kg.h"
#include "original_ui_shared_dialog_material_history50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "vehicle_window_gl50ds.h"
#include "original_ui_rider_addon_overlay50dl.h"
#include "../../../src/assets/dlc_registry50dl.h"
#include "../../../src/game/original_ui_controller_popup_geometry50kg.h"
#include "../../../src/assets/original_ui_material_equivalence50kg.h"
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <optional>
#include <utility>
namespace rr::platform {
namespace {
using D=OriginalUiRiderContributionDiagnostic50KG;using E=OriginalUiRiderContributionError50KG;
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
enum class RegistrationKind : std::uint8_t {existingStatic,ledgerDynamic,sharedCanonical};
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
    // PORT EXTENSION (DLC): an add-on model table row's own static pass, ranked
    // after Mat_Stbl (addonShared: its copy of a shared original name).
    bool addon=false,addonShared=false;
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
        return assets::prepareOriginalUiRider19ShadowMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19ShadowMaterialSpan50KM(policy,images,range,output);}
    // PORT EXTENSION, NOT ORIGINAL (DLC): an add-on pod's own "<stem>_shdw" pass
    // takes the same shadow admission (it checks the add-on owner itself).
    if(assets::originalUiAddonMemberArchive50DL(pass.identity)&&pass.textureName.size()>5&&
       pass.textureName.compare(pass.textureName.size()-5,5,"_shdw")==0){assets::OriginalUiRider19ShadowMaterial50KM policy{};
        return assets::prepareOriginalUiRider19ShadowMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19ShadowMaterialSpan50KM(policy,images,range,output);}
    if(same(pass.microprogram,"SpecularMapCompressed")){assets::OriginalUiRider19SpecularMaterial50KM policy{};
        return assets::prepareOriginalUiRider19SpecularMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19SpecularMaterialSpan50KM(policy,images,range,output);}
    if(pass.gs[assets::OriginalUiGsField50KG::onePassAntiAliasingOnOff].value==1){
        assets::OriginalUiRider19IpumMaterial50KM policy{};
        return assets::prepareOriginalUiAa1IpumMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19IpumMaterialSpan50KM(policy,images,range,output);}
    if(texture->kind==2){assets::OriginalUiRider19IpumMaterial50KM policy{};
        return assets::prepareOriginalUiRider19IpumMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19IpumMaterialSpan50KM(policy,images,range,output);}
    if(same(pass.textureName,"17_3")){assets::OriginalUiRider17CutoutMaterial50KM policy{};
        return assets::prepareOriginalUiRider17CutoutMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider17CutoutMaterialSpan50KM(policy,images,range,output);}
    if(texture->mips.size()>1){
        if(same(pass.textureName,"pod_wndw")||same(pass.textureName,"12c_4")){assets::OriginalUiRiderWindowMaterial50KM policy{};
            return assets::prepareOriginalUiRiderWindowMaterial50KM(pass,images,index,policy,d.materialDetail)&&
                convertOriginalUiRiderWindowMaterialSpan50KM(policy,images,range,output);}
        assets::OriginalUiRider19BodyMaterial50KM policy{};
        return assets::prepareOriginalUiRider19BodyMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19BodyMaterialSpan50KM(policy,images,range,output);
    }
    assets::OriginalUiLitMaterial50KM policy{};
    return assets::prepareOriginalUiLitMaterial50KM(pass,*texture,policy,d.materialDetail)&&
        convertOriginalUiLitMaterialSpan50KM(policy,range,output);
}
}
struct OriginalUiRiderDrawContribution50KG::Storage {
    std::shared_ptr<OriginalUiRiderDrawResources50KG> resources;
    std::shared_ptr<OriginalUiControllerPopupResources50KG> popup;
    std::shared_ptr<OriginalUiSharedDialogMaterialHistory50KG> common;
    std::array<std::uint32_t,6> sharedBindings{{unresolved,unresolved,unresolved,unresolved,unresolved,unresolved}};
    std::array<std::uint32_t,5> popupMaterials{{unresolved,unresolved,unresolved,unresolved,unresolved}};
    std::array<assets::OriginalUiMaterialBindingState50KG,6> commonPending;
    std::vector<OriginalUiSharedDialogMaterialHistory50KG::MenuUse> menuUses;
    std::uint64_t popupRevision=0;
    bool popupLeased=false,popupVisited=false;
    bool historyCurrent(const OriginalUiSourceOwner50KM* owner)const noexcept {
        return !popup||(popupLeased&&common&&common->current(owner,popupRevision));
    }
    void poisonHistory(const OriginalUiSourceOwner50KM* owner)noexcept {
        historyPoisoned=true;if(common&&popupLeased)common->poison(owner);
    }
    std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> statAssets;
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binderAssets;
    game::OriginalUiRiderBackend50KG* backend=nullptr;
    game::OriginalUiRiderDynamicOwner50KG* dynamic=nullptr;
    game::OriginalUiLightManagerLifetime50KG* light=nullptr;
    std::shared_ptr<const game::OriginalUiTransitionOwner50KG> transition;
    std::array<const game::OriginalUiApertureOwner50KG*,2> pageApertures{};
    game::OriginalUiApertureSnapshot50KG apertureAtBegin{};
    std::uint8_t sourcePlayer=0;
    bool exitExpected=false,exitProjected=false;
    std::uint32_t staticOwner=0,statOwner=0;
    game::OriginalUiRiderStatOwner50KG stats;
    game::OriginalUiRiderStatFrame50KG statFrame{};
    OriginalUiRiderStatDraw50KG statDraw;
    game::OriginalUiLitGeometryWorkspace50KM workspace{};
    // PORT EXTENSION (DLC): the anchor pod's selection of this frame, recorded
    // only while an add-on overlay is active (never on a stock frame).
    std::uint8_t addonFlags191=0;bool addonFlagsValid=false;
    // Step B: the anchor character's selection and Rider-light context of this frame.
    std::uint8_t addonCharacterFlags191=0;bool addonCharacterValid=false;
    game::OriginalUiLitContext50KM addonCharacterContext{};
    game::OriginalUiTextLayout50KG textScratch{};
    std::vector<game::OriginalUiTextureAnimation50KG> animations;
    std::vector<std::uint32_t> frames;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;
    std::uint64_t bankGeneration=0,publication=0,frameGeneration=0;
    std::uint64_t registryGeneration=0;
    std::uint64_t acknowledgedGeneration=0,attemptedGeneration=0;
    const VehicleWindowGl50DS* submittedRenderer=nullptr;
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
    game::OriginalUiRiderLighting50KG lighting{};
    OriginalUiRiderDrawClock50KG clock{};
    OriginalUiNativeDraw50KG draw{};
    bool active=false,ordinary=false,page=false,pageShown=false,prompts=false,top=false,finished=false,failed=false;
    bool submissionPending=false,historyPoisoned=false;
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
        if(record.kind!=RegistrationKind::ledgerDynamic)return !record.ledger&&!record.row.prepared()&&!record.epoch&&!record.live&&
            (record.kind!=RegistrationKind::sharedCanonical||(popup&&record.rank>=3228&&record.rank<=3233&&record.pass==1));
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
OriginalUiRiderDrawContribution50KG::OriginalUiRiderDrawContribution50KG() noexcept=default;
OriginalUiRiderDrawContribution50KG::~OriginalUiRiderDrawContribution50KG(){
    if(storage_&&storage_->popupLeased)storage_->poisonHistory(this);
    drain();if(storage_&&storage_->gpu)storage_->gpu->releaseRiderBorrow(this);
}
bool OriginalUiRiderDrawContribution50KG::historyRendererRetired(const VehicleWindowGl50DS& renderer)const noexcept {
    return storage_&&menuRetiredRenderer_==&renderer&&!storage_->submittedRenderer&&!storage_->submissionPending;
}
bool OriginalUiRiderDrawContribution50KG::bindSharedMenu470(OriginalUiRiderSession50KG& session,
    std::shared_ptr<OriginalUiSharedDialogs50KG> shared)noexcept {
    if(menuShared_||menuSession_||!storage_||!shared||storage_->active||storage_->popupLeased||
       !storage_->popup||storage_->popup!=shared->controller47C_.resources)return false;
    menuSession_=&session;menuShared_=std::move(shared);return true;
}
bool OriginalUiRiderDrawContribution50KG::sharedMenuBoundCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiRiderSession50KG& session)const noexcept {
    return storage_&&menuShared_.get()==&shared&&menuSession_==&session&&storage_->resources&&
        storage_->popup&&storage_->resources->controllerPopup()==storage_->popup&&
        storage_->popup==shared.controller47C_.resources&&storage_->popup->loaded()&&!storage_->popup->poisoned()&&
        bank()&&storage_->gpu&&storage_->gpu->initialized()&&storage_->publication&&
        storage_->gpu->publicationToken()==storage_->publication&&
        (!storage_->gpu->riderSparse()||(storage_->gpu->riderSession()==&session&&
          storage_->gpu->riderBank()==storage_->bank.get()&&storage_->gpu->riderDemandSerial()));
}
bool OriginalUiRiderDrawContribution50KG::sharedMenuInvocationCurrent(const OriginalUiSharedDialogs50KG& shared,
    std::uint64_t epoch)const noexcept {
    return menuSession_&&sharedMenuBoundCurrent(shared,*menuSession_)&&epoch&&epoch==menuInvocationEpoch_;
}
bool OriginalUiRiderDrawContribution50KG::sharedMenuCollectorInitialized(const OriginalUiSharedDialogs50KG& shared,
    std::uint64_t epoch)const noexcept {
    if(!sharedMenuInvocationCurrent(shared,epoch))return false;const auto& s=*storage_;
    return s.active&&!s.failed&&!s.finished&&!s.submissionPending&&!s.historyPoisoned&&s.popupLeased&&s.historyCurrent(this)&&
        !s.popupVisited&&!s.ordinary&&!s.page&&!s.pageShown&&!s.prompts&&!s.top&&!s.apertures&&!s.exitProjected&&
        !frame_.owner_&&!frame_.native_&&!frame_.frameGeneration_&&s.vertices.empty()&&s.w.empty()&&s.fog.empty()&&
        s.packets.empty()&&s.order.empty()&&s.spans.empty()&&s.states.empty()&&s.orderedVertices.empty()&&
        s.orderedW.empty()&&s.orderedFog.empty()&&s.pendingHistory.empty()&&s.pendingLive.empty()&&s.menuUses.empty();
}
bool OriginalUiRiderDrawContribution50KG::sharedMenuFrameCurrent(const OriginalUiSharedDialogs50KG& shared,
    std::uint64_t epoch,std::uint64_t generation)const noexcept {
    return sharedMenuInvocationCurrent(shared,epoch)&&storage_->active&&storage_->finished&&!storage_->failed&&
        storage_->popupVisited&&storage_->historyCurrent(this)&&frame_.owner_==this&&frame_.native_==&storage_->draw&&
        generation&&generation==frame_.frameGeneration_&&generation==storage_->frameGeneration;
}
bool OriginalUiRiderDrawContribution50KG::sharedMenuRetiredCurrent(const OriginalUiSharedDialogs50KG& shared,
    const VehicleWindowGl50DS& renderer)const noexcept {
    return menuSession_&&sharedMenuBoundCurrent(shared,*menuSession_)&&historyRendererRetired(renderer);
}
bool OriginalUiRiderDrawContribution50KG::sharedPopupInputCurrent(const OriginalUiSharedDialogs50KG& shared)const noexcept {
    if(!menuSession_||!sharedMenuBoundCurrent(shared,*menuSession_)||!menuAcceptedEpoch_||
       menuAcceptedEpoch_!=menuInvocationEpoch_||shared.menu470PendingFor(*this))return false;
    const auto& s=*storage_;
    if(s.failed||s.historyPoisoned||s.submissionPending||!s.historyCurrent(this)||!s.groupsValid||s.awaitingGroupRecord||
       s.common->menuStagePending(this)||s.acknowledgedGeneration!=s.frameGeneration||
       s.lifecycleSequence!=s.backend->materialLifetime().sequence()||s.registryGeneration!=s.backend->registryGeneration())return false;
    const auto* images=s.bank->images()?s.bank->images()->textures():nullptr;if(!images)return false;
    if(!s.gpu->riderSparse()&&!s.gpu->matchesSourceBank(*images,s.publication))return false;
    for(std::size_t role=0;role<s.sharedBindings.size();++role){const auto index=s.sharedBindings[role];
        if(index>=s.bindings.size())return false;const auto& binding=s.bindings[index];
        if(binding.kind!=RegistrationKind::sharedCanonical||!binding.sourceRegistered||!binding.prototype||
           !s.drawablePacket(index)||binding.registration>=s.history.size())return false;
        const auto& record=s.history[binding.registration];
        if(record.rank!=3228+role||record.pass!=1||!s.registrationCurrent(record))return false;
        const auto& representative=s.bindings[s.representatives[binding.leaf]];
        if(representative.registration>=s.history.size())return false;
        const auto& final=s.history[representative.registration];
        if(final.kind!=RegistrationKind::sharedCanonical||final.rank<3228||final.rank>3233||
           !s.registrationCurrent(final))return false;
        if(binding.image==unresolved){if(role!=0)return false;continue;}
        const auto* cpu=images->texture(binding.image);const auto* gpu=s.gpu->asset(binding.image);
        if(!cpu||!gpu||!gpu->name||cpu->identity!=gpu->identity)return false;
    }
    return true;
}
bool OriginalUiRiderDrawContribution50KG::acquireSharedHistory(D& d)noexcept {
    auto& s=*storage_;if(!s.popup)return true;
    if(s.popupLeased)return s.historyCurrent(this)||fail(d,E::schedule,0x249BA0);
    s.common=s.popup->sharedHistory();
    if(!menuShared_||!s.common||!s.common->acquire(this,s.popupRevision,d.controllerPopup))
        return fail(d,E::schedule,d.controllerPopup.sourcePC);
    s.popupLeased=true;
    try {
        auto bindings=s.bindings;auto history=s.history;
        std::array<std::uint32_t,6> joined{{unresolved,unresolved,unresolved,unresolved,unresolved,unresolved}};
        std::array<std::uint32_t,5> popupMaterials{{unresolved,unresolved,unresolved,unresolved,unresolved}};
        const auto& materials=*s.resources->materials();
        for(std::size_t m=0;m<materials.size();++m){const auto& material=materials[m];
            if(material.registryToken<3228||material.registryToken>3233)continue;
            const auto role=material.registryToken-3228;
            if(joined[role]!=unresolved||material.material.passes.size()!=1||material.animated||
               material.images.size()!=(role?1u:0u)){s.poisonHistory(this);return fail(d,E::material);}
            for(std::size_t i=0;i<bindings.size();++i){auto& binding=bindings[i];
                if(binding.kind!=RegistrationKind::sharedCanonical||binding.pass!=&material.material.passes[0])continue;
                if(joined[role]!=unresolved){s.poisonHistory(this);return fail(d,E::material);}
                assets::OriginalUiMaterialEquivalenceKey50KG key;assets::OriginalUiMaterialEquivalenceDiagnostic50KG detail;
                assets::OriginalUiMaterialBindingState50KG copied;
                if(!assets::prepareOriginalUiMaterialEquivalence50KG(*binding.pass,binding.provenance,key,detail)||
                   !s.common->copyHistory(this,s.popupRevision,material.registryToken,key,copied,d.controllerPopup)){
                    s.poisonHistory(this);return fail(d,E::schedule,d.controllerPopup.sourcePC?d.controllerPopup.sourcePC:detail.sourcePC);}
                std::size_t registration=0;
                for(;registration<history.size();++registration)if(history[registration].rank==material.registryToken)break;
                if(registration==history.size()){
                    Registration record;record.kind=RegistrationKind::sharedCanonical;record.rank=material.registryToken;
                    record.pass=1;record.source=binding.pass;record.state=std::move(copied);history.push_back(std::move(record));
                }else{
                    auto& record=history[registration];
                    if(record.kind!=RegistrationKind::sharedCanonical||record.pass!=1||record.source!=binding.pass){
                        s.poisonHistory(this);return fail(d,E::schedule);}
                    record.state=std::move(copied);
                }
                binding.sourceRegistered=true;joined[role]=static_cast<std::uint32_t>(i);
            }
            for(std::size_t p=0;p<popupMaterials.size();++p)
                if(material.registryToken==OriginalUiControllerPopupResources50KG::registryTokens[p])popupMaterials[p]=static_cast<std::uint32_t>(m);
        }
        for(const auto index:joined)if(index==unresolved){s.poisonHistory(this);return fail(d,E::material);}
        for(const auto index:popupMaterials)if(index==unresolved){s.poisonHistory(this);return fail(d,E::material);}
        // Rejoin the retained canonical states through Rider's actual ordered
        // registry. Dynamic row epochs and their dependency barriers survive.
        std::vector<Registration> scheduled;std::vector<std::uint32_t> representatives;
        if(!s.schedule(bindings,history,scheduled,representatives,d)){s.poisonHistory(this);return false;}
        s.bindings.swap(bindings);s.history.swap(scheduled);s.representatives.swap(representatives);
        s.sharedBindings=joined;s.popupMaterials=popupMaterials;return true;
    }catch(...){s.poisonHistory(this);return fail(d,E::allocation);}
}
bool OriginalUiRiderDrawContribution50KG::bind(std::shared_ptr<OriginalUiRiderDrawResources50KG> resources,
    std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> stat,
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binder,
    game::OriginalUiRiderBackend50KG& backend,game::OriginalUiRiderDynamicOwner50KG& dynamic,
    game::OriginalUiLightManagerLifetime50KG& light,std::uint32_t staticOwner,std::uint32_t statOwner,
    std::shared_ptr<const game::OriginalUiTransitionOwner50KG> transition,
    const std::array<const game::OriginalUiApertureOwner50KG*,2>& apertures,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound);
    if(!resources||!resources->loaded()||!stat||!stat->loaded()||!binder||!binder->loaded()||
        !backend.bank()||dynamic.poisoned()||!transition||!apertures[0]||
        !apertures[0]->snapshot().bound||(apertures[1]&&!apertures[1]->snapshot().bound))return fail(d,E::input);
    try {
        const assets::OriginalUiMemberIdentity50KG* indexIdentity=nullptr;
        const auto* archiveIdentity=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider);
        if(!archiveIdentity)return fail(d,E::resource);
        for(std::size_t i=0;i<archiveIdentity->memberCount;++i){const auto* member=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::rider,i);
            if(member&&std::strcmp(member->name,"UI\\UIRider\\ui.dat")==0)indexIdentity=member;
        }
        const auto* selection=resources->widgets()->selectionData();const auto* page=selection?selection->page():nullptr;
        for(const auto* aperture:apertures){if(!aperture)continue;const auto* config=aperture->config();
            if(!indexIdentity||!page||!config||config->indexIdentity!=indexIdentity||config->pageIdentity!=page->identity)return fail(d,E::resource);
        }
        auto s=std::make_unique<Storage>();s->resources=std::move(resources);s->statAssets=std::move(stat);s->binderAssets=std::move(binder);
        s->popup=s->resources->controllerPopup();
        s->backend=&backend;s->dynamic=&dynamic;s->light=&light;s->staticOwner=staticOwner;s->statOwner=statOwner;
        s->transition=std::move(transition);s->pageApertures=apertures;
        if(!s->stats.bind(s->statAssets,d.stat))return fail(d,E::stat);
        const auto& materials=*s->resources->materials();s->animations.resize(materials.size());s->frames.resize(materials.size());
        for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
           !game::initializeOriginalUiTextureAnimation50KG(s->animations[i],materials[i].animation,
                static_cast<std::uint32_t>(materials[i].images.size())))return fail(d,E::clock);
        for(auto* list:{&s->vertices,&s->orderedVertices,&s->scratchVertices})list->reserve(originalUiMaximumSourceQuads50KM*4u);
        for(auto* list:{&s->w,&s->fog,&s->orderedW,&s->orderedFog,&s->scratchW,&s->scratchFog})list->reserve(originalUiMaximumSourceQuads50KM*4u);
        s->packets.reserve(originalUiMaximumSpans50KG);s->order.reserve(originalUiMaximumSpans50KG);
        s->spans.reserve(originalUiMaximumSpans50KG);s->states.reserve(originalUiMaximumSpans50KG);
        storage_=std::move(s);return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderDrawContribution50KG::prepareBank(D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(s.active||frame_.native_)return fail(d,E::phase);
    if(s.historyPoisoned||s.submissionPending)return fail(d,E::schedule,0x249814);
    const auto bank=s.backend->bank();if(!bank||!bank->loaded()||!bank->images())return fail(d,E::bank);
    if(s.bank){
        // The sealed cold bank is immutable for this entire manager lifetime.
        // Source lifecycle consumption never rolls back through this method.
        return s.bankGeneration==s.backend->generation()&&s.bank==bank&&!s.bindings.empty()
            ?true:fail(d,E::bank);
    }
    if(s.backend->materialLifetime().sequence()!=0)return fail(d,E::schedule,0x14D370);
    try {
        struct Rollback {
            Storage& s;bool committed=false;
            ~Rollback(){if(!committed)s.bank.reset();}
        } rollback{s};
        s.bank=bank;
        std::vector<Binding> bindings;
        const auto& materials=*s.resources->materials();
        for(std::size_t i=0;i<materials.size();++i){d.material=static_cast<std::uint32_t>(i);const auto& m=materials[i];
            if(m.material.passes.size()!=1)return fail(d,E::material);const auto& pass=m.material.passes[0];
            // Shared canonical rows exist on the retained global owner. Until
            // its lease transfers, admit only their immutable image/policy
            // closure, never a cold replacement history or native registration.
            const auto kind=s.popup&&m.registryToken>=3228&&m.registryToken<=3233?
                RegistrationKind::sharedCanonical:RegistrationKind::existingStatic;
            if(m.images.empty()){
                if(!s.add(m.registeredName,pass,*s.resources->images(),unresolved,nullptr,bindings,d,kind))return false;
            }else for(auto image:m.images)
                if(!s.add(m.registeredName,pass,*s.resources->images(),image,s.backend->staticImage(s.staticOwner,image),bindings,d,kind))return false;
        }
        const auto statImages=s.statAssets->images();
        const auto addStat=[&](const assets::OriginalUiRiderStatResource50KG* resource){
            if(!resource||resource->material.passes.size()!=1)return false;
            for(auto image:resource->images)if(!s.add(resource->registeredName,resource->material.passes[0],*statImages,
                image,s.backend->staticImage(s.statOwner,image),bindings,d))return false;
            return true;
        };
        for(unsigned i=0;i<=static_cast<unsigned>(assets::OriginalUiRiderStatAsset50KG::labelFont);++i)
            if(!addStat(s.statAssets->resource(static_cast<assets::OriginalUiRiderStatAsset50KG>(i))))return fail(d,E::material);
        for(std::uint8_t rider=1;rider<=26;++rider)if(const auto* portrait=s.statAssets->portrait(rider))
            if(!addStat(portrait))return fail(d,E::material);
        // Only sealed material pairs participate. Cold descriptors whose exact
        // kind2 cache is absent have no backend material owner and are omitted.
        // Include each exact owner for upload policy without acquiring a row,
        // initializing a dynamic state, or granting permission to emit geometry.
        for(std::uint16_t id=1;id<120;++id){
            const auto* m=s.backend->materials(id);if(!m||!s.backend->modelImage(id,0))continue;
            const assets::Scene48D* scene=nullptr;
            if(const auto cold=s.dynamic->podAssets(id))scene=cold->scene();
            else if(const auto cold=s.dynamic->characterAssets(id))scene=cold->scene();
            if(!scene||scene->materials.size()!=m->materialCount())return fail(d,E::model);
            for(std::uint32_t slot=0;slot<m->materialCount();++slot)for(std::uint32_t p=0;p<m->passCount(slot);++p){
                d.widget=id;d.material=slot;d.pass=p+1;
                const auto* pass=m->pass(slot,p);if(!pass)return fail(d,E::material);
                const auto* map=pass->imageIndex==unresolved?nullptr:s.backend->modelImage(id,pass->imageIndex);
                if(!s.add("ui/textures/"+scene->materials[slot],pass->source,*m->images(),pass->imageIndex,map,bindings,d,RegistrationKind::ledgerDynamic))return false;
            }
        }
        // PORT EXTENSION, NOT ORIGINAL (DLC): add-on model table rows. Their own
        // materials are static rows of this page (resident with the static set,
        // uploaded by the frontend's own entry upload); shared original names use
        // the stock rows above. Nothing is added without add-on models.
        std::size_t addonBindings=0;
        if(const auto* table=s.backend->addonModels())for(std::size_t r=0;r<table->size();++r){
            const auto* row=table->row(r);if(!row||!row->ready||!row->pod||!row->materials)continue;
            const auto* scene=row->pod->scene();const auto* m=row->materials.get();
            if(!scene||!m->images()||scene->materials.size()!=m->materialCount())return fail(d,E::model);
            for(std::uint32_t slot=0;slot<m->materialCount();++slot)for(std::uint32_t p=0;p<m->passCount(slot);++p){
                d.material=slot;d.pass=p+1;
                const auto* pass=m->pass(slot,p);if(!pass)return fail(d,E::material);
                const auto* map=pass->imageIndex==unresolved?nullptr:s.backend->addonImage(r,pass->imageIndex);
                if(!s.add("ui/textures/"+scene->materials[slot],pass->source,*m->images(),pass->imageIndex,map,bindings,d))return false;
                bindings.back().addon=true;bindings.back().addonShared=m->addonShared(slot);
                ++addonBindings;
            }
            // Step C: portrait and background, static add-on rows of their own.
            if(row->pageImages){
                const auto page=[&](bool ready,const std::string& name,const assets::OriginalUiMaterial50KG& material,std::uint32_t image){
                    if(!ready)return true;
                    if(material.passes.size()!=1)return false;
                    const auto* map=s.backend->addonPageImage(r,image);
                    if(!map||!s.add(name,material.passes[0],*row->pageImages,image,map,bindings,d))return false;
                    bindings.back().addon=true;++addonBindings;return true;};
                if(!page(row->portraitReady,row->portraitName,row->portraitMaterial,row->portraitImage)||
                   !page(row->backgroundReady,row->backgroundName,row->backgroundMaterial,row->backgroundImage))return fail(d,E::material);
            }
            // Step B: the character's own materials (character pages), same static rows.
            if(!row->characterReady||!row->characterAssets||!row->characterMaterials)continue;
            const auto* cscene=row->characterAssets->scene();const auto* cm=row->characterMaterials.get();
            if(!cscene||!cm->images()||cscene->materials.size()!=cm->materialCount())return fail(d,E::model);
            for(std::uint32_t slot=0;slot<cm->materialCount();++slot)for(std::uint32_t p=0;p<cm->passCount(slot);++p){
                d.material=slot;d.pass=p+1;
                const auto* pass=cm->pass(slot,p);if(!pass)return fail(d,E::material);
                const auto* map=pass->imageIndex==unresolved?nullptr:s.backend->addonCharacterImage(r,pass->imageIndex);
                if(!s.add("ui/textures/"+cscene->materials[slot],pass->source,*cm->images(),pass->imageIndex,map,bindings,d))return false;
                bindings.back().addon=true;bindings.back().addonShared=cm->addonShared(slot);
                ++addonBindings;
            }
        }
        const auto* binderImages=s.binderAssets->images();const auto* binderPass=s.binderAssets->pass();
        if(!binderImages||!binderImages->textures()||!binderPass)return fail(d,E::resource);
        for(std::uint32_t i=0;i<binderImages->textures()->textureCount();++i)
            if(!s.add("ui/textures/binder",*binderPass,*binderImages,i,s.backend->binderImage(i),bindings,d,RegistrationKind::ledgerDynamic))return false;
        // Cold aliases carry canonical identity and policy only. A live epoch
        // may later promote every exact alias, irrespective of cold ownership.
        std::vector<Registration> history;
        std::vector<std::uint32_t> representatives;
        if(!s.schedule(bindings,s.history,history,representatives,d))return false;
        std::vector<MaterialTextureSampler50EB> samplers(bank->images()->textures()->textureCount());
        std::vector<bool> assigned(samplers.size(),false);
        for(const auto& binding:bindings)if(binding.image!=unresolved){
            if(binding.image>=samplers.size())return fail(d,E::bank);
            const auto& sampler=binding.resourceSampler;
            // Keep a stable upload default, while each authenticated prototype
            // retains its own sampler for effective per-span GL binding.
            if(!assigned[binding.image])samplers[binding.image]=sampler;
            assigned[binding.image]=true;
        }
        for(bool known:assigned)if(!known)return fail(d,E::material);
        s.bindings=std::move(bindings);s.samplers=std::move(samplers);s.bankGeneration=s.backend->generation();
        if(addonBindings)std::fprintf(stderr,"dlc_rider_materials status=registered addon_passes=%zu bindings=%zu\n",addonBindings,s.bindings.size());
        s.history=std::move(history);s.representatives=std::move(representatives);s.groupsValid=true;
        s.registryGeneration=s.backend->registryGeneration();if(s.gpu)s.gpu->releaseRiderBorrow(this);s.gpu.reset();s.publication=0;
        rollback.committed=true;return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderDrawContribution50KG::drawableModel(std::uint16_t id,D& d) const noexcept {
    d={};d.widget=id;if(!storage_||!storage_->bank||storage_->historyPoisoned||id==0||id>=120)return fail(d,E::bank);
    const auto& s=*storage_;const auto* materials=s.backend->materials(id);
    if(!materials||!materials->loaded()||bool(s.dynamic->podAssets(id))==bool(s.dynamic->characterAssets(id)))return fail(d,E::model);
    for(std::uint32_t slot=0;slot<materials->materialCount();++slot)
        for(std::uint32_t ordinal=0;ordinal<materials->passCount(slot);++ordinal){
            d.material=slot;d.pass=ordinal+1;
            const auto* pass=materials->pass(slot,ordinal);if(!pass)return fail(d,E::material);
            std::uint32_t image=unresolved;
            if(pass->source.textureCount){const auto* map=s.backend->modelImage(id,pass->imageIndex);
                if(!map)return fail(d,E::bank);image=map->combinedIndex;
            }else if(pass->imageIndex!=unresolved)return fail(d,E::material);
            bool found=false;
            for(const auto& binding:s.bindings)if(binding.kind==RegistrationKind::ledgerDynamic&&
                binding.pass==&pass->source&&binding.image==image){
                if(!binding.prototype)return fail(d,E::material);found=true;break;}
            if(!found)return fail(d,E::material);
        }
    return true;
}
bool OriginalUiRiderDrawContribution50KG::nativeDrawableModels(const std::array<bool,120>& models,D& d)const noexcept {
    d={};if(!storage_||!storage_->bank||storage_->historyPoisoned)return fail(d,E::bank);const auto& s=*storage_;
    // Static includes the actual stat/widget/ATM lifetime registrations.
    for(const auto& binding:s.bindings)if(binding.kind==RegistrationKind::existingStatic&&
        (!binding.sourceRegistered||!binding.prototype))return fail(d,E::material);
    for(const auto& binding:s.bindings)if(binding.kind==RegistrationKind::sharedCanonical&&
        (!s.popup||!s.popup->loaded()||s.popup->poisoned()||!binding.prototype))return fail(d,E::material);
    if(models[0])return fail(d,E::model);
    bool pod=false;
    for(std::uint16_t id=1;id<120;++id)if(models[id]){
        if(!drawableModel(id,d))return false;pod=pod||bool(s.dynamic->podAssets(id));
    }
    if(pod){const auto* images=s.binderAssets->images();const auto* pass=s.binderAssets->pass();
        if(!images||!images->textures()||!pass)return fail(d,E::resource);
        for(std::uint32_t i=0;i<images->textures()->textureCount();++i){
            const auto* map=s.backend->binderImage(i);if(!map)return fail(d,E::bank);bool found=false;
            for(const auto& binding:s.bindings)if(binding.kind==RegistrationKind::ledgerDynamic&&binding.pass==pass&&binding.image==map->combinedIndex){
                if(!binding.prototype)return fail(d,E::material);found=true;break;}
            if(!found)return fail(d,E::material);
        }
    }
    return true;
}
bool OriginalUiRiderDrawContribution50KG::nativeResidencyImages(
    const std::array<bool,120>& models,std::vector<std::uint32_t>& output,bool& requiresCoverage,D& d) const noexcept {
    d={};if(!storage_||!storage_->bank||storage_->historyPoisoned)return fail(d,E::bank);
    if(!nativeDrawableModels(models,d))return false;
    const auto& s=*storage_;const auto* textures=s.bank->images()?s.bank->images()->textures():nullptr;
    if(!textures||textures->textureCount()>assets::maxMaterialTextureAssets50EA)return fail(d,E::bank);
    try {
        std::vector<bool> needed(textures->textureCount(),false);
        bool nextCoverage=false;
        const auto coverage=[&](const Binding& binding){
            if(!binding.prototype||!binding.pass)return false;
            const auto sourceAa=binding.pass->gs[assets::OriginalUiGsField50KG::onePassAntiAliasingOnOff].value;
            switch(binding.prototype->native.coverage){
            case OriginalUiCoverage50KG::aa0:return sourceAa==0;
            case OriginalUiCoverage50KG::fourSampleAa1:
                if(sourceAa!=1)return false;nextCoverage=true;return true;
            default:return false;
            }
        };
        const auto add=[&](std::uint32_t index){
            if(index==unresolved)return true;
            if(index>=needed.size()||!textures->texture(index))return false;
            needed[index]=true;return true;
        };
        // Manager statics and the retained shared47C material population keep
        // their complete real image/frame closures. Dormant shared bindings
        // grant residency only until their canonical lease joins at begin().
        for(const auto& binding:s.bindings)
            if(binding.kind!=RegistrationKind::ledgerDynamic){
                if((binding.kind==RegistrationKind::existingStatic&&!binding.sourceRegistered)||
                   !coverage(binding)||!add(binding.image))return fail(d,E::material);
            }
        bool pod=false;
        for(std::uint16_t id=1;id<120;++id)if(models[id]){
            const auto* material=s.backend->materials(id);
            if(!material||!material->loaded())return fail(d,E::model);
            const auto ownedPod=s.dynamic->podAssets(id);const auto ownedCharacter=s.dynamic->characterAssets(id);
            if(bool(ownedPod)==bool(ownedCharacter))return fail(d,E::model);
            pod=pod||bool(ownedPod);
            for(std::uint32_t slot=0;slot<material->materialCount();++slot)
                for(std::uint32_t ordinal=0;ordinal<material->passCount(slot);++ordinal){
                    const auto* pass=material->pass(slot,ordinal);if(!pass)return fail(d,E::material);
                    std::uint32_t image=unresolved;
                    if(pass->imageIndex!=unresolved){
                        const auto* map=s.backend->modelImage(id,pass->imageIndex);
                        if(!map||!add(map->combinedIndex))return fail(d,E::bank);
                        image=map->combinedIndex;
                    }
                    bool policy=false;
                    for(const auto& binding:s.bindings)
                        if(binding.kind==RegistrationKind::ledgerDynamic&&binding.pass==&pass->source&&binding.image==image){policy=coverage(binding);break;}
                    if(!policy)return fail(d,E::material);
                }
        }
        // Real binder material ownership is independent of parent clipping.
        if(pod){const auto* images=s.binderAssets->images();const auto* pass=s.binderAssets->pass();
            if(!images||!images->textures()||!pass)return fail(d,E::resource);
            for(std::uint32_t i=0;i<images->textures()->textureCount();++i){
                const auto* map=s.backend->binderImage(i);if(!map||!add(map->combinedIndex))return fail(d,E::bank);
                bool policy=false;
                for(const auto& binding:s.bindings)
                    if(binding.kind==RegistrationKind::ledgerDynamic&&binding.pass==pass&&binding.image==map->combinedIndex){
                        policy=coverage(binding);break;
                    }
                if(!policy)return fail(d,E::material);
            }
        }
        std::vector<std::uint32_t> next;
        for(std::uint32_t i=0;i<needed.size();++i)if(needed[i])next.push_back(i);
        if(next.empty())return fail(d,E::bank);
        output=std::move(next);requiresCoverage=nextCoverage;return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderDrawContribution50KG::consumeMaterials(bool rebuild,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(s.active||frame_.native_||s.submissionPending||!s.bank||s.bank!=s.backend->bank()||
       s.bankGeneration!=s.backend->generation()||(rebuild&&s.historyPoisoned))return fail(d,E::phase);
    const auto& ledger=s.backend->materialLifetime();const auto frontier=ledger.sequence();
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
                if(binding.kind==RegistrationKind::sharedCanonical&&!binding.sourceRegistered){
                    binding.leaf=binding.registration=unresolved;continue;}
                if(old.registration>=s.history.size()||s.history[old.registration].kind!=binding.kind)return fail(d,E::schedule);
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
std::uint64_t OriginalUiRiderDrawContribution50KG::materialSequence()const noexcept {
    return storage_?storage_->lifecycleSequence:0;
}
std::uint64_t OriginalUiRiderDrawContribution50KG::sourceSubmissionWatermark()const noexcept {
    return storage_?storage_->attemptedGeneration:0;
}
bool OriginalUiRiderDrawContribution50KG::hasSourceSubmission()const noexcept {
    return sourceSubmissionWatermark()!=0;
}
const VehicleWindowGl50DS* OriginalUiRiderDrawContribution50KG::sourceRenderer()const noexcept {
    return storage_?storage_->submittedRenderer:nullptr;
}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiRiderDrawContribution50KG::bank()const noexcept {
    return storage_&&storage_->bankGeneration==storage_->backend->generation()?storage_->bank:nullptr;
}
const std::vector<MaterialTextureSampler50EB>* OriginalUiRiderDrawContribution50KG::samplers()const noexcept {
    return bank()?&storage_->samplers:nullptr;
}
bool OriginalUiRiderDrawContribution50KG::releasePublishedBank(D& d) noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.active||frame_.native_||s.submissionPending||s.popupLeased||
       (menuShared_&&menuShared_->menu470PendingFor(*this)))return fail(d,E::phase);
    if(s.gpu&&!s.gpu->releaseRiderBorrow(this))return fail(d,E::publication);
    s.gpu.reset();s.publication=0;return true;
}
bool OriginalUiRiderDrawContribution50KG::bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB> gpu,D& d) noexcept {
    d={};if(!storage_||storage_->active||frame_.native_||!bank()||!gpu||!gpu->initialized()||!gpu->publicationToken())return fail(d,E::publication);
    const auto* textures=storage_->bank->images()->textures();if(gpu->textureCount()!=textures->textureCount())return fail(d,E::publication);
    if(gpu->riderSparse()&&gpu->riderBank()!=storage_->bank.get())return fail(d,E::publication);
    for(std::uint32_t i=0;i<gpu->textureCount();++i){const auto* a=gpu->asset(i);const auto* b=textures->texture(i);
        if(!b||(!a&&!gpu->riderSparse())||(a&&(!a->name||a->identity!=b->identity)))return fail(d,E::publication);}
    if(storage_->gpu==gpu)return true;
    if(storage_->popupLeased||(menuShared_&&menuShared_->menu470PendingFor(*this)))return fail(d,E::phase);
    if(!gpu->acquireRiderBorrow(this))return fail(d,E::publication);
    if(storage_->gpu&&!storage_->gpu->releaseRiderBorrow(this)){
        gpu->releaseRiderBorrow(this);return fail(d,E::publication);}
    storage_->gpu=std::move(gpu);storage_->publication=storage_->gpu->publicationToken();return true;
}
bool OriginalUiRiderDrawContribution50KG::begin(const OriginalUiRiderDrawClock50KG& clock,
    const game::OriginalUiRiderRenderContext50KG& render,const game::OriginalUiRiderLighting50KG& lighting,D& d) noexcept {
    d={};if(!storage_||!clock.known)return fail(d,E::input);auto& s=*storage_;
    if(s.active||frame_.native_||s.historyPoisoned||s.submissionPending||!s.groupsValid||s.awaitingGroupRecord||
       s.lifecycleSequence!=s.backend->materialLifetime().sequence()||s.registryGeneration!=s.backend->registryGeneration()||!bank()||!s.gpu||!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,E::publication);
    const auto lifecycle=s.transition->snapshot();
    if(!lifecycle.bound||lifecycle.current.screen!=5||lifecycle.current.page<1||lifecycle.current.page>2)return fail(d,E::phase);
    const auto player=static_cast<std::uint8_t>(lifecycle.current.page-1);
    // Unsupported page2 has no fabricated hook owner. Page1 is mandatory at
    // bind; every optional supplied owner was provenance-checked there.
    if(!s.pageApertures[player])return fail(d,E::phase);
    const auto aperture=s.pageApertures[player]->snapshot();
    if(!aperture.bound)return fail(d,E::phase);
    if((menuShared_&&menuShared_->menu470PendingFor(*this))||(s.common&&s.common->menuStagePending(this)))
        return fail(d,E::phase,0x204A00);
    // Rider has no Controls2 warning authority. It joins only the genuine
    // invisible470 inherited callbacks followed by the retained47C sibling.
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic shared;
        if(!menuShared_->invisible470204AC0(shared)||menuInvocationEpoch_==std::numeric_limits<std::uint64_t>::max())
            return fail(d,E::phase,shared.sourcePC);}
    if(!acquireSharedHistory(d))return false;
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic shared;++menuInvocationEpoch_;menuRetiredRenderer_=nullptr;
        if(!menuSession_||!menuShared_->beginMenu470(*menuSession_,*this,menuInvocationEpoch_,shared)){
            s.failed=true;return fail(d,E::phase,shared.sourcePC);}}
    game::OriginalUiRiderVisibilityDiagnostic50KG visibility;
    game::OriginalUiLightManagerLifetimeDiagnostic50KG lights;
    if(!s.backend->beginFrame())return fail(d,E::phase);
    struct Lease {game::OriginalUiRiderBackend50KG* backend;bool retained=false;~Lease(){if(!retained)backend->endFrame();}} lease{s.backend};
    if(!s.light->publishRiderRegistry(lighting,lights)||!s.backend->visibility()||
       !s.backend->visibility()->prepareFrame(render.cameraAfterFog,{},visibility))return fail(d,E::model);
    lease.retained=true;
    s.clock=clock;s.render=render;s.lighting=lighting;s.active=true;
    s.sourcePlayer=player;s.apertureAtBegin=aperture;s.exitProjected=false;
    s.exitExpected=lifecycle.lifecycle==game::OriginalUiTransitionLifecycle50KG::waitScreenExit&&
        aperture.state==game::OriginalUiApertureState50KG::closing&&aperture.top284>=224&&aperture.bottom28C<=224;
    s.popupVisited=false;s.ordinary=s.page=s.pageShown=s.prompts=s.top=s.finished=s.failed=false;s.apertures=0;
    s.vertices.clear();s.w.clear();s.fog.clear();s.packets.clear();s.order.clear();s.spans.clear();s.states.clear();
    s.orderedVertices.clear();s.orderedW.clear();s.orderedFog.clear();s.pendingHistory.clear();s.pendingLive.clear();s.menuUses.clear();
    for(std::size_t i=0;i<s.animations.size();++i)if((*s.resources->materials())[i].animated&&
       !game::selectOriginalUiTextureAnimationFrame50KG(s.animations[i],s.frames[i])){s.failed=true;return fail(d,E::clock);}
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic shared;
        if(!menuShared_->continueMenu470AfterCollectorBegin(*this,shared)){
            s.failed=true;return fail(d,E::phase,shared.sourcePC);}}
    return true;
}
bool OriginalUiRiderDrawContribution50KG::append(const game::OriginalUiOverlayQuad50KG& quad,
    std::uint32_t materialIndex,float z,D& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(materialIndex>=materials.size()||!std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z||
       quad.sourcePacketColor[3]>128)return fail(d,E::geometry);
    const auto& material=materials[materialIndex];
    if(material.animated&&s.clock.advanceAnimations&&
       !game::advanceOriginalUiTextureAnimation50KG(s.animations[materialIndex],s.clock.milliseconds,s.frames[materialIndex]))return fail(d,E::clock);
    const auto frame=s.frames[materialIndex];std::uint32_t image=unresolved;
    if(!material.images.empty()){
        if(frame>=material.images.size())return fail(d,E::clock);
        const auto* map=s.backend->staticImage(s.staticOwner,material.images[frame]);if(!map)return fail(d,E::bank);image=map->combinedIndex;
    }
    const auto binding=s.find(&material.material.passes[0],image);if(binding==unresolved)return fail(d,E::material);
    if(!s.capacity(1))return fail(d,E::capacity);
    std::array<assets::Vertex48B,4> vertices{};constexpr unsigned order[]={0,1,3,2};
    for(unsigned i=0;i<4;++i){const auto& v=quad.vertices[order[i]];
        if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v))return fail(d,E::geometry);
        auto& out=vertices[i];out.position[0]=v.x;out.position[1]=v.y;out.position[2]=z;out.uv[0]=v.u;out.uv[1]=v.v;
        for(unsigned c=0;c<4;++c)out.color[c]=quad.sourcePacketColor[c];
    }
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
    s.vertices.insert(s.vertices.end(),vertices.begin(),vertices.end());s.w.insert(s.w.end(),4,1.f);s.fog.insert(s.fog.end(),4,0.f);
    s.packets.push_back({first,1,binding,static_cast<std::uint32_t>(s.packets.size())});return true;
}
bool OriginalUiRiderDrawContribution50KG::model(std::uint16_t id,D& d) noexcept {
    auto& s=*storage_;if(!s.backend->attached(id)||!s.backend->shown(id))return true;
    game::OriginalUiRiderSelectedPacket50KG selected{};
    if(!game::prepareOriginalUiRiderSelectedPacket50KG(id,*s.dynamic,*s.backend,s.render,s.lighting,*s.light,selected,d.selected))return fail(d,E::model);
    if(const auto* addon=riderAddon50DL::overlayFor(this);addon&&id==addon->suppressed[0]){
        s.addonFlags191=selected.selection.sourceModelFlags191;s.addonFlagsValid=selected.emitsGeometry;}
    else if(addon&&id==addon->suppressed[3]){
        s.addonCharacterFlags191=selected.selection.sourceModelFlags191;s.addonCharacterValid=selected.emitsGeometry;
        if(selected.emitsGeometry)s.addonCharacterContext=selected.packet.context;}
    if(selected.emitsGeometry){
        const auto& packet=selected.packet;const auto* materials=s.backend->materials(id);
        if(!packet.scene||!materials||packet.scene->lods.size()!=1)return fail(d,E::geometry);
        const auto& surfaces=packet.scene->lods[0].surfaces;
        for(std::uint32_t surface=0;surface<surfaces.size();++surface){
            const auto materialIndex=surfaces[surface].material_index;
            d.geometrySurface=surface;d.geometryComponent=surfaces[surface].component_word;
            d.geometryStoredColors=surfaces[surface].has_original_colors;d.geometryStatus=255;d.material=materialIndex;
            if(materialIndex>=materials->materialCount()||surfaces[surface].indices.size()%3)return fail(d,E::material);
            const auto count=surfaces[surface].indices.size()/3;
            if(!count)continue;
            if(count>originalUiMaximumSourceQuads50KM)return fail(d,E::capacity);
            s.scratchVertices.resize(count*4);s.scratchW.resize(count*4);s.scratchFog.resize(count*4);
            for(std::uint32_t passIndex=0;passIndex<materials->passCount(materialIndex);++passIndex){
                d.pass=passIndex+1;d.geometryStatus=255;
                const auto* material=materials->pass(materialIndex,passIndex);if(!material)return fail(d,E::material);
                const auto* map=material->imageIndex==unresolved?nullptr:s.backend->modelImage(id,material->imageIndex);
                if(material->imageIndex!=unresolved&&!map)return fail(d,E::bank);
                const auto binding=s.find(&material->source,map?map->combinedIndex:unresolved);
                if(binding==unresolved)return fail(d,E::material);
                game::OriginalUiLitSurfaceOutput50KM projected{s.scratchVertices.data(),s.scratchW.data(),s.scratchFog.data(),static_cast<std::uint32_t>(count),0,0};
                const auto coloredOwner=s.dynamic->characterAssets(id);
                const bool ownColored=surfaces[surface].has_original_colors&&coloredOwner&&
                    coloredOwner->scene()==packet.scene;
                const auto status=ownColored&&id==57?
                    game::projectOriginalUiKraidColoredSurface50KM(*coloredOwner,0,surface,passIndex,
                        material->source.microprogram,packet.palette,packet.context,
                        packet.fog.computeRadialFog1B2,projected,s.workspace):ownColored?
                    game::projectOriginalUiKnireColoredSurface50KM(*coloredOwner,0,surface,passIndex,
                        material->source.microprogram,packet.palette,packet.context,
                        packet.fog.computeRadialFog1B2,projected,s.workspace):
                    same(material->source.microprogram,"SpecularMapCompressed")?
                    game::projectOriginalUiSpecularMapCompressedSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,projected,s.workspace):
                    game::projectOriginalUiLitSceneSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,packet.fog.computeRadialFog1B2,projected,s.workspace);
                d.geometryStatus=static_cast<std::uint8_t>(status);
                if(status!=game::OriginalUiLitGeometryStatus50KM::complete||projected.materialIndex!=materialIndex)return fail(d,E::geometry);
                if(!s.capacity(projected.slotCount))return fail(d,E::capacity);
                if(!projected.slotCount)continue;const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
                const auto n=std::size_t(projected.slotCount)*4;
                s.vertices.insert(s.vertices.end(),s.scratchVertices.begin(),s.scratchVertices.begin()+n);
                s.w.insert(s.w.end(),s.scratchW.begin(),s.scratchW.begin()+n);s.fog.insert(s.fog.end(),s.scratchFog.begin(),s.scratchFog.begin()+n);
                s.packets.push_back({first,projected.slotCount,binding,static_cast<std::uint32_t>(s.packets.size())});
            }
        }
    }
    // Entity-child traversal is independent of the graphics model's191 clip
    // bit. Real enabled binder helpers still run after a culled model parent.
    auto* pod=s.dynamic->podScene(id);if(!pod)return true;
    for(std::uint8_t pair=0;pair<2;++pair){auto* animation=s.backend->binder(id,pair);if(!animation)continue;
        game::OriginalUiRiderBinderQuad50KG source{};
        if(!pod->projectBinder(pair,s.render.cameraAfterFog.binderCameraColumn,source,d.model))return fail(d,E::model);
        game::OriginalUiRiderBinderFrame50KG image;
        if(!(s.clock.advanceAnimations?animation->advanceAfterGeometry(s.clock.milliseconds,image):animation->current(image)))return fail(d,E::clock);
        game::original_racer_lighting50KM::LightMatrices global{};
        if(game::projectOriginalUiGlobalLightRows50KM(true,s.lighting.directionalLook78,s.lighting.directional3C,
            s.lighting.ambient50,global)!=game::original_racer_lighting50KM::Status::complete)return fail(d,E::model);
        game::OriginalUiLitContext50KM context{};
        if(!game::prepareOriginalUiRiderLitContext50KG(*s.backend->riderArchive(),s.render,global,context))return fail(d,E::model);
        game::OriginalUiLitBinderPacket50KM projected{};
        if(game::projectOriginalUiRiderBinderPacket50KM(source,context,s.lighting.ambient50,projected)!=game::OriginalUiLitGeometryStatus50KM::complete)return fail(d,E::geometry);
        const auto* map=s.backend->binderImage(image.imageIndex);if(!map)return fail(d,E::bank);
        const auto binding=s.find(image.pass,map->combinedIndex);if(binding==unresolved)return fail(d,E::material);
        if(!s.capacity(2))return fail(d,E::capacity);const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
        s.vertices.insert(s.vertices.end(),projected.vertices.begin(),projected.vertices.end());
        s.w.insert(s.w.end(),projected.rasterW.begin(),projected.rasterW.end());s.fog.insert(s.fog.end(),projected.fogFactors.begin(),projected.fogFactors.end());
        s.packets.push_back({first,2,binding,static_cast<std::uint32_t>(s.packets.size())});
    }
    return true;
}
bool OriginalUiRiderDrawContribution50KG::addonModel(std::size_t index,std::uint8_t flags,D& d) noexcept {
    // PORT EXTENSION, NOT ORIGINAL (DLC menu-art step A): the add-on pod model,
    // projected exactly as a stock pod (same light rows, Rider camera context,
    // palette, fog and lit/specular surface projection) from its own table row.
    // No binder helpers yet. A row that cannot draw is reported, never a frame failure.
    auto& s=*storage_;const auto* addon=riderAddon50DL::overlayFor(this);
    // A refused row withdraws everything it emitted this frame (never half a model).
    const auto vertices0=s.vertices.size(),w0=s.w.size(),fog0=s.fog.size(),packets0=s.packets.size();
    const auto refuse=[&](const char* why){
        s.vertices.resize(vertices0);s.w.resize(w0);s.fog.resize(fog0);s.packets.resize(packets0);
        if(addon)addon->modelRefused=why;return true;};
    const auto* table=s.backend->addonModels();
    auto* row=table?const_cast<game::OriginalUiRiderAddonModels50DL*>(table)->row(index):nullptr;
    if(!row||!row->ready||!row->pod||!row->materials||!row->scene||!row->scene->loaded())return refuse("add-on model row not ready");
    if((flags&1u)==0)return refuse("anchor pod culled");
    const auto* scene=row->pod->scene();const auto* materials=row->materials.get();const auto archive=s.backend->riderArchive();
    if(!scene||!archive||!row->pod->hierarchy()||scene->lods.size()!=1||scene->materials.size()!=materials->materialCount())
        return refuse("add-on model shape");
    game::original_racer_lighting50KM::LightMatrices lights{};
    if(game::projectOriginalUiGlobalLightRows50KM(true,s.lighting.directionalLook78,s.lighting.directional3C,
        s.lighting.ambient50,lights)!=game::original_racer_lighting50KM::Status::complete)return refuse("light rows");
    game::OriginalUiRiderModelPacket50KG packet{};
    if(!game::prepareOriginalUiRiderLitContext50KG(*archive,s.render,lights,packet.context))return refuse("camera context");
    if(!row->scene->projectPalette(flags,packet.palette,d.model))return refuse("palette");
    if(!game::prepareOriginalUiRiderModelFog50KG(*row->pod->hierarchy(),row->scene->graph(),s.render.cameraAfterFog.position,
        s.render.fogStart,packet.fog))return refuse("fog bounds");
    packet.scene=scene;packet.sourceModelFlags191=flags;
    const auto& surfaces=scene->lods[0].surfaces;std::uint32_t emitted=0;
    for(std::uint32_t surface=0;surface<surfaces.size();++surface){
        const auto materialIndex=surfaces[surface].material_index;
        if(materialIndex>=materials->materialCount()||surfaces[surface].indices.size()%3)return refuse("surface material");
        const auto count=surfaces[surface].indices.size()/3;if(!count)continue;
        if(count>originalUiMaximumSourceQuads50KM)return refuse("surface size");
        s.scratchVertices.resize(count*4);s.scratchW.resize(count*4);s.scratchFog.resize(count*4);
        // Own material passes, or the stock rows of a shared original name.
        std::vector<std::pair<const assets::OriginalUiMaterialPass50KG*,std::uint32_t>> passes;
        // Own passes, including this row's own copies of shared original names.
        for(std::uint32_t p=0;p<materials->passCount(materialIndex);++p){
            const auto* material=materials->pass(materialIndex,p);if(!material)return refuse("material pass");
            const auto* map=material->imageIndex==unresolved?nullptr:s.backend->addonImage(index,material->imageIndex);
            if(material->imageIndex!=unresolved&&!map)return refuse("add-on image not in the bank");
            const auto binding=s.find(&material->source,map?map->combinedIndex:unresolved);
            if(binding==unresolved)return refuse("add-on material not registered");
            passes.emplace_back(&material->source,binding);
        }
        for(const auto& [source,binding]:passes){
            game::OriginalUiLitSurfaceOutput50KM projected{s.scratchVertices.data(),s.scratchW.data(),s.scratchFog.data(),static_cast<std::uint32_t>(count),0,0};
            const auto status=same(source->microprogram,"SpecularMapCompressed")?
                game::projectOriginalUiSpecularMapCompressedSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,projected,s.workspace):
                game::projectOriginalUiLitSceneSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,packet.fog.computeRadialFog1B2,projected,s.workspace);
            if(status!=game::OriginalUiLitGeometryStatus50KM::complete||projected.materialIndex!=materialIndex)return refuse("surface projection");
            if(!s.capacity(projected.slotCount))return fail(d,E::capacity);
            if(!projected.slotCount)continue;const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
            const auto n=std::size_t(projected.slotCount)*4;
            s.vertices.insert(s.vertices.end(),s.scratchVertices.begin(),s.scratchVertices.begin()+n);
            s.w.insert(s.w.end(),s.scratchW.begin(),s.scratchW.begin()+n);s.fog.insert(s.fog.end(),s.scratchFog.begin(),s.scratchFog.begin()+n);
            s.packets.push_back({first,projected.slotCount,binding,static_cast<std::uint32_t>(s.packets.size())});
            ++emitted;
        }
    }
    if(addon){addon->modelPackets+=emitted;if(!emitted&&!addon->modelRefused)addon->modelRefused="no surface emitted";}
    return true;
}
bool OriginalUiRiderDrawContribution50KG::addonCharacter(std::size_t index,D& d) noexcept {
    // PORT EXTENSION, NOT ORIGINAL (DLC menu-art step B): the add-on character,
    // projected like a stock character (its own palette from its own playback
    // graph, its own fog bounds, the Rider page's camera and two-light context
    // that the anchor character's packet used this frame). A row that cannot
    // draw withdraws its packets for this frame and reports why.
    auto& s=*storage_;const auto* addon=riderAddon50DL::overlayFor(this);
    const auto vertices0=s.vertices.size(),w0=s.w.size(),fog0=s.fog.size(),packets0=s.packets.size();
    const auto refuse=[&](const char* why){
        s.vertices.resize(vertices0);s.w.resize(w0);s.fog.resize(fog0);s.packets.resize(packets0);
        if(addon)addon->characterRefused=why;return true;};
    auto* table=const_cast<game::OriginalUiRiderAddonModels50DL*>(s.backend->addonModels());
    auto* row=table?table->row(index):nullptr;auto* scene=table?table->liveCharacter(index):nullptr;
    if(!row||!row->characterReady||!row->characterAssets||!row->characterMaterials)return refuse("add-on character row not ready");
    if(!scene)return refuse("add-on character graph not attached");
    // The Rider page two-light registry (the same projection the stock character
    // receives) and the Rider camera context; selected, unclipped (flags 1).
    constexpr std::uint8_t flags=1;
    game::original_racer_lighting50KM::LightMatrices lights{};game::OriginalUiLightManagerLifetimeDiagnostic50KG ld;
    const auto archive=s.backend->riderArchive();
    if(!s.light||!s.light->addonRiderCharacterLights50DL(lights,ld))return refuse("Rider character lights");
    const auto& assets=*row->characterAssets;const auto* materials=row->characterMaterials.get();
    const auto* model=assets.scene();
    if(!model||!assets.hierarchy()||scene->scene()!=model||model->lods.size()!=1||model->materials.size()!=materials->materialCount())
        return refuse("add-on character shape");
    game::OriginalUiRiderModelPacket50KG packet{};
    if(!archive||!game::prepareOriginalUiRiderLitContext50KG(*archive,s.render,lights,packet.context))return refuse("camera context");
    if(!scene->projectPalette(flags,packet.palette,d.model))return refuse("palette");
    if(!game::prepareOriginalUiRiderModelFog50KG(*assets.hierarchy(),scene->graph(),s.render.cameraAfterFog.position,
        s.render.fogStart,packet.fog))return refuse("fog bounds");
    packet.scene=model;packet.sourceModelFlags191=flags;
    const auto& surfaces=model->lods[0].surfaces;std::uint32_t emitted=0;
    for(std::uint32_t surface=0;surface<surfaces.size();++surface){
        const auto materialIndex=surfaces[surface].material_index;
        if(materialIndex>=materials->materialCount()||surfaces[surface].indices.size()%3)return refuse("surface material");
        const auto count=surfaces[surface].indices.size()/3;if(!count)continue;
        if(count>originalUiMaximumSourceQuads50KM)return refuse("surface size");
        s.scratchVertices.resize(count*4);s.scratchW.resize(count*4);s.scratchFog.resize(count*4);
        std::vector<std::pair<const assets::OriginalUiMaterialPass50KG*,std::uint32_t>> passes;
        // Own passes, including this row's own copies of shared original names.
        for(std::uint32_t p=0;p<materials->passCount(materialIndex);++p){
            const auto* material=materials->pass(materialIndex,p);if(!material)return refuse("material pass");
            const auto* map=material->imageIndex==unresolved?nullptr:s.backend->addonCharacterImage(index,material->imageIndex);
            if(material->imageIndex!=unresolved&&!map)return refuse("add-on character image not in the bank");
            const auto binding=s.find(&material->source,map?map->combinedIndex:unresolved);
            if(binding==unresolved)return refuse("add-on character material not registered");
            passes.emplace_back(&material->source,binding);
        }
        const bool colored=surfaces[surface].has_original_colors;
        for(std::uint32_t passIndex=0;passIndex<passes.size();++passIndex){const auto& [source,binding]=passes[passIndex];
            game::OriginalUiLitSurfaceOutput50KM projected{s.scratchVertices.data(),s.scratchW.data(),s.scratchFog.data(),static_cast<std::uint32_t>(count),0,0};
            // Stored-colour surfaces take the original no-fog colour kernel when it
            // admits this owner; otherwise (PORT STAND-IN) the ordinary lit kernel.
            auto status=colored?
                game::projectOriginalUiKnireColoredSurface50KM(assets,0,surface,passIndex,source->microprogram,packet.palette,packet.context,
                    packet.fog.computeRadialFog1B2,projected,s.workspace):game::OriginalUiLitGeometryStatus50KM::input;
            if(status!=game::OriginalUiLitGeometryStatus50KM::complete)status=
                same(source->microprogram,"SpecularMapCompressed")?
                game::projectOriginalUiSpecularMapCompressedSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,projected,s.workspace):
                game::projectOriginalUiLitSceneSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,packet.fog.computeRadialFog1B2,projected,s.workspace);
            if(status!=game::OriginalUiLitGeometryStatus50KM::complete||projected.materialIndex!=materialIndex)return refuse("surface projection");
            if(!s.capacity(projected.slotCount))return fail(d,E::capacity);
            if(!projected.slotCount)continue;const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
            const auto n=std::size_t(projected.slotCount)*4;
            s.vertices.insert(s.vertices.end(),s.scratchVertices.begin(),s.scratchVertices.begin()+n);
            s.w.insert(s.w.end(),s.scratchW.begin(),s.scratchW.begin()+n);s.fog.insert(s.fog.end(),s.scratchFog.begin(),s.scratchFog.begin()+n);
            s.packets.push_back({first,projected.slotCount,binding,static_cast<std::uint32_t>(s.packets.size())});
            ++emitted;
        }
    }
    if(addon){addon->characterPackets+=emitted;if(!emitted&&!addon->characterRefused)addon->characterRefused="no surface emitted";}
    return true;
}
bool OriginalUiRiderDrawContribution50KG::widgets(bool top,bool shown,D& d) noexcept {
    auto& s=*storage_;auto& widgets=*s.resources->widgets();const auto* order=widgets.order(top);
    if(!order)return fail(d,E::widget);const auto& materials=*s.resources->materials();
    const auto* addon=riderAddon50DL::overlayFor(this); // PORT EXTENSION: add-on rider frame only
    for(const auto id:*order){d.widget=id;
        d.geometrySurface=0xffffffffu;d.geometryComponent=0;d.geometryStoredColors=false;d.geometryStatus=255;
        const auto state=widgets.state(id);if(!shown||!state.shown)continue;
        if(addon&&addon->suppresses(id)){
            // PORT EXTENSION, NOT ORIGINAL: the anchor control under an add-on rider.
            if(state.kind==game::OriginalUiRiderWidgetKind50KG::pod||state.kind==game::OriginalUiRiderWidgetKind50KG::character){
                // Run the unchanged model projection (same per-frame owner effects),
                // then drop its output: no stock model under an add-on name.
                const auto vertices=s.vertices.size(),w=s.w.size(),fog=s.fog.size(),packets=s.packets.size();
                if(!model(id,d))return false;
                if(s.vertices.size()<vertices||s.w.size()<w||s.fog.size()<fog||s.packets.size()<packets)return fail(d,E::geometry);
                s.vertices.erase(s.vertices.begin()+std::ptrdiff_t(vertices),s.vertices.end());
                s.w.erase(s.w.begin()+std::ptrdiff_t(w),s.w.end());s.fog.erase(s.fog.begin()+std::ptrdiff_t(fog),s.fog.end());
                s.packets.erase(s.packets.begin()+std::ptrdiff_t(packets),s.packets.end());
                if(state.kind==game::OriginalUiRiderWidgetKind50KG::pod&&id==addon->suppressed[0]&&
                   addon->modelRow!=riderAddon50DL::Overlay::noRow){
                    if(!s.addonFlagsValid)addon->modelRefused="anchor pod not selected this frame";
                    else if(!addonModel(addon->modelRow,s.addonFlags191,d))return false;
                }
                s.addonFlagsValid=false;
                if(state.kind==game::OriginalUiRiderWidgetKind50KG::character&&id==addon->suppressed[3]&&
                   addon->modelRow!=riderAddon50DL::Overlay::noRow){
                    // Drawn because the add-on row is highlighted, independent of the
                    // hidden anchor character's own selection this frame.
                    if(!addonCharacter(addon->modelRow,d))return false;
                }
                s.addonCharacterValid=false;
                continue;
            }
            const auto* descriptor=id==addon->nameWidget?widgets.textDescriptor(id):nullptr;
            if(!descriptor)continue; // the anchor's large name: nothing drawn for it
            const auto materialIndex=s.resources->widgetMaterial(id);
            const auto* font=s.resources->textFont(id);const auto* atlas=s.resources->textAtlas(id);
            if(!font||!atlas||materialIndex>=materials.size()||materials[materialIndex].images.size()!=1)return fail(d,E::resource);
            const auto& material=materials[materialIndex];const auto* map=s.backend->staticImage(s.staticOwner,material.images[0]);
            if(!map)return fail(d,E::bank);const auto binding=s.find(&material.material.passes[0],map->combinedIndex);
            if(binding==unresolved)return fail(d,E::material);float z=0;if(!s.resources->textPacketDepth(id,z))return fail(d,E::geometry);
            float below=0; // the package name goes one font line under the pilot name
            for(const auto& line:addon->lines){
                // A line whose glyphs the authored font lacks is skipped (logged by the
                // session), never drawn with invented glyphs and never a frame failure.
                const std::string* text=nullptr;game::OriginalUiTextDiagnostic50KG probe;
                for(const auto* candidate:{line.text,line.fallback}){
                    if(!candidate||candidate->empty())continue;
                    const game::OriginalUiTextInput50KG trial{font,atlas,reinterpret_cast<const std::uint8_t*>(candidate->data()),candidate->size(),0,0};
                    if(game::layoutOriginalUiText50KG(trial,s.textScratch,probe)){text=candidate;break;}
                }
                if(!text){if(line.text&&!line.text->empty())++addon->skippedLines;continue;}
                game::OriginalUiTextInput50KG input{font,atlas,reinterpret_cast<const std::uint8_t*>(text->data()),text->size(),0,0};
                std::int32_t width=0,height=0;
                if(!game::roundOriginalUiTextMetric50KG(s.textScratch.widthBeforeScaleAndRounding,1,width)||
                    !game::roundOriginalUiTextMetric50KG(s.textScratch.firstGlyphHeightBeforeScaleAndRounding,1,height)){++addon->skippedLines;continue;}
                const float lineHeight=s.textScratch.fontLineHeight;
                game::OriginalUiRiderTextProjection50KG live;
                if(!widgets.projectText(id,0,lineHeight,width,height,shown,live,d.widgetDetail)){
                    d.widgetDetail={};break; // hidden (or not projectable): no add-on name either
                }
                if(live.descriptor!=descriptor||live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128)
                    return fail(d,E::geometry);
                input.localX=live.position.positionedX;input.localY=live.position.positionedY+below;
                if(!std::isfinite(input.localY)||!game::layoutOriginalUiText50KG(input,s.textScratch,d.text)){d.text={};++addon->skippedLines;continue;}
                if(std::isfinite(lineHeight)&&lineHeight>0)below+=lineHeight;
                const auto count=s.textScratch.count;if(!count)continue;if(!s.capacity(count))return fail(d,E::capacity);
                const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
                for(unsigned q=0;q<count;++q)for(const auto& glyph:s.textScratch.quads[q].vertices){
                    assets::Vertex48B vertex{};vertex.position[0]=glyph.x;vertex.position[1]=glyph.y;vertex.position[2]=z;
                    vertex.uv[0]=glyph.u;vertex.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)vertex.color[c]=live.line.packetColor[c];s.vertices.push_back(vertex);
                }
                s.w.insert(s.w.end(),std::size_t(count)*4,1.f);s.fog.insert(s.fog.end(),std::size_t(count)*4,0.f);
                s.packets.push_back({first,count,binding,static_cast<std::uint32_t>(s.packets.size())});
                ++addon->drawnLines;
            }
            continue;
        }
        if(state.kind==game::OriginalUiRiderWidgetKind50KG::pod||state.kind==game::OriginalUiRiderWidgetKind50KG::character){
            if(!model(id,d))return false;continue;
        }
        const auto materialIndex=s.resources->widgetMaterial(id);
        if(const auto* descriptor=widgets.textDescriptor(id)){
            const auto* font=s.resources->textFont(id);const auto* atlas=s.resources->textAtlas(id);const auto* lines=widgets.textLines(id);
            if(!font||!atlas||!lines||materialIndex>=materials.size()||materials[materialIndex].images.size()!=1)return fail(d,E::resource);
            const auto& material=materials[materialIndex];const auto* map=s.backend->staticImage(s.staticOwner,material.images[0]);
            if(!map)return fail(d,E::bank);const auto binding=s.find(&material.material.passes[0],map->combinedIndex);
            if(binding==unresolved)return fail(d,E::material);float z=0;if(!s.resources->textPacketDepth(id,z))return fail(d,E::geometry);
            for(std::size_t line=0;line<lines->size();++line){const auto& text=(*lines)[line];
                game::OriginalUiTextInput50KG input{font,atlas,reinterpret_cast<const std::uint8_t*>(text.data()),text.size(),0,0};
                if(!game::layoutOriginalUiText50KG(input,s.textScratch,d.text))return fail(d,E::geometry);
                std::int32_t width=0,height=0;
                if(!game::roundOriginalUiTextMetric50KG(s.textScratch.widthBeforeScaleAndRounding,1,width)||
                    !game::roundOriginalUiTextMetric50KG(s.textScratch.firstGlyphHeightBeforeScaleAndRounding,1,height))return fail(d,E::geometry);
                game::OriginalUiRiderTextProjection50KG live;
                if(!widgets.projectText(id,line,s.textScratch.fontLineHeight,width,height,shown,live,d.widgetDetail)){
                    if(d.widgetDetail.error==game::OriginalUiRiderWidgetError50KG::hidden)continue;return fail(d,E::widget);
                }
                if(live.descriptor!=descriptor||live.line.text!=&text||live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128)
                    return fail(d,E::geometry);
                input.localX=live.position.positionedX;input.localY=live.position.positionedY;
                if(!game::layoutOriginalUiText50KG(input,s.textScratch,d.text))return fail(d,E::geometry);
                const auto count=s.textScratch.count;if(!count)continue;if(!s.capacity(count))return fail(d,E::capacity);
                const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
                for(unsigned q=0;q<count;++q)for(const auto& glyph:s.textScratch.quads[q].vertices){
                    assets::Vertex48B vertex{};vertex.position[0]=glyph.x;vertex.position[1]=glyph.y;vertex.position[2]=z;
                    vertex.uv[0]=glyph.u;vertex.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)vertex.color[c]=live.line.packetColor[c];s.vertices.push_back(vertex);
                }
                s.w.insert(s.w.end(),std::size_t(count)*4,1.f);s.fog.insert(s.fog.end(),std::size_t(count)*4,0.f);
                s.packets.push_back({first,count,binding,static_cast<std::uint32_t>(s.packets.size())});
            }
        }else if(widgets.iconDescriptor(id)){
            game::OriginalUiRiderIconProjection50KG live;
            if(!widgets.projectIcon(id,shown,live,d.widgetDetail))return fail(d,E::widget);
            if(live.images!=s.resources->images().get()||materialIndex>=materials.size()||materials[materialIndex].images.empty()||
               live.imageIndex!=materials[materialIndex].images[0])return fail(d,E::resource);
            // Icon geometry keeps its constructor dimensions. append selects
            // the actual live ATM frame without replacing these dimensions.
            if(!append(live.layout.quad,materialIndex,live.layout.packetDepth,d))return false;
        }
    }
    return true;
}
bool OriginalUiRiderDrawContribution50KG::projectControllerRequiredPopup(
    const game::OriginalUiControllerPopupRender50KG& render,D& d)noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.popup||!menuShared_||s.popupVisited||s.ordinary||s.page||s.top||s.finished||s.failed||
       !s.historyCurrent(this)||!menuShared_->menuPopupProjectionCurrent(*this,render))return fail(d,E::phase,0x2064C0);
    if(!s.popup->project(render,d.controllerPopup)){s.failed=true;return fail(d,E::geometry,d.controllerPopup.sourcePC);}
    const auto* geometry=s.popup->geometry();if(!geometry){s.failed=true;return fail(d,E::resource);}
    try {
        for(std::size_t i=0;i<geometry->packetCount();++i){const auto& packet=geometry->packets()[i];
            const auto role=static_cast<std::size_t>(packet.material);
            if(role>=s.popupMaterials.size()||bool(packet.quad)==bool(packet.text)){
                s.failed=true;return fail(d,E::geometry,packet.sourcePC);}
            const auto material=s.popupMaterials[role];
            if(material>=s.resources->materials()->size()){s.failed=true;return fail(d,E::material);}
            if(packet.quad){if(!append(*packet.quad,material,packet.packetDepth,d)){s.failed=true;return false;}continue;}
            const auto& text=*packet.text;const auto& source=(*s.resources->materials())[material];
            if(source.images.size()!=1||source.animated||source.material.passes.size()!=1||
               text.count>text.quads.size()||packet.packetColor[3]>128||packet.packetDepth!=16777214.f){
                s.failed=true;return fail(d,E::geometry,packet.sourcePC);}
            if(!text.count)continue;
            const auto* map=s.backend->staticImage(s.staticOwner,source.images[0]);
            const auto binding=map?s.find(&source.material.passes[0],map->combinedIndex):unresolved;
            if(binding==unresolved){s.failed=true;return fail(d,E::material);}
            if(!s.capacity(text.count)){s.failed=true;return fail(d,E::capacity);}
            for(std::size_t q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices)
                if(!std::isfinite(glyph.x)||!std::isfinite(glyph.y)||!std::isfinite(glyph.u)||!std::isfinite(glyph.v)){
                    s.failed=true;return fail(d,E::geometry,packet.sourcePC);}
            const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),ordinal=static_cast<std::uint32_t>(s.packets.size());
            for(std::size_t q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices){assets::Vertex48B vertex{};
                vertex.position[0]=glyph.x;vertex.position[1]=glyph.y;vertex.position[2]=packet.packetDepth;
                vertex.uv[0]=glyph.u;vertex.uv[1]=glyph.v;
                for(unsigned c=0;c<4;++c)vertex.color[c]=packet.packetColor[c];
                s.vertices.push_back(vertex);s.w.push_back(1);s.fog.push_back(0);
            }
            s.packets.push_back({first,text.count,binding,ordinal});
        }
        s.popupVisited=true;OriginalUiSharedDialogs50KG::Diagnostic shared;
        if(!menuShared_->completeMenuPopupProjection(*this,render,shared)){
            s.failed=true;return fail(d,E::phase,shared.sourcePC);}
        return true;
    }catch(...){s.failed=true;return fail(d,E::allocation);}
}
bool OriginalUiRiderDrawContribution50KG::projectOrdinaryWidgets(bool shown,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||(s.popup&&!s.popupVisited)||s.ordinary||s.page||s.failed)return fail(d,E::phase);
    if(!widgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectPage(const game::OriginalUiRiderStatInput50KG& input,bool shown,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.page||s.failed)return fail(d,E::phase);s.page=true;s.pageShown=shown;
    if(!shown)return true;
    struct Guard {bool& failed;bool complete=false;~Guard(){if(!complete)failed=true;}} guard{s.failed};
    if(!s.stats.applyAppearance(input,d.stat)||!s.stats.project(input,{s.clock.known,s.clock.advanceAnimations,s.clock.milliseconds},s.statFrame,d.stat)||
       !s.statDraw.project(s.statAssets,s.statFrame,s.bank,s.statOwner,d.statDraw)){s.failed=true;return fail(d,E::stat);}
    if(!s.resources->widgets()->setTextPosition(4,s.statFrame.lockedWidgetX,s.statFrame.lockedWidgetY,d.widgetDetail))return fail(d,E::widget);
    // PORT EXTENSION, NOT ORIGINAL: an add-on rider frame (stats input carries the
    // add-on's own registry number, so no stock portrait is projected) draws no
    // stock NN_bkg background either.
    const auto* addon=riderAddon50DL::overlayFor(this);
    if(addon&&input.selectedRider!=addon->statRider)return fail(d,E::input);
    for(const auto& packet:s.statDraw.packets()){
        const auto binding=s.find(&packet.resource->material.passes[0],packet.imageIndex);
        if(binding==unresolved)return fail(d,E::material);if(!s.capacity(packet.quadCount))return fail(d,E::capacity);
        const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);const auto begin=s.statDraw.vertices().begin()+std::size_t(packet.firstQuad)*4;
        s.vertices.insert(s.vertices.end(),begin,begin+std::size_t(packet.quadCount)*4);
        s.w.insert(s.w.end(),std::size_t(packet.quadCount)*4,1.f);s.fog.insert(s.fog.end(),std::size_t(packet.quadCount)*4,0.f);
        s.packets.push_back({first,packet.quadCount,binding,static_cast<std::uint32_t>(s.packets.size())});
    }
    if(addon){
        // PORT EXTENSION, NOT ORIGINAL (DLC step C): the add-on's own portrait (the
        // stat page's riderPortrait spot, 250/300+panelY, texture size, depth of the
        // stat objects) and its own NN_bkg-style background (the four mirrored
        // 320x224 quadrants, depth 16777200). A page that cannot draw is skipped.
        const auto* table=s.backend->addonModels();
        const auto* row=table&&addon->modelRow!=riderAddon50DL::Overlay::noRow?table->row(addon->modelRow):nullptr;
        const auto bound=[&](const std::string& name,const assets::OriginalUiMaterial50KG& material,std::uint32_t image,
            std::uint32_t& binding,const assets::Texture48B*& texture)->bool{
            const auto* map=row?s.backend->addonPageImage(addon->modelRow,image):nullptr;
            if(!map||material.passes.size()!=1)return false;
            binding=s.find(&material.passes[0],map->combinedIndex);
            const auto* t=s.bank->images()->textures()->texture(map->combinedIndex);
            texture=t?&t->texture:nullptr;(void)name;
            return binding!=unresolved&&texture&&texture->width&&texture->height;};
        const auto emit=[&](const std::array<game::OriginalUiOverlayVertex50KG,4>& v,std::uint32_t binding,float z)->bool{
            if(!s.capacity(1))return false;
            std::array<assets::Vertex48B,4> vertices{};constexpr unsigned order[]={0,1,3,2};
            for(unsigned i=0;i<4;++i){const auto& p=v[order[i]];auto& out=vertices[i];
                out.position[0]=p.x;out.position[1]=p.y;out.position[2]=z;out.uv[0]=p.u;out.uv[1]=p.v;
                out.color[0]=127;out.color[1]=127;out.color[2]=127;out.color[3]=63;}
            const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
            s.vertices.insert(s.vertices.end(),vertices.begin(),vertices.end());s.w.insert(s.w.end(),4,1.f);s.fog.insert(s.fog.end(),4,0.f);
            s.packets.push_back({first,1,binding,static_cast<std::uint32_t>(s.packets.size())});return true;};
        if(row&&row->portraitReady){std::uint32_t binding=unresolved;const assets::Texture48B* t=nullptr;
            if(!bound(row->portraitName,row->portraitMaterial,row->portraitImage,binding,t))addon->pageRefused="portrait not bound";
            else{const float x=250.f,y=300.f+input.panelY4DC,w=float(t->width),h=float(t->height);
                if(!std::isfinite(y)||!emit({{{x,y,0,0},{x+w,y,1,0},{x,y+h,0,1},{x+w,y+h,1,1}}},binding,16777214.f))return fail(d,E::capacity);
                addon->portraitDrawn=true;}
        }
        if(row&&row->backgroundReady){std::uint32_t binding=unresolved;const assets::Texture48B* t=nullptr;
            if(!bound(row->backgroundName,row->backgroundMaterial,row->backgroundImage,binding,t))addon->pageRefused="background not bound";
            else{const float u=319.f/float(t->width),v=223.f/float(t->height);
                const std::array<std::array<float,8>,4> q{{{0,0,320,224,0,0,u,v},{320,0,320,224,u,0,0,v},
                    {0,224,320,224,0,v,u,0},{320,224,320,224,u,v,0,0}}};
                for(const auto& e:q){
                    const std::array<game::OriginalUiOverlayVertex50KG,4> vv{{{e[0],e[1],e[4],e[5]},{e[0]+e[2],e[1],e[6],e[5]},
                        {e[0],e[1]+e[3],e[4],e[7]},{e[0]+e[2],e[1]+e[3],e[6],e[7]}}};
                    if(!emit(vv,binding,16777200.f))return fail(d,E::capacity);
                }
                addon->backgroundDrawn=true;}
        }
        guard.complete=true;return true;
    }
    const auto* background=s.resources->background(static_cast<std::uint8_t>(input.selectedRider));
    if(!background)return fail(d,E::resource);
    for(const auto& quad:background->quadrants)if(!append(quad,background->material,16777200.f,d))return false;
    guard.complete=true;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectExitText(const game::OriginalUiApertureHook50KG& hook,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertures||s.prompts||s.top||s.failed||s.exitProjected||!s.exitExpected)
        return fail(d,E::phase,0x1D75A4);
    struct Guard {bool& failed;bool complete=false;~Guard(){if(!complete)failed=true;}} guard{s.failed};
    if(hook.observed.state!=s.apertureAtBegin.state||hook.observed.top284!=s.apertureAtBegin.top284||
       hook.observed.bottom28C!=s.apertureAtBegin.bottom28C||!s.capacity(10))return fail(d,E::phase,0x1D75A4);
    auto* exit=s.resources->exitText(s.sourcePlayer);
    if(!exit||exit->snapshot().route.screen!=5||exit->snapshot().route.page!=s.sourcePlayer+1)return fail(d,E::resource);
    const auto materialIndex=s.resources->materialIndex("font/denmrk16top");
    const auto& materials=*s.resources->materials();if(materialIndex>=materials.size())return fail(d,E::material);
    const auto& material=materials[materialIndex];if(material.images.size()!=1||material.material.passes.size()!=1)return fail(d,E::material);
    const auto* map=s.backend->staticImage(s.staticOwner,material.images[0]);
    const auto binding=map?s.find(&material.material.passes[0],map->combinedIndex):unresolved;
    if(binding==unresolved)return fail(d,E::bank);
    game::OriginalUiExitTextProjection50KG live{};
    if(exit->projectRender2E0170(hook,live)!=game::OriginalUiExitTextStatus50KG::ok||!live.glyphs||
       live.images!=s.resources->images().get()||live.imageIndex!=material.images[0]||
       live.materialIdentity!=material.material.identity||live.assignedDepth!=1||live.packetColor[3]>128||live.glyphs->count>10)
        return fail(d,E::geometry,0x2E0170);
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
    for(std::size_t i=0;i<live.glyphs->count;++i)for(const auto& glyph:live.glyphs->quads[i].vertices){
        assets::Vertex48B v{};v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=16777214.f;
        v.uv[0]=glyph.u;v.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)v.color[c]=live.packetColor[c];s.vertices.push_back(v);
        s.w.push_back(1.f);s.fog.push_back(0.f);
    }
    if(live.glyphs->count)s.packets.push_back({first,live.glyphs->count,binding,static_cast<std::uint32_t>(s.packets.size())});
    s.exitProjected=true;guard.complete=true;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectAperture(std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& rectangle,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.prompts||s.top||s.failed||ordinal!=s.apertures||ordinal>1||
       rectangle.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rectangle.dirty3C||
       (s.exitExpected&&!s.exitProjected))return fail(d,E::phase);
    game::OriginalUiOverlayQuad50KG quad;quad.authoredDepth=1;quad.sourcePC=0x1D798C;quad.sourcePacketColor={{0,0,0,63}};
    constexpr unsigned order[]={0,2,1,3};for(unsigned i=0;i<4;++i){const auto& v=rectangle.vertices[order[i]];quad.vertices[i]={v.x,v.y,0,0};}
    if(!append(quad,s.resources->materialIndex("ui/textures/notexture"),16777214.f,d)){s.failed=true;return false;}
    ++s.apertures;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectPrompts(std::uint32_t mask,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertures!=2||s.prompts||s.top||s.failed||(mask&~15u))return fail(d,E::phase);
    for(const auto& prompt:*s.resources->prompts())if(mask&prompt.mask)
        if(!append(prompt.quad,prompt.material,16777214.f,d)){s.failed=true;return false;}
    s.prompts=true;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectOnTopWidgets(bool shown,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||s.top||s.failed||(s.pageShown&&(!s.prompts||s.apertures!=2)))return fail(d,E::phase);
    if(!widgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}

bool OriginalUiRiderDrawContribution50KG::Storage::schedule(std::vector<Binding>& input,
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
        for(std::size_t i=0;i<input.size();++i){if(input[i].addon)continue; // add-on rows rank below
            const auto& name=input[i].registeredName;
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
    // PORT EXTENSION, NOT ORIGINAL (DLC): an add-on pod's own menu material that
    // Mat_Stbl does not have ranks after its 3235 rows, in registry order (the
    // same rows the material table appends). Only static add-on rows reach here.
    {
        const auto& names=rr::assets::dlc50DL::addonPodUiMaterialNames();
        // Copies of shared original names rank after every own add-on name, one
        // rank per distinct name (in first-binding order); never a Mat_Stbl rank.
        std::vector<std::string> shared;
        for(std::size_t i=0;i<input.size();++i){
            if(!input[i].addon||ranks[i]!=unresolved||input[i].kind!=RegistrationKind::existingStatic)continue;
            if(!input[i].addonShared){
                for(std::size_t k=0;k<names.size()&&k<0x100000u;++k)
                    if(same(input[i].registeredName,names[k].c_str())){ranks[i]=3235u+static_cast<std::uint32_t>(k);break;}
                if(ranks[i]!=unresolved)continue; // else (a PSM-less page such as the portrait): extra list
            }
            std::size_t k=0;for(;k<shared.size();++k)if(same(input[i].registeredName,shared[k].c_str()))break;
            if(k==shared.size()){if(shared.size()>=0x10000u)return fail(d,E::schedule);shared.push_back(input[i].registeredName);}
            ranks[i]=3235u+static_cast<std::uint32_t>(names.size()+k);
        }
    }
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
            // PORT EXTENSION, NOT ORIGINAL (DLC): add-on rows (ranks after the 3235
            // Mat_Stbl rows) and stock rows never redirect into each other. Without
            // this an equivalent later add-on row absorbed a stock group (e.g. the
            // terminal icon prompt), changing stock representatives and the stock
            // frame's drained terminal. With no add-on rows nothing is skipped.
            if((prior.rank>=3235u)!=(next.rank>=3235u))continue;
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

bool OriginalUiRiderDrawContribution50KG::finish(D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||(s.popup&&(!s.popupVisited||!s.historyCurrent(this)))||!s.ordinary||!s.page||!s.top||s.failed||s.finished||
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
        s.pendingHistory=s.history;s.pendingLive.clear();s.menuUses.clear();
        OriginalUiSourceTerminal50KN terminal{};
        std::uint32_t terminalLeaf=unresolved,spanOrdinal=0;
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
            if(s.common){const auto& origin=s.bindings[s.packets[index].binding];
                if(origin.registration>=s.pendingHistory.size()){s.failed=true;return fail(d,E::schedule);}
                // Observations follow the real sorted FINALREP walk. Aliases
                // retain their origin token; only the final leaf receives the
                // actual native binding, in the same span/FIFO order.
                s.menuUses.push_back({s.pendingHistory[origin.registration].rank,
                    s.pendingHistory[representative.registration].rank,spanOrdinal,origin.image});
            }
            ++spanOrdinal;
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
        const auto* data=s.resources->widgets()->selectionData();const auto* page=data?data->page():nullptr;
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
        frame_.riderSession_=s.gpu->riderSession();frame_.riderTextures_=s.gpu->riderSparse()?s.gpu.get():nullptr;
        frame_.riderResidencySerial_=s.gpu->riderDemandSerial();
        if(s.common){
            for(std::size_t role=0;role<s.sharedBindings.size();++role){const auto index=s.sharedBindings[role];
                if(index>=s.bindings.size()||s.bindings[index].registration>=s.pendingHistory.size()){
                    s.failed=true;return fail(d,E::schedule);}
                const auto& record=s.pendingHistory[s.bindings[index].registration];
                if(record.kind!=RegistrationKind::sharedCanonical||record.rank!=3228+role||record.pass!=1){
                    s.failed=true;return fail(d,E::schedule);}
                s.commonPending[role]=record.state;
            }
            if(!s.common->stageMenu(this,s.popupRevision,frame_,*s.bank,s.menuUses,s.commonPending,d.controllerPopup)){
                s.failed=true;frame_.native_=nullptr;frame_.owner_=nullptr;return fail(d,E::schedule,d.controllerPopup.sourcePC);}
        }
        if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic shared;
            if(!menuShared_->sealMenu470Frame(*this,s.frameGeneration,shared)){
                s.failed=true;return fail(d,E::phase,shared.sourcePC);}}
        return true;
    }catch(...){s.failed=true;return fail(d,E::allocation);}
}
const OriginalUiSourceFrame50KM* OriginalUiRiderDrawContribution50KG::frame()const noexcept {
    if(!(storage_&&storage_->active&&storage_->finished&&!storage_->failed&&
        !storage_->historyPoisoned&&!storage_->submissionPending&&storage_->groupsValid&&!storage_->awaitingGroupRecord&&
        storage_->lifecycleSequence==storage_->backend->materialLifetime().sequence()&&
        storage_->bankGeneration==storage_->backend->generation()&&
        storage_->registryGeneration==storage_->backend->registryGeneration()&&frame_.native_&&storage_->historyCurrent(this)))return nullptr;
    if(menuShared_&&!menuShared_->menu470FrameCurrent(*this,frame_.frameGeneration_)&&
       !(menuAcceptedEpoch_==menuInvocationEpoch_&&storage_->acknowledgedGeneration==frame_.frameGeneration_))return nullptr;
    return &frame_;
}
const OriginalUiNativeDraw50KG* OriginalUiRiderDrawContribution50KG::draw()const noexcept {
    return storage_&&storage_->active&&storage_->finished&&!storage_->failed?&storage_->draw:nullptr;
}
bool OriginalUiRiderDrawContribution50KG::residentSourceFrameCurrent(const OriginalUiSourceFrame50KM& frame,
    const assets::OriginalUiImageBank50KG& bank)const noexcept {
    if(!storage_||&frame!=&frame_||frame.owner_!=this||frame.native_!=&storage_->draw||
       !frame.frameGeneration_||frame.frameGeneration_!=storage_->frameGeneration||frame.garageCanonical_||frame.directVideoPresent_)
        return false;
    const auto& s=*storage_;
    if(!s.active||!s.finished||s.failed||s.historyPoisoned||s.submissionPending||!s.historyCurrent(this)||
       !s.groupsValid||s.awaitingGroupRecord||s.bank.get()!=&bank||!bank.loaded()||
       s.bankGeneration!=s.backend->generation()||s.registryGeneration!=s.backend->registryGeneration()||
       s.lifecycleSequence!=s.backend->materialLifetime().sequence()||
       !s.gpu||!s.gpu->initialized()||!s.publication||s.gpu->publicationToken()!=s.publication)return false;
    if(s.popup&&(!menuShared_||!sharedMenuFrameCurrent(*menuShared_,menuInvocationEpoch_,frame.frameGeneration_)))return false;
    const auto images=bank.images();const auto* textures=images?images->textures():nullptr;
    if(!textures||s.gpu->textureCount()!=textures->textureCount())return false;
    if(s.gpu->riderSparse()){
        if(!menuSession_||s.gpu->riderSession()!=menuSession_||frame.riderSession_!=menuSession_||
           s.gpu->riderBank()!=&bank||frame.riderTextures_!=s.gpu.get()||!frame.riderResidencySerial_||
           frame.riderResidencySerial_!=s.gpu->riderDemandSerial())return false;
    }else if(frame.riderSession_||frame.riderTextures_||frame.riderResidencySerial_||
             !s.gpu->matchesSourceBank(*textures,s.publication))return false;
    const auto& native=*frame.native_;const auto& draw=native.draw;
    if(draw.textures!=s.gpu.get()||draw.texturePublication!=s.publication||draw.vertices!=s.orderedVertices.data()||
       draw.quadCount!=s.orderedVertices.size()/4||!draw.quadCount||s.orderedVertices.size()%4||
       draw.spans!=s.spans.data()||draw.spanCount!=s.spans.size()||draw.spanCount!=s.order.size()||
       native.states!=s.states.data()||native.stateCount!=s.states.size()||native.stateCount!=draw.spanCount||
       native.sourceRasterW!=s.orderedW.data()||native.sourceRasterWCount!=s.orderedW.size()||
       native.particleFog!=s.orderedFog.data()||native.particleFogCount!=s.orderedFog.size()||
       s.orderedW.size()!=s.orderedVertices.size()||s.orderedFog.size()!=s.orderedVertices.size())return false;
    std::uint32_t first=0;
    for(std::size_t i=0;i<s.order.size();++i){
        if(s.order[i]>=s.packets.size())return false;const auto& packet=s.packets[s.order[i]];
        if(!s.drawablePacket(packet.binding)||packet.binding>=s.bindings.size())return false;
        const auto& binding=s.bindings[packet.binding];const auto& span=s.spans[i];
        if(!span.quadCount||span.firstQuad!=first||span.quadCount!=packet.count||span.quadCount>draw.quadCount-first||
           span.textureIndex!=binding.image||!samplerSame(s.states[i].sampler,binding.prototype->native.sampler))return false;
        first+=span.quadCount;if(binding.image==unresolved)continue;
        const auto* cpu=textures->texture(binding.image);const auto* gpu=s.gpu->asset(binding.image);
        if(!cpu||!gpu||!gpu->name||cpu->identity!=gpu->identity)return false;
    }
    return first==draw.quadCount;
}
void OriginalUiRiderDrawContribution50KG::drain() noexcept {
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;frame_.terminal_={};
    frame_.riderSession_=nullptr;frame_.riderTextures_=nullptr;frame_.riderResidencySerial_=0;
    if(!storage_)return;auto& s=*storage_;
    if(s.submissionPending){s.poisonHistory(this);if(menuShared_)menuShared_->failMenu470Submission(*this);}
    if(s.active)s.backend->endFrame();s.active=false;s.finished=false;s.draw={};s.pendingHistory.clear();s.pendingLive.clear();
}
bool OriginalUiRiderDrawContribution50KG::sourceTeardownPending()const noexcept {
    return storage_&&(storage_->submittedRenderer||storage_->popupLeased||
        (menuShared_&&menuShared_->menu470PendingFor(*this)));
}
bool OriginalUiRiderDrawContribution50KG::retireSourceRenderer(VehicleWindowGl50DS& renderer,
    VehicleWindowDiagnostic50DS& rendererDiagnostic,D& d)noexcept {
    d={};rendererDiagnostic={};if(!storage_)return true;auto& s=*storage_;
    if(s.submissionPending||(s.submittedRenderer&&s.submittedRenderer!=&renderer))return fail(d,E::phase);
    const bool submitted=s.submittedRenderer!=nullptr;
    if(!renderer.retireOriginalUiSourceOwner(*this,rendererDiagnostic)||
       (submitted&&!rendererDiagnostic.submittedWorkCompleted))return fail(d,E::publication);
    s.submittedRenderer=nullptr;menuRetiredRenderer_=&renderer;
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic shared;
        if(!menuShared_->retireMenu470AfterRenderer(*this,renderer,shared))return fail(d,E::phase,shared.sourcePC);}
    if(s.popupLeased&&(!s.common||!s.common->releaseMenu(this,&renderer)))return fail(d,E::schedule,0x249BA0);
    s.popupLeased=false;
    // CPU publication and backend frame lifetime end separately in drain().
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;return true;
}
bool OriginalUiRiderDrawContribution50KG::beginSourceSubmission(std::uint64_t generation,const VehicleWindowGl50DS& renderer)const noexcept {
    if(!storage_||!frame()||generation!=frame_.frameGeneration_)return false;
    auto& s=*storage_;if((s.submittedRenderer&&s.submittedRenderer!=&renderer)||s.submissionPending||s.historyPoisoned||generation<=s.acknowledgedGeneration||
       generation<=s.attemptedGeneration||s.pendingHistory.size()!=s.history.size())return false;
    for(const auto& record:s.pendingHistory)if(!s.registrationCurrent(record))return false;
    if(s.common&&!s.common->beginMenu(this,s.popupRevision,frame_,renderer))return false;
    if(menuShared_&&!menuShared_->beginMenu470Submission(*this,generation,renderer)){
        if(s.common&&!s.common->cancelMenuBegin(this,s.popupRevision,frame_,renderer))s.poisonHistory(this);
        menuShared_->failMenu470Submission(*this);return false;}
    if(!s.backend->sourceSubmissionStarted(this,generation)){
        if(s.common&&!s.common->cancelMenuBegin(this,s.popupRevision,frame_,renderer))s.poisonHistory(this);
        if(menuShared_)menuShared_->failMenu470Submission(*this);return false;}
    s.submittedRenderer=&renderer;s.attemptedGeneration=generation;s.submissionPending=true;return true;
}
void OriginalUiRiderDrawContribution50KG::finishSourceSubmission(std::uint64_t generation,bool accepted,bool started)const noexcept {
    if(!storage_)return;auto& s=*storage_;
    const auto reject=[&]()noexcept {s.poisonHistory(this);if(menuShared_)menuShared_->failMenu470Submission(*this);};
    if(!s.submissionPending||generation!=frame_.frameGeneration_){reject();return;}
    s.submissionPending=false;
    if(accepted){
        // Validate the whole transaction before the first nonallocating swap.
        // Synchronous backend submission ownership forbids source destruction
        // in between; an invariant failure poisons rather than partially commits.
        if(s.lifecycleSequence!=s.backend->materialLifetime().sequence()||s.pendingHistory.size()!=s.history.size()){
            reject();return;
        }
        for(const auto& record:s.pendingHistory)if(!s.registrationCurrent(record)){reject();return;}
        for(const auto& pending:s.pendingLive){
            if(!pending.live||pending.live->states.size()!=pending.states.size()) {reject();return;}
            for(const auto& state:pending.states)if(!state.initialized()){reject();return;}
        }
    }
    // Every local accepted store is preflighted before the canonical commit.
    // Only the renderer's actual finish callback can commit either history.
    if(s.common&&!s.common->finishMenu(this,s.popupRevision,frame_,accepted,started)){reject();return;}
    if(accepted){
        for(auto& pending:s.pendingLive)pending.live->states.swap(pending.states);
        s.history.swap(s.pendingHistory);s.acknowledgedGeneration=generation;s.pendingHistory.clear();s.pendingLive.clear();
    }else{
        s.pendingLive.clear();if(started)s.poisonHistory(this);
    }
    if(menuShared_){if(!menuShared_->finishMenu470Submission(*this,generation,accepted,started)){reject();return;}
        if(accepted)menuAcceptedEpoch_=menuInvocationEpoch_;}
}
} // namespace rr::platform
