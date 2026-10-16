# Native voice membership on counted race audio groups

The actual `RaceAudioGroups50IC` owns both its seven canonical counted group
prefixes and its native per-slot binding ledger. Native providers may now bind
only to the same constructed, loaded group published in the actual Application
sound-group table. The original source logical-member list remains genuinely
empty; native mixer voices are not represented as invented logical Entity nodes.

Existing gain, pause state44, ducking, and AI group routing remain unchanged.
Binding preflights control-revision capacity before gain/pause publication;
range binding preflights the whole requested range. Previously acquired bindings
remain real membership if later provider preparation fails.

`retireNativeVoiceBindings()` is an explicit native shutdown operation. The
caller must first stop/close its actual providers and serialize access to the
mixer, as required by the existing main-thread bridge. Every actual voice must
be stopped. Same mixer epoch, actual group publication, and correspondence
between the ledger and mixer gain bindings are checked before any mutation.
Live voices, foreign/missing bindings, stale epochs, wrong threads, attachment
locks, and exhausted control revision capacity reject unchanged. Retirement
removes only those stopped memberships and terminally closes this group owner's
binding admission. It does not stop a voice, advance a cursor, or alter the
original group levels/state44. Mixer group admission remains required, so a stale
provider cannot restart a retired slot. Repeated retirement verifies the stopped,
empty state and succeeds; partial preparation with any subset bound uses the same
operation. A never-initialized constructor prefix instead retains its existing
empty-list construction rollback.

The private `canReleaseEmptyCold142590()` is the authoritative nonmutating
preflight used by both native world cleanup and actual empty group release.
It verifies canonical authority registration, detached reference-one ownership,
process slot identity, and stopped/empty native membership. The release calls
this exact preflight before clearing its real source process slot/list/count.
These operations do not implement source2353D0 callbacks for populated logical
sound lists and do not establish original world/race destructor completion.
No driver activation is included.
