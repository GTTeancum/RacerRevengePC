# WatchtowerRun original-AI fresh selection

Source-only candidate against `c6814e73ccfd179c26c700a4337f5005a1d8b0b2`.
This closes the fresh GA course identity, AI creation class, and course-record
pod join. It does not certify a runnable GA track or a completed lap. No builds,
compiler/syntax checks, tests, asset decoders, or gameplay were executed.

## Exact source mapping

| Original value | GA WatchtowerRun | Retained GC TheCitadel |
| --- | ---: | ---: |
| CourseIndex / selected track ID1125F low nibble | 8 | 10 |
| Descriptor11278 + 12 * track ID | 112D8 | 112F0 |
| Position returned by187B10 in the fresh course-order table | 3 | 10 |
| AI creation class from16E590..16E5B4 | A / 1 | C / 3 |
| Fresh course-record pod11280 + 12 * track ID | 10 | 17 |
| Class1 override to profile RepairTime1064 | 2 seconds | no override |

These are three distinct indexes: Course control, source track ID, and fresh
course-order position. The source order is
`{0,11,4,8,7,1,5,9,12,6,10,3,2}`. Never infer the AI class from CourseIndex8
alone, or copy GC's class C and record pod17 into GA.

The AI creation class is also a different source decision from the PHY handling
class selector214954..214AA0. The existing handling selector already receives
the actual lower-case selected track name; this candidate does not replace it.

## Original instructions

- Original Course `track.dat`, Control2, explicitly contains CourseIndex8 and
  Planet=gamorr. Widget10 is the representation and widget28 is the track name.
-17E6C4..17E6C8 forms3A89F8 in s0. Original ELF bytes at3A89F8 are `ga\0`.
-17EAF8..17EB1C copies those three bytes to descriptor112D8,
  exactly11278 +12*8.17EB30 stores localization key107 at112DC.
-17EB3C loads10 and17EB44 stores that byte to112E0,
  exactly11280 +12*8.16E4AC..16E4CC later masks selected1125F to15,
  multiplies by12, and reads this signed byte.16E4D0..16E4DC compares it with
  the actual racer's pod60 before setting FE4.
-17F0F8 loads8 and17F100 stores it at1013C,
  exactly100C4 +40*3. The other course-order byte stores are preserved in
  the accompanying original source excerpt.
-187B10..187B5C scans those13 source rows in stride40 and returns the
  zero-based position of the selected low-nibble track ID. GA returns3.
-16E590 compares that position with5. For GA it selects class1 via16E5A0.
 16E5C8 supplies binary32 word40000000;16E5CC writes2.0 to1064 after the
  original16E630 .ai profile load. Classes2/3 retain authored/default values.

The independent instruction CSV omits187B44,16E5A4,16E5B8. Their actual ELF
words are retained explicitly in evidence instead of inventing CSV coverage.
Those alternate entries are not needed to infer the ordinary GA class1 path.
The recovered full course-order excerpt also matches532 original ELF words.

## Native implementation

`AiFreshCreateSelection50IU` is an immutable fresh-profile projection. Its
binder accepts only GA or GC, case-insensitively, and preserves the caller's
output on failure. It has no saved-profile or live menu override. The existing
strict GC-only helper remains available unchanged for compatibility.

The native session binds this selection once from the actual admitted track
identity, before any racer tick. Every actual cold .ai profile is still read
from its own original pod archive. The selected source class is applied after
that read. Each real racer uses its real pod60 and the selected course-record
pod for FE4; no selected-player substitution or synthetic racer is introduced.

GA admission additionally requires a fresh race, all eight original-AI owners,
no checkpoint, and no original/host menu start. Thus camera slot0 also takes
the original-rig/.ai cold path. Existing all-AI control gates continue to reject
RR_AUTODRIVE and skip controller/keyboard input. No driving input is generated.

Only inside the allAiEnabled readiness branch, the sweep now reads each actual
original-AI owner's racer60/FE4 and requires that comparison to match the
selected record pod. It also verifies the class1 RepairTime2 value. Mixed-GC
and non-AI slots acquire no new runtime identity/repair read dependency. These
checks read scalar subranges of the already registered AiRuntimeState; they
do not create a second live state owner.

Source16E4CC uses signed LB for the course-record byte and16E4D0 compares it
with the actual pod60 word. Both admitted record bytes,10 and17, are positive
signed bytes. Every admitted original-racer identity is a positive source
pod ID from the existing original-racer catalog. The native signed cold
comparison and the runtime unsigned comparison therefore have identical
equality results on this admitted domain. This candidate does not admit or
reinterpret high-bit, negative, sentinel, or loaded-save record bytes.

Telemetry adds an `original_ai_selection` line containing track, source track
ID, order, class, course-record pod, and explicit fresh/no-loaded-save status.
Each original_ai racer row also publishes its actual FE4 and RepairTime1064.
All existing original_ai_all counters and eight-racer ownership checks remain.

## GA authored input contract

The independent track-assets candidate must provide the authenticated GA.RES,
its actual `tracks\\mga.spl`, `tracks\\aga.spl`, and `tracks\\ga.tun`.
Their existing native readers already consume the same selected byte buffers
as the geometry/navigation owners; this candidate adds no substitute route.

Direct text inspection gives:

- MAP NumNodes88; AI NumNodes107
- GA AIDriving Lookahead0.7, RacePace60, RacePaceCutoffTime2,
  BehindPlayerClearance50, StartLineBoostTime0
- GA PodDamage WaitToRepairEnvTime5, RepairEnvThreshold0.5,
  RepairEnvUntil2.5, AIEnvPointsMultiplier1
- Packing counts4/4, distances1250/500/750/0, speed percentages0.5/1

These are evidence, not new hardcoded runtime values. GA and GC differ in
RacePace, its cutoff, and RepairEnvUntil. Loading the selected original TUN is
required. The A/M geometry, node flags, branch links, and DistanceToFinish
continue through their real existing native readers.

## Deliberate remaining boundaries

- Source data binding is not admission of missing GA collision, height,
  materials, sky, animated objects, native lights, or trigger owners. The
  track-assets integration owns those providers and their fail-closed checks.
- RR_RACE_MODE2 remains required by the current original-AI manager join.
  Numeric fastest-lap/race records and mode1 event/record integration are not
  supplied by this course-record-pod change.
- The current runtime still stops at unrecovered original AI/reset/contact,
  lap/finish, and world boundaries. No callback success or reset is fabricated.
- The all-AI roster is the explicit native zero-human schedule, not a claim
  that the original human-dependent retail roster root was reimplemented.
- Source handling, original AI decisions, physics, and CPU/GPU rendering stay
  native. No guest CPU/chip execution, emulation, or original executable is
  linked or run by this candidate.
- Original Course directional selection and menu-to-race publication are not
  opened. Frozen menu-check source must not receive these hunks until released.
