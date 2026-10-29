#pragma once
// Stable native references only; independent of Operation and Session factories.
#include "ra_cold_roster_storage50la.h"
#include "runtime_startup50fl_fixture.h"
#include "native_racer_handling50hw_fixture.h"
#include "native_ai_session50iv_fixture.h"
#include "original_ai_physical_recovery.h"
#include "lap_drive50gj_fixture.h"
#include "effect_cold50bj.h"
#include "reset_effects50bk.h"
#include "terrain_probe_authoring50fq_fixture.h"
namespace ra_cold_roster50LA {
// Explicit stable arena borrows. This wrapper is never Bank-snapshotted. All
// persistent referred-to lanes must already be registered, with fixed capacity.
struct Arena {
    runtime_startup50fl::Fixture& live;
    native_racer50HW::HandlingTuning tuning;
    const g::AuthoredSelectionContext49N& selection;
    g::AiRuntimeState50IV& ai;
    native_ai50IV::Session& aiSession;
    g::TrackNavigationProgress50EU& navigation;
    lap_drive50GJ::Continuity& lap;
    g::BasePoseHistoryState50Y& history;
    g::PodSceneRebaseModel50FB (&rebaseModels)[5];
    unsigned& rebaseCount;
    g::PodSceneRebaseWorkspace50FB& rebaseWorkspace;
    g::ModelBoundsResult49O (&rebaseBounds)[5];
    g::PodSceneRebaseDiagnostic50FB& rebaseDiagnostic;
    a::EffectNames50BL& effectNames;
    g::EffectOwner50BE (&effects)[14];
    std::size_t& effectCount;
    g::ResetEffectBindings50BK& resetBindings;
    g::ResetEffectState50BK& resetState;
    g::ResetEffectWorkspace50BK& resetWorkspace;
    native_original_ai_recovery::Models& recoveryModels;
    const terrain_probe_authoring50fq_fixture::SourceBindings& terrainBindings;
    a::PodFlameOwners50W& flame;
    // Real application launch kind, not a caller's invented race phase.
    const std::uint32_t& launchKind11258;
    // Actual application role split: zero for legacy ALL8, one for normal Course.
    // Capture it once, outside the per-racer Bank. Human preset configuration
    // and native AI ownership remain separate from this source roster prefix.
    std::uint32_t humanCount=0;
    g::AuthoredBinding49N* selectedAuthored=nullptr;
    float* selectedSteering=nullptr;
};
} // namespace ra_cold_roster50LA
