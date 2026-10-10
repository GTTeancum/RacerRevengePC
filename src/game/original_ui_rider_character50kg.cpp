#include "original_ui_rider_character50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
bool fail(ModelPlaybackDiagnostic50KG& d,std::uint32_t pc) noexcept {
    d={};d.error=ModelPlaybackError50KG::input;d.sourceStage=pc;return false;
}
float multiply(float a,float b) noexcept {volatile float result=a*b;return result;}
std::int32_t choice(CoupledNoiseContext49M& random,float scale) noexcept {
    // Source308030, CVT.S.W, multiply2^-31, multiply5/3, add1, CVT.W.S.
    // Native conversion policy matches the existing game's truncating CVT.W.S
    // projections. Retain float rounding; a high draw can become exactly1.0.
    const auto raw=drawOriginalSharedRng50IU(random);
    volatile float value=static_cast<float>(raw);
    const float normalized=multiply(0x1p-31f,value);
    volatile float result=1.0f+multiply(scale,normalized);
    return static_cast<std::int32_t>(result);
}
}
bool OriginalUiRiderCharacter50KG::bind(const assets::OriginalUiRiderCharacterAssets50KG& source) noexcept {
    if (assets_||!source.loaded()||!source.baseName()) return false;
    try {
        const char* suffix[]{"_ent","_sel","_breathe","_idl1","_idl2","_idl3"};
        std::array<ModelPlaybackMotion50KG,6> descriptors{};
        for (std::size_t i=0;i<descriptors.size();++i) {
            const auto* motion=source.findMotion(*source.baseName()+suffix[i]);
            if (!motion||!std::isfinite(motion->motion.duration)||motion->motion.duration<=0) return false;
            descriptors[i]={&motion->motion,motion->looping};
        }
        motions_=descriptors;assets_=&source;return true;
    } catch (...) {return false;}
}
bool OriginalUiRiderCharacter50KG::startEntry(const assets::MotionNodeBindings49O& bindings,
    const SceneGraph49P& graph,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,
    ModelPlaybackWorkspace50KG& workspace,ModelPlaybackDiagnostic50KG& diagnostic) noexcept {
    if (!assets_||loaded_||entryStarted_) return fail(diagnostic,0x1D22A0);
    // The shared leaves are transactional individually. On a later failure,
    // the manager must dispose of this unpublished real model, not retry it as
    // loaded or expose a partially constructed preview.
    if (!startModelPlayback50KG(motions_[0],playback_,diagnostic)) return false;
    ModelPlaybackIk50KG beforeEndpointRegistration{};
    float initial;const std::uint32_t bits=0x3a83126fu;std::memcpy(&initial,&bits,4);
    if (!advanceModelPlayback50KG(initial,bindings,graph,beforeEndpointRegistration,
            playback_,shared,ikShared,workspace,diagnostic)) return false;
    entryStarted_=true;return true;
}
bool OriginalUiRiderCharacter50KG::captureAfterAttachment(const assets::MotionNodeBindings49O& bindings,
    const SceneGraph49P& graph,ModelPlaybackDiagnostic50KG& diagnostic) noexcept {
    if (!entryStarted_||loaded_||captured_) return fail(diagnostic,0x1D24A8);
    if (!captureModelPlaybackMatrices50KG(bindings,graph,playback_,diagnostic)) return false;
    captured_=true;return true;
}
bool OriginalUiRiderCharacter50KG::modelAttached() noexcept {
    if (!entryStarted_||!captured_||loaded_) return false;
    loaded_=true;return true;
}
bool OriginalUiRiderCharacter50KG::confirm(const assets::MotionNodeBindings49O& bindings,
    const SceneGraph49P& graph,ModelPlaybackDiagnostic50KG& diagnostic) noexcept {
    diagnostic={};if (!loaded_) return true;
    if (!transitionModelPlayback50KG(motions_[1],1.0f,bindings,graph,playback_,diagnostic)) return false;
    selectionRequested_=true;ready_=false;return true;
}
bool OriginalUiRiderCharacter50KG::advance(float dt,const assets::MotionNodeBindings49O& bindings,
    const SceneGraph49P& graph,const ModelPlaybackIk50KG& ik,CoupledNoiseContext49M& random,
    ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,ModelPlaybackWorkspace50KG& workspace,
    ModelPlaybackDiagnostic50KG& diagnostic) noexcept {
    diagnostic={};if (!loaded_) return true;
    if (!std::isfinite(dt)||dt<0||std::fegetround()!=FE_TONEAREST||!playback_.current18||
        !playback_.current18->motion) return fail(diagnostic,0x1D2730);
    const float duration=playback_.current18->motion->duration;
    if (!std::isfinite(duration)||duration<=0||!std::isfinite(playback_.time34))
        return fail(diagnostic,0x1D27C0);
    volatile float ratio=playback_.time34/duration;
    if (ratio>0.8f) {
        const bool oldReady=ready_;
        ready_=true;
        if (!selectionRequested_) {
            const float blend=multiply(.25f,duration);
            std::uint8_t next=0;
            if (!(idleCount_&0x80000000u)&&idleCount_>=6) {
                const auto probability=choice(random,5.0f);
                if (probability>0&&probability<3) {
                    const auto idle=choice(random,3.0f);
                    if (idle<4&&idle>0&&idle!=lastIdle_) {
                        lastIdle_=static_cast<std::uint8_t>(idle);
                        ready_=false;idleCount_=0;
                        next=static_cast<std::uint8_t>(2+idle);
                    }
                }
            }
            if (ready_&&!oldReady) next=2; // actual stem_breathe
            if (next&&!transitionModelPlayback50KG(motions_[next],blend,bindings,graph,playback_,diagnostic))
                return false;
            if (ready_) ++idleCount_; // source32-bit wrap, per observation, not loop
        }
    }
    // UI/RNG changes above are not rolled back on a later native failure.
    // Stop publication on false; do not consume the same RNG chronology twice.
    return advanceModelPlayback50KG(dt,bindings,graph,ik,playback_,shared,ikShared,workspace,diagnostic);
}
}
