# Source-backed D44 material playback semantics

## Result

The AI material route is real. At224100 the receiver is vehicle.D40, not vehicle.
224130/224148 invoke D40 vtable+A8 with2/3. AI constructor16D8F4..16D8FC
installs3CE130; table3CE1D8 is16E200. 16E200 is raw0805E41C, an unconditional
jump179070. AI vehicle+A8=173CB0 being a no-op is irrelevant to this route.

An isolated header-only implementation is staged at
src/game/original_pod_material_audio.h. It is a source-only candidate, NOT built,
syntax-checked, imported, tested, decoded, played, or executed. The central checkout
was read only. The parent owns all cold ownership and runtime integration.

Original ELF SHA256:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
VA-to-file mapping is VA-0x100000+0x80. New raw reads are retained under evidence/.

## Hard metadata boundary

233E60 calls233E90 then returns logical sound+50. 233E90 only resolves while
sample28<0 and stream1A8=0; it calls26DB10, stores28, and on success reads duration
26C200 into20 and pitch26C240 into50, copying50 into40/4C. 26C240 reads the u16
sample-table field at this+(sampleId>>16)*8+0x382.

The producer is268930. It issues IOP RPC16 at26931C, reads reply+300 at269324,
and stores the returned halfword to sample table+382 at269338. There is NO EE
sampleRate-to-pitch arithmetic here. No IRX input is present in the retained input
set. Existing docs/audio-assets50hc.md already documents this missing IOP boundary.
A conventional SPU2 conversion is not evidence for this title's RPC response.

Thus decoded PCM and VAG header rates cannot authorize basePitchKnown=true.
Constructor50=4096 is proven, but resolved sample50=4096 is not. The core reports
sourceSampleMetadata at233E90 before293010's cache write/pitch RNG draw or a
233410 start that requires it. Earlier source effects and variant RNG remain
committed, and the owner is blocked against accidental replay. This prevents
claiming original AI playback/RNG completion with host50HI's native4096 stand-in.

Once actual metadata exists, material-only rendering uses sourcePitch/basePitch50
against decoded-rate PCM. Source fields and RNG still use the actual source word.
Do not use the generic logical wrapper's unconditional sourcePitch/4096 conversion
when the established source base differs. Source-positive sample IDs, duration,
and metadata resolution chronology still need the actual parent-owned backend.

## Cold setup and array ownership

-1A3CAC..1A3CE0 constructs eight distinct0x250-byte D44s at manager.C0[0..7].
 Each receives2C21C0(manager+50). The manager's packet+A8=0 near and+AC=450 far
 are written1A3C98..1A3CA0.
-Manager constructor1A39F0 writes vf0 at+60 (1A3A14/18),+70 (1A3A1C/20),
 and+90 (1A3A24/28). These packet vectors are(0,0,0,1), not four zeros.
 2911E0 copies them to D44+170/+180/+1A0. Other unconsumed packet words need
 not be fabricated. setupApplied is only the represented numeric setup boundary,
 not a stand-in for unrepresented141C20/global object registration.
-2C20F0 calls2928B0 with actual AuralScape, actual Effects110 group, category1,
 then writes vtable3D4BE0,240=244=-1,248=null,24C=0.
-2928B0 calls290F10 and initializes three actual dynamic arrays: collision+200,
 drive+214, and unused-by-these-paths+228. Collision/drive initial capacity1,
 growth16; activeDriveKey23C=-1. The arrays are per D44, not shared live sounds.
-2C21C0 copies the0x70-byte manager packet, calls292A90 with flags2, trigger20,
 chance100, a3=0,t0=0, and returns this irrespective of downstream V0.
-292A90 copies that packet again and calls2911E0. It does NOT populate sound
 arrays. 2911E0 calls141C20, requires1D8, writes packet160..1C0, flags158=2,
 trigger1DC=20,chance1E0=100, clears1D4 bit3. It leaves bit2 false for flags2/a3=0.
-279C80 populates collision rows ascending1..authored count, calling virtual80
 (292AF0). 279F60 does drive rows ascending1..its independent count via virtual8C
 (292CB0). Both log a false append but continue their loops.
-292AF0 calls232DB0 with group=this+30, interface=this+1D8->A0,
 filename=parameter-row base, flags1,t0=1. Only a nonnull result allocates and
 appends{sound,key} (8 bytes). No ghost entry is appended on factory-null.
-292CB0 fixes flags0x81,t0=1 and appends36 bytes: sound0,key4,sampleSpeed8,
 minVolumeC,minSpeed10,maxSpeed14,resonant18,slope1C,cachedSlope20=-1.
 Six extra values come directly from authoring row+1D0..1E4. No other generic
 authored sound controls are consumed by this factory route.
