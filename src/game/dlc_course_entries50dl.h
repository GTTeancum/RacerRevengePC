#pragma once
// Add-on courses for free play (DLC phase 3, helper 146c): the Course-screen
// entries of the add-on courses, the port-owned add-on course selection and the
// race-load data decisions for an add-on course. PORT EXTENSION, NOT ORIGINAL BEHAVIOUR.
// Design: work\scratch\dlc\DESIGN.md 4.2 / 4.4, user decisions D1-D3, D10.
// Contract: work\scratch\dlc\DLC-JSON.md "Add-on items" (course item).
//
// - Rows come only from the frozen DLC registry (src\assets\dlc_registry50dl.h),
//   in number order (32, 33, ...). Original tables are never written.
// - Course screen (coordinator correction 2026-11-16): free play has no planet
//   list. The top arrows step through the courses one at a time by name; the
//   add-on courses follow the 13 stock courses in that arrow cycle. The planet
//   shows only as the full-screen background (course_background) and the
//   rotating hologram card (course_screen). No "DLC" label is shown. The profile
//   planet bits (1125E bits 5-7) and every remembered course field keep the last
//   STOCK selection.
// - Modes: Single Event, Time Trial and two-player VS per the item's modes; the
//   stock tournament never lists an add-on course (D3).
// - With no admitted add-on course every query answers "none", nothing is
//   logged and no control is appended (stock stays bit-identical).
// Narrow on purpose (standard headers only).
#include <cstddef>
#include <string>
#include <vector>

