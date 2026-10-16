# Portal render entry: bounded static-source recovery

This is source inspection, not executable validation or a gameplay result.
No original executable, guest instruction stream or emulation was run. The
existing authenticated original file was read as data; implementation remains
native host code. Windows is idle after the Ben/GA pass.

## Caller argument identity now established

The previously retained 96-byte range 0x114CE0–0x114D40 has SHA-256
774696d122c1c28582fc7fcaff4d02306988070538c245cf225587df62c3258e.
At entry 0x114D00, the incoming scene owner is retained in s0. The body:

1. Reads the global word at 0x36ADA8 (0x00370000 plus signed -21080).
   Zero skips portal rendering. Its retained initial image word is 1.
2. Loads a0 from the scene's +0x38 parent field.
3. The branch-likely at 0x114D20 skips the portal call if that parent is null.
   Its a0=s0 delay-slot assignment occurs only on the taken null-parent path.
4. On the nonnull-parent path, 0x114D28 calls 0x27F650. Its delay slot copies
   a0 to a1. Both arguments therefore identify the SAME actual parent zone.
5. 0x114D30 restores a0=s0 and calls the ordinary base 0x141C30 continuation.

Consequently the first-hop exclusion array at second-argument +0x3C and count
+0x40 belong to the starting zone. They are not a separate camera exclusion
owner. This closes the immediate call argument gap in the earlier owner map.
The existing native OriginalCameraSceneOwner parent38 is the appropriate
provenance-bearing field, once the actual render phase is joined.

## Target-only reference query

The authenticated merged source section SHA-256 remains
852dbe271b6b1caf2758e243ff54df11487fdc783f3a5b4134275377a72906f4.
A static target-only query for 0x114D00 found literal pointers at:

- 0x3CCA40
- 0x3D1A30
- 0x3D1C50

No direct J/JAL or bounded literal high/low candidate was found. This result
is syntactic, not a proof that computed references cannot exist. The three
bounded surrounding tables share the base camera update/render methods;
0x3D1C48 contains the previously recovered game-camera update 0x2363B0.
Table boundaries and application render invocation chronology still require
constructor/call-site evidence. Nearby strings are not used to assign classes.

## Still required before gameplay integration

- Actual application render invocation and render-gate mutation/lifetime.
  The initial image word is known, but a bounded literal reference query cannot
  exclude computed writes or establish all application configuration paths.
- Native admission of the now identified authored exclusion-array construction
  and field parsing, including an explicitly bounded numeric-token domain.
- Application admission/dispatch generation lifetime and zone stamp reset.
- Real zone +0x34/+0x38 method dispatch and default-zone +0x44/0x443108 owners.
- Stable viewport fields, per-zone matrix publication and actual model dispatch.

The existing projection/intersection leaves and camera membership do not by
themselves satisfy these owners. No all-zones-visible substitute, host frame
counter, synthetic membership, or draw-completion receipt is introduced.
No code-path admission or performance claim is changed by this document.

## Base child continuation

The bounded 0x141C30–0x141D30 source range has SHA-256
36225a00b901413e3a3ccec20095f9d2b9e65caeecb9d15669636cbc702cb669.
The complete 0x141C30 body reaches its return at 0x141CD0:

- Parent word +0x24 must contain 0x1000; otherwise return 1.
- Visit children from +0x18 through each child's +0x14 link, in stored order.
- Child byte +0x29 bit0 must be set and bit3 clear; child word +0x20 must
  contain 0x1000. The two 64-bit shift words implement the bit3 gate here.
- Before each admitted child, invoke the actual renderer object's table +0x20
  with argument zero, then invoke the child's table +0x40.
- Return 1 after the list, including an empty list.

The camera entry is present at +0x40 relative to table starts 0x3CCA00,
0x3D19F0 and 0x3D1C10 (their first two words precede method pointers). This
is consistent with the child continuation's exact dispatch slot. It still does
not establish the top-level application invocation or all method implementations.
The native join must preserve the real enabled/hidden/method gates and renderer
callback order; an unconditional camera call per host swap is not equivalent.

## Zone construction and exclusion copy

The scene-creation slice 0x272900–0x272B20 (SHA-256
71a56e9950872f652fa791aa40b3bb35e5f33364034ae8f9024a6198a8639cc6)
identifies the actual zone constructor 0x27F120. It creates distinct 0xB0-byte
objects for both 4C0 and 4C4 and writes their zero-based ordinal at +0x48.
For a positive signed count from the scene's per-zone record +0x124, it invokes
0x27F500 on the 4C0 zone with the record's +0x120 array and that count.
The per-zone source record stride is 0x160. The authored parser for these two
record fields is identified in the section below.

