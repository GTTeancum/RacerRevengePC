#include "../../../tests/native/ra_static_fire_visual50la.h"
#include "material_texture_gl50eb.h"
#include "original_ui_rider_residency50kg.h"
#include "../../../src/assets/original_native_texture_upload50la.h"
#include "../../../src/game/original_options_mutation_interlock50kg.h"
#include "../../../tests/native/ra_cold_aggregate50la.h"
#include <exception>
#include <limits>
#include <new>
#include <thread>
#include <utility>

namespace rr::platform {
namespace {
using U = std::uint32_t;
using I = std::int32_t;
constexpr U texture2D = 0x0DE1, rgba = 0x1908, rgba8 = 0x8058, unsignedByte = 0x1401;
constexpr U minFilter = 0x2801, magFilter = 0x2800, wrapS = 0x2802, wrapT = 0x2803;
constexpr U baseLevel = 0x813C, maximumLevel = 0x813D;
constexpr U pixelUnpackBuffer = 0x88EC, unpackAlignment = 0x0CF5;
constexpr U unpackRowLength = 0x0CF2, unpackSkipRows = 0x0CF3, unpackSkipPixels = 0x0CF4;
constexpr U maxTextureSize = 0x0D33;
constexpr U majorVersion = 0x821B, minorVersion = 0x821C, contextProfileMask = 0x9126;
constexpr I coreProfileBit = 1;
constexpr U textureWidth = 0x1000, textureHeight = 0x1001, textureInternalFormat = 0x1003;

bool fail(MaterialTextureGlDiagnostic50EB& d, MaterialTextureGlError50EB error,
    const char* operation) noexcept {
    if (d.error == MaterialTextureGlError50EB::none) { d.error = error; d.operation = operation; }
    return false;
}
I filter(MaterialTextureFilter50EB value) noexcept {
    switch (value) {
    case MaterialTextureFilter50EB::nearest: return 0x2600;
    case MaterialTextureFilter50EB::linear: return 0x2601;
    default: return 0;
    }
}
I wrap(MaterialTextureWrap50EB value) noexcept {
    switch (value) {
    case MaterialTextureWrap50EB::repeat: return 0x2901;
    case MaterialTextureWrap50EB::clampToEdge: return 0x812F;
    default: return 0;
    }
}
bool validate(const MaterialTextureGlRequest50EB& request,
    const assets::MaterialTextureAssets50EA& bank, std::size_t& total,
    MaterialTextureGlDiagnostic50EB& d) noexcept {
    if (!request.isCurrent)
        return fail(d, MaterialTextureGlError50EB::request, "exact-current-context predicate required");
#define REQUIRE(member, name) if (!request.gl.member) \
    return fail(d, MaterialTextureGlError50EB::entryPoint, name)
    REQUIRE(genTextures, "glGenTextures"); REQUIRE(deleteTextures, "glDeleteTextures");
    REQUIRE(bindTexture, "glBindTexture"); REQUIRE(texParameteri, "glTexParameteri");
    REQUIRE(texImage2D, "glTexImage2D"); REQUIRE(getTexLevelParameteriv, "glGetTexLevelParameteriv");
    REQUIRE(bindBuffer, "glBindBuffer"); REQUIRE(pixelStorei, "glPixelStorei");
    REQUIRE(getIntegerv, "glGetIntegerv"); REQUIRE(getError, "glGetError");
#undef REQUIRE
    if (!bank.loaded() || bank.textureCount() > assets::maxMaterialTextureAssets50EA)
        return fail(d, MaterialTextureGlError50EB::assets, "loaded bounded 50EA bank required");
    d.bankTextureCount = bank.textureCount();d.bankRgbaBytes = bank.rgbaByteCount();
    d.maximumGpuBytes = request.maximumGpuBytes;
    d.maximumBankRgbaBytes = assets::maxMaterialTextureRGBABytes50EA;
    if (request.samplerCount != bank.textureCount() ||
        (request.samplerCount && !request.samplers))
        return fail(d, MaterialTextureGlError50EB::request, "one explicit sampler per bank texture required");
    total = 0;
    for (U t = 0; t < bank.textureCount(); ++t) {
        d.texture = t;d.validatedPrefixRgbaBytes = total;
        d.candidateIdentity = 0;d.candidateRgbaBytes = 0;d.candidateWidth = 0;d.candidateHeight = 0;
        const auto* asset = bank.texture(t);
        if (!asset)
            return fail(d, MaterialTextureGlError50EB::assets, "missing immutable bank texture");
        const auto& texture = asset->texture;
        d.candidateIdentity = asset->identity;
        d.candidateWidth = texture.width;d.candidateHeight = texture.height;
        if (!texture.width || !texture.height ||
            texture.width > assets::max_texture_dimension || texture.height > assets::max_texture_dimension ||
            texture.width > static_cast<U>(std::numeric_limits<I>::max()) ||
            texture.height > static_cast<U>(std::numeric_limits<I>::max()))
            return fail(d, MaterialTextureGlError50EB::assets, "invalid RGBA8 dimensions");
        const std::uint64_t bytes = 4ull * texture.width * texture.height;
        d.candidateRgbaBytes = bytes;
        if (bytes > std::numeric_limits<std::size_t>::max() || bytes != texture.rgba.size())
            return fail(d, MaterialTextureGlError50EB::assets, "RGBA8 byte extent mismatch");
        if (total > request.maximumGpuBytes || bytes > request.maximumGpuBytes - total ||
            total > assets::maxMaterialTextureRGBABytes50EA ||
            bytes > assets::maxMaterialTextureRGBABytes50EA - total)
            return fail(d, MaterialTextureGlError50EB::budget, "RGBA8 upload exceeds explicit GPU byte budget");
        total += static_cast<std::size_t>(bytes);
        const auto& sampler = request.samplers[t];
        if (!filter(sampler.minification) || !filter(sampler.magnification) || !wrap(sampler.s) || !wrap(sampler.t))
            return fail(d, MaterialTextureGlError50EB::sampler, "explicit nonmip filters and wrap modes required");
    }
    d.texture = assets::unresolvedMaterialTexture50EA;
    d.validatedPrefixRgbaBytes = total;d.candidateIdentity = 0;d.candidateRgbaBytes = 0;
    d.candidateWidth = 0;d.candidateHeight = 0;
    return total == bank.rgbaByteCount() ||
        fail(d, MaterialTextureGlError50EB::assets, "immutable bank RGBA total mismatch");
}
}

struct MaterialTextureGl50EB::Storage {
    // One fixed, bounded allocation. No RGBA copy or per-texture CPU allocation.
    MaterialTextureGlAsset50EB textures[assets::maxMaterialTextureAssets50EA]{};
    MaterialTextureGlFunctions50EB gl{};
    void* context = nullptr;
    bool (*isCurrent)(void*) noexcept = nullptr;
    const std::thread::id thread = std::this_thread::get_id();
    const OriginalUiRiderSession50KG* riderSession=nullptr;
    std::uint64_t riderSerial=0;
    const void* riderBorrowers[4]{};
    bool riderIncluded[assets::maxMaterialTextureAssets50EA]{};
    bool riderRefsReleased=false;
    MaterialTextureSampler50EB riderSamplers[assets::maxMaterialTextureAssets50EA]{};
    U count = 0;
    std::size_t bytes = 0;
    bool initialized = false;
    const assets::MaterialTextureAssets50EA* uploadedSourceBank = nullptr;
    std::uint64_t uploadedSourcePublication = 0;

