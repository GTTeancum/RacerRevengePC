# Actual renderer reset suffix, inactive

The authenticated SLUS_202.68 static words prove that renderer field40 is its
function-table pointer, not a separate backend object. Constructor243A80 calls
base26F870 and installs table3D2050 at243AA0. File-backed word3D2074 (table+24)
is00245090. Function245090 sets a0=1 and tail-calls242EE0 with a1=3.

2450A0 loads this virtual slot and calls it before changing flagsD8.
2450C0..D4 preserve every bit except bit0, which is cleared. Raw30040001 at
2450C8 is ANDI a0,zero,1, so its result is zero. There is no D8-enabled gate
before the call and no requirement that an image has been committed.

resetOriginalRenderer2450A0 uses the existing native initialized-context GPU
completion once, then mutates the actual borrowed dispatch fields. It does not
run244D60, submit source queues, close renderer names, or replace the separate
two-drain2458C0 path. A failed completion leaves D8 unchanged. A pending original
callback or any nonzero queue count is explicitly unsupported before completion;
this is a bounded admission restriction, not an assertion that original2450A0
has those tests. The concrete RaceDisplay owner must also admit only its real
idle renderer/context and preserve ownership through completion.

The existing RaceDisplayLifetime50LA privately owns both the native renderer
and dispatch_{1,2,0,0,0}. This is the genuine integration point for a private
cleanup operation, rather than a caller-supplied completion receipt. This
kernel alone does not activate race cleanup or Pause.

Additional constructor evidence: Video259E00 writes zero to44 at259F58 through
its saved actual this pointer; renderer base26F870 writes zero to2C at26F880.
Their nonzero producers and pointer meanings still need recovery before adding
native storage or claiming those full race-destructor stores are represented.
Likewise race cleanup's independent global3E1C10 reset is not supplied here.

Static source inspection only; no builds, tests, syntax checks, gameplay,
project decoder execution, or guest execution performed.
