// Private member declarations; definitions are in vehicle_window_gl50ds_world.cpp.
// Included only in VehicleWindowGl50DS::Storage. Lower-level native GPU reuse;
// no authenticated Main/Options owner or UI presentation target participates.
std::uint64_t gcParticlePublication=0;
std::uint8_t gcParticleMaximumAlpha=0;
const game::OriginalGcParticleDraw50KG* frameGcParticles=nullptr;
I gcParticleModeUniform=-1;
// Distinct RC source authority; the vertex/run/GPU kernel below is shared.
std::uint64_t rcParticlePublication=0;
std::uint8_t rcParticleMaximumAlpha=0;
bool frameRcParticles=false;
bool validateRcParticleBank(const VehicleWindowRequest50DS& input) noexcept;
// Distinct GB source authority; the vertex/run/GPU kernel below is shared.
std::uint64_t gbParticlePublication=0;
std::uint8_t gbParticleMaximumAlpha=0;
bool frameGbParticles=false;
bool validateGbParticleBank(const VehicleWindowRequest50DS& input) noexcept;

// Distinct MC source authority; the vertex/run/GPU kernel below is shared.
std::uint64_t mcParticlePublication=0;
std::uint8_t mcParticleMaximumAlpha=0;
bool frameMcParticles=false;
bool validateMcParticleBank(const VehicleWindowRequest50DS& input) noexcept;

bool validateGcParticleBank(const VehicleWindowRequest50DS& input) noexcept;
// Identity-free geometry validation reused only after each typed owner gate.
// Existing GC conditions and diagnostics remain byte-for-byte in this body,
// with only its already-selected bank presence/alpha bound parameterized.
bool validateParticleDraw(const game::OriginalGcParticleDraw50KG* draw,
    const VehicleWindowFrame50DS& frame,bool bankPresent,std::uint8_t maximumAlpha,
    VehicleWindowDiagnostic50DS& d) const noexcept;
bool validateGcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept;
bool validateRcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept;
bool validateGbParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept;
bool validateMcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept;
bool drawGcParticles() noexcept;
