# RA trigger cold authoring and exact entry boundary

Source-only candidate. No tests, builds, syntax checks, conversions, gameplay or central edits.

## Cold artifact

`ra-trigger-authoring.patch` adds only `tests/native/ra_trigger_authoring50la.h`.
`Owner::decode(shared_ptr<const ra_cold_sources50LA::Owner>, Diagnostic&)` extracts the exact TOB from the already authenticated, retained RA archive. It verifies 95558 bytes and SHA256 `7336c6bab40265adb5e82aec8ed3ee66669c19b26a597cfacedddfca4be48524`, uses the actual existing parameter decoder and integer-only original decimal parser, and publishes a const two-record array only on completion. It retains source/archive and decoded TOB/parameter backing. Failure is terminal and keeps an unpublished prefix; no actions or mutable race state are touched.

Source evidence: `ryloth-closure-2240/packet/RA-readiness-source-inventory.json`, parameter_members/tracks/ra.tob and member_identities. The candidate validates exact bytes itself when eventually called; this work did not execute it.

| Source | Zone | Position | Radius | LostAI target |
|---|---:|---|---|---:|
| trigger1/rightSkull |4|8040.589844,2228.016113,8484.408203|170.587753|29|
| trigger2/leftskull |4|7809.741699,2201.172119,8290.046875|169.766968|104|

Both author StartEnabled TRUE, OneTimeOnly FALSE, RandomTriggerPercent100.000000, IgnoreTrigger PLAYER, one LostAI action. Both omit Reset; existing recovered1B5A60 default is false. Exact decoded AI track has113 nodes, so both actual targets are bounded.

## Real action and contact owners required later

- Original1B5340 PLAYER resolution visits actual manager+34 source roster prefix via1AF330 and reads each main690. It is independent of AI class58, spectator/camera choice and native array slot. Empty prefix remains empty.
- Original1B27C0 traverses race identities and writes only C80 on a matching main690. Secondary collider contact may participate in lifecycle but must not write C80. Reset=false makes no pose/speed/C84/C88/C8C changes.
- Existing `triggerLostAi50IN` supplies this scalar callback. A future adapter must borrow the actual C80 only inside that racer's synchronous Bank visit; no pointer survives into a different arena visit.
- Existing `TriggerEntryState50IN` and contact CC/D0 storage supply real prior-contact state: duplicate suppression; reuse null holes before extending high-water; actions before reciprocal adds; service removes reciprocal entries before exit callbacks, preserving holes/high-water. Percent100 consumes no random word.
- Candidate publishes no trigger identity, native group, C80 destination, Entry event, registration or geometry policy. These cannot be certified merely by authentic TOB fields.

## Additional world extent

The retained complete TOB inventory has two trigger instances (the two action blocks are not instances), four type8 particle emitters, one sunflare, seven camera records and673 static-model parameter records. The latter contain only SLT/allowShadowReceiving in the inventory and are not673 new collider placements. The archive has no MOT resource or authored animation-object block in this retained inventory; no additional animated compound collider has been established.

The four emitters attach raFire.emt to FireP0t01..04.slt at offset0,15,0, startingzone1. Their eventual particle/query owners must be inspected separately; mere presence does not justify allocating four dynamic collision records. Sunflare/camera/terrain metadata likewise do not prove additive static collider registrations.

Thus178 SCB colliders plus two authored trigger candidates is a known subset, not the complete live registry extent. Actual race body/query/wreck/camera/particle needs remain separate owner joins before final allocation.

## Original shape5/type2 entry: concrete blocker, not a generic missing algorithm

Static listings in this directory come from original ELF words as data only.

1. `121410` loads actual type2 shape+8C and writes its normal-mode+54 at121438. Shape switch table388AF0 maps shape5 to121884.
2. `121884–1218B0` loads candidate sphere shape+8C; passes the type2 root+40, pair of source pointers+4C/+50, actual prepared type2 matrix, sphere radius+50 and stored squared radius+54. It calls108E90 directly at1218AC with t0=0.
3. `108EB4–108EF4` splits high-bit leaf tags. Leaf pointer is descriptor[1]+20*(tag&0x7fff). t0=0 chooses1095D0, while the existing camera query uses selection1 and the earlier branch.
4. `1095D0–1096C8` copies the current sphere point, transforms through10A470, reads three signed normal shorts at leaf+0C/+0E/+10 scaled by exact0x38000100, and calls106ED0 for triangle vertices. It admits abs(signed plane distance)<=radius. `1096D4–109740` projects onto the plane and calls109F10; success returns true.
5. Remaining three edge tests project/clamp onto each triangle edge in source order and accept strict distance<radius (last result109C0C–109C1C). This is not122BA0 root overlap. Node branch109C28–109E58 uses actual36-byte node center/half, strict squared-radius box distance and child0-before-child1 short circuit.
6. No previous pose or swept-endpoint argument is supplied by this direct branch. Existing CC/D0 is prior-contact bookkeeping; do not manufacture a previous matrix to replace the leaf.

### Why native type2 rays cannot yet be supplied to that leaf

`126D38–126E18` allocates32 bytes per endpoint pair into type2+50, reads XYZ at+00/+04/+08 and+10/+14/+18, and leaves the two W words outside those reads. In contrast,108E90 uses20-byte triangle stride and signed normal/index fields from that very base. The direct caller does not transform these endpoints into triangles.

Both106ED0 and109F10 dereference external vertex descriptor3DFE60, including descriptor+8 vertex-format selector. Source108E90 does not establish this global. A bounded aligned-word scan found stores at1089FC (t3),10A4E4 (t0 wrapper),10AF68 (t2 wrapper),10B724 (t0),111304 (a3 wrapper); their instruction context confirms LUI3E/base offsetFE60.1218AC bypasses10A4E0. Earlier ordinary type2-vs-static1089F0 stores its target vertex descriptor, so the retained global context can depend on preceding actual query order. This scan is a lead, not proof that those are the only indirect writers.

This independently reconfirms the retained `docs/gc-trigger-source50in.md` blocker. Native `Type2ResourceView49X` deliberately exposes typed32-byte endpoint pairs and explicit opaque W inputs; it has no authenticated20-byte triangle or external vertex-context view. Treating those rays as a CollisionMesh48I, inventing normal/index records or using a current static mesh as the global descriptor would fabricate source input.

## Smallest faithful next join

First establish the actual global3DFE60 owner and preceding collector/query chronology, plus bounded interpretation of the actual type2 leaf bytes including W words. That requires an explicit valid vertex descriptor/format/extent; use neither a guessed selected course mesh nor a default descriptor. If this path genuinely consumes historical/opaque memory, keep that dependency explicit rather than declare output parity.

Only after that proof, a boolean sphere kernel may share the numeric `Arithmetic`, `inside`, `edgeHit` and node overlap code currently private in `original_camera_sphere_query.cpp`. It must use abs(distance)<=radius, return first boolean hit, avoid selection1's front-face restriction/prior-distance update/contact packet, and retain the real decoded vertex format. Existing camera API rejects reverse/quantized storage and cannot be called unchanged.

No RA endpoint-policy adaptation or runtime activation is proposed by this patch.
