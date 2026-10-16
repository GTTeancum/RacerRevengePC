# Native one-piece wreck physics frame callers

## Delivered scope

`original_wreck_frame50la.h` composes the real canonical one-piece owner with
the existing flight, settling, body-cache, collision registry and renderer
owners. It adds no persistent body, model, collection, collision table or
effect owner. The two entry points implement the reached base calls:

- `advanceOriginalWreckPiece166E40_50LA`: PhysicsObject `166E40`, called by the
  derived piece `2CB8F0`.
- `advanceOriginalWreckGroup2CA840_50LA`: PhysicsObjectGroup `2CA840`, called by
  the derived group `2CC1B0`, for the actual one-piece/capacity-zero owner.

The genuine flying, initially-disabled and bounded nonflying branches are
joined. Nonflight executes the real detector and static generated-contact
response using canonical records/body/query owners. The piece and capacity-zero
group retain their distinct source continuations; see the [world-query](original-wreck-nonflight-world50la.md),
[response](original-wreck-response50la.md) and [group continuation](original-wreck-group-nonflight50la.md)
reports. No supplied contact result, fabricated miss, terrain shortcut or
successful-step callback discharges a remaining boundary. Surrounding
collection/Session scheduling and unsupported source routes remain separate.

## Actual caller order

### Piece166E40

1. `166E58..166E74`: read actual application `382B80->11C`. Nonzero returns
   actual V0=1 immediately; no globals, owner fields, dt, renderer or workspace
   are consumed by this source branch.
2. `166E78..166EC8`: lazily initialize shared guard gp8538/value433B68 to
   raw `3C888889`, then gp853C/value433B70 to `FFFFFFFF`. Even the disabled
   branch performs these writes. The maximum-step value is not read by the
   admitted flying or disabled branches.
3. `166ECC..166ED8`: write actual piece124 and128 to positive zero, then read
   the canonical D0.enabled204. Zero jumps directly to167228/141ED0. It does
   not settle a disabled body or produce queries/forces.
4. `166EE4..166F04`: only both actual34 and38 nonzero reach166790. Call the
   integrated real force/water/body-observer/E8 repeated movement-query path.
5. `166F08..166F70`: compute scalar norms of actual D0.C0 and D0.D0 into
   piece124 and128. Operation order is Y*Y, X*X, ADDA(X²,Y²), MADD(Z,Z),
   SQRT.S. Reuse the matching16786C scalar helper. Preserve the124 store if
   angular arithmetic subsequently reaches a native admission boundary.
6. `166F6C ->167218..167220`: call genuine167640 settling. The E0/E4 updates
   at1671B8/1671C0 belong only to the nonflight branch and are not run here.
7. `167228`: dispatch the actual piece component child list through141ED0.
   Body167B80 refresh follows settling, even if settling just disabled204.
   It must not be moved ahead of settling or inferred from204.

### Group2CA840

1. `2CA85C..2CA878`: same actual application11C pause gate and V0=1 return.
2. `2CA87C..2CA89C`: separately initialize shared gp8E18/value444230 to raw
   `3C888889`. This is not the piece433B68 global.
3. `2CA8F0..2CA90C`: clear primary40.piece124/128, then read primary3C204.
   Disabled body jumps directly to the group's141ED0 at2CB3EC.
4. `2CA910..2CA9A4`: both primary piece34/38 nonzero run genuine166790,
   followed by the same actual C0/D0 scalar norms. Otherwise the real
   `2CA9C4..2CAD50` nonflight path runs primary detector295630, clears group38,
   skips the capacity-zero attached loops, resets impulseC0 from4441F0 and
   performs forces/response/integration substeps using444230. Movement166C60
   and the contact-dependent airborne timer follow. No post-movement E0 update
   is inserted; the next source operation is the common root E4 update below.
5. `2CAD50..2CAD58`: always run primary E4 `11FF60`, which is exactly
   `11FE50` preparation followed by `120230` membership update. The native
   query owner performs both real kernels. E0 is not updated here.
6. `2CADA0`: capacity34=0 skips the attached E4 loop. Then2CADB4 calls
   genuine167640 settling. The passed primary3C/40 references are checked
   against the same physical and piece authority owned by the aggregate.
