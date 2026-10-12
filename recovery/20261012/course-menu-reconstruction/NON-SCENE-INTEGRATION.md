# Ready non-scene source checkpoint

New reconstructed WIP; hashes identify current reconstructed bytes only, never pre-rollback originals. No builds/tests/syntax checks. Root exclusively integrates. All 31 non-scene source files are saved. Independent source review is complete for the bounded muted Rider19 → Course6/1 → fresh Rider19 Back route. Both admission constants remain false by root scheduling decision until after the focused GC lighting/fog observation and central integration.

## core

REPLACEMENT platform/linux/native/original_ui_rider_profile50kg.cpp <- core/platform/linux/native/original_ui_rider_profile50kg.cpp
REPLACEMENT platform/linux/native/original_ui_rider_profile50kg.h <- core/platform/linux/native/original_ui_rider_profile50kg.h
NEW src/game/original_ui_course_selection50kg.cpp <- core/src/game/original_ui_course_selection50kg.cpp
NEW src/game/original_ui_course_selection50kg.h <- core/src/game/original_ui_course_selection50kg.h

## session

NEW platform/linux/native/original_ui_course_session50kg.cpp <- session/platform/linux/native/original_ui_course_session50kg.cpp
NEW platform/linux/native/original_ui_course_session50kg.h <- session/platform/linux/native/original_ui_course_session50kg.h

## aperture

REPLACEMENT src/game/original_ui_aperture_owner50kg.cpp <- aperture/src/game/original_ui_aperture_owner50kg.cpp
REPLACEMENT src/game/original_ui_aperture_owner50kg.h <- aperture/src/game/original_ui_aperture_owner50kg.h

## widgets

NEW platform/linux/native/original_ui_course_draw_resources50kg.cpp <- widgets/platform/linux/native/original_ui_course_draw_resources50kg.cpp
NEW platform/linux/native/original_ui_course_draw_resources50kg.h <- widgets/platform/linux/native/original_ui_course_draw_resources50kg.h
NEW src/game/original_ui_course_widget_controller50kg.cpp <- widgets/src/game/original_ui_course_widget_controller50kg.cpp
NEW src/game/original_ui_course_widget_controller50kg.h <- widgets/src/game/original_ui_course_widget_controller50kg.h
NEW src/game/original_ui_course_widgets50kg.cpp <- widgets/src/game/original_ui_course_widgets50kg.cpp
NEW src/game/original_ui_course_widgets50kg.h <- widgets/src/game/original_ui_course_widgets50kg.h
REPLACEMENT src/game/original_ui_light_manager_lifetime50kg.cpp <- widgets/src/game/original_ui_light_manager_lifetime50kg.cpp
REPLACEMENT src/game/original_ui_light_manager_lifetime50kg.h <- widgets/src/game/original_ui_light_manager_lifetime50kg.h

## page

NEW src/game/original_ui_course_page_draw50kg.cpp <- page/src/game/original_ui_course_page_draw50kg.cpp
NEW src/game/original_ui_course_page_draw50kg.h <- page/src/game/original_ui_course_page_draw50kg.h

## frontend

REPLACEMENT platform/linux/native/original_ui_frontend50kg.cpp <- frontend/platform/linux/native/original_ui_frontend50kg.cpp
REPLACEMENT platform/linux/native/original_ui_frontend50kg.h <- frontend/platform/linux/native/original_ui_frontend50kg.h

## rider

REPLACEMENT platform/linux/native/original_ui_main_session50kg.h <- rider/platform/linux/native/original_ui_main_session50kg.h
REPLACEMENT platform/linux/native/original_ui_rider_session50kg.cpp <- rider/platform/linux/native/original_ui_rider_session50kg.cpp
REPLACEMENT platform/linux/native/original_ui_rider_session50kg.h <- rider/platform/linux/native/original_ui_rider_session50kg.h
REPLACEMENT src/game/original_ui_rider_character50kg.cpp <- rider/src/game/original_ui_rider_character50kg.cpp
REPLACEMENT src/game/original_ui_rider_character50kg.h <- rider/src/game/original_ui_rider_character50kg.h
REPLACEMENT src/game/original_ui_rider_dynamic_owner50kg.cpp <- rider/src/game/original_ui_rider_dynamic_owner50kg.cpp
REPLACEMENT src/game/original_ui_rider_dynamic_owner50kg.h <- rider/src/game/original_ui_rider_dynamic_owner50kg.h
REPLACEMENT src/game/original_ui_rider_model_scene50kg.cpp <- rider/src/game/original_ui_rider_model_scene50kg.cpp
REPLACEMENT src/game/original_ui_rider_model_scene50kg.h <- rider/src/game/original_ui_rider_model_scene50kg.h
NEW src/game/original_ui_rider_voice_choice50kg.cpp <- rider/src/game/original_ui_rider_voice_choice50kg.cpp
NEW src/game/original_ui_rider_voice_choice50kg.h <- rider/src/game/original_ui_rider_voice_choice50kg.h

