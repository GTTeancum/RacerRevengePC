#pragma once
#include "original_ui_archive50kg.h"
#include "import_texture48e.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace rr::assets {
// The first bounded closure is the actual Rider19 character texture19c_1.
// More sources require a separately authenticated, compiled catalog change.
inline constexpr std::size_t originalUiIpumMipCount50KG = 5;
inline constexpr std::size_t originalUiIpumAdditionalCapacity50KG = 29;
enum class OriginalUiIpumColorPolicy50KG : std::uint8_t {
    unavailable,original50FcRgb5Texa255Aem1
};
struct OriginalUiIpumMipProvenance50KG {
    std::uint32_t width=0,height=0;
    std::size_t sourceHeaderOffset=0,sourcePayloadOffset=0,sourcePayloadBytes=0;
    std::string sourcePayloadSha256,yuv420pSha256,rgbaSha256,rrtexSha256;
    std::string retainedFilename;
};
struct OriginalUiIpumProvenance50KG {
    OriginalUiIpumColorPolicy50KG colorPolicyKind=OriginalUiIpumColorPolicy50KG::unavailable;
    std::string sourceMember,sourcePstSha256,sourceArchiveSha256;
    std::string converterSha256,generatorSha256,ffmpegVersion,ffmpegSha256;
    std::string ffmpegCommand,colorPolicy;
    // Codec fields above identify the pinned reference conversion. This flag
    // distinguishes fresh in-process decoding from supplied cached pixels.
    bool decodedInProcess=false;
    // Native host decoding of the original image-codec bytes. This is NOT
    // measured bit parity with a hardware inverse-DCT/framebuffer capture.
    bool hardwarePixelParityVerified=false;
    std::array<OriginalUiIpumMipProvenance50KG,originalUiIpumMipCount50KG> mips{};
    // Only [0,mipCount) is authenticated; zero means unavailable.
    std::size_t mipCount=0;
};
enum class OriginalUiIpumCacheError50KG : std::uint8_t {
    none,alreadyLoaded,cacheUnavailable,archive,source,imageCount,sourceLayout,
    encodedIdentity,imageIdentity,imageFormat,limit,cancelled,allocation,unexpectedFailure,codec
};
struct OriginalUiIpumCacheDiagnostic50KG {
    OriginalUiIpumCacheError50KG error=OriginalUiIpumCacheError50KG::none;
    std::size_t mip=0;
    std::string detail;
};

// Cold, load-once authentication of a compiled, source-derived image cache.
// Input files are untrusted bytes; callers cannot provide expected hashes,
// pixels through a callback, codec claims or alternate original identities.
// The generated catalog is unavailable until the root's authorized cold
// FFmpeg conversion writes real output hashes. A missing catalog fails closed.
// Original-disc methods use the native in-process decoder before the same
// admission. No FFmpeg process, guest code, PS2 device, filesystem or GL call
// runs here. No replacement pixels, palette or selected-mip collapse.
// Keep this owner only until consumers have copied its immutable result;
// the authenticated source archive is retained for all borrowed source views.
class OriginalUiIpumTextureCache50KG final {
public:
    OriginalUiIpumTextureCache50KG() noexcept;
    ~OriginalUiIpumTextureCache50KG();
    OriginalUiIpumTextureCache50KG(const OriginalUiIpumTextureCache50KG&)=delete;
    OriginalUiIpumTextureCache50KG& operator=(const OriginalUiIpumTextureCache50KG&)=delete;

    // True means a generated output catalog was compiled, never that cache
    // bytes, material, sampler, renderer or original hardware were verified.
    static bool catalogAvailable() noexcept;
    // Finite optional source catalog. Zero means unavailable, not an empty
    // authenticated cache. Indexes are compiled identities, never caller pins.
    // A source-only pin (including Gasgano07c_2) does not add an index: real
    // generated pixel/codec provenance must exist in the compiled catalog.
    static std::size_t additionalCatalogSourceCount() noexcept;
    static const char* additionalSourceMember(std::size_t source) noexcept;
    static std::size_t additionalMipCount(std::size_t source) noexcept;
    static const char* additionalRetainedMipFilename(std::size_t source,std::size_t mip) noexcept;
    // Independently transactional, load once per source; existing19 ownership
    // and any earlier additional source remain unchanged on failure.
    bool loadAdditional(std::size_t source,std::shared_ptr<const OriginalUiArchive50KG>,
        const std::vector<std::vector<std::uint8_t>>& retainedMipRrtex,
        OriginalUiIpumCacheDiagnostic50KG&,const DecodeHooks48C* =nullptr,
        const TextureLimits48E& =TextureLimits48E{}) noexcept;
    static const char* retainedMipFilename(std::size_t mip) noexcept;
    // Cold native decoding from the authenticated archive. Outputs pass the
    // same fixed source/pixel hash admission as the compatibility cache path.
    // No filesystem cache, subprocess, Python or external image pack is used.
    bool loadOriginal(std::shared_ptr<const OriginalUiArchive50KG>,
        OriginalUiIpumCacheDiagnostic50KG&,const DecodeHooks48C* =nullptr,
        const TextureLimits48E& =TextureLimits48E{}) noexcept;
    bool loadAdditionalOriginal(std::size_t source,std::shared_ptr<const OriginalUiArchive50KG>,
        OriginalUiIpumCacheDiagnostic50KG&,const DecodeHooks48C* =nullptr,
        const TextureLimits48E& =TextureLimits48E{}) noexcept;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,
        const std::vector<std::vector<std::uint8_t>>& retainedMipRrtex,
        OriginalUiIpumCacheDiagnostic50KG&,const DecodeHooks48C* =nullptr,
        const TextureLimits48E& =TextureLimits48E{}) noexcept;
    // True when at least one exact source has authenticated successfully.
    bool loaded() const noexcept;
    // Only the successfully authenticated fixed-catalog owner can return this
    // admitted format. The provenance prose is descriptive, not authority.
    OriginalUiIpumColorPolicy50KG colorPolicy() const noexcept;
    // Requires this very authenticated member view and its whole PST range;
    // a copied identity, same-name file or unrelated archive does not qualify.
    const Texture48E* textureFor(const OriginalUiMemberView50KG&,
        std::size_t pstOffset,std::size_t pstBytes) const noexcept;
    // Legacy accessor remains specifically19c_1; use provenanceFor for others.
    const OriginalUiIpumProvenance50KG* provenance() const noexcept;
    const OriginalUiIpumProvenance50KG* provenanceFor(const OriginalUiMemberView50KG&,
        std::size_t pstOffset,std::size_t pstBytes) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
    std::array<std::unique_ptr<Storage>,originalUiIpumAdditionalCapacity50KG> additional_{};
};
} // namespace rr::assets
