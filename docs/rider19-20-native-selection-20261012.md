# Original Rider19/20 selection, source-only candidate

Baseline: `56ec194bd87591bcf642b2890a102364a0ee7897`.

This change implements the bounded adjacent original Rider19 ↔ Rider20 path.
It does not implement the remaining original roster or supply missing racers
with substitute art. No build, test, syntax/import check, asset decoder,
gameplay run, or user-computer action was performed. Human testers own
acceptance; this document is source provenance, not runtime evidence.

## Scope

- Right from original Rider19 selects original Rider20. Left from20 selects19.
  Original roster order and profile unlock/hidden rules still come from the
  authenticated Rider data. Left from19 and Right from20 remain unavailable
  when the real next eligible control is outside this prepared pair. There is
  no synthetic two-entry wrapping roster or silent skipping of unsupported
  riders.
- The shared immutable bank prepares the genuine widgets27/53 (19) and28/54
  (20), including authored pod/character geometry, body/specular/shadow images,
  original backgrounds, portraits and stat/profile projections. Live graphs
  and mutable material state are created only by source callbacks.
- Direction captures the outgoing pod's local Look/Up, performs the real
  deselect/select sequence, publishes selected rider/slot and appearance,
  suspends publication, repeats selected-control SELECT, restores orientation
  while preserving the incoming pod's position, sets deferred4EC=3, and issues
  original event2 once. Source direction does not select a voice.
- Normal Back remembers19 or20. Mode0 Confirm commits the selected rider,
  starts that character's genuine selection animation, and draws exactly one
  shared original RNG value for its actual voice range. Rider19 uses39–43;
  Rider20 uses44–45. Course entry and Course→Rider Back preserve the selection.
- Course→race remains subject to the existing actual prepared-race predicate,
  including racer19 and the prepared course. Selecting20 does not admit a
  fabricated20 race or silently race as19.

## Lifetime and failure ownership

The renderer retains the last actually published original RGBA source before
hidden incoming work can overwrite the reusable source target. Before any
outgoing graph is destroyed, the exact source renderer retires that collector
and reports real submitted-work completion. The backend consumes authenticated
row-retirement events before reconstructing the same row with a fresh epoch.
An immutable descriptor or texture cache never substitutes for a live epoch.
The next actual renderer submission revokes the prior retirement authority.

All destructive callback failures remain terminal for source work; partial
owners remain accessible to the existing explicit drain/close retry paths.
There is no graph rollback, callback replay, or hidden success return. GPU
snapshot/blend/promotion names stay owned by the renderer until their checked
release or renderer close, including failed creation/publication.

## Native deferred presentation

The first two genuine Rider updates decrement3→2→1 and submit hidden incoming
source frames. The native loop does not swap or capture those frames as newly
displayed images. On the third update the counter reaches0. Before the next
ordinary render, the frontend completes the previous hidden source, creates
the existing immutable old/incoming byte-blend pair, and runs the recovered
25D2F0 schedule. It does not re-render models to manufacture an incoming image.

The original261F00(250) yields an integer2ms sample budget. Alpha starts0,
step starts1; every sample is actually drawn, including128. Alpha advances
by the current step, clamps at128, and the step increases when the measured
source-clock interval exceeds2ms. Faster samples wait for the remainder.
The existing integer-byte native blend preserves the source's uncovered border
pixels even at128; final whole-incoming promotion is separate.

A real window/context temporarily requests and queries native swap interval1,
performs each checked completed-publication BACK copy and actual SDL swap call,
then restores its previous interval. This maps the original presentation wait
to the native window backend; a void SDL swap cannot certify physical scanout
or exact PS2 timing. No headless fake-vblank path is supplied. Missing window
presentation support rejects direction before graph/profile mutation.

PCM is serviced during the blocking effect using measured intervals on the
same native monotonic timeline and existing output fractional carry. Those
intervals move the next page-delta baseline exactly once. They do not update
the page, transport fades, model animation or shared RNG. After the effect,
the ordinary material clock is freshly sampled with the existing active/cold
exclusions, rather than reusing its pre-effect timestamp. Focus/resize/quit or
output failure during presentation stops source work and retains cleanup.

## Recovered source evidence

Original executable identity:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

- Direction1EB1F0/1EB370: capture28A530; appearance1EAFB0;
  video virtual+1C(1); control2A5A00; restore289EC0; deferred4EC=3.
  Raw source2A5A24..3C calls widget virtual+78. Pod vtable3D0200+78
  points to1D33C0: repeated SELECT, not generic show/reset.
- Retained `recovery/20261010/ui-transition-callback-reconstruction/evidence/rider-entity-update.txt`
  establishes third-update resume/drain/wait/effect request. Raw vtable words
  at3D2070/74 identify renderer244D60(1)/245090. Video virtual+1C is
  25B100, assigning+84. 261F00 sets+405 and+408;25A9C0 invokes25D2F0.
- Retained video source shows14057C video pre-flip work before same-iteration
  root RenderPre3D/Render3D. Hence the incoming image is the prior completed
  hidden render, not the next ordinary render.
- Rider constructor1E7D74 stores count2 for20;1E7E44 stores first44.
  UIACCEPT.INI Events44/45 name OCCSEL01/OCCSEL02, both IsStream=1.
  The native transport preloads those authored streams through its existing
  decoder/mixer; this source change did not execute that decoder.
- OCCSEL01.VAG:24384 bytes, SHA256
  `f826ff76992d67646856ce9baa455400e807766edf77716d228ef9f7b6931f85`,
  expected42560 mono frames at22050Hz.
- OCCSEL02.VAG:29712 bytes, SHA256
  `d0c0af3755a80d7840aa3f52b2f1903e3191c4ace12fe9b6c060bcb8573ce41e`,
  expected51884 mono frames at22050Hz.
  Frame expectations derive from retained raw headers and first end-block
  flags under the existing decoder's stop rule, not an executed decode.

## Human acceptance still required

Verify initial19,19→20→19 repeatedly, both unsupported outer directions,
correct20 portraits/model/stat colors and sound2,20 Confirm voices44/45,
20→Course→Back→20, normal Back remembering20, and returning to19 to launch
only the actually prepared race. Inspect hidden-frame publication, real final
alpha128 and whole-image promotion, restored host swap setting, uninterrupted
menu audio, no duplicated source updates/RNG draws, and cleanup after focus,
resize, quit or GPU/output failures. No result for these cases is claimed here.
