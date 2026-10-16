#pragma once
#include "original_pause_material_admission50la.h"
#include "../assets/import_texture48e.h"

namespace rr::game {
// Cold CPU pixels plus prospective native policies. No GL resource, submitted
// frame, renderer binding or original lifecycle completion is represented.
class OriginalPauseNativeResources50LA final {
    using Storage=OriginalPauseStorage50LA;
    using Admission=OriginalPauseMaterialAdmission50LA;
public:
    struct Diagnostic {
        std::uint32_t sourcePC=0;std::size_t reference=0,pass=0;
        assets::OriginalStandardMaterialDiagnostic50LA canonical;
        assets::OriginalUiMaterialDiagnostic50KG material;
        assets::DecodeError48C texture;
    };
    struct Image {
        const assets::OriginalStandardTextureResource50LA* resource=nullptr;
        const assets::OriginalUiMemberIdentity50KG* member=nullptr;
        std::size_t pstOffset=0,pstBytes=0;
        assets::Texture48E decoded;
    };
    struct Pass {
        // Full canonical origin/FINAL/map proof survives beside native policy.
        Admission::Pass canonical;
        bool enabled=false,hasTexture=false;
        assets::OriginalUiNativeMaterial50KG textured;
        assets::OriginalUiNativeUntexturedMaterial50KG untextured;
        std::shared_ptr<const Image> image;
        std::vector<std::shared_ptr<const Image>> frames;
    };
    struct Projection {std::size_t reference=0;std::vector<Pass> passes;};
private:
    friend class OriginalPauseNativeDraw50LA;
    bool prepareEmission(std::size_t reference,Projection& output,Diagnostic& d)noexcept {
        if(!current()||!admission_.serviceEmission(reference,d.canonical))return fail(d,0x24A758);
        return prepare(reference,output,d);
    }
    std::shared_ptr<Storage> owner_;
    Admission admission_;
    std::array<assets::OriginalUiMaterial50KG,36> parsed_;
    std::vector<std::shared_ptr<const Image>> images_;
    bool prepared_=false;
    static bool fail(Diagnostic& d,std::uint32_t pc)noexcept {d.sourcePC=pc;return false;}
    bool canonicalMatches(const assets::OriginalUiMaterialPass50KG& parsed,
        const Admission::PassState& source)const noexcept {
        if(source.state.contextCount()!=1||parsed.textureCount!=source.maps||
           parsed.enabled!=int((source.flags&2u)!=0)||parsed.textureCoordinateIndex!=source.uv[0])return false;
        for(std::size_t i=0;i<assets::originalUiGsFieldCount50KG;++i)
            if(parsed.gs.fields[i].value!=source.state.fields[i].value)return false;
        const std::int32_t sampler[]{parsed.lodCalculationMethod,parsed.maximumMipLevel,
            parsed.magFilter,parsed.minFilter,parsed.lodL,parsed.lodK};
        for(std::size_t i=0;i<6;++i)if(sampler[i]!=source.state.sampler[i].value)return false;
        return true;
    }
    static void effective(assets::OriginalUiMaterialPass50KG& p,
        const assets::OriginalStandardMaterialStateContext50LA& state)noexcept {
        for(std::size_t i=0;i<assets::originalUiGsFieldCount50KG;++i){
            const auto& source=state.fields[i];
            // Runtime TEXA has no authored member. Full canonical provenance
            // remains retained beside this existing native-policy descriptor.
            p.gs.fields[i]={source.value,source.templateMember,source.byteOffset,source.sourcePC};
        }
        p.lodCalculationMethod=state.sampler[0].value;p.maximumMipLevel=state.sampler[1].value;
        p.magFilter=state.sampler[2].value;p.minFilter=state.sampler[3].value;
        p.lodL=state.sampler[4].value;p.lodK=state.sampler[5].value;
    }
    std::shared_ptr<const Image> imageFor(const assets::OriginalStandardTextureResource50LA* texture,Diagnostic& d){
        for(const auto& image:images_)if(image->resource==texture)return image;
        if(prepared_)return {}; // Immutable cold bank: emission never decodes/adds a child.
        if(!texture||!texture->sourceMember()||!texture->originalBytes())return {};
        const auto member=owner_->data_->overlays()->find(*texture->sourceMember());
        auto image=std::make_shared<Image>();image->resource=texture;image->member=member.identity;
        if(!member.identity||!member.data||!texture->pstExtent24E880(image->pstOffset,image->pstBytes)||
           image->pstOffset>member.size||image->pstBytes>member.size-image->pstOffset)return {};
        const auto* bytes=texture->originalBytes();
        if(bytes->size()!=member.size||std::memcmp(bytes->data(),member.data,member.size))return {};
        // The canonical loader owns the exact child extent, including ATM.
        // No independent frame decoder, filename guess or invented image ID.
        const std::vector<std::uint8_t> pst(bytes->begin()+image->pstOffset,
            bytes->begin()+image->pstOffset+image->pstBytes);
        if(!assets::decodeTexture48E(pst,image->decoded,d.texture)||image->decoded.mips.empty())return {};
        const auto& base=image->decoded.mips.back();
        if(!texture->sourceWidth14()||!texture->sourceHeight18()||
           base.width!=std::uint32_t(*texture->sourceWidth14())||
           base.height!=std::uint32_t(*texture->sourceHeight18()))return {};
        images_.push_back(image);return image;
    }
public:
    OriginalPauseNativeResources50LA()noexcept=default;
    OriginalPauseNativeResources50LA(const OriginalPauseNativeResources50LA&)=delete;
    OriginalPauseNativeResources50LA& operator=(const OriginalPauseNativeResources50LA&)=delete;
    ~OriginalPauseNativeResources50LA(){close();}
    bool current()const noexcept{return prepared_&&admission_.current();}
    // Per-emission refresh; caller retains this owner while using raw canonical
    // proof pointers. Shared Image independently retains immutable decoded data.
    bool prepare(std::size_t reference,Projection& output,Diagnostic& d)noexcept {
        d={};d.reference=reference;
        if(!owner_||!admission_.current()||reference>=parsed_.size())return fail(d,0x246760);
        try {
            Admission::Projection canonical;
            if(!admission_.prepare(reference,canonical,d.canonical))return fail(d,0x24B1A0);
            if(canonical.passes.size()!=parsed_[reference].passes.size())return fail(d,0x247110);
            Projection next;next.reference=reference;
            for(std::size_t i=0;i<canonical.passes.size();++i){
                d.pass=i;auto& c=canonical.passes[i];Pass p;
                if(!canonicalMatches(parsed_[reference].passes[i],c.original))return fail(d,0x2476A0);
                p.enabled=c.enabled;
                if(c.enabled){
                    // Existing native ScreenObject policies support zero/one
                    // map. Never collapse a dual-map/fill/parameter program.
                    if(c.final.maps>1||c.final.state.contextCount()!=1||(c.final.flags&~2u)||
                       c.final.animation||c.final.parameterMask||!c.final.parameters.empty()||
                       c.final.program!=c.original.program)return fail(d,0x2474B8);
                    auto descriptor=parsed_[reference].passes[i];
                    descriptor.enabled=1;descriptor.textureCount=c.final.maps;
                    descriptor.textureCoordinateIndex=c.final.uv[0];
                    if(!c.final.maps){
                        effective(descriptor,c.final.state);
                        if(!assets::prepareOriginalUiNativeUntexturedMaterial50KG(descriptor,p.untextured,d.material))
                            return fail(d,0x2474B8);
                    }else{
                        const auto& map=c.maps[0];
                        if(map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready)
                            return fail(d,0x24B260); //Pending/null is not drawable pixels.
                        p.image=imageFor(map.texture,d);if(!p.image)return fail(d,0x24E880);
                        assets::OriginalStandardTextureRowView50LA textureRow;
                        if(!owner_->domain_->textures383400()->row(map.textureToken,textureRow)||
                           !textureRow.object04||!textureRow.references08||*textureRow.references08<=0)
                            return fail(d,0x298E60);
                        const auto* root=textureRow.object04;
                        if(root->animated102070()){
                            const auto* count=root->frameCount20();
                            if(!count||*count<=0||*count>64)return fail(d,0x102050);
                            for(std::int32_t f=0;f<*count;++f){
                                auto image=imageFor(root->frame102050(f),d);
                                if(!image)return fail(d,0x24E880);
                                p.frames.push_back(std::move(image));
                            }
                        }else{
                            if(root!=map.texture)return fail(d,0x102050);
                            p.frames.push_back(p.image);
                        }
                        bool selected=false;for(const auto& image:p.frames)selected|=image==p.image;
                        if(!selected)return fail(d,0x102050);
                        effective(descriptor,map.contextAfter249610);
                        const bool policy=c.original.textureAnimation ?
                            assets::prepareOriginalPauseAnimatedMaterial50LA(descriptor,p.image->decoded,
                                *c.original.textureAnimation,p.textured,d.material) :
                            assets::prepareOriginalUiNativeMaterial50KG(descriptor,p.image->decoded,p.textured,d.material);
                        if(!policy)
                            return fail(d,0x249610);
                        p.hasTexture=true;
                    }
                }
                p.canonical=std::move(c);next.passes.push_back(std::move(p));
            }
            if(!admission_.current())return fail(d,0x24B1A0);
            output=std::move(next);return true;
        }catch(...){return fail(d,0x247110);}
    }
    bool initialize(std::shared_ptr<Storage> owner,Diagnostic& d)noexcept {
        d={};if(owner_||!owner)return fail(d,0x246760);
        if(!admission_.initialize(owner,d.canonical))return fail(d,0x24B1A0);
        owner_=std::move(owner);
        try {
            const auto& archive=*owner_->data_->overlays();
            const auto& templates=owner_->context_->materialApplication()->templates();
            for(std::size_t i=0;i<parsed_.size();++i){
                d.reference=i;const auto source=assets::OriginalPausePrimitiveReferences50LA::source(i);
                bool parsed=false;
                if(source.missingPsm)parsed=assets::readOriginalPauseBackdropFallbackMaterial50LA(
                    archive,i==0?0:1,templates,assets::OriginalUiTemplate50KG::screenMt,
                    owner_->context_->materialRuntime(),parsed_[i],d.material);
                else{
                    std::string path=source.directory;path+='\\';path+=source.name;path+=".psm";
                    parsed=assets::readOriginalUiMaterial50KG(archive,path,templates,
                        assets::OriginalUiTemplate50KG::screenMt,owner_->context_->materialRuntime(),parsed_[i],d.material);
                }
                Projection ignored;
                if(!parsed){close();return fail(d,d.material.sourcePC?d.material.sourcePC:0x247110);}
                if(!prepare(i,ignored,d)){close();return false;}
            }
            prepared_=true;return true;
        }catch(...){close();return fail(d,0x247110);}
    }
    void close()noexcept {
        prepared_=false;images_.clear();
        for(auto& material:parsed_)material=assets::OriginalUiMaterial50KG{};
        admission_.close();owner_.reset();
    }
};
}
