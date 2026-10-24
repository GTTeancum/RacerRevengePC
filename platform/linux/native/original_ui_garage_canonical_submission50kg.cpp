#include "original_ui_garage_canonical_submission50kg.h"
#include "original_ui_garage_model_resource50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_prelit_adapter50kg.h"
#include "original_ui_garage_lit_adapter50kg.h"
#include "../../../src/assets/original_standard_ipum_image_lease50la.h"
#include <cstring>
#include <cstdio>
#include <utility>
namespace rr::platform {
namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool reject(D& d,S s,std::uint32_t pc)noexcept {d.status=s;d.sourcePC=pc;return false;}
bool sameSpan(const OriginalUiMaterialSpan50KG& expected,const OriginalUiSpan50KG& s,
    const OriginalUiNativeSpan50KG& n)noexcept {
    const auto& a=expected.span;const auto& b=expected.native;
    return a.firstQuad==s.firstQuad&&a.quadCount==s.quadCount&&a.textureIndex==s.textureIndex&&
        a.blend==s.blend&&a.alphaScale==s.alphaScale&&a.colorScale==s.colorScale&&a.materialMode==s.materialMode&&
        b.coverage==n.coverage&&b.depth==n.depth&&b.depthWrite==n.depthWrite&&
        b.sampler.minification==n.sampler.minification&&b.sampler.magnification==n.sampler.magnification&&
        b.sampler.s==n.sampler.s&&b.sampler.t==n.sampler.t&&b.sourceFogEnabled==n.sourceFogEnabled&&
        b.sourceAlphaTestGreaterZero==n.sourceAlphaTestGreaterZero&&b.sourcePacketStq==n.sourcePacketStq;
}
}
OriginalUiGarageCanonicalSubmission50KG::~OriginalUiGarageCanonicalSubmission50KG(){
    if(pending_&&table_)table_->poisonGarageNativeBindings();
    unpin();
}
void OriginalUiGarageCanonicalSubmission50KG::unpin()noexcept {
    if(pinned_){table_->unpinNativeCollector();pinned_=false;}
}
bool OriginalUiGarageCanonicalSubmission50KG::initialize(std::shared_ptr<Table> table,
    const assets::OriginalStandardMaterialCollectorReceipt50LA& receipt,
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank,const OriginalUiSourceOwner50KM& owner,
    std::uint64_t generation,D& d)noexcept {
    d={};if(table_||!table||!bank||!bank->loaded()||!generation||!table->coldCollectorCurrent(receipt))
        return reject(d,S::phase,0x29C490);
    if(!table->pinNativeCollector(receipt))return reject(d,S::phase,0x29C490);
    table_=std::move(table);bank_=std::move(bank);receipt_=receipt;owner_=&owner;generation_=generation;pinned_=true;return true;
}
bool OriginalUiGarageCanonicalSubmission50KG::authenticateImage(const Map& next,std::uint32_t index,
    assets::OriginalStandardMaterialNativeBinding50KG& history,D& d)const noexcept {
    using assets::OriginalStandardTextureLevelView50LA;
    using assets::OriginalUiIpumColorPolicy50KG;
    using assets::OriginalStandardMaterialNativeBinding50KG;
    const auto images=bank_->images();const auto member=bank_->sourceMember(index);
    const auto* provenance=images?images->sourceProvenance(index):nullptr;
    const auto* decoded=images?images->decodedTexture(index):nullptr;
    const auto* published=images&&images->textures()?images->textures()->texture(index):nullptr;
    const auto* original=next.texture->originalBytes();
    const auto* kind=next.texture->sourceKind();
    std::size_t offset=0,bytes=0;
    if(!bank_->loaded()||!member.identity||!member.data||!provenance||!decoded||!published||
       provenance->celIdentity||!original||original->size()!=member.size||
       std::memcmp(original->data(),member.data,member.size)!=0||!kind||*kind!=decoded->kind||
       !next.texture->pstExtent24E880(offset,bytes)||offset!=provenance->pstOffset||
       bytes!=provenance->pstBytes||decoded->revision!=1||decoded->flags||decoded->kind>2||
       decoded->mips.empty()||decoded->mips.size()>7)
        return reject(d,S::texture,0x24B254);
    const auto& base=decoded->mips.back();OriginalStandardTextureLevelView50LA level;
    // ScreenAssets decodes the exact selected PST slice, so its mip offsets
    // are PST-relative. Canonical TextureResource retains full-member offsets,
    // including an ATM child's real nonzero PST origin. Prove both ranges
    // before adding that origin; never substitute another frame or image.
    const std::size_t headerBytes=decoded->kind==2?20u:16u;
    if(offset>member.size||bytes>member.size-offset||base.source_header_offset>bytes||
       headerBytes>bytes-base.source_header_offset||
       base.source_indices_offset!=base.source_header_offset+headerBytes||base.source_indices_offset>=bytes){
        d.field="actual selected PST-relative mip extent";
        return reject(d,S::texture,0x249780);
    }
    const auto memberHeader=offset+base.source_header_offset;
    const auto memberIndices=offset+base.source_indices_offset;
    if(!next.texture->level100(0,level)||level.sourceMipOrdinal+1!=decoded->mips.size()||
       level.width104!=static_cast<std::int32_t>(base.width)||level.height110!=static_cast<std::int32_t>(base.height)||
       level.sourceHeaderOffset!=memberHeader||level.sourceIndicesOffset!=memberIndices||
       base.width<16||base.height<16||base.width>1024||base.height>1024||
       (base.width&(base.width-1))||(base.height&(base.height-1))||
       std::uint64_t(base.width)*base.height*4!=base.rgba.size()||
       published->texture.width!=base.width||published->texture.height!=base.height||published->texture.rgba!=base.rgba)
        return reject(d,S::texture,0x249780);
    const auto* ipum=images->ipumProvenance(index);
    if(decoded->kind==2){
        if(!ipum||ipum->colorPolicyKind!=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1||
           ipum->sourcePstSha256!=member.identity->sha256||ipum->mipCount!=decoded->mips.size()||
           (!level.garageIpumProvenance&&(!level.hostImage||!level.hostImage->rgba()||
            level.hostImage->rgbaBytes()!=base.rgba.size()||
            std::memcmp(level.hostImage->rgba(),base.rgba.data(),base.rgba.size())!=0)))
            return reject(d,S::texture,0x249818);
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
                return reject(d,S::texture,0x249818);
        }
    }else if(ipum)return reject(d,S::texture,0x249818);
    OriginalStandardMaterialNativeBinding50KG bound;
    bound.member_=member.identity;bound.pstOffset_=offset;bound.pstBytes_=bytes;
    // History retains its existing (member, PST origin, PST-relative offsets)
    // tuple. equivalent() compares that tuple; it does not index member bytes.
    bound.storedMip_=decoded->mips.size()-1;bound.header_=base.source_header_offset;
    bound.payload_=base.source_indices_offset;bound.width_=base.width;bound.height_=base.height;bound.kind_=decoded->kind;
    history=bound;return true;
}
bool OriginalUiGarageCanonicalSubmission50KG::stageModelPass(std::shared_ptr<const Model> origin,
    std::uint32_t slot,std::uint32_t pass,std::shared_ptr<const Model> representative,std::uint32_t repSlot,
    std::shared_ptr<const assets::OriginalUiGarageModelMaterials50KG> policy,std::uint32_t imageOwner,
    const OriginalUiMaterialSpanBinding50KG& binding,OriginalUiMaterialSpan50KG& output,Map& outputMap,D& d)noexcept {
    d={};if(!pinned_||frame_||attempted_||terminal_||!origin||!representative||!policy||
       !origin->current()||!representative->current()||!policy->loaded()||
       origin->materials().table382B38()!=table_.get()||representative->materials().table382B38()!=table_.get()||
       !representative->source()||representative->source()->source.identity!=policy->modelIdentity()||
       contributions_.size()>=originalUiMaximumSpans50KG)return reject(d,S::phase,0x246810);
    const auto* row=origin->materials().handle1C4(slot);const auto* repRow=representative->materials().handle1C4(repSlot);
    if(!row||!repRow)return reject(d,S::reference,0x246810);
    try {
        Map map;assets::OriginalStandardMaterialNativeBinding50KG history;
        assets::OriginalUiGarageProgram50KG family;
        if(!table_->prepareGarageNativeBinding249610(receipt_,*row,pass,map,family,d)||
           !authenticateImage(map,binding.textureIndex,history,d))return false;
        if(map.representative.materialRow!=*repRow||map.representative.pass<0||
           (!maps_.empty()&&map.representative.registration<maps_.back().representative.registration))
            return reject(d,S::collector,0x29C490);
        const auto repPass=static_cast<std::uint32_t>(map.representative.pass);
        const auto* authored=policy->pass(repSlot,repPass);
        const auto archive=policy->archive();const auto* source=map.representative.material->source();
        const auto* original=source?source->originalBytes():nullptr;
        const auto member=archive&&authored&&authored->source.identity?archive->find(authored->source.identity->name):assets::OriginalUiMemberView50KG{};
        assets::OriginalStandardMaterialPassView50LA view;
        if(!authored||!member.identity||member.identity!=authored->source.identity||!original||
           original->size()!=member.size||!member.data||std::memcmp(original->data(),member.data,member.size)!=0||
           !map.representative.material->pass4C(repPass,view)||!view.constructionTemplate||
           view.constructionTemplate->identity!=authored->source.inheritedTemplate||!view.uv1E||
           (*view.uv1E)[0]!=authored->source.textureCoordinateIndex||!view.flags21||
           bool(*view.flags21&2u)!=bool(authored->source.enabled)||
           !map.representative.material->passCount48()||
           static_cast<std::uint32_t>(*map.representative.material->passCount48())!=policy->passCount(repSlot))
            return reject(d,S::reference,0x249BA0);
        // No fabricated material pass. Scalar lowering uses the authenticated
        // representative recipe only after checking EVERY actual effective GS
        // and sampler field. The sole reached mutation is249818..24983C TEXA.
        const auto decoded=bank_->images()->decodedTexture(binding.textureIndex);
        if(!decoded)return reject(d,S::texture,0x249818);
        using F=assets::OriginalUiGsField50KG;
        for(std::size_t i=0;i<map.contextAfter249610.fields.size();++i){
            auto expected=authored->source.gs.fields[i].value;
            if(decoded->kind==2){
                if(i==static_cast<std::size_t>(F::sourceAlphaValueWhenAis0)||i==static_cast<std::size_t>(F::sourceAlphaValueWhenAis1))expected=255;
                if(i==static_cast<std::size_t>(F::methodOfExpandingTextureAlpha))expected=1;
            }
            if(map.contextAfter249610.fields[i].value!=expected)return reject(d,S::unsupportedPass,0x249BA0);
        }
        const std::int32_t sampler[]{authored->source.lodCalculationMethod,authored->source.maximumMipLevel,
            authored->source.magFilter,authored->source.minFilter,authored->source.lodL,authored->source.lodK};
        for(std::size_t i=0;i<6;++i)if(map.contextAfter249610.sampler[i].value!=sampler[i])
            return reject(d,S::unsupportedPass,0x249BA0);
        OriginalUiMaterialSpan50KG span;assets::OriginalUiMaterialDiagnostic50KG md;
        const bool prelit=family==assets::OriginalUiGarageProgram50KG::prelitCompressed;
        const bool empty=binding.firstQuad==0&&binding.quadCount==0&&binding.drawQuadCount==0;
        //Scalar policy conversion is independent of raster geometry. An actual
        //fully clipped record still binds249610; no fake quad is emitted.
        auto scalar=binding;if(empty)scalar.quadCount=scalar.drawQuadCount=1;
        const bool converted=prelit?
            convertOriginalUiGaragePrelitMaterialSpan50KG(*policy,repSlot,repPass,*bank_,imageOwner,scalar,span,md):
            convertOriginalUiGarageLitMaterialSpan50KG(*policy,repSlot,repPass,*bank_,imageOwner,scalar,span,md);
        if(empty){span.span.firstQuad=0;span.span.quadCount=0;}
        if(!converted){
            // md owns its field string. Report it while alive, without casting
            // its distinct error enum or retaining a dangling field pointer.
            std::fprintf(stderr,"original_garage_model_scalar_rejected row=%d slot=%u pass=%u representative_row=%d representative_slot=%u representative_pass=%u family=%u error=%u policy_pass=%u policy_pc=%u field=%.160s\n",
                *row,slot,pass,*repRow,repSlot,repPass,static_cast<unsigned>(family),
                static_cast<unsigned>(md.error),md.pass,md.sourcePC,md.field.c_str());
            d.field="actual Garage model scalar policy";
            return reject(d,S::unsupportedPass,md.sourcePC);
        }
        using P=assets::OriginalUiGarageProgram50KG;
        const auto expected=family==P::prelitCompressed?OriginalUiMaterialMode50KG::sourcePrelitCompressed:
            family==P::diffuse?OriginalUiMaterialMode50KG::sourceDiffuse:
            family==P::diffuseCompressed?OriginalUiMaterialMode50KG::sourceDiffuseCompressed:
            OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
        if(span.span.materialMode!=expected)return reject(d,S::unsupportedPass,0x247110);
        if(!contributions_.empty()){
            const auto& prior=contributions_.back().span.span;
            if(binding.firstQuad<prior.firstQuad+prior.quadCount)return reject(d,S::collector,0x24B9B0);
            if(map.representative.registration==maps_.back().representative.registration&&!history.equivalent(images_.back()))
                return reject(d,S::texture,0x249610);
        }
        // Reserve all allocations before publishing the new parallel records.
        contributions_.reserve(contributions_.size()+1);maps_.reserve(maps_.size()+1);images_.reserve(images_.size()+1);
        contributions_.push_back({std::move(origin),std::move(representative),std::move(policy),slot,repSlot,span});
        maps_.push_back(map);images_.push_back(history);output=span;outputMap=map;return true;
    }catch(...){return reject(d,S::allocation,0x24B9B0);}
}
bool OriginalUiSourceFrame50KM::attachGarageCanonical(OriginalUiGarageCanonicalSubmission50KG& transaction)noexcept {
    // This remains private to the EXISTING complete frame owners. No Garage
    // SourceOwner or SourceFrame construction friend is introduced here.
    if(transaction.frame_||!transaction.pinned_||transaction.terminal_||transaction.maps_.empty()||
       owner_!=transaction.owner_||frameGeneration_!=transaction.generation_||!native_||garageCanonical_)return false;
    garageCanonical_=&transaction;transaction.frame_=this;return true;
}
bool OriginalUiGarageCanonicalSubmission50KG::preflight(const OriginalUiSourceFrame50KM& frame,
    const OriginalUiSourceOwner50KM* owner,std::uint64_t generation,const OriginalUiNativeDraw50KG* input,
    const VehicleWindowGl50DS& renderer)const noexcept {
    return frame_==&frame&&preflightBindingSet(owner,generation,input,renderer);
}
bool OriginalUiGarageCanonicalSubmission50KG::stageParticleContexts249610(
    std::int32_t row,const void* receiver,const assets::OriginalUiArchive50KG& archive,
    const assets::OriginalUiMainParticleMaterial50KG& policy,const std::array<std::uint32_t,2>& indices,
    const std::vector<OriginalUiMaterialSpan50KG>& spans,D& d)noexcept {
    d={};
    if(!pinned_||frame_||attempted_||terminal_||!receiver||!contributions_.empty()||
       archive.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::garage)||
       !policy.identity||!policy.templateIdentity||!policy.textureIdentity)
        return reject(d,S::phase,0x24B218);
    try {
        for(std::size_t state=0;state<2;++state){
            Map map;assets::OriginalStandardMaterialNativeBinding50KG history;
            if(!table_->prepareGarageParticleMap249610(receipt_,row,state,map,d)||
               map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
               !authenticateImage(map,indices[state],history,d))return false;
            const auto* source=map.representative.material->source();
            const auto* bytes=source?source->originalBytes():nullptr;
            const auto member=archive.find(policy.identity->name);
            assets::OriginalStandardMaterialPassView50LA view;
            if(!bytes||member.identity!=policy.identity||bytes->size()!=member.size||
               std::memcmp(bytes->data(),member.data,member.size)||
               !map.representative.material->pass4C(0,view)||!view.constructionTemplate||
               view.constructionTemplate->identity!=policy.templateIdentity||
               bank_->sourceMember(indices[state]).identity!=policy.textureIdentity)
                return reject(d,S::reference,0x249BA0);
            for(std::size_t i=0;i<map.contextAfter249610.fields.size();++i)
                if(map.contextAfter249610.fields[i].value!=policy.sourceStates[state].fields[i].value)
                    return reject(d,S::unsupportedPass,0x249BA0);
            for(std::size_t i=0;i<6;++i)
                if(map.contextAfter249610.sampler[i].value!=policy.sourceSamplers[state][i])
                    return reject(d,S::unsupportedPass,0x249BA0);
            //Source24B218..2C8 binds BOTH real contexts before the ParticleCode
            //run. They are not two passes and not a binding per visible quad.
            Contribution contribution;contribution.screenReceiver=receiver;
            contributions_.push_back(std::move(contribution));maps_.push_back(map);images_.push_back(history);
        }
        particleSpans_=spans;particleReceiver_=receiver;return true;
    }catch(...){return reject(d,S::allocation,0x24B218);}
}
bool OriginalUiGarageCanonicalSubmission50KG::stageWarning470ScreenPass(
    OriginalUiGarageDrawContribution50KG& draw,std::int32_t row,std::uint32_t pass,const void* receiver,
    const assets::OriginalUiArchive50KG& archive,const assets::OriginalUiMaterial50KG& material,
    const assets::OriginalUiNativeMaterial50KG& policy,const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& out,Map& mapOut,D& d)noexcept {
    d={};assets::OriginalStandardMaterialCollectorEntryView50LA origin;
    if(owner_!=&draw||generation_!=draw.publication_||draw.sourcePC_!=0x249610||!table_||
       !table_->coldCollectorPass(receipt_,row,pass,origin)||
       !draw.warning470DrainReceiverCurrent(receiver,origin.registration,origin.finalLeaf))
        return reject(d,S::phase,0x249610);
    // Shared material conversion/history, separate exact470 receiver proof.
    return stageScreenPass(row,pass,receiver,archive,material,policy,binding,out,mapOut,d);
}
bool OriginalUiGarageCanonicalSubmission50KG::stageWarning470UntexturedScreenPass(
    OriginalUiGarageDrawContribution50KG& draw,std::int32_t row,std::uint32_t pass,const void* receiver,
    const assets::OriginalUiArchive50KG& archive,const assets::OriginalUiMaterial50KG& material,
    const assets::OriginalUiNativeUntexturedMaterial50KG& policy,const OriginalUiUntexturedMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& out,Map& mapOut,D& d)noexcept {
    d={};assets::OriginalStandardMaterialCollectorEntryView50LA origin;
    if(owner_!=&draw||generation_!=draw.publication_||draw.sourcePC_!=0x249BA0||!table_||
       !table_->coldCollectorPass(receipt_,row,pass,origin)||
       !draw.warning470DrainReceiverCurrent(receiver,origin.registration,origin.finalLeaf))
        return reject(d,S::phase,0x249BA0);
    return stageUntexturedScreenPass(row,pass,receiver,archive,material,policy,binding,out,mapOut,d);
}
bool OriginalUiGarageCanonicalSubmission50KG::stageScreenPass(std::int32_t row,std::uint32_t pass,const void* receiver,
    const assets::OriginalUiArchive50KG& archive,const assets::OriginalUiMaterial50KG& material,
    const assets::OriginalUiNativeMaterial50KG& policy,const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& out,Map& mapOut,D& d)noexcept {
    d={};if(!pinned_||frame_||attempted_||terminal_||!receiver)return reject(d,S::phase,0x24B278);
    try {Map map;assets::OriginalStandardMaterialNativeBinding50KG history;
        if(!table_->prepareColdScreenPrototypeMap249610(receipt_,row,pass,0,map,d)||
           map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
           !authenticateImage(map,binding.textureIndex,history,d))return false;
        const auto rp=map.representative.pass;
        const auto* source=map.representative.material->source();const auto* bytes=source?source->originalBytes():nullptr;
        const auto member=material.identity?archive.find(material.identity->name):assets::OriginalUiMemberView50KG{};
        assets::OriginalStandardMaterialPassView50LA view;
        if(rp<0||static_cast<std::size_t>(rp)>=material.passes.size()||
           !map.representative.material->pass4C(static_cast<std::size_t>(rp),view)||!view.mapCount20||*view.mapCount20!=1||
           view.animation10||!view.uv1E||!view.flags21||!view.constructionTemplate)return reject(d,S::reference,0x249BA0);
        const auto* decoded=bank_->images()->decodedTexture(binding.textureIndex);if(!decoded)return reject(d,S::texture,0x249818);
        assets::OriginalUiMaterial50KG fallback;assets::OriginalUiNativeMaterial50KG fallbackPolicy;
        if(source){
            if(!bytes||!member.identity||member.identity!=material.identity||bytes->size()!=member.size||
               std::memcmp(bytes->data(),member.data,member.size))return reject(d,S::reference,0x249BA0);
        }else{
            const auto* textureName=map.texture->sourceMember();
            const auto textureMember=textureName?archive.find(*textureName):assets::OriginalUiMemberView50KG{};
            if(material.identity||material.passes.size()!=1||
               material.provenance.origin!=assets::OriginalUiMaterialOrigin50KG::garageCanonicalScreenMissingPsmFallback)
                return reject(d,S::reference,0x2475C0);
            if(!map.representative.material->prepareGarageScreenFallback2475C0(static_cast<std::size_t>(rp),
                *map.texture,archive,textureMember,*decoded,fallback,fallbackPolicy,d))return false;
            if(material.provenance.archive!=fallback.provenance.archive||
               material.provenance.fallbackTexture!=fallback.provenance.fallbackTexture||
               material.provenance.registeredName!=fallback.provenance.registeredName||
               material.provenance.attemptedPsmMember!=fallback.provenance.attemptedPsmMember)
                return reject(d,S::reference,0x2475C0);
        }
        // A fallback is lowered from this exact representative's current
        // constructed state again, not from a caller-authored recipe or alias.
        const auto& authored=source?material.passes[static_cast<std::size_t>(rp)]:fallback.passes[0];
        if(authored.microprogram!="ScreenObject"||view.constructionTemplate->identity!=authored.inheritedTemplate||
           (*view.uv1E)[0]!=authored.textureCoordinateIndex||bool(*view.flags21&2u)!=bool(authored.enabled))return reject(d,S::program,0x24B278);
        using F=assets::OriginalUiGsField50KG;
        for(std::size_t i=0;i<map.contextAfter249610.fields.size();++i){auto value=authored.gs.fields[i].value;
            if(decoded->kind==2){if(i==static_cast<std::size_t>(F::sourceAlphaValueWhenAis0)||i==static_cast<std::size_t>(F::sourceAlphaValueWhenAis1))value=255;
                if(i==static_cast<std::size_t>(F::methodOfExpandingTextureAlpha))value=1;}
            if(map.contextAfter249610.fields[i].value!=value)return reject(d,S::unsupportedPass,0x249BA0);}
        const std::int32_t sampler[]{authored.lodCalculationMethod,authored.maximumMipLevel,authored.magFilter,authored.minFilter,authored.lodL,authored.lodK};
        for(std::size_t i=0;i<6;++i)if(map.contextAfter249610.sampler[i].value!=sampler[i])return reject(d,S::unsupportedPass,0x249BA0);
        OriginalUiMaterialSpan50KG span;if(!convertOriginalUiMaterialSpan50KG(source?policy:fallbackPolicy,binding,span))return reject(d,S::unsupportedPass,0x24B26C);
        Contribution contribution;contribution.screenReceiver=receiver;contribution.span=span;
        contributions_.reserve(contributions_.size()+1);maps_.reserve(maps_.size()+1);images_.reserve(images_.size()+1);
        contributions_.push_back(std::move(contribution));maps_.push_back(map);images_.push_back(history);out=span;mapOut=map;return true;
    }catch(...){return reject(d,S::allocation,0x24B278);}
}
bool OriginalUiGarageCanonicalSubmission50KG::stageUntexturedScreenPass(std::int32_t row,std::uint32_t pass,const void* receiver,
    const assets::OriginalUiArchive50KG& archive,const assets::OriginalUiMaterial50KG& material,
    const assets::OriginalUiNativeUntexturedMaterial50KG& policy,const OriginalUiUntexturedMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& out,Map& mapOut,D& d)noexcept {
    d={};if(!pinned_||frame_||attempted_||terminal_||!receiver)return reject(d,S::phase,0x249BA0);
    try {Map map;if(!table_->prepareGarageUntexturedState249BA0(receipt_,row,pass,map,d))return false;
        const auto rp=map.representative.pass;const auto* source=map.representative.material->source();
        const auto* bytes=source?source->originalBytes():nullptr;
        const auto member=material.identity?archive.find(material.identity->name):assets::OriginalUiMemberView50KG{};
        assets::OriginalStandardMaterialPassView50LA view;
        if(rp<0||static_cast<std::size_t>(rp)>=material.passes.size()||!bytes||!member.identity||member.identity!=material.identity||
           bytes->size()!=member.size||std::memcmp(bytes->data(),member.data,member.size)||
           !map.representative.material->pass4C(static_cast<std::size_t>(rp),view)||!view.constructionTemplate)
            return reject(d,S::reference,0x249BA0);
        const auto& authored=material.passes[static_cast<std::size_t>(rp)];
        if(authored.microprogram!="ScreenObject"||authored.textureCount||view.constructionTemplate->identity!=authored.inheritedTemplate)
            return reject(d,S::program,0x249BA0);
        for(std::size_t i=0;i<map.contextAfter249610.fields.size();++i)
            if(map.contextAfter249610.fields[i].value!=authored.gs.fields[i].value)return reject(d,S::unsupportedPass,0x249BA0);
        OriginalUiMaterialSpan50KG span;if(!convertOriginalUiUntexturedMaterialSpan50KG(policy,binding,span))return reject(d,S::unsupportedPass,0x24B26C);
        Contribution contribution;contribution.screenReceiver=receiver;contribution.span=span;
        contributions_.reserve(contributions_.size()+1);maps_.reserve(maps_.size()+1);images_.reserve(images_.size()+1);
        contributions_.push_back(std::move(contribution));maps_.push_back(map);images_.emplace_back();out=span;mapOut=map;return true;
    }catch(...){return reject(d,S::allocation,0x24B278);}
}
bool OriginalUiGarageCanonicalSubmission50KG::preflightBindingSet(
    const OriginalUiSourceOwner50KM* owner,std::uint64_t generation,
    const OriginalUiNativeDraw50KG* input,const VehicleWindowGl50DS& renderer)const noexcept {
    if(!pinned_||attempted_||terminal_||pending_||
       owner!=owner_||generation!=generation_||!input||
       (renderer_&&renderer_!=&renderer)||maps_.size()!=contributions_.size()||maps_.size()!=images_.size()||
       !table_->garageNativeBindingsCurrent(receipt_,maps_))return false;
    const auto& native=*input;const auto& draw=native.draw;
    const auto images=bank_->images();
    if(!images||!images->textures()||!draw.textures||
       !draw.textures->matchesSourceBank(*images->textures(),draw.texturePublication)||
       ((draw.spanCount!=0)&&(!draw.spans||!native.states))||native.stateCount!=draw.spanCount||
       (draw.spanCount==0&&draw.quadCount!=0))return false;
    if(particleReceiver_){
        if(contributions_.size()!=2||maps_.size()!=2||!maps_[0].nativeParticleRoute||!maps_[1].nativeParticleRoute||
           maps_[0].map!=0||maps_[1].map!=1||draw.spanCount!=particleSpans_.size())return false;
        for(std::size_t i=0;i<particleSpans_.size();++i)
            if(draw.spans[i].materialMode!=OriginalUiMaterialMode50KG::mainParticle||
               !sameSpan(particleSpans_[i],draw.spans[i],native.states[i]))return false;
        return true;
    }
    for(std::size_t i=0;i<contributions_.size();++i){const auto& c=contributions_[i];const auto& map=maps_[i];
        if(c.screenReceiver)continue;
        if(!c.origin||!c.representative||!c.origin->current()||!c.representative->current())return false;
        const auto* a=c.origin->materials().handle1C4(c.originSlot);const auto* b=c.representative->materials().handle1C4(c.representativeSlot);
        if(!a||!b||*a!=map.origin.materialRow||*b!=map.representative.materialRow)return false;}
    std::size_t found=0;
    for(std::uint32_t i=0;i<draw.spanCount;++i){
        const auto mode=draw.spans[i].materialMode;
        const bool model=mode==OriginalUiMaterialMode50KG::sourcePrelitCompressed||
            mode==OriginalUiMaterialMode50KG::sourceDiffuse||mode==OriginalUiMaterialMode50KG::sourceDiffuseCompressed||
            mode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
        if(!model&&!(found<contributions_.size()&&contributions_[found].screenReceiver))continue;
        while(found<contributions_.size()&&!contributions_[found].span.span.quadCount)++found;
        if(found>=contributions_.size()||!sameSpan(contributions_[found].span,draw.spans[i],native.states[i]))return false;
        ++found;
    }
    // No missing or invented model-family spans. Other programs/visibility and
    // callback completeness remain the sealed frame owner's actual contract.
    while(found<contributions_.size()&&!contributions_[found].span.span.quadCount)++found;
    return found==contributions_.size();
}
bool OriginalUiGarageCanonicalSubmission50KG::begin(const OriginalUiSourceFrame50KM& frame,
    const OriginalUiSourceOwner50KM* owner,std::uint64_t generation,const OriginalUiNativeDraw50KG* input,
    const VehicleWindowGl50DS& renderer)noexcept {
    if(!preflight(frame,owner,generation,input,renderer))return false;
    renderer_=&renderer;attempted_=true;pending_=true;return true;
}
bool OriginalUiGarageCanonicalSubmission50KG::beginGarageBatch(
    OriginalUiGarageDrawContribution50KG& draw,const VehicleWindowGl50DS& renderer)noexcept {
    //Only the actual source leaf drain can issue this subordinate transaction.
    //It has no SourceFrame pointer and never seals/accepts the enclosing frame.
    if(frame_||!draw.identityCurrent()||draw.phase_!=decltype(draw.phase_)::collectorDrain24B7B0||
       draw.sourcePC_!=0x24B26Cu||!draw.drainBatchPending_||draw.drainBatchSubmitted_||draw.drainBatchFailed_||
       draw.drainBatchCanonical_.get()!=this||!draw.renderer_||
       !preflightBindingSet(&draw,draw.publication_,&draw.drainBatchNative_,renderer))return false;
    // UI470's exact retained typed payload and epoch must still own the
    // current FIFO record at submission, not only when geometry was lowered.
    for(std::size_t i=0;i<contributions_.size();++i)if(contributions_[i].screenReceiver&&
       !draw.warning470DrainBindingCurrent(contributions_[i].screenReceiver,
           maps_[i].origin.registration,maps_[i].representative.registration))return false;
    renderer_=&renderer;attempted_=true;pending_=true;return true;
}
bool OriginalUiGarageCanonicalSubmission50KG::finish(std::uint64_t generation,bool accepted,bool started)noexcept {
    if(!pending_||generation!=generation_){terminal_=true;if(table_)table_->poisonGarageNativeBindings();unpin();return false;}
    pending_=false;
    if(accepted){
        if(!table_->garageNativeBindingsCurrent(receipt_,maps_)){terminal_=true;table_->poisonGarageNativeBindings();unpin();return false;}
        table_->commitGarageNativeBindings249610(receipt_,maps_,images_);
    }else if(started){terminal_=true;table_->poisonGarageNativeBindings();}
    unpin();return !terminal_;
}
}
