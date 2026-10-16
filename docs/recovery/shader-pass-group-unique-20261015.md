# ShaderPassGroup literal unique: exact sentinel and native boundary

2026-10-15; authenticated SLUS_202.68 static-data inspection only. No tests or execution of guest/project code. TimeStats remains unactivated.

## Parsing and construction order

- Pass constructor246FB0 stores signed group22=-1 at246FD4.
- Concrete pass setup247110 performs its other setup and parameter-buffer construction first:2474D0 tests parameter count28;2474E0 calls249420;2474E8 stores parameter buffer34.
-2474F0 then CALLS247530, with this passed in the2474F4 delay slot. **2474F8 stores the returned signed halfword to group22 AFTER the helper returns. It is not the initial default store.**
- Helper247530 copies the section argument intoa0, reads ShaderPassGroup via158AD0 at247550, with default3B61E0=`none` and destination stack+10. Literal bytes verified directly:3B61D0=`ShaderPassGroup`,3B61E0=`none`,3B61E8=`unique`.
-247560 calls308F00 against`none`; equal returns−1 at247574.247580 compares`unique`; equal returns−2 at247594.
- Only OTHER strings call actual382FC0 group's manager virtual+1C at2475AC with name and argument0. The unique branch skips that call completely. It does not allocate or retain a named group object.

## Collector nonmerge semantics

- Material registration29BED0 walks source passes ascending.29BF04 calls24B7A0, which tail-jumps29C490; returned registration is stored in material5C at29BF0C. The predecessor's later-pass barrier is written at29BF74.
-29C490 creates a collector node for the new pass, stores pass pointer at node4 (29C5E0), and compares it against ALL earlier registrations.29C60C calls the new pass's virtual+1C comparator. Authenticated vtable3D2110+1C contains249BA0.
-249BA0 compares program, map count, texture IDs, UV indices, flags and state packet, then rejects unique texture-animation prototypes. At249CF4 it loads the FIRST pass'sgroup22;249CF8 supplies−2;249D04/08 returnsfalse if equal. Otherwise249D0C–18 compares group22 values.
- Thus unique is a **per-pass nonmerge sentinel**, including comparisons of two otherwise identical unique passes. Native equality merely comparing−2==−2 would be wrong. A symmetric early rejection when either is−2 is equivalent in the admitted−1/−2 subset (the original's second operand−2 against first−1 fails the final equality).
- Successful equality may redirect the earlier node+18 to the new node at29C644 only while current index<prior limit1C. Since unique can never compare equal, its node is neither merged into another pass nor a destination for an equal pass. All existing registration order/barrier logic remains necessary.
- Six stats ScreenTextures retain ONE canonical stats material/pass. They do not create six material passes or six collector nodes. Their six primitive submissions queue into that one pass's independent node.

## Draw and release consumers

- Screen primitives submit246760. It resolves actual material00 (or explicit override) through298E60 and calls24A620 at2467B0/2467E0.
-24A620 iterates enabled passes, services the actual animation route, reads material5C at24A790, resolves that collector node and calls24B9B0 at24A7B4. No unique-specific alternate draw call is introduced.
-24B9B0 calls29C150 at24B9DC.29C150 follows any node+18 redirects and appends the primitive FIFO entry to the final node; for an unmerged unique pass no redirect exists. It preserves linked submission order via node0C/head and10/tail at29C238–250.24B9B0 additionally preserves origin parameter data when a redirect exists (24BA0C–24BA24).
- Collector drain24B7B0 walks registration slots ascending at24B8E0–24B93C, optional prologue24AF30, current-epoch node24B130, optional epilogue24B610.24B130 emits the pass state via244B90 at24B39C, then visits its submission chain; primitive packet submission is244B90 at24B50C, next-node traversal24B530. The unique effect is retained independent pass grouping and order, not a new shader/raster policy.
- Pass destructor247030 frees state30 and parameters34, releases genuine texture tokens1A/1C via29E400 (sentinels skipped), then calls base29ADC0 at2470D4. Full29ADC0–29AF8C releases animation/buffer owners and base object state. Neither accesses/releases group22. Unique creates no additional numeric reference or destructor obligation.
- Collector-node lifecycle29C3F0/29C300 destroys nodes through virtual destructor, without turning a borrowed pass pointer into a newly retained pass. Existing source pass/table/renderer retirement constraints still apply.

## Smallest faithful native support

1. `original_standard_material_object50la.cpp`: admit only missing/none and literal unique. After source parameter setup, read the existing string helper with defaultnone at247550; setgroup22=-1/-2 before constructed publication, corresponding to2474F8. Named strings remain rejected. Do not mapunique tonone or use TextureAnimation uniqueness as a substitute.
2. `original_standard_material_table50la.cpp`: collectorPass validates only−1/−2; equalCollectorPass249BA0 explicitly refuses−2 merging. Leave all other validity/state/texture-animation comparisons, complete registry iteration, registration cap, barriers and terminal-failure rules unchanged.
3. Update stale comments saying all non-none groups are unsupported. No new group manager, reference owner, per-primitive material, sort policy, special shader or release path is warranted for this literal.
4. Renderer must preserve the independent pass identity and queue order; matching GPU state is not permission to discard a unique pass boundary. These changes alone do not supply TimeStats primitive geometry, draw owner, demand lifecycle or activation.

Bounded conclusion: the proposed root sentinel support matches the recovered paths. General named-group382FC0 ownership/behavior remains intentionally unrecovered and closed.

## Root uncommitted implementation review

Read the actual object/table/header diff after root applied sentinel support. No concrete source-proven defect found: assignment follows parameters34 publication; missing/none stays−1; unique becomes−2; named strings reject; collector validates−1/−2 and rejects either operand−2 before remaining equality. Moving the pure nonmerge comparison earlier has no source side effects for validated pass inputs.

Case handling verified independently: original308F00 uses character table3C6391, adds32 iff classification bit0 is set, and compares resulting bytes. Direct static table bytes for ASCIIA..Z have bit0 set; a..z have bit0 clear. Native `same` folds exactlyA..Z within its explicitly admitted printable-ASCII string domain, so mixed-caseNONE/UNIQUE comparisons match. Non-ASCII and arbitrary named groups remain rejected rather than claiming broad locale parity.

Root also added mounted-provider byte equality before the TimeStats facet retain. This closes the previously reported disconnected-archive check at acquisition. It still does not certify an already-populated row's source or GPU readiness; concrete loader must validate resolved canonical material/texture before publication.

## Native change

Source-reviewed, untested native support now parses the literal after parameter setup and stores−1/−2 at the original2474F8 boundary. Named groups remain rejected. Canonical collector admission accepts the two negative sentinels and explicitly excludes−2 from merging even against itself. No group allocation or release was added. This is one registration per material/pass, not a new node per primitive. It removes the identified stats material population restriction; it does not activate the HUD or establish runtime rendering.
