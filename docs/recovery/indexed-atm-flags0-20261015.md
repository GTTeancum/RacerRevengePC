# Indexed ATM flags0 mip construction candidate

Source-only isolated patch against frozen e771b0b. No tests, builds, syntax checks, project execution, conversion or generated artifacts. Central checkout untouched.

## Original proof

- 10237C stores each real child before loading. 10239C calls24E880 with a2=0 in delay slot1023A0. 24E8A8 copies a2 into flags register s1.
- 24EAD0..24EAD8 obtains authored count and starts descending slot=count-1. File order is the validated small-to-large sequence.
- 24EADC..24EAE8: flags&2 loads every slot; flags0 skips any slot other than0.
- 24EB50..24EB80 reads each skipped16-byte header and calls24EC00 to advance over the actual indexed payload. Kind0 skips width*height/2; kind1 skips width*height. No pixel transformation occurs for skipped levels.
- 24EAF4..24EB10 selects destination resource+100+34*slot;24EB38 invokes24ED50 for the loaded level. Flags0 therefore loads only the final stored/largest mip into native slot0.
- 24EB90..24EB9C stores dimensions from that final header. 24EBB0..24EBC8 publishes authored count under flags2, otherwise1.

Full static words are retained alongside ra-atm-candidate/pst-constructor-words.txt. The caller words are in atm-child-loader-words.txt in this directory.

## Patch and preserved boundaries

`indexed-flags0.patch` changes original_standard_texture_resource50la.cpp and its header documentation. Existing bounded parser still validates every stored mip, exact frame extent, ascending dimensions, palette and indices. Indexed kinds0/1 and count1..5 remain the only admitted canonical PST subset. The splitter's larger count limit does not expand canonical admission.

Child construction starts at source ordinal count-1 and fills native slot0; sourceMipOrdinal retains the actual final ordinal. The original file/palette and complete child PST extent remain immutable and retained. Width/height come from slot0. Transfer conversion and backing allocation run only for the loaded kind1 mip. levelCountB0 becomes1 for children. Ordinary nonchild flags2 remains byte-for-byte the same operation sequence; existing single-mip ATM children are unchanged. No TGA branch, context, texture list registration, aggregate partial-child retention, release or GPU publication changes.

Validation remains a bounded host admission pass before original-equivalent resource construction. It does not claim original malformed-file partial-I/O parity. Failure during loaded mip transfer keeps the existing registered partial child and its owned backing; aggregate construction remains terminal through its existing caller contract.

## RA scope and remaining work

This fixes a proven independent canonical loader limitation, but RA crowd's six kind2 IPUM frames remain rejected. No pixels, generated-output hashes, material program permissions, domain activation or renderer claims are supplied. fholobeam's existing one-mip indexed frames behave identically.

The model-template route is already implemented by OriginalStandardMaterialReferences50LA::acquire26E640: it requires an actual prepared OriginalStandardModelResource50LA, uses actual36B970 prefix, stores each numeric token at26E7C8 before retainDefault14D590, and retains the canonical table. clone26EB44 retains the same DefaultMT association. No duplicate template owner is needed. RA still needs the genuine model-resource/caller join and exact pass admission; existing ScreenObject-only animation admission is intentionally untouched.
