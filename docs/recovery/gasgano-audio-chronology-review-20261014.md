# Gasgano audio integration source review

2026-10-14. Reviewed root's uncommitted changes in original_ui_main_audio50kg.{h,cpp} and original_ui_rider_voice_choice50kg.cpp, plus existing preparation/playback/session callers. No production edits, builds, tests, syntax checks, decoder/converter execution or project execution.

## Verdict

No blocking storage, event-index, preflight or RNG-chronology defect found in the three-file delta. Supplied source-worker size/hash/frame evidence is transcribed exactly. This is manual source acceptance, not decoded-audio or runtime validation.

## Deployment consequence

Enabled frontend audio with a present cold Course archive now requires BOTH `SPEECH/UI/GASSEL01.VAG` and `SPEECH/UI/GASSEL02.VAG`, even if only default-unlocked riders are used. `prepareCourseMenu` loads all21 choices unconditionally. Existing `original_ui_main_entry50kg.h:631..632` throws if it fails, so a missing/newly unauthenticated Gasgano file aborts frontend cold preparation; it does not merely disable Gasgano or quietly retain a Course-capable19-choice transport. Root should deploy both originals with this change. Muted output bypasses these file requirements.

## Storage and authentication

- riderConfirmClipCount becomes21 consistently across original-file descriptor array, owned PCM array, borrowed clip array and temporary preparation array. No fixed19 loop or ordinal/event-ID assumption remains in these paths.
- Events15 and16 map to the new original file rows. Existing later rows merely move storage index; playback resolves by event ID using riderConfirmClipIndex, so their selection is unchanged.
- GASSEL01:26656 bytes, SHA4076d3b4943d5828a0b7977ca0bc9a6e1fdffc1e4a0925f41aed560713183c90,46536 frames. Supplied1661 zero-based end-block index means1662*28=46536 frames; payload26608 bytes is1663 blocks, leaving the stated one pad block.
- GASSEL02:26512 bytes, SHAc647576fb9e438d58b921142c0592041ba2a9ccb0a9db539e0f6ceeb34eb28e9,46284 frames. Supplied1652 end index means1653*28=46284 frames; payload26464 bytes is1654 blocks, likewise one pad block.
- Existing decoder source accepts the retained48-byte zero-header variant only with declared payload reaching EOF, emits28 samples from the end-flag block before breaking, and computes source SHA over the whole file including unconsumed padding. That source behavior matches the supplied metadata. No decoder relaxation was added.
- preparedClipMatches checks retained whole-file SHA, exact frames/rate22050/channels1, no loop, exact sample vector size and borrowed sample pointer identity. This remains required for all21 clips.
- Preparation decodes into local owners before moving them into transport. Clip pointers are then rebuilt from the moved-to member PCM vectors. Audio owner is neither copyable nor movable, so member backing cannot accidentally relocate afterward. Destructor/reset ownership remains unchanged.
- Missing/new-byte failure keeps coursePrepared false and courseAttempted true; a fresh cold preparation is required. No partial success or fallback voice is invented.

## All-choice preflight and chronology

`canPlayRiderConfirm` checks all21 retained choices before any accepted choice draw, including exact PCM metadata and available dedicated speech voice/control revisions. Both proposal-time confirmReady and accepted-route notification recheck readiness. Thus the new event cannot be selected first and only then discover an unprepared clip under ordinary exclusive-owner operation.

Range{rider7,first15,count2} joins the existing source-ID lookup. No compact roster indexing or profile unlock is introduced. choose retains the existing lazy80A8/seed3D6B30 behavior and conditional SAME host-clock seed sample, then exactly one308030 draw, positive source-domain quantization and count scaling, range assertion and first+choice mapping. Count2 yields15 or16. No preparation/preflight function calls choose or consumes random state.

Accepted notification still verifies profile/route commitment, marks fadeMusic, performs voice selection/playback, runs character `_sel`, then selection effect1. Muted mode still performs the same choice chronology and suppresses only output. Enabled output plays exactly the selected event with no second draw, no replacement retry and no stop/restart of an existing speech utterance. Voice slot7 and total frontend voice count8 are unchanged:21 is storage count, not simultaneous voice count.

## Scope

The new range permits low-level voice preflight/selection for rider7. Actual session preparedRider, profile/unlock, route and Course capability checks remain independent and unchanged, so this delta alone does not make Gasgano selectable or grant a destination. No claim of such admission is appropriate.

No additional issue found. Supplied original-file evidence was treated as source-worker evidence; this review did not independently decode or inspect those original payloads.
