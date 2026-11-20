#pragma once
// The ONE allowed character set for DLC display strings (dlc.json "strings").
// PORT EXTENSION, NOT ORIGINAL: the original game has no add-on content.
//
// A package string can become a racer name, and racer names reach every font
// that draws one: the race HUD and standings (the port's host HUD font,
// platform\linux\native\vehicle_window_gl50ds_hud.cpp hostHudGlyph, a PORT
// STAND-IN font), Results 104 and the Hall (original Denmark/UIFONTS fonts),
// and the Rider-screen menu font (UI/UIFONTS.RES). The original fonts draw all
// of ASCII 32..126 (checked by the arcade converters), so the allowed set is
// the host HUD font's own set: space, A-Z, a-z (drawn upper case, as for stock
// names), 0-9 and ! ' ( ) + , - . / : ;
// No glyph is invented for the HUD font: a character outside this set is
// refused by the registry and tools\dlc\dlc_pack.py, naming the key and the
// character. The HUD source static_asserts that its glyph table draws exactly
// this set, so the two cannot drift apart without a build failure.
// Narrow on purpose (no includes): the registry and the HUD source include it.

namespace rr::assets::dlc50DL {

inline constexpr const char* dlcNameFontPunctuation="!'()+,-./:;";
inline constexpr bool dlcNameCharacter(char c) noexcept {
    if(c==' '||(c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9'))return true;
    for(const char* p=dlcNameFontPunctuation;*p;++p)if(*p==c)return true;
    return false;
}
// Human-readable form for refusal reasons and the report.
inline constexpr const char* dlcNameCharacterSetText="space, A-Z, a-z, 0-9 and ! ' ( ) + , - . / : ;";

} // namespace rr::assets::dlc50DL
