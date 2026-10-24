#include "original_ui_shared_dialog_material_history50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_options_draw_resources50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <exception>
#include <cstring>
#include <limits>
#include "original_ui_garage_canonical_submission50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_main_draw50kg.h"
#include "original_ui_options_draw_contribution50kg.h"
#include "../../../src/assets/original_standard_ipum_image_lease50la.h"
#include "../../../src/assets/original_standard_texture_resource50la.h"
#include <type_traits>
#include <utility>

namespace rr::platform {
namespace {
using E=OriginalUiControllerPopupResourceError50KG;
using D=OriginalUiControllerPopupResourceDiagnostic50KG;
using Archive=assets::OriginalUiArchiveId50KG;
bool fail(D& d,E e,std::uint32_t pc)noexcept {d.error=e;d.sourcePC=pc;return false;}
constexpr const char* names[]={"ui/textures/dimpopup","ui/textures/overlay1popup","font/venus16popup",
    "font/venus14popup","ui/textures/icon_x_popup","ui/textures/icon_tri_popup"};
constexpr const char* textures[]={nullptr,"overlay1","venus16","venus14","icon_x","icon_tri"};
constexpr const char* images[]={nullptr,"ui/textures/overlay1.pst","font/venus16.pst",
    "font/venus14.pst","ui/textures/icon_x.pst","ui/textures/icon_tri.pst"};
constexpr std::size_t offsets[]={70804,70826,70853,70872,70891,70917};
char fold(char c)noexcept{return c=='\\'?'/':c>='A'&&c<='Z'?char(c-'A'+'a'):c;}
template<class A,class B> bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
bool sameRuntime(const assets::OriginalUiMaterialRuntime50KG& a,const assets::OriginalUiMaterialRuntime50KG& b)noexcept {
    return a.overrideKnown==b.overrideKnown&&a.override435670==b.override435670&&
        a.callbackOverrideKnown==b.callbackOverrideKnown&&a.callbackOverride435678==b.callbackOverride435678;
}
bool registry(const assets::OriginalUiMemberView50KG& table)noexcept {
    if(!table.identity||!table.data||table.size!=70962)return false;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    std::size_t cursor=0,matched=0;std::uint32_t token=0;
    while(cursor<table.size){
        while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;
        if(cursor==table.size)break;
        const auto start=cursor;while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        if(token>=3228&&token<=3233){const auto role=token-3228;std::size_t n=0;
            if(start!=offsets[role])return false;
            for(;names[role][n];++n)if(start+n>=cursor||fold(char(table.data[start+n]))!=names[role][n])return false;
            if(start+n!=cursor)return false;++matched;
        }
        ++token;
    }
    return matched==6;
}
}
struct OriginalUiSharedDialogMaterialHistory50KG::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,popup,fonts;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> initialImages;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context;
    const ::original_main_entry50KG::Application* root=nullptr;
    OriginalUiOptionsVideoOwner50KG video;
    assets::OriginalUiMaterialRuntime50KG runtime;
    std::array<Key,6> keys;
    std::array<assets::OriginalUiMemberView50KG,3> canonicalSources{};
    std::array<const assets::OriginalUiMemberIdentity50KG*,3> canonicalTemplates{};
    std::array<History,6> history;
    const OriginalUiSharedDialogs50KG* prospectiveOwner=nullptr;
    const OriginalUiSharedDialogs50KG* attachedOwner=nullptr;
    const OriginalUiControllerPopupResources50KG* source47C=nullptr;
    const OriginalUiSourceOwner50KM* lease=nullptr;
    std::uint64_t revision=0;
    Phase phase=Phase::prepared;
    bool poisoned=false,transferred=false;
    std::array<const assets::OriginalStandardMaterialObject50LA*,3> canonical{};
    std::array<const assets::OriginalStandardMaterialState50LA*,3> canonicalState{};
    std::array<const std::array<assets::OriginalStandardMaterialNativeBinding50KG,2>*,3> canonicalBindings{};
    bool canonicalAttached=false,menuStaged=false,menuPending=false;
    std::array<History,6> stagedHistory;
    assets::OriginalStandardMaterialTable50LA::UiSharedHistoryReceipt menuReceipt;
    assets::OriginalStandardMaterialTable50LA::UiSharedHistorySlots menuSlots{};
    const assets::OriginalUiImageBank50KG* stagedBank=nullptr;
    bool menuRendererNew=false;
    const OriginalUiSourceFrame50KM* menuFrame=nullptr;
    const OriginalUiNativeDraw50KG* menuDraw=nullptr;
    const VehicleWindowGl50DS* renderer=nullptr;
    std::uint64_t menuGeneration=0,attemptedGeneration=0;
    OriginalUiGarageCanonicalSubmission50KG* garage=nullptr;
};
OriginalUiSharedDialogMaterialHistory50KG::~OriginalUiSharedDialogMaterialHistory50KG()noexcept {
    if(storage_&&storage_->attachedOwner)std::terminate();
}
bool OriginalUiSharedDialogMaterialHistory50KG::initialOwnerCurrent(const OriginalUiSharedDialogs50KG& owner)const noexcept {
    const auto app=owner.application_.lock();
    return owner.loadedCurrent()&&app&&owner.phase_==OriginalUiSharedDialogs50KG::Phase::loaded&&
        !owner.sharedMaterialHistory_&&!owner.initialMainAttached_&&!owner.initialMainResetPending_&&
        !owner.reorderStep_&&!owner.completedReorders_&&!owner.update470Count_&&!owner.update470Pending_&&
        !owner.demand470Pending_&&owner.loaded470Texts_==2&&owner.loaded470Quads_==3&&owner.constructed470Quads_==3&&
        owner.dialog470_.constructorStateOwned&&owner.dialog470_.loaded&&owner.controller47C_.loaded&&
        owner.controller47C_.resources&&app->controllerPopupResources==owner.controller47C_.resources&&
        owner.inputs_.root&&owner.inputs_.domain&&owner.inputs_.domain->prepared()&&
        owner.inputs_.context&&owner.inputs_.context->live()&&owner.inputs_.video&&owner.inputs_.popup;
}
bool OriginalUiSharedDialogMaterialHistory50KG::source47CTransferCurrent(
    const OriginalUiSharedDialogs50KG& owner,const OriginalUiControllerPopupResources50KG& source)const noexcept {
    return storage_&&storage_->phase==Phase::prepared&&!storage_->transferred&&!storage_->attachedOwner&&
        storage_->prospectiveOwner==&owner&&storage_->source47C==&source&&initialOwnerCurrent(owner)&&
        owner.controller47C_.resources.get()==&source&&source.sharedDialogBoundCurrent(owner)&&
        storage_->root==owner.inputs_.root&&sameOwner(storage_->domain,owner.inputs_.domain)&&
        sameOwner(storage_->context,owner.inputs_.context)&&sameOwner(storage_->popup,owner.inputs_.popup)&&
        sameOwner(storage_->video.lifetime,owner.inputs_.video);
}
bool OriginalUiSharedDialogMaterialHistory50KG::preparedSourcesCurrent(const OriginalUiOptionsColdInput50KG& in)const noexcept {
    return storage_&&sameOwner(storage_->appStart,in.appStart)&&sameOwner(storage_->popup,in.controllerPopup)&&
        sameOwner(storage_->fonts,in.fonts)&&sameOwner(storage_->video.lifetime,in.video.lifetime)&&
        storage_->video.width==in.video.width&&storage_->video.height==in.video.height&&
        storage_->video.colorBits==in.video.colorBits&&storage_->video.packetZMaximum==in.video.packetZMaximum&&
        storage_->video.zBufferFormat==in.video.zBufferFormat&&sameRuntime(storage_->runtime,in.runtime);
}
bool OriginalUiSharedDialogMaterialHistory50KG::takeColdFive(const OriginalUiSharedDialogs50KG& owner,
    const OriginalUiControllerPopupResources50KG& source,const std::array<Key,5>& keys,
    std::array<History,5>& history,D& d)noexcept {
    static_assert(std::is_nothrow_swappable_v<History>);
    if(!source47CTransferCurrent(owner,source)||!storage_->history[2].initialized()||storage_->history[2].bound())
        return fail(d,E::history,0x2055B0);
    for(std::size_t i=0;i<from47C.size();++i){const auto role=from47C[i];
        if(!history[i].initialized()||history[i].bound()||storage_->history[role].initialized()||
            assets::compareOriginalUiMaterialEquivalence50KG(keys[i],storage_->keys[role])!=assets::OriginalUiMaterialEquivalence50KG::equivalent)
            return fail(d,E::history,0x249BA0);
    }
    // All allocations, source identity checks and comparisons finished first.
    // Each former record becomes default/uninitialized; none is reinitialized.
    for(std::size_t i=0;i<from47C.size();++i)std::swap(history[i],storage_->history[from47C[i]]);
    storage_->transferred=true;return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::bootstrapInitial(OriginalUiSharedDialogs50KG& owner,
    const OriginalUiOptionsColdInput50KG& in,std::shared_ptr<const assets::OriginalUiScreenAssets50KG> bank,
    const std::vector<OriginalUiOptionsMaterialBinding50KG>& bindings,D& d)noexcept {
    d={};
    try {
        auto next=std::shared_ptr<OriginalUiSharedDialogMaterialHistory50KG>(new OriginalUiSharedDialogMaterialHistory50KG);
        if(!next->initialOwnerCurrent(owner)||!bank||!bank->loaded()||!in.appStart||!in.controllerPopup||!in.fonts||
            in.appStart->identity()!=assets::originalUiArchiveIdentity50KG(Archive::appStart)||
            in.controllerPopup->identity()!=assets::originalUiArchiveIdentity50KG(Archive::controllerPopup)||
            in.fonts->identity()!=assets::originalUiArchiveIdentity50KG(Archive::fonts)||
            !sameOwner(in.controllerPopup,owner.inputs_.popup)||!sameOwner(in.video.lifetime,owner.inputs_.video))
            return fail(d,E::identity,0x2055B0);
        const auto table=in.appStart->find("AppStart/Mat_Stbl.txt");
        if(!registry(table))return fail(d,E::identity,0x298724);
        next->storage_=std::make_unique<Storage>();auto& s=*next->storage_;
        s.appStart=in.appStart;s.popup=in.controllerPopup;s.fonts=in.fonts;s.initialImages=std::move(bank);
        s.video=in.video;s.runtime=in.runtime;s.root=owner.inputs_.root;s.domain=owner.inputs_.domain;s.context=owner.inputs_.context;
        s.prospectiveOwner=&owner;s.source47C=owner.controller47C_.resources.get();
        for(std::size_t i=0;i<registryTokens.size();++i){d.registryToken=registryTokens[i];
            const OriginalUiOptionsMaterialBinding50KG* found=nullptr;
            for(const auto& b:bindings)if(b.registryToken==registryTokens[i]){if(found)return fail(d,E::identity,0x29BEA0);found=&b;}
            const auto psm=in.controllerPopup->find(std::string(names[i])+".psm");
            if(!found||found->registeredName!=names[i]||found->registryIdentity!=table.identity||!psm.identity||
                found->material.identity!=psm.identity||found->material.passes.size()!=1||found->images.size()!=(i?1u:0u))
                return fail(d,E::identity,0x247110);
            const auto& pass=found->material.passes[0];
            if(i<3){s.canonicalSources[i]=psm;s.canonicalTemplates[i]=pass.inheritedTemplate;}
            if(pass.identity!=psm.identity||pass.pass!=1||pass.provenance.archive!=in.controllerPopup->identity()||
                pass.provenance.origin!=assets::OriginalUiMaterialOrigin50KG::authoredPsm||pass.shaderPassGroup!="popup"||
                pass.microprogram!="ScreenObject"||pass.textureCount!=(i?1u:0u)||(i&&pass.textureName!=textures[i]))
                return fail(d,E::identity,0x247110);
            const auto* image=i?s.initialImages->sourceProvenance(found->images[0]):nullptr;
            if(i){const auto& archive=(i==2||i==3)?in.fonts:in.controllerPopup;
                const auto member=archive->find(images[i]);
                if(!image||!member.identity||image->memberIdentity!=member.identity||image->pstOffset||image->pstBytes!=member.size)
                    return fail(d,E::identity,0x2472F8);
            }
            assets::OriginalUiMaterialEquivalenceDiagnostic50KG detail;
            if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,image,s.keys[i],detail))return fail(d,E::material,detail.sourcePC);
        }
        assets::OriginalUiMaterialBindingDiagnostic50KG detail;
        if(!s.history[2].initialize(s.keys[2],detail))return fail(d,E::material,detail.sourcePC);
        if(!next->bindInitialCanonical(d))return false;
        // Source47C proves its exact storage, lease, revision and unbound state.
        // There is no fallback to initialize the other five histories cold.
        if(!owner.controller47C_.resources->transferInitialHistoryTo(owner,next,d))return false;
        s.attachedOwner=&owner;s.phase=Phase::attached;
        owner.sharedMaterialHistory_=std::move(next);d={};return true;
    }catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiSharedDialogMaterialHistory50KG::bootstrapInitial(OriginalUiSharedDialogs50KG& owner,
    const OriginalUiMainColdInput50KG& in,std::shared_ptr<const assets::OriginalUiScreenAssets50KG> bank,
    const std::vector<OriginalUiMainMaterialBinding50KG>& bindings,D& d)noexcept {
    try {
        OriginalUiOptionsColdInput50KG mapped;mapped.appStart=in.appStart;mapped.controllerPopup=in.controllerPopup;
        mapped.fonts=in.fonts;mapped.runtime=in.runtime;
        mapped.video={in.video.lifetime,in.video.width,in.video.height,in.video.colorBits,in.video.packetZMaximum,in.video.zBufferFormat,{}};
        std::vector<OriginalUiOptionsMaterialBinding50KG> materials;
        for(const auto& b:bindings)for(const auto token:registryTokens)if(b.registryToken==token){
            OriginalUiOptionsMaterialBinding50KG copy;copy.registeredName=b.registeredName;copy.registryToken=b.registryToken;
            copy.registryIdentity=b.registryIdentity;copy.material=b.material;copy.images=b.images;materials.push_back(std::move(copy));
        }
        return bootstrapInitial(owner,mapped,std::move(bank),materials,d);
    }catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiSharedDialogMaterialHistory50KG::preflight47CRetirement(const OriginalUiSharedDialogs50KG& owner)const noexcept {
    // Poison never grants reuse, but does not prohibit authentic final cleanup.
    return storage_&&storage_->phase==Phase::attached&&storage_->attachedOwner==&owner&&
        owner.current()&&owner.sharedMaterialHistory_.get()==this&&!storage_->lease&&
        storage_->source47C==owner.controller47C_.resources.get()&&storage_->root==owner.inputs_.root&&
        sameOwner(storage_->domain,owner.inputs_.domain)&&sameOwner(storage_->context,owner.inputs_.context)&&
        sameOwner(storage_->video.lifetime,owner.inputs_.video);
}
bool OriginalUiSharedDialogMaterialHistory50KG::finish47CPayload(const OriginalUiSharedDialogs50KG& owner,
    const OriginalUiControllerPopupResources50KG& source)noexcept {
    if(!preflight47CRetirement(owner)||storage_->source47C!=&source||source.storage_||
        !owner.controllerRetirementCurrent()||owner.retirementPayloadStep_!=9)return false;
    storage_->source47C=nullptr;storage_->phase=Phase::awaiting470;return true;
}
bool OriginalUiSharedDialogs50KG::sharedHistoryFinalRetirementCurrent()const noexcept {
    // reference00 reaches zero at142590 BEGIN, not at1413E0 completion.
    // These exact Scope flags and null slots are written only AFTER each real
    // final1413E0 decrements its counted object and relinquishes the slot.
    if(!current()||phase_!=Phase::retiring||retiringOrdinal_||retirementPayloadStep_!=6||
        !scope_||!scope_->live().okay()||scope_->sharedDialogsOwner_||
        scope_->sharedDialogs_[0]||scope_->sharedDialogs_[1]||
        scope_->sharedDialogRetiring_[0]||scope_->sharedDialogRetiring_[1]||
        !scope_->sharedDialogRetired_[0]||!scope_->sharedDialogRetired_[1]||
        scope_->sharedDialogDetached_||scope_->sharedDialogReorderOrdinal_||
        scope_->dispatcher_.firstChild18||!retirementDisplayPinned_||!retirementDisplay_)return false;
    const auto issued=inputs_.root->raceDisplayOwner50LA_.lock();
    const auto& display=*retirementDisplay_;
    return sameOwner(issued,retirementDisplay_)&&display.application_==inputs_.root&&
        sameOwner(display.video_,inputs_.video)&&display.frontendOwnerCurrent50KG()&&display.canRetain();
}
bool OriginalUiSharedDialogMaterialHistory50KG::finishAfter470(OriginalUiSharedDialogs50KG& owner,D& d)noexcept {
    d={};
    if(!storage_||storage_->phase!=Phase::awaiting470||storage_->attachedOwner!=&owner||
        !owner.sharedHistoryFinalRetirementCurrent()||owner.sharedMaterialHistory_.get()!=this||storage_->lease||
        owner.phase_!=OriginalUiSharedDialogs50KG::Phase::retiring||owner.retiringOrdinal_!=0||owner.retirementPayloadStep_!=6||
        owner.controller47C_.resources||owner.controller47C_.entity.reference00||owner.dialog470_.entity.reference00||
        owner.dialog470_.message1B0||owner.dialog470_.alternate220||owner.dialog470_.dim160||
        owner.loaded470Texts_||owner.loaded470Quads_||owner.constructed470Quads_||
        !owner.retirementDisplay_||!owner.retirementDisplayPinned_||
        storage_->root!=owner.inputs_.root||!sameOwner(storage_->domain,owner.inputs_.domain)||
        !sameOwner(storage_->context,owner.inputs_.context)||!sameOwner(storage_->video.lifetime,owner.inputs_.video))
        return fail(d,E::busy,0x1413E0);
    // No fabricated clean state or revision reset: discard only at the exact
    // authenticated lifetime end. Caller then releases its retained pointer.
    storage_->phase=Phase::retired;storage_->attachedOwner=nullptr;storage_.reset();return true;
}
bool OriginalUiSharedDialogs50KG::sharedHistoryCanonicalOwnersCurrent(
    std::array<const assets::OriginalStandardMaterialObject50LA*,3>& out)const noexcept {
    if(!loadedCurrent()||!inputs_.domain||!inputs_.context||!inputs_.context->live()||
       inputs_.context->videoIdentity()!=inputs_.video.get()||!dialog470_.dim160||
       !dialog470_.dim160->primitives30||dialog470_.dim160->count34<=0||
       !dialog470_.message1B0||!dialog470_.alternate220)return false;
    const auto table=inputs_.domain->materials382B38();if(!table)return false;
    const auto primitive=[&](const auto* p,std::int32_t row){return p&&p->materialStored&&p->materialRetained&&
        p->material00==row&&sameOwner(p->domain,inputs_.domain)&&sameOwner(p->context,inputs_.context);};
    for(std::int32_t i=0;i<dialog470_.dim160->count34;++i)
        if(!primitive(dialog470_.dim160->primitives30[static_cast<std::size_t>(i)].get(),3228))return false;
    for(const auto& strip:dialog470_.strips)
        if(!primitive(strip.material54,3229)||strip.material54!=strip.primitiveOwner_.get()||
           !sameOwner(strip.domain_,inputs_.domain)||!sameOwner(strip.context_,inputs_.context))return false;
    for(const auto* text:{dialog470_.message1B0.get(),dialog470_.alternate220.get()}){
        if(!sameOwner(text->domain_,inputs_.domain)||!sameOwner(text->context_,inputs_.context)||
           !text->primitives4_||!text->publishedPrimitives_)return false;
        for(std::size_t i=0;i<text->publishedPrimitives_;++i)
            if(!text->primitives4_[i]||!text->primitives4_[i]->materialStored||
               !text->primitives4_[i]->materialRetained||text->primitives4_[i]->material00!=3230)return false;
    }
    for(std::size_t i=0;i<out.size();++i){assets::OriginalStandardMaterialRowView50LA row;
        if(!table->row(static_cast<std::int32_t>(3228+i),row)||!row.object04||!row.references08||
           *row.references08<=0||!row.object04->constructed29B550())return false;
        out[i]=row.object04;
    }
    return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::bindInitialCanonical(D& d)noexcept {
    auto& s=*storage_;std::array<const assets::OriginalStandardMaterialObject50LA*,3> objects{};
    if(!s.prospectiveOwner||!s.prospectiveOwner->sharedHistoryCanonicalOwnersCurrent(objects))return fail(d,E::identity,0x298E60);
    for(std::size_t i=0;i<objects.size();++i){const auto* object=objects[i];
        const auto* source=object->source();const auto* bytes=source?source->originalBytes():nullptr;
        const auto member=s.canonicalSources[i];
        assets::OriginalStandardMaterialPassView50LA pass;
        assets::OriginalStandardProgramRowView50LA program;
        if(!member.identity||!member.data||!bytes||bytes->size()!=member.size||
           std::memcmp(bytes->data(),member.data,member.size)||!object->passCount48()||*object->passCount48()!=1||
           !object->pass4C(0,pass)||!pass.state30||pass.state30->contextCount()!=1||!pass.nativeBindings||
           (*pass.nativeBindings)[0].bound()||(*pass.nativeBindings)[1].bound()||
           !pass.mapCount20||*pass.mapCount20!=(i?1u:0u)||!pass.flags21||((*pass.flags21&2u)!=0)!=s.keys[i].enabled_||
           !pass.uv1E||(*pass.uv1E)[0]!=s.keys[i].uvIndex_||!pass.constructionTemplate||
           pass.constructionTemplate->identity!=s.canonicalTemplates[i]||
           !pass.program18||!s.domain->programs382B70()->row(*pass.program18,program)||!program.canonicalName||
           std::strcmp(program.canonicalName,"ScreenObject")||bool(*pass.flags21&4u)!=s.keys[i].fillColor_||
           pass.animation10||pass.textureAnimation10||!pass.group22||*pass.group22<0||
           !pass.references04||*pass.references04<=0||object->textures383400()!=s.domain->textures383400().get())
            return fail(d,E::identity,0x247110);
        for(std::size_t f=0;f<s.keys[i].state_.size();++f)
            if(pass.state30->fields[f].value!=s.keys[i].state_[f])return fail(d,E::history,0x249BA0);
        for(std::size_t f=0;f<s.keys[i].sampler_.size();++f)
            if(pass.state30->sampler[f].value!=s.keys[i].sampler_[f])return fail(d,E::history,0x249BA0);
        if(pass.state30->videoDepthBase2E0!=s.context->depthBufferBase2E0()||
           pass.state30->videoDepthFormat5C!=s.context->depthBufferFormat5C()||
           pass.state30->runtime435670!=s.context->runtime435670())return fail(d,E::video,0x2476A0);
        s.canonical[i]=object;s.canonicalState[i]=pass.state30;s.canonicalBindings[i]=pass.nativeBindings;
    }
    s.canonicalAttached=true;return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::sameCanonicalHistory(std::size_t role,const History& h,
    const assets::OriginalStandardMaterialStateContext50LA& context,
    const assets::OriginalStandardMaterialNativeBinding50KG& native)const noexcept {
    if(role>=3||!h.initialized_||!h.effective_.prepared_||h.bound_!=native.bound()||
       assets::compareOriginalUiMaterialEquivalence50KG(h.construction_,storage_->keys[role])!=assets::OriginalUiMaterialEquivalence50KG::equivalent)
        return false;
    for(std::size_t i=0;i<h.effective_.state_.size();++i)if(context.fields[i].value!=h.effective_.state_[i])return false;
    for(std::size_t i=0;i<h.effective_.sampler_.size();++i)if(context.sampler[i].value!=h.effective_.sampler_[i])return false;
    if(!h.bound_)return !native.bound();
    return h.member_==native.member_&&h.pstOffset_==native.pstOffset_&&h.pstBytes_==native.pstBytes_&&
        h.storedMip_==native.storedMip_&&h.mipHeader_==native.header_&&h.mipPayload_==native.payload_&&
        h.width_==native.width_&&h.height_==native.height_&&h.kind_==native.kind_&&
        (h.kind_==2?h.ipumPolicy_==assets::OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1:
                   h.ipumPolicy_==assets::OriginalUiIpumColorPolicy50KG::unavailable);
}
bool OriginalUiSharedDialogMaterialHistory50KG::canonicalCurrent()const noexcept {
    if(!storage_||!storage_->canonicalAttached||storage_->phase!=Phase::attached||!storage_->attachedOwner||
       storage_->poisoned||!storage_->context||!storage_->context->live()||
       storage_->attachedOwner->sharedMaterialHistory_.get()!=this||
       !storage_->domain->materials382B38()->nativeBindingHistoryUsable())return false;
    auto& s=*storage_;std::array<const assets::OriginalStandardMaterialObject50LA*,3> objects{};
    if(!s.attachedOwner->sharedHistoryCanonicalOwnersCurrent(objects))return false;
    for(std::size_t i=0;i<objects.size();++i){assets::OriginalStandardMaterialPassView50LA pass;
        if(objects[i]!=s.canonical[i]||!objects[i]->pass4C(0,pass)||pass.state30!=s.canonicalState[i]||
           pass.nativeBindings!=s.canonicalBindings[i]||!pass.state30||!pass.nativeBindings||
           (*pass.nativeBindings)[1].bound()||!sameCanonicalHistory(i,s.history[i],*pass.state30,(*pass.nativeBindings)[0]))return false;
    }
    return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::rejoinBank(const OriginalUiOptionsColdInput50KG& in,
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> bank,
    const std::vector<OriginalUiOptionsMaterialBinding50KG>& bindings,D& d)const noexcept {
    d={};if(!preparedSourcesCurrent(in)||!canonicalCurrent()||!bank||!bank->loaded())return fail(d,E::history,0x249BA0);
    try {for(std::size_t role=0;role<6;++role){const OriginalUiOptionsMaterialBinding50KG* found=nullptr;
        for(const auto& b:bindings)if(b.registryToken==registryTokens[role]){if(found)return fail(d,E::identity,0x29BEA0);found=&b;}
        const auto member=storage_->popup->find(std::string(names[role])+".psm");
        if(!found||found->registeredName!=names[role]||found->registryIdentity!=in.appStart->find("AppStart/Mat_Stbl.txt").identity||
           found->material.identity!=member.identity||found->material.passes.size()!=1||found->images.size()!=(role?1u:0u))
            return fail(d,E::identity,0x247110);
        const auto& pass=found->material.passes[0];
        const auto* provenance=role?bank->sourceProvenance(found->images[0]):nullptr;
        if(!member.identity||pass.identity!=member.identity||pass.pass!=1||
           pass.provenance.archive!=storage_->popup->identity()||pass.provenance.origin!=assets::OriginalUiMaterialOrigin50KG::authoredPsm||
           pass.shaderPassGroup!="popup"||pass.microprogram!="ScreenObject"||pass.textureCount!=(role?1u:0u)||
           (role&&pass.textureName!=textures[role]))return fail(d,E::identity,0x247110);
        if(role){const auto& archive=(role==2||role==3)?storage_->fonts:storage_->popup;
            const auto image=archive->find(images[role]);
            if(!provenance||!image.identity||provenance->memberIdentity!=image.identity||
               provenance->pstOffset||provenance->pstBytes!=image.size)return fail(d,E::identity,0x2472F8);
        }
        Key key;assets::OriginalUiMaterialEquivalenceDiagnostic50KG detail;
        if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,provenance,key,detail)||
           assets::compareOriginalUiMaterialEquivalence50KG(key,storage_->keys[role])!=assets::OriginalUiMaterialEquivalence50KG::equivalent)
            return fail(d,E::history,0x249BA0);
    }return true;}catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiSharedDialogMaterialHistory50KG::acquire(const OriginalUiSourceOwner50KM* owner,std::uint64_t& revision,D& d)noexcept {
    d={};if(!owner||!canonicalCurrent()||storage_->lease||storage_->menuStaged||storage_->garage)
        return fail(d,E::history,0x249BA0);
    storage_->lease=owner;revision=storage_->revision;return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::current(const OriginalUiSourceOwner50KM* owner,std::uint64_t revision)const noexcept {
    return owner&&storage_&&storage_->lease==owner&&storage_->revision==revision&&canonicalCurrent();
}
bool OriginalUiSharedDialogMaterialHistory50KG::copyHistory(const OriginalUiSourceOwner50KM* owner,
    std::uint64_t revision,std::uint32_t token,const Key& key,History& out,D& d)const noexcept {
    d={};d.registryToken=token;
    if(!current(owner,revision)||token<3228||token>3233)return fail(d,E::history,0x249BA0);
    const auto role=token-3228;
    if(assets::compareOriginalUiMaterialEquivalence50KG(key,storage_->keys[role])!=assets::OriginalUiMaterialEquivalence50KG::equivalent)
        return fail(d,E::history,0x249BA0);
    try{out=storage_->history[role];return true;}catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiSharedDialogMaterialHistory50KG::authenticateImage(std::size_t role,
    const assets::OriginalUiImageBank50KG& bank,std::uint32_t index,const History& h,
    assets::OriginalStandardMaterialNativeBinding50KG& history,D& d)const noexcept {
    if(role==0||role>=3||!storage_)return fail(d,E::identity,0x249610);
    assets::OriginalStandardMaterialPassView50LA pass;assets::OriginalStandardTextureRowView50LA row;
    if(!storage_->canonical[role]->pass4C(0,pass)||!pass.texture1A||
       !storage_->domain->textures383400()->row((*pass.texture1A)[0],row)||!row.object04||
       !row.references08||*row.references08<=0)return fail(d,E::identity,0x249610);
    const auto* texture=row.object04;
    const auto* flags=texture->flagsF8();const auto* known=texture->flagsF8KnownMask();
    if(!flags||!known||!(*known&1u)||bool(*flags&1u)!=(h.kind_==2))return fail(d,E::material,0x249818);
    using assets::OriginalStandardTextureLevelView50LA;
    using assets::OriginalUiIpumColorPolicy50KG;
    using assets::OriginalStandardMaterialNativeBinding50KG;
    const auto images=bank.images();const auto member=bank.sourceMember(index);
    const auto* provenance=images?images->sourceProvenance(index):nullptr;
    const auto* decoded=images?images->decodedTexture(index):nullptr;
    const auto* published=images&&images->textures()?images->textures()->texture(index):nullptr;
    const auto* original=texture->originalBytes();
    const auto* kind=texture->sourceKind();
    std::size_t offset=0,bytes=0;
    if(!bank.loaded()||!member.identity||!member.data||!provenance||!decoded||!published||
       !original||original->size()!=member.size||
       std::memcmp(original->data(),member.data,member.size)!=0||!kind||*kind!=decoded->kind||
       !texture->pstExtent24E880(offset,bytes)||offset!=provenance->pstOffset||
       bytes!=provenance->pstBytes||decoded->revision!=1||decoded->flags||decoded->kind>2||
       decoded->mips.empty()||decoded->mips.size()>7)
        return fail(d,E::material,0x24B254);
    const auto& base=decoded->mips.back();OriginalStandardTextureLevelView50LA level;
    // ScreenAssets decodes the exact selected PST slice, so its mip offsets
    // are PST-relative. Canonical TextureResource retains full-member offsets,
    // including an ATM child's real nonzero PST origin. Prove both ranges
    // before adding that origin; never substitute another frame or image.
    const std::size_t headerBytes=decoded->kind==2?20u:16u;
    if(offset>member.size||bytes>member.size-offset||base.source_header_offset>bytes||
       headerBytes>bytes-base.source_header_offset||
       base.source_indices_offset!=base.source_header_offset+headerBytes||base.source_indices_offset>=bytes){
        
        return fail(d,E::material,0x249780);
    }
    const auto memberHeader=offset+base.source_header_offset;
    const auto memberIndices=offset+base.source_indices_offset;
    if(!texture->level100(0,level)||level.sourceMipOrdinal+1!=decoded->mips.size()||
       level.width104!=static_cast<std::int32_t>(base.width)||level.height110!=static_cast<std::int32_t>(base.height)||
       level.sourceHeaderOffset!=memberHeader||level.sourceIndicesOffset!=memberIndices||
       base.width<16||base.height<16||base.width>1024||base.height>1024||
       (base.width&(base.width-1))||(base.height&(base.height-1))||
       std::uint64_t(base.width)*base.height*4!=base.rgba.size()||
       published->texture.width!=base.width||published->texture.height!=base.height||published->texture.rgba!=base.rgba)
        return fail(d,E::material,0x249780);
    const auto* ipum=images->ipumProvenance(index);
    if(decoded->kind==2){
        if(!ipum||ipum->colorPolicyKind!=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1||
           ipum->sourcePstSha256!=member.identity->sha256||ipum->mipCount!=decoded->mips.size()||
           (!level.garageIpumProvenance&&(!level.hostImage||!level.hostImage->rgba()||
            level.hostImage->rgbaBytes()!=base.rgba.size()||
            std::memcmp(level.hostImage->rgba(),base.rgba.data(),base.rgba.size())!=0)))
            return fail(d,E::material,0x249818);
        if(level.garageIpumProvenance){
            // Selected19E and UIGarage both contain the identical spcgarge PST.
            // The canonical1326A0 cache keeps the first real resource/lease;
            // ImageBank may borrow the later authenticated archive. Full PST
            // bytes, native image bytes and color policy must still agree.
            const auto& retained=*level.garageIpumProvenance;
            if(level.storage!=assets::OriginalStandardTextureLevelStorage50LA::garageIpumCompressed1555||
               level.hostImage||!retained.decodedInProcess||!ipum->decodedInProcess||
               retained.colorPolicyKind!=ipum->colorPolicyKind||
               retained.sourcePstSha256!=ipum->sourcePstSha256||
               retained.mipCount!=decoded->mips.size()||!level.rgba8888||
               level.rgba8888Bytes!=base.rgba.size()||
               std::memcmp(level.rgba8888,base.rgba.data(),base.rgba.size())!=0)
                return fail(d,E::material,0x249818);
        }
    }else if(ipum)return fail(d,E::material,0x249818);
    OriginalStandardMaterialNativeBinding50KG bound;
    bound.member_=member.identity;bound.pstOffset_=offset;bound.pstBytes_=bytes;
    // History retains its existing (member, PST origin, PST-relative offsets)
    // tuple. equivalent() compares that tuple; it does not index member bytes.
    bound.storedMip_=decoded->mips.size()-1;bound.header_=base.source_header_offset;
    bound.payload_=base.source_indices_offset;bound.width_=base.width;bound.height_=base.height;bound.kind_=decoded->kind;
    if(!sameCanonicalHistory(role,h,*storage_->canonicalState[role],bound)){
        // Compare after-state below in stageMenu; this helper authenticates
        // source image provenance, not a claim that the future bind occurred.
        if(!h.bound_||h.member_!=bound.member_||h.pstOffset_!=bound.pstOffset_||h.pstBytes_!=bound.pstBytes_||
           h.storedMip_!=bound.storedMip_||h.mipHeader_!=bound.header_||h.mipPayload_!=bound.payload_||
           h.width_!=bound.width_||h.height_!=bound.height_||h.kind_!=bound.kind_)return fail(d,E::history,0x249610);
    }
    history=bound;return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::stageMenu(const OriginalUiSourceOwner50KM* owner,
    std::uint64_t revision,const OriginalUiSourceFrame50KM& frame,const assets::OriginalUiImageBank50KG& bank,
    const std::vector<MenuUse>& uses,const std::array<History,6>& candidate,D& d)noexcept {
    d={};if(!current(owner,revision)||storage_->menuStaged||storage_->garage||storage_->menuPending||
       storage_->revision==std::numeric_limits<std::uint64_t>::max()||frame.owner_!=owner||!frame.native_||
       !frame.frameGeneration_||frame.garageCanonical_||!bank.loaded())return fail(d,E::history,0x249BA0);
    auto& s=*storage_;const auto& draw=frame.native_->draw;const auto images=bank.images();
    if(!images||!images->textures()||!draw.textures||!draw.textures->matchesSourceBank(*images->textures(),draw.texturePublication)||
       !draw.spans||!frame.native_->states||draw.spanCount!=frame.native_->stateCount)return fail(d,E::identity,0x24B26C);
    try {
        auto expected=s.history;std::array<bool,3> touched{};std::array<std::uint32_t,3> imageIndex{};
        std::uint32_t last=0;bool haveLast=false;
        for(const auto& use:uses){
            if(use.spanIndex>=draw.spanCount||(haveLast&&use.spanIndex<=last))return fail(d,E::identity,0x29C490);
            last=use.spanIndex;haveLast=true;const auto& span=draw.spans[use.spanIndex];
            if(!span.quadCount||use.imageIndex!=span.textureIndex)return fail(d,E::identity,0x24B26C);
            const bool originCanonical=use.originToken>=3228&&use.originToken<=3230;
            const bool finalCanonical=use.representativeToken>=3228&&use.representativeToken<=3230;
            // No origin alias receives its final leaf's accepted history. An
            // unrepresented counterpart cannot be bridged by token equality.
            if(originCanonical&&!finalCanonical)return fail(d,E::canonicalBridgePending,0x29C150);
            if(use.representativeToken<3228||use.representativeToken>3233)continue;
            const auto role=use.representativeToken-3228;
            if(!expected[role].construction_.enabled_)return fail(d,E::history,0x24B1B0);
            if(expected[role].construction_.mapCount_){assets::OriginalUiMaterialBindingDiagnostic50KG detail;
                if(!expected[role].recordDrainedBinding(bank,use.imageIndex,detail))return fail(d,E::material,detail.sourcePC);
            }else if(use.imageIndex!=assets::unresolvedMaterialTexture50EA||
                span.materialMode!=OriginalUiMaterialMode50KG::untexturedVertexColor)return fail(d,E::identity,0x249BA0);
            if(finalCanonical){touched[role]=true;imageIndex[role]=use.imageIndex;}
        }
        for(std::size_t i=0;i<6;++i)
            if(assets::compareOriginalUiMaterialEquivalence50KG(candidate[i].construction_,s.keys[i])!=assets::OriginalUiMaterialEquivalence50KG::equivalent||
               candidate[i].compare(expected[i])!=assets::OriginalUiMaterialEquivalence50KG::equivalent)return fail(d,E::history,0x249BA0);
        // Copying histories can allocate. Finish every such conversion before
        // the table lease or renderer work; accepted commits below only swap.
        s.stagedHistory=candidate;
        const auto table=s.domain->materials382B38();
        if(!table->beginUiSharedHistory(this,s.context.get(),s.menuReceipt,s.menuSlots))return fail(d,E::busy,0x249BA0);
        bool okay=true;
        for(std::size_t i=0;i<3&&okay;++i){auto& slot=s.menuSlots[i];
            if(slot.object!=s.canonical[i]||slot.state30!=s.canonicalState[i]||slot.nativeBindings!=s.canonicalBindings[i]||
               !sameCanonicalHistory(i,s.history[i],slot.beforeContext,slot.beforeNativeBindings[0])){okay=false;break;}
            slot.write=touched[i];if(!slot.write)continue;
            if(i==0){slot.writeKind=assets::OriginalStandardMaterialUiSharedHistoryWrite50LA::untextured;continue;}
            slot.writeKind=assets::OriginalStandardMaterialUiSharedHistoryWrite50LA::bound;
            if(!authenticateImage(i,bank,imageIndex[i],s.stagedHistory[i],slot.afterNativeBindings[0],d)){okay=false;break;}
            if(s.stagedHistory[i].kind_==2){using F=assets::OriginalUiGsField50KG;
                for(const auto f:{F::sourceAlphaValueWhenAis0,F::sourceAlphaValueWhenAis1,F::methodOfExpandingTextureAlpha})
                    slot.afterContext.fields[static_cast<std::size_t>(f)]={f==F::methodOfExpandingTextureAlpha?1:255,
                        assets::OriginalStandardMaterialValueOrigin50LA::runtime,nullptr,0,0x24983C};
            }
            if(!sameCanonicalHistory(i,s.stagedHistory[i],slot.afterContext,slot.afterNativeBindings[0]))okay=false;
        }
        if(!okay){table->cancelUiSharedHistory(this,s.menuReceipt);return fail(d,E::history,d.sourcePC?d.sourcePC:0x249BA0);}
        s.menuStaged=true;s.menuFrame=&frame;s.menuDraw=frame.native_;s.menuGeneration=frame.frameGeneration_;s.stagedBank=&bank;return true;
    }catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiSharedDialogMaterialHistory50KG::beginMenu(const OriginalUiSourceOwner50KM* owner,
    std::uint64_t revision,const OriginalUiSourceFrame50KM& frame,const VehicleWindowGl50DS& renderer)noexcept {
    if(!current(owner,revision))return false;auto& s=*storage_;
    if(!s.menuStaged||s.menuPending||s.menuFrame!=&frame||s.menuDraw!=frame.native_||frame.owner_!=owner||
       frame.garageCanonical_||s.menuGeneration!=frame.frameGeneration_||s.menuGeneration<=s.attemptedGeneration||
       (s.renderer&&s.renderer!=&renderer)||!s.stagedBank||!s.stagedBank->loaded()||
       !s.domain->materials382B38()->uiSharedHistoryCurrent(this,s.menuReceipt))return false;
    s.menuRendererNew=!s.renderer;s.renderer=&renderer;s.attemptedGeneration=s.menuGeneration;s.menuPending=true;return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::cancelMenuBegin(const OriginalUiSourceOwner50KM* owner,
    std::uint64_t revision,const OriginalUiSourceFrame50KM& frame,const VehicleWindowGl50DS& renderer)noexcept {
    if(!current(owner,revision))return false;auto& s=*storage_;
    if(!s.menuStaged||!s.menuPending||s.menuFrame!=&frame||s.menuDraw!=frame.native_||s.renderer!=&renderer||
       frame.frameGeneration_!=s.menuGeneration||!s.domain->materials382B38()->uiSharedHistoryCurrent(this,s.menuReceipt))return false;
    s.menuPending=false;if(s.menuRendererNew)s.renderer=nullptr;s.menuRendererNew=false;return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::finishMenu(const OriginalUiSourceOwner50KM* owner,
    std::uint64_t& revision,const OriginalUiSourceFrame50KM& frame,bool accepted,bool started)noexcept {
    static_assert(std::is_nothrow_swappable_v<History>);
    if(!storage_)return false;auto& s=*storage_;const auto table=s.domain->materials382B38();
    if(!current(owner,revision)||!s.menuStaged||!s.menuPending||s.menuFrame!=&frame||s.menuDraw!=frame.native_||
       frame.owner_!=owner||frame.garageCanonical_||frame.frameGeneration_!=s.menuGeneration||
       !table->uiSharedHistoryCurrent(this,s.menuReceipt)){
        poison(owner);if(s.menuStaged)table->poisonUiSharedHistory(this,s.menuReceipt);s.menuPending=false;return false;
    }
    s.menuPending=false;
    if(accepted){
        // table commit validates every staged slot before its FIRST store.
        // With this exact lease held, nothing can mutate/rebuild these passes
        // between that preflight and the following nonallocating dual commit.
        if(!table->commitUiSharedHistory(this,s.menuReceipt,s.menuSlots)){
            poison(owner);table->poisonUiSharedHistory(this,s.menuReceipt);return false;
        }
        s.history.swap(s.stagedHistory);revision=++s.revision;
    }else if(started){poison(owner);table->poisonUiSharedHistory(this,s.menuReceipt);}
    const bool released=table->releaseUiSharedHistory(this,s.menuReceipt);
    if(!released){s.poisoned=true;return false;}
    s.menuStaged=false;s.menuFrame=nullptr;s.menuDraw=nullptr;s.stagedBank=nullptr;s.menuGeneration=0;s.menuRendererNew=false;
    return !s.poisoned;
}
bool OriginalUiSharedDialogMaterialHistory50KG::menuStagePending(const OriginalUiSourceOwner50KM* owner)const noexcept {
    return storage_&&storage_->lease==owner&&(storage_->menuStaged||storage_->menuPending);
}
bool OriginalUiSharedDialogMaterialHistory50KG::releaseMenu(const OriginalUiMainDraw50KG* owner,
    const VehicleWindowGl50DS* renderer)noexcept {
    return owner&&renderer&&owner->historyRendererRetired(*renderer)&&releaseMenuAfterRetirement(owner,renderer);
}
bool OriginalUiSharedDialogMaterialHistory50KG::releaseMenu(const OriginalUiOptionsDrawContribution50KG* owner,
    const VehicleWindowGl50DS* renderer)noexcept {
    return owner&&renderer&&owner->historyRendererRetired(*renderer)&&releaseMenuAfterRetirement(owner,renderer);
}
bool OriginalUiSharedDialogMaterialHistory50KG::releaseMenuAfterRetirement(const OriginalUiSourceOwner50KM* owner,
    const VehicleWindowGl50DS* retiredRenderer)noexcept {
    if(!storage_||!owner||storage_->lease!=owner||storage_->garage||storage_->menuPending)return false;
    auto& s=*storage_;if(s.renderer&&s.renderer!=retiredRenderer)return false;
    if(s.menuStaged&&!s.domain->materials382B38()->cancelUiSharedHistory(this,s.menuReceipt))return false;
    s.menuStaged=false;s.menuFrame=nullptr;s.menuDraw=nullptr;s.stagedBank=nullptr;s.menuGeneration=0;
    s.renderer=nullptr;s.lease=nullptr;s.attemptedGeneration=0;return true;
}
bool OriginalUiSharedDialogMaterialHistory50KG::stageGarage(OriginalUiGarageCanonicalSubmission50KG& tx,
    OriginalUiGarageDrawContribution50KG& draw,D& d)noexcept {
    d={};bool touches=false;
    for(const auto& map:tx.maps_){const auto origin=map.origin.materialRow,rep=map.representative.materialRow;
        if(origin>=3228&&origin<=3230&&!(rep>=3228&&rep<=3230))return fail(d,E::canonicalBridgePending,0x29C150);
        touches|=rep>=3228&&rep<=3230;
    }
    if(!touches)return true;
    if(!draw.issuer_||!draw.issuer_->state||!draw.issuer_->state->sharedDialogs||tx.owner_!=&draw||
       tx.sharedHistory_||!tx.pinned_||tx.pending_||tx.attempted_||!tx.bank_)return fail(d,E::history,0x24B26C);
    const auto common=draw.issuer_->state->sharedDialogs->sharedMaterialHistory_;
    if(!common||!common->canonicalCurrent()||common->storage_->lease||
       common->storage_->domain->materials382B38()!=tx.table_)return fail(d,E::history,0x249BA0);
    auto& s=*common->storage_;
    if(s.menuStaged||s.garage||s.revision==std::numeric_limits<std::uint64_t>::max()||
       tx.maps_.size()!=tx.images_.size()||tx.maps_.size()!=tx.contributions_.size())return fail(d,E::history,0x249BA0);
    try {
        auto candidate=s.history;
        for(std::size_t i=0;i<tx.maps_.size();++i){const auto& map=tx.maps_[i];const auto row=map.representative.materialRow;
            if(row<3228||row>3230)continue;const auto role=static_cast<std::size_t>(row-3228);
            if(map.representative.material!=s.canonical[role]||map.representative.pass!=0||map.map||
               !tx.contributions_[i].screenReceiver)return fail(d,E::identity,0x249610);
            if(role==0){if(!map.nativeUntexturedRoute||tx.images_[i].bound())return fail(d,E::identity,0x249BA0);}
            else {assets::OriginalUiMaterialBindingDiagnostic50KG detail;
                const auto image=tx.contributions_[i].span.span.textureIndex;
                if(!candidate[role].recordDrainedBinding(*tx.bank_,image,detail))return fail(d,E::material,detail.sourcePC);
                assets::OriginalStandardMaterialNativeBinding50KG verified;
                if(!common->authenticateImage(role,*tx.bank_,image,candidate[role],verified,d)||
                   !verified.equivalent(tx.images_[i]))return fail(d,E::history,0x249610);
            }
            if(!common->sameCanonicalHistory(role,candidate[role],map.contextAfter249610,tx.images_[i]))
                return fail(d,E::history,0x249BA0);
        }
        // No source work has started. Allocate/copy all histories first; only
        // then retain this exact subordinate canonical transaction and lease.
        s.stagedHistory=std::move(candidate);s.lease=&draw;s.garage=&tx;
        tx.sharedRevision_=s.revision;tx.sharedHistory_=common;return true;
    }catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiSharedDialogMaterialHistory50KG::garageCurrent(const OriginalUiGarageCanonicalSubmission50KG& tx)const noexcept {
    return storage_&&storage_->garage==&tx&&storage_->lease==tx.owner_&&storage_->revision==tx.sharedRevision_&&
        tx.sharedHistory_.get()==this&&tx.table_==storage_->domain->materials382B38()&&canonicalCurrent();
}
void OriginalUiSharedDialogMaterialHistory50KG::commitGarage(OriginalUiGarageCanonicalSubmission50KG& tx)noexcept {
    static_assert(std::is_nothrow_swappable_v<History>);
    // Called immediately after the canonical store, ONLY after the joint
    // preflight passed. Do not revalidate old state after one half committed.
    auto& s=*storage_;s.history.swap(s.stagedHistory);++s.revision;s.garage=nullptr;s.lease=nullptr;
    tx.sharedHistory_.reset();tx.sharedRevision_=0;
}
void OriginalUiSharedDialogMaterialHistory50KG::cancelGarage(OriginalUiGarageCanonicalSubmission50KG& tx,bool failed)noexcept {
    if(storage_&&storage_->garage==&tx&&storage_->lease==tx.owner_){
        if(failed){storage_->poisoned=true;tx.table_->poisonGarageNativeBindings();}
        storage_->garage=nullptr;storage_->lease=nullptr;
    }
    tx.sharedHistory_.reset();tx.sharedRevision_=0;
}
bool OriginalUiSharedDialogMaterialHistory50KG::commit(const OriginalUiSourceOwner50KM*,std::uint64_t&,
    std::array<History,6>&,D& d)noexcept {
    d={};return fail(d,E::canonicalBridgePending,0x249BA0);
}
void OriginalUiSharedDialogMaterialHistory50KG::poison(const OriginalUiSourceOwner50KM* owner)noexcept {
    if(storage_&&owner&&storage_->lease==owner){
        storage_->poisoned=true;
        if(storage_->menuStaged)storage_->domain->materials382B38()->poisonUiSharedHistory(this,storage_->menuReceipt);
        else storage_->domain->materials382B38()->poisonGarageNativeBindings();
    }
}
}
