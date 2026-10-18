# Course lap-text API subset — October 18, 2026

This partial source milestone adds setLapCount for authentic embedded Course text at counts1–25, retaining original localization, font, material and layout. Existing callers are unchanged; full duration-control UI and race integration remain pending. Existing source warning labels are preserved.

Fresh isolated compile/link/test passed against main337ce6d7ca4619924ee76a1fe25b44037e527343 source dependencies: 62 hash-matched source files, only these two production files changed;16 implementation TUs plus the exact included checker. All generated non-system dependencies were verified; no older objects or synthetic successful owner stubs were used. Unreachable unused decoder code is discarded by normal function-section linking.

Result: TEXT_SUBSET_PASS counts=25 assertions=539. Binary SHA256 ecdb6de6b5ea06844960b9390e7d6f7dfda1be9d7049c68096555fe3a472e908. Cases use authentic original user-supplied disc archives and cover real singular/plural text, unchanged other packets, idempotence, invalid-count atomicity and restoration to3. No original assets are published. The checker's retained author note refers to its proposal preparation; the parent compiled and ran these exact bytes on October18.

Limits: CPU text generation only. No allocation-failure injection, actual renderer submission, session ownership, GUI input wiring, completed lap or finish is established by this isolated test.

Reproduce from repository root:

```sh
g++ -std=c++20 -O2 -fno-fast-math -frounding-math -ffp-contract=off -UNDEBUG -ffunction-sections -fdata-sections -pthread -Isrc/game -Isrc/assets src/assets/import_decode48c.cpp src/assets/import_texture48e.cpp src/assets/material_texture_assets50ea.cpp src/assets/native_asset48b.cpp src/assets/original_ui_archive50kg.cpp src/assets/original_ui_controls_specmap50kg.cpp src/assets/original_ui_course_background50kg.cpp src/assets/original_ui_font50kg.cpp src/assets/original_ui_ipum_texture_cache50kg.cpp src/assets/original_ui_localization50kg.cpp src/assets/original_ui_pages50kg.cpp src/assets/original_ui_screen_assets50kg.cpp src/assets/pod_physics49n.cpp src/game/original_ui_course_page_draw50kg.cpp src/game/original_ui_text50kg.cpp src/game/original_ui_text_position50kg.cpp tests/native/original_course_text_subset_check.cpp -Wl,--gc-sections -lz -o /tmp/course-text-check
/tmp/course-text-check /path/to/original/disc
```
