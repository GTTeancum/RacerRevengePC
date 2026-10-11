# Detached cold logical audio domain

`ColdLogicalAudioDomain50KY` is an owned, source-derived cold component whose
completed endpoint is **PendingRegistration**. It is not AuralScape, the source
sound interface, an event-attached Effects110 group, a sample provider or a
playback owner. Source candidate only: this change and its prospective check
have not been compiled, syntax-checked, tested or run.

## Real ownership and admission

- Prepare retains a shared, const, actually prepared
  `OriginalAudioDirectory50KX`. It creates all 267 mutable rows in authenticated
  directory order, reserves namespace slots 0/1 and admits dynamic IDs 2..24.
  It never synthesizes the directory from PCM requests. Failed preparation
  publishes nothing and stays retryable. Repeated preparation and reopening a
  closed domain are refused; neither can reset occupancy under live groups.
- A dynamic group is a stable allocated object with its own real eight-slot
  sound-pointer array. The first free ID is claimed only after both allocations
  succeed. Subsequent array growth adds 16 slots. No caller-selected group ID
  or Effects110 label can authorize construction.
- Every `createGcSound` consumes an actually prepared immutable
  `GcMaterialResourceBindings50KX` owner, material kind and source key. It
  revalidates the binding's actual full directory name against its source
  ordinal in this domain. The sound retains its binding owner, so original
  `.wav` factory spelling, full backslash registry `.vag` key and normalized
  PCM request remain three separate authentic fields.
- Collision/drive kinds select the proven source flags 1/0x81 internally.
  Streaming, stereo, caller-supplied flags and arbitrary binding records have
  no admission API. Invalid enum values and out-of-range keys explicitly fail.
- Each successful call creates and inserts a distinct stable logical sound.
  Array growth moves only owning pointers. The group owns the sound's initial
  64-bit reference; registration does not retain it. Eight 24-sound material
  families therefore produce 192 objects and 21 distinct resource demands,
  rather than 21 shared mutable sounds. This prospective case uses a single
  shared group and does not claim eight D44 records or completed D44s.
- Use genuine shared ownership of these immutable owners, normally
  `make_shared`, not a no-op deleter around a short-lived borrow or an unrelated
  aliasing control block. `shared_ptr` cannot runtime-certify this ownership
  precondition. All operations and borrows are single-threaded/non-reentrant;
  immutable-owner destruction and custom deleters must be side-effect-free
  with respect to the domain, never call back into it.
  This component adds no application/global service singleton: integration
  must supply one shared namespace at the appropriate application boundary,
  not instantiate a new namespace for every D44.

## Typed identities and immutable inspection

Resource ordinal and dynamic group number have separate types. Group and sound
handles are private-construction value tokens. A weak domain lifetime token
and nonwrapping slot generation reject foreign/stale groups and reused IDs;
the token prevents aliases even when a new domain occupies the exact address
of a destroyed one. A sound handle additionally identifies its insertion
ordinal within that group generation. There is no individual sound-removal
API, so ordinals cannot be reused during that group's lifetime.

Inspection returns const references to actual objects and const rows. Borrowed
group/sound pointers expire on group destruction or domain close. Copying a
row representative does not extend its lifetime or preserve a safe borrow
after clear and group teardown. Binding/string views require the retained
binding owner to stay alive, guaranteed here for at least sound lifetime.
Directory and row pointers expire on close; row contents change on register
and clear. Revalidate value handles through the domain
instead of dereferencing an expired borrow. No public method accepts a claimed
sound pointer, occupancy snapshot, sample integer, provider callback, source
pitch result or event attachment receipt.

## Exact cold projection, deliberately incomplete

The group stores the detached common-base values: 64-bit reference count 1,
null-only previous10/next14/firstChild18/parent1C links, zero masks, byte28=FF,
known flags29 low nibble 3 and
known flag30 bit0=1. Unknown upper bits are represented with explicit known-bit
masks, not silently initialized as proven zero. After actual ID claim, its
64-bit source volume projection is 100 and state44 is 0. The domain's
GameObject contribution is the count of its live groups only. It does not
read, set or claim the original application-wide counter423F60.

Each sound projects the exact proven cold constructor fields, including
64-bit reference1, sample/channel/stream-index -1, zero duration/elapsed,
volume100, pan0, pitches4096, the proven float constants/vectors and the
source resource ordinal. Unknown +0C/+1C/+84..8C/+1BC storage is absent. The
source factory name is supplied by the authentic retained binding; this does
not claim that the complete 260-byte source name buffer was zero-filled.
Constructor pitches4096 are not source-positive sample metadata.

Native domain/group relations are real ownership. They are intentionally not
labelled source interface30/interface5C or a complete GameObject. The missing
application identity, source global guard423F58, tree links/status operations,
event attachment and canonical Effects110 gain authority remain separate
joins. No private zero guard is used to stand in for application state.

## Registration and unavailable resolution

Mutable row alternatives are `unreferenced`, `pendingRegistration` and
`cleared`. An empty or cleared row has no representative and count0. A pending
row borrows the actual last registered sound and carries a positive signed32-
compatible registration count. New registrations increment the count and
replace the representative. Earlier sounds remain independently owned and
keep sample28=-1. Counts are registrations since clear, not live-sound counts.

All rows can be inspected in directory order through their typed source
index. The pending representative exposes the actual group, full resource
name, flags and cold fields. `resolvePending` returns
`sampleOwnerUnavailable` without changing any tag, count, representative or
sound field. It performs no provider attempt and publishes no synthetic
negative result. There is deliberately no `resolved` alternative until an
actual provider-owned sample/result type and positive metadata provenance
exist. A cleared zero never becomes sample0.

