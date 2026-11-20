#include "original_race_f8_native_bars50la.h"
#include "original_race_f8_renderer50la.h"
#include "original_race_constructor50la.h"
#include "original_race_display_lifetime50la.h"
#include "original_race_world_join50la.h"
#include "original_camera_session50kx.h"
#include "../../src/game/original_race_f8_storage50la.h"
#include "../../src/assets/original_standard_material_source50la.h"
#include "../../platform/linux/native/native_camera_view50dx.h"
#include <bit>
#include <cmath>
#include <thread>
#include <cstring>
#include <exception>
#include <limits>
#include <cfenv>
#include <vector>
#include "../../src/game/original_ui_text50kg.h"

namespace original_main_entry50KG {
namespace {
using D=rr::game::OriginalRaceF8Diagnostic50LA;
using Table=rr::assets::OriginalStandardMaterialTable50LA;
using Receipt=rr::assets::OriginalStandardMaterialCollectorReceipt50LA;
using Domain=rr::assets::OriginalStandardMaterialDomain50LA;
using Material=rr::assets::OriginalStandardMaterialObject50LA;
using Pass=rr::assets::OriginalStandardMaterialPassView50LA;
using Parsed=rr::assets::OriginalUiMaterialPass50KG;
bool reject(D& d,std::uint32_t pc,const char* why)noexcept {d.sourcePC=pc;d.operation=why;return false;}
struct PassWords {
    const Material* material=nullptr;
    const rr::assets::OriginalUiMaterialTemplateState50KG* constructionTemplate=nullptr;
    std::int16_t program=-1,group=-1;
    std::array<std::int16_t,2> texture{};
    std::array<std::uint8_t,2> uv{};
    std::array<std::uint8_t,4> fill{};
    std::array<std::int32_t,rr::assets::originalUiGsFieldCount50KG> gs{};
    std::array<std::int32_t,6> sampler{};
    std::uint32_t depthBase=0,runtime=0;std::int32_t depthFormat=0;
    bool operator==(const PassWords&)const=default;
};
struct Proof {
    std::int32_t row=-1,origin=-1,leaf=-1,finalPass=-1;
    PassWords original,final;
    bool operator==(const Proof&)const=default;
};
bool words(const std::shared_ptr<Domain>& domain,const Material* material,const Pass& p,PassWords& out,std::uint8_t maps=0)noexcept {
    if(!material||!p.references04||*p.references04<=0||!p.program18||!p.group22||
       !p.mapCount20||*p.mapCount20!=maps||!p.flags21||*p.flags21!=2u||!p.texture1A||!p.uv1E||
       !p.fillRgba2C||!p.constructionTemplate||!p.state30||p.state30->contextCount()!=1||
       !p.parameterMask24||*p.parameterMask24||!p.parameterCount28||*p.parameterCount28||
       (p.parameters34&&!p.parameters34->empty())||p.animation10||p.textureAnimation10)return false;
    rr::assets::OriginalStandardProgramRowView50LA program;
    if(!domain||!domain->programs382B70()->row(*p.program18,program)||!program.canonicalName||
       std::strcmp(program.canonicalName,"ScreenObject"))return false;
    out.material=material;out.constructionTemplate=p.constructionTemplate;
    out.program=*p.program18;out.group=*p.group22;out.texture=*p.texture1A;out.uv=*p.uv1E;out.fill=*p.fillRgba2C;
    for(std::size_t i=0;i<out.gs.size();++i)out.gs[i]=p.state30->fields[i].value;
    for(std::size_t i=0;i<out.sampler.size();++i)out.sampler[i]=p.state30->sampler[i].value;
    out.depthBase=p.state30->videoDepthBase2E0;out.runtime=p.state30->runtime435670;
    out.depthFormat=p.state30->videoDepthFormat5C;return true;
}
bool readProof(const std::shared_ptr<Domain>& domain,const Receipt& receipt,std::int32_t row,
    Proof& out,rr::assets::OriginalStandardMaterialDiagnostic50LA& d,std::uint8_t maps=0)noexcept {
    const auto table=domain?domain->materials382B38():nullptr;const Material* material=nullptr;
    rr::assets::OriginalStandardMaterialCollectorEntryView50LA origin,final;Pass p,q;
    if(!table||!table->coldCollectorCurrent(receipt)||!table->resolve298E60(row,material,d)||
       !material||!material->passCount48()||*material->passCount48()!=1||
       !table->coldCollectorPass(receipt,row,0,origin)||origin.material!=material||origin.pass!=0||
       origin.finalLeaf<0||!table->coldCollectorEntry(receipt,std::size_t(origin.finalLeaf),final)||
       final.redirect18!=-1||final.registration!=origin.finalLeaf||!final.material||final.pass<0||
       !material->pass4C(0,p)||!final.material->pass4C(std::size_t(final.pass),q)||
       !words(domain,material,p,out.original,maps)||!words(domain,final.material,q,out.final,maps)||
       out.original.program!=out.final.program)return false;
    out.row=row;out.origin=origin.registration;out.leaf=origin.finalLeaf;out.finalPass=final.pass;return true;
}
bool authoredMatches(const Parsed& parsed,const PassWords& original)noexcept {
    if(parsed.microprogram!="ScreenObject"||parsed.enabled!=1||parsed.textureCount||
       !parsed.textureName.empty()||parsed.textureCoordinateIndex!=original.uv[0])return false;
    for(std::size_t i=0;i<original.gs.size();++i)if(parsed.gs.fields[i].value!=original.gs[i])return false;
    const std::int32_t sampler[]{parsed.lodCalculationMethod,parsed.maximumMipLevel,
        parsed.magFilter,parsed.minFilter,parsed.lodL,parsed.lodK};
    for(std::size_t i=0;i<original.sampler.size();++i)if(sampler[i]!=original.sampler[i])return false;
    return true;
}
bool frameMatrixCurrent(const original_camera_session50KX::Session& camera,
    const rr::platform::VehicleWindowFrame50DS& frame)noexcept {
    if(frame.width<=0||frame.height<=0||!frame.worldViewProjection||!camera.hasView())return false;
    const rr::platform::NativeCameraViewContract50DX contract{
        rr::platform::NativeCameraViewProvenance50DX::currentCore49FForOutput,frame.width,frame.height,
        rr::platform::NativeCameraClipDepth50DX::negativeOneToOne};
    rr::platform::NativeCameraMatrix50DX converted;rr::platform::NativeCameraViewError50DX error;
    return rr::platform::convertNativeCameraView50DX(camera.view(),contract,converted,error)&&
        !std::memcmp(frame.worldViewProjection,converted.worldViewProjection,sizeof converted.worldViewProjection);
}
}
// Source-owned native pixels only. This cache is privately published by the
// actual F8 batch and cannot grant material service, draw or queue authority.
class NativeF8ImageCache50LA final {
    friend class NativeF8BarPrefix50LA;
    NativeF8ImageCache50LA()=default;
    std::weak_ptr<rr::game::OriginalRaceConstructor50LA> race;
    std::weak_ptr<Domain> domain;
    std::weak_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context;
    const rr::game::OriginalRaceF850LA* source=nullptr;
    std::uint64_t generation=0;
    std::array<const rr::assets::OriginalStandardTextureResource50LA*,3> textures{};
    std::array<std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,3> archives;
    std::array<const rr::assets::OriginalUiMemberIdentity50KG*,3> members{};
    std::array<std::size_t,3> extents{};
    std::shared_ptr<const rr::assets::OriginalUiScreenAssets50KG> images;
public:
    ~NativeF8ImageCache50LA()noexcept=default;
    NativeF8ImageCache50LA(const NativeF8ImageCache50LA&)=delete;
    NativeF8ImageCache50LA& operator=(const NativeF8ImageCache50LA&)=delete;
};
struct NativeF8BarPrefix50LA::Impl {
    const Application* issuer=nullptr;
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> race;
    // The race's one World (identity retained for the frame) and the
    // synchronous borrow of the Application-issued Camera34 Session.
    std::shared_ptr<common_race_world50CR::World> world;
    std::shared_ptr<const original_camera_session50KX::Session> camera;
    std::shared_ptr<RaceDisplayLifetime50LA> display;
    std::shared_ptr<RaceF8RendererLease50LA> rendererLease;
    rr::game::OriginalApplicationAnimationClockView50KG applicationLifetime;
    rr::game::OriginalRaceF850LA* source=nullptr;
    std::shared_ptr<Domain> domain;std::shared_ptr<Table> table;
    Receipt receipt;std::uint64_t providerRevision=0,selectionRevision=0,cameraPublication=0,displayGeneration=0;
    rr::platform::VehicleWindowFrame50DS* frame=nullptr;
    const std::thread::id ownerThread=std::this_thread::get_id();
    std::int32_t width=0,height=0;
    std::array<std::int32_t,4> rectangle{};
    std::array<std::uint64_t,4> revisions{};
    std::array<rr::assets::Vertex48B,16> vertices{};
    std::array<Proof,4> proofs{};
    rr::assets::OriginalUiMaterial50KG authored;
    Suffix suffix;
    std::array<std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,3> suffixArchives;
    std::array<const rr::assets::OriginalStandardTextureResource50LA*,3> suffixTextures{};
    std::array<Proof,maximumSuffixPackets> suffixProofs{};
    std::array<rr::assets::OriginalStandardMaterialMapBindingView50LA,maximumSuffixPackets> suffixMaps{};
    std::shared_ptr<const rr::assets::OriginalApplicationLocalization50KG::SelectedText> selectedCross;
    bool quadEnabled=false,textEnabled=false;
    bool tablePinned=false,sourcePinned=false,prepared=false,enabled=false,suffixPending=false;
    std::uint8_t flags=0;std::uint32_t transitionBits=0;
};
#include "original_race_f8_native_suffix50la.inl"
NativeF8BarPrefix50LA::NativeF8BarPrefix50LA():impl_(new Impl){}
NativeF8BarPrefix50LA::~NativeF8BarPrefix50LA()noexcept {
    if(!impl_)return;auto& n=*impl_;
    if(n.ownerThread!=std::this_thread::get_id())std::terminate();
    if(n.frame&&n.frame->nativeF8BarPrefix==this)n.frame->nativeF8BarPrefix=nullptr;
    if(n.sourcePinned){
        if(!n.source||!n.source->impl_||n.source->impl_->nativeBarBorrows!=1)std::terminate();
        auto& p=*n.source->impl_;
        for(auto& polygon:p.polygons30){if(!polygon||polygon->nativePacketPins!=1)std::terminate();--polygon->nativePacketPins;}
        --p.nativeBarBorrows;n.sourcePinned=false;
    }
    if(n.tablePinned){n.table->unpinNativeCollector();n.tablePinned=false;}
    // No original FIFO/105990 or GPU completion is asserted. Synchronous
    // buffer upload owns copied bytes; the real F8 renderer lease still owns
    // its later per-resource20(1)/24 retirement. The exact Frame outlives this scope.
}
bool NativeF8BarPrefix50LA::initializePolicies(D& d)noexcept {
    auto& n=*impl_;auto& p=*n.source->impl_;
    if(!p.domain||!p.context||!p.context->live()||!p.context->materialApplication()||!p.sourceData||
       !p.sourceData->overlays()||!p.domain->archiveProvider3E1A70()||
       !p.domain->archiveProvider3E1A70()->sealed())return reject(d,0x246760,"F8 actual canonical material owners");
    const auto& runtime=p.context->materialRuntime();
    if(!runtime.overrideKnown||!runtime.callbackOverrideKnown||runtime.callbackOverride435678)
        return reject(d,0x24a674,"F8 known actual material callback/runtime route");
    n.domain=p.domain;n.table=p.domain->materials382B38();n.receipt=p.domain->coldCollector14D440();
    if(!n.table||!n.table->pinNativeCollector(n.receipt))return reject(d,0x29c150,"F8 actual canonical registry pin");
    n.tablePinned=true;n.providerRevision=p.domain->archiveProvider3E1A70()->revision();
    try {
        constexpr const char* path="textures\\SplitScreenBars.psm";
        const auto member=p.sourceData->overlays()->find(path);
        rr::assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
        using Lookup=rr::assets::OriginalStandardArchiveProvider50LA::Lookup;
        if(!member.identity||!member.data||p.domain->archiveProvider3E1A70()->lookup132820(path,mounted)!=Lookup::present||
           !mounted.data||mounted.size!=member.size||std::memcmp(mounted.data,member.data,member.size))
            return reject(d,0x132820,"F8 exact winning SplitScreenBars bytes");
        rr::assets::OriginalUiMaterialDiagnostic50KG md;
        if(!rr::assets::readOriginalUiMaterial50KG(*p.sourceData->overlays(),path,
            p.context->materialApplication()->templates(),rr::assets::OriginalUiTemplate50KG::screenMt,
            runtime,n.authored,md)||n.authored.passes.size()!=1)
            return reject(d,md.sourcePC?md.sourcePC:0x247110,"F8 actual zero-map authored policy");
        for(std::size_t i=0;i<4;++i){d.object=0x30+4*i;rr::assets::OriginalStandardMaterialDiagnostic50LA canonical;
            if(!p.polygonCurrent(i)||!readProof(n.domain,n.receipt,p.references[i].row,n.proofs[i],canonical))
                return reject(d,canonical.sourcePC?canonical.sourcePC:0x24b1a0,"F8 same origin/final canonical pass");
            const auto* source=n.proofs[i].original.material->source();const auto* bytes=source?source->originalBytes():nullptr;
            if(!bytes||bytes->size()!=member.size||std::memcmp(bytes->data(),member.data,member.size)||
               !authoredMatches(n.authored.passes[0],n.proofs[i].original))
                return reject(d,0x2476a0,"F8 original pass/PSM/template identity");
            auto effective=n.authored.passes[0];const auto& final=n.proofs[i].final;
            for(std::size_t j=0;j<final.gs.size();++j)effective.gs.fields[j].value=final.gs[j];
            effective.textureCoordinateIndex=final.uv[0];
            effective.lodCalculationMethod=final.sampler[0];effective.maximumMipLevel=final.sampler[1];
            effective.magFilter=final.sampler[2];effective.minFilter=final.sampler[3];
            effective.lodL=final.sampler[4];effective.lodK=final.sampler[5];
            rr::assets::OriginalUiNativeUntexturedMaterial50KG policy;
            if(!rr::assets::prepareOriginalUiNativeUntexturedMaterial50KG(effective,policy,md)||
               policy.blend!=rr::assets::OriginalUiMaterialBlend50KG::opaque||policy.depthMethod!=1||
               !policy.depthWrite||policy.environmentContext||policy.requiresSeparateAlphaStore||
               !policy.requiresOriginalDepthOwner||policy.colorScale!=1.f||policy.storedAlphaScale!=1.f||
               policy.maximumVertexAlpha<63)
                return reject(d,md.sourcePC?md.sourcePC:0x249ba0,"F8 bounded opaque/ALWAYS/depth-write policy");
        }
        return true;
    }catch(...){return reject(d,0x247110,"F8 native material projection allocation");}
}
bool NativeF8BarPrefix50LA::servicePolicy(std::size_t ordinal,D& d)noexcept {
    auto& n=*impl_;if(ordinal>=4||!policiesCurrent())return reject(d,0x24a728,"F8 native material policy changed");
    rr::assets::OriginalStandardMaterialDiagnostic50LA md;
    // Exact originating null-key ScreenObject service, once per actual bar.
    // This does not append24B9B0, execute249BA0, or certify source mode5.
    if(!n.table->serviceRetainedScreenPrototype24A740(n.receipt,n.proofs[ordinal].row,0,md)){
        d.sourceEffectsCommitted|=md.sourceEffectsCommitted;
        return reject(d,md.sourcePC?md.sourcePC:0x24a740,"F8 originating ScreenObject service incomplete");
    }
    d.sourceEffectsCommitted|=md.sourceEffectsCommitted;
    return policiesCurrent()||reject(d,0x24a758,"F8 actual policy changed after originating service");
}
bool NativeF8BarPrefix50LA::policiesCurrent()const noexcept {
    if(!impl_||!impl_->tablePinned)return false;const auto& n=*impl_;
    if(!n.source||!n.source->impl_||n.source->impl_->domain!=n.domain||!n.domain||
       !n.source->impl_->context||!n.source->impl_->context->live()||
       !n.domain->archiveProvider3E1A70()||!n.domain->archiveProvider3E1A70()->sealed()||
       n.domain->archiveProvider3E1A70()->revision()!=n.providerRevision)return false;
    const auto& runtime=n.source->impl_->context->materialRuntime();
    if(!runtime.overrideKnown||!runtime.callbackOverrideKnown||runtime.callbackOverride435678)return false;
    for(std::size_t i=0;i<4;++i){Proof fresh;rr::assets::OriginalStandardMaterialDiagnostic50LA d;
        if(!readProof(n.domain,n.receipt,n.proofs[i].row,fresh,d)||!(fresh==n.proofs[i]))return false;}
    for(std::size_t i=0;i<n.suffix.packetCount;++i){
        Proof fresh;rr::assets::OriginalStandardMaterialDiagnostic50LA d;
        if(!readProof(n.domain,n.receipt,n.suffixProofs[i].row,fresh,d,1)||!(fresh==n.suffixProofs[i]))return false;
        rr::assets::OriginalUiMaterial50KG parsed;rr::assets::OriginalUiNativeMaterial50KG policy;
        const auto& map=n.suffixMaps[i];
        if(!n.suffix.images||!map.representative.material||
           !map.representative.material->prepareF8ScreenFallback2475C0(*n.table,n.receipt,map,
               *n.suffix.images,n.suffix.packets[i].image,parsed,policy,d))return false;
    }
    return true;
}
const NativeF8BarPrefix50LA::Suffix& NativeF8BarPrefix50LA::suffix()const noexcept {
    if(!impl_||impl_->ownerThread!=std::this_thread::get_id())std::terminate();
    return impl_->suffix;
}
bool NativeF8BarPrefix50LA::current(const rr::platform::VehicleWindowGl50DS& renderer,
    const rr::platform::VehicleWindowFrame50DS& frame)const noexcept {
    // The immutable creator-thread fence precedes mutable batch/clock reads.
    if(!impl_||impl_->ownerThread!=std::this_thread::get_id())return false;
    if(!impl_->prepared||!impl_->issuer)return false;
    std::uint32_t pause=0;if(!impl_->applicationLifetime.readPause11C(pause))return false;
    return impl_->issuer->nativeF8BarsCurrent50LA(*this,renderer,frame);
}
const std::array<rr::assets::Vertex48B,16>& NativeF8BarPrefix50LA::vertices()const noexcept{
    if(!impl_||impl_->ownerThread!=std::this_thread::get_id())std::terminate();
    return impl_->vertices;
}
bool NativeF8BarPrefix50LA::enabled()const noexcept{
    return impl_&&impl_->ownerThread==std::this_thread::get_id()&&impl_->prepared&&impl_->enabled;
}
bool NativeF8BarPrefix50LA::suffixPending()const noexcept{
    return impl_&&impl_->ownerThread==std::this_thread::get_id()&&impl_->prepared&&impl_->suffixPending;
}

bool Application::nativeF8BarsCurrent50LA(const NativeF8BarPrefix50LA& batch,
    const rr::platform::VehicleWindowGl50DS& renderer,const rr::platform::VehicleWindowFrame50DS& frame)const noexcept {
    // Reject before any mutable Application owner, Race or weak-Display read.
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    if(!batch.impl_)return false;const auto& n=*batch.impl_;
    if(n.ownerThread!=std::this_thread::get_id())return false;
    if(!n.prepared||n.issuer!=this||!n.sourcePinned||!n.source||!n.race||sourceRace28COwner_!=n.race||
       n.race->application_!=entitiesOwner_||!n.race->counted_||n.race->rolledBack_||
       n.race->world74_!=n.world||n.race->loadFifo_!=nativeCollectorFifoOwner_||
       !n.camera||!n.display||n.race->loadDisplay_!=n.display||
       n.race->f8Owner_.get()!=n.source||n.race->f8Source_!=n.source||!n.race->f8Attached_||
       raceDisplayOwner50LA_.lock()!=n.display||raceDisplayGeneration50LA_!=n.displayGeneration||
       n.display->generation_!=n.displayGeneration||n.display->application_!=this||
       &n.display->renderer_!=&renderer||n.display->retiring_||!n.display->canRetain()||
       !n.rendererLease||!n.rendererLease->identityCurrent()||n.frame!=&frame||
       frame.nativeF8BarPrefix!=&batch||frame.width!=n.width||frame.height!=n.height||
       n.camera->generation_!=n.race->entity_.sessionGeneration||n.camera->actual_.humanCount34!=1||
       n.camera->actual_.applicationEntities!=entitiesOwner_||!n.camera->nativeTaIssuance_||
       n.camera->nativeTaIssuance_->issuer!=this||n.camera->nativeTaIssuance_->generation!=n.camera->generation_||
       !n.race->raceCamera34Issued(*n.camera)||
       n.camera->publicationCount()!=n.cameraPublication||!frameMatrixCurrent(*n.camera,frame))return false;
    auto& p=*n.source->impl_;
    const auto font=p.fonts?p.fonts->find282210(rr::assets::OriginalApplicationFonts50LA::Name::denmark16):nullptr;
    if(!p.fonts||!p.domain||p.fonts!=fonts328Owner_||p.domain!=standardMaterialDomainOwner_||
       p.fonts->domain_!=p.domain||p.fonts->context_!=p.context||!font||!p.texts100||
       (*p.texts100)[0].font4C!=font||(*p.texts100)[1].font4C!=font||
       !rr::assets::OriginalApplicationFonts50LA::currentResource281F90(*p.domain,*font))return false;
    if(p.depth!=0xffffffu||!n.display->video_||n.display->video_->packetZMaximum!=p.depth||
       !p.currentLeaf()||p.retirementStarted||p.nativeBarBorrows!=1||p.rendererLease!=n.rendererLease||
       p.component.parent1C!=&n.race->entity_||p.component.mask20!=0xffffffffu||
       (p.component.flags29&9u)!=1u||p.fields->flags1E4Low5!=n.flags||
       std::bit_cast<std::uint32_t>(p.fields->transition1E0)!=n.transitionBits||
       (entitiesOwner_->apertureRenderEnabled383680_!=0)!=n.enabled||
       (entitiesOwner_->screenQuadEnabled383678_!=0)!=n.quadEnabled||
       (entitiesOwner_->screenTextEnabled383688_!=0)!=n.textEnabled||!batch.policiesCurrent())return false;
    if(n.selectedCross&&!n.selectedCross->currentOwnedBy(n.selectedCross,*this))return false;
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    if(!n.display->readGameplayViewport115990(*n.camera,rectangle,revision)||
       revision!=n.selectionRevision||rectangle!=n.rectangle)return false;
    for(std::size_t i=0;i<4;++i)if(!p.polygonCurrent(i)||p.polygons30[i]->nativePacketPins!=1||
       p.polygons30[i]->packetRevision!=n.revisions[i])return false;
    return true;
}
bool Application::prepareNativeF8Bars50LA(const original_camera_session50KX::Session& expectedSession,
    rr::platform::VehicleWindowFrame50DS& frame,
    std::unique_ptr<NativeF8BarPrefix50LA>& output,D& d)const noexcept {
    d={};
    // Use the existing immutable Application fence before copying shared owners
    // or locking weak Display state; a later Display/currentLeaf check is too late.
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return reject(d,0x199554,"F8 native prefix requires the Application owner thread");
    if(std::fegetround()!=FE_TONEAREST)return reject(d,0x199554,"F8 native binary32 geometry rounding");
    const auto race=sourceRace28COwner_;const auto display=raceDisplayOwner50LA_.lock();
    if(output||frame.nativeF8BarPrefix||!race||race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_||
       !race->anyWorld()||!race->f8Owner_||race->f8Source_!=race->f8Owner_.get()||!race->f8Attached_||
       !display||race->loadDisplay_!=display||display->application_!=this||
       display->generation_!=raceDisplayGeneration50LA_||!display->canRetain())
        return reject(d,0x199554,"F8 native prefix requires the actual consumed Race/Display Frame owners");
    auto& source=*race->f8Owner_;auto& p=*source.impl_;
    const auto font=p.fonts?p.fonts->find282210(rr::assets::OriginalApplicationFonts50LA::Name::denmark16):nullptr;
    if(!p.fonts||!p.domain||p.fonts!=fonts328Owner_||p.domain!=standardMaterialDomainOwner_||
       p.fonts->domain_!=p.domain||p.fonts->context_!=p.context||!font||!p.texts100||
       (*p.texts100)[0].font4C!=font||(*p.texts100)[1].font4C!=font||
       !rr::assets::OriginalApplicationFonts50LA::currentResource281F90(*p.domain,*font))
        return reject(d,0x281f90,"F8 actual Application328 Denmark16 registry/resource owner");
    if(p.depth!=0xffffffu||!display->video_||display->video_->packetZMaximum!=p.depth||
       !source.loadedCold()||!p.currentLeaf()||p.nativeBarBorrows||p.retirementStarted||
       p.component.parent1C!=&race->entity_||p.component.mask20!=0xffffffffu||(p.component.flags29&9u)!=1u||
       !source.belongsTo(entitiesOwner_,race->entity_.sessionGeneration)||!std::isfinite(p.fields->transition1E0))
        return reject(d,0x199554,"F8 actual state/empty attached leaf required; no Session flag substitution");
    // One Camera for every race: the issued Session bound as Camera34.
    const auto world=race->world74_;
    const std::shared_ptr<const original_camera_session50KX::Session> borrowed=race->borrowRaceCamera34();
    const auto* camera=borrowed.get();
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    if(!camera||camera!=&expectedSession||
       camera->frameStage_!=original_camera_session50KX::Session::FrameStage::published||
       camera->generation_!=race->entity_.sessionGeneration||camera->actual_.humanCount34!=1||
       camera->actual_.applicationEntities!=entitiesOwner_||!camera->nativeTaIssuance_||
       camera->nativeTaIssuance_->issuer!=this||camera->nativeTaIssuance_->generation!=camera->generation_||
       !race->raceCamera34Issued(*camera)||
       !display->readGameplayViewport115990(*camera,rectangle,revision)||rectangle!=std::array<std::int32_t,4>{{0,0,640,448}}||
       !frameMatrixCurrent(*camera,frame))return reject(d,0x115990,"F8 same counted current one-human course Camera/Frame projection");
    if(!p.rendererLease&&!retainConsumedF8Renderer50LA(d))return false;
    if(!p.rendererLease||!p.rendererLease->identityCurrent())return reject(d,0x141dd8,"F8 actual native renderer resource custody");
    try {
        auto batch=std::unique_ptr<NativeF8BarPrefix50LA>(new NativeF8BarPrefix50LA);auto& n=*batch->impl_;
        n.issuer=this;n.race=race;n.world=world;n.camera=borrowed;n.display=display;n.source=&source;
        n.rendererLease=p.rendererLease;n.applicationLifetime=animationClock();
        n.selectionRevision=revision;n.rectangle=rectangle;n.cameraPublication=camera->publicationCount();
        n.displayGeneration=display->generation_;n.width=frame.width;n.height=frame.height;n.frame=&frame;
        n.flags=p.fields->flags1E4Low5;n.transitionBits=std::bit_cast<std::uint32_t>(p.fields->transition1E0);
        n.enabled=entitiesOwner_->apertureRenderEnabled383680_!=0;
        n.quadEnabled=entitiesOwner_->screenQuadEnabled383678_!=0;
        n.textEnabled=entitiesOwner_->screenTextEnabled383688_!=0;
        n.suffixPending=(n.flags&1u)&&p.fields->transition1E0<=0.f&&(n.flags&2u)&&(n.flags&12u);
        if(!batch->initializePolicies(d))return false;
        // Native preparation explicitly joins layout and packet CPU leaves.
        // No source mode3/4/5, camera268 value, root traversal or199500 gate is
        // manufactured. The actual Session caller deliberately selects this native adapter.
        if(!source.layout198CF0(rectangle,d))return false;
        rr::platform::VehicleWindowDiagnostic50DS renderer;
        if(!display->renderer_.prepareNativeF8BarResources50LA(renderer))
            return reject(d,0x199554,"F8 real native scratch/UI resource preparation failed");
        std::int32_t actualV0=0;
        const auto service=rr::game::serviceOriginalRenderer244D60(display->dispatch_,false,
            display->renderer_,renderer,actualV0);
        if(service.status!=rr::game::OriginalCameraStartStatus::complete)
            return reject(d,service.sourcePC,"F8 actual renderer20(0) native prefix incomplete");
        for(std::size_t i=0;i<4;++i){
            if(n.enabled){
                D leaf;if(!source.preparePolygon2E2DD0(i,camera->viewport_,display->video_->packetZMaximum,leaf)){
                    const bool prior=d.sourceEffectsCommitted;d=leaf;d.sourceEffectsCommitted|=prior;return false;}
                d.sourceEffectsCommitted|=leaf.sourceEffectsCommitted;
                if(!batch->servicePolicy(i,d))return false;
                const auto& packet=*p.polygons30[i]->packets44[0].allocation00;
                for(std::size_t j=0;j<4;++j){const auto& in=packet.vertices[j];auto& out=n.vertices[4*i+j];
                    if(in.stq[2]!=16777205.f||in.stq[3]!=1.f||in.rgba!=rr::game::OriginalRaceF8PolygonStorage50LA::Qword{{0,0,0,63}}||
                       !std::isfinite(in.stq[0])||!std::isfinite(in.stq[1])||
                       in.xyz!=std::array<float,4>{{0,0,1,0}})
                        return reject(d,0x2e389c,"F8 native actual packet position/color subset");
                    for(std::size_t k=0;k<3;++k)out.position[k]=in.stq[k];
                    for(std::size_t k=0;k<4;++k)out.color[k]=std::uint8_t(in.rgba[k]);
                    out.uv[0]=in.xyz[0];out.uv[1]=in.xyz[1];
                }
            }
            if(!p.polygonCurrent(i)||p.polygons30[i]->nativePacketPins)
                return reject(d,0x2e23dc,"F8 actual packet changed during native preparation");
            n.revisions[i]=p.polygons30[i]->packetRevision;
        }
        //199574..19973C: genuine flags/timer decide the source suffix. The
        //three global gates stay independent. Text selection/setter still
        //runs when ScreenTexture and/or ScreenText rendering is disabled.
        if(n.suffixPending){
            if((n.quadEnabled||n.textEnabled)&&!batch->initializeSuffixImages(d))return false;
            if(n.flags&4u){
                if(n.quadEnabled&&!batch->prepareIcon(0,d))return false; //1995B8
                std::string_view selected="DONE"; //exact original3A9088 bytes
                if(n.flags&16u){
                    rr::assets::OriginalApplicationLocalizationDiagnostic50KG ld;
                    if(!sourceLocalization32COwner_||!borrowLocalizationText18AC60(*sourceLocalization32COwner_,8,n.selectedCross,ld)||
                       !n.selectedCross->readCurrent(n.selectedCross,*this,selected))
                        return reject(d,0x1995e0,"F8 actual Application32C selected text8 unavailable");
                }
                if(!batch->setCrossText(selected,d))return false;
                if(n.textEnabled&&!batch->prepareText(0,d))return false; //199714
            }
            if(n.flags&8u){
                if(n.quadEnabled&&!batch->prepareIcon(1,d))return false; //199730
                if(n.textEnabled&&!batch->prepareText(1,d))return false; //199738
            }
            n.suffixPending=false;
        }
        // Native uploads retain copied immutable pixels, not canonical source
        // ownership. Real GL completion/name cleanup is renderer-owned.
        if(!display->renderer_.prepareNativeF8SuffixResources50LA(*batch,renderer))
            return reject(d,0x199744,"F8 native suffix image resource preparation incomplete");
        // No callbacks/interleaving remain after allocation. Preserve actual
        // owner and selected-camera observations before publishing CPU pins.
        std::array<std::int32_t,4> after{};std::uint64_t afterRevision=0;
        if(sourceRace28COwner_!=race||race->world74_!=world||race->loadDisplay_!=display||
           raceDisplayOwner50LA_.lock()!=display||raceDisplayGeneration50LA_!=n.displayGeneration||
           !p.rendererLease->identityCurrent()||!p.currentLeaf()||p.nativeBarBorrows||
           p.fields->flags1E4Low5!=n.flags||std::bit_cast<std::uint32_t>(p.fields->transition1E0)!=n.transitionBits||
           (entitiesOwner_->apertureRenderEnabled383680_!=0)!=n.enabled||
           (entitiesOwner_->screenQuadEnabled383678_!=0)!=n.quadEnabled||
           (entitiesOwner_->screenTextEnabled383688_!=0)!=n.textEnabled||
           (n.selectedCross&&!n.selectedCross->currentOwnedBy(n.selectedCross,*this))||
           !display->readGameplayViewport115990(*camera,after,afterRevision)||after!=rectangle||afterRevision!=revision||
           camera->publicationCount()!=n.cameraPublication||!frameMatrixCurrent(*camera,frame)||!batch->policiesCurrent())
            return reject(d,0x199574,"F8 owners changed after native CPU effects; no Frame published");
        for(auto& polygon:p.polygons30)++polygon->nativePacketPins;
        ++p.nativeBarBorrows;n.sourcePinned=true;n.prepared=true;
        frame.nativeF8BarPrefix=batch.get();output=std::move(batch);
        d.sourcePC=0x199744;d.operation="native F8 bars/icon/text prepared; original mode/index/FIFO/base traversal not claimed";
        return true;
    }catch(...){return reject(d,0x199554,"F8 scoped native bar-prefix allocation failed");}
}
}

