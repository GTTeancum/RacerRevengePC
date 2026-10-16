# Bounded original lap HUD GPU rendering

This source-only change adds a real gameplay GPU path for `OriginalRaceHud50KG` objects12C,130,134,164 and the two gleam owners184/194. It renders into the pending native gameplay transaction after the existing native HUD and before gameplay blur. Native gauges, general status, countdown and KO remain on their existing path. This is not full retail HUD recovery or a claim of executed validation.

## Source geometry and ownership

The existing loaded race owner retains authenticated APPSTART/OVERLAYS/LOC, the real CEL/PST font/image bank, the parsed ScreenMT materials and the actual stable race viewport. The new renderer cold-binds that owner rather than accepting caller-generated readiness or copied diagnostic quads. It borrows the owner through renderer close. Only one prepared640x448 view at origin0,0 is admitted; scene identity, generation and texture publication are checked again for each frame. An inactive second view is not fabricated.

Text29D53C..29D68C uses the pending overlay color/alpha/scale/rotation and justification74, independently of the last displayed ScreenText caches. Source2E13A0 centers using scaled integer width/first-glyph height and truncates positioned coordinates.2E0A30 divides those coordinates by scale before CEL bearing/half-pixel arithmetic;2E0F50 then scales the genuine corners. The new code reuses the native CEL layout and packet-color helpers. RGB bytes retain the original shift by1 and signed-short alpha the shift by2. Text rotation is admitted only at the actual lap value0. Warm storage is bounded to164 quads and8 leaf packets, without heap growth.

Gleam2E6960 uses elapsed/duration, source x/y/travel/offset, width30, height50 or100, and the actual angle0x3F060A91. The rotation uses the already recovered13BFE0 native trigonometric owner, not a different host trigonometric approximation. Both embedded ScreenTextures retain the same geometry, original full0..1 UVs, bright RGB127/A63 and clear RGBA0. Source strip lanes are reordered to the existing native quad index order.

## Materials, ordering and alpha

The renderer reads the authenticated APPSTART material registry from the race owner. Its3235 space/CR/LF-delimited tokens, including comment tokens, determine the actual leaf ranks. Stable sorting retains the constructor/FIFO order within a font or gleam leaf. The admitted order is gleamclear, font14gleam, font30gleam, gleam. The named Gleam group does not itself authorize a prolog/epilog: these PSMs have no such sections. No diagnostic group is substituted.

A dedicated typed material adapter admits only the actual catalog identities gleam/gleamclear and their exact604-byte gleam PST provenance. Existing generic UI admission remains unchanged. Clear is ScreenMT ATE1/NEVER/AREF0/AFAIL FB_ONLY: zero packet RGBA preserves destination RGB, replaces stored alpha with0 and suppresses depth writes. The bright pass is ALPHA A=source,B=zero,C=destination-alpha,D=destination. Font source-over uses source alpha. All surviving fragments independently replace the raw stored alpha byte.

The existing native source-byte UI shader performs these equations in a private RGBA8 target at the full native drawable capacity. The pending gameplay RGB is copied at its actual dimensions, without downsampling to640x448. Every bright quad has an identical earlier clear quad, so its destination-alpha reads are dominated by the real clear/glyph sequence; initial gameplay alpha is not treated as recovered GS alpha. Ordinary font source-over does not depend on destination alpha. The source target never escapes into the sealed Rider/Options source-owner API or supplies a fake UI receipt.

An already compiled coherent framebuffer-fetch variant can be reused. Otherwise GL4.5 or ARB_texture_barrier is required and each actual triangle is drawn separately with a barrier before it. No fixed-function SRC_ALPHA shortcut loses source alpha greater than128 or the bright pass's destination-alpha coefficient. Four authentic images are uploaded through `MaterialTextureGl50EB`, with their parsed immutable samplers.

## Explicit native presentation boundary

Source ScreenText/ScreenTexture default depth is1; video maximum0xFFFFFF produces packet Z16777214, and the admitted materials use GEQUAL. The layer owns a separate D24 depth plane cleared0, retaining the source GEQUAL and per-pass depth-write behavior within the admitted objects. It does not share or claim recovered retail world Z. Native viewport presentation uses a640x448 letterboxed canvas and native GL rasterization; original GS subpixel coverage and nonuniform retail display mapping are not claimed. Broader projection/depth parity remains separate work.

After all source draws succeed, one color blit transfers the result into the pending gameplay target. Native presentation alpha is restored to255 there; source alpha stays private. The existing complete-frame transaction publishes only after the HUD and later blur succeed. A GL failure aborts that pending frame and retains the previous published image. An unsupported CPU live state skips only this optional lap layer, preserving the existing native HUD/world render.

Cold rejection retires any new HUD work before deleting its names. Failed deletion retains ownership and reports cleanupIncomplete. Empty CPU rejection drops the empty owner. A failed late shared-UI-program initialization cannot be mistaken for a reusable ready program on a later bind. Rebinding an existing owner is rejected without deleting that owner's resources. Normal renderer close drains and deletes the owned HUD textures/target/depth before releasing shared shader resources.

## Actual integration and remaining scope

The human course runtime binds after authentic viewport/HUD construction and passes the same owner only while the original camera admits visible HUD. Existing native HUD still draws. Four joined text objects and two gleams can render; enabling an unjoined status/countdown/KO/numeric/explosion text state rejects this optional layer. No arbitrary test string, synthetic event, forced glyph batch or second-player effect is admitted.

The diagnostic quad/leaf counts are set only after nonempty checked GPU draws and copy-back. The host emits a first-display notice only after the existing actual gameplay completion boundary; cold binding and empty frames produce no such notice. Cache commits retain their existing completed-display contract and do not impersonate GPU receipts.

One new included implementation file, `platform/linux/native/original_race_hud_render.inl`, is compiled through the existing shared renderer translation unit on Linux and Windows. No translation unit or decoder was added. Preparation used source reads, raw retained ELF-word inspection and staged edits only. No build, test, syntax/import/lint check, sanitizer, gameplay, project asset decoder, guest/VU execution or user-PC operation was run. Windows-first and later runtime validation remain centrally owned.
