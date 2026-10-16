# World74: same-owner construction and retirement gates

Source-only recovery at d83ae96. No build, test, decoder or guest execution.

## Exact reached constructor

Race1A6BC0 allocates550 bytes;1A6BD4 calls1B2870 with A1=1, then
1A6BDC publishes74. This precedes racers1A6C88 and B8/BC/C0.
1B287C calls26FC90, which calls141300 before installing scene table3D2BB0.
The base sets30=1 and invokes its own slot78=270550 to initialize actual
scene fields. It then allocates60 bytes and calls2350D0 with the actual
Application context's audioA0 and A2=1. This is another counted Entity:
2350D0 ->102620 ->141300. Publication4D8 and virtual30 load follow.
Only after that does the derived constructor install3CF110, initialize the
500 list with capacity10/growth100000/count0 through1B5F40, zero its proven
510/514/518/51C/528/52C/530/534/53C/540 fields, set524=3 and clear434320.
Do not clear unrepresented fields merely because native allocation is zeroed.

Raw authenticated tables establish:
-3D2BB0: slot8=26FD80, slot10=142590, slot30=270660,
 slot78=270550, slot9C=271C10.
-3CF110: slot8=1B2920, slot10=142590, slot30=1B2AB0,
 slot78=1B2AA0 (tailcalls270550), slot9C=271C10.

## Existing concrete native owners

ra_cold_aggregate50LA::Owner already inherits the single ra_pod_scene50LA::Owner.
That scene owns lighting_, runtimeQueries_, trackParticles_, container4BC_,
ten visibility4C0 nodes, ten collision4C4 nodes and environment443108_.
constructRuntimeTrackParticleManager reaches constructTopology then
particlePredecessor1B4DC0 on this same object. Canonical collection rows are
borrowed from the existing lighting/query owners, not copied.
ra_static_world50LA::Owner is a separate raster/material resource child and
cannot stand in for the original counted world Entity.

The future world Entity must therefore be an additional private prefix on
this SAME scene owner, admitted before these existing counted children and
before racer constructors. It must not reconstruct the scene or relabel its
already-counted children as newly constructed. The existing application,
session generation, source archive and material domain must remain identical.

## Typed retirement differs from aggregate close

Race invokes world slot10:142590, whose zero-reference destruction reaches
1B2920. Derived teardown frees510 records (each three owned arrays),514,
53C, paired52C/530 arrays indexed by528, then the500 list, before26FD80.
Base26FD80 invokes271C10 to retire reader34, then releases World4D8 through
its actual slot10. It retires scene-owned arrays and typed resources, clears
actual globals including443108, releases counted arrays4C8/4D0/4CC/4D4,
reaches148110(global426F60), conditional global443148->280920,
actual context103DD0, final arrays4DC..4EC and finally1413E0.
These global and typed effects cannot be replaced with a base Entity decrement.

Current aggregate.close has honest native dependency guards (camera/world
borrows, navigation leases, scene topology, images, particles and visibility
leases), but is not a receipt for this exact source destructor. Keep its
existing safety until each reached original resource is mapped to its owner.

## Immediate prerequisite: genuine SoundGroup ownership

World4D8 mixer binding currently represents group levels and pause, not a
2350D0 counted owner. Constructor writes volume38=0, group40=2,state44=0;
its actual list48 has capacity8/growth16/count0. Source load2351F0 scans
process434A10[25] for its first free slot, reserves it, writes that index40
and volume38=100. Six race group loads precede this world group load.
Do not hardcode index6 or allocate a world-only private slot registry.
Current RaceAudioGroups::initialize publishes later configured race levels;
it cannot be used as proof that this earlier constructor/load already ran.

Source235150 clears the actual allocated registry slot, calls2353D0, frees
list backing and calls102670->1413E0.2353D0 snapshots real logical-sound
members, invokes each slot20(false) then slot10, then clears the original
list. Native voice activity alone is not proof that this logical list is empty.
The finite implementation must stage the SAME group state before configured
levels, retain the actual process slot authority and logical sound membership,
and preserve failed/partial owners. Until that exists, no full world74
construction or clean retirement can be truthfully admitted.
