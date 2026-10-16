# Ordered positive wreck smoke/fire loop

Source-only increment against9e99915. No build, test, syntax check, gameplay,
project decoder or guest instruction execution was performed.

## Source order and continuation

Authenticated SLUS_202.68, VA-to-file mapping VA-0x100000+0x80:
2CB6CC..2CB7C0 creates smoke, publishes160[index], loads/appends smoke,
creates fire, publishes164[index], loads/appends fire, then invokes smoke84
at2CB794 and fire84 at2CB7BC.2CB7C8 increments the pointer byte offset by4;
2CB7CC increments the index;2CB7D0/7D4 compares against the real count174;
2CB7D8 branches back to2CB6CC. Final fall-through is2CB7E0.
No anchor170 read occurs inside this child-construction loop.

The existing constructFirstPositiveSmoke/constructFirstPositiveFireAndStops
APIs now process the next pair of the first actual piece. Each call takes a
fresh real external owner. Smoke/fire written counts advance at the actual
pointer stores BEFORE setup, and the pair index advances only after both
Stop84 calls. A failed setup retains its constructed Entity, array pointer,
manager registration if reached, and the terminal failure cursor. It cannot
be replayed as the next pair. Capacity/count rejection before construction
leaves the cursor and all source writes unchanged.

All iterations retain precisely the first pair's external domain descriptor,
backing pointer and capacity. Prior pair pointers must still be members of
that same domain and retain the actual particle parent. Existing source
reference/count/leaf/tail retirement is reused unchanged in reverse append
order: last fire, last smoke, then preceding pairs. Piece array destruction
never destroys those children. No extra callback or manager cancellation is
introduced.

## Native identity admission

Canonical components can borrow the address of their actual EffectOwner.id.
This is native lifetime metadata, not an invented guest field or a second
control owner. Smoke/fire and the existing ordinary four-effect owner publish
that borrow with genuine Entity construction. Known effect kinds lacking a
nonzero canonical control identity are rejected before a new constructor.
New control IDs are checked
against every live component in the supplied external forest; key checks
cover that forest plus the current piece prefix. The smoke/fire retirement
ends this metadata borrow only after the actual Entity reference reaches zero
and the live-domain row is removed. Historical controls and manager emitter
rows remain unchanged.

The external domain contract is essential: it must include every live effect
whose IDs share this native control namespace. This patch cannot authenticate
an omitted independent forest or a separate manager's arbitrary control IDs.
It does not claim process-wide uniqueness from a partial caller list. A later
active scene join must supply the complete canonical forest/identity domain;
there is still no active driver entry.

## Remaining reached boundary

Once all count174 pairs are stopped, the result is effectDustRequired2CB7E0.
The next source dust sequence is allocation2CB7E4,19A5E0 call2CB7F8,
168 store2CB800,19A720 setup2CB820,Stop84 call2CB834,178/17C zero writes
2CB83C/840,append2CB84C. Splash follows allocation2CB858,19DBE0 call2CB86C,
16C store2CB874,19DCD0 setup2CB898,Stop84 call2CB8AC,append2CB8BC.
Both need separate external owners with authenticated members and matched
19A6C0/19DC50 retirement. Splash's actual null a3 setup proves optional250
unallocated; common forced-set1C4 still needs authenticated zero-count proof.
Group/collection suffix, effects runtime dispatch, and destructive gameplay
remain unjoined. No completed constructor or active death/KO is reported.
