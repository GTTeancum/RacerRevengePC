# Native cable shared vectors and complete cold construction

Source-only and **untested**. This additive candidate extends integrated native-attribute delivery `libfile_a728f08e404481919f6f3d0454880e44`. No build, syntax check, test, converter, guest execution, emulation, simulated input or gameplay check was performed. Runtime entry points, full224720, RA activation and renderer readiness remain closed. Central integration/testing remain with root.

## Recovered startup publication

The previous scan stopped at340000 and missed the later executable static-constructor region. The ELF's executable-backed section extends from100000 through3D5A00. Static data review now establishes:

1. Startup1000A4 calls main239C40; main239C54 calls2EADE0 before subsequent main initialization.
2. 2EADE0 constructs table bounds3CC450 and3CC650 and tail-calls2E91F0. Its actual address construction uses LUI003D plus signed negative offsets, not a guessed unsigned address.
3. 2E91F0 walks increasing four-byte table entries and invokes each function pointer. Entry3CC4F4 is3C9490.
4. 3C9490 constructs four numeric vectors on its stack and publishes them with stores3C94EC/3C94F4/3C94FC/3C950C. In particular433CD0 is(0,0,0,1), and433CF0 is(0,1,0,1). These are explicit constructor writes, not inferred post-constructor BSS zeros.

Startup100048/10005C establishes GP3DD970. Direct signed16-bit GP displacements cannot reach433CD0/433CF0. The function-pointer table is the relevant indirect startup publication. No guest instructions are executed or interpreted by this recovery. Raw words and table addresses remain evidence only.

The same retained native Application now owns these two immutable numeric vectors for its entire fresh lifetime, before racer construction. There is no public setter, caller-vector argument or ready flag. This does not import or adopt arbitrary live guest state.

## Native construction advance

For each source-derived strip in order, the existing material/attribute construction is followed by actual mask8 attachment. For each of its18 original vertices,174138 first writes all triple aliases from the Application's433CF0 value, then174160 writes all XYZ aliases from433CD0. Native batches remain16/4 with two aliases for original vertices14/15. Position strip-control values remain separate and unchanged. Only genuine setters replace the corresponding unknown scalar values.

After all strips, four independent nine-float basis arrays70/74/78/7C are allocated in source order. Native scalar expressions preserve1741EC..1742A0's operation ordering. For each sample, each strip receives UV(0,10*t) at even vertex2*i, thenUV(1,10*t) at odd vertex2*i+1 through all aliases, witht=i/8. Finally the actual cable Entity is appended to the same racer's detached7F8 using source mask1008 at214238. The result `selectionAttachmentComplete` describes this cold attachment only. Material population, texture demand, callback dispatch, per-frame geometry updates and native rendering are not claimed by that result.

The precondition intentionally admits only the represented fresh detached7F8 topology, with no foreign children or parent. It does not silently remove an existing owner. Failures retain the exact reached strip and allocation prefix for explicit close.

## Teardown review

Additional source review distinguishes release142590/1424E0 from destructor1413E0. Release traverses siblings from tail to head before reaching concrete destructors through104970. Destructor1413E0 only decrements the shared object counter and releases its base; it does not traverse children.

Close first validates the complete represented topology. A detached final partial strip is unwound first; attached strips are released tail-to-head, repairing sibling/head links. Each source reference is decremented before its concrete resource teardown; its Application object counter is decremented at the base destructor position. After all children, the parent is removed from detached7F8, its reference is decremented, and173D40 releases6C,70,74,78,7C,58,5C,60,64,30,68 in order. Parent counter and racer254 alias are then released. Explicit close must still precede racer/Application destruction.

## Central source-review checklist

- Match MANIFEST preimages against the integrated native-attribute candidate; root alone merges.
- Review the complete startup chain and typed constructor-table evidence, including signed table bounds and exact vector stores.
- Review23C260/23C090 alias writes and23C800 UV writes against the native methods; confirm no command serialization was reintroduced.
- Review173EB0 loops, basis expressions and214238's actual parent/mask.
- Review partial-allocation failures and reverse sibling release against142590/1424E0/104970/173D40/1413E0.
- Central compilation/tests remain outstanding. This artifact establishes neither PS2 bitwise floating-point equivalence nor visible gameplay.

All recovered executable evidence is from SLUS_202.68 SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`; range hashes and additive preimage hashes accompany the archive.
