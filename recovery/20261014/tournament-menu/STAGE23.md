# Stage 23: actual UI SearchList model and binder traversal

Correction recorded by stage 24: selected-list insertion tests member bit 1,
not bit 3. Constructor 27E130 sets bit 0 and clears bits 1/2 using literal
zero operands, not prior return-register values. The bounded binder remains
unselected and draws through the selected model's attached-child continuation.
The affected interpretations below are historical and superseded by STAGE24.md.

Status: source recovery only. No production edits, tests, builds, syntax checks, conversion, launch, gameplay, or guest execution. SLUS_202.68 was read only as bytes from the permitted disc directory. Frontend admission remains closed. This does not claim a complete Tournament composer.

## Ownership

Only recovery/20261014/tournament-menu/STAGE23.md and stage23-search-model-words.txt are owned in this stage. Shared frontend/session integration, capture route headers, renderer dispatch, profile/result writeback, and all validation remain with root. No Rider identity or retirement receipt is repurposed for Tournament.

## Proven search owner

UI manager initialization at 1CCD14..1CCD78 constructs SearchList 27E260 with capacity 0x400, stores it at manager+2A0, constructs zone 27F120 at manager+2A4, and assigns this SAME SearchList to zone+34 and +38 through 27F420/27F490. Those assignments release and retain actual references. This is not the Options background grid. 27F650 therefore calls this search once when the two pointers still alias. The zone initially has null portal/neighbor fields, but future absence cannot be assumed without owning mutations.

27E260 installs vtable 3D2D30; virtual44 is 27E880. It visits allocation blocks in order, full blocks with count pointer +30 and the last block with count pointer +38. 27EAF0 resets selected list +18 for EACH block. It skips free slots marked bit1, strips slot bit0, requires member+3 bit0 and rejects member+3 bit2. Type tags come from the actual registry lookup of SoultreeObject and SearchableGameObject (3BBFC0/3BBFD0), not invented constants.

For SoultreeObject: require member+31 bit0, call Entity(member+8) virtual40 through member+10, then append Entity only if member+191 bit0 is set. For SearchableGameObject: Entity=member-30, require Entity+29 bit0, call Entity virtual40, then append only if member+3 bit3 is set. Callback invocation and selected-list insertion are separate decisions. Unknown tags are ignored. Slot count is reread after each callback. After EACH block, 27EAF0 invokes actual global 442F70 renderer virtual20 with argument zero (244D60). There is no per-member Entity141C30 renderer prelude here.

## Model dispatch

Model constructor 2403A0 installs 3D1F48 at model+10. Its virtual40 entry is 2418F0, which subtracts eight and jumps to 26EFC0. Disabled model+3 bit2 clears selected bit191 and returns one. Otherwise actual model virtualBC (240C00) performs selection; selected models enter 26F070, and actual Application382B80+278 is incremented. 26F070 resolves actual LOD/geometry and invokes selected packet virtual10 with the model material token and computed fog/distance values. It does not traverse binder children. Reusing an inherited Entity walk in place of this dispatch is incorrect.

## Independent binder registration

DynamicUserRotate init1D31D0 retains the actual zone argument at widget+254; the factory supplies manager+2A4. After pose setup, 1D3690 calls 28E8A0(model,zone,1), before binder construction. 28E8A0 creates observer at model+19C, obtains actual bound vectors via 28E230, then invokes 2809C0(observer,model,zone,flags1,vectors,zero radius argument).

Binder init1D3D80 receives the same widget+254 zone. It owns primitive+38 and observer+3C. At 1D3F5C it calls 2809C0(observer,binder+30,zone,flags2,actual zero vectors,zero radius). 27F590 maps flags1 to zone+34 and flags2 to zone+38. Thus model and binders register into the same actual list. Binder graph attachment mask100A beneath model+8 does not explain callback reachability; independent search registration does. After both binder setups, 1D37D4 calls actual widget parent virtualA4, so material synchronization cannot be moved before those registrations.

27E130 establishes the searchable subobject at Entity+30, looks up SearchableGameObject at 3BBE60, and stores its registry tag at Entity+32. Its flag stores preserve untouched bits; bit3 is not established by the captured constructor. Do not infer selected-list admission or blanket-clear flag bytes. The raw sequence includes return-register-derived bit expressions; reproduce their actual inputs rather than substituting a guessed constant.

Search allocation 27E3C0 uses existing keys, then free-list slots, then sequential block allocation. 27E570 inserts the actual member, unlinking a free slot or advancing used count. 27E5B0 links removed slots into the free list using bit1. Holes/reuse order are observable; a compact vector is not a faithful substitute.

Binder virtual40=1D3F90 uses actual endpoints, active camera and primitive material token; missing endpoints return zero. Its canonical298E60/29BFF0 floor and inherited child walk are documented in stage22. Search callback and selected-list bit3 must remain distinct.

## Removal ownership

Model unload1D3800 orders actual renderer virtual20(1), renderer virtual24, actual model Entity virtual1C, 28B520(model,widget+210), 28EA80(model), 141A70(Entity), Entity virtual10, then clears widget+60. 28EA80 removes model observer registrations through 281490(observer,model), destroys observer through 280920(observer,1), and clears model+19C.

Binder destructor1D3CD0 similarly calls 281490(observer,binder+30), then 280920(observer,1), clears observer+3C and continues base destruction. 281490 removes actual primary and linked search registrations through search virtual2C=27E5B0. Observer destruction updates real shared observer count/pool lifetime. This proves the registration/removal source pair; it does not grant a new host owner authority to mint retirement receipts.

## Remaining requirements before production traversal/composer

1. A genuine Tournament issuer must own actual SearchList slots, registry tags, selected-list resets and insertion, observer lifetime, zone state, and mutation order. Current Rider visibility bind rejects Tournament archive/widget identities and only represents registered models, not this full typed search.
2. Exact actual material/packet callback population and post-load parent virtualA4 synchronization must be connected to canonical Tournament occurrences/history. Rider pod material footprint explicitly gates RiderWidgets.Dat; passing Tournament as Rider would fabricate provenance. Binder token ownership also needs a genuine Tournament issuer.
3. Actual shared renderer fields, queue/callback domain and retirement entitlement must be supplied to serviceOriginalRenderer244D60; initial/default fields are not a valid lease. Source invokes it at both inherited Entity boundaries and per-search-block boundaries.
4. Camera portal/neighbor mutations, binder selected-list bit3 source, selected-list virtual48 consumers, and callback mutation behavior must be bounded by actual owned state. The recovered straight-line dispatch alone is not a complete draw composer.
5. Mode2/career-selected-profile and actual result owner remain root dependencies; no unlock, manufactured win, or fabricated result is introduced.

Evidence: companion raw-word file; VA maps to file offset VA-100000+80. All addresses above are manually interpreted static data. Root owns integration and all software validation.
