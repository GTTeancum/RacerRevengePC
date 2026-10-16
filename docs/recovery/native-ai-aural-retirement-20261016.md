# Native AI audio registry retirement

The existing `native_original_ai_audio::Owner::close` is reached by the real
driver before mixer closure and by the owner's destructor. It now terminally
revokes its `OriginalAiAural` borrowed emitter registry before retiring the
owned logical channels. The private method is accessible only to that owner.
No independent success token or replacement emitter is introduced.

Shutdown first closes update/material/speech admission and removes the borrowed
sound-mode pointer. It saves the reached speech-preparation state for the
existing real voice stops. The registry clears every pointer and count and
retains a terminal blocked state. This also covers an unsuccessful preparation
that registered only some emitters. A close before preparation is terminal;
the existing fresh-owner policy requires a new owner for another session.
Repeated close performs no second speech stop, and existing logical channel
close methods already tolerate an absent mixer. PCM storage remains owned
until the containing owner is destroyed.

## Original cleanup boundary

This operation is native resource retirement, not source `103DD0` completion.
The retained source annotation proves that `103DD0` repeatedly takes index zero
from the actual Application audio-context list at `+5C` and calls `103D00`.
That function removes the list entry (`1046D0`), detaches its Entity (`141A70`),
then invokes virtual `+10` release. Source `103CA4` appends the secondary emitter
to that list, and `103CB4` attaches it with mask `FFFFFFFF`. The current numeric
emitter projection explicitly excludes those counted Entity operations.

Similarly, world cleanup's `148110` releases both original LightManager zone
lists `88` and `8C` in ascending ordinal order, clears their storage and count
`90`, then frees `60`, `80`, and `44`; it preserves cache `A4`. Current gameplay
lighting explicitly owns an authored native lighting policy and a separate
visibility namespace, rather than source Light+D0/LightManager+88 objects.
Its existing aggregate-borrow guards and storage destruction remain the native
lifetime mechanism. Neither that destruction nor `endNativeGameplay` is an
original `148110` receipt. These original callback joins remain unavailable.

Source-only review; no build or execution was performed for this change.
