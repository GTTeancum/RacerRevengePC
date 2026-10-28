#pragma once
#include "native_borrowed_call50du.h"
#include "native_racer_contact_status50du.h"
#include <cstdint>

namespace rr::platform {
struct VehicleWindowWallSample50DS;
struct VehicleWindowDiagnostic50DS;
}

namespace interactive_loop50DU {
// Original observation-only layouts; source objects still live in runSession.
struct GameplayDisplayWallTotals50DU {
    struct Total {
        std::uint64_t attempted=0,valid=0,wallNs=0,maxWallNs=0;
        bool overflow=false;
        void add(std::uint64_t& value,std::uint64_t amount) noexcept;
        void record(const rr::platform::VehicleWindowWallSample50DS& sample) noexcept;
    };
    struct Group {
        Total phase[3]{};
        std::uint64_t calls=0,failures=0;
        bool overflow=false;
    };
    bool enabled=false;
    Group gameplay{},menu{};
    void record(bool isMenu,bool accepted,const rr::platform::VehicleWindowDiagnostic50DS& d) noexcept;
    void reportAndReset(const char* boundary);
};

struct RacerContactTotals50HV {
  std::uint64_t passes=0,noPairs=0,testedPairs=0,geometryPairs=0,suppressedPairs=0;
  std::uint64_t respondedPairs=0,generatedRecords=0,activeRecords=0,failures=0;
  native_racer_contact50HW::Status lastStatus=native_racer_contact50HW::Status::Complete;
 };

struct RacePresentation50HZ {
  bool valid=false;
  std::uint32_t submittedOpponents=0,submittedFinishedOpponents=0;
  std::uint32_t retainedStoppedOpponents=0,standingsRows=0;
 };
} // namespace interactive_loop50DU
