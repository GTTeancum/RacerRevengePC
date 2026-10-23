#pragma once
// Retained source2743B0/27BEA0/1B5590 collision owners. Neither visual PSG
// ownership nor a counted descriptor is evidence of a live collision shape.
#include "original_ui_garage_scene_dispatch50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "../../../src/assets/original_standard_collision_type0_resource50la.h"
#include "../../../src/game/pod_shape49w.h"
#include <array>
#include <memory>
#include <optional>
#include <string>
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalUiGarageSceneCollision50KG final {
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiGarageStaticModels50KG;
    friend class OriginalUiGarageActors50GN;
    using C=game::OriginalWreckComponent50LA;
    using K=game::OriginalWreckComponentKind50LA;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using Resource=assets::OriginalStandardCollisionType0Resource50LA;
    using Provider=assets::OriginalStandardArchiveProvider50LA;
    static constexpr std::size_t count=assets::originalUiGarageStaticModelCount50KG;
    struct Query final {
        // Actual11B190 object8 Entity, not a visualModel30 alias. Native
        // value-initialization of raw storage is not source field provenance.
        C entity8;
        std::uint16_t field00=0;
        std::uint8_t type02=0xff,flags03=0,materialA8=0,materialA9=0,flagsF8=0;
        game::SpringVector4_48N copied40{},copied50{},previous70{};
        float distance84=0;
        std::uint32_t type88=0,current94=0,field98=0,categoryA4=0,acceptB8=0xffffffffu;
        const void* owner9C=nullptr;const void* bodyA0=nullptr;
        const void* callbackE4=nullptr;const void* callbackE8=nullptr;
        const void* callbackEC=nullptr;const void* callbackF0=nullptr;
        std::uint32_t fieldF4=0;
        // Three genuine empty constructor lists atBC/C8/D4. No synthetic
        // per-face material IDs or query-result90 allocation is introduced.
        struct EmptyFilter {std::int32_t mode=1;const void* data=nullptr;std::uint32_t count=0;};
        std::array<EmptyFilter,3> filters{};
        std::optional<assets::OriginalStandardModelFileId50LA> fileB0;
        // Shape110 is authentically NULL for the static1B5728 call(a1=0).
        const C* model110=nullptr;
        std::shared_ptr<Resource> resource124,loading;
        bool shape8C=false;
        game::PodShapeSource49W source{};
        game::PodShapeState49W shape{};
        //120070(F8.bit3) uses a transient stack observer, leaving queryE0 NULL.
        // The persistent registration receipt belongs to EXISTING secondary4C4.
        // Slot ordinal+1 is local to that domain, never a primary visual ID.
        std::array<game::CollectionZoneLink50BX,1> links{};
        std::array<game::LiveZoneEntry50EN,1> entries{};
        game::LiveZoneMembership50EN membership{};
        const C* child04=nullptr;const void* method08=nullptr;
        std::array<std::uint8_t,48> transientObserver50{};
        bool sharedObserver=false,entityLive=false,retiring=false,retired=false;
        bool loaded=false,providerAbsent=false;
    };
    struct Row final {
        OriginalUiGarageSceneDispatch50KG::CollisionWrapper* wrapper=nullptr;
        // Comparison-only identity after27BB80 frees the raw wrapper. The
        // writable borrow above is cleared at that actual destruction, before
        // the separate StaticModels child walk can reach query11B4A0.
        const void* wrapperIdentity=nullptr;
        // Comparison-only tombstone after source1B58B0 deletes temporary08.
        const void* descriptorIdentity=nullptr;
        std::string originalStem,lowerStem,requestedMember;
        Provider::MemberView member{};
        std::unique_ptr<Query> query;
        bool attempted=false,completed=false;
    };
    OriginalUiGaragePodScene50KG* scene_=nullptr;
    std::shared_ptr<Grant> grant_;
    std::shared_ptr<OriginalUiGarageModelResources50KG> resources_;
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility_;
    std::shared_ptr<Provider> provider_;
    std::shared_ptr<assets::OriginalStandardTextureResolver50LA> resolver_;
    C* zone3C4_=nullptr;
    std::array<Row,count> rows_;
    std::uint64_t providerRevision_=0;
    std::size_t completed_=0;
    std::uint32_t boundary_=0;
    bool attempted_=false,complete_=false,closed_=false,visibilityBound_=false;
    const OriginalUiGaragePageOwner50KG* rawListRetirementPage_=nullptr;
    // Distinct failed attachment6 custody. This is never a Page slot6 receipt.
    const OriginalUiGaragePageOwner50KG* failedRawListPage_=nullptr;
    bool pageQueriesRetired_=false;
    OriginalUiGarageSceneCollision50KG()noexcept=default;
    bool populate2743B0(OriginalUiGaragePodScene50KG&,D&)noexcept;
    bool populateRow1B5590(std::size_t,D&)noexcept;
    bool load11B920(std::size_t,D&)noexcept;
    bool releaseShape11B600(Query&,D&)noexcept;
    bool belongsTo(const OriginalUiGaragePodScene50KG&,const std::shared_ptr<Grant>&,
        const std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>&)const noexcept;
    bool current2743B0(const OriginalUiGaragePodScene50KG&)const noexcept;
    bool staticSuffixCurrent(const OriginalUiGarageStaticModels50KG&,const C* previous,const C* child)const noexcept;
    bool retireQuery11B4A0(std::size_t,D&)noexcept;
    // Actual Page ordinal6 raw payload dies before ordinal5 query children.
    // Neither step releases secondary SearchList membership or its backing.
    bool retirePageRawList27BB80(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool rawListRetired27BB80()const noexcept;
    // Actual raw3CC is still detached at the immutable attachment6 failure.
    // Its source27BB80 payload dies before the already-attached Static3C4.
    bool preflightFailedRawList27BB80(const OriginalUiGaragePageOwner50KG&,D&)const noexcept;
    bool retireFailedRawList27BB80(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool failedRawListRetired27BB80(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool releaseRawWrappers27BBCC(D&)noexcept;
    bool retireFailedPageQueries1424E0(const OriginalUiGaragePageOwner50KG&,
        OriginalUiGarageStaticModels50KG&,D&)noexcept;
    bool retirePageQueriesBody1424E0(const OriginalUiGaragePageOwner50KG&,
        OriginalUiGarageStaticModels50KG&,D&)noexcept;
    bool retirePageQueries1424E0(const OriginalUiGaragePageOwner50KG&,
        OriginalUiGarageStaticModels50KG&,D&)noexcept;
    // Explicit partial-prefix cleanup only. Full Page teardown remains its
    // owner's ordered predecessor, and cannot be inferred from this method.
    bool closePartial2743B0(D&)noexcept;
public:
    ~OriginalUiGarageSceneCollision50KG()noexcept;
    OriginalUiGarageSceneCollision50KG(const OriginalUiGarageSceneCollision50KG&)=delete;
    OriginalUiGarageSceneCollision50KG& operator=(const OriginalUiGarageSceneCollision50KG&)=delete;
};
}