-Source232ED0 initializes38=44=100;3C=48=0;40=4C=50=4096;
54=58=1;5C=.25; position60 and relative90=(0,0,0,1);80=0;28=2C=-1.
233110 applies loop1B0 from flags0x80; therefore collision false, drive true.
The group44=0 constructor projection is already preserved by cold50KY. Group44
must be ONE shared actual Effects110 field, not duplicated per sound.

Numeric lookups are positional: key in1..constructedCount selects array[key-1].
Stored authoredKey4 is not searched. Factory-null therefore compacts subsequent
indices. Parent strict all-resource admission avoids this case without inventing
aliases. Repeated filenames share immutable PCM only, never logical/playback state.

## Runtime methods and ordering

179070 reads actual local position via22A180 and copies main-body velocity70.
For1/2 it reads vehicle354->130->A9, using-1 for null contact. No contact returns
without altering onset bit3 or stopping a drive. Argument2 sets D40.22C bit3 on
first contact, calls collision once, then drive every reached tick. Argument3 clears
bit3 and calls D44 virtualA8=2C27D0. Speed is trunc(100*vehicle70).

2C2850 computes minimum planar distance over the actual Aural listener array,
writes1D0, clears1D4 bit1, then sets bit1 iff distance<540. Zero listeners keeps
FLT_MAX and clears eligibility. No camera proxy is created by the core.

2C2240: if24C!=0, return1. Null position enters diagnostic branch. Otherwise
2C2850, reject noneligible, then draw variant1..4 from shared308030. Form key=
4*material+variant, lookup collision by virtual88=292C60, and invoke104140 with
near0/far400+speed. On true spatial result, player58=0 and material>0 generates
source event3; AI58=1 skips that branch. Clamp minimum speed20 and call292B80.
The AI entry admits58=1; the human entry described below admits its real58=0
and completes the source event3 enqueue before292B80.

292B80: reject out-of-range key with source0. Clamp volume0..100. Lookup actual
sound, call233710; if active return0 WITHOUT changing raw volume or position.
If inactive:233830 writes38,233DB0 copies XYZ preservingW,233410(flags0) starts.
The earlier104140 call may already have updated spatial fields on an active sound.

2C2530:24C/non-position/540 gates first. If material240 matches, use remembered244;
104140 near0/far450, then minimum speed50 and292F30 only if spatial V0 true.
If changed, call2C27D0, draw new variant, store244 then240, lookup drive via293470,
104140 then minimum speed50 and292DA0. Those selected fields stay changed even
if the spatial test fails or the selected drive key has no constructed owner.

292DA0: reject key outside drive count. If key differs from23C and previous23C
is active, call293330(previous). Lookup target and check233710 BEFORE293010.
An active target suppresses position writes, source pitch RNG, raw setters, and
start. Otherwise copy positionXYZ, calculate293010,233830 volume38,
233A50 pitch40 (out-of-range false is ignored), write23C, then233410(flags0).
23C is written even if source start returnsfalse.

292F30: reject invalid key; there is NO active guard. Calculate293010 first,
then positionXYZ,2339A0 pitch4C,233750 volume44. Even a naturally ended drive
consumes pitch RNG and receives logical field writes, but is NEVER restarted.
2339A0 accepts0..16383, returnsfalse if inhibited, calls backend only if active,
then stores4C.233750 always clamps/writes44 and controls the backend only if active.

293010: calls233E60. If cachedSlope20==-1, integer-divide base50/sampleSpeed8,
convert tofloat, multiply slope1C, truncate and cache. Consume one shared RNG draw
scaled by cachedSlope+1. Pitch=trunc(float(base50)+float(wrap32((speed-sampleSpeed)*
cachedSlope-random))). Volume is the retained piecewise float formula, clamped
0..100 then truncated. Cached slope is persistent per actual drive row.

293330 valid key: writes23C=-1 before active check; stops only if active.
2933D0: iterate EVERY actual drive row1..count and stop active sounds, then23C=-1.
2C27D0:2933D0, then240=244=-1. It is incorrect to stop only remembered244.
293470: bounds-check independent drive count; return row.sound or null.

104140: no listeners returnsfalse without mutation. Otherwise computes nearest,
102950 intensity/angle, THEN calls233AE0 on any nonnull sound EVEN WHEN102950
returnedfalse (then attenuation0/angle0). It returns the saved102950 V0; the
233AE0 result is ignored. It does not pre-copy event position into sound60.
Starts(flags0) later overwrite44/48/4C from38/3C/40; continued drive setters
later overwrite raw pitch/volume while retaining the spatial angle.

## Differences from host50HI

