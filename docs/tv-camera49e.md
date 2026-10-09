# 49E: authored TV-camera data and native selection

## Useful production closure

The authentic TA camera configuration now has a compact caller-owned native
representation, plus the original position-selection, delayed-switch, reset and
random-state behavior. This is a real game camera path: game function175D70
passes the player position and its supplied frame delta through the camera
vtable at1769C8; TV vtable3D5160 slot+24 resolves to2D30D0. The same game-object
camera pointer is used by the original loader.

The new production modules are `src/assets/tv_camera 49e.h/.cpp` and
`src/game/tv_camera_selection49e.h/.cpp`. No original-instruction interpreter
or game-memory access runs in production. Selection is bounded and serial, with
no hot-path heap allocation. The full camera update2D3400 and its world-dependent
2D38C0 call remain outside this stage. No scene visibility, camera matrix,
rendered image, hardware transport, full race loop or FPS claim is implied.

## Real source asset

The already-authorized standalone cloud TA.RES was hash-checked and retained at
`work/native/project/disc/TRACKS/TA.RES`; no PC retrieval or49C code was used.
The unchanged bounded48C v3 RES decoder extracted the fixed requested member:

- TA.RES: 3,334,562 bytes, SHA256
  `b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`
- `tracks\taTVCam.pos`: logical offset 13,000,704, 5,527 bytes, SHA256
  `e384bcf4309808e7b752b20546ae8beceb17f0fe6c3b59dd320bea1a7beb5ac8`
- Container decoding: 383 members, 533 blocks,13,099,008 decoded bytes
- Both normal and ASan/UBSan extraction reproduce the exact member hash.

All 19 camera records and 20 ordered links are retained. Camera4 links to5 then6;
camera 19 links to1. Native links subtract one once. InitialZone is retained
unchanged, including its original default -1; it is not silently decremented.
The raw member and container are included for independent recovery.

## Data, defaults and numeric boundaries

The full source loader is2D29F0–2D30AC and the record append helper is
2D2720–2D286C (half-open ranges). Source evidence and the unchanged original
ELF hash are recorded in `evidence/tvCamera49E/loader-source`.

NumberOfCameras must be positive. SwitchEvery defaults to5. Camera defaults:
Position,Look,Up have XYZ0 and W1; Stationary0; FOV-1; MaxFOV70; MinFOV10;
MaxFOVDistance200; MinFOVDistance20; InitialZone-1. Converted FOV exactly-1
triggers the separate Max/Min fields; any other finite FOV supplies both and
does not consume Max/Min values. No normalization, range sorting, clamping or
unit conversion is applied. In the real file, camera 5's MaxFOV25/MinFOV65 and
distances400/1572 are deliberately preserved.

TV,Follow,LeftSide,RightSide,LookBack,FirstPerson and Blur retain their exact
signed authored/default integers. Defaults are50,10,10,10,10,0,25. The original
threshold preparation first converts wrap32(100-TV) to binary32, then subtracts
separately converted Follow/LeftSide/RightSide/LookBack/FirstPerson in order.
Blur separately converts wrap32(100-Blur). Integers are not summed first and
percentages are not normalized or constrained to0..100.

Original decimal ingestion calls a binary64 parser then its own software
binary64-to-binary32 conversion. The recovered finite narrowing is implemented
with integer bits: nearest-even normal rounding, distinct truncating underflow,
signed zero, and binary64 subnormal-to-signed-zero behavior. It is not replaced
by a host float cast. Nonfinite inputs/results are outside native admission.
The 119 reachable finite-domain original instructions are exercised independently;
remaining 19 retained words are nonfinite branches/dead padding.

Native text ingestion is an explicitly narrower ASCII grammar using decimal
binary64 parsing. It accepts the actual whitespace, CRLF/LF, semicolon comments,
scientific decimal notation and optional f/F suffix. Integer fields are signed
int32 decimal; nontrivial leading zeros are rejected to avoid inventing equivalence
to the original base-zero integer parser. Original suffix-stop evidence confirms
that authored f/semicolon suffixes terminate the numeric prefix. All actual 140
float components have at most 8 significant digits and 3 decimal places. An
independent exact-rational projection provides the authenticated numeric reference;
this does not claim full equivalence to every original libc/parameter-parser input.

