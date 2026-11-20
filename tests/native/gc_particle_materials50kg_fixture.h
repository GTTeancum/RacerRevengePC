#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/original_gc_particle_material50kg.h"
#include "../../platform/linux/native/material_texture_gl50eb.h"
#include <memory>
#include <iostream>

namespace gc_particle_materials50kg_fixture {
namespace a=rr::assets;
namespace p=rr::platform;
namespace shared=pod_materials50ed_fixture::detail;
// Opaque bank identity for this exact source image, not an inferred GL name.
inline constexpr std::uint64_t textureIdentity=0x474344454d4f5052ull;
inline constexpr std::uint64_t materialIdentity=0x4743504152544943ull;
inline constexpr std::size_t rgbaBytes=128u*1024u*4u;           // stock atlas (receipt only)
inline constexpr std::size_t maximumAtlasRgbaBytes=1024u*1024u*4u; // the 1024 per-texture GS bound
struct Diagnostic {
    std::string message;
    a::DecodeError48C source{};
    a::OriginalUiMaterialDiagnostic50KG material{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
// Cold, load-once source/texture owner. Raw indexed palette, exact RGBA and
// original alpha are retained. A single authentic atlas backs both selectable
// contexts. No cached menu texture, procedural atlas, recoloring or fallback.
// This source was authored without executing its loaders or any native draw.
class Owner final {
    struct Storage {
        a::Texture48E sourceTexture;
        a::OriginalGcParticleMaterial50KG material;
        a::MaterialTextureAssets50EA bank;
    };
    std::unique_ptr<Storage> storage_;
    // Explicit closeGpu while the creating context is current is mandatory.
    // As declared, GPU destruction precedes immutable CPU storage destruction.
    p::MaterialTextureGl50EB gpu_;
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    bool ownsResources()const noexcept{return gpu_.ownsResources();}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    const a::Texture48E* sourceTexture()const noexcept{return storage_?&storage_->sourceTexture:nullptr;}
    const a::OriginalGcParticleMaterial50KG* material()const noexcept{return storage_?&storage_->material:nullptr;}
    const p::MaterialTextureGl50EB* gpu()const noexcept{return gpu_.initialized()?&gpu_:nullptr;}
    // Original extracted members may be supplied directly. BOTH exact digests
    // are required; PSM identity alone does not authenticate caller pixels.
    // Publication is atomic; failure leaves an unloaded owner retryable.
    bool load(const std::vector<std::uint8_t>& psm,const std::vector<std::uint8_t>& pst,
        const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,Diagnostic& d)noexcept {
        d={};try{
            using shared::need;
            need(!storage_,"GC particle material already loaded");
            // The PSM is admitted by its own structure (prepareOriginalGcParticleMaterial50KG); the
            // stock GC identity is a receipt there.
            // The atlas pixels (PST) are each course's own data: the stock GC identity is a
            // receipt, not a gate (TA authors its own demoprt.pst). The PSM stays authenticated:
            // it is the one ParticleCode material every stock course shares.
            // The caller logs the course's own receipt (common_track_data role=particles.atlas_pst,
            // stock_course_receipts50CR::findHash(course,...)); nothing here compares with GC.
            need(pst.size()>=16&&pst.size()<=8u*1024u*1024u,"track particle atlas PST extent");
            auto next=std::make_unique<Storage>();
            a::TextureLimits48E limits;
            limits.input_bytes=pst.size();
            limits.decoded_bytes=2u*maximumAtlasRgbaBytes+16u;
            need(a::decodeTexture48E(pst,next->sourceTexture,d.source,nullptr,limits),
                "GC original demoprt PST decode");
            need(a::prepareOriginalGcParticleMaterial50KG(psm,templates,runtime,
                next->sourceTexture,next->material,d.material),"GC original particle material policy");
            const std::size_t atlasRgba=next->sourceTexture.mips.empty()?0u:next->sourceTexture.mips[0].rgba.size();
            need(atlasRgba&&atlasRgba<=maximumAtlasRgbaBytes,"track particle atlas RGBA extent");
            const a::MaterialTextureAssetInput50EA image{textureIdentity,&next->sourceTexture.base_rrtex};
            const std::array<a::MaterialTextureSlotInput50EA,2> slots{{
                {{materialIdentity,0,1,1},textureIdentity,a::MaterialTextureRequirement50EA::required},
                {{materialIdentity,0,1,2},textureIdentity,a::MaterialTextureRequirement50EA::required}}};
            const a::MaterialTextureAssetsInput50EA input{
                &image,1,slots.data(),2,{1,2,atlasRgba+16u,atlasRgba,1024}};
            need(next->bank.load(input,d.bank),"GC immutable particle atlas bank");
            const auto* published=next->bank.texture(0);
            need(published&&published->identity==textureIdentity&&
                published->texture.width==next->sourceTexture.mips[0].width&&
                published->texture.height==next->sourceTexture.mips[0].height&&
                published->texture.rgba==next->sourceTexture.mips[0].rgba,
                "track particle atlas bank differs from its decoded PST");
            storage_=std::move(next);return true;
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
        catch(...){try{d.message="GC particle cold source failure";}catch(...){}return false;}
    }
    // Original TRACKS/GC.RES only. This convenience cold path authenticates
    // the archive before decoding/extracting, then uses the same member gate.
    bool load(const std::filesystem::path& sourceArchive,
        const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,Diagnostic& d)noexcept {
        d={};try{
            using shared::need;
            need(!storage_,"GC particle material already loaded");
            shared::Inputs inputs;inputs.capacity=4205067;
            const auto bytes=inputs.read(sourceArchive,4205067);
            need(bytes.size()==4205067&&shared::sha256(bytes)==
                "69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",
                "GC particle exact original archive identity differs");
            a::DecodeLimits48C limits;
            limits.resource_file_bytes=4205067;limits.resource_decoded_bytes=31580160;
            limits.resource_entries=1190;
            a::ResArchive48C archive;
            need(a::decodeResource48C(bytes,archive,d.source,nullptr,limits),"GC particle original archive decode");
            std::vector<std::uint8_t> psm,pst;
            need(a::extractResMember48C(archive,a::originalGcParticlePsm50KG().member,psm,d.source,
                a::originalGcParticlePsm50KG().bytes),"GC particle original PSM extraction");
            need(a::extractResMember48C(archive,a::originalGcParticlePst50KG().member,pst,d.source,
                a::originalGcParticlePst50KG().bytes),"GC particle original PST extraction");
            return load(psm,pst,templates,runtime,d);
        }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
        catch(...){try{d.message="GC particle archive source failure";}catch(...){}return false;}
    }
    // No context creation, renderer publication or implicit draw. The caller
    // supplies the actual unshared context and its creating-thread callback.
    // As in other cold fixtures, this owner sets its exact fixed byte budget.
    bool createGpu(const p::MaterialTextureGlRequest50EB& caller,
        p::MaterialTextureGlDiagnostic50EB& d)noexcept {
        d={};
        if(!storage_){d.error=p::MaterialTextureGlError50EB::assets;d.operation="GC particle cold bank missing";return false;}
        const p::MaterialTextureSampler50EB sampler{
            p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
            p::MaterialTextureWrap50EB::repeat,p::MaterialTextureWrap50EB::repeat};
        auto request=caller;request.samplers=&sampler;request.samplerCount=1;request.maximumGpuBytes=storage_->sourceTexture.mips.empty()?0u:storage_->sourceTexture.mips[0].rgba.size();
        return gpu_.create(request,storage_->bank,d);
    }
    // Any close attempt ends renderer publication. If cleanupIncomplete is
    // true, retain this owner/context and retry close before destruction.
    bool closeGpu(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