namespace original_camera_session50KX {
bool Session::prepareNativeF8Frame50LA(rr::platform::VehicleWindowFrame50DS& frame,
    std::unique_ptr<original_main_entry50KG::NativeF8BarPrefix50LA>& output,
    rr::game::OriginalRaceF8Diagnostic50LA& d)const noexcept {
    d={};
    const auto reject=[&](const char* why)noexcept {d.sourcePC=0x199500;d.operation=why;return false;};
    // Reuse this Session's existing immutable construction-thread identity.
    // No mutable Session, Frame or lifetime view is read before this fence.
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return reject("F8 native Frame requires Session owner thread");
    if(output||frame.nativeF8BarPrefix||!coldBound_||closed_||abandoned_||
       frameStage_!=FrameStage::published||!hasView()||actual_.humanCount34!=1||
       !nativeTaIssuance_||!nativeTaIssuance_->issuer||nativeTaIssuance_->generation!=generation_)
        return reject("F8 native Frame requires actual issued published human Session");
    std::uint32_t ignoredPause=0;
    if(!actual_.applicationAnimation.readPause11C(ignoredPause))
        return reject("F8 native Frame Application lifetime expired");
    // Application's own immutable fence runs before mutable owner access. It
    // then compares this exact Session to the counted current-world borrow.
    return nativeTaIssuance_->issuer->prepareNativeF8Bars50LA(*this,frame,output,d);
}
}
