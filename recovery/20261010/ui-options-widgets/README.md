# Original Options menu widgets and draw contribution

## Status

Source-only native C++ reconstruction, staged separately. **No builds, software tests, syntax checks, parser/decoder execution against the implementation, sanitizer runs, gameplay, or guest execution.** Static ELF byte-to-text instruction inspection and raw data/hash reads only. The active repository was not edited.

This closes real menu widget callbacks, original font/icon geometry, original material/texture ownership, background/selection arrows, prompt provenance, Options aperture binding, and cold camera preparation. It does **not** certify the complete Options screen: the genuine `opt.scn`/`opt.toy` particle scene is still a separate live contribution, and popup, audio, manager registration, source profile transactions, exit-object effects, and all other Options pages still require their actual owners.

## Integration files

New translation units, with matching headers:

1. `src/game/original_ui_options_menu_widgets50kg.cpp`
2. `src/game/original_ui_options_menu_layout50kg.cpp`
3. `src/game/original_ui_options_camera50kg.cpp`
4. `platform/linux/native/original_ui_options_draw_resources50kg.cpp`
5. `platform/linux/native/original_ui_options_draw_contribution50kg.cpp`

Existing-file candidates:

- `src/assets/original_ui_material50kg.{h,cpp}`: narrowly adds Options-menu cross/triangle missing-PSM provenance and admission. Existing Main behavior stays separate.
- `src/game/original_ui_aperture_owner50kg.{h,cpp}`: adds authenticated Options/menu.dat full and entry-only binding, actual Options hook targets, Options update-delta entry, and screen12 completion identity.

Exact existing-file preimages are in `preimages/`, with `PREIMAGE-SHA256SUMS`. Merge these small changes with concurrent work; do not overwrite another worker's additions. No build-definition candidate was authored because central ownership is changing it. Existing controller dependency is root's integrated `original_ui_options_menu_controller50kg` (including its noChange audio guard).

## Native mixed bank

`OriginalUiOptionsMenuWidgets50KG` cold-binds the real Options controller/Widgets.Dat. The admitted menu-only population is exactly20 source objects:

`1,2,3,5,6,7,8,9,10,12,97,98,99,100,101,102,103,104,105,106`

- 16 genuine Overlay_Text objects, including directly controlled widget3, whose authored text is empty.
- Three genuine Icon objects98/99/105 using `overlay1` and original MapRect/PartialRect/color/alpha.
- Movement_Control5, already descriptor-authenticated by the controller. It is not bound through Main's identity-restricted movement class.

The bank preserves the admitted widgets' relative manager sibling order (ordinary ascending, OnTop descending); it does not turn control memberships into entity parents. This is a **closed menu population projection**, not a fake complete357-widget Options manager. Other original Options objects are omitted only while they remain inactive in this closed menu-only session. No other page is admitted. Full scene/manager integration must prove that omitted siblings have no live contribution before enabling a whole-screen route.

`phase()` consumes the source controller plans internally, preflights every ordered widget callback, then commits native widget and navigation effects together. All text uses the existing generic `OriginalUiTextWidget50KG`, including its special nonvirtual `setControlEnabled` distinction. Icon selection uses its actual authored visibility flags. Movement_Control uses the source selection/show behavior. There is no synthetic label or host-colored highlight.

`input()` invokes the page callback first, then snapshots widget5 selected+38, and invokes exactly its one source registration. A popup/controller early exit from the page still reports original return1; movement delivery remains controlled by the actual inherited container gates, rather than inferred from route admission. The returned route/profile/sound/flush decisions are requests, not completed effects.

## Real resources and material ordering

`OriginalUiOptionsDrawResources50KG` pins five authenticated archives (APPSTART, UIOPTION, UIFONTS, CONTPOP, OVERLAYS) and a genuine original native video-mode owner. It admits only640x448/color32/packetZMaximum0xFFFFFF/Zbuffer0x31 and an explicitly known material-runtime override state.

Actual texture/font sources:

- UIFONTS: `font/denmrk16.pst` + same-name CEL; `font/venus16.pst` + CEL.
- CONTPOP: `ui/textures/overlay1.pst`.
- UIOPTION: `ui/textures/mmbkg.pst`, `arrwrght.atm`, `arrwleft.atm`.
- OVERLAYS: original cross and triangle PSTs. Options lacks those PST/PSM names, so these are the shared source lookup results, not invented replacements.

Every ATM frame uses its original stored PST range. Generic `OriginalUiScreenAssets50KG`, material parser, exact-atlas native material policies, text geometry, icon layout, and native sampler conversion are reused. No Main archive or page identity is impersonated. Arrow animation config is read from the actual Options PSMs (`unique`,16FPS,reverseLoopOnEnd1,loopOnEndToFrame0); no UIMAIN-specific config shortcut is called.

Actual registry leaves are authenticated against APPSTART Mat_Stbl's token ordinal AND byte offset. The original delimiter set is only space/CR/LF, not tabs or guessed comments. All3235 tokens must still be present.

| Material | Token | Byte offset |
| --- | --- | --- |
| ui/textures/mmbkg |2956|65593|
| ui/textures/overlay1 |2962|65677|
| ui/textures/arrwleft |2996|66421|
| ui/textures/arrwrght |2999|66491|
| font/venus16 |3006|66635|
| ui/textures/notexture |3155|69494|
| font/venus16top |3178|69882|
| font/denmrk16top |3180|69917|
| ui/textures/icon_tri |3216|70543|
| ui/textures/icon_x |3217|70565|

Policies retain the actual authored ShaderPassGroup, depth, blend, wrap/filter and alpha semantics. Resource-table ordering is not a standalone frame-order promise; merge real external scene submissions before the final global material drain. In particular the missing `particle/mmprt` leaf token2902 precedes these UI leaves.

