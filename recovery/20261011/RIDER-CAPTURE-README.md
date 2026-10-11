# Bounded genuine Rider19 capture candidate

Status: source-authored and source-reviewed only. Nothing in this package has
been run, imported, syntax-checked, built, decoded, tested, or used for gameplay.
Root alone owns the later centralized validation slot, no earlier than
2026-10-11T05:19:42.970824Z, and the shared one-check ledger. This package performs
no conversion or build and never relaunches a native process.

## Contents and provenance

- `record_original_rider_30hz_recovery.py`: independent Rider-only recorder.
  The existing Options recorder is unchanged. The retained tested-design
  preimage is `provenance/options-recorder-preimage.py`, SHA256
  `5be063cffd13bef90e7b89034154aba7e9cb23aa6032578d110fe6b4a24377f5`.
- `files/tests/native/original_ui_main_entry50kg.h`: one narrowly diagnostic
  native header candidate; `rider-observation-diagnostics.patch` is its delta.
  Exact preimage is retained in `provenance/`. Apply only after matching it.
- Native source inspected: `5f650ed11fbe6b57cb62174eefdb84c68ef9ccb9`, which
  includes the six-file enabled Rider audio integration. Source hashes and
  copied preimages are retained; they do not prove binary build provenance.
- `SOURCE_SHA256SUMS` names the exact native files used for source review.
  `CANDIDATE_SHA256SUMS` freezes deliverables after final static review.
- `provenance/rider.dat` retains the original6495-byte authored slot evidence,
  SHA256 `fc02bd935bcc540f69137a403f11751f71d9f9b87d764d03cc0df0117cff7cb0`.
  Its exact matching member record is in the retained native archive catalog:
  `UI\\UIRider\\rider.dat`, offset1425408. Group1 ControlID1=16; Control16
  PodIndex=19. The containing5552683-byte UIRIDER.RES catalog SHA256 is
  `112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856`.

The diagnostic patch only reads existing real owners. It reports Rider profile
stamp/generation/mode, the public remembered-rider snapshot, Main profile fields,
settings/sound revision, shared-profile linkage and LightManager generation/cache
state; Rider additionally reports existing bank/registry generations and actual
device counters. Main Song9 is exposed when Rider support is present. No setter,
new owner, lifecycle action, RNG draw/reset, PCM advance, model reset or readiness
flag is introduced. Main mode4 legitimately makes the public Rider selection
snapshot unreadable: remembered fields are null then, not invented or coerced.
Main/Options runs without Rider cache retain their prior report fields.

## Exact sequence

All state assertions gate real host keys; no source/model/controller state is
written by the recorder.

1. Observe genuinely open Main4/1, controller group1, slot1.
2. Send Enter once, capture its actual committed image and state, release Enter
   with a separate capture barrier. Observe Main closing and Rider5/1 opening.
3. Wait for genuine Rider19 at original Group1 slot1, attached selection, open
   aperture, visible input, unblocked panel at y=0, prompt mask3, and accepted
   source renderer frame/target generations. Require actual Song7.
4. Retain at least3 seconds of idle measured by the shared audio output's real
   accepted source nanoseconds, while capturing real updates/frames. RNG and
   animations proceed normally. No particular random idle choice is demanded.
5. Send Left, Right, Enter exactly once each, in that order. Each down and up
   has its own image/state capture barrier. Require one explicit native
   unavailableDestination diagnostic and counter increment per press, preserved
   selection/attachment/GPU/bank/registry/light generations and profile fields,
   and no new Rider audio submission. Retain at least350ms accepted source time
   after each rejection before sending the next key.
6. Send Escape once with distinct press/release barriers. Back really commits
   remembered19 to19 and increments the profile generation1 to2. It plays the
   actual Back5 effect and then fades the shared music during closing; the
   recorder requires the associated native Rider submission count. Observe
   actual Rider closing, Main reopening at saved slot1 and Song9. Retain1 second
   accepted source time of settled Main, then send the normal host quit.

There is no optional second entry in this candidate. Its five-press core leaves
headroom for cold admission/real transitions under one bounded native process.
No Up/Down, driving, pause, restart, checkpoint, save import, alternate Rider,
page2 or Course action is allowed. A failed gate retains a partial observation,
requests quit and does not retry.

## Root-only later preparation

The obsolete muted-only recipe remains a historical prerequisite note; this
candidate requires the integrated Song7/Back5 enabled audio owner.

1. Perform the pinned original kind2 conversion once in root's authorized slot.
   Retain all five exact mip files and conversion manifest. Integrate its
   generated catalog without editing pins or manufacturing fallback pixels.
2. Match this diagnostic header's exact preimage, integrate its candidate, and
   freeze the full source revision including the genuine generated catalog.
