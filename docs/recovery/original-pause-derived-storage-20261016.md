# Original Pause derived cold storage (2026-10-16)

Increment against `ff8dd4d`. Static source recovery only; no build, decoder,
syntax check, test, or gameplay execution was performed by this worker.

`OriginalPauseStorage50LA::loadDerived1BA390` follows the already-loaded shared
base with original derived resources. `derivedResources` is a cold-storage
progress stage, not a full Entity/load/draw/input readiness receipt.

## Source joins

- `1BA3C0..1BA3E8`: second one-name Application font request via actual registry,
  count 1, reload 0. ELF `382C78` points to `denmarkbold16ingame`, as does base
  `382C70`. A private wrapper preserves the actual caller and existing registry.
- `1BA3FC`: icon ScreenTexture at `+890`, reference 7, immediate whole-table
  material/texture population, then position (462,382). Dirty bit 2 is set only
  when `field3C != 0`, matching the source floating comparison/branch.
- `1BA4AC` font lookup supplies the same original font owner to title `+2D0`
  (60 glyphs, batches 36/24), prompt `+340` (40, batches 36/4), and eleven
  list texts `+3B0+i*70` (40 each). References 8..33 preserve primitive order.
  Each text temporarily sets the actual directory to `font\\` and restores it
  before source styling. Native glyph/packet pointers retain the original pair
  aliases. Initial packet RGBA is 127/127/127/63; source low dirty flags are 0xB.
- Title: justification 3, color E5D96C, scale float bits 3F99999A. Prompt:
  justification 0, initial same scale, position (493,389), localization 4,
  then scale 1 and color 669BCC. List texts get justification 3 and scale 1.2;
  unassigned labels stay empty. List indices 0,1,9 receive localization 8,401,424.
- `1BAB5C..1BAB70`: the actual Application+2DC/+11258 subtype is compared
  with **3**. List index 10 receives the original literal `END TIME TRIAL`
  in that branch; otherwise localization 425. The caller must supply that
  real source field; no host mode/player identity is inferred.
- `1BACBC/1BAD50`: left/right ScreenTextures, references 34/35. Source cached
  color condition writes RGB 127 without changing alpha.

Authored ScreenTextures resolve their originating retained reference through the
actual canonical table. The object must have the original ScreenMT association,
positive reference ownership, exact authored PSM bytes from the same archive,
and an actual retained first-pass/map texture. Source 2DF164/174 dimensions use
that map's actual resource width/height. No guessed icon/arrow PST/ATM name,
replacement texture, reconstructed material, or animation suppression is used.
Authored passes and animation ownership stay in the canonical object. Each
ScreenTexture retains the source immediate population boundary; ScreenText
primitive acquisition does not invent an extra population call.

## Partial ownership and remaining gates

A failed load preserves acquired objects/references for cold teardown. Directory
restoration also occurs on returns and exceptions. Derived destruction follows
1BA2C0: icon, eleven list texts in reverse, prompt, title, right arrow, left
arrow, then the existing shared-base destructor order. Within each text,
primitives release in forward order. Font handles are native lifetime borrows
from the retained registry, not invented per-text source references.

This completes the bounded resource/storage segment through 1BADF4, conditional
on genuine existing parser/population success. It does not implement subsequent
Entity callbacks, animation/setup 1BAE00, dynamic layout 1BB010, glyph regeneration,
ScreenObject draw traversal, retirement-gated GPU submission, input routing,
selection callbacks, or real race-parent attachment. These must be recovered and
joined before activation. Existing host menu behavior is untouched. Current
storage intentionally exposes no new drawable accessors or readiness flag.