1. Live logical sounds and distinct per-D44 arrays participate before source RNG.
2.292DA0's actual active test suppresses293010; generation-first reduction cannot
 know this and can consume a forbidden RNG draw.
3.Stop-all visits all constructed drive owners. An old244-only stop is weaker.
4.23C is distinct from material240/key244 and has its own source write order.
5.104140 modifies spatial state even on far failure; position ordering matters.
6. Source base50 controls integer cached slope and pitch RNG range. Host4096 is
 explicitly a presentation convention, not provenance for original sample50.
7.Missing drive array key retains240/244 but produces no293010 or pitch RNG.
8.Reached unsupported work is partial-prefix/blocked, not a transactional rollback
 of original shared RNG and logical sound writes.

The material-only transport admits the actual constructor multiplier54=1.
Independent changes to that field require real pitch-only/volume-only native
operations before generalized pause/rejected-pitch behavior can be claimed.

## Retained prior evidence used

- original-camera-owner-candidate-20261012/frame-schedule/ai-emitter/
  constructor-setup.words.txt; boost-event.words.txt; spatial-setup.words.txt
- engine-band-candidate-20261012/source-evidence/binder-update-224000-224730.txt
- sa-ma-material-audio-candidate-20261012/evidence/
  material-construction.txt; material-source-loader.txt; material-owners.txt;
  material-lookups.txt; null-sound-spatial-gate.txt
- original-ai-audio-candidate-20261012/
  sound-factory-complete.words.txt; logical-sound-start.words.txt;
  logical-sound-active.words.txt; logical-spatial-controls.words.txt;
  logical-volume.words.txt; sound-pitch-setters.words.txt

## Integrated native ownership and rendering policy (2026-10-12)

The actual `OriginalPodCollisionResponseOwners::Emitter` now contains the sole
material arrays and 23C/240/244/248/24C storage. The existing physical-reset borrow
points into this same 24C. `2C2810(0)` still does not stop/restart material audio,
clear array state, or change 240/244. Constructor `291134` writes distance1D0=0;
`2C2850` later replaces it with its real nearest-listener result.

The pool applies only the source-backed numeric manager+50 setup projection
before assigning a racer. Position/velocity/ancillary vectors are VF0, near/far
are 0/450, flags158=2, trigger1DC=20, chance1E0=100, and setup clears bit3 of1D4.
Unconsumed packet words, `141C20`, and full global object registration are not
claimed. The actual source-created arrays are a separate cold step.

`native_original_ai_audio::Owner::prepareMaterialArrays` borrows the selected
material provider's already-authenticated catalog and PCM bank. Each racer gets
its own logical sound for every constructed authored row, including duplicate
filenames; immutable PCM alone is shared. Collision rows precede drive rows.
Actual keys and six drive parameters are copied without reinterpretation. Native
slot ownership is an exclusive suffix: 220 + racerOrdinal*44, at most44 rows.
TA has8+8 rows, GC12+12, GA20+20, MA16+12, and SA32+12. Existing0..219 reservations
are untouched. The maximum native mixer capacity becomes572. These are host
logical-playback slots, not original hardware channel IDs.

The actual shared `RaceAudioGroups50IC` now owns source group44, initially zero;
rows borrow that stable Effects110 word. There is no per-row copy or parallel
group namespace. Existing volume ducking preserves it. The newly admitted
material-only path retains logical sound multiplier54=1; a generalized original
pause/multiplier lifecycle is not claimed. Channel cleanup precedes bank and
mixer/group destruction. Failed cold construction is never retried; any completed
prefix remains visible until normal teardown. The native live policy requires
all actual authored rows to be available before race activation. An unavailable
row reports the real cold boundary; it is never replaced with a sample or an
invented drive entry. Independent D40/D44 reset fields remain canonical.

The hot `179070` kernel now dispatches the real `224130/224148` channel tail after
the particle commands for each of the eight racers. The D40 native event identity
is checked against that racer's actual engine and same D44. Position is actual288
LOCAL XYZ with the caller VF0.W=1; velocity is actual340 body70 including70.W.
ContactA9 is read from the real current spring candidate only on channel2.
Channel3 preserves its source lazy stop-all branch and does not require contact.
The explicit existing native listener is the committed selected body listener;
retail Aural/listener root scheduling parity is not claimed. The former camera-only
host material subscriber is disabled in ALL8 and the actual human Course row0
to avoid duplicate updates/draws. Other unjoined human modes retain that subscriber.

