# Native original-Main movement and camera support

The Main input leaf, camera configuration and camera preparation are reconstructed as native C++ code from the original executable's static call sites. The implementation does not execute the original instruction set. No original asset payload or executable excerpt is included in this note.

## Movement_Control

The original constructor and reader at 2AAF40/2AB010 establish a selected-state input leaf with sixteen integer mappings. Its handler at 2AB390 resolves the current page's controller-attached group and performs navigation. Main's bounded native owner supports its authored zero/previous/next mapping domain and rejects other operations explicitly.

Input delivery follows container 2A7290: page input gate, page callback, a snapshot of registered widgets' selected flags, then ordered leaf callbacks. Main callback 1E5E00 returns acceptance even when its own action makes no change. The native composition must not infer leaf rejection from that no-change result.

The callback and visibility operations follow the actual Movement vtable. Its inherited update/render entries only traverse children; its factory creates none. It has no own geometry or camera operations. A nonzero mapping also invokes the Main manager notification whose vtable resolves to the proven-empty function 2A4330. This does not stand in for an unimplemented manager action.

## Main camera

Main page camera factory 1E5CF0 borrows its manager's existing camera. The manager's configuration reader is 1CF4F0; Main does not call the generic page-camera reader for its own PageInfo camera entries. Native admission retains those original entries as metadata only in the verified Main factory, without relaxing unknown-field handling for other page classes.

Native camera preparation follows constructor and viewport setup 1143F0/2361C0, the actual pose-setter semantics at 114F90, and the already recovered camera-view numeric implementation. It retains original binary32 evaluation order and original viewport dimensions. The resulting core matrices are intended for the scene rendering adapter; 2D overlay coordinates remain in their own rendering path.

The original manager registers this camera with its scene, appends it to the entity owner and activates the borrowed camera on Main entry. The staged preparation API is intentionally a cold numeric boundary. Live scene attachment, visibility, camera-update gates and render ordering remain explicit responsibilities of the native composition owner.

## Integration status

Eight source files, comprising four translation units and matching headers, are staged for root integration. Source-level review checked call-site bindings, callback semantics, input gating, per-key defaults, raw-metadata lifetime and the native camera math boundary. No build, syntax checker, test, importer, project parser or gameplay execution was performed during this reconstruction. Runtime behavior remains unverified until the authorized central validation.
