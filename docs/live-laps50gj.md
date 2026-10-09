# Live development lap tracker 50GJ

The 50DU development client composes the actual TA map's five authored
ProgressNode markers with the native 50GF accepted-crossing effects. 50GH's
navigation entry point executes AI sequential, map sequential, AI fallback,
then map fallback in original order. Each map callback runs before the source
C8C/C88 index stores. An uncompleted effect rejects the drive rather than
silently acknowledging it or advancing navigation past it.

The client explicitly chooses config mode 2 for 50GG completion. This is a host
development configuration, not recovered menu state or applicationByte58.
It has no timing/record queries or race-finish controller. The mandatory keyed
owner-selection call really executes against six actual POD01 p_event.ini
entries. An explicitly disabled native audio provider supplies Unavailable at
the source resource-resolution boundary; source registration, publication and
negative-resource playback-failure behavior are represented. No sound playback
or available audio hardware is claimed. After the actual callback completes,
50GF resets FF8 and navigation continues.

HOST LAPS shows the represented completed CC8 count, capped visually at 999.
It does not add one, infer a race result, or claim original HUD parity. Native
CC8 itself is not capped. Its cursor and owned CD8 bits survive host checkpoint
sessions; a deliberate grid restart creates fresh race/cursor ownership. The
source reset-prefix effect runs on fresh/checkpoint setup. The dynamic wrong-way
update 1A2E30 remains unjoined: this is a scoped authored-marker/node-zero lap
tracker for the development drive, not complete original wrong-way, shortcut,
record, race-end or ranking behavior. In particular, do not interpret successful
route wrap as proof of the absent rules.

The released 50GD executable and archives remain unchanged. The new core lap
sources and navigation hook compiled on Linux. A focused check using the real
TA marker table passed 17 assertions: missing markers do not count, all five
markers plus node zero count once, actual unavailable-sound owner dispatch
completes, FF8 resets, repeated node zero alone does not count again, and the
joined hot crossing/callback sequence allocates no C++ heap storage. This is a
component check, not a gameplay run. Live validation is separately recorded in
solo-live-laps50gj; no result is assumed until it finishes.

## First actual live run

The frozen binary 736131f709ae36d3f59e8cca47d6b5486ff8a2f0c80d17ff2f070ff10d776e55
ran for 184.085 seconds, reached HOST LAPS 1, completed its one actual
unavailable-sound callback and quit with exit 0. One manual checkpoint at
anchor82 retained marker cursor4. The subsequent sampled map97→7 progression
with nine sequential advances and unchanged scan reconstructs traversal through
node0 and the next lap's first marker; the instantaneous cursor5/node0 frame was
not directly sampled. Both zero and one HUD JPEGs are committed live captures.
9,952 successful physics steps retained the C++ allocation guard; no failure.

PERFORMANCE UNRESOLVED: all34 completed-FPS samples range0.583777–62.7919.
Two intervals fell below10FPS (1.24557 and0.583777), both before the lap callback.
No cause is established. This is a feature preview with a known severe stall,
not a replacement claim for the delivered50GD performance result. The current
next action is focused per-phase wall/thread-CPU measurement. No Windows test
was performed; no broad regression or sanitizer result is implied.
