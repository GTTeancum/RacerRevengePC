# Same-lease raceC0 tuning ownership

Source1A7184 calls1AAB30 after the real B8/BC spline load stages.1AAB4C/50
allocatesA8,1AAB54 publishes raceC0, and the finite following stores initialize
tuning before1AAD14 calls1AA5D0 for authored overrides. This is an uncounted
allocation, not another Entity constructor. Race destructor1A68CC testsC0 and
1A68D8 frees it AFTER the independent1AB4F0 manager reset.

The already-authenticated RA source bundle reads the same tracks\\ra.tun with
existing original authoring kernels. Its immutable records cover all reached
semantic fields of C0:

-00..10: aiImpact.thresholds00[5];14..24: aiImpact.severities14[5].
-28..38: dynamicImpact.thresholds28[5];3C..4C: dynamicImpact.points3C[5].
-50: dynamicImpact.podGrindingPoints50; source1AAC64 defaults1 and
 1AA8A0 reads PodDamage.PodGrindingPoints when present. The SAME existing
 dynamic authoring reader now retains this previously uncovered field. Its
 provenance is appended at index13, preserving previous indices0..12.
 Missing authoring keeps the genuine constructor1; malformed present data fails.
-54/58/5C: minGrindingSpeed, aiPointsMultiplier, aiMultiplierDecrement.
-60: aiImpact.aiEnvironmentMultiplier60.
-64/68: finalImpact.threshold64/increment68.
-6C..88: aiTuning repair, lookahead, pace and start-line fields.
-8C/8D: signed packing bytes;8E/8F are untouched padding, not invented values.
-90..A4: aiTuning packing distances and speed percentages.

The owner retains one SAME ColdNavigationLease. No separate C0 bytes, default
array, navigation copy, AI state or immutable source owner is introduced.
Application::retainRaceTuningCold1A7184 derives that lease from its actual
observed mapB8 andaiBC owners, requiring identical leases/generations and exact
projection identities. It cannot accept a caller-filled tuning record or a
replacement readiness flag. Source parsers have already authenticated the
actual retained RA member; the owner does not execute another decoder.

Const getters borrow those real records. release1A68D8 revokes this owner's
lease; other actual clients keep their independent source leases. Calling the
leaf alone does not prove manager reset or full race teardown. No runtime calls
or race28C publication are added. This maps the reached semantic C0 fields into the SAME retained source; it
does not claim completion of the surrounding race load or original heap ABI.

Source-only implementation and review; no builds/tests/syntax checks, decoder
execution, gameplay or guest execution.
