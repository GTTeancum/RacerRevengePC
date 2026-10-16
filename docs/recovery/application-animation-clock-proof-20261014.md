# Application animation clock and pause proof

Source-only recovery, 2026-10-14. Repository inspected at the supplied a67bcaf tree. No production edits, tests, builds, syntax/import checks, project programs, remote-computer work, guest execution or emulation. Original ELF inspected only as data using readelf/od and the existing external awk field formatter.

## Result

The real animation owner is the Application reached through **global382B80**. Its current timestamp is uint32 **230**, its material-pause word is **11C**, and its delayed re-pause word is **120**. Global3E1A70 is the separately constructed file/cache service (13FB4C..6C), not this application clock owner.

There is already a recovered native typed projection: `src/game/frame_timing49i.h` FrameTimingState49I has current230, previous234, raw250, lastDispatched23C, phase22C, active11C and pending120. Its historical neutral name active11C must not obscure the independently proven29AC90 pause gate. The existing actual production application's retained host clock and Video are the legitimate place to extend publication. A new per-screen clock, owner, epoch or inverse-activity pause would be wrong.

**No already-canonical application tick/pause pair exists in the inspected UI route.** There is an existing race timestamp publication, described below, but neither it nor the local physics FrameTimingState49I may be certified as an existing process-wide source pair.

## Original writers and numeric units

Application constructor13FB30 uses s0=actual this. It clears previous234 at13FC40, current230 at13FC44, pause11C at13FC4C and pending120 at13FC50. These are real initial zeros; an uninitialized/default UI clock still cannot claim a successful sampled timestamp.

PodGame slot30 trampoline17DE00 jumps to140AC0. That routine publishes time BEFORE the manager/root update, independently of the11C material-pause value:

1. Read previous234 at140B10. If zero, call140D70 at140B1C. Otherwise sample in140B30..140BE0.
2. Original count acquisition reads 16-bit COUNT at10000000, previous sample gp8850 (3D61C0), pending overflow435368; then clears pending. Accumulated uint64 ticks435370 are updated with the recovered signed-32 increment recurrence.
3. Convert accumulated ticks via2EAE10, binary32 multiply by3AE38E39 (~1/576), then310F38 truncating/saturating uint32 conversion. Write current230 at140BE0. Original nominal timer576000 ticks/sec makes this a millisecond timestamp, with49J quantization and saturation, NOT ideal wrapping wall milliseconds or a frame counter.
4. The initialization/reset duplicate140D70 performs the same sample and conversion, then writes previous234 at140E38 BEFORE current230 at140E3C.
5.140BEC..140C30 computes unsigned wrapping current-minus-previous, converts to float and multiplies3A83126F (~.001). raw250 retains uncapped seconds. The ordinary .06 cap and optional423ED8 cap affect update dt only, not current230.
6.128080 manager call140C90 precedes the flags228 read140C98. Bit3 suppresses the subsequent root update; it does not prevent time publication. Otherwise phase22C=2 then factor23C, root26C slot38 callback, phase22C=0, Application virtual28, global26F910 occur.
7. After267BE0, read live current230 at140D2C, write previous234 at140D30. If live pending120 is nonzero, write11C=1 at140D44, then clear120 at140D48.

Existing exact numeric helpers: `clock_quantization49j.{h,cpp}` and `frame_timing49i.{h,cpp}`. Existing HOST backend `lap_host_clock50id.h` explicitly converts one retained steady-clock epoch through576000-tick mapping and49J. It is a permitted native backend with honest host provenance, not recovery of console sample values. Do not simulate timer MMIO or call a guest oracle.

## Independently proven pause writers

- Constructor13FC4C:11C=0;13FC50:120=0.
- PodGame event handler17CBC0, actual this saved as s2: gated by flags228 bit0, enabled child flags29, and event+4=0. Input matcher call17CC64 uses arguments9/0/0C/0. On success,17CC74..84 writes11C=1-old11C. Do not assign an unproven physical key label.
- The alternate matcher17CCA0 uses8/0/3F/0. If matched and11C nonzero,17CCC0 writes120=1, then17CCC4 clears11C. Combined with140D44 this is a bounded one-update/re-pause path.
-1BE8A0 loads actual382B80 and writes11C=1 at1BE8C8, before its further UI/audio actions.1BE830 clears the same11C at1BE848 before race/UI continuation. This is a concrete enter/leave pair, not a focus heuristic.
-1A9240's specific destination cases clear actual382B80->11C: case1 conditional on app2E0 at1A92A0; cases2/3/4 at1A92E8,1A92FC,1A9310.
-1CA698 also clears actual382B80->11C before1AC440 continuation.

