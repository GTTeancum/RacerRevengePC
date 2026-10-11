# Bounded 20-or-60-second capture: unexecuted

The source-only update permits exactly 20 or 60 seconds in capture_multitrack.py, with the default still 20 and exactly 20 requested captures per second. Receipt scope uses the requested duration, and completion requires measured duration to reach that target. The existing loop retains its target-plus-three-second stall bound.

The existing central reservation, exact source identity, shared lock, sole launch, two expected TA/GC sessions, menu flow and clock checks are unchanged. Density still requires at least 12 measured fps, no gap over 250 ms and at least 80% unique original images. There are no catch-up bursts, retries, resets or encoding during gameplay.

The adjacent clock contract, menu controller and clock gate are byte-identical to the previous bundle. Only capture_multitrack.py and encode_gameplay.py changed.

The artifact encoder admits a declared 60-second target only with a measured window of 60–60.5 seconds. Other targets retain the previous 15–30.5-second range. It preserves real acknowledgement timestamps, VFR timing, the 1 ms endpoint, no B-frame reordering and source-frame hashes. Actual PCM is included only within the existing 80 ms maximum-anchor and 50 ms final-skew limits; no resampling, stretching, normalization or synthesis is allowed.

This bounded duration change has not been imported, executed, syntax-checked or tested. Earlier captures and exports remain intact. Artifact preparation cannot launch gameplay or reserve another runtime slot.