3. Use the centralized build process. Retain a build manifest/log identifying
   the frozen source, catalog and produced ELF. Record its SHA256 and the
   actual native diagnostic header SHA256. A build pass is not a UI pass.
4. Choose a new direct child output directory under this package. No overwrite
   or cleanup/retry against an existing run directory is supported.
5. Run this candidate only if root still owns that exact ledger slot. Proposed
   invocation, NOT EXECUTED (replace all angle-bracket values with verified
   root-owned artifacts):

```sh
python3 -B /workspace/shared/recovery-20261011/rider-capture-sequence/record_original_rider_30hz_recovery.py \
  <native-ELF> \
  --expected-exe-sha256 <ELF-SHA256> \
  --expected-entry-sha256 <native-header-SHA256> \
  --source-commit <frozen-40-hex-commit> \
  --build-provenance <retained-root-build-log-or-manifest> \
  --rider-ipum-cache <conversion-root>/evidence/ui-rider-ipum50kg \
  --output /workspace/shared/recovery-20261011/rider-capture-sequence/rider-validation-01 \
  --root-reviewed-one-check \
  --not-before 2026-10-11T05:19:42.970824Z \
  --ack-timeout 10
```

The recorder refuses early launch rather than waiting, verifies the ELF before
launch and its running `/proc/<pid>/exe`, launches the native ELF exactly once,
and rehashes it after exit. It hashes the explicitly supplied cache files,
conversion manifest, compiled source/catalog and root build artifact. Native
startup remains responsible for authenticating cache/source bytes. The claimed
source commit and recorded source hashes do not independently attest what the
ELF was built from; root's retained build provenance supplies that link.

## Time, process, storage and controls

- One Popen/native process, owned process group; no helper launch, fallback,
  extra game launch or retry. Original native corpus/CWD remain unchanged.
- Readiness timeout180s (positive, maximum300s); capture window30s from real
  Main readiness. Capture acknowledgment default5s, maximum10s; navigation
  gate timeout12s, maximum20s. Normal quit timeout10s, maximum15s.
- Failure cleanup waits only through any already-issued capture deadline,
  sends normal quit, then escalates only the owned process group after the
  quit timeout: SIGTERM,2s, SIGKILL,2s. Forced cleanup is never a pass.
- Capture request target/ceiling30Hz, maximum920 captures, one in flight,
  maximum16KiB stdin. No padded, interpolated or synthesized frames. A new
  actual native frame and accepted update must be present for each capture.
- Each PPM is exactly640x448 RGB with the native15-byte P6 header:860175 bytes;
  maximum preserved PPM data791361000 bytes. Raw log32MiB, event log64MiB,
  frame metadata16MiB, native line256KiB, manifest/summary2MiB each.
- Actual48kHz stereo16-bit WAV bounded to70s/3360000 sample frames,
  13440044 bytes. Child-only RLIMIT_FSIZE uses that bound for each file and
  core dumps are disabled. Additional home/tmp/cache files have the same
  per-file native limit; the named-evidence bound is not a filesystem quota.
- The full named-evidence upper bound is computed into `launch.json`. Allow
  about1GiB free space for the retained evidence, plus native temp/cache needs.
- Unrelated environment variables are not inherited. Device output is off;
  original UI audio is enabled and WAV is explicit. RR_AUTODRIVE=0 and
  RR_ORIGINAL_AI_ALL_RACERS=1 remain set; any world tick/race mix is rejected.

## Evidence interpretation

`game.log` retains native bytes. `events.jsonl` retains host writes, receipt
arrival times and diagnostics. `frames.jsonl` binds every native atomic PPM
commit ACK to its immediately following matching Main/Rider source state and
hashes the copied image before another capture request. All capture ACKs use
`original_main_capture`; genuine Rider states use `original_rider_state` and
ACK `selected_group` is null. Rider state `quads`/`draws` pair with ACK
`source_quads`/`source_draws`. Every Rider image needs strictly progressing
accepted source frame and target generations.

Each preserved image records the genuine accepted-source nanosecond boundary
and the corresponding real WAV sample-frame boundary. It checks
mixed=captured=source-mixed=floor(clock_microseconds*48000/1000000) and carries
the source nanosecond remainder. Final WAV size/header is checked against the
actual closed receipt after quit, not the earlier last image. There is no
resampling, silence synthesis, decoder, mux, physical-device/audible-quality
claim, or invented native presentation timestamp.

Profile and ownership invariants concern reported real stamp/selection/bank/
registry/light generation fields; the tool cannot prove all invisible state or
physical pointer identities. Returning to Main traverses the native mandatory
source-owner retirement gate; screenshots still require visual inspection for
stale imagery and original art/text/model quality. Source frame advancement is
not by itself proof of a particular animation pose or retail pixel parity.
World ticks must remain zero, native cleanup must report success, return code
must be zero, and every required observation must pass before the result is
classified as a completed bounded observation. Partial evidence is retained.
