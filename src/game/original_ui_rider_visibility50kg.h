#pragma once
#include "original_ui_rider_model_scene50kg.h"
#include "original_ui_rider_model_fog50kg.h"
#include "original_ui_rider_clip_camera50kg.h"
#include <memory>

namespace rr::game {
enum class OriginalUiRiderVisibilityError50KG : std::uint8_t {
    none,input,alreadyBound,registration,staleHandle,camera,bounds,arithmetic,allocation
};
struct OriginalUiRiderVisibilityDiagnostic50KG {
    OriginalUiRiderVisibilityError50KG error=OriginalUiRiderVisibilityError50KG::none;
    std::uint32_t sourcePC=0;
};
// Actual normal source defaults:3833D8 is initialized1 in the executable;
//4354F0 is zero-initialized. A caller changing either original debug gate must
// supply its current value. No inferred always-visible or widget-selected gate.
struct OriginalUiRiderVisibilityPolicy50KG {
    bool useMembership3833D8=true;
    bool guardInsideSets80_4354F0=false;
};
struct OriginalUiRiderMembershipCamera50KG {
    //27F650 bounds4C,50,54,58 are [-1,+1,-1,+1]. No portal list30 exists
    // in this flat UI owner. Matrix60 is viewC0 * projection14DAF0, NOT core80.
    std::array<float,4> viewport4C{{-1,1,-1,1}};
    std::array<float,16> projection{},matrix60{};
};
bool prepareOriginalUiRiderMembershipCamera50KG(const OriginalUiRiderCamera50KG&,
    OriginalUiRiderMembershipCamera50KG&) noexcept;
struct OriginalUiRiderBoxClip50KG {
    // Original low six flags: +x,-x,+y,-y,+z,-z. Edges are inside.
    std::uint8_t intersection=0,unionMask=0;
    bool outside=false;
};
// Native geometric projection of2B0580. Preserves its eight-corner walk and
// separate binary32 additions, rather than independently rebuilding each
// center +/- extent corner. No guest registers, instructions or chip state.
bool projectOriginalUiRiderBoxClip50KG(const std::array<float,16>& modelToClip,
    const std::array<float,4>& center160,const std::array<float,4>& halfExtent170,
    OriginalUiRiderBoxClip50KG&) noexcept;

class OriginalUiRiderVisibilityOwner50KG;
struct OriginalUiRiderVisibilityHandle50KG {
    const OriginalUiRiderVisibilityOwner50KG* owner=nullptr;
    std::uint64_t generation=0;
    std::uint16_t widget=0;
};
struct OriginalUiRiderModelSelection50KG {
    OriginalUiRiderVisibilityHandle50KG membership;
    std::uint64_t frameSerial=0,selectionSerial=0;
    // Real member+4 names THIS flat owner, whose original zone+48 is zero.
    // This value is only evidence when owner.isCurrentSelection returns true.
    std::uint32_t sourceZoneIndex48=0;
    std::uint8_t sourceModelFlags191=0,sourceRenderFlags1B2=0;
    OriginalUiRiderBoxClip50KG viewportClip,guardClip;
    std::uint8_t remainingClipMask=0;
    bool guardChecked=false,disabled=false;
    BodyMatrix48G rootWorld100{};
    ModelBoundsResult49O bounds{};
    bool selected() const noexcept {return (sourceModelFlags191&1u)!=0;}
};
// Real native membership owner for the Rider dynamic-model subset of the
// UI's flat search. Registration pins original cold assets and binds the actual
// model scene. Scene objects must outlive their registrations. It does not
// replace the UI entity owner, binder owner, material ordering or packet queue.
// Caller drains every borrowed model packet BEFORE removeModel; unregister
// BEFORE modelDestroyedAfterDetach/reload. Stale handles cannot select a new
// model generation. Disabled models retain membership and skip240C00.
class OriginalUiRiderVisibilityOwner50KG final {
public:
    OriginalUiRiderVisibilityOwner50KG() noexcept;
    ~OriginalUiRiderVisibilityOwner50KG();
    OriginalUiRiderVisibilityOwner50KG(const OriginalUiRiderVisibilityOwner50KG&)=delete;
    OriginalUiRiderVisibilityOwner50KG& operator=(const OriginalUiRiderVisibilityOwner50KG&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        OriginalUiRiderVisibilityDiagnostic50KG&) noexcept;
    // Separate source-authenticated owner; does not widen Rider bind().
    bool bindTournament(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        OriginalUiRiderVisibilityDiagnostic50KG&) noexcept;
    bool registerModel(std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG>,
        OriginalUiRiderCharacterScene50KG&,bool shown,OriginalUiRiderVisibilityHandle50KG&,
        OriginalUiRiderVisibilityDiagnostic50KG&) noexcept;
    bool registerModel(std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG>,
        OriginalUiRiderPodScene50KG&,bool shown,OriginalUiRiderVisibilityHandle50KG&,
        OriginalUiRiderVisibilityDiagnostic50KG&) noexcept;
    bool removeModel(const OriginalUiRiderVisibilityHandle50KG&) noexcept;
    bool setShown(const OriginalUiRiderVisibilityHandle50KG&,bool) noexcept;
    // After the actual selected packet owner completes24136C, retain its
    // independently computed radial-fog bit1 across the next240C00 invocation.
    // The selection snapshot remains the pre-packet boundary it represents.
    bool recordPacketFog(const OriginalUiRiderModelSelection50KG&,bool computeRadialFog) noexcept;
    bool contains(const OriginalUiRiderVisibilityHandle50KG&) const noexcept;
    std::uint32_t registeredCount() const noexcept;
    // Invoke after current camera/fog update and before model selected-render
    // traversal. Rebuilds membership60 and the separate guard camera2D0.
    bool prepareFrame(const OriginalUiRiderCamera50KG& cameraAfterFog,
        const OriginalUiRiderVisibilityPolicy50KG&,
        OriginalUiRiderVisibilityDiagnostic50KG&) noexcept;
    // Current post-motion aggregate bounds,240C00 clip flags and26EFC0's191.
    // Selected-only26F070 distance/fog continuation remains the packet owner's
    // existing prepareOriginalUiRiderModelFog50KG boundary.
    // Refreshes the actual root's world cache; bounds use the shared original
    // aggregate helper. Failure can refresh that cache but never publishes a
    // selection or changes the member's render flags/counters.
    bool selectModel(const OriginalUiRiderVisibilityHandle50KG&,
        OriginalUiRiderModelSelection50KG&,OriginalUiRiderVisibilityDiagnostic50KG&) noexcept;
    bool isCurrentSelection(const OriginalUiRiderModelSelection50KG&) const noexcept;
    // Enabled26EFC0 invocations, including culled models, since owner creation.
    // Disabled invocations do not increment source manager+278.
    std::uint32_t processedModels278() const noexcept;
    const OriginalUiRiderMembershipCamera50KG* membershipCamera() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