The Options-specific missing-PSM API only admits cross/triangle from the real inherited constructor1D63B0 called at1F8E54. Provenance stores Options menu.dat, shared original PST, explicit ScreenMT, source call1D682C or1D68DC, and builder2475C0. No material text or fake PSM identity is synthesized. Existing Main logo/prompt provenance remains distinct.

## Original page render1F9A80

The creator1F8E40 initializes four ScreenTextures at+4D0/+530/+590/+5F0 from literal`mmbkg` (3AF3D8); the loop draws them at1F9ABC. They are four320x224 quadrants at(0,0),(320,0),(0,224),(320,224), using original source endpoints319/223 divided by actual cold atlas dimensions, mirrored in the original quadrants. Authored depth15 becomes original packetZ16777200.

The selected-label dispatch table3B0110 maps slots1..6 to widgets`6,12,7,8,9,10`. These are not consecutive IDs. The page reads the actual first ScreenText position and calls GetWidth2E16F0 at1F9BA0. Native text geometry reproduces those font/CEL measurements and post-justification positions.

The real source selection decoration is two animated material images:

- arrwrght before text: x=2+(textX-coldRightWidth),y=textY-3; draw1F9C48.
- arrwleft after text: x=3+(textX+integerMeasuredWidth),y=textY-3; draw1F9CE4.

They retain actual cold dimensions, full0..1 UVs, packet channels127/127/127/63 and authored depth1. Animation switches the authenticated material frame; it does not substitute a rectangle or resize the ScreenTexture every frame.

`OriginalUiOptionsDrawContribution50KG` owns native packet collection and per-material animation state. It expects source call order:

1. begin with known source milliseconds/pause gating
2. project ordinary widget siblings
3. project Options page: four backgrounds, then right/left selection images
4. shared aperture's genuine+424 and+428 submissions
5. shared prompt projection, reading this bank's actual mask42C=0 or3
6. OnTop widget siblings
7. finishContribution

Cross and triangle use source x484/x397,y378, actual dimensions, source call1D7A2C/1D7A44, and mask bits1/2. Text retains original glyph quads, including zero-area glyphs. ScreenTexture strip orderTL,TR,BL,BR converts to nativeTL,TR,BR,BL. Aperture rectangles use the independently recovered source order and raw black/alpha63.

The result exposes original-order vertices and packets with material/image/frame/source-ordinal IDs. It deliberately exposes no whole-frame native draw, target clearing, GPU-publication certificate, or opaque-alpha promise. The live frame owner must join the actual scene, prove global alpha/depth assumptions, and perform real registry/FIFO drain before GPU submission. No implicit update or fabricated frame completeness occurs.

## Options camera and aperture

Source Options manager1D0580 creates its own640x448 camera with aspect bits3FAAAAAB, then uses CameraInfo reader1D07A0 ->114F90. The staged camera leaf reads actual Options ui.dat and uses the generic native49F composition. Effective authored position is(12.249284,18.657846,115.269455),look(0.636078,0,-0.771625),up(0,1,0),FOV30,roll0. Page callback1F9530 borrows manager+2B0, so menu.dat's CameraPosition1/Look1/Up1/FOV1/Roll1 are not a substitute effective camera. No live scene registration or motion update is claimed by cold preparation.

The aperture candidate reuses the original shared numeric1D7550 implementation, with real Options menu.dat OpenSize69,Time0.5,StartClosed. It selects hook targets from the actual Options vtable:

- opening+AC1F9D20
- opened+B01F9E20
- closing+B41F9ED0
- closing threshold+B81F9FE0 (source no-op body)
- closed+BC1D7A00 (shared)

Options update+38=1F9A70 saves the real delta at1F9A74 before jumping to2A6BB0. `saveOptionsMenuUpdateDelta38()` is separate from the Main call; wrong owner kind rejects. Options completion polls match screen12/page1. Entry-only binding still refuses every exit operation before mutation. Full binding still requires a genuine+440 exit-object owner, not a trace sink. The calling session must supply real hook/overlay owners; this module does not auto-admit itself.

## Remaining scene dependency, with exact evidence

The authenticated Options catalog includes:

- `ui/UIOption/opt.scn`,3442bytes,SHA256`147235426b8ac34cf4796dd2e4069efcc2401ef2533e6617791c15407275baa4`
- `ui/UIOption/opt.toy`,433bytes,SHA256`760c7458c3879f74a5f3d5e9d697f58bce1db906781ac2d2ff60f5267ddbe96b`
- mmprt.prt,mmfire.emt,mmprt.psm,mmprt.pst; their exact hashes match the Main copies.

Manager1D0580 creates the scene object at1D06A8, loads it through1D08E0 at1D06B4, and attaches its entities/camera through1D0DA0 at1D06D0. `opt.scn`/`opt.toy` were not already extracted in the available authored-data directory. No new archive decoder was run outside the root's centralized check slot. Those exact two members should be materialized/authenticated there before adapting the scene/emitter/lighting owner. Matching particle asset hashes alone do not prove identical scene transforms/emitter timing/lighting or authorize using the Main scene identity.

Other remaining boundaries: authentic second-controller popup, source audio playback/flush binding, manager/background-camera/global-active-object entry effects, original exit ScreenText+440, frontend/GPU publication, and actual native route transaction admission. ControllerConfig,GameOptions,MemoryCard,Credits,HOF remain explicitly unavailable. No settings emulation or save fallback is supplied.

## Source evidence

`evidence/` contains original creator/render/lifecycle and Options manager static word listings, raw background/arrow name bytes, raw selection jump table, actual Widgets.Dat/ui.dat/arrow PSMs, and authenticated Options catalog entries. Source instructions are read as data only; no PS2/VU or guest execution path exists.