    bool current(MaterialTextureGlDiagnostic50EB& d) const noexcept {
        if (thread != std::this_thread::get_id())
            return fail(d, MaterialTextureGlError50EB::wrongThread, "texture owner requires creating thread");
        return (isCurrent && isCurrent(context)) ||
            fail(d, MaterialTextureGlError50EB::notCurrent, "caller texture context is not current");
    }
    bool hasNames() const noexcept {
        for (U t = 0; t < count; ++t) if (textures[t].name) return true;
        return false;
    }
    bool check(MaterialTextureGlDiagnostic50EB& d, const char* operation) const noexcept {
        const U error = gl.getError();
        if (!error) return true;
        if (!d.glError) d.glError = error;
        return fail(d, MaterialTextureGlError50EB::gl, operation);
    }
    bool upload(const assets::MaterialTextureAssets50EA& bank,
        const MaterialTextureSampler50EB* samplers, MaterialTextureGlDiagnostic50EB& d) noexcept {
        if (!current(d)) return false;
        // A valid empty/all-unresolved bank publishes without issuing GL calls.
        if (!count) { initialized = true; return true; }
        if (!check(d, "before cold texture upload")) return false;
        // Core profile excludes legacy pixel-transfer maps/scales/bias that
        // could otherwise alter the supplied bytes during texture upload.
        I major = 0, minor = 0, profile = 0, maximumDimension = 0;
        gl.getIntegerv(majorVersion, &major);
        gl.getIntegerv(minorVersion, &minor);
        gl.getIntegerv(contextProfileMask, &profile);
        if (!check(d, "query desktop core GL version/profile")) return false;
        if (major < 3 || (major == 3 && minor < 2) || !(profile & coreProfileBit))
            return fail(d, MaterialTextureGlError50EB::limits, "desktop core GL 3.2 or newer required");
        gl.getIntegerv(maxTextureSize, &maximumDimension);
        if (!check(d, "glGetIntegerv(GL_MAX_TEXTURE_SIZE)")) return false;
        if (maximumDimension <= 0)
            return fail(d, MaterialTextureGlError50EB::limits, "invalid GL maximum texture dimension");
        for (U t = 0; t < count; ++t) {
            d.texture = t;
            const auto& input = *bank.texture(t);
            if (input.texture.width > static_cast<U>(maximumDimension) ||
                input.texture.height > static_cast<U>(maximumDimension))
                return fail(d, MaterialTextureGlError50EB::limits, "texture exceeds GL maximum dimension");
            textures[t].identity = input.identity;
        }
        d.texture = assets::unresolvedMaterialTexture50EA;
        // Client pointers must never be interpreted as offsets into a PBO, and
        // caller row/skip state must never change the actual source byte range.
        gl.bindBuffer(pixelUnpackBuffer, 0);
        gl.pixelStorei(unpackAlignment, 1);
        gl.pixelStorei(unpackRowLength, 0);
        gl.pixelStorei(unpackSkipRows, 0);
        gl.pixelStorei(unpackSkipPixels, 0);
        if (!check(d, "prepare tightly packed client RGBA8 unpack")) return false;
        for (U t = 0; t < count; ++t) {
            d.texture = t;
            auto& owned = textures[t];
            const auto& input = bank.texture(t)->texture;
            const auto& sampler = samplers[t];
            // Store the returned name before checking errors so partial
            // generation is also covered by the ordinary cleanup path.
            gl.genTextures(1, &owned.name);
            if (!check(d, "glGenTextures")) return false;
            if (!owned.name)
                return fail(d, MaterialTextureGlError50EB::allocation, "zero generated texture name");
            gl.bindTexture(texture2D, owned.name);
            if (!check(d, "glBindTexture(GL_TEXTURE_2D)")) return false;
            gl.texParameteri(texture2D, minFilter, filter(sampler.minification));
            gl.texParameteri(texture2D, magFilter, filter(sampler.magnification));
            gl.texParameteri(texture2D, wrapS, wrap(sampler.s));
            gl.texParameteri(texture2D, wrapT, wrap(sampler.t));
            gl.texParameteri(texture2D, baseLevel, 0);
            gl.texParameteri(texture2D, maximumLevel, 0);
            if (!check(d, "apply explicit level-zero texture sampling")) return false;
            gl.texImage2D(texture2D, 0, static_cast<I>(rgba8), static_cast<I>(input.width),
                static_cast<I>(input.height), 0, rgba, unsignedByte, input.rgba.data());
            if (!check(d, "upload actual level-zero RGBA8 bytes")) return false;
            I width = 0, height = 0, format = 0;
            gl.getTexLevelParameteriv(texture2D, 0, textureWidth, &width);
            gl.getTexLevelParameteriv(texture2D, 0, textureHeight, &height);
            gl.getTexLevelParameteriv(texture2D, 0, textureInternalFormat, &format);
            if (!check(d, "verify level-zero RGBA8 allocation")) return false;
            if (width != static_cast<I>(input.width) || height != static_cast<I>(input.height) ||
                format != static_cast<I>(rgba8))
                return fail(d, MaterialTextureGlError50EB::storage, "GL texture size/format mismatch");
        }
        d.texture = assets::unresolvedMaterialTexture50EA;
        gl.bindTexture(texture2D, 0);
        if (!check(d, "clear cold-upload GL_TEXTURE_2D binding")) return false;
        initialized = true;
        return true;
    }
    bool cleanup(MaterialTextureGlDiagnostic50EB& d) noexcept {
        initialized = false;
        uploadedSourceBank = nullptr; uploadedSourcePublication = 0;
        if (thread != std::this_thread::get_id()) {
            d.cleanupIncomplete = true;
            return fail(d, MaterialTextureGlError50EB::wrongThread, "texture cleanup requires creating thread");
        }
        if (!hasNames()) { d.cleanupIncomplete = false; return true; }
        d.cleanupIncomplete = true;
        if (!current(d)) return false;
        // A bounded drain prevents old failures from being attributed to a
        // deletion. If it cannot settle, preserve every name for recovery.
        bool clean = false;
        for (unsigned n = 0; n < 32; ++n) {
            const U error = gl.getError();
            if (!error) { clean = true; break; }
            if (!d.cleanupGlError) d.cleanupGlError = error;
        }
        if (!clean) {
            d.errorStateUnclear = true;
            return fail(d, MaterialTextureGlError50EB::cleanup, "GL error state will not clear for texture cleanup");
        }
        for (U t = 0; t < count; ++t) {
            auto& name = textures[t].name;
            if (!name) continue;
            d.cleanupTexture = t;
            gl.deleteTextures(1, &name);
            const U error = gl.getError();
            if (error) {
                if (!d.cleanupGlError) d.cleanupGlError = error;
                // The context owner must not generate/reuse names before a
                // retry: this deletion's acceptance is not established.
                return fail(d, MaterialTextureGlError50EB::cleanup, "GL texture deletion failed");
            }
            name = 0;
        }
        d.cleanupTexture = assets::unresolvedMaterialTexture50EA;
        d.cleanupIncomplete = false;
        return !d.cleanupGlError ||
            fail(d, MaterialTextureGlError50EB::cleanup, "texture cleanup completed after GL errors");
    }
};

MaterialTextureGl50EB::MaterialTextureGl50EB() noexcept = default;
MaterialTextureGl50EB::~MaterialTextureGl50EB() noexcept {
    // Sparse publications require explicit context-bound retirement, including
    // failed staging prefixes. Destruction must never issue hidden GL calls.
    if (riderPool_) std::terminate();
    MaterialTextureGlDiagnostic50EB diagnostic{};
    close(diagnostic);
    if (diagnostic.cleanupIncomplete) std::terminate();
}
bool MaterialTextureGl50EB::create(const MaterialTextureGlRequest50EB& request,
    const assets::MaterialTextureAssets50EA& bank, MaterialTextureGlDiagnostic50EB& d) noexcept {
    d = {};
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())
        return fail(d,MaterialTextureGlError50EB::request,"Options reservation excludes native texture creation");
    if (storage_)
        return fail(d, MaterialTextureGlError50EB::alreadyOwned, "texture owner already owns a bank");
    if (lastPublication_ == std::numeric_limits<std::uint64_t>::max())
        return fail(d, MaterialTextureGlError50EB::limits, "texture publication token exhausted");
    std::size_t bytes = 0;
    if (!validate(request, bank, bytes, d)) return false;
    std::unique_ptr<Storage> next(new (std::nothrow) Storage);
    if (!next)
        return fail(d, MaterialTextureGlError50EB::allocation, "allocate bounded texture owner metadata");
    next->gl = request.gl;
    next->context = request.context;
    next->isCurrent = request.isCurrent;
    next->count = bank.textureCount();
    next->bytes = bytes;
    assets::OriginalNativeTextureUploadScope50LA transfer(
        request.originalUploadBinding,*this,request.context,request.isCurrent);
    if(!transfer.admitted())
        return fail(d,MaterialTextureGlError50EB::request,"actual display upload lifetime rejected");
    if (!next->upload(bank, request.samplers, d)) {
        next->cleanup(d);
        if (d.cleanupIncomplete) storage_ = std::move(next);
        return false;
    }
    transfer.completed();
    storage_ = std::move(next);
    ++lastPublication_;
    // Publish only after upload, validation and the actual transfer complete.
    storage_->uploadedSourceBank = &bank;
    storage_->uploadedSourcePublication = lastPublication_;
    return true;
}
bool MaterialTextureGl50EB::createRaStaticWorld(const MaterialTextureGlRequest50EB& request,
    const ra_cold_aggregate50LA::Owner& aggregate,MaterialTextureGlDiagnostic50EB& d) noexcept {
    d={};
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())
        return fail(d,MaterialTextureGlError50EB::request,"Options reservation excludes native world image publication");
    if(storage_||raStaticFire_||raStaticWorld_||aggregate.closed_||aggregate.terminal_||
       !aggregate.prefix_.staticWorldImages||!aggregate.world_)
        return fail(d,MaterialTextureGlError50EB::assets,"actual unpublished full RA world required");
    auto owner=aggregate.world_;
    try {
        if(owner->modelServiceTerminal_||!owner->coldImagesCurrent()||owner->imageFamilies_.size()!=44||
           owner->coldImages_.size()!=49||owner->pixelBank_.textureCount()!=49)
            return fail(d,MaterialTextureGlError50EB::assets,"current 44-family/49-image RA closure required");
        std::array<MaterialTextureSampler50EB,49> samplers{};
        std::array<bool,49> assigned{};
        const auto& table=owner->domain_->materials382B38();
        for(std::size_t fi=0;fi<owner->imageFamilies_.size();++fi){
            const auto& family=owner->imageFamilies_[fi];
            assets::OriginalStandardMaterialCollectorEntryView50LA leaf;
            assets::OriginalStandardMaterialPassView50LA pass;
            assets::OriginalStandardMaterialMapBindingView50LA map;
            assets::OriginalStandardMaterialDiagnostic50LA md;
            if(!table->coldCollectorPass(owner->imageCollector_,family.materialRow,0,leaf)||
               leaf.registration!=family.collector.registration||leaf.finalLeaf!=family.collector.finalLeaf||
               !leaf.material||!leaf.material->source()||!leaf.material->source()->originalBytes()||
               *leaf.material->source()->originalBytes()!=*owner->psms_.at(owner->normalized(family.materialMember))->originalBytes()||
               !leaf.material->pass4C(0,pass)||!pass.texture1A||!pass.mapCount20||*pass.mapCount20!=1||
               (*pass.texture1A)[0]!=family.textureToken||
               pass.constructionTemplate!=owner->context_->materialApplication()->templates().get(assets::OriginalUiTemplate50KG::defaultMt))
                return fail(d,MaterialTextureGlError50EB::assets,"actual RA canonical material/collector changed");
            assets::OriginalStandardTextureRowView50LA row;
            if(!owner->domain_->textures383400()->row(family.textureToken,row)||!row.references08||*row.references08<=0||
               !row.object04||!row.object04->constructed24DA30()||!row.object04->originalBytes()||
               *row.object04->originalBytes()!=family.sourceBytes||!row.object04->sourceMember()||
               owner->normalized(*row.object04->sourceMember())!=owner->normalized(family.sourceMember))
                return fail(d,MaterialTextureGlError50EB::assets,"actual RA canonical texture source changed");
            const bool crowd=family.animatedCurrentMapPending;
            if(crowd){
                // Real owned placement proof selects the current actual child;
                // image publication does not advance Application time or phase.
                bool found=false;
                for(std::size_t si=0;si<owner->surfaces_.size();++si)if(owner->surfaces_[si].materialRow==family.materialRow){
                    ra_static_world50LA::Owner::ModelCrowdMap selected;
                    ra_static_world50LA::Owner::Diagnostic wd;
                    if(!owner->currentCrowdSurfaceMap24B26C(si,selected,wd))
                        return fail(d,MaterialTextureGlError50EB::assets,"actual retained crowd map/leases rejected");
                    map=selected.binding;found=true;break;
                }
                if(!found||family.imageCount!=6)return fail(d,MaterialTextureGlError50EB::assets,"actual crowd placement/six-set missing");
            }else if(family.imageCount!=1||row.object04->animated102070()||pass.textureAnimation10||
                !table->prepareColdRepresentativeMap249610(owner->imageCollector_,family.materialRow,0,0,map,md)||
                map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||map.texture!=row.object04)
                return fail(d,MaterialTextureGlError50EB::assets,"actual ordinary RA final map changed");
            const auto& state=map.contextAfter249610;
            constexpr std::int32_t expected[6]={0,0,1,5,0,-75};
            for(std::size_t i=0;i<6;++i)if(state.sampler[i].value!=expected[i])
                return fail(d,MaterialTextureGlError50EB::sampler,"RA canonical sampler is outside exact base-linear subset");
            using F=assets::OriginalUiGsField50KG;
            const auto horizontal=state.fields[static_cast<std::size_t>(F::wrapModeHorizontal)].value;
            const auto vertical=state.fields[static_cast<std::size_t>(F::wrapModeVertical)].value;
            const bool flag=owner->normalized(family.materialMember)=="textures/largeflag.psm";
            if(horizontal!=0||vertical!=(flag?1:0))
                return fail(d,MaterialTextureGlError50EB::sampler,"RA canonical wrap differs from authenticated closure");
            // CPU bytes were lowered with these exact TEXA values. Revalidate
            // final canonical fields; sampler admission must not conceal alpha changes.
            if(!crowd)for(auto field:{F::sourceAlphaValueWhenAis0,F::sourceAlphaValueWhenAis1,F::methodOfExpandingTextureAlpha}){
                const auto i=static_cast<std::size_t>(field);
                if(state.fields[i].value!=family.context.fields[i].value)
                    return fail(d,MaterialTextureGlError50EB::assets,"RA final TEXA differs from retained CPU image");
            }
            if(family.firstImage>49||family.imageCount>49-family.firstImage)
                return fail(d,MaterialTextureGlError50EB::assets,"RA image range exceeds exact bank");
            for(std::size_t f=0;f<family.imageCount;++f){
                const auto i=family.firstImage+f;const auto& image=owner->coldImages_[i];
                const auto* canonical=crowd?row.object04->frame102050(static_cast<std::int32_t>(f)):row.object04;
                const auto* pixel=owner->pixelBank_.texture(static_cast<std::uint32_t>(i));
                if(assigned[i]||image.family!=fi||image.frame!=f||!canonical||canonical!=image.canonical||
                   !pixel||pixel->identity!=i+1||image.levels.empty()||image.mips.empty())
                    return fail(d,MaterialTextureGlError50EB::assets,"distinct RA frame/canonical/bank identity changed");
                assigned[i]=true;
                samplers[i]={MaterialTextureFilter50EB::linear,MaterialTextureFilter50EB::linear,
                    MaterialTextureWrap50EB::repeat,flag?MaterialTextureWrap50EB::clampToEdge:MaterialTextureWrap50EB::repeat};
            }
        }
        for(bool complete:assigned)if(!complete)return fail(d,MaterialTextureGlError50EB::assets,"RA image bank incomplete");
        if(!owner->publishImage(this))return fail(d,MaterialTextureGlError50EB::assets,"RA world publication already leased");
        raStaticWorld_=std::move(owner);
        auto input=request;input.samplers=samplers.data();input.samplerCount=49;
        const bool okay=create(input,raStaticWorld_->pixelBank_,d);
        if(!storage_){raStaticWorld_->retireImage(this);raStaticWorld_.reset();}
        return okay;
    }catch(...){return fail(d,MaterialTextureGlError50EB::allocation,"RA publication validation allocation failed");}
}