Constructor 0x27F120–0x27F174 initializes incident links +0x30, both method
owners +0x34/+0x38, ordinal +0x48, exclusions +0x3C/+0x40, fallback control
+0x44 and auxiliary +0xA0 to zero after the base constructor. Critically, it
DOES NOT initialize render stamps +0xA4/+0xA6 or bounds/matrix in this body.
A host zero initialization cannot be described as an observed source store.
The retained constructor/destructor range 0x27F120–0x27F280 has SHA-256
6792541efc80df71ceb3c1cd0b8bac97221292b366230aec88c77be77bbe48ed.

The exact exclusion setter 0x27F500–0x27F590 (SHA-256
73ceaae26f9da4affac223eb9a33d178e9a1aa2e9b1e0ed571704ffc161909e5)
releases the previous array, stores the new count, allocates count times four
bytes, and copies the input words in order. It does not sort or deduplicate.
The native implementation may use owned storage, but must retain these actual
values and the first-hop-only consumer scope.

The source allocation wrapper 0x2E8D90 delegates to 0x305700 and retries through
its failure policy; no zeroing is present in that wrapper. This observation does
not establish the allocation payload contents or the lower allocator's policy.

## Additional shared-counter users

A static query for exact GP-relative access words found dispatch-counter writes
at 0x27FF28 and 0x2DEC44 in addition to the already understood 0x27F840.
Their containing operations must be reconciled before assigning an application
counter lifetime or claiming one increment per frame. There was no extra
admission-counter store in that syntactic query. Computed or bulk stores are
outside its scope, so it is not proof of globally exclusive mutation.

## Authored exclusion field identified

The zone metadata constructor 0x275C50 installs table 0x3D2B90; its reader
slot +8 resolves to 0x275E30. The scene loader invokes that reader for each
0x160-byte record with the original one-based zone number. Within that reader,
0x27632C–0x27641C reads the field named `ExclusionList` with an empty default
and a 128-byte destination. The exact retained label at 0x3BA7E8 is
`ExclusionList`; delimiter data at 0x3BA7F8 is comma followed by newline.

The reader resets count +0x124, counts tokens using 0x29FFA0, allocates the
array at +0x120, then converts each token through 0x302E30 and subtracts one
before storing. Thus authored labels are one-based and the array consumed by
0x2806B0 contains zero-based zone ordinals. Order and duplicates survive.
Malformed numeric-token and delimiter edge behavior must be preserved or
explicitly rejected by the native admission domain, rather than silently
normalized. Tokenizer 0x29FFA0 is a stateful source helper, not assumed strtok.
Its complete tail and integer wrapper are reviewed below; no native parser
was added without a consumer or admitted source domain.

Reader range 0x276300–0x276470 SHA-256:
07fdc304e31f3a8f9184b8ae5c66d356ecfe11ef1035829c26a6986df9abfc92.
No cache format or native world selection was modified during this recovery.


## Separate camera draw phase and recursive exclusion scope

A target-only reference query for 0x27FEE0 finds calls at 0x114E00 and its
recursive edge 0x27FF94. The former belongs to camera method 0x114D50,
which occupies slot +0x48 in the same retained tables whose slot +0x40 is
0x114D00. These are distinct methods, not one interchangeable render callback.
Both pass the actual camera parent38 as BOTH zone and exclusion owner.

The complete 0x114D50 body through 0x114ED8 uses the same 0x36ADA8 gate.
If disabled, it returns 1 without its base continuation. If enabled, it has
renderer mode/flush calls controlled by 0x435600, calls 0x27FEE0 only when
parent38 exists, conditionally calls 0x24B7B0 on 0x443848, and finally calls
0x141D70 on the camera. These renderer/effect owners are not assumed empty.

0x27FEE0 is a later, separately stamped traversal:

- Increment shared recursion depth 0x443120; only depth 1 increments the
  shared unsigned halfword dispatch generation gp8BE4.
- Assign this zone A6; call 0x280230(zone, zone.A0) and publish its result A0.
- Walk authored adjacency order. Require destination A4 equal gp8BE0 and
  destination A6 unequal gp8BE4 before checking its ordinal against the
  ORIGINAL second-argument exclusions.
