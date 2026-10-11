# Main to Rider: bounded material registry lifetime

Static original-source research, 2026-10-11. No active changes, builds, tests, syntax checks, project parsers, gameplay, guest execution or device emulation. Original ELF SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30; original-data mapping file offset = VA - 0x100000 + 0x80. The included word listings are static byte-to-text extracts.

## Result

Ordinary Main/Options to Rider is an old active-screen release followed by a new screen construction within the existing global UI. It is not a full material-registry reset. Material references owned only by the old screen end before the next screen factory runs; actual shared owners keep their references and loaded materials alive. Do not carry all Main passes forever, nor reset all shared material history to cold.

The two actual global popup owners provide a concrete surviving population of six material names. All six have a distinct popup equality tag and no pre/post actions. When those owners submit no geometry, omitting their groups preserves the ordered projection of the inspected Rider leaves. This is a bounded projection proof for these identified owners, not a complete assertion about every global, debug or gameplay owner.

## Screen and registry ownership

17DB80 is the ordinary UI-screen replacement path. It calls current global UI virtual+7C at17DC64 before virtual+78 factory at17DC7C. The actual global vtable3CF310 gives+7C=1B86A0 and+78=1B7DF0.1B86A0 jumps2A3510.

2A3510 drains the renderer and deactivates the global helper list, then detaches current manager globalUI+30 via141A70, invokes its virtual+10 at2A35B8, and nulls+30. The old manager is released before the new manager is constructed and attached at1B7DF0..1B8300. This routine does not call full material or texture clearing.

Entity virtual+10=142590 recursively releases its child hierarchy and calls104970.104970 decrements the actual entity reference count and invokes its destructor only when that count reaches zero. This is an ownership release, not permission to assume that unrelated external references disappear. Main destructor1CF220 specifically releases its scene+2B4, then calls generic manager destructor1CCAA0. The latter releases+2A0/+190, frees its auxiliary array, releases the named screen resource via12E5E0, and calls base2A3AD0. These source paths support retirement of genuinely screen-owned lifetimes; they do not justify retirement of arbitrary retained shared references.

A ScreenTexture owns its material wrapper at+54. Its destructor2DEFE0 destroys that wrapper at2DF068..74; wrapper destructor246260 calls base299AC0.299AF8 calls14D500 for its valid material token.14D500 decrements the material registry entry's reference count, checks the actual count at entry+8, and only at zero destroys the loaded material using virtual material+70/+8 and stores a null registry object with298F80. The material registry is global382B38; entries are12bytes starting at registry+1C.14D590/298A30 supply the matching increment and associated-template ownership.

14D370 walks referenced unloaded entries and loads them.14D440 resets group bookkeeping and registers every nonnull loaded object in registry order, without filtering for the new screen.29BEA0 clears pass-group indices, not constructor/binding history.

The whole-UI departure path17DCE0 is different: after detaching/releasing the global UI and nulling application+2D8, it calls14D240 at17DD4C and29E270 at17DD58.14D240 destroys every loaded material and clears references;29E270 clears the texture registry. Ordinary Main to Rider does not traverse this path.

## Exact shared popup owners

Global UI constructor stores PodUIPopupDialog at+470 (1B7BF0; constructor203C90, setup203D40) and PodUIControllerPopupDialog at+47C (1B7C1C; constructor2054D0, setup2055B0). These fields are outside the active manager+30 and are not destroyed by2A3510.1B86D0 subsequently detaches/reattaches these existing objects; it does not reconstruct them.

Original strings at3B0518..3B057F and direct setup calls establish the actual material names:

- ui/textures/dimpopup: both dialogs' zero-map overlay primitive, setup203D78/2055EC via2E1AE0.
- font/venus16popup: popup ScreenText instances at204058 and204244 via2DFD90.
- ui/textures/overlay1popup: both dialogs' three ScreenTexture frame pieces, e.g.2043A4 and205B4C.
- font/venus14popup: controller popup text at205818,205970,205A48.
- ui/textures/icon_x_popup and ui/textures/icon_tri_popup: controller popup ScreenTextures205B1C/205B34.

2DF0B0 creates each real material wrapper and calls14D370/29E340, so these are actual loaded material references, not merely names listed in an archive. Dialog visibility does not release their embedded ScreenTexture/ScreenText owners. The global UI+5C transition entity calls2AE720 with a null material name at1B7D28; the guarded allocation at2AE734 is skipped, so this particular owner is not evidence of another material reference.

Original PSM contents, hashes and full section lists for these six names are retained in /workspace/shared/recovery-20261010/ui-main-registry-reconstruction/evidence/cold-material-alias-candidates.json (controllerPopup entries, lines2474..2684). Each has exactly General plus ShaderPass_1, one pass, ShaderPassGroup=popup, and no Prologue or Epilogue. The first has zero texture maps; the remaining five have their named texture maps.

## Empty-group action and ordering proof

Material constructor29B44C/29B450 clears prologue+44 and epilogue+6C. Loader29B6B0..6F4 allocates a prologue only if the Prologue section exists;29B800..844 likewise for Epilogue. These operations do not inherit a pre/post action from the ScreenMT shader template.29BF90..BFD0 installs only nonnull actions on first/last groups through29C690/29C6B0. Group constructor29C0C0 initializes group+0 and+8 to null.

24B7B0's drain checks group+0 at24B8EC and calls24AF30 only if nonnull. It checks group+8 at24B918 and calls24B610 only if nonnull. These two guards are outside the current-generation geometry test24B900..908, but both are null for the six materials above. Therefore an unqueued popup group has neither geometry draw nor pre/post target action. Do not generalize this to arbitrary empty groups with authored actions.

Source inspection of the actual Rider menu PSMs under authored-menu-data-inspection/members/rider and preview/pod-assets/model-materials/stat-assets member sets finds only the explicit top tags on venus14top, venus16top and denmrk16top. Other inspected passes use the original default none. TextureAnimation=unique on binder/arrows/medstrsm is an independent exclusion, not a named ShaderPassGroup. No popup tag is authored in the inspected Rider population.

249BA0 rejects unequal group tags. Thus none/top Rider passes cannot redirect to or receive redirects from the six popup-tag passes, whether those passes are cold or bound. Registering non-equivalent popup groups inserts index gaps only. It does not change the relative order of Rider registrations, and an inequality newIndex<earlierNextPassIndex stays equivalent after this monotone insertion; the actual next-pass barrier identity is preserved. Deleting these unqueued, action-free, non-equivalent groups therefore preserves the Rider registration/redirect/FIFO drain projection and all Rider-local representative histories.

## Admission boundary

This closes the identified popup-owner concern for a closed ordinary navigation frame in which those popups are not drawing. It does not certify all possible foreign loaded owners, overlays, debug geometry, device reconnection dialogs, resource substitution, or render-target actions. If a popup opens, it needs its own actual material registration and submitted geometry; the omission proof no longer applies. Unrecovered foreign owners must not be declared harmless merely because the current composer has no packets for them.

Similarly, after removing Rider registrations, preserve or retire each material history according to its actual remaining native owner/reference lifetime. The original refcount mechanism proves the required policy; absence from one frame's draw list does not prove unload. This research supplies no broad caller-set readiness or synthetic loaded-state flag.

Root integrated this source after the 03:17 build failure. It remains unbuilt and untested; Rider destination activation is not included.
