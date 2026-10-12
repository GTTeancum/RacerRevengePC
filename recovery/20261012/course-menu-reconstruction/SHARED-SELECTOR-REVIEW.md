# Shared selector-state review addendum

2026-10-12 03:01 UTC. Source-only review at main `c8e53dfd95c5e9cddb84349cd3de9bc1c1bd24c0`. No source edits, builds, tests, syntax/compiler checks, project scripts or guest execution. Original ELF was inspected as data with `od`; the existing fog and Rider word listings were read.

## Definite source identity

These are the same original static storage, not merely similarly named local caches:

- Rider voice1EA65C..1EA6AC reads/writes gp−0x7F58 (guard80A8) and absolute3D6B30 before timer101A50 and seed308020.
- Fog160998..1609E0 uses those identical addresses. The separate assertion guard/cache is also identical: gp−0x7F54 (80AC) and3D6B38 at160A08..160A44, matching Rider1EA6CC..1EA704.
- Native world-service source10189C..1018E0 has the same seed guard/cache words, and101900..101938 uses the same assertion guard/cache.
- Last-lap source1A57D8..1A581C and contact-music22A410..22A454 use the same seed guard/cache. This was independently confirmed against original ELF words, not inferred only from native names.
- All seed308020 calls write the same libc RNG word subsequently advanced by308030.

The source ELF is the retained `SLUS_202.68`, SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`, mapping file offset = VA−0x100000+0x80.

## Present bounded menu route

No demonstrated conflicting lazy-cache access exists in the implemented Main/Options→Rider19→Course→Back route. Main/Options fire uses three direct308030 draws (source15E2F8/15E314/15E330); its represented native fire owner does not seed this selector cache. Rider idle's represented path similarly draws directly. Main startup's explicit native raw-RNG seed does not initialize the original selector lazy cache and must not be made to do so.

Menu runtime is exclusive: `terrain_interactive_drive50du.cpp` bypasses race activation at2513 and continues out of the UI branch at3469 before world/race work. Gameplay launch remains disabled. Thus voice's private application-lifetime cache currently has no second admitted menu consumer with which to disagree. This qualifies, but does not reverse, the earlier bounded route review; it does invalidate treating the private cache as a generally shared source-static implementation.

## Existing native ownership gap beyond menus

`LapEventRandomOwner50ID` already represents guard80A8 and seed3D6B30. Runtime `sessionLazy` at terrain_interactive_drive50du.cpp338 is shared by lap/category6, announcer and contact music. `DynamicContactRangeOwner50IW` represents80AC/3D6B38 separately.

GC animation independently allocates `ServiceShared gcAnimationLazy` at525; its type contains all four same-address source fields. Service at gc_animated_world50ig_fixture.h298..310 consumes the same `s.noise.sharedRandomState` but uses this different lazy cache. This is an existing intra-session sharing gap, separate from the expressly unclaimed cross-race lifetime. After animation seeds the raw RNG, the first announcer/lap/contact selector can seed it again because `sessionLazy` remains cold. Original shared3D6B30 would suppress that second seed when its value is nonzero.

The Course candidate adds another private pair of seed fields and another private assertion pair inside `OriginalUiRiderVoiceChoice50KG`. Its application lifetime alone cannot establish sharing with those existing owners or future fog.

## Minimal candidate proposal, not implemented

1. Retain one instance of the existing `LapEventRandomOwner50ID` in `OriginalUiMainApplication50KG`, initialized once at actual application construction. Retain the existing typed80AC/3D6B38 owner there too, or factor its definition without changing semantics.
2. Make `chooseRider19` accept explicit references to those retained owners, along with the already genuine clock and noise owner. Remove all four voice-private copies. The receipt's seed value remains a snapshot only.
3. Leave the seed/draw/guard call order and bounded arithmetic unchanged. Do not initialize the selector cache from Main startup's separate raw seed. Preserve zero-timestamp reseeding and completed mutation prefixes on later failure.
4. Keep menu→race unavailable. Future admission must make lap/announcer/contact/GC animation/fog borrow those exact owners and the exact raw RNG/clock, rather than copy fields or allocate a fresh per-consumer cache.
5. Repairing existing gameplay's `sessionLazy` versus `gcAnimationLazy` duplication is a separate root-owned scope. A fresh-session policy can retain its explicit reset boundary while still sharing one owner inside that session.

## Identity pins

- Candidate voice header: `fc08fcbb3215ffe22b1c46ffcabe12c9a3df7713dd78c2d1642f0b3b363d30ad`.
- Candidate voice CPP: `18e2fc7d853fc7dcfba655fdae79a7b008bf96a94f2b06ccebbb692851d5cd6c`.
- Main lap_event6_subscriber50id.h: `f55c556b05440d80b409dfbb24d055962d59906b95bb6f57952ead7d2cd79799`.
- Main gc_animated_world50ig_fixture.h: `13a5f187636452944128ce55ddad17b8734e35c06fce96675f9f3d8701cbbd06`.
