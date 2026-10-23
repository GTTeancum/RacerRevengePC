#pragma once
#include "original_application_animation_clock50kg.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <thread>
namespace original_main_entry50KG {struct Application;struct Video;}
namespace rr::platform {class OriginalScreenRain50KG;}
namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalVideoPreFlipObject50KG;
enum class OriginalVideoPreFlipStatus50KG : std::uint8_t {
 complete,authority,phase,backing,allocation,capacity,nativeLifetime
};
struct OriginalVideoPreFlipDiagnostic50KG {
 OriginalVideoPreFlipStatus50KG status=OriginalVideoPreFlipStatus50KG::authority;
 std::uint32_t sourcePC=0;
 bool sourceEffectsCommitted=false;
};
// Actual Video+18 ContainerList constructor and borrowed membership. This is
// NOT a callback dispatcher or a witness of complete source registration history.
class OriginalVideoPreFlipList50KG final {
 friend struct ::original_main_entry50KG::Application;
 friend struct ::original_main_entry50KG::Video;
 friend class OriginalVideoPreFlipObject50KG;
 using Application=::original_main_entry50KG::Application;
 using Video=::original_main_entry50KG::Video;
 struct Slot {OriginalVideoPreFlipObject50KG* value=nullptr;bool known=false;};
 struct LazyRecord {std::atomic<std::uint8_t> guard{0};std::atomic<std::int32_t> assertion{0};};
 struct Records {LazyRecord size8D14,empty8D18,initialized8D1C,nonnull8D20;};
 static Records& records()noexcept;
 static bool firstUse(LazyRecord&)noexcept;
 OriginalVideoPreFlipList50KG(const Video&,const Application&)noexcept;
 void bindPublishedVideo(const std::shared_ptr<Video>&)noexcept;
 static bool applicationVideoEntryCurrent(const Application&)noexcept;
 static bool applicationMayRetire(const Application&)noexcept;
 bool current()const noexcept;
 bool retainMembership(OriginalVideoPreFlipDiagnostic50KG&)noexcept;
 void releaseMembership()noexcept;
 bool append2AE370(OriginalVideoPreFlipObject50KG&,OriginalVideoPreFlipDiagnostic50KG&)noexcept;
 bool grow2AE550(std::int32_t,OriginalVideoPreFlipDiagnostic50KG&)noexcept;
 bool remove2AE4D0(OriginalVideoPreFlipObject50KG&)noexcept;
 const Application* issuer_=nullptr;
 const Video* video_=nullptr;
 std::weak_ptr<Video> issuedVideo_;
 std::weak_ptr<OriginalApplicationEntities50LA> authority_;
 OriginalApplicationAnimationClockView50KG lifetime_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool bound_=false;
 //Native lifetime ledger includes constructed bases whose append failed.
 //It is neither the source list count nor a guest Entity/reference field.
 std::uint32_t liveMemberships_=0;
 //2AE1E0 initializes pointer/count/growth/capacity and only low flag bit.
 std::unique_ptr<Slot[]> slots4_;
 std::int32_t count0_=0,growth8_=0,capacityC_=0;
 bool initialized10Bit0_=false;
public:
 struct Observation {
  std::int32_t count=0,growth=0,capacity=0;
  bool initialized=false,backing=false,registrationHistoryEstablished=false;
 };
 ~OriginalVideoPreFlipList50KG()noexcept;
 OriginalVideoPreFlipList50KG(const OriginalVideoPreFlipList50KG&)=delete;
 OriginalVideoPreFlipList50KG& operator=(const OriginalVideoPreFlipList50KG&)=delete;
 bool inspect(Observation&)const noexcept;
};
// Real uncounted VideoPreFlipObject base membership. Only the future concrete
// ScreenRain owner may enter its source constructor/retirement. Tests use an
// explicit private aperture, not an actual ScreenRain or no-op callback.
class OriginalVideoPreFlipObject50KG final {
 friend class OriginalVideoPreFlipList50KG;
 friend class rr::platform::OriginalScreenRain50KG;
 OriginalVideoPreFlipObject50KG()noexcept=default;
 bool construct2ADFD0(OriginalVideoPreFlipList50KG&,OriginalVideoPreFlipDiagnostic50KG&)noexcept;
 bool retire2AE010(OriginalVideoPreFlipDiagnostic50KG&)noexcept;
 struct SourceVtable {
  bool (OriginalVideoPreFlipObject50KG::*retire8)(OriginalVideoPreFlipDiagnostic50KG&)noexcept;
  bool (*callbackC)(OriginalVideoPreFlipObject50KG&,OriginalVideoPreFlipDiagnostic50KG&)noexcept;
 };
 static const SourceVtable& baseVtable3D44D0()noexcept;
 const SourceVtable* vtable0_=nullptr;
 OriginalVideoPreFlipList50KG* list_=nullptr;
 std::shared_ptr<::original_main_entry50KG::Video> video_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool constructed_=false,inserted_=false,retired_=false;
public:
 ~OriginalVideoPreFlipObject50KG()noexcept;
 OriginalVideoPreFlipObject50KG(const OriginalVideoPreFlipObject50KG&)=delete;
 OriginalVideoPreFlipObject50KG& operator=(const OriginalVideoPreFlipObject50KG&)=delete;
};
}
