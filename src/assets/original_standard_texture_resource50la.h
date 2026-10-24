#pragma once
#include "original_standard_texture_table50la.h"
#include "original_standard_texture_lifetime50la.h"
#include <array>
#include <vector>
#include <thread>

namespace original_main_entry50KG { struct Application; }
namespace ra_crowd_image50LA { class Verifier; }
namespace ra_tga_source50LA { class Verifier; }
namespace rr::platform { class OriginalUiGarageSession50KG; class OriginalUiGarageActors50GN; class OriginalUiGarageWattoOwner50KG; }
namespace rr::game { class OriginalRacerAnimationCold50LA; class OriginalTimeStats50LA; class OriginalSpeedBoost50LA; class OriginalDamage50LA; class OriginalPositionStatsHud50LA; class OriginalPauseStorage50LA; }
namespace rr::assets {
class OriginalStandardMaterialContext50LA;
class OriginalStandardMaterialObject50LA;
class OriginalStandardTextureResolver50LA;
class OriginalStandardModelGeometry50LA;
class OriginalStandardCollisionType0Resource50LA;
class OriginalStandardArchiveProvider50LA;
class OriginalUiArchive50KG;
class OriginalUiIpumTextureCache50KG;
struct OriginalUiIpumProvenance50KG;
class OriginalStandardGarageIpumLease50KG;
class OriginalStandardIpumCompressedFrames50LA;
// Private fixed-source image leases/sets are minted only by the concrete
// native verifier. Resource ownership and GPU publication remain separate.
class OriginalStandardIpumImageLease50LA;
class OriginalStandardIpumImageSet50LA;

enum class OriginalStandardModelFileKind50LA : std::uint8_t { mcf,psg,colType0 };
// Explicit sealed-namespace absence is distinct from an unresolved native row.
enum class OriginalStandardCollisionLookup50LA : std::uint8_t { rejected,absent,present };
//Native stable row identity, not an original archive offset or retail index.
//Both parts are checked; a row from another application cannot be adopted.
struct OriginalStandardModelFileId50LA {
    const OriginalStandardTextureResolver50LA* resolver=nullptr;
    std::size_t row=static_cast<std::size_t>(-1);
};
struct OriginalStandardModelFileView50LA {
    OriginalStandardModelFileId50LA id{};
    OriginalStandardModelFileKind50LA kind=OriginalStandardModelFileKind50LA::mcf;
    const std::string* member=nullptr;
    const std::vector<std::uint8_t>* bytes=nullptr;
    const rr::game::OriginalRacerAnimationCold50LA* animation1536F0=nullptr;
    //Pins native backing only; source reference00 is retained separately by
    //the model's actual26E9F4 call, never by this lookup or shared_ptr copy.
    std::shared_ptr<OriginalStandardModelGeometry50LA> geometry240110;
    //132820 lookup pins backing only;12559C owns the separate signed ref60.
    std::shared_ptr<OriginalStandardCollisionType0Resource50LA> collision125014;
};

// Exact decompressed original PST/TGA/ATM already selected/authenticated by the actual
// application file provider. This is an input contract, not an authentication
// token: preparation never assigns an unrelated UI archive/catalog identity.
struct OriginalStandardTextureSourceInput50LA {
    std::string member;
    std::vector<std::uint8_t> bytes;
};
enum class OriginalStandardTextureSourceFormat50LA : std::uint8_t { pstIndexed,tgaBgr24,atmPstFrames,tgaBgra32,pstIpum };
enum class OriginalStandardTextureLevelStorage50LA : std::uint8_t {
    pstPacked4,pstTransfer8,tgaConverted1555,tgaGenerated1555,tgaConverted8888,tgaGenerated8888,ipumCompressed1555,garageIpumCompressed1555
};
struct OriginalStandardTextureLevelView50LA {
    //Actual final100 storage: kind0's retained packed4 source, or kind1's
    //owned24D6F0 transfer bytes after24F024. Never native RGBA coordinates.
    const std::uint8_t* packedIndices100=nullptr;
    std::size_t packedBytes=0,sourceHeaderOffset=0,sourceIndicesOffset=0;
    std::int32_t width104=-1,height110=-1;
    //Immutable authentic PST payload survives the source's transfer-only
    //replacement. A native image consumer must use these linear indices (or
    //its authenticated retained RGBA), never packedIndices100 for kind1.
    const std::uint8_t* linearSourceIndices=nullptr;
    std::size_t linearSourceBytes=0;
    std::uint32_t sourceMipOrdinal=0;
    //Only bits marked known are source claims.24E290..2B4 clear bits0/1;
    //kind0 never initializes bit2 here.24F044 sets kind1 bit2 after conversion.
    std::uint8_t flags30=0,flags30KnownMask=3;
    OriginalStandardTextureLevelStorage50LA storage=OriginalStandardTextureLevelStorage50LA::pstPacked4;
    //TGA owns scalar CPU1555 words at100, never palette indices. Low5 bits
    //are red, next5 green, next5 blue, high bit coverage. No GS expansion.
    const std::uint16_t* packed1555=nullptr;
    std::size_t packed1555Words=0;
    // Top-origin, unpremultiplied RGBA8888. TGA owns it; the distinct Garage tag
    // borrows sealed native IPUM cache pixels. This is CPU storage,
    // not a decoded image lease, renderer upload or fabricated PST indices.
    const std::uint8_t* rgba8888=nullptr;
    std::size_t rgba8888Bytes=0;
    // Kind2 encoded storage: adopted aligned RA span or retained static source span,
    // never palette indices or decoded1555. Image lease is a separate borrow.
    const std::uint8_t* compressed00=nullptr;
    std::size_t compressedBytes=0;
    const std::uint8_t* codec18=nullptr;
    std::size_t codecBytes1C=0;
    const OriginalStandardIpumImageLease50LA* hostImage=nullptr;
    // Distinct fixed-catalog static Garage bridge. rgba8888 is borrowed from
    // the retained native cache, not a RA animated lease or a second decode.
    const OriginalUiIpumProvenance50KG* garageIpumProvenance=nullptr;
};

// Native cold projection of resolved PST/TGA/ATM/MCF/PSG/COL entries in source3E1A70. Owns
// immutable original bytes and the actual non-owning1326A0 cache association.
// Full names compare case-insensitively, without slash conversion. Conflicting
// duplicate names reject; one shared resolver is required across the whole population
// lifetime. A missing supplied member is unresolved, never proof of absence.
// This does not open files or adopt a warm/foreign cache. TGA fallback also
// requires the same actual mounted provider's explicit PST/IPU absence;
// missing input in this corpus remains unresolved. IPU and nonzero ATM storage remain closed.
class OriginalStandardTextureResolver50LA final {
public:
    // Opaque one-bank capability; only the actual Application can issue/use it.
    // Destruction never unloads anything. A stale copy cannot replay retirement.
    class GarageBankLease50LA final {
        friend class OriginalStandardTextureResolver50LA;
        std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
        std::shared_ptr<const OriginalUiArchive50KG> archive_;
        std::shared_ptr<const void> managerIdentity_;
        const OriginalStandardTextureResolver50LA* resolver_=nullptr;
        std::size_t mount_=static_cast<std::size_t>(-1);
        std::uint64_t generation_=0;
        bool selected19E_=false; //Exact separate1D1D68 bank issuer, never a caller-selected archive.
        std::thread::id thread_{};
        // Historical native closure proof, NOT an active bank capability.
        // Minted only after actual12EB60; reissue invalidates it. It cannot
        // open a file, revive a row or stand in for a resource destructor.
        struct ClosedBackingReceipt {
            std::shared_ptr<OriginalStandardArchiveProvider50LA> provider;
            std::shared_ptr<const void> managerIdentity;
            const OriginalStandardTextureResolver50LA* resolver=nullptr;
            std::size_t mount=static_cast<std::size_t>(-1);
            std::uint64_t generation=0;
            bool selected19E=false;
            std::thread::id thread{};
        } closedBacking_;
    public:
        GarageBankLease50LA() noexcept=default;
        GarageBankLease50LA(const GarageBankLease50LA&)=delete;
        GarageBankLease50LA& operator=(const GarageBankLease50LA&)=delete;
    };
    OriginalStandardTextureResolver50LA() noexcept;
    ~OriginalStandardTextureResolver50LA();
    OriginalStandardTextureResolver50LA(const OriginalStandardTextureResolver50LA&)=delete;
    OriginalStandardTextureResolver50LA& operator=(const OriginalStandardTextureResolver50LA&)=delete;
    //Fresh form of appendOriginalSources. PST admits revision1, kind0/1,
    //1..5 complete ascending power-of-two16..1024 mips.
    // Owns exact indexed source and16/256 raw RGBA palette entries. This
    // preparation admits structure only; it does not expand pixels or upload.
    // Kind1 resource construction takes the proven cold initial382FC8=1
    // branch and owns its scalar24D6F0 result. Scoped-zero callers are outside
    // this route; callers cannot fabricate a bypass state through this API.
    bool prepareFresh(const std::vector<OriginalStandardTextureSourceInput50LA>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    //Append-only cold source admission on the SAME resolver. Exact repeat
    //bytes deduplicate case-insensitively; a conflicting name rejects before
    //mutation. Existing file IDs, texture ordinals and cache objects survive.
    //Public TGA subset stays64x64 type2 BGR24, bottom-origin, no ID/palette.
    //The private fixed-pin RA verifier separately admits its three exact TGAs.
    //ATM admits revision1/storage0,1..64 consecutive indexed PST frames.
    //Indexed frames use the existing splitter; kind2-only ATM framing is
    //retained for compressed staging, with canonical construction still closed.
    //ATM flags0 consumes all1..5 stored mips and loads only the final mip
    //into native slot0; ordinary flags2 PST retains all authored levels.
    //Source authentication remains the actual provider's responsibility.
    bool appendOriginalSources(const std::vector<OriginalStandardTextureSourceInput50LA>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    //One Application-owned complete native mounted namespace. Only this
    //provider can distinguish absence from unresolved source-vector input.
    //Must be sealed before lookup; later mounts use the same object and must
    //be sealed again. This is bounded native scope, not retail mount parity.
    bool bindProvider3E1A70(std::shared_ptr<const OriginalStandardArchiveProvider50LA>,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    //prepared means a real texture corpus was admitted. Model rows may already
    //exist before this; later prepareFresh preserves them and their IDs.
    bool prepared() const noexcept;
    // Cold CPU staging only, on this SAME resolver/provider. On allocation
    // failure out may retain a private copied-frame prefix for inspection and
    // cleanup. No canonical texture list/cache/ref or decoded pixel is published.
    bool stageIpumFlags0(std::shared_ptr<OriginalStandardTextureResolver50LA> actualSelf,
        std::size_t source,std::shared_ptr<const OriginalStandardIpumCompressedFrames50LA>& out,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    std::size_t sourceCount() const noexcept;
    const std::string* member(std::size_t) const noexcept;
    const std::vector<std::uint8_t>* originalBytes(std::size_t) const noexcept;
    //Like member()/originalBytes(), this points into a stable owned file row
    //and remains valid across later texture/model corpus appends.
    const OriginalStandardTextureSourceFormat50LA* sourceFormat(std::size_t) const noexcept;
    //Same actual3E1A70 owner and name/association rows used by PST above.
    //The application provider authenticates each exact MCF/PSG member. COL
    //requires a sealed mounted-provider name/bytes match on admission/lookup. This
    //only retains its positive resolution; absent input remains unresolved,
    //never a missing-file/fallback result. Rebinding identical bytes is safe.
    //ASCII case folds only; slash spelling is significant. Existing PST
    //sourceCount/member ordinals do not change when model rows are added.
    bool bindModelMember(const std::string&,OriginalStandardModelFileKind50LA,
        const std::vector<std::uint8_t>&,OriginalStandardModelFileId50LA&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool lookupModel132820(const std::string&,OriginalStandardModelFileKind50LA,
        OriginalStandardModelFileView50LA&,OriginalStandardTextureDiagnostic50LA&) const noexcept;
    // Actual11BA34 missing-COL branch may consume only this provider-backed
    // absence. Present requires an already authenticated canonical COL row.
    // Neither outcome constructs a query, shape, cache association or ref60.
    OriginalStandardCollisionLookup50LA lookupCollision132820(const std::string&,
        OriginalStandardModelFileView50LA&,OriginalStandardTextureDiagnostic50LA&) const noexcept;
    bool modelFile(OriginalStandardModelFileId50LA,OriginalStandardModelFileView50LA&,
        OriginalStandardTextureDiagnostic50LA&) const noexcept;
    //These are non-owning1326A0 associations, not Entity/resource retains.
    //Serial native callers publish the actual owner at its source store and
    //must clear it before owner disposal. A live association cannot be replaced.
    bool publishAnimation1326A0(OriginalStandardModelFileId50LA,
        const rr::game::OriginalRacerAnimationCold50LA*,OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool publishGeometry1326A0(OriginalStandardModelFileId50LA,
        const std::shared_ptr<OriginalStandardModelGeometry50LA>&,OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool publishCollision1326A0(OriginalStandardModelFileId50LA,
        const std::shared_ptr<OriginalStandardCollisionType0Resource50LA>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    void clearCollision1327D0(const OriginalStandardCollisionType0Resource50LA*) noexcept;
    void clearAnimation1327D0(const rr::game::OriginalRacerAnimationCold50LA*) noexcept;
    void clearGeometry1327D0(const OriginalStandardModelGeometry50LA*) noexcept;
private:
    friend class OriginalStandardModelGeometry50LA;
    friend class rr::platform::OriginalUiGarageActors50GN;
    friend class rr::platform::OriginalUiGarageWattoOwner50KG;
    // Only the two actual loaded Garage animation owners call this AFTER
    // motion/array/configuration teardown and their final Entity1413E0.
    //1327D0 or native pointer expiration alone never authorizes reclamation.
    void recordGarageFinalMcfRelease153F50(OriginalStandardModelFileId50LA,
        const rr::game::OriginalRacerAnimationCold50LA&) noexcept;
    // Records the actual resource's final26E598/26E0A0/1327D0. Only an
    // already-retired Garage PSG row can release backing here. Active bank
    // rows retain both bytes and the private historical final-release proof.
    // Does not evict active rows, reuse IDs, or retire any source reference.
    void reclaimRetiredPsgBacking26E5C8(const OriginalStandardModelGeometry50LA&) noexcept;
    // Native normal-manager completion only, after source bank/children,
    // transfer/renderer and audio/routing retirement. Requires BOTH genuine
    // final-resource and closed-bank receipts;12E5E0 itself still frees nothing.
    // Includes proved-final Garage MCF/PSG and closed-bank PST rows with exact
    // final-destructor or never-borrowed proof. No decoded cache eviction.
    bool reclaimGaragePsgBackingAfterManagerRetirement(GarageBankLease50LA&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    friend struct ::original_main_entry50KG::Application;
    friend class rr::platform::OriginalUiGarageSession50KG;
    // Native PST pointer custody, separate from the weak1326A0 association.
    // Every real PST resource publishes before exposing source pointers; its
    // actual destructor discharges only after levels/manager/cache teardown.
    bool beginPstSourceBorrow24E1B0(OriginalStandardTextureResource50LA&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    void finishPstSourceBorrow29C7A0(OriginalStandardTextureResource50LA&) noexcept;
    // Called only after actual selected1D1EA4 bank close and completed manager
    // population. Does not reclaim any row ever borrowed by a Resource.
    bool reclaimNeverBorrowedSelectedPstAfterBankClose(GarageBankLease50LA&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool preflightRetiredGaragePstBacking(std::size_t fileIndex,bool allowNeverBorrowed,
        bool& eligible,OriginalStandardTextureDiagnostic50LA&) const noexcept;
    // Native immutable ownership only, before actual source corpus admission.
    // Requires the exact sealed mounted provider and already-authenticated
    // fixed Garage cache. Does not decode, populate, upload or rebuild rows.
    bool bindGarageStaticIpumCache24E880(
        const std::shared_ptr<const OriginalUiIpumTextureCache50KG>&,
        const std::shared_ptr<const OriginalUiArchive50KG>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool validateGarageBankRows12EAA0(std::size_t,std::uint64_t,
        OriginalStandardTextureDiagnostic50LA&) const noexcept;
    bool issueGarageBank12E5E0(const std::shared_ptr<OriginalStandardArchiveProvider50LA>&,
        const std::shared_ptr<const OriginalUiArchive50KG>&,GarageBankLease50LA&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool issueGarageSelectedBank1D1D68(const std::shared_ptr<OriginalStandardArchiveProvider50LA>&,
        const std::shared_ptr<const OriginalUiArchive50KG>&,GarageBankLease50LA&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool issueGarageBank(const std::shared_ptr<OriginalStandardArchiveProvider50LA>&,
        const std::shared_ptr<const OriginalUiArchive50KG>&,GarageBankLease50LA&,bool selected19E,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool canRetireGarageBank12E5E0(const GarageBankLease50LA&,
        OriginalStandardTextureDiagnostic50LA&) const noexcept;
    bool retireGarageBank12E5E0(GarageBankLease50LA&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    // Actual resolver-owned native scratch backing. Explicit dormant suffixes,
    // never inferred from file/cache readiness or called by ordinary lookup.
    bool initializeScratch12CDF0(OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool restoreScratch132BD0(OriginalStandardTextureDiagnostic50LA&)noexcept;
    struct Scratch;
    std::unique_ptr<Scratch> scratch_;
    friend class ::ra_crowd_image50LA::Verifier;
    bool bindIpumImages(const std::shared_ptr<const OriginalStandardIpumImageSet50LA>&,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    friend class ::ra_tga_source50LA::Verifier;
    bool appendRaTgaVerified(const std::shared_ptr<const OriginalStandardArchiveProvider50LA>&,
        const std::vector<OriginalStandardTextureSourceInput50LA>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool appendSources(const std::vector<OriginalStandardTextureSourceInput50LA>&,
        bool verifiedRaTga,OriginalStandardTextureDiagnostic50LA&) noexcept;
    friend class OriginalStandardIpumCompressedFrames50LA;
    friend class OriginalStandardTextureTable50LA;
    friend class OriginalStandardTextureResource50LA;
    bool bind(const std::shared_ptr<const OriginalStandardMaterialContext50LA>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool resolve24DADC(const std::string&,std::size_t&,std::string& resolvedName,
        OriginalStandardTextureDiagnostic50LA&) const noexcept;
    bool validateProviderCache(OriginalStandardTextureDiagnostic50LA&) const noexcept;
    bool cached132820(std::size_t,std::shared_ptr<OriginalStandardTextureResource50LA>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool cache1326A0(std::size_t,const std::shared_ptr<OriginalStandardTextureResource50LA>&) noexcept;
    void clear1327D0(const OriginalStandardTextureResource50LA*) noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};

// Immutable request/provenance surface for a future typed image verifier.
// Actual source bytes/ranges and provider archive digest are retained; payload
// hashes must be checked by that verifier against its compiled catalog, never
// accepted from a caller. This is not an OriginalStandardTextureResource50LA.
class OriginalStandardIpumCompressedFrames50LA final {
    friend class OriginalStandardTextureResolver50LA;
    friend class OriginalStandardTextureResource50LA;
    bool canAdopt(std::size_t)const noexcept;
    bool adopt(std::size_t,const OriginalStandardTextureResource50LA*)const noexcept;
    void retire(std::size_t,const OriginalStandardTextureResource50LA*)const noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
    OriginalStandardIpumCompressedFrames50LA() noexcept;
public:
    struct FrameView {
        std::size_t frame=0,pstOffset=0,pstBytes=0,sourceMipOrdinal=0;
        std::size_t sourceHeaderOffset=0,sourcePayloadOffset=0,sourcePayloadBytes=0;
        std::uint32_t width=0,height=0;
        // Native owned compressed allocation, source level+00. Never RGBA,
        // palette indices or decoded1555. codec18 starts16 bytes later.
        const std::uint8_t* allocation00=nullptr;
        const std::uint8_t* codec18=nullptr;
        std::size_t codecBytes1C=0;
    };
    ~OriginalStandardIpumCompressedFrames50LA();
    OriginalStandardIpumCompressedFrames50LA(const OriginalStandardIpumCompressedFrames50LA&)=delete;
    OriginalStandardIpumCompressedFrames50LA& operator=(const OriginalStandardIpumCompressedFrames50LA&)=delete;
    // True only for complete compressed CPU staging. This owner cannot mint
    // image leases; canonical adoption additionally requires the verified set.
    bool compressedComplete()const noexcept;
    std::size_t copiedFrameCount()const noexcept;
    bool frame(std::size_t,FrameView&)const noexcept;
    const OriginalStandardTextureResolver50LA* resolverIdentity()const noexcept;
    std::size_t sourceIndex()const noexcept;
    const std::vector<std::uint8_t>* originalBytes()const noexcept;
    const std::string* sourceMember()const noexcept;
    const std::string* archiveSha256()const noexcept;
};

// Tagged bounded ordinary/ATM CPU resource allocation. ATM retains real
// ordinary children and exposes only common refs/demand/dimensions plus its
// count/checked frame selection; it never exposes ordinary format/list fields.
// Actual bounded CPU resource allocation, including independent1048F0 object
// references, exact dimensions, packed level ownership and native lifetime-list
// membership. Only its SAME dynamic383400 table can create/publish it. There is
// no arbitrary pointer, image identity, size, demand, or readiness setter.
// Largest native level is slot0, unlike the smallest-first source file order.
// Source storage/palette are immutable views into the pinned resolver. Kind1
// additionally owns actual native transfer backing, with the24F024 replacement
// and per-level30 bit2 represented, not a fabricated cleared382FC8 flag.
// Kind0 AC/E8=4; kind1 AC/E8=8 (29E790/29E6D0). These are CPU resource fields,
// TGA follows250EE0: owned descending1555 or8888 levels. Public specmap5
// retains64/32/16; privately verified RA sources extend to128/256.
// These are not guest GS storage/readiness. constructed24DA30 certifies this construction.
class OriginalStandardTextureResource50LA final {
    class AllocationKey {
        friend class OriginalStandardTextureResource50LA;
        //User-provided so C++17 aggregate initialization cannot bypass the
        //private constructor by passing an untyped {} to the public wrapper.
        AllocationKey() noexcept {}
    };
public:
    //The inaccessible construction key permits one allocation/control block
    //before construction, without a cleanup path that loses native refs.
    explicit OriginalStandardTextureResource50LA(AllocationKey) noexcept;
    ~OriginalStandardTextureResource50LA();
    OriginalStandardTextureResource50LA(const OriginalStandardTextureResource50LA&)=delete;
    OriginalStandardTextureResource50LA& operator=(const OriginalStandardTextureResource50LA&)=delete;
    bool constructed24DA30() const noexcept;
    //102070 aggregate owns real ordinary children; it never joins29C6E0 itself.
    bool animated102070() const noexcept;
    const std::int32_t* frameCount20() const noexcept;
    //Borrowed actual array24 child. No clamp, substitution, retain or cache alias.
    const OriginalStandardTextureResource50LA* frame102050(std::int32_t) const noexcept;
    //Exact PST extent in originalBytes(); children retain the enclosing ATM.
    bool pstExtent24E880(std::size_t& offset,std::size_t& bytes) const noexcept;
    const std::int64_t* references00() const noexcept;
    const float* demand10() const noexcept;
    const std::int32_t* sourceWidth14() const noexcept;
    const std::int32_t* sourceHeight18() const noexcept;
    const std::int32_t* levelCountB0() const noexcept;
    const std::string* name20() const noexcept;
    const std::string* sourceMember() const noexcept; //Canonical provider spelling.
    const std::vector<std::uint8_t>* originalBytes() const noexcept;
    const OriginalStandardTextureSourceFormat50LA* sourceFormat() const noexcept;
    const std::uint32_t* sourceKind() const noexcept; //PST enum0/1 or sealed static Garage2; null forTGA.
    const std::int32_t* formatAC() const noexcept;
    const std::int32_t* bitsPerPixelE8() const noexcept;
    // Only known-mask bits are source claims. The admitted24E1B0 cold
    // PST0/1 and TGA routes preserve its cleared F8 bit0; IPU kind2 and other
    // constructors are not admitted. Never infer this policy from a filename.
    const std::uint8_t* flagsF8() const noexcept;
    const std::uint8_t* flagsF8KnownMask() const noexcept;
    //PST-only cold initial data value actually used by this construction. No setter
    //and no claim that an unrelated scoped-zero caller has been recovered.
    const std::uint32_t* uploadConversion382FC8() const noexcept;
    const std::uint8_t* palette204() const noexcept; //Kind0 only,16 raw RGBA words.
    const std::uint8_t* palette208() const noexcept; //Kind1 only,256 raw RGBA words.
    bool level100(std::size_t,OriginalStandardTextureLevelView50LA&) const noexcept;
private:
    friend class OriginalStandardTextureTable50LA;
    friend class OriginalStandardTextureResolver50LA;
    friend class OriginalStandardMaterialObject50LA;
    friend class OriginalStandardTextureLifetime50LA;
    friend class rr::game::OriginalTimeStats50LA;
    friend class rr::game::OriginalSpeedBoost50LA;
    friend class rr::game::OriginalDamage50LA;
    friend class rr::game::OriginalPositionStatsHud50LA;
    friend class rr::game::OriginalPauseStorage50LA;
    static std::shared_ptr<OriginalStandardTextureResource50LA> allocate();
    bool construct24DA30(std::shared_ptr<OriginalStandardTextureResolver50LA>,std::size_t,
        const std::string& resolvedName,
        std::shared_ptr<const OriginalStandardMaterialContext50LA>,
        OriginalStandardTextureDiagnostic50LA&,
        std::size_t frame=static_cast<std::size_t>(-1)) noexcept;
    bool retain104960(OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool release104970() noexcept;
    // Direct original1F2530 floor, not projected-size conversion104A00.
    // Only the canonical material consumer can reach this operation.
    bool demandTournamentFallbackFloor10(std::uint32_t storePC,bool& written,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    // Actual1FDD60 resolved prototype, not a selected animation frame.
    bool demandGarageFloor1FDD60(std::uint32_t storePC,bool& written,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    // Direct204A84 floor on the resolved texture, not projected104A00 demand.
    bool demandSharedWarning204A84(bool& written,OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool demandTournamentMarker1F264C(bool& written,OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool demandTournamentOutline1F2530(bool& written,OriginalStandardTextureDiagnostic50LA&) noexcept;
    //18BA70 texture/font/triangle floors. The concrete owner must first
    //resolve and authenticate each exact canonical lease for its storePC.
    bool demandDamage18F470(bool& written,OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool demandSpeedBoost18BA70(std::uint32_t storePC,bool& written,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    //194E20 stats/13ingame/16ingame floors; same authenticated-lease duty.
    bool demandTimeStats194E20(std::uint32_t storePC,bool& written,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    //191AF0 stats/13ingame floors, reached through the retained HUD owner.
    bool demandPositionStats191AF0(std::uint32_t storePC,bool& written,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    // Exact1BAE00 stores; the Pause owner proves each original resource lease.
    bool demandPause1BAE00(std::uint32_t storePC,bool& written,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool demandSelection249580(bool& written,OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool demand104A00(float,bool& written,OriginalStandardTextureDiagnostic50LA&) noexcept;
    void resetDemand14D1A8() noexcept;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> construction_;
    OriginalStandardTextureLifetimeLink50LA lifetimeLink_;
    std::int64_t references00_=1; //1048F0 before29C6E0 registration.
    float demand10_=0.f; //1049C0; raised104A00, reset only by shared-list14D180.
    std::int32_t width14_=0,height18_=0,levelCountB0_=0;
    std::uint32_t sourceKind_=0;
    OriginalStandardTextureSourceFormat50LA sourceFormat_=OriginalStandardTextureSourceFormat50LA::pstIndexed;
    std::int32_t formatAC_=0,bitsPerPixelE8_=0;
    std::uint8_t flagsF8_=0,flagsF8KnownMask_=0;
    const std::uint32_t uploadConversion382FC8_=1; //Original data at00382FC8.
    std::size_t sourceIndex_=static_cast<std::size_t>(-1);
    const void* pstSourceBorrowIdentity_=nullptr; // Actual stable resolver File.
    std::uint64_t pstSourceBorrowPublication_=0;
    std::string name20_;
    std::array<OriginalStandardTextureLevelView50LA,5> levels_{};
    std::array<std::vector<std::uint8_t>,5> transferBacking_{};
    std::array<std::vector<std::uint16_t>,5> colorBacking_{};
    std::array<std::vector<std::uint8_t>,5> rgbaBacking_{};
    //29F770's temporary reader image survives through1326A0, then is freed
    //at29FEF0. A partial native resource owns it until terminal cleanup.
    std::vector<std::uint8_t> tgaReaderScratch_;
    // Aggregate retains all six verified leases throughout partial-child
    // construction and ascending retirement. Resolver association is weak.
    std::shared_ptr<const OriginalStandardIpumImageSet50LA> ipumImages_;
    std::shared_ptr<const OriginalStandardIpumImageLease50LA> ipumLease_;
    std::shared_ptr<const OriginalStandardGarageIpumLease50KG> garageIpumLease_;
    std::vector<std::shared_ptr<OriginalStandardTextureResource50LA>> frames24_;
    std::int32_t frameCount20_=0;
    std::size_t pstOffset_=0,pstBytes_=0;
    bool atmChild_=false,aggregate_=false;
    bool registered_=false,constructed_=false;
};
}
