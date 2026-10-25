# RacerRevengePC: Fastest Path to Beta Testers

Approved working direction: October 24, 2026, Eastern time.

## Controlling instruction: code first, testing requires sign-off

Complete all remaining code for priorities **1 and 2** before requesting testing sign-off. **Do not test until that code is complete and Steve explicitly signs off.** Do not rerun racer/track combinations; that coverage has already been done and further combination testing belongs to beta testers. Lap completion, race finish, and Results testing remain tester-owned.

Keep the parent and three engineering agents working on distinct, useful coding tasks. The parent owns integration. Do not substitute repeated reviews, small-patch test cycles, or inactive prerequisites for completed player-facing functionality. If genuinely blocked, report the concrete blocker rather than fabricate progress or broaden scope.

Provide concise progress updates every 20 minutes. **Stop and report when priorities 1 and 2 are code-complete.** Do not proceed into testing or priorities 3 and 4 at that stop point without further direction. This is Steve's October 24, 2026, 8:10 p.m. EDT clarification (Sentinel_f70026a9f9088191b899252b42aedd7f).

No new builds or runtime checks are scheduled during this code-first phase. The testing approach below describes a later phase and does not authorize testing now.

## Goal

The next milestone should be a usable beta, not another collection of inactive prerequisites.

## Priorities

### 1. Racer and track selection/loading first

Use the existing coverage record to identify actual missing or broken routes. Finish those routes so racers and tracks load normally from original disc files. Prioritize crashes, wrong assets, and failed startup over deeper internal refactoring. Use existing evidence rather than replaying racer/track combinations. Any future testing requires the sign-off above; combination testing belongs to testers.

### 2. Essential menus and controls second

Batch the remaining work needed to select a race, configure controls, drive, Pause/Resume/Restart, and return. Finish broken Back/Cancel/Accept routes together rather than treating each helper as a separate milestone. Preserve the original menu behavior.

### 3. Minimum progression and persistence needed for beta

Separate what testers need to access and retain playable content from what can be a clearly documented limitation. Prioritize anything that blocks access or risks losing/corrupting data. Do not require complete career or optional-settings parity before a useful tester build can go out.

### 4. Package and hand off

Deliver a versioned build with original-disc setup instructions, controls, known limitations, and a focused tester checklist. Clearly separate compiled code, observed working routes, and unverified functionality. Tester feedback should drive the next fixes.

## Defer

- Further inactive cutscene, audio, or Options infrastructure unless it directly unlocks one of those priorities.
- Exhaustive internal lifecycle parity, cosmetic polish, FPS tuning, and repeated reviews of unchanged code.

## Later verification approach: only after explicit sign-off

- Write and integrate coherent feature batches rather than running a build/test cycle after every small change.
- After items 1 and 2 are code-complete, present implemented routes and genuine remaining blockers to Steve for sign-off before testing.
- If authorized, focus checks on changed playable routes, not a repeated whole-menu tour.
- A combined release-candidate regression before handoff is a proposal for that approved phase, not current permission. Reuse existing evidence where it still applies.
- Do not repeat every racer/track combination. Beta testers own that coverage.
- Retain same-run selected-racer/in-race proofs for any subsequently authorized GUI run.
- Lap completion, race finish, and Results testing remain entirely with Steve/testers.

## Parallel coding assignments

1. Selection/loading agent: missing normal racer/track selection and original-disc loading code.
2. Menu-route agent: essential menus, submenus, and Back/Cancel/Accept routes.
3. Controls agent: control configuration, driving input, Pause/Resume/Restart/Main return.
4. Parent: resolve shared-file integration, finish cross-cutting connections, and maintain an honest code-completion record.

Parallel work addresses separate concrete beta blockers, not more layers of review. Isolated source changes are integrated by the parent. Preserve original assets and existing evidence; do not introduce generated runtime data packs or claim untested code is verified.

## Current phase

**STOPPED: priorities 1 and 2 are code-complete and integrated in the v100 source candidate.** No builds or tests were run. Await Steve's explicit sign-off before verification; do not start priorities 3 or 4. This supersedes earlier in-progress status.

As of October 24, 2026, 8:28 p.m. EDT, the parent has integrated source changes for normal Ryloth human entry, pad Pause navigation, retained menu-to-driving device choice, normal disc-entry defaults, fresh selected-career start, selected-racer Garage resources, Event Type selection, and live-roster count propagation. These are **source implementations, not compiled or runtime-tested results**. Those camera/music and distinct record-mode entity/radar dependencies were subsequently integrated. See the final source-only completion record in the candidate's docs/CODE-COMPLETION.md.

### Item 1–2 implementation batch

- **Existing source:** the catalog already contains 23 racer identities and 13 course identities. Do not confuse catalog admission with full cold-load implementation, and do not repeat prior combination coverage.
- **Selection/loading work:** finish the actual Ryloth normal-human cold-load path, which still has spectator/test-profile restrictions in the current source.
- **Menu entry work:** make the ordinary `--assets` entry configure the existing logical Main audio and native Course defaults so Game Options Accept and normal loading do not depend on test-launch environment settings. Preserve explicitly supplied settings.
- **Controls work:** connect physical-controller navigation and confirmation to native Pause/Restart/Main/Controls menus with focus/disconnect/reconnect input gating, without synthesizing gameplay input.
- **Normal device handoff:** retain the genuine input device used for normal menu selection into driving, while honoring explicit input policy, rather than requiring an undocumented controller environment setting.
- **Career/Tournament launch:** implement the actual mode-2 Course Confirm transaction and its native handoff. Existing mode-0-only gates are a source implementation gap, not a reason to rerun old combination coverage. Do not substitute mode 0, bypass original profile semantics, or claim progression/Results implementation as part of this first-race entry work.
- **Parent integration:** combine the three source overlays in `full-app-beta-routes-20261025-v100`, preserving the immutable v99 source baseline and recording exact changes. The candidate is source-only until explicitly approved verification occurs.
- **Completion condition:** each remaining essential route has real implementation or an explicitly reported blocker; all cross-file connections are integrated. A file being compiled in an earlier checkpoint, or a helper existing but never called, is not sufficient to mark a new route complete.

## Instruction provenance

- Plan request and clarification: October 24, 2026, 8:02 p.m. EDT.
- Code-first and explicit testing-sign-off instruction: message Sentinel_df093f8427148191a2d68a12d10bc30c, October 24, 2026, 8:06 p.m. EDT.
- Request to save this Markdown game plan: message Sentinel_9f181bc60f248191a0ef93f61c3d52c7.
