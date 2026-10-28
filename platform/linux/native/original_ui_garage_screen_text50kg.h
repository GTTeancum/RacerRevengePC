#pragma once
#include "original_ui_menu_warning_packet50kg.h"
#include "../../../src/assets/original_application_fonts50la.h"
#include "../../../src/assets/original_application_localization50kg.h"
#include <array>
#include <vector>
#include "../../../src/assets/native_asset48b.h"
#include <atomic>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include "../../../src/game/original_application_entities50la.h"
namespace original_main_entry50KG { struct Application; class NativeCollectorFifo50LA; }
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::platform {
class OriginalUiGarageDrawContribution50KG;
class OriginalUiSharedDialogs50KG;
class OriginalUiGarageTextOverlay50KG;
enum class OriginalUiGarageScreenTextStatus50KG : std::uint8_t {
 complete, phase, authority, source, allocation, material, assertion, selectedTextPending
};
struct OriginalUiGarageScreenTextDiagnostic50KG {
 OriginalUiGarageScreenTextStatus50KG status=OriginalUiGarageScreenTextStatus50KG::complete;
 std::uint32_t sourcePC=0;
 bool sourceEffectsCommitted=false;
 std::size_t publishedPrimitives=0;
 assets::OriginalStandardMaterialDiagnostic50LA material{};
 assets::OriginalApplicationLocalizationDiagnostic50KG localization{};
};
// Concrete uncounted2E02F0/2DFD90 payload. No Widget/TextOverlay Entity, glyph
// regeneration, material population, demand, GPU packet or renderer admission.
// Construction and mutation are private. Actual text callers must retain the
// real selected32C borrow; dynamic rows retain their actual empty-string path.
// The separate legacy root probe remains closed and is not an issuance route.
class OriginalUiGarageScreenText50KG final {
 friend class ::ra_cold_aggregate50LA::Owner; //Same299BE0 process guard for real SunFlare handles.
 friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiSharedDialogs50KG;
 friend struct ::original_main_entry50KG::Application;
 friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
 friend class OriginalUiGarageTextOverlay50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageQuad50KG;
 using Authority=game::OriginalApplicationEntities50LA;
 using Application=::original_main_entry50KG::Application;
 using Fonts=assets::OriginalApplicationFonts50LA;
 using Domain=assets::OriginalStandardMaterialDomain50LA;
 using Context=assets::OriginalStandardMaterialContext50LA;
 using Archive=assets::OriginalUiArchive50KG;
 using Diagnostic=OriginalUiGarageScreenTextDiagnostic50KG;
 struct alignas(16) Qword {std::array<std::uint32_t,4> words{};};
 struct PacketDelete {void operator()(Qword*)const noexcept;};
 struct Vertex {
  std::array<float,4> authored00{{0,0,1,1}}; //2DFBA0, not packet XYZ.
  std::uint32_t aliases10=0;
  std::array<Qword*,2> stq14{},rgba1C{},xyz24{};
 };
 struct Glyph {std::array<Vertex,2> vertices;};
 struct Packet {
  std::unique_ptr<Qword[],PacketDelete> buffer00;
  Qword* vif04=nullptr;Qword* gif08=nullptr;Qword* giftag0C=nullptr;
  // Source descriptor+10 is UNWRITTEN. No fabricated initialized lane.
  bool descriptor10Known=false;
  std::size_t qwords=0,glyphs=0;
  // Source DMA header+4 is an actual pointer. Native pointer is retained here,
  // NEVER truncated to a guessed guest address in the word image.
  Qword* dmaData4=nullptr;
  bool dmaPointerPublished=false;
 };
 struct Primitive {
  std::int32_t material00=-1;void* optional04=nullptr;
  std::uint8_t type0C=0,field0D=0,field0E=255,flags0F=0;
  Qword* packet10=nullptr;void* optional14=nullptr;
  float scale18=1,scale1C=1;std::uint16_t field20=0,field22=0;
  bool materialStored=false,materialRetained=false;
 };
 enum class Phase : std::uint8_t {constructed,loading,coldLoaded,failed,retired};
 OriginalUiGarageScreenText50KG()noexcept=default;
 // Lower, independently testable source body only. Inputs must be the genuine
 // same328 font Entry and canonical namespace. No caller text/language/ready
 // token is accepted. The focused private aperture supplies authenticated
 // Authenticated Garage text rows supply only the explicitly represented
 // font/material/capacity tuples. The old signature is the fixed row1 body.
 bool loadCold2DFD90(const Application&,std::shared_ptr<Domain>,std::shared_ptr<const Context>,
   std::shared_ptr<Fonts>,std::shared_ptr<const Fonts::Entry>,std::shared_ptr<const Archive>,
   std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
   std::int32_t capacity,Diagnostic&)noexcept;
 bool loadCold2DFD90(const Application&,std::shared_ptr<Domain>,std::shared_ptr<const Context>,
   std::shared_ptr<Fonts>,std::shared_ptr<const Fonts::Entry>,std::shared_ptr<const Archive>,
   std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
   Fonts::Name,std::string_view materialName,std::int32_t capacity,Diagnostic&)noexcept;
 //Exact Garage1FA360 caller:denmrk16/denmrk16,capacity6,NULL initial text.
 bool loadCost1FA360(const Application&,std::shared_ptr<Domain>,std::shared_ptr<const Context>,
   std::shared_ptr<Fonts>,std::shared_ptr<const Fonts::Entry>,std::shared_ptr<const Archive>,
   std::string& actualDirectory36B970,const std::string& fontDirectory36B420,Diagnostic&)noexcept;
 bool loadColdSource2DFD90(const Application&,std::shared_ptr<Domain>,std::shared_ptr<const Context>,
   std::shared_ptr<Fonts>,std::shared_ptr<const Fonts::Entry>,std::shared_ptr<const Archive>,
   std::string&,const std::string&,Fonts::Name,std::string_view,std::int32_t,bool initialNull,Diagnostic&,std::string_view initialText={})noexcept;
 bool buildPacket2E03F0(Packet&,std::size_t firstGlyph,Diagnostic&)noexcept;
 bool loadPrimitive2465B0(Primitive&,const std::string&,Diagnostic&)noexcept;
 bool coldInputsCurrent()const noexcept;
 bool writeText29D990(std::string_view,Diagnostic&)noexcept;
 bool draw2E0170(OriginalUiGarageDrawContribution50KG&,const OriginalUiGarageTextOverlay50KG&,Diagnostic&)noexcept;
 bool draw2E0170(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
 bool drawWarning4702E0170(OriginalUiGarageDrawContribution50KG&,OriginalUiSharedDialogs50KG&,Diagnostic&)noexcept;
 bool drawPopup47C2E0170(OriginalUiGarageDrawContribution50KG&,OriginalUiSharedDialogs50KG&,Diagnostic&)noexcept;
 bool drawMenuWarning4702E0170(OriginalUiSharedDialogs50KG&,Diagnostic&)noexcept;
 struct LoweredPacket {
  std::vector<assets::Vertex48B> vertices;
  std::vector<std::array<float,3>> stq;
  std::vector<float> rasterW,fog;
  std::uint32_t quadCount=0;
 };
 struct FontAtlasBorrow {
  std::shared_ptr<const Fonts::Entry> font;
  std::shared_ptr<const Archive> archive;
  assets::OriginalUiMemberView50KG pst;
  std::int16_t canonicalTexture=-1;
 };
 bool lowerQueuedPacket(std::size_t,const std::array<std::uint32_t,2>&,LoweredPacket&,Diagnostic&)const;
 bool borrowQueuedFontAtlas(FontAtlasBorrow&,Diagnostic&)const noexcept;
 bool retainMenuWarningPacket(OriginalUiSharedDialogs50KG&,std::size_t,
   OriginalUiMenuWarningPacket50KG&,Diagnostic&)noexcept;
 bool releaseMenuWarningPacket(OriginalUiMenuWarningPacket50KG&)noexcept;
 bool lowerMenuWarningPacket(const OriginalUiSharedDialogs50KG&,
   const OriginalUiMenuWarningPacket50KG&,const std::array<std::uint32_t,2>&,
   OriginalUiMenuWarningGeometry50KG&,Diagnostic&)const;
 bool borrowMenuWarningFontAtlas(const OriginalUiSharedDialogs50KG&,
   const OriginalUiMenuWarningPacket50KG&,FontAtlasBorrow&,Diagnostic&)const noexcept;
 bool menuWarningPacketMatches(const OriginalUiSharedDialogs50KG&,
   const OriginalUiMenuWarningPacket50KG&)const noexcept;
 bool lowerRetainedPacketBody(std::size_t,const std::array<std::uint32_t,2>&,LoweredPacket&,Diagnostic&)const;
 bool borrowRetainedFontAtlasBody(FontAtlasBorrow&,Diagnostic&)const noexcept;

 bool applyOverlay29D690(std::uint32_t,std::int16_t,float,float,std::int32_t,std::int32_t,std::int32_t,Diagnostic&)noexcept;
 bool regenerate2E0F50(std::uint32_t viewportWidth,std::uint32_t viewportHeight,
   float actualVideoMaximum60,Diagnostic&)noexcept;
 bool preflightRetirement2E1340(Diagnostic&)const noexcept;
 static bool authorityThread(const std::shared_ptr<Authority>&)noexcept;
 bool retire2E1340(Diagnostic&)noexcept;
 bool stop(Diagnostic&,OriginalUiGarageScreenTextStatus50KG,std::uint32_t)noexcept;
 void releasePrimitive246260(std::unique_ptr<Primitive>&)noexcept;
 // Process BSS records, not per-root/per-widget state. Atomic storage is only
 // the native data-race envelope. These private locations remain reusable by
 // future concrete source callers, notably the shared299BE0 primitive guard.
 static std::atomic<std::uint8_t>& guard8F14()noexcept;
 static std::atomic<std::int32_t>& assertion445400()noexcept;
 static std::atomic<std::uint8_t>& guard8F18()noexcept;
 static std::atomic<std::int32_t>& assertion445408()noexcept;
 static std::atomic<std::uint8_t>& guard8F20()noexcept;
 static std::atomic<std::int32_t>& assertion445418()noexcept;
 static std::atomic<std::uint8_t>& guard8F24()noexcept;
 static std::atomic<std::int32_t>& assertion445420()noexcept;
 static std::atomic<std::uint8_t>& guard8CD8()noexcept;
 static std::atomic<std::int32_t>& assertion443640()noexcept;
 const std::thread::id ownerThread_=std::this_thread::get_id();
 const Application* issuer_=nullptr;
 std::shared_ptr<Authority> authority_;
 game::OriginalApplicationAnimationClockView50KG lifetime_;
 std::shared_ptr<Domain> domain_;
 std::shared_ptr<const Context> context_;
 std::shared_ptr<Fonts> fonts_;
 std::shared_ptr<const Fonts::Entry> font4C_;
 std::shared_ptr<const Archive> garage_;
 const assets::OriginalUiMemberIdentity50KG* materialIdentity_=nullptr;
 // Native immutable identities for the actual source input pointers, not
 // additional guest fields or a new per-row font/material namespace.
 Fonts::Name fontName_=Fonts::Name::frontendDenmrk16;
 std::string_view materialName_;
 bool initialNullInput_=false;std::size_t initializedStringBytes_=0;
 std::unique_ptr<char[]> string8_;
 std::unique_ptr<Glyph[]> glyphsC_;
 std::unique_ptr<std::unique_ptr<Primitive>[]> primitives4_;
 std::unique_ptr<Packet[]> packets64_;
 const void* source4Observation_=nullptr; //Source4 stale after2DFD00, never dereferenced.
 // Native ownership envelope for the actual2461E0 object before source4[i]
 // publication2DFAF4. Failure cannot leak its real canonical material ref.
 std::unique_ptr<Primitive> pendingPrimitive_;
 std::size_t publishedPrimitives_=0,allocatedPackets_=0;
 std::uint32_t count0_=0,justification50_=0,color54_=0;
 std::array<float,4> vector10_{{0,0,0,1}},vector20_{{0,0,0,1}},vector30_{{0,0,0,1}};
 float scale40_=1,angle44_=0,depth48_=1;
 std::int16_t alpha58_=-1;
 std::uint8_t flags5A_=3,flags5AKnown_=15,capacity5B_=0,length5C_=0,packetCount5D_=0,active5E_=0;
 // Source60 starts0BADBEEF, then borrows the SAME Font+8 cells. The native
 // immutable metrics owner replaces that address, not a fabricated integer.
 std::uint32_t sentinel60_=0x0badbeefu;
 const assets::OriginalUiFont50KG* cells60_=nullptr;
 bool cells60Published_=false,primitiveArrayFreed_=false;
 // Actual queued primitive borrows keep glyph, packet and material storage live.
 // Only the retained source FIFO may acquire/release these pins.
 std::uint32_t nativeGarageFifoPins_=0;
 // Distinct retained Main/Options packet borrows, issued/released only by Shared.
 std::uint32_t nativeMenuWarningPins_=0;
 std::uint64_t packetRevision_=0;
 Phase phase_=Phase::constructed;
public:
 ~OriginalUiGarageScreenText50KG()noexcept;
 OriginalUiGarageScreenText50KG(const OriginalUiGarageScreenText50KG&)=delete;
 OriginalUiGarageScreenText50KG& operator=(const OriginalUiGarageScreenText50KG&)=delete;
};
}