These are directly verified source writers, not a claim that every indirect alias write across the executable is excluded. Numerous unrelated objects also have offset11C; a displacement scan alone is not proof of an Application writer. No evidence supports flags228, zero dt, focus, audio pause, minimized state or !advanceAnimations as this word.

## Exact29AC90 read/dispatch/commit order

1. Read controller08; load global382B80 and its current230 (29AC9C..ACAC). Equal stamps return at29ACB0 without effects.
2. Read that Application11C at29ACB8. Nonzero skips unless controller animatesWhilePaused18 is nonzero.
3. Invoke callback00(context04, controller) if present (29ACDC..ACF0).
4. If callback result is nonzero, update texture leafC via29A020, then vertex10 virtual10, then pixel14 virtual10, in that order.
5. **Even a zero callback result reaches the final stamp write.**29ACF4 branches to29AD48, not to the early return. At29AD48..54 reload global382B80 and its LIVE230, then write controller08 at29AD58. Do not say only a successful leaf update can stamp.

The texture leaf itself reads Application230 at29A118 to calculate delta from leaf10, and re-reads Application230 at29A16C before writing leaf10 at29A174. A previous leaf10=FFFFFFFF starts with zero delta. The frame count is the real resolved aggregate20. The controller stamp sentinel is alsoFFFFFFFF; do not clamp/replace the application value to avoid equality. An initial saturated sample can legitimately match the sentinel and skip.

An immutable per-traversal native snapshot is valid only within a bounded service that cannot reenter an application publication or pause writer. General callback support must read the same live owner at the source boundaries above. Do not present one captured entry stamp as exact when callbacks may publish time.

## Existing native owners and retained publication

`tests/native/original_ui_main_entry50kg.h` Application owns process lifetime and the retained Video/context. The constructor receives the SAME shared HostMonotonicClock50ID built once at terrain_interactive_drive50du.cpp:7516. UI `state->clock101A50` aliases it at main_entry:204, but that assignment occurs only on the enabled-Main branch. Direct entry still receives the same constructor argument. Extend Application before its disabled-Main early return, not only the Main-specific state.

Application::videoForSession preserves the same Video identity over serial sessions. Video::standardMaterialConstructionContext pins its authoritative application-owned backing, templates/runtime and texture-list owner. This is the existing sealed extension boundary for a read-only canonical clock capability if material code needs it. Video should borrow/pin the actual Application-owned publication backing, never originate time or own another pause bool. Preserve no-backreference/no-cycle topology and actual host-current checks.

The existing race publication is `sourceApplicationTimestamp` plus `sourceApplicationTimestampPublished`, terrain_interactive_drive50du.cpp:4422..23. `advanceWorldAnimations` samples the SAME host clock at4433 once then shares the value with GC/GA/SA world and ATM consumers. Binder first-render fallback6996..7000 samples only when that publication does not yet exist;6993 comments already require reuse. MA shield consumes it at7009. **Move/alias this existing publication into the one Application owner; do not add a parallel UI-only canonical timestamp.** Its current local lifetime resets on each runSession and its publication has no true11C lane yet.

The local `FrameTimingState49I clock{}` at terrain_interactive_drive50du.cpp:1815 sets only lastDispatched23C=.016f for physics consumers. No begin/reset/finish writes to its current230/active11C were found in this production function. Reusing its zero current230 or active11C as a known application publication would falsely certify an unconnected field. Preserve physics factor behavior while moving the actual publication lanes to the application owner; do not start executing49I root phases just to obtain a timestamp.

UI Session::sampleClock at main_entry:447 retains rawSample only temporarily. It returns filtered activeSourceNanoseconds quantized into sourceMilliseconds, subtracting cold time and host inactivity. Preserve this legacy timeline for existing consumers. Publish its SAME already-obtained rawSample into the Application's canonical timestamp; no second OS sample, inverse filter, dt accumulation or draw-local counter.

## Minimal connected root-owned plan

