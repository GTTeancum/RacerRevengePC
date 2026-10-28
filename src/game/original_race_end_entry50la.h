#pragma once
#include <cstdint>

namespace rr::game {
// Observes one private1A7DD0 invocation. callerReturned denotes only the
//supported same-invocation epilogue, not end admission, all-branch support,
//Results acknowledgment, world load readiness or a race reward.
struct OriginalRaceEndEntryDiagnostic50LA {
    enum class Status:std::uint8_t {owner,profile,source,incomplete,callerReturned};
    Status status=Status::owner;
    std::uint32_t sourcePC=0x1a7dd0;
    //False when outer identity/thread rejection prevented reading the cursor;
    //then the committed flags below make no assertion about an earlier invocation.
    bool reachedStateKnown=false;
    bool f8VisibilityCommitted=false,resultsActivationCommitted=false;
    bool hudStopCommitted=false;
    bool recordBit7E4Committed=false,rivalWord7E8Committed=false;
    bool acknowledgement20CRead=false;
    // Reached effects of the zero20C branch, not finished end dispatch/input.
    // ScalarReached includes the unchanged-mode case, which performs no stores.
    bool waitingBit5Committed=false,resultsModeScalarReached=false;
    bool resultsModeFeedbackReturned=false;
    //Only1AB520's actual epilogue returned. The caller's later78/root tail
    //may still be incomplete; this never means full1A7B40 or race completion.
    bool dispatchReturned1AB520=false;
    bool callerReturned1A7B40=false;
    std::uint32_t callerReturnV0=0; //Meaningful only when callerReturned1A7B40.
    std::uint32_t observedAcknowledgement20C=0; //Meaningful only when Read=true.
    const char* operation=nullptr;
};
}
