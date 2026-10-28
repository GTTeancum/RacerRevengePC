// Exact existing non-template bodies, outlined without owner or layout changes.
#include "ra_cold_aggregate50la.h"
#include "original_race_pause_world_authority50la.h"

namespace ra_cold_aggregate50LA {
bool Owner::fail(Diagnostic& d,const char* why) noexcept {staticFog_.reset();staticLighting_.reset();terminal_=true;d.failure=why;return false;}
Owner::~Owner(){if(!close())std::terminate();}
} // namespace ra_cold_aggregate50LA

namespace rr::game {
bool OriginalRacePauseWorldAuthority50LA::current(
    const ra_cold_aggregate50LA::Owner& world,
    const std::shared_ptr<OriginalApplicationEntities50LA>& application,
    platform::RaceAudioGroups50IC* const& audio,const std::uint64_t& generation)noexcept {
    return world.worldColdConstructed_&&
        world.worldApplication_==application&&world.worldAudio_==audio&&
        world.worldEntity_.sessionGeneration==generation;
}
} // namespace rr::game
