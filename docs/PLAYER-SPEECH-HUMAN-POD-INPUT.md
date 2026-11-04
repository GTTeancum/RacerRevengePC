# Retained human POD01 encoded input

Status: staged source-only input custody, November 4, 2026. No compiler, syntax check, build, test, probe, emulation, or runtime action was performed. Original manager/Player/World issuance and live speech open remain pending; the speech PC stays `2C11C8`.

## Corrected input-custody finding

The earlier POD-enrollment investigation was too broad when it described original encoded input as absent from the native roster. The temporary animation `Archive` dropped its own input vector after decoding, but the actual retained `ra_cold_roster50LA::Resources` already kept complete, hash-authenticated original files in `rawRes_`. Existing wreck inventory consumes those arrays. They are populated during `Resources::adopt` and are not subsequently modified by the current source.

This packet uses that existing input rather than reopening the original file or promoting decoded native members into compressed-file input.

## Implemented

- During the same roster adoption, only when there is exactly one native human, slot zero is that human and its authenticated racer+60 identity is POD01, a private `OriginalUiArchive50KG(sourcePod01)` owner is constructed from the same already-read byte vector before it moves into `rawRes_[0]`.
- Existing archive loading authenticates the full original 349,110-byte file digest, resource framing, all 96 member names/offsets/sizes/hashes, and block table before publishing the owner. The sourcePod01 catalog agrees with the existing POD01 identity pin. The owner is immutable after publication and retained privately by the same Resources object.
- The existing const-input archive API creates its own encoded copy and decoded/member/block storage. This is explicit native input preparation, not a source allocation, registration, file handle, or source return. The original `rawRes_` allocation remains intact for existing wreck consumers. Native allocation/authentication failure rejects the unpublished roster adoption; it is not a guest `12CEA0` or Config failure result.
- Native roster mounting now uses each slot's same retained `rawRes_` through a private byte-input `Archive` constructor available only to the actual aggregate owner. It retains the existing digest comparison against that Rig and the same decode/mount authentication. The old public Rig-based constructor still performs its existing read for its other callers; this packet removes only the actual roster mount's redundant reopen.
- The current D48 input path retains an aliasing shared Rig pointer whose control block is that Resources object. Thus the same existing owner lifetime also retains its private human-POD01 input; no source-bank or source-World authority is inferred from that keepalive.

For other human POD identities, zero-human rosters, or nonhuman POD01 slots, the new source archive field stays absent. No remapping to POD01 occurs, no catalog identities are invented, and the preexisting native roster remains supported. This packet does not claim original source-POD support beyond the already admitted POD01 catalog.

## Original occurrence is still separate

`OriginalPodSourceBank50LA` can use an authenticated sourcePod01 owner only after the genuine same-manager occurrence is issued. Merely having the correct encoded bytes or a matching mounted directory cannot authorize `12CEA0`, `132A20`, source World-row borrowing, Player construction, or speech Config open.

The actual source World tail already records original `1A9EE0` return value one and stops at outer Race `1A6D88`. The next original path reads Race+30, may call `114770` for World4C0[0], and then calls `1A95B0`. The current native Session adapter explicitly lacks complete Race30 writer-history and cannot certify that branch or those callees. Original manager loading at `1AA580` is substantially later, after additional VCR/map/resource/controller/World/music work. Its `1AD624 -> 1AE280` edge, actual source Player, and World-borrow issuer remain separate missing joins.

Accordingly, this packet changes no original source PC, manager occurrence, bank enrollment, Config state, count/cursor/RNG state, Event/Sound, source child, attachment, or audio readiness. The separate mapping file records exact source and ELF evidence for the next legitimate outer-Race boundary.
