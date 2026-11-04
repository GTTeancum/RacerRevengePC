# Course Control1: Widget9 byte provenance

This is a source-only finding, not a source-progress or readiness patch. The existing2A71D4 stop is retained. No runtime, emulation, build, syntax check, test or program probe was performed.

## Actual instance and read

Course Widget9 is source table index8, the first Soultree_Character using holo_ta. Factory1CD320 allocates0x3B0 bytes;1CD338 invokes2A85F0, and the virtual loader receives the unadjusted same instance.2A8770 returns that instance at2A8B70. The factory and source table publication do not adjust it to a different interface/subobject.

Control1's WidgetID1 is9. Caller1DA318..1DA338 indexes manager154 by ID−1 and stores the actual widget pointer in Control1's first borrowed slot.1DA344 passes that pointer unchanged to router2A71A0 at1DA350. The current implementation therefore correctly retains Widget9 before entering the router; a native projection or different widget cannot replace it.

Router2A71D4 loads from widget+50, then advances its widget and list offsets by4 for8 iterations. The source read range is+50..+6C, not an eight-bit mask. Zero skips insertion; nonzero inserts the actual widget in the corresponding list. The initial three values+50,+54,+58 are knownzero for this exact cohort. The next read is+5C, lane3, where current source correctly stops.

## Known producers and untouched bytes

Base constructor2A7760 calls Entity141300, stores the manager30/type34 and zeroes fields38..58. Its last zero store is2A77B4 at+58. There is no constructor store to+5C.

Derived2A85F0 installs vtable3D41E0, writes animation60=null at2A8628, initializes vectors70/80/90 and fields3A0/3A4. It does not initialize+5C,+64,+68 or+6C. The later actual animation constructor/load publishes a real pointer to+60 at2A8B10. Successful owner construction proves that pointer is nonnull; its numeric guest address must not be replaced by a host address.

The generic post-factory getter loop1CD8C0..1CD8FC writes exactly two input fields+50/+54. The actual source1CD908 Disable and1CD9F8 Attach operate on the entity/attachment state. Their successful native analogues do not initialize padding. The model observer attaches to the animation model's observer19C; it is not a write to the outer Soultree+5C.

The supplied independent writer audit scopes the earlier callbacks and aliases it examined. No reached same-instance producer proving+5C has been found. This is not a claim that an arbitrary whole-program offset5C store targets this object, nor an exhaustive proof over unrelated unreachable code.

## Allocator does not supply a default

The outer allocation is2E8D90→305700→305990. The operator-new and malloc wrappers do not clear the payload. The allocator rounds request0x3B0 to a0x3C0 chunk and updates chunk/bin metadata. Its successful return is chunk+8; those operations do not establish zero bytes at object+5C or the later padding words. Free-list and new-arena branches require their actual byte history. A missing payload initializer is not evidence of zero.

For this allocation, the first blocked four bytes correspond to chunk+64..67. Later padding reads would need chunk+6C..77. The known animation pointer lies between them at object+60. A full continuation must preserve the actual byte provenance and list insertion effects for every lane, rather than skip the padding or pretend every later word is zero.

Host allocation, value initialization, source table capacity, renderer readiness, valid assets, a successful animation load and current FileManager mode do not establish those historical source bytes. There is no caller-supplied completion or value setter that can validly fill the gap.

## Consequence and next work

No meaningful source-PC advance is justified by the evidence presently retained. The packet changes documentation only and preserves the existing unknown-field stop. It does not manufacture Control1, Page, manager-loader success/failure, FileManager mode restoration or music readiness.

A faithful continuation needs an authentic same-instance earlier write or an original allocator/arena history proving the observed bytes. If a deliberate portability behavior is chosen instead, it must be described as an adaptation, not original initialization or evidence that the loader returned. Neither path is implemented by this packet.

The prior corrected Course chronology remains applicable: real mode restoration is loader1CCE94 or genuine failure1CCEF8 after their complete predecessors;1872B8 is startup POD-ability loading, not UICourse destruction. The new finding validates why the current Control1 boundary cannot authorize those suffixes.