7. `2CADBC..2CADD0`: execute root model14C `28A700(null,stack40,stack50)`.
   `refreshSceneWorld49P` performs the actual dirty-cache refresh; world
   look120 and up110 XYZ are copied into the ephemeral vectors, preserving
   the original stack W=1 lanes. Publish canonical physical.body.root.scene
   and sceneFlags190 alias after the getter.
8. `2CB3D8`: capacity34=0 skips attached fracture/RNG/placement work.
9. `2CB3EC`: execute141ED0 on the actual group component. A fresh one-piece
   group has no component children, so it does not refresh the primary body
   cache here. The piece is a sibling owned by the collection, not a child
   of the group. This distinction is retained even though group flight uses
   the piece's physical body.

## Real ordered component scheduling

`141ED0..141F8C` first tests parent.descendants24&2. If clear, it returns1
without reading child18. Otherwise traverse the actual linked children.
Eligibility is original29 bit0 set, bits2/3 clear, and own mask20&2.

For each eligible child the implementation calls the existing
`serviceOriginalRenderer244D60(actualFields, false, actualGL, diagnostic, v0)`
against the actual Session renderer borrow, then dispatches the concrete
child slot38. Disabled-D8/empty-queue routes are that real service's branches.
Nonempty queue and callbackDC guards remain; glFinish or a caller receipt is
not substituted. The source child call ignores renderer V0.

Only this owner's actual D0/body167920 component has an admitted child
method. Its existing `refreshOriginalWreckBodyFrame50LA` implements167B80:
body320 nonzero skips integration, then real14FC00 orientation cache and
167BC0 speed cache run, followed by its actual empty141ED0 child route.
The component is selected by canonical address and kind. A reached other
method stops after its real renderer prelude at141F60. next14 is read only
after the actual child completes, preserving source order rather than a
snapshot scheduler. Topology/domain checks protect native borrowed storage;
there is no invented source iteration count or fake child.

## Binding and result contract

`OriginalWreckFrameGlobals50LA` is one application-owned projection shared
by all these callers. `OriginalWreckFrameBinding50LA` borrows actual
application11C, those frame globals, the already-shared166790 flight globals,
the existing graph-body workspace, and the Session renderer fields/GL and
diagnostic. These references must remain alive and serially exclusive. A
fresh value must not be supplied on every frame or per wreck.

The actual owner must have completed genuine construction. Entry points are
called at their respective base source call, after any outer component
renderer prelude; they do not silently select an inactive piece or invoke
both group and released-piece updates in one frame. Outer activity29 and
mask20 selection remains the real caller's responsibility.

`complete` and `nativeReturnKnown=true,nativeV0=1` certify only this executed
base function, including source pause. They are outputs, never provider
inputs. They do not authorize skipping the derived2CB8F0/2CC1B0 suffix,
particle forest, renderer work or Session guards. Result fields distinguish
completed prefixes; a later failure retains actual earlier mutations.
Do not replay the entry point after a partial failure. Existing numerical
finite/normal-or-zero, FE_TONEAREST and strict binary32 limitations remain
unchanged. Disabled or paused branches do not prevalidate unused dt.

## Remaining concrete boundaries

- Nonflight query/response routes outside the bounded direct static
  SearchList domain, including point-terrain, dynamic callbacks and unsupported
  world shapes/methods. A genuinely absent fresh-track terrain330 makes
  pointContacts48 false but does not make E0 queries disappear or infer a miss.
- Reached renderer queue/callbackDC methods outside the existing244D60
  service's admitted branch, or an unexpected eligible child method.
- Existing real flight limits, including supported target/callback providers,
  registered world ownership, water and observer routes.
- Nonzero attached group capacity and its genuine fracture/effect ownership.
- Derived piece/group effect suffixes, actual collection dispatch and any
  Session/racer40 state joins. This packet does not change their guards.

## Evidence and verification

`evidence/format_words.py` is a copy of the authorized read-only scalar MIPS
formatter, changed only for this output directory and exact address spans.
It reads the original SLUS_202.68 bytes with the established
`address - 0x100000 + 0x80` mapping. Raw unformatted VU/COP1 words remain
included. Evidence covers166E40..167250,2CA840..2CB420,141ED0..141F90,
167B80..167C40,28A700..28A890,11FF60..11FF90 and244D60..244E50.

Verification was static source/word review only. No build, compiler, syntax
or import check, test, lint, sanitizer, gameplay, asset decoder or PC action
was run. The central checkout was not edited.
