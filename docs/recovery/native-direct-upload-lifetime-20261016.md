# Incremental actual direct-upload lifetime

Base22863ba. Native integration only; no complete2543F0 admission.

The actual RaceDisplayLifetime now retains the SAME Application texture lifetime
passed by Application's existing private factory. bindTextureUploadRequest seals
that manager, the real display weak identity, actual Video host pointer and exact
current callback. Copies of a request retain this same immutable binding. A stale
request cannot borrow a new display, even if the Video host is later reopened.

The driver's existing base MaterialTextureGl request is bound before its real
uploads. Copies used for primary/opponent/track/cable/sky/specular images inherit
that binding. This operation occurs before renderer.create, as the genuine cold
upload order requires; the actual Video must already be current. It never marks
the renderer created or changes image readiness.

MaterialTextureGl50EB::create constructs a stack transfer owner immediately
before the actual client-memory upload. It validates the sealed context pair and
real display, retains that display through the call, and registers its actual
uploader identity in the canonical manager. Successful upload records completion;
all exits unlink the transfer. A failed partial upload also unwinds the transfer
registration after existing checked GL cleanup. Unresolved GL names still belong
to their existing persistent owner, without any retained client pointer.

The manager's native transfer list and attempt/completion counters are distinct
from source30's canonical resource list. They are not a fabricated guest13C, an
image-ready token, or proof of complete upload coverage. A registration refusal
occurs before upload, leaving the unpopulated candidate safely disposable. The
manager rejects destruction with a live transfer. Actual transfer scopes pin the
display and manager; retained request bindings weakly borrow the display, so no
renderer/request/display ownership cycle is introduced.

MaterialTextureGl's existing upload explicitly unbinds PIXEL_UNPACK_BUFFER and
passes actual decoded client memory directly to texImage2D. This scope models
that real call lifetime. Persistent decoded assets and GL images are untouched.
No texture values, samplers, image bytes, draw order or cleanup order changes.

## Uncovered paths

The optional binding preserves unrelated existing MaterialTextureGl consumers.
HUD-local upload requests and independent dynamic upload implementations are not
yet attached. Their absence must continue to block any aggregate claim that all
native transfer/cache backing is covered. No253DF0 null-state claim,252EA0 stub,
or full texture-manager reset callback is installed. Future complete cleanup
must admit all reached participating transfer/cache owners, perform the existing
actual renderer reset first, and retire only transient transfer/cache state.

No build, test, syntax check, gameplay or decoder execution was performed.

The actual display reset consumes the incremental ledger: it rejects a still-live
registered transfer before entering the real drain. This guard does not infer
that unregistered upload paths are covered or that persistent GPU names retired.
