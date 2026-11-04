#pragma once
namespace original_local_flare50LA {class Emission;}
#include "original_standard_texture_table50la.h"
#include "original_ui_archive50kg.h"
#include "original_standard_texture_lifetime50la.h"
#include <array>
#include <vector>
#include <thread>
#include <new>
#include <optional>

namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_sources50LA {class Owner;}
namespace sky50gw_fixture {class Owner;}
namespace original_world_sky50LA {struct NativeSkyPalette2E38C0;}

namespace original_main_entry50KG { struct Application; }
namespace ra_crowd_image50LA { class Verifier; }
namespace ra_tga_source50LA { class Verifier; }
namespace ra_cold_aggregate50LA { class Owner; }
namespace rr::platform { class OriginalLoadingLsiConfig50LA; class OriginalUiCourseManagerConfig50KG; class OriginalUiGarageSession50KG; class OriginalUiGarageActors50GN; class OriginalUiGarageWattoOwner50KG; }
namespace rr::game { class OriginalRacerModelSceneOwner50LA; class OriginalLoadingScreen40Mode0LA; class OriginalRacerAnimationCold50LA; class OriginalTimeStats50LA; class OriginalSpeedBoost50LA; class OriginalDamage50LA; class OriginalPositionStatsHud50LA; class OriginalPauseStorage50LA; class OriginalRaceF850LA; class OriginalResults10450LA; }
namespace rr::assets {
class OriginalPodSourceBank50LA;
class OriginalApplicationFonts50LA;
class OriginalLoadingFontSource50LA;
class OriginalLoadingPstFile50LA;
class OriginalLoadingRaFile50LA;
class OriginalMusicSourceFile50LA;
class OriginalStreamLibraryFile50LA;
class OriginalRaTrackSourceFile50LA;
class OriginalSourcePsgFile50LA;
class OriginalSourceCollisionFile50LA;
class OriginalSourceSkyFile50LA;
class OriginalSourcePsgMissingLookup50LA;
class OriginalLoadingTextureFc50LA;
class MaterialTextureAssets50EA;
struct MaterialTextureAsset50EA;
class OriginalResultsPrimitiveReferences50LA;
class OriginalStandardMaterialContext50LA;
class OriginalStandardMaterialObject50LA;
class OriginalStandardTextureResolver50LA;
class OriginalStandardModelGeometry50LA;
class OriginalStandardCollisionType0Resource50LA;
class OriginalStandardArchiveProvider50LA;
class NativeRaceMountLease50LA;
class OriginalUiArchive50KG;
class OriginalAppStartTemplateCache50KG;
class OriginalUiMaterialTemplates50KG;
struct OriginalUiMemberIdentity50KG;
struct OriginalUiMemberView50KG;
class OriginalUiIpumTextureCache50KG;
struct OriginalUiIpumProvenance50KG;
class OriginalStandardGarageIpumLease50KG;
class OriginalLoadingTextureSource50LA;
class OriginalStandardIpumCompressedFrames50LA;
// Private fixed-source image leases/sets are minted only by the concrete
// native verifier. Resource ownership and GPU publication remain separate.
class OriginalStandardIpumImageLease50LA;
class OriginalStandardIpumImageSet50LA;

enum class OriginalStandardModelFileKind50LA : std::uint8_t { mcf,psg,colType0,colType2 };
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
    pstPacked4,pstTransfer8,tgaConverted1555,tgaGenerated1555,tgaConverted8888,tgaGenerated8888,ipumCompressed1555,garageIpumCompressed1555,pstLoadingLinear8
};
struct OriginalStandardTextureLevelView50LA {
    //Actual final100 storage: kind0 packed4, ordinary kind1 owned24D6F0
    //transfer bytes, or distinct authenticated loading linear8 with conversion
    //bypassed at24F128. Never native RGBA coordinates.
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
    struct UiSelection;
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
        bool loadingFonts_=false; //Only private distinct LoadingFontsBankLease may receive this role.
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
    friend class ::ra_cold_aggregate50LA::Owner;
    // Native scoped1316A0 mapping: exact producer-issued RA banks only,
    // after real borrowers retire. Preserves process/serial UI mount backing.
    bool retireNativeRaceMounts1316A0(NativeRaceMountLease50LA&,
        OriginalStandardTextureDiagnostic50LA&)noexcept;
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
        OriginalStandardTextureDiagnostic50LA&,bool loadingFonts=false) noexcept;
    bool canRetireGarageBank12E5E0(const GarageBankLease50LA&,
        OriginalStandardTextureDiagnostic50LA&) const noexcept;
    bool retireGarageBank12E5E0(GarageBankLease50LA&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    friend class OriginalStandardMaterialDomain50LA;
    // Private one-shot native construction prerequisite, before any provider bind
    // or corpus. Not source13FB6C publication, registration, open/read or readiness.
    bool bootstrapFreshFileCapacity12CBA0(
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,
        OriginalStandardTextureDiagnostic50LA&)noexcept;
    // Actual Application-only, bounded first source17CAB8 call. No general
    // registration API, mount-derived history, activation or readiness token.
    bool registerAppStart17CAB8(std::shared_ptr<const OriginalUiArchive50KG>,
        OriginalStandardTextureDiagnostic50LA&)noexcept;
    std::shared_ptr<const OriginalUiArchive50KG> registeredAppStart17CAB8()const noexcept;
    //Private native chronology check, including failed registration attempts.
    //Not a source return, resource grant, or caller-set startup-ready flag.
    bool nativeBeforeAppStart17CAB8()const noexcept;
    bool joinAppStartCanonicalMount(OriginalStandardTextureDiagnostic50LA&)noexcept;
    // Actual first17CAD4/2985B0 Mat_Stbl consumer only. It opens/reads/closes
    // its real source member and supplies bytes to the same canonical table.
    bool readAppStartNames17CAD4(std::vector<std::uint8_t>&,
        OriginalStandardTextureDiagnostic50LA&,bool retainTerminator=false)noexcept;
    //Read-only custody of THIS actual DefaultMT open/getter/close stage.
    //No completion setter, next-template operation or readiness publication.
    bool sourceDefaultTemplate17CAE8Closed(
        const std::shared_ptr<const OriginalUiArchive50KG>&)const noexcept;
    //Read-only observation after the second genuine template close. This
    //does not publish native storage or advance the next startup operation.
    bool sourceScreenTemplate17CAFCClosed(
        const std::shared_ptr<const OriginalUiArchive50KG>&)const noexcept;
    friend class OriginalUiMaterialTemplates50KG;
    // Exactly DefaultMT then ScreenMT after the reached first Mat_Stbl read.
    bool beginAppStartTemplate24AA10(bool screen,OriginalAppStartTemplateCache50KG&,
        OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeAppStartTemplate1573B0(bool success,
        OriginalStandardTextureDiagnostic50LA&)noexcept;
    friend class OriginalParticleSourceConfig50LA;
    friend class ::rr::platform::OriginalUiCourseManagerConfig50KG;
    friend class ::rr::platform::OriginalLoadingLsiConfig50LA;
    friend class OriginalLoadingFontSource50LA;
    friend class OriginalLoadingRaFile50LA;
    friend class OriginalMusicSourceFile50LA;
    friend class OriginalStreamLibraryFile50LA;
    bool streamLibraryRootCurrent(const void*)const noexcept;
    friend class OriginalApplicationFonts50LA; // Exact private Loading PST population interval only.
    friend class OriginalPodSourceBank50LA;
    bool registerPodBank12CEA0(OriginalPodSourceBank50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closePodBankNative(OriginalPodSourceBank50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    friend class OriginalRaTrackSourceFile50LA;
    friend class OriginalSourcePsgFile50LA;
    friend class OriginalSourceCollisionFile50LA;
    friend class OriginalSourceSkyFile50LA;
    bool continueSourceCollisionFound132988(std::shared_ptr<OriginalSourcePsgMissingLookup50LA>,const void*,
        std::shared_ptr<OriginalSourceCollisionFile50LA>&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool sourceCollisionFileCurrentNative(const OriginalSourceCollisionFile50LA&,bool active)const noexcept;
    friend class OriginalSourcePsgMissingLookup50LA;
    bool captureSourcePsgMissing132820(std::shared_ptr<OriginalRaTrackSourceFile50LA>,const std::string&,
        std::shared_ptr<OriginalSourcePsgMissingLookup50LA>&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool sourcePsgMissingCurrentNative(const OriginalSourcePsgMissingLookup50LA&)const noexcept;
    bool closeSourcePsgMissingNative(OriginalSourcePsgMissingLookup50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool captureSourcePsg132820(std::shared_ptr<OriginalRaTrackSourceFile50LA>,const std::string&,OriginalStandardModelFileId50LA,std::shared_ptr<OriginalSourcePsgFile50LA>&,OriginalStandardTextureDiagnostic50LA&,
        std::shared_ptr<OriginalSourcePsgFile50LA> warmCreatorFile={},const OriginalStandardModelGeometry50LA* warmGeometry=nullptr,
        const rr::game::OriginalRacerModelSceneOwner50LA* warmCreator=nullptr)noexcept;
    bool continueSourcePsgFound132988(std::shared_ptr<OriginalSourcePsgMissingLookup50LA>,OriginalStandardModelFileId50LA,
        std::shared_ptr<OriginalSourcePsgFile50LA>&,OriginalStandardTextureDiagnostic50LA&,
        std::shared_ptr<OriginalSourcePsgFile50LA>,const OriginalStandardModelGeometry50LA*,
        const rr::game::OriginalRacerModelSceneOwner50LA*)noexcept;
    bool sourcePsgFoundPredecessorCurrentNative(const OriginalSourcePsgFile50LA&)const noexcept;
    bool withdrawSourcePsgFoundNative(OriginalSourcePsgFile50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool sourcePsgWarmCurrent(const OriginalSourcePsgFile50LA&,const OriginalStandardModelGeometry50LA*,
        const rr::game::OriginalRacerModelSceneOwner50LA*)const noexcept;
    bool sourcePsgFileCurrent(const OriginalSourcePsgFile50LA&)const noexcept;
    bool sourcePsgFrameCurrentNative(const OriginalSourcePsgFile50LA&)const noexcept;
    bool associateSourcePsgGeometry1326A0(OriginalSourcePsgFile50LA&,const std::shared_ptr<OriginalStandardModelGeometry50LA>&,
        const rr::game::OriginalRacerModelSceneOwner50LA*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool sourcePsgGeometryCurrent(const OriginalSourcePsgFile50LA&,const OriginalStandardModelGeometry50LA*,
        const rr::game::OriginalRacerModelSceneOwner50LA*)const noexcept;
    bool withdrawSourcePsgGeometryNative(OriginalSourcePsgFile50LA&,const OriginalStandardModelGeometry50LA*,
        const rr::game::OriginalRacerModelSceneOwner50LA*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool sourcePsgCursorNative(const OriginalSourcePsgFile50LA&,std::size_t&,OriginalStandardTextureDiagnostic50LA&)const noexcept;
    bool registerTrackBank12CEA0(OriginalRaTrackSourceFile50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool resizeTrackCache132E40(OriginalRaTrackSourceFile50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeTrackBankNative(OriginalRaTrackSourceFile50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    enum class SourceBankFamily : std::uint8_t {course,loadingFonts,loadingIcon};
    bool sourceBankFamilyCurrent(const void*,SourceBankFamily)const noexcept;
    bool registerSourceBank12CEA0(std::shared_ptr<const OriginalUiArchive50KG>,const OriginalStandardArchiveProvider50LA*,const void*,SourceBankFamily,bool&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    // Same real FileManager tables as APPSTART. Only the actual Course manager
    // may acquire this second-bank owner; native namespace lookup grants no ID.
    bool registerCourseBank12CEA0(std::shared_ptr<const OriginalUiArchive50KG>,const OriginalStandardArchiveProvider50LA*,const void*,bool&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool adoptCourseNativeMount(const void*,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool joinCourseBankCanonical(const void*,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool enterCourseArchiveOnly132A20(const void*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool borrowCourseConfigNative(const void*,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool openCourseConfig131230(const void*,const void*,const std::string&,std::int32_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool courseConfigFileCurrent(const void*,const void*,std::int32_t)const noexcept;
    bool readCourseConfig12FB30(const void*,const void*,std::int32_t,void*,std::size_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool readCourseConfig130560(const void*,const void*,std::int32_t,void*,std::size_t,std::size_t&,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool readCourseConfigLine12EE90(const void*,const void*,std::int32_t,std::array<char,1024>&,bool&,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool tellCourseConfig12F520(const void*,const void*,std::int32_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool seekCourseConfig12F640(const void*,const void*,std::int32_t,std::size_t,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeCourseConfig131040(const void*,const void*,std::int32_t,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool retireCourseBankNative(const void*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool activateCourseDevice12E8B0(std::int32_t,std::uint32_t,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool refillCourseWindow130340(std::int32_t,std::size_t,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool adoptSourceBankNativeMount(const void*,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool joinSourceBankCanonical(const void*,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool borrowSourceBankConfigNative(const void*,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool beginLoadingFontPstPopulation(std::shared_ptr<OriginalLoadingFontSource50LA>,const void*,OriginalUiMemberView50KG,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool endLoadingFontPstPopulation(const OriginalLoadingFontSource50LA&,const void*,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool captureLoadingPst132820(const std::string&,std::size_t,std::shared_ptr<OriginalLoadingPstFile50LA>&,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool loadingPstFileCurrent(const OriginalLoadingPstFile50LA&,bool requireOpen)const noexcept;
 bool bindLoadingPstResource(OriginalLoadingPstFile50LA&,OriginalStandardTextureResource50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool readLoadingPst12FB30(OriginalLoadingPstFile50LA&,void*,std::size_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool readLoadingPst12FF70(OriginalLoadingPstFile50LA&,void*,std::size_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool associateLoadingPst1326A0(OriginalLoadingPstFile50LA&,const std::shared_ptr<OriginalStandardTextureResource50LA>&,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool closeLoadingPst131040(OriginalLoadingPstFile50LA&,bool nativeAbort,OriginalStandardTextureDiagnostic50LA&)noexcept;
 bool canClearLoadingPst1327D0(const OriginalLoadingPstFile50LA&,const OriginalStandardTextureResource50LA&)const noexcept;
 bool clearLoadingPst1327D0(OriginalLoadingPstFile50LA&,const OriginalStandardTextureResource50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    // Sticky native projection history, NOT a fabricated FileManager device.
    // Actual loose source132820/12DCB0/cache lifetime must replace this path.
    bool unjoinedLoadingRaFileHistory_=false;
    bool admitLoadingRaCanonicalNative(std::shared_ptr<const OriginalLoadingTextureSource50LA>,std::size_t&,UiSelection&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool openExactSourceMember131380(const void*,std::int32_t,std::int32_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool beginLoadingRa132820(OriginalLoadingRaFile50LA&,std::size_t&,UiSelection&,std::shared_ptr<OriginalLoadingPstFile50LA>&,
        std::shared_ptr<OriginalStandardTextureResource50LA>&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool loadingRaFileCurrent(const OriginalLoadingPstFile50LA&,bool)const noexcept;
    bool loadingRaRetainedCurrent()const noexcept;
    bool canWithdrawLoadingRaAssociationNative(const OriginalLoadingPstFile50LA&,const OriginalStandardTextureResource50LA&)const noexcept;
    bool withdrawLoadingRaAssociationNative(OriginalLoadingPstFile50LA&,const OriginalStandardTextureResource50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool readLoadingRa130560(OriginalLoadingPstFile50LA&,void*,std::size_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeLoadingRa131040(OriginalLoadingPstFile50LA&,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeLoadingRaNative(OriginalLoadingRaFile50LA&,OriginalStandardTextureDiagnostic50LA&)noexcept;

    std::weak_ptr<OriginalLoadingFontSource50LA> loadingPstRoot_;
    const void* loadingPstFontEntry_=nullptr;
    OriginalUiMemberView50KG loadingPstExpected_{};
    // Original manager-issued POD registration only; decoded native mounts
    // never satisfy this same-root/encoded-owner/active-generation predicate.
    bool sourcePodBankCurrentNative(const void*)const noexcept;
    bool openSourceBankMember131380(const void*,const void*,std::int32_t,std::int32_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool openSourceBankMember131230(const void*,const void*,const std::string&,std::int32_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool sourceBankMemberCurrent(const void*,const void*,std::int32_t)const noexcept;
    bool readSourceBankMember12FB30(const void*,const void*,std::int32_t,void*,std::size_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool readSourceBankMember130560(const void*,const void*,std::int32_t,void*,std::size_t,std::size_t&,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool readSourceBankLine12EE90(const void*,const void*,std::int32_t,std::array<char,1024>&,bool&,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool tellSourceBankMember12F520(const void*,const void*,std::int32_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool seekSourceBankMember12F640(const void*,const void*,std::int32_t,std::size_t,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeSourceBankMember131040(const void*,const void*,std::int32_t,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool retireSourceBankNative(const void*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool adoptLoadingIconNativeMount(const void*,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool joinLoadingIconBankCanonical(const void*,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool openLoadingLsi131230(const void*,const void*,const std::string&,std::int32_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool loadingLsiFileCurrent(const void*,const void*,std::int32_t)const noexcept;
    bool readLoadingLsi12FB30(const void*,const void*,std::int32_t,void*,std::size_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool readLoadingLsiLine12EE90(const void*,const void*,std::int32_t,std::array<char,1024>&,bool&,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool tellLoadingLsi12F520(const void*,const void*,std::int32_t,std::size_t&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool seekLoadingLsi12F640(const void*,const void*,std::int32_t,std::size_t,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeLoadingLsi131040(const void*,const void*,std::int32_t,bool,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool retireLoadingIconBankNative(const void*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool registerLoadingIconBank12CEA0(std::shared_ptr<const OriginalUiArchive50KG>,const OriginalStandardArchiveProvider50LA*,const void*,bool&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    struct FreshFileCapacity;
    std::unique_ptr<FreshFileCapacity> freshFileCapacity_;
    // Actual resolver-owned native scratch backing. Initialization is reached
    // only by that fresh prefix; restoration remains dormant, never lookup-driven.
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
    bool preflightPopulationOwners(const std::shared_ptr<const OriginalStandardMaterialContext50LA>&,
        const std::shared_ptr<const OriginalStandardArchiveProvider50LA>&,
        OriginalStandardTextureDiagnostic50LA&)const noexcept;
    bool bind(const std::shared_ptr<const OriginalStandardMaterialContext50LA>&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    // Captured only from the actual positive resolver lookup. This private
    // move-only value is not an occurrence/material/population capability.
    struct UiSelection {
        UiSelection()noexcept=default;
        UiSelection(const UiSelection&)=delete;
        UiSelection& operator=(const UiSelection&)=delete;
        UiSelection(UiSelection&&)noexcept=default;
        UiSelection& operator=(UiSelection&&)noexcept=default;
        const OriginalStandardTextureResolver50LA* resolver=nullptr;
        const OriginalStandardArchiveProvider50LA* providerIdentity=nullptr;
        std::shared_ptr<const OriginalUiArchive50KG> archive;
        const OriginalUiMemberIdentity50KG* member=nullptr;
        const std::uint8_t* data=nullptr;
        std::size_t bytes=0,sourceIndex=static_cast<std::size_t>(-1);
        std::size_t mount=static_cast<std::size_t>(-1),ordinal=static_cast<std::size_t>(-1);
        std::uint64_t providerRevision=0,bankGeneration=0;
        // Separate concrete raw-RES flare capture. Never a UI archive/member
        // identity and never a retrospective resolver File.bank observation.
        struct NativeFlare {
            std::shared_ptr<const std::vector<std::uint8_t>> backing;
            const std::string* member=nullptr;
            const void* mountIdentity=nullptr;
            std::size_t offset=0;
        } nativeFlare;
        std::shared_ptr<const OriginalLoadingTextureSource50LA> loadingRa;
    };
    friend class rr::game::OriginalLoadingScreen40Mode0LA;
    class LoadingFontsBankLease final {
        friend class OriginalStandardTextureResolver50LA;
        friend class OriginalLoadingFontSource50LA;
        GarageBankLease50LA bank_;
    public:
        LoadingFontsBankLease()=default;
        LoadingFontsBankLease(const LoadingFontsBankLease&)=delete;
        LoadingFontsBankLease& operator=(const LoadingFontsBankLease&)=delete;
    };
    bool mountLoadingFontSource1CAAE4(OriginalLoadingFontSource50LA&,LoadingFontsBankLease&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool recordLoadingFontColdRegistry2823EC(OriginalLoadingFontSource50LA&,
        const std::shared_ptr<const void>&,OriginalUiMemberView50KG,OriginalUiMemberView50KG,
        const OriginalStandardTextureResource50LA*,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool retireLoadingFontSource1CAD94(OriginalLoadingFontSource50LA&,LoadingFontsBankLease&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool closeLoadingFontSourceNative(OriginalLoadingFontSource50LA&,LoadingFontsBankLease&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool issueLoadingFontsBank1CAAE4(const std::shared_ptr<OriginalStandardArchiveProvider50LA>& provider,
        const std::shared_ptr<const OriginalUiArchive50KG>& archive,LoadingFontsBankLease& lease,
        OriginalStandardTextureDiagnostic50LA& d)noexcept;
    bool retireLoadingFontsBank1CAD94(LoadingFontsBankLease& lease,
        OriginalStandardTextureDiagnostic50LA& d)noexcept {
        return lease.bank_.loadingFonts_&&retireGarageBank12E5E0(lease.bank_,d);
    }
    // Exact loose LD_RA.PST path only. This is not generic source admission.
    bool acquireLoadingRa24DA30(std::shared_ptr<const OriginalLoadingTextureSource50LA>,
        std::size_t&,UiSelection&,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool loadingRaSourceCurrent(const UiSelection&)const noexcept;
    bool nativeFlareSourceCurrent(const UiSelection&)const noexcept;
    bool resolve24DADC(const std::string&,std::size_t&,std::string& resolvedName,
        OriginalStandardTextureDiagnostic50LA&,UiSelection* capture=nullptr) const noexcept;
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
    friend class rr::game::OriginalLoadingScreen40Mode0LA;
    friend class OriginalResultsPrimitiveReferences50LA;
    friend class rr::game::OriginalTimeStats50LA;
    friend class rr::game::OriginalSpeedBoost50LA;
    friend class rr::game::OriginalDamage50LA;
    friend class rr::game::OriginalPositionStatsHud50LA;
    friend class rr::game::OriginalPauseStorage50LA;
    friend class rr::game::OriginalRaceF850LA;
    friend class rr::game::OriginalResults10450LA;
    friend class ::ra_pod_scene50LA::Owner;
    // Only the reached actual World can construct this private source capability.
    // No generic resolver/file-table admission or caller-created public view.
    struct CubSource1295B0 {
        std::shared_ptr<const ::ra_cold_sources50LA::Owner> sourcePin;
        std::shared_ptr<OriginalStandardArchiveProvider50LA> provider;
        const std::string* member=nullptr;
        const std::uint8_t* memberData=nullptr;
        std::size_t memberBytes=0,mount=0,ordinal=0;
        std::uint64_t providerRevision=0,bankGeneration=0;
        const std::uint8_t* authenticatedMemberData=nullptr;
        const ::sky50gw_fixture::Owner* imageOwner=nullptr;
        const MaterialTextureAssets50EA* bank=nullptr;
        const MaterialTextureAsset50EA* image=nullptr;
        ::original_world_sky50LA::NativeSkyPalette2E38C0* palette=nullptr;
        std::size_t tileOrdinal=0,tileOffset=0,tileBytes=0;
        std::uint32_t face=0,tile=0,descriptorWord0=0;
    };
    struct CubAlignedDelete {
        void operator()(std::uint8_t* p)const noexcept {::operator delete(p,std::align_val_t(64));}
    };
    struct CubMip24F7B8 {
        std::unique_ptr<std::uint8_t,CubAlignedDelete> allocation;
        std::size_t allocationBytes=0,sourceLengthOffset=0,sourcePayloadOffset=0;
        const std::uint8_t* pointer100=nullptr;
        const std::uint8_t* codec118=nullptr;
        std::int32_t width104=-1,blocks108=-1,logWidth10C=-1,height110=-1,logHeight114=-1;
        std::int32_t bytes11C=-1,height128=-1,width12C=-1;
        std::uint16_t width124=0xffff;
        std::uint8_t flags130=0,flags130KnownMask=3;
        bool payloadRead=false;
    };
    struct CubBuffer257750 {
        // Source object0x20 bytes; native pointers are wider, explicitly adapted.
        std::unique_ptr<std::uint8_t,CubAlignedDelete> backing;
        std::uint8_t* first00=nullptr;
        std::uint8_t* current04=nullptr;
        std::uint32_t capacity08=0,field0C=0,field10=0,field14=0,field18=0,field1C=0;
    };
    struct CubFacet1295B0 {
        CubSource1295B0 source;
        std::array<CubMip24F7B8,5> mips;
        std::unique_ptr<CubBuffer257750> bufferFCOwner;
        CubBuffer257750* bufferFC=nullptr; //257740 null; published only after2592D0.
        std::array<std::uint32_t,5> offsetWords{};
        std::size_t orderedReadBytes=0,compressedAllocationBytes=0,finalCursor=0;
        std::uint32_t lastPC=0x24e1b0;
        std::optional<std::int32_t> fieldA8;
        bool constructed24E1B0=false,loadAttempted=false,loadActive=false;
        bool loadComplete24F2C0=false,finished2516F0=false;
    };
    std::unique_ptr<CubFacet1295B0> cub_;
    std::shared_ptr<OriginalLoadingPstFile50LA> loadingPstFile_;
    struct LoadingPstNative50LA;
    std::unique_ptr<LoadingPstNative50LA> loadingPstNative_;
    std::unique_ptr<OriginalLoadingTextureFc50LA> loadingPstFc_;
    bool admitLoadingPstNative(OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool initializeLoadingPstFields24E1E0(OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool constructLoadingPst24E880(OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool preflightLoadingPstNative()const noexcept;
    bool closeLoadingPstNative(OriginalStandardTextureDiagnostic50LA&)noexcept;


    bool constructCub24E1B0(const CubSource1295B0&,
        std::shared_ptr<const OriginalStandardMaterialContext50LA>,OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool loadCub24F2C0(std::size_t&,const std::array<std::uint32_t,5>&,
        OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool finishCub2516F0(std::uint32_t,std::uint32_t,std::uint32_t,
        OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool cubSourceCurrent1295B0()const noexcept;
    bool canRetireCub24E370()const noexcept;
    bool withdrawCubPaletteBorrow()noexcept;
    static std::shared_ptr<OriginalStandardTextureResource50LA> allocate();
    bool construct24DA30(std::shared_ptr<OriginalStandardTextureResolver50LA>,std::size_t,
        const std::string& resolvedName,
        std::shared_ptr<const OriginalStandardMaterialContext50LA>,
        OriginalStandardTextureDiagnostic50LA&,
        std::size_t frame=static_cast<std::size_t>(-1),
        OriginalStandardTextureResolver50LA::UiSelection* selection=nullptr) noexcept;
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
    // Exact199350 three stores. The actual F8 owner proves the retained
    // font / first-pass icon resource before requesting this CPU demand floor.
    bool demandRaceF8199350(std::uint32_t storePC,bool& written,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    // Exact Results slot40 per-occurrence floor, not a preload or image selection.
    bool demandResults1C15B0(std::uint32_t storePC,bool& written,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool demandSelection249580(bool& written,OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool demand104A00(float,bool& written,OriginalStandardTextureDiagnostic50LA&) noexcept;
    void resetDemand14D1A8() noexcept;
    // Historical first selection belongs to this resource, not its mutable
    // resolver File.bank association. Cached aliases never replace it.
    OriginalStandardTextureResolver50LA::UiSelection firstUiSource_;
    bool firstUiSourceCurrent()const noexcept;
    // Results' actual reference ledger fences its immutable creator thread.
    // Called on the live texture-table object, not on a guessed ATM child.
    // This only authenticates pixels; no material policy/service/GPU authority.
    friend class ::original_local_flare50LA::Emission;
    bool borrowLocalFlarePstImage50LA(std::shared_ptr<const std::vector<std::uint8_t>>&,
        const std::string*&,const std::uint8_t*&,std::size_t&)const noexcept;
    bool borrowResultsUiPstImage50LA(const OriginalStandardTextureResource50LA& selected,
        std::shared_ptr<const OriginalUiArchive50KG>&,OriginalUiMemberView50KG&,
        std::size_t& offset,std::size_t& bytes)const noexcept;
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
