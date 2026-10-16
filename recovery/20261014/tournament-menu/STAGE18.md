# Stage18:102570 is a storage-summary fold, not demand forwarding

Proof-only incremental patch after stage17(62143bd); root integrated16+17 as a2c8788. No production source changed. Requested demand forwarding is not implemented because the exact original routine contradicts that interpretation. Stage17 terminal behavior remains intact.

## Evidence identity and method

Read original SLUS_202.68 solely as data from the authorized disc directory. SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30 matches established source. Existing ELF mapping: file offset=virtual address-100000+80. aggregate-102570-words.txt retains the exact bounded little-endian words and per-slice SHA256 values. Instruction meaning below was interpreted manually from those words. No instruction decoder, conversion, compiler, build, test, syntax check, project executable, game launch, emulation or guest execution ran.

## Exact102570 control/data flow

-102588: copy this(a0) to s3;10258C: copy output pointer(a1) to s2.102590 and102598 initialize ordinal s0 and byte offset s1 to zero.
-102594 branches first to1025F0, which reads LIVE aggregate20 count.1025F4 compares ordinal<count as signed;1025F8 branches to10259C if true.
-10259C reads actual aggregate24 pointer array;1025A0 adds offset;1025A4 loads actual child pointer.1025A8 is branch-likely on null, with offset+=4 in its taken delay slot1025AC, then continues at ordinal increment1025EC. Null children produce no output writes.
-For nonnull child,1025B0 reads vtable8,1025B4 reads virtual1C,1025B8 invokes it, and delay slot1025BC supplies a1=sp+50, a three-word scratch result. No demand float is supplied or forwarded.
-1025C0..C4 copies scratch50 into output0 (OVERWRITE, not addition).1025C8..D4 adds scratch54 to existing output4 using32-bit addu.1025D8..E4 adds scratch58 to existing output8 similarly. Existing output accumulators are not cleared by this routine. Word0 is the last visited nonnull child's value; if no nonnull child is visited, all output words remain untouched.
-1025E8 increments array byte offset by4;1025EC increments ordinal by1; the live count is read again at1025F0. Traversal is ascending actual pointers.102600..618 restores registers and returns. There is no load/store of aggregate or child+10, no104A00 call, and no texture-list/residency mutation in this routine.

## Child target proves the result domain

ATM vtable3CC750 has slot18=102050 and slot1C=102570. Actual ordinary frame constructor24E1B0 installs vtable3D21C0 at24E1C4..D0. That vtable's slot1C is251980.

251980 clears output4/output8, then reads child flagsF8 bit0. If set, it sums +1C of each present level record into output4. Otherwise it sums (level.width104 * level.height110 * bitsPerPixelE8)>>3 across five slots into output8. Finally251A14 reads child.formatAC and251A1C writes output0 in the return delay slot. This is format/storage accounting; neither branch reads demand10. Exact semantics of the alternate per-level+1C metric need not be guessed to prove it is not float-demand forwarding.

## Actual Tournament marker demand and frame selection

1F2624 calls2DF8A0. That routine resolves the ScreenTexture primitive material through SAME382B38, reads pass4C/map1A, then resolves SAME383400 and returns that actual resource.1F262C reads returned resource+10;1F2630 loads bits41200000 (10.0f);1F263C compares10<=current;1F264C stores those literal bits to returned resource+10 only when comparison fails. For an ATM row this is the aggregate's common demand field. No102570 call or child iteration occurs in this source sequence.

102050 itself only loads aggregate24, indexes by ordinal*4, and returns the actual child pointer. It does not copy aggregate demand into the selected child.24B254 resolves the actual table resource,24B26C obtains the animation ordinal,24B280 invokes virtual18, then24B294 passes the selected child to2559D0;24B2A0 invokes256340;24B2B0 invokes249610. The inspected binding slice likewise does not copy aggregate10 to a child.

## Precise proof gap / next source requirement

The requested 'aggregate102570 demand forwarding' is not a missing implementation of that routine; its premise is incorrect. The existing stage16 unsupported diagnostic at102570 must not be read as recovered demand-call provenance. It was a conservative boundary label, not proof. Runtime code is left unchanged in this proof-only handoff.

Before implementing actual child demand behavior, recover its real writer/caller in the selected-child manager path2559D0 ->256340 and the ordinary texture-list update/reset path (including24E670 /14D180), or identify another proven dataflow from aggregate10 to actual children. Establish whether any propagation exists, whether it targets selected or all frames, the source value/transform, actual iteration/timing and partial-effect/retirement semantics. Absence of a transfer in these bounded slices is NOT a claim that no other original path can transfer demand.

Do not substitute ordinary aggregate104A00, distribute the floor to all decoded images, copy it to the selected image based on visual intuition, or register the aggregate in the ordinary list. Source does prove the finite aggregate direct-floor hook at1F264C, but joining that hook alone would not prove the requested child forwarding or complete manager lifecycle, so no isolated demand setter was added here.

## Deliverable

Changed files only:
- recovery/20261014/tournament-menu/STAGE18.md
- recovery/20261014/tournament-menu/aggregate-102570-words.txt

No production source, scheduler, frontend, profile/results, generated assets, callbacks, or demand behavior changed. No validation claim. Root retains all integration and testing ownership.
