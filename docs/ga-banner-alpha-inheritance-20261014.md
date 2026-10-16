# GA banner alpha inheritance

The user identified black rectangles around Watchtower's hanging banners in the
fresh Windows Dud/GA screenshot. Source inspection establishes a missing alpha
cutout in the GA policy. Fresh Windows visual verification passed with the Ben/GA recovery below.

## Retained evidence

The original three PSMs match the identities already pinned in
`ga_atm_animation_index.inc`:

- s_flaga: a440549ae7fcaac1fb6b664ca3ba521fdccd825e24b80cb45190859f1686196a
- s_flagb: 8235c9527e1bb7be4667b5d0acb2661b10db53b81cd9e3087ddd757da84ce390
- s_flagc: 78cbdf98f3ecc14a0c70bc4dd8c8e765208546aee9dd7ec8488b5d873496b06d

They specify PreLitCompressed and their real texture-animation parameters but
omit alpha-test fields. The retained starting RRTEX frames match the committed
texture index hashes. Of 16,384 texels, their alpha-zero counts are respectively
8,872, 9,237, and 9,359. The transparent data was present; no texture repaint,
black-color key, or regenerated image is needed.

Authenticated APPSTART.RES SHA-256
`04eb376a507d0745925e4351903a8ef7a1024d37d8250b78892cba0c74220b89`
contains DefaultMT.psm SHA-256
`1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7`.
Its explicit alphaReferenceValue is 0. The existing recovered constructor fields
supply alphaTestOnOff=1, alphaTestMethod=6 (GREATER), and failure method 0 (KEEP).
Thus these flag materials inherit a GREATER-zero cutout. The previous GA policy
left sourceAlphaTest false when the pass omitted the field.

## Bounded correction

`selectGa` first retains the admitted GA material policy, then borrows the actual
loaded DefaultMT owner and resolves its four alpha-test fields with authored
pass overrides. Unsupported values, duplicates, invalid integers, missing owner,
and unknown or active runtime overrides are rejected before publication. The
native shader's existing GS-unit GREATER/KEEP path consumes the result.

The change applies to genuine admitted GA selections, not a hardcoded visual
exception for three names. Explicit alpha-test-off values remain off. Lighting,
fog, blend equations, depth policy, texture bytes, samplers, animation clocks,
geometry and original AI are unchanged. TA/MA/SA/GC policies are unchanged.
This does not establish full GS fragment precision or hardware pixel parity.

## Windows verification

The correction was built into `1791f95910c64e11af8707029756f25bdee1f3ba`.
The Ben/GA recovery completed successfully at 2026-10-14 13:19:41.246790 UTC.
The fresh track screenshot shows the hanging banners without their former
black rectangular backgrounds; authored fog and lighting remain intact.
Main, Rider, Course and track screenshots plus the requested 60-second video
were inspected. See `ben-ga-video-window-20261014.md` for exact evidence,
pre-recording baseline and limitations. No additional test was run for this
review or documentation update.

## Follow-on moving-world publication

Source inspection found a second GA material publisher in
`ga_dynamic_window_fixture.h`: the authenticated moving-world surfaces still
called the older generic GA/GC selector, and did not publish the alpha-test
fields. It now borrows the same live DefaultMT templates/runtime as the static
world, resolves `selectGa`, and copies the resulting test/reference into the
existing dynamic renderer span. The dynamic renderer already retains those
fields and uses the same shader discard path. No shader, texture, geometry,
animation or AI changes are required. The authenticated dynamic programs are
DiffuseCompressed; existing lighting and fog policy is retained.

### Follow-on Windows result

Source `34b352f1db0ffda9c6476c756daf6793428b7e46` built successfully at
14:21:24.876779–14:23:49.188961 UTC on October 14. The sole basic visible
Ben/GA smoke ran 14:24:22.250355–14:25:07.926789 UTC, exited 0 naturally,
and left zero owned processes. All four fresh PNGs and receipts were reviewed.
Baseline: 13.7962901776 FPS, 69 GPU-completed swaps over 5.0013445 seconds;
eight original AI racers, zero human/input/host-driver samples. No video hold
or recorder was used. This basic route result is not broad moving-object
visual parity or an isolated performance improvement.

EXE SHA-256: 5c715d3feec6cd810ec6919da588dbe80ad724ff833803e690e792bba91c0344.
Evidence: libfile_a818e0c518bc8191b886fff74eff5cf8.
Next routine eligibility: 2026-10-14 15:25:08 UTC. No additional test ran
for receipt or screenshot inspection.
