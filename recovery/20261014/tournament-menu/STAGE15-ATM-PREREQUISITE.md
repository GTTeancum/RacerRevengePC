# ATM connected-path prerequisite hold

Base: helper/tournament-menu 3223697 (stage14). Root reported integrating stage14 as fb40565. This note changes no runtime behavior.

## Evidence reviewed

Read the canonical source and Library plan libfile_4937887a5d20819198df34555d1df51b, rr-tournament-atm-owner-plan.md. No decoder, test, build, syntax check, launcher, project executable, or guest instruction was run.

The plan explicitly leaves application phase/update ordering and complete ScreenTexture ownership open. It proves the inner controller behavior: 29AC90 compares the application230 timestamp first, then application11C pause against controller18 animatesWhilePaused; 29AD58 writes the stamp after dispatch. The UI null-key route selects the actual prototype even for unique texture animation. It must not register a racer clone.

In this checkout, platform/linux/native/original_ui_main_draw50kg.h:110 exposes OriginalUiMainFrameClock50KG with known, advanceAnimations, and milliseconds. Its contract says the caller already owns pause/update gating. It does not expose application11C or distinguish a pause suppression from another update suppression. src/assets/original_standard_material_context50la.h retains real Video/application/runtime/lifetime identity but provides no application230/11C snapshot. The existing 29AC90 API in original_standard_material_animation50la.h belongs to the engine vertex instance, not a UI texture prototype.

Therefore advanceAnimations=false cannot safely be interpreted as applicationPaused=true: authored animatesWhilePaused would potentially admit a dispatch that the caller suppressed for another reason. Conversely unconditional suppression would not implement the original paused-animation override. A caller-filled readiness callback would not recover this prerequisite.

## Required root join

Identify the real Application owner and source-proven UI call point that supplies both the same application230 timestamp and application11C pause state to 24A740 -> 29B1D0 / 29AC90, including whether that call occurs when the current frontend suppresses animation updates. Supply an immutable owner-bound snapshot or a concrete private owner call, with the dispatch order and lifetime documented. Do not broaden the existing generic clock by guessing pause from advanceAnimations.

This is a prerequisite for the connected dispatch stage, not a claim that the ATM resource constructor itself lacks proof. The aggregate and child lifecycle are described concretely in the supplied plan. They remain implementation work; no partial aggregate admission was added in this hold.

## Proposed next implementation ownership

Helper owns src/assets/original_standard_texture_resource50la.h and .cpp; original_standard_texture_table50la.h and .cpp; original_standard_material_animation50la.h; original_standard_material_object50la.h and .cpp; original_standard_material_table50la.h and .cpp; and original_ui_screen_material_reference50kg.h and .cpp for the canonical medstar retain. Any change to platform/linux/native/original_ui_tournament_career_resources50kg.h/.cpp is confined to authentic selected-child/CPU-image correspondence. No new translation unit is proposed.

Root owns the concrete Application/ScreenTexture clock and traversal join, frontend admission, capture headers, profile/results, renderer retirement, and all tests. No shared frontend or profile/result file is changed by this note.

## Boundaries retained

Resolver still rejects ATM at original_standard_texture_resource50la.cpp:483 (24DC30). Material object still rejects TextureAnimation at original_standard_material_object50la.cpp:641-643. Cold representative binding still documents ordinary29C6D0 self-return at original_standard_material_table50la.cpp:365; it does not certify an aggregate as a selected frame.

Later bounded admission must keep nonzero ATM storage selector unsupported, explicit prior-candidate provider absence, real consecutive PST substream children, separate aggregate cache identity, ascending child retirement, terminal partial effects, null-key prototype selection, and unique-texture nonmerge. Named animation manager, racer clone registration, residency inference, manufactured texture rows, profile unlocks/wins, and frontend admission remain excluded.

No implementation or validation success is claimed for this stage. Root must review the prerequisite before the dependent connected stage proceeds.