Original `233E90` sample metadata is still unresolved. Constructor4096 and the
decoded PCM sample rate do not prove its IOP RPC16 base-pitch result. Every cold
row therefore retains `basePitchKnown=false`; `basePitch50` is never promoted to
resolved source metadata. The owner explicitly selects the separate
`nativeDecodedRateUnity4096` policy before dispatch. Its effective base4096 is a
host-defined unity scale for integer slope/pitch quantization and rendering ratio
against the real decoded PCM rate. Exact source pitch quantization/hardware parity
is not claimed. The default strict policy remains available and still rejects
unknown source metadata instead of silently selecting native policy.

The native source-volume backend admits normalized zero rate as silent paused
playback: it retains the active token and cursor and resumes on positive controls.
It neither fabricates an original start/stop nor claims IOP/SPU pitch-zero output.
Other ignored source pitch-setter failures retain their actual prior40/4C values.
No playback, source RNG, or hot material fields are advanced during cold setup.

This native pitch policy introduces no RNG call and changes no material event,
active, or missing-key guard. Each admitted293010 still consumes exactly one
shared word, even for cachedSlope20=0. Drive owners loop, so their positive-rate
rendering duration cannot retire them; zero-rate native playback also preserves
that active guard. Collision variant RNG precedes its active guard and collision
playback consumes no pitch RNG. Restoring genuine ALL8 material events does add
their original draw sites compared with the former camera-only subscriber, so
later shared-RNG-driven physics/AI outcomes may change from that old incomplete
schedule. This is the restored event chronology, not additional policy RNG.

The missing static inputs are the original modules named by the authenticated
ELF: `PS2SOUND.IRX`, `LIBSD.IRX`, and any needed sound-provider dependencies (the
ELF also names `PANICSYS.IRX` and `IOPRP242.IMG`). No modules will be executed.
Workspace filename inventory plus ZIP member-name inspection of all seven
retained input-recovery archives, including48F and50HS, found no IRX/IOP payload
or retained disc image. The complete member-name search receipt is retained with
the source candidate. Further original-file acquisition is a separate boundary.

The prior `RR_AUDIO=0` logical-owner/software-mix policy remains in force:
ALL8's actual owners and clock exist without a host device or WAV sink. Cold
material ownership uses those same owners and is not gated on audible output.

No builds, tests, syntax checks, imports, decoder execution, gameplay, or guest
execution were performed. Manual source review found no additional CMake or
Windows source-manifest entry needed: both new code files are inline headers,
and the Windows script already includes them in its recursive dependency hashes.

## Human Course material dispatch (2026-10-13)

Human19EBA0 installs vtable3CECA0; the actual table word3CED48 is1A0120.
For arguments1..3,1A013C branches to1A0220, which calls179070. Argument4's
separate boost behavior is unchanged. Human admission never constructs an AI
engine, changes vehicle58, or sets nativeAiOwnersBound50IV.

The already-bound `OriginalHumanEngineResetOwner50LB` and actual manager D44
row0 now enter the same source material kernel as the AI rows. The human's
canonical `boost.flags22C` owns onset bit3 alongside boost bit1 and reset bit2;
there is no subscriber copy of22C. Its actual D44 owns23C/240/244/248/24C,
the collision/drive arrays, and the same24C borrowed by physical reset. Cold
array construction now covers all eight real rows in a mixed race; row0 uses
its existing220+0*44 exclusive native voice range. Immutable authenticated
catalog/PCM and the explicit decoded-rate unity4096 pitch policy are reused.
Original RPC16 metadata remains unknown.

After2C23E8's successful spatial result,2C23F8 reads actual vehicle248/58.
Only a human with positive material A9 reaches2C2420/2C2458. Source1A39B0
constructs exactly `{word0=0,type4=3,word8=0,actor0C=vehicle,actor10=0}`;
the existing real vehicle33C FIFO receives it before292B80's active/missing
sound guards. Native enqueue resolves that same human ordinal/racer60 and
retains1A64B0's actor3C suppression/capacity behavior. Failure blocks this
material owner without replaying committed flags, spatial fields or RNG.

At the accepted-world FIFO flush,3AA270[3]=1A4D94 calls1A52A0. The complete
original1A52A0 body is `jr ra; nop`. This genuine event3 no-op is acknowledged
in FIFO order; no wall-hit speech, controller feedback, music, or RNG is added.
The material onset/drive random draws remain at their existing source sites.

The real224130/224148 event now targets the human D40 after its particles.
Course no longer also runs `material_audio50HI::Provider::update`; the provider
continues to own the authenticated immutable catalog/PCM used by these rows.
The zero-contact branch and channel3's lazy stop behavior are preserved. The
existing native listener/phase policy and other documented parity limits remain.
Only raw ELF reads, source inspection, textual patching and hashes were used;
no build, test, syntax/import/lint/sanitizer, decoder or gameplay run occurred.
