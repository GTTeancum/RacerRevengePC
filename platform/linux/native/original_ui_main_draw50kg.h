#pragma once
#include "original_ui_material_adapter50kg.h"
#include "original_ui_main_particle_adapter50kg.h"
#include "../../../src/game/original_ui_main_particle_geometry50kg.h"
#include "../../../src/game/original_ui_main_particle_render_state50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/assets/original_ui_main_material_schedule50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
#include "../../../src/game/original_ui_main_widget_manager50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace rr::platform {
// NEW native reconstruction. No source PCs are executable addresses.
enum class OriginalUiMainDrawError50KG : std::uint8_t {
    none,alreadyLoaded,notLoaded,archive,videoOwner,manager,resource,material,
    animation,geometry,samplerConflict,allocation,input,capacity,sourceOrder,
    projection,publication,registry,unfinished,unsupportedDepth
};
struct OriginalUiMainDrawDiagnostic50KG {
    OriginalUiMainDrawError50KG error=OriginalUiMainDrawError50KG::none;
    std::uint16_t widgetId=0;
    std::uint32_t materialIndex=0,imageIndex=0,sourcePC=0;
    assets::OriginalUiScreenAssetDiagnostic50KG resource{};
    assets::OriginalUiMaterialDiagnostic50KG material{};
    assets::DecodeError48C atm{};
    assets::OriginalUiMainMaterialScheduleDiagnostic50KG registry{};
    game::OriginalUiMainWidgetDiagnostic50KG manager{};
    game::OriginalUiTextGeometryDiagnostic50KG text{};
    game::OriginalUiExitTextDiagnostic50KG exitText{};
    game::OriginalUiMainFireStatus50KG fire=game::OriginalUiMainFireStatus50KG::complete;
    game::OriginalUiMainParticleGeometryStatus50KG particle=game::OriginalUiMainParticleGeometryStatus50KG::complete;
};
struct OriginalUiMainVideoOwner50KG {
    // Genuine shared ownership of the admitted native video-mode owner, never
    // a no-op alias. Numbers alone are not proof of an original mode lifecycle.
    std::shared_ptr<const void> lifetime;
    std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;
    std::uint32_t zBufferFormat=0;
};
struct OriginalUiMainColdInput50KG {
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,mainMenu,fonts,controllerPopup,overlays;
    const assets::OriginalUiEnglish50KG* english=nullptr; // synchronous copy only
    assets::OriginalUiMaterialRuntime50KG runtime{};
    OriginalUiMainVideoOwner50KG video{};
};
struct OriginalUiMainMaterialBinding50KG {
    // Original registry spelling normalized only for lookup. Original PSM and
    // each pass retain their actual spelling, group and provenance unchanged.
    std::string registeredName;
    assets::OriginalUiMaterial50KG material;
    std::vector<std::uint32_t> images; // one PST, or ALL actual ATM frame ranges
    std::vector<assets::OriginalUiNativeMaterial50KG> policies; // exact per-frame atlas
    assets::OriginalUiNativeUntexturedMaterial50KG untextured{};
    game::OriginalUiTextureAnimationConfig50KG animation{};
    bool animated=false;
    std::uint8_t projectedLeafOrdinal=0;
};
// One immutable cold resource owner and one real shared49-widget population.
// CPU-only load is all-or-nothing. All five archives, video owner, descriptors,
// CEL metrics, actual PST/ATM images and native policies stay pinned. No path
// search, substitute image, synthesized PSM, font fallback or menu labels.
class OriginalUiMainDrawResources50KG final {
public:
    OriginalUiMainDrawResources50KG() noexcept;
    ~OriginalUiMainDrawResources50KG();
    OriginalUiMainDrawResources50KG(const OriginalUiMainDrawResources50KG&)=delete;
    OriginalUiMainDrawResources50KG& operator=(const OriginalUiMainDrawResources50KG&)=delete;
    bool load(const OriginalUiMainColdInput50KG&,OriginalUiMainDrawDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    // Source page/controller owns mutation at its real callback/update points.
    // A draw projection never silently performs an entity update or selection.
    game::OriginalUiMainWidgetManager50KG* manager() noexcept;
    const game::OriginalUiMainWidgetManager50KG* manager() const noexcept;
    const assets::OriginalUiScreenAssets50KG* images() const noexcept;
    const assets::MaterialTextureAssets50EA* textures() const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers() const noexcept;
    const std::vector<OriginalUiMainMaterialBinding50KG>* materials() const noexcept;
    const OriginalUiMainVideoOwner50KG* video() const noexcept;
    const game::OriginalUiTextGeometry50KG* textGeometry(std::uint16_t) const noexcept;
    std::uint32_t widgetMaterial(std::uint16_t) const noexcept;
    std::uint32_t materialIndex(const char* registeredName) const noexcept;
    const game::OriginalUiMainBackground50KG* background() const noexcept;
    // One embedded shared-page+440 owner, independent of DAT widget IDs.
    game::OriginalUiExitText50KG* exitText() noexcept;
    const game::OriginalUiExitText50KG* exitText() const noexcept;
    const assets::OriginalUiMainParticleMaterial50KG* particleMaterial() const noexcept;
    std::uint32_t particleImageIndex() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};

enum class OriginalUiMainDrawSegment50KG : std::uint8_t {
    ordinaryWidgets,mainPage,aperture,basePrompts,onTopWidgets,particles,exitText
};
struct OriginalUiMainSubmittedPacket50KG {
    // materialIndex==unresolvedMaterialTexture50EA denotes the separately
    // admitted particle/mmprt leaf; frameIndex then selects its source state0/1.
    std::uint32_t firstQuad=0,quadCount=0,materialIndex=0,imageIndex=0,frameIndex=0;
    std::uint32_t submissionOrdinal=0,sourcePC=0;
    std::uint16_t widgetId=0;
    OriginalUiMainDrawSegment50KG segment=OriginalUiMainDrawSegment50KG::ordinaryWidgets;
};
struct OriginalUiMainFrameClock50KG {
    // Same application millisecond sample used by source material24A740.
    // Caller already owns pause/update gating; no host clock is sampled here.
    bool known=false,advanceAnimations=false;
    std::uint32_t milliseconds=0;
};
// Real scene/particle groups are deliberately separate contributions. This
// marker is an insertion point in the original submission sequence, NOT an
// empty scene, a certified scene draw, or a promise that a whole frame is ready.
struct OriginalUiMainSceneInsertion50KG { std::uint32_t nextSubmissionOrdinal=0; };

// Reusable CPU packet collector. bind pins exact CPU/GPU owners; GL creation
// remains the context owner's operation using resources.textures()/samplers().
// Serial use only. Project at actual traversal points, then drain through the
// actual registry. drain requires the real source scene-packet join as well as
// Main page/aperture/prompt points. Complete draw preparation does not itself
// certify page transition hooks, audio, route capability or GPU submission.
class OriginalUiMainDraw50KG final {
public:
    OriginalUiMainDraw50KG() noexcept;
    ~OriginalUiMainDraw50KG();
    OriginalUiMainDraw50KG(const OriginalUiMainDraw50KG&)=delete;
    OriginalUiMainDraw50KG& operator=(const OriginalUiMainDraw50KG&)=delete;
    bool bind(std::shared_ptr<OriginalUiMainDrawResources50KG>,
        std::shared_ptr<const MaterialTextureGl50EB>,OriginalUiMainDrawDiagnostic50KG&) noexcept;
    bool begin(const OriginalUiMainFrameClock50KG&,OriginalUiMainDrawDiagnostic50KG&) noexcept;
    // Preserves exact same-manager sibling order; does not filter by page
    // control membership. These are source attachment segments, not passes.
    bool projectOrdinaryWidgets(bool ancestorsShown,OriginalUiMainDrawDiagnostic50KG&) noexcept;
    // Call at Main1E6410 before shared1D7550, only if that page is actually
    // traversed. Selected slot comes from the SAME manager's page1 Group1.
    bool projectMainPage(bool pageAncestorsShown,OriginalUiMainDrawDiagnostic50KG&) noexcept;
    // Wire to screenExitObject440 at1D75A4 before aperture rectangle0. This
    // prepares the actual localized shared ScreenText and queues its genuine
    // denmrk16Top packet through the same source registry as all other text.
    bool projectExitText(const game::OriginalUiApertureHook50KG&,
        OriginalUiMainDrawDiagnostic50KG&) noexcept;
    // Wire directly to aperture.submitOverlay at1D798C. No render/update is
    // invoked here. Original RGB=0, alpha=63, primitive depth=1; source order
    // is +424 then +428. Source rectangle order becomes0,2,3,1.
    bool projectAperture(std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG&,
        OriginalUiMainDrawDiagnostic50KG&) noexcept;
    // Shared1D7A10 after the two aperture submissions. Real Main mask42C is
    // zero on entry/closing and1 (cross) when opened. No label/image substitute.
    bool projectMainBasePrompts(std::uint8_t sourceMask42C,OriginalUiMainDrawDiagnostic50KG&) noexcept;
    OriginalUiMainSceneInsertion50KG sceneInsertion() const noexcept;
    // Actual1CF410 attaches Main scene children AFTER all Pod children/OnTop.
    // Reserve one sequence position at that tail for a real collected scene
    // contribution. Returns the slot; never claims to have rendered particles.
    bool recordSceneInsertion(OriginalUiMainSceneInsertion50KG&,
        OriginalUiMainDrawDiagnostic50KG&) noexcept;
    bool projectOnTopWidgets(bool ancestorsShown,OriginalUiMainDrawDiagnostic50KG&) noexcept;
    // Actual Main scene tail: prepare native source packets, validate run headers,
    // expand ParticleCode quads, and retain fog factors/source-state selection.
    // Uses the concrete later scene fog owner: adjusted camera, VU fog constants
    // and original packed FOGCOL conversion, not the earlier cold camera alone.
    // An observed empty source batch counts as a join; an absent owner does not.
    bool projectMainParticles(game::OriginalUiMainFireOwner50KG&,
        const game::OriginalUiMainSceneOwner50KG&,const game::OriginalUiMainParticleRenderState50KG&,
        OriginalUiMainDrawDiagnostic50KG&) noexcept;
    // Authenticated16-leaf Mat_Stbl projected drain, FIFO per leaf. This closed
    // Main graph admits only immutable PST/ATM images and typed ScreenObject/
    // ParticleCode policies with no destination-alpha consumer. It owns opaque
    // presentation alpha; no user-supplied certification boolean is accepted.
    bool drain(OriginalUiMainDrawDiagnostic50KG&) noexcept;
    // Borrowed immutable frame view until next begin/destruction. Submit it
    // synchronously; no concurrent manager/frame/GPU-publication mutation.
    const OriginalUiNativeDraw50KG* draw() const noexcept;
    const std::vector<OriginalUiMainSubmittedPacket50KG>* packets() const noexcept;
    // Parallel to draw spans; identifies exact source packet after draining.
    const std::vector<std::uint32_t>* drainedPacketIndices() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
    bool projectWidgets(bool onTop,bool ancestorsShown,OriginalUiMainDrawDiagnostic50KG&) noexcept;
    bool appendOverlay(const game::OriginalUiOverlayQuad50KG&,std::uint32_t material,
        float packetZ,OriginalUiMainDrawSegment50KG,std::uint16_t widget,
        OriginalUiMainDrawDiagnostic50KG&) noexcept;
};
}