- The recursive call preserves that SAME second argument. Thus exclusions
  here apply throughout this traversal. This differs from the first-edge-only
  exclusion scope within 0x27F650's admission walk. Do not conflate the phases.
- After returning from an admitted child, optional 0x443110 behavior calls
  link+8's table slot +0x48. Its concrete semantics remain to be joined.
- At 0x28011C, call zone method34 slot +0x48 when nonnull, then method38
  slot +0x48 when nonnull and different. A nonzero 0x443118 can suppress this
  own-zone block unless equal to current recursion depth, without undoing the
  already performed child traversal.
- At outermost depth only, if global 0x443108 exists and its A4 equals the
  admission generation, call ONLY its method34 slot +0x48. This later phase's
  condition is deliberately different from the 0x27F650 fallback admission.
- Decrement shared depth and return 1. No counter reset sweep was found in
  this complete body; application initialization is still a separate question.

The base continuation 0x141D70 uses root24/child20 mask 8, child29 enabled
bit0 and hidden bit3, then the renderer mode-zero prelude and child slot
+0x48. It reads next14 after the child callback. Its gates are distinct from
0x141C30's mask 0x1000 and slot +0x40.

New retained source slices (SHA-256):

- 0x114D40–0x114E70: 7dfa8b98ef6add37a27497e05fa50a8c267b3f1c881475adde3e18ad2732a9fd
- 0x114E70–0x114F00: 6ce24df874b5d5198559605ca9667017f343c7ef7d835bd4c7b6d0764de28d14
- 0x280010–0x280230: ef4272d595ff2292778facd2ca4b4ae5d6f791f1e997a9c8b0e9abf1113d5eaf
- 0x141D70–0x141E20: 665af30cb3f1e7b82a029047edb63ac336101d9ec1c35b9fe0a36123f64b9ee4

This closes the additional 0x27FEE0 caller and local ordering, not the
application's ordering of the two camera methods or the concrete registry
callbacks. No native rendering change, executable test, or extra Windows
launch occurred during this static-source work.

## Tokenizer tail and decimal conversion boundary

The complete tokenizer tail returns the stored token start after skipping
following delimiter characters. It does not skip a leading delimiter before
selecting the first token. Empty input initializes an exhausted cursor; a
leading delimiter can yield an empty first token. The native reader must not
silently replace this with ordinary strtok semantics.

0x302E30 passes a null end pointer and base 10 to 0x30AE70, which forwards
through the shared library context at 0x38544C to 0x30AC38. The wrapper returns
the signed low 32 bits of that result. The bounded conversion body has character
classification, sign, digit accumulation and overflow branches. Consequently a
host std::stoi/full-token validation cannot be described as the exact original
behavior. A future strict positive-zone subset may reject unsupported tokens
explicitly instead of reproducing unrelated locale/error/overflow machinery.
No malformed-token policy is introduced by this inspection.

Retained SHA-256:

- 0x302E30–0x302E90: 6547a28d189cefbf70a56d794220e9913179e1e79e7ccb1f0a4837956ae3ae2e
- 0x30AE70–0x30AEC0: 508c092d66b5acd5efc3e7b7e79629086c6d5ac1c89bcd446cd8184979e0433f
- 0x30AC38–0x30AE70: 97b4c22ccc2b8695f5df35298e5f022ba69b38cddc6183001e7a2cf0c5aadc67

## Application root phase sites located

A bounded literal method-slot query over authenticated 0x13F000–0x141300
(SHA-256 85f4a1a43c8a95b22cd1e77e87d3f8350f5b51ed5c0c8dc6b235dc083e365558)
located the actual application root dispatch sites. The retained bodies show:

- 0x1404CC sets application +0x22C to 3, calls application root26C's table
  slot +0x40, then clears +0x22C at 0x1404E4.
- 0x140728 sets +0x22C to 4, calls root26C slot +0x44, then clears it.
- Following the renderer slot +0x0C success gate and application table +0x24
  callback, 0x140778 sets +0x22C to 5, calls root26C slot +0x48, then clears it.
- The containing loop uses application +0x0C as an index over four +0x254
  entries. Null entries skip the body. Each nonnull entry is supplied to
  0x26F940 before these root phases; its concrete ownership must be retained.
  Thus source ordering is per reached entry, not automatically once per swap.
- After this loop, the +0x22C=6 phase calls root26C slot +0x4C.

