#pragma once
// Add-on rider overlay for one Rider frame (DLC phase 2, user decisions D4/D5).
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR: the original Rider screen has 23 authored
// rider controls and no add-on content.
//
// An add-on carousel slot is port state only (original_ui_rider_session50kg.cpp):
// the original navigation, widgets, models and profile stay on the stock control
// the add-on block follows (the "anchor"). For that frame the anchor control's
// four authored widgets (pod model, venus16 name, swhutt44 name, character model)
// draw nothing; the add-on's name line is laid out with the venus16 name widget's own
// font, colour and position. The stats code gets the add-on's own registry
// number (no stock portrait), and the anchor's NN_bkg background is not drawn,
// so no stock rider's model or image shows under an add-on name.
//
// The session sets the overlay for exactly one collector frame (Scope) and the
// collector reads it only when it names that collector. Narrow on purpose
// (standard headers only): included by the Rider session and its collector .cpp.
#include <array>
#include <cstdint>
#include <string>

namespace rr::platform::riderAddon50DL {

struct Line {
    const std::string* text=nullptr;      // first choice (pilot name / package name)
    const std::string* fallback=nullptr;  // used when the font cannot draw `text`
};
struct Overlay {
    const void* collector=nullptr;               // the one collector this frame belongs to
    std::array<std::uint16_t,4> suppressed{};    // anchor control widgets, authored order
    std::uint16_t nameWidget=0;                  // suppressed[1]: venus16 name text
    // The pilot name only, as on a stock control: package and source game stay in the
    // logs and dlc.json, never on screen.
    std::array<Line,1> lines{};
    std::uint32_t statRider=0;                   // the add-on's registry number the stats input carries
    std::uint32_t anchorRider=0;                 // the stock rider the add-on block follows (draw-order reference)
    // Menu-art step A: add-on model table row drawn in place of the anchor pod
    // (podWidget = suppressed[0]); npos = no add-on model this frame.
    static constexpr std::size_t noRow=static_cast<std::size_t>(-1);
    std::size_t modelRow=noRow;
    mutable std::uint32_t modelPackets=0;        // add-on model packets emitted this frame
    mutable std::uint32_t characterPackets=0;    // step B: add-on character packets this frame
    mutable const char* characterRefused=nullptr;
    mutable bool portraitDrawn=false,backgroundDrawn=false; // step C, this frame
    mutable const char* pageRefused=nullptr;
    mutable const char* modelRefused=nullptr;    // why the add-on model drew nothing (static text)
    mutable std::uint32_t drawnLines=0;          // this frame (logged once per highlighted add-on)
    mutable std::uint32_t skippedLines=0;        // lines the font could not draw
    bool suppresses(std::uint16_t id) const noexcept {
        if(!id)return false;
        for(const auto w:suppressed)if(w==id)return true;
        return false;
    }
};

inline const Overlay*& activeOverlay() noexcept {
    static thread_local const Overlay* overlay=nullptr;
    return overlay;
}
inline const Overlay* overlayFor(const void* collector) noexcept {
    const auto* overlay=activeOverlay();
    return overlay&&collector&&overlay->collector==collector?overlay:nullptr;
}
// One render call: set on entry, always cleared on exit (also on early return).
struct Scope {
    explicit Scope(const Overlay* overlay) noexcept {activeOverlay()=overlay;}
    ~Scope() {activeOverlay()=nullptr;}
    Scope(const Scope&)=delete;
    Scope& operator=(const Scope&)=delete;
};

} // namespace rr::platform::riderAddon50DL
