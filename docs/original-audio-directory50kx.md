# Original audio directory and GC material bindings

This native metadata layer loads the real 267-entry AudioStr directory through
the existing authenticated APPSTART archive owner. It preserves original
ordering, token byte offsets and source ASCII case-folding. It does not infer
registry membership from whichever PCM files happen to be available.

`OriginalAudioDirectory50KX` requires the authenticated appStart archive and
AudioStr member 3. `GcMaterialResourceBindings50KX` additionally authenticates
the exact original GC AUB before using the existing BINARY/material readers.
The resulting 12 collision and 12 drive entries retain three separate names:
the original `.wav` factory filename, full `audio\\...vag` registry name, and
normalized `.vag` PCM request. Resource ordinals are never sample handles.
Six authored drive controls are retained without a guessed sample base pitch.

Both owners are immutable, noncopyable, nonmoving and load-once. Published
strings are owned; source archive, directory and AUB lifetimes do not leak into
the resulting views. Failed preparation leaves an empty retryable owner;
repeated successful preparation preserves the published storage.

Original identities:

- APPSTART.RES: 58,977 bytes, SHA256
  `04eb376a507d0745925e4351903a8ef7a1024d37d8250b78892cba0c74220b89`
- AudioStr member 3: 7,542 bytes, SHA256
  `be478aab56545abac25dcb677cd363e84003b13d81fc9a94246ad752741ed534`
- GC AUB: 32,438 bytes, SHA256
  `eb4046a25b5b08412c8fa8644107ab826499af754f1093283cc4566e2d879c1d`

The original asset bytes are not included in source control. The focused check
accepts an APPSTART.RES path and GC AUB path. It covers ordering, case semantics,
identity failures, stable ownership, all 24 resource mappings and independent
AUB offset expectations. Source review is complete; no test result is claimed
by this addition.

The focused executable links its test and these five production units:
`original_audio_directory50kx.cpp`, `original_ui_archive50kg.cpp`,
`material_audio50hi.cpp`, `import_world48f.cpp`, and `import_decode48c.cpp`, all
under `src/assets`, plus zlib. Use the existing native strict-FP C++20 flags.
One authorized build/run must use the centralized hourly slot; there is no
automatic test or runtime launch in the production source-list registration.

## Remaining live ownership

This is metadata, not an instantiated sound group, mutable resource registry,
sample loader, playback allocator, emitter or completed reset. The original
sound factory attaches distinct logical sounds to a real group; emitter rows
borrow them. Registry entries also change meaning during registration/loading.
Those canonical mutable owners and their lifecycle are still separate work.
Adding this asset unit to the integrated source lists does not enable a live
provider, invent a human listener or widen the zero-human audio scheduler.
