#pragma once
#include "original_ui_archive50kg.h"
#include "import_texture48e.h"
namespace rr::assets {
// A distinct native cold kind, not a PST kind or a fabricated indexed palette.
inline constexpr std::uint32_t originalUiControlsTga1555Kind50KG=3;
bool isOriginalUiControlsSpecmap50KG(const OriginalUiMemberIdentity50KG*) noexcept;
// Exact authenticated 256x256, type2, BGR24, bottom-origin specmap3.tga.
//29EA90/29EB70/29ECD8 reads/swaps/reverses rows. The texture registry
//29E3B0->24DEDC selects1555,2511B0 swaps source B/R again, and16C0D0
//quantizes RGB to5 bits (magenta keeps RGB and clears its alpha bit).
//The typed DefaultMT policy requires TA0=TA1=255/AEM0: sampled alpha255.
// No other TGA or image format is admitted and no source member is rewritten.
bool decodeOriginalUiControlsSpecmap50KG(const OriginalUiMemberView50KG&,Texture48E&,DecodeError48C&) noexcept;
}