These are real application dispatch sites, but the full reached root/camera
attachment chain and the intervening +0x44 method are not yet natively joined.
The selected camera can neither be assumed the only child nor substituted for
the real application26C traversal. Renderer gates and callbacks between the
phase sites cannot be dropped merely because the roots' slot order is known.

Retained ranges and SHA-256:

- 0x1402C0–0x140470: cd6cc0924b6e8b4228dd00e482be13ee1941c2900ddc5bf8ae1be7ff50c5f185
- 0x140470–0x140550: 557c176a388c8aaa24c5c8d2a75936508ea951e83877cad5393c27568a20f60c
- 0x1406E0–0x1407E0: 4503e284205bbac0cdd6ac4ca87371d2a2b9cc5075884c1b5f4074b8ab48cf49
- 0x1407D8–0x140850: f659da79f29b4834e2e02acd1bee833fb72ac27d22bf69ac8a4f3a624cd7f2c3

The separate literal query for 0x36ADA8 finds only the two already recovered
camera-method loads, no direct stores or address-valued pointer candidates.
That narrows the observed gate mutations; it does not prove computed or bulk
writes impossible. The retained image value remains 1.

## Fallback zone is a distinct environment-node owner

The direct-store query for global 0x443108 located its publication at 0x2730F8
and teardown clearing at 0x26FF54. In 0x272FC0, the source creates the shared
0x443A98 method owner using constructor 0x2AEF70 when absent, then configures
it from scene metadata bounds and a 5000 capacity request. It independently
allocates a 0xB0 zone with 0x27F120 when 0x443108 is absent, sets its ordinal48
to zero, and attaches it to the scene's real entity4BC.

The SAME 0x443A98 owner is installed through both 0x27F420 (method34) and
0x27F490 (method38). Each setter releases the old method if present, sets
incoming method10 to 1 or 2 respectively, publishes the pointer, and retains
it. The equal-method skip in traversal is therefore meaningful. This global
zone is not simply authored zone1, despite having the same numeric ordinal.
It is also not the separate membership fallback443148 initialized just before
this function from collision4C4's first zone.

The actual enabling field is now traced: 0x2741AC reads per-zone metadata
byte148, extracts bit1, and writes the resulting 0/1 into that actual 4C0
zone44 at 0x2741C0. The metadata constructor sets bit1. The reader at
0x275EE4 calls 0x158B30 with label `EnvironmentNodeVisible` (0x3BA630),
default 1, and stores the result's LOW BIT into metadata148 bit1. Thus a
native generic nonzero-to-true conversion would differ for values such as 2.
The full native authoring domain must preserve or explicitly reject such
values. The scene3C4 gate and this construction chronology remain significant;
merely parsing the flag does not instantiate the environment method owner.

Source hashes:

- 0x272EF0–0x273030: c6724293b53facce093de172e18d63100d5abde0d7fe43750720da75ad6bfbc3
- 0x273030–0x273150: adcc68dfb8cb5de0958a39ca5c8a4242b55485ede9e4f69559eb5039126386e3
- 0x26FF20–0x26FF80: 9d6cc1a02cbf40db22302f14655c775d59ffa289b9f8038ea64890c798888503
- 0x27F420–0x27F500: 32d229e1ba2fb3c7e5e3c723d429d33e2259317281e4accbfc599ff8d0aa344e
- 0x274120–0x274210: a1a210fb563c3397e090795ccab20efb099a33c7c409e1823b8151fbb665d0b7
- 0x275D40–0x275DB0: af13d66ee2ae11fcca7f84e5967595b0ac4b08952dcfcb47ed5f4a8a8e7db551
- 0x275EB0–0x275F80: bdd05ee0b6f8e17c47fe614b0b0da4a469c8e45a8b50186826aa8af6f42e03fb
- 32 bytes at 0x3BA630: f2f9aa081b61faa2754096954fe493689dc2506fa0dc306796790b6fae4d08f9

## Shared generation initialization

Startup 0x100034–0x10005C establishes GP as 0x3DD970. Therefore gp8BE0 and
gp8BE4 identify 0x3D6550 and 0x3D6554. Both lie within the explicit startup
zero-fill region beginning at 0x3D5A00 and extending through 0x44F600. Their
initial zero values are now source-backed, as is zero initial recursion depth
0x443120. This establishes process-start state, not a per-frame/per-race reset
or a dynamic zone allocation's stamp contents. No wrap-reset sweep is inferred.

