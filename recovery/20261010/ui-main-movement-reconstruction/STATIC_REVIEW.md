# Static review and frozen candidate

Reviewed against the original byte evidence and current public native headers, without compiling or executing candidate code:

* All sixteen mapping read order/offset pairs match 2AB010. WantsJoystickInput1/2 and OnTop use the corrected source boolean helper; mappings and GridSize use integer parsing.
* The Movement factory does not call the base common-flag reader. Those fields remain constructor defaults and are not silently admitted as authored behavior.
* Main admission is catalog-identity bounded and rejects unrecovered grid/direct-slot operations. Asset descriptors retain all mapping values so rejection does not erase evidence.
* Lifecycle operations match source calls. The separate UIWidget disabled field is not confused with UIControl setEnabled.
* Input selection is a post-page-callback snapshot, not visibility or a live flag re-read. Main's unconditional return1 is documented. Group lookup uses current controller ownership. Mapping0 and missing group perform no action; nonzero source notification is preserved.
* Navigation publication is atomic within the leaf. The mixed owner is explicitly responsible for preflighting external widget command effects before publishing its private navigation copy.
* Camera configuration uses the Main manager's conditional CameraInfo defaults and zero-FOV retention. Main PageInfo camera entries remain raw metadata only for the actual Main factory override.
* Camera aspect arithmetic matches source instruction order; strict rounding is checked and existing recovered camera49F handles numeric leaves. Output precedes platform-specific screen adjustment and does not claim live scene or entity execution.
* Vtable bindings were read directly from source words and are listed in evidence/manifest.json. Authenticated DAT byte counts and hashes match the existing catalog identities.

Candidate source is frozen for root integration. Root must register the four translation units in its build recipes and retain strict floating-point compilation policy for original_ui_main_camera50kg.cpp. No active repository files were edited by this worker.

Public source notes are in PUBLIC_SOURCE_NOTES.md. The README gives the source contract and smallest composition join. The evidence directory contains static byte listings and provenance for local review; it is not a distributable asset package.
