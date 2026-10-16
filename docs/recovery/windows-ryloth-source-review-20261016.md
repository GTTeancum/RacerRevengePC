# Windows parity source review, 2026-10-16

Source-only review after the passing Linux Mawhonic/Ryloth smoke on local
`acdf3001fb81670bd92e19bd1265310ad13216cd`. Published main checkpoint
`9d41f071741b99ca3152ac2ad6b609c233905ccf` has the same tested tree
`fc9ff499fc655d33ccf0e33d3e713cf75d736a59`.
No Windows compiler, launcher, syntax check, smoke, or PC transfer was run.
The current VM-only direction remains in force.

## Build closure

The Windows manifest contains 352 C++ translation units and six vendored zlib C
units. Its three generator provenance hashes still match the checked-in files.
The passing Linux CMake closure contains those same 352 C++ units plus:

- `platform/linux/native/offscreen_gl_context50dc.cpp`: intentionally excluded
  from Windows; this implementation explicitly requires Linux EGL and dlfcn.
- `src/assets/static_draw_assets50dw.cpp`: an additional Linux library unit;
  the interactive driver and platform renderer have no caller of that API.

The dedicated RA renderer is included through
`platform/linux/native/ra_static_world_gl_storage.inl` by the existing
`vehicle_window_gl50ds.cpp`. RA session, camera, sky, trigger and cold world
owners are header dependencies of the existing integrated driver. No new
Windows translation unit is needed solely because these owners were added.
This establishes source-list coverage, not successful MSVC compilation.

## Launch and asset boundary

The existing manual launcher clears inherited RR settings and selects the
ordinary TA/Main route. It cannot be used unchanged to claim the explicit
Mawhonic/Ryloth evidence route. That test profile intentionally requires all
four captures, baseline FPS and original AI through its complete unattended
route. Do not add only RR_TRACK=RA or only RR_ORIGINAL_TEST_PROFILE to the manual
launcher: incomplete profile admission is rejected by the application.

A future central Windows smoke must use the native PE exe and SDL2.dll beside
SLUS in the authentic original asset folder, with that folder as cwd. SLUS is
read-only source data and must never be executed. Retain original RA.RES,
Ryloth UI archives, authentic audio, camera files, original eight racer resources,
and the matching prepared rider cache. The generated runtime also needs the
48 recovered skyRA images and six original RA crowd images. Packaging must copy
those approved existing files with their identities; it must not substitute TA
geometry, fabricate images, or invoke conversion while launching.

Windows remains unverified for this Ryloth revision. The previous Windows
Gasgano/Sullust result and fresh Linux Mawhonic/Ryloth result are separate.

## Evidence metadata follow-up

The passing Linux receipt's existing `authored_world_fog` and
`track_frustum_culling` keys refer to the legacy track adapter. RA uses a dedicated
renderer with authored material fog and conservative clip rejection. New
additive receipt fields identify that distinction and its four-sample internal
coverage target; window MSAA is a separate request. These metadata changes
are source-only and await the next eligible centralized smoke.
