# Inactive173EB0 allocation and first strip owner

This additive prerequisite ends at174090, before the first strip's23BEB0 setup.
It is NOT complete173EB0,224720, dynamic selection, packet submission, or runtime
activation. No build, test, syntax check, converter, gameplay or emulation ran.

## Implemented owner path

`original_racer_cable_allocation50la.h` is an explicit, currently uncalled native
adapter. It joins the existing persistent roster Cables to the retained chariot
model owner, exact Rig, real racer aliases, and shared Application counter/lock.
It refuses a noncold membership50 and an already-started prefix. Ordinary cable
mesh construction neither allocates this prefix nor changes its counts.

The count is recovered from original20F710/20F94C and211C08..211CC0: racer258
starts1; a primary engine's genuine second anchor sets it to2 only when that
side has no bottom engine. The adapter performs those same retained-PSG named
lookups and writes the actual racer's258.214120 contributes258 for each present
primary engine and1 per bottom engine. The bounded existing roster has both
primaries and3/5 models. Count is never copied from the connected mesh output.

The persistent Cables owns the new allocation object; racer254 aliases it only
after173CC0's actual Entity constructor. The Application is pinned, and the
same423F60 counter receives both constructors. Source Entity kinds are explicit
`podCables173CC0` and `strip23BCA0`; their28/2C notification table entries were
verified as the existing base methods. No caller count, ready flag, source
address, copied racer, or replacement Application is accepted.

173EB0 writes count3C, samples38=9, scale54=6.5, then separately allocates58,5C,
60,64 vector arrays,30 strip-owner backing,6C enabled backing, and68 vertices in
the original order. Vector1048E0 values preserve W=1. Source-undefined enable
entries are not initialized or exposed. Native null unique_ptr slots are only
allocation bookkeeping; no source claim is made for unconstructed strip entries.

Only the reached first iteration writes6C[0]=1 and constructs its real23BCA0
owner. That owner embeds a genuine Entity reference/counter and the150320
count34/format3C/color44/flags48 state. Source-uninitialized40 remains absent.
Its original material30 and packet-related fields remain genuinely unconstructed
at this cut. This is not a boolean-only substitute for strip flags48.

The operation returns `stripSetupRequired` at174090; there is no generic
success/ready predicate. No later strip, basis70..7C,141960 strip attachment8,
or detached7F8 attachment1008 is skipped or fabricated. The new racer254 guard
prevents containing-racer cleanup while this concrete prefix remains live.

## Partial teardown

Call `closeSelectionAllocationPrefix` before closing the racer/Application.
Only unlinked, reference1 components are admitted. A successfully constructed
first strip has not yet reached141960, so native partial-construction unwind
explicitly invokes its represented23BCF0->1503B0->1413E0 prefix before freeing
the30 pointer backing. Its material/packet fields are still null: there is no
reference to release or renderer operation to invent. The same Application
counter is decremented once per constructed Entity.

The parent then releases allocated6C,58,5C,60,64,30,68 backing in173D40 order
(70..7C were never allocated), performs its1413E0 decrement, and invalidates the
native racer254 alias. Allocation failures retain the successfully reached
prefix for this explicit cleanup. Destruction fails closed if an unrepresented
attachment or extra reference appears. Closed allocation remains terminal;
destruction cannot silently retry source construction.

## Exact next boundary: real material and packet retention

The next call174090 is table3D1EA0+74=23BEB0, with literal3A7DF8 `cable`, type7,
one group, and18 vertices. Its base150450 allocates38 and calls actual slot9C
23D750 (color setter), then constructs a2461E0 primitive and calls2465B0->299BE0
with the actual Application3833E8 material context. The existing AIDamage-only
screen-material adapter does not authorize this cable path.

After that call23BEB0 owns23C050 group data,23CB20 setup, counts/array60, a133F80
packet builder,23CCA0 packet creation and134200 finalization, then publishes
primitive10/flags0F/field22 and attributes. Those concrete owners and their
destructors are not represented by this prefix. Consequently **no material
numeric reference or packet is acquired or claimed here**. The previously
loaded immutable cable texture bank is not substituted for these resources.

After complete first-strip setup,174098's real racer3C branch may resolve
`textures\\cable` and `textures\\cableGhost` and invoke the primitive's slot14.
The actual racer3C ownership and this material change must be joined before
advancing past that branch. Only then may174108 append the strip and initialize
its attributes. Later iterations must follow that same order. Complete224720
also still needs the genuine shared lazy fields and render/graph lifetime join.

## Central review checklist

- Check count258 default and second-anchor writes; verify exact source strings.
- Confirm caller must include the explicit adapter; no runtime callsite added.
- Review source array order, first-only enable write, W lanes, and first-strip
  counter; ensure uninitialized entries cannot be consumed as source data.
- Confirm174090 returns a blocked status, with no later attachment or fake
  material/packet retention. Preserve actual Application identity and all AI.
- Close the prefix before racer teardown; review allocation-failure cleanup and
  terminal lifetime. Existing retireRaFreshVehicle predicate is unchanged.
- Root alone integrates and validates. Exact preimages are in MANIFEST.json.

Evidence is static source-word labeling only. Original SLUS SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Each annotated range/table/string carries its source-byte SHA256.