1. Extend the existing Application with the single canonical publication backing that replaces/aliases race sourceApplicationTimestamp(+Published) and owns source11C/120 semantics. Retain constructor clock even with Main disabled. Reuse49I lane names/operations where appropriate; do not create a second independent FrameTimingState next to a still-authoritative timestamp. Initialize11C/120 once by constructor source proof, timestamp unavailable until an actual sample has been published. Preserve previous234/reset semantics only at actual represented source boundaries.
2. Move the native explicit pause command writer into that owner and have Session::hostPaused become a read/alias instead of a second authority. This is an explicitly documented native input mapping onto the recovered source pause word, not a claim of original matcher parity. All current focus/minimize/event-budget/controller-loss suppression remains separate scheduling policy. The race `paused` local (6070 onwards) combines all those reasons and cannot populate11C. Preserve its scheduling uses; migrate only genuinely explicit menu/pause requests to the one owner. Future recovered1BE8A0/1BE830/1A9240 transitions must use their actual ordered writer methods rather than blanket page-entry resets.
3. Every existing raw material sampling site must publish/read the same backing: UI Session::sampleClock; retained in-race frontend sample; race advanceWorldAnimations; Binder first-render fallback. Read back one timestamp/pause pair for the bounded traversal. Do not sample again in an adapter or material. A post-blocking Rider sample remains an intentional new publication at its existing source boundary.
4. Add a typed read-only application animation view to OriginalUiMainFrameClock50KG while preserving its legacy three fields. Defaults must have timestamp/pause provenance unavailable. A live sealed owner/view is preferable for future reentrant callbacks; a value sample is sufficient only for the explicitly callback-free bounded ATM route. Factory/producer grants known state; legacy aggregate construction does not.
5. Pass the view through Session::render -> coordinator.render -> actual Tournament canonical controller service. Main render already takes the full clock. Options/Rider/Course reconstruction at frontend50kg.cpp:909..911 must forward the view (Rider uses refreshed renderClock, not original clock). Extend their three clock structs only if this path needs canonical service there; never silently drop provenance and then replace unknown pause with false.
6. Replace Tournament sampleMarker's legacy arithmetic gate only when actual canonical pass/controller/aggregate ownership is joined. Its current career_resources.cpp:348..353 checks advanceAnimations and milliseconds. Those are legacy scheduling/numeric fields, not29AC90 inputs. Canonical dispatch requires the real view and runs duplicate/pause gates itself; no outer !advanceAnimations-as-pause gate. Suppressed service leaves stamps unchanged. Preserve callback-false stamp behavior and actual source count/frame ordinal.
7. Paused-only presentation needs a real scheduling join if required: standalone frontend currently continues without update/render when !active, and coordinator rejects rendering twice per accepted update. Do not bypass these by advancing transitions, fake dt or gameplay. Keep source11C and source flags228 suppression distinct. If only active traversal is being joined now, state that paused material service is not yet scheduled rather than claiming animatesWhilePaused runtime coverage.

## Adapter/default inventory

- `platform/linux/native/original_ui_main_draw50kg.h:110` MainFrameClock aggregate; main_draw.cpp:327 stored default,362 begin,388 legacy helper.
- `tests/native/original_ui_main_entry50kg.h:447..472` sole MainFrameClock construction return{true,active,sourceMilliseconds};401 materialClock refresh;760 Session::render full forwarding.
- `tests/native/original_frontend_session50la.h:242..243` explicit pause writers;263 composite active;287 sample; inactive early continuation after it.
- `tests/native/terrain_interactive_drive50du.cpp:5757..58` retained frontend explicit pause writers;5783 sample.6070..83 composite gameplay pause is NOT11C.
- `platform/linux/native/original_ui_frontend50kg.cpp:899..911` copies full clock initially; blocking Rider refresh; then three-field Options/Rider/Course aggregate reconstructions.
- `original_ui_options_draw_contribution50kg.h:22`, rider_draw_contribution header:9, course_draw_contribution header:10: three-field defaults. Their storage clock defaults remain unknown unless producer supplies view.
- `original_ui_rider_draw_contribution50kg.cpp:641` downstream stat projection reconstructs its legacy three numeric fields. Leave it explicitly legacy unless its consumer is separately made canonical; it cannot synthesize known canonical pause.
- Main/Options/Rider/Course session render guards reject their own paused&&advanceAnimations combinations. Keep these guards scoped to legacy session policy; they are not canonical29AC90 proof.
- Tournament career_resources header forward-declares MainFrameClock and its sampleMarker takes that full type, so no separate tournament clock struct is needed.

## Evidence and scope

ELF source: `/workspace/scratch/74cd1d3d7a45/original-source-recovery-1906/SLUS_202.68`; established SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. ELF LOAD offset80 maps virtual100000. Retained static field listings: `/workspace/shared/rr-clock-13fb80-fields.txt`, `/workspace/shared/rr-clock-full-static-fields.txt`, `/workspace/shared/rr-clock-writer-candidates.txt`, and existing rr-atm-29aaa0/299fe0-fields.txt. Full listing is static data decode, never executed instructions.49I/49J documentation was used as prior source evidence; its historical validation results were NOT rerun or newly certified here.

Root owns integration. This report is a connected implementation plan, not implementation or runtime readiness. The chief remaining decision is the exact native explicit-pause input mapping and paused-service scheduling; there is no missing source identification for11C or230 that justifies substituting generic activity.
