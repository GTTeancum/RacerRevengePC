# Avoid duplicate native raster operand initialization

UNTESTED. Source-only independent review; no builds, syntax checks, gameplay,
benchmarks or instrumentation. No measured FPS benefit claimed.

NativeRasterVertex no longer has default member zero initializers. Its explicit
producer nativeRasterVertex still begins out={}, initializing clip, error and
outside before arithmetic or early return. No mathematical operation changes.

Full use inventory in the owning cpp and its ten direct inl includes found the
type only in ra_static_world_gl_storage.inl: the producer, const consumer, two
three-element local arrays, and CachedNativeRasterVertex::operand. The existing
racerRasterOperands{} value initialization remains, as do occupied/finite gates.
The direct local array reaches the consumer only after all three producer calls
succeed. The cached array reaches it only after three complete finite-record
copies. Earlier failure or bounds returns never consume partial arrays.

No raw-byte comparison, serialization, padding observer, escaping reference or
wholesale uninitialized-array copy exists. Whole-record member copies originate
initialized finite records. The changed type remains an ordinary aggregate;
explicit out={} and enclosing cache value initialization still zero members.

This removes redundant requested initialization of eight doubles and one unsigned
per local vertex before producer reset or complete copy, nominally 204 member
bytes per three-vertex array (excluding padding). Compiler dead-store elimination
may already remove it. Runtime correctness and benefit remain unvalidated.
