# Known rigid-body contact list and lifecycle reset (48R)

## Proven source and boundary

The authorized disc ELF `work/native/project/disc/SLUS_202.68` is pinned to
SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
`tools/gameplay/body_contact_list_fixtures48r.py` verifies its ELF program headers
and extracts each unique file-backed PT_LOAD extent. The self-contained freeze
is `evidence/bodyContactList48R/original-words.json` plus matching raw blocks.

- PhysicsRigidBody vtable `3CE000+98` contains `00165430`. Its callback is exactly
  `165430..1654AC`, including the return delay slot; code SHA-256 is
  `efd33a85235838ae49e0427a268e2e9815e8c28014d27461736c88ea70db0e20`.
- The separate lifecycle vtable slot `3CE000+94` contains `00165420`. Its entire
  helper is `165420: JR ra`, `165424: SW zero,20C(a0)` in the return delay slot.
  Code SHA-256 is
  `975c7d0eda40f6fd62e3bb9b7c4c277da586de66444ac7d74e5b33f3270b5d84`.

The first helper is the known virtual target used by the recovered vehicle
contact handler. This API does not accept arbitrary host callbacks, resolve
arbitrary guest function pointers, establish body ownership, or implement the
vehicle scheduler. The separate reset must be scheduled explicitly: the mapped
original caller invokes slot94 at `219E94`, after body integration at `219E80`.
The handler and append helper never perform an automatic reset.

## Exact callback semantics

1. `165430..165438` loads signed count at body+20C and returns immediately for
   count >=10. The original does not read a single contact slot on this path.
   Positive overfull values, including INT32_MAX, are unchanged no-ops.
2. Otherwise, original `165450..165488` searches slots at body+210 in order,
   comparing complete 32-bit other-body identity values. Only the active prefix
   `[0,count)` participates. The first duplicate suppresses insertion.
3. When absent, `165490..1654A4` stores other at slot[count], reloads count, and
   increments it. Existing slots, inactive tail slots and all unrelated state
   remain unchanged. There is no null, self-identity or pointer-validity check.
4. No return-value meaning is assumed. Native Appended, Duplicate and Full are
   diagnostics of these observed paths.

The callback's signed `<10` check alone does not make negative values safe.
Negative counts skip the search and reach unchecked 32-bit shift/address
arithmetic. For example, count=-1 writes other into count20C itself, then reloads
and increments that overwritten value. INT32_MIN wraps the slot offset to zero.
Other negatives write before the list or outside the bounded address space.
`negative-count-probes.json` records original instruction execution illustrating
these unsafe states; those observations are not native equivalence fixtures.

The native API rejects every negative count with InvalidInput and no mutation.
It does not reproduce negative indexing, normalize overfull positive counts, or
invent an empty-list interpretation of invalid state. Separately, the original
reset is safe for every count bit pattern: it writes zero without reading count
or slots, and leaves all ten slots untouched.

## Native contract

`src/game/body_contact_list48r.h` exposes:

- `BodyContactIdentity48R`: an explicit opaque 32-bit identity value
- `BodyContactListState48R`: signed count20C and ten inline slots210, 44 bytes
- `applyBodyContactList48R(other, state)`: the known 165430 operation
- `resetBodyContactList48R(state)`: the separate known 165420 operation

A valid, aligned C++ state object is required. Inline fixed capacity excludes
caller-supplied pointer/length mismatches and undersized declared storage. No
runtime guest addresses are used. Zero, self and arbitrary identity bits are
accepted by this helper; higher-level callers are responsible for their own
explicit body binding before dispatch.

Identity is passed by value and captured before mutation. A state slot may be
the identity source, including the exact insertion destination. No distinct
source/output buffers or overlap rejection is needed. Duplicate, Full and
InvalidInput preserve every state byte. On Appended, precisely one slot and
count change. Reset changes only count. All arithmetic converting an index to
size_t follows the signed negative/full guards; increment is bounded to 1..10.

The production helper has constant bounded storage, at most nine comparisons
before an append, no allocation, no locks, no threads, no retained pointers and
no scratch-buffer ownership. Original-Xbox performance is not measured by these
host tests.

## Independent original-instruction evidence

Run from the repository root:

    python3 -B tools/gameplay/body_contact_list_fixtures48r.py
    python3 -B tools/gameplay/body_contact_list_fixtures48r.py --frozen

The first command rechecks the authorized ELF. The second uses hash-pinned frozen
blocks only. Both produce byte-identical fixtures. The existing original-word
executor runs the original instructions end to end with `data=b''`; every data
read is explicitly initialized or previously written. All cases have
`source_reads=0`, no RAM input, no helper stubs, no instruction patches, and no
native expected values.

Callback evidence contains 3,260 cases and 160,697 original instructions:

- All ordered count 0..5 prefixes over zero, self, UINT32_MAX, with the three alphabet
  members and one outside-alphabet test identity for every state
- Every duplicate position for count 0..10 and six identity corners
- Every inactive-tail match position for counts 0..9
- Full and overfull positive counts through INT32_MAX
- Random 32-bit identities and lists
- 1,595 Appended, 1,452 Duplicate and 213 Full outcomes

All 31 reachable words execute. Both outcomes of every conditional branch at
165438,165454,165474,16547C and165488 are covered. Word165464 is statically
unreachable behind an unconditional branch and its delay slot. Full-path traces
verify that count is the only data read. Original writes are checked to remain
in count20C or the ten bounded slots for all equivalence fixtures.

Reset evidence contains 1,024 cases and 2,048 original instructions, including
zero, negative, full and large signed counts. Both words execute; reset has no
original data reads and exactly one write, zero to count20C.

Binary schemas, in little-endian 32-bit words:

- `body-contact-list-fixtures.bin`: header(3260,44); each row is other identity,
  initial 21-word body+200..250 span, original output 21-word span, path diagnostic
- `body-contact-reset-fixtures.bin`: header(1024,42); each row is initial and
  original output 21-word spans

The spans preserve three preceding words and seven trailing words in addition
to count and all ten slots. Fixture generation records complete identity and
case-group provenance in the accompanying JSON reports.

## Native checks

`tests/native/body_contact_list48r.cpp` requires both frozen binaries. Missing,
malformed, truncated or trailing fixture bytes block the test. Every original
output word is compared, including all slot tails and neighboring canaries.
Additional cases check transactional negative rejection, all 110 source-slot
alias combinations, full-before-dedup diagnostic ordering, large signed counts,
and 86,016 repeated calls with allocation interception enabled.

Focused C++17 release and AddressSanitizer/UndefinedBehaviorSanitizer runs each
passed 96,803 checks with zero failures and zero runtime allocations. Both builds
used `-Wall -Wextra -Wpedantic -Werror`; release used `-O2`, and sanitizers used
`-O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all`. LeakSanitizer was
explicitly disabled; allocation interception and ASan/UBSan remained enabled.
Exact inputs and results are in `native-validation.json`.

Reproduce the focused build without downloads:

    c++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2 -Isrc/game \
      src/game/body_contact_list48r.cpp tests/native/body_contact_list48r.cpp \
      -o /tmp/body-contact-list48r
    /tmp/body-contact-list48r .

For serial disposable mutation checks:

    python3 -B evidence/bodyContactList48R/sensitivity48r.py \
      --output-dir /tmp/body-contact-list48r-sensitivity-new

All six mutants are detected: capacity 9, disabled dedup, zero rejection,
misdiagnosed full, reset tail clearing, and missing reset. The script never
changes production sources. This validates the bounded callback and explicit
lifecycle operation; it does not establish live world registration, full vehicle
integration, stable races, gameplay FPS, or console readiness.
