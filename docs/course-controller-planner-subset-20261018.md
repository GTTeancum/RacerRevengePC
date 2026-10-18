# Course controller planner subset — October 18, 2026

This partial milestone adds pure moveRow and moveLaps planner APIs to the Course widget controller. It preserves existing callers and does not enable new menu input or change live race behavior by itself. Full duration-control integration remains pending.

The exact two production files were freshly compiled with six implementation translation units and the included checker against the published 45d24c922d44a9c9b88a3492c8bb1c86758a4b6d source dependencies. All 30 source dependencies were matched to that baseline by Git blob hash; compiler inputs and generated dependency closure were checked. Strict C++20/O2 floating-point flags were retained.

Parent execution passed compile, link, and 938 assertions: six row cases, 50 lap cases, 126 negative cases, and eight unchanged-course cases. Tests use authentic user-supplied original disc archives; no asset bytes are included here. Test binary SHA256 d9a230d3f9659601fe90e811be4da19711649a6241f73f386ea78b23fc587f35. The test's retained proposal-author note describes its author's earlier status; the parent subsequently compiled and ran these exact bytes successfully on October 18.

This validates planner ordering, 1–25 lap wrapping and failure guards, not full UI input wiring, profile commits, gameplay, race finish, or all racer/track coverage. Existing source warning labels are preserved.

From the repository root, reproduce with:

```sh
g++ -std=c++20 -O2 -fno-fast-math -frounding-math -ffp-contract=off -UNDEBUG -ffunction-sections -fdata-sections -pthread -Isrc/game -Isrc/assets src/game/original_ui_course_widget_controller50kg.cpp src/game/original_ui_navigation50kg.cpp src/assets/original_ui_pages50kg.cpp src/assets/original_ui_archive50kg.cpp src/assets/pod_physics49n.cpp src/assets/import_decode48c.cpp tests/native/original_course_controller_subset_check.cpp -Wl,--gc-sections -lz -o /tmp/course-controller-check
/tmp/course-controller-check /path/to/original/disc
```