Startup 0x100000–0x100080 SHA-256:
37e05babb48c6971d4870c7b6ad67fa57c5c1268cf6610aa4d08d07fc94ad27d.

## Native integration sequence from this recovery

The next gameplay change should consume real authored records before adding
render admission. Keep the current passing renderer intact until each join has
its actual owner:

1. Read `ExclusionList` and `EnvironmentNodeVisible` from the already retained
   World48F parameter blocks. Keep authored order and duplicates. Establish the
   concrete current GA values before narrowing the numeric/token domain or
   changing cold preparation. Do not change RRWORLD1's wire format for fields
   already retained in its parameter document.
2. Put per-zone render fields beside the existing live 4C0 zones in the same
   Owner storage, without enlarging or aliasing collision4C4. The fallback
   environment zone must have independent identity/storage outside the authored
   ordinal table. A stamp address must remain stable for the existing wreck
   observer binding API across leases and frame publications.
3. Keep admission generation, dispatch generation and recursive depth in the
   application lifetime, outside camera/racer/track snapshots. Startup zeros
   are proven; dynamic stamp initialization and wrap behavior still require an
   explicit native admission decision. A stale or missing traversal cannot
   publish all-zone visibility or a draw-completion receipt.
4. Join the real current-camera selection and phase3 root traversal before
   source slot44 preparation and phase5 draw traversal. Use the current camera
   matrices, including source-ordered range changes; a camera pose revision
   alone is insufficient after fog changes. Existing selectOriginalRaceScene26F940
   and OriginalRaceCameraDisplayFields supply real native owner identities,
   but the full root attachment chain remains a distinct integration boundary.
5. Bind actual static/dynamic method owners and draw objects. The current
   racer-plus-camera registration capacity cannot represent every world model
   or the global environment method by renaming draw spans. Preserve phase40
   slot44 callbacks separately from phase48 slot48 callbacks and their distinct
   exclusion/fallback rules.
6. Only then let admitted original zones affect native world/racer submission
   and wreck visibility. Keep the existing optional host AABB culler labelled
   separately. Preserve every required map-2 feedback barrier and source draw
   order. Validate centrally in the next eligible Windows smoke; no standalone
   helper tests or routine extra launches are authorized.

Current checkpoint: documentation/source recovery only, cloud commit lineage
through this note; Windows runtime remains the verified 34b352f Ben/GA build.
The GA.SCB upload request is separately pending. No transfer was performed
following its rejection, and no test slot was consumed by this investigation.

## Authenticated GA authoring recovered without a new transfer (14:50 UTC)

The already-uploaded October 12 GA.RES supplies the exact SCB required above.
Archive SHA-256: 063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9.
Member tracks\\ga.scb is 292650 bytes at logical offset 73728; its SHA-256 is
ad38b0d47efdda92e36bf36f9b380f7f3111698cb068532068a19229097de5d7.
The previous pending upload question is withdrawn. No new Windows transfer was
performed. Reading this existing source asset consumed no software test slot.

All twelve zones author SearchType=List and omit EnvironmentNodeVisible, so the
original default of one applies. Exact ExclusionList strings, one-based labels:

- Zone 1: `3,4,5,6,7,9,10,11`
- Zone 2: `4,5,6,7,8,9,10,11`
- Zone 3: `1,5,6,7,8,9,10,11,12,`
- Zone 4: `1,2,6,7,8,11,12`
- Zone 5: `1,2,3,8,11,12`
- Zone 6: `1,2,3,4,10,12`
- Zone 7: `2,3,4,5,9,10,11,12,`
- Zone 8: `2,3,4,5,6,9,10,11,12`
- Zones 9–12: field absent, original default empty

### Cold owner integration, not yet built or tested

The same existing live 4C0 Owner now cold-prepares immutable AuthoredRenderZone
rows from its authenticated GA World48F parameter document. It retains order
and duplicates, subtracts one from each positive decimal zone label, and handles
the real single trailing delimiter in zones 3 and 7 without an empty token.
The admitted parser domain is explicitly narrower than the original general
integer conversion: decimal labels in the actual zone range, comma/newline
separators, less than 128 bytes, and no unseen EnvironmentNodeVisible override.
Duplicate ExclusionList fields and unsupported forms reject cold preparation.
Output publication remains transactional with the rest of Owner::prepareScene.

