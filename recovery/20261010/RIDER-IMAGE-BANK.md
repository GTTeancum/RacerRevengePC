# Combined authenticated UI image bank

New original_ui_image_bank50kg pair only. No shared modifications or preimages.
Depends on frozen IPUM ScreenAssets third-argument/cache/provenance extension.
No builds, syntax checks, tests or project parsers/importers were executed.

This is a native cold composition owner required by the existing renderer's
whole-screen texture bank contract. It does not emulate a guest texture manager.
Actual Main/Options IDs and binder owner-local IDs cannot be merged by numbers.
Each input owner retains its source ID plus explicit combined index/ID mapping.

Caller pins all real source archives and immutable source image owners. Resolve
provenance by exact canonical member identity pointer within those authenticated
archives. Preserve original PST frame byte range and exact associated CEL. Fonts
with and without CEL are distinct metadata tuples. Duplicate identical tuples
share a single combined entry; differing tuples cannot silently collide.

Combined IDs use native namespace5549, 15-bit global original catalog ordinal+1,
a CEL-presence bit and original frame byte offset. They are independent of source
owner IDs and input-bank order. Frame size and CEL identity remain explicit tuple
checks; any collision with a differing tuple rejects. Canonical source provenance
remains available from combined ScreenAssets. Do not treat numeric ID alone as
source authentication; the existing genuine provenance/texture guards still apply.

All combined pixels are decoded again from PINNED AUTHENTIC ARCHIVE bytes using
shared ScreenAssets, including the actual typed IPUM cache where needed. Full
revision/kind/flags/palette/mip bytes and offsets/base RRTEX are compared against
every source owner before publication. Nothing clones caller pixels or invents a
successful missing IPUM decode. Combined owner preserves all source mip evidence,
font metadata and IPUM conversion provenance, within existing256-image/byte limits.
No GPU upload, material admission, default sampler or destination readiness occurs.

Typical join:
1. Resolve/load actual font, icon, model texture and binder ATM-frame owners.
2. Supply pinned authentic archives plus owning or aliasing shared_ptr image
   owners (binder alias pins the binder asset owner, not a no-op deleter).
3. Build bank once, then construct material/geometry spans using combined indices
   and identities and original program/sampler/depth/fog/blend contracts. Remap
   BOTH index and identity. Never mutate a validated span's ID as a bypass.
4. Publish images() through root's real whole-screen renderer-bank switch only
   after all route owners are complete. Root's generation/guard/lifetime checks
   remain authoritative. Drain old spans before switching/releasing a bank.

This owner can compose the current real destination resources or a larger admitted
roster bank. It does not preload an unsupported rider, omit missing surfaces, exceed
existing budgets, or promise that every Rider texture format is already supported.

Archive input bound is eleven, matching the current authenticated catalog after
original UIPODTEX was appended. This is a capacity bound, not a requirement to
load all eleven archives for one screen. Existing image/owner budgets are unchanged.
