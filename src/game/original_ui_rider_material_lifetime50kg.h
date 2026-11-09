#pragma once
#include "../assets/original_ui_rider_character_assets50kg.h"
#include "../assets/original_ui_rider_pod_assets50kg.h"
#include "../assets/original_ui_rider_binder_assets50kg.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

namespace rr::assets { class OriginalUiGarageSceneAssets50KG; class OriginalUiGarageWattoAssets50KG; }
namespace rr::game {
enum class OriginalUiRiderMaterialLifetimeError50KG : std::uint8_t {
    none,input,archive,table,row,footprint,alreadyBound,notBound,staleSequence,
    staleOwner,rowCapacity,ownerCapacity,journalCapacity,overflow,invariant,allocation,
    populationActive,populationInactive,populationOrder,populationIncomplete,aborted
};
struct OriginalUiRiderMaterialLifetimeDiagnostic50KG {
    OriginalUiRiderMaterialLifetimeError50KG error=OriginalUiRiderMaterialLifetimeError50KG::none;
    std::uint32_t row=0,sourcePC=0;
};
// FIRST matching298C80 row of authenticated APPSTART/Mat_Stbl.txt. A pass,
// texture, native pointer/refcount or bank-local image index is never a row key.
class OriginalUiRiderMaterialRow50KG final {
public:
    bool prepared()const noexcept{return table_!=nullptr;}
    const assets::OriginalUiMemberIdentity50KG* tableIdentity()const noexcept{return table_;}
    std::uint32_t rank()const noexcept{return rank_;}
    std::uint32_t byteOffset()const noexcept{return offset_;}
    bool same(const OriginalUiRiderMaterialRow50KG&)const noexcept;
    static bool prepare(const assets::OriginalUiArchive50KG&,const std::string& registeredName,
        OriginalUiRiderMaterialRow50KG&,OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
private:
    const assets::OriginalUiMemberIdentity50KG* table_=nullptr;
    std::uint32_t rank_=0,offset_=0;
};
enum class OriginalUiRiderMaterialOwnerKind50KG : std::uint8_t {absent,podGraph,characterGraph,binderHelper,garageStaticModel,garageActorModel,garageSelectedModel,garageWattoModel};
constexpr std::size_t originalUiRiderMaterialRows50KG=256;
constexpr std::size_t originalUiRiderMaterialOwners50KG=128;
constexpr std::size_t originalUiRiderMaterialEvents50KG=4096;
// Immutable FOOTPRINT evidence, not a live owner. The original mesh acquires
// every material slot at26E7D8/26EB90, including repeated names. Those slot
// multiplicities are retained, but are NOT the original total PSM refcount:
// geometry/wrappers add references too. One acquired token projects the
// nonzero presence of ONE complete, proven-contained runtime ownership domain.
// Its final release is legal only after all that domain's references are gone.
class OriginalUiRiderMaterialFootprint50KG final {
public:
    bool prepared()const noexcept{return kind_!=OriginalUiRiderMaterialOwnerKind50KG::absent;}
    OriginalUiRiderMaterialOwnerKind50KG kind()const noexcept{return kind_;}
    std::uint16_t widget()const noexcept{return widget_;}
    // Original one-based SCN/TOY source ordinal; never a fabricated widget identity.
    std::uint16_t garageOrdinal()const noexcept{return garageOrdinal_;}
    const assets::OriginalUiMemberIdentity50KG* source()const noexcept{return source_;}
    std::size_t rowCount()const noexcept{return rows_;}
    std::uint32_t materialSlotCount()const noexcept{return slots_;}
    const OriginalUiRiderMaterialRow50KG* row(std::size_t i)const noexcept{return i<rows_?&keys_[i]:nullptr;}
    std::uint32_t slotOccurrences(std::size_t i)const noexcept{return i<rows_?occurrences_[i]:0;}
    static bool pod(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiRiderPodAssets50KG&,OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool tournamentPod(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiRiderPodAssets50KG&,OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool tournamentBinder(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiRiderBinderAssets50KG&,OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    // Authenticated UIUnlock domains (pod 9..23, pilot 24..38, binder).
    static bool unlockPod(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiRiderPodAssets50KG&,OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool unlockCharacter(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiRiderCharacterAssets50KG&,OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool unlockBinder(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiRiderBinderAssets50KG&,OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool character(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiRiderCharacterAssets50KG&,OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool binder(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiRiderBinderAssets50KG&,OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    // Authenticated Garage domains. These record immutable slot footprints,
    // not graph construction, live refs, native rows, observer or draw readiness.
    static bool garageStatic(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiGarageSceneAssets50KG&,std::size_t ordinal,
        OriginalUiRiderMaterialFootprint50KG&,OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool garageActor(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiGarageSceneAssets50KG&,std::size_t ordinal,
        OriginalUiRiderMaterialFootprint50KG&,OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool garageSelected(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiGarageSceneAssets50KG&,
        OriginalUiRiderMaterialFootprint50KG&,OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    static bool garageWatto(const assets::OriginalUiArchive50KG& appStart,
        const assets::OriginalUiGarageWattoAssets50KG&,
        OriginalUiRiderMaterialFootprint50KG&,OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;

private:
    bool model(const assets::OriginalUiArchive50KG&,const assets::Scene48D&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    std::array<OriginalUiRiderMaterialRow50KG,originalUiRiderMaterialRows50KG> keys_{};
    std::array<std::uint32_t,originalUiRiderMaterialRows50KG> occurrences_{};
    const assets::OriginalUiMemberIdentity50KG* source_=nullptr;
    std::size_t rows_=0;std::uint32_t slots_=0;std::uint16_t widget_=0,garageOrdinal_=0;
    OriginalUiRiderMaterialOwnerKind50KG kind_=OriginalUiRiderMaterialOwnerKind50KG::absent;
};
class OriginalUiRiderMaterialLifetime50KG;
class OriginalUiRiderBackend50KG;
class OriginalUiRiderMaterialOwnerToken50KG final {
public:
    bool issued()const noexcept{return ledger_!=nullptr;}
    std::uint64_t serial()const noexcept{return serial_;}
private:
    friend class OriginalUiRiderMaterialLifetime50KG;
    const OriginalUiRiderMaterialLifetime50KG* ledger_=nullptr;
    std::uint64_t serial_=0;std::uint16_t slot_=0;
};
// Explicit proof of one recorded positive-to-zero LIVE epoch retirement.
// Consumers must validate against the still-retained journal before retiring
// history, then acknowledge it. Default or previously acknowledged tokens fail.
class OriginalUiRiderMaterialRetirement50KG final {
public:
    bool issued()const noexcept{return ledger_!=nullptr;}
    const OriginalUiRiderMaterialRow50KG& row()const noexcept{return row_;}
    std::uint64_t epoch()const noexcept{return epoch_;}
    std::uint64_t sequence()const noexcept{return sequence_;}
private:
    friend class OriginalUiRiderMaterialLifetime50KG;
    const OriginalUiRiderMaterialLifetime50KG* ledger_=nullptr;
    OriginalUiRiderMaterialRow50KG row_{};
    std::uint64_t epoch_=0,sequence_=0;
};
enum class OriginalUiRiderMaterialEventKind50KG : std::uint8_t {
    ownerAcquired,ownerReleased,rowPopulated,rowRetired,populationBoundary,groupRebuilt
};
struct OriginalUiRiderMaterialEvent50KG {
    OriginalUiRiderMaterialEventKind50KG kind=OriginalUiRiderMaterialEventKind50KG::ownerAcquired;
    std::uint64_t sequence=0,ownerSerial=0,epoch=0;
    OriginalUiRiderMaterialRow50KG row{};
    // Count of actual projected OWNER DOMAINS, never original numeric refs.
    std::uint32_t ownerPresenceAfter=0,sourceSlotOccurrences=0;
    OriginalUiRiderMaterialRetirement50KG retirement{};
};
struct OriginalUiRiderMaterialMutation50KG {
    std::uint64_t firstSequence=0,lastSequence=0;
    std::uint32_t eventCount=0;
};
struct OriginalUiRiderMaterialRowState50KG {
    OriginalUiRiderMaterialRow50KG row{};
    std::uint32_t ownerPresence=0;
    // Zero means no constructed PSM, including positive pending references.
    std::uint64_t epoch=0;
};
// Dynamic owner-domain ledger. Cold descriptors/footprints never acquire a
// token. Callers must prove domain containment, source callback order,14D370
// construction and real borrower/renderer drain. Only the backend may issue
// constructed-row population and successful14D440 group rebuild records.
// The ledger must outlive EVERY token copy and consumer, including released
// tokens. It cannot be reset/moved. Destroying/recreating a ledger at the same
// address while any old token survives is forbidden; these are scoped handles.
// Calls are synchronous, single-threaded and nonreentrant. The expected event
// sequence rejects stale work; it is not a lock or concurrent-access guard.
// No method asserts renderer completion or acknowledges a successful draw.
class OriginalUiRiderMaterialLifetime50KG final {
public:
    OriginalUiRiderMaterialLifetime50KG() noexcept;
    ~OriginalUiRiderMaterialLifetime50KG();
    OriginalUiRiderMaterialLifetime50KG(const OriginalUiRiderMaterialLifetime50KG&)=delete;
    OriginalUiRiderMaterialLifetime50KG& operator=(const OriginalUiRiderMaterialLifetime50KG&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    std::uint64_t sequence()const noexcept;
    std::uint64_t acknowledgedSequence()const noexcept;
    std::size_t pendingEvents()const noexcept;
    bool hasOwners()const noexcept;
    bool populationActive()const noexcept;
    bool aborted()const noexcept;
    // Nonmutating capacity/identity previews for the same exact operation.
    // Recheck the unchanged sequence immediately before real source effects.
    bool canAcquire(const OriginalUiRiderMaterialFootprint50KG&,std::uint64_t expectedSequence,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&)const noexcept;
    bool canRelease(const OriginalUiRiderMaterialOwnerToken50KG&,std::uint64_t expectedSequence,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&)const noexcept;
    bool canPopulate(std::uint64_t expectedSequence,OriginalUiRiderMaterialLifetimeDiagnostic50KG&)const noexcept;
    // Explicit ordered owner events. Both outputs and all ledger state remain
    // unchanged on failure. Every operation checks its expected prior sequence.
    // Acquiring refs does NOT create/populate a PSM or assign a live epoch.
    // It reserves journal AND sequence capacity for this domain's worst-case
    // release (ownerReleased+rowRetired per distinct row). Other operations
    // cannot consume that reserve. Release remains legal after scan abort.
    // No acquire/release can interleave with a scan or awaited group rebuild.
    // Acquire's output owner token must be default/unissued, never overwritten.
    bool acquire(const OriginalUiRiderMaterialFootprint50KG&,std::uint64_t expectedSequence,
        OriginalUiRiderMaterialOwnerToken50KG&,OriginalUiRiderMaterialMutation50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    bool release(const OriginalUiRiderMaterialOwnerToken50KG&,std::uint64_t expectedSequence,
        OriginalUiRiderMaterialMutation50KG&,OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    bool owns(const OriginalUiRiderMaterialOwnerToken50KG&)const noexcept;
    bool rowState(const OriginalUiRiderMaterialRow50KG&,OriginalUiRiderMaterialRowState50KG&)const noexcept;
    bool event(std::uint64_t sequence,OriginalUiRiderMaterialEvent50KG&)const noexcept;
    bool retirementCurrent(const OriginalUiRiderMaterialRetirement50KG&)const noexcept;
    // Journal consumption only, after all dependent consumers accepted those
    // exact lifecycle events. Does not mutate rows/epochs or source sequence.
    bool acknowledgeThrough(std::uint64_t sequence,OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
private:
    friend class OriginalUiRiderBackend50KG;
    friend class OriginalUiTournamentPodOwner50KG;
    // Fix the exact sorted pending plan and reserve its row events, one
    // population boundary and one later group rebuild before construction.
    // A successful finish stays locked until recordGroupRebuilt or abort.
    bool beginPopulation(std::uint64_t expectedSequence,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    const OriginalUiRiderMaterialRow50KG* nextPopulationRow()const noexcept;
    // Backend invokes only AFTER the complete native row candidate exists.
    // One new epoch is committed immediately; later failure never rolls it back.
    bool commitConstructedRow(const OriginalUiRiderMaterialRow50KG&,std::uint64_t expectedSequence,
        OriginalUiRiderMaterialRowState50KG&,OriginalUiRiderMaterialMutation50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    // Receipt covers this scan's row events and its success boundary.
    bool finishPopulation(std::uint64_t expectedSequence,OriginalUiRiderMaterialMutation50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    // Issue only after actual synchronous collector grouping has succeeded.
    bool recordGroupRebuilt(std::uint64_t expectedSequence,OriginalUiRiderMaterialMutation50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    // No event or epoch is fabricated. Permanently poisons new acquisition
    // and population, drops only the unfinished scan reservation, and permits
    // reserved cleanup releases of every still-active domain.
    bool abortPopulation(std::uint64_t expectedSequence,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    // Dormant all-success wrapper. Only the private issuer can assert that
    // every planned native row already exists. Uses the same plan/lock and
    // still requires a separate actual group rebuild, never a public shortcut.
    bool populate(std::uint64_t expectedSequence,OriginalUiRiderMaterialMutation50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
