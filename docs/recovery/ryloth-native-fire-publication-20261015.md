# Native FireP0t publication and retirement candidate

Base: central 364087f. Isolated source-only candidate; no compilation, syntax check, conversion, GL call or gameplay execution performed. `native-fire-publication.patch` modifies five existing files, with new definitions in the already-built native renderer/image-owner translation units. No startup or RA course admission is added.

## Concrete joining API

1. Prepare the existing authentic `ra_static_fire_visual50LA::Owner` with the actual RA source, Application material domain/context and current path; call its real final population only after the intended references are staged.
2. `MaterialTextureGl50EB::createRaStaticFire(request, sharedOwner, diagnostic)` uses that exact owner's immutable pixel bank. It reacquires the actual canonical map, admits only source sampler `{0,0,1,5,0,-75}` and repeat/repeat. With MXL=0, MMAG=1 and MMIN=5, the admitted host sampler is linear base level in both directions. It does not invent generated mips or accept a caller's replacement image/hash.
3. Append `request.track.raStaticFire=sharedOwner` to the existing genuine renderer request, together with its exact mesh, four exact source-order spans, exact source image GPU owner and actual shared context/callback. The generic track path rejects a typed FireP0t image bank if the matching source owner is omitted. No caller-supplied retirement or readiness boolean exists.
4. Existing renderer validation still requires actual vehicle assets, bounds, budgets, context, image publication, span partition and index ranges. This is a bounded track-input slice, not a standalone RA world or a whole-scene renderer activation.

## Publication leases and cleanup

The source owner has two private typed publication links, accessible only to the actual image and renderer classes. Each publishing class simultaneously holds a strong source-owner reference. No source-to-publisher strong reference exists, so there is no ownership cycle. Source owner explicit `close()` returns false before releasing any facet while either private link is live.

Image publication happens before upload. A failure with no retained GL storage immediately retires that link. A partial upload/cleanup failure retains both GL storage and the source owner for the existing retry contract. Success does not imply any draw.

Renderer publication happens after all cold input validation and storage allocation, before initialization can create GL objects. A failure before any GL allocation retires it through the existing empty cleanup branch. Otherwise cleanup now includes this lease in the existing checked `glFinish` obligation. Wrong thread/context, finish error, unbind error, failed deletion or remaining names retain the lease. Only acknowledged retirement of all renderer names releases it. The no-names branch requires no invented completion: it can only precede GL work or follow already-acknowledged cleanup. Even when stale GL errors make close return false, a genuinely complete cleanup releases the lease consistently with existing `cleanupIncomplete` semantics.

Image `close()` rejects while the renderer link is live, without changing publication. After renderer close has genuinely retired its names, image close performs existing checked texture deletion, retaining failures for retry. Only then can source close release canonical material references. Destructors preserve existing terminate-on-incomplete-cleanup discipline; callers must close renderer, image bank, source in that order. All accesses retain the existing serial creating-thread/context ownership contract, not a new concurrent publication protocol.

## Exact material and geometry binding

The four spans remain the actual source order and actual worldInstance associations already retained by the CPU owner. The validator compares every source vertex/index range, exact mesh pointer, actual image owner and all current canonical maps. It requires original vertex RGB scale 1 and UVs, PreLitCompressed native prelit lowering, opaque blend, depth write, inclusive depth, GREATER/0/KEEP alpha test, source fog enabled and one nonanimated pass. It rejects projection sidecars, host lighting normals and nonmatching policies.

The constructor's 35 raster fields are checked in declared enum order against DefaultMT plus the authenticated gargoylebody override. DefaultMT sets alphaReferenceValue=0; the rest use the source constructor defaults. All six sampler fields are checked too. A runtime/context override that produces different map fields is rejected, not silently ignored. Per-frame revalidation reads current canonical maps only; it never dereferences the create-only span arrays or CPU mesh pointers, which renderer initialization clears as before.

Existing native world fog remains caller-supplied actual scene policy. Existing native pod-shadow receiver/culling policy is not upgraded into proof of unrecovered retail model flags. No retail `240110` palette/trailer value, source model pointer, stamp or completed traversal is synthesized.

## Real draw receipt boundary

The existing `completedTrackSpanVisibility` remains the only usable completed-submission receipt: its four span IDs map to this owner's actual source-order spans. It publishes only after actual renderer completion and reports issued draws, not pixel visibility. The candidate does not increment any SoulTree stamp. A future integrated scene adapter must consume that actual receipt and map each span to its retained real worldInstance before updating the admitted native scene's stamp owner. No such adapter or RA startup call is included.

## Remaining integration boundary

The GPU image owner is still borrowed by the renderer according to its established API and must outlive it; the private link makes premature checked image close fail. Source material/domain/context must remain live and collector current, checked again during frame validation. Any future general world batching must preserve these four exact source identities and extend the typed lifetime contract rather than copy their pixels into an unrelated bank.

This patch does not create a particle manager. The eventual RA attachment path must use the existing actual `OriginalTrackParticleCold50LA` single 5000-slot owner and the four authored SoulTree attachments, with actual Application inputs. It does not manufacture RA collision/query extent, course admission or renderer success.
