# Native Player speech input custody

Status: staged source-only continuation, November 4, 2026. No compiler, syntax check, build, test, or runtime execution has been performed. This is not speech playback or complete Player loading.

The existing native base-Racer path constructs its actual counted D48 and publishes that same owner before reaching the unresolved source `2C1170` load. This change adds native input custody at that already-reached boundary. The source PC stays at `2C1170` and the continuation still returns pending.

## Implemented

- The real ownership bridge supplies the current Application material provider and an aliasing shared owner of the same selected slot-0 Rig, retained through its existing roster Resources control block. A racer number or independent extracted file is not an issuance path.
- An immutable owner captures the selected `sounds.ini`, `curses.ini`, `taunts.ini`, and `special.ini` directly from the sealed mounted provider. It checks the existing authenticated archive identity, actual Rig archive path, original member order and size, exact selected Rig bytes, copied-member SHA-256, and unchanged lookup winner, generation, and provider revision.
- The inventory retains original field text and byte offsets, declared event counts, event IDs, and absent sections/flags. It covers the 23 identities already in the source identity catalog. POD26 retains the original declaration of 14 curses events and absent Event2; the 13 count-selected entries are not compacted.
- The owner retains the same Application Aural's immutable stream metadata. Each present event references that owner's exact metadata row.
- The private live-audio join matches this mounted authoring to the existing selected catalog, authored slots, Character-group voice mapping, exact existing bank records and decoded PCM pointers/shapes. It creates no second sample bank, PCM allocation, voice range, or mutable speech state. Missing/unsupported/invalid native samples stay explicit and do not become playback success.
- Capture is one-shot. Failure retains the already-reached D48 prefix without resetting constructor state or retrying a new source path. Normal D48 retirement revokes all new event/PCM aliases and releases immutable custody before actual source-owner destruction. Historical input custody does not require a still-mounted provider to be safely withdrawn.

## Deliberate limits

The field inventory is a bounded reader of hash-pinned original text, not the original Config implementation. No original `1577D0`, path-global, file-handle, Config read, source count store, RNG draw, cursor store, B4 snapshot, `2921C0` child, Event wrapper, `232DB0` Sound, Character source membership, stream-channel claim, `2925A0` return, or outer `22A25C` attachment is implemented by this step. The native supplied-AUDIO sample identity is retained, not upgraded to independently authenticated original VAG identity.

The current dispatch remains the authentic base-Racer dispatch. Player D40 construction and the later unconditional D40 dereference remain separate unresolved work. The Application's existing outer pending fence is unchanged.

## Next source work

Use the retained exact inputs while recovering real Config/file-manager occurrences and source-ordered count/RNG effects. Then join the three actual response-emitter children and their Event/Sound ownership to the same native provider and shared Aural resources. Keep missing-section behavior, partial construction, group service/retirement and source-versus-native distinctions explicit.
