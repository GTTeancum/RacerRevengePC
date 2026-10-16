# Bounded opaque process word3E1C10

## Proven producer and writes

The authenticated executable entry initializes this storage before Application:

-100008 LUI v0,003D;100010 ADDIU v0,5A00 =>3D5A00.
-10000C LUI v1,0045;100014 ADDIU v1,F600 =>44F600.
-100018 SQ zero,0(v0).
-100020 SLTU at,v0,v1;10002C BNE at,zero,100018.
-100030 increments v0 by16 in the branch delay slot.

The loop includes the store at44F600, so the zeroed byte range is
[3D5A00,44F610). Word3E1C10 is inside it and starts zero.
Race destructor1A6880 LUI at,003E plus1A6884 SW zero,1C10(at)
clears this exact word after Video44 and renderer2C. Alternate destructor
2C7160/68 performs the same store in the delay slot of its reset call.

No nonzero value, pointer interpretation, active-state meaning, or semantic
reader is established. Direct immediate references only identified these two
clears. Exact aligned literal3E1C10 was not found in the source file. ELF
reginfo and startup100034/48/5C independently identify GP=3DD970; the target's
GP offset is42A0, with no direct GP access found in the retained text. These
bounded searches do not prove that indirect/base-relative accesses never exist.

## Native ownership

The existing actual Application already retains process BSS-derived material
state across frontend/race sessions. It now privately retains one opaque
uint32_t sourceProcessWord3E1C10_, initialized once to zero, and the private
exact reached-store leaf clearRaceProcessWord1A6884. There is no public setter,
readiness state, inferred active value or per-session replacement owner.
This retains an actual source storage effect rather than treating the clear as
an empty callback. No runtime call is added. The private word is not consumed
as a gameplay decision and cannot certify unknown source operations.

Future actual race cleanup must reach this store after Video1A6870 and
renderer1A687C, before the source's conditional resource releases. Additional
source accesses, if recovered, must bind to this same process-owned word and
preserve their real value semantics. This facet does not complete manager13C,
full race destruction, constructor failure handling, or Pause activation.

Static source/data inspection only. No builds/tests/syntax checks, project
asset decoders, gameplay or guest code were executed.
