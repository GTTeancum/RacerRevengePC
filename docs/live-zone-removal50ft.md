# Live portal removal investigation 50FT

## Result

No source-supported production fix was found. `live_zone_membership50en.cpp`
and `.h` are unchanged. The retained original words really assign the secondary
head from the searched node's next pointer, even when that node is not the
head. Neither the caller nor the constructor enforces head-only removal.
Changing traversal order, unlinking through a predecessor, or rebuilding all
overlaps would change the retained source behavior.

The existing transactional `UnsupportedLinkRemoval` boundary remains necessary
under the current contract, which requires every live secondary registration to
remain owned and reachable. This investigation does not claim a fix or wider
playable track coverage.

Only static source inspection, byte hashing, and documentation were performed.
No build, native test, PC executable, guest execution, or `ee.bin` was used.

## Retained source provenance

Read-only input: `work/native/project/disc/SLUS_202.68`.
Whole-file SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The code ranges below are file-backed at `VA - 0x100000 + 0x80`; ends are
exclusive. Decoding was checked against the raw instruction words, including
branch delay slots. These are static observations, not runtime results.

| Range | Purpose | Raw SHA-256 |
| --- | --- | --- |
| `120230..120284` | Collision caller | `beb38caa7cdeec66ff944b8b05e043ea8fa529a3f8d143c0058c6f3a791b86a3` |
| `27F3B0..27F414` | Prepend incident portal link | `c1608d05397282851aa6b7aba441307222eff1ee4e9e30f52250d535d0984609` |
| `280BF0..280D40` | Add every overlapping secondary | `a26ae8b90a998aaa22978ea4d85e0ffdb3e89e437126cb0a56a3dd1dff1f5709` |
| `280D40..281020` | Crossing/primary update caller | `d510a6edd97da6a1aaf6a6c230aed424fe8d91b16fc5879de75fa081c8aed096` |
| `281020..281380` | Secondary maintenance | `3e94d97c1dd8f5c32ee6d5ea75f5b7973a90833377199d9a32ae19ec5e9cacec` |

## Pointer identity and removal

`281078: 7080AE28` copies the primary record pointer `a0` to `s5` with `por`.
There is no subsequent assignment to `s5` inside `281020`. It is not an address
of the predecessor's next field. `s2` independently walks the secondary chain.

The decisive integer words are:

```text
2810FC 8EB20000  lw s2,0(s5)          # start at primary.secondaryHead
281100 8E430004  lw v1,4(s2)          # secondary zone
281104 8E020004  lw v0,4(s0)          # incident edge's adjacent zone
281108 10620007  beq v1,v0,281128
28110C 00000000  nop
281110 8E520000  lw s2,0(s2)          # advance without a predecessor
28111C 1640FFF8  bne s2,zero,281100
281120 00000000  nop
...
28113C 1040006C  beq v0,zero,2812F0   # overlap miss
281140 00000000  nop
281144 8EB30000  lw s3,0(s5)          # a hit updates HEAD, not found s2
...
2812F0 12400011  beq s2,zero,281338   # absent secondary: no removal
2812F4 00000000  nop
281308 8E440008  lw a0,8(s2)          # searched node's registry method
28130C 8E460014  lw a2,20(s2)         # searched node's slot
281310 8C990008  lw t9,8(a0)
281314 8F39002C  lw t9,44(t9)
281318 0320F809  jalr ra,t9           # unregister searched node only
28131C 72C02E28  por a1,s6,zero       # object identity, delay slot
281320 8E420000  lw v0,0(s2)
281324 3C010044  lui at,0x44
281328 AEA20000  sw v0,0(s5)          # primary.secondaryHead = found.next
28132C 8C243138  lw a0,0x3138(at)
281330 0C04160C  jal 105830           # release searched node only
281334 72402E28  por a1,s2,zero       # delay slot
281338 8E100000  lw s0,0(s0)          # next incident edge, forward
281340 1600FF67  bne s0,zero,2810E0
281344 00000000  nop
```

Thus a chain `H -> ... -> F -> T`, with a searched miss at non-head `F`, becomes
`T`. Only `F` is unregistered and released by this path. The preceding nodes,
including `H`, are no longer reachable through the primary record, but their
registry entries were not removed. The native adapter correctly refuses to
publish that state rather than silently abandoning those registrations.

## Constructor and caller ordering

- `27F3D8`, `27F3E4`, and `27F3F8` load the old zone incident head, store it as
  the new link's next pointer, and replace the head. Together with ascending
  authored creation, incident edges are in reverse authored order.
- `280BF0` follows that incident chain forward (`280CF8`, `280CFC`). Every
  admitted overlap creates a secondary, saves the previous secondary head in
  its next pointer (`280CEC`, `280CF0`), and publishes it as head (`280CF4`).
  Secondary order is consequently the reverse of the overlap scan order.
  There is no one-secondary limit or early exit after the first insertion.
- After a primary crossing, `280E04..280E60` unregisters the old primary and
  walks/removes its reachable secondaries. `280EC8` then calls `280BF0` using
  the current cached radius. This path can create several new secondaries.
- With an unchanged primary, `280FDC` calls `281020` once. Its incident scan
  starts at the same zone head (`2810CC..2810D4`) and follows the same forward
  links. It does not first reverse or rearrange the secondary list.
