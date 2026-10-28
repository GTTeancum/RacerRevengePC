#pragma once
#include "original_ui_archive50kg.h"
#include "../game/original_application_animation_clock50kg.h"
#include <array>
#include <atomic>
#include <memory>
#include <string_view>
#include <string>
#include <thread>
#include <vector>
namespace original_main_entry50KG { struct Application; }
namespace rr::platform { class OriginalLocalizationRendererLease50KG; class OriginalUiGarageWidget1Owner50KG; class OriginalUiGaragePageOwner50KG; class OriginalUiSharedDialogs50KG; }
namespace rr::game { class OriginalApplicationEntities50LA; class OriginalResults10450LA; }
namespace rr::assets {
class OriginalStandardArchiveProvider50LA;
enum class OriginalApplicationLocalizationStatus50KG : std::uint8_t {
 complete, authority, archive, shape, phase, allocation, precedingFonts, rendererPending,
 selectedBorrowRequired, selectedBorrowLive
};
struct OriginalApplicationLocalizationDiagnostic50KG {
 OriginalApplicationLocalizationStatus50KG status=OriginalApplicationLocalizationStatus50KG::complete;
 std::uint32_t sourcePC=0;
 std::size_t ordinal=0;
 bool sourceOwnerPublished=false;
};
// Actual Application32C GameText owner, NOT OriginalUiEnglish50KG's decoded
// convenience table. Fixed configured startup-English selection can continue
// only through the private actual-root completed renderer invocation. This is
// not full17DE80, mutable36ADC0, profile language or mount chronology.
class OriginalApplicationLocalization50KG final {
 friend struct ::original_main_entry50KG::Application;
 friend class rr::platform::OriginalLocalizationRendererLease50KG;
 using Application=::original_main_entry50KG::Application;
 using Authority=game::OriginalApplicationEntities50LA;
 struct Row {const char* text0=nullptr;std::int16_t maximum4=0;bool textKnown=false;};
 struct Node {std::int32_t ordinal0,hash4;const char* key8;Node* nextC;};
 struct Hash {
  std::unique_ptr<Node*[]> buckets0;
  std::int32_t count4=0;
  std::uint32_t (*hash8)(std::string_view)noexcept=nullptr;
  bool (*compareC)(const char*,std::string_view)noexcept=nullptr;
  std::vector<std::unique_ptr<Node>> ownedNodes;
 };
 enum class Phase : std::uint8_t {constructed,indexed,selected,retired};
 explicit OriginalApplicationLocalization50KG()noexcept=default;
 static bool stage(const Application&,std::shared_ptr<Authority>,
     std::shared_ptr<const OriginalUiArchive50KG>,
     std::shared_ptr<OriginalApplicationLocalization50KG>&,
     OriginalApplicationLocalizationDiagnostic50KG&)noexcept;
 bool loadIndex14BC30(OriginalApplicationLocalizationDiagnostic50KG&)noexcept;
 bool selectEnglish14C5F0(const Application&,OriginalApplicationLocalizationDiagnostic50KG&)noexcept;
 // Native unpublished staging only. No absent service is treated as authority.
 // The real select wrapper checks its exact completed call before preparing
 // and again before the nonthrowing publication kernel. Private CPU tests can
 // exercise these two lower pieces, not positive production service issuance.
 struct PreparedEnglish {
  std::unique_ptr<char[]> buffer;
  std::string lookup;
  std::array<std::size_t,467> offsets{},lengths{};
  const OriginalApplicationLocalization50KG* issuer=nullptr;
  std::weak_ptr<OriginalApplicationLocalization50KG> issuedOwner;
  OriginalUiMemberView50KG member{};
  std::uint64_t providerRevision=0;
 };
 bool firstEnglishPrefixCurrent()const noexcept;
 bool prepareFirstEnglish14C5F0(PreparedEnglish&,OriginalApplicationLocalizationDiagnostic50KG&)noexcept;
 bool publishFirstEnglish14C690(PreparedEnglish&&,OriginalApplicationLocalizationDiagnostic50KG&)noexcept;
 bool selectedRowsCurrent()const noexcept;
 static bool scanEnglishBin(const OriginalUiMemberView50KG&,
     std::array<std::size_t,467>&,std::array<std::size_t,467>&,
     OriginalApplicationLocalizationDiagnostic50KG&)noexcept;
 bool sourceText18AC60(std::int32_t,const char*&,
     OriginalApplicationLocalizationDiagnostic50KG&)const noexcept;
 bool retire(const Application&,OriginalApplicationLocalizationDiagnostic50KG&)noexcept;
 // Cleanup retains exact identity/lifetime, not mutable mount lookup eligibility.
 bool cleanupIdentityCurrent()const noexcept;
 // Process BSS storage, not per-Application/per-GameText state. Atomics are
 // a native race-safety envelope, not claimed original synchronization.
 static std::atomic<std::uint8_t>& countGuard83E4()noexcept;
 static std::atomic<std::int32_t>& countAssertion427078()noexcept;
 static std::atomic<std::uint8_t>& maximumGuard83F4()noexcept;
 static std::atomic<std::int32_t>& maximumAssertion427098()noexcept;
 static std::atomic<std::uint8_t>& selectedGuard80A0()noexcept;
 static std::atomic<std::int32_t>& selectedAssertion3D6B20()noexcept;
 static std::uint32_t hash2D3A80(std::string_view)noexcept;
 static bool compare2D3AB0(const char*,std::string_view)noexcept;
 static bool bucketFor(std::uint32_t,std::uint32_t,std::size_t&)noexcept;
 const Application* issuer_=nullptr;
 std::weak_ptr<Authority> authority_;
 game::OriginalApplicationAnimationClockView50KG lifetime_;
 std::weak_ptr<OriginalApplicationLocalization50KG> issuedOwner_;
 std::shared_ptr<const OriginalUiArchive50KG> archive_;
 std::weak_ptr<const OriginalUiArchive50KG> issuedArchive_;
 std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
 std::weak_ptr<OriginalStandardArchiveProvider50LA> issuedProvider_;
 std::string lookupName_;
 const OriginalUiMemberIdentity50KG* indexIdentity_=nullptr;
 std::thread::id ownerThread_=std::this_thread::get_id();
 Phase phase_=Phase::constructed;
 //14BB60 only produces00/04/24/28/2C=null and signed16 count10=0.
 // Native zero/default storage is not a claim that other source bytes were set.
 std::unique_ptr<char[]> text00_;
 std::unique_ptr<Row[]> rows04_;
 std::int16_t count10_=0;
 std::array<char,18> base12_{};
 bool baseKnown_=false;
 std::unique_ptr<const char*[]> names24_;
 std::unique_ptr<char[]> names28_;
 std::unique_ptr<Hash> hash2C_;
 std::size_t nameBytes_=0;
 // Native selection/lifetime bookkeeping, NOT additional GameText fields.
 // There is no guest selected-language integer written by14C5A0.
 const OriginalUiMemberIdentity50KG* textIdentity_=nullptr;
 std::string textLookupName_;
 std::size_t textBytes_=0,publishedTextRows_=0;
 std::array<std::size_t,467> textOffsets_{},textLengths_{};
 std::uint64_t selectionEpoch_=0;
 std::uint32_t selectedBorrowCount_=0;
public:
 class SelectedText final {
  friend class OriginalApplicationLocalization50KG;
  friend struct ::original_main_entry50KG::Application;
  SelectedText(std::shared_ptr<OriginalApplicationLocalization50KG>,std::int32_t,
      std::uint64_t,const char*,std::size_t)noexcept;
  bool currentOwnedBy(const std::shared_ptr<const SelectedText>&,const Application&)const noexcept;
  bool readCurrent(const std::shared_ptr<const SelectedText>&,const Application&,
      std::string_view&)const noexcept;
  const std::shared_ptr<OriginalApplicationLocalization50KG> owner_;
  std::weak_ptr<const SelectedText> issued_;
  const std::int32_t ordinal_;
  const std::uint64_t epoch_;
  const char* const text_;
  const std::size_t bytes_;
  bool registered_=false;
 public:
  // A concrete caller may validate/read an already-issued retained token, but
  // receives no friendship to token construction, fields or issuance state.
  class BorrowReader final {
   friend class rr::game::OriginalResults10450LA;
   friend class rr::platform::OriginalUiGarageWidget1Owner50KG;
   friend class rr::platform::OriginalUiGaragePageOwner50KG;
   friend class rr::platform::OriginalUiSharedDialogs50KG;
   static bool readCurrent(const std::shared_ptr<const SelectedText>& token,
       const Application& root,const OriginalApplicationLocalization50KG& expected,
       std::int32_t ordinal,std::string_view& out)noexcept {
    return token&&token->owner_.get()==&expected&&token->ordinal_==ordinal&&
        token->readCurrent(token,root,out);
   }
  };
  ~SelectedText()noexcept;
  SelectedText(const SelectedText&)=delete;
  SelectedText& operator=(const SelectedText&)=delete;
 };
private:
 bool borrowText18AC60(const Application&,std::int32_t,std::shared_ptr<const SelectedText>&,
     OriginalApplicationLocalizationDiagnostic50KG&)noexcept;
public:
 struct Observation {
  std::int16_t count=0;
  std::size_t rowAllocationBytes=0,nameBytes=0,namePointerAllocationBytes=0;
  std::size_t publishedNames=0,publishedNodes=0;
  bool baseKnown=false,indexLoaded=false,textBufferKnownNull=false,textPointersProduced=false;
  const OriginalUiMemberIdentity50KG* indexIdentity=nullptr;
  std::uint32_t pendingSourcePC=0;
  const OriginalUiMemberIdentity50KG* textIdentity=nullptr;
  std::size_t selectedBytes=0,publishedTextRows=0;
  std::uint32_t nativeSelectedBorrows=0;
 };
 ~OriginalApplicationLocalization50KG()noexcept;
 OriginalApplicationLocalization50KG(const OriginalApplicationLocalization50KG&)=delete;
 OriginalApplicationLocalization50KG& operator=(const OriginalApplicationLocalization50KG&)=delete;
 bool current()const noexcept;
 bool inspect(Observation&)const noexcept;
 // Exact20ED30 hash-chain lookup. Case-sensitive strcmp equivalence; misses
// leave output unchanged. This returns an ID, NEVER selected/localized text.
 bool lookup20ED30(std::string_view,std::int32_t&)const noexcept;
 // Compatibility observation remains closed even after selection. Positive
 // text access requires the private-issued retained SelectedText capability.
 bool selectedText18AC60(std::int32_t,const char*&,OriginalApplicationLocalizationDiagnostic50KG&)const noexcept;
};
}
