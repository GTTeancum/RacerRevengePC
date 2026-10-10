# Source recovery

The initial recovered source tree exactly matched remote commit 51c6e7071df6b54d472f1aa16a86db0e6090f54a (Git tree 09dace21929d3802556d48f375163806e8025776). This branch starts new local recovery history; it does not recreate missing commits. The latest historical local commit 64a4aca is not restored.

These subdirectories preserve recovered payloads and newly reconstructed candidates. Their manifests distinguish exact historical hashes, complete but unverified payloads, older variants and new source. Some reviewed pieces have subsequently been integrated into the project root; preservation directories remain provenance records and must not be blindly copied over the current source.

The original UI renderer, material adapter and archive loader are inactive source integrations. Original UI archives and missing source-owned screen/depth/material wiring are still required for complete menus. Opt-in track fog remains disabled by default pending stronger source evidence and validation.

See docs/recovery-validation-20261010.md for exact checks: the focused renderer/adapter compile and native asset preparation passed. Neither establishes full runtime, GPU or gameplay validation. Changes made after each check remain untested.