- `120230` prepares bounds at `120254`, reads the actual `object.E0` at
  `12025C`, reads `object+0x80` as radius at `120260`, and calls `280D40` at
  `12026C`. It supplies no alternate list cursor or removal order.
- Collision construction really does pass zero radius at `120170` before
  `120188 -> 2809C0`. That does not prevent multiple overlaps being inserted
  on a subsequent primary-zone crossing with the nonzero cached radius.

Reversing one list in the native caller is therefore not a correction to a
misread source traversal. It would also affect hit processing, insertion order,
registry reuse order, and which primary crossing wins on a multi-portal walk.

## Finite counterexample, derived without execution

This is an analytical counterexample to a general head-only invariant, not an
assertion that these are the retained TA track dimensions or a captured failing
gameplay frame. All coordinates and the radius below are exactly representable
in binary32.

Use three zones and two authored portals, in this order:

1. Portal A connects zones 1 and 2 at `x=0`.
2. Portal B connects zones 1 and 3 at `x=1`.

For either plane `x`, use the ordered vertices
`(x,-10,-10), (x,10,-10), (x,10,10), (x,-10,10)`.
The normal is positive X and the center path stays strictly inside the four
edge bounds. Zone 1's incident scan order is B then A. Use a fixed cached
radius `0.625` and centers with `y=z=0`.

1. Attach at `x=-0.5`, initial zone 2, initial overlap radius zero. There are
   no secondaries.
2. Update to `x=0.5`. The segment crosses A into zone 1 and does not cross B.
   The previous-zone guard prevents crossing straight back through A.
   `280BF0` finds both planes at distance `0.5`, first B and then A. Head
   insertion creates secondary `A(zone2) -> B(zone3)`.
3. Update to `x=0.25`. Neither plane is crossed; the primary stays zone 1.
   The first incident edge is B, whose distance is now `0.75 > 0.625`.
   Its searched secondary B is non-head, while A still overlaps at distance
   `0.25`. Source `281328` would assign head from `B.next`, which is null,
   leaving A's registered object unowned by the primary chain.

This uses the source collision caller's zero-radius attachment and fixed-radius
updates. No resize, teleport, reversed authored ordering, malformed topology,
degenerate geometry, registry exhaustion, or exceptional float value is needed.

## Remaining blocker

The source instruction behavior is unambiguous; a safe native implementation
that both preserves that behavior and preserves the adapter's reachable
registration ownership is not established. Source-level predecessor erasure
would be an intentional behavior change. Literal publication would abandon
live registrations, alter later candidate membership, and leave unregister/reset
without the original links required to clean them up.

The next useful evidence is a captured failure's object identity, current and
old center/radius, primary zone, secondary chain, and incident portal geometry,
checked against the original bound producer. That can determine whether an
upstream native input error causes the actual gameplay case. The generic
counterexample means caller ordering alone cannot justify removing this guard.

## Possible literal representation, unimplemented

If a real route reaches this case with verified source-correct inputs, one
concrete option is an explicit, bounded world/lifecycle-owned ledger for
source-abandoned secondary nodes. This is a possible representation of the
original result, not permission to implement it now:

1. Stage the full chain prefix preceding the searched node F. Preserve every
   prefix node's registry owner, slot, object identity, and existing link words
   in ledger entries marked `source-abandoned`, distinct from reachable nodes.
2. Unregister only F, preserving the source SearchList free-slot order. Assign
   the reachable head to F's next exactly as `281328` does and release only F.
3. Do not include abandoned nodes in the object's reachable collector chain.
   Do retain their actual registry words, so other objects querying those zone
   registries see the same entries the original left behind. Do not reuse their
   node capacity or silently delete their registrations.
4. A later primary crossing or `281490` removal should walk only the reachable
   chain, as the original does. The ledger must therefore outlive a membership
   detach/re-attach, retain explicit object identity, and have an established
   policy for object destruction/identity reuse and final world teardown.
   Automatically cleaning it during ordinary unregister would change source
   behavior. That lifetime policy is not recovered here.
5. Preflight ledger capacity and ownership, then publish ledger, reachable
   chain, and registry changes together. Exhaustion must remain a transactional
   reported boundary. Topology validation must distinguish intentionally
   recorded source-abandoned nodes from unexplained unreachable registrations.

Such a change would need source comparisons covering the truncated chain,
unchanged abandoned registry slots, freed F slot, later primary transfers, and
detach/re-attach behavior. It would make retained source abandonment explicit
and owned, but would not repair the original membership behavior or justify
claims about complete races. No ledger, new state, API change, or relaxed
admission was added in 50FT. The delivered playable checkpoint is unchanged.

Preimages of the unchanged production files were saved under the coordinating
50FT workspace before creating this document. Their SHA-256 values are:

- `src/game/live_zone_membership50en.cpp`:
  `9e84309efcb3679b8e7d788c61e3794af9b12a8b2e5c8d7b94b88384f1c83e70`
- `src/game/live_zone_membership50en.h`:
  `b2e5895aa6a495bd3dc41a27a6a2d982e575adee93a2fb018f4a953d7a08d048`
