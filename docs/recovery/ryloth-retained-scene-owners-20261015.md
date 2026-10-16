# Retained190/198/194 SceneObject prerequisite — UNTESTED

Base461d8b7. Root owns all integration and testing. No builds, tests, syntax checks, sanitizers, converters or gameplay ran.

## Connected implementation

The chariot's actual retained construction now owns three distinct native2893A0 allocations for190,198 and194.198 is included because its211808 Entity append intervenes between2117B8 and211914. These are actual SceneObjects with embedded Entity8 references, shared Application423F60 increments and383408 type registration. The existing private2893A0 implementation is shared without changing its stores. Host allocation/type failures retain earlier constructed owners for bounded cleanup.

The source object's Entity table3D3198+30 contains141C20. Its two words are03E00008 and70801628: identity return, no initialization stores. The concrete call returns this actual Entity8; it does not mark activity, allocate effects, or fabricate success of an overriding virtual method.

Construction happens while the canonical Rig nodes are fresh, before chariot normalization/motion/placement. The same bound chariot owner calls the fresh-child28BEF0 case at211A34 immediately after its existing actual canonical2F0/854/858 publication. Only main190's firstChild184 and container194's parent180 change. Rig subsequently preserves these actual objects instead of replacing their matrices/flags/hierarchy with previous numeric snapshots. Physical19C remains on the existing numeric path.

Session publishes borrowed aliases to the same SceneObjects and appends Entity8 for190,198,194 with mask2 before appending the chariot animation. The actual entity domain includes these objects for subsequent model operations and close. Partial append failure retains the reached sibling prefix. Later chariot close repairs its current parent list, then closes194,198,190 before any earlier engine owner can close. The fresh scene parent is removed when194 closes: the shared28BFC0 prefix first marks194 and ancestor190 dirty80 and invalidates194 ready01/subtree while the parent links still exist, then the admitted leaf is unlinked. References/counter contributions retire once. Unexpected extra graph descendants, observers or topology stop cleanup instead of inventing another owner's destructor.

## Explicit staging and remaining limitations

Existing native resource/material/Session staging still applies. Scene constructors and slot30 identity calls run early; racer aliases and Entity append run at Session join. This does not assert full retail210770 chronology, including allocation/publication interleaving. Earlier writes survive failure. The engine-present194 branch alone is represented; no-engine194 is a different animation/model owner and remains unadmitted. Source-null allocation followed by its unconditional virtual dereference is reported as a native allocation boundary, never executed as a guest crash.

The numeric main/hidden/container whole-node snapshot overwrite is intentionally removed for concrete retained callers. After construction and motion setup, the existing native caller's saved main LOCAL placement is applied with setLocalMatrix49P and refreshSceneWorld49P, before f::project and placeColdHandlingRoots consume main.world100.2893A0 itself correctly leaves world100 zero; that unrefreshed cache must not become the physics root. Only the existing caller pose is transported. Concrete object identity, hierarchy and flags remain owned by the constructor and canonical setter/cache effects; no old whole-node snapshot or synthetic original placement claim is introduced. Generic callers retain the prior numeric setup. Additional runtime graph children are not admitted by this bounded cold cleanup. Central validation remains required.

Optional208/20C separate allocation/lifetimes, full360 Entity/slot30/append, effects/virtualA0/2146C0, bounds update caller and late-strip completion remain missing. Original AI, collision gates and RA activation are unchanged. No substituted input/course driver, replacement collision, guest execution/emulation, guest commands or disc edits.

## Source evidence

Original SLUS_202.68 sha256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Packet includes exact static words and hashes for21176C..21191C (three allocation/constructor/slot30/append sequences),211A20..211A40,2893A0..2896D0,3D3198..3D31D8,141C20..141C28,28BEF0..28BFC0 and2896D0..2897B0. Exact base preimages accompany the patch.
