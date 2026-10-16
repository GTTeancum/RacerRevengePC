#include "../../../tests/native/ra_static_fire_visual50la.h"
#include "material_texture_gl50eb.h"
#include "../../../src/assets/original_native_texture_upload50la.h"
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
    if (request.samplerCount != bank.textureCount() ||
        (request.samplerCount && !request.samplers))
        return fail(d, MaterialTextureGlError50EB::request, "one explicit sampler per bank texture required");
    total = 0;
    for (U t = 0; t < bank.textureCount(); ++t) {
        d.texture = t;
        const auto* asset = bank.texture(t);
        if (!asset)
            return fail(d, MaterialTextureGlError50EB::assets, "missing immutable bank texture");
        const auto& texture = asset->texture;
        if (!texture.width || !texture.height ||
            texture.width > assets::max_texture_dimension || texture.height > assets::max_texture_dimension ||
            texture.width > static_cast<U>(std::numeric_limits<I>::max()) ||
            texture.height > static_cast<U>(std::numeric_limits<I>::max()))
            return fail(d, MaterialTextureGlError50EB::assets, "invalid RGBA8 dimensions");
        const std::uint64_t bytes = 4ull * texture.width * texture.height;
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
    U count = 0;
    std::size_t bytes = 0;
    bool initialized = false;

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
    MaterialTextureGlDiagnostic50EB diagnostic{};
    close(diagnostic);
    if (diagnostic.cleanupIncomplete) std::terminate();
}
bool MaterialTextureGl50EB::create(const MaterialTextureGlRequest50EB& request,
    const assets::MaterialTextureAssets50EA& bank, MaterialTextureGlDiagnostic50EB& d) noexcept {
    d = {};
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
    return true;
}
bool MaterialTextureGl50EB::createRaStaticWorld(const MaterialTextureGlRequest50EB& request,
    const ra_cold_aggregate50LA::Owner& aggregate,MaterialTextureGlDiagnostic50EB& d) noexcept {
    d={};
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
bool MaterialTextureGl50EB::validBorrow(void* context, bool (*isCurrent)(void*) noexcept,
    std::uint64_t token) const noexcept {
    // The full-world image bank has no admitted renderer route yet. A future
    // typed friend renderer must hold its actual private lease before generic
    // borrowing can succeed. This also rejects stale CPU/canonical ownership.
    if(raStaticWorld_&&(!raStaticWorld_->rendererPublication_||
       raStaticWorld_->imagePublication_!=this||raStaticWorld_->modelServiceTerminal_||
       !raStaticWorld_->coldImagesCurrent()))return false;
    return token && token == publicationToken() && storage_->context == context &&
        storage_->isCurrent == isCurrent && storage_->thread == std::this_thread::get_id() &&
        isCurrent && isCurrent(context);
}
const MaterialTextureGlAsset50EB* MaterialTextureGl50EB::asset(U index) const noexcept {
    return initialized() && index < storage_->count ? &storage_->textures[index] : nullptr;
}
std::uint32_t MaterialTextureGl50EB::texture(U index) const noexcept {
    const auto* result = asset(index);
    return result ? result->name : 0;
}
}