A current CameraVisibilityLease can borrow each corresponding immutable row.
Other scenes return null; they are not incorrectly represented by empty GA
rules. No admission stamps, draw suppression, wreck visibility publication,
application generation resets, fallback environment identity or performance
claims are introduced by this change. The next join remains the genuine render
phase owner and complete static/dynamic method registration.

Windows remains on 34b352f. No new build, syntax check or runtime test was run.

## Root attachment constraints (14:58 UTC source inspection)

The original application root is not an alias for the active camera. Existing
native OriginalApplicationEntities50LA already constructs the authentic base
26C -> 10 pair, but deliberately represents no complete race forest. Its
teardown rejects unrecovered children, so attaching a camera there solely to
make a render callback reachable would violate that owner's lifetime contract.

New literal source reads establish these additional edges:

- 0x14006C appends global443870 to application10 with mask -1 after construction
  through 0x253D80 and its real slot30 call. 0x140094 appends the independently
  constructed 0x147ED0 object with mask 0x402. 0x140108 appends application0 to
  application10 with mask -1, after the preceding application callbacks succeed.
- Race attachment body 0x1AAD40 appends its actual scene34 at 0x1AAF6C with
  mask -1, followed by optional UI44. The real scene34 thus has render eligibility
  through this attachment; it is not the controller48 object.
- 0x1AB1E8 appends controller48 with mask 2. Subsequent controller58 and admitted
  additional controllers4C/50/54 use mask 2 as well. These attachments do not
  select the base render walkers' mask0x1000 or mask8. Controller update ordering
  therefore cannot substitute for scene render traversal.
- The same body has many earlier world/service/UI attachments and conditional
  later attachments. A compact list containing just scene34/controller48 is
  not a recovered full root. The application0-to-race ancestor link and each
  reached concrete method still need their own join.

This is source inspection only. No root attachment was changed in native code,
no callbacks were silently treated as no-ops, and no game was launched.

Authenticated range SHA-256:
- 0x140000–0x140130: d298295b96360a87bd7ddbf4900bb012972b5b9ed687c0c5bb5f4db70fd29d6c
- 0x1AAD40–0x1AAF30: 210e33e66762607d06da2343f719ca2358b1aec7591836314c43afbe13feaacc
- 0x1AAF30–0x1AB3A0: 3a9ad7bfddd1f3f6d9cab6cbbe82805c06c9e613626792ce5a40eef711aa4dac

## Zone viewport matrix 280720 recovered (15:00 UTC)

280700 supplies bounds4C/50/54/58 and destination60 to 280720. The latter reads
the actual current camera from renderer442F70+0, uses its FOV1D0, aspect1F8,
near1FC, far200 and view matrixC0. It computes tan(FOV * raw0x3C0EFA34), then
near*tan, then aspect times that result for the vertical extent. Each min/max
bound scales its corresponding extent. 14DAF0 constructs the off-axis matrix;
2807D4..280860 multiplies the actual view by that projection and publishes it.

Notably 14DAF0's depth scale is (far+near)/(far-near), with translation
((-2*far)*near)/(far-near). Camera14DB90 has different depth terms. Reusing the
existing core projection would therefore be an incorrect replacement even at
full-screen bounds. The retained native camera unit now implements the bounded
composePortalZoneView280720 leaf using its existing native tangent and matrix
multiplication, strict separate binary32 arithmetic, transactional publication,
and explicit finite/nondegenerate input admission. It does not run source
instructions or create an emulated machine. Zero-area or invalid input returns
failure, never a successful culling result.

This leaf is not yet called by gameplay and has not been built or tested. It
provides the required matrix operation for the still-unjoined render traversal;
no original visibility or performance result is claimed.

Range SHA-256:
- 0x280700–0x2808D0: 00caf3a7ed6dc697e45933ccef6be9404bc228377270c8872accb29853195476
- 0x14DAF0–0x14DB90: 0bd9a2aa26557c6bd4338b022af57091c78b072bd60e1a1b86ddc5d83db1695c

### Shared real UI consumer (15:02 UTC)

The existing Rider/Controls membership camera already performed the same
full-viewport operation. Its wrapper now calls the shared 280720 helper and
copies both projection and matrix60, retaining its actual page identity,
full-viewport ownership and separate guard/clip owner. Manual source comparison
confirmed the same tangent polynomial constants, individually rounded products,
signed-zero offsets and matrix multiplication order. This removes duplicate
projection math and makes the next real Rider-screen smoke exercise the shared
calculation. It still does not activate gameplay portal admission. This change
has not been compiled or run; no extra test slot was consumed.