The native loader rejects duplicate/unknown keys and sections, malformed consumed
values, missing/nonpositive link counts, missing links and targets outside1..N.
Ignored Max/Min values under an overriding FOV are not parsed. Limits are 64KiB
text, 128 cameras and 1,024 total links, with additional scalar/vector token bounds.
These are native admission limits, not invented original loader checks. Source
loading writes incrementally and does not validate link target ranges. Native
loading instead validates before filling typed caller storage, so failures preserve
storage/output. This stronger failure policy is explicit, not claimed as original.
The native decoder processes each requested input; it does not reproduce the
original existing-nonzero-count early return that skips loading an already
initialized object. File opening, allocation and the full object constructor
remain outside this caller-owned data adapter.
Nearest-even host rounding is required for ingestion/threshold arithmetic.

## Exact selection, reset and RNG semantics

Selector2D30D0–2D33FC consumes supplied player XYZ and frame delta. It has no
world-query or global-clock call. Initial negative current scans all cameras.
Established current scans its ordered outgoing links, then compares current.
All-camera/link-list ties retain the first candidate; a current-versus-linked
tie chooses the linked camera.

Distances execute player-minus-camera differences in Y,Z,X order, then separately
rounded X squared and Y squared, their rounded accumulator sum, and Z squared
plus that accumulator. Tests explicitly use the strict host-binary32 instruction-
step model. ELF instructions alone do not prove all PS2 silicon MADD, denormal
or exception micro-arithmetic; no universal silicon-bit-exact claim is made.

When desired differs, a positive pre-decrement hold subtracts elapsed and does
not switch that call, even if the result reaches/crosses zero. A later call with
hold<=0 switches, stores previous/current, and resets hold to binary32
`3F19999A` (0.6). Unchanged desired leaves hold untouched. Every successful call
copies selected Position128 into state and output. Changes alone reset C0,C4,
CC,C8 to zero, D0 to-1, and the supplied filter-current word to selected MaxFOV.

A change advances the original closed generator308030 exactly once:
raw32 = raw32 * 0x41C64E6D + 0x3039, wrapping32 bits; return raw32 & 0x7FFFFFFF.
The static ELF seed is1 and 308020 supplies explicit seed replacement. The
stationary flag is the strict test
100 * (binary32(2^-31) * binary32_signed_word(random31)) < Stationary,
with each conversion/multiply kept in original order. A100-percent endpoint can
still fail the strict comparison if rounding produces exactly100. RNG state is
caller-owned; other original game RNG consumers and their scheduling must share
that state when later integrated. It is not silently restarted for each camera.

Reset2D30B0–2D30C4 writes only current=0 and hold=0.6; it does not immediately
select nearest or reset previous/output/filter/RNG. SwitchEvery=20 from this
asset is loaded but is NOT used by this selector. Downstream mode selection,
the threshold getter and an additional game RNG call are separate consumers.

Native selection admits a loaded nonempty structurally immutable view, negative
or valid current index, finite supplied XYZ/delta, and reached finite squared
distances below FLT_MAX. Negative finite delta is not clamped. Only consumed
hold/MaxFOV/Stationary values are checked; W is copied bitwise. Unsupported
numeric/state inputs preserve state/RNG/output. Typed live disjoint objects and
unchanged view backing are caller preconditions, not pointer-probing diagnostics.

## Memory and preservation

Each native record is 136 bytes, including raw probability integers and prepared
thresholds. Exact TA resident payload is 2,664 bytes: 19 records plus 20 uint32
links. Host view 40 bytes, selector state 56, RNG 4 and output 20 are separate.
Parsing uses scalar fixed scratch and makes repeated bounded passes rather than
retaining the source text or a dynamic document tree. The original RES decode
buffer is a load-time extraction resource, not camera resident memory.

This tree comes directly from verified 49D, archive SHA256
`6d9ddcaf055456bd6c7c1a547e9fa4a501c65902a56e36e5d66cab18b99580a4`.
No unverified 49C source was copied. 49C remains separately frozen and unreleased;
the collision contract remains unresolved and untouched. Windows/Xbox targets
were not accessed, built or run; no installs or RAM access occurred.

Run `python3 tools/test_native_tv_camera 49e.py --output-dir /new/external/path`.
Add `--sanitizers --no-leak-check --stack-usage` for this host. CMake option
`RR_NATIVE_TV_CAMERA_ONLY` is supplied but unexecuted because CMake is absent.
LeakSanitizer is explicitly excluded for the tracing limitation. Original
source extraction, independent authored data, exact-word oracles, regression
reports, semantic review and compiler footprint are retained under
`evidence/tvCamera49E`; final results are in its `validation.json`.