namespace rr::assets::dlc50DL {struct AddonCourse;}
namespace rr::game::dlc50DL {

// ---- Rows ---------------------------------------------------------------------
std::size_t addonCourseCount() noexcept;
const rr::assets::dlc50DL::AddonCourse* addonCourseAt(std::size_t index) noexcept;  // menu order
const rr::assets::dlc50DL::AddonCourse* addonCourse(unsigned number) noexcept;      // 32..95

// ---- Course-screen controls --------------------------------------------------
// The authored course group (track.dat group 1): control id, CourseIndex (0..12),
// planet ordinal (0..5) and widget count of every authored course control, in
// group order. The original cycles course by course in group order (1DF9C0 /
// 1DFBD0); the appended controls follow the last authored one, so the arrows
// reach the add-on courses after the 13 stock courses and then wrap.
struct AuthoredCourseControl {int controlId=-1;unsigned courseIndex=0,planet=0,widgets=0;};
// One port-owned control per add-on course, appended after the authored
// controls. Its layout (widgets, positions, hologram widget) is copied from the
// authored control of the same group whose course is the item's
// course_screen.stock_template; name, preview and background come from the package.
struct AppendedCourseControl {
    int controlId=-1;          // authoredControlCount + i (port-owned id)
    int layoutFrom=-1;         // authored control id whose layout is copied
    unsigned number=0;         // add-on course number (32..)
    std::string key,name,planetText,templateCode,preview; // preview: dlc:/ path, "" = template's own image (borrowed)
    std::string backgroundFile,backgroundStock;           // dlc:/ path, or the stock code whose background is reused
};
// Refuses (false, why) when an add-on would not fit the page's control capacity
// (src\assets\original_ui_pages50kg.h originalUiControlLimit50KG) or when the
// template course has no authored control. No add-on course: empty, true.
bool planAppendedCourseControls(const std::vector<AuthoredCourseControl>& authored,std::size_t authoredControlCount,
                                std::size_t controlLimit,std::vector<AppendedCourseControl>& out,std::string& why);

// ---- Port-owned selection ------------------------------------------------------
bool selectAddonCourse(unsigned number) noexcept;    // admitted add-on numbers only
void clearAddonCourse() noexcept;
unsigned selectedAddonCourse() noexcept;              // 0 = a stock course is selected
std::string courseKey(unsigned number);               // "stock:ta".. for 0..12, item key for 32.., "" unknown
enum class AddonCourseMode : unsigned char {single,timeTrial,vs};    // never the tournament
bool addonCourseAllowsMode(unsigned number,AddonCourseMode mode) noexcept;
// Launch rule (146r): the add-on course selected right now races only in free
// play (mode34 0) and only in a mode its item allows: subtype 3 = Time Trial,
// subtypes 1/2 = Single Event. Returns the number that races, 0 = the stock
// course the profile holds (the original Course target).
unsigned addonCourseForLaunch(unsigned mode34,unsigned subtype) noexcept;
// Test route only: RR_TEST_ADDON_COURSE=<package-id>/<item-id> selects that add-on
// course once (one dlc_course_test line either way). Unset = no-op.
void applyTestAddonCourse() noexcept;

// ---- Race-load data decisions ----------------------------------------------------
// Everything the common race load needs to know about an add-on course that is
// not in its archive's own members. Members are named by the item's code, as the
// original names them by course code.
struct AddonCourseLoad {
    unsigned number=0;
    std::string key,code,codeUpper,name,planetName;
    std::string archive;                                  // dlc:/<package>/TRACKS/<CODE>.RES
    std::string scb,tob,aub,cub,mapSpline,aiSpline,tvCamera; // member names ("" = absent optional member)
    std::vector<std::string> cutCameras;                  // tracks\<code>NN.vub
    std::string tuningArchive,tuningMember;bool tuningInherited=false;     // stock TRACKS/<CODE>.RES when inherited
    std::string particleArchive;bool particlesInherited=false;            // particle\demoprt.prt/.pst/.psm
    unsigned introEvents=0,scriptLines=0;                 // 0 accepted: no announcer intro lines, cut cameras still play
    std::string loadingScreen;bool loadingFromPackage=false;std::string loadingSha256; // package file (fingerprint = receipt) or stock LD_<CODE>.PST
    char phyClass='B',aiClass='B';                        // from dlc.json (the code-letter rule is for the 13 stock codes)
    unsigned rivalRacer=0;                                // 0 = none
    std::string modes;                                    // "single,time_trial,vs" subset
    std::string background;                               // course-screen background: dlc:/ file or "stock:<code>"
    const char* route="none";                             // see addonCourseRoute()
};
bool addonCourseLoad(unsigned number,AddonCourseLoad& out);
// Same rows by the course code ("nv" / "NV"); false for any non-add-on code.
bool addonCourseLoadByCode(const char* code,AddonCourseLoad& out);
std::string addonCourseLoadLine(const AddonCourseLoad&);

// Handling (PHY) class of a loaded course code (original 214954..214AA0 takes it
// from the code letters). Add-on code -> its dlc.json phy_class; false for any
// code that is not an admitted add-on (the caller keeps the original rule).
bool addonHandlingClass(const char* code,char& out) noexcept;

// Which race route an add-on course runs (146r): the ONE common route
// (HostTrack50IM::Course, track_session_assets50IG::registrySession, the common
// world/trigger/lighting/camera owners that SA also runs). Every member is named
// by the add-on code; no stock course's loader is chosen for it.
struct AddonCourseRoute {bool available=false;const char* name="none";const char* reason="";};
AddonCourseRoute addonCourseRoute() noexcept;

// Startup (after dlc50DL::startup()): one dlc_course line per add-on course with
// the race-load decisions, then the test selection. No add-on course: silent.
void announceAddonCourses() noexcept;

// ---- Records (DLC save side file, helper 146s src\assets\dlc_progress50dl.h) ----
// HOOK, not wired: the side file owner is not merged in this branch. The three
// call sites and what they pass are listed in dlc_course_entries50dl.cpp
// ("RECORDS HOOK"). Item key for ProgressRaceResult::courseKey / courseHall():
std::string addonCourseRecordsTarget(unsigned number); // item key; "" for stock numbers

// ---- Course-screen highlight (146r-U) ---------------------------------------------
// The add-on course entry the free-play Course screen shows right now (port state of
// the Course session; 0 = a stock course entry). Set only by the Course session; read
// by the test route (--addon-course) to know when its add-on is highlighted. Never
// selects anything: selectAddonCourse() at the accepted Confirm does that.
bool setHighlightedAddonCourse(unsigned number) noexcept; // 0, or an admitted add-on number
unsigned highlightedAddonCourse() noexcept;

} // namespace rr::game::dlc50DL
