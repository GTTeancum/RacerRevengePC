# Explicit native loaded-world ownership retirement

Source-only. No build, test, syntax check, decoder execution or gameplay was performed.

The new aggregate closeNativeLoadedWorld() entry permits the existing real native cleanup sequence to retire an acquired counted world prefix. It is explicitly native ownership cleanup; it neither calls nor reports completion of original26FD80,148110 or103DD0. Ordinary close() retains its existing refusal while the counted world remains.

## Before any destructive cleanup

The actual world Entity must have reference1, its same captured identity/session, and no parent, children or sibling links. Actual Application attachment lock must be clear. The actual audio owner, mixer epoch and creating thread must match. The world list is inspected only when its backing was acquired; nonempty lists reject. Every acquired3CC zone must be counted, detached and reference1, retaining its authentic descriptors with no live wrapper04; canonical collision backing must have no query or ref60 borrow. Every acquired3D4 zone remains the same admitted Standard-only empty model index.

The seventh group's authoritative canReleaseEmptyCold142590 preflight must pass whenever that actual group exists. This is a dependency on the audio owner's matching nonmutating preflight; no duplicated voice-admission rules or callback success receipt is introduced.

All old aggregate lifetime gates remain: camera/world borrows, navigation lease, scene topology, static-world image/renderer publication, static model/material references, runtime bank/camera/FIFO/query retirement, lighting and track-particle pins. A counted world also requires unique ownership of runtimeQueries_, ensuring its reset actually destroys runtime Storage. Race B8/BC/C0 navigation borrows and actual native voice bindings therefore must retire first.

## Actual cleanup and retries

The unchanged existing aggregate sequence closes actual emitter/track-particle/static-world owners and scene topology, then destroys runtime/registry storage. Only afterward does the new leaf release the actually acquired unloaded canonical collision backing,3CC zones/descriptors, seventh empty group,3D4 index, world list and world Entity count/reference. Each owner is reset or marked retired immediately after its actual release. A later rejection retains remaining ownership; retry cannot decrement an already released owner again. No source field with an unreached producer is read to invent a destructor gate.

The final world release requires runtimeQueries_, trackParticles_ and environmentMethod_ gone and any attempted topology closed. Application authority retirement occurs before world reference1→0; the actual423F60 counter decrements once. The aggregate then releases candidates/navigation/source and its collision Application pin and records closed.

## Integration

Wire's race-level owner must call closeNativeLoadedWorld after retiring its genuine outside borrows and native audio bindings, then release its retained aggregate. This patch does not add driver activation, alter original AI, or supply Pause parent/input callbacks. Any future positive27B040 query owner or nonempty world list must extend these concrete ownership gates and cleanup rather than relying on today's cold-only admission.
