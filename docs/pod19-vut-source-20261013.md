# POD19 left-engine VUT source correction

Source/byte review against central revision
`c2e2e8141bb55305da36814bcb80421f2946510a`. This patch changes only the
POD19 left-engine cold binding admission in
`tests/native/native_racer_rig50hw_fixture.h`.

## Authentic authoring mismatch

The Windows cold load stopped with `unknownNode byte=181 skippedUnknown=0`.
The retained raw originals are in
`/workspace/shared/camera-basic-smoke-20261012/run-01/runtime/evidence/originalRoster50HW/raw-imports/pod19/`.
Their `import.json` identifies the source members and matching SHA-256 values:

- `49-original.vut`, `Pods\\Pod19\\englt1.vut`, 374 bytes, SHA-256
  `37e4cd4e0fff2472b2a045ef5da8a8e4ca02d1775e7c0d9afbca4affa3ae3524`.
  Byte181 starts `engrtflame01=engrtflame01`; byte208 starts
  `engrtflame02=engrtflame02`. Both are CRLF-terminated.
- `48-original.psg`, `pods\\pod19\\englt1.psg`, SHA-256
  `a5f24cf472e3f52f989cc053236865503c0f6d838a3ce9a18c3b555c0b524b9e`.
  Its version3 header declares18 nodes. The original name fields at offsets
  2028 and2252 contain `engltflame01` and `engltflame02` (PSG nodes9 and10).
  It has no right-flame names. Every other VUT lookup key has a matching
  original name field. The right-engine VUT and PSG both use the right-flame
  names; the chariot names also agree by direct string/field inspection.

This is a genuine missing-name authoring case, not an unknown binary tag or a
Windows-specific text-format difference. Source bytes are not corrected or
rewritten, and absent right-flame names are not aliased to actual left flames.

## Original missing-name branch, inspected directly

Original input:
`/workspace/shared/camera-basic-smoke-20261012/run-01/runtime/work/native/project/disc/SLUS_202.68`
SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The ELF32 little-endian program header maps file offset0x80 to virtual
address0x00100000. Thus file offset = VA -0x00100000 +0x80 for this block.
The following original little-endian words were read with `od`; instruction
meanings were reviewed manually, without executing guest code or a decoder:

| Original VA | File offset | Word | Meaning |
| --- | --- | --- | --- |
| 00153238 | 000532B8 | 0C0550F4 | Call name lookup001543D0 |
| 00153240 | 000532C0 | 305300FF | Mask returned index to8 bits |
| 00153244 | 000532C4 | 240200FF | Load missing sentinelFF |
| 00153248 | 000532C8 | 16620006 | Branch to153264 if index differs fromFF |
| 0015324C | 000532CC | 00000000 | Delay-slot nop |
| 00153250 | 000532D0 | 3C02003A | Begin diagnostic-string address |
| 00153254 | 000532D4 | 0C04CDB0 | Call diagnostic001336C0 |
| 00153258 | 000532D8 | 24445F40 | Argument points to003A5F40 |
| 0015325C | 000532DC | 10000064 | Unconditionally branch to001533F0 |
| 00153260 | 000532E0 | 00000000 | Delay-slot nop |
| 001533F0 | 00053470 | 3C01003E | Enter next-line-read block |
| 00153400 | 00053480 | 0C04BBA4 | Call next-line reader0012EE90 |
| 00154454 | 000544D4 | 240200FF | Lookup missing-name path returnsFF |

The raw NUL-terminated bytes at file0x002A5FC0 (VA003A5F40) read
`Hmmm, VUT string invalid...\n`. The unconditional branch skips the whole
rename/+38/+3C update block and resumes line reading. This independently
confirms the existing `readMotionNodeBindings49O` `sourceDiagnosticSkip`
implementation, previously documented in `native-roster-rigs50hw.md`.

## Narrow native change

The opt-in requires all of:

- POD19, left-engine role, exact `englt1.psg` and `englt1.vut` member paths
- Both original hashes above (the existing read path checks actual file hashes)
- Successful existing strict hierarchy/text admission
- Exactly two missing lookup keys, first at byte181

The original hierarchy and binding row count, scene references, node names,
and unlisted flags remain intact. Matching rows still receive their ordinary
VUT flags. The native decoder default remains strict; no unknown binary tags,
general missing names, other models, or unverified file pairs are admitted.
No visual, physics, animation kernel, source asset, or guest-execution path is
changed. The existing native diagnostic counter records the source skip.

## Windows translation-unit scope, source review only

The manifest `platform/windows/native/interactive50du.sources.json` has exactly
one affected translation unit: `tests/native/terrain_interactive_drive50du.cpp`,
source index1, corresponding to `obj/0001-terrain_interactive_drive50du.obj`
under the existing build script's deterministic naming.
The reviewed manifest SHA-256 is
`a22adc71878c0a036912de0646da7e97f718b870c7dc3fdb1475f79c397bb26d`;
the reviewed build-script SHA-256 is
`fd55ad4ac5d0f4a0e54332a462f6087736554109176172d2cef38f8b4871fc93`.

Reverse include search of the current source tree finds these direct parents
of the changed header:

- `native_racer_cables50hw_fixture.h`
- `selected_player_audio50ij_fixture.h`
- `original_racer_visual50hw_fixture.h`
- `original_course_roster50kg_fixture.h`
- `gameplay_binder_candidate_fixture.h`
- `terrain_interactive_drive50du.cpp` itself

The additional reverse paths are `original_ai_audio_owner.h` and
`live_audio50hf_fixture.h` through selected-player audio,
`original_course_race_targets50kx.h` through the Course roster, and
`gameplay_binder_window_candidate_fixture.h` through the Binder fixture.
Every path ends only at the same integrated translation unit. No current
source/header computed-include directive was found. The build's sole forced
include is its generated native x64 guard, which does not include this owner.
Historical `recovery/` copies are excluded from the Windows manifest.

This proves the edited-header dependency scope by source reading, not prior
object provenance. Reuse of unchanged objects still requires the root to
verify the previous receipt, all unchanged input hashes, flags, toolchain and
objects. The shipped build script itself requires a fresh full build and has
no incremental mode; this patch does not change it or claim reuse occurred.

## Validation boundary

Only raw byte/string reads, SHA-256 identity checks, source reading and file
editing were performed. No compiler, preprocessor, test, syntax/lint check,
sanitizer, model/motion decoder, gameplay, VU/PS2 emulation, or original
instruction execution was performed. Central Windows build and cold-load
retry remain required; this document does not claim that loading or rendering
the race now succeeds.
