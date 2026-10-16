# Texture-manager 13C: actual transfer allocation mapping

Source-only audit against a40ec73. No execution, build, test, syntax check or
project decoder. This records a concrete missing ownership join, not a complete
2543F0 implementation or a claim that the source allocation is absent.

## Original allocation and release

253DF0 initializes the actual Application texture-manager13C to null.
2542F0 computes its allocation sizing;2543A4 first invokes2543F0 on the existing
manager, then2543AC allocates0x1824 bytes,2543C0 constructs252C70 and2543CC
publishes that actual result at13C. Therefore a reset implementation cannot
assume constructor-null forever.

252C70 owns:

- 256 inline0x18 records initialized through252E70.
- +1800: a0x20000-byte allocation (252CA4..B8).
- +1808: record count, at least12 (252CB0..CC).
- +1804: an owned array of0x24-byte records, constructed through2520D0 with
 252100 destruction (252CD4..252D0C).
- Each +1804 record receives a separate0x20000-byte allocation at its first
 word (252D68..90), initialized through252150.
- +1814: a separate computed-size buffer, also published as global442B60
 (252D10..58).

The cache/block consumers include2530A0 (logical-record association),2531E0
(record processing),2533E0 (allocation selection),253850 (release/reset of
logical record state and block allocation) and253BC0 (per-block size classes).
This is a distinct CPU transfer/cache allocation family, not the registration
list30 and not the canonical decoded asset objects themselves.

2543F0 first calls the actual renderer2450A0. It then invokes252EA0(13C,1)
when nonnull and always clears13C.252EA0 releases each +1804 record's first
allocation, destroys the array, releases +1814, zeros442B60, releases +1800,
and finally deletes the owner when the deleting argument is positive. It does
not call canonical texture-resource destructors or delete persistent native GL
image publications.

## Existing concrete native owners

MaterialTextureGl50EB::Storage retains bounded texture metadata, GL names and
context/thread identity. Its source explicitly retains no RGBA copy. upload()
binds PIXEL_UNPACK_BUFFER to0, sets tightly packed client-memory state and
passes the actual immutable decoded pixels to texImage2D. This is a real direct
client upload, not a surviving emulated source transfer-pool allocation.

Its close() destroys persistent GL image names and invalidates publication.
Typed RA world/FireP0t owners reject that operation while their renderer leases
are alive. Consequently substituting close() for252EA0 would add unrelated
image retirement, break live borrows, and contradict source reset-before-child
release order. The driver separately owns primary/opponent/cable/specular/sky
banks and several dynamic texture owners; HUD image owners also live inside
the actual renderer. No one current Application texture-manager facet owns the
combined upload/cache lifetime of these paths.

OriginalStandardTextureLifetime50LA represents the canonical registration list
only. Its current ownership cannot prove that every native uploader is idle,
that no temporary transfer state survives, or that all cache aliases are retired.
Adding a null13C field without binding these actual paths is insufficient.

## Minimal implementation boundary

Use the SAME Application texture lifetime and actual RaceDisplayLifetime. Bind
actual native upload/cache lifetimes, rather than copying pixels or emulating
PS2 blocks. For each admitted uploader, establish that no client transfer borrow
outlives its operation; account for any persistent native transfer/cache backing
and aliases separately from persistent image names. Then execute the actual
single renderer reset/drain before retiring only that transfer/cache ownership.
Retain failure state and prevent reuse until checked retirement finishes.

The existing MaterialTextureGl direct-upload path supplies useful proof for one
family. It does not certify independently owned dynamic uploads, HUD uploads or
future uploader additions. Their concrete identity/registration integration is
the remaining prerequisite. No safe complete2543F0 join is exported here.
