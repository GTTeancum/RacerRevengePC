# Pause audio command25: retained producer and missing receiver

Static-source investigation only. No guest/module/decoder/build/test execution.

## What the authenticated EE executable proves

* 1A7AA0..1A7AD0 calls235520 with1 for Character11C, Effects110,
  Engines114 and actual World4D8, in that order.
* 1A7AD4..1A7AF4 calls235520 with0 for Auxiliary124, Announcer120,
  and Music118, in that order. These are real source values; their semantic
  name is not established merely by the choice of groups.
* 235520 copies the caller boolean toA2, loads the actual allocated group40
  intoA1, obtains the real audio authority5C+40, and tailcalls26D6C0.
* 26D6E0..EC zeroes a12-byte payload.26D6F4 stores group at payload0;
  26D6F0..FC writes payload8=1 only when requested.26D7C4/26D7F8 emit
  command tag0x19 (decimal25) and the actual payload into the command buffer.
* Full-buffer delivery reaches266A10 with function0x20 or0x21 and buffer
  extent0x4508.266A9C/266B28 call2F8D30 to send that external request.
  The audio owner binds IDs0x12346 and0x23456 at2679B0/2679E0.
* Separately,2356A0 invokes142040, compares Boolean state44, writes the new
  state on a change, then invokes26D380.26D45C/26D490 emit tag0x17
  (decimal23);26D4BC stores the allocated group argument. This is a different
  operation from command25.

The six genuine race children all retain maskFFFFFFFF. No proven Entity
mask/flag exemption excludes Auxiliary124 from race142040. Actual race UI
feedback binds race124 (1A43A8..B8), and opening plays it after race142040.
These facts do not prove command25 means pause exemption, nor whether a backend
pause affects subsequently started voices. Neither behavior may be inferred
from the EE sender alone.

## Exact missing original inputs

The authenticated SLUS names PS2SOUND.IRX: literal file offset0x2B4150,
VA0x3B40D0 is "%sPS2SOUND.IRX%s"; file offset0x289D20 names the disc member
"\\PS2SOUND.IRX;1". It also names LIBSD.IRX, PANICSYS.IRX and IOPRP242.IMG.
The required next input is the original PS2SOUND.IRX receiver, with its matching
LIBSD/provider dependencies if that receiver delegates command23/25 elsewhere.
IOPRP242.IMG alone is not presumed to contain this separately named module.

Workspace filename inventory found no IRX, IOPRP image or disc ISO. Existing
`docs/original-ai-material-audio.md` separately records member-name inspection
of all seven retained recovery ZIPs, including48F and50HS, with no IRX/IOP
payload or disc image. No new archive payload was found in this investigation.
The retained SLUS is an EE command producer, not this missing IOP receiver.

## Current honest native boundary

Preserve actual source group state44 and original six-child traversal. Do not
manually exclude/unpause Auxiliary, relabel its voice, or assert an invented
command25 policy. The explicitly muted consumed ALL8 spectator-menu route
needs no audible feedback claim. Host-output activation remains gated until
actual receiver semantics (or another equally direct original source proof)
resolve native cursor/group behavior. The independent native host-time pump
only services the current mixer policy; it does not close this source gap.
