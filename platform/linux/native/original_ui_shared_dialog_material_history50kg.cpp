#include "original_ui_shared_dialog_material_history50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_options_draw_resources50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <exception>
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
    std::array<History,6> history;
    const OriginalUiSharedDialogs50KG* prospectiveOwner=nullptr;
    const OriginalUiSharedDialogs50KG* attachedOwner=nullptr;
    const OriginalUiControllerPopupResources50KG* source47C=nullptr;
    const OriginalUiSourceOwner50KM* lease=nullptr;
    std::uint64_t revision=0;
    Phase phase=Phase::prepared;
    bool poisoned=false,transferred=false;
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
bool OriginalUiSharedDialogMaterialHistory50KG::copyHistory(std::uint32_t token,const Key&,History&,D& d)const noexcept {
    d={};d.registryToken=token;return fail(d,E::canonicalBridgePending,0x249BA0);
}
bool OriginalUiSharedDialogMaterialHistory50KG::acquire(const OriginalUiSourceOwner50KM*,std::uint64_t&,D& d)noexcept {
    d={};return fail(d,E::canonicalBridgePending,0x249BA0);
}
bool OriginalUiSharedDialogMaterialHistory50KG::commit(const OriginalUiSourceOwner50KM*,std::uint64_t&,
    std::array<History,6>&,D& d)noexcept {
    d={};return fail(d,E::canonicalBridgePending,0x249BA0);
}
void OriginalUiSharedDialogMaterialHistory50KG::poison(const OriginalUiSourceOwner50KM* owner)noexcept {
    if(storage_&&owner&&storage_->lease==owner)storage_->poisoned=true;
}
}
