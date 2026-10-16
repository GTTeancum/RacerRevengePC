# Windows Clegg binding failure recovery

8a911b3 built successfully on Windows at 15:27:59.898915–15:30:42.270635 UTC. Its visible smoke failed at 15:31:17.370942–15:31:19.047628 UTC with exit1 while preparing Rider, before gameplay. Main only was captured; no track FPS or HUD completion was observed. Cleanup confirmed zero owned processes. Evidence: libfile_bbbfba6731f481919da832e465536b63, SHA256 f9969e5a33fc37e5bc9732626d9294152024228c82ed792709c7e30a96837b39.

Actual character widget37 resolves03c.mcf/03c.vut. MotionBindingError49O::line (4) was emitted at153218. The original authenticated03c.vut has22 lines with trailing ASCII spaces, including its final no-newline row. Its SHA256 is811fdc48ff923656ee769fc2ce11dcf622fca8b4059f2c476059e707d27122f9. The first affected RHS begins at byte28. The original15326C/153374 strtok delimiter strings at3A5F60/3A5F68 include ASCII space, so those terminal spaces are delimiters rather than name characters.

The central correction removes trailing ASCII spaces after checking the original physical-line bound and removing CR. Identifier, hierarchy, unknown-node and all geometry checks remain unchanged. Other whitespace is still outside this bounded admission. Diagnostic output now includes the binding byte offset. No source asset is rewritten, no requested racer is dropped, and profile/unlock/AI behavior remains unchanged.

Source review only. No Linux test, parser execution, syntax check or guest execution. Windows recovery build and real-window smoke must both pass before feature work resumes. The immediate rerun is authorized by the user's failure recovery exception.

## Follow-on original missing-node behavior

The e4a0373 recovery build passed, but its smoke still stopped before Rider with unknownNode5 at VUT line offset17, the original Main_Dummy row. No gameplay/FPS was reached. The next correction opts only the authenticated Rider character loader into the existing sourceDiagnosticSkip policy. Original153238 returnsFF for missing LHS;153248 falls through to diagnostic153254, then15325C branches to1533F0 without rename or +38/+3C stores. The existing native policy preserves every real PSG row/name/reference/flag and records skipped lines; it never synthesizes a node. Global default rejection and all hierarchy/identifier/geometry guards remain unchanged. Evidence for this failed recovery: libfile_518b13d355148191932d36b5cbb1eb91, SHA256149e2e044a0214e678b9d3840291221e41c9b37d5ccb6184f8afda830dbb8135.

## Kraid original palette-zero authored-color variant

1fabf8d Windows build passed at15:51:59.836778–15:54:37.445124 UTC. Smoke failed at15:54:49.186498–15:54:50.897199 UTC, now character widget57 with scene.attribute_variant at byte25300, before Rider/gameplay. Cleanup confirmed zero owned processes. Evidence libfile_94d05206c0a88191a293cb4594f40f4b, SHA25617e082fbd3e9610f44419ccfa7f6e056ec00da2bf03144e3fb2f0736c05610bc.

Authenticated original23c.psg contains exact component word0000010f: position/normal/UV plus stored colors using palette zero. The decoder already supports palette-zero uncolored00000107 and no-palette colored00ff010f, but omitted this exact combination. This correction admits only0000010f and sets actual color presence so existing6e stored-color decoding runs. It retains all packet, palette footer, bone association, strip/index, extent and numeric checks. No surface/attribute is synthesized or dropped. Source inspection found145 groups and1850 vertices for the colored surface; RGBA values include(0,0,0,63) and(127,127,127,63). Original24c has three existing00000107 surfaces, as do the other requested03/07/09 model headers.

No additional software checks ran. Immediate Windows recovery validation remains required before any feature work resumes.

## Finite original material identities for requested previews

fbb4c7d Windows build passed at16:02:21.743620–16:04:57.419643 UTC. Smoke failed at16:05:09.589018–16:05:11.398102 UTC during Rider draw resource preparation with canonical unlocked Rider single-map specular pass2. Cleanup confirmed zero owned processes; no gameplay or FPS. Evidence libfile_dffaea80e87c8191b9e434ad58b5348c, SHA2564dce34cd31cba46111d4b46d11230295848c85724d783b8b2b316fce2f3ce2b4.

Original09/23/24 body and shadow PSMs match already admitted04 profiles after replacing their own texture name and trimming trailing whitespace.09c_1 matches04c_1's opaque IPUM profile. Add only these finite authenticated identities to existing adapters, and route their shadows through the existing exact attenuation adapter. Every semantic and source/image/provenance gate remains unchanged. Character09c_2 and23c_1/2,24c_1/2 explicitly declare one pass; trailing ShaderPass_2 text remains inactive. No profile unlock, skipped geometry, new blend formula, fabricated material or extra pass is introduced. Terminal diagnostics also expose existing draw material index/pass/widget for future recovery evidence.

Source-only comparison and central integration; no independent software check. Immediate Windows build plus real-window smoke remains the acceptance gate.
