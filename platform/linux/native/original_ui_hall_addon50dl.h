#pragma once
// Hall of Fame add-on course entry (DLC, helper 148r). PORT EXTENSION, NOT ORIGINAL BEHAVIOUR.
// While the Hall shows an add-on course (port state of the Hall session over its
// stock anchor control; original navigation, widgets and profile stay on the
// anchor), the session sets ONE frame record for the render and clears it right
// after. The Hall draw code reads it to draw the add-on name in place of the
// anchor's course-name text (line 0, that widget's own font, colour and
// position) and the add-on planet name in place of the planet header.
// Owner thread only. Null = an ordinary stock frame (nothing substituted).
// Narrow on purpose (standard headers only); implemented in
// original_ui_hall_draw_contribution50kg.cpp.
#include <cstdint>
#include <string>

namespace rr::platform {
struct OriginalUiHallAddonFrame50DL {
    std::uint16_t anchorName=0;              // course-name text widget of the anchor control (0 = none)
    const std::string* name=nullptr;         // add-on course name drawn there
    const std::string* planetName=nullptr;   // planet header text (<= 14 characters), null = stock planet name
    unsigned nameDrawn=0,nameSkipped=0,planetDrawn=0; // filled by the frame
};
void setOriginalUiHallAddonFrame50DL(OriginalUiHallAddonFrame50DL*) noexcept;
OriginalUiHallAddonFrame50DL* originalUiHallAddonFrame50DL() noexcept;
} // namespace rr::platform
