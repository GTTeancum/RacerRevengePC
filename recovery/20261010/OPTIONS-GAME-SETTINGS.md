# Authentic Game Options page4: source-only integration package

## Delivered and remaining

The staged package joins the actual screen12/page4 `options.dat` controller, all 37 directly used widgets, live percentage labels, original arrow/prompt/background rendering, and a distinct original page aperture into the existing Options manager/session. Menu1 and Game Options4 share one real widget3, one authenticated texture bank, one scene/camera/fire owner, and one external transition manager. No separate screen manager or bank switch is created for page changes.

Page4 supports real local edits, restore defaults, original navigation/clamp/defaults audio, and an exact uncommitted Cancel route back to menu1. Accept remains explicitly unavailable: source1F66F0 commits settings then calls188740, whose mono/stereo/surround application needs a genuine native application sound-mode consumer. This package does not fake that effect or save-file persistence. Other Options pages and HOF remain unavailable.

Root still owns coordinator/host route integration. The active coordinator's fixed menu page identity must be extended for actual same-manager page notifications/current-page publication before page4 is enabled. No build, test, syntax check, project parser/decoder execution, sanitizer, or gameplay was run. Only source editing and static original data/instruction inspection were performed.

## Files and merge order

- `files/` contains22 candidate files, including the new controller pair. `preimages/` contains the exact20 replaced files copied before editing.
- Register the one new translation unit: `src/game/original_ui_game_options_controller50kg.cpp` in every central original-UI build list. No other new translation units.
- The material pair preimages are the material worker's `racer-lighting-reconstruction/rider-ipum-material` freeze, integrated by root as c62aab5. The Options-local square correction is frozen separately and may be integrated first. Do not overwrite newer material policy additions when applying this delta.
- `audio/` is an independently frozen overlay with its own active preimages and evidence. It adds authenticated Event2 ADVANCE and `playGameOptionsEffect`, required by the session candidate. Main and menu allowlists remain unchanged; requiredVoices becomes7.
- Draw resource candidates preserve root's new const `fire()` getter. No root-owned frontend coordinator or host entry files are replaced.
- Shared aperture and ExitText extensions retain existing Main/menu behavior, adding exact page4 identity only. Shared text geometry keeps `prepare` static-only and adds bounded `prepareDynamic` without changing authored source type or identity.

## Coordinator contract

Cold Options input optionally retains `shared_ptr<game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile`, the sole application owner of the fields below. Missing owner leaves page4 unavailable. It must be known from the real imported/created profile lifecycle. `resetOriginalUiGameSettingsProfileFields50KG` implements only the original field resets and deliberately does not set `known`, apply audio mode, or save.

`ownsPreparedPage(1)` certifies the existing menu owner. `ownsPreparedPage(4)` additionally requires the real settings profile, distinct bound page4 aperture, and joined page4 resources. It is not an external readiness callback.

Existing input receipts now return `pageProposed` and the existing typed `routeProposal` for these exact shapes:

1. Menu slot3 -> screen12/page4: inputPC1F9640; changePage requestPC1F9898; one word `optionsSlot6E4=3`, maskFFFFFFFF, storePC1F9940.
2. Page4 Cancel -> screen12/page1: inputPC1F50F0; changePage requestPC1F51D8; one word new field `optionsGroup6E8=attachedGroup1..8`, maskFFFFFFFF, storePC1F51C8.

Both proposals are unadmitted, action unavailable, with exactly destinationOwner blocker. Root revalidates `routeProposalCurrent`, admits the real destination through `ownsPreparedPage`, promotes only its retained decision copy, and invokes the actual external owner's `requestRouteDecision`. Its commitRouteSelection callback revalidates and writes the corresponding sole profile field. Immediately afterwards call `notifyRouteAccepted` with the original unmodified proposal. The session verifies the actual lifecycle7/requested page/profile store and emits source SELECT1 for menu->4 or BACK5 for Cancel exactly once. It never commits route/profile itself.

Cross-screen Back remains the earlier exact4/page1 contract. Accept never produces a route proposal. Settings edits do not mutate the application profile.

For page transitions, keep the same manager, session, GPU publication and scene. Dispatch original previousPage94/requestedPage90/entry74/7C/auxiliary notifications and phase polls to this session using their actual route, including requestedPage90 before manager current-page publication. The coordinator must read the authoritative manager current/requested page rather than continue demanding menu1 for every notification. Keep audio-before-manager-before-children scheduling unchanged.

Initial manager activation remains menu1; direct cold page4 activation is not admitted by this package. Same-manager selectRequestedPage runs page4 opening before its first frame, so the original attached group/value is available to1F5F40.

## Original source proof

Authenticated Options archive:200341 bytes, SHA256fde295f01154aa9fa6fb7ff5faccdb0a3ac8380214241ba3d13490ebd0d8ade2.

- `UI/UIOption/options.dat`:2609 bytes, SHA256940544663e9a76e660db3a023ba9f7da7582b4a93071217cb92bbec36554ebf2; PageNo4, eight groups plus group0,21 controls plus control0.
- `UI/UIOption/Widgets.Dat`:58854 bytes, SHA25605afba1bd71336fc1861c8045c6b3bde41002ca211558eef592a4d0fcd34117d.
- Factory1D0E30, ctor1F46A0, vtable3D0D50. Page create1F4710 calls shared1D63B0 at1F4724. Camera+C4 at1F4FD0 borrows the manager's actual+2B0 camera, not authored per-page CameraPosition1 fields.
- Entry1F4FF0; input1F50F0; update1F5BE0; render1F5F40; opening1F5BF0; opened1F5D20; closing1F5DD0; threshold1F5F30(return only); closed1D7A00(shared).
- Static instruction listings and the actual DAT copy are under `evidence/`. They are read-only instruction/data descriptions, never executed guest code.

