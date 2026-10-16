# Same-profile race constructor scalar facet

Source-only implementation, base cef8c0c. No runtime call, build, test, syntax
check or decoder execution.

The existing application-lifetime OriginalUiRiderProfileOwner50KG::Storage now
owns exactly the source constructor lanes7E4 bit0 and full word7E8. Startup
17E330..33C masks off bit0 and preserves the upper seven bits;17E348 stores zero
to7E8. Consequently the native facet represents bit0 as a boolean, without
inventing values for the unrepresented upper bits, and initializes7E8 to zero.
It is part of the SAME real profile storage already used by Rider/Course and
race selection. No second profile or race state exists.

Private beginPodRace1A66CC verifies the actual retained common-profile identity
and initialized owner before clearing bit0 and clearing7E8, in source order. The
source at1A66CC/D0 resolves Application382B80->2DC,1A66D8..E4 preserves upper
bits and clears bit0, then1A66E8..F0 resolves the SAME owner and clears7E8.
A native generation increment invalidates stale profile observations after this
mutation; generation exhaustion rejects before effects. No source meaning is
assigned to either lane beyond those authenticated writes.

Only the existing actual Application is newly friended to this private method.
No public setter, fabricated launch token or automatic frontend/page call is
added. Future genuine race construction must invoke this through the actual
Application2DC profile owner at its reached1A66CC boundary, after preceding
source constructor stores and before following1A66F4 stores. The method does
not validate or claim the absent full race constructor/lifecycle by itself.

This closes scalar storage and the concrete same-owner mutation. Remaining
race gates include authentic committed-launch allocation order, full17DF80
Entity lifecycle, race-owned resource load, canonical sibling attachment, and
unconditional race destructor display/texture/resolver operations. The existing
profile constructor is still an explicitly bounded fresh-profile subset.

Raw-word correction:1A666C is30070001 (ANDI a3,zero,1), yielding ZERO,
not DADDIU1.1A6674 supplies A2=-2 and A3 is not rewritten before1A66E0.
Both startup and race constructor therefore CLEAR bit0. This repeats the real
source store even when the current represented bit was already clear.
