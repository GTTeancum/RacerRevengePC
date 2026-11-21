#pragma once
#include "original_ui_pages50kg.h"
#include "model_geometry49o.h"
#include "import_scene48d.h"
#include <array>
#include <memory>

namespace rr::assets {
struct OriginalUiRiderPodConfig50KG {
    std::array<float,3> position{},look{},up{},velocity{};
    std::array<bool,3> rotate{};
    bool lit=false,startPaused=true,cleanFromMemory=false;
};
enum class OriginalUiRiderPodError50KG : std::uint8_t {
    none,alreadyLoaded,archive,widget,member,hierarchy,geometry,allocation
};
struct OriginalUiRiderPodDiagnostic50KG {
    OriginalUiRiderPodError50KG error=OriginalUiRiderPodError50KG::none;
    OriginalUiPageDiagnostic50KG text{};
    GeometryDiagnostic49O hierarchy{};
    DecodeError48C geometry{};
    std::uint32_t sourcePC=0;
};
// Authenticated cold owner for the real dynamic User_Rotate pod geometry.
// No fictitious character/engine split, static portrait or replacement mesh.
// loaded() proves cold assets only, not dynamic model/render/route readiness.
class OriginalUiRiderPodAssets50KG final {
public:
    OriginalUiRiderPodAssets50KG() noexcept;
    ~OriginalUiRiderPodAssets50KG();
    OriginalUiRiderPodAssets50KG(const OriginalUiRiderPodAssets50KG&)=delete;
    OriginalUiRiderPodAssets50KG& operator=(const OriginalUiRiderPodAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,std::uint16_t widgetId,
        OriginalUiRiderPodDiagnostic50KG&) noexcept;
    // Separate archive gate preserves Rider load's contract. career is the
    // actual selected100C0 value, never an ordinal in a host character list.
    // Source1EEC40 table3AFEA0 selects DAT widgets39..61. Default cases fail
    // closed: they do not borrow another pod or fabricate an unlock.
    bool loadTournament(std::shared_ptr<const OriginalUiArchive50KG>,
        std::uint32_t career,OriginalUiRiderPodDiagnostic50KG&) noexcept;
    // Separate archive gate: UIUnlock widgets 9..23 from UI/UIUnlock/Widgets.Dat
    // (geometry UI/Geometry/NN.psg in UIUNLOCK; textures from UIPODTEX/UIUNLOCK).
    bool loadUnlock(std::shared_ptr<const OriginalUiArchive50KG>,std::uint16_t widgetId,
        OriginalUiRiderPodDiagnostic50KG&) noexcept;
    // PORT EXTENSION, NOT ORIGINAL (DLC): an add-on pod's menu model. The
    // User_Rotate layout (position, look, up, rotation) is a read-only copy of the
    // authored stock Rider widget `layoutWidget` (PORT STAND-IN: an add-on package
    // has no Rider widget of its own); the model is `modelName` in the add-on's
    // own archive (identity addonUi), found by name. widgetId() is layoutWidget.
    bool loadAddon(std::shared_ptr<const OriginalUiArchive50KG> stockRider,std::uint16_t layoutWidget,
        std::shared_ptr<const OriginalUiArchive50KG> addonUi,const std::string& modelName,
        OriginalUiRiderPodDiagnostic50KG&) noexcept;
    // PORT EXTENSION, NOT ORIGINAL (DLC career): an add-on pod's menu model
    // laid out by the Tournament page's own User_Rotate widget of the selected
    // (anchor) career (the 1EEC40 table3AFEA0 widget 39..61 loadTournament
    // uses), the model `modelName` from the add-on's own archive. archive() is
    // the add-on archive; layoutArchive() the UITourn archive.
    bool loadTournamentAddon(std::shared_ptr<const OriginalUiArchive50KG> tournament,std::uint32_t career,
        std::shared_ptr<const OriginalUiArchive50KG> addonUi,const std::string& modelName,
        OriginalUiRiderPodDiagnostic50KG&) noexcept;
    // The stock archive the layout was read from (add-on pods only; else null).
    std::shared_ptr<const OriginalUiArchive50KG> layoutArchive() const noexcept;
    bool addon() const noexcept;
    bool loaded() const noexcept;
    std::uint16_t widgetId() const noexcept;
    const OriginalUiRiderPodConfig50KG* config() const noexcept;
    const PsgHierarchyView49O* hierarchy() const noexcept;
    const Scene48D* scene() const noexcept;
    const OriginalUiMemberIdentity50KG* widgetSource() const noexcept;
    OriginalUiMemberView50KG modelSource() const noexcept;
    // Pin retains all original material members for the shared material owner.
    std::shared_ptr<const OriginalUiArchive50KG> archive() const noexcept;
private:
    bool loadImpl(std::shared_ptr<const OriginalUiArchive50KG>,std::uint16_t,
        bool tournament,OriginalUiRiderPodDiagnostic50KG&,bool unlock=false) noexcept;
    bool decodeModel(OriginalUiRiderPodDiagnostic50KG&) noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
