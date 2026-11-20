// Ordered dispatch for the source-identical borrowed frame continuations.
#include "terrain_interactive_frame_calls50du.h"
#include "runtime_startup50fl_fixture.h"
#include <sstream>

namespace interactive_frame50DU {
using detail::require50DU;
using detail::addCounter50DU;

void Context::recordNativeResult(bool ok,const char* label) {
 addCounter50DU(nativeCalls);
 if(!ok){std::ostringstream message;message<<"native frame "<<completedFrames
  <<" "<<label<<" error="<<static_cast<unsigned>(x.diagnostic.error)
  <<" source-stage="<<std::hex<<x.diagnostic.sourceStage
  <<" body="<<static_cast<unsigned>(x.diagnostic.body);
  throw std::runtime_error(message.str());}
}

bool Context::advance(std::size_t actualRacerSlot,Part part,
    float preparedOuterFactor,float liveSubstepScale) {
 if(part==Part::Replay44_1||part==Part::Replay44_2||part==Part::Replay44_3||part==Part::Replay44_4)
  return replayDestroy228620(actualRacerSlot,static_cast<std::uint32_t>(part)-static_cast<std::uint32_t>(Part::Replay44_1)+1u,
      preparedOuterFactor);
 NativeCalls runNative{*this};
 requireRuntimeNavigation();
 require50DU(preparedOuterFactor==.016f&&liveSubstepScale==60.f,
     "isolated two-phase client retains its actual16ms one-substep domain");
 const bool sourceAiAudio=originalAiAudioEnabled&&s.fields.vehicle58==1u;
 if(part==Part::Children)
  return advanceChildren(actualRacerSlot,preparedOuterFactor,sourceAiAudio,runNative);
 return advancePhysics(actualRacerSlot,part,liveSubstepScale,sourceAiAudio,runNative);
}
} // namespace interactive_frame50DU
