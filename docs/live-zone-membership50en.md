# Live collision membership 50EN

Source written and manually inspected only. No build, test, runtime probe or
guest execution was run. The coordinating task owns integration and validation.

`live_zone_membership50en.h/.cpp` supplies real native E0 membership over the
existing caller-owned 50CL SearchLists. It computes 1202E0 bounds, follows
280D40 portal crossings through 50EO geometry, transfers the primary registry
slot, adds/removes secondary memberships, refreshes published collection views,
and removes the registration in 120290/281490 order. Writes use caller-owned
shadow arrays and publish only on complete success. Registry free slots retain
50CJ's original LIFO order. No global all-zone chain or collision fallback is made.

## Cold integration

- Parse each retained World48F portal's four original vertex fields with the
  existing binary32 authoring parser. Preserve authored portal and vertex order.
  Supply `LiveZonePortalInput50EN{zoneA,zoneB,vertices}` to
  `prepareLiveZoneWorld50EN`; allocate Z heads, 2P directed links and P geometries.
- Source272B3C..272C70 processes ascending authored portals. Each27F3B0 prepends
  its directional link, making each incident list reverse authored order.
- Extend lifecycle object/registration counts for distinct main and secondary
  query refs (for the retained 37 terrain owners, refs38 and39 are possible).
  Keep terrain refs stable. Each query needs its own links and entries with a
  caller-chosen capacity, normally at least 1+P. Entry/ref1 is primary.
- Share a workspace serially: 2Z shadow registries, membership-capacity shadow
  links and entries, 2P shadow portal links, and the embedded 50CJ transfer scratch.
- Attach collision queries with `ZoneMethod50CL::Second`, explicit initial zone,
  and initial overlap radius **0.0f**. Source120070 passes zero at120170; it does
  not pass the cached sphere radius. Later120230 uses that cached radius.
- Preserve the spring query's actual target/query distinction. Constructor
  295424 calls11B570 with mask2. That setter replaces flagsF8 bits0x0E, so cold
  initialization is `flagsF8 = (flagsF8 & ~0x0E) | 0x02` for both spring queries.
  This leaves querying enabled and clears target bit0x04 and temporary bit0x08.
  Merely OR-ing bit2 into the corpus's flags6 incorrectly makes a spring query
  an eligible dynamic target. The existing collector already excludes self by
  Ref and rejects targets without bit4; no fabricated owner-exclusion list or
  silent dropping of unknown candidate identities is needed.
  The query's collection object type is the actual collision RTTI tag
  (`query.collisionTypeTag`,7 in the retained corpus), distinct from its shape
  tag2; constructor11B2E4 writes object+2 and11C43C writes shape+88 separately.
- Initial collision zone1 has a source basis for this retained grid-start path:
  1AE578..1AE594 passes `world4C4[0]` into2169C0, whose216A2C stores C34. The
  first collision registrations21691C..21699C use C34 and method-mask2. This is
  not permission to assign zone1 after arbitrary resets or teleports.

## Current-frame prefix and collection order

Prepare the current type2 matrix/sphere with48O before each live update. Supply
its matrix and cached center/radius to `LiveZoneBounds50EN` and the exact current
type2 resource. Successful updates save the matrix/resource/tag, native bounds
and a monotonically increasing revision. A live query wrapper must verify the
receipt against the same object, current scene and resource; matching sphere
bounds alone does not prove the same transform.

The actual50H/50J null-pair spring route is295630:2959EC calls11FF60, which calls
11FE50/11F680 (scene/matrix/sphere preparation), then120230 at11FF74. Only then
295A08 calls the limited-candidate query122660. The outer vehicle collection at
219AF0/219B14 precedes the main spring call219BDC. Preserve that order: collect
from the prior accepted membership, then execute the current spring prefix and
narrowphase. Do not silently move collection after the new prefix or clear the
old zero-E0 admission guard; use a distinct live entrypoint with its receipt.
The separate1B1800 detector also calls11FF90/120230 before its own limited query.

Bind collection to the query's own `storage.links`, its declared capacity, and
head1 while attached. Publish cached bounds back to the corresponding query
state as required by the caller. Keep the global443148 head separate. Calls are
serial and occur with no open collector cursor. Other objects'50CL registrations
are unchanged. Do not update a50EN-managed object through50CL or50CQ concurrently.

## Explicit boundaries

Only existing bounded SearchList methods are admitted; growth, malformed world
links, exhausted caller storage and a cyclic crossing walk reject transactionally.
280D40's finite walk is bounded by `(2P+1)*(Z+1)` portal-link visits. No unsupported
state is reported as a collision miss.

281144 updates the current secondary **head**, even though earlier source words
searched for a matching zone;50EN retains that source behavior. Source281320 also
assigns the head from the removed node's next pointer. A reached non-head removal
would strand existing live registrations, so it is explicitly rejected as
`UnsupportedLinkRemoval`, without inventing an alternative linked-list policy.
An unsupported case stops before publication and requires further source-domain
work; it does not grant broader terrain coverage.
The source does not enforce one secondary per object:280BF0 inserts every
overlapping portal, and retained TA zone2 has three incident portals. No general
proof that the non-head case is unreachable is claimed.

50EO supplies ordinary finite binary32 portal geometry under its documented
48G/49F normalization model. No claim of every PS2 exceptional arithmetic mode,
full318-model collision availability, whole original scene callbacks, allocator
side effects or complete race/world registration is made.