## Central Windows checkpoint 5244448 PASS (15:33 UTC)

Source 524444818300886166ba56196ff520e04179a292, tree
ded5535c18b71914320fbf123a92180d406a0a91. Windows x64 Release build completed
15:27:32.049895–15:29:55.254681 UTC with exit 0, 350 sources. One visible
main-ben02-course-ga smoke ran 15:30:12.390529–15:30:57.878243 UTC and exited 0
naturally, with cleanup confirmed and zero remaining owned processes.
Root inspected the Main, Ben Rider, Watchtower Course and track image pixels.
The track baseline was 13.779161223315532 FPS: 69 GPU-completed swaps over
5.0075617 seconds, following the separate warmup. All eight racers used original
AI; human count, keyboard/controller packets and host-driver calls were zero.
Fog, pod shadows and original blur remained enabled. No recorder or extra run.

This exercises shared projection through the actual Rider wrapper and the
GA cold metadata preparation through race startup. The separate Controls page
was not visited. Gameplay portal admission remains unjoined; this is not a
portal-culling or full-game acceptance result. No fresh Linux test was run.

EXE SHA-256: 946ca0ca20326d38560ce09cd58ac4c5f9bd2d0c841aff61247eec36ce3dbde8.
Evidence ZIP SHA-256: 9a229f7e21f373ec9ad84a55e4300d8875a1ab5b1d0e15cb11b62c5b4b6d7fcc.
Next routine test may start no earlier than 16:30:57.878243 UTC.

## Concrete SearchList render callbacks (15:37 UTC source inspection)

The actual 27E260 constructor installs table3D2D30. Its +44 slot is 27E880
and +48 slot is 27E920. This resolves the previously opaque List method callbacks;
these are not the generic Entity+44/+48 traversals.

- 27E880 returns immediately for null block table3C. Otherwise it visits full
  preceding blocks in physical order and the final block, calling 27EAF0 with
  a pointer to perBlock30 or lastUsed38 respectively. The helper rereads that
  count while iterating. The currently admitted native one-block registry
  needs no invented multi-block behavior.
- 27EAF0 clears the method's actual selected-object collection at +18 through
  27F050 before its block scan. It skips words with bit1 set and masks bit0
  before interpreting the object. For enabled objects (03 bit0) with 03 bit2
  clear, it branches on the real type02 against method2C/2D.
- Matching method2C requires object31 bit0, invokes the embedded entity at
  object+8 through its table+40, then enqueues that same embedded entity only
  if model191 bit0 is set afterward.
- Matching method2D uses the actual searchable subobject's containing entity
  at object-30, requires entity29 bit0, invokes entity table+40, and enqueues
  it only if searchable03 bit1 is set afterward. No guessed type constants or
  reinterpretation of collision object references is justified here.
- After scanning a block it calls renderer442F70's table40->slot20 with zero.
  This reached backend step still needs an actual native owner.
- 27E920 walks the same blocks but passes count values to 27ECA0. That helper
  does not rescan those blocks: it iterates the selected collection at method18,
  retrieves each actual queued entity through 27F000 and calls table+48.
  With multiple physical blocks this outer structure can repeat the selected
  collection. Do not replace it with an unproven deduplication policy.

This establishes why a zone-visible Boolean plus an ad hoc racer loop is
insufficient: source +40 selection precedes selected-only +48 drawing, with
real model/searchable state and queue ownership. Existing native racer model
owners already retain type02/flags03 and actual entity8; the join should consume
those fields rather than making another detached model copy. No runtime code
or selected lists changed during this source inspection, and no test ran.

Source ranges and SHA-256:
- 27E260–27E2F0: da61ea0aa29792da23d2f49cebabac84aad6f2477d941154de0bc7e79ae46c75
- 3D2D30–3D2DB0 table: 929d2b568c04f5745098bf6147671e407fc110eb4994c91725d1f2f379ff61b3
- 27E880–27E9C0: 173d8bf4f05cb5c605385380de76deb972f7c49fb773d604d7ed9bbfd5def4e4
- 27EAF0–27ED70: 0949db4aae9c01f4ef28c16268895c8eb7df9f9ab334ba075e52e0d180a47cff

