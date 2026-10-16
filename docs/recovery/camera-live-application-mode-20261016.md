# Consumed race camera reads the actual Application22C

The actual Application constructor13FDBC and race load1A6AC8 store1. The previous
native camera DisplayInputs copied literal0 even on the newly consumed route.
Its source mode observation now borrows the actual Application scalar and reads
it at each of the three existing fade/intro-exit calls. Application outlives the
camera and DisplayInputs. Legacy callers without the optional pointer retain
their prior copied mode policy.

Retained source annotations under render-entry114d00 show scheduler phase3 at
1404CC around Application26C virtual40, followed by clear0 at1404E4. Other phase
boundaries similarly clear0 at140568,140740,140790 and14084C. Those are actual
callback boundaries, not evidence for assigning0 after arbitrary native load.
This change does not manufacture any of those callbacks or their reset stores.
The existing native videoRequest implementation rejects mode6; actual mode1 is
admitted, so there is no need to substitute0 for the new source-owned route.

Static source reading only; no execution, builds or checks.
