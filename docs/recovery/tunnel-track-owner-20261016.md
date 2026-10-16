# Genuine TunnelTrack ownership over the retained native navigation

Source-only bounded implementation; no runtime construction, race publication,
load completion, parent insertion, input activation or tests are added.

## Actual source

Race load constructs mapB8 at1A6F38..54 and aiBC at1A7068..84. Both allocate78,
call2A1160(A1=1), publish the returned owner, then call2A1260. Constructor2A1160
calls141300 and installs table3D39B0. It initializes array30 and array44 null,
scalar34/3C/40/48/74 zero, and ten words4C..70 zero. It does not initialize38.
Unlike the enclosing race constructor, this finite constructor establishes its
own typed-destructor allocation gates.

Raw table3D39B8=2A11E0;3D39C0=142590. Slot10 first follows original Entity
release142590, including lock423F58 refusal, topology unlink and104970 reference
release. Only the final reference dispatches typed destructor2A11E0. That
retires array30 through reverse2A1100 node teardown (node next-array6C), frees
shortcut array44, then calls1413E0 before optional storage deletion. The implemented
admission is detached, childless, reference1; no generic descendant callback or
linked sibling operation is skipped under that admitted topology.

## Concrete retained data

Existing RA ColdNavigationLease strongly retains the authentic DecodedSources
owner, with map/ai TrackNavigation50EU and their inline node/next arrays.
OriginalTunnelTrack50LA retains this SAME sealed lease; navigation() returns the
same map or ai address. No second immutable map, array copy, parser invocation,
guest allocation layout or duplicate AI state is introduced.

Application weakly observes at most one map and one AI owner. Its constructor
factory reserves native storage and canonical enrollment capacity before141300,
uses the same actual Application object counter/authority, preserves the actual
session generation, and leaves the projection unpublished. The separate
bindTunnelTrackColdProjection2A1260 verifies the actual observed owner and
adopts its retained map/AI projection only after finite native navigation is
present. This is a native projection join at the future reached source load
stage; it is not evidence that full2A1260 or all fallback branches executed.

The private constructor stores retain actual source scalar zeros. Source array30 has no independent native heap allocation: its represented
node/next data are already owned by the sealed lease. Array44 is an independent
shortcut-pair allocation, not nodeIndex78. Loaded adoption calls a private lease
check using the existing authoring scan over actual retained memberBytes[1] for
map or[2] for AI. Only absent or integer-zero Track.NumShortcuts is admitted.
Source2A13EC skips the count40 store on missing lookup, preserving constructor
zero;2A1C60 then skips the allocation. Nonzero, invalid, duplicate, or otherwise
unsupported authoring rejects adoption before effects. No fixed RA absence flag
or replacement shortcut data is introduced. Actual retained mra.spl and ara.spl
were statically inspected and omit NumShortcuts; no parser was executed. Before typed Entity retirement the owner
revokes its projection and releases that lease; other legitimate source clients
retain their own leases. The existing source owner keeps native storage live
until its last real borrower retires. No persistent GPU storage is involved.

## Retirement and remaining integration

release142590 rejects a live lock or changed topology/reference before effects.
For the admitted terminal route it removes canonical enrollment immediately
before reference1→0, releases the navigation borrow, then decrements the actual
Application count at1413E0. Destruction fails closed if an admitted Entity cannot
retire; no fake detached repair or silently dropped original count occurs.
A constructed owner may retire before projection adoption because2A1160 itself
proved its null-array gates. A failed native bind preserves the actual owner.

Future actual race load must create B8 before BC at the authentic counted
allocation boundaries and retain these same owners in race fields. They cannot
be constructed during speculative RA resource prewarming or at the later
Camera Session constructor. Actual race destructor must release B8 then BC
before manager reset1AB4F0. No race parent exists yet, and this patch does not
mint Application28C or claim enclosing1A6840 disposal is complete. C0 tuning,
manager13C/native transfer retirement, other reached race children, and exact
launch/parent/scheduler joins remain separate gates.