The base method constructor 27E9D0 also resolves the concrete type ownership:
method2C is the result of registering exact `SoultreeObject`; method2D is the
result of registering exact `SearchableGameObject` through the actual shared
383408 registry. These are insertion-derived IDs, not fixed byte constants.
The selected collection is initialized at method18 by 27ED10 then 27ED40 with
initial capacity30 and growth30. 27EEA0 appends at the current count, preserving
order and duplicates; it grows only when count reaches capacity. 27F050 zeros
the old used pointer prefix and sets count to zero without discarding capacity.
27F000 retrieves by index. Neither selection nor drawing permits replacing the
real queued embedded entity with a newly allocated wrapper.

Additional range SHA-256:
- 27E9D0–27EA70: b5176cbc03baa856ccb5181ab5c30e05dd82aa512c5f0583e0b6137adc151a61
- 27EEA0–27F080: 03a5fe02b946daa7a856fbb255843687b17342cc1f251b4e7706e4b36dc4d634
- 48 label bytes at3BBFC0: 892805954fa6ae9cccd310115b541db0629cc5d51b007be172c4a0be2a9fed9d

Queue initialization confirmation: 27ED40 preserves the supplied growth value
in +08 and capacity in +0C, allocates capacity*4 source pointer bytes, and sets
initialized10 bit0. Its body was inspected separately rather than inferring
those meanings solely from the append path. 27ED40–27EEA0 SHA-256:
63d98ac35e8790c106026f5607af492441f0b184035f37e9e89a74137e628595.

### Native lease boundary still required

The current CameraVisibilityLease excludes DrawLease. Consequently a proposed
render walk cannot hold that camera lease while calling existing beginDraw:
that would correctly reject as overlapping ownership. A future joined render
phase must own the same 4C0 registry and expose an explicitly ordered nested
model draw borrow, while continuing to exclude reset/membership mutation and
stale camera publications. Copying the zone registry or releasing its protection
mid-walk would evade the real lifetime contract. The current authored accessor
is read-only and intentionally does not grant any such nested dispatch right.

## Windows observation parity, pending next smoke

Added optional Windows current-thread CPU sampling to `phase_timing50gk_fixture.h`.
`GetThreadTimes(GetCurrentThread(), ...)` supplies kernel plus user durations;
FILETIME high/low words are combined as unsigned 64-bit values, checked before
addition and conversion from 100-ns units to ns. Failed reads remain invalid.
The existing steady wall clock and disabled-by-default scopes are unchanged.
No process CPU substitution, simulation timing change, or resolution guarantee.
Reference: https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadtimes

Manual source review only. This change has not been built or run. The latest
verified Windows runtime remains 5244448, completed 2026-10-14 15:30:57.878243 UTC.
Next routine smoke may begin at or after 16:30:57.878243 UTC. Enable existing
RR_TRACE_PHASES=1 and RR_TRACE_RENDER=1 for that one basic Windows smoke and collect
phase totals alongside the required four screenshots and track FPS. CPU sampling
may be coarsely quantized; use interval totals rather than treating zero in a
short scope as evidence of no work. Instrumented FPS is not an uninstrumented
performance comparison. Original gameplay render admission remains pending.

## New followed racer and planet request, 2026-10-14 16:03 UTC

User requested a racer not yet followed and a planet not yet visited. Shared
chat history records Tatooine, Occo/Sullust, and Dud/Ben on Gamorr. No Mon Calamari
or Alder followed-racer run was found. Initial Gasgano choice was corrected
before any launch: POD07 is outside the currently admitted Course racer domain.
POD10 (Alder, original PHY spelling Alder Bedo) is in that domain and the actual
eight-racer field. No profile unlock or new roster capability is introduced.

Added `main-alder10-course-ma` using the existing original UI event route.
Bounded Rider Next navigation waits for original racer ID10 and a fresh accepted
model publication; Course follows actual fresh TA -> SA -> MA and validates
slot3/course4/planet5, subtype1, three laps, eight entrants. Captures wait for
those actual selections. Missing assets or failed cold admission reject rather
than substituting another destination. Both existing GA routes retain three
Course Nexts and their original racer IDs. All racer controls remain original AI.

The evidence-complete predicate now requires the Course image as well as Main,
Rider, Track, and FPS. This closes an omitted required stage in that predicate.
Manual source/diff review only; no build, syntax check, or gameplay check ran.
The next central Windows smoke must use this route rather than Ben/GA, and keep
the already scheduled thread-CPU observations. D:-local MA runtime readiness and
preparation are assigned to the existing central executor without launching a
software test. Earliest routine test remains 16:30:57.878243 UTC.