### Settings and authentic labels

Groups1..8 are Auto Save, Sound Mode, SFX volume, Music volume, Voice volume, Announcer volume, Split Screen, Vibration. Labels and descriptions resolve through original English localization and original Venus16/Denmrk16 fonts, with authored positions/colors/selection flags. No labels or rectangles were substituted.

Profile fields loaded1F65F0:
- flags11050 bits0(auto save),1(vertical split),2(vibration)
- word1101C(sound mode0/1/2)
- words11020/24/28/2C(SFX/music/voice/announcer)
- remembered group word6E8

Local defaults1F6840 and profile reset1865B4..18666C agree: auto save on, stereo1, volumes50/100/80/80, horizontal split, vibration on; remembered group1. Profile field reset preserves unrelated packed bits.

Input events0/1 move the current group's control before adjusting its setting. Numeric values move10 and clamp0..100; source sound7 when the arithmetic overshoots, otherwise3/4. Events2/3 wrap enabled groups while retaining each group's selected slot. Event5 restores local defaults, synchronizes choices, then requests sound2. Event4 applies source settings then exits; native leaves this unavailable pending the real apply owner. Event6 remembers group and returns without committing settings. Nonzero controller and events>6 return accepted with no work.

Group movement source2A68E0/2A69B0 and non-attaching slot selection2A6BC0 are added to the shared navigation leaves. Callback order and repeated calls are preserved. Requested90 resets groups0..8; previous94 hides them; entry disables125/126/127/3/15; opened sets mask7 and enables them; closing clears mask and disables125/126/127/15, then deselects descriptions in original order.

### Resources and frame sequence

The actual page4 widgets are3,15,18,19,20,37,38,48,49,50,107,108,109,110,111,112,114,116,117,118,119,120,121,122,123,125,126,127,156,157,158,159,160,161,162,165,167. Union with menu1 is56 widgets. Ordinary siblings remain ascending and OnTop siblings descending. Hidden other-page widgets remain in the real shared bank.

Original1F5F40 writes numeric widget38/112/114/165 through SetText at1F5FB8/1F5FE4/1F6010/1F603C. ELF3AFFF0 is `%d%%`, not `%d`. Live labels are0%..100%, fitting the authored four-character capacity. The resource owner cold-prepares all101 possible numeric glyph runs plus the original empty initial string, using actual CEL/PST font metrics. Render updates live strings at the original page callback point AFTER ordinary widgets and BEFORE arrows/aperture, preserving the source's first-frame and changed-value ordering rather than updating them early during input.

Background4quads: create1F4734..1F4Cxx, draw1F5F7C. Selection arrows use the selected VALUE widget's first text position and actual integer width. Before is2+(x-rightWidth), after is3+(x+width), y-3; actual animated arrwrght/arrwleft draw PCs1F6374/1F6410.

Shared prompt draw1D7A10 reads page mask7. Page4 create sets cross(514,378), triangle(410,378), square(280,378), unlike menu1's cross484/triangle397. Square registry ordinal3215/byte70521 comes from actual APPSTART Mat_Stbl. Its PST is OPTIONS-LOCAL member ordinal5, offset172032,604 bytes, SHA2564ab9cac37b5359729352e70d6ff92bf91a2841db60e98f91cff467ba37e51ae8. Identical OVERLAYS bytes do not replace this source identity. The original PSM is absent; typed fallback retains options.dat/local PST/shared creation1D6994/ScreenMT provenance.

Shared ExitText+440 is prepared separately for actual page4 and uses original English89/Denmrk16 atlas. It is not normally emitted on page transitions; shared aperture only submits it on actual screen/course exit endpoint semantics. Existing scene, fog, camera, particle material and lighting identities remain the authentic Options owners.

## Exact unfinished owners

1. Central same-manager coordinator/host admission and profile lifetime are root-owned and not changed here.
2. Accept requires source1F66F0 profile commit followed by188740: sound mode0 sets E38=1/E34=0/9900=0; mode1 sets0/0/1; mode2 sets0/1/1; each resets9918. Those fields need real native audio consumers, not mere stored booleans. Volume/category, split, vibration and autosave application consumers must share the actual committed application profile. No save-file writer or memory-card readiness is claimed.
3. Other Options pages, real second-controller popup and HOF remain unavailable.
4. Source integration, compilation, runtime rendering/navigation/audio acceptance remain unverified until root's permitted centralized checks.

## Final source review and freeze

Source self-review completed. An independent lifecycle follow-up could not start because the agent thread limit was reached; no independent acceptance is claimed. Root will review before integration.

Additional SetText proof:2A9FF0->2A9DE0 calls29D940 then29DA40 with alpha255/indefinite each time, including unchanged strings. `refreshGameOptionsVolumeText` reproduces the shared TextWidget setup at that exact render point and honors the original +CC lock for both string and setup effects. `evidence/dynamic-settext.txt` retains those original instructions.

The material pair was independently frozen, reviewed and integrated by root as ccc17a7. Skip those two already-integrated files when applying the rest of this package. Its preimages intentionally remain the exact prior c62aab5 source rather than being rewritten after integration. The remaining20 files and separate audio pair are now frozen for root source review. Any later correction will be announced or staged separately.