## Explicit reset and teardown

`clearRegistrations` implements the source26D960 row-clear boundary only:
all rows become `cleared`, all representative borrows disappear, and all
counts become zero. Groups, sounds, IDs, names, fields and their stable storage
remain intact. New registration after a clear starts a fresh pending count;
it does not restore old sounds as representatives. This is not backend
reset268300, sample eviction or playback shutdown.

`destroyDetachedGroup` first scans the real pending rows. If a representative
belongs to the target, it returns `pendingRepresentativeBorrow` without
mutation; the caller must explicitly clear registrations before retrying.
Deleting a group containing only earlier nonrepresentatives is allowed and
leaves historical counts and the later representative untouched. Teardown
returns the claimed ID first, visits sounds in insertion order, consumes
their initial owned references, frees the pointer array, then destroys the
group and its owned GameObject contribution. All admitted channels are -1,
which is the source233670 stop early-return domain. No transport work occurs.

The original2353D0 snapshots its sound list to survive reentrant list changes.
This component exposes no source stop/release callbacks or membership-changing
sound destructor, so its own insertion-order list is safe without a teardown
allocation. It does not call or claim complete source virtual release142590.
That source operation consults a live application guard and tree state that
are outside this component's ownership.

`close` first prevents new admissions, clears the rows before destroying any
sound, destroys all owned groups, then releases the rows, token and directory.
It is idempotent and is also invoked by the destructor. Close's clear-before-
destruction rule and per-group representative guard are explicit host safety
admissions, not reconstructed source RemoveSound registry callbacks.

## Failure contract

Every admission failure leaves output handles and published state unchanged.
Group object and initial-array allocation happen before ID claim. Sound and
expanded-array allocation both happen before the current array is moved or a
sound is inserted. After insertion, registration consists only of nonthrowing
owned-pointer/scalar assignments. Diagnostics distinguish unprepared owners,
wrong/stale/invalid handles, bad keys/kinds, missing resources, exhausted IDs,
allocation/storage failure, pending borrows and unavailable sample ownership.

The count guard refuses at `INT32_MAX` before increment because source26D9F0
selects signed count>0. Slot generation exhaustion also refuses before reuse,
never wraps into an old identity. Storage arithmetic is checked before growth.
These are bounded host admissions, not invented original clean-failure paths.
The actual source ignored several allocation/append failures and asserted on
missing directory rows; this owner cannot safely reproduce those failures.

## Prospective regression and integration

`tests/native/cold_logical_audio_domain50ky_check.cpp` requires the retained
original APPSTART.RES and extracted GC AUB. It uses the actual authenticated
archive/directory/binding owners and no synthetic registry. Its source covers:

- all267 source-ordered tagged rows and authentic naming/ordinal relations
- four distinct same-resource collision sounds and last representative/count
- 24 and192 distinct sounds,21 pending resources, source8+16 array capacity
- initial detached/cold fields and deliberately partial known flags
- no mutation on unavailable resolution, repeated prepare or guarded teardown
- clear-only reset preserving every sound/group and pointer
- historical counts after deletion of earlier nonrepresentatives
- 23 dynamic groups, capacity exhaustion, first-hole reuse and stale handles
- wrong-domain handles, group expiry and exact-address domain reincarnation
- actual directory/binding owner retention until their dependent storage ends
- injected allocation failure at preparation, group/initial array, sound and
  sound-array growth, including preserved capacity/count/representative/output

The isolated prospective executable replaces ordinary allocation operators
only to inject failures around one operation at a time. It exposes no
production callback, test-only fake owner, counter mutation or privileged
handle constructor. INT32_MAX and uint64 generation exhaustion guards are
source-inspected; exercising them through authentic public admissions would
require impractical object counts and is not claimed by the prospective test.

The production source is `src/game/cold_logical_audio_domain50ky.cpp`. The
prospective check also needs the existing50KX focused-check dependencies:
`original_audio_directory50kx.cpp`, `original_ui_archive50kg.cpp`,
`material_audio50hi.cpp`, `import_world48f.cpp` and `import_decode48c.cpp` under
`src/assets`, plus zlib and the native C++20/strict-FP flags. Source-list
registration alone must not start builds/tests or enable a runtime provider.
Only the parent may integrate source or authorize the centralized test slot.

## Source evidence

Reconstruction used the reviewed cold-domain ownership design and the prior
material factory/registry report against native main5d2f51f. Original ELF
SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
No guest instructions were executed. The implementation follows:

- 232AA0/232AF4..232B44: 25 occupied/free slots, reserved0/1
- 2350D0/2351F0: actual group array, claim-first-free ID and64-bit volume
- 1048F0,141300,102620: initial64-bit reference and partial cold group fields
- 1027B0/232ED0: sound constructor projection, actual group insertion
- 2787B0/292AF0/292CB0/232DB0: authentic names and factory flags
- 233110/298A30/298F80: actual resource ordinal, pending increment/publication
- 26D9F0/2332F0: directory-order resolution boundary, still unimplemented
- 26D960: explicit clear-only row operation
- 235150/2353D0/233670: ID return, ordered cold cleanup and channel-1 stop
- 142590: canonical guarded/tree-aware release, expressly not implemented

Remaining prerequisites are canonical application/event lifetime and gain
joining; actual sample/backing ownership with source-positive duration/pitch
provenance; real mixer leases and service/listener scheduling. PCM readiness
alone cannot complete these joins. No live behavior is changed by this leaf.
