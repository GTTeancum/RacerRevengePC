# Same-owner SoundGroup constructor/load, bounded empty retirement

Source-only, base d83ae96. No tests, build, syntax checks or decoder execution.

RaceAudioGroups50IC now has an inactive counted cold route on its SAME mixer
and volume38/state44 arrays. Existing initialize behavior is unchanged unless
this owner actually entered that route. No second group state is introduced.

constructNextCold2350D0 accepts six original race groups in ascending original
110..124 order, then World4D8. Caller must stop after six, construct the real
world Entity and then reach its26FCF8 group constructor. This leaf does not
claim that interleaving, actual race parenting or world construction occurred.
The six race groups are not attached to an invented parent. The owner pins the
actual Application Entity lifetime and enrolls every real counted prefix with
its unchanged session/key. Native backing and slot/capacity preflight precede
all count effects; each admitted leaf completes constructor and load.

2350D0 calls102620 ->141300 with source count argument1;10265C writes volume0.
2358D0 initializes the real logical-member list count0;235900 reserves8 entries
with growth16.235128..138 writes group40=2 and state44=0.2351F0 loads by
scanning the SAME process434A10[25] authority, reserves first free, then writes
its real allocated index and volume100. The native process authority belongs
to ApplicationEntities; actual BSS startup zeroes this address. Enum values
are never substituted for source IDs. Final configured levels are published
only by existing initialize after all seven cold groups have loaded, on the
same mixer/thread/epoch. Its successful return is not a race-load receipt.

The source group vtable3D1AA0 has slot8=235150,10=142590,30=2351F0,
74=2353D0. releaseEmptyCold142590 requires original reference1, detached
actual topology, matching process slot/session/key, same live serial mixer
and no native voice binding or activity. It unregisters before104970, clears
actual434A10 entry before2353D0, frees real list storage, then reaches the
102670 ->1413E0 count decrement. The zero member count is concrete: no member
insertion API exists on this cold route, and canBind explicitly rejects cold
owners before any provider voice can bypass that missing membership join.
Source2353E8 skips its snapshot/stop/release loop on this actual empty list.
No logical-sound callback is replaced by a voice count or a successful no-op.

Any retirement closes construction/configuration admission for that group set.
Owner destruction fail-stops if any counted prefix remains; partial owners
must be retained and explicitly retired, not silently discarded. Source load
slot exhaustion is rejected before construction; an unsafe slot40=-1 owner
is not admitted. Legacy configured owners never enter these new paths.

Remaining integration gates: genuine context audioA0 ownership/driver join,
actual race and world interleaving and attachments, populated logical member
insertion/removal and typed callbacks, process audio groups outside this native
route, and actual world/base teardown. The process table must be shared by any
future authentic frontend/sound-group producer, never inferred empty from
current mixer inactivity. This patch adds no runtime calls or Pause activation.

The raw native mixer is a caller-lifetime borrow. Its actual owner must outlive
every explicit counted group retirement; retaining Application does not pin
the mixer. Source six-group attachment is141960(race,group,-1), children of
the race rather than siblings of it.
