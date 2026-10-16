#pragma once
#include "ra_fire_model_sources50la.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/material_texture_assets50ea.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace rr::platform {class MaterialTextureGl50EB;class VehicleWindowGl50DS;}
namespace ra_static_fire_visual50LA {
namespace a=rr::assets;
class Owner;
// Only the actual renderer can mint this receipt. Copying it retains the source
// owner, but does not preserve validity after another frame or publication.
class CompletedDrawReceipt final {
    friend class Owner;
    friend class rr::platform::VehicleWindowGl50DS;
    std::shared_ptr<const Owner> owner_;
    std::uint64_t publication_=0,generation_=0;
public:
    CompletedDrawReceipt()=default;
};
// Native static-world CPU lowering, NOT a retail240110/model-cache owner.
// Cold construction mints no completion. Private renderer joins below own
// only this bounded native publication's stamps, never retail scene ancestry.
class Owner final {
public:
    struct Span {
        std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0;
        std::size_t worldInstance=0;
        std::uint32_t zone=0,ordinal=0;
        std::int32_t materialRow=-1;
    };
    // Bounded native-publication stamps, NOT the retail shared scene stamp or
    // a SoulTree/emission-ready attachment. Numeric data comes from real spans.
    struct NativeAttachment {
        std::size_t worldInstance=0;
        std::uint32_t zone=0,ordinal=0;
        std::array<double,16> actualWorldMatrix{};
        std::uint16_t publicationStamp=0,lastSubmittedStamp=0;
        bool submittedInCompletedDraw=false;
    };
    struct Diagnostic {
        const char* failure=nullptr;
        ra_fire_model_sources50LA::Owner::Diagnostic facets;
        a::DecodeError48C decode;
        a::OriginalUiPageDiagnostic50KG text;
        a::OriginalStandardMaterialDiagnostic50LA material;
        a::MaterialTextureAssetDiagnostic50EA image;
    };
private:
    friend class rr::platform::MaterialTextureGl50EB;
    friend class rr::platform::VehicleWindowGl50DS;
    // Private typed publication leases. Each publishing owner also holds a
    // strong Owner reference; callers cannot mint, reset or retire these links.
    const rr::platform::MaterialTextureGl50EB* imagePublication_=nullptr;
    const rr::platform::VehicleWindowGl50DS* rendererPublication_=nullptr;
    std::uint64_t rendererPublicationSerial_=0,nativeCompletedGeneration_=0;
    std::uint16_t nativePublicationStamp_=0;
    bool nativeCompletionLive_=false;
    std::array<std::uint16_t,4> nativeLastSubmittedStamp_{};
    std::array<bool,4> nativeSubmitted_{};
    // Called only from the renderer's actual completed gameplay publication.
    // This is not an externally callable boolean/generation update surface.
    bool recordNativeCompleted(const rr::platform::VehicleWindowGl50DS* renderer,
        std::uint64_t serial,std::uint64_t generation,const std::array<bool,4>& submitted)noexcept {
        if(!renderer||rendererPublication_!=renderer||!imagePublication_||closed_||
           serial!=rendererPublicationSerial_||!serial||
           nativeCompletedGeneration_==std::numeric_limits<std::uint64_t>::max()||
           generation!=nativeCompletedGeneration_+1)return false;
        nativeCompletedGeneration_=generation;nativeCompletionLive_=true;
        nativePublicationStamp_=static_cast<std::uint16_t>(generation);
        nativeSubmitted_=submitted;
        for(std::size_t i=0;i<4;++i)if(submitted[i])nativeLastSubmittedStamp_[i]=nativePublicationStamp_;
        return true;
    }
    bool publishImage(const rr::platform::MaterialTextureGl50EB* image)noexcept {
        if(!image||!finished_||closed_||imagePublication_||rendererPublication_)return false;
        imagePublication_=image;return true;
    }
    void retireImage(const rr::platform::MaterialTextureGl50EB* image)noexcept {
        if(imagePublication_!=image||rendererPublication_)std::terminate();imagePublication_=nullptr;
    }
    bool publishRenderer(const rr::platform::VehicleWindowGl50DS* renderer,
        const rr::platform::MaterialTextureGl50EB* image)noexcept {
        if(!renderer||!image||imagePublication_!=image||rendererPublication_||!finished_||closed_||
           rendererPublicationSerial_==std::numeric_limits<std::uint64_t>::max())return false;
        ++rendererPublicationSerial_;nativeCompletedGeneration_=0;nativePublicationStamp_=0;nativeCompletionLive_=false;
        nativeLastSubmittedStamp_={};nativeSubmitted_={};
        rendererPublication_=renderer;return true;
    }
    void retireRenderer(const rr::platform::VehicleWindowGl50DS* renderer)noexcept {
        if(rendererPublication_!=renderer)std::terminate();rendererPublication_=nullptr;nativeCompletionLive_=false;
    }
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context_;
    ra_fire_model_sources50LA::Owner facets_;
    a::Mesh48B mesh_;
    std::array<Span,4> spans_{};
    std::array<std::array<double,16>,4> attachmentMatrices_{};
    std::vector<std::uint8_t> pst_,psm_;
    a::Texture48E decoded_;
    a::MaterialTextureAssets50EA pixels_;
    a::OriginalStandardMaterialCollectorReceipt50LA collector_;
    const a::OriginalStandardTextureResource50LA* atlas_=nullptr;
    bool attempted_=false,staged_=false,finishAttempted_=false,finished_=false,closed_=false;
    static bool reject(Diagnostic& d,const char* why)noexcept{d.failure=why;return false;}
    bool mounted(const char* name,std::size_t size,const char* hash,std::vector<std::uint8_t>& out) {
        const auto& provider=domain_->archiveProvider3E1A70();
        a::OriginalStandardArchiveProvider50LA::MemberView member;
        a::OriginalStandardArchiveProvider50LA::MountView mount;
        if(!provider||!provider->sealed()||provider->lookup132820(name,member)!=
            a::OriginalStandardArchiveProvider50LA::Lookup::present||!provider->mount(member.mount,mount)||
           !mount.archiveSha256||*mount.archiveSha256!=ra_cold_sources50LA::archiveSha||
           mount.archiveBytes!=4604561||mount.memberCount!=1010||!member.data||member.size!=size)return false;
        out.assign(member.data,member.data+member.size);
        return pod_materials50ed_fixture::detail::sha256(out)==hash;
    }
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    // Caller already mounted authentic RA into its actual Application domain.
    // Appends exact corpus, retains real DefaultMT slots, then prepares CPU
    // geometry/image. Partial source effects remain owned until terminal close.
    bool stage(std::shared_ptr<const ra_cold_sources50LA::Owner> source,
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context,
        const a::OriginalStandardMaterialPath50LA& path,Diagnostic& d)noexcept {
        d={};if(attempted_||closed_)return reject(d,"native static stage already attempted");attempted_=true;
        if(!source||!domain||!domain->prepared()||!context||!context->live())return reject(d,"actual source/domain/Video required");
        domain_=std::move(domain);context_=std::move(context);
        try {
            if(!mounted("textures\\gargoylebody.psm",362,"8540bd531107844a55c9aa4e8973f1860b834c7ae2e861a9265f68d70204cf47",psm_)||
               !mounted("textures\\Gargoylebody.pst",22860,"ce2b8a9f2b7bcba13a55d099bd9f6c3dcbc0c02de54e80b8db9f997bc141762f",pst_))
                return reject(d,"actual same-provider gargoylebody source differs");
            auto material=std::make_shared<a::OriginalStandardMaterialSource50LA>();
            if(!material->prepare("textures\\gargoylebody.psm",psm_,d.text))return reject(d,"PSM syntax");
            const std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials{material};
            const std::vector<a::OriginalStandardTextureSourceInput50LA> textures{{"textures\\Gargoylebody.pst",pst_}};
            if(!domain_->appendOriginalSources(materials,textures,d.material)||
               !facets_.prepare(std::move(source),domain_,path,d.facets))return reject(d,"canonical source/reference prefix");
            mesh_.vertices.reserve(76);
            for(std::size_t i=0;i<4;++i){
                const auto* facet=facets_.sourceFacet(i);const auto* place=facets_.placement(i);
                const auto* scene=facet&&facet->payload?facet->payload->scene():nullptr;
                if(!place||!scene||scene->nodes.size()!=1||scene->world_space||scene->identity_matrix_palette||
                   scene->lods.size()!=1||scene->lods[0].surfaces.size()!=1||scene->nodes[0].parent!=-1)
                    return reject(d,"actual single-node local geometry differs");
                for(unsigned j=0;j<16;++j){
                    if(scene->nodes[0].local_matrix[j]!=(j%5==0?1.0:0.0)||!std::isfinite(place->matrix[j]))
                        return reject(d,"authenticated root or placement differs");
                    attachmentMatrices_[i][j]=place->matrix[j];
                }
                for(unsigned j=4;j<7;++j)if(scene->quantization[j]!=0)return reject(d,"local quantization origin");
                const auto& surface=scene->lods[0].surfaces[0];
                const auto* row=facet->materials.handle1C4(0);
                if(!row||surface.material_index!=0||surface.component_word!=0x00ff010fu||
                   !surface.has_original_colors||surface.vertices.size()!=19||surface.source_strips!=4||
                   surface.indices.empty()||surface.indices.size()%3||
                   surface.indices.size()>a::max_mesh_indices-mesh_.indices.size())return reject(d,"source surface shape");
                auto& span=spans_[i];span.vertexStart=static_cast<std::uint32_t>(mesh_.vertices.size());
                span.indexStart=static_cast<std::uint32_t>(mesh_.indices.size());span.vertexCount=19;
                span.indexCount=static_cast<std::uint32_t>(surface.indices.size());span.worldInstance=facet->worldInstance;
                span.zone=place->zone;span.ordinal=place->ordinal;span.materialRow=*row;
                for(const auto& vertex:surface.vertices){
                    if(vertex.bone!=0)return reject(d,"unexpected vertex palette dependency");
                    a::Vertex48B out{};
                    for(unsigned axis=0;axis<3;++axis){
                        double value=0;
                        for(unsigned k=0;k<3;++k)value+=vertex.position_local[k]*place->matrix[k*4+axis];
                        value+=place->matrix[12+axis];out.position[axis]=static_cast<float>(value);
                        if(!std::isfinite(value)||!std::isfinite(out.position[axis]))return reject(d,"placed vertex nonfinite");
                        if(mesh_.vertices.empty())mesh_.bounds.minimum[axis]=mesh_.bounds.maximum[axis]=out.position[axis];
                        else {mesh_.bounds.minimum[axis]=std::min(mesh_.bounds.minimum[axis],out.position[axis]);
                              mesh_.bounds.maximum[axis]=std::max(mesh_.bounds.maximum[axis],out.position[axis]);}
                    }
                    for(unsigned c=0;c<2;++c){out.uv[c]=static_cast<float>(vertex.uv[c]);
                        if(!std::isfinite(out.uv[c]))return reject(d,"source UV nonfinite");}
                    for(unsigned c=0;c<4;++c)out.color[c]=vertex.color[c];
                    mesh_.vertices.push_back(out);
                }
                for(auto index:surface.indices){if(index>=19)return reject(d,"source triangle index");
                    mesh_.indices.push_back(span.vertexStart+index);}
            }
            // Existing native indexed decoder, not an offline cache identity.
            if(!a::decodeTexture48E(pst_,decoded_,d.decode)||decoded_.kind!=1||decoded_.mips.size()!=4||
               decoded_.mips.back().width!=128||decoded_.mips.back().height!=128)return reject(d,"original indexed PST decode");
            const a::MaterialTextureAssetInput50EA image{1,&decoded_.base_rrtex}; // local bank ordinal only
            const a::MaterialTextureAssetsInput50EA input{&image,1,nullptr,0,{1,0,65552,65536,128}};
            if(!pixels_.load(input,d.image))return reject(d,"same-source immutable image bank");
            staged_=true;return true;
        }catch(...){return reject(d,"native static cold allocation failure");}
    }
    // Explicit native cold population policy. Call only after all intended
    // domain references have been staged. No GPU or display receipt is issued.
    bool populateAndFinish(Diagnostic& d)noexcept {
        d={};if(!staged_||finishAttempted_||closed_||!context_->live())return reject(d,"native cold phase");
        finishAttempted_=true; // A partial population/verification is terminal for this owner.
        if(!domain_->populate(context_,d.material))return reject(d,"actual whole-domain population");
        collector_=domain_->coldCollector14D440();
        a::OriginalStandardMaterialMapBindingView50LA map;
        for(std::size_t i=0;i<4;++i){
            const auto& table=domain_->materials382B38();const a::OriginalStandardMaterialObject50LA* material=nullptr;
            a::OriginalStandardMaterialPassView50LA pass;
            if(!table->resolve298E60(spans_[i].materialRow,material,d.material)||!material||
               !material->source()||!material->source()->originalBytes()||*material->source()->originalBytes()!=psm_||
               !material->passCount48()||*material->passCount48()!=1||!material->pass4C(0,pass)||
               !pass.mapCount20||*pass.mapCount20!=1||!pass.state30||pass.state30->contextCount()!=1||
               !pass.program18||!pass.uv1E||(*pass.uv1E)[0]!=0||pass.textureAnimation10||pass.animation10||
               pass.constructionTemplate!=context_->materialApplication()->templates().get(a::OriginalUiTemplate50KG::defaultMt))
                return reject(d,"actual canonical DefaultMT pass");
            std::int16_t program=-1;
            if(!domain_->programs382B70()->find298C80("PreLitCompressed",program,d.material.program)||program!=*pass.program18||
               !table->prepareColdRepresentativeMap249610(collector_,spans_[i].materialRow,0,0,map,d.material)||
               map.preparation!=a::OriginalStandardMaterialMapPreparation50LA::ready||!map.texture||map.origin.finalLeaf<0)
                return reject(d,"actual canonical program/map/collector");
            const auto* texture=map.texture;a::OriginalStandardTextureLevelView50LA level;
            if(!texture->originalBytes()||*texture->originalBytes()!=pst_||!texture->sourceKind()||*texture->sourceKind()!=1||
               !texture->levelCountB0()||*texture->levelCountB0()!=4||
               !texture->formatAC()||*texture->formatAC()!=8||!texture->bitsPerPixelE8()||*texture->bitsPerPixelE8()!=8||
               !texture->palette208()||
               std::memcmp(texture->palette208(),pst_.data()+12,1024)||!texture->level100(0,level)||
               level.sourceMipOrdinal!=3||level.width104!=128||level.height110!=128||
               level.sourceHeaderOffset!=decoded_.mips.back().source_header_offset||
               level.sourceIndicesOffset!=decoded_.mips.back().source_indices_offset||!level.linearSourceIndices||
               level.linearSourceBytes!=16384||std::memcmp(level.linearSourceIndices,pst_.data()+level.sourceIndicesOffset,16384)||
               (i&&texture!=atlas_))return reject(d,"same canonical original mip/image");
            for(std::size_t slot=0;slot<4;++slot){
                a::OriginalStandardTextureLevelView50LA actual;
                const auto& decoded=decoded_.mips[3-slot];
                if(!texture->level100(slot,actual)||actual.sourceMipOrdinal!=3-slot||
                   actual.width104!=static_cast<std::int32_t>(decoded.width)||
                   actual.height110!=static_cast<std::int32_t>(decoded.height)||
                   actual.sourceHeaderOffset!=decoded.source_header_offset||actual.sourceIndicesOffset!=decoded.source_indices_offset)
                    return reject(d,"canonical/decoded mip ordinal mismatch");
            }
            atlas_=texture;
        }
        finished_=true;return true;
    }
    // Reacquire actual final map state before a future native renderer lowers
    // raster/sampler policy. Collector validity alone does not prove map bytes.
    bool currentMap(std::size_t i,a::OriginalStandardMaterialMapBindingView50LA& out,
        a::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
        if(!finished_||closed_||i>=4||!context_->live())return false;
        a::OriginalStandardMaterialMapBindingView50LA next;
        if(!domain_->materials382B38()->prepareColdRepresentativeMap249610(collector_,spans_[i].materialRow,0,0,next,d)||
           next.preparation!=a::OriginalStandardMaterialMapPreparation50LA::ready||next.texture!=atlas_)return false;
        out=next;return true;
    }
    // Current-only read of a renderer-minted receipt. A copied receipt cannot
    // forge a generation, survive close/recreate at the same address, or keep
    // canonical state valid after its actual collector/context has changed.
    bool nativeAttachment(const CompletedDrawReceipt& receipt,std::size_t i,
        NativeAttachment& out)const noexcept {
        out={};
        if(!nativeCompletionLive_||receipt.owner_.get()!=this||!rendererPublication_||!imagePublication_||closed_||i>=4||
           !receipt.publication_||receipt.publication_!=rendererPublicationSerial_||
           !receipt.generation_||receipt.generation_!=nativeCompletedGeneration_)return false;
        a::OriginalStandardMaterialMapBindingView50LA map;a::OriginalStandardMaterialDiagnostic50LA d;
        if(!currentMap(i,map,d))return false;
        const auto& source=spans_[i];out.worldInstance=source.worldInstance;out.zone=source.zone;out.ordinal=source.ordinal;
        out.actualWorldMatrix=attachmentMatrices_[i];out.publicationStamp=nativePublicationStamp_;
        out.lastSubmittedStamp=nativeLastSubmittedStamp_[i];out.submittedInCompletedDraw=nativeSubmitted_[i];
        return true;
    }
    const a::Mesh48B* mesh()const noexcept{return staged_&&!closed_?&mesh_:nullptr;}
    const Span* span(std::size_t i)const noexcept{return staged_&&!closed_&&i<4?&spans_[i]:nullptr;}
    const std::array<double,16>* attachmentMatrix(std::size_t i)const noexcept {
        return staged_&&!closed_&&i<4?&attachmentMatrices_[i]:nullptr;
    }
    const a::MaterialTextureAssets50EA* pixels()const noexcept{return staged_&&!closed_?&pixels_:nullptr;}
    const a::Texture48E* decodedTexture()const noexcept{return staged_&&!closed_?&decoded_:nullptr;}
    // Private native publications hold this owner through genuine GPU cleanup.
    // Explicit close cannot release canonical facets beneath either lease.
    bool close()noexcept {
        if(closed_)return true;
        if(imagePublication_||rendererPublication_)return false;
        if(!facets_.close())return false;
        finished_=false;staged_=false;atlas_=nullptr;context_.reset();domain_.reset();closed_=true;return true;
    }
};
}
