#pragma once
// PC-NATIVE ADDITION, NOT ORIGINAL (and not a port stand-in): the Main menu EXIT GAME row.
//
// The PS2 Main page (UIMAIN ui.dat page 1, Group 1) has exactly five rows and no way to
// leave the game. This row and its yes/no dialog live entirely in host memory inside the
// Main session (original_ui_main_session50kg.cpp). They never add to or write ui.dat, the
// 49-slot widget bank, the navigation arrays beyond the manager's own select/deselect of
// an existing Group 1 control, the application's UI+47C popup state or the shared 470/47C
// dialogs. The row is drawn with Main's own row font/material and arrows, the dialog with
// the 47C popup's own materials, layout and timing (a host copy of its state machine).
//
// Includers (keep this list short; no other includes here):
//   platform/linux/native/original_ui_main_session50kg.cpp  (row, dialog, status, request)
//   platform/linux/native/original_ui_main_draw50kg.cpp     (arrow owner, read in projectMainPage)
//   tests/native/original_frontend_session50la.h            (frontend loop + test hook)
namespace rr::platform {
class OriginalUiMainDraw50KG;

// Main draw collector whose ORIGINAL selection arrows are not drawn because the EXIT GAME
// row owns the selection (the session draws the same arrows around that row instead).
// One owner at a time; set/cleared only by the owning Main session. Defined in the draw .cpp.
void setOriginalUiMainExitRowOwner50XG(const OriginalUiMainDraw50KG*) noexcept;
void clearOriginalUiMainExitRowOwner50XG(const OriginalUiMainDraw50KG*) noexcept;
bool originalUiMainExitRowOwns50XG(const OriginalUiMainDraw50KG*) noexcept;

// Observation of the live Main session's EXIT GAME state, for the frontend loop and the
// unattended test hook. Defined in the session .cpp.
struct OriginalUiMainExitGameStatus50XG {
    bool rowAvailable=false,rowSelected=false,dialogOpen=false,dialogAwaitingInput=false,exitRequested=false;
};
OriginalUiMainExitGameStatus50XG originalUiMainExitGameStatus50XG() noexcept;
// One-shot: true once, after the player confirmed EXIT GAME and its dialog finished closing.
// The frontend loop then takes the same clean close as the window Close button.
bool takeOriginalUiMainExitGameRequest50XG() noexcept;
}
