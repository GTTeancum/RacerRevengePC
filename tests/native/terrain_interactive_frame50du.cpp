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
 NativeCalls runNative{*this};
 requireMbRuntimeNavigation50MB();
 require50DU(preparedOuterFactor==.016f&&liveSubstepScale==60.f,
     "isolated two-phase client retains its actual16ms one-substep domain");
 const bool sourceAiAudio=originalAiAudioEnabled&&s.fields.vehicle58==1u;
 if(part==Part::Children)
  return advanceChildren(actualRacerSlot,preparedOuterFactor,sourceAiAudio,runNative);
 return advancePhysics(actualRacerSlot,part,liveSubstepScale,sourceAiAudio,runNative);
}
} // namespace interactive_frame50DU
