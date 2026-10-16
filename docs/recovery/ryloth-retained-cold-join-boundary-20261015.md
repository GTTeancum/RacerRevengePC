# Retained cold constructor boundary after213380 — source review,20261015

## Connected change
The actual Session retained-model append now reaches213380 for right/left/bottom-right/bottom-left/chariot immediately after its existing detached7F8 alias append. attachedFor checks actual animation.model04 and racer aliases without requiring a completed join; finishedFor additionally requires the material pass to have returned. Failure retains the appended owners for ordered close and leaves completion false. Normal mode uses the canonical material table; unsupported ghost replacement still stops213428. No new caller-ready flag or runtime activation.

Session reports the following uncompleted source boundary: right210CBC, left210F80, bottom-right211284, bottom-left211580, chariot2119B0. This remains the existing bounded model projection, not a claim that intervening particle/effect construction ran. The new work joins a recovered operation into the real retained owner instead of adding another uncalled adapter.

## Exact2F0 /stack0F70 provenance
-210BBC/BC0 loads shared434630;210BCC saves all16 bytes at stack0F70, including the branch delay slot. Original initializer3C9FDC produces(0,0,0,1), but the source reads its live shared owner.
- After right213380,210CC4 looks up nullcg using28C590.210CD8 calls28B1A0 with relative=null to obtain its WORLD XYZ into racer2C0; this is not local XYZ.
-210CE8 resets that nullcg's LOCAL XYZ through28AB90.210CF0/F4 copies2C0 to910,210CF8/CFC copies2C8 to stack0F78,210D00 zeros2C8,210D04/D08 copies the modified2C0 to910 again. Thus stack0F70XY remain source434630XY while Z is right nullcg worldZ captured before reset.
-2119B8 finds chariot nullcg.2119C4 calls28B180, which copies LOCAL F0/F4/F8 only into racer2F0XYZ, preserving racer2FC.2119D8/DE8/DF8 subtract stack0F70/74/78 in order and store each result.
-211A0C..A1C squares XYZ and accumulates with source MUL.S/MULA.S/MADDA.S/MADD.S before SQRT.S.211A28 and211A2C store the same result to854 and858. Later2126xx alters858; do not substitute that later result here.

The current Rig initializes all canonical model graphs during install, then normalizes engine nullcg, samples motion and performs numeric setup before Session appends these objects. Reading current graph at finish1538B0 would therefore use a later pose. The numeric fresh-geometry binding contains derived target/length values, not retained execution of these field writes. No derived target, re-read PSG matrix, caller vector or later world cache is accepted as the missing cold stack owner in this patch. The exact next implementation needs an owner-bound capture at the existing pre-normalization model construction boundary and publication into the existing canonical target/length owners, with their native lifetime staging made explicit. It must not add shadow live2F0/854/858 fields.

##190/194 and208/20C
21178C publishes a new SceneObject190,21179C invokes its Entity slot30, then2117B8 appends it. On the engine-present branch2118E8 publishes a distinct SceneObject194,2118F8 invokes its slot30 and211914 appends it.211A34 then calls28BEF0 with actual190 parent and194 child. Existing Rig main/container indices and already-parented numeric nodes are not these missing retained Entity owners. Do not replay parenting on that later graph or claim those indices constructed the source objects.

211A4C finds the first wrist, checks its parent and grandparent, allocates0x60, calls28EB90 at211A9C with actual434650 axis, then publishes208 at211AA4.211AB0 does the second chain;211AEC..211B30 negates each actual434650XYZ and writes W=1;211B34 constructs,211B3C publishes20C. Absent endpoints/parents skip those allocations; allocation-null and prior successful owner mutations remain distinct.

Existing initializeChariotIk49T implements pair numeric construction from a supplied pre-motion boundary, requires both bindings, and commits both records atomically. It does not supply separate optional208/20C allocation/lifecycle and cannot stand in for this sequence. Existing Rig handRecords likewise do not prove these retained source objects were constructed. A concrete one-record lifecycle must join the real scene objects before consuming this numeric implementation. VirtualA0 at212DD8/DDC and2146C0 at212DE8 still follow in the source; no completion token bypasses them or enables late strips.

## Bounds route enclosing update
228704 belongs to common base update228620, not a constructor. Direct callers found in original words are16E99C (AI),1A1284 (player special branch),1A1644 (player normal tail); source data word3D17C8 also points to228620.228620 initializes lazy assertion state, tests Application11C at22865C and may take141ED0 then return; otherwise it performs the source entity20.bit08 assertion before2286F0. Then the existing B9C.bit80 gate reaches217440;228724 continues with340+288 position toCA0,229BB0, mode-specific controls, animation and later update work. The private bounds cut remains uncalled; neither missing radius nor a ready flag proves preceding updates completed. Preserve original AI callers.

## Review checklist and status
- [x] Real model append-to-material callsites connected; actual aliases validated before pass, completion after pass only.
- [x] Original partial attachment retained on failure; no nested Bank visits introduced.
- [x] Precise cold vectors, XYZ-only stores, separate IK optionality and outer update callers documented from original words.
- [x] No forged cold pose, replacement geometry/collision, effect completion, camera/observer lease or gameplay claim.
- [ ]2F0/854/858, retained190/194 and208/20C remain blocked on the ownership boundaries above.
- [ ] Root integration and all tests remain pending. NO builds, tests, syntax checks, sanitizers, converters or gameplay run. RA inactive.

Evidence is readonly static word annotation of SLUS_202.68 SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30; range hashes included. No original execution, guest commands or simulated input.