bool MaterialTextureGl50EB::createRaStaticFire(const MaterialTextureGlRequest50EB& request,
    std::shared_ptr<ra_static_fire_visual50LA::Owner> owner,MaterialTextureGlDiagnostic50EB& d) noexcept {
    d={};
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())
        return fail(d,MaterialTextureGlError50EB::request,"Options reservation excludes native fire image publication");
    if(storage_||raStaticFire_||raStaticWorld_||!owner||!owner->pixels())
        return fail(d,MaterialTextureGlError50EB::assets,"actual unpublished FireP0t bank required");
    assets::OriginalStandardMaterialMapBindingView50LA map;
    assets::OriginalStandardMaterialDiagnostic50LA md;
    if(!owner->currentMap(0,map,md))return fail(d,MaterialTextureGlError50EB::assets,"current FireP0t canonical map required");
    // Exact authored source has MXL0, MMAG1/MMIN5: both select linear base
    // texels. No generated mips or guessed LOD selection is introduced.
    const auto& state=map.contextAfter249610;
    constexpr std::int32_t sampler[6]={0,0,1,5,0,-75};
    for(std::size_t i=0;i<6;++i)if(state.sampler[i].value!=sampler[i])
        return fail(d,MaterialTextureGlError50EB::sampler,"unsupported FireP0t canonical sampler");
    using F=assets::OriginalUiGsField50KG;
    if(state.fields[static_cast<std::size_t>(F::wrapModeHorizontal)].value!=0||
       state.fields[static_cast<std::size_t>(F::wrapModeVertical)].value!=0)
        return fail(d,MaterialTextureGlError50EB::sampler,"unsupported FireP0t canonical wrap");
    if(!owner->publishImage(this))return fail(d,MaterialTextureGlError50EB::assets,"FireP0t source already published");
    raStaticFire_=std::move(owner);
    MaterialTextureSampler50EB selected{MaterialTextureFilter50EB::linear,MaterialTextureFilter50EB::linear,
        MaterialTextureWrap50EB::repeat,MaterialTextureWrap50EB::repeat};
    auto input=request;input.samplers=&selected;input.samplerCount=1;
    const bool okay=create(input,*raStaticFire_->pixels(),d);
    if(!storage_){raStaticFire_->retireImage(this);raStaticFire_.reset();}
    return okay;
}
bool MaterialTextureGl50EB::close(MaterialTextureGlDiagnostic50EB& d) noexcept {
    d = {};
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted()){
        if(!storage_)return true;
        d.cleanupIncomplete=true;
        return fail(d,MaterialTextureGlError50EB::cleanup,"Options reservation retains native texture ownership");
    }
    if(riderPool_){
        for(const auto* borrower:storage_->riderBorrowers)if(borrower){
            d.cleanupIncomplete=true;return fail(d,MaterialTextureGlError50EB::cleanup,"Rider renderer/collector still borrows publication");}
        const bool okay=riderPool_->dropPublication(*this,d);
        if(!d.cleanupIncomplete){storage_.reset();riderPool_.reset();}
        return okay;
    }
    if(raStaticWorld_&&raStaticWorld_->rendererPublication_){
        d.cleanupIncomplete=true;
        return fail(d,MaterialTextureGlError50EB::cleanup,"RA world renderer lease must retire before images");
    }
    if(raStaticFire_&&raStaticFire_->rendererPublication_){
        d.cleanupIncomplete=true;
        return fail(d,MaterialTextureGlError50EB::cleanup,"FireP0t renderer lease must retire before image publication");
    }
    if (!storage_) return true;
    const bool result = storage_->cleanup(d);
    if (!d.cleanupIncomplete) {
        storage_.reset();
        if(raStaticFire_){raStaticFire_->retireImage(this);raStaticFire_.reset();}
        if(raStaticWorld_){raStaticWorld_->retireImage(this);raStaticWorld_.reset();}
    }
    return result;
}
bool MaterialTextureGl50EB::initialized() const noexcept { return storage_ && storage_->initialized; }
bool MaterialTextureGl50EB::ownsResources() const noexcept { return bool(storage_); }
std::uint32_t MaterialTextureGl50EB::textureCount() const noexcept {
    return initialized() ? storage_->count : 0;
}
std::size_t MaterialTextureGl50EB::gpuByteCount() const noexcept {
    return initialized() ? storage_->bytes : 0;
}
std::uint64_t MaterialTextureGl50EB::publicationToken() const noexcept {
    return initialized() ? lastPublication_ : 0;
}
bool MaterialTextureGl50EB::matchesSourceBank(const assets::MaterialTextureAssets50EA& bank,
    std::uint64_t token) const noexcept {
    return token && token == publicationToken() && storage_->uploadedSourcePublication == token &&
        storage_->uploadedSourceBank == &bank && storage_->thread == std::this_thread::get_id();
}
bool MaterialTextureGl50EB::validBorrow(void* context, bool (*isCurrent)(void*) noexcept,
    std::uint64_t token) const noexcept {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return false;
    // The full-world image bank has no admitted renderer route yet. A future
    // typed friend renderer must hold its actual private lease before generic
    // borrowing can succeed. This also rejects stale CPU/canonical ownership.
    if(raStaticWorld_&&(!raStaticWorld_->rendererPublication_||
       raStaticWorld_->imagePublication_!=this||raStaticWorld_->modelServiceTerminal_||
       !raStaticWorld_->coldImagesCurrent()))return false;
    if(riderPool_&&riderPool_->faulted())return false;
    return token && token == publicationToken() && storage_->context == context &&
        storage_->isCurrent == isCurrent && storage_->thread == std::this_thread::get_id() &&
        isCurrent && isCurrent(context);
}
const MaterialTextureGlAsset50EB* MaterialTextureGl50EB::asset(U index) const noexcept {
    return initialized() && index < storage_->count && (!riderPool_||storage_->textures[index].name) ? &storage_->textures[index] : nullptr;
}
std::uint32_t MaterialTextureGl50EB::texture(U index) const noexcept {
    const auto* result = asset(index);
    return result ? result->name : 0;
}
namespace {
bool riderSamplerEqual(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b)noexcept {
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
}
bool riderFunctionsEqual(const MaterialTextureGlFunctions50EB& a,const MaterialTextureGlFunctions50EB& b)noexcept {
    return a.genTextures==b.genTextures&&a.deleteTextures==b.deleteTextures&&a.bindTexture==b.bindTexture&&
        a.texParameteri==b.texParameteri&&a.texImage2D==b.texImage2D&&a.getTexLevelParameteriv==b.getTexLevelParameteriv&&
        a.bindBuffer==b.bindBuffer&&a.pixelStorei==b.pixelStorei&&a.getIntegerv==b.getIntegerv&&a.getError==b.getError;
}
}
struct OriginalUiRiderResidentPool50KG::Storage {
    struct Image {MaterialTextureGlAsset50EB asset{};MaterialTextureSampler50EB sampler{};
        assets::OriginalUiImageProvenance50KG provenance{};std::size_t bytes=0;U references=0;bool ready=false;};
    const OriginalUiRiderSession50KG* const session;
    const std::shared_ptr<const assets::OriginalUiImageBank50KG> bank;
    const std::thread::id thread=std::this_thread::get_id();
    Image images[assets::maxMaterialTextureAssets50EA]{};
    MaterialTextureGlFunctions50EB gl{};void* context=nullptr;bool (*isCurrent)(void*)noexcept=nullptr;
    std::shared_ptr<const assets::OriginalNativeTextureUploadBinding50LA> uploadBinding;
    I saved[7]{};bool configured=false,restorePending=false,fault=false,closed=false;
    Storage(const OriginalUiRiderSession50KG& s,std::shared_ptr<const assets::OriginalUiImageBank50KG> b):session(&s),bank(std::move(b)){}
    std::size_t bytes()const noexcept {std::size_t n=0;for(const auto& i:images)if(i.asset.name)n+=i.bytes;return n;}
    U count()const noexcept {U n=0;for(const auto& i:images)if(i.asset.name)++n;return n;}
    bool current(MaterialTextureGlDiagnostic50EB& d)const noexcept {
        if(thread!=std::this_thread::get_id())return fail(d,MaterialTextureGlError50EB::wrongThread,"Rider resident pool creating thread required");
        return (isCurrent&&isCurrent(context))||fail(d,MaterialTextureGlError50EB::notCurrent,"Rider resident pool exact current context required");
    }
    bool check(MaterialTextureGlDiagnostic50EB& d,const char* operation)noexcept {
        const U e=gl.getError();if(!e)return true;if(!d.glError)d.glError=e;return fail(d,MaterialTextureGlError50EB::gl,operation);
    }
    bool capture(MaterialTextureGlDiagnostic50EB& d)noexcept {
        if(!check(d,"Rider staging initial GL error"))return false;
        constexpr U queries[7]={0x84E0,0x8069,0x88EF,unpackAlignment,unpackRowLength,unpackSkipRows,unpackSkipPixels};
        for(U n=0;n<7;++n)gl.getIntegerv(queries[n],&saved[n]);
        if(!check(d,"capture Rider staging binding/unpack state"))return false;
        if(saved[0]<0x84C0||saved[1]<0||saved[2]<0||
            (saved[3]!=1&&saved[3]!=2&&saved[3]!=4&&saved[3]!=8)||saved[4]<0||saved[5]<0||saved[6]<0)
            return fail(d,MaterialTextureGlError50EB::storage,"invalid captured Rider GL state");
        restorePending=true;return true;
    }
    bool restore(MaterialTextureGlDiagnostic50EB& d)noexcept {
        if(!restorePending)return true;
        if(!current(d)){fault=true;d.cleanupIncomplete=d.errorStateUnclear=true;return false;}
        // No staging operation changes the active unit. A changed unit means
        // callback/reentry contract failure; never restore a different unit.
        I active=0;gl.getIntegerv(0x84E0,&active);
        if(active!=saved[0]){fault=true;d.cleanupIncomplete=d.errorStateUnclear=true;
            return fail(d,MaterialTextureGlError50EB::cleanup,"Rider staging active texture unit unexpectedly changed");}
        gl.bindTexture(texture2D,static_cast<U>(saved[1]));
        gl.bindBuffer(pixelUnpackBuffer,static_cast<U>(saved[2]));
        gl.pixelStorei(unpackAlignment,saved[3]);gl.pixelStorei(unpackRowLength,saved[4]);
        gl.pixelStorei(unpackSkipRows,saved[5]);gl.pixelStorei(unpackSkipPixels,saved[6]);
        const bool commands=check(d,"restore Rider staging binding/unpack state");
        constexpr U queries[7]={0x84E0,0x8069,0x88EF,unpackAlignment,unpackRowLength,unpackSkipRows,unpackSkipPixels};
        bool same=true;for(U n=0;n<7;++n){I actual=0;gl.getIntegerv(queries[n],&actual);same=same&&actual==saved[n];}
        const bool observed=check(d,"verify restored Rider GL state");
        if(!commands||!observed||!same){fault=true;d.cleanupIncomplete=d.errorStateUnclear=true;
            return fail(d,MaterialTextureGlError50EB::cleanup,"Rider staging state restoration incomplete");}
        restorePending=false;return true;
    }
};
OriginalUiRiderResidentPool50KG::OriginalUiRiderResidentPool50KG(const OriginalUiRiderSession50KG& session,
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank):storage_(new Storage(session,std::move(bank))){}
OriginalUiRiderResidentPool50KG::~OriginalUiRiderResidentPool50KG()noexcept {
    if(storage_&&(storage_->count()||storage_->restorePending))std::terminate();
}
std::size_t OriginalUiRiderResidentPool50KG::liveGpuBytes()const noexcept{return storage_?storage_->bytes():0;}
U OriginalUiRiderResidentPool50KG::liveImageCount()const noexcept{return storage_?storage_->count():0;}
bool OriginalUiRiderResidentPool50KG::faulted()const noexcept{return !storage_||storage_->fault||storage_->restorePending;}
bool OriginalUiRiderResidentPool50KG::stage(const std::shared_ptr<OriginalUiRiderResidentPool50KG>& self,
    const OriginalUiRiderResidencyDemand50KG& demand,const MaterialTextureGlRequest50EB& request,
    MaterialTextureGl50EB& out,MaterialTextureGlDiagnostic50EB& d)noexcept {
    d={};auto& s=*storage_;constexpr std::size_t ceiling=32u*1024u*1024u;
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())
        return fail(d,MaterialTextureGlError50EB::request,"Options reservation excludes native Rider staging");
    if(self.get()!=this||s.closed||s.fault||s.restorePending||out.ownsResources()||out.riderPool_||
       demand.session_!=s.session||!demand.serial_||demand.indices_.empty()||demand.consumed_||demand.bank_!=s.bank||!s.bank||!s.bank->loaded()||
       !request.context||!request.isCurrent||!request.isCurrent(request.context)||!request.originalUploadBinding||
       request.maximumGpuBytes!=ceiling||s.thread!=std::this_thread::get_id())
        return fail(d,MaterialTextureGlError50EB::request,"current private Rider demand/pool/context and unchanged32MiB ceiling required");
    const auto images=s.bank->images();const auto* cpu=images?images->textures():nullptr;
    if(!cpu||!cpu->loaded()||!cpu->textureCount()||cpu->textureCount()>assets::maxMaterialTextureAssets50EA||
       request.samplerCount!=cpu->textureCount()||!request.samplers)
        return fail(d,MaterialTextureGlError50EB::assets,"complete immutable Rider CPU bank/sampler table required");
    const auto& g=request.gl;
    if(!g.genTextures||!g.deleteTextures||!g.bindTexture||!g.texParameteri||!g.texImage2D||!g.getTexLevelParameteriv||
       !g.bindBuffer||!g.pixelStorei||!g.getIntegerv||!g.getError)
        return fail(d,MaterialTextureGlError50EB::entryPoint,"complete Rider upload entry points required");
    if(s.configured&&(s.context!=request.context||s.isCurrent!=request.isCurrent||!riderFunctionsEqual(s.gl,g)||s.uploadBinding!=request.originalUploadBinding))
        return fail(d,MaterialTextureGlError50EB::request,"Rider pool context/functions/sealed upload owner changed");
    bool included[assets::maxMaterialTextureAssets50EA]{};
    std::size_t unionBytes=s.bytes(),publicationBytes=0;
    if(unionBytes>ceiling)return fail(d,MaterialTextureGlError50EB::budget,"existing Rider pool already exceeds unchanged ceiling");
    d.bankTextureCount=cpu->textureCount();d.bankRgbaBytes=cpu->rgbaByteCount();d.maximumGpuBytes=ceiling;
    d.poolLiveGpuBytes=s.bytes();d.poolLiveTextureCount=s.count();
    for(auto i:demand.indices_){
        d.texture=i;
        if(i>=cpu->textureCount()||included[i])return fail(d,MaterialTextureGlError50EB::assets,"duplicate/out-of-range private Rider closure index");
        included[i]=true;++d.residentTextureCount;const auto* source=cpu->texture(i);const auto* provenance=images->sourceProvenance(i);
        const auto& sampler=request.samplers[i];
        if(!source||!provenance||!provenance->memberIdentity||!provenance->pstBytes||
           !source->texture.width||!source->texture.height||source->texture.width>static_cast<U>(std::numeric_limits<I>::max())||
           source->texture.height>static_cast<U>(std::numeric_limits<I>::max())||
           std::uint64_t(source->texture.width)*source->texture.height*4!=source->texture.rgba.size()||
           !filter(sampler.minification)||!filter(sampler.magnification)||!wrap(sampler.s)||!wrap(sampler.t))
            return fail(d,MaterialTextureGlError50EB::assets,"Rider closure exact provenance/RGBA/sampler required");
        auto& resident=s.images[i];const auto bytes=source->texture.rgba.size();publicationBytes+=bytes;
        if(resident.asset.name){
            const auto& p=resident.provenance;
            if(!resident.ready||resident.asset.identity!=source->identity||resident.bytes!=bytes||!riderSamplerEqual(resident.sampler,sampler)||
               p.memberIdentity!=provenance->memberIdentity||p.celIdentity!=provenance->celIdentity||
               p.pstOffset!=provenance->pstOffset||p.pstBytes!=provenance->pstBytes)
                return fail(d,MaterialTextureGlError50EB::assets,"Rider shared image provenance or sampler changed");
        }else{
            if(bytes>ceiling-unionBytes)return fail(d,MaterialTextureGlError50EB::budget,"whole Rider pool including staged/queued/cleanup names exceeds32MiB");
            unionBytes+=bytes;
        }
        if(resident.references==std::numeric_limits<U>::max())return fail(d,MaterialTextureGlError50EB::allocation,"Rider publication reference overflow");
    }
    if(unionBytes>ceiling||out.lastPublication_==std::numeric_limits<std::uint64_t>::max())
        return fail(d,MaterialTextureGlError50EB::budget,"Rider pool bytes/publication generation exhausted");
    // Actual sealed frontend upload lifetime spans every staged attempt,
    // including partial failure and state restoration. Reuse-only stages are
    // successful admitted batches too, matching dense empty-batch accounting.
    assets::OriginalNativeTextureUploadScope50LA transfer(request.originalUploadBinding,out,request.context,request.isCurrent);
    if(!transfer.admitted())return fail(d,MaterialTextureGlError50EB::request,"actual frontend sparse upload lifetime rejected");
    try {out.storage_=std::make_unique<MaterialTextureGl50EB::Storage>();}
    catch(...){return fail(d,MaterialTextureGlError50EB::allocation,"allocate sparse Rider publication");}
    auto& publication=*out.storage_;publication.gl=g;publication.context=request.context;publication.isCurrent=request.isCurrent;
    publication.count=cpu->textureCount();publication.bytes=publicationBytes;publication.riderSession=s.session;publication.riderSerial=demand.serial_;
    out.riderPool_=self;
    if(!s.configured){s.gl=g;s.context=request.context;s.isCurrent=request.isCurrent;s.uploadBinding=request.originalUploadBinding;s.configured=true;}
    for(U i=0;i<publication.count;++i)if(included[i]){
        publication.riderIncluded[i]=true;publication.riderSamplers[i]=request.samplers[i];++s.images[i].references;
    }
    // Candidate remains cleanup-owned after any GL failure; outgoing objects,
    // names and CPU metadata are never replaced or decremented here.
    const bool captured=s.capture(d);
    bool okay=captured;
    if(okay){I major=0,minor=0,profile=0,maximum=0;
        g.getIntegerv(majorVersion,&major);g.getIntegerv(minorVersion,&minor);g.getIntegerv(contextProfileMask,&profile);
        g.getIntegerv(maxTextureSize,&maximum);
        okay=s.check(d,"validate sparse Rider GL context limits")&&major>=3&&(major!=3||minor>=2)&&(profile&coreProfileBit)&&maximum>0;
        if(!okay)fail(d,MaterialTextureGlError50EB::limits,"sparse Rider requires desktop core3.2 and positive texture limit");
        for(U i=0;okay&&i<publication.count;++i)if(included[i]){
            const auto& t=cpu->texture(i)->texture;
            if(t.width>static_cast<U>(maximum)||t.height>static_cast<U>(maximum))okay=fail(d,MaterialTextureGlError50EB::limits,"Rider demanded image exceeds GL dimension limit");
        }
    }
    if(okay){g.bindBuffer(pixelUnpackBuffer,0);g.pixelStorei(unpackAlignment,1);g.pixelStorei(unpackRowLength,0);
        g.pixelStorei(unpackSkipRows,0);g.pixelStorei(unpackSkipPixels,0);okay=s.check(d,"prepare sparse Rider exact RGBA unpack");}
    for(U i=0;okay&&i<publication.count;++i)if(included[i]){
        d.texture=i;auto& resident=s.images[i];const auto& source=*cpu->texture(i);const auto& sampler=request.samplers[i];
        if(!resident.asset.name){
            resident.bytes=source.texture.rgba.size();resident.asset.identity=source.identity;
            resident.sampler=sampler;resident.provenance=*images->sourceProvenance(i);
            g.genTextures(1,&resident.asset.name);okay=s.check(d,"generate sparse Rider image");
            if(okay&&!resident.asset.name)okay=fail(d,MaterialTextureGlError50EB::allocation,"zero sparse Rider image name");
            if(!okay)break;
            g.bindTexture(texture2D,resident.asset.name);
            okay=s.check(d,"bind sparse Rider candidate image");if(!okay)break;
            g.texParameteri(texture2D,minFilter,filter(sampler.minification));g.texParameteri(texture2D,magFilter,filter(sampler.magnification));
            g.texParameteri(texture2D,wrapS,wrap(sampler.s));g.texParameteri(texture2D,wrapT,wrap(sampler.t));
            g.texParameteri(texture2D,baseLevel,0);g.texParameteri(texture2D,maximumLevel,0);
            okay=s.check(d,"configure sparse Rider candidate sampler");if(!okay)break;
            g.texImage2D(texture2D,0,static_cast<I>(rgba8),static_cast<I>(source.texture.width),static_cast<I>(source.texture.height),0,rgba,unsignedByte,source.texture.rgba.data());
            okay=s.check(d,"upload sparse Rider original RGBA image");
            if(!okay)break;
            I width=0,height=0,format=0;g.getTexLevelParameteriv(texture2D,0,textureWidth,&width);
            g.getTexLevelParameteriv(texture2D,0,textureHeight,&height);g.getTexLevelParameteriv(texture2D,0,textureInternalFormat,&format);
            okay=s.check(d,"verify sparse Rider image storage")&&width==static_cast<I>(source.texture.width)&&height==static_cast<I>(source.texture.height)&&format==static_cast<I>(rgba8);
            if(!okay){fail(d,MaterialTextureGlError50EB::storage,"sparse Rider image storage mismatch");break;}
            resident.ready=true;
        }
        publication.textures[i]=resident.asset;
    }
    d.poolLiveGpuBytes=s.bytes();d.poolLiveTextureCount=s.count();
    const bool restored=s.restore(d);
    if(!okay||!restored){s.fault=true;d.cleanupIncomplete=true;return false;}
    transfer.completed();
    publication.initialized=true;++out.lastPublication_;d.texture=assets::unresolvedMaterialTexture50EA;return true;
}
bool OriginalUiRiderResidentPool50KG::collect(MaterialTextureGlDiagnostic50EB& d)noexcept {
    d={};auto& s=*storage_;if(!s.configured)return true;
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted()){
        d.cleanupIncomplete=true;
        return fail(d,MaterialTextureGlError50EB::cleanup,"Options reservation retains native Rider pool ownership");
    }
    if(!s.current(d)){d.cleanupIncomplete=true;return false;}
    if(!s.restore(d))return false;
    bool settled=false;for(unsigned i=0;i<32;++i){const U e=s.gl.getError();if(!e){settled=true;break;}if(!d.cleanupGlError)d.cleanupGlError=e;}
    if(!settled){s.fault=true;d.cleanupIncomplete=d.errorStateUnclear=true;return fail(d,MaterialTextureGlError50EB::cleanup,"Rider pool cleanup GL error state unclear");}
    for(U i=0;i<assets::maxMaterialTextureAssets50EA;++i){auto& image=s.images[i];
        if(!image.asset.name||image.references)continue;
        d.cleanupTexture=i;s.gl.deleteTextures(1,&image.asset.name);const U e=s.gl.getError();
        if(e){s.fault=true;d.cleanupIncomplete=true;d.cleanupGlError=e;
            return fail(d,MaterialTextureGlError50EB::cleanup,"Rider image deletion uncertain; pool reuse blocked");}
        image={};
    }
    for(const auto& image:s.images)if(image.asset.name&&!image.ready){s.fault=true;d.cleanupIncomplete=true;
        return fail(d,MaterialTextureGlError50EB::cleanup,"failed Rider image still belongs to an unretired candidate");}
    s.fault=false;d.cleanupIncomplete=false;return true;
}
bool OriginalUiRiderResidentPool50KG::close(MaterialTextureGlDiagnostic50EB& d)noexcept {
    d={};
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted()){
        d.cleanupIncomplete=true;
        return fail(d,MaterialTextureGlError50EB::cleanup,"Options reservation retains native Rider pool ownership");
    }
    for(const auto& i:storage_->images)if(i.references){d.cleanupIncomplete=true;
        return fail(d,MaterialTextureGlError50EB::cleanup,"Rider publications must close before pool");}
    if(!collect(d))return false;storage_->closed=true;return true;
}
bool OriginalUiRiderResidentPool50KG::dropPublication(MaterialTextureGl50EB& owner,MaterialTextureGlDiagnostic50EB& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted()){
        d.cleanupIncomplete=true;
        return fail(d,MaterialTextureGlError50EB::cleanup,"Options reservation retains native Rider publication ownership");
    }
    auto& s=*storage_;auto& p=*owner.storage_;
    if(!s.current(d)){d.cleanupIncomplete=true;return false;}
    if(!p.riderRefsReleased){
        for(U i=0;i<p.count;++i)if(p.riderIncluded[i]&&!s.images[i].references){d.cleanupIncomplete=true;s.fault=true;
            return fail(d,MaterialTextureGlError50EB::cleanup,"Rider pool publication reference mismatch");}
        p.initialized=false;for(U i=0;i<p.count;++i)if(p.riderIncluded[i])--s.images[i].references;p.riderRefsReleased=true;
    }
    return collect(d);
}
U MaterialTextureGl50EB::residentTextureCount()const noexcept {
    if(!initialized())return 0;U n=0;for(U i=0;i<storage_->count;++i)if(storage_->textures[i].name)++n;return n;
}
bool MaterialTextureGl50EB::riderSparse()const noexcept{return bool(riderPool_);}
const OriginalUiRiderSession50KG* MaterialTextureGl50EB::riderSession()const noexcept{return storage_?storage_->riderSession:nullptr;}
std::uint64_t MaterialTextureGl50EB::riderDemandSerial()const noexcept{return storage_?storage_->riderSerial:0;}
const assets::OriginalUiImageBank50KG* MaterialTextureGl50EB::riderBank()const noexcept {return riderPool_?riderPool_->storage_->bank.get():nullptr;}
bool MaterialTextureGl50EB::riderSamplerMatches(U index,const MaterialTextureSampler50EB& sampler)const noexcept {
    return riderPool_&&initialized()&&index<storage_->count&&storage_->riderIncluded[index]&&riderSamplerEqual(storage_->riderSamplers[index],sampler);
}
bool MaterialTextureGl50EB::acquireRiderBorrow(const void* borrower)const noexcept {
    if(!riderPool_)return true;
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return false;
    if(!borrower||!initialized()||riderPool_->faulted()||storage_->thread!=std::this_thread::get_id()||
       !storage_->isCurrent||!storage_->isCurrent(storage_->context))return false;
    for(const auto* existing:storage_->riderBorrowers)if(existing==borrower)return false;
    for(auto& slot:storage_->riderBorrowers)if(!slot){slot=borrower;return true;}return false;
}
bool MaterialTextureGl50EB::releaseRiderBorrow(const void* borrower)const noexcept {
    if(!riderPool_)return true;
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return false;
    if(!borrower||!storage_||storage_->thread!=std::this_thread::get_id()||!storage_->isCurrent||!storage_->isCurrent(storage_->context))return false;
    for(auto& slot:storage_->riderBorrowers)if(slot==borrower){slot=nullptr;return true;}return false;
}

}
