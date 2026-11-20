// Private member declarations; definitions are in vehicle_window_gl50ds_world.cpp.
// Included only in VehicleWindowGl50DS::Storage. Lower-level native GPU reuse;
// no authenticated Main/Options owner or UI presentation target participates.
// The one track particle path (1B4DC0 manager over the 27DE00 reader, every
// course): request.particle* bank and frame.particles draw. The "Gc" names are
// the shared manager kernel's type family (OriginalGcParticle*50KG), not a course.
std::uint64_t gcParticlePublication=0;
std::uint8_t gcParticleMaximumAlpha=0;
const game::OriginalGcParticleDraw50KG* frameGcParticles=nullptr;
I gcParticleModeUniform=-1;
bool validateGcParticleBank(const VehicleWindowRequest50DS& input) noexcept;
// Identity-free geometry validation of the borrowed manager draw.
bool validateParticleDraw(const game::OriginalGcParticleDraw50KG* draw,
    const VehicleWindowFrame50DS& frame,bool bankPresent,std::uint8_t maximumAlpha,
    VehicleWindowDiagnostic50DS& d) const noexcept;
bool validateGcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept;
bool drawGcParticles() noexcept;
