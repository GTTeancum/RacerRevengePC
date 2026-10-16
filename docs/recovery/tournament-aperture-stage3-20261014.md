# Stage 3: original Tournament aperture lifecycle

Source-only; no build, test, syntax check or launch. New changes are confined
to src/game/original_ui_aperture_owner50kg.h and .cpp plus this report. No
frontend admission, profile/result implementations, or mode0 Course launch
guards changed.

## Implemented owner behavior

- bindTournament authenticates archive tournament12 and follows ui.dat to the
  real standing.dat. It retains that archive through the existing owner.
- Source1F2E30 entry is a no-op. The actual popup-inactive render branch
  1F2DB8..1F2DF8 starts opening only while aperture4B4 is closed and actual
  manager170 is entry1. Active-popup branches stay unavailable in this API.
- Opening callback is1EEB50; opened is1EEBA0; closing is1EEBE0; threshold is
  1EEC30. Existing native source1D7550 arithmetic/geometry/overlay owner is
  reused with the genuine authored OpenSize69/Time0.5.
- saveTournamentUpdateDelta38 supplies the actual source dt298. Source timer
  subtraction4064 before that store remains the future session's responsibility.
- Source1F2F00 entry completion returns complete immediately, including while
  closed. Exit completion returns complete only while closed; other pending
  states set the retained tournamentExitRequested4060 bit. Session popup/save
  branching must read this owner field, rather than keeping a divergent copy.
- Existing variants keep their branch paths and callback identities; the new
  boolean argument defaults false. No execution-based verification was run.

The ordinary common poll remains read-only; the Tournament branch explicitly
preserves its original poll mutation. A trace-only callback cannot satisfy the
existing available() contract. Binding this aperture still does not certify
models, draw contributions, destination ownership or the whole Tournament UI.
The initial page-entry74 sequence itself is not implemented by this owner.
Fresh per-screen construction starts4060 false, matching entry1EEAD0; if a
future session reuses an owner across a new entry74, it must add the proven
entry reset boundary rather than assume reuse works.

## Current profile contract from root

armTournamentResultForCommittedRace(selection, stamp,
    const actual LapTimingManager50ID&, actual OriginalUiMainApplication50KG&)
requires real manager flags58&3 == 2 (active and unended), retaining manager
and application identity. Result commit requires flags58&3 == 3 and the same
owners. tournamentPodCareerSnapshot(C,P,row) exposes actual off-diagonal data.
These methods are not reimplemented here. Their exact cloud declarations must
be delivered before the final session can compile against the canonical API.

Remaining full-screen work: native Tournament dynamic text/trophy projection,
23 User_Rotate pod model ownership, popup/save/reopen callbacks, complete
source graph submission and deferred renderer retirement. Existing mode0
Course guards stay closed until those owners and genuine mode2 launch receipt
are joined. Garage remains unsupported.

## Root integration review

This is a deliberately restricted aperture API. Original 1F28D0 may also reach
1F2DB8 with an active popup when actual exitRequested4060 is nonzero; the current
API does not admit that branch. Full session integration must preserve that
original route and the genuine entry74 reset before claiming complete behavior.
Both new Tournament input/page translation units are registered in native CMake.
No build or smoke was run for this integration.
