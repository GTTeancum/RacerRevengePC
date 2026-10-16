# Same-resolver kind2 framing / compressed CPU slice

Isolated source candidate against9acd7fb (including indexed flags0 bce329c). No execution, tests, syntax checks, build, conversion or runtime activation. The integrated checkout was read only.

## Implemented scope

- Existing resolver append admits a separate kind2-only ATM branch. Maximum8MiB source,64frames,1..5mips,16..1024 power-of-two ascending dimensions remain bounded. Every frame/PST extent,20-byte mip framing, payload length,16-byte IPUM header, frame flags and B0/B1 terminators must consume the exact source. Mixed-kind ATM and standalone kind2 PST remain closed. Existing indexed/TGA parsing paths are unchanged.
- Immutable source remains in the existing resolver File row. There is no additional provider namespace, copied archive identity, texture table, clock, material or runtime registry.
- `resolver.stageIpumFlags0(actualSharedResolver, sourceIndex, out, diagnostic)` requires self identity, a prepared corpus, the SAME sealed provider, positive actual mounted member, complete source byte equality and real mount digest. It mints only `shared_ptr<const OriginalStandardIpumCompressedFrames50LA>` through its private constructor.
- Each source frame receives its own64-byte-aligned native compressed allocation, including repeated source/pixel content. Only final source mip is copied, as flags0 requires. Allocation base corresponds to level+00; codec view corresponds to+18 after16-byte header; codec length is payloadBytes-16. The complete PST/mip/payload offsets and actual ordinal survive in FrameView. No compressed byte is reinterpreted as a palette index,1555 pixel or RGBA.
- Source/provenance/provider ownership outlives every buffer. Successful corpus append cannot move source File backing; stage stores source index, never an Entry reference across calls. Within a call all operations are serial and no callbacks run.
- Aligned allocation precedes copy and view publication. `out` retains an actual copied prefix if a later allocation fails; `copiedFrameCount`/`frame` expose only successfully copied buffers. `compressedComplete` is false until every source frame and final provider revision check succeeds. Diagnostic sourceEffectsCommitted remains false because these are native staging effects, not original manager/cache writes. Reusing nonempty out rejects.
- Destruction clears/frees frames ascending before source/provider owners die. This stage has never been submitted to GPU or registered in29C6E0, so it does not fabricate the original global443870 drain. A future actual canonical resource still needs its genuine retirement integration.

## Explicit closed boundary

`OriginalStandardTextureResource50LA::construct24DA30` rejects kind2 at24F148 before ordinary/aggregate allocation, lifetime-list registration or cache publication. Existing table population remains fail-closed/terminal if requested prematurely. No canonical constructed flag, format1555/E8/F8 projection, material readiness or GPU claim is set by the stage.

`OriginalStandardIpumImageLease50LA` is an opaque reserved type with no definition, constructor, setter or factory yet. No image lease can be fabricated through this patch. The immutable, privately minted compressed owner is the typed source request for its eventual verifier. This is deliberately a bounded parser/owner slice, not the finished image bridge.

Required next API shape: an actual compiled-catalog verifier accepts `shared_ptr<const OriginalStandardIpumCompressedFrames50LA>` and frame ordinal, verifies complete stage, real source/mount/frame/mip identity and hashes against compiled actual conversion output, then returns `shared_ptr<const OriginalStandardIpumImageLease50LA>`. Lease must strongly retain stage/source and immutable authenticated RGBA. No caller expected hash, arbitrary image callback or Boolean readiness argument is permitted. It must revalidate the current provider/source association before canonical adoption; the stage's retained provenance describes its original cold snapshot, not future mount readiness.

Source archive digest is copied from the actual provider mount, while exact member bytes and all relevant offsets remain available for genuine hash verification. This slice does not add a second SHA implementation or claim it verified compressed/output hashes. That belongs to the typed catalog verifier. If byte-identical TA/RA mounts are present, the recorded digest is the provider's actual first positive resolved mount; do not retag it RA. The RA caller additionally retains its independently authenticated RA source evidence.

The pending lease must preserve RA generator frame identity even for equal images. Its generated catalog does not exist yet; no output hashes are invented here. The actual canonical resource must eventually consume/adopt this compressed backing rather than silently allocate a parallel diagnostic copy, set genuine kind2 fields and own all real texture/material references, and keep resource/render lifecycle checks. Any adoption API must explicitly transfer backing once or retain the stage as the one compressed backing owner. None is added before that consumer is proven.

## Source anchors / integration

10237C/10239C: real child store and flags0 load.24EADC..24EBC8: skip small mips/load final into native slot0/count1.24F158: payload byte count.2507D4..2507F0: full byte count and aligned allocation.24F198: complete payload read.24F1A0..24F1C0: base retained and codec offset+16.24E370..24E454: five-slot backing/palette/resource cleanup, separate decoder drain/list unlink in actual resource.

The parser header is internal under assets/detail and used only by resolver append. The staging class implementation lives in the existing texture resource translation unit, requiring no new build-unit registration. Header declarations and source patch are packaged together. Existing programs/material guards, source animation service and RA course admission are untouched. Root owns source review/application and any later centralized checks.