## Additional targeted runtime replacements

frontend/RUNTIME-COURSE-THREADING.patch patches tests/native/original_ui_main_entry50kg.h and tests/native/terrain_interactive_drive50du.cpp. Apply individual UI/clock ownership hunks against current main; preserve new TA/GC illumination/fog and unrelated edits. No full runtime file replacement is supplied.

## Dependencies

- Core retained profile + selection is used by CourseSession, frontend and Rider Confirm; it must be integrated as one transaction contract.
- CourseSession depends on page, widgets/resources, shared aperture and shared LightManager plus all22 scene source files.
- Widgets owns the actual inactive popup cloned ScreenTexts/resources and real flags29 disable; session and collector read this actual owner. Active/card popup remains unavailable.
- Rider cold predecessor extension consumes actual Course static/hologram material inventory; Main predecessor behavior is retained.
- New voice-choice owner shares original application rawRandom308030 and retains lifetime guard/cache plus the SAME shared actual HostMonotonicClock backend already used by native loop consumers. Existing lap_host_clock50id.h supplies explicit native101A50 mapping; no chip parity claim.
- Independent Confirm review accepted the retained clock/shared-RNG owner and corrected truncating voice arithmetic. Both admission constants deliberately remain false. Source review does not establish runtime readiness.

## Explicit build source lists to update after root integration

1. platform/linux/native/CMakeLists.txt (add ${RR_ROOT}/ prefix around every new CPP below).
2. platform/windows/native/interactive50du.sources.json sources array (plain source-relative paths below).
3. tools/prepare_interactive_build50dv.py explicit native source list (same paths). Read/edit only; do not execute it. Windows sourceListProvenance hash for this script and reviewedSourceRevision must be refreshed to the actual integrated revision, not fabricated.

New non-scene translation units:

- src/game/original_ui_course_selection50kg.cpp
- platform/linux/native/original_ui_course_session50kg.cpp
- platform/linux/native/original_ui_course_draw_resources50kg.cpp
- src/game/original_ui_course_widget_controller50kg.cpp
- src/game/original_ui_course_widgets50kg.cpp
- src/game/original_ui_course_page_draw50kg.cpp
- src/game/original_ui_rider_voice_choice50kg.cpp

New scene translation units (complements22-file scene snapshot):

- platform/linux/native/original_ui_course_draw_contribution50kg.cpp
- platform/linux/native/original_ui_course_hologram_draw50kg.cpp
- src/assets/original_ui_course_background50kg.cpp
- src/assets/original_ui_course_hologram_assets50kg.cpp
- src/game/original_ui_course_camera50kg.cpp
- src/game/original_ui_course_clip_camera50kg.cpp
- src/game/original_ui_course_hologram_scene50kg.cpp
- src/game/original_ui_course_lighting50kg.cpp
- src/game/original_ui_course_render_context50kg.cpp

Existing replacement CPP paths are already in the manifests; do not duplicate them. Header-only changes require no new compilation unit. If root’s validation pin manifests track the explicit lists/new headers, their exact source hashes also need central refresh; this inventory does not execute that workflow.

## Final delivery artifacts

COMPLETE-SOURCE-INVENTORY.json maps all 53 candidate source files to repository paths with hashes and new/replacement status. BUILD-MANIFEST-ADDITIONS.patch supplies the 16 new translation units for all three explicit source lists. It deliberately leaves Windows provenance/revision pins for root to refresh from the actual integrated script/revision. frontend/RUNTIME-COURSE-THREADING.patch contains only the two targeted runtime/header edits. Copy source files by the inventory, apply targeted patches centrally, and retain gates closed until root admits the route. No patch application or build validation was performed.

## Shared selector correction, 03:11 UTC

The original lazy80A8/seed3D6B30 and lazy80AC/range3D6B38 are shared application statics, not private voice state. The candidate now retains one SharedRandomOwner50ID in OriginalUiMainApplication50KG. The stateless voice selector borrows that owner with the same application clock and sharedNoise308030. Main startup raw seeding does not initialize either lazy projection. Source mutation prefixes, cached-zero reseeding, exactly one draw and the accepted bounded integer choice formula are unchanged. This uses main 9fba56eb's existing shared_random_owner50id.h and adds no translation unit. Independent source review accepted all four hash-pinned changes at 03:11 UTC: ownership only, with unchanged chronology, arithmetic and failure prefixes.

The live runtime retains its separate fresh-race sessionRandom owner unchanged. Menu-to-race remains disabled; these ownership domains are not falsely joined. Both Course admission gates remain false. COMPLETE-INTEGRATION.patch and source preimage manifest are refreshed against current main; no patch/build/test/syntax or gameplay execution occurred.
