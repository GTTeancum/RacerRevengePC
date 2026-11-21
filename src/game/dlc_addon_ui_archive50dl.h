#pragma once
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR (DLC career art): the one reader of an
// add-on pod's own menu archive (dlc.json "ui.archive") for the screens that
// show the selected career's art (Tournament 1EEC40, Win 2077F0, Garage
// 1D1D68). Same bounded read and decode as the Rider add-on rows
// (original_ui_rider_addon_models50dl.cpp): the package's listed file through
// dlc50DL::host, at most RegistryLimits::uiArchiveBytes / uiDecodedBytes /
// uiEntries, archive identity addonUi (never a catalog row).
// Also the one place that names an add-on pod's art members, by the original
// rule each screen applies to a stock career (the %02d_flrg portrait, the
// %02d_bkg background, the %02dE Garage model) applied to the package's lists.
#include "../assets/original_ui_archive50kg.h"
#include <memory>
#include <string>

namespace rr::game::dlc50DL {
struct AddonCareerArt50DL {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive; // identity addonUi
    // Member names exactly as dlc.json lists them ("" = not listed).
    std::string portraitPst;        // ui.portraits  *.pst  (Tournament / Rider %02d_flrg)
    std::string backgroundPst,backgroundPsm; // ui.background *.pst / *.psm (%02d_bkg)
    std::string garagePsg;          // ui.garage_model *.psg (%02de.psg)
};
// Reads and decodes the archive of admitted add-on pod `number` and names its
// art members. False (with `why`) for an unknown pod or an archive outside the
// capacity; nothing is cached (each screen owns what it loads).
bool loadAddonCareerArt50DL(unsigned number,AddonCareerArt50DL& out,std::string& why) noexcept;
// "ui/textures/a2r_ark_flrg.pst" -> path "ui/textures/a2r_ark_flrg", leaf "a2r_ark_flrg".
void addonMemberStem50DL(const std::string& member,std::string& path,std::string& leaf);
}
